//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

#define PATTERN_SIZE          8
#define PATTERN_SIZE_HALF     4
#define PATTERN_MARCH         1 // Up and to the left
// #define PATTERN_MARCH         7 // Down and to the right
#define PATTERN_MASK       0x07
#define PATTERN_THRESH      127

// Static prototypes
static BOOL GetMaskMarqueeDispRect(LPIMAGE lpImage, LPRECT lpRect);
static BOOL GetObjectMarqueeDispRect(LPIMAGE lpImage, LPRECT lpRect);
static BOOL ImgMarqueeAlloc(int FileWidth, int DispWidth);

static LPTR lpDispBuf[3];
static int iDispBufSize;
static LPTR lpMaskBuf[3];
static int iMaskBufSize;
static int nexty;
static int idPattern;

/***********************************************************************/
BOOL ShowMarquee(LPIMAGE lpImage, BOOL fShow)
/***********************************************************************/
{
BOOL fOldShow;

fOldShow = (lpImage->MarqueeStatus & MARQUEE_OFF) == 0;
// see if we need to remove the marquee
if (!fShow && fShow != fOldShow)
	RemoveMaskMarquee(lpImage);
if (fShow)
	lpImage->MarqueeStatus &= ~(MARQUEE_OFF); // beware of the
else
	lpImage->MarqueeStatus |= MARQUEE_OFF;
return(fOldShow);
}

/***********************************************************************/
BOOL IsMarqueeVisible(LPIMAGE lpImage)
/***********************************************************************/
{
return((lpImage->MarqueeStatus & MARQUEE_OFF) == 0);
}

/***********************************************************************/
void Marquee(BOOL fDisplayAll)
/***********************************************************************/
{
HDC hDC;
LPOBJECT lpObject;
LPMASK lpMask;
RECT rDisp, rMask, rObject;

if (!lpImage)
	return;
if ( lpImage->MarqueeStatus & MARQUEE_NOT_READY )
	return;
if ( GetUpdateRect( lpImage->hWnd, NULL, NO ) )
	return;
if ( !(hDC = Window.hDC) )
	return;
GetObjectMarqueeDispRect(lpImage, &rObject);
GetMaskMarqueeDispRect(lpImage, &rMask);
if (rMask.left > rMask.right && rObject.left > rObject.right)
	return;
AstralUnionRect(&rDisp, &rMask, &rObject);

if (fDisplayAll || nexty > rDisp.bottom || nexty < rDisp.top)
	{
	nexty = rDisp.top;
	idPattern = (idPattern+PATTERN_MARCH) & PATTERN_MASK;
	}
else
	rDisp.top = nexty;

if (rDisp.top >= rMask.top && rDisp.top <= rMask.bottom)
	{
	// if normal view mode always get the mask from the real base object
	lpObject = ImgGetMaskMarqueeObject(lpImage);
	lpMask = ObjGetAlpha(lpObject);

	if (FrameDepth(ImgGetBaseEditFrame(lpImage)) < 3)
	{
		PaintMarquee(lpImage, hDC, lpObject, lpMask, fDisplayAll,
			&rDisp, RGB(255,0,0), RGB(0,255,0), 0, 0);
	}
	else
	{
		PaintMarquee(lpImage, hDC, lpObject, lpMask, fDisplayAll,
			&rDisp, RGB(0,0,0), RGB(255,255,255), 0, 0);
	}

	// now draw marquees for selected objects
	if (rObject.left <= rObject.right)
		{
		if (lpObject = ImgGetObjectMarqueeObject(lpImage))
			PaintMarquee(lpImage, hDC, lpObject, NULL, YES, &rDisp,
								 RGB(0,0,0), RGB(0,255,255), 0, 0);
		}
	}
else
	{
	if (lpObject = ImgGetObjectMarqueeObject(lpImage))
		PaintMarquee(lpImage, hDC, lpObject, NULL, fDisplayAll, &rDisp,
								 RGB(0,0,0), RGB(0,255,255), 0, 0);
	if (rMask.left <= rMask.right)
		{
		// if normal view mode always get the mask from the real base object
		lpObject = ImgGetMaskMarqueeObject(lpImage);
		lpMask = ObjGetAlpha(lpObject);


		if (FrameDepth(ImgGetBaseEditFrame(lpImage)) < 3)
			{
			PaintMarquee(lpImage, hDC, lpObject, lpMask, YES,
				&rDisp, RGB(255,0,0), RGB(0,255,0), 0, 0);
			}
		else
			{
			PaintMarquee(lpImage, hDC, lpObject, lpMask, YES,
				&rDisp, RGB(0,0,0), RGB(255,255,255), 0, 0);
			}
		}
	}
nexty = rDisp.bottom+1;
}

/************************************************************************/
void UpdateMarquee(
/************************************************************************/
LPRECT 	lpRepairRect,
BOOL 	fInvalidate)
{
RECT rDisp;
LPOBJECT lpObject;
LPMASK lpMask;
HDC hDC;

if (!lpImage)
	return;
if ( !(hDC = Window.hDC) )
	return;
if ( lpImage->MarqueeStatus & MARQUEE_OFF)
	return;
if (lpRepairRect)
	{
	rDisp = *lpRepairRect;
	InflateRect(&rDisp, 1, 1);
	File2DispRect(&rDisp, &rDisp);
	}
else
	GetMaskMarqueeDispRect(lpImage, &rDisp);
if (rDisp.left > rDisp.right)
	return;
if ((lpObject = ImgGetMaskMarqueeObject(lpImage)) &&
	(lpMask = ObjGetAlpha(lpObject)))
	{
	if (FrameDepth(ImgGetBaseEditFrame(lpImage)) < 3)
		{
		PaintMarquee(lpImage, hDC, lpObject, lpMask, YES,
			&rDisp, RGB(255,0,0), RGB(0,255,0), 0, 0);
		}
	else
		{
		PaintMarquee(lpImage, hDC, lpObject, lpMask, YES,
			&rDisp, RGB(0,0,0), RGB(255,255,255), 0, 0);
		}
	}
}

/***********************************************************************/
BOOL EnableMarquee(BOOL bEnable)
/***********************************************************************/
{
int MarqueeStatus;

if (!lpImage)
	return(FALSE);
MarqueeStatus = lpImage->MarqueeStatus;
if (bEnable)
		lpImage->MarqueeStatus &= ~(MARQUEE_NOT_READY); // beware of the
else	lpImage->MarqueeStatus |= MARQUEE_NOT_READY;
return(!(MarqueeStatus & MARQUEE_NOT_READY));
}

/***********************************************************************/
void EnableMaskDisplay(LPIMAGE lpImage, BOOL fEnable)
/***********************************************************************/
{
if (!lpImage)
	return;

if (fEnable)
	{
	EnableMarquee( YES );
	EnableSlime( lpImage, YES );
	}
else
	{
	EnableMarquee( NO );
	RemoveMarquee();
	EnableSlime( lpImage, NO );
	}
}

/***********************************************************************/
void EnableSlime( LPIMAGE lpImage, BOOL fEnable)
/***********************************************************************/
{
RECT rUpdate;

if (!lpImage)
	return;

if (ImgGetMaskUpdateRect(lpImage, YES, NO, &rUpdate))
	{
	File2DispRect(&rUpdate, &rUpdate);
	rUpdate.right++; rUpdate.bottom++;
	InvalidateRect(lpImage->hWnd, &rUpdate, FALSE);
	}
}

/***********************************************************************/
void RemoveMarquee(void)
/***********************************************************************/
{
RECT rMask, rObject, rDisp;

if ( !lpImage )
	return;
GetObjectMarqueeDispRect(lpImage, &rObject);
GetMaskMarqueeDispRect(lpImage, &rMask);
if (rMask.left > rMask.right && rObject.left > rObject.right)
	return;
AstralUnionRect(&rDisp, &rMask, &rObject);
rDisp.right++; rDisp.bottom++;
InvalidateRect( lpImage->hWnd, &rDisp, FALSE );
AstralUpdateWindow( lpImage->hWnd );
}

/***********************************************************************/
void RemoveObjectMarquee(LPIMAGE lpImage)
/***********************************************************************/
{
RemoveObjectMarqueeEx(lpImage, YES);
}

/***********************************************************************/
void RemoveObjectMarqueeEx(LPIMAGE lpImage, BOOL fUpdate)
/***********************************************************************/
{
RECT rDisp;

if ( !lpImage )
	return;
GetObjectMarqueeDispRect(lpImage, &rDisp);
if (rDisp.left > rDisp.right)
	return;
rDisp.right++; rDisp.bottom++;
InvalidateRect( lpImage->hWnd, &rDisp, FALSE );
if (fUpdate)
	AstralUpdateWindow( lpImage->hWnd );
}

/***********************************************************************/
void RemoveMaskMarquee(LPIMAGE lpImage)
/***********************************************************************/
{
RECT rMask, rDisp;

if ( !lpImage )
	return;
GetMaskMarqueeDispRect(lpImage, &rMask);
if (rMask.left > rMask.right)
	return;
rDisp = rMask;
rDisp.right++; rDisp.bottom++;
InvalidateRect( lpImage->hWnd, &rDisp, FALSE );
AstralUpdateWindow( lpImage->hWnd );
}

/***********************************************************************/
static BOOL GetMaskMarqueeDispRect(LPIMAGE lpImage, LPRECT lpRect)
/***********************************************************************/
{
LPMASK lpMask;

SetRect(lpRect, 32767, 32767, -32767, -32767);

if (lpMask = ImgGetMarqueeMask(lpImage))
	MaskRect(lpMask, lpRect);
if (lpRect->left <= lpRect->right)
	{
	InflateRect(lpRect, 1, 1);
	File2DispRect(lpRect, lpRect);
	return(TRUE);
	}
else
	return(FALSE);
}

/***********************************************************************/
static BOOL GetObjectMarqueeDispRect(LPIMAGE lpImage, LPRECT lpRect)
/***********************************************************************/
{
LPOBJECT lpObject, lpBaseObject;

SetRect(lpRect, 32767, 32767, -32767, -32767);
if (ImgMultipleObjects(lpImage) &&
	(lpObject = ImgGetObjectMarqueeObject(lpImage)))
	{
	lpBaseObject = ImgGetBase(lpImage);
	while (lpObject)
		{
		if (!lpObject->fTransform)
			{
			if (lpObject == lpBaseObject)
				{ // can't repaint more than whole display
				*lpRect = lpImage->lpDisplay->DispRect;
				return(TRUE);
				}
			AstralUnionRect(lpRect, lpRect, &lpObject->rObject);
			}
		lpObject = ImgGetSelObject(lpImage, lpObject);
		}
	}
if (lpRect->left <= lpRect->right)
	{
	InflateRect(lpRect, 1, 1);
	File2DispRect(lpRect, lpRect);
	return(TRUE);
	}
else
	return(FALSE);
}

/************************************************************************/
BOOL PaintMarquee( LPIMAGE lpImage, HDC hDC, LPOBJECT lpObject,
						LPMASK lpMask,
						BOOL fDisplayAll, LPRECT lpUpdate,
						COLOR Color1, COLOR Color2, int xOffset, int yOffset )
/************************************************************************/
{
	int sdx, ddx, ddy, i;
	int FileWidth, DispWidth, PaintWidth;
	int y, yline, ylast, xstart, xend, xtotal;
	int dxCount, increment;
	LFIXED yrate, xrate, yoffset, xoffset;
	RECT rPaint, rFile, rDisp, rObject;
	LPTR lpMaskPtr[3];
	BYTE p, n, c;
	LPTR lpP, lpC, lpN;
	int x;
	int prev, curr, next, temp, cury;
	LPOBJECT lpBase;
	int OldROP;
	int iOffset;

	// get display and file rectangles for easier access
	rDisp = lpImage->lpDisplay->DispRect;
	rFile = lpImage->lpDisplay->FileRect;

	if (lpMask)
	{
		// get rectangle for mask adjusted by object location
		if ( !lpObject )
			return(NO);
		MaskRect( lpMask, &rPaint );
		InflateRect(&rPaint, 1, 1);
		rObject = lpObject->rObject;
		OffsetRect(&rPaint, rObject.left, rObject.top);
		File2DispRect(&rPaint, &rPaint);

		// if we are updating a specific area, see if it intersects the mask
		if (!AstralIntersectRect(&rPaint, &rPaint, lpUpdate))
			return(NO);
	}
	else
		rPaint = *lpUpdate;

	// does this mask intersect the display?
	if (!AstralIntersectRect(&rPaint, &rPaint, &rDisp))
		return(NO);

	// get pixel width of display and file
	DispWidth = RectWidth( &rDisp );
	FileWidth = RectWidth( &rFile );
	PaintWidth = RectWidth( &rPaint );

	if (!ImgMarqueeAlloc(FileWidth, DispWidth))
		return(FALSE);

	if (Color1 == Color2)
	{
		OldROP = SetROP2(hDC, R2_NOT);
		fDisplayAll = YES;
	}

	// compute x and y display rates
	yrate = FGET(RectHeight( &rFile),RectHeight(&rDisp));
	xrate = FGET(FileWidth, DispWidth);

	// get offsets from display edge to paint rect to adjust xoffset/yoffset
	ddx = rPaint.left - rDisp.left;
	ddy = rPaint.top - rDisp.top;
	dxCount = ddx + PaintWidth;
	yoffset = (ddy*yrate)+((long)yrate>>1);
	xoffset = (ddx*xrate)+((long)xrate>>1);

	// Find x-position in file coordinates where painting begins and ends
#ifdef WIN32				
	sdx = WHOLE(xoffset);
	xstart = rFile.left + sdx;
	xoffset += ((PaintWidth-1)*xrate);
	xend = rFile.left + WHOLE(xoffset);
#else
	sdx = HIWORD(xoffset);
	xstart = rFile.left + sdx;
	xoffset += ((PaintWidth-1)*xrate);
	xend = rFile.left + HIWORD(xoffset);
#endif

	xtotal = xend - xstart + 1;
	for (i = 0; i < 3; ++i)
		lpMaskPtr[i] = lpMaskBuf[i] + (long)sdx;

	prev = 0;
	curr = 1;
	next = 2;

	// get mask for previous line
	yoffset -= yrate;
#ifdef WIN32
	yline = rFile.top + WHOLE( yoffset );
#else
	yline = rFile.top + HIWORD( yoffset );
#endif
	ImgMaskLoad(lpImage, (LPOBJECT)lpObject, lpMask, xstart, yline, xtotal,
				lpMaskPtr[prev], lpMask == NULL, CR_XOR, xOffset, yOffset);
	Sample_8(lpMaskBuf[prev], 0, lpDispBuf[prev], ddx, dxCount, xrate);

	// get mask for first line
	yoffset += yrate;
#ifdef WIN32
	cury = yline = rFile.top + WHOLE( yoffset );
#else
	cury = yline = rFile.top + HIWORD( yoffset );
#endif
	ImgMaskLoad(lpImage, (LPOBJECT)lpObject, lpMask, xstart, yline, xtotal,
				lpMaskPtr[curr], lpMask == NULL, CR_XOR, xOffset, yOffset);
	Sample_8(lpMaskBuf[curr], 0, lpDispBuf[curr], ddx, dxCount, xrate);
	ylast = yline;

	// setup for getting line following current line
	yoffset += yrate;
	increment = 10;
	lpBase = ImgGetBase(lpImage);
	for ( y=rPaint.top; y<=rPaint.bottom; y++ )
	{
		iOffset = y + idPattern;

		if (!fDisplayAll && (--increment <= 0))
		{
			increment = 10;
			if (AbortUpdate())
			{
				lpUpdate->bottom = y;
				break;
			}
		}
#ifdef WIN32
		yline = rFile.top + WHOLE( yoffset );
#else
		yline = rFile.top + HIWORD( yoffset );
#endif
		yoffset += yrate;
		if ( yline != ylast )
		{
			ylast = yline;
			ImgMaskLoad(lpImage, (LPOBJECT)lpObject, lpMask, xstart, yline, xtotal,
				lpMaskPtr[next], lpMask == NULL, CR_XOR, xOffset, yOffset);
			Sample_8(lpMaskBuf[next], 0, lpDispBuf[next], ddx, dxCount, xrate);
		}
		else
		{
			copy(lpDispBuf[curr], lpDispBuf[next], PaintWidth);
		}

		lpP = lpDispBuf[prev];
		lpC = lpDispBuf[curr];
		lpN = lpDispBuf[next];

		if (rPaint.left == rDisp.left)
		{
			ImgMaskLoad(lpImage, (LPOBJECT)lpObject, lpMask, xstart-1, cury, 1,
				&p, lpMask == NULL, CR_XOR, xOffset, yOffset);
			x = rPaint.left;
		}
		else
		{
			p = *lpC++;
			++lpP;
			++lpN;
			x = rPaint.left+1;
		}

		c = *lpC++;

		for (; x < rPaint.right; ++x)
		{
			if (x == (rDisp.right-1))
			{
				ImgMaskLoad(lpImage, (LPOBJECT)lpObject, lpMask, xend+1, cury, 1,
						&n, lpMask == NULL, CR_XOR, xOffset, yOffset);
			}
			else
				n = *lpC++;

			if (c > PATTERN_THRESH)  // a pixel is on
			{
				// If any neighbors are off...
				if ((p    <= PATTERN_THRESH) ||
					(n    <= PATTERN_THRESH) ||
					(*lpP <= PATTERN_THRESH) ||
					(*lpN <= PATTERN_THRESH))
				{
					SetPixel(hDC, x, y,
						((iOffset+x)&PATTERN_MASK) >= PATTERN_SIZE_HALF ? 
						Color1 : Color2 );
				}
			}
			p = c;
			c = n;
			++lpP;
			++lpN;
		}

		temp = prev;
		prev = curr;
		curr = next;
		next = temp;
		cury = yline;
	}

	if (Color1 == Color2)
		SetROP2(hDC, OldROP);

	return(TRUE);
}

/************************************************************************/
static BOOL ImgMarqueeAlloc(int FileWidth, int DispWidth)
/************************************************************************/
{
int 	i;

if (DispWidth != iDispBufSize)
	{
	if (lpDispBuf[0])
		{
		FreeUp(lpDispBuf[0]);
		lpDispBuf[0] = NULL;
		}
	iDispBufSize = DispWidth;
	if (!AllocLines(lpDispBuf, 3, iDispBufSize, 1))
		{
		iDispBufSize = 0;
		return(NO);
		}
	}
if (FileWidth != iMaskBufSize)
	{
	for (i = 0; i < 3; ++i)
		if (lpMaskBuf[i])
			{
			FreeUp(lpMaskBuf[i]);
			lpMaskBuf[i] = NULL;
			}
	iMaskBufSize = FileWidth;
	for (i = 0; i < 3; ++i)
		{
		if (!(lpMaskBuf[i] = Alloc((long)iMaskBufSize)))
			{
			iMaskBufSize = 0;
			return(NO);
			}
		}
	}
return(YES);
}

