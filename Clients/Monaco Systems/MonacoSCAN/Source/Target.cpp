// Target.cpp : implementation file
//

#include "stdafx.h"
#include "monacoscan.h"
#include "Target.h"
#include "Dibapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTarget

/////////////////////////////////////////////////////////////////////////////
CTarget::CTarget() : _scancal(4)
{
	//{{AFX_DATA_INIT(CTarget)
	memset(m_fTargetData, 0, sizeof(m_fTargetData));
	memset(m_fPatchData, 0, sizeof(m_fPatchData));
	m_nTargetPatches = 0;
	InitTable(m_fTable);
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
CTarget::~CTarget()
{
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTarget, CWnd)
	//{{AFX_MSG_MAP(CTarget)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CTarget::ReadTargetDataFile(CString strFileName)
{
	if (strFileName.IsEmpty())
		return FALSE;

	FILE* fs = fopen( strFileName, "r" );
	if (!fs)
		return FALSE;
		
	char buffer[144]; // Buffer to read each line

	// Find the manufacturer's name
	int type = 0;
	while (fgets(buffer, sizeof(buffer)-1, fs))
	{
		if (strstr(buffer, "Agfa"))
		{
			type = 1;
			break; // found
		}	
		else
		if (strstr(buffer, "Fuji"))
		{
			type = 2;
			break; // found
		}	
		else
		if (strstr(buffer, "Kodak"))
		{
			type = 2;
			break; // found
		}	
	}			 		
	
	// See if we found a recognizable type
	if (!type || feof(fs))
	{
		fclose(fs);		
		return FALSE;
	}

	// Find the start of the data
	int nLinesOfData = 0;
	while (fgets(buffer, sizeof(buffer)-1, fs))
	{
		if ( type == 1 /*AGFA*/ && strstr(buffer, "A1"))
		{
			nLinesOfData = 288;
			break; // found
		}
		else
		if ( type == 2 /*KODAK and FUJI*/ && strstr(buffer, "A01"))
		{
			nLinesOfData = 264;
			break; // found
		}
	}
	
	// See if we found the start of the data
	if (!nLinesOfData || feof(fs))
	{
		fclose(fs);		
		return FALSE;
	}

	// Initialize the target data array
	memset(m_fTargetData, 0, sizeof(m_fTargetData));
	double* pfTargetData = m_fTargetData;

	// The data format is as follows:
	// SAMPLE_ID   X   Y   Z   L   A   B   C   H
	// We want the Lab values
	for (int i = 0; i < nLinesOfData; i++)
	{
		// Get a new line of data
		if (i != 0 && !fgets(buffer, sizeof(buffer)-1, fs))
			break;

		double dL = 0;
		double dA = 0;
		double dB = 0;
		char* p = buffer;

		// Skip past the label (e.g., A1)
		p = strstr(p, " ");
		if (!p)
		{
			fclose(fs);		
			return FALSE;
		}

		// Loop over the 8 values on the line, and collect the ones we want
		for (int j = 0; j < 8; j++)
		{
			// Skip any white space
			while (*p == ' ')
				p++;

			// Collect the digits in the number
			char szValue[16];
			szValue[0] = '\0';
			int k = 0;
			while (*p != ' ' && *p != '\n' && *p != '\0')
			{
				// Add the character
				szValue[k++] = *p++;
				// Add the null termination
				szValue[k] = '\0';
			}

			// Make sure we got something
			if (!k)
			{
				fclose(fs);		
				return FALSE;
			}

			switch (j)
			{ // Ignore the XYZ values, and the CH values
				case 3:
					dL = strtod(szValue, NULL);
					break;
				case 4: 
					dA = strtod(szValue, NULL);
					break;
				case 5:
					dB = strtod(szValue, NULL);
					break;
			}
		}
		
		*pfTargetData++ = dL;
		*pfTargetData++ = dA;
		*pfTargetData++ = dB;
	}

	fclose(fs);		

	// Make sure we got all of the data
	if (i != nLinesOfData)
		return FALSE;

	// Set the number of patches we will need to read
	m_nTargetPatches = nLinesOfData;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTarget::ReadPatchData(HGLOBAL hDib, CPoint ptUL, CPoint ptUR, CPoint ptLL, CPoint ptLR)
{
	if (m_nTargetPatches <= 0)
		return FALSE;

	CRect rPatches[300];
	memset(rPatches, 0, sizeof(rPatches));
	if (!GeneratePatchLocations( rPatches, ptUL, ptUR, ptLL, ptLR ))
		return FALSE;

	PBITMAPINFO pDib = (PBITMAPINFO)GlobalLock(hDib);
	if (!pDib)
		return FALSE;

	// Initialize the patch data array
	memset(m_fPatchData, 0, sizeof(m_fPatchData));
	double* pfPatchData = m_fPatchData;

	for (int i = 0; i < m_nTargetPatches; i++)
		ReadPatch(pDib, rPatches[i], &pfPatchData[i*3]);

	GlobalUnlock(hDib);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTarget::GeneratePatchLocations( CRect* prPatches, CPoint ptUL, CPoint ptUR, CPoint ptLL, CPoint ptLR )
{
	if (m_nTargetPatches <= 0)
		return FALSE;

	if (!prPatches)
		return FALSE;

	int iWidth  = min( (ptUR.x - ptUL.x), (ptLR.x - ptLL.x) );
	int iHeight = min( (ptLL.y - ptUL.y), (ptLR.y - ptUR.y) );

	double fPatchSizeY = (double)iHeight / 16.0;
	double fPatchSizeX = (double)iWidth  / 24.0;
	double fOriginX = (double)ptUL.x + fPatchSizeY/3 + .5;
	double fOriginY = (double)ptUL.y + fPatchSizeY/3 + .5;
	
	double left, top;
	int i, j;
	int c = 0;

	// Compute the locations of each patch
	for (i = 0; i < 12; i++)
	{
		// The first 8 rows might be short if Kodak or Fuji (264 patches)
		int nPatchesWide = ((i < 8 && m_nTargetPatches == 264) ? 19 : 22);

		// Set the locations of a row of color patches
		top = fOriginY + ((double)(i+1) * fPatchSizeY);
 		for(j = 0; j < nPatchesWide; j++)
		{
			left = fOriginX + ((double)(j+1) * fPatchSizeX);
			prPatches[c].top    = (int)(top);
			prPatches[c].bottom = (int)(top  + fPatchSizeY/3);
			prPatches[c].left   = (int)(left);
			prPatches[c].right  = (int)(left + fPatchSizeX/3);
			c++;
		}
	}	

	// Set the locations of the gray strip
	top = fOriginY + (14.0 * fPatchSizeY);
	for(j = 0; j < 24; j++)
	{
		left = fOriginX + ((double)j * fPatchSizeX);
		prPatches[c].top    = (int)(top);
		prPatches[c].bottom = (int)(top  + fPatchSizeY/3);
		prPatches[c].left   = (int)(left);
		prPatches[c].right  = (int)(left + fPatchSizeX/3);
		c++;
	}

	return TRUE;
}
	
/////////////////////////////////////////////////////////////////////////////
BOOL CTarget::ReadPatch(PBITMAPINFO pDib, CRect rRect, double* pfPatchData)
{
	LPBYTE pData = (LPBYTE)FindDIBBits((LPSTR)pDib);

	if (!pData)
		return FALSE;

	int iWidthBytes = WIDTHBYTES(pDib);
	int iHeight = (int)DIBHeight((LPSTR)pDib);
	if (iWidthBytes <= 0 || iHeight <= 0)
		return FALSE;

	double r = 0;
	double g = 0;
	double b = 0;
	int nSamples = 0;

	for (int y = rRect.top; y < rRect.bottom; y++)
	{
		int yFlipped = (iHeight-1) - y;
		LPBYTE p = pData + ((yFlipped*iWidthBytes) + (rRect.left*3));
		for (int x = rRect.left; x < rRect.right; x++)
		{
			b += *p++;
			g += *p++;
			r += *p++;
			nSamples++;
		}
	}

	if (nSamples)
	{
		*pfPatchData++ = r / nSamples;
		*pfPatchData++ = g / nSamples;
		*pfPatchData++ = b / nSamples;
	}
	else
	{
		*pfPatchData++ = 0;
		*pfPatchData++ = 0;
		*pfPatchData++ = 0;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CTarget::InitTable(double* pfTable)
{
	int m = 0;
	for (int i=0; i<33; i++)
		for (int j=0; j<33; j++)
			for (int k=0; k<33; k++)
			{
				pfTable[m++] = (double)i*7.96875;
				pfTable[m++] = (double)j*7.96875;
				pfTable[m++] = (double)k*7.96875;
			} 

}

/////////////////////////////////////////////////////////////////////////////
BOOL CTarget::Calibrate( double* pfAvgDeltaE, double* pfMaxDeltaE )
{
	McoStatus status = _scancal.calibrate(m_nTargetPatches, m_fPatchData, m_fTargetData);
	if (status != MCO_SUCCESS)
		return status;
	
	InitTable(m_fTable);

	status = _scancal.compute(TABLE_ENTRIES, m_fTable, m_fTable);
	if (status != MCO_SUCCESS)
		return status;
	
	if (pfAvgDeltaE && pfMaxDeltaE)
	{
		double fPatchData[300*3];
		memcpy( fPatchData, m_fPatchData, sizeof(fPatchData));
		int32 which;
		status = _scancal.deltaE(m_nTargetPatches, fPatchData, m_fTargetData, pfAvgDeltaE, pfMaxDeltaE, &which);
		if (status != MCO_SUCCESS)
			return status;
	}

	return TRUE;
}
	
#include "mcomem.h"
#include "csprofile.h"

/////////////////////////////////////////////////////////////////////////////
BOOL CTarget::SaveProfile(CString szProfileName)
{
	McoStatus status = MCO_SUCCESS;

	char szFileName[255];
	strcpy( szFileName, szProfileName );
	CsFormat csformat;
	status = csformat.createFile(szFileName, O_CREAT | _O_TRUNC | _O_BINARY | O_WRONLY /*| _O_RDWR*/);
	if (status != MCO_SUCCESS)
		return FALSE;

	// Set header
	CM2Header header;
	header.CMMType = MONACO_CMM;
	header.profileVersion = (MCOP_MAJOR_VERSION + MCOP_MINOR_VERSION);
	header.profileClass = cmInputClass;
	header.dataColorSpace = cmRGBData;
	header.profileConnectionSpace = cmLabData;
	header.CS2profileSignature = cmMagicNumber;	//?
	header.platform = MCOP_DEFAULT_PLATFORM;
	header.flags = MCOP_DEFAULT_FLAGS;
	header.deviceManufacturer = MCOP_DEFAULT_MANUFACTURER;
	header.deviceModel = MCOP_DEFAULT_MODEL;
	header.deviceAttributes[0] = MCOP_DEFAULT_ATTRIBUTES;
	header.deviceAttributes[1] = MCOP_DEFAULT_ATTRIBUTES;
	header.renderingIntent = MCOP_DEFAULT_RENDERINGINTENT;
	header.white.X = MCOP_DEFAULT_WHITE_X;
	header.white.Y = MCOP_DEFAULT_WHITE_Y;
	header.white.Z = MCOP_DEFAULT_WHITE_Z;

	status = csformat.setHeader(&header, cmSigLut8Type);
	if (status != MCO_SUCCESS)
		return FALSE;

	double rgbtoxyz[9];
	for(int i = 0; i < 9; i++)
		rgbtoxyz[i] = 1.0;
		
	CMXYZType xyz;
	xyz.typeDescriptor = cmSigXYZType;
	xyz.reserved = 0x00000000;
	xyz.XYZ[0].X = (unsigned long)(rgbtoxyz[0]*65536.0+0.5);
	xyz.XYZ[0].Y = (unsigned long)(rgbtoxyz[3]*65536.0+0.5);
	xyz.XYZ[0].Z = (unsigned long)(rgbtoxyz[6]*65536.0+0.5);
	status = csformat.setXYZtypedata(cmRedColorantTag, &xyz);
	if (status != MCO_SUCCESS)
		return FALSE;

	xyz.XYZ[0].X = (unsigned long)(rgbtoxyz[1]*65536.0+0.5);
	xyz.XYZ[0].Y = (unsigned long)(rgbtoxyz[4]*65536.0+0.5);
	xyz.XYZ[0].Z = (unsigned long)(rgbtoxyz[7]*65536.0+0.5);
	status = csformat.setXYZtypedata(cmGreenColorantTag, &xyz);
	if (status != MCO_SUCCESS)
		return FALSE;

	xyz.XYZ[0].X = (unsigned long)(rgbtoxyz[2]*65536.0+0.5);
	xyz.XYZ[0].Y = (unsigned long)(rgbtoxyz[5]*65536.0+0.5);
	xyz.XYZ[0].Z = (unsigned long)(rgbtoxyz[8]*65536.0+0.5);
	status = csformat.setXYZtypedata(cmBlueColorantTag, &xyz);
	if (status != MCO_SUCCESS)
		return FALSE;

	// Set the parameters
	double wX = 0.8249;
	double wY = 1.0;
	double wZ = 0.9659;

	xyz.XYZ[0].X = (unsigned long)(wX*65536.0+0.5);
	xyz.XYZ[0].Y = (unsigned long)(wY*65536.0+0.5);
	xyz.XYZ[0].Z = (unsigned long)(wZ*65536.0+0.5);
	status = csformat.setXYZtypedata(cmMediaWhitePointTag, &xyz);
	if (status != MCO_SUCCESS)
		return FALSE;

	short rcurve_size = 256;
	short gcurve_size = 256;
	short bcurve_size = 256;
	unsigned short rcurve[256], gcurve[256], bcurve[256];

	CMCurveType rh;
	rh.typeDescriptor = cmSigCurveType;
	rh.reserved = 0x00000000;
	rh.countValue = rcurve_size;
	status = csformat.setCurvetypedata(cmRedTRCTag, &rh, rcurve);
	if (status != MCO_SUCCESS)
		return FALSE;

	rh.countValue = gcurve_size;
	status = csformat.setCurvetypedata(cmGreenTRCTag, &rh, gcurve);
	if (status != MCO_SUCCESS)
		return FALSE;

	rh.countValue = bcurve_size;
	status = csformat.setCurvetypedata(cmBlueTRCTag, &rh, bcurve);
	if (status != MCO_SUCCESS)
		return FALSE;

//copyright tag
	if( (status = csformat.setCprttypedata(MCOP_CPRT, strlen(MCOP_CPRT))) != MCO_SUCCESS )
		return status;

//description tag
	//char	filename[256];
	//if( (status = csformat.getFname(filename)) != MCO_SUCCESS)
	//	return status;
		
	status = csformat.setDesctypedata(szFileName, strlen(szFileName)+1);
	if (status != MCO_SUCCESS)
		return FALSE;

	// Some init	
	Ctype A2B0;
	A2B0.input_channels  = 3;
	A2B0.output_channels = 3;
	A2B0.grid_points	 = 33;
	A2B0.identity_matrix = 1;	//is it identity matrix 1 == yes
	A2B0.input_entries = 256;
	A2B0.output_entries = 256;

	char linear[256];
	for(i = 0; i < 256; i++)
		linear[i] = i;

	for(i = 0; i < A2B0.input_channels; i++)
		A2B0.input_tables[i] = linear;	
		
	for(i = 0; i < A2B0.output_channels; i++)
		A2B0.output_tables[i] = linear;
	
	long table_entry = 33*33*33;
	McoHandle tableH = McoNewHandle(sizeof(char)*table_entry*3);
	if (!tableH)
		return FALSE; //MCO_MEM_ALLOC_ERROR;
	
	unsigned char* ctable = (unsigned char*)McoLockHandle(tableH);
	double* dtable = m_fTable;
	
	for(i = 0; i < table_entry; i++)
	{
		long temp = (long)(dtable[0]*2.55+0.5);
		if (temp < 0)
			ctable[0] = 0;
		else
		if (temp > 255)
			ctable[0] = 255;
		else	
			ctable[0] = (unsigned char)temp;
			
		temp = (long)(dtable[1]+0.5);
		if (temp < -128)
			ctable[1] = 0;
		else
		if (temp > 127)
			ctable[1] = 255;
		else	
			ctable[1] = (unsigned char)(temp + 128);

		temp = (long)(dtable[2]+0.5);
		if (temp < -128)
			ctable[2] = 0;
		else
		if (temp > 127)
			ctable[2] = 255;
		else	
			ctable[2] = (unsigned char)(temp + 128);
					
		ctable += 3;
		dtable += 3;		
	}
	
	McoUnlockHandle(tableH);
	
	status = csformat.setLuttypedata(1, &A2B0, tableH);
	if (status != MCO_SUCCESS)
	{
		McoDeleteHandle(tableH);
		csformat.closeFile();
		return FALSE;
	}	

	status = csformat.setLuttypedata(4, &A2B0, tableH);
	if (status != MCO_SUCCESS)
	{
		McoDeleteHandle(tableH);
		csformat.closeFile();
		return FALSE;
	}	

	status = csformat.setLuttypedata(6, &A2B0, tableH);
	if (status != MCO_SUCCESS)
	{
		McoDeleteHandle(tableH);
		csformat.closeFile();
		return FALSE;
	}	
		
	status = csformat.writeTag(1);
	McoDeleteHandle(tableH);
	csformat.closeFile();

	if (status != MCO_SUCCESS)
		return FALSE;

	return TRUE;
}
