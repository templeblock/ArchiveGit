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
BOOL process()
/************************************************************************/
{
int pix, lin;
LPFRAME lpFrame, lpOldFrame;

if (!CacheInit(YES, NO))
	return(FALSE);

if ( !(lpOldFrame = frame_set( NULL )) )
	return( FALSE );
if (lpImage->UndoFrame)
    {
    frame_close(lpImage->UndoFrame);
    lpImage->UndoFrame = NULL;
    }

pix = FMUL( Edit.Resolution, Edit.Width );
lin = FMUL( Edit.Resolution, Edit.Height );

if ( Edit.SmartSize && abs(CROP_WIDTH-pix) > 2 && abs(CROP_HEIGHT-lin) > 2 )
	lpFrame = DoSmartSize();
else	lpFrame = DoSize();

if ( !lpFrame )
	{
	frame_set(lpOldFrame);
	return( FALSE );
	}
/* Setup the new image and bring up the new image window */
CacheNewFrame(lpFrame);
UpdateImageSize();

return( TRUE );
}

/************************************************************************/
BOOL DoMirror(xmirror, ymirror)
/************************************************************************/
BOOL xmirror, ymirror;
{
RECT rMask;
int i, dy, lines;
SPAN span1, span2;
LPTR lpBuffer[2];

mask_rect(&rMask);
dy = RectHeight(&rMask);
lines = 1;
if (ymirror)
	{
	dy = ( dy + 1 ) / 2;
	++lines;
	}

if ( !(AllocLines( lpBuffer, lines, RectWidth(&rMask), DEPTH )) )
	{
	Message( IDS_EMEMALLOC );
	return( FALSE );
	}

span1.sy = rMask.top;
if (ymirror)
	span2.sy = rMask.bottom;
else
	span2.sy = span1.sy;
span2.sx = span1.sx = rMask.left;
span2.dy = span1.dy = 1;
span2.dx = span1.dx = RectWidth(&rMask);
for (i = 0; i < dy; ++i)
	{
	AstralClockCursor(i, dy);
	if ( !CacheRead( &span1, lpBuffer[0], span1.dx) )
		{
		Message( IDS_EREAD, (LPTR)Control.RamDisk );
		FreeUp(lpBuffer[0]);
		return(FALSE);
		}
	if (ymirror)
		{
		if ( !CacheRead( &span2, lpBuffer[1], span2.dx) )
			{
			Message( IDS_EREAD, (LPTR)Control.RamDisk );
			FreeUp(lpBuffer[0]);
			return(FALSE);
			}
		if (xmirror)
			frame_mirror(lpBuffer[1], span2.dx);
		}

	if ( xmirror )
		frame_mirror( lpBuffer[0], span1.dx );
	
	if ( !CacheWrite( &span2, lpBuffer[0], span2.dx) )
		{
		Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
		FreeUp(lpBuffer[0]);
		return(FALSE);
		}
	if (ymirror)
		{
		if ( !CacheWrite( &span1, lpBuffer[1], span1.dx) )
			{
			Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
			FreeUp(lpBuffer[0]);
			return(FALSE);
			}
		--span2.sy;
		}
	else
		++span2.sy;
	++span1.sy;
	}
FreeUp(lpBuffer[0]);
AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &rMask);
UpdateImage(&rMask, YES);

return( TRUE );
}

/************************************************************************/
LPFRAME DoSize()
/************************************************************************/
{
int	pix, lin, ystart, yline, ylast;
LPTR	lpBuffer;
char 	xmirror, ymirror;
FIXED	yrate, yoffset;
SPAN	span;
LPFRAME	lpFrame, lpOldFrame;

if ( !(lpOldFrame = frame_set( NULL )) )
	return( FALSE );

pix = FMUL( Edit.Resolution, Edit.Width );
lin = FMUL( Edit.Resolution, Edit.Height );

xmirror = ymirror = NO;

yrate = FGET( CROP_HEIGHT, lin );
if ( ymirror )
	{
	ystart = Edit.Crop.bottom;
	yoffset = yrate>>1; /* was UNITY-1; almost 1 */
	yrate = -yrate;
	}
else	{
	ystart = Edit.Crop.top;
	yoffset = yrate>>1;
	}

span.sx = 0;
span.dy = 1;

if ( !(lpFrame = frame_open( DEPTH, pix, lin, Edit.Resolution )) )
	{
	Message( IDS_ESCROPEN, (LPTR)Control.RamDisk );
	return( NULL );
	}

if ( !(AllocLines( &lpBuffer, 1, pix, DEPTH )) )
	{
	frame_close( lpFrame );
	Message( IDS_EMEMALLOC );
	return( NULL );
	}

yline = -1;
span.dx = pix;
for ( span.sy=0; span.sy<lin; span.sy++ )
	{
	AstralClockCursor( span.sy, lin );
	ylast = yline;
	yline = ystart + HIWORD( yoffset );
	yoffset += yrate;
	if ( yline != ylast )
		{
		frame_set( lpOldFrame );
		if ( !readimage( yline, Edit.Crop.left, Edit.Crop.right,
		    pix, lpBuffer) )
			{
			Message( IDS_EREAD, (LPTR)Control.RamDisk );
			goto ErrorExit;
			}
		if ( xmirror )
			frame_mirror( lpBuffer, pix );
		}
	frame_set( lpFrame );
	if ( !frame_write( &span, lpBuffer, pix) )
		{
		Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
		goto ErrorExit;
		}
	}

FreeUp( lpBuffer );
return( lpFrame );

ErrorExit:
frame_close( lpFrame );
frame_set(lpOldFrame);
FreeUp( lpBuffer );
return( NULL );
}


/************************************************************************/
LPFRAME DoSmartSize()
/************************************************************************/
{
int	pix, lin, yline, yincr;
LPTR	lpBuffer, lp;
SPAN	span;
LPFRAME	lpFrame, lpOldFrame;
BOOL	xmirror, ymirror, fNeedNewLine;

if ( !(lpOldFrame = frame_set( NULL )) )
	return( NULL );

pix = FMUL( Edit.Resolution, Edit.Width );
lin = FMUL( Edit.Resolution, Edit.Height );

xmirror = ymirror = NO;

if ( ymirror )
	{
	yline = Edit.Crop.bottom;
	yincr = -1;
	}
else	{
	yline = Edit.Crop.top;
	yincr = 1;
	}

span.sx = 0;
span.dy = 1;
span.dx = pix;

if ( !(lpFrame = frame_open( DEPTH, pix, lin, Edit.Resolution )) )
	{
	Message( IDS_ESCROPEN, (LPTR)Control.RamDisk );
	return( NULL );
	}

if ( !(AllocLines( &lpBuffer, 1, pix, DEPTH )) )
	{
	frame_close( lpFrame );
	Message( IDS_EMEMALLOC );
	return( NULL );
	}

if ( !SmartSetup( CROP_WIDTH, pix, CROP_HEIGHT, lin, DEPTH ) )
	{
	frame_close( lpFrame );
	FreeUp( lpBuffer );
	return( NULL );
	}

for ( span.sy=0; span.sy<lin; )
	{
	AstralClockCursor( span.sy, lin );
	frame_set( lpOldFrame );
	if ( !(lp = frame_ptr( 0, Edit.Crop.left, yline, NO)) )
		{
		Message( IDS_EREAD, (LPTR)Control.RamDisk );
		goto ErrorExit;
		}
	if ( SmartSize( lp, lpBuffer, &fNeedNewLine ) )
		{
		if ( xmirror )
			frame_mirror( lpBuffer, pix );
		frame_set( lpFrame );
		if ( !frame_write( &span, lpBuffer, pix) )
			{
			Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
			goto ErrorExit;
			}
		span.sy++;
		}
	if ( fNeedNewLine )
		yline += yincr;
	}

SmartDone();
FreeUp( lpBuffer );
return( lpFrame );

ErrorExit:

SmartDone();
frame_close( lpFrame );
frame_set( lpOldFrame );
FreeUp( lpBuffer );
return( NULL );
}


/************************************************************************/
BOOL DoCrop( lpCropRect )
/************************************************************************/
LPRECT lpCropRect;
{
int	pix, lin, yline, x;
LPTR	lpBuffer, lpLine;
SPAN	span;
LPFRAME	lpFrame, lpOldFrame;
RGBS whiteRGB;
long white;

if (!CacheInit(YES, NO))
	return(FALSE);
if ( !(lpOldFrame = frame_set( NULL )) )
	return( FALSE );
if (lpImage->UndoFrame)
    {
    frame_close(lpImage->UndoFrame);
    lpImage->UndoFrame = NULL;
    }

lpCropRect->left = bound( lpCropRect->left, 0, lpImage->npix-1 );
lpCropRect->right = bound( lpCropRect->right-1, 0, lpImage->npix-1 );
lpCropRect->top = bound( lpCropRect->top, 0, lpImage->nlin-1 );
lpCropRect->bottom = bound( lpCropRect->bottom-1, 0, lpImage->nlin-1 );

pix = RectWidth( lpCropRect );
lin = RectHeight( lpCropRect );
if ( pix == lpOldFrame->Xsize && lin == lpOldFrame->Ysize )
	return( TRUE );

if ( !(lpFrame = frame_open( DEPTH, pix, lin, lpOldFrame->Resolution )) )
	{
	Message( IDS_ESCROPEN, (LPTR)Control.RamDisk );
	return( FALSE );
	}

if ( !(AllocLines( &lpBuffer, 1, pix, DEPTH )) )
	{
	frame_close( lpFrame );
	Message( IDS_EMEMALLOC );
	return( FALSE );
	}

yline = lpCropRect->top;
span.sx = 0;
span.dy = 1;
span.dx = pix;
whiteRGB.red = whiteRGB.green = whiteRGB.blue = 255;
white = frame_tocache(&whiteRGB);
for ( span.sy=0; span.sy<lin; span.sy++ )
	{
	AstralClockCursor( span.sy, lin );
	frame_set( lpOldFrame );
	if ( !readimage( yline++, lpCropRect->left, lpCropRect->right,
	    pix, lpBuffer) )
		{
		Message( IDS_EREAD, (LPTR)Control.RamDisk );
		goto ErrorExit;
		}
	lpLine = lpBuffer;
	for (x = lpCropRect->left; x <= lpCropRect->right; ++x, lpLine += DEPTH)
		if (!IsMasked(x, span.sy+lpCropRect->top))
			frame_putpixel(lpLine, white);
	frame_set( lpFrame );
	if ( !frame_write( &span, lpBuffer, pix) )
		{
		Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
		goto ErrorExit;
		}
	}

FreeUp( lpBuffer );

/* Setup the new image and bring up the new image window */
CacheNewFrame(lpFrame);
UpdateImageSize();

return( TRUE );

ErrorExit:
frame_close( lpFrame );
frame_set(lpOldFrame);
FreeUp( lpBuffer );
return( FALSE );
}


/************************************************************************/
BOOL new( Depth )
/************************************************************************/
int Depth;
{
int	pix, lin, bpp;
LPTR	lpBuffer;
SPAN	span;
LPFRAME	lpFrame;
RGBS rgb;
long pixel;

pix = FMUL( Edit.Resolution, Edit.Width );
lin = FMUL( Edit.Resolution, Edit.Height );

if ( Depth == 1 )
	bpp = 8;
else	bpp = 24;

if ( !(lpFrame = frame_open( Depth, pix, lin, Edit.Resolution )) )
	{
	Message( IDS_ESCROPEN, (LPTR)Control.RamDisk );
	return( FALSE );
	}

if ( !(AllocLines( &lpBuffer, 1, pix, Depth )) )
	return( FALSE );

frame_set( lpFrame );
rgb.red = rgb.green = rgb.blue = 255;
pixel = frame_tocache(&rgb);
frame_setpixels(lpBuffer, pix, pixel);
span.sx = 0;
span.dx = pix;
span.dy = 1;
for ( span.sy=0; span.sy<lin; span.sy++ )
	{
	AstralClockCursor( span.sy, lin );
	if ( !frame_write( &span, lpBuffer, pix ) )
		{
		Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
		frame_close( lpFrame );
		FreeUp( lpBuffer );
		return( FALSE );
		}
	}

FreeUp( lpBuffer );

/* Setup the new image and bring up the new image window */
NewImageWindow( NULL, NULL, lpFrame, IDC_SAVETIFF, FALSE, FALSE, IMG_DOCUMENT, NULL );

return( TRUE );
}


/************************************************************************/
BOOL CreateScratchPad()
/************************************************************************/
{
int Depth;
int	pix, lin;
LPTR	lpBuffer;
SPAN	span;
LPFRAME	lpFrame;
RGBS rgb;
long pixel;

Depth = ( Control.IsScratchpadColor ? 3 : 1 );
pix = Control.ScratchpadWidth;
lin = Control.ScratchpadHeight;
if ( !(lpFrame = frame_open( Depth, pix, lin, 100 /*Resolution*/ )) )
	{
	Message( IDS_ESCROPEN, (LPTR)Control.RamDisk );
	return( FALSE );
	}

if ( !(AllocLines( &lpBuffer, 1, pix, Depth )) )
	return( FALSE );

frame_set( lpFrame );
rgb.red = rgb.green = rgb.blue = 255;
pixel = frame_tocache(&rgb);
frame_setpixels(lpBuffer, pix, pixel);
span.sx = 0;
span.dx = pix;
span.dy = 1;
for ( span.sy=0; span.sy<lin; span.sy++ )
	{
	AstralClockCursor( span.sy, lin );
	if ( !frame_write( &span, lpBuffer, pix ) )
		{
		Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
		frame_close( lpFrame );
		FreeUp( lpBuffer );
		return( FALSE );
		}
	}

FreeUp( lpBuffer );

/* Setup the new image and bring up the new image window */
NewImageWindow( NULL, "ScratchPad", lpFrame, IDC_SAVETIFF, FALSE, FALSE, IMG_DOCUMENT, NULL );

return( TRUE );
}


