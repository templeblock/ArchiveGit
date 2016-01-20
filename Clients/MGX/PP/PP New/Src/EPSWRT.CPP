//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "getline.h"
#include "poly.h"
#include "maskutil.h"
#include <time.h>

class CFileInfo
{
public:
	// Constructors
 	CFileInfo() {};

	// Destructor
	~CFileInfo() {};						
	
	FNAME		ofile;
	CFile 		OFile;
	FNAME		sfile[4];
	CFile		SFile[4];
};
typedef CFileInfo FAR *LPFILES;

typedef enum
	{
	MAINFILE,
	ALLFILES,
	SEPFILES,
	CSEPFILE,
	MSEPFILE,
	YSEPFILE,
	KSEPFILE
	} OUTFILE;

// Static prototypes
static BOOL PSIDout(OUTFILE OutFile, LPFILES lpFiles, int id,...);
static int SendEPSData(int Asciize,LPTR lpBuffer,int dxDest,LPTR lp);
LOCAL LPCPolyList LoadClipPath(LPTSTR lpFileName, int res);
LOCAL BOOL WriteClipPath(LPCPolyList lpPolyList, OUTFILE OutFile, LPFILES lpFiles, LPRECT lpRect, int res);
LOCAL BOOL WriteClipPoly(LPCPoly lpPoly, OUTFILE OutFile, LPFILES lpFiles, LPRECT lpRect, int res);
LOCAL BOOL WriteEPSHeader(LPFILES lpFiles, BOOL fDoDCS, LPRECT lpRect, int res,
						LPEPSOPTIONS lpOptions, FRMTYPEINFO TypeInfo, int SepFlags, LPCPolyList lpPolyList);
LOCAL BITMAP_ERROR_CODE SetupDCSFiles(LPFILES lpFiles);
LOCAL BOOL WriteCMYKData(CFile* pFile, LPTR lpIn, LPTR lpOut, LPTR lpAscii,
					int npix, BOOL Negative, BOOL Asciize);
LOCAL BOOL WriteRGBData(CFile* pFile, LPTR lpIn, LPTR lpOut, LPTR lpAscii,
					int npix, BOOL Negative, BOOL Asciize);
LOCAL BOOL WriteGrayData(CFile* pFile, LPTR lpOut, LPTR lpAscii,
					int npix, FRMTYPEINFO TypeInfo, BOOL Asciize);
LOCAL BOOL WriteDCSData(LPFILES lpFiles, LPTR lpCMYK, LPTR lpOut, LPTR lpAscii,
					int npix, BOOL Negative, BOOL Asciize);

LOCAL STATUS_CODE WriteTIFFPreview(CFile* pFile, LPOBJECT lpObject, LPFRAME lpFrame, LPEPSOPTIONS lpOptions,
					BOOL bEscapable);

#define C_ANGLE  Halftone.ScreenAngle[0]
#define M_ANGLE  Halftone.ScreenAngle[1]
#define Y_ANGLE  Halftone.ScreenAngle[2]
#define K_ANGLE  Halftone.ScreenAngle[3]
#define C_RULING Halftone.ScreenRuling[0]
#define M_RULING Halftone.ScreenRuling[1]
#define Y_RULING Halftone.ScreenRuling[2]
#define K_RULING Halftone.ScreenRuling[3]

/************************************************************************/
BOOL CWriteImage::EPSWrite(BOOL fDoDCS)
/************************************************************************/
{
#ifndef _PPC_
	int         SepFlags;
	int         depth, res;
	int         npix, nlin, bpl;
	int         i, nPasses;
	long        endps, endtiff, temp, lSize;
	LPTR		lpIn, lpOut, lpAscii;
	LPTR        lpSrc, lpCMYK;
	BOOL        Negative, bSinglePlane, fNoComposite, Asciize;
	BOOL        bEscapable;
	FRMTYPEINFO inType, outType, dcsType;
	CFrameTypeConvert TypeConvert, DCSConvert;
	LPFILES		lpFiles;
	LPOBJECT	lpObject;
	LPFRAME 	lpFrame;
	OUTFILE		OutFile;
	DITHER_TYPE DitherType;
	BOOL		fSuccess;
	STATUS_CODE	StatusCode;
	LPCPolyList lpPolyList = NULL;
	FNAME		szFileName;
	RECT		rSave, rImage;
	TFORM		tform;
	BITMAP_ERROR_CODE err;
	BOOL		fRet = FALSE;

	LPEPSOPTIONS lpOptions = (LPEPSOPTIONS)m_lpOptions;

	if (m_lpObjList)
	{
		lpObject = m_lpObjList->lpHead;
		if (lpObject)
			lpFrame = ObjGetEditFrame(lpObject);
	}
	else
	{
		lpFrame = m_lpFrame;
		lpObject = NULL;
	}

	ASSERT(lpFrame);
	ASSERT(lpOptions);
	ASSERT(m_pFile == NULL);

	inType = FrameTypeInfo(lpFrame);
	// cause FramePointer never returns line art
	if (inType.DataType == FDT_LINEART)
		inType.DataType = FDT_GRAYSCALE;
	// make sure colormap is inited for outType so we don't free bogus ptr
	FrameSetTypeInfo(&outType, FDT_NONE);

	Negative = FALSE;
	fNoComposite = fDoDCS && !lpOptions->CompositeBPP;

	nPasses = fNoComposite ? 0 : 1;
	if (lpOptions->PreviewBPP != IDC_PREVIEWNONE)
		++nPasses;

	ProgressBegin(nPasses);
   
	res = FrameResolution(lpFrame);
	depth = FrameDepth(lpFrame);
	if (depth == 0) depth = 1;

	rImage.left = rImage.top = 0;
	rImage.right = FrameXSize(lpFrame)-1;
	rImage.bottom = FrameYSize(lpFrame)-1;
	if (LookupExtFileN(lpOptions->ClipPath, szFileName, IDN_SHAPE, NO))
	{
		if (!(lpPolyList = LoadClipPath(szFileName, res)))
		{
			ProgressEnd();
			SetError(BEC_errFileRead);
			return(FALSE);
		}
		PolyListGetBounds(lpPolyList, &rSave);
		BoundRect(&rSave, rImage.left, rImage.top, rImage.right, rImage.bottom);

		TInit(&tform);
		TMove(&tform, -rSave.left, -rSave.top);
		lpPolyList->ForEach1((LPLISTPROC1)PolyTForm, (long)&tform);
	}
	else
	{
		rSave = rImage;
	}

	npix = RectWidth(&rSave);
	nlin = RectHeight(&rSave);

	TRY
	{
		lpFiles = new CFileInfo;
	}
	CATCH_ALL(e)
	{
		if (lpPolyList)
			FreePolyList(lpPolyList);
		SetError(BEC_errMemory);
		return(FALSE);
	}
	END_CATCH_ALL

	bEscapable = !FileExists(m_lpFileName);
	if (!lpFiles->OFile.Open(m_lpFileName, CFile::modeCreate|CFile::modeWrite))
 	{
		delete lpFiles;
		if (lpPolyList)
			FreePolyList(lpPolyList);
		SetError(BEC_errOpen);
		ProgressEnd();
		return(FALSE);
 	}

	lstrcpy(lpFiles->ofile, m_lpFileName);
	Uppercase(lpFiles->ofile);
	OutFile = MAINFILE;

	Asciize = !Page.BinaryEPS;
	SepFlags = RGB_SEPS;
	bSinglePlane = ( inType.DataType <= FDT_GRAYSCALE || !SepFlags ||
		(SepFlags & (C_SEP | M_SEP | Y_SEP | K_SEP)) );

	DitherType = DT_DEFAULT;
	if (fDoDCS)
	{
		outType = inType;
		FrameSetTypeInfo(&dcsType, FDT_CMYKCOLOR);
	}
	else
	{
		switch(m_DataType)
		{
			case FDT_LINEART :
				FrameSetTypeInfo(&outType, FDT_LINEART);
				bpl = (npix + 7) / 8;
			break;

			case FDT_GRAYSCALE :
				FrameSetTypeInfo(&outType, FDT_GRAYSCALE);
				bpl = npix;
			break;

			case FDT_PALETTECOLOR :
			case FDT_RGBCOLOR :
				FrameSetTypeInfo(&outType, FDT_RGBCOLOR);
				bpl = npix * 3;
			break;

			case FDT_CMYKCOLOR :
				FrameSetTypeInfo(&outType, FDT_CMYKCOLOR);
				bpl = npix * 4;
			break;

			default :
				goto Exit;
			break;
		}
	}

	lpIn = lpOut = lpAscii = lpCMYK = NULL;

	if (!(lpIn = Alloc(npix * depth)))
		goto BadAlloc;

	if (!(lpOut = Alloc(max(bpl, npix))))
		goto BadAlloc;

	// figure out how many extra characters for line endings
	lSize = npix / 40;	 	// 40 bytes, 80 chars per line
	if (npix % 40)			// any partial lines
		++lSize;			// yes
	lSize *= 2;			// 2 extra chars per line "/r/n"
	++lSize;				// null terminator
	lSize += (npix*2);		// 2 characters per pixel

	if (!(lpAscii = Alloc(lSize)))
		goto BadAlloc;

	if (fDoDCS)
	{
		if (!(lpCMYK = Alloc(npix*4)))
			goto BadAlloc;
		// initialize stuff to do the type conversion
		if (!DCSConvert.Init(inType, dcsType, npix))
			goto BadAlloc;
		if ((err = SetupDCSFiles(lpFiles)) != BEC_errNone)
		{
			SetError(err);
			goto BadEps;
		}
		OutFile = ALLFILES;
	}

	// initialize stuff to do the type conversion
	if (!TypeConvert.Init(inType, outType, npix, DitherType))
		goto BadAlloc;

	TRY
	{
		if (!WriteEPSHeader(lpFiles, fDoDCS, &rSave, res,
							lpOptions, outType, SepFlags, lpPolyList))
			goto BadWrite;

		ProgressBegin(1);
		for (i = rSave.top; i <= rSave.bottom; i++)
		{
	   		if (Progress( i-rSave.top, nlin, bEscapable ))
	   		{
	   			ProgressEnd();
	   			goto Cancelled;
	   		}

			if ( lpObject )
			{
				if (!ObjGetLine( lpObject, rSave.left, i, npix, lpIn))
				{
					ProgressEnd();
					goto BadRead;
				}
			}
			else
			{
				if ( !FrameRead( lpFrame, rSave.left, i, npix, lpIn, npix ) )
				{
					ProgressEnd();
					goto BadRead;
				}
			}


			if (!fNoComposite)
			{
				// convert the data to the new type
				TypeConvert.ConvertData(lpIn, lpOut, i, npix);

				if (outType.DataType <= FDT_GRAYSCALE)
				{
					if (!WriteGrayData(&lpFiles->OFile, lpOut, lpAscii, npix, outType, Asciize))
					{
						ProgressEnd();
						goto BadWrite;
					}
				}
				else
				if (outType.DataType == FDT_RGBCOLOR)
				{
					if (!WriteRGBData(&lpFiles->OFile, lpOut, lpIn, lpAscii, npix,
									Negative, Asciize))
					{
						ProgressEnd();
						goto BadWrite;
					}
				}
				else
				if (outType.DataType == FDT_CMYKCOLOR)
				{
					if (!WriteCMYKData(&lpFiles->OFile, lpOut, lpIn, lpAscii, npix,
										Negative, Asciize))
					{
						ProgressEnd();
						goto BadWrite;
					}
				}
				lpSrc = lpOut;
			}
			else
				lpSrc = lpIn;
			if (fDoDCS)
			{
				if (outType.DataType == FDT_CMYKCOLOR)
					lpCMYK = lpSrc;
				else
					DCSConvert.ConvertData(lpSrc, lpCMYK, i, npix);
				if (!WriteDCSData(lpFiles, lpCMYK, lpIn, lpAscii, npix,
								Negative, Asciize))
				{
					ProgressEnd();
					goto BadWrite;
				}
			}
		}
		ProgressEnd();

		if ( !Asciize )
		{
			fSuccess = PSIDout( OutFile, lpFiles, IDS_PS_ENDBINARY );
		}

		fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_GRESTORE );

		if (lpOptions->PreviewBPP == IDC_PREVIEWNONE)
		{
			fRet = TRUE;
			goto Exit;
		}


		if (!fSuccess)
			goto BadWrite;

		endps = lpFiles->OFile.GetPosition();
		lpFiles->OFile.Seek(8L,CFile::begin);
		temp = endps - 30;
		lpFiles->OFile.Write((LPTSTR)&temp,4);
		lpFiles->OFile.Seek(20L,CFile::begin);
		lpFiles->OFile.Write((LPTSTR)&endps,4); /* write file position of tiff section */
		lpFiles->OFile.Seek(endps,CFile::begin); /* seek to the end of the file */

		StatusCode = WriteTIFFPreview(&lpFiles->OFile, lpObject, lpFrame,
									lpOptions, bEscapable);
		if (StatusCode != SC_SUCCESS)
			goto StatusError;

		endtiff =lpFiles->OFile.GetPosition();
		lpFiles->OFile.Seek(24L,CFile::begin);
		temp = endtiff - endps;

		lpFiles->OFile.Write((LPTR)&temp,4);

		fRet = TRUE;
		goto Exit;
	}
	CATCH_ALL(e)
	{
		goto BadWrite;
	}
	END_CATCH_ALL

StatusError:
if (StatusCode == SC_CANCELLED)
	goto Cancelled;
if (StatusCode == SC_READERROR)
	goto BadRead;
// fall thru for all others

BadWrite:
	SetError(BEC_errWriteFile);
	goto BadEps;

BadAlloc:
	SetError(BEC_errMemory);
	goto BadEps;
BadRead:
	SetError(BEC_errFrameRead);

Exit:
Cancelled:
BadEps:

	lpFiles->OFile.Close();
	if (!fRet)
		CFile::Remove(lpFiles->ofile);
	if (fDoDCS)
	{
		for (i = 0; i < 4; ++i)
		{
			lpFiles->SFile[i].Close();
			if (!fRet)
				CFile::Remove(lpFiles->sfile[i]);
		}
	}

	if (lpPolyList)
		FreePolyList(lpPolyList);
	if (lpFiles)
		delete lpFiles;
	if (lpIn)
		FreeUp(lpIn);
	if (lpOut)
		FreeUp(lpOut);
	if (lpAscii)
		FreeUp(lpAscii);
	if (lpCMYK)
		FreeUp(lpCMYK);

	ProgressEnd();
	return(fRet);

#endif
	return(FALSE);
}

/************************************************************************/
static BOOL PSIDout( OUTFILE OutFile, LPFILES lpFiles, int id, ... )
/************************************************************************/
{
	va_list  lpArguments;
	int      i;
	TCHAR     Buffer[1024];
	LPTSTR     lpBuffer;
	LPTSTR     lpFormat;

	lpBuffer = Buffer;

	if( AstralStr( id, ( LPPTSTR )(&lpFormat )) <= 0 )
		return( FALSE );

	va_start( lpArguments, id );
	wvsprintf( lpBuffer, lpFormat, lpArguments );
	va_end( lpArguments );

	i = lstrlen( lpBuffer );

	lpBuffer[i++] = '\r';
	lpBuffer[i++] = '\n';

	TRY
	{
		if (OutFile == ALLFILES || OutFile == MAINFILE)
		{
	 		lpFiles->OFile.Write( lpBuffer, i );
		}
		if (OutFile == ALLFILES || OutFile == SEPFILES || OutFile == CSEPFILE)
		{
			lpFiles->SFile[0].Write( lpBuffer, i );
		}
		if (OutFile == ALLFILES || OutFile == SEPFILES || OutFile == MSEPFILE)
		{
			lpFiles->SFile[1].Write( lpBuffer, i );
		}
		if (OutFile == ALLFILES || OutFile == SEPFILES || OutFile == YSEPFILE)
		{
			lpFiles->SFile[2].Write( lpBuffer, i );
		}
		if (OutFile == ALLFILES || OutFile == SEPFILES || OutFile == KSEPFILE)
		{
			lpFiles->SFile[3].Write( lpBuffer, i );
		}
		return(TRUE);
	}
	CATCH_ALL(e)
	{
		return(FALSE);
	}
	END_CATCH_ALL
}

/************************************************************************/
static int SendEPSData( BOOL Asciize, LPTR lpBuffer, int dxDest,
                           LPTR lpOutBuffer )
/************************************************************************/
{
	if ( Asciize )
	{
		return( asciize( lpBuffer, dxDest, (LPTSTR)lpOutBuffer ) );
	}
	else
	{
		copyPS( lpBuffer, lpOutBuffer, dxDest );
		return( dxDest );
	}
}

//************************************************************************
LOCAL LPCPolyList LoadClipPath(LPTSTR lpFileName, int res)
//************************************************************************
{
	LPCPolyList lpPolyList;

	lpPolyList = LoadShape(lpFileName, res);
	if (lpPolyList)
	{
		POSITION pos = lpPolyList->GetHeadPosition();
		while (pos)
		{
			LPCPoly lpPoly = (LPCPoly)lpPolyList->GetNextPtr(pos);
			if (lpPoly)
				lpPoly->ClosePoly(TRUE);
		}
	}
	return(lpPolyList);
}

//************************************************************************
LOCAL BOOL WriteClipPath(LPCPolyList lpPolyList, OUTFILE OutFile, LPFILES lpFiles,
						LPRECT lpRect, int res)
//************************************************************************
{
	POSITION pos;
	LPCPoly lpPoly;
	BOOL fSuccess;

	fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_NEWPATH );

	// write out polys
	pos = lpPolyList->GetHeadPosition();
	while (pos)
	{
		lpPoly = (LPCPoly)lpPolyList->GetNextPtr(pos);
		if (!lpPoly)
			break;
		if (!WriteClipPoly(lpPoly, OutFile, lpFiles, lpRect, res))
			return(FALSE);
	}
	fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_CLIP );
	fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_NEWPATH );
	return(fSuccess);
}

//************************************************************************
LOCAL BOOL WriteClipPoly(LPCPoly lpPoly, OUTFILE OutFile, LPFILES lpFiles,
						LPRECT lpRect, int res)
//************************************************************************
{
	BOOL bAbort, fSuccess;
	POINT pts[3];
	TCHAR szNums[6][25];
	int nlin;
		
	if (lpPoly->NumPoints() <= 0)
		return(TRUE);
		
	nlin = RectHeight(lpRect);

	// write out begin point
	lpPoly->GetPoint(0, &pts[0]);
	point_to_strings(pts[0].x, nlin-pts[0].y, szNums[0], szNums[1], res); 
	fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_MOVETO, (LPTSTR)szNums[0], (LPTSTR)szNums[1]);

	// write out points and beziers
	bAbort = FALSE;
	while(!bAbort && lpPoly->ListNextPoint(&pts[0]))
	{
		if (lpPoly->IsHandle())
		{
			// get other handle
			lpPoly->ListNextPoint(&pts[1]);
			// get endpoint
			if (!lpPoly->ListNextPoint(&pts[2]))
			{
				lpPoly->NextPoint(&pts[2]);
				bAbort = TRUE;
			}
			// convert
			point_to_strings(pts[0].x, nlin-pts[0].y, szNums[0], szNums[1], res); 
			point_to_strings(pts[1].x, nlin-pts[1].y, szNums[2], szNums[3], res); 
			point_to_strings(pts[2].x, nlin-pts[2].y, szNums[4], szNums[5], res); 
			// just write out curve
			fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_CURVETO,
								(LPTSTR)szNums[0], (LPTSTR)szNums[1],
								(LPTSTR)szNums[2], (LPTSTR)szNums[3],
								(LPTSTR)szNums[4], (LPTSTR)szNums[5]);
		}
		else
		{
			// just write out point
			point_to_strings(pts[0].x, nlin-pts[0].y, szNums[0], szNums[1], res); 
			fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_LINETO, (LPTSTR)szNums[0], (LPTSTR)szNums[1]);
		}
	}
	return(fSuccess);
}

//************************************************************************
LOCAL BOOL WriteEPSHeader(LPFILES lpFiles, BOOL fDoDCS,
						LPRECT lpRect, int res,
						LPEPSOPTIONS lpOptions, FRMTYPEINFO TypeInfo,
						int SepFlags, LPCPolyList lpPolyList)
//************************************************************************
{
	LFIXED xsize, ysize;
	OUTFILE OutFile;
	WORD bpl;
	STRING szString, szXSize, szYSize;
	long ltime, lSize;
	BOOL fNoComposite, Asciize;
	BOOL fSuccess;
	BYTE buffer[30];
	STRING szAngle, szRuling;
	int npix, nlin;

	npix = RectWidth(lpRect);
	nlin = RectHeight(lpRect);

	if (lpOptions->PreviewBPP != IDC_PREVIEWNONE)
	{
		buffer[0] = 0xc5;
		buffer[1] = 0xd0;
		buffer[2] = 0xd3;
		buffer[3] = 0xc6;
		buffer[4] = 0x1e;
		set(&buffer[5], 23, 0);
		buffer[28] = 0xff;
		buffer[29] = 0xff;
		lpFiles->OFile.Write(buffer,30);
	}

	if (fDoDCS)
		OutFile = ALLFILES; // output to main and all separation files
	else
		OutFile = MAINFILE; // only to main file

	Asciize = !Page.BinaryEPS;
					  
	fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_PSADOBE );

	xsize = FGET((long)npix * 72, res);
	ysize = FGET((long)nlin * 72, res);
	FixedAscii( xsize, szXSize, -2 );
	FixedAscii( ysize, szYSize, -2 );

	fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_BOUNDINGBOX, (LPTSTR)szXSize, (LPTSTR)szYSize );

	AstralStrEx( IDS_APPNAME, szString, MAX_STR_LEN );
	fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_CREATOR, (LPTSTR)szString );

	fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_TITLE, stripdir(lpFiles->ofile));

	time(&ltime); /* get the current system time */
	fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_CREATIONDATE, (LPTSTR)ctime(&ltime));

	if (fDoDCS)
	{
		if (Calibrate.DoCalibration)
			fSuccess = PSIDout( ALLFILES, lpFiles, IDS_EPS_SUPPRESSDOT );
		fSuccess = PSIDout(MAINFILE, lpFiles, IDS_EPS_PROCESSCOLORS );
		fSuccess = PSIDout(MAINFILE, lpFiles, IDS_EPS_CYANPLATE,    stripdir(lpFiles->sfile[0]) );
		fSuccess = PSIDout(MAINFILE, lpFiles, IDS_EPS_MAGENTAPLATE, stripdir(lpFiles->sfile[1]) );
		fSuccess = PSIDout(MAINFILE, lpFiles, IDS_EPS_YELLOWPLATE,  stripdir(lpFiles->sfile[2]) );
		fSuccess = PSIDout(MAINFILE, lpFiles, IDS_EPS_BLACKPLATE,   stripdir(lpFiles->sfile[3]) );
	}

	fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_ENDCOMMENTS );
	fNoComposite = fDoDCS && !lpOptions->CompositeBPP;

	if (fNoComposite) // for now CompositeBPP is a BOOL
		OutFile = SEPFILES;  // just output to separation files from here on out

	fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_DICTDEF );

	if ( Asciize )
		fSuccess = PSIDout( OutFile, lpFiles, IDS_PS_HEXDATA );
	else
		fSuccess = PSIDout( OutFile, lpFiles, IDS_PS_BINDATA );

	if (fDoDCS)
	{
		if (Halftone.DoHalftoning)
		{
			/* Send the definition of the spot function */
			if ( Halftone.DotShape == IDC_ELLIPSEDOT )
			{
				fSuccess = PSIDout( SEPFILES, lpFiles, IDS_PS_ELLDOT1 );
				fSuccess = PSIDout( SEPFILES, lpFiles, IDS_PS_ELLDOT2 );
			}
			else
			{
				if ( Halftone.DotShape == IDC_SQUAREDOT )
				{
					fSuccess = PSIDout( SEPFILES, lpFiles, IDS_PS_SQUDOT );
				}
				else
				{
					fSuccess = PSIDout( SEPFILES, lpFiles, IDS_PS_CIRDOT );
				}
			}
			FixedAscii( C_RULING, szRuling, -2 );
			FixedAscii( C_ANGLE, szAngle, -2 );
			fSuccess = PSIDout( CSEPFILE, lpFiles, IDS_PS_SETSCREEN, (LPTSTR)szRuling, (LPTSTR)szAngle );

			FixedAscii( M_RULING, szRuling, -2 );
			FixedAscii( M_ANGLE, szAngle, -2 );
			fSuccess = PSIDout( MSEPFILE, lpFiles, IDS_PS_SETSCREEN, (LPTSTR)szRuling, (LPTSTR)szAngle );

			FixedAscii( Y_RULING, szRuling, -2 );
			FixedAscii( Y_ANGLE, szAngle, -2 );
			fSuccess = PSIDout( YSEPFILE, lpFiles, IDS_PS_SETSCREEN, (LPTSTR)szRuling, (LPTSTR)szAngle );

			FixedAscii( K_RULING, szRuling, -2 );
			FixedAscii( K_ANGLE, szAngle, -2 );
			fSuccess = PSIDout( KSEPFILE, lpFiles, IDS_PS_SETSCREEN, (LPTSTR)szRuling, (LPTSTR)szAngle );
		}
		fSuccess = PSIDout( SEPFILES, lpFiles, IDS_EPS_DOIMAGE, npix );
	}

	if (fNoComposite)
	{
	}
	else
	if (TypeInfo.DataType == FDT_LINEART)
	{
	 	fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_DOIMAGE, (npix+7)/8 );
	}
	else
	if (TypeInfo.DataType == FDT_GRAYSCALE)
	{
		fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_DOIMAGE, npix );
	}
	else
	{
		fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_BIGDEF1 );
		fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_BIGDEF2 );
		fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_BIGDEF3 );
		fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_BIGDEF4 );
		fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_BIGDEF5 );

		fSuccess = PSIDout( MAINFILE, lpFiles, IDS_PS_LINE1, npix );
		fSuccess = PSIDout( MAINFILE, lpFiles, IDS_PS_LINE2, npix );
		fSuccess = PSIDout( MAINFILE, lpFiles, IDS_PS_LINE3, npix );
		fSuccess = PSIDout( MAINFILE, lpFiles, IDS_PS_LINE4, npix );

		if (SepFlags & CMYK_SEPS)
		{
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_PS_LINE5, npix );
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_PS_DOIMAGEDEF );
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_PLANE0 );
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_PLANE1 );
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_PLANE2 );
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_PLANE3 );
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_PLANE4 );
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_PLANE5 );
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_PLANE6 );
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_PS_DEF );
		}
		else
		{
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_PS_DOIMAGEDEF );
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_PLANE7 );
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_PLANE8 );
			fSuccess = PSIDout( MAINFILE, lpFiles, IDS_EPS_PLANE9 );
		}
		fSuccess = PSIDout( MAINFILE, lpFiles, IDS_PS_DEF );
	}

	if (lpPolyList)
	{
		if (!WriteClipPath(lpPolyList, OutFile, lpFiles, lpRect, res))
			return(FALSE);
	}

	fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_SCALE, (LPTSTR)szXSize, (LPTSTR)szYSize );
	fSuccess = PSIDout( OutFile, lpFiles, IDS_EPS_TRANSFORM,
		npix,
		nlin,
		TypeInfo.DataType == FDT_LINEART ? 1 : 8,
		npix,
		nlin,
		nlin );

	if (TypeInfo.DataType == FDT_LINEART)
		bpl = (npix + 7)/8;
	else
	if (TypeInfo.DataType == FDT_RGBCOLOR)
		bpl = npix * 3;
	else
	if (TypeInfo.DataType == FDT_CMYKCOLOR)
		bpl = npix * 4;
	else
	//if (TypeInfo.DataType == FDT_GRAYSCALE)
		bpl = npix;
		
	if ( !Asciize )
	{ // The size must include the doimage command that follows
		lSize = (long)bpl * nlin;
		fSuccess = PSIDout( OutFile, lpFiles, IDS_PS_BEGINBINARY, lSize + 9 + 2 );
	}
	fSuccess = PSIDout( OutFile, lpFiles, IDS_PS_DOIMAGE ); // 9 characters
	return(fSuccess);
}

//************************************************************************
LOCAL BITMAP_ERROR_CODE SetupDCSFiles(LPFILES lpFiles)
//************************************************************************
{
	int i;

	lstrcpy(lpFiles->sfile[0], lpFiles->ofile);
	stripext(lpFiles->sfile[0]);

	lstrcpy(lpFiles->sfile[1], lpFiles->sfile[0]);
	lstrcpy(lpFiles->sfile[2], lpFiles->sfile[0]);
	lstrcpy(lpFiles->sfile[3], lpFiles->sfile[0]);

	lstrcat(lpFiles->sfile[0], ".C");
	lstrcat(lpFiles->sfile[1], ".M");
	lstrcat(lpFiles->sfile[2], ".Y");
	lstrcat(lpFiles->sfile[3], ".K");

	for (i = 0; i < 4; ++i)
	{
		if (!lpFiles->SFile[i].Open(lpFiles->sfile[i], CFile::modeCreate|CFile::modeWrite))
		{
			for (--i; i >= 0; --i)
 			{
				lpFiles->SFile[i].Close();
				CFile::Remove(lpFiles->sfile[i]);
			}
		return(BEC_errWrite);
		}
	}
	return(BEC_errNone);
}

//************************************************************************
LOCAL BOOL WriteRGBData(CFile* pFile, LPTR lpIn, LPTR lpOut, LPTR lpAscii,
					int npix, BOOL Negative, BOOL Asciize)
//************************************************************************
{
	LPRGB lpRGB;
	LPTR lp;
	int x;
	DWORD length;
	BYTE r, g, b;

	TRY
	{
		lpRGB = (LPRGB)lpIn;
		lp = lpOut;
		for (x = 0; x < npix; x++, lpRGB++)
			*lp++ = lpRGB->red;
		if (Negative)
			negate(lpOut, (long)npix);
		length = SendEPSData( Asciize, lpOut, npix,lpAscii);
		pFile->Write(lpAscii, length );

		lpRGB = (LPRGB)lpIn;
		lp = lpOut;
		for (x = 0; x < npix; x++, lpRGB++)
			*lp++ = lpRGB->green;
		if (Negative)
			negate(lpOut, (long)npix);

		length = SendEPSData( Asciize, lpOut, npix,lpAscii);
		pFile->Write(lpAscii, length );

		lpRGB = (LPRGB)lpIn;
		lp = lpOut;
		for (x = 0; x < npix; x++, lpRGB++)
			*lp++ = lpRGB->blue;

		if (Negative)
			negate(lpOut, (long)npix);

		length = SendEPSData( Asciize, lpOut, npix,lpAscii);

		pFile->Write(lpAscii, length );

		lpRGB = (LPRGB)lpIn;
		lp = lpOut;
		for (x = 0; x < npix; x++, lpRGB++)
		{
			r = lpRGB->red;
			g = lpRGB->green;
			b = lpRGB->blue;
			*lp++ = TOLUM(r,g,b);
		}
		if (Negative)
			negate(lpOut, (long)npix);
		length = SendEPSData( Asciize, lpOut, npix,lpAscii);
		pFile->Write(lpAscii, length );
		return(TRUE);
	}
	CATCH_ALL(e)
	{
		return(FALSE);
	}
	END_CATCH_ALL
}

//************************************************************************
LOCAL BOOL WriteGrayData(CFile* pFile, LPTR lpOut, LPTR lpAscii,
					int npix, FRMTYPEINFO TypeInfo, BOOL Asciize)
//************************************************************************
{
	int bpl;
	DWORD length;

	if (TypeInfo.DataType == FDT_LINEART)
	{
		LPTR lp = lpOut;
		lp += (npix >> 3);

		// Make the last bits white(1)
		if (npix & 7)
			*lp |= (0xFF) >> (npix & 7);
		bpl = (npix + 7) / 8;
	}
	else
		bpl = npix;

	/* convert to ascii for postscript */
	length = SendEPSData( Asciize, lpOut, bpl, lpAscii);

	TRY
	{
		/* write lines */
		pFile->Write(lpAscii, length );
		return(TRUE);
	}
	CATCH_ALL(e)
	{
		return(FALSE);
	}
	END_CATCH_ALL
}

//************************************************************************
LOCAL BOOL WriteCMYKData(CFile* pFile, LPTR lpIn, LPTR lpOut, LPTR lpAscii,
					int npix, BOOL Negative, BOOL Asciize)
//************************************************************************
{
	LPTR lp;
	int x;
	DWORD length;
	LPCMYK lpCMYK;

	TRY
	{
		lpCMYK = (LPCMYK)lpIn;
		lp = lpOut;
		for (x = 0; x < npix; x++, lpCMYK++)
			*lp++ = lpCMYK->c;

		if (Negative)
			negate(lpOut, (long)npix);

		length = SendEPSData( Asciize, lpOut, npix,lpAscii);
		pFile->Write(lpAscii, length );

		lpCMYK = (LPCMYK)lpIn;
		lp = lpOut;
		for (x = 0; x < npix; x++, lpCMYK++)
			*lp++ = lpCMYK->m;

		if (Negative)
			negate(lpOut, (long)npix);

		length = SendEPSData( Asciize, lpOut, npix,lpAscii);
		pFile->Write(lpAscii, length );

		lpCMYK = (LPCMYK)lpIn;
		lp = lpOut;
		for (x = 0; x < npix; x++, lpCMYK++)
			*lp++ = lpCMYK->y;

		if (Negative)
			negate(lpOut, (long)npix);

		length = SendEPSData( Asciize, lpOut, npix,lpAscii);
		pFile->Write(lpAscii, length );

		lpCMYK = (LPCMYK)lpIn;
		lp = lpOut;
		for (x = 0; x < npix; x++, lpCMYK++)
			*lp++ = lpCMYK->k;

		if (Negative)
			negate(lpOut, (long)npix);

		length = SendEPSData( Asciize, lpOut, npix,lpAscii);
		pFile->Write(lpAscii, length );

		return(TRUE);
	}
	CATCH_ALL(e)
	{
		return(FALSE);
	}
	END_CATCH_ALL
}

//************************************************************************
LOCAL BOOL WriteDCSData(LPFILES lpFiles, LPTR lpCMYK, LPTR lpOut, LPTR lpAscii,
					int npix, BOOL Negative, BOOL Asciize)
//************************************************************************
{
	int plane, j;
	LPTR lp, lpPlane;
	DWORD length;

	TRY
	{
		for (plane = 0; plane < 4; ++plane)
		{
			lp = lpOut;
			lpPlane = lpCMYK + plane;
			for (j = 0; j < npix; ++j)
			{
				*lp++ = *lpPlane;
				lpPlane += 4;
			}
			if (!Negative)
				negate(lpOut, npix);

			length = SendEPSData( Asciize, lpOut, npix, lpAscii);
			lpFiles->SFile[plane].Write(lpAscii, length );
		}
		return(TRUE);
	}
	CATCH_ALL(e)
	{
		return(FALSE);
	}
	END_CATCH_ALL
}

//************************************************************************
LOCAL STATUS_CODE WriteTIFFPreview(CFile* pFile, LPOBJECT lpObject, LPFRAME lpFrame,
									LPEPSOPTIONS lpOptions,	BOOL bEscapable)
//************************************************************************
{
	LPTR lp;
	int npix, nlin, res, DestWidth, DestHeight, depth, i, line, j;
	RGBS whiteRGB;
	long whitePixel;
	FRMTYPEINFO inType;
	STATUS_CODE StatusCode;
	RECT 		rSave, rImage;
	LFIXED      xrate, yrate;
	FNAME		szFileName;
	TFORM		tform;
	int 		SaveType;
	TIFFOPTIONS Options;
	BOOL		fOptimize;
	LPTR		lpOut;
	LPTR		lpIn = NULL;
	LPMASK		lpMask = NULL;
	LPFRAME 	lpDstFrame = NULL;
	LPCPolyList	lpPolyList = NULL;

	TRY
	{
		ProgressBegin(2, 0);

		res = FrameResolution(lpFrame);
		depth = FrameDepth(lpFrame);
		if (!depth) depth = 1;

		// get original size of image
		npix = FrameXSize(lpFrame);
		nlin = FrameYSize(lpFrame);

		// get rectangle in file coordinates
		rSave.left = rSave.top = 0;
		rSave.right = npix-1;
		rSave.bottom = nlin-1;

		if (LookupExtFileN(lpOptions->ClipPath, szFileName, IDN_SHAPE, NO))
		{
			// get shape in image resolution
			lpPolyList = LoadShape(szFileName, res);
			if (lpPolyList)
			{
				rImage = rSave;
				PolyListGetBounds(lpPolyList, &rSave);
				BoundRect(&rSave, rImage.left, rImage.top, rImage.right, rImage.bottom);

				TInit(&tform);
				TMove(&tform, -rSave.left, -rSave.top);
				lpPolyList->ForEach1((LPLISTPROC1)PolyTForm, (long)&tform);

				POSITION pos = lpPolyList->GetHeadPosition();
				while (pos)
				{
					LPCPoly lpPoly = (LPCPoly)lpPolyList->GetNextPtr(pos);
					if (lpPoly)
						lpPoly->ClosePoly(TRUE);
				}
				npix = RectWidth(&rSave);
				nlin = RectHeight(&rSave);
			}
		}

		// get image size scaled down to points 72/inch
		DestWidth  = ((long)npix*72) / res;
		DestHeight = ((long)nlin*72) / res;

		if (lpPolyList)
		{
			lpMask = MaskCreate(NULL, npix, nlin, OFF, TRUE, FDT_GRAYSCALE);
			if (lpMask)
			{
				if (!MaskAddPoly( lpMask, lpPolyList, SHAPE_ADD, FALSE))
				{
					MaskClose(lpMask);
					lpMask = NULL;
				}
			}
			FreePolyList(lpPolyList);
		}

		FrameGetTypeInfo(lpFrame, &inType);
		if (inType.DataType == FDT_LINEART || lpOptions->PreviewBPP == IDC_PREVIEW1)
			SaveType = IDC_SAVESP;
		else
		if (inType.DataType <= FDT_GRAYSCALE || lpOptions->PreviewBPP == IDC_PREVIEW8)
			SaveType = IDC_SAVECT;
		else
			SaveType = IDC_SAVE8BITCOLOR;

		if (!(lpDstFrame = FrameOpen(inType, DestWidth, DestHeight, 72)))
		{
			FrameError( IDS_EPROCESSOPEN );
			StatusCode = SC_MEMERROR;
			goto Exit;
		}

		if (!(lpIn = Alloc(npix*depth)))
		{
			StatusCode = SC_MEMERROR;
			goto Exit;
		}

		whiteRGB.red = whiteRGB.green = whiteRGB.blue = 255;
		FrameRGB2Pixel( lpFrame, &whiteRGB, &whitePixel );
		yrate = FGET( nlin, DestHeight );
		xrate = FGET( npix, DestWidth );
		ProgressBegin(1, 0);
		for (i= 0; i < DestHeight; i++)
		{
			if (AstralClockCursor( i, DestHeight, bEscapable ))
			{
				ProgressEnd();
				StatusCode = SC_CANCELLED;
				goto Exit;
			}

			line = FMUL( i, yrate ) + rSave.top;

			lpOut = FramePointer(lpDstFrame, 0, i, YES);
			if (!lpOut)
				continue;

			if ( lpObject )
			{
				if (!ImgGetLine( NULL, lpObject, rSave.left, line, npix, lpIn))
				{
					ProgressEnd();
					StatusCode = SC_READERROR;
					goto Exit;
				}
			}
			else
			{
				if (!FrameRead(lpFrame, rSave.left, line, npix, lpIn, npix))
				{
					ProgressEnd();
					StatusCode = SC_READERROR;
					goto Exit;
				}
			}

			if (lpMask)
			{
				MaskLoad( lpMask, 0, line-rSave.top, npix, lpMask->mbuffer );
				LPTR lpMsk = lpMask->mbuffer;
				lp = lpIn;
				for ( j=0; j<npix; j++ )
				{
					if (!(*lpMsk++))
						FrameSetPixel( lpFrame, whitePixel, lp, 1);
					lp += depth;
				}
			}
			FrameSample(lpFrame, lpIn, 0, lpOut, 0, DestWidth, xrate );

		}
		ProgressEnd();
		fOptimize = Convert.fOptimize;
		Convert.fOptimize = FALSE;
		CWriteImage TiffWrite(pFile, IDN_TIFF);
		Options.TIFFCompress = Options.TIFFDiff = FALSE;
		Options.bWritePT = NO;
		Options.bSaveMask = NO;
		TiffWrite.SetOptions(&Options);
		if (TiffWrite.WriteFrame(lpDstFrame, SaveType))
			StatusCode = SC_SUCCESS;
		else
			StatusCode = SC_WRITEERROR;
		Convert.fOptimize = fOptimize;
		goto Exit;
	}
	CATCH_ALL(e)
	{
		StatusCode = SC_WRITEERROR;
		goto Exit;
	}
	END_CATCH_ALL

Exit:
	if (lpIn)
		FreeUp(lpIn);
	if (lpMask)
		MaskClose(lpMask);
	if (lpDstFrame)
		FrameClose(lpDstFrame);

	ProgressEnd();
	return(StatusCode);
}
