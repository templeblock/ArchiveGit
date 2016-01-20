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

extern int FullBrush;

/************************************************************************/
/*  Brush Delta Definition						*/
/************************************************************************/
typedef struct _element
	{
	RECT rStrip;
	struct _element far *next;
} ELEMENT;
typedef ELEMENT far *LPELEMENT;

static fDisplayOn;	/* Engine strokes are displayed if TRUE */
static LPTR pm;		/* DMASK pointer */
static LPTR pd;		/* DEST pointer */
static LPTR pdsavecurr;		/* DEST pointer */
static LPTR pdsavelast;		/* DEST pointer */
static LPTR ps;		/* SRC pointer */
static LPTR pb;		/* BMASK pointer */
static LPINT pe;		/* error buffer pointer */
static LPTR pm1;	/* Primary rectangle mask pointer */
static LPTR pm2;	/* Secondary rectangle mask pointer */
static LPTR pmsave;	/* Secondary rectangle mask pointer */
static int bh, bv;
static int BrushFull, BrushHalf;
static int cursor_x, cursor_y;
static BOOL UsingMask, onMask;
static LPFRAME lpSrcFrame;
static LPFRAME lpDstFrame;
static LPFRAME lpOldFrame;
static LPROC lpStrokeFunction;
static LPROC lpLineFunction;
static LPROC lpMergeProc;
static LPROC lpMaskProc;
static LPROC lpProcessProc;
static FIXED FadeOut;
static FIXED FadeBrushSize;
static int iTileMode;
static int BrushSize;
static int bcount;
static int start_dx, start_dy, end_dx, end_dy;
static int sx_delta, sy_delta;
static BOOL fFirstStroke, fCheckLastStroke, fHorzStrips, fRightBottom, fDone;
static BOOL fDoDDA = TRUE;
static RECT rMax;
static LPELEMENT ElementHead = NULL, lpElement1, lpElement2;
static int Strip1Width, Strip2Width;
static RECT rStrip1, rStrip2, rSave, rStripSave;

static BOOL InsertElement(LPELEMENT);
static BOOL AddElement(LPELEMENT);
static int  NumElements(void);
static LPELEMENT GetElement(int);
static BOOL RemElement(int);
static void DestroyElements(void);

/************************************************************************/
BOOL LineEngine(lpRect, lpRGB, lpSourceFrame, lpDestFrame, lpFunction, TileMode, Opacity)
/************************************************************************/
LPRECT lpRect;
LPRGB lpRGB;
LPFRAME lpSourceFrame, lpDestFrame;
LPROC lpFunction;
int TileMode;
int Opacity;
{
int y;
RECT rDest, rSource, rOutput, rLines;
int dx, dy, bh, bv, bufsize, sx, sy, depth;

if (!lpDestFrame)
	return(FALSE);
lpSrcFrame = lpSourceFrame;
lpDstFrame = lpDestFrame;
lpLineFunction = lpFunction;
iTileMode = TileMode;
fDisplayOn = NO;

UsingMask = (lpDstFrame->WriteMask != NULL) || (ColorMask.Mask && !ColorMask.Off);
if (lpDstFrame->WriteMask)
	onMask = !Mask.IsOutside;
else
	onMask = FALSE;

rLines.left = bound(lpRect->left, 0, lpImage->npix-1);
rLines.right = bound(lpRect->right, 0, lpImage->npix-1);
rLines.top = bound(lpRect->top, 0, lpImage->nlin-1);
rLines.bottom = bound(lpRect->bottom, 0, lpImage->nlin-1);
dx = rLines.left;
bh = RectWidth(&rLines);
bv = RectHeight(&rLines);
depth = lpDstFrame->Depth;
pm = Alloc((long)bh);
ps = Alloc((long)(bh * depth));
if (!pm || !ps)
	{
	if (pm)
		FreeUp(pm);
	if (ps)
		FreeUp(ps);
	return(FALSE);
	}
if (depth == 3)
    {
    lpMaskProc = mload24;
    lpProcessProc = ProcessData24;
    }
else if (depth == 2)
    {
    lpMaskProc = mload16;
    lpProcessProc = ProcessData16;
    }
else
    {
    lpMaskProc = mload8;
    lpProcessProc = ProcessData8;
    }
if (lpRGB)
	{
	bload(0, 0, bh, 1, ps, NULL, &rSource, iTileMode, lpRGB);
	}
AstralCursor( IDC_WAIT );
for (y = rLines.top; y <= rLines.bottom; ++y)
	{
	AstralClockCursor(y-rLines.top, bv);
	if (!(pd = CachePtr(0, dx, y, YES)))
		continue;
	if ( lpSrcFrame )
		if (!bload( dx, y, bh, 1, ps, lpSrcFrame, &rSource, iTileMode, NULL ))
			continue;

	if (lpLineFunction)
        	(*lpLineFunction)(y, rLines.left, rLines.right, pd, ps);

	if (UsingMask)		/* If DMASK being used */
		{
		(*lpMaskProc)( dx, y, bh, 1, pm, pd ); /* load the DMASK area */
		setifset(pm, bh, Opacity);
		}
	else
		set(pm, bh, Opacity);
	(*lpProcessProc)(pd, ps, pm, bh);
	}
AstralCursor( NULL );
AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &rLines);

if (pm)
	FreeUp(pm);
if (ps)
	FreeUp(ps);
return(TRUE);
}

/************************************************************************/
BOOL EngineInit(h, v, lpSourceFrame, lpDestFrame, lpFunction,
	FadeOutIncrement, iSourceTileMode, x, y, fDisplayOnSetting)
/************************************************************************/
int h, v; 		/* horizontal and vertical brush dimensions */
LPFRAME lpSourceFrame;
LPFRAME lpDestFrame;
LPROC lpFunction;
FIXED FadeOutIncrement;
int iSourceTileMode;
int x, y;
BOOL fDisplayOnSetting;
{
AstralUpdateWindow(lpImage->hWnd);

if (!lpDestFrame)
    return(FALSE);

fDoDDA = TRUE /*!Control.EMSdisable*/;

fDisplayOn = fDisplayOnSetting;
lpSrcFrame = lpSourceFrame;
lpDstFrame = lpDestFrame;
lpStrokeFunction = lpFunction;
if (Retouch.BrushShape == IDC_BRUSHCUSTOM)
	FadeOut = 0;
else
	FadeOut = FadeOutIncrement;
BrushSize = Retouch.BrushSize;
FadeBrushSize = TOFIXED(Retouch.BrushSize);
iTileMode = iSourceTileMode;

lpOldFrame = frame_set(lpDestFrame);
frame_mode(RELEASE_OLDEST);

if (lpDstFrame->Depth == 3)
    {
    lpMergeProc = MergeData24;
    lpMaskProc = mload24;
    lpProcessProc = ProcessData24;
    }
else if (lpDstFrame->Depth == 2)
    {
    lpMergeProc = MergeData16;
    lpMaskProc = mload16;
    lpProcessProc = ProcessData16;
    }
else
    {
    lpMergeProc = MergeData8;
    lpMaskProc = mload8;
    lpProcessProc = ProcessData8;
    }
    
/* setup pointers */
pb = Retouch.BrushMask;
BrushFull = h;
BrushHalf = BrushFull / 2;
bh = MAX_BRUSH_SIZE+1;
bv = MAX_BRUSH_SIZE+1;
bcount = bh * bv;
if (fDoDDA)
	bcount *= 3;
ps  = Alloc( (long)bcount * lpDstFrame->Depth );
pd  = Alloc( (long)bcount * lpDstFrame->Depth );
pm  = Alloc( (long)bcount);
if (!ps || !pd || !pm)
	{
    	if (ps)
		FreeUp((LPTR)ps);
    	if (pd)
		FreeUp((LPTR)pd);
    	if (pm)
		FreeUp((LPTR)pm);
	return(FALSE);
	}
if (Retouch.BrushStyle == IDC_BRUSHSCATTER)
	{
	pe = (LPINT)Alloc((long)(bh+1)*3L*2L);
	if (!pe)
		{
		FreeUp((LPTR)ps);
		FreeUp((LPTR)pd);
		FreeUp((LPTR)pm);
		return(FALSE);
		}
	}
if (fDoDDA)
	{
	pdsavecurr  = Alloc( (long)bcount * lpDstFrame->Depth );
	pdsavelast  = Alloc( (long)bcount * lpDstFrame->Depth );
	pm1 = Alloc((long)bcount);
	pm2 = Alloc((long)bcount);
	pmsave = Alloc((long)bcount);
	if (!pdsavecurr || !pdsavelast || !pm1 || !pm2 || !pmsave)
		{
		FreeUp((LPTR)ps);
		FreeUp((LPTR)pd);
		FreeUp((LPTR)pm);
    		if (Retouch.BrushStyle == IDC_BRUSHSCATTER)
			FreeUp((LPTR)pe);
		if (pdsavecurr)
			FreeUp((LPTR)pdsavecurr);
    		if (pdsavelast)
			FreeUp((LPTR)pdsavelast);
    		if (pm1)
			FreeUp((LPTR)pm1);
    		if (pm2)
			FreeUp((LPTR)pm2);
    		if (pmsave)
			FreeUp((LPTR)pmsave);
		return(FALSE);
		}
	}
if (!lpSrcFrame)
    {
    bload( 0, 0, bh, bcount/bv, ps, lpSrcFrame, NULL, 0, &Palette.ActiveRGB);	/* Load the SRC area with 
color or texture */
    }
UsingMask = (lpDstFrame->WriteMask != NULL) || (ColorMask.Mask && !ColorMask.Off);
if (lpDstFrame->WriteMask)
	onMask = !Mask.IsOutside;
else
	onMask = FALSE;
fFirstStroke = TRUE;
return(TRUE);
}

/************************************************************************/
void EngineDone()
/************************************************************************/
{
/* freeup buffers */
if (FadeOut)
    {
    if (fDisplayOn)
	DisplayBrush(0, 0, 0, OFF);
    Retouch.BrushSize = BrushSize;
    SetupMask();
    if (fDisplayOn)
	DisplayBrush(lpImage->hWnd, cursor_x, cursor_y, ON);
    }
FreeUp( ps );
FreeUp( pm );
FreeUp( pd );
if (Retouch.BrushStyle == IDC_BRUSHSCATTER)
	FreeUp( (LPTR)pe );
if (fDoDDA)
	{
	FreeUp( pdsavecurr );
	FreeUp( pdsavelast );
	FreeUp( pm1 );
	FreeUp( pm2 );
	FreeUp( pmsave );
	}
frame_mode(RELEASE_NEWEST);
if (lpSrcFrame)
    {
    frame_set(lpSrcFrame);
    frame_mode(RELEASE_NEWEST);
    }
frame_set(lpOldFrame);
}

/************************************************************************/
void FadeBrush(fSetupMask)
/************************************************************************/
BOOL fSetupMask;
{
if (FadeOut)
	{
	Retouch.BrushSize = FMUL(1, FadeBrushSize);
	Retouch.BrushSize = bound(Retouch.BrushSize, 0, MAX_BRUSH_SIZE);
	if (fSetupMask)
		{
    		SetupMask();
    		BrushFull = FullBrush;
		}
    	else
		{
		BrushFull = Retouch.BrushSize;
		if (BrushFull < 1) BrushFull = 1;
		if (BrushFull > MAX_BRUSH_SIZE) BrushFull = MAX_BRUSH_SIZE;
		if ( !(BrushFull & 1) )
			BrushFull++;
		}
	BrushHalf = BrushFull / 2;
	}
}

/************************************************************************/
void FadeOutBrush(fSetupMask)
/************************************************************************/
BOOL fSetupMask;
{
if (FadeOut)
	{
	FadeBrushSize -= FadeOut;
	if (FadeBrushSize > 0)
		FadeBrush(fSetupMask);
	}
}

/************************************************************************/
void FadeInBrush(fSetupMask)
/************************************************************************/
BOOL fSetupMask;
{
if (FadeOut)
    {
    FadeBrushSize += FadeOut;
    FadeBrush(fSetupMask);
    }
}

/************************************************************************/
void BltRectList()
/************************************************************************/
{
int i, y, iCount;
LPTR lpLine;
LPELEMENT lpElement;
RGBS blackRGB;
long black;

blackRGB.red = blackRGB.green = blackRGB.blue = 0;
black = frame_tocache(&blackRGB);
for (i = 0; i < NumElements(); ++i)
	{
	lpElement = GetElement(i);
	iCount = RectWidth(&lpElement->rStrip);
	for (y = lpElement->rStrip.top; y <= lpElement->rStrip.bottom; ++y)
		{
		lpLine = CachePtr(0, lpElement->rStrip.left, y, YES);
		if (lpLine)
			frame_setpixels(lpLine, iCount, black);
		}
	UpdateImage(&lpElement->rStrip, NO);
 	AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &lpElement->rStrip);
	}
}

/************************************************************************/
void EngineStroke( sx, sy, dx, dy, curs_x, curs_y )
/************************************************************************/
int sx, sy, dx, dy;
int curs_x, curs_y;
{
RECT iRect;
LPTR temp;
LPELEMENT lpLastElement;
int i, y, hb, deltax, deltay;
FIXED SaveFadeBrushSize;

cursor_x = curs_x;
cursor_y = curs_y;

if (fDoDDA)
	{
	DestroyElements();
	if (fFirstStroke)
		{
		sx_delta = sx - dx;
		sy_delta = sy - dy;
		start_dx = dx;
		start_dy = dy;
		fFirstStroke = FALSE;
		fCheckLastStroke = FALSE;
		}
	else if (dx == start_dx && dy == start_dy)
		return;
	end_dx = dx;
	end_dy = dy;
	deltax = abs(end_dx-start_dx);
	deltay = abs(end_dy-start_dy);
	fHorzStrips = deltax >= deltay;
	if (fHorzStrips)
		fRightBottom = start_dx > end_dx;
	else
		fRightBottom = start_dy > end_dy;
	hb = BrushFull - BrushHalf;
	rMax.top = start_dy - BrushHalf;
	rMax.bottom = start_dy + hb - 1;
	rMax.left = start_dx - BrushHalf;
	rMax.right = start_dx + hb - 1;

	if (fDisplayOn)
		DisplayBrush(0, 0, 0, OFF);

	SaveFadeBrushSize = FadeBrushSize;
	FadeBrushSize -= max(deltax,deltay)*FadeOut;
	FadeBrush(NO);

	hb = BrushFull - BrushHalf;
	rMax.top = min(rMax.top, end_dy - BrushHalf);
	rMax.bottom = max(rMax.bottom, end_dy + hb - 1);
	rMax.left = min(rMax.left, end_dx - BrushHalf);
	rMax.right = max(rMax.right, end_dx + hb - 1);
	if (fHorzStrips)
		ddaline(end_dx, end_dy, start_dx, start_dy, BuildHorzRectList);
	else
		ddaline(end_dx, end_dy, start_dx, start_dy, BuildVertRectList);
	FadeBrushSize = SaveFadeBrushSize;
	FadeBrush(NO);
	if ((lpElement1 = GetElement(0)))
		{
		rStrip1 = lpElement1->rStrip;
		Strip1Width = RectWidth(&rStrip1);
		lpElement2 = lpElement1->next;
		if (lpElement2)
			{
			rStrip2 = lpElement2->rStrip;
			Strip2Width = RectWidth(&rStrip2);
			}
		}
	else
		{
		if (fDisplayOn)
    			DisplayBrush(lpImage->hWnd, cursor_x, cursor_y, ON);
		return;
		}

	lpLastElement = GetElement(NumElements()-1);
    	bload( 	lpLastElement->rStrip.left,
		lpLastElement->rStrip.top,
		RectWidth(&lpLastElement->rStrip),
		RectHeight(&lpLastElement->rStrip),
		pdsavecurr, lpDstFrame, &rSave, 0, NULL);
        if (fCheckLastStroke &&
	    AstralIntersectRect(&iRect, &lpLastElement->rStrip, &rStripSave))
		(*lpMergeProc)(pdsavecurr, (LPRECT)&lpLastElement->rStrip,
			       RectWidth(&lpLastElement->rStrip), 
			       pdsavelast, (LPRECT)&rStripSave,
			       RectWidth(&rStripSave), (LPRECT)&iRect); 
	clr(pm1, bcount);
	clr(pm2, bcount);
	fDone = NO;
	ddaline(start_dx, start_dy, end_dx, end_dy, ProcessRectList);
	if (fDisplayOn)
    		DisplayBrush(lpImage->hWnd, cursor_x, cursor_y, ON);

	temp = pdsavelast;
	pdsavelast = pdsavecurr;
	pdsavecurr = temp;
	rStripSave = lpLastElement->rStrip;
	temp = pm1;
	pm1 = pmsave;
	pmsave = temp;
	fCheckLastStroke = TRUE;

	start_dx = end_dx;
	start_dy = end_dy;
	}
else
	{
	if (FadeBrushSize > 0)
    		DoStroke(dx, dy, sx, sy);
	}
}

/************************************************************************/
void BuildHorzRectList(x, y)
/************************************************************************/
int x, y;
{
RECT bRect;
ELEMENT element;
LPELEMENT lpElement;
int i, elements;

if (FadeBrushSize > 0)
	{
	bRect.top = y - BrushHalf;
	bRect.bottom = bRect.top + BrushFull - 1;
	bRect.left = x - BrushHalf;
	bRect.right = bRect.left + BrushFull - 1;
	elements = NumElements();
	for (i = 0; i < elements; ++i)
		{
		lpElement = GetElement(i);
		if (bRect.right < lpElement->rStrip.left ||
	    		bRect.left > lpElement->rStrip.right)
			continue;
		if (bRect.top < lpElement->rStrip.top)
			lpElement->rStrip.top = bRect.top;
		if (bRect.bottom > lpElement->rStrip.bottom)
			lpElement->rStrip.bottom = bRect.bottom;
		if (bRect.left <= lpElement->rStrip.left)
			bRect.right = lpElement->rStrip.left - 1;
		else
			bRect.left = lpElement->rStrip.right + 1;
		}
	if (RectWidth(&bRect) > 0)
		{
		if (elements)
			{
			if (fRightBottom)
				bRect.right = bRect.left + bh - 1;
			else
				bRect.left = bRect.right - bh + 1;
			}
		BoundRect(&bRect, rMax.left, rMax.top, rMax.right, rMax.bottom);
		if (RectWidth(&bRect) > 0)
			{
			element.rStrip = bRect;
			InsertElement(&element);
			}
		}
	}
FadeInBrush(NO);
}

/************************************************************************/
void BuildVertRectList(x, y)
/************************************************************************/
int x, y;
{
RECT bRect;
ELEMENT element;
LPELEMENT lpElement;
int i, elements;

if (FadeBrushSize > 0)
	{
	bRect.top = y - BrushHalf;
	bRect.bottom = bRect.top + BrushFull - 1;
	bRect.left = x - BrushHalf;
	bRect.right = bRect.left + BrushFull - 1;
	elements = NumElements();
	for (i = 0; i < elements; ++i)
		{
		lpElement = GetElement(i);
		if (bRect.bottom < lpElement->rStrip.top ||
	    		bRect.top > lpElement->rStrip.bottom)
			continue;
		if (bRect.left < lpElement->rStrip.left)
			lpElement->rStrip.left = bRect.left;
		if (bRect.right > lpElement->rStrip.right)
			lpElement->rStrip.right = bRect.right;
		if (bRect.top <= lpElement->rStrip.top)
			bRect.bottom = lpElement->rStrip.top - 1;
		else
			bRect.top = lpElement->rStrip.bottom + 1;
		}
	if (RectHeight(&bRect) > 0)
		{
		if (elements)
			{
			if (fRightBottom)
				bRect.bottom = bRect.top + bv - 1;
			else
				bRect.top = bRect.bottom - bv + 1;
			}
		BoundRect(&bRect, rMax.left, rMax.top, rMax.right, rMax.bottom);
		if (RectHeight(&bRect) > 0)
			{
			element.rStrip = bRect;
			InsertElement(&element);
			}
		}
	}
FadeInBrush(NO);
}

/************************************************************************/
void ProcessRectList(x, y)
/************************************************************************/
int x, y;
{
RECT bRect, iRect;
ELEMENT element;
BOOL fIntersect1, fIntersect2;
LPTR temp;

if (!lpElement1 || fDone)
	return;
if (FadeBrushSize > 0)
	{
	bRect.top = y - BrushHalf;
	bRect.bottom = bRect.top + BrushFull - 1;
	bRect.left = x - BrushHalf;
	bRect.right = bRect.left + BrushFull - 1;
	if ((fIntersect1 = AstralIntersectRect(&iRect, &rStrip1, &bRect)))
		MergeMask(pm1, &rStrip1, Strip1Width, pb, &bRect, BrushFull, &iRect);

	if (lpElement2)
		if ((fIntersect2 = AstralIntersectRect(&iRect, &rStrip2, &bRect)))
			MergeMask(pm2, &rStrip2, Strip2Width, pb, &bRect, BrushFull, &iRect);
	}
else
	{
	fDone = YES;
	fIntersect1 = fIntersect2 = FALSE;
	}

/* as soon as we get a stroke that does not intersect rectangle 1 */
/* or we are at the last stroke, blt rectangle 1.  If we are      */
/* working with a rectangle 2, check for the same things.         */
/* set rectangle 1 to rectangle 2 if we are done with rectangle 1 */
if (!fIntersect1 || (x == end_dx && y == end_dy))  /* done with this rectangle */
	{
	if (fCheckLastStroke)
		{
		if ((fCheckLastStroke = AstralIntersectRect(&iRect, &rStrip1, &rStripSave)))
			MergeMask(pm1, &rStrip1, Strip1Width, pmsave, &rStripSave, RectWidth(&rStripSave), &iRect);
		}
	if (fCheckLastStroke)
		BltRect(&rStrip1, pm1, &rStripSave, pdsavelast, &iRect);
	else
		BltRect(&rStrip1, pm1, NULL, NULL, NULL);
	if (lpElement2)
		{
		if (!fIntersect2 || (x == end_dx && y == end_dy))
			{
			if (fCheckLastStroke)
				{
				if ((fCheckLastStroke = AstralIntersectRect(&iRect, &rStrip2, &rStripSave)))
					MergeMask(pm2, &rStrip2, Strip2Width, pmsave, &rStripSave, RectWidth(&rStripSave), &iRect);
				}
			if (fCheckLastStroke)
				BltRect(&rStrip2, pm2, &rStripSave, pdsavelast, &iRect);
			else
				BltRect(&rStrip2, pm2, NULL, NULL, NULL);
			if (!lpElement2->next)
				fDone = YES;
			}
		lpElement1 = lpElement2;
		rStrip1 = rStrip2;
		Strip1Width = Strip2Width;
		lpElement2 = lpElement1->next;
		if (lpElement2)
			{
			rStrip2 = lpElement2->rStrip;
			Strip2Width = RectWidth(&rStrip2);
			}
		temp = pm1;
		pm1 = pm2;
		pm2 = temp;
		clr(pm2, bcount);
		}
	else
		fDone = YES;
	}
//if (fDone)
//	{
//	if (AstralIntersectRect(&iRect, &rStrip1, &bRect))
//		AndMask(pm1, &rStrip1, Strip1Width, pb, &bRect, BrushFull, //&iRect);
//	}
FadeOutBrush(YES);
}

/************************************************************************/
void BltRect(lpRect, pb, lpMergeRect, lpMergeData, lpSectRect)
/************************************************************************/
LPRECT lpRect;
LPTR pb;
LPRECT lpMergeRect;
LPTR	lpMergeData;
LPRECT lpSectRect;
{
int i, j;
WORD b;
WORD s, d;
RECT rDest, rSource, rOutput;
int dx, dy, bh, bv, bufsize, sx, sy;
LPTR tpm;
char num[10], line[512];

dx = lpRect->left;
dy = lpRect->top;
bh = RectWidth(lpRect);
bv = RectHeight(lpRect);
bufsize = bh * bv;

if (lpDstFrame)
    {
    if (!bload(dx, dy, bh, bv, pd, lpDstFrame, &rDest, 0, NULL))	/* Load the DEST area */
	return;

    if (lpMergeData)
	{
	(*lpMergeProc)(pd, lpRect, bh, lpMergeData, lpMergeRect, RectWidth(lpMergeRect), lpSectRect);
	}
    }
if ( lpSrcFrame )		/* If necessary, load the SRC area (clone, smear) */
	{
	sx = dx + sx_delta;
	sy = dy + sy_delta;
	if (!bload( sx, sy, bh, bv, ps, lpSrcFrame, &rSource, iTileMode, NULL ))
		return;
	}
else
	rSource = rDest;

if (lpStrokeFunction)
        (*lpStrokeFunction)(dx, dy, bh, bv, pd, ps, pb, start_dx, start_dy, end_dx, end_dy);

if (UsingMask)		/* If DMASK being used */
	{
	(*lpMaskProc)( dx, dy, bh, bv, pm, pd ); /* load the DMASK area */
	copyifset( pb, pm, bufsize ); /* Copy BRUSH to DMASK area if DMASK is set */
	if (Retouch.BrushStyle == IDC_BRUSHSCATTER)
		ScatterMask(pm, pm, pe, bh, bv);
	tpm = pm;
	}
else if (Retouch.BrushStyle == IDC_BRUSHSCATTER)
	{
	ScatterMask(pb, pm, pe, bh, bv);
	tpm = pm;
	}
else
	tpm = pb;

(*lpProcessProc)(pd, ps, tpm, bufsize);

if (AstralIntersectRect(&rOutput, &rSource, &rDest))
    {
    BltToFrame(dx, dy, bh, bv, pd, &rOutput); /* BLT the DEST area into the FRAME */
    }
}

/************************************************************************/
void DoStroke(int dx, int dy, int sx, int sy)
/************************************************************************/
{
int i;
LPTR tpm;
WORD b;
WORD s, d;
RECT rDest, rSource, rOutput;

sx -= BrushHalf;
sy -= BrushHalf;
dx -= BrushHalf;
dy -= BrushHalf;

if (lpDstFrame)
    if (!bload( dx, dy, BrushFull, BrushFull, pd, lpDstFrame, &rDest, 0, NULL))	/* Load the DEST area */
	return;
if ( lpSrcFrame )		/* If necessary, load the SRC area (clone, smear) */
	{
	if (!bload( sx, sy, BrushFull, BrushFull, ps, lpSrcFrame, &rSource, iTileMode, NULL ))
		return;
	}
else
	rSource = rDest;

if (UsingMask)		/* If DMASK being used */
	{
	(*lpMaskProc)( dx, dy, BrushFull, BrushFull, pm, pd ); /* load the DMASK area */
	copyifset( pb, pm, bcount ); /* Copy BRUSH to DMASK area if DMASK is set */
	tpm = pm;
	}
else	tpm = pb;

if (lpStrokeFunction)
        (*lpStrokeFunction)(dx, dy, bh, bv, pd, ps, tpm, start_dx, start_dy, end_dx, end_dy);

(*lpProcessProc)(pd, ps, tpm, bcount);

if (fDisplayOn)
    DisplayBrush(0, 0, 0, OFF);
if (AstralIntersectRect(&rOutput, &rSource, &rDest))
    {
    BltToFrame(dx, dy, BrushFull, BrushFull, pd, &rOutput); /* BLT the DEST area into the FRAME */
    }
FadeOutBrush(YES);
if (fDisplayOn)
    DisplayBrush(lpImage->hWnd, cursor_x, cursor_y, ON);
}

/************************************************************************/
void BltToDisplay( int x, int y, int dh, int dv, LPTR pdata, LPRECT lpRect)
/************************************************************************/
    {
    RECT StrokeRect;
    
    if (y >= lpImage->nlin || (y+dv) < 0)
        return;
    if (x >= lpImage->npix || (x+dh) < 0)
        return;
        
    StrokeRect.left = bound(x+lpRect->left, 0, lpImage->npix-1);
    StrokeRect.top = bound(y+lpRect->top, 0, lpImage->nlin-1);
    StrokeRect.right = bound(x+lpRect->right, 0, lpImage->npix-1);
    StrokeRect.bottom = bound(y+lpRect->bottom, 0, lpImage->nlin-1);
    
    //pdata += (StrokeRect.top-y) * dh * lpDstFrame->Depth; /* advance in number of lines */
  //  pdata += (StrokeRect.left-x) * lpDstFrame->Depth; /* advance in number of pixels */
    
    if (fDisplayOn)
	UpdateImage( &StrokeRect, NO );
    }
    
/************************************************************************/
void BltToFrame( int x, int y, int dh, int dv, LPTR pdata, LPRECT lpRect)
/************************************************************************/
    {
    int bwidth, width, sy, sx, ey, ex;
    LPTR lp, pdatasave;
    RECT StrokeRect;

//    caller should have done this bounds checking already for other reasons
//    if (y >= lpImage->nlin || (y+dv) < 0)
//        return;
//    if (x >= lpImage->npix || (x+dh) < 0)
//        return;
    
    StrokeRect.left = sx = bound(x+lpRect->left, 0, lpImage->npix-1);
    StrokeRect.top = sy = bound(y+lpRect->top, 0, lpImage->nlin-1);
    StrokeRect.right = ex = bound(x+lpRect->right, 0, lpImage->npix-1);
    StrokeRect.bottom = ey = bound(y+lpRect->bottom, 0, lpImage->nlin-1);
    
    bwidth = dh*lpDstFrame->Depth;
    pdata += (sy-y) * bwidth; /* advance in number of lines */
    pdata += (sx-x) * lpDstFrame->Depth; /* advance in number of pixels */
    pdatasave = pdata;
    width = (ex-sx+1)*lpDstFrame->Depth;
    for (; sy <= ey; ++sy)
        {
        if ((lp = CachePtr(0, sx, sy, YES)))
	    {
            copy(pdata, lp, width);
	    }
        pdata += bwidth;
        }
    AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &StrokeRect);
    if (fDisplayOn)
    	UpdateImage(&StrokeRect, NO);
    }

/************************************************************************/
int mload(int xs,int ys,int dx,int dy, LPTR pm, LPTR pd, LPFRAME lpFrame)
/************************************************************************/
{
if (lpFrame->WriteMask)
	onMask = !Mask.IsOutside;
else
	onMask = FALSE;
if (lpFrame->Depth == 3)
	mload24(xs, ys, dx, dy, pm, (LPRGB)pd);
else if (lpFrame->Depth == 2)
	mload16(xs, ys, dx, dy, pm, (LPWORD)pd);
else
	mload8(xs, ys, dx, dy, pm, pd);
}

/************************************************************************/
int mload8(int xs,int ys,int dx,int dy, LPTR pm, LPTR pd)
/************************************************************************/
{
int y, x, ye, xe;
LPTR tpm;
BYTE value, mask;
int i;
LPTR RMask, GMask, BMask;
BOOL Include, NotInclude;

ye = ys + dy - 1;
xe = xs + dx - 1;
for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
    MaskLoad(xs, y, dx, tpm, onMask);
if (ColorMask.Mask && !ColorMask.Off)
    {
    i = dx * dy;
    tpm = pm;
    RMask = ColorMask.RMask;
    GMask = ColorMask.GMask;
    BMask = ColorMask.BMask;
    Include = ColorMask.Include;
    NotInclude = !Include;
    while (--i >= 0)
	{
	value = *pd++;
        if (!(mask = RMask[value]))
	    *tpm++ &= NotInclude;  /* not in range */
    	else if (!(mask &= GMask[value]))
	    *tpm++ &= NotInclude;  /* not in range */
	else if ((mask &= BMask[value]))
	    *tpm++ &= Include;  /* in range */
	else
	    *tpm++ &= NotInclude;	/* not in range */
	}
    }
}

/************************************************************************/
int mload16(int xs,int ys,int dx,int dy, LPTR pm, LPWORD pd)
/************************************************************************/
{
int y, x, ye, xe;
LPTR tpm;
WORD wPixel;
BYTE mask, red, green, blue;
int i;
LPTR RMask, GMask, BMask;
BOOL Include, NotInclude;

ye = ys + dy - 1;
xe = xs + dx - 1;
for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
    MaskLoad(xs, y, dx, tpm, onMask);
if (ColorMask.Mask && !ColorMask.Off)
    {
    i = dx * dy;
    tpm = pm;
    RMask = ColorMask.RMask;
    GMask = ColorMask.GMask;
    BMask = ColorMask.BMask;
    Include = ColorMask.Include;
    NotInclude = !Include;
    while (--i >= 0)
	{
	wPixel = *pd++;
	red = MaxiR(wPixel);
        if (!(mask = RMask[red]))
	    {
	    *tpm++ &= NotInclude;  /* not in range */
	    continue;
	    }
	green = MaxiG(wPixel);
    	if (!(mask &= GMask[green]))
	    {
	    *tpm++ &= NotInclude;  /* not in range */
	    continue;
	    }
	blue = MaxiB(wPixel);
	if ((mask &= BMask[blue]))
	    *tpm++ &= Include;  /* in range */
	else
	    *tpm++ &= NotInclude;	/* not in range */
	}
    }
}

/************************************************************************/
int mload24(int xs,int ys,int dx,int dy, LPTR pm, LPRGB pd)
/************************************************************************/
{
int y, x, ye, xe;
LPTR tpm;
BYTE mask;
int i;
LPTR RMask, GMask, BMask;
BOOL Include, NotInclude;

ye = ys + dy - 1;
xe = xs + dx - 1;
for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
    MaskLoad(xs, y, dx, tpm, onMask);
if (ColorMask.Mask && !ColorMask.Off)
    {
    i = dx * dy;
    tpm = pm;
    RMask = ColorMask.RMask;
    GMask = ColorMask.GMask;
    BMask = ColorMask.BMask;
    Include = ColorMask.Include;
    NotInclude = !Include;
    while (--i >= 0)
	{
        if (!(mask = RMask[pd->red]))
	    *tpm++ &= NotInclude;  /* not in range */
    	else if (!(mask &= GMask[pd->green]))
	    *tpm++ &= NotInclude;  /* not in range */
	else if ((mask &= BMask[pd->blue]))
	    *tpm++ &= Include;  /* in range */
	else
	    *tpm++ &= NotInclude;	/* not in range */
	++pd;
	}
    }
}

/************************************************************************/
BOOL bload(int x,int y,int dx,int dy,LPTR pdata, LPFRAME lpFrame, LPRECT lpRect, int iTileMode, LPRGB lpRGB) 
/************************************************************************/
    { /* Load the DEST area */
    LPTR lp;
    int width, cwidth, awidth, iCount, sx, sdx, sy, nlines, Xsize, Ysize, ox, oy, xTile, yTile, depth2, depth, xpos, temp, pair_width, pair_height, ystart, yend, yinc, lines;
    long pixel;
    LPFRAME lpOldFrame;

    iCount = dx * dy;
    if (lpFrame)
        {
	Xsize = lpFrame->Xsize;
	Ysize = lpFrame->Ysize;
	depth = lpFrame->Depth;
        cwidth = width = dx * depth;
	if (iTileMode)
	    {
	    // set up the return rectangle, we can always fill it up
	    lpRect->top = lpRect->left = 0;
	    lpRect->bottom = dy - 1; lpRect->right = dx - 1;

	    // save original x and y on image
	    ox = x;
	    oy = y;

	    // set to proper frame
	    lpOldFrame = frame_set(lpFrame);

	    // calculate the width and height or horizontally and
	    // vertically flipped tiles.  The flipped tiles use don't
	    // use the last pixel or the last line so as to tile more
	    // naturally - so the theory goes...
	    // these width and heights include the even tile which is
	    // is not flipped and the odd tile which is flipped and
	    // is smaller by 1.
            pair_width = (2*Xsize)-1;
	    pair_height = (2*Ysize)-1;
	    depth2 = depth * 2;
	    lines = dy;
	    while (lines > 0)
		{
		if (iTileMode & 4)
			{
			// get position within two tile pair
			y = oy-((oy/pair_height)*pair_height);
			if (oy < 0)
				y += pair_height;
			// determine whether this is an even or odd file
			yTile = y/Ysize;
			}
		else 
			{
			if (oy < 0)
				y = Ysize - ((-oy)%Ysize);
			else
				y = oy%Ysize;
			}
		if (iTileMode & 4 && yTile & 1) // vertical flipping and
		    {				// odd tile
		    y -= Ysize;
		    nlines = min(lines, (Ysize-y-1));
		    ystart = Ysize-y-2;
		    yend = ystart - nlines;
		    yinc = -1;
		    }
		else	// even tile - same for normal and vertical flip
		    {
         	    nlines = min(lines, (Ysize-y));
		    ystart = y;
		    yend = y + nlines;
		    yinc = 1;
		    }
		lines -= nlines;	// decrement lines still to process
		oy += nlines;	// increment position on image
		for (y = ystart; y != yend; y += yinc)
		    {
            	    xpos = ox;	// get position on image
	            iCount = cwidth; // get amount of data to transfer
		    while (iCount > 0)
		    	{
			if (iTileMode & 2)
				{
				// get position within two tile pair
				x = xpos-((xpos/pair_width)*pair_width);
				if (xpos < 0)
					x += pair_width;
				// determine whether this is an even or odd file
	    			xTile = x/Xsize;
				}
			else
				{
				if (xpos < 0)
					x = Xsize - ((-xpos)%Xsize);
				else
					x = xpos%Xsize;
				}
		    	if (iTileMode & 2 && xTile & 1) // horz flip and
			    {				// odd tile
			    x -= Xsize;
			    // get amount of data left in this tile
		    	    awidth = min(iCount, (Xsize-x-1)*depth);
			    // reduce total amount to transfer
		            iCount -= awidth;
            	
			    // get pointer to cache data
    	    		    lp = CachePtr(0, Xsize-x-2, y, NO);
            	    	    if (lp) // got good pointer - should never fail!!
			        {
				// get number of pixels to move
	    			awidth /= depth;
				// increment position on image
				xpos += awidth;
			    	while (--awidth >= 0)
				    {
				    // move pixel by pixel going backwards
				    // through source
				    temp = depth;
				    while (--temp >= 0)
				    	*pdata++ = *lp++;
				    lp -= depth2;
				    }
			    	}
			    else
			    	{
				// no data - just advance pointers and xpos
		            	pdata += awidth;
			    	xpos += (awidth/depth);
			    	}
			    }
		    	else 	// even tile - same for normal and horz flip
			    {
			    // get amount of data left in this tile
   			    awidth = min(iCount, (Xsize-x)*depth);
			    // get pointer to cache data
            	    	    lp = CachePtr(0, x, y, NO);
            	    	    if (lp) // good pointer - this should never fail
                                copy(lp, pdata, awidth);
			    // adjust counts and pointers
		            iCount -= awidth;
		            pdata += awidth;
			    xpos += (awidth/depth);
			    }
			// always starts on a new tile
		        x = 0;
		        }
		    }
		// always starts on a new tile
		y = 0;
		}
	    frame_set(lpOldFrame);
	    }
	else
	    {
	    if (y >= Ysize) /* below image */
 	    	return(FALSE);
	    if ((y + dy) <= 0)	/* above image */
 	    	return(FALSE);
	    if (x >= Xsize) /* right of image */
 	    	return(FALSE);
	    if ((x + dx) <= 0)	/* left of image */
 	    	return(FALSE);
	    sx = x;
	    sy = y;
	    if (y < 0)
	    	{
	    	pdata += (-y)*width;
	    	lpRect->top = 0;
 	    	}
	    else
	    	lpRect->top = y;
	    y = y + dy - 1;
	    if (y >= Ysize)
	    	lpRect->bottom = Ysize-1;
	    else
	    	lpRect->bottom = y;

	    if (x < 0)
	    	{
	    	sdx = (-x)*depth;
	    	pdata += sdx;
	    	cwidth -= sdx;
	    	lpRect->left = 0;
	    	}
	    else
 	    	lpRect->left = x;
	    x = x + dx - 1;
	    if (x >= Xsize)
	    	{
	    	lpRect->right = Xsize - 1;
	    	cwidth -= (x-lpRect->right)*depth;
	    	}
	    else
  	    	lpRect->right = x;

	    lpOldFrame = frame_set(lpFrame);
	    lines = lpRect->bottom - lpRect->top + 1;
            for (y = lpRect->top; y <= lpRect->bottom; ++y)
            	{
            	lp = CachePtr(0, lpRect->left, y, NO);
            	if (lp)
                    copy(lp, pdata, cwidth);
            	pdata += width;
            	}
	    frame_set(lpOldFrame);
	    OffsetRect(lpRect, -sx, -sy);
       	    }
	}
    else
        {
	pixel = frame_tocache(lpRGB);
        frame_setpixels(pdata, iCount, pixel);
        }
    return(TRUE);
    }

/************************************************************************/
void copyifset( lptr1, lptr2, count )
/************************************************************************/
LPTR lptr1, lptr2;
int count;
{
while ( --count >= 0 )
	{
	if ( *lptr2 )
		*lptr2++ = *lptr1++;
	else	{
		lptr2++;
		lptr1++;
		}
	}
}

#ifdef C_CODE
/************************************************************************/
void MergeMask(lpDst, lpDstRect, DstWidth, lpSrc, lpSrcRect, SrcWidth, lpSectRect)
/************************************************************************/
LPTR lpDst;
LPRECT lpDstRect;
int DstWidth;
LPTR lpSrc;
LPRECT lpSrcRect;
int SrcWidth;
LPRECT lpSectRect;
{
int dx, dy, iWidth, iHeight, iCount;
LPTR lpSrcBuf, lpDstBuf, lpSrcTmp, lpDstTmp;

dy = lpSectRect->top - lpDstRect->top;
dx = lpSectRect->left - lpDstRect->left;
lpDstBuf = lpDst + (DstWidth * dy) + dx;
dy = lpSectRect->top - lpSrcRect->top;
dx = lpSectRect->left - lpSrcRect->left;
lpSrcBuf = lpSrc + (SrcWidth * dy) + dx;
iWidth = RectWidth(lpSectRect);
iHeight = RectHeight(lpSectRect);
while (--iHeight >= 0)
	{
	lpSrcTmp = lpSrcBuf;
	lpDstTmp = lpDstBuf;
	iCount = iWidth;
	while (--iCount >= 0)
		{
		if (*lpSrcTmp > *lpDstTmp)
			*lpDstTmp = *lpSrcTmp;
		++lpDstTmp;
		++lpSrcTmp;
		}
	lpSrcBuf += SrcWidth;
	lpDstBuf += DstWidth;
	}
}
#endif

#ifdef C_CODE
/************************************************************************/
void MergeData8(lpDst, lpDstRect, DstWidth, lpSrc, lpSrcRect, SrcWidth, lpSectRect)
/************************************************************************/
LPTR lpDst;
LPRECT lpDstRect;
int DstWidth;
LPTR lpSrc;
LPRECT lpSrcRect;
int SrcWidth;
LPRECT lpSectRect;
{
int dx, dy, iWidth, iHeight, iCount;
LPTR lpSrcBuf, lpDstBuf, lpSrcTmp, lpDstTmp;

dy = lpSectRect->top - lpDstRect->top;
dx = lpSectRect->left - lpDstRect->left;
lpDstBuf = lpDst + (DstWidth * dy) + dx;
dy = lpSectRect->top - lpSrcRect->top;
dx = lpSectRect->left - lpSrcRect->left;
lpSrcBuf = lpSrc + (SrcWidth * dy) + dx;
iWidth = RectWidth(lpSectRect);
iHeight = RectHeight(lpSectRect);
while (--iHeight >= 0)
	{
	lpSrcTmp = lpSrcBuf;
	lpDstTmp = lpDstBuf;
	iCount = iWidth;
	while (--iCount >= 0)
		*lpDstTmp++ = *lpSrcTmp++;
	lpSrcBuf += SrcWidth;
	lpDstBuf += DstWidth;
	}
}
#endif

/************************************************************************/
void MergeData16(lpDst, lpDstRect, DstWidth, lpSrc, lpSrcRect, SrcWidth, lpSectRect)
/************************************************************************/
LPWORD lpDst;
LPRECT lpDstRect;
int DstWidth;
LPWORD lpSrc;
LPRECT lpSrcRect;
int SrcWidth;
LPRECT lpSectRect;
{
int dx, dy, iWidth, iHeight, iCount;
LPWORD lpSrcBuf, lpDstBuf, lpSrcTmp, lpDstTmp;

dy = lpSectRect->top - lpDstRect->top;
dx = lpSectRect->left - lpDstRect->left;
lpDstBuf = lpDst + (DstWidth * dy) + dx;
dy = lpSectRect->top - lpSrcRect->top;
dx = lpSectRect->left - lpSrcRect->left;
lpSrcBuf = lpSrc + (SrcWidth * dy) + dx;
iWidth = RectWidth(lpSectRect);
iHeight = RectHeight(lpSectRect);
while (--iHeight >= 0)
	{
	lpSrcTmp = lpSrcBuf;
	lpDstTmp = lpDstBuf;
	iCount = iWidth;
	while (--iCount >= 0)
		*lpDstTmp++ = *lpSrcTmp++;
	lpSrcBuf += SrcWidth;
	lpDstBuf += DstWidth;
	}
}

#ifdef C_CODE
/************************************************************************/
void MergeData24(lpDst, lpDstRect, DstWidth, lpSrc, lpSrcRect, SrcWidth, lpSectRect)
/************************************************************************/
LPRGB lpDst;
LPRECT lpDstRect;
int DstWidth;
LPRGB lpSrc;
LPRECT lpSrcRect;
int SrcWidth;
LPRECT lpSectRect;
{
int dx, dy, iWidth, iHeight, iCount;
LPRGB lpSrcBuf, lpDstBuf, lpSrcTmp, lpDstTmp;

dy = lpSectRect->top - lpDstRect->top;
dx = lpSectRect->left - lpDstRect->left;
lpDstBuf = lpDst + (DstWidth * dy) + dx;
dy = lpSectRect->top - lpSrcRect->top;
dx = lpSectRect->left - lpSrcRect->left;
lpSrcBuf = lpSrc + (SrcWidth * dy) + dx;
iWidth = RectWidth(lpSectRect);
iHeight = RectHeight(lpSectRect);
while (--iHeight >= 0)
	{
	lpSrcTmp = lpSrcBuf;
	lpDstTmp = lpDstBuf;
	iCount = iWidth;
	while (--iCount >= 0)
		*lpDstTmp++ = *lpSrcTmp++;
	lpSrcBuf += SrcWidth;
	lpDstBuf += DstWidth;
	}
}
#endif

/************************************************************************/
void ProcessData(lpDst, lpSrc, lpMsk, iCount, depth)
/************************************************************************/
LPTR lpDst;
LPTR lpSrc;
LPTR lpMsk;
int iCount;
int depth;
{
if (depth == 3)
	ProcessData24((LPRGB)lpDst, (LPRGB)lpSrc, lpMsk, iCount);
else if (depth == 2)
	ProcessData16((LPWORD)lpDst, (LPWORD)lpSrc, lpMsk, iCount);
else
	ProcessData8(lpDst, lpSrc, lpMsk, iCount);
}

#ifdef C_CODE
{
WORD b;
WORD s, d;

while (--iCount >= 0)
	{
	b = *lpMsk;
	if (b)
	    {
	    //s = *tps;
	    if (b > 127)
		++b;
	    //s *= b;
	    //b = 256-b;
	    //d = *tpd;
	    //d *= b;
	    *lpDst = ((*lpDst*(256-b)) + (*lpSrc*b)) >> 8;
	    }
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
}
#endif

/************************************************************************/
void ProcessData16(lpDst, lpSrc, lpMsk, iCount)
/************************************************************************/
LPWORD lpDst;
LPWORD lpSrc;
LPTR lpMsk;
int iCount;
{
WORD b;
WORD sr, sg, sb, dr, dg, db, wPixel;
RGBS rgb;

while (--iCount >= 0)
	{
	b = *lpMsk;
	if (b)
	    {
	    wPixel = *lpSrc;
	    sr = MaxiR(wPixel);
	    sg = MaxiG(wPixel);
	    sb = MaxiB(wPixel);
	    if (b > 127)
		++b;
	    sr *= b;
	    sg *= b;
	    sb *= b;
	    b = 256-b;
	    wPixel = *lpDst;
	    dr = MaxiR(wPixel);
	    dg = MaxiG(wPixel);
	    db = MaxiB(wPixel);
	    dr *= b;
	    dg *= b;
	    db *= b;
	    
	    rgb.red = (dr + sr) >> 8;
	    rgb.green = (dg + sg) >> 8;
	    rgb.blue = (db + sb) >> 8;
	    *lpDst = RGBtoMiniRGB(&rgb);
	    }
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
}

#ifdef C_CODE
/************************************************************************/
void ProcessData24(lpDst, lpSrc, lpMsk, iCount)
/************************************************************************/
LPRGB lpDst;
LPRGB lpSrc;
LPTR lpMsk;
int iCount;
{
WORD b;
WORD sr, sg, sb, dr, dg, db;

while (--iCount >= 0)
	{
	b = *lpMsk;
	if (b)
	    {
	    sr = lpSrc->red;
	    sg = lpSrc->green;
	    sb = lpSrc->blue;
	    if (b > 127)
		++b;
	    sr *= b;
	    sg *= b;
	    sb *= b;
	    b = 256-b;
	    dr = lpDst->red;
	    dg = lpDst->green;
	    db = lpDst->blue;
	    dr *= b;
	    dg *= b;
	    db *= b;
	    lpDst->red = (dr + sr) >> 8;
	    lpDst->green = (dg + sg) >> 8;
	    lpDst->blue = (db + sb) >> 8;
	    }
	lpDst++;
	lpSrc++;
	lpMsk++;
	}
}
#endif

/************************************************************************/
void MergeData(lpDst, lpDstRect, DstWidth, lpSrc, lpSrcRect, SrcWidth, lpSectRect)
/************************************************************************/
LPTR lpDst;
LPRECT lpDstRect;
int DstWidth;
LPTR lpSrc;
LPRECT lpSrcRect;
int SrcWidth;
LPRECT lpSectRect;
{
(*lpMergeProc)(lpDst, lpDstRect, DstWidth, lpSrc, lpSrcRect, SrcWidth, lpSectRect);
}

#ifdef C_CODE
/************************************************************************/
void ScatterMask(lpSrc, lpDst, lpErrors, dx, dy)
/************************************************************************/
LPTR lpSrc;
LPTR lpDst;
LPINT lpErrors;
int dx, dy;
{
int error, iCount;
LPINT lpError;
static ULONG lSeed = 0xDEADBABA;

clr( (LPTR)lpErrors, (dx+1)*2 );
while (--dy >= 0)
	{
	lpError = lpErrors;
	iCount = dx;
	while (--iCount >= 0)
		{
		error = 101;
		if ((error += *lpError) > 127 )
			{
			*lpDst++ = *lpSrc++;
			error -= 255;
			}
		else
			{
			*lpDst++ = 0;
			++lpSrc;
			}
		if ( lSeed & BIT18 )
			{
			lSeed += lSeed;
			lSeed ^= BITMASK;
			*lpError++ = 0;
			*lpError  += error;
			}
		else	{
			lSeed += lSeed;
			*lpError++ = error;
			}
		}
	}
}
#endif

/************************************************************************/
static BOOL InsertElement(lpElement)
/************************************************************************/
LPELEMENT lpElement;
{
LPELEMENT new;

if ( !(new = (LPELEMENT)Alloc((long)sizeof(ELEMENT))) )
	return(FALSE);
*new = *lpElement;
new->next = ElementHead;
ElementHead = new;
return(TRUE);
}

/************************************************************************/
static BOOL AddElement(lpElement)
/************************************************************************/
LPELEMENT lpElement;
{
LPELEMENT node, new;

if ( !(new = (LPELEMENT)Alloc((long)sizeof(ELEMENT))) )
	return(FALSE);
*new = *lpElement;
new->next = NULL;
if (ElementHead)
	{
	node = ElementHead;
	while (node->next)
		node = node->next;
	node->next = new;
	}
else
	ElementHead = new;
return(TRUE);
}

/************************************************************************/
static int NumElements()
/************************************************************************/
{
LPELEMENT node;
int num_elements;

num_elements = 0;
node = ElementHead;
while (node)
	{
	++num_elements;
	node = node->next;
	}
return(num_elements);
}

/************************************************************************/
static LPELEMENT GetElement(index)
/************************************************************************/
int index;
{
LPELEMENT node, temp;
int doc_index;

doc_index = 0;
node = ElementHead;
while (node)
	{
	if (doc_index == index)
		{
		return(node);
		}
	++doc_index;
	node = node->next;
	}
return(NULL);
}

/************************************************************************/
static BOOL RemElement(index)
/************************************************************************/
int index;
{
LPELEMENT node, prev;
int doc_index;

doc_index = 0;
prev = NULL;
node = ElementHead;
while (node)
	{
	if (doc_index == index)
		{
		if (prev)
			prev->next = node->next;
		else
			ElementHead = node->next;
		FreeUp((LPTR)node);
		return(TRUE);
		}
	++doc_index;
	prev = node;
	node = node->next;
	}
return(FALSE);
}

/************************************************************************/
static void DestroyElements()
/************************************************************************/
{
while (NumElements())
	RemElement(0);
}

