//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

#define PATTERN_SIZE          8
#define PATTERN_SIZE_HALF     4
#define PATTERN_MARCH         1 // Up and to the left
// #define PATTERN_MARCH         7 // Down and to the right
#define PATTERN_MASK       0x07
#define PATTERN_THRESH      127

// Static prototypes
static BOOL GetMaskMarqueeDispRect(HWND hWnd, LPRECT lpRect);
static BOOL GetObjectMarqueeDispRect(HWND hWnd, LPRECT lpRect);
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
void Marquee( BOOL fDisplayAll )
/***********************************************************************/
{
HDC hDC;
HWND hActiveWnd;
LPIMAGE lpImage;
LPOBJECT lpObject;
LPMASK lpMask;
RECT rDisp, rMask, rObject;

if (!(hActiveWnd = GetActiveDoc()))
	return;
lpImage = GetImagePtr(hActiveWnd);
if (ImageIsBusy(lpImage))
	return;
if ( lpImage->MarqueeStatus & MARQUEE_NOT_READY )
	return;
if ( GetUpdateRect( hActiveWnd, NULL, NO ) )
	return;
if ( !(hDC = Window.hDC) )
	return;
GetObjectMarqueeDispRect(hActiveWnd, &rObject);
GetMaskMarqueeDispRect(hActiveWnd, &rMask);
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

	if (FrameType(ImgGetBaseEditFrame(lpImage)) <= FDT_GRAYSCALE)
	{
		PaintMarquee(hActiveWnd, hDC, lpObject, lpMask, fDisplayAll,
			&rDisp, RGB(255,0,0), RGB(0,255,0), 0, 0);
	}
	else
	{
		PaintMarquee(hActiveWnd, hDC, lpObject, lpMask, fDisplayAll,
			&rDisp, RGB(0,0,0), RGB(255,255,255), 0, 0);
	}

	// now draw marquees for selected objects
	if (rObject.left <= rObject.right)
		{
		if (lpObject = ImgGetObjectMarqueeObject(lpImage))
			PaintMarquee(hActiveWnd, hDC, lpObject, NULL, YES, &rDisp,
								 RGB(0,0,0), RGB(0,255,255), 0, 0);
		}
	}
else
	{
	if (lpObject = ImgGetObjectMarqueeObject(lpImage))
		PaintMarquee(hActiveWnd, hDC, lpObject, NULL, fDisplayAll, &rDisp,
								 RGB(0,0,0), RGB(0,255,255), 0, 0);
	if (rMask.left <= rMask.right)
		{
		// if normal view mode always get the mask from the real base object
		lpObject = ImgGetMaskMarqueeObject(lpImage);
		lpMask = ObjGetAlpha(lpObject);


		if (FrameType(ImgGetBaseEditFrame(lpImage)) <= FDT_GRAYSCALE)
			{
			PaintMarquee(hActiveWnd, hDC, lpObject, lpMask, YES,
				&rDisp, RGB(255,0,0), RGB(0,255,0), 0, 0);
			}
		else
			{
			PaintMarquee(hActiveWnd, hDC, lpObject, lpMask, YES,
				&rDisp, RGB(0,0,0), RGB(255,255,255), 0, 0);
			}
		}
	}
nexty = rDisp.bottom+1;
}

/************************************************************************/
void UpdateMarquee( HWND hWnd, LPRECT lpRepairRect, BOOL fInvalidate )
/************************************************************************/
{
RECT rDisp;
LPOBJECT lpObject;
LPMASK lpMask;
HDC hDC;
LPIMAGE lpImage;

lpImage = GetImagePtr(hWnd);

if ( !(hDC = Window.hDC) )
	return;
if ( lpImage->MarqueeStatus & MARQUEE_OFF)
	return;
if (lpRepairRect)
	{
	rDisp = *lpRepairRect;
	InflateRect(&rDisp, 1, 1);
	File2DispRect(hWnd, &rDisp, &rDisp);
	}
else
	GetMaskMarqueeDispRect(hWnd, &rDisp);
if (rDisp.left > rDisp.right)
	return;
if ((lpObject = ImgGetMaskMarqueeObject(lpImage)) &&
	(lpMask = ObjGetAlpha(lpObject)))
	{
	if (FrameType(ImgGetBaseEditFrame(lpImage)) <= FDT_GRAYSCALE)
		{
		PaintMarquee(hWnd, hDC, lpObject, lpMask, YES,
			&rDisp, RGB(255,0,0), RGB(0,255,0), 0, 0);
		}
	else
		{
		PaintMarquee(hWnd, hDC, lpObject, lpMask, YES,
			&rDisp, RGB(0,0,0), RGB(255,255,255), 0, 0);
		}
	}
}

/***********************************************************************/
BOOL EnableMarquee( BOOL bEnable )
/***********************************************************************/
{
LPIMAGE lpImage;
int MarqueeStatus;

if (!(lpImage = GetActiveImage()))
	return(FALSE);
MarqueeStatus = lpImage->MarqueeStatus;
if (bEnable)
		lpImage->MarqueeStatus &= ~(MARQUEE_NOT_READY); // beware of the
else	lpImage->MarqueeStatus |= MARQUEE_NOT_READY;
return(!(MarqueeStatus & MARQUEE_NOT_READY));
}

/***********************************************************************/
BOOL EnableMarqueeEx(LPIMAGE lpImage, BOOL bEnable)
/***********************************************************************/
{
int MarqueeStatus;

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
HWND hActiveWnd;

if (!(hActiveWnd = GetActiveDoc()))
	return;

if (ImgGetMaskUpdateRect(lpImage, YES, NO, &rUpdate))
	{
	File2DispRect(hActiveWnd, &rUpdate, &rUpdate);
	rUpdate.right++; rUpdate.bottom++;
	InvalidateRect(hActiveWnd, &rUpdate, FALSE);
	}
}

/***********************************************************************/
void RemoveMarquee(void)
/***********************************************************************/
{
HWND hActiveWnd;
RECT rMask, rObject, rDisp;

if (!(hActiveWnd = GetActiveDoc()))
	return;
GetObjectMarqueeDispRect(hActiveWnd, &rObject);
GetMaskMarqueeDispRect(hActiveWnd, &rMask);
if (rMask.left > rMask.right && rObject.left > rObject.right)
	return;
AstralUnionRect(&rDisp, &rMask, &rObject);
rDisp.right++; rDisp.bottom++;
InvalidateRect( hActiveWnd, &rDisp, FALSE );
AstralUpdateWindow( hActiveWnd );
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
HWND hActiveWnd;

if (!(hActiveWnd = GetActiveDoc()))
	return;

GetObjectMarqueeDispRect(hActiveWnd, &rDisp);
if (rDisp.left > rDisp.right)
	return;
rDisp.right++; rDisp.bottom++;
InvalidateRect( hActiveWnd, &rDisp, FALSE );
if (fUpdate)
	AstralUpdateWindow( hActiveWnd );
}

/***********************************************************************/
void RemoveMaskMarquee(LPIMAGE lpImage)
/***********************************************************************/
{
RECT rMask, rDisp;

HWND hActiveWnd;

if (!(hActiveWnd = GetActiveDoc()))
	return;
GetMaskMarqueeDispRect(hActiveWnd, &rMask);
if (rMask.left > rMask.right)
	return;
rDisp = rMask;
rDisp.right++; rDisp.bottom++;
InvalidateRect( hActiveWnd, &rDisp, FALSE );
AstralUpdateWindow( hActiveWnd );
}

/***********************************************************************/
static BOOL GetMaskMarqueeDispRect(HWND hWnd, LPRECT lpRect)
/***********************************************************************/
{
LPIMAGE lpImage;

lpImage = GetImagePtr(hWnd);
GetMaskMarqueeRect(lpImage, lpRect);
if (!AstralIsRectEmpty(lpRect))
	{
	InflateRect(lpRect, 1, 1);
	File2DispRect(hWnd, lpRect, lpRect);
	return(TRUE);
	}
else
	return(FALSE);
}

/***********************************************************************/
BOOL GetMaskMarqueeRect(LPIMAGE lpImage, LPRECT lpRect)
/***********************************************************************/
{
LPMASK lpMask;

AstralSetRectEmpty(lpRect);
if (lpMask = ImgGetMarqueeMask(lpImage))
	MaskRect(lpMask, lpRect);
return(!AstralIsRectEmpty(lpRect));
}

/***********************************************************************/
static BOOL GetObjectMarqueeDispRect(HWND hWnd, LPRECT lpRect)
/***********************************************************************/
{
LPIMAGE lpImage;

lpImage = GetImagePtr(hWnd);
GetObjectMarqueeRect(lpImage, lpRect);
if (lpRect->left <= lpRect->right)
	{
	InflateRect(lpRect, 1, 1);
	File2DispRect(hWnd, lpRect, lpRect);
	return(TRUE);
	}
else
	return(FALSE);
}

/***********************************************************************/
BOOL GetObjectMarqueeRect(LPIMAGE lpImage, LPRECT lpRect)
/***********************************************************************/
{
LPOBJECT lpObject, lpBaseObject;
int dx, dy;

AstralSetRectEmpty(lpRect);
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
				ImgGetInfo(lpImage, &dx, &dy, NULL, NULL);
				SetRect(lpRect, 0, 0, dx-1, dy-1);
				return(TRUE);
				}
			AstralUnionRect(lpRect, lpRect, &lpObject->rObject);
			}
		lpObject = ImgGetSelObject(lpImage, lpObject);
		}
	}
return(lpRect->left <= lpRect->right);
}

/************************************************************************/
BOOL PaintMarquee( HWND hWnd, HDC hDC, LPOBJECT lpObject,
						LPMASK lpMask,
						BOOL fDisplayAll, LPRECT lpUpdate,
						COLOR Color1, COLOR Color2, int xOffset, int yOffset )
/************************************************************************/
{
	int sdx, ddx, ddy, i;
	int FileWidth, DispWidth, PaintWidth;
	int y, yline, ylast, xstart, xend, xtotal, right;
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
	LPIMAGE lpImage;
	LPDISPLAY lpDisplay;

	lpImage = GetImagePtr(hWnd);
	lpDisplay = GetDisplayPtr(hWnd);

	// get display and file rectangles for easier access
	rDisp = lpDisplay->DispRect;
	rFile = lpDisplay->FileRect;

	if (lpMask)
	{
		// get rectangle for mask adjusted by object location
		if ( !lpObject )
			return(NO);
		MaskRect( lpMask, &rPaint );
		InflateRect(&rPaint, 1, 1);
		rObject = lpObject->rObject;
		OffsetRect(&rPaint, rObject.left, rObject.top);
		File2DispRect(GetActiveDoc(), &rPaint, &rPaint);

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
	yoffset = (yrate*ddy)+FDIV2(yrate);
	xoffset = (xrate*ddx)+FDIV2(xrate);

	// Find x-position in file coordinates where painting begins and ends
	sdx = WHOLE(xoffset);
	xstart = rFile.left + sdx;
	xoffset += (xrate*(PaintWidth-1));
	xend = rFile.left + WHOLE(xoffset);

	xtotal = xend - xstart + 1;
	for (i = 0; i < 3; ++i)
		lpMaskPtr[i] = lpMaskBuf[i] + (long)sdx;

	prev = 0;
	curr = 1;
	next = 2;

	// get mask for previous line
	yoffset -= yrate;
	yline = rFile.top + WHOLE(yoffset);
	ImgMaskLoad(lpImage, (LPOBJECT)lpObject, lpMask, xstart, yline, xtotal,
				lpMaskPtr[prev], lpMask == NULL, CR_XOR, xOffset, yOffset);
	Sample_8(lpMaskBuf[prev], 0, lpDispBuf[prev], ddx, dxCount, xrate);

	// get mask for first line
	yoffset += yrate;
	cury = yline = rFile.top + WHOLE(yoffset);
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

		yline = rFile.top + WHOLE(yoffset);
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

		if (rPaint.right == rDisp.right)
			right = rPaint.right;
		else
			right = rPaint.right - 1;
	
		c = *lpC++;

		for (; x <= right; ++x)
		{
			if (x == rDisp.right)
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



