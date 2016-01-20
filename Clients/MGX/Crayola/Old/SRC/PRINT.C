//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// This has to stay here because MGXPS.DRV is not yet fully WIN3.1!
#define NOTWIN31

/************************************************************************\
	Print.c   print functions for astral's picture publisher
\************************************************************************/

#include <windows.h>
#include <time.h>
#include "id.h"
#include "data.h"
#include "routines.h"

/************************************************************************/

#define LX(x) ((int) (((long)x*1200L)/(long)iPrResX) )
#define LY(y) ((int) (((long)y*1200L)/(long)iPrResY) )
#define PX(x) ((int) (((long)x*72L)/(long)iPrResX) )
#define PY(y) ((int) (((long)y*72L)/(long)iPrResY) )

/************************************************************************\

	NOTE: these routines send a document to the print spooler, and also
	control a modeless dialog box which lets the user abort the job.
	The way this application prints is by a method called "nextbanding".
	Instead of sending a whole document at once to the spooler, nextbanding
	sends a portion of the document at a time.  This method is faster and
	doesn't require as much disk space.

	Printed page

	            _______dxPrinter_______
	           |                       |
	           |       dxOutput        |
	           |       +-------+       |
	           |       |       |       |
	           |____dxDest_____|_______|
	rBand ---> |       |       |       dyDest
	           |_______|_______|_______|
	           |       |       |       |
	           |  dyOutput     |       dyPrinter
	           |       +-------+       |
	           |                       |
	           |_______________________|

	Each time you call Escape(..,NEXTBAND,..,..,..) you are given the
	coordinates of the current band on the printer page.  You must then
	compute what you want to print in this band, and then use GDI functions
	to write to the printerDC.

\***********************************************************************/

extern HWND     hWndAstral;
extern HINSTANCE hInstAstral;   /* handle to calling application instance */
extern char     szAppName[MAX_STR_LEN];    /* calling application name */

static DLGPROC lpDlgFnAbort;
//static RXCALLBACK lpDlgFnAbort;
static HWND     hWndAbort;      /* handle to modeless abort dialog box */
static FNAME    szFilename;     /* file to print. (no path) */
static STRING   szDocName;
static STRING   szPrinter;      /* printer device name (NAME,DRIVER,PORT) */
static LPTR     lpchDevice,     /* pointers into the szPrinter string */
		lpchDriver,
		lpchPort;

int     iEscErr;
HDC     hPrinterDC;     /* DC of the printer */
ABORTPROC lpAbortTest;
BOOL    fAbortPrint;    /* abort sending job to spooler? */

BOOL    BandingDevice;
short   BandInfoDevice;
BOOL    bNextBandReached = FALSE;
RECT    rBand, rPrintBand;/* 4 coordinates of current band of printer */

DOCINFO PrinterDocInfo;

/***************************************************************************/

int PrintLabelCentered( HDC hDC, int x, int y, COLOR Color, int idStr, ... )
{
	char szBuffer[1024];
	LPTR lpArguments;
	LPSTR lpString;
	int retc, length;
	SIZE Extent;
	int dx, dy;

	/* The caller passes stringtable ID's, and not hardcoded strings */
	/* Used to put a variety of labels on the printed page */
	if ( !AstralStr( idStr, &lpString ) )
		return( IDCANCEL );

	lpArguments = (LPTR)&idStr + sizeof(idStr);
	length = wvsprintf( szBuffer, lpString, (LPSTR)lpArguments );

	GetTextExtentPoint( hDC, lpString, length, &Extent );

	dx = Extent.cx;
	dy = Extent.cy;

	ColorText( hDC, x-(dx/2), y-(dy/2), szBuffer, length, Color );

	return( retc );
}

/************************************************************************/
// set lpImage to NULL to use lpFrame
/************************************************************************/

int PrintImage(
	HDC     hDC,
	BOOL    bToScreen,
	LPRECT  lpSourceRect,
	LPRECT  lpDestRect,
	int     xDiva,
	int     yDiva,
	int     yTop,
	int     yTotal,
	int     nTotalBits,
	LPIMAGE lpImage,
	LPFRAME lpFrame)
{
int y, yline, ystart, ylast, DispWidth, count, sx, Excess, depth, depthOut;
LFIXED yrate, xrate, yoffset;
RECT rPaint, rSource, rDest;
long dx, dy;
LPRECT lpRepairRect;
LPBLT lpBltPrint;
BOOL bPrintAsGray;
BLTSESSION BltSession;
HPALETTE hOldPal;
LPFRAME lpBaseFrame;
LPTR lpBuffer;
LPTR lpImageData;
long lSizeNeeded;
#ifdef STATIC16  // Only in the new framelib
FRMTYPEINFO TypeInfo;
#endif

#define PRLINES 10

ProgressBegin(1,0);

if (lpImage)
	lpBaseFrame = ImgGetBaseEditFrame(lpImage);
else
	lpBaseFrame = lpFrame;

depth = FrameDepth( lpBaseFrame );
if (depth == 0) depth = 1;

rDest = *lpDestRect;
rSource = *lpSourceRect;
lpRepairRect = NULL;
DispWidth = RectWidth( &rDest );
lSizeNeeded = (long)DispWidth * depth;

if ( !(lpBuffer = Alloc(lSizeNeeded)) )
{
	ProgressEnd();
	return(FALSE);
}

if (!AllocLines((LPPTR)&lpImageData, 1, FrameXSize(lpBaseFrame), depth))
{
	FreeUp( lpBuffer );
	ProgressEnd();
	return(FALSE);
}

if ( !lpRepairRect )
	rPaint = rDest;
else
	{
	rPaint = *lpRepairRect;
	BoundRect( &rPaint, rDest.left, rDest.top, rDest.right, rDest.bottom );
	}

sx = rSource.left;
yrate = FGET( RectHeight(&rSource), RectHeight(&rDest) );
xrate = FGET( RectWidth(&rSource), RectWidth(&rDest) );
ystart = rSource.top;
ylast = -1;

// Due to a bug in the postscript driver when 1 line is blt'ed
if ( !bToScreen )
	{
	Excess = RectHeight( &rPaint ) % PRLINES;
	if ( Excess == 1 )
		rPaint.bottom--;
	}

dx = rPaint.left - rDest.left;
dy = rPaint.top - rDest.top;

yoffset = (dy*yrate)+((long)yrate>>1);
count = dx + RectWidth(&rPaint);

bPrintAsGray = (!bToScreen && nTotalBits == 8 && depth != 1);
if (bPrintAsGray)
		depthOut = 1;
else	depthOut = depth;

if ( bToScreen )
	lpBltPrint = lpBltScreen;
else
	{
	if ( Page.ScatterPrint )
		nTotalBits = 1;
	else
	if ( !nTotalBits )
		nTotalBits = (depthOut == 1 ? 8 : 24);
	if ( Page.UsePrinterScreening )
		nTotalBits = -nTotalBits;
	lpBltPrint = InitSuperBlt( hDC, nTotalBits, (LPTR)SBLTLineBuffer[0], NO, YES );
	if (!lpBltPrint)
	{
		ProgressEnd();
		return(FALSE);
	}
	hOldPal = SelectPalette(hDC, (HPALETTE)GetStockObject(DEFAULT_PALETTE), FALSE);
	SelectPalette(hDC, hOldPal, FALSE);
	}

#ifdef STATIC16 // Only in the new framelib
FrameSetTypeInfo(&TypeInfo, (depthOut == 1 ? FDT_GRAYSCALE : FDT_RGBCOLOR), NULL);
StartSuperBlt( &BltSession, hDC, NULL, lpBltPrint, &rPaint, TypeInfo,
	PRLINES, xDiva, yDiva, bToScreen, NULL );
#else
StartSuperBlt( &BltSession, hDC, NULL, lpBltPrint, &rPaint, depthOut,
	PRLINES, xDiva, yDiva, bToScreen, NULL );
#endif

for ( y=rPaint.top; y<=rPaint.bottom; y++ )
	{
	if ( !bToScreen )
		{
		if ( AstralClockCursor( yTop+y-rPaint.top, yTotal, YES ) )
			{
			fAbortPrint = YES;
			break;
			}
		/* Check for user input to abort dialog box */
		(*lpAbortTest)(hPrinterDC, 0);
		if ( fAbortPrint )
			break;
		}
#ifdef WIN32		
	yline = ystart + WHOLE( yoffset );
#else
	yline = ystart + HIWORD( yoffset );
#endif
	yoffset += yrate;
	if ( yline != ylast )
	{
		ylast = yline;

		if (lpImage)
			ImgGetLine( lpImage, NULL, sx, yline, FrameXSize(lpBaseFrame) - sx, lpImageData );
		else 
			copy(FramePointer(lpBaseFrame, sx, yline, NO), lpImageData, (FrameXSize(lpBaseFrame) - sx)*depth);

		FrameSample( lpBaseFrame, lpImageData, 0, lpBuffer, (WORD)dx, count,
			xrate );

		if (bPrintAsGray || depth != depthOut)
		{
			if (depthOut < depth)
				ConvertData( lpBuffer, depth, count,
					lpBuffer, depthOut );
		}

		if (!bToScreen)
		{
			if ( Page.Negative )
				negate( lpBuffer, (long)count * depthOut );
			if ( depthOut == 1 )
				CorrectGray( lpBuffer, count, YES, NO );
			else
			if ( depthOut == 3 && depth != 4 )
				CorrectRGB( (LPRGB)lpBuffer, count,YES, NO);
		}
	}
	SuperBlt( &BltSession, lpBuffer );
	}
SuperBlt( &BltSession, NULL );
if (!bToScreen)
	{
	SelectPalette(hDC, hOldPal, FALSE);
	EndSuperBlt(lpBltPrint);
	}

if (lpBuffer)
	FreeUp(lpBuffer);

if (lpImageData)
	FreeUp(lpImageData);

ProgressEnd();
return( TRUE );
}

/***********************************************************************\

	Printfile reads win.ini to get information about the user's printer,
	creates a DC for the printer, puts up a modeless dialog box informing 
	the user that the job is being sent, and finally sends the job to
	the spooler one band at a time.
	
	Set lpImage to NULL to use lpFrame.

\***********************************************************************/

int PrintFile (
	HWND    hWnd,        /* window handle of parent window */
	LPSTR   szFile,      /* name of file to print.  (no path!) */
	BOOL    EjectPage,
	LPIMAGE lpImage,
	LPFRAME lpFrame)
{
	int    iPrResX,   iPrResY;
	int    xDest,     yDest;     /* location of current slice on printer */
	int    dxDest,    dyDest;    /* width & height of each printer band */
	int    xSrc,      ySrc;      /* location of current slice of source */
	int    dxSrc,     dySrc;     /* width & height of current slice of src */
	int    dxOutput,  dyOutput;  /* width & height of image area on page */
	int    dxPrinter, dyPrinter; /* width & height of page */
	LFIXED xScale,    yScale;    /* scale factor of dest to source image */
	int    dyDestSlice;          /* height of current slice */
	RECT   rImage;               /* 4 coord of the image on the page */
	STRING szDocName;            /* spooler only takes 32 char doc name */
	STRING szTech;
	RECT   rSource, rDest;
	LPSTR  pTech;
	BOOL   bSuperBlt;
	BYTE   SepList[5];
	int    iSep, x, y;
	int    nTotalBits;
	LFIXED f, tfMirror[9];
	RECT   rRect;
	PAGE   OldPage;
	int    NumColors;
	static int xDiva, yDiva;
	LPFRAME lpBaseFrame;
//	struct ORIENT
//	{
//	DWORD Orientation;
//	DWORD Reserved1;
//	DWORD Reserved2;
//	DWORD Reserved3;
//	DWORD Reserved4;
//	} Orient;

	struct
	{
		BOOL    fGraphics;
		BOOL    fText;
		RECT    rBand;
	} Info;

	if (lpImage)
		lpBaseFrame = ImgGetBaseEditFrame(lpImage);
	else
		lpBaseFrame = lpFrame;

	/* put document name in szDocName */
	lstrcpy( szFilename, szFile );
	lstrcpy( szDocName, szAppName );
	lstrcat( szDocName, " " );
	lstrcat( szDocName, szFilename );

	if (EjectPage)
	{
		fAbortPrint = FALSE;
	}
	iEscErr = 0;
	OldPage = Page;

	if (EjectPage)
	{
		/* get the printer of the current printer */
		if ( !(hPrinterDC = GetPrinterDC()) )
			return( FALSE );
	}

	/* Find out the output device color type */
	NumColors = GetDeviceCaps(hPrinterDC, NUMCOLORS);

	if (NumColors < 0) NumColors = 32000;

	if (Page.OutputType == IDC_PRINT_COLOR)
	{ 
		if (NumColors <= 2)
		{
			Page.OutputType = IDC_PRINT_GRAY;
		}
	}

	if (EjectPage)
	{
		/* Determine the banding capabilities of the printer driver */
		BandingDevice = GetDeviceCaps(hPrinterDC, RASTERCAPS) & RC_BANDING;

		bNextBandReached = FALSE;

		BandInfoDevice = BANDINFO;
		BandInfoDevice = Escape(hPrinterDC, QUERYESCSUPPORT, 2,
			(LPSTR)&BandInfoDevice, NULL);

		hWndAbort = NULL;

		/* Set up abort dialog box */
		lpDlgFnAbort = (DLGPROC)MakeProcInstance( (FARPROC)DlgFnAbort, hInstAstral );
//		lpDlgFnAbort = (RXCALLBACK)MakeProcInstance( (FARPROC)DlgFnAbort, hInstAstral );
		EnableWindow( hWnd, FALSE );
		GetClientRect( hWndAstral, &rRect );
		InflateRect( &rRect, 1, 1 );
		hWndAbort = CreateDialog(
			hInstAstral, MAKEINTRESOURCE( IDD_ABORT ),
			hWnd, lpDlgFnAbort);
//		hWndAbort = RxCreateDialog(
//			hInstAstral, hWnd, MAKEINTRESOURCE( IDD_ABORT ),
//			&rRect, lpDlgFnAbort, NULL);

		/* Give abort procedure to gdi to check abort print command */
		lpAbortTest = (ABORTPROC)MakeProcInstance( (FARPROC)AbortTest, hInstAstral );

#ifdef NOTWIN31
//		Orient.Orientation = 1; // Landscape
//		Escape( hPrinterDC, GETSETPRINTORIENT, sizeof(Orient),
//			(LPSTR)&Orient, 0L );

		if ((iEscErr = Escape(hPrinterDC, SETABORTPROC,
			0, (LPSTR)lpAbortTest,0L)) <0)
		{
			goto EXIT;
		}

		/* Send document name to spooler */
		if ((iEscErr = Escape(hPrinterDC, STARTDOC,
			lstrlen( szDocName ), (LPSTR)szDocName, 0L)) < 0)
		{
			goto EXIT;
		}
#else // WIN3.1
		if ((iEscErr = SetAbortProc( hPrinterDC, lpAbortTest )) < 0)
		{
			goto EXIT;
		}

		PrinterDocInfo.cbSize = sizeof(DOCINFO);
		PrinterDocInfo.lpszDocName = szDocName;
		PrinterDocInfo.lpszOutput  = NULL;	

		if ((iEscErr = StartDoc( hPrinterDC, &PrinterDocInfo )) < 0)
		{
			goto EXIT;
		}
#endif // WIN3.1
	}

	/* Get size of printer page */
	dxPrinter = GetDeviceCaps(hPrinterDC, HORZRES);
	dyPrinter = GetDeviceCaps(hPrinterDC, VERTRES);

	if (EjectPage)
	{
		if( !BandingDevice )
		{
			/* Create a single band for the entire page */
			rPrintBand.top    = 0;
			rPrintBand.left   = 0;
			rPrintBand.bottom = dyPrinter-1;
			rPrintBand.right  = dxPrinter-1;
		}
	}

	/* Use the device resolution (in pixels per inch) to compute */
	/* how big the image will be on the output device */
	iPrResX = GetDeviceCaps(hPrinterDC, LOGPIXELSX);
	iPrResY = GetDeviceCaps(hPrinterDC, LOGPIXELSY);
	dxOutput = FMUL( iPrResX, Edit.Width );
	dyOutput = FMUL( iPrResY, Edit.Height );

	x = dxPrinter;
	y = dyPrinter;
	ScaleToFit( &x, &y, dxOutput, dyOutput );
	dxOutput = x;
	dyOutput = y;

	/* Locate the image on the page */
	if ( Page.Centered )
	{
		rImage.left = rImage.right  = (dxPrinter-dxOutput)/2;
		rImage.top  = rImage.bottom = (dyPrinter-dyOutput)/2;
	}
	else
	{
		rImage.left = rImage.right  = FMUL( iPrResX, Page.OffsetX );
		rImage.top  = rImage.bottom = FMUL( iPrResY, Page.OffsetY );
	}
	rImage.right  += (dxOutput-1);
	rImage.bottom += (dyOutput-1);

	/* Compute the scale factors that will help transform */
	/* destination coordinates to image pixel coordinates */
	xScale = FGET( CROP_WIDTH, dxOutput );
	yScale = FGET( CROP_HEIGHT, dyOutput );

	bSuperBlt = YES;

	if ( !Page.ScatterPrint )
	{
		/* Get the technology parameters */
		/* The 1st technology parameter is either "PostScript" or "PCL" */
		if ( Escape(hPrinterDC, GETTECHNOLGY, NULL, NULL, szTech) < 0 )
			szTech[0] = '\0';

		/* The 2nd parameter may be "LaserPort" if the the 1st is "PCL" */
		/* If so, point to the 2nd technology parameter "Laserport" */
		if ( !lstrcmp( "PCL", szTech ) )
		{
			pTech = szTech;
			while( *pTech )
				pTech++;
			lstrcpy( szTech, pTech+1 );
		}

		if ((!lstrcmp( "LaserPort", szTech ) &&
			FrameDepth(lpBaseFrame) == 1) ||
			!lstrcmp( "PostScript", szTech ) )
		{
			/* For HP grayscale boards and PostScript devices... */
			/* Someday tell people about the printing technology */
			BandingDevice = NO;
			BandInfoDevice = NO;
			bSuperBlt = NO;
		}
	}

	InitRGBtoCMYK();

	AstralCursor( IDC_WAIT );
	if ( Page.OutputType == IDC_PRINT_COLORSEPS ||
		 Page.OutputType == IDC_PRINT_BLACKSEPS )
	{
		iSep = 0;
		if (Page.SepFlags & C_SEP) SepList[iSep++] = 'C';
		if (Page.SepFlags & M_SEP) SepList[iSep++] = 'M';
		if (Page.SepFlags & Y_SEP) SepList[iSep++] = 'Y';
		if (Page.SepFlags & K_SEP) SepList[iSep++] = 'K';
		if ( !iSep ) SepList[iSep++] = 'K';
			SepList[iSep] = '\0';
	}
	else if ( Page.OutputType == IDC_PRINT_GRAY )
	{
		SepList[0] = 'X';
		SepList[1] = '\0';
	}
	else
	{
		SepList[0] = '\0';
		SepList[1] = '\0';
	}

	iSep = 0; // Start with the first sep in the list

StartPage:

	/* Send the mirror flag */
	if ( Page.EmulsionDown )
	{
		f = FGET( dxPrinter, 1 );
		tfMirror[0] = -UNITY; tfMirror[1] = 0;     tfMirror[2] = 0;
		tfMirror[3] = 0;      tfMirror[4] = UNITY; tfMirror[5] = 0;
		tfMirror[6] = f;      tfMirror[7] = 0;     tfMirror[8] = UNITY;

		if ( Escape(hPrinterDC, TRANSFORM_CTM, sizeof(tfMirror),
			(LPSTR)tfMirror, 0L) < 0 )
		; // So what
	}

	/* Loop to transfer one band at a time to the spooler. */
	do
	{
		/* Check for user input to abort dialog box */
		(*lpAbortTest)(hPrinterDC, 0);
		if ( fAbortPrint )
			break;

		if (EjectPage)
		{
			if ( BandingDevice )
			{
				/* Get the next band rectangle */
				if ((iEscErr = Escape(hPrinterDC, NEXTBAND, 0, NULL,
					(LPSTR)&rPrintBand)) <0)
				{
					goto EXIT;
				}
				bNextBandReached = TRUE;
			}
		}

		if ( IsRectEmpty( &rPrintBand ) )
			break;

		/* Compute the intersection of the band and the image area */
		IntersectRect( &rBand, &rPrintBand, &rImage );

		Info.fText = NO;
		Info.fGraphics = YES;

		if ( BandInfoDevice )
		{
			CopyRect( &Info.rBand, &rBand );
			if ( Escape(hPrinterDC, BANDINFO, 0, (LPSTR)&Info,
				(LPSTR)&Info) < 0 )
			{
				Info.fText = NO;
				Info.fGraphics = YES;
			}
		}

		if ( !Info.fGraphics )
			continue;

		xDest  = rBand.left;
		yDest  = rBand.top;
		dxDest = RectWidth( &rBand );
		dyDest = RectHeight( &rBand );

		if (EjectPage)
		{
			// Only print funriture on single image output
			OutputFurniture( hPrinterDC, &rPrintBand, &rImage,
				iPrResX, iPrResY, SepList[iSep] );
		}

		/* If there's no image data to output, go get another source band */
		if ( dxDest <= 1 || dyDest <= 1 )
			continue;

		/* Compute the destination slice height */
		dyDestSlice = dyDest;

		/* Compute where we should get the imagery from */
		xSrc  = Edit.Crop.left + FMUL( (xDest - rImage.left), xScale );
		dxSrc = FMUL( dxDest, xScale );

		while ( !fAbortPrint )
		{
			/* Check for a user cancellation */
			(*lpAbortTest)(hPrinterDC, 0);

			/* If don't have a whole Destination slice left, reduce it */
			if ( dyDest < dyDestSlice )
				dyDestSlice = dyDest;

			/* Compute where we should get the imagery from */
			ySrc  = Edit.Crop.top + FMUL( (yDest - rImage.top), yScale );
			dySrc = FMUL( dyDestSlice, yScale );

			if ( bSuperBlt )
			{
				SetRect( &rSource, xSrc, ySrc,
					xSrc + dxSrc - 1, ySrc + dySrc - 1 );

				SetRect( &rDest, xDest, yDest,
					xDest + dxDest - 1, yDest + dyDestSlice - 1 );

				if (SepList[iSep] == 'X')
					nTotalBits = 8; // print image as gray
				else
					nTotalBits = 0; // print image as is

#ifdef _MAC		// a hack to get rid of a black line at the right edge
				rDest.right -= 4;
#endif				
				SuperLock( YES );
				PrintImage( hPrinterDC, NO, &rSource, &rDest,
					xDiva, yDiva, (yDest - rImage.top), dyOutput,
					nTotalBits,
					lpImage, 
					lpFrame);
				SuperLock( NO );
			}
			else
			{
				if ( !lstrcmp( "LaserPort", szTech ) )
				{
					AstralLaserPrint(
						lpImage,
						lpFrame,
						xSrc, ySrc, dxSrc, dySrc, xDest, yDest,
						dxDest, dyDest, iPrResX, iPrResY );
				}
				else
				{
					PSPrint(
						lpImage,
						lpFrame,
						SepList[iSep],
						xSrc, ySrc, dxSrc, dySrc, xDest, yDest,
						dxDest, dyDest, iPrResX, iPrResY );
				}
			}

			/* Adjust the destination start location */
			yDest += dyDestSlice;

			/* Adjust the destination lines left */
			/* If there are no lines left, go get another band */
			if ( ( dyDest -= dyDestSlice ) <= 0 )
				break;
		}
	}
	while ( BandingDevice && EjectPage );

EXIT:

	MessageStatus( IDS_PLEASEWAIT );

	if ( iEscErr < 0 || fAbortPrint )
	{
		/* Remove document from spooler. */
		if ( iEscErr >= 0 )
		{
			// If its not a real print error
			if (EjectPage)
			{
#ifdef NOTWIN31
				Escape(hPrinterDC, ABORTDOC, 0, NULL, 0L);
#else // WIN3.1
				AbortDoc( hPrinterDC );
#endif // WIN3.1
			}
		}
	}
	else
	{
		/* Sent all of page without problems; eject it. */
		if (EjectPage)
		{
#ifdef NOTWIN31
			if ( !BandingDevice )
			{
				Escape(hPrinterDC, NEWFRAME, 0, NULL, 0L);
			}
#else // WIN3.1
			EndPage( hPrinterDC );
#endif // WIN3.1
		}

		if ( SepList[++iSep] )
			goto StartPage;

		if (EjectPage)
		{
#ifdef NOTWIN31
			Escape(hPrinterDC, ENDDOC, 0, NULL, 0L);
#else // WIN3.1
			EndDoc( hPrinterDC );
#endif // WIN3.1
		}
	}

	if (EjectPage)
	{
		if ( hWndAbort )
		{
			/* abort dialog box still up, so bring it down  */
			EnableWindow( hWnd, TRUE );
//			RxOnDestroyDialog( hWndAbort );
			DestroyWindow( hWndAbort );
			hWndAbort = NULL;
		}

		FreeProcInstance( (FARPROC)lpDlgFnAbort );
		FreeProcInstance( (FARPROC)lpAbortTest );

		DeleteDC( hPrinterDC );
		hPrinterDC = NULL;
	}

	Page = OldPage;

	MessageStatus( NULL );
	AstralCursor( NULL );

	if (EjectPage)
	{
		if ( fAbortPrint )
			PrintErr(hWnd, SP_USERABORT);
		else if ( iEscErr < 0 )
			PrintErr(hWnd, iEscErr);
	}

	return( (iEscErr >= 0) && !fAbortPrint );
}

/***************************************************************************/

void OutputFurniture(
	HDC 	hDC,
	LPRECT 	lpBandRect,
	LPRECT	lpRect,
	int 	xRes,
	int		yRes,
	BYTE 	cSep)
{
	RECT Rect, rDummy;
	int i, dx, dy, value, quarter, eighth, iPenWidth, iHeight;
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	COLOR Color;
	LPSTR lpString;
	STRING sz, szType;
	char sz0[16], sz1[16], sz2[16], sz3[16];
	TEXTMETRIC tm;
	long ltime;

	if ( Page.OutputType == IDC_PRINT_COLORSEPS )
	{
		switch ( cSep )
		{
			case 'C': Color = RGB(0,255,255); break;
			case 'M': Color = RGB(255,0,255); break;
			case 'Y': Color = RGB(255,255,0); break;
			case 'K': Color = RGB(0,0,0); break;
			case 'X': Color = RGB(0,0,0); break;
		}
	}
	else
	{
		Color = RGB(0,0,0);
	}

	if ( !(iPenWidth = ((4L * max(xRes,yRes)) + 500L) / 1000L) )
		iPenWidth = 1;

	GetTextMetrics( hDC, &tm );
	dy = tm.tmHeight /*(tmAscent+tmDescent)*/ +
		tm.tmInternalLeading + 4*iPenWidth;

	hPen = CreatePen( PS_SOLID, iPenWidth, Color );
	hOldPen = (HPEN)SelectObject( hDC, hPen );
	hOldBrush = (HBRUSH)SelectObject( hDC, GetStockObject(WHITE_BRUSH) );

	quarter = (xRes+2)/4; // 1/4 inch
	eighth  = quarter/2;  // 1/8 inch

	if ( Page.RegMarks || Page.TrimMarks || Page.Labels )
	{
		Rect.left   = lpRect->left + eighth;
		Rect.right  = lpRect->right;
		Rect.top    = lpRect->top - eighth - quarter - dy;
		Rect.bottom = lpRect->top - eighth - quarter;

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
			if ( cSep != 'C' && cSep != 'M' && cSep != 'Y' && cSep != 'K' )
				PrintLabel( hDC, Rect.left, Rect.top, Color, Page.OutputType );

			dx = 0;
			if ( cSep == 'C' )
				PrintLabel( hDC, Rect.left+dx, Rect.top, Color, IDS_LABELSEPC );

			if ( AstralStr( IDS_LABELSEPC, &lpString ) )
			{
				SIZE tx;
				GetTextExtentPoint( hDC, lpString, lstrlen(lpString), &tx );
				dx += tx.cx;
				dx += eighth;
			}

			if ( cSep == 'M' )
				PrintLabel( hDC, Rect.left+dx, Rect.top, Color, IDS_LABELSEPM );

			if ( AstralStr( IDS_LABELSEPM, &lpString ) )
			{
				SIZE tx;
				GetTextExtentPoint( hDC, lpString, lstrlen(lpString), &tx );
				dx += tx.cx;
				dx += eighth;
			}

			if ( cSep == 'Y' )
				PrintLabel( hDC, Rect.left+dx, Rect.top, Color,	IDS_LABELSEPY );

			if ( AstralStr( IDS_LABELSEPY, &lpString ) )
			{
				SIZE tx;
				GetTextExtentPoint( hDC, lpString, lstrlen(lpString), &tx );
				dx += tx.cx;
				dx += eighth;
			}

			if ( cSep == 'K' )
				PrintLabel( hDC, Rect.left+dx, Rect.top, Color, IDS_LABELSEPK );
		}
	}

	if ( Page.TrimMarks )
	{
		// top, left
		Rect.left   = lpRect->left - eighth - quarter;
		Rect.right  = lpRect->left;
		Rect.top    = lpRect->top  - eighth - quarter;
		Rect.bottom = lpRect->top;

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
			MoveToEx( hDC, Rect.left, Rect.bottom, NULL );
			LineTo( hDC, Rect.right - eighth, Rect.bottom );
			MoveToEx( hDC, Rect.right, Rect.top, NULL );
			LineTo( hDC, Rect.right, Rect.bottom - eighth );
		}

		// top, right
		Rect.left   = lpRect->right;
		Rect.right  = lpRect->right + eighth + quarter;
		Rect.top    = lpRect->top   - eighth - quarter;
		Rect.bottom = lpRect->top;

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
			MoveToEx( hDC, Rect.right, Rect.bottom, NULL );
			LineTo( hDC, Rect.left + eighth, Rect.bottom );
			MoveToEx( hDC, Rect.left, Rect.top, NULL );
			LineTo( hDC, Rect.left, Rect.bottom - eighth );
		}

		// bottom, left
		Rect.left   = lpRect->left   - eighth - quarter;
		Rect.right  = lpRect->left;
		Rect.top    = lpRect->bottom;
		Rect.bottom = lpRect->bottom + eighth + quarter;

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
			MoveToEx( hDC, Rect.left, Rect.top, NULL );
			LineTo( hDC, Rect.right - eighth, Rect.top );
			MoveToEx( hDC, Rect.right, Rect.bottom, NULL );
			LineTo( hDC, Rect.right, Rect.top + eighth );
		}

		// right, bottom
		Rect.left   = lpRect->right;
		Rect.right  = lpRect->right  + eighth + quarter;
		Rect.top    = lpRect->bottom;
		Rect.bottom = lpRect->bottom + eighth + quarter;

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
			MoveToEx( hDC, Rect.right, Rect.top, NULL );
			LineTo( hDC, Rect.left + eighth, Rect.top );
			MoveToEx( hDC, Rect.left, Rect.bottom, NULL );
			LineTo( hDC, Rect.left, Rect.top + eighth );
		}
	}

	if ( Page.RegMarks )
	{
		// top
		Rect.left   = (lpRect->left + lpRect->right)/2 - quarter/2;
		Rect.right  = Rect.left + quarter;
		Rect.top    = lpRect->top - eighth - quarter;
		Rect.bottom = lpRect->top - eighth;

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
			OutputRegMark( hDC, &Rect );

		// bottom
		Rect.left   = (lpRect->left + lpRect->right)/2 - quarter/2;
		Rect.right  = Rect.left + quarter;
		Rect.top    = lpRect->bottom + eighth;
		Rect.bottom = lpRect->bottom + eighth + quarter;

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
			OutputRegMark( hDC, &Rect );

		// left
		Rect.left   = lpRect->left - eighth - quarter;
		Rect.right  = lpRect->left - eighth;
		Rect.top    = (lpRect->top + lpRect->bottom)/2 - quarter/2;
		Rect.bottom = Rect.top + quarter;

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
			OutputRegMark( hDC, &Rect );

		// right
		Rect.left   = lpRect->right + eighth;
		Rect.right  = lpRect->right + eighth + quarter;
		Rect.top    = (lpRect->top + lpRect->bottom)/2 - quarter/2;
		Rect.bottom = Rect.top + quarter;

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
			OutputRegMark( hDC, &Rect );
	}

	if ( Page.Labels )
	{
		Rect.left   =  4 * quarter; // 1 inch
		Rect.right  = 34 * quarter; // 8.5 inches
		Rect.top    = quarter;      // .25 inches
		Rect.bottom =  9 * quarter; // 2.25 inches

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
			time(&ltime);
			if (lpImage)
				PrintLabel( hDC, Rect.left, Rect.top, Color,
						IDS_LABELPICNAME, lpImage->CurFile,
						(LPTR)ctime(&ltime) );

			Rect.top += dy;

			if ( AstralStr( Page.Type, &lpString ) )
				lstrcpy( szType, lpString );
			else
				lstrcpy( szType, "???" );

			DeviceDescription( Names.Printer, sz );
			PrintLabel( hDC, Rect.left, Rect.top, Color,
				IDS_LABELPRINTER, (LPSTR)szType, (LPSTR)sz );
			Rect.top += dy;
			PrintLabel( hDC, Rect.left, Rect.top, Color,
				IDS_LABELSTYLE, (LPTR)Names.PrintStyle );
			Rect.top += dy;
			if ( !Calibrate.DoCalibration )
			{
				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELNOCALIBRATION );
				Rect.top += dy;
			}
			else
			{
				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELCALIBRATION,
				(LPTR)PrintStyle.CalibrateName );
				Rect.top += dy;
			}

			if ( !Halftone.DoHalftoning )
			{
				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELNOHALFTONING );
				Rect.top += dy;
			}
			else
			{
				if ( AstralStr( Halftone.DotShape, &lpString ) )
					lstrcpy( szType, lpString );
				else
					lstrcpy( szType, "???" );

				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELMINMAXDOT, (LPTR)szType,
					(int)TOPERCENT(Halftone.Hilight),
					(int)TOPERCENT(Halftone.Shadow) );
				Rect.top += dy;
				if ( cSep )
				{
					i = ( cSep == 'C' ? 0 :
						( cSep == 'M' ? 1 :
						( cSep == 'Y' ? 2 :
						( cSep == 'K' ? 3 : 3 ) ) ) );
					FixedAscii( Halftone.ScreenRuling[i], sz0, -1 );
					FixedAscii( Halftone.ScreenAngle[i], sz1, -1 );
					PrintLabel( hDC, Rect.left, Rect.top, Color,
						IDS_LABELSINGLEHALFTONE,
						(LPTR)sz0, (LPTR)sz1 );
					Rect.top += dy;
				}
				else
				{
					FixedAscii( Halftone.ScreenAngle[0], sz0, -1 );
					FixedAscii( Halftone.ScreenAngle[1], sz1, -1 );
					FixedAscii( Halftone.ScreenAngle[2], sz2, -1 );
					FixedAscii( Halftone.ScreenAngle[3], sz3, -1 );
					PrintLabel( hDC, Rect.left, Rect.top, Color,
						IDS_LABELCOLORANGLE,
						(LPTR)sz0, (LPTR)sz1,
						(LPTR)sz2, (LPTR)sz3 );
					Rect.top += dy;
					FixedAscii( Halftone.ScreenRuling[0], sz0, -1 );
					FixedAscii( Halftone.ScreenRuling[1], sz1, -1 );
					FixedAscii( Halftone.ScreenRuling[2], sz2, -1 );
					FixedAscii( Halftone.ScreenRuling[3], sz3, -1 );
					PrintLabel( hDC, Rect.left, Rect.top, Color,
						IDS_LABELCOLORFREQUENCY,
						(LPTR)sz0, (LPTR)sz1,
						(LPTR)sz2, (LPTR)sz3 );
					Rect.top += dy;
				}
			}

			if ( Page.OutputType != IDC_PRINT_GRAY )
			{
				if ( !Separate.DoBlackAndUCR )
				{
					PrintLabel( hDC, Rect.left, Rect.top, Color,
						IDS_LABELNOBLACKGEN );
					Rect.top += dy;
				}
				else
				{
					PrintLabel( hDC, Rect.left, Rect.top, Color,
						IDS_LABELBLACKGEN,
						(LPTR)Separate.BGName );
					Rect.top += dy;
					PrintLabel( hDC, Rect.left, Rect.top, Color,
						IDS_LABELUCR,
					Separate.UCR, 100-Separate.UCR );
					Rect.top += dy;
				}

				if ( !Separate.DoInkCorrect )
				{
					PrintLabel( hDC, Rect.left, Rect.top, Color,
						IDS_LABELNOINKCORRECT );
					Rect.top += dy;
				}
				else
				{
					PrintLabel( hDC, Rect.left, Rect.top, Color,
						IDS_LABELRINKCORRECT,
					bound(100+Separate.MfromR,0,100),
					bound(100-Separate.MfromR,0,100) );
					Rect.top += dy;
					PrintLabel( hDC, Rect.left, Rect.top, Color,
						IDS_LABELGINKCORRECT,
						bound(100+Separate.YfromG,0,100),
						bound(100-Separate.YfromG,0,100) );
					Rect.top += dy;
					PrintLabel( hDC, Rect.left, Rect.top, Color,
						IDS_LABELBINKCORRECT,
						bound(100+Separate.CfromB,0,100),
						bound(100-Separate.CfromB,0,100) );
					Rect.top += dy;
				}
			}
		}
	}

	if ( Page.StepScale )
	{
		iHeight = 11 * ( quarter + iPenWidth );
		if ( cSep == 'K' )
		{
			Rect.left  = lpRect->right + eighth + quarter + eighth;
			Rect.right = lpRect->right + eighth + quarter + eighth + quarter;
		}
		else
		{
			Rect.left  = lpRect->left  - eighth - quarter - eighth - quarter;
			Rect.right = lpRect->left  - eighth - quarter - eighth;
		}
		Rect.top = (lpRect->top + lpRect->bottom - iHeight )/2;

		for ( i=0; i<11; i++ )
		{
			Rect.bottom = Rect.top + quarter;
			if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
			{
				value = (255*i)/10;
				if ( Page.OutputType == IDC_PRINT_COLORSEPS )
				{
					switch ( cSep )
					{
						case 'C': Color = RGB(value,255,255); break;
						case 'M': Color = RGB(255,value,255); break;
						case 'Y': Color = RGB(255,255,value); break;
						case 'K': Color = RGB(value,value,value); break;
						case 'X': Color = RGB(value,value,value); break;
					}
				}
				else
				{
					Color = RGB(value,value,value);
				}
				FillRect( hDC, &Rect, hBrush = CreateSolidBrush(Color));
				DeleteObject( hBrush );
			}
			Rect.top = Rect.bottom + iPenWidth;
		}
	}

	SelectObject( hDC, hOldBrush );
	SelectObject( hDC, hOldPen );
	DeleteObject( hPen );
}

/***************************************************************************/

int PrintLabel( HDC hDC, int x, int y, COLOR Color, int idStr, ... )
{
	char szBuffer[1024];
	LPTR lpArguments;
	LPSTR lpString;
	int  retc, length;

	/* The caller passes stringtable ID's, and not hardcoded strings */
	/* Used to put a variety of labels on the printed page */
	if ( !AstralStr( idStr, &lpString ) )
		return( IDCANCEL );

	lpArguments = (LPTR)&idStr + sizeof(idStr);
	length = wvsprintf( szBuffer, lpString, (LPSTR)lpArguments );

	ColorText( hDC, x, y, szBuffer, length, Color );

	return( retc );
}

/***************************************************************************/

void OutputRegMark(
	HDC    hDC,
	LPRECT lpRect)
{
	RECT Rect;
	int xsize, ysize;

	Rect  = *lpRect;
	xsize = RectWidth(lpRect);
	ysize = RectHeight(lpRect);

	InflateRect( &Rect, -xsize/12, -ysize/12 );

	Ellipse( hDC, Rect.left, Rect.top, Rect.right, Rect.bottom );

	InflateRect( &Rect, xsize/12, ysize/12 );

	MoveToEx( hDC, Rect.left, (Rect.top + Rect.bottom)/2, NULL );
	LineTo( hDC, Rect.right, (Rect.top + Rect.bottom)/2 );
	MoveToEx( hDC, (Rect.left + Rect.right)/2, Rect.top, NULL );
	LineTo( hDC, (Rect.left + Rect.right)/2, Rect.bottom );

	InflateRect( &Rect, -xsize/12, -ysize/12 );

	xsize = RectWidth(lpRect);
	ysize = RectHeight(lpRect);

	InflateRect( &Rect, -xsize/5, -ysize/5 );

	Ellipse( hDC, Rect.left, Rect.top, Rect.right, Rect.bottom );

	MoveToEx( hDC, Rect.left, (Rect.top + Rect.bottom)/2, NULL );
	LineTo( hDC, Rect.right, (Rect.top + Rect.bottom)/2 );
	MoveToEx( hDC, (Rect.left + Rect.right)/2, Rect.top, NULL );
	LineTo( hDC, (Rect.left + Rect.right)/2, Rect.bottom );
}

/***********************************************************************\

	ABORTTEST is passed to gdi to allow the spooler to call it during
	spooling to let the application cancel the print job or to handle
	out-of-disk-space conditions.  All it does is look in the message
	queue and see if the next message is for the abort dialog box, and
	if it is it sends it to the dialog box, and if it is not it is
	translated and dispatched like normal.

\***********************************************************************/

BOOL CALLBACK EXPORT AbortTest(
	HDC hDC,
	int iCode)
{
	MSG msg;

	while ( !fAbortPrint && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
	{
		if ( !hWndAbort || !IsDialogMessage(hWndAbort, &msg) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	/* fAbortPrint is TRUE (return is FALSE) if the user has aborted */
	return( !fAbortPrint );
}

/***************************************************************************\

	DLGFNABORT controls the modeless Abort dialog box.  It is called
	whenever a print job is sent to the spooler.

\***************************************************************************/

BOOL WINPROC EXPORT DlgFnAbort (
	HWND   hDlg,
	UINT   msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
			SET_CLASS_HBRBACKGROUND(hDlg, ( HBRUSH )GetStockObject(LTGRAY_BRUSH));
			SetWindowText( hDlg, szAppName);
			SetDlgItemText(hDlg, ID_NAME,    szFilename);
			SetDlgItemText(hDlg, ID_PRINTER, (LPSTR)lpchDevice);
			SetDlgItemText(hDlg, ID_PORT,    (LPSTR)lpchPort);
			CenterPopup(hDlg);
		break;

		case WM_SETCURSOR:
			return( SetupCursor( wParam, lParam, IDD_ABORT ) );

		case WM_CTLCOLOR:
			return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
				HIWORD(lParam) ) );

		case WM_COMMAND:
			switch (wParam)
			{
				case ID_FILENAME :
					SetDlgItemText(hDlg, ID_NAME, (LPSTR)lParam);
				break;

				case IDCANCEL:
					fAbortPrint = TRUE;
				break;

				default:
					return( FALSE );
				break;
			}
		break;

		default:
			return( FALSE );
		break;
	}

	return( TRUE );
}

/***************************************************************************\

	GETPRINTERDC reads the device string line from WIN.INI and trys to
	create a DC for  the device.  It rejects all DCs incapable of BITBLT. 
	It returns a DC or NULL if there is an error.

\***************************************************************************/

HDC GetPrinterDC()
{
#ifdef _MAC
	return CreateDC("PRINTER", NULL, NULL, NULL);
#else
	LPSTR lpch;
	HDC hDC;

	if ( !Names.Printer[0] )
		GetDefaultPrinter( Names.Printer );

	lstrcpy( szPrinter, Names.Printer );
	lpch = szPrinter;
	lpchDevice = (LPTR)lpch;
	lpchDriver = (LPTR)lpch;
	lpchPort   = (LPTR)lpch;

	if ( !(*lpch) )
	{
		Message( IDS_EBADDEVICE );
		return( NULL );
	}

	/* skip leading blanks */
	while ( *lpch && *lpch == ' ' )
		lpch = (LPSTR) AnsiNext(lpch);

	lpchDevice = (LPTR)lpch;

	/* find end of device name */
	while ( *lpch && *lpch != ',' )
		lpch = (LPSTR) AnsiNext(lpch);

	/* if there is no device, we can't output */
	if ( !(*lpch) )
	{
		Message( IDS_EBADDEVICE );
		return( NULL );
	}

	/* null terminate driver */
	*lpch++ = NULL;

	/* skip leading blanks */
	while ( *lpch && *lpch == ' ' )
	{
		if (*lpch == '.')
			*lpch = NULL;
		lpch = (LPSTR) AnsiNext(lpch);
	}

	lpchDriver = (LPTR)lpch;

	/* find end of driver name */
	while ( *lpch && *lpch != ',' )
		lpch = (LPSTR) AnsiNext(lpch);

	/* if the is no driver for device, we can't output */
	if ( !(*lpch) )
	{
		Message( IDS_EBADDEVICE );
		return( NULL );
	}

	/* null terminate device */
	*lpch++ = NULL;

	/* skip leading blanks */
	while ( *lpch && *lpch == ' ' )
		lpch = (LPSTR) AnsiNext(lpch);

	lpchPort = (LPTR)lpch;

	/* Create the DC. */
	if ( hDC = CreateDC( (LPSTR)lpchDriver, (LPSTR)lpchDevice, (LPSTR)lpchPort, NULL ) )
	{
		/* if the DC cannot do DIBTODEV (BITBLT) ... */
		if ( !(GetDeviceCaps(hDC, RASTERCAPS) & RC_BITBLT) )
		{
			DeleteDC(hDC);
			hDC = NULL;
		}
	}

	return( hDC );
#endif // _MAC
}

/***************************************************************************/

void PrintErr(
	HWND hWnd,
	int  ierr)
{
	int ids;

	switch (ierr)
	{
		case SP_OUTOFDISK:
			ids = IDS_PRINTDISK;
		break;

		case SP_OUTOFMEMORY:
			ids = IDS_PRINTMEM;
		break;

		case SP_APPABORT:
		case SP_USERABORT:
			ids = IDS_PRINTUSER;
		break;

		case SP_ERROR:
		default:
			ids = IDS_PRINTERR;
		break;
	}

	Message( ids );
}

/***************************************************************************/

typedef struct _laser
{
	int  nCount;
	char cData[1];
}
LASER, far *LPLASER;

/***************************************************************************/
//	Set lpImage to NULL to use lpFrame.
/***************************************************************************/

BOOL AstralLaserPrint(
	LPIMAGE lpImage,
	LPFRAME lpFrame,
	int xSrc,
	int ySrc,
	int dxSrc,
	int dySrc,
	int xDest,
	int yDest,
	int dxDest,
	int dyDest,
	int iPrResX,
	int iPrResY )
{
	/* For the laser board... */
	int y, yline, ystart, ylast;
	LFIXED yrate, yoffset;
	LPTR lpBits;
	LPLASER lpLaser;
	int iErr, start;
	LPFRAME lpBaseFrame;
	
	ProgressBegin(1,0);
	
	if (lpImage)
		lpBaseFrame = ImgGetBaseEditFrame(lpImage);
	else
		lpBaseFrame = lpFrame;
	/* Send the style (1=line, 2=diam), freq, angle(0,45,90) */
	if ( !LaserPrint( "\033$s%ds%df%dA", 2,
   	HIWORD(Halftone.ScreenRuling[0]), HIWORD(Halftone.ScreenAngle[0]) ) )
		goto ExitFalse;

	/* Send the destination (x,y) and size (w,h) 1/1200" */
	if ( !LaserPrint( "\033$d%dx%dy%dw%dH",
		LX(xDest),  LY(yDest), LX(dxDest), LY(dyDest) ) )
		goto ExitFalse;

	/* Let the printer do any upsizing */
	if ( dySrc < dyDest )
	{
		yrate  = UNITY;
		dxDest = dxSrc;
		dyDest = dySrc;
	}
	else
	{
		yrate = FGET( dySrc, dyDest );
	}

	/* Send the inline image's size and packing (bpp,ppb,w,h) */
	if ( !LaserPrint( "\033$i%db%dp%dw%dH", 8, 1, dxDest, dyDest ) )
		goto ExitFalse;

	lpLaser = (LPLASER)LineBuffer[0];
	start   = wsprintf( lpLaser->cData, "\033$i%dT", dxDest /*1 for dptek*/ );
	lpLaser->nCount = start + dxDest;
	lpBits  = (LPTR)&(lpLaser->cData[start]);

	ystart  = ySrc;
	yline   = -1;
	yoffset = (long)yrate>>1;

	for (y=0; y<dyDest; y++)
	{
		if (AstralClockCursor( y, dyDest, YES ))
		{
			fAbortPrint = YES;
			break;
		}

		/* Check for user input to abort dialog box */
		(*lpAbortTest)(hPrinterDC, 0);
		if ( fAbortPrint )
			break;

		ylast = yline;
#ifdef WIN32
		yline = ystart + WHOLE( yoffset );
#else
		yline = ystart + HIWORD( yoffset );
#endif
		yoffset += yrate;
		if ( yline != ylast )
		{
			LFIXED rate;
			LPTR lpSrcData = FramePointer(lpBaseFrame, xSrc, yline, NO);

			rate = FGET( dxSrc, dxDest );

			FrameSample(
				lpBaseFrame,
				lpSrcData,
				0,
				lpBits,
				0,
				dxDest,
				rate);

			CorrectGray( lpBits, dxDest, YES, NO );
		}

		/* Send raw data to the Intel or DPTek grayscale board */
		if ( (iErr = Escape( hPrinterDC, DEVICEDATA, lpLaser->nCount,
			(LPSTR)lpLaser, 0 )) < 0 )
			goto ExitFalse;
	}

	ProgressEnd();
	return( TRUE );

ExitFalse:

	ProgressEnd();
	return( FALSE );
}

/***************************************************************************/

BOOL LaserPrint( LPSTR lpFormat, ... )
{
	LPTR lpArguments;
	LPLASER lpLaser;
	int iErr;

	lpLaser = (LPLASER)LineBuffer[0];

	/* Send short escape sequences to the Intel or DPTek grayscale */
	/* board with arguments specified exactly like printf */
	lpArguments = (LPTR)&lpFormat + sizeof(lpFormat);
	lpLaser->nCount = wvsprintf( lpLaser->cData, (LPSTR)lpFormat, (LPSTR)lpArguments );
	iErr = Escape( hPrinterDC, DEVICEDATA, lpLaser->nCount, (LPSTR)lpLaser, 0 );

	return( iErr >= 0 );
}

/***************************************************************************/

BOOL IsPostScript()
{
	HDC    hPrinterDC;     /* DC of the printer */
	STRING szTech;

	if ( !(hPrinterDC = GetPrinterDC()) )
		return( FALSE );

	/* Get the technology parameters */
	/* The 1st technology parameter is either "PostScript" or "PCL" */
	if ( Escape(hPrinterDC, GETTECHNOLGY, NULL, NULL, szTech) < 0 )
		szTech[0] = '\0';

	DeleteDC( hPrinterDC );

	if ( lstrcmp( "PostScript", szTech ) )
		return(FALSE);

	return(TRUE);
}

/***************************************************************************/

static BOOL bSuperLock;
static RECT rSuperPaint;

/***************************************************************************/

void SuperLock( BOOL Bool )
{
	if ( !(bSuperLock = Bool) )
	{
		if ( !IsRectEmpty( &rSuperPaint ) )
			InvalidateRect( NULL, NULL, FALSE );
	}
	SetRectEmpty( &rSuperPaint );
}

/***************************************************************************/

BOOL SuperPaint( HWND hWnd )
{
	PAINTSTRUCT ps;

	if ( !bSuperLock )
		return( NO );

	if ( BeginPaint( hWnd, &ps ) )
	{
		UnionRect( &rSuperPaint, &rSuperPaint, &ps.rcPaint );
		EndPaint( hWnd, &ps );
	}
	return( YES );
}

/***************************************************************************/
