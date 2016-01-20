// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"

extern HANDLE hInstAstral;

/***********************************************************************/
void Marquee(fDisplayAll)
/***********************************************************************/
BOOL fDisplayAll;
{
int x1, y1, x2, y2;
LPFRAME lpFrame;
LPMASK lpMask;
LPPOINT lpMarquee;
static LPPOINT lpHeadMarquee, lpNextMarquee, lpLastMarquee;
static int idPattern;
HDC hDC;

//if ( lpImage->MarqueeStatus & MARQUEE_NOT_READY )
//	return;
if (!lpImage)
    return;
if ( !Window.hDC )
	return;
if ( !(lpFrame = frame_set(NULL)) )
	return;
if ( !(lpMask = lpFrame->WriteMask) )
	return;
if ( !(lpMarquee = lpMask->lpMarquee) )
	return;
hDC = Window.hDC;
if (fDisplayAll) /* start at beginning of list */
    {
    lpHeadMarquee  = NULL;
    hDC = GetDC(lpImage->hWnd);
    }
while (TRUE)
    {
    /* If the marquee pointers have never been set, or there is a */
    /* different mask, setup the pointers into the marquee list */
    if ( (lpHeadMarquee != lpMarquee) ||
         (lpLastMarquee != lpMarquee + lpMask->nMarqueePoints - 1) )
	{
	lpHeadMarquee  = lpMarquee;
	lpLastMarquee  = lpMarquee + lpMask->nMarqueePoints - 1;
	lpNextMarquee  = lpMarquee;
	}

    /* If we have finished drawing everything in the marquee list, */
    /* reset to the top (unless we're going to wait for more) */
    if ( lpNextMarquee >= lpLastMarquee )
	{
	if ( lpImage->MarqueeStatus & MARQUEE_CYCLE )
		{
		if ( ++idPattern >= 8 )
			idPattern = 0;
		lpNextMarquee = lpHeadMarquee;
		}
	break;
	}

    /* Extract the x and y values to draw */
    x1 = lpNextMarquee->x;
    y1 = lpNextMarquee->y;
    lpNextMarquee++;
    x2 = lpNextMarquee->x;
    y2 = lpNextMarquee->y;
    if ( x2 == PENUP_MARKER && y2 == PENUP_MARKER )
	{ /* If we run into a "pen up" marker, skip over it */
	lpNextMarquee++;
        if (fDisplayAll)
	    continue;
	else
	    break;
	}

    /* Draw the next vector in the marquee list */
    File2Display( &x1, &y1 );
    File2Display( &x2, &y2 );
    DottedLine( hDC, x1, y1, x2, y2, idPattern );
    if (!fDisplayAll)
        break;
    }
if (fDisplayAll)
    ReleaseDC(lpImage->hWnd, hDC);
}


static HDC hDCsaved;
static int idSaved;
//static BYTE DottedPattern[24] =
//{ 1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0 };

/***********************************************************************/
void DottedLine( hDC, x1, y1, x2, y2, idPattern )
/***********************************************************************/
HDC hDC;
int x1, y1, x2, y2, idPattern;
{
static FARPROC hDottedLineInst;
#define SPEED 3

if (!hDottedLineInst)
    hDottedLineInst = MakeProcInstance(DottedLineProc, hInstAstral);
hDCsaved = hDC;
idSaved = idPattern * SPEED;
LineDDA( x1, y1, x2, y2, hDottedLineInst, NULL );
}


/***********************************************************************/
void far PASCAL DottedLineProc( x, y, lpData )
/***********************************************************************/
int x, y;
LPTR lpData;
{
static WORD offset;
COLOR Color;

//offset = 16 - idSaved - (y%8) + (x%8);
//if ( DottedPattern[offset] )
offset++;
if ( ((offset+idSaved) % 16) < 8 )
	Color = RGB(255,0,0);
else	Color = RGB(0,255,0);
SetPixel( hDCsaved, x, y, Color );
}


/***********************************************************************/
void BuildMarquee()
/***********************************************************************/
{
LPFRAME lpFrame;
LPMASK lpMask;
int nPointEstimate;

if (!lpImage)
    return;
if ( !(lpFrame = frame_set(NULL)) )
	return;
if ( !(lpMask = lpFrame->WriteMask) )
	return;
lpImage->MarqueeStatus = MARQUEE_NOT_READY;

/* If we already have a marquee list, free it up */
if ( lpMask->lpMarquee )
	FreeUp( (LPTR)lpMask->lpMarquee );
lpMask->lpMarquee = NULL;

/* Estimate the number of points needed to display the marquee */
nPointEstimate = BuildEdgesFromShapes(NULL,  lpMask->lpHeadShape, NULL, NULL, NULL, TRUE );

if (nPointEstimate)
    {
    /* Allocate the space for the marquee */
    lpMask->lpMarquee = (LPPOINT)Alloc( (long)sizeof(POINT) * nPointEstimate );
    if ( !lpMask->lpMarquee )
	return;

    /* Generate the points for the marquee from each shape */
    lpMask->nMarqueePoints = BuildEdgesFromShapes(NULL, lpMask->lpHeadShape,
    lpMask->lpMarquee, BuildDxMarqueeProc, BuildDyMarqueeProc, FALSE );
    //Print("MarqueeEstimate=%d, MarqueeExact=%d",
    //	nPointEstimate, lpMask->nMarqueePoints);
    lpImage->MarqueeStatus = MARQUEE_CYCLE;
    }
}
