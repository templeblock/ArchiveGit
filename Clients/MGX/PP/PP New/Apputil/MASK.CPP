//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
/*=======================================================================*\

	MASK.C - Mask Management routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include "stdafx.h"
#include <math.h>
#include "mask.h"
#include "pixmap.h"

/************************************************************************/
LPMASK MaskCreate( LPFRAME lpAlphaIn, int iMaxWidth, int iMaxHeight,
				BOOL fOn, BOOL fNoUndo, FRMDATATYPE type)
/************************************************************************/
{
LPFRAME lpAlpha;
int dx, dy;
LPMASK lpMask;

lpAlpha = lpAlphaIn;
// Create an alpha channel
if (lpAlpha)
	{
	dx = FrameXSize(lpAlpha);
	dy = FrameYSize(lpAlpha);
	}
else
	{
	dx = iMaxWidth;
	dy = iMaxHeight;
	if ( !(lpAlpha = FrameOpen( type, dx, dy, 75 )) )
		return( NULL );
	// Force all lines to be set properly
	FrameSetBackground( lpAlpha, (BYTE)( fOn ? 255 : 0 ) );
	}

if (!(lpMask = (LPMASK)Alloc((long)(sizeof(MASK)+dx))))
	{
	FrameClose(lpAlpha);
	return( NULL );
	}
lpMask->mbuffer = Alloc(dx);
if (!lpMask->mbuffer)
	{
	FrameClose(lpAlpha);
	FreeUp((LPTR)lpMask);
	return( NULL );
	}
lpMask->mbufsize = dx;
lpMask->bInvert = NO;
lpMask->bInvertState = NO;
PixmapSetup(&lpMask->Pixmap, lpAlpha, fNoUndo);
if (fOn || lpAlphaIn)
	SetRect(&lpMask->rMask, 0, 0, dx-1, dy-1);
else
	AstralSetRectEmpty(&lpMask->rMask);
return(lpMask);
}

/************************************************************************/
BOOL MaskSizeChange(LPMASK lpMask, int dx, int dy)
/************************************************************************/
{
int dxCurrent = 0;
LPTR lp;

if (dx > lpMask->mbufsize)
	{
	if (!(lp = Alloc(dx)))
		return(FALSE);
	if (lpMask->mbuffer)
		FreeUp(lpMask->mbuffer);
	lpMask->mbuffer = lp;
	lpMask->mbufsize = dx;
	}
return(TRUE);
}

/************************************************************************/
BOOL MaskClose( LPMASK lpMask )
/************************************************************************/
{
if ( !lpMask )
	return( NO );

/* Free up the alpha frame */
PixmapFree(&lpMask->Pixmap);

/* free up the mask buffer line */
if (lpMask->mbuffer)
	FreeUp(lpMask->mbuffer);

/* Free up the mask itself */
FreeUp( (LPTR)lpMask );
return( YES );
}



/************************************************************************/
BOOL MaskRect( LPMASK lpMask, LPRECT lpRect )
/************************************************************************/
{
LPFRAME lpAlpha;

if ( !lpMask || !(lpAlpha = lpMask->Pixmap.EditFrame))
	{
	SetRectEmpty( lpRect );
	return( FALSE );
	}

// Start with the assumption of the entire image
lpRect->left = 0; lpRect->right = FrameXSize(lpAlpha) - 1;
lpRect->top = 0; lpRect->bottom = FrameYSize(lpAlpha) - 1;

if ( lpMask->bInvert )
	return( TRUE );

*lpRect = lpMask->rMask;
BoundRect( lpRect, 0, 0, FrameXSize(lpAlpha)-1, FrameYSize(lpAlpha)-1 );
return( TRUE );
}


/************************************************************************/
BOOL MaskRectUpdate( LPMASK lpMask, LPRECT lpRect )
/************************************************************************/
{
LPFRAME lpAlphaFrame;
int dx, dy;

if ( !lpMask || !(lpAlphaFrame = lpMask->Pixmap.EditFrame))
	{
	SetRectEmpty( lpRect );
	return( FALSE );
	}
dx = FrameXSize(lpAlphaFrame);
dy = FrameYSize(lpAlphaFrame);
SetRect(lpRect, 0, 0, dx-1, dy-1);
if (!MaskRectDwindle(lpMask, lpRect, NULL))
	{// assume the entire image and return FALSE
	SetRect( &lpMask->rMask, 0, 0, dx-1, dy-1 );
	*lpRect = lpMask->rMask;
	return( FALSE );
	}

lpMask->rMask = *lpRect;
if ( lpMask->bInvert )
	{ // Pass back the entire image if the mask is inverted
	SetRect( lpRect, 0, 0, dx-1, dy-1 );
	}
return( TRUE );
}

/************************************************************************/
BOOL MaskRectDwindle( LPMASK lpMask, LPRECT lpRect, LPFRAME lpAlphaFrame )
/************************************************************************/
{
long lx, lx1, lx2, lCount;
int x1, x2, y, y1, y2, dx, dy, depth;
BOOL bLeftEdge, bRightEdge;
LPTR lp;
RECT rAlpha;

if ( !lpAlphaFrame && (!lpMask || !(lpAlphaFrame = lpMask->Pixmap.EditFrame)))
	return( FALSE );

SetRect(&rAlpha, 0, 0, FrameXSize(lpAlphaFrame)-1, FrameYSize(lpAlphaFrame)-1);
if (!AstralIntersectRect(lpRect, lpRect, &rAlpha))
	return(FALSE);

dx = RectWidth(lpRect);
dy = RectHeight(lpRect);
if ( !(depth = FrameDepth(lpAlphaFrame)) )
	depth = 1;

for ( y1 = lpRect->top; y1 <= lpRect->bottom; y1++ )
	{
	if ( !(lp = FramePointer( lpAlphaFrame, lpRect->left, y1, NO, dx )) )
		break;
	lCount = (long)dx * depth;
	while ( --lCount >= 0 )
		{
		if ( *lp++ )
			goto FoundTop;
		}
	}

FoundTop:

if ( y1 > lpRect->bottom ) // If the mask is empty, ...
	return(FALSE);

for ( y2 = lpRect->bottom; y2 > y1; y2-- )
	{
	if ( !(lp = FramePointer( lpAlphaFrame, lpRect->left, y2, NO, dx )) )
		break;
	lCount = (long)dx * depth;
	while ( --lCount >= 0 )
		{
		if ( *lp++ )
			goto FoundBottom;
		}
	}

FoundBottom:

lx1 = +999999999;
lx2 = -999999999;
lCount = (long)dx * depth;
bLeftEdge = NO;
bRightEdge = NO;
for ( y = y1; y <= y2; y++ )
	{
	if ( !(lp = FramePointer( lpAlphaFrame, lpRect->left, y, NO, dx )) )
		break;
	if ( !bRightEdge )
		{ // If its worth doing a search for the right most...
		for ( lx = lCount-1; lx > 0; lx-- )
			{ // Do a byte search
			if ( *(lp+lx) )
				break;
			}
		if ( lx > lx2 )
			{
			lx2 = lx;
			if ( lCount - lx2 <= depth )
				bRightEdge = YES;
			}
		}
	if ( !bLeftEdge )
		{ // If its worth doing a search for the left most...
		for ( lx = 0; lx < lCount-1; lx++ )
			{ // Do a byte search
			if ( *(lp+lx) )
				break;
			}
		if ( lx < lx1 )
			{
			lx1 = lx;
			if ( lx1 < depth )
				bLeftEdge = YES;
			}
		}
	if ( bLeftEdge && bRightEdge )
		break;
	}

// Convert byte counts to pixel counts
x1 = lpRect->left + (lx1 / depth);
x2 = lpRect->left + (lx2 / depth);

// Set the mask rectangle
SetRect( lpRect, x1, y1, x2, y2 );
return(TRUE);
}

/************************************************************************/
int IsMasked( LPMASK lpMask, int x, int y )
/************************************************************************/
{
BYTE State;

if ( !MaskLoad( lpMask, x, y, 1, &State ) )
	return( 0 );
return( State );
}


/************************************************************************/
BOOL MaskLoaderHuge( LPMASK lpMask, int x, int y, int dx, HPTR lpM, BOOL bInvert, COMBINE_ROP rop )
/************************************************************************/
{
DWORD dwLeft;
BOOL bRet;

while (dx)
	{
	// get how many bytes left in segment
	dwLeft = 65536L - ((DWORD)lpM & 0xFFFFL);
	if (dwLeft > (DWORD)dx)
		dwLeft = dx;
	bRet = MaskLoad(lpMask, x, y, (int)dwLeft, lpM);
	dx -= (int)dwLeft;
	x += (int)dwLeft;
	lpM += dwLeft;
	}
return(bRet);
}

/************************************************************************/
BOOL MaskLoad( LPMASK lpMask, int x, int y, int dx, LPTR lpM )
/************************************************************************/
{
BOOL bRet;

if ( !(bRet = MaskLoader( lpMask, x, y, dx, lpM, NO, CR_COPY )) )
	clr( lpM, dx );
return( bRet );
}


/************************************************************************/
BOOL MaskLoader( LPMASK lpMask, int x, int y, int iCount, LPTR lpM,
				BOOL bInvert, COMBINE_ROP ROP)
/************************************************************************/
{
LPFRAME lpAlpha;
LPTR lp;
int iCountMax, Xsize, iCountClear = 0;
int lastx;
BYTE val;

if (bInvert)
	val = 0xff;
else
	val = 0x00;
	
if ( !lpMask )
{
	set( lpM, iCount, ~val); //squiggle
	return(YES);
}
if ( !(lpAlpha = lpMask->Pixmap.EditFrame) )
{
	set( lpM, iCount, ~val); //squiggle
	return( YES );
}
if ( y < 0 || y >= lpAlpha->Ysize )
{
	if (ROP == CR_COPY)
	{
		set(lpM, iCount, val);
		return(YES);
	}
	return( NO);
}
Xsize = lpAlpha->Xsize;
lastx = x + iCount - 1;
if ( lastx < 0 || x >= Xsize )
{
	if (ROP == CR_COPY)
	{
		set(lpM, iCount, val);
		return(YES);
	}
	return( NO);
}
if (x < 0)
	{
	iCountClear = -x;
	x = 0;
	}
lp = FramePointer( lpAlpha, x, y, NO, iCount );
if (iCountClear)
	{
	if (ROP == CR_COPY)
		set(lpM, iCountClear, val);
	lpM += iCountClear;
	iCount -= iCountClear;
	iCountClear = 0;
	}
iCountMax = Xsize - x;
if (iCount > iCountMax)
	{
	if (ROP == CR_COPY)
		iCountClear = iCount - iCountMax;
	iCount = iCountMax;
	}
if (lp)
{
	bInvert = (bInvert ^ lpMask->bInvert);
	CombineData( lp, lpM, iCount, bInvert, ROP );
}
else if (ROP == CR_COPY)
	set(lpM, iCount, val);
else
	return(NO);
	
if (iCountClear)
	if (ROP == CR_COPY)
		set(lpM+iCount, iCountClear, val);

return(YES);
}

/************************************************************************/
LPFRAME AlphaGetEditFrame(LPALPHA lpAlpha)
/***********************************************************************/
{
return(lpAlpha->Pixmap.EditFrame);
}

