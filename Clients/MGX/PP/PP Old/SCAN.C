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
#include "scan.h"

extern HWND	hWndAstral;
extern HANDLE	hInstAstral;
extern SCINIT	SCinit;
extern SCENV	SCenv;
extern SCATT	SCatt;

#define UsingLineBuffer 1

/***********************************************************************/
BOOL Scanner()
/***********************************************************************/
{
SCATT TempAtt;
SCMSG SCmsg;
int ret;
DWORD Selector;

Selector = GlobalDosAlloc(1024L);
SCinit.DOSMemory  = (LPTR)(Selector & 0xffff0000L);
Selector &= 0x0000ffffL;

ret = FALSE;

/* Check to see if the scanner driver is installed and ready */
/* No communications with the hardware is attempted here */
if ( !ScanOpen() )
	{
	ScanCall( SCANMSG, (LPTR)&SCmsg );
	Message( IDS_ESCANOPEN, (LPTR)SCmsg.Message );
	return( FALSE );
	}

/* Check the scanner hardware and communications */
/* Make sure the scanner is turned on, connected, and available */
if ( !ScanCall( SCANINIT, (LPTR)&SCinit ) )
	{
	ScanCall( SCANMSG, (LPTR)&SCmsg );
	Message( IDS_ESCANINIT, (LPTR)SCmsg.Message );
	goto ErrorExit;
	}

/* Get the scanner environment */
if ( !ScanCall( SCANENV, (LPTR)&SCenv ) )
	{
	ScanCall( SCANMSG, (LPTR)&SCmsg );
	Message( IDS_ESCANENV, (LPTR)SCmsg.Message );
	goto ErrorExit;
	}

/*Print("Env OK: company=%ls, port=%d, bits=0x%x, minlen=%d, maxlen=%d, maxwide=%d, chan=0x%x",
	(LPTR)SCenv.Company, SCenv.PortAddr,
	SCenv.ScanBits, SCenv.MinScanLength,
	SCenv.MaxScanLength, SCenv.MaxScanWidth, SCenv.DataChannel );*/

/* Get the scanner attributes into a temporary structure */
/* Just to see if it works OK */
if ( !ScanCall( SCANGETATT, (LPTR)&TempAtt ) )
	{
	ScanCall( SCANMSG, (LPTR)&SCmsg );
	Message( IDS_ESCANGETATT, (LPTR)SCmsg.Message);
	goto ErrorExit;
	}

/*Print("GetAtt1: bits=%d, res=%d, frame.l=%d, frame.t=%d, frame.r=%d, frame.b=%d, dma=%d, speed=%d",
	TempAtt.BitsPerPixel, TempAtt.Resolution, TempAtt.Frame.left,
	TempAtt.Frame.top, TempAtt.Frame.right, TempAtt.Frame.bottom,
	TempAtt.UseDMA, TempAtt.ScanSpeed );*/

/* Set the scanner attributes from our stored settings */
if ( !ScanCall( SCANSETATT, (LPTR)&SCatt ) )
	{
	ScanCall( SCANMSG, (LPTR)&SCmsg );
	Message( IDS_ESCANSETATT, (LPTR)SCmsg.Message);
//	goto ErrorExit;
	}

/* Get the scanner attributes again; they should be what you set */
if ( !ScanCall( SCANGETATT, (LPTR)&SCatt ) )
	{
	ScanCall( SCANMSG, (LPTR)&SCmsg );
	Message( IDS_ESCANGETATT, (LPTR)SCmsg.Message);
	goto ErrorExit;
	}

/*Print("GetAtt2: bits=%d, res=%d, frame.l=%d, frame.t=%d, frame.r=%d, frame.b=%d, dma=%d, speed=%d",
	SCatt.BitsPerPixel, SCatt.Resolution, SCatt.Frame.left,
	SCatt.Frame.top, SCatt.Frame.right, SCatt.Frame.bottom, SCatt.UseDMA,
	SCatt.ScanSpeed );*/

/* Bring up dialog box to control scanning */
/* User modifies values in the dialog box then hits SCAN, PRESCAN, or TEST */
ret = AstralDlg( NO, hInstAstral, hWndAstral, IDD_SCAN, DlgScanProc);

ErrorExit:

if ( Selector )
	GlobalDosFree( (WORD)Selector );
if ( !ScanClose() )
	{
	ScanCall( SCANMSG, (LPTR)&SCmsg );
	Message( IDS_ESCANCLOSE, (LPTR)SCmsg.Message);
	}

return( ret );
}


/***********************************************************************/
BOOL DoScan( iPercentage )
/***********************************************************************/
int	iPercentage;
{
long	lScanBufferSize, limit;
int	i, ret, bytes, lines, Resolution, LinesPerCall, nPlanes, npix, nlin, dyplane, yscans, last_plane, plane, depth;
LPTR	lpImage, lpDOSImage, lpOut, lpBuffer;
SCSTART SCstart;
SCGET	SCget;
SCMSG	SCmsg;
SPAN	span;
LPFRAME	lpFrame, lpOldFrame, lpNewFrame;

lpOldFrame = frame_set( NULL );
lpFrame = NULL;
lpImage = NULL;
lpBuffer = NULL;

/* Temporarily change scanning resolution to accomplish the desired scaling */
Resolution = SCatt.Resolution;
SCatt.Resolution = (((long)SCatt.Resolution * iPercentage)+50) / 100;
// If scanning lineart, don't do calibration
if ( !Scans.DoCalibration || SCatt.BitsPerPixel == 1 )
	SCatt.Lut = SCatt.RLut = SCatt.GLut = SCatt.BLut = NULL;
else	{
	SCatt.Lut  = Scans.CalMap[0].Lut;
	SCatt.RLut = Scans.CalMap[1].Lut;
	SCatt.GLut = Scans.CalMap[2].Lut;
	SCatt.BLut = Scans.CalMap[3].Lut;
	}

/* Set the scanner attributes from the user's settings */
if ( !ScanCall( SCANSETATT, (LPTR)&SCatt ) )
	{
	ScanCall( SCANMSG, (LPTR)&SCmsg );
	Message( IDS_ESCANSETATT, (LPTR)SCmsg.Message);
	/* Restore the original resolution */
	SCatt.Resolution = Resolution;
	return( FALSE );
	}

/* Restore the original resolution */
SCatt.Resolution = Resolution;

if ( !ScanCall( SCANSTART, (LPTR)&SCstart) )
	{
	ScanCall( SCANMSG, (LPTR)&SCmsg );
	Message( IDS_ESCANSTART, (LPTR)SCmsg.Message);
	return( FALSE );
	}

/* Start the scan and get back the number of bytes per line */
npix = bytes = SCstart.ScanLength;
nlin = lines = SCstart.ScanLines;
if ( bytes <= 0 || lines <= 0 )
	{
	Message( IDS_ESCANCROP );
	goto ErrorExit;
	}

nPlanes = 1;
depth = 1;
if ( SCatt.ColorOption)
	{
	depth = 3;
	if (SCenv.ScanBits & 8) /* long color */
		{
		nlin /= 3;
		nPlanes = 3;
		}
	else			/* wide color */
		npix /= 3;
	}
else if ( SCatt.BitsPerPixel == 1 )
	npix *= 8;

/* Recompute "real" resolution so the user gets the right height and width */
if ( npix < nlin )
	Resolution  = ( TOFIXED(nlin) + Edit.Height/2 ) / Edit.Height;
else	Resolution  = ( TOFIXED(npix) + Edit.Width/2 ) / Edit.Width;

if ( abs(Resolution - SCatt.Resolution) < 10 )
	Resolution = SCatt.Resolution; // If it's close enough, give it to him

if ( !(lpFrame = frame_open( depth, npix, nlin, Resolution )) )
	{
	Message( IDS_ESCROPEN, (LPTR)Control.RamDisk );
	goto ErrorExit;
	}

frame_set( lpFrame );
span.sx = 0;
span.sy = 0;
span.dx = npix;
span.dy = 1;

if (SCatt.BitsPerPixel == 1)
	{
	if ( !(lpBuffer = Alloc( (long)npix)))
		{
		Message( IDS_ESCANMEM );
		goto ErrorExit;
		}
	}

/* Allocate as much memory as possible for the scan buffer (up to a limit) */
if ( UsingLineBuffer )
	{
	lScanBufferSize = 65536L;
	lpImage = Control.lpSelector;
	lpDOSImage = Control.lpSegment;
	}
else	{
	lScanBufferSize = AvailableMemory();
	limit = (long)bytes * lines; /* don't need more than the whole image */
	if ( limit > 65536L ) limit = 65536L; /* can't handle more than 64K */
	if ( lScanBufferSize > limit ) /* If its too much... */
		lScanBufferSize = limit;
	if ( !(lpImage = Alloc( lScanBufferSize )) )
		{
		Message( IDS_ESCANMEM );
		goto ErrorExit;
		}
	lpDOSImage = lpImage;
	}

/* Compute how many lines there are in each band (each SCANIMAGE call) */
SCget.LinesLeft = lines;
LinesPerCall = lScanBufferSize / bytes;

yscans = 0;
dyplane = lines/nPlanes;
last_plane = -1;

/* The main scanning loop */
AstralClockCursor( 0, lines );
while( SCget.LinesLeft > 0 )
	{
	SCget.ImageAddress = lpImage;
	SCget.DOSImageAddress = lpDOSImage;
	SCget.ScanLines = LinesPerCall;

	/* Correct the band size if there aren't enough lines left */
	if ( SCget.ScanLines > SCget.LinesLeft )
		SCget.ScanLines = SCget.LinesLeft;

	/* Get the scanned data */
	if ( !ScanCall( SCANIMAGE, (LPTR)&SCget ) )
		{
		ScanCall( SCANMSG, (LPTR)&SCmsg );
		Message( IDS_ESCANIMAGE, (LPTR)SCmsg.Message );
		break;
		}

	if ( !SCget.ScanLines )
		break;

	/* Write the image data into the cache */
	for ( i=0; i<SCget.ScanLines; i++ )
		{
		AstralClockCursor( yscans, lines );
		plane = yscans/dyplane;
		if (plane != last_plane)
		    span.sy = 0;
		last_plane = plane;
		if ( SCatt.ColorOption && SCenv.ScanBits & 8)
			{
			lpOut = frame_ptr(0, 0, span.sy, YES);
			if (lpOut)
				SetEachThirdValue(SCget.ImageAddress,
					lpOut+plane, npix );
			ret = lpOut != NULL;
			}
		else
		if ( SCatt.BitsPerPixel == 8 )
			ret = frame_write( &span, SCget.ImageAddress, npix );
		else	{
			la2con( SCget.ImageAddress, npix, lpBuffer, NO );
			ret = frame_write( &span, lpBuffer, npix );
			}
		if ( !ret )
			{
			Message( IDS_ESCANWRITE, (LPTR)Control.RamDisk);
			goto ErrorExit;
			}
		span.sy++;
		yscans++;
		SCget.ImageAddress += bytes;
		}
	}

/* Scan completed successfully */
ScanCall( SCANABORT, NULL );
if ( !UsingLineBuffer )
	FreeUp( lpImage );
if (lpBuffer)
	FreeUp( lpBuffer);

/* Setup the new image and bring up the new image window */
NewImageWindow( NULL, NULL, lpFrame, IDC_SAVETIFF, (SCatt.BitsPerPixel == 1), FALSE, IMG_DOCUMENT, NULL );

return( TRUE );

ErrorExit:

/* Something went wrong; abort the scan */
ScanCall( SCANABORT, NULL );
if ( !UsingLineBuffer )
    if ( lpImage )
	FreeUp( lpImage );
if (lpBuffer)
	FreeUp( lpBuffer);

/* Close the new frame */
if ( lpFrame )
	frame_close( lpFrame );

/* Activate the old frame */
frame_set( lpOldFrame );
return( FALSE );
}


/***********************************************************************/
BOOL DoPreScan( hDlg, bCheck )
/***********************************************************************/
HWND	hDlg;
BOOL	bCheck;
{
long	lScanBufferSize, limit;
int	bytes, lines, y, Levels, MaxGray, MinGray, i, fy;
int	width, height, yscans, npix, nlin, plane, nPlanes, yplane, dyplane, last_plane;
LPTR	lpImage, lpDOSImage, lpScanBuffer, lpOut, lpBuffer;
SCSTART SCstart;
SCGET	SCget;
SCMSG	SCmsg;
SCATT	PreAtt;
RECT	PlatenRect;
FIXED	scale;
long	Histogram[256];
LPFRAME lpOldFrame;
SPAN 	span;
FIXED yrate, yoffset, xrate;
int ystart;

lpOldFrame = frame_set(NULL);

AstralControlRect( hDlg, IDC_SCANFRAME, &PlatenRect );
width = RectWidth( &PlatenRect );
height = RectHeight( &PlatenRect );
scale = ScaleToFit( &width, &height, SCenv.MaxScanWidth, SCenv.MaxScanLength );

/* Set the scanner attributes from the user's settings */
copy( (LPTR)&SCatt, (LPTR)&PreAtt, sizeof(SCATT) );
if (bCheck)
	{
	PreAtt.BitsPerPixel = 8;
	PreAtt.ColorOption = 0;
	}
PreAtt.Mirror = NO;
PreAtt.Invert = NO;
PreAtt.Contrast = 0;
PreAtt.Brightness = 0;
PreAtt.ScanSpeed = 5; /* The fastest speed */
PreAtt.Resolution = FMUL( 1000, scale );
PreAtt.Lut = NULL;
if ( !bCheck )
	{
	PreAtt.Frame.left = 0;
	PreAtt.Frame.right = SCenv.MaxScanWidth - 1;
	PreAtt.Frame.top = 0;
	PreAtt.Frame.bottom = SCenv.MaxScanLength - 1;
	}
else	clr( (LPTR)Histogram, sizeof(Histogram) );
// If scanning lineart, don't do calibration
if ( !Scans.DoCalibration || PreAtt.BitsPerPixel == 1 )
	PreAtt.Lut = PreAtt.RLut = PreAtt.GLut = PreAtt.BLut = NULL;
else	{
	PreAtt.Lut  = Scans.CalMap[0].Lut;
	PreAtt.RLut = Scans.CalMap[1].Lut;
	PreAtt.GLut = Scans.CalMap[2].Lut;
	PreAtt.BLut = Scans.CalMap[3].Lut;
	}

if ( !ScanCall( SCANSETATT, (LPTR)&PreAtt ) )
	{
	ScanCall( SCANMSG, (LPTR)&SCmsg );
	Message( IDS_ESCANSETATT, (LPTR)SCmsg.Message);
//	return( FALSE );
	}

/* Start the scan and get back the number of bytes per line */
if ( !ScanCall( SCANSTART, (LPTR)&SCstart) )
	{
	ScanCall( SCANMSG, (LPTR)&SCmsg );
	Message( IDS_ESCANSTART, (LPTR)SCmsg.Message);
	return( FALSE );
	}

/* Note: It may not be exactly what we want - they may have min resolution */
npix = bytes = SCstart.ScanLength;
nlin = lines = SCstart.ScanLines;

lpBuffer = NULL;
if ( Scans.lpScanFrame )
	frame_close( Scans.lpScanFrame );
Scans.lpScanFrame = NULL;
nPlanes = 1;
if (PreAtt.ColorOption)
	{
	if (SCenv.ScanBits & 8)
		{
		nPlanes = 3;
		nlin /= 3;
		/* We will only downsize to fit the scan frame - no upsizing */
		width = min(npix, RectWidth(&PlatenRect));
		height = min(nlin, RectHeight(&PlatenRect));
		if (!AllocLines(&lpBuffer, 1, width, 3))
			{
			Message( IDS_ESCANMEM );
			ScanCall( SCANABORT, NULL );
			return( FALSE );
			}
		}
	else	{
		npix /= 3;
		/* We will only downsize to fit the scan frame - no upsizing */
		width = min(npix, RectWidth(&PlatenRect));
		height = min(nlin, RectHeight(&PlatenRect));
		}
	Scans.lpScanFrame = frame_open(3, width, height, PreAtt.Resolution);
	}
else	{
	if (PreAtt.BitsPerPixel == 1)
		{
		npix *= 8;
		if (!AllocLines(&lpBuffer, 1, npix, 1))
			{
			Message( IDS_ESCANMEM );
			ScanCall( SCANABORT, NULL );
			return( FALSE );
			}
		}
	/* We will only downsize to fit the scan frame - no upsizing */
	width = min(npix, RectWidth(&PlatenRect));
	height = min(nlin, RectHeight(&PlatenRect));
	Scans.lpScanFrame = frame_open(1, width, height, PreAtt.Resolution);
	}
if (!Scans.lpScanFrame)
	{
	Message( IDS_ESCANMEM );
	ScanCall( SCANABORT, NULL );
	if (lpBuffer)
		FreeUp(lpBuffer);
	return( FALSE );
	}

// Print("bytes=%d, width=%d, lines=%d, height=%d, bpl=%d",
//	bytes, width, lines, height, bpl );

if ( bytes <= 0 || lines <= 0 )
	{
	Message( IDS_ESCANCROP );
	ScanCall( SCANABORT, NULL );
	if (lpBuffer)
		FreeUp(lpBuffer);
	return( FALSE );
	}

/* Allocate as much memory as possible for the scan buffer (up to a limit) */
if ( UsingLineBuffer )
	{
	lScanBufferSize = 65536L;
	lpImage = Control.lpSelector;
	lpDOSImage = Control.lpSegment;
	}
else	{
	lScanBufferSize = AvailableMemory();
	limit = (long)bytes * lines; /* don't need more than the whole image */
	if ( limit > 65536L ) limit = 65536L; /* can't handle more than 64K */
	if ( lScanBufferSize > limit ) /* If its too much... */
		lScanBufferSize = limit;
	if ( !(lpImage = Alloc( lScanBufferSize )) )
		{
		Message( IDS_ESCANMEM );
		ScanCall( SCANABORT, NULL );
		if (lpBuffer)
		    FreeUp(lpBuffer);
		return( FALSE );
		}
	lpDOSImage = lpImage;
	}

lpScanBuffer = lpImage;

/* Make sure we have enough for the entire prescan bitmap */
//limit = (long)bpl * height + 2*bytes;
/* Make sure we have enough for one scan line */
limit = bytes;
if ( lScanBufferSize < limit )
	{
	if ( UsingLineBuffer )
		FreeUp( lpScanBuffer );
	Message( IDS_ESCANMEM );
	ScanCall( SCANABORT, NULL );
	if (lpBuffer)
		FreeUp(lpBuffer);
	return( FALSE );
	}

y = 0;
xrate = FGET( npix, width );
yrate = FGET( nlin, height );
yscans = -1;
dyplane = lines / nPlanes;
SCget.LinesLeft = lines;

/* The main scanning loop */
AstralClockCursor( 0, lines );
frame_set(Scans.lpScanFrame);
span.dx = width;
span.dy = 1;
span.sx = 0;
span.sy = 0;
last_plane = -1;
while( SCget.LinesLeft > 0 )
	{
	SCget.ImageAddress = lpImage;
	SCget.DOSImageAddress = lpDOSImage;
	SCget.ScanLines = lScanBufferSize / bytes;
	/* Correct the band size if there aren't enough lines left */
	if ( SCget.ScanLines > SCget.LinesLeft )
		SCget.ScanLines = SCget.LinesLeft;

	/* Get the scanned data */
	if ( !ScanCall( SCANIMAGE, (LPTR)&SCget ) )
		{
		ScanCall( SCANMSG, (LPTR)&SCmsg );
		Message( IDS_ESCANIMAGE, (LPTR)SCmsg.Message );
		break;
		}

	if ( !SCget.ScanLines )
		break;

	for ( i=0; i<SCget.ScanLines; i++ )
		{
		AstralClockCursor( yscans, lines );
		yscans++;
		plane = yscans/dyplane;
		if (plane != last_plane)
		    {
		    yoffset = yrate >> 1;
		    span.sy = 0;
		    }
		last_plane = plane;
		yplane = yscans - (plane*dyplane);
		if ( bCheck )
			hist( SCget.ImageAddress, bytes, Histogram );
		else if (yplane >= HIWORD(yoffset))
			{
			yoffset += yrate;
			if (PreAtt.ColorOption && SCenv.ScanBits & 8)
				{
				sample8(SCget.ImageAddress, 0, lpBuffer, 0, width, xrate);
				lpOut = frame_ptr(0, 0, span.sy, YES);
				if (lpOut)
					SetEachThirdValue(lpBuffer, lpOut+plane, width );
				}
			else if (PreAtt.BitsPerPixel == 1)
				{
				la2con(SCget.ImageAddress, npix, lpBuffer, NO);
				frame_write(&span, lpBuffer, npix);  
				}
			else
				frame_write(&span, SCget.ImageAddress, npix);  
			++span.sy;
			}
		SCget.ImageAddress += bytes;
		}
	}

/* Scan completed successfully but send an abort anyway */
ScanCall( SCANABORT, NULL );

if ( !bCheck )
	AstralControlPaint( hDlg, IDC_SCANFRAME );
else	{
	Levels = 0;
	MinGray = 256;
	MaxGray = 0;
	for ( i=0; i<256; i++ )
		{
		if ( !Histogram[i] )
			continue;
		Levels++;
		if ( i < MinGray ) MinGray = i;
		if ( i > MaxGray ) MaxGray = i;
		}
	Message( IDS_SCANCHECK, Levels, MaxGray, MinGray );
	}

if ( !UsingLineBuffer )
	FreeUp( lpScanBuffer );
if (lpBuffer)
	FreeUp( lpBuffer );
/* Return to the previous scanning attributes */
if ( !ScanCall( SCANSETATT, (LPTR)&SCatt ) )
	{
	ScanCall( SCANMSG, (LPTR)&SCmsg );
	Message( IDS_ESCANSETATT, (LPTR)SCmsg.Message);
//	return( FALSE );
	}

frame_set(lpOldFrame);
return( Scans.lpScanFrame != NULL );
}


/***********************************************************************/
long FAR PASCAL FrameControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
LONG	   lParam;
{
int dim, cx, cy;
LPRECT lpRect;
LPTR lpNewCursor;
RECT InRect;
static FIXED scale;
static BOOL bTrack;
static RECT Rect, SelectRect;
static LPTR lpCursor;

switch ( message )
    {
    case WM_PAINT:
	ScanImagePaint( hWindow, &SelectRect );
	break;

    case WM_LBUTTONDOWN:
	if ( bTrack )
		break;
	bTrack = TRUE;
	StartSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect,
		(wParam & MK_SHIFT) ? SL_BOXHANDLES | SL_SPECIAL : SL_BOXHANDLES );
	break;

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	bTrack = FALSE;
	EndSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect, SL_BOXHANDLES );
	CopyRect( &Rect, &SelectRect );
	/* Send the parent the selection rectangle in 1/1000's of an inch */
	ScaleRect( &Rect, FGET( UNITY, scale ) ); /* invert the scale */
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), (long)(LPTR)&Rect );
	SetFocus( GetParent(hWindow) );
	break;

    case WM_MOUSEMOVE:
	CopyRect( &InRect, &SelectRect );
	InflateRect(&InRect, -(CLOSENESS), -(CLOSENESS));
	if (PtInRect(&InRect, MAKEPOINT(lParam)))
		lpNewCursor = IDC_SIZE;
	else 
		lpNewCursor = IDC_ARROW;
//		lpNewCursor = WhichEdgeCursor( MAKEPOINT(lParam), &SelectRect //);
	if ( lpCursor != lpNewCursor )
		{
		lpCursor = lpNewCursor;
		SetClassWord(hWindow, GCW_HCURSOR, LoadCursor( NULL, lpCursor ) );
		}
	if ( !bTrack )
		break;
	UpdateSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect,
		SL_BOXHANDLES );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam )
	    {
	    case 1:	/* Message to initialize the bounding rectangle */
			/* Caller passes a rect pointer in 1/1000th inches */
/* JIM */	GetWindowRect( hWindow, &Rect );
		cx = RectWidth( &Rect );
		cy = RectHeight( &Rect );
		scale = ScaleToFit( &cx, &cy, LOWORD(lParam), HIWORD(lParam) );
//		GetWindowRect( hWindow, &Rect );
		/* Convert control's coordinates to be relative to parent */
		ScreenToClient( GetParent(hWindow), (LPPOINT)&Rect.left );
		ScreenToClient( GetParent(hWindow), (LPPOINT)&Rect.right );
		dim = RectWidth( &Rect ) - cx;
		Rect.left += dim/2; Rect.right -= (dim+1)/2;
		dim = RectHeight( &Rect ) - cy;
		Rect.top += dim/2; Rect.bottom -= (dim+1)/2;
		SetWindowPos( hWindow, 0, /* Z order ignored */
			Rect.left, Rect.top, /* New positions */
			cx, cy, /* New size */
			SWP_NOZORDER );
		break;

	    case 2:	/* Message to initialize the selection rectangle */
			/* Caller passes a rect pointer in 1/1000th inches */
		lpRect = (LPRECT)lParam;
		CopyRect( &SelectRect, lpRect );
		ScaleRect( &SelectRect, scale );
		break;
	    }
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( 0L );
}


/***********************************************************************/
BOOL FAR PASCAL DlgScanProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
LONG	 lParam;
{
BOOL	 Bool;
FNAME	 szFileName;
LPRECT	 lpRect;
long	 lPlaten;
int	 Value;

switch (msg)
    {
    case WM_INITDIALOG:
	DoScanInits( hDlg );
	lPlaten = MAKELONG( SCenv.MaxScanWidth, SCenv.MaxScanLength );
	SendDlgItemMessage(hDlg, IDC_SCANFRAME, WM_COMMAND, 1, lPlaten );
	SendDlgItemMessage(hDlg, IDC_SCANFRAME, WM_COMMAND, 2,
		(long)(LPTR)&SCatt.Frame );
	if ( !Control.ColorEnabled )
		{
		SCatt.ColorOption = 0;
		SCenv.ScanBits &= 3;
		}
	if ( *SCenv.Company == 'A' ||
	     *SCenv.Company == 'H' ||
	     *SCenv.Company == ' ' )
		{
		CheckDlgButton( hDlg, IDC_SCANINVERT, SCatt.Invert );
		CheckDlgButton( hDlg, IDC_SCANMIRROR, SCatt.Mirror );
		}
	else	{
		CheckDlgButton( hDlg, IDC_SCANINVERT, NO );
		CheckDlgButton( hDlg, IDC_SCANMIRROR, NO );
		ControlEnable( hDlg, IDC_SCANINVERT, NO );
		ControlEnable( hDlg, IDC_SCANMIRROR, NO );
//		ControlEnable( hDlg, IDC_CONTBRIT, NO );
		ControlEnable( hDlg, IDC_BRIGHTNESS, NO );
		ControlEnable( hDlg, IDC_CONTRAST, NO );
		}
	CheckDlgButton( hDlg, IDC_DMA, SCatt.UseDMA );
	CheckRadioButton( hDlg, IDC_VELOCITY1, IDC_VELOCITY5,
		IDC_VELOCITY1 + SCatt.ScanSpeed - 1 );
	SetWindowText( hDlg, SCenv.Company );
	CheckRadioButton( hDlg, IDC_SCAN1BIT, IDC_SCANCOLOR,
		SCatt.ColorOption ? IDC_SCANCOLOR :
		(SCatt.BitsPerPixel == 1) ? IDC_SCAN1BIT : IDC_SCAN8BIT );
	CheckDlgButton( hDlg, IDC_SCANMAP, Scans.DoCalibration );
//	SetWindowLong( GetDlgItem( hDlg, IDC_CONTBRIT ), 0,
//		MAKELONG(SCatt.Contrast,SCatt.Brightness) );
	InitDlgItemSpin( hDlg, IDC_BRIGHTNESS, SCatt.Brightness, YES,
		-100, 100 );
	InitDlgItemSpin( hDlg, IDC_CONTRAST, SCatt.Contrast, YES,
		-100, 100 );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.ScanMap,
		IDN_SCANMAP );
	ControlEnable( hDlg, IDC_EXTNAMES, Scans.DoCalibration );
	ControlEnable( hDlg, IDC_SCAN1BIT, SCenv.ScanBits & 1 );
	ControlEnable( hDlg, IDC_SCAN8BIT, SCenv.ScanBits & 2 );
	ControlEnable( hDlg, IDC_SCANCOLOR, SCenv.ScanBits & (4|8) );
	ControlEnable( hDlg, IDC_VELOCITY5, SCenv.NumSpeeds >= 1 );
	ControlEnable( hDlg, IDC_VELOCITY4, SCenv.NumSpeeds >= 2 );
	ControlEnable( hDlg, IDC_VELOCITY3, SCenv.NumSpeeds >= 3 );
	ControlEnable( hDlg, IDC_VELOCITY2, SCenv.NumSpeeds >= 4 );
	ControlEnable( hDlg, IDC_VELOCITY1, SCenv.NumSpeeds >= 5 );
	ControlEnable( hDlg, IDC_DMA, SCenv.DataChannel );
	SetFocus( GetDlgItem( hDlg, IDC_PRESCAN ) );
	return( FALSE );

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_RES:
	    case IDC_HEIGHT:
	    case IDC_WIDTH:
	    case IDC_DISTORT:
	    case IDC_SCALEX:
	    case IDC_SCALEY:
		DoSizeControls( hDlg, wParam, lParam );
		SCatt.Resolution = Edit.Resolution;
		break;

	    case IDC_SCANFRAME:
		lpRect = (LPRECT)lParam;
		/* Passes back the crop boundaries in thousanths of inches */
		SCatt.Frame.left =
			bound( lpRect->left, 0, SCenv.MaxScanWidth-1 );
		SCatt.Frame.top =
			bound( lpRect->top, 0, SCenv.MaxScanLength-1 );
		SCatt.Frame.right =
			bound( lpRect->right, 0,SCenv.MaxScanWidth-1 );
		SCatt.Frame.bottom =
			bound( lpRect->bottom, 0, SCenv.MaxScanLength-1 );
		CopyRect( &Edit.Crop, &SCatt.Frame );
		/* Fake a scale change so all are recomputed and displayed */
		SetFocus( GetDlgItem( hDlg, IDC_SCALEX ) );
		DoSizeControls( hDlg, IDC_SCALEX, (long)EN_CHANGE<<16 );
		SetFocus( GetDlgItem( hDlg, IDC_SCALEY ) );
		DoSizeControls( hDlg, IDC_SCALEY, (long)EN_CHANGE<<16 );
		break;

	    case IDC_DMA:
		SCatt.UseDMA = !SCatt.UseDMA;
		CheckDlgButton( hDlg, IDC_DMA, SCatt.UseDMA );
		break;

	    case IDC_SCANINVERT:
		SCatt.Invert = !SCatt.Invert;
		CheckDlgButton( hDlg, IDC_SCANINVERT, SCatt.Invert );
		break;

	    case IDC_SCANMIRROR:
		SCatt.Mirror = !SCatt.Mirror;
		CheckDlgButton( hDlg, IDC_SCANMIRROR, SCatt.Mirror );
		break;

	    case IDC_SCAN1BIT:
	    case IDC_SCAN8BIT:
	    case IDC_SCANCOLOR:
		CheckRadioButton( hDlg, IDC_SCAN1BIT, IDC_SCANCOLOR, wParam );
		SCatt.BitsPerPixel = ( (wParam == IDC_SCAN1BIT) ? 1 : 8 );
		SCatt.ColorOption = ( (wParam == IDC_SCANCOLOR) ? 8 : 0 );
		Edit.Depth = ( SCatt.ColorOption ? 2 :
			( SCatt.BitsPerPixel == 1 ? 1 : 1 ) );
		SetDlgItemInt( hDlg, IDC_MEMORY, AstralImageMemory(), NO );
		AstralControlPaint( hDlg, IDC_MEMORY );
		break;

	    case IDC_SCANMAP:
		Scans.DoCalibration = !Scans.DoCalibration;
		CheckDlgButton( hDlg, IDC_SCANMAP, Scans.DoCalibration );
		ControlEnable( hDlg, IDC_EXTNAMES, Scans.DoCalibration );
		break;

	    case IDC_EXTNAMES:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		GetExtName( hDlg, wParam, IDC_FILENAME, Names.ScanMap,
			IDN_SCANMAP, HIWORD(lParam) );
		if ( !LookupExtFile( Names.ScanMap, szFileName, IDN_SCANMAP ) )
			break;
		if ( !LoadMap( &Scans.CalMap[0], &Scans.CalMap[1],
			       &Scans.CalMap[2], &Scans.CalMap[3], szFileName ))
			break;
		break;

	    case IDC_EXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.ScanMap,
			IDN_SCANMAP, wParam, NO );
		break;

	    case IDC_VELOCITY1:
	    case IDC_VELOCITY2:
	    case IDC_VELOCITY3:
	    case IDC_VELOCITY4:
	    case IDC_VELOCITY5:
		CheckRadioButton( hDlg, IDC_VELOCITY1, IDC_VELOCITY5, wParam );
		SCatt.ScanSpeed = wParam - IDC_VELOCITY1 + 1;
		break;

//	    case IDC_CONTBRIT:
//		if ( GetFocus() != GetDlgItem( hDlg, IDC_CONTBRIT ) )
//			break;
//		SCatt.Brightness = (int)LOWORD(lParam);
//		SCatt.Contrast = (int)HIWORD(lParam);
//		SetDlgItemInt( hDlg, IDC_CONTRAST, SCatt.Contrast, YES );
//		AstralControlPaint( hDlg, IDC_CONTRAST );
//		SetDlgItemInt( hDlg, IDC_BRIGHTNESS, SCatt.Brightness, YES );
//		AstralControlPaint( hDlg, IDC_BRIGHTNESS );
//		break;

	    case IDC_CONTRAST:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		SCatt.Contrast = (int)GetDlgItemSpin( hDlg, wParam,
			&Bool, YES );
//		SetWindowLong( GetDlgItem( hDlg, IDC_CONTBRIT ), 0,
//			MAKELONG(SCatt.Brightness,SCatt.Contrast) );
//		AstralControlPaint( hDlg, IDC_CONTBRIT );
		break;

	    case IDC_BRIGHTNESS:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		SCatt.Brightness = (int)GetDlgItemSpin( hDlg, wParam,
			&Bool, YES );
//		SetWindowLong( GetDlgItem( hDlg, IDC_CONTBRIT ), 0,
//			MAKELONG(SCatt.Brightness,SCatt.Contrast) );
//		AstralControlPaint( hDlg, IDC_CONTBRIT );
		break;

	    case IDC_PRESCAN:
		AstralCursor( IDC_WAIT );
		DoPreScan( hDlg, NO );
		AstralCursor( NULL );
		break;

	    case IDC_SCANCHECK:
		AstralCursor( IDC_WAIT );
		DoPreScan( hDlg, YES );
		AstralCursor( NULL );
		break;

	    case IDOK:
		if ( FMUL( Edit.Resolution, Edit.Width ) > MAX_IMAGE_LINE )
			{
			Message( IDS_ETOOWIDE );
			break;
			}
		if ( !LookupExtFile( Names.ScanMap, szFileName, IDN_SCANMAP ) )
			;//break;
		if ( !LoadMap( &Scans.CalMap[0], &Scans.CalMap[1],
			       &Scans.CalMap[2], &Scans.CalMap[3], szFileName ))
		AstralCursor( IDC_WAIT );
		Bool = DoScan( Edit.ScaleX );
		AstralCursor( NULL );
		if ( Bool )
			AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
} /* end DlgScanProc */


/***********************************************************************/
VOID DoScanInits( hDlg )
/***********************************************************************/
HWND	hDlg;
{
LPTR	lpUnit;

/* Be careful to set all values before doing a SetDlg... with them */
/* The Edit structure is a temorary holding area for sizing dialog boxes */
// Leave Edit.SmartSize alone
Edit.Distortable = NO;
Edit.ScaleX = Edit.ScaleY = 100;
Edit.Resolution = SCatt.Resolution;
Edit.CropResolution = 1000;
CopyRect( &Edit.Crop, &SCatt.Frame );
Edit.Depth = ( SCatt.ColorOption ? 2 : ( SCatt.BitsPerPixel == 1 ? 1 : 1 ) );
Edit.Width  = FGET( CROP_WIDTH, Edit.CropResolution );
Edit.Height = FGET( CROP_HEIGHT, Edit.CropResolution );
CheckDlgButton( hDlg, IDC_DISTORT, Edit.Distortable );
SetDlgItemFixed( hDlg, IDC_HEIGHT, Edit.Height, NO );
SetDlgItemFixed( hDlg, IDC_WIDTH, Edit.Width, NO );
SetDlgItemInt( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
SetDlgItemInt( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
SetDlgItemInt( hDlg, IDC_RES, Edit.Resolution, NO );
SetDlgItemInt( hDlg, IDC_MEMORY, AstralImageMemory(), NO );
if ( AstralStr( Control.Units, &lpUnit ) )
	{
	SetDlgItemText( hDlg, IDC_WIDTHLABEL, lpUnit );
	SetDlgItemText( hDlg, IDC_HEIGHTLABEL, lpUnit );
	}
}


/***********************************************************************/
BOOL FAR PASCAL DlgScanSetupProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND hDlg;
unsigned msg;
WORD wParam;
LONG lParam;
{
BYTE szPort[5];
LPTR lpString;
FNAME szFileName;
static WORD wPort;
extern LPTR lpScannerDriver;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	lstrcpy( Names.Saved, Names.Scanner );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Scanner,
		IDN_SCANNER );
	wPort = SCinit.PortAddr;
	SetDlgItemText( hDlg, IDC_PORT, itoh(SCinit.PortAddr) );
	return( FALSE );

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	if ( HandleMiniScroll( hDlg, wParam, lParam ) )
		break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_PORT:
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		GetDlgItemText( hDlg, wParam, szPort, sizeof(szPort) );
		SCinit.PortAddr = htoi( szPort );
		break;

	    case IDOK:
		AstralDlgEnd( hDlg, TRUE );
		PutDefaultString( "Names.Scanner", Names.Scanner );
		PutDefaultString( "SCinit.PortAddr", itoh(SCinit.PortAddr) );
		if ( StringsEqual( Names.Scanner, Names.Saved ) )
			break;
		if ( !LookupExtFile( Names.Scanner, szFileName, IDN_SCANNER ) )
			break;
		if ( !lpScannerDriver )
		      lpScannerDriver = Alloc( (long)MAX_FNAME_LEN );
		if (  lpScannerDriver )
			lstrcpy( lpScannerDriver, szFileName );
		if ( lpString = strchr( szFileName, '.' ) )
			*lpString = NULL;
		Message( IDS_WARNDRV, stripdir(szFileName) );
		break;

	    case IDCANCEL:
		SCinit.PortAddr = wPort;
		lstrcpy( Names.Scanner, Names.Saved );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_EXTNAMES:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		GetExtName( hDlg, wParam, IDC_FILENAME, Names.Scanner,
			IDN_SCANNER, HIWORD(lParam) );
		break;

	    case IDC_EXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Scanner,
			IDN_SCANNER, wParam, NO );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }
}


/***********************************************************************/
BOOL FAR PASCAL DlgGrabSetupProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND hDlg;
unsigned msg;
WORD wParam;
LONG lParam;
{
BYTE szPort[5];
LPTR lpString;
FNAME szFileName;
static WORD wPort;
extern LPTR lpGrabberDriver;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	lstrcpy( Names.Saved, Names.Grabber );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Grabber,
		IDN_GRABBER );
	wPort = Control.FGPort;
	SetDlgItemText( hDlg, IDC_PORT, itoh(Control.FGPort) );
	return( FALSE );

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	if ( HandleMiniScroll( hDlg, wParam, lParam ) )
		break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_PORT:
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		GetDlgItemText( hDlg, wParam, szPort, sizeof(szPort) );
		Control.FGPort = htoi( szPort );
		break;

	    case IDOK:
		AstralDlgEnd( hDlg, TRUE );
		PutDefaultString( "Names.Grabber", Names.Grabber );
		PutDefaultString( "Control.FGPort", itoh(Control.FGPort) );
		if ( StringsEqual( Names.Grabber, Names.Saved ) )
			break;
		if ( !LookupExtFile( Names.Grabber, szFileName,
		     IDN_GRABBER ) )
			break;
		if ( strcmp( ".DGR", strchr( szFileName, '.' ) ) )
			break;
		if ( !lpGrabberDriver )
		      lpGrabberDriver = Alloc( (long)MAX_FNAME_LEN );
		if (  lpGrabberDriver )
			lstrcpy( lpGrabberDriver, szFileName );
		if ( lpString = strchr( szFileName, '.' ) )
			*lpString = NULL;
		Message( IDS_WARNDRV, stripdir(szFileName) );
		break;

	    case IDCANCEL:
		Control.FGPort = wPort;
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_EXTNAMES:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Grabber,
			IDN_GRABBER, HIWORD(lParam) );
		break;

	    case IDC_EXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Grabber,
			IDN_GRABBER, wParam, NO );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }
}


/***********************************************************************/
int ScanImagePaint(hWindow, lpSelectRect)
/***********************************************************************/
HWND	 	hWindow;
LPRECT		lpSelectRect;
{
PAINTSTRUCT	ps;
HDC		hDC, hMemDC;
RECT		rRect;
int		y, yline, ylast, ystart;
LPTR		lp, lpBuffer;
LPFRAME		lpOldFrame;
SPAN		span;
FIXED		yrate, yoffset;

/* Need to paint the image into the image window */
hDC = BeginPaint( hWindow, &ps );
SetMapMode( hDC, MM_TEXT );
GetClientRect( hWindow, &rRect );
//EraseControlBackground( hDC, hWindow, &rRect );

//GetWindowRect( hWindow, &rRect );
/* convert the rectangle coordinates to be relative to the parent */
//ScreenToClient( GetParent(hWindow), (LPPOINT)&rRect.left );
//ScreenToClient( GetParent(hWindow), (LPPOINT)&rRect.right );

if ( Scans.lpScanFrame )
	{
	if (AllocLines(&lpBuffer, 1, RectWidth(&rRect), Scans.lpScanFrame->Depth))
		{
		yrate = FGET(Scans.lpScanFrame->Ysize,RectHeight(&rRect));
		ystart = 0;
		yoffset = yrate>>1;
		ylast   = -1;
	
		lpOldFrame = frame_set(Scans.lpScanFrame);
		StartSuperBlt( hDC, &BltScreen, &rRect, Scans.lpScanFrame->Depth, 10, 0, 0);
		span.sx = 0;
		span.sy = 0;
		span.dx = Scans.lpScanFrame->Xsize;
		span.dy = 1;
		for (y = rRect.top; y <= rRect.bottom; ++y)
			{
			yline = ystart + HIWORD( yoffset );
			yoffset += yrate;
			if ( yline != ylast )
				{
				ylast = yline;
				span.sy = yline;
				frame_read(&span, lpBuffer, RectWidth(&rRect));
				}
			SuperBlt( lpBuffer );
			}
		SuperBlt(NULL);
		frame_set(lpOldFrame);
		FreeUp(lpBuffer);
		}
	}

//rRect.right--; rRect.bottom--;
FrameRect( hDC, &rRect, Window.hBlackBrush );

InvertSelection( hWindow, hDC, lpSelectRect, SL_BOXHANDLES );

/* Indicate that we're finished painting the entire client area */
ValidateRect( hWindow, NULL );
EndPaint( hWindow, &ps );
return( TRUE );
}
