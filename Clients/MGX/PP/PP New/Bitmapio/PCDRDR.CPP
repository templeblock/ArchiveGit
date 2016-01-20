 //®PL1¯®FD1¯®BT0¯®TP0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "stdafx.h"
#include "bitmapio.h"
#define KPWIN32
#include "pcdlib.h"
#include <ctype.h>
#include "cmsutil.h"
#include "imgutil.h"

// Static prototypes
LOCAL HANDLE LoadPCDLut(LPTSTR lpLutName, LPINT lpEntries);
LOCAL LPTSTR FindNumber(LPTSTR lp, LPTSTR lpEnd, LPBYTE lpValue);
LOCAL void GetPCDResolution(int PhotoCDRes, PCDresolution *pcdRes, LPINT lpRes);
LOCAL BOOL IsPCDImage( LPTSTR lpFileName );
LOCAL BOOL GetPCDOverview( LPTSTR lpFileName, LPTSTR lpOverview );
LOCAL void Convert24to24( LPRGB lpPixel, int iCount, LPTR lpOutput);

VOID EXPORT PhotoCDProgress(short nNum, short nDenom, long lData);
PCDboolean EXPORT PhotoCDAbort(long lData);

typedef struct _pcdprocinfo
{
	int nLines;
	int	nCurrentLine;
	int nLinesRead;
	BOOL fAbort;
	CReadBitmap *pReader;
} PCDPROCINFO;

/* 
 * Photo CD related paths
 * These paths are hard-coded because they
 * involve the file system structure that is
 * defined by Photo CD technology.
 */
#define IMAGE_PAC_PREFIX _T("IMG")
#define OVERVIEW_FILE	 _T("OVERVIEW.PCD")
#define OVERVIEW_PATH    _T("\\PHOTO_CD\\OVERVIEW.PCD") 
#define IMAGES_DIR       _T("\\PHOTO_CD\\IMAGES\\")    
#define DISC_RIGHTS_DIR	 _T("RIGHTS\\")
#define DISC_RIGHTS_FILE _T("RIGHTS.USE")    
#define PCD_IMAGEPACK 	 _T(".pcd")

#define MAIN_MEM_MINK	100L

/************************************************************************/
BOOL CReadBitmap::PCDRead()
/************************************************************************/
{
PCDphotoHdl	hPcd;	// handle to selected image pac
PCDtransform Transform;
PCDresolution pcdRes;
PCDformat pcdFormat;
PCDbuttonRec pcdButtons;
PCDrect rImage;
LPTR lpOut, lpConvertLine;
HPTR lpIn, lpBuffer, lpLine, lpCMYKBuffer;
long lStride, dxBytes;
long nLines, nLinesLeft;
LPFRAME lpFrame;
int npix, nlin, y, xres;
PCDpacInfoRec info;
long lW, lH, lMaxMem, lMemAlloc;
int iAllocTries;
short stat;
LPPCDOPTIONS lpOptions;
int PhotoCDRes;
FRMTYPEINFO inType, outType;
CFrameTypeConvert TypeConvert;
BOOL fConvert;
int indepth;
HANDLE hLut;
int nEntries;
ITEMID id;
short wResCount;
DWORD dwRes, dwMyRes;
BOOL fRet = FALSE, fResSupported;
CMSINFO CmsInfo;
BOOL fUsingCMS;
PCDPROCINFO procInfo;

ASSERT(m_lpOptions != NULL);
ASSERT(m_pFile == NULL);

if (!m_lpOptions)
	return(FALSE);
if (m_pFile != NULL)
	return(FALSE);

lpOptions = (LPPCDOPTIONS)m_lpOptions;

ProgressBegin(1);

lpFrame = NULL;
lpBuffer = NULL;
lpConvertLine = NULL;
hPcd = NULL;
outType = m_cmsInfo.dst;

// Call PCDopen to obtain an image pac handle
if(PCDopen(m_lpFileName, &hPcd))
	{
	SetError(BEC_errOpen);
	ProgressEnd();
	return(FALSE);
	}

if (PCDgetCount(hPcd, &dwRes, &wResCount))
	{
	PCDclose(hPcd);
	SetError(BEC_errFileRead);
	ProgressEnd();
	return(FALSE);
	}

// Read image information which is documented nowhere!!!
if (PCDreadImageInfo(hPcd, &info))
	{
	PCDclose(hPcd);
	SetError(BEC_errFileRead);
	ProgressEnd();
	return(FALSE);
	}

//if (info.mediaId == PCD_BW_NEGATIVE ||
//    info.mediaId == PCD_BW_REVERSAL ||
//    info.mediaId == PCD_BW_HARDCOPY)
CmsInfo = m_cmsInfo;

if (lpOptions->PhotoCDFormat == PCD_SINGLE)
	FrameSetTypeInfo(&inType, FDT_GRAYSCALE);
else
	FrameSetTypeInfo(&inType, FDT_RGBCOLOR, NULL, &CmsInfo.src.ptInfo);
if (outType.DataType == FDT_NONE)
	outType = inType;
else
	{
	if (outType.DataType == FDT_GRAYSCALE || outType.DataType == FDT_LINEART)
		FrameSetTypeInfo(&inType, FDT_GRAYSCALE);
	else
		FrameSetTypeInfo(&inType, FDT_RGBCOLOR, NULL, &CmsInfo.src.ptInfo);
	}
if (inType.DataType == FDT_GRAYSCALE)
	{
	indepth = 1;
	pcdFormat = PCD_SINGLE;
	}
else
	{
	indepth = 3;
	pcdFormat = ( m_CMSEnabled && IsDstPTSelected( &outType.ptInfo ) &&
		IsSrcPTSelected( &inType.ptInfo ) ) ? PCD_YCC : PCD_RGB;
	}

// use override to the resolution and then bound it
// and then check if it is supported
PhotoCDRes = lpOptions->PhotoCDRes + m_PhotoCDResOverride;

if (PhotoCDRes < PCD_BASE_OVER_16)
	PhotoCDRes = PCD_BASE_OVER_16;
if (PhotoCDRes > PCD_64BASE)
	PhotoCDRes = PCD_64BASE;

for (id = PCD_64BASE; id >= PCD_BASE_OVER_16; --id)
	{
	dwMyRes = 1 << (id-PCD_BASE_OVER_16+1);
	fResSupported = (dwMyRes & dwRes) != 0;
	if (PhotoCDRes == id && !fResSupported)
		--PhotoCDRes;
	}
// make sure new resolution is setup
lpOptions->PhotoCDRes = PhotoCDRes;

GetPCDResolution(PhotoCDRes, &pcdRes, &xres);

// Call PCDgetRotation() to ascertain the affine transformation
// necessary to specify before retrieving the image in order
// to display the image rightside-up.
//
// In preparation for PCDloadImage(), specify the transformation
// to be used in retrieving data from the image pac.

// try to set the color buttons
pcdButtons.BrightButton = lpOptions->BrightButton;
pcdButtons.RedButton = lpOptions->RedButton;
pcdButtons.GreenButton = lpOptions->GreenButton;
pcdButtons.BlueButton = lpOptions->BlueButton;
pcdButtons.SatButton = lpOptions->SatButton;
pcdButtons.OutOfGamut = lpOptions->OutOfGamut;
pcdButtons.SubtractSBA = lpOptions->SubtractSBA;
PCDsetColorButtons(hPcd, &pcdButtons);

hLut = LoadPCDLut(lpOptions->LutFileName, &nEntries);
if (hLut)
	{
	PCDsetLut(hPcd, hLut, nEntries);
	GlobalFree(hLut);
	}

if(PCDgetRotation(hPcd, &Transform))
	goto BadRead;
if (PCDsetTransform(hPcd, Transform))
	goto BadRead;
if (PCDsetFormat(hPcd, pcdFormat))
	goto BadRead;
if (PCDsetPlaneColumn(hPcd, 1L, (short)indepth))
	goto BadRead;
if (PCDsetResolution(hPcd, pcdRes))
	goto BadRead;
if (PCDgetSize(hPcd, &rImage))
	goto BadRead;
npix = rImage.right;
nlin = rImage.bottom;

if ( lpOptions->fHaveArea )
	{
	rImage.left = FMUL(xres, lpOptions->rArea.left);
	rImage.top = FMUL(xres, lpOptions->rArea.top);
	rImage.right = FMUL(xres, lpOptions->rArea.right);
	rImage.bottom = FMUL(xres, lpOptions->rArea.bottom);

	rImage.left = bound( rImage.left, 0, npix-1 );
	rImage.right = bound( rImage.right, 0, npix-1 );
	rImage.top = bound( rImage.top, 0, nlin-1 );
	rImage.bottom = bound( rImage.bottom, 0, nlin-1 );

	npix = rImage.right - rImage.left + 1;
	nlin = rImage.bottom - rImage.top + 1;
	}
else
	{
	rImage.left = rImage.top = 0;
	rImage.right = npix - 1;
	rImage.bottom = nlin - 1;
	}

procInfo.nLines = nlin;
procInfo.nCurrentLine = 0;
procInfo.nLinesRead = 0;
procInfo.fAbort = FALSE;
procInfo.pReader = this;
if (PCDsetProgress(hPcd, PhotoCDProgress, (long)&procInfo))
	goto BadRead;
if (PCDsetAbort(hPcd, PhotoCDAbort, (long)&procInfo))
	goto BadRead;

fConvert = !FrameTypeInfoEqual( inType, outType);

fUsingCMS = m_CMSEnabled && 
			IsSrcPTSelected(&inType.ptInfo) &&
			IsDstPTSelected(&outType.ptInfo);

if (fConvert)
	{
	if (!TypeConvert.Init(inType, outType, npix, m_DitherType))
		{
		SetError(BEC_errMemory);
		goto BadPCD;	
		}
	lpConvertLine = Alloc((long)npix*indepth);
	if (!lpConvertLine)
		{
		SetError(BEC_errMemory);
		goto BadPCD;	
		}
	}

if (! (lpFrame = FrameOpen(outType, npix, nlin, xres)))
	{
	SetError(BEC_errFrameOpen);
	goto BadPCD;
	}
lStride = (long)npix * (long)indepth;

#ifdef UNUSED
/*******************************************************/
// Old way of memory allocation PP 3.1
nLines = 65536L / lStride;
if (!(lpBuffer = Alloc(lStride*nLines)))
	{
	nLines = 7;
	if (!(lpBuffer = Alloc(lStride*nLines)))
		{
		SetError(BEC_errMemory);
		goto BadRead;
		}
	}
/*******************************************************/
#endif

// try a little more than a MB to better optimize for medium resolution
// tries 512x768x3, 256x384x3, 128x192x3, and then 64K
// these size correspond to the PhotoCD resolutions
// leave at least a 100K around
lMaxMem = (long)AvailableMemory() - (MAIN_MEM_MINK*1024L);
lW = 512L; // start with PCD_BASE resolution
lH = 768L;
iAllocTries = 4;
while (--iAllocTries >= 0)
	{
	if (iAllocTries)
		nLines = (lW * lH * 3L ) / lStride;
	else // last time try 64K buffer
		{
		dxBytes = lStride;
		nLines = 65536L / lStride;
		}
	if (nLines > nlin)
		nLines = nlin;
	lMemAlloc = lStride * (long)nLines;
	if (lMemAlloc <= lMaxMem)
		if (lpBuffer = (HPTR)Alloc(lMemAlloc))
			{
			dxBytes = lStride;
			break;
			}
	lW /= 2;
	lH /= 2;
	}

if( fUsingCMS && outType.DataType == FDT_CMYKCOLOR )
	{	
	dxBytes = (long)npix * (long)4;
	iAllocTries = 4;
	while (--iAllocTries >= 0)
		{
		if (iAllocTries)
			nLines = (lW * lH * 4L ) / dxBytes;
		else // last time try 64K buffer
			nLines = 65536L / dxBytes;
		if (nLines > nlin)
			nLines = nlin;
		lMemAlloc = dxBytes * (long)nLines;
		if (lMemAlloc <= lMaxMem)
			if (lpCMYKBuffer = (HPTR)Alloc(lMemAlloc))
				break;
		lW /= 2;
		lH /= 2;
		}
	}
else
	lpCMYKBuffer = lpBuffer;

if (!lpBuffer || !lpCMYKBuffer)
	goto NoMemory;
//Print("npix = %d nlin = %d lStride = %ld nLines = %d buffer size = %ld maxmem //= %ld memory left = %ld", npix, nlin, lStride, nLines, lMemAlloc, lMaxMem, //AvailableMemory());
nLinesLeft = nlin;
++rImage.right; // PhotoCD toolkit uses Windows style rectangles
rImage.bottom = rImage.top;
y = 0;
procInfo.fAbort = FALSE; // for progress and abort procs
while (nLinesLeft > 0)
	{
	procInfo.nCurrentLine = y; // for PhotoCDProgress proc
	procInfo.nLinesRead = nLines;
	if (procInfo.nLinesRead > nLinesLeft)
		procInfo.nLinesRead = nLinesLeft;
	nLinesLeft -= procInfo.nLinesRead;
	rImage.top = rImage.bottom;
	rImage.bottom = rImage.top + procInfo.nLinesRead;
	if (stat = PCDgetBlock(hPcd, &rImage, lpBuffer, lStride))
		{
		if (procInfo.fAbort)
			goto Cancelled;
		else
			{
			SetError(BEC_errPCDToolkit);
			//Message(IDS_PCDERROR, stat);
			goto BadRead;
			}
		}

	if( fConvert && fUsingCMS)
	{
		TypeConvert.ConvertData(lpBuffer, lpCMYKBuffer, y, npix, procInfo.nLinesRead);
	}

	lpLine = lpCMYKBuffer + ((long)(procInfo.nLinesRead-1) * dxBytes);

	while (--procInfo.nLinesRead >= 0)
		{
		lpOut = FramePointerRaw(lpFrame, 0, y, YES);
		if (!lpOut)
			goto BadWrite;
		lpIn = lpLine;
		if (fConvert && !fUsingCMS)
			{
			CopyFromHuge(lpIn, lpConvertLine, lStride);
			swapBGR(lpConvertLine, lpConvertLine, npix);
			TypeConvert.ConvertData(lpConvertLine, lpOut, y, npix, 1);
			}
		else
		if (fConvert)
			{
			CopyFromHuge(lpIn, lpOut, dxBytes);
			}
		else
			{
			CopyFromHuge(lpIn, lpOut, lStride);
			if (outType.DataType == FDT_RGBCOLOR)
				swapBGR(lpOut, lpOut, npix);
			}
	  	++y;
	  	lpLine -= dxBytes;
		}
	}

m_iWidth = npix;
m_iHeight = nlin;
m_iRes = xres;
m_lpFrame = lpFrame;
fRet = TRUE;
goto Exit;

NoMemory:
SetError(BEC_errMemory);
goto BadPCD;

BadRead:
SetError(BEC_errFileRead);
goto BadPCD;

BadWrite:
SetError(BEC_errFrameRead);

Cancelled:
BadPCD:
Exit:
ProgressEnd();
if ( !fRet )
	{
	if ( lpFrame )
		FrameClose( lpFrame );
	}
if (hPcd)
	PCDclose(hPcd);
if (lpBuffer)
	FreeUp (lpBuffer);
if (lpCMYKBuffer && (lpCMYKBuffer != lpBuffer))
	FreeUp (lpCMYKBuffer);
if (lpConvertLine)
	FreeUp(lpConvertLine);
return(fRet);
return(FALSE);
}

/************************************************************************/
LPFRAME CReadBitmap::ReadPCDThumbnail( short nImgNo )
/************************************************************************/
{
	FRMTYPEINFO TypeInfo;
	FrameSetTypeInfo(&TypeInfo, FDT_NONE);
	return(ReadPCDThumbnail(TypeInfo, nImgNo));
}

/************************************************************************/
LPFRAME CReadBitmap::ReadPCDThumbnail( FRMTYPEINFO outType, short nImgNo )
/************************************************************************/
{
	PCDoviewHdl	hPcd;	// handle to selected image pac
	PCDtransform Transform;
	PCDformat pcdFormat;
	PCDrect rImage;
	FRMTYPEINFO inType;
	LPTR lpOut;
	LPTR lpIn, lpBuffer;
	long lStride;
	int nLines, outDepth;
	LPFRAME lpOldFrame, lpFrame;
	int npix;
	int nlin;
	int y, i, iCount;
	FNAME szOverview;
	TCHAR szNum[10];

	lpOldFrame = NULL;
	lpFrame = NULL;
	lpBuffer = NULL;
	hPcd = NULL;

	if (!nImgNo && !IsPCDImage(m_lpFileName))
		goto BadRead;

	if( !nImgNo )
	{
		lstrcpy( szOverview, filename( m_lpFileName ));

		WORD wChar;
		LPCTSTR pcsz;

		for( i = 0; i < 4; i++ )
		{
			pcsz = MyCharNextN( szOverview, i+3 );

			if( IsDBCSLeadByte(( BYTE )*pcsz ))
			{
				wChar = *(( LPWORD )pcsz );
				*(( LPWORD )MyCharNextN( szNum, i )) = wChar;
			}
			else
			{
				wChar = *pcsz;
				*MyCharNextN( szNum, i ) = LOBYTE(  wChar );
			}
			if( wChar < '0' || wChar > '9')
				goto BadRead;
		}
		*MyCharNextN( szNum, i ) = '\0';

		nImgNo = ( short )Ascii2Int( szNum );
	}
	if (!GetPCDOverview(m_lpFileName, szOverview))
	{
		SetError(BEC_errOpen);
		goto	BadPCD;
	}

	// Call PCDopen to obtain an image pac handle
	if(PCDOopen(szOverview, &hPcd))
	{
		SetError(BEC_errOpen);
		goto	BadPCD;
	}

	// Call PCDgetRotation() to ascertain the affine transformation
	// necessary to specify before retrieving the image in order
	// to display the image rightside-up.
	//
	// In preparation for PCDloadImage(), specify the transformation
	// to be used in retrieving data from the image pac.
	if (PCDOsetSelect(hPcd, nImgNo))
		goto BadRead;
	if (PCDOgetRotation(hPcd, &Transform))
		goto BadRead;
	if (PCDOsetTransform(hPcd, Transform))
		goto BadRead;

	FrameSetTypeInfo(&inType, FDT_RGBCOLOR);
	if (outType.DataType == FDT_NONE)
		outType = inType;
	if (outType.DataType == FDT_LINEART)
		FrameSetTypeInfo(&outType, FDT_GRAYSCALE);
	else
	if (outType.DataType != FDT_RGBCOLOR)
		FrameSetTypeInfo(&outType, FDT_RGBCOLOR);
	
	if (outType.DataType == FDT_GRAYSCALE)
	{
		pcdFormat = PCD_SINGLE;
		outDepth = 1;
	}
	else
	{
		pcdFormat = PCD_RGB;
		outDepth = 3;
	}
	if (PCDOsetFormat(hPcd, pcdFormat))
		goto BadRead;
	if (PCDOsetPlaneColumn(hPcd, 1L, 3))
		goto BadRead;
	if (PCDOsetResolution(hPcd, PCD_BASE_OVER_64))
		goto BadRead;
	if (PCDOgetSize(hPcd, &rImage))
		goto BadRead;
	npix = rImage.right;
	nlin = rImage.bottom;

	if (! (lpFrame = FrameOpen(outType, npix, nlin, 100)))
	{
		SetError(BEC_errFrameOpen);
		goto BadPCD;
	}

	lStride = (long)npix * (long)outDepth;
	nLines = nlin;
	if (!(lpBuffer = Alloc(lStride*nLines)))
		goto NoMemory;

	y = 0;
	if (PCDOgetBlock(hPcd, lpBuffer, lStride))
		goto BadRead;
	lpIn = lpBuffer + ((long)(nLines-1) * lStride);
	while (--nlin >= 0)
	{
		lpOut = FramePointer(lpFrame, 0, y, YES);
		if (!lpOut)
			goto BadWrite;
		iCount = npix;
		if (outDepth == 1)
			copy(lpIn, lpOut, npix);
		else
			Convert24to24((LPRGB)lpIn, npix, lpOut);
		++y;
		lpIn -= lStride;
	}

	if (hPcd)
		PCDOclose(hPcd);
	if (lpBuffer)
		FreeUp (lpBuffer);
	return (lpFrame);

	NoMemory:
	SetError(BEC_errMemory);
	goto BadPCD;

	BadRead:
	SetError(BEC_errFileRead);
	goto BadPCD;

	BadWrite:
	SetError(BEC_errFrameRead);

	BadPCD:
	if (hPcd)
		PCDOclose(hPcd);
	if (lpBuffer)
		FreeUp (lpBuffer);
	if ( lpFrame )
		FrameClose( lpFrame );
	return (NULL);
}

/************************************************************************/
LOCAL BOOL IsPCDImage( LPTSTR lpFileName )
/************************************************************************/
{
FNAME	szFileName, szPrefix;

if (!StringsEqual(PCD_IMAGEPACK, extension(lpFileName)))
	return(FALSE);
lstrcpy(szFileName, filename(lpFileName));
lstrcpy(szPrefix, IMAGE_PAC_PREFIX);
if (!StringsEqualN(szFileName, szPrefix, lstrlen(szPrefix)))
	return(FALSE);
return(TRUE);
}

/************************************************************************/
LOCAL BOOL GetPCDOverview( LPTSTR lpFileName, LPTSTR lpOverview )
/************************************************************************/
{
	int	len;
	WORD wChar; 
	LPTSTR psz;

	/* get name of Image - .\PHOTO_CD\IMAGES\IMGNNNN.PCD */
	lstrcpy( lpOverview, lpFileName );

	/* strip off filename - .\PHOTOCD\IMAGES\ */
	stripfile( lpOverview );

	/* strip off IMAGES directory name - .\PHOTO_CD\ */
	len = MBStrlen( lpOverview );

	psz = MyCharNextN( lpOverview, len-1 );
	if( IsDBCSLeadByte(( BYTE )*psz ))
		wChar = *(( LPWORD )psz );
	else	
		wChar = *psz;

	if( wChar == '\\' )
		*psz = '\0';

	stripfile( lpOverview );

	// add name of Overview PAC - .\PHOTO_CD\OVERVIEW.PCD
	lstrcat( lpOverview, OVERVIEW_FILE );

	// see if this is a valid Photo CD Disk
	return( FileExists( lpOverview ));
}

/************************************************************************/
LOCAL void GetPCDResolution(int PhotoCDRes, PCDresolution *pcdRes, LPINT lpRes)
/************************************************************************/
{
	if (PhotoCDRes == PCD_BASE_OVER_16)
 	{
		*pcdRes = PCD_BASE_OVER_16;
		*lpRes = 25;
 	}
	else if (PhotoCDRes == PCD_BASE_OVER_4)
 	{
		*pcdRes = PCD_BASE_OVER_4;
		*lpRes = 50;
 	}
	else if (PhotoCDRes == PCD_4BASE)
 	{
		*pcdRes = PCD_4BASE;
		*lpRes = 200;
 	}
	else if (PhotoCDRes == PCD_16BASE)
 	{
		*pcdRes = PCD_16BASE;
		*lpRes = 400;
 	}
	else if (PhotoCDRes == PCD_64BASE)
 	{
		*pcdRes = PCD_64BASE;
		*lpRes = 800;
 	}
	else //if (PhotoCDRes == PCD_BASE)
 	{
		*pcdRes = PCD_BASE;
		*lpRes = 100;
 	}
} 

/***********************************************************************/
LOCAL HANDLE LoadPCDLut(LPTSTR szFileName, LPINT lpEntries)
/***********************************************************************/
{
	HANDLE hLut;
	LPTSTR lp, lpBuf, lpEnd;
	LPTR lpLut;
	long lSize, lRead;
	FILE_HANDLE fh;
	int i, nEntries;

	if (!(*szFileName))
		return(NULL);

	fh = FileOpen(szFileName, FO_READ);
	if (fh == FILE_HANDLE_INVALID)
	{
		return(NULL);
	}

	hLut = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, 512);
	if (!hLut)
	{
		FileClose(fh);
		return(NULL);
	}

	lpLut = (LPTR)GlobalLock(hLut);
	if (!lpLut)
	{
		GlobalFree(hLut);
		hLut = NULL;
		FileClose(fh);
		return(NULL);
	}

	lSize = FileSeek(fh, 0L, SEEK_END);
	lpBuf = (LPTSTR)Alloc( lSize );
	if (!lpBuf)
	{
		GlobalUnlock(hLut);
		GlobalFree(hLut);
		hLut = NULL;
		FileClose(fh);
		return(NULL);
	}
	// back to beginning
	FileSeek(fh, 0L, SEEK_SET);
	lRead = FileRead(fh, lpBuf, lSize);
	FileClose(fh);
	if (lRead != lSize)
	{
		FreeUp(lpBuf);
		GlobalUnlock(hLut);
		GlobalFree(hLut);
		hLut = NULL;
		FileClose(fh);
		return(NULL);
	}
	if (StringsEqualN( extension( szFileName ), _T(".bin"), 4 ))
	{
		if (lSize > 512)
			lSize = 512;
		copy((LPTR)lpBuf, (LPTR)lpLut, (int)lSize);
		nEntries = (int)lSize;
	}
	else
	{
		lpEnd = lpBuf + lSize;
		for (i = 0; i < 512; ++i)
		{
			lpBuf = FindNumber(lpBuf, lpEnd, &lpLut[i]);
			if (!lpBuf)
				break;
		}
		nEntries = i;
	}
	FreeUp(lpBuf);
	GlobalUnlock(hLut);

	*lpEntries = nEntries;
	return(hLut);
}

/***********************************************************************/
LOCAL LPTSTR FindNumber(LPTSTR lp, LPTSTR lpEnd, LPBYTE lpValue)
/***********************************************************************/
{
	TCHAR num[20];
	int i;
	WORD wChar;

	while( lp < lpEnd )
	{
		if( IsDBCSLeadByte(( BYTE )*lp ))
			wChar = *(( LPWORD )lp );
		else	
			wChar = *lp;

		if( isdigit( wChar ))
		{	
			i = 0;
			while( TRUE )
			{
				if( IsDBCSLeadByte(( BYTE )*lp ))
					wChar = *(( LPWORD )lp );
				else	
					wChar = *lp;

				if( !isdigit( wChar ) || ( lp >= lpEnd ))
					break;

				if( wChar & 0xff00 )
					*(( LPWORD )MyCharNextN( num, i )) = wChar;
				else
					*MyCharNextN( num, i ) = LOBYTE( wChar );

				i++; lp = MyCharNext( lp );
			}
			*MyCharNextN( num, i ) = '\0';

			*lpValue = Ascii2Int( num );
			if( lp < lpEnd )
				return( lp );
			return( NULL );
		}
		lp = MyCharNext( lp );
	}
	return( NULL );
}

/************************************************************************/
VOID EXPORT PhotoCDProgress(short nNum, short nDenom, long lData)
/************************************************************************/
{
PCDPROCINFO *pProcInfo;
int	y;

pProcInfo = (PCDPROCINFO *)lData;
y = pProcInfo->nCurrentLine + (((long)nNum * (long)pProcInfo->nLinesRead)/(long)nDenom);
if (pProcInfo->pReader->Progress(y, pProcInfo->nLines, YES))
	pProcInfo->fAbort = TRUE;
}

/************************************************************************/
PCDboolean EXPORT PhotoCDAbort(long lData)
/************************************************************************/
{
PCDPROCINFO *pProcInfo;

pProcInfo = (PCDPROCINFO *)lData;
return(pProcInfo->fAbort );
}

/************************************************************************/
void Convert24to24( LPRGB lpPixel, int iCount, LPTR lpOutput)
/************************************************************************/
{
while (--iCount >= 0)
    {
	 *lpOutput++ = lpPixel->blue;
	 *lpOutput++ = lpPixel->green;
	 *lpOutput++ = lpPixel->red;
	 ++lpPixel;
	 }
}
 
