//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

/************************************************************************\
	Print.c   print functions for astral's picture publisher
\************************************************************************/

#include "pp.h"
#include "ppafx.h"
#include <time.h>
#include "id.h"
#include "basetype.h"

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

extern char     szAppName[];    /* calling application name */

static HWND     hWndAbort;      /* handle to modeless abort dialog box */
static FNAME    szFilename;     /* file to print. (no path) */
static STRING   szDocName;
static STRING   szPrinter;      /* printer device name (NAME,DRIVER,PORT) */
static LPSTR     lpchDevice,     /* pointers into the szPrinter string */
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

/******************************************************\
 This is a VERY minimal integer square root function.
\******************************************************/

int my_sqrt(int value)
{
	int i;

	i = 2;

	while((i*i) <= value ) {
		i++;
	}

	return( i-1 );
}

/***************************************************************************/

LOCAL void SetSepString(HWND hDlg, char sep)
{
	STRING szString;
	ITEMID idStr;

	if (sep == 'C')
		idStr = IDS_PRINTCYAN;
	else
	if (sep == 'M')
		idStr = IDS_PRINTMAGENTA;
	else
	if (sep == 'Y')
		idStr = IDS_PRINTYELLOW;
	else
	if (sep == 'K')
		idStr = IDS_PRINTBLACK;
	else
	if (sep == 'X')
		idStr = IDS_PRINTGRAY;
	else
		idStr = IDS_PRINTCOLOR;
	AstralStrEx(idStr, szString, sizeof(szString));
	SetDlgItemText(hDlg, ID_OUTPUTTYPE, szString);
}

/***************************************************************************/

int PrintLabelCentered( HDC hDC, int x, int y, COLOR Color, int idStr, ... )
{
	char     szBuffer[1024];
	va_list  lpArguments;
	LPSTR     lpString;
	int      retc, length;
	SIZE     size;
	int      dx, dy;

	/* The caller passes stringtable ID's, and not hardcoded strings */
	/* Used to put a variety of labels on the printed page */
	if ( !AstralStr( idStr, &lpString ) )
		return( IDCANCEL );

   va_start( lpArguments, idStr );
	length = wvsprintf( szBuffer, lpString, lpArguments );
   va_end( lpArguments );

	GetTextExtentPoint( hDC, lpString, length, &size );

	dx = size.cx;
	dy = size.cy;

	ColorText( hDC, x-(dx/2), y-(dy/2), szBuffer, length, Color );

	return( retc );
}

/***********************************************************************\

	PrintTest uses PrintFile defined below to print an NxM array of print
	style samples using the currently selected image.

\***********************************************************************/

int PrintTest( HWND hWnd, LPSTR szFile, LPIMAGE lpImage )
{
	int styleCount, curStyle, baseStyle;
	BOOL aborted = FALSE;
	RECT rRect;

	int curColumn, curRow;
	int pgColumns, pgRows;

	int edWidth, edHeight;
	LFIXED scWidth, scHeight;
	LFIXED dxImage, dyImage;
	LFIXED PageWidth, PageHeight;
	int dxPrinter, dyPrinter;

	static STRING szStyle;

	PAGE oldPage;
	EDIT oldEdit;

	int iPrResX, iPrResY;

	HPEN hPen, hOldPen;
	HBRUSH hOldBrush;
	TEXTMETRIC tm;
	int dy;

	oldPage = Page;
	oldEdit = Edit;

	AstralCursor( IDC_WAIT );

	/* get the printer of the current printer */
	if ( !(hPrinterDC = GetPrinterDC()) )
		return( FALSE );

	ProgressBegin(1,PROGRESS_ID(IDS_UNDOPRINTING));

	hPen = CreatePen( PS_SOLID, 1, RGB(0, 0, 0) );
	hOldPen = (HPEN)SelectObject( hPrinterDC, hPen );
	hOldBrush = (HBRUSH)SelectObject( hPrinterDC, GetStockObject(WHITE_BRUSH) );

	/* Get the text metric information */
	GetTextMetrics( hPrinterDC, &tm );
	dy = tm.tmHeight + tm.tmInternalLeading + 4;

	/* Determine the banding capabilities of the printer driver */
	BandingDevice   = GetDeviceCaps(hPrinterDC, RASTERCAPS) & RC_BANDING;
	bNextBandReached = FALSE;
	BandInfoDevice = BANDINFO;
	BandInfoDevice = Escape(hPrinterDC, QUERYESCSUPPORT, 2,(LPSTR)&BandInfoDevice, NULL);

	hWndAbort = NULL;
	fAbortPrint = FALSE;

	/* Send document name to spooler */
	lstrcpy( szFilename, szFile );
	lstrcpy( szDocName, szAppName );
	lstrcat( szDocName, " " );
	lstrcat( szDocName, szFilename );

	PrinterDocInfo.cbSize = sizeof(DOCINFO);
	PrinterDocInfo.lpszDocName = szDocName;
	PrinterDocInfo.lpszOutput  = NULL;      

	/* Set up abort dialog box */
	EnableWindow( hWnd, FALSE );
	GetClientRect( PictPubApp.Get_hWndAstral(), &rRect );
	InflateRect( &rRect, 1, 1 );

	hWndAbort = AstralDlg(YES, PictPubApp.GetResourceHandle(), hWnd, 
						IDD_ABORT, DlgPrintAbortProc);

	/* Give abort procedure to gdi to check abort print command */
	lpAbortTest = (ABORTPROC)MakeProcInstance((FARPROC) AbortTest, PictPubApp.Get_hInstAstral() );

	if ((iEscErr = SetAbortProc( hPrinterDC, lpAbortTest )) < 0)
	{
		aborted = FALSE;
		goto test_done;
	}

	/* Use the device resolution (in pixels per inch) to compute */
	/* how big the image will be on the output device */
	iPrResX = GetDeviceCaps(hPrinterDC, LOGPIXELSX);
	iPrResY = GetDeviceCaps(hPrinterDC, LOGPIXELSY);

	/* Get size of printer page */
	PageWidth  = FGET(GetDeviceCaps(hPrinterDC, HORZRES), iPrResX);
	PageHeight = FGET(GetDeviceCaps(hPrinterDC, VERTRES), iPrResY);

	dxPrinter = GetDeviceCaps( hPrinterDC, HORZRES );
	dyPrinter = GetDeviceCaps( hPrinterDC, VERTRES );

	if( !BandingDevice ) {
		/* Create a single band for the entire page */
		rPrintBand.top    = 0;
		rPrintBand.left   = 0;
		rPrintBand.bottom = dyPrinter-1;
		rPrintBand.right  = dxPrinter-1;
	}

	/* Find out the number of Rows and Columns of tiled images */
	styleCount = GetExtNameCount( IDN_STYLE );

	if (styleCount <= 0) {
		aborted = TRUE;
		goto test_done;         
	}

	// want at most 9 per page
	curStyle = Min(9, styleCount);
	
	if (PageWidth < PageHeight) {
		pgColumns = my_sqrt( curStyle );
		pgRows    = curStyle/ pgColumns;
	
		while ((pgColumns * pgRows) < curStyle) pgRows++;
	} else {
		pgRows    = my_sqrt( curStyle);
		pgColumns = curStyle / pgRows;
	
		while ((pgColumns * pgRows) < curStyle) pgColumns++;
	}
	
	/* Scale the image to fit the tile size */
	scWidth  = (PageWidth  / pgColumns);
	scHeight = (PageHeight / pgRows);

	edWidth  = WHOLE((scWidth  -  8192L) * iPrResX);
	edHeight = WHOLE((scHeight - 20480L) * iPrResY);

	ScaleToFit( &edWidth, &edHeight, (int)FMUL( iPrResX, Edit.Width ),
					 (int)FMUL( iPrResY, Edit.Height ) );

	Edit.Width  = FGET(edWidth,  iPrResX);
	Edit.Height = FGET(edHeight, iPrResY);

	dxImage = Edit.Width;
	dyImage = Edit.Height;

	PrinterDocInfo.cbSize = sizeof(DOCINFO);
	PrinterDocInfo.lpszDocName = szDocName;
	PrinterDocInfo.lpszOutput  = NULL;	

	if ((iEscErr = StartDoc( hPrinterDC, &PrinterDocInfo )) < 0)
	{
		aborted = FALSE;
		goto test_done;
	}

	baseStyle = 0;
	// loop on pages
	do
	{	
		Page.OffsetX     = 0;
		Page.OffsetY     = 0;
		do
		{
			if ( BandingDevice )
			{
				/* Get the next band rectangle */
				if ((iEscErr = Escape(hPrinterDC, NEXTBAND, 
					0, NULL, (LPSTR)&rPrintBand)) <0)
				{
					goto test_done;
				}
				bNextBandReached = TRUE;
			}
		
			if ( IsRectEmpty( &rPrintBand ) )
				break;
		
			curStyle = baseStyle;
		
			/* Send the multiple print commands to the printer */
			for(curRow = 0; curRow < pgRows; curRow++ )
			{
				for(curColumn = 0; curColumn < pgColumns ; curColumn++)
				{
					if ((curStyle < styleCount) && !aborted)
					{
						if (AstralClockCursor(curStyle, styleCount, TRUE))
						{
							aborted = TRUE;
							break;  
						}
		
						AstralCursorEnable( FALSE );
		
						if (GetExtNameString(IDN_STYLE, curStyle, szStyle))
							{ // Load the print style for this tile
							LoadPrintStyle( szStyle );
							}
		
						/* Draw the Outline Box */
						rRect.left = rRect.right  = 
							WHOLE((scWidth * curColumn)  * iPrResX);
						rRect.top  = rRect.bottom =
							WHOLE((scHeight * curRow) * iPrResY);
		
						rRect.right  += WHOLE(scWidth  * iPrResX);
						rRect.bottom += WHOLE(scHeight * iPrResY);
		
						MoveTo( hPrinterDC, rRect.left,  rRect.top );
						LineTo( hPrinterDC, rRect.right, rRect.top );
						LineTo( hPrinterDC, rRect.right, rRect.bottom );
						LineTo( hPrinterDC, rRect.left,  rRect.bottom );
						LineTo( hPrinterDC, rRect.left,  rRect.top );
		
						/* Print out the style label */
						PrintLabelCentered(
							hPrinterDC, 
							(rRect.left + rRect.right)/2,
							rRect.top + (iPrResY/8),
							RGB(0, 0, 0), 
							IDS_LABELSTYLE, (LPSTR)szStyle );
		
						/* Move down the top */
						rRect.top += (iPrResY/4);
		
						/* Draw the bottom of the text box */
						MoveTo( hPrinterDC, rRect.left,  rRect.top );
						LineTo( hPrinterDC, rRect.right, rRect.top );
		
						/* Perform Our Page Setup */
						Page.Centered = FALSE;
		
						/* Compute image position on output device */
						Page.OffsetX = FGET((rRect.left+rRect.right)/2, iPrResX);
						Page.OffsetY = FGET((rRect.top+rRect.bottom)/2, iPrResY);
		
						Page.OffsetX -= (Edit.Width  / 2L);
						Page.OffsetY -= (Edit.Height / 2L);
		
						/* Print the individual Tiled image */
						if (!PrintFile( hWnd, szFile, NO, lpImage, NULL ))
						{
							aborted = TRUE;
						}
		
						AstralCursorEnable( TRUE );
					}
					curStyle++;
				}
			}
		}
		while( BandingDevice && (!aborted) );
	
test_done:
		if (aborted)
			break;
		else
			EndPage( hPrinterDC );

		baseStyle = curStyle;
		if (BandingDevice && (!aborted))
		{
			// to fix banding device bug
			EndDoc( hPrinterDC );
			if ((iEscErr = StartDoc( hPrinterDC, &PrinterDocInfo )) < 0)
			{
				aborted = TRUE;
				goto test_done;
			}
		}
	}while(!aborted && baseStyle<styleCount); 
				  
	if (aborted)
	{
		AbortDoc( hPrinterDC );
	}
	else
	{
		EndDoc( hPrinterDC );
	}

	EnableWindow( hWnd, TRUE );
	if ( hWndAbort )
	{
		/* abort dialog box still up, so bring it down  */
		AstralDlgEnd(hWndAbort, TRUE);
		hWndAbort = NULL;
	}

	SelectObject( hPrinterDC, hOldBrush );
	SelectObject( hPrinterDC, hOldPen );
	DeleteObject( hPen );

	DeleteDC( hPrinterDC );
	hPrinterDC = NULL;

//	FreeProcInstance( (FARPROC)lpDlgFnAbort );
	FreeProcInstance((FARPROC) lpAbortTest );

	Page = oldPage;
	Edit = oldEdit;

	if ( aborted )
	{
		PrintErr(hWnd, SP_USERABORT);
	}
	else
	{
		if ( iEscErr < 0 )
		{
			PrintErr(hWnd, iEscErr);
		}
	}

	ProgressEnd();
	return(!aborted);
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
	LPFRAME lpFrame,
	BOOL bUsePrinterScreening)
{
int y, yline, ystart, ylast, DispWidth, count, sx, Excess, depth, depthOut;
LFIXED yrate, xrate, yoffset;
RECT rPaint, rSource, rDest;
long dx, dy;
LPRECT lpRepairRect;
LPBLT lpBltPrint;
BLTSESSION BltSession;
HPALETTE hOldPal;
LPFRAME lpBaseFrame;
LPTR lpBuffer, lpIn, lpOut, lpConvertLine;
LPTR lpImageData;
BOOL fConvert;
CMSINFO	CmsInfo;
FRMTYPEINFO SrcType, DstType;
CFrameTypeConvert TypeConvert;

#define PRLINES 10

ProgressBegin(1,PROGRESS_ID(IDS_UNDOPRINTING));

lpConvertLine = NULL;
lpImageData = NULL;
if (lpImage)
	lpBaseFrame = ImgGetBaseEditFrame(lpImage);
else
	lpBaseFrame = lpFrame;

// get rects for processing
rDest = *lpDestRect;
rSource = *lpSourceRect;
lpRepairRect = NULL;
DispWidth = RectWidth( &rDest );
if ( !lpRepairRect )
	rPaint = rDest;
else
	{
	rPaint = *lpRepairRect;
	BoundRect( &rPaint, rDest.left, rDest.top, rDest.right, rDest.bottom );
	}

// get source type
FrameGetTypeInfo(lpBaseFrame, &SrcType);

// Check for CMS'd Src
if( Control.CMSEnabled && PrintStyle.fUseCMS &&
		( SrcType.DataType == FDT_RGBCOLOR ||
		SrcType.DataType == FDT_CMYKCOLOR ) &&
		!IsSrcPTSelected( &SrcType.ptInfo ) )
	{
	if( CmsGetSrcPTType( &SrcType, &CmsInfo ) )
		SrcType = CmsInfo.src;
	}
// FramePointer never returns line art
if (SrcType.DataType == FDT_LINEART) 
	FrameSetTypeInfo(&SrcType, FDT_GRAYSCALE);
depth = FrameDepth( lpBaseFrame );
if (depth == 0) depth = 1;

// if printing and nTotalBits == 8 
// need to convert to grayscale
if (!bToScreen && nTotalBits == 8)
	{
	FrameSetTypeInfo(&DstType, FDT_GRAYSCALE);
	depthOut = 1;
	}
else
// if printing as color and we have palette color, convert to RGB
if (SrcType.DataType == FDT_PALETTECOLOR)
	{
	FrameSetTypeInfo(&DstType, FDT_RGBCOLOR);
	depthOut = 3;
	}
else
// if printing using CMS
if (Control.CMSEnabled && PrintStyle.fUseCMS)
	{
	DstType = Separate.cmsInfo.dst; //SrcType;
	depthOut = depth;
	}
// otherwise, print what we got
else
	DstType = SrcType;

// see if a data conversion is needed
fConvert = !FrameTypeInfoEqual(SrcType, DstType);
if (fConvert)
	{
	if (!TypeConvert.Init(SrcType, DstType, count))
		{
		Message(IDS_EMEMALLOC);
		ProgressEnd();
		return(FALSE);
		}
	// allocate a buffer for data conversion
	// buffer is used after sampling so use DispWidth
	lpConvertLine = Alloc((long)DispWidth * depthOut);
	if (!lpConvertLine)
		{
		Message(IDS_EMEMALLOC);
		ProgressEnd();
		return(FALSE);
		}
	}

if ( !(lpBuffer = Alloc((long)DispWidth * depth)) )
	{
	ProgressEnd();
	return(FALSE);
	}

if (lpImage)
	{
	if (!AllocLines((LPPTR)&lpImageData, 1, FrameXSize(lpBaseFrame), depth))
		{
		FreeUp( lpBuffer );
		ProgressEnd();
		return(FALSE);
		}
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

yoffset = (yrate*dy)+FDIV2(yrate);
count = dx + RectWidth(&rPaint);

if ( bToScreen )
	lpBltPrint = lpBltScreen;
else
	{
	if ( Page.ScatterPrint )
		nTotalBits = 1;
	else
	if ( !nTotalBits )
		nTotalBits = (DstType.DataType == FDT_GRAYSCALE == 1 ? 8 : 24);
	if (Page.UsePrinterScreening)
		nTotalBits = -nTotalBits;
	lpBltPrint = InitSuperBlt( hDC, nTotalBits, NULL, NO, YES );
	if (!lpBltPrint)
		{
		ProgressEnd();
		return(FALSE);
		}
	hOldPal = SelectPalette(hDC, (HPALETTE)GetStockObject(DEFAULT_PALETTE), FALSE);
	SelectPalette(hDC, hOldPal, FALSE);
	}

StartSuperBlt( &BltSession, hDC, NULL, lpBltPrint, &rPaint, DstType,
	PRLINES, xDiva, yDiva, bToScreen, NULL, NULL );
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
		(*lpAbortTest)((HDC)hPrinterDC, 0);
		if ( fAbortPrint )
			break;
		}
	yline = ystart + WHOLE(yoffset);
	yoffset += yrate;
	if ( yline != ylast )
		{
		ylast = yline;

		if (lpImage)
			{
			ImgGetLine( lpImage, NULL, sx, yline, FrameXSize(lpBaseFrame) - sx, lpImageData );
			lpIn = lpImageData;
			}
		else 
			lpIn = FramePointer(lpBaseFrame, sx, yline, NO);

		FrameSample( lpBaseFrame, lpIn, 0, lpBuffer, (WORD)dx, count,
			xrate );

		if (fConvert)
			{
			TypeConvert.ConvertData(lpBuffer, lpConvertLine, y, count);
			lpOut = lpConvertLine;
			}
		else
			lpOut = lpBuffer;

		if (!bToScreen)
			{
			if ( PrintStyle.Negative )
				negate( lpOut, (long)count * depthOut );
			if ( DstType.DataType == FDT_GRAYSCALE )
				CorrectGray( lpOut, count );
			else
			if ( DstType.DataType == FDT_RGBCOLOR )
				CorrectRGB( (LPRGB)lpOut, count);
			}
		}
	SuperBlt( &BltSession, lpOut );
	}
SuperBlt( &BltSession, NULL );
if (!bToScreen)
	{
	SelectPalette(hDC, hOldPal, FALSE);
	EndSuperBlt(lpBltPrint);
	}

//if ( !bToScreen )
//	{
//	if ( BltPrint.hGrayPal )
//		DeleteObject( BltPrint.hGrayPal );
//	if ( BltPrint.hColorPal && (BltPrint.hGrayPal != BltPrint.hColorPal) )
//		DeleteObject( BltPrint.hColorPal );
//	}

FreeUp(lpBuffer);
if (lpImageData)
	FreeUp(lpImageData);
if (lpConvertLine)
	FreeUp(lpConvertLine);

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

int PrintFile( HWND hWnd, LPSTR szFile, BOOL EjectPage, LPIMAGE lpImage,
                  LPFRAME lpFrame )
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
	P_STR  pTech;
	BOOL   bSuperBlt;
	BYTE   SepList[5];
	int    iSep;
	int    nTotalBits;
	RECT   rRect;
	PAGE   OldPage;
	int    NumColors;
	static int xDiva, yDiva;
	LPFRAME lpBaseFrame;
	short PassThrough;
	BOOL	bPostScript;

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
		{
			return( FALSE );
		}
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


		/* Set up abort dialog box */
		EnableWindow( hWnd, FALSE );
		hWndAbort = AstralDlg(YES, PictPubApp.GetResourceHandle(), hWnd, 
						IDD_ABORT, DlgPrintAbortProc);

		/* Give abort procedure to gdi to check abort print command */
		lpAbortTest = (ABORTPROC)MakeProcInstance((FARPROC) AbortTest, PictPubApp.Get_hInstAstral() );

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
	xScale = FGET( (Edit.Crop.right-Edit.Crop.left-1), dxOutput );
	yScale = FGET( (Edit.Crop.bottom-Edit.Crop.top-1), dyOutput );

	bSuperBlt = YES;

	// first see if the PostScript passthrought escape exists
#ifdef WIN32
	PassThrough = POSTSCRIPT_PASSTHROUGH;
	PassThrough = Escape(hPrinterDC, QUERYESCSUPPORT, 2,
				(LPSTR)&PassThrough, NULL);
#else
	PassThrough = 0;
#endif
	// if so, we can do PostScript
	if (PassThrough)
		lstrcpy(szTech, "PostScript");
	else // try GETTECHNOLOGY (this won't work on NT, but will on Win32s)
	{	
		if ( Escape(hPrinterDC, GETTECHNOLGY, 0, NULL, szTech) < 0 )
			szTech[0] = '\0';
	}

	bPostScript = StringsEqual("PostScript", szTech);

	if ( !Page.ScatterPrint && Page.GeneratePS ) // Scatterprint goes through GDI
	{
		/* The 2nd parameter may be "LaserPort" if the the 1st is "PCL" */
		/* If so, point to the 2nd technology parameter "Laserport" */
		if ( StringsEqual( "PCL", szTech ) )
		{
			pTech = szTech;
			while( *pTech )
				pTech++;
			lstrcpy( szTech, pTech+1 );
		}

		BOOL fGray = FrameType(lpBaseFrame) <= FDT_GRAYSCALE;
		if ((StringsEqual( "LaserPort", szTech ) &&	fGray) || bPostScript )
		{
			/* For HP grayscale boards and PostScript devices... */
			/* Someday tell people about the printing technology */
			BandingDevice = NO;
			BandInfoDevice = NO;
			bSuperBlt = NO;
		}
	}

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

	SetSepString(hWndAbort, SepList[iSep]);

	// prepare the printer driver to accept data
	if ((iEscErr = StartPage(hPrinterDC)) <= 0)
	{
		if (!iEscErr)
			iEscErr = -1;
		goto EXIT;
	}

	/* Send the mirror flag */
	if ( PrintStyle.EmulsionDown )
	{
#ifdef WIN32
		XFORM	tfMirror;

		tfMirror.eM11 = -1; 				tfMirror.eM12 = 0;	//tfMirror[2] = 0;
		tfMirror.eM21 = 0;      			tfMirror.eM22 = 1; 	//tfMirror[5] = 0;
		tfMirror.eDx = (FLOAT)dxPrinter; 	tfMirror.eDy = 0;   //tfMirror[8] = 1;
		SetWorldTransform(hPrinterDC, &tfMirror);
#else
		LFIXED f, tfMirror[9];

		f = FGET( dxPrinter, 1 );
		tfMirror[0] = -UNITY; tfMirror[1] = 0;     tfMirror[2] = 0;
		tfMirror[3] = 0;      tfMirror[4] = UNITY; tfMirror[5] = 0;
		tfMirror[6] = f;      tfMirror[7] = 0;     tfMirror[8] = UNITY;
		if ( Escape(hPrinterDC, TRANSFORM_CTM, sizeof(tfMirror),
			(LPSTR)tfMirror, 0L) < 0 )
		; // So what
#endif
	}

	/* Loop to transfer one band at a time to the spooler. */
	do
	{
		/* Check for user input to abort dialog box */
		(*lpAbortTest)((HDC)hPrinterDC, 0);
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
			OutputFurniture( lpImage, hPrinterDC, &rPrintBand, &rImage,
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
			(*lpAbortTest)((HDC)hPrinterDC, 0);

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

				SuperLock( YES );
				PrintImage( hPrinterDC, NO, &rSource, &rDest,
					xDiva, yDiva, (yDest - rImage.top), dyOutput,
					nTotalBits,
					lpImage, 
					lpFrame,
					Page.UsePrinterScreening || bPostScript );
				SuperLock( NO );
			}
			else
			{
				if ( StringsEqual( "LaserPort", szTech ) )
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
				AbortDoc( hPrinterDC );
			}
		}
	}
	else
	{
		/* Sent all of page without problems; eject it. */
		if (EjectPage)
		{
			EndPage( hPrinterDC );
		}

		if ( SepList[++iSep] )
			goto StartPage;

		if (EjectPage)
		{
			EndDoc( hPrinterDC );
		}
	}

	if (EjectPage)
	{
		EnableWindow( hWnd, TRUE );
		if ( hWndAbort )
		{
			AstralDlgEnd(hWndAbort, TRUE);
			/* abort dialog box still up, so bring it down  */
			hWndAbort = NULL;
		}

		FreeProcInstance((FARPROC) lpAbortTest );

		DeleteDC( hPrinterDC );
		hPrinterDC = NULL;
	}

	Page = OldPage;

	if (EjectPage)
	{
		if ( fAbortPrint )
			PrintErr(hWnd, SP_USERABORT);
		else if ( iEscErr < 0 )
			PrintErr(hWnd, iEscErr);
	}

    if ( (iEscErr >= 0) && !fAbortPrint )
	{
		if (lpImage && lpImage->pServerDoc)
		    lpImage->pServerDoc->m_SummaryInfo.MarkAsPrinted();
	}
	return( (iEscErr >= 0) && !fAbortPrint );
}

/***************************************************************************/

void OutputFurniture( LPIMAGE lpImage, HDC hDC, LPRECT lpBandRect,
                        LPRECT lpRect, int xRes, int yRes, BYTE cSep )
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
	SIZE size;

	// always make sure GDI has somthing to do, otherwise no
	// header info gets output
	MoveTo(hDC, 0, 0);
	LineTo(hDC, 0, 0);

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

	if ( PrintStyle.RegMarks || PrintStyle.TrimMarks || PrintStyle.Labels )
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
				
				GetTextExtentPoint( hDC, lpString, lstrlen(lpString), &size );
				dx += size.cx;
				dx += eighth;
			}

			if ( cSep == 'M' )
				PrintLabel( hDC, Rect.left+dx, Rect.top, Color, IDS_LABELSEPM );

			if ( AstralStr( IDS_LABELSEPM, &lpString ) )
			{
				GetTextExtentPoint( hDC, lpString, lstrlen(lpString), &size );
				dx += size.cx;
				dx += eighth;
			}

			if ( cSep == 'Y' )
				PrintLabel( hDC, Rect.left+dx, Rect.top, Color,	IDS_LABELSEPY );

			if ( AstralStr( IDS_LABELSEPY, &lpString ) )
			{
				GetTextExtentPoint( hDC, lpString, lstrlen(lpString), &size );
				dx += size.cx;
				dx += eighth;
			}

			if ( cSep == 'K' )
				PrintLabel( hDC, Rect.left+dx, Rect.top, Color, IDS_LABELSEPK );
		}
	}

	if ( PrintStyle.TrimMarks )
	{
		// top, left
		Rect.left   = lpRect->left - eighth - quarter;
		Rect.right  = lpRect->left;
		Rect.top    = lpRect->top  - eighth - quarter;
		Rect.bottom = lpRect->top;

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
			MoveTo( hDC, Rect.left, Rect.bottom );
			LineTo( hDC, Rect.right - eighth, Rect.bottom );
			MoveTo( hDC, Rect.right, Rect.top );
			LineTo( hDC, Rect.right, Rect.bottom - eighth );
		}

		// top, right
		Rect.left   = lpRect->right;
		Rect.right  = lpRect->right + eighth + quarter;
		Rect.top    = lpRect->top   - eighth - quarter;
		Rect.bottom = lpRect->top;

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
			MoveTo( hDC, Rect.right, Rect.bottom );
			LineTo( hDC, Rect.left + eighth, Rect.bottom );
			MoveTo( hDC, Rect.left, Rect.top );
			LineTo( hDC, Rect.left, Rect.bottom - eighth );
		}

		// bottom, left
		Rect.left   = lpRect->left   - eighth - quarter;
		Rect.right  = lpRect->left;
		Rect.top    = lpRect->bottom;
		Rect.bottom = lpRect->bottom + eighth + quarter;

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
			MoveTo( hDC, Rect.left, Rect.top );
			LineTo( hDC, Rect.right - eighth, Rect.top );
			MoveTo( hDC, Rect.right, Rect.bottom );
			LineTo( hDC, Rect.right, Rect.top + eighth );
		}

		// right, bottom
		Rect.left   = lpRect->right;
		Rect.right  = lpRect->right  + eighth + quarter;
		Rect.top    = lpRect->bottom;
		Rect.bottom = lpRect->bottom + eighth + quarter;

		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
			MoveTo( hDC, Rect.right, Rect.top );
			LineTo( hDC, Rect.left + eighth, Rect.top );
			MoveTo( hDC, Rect.left, Rect.bottom );
			LineTo( hDC, Rect.left, Rect.top + eighth );
		}
	}

	if ( PrintStyle.RegMarks )
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

	if ( PrintStyle.Labels )
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
						(LPSTR)ctime(&ltime) );

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
				IDS_LABELSTYLE, (LPSTR)Names.PrintStyle );
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
				(LPSTR)PrintStyle.CalibrateName );
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
					IDS_LABELMINMAXDOT, (LPSTR)szType,
					(int)TOPERCENT(Calibrate.Hilight),
					(int)TOPERCENT(Calibrate.Shadow) );
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
						(LPSTR)sz0, (LPSTR)sz1 );
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
						(LPSTR)sz0, (LPSTR)sz1,
						(LPSTR)sz2, (LPSTR)sz3 );
					Rect.top += dy;
					FixedAscii( Halftone.ScreenRuling[0], sz0, -1 );
					FixedAscii( Halftone.ScreenRuling[1], sz1, -1 );
					FixedAscii( Halftone.ScreenRuling[2], sz2, -1 );
					FixedAscii( Halftone.ScreenRuling[3], sz3, -1 );
					PrintLabel( hDC, Rect.left, Rect.top, Color,
						IDS_LABELCOLORFREQUENCY,
						(LPSTR)sz0, (LPSTR)sz1,
						(LPSTR)sz2, (LPSTR)sz3 );
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
						(LPSTR)Separate.BGName );
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

	if ( PrintStyle.StepScale )
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
	char     szBuffer[1024];
	va_list  lpArguments;
	LPSTR     lpString;
	int      retc, length;

	/* The caller passes stringtable ID's, and not hardcoded strings */
	/* Used to put a variety of labels on the printed page */
	if ( !AstralStr( idStr, &lpString ) )
		return( IDCANCEL );

   va_start( lpArguments, idStr );
	length = wvsprintf( szBuffer, lpString, lpArguments );
   va_end( lpArguments );

	ColorText( hDC, x, y, szBuffer, length, Color );

	return( retc );
}


/***************************************************************************/

void OutputRegMark( HDC hDC, LPRECT lpRect )
{
	RECT Rect;
	int xsize, ysize;

	Rect  = *lpRect;
	xsize = RectWidth(lpRect);
	ysize = RectHeight(lpRect);

	InflateRect( &Rect, -xsize/12, -ysize/12 );

	Ellipse( hDC, Rect.left, Rect.top, Rect.right, Rect.bottom );

	InflateRect( &Rect, xsize/12, ysize/12 );

	MoveTo( hDC, Rect.left, (Rect.top + Rect.bottom)/2 );
	LineTo( hDC, Rect.right, (Rect.top + Rect.bottom)/2 );
	MoveTo( hDC, (Rect.left + Rect.right)/2, Rect.top );
	LineTo( hDC, (Rect.left + Rect.right)/2, Rect.bottom );

	InflateRect( &Rect, -xsize/12, -ysize/12 );

	xsize = RectWidth(lpRect);
	ysize = RectHeight(lpRect);

	InflateRect( &Rect, -xsize/5, -ysize/5 );

	Ellipse( hDC, Rect.left, Rect.top, Rect.right, Rect.bottom );

	MoveTo( hDC, Rect.left, (Rect.top + Rect.bottom)/2 );
	LineTo( hDC, Rect.right, (Rect.top + Rect.bottom)/2 );
	MoveTo( hDC, (Rect.left + Rect.right)/2, Rect.top );
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

BOOL CALLBACK EXPORT AbortTest( HDC hDC, int iCode )
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

// LOCAL prototypes
LOCAL BOOL PrintAbort_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PrintAbort_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPrintAbortProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, PrintAbort_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, PrintAbort_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_ABORT);
	}
}

/************************************************************************/
LOCAL BOOL PrintAbort_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
SetWindowText( hDlg, szAppName);
SetDlgItemText(hDlg, ID_NAME,    szFilename);
SetDlgItemText(hDlg, ID_PRINTER, lpchDevice);
SetDlgItemText(hDlg, ID_PORT,    lpchPort);
CenterPopup(hDlg);
return(TRUE);
}

/************************************************************************/
LOCAL void PrintAbort_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDCANCEL:
		fAbortPrint = TRUE;
	break;

   default:
	break;
   }
}

/***************************************************************************\

	GETPRINTERDC reads the device string line from WIN.INI and trys to
	create a DC for  the device.  It rejects all DCs incapable of BITBLT. 
	It returns a DC or NULL if there is an error.

\***************************************************************************/

HDC GetPrinterDC()
{
	LPSTR lpch;
	HDC hDC;

	if ( !Names.Printer[0] )
		GetDefaultPrinter( Names.Printer );

	lstrcpy( szPrinter, Names.Printer );
	lpch = szPrinter;
	lpchDevice = lpch;
	lpchDriver = lpch;
	lpchPort   = lpch;

	if ( !(*lpch) )
	{
		Message( IDS_EBADDEVICE );
		return( NULL );
	}

	/* skip leading blanks */
	while ( *lpch && *lpch == ' ' )
		lpch = (LPSTR) AnsiNext(lpch);

	lpchDevice = lpch;

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
	*lpch++ = 0;

	/* skip leading blanks */
	while ( *lpch && *lpch == ' ' )
	{
		if (*lpch == '.')
			*lpch = 0;
		lpch = (LPSTR) AnsiNext(lpch);
	}

	lpchDriver = lpch;

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
	*lpch++ = 0;

	/* skip leading blanks */
	while ( *lpch && *lpch == ' ' )
		lpch = (LPSTR) AnsiNext(lpch);

	lpchPort = lpch;

	/* Create the DC. */
	if ( hDC = CreateDC( lpchDriver, lpchDevice, lpchPort, NULL ) )
	{
		/* if the DC cannot do DIBTODEV (BITBLT) ... */
		if ( !(GetDeviceCaps(hDC, RASTERCAPS) & RC_BITBLT) )
		{
			DeleteDC(hDC);
			hDC = NULL;
		}
	}

	return( hDC );
}

/***************************************************************************/

void PrintErr( HWND hWnd, int ierr )
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
	INT16  nCount;
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
	int start;
	LPFRAME lpBaseFrame;
	int      iErr;

	ProgressBegin(1,PROGRESS_ID(IDS_UNDOPRINTING));
	
	if (lpImage)
		lpBaseFrame = ImgGetBaseEditFrame(lpImage);
	else
		lpBaseFrame = lpFrame;
	/* Send the style (1=line, 2=diam), freq, angle(0,45,90) */
	if ( !LaserPrint( "\033$s%ds%df%dA", 2,
   	WHOLE(Halftone.ScreenRuling[0]), WHOLE(Halftone.ScreenAngle[0]) ) )
		goto ExitFalse;

	/* Send the destination (x,y) and size (w,h) 1/1200" */
	if ( !LaserPrint( "\033$d%dx%dy%dw%dH",
		LX(xDest),  LY(yDest), LX(dxDest), LY(dyDest) ) )
		goto ExitFalse;

	/* Let the printer do any upsizing */
	if ( dySrc < dyDest )
	{
		yrate  = FUNITY;
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
	yoffset = FDIV2(yrate);

	for (y=0; y<dyDest; y++)
	{
		if (AstralClockCursor( y, dyDest, YES ))
		{
			fAbortPrint = YES;
			break;
		}

		/* Check for user input to abort dialog box */
		(*lpAbortTest)((HDC)hPrinterDC, 0);
		if ( fAbortPrint )
			break;

		ylast = yline;
		yline = ystart + WHOLE(yoffset);
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

			CorrectGray( lpBits, dxDest );
		}

		/* Send raw data to the Intel or DPTek grayscale board */
		if ( (iErr = Escape( hPrinterDC, PASSTHROUGH, lpLaser->nCount,
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
	va_list  lpArguments;
	LPLASER  lpLaser;
	int      iErr;

	lpLaser = (LPLASER)LineBuffer[0];

	/* Send short escape sequences to the Intel or DPTek grayscale */
	/* board with arguments specified exactly like printf */

   va_start( lpArguments, lpFormat );
	lpLaser->nCount = wvsprintf( lpLaser->cData, lpFormat, lpArguments );
   va_end( lpArguments );

	iErr = Escape( hPrinterDC, PASSTHROUGH, lpLaser->nCount, (LPSTR)lpLaser, 0 );

	return( iErr >= 0 );
}

/***************************************************************************/

BOOL IsPostScript()
{
	HDC    hPrinterDC;     /* DC of the printer */
	STRING szTech;
	int PassThrough;

	if ( !(hPrinterDC = GetPrinterDC()) )
		return( FALSE );

	// first see if the PostScript passthrought escape exists
#ifdef WIN32
	PassThrough = POSTSCRIPT_PASSTHROUGH;
	PassThrough = Escape(hPrinterDC, QUERYESCSUPPORT, 2,
			(LPSTR)&PassThrough, NULL);
#else
	PassThrough = 0;
#endif
	// if so, we can do PostScript
	if (PassThrough)
		lstrcpy(szTech, "PostScript");
	else // try GETTECHNOLOGY (this won't work on NT, but will on Win32s)
	{	
		if ( Escape(hPrinterDC, GETTECHNOLGY, 0, NULL, szTech) < 0 )
			szTech[0] = '\0';
	}

	DeleteDC( hPrinterDC );

	if ( !StringsEqual( "PostScript", szTech ) )
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








