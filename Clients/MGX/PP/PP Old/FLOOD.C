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
#include "malloc.h"

#define OK2FLOOD8(v, x, y) \
(IsMasked((x), (y)) == onMask && (v) >= minval && (v) <= maxval && (v) != tintval)

#define OK2FLOOD16(v, x, y) \
(IsMasked((x), (y)) == onMask && (v) != miniTint && CDistance((v)) < maxdistance)

#define OK2FLOOD24(v, x, y) \
(IsMasked((x), (y)) == onMask && (v) != CTint && CDistance24((v)) < maxdistance)

#define MIN_STACK_SIZE 1024

/*	where Frange is flood range (0-100%)				*/
/*	where Ftint is the value to flood with (0-MAXVAL +HIBITS)	*/
/*	where Fsource is pixel value to base decision on		*/

static HDC hDC;
static BOOL onMask;
static BYTE tintval, minval, maxval;
static BOOL FAbort;
static WORD miniTint;
static long CTint;
static long maxdistance;
static int sR, sG, sB;
static RECT rFlood;

/************************************************************************/
void Flood8( hmyDC, x, y)
/************************************************************************/
HDC hmyDC;
int x, y;
{
int HighBits, delta;
long v;
LPTR lpLine;
BYTE Fsource;

lpLine = CachePtr(0, x, y, NO);
if (!lpLine)
	return;

hDC = hmyDC;
onMask = IsMasked(x, y);
Fsource = *lpLine;
tintval = RGBtoL(&Palette.ActiveRGB);
if (Fsource == tintval)
    return;

if (!CacheInit( NO, YES ))
	return;
delta = Fsource - TOGRAY(Retouch.FloodRange);
minval = bound( delta, 0, 255 );
delta = Fsource + TOGRAY(Retouch.FloodRange);
maxval = bound( delta, 0, 255 );
FAbort = FALSE;
rFlood.top = rFlood.left = 32767;
rFlood.bottom = rFlood.right = -32767;
AstralCursor(IDC_WAIT);
do_region_fill8( x, y, 1, x, x );
AstralCursor( NULL );
if (RectWidth(&rFlood) > 0)
	{
	AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &rFlood);
	lpImage->dirty = IDS_UNDOFLOOD;
	UpdateImage(&rFlood, YES);
	}
}


/************************************************************************/
void Flood16( hmyDC, x, y)
/************************************************************************/
HDC hmyDC;
int x, y;
{
long v;
LPWORD lpLine;
WORD Fsource;

lpLine = (LPWORD)CachePtr(0, x, y, NO);
if (!lpLine)
	return;

hDC = hmyDC;
onMask = IsMasked(x, y);
Fsource = *lpLine;
miniTint = RGBtoMiniRGB( &Palette.ActiveRGB );
if (Fsource == miniTint)
    return;

if (!CacheInit( NO, YES ))
	return;

sR = (int)MaxiR(Fsource);
sG = (int)MaxiR(Fsource);
sB = (int)MaxiR(Fsource);

maxdistance = ((long)TOGRAY(Retouch.FloodRange)*(long)TOGRAY(Retouch.FloodRange)) * 3;

FAbort = FALSE;
rFlood.top = rFlood.left = 32767;
rFlood.bottom = rFlood.right = -32767;
AstralCursor(IDC_WAIT);
do_region_fill16( x, y, 1, x, x );
AstralCursor( NULL );
if (RectWidth(&rFlood) > 0)
	{
	AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &rFlood);
	lpImage->dirty = IDS_UNDOFLOOD;
	UpdateImage(&rFlood,YES);
	}
}

/************************************************************************/
void Flood24( hmyDC, x, y)
/************************************************************************/
HDC hmyDC;
int x, y;
{
long v;
LPTR lpLine;
RGBS rgb;

lpLine = CachePtr(0, x, y, NO);
if (!lpLine)
	return;

hDC = hmyDC;
onMask = IsMasked(x, y);
frame_getpixel(lpLine, &v);
CTint = frame_tocache(&Palette.ActiveRGB);
if (v == CTint)
	return;
if (!CacheInit( NO, YES ))
	return;

frame_getpixelRGB(NO, v, &rgb);
sR = (int)rgb.red;
sG = (int)rgb.green;
sB = (int)rgb.blue;

maxdistance = ((long)TOGRAY(Retouch.FloodRange)*(long)TOGRAY(Retouch.FloodRange)) * 3;

FAbort = FALSE;
rFlood.top = rFlood.left = 32767;
rFlood.bottom = rFlood.right = -32767;
AstralCursor(IDC_WAIT);
do_region_fill24( x, y, 1, x, x );
AstralCursor( NULL );
if (RectWidth(&rFlood) > 0)
	{
	AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &rFlood);
	lpImage->dirty = IDS_UNDOFLOOD;
	UpdateImage(&rFlood, YES);
	}
}

/************************************************************************/
static int do_region_fill8(x,y,dir,pxl,pxr)
/************************************************************************/
int x,y,dir,pxl,pxr;
{
int xl, xr;
int v;
LPTR lpLine, lpPixel;

if ( y < 0 || y >= lpImage->nlin )
	return(-1);
if ( stackavail() < MIN_STACK_SIZE )
	return(-1);

AstralBeachCursor();

xl = x;
xr = x;

/* scan to the left to determine endpts of seed line seg */
lpLine = CachePtr(0, x, y, NO);
if (lpLine)
	{
	lpPixel = lpLine;
	while ( --xl >= 0 )
		{
		--lpPixel;
		if ( !OK2FLOOD8(*lpPixel, xl, y) )
			break;
		}
	++xl;
	}

/* scan to the right to determine endpts of seed line seg */
if (lpLine)
	{
	lpPixel = lpLine;
	while ( ++xr < lpImage->npix )
		{
		++lpPixel;
		if ( !OK2FLOOD8(*lpPixel, xr, y) )
			break;
		}
	--xr;
	}

if (xr >= xl && lpLine)
	{
	lpLine = CachePtr(0, xl, y, YES);
	if (lpLine)
		{
		set(lpLine, xr-xl+1, tintval);
		if (xl < rFlood.left)
			rFlood.left = xl;
		if (xr > rFlood.right)
			rFlood.right = xr;
		if (y < rFlood.top)
			rFlood.top = y;
		if (y > rFlood.bottom)
			rFlood.bottom = y;
		}
	}

/* find and fill adjacent line segs in same direction */
if ((y+dir) >= 0 && (y+dir) < lpImage->nlin)
	lpLine = CachePtr(0, 0, y+dir, NO);
else	lpLine = NULL;
if (lpLine)
	{
	for (x=xl; x<=xr; x++)
		{
		if (x < 0 || x >= lpImage->npix)
			continue;
		if ( !OK2FLOOD8(*(lpLine+x), x, y+dir) )
			continue;
		if ( (v = do_region_fill8(x,y+dir,dir,xl,xr)) < 0)
			break;
		x = v;
		}
	}

/* find and fill adjacent line segs in opposite direction */
if ((y-dir) >= 0 && (y-dir) < lpImage->nlin)
	lpLine = CachePtr(0, 0, y-dir, NO);
else
	lpLine = NULL;
if (lpLine)
	{
	for (x=xl; x<=pxl; x++)
		{
		if (x < 0 || x >= lpImage->npix)
			continue;
		if ( !OK2FLOOD8(*(lpLine+x), x, y-dir) )
			continue;
		if ( (v = do_region_fill8(x,y-dir,-dir,xl,xr)) < 0 )
			break;
		x = v;
		}
	}

if ((y-dir) >= 0 && (y-dir) < lpImage->nlin)
	lpLine = CachePtr(0, 0, y-dir, NO);
else
	lpLine = NULL;
if (lpLine)
	{
	for (x=pxr; x<=xr; x++)
		{
		if (x < 0 || x >= lpImage->npix)
			continue;
		if ( !OK2FLOOD8(*(lpLine+x), x, y-dir) )
			continue;
		if ( (v = do_region_fill8(x,y-dir,-dir,xl,xr)) < 0 )
			break;
		x = v;
		}
	}

return(xr);
}

/************************************************************************/
static int do_region_fill16(x,y,dir,pxl,pxr)
/************************************************************************/
int x,y,dir,pxl,pxr;
{
int xl, xr;
int v;
LPWORD lpLine, lpPixel;

if ( y < 0 || y >= lpImage->nlin )
	return(-1);
if ( stackavail() < MIN_STACK_SIZE )
	return(-1);

AstralBeachCursor();

xl = x;
xr = x;

/* scan to the left to determine endpts of seed line seg */
lpLine = (LPWORD)CachePtr(0, x, y, NO);
if (lpLine)
	{
	lpPixel = lpLine;
	while ( --xl >= 0 )
		{
		--lpPixel;
		if ( !OK2FLOOD16(*lpPixel, xl, y) )
			break;
		}
	++xl;
	}

/* scan to the right to determine endpts of seed line seg */
if (lpLine)
	{
	lpPixel = lpLine;
	while ( ++xr < lpImage->npix )
		{
		++lpPixel;
		if ( !OK2FLOOD16(*lpPixel, xr, y) )
			break;
		}
	--xr;
	}

if (xr >= xl && lpLine)
	{
	lpLine = (LPWORD)CachePtr(0, xl, y, YES);
	if (lpLine)
		{
		set16(lpLine, (xr-xl+1), miniTint);
		if (xl < rFlood.left)
			rFlood.left = xl;
		if (xr > rFlood.right)
			rFlood.right = xr;
		if (y < rFlood.top)
			rFlood.top = y;
		if (y > rFlood.bottom)
			rFlood.bottom = y;
		}
	}

/* find and fill adjacent line segs in same direction */
if ((y+dir) >= 0 && (y+dir) < lpImage->nlin)
	lpLine = (LPWORD)CachePtr(0, 0, y+dir, NO);
else
	lpLine = NULL;
if (lpLine)
	{
	for (x=xl; x<=xr; x++)
		{
		if (x < 0 || x >= lpImage->npix)
			continue;
		if ( !OK2FLOOD16(*(lpLine+x), x, y+dir) )
			continue;
		if ( (v = do_region_fill16(x,y+dir,dir,xl,xr)) < 0)
			break;
		x = v;
		}
	}

/* find and fill adjacent line segs in opposite direction */
if ((y-dir) >= 0 && (y-dir) < lpImage->nlin)
	lpLine = (LPWORD)CachePtr(0, 0, y-dir, NO);
else
	lpLine = NULL;
if (lpLine)
	{
	for (x=xl; x<=pxl; x++)
		{
		if (x < 0 || x >= lpImage->npix)
			continue;
		if ( !OK2FLOOD16(*(lpLine+x), x, y-dir) )
			continue;
		if ( (v = do_region_fill16(x,y-dir,-dir,xl,xr)) < 0 )
			break;
		x = v;
		}
	}

if ((y-dir) >= 0 && (y-dir) < lpImage->nlin)
	lpLine = (LPWORD)CachePtr(0, 0, y-dir, NO);
else
	lpLine = NULL;
if (lpLine)
	{
	for (x=pxr; x<=xr; x++)
		{
		if (x < 0 || x >= lpImage->npix)
			continue;
		if ( !OK2FLOOD16(*(lpLine+x), x, y-dir) )
			continue;
		if ( (v = do_region_fill16(x,y-dir,-dir,xl,xr)) < 0 )
			break;
		x = v;
		}
	}

return(xr);
}


/************************************************************************/
static int do_region_fill24(x,y,dir,pxl,pxr)
/************************************************************************/
int x,y,dir,pxl,pxr;
{
int xl, xr;
int v;
long pixel;
LPTR lpLine, lpPixel;

if ( y < 0 || y >= lpImage->nlin )
	return(-1);
if ( stackavail() < MIN_STACK_SIZE )
	return(-1);

AstralBeachCursor();

xl = x;
xr = x;

/* scan to the left to determine endpts of seed line seg */
lpLine = CachePtr(0, x, y, NO);
if (lpLine)
    {
    lpPixel = lpLine;
    while ( --xl >= 0 )
	{
	lpPixel -= 3;
	frame_getpixel(lpPixel, &pixel);
	if ( !OK2FLOOD24(pixel, xl, y) )
	    break;
	}
    ++xl;
    }

/* scan to the right to determine endpts of seed line seg */
if (lpLine)
    {
    lpPixel = lpLine;
    while ( ++xr < lpImage->npix )
	{
	lpPixel += 3;
	frame_getpixel(lpPixel, &pixel);
	if ( !OK2FLOOD24(pixel, xr, y) )
	    break;
	}
    --xr;
    }

if (xr >= xl && lpLine)
    {
    lpLine = CachePtr(0, xl, y, YES);
    if (lpLine)
        {
        set24((LPRGB)lpLine, (xr-xl+1), CTint);
	if (xl < rFlood.left)
		rFlood.left = xl;
	if (xr > rFlood.right)
		rFlood.right = xr;
	if (y < rFlood.top)
		rFlood.top = y;
	if (y > rFlood.bottom)
		rFlood.bottom = y;
        }
    }
	   
/* find and fill adjacent line segs in same direction */
if ((y+dir) >= 0 && (y+dir) < lpImage->nlin)
    lpLine = CachePtr(0, 0, y+dir, NO);
else
    lpLine = NULL;
if (lpLine)
    {
    for (x=xl; x<=xr; x++)
	{
	if (x >= 0 && x < lpImage->npix)
	    {
	    frame_getpixel(lpLine+(x*3L), &pixel);
	    if ( OK2FLOOD24(pixel, x, y+dir) )
		{
		if ( (v = do_region_fill24(x,y+dir,dir,xl,xr)) < 0)
		    break;
		else
		    x = v;
		}
	    }
	}
    }

/* find and fill adjacent line segs in opposite direction */
if ((y-dir) >= 0 && (y-dir) < lpImage->nlin)
    lpLine = CachePtr(0, 0, y-dir, NO);
else
    lpLine = NULL;
if (lpLine)
    {
    for (x=xl; x<=pxl; x++)
	{
	if (x >= 0 && x < lpImage->npix)
	    {
	    frame_getpixel(lpLine+(x*3L), &pixel);
	    if ( OK2FLOOD24(pixel, x, y-dir) )
		{
		if ( (v = do_region_fill24(x,y-dir,-dir,xl,xr)) < 0 )
		    break;
		else	
		    x = v;
		}
	    }
	}
    }

if ((y-dir) >= 0 && (y-dir) < lpImage->nlin)
    lpLine = CachePtr(0, 0, y-dir, NO);
else
    lpLine = NULL;
if (lpLine)
    {
    for (x=pxr; x<=xr; x++)
	{
	if (x >= 0 && x < lpImage->npix)
	    {
	    frame_getpixel(lpLine+(x*3L), &pixel);
	    if ( OK2FLOOD24(pixel, x, y-dir) )
		{
		if ( (v = do_region_fill24(x,y-dir,-dir,xl,xr)) < 0 )
		    break;
		else	
		    x = v;
		}
	    }
	}
    }

return(xr);
}


/************************************************************************/
static long CDistance(v)
/************************************************************************/
WORD v;
{
long rError, gError, bError;

rError = abs((int)MaxiR(v)-sR);
rError *= rError;
gError = abs((int)MaxiG(v)-sG);
gError *= gError;
bError = abs((int)MaxiB(v)-sB);
bError *= bError;
return(rError+gError+bError);
}


/************************************************************************/
static long CDistance24(v)
/************************************************************************/
long v;
{
long rError, gError, bError;
RGBS rgb;

CopyRGB(&v, &rgb);
rError = abs((int)rgb.red - sR);
rError *= rError;
gError = abs((int)rgb.green - sG);
gError *= gError;
bError = abs((int)rgb.blue - sB);
bError *= bError;
return(rError+gError+bError);
}
