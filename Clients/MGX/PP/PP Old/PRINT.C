// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

/************************************************************************/
/*  Print.c   print functions for astral's picture publisher		*/
/************************************************************************/
/*
 NOTE: these routines send a document to the print spooler, and also
 control a modeless dialog box which lets the user abort the job.
 The way this application prints is by a method called "nextbanding".
 Instead of sending a whole document at once to the spooler, nextbanding
 sends a portion of the document at a time.  This method is faster and
 doesn't require as much disk space.

				       Printed page

				 _______dxPrinter_______
				|			|
				|	dxOutput	|
				|	+-------+	|
				|	|	|	|
				|____dxDest_____|_______|
		     rBand ---> |	|	|	dyDest
	(holds 4 coords.)       |_______|_______|_______|
				|	|	|	|
				|  dyOutput	|	dyPrinter
				|	+-------+	|
				|			|
				|			|
				|			|
				|_______________________|

 Each time you call Escape(..,NEXTBAND,..,..,..) you are given the
 coordinates of the current band on the printer page.  You must then
 compute what you want to print in this band, and then use GDI functions
 to write to the printerDC.

***********************************************************************/

extern HWND	hWndAstral;
extern HANDLE	hInstAstral;	/* handle to calling application instance */
extern char	szAppName[];	/* calling application name */

static HWND	hWndAbort;	/* handle to modeless abort dialog box */
static FNAME	szFilename;	/* file to print. (no path) */
static STRING	szProfile;	/* printer profile from WIN.INI */
static PTR	pchDevice, pchDriver, pchPort; /* ...into the profile string */
HDC	hPrinterDC; 	/* DC of the printer */
FARPROC	lpAbortTest;
BOOL	fAbortPrint;	/* abort sending job to spooler? */

/***********************************************************************
 Printfile reads win.ini to get information about the
 user's printer, creates a DC for the printer, puts up a modeless dialog
 box informing the user that the job is being sent, and finally sends the
 job to the spooler one band at a time.
***********************************************************************/

/***********************************************************************/
int PrintFile (hWnd, szFile)
/***********************************************************************/
HWND hWnd;		/* window handle of parent window */
LPTR szFile;		/* name of file to print.  (no path!) */
{
int	iEscErr, iPrResX, iPrResY;
int	xDest,  yDest;	/* location of current slice on printer */
int	dxDest, dyDest;	/* width & height of each printer band */
int	xSrc,   ySrc;	/* location of current slice of source */
int	dxSrc,  dySrc;	/* width & height of current slice of source */
int	dyDestSlice;	/* height of current slice */
int	dxOutput, dyOutput; /* width & height of image area on page */
int	dxPrinter, dyPrinter; /* width & height of page */
FIXED	xScale, yScale;	/* scale factor of destination to source image */
STRING	szDocName;	/* spooler only takes max. 32 char doc name */
RECT	rBand, rPrintBand;/* 4 coordinates of current band of printer */
RECT	rImage;		/* 4 coordinates of the image on the printed page */
RECT	rSource, rDest;
FARPROC	lpDlgFnAbort;
STRING	szTech;
WORD	v;
FIXED	rate;
PTR	pTech;
BOOL	bSuperBlt, BandingDevice;
short	BandInfoDevice;
BYTE	SepList[5];
int	iSep;
struct	{
	BOOL	fGraphics;
	BOOL	fText;
	RECT	rBand;
	} Info;
static int xDiva, yDiva;

#define LX(x) ((int) (((long)x*1200L)/(long)iPrResX) )
#define LY(y) ((int) (((long)y*1200L)/(long)iPrResY) )
#define PX(x) ((int) (((long)x*72L)/(long)iPrResX) )
#define PY(y) ((int) (((long)y*72L)/(long)iPrResY) )

/* put document name in szDocName */
lstrcpy( szFilename, szFile );
lstrcpy( szDocName, szAppName );
lstrcat( szDocName, " " );
lstrcat( szDocName, szFilename );

hWndAbort = NULL;
fAbortPrint = FALSE;
iEscErr = 0;

/* get the printer of the current printer */
if ( !(hPrinterDC = GetPrinterDC()) )
	return( FALSE );

/* Determine the banding capabilities of the printer driver */
BandingDevice = GetDeviceCaps(hPrinterDC, RASTERCAPS) & RC_BANDING;
BandInfoDevice = BANDINFO;
BandInfoDevice = Escape(hPrinterDC, QUERYESCSUPPORT, 2,
	(LPSTR)&BandInfoDevice, NULL);
/*if ( BandingDevice )
	Print("A banding device");
else	Print("A non-banding device");
if ( BandInfoDevice )
	Print("Bandinfo supported");
else	Print("Bandinfo NOT supported");*/

/* Set up abort dialog box */
lpDlgFnAbort = MakeProcInstance( DlgFnAbort, hInstAstral );
EnableWindow(hWnd, FALSE);
hWndAbort = CreateDialog( hInstAstral, MAKEINTRESOURCE( IDD_ABORT ),
	 hWnd, lpDlgFnAbort);

/* Give abort procedure to gdi to check abort print command */
lpAbortTest = MakeProcInstance( AbortTest, hInstAstral );
if ((iEscErr = Escape(hPrinterDC, SETABORTPROC, 0, (LPSTR)lpAbortTest,0L)) <0)
	goto EXIT;

/* Send the screen angle */
//if ((iEscErr = Escape(hPrinterDC, SET_SCREEN_ANGLE, lstrlen( szDocName ),
//    (LPSTR)szDocName, 0L)) < 0)
//	goto EXIT;

/* Send document name to spooler */
if ((iEscErr = Escape(hPrinterDC, STARTDOC, lstrlen( szDocName ),
    (LPSTR)szDocName, 0L)) < 0)
	goto EXIT;

/* Get size of printer page */
dxPrinter = GetDeviceCaps(hPrinterDC, HORZRES);
dyPrinter = GetDeviceCaps(hPrinterDC, VERTRES);

/* Use the device resolution (in pixels per inch) to compute */
/* how big the image will be on the output device */
iPrResX = GetDeviceCaps(hPrinterDC, LOGPIXELSX);
iPrResY = GetDeviceCaps(hPrinterDC, LOGPIXELSY);
dxOutput = FMUL( iPrResX, Edit.Width );
dyOutput = FMUL( iPrResY, Edit.Height );

/* Locate the image on the page */
if ( Page.Centered )
	{
	rImage.left = rImage.right = (dxPrinter-dxOutput)/2;
	rImage.top = rImage.bottom = (dyPrinter-dyOutput)/2;
	}
else	{
	rImage.left = rImage.right = FMUL( iPrResX, Page.OffsetX );
	rImage.top = rImage.bottom = FMUL( iPrResY, Page.OffsetY );
	}
rImage.right += (dxOutput-1);
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
	if ( Escape(hPrinterDC, GETTECHNOLGY, NULL, NULL, szTech) < 0)
		szTech[0] = '\0';

	/* The 2nd parameter may be "LaserPort" if the the 1st is "PCL" */
	/* If so, point to the 2nd technology parameter "Laserport" */
	if ( !strcmp( "PCL", szTech ) )
		{
		pTech = szTech;
		while( *pTech )
			pTech++;
		strcpy( szTech, pTech+1 );
		}

	if ( (!strcmp( "LaserPort", szTech ) && DEPTH == 1) ||
	      !strcmp( "PostScript", szTech ) )
		{ /* For HP grayscale boards and PostScript devices... */
		/* Someday tell people about the printing technology */
		BandingDevice = NO;
		BandInfoDevice = NO;
		bSuperBlt = NO;
		}
	}

// Make the Page.UCRlut for faster processing
rate = FGET( Printer.UCR, 100 );
for ( v=0; v<256; v++ )
	Page.UCRlut[v] = FMUL( v, rate );

// Make the Page.Dotlut for faster processing
for ( v=0; v<Printer.Hilight; v++ )
	Page.Dotlut[v] = Printer.Hilight;
for ( v=Printer.Hilight; v<Printer.Shadow; v++ )
	Page.Dotlut[v] = v;
for ( v=Printer.Shadow; v<256; v++ )
	Page.Dotlut[v] = Printer.Shadow;

AstralCursor( IDC_WAIT );
if ( Page.Type == IDC_PRINT_COLORSEPS || Page.Type == IDC_PRINT_BLACKSEPS )
	{
	iSep = 0;
	if (Page.SepFlags & C_SEP) SepList[iSep++] = 'C';
	if (Page.SepFlags & M_SEP) SepList[iSep++] = 'M';
	if (Page.SepFlags & Y_SEP) SepList[iSep++] = 'Y';
	if (Page.SepFlags & K_SEP) SepList[iSep++] = 'K';
	if ( !iSep ) SepList[iSep++] = 'K';
	SepList[iSep] = '\0';
	}
else
if ( Page.Type == IDC_PRINT_GRAY )
	{
	SepList[0] = 'X';
	SepList[1] = '\0';
	}
else	SepList[0] = '\0';

iSep = 0; // Start with the first sep in the list

StartPage:

/* Loop to transfer one band at a time to the spooler. */
do	{
	/* Check for user input to abort dialog box */
	(*lpAbortTest)((HDC)hPrinterDC, 0);
	if ( fAbortPrint )
		break;

	if ( BandingDevice )
		{ /* Get the next band rectangle */
		if ((iEscErr = Escape(hPrinterDC, NEXTBAND, 0, NULL,
		    (LPSTR)&rPrintBand)) <0)
			goto EXIT;
		}
	else	{ /* Create a single band for the entire page */
		rPrintBand.top = 0;
		rPrintBand.left = 0;
		rPrintBand.bottom = dyPrinter-1;
		rPrintBand.right = dxPrinter-1;
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
		if ((iEscErr = Escape(hPrinterDC, BANDINFO, 0,
		   (LPSTR)&Info, (LPSTR)&Info)) <0)
			{
			Info.fText = NO;
			Info.fGraphics = YES;
			}
		}

/*	Print("graphics=%d, text=%d, rect=(%d,%d)-(%d,%d)",
		Info.fGraphics, Info.fText,
		Info.rBand.left, Info.rBand.top,
		Info.rBand.right, Info.rBand.bottom ); */

	if ( !Info.fGraphics )
		continue;

	xDest  = rBand.left;
	yDest  = rBand.top;
	dxDest = RectWidth( &rBand );
	dyDest = RectHeight( &rBand );

	OutputFurniture( hPrinterDC, &rPrintBand, &rImage, iPrResX, iPrResY,
		SepList[iSep] );

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
			dbg("Print src (%d,%d)-(%d,%d) dest (%d,%d)-(%d,%d)",
			rSource.left,rSource.top,rSource.right,rSource.bottom,
			rDest.left,rDest.top,rDest.right,rDest.bottom); dbg(0);
			PrintImage( hPrinterDC, &rSource, &rDest, xDiva, yDiva,
 				(yDest - rImage.top), dyOutput );
			}
		else	{
			if ( !strcmp( "LaserPort", szTech ) )
				iEscErr = AstralLaserPrint(
				xSrc, ySrc, dxSrc, dySrc, xDest, yDest,
				dxDest, dyDest, iPrResX, iPrResY );
			else	iEscErr = PSPrint( SepList[iSep],
				xSrc, ySrc, dxSrc, dySrc, xDest, yDest,
				dxDest, dyDest, iPrResX, iPrResY );
			}

		/* Adjust the destination start location */
		yDest += dyDestSlice;

		/* Adjust the destination lines left */
		/* If there are no lines left, go get another band */
		if ( ( dyDest -= dyDestSlice ) <= 0 )
			break;
		}

	} while ( BandingDevice );

EXIT:

if ( iEscErr < 0 || fAbortPrint )
	{ /* Remove document from spooler. */
	if ( fAbortPrint )
		iEscErr = SP_USERABORT;
// reference manual says we should do this if an abort, not if a real error
	Escape(hPrinterDC, ABORTDOC, 0, NULL, 0L);
	}
else	{ /* Sent all of page without problems; eject it. */
	if ( !BandingDevice )
		Escape(hPrinterDC, NEWFRAME, 0, NULL, 0L);
	if ( SepList[++iSep] )
		goto StartPage;
	Escape(hPrinterDC, ENDDOC, 0, NULL, 0L);
	}

AstralCursor( NULL );
if ( hWndAbort != NULL )
	{
	/* abort dialog box still up, so bring it down  */
	EnableWindow( hWnd, TRUE );
	DestroyWindow( hWndAbort );
	}

FreeProcInstance( lpDlgFnAbort );
FreeProcInstance( lpAbortTest );
DeleteDC( hPrinterDC );

if ( iEscErr < 0 )
	PrintErr(hWnd, iEscErr);

return( (iEscErr >= 0) && !fAbortPrint );
} /* end printfile */


/***********************************************************************/
void OutputFurniture( hDC, lpBandRect, lpRect, xRes, yRes, cSep )
/***********************************************************************/
HDC hDC;
LPRECT lpBandRect, lpRect;
int xRes, yRes;
BYTE cSep;
{
RECT Rect, rDummy;
int i, x, y, dx, dy, value, quarter, eighth, iPenWidth, iHeight, idStr;
HPEN hPen, hOldPen;
HBRUSH hBrush, hOldBrush;
COLOR Color;
LPTR lpString;
STRING sz, szType;
BYTE sz0[16], sz1[16], sz2[16], sz3[16];
TEXTMETRIC tm;
long ltime;

if ( Page.Type == IDC_PRINT_COLORSEPS )
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
else Color = RGB(0,0,0);

if ( !(iPenWidth = ((4L * max(xRes,yRes)) + 500L) / 1000L) )
	iPenWidth = 1;
GetTextMetrics( hDC, &tm );
dy = tm.tmHeight /*(tmAscent+tmDescent)*/ + tm.tmInternalLeading + 4*iPenWidth;

hPen = CreatePen( PS_SOLID, iPenWidth, Color );
hOldPen = SelectObject( hDC, hPen );
hOldBrush = SelectObject( hDC, GetStockObject(WHITE_BRUSH) );

quarter = (xRes+2)/4; // 1/4 inch
eighth = quarter/2; // 1/8 inch
if ( Printer.RegMarks || Printer.TrimMarks || Printer.Labels )
	{
	Rect.left = lpRect->left + eighth;
	Rect.right = lpRect->right;
	Rect.top = lpRect->top - eighth - quarter - dy;
	Rect.bottom = lpRect->top - eighth - quarter;
	if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
		if ( cSep != 'C' && cSep != 'M' && cSep != 'Y' && cSep != 'K' )
			PrintLabel( hDC, Rect.left, Rect.top, Color,
				Page.Type );
		dx = 0;
		if ( cSep == 'C' )
			PrintLabel( hDC, Rect.left+dx, Rect.top, Color,
				IDS_LABELSEPC );
		if ( AstralStr( IDS_LABELSEPC, &lpString ) )
			{
			dx += GetTextExtent( hDC, lpString, lstrlen(lpString) );
			dx += eighth;
			}
		if ( cSep == 'M' )
			PrintLabel( hDC, Rect.left+dx, Rect.top, Color,
				IDS_LABELSEPM );
		if ( AstralStr( IDS_LABELSEPM, &lpString ) )
			{
			dx += GetTextExtent( hDC, lpString, lstrlen(lpString) );
			dx += eighth;
			}
		if ( cSep == 'Y' )
			PrintLabel( hDC, Rect.left+dx, Rect.top, Color,
				IDS_LABELSEPY );
		if ( AstralStr( IDS_LABELSEPY, &lpString ) )
			{
			dx += GetTextExtent( hDC, lpString, lstrlen(lpString) );
			dx += eighth;
			}
		if ( cSep == 'K' )
			PrintLabel( hDC, Rect.left+dx, Rect.top, Color,
				IDS_LABELSEPK );
		}
	}
if ( Printer.TrimMarks )
	{
	// top, left
	Rect.left = lpRect->left - eighth - quarter;
	Rect.right = lpRect->left;
	Rect.top = lpRect->top - eighth - quarter;
	Rect.bottom = lpRect->top;
	if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
		MoveTo( hDC, Rect.left, Rect.bottom );
		LineTo( hDC, Rect.right - eighth, Rect.bottom );
		MoveTo( hDC, Rect.right, Rect.top );
		LineTo( hDC, Rect.right, Rect.bottom - eighth );
		}
	// top, right
	Rect.left = lpRect->right;
	Rect.right = lpRect->right + eighth + quarter;
	Rect.top = lpRect->top - eighth - quarter;
	Rect.bottom = lpRect->top;
	if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
		MoveTo( hDC, Rect.right, Rect.bottom );
		LineTo( hDC, Rect.left + eighth, Rect.bottom );
		MoveTo( hDC, Rect.left, Rect.top );
		LineTo( hDC, Rect.left, Rect.bottom - eighth );
		}
	// bottom, left
	Rect.left = lpRect->left - eighth - quarter;
	Rect.right = lpRect->left;
	Rect.top = lpRect->bottom;
	Rect.bottom = lpRect->bottom + eighth + quarter;
	if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
		MoveTo( hDC, Rect.left, Rect.top );
		LineTo( hDC, Rect.right - eighth, Rect.top );
		MoveTo( hDC, Rect.right, Rect.bottom );
		LineTo( hDC, Rect.right, Rect.top + eighth );
		}
	// right, bottom
	Rect.left = lpRect->right;
	Rect.right = lpRect->right + eighth + quarter;
	Rect.top = lpRect->bottom;
	Rect.bottom = lpRect->bottom + eighth + quarter;
	if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
		MoveTo( hDC, Rect.right, Rect.top );
		LineTo( hDC, Rect.left + eighth, Rect.top );
		MoveTo( hDC, Rect.left, Rect.bottom );
		LineTo( hDC, Rect.left, Rect.top + eighth );
		}
	}
if ( Printer.RegMarks )
	{
	// top
	Rect.left = (lpRect->left + lpRect->right)/2 - quarter/2;
	Rect.right = Rect.left + quarter;
	Rect.top = lpRect->top - eighth - quarter;
	Rect.bottom = lpRect->top - eighth;
	if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		OutputRegMark( hDC, &Rect );
	// bottom
	Rect.left = (lpRect->left + lpRect->right)/2 - quarter/2;
	Rect.right = Rect.left + quarter;
	Rect.top = lpRect->bottom + eighth;
	Rect.bottom = lpRect->bottom + eighth + quarter;
	if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		OutputRegMark( hDC, &Rect );
	// left
	Rect.left = lpRect->left - eighth - quarter;
	Rect.right = lpRect->left - eighth;
	Rect.top = (lpRect->top + lpRect->bottom)/2 - quarter/2;
	Rect.bottom = Rect.top + quarter;
	if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		OutputRegMark( hDC, &Rect );
	// right
	Rect.left = lpRect->right + eighth;
	Rect.right = lpRect->right + eighth + quarter;
	Rect.top = (lpRect->top + lpRect->bottom)/2 - quarter/2;
	Rect.bottom = Rect.top + quarter;
	if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		OutputRegMark( hDC, &Rect );
	}
if ( Printer.Labels )
	{
	Rect.left = 4 * quarter; // 1 inch
	Rect.right = 34 * quarter; // 8.5 inches
	Rect.top = quarter; // .25 inches
	Rect.bottom = 9 * quarter;// 2.25 inches
	if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
		{
		time(&ltime);
		PrintLabel( hDC, Rect.left, Rect.top, Color,
			IDS_LABELPICNAME, lpImage->CurFile,
			(LPTR)ctime(&ltime) );
		Rect.top += dy;
		if ( AstralStr( Printer.Type, &lpString ) )
			lstrcpy( szType, lpString );
		else	lstrcpy( szType, "???" );
		DeviceDescription( Names.Printer, sz );
		PrintLabel( hDC, Rect.left, Rect.top, Color,
			IDS_LABELPRINTER, (LPTR)szType, (LPTR)sz );
		Rect.top += dy;
		PrintLabel( hDC, Rect.left, Rect.top, Color,
			IDS_LABELSTYLE, (LPTR)Names.PrintStyle );
		Rect.top += dy;
		if ( !Printer.DoCalibration )
			{
			PrintLabel( hDC, Rect.left, Rect.top, Color,
				IDS_LABELNOCALIBRATION );
			Rect.top += dy;
			}
		else	{
			PrintLabel( hDC, Rect.left, Rect.top, Color,
				IDS_LABELCALIBRATION,
				(LPTR)Printer.CalName );
			Rect.top += dy;
			}
		if ( !Printer.DoHalftoning )
			{
			PrintLabel( hDC, Rect.left, Rect.top, Color,
				IDS_LABELNOHALFTONING );
			Rect.top += dy;
			}
		else	{
			if ( AstralStr( Printer.DotShape, &lpString ) )
				lstrcpy( szType, lpString );
			else	lstrcpy( szType, "???" );
			PrintLabel( hDC, Rect.left, Rect.top, Color,
				IDS_LABELMINMAXDOT, (LPTR)szType,
				TOPERCENT(Printer.Hilight),
				TOPERCENT(Printer.Shadow) );
			Rect.top += dy;
			if ( cSep )
				{
				i = ( cSep == 'C' ? 0 :
				    ( cSep == 'M' ? 1 :
				    ( cSep == 'Y' ? 2 :
				    ( cSep == 'K' ? 3 : 3 ) ) ) );
				FixedAscii( Printer.ScreenRuling[i], sz0 );
				FixedAscii( Printer.ScreenAngle[i], sz1 );
				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELSINGLEHALFTONE,
					(LPTR)sz0, (LPTR)sz1 );
				Rect.top += dy;
				}
			else	{
				FixedAscii( Printer.ScreenAngle[0], sz0 );
				FixedAscii( Printer.ScreenAngle[1], sz1 );
				FixedAscii( Printer.ScreenAngle[2], sz2 );
				FixedAscii( Printer.ScreenAngle[3], sz3 );
				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELCOLORFREQUENCY,
					(LPTR)sz0, (LPTR)sz1,
					(LPTR)sz2, (LPTR)sz3 );
				Rect.top += dy;
				FixedAscii( Printer.ScreenRuling[0], sz0 );
				FixedAscii( Printer.ScreenRuling[1], sz1 );
				FixedAscii( Printer.ScreenRuling[2], sz2 );
				FixedAscii( Printer.ScreenRuling[3], sz3 );
				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELCOLORANGLE,
					(LPTR)sz0, (LPTR)sz1,
					(LPTR)sz2, (LPTR)sz3 );
				Rect.top += dy;
				}
			}
		if ( Page.Type != IDC_PRINT_GRAY )
			{
			if ( !Printer.DoBlackAndUCR )
				{
				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELNOBLACKGEN );
				Rect.top += dy;
				}
			else	{
				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELBLACKGEN,
					(LPTR)Printer.BGName );
				Rect.top += dy;
				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELUCR,
					Printer.UCR, 100-Printer.UCR );
				Rect.top += dy;
				}
			if ( !Printer.DoInkCorrect )
				{
				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELNOINKCORRECT );
				Rect.top += dy;
				}
			else	{
				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELRINKCORRECT,
					bound(100+Printer.MfromR,0,100),
					bound(100-Printer.MfromR,0,100) );
				Rect.top += dy;
				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELGINKCORRECT,
					bound(100+Printer.YfromG,0,100),
					bound(100-Printer.YfromG,0,100) );
				Rect.top += dy;
				PrintLabel( hDC, Rect.left, Rect.top, Color,
					IDS_LABELBINKCORRECT,
					bound(100+Printer.CfromB,0,100),
					bound(100-Printer.CfromB,0,100) );
				Rect.top += dy;
				}
			}
		}
	}
if ( Printer.StepScale )
	{
	iHeight = 11 * ( quarter + iPenWidth );
	if ( cSep == 'K' )
		{
		Rect.left = lpRect->right + eighth + quarter + eighth;
		Rect.right = lpRect->right + eighth + quarter + eighth +quarter;
		}
	else	{
		Rect.left = lpRect->left - eighth - quarter - eighth - quarter;
		Rect.right = lpRect->left - eighth - quarter - eighth;
		}
	Rect.top = (lpRect->top + lpRect->bottom - iHeight )/2;
	for ( i=0; i<11; i++ )
		{
		Rect.bottom = Rect.top + quarter;
		if ( IntersectRect( &rDummy, &Rect, lpBandRect ) )
			{
			value = (255*i)/10;
			if ( Page.Type == IDC_PRINT_COLORSEPS )
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
			else Color = RGB(value,value,value);
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


/***********************************************************************/
int PrintLabel( hDC, x, y, Color, idStr, ... )
/***********************************************************************/
HDC hDC;
int x, y, idStr;
COLOR Color;
{
char szBuffer[1024];
LPTR lpArguments;
LPTR lpString;
int retc, length;

/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to put a variety of labels on the printed page */
if ( !AstralStr( idStr, &lpString ) )
	return( IDCANCEL );
lpArguments = (LPTR)&idStr + sizeof(idStr);
length = wvsprintf( szBuffer, lpString, lpArguments );
ColorText( hDC, x, y, szBuffer, length, Color );
return( retc );
}


/***********************************************************************/
void OutputRegMark( hDC, lpRect )
/***********************************************************************/
HDC hDC;
LPRECT lpRect;
{
RECT Rect;
int xsize, ysize, iOldROP;

Rect = *lpRect;
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

iOldROP = SetROP2( hDC, R2_NOTCOPYPEN );
xsize = RectWidth(lpRect);
ysize = RectHeight(lpRect);
InflateRect( &Rect, -xsize/5, -ysize/5 );
Ellipse( hDC, Rect.left, Rect.top, Rect.right, Rect.bottom );
MoveTo( hDC, Rect.left, (Rect.top + Rect.bottom)/2 );
LineTo( hDC, Rect.right, (Rect.top + Rect.bottom)/2 );
MoveTo( hDC, (Rect.left + Rect.right)/2, Rect.top );
LineTo( hDC, (Rect.left + Rect.right)/2, Rect.bottom );
SetROP2( hDC, iOldROP );
}


/***********************************************************************
 ABORTTEST is passed to gdi to allow the spooler to call it during
 spooling to let the application cancel the print job or to handle
 out-of-disk-space conditions.  All it does is look in the message
 queue and see if the next message is for the abort dialog box, and
 if it is it sends it to the dialog box, and if it is not it is
 translated and dispatched like normal.
***********************************************************************/

/***********************************************************************/
BOOL FAR PASCAL AbortTest( hDC, iCode )
/***********************************************************************/
HDC	hDC;
int	iCode;
{
MSG	msg;

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
} /* end aborttest */


/***********************************************************************
 DLGFNABORT controls the modeless Abort dialog box.  It is called
 whenever a print job is sent to the spooler.
***********************************************************************/

/***********************************************************************/
int FAR PASCAL DlgFnAbort (hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	hDlg;
unsigned msg;
WORD	wParam;
LONG	lParam;
{
switch (msg)
    {
    case WM_INITDIALOG:
	SetWindowText(hDlg, szAppName);
	SetDlgItemText(hDlg, ID_NAME, szFilename);
	SetDlgItemText(hDlg, ID_PRINTER, pchDevice);
	SetDlgItemText(hDlg, ID_PORT, pchPort);
	CenterPopup(hDlg);
	break;

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDCANCEL:
		fAbortPrint = TRUE;
		EnableWindow(GetParent(hDlg), TRUE);
		DestroyWindow(hDlg);
		hWndAbort = NULL;
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    } /* end case */

return( TRUE );
} /* end DlgFnAbort */


/***********************************************************************
 GETPRINTERDC reads the device string line from WIN.INI and trys to
 create a DC for  the device.  It rejects all DCs incapable of BITBLT. 
 It returns a DC or NULL if there is an error.
***********************************************************************/

/***********************************************************************/
HDC GetPrinterDC()
/***********************************************************************/
{
PTR	pch;
HDC	hDC;

GetProfileString( "Windows", "Device", NULL, szProfile, sizeof(szProfile));
if (*szProfile == NULL)
	{
	Message( IDS_EBADDEVICE );
	return ((HANDLE)NULL);
	}

pch = szProfile;
pchDevice = pch;
pchDriver = pch;
pchPort   = pch;

/* skip leading blanks */
while (*pch != NULL && *pch == ' ')
	pch = (PTR) AnsiNext(pch);

pchDevice = pch;

/* find end of device name */
while (*pch != NULL && *pch != ',')
	pch = (PTR) AnsiNext(pch);

/* if there is no device, we can't output */
if (*pch == NULL)
	{
	Message( IDS_EBADDEVICE );
	return ((HANDLE)NULL);
	}

/* null terminate driver */
*pch++ = NULL;

/* skip leading blanks */
while (*pch != NULL && *pch == ' ')
	{
	if (*pch == '.')
		*pch = NULL;
	pch = (PTR) AnsiNext(pch);
	}

pchDriver = pch;

/* find end of driver name */
while (*pch != NULL && *pch != ',')
	pch = (PTR) AnsiNext(pch);

/* if the is no driver for device, we can't output */
if (*pch == NULL)
	{
	Message( IDS_EBADDEVICE );
	return ((HANDLE)NULL);
	}

/* null terminate device */
*pch++ = NULL;

/* skip leading blanks */
while (*pch != NULL && *pch == ' ')
	pch = (PTR) AnsiNext(pch);

pchPort = pch;

/* Create the DC. */
hDC = CreateDC( pchDriver, pchDevice, pchPort, NULL);

if (hDC != (HDC) NULL)
	{
	/* if the DC cannot do DIBTODEV (BITBLT) ... */
//	if ( !(GetDeviceCaps(hDC, RASTERCAPS) & RC_DIBTODEV) )
	if ( !(GetDeviceCaps(hDC, RASTERCAPS) & RC_BITBLT) )
		{
		DeleteDC(hDC);
		hDC = (HDC) NULL;
		}
	}

return( hDC );
}


/***********************************************************************/
void PrintErr(hWnd, ierr)
/***********************************************************************/
HWND	hWnd;
int	ierr;
{
int	ids;

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
} /* end printerr */


typedef struct _laser
	{
	int	nCount;
	char	cData[2*MAX_IMAGE_LINE];
	} LASER;
typedef LASER far *LPLASER;


/***********************************************************************/
int LaserPalette( lpPalette, nSize )
/***********************************************************************/
LPTR	lpPalette;
int	nSize;
{
int	ret;
LPLASER	lpLaser;

lpLaser = (LPLASER)LineBuffer[0];
ret = wsprintf( lpLaser->cData, "\033$p256T", nSize );
lmemcpy( &(lpLaser->cData[ret]), lpPalette, nSize );
lpLaser->nCount = ret + nSize;
ret = Escape( hPrinterDC, DEVICEDATA, lpLaser->nCount, (LPTR)lpLaser, NULL );
return( ret );
}


/***********************************************************************/
int AstralLaserPrint( xSrc,  ySrc,  dxSrc,  dySrc,
		xDest, yDest, dxDest, dyDest, iPrResX, iPrResY )
/***********************************************************************/
int xSrc, ySrc, dxSrc, dySrc;
int xDest, yDest, dxDest, dyDest, iPrResX, iPrResY;
{
/* For the laser board... */
int	y, yline, ystart, ylast, err;
FIXED	yrate, yoffset;
STRING	szFixed;
LPTR	lpBits;
LPLASER	lpLaser;
int	start;

/*Print( "source=(%d,%d), dest=(%d,%d), size=(%d,%d)",
	dxSrc, dySrc, xDest, yDest, dxDest, dyDest );*/

/* Send the style (1=line, 2=diam), freq, angle(0,45,90) */
if ( (err = LaserPrint( "\033$s%ds%df%dA", 2,
   HIWORD(Printer.ScreenRuling[0]), HIWORD(Printer.ScreenAngle[0]) )) < 0 )
		return( err );

/* Send the image palette (lut) */
// if ( Printer.DoCalibration )
//	if ( (err = LaserPalette( Printer.CalMap[0].Lut,
//	     sizeof(Printer.CalMap[0].Lut) )) < 0 )
//		return( err );

/* Send the destination (x,y) and size (w,h) 1/1200" */
if ( (err = LaserPrint( "\033$d%dx%dy%dw%dH",
    LX(xDest),  LY(yDest), LX(dxDest), LY(dyDest) )) < 0 )
	return( err );

/* Let the printer do any upsizing */
if ( dySrc < dyDest )
	{
	yrate = UNITY;
	dxDest = dxSrc;
	dyDest = dySrc;
	}
else	yrate = FGET( dySrc, dyDest );

/* Send the inline image's size and packing (bpp,ppb,w,h) */
if ( (err = LaserPrint( "\033$i%db%dp%dw%dH", 8, 1, dxDest, dyDest )) < 0 )
	return( err );

lpLaser = (LPLASER)LineBuffer[0];
start = wsprintf( lpLaser->cData, "\033$i%dT", dxDest /*1 for dptek*/ );
lpLaser->nCount = start + dxDest;
lpBits = &(lpLaser->cData[start]);

ystart = ySrc;
yline = -1;
yoffset = yrate>>1;
for (y=0; y<dyDest; y++)
	{
	AstralClockCursor( y, dyDest );

	/* Check for user input to abort dialog box */
	(*lpAbortTest)((HDC)hPrinterDC, 0);
	if ( fAbortPrint )
		break;

	ylast = yline;
	yline = ystart + HIWORD( yoffset );
	yoffset += yrate;
	if ( yline != ylast )
		{
		if ( !readimage( yline, xSrc, xSrc+dxSrc-1,
		    dxDest, lpBits ) )
			return( -1 ); /* Error reading from image file */
		if ( Printer.DoCalibration )
			map( Printer.CalMap[0].Lut, lpBits, dxDest );
		}

	/* Send raw data to the Intel or DPTek grayscale board */
	if ( (err = Escape( hPrinterDC, DEVICEDATA, lpLaser->nCount,
	    (LPTR)lpLaser, 0 )) < 0 )
		return( err );
	}
return( 0 );
}


/***********************************************************************/
int LaserPrint( lpFormat, ... )
/***********************************************************************/
LPTR	lpFormat;
{
LPTR	lpArguments;
int	ret;
LPLASER	lpLaser;

lpLaser = (LPLASER)LineBuffer[0];
/* Send short escape sequences to the Intel or DPTek grayscale */
/* board with arguments specified exactly like printf */
lpArguments = (LPTR)&lpFormat + sizeof(lpFormat);
lpLaser->nCount = wvsprintf( lpLaser->cData, lpFormat, lpArguments );
ret = Escape( hPrinterDC, DEVICEDATA, lpLaser->nCount, (LPTR)lpLaser, NULL );
return( ret );
}


#ifdef UNUSED
/***********************************************************************/
BOOL AstralCreateBitmap( hDC, dxDest, dyDest, xSrc, ySrc,
			 dxSrc, dySrc, iStyle,
			 lpLut, yTop, yTotal )
/***********************************************************************/
/* This creates bitmaps for all of the image repaint routines		*/
/* which use them for both display painting and painting to the printer */

HDC	hDC;
int	dxDest, dyDest, xSrc, ySrc, dxSrc, dySrc, iStyle, yTop, yTotal;
LPTR	lpLut;
{
int	y, yline, ystart, ylast, bpl, i, iDevplanes, iDevbpp;
BYTE	nPlanes, BitsPerPixel;
FIXED	yrate, yoffset;
long	bytes, z;
LPTR	lpBits, lpImageOut, lpOut, lpGreen;
BYTE	Red[256], Green[256], Blue[256];
RGBS	TheRGB;

if ( dxDest <= 0 || dyDest <= 0 )
	return( TRUE );

iDevplanes = GetDeviceCaps(hDC, PLANES);
iDevbpp = GetDeviceCaps(hDC, BITSPIXEL);
nPlanes = 1;
if ( DEPTH == 2 && iDevbpp == 1 && iStyle == 1 )
	{
	if ( iDevplanes == 4 )
		nPlanes = 4;
	else
	if ( iDevplanes == 3 )
		nPlanes = 3;
	}

//if ( lpLut )
//	Print("planes=%d, bpp=%d", iDevplanes, iDevbpp );

// If memory is low, and its not for printing, don't try to do color
if ( AvailableMemory() < (400L * 1024L) && !lpLut )
	nPlanes = 1;

switch ( iStyle )
    {
    case 1:
	BitsPerPixel = 1;
   	break;
    case 2:
	BitsPerPixel = 1;
   	break;
    case 8:
	BitsPerPixel = 8;
   	break;
    default:
	return( FALSE );
    }

/* Compute how many bytes to allocate for the dithered bitmap */
/* Round to the nearest 16 byte boundary */
bpl = ( ((dxDest+0xF) & 0xFFF0) * BitsPerPixel ) / 8;
bytes = (long)dyDest * bpl;
if ( nPlanes > 1 )
	{
	z = bytes;
	bytes *= 4L;
	bytes += bpl;	/* add some extra */
	}

if ( DEPTH > 1 )
	{
	for ( i=0; i<218; i++ )
		{
		// Just temporary
		Red[i]  = Green[i] = Blue[i]  = i;
//		GetRGBColor( YES, i, &TheRGB );
//		if ( lpLut )
//			{
//			Red[i]   = lpLut[ TheRGB.red ];
//			Green[i] = lpLut[ TheRGB.green ];
//			Blue[i]  = lpLut[ TheRGB.blue ];
//			}
//		else	{
//			Red[i]   = TheRGB.red;
//			Green[i] = TheRGB.green;
//			Blue[i]  = TheRGB.blue;
//			}
		}
	}

/* Allocate the memory from the global heap */
if ( !(lpImageOut = Alloc( bytes )) )
	{
	Message( IDS_EIMGMEMORY, bytes );
	return( FALSE );
	}

/* Setup the output line pointer */
lpOut = lpImageOut;

/* Setup the input line pointer */
if ( !AllocLines( &lpBits, 1, dxDest, DEPTH ) )
	{
	FreeUp( lpImageOut );
	return( FALSE );
	}

yrate = FGET( dySrc, dyDest );
ystart = ySrc;
yline = -1;
yoffset = yrate>>1;
for (y=0; y<dyDest; y++)
	{
	AstralClockCursor( yTop+y, yTotal );
	ylast = yline;
	yline = ystart + HIWORD( yoffset );
	yoffset += yrate;
	if ( yline != ylast )
		{
		if ( !readimage( yline, xSrc, xSrc+dxSrc-1, dxDest, lpBits))
			break; /* Error reading from image file */
		if ( DEPTH > 1 )
			frame_display( lpBits, dxDest );
		else
		if ( lpLut && iStyle != 1 )
			map( lpLut, lpBits, dxDest );
		}

	if ( iStyle == 1 )
		{
		if ( DEPTH > 1 )
		    {
		if ( nPlanes == 4 )
			{
			 diffuse(0,yTop+y,1,Red,lpBits,dxDest,lpOut);
			 lpOut = BumpPtr( lpOut, (long)bpl );
			 diffuse(0,yTop+y,2,Green,lpBits,dxDest,lpOut);
			 lpGreen = lpOut;
			 lpOut = BumpPtr( lpOut, (long)bpl );
			 diffuse(0,yTop+y,3,Blue,lpBits,dxDest,lpOut);
			 lpOut = BumpPtr( lpOut, (long)bpl );
			 copy( lpGreen, lpOut, bpl );
			 lpOut = BumpPtr( lpOut, (long)bpl );
			}
		else
			{
			if ( nPlanes == 3 )
			  {
			   diffuse(0,yTop+y,1,Red,lpBits,dxDest,
				lpOut );
			   diffuse(0,yTop+y,2,Green,lpBits,dxDest,
				BumpPtr( lpOut, z ) );
			   diffuse(0,yTop+y,3,Blue,lpBits,dxDest,
				BumpPtr( lpOut, 2*z ) );
			   lpOut = BumpPtr( lpOut, (long)bpl );
			  }
			else
			  {
			   /* use the green value */
			   diffuse(0,yTop+y,0,Green,lpBits,dxDest,lpOut);
			   lpOut = BumpPtr( lpOut, (long)bpl );
			  }
			}
		    }
		else
		  {
		   diffuse( 0, yTop+y, 0, lpLut, lpBits, dxDest, lpOut );
		   lpOut = BumpPtr( lpOut, (long)bpl );
		  }
		}
	else
	if ( iStyle == 2 )
		{
		dither( 0, yTop+y, lpBits, dxDest, lpOut );
		lpOut = BumpPtr( lpOut, (long)bpl );
		}
	else
	if ( iStyle == 8 )
		{
		lmemcpy( lpOut, lpBits, dxDest );
		lpOut = BumpPtr( lpOut, (long)bpl );
		}
	}

DeleteImageBitmap();
/* Create the Windows bitmap */
if ( !(Window.hBitmap = CreateBitmap( dxDest, dyDest,
	nPlanes, BitsPerPixel, lpImageOut ) ) )
		Message( IDS_EIMGBITMAP );

FreeUp( lpImageOut );
FreeUp( lpBits );
return( Window.hBitmap != NULL );
}
#endif
