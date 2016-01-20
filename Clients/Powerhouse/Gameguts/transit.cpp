#include <windows.h> 
#include <stdlib.h> // needed for srand
#include <memory.h> // needed for _fmemset
#include <mmsystem.h>
#include "proto.h"
#include "transit.h"
#include "wing.h"
#include "randseq.h"

#define CLOCK			1
#define TICKS(t)		((t) / CLOCK)

#define Effect(t)		((t) & 0x00ff)
#define Speed(t)		((t) & 0xff00)

#define SPEED(vf,f,m,s,vs)	(tp->nTicksOverride >= 0 ? tp->nTicksOverride : \
							Speed(tp->Transition) == TRANS_VERYSLOW ? vs : \
							Speed(tp->Transition) == TRANS_SLOW ? s : \
							Speed(tp->Transition) == TRANS_MEDIUM	? m : \
							Speed(tp->Transition) == TRANS_VERYFAST ? vf : f)
#define STEPSIZE(size) (tp->nStepSizeOverride ? tp->nStepSizeOverride : (size)) 

#define bitTEST(p, b)	((p)[(b) >> 3] & (1 << ((b) & 7)))
#define bitSET(p, b)	((p)[(b) >> 3] |= (1 << ((b) & 7)))

static int rainoff[] = { 6, -7, 7, 2, -1, 5, -6, -3, 0, -10,
						1, -5, 4, -8, 3, 8, -4, -9, -2, 9};

/***********************************************************************/
static void TransStretchDIBlt( LPTRANS tp, int xDst, int yDst, int xDstExt, int yDstExt, int xSrc, int ySrc, int xSrcExt, int ySrcExt, DWORD rop = SRCCOPY )
/***********************************************************************/
{
	int xDelta, yDelta;

	if ((xDelta = tp->xDst - xDst) > 0)
		xDst += xDelta, xSrc += xDelta, xDstExt -= xDelta, xSrcExt -= xDelta;

	if ((xDelta = (xDst + xDstExt) - (tp->xDst + tp->xExt)) > 0)
		xDstExt -= xDelta, xSrcExt -= xDelta;

	if ((yDelta = tp->yDst - yDst) > 0)
		yDst += yDelta, ySrc += yDelta, yDstExt -= yDelta, ySrcExt -= yDelta;

	if ((yDelta = (yDst + yDstExt) - (tp->yDst + tp->yExt)) > 0)
		yDstExt -= yDelta, ySrcExt -= yDelta;

	if (xDstExt <= 0 || yDstExt <= 0)
		return;

	if ( tp->dcSrc && rop == SRCCOPY)
		WinGStretchBlt( tp->dcDst, xDst, yDst, xDstExt, yDstExt, tp->dcSrc, xSrc, ySrc, xSrcExt, ySrcExt );
	else
		tp->lpSrcDib->DCBlt( tp->dcDst, xDst, yDst, xDstExt, yDstExt, xSrc, ySrc, xSrcExt, ySrcExt, rop );
}

/***********************************************************************/
static void TransDIBltPixel( LPTRANS tp, int xDst, int yDst, int xSrc, int ySrc, DWORD rop = SRCCOPY )
/***********************************************************************/
{
	int maxx = tp->xDst + tp->xExt - 1;
	int maxy = tp->yDst + tp->yExt - 1;
	if (xDst < 0 || xDst > maxx)
		return;
	if (yDst < 0 || yDst > maxy)
		return;
	if (xSrc < 0 || xSrc > maxx)
		return;
	if (ySrc < 0 || ySrc > maxy)
		return;

	if ( tp->dcSrc && rop == SRCCOPY)
		WinGBitBlt( tp->dcDst, xDst, yDst, 1, 1, tp->dcSrc, xSrc, ySrc);
	else
		tp->lpSrcDib->DCBlt( tp->dcDst, xDst, yDst, 1, 1, xSrc, ySrc, 1, 1, rop );
}

/***********************************************************************/
static void TransDIBlt( LPTRANS tp, int xDst, int yDst, int xExt, int yExt, int xSrc, int ySrc, DWORD rop = SRCCOPY )
/***********************************************************************/
{
	TransStretchDIBlt( tp, xDst, yDst, xExt, yExt, xSrc, ySrc, xExt, yExt, rop );
}

/***********************************************************************/
static void TransDCBlt( LPTRANS tp, int xDst, int yDst, int xExt, int yExt, int xSrc, int ySrc, DWORD rop = SRCCOPY )
/***********************************************************************/
{
	int xDelta, yDelta;

	if ((xDelta = tp->xDst - xDst) > 0)
		xDst += xDelta, xSrc += xDelta, xExt -= xDelta;

	if ((xDelta = (xDst + xExt) - (tp->xDst + tp->xExt)) > 0)
		xExt -= xDelta;

	if ((yDelta = tp->yDst - yDst) > 0)
		yDst += yDelta, ySrc += yDelta, yExt -= yDelta;

	if ((yDelta = (yDst + yExt) - (tp->yDst + tp->yExt)) > 0)
		yExt -= yDelta;

	if (xExt <= 0 || yExt <= 0)
		return;
		
	BitBlt(tp->dcDst, xDst, yDst, xExt, yExt, tp->dcDst, xSrc, ySrc, rop);
}

/***********************************************************************/
static BOOL setupBlindsHorizontal( LPTRANS tp )
/***********************************************************************/
{
	tp->xBox	= tp->yExt;
	tp->xBox	= ((tp->xBox + 7) / 8) & 0xfffe;
	tp->vStep = tp->xBox / STEPSIZE(10);
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(10, 25, 50, 85, 120);

	if (tp->vStep == 1) tp->xBox == 10;

	return(TRUE);
}

/***********************************************************************/
static BOOL stepBlindsHorizontal( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int Off = tp->nIteration * tp->vStep;

	if (Off >= tp->xBox)
	{
		bDone = TRUE;
	}
	else
	{
		for (; Off < tp->yExt; Off += tp->xBox)
		{
			TransDIBlt(tp, tp->xDst, tp->yDst + Off, tp->xExt, tp->vStep, tp->xSrc, tp->ySrc + Off);
		}
	}

	return(bDone);
}

/***********************************************************************/
static BOOL setupBlindsVertical( LPTRANS tp )
/***********************************************************************/
{
	tp->xBox	= tp->xExt;
	tp->xBox	= ((tp->xBox + 7) / 8) & 0xfffe;
	tp->vStep = tp->xBox / STEPSIZE(10);
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(10, 25, 50, 85, 120);

	if (tp->vStep == 1) tp->xBox == 10;

	return(TRUE);
}

/***********************************************************************/
static BOOL stepBlindsVertical( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int Off = tp->nIteration * tp->vStep;

	if (Off >= tp->xBox)
	{
		bDone = TRUE;
	}
	else
	{
		for (; Off < tp->xExt; Off += tp->xBox)
		{
			TransDIBlt(tp, tp->xDst + Off, tp->yDst, tp->vStep, tp->yExt, tp->xSrc + Off, tp->ySrc);
		}
	}

	return(bDone);
}

/***********************************************************************/
static BOOL setupBoxIn( LPTRANS tp )
/***********************************************************************/
{
	RECT rOuter;

	int factor = SPEED(40, 45, 55, 70, 90);

	tp->hStep = STEPSIZE(tp->xExt / factor);
	tp->vStep = STEPSIZE(tp->yExt / factor);
	tp->hStep = max(tp->hStep, 1);
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(1, 5, 10, 20, 40);

	SetRect(&rOuter, 0, 0, tp->xExt, tp->yExt);
	tp->dwTotalIterations = 0;
	while ((RectWidth(&rOuter) > 0) && (RectHeight(&rOuter) > 0))
	{
		InflateRect(&rOuter, -tp->hStep, -tp->vStep);
		++tp->dwTotalIterations;
	}

	return(TRUE);
}

/***********************************************************************/
static BOOL stepBoxIn( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	RECT rOuter;
	RECT rInner;

	SetRect(&rOuter, tp->nIteration * tp->hStep, tp->nIteration * tp->vStep,
			tp->xExt - (tp->nIteration * tp->hStep),
			tp->yExt - (tp->nIteration * tp->vStep) );
	rInner = rOuter;
	InflateRect(&rInner, -tp->hStep, -tp->vStep);

	if ((RectWidth(&rOuter) > 0) && (RectHeight(&rOuter) > 0))
	{
		TransDIBlt(tp,
		tp->xDst + rOuter.left,
		tp->yDst + rOuter.top,
		RectWidth(&rOuter),
		tp->vStep,
		tp->xSrc + rOuter.left,
		tp->ySrc + rOuter.top);

		TransDIBlt(tp,
		tp->xDst + rOuter.left,
		tp->yDst + rInner.bottom,
		RectWidth(&rOuter),
		tp->vStep,
		tp->xSrc + rOuter.left,
		tp->ySrc + rInner.bottom);

		TransDIBlt(tp,
		tp->xDst + rOuter.left,
		tp->yDst + rOuter.top,
		tp->hStep,
		RectHeight(&rOuter),
		tp->xSrc + rOuter.left,
		tp->ySrc + rOuter.top);

		TransDIBlt(tp,
		tp->xDst + rInner.right,
		tp->yDst + rOuter.top,
		tp->hStep,
		RectHeight(&rOuter),
		tp->xSrc + rInner.right,
		tp->ySrc + rOuter.top);
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupBoxOut( LPTRANS tp )
/***********************************************************************/
{
	int factor = SPEED(40, 45, 55, 70, 90);
	RECT rOuter;

	tp->hStep = STEPSIZE(tp->xExt / factor);
	tp->vStep = STEPSIZE(tp->yExt / factor);
	tp->hStep = max(tp->hStep, 1);
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(1, 5, 10, 20, 40);

	SetRect(&rOuter, 0, 0, tp->xExt, tp->yExt);

	tp->dwTotalIterations = 0;
	while ((RectWidth(&rOuter) > 0) && (RectHeight(&rOuter) > 0))
	{
		InflateRect(&rOuter, -tp->hStep, -tp->vStep);
		++tp->dwTotalIterations;
	}

	tp->xMin = rOuter.left;
	tp->xMax = rOuter.right;
	tp->yMin = rOuter.top;
	tp->yMax = rOuter.bottom;

	return(TRUE);
}

/***********************************************************************/
static BOOL stepBoxOut( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	RECT rOuter;
	RECT rInner;

	SetRect(&rInner, tp->xMin, tp->yMin, tp->xMax, tp->yMax);
	rOuter = rInner;
	InflateRect(&rOuter, tp->hStep, tp->vStep);

	if ((rInner.left > 0) || (rInner.top > 0))
	{
		TransDIBlt(tp,
		tp->xDst + rOuter.left,
		tp->yDst + rOuter.top,
		RectWidth(&rOuter),
		tp->vStep,
		tp->xSrc + rOuter.left,
		tp->ySrc + rOuter.top);

		TransDIBlt(tp,
		tp->xDst + rOuter.left,
		tp->yDst + rInner.bottom,
		RectWidth(&rOuter),
		tp->vStep,
		tp->xSrc + rOuter.left,
		tp->ySrc + rInner.bottom);

		TransDIBlt(tp,
		tp->xDst + rOuter.left,
		tp->yDst + rOuter.top,
		tp->hStep,
		RectHeight(&rOuter),
		tp->xSrc + rOuter.left,
		tp->ySrc + rOuter.top);

		TransDIBlt(tp,
		tp->xDst + rInner.right,
		tp->yDst + rOuter.top,
		tp->hStep,
		RectHeight(&rOuter),
		tp->xSrc + rInner.right,
		tp->ySrc + rOuter.top);
	}
	else
		bDone = TRUE;

	tp->xMin = rOuter.left;
	tp->xMax = rOuter.right;
	tp->yMin = rOuter.top;
	tp->yMax = rOuter.bottom;

	return(bDone);
}

/***********************************************************************/
static BOOL setupCheckerboardDown( LPTRANS tp )
/***********************************************************************/
{
	tp->yBox	= (tp->yExt / 4) & 0xfffe;
	tp->yBox	= max(tp->yBox, 16);
	tp->xBox	= tp->yBox / 2;
	tp->vStep = STEPSIZE(tp->yBox / SPEED(8,10,12,14,16));
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(1,12,24,48,96);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepCheckerboardDown( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int xOff;
	int yOff	= tp->nIteration * tp->vStep;

	if (yOff < tp->yBox)
	{
		for (; (yOff - (tp->yBox / 2)) < tp->yExt; yOff += tp->yBox)
		{
			for (xOff = 0; xOff < tp->xExt; xOff += tp->xBox)
			{
				TransDIBlt(tp,
				tp->xDst + xOff,
				tp->yDst + yOff,
				tp->xBox,
				tp->vStep,
				tp->xSrc + xOff,
				tp->ySrc + yOff);

				xOff += tp->xBox;

				TransDIBlt(tp,
				tp->xDst + xOff,
				tp->yDst + yOff - (tp->yBox / 2),
				tp->xBox,
				tp->vStep,
				tp->xSrc + xOff,
				tp->ySrc + yOff - (tp->yBox / 2));
			}
		}
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupCheckerboardLeft( LPTRANS tp )
/***********************************************************************/
{
	tp->xBox	= (tp->xExt / 4) & 0xfffe;
	tp->xBox	= max(tp->xBox, 16);
	tp->yBox	= tp->xBox / 2;
	tp->hStep = STEPSIZE(tp->xBox / SPEED(8,10,12,14,16));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,12,24,48,96);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepCheckerboardLeft( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int xOff	= tp->xBox - (tp->nIteration * tp->hStep);
	int yOff;

	if (xOff >= 0)
	{
		for (xOff += tp->xExt; xOff >= 0; xOff -= tp->xBox)
		{
			for (yOff = 0; yOff < tp->yExt; yOff += tp->yBox)
			{
				TransDIBlt(tp,
				tp->xDst + xOff,
				tp->yDst + yOff,
				tp->hStep,
				tp->yBox,
				tp->xSrc + xOff,
				tp->ySrc + yOff);

				yOff += tp->yBox;

				TransDIBlt(tp,
				tp->xDst + xOff - (tp->xBox / 2),
				tp->yDst + yOff,
				tp->hStep,
				tp->yBox,
				tp->xSrc + xOff - (tp->xBox / 2),
				tp->ySrc + yOff);
			}
		}
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupCheckerboardRight( LPTRANS tp )
/***********************************************************************/
{
	tp->xBox	= (tp->xExt / 4) & 0xfffe;
	tp->xBox	= max(tp->xBox, 16);
	tp->yBox	= tp->xBox / 2;
	tp->hStep = STEPSIZE(tp->xBox / SPEED(8,10,12,14,16));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,12,24,48,96);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepCheckerboardRight( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int xOff	= tp->nIteration * tp->hStep;
	int yOff;

	if (xOff < tp->xBox)
	{
		for (; (xOff - (tp->xBox / 2)) < tp->xExt; xOff += tp->xBox)
		{
			for (yOff = 0; yOff < tp->yExt; yOff += tp->yBox)
			{
				TransDIBlt(tp,
				tp->xDst + xOff,
				tp->yDst + yOff,
				tp->hStep,
				tp->yBox,
				tp->xSrc + xOff,
				tp->ySrc + yOff);

				yOff += tp->yBox;

				TransDIBlt(tp,
				tp->xDst + xOff - (tp->xBox / 2),
				tp->yDst + yOff,
				tp->hStep,
				tp->yBox,
				tp->xSrc + xOff - (tp->xBox / 2),
				tp->ySrc + yOff);
			}
		}
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupCheckerboardUp( LPTRANS tp )
/***********************************************************************/
{
	tp->yBox	= (tp->yExt / 4) & 0xfffe;
	tp->yBox	= max(tp->yBox, 16);
	tp->xBox	= tp->yBox / 2;
	tp->vStep = STEPSIZE(tp->yBox / SPEED(8,10,12,14,16));
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(1,12,24,48,96);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepCheckerboardUp( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int xOff;
	int yOff	= tp->yBox - (tp->nIteration * tp->vStep);

	if (yOff >= 0)
	{
		for (yOff += tp->yExt; yOff >= 0; yOff -= tp->yBox)
		{
			for (xOff = 0; xOff < tp->xExt; xOff += tp->xBox)
			{
				TransDIBlt(tp,
				tp->xDst + xOff,
				tp->yDst + yOff,
				tp->xBox,
				tp->vStep,
				tp->xSrc + xOff,
				tp->ySrc + yOff);

				xOff += tp->xBox;

				TransDIBlt(tp,
				tp->xDst + xOff,
				tp->yDst + yOff - (tp->yBox / 2),
				tp->xBox,
				tp->vStep,
				tp->xSrc + xOff,
				tp->ySrc + yOff - (tp->yBox / 2));
			}
		}
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupCoverDown( LPTRANS tp )
/***********************************************************************/
{
	tp->vStep = STEPSIZE(tp->yExt / SPEED(16,24,36,48,60));
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);
	tp->dwTotalIterations = (tp->yExt + tp->vStep - 1) / tp->vStep;

	return(TRUE);
}

/***********************************************************************/
static BOOL stepCoverDown( LPTRANS tp )
/***********************************************************************/
{
	int yHeight = tp->nIteration * tp->vStep;
	BOOL bDone	= FALSE;

	if (yHeight > tp->yExt)
	{
		yHeight = tp->yExt;
		bDone	= TRUE;
	}

	TransDIBlt(tp, tp->xDst, tp->yDst, tp->xExt, yHeight, tp->xSrc, tp->ySrc + (tp->yExt - yHeight));

	return(bDone);
}

/***********************************************************************/
static BOOL setupCoverLeft( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->yExt / SPEED(16,24,36,48,60));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);
	tp->dwTotalIterations = (tp->xExt + tp->hStep - 1) / tp->hStep;

	return(TRUE);
}

/***********************************************************************/
static BOOL stepCoverLeft( LPTRANS tp )
/***********************************************************************/
{
	int xWidth = tp->nIteration * tp->hStep;
	BOOL bDone = FALSE;

	if (xWidth > tp->xExt)
	{
		xWidth = tp->xExt;
		bDone = TRUE;
	}

	TransDIBlt(tp, tp->xDst + (tp->xExt - xWidth), tp->yDst, xWidth, tp->yExt, tp->xSrc, tp->ySrc);

	return(bDone);
}

/***********************************************************************/
static BOOL setupCoverRight( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->yExt / SPEED(16,24,36,48,60));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);
	tp->dwTotalIterations = (tp->xExt + tp->hStep - 1) / tp->hStep;

	return(TRUE);
}

/***********************************************************************/
static BOOL stepCoverRight( LPTRANS tp )
/***********************************************************************/
{
	int xWidth = tp->nIteration * tp->hStep;
	BOOL bDone = FALSE;

	if (xWidth > tp->xExt)
	{
		xWidth = tp->xExt;
		bDone = TRUE;
	}

	TransDIBlt(tp, tp->xDst, tp->yDst, xWidth, tp->yExt, tp->xSrc + (tp->xExt - xWidth), tp->ySrc);

	return(bDone);
}

/***********************************************************************/
static BOOL setupCoverUp( LPTRANS tp )
/***********************************************************************/
{
	tp->vStep = STEPSIZE(tp->yExt / SPEED(16,24,36,48,60));
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);
	tp->dwTotalIterations = (tp->yExt + tp->vStep - 1) / tp->vStep;

	return(TRUE);
}

/***********************************************************************/
static BOOL stepCoverUp( LPTRANS tp )
/***********************************************************************/
{
	int yHeight = tp->nIteration * tp->vStep;
	BOOL bDone	= FALSE;

	if (yHeight > tp->yExt)
	{
		yHeight = tp->yExt;
		bDone	= TRUE;
	}

	TransDIBlt(tp, tp->xDst, tp->yDst + (tp->yExt - yHeight), tp->xExt, yHeight, tp->xSrc, tp->ySrc);

	return(bDone);
}

/***********************************************************************/
static BOOL setupCoverLeftDown( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->yExt / SPEED(16,24,36,48,60));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);

	return(TRUE);
}

/***********************************************************************/
void CALLBACK doCoverLeftDown( int x, int y, LPTRANS lpTrans )
/***********************************************************************/
{
	lpTrans->vStep -= 1;

	if (lpTrans->vStep == 0)
	{
		lpTrans->xPos = x;
		lpTrans->yPos = y;
	}
}

/***********************************************************************/
static BOOL stepCoverLeftDown( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->vStep = (tp->nIteration + 1) * tp->hStep;

	LineDDA(tp->xExt - 1, 0, 0, tp->yExt - 1,
	(LINEDDAPROC)doCoverLeftDown, (LPARAM)(DWORD)tp);

	if (tp->vStep > 0)
	{
		tp->xPos = 0;
		tp->yPos = tp->yExt;
		bDone	= TRUE;
	}

	TransDIBlt(tp, tp->xDst + tp->xPos, tp->yDst, tp->xExt - tp->xPos, tp->yPos, tp->xSrc, tp->ySrc + (tp->yExt - tp->yPos));

	return(bDone);
}

/***********************************************************************/
static BOOL setupCoverLeftUp( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->yExt / SPEED(16,24,36,48,60));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);

	return(TRUE);
}

/***********************************************************************/
void CALLBACK doCoverLeftUp
/***********************************************************************/
(
int x,
int y,
LPTRANS lpTrans
)
{
	lpTrans->vStep -= 1;

	if (lpTrans->vStep == 0)
	{
		lpTrans->xPos = x;
		lpTrans->yPos = y;
	}
}

/***********************************************************************/
static BOOL stepCoverLeftUp( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->vStep = (tp->nIteration + 1) * tp->hStep;

	LineDDA(tp->xExt - 1, tp->yExt - 1, 0, 0,
	(LINEDDAPROC)doCoverLeftDown, (LPARAM)(DWORD)tp);

	if (tp->vStep > 0)
	{
		tp->xPos = 0;
		tp->yPos = 0;
		bDone	= TRUE;
	}

	TransDIBlt(tp, tp->xDst + tp->xPos, tp->yDst + tp->yPos, tp->xExt - tp->xPos, tp->yExt - tp->yPos, tp->xSrc, tp->ySrc);

	return(bDone);
}

/***********************************************************************/
static BOOL setupCoverRightDown( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->yExt / SPEED(16,24,36,48,60));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);

	return(TRUE);
}

/***********************************************************************/
void CALLBACK doCoverRightDown
/***********************************************************************/
(
int x,
int y,
LPTRANS lpTrans
)
{
	lpTrans->vStep -= 1;

	if (lpTrans->vStep == 0)
	{
		lpTrans->xPos = x;
		lpTrans->yPos = y;
	}
}

/***********************************************************************/
static BOOL stepCoverRightDown( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->vStep = (tp->nIteration + 1) * tp->hStep;

	LineDDA(0, 0, tp->xExt - 1, tp->yExt - 1,
	(LINEDDAPROC)doCoverLeftDown, (LPARAM)(DWORD)tp);

	if (tp->vStep > 0)
	{
		tp->xPos = tp->xExt;
		tp->yPos = tp->yExt;
		bDone	= TRUE;
	}

	TransDIBlt(tp, tp->xDst, tp->yDst, tp->xPos, tp->yPos, tp->xSrc + (tp->xExt - tp->xPos), tp->ySrc + (tp->yExt - tp->yPos));

	return(bDone);
}

/***********************************************************************/
static BOOL setupCoverRightUp( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->yExt / SPEED(16,24,36,48,60));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);

	return(TRUE);
}

/***********************************************************************/
void CALLBACK doCoverRightUp
/***********************************************************************/
(
int x,
int y,
LPTRANS lpTrans
)
{
	lpTrans->vStep -= 1;

	if (lpTrans->vStep == 0)
	{
		lpTrans->xPos = x;
		lpTrans->yPos = y;
	}
}

/***********************************************************************/
static BOOL stepCoverRightUp( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->vStep = (tp->nIteration + 1) * tp->hStep;

	LineDDA(0, tp->yExt - 1, tp->xExt - 1, 0,
	(LINEDDAPROC)doCoverLeftDown, (LPARAM)(DWORD)tp);

	if (tp->vStep > 0)
	{
		tp->xPos = tp->xExt;
		tp->yPos = 0;
		bDone	= TRUE;
	}

	TransDIBlt(tp, tp->xDst, tp->yDst + tp->yPos, tp->xPos, tp->yExt - tp->yPos, tp->xSrc + (tp->xExt - tp->xPos), tp->ySrc);

	return(bDone);
}

#define ITERATIONS_PER_STEP 32
#define TIMETEST_STEPS	3

CRandomSequence RandomSequence;

/***********************************************************************/
static BOOL stepDissolve( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int xOff, yOff, xCell, yCell, i;
	DWORD dwSequence;
							   
	for (i = 0; i < ITERATIONS_PER_STEP; ++i)
	{
		dwSequence = RandomSequence.GetNextNumber();
		if (dwSequence == 0xFFFFFFFFL)
		{
			bDone = TRUE;
			break;
		}

		xCell = (int)(dwSequence % (DWORD)tp->xPos);	
		yCell = (int)(dwSequence / (DWORD)tp->xPos);

		if (tp->vStep)
		{
			// blt the cell
			TransDIBltPixel(tp,
				tp->xDst + xCell,
				tp->yDst + yCell,
				tp->xSrc + xCell,
				tp->ySrc + yCell,
				SRCCOPY);
		}
		else
		{
			// get location of this cell
			xOff = xCell * tp->xBox;
			yOff = yCell * tp->yBox;
	
			// blt the cell
			TransDIBlt(tp,
				tp->xDst + xOff,
				tp->yDst + yOff,
				tp->xBox,
				tp->yBox,
				tp->xSrc + xOff,
				tp->ySrc + yOff,
				SRCCOPY);
		}
	}
	return(bDone);
}

/***********************************************************************/
static void initDissolve( LPTRANS tp, int iStepSize )
/***********************************************************************/
{
	tp->nTicks = TICKS(SPEED(20,20,20,20,20));
	// number of cells involved in dissolve
	tp->xPos = tp->xExt / iStepSize;
	tp->yPos = tp->yExt / iStepSize;

	// size of cells
	if ( !tp->xPos ) tp->xPos = 1;
	if ( !tp->yPos ) tp->yPos = 1;
	tp->xBox	= (tp->xExt + (tp->xPos - 1)) / tp->xPos;
	tp->yBox	= (tp->yExt + (tp->yPos - 1)) / tp->yPos;

	tp->vStep = (tp->xBox == 1) && (tp->yBox == 1);
	RandomSequence.Init((WORD)tp->xPos*(DWORD)tp->yPos);
}

/***********************************************************************/
static BOOL setupDissolve( LPTRANS tp )
/***********************************************************************/
{
	DWORD dwTime, dwTicks, dwMax;
	int iSteps, iStepSize;

	if (tp->dwTotalTime)
	{
		iStepSize = STEPSIZE(32);
		do
		{
			initDissolve(tp, iStepSize);
			dwMax = (((DWORD)tp->xPos * (DWORD)tp->yPos)+ITERATIONS_PER_STEP-1) / ITERATIONS_PER_STEP;
			dwTicks = ((tp->dwTotalTime*TIMETEST_STEPS)+(dwMax/2)) / dwMax;
			dwTime = timeGetTime();
			iSteps = TIMETEST_STEPS;
			while (--iSteps >= 0)
				stepDissolve(tp);
			dwTime = timeGetTime() - dwTime;
			iStepSize += 1;
		}
		while (dwTime > dwTicks);

		tp->dwTotalIterations = dwMax;
	}
	else
	{
		iStepSize = STEPSIZE(32);
		dwTicks = (DWORD)tp->nTicks * TIMETEST_STEPS;
		do
		{
			initDissolve(tp, iStepSize);
			dwTime = timeGetTime();
			iSteps = TIMETEST_STEPS;
			while (--iSteps >= 0)
				stepDissolve(tp);
			dwTime = timeGetTime() - dwTime;
			iStepSize += 1;
		}
		while (dwTime > dwTicks);
	}

	return(TRUE);

}

#ifdef UNUSED
/***********************************************************************/
static BOOL setupDissolve( LPTRANS tp )
/***********************************************************************/
{
	tp->nTicks = TICKS(SPEED(1,1,1,1,1));
	tp->xBox	=
	tp->yBox	= SPEED(20, 16, 12, 8, 4);
	tp->xPos	= (tp->xExt + (tp->xBox - 1)) / tp->xBox;
	tp->xPos	= max(1, (tp->xExt + (32 - 1)) / 32);
	tp->yPos	= (tp->yExt + (tp->yBox - 1)) / tp->yBox;
	tp->yPos	= max(1, (tp->yExt + (32 - 1)) / 32);

	srand(LOWORD(GetCurrentTime()));

	if (tp->hData = GlobalAlloc(GMEM_MOVEABLE, ((32 * 32) + 7L) / 8))
	{
		tp->lpData = (LPBYTE)GlobalLock(tp->hData);
		_fmemset(tp->lpData, 0, (WORD)GlobalSize(tp->hData));
	}

	//return(FALSE);
	return(tp->lpData != NULL);
}

		//++tp->dwIteration;

		// see if we are done yet
		if (tp->dwIteration < dwMax)
		{
			if (tp->vStep)
			{
				// get random cell for both x and y position
				xCell = rand() % tp->xPos;
				yCell = rand() % tp->yPos;

				// get location of cell in bitmap to see if we've been there
				lBit = ((long)yCell * (long)tp->xPos) + (long)xCell;
				while (bitTEST(tp->lpData, lBit))
				{
					// already processed this cell, so increment to next one
					xCell = (xCell + 1) % tp->xPos;
					// x wrapped, so increment y
					if (xCell == 0)
						yCell = (yCell + 1) % tp->yPos;
					// get new location in bitmap
					lBit = ((long)yCell * (long)tp->xPos) + (long)xCell;
				}
				bitSET(tp->lpData, lBit);
			}
			else
			{
				short FAR *lpTable = (short FAR *)tp->lpData;
				r = lpTable[tp->dwIteration] & 0x7FFF;
				xCell = r % tp->xPos;	
				yCell = r / tp->xPos;
			}
	
			// get location of this cell
			xOff = xCell * tp->xBox;
			yOff = yCell * tp->yBox;
	
			// blt the cell
			TransDIBlt(tp,
				tp->xDst + xOff,
				tp->yDst + yOff,
				tp->xBox,
				tp->yBox,
				tp->xSrc + xOff,
				tp->ySrc + yOff,
				SRCCOPY);
			++tp->dwIteration;
		}
		else
		{
			bDone = TRUE;
			break;
		}

	srand(LOWORD(GetCurrentTime()));

	DWORD dwMax = (DWORD)tp->xPos * (DWORD)tp->yPos;
	// vStep is TRUE is using bit table
	tp->vStep = dwMax > RAND_MAX;

	if (tp->vStep)
		tp->hData = GlobalAlloc(GMEM_MOVEABLE, (((long)tp->xPos * (long)tp->yPos) + 7L) / 8);
	else
		tp->hData = GlobalAlloc(GMEM_MOVEABLE, (long)tp->xPos * (long)tp->yPos * sizeof(short));

	if (tp->hData)
	{
		tp->lpData = (LPBYTE)GlobalLock(tp->hData);
		_fmemset(tp->lpData, 0, (WORD)GlobalSize(tp->hData));
	}

	if (!tp->vStep)
	{
		short FAR *lpTable = (short FAR *)tp->lpData;
		int i, r, num, max;

		max = (int)dwMax;
		for (i = 0; i < max; ++i)
		{
			r = rand();
			num = r % max;
			while (lpTable[num] & 0x8000) // already used?
			{                       
				++num;
				if (num >= max) // wrap
					num = 0;
			}
			lpTable[i] |= num;
			lpTable[num] |= 0x8000;
		}
	}

	return(tp->lpData != NULL);

/***********************************************************************/
static BOOL stepDissolve( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
//	int nBit;

	while ((tp->nIteration + 1) % 8)
	{
		if ((int)tp->nIteration < (32 * 32))
		{
	//		int xGrid = rand() % tp->xPos;
	//		int yGrid = rand() % tp->yPos;
	//
	//		nBit = (yGrid * tp->xPos) + xGrid;
	//
	//		while (bitTEST(tp->lpData, nBit))
	//		{
	//			if ((xGrid = (xGrid + 1) % tp->xPos) == 0)
	//				yGrid = (yGrid + 1) % tp->yPos;
	//
	//			nBit = (yGrid * tp->xPos) + xGrid;
	//		}
	//
	//		bitSET(tp->lpData, nBit);
	//
	//		xGrid = tp->xPos;
	//		yGrid = tp->yPos;
	
			int xCell = rand() % 32;
			int yCell = rand() % 32;
	
			while (bitTEST(tp->lpData, (yCell * 32) + xCell))
			{
				if ((xCell = (xCell + 1) % 32) == 0)
					yCell = (yCell + 1) % 32;
			}
	
			bitSET(tp->lpData, (yCell * 32) + xCell);
	
			for (int Row = 0; Row < tp->yPos; ++Row)
			{
				for (int Col = 0; Col < tp->xPos; ++Col)
				{
					int xOff = (Col * (32 * tp->xBox)) + (xCell * tp->xBox);
					int yOff = (Row * (32 * tp->yBox)) + (yCell * tp->yBox);
	
					TransDIBlt(tp,
						tp->xDst + xOff,
						tp->yDst + yOff,
						tp->xBox,
						tp->yBox,
						tp->xSrc + xOff,
						tp->ySrc + yOff,
						SRCCOPY);
				}
			}
			tp->nIteration += 1;
		}
		else
			bDone = TRUE;
	}

//	DWORD point = OldDitherOrder(tp->nIteration, tp->Rate);
//	int X	= LOWORD(point) * tp->xBox;
//	int Y	= HIWORD(point) * tp->yBox;
//
//	if (X <= tp->xExt && Y <= tp->yExt)
//	{
//		TransDIBlt(tp, tp->xDst + X, tp->yDst + Y,
//			tp->xBox, tp->yBox, tp->xSrc + X, tp->ySrc + Y, SRCCOPY);
//	}
//
//	tp->nSteps	-= 1;
//	tp->nIteration += 1;

	return(bDone);
}
#endif

/***********************************************************************/
static BOOL setupFadeThroughBlack( LPTRANS tp )
/***********************************************************************/
{
	UINT speed = Speed(tp->Transition);

	tp->hStep = STEPSIZE(8);
	tp->vStep = 255;
	tp->nTicks = TICKS(SPEED(1,1,1,1,1));
//	tp->nTicks = TICKS
//		(speed == TRANS_FAST ? 10 : speed == TRANS_MEDIUM ? 20 : 100);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepFadeThroughBlack( LPTRANS tp )
/***********************************************************************/
{
	if (tp->vStep == 0)
	{
		tp->hStep = -tp->hStep;
		tp->vStep = 0;
	}

	tp->vStep -= tp->hStep;

	if (tp->vStep < 0)
		tp->vStep = 0;
	else if (tp->vStep > 255)
		tp->vStep = 255;

	HBRUSH FadeBrush, OldBrush;

	FadeBrush = CreateSolidBrush(RGB(tp->vStep, tp->vStep, tp->vStep));

	OldBrush = (HBRUSH)SelectObject(tp->dcDst, FadeBrush);

	TransDIBlt(tp,
		tp->xDst,
		tp->yDst,
		tp->xExt,
		tp->yExt,
		tp->xSrc,
		tp->ySrc,
		tp->hStep > 0 ? 0x00a000c9L : MERGECOPY);
//	tp->hStep > 0 ? 0x00a000c9L : 0x00ca0749L);

	SelectObject(tp->dcDst, OldBrush);
	return((tp->hStep < 0) && (tp->vStep == 255));
}

/***********************************************************************/
static BOOL setupXRay( LPTRANS tp )
/***********************************************************************/
{
	tp->nTicks = SPEED(5,10,20,40,60);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepXRay( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	if (tp->nIteration < 7)
	{
		TransDIBlt(tp, tp->xDst, tp->yDst, tp->xExt, tp->yExt, 0, 0, DSTINVERT);
	}
	else if (tp->nIteration == 7)
	{
		TransDIBlt(tp, tp->xDst, tp->yDst, tp->xExt, tp->yExt, tp->xSrc, tp->ySrc, SRCCOPY);
	}
	else if (tp->nIteration < 16)
	{
		TransDIBlt(tp, tp->xDst, tp->yDst, tp->xExt, tp->yExt, 0, 0, DSTINVERT);
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupStretchHorizontalOut( LPTRANS tp )
/***********************************************************************/
{
	tp->vStep = STEPSIZE((tp->yExt / 2) / SPEED(4,6,8,10,12));
	tp->nTicks = SPEED(1,2,5,10,15);
	tp->yMin	=
	tp->yPos	= tp->yExt / 2;
	tp->yMax	=
	tp->xPos	= tp->yMin + 1;

	return(TRUE);
}

/***********************************************************************/
static BOOL stepStretchHorizontalOut( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->yPos -= tp->vStep;
	tp->xPos += tp->vStep;

	if (tp->yPos <= 0 || tp->xPos >= tp->yExt)
	{
		tp->yPos = 0;
		tp->xPos = tp->yExt;
		bDone		= TRUE;
	}

	TransStretchDIBlt(tp,
		tp->xDst,
		tp->yDst + tp->yPos,
		tp->xExt,
		tp->yMin - tp->yPos + 1,
		tp->xSrc,
		tp->ySrc,
		tp->xExt,
		tp->yExt - tp->yMin + 1,
		SRCCOPY);

	TransStretchDIBlt(tp,
		tp->xDst,
		tp->yDst + tp->yMax,
		tp->xExt,
		tp->xPos - tp->yMax + 1,
		tp->xSrc,
		tp->ySrc + tp->yMax,
		tp->xExt,
		tp->yExt - tp->yMax,
		SRCCOPY);

	return(bDone);
}

/***********************************************************************/
static BOOL setupStretchVerticalOut( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE((tp->xExt / 2) / SPEED(4,6,8,10,12));
	tp->nTicks = SPEED(10,20,50,100,150);
	tp->xMin	=
	tp->xPos	= tp->xExt / 2;
	tp->xMax	=
	tp->yPos	= tp->xMin + 1;

	return(TRUE);
}

/***********************************************************************/
static BOOL stepStretchVerticalOut( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->xPos -= tp->hStep;
	tp->yPos += tp->hStep;

	if (tp->xPos <= 0 || tp->yPos >= tp->xExt)
	{
		tp->xPos = 0;
		tp->yPos = tp->xExt;
		bDone		= TRUE;
	}

	TransStretchDIBlt(tp,
		tp->xDst + tp->xPos,
		tp->yDst,
		tp->xMin - tp->xPos + 1,
		tp->yExt,
		tp->xSrc,
		tp->ySrc,
		tp->xExt - tp->xMin + 1,
		tp->yExt,
		SRCCOPY);

	TransStretchDIBlt(tp,
		tp->xDst + tp->xMax,
		tp->yDst,
		tp->yPos - tp->xMax + 1,
		tp->yExt,
		tp->xSrc + tp->xMax,
		tp->ySrc,
		tp->xExt - tp->xMax,
		tp->yExt,
		SRCCOPY);

	return(bDone);
}

/***********************************************************************/
static BOOL setupStretchDown( LPTRANS tp )
/***********************************************************************/
{
	tp->vStep = STEPSIZE((tp->yExt / 2) / SPEED(4,6,8,10,12));
	tp->nTicks = SPEED(1,2,5,10,15);
	tp->yPos	= 0;

	return(TRUE);
}

/***********************************************************************/
static BOOL stepStretchDown( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->yPos += tp->vStep;

	if (tp->yPos >= tp->yExt)
	{
		tp->yPos = tp->yExt;
		bDone		= TRUE;
	}

	TransStretchDIBlt(tp,
		tp->xDst,
		tp->yDst,
		tp->xExt,
		tp->yPos,
		tp->xSrc,
		tp->ySrc,
		tp->xExt,
		tp->yExt,
		SRCCOPY);

	return(bDone);
}

/***********************************************************************/
static BOOL setupStretchLeft( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE((tp->xExt / 2) / SPEED(4,6,8,10,12));
	tp->nTicks = SPEED(1,2,5,10,15);
	tp->xPos	= tp->xExt;
	tp->dwTotalIterations = (tp->xExt + tp->hStep - 1) / tp->hStep;
	return(TRUE);
}

/***********************************************************************/
static BOOL stepStretchLeft( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->xPos -= tp->hStep;

	if (tp->xPos <= 0)
	{
		tp->xPos = 0;
		bDone		= TRUE;
	}

	TransStretchDIBlt(tp,
		tp->xDst + tp->xPos,
		tp->yDst,
		tp->xExt - tp->xPos,
		tp->yExt,
		tp->xSrc,
		tp->ySrc,
		tp->xExt,
		tp->yExt,
		SRCCOPY);

	return(bDone);
}

/***********************************************************************/
static BOOL setupStretchRight( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE((tp->xExt / 2) / SPEED(4,6,8,10,12));
	tp->nTicks = SPEED(1,2,5,10,15);
	tp->xPos	= 0;
	tp->dwTotalIterations = (tp->xExt + tp->hStep - 1) / tp->hStep;
	return(TRUE);
}

/***********************************************************************/
static BOOL stepStretchRight( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->xPos += tp->hStep;

	if (tp->xPos >= tp->xExt)
	{
		tp->xPos = tp->xExt;
		bDone		= TRUE;
	}

	TransStretchDIBlt(tp,
		tp->xDst,
		tp->yDst,
		tp->xPos,
		tp->yExt,
		tp->xSrc,
		tp->ySrc,
		tp->xExt,
		tp->yExt,
		SRCCOPY);

	return(bDone);
}

/***********************************************************************/
static BOOL setupStretchUp( LPTRANS tp )
/***********************************************************************/
{
	tp->vStep = STEPSIZE((tp->yExt / 2) / SPEED(4,6,8,10,12));
	tp->nTicks = SPEED(1,2,5,10,15);
	tp->yPos	= tp->yExt;

	return(TRUE);
}

/***********************************************************************/
static BOOL stepStretchUp( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->yPos -= tp->vStep;

	if (tp->yPos <= 0)
	{
		tp->yPos = 0;
		bDone		= TRUE;
	}

	TransStretchDIBlt(tp,
		tp->xDst,
		tp->yDst + tp->yPos,
		tp->xExt,
		tp->yExt - tp->yPos,
		tp->xSrc,
		tp->ySrc,
		tp->xExt,
		tp->yExt,
		SRCCOPY);

	return(bDone);
}

/***********************************************************************/
static BOOL setupRainDown( LPTRANS tp )
/***********************************************************************/
{
	tp->vStep = STEPSIZE(SPEED(16,8,6,4,2));
	tp->xBox	= tp->xExt / 50; //SPEED(40,50,60,70,80);
	tp->xBox &= 0xfffe;
	tp->xBox	= max(tp->xBox, 2);
	tp->yBox	= tp->xBox * 20;
	tp->nTicks = SPEED(1,1,4,8,8);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepRainDown( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int yOff = tp->nIteration * tp->vStep;

	if (yOff < tp->yBox)
	{
		yOff -= tp->yBox;

		for (; ((yOff - tp->yBox) < tp->yExt); yOff += tp->yBox)
		{
			for (int xOff = 0; xOff < tp->xExt;)
			{
				for (int i = 0; i < 20; ++i)
				{
					TransDIBlt(tp,
					tp->xDst + xOff,
					tp->yDst + yOff + (rainoff[i] * tp->xBox),
					tp->xBox,
					tp->vStep,
					tp->xSrc + xOff,
					tp->ySrc + yOff + (rainoff[i] * tp->xBox));

					xOff += tp->xBox;
				}
			}
		}
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupRainLeft( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(SPEED(16,8,6,4,2));
	tp->yBox	= tp->yExt / SPEED(40,50,60,70,80);
	tp->yBox &= 0xfffe;
	tp->yBox	= max(tp->yBox, 2);
	tp->xBox	= tp->yBox * 20;
	tp->nTicks = SPEED(1,1,4,8,8);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepRainLeft( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int xOff = -((int)tp->nIteration * tp->hStep);

	if (xOff >= -tp->xBox)
	{
		xOff += tp->xExt + (tp->xBox * 2);

		for (; ((xOff + tp->xBox) >= 0); xOff -= tp->xBox)
		{
			for (int yOff = 0; yOff < tp->yExt;)
			{
				for (int i = 0; i < 20; ++i)
				{
					TransDIBlt(tp,
					tp->xDst + xOff + (rainoff[i] * tp->yBox),
					tp->yDst + yOff,
					tp->hStep,
					tp->yBox,
					tp->xSrc + xOff + (rainoff[i] * tp->yBox),
					tp->ySrc + yOff);

					yOff += tp->yBox;
				}
			}
		}
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupRainRight( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(SPEED(16,8,6,4,2));
	tp->yBox	= tp->yExt / SPEED(40,50,60,70,80);
	tp->yBox &= 0xfffe;
	tp->yBox	= max(tp->yBox, 2);
	tp->xBox	= tp->yBox * 20;
	tp->nTicks = SPEED(1,1,4,8,8);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepRainRight( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int xOff = tp->nIteration * tp->hStep;

	if (xOff < tp->xBox)
	{
		xOff -= tp->xBox;

		for (; ((xOff - tp->xBox) < tp->xExt); xOff += tp->xBox)
		{
			for (int yOff = 0; yOff < tp->yExt;)
			{
				for (int i = 0; i < 20; ++i)
				{
					TransDIBlt(tp,
					tp->xDst + xOff + (rainoff[i] * tp->yBox),
					tp->yDst + yOff,
					tp->hStep,
					tp->yBox,
					tp->xSrc + xOff + (rainoff[i] * tp->yBox),
					tp->ySrc + yOff);

					yOff += tp->yBox;
				}
			}
		}
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupRainUp( LPTRANS tp )
/***********************************************************************/
{
	tp->vStep = STEPSIZE(SPEED(16,8,6,4,2));
	tp->xBox	= tp->xExt / SPEED(40,50,60,70,80);
	tp->xBox &= 0xfffe;
	tp->xBox	= max(tp->xBox, 2);
	tp->yBox	= tp->xBox * 20;
	tp->nTicks = SPEED(1,1,4,8,8);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepRainUp( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int yOff = -((int)tp->nIteration * tp->vStep);

	if (yOff >= -tp->yBox)
	{
		yOff += tp->yExt + (tp->yBox * 2);

		for (; ((yOff + tp->yBox) >= 0); yOff -= tp->yBox)
		{
			for (int xOff = 0; xOff < tp->xExt;)
			{
				for (int i = 0; i < 20; ++i)
				{
					TransDIBlt(tp,
					tp->xDst + xOff,
					tp->yDst + yOff + (rainoff[i] * tp->xBox),
					tp->xBox,
					tp->vStep,
					tp->xSrc + xOff,
					tp->ySrc + yOff + (rainoff[i] * tp->xBox));

					xOff += tp->xBox;
				}
			}
		}
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupRandomBarsHorizontal( LPTRANS tp )
/***********************************************************************/
{
	tp->nTicks = SPEED(1,4,8,12,16);

	srand(LOWORD(GetCurrentTime()));

	if (tp->hData = GlobalAlloc(GMEM_MOVEABLE, (tp->yExt + 7L) / 8))
	{
		tp->lpData = (LPBYTE)GlobalLock(tp->hData);
		_fmemset(tp->lpData, 0, (WORD)GlobalSize(tp->hData));
	}

	return(tp->lpData != NULL);
}

/***********************************************************************/
static BOOL stepRandomBarsHorizontal( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int yBlend;

	if (tp->nIteration < (UINT)tp->yExt)
	{
		yBlend = rand() % tp->yExt;

		while (bitTEST(tp->lpData, yBlend))
			yBlend = (yBlend + 1) % tp->yExt;

		bitSET(tp->lpData, yBlend);

		TransDIBlt(tp, tp->xDst, tp->yDst + yBlend, tp->xExt, 1, tp->xSrc, tp->ySrc + yBlend);
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupRandomBarsVertical( LPTRANS tp )
/***********************************************************************/
{
	tp->nTicks = SPEED(1,4,8,12,16);

	srand(LOWORD(GetCurrentTime()));

	if (tp->hData = GlobalAlloc(GMEM_MOVEABLE, (tp->xExt + 7L) / 8))
	{
		tp->lpData = (LPBYTE)GlobalLock(tp->hData);
		_fmemset(tp->lpData, 0, (WORD)GlobalSize(tp->hData));
	}

	return(tp->lpData != NULL);
}

/***********************************************************************/
static BOOL stepRandomBarsVertical( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int xBlend;

	if (tp->nIteration < (UINT)tp->xExt)
	{
		xBlend = rand() % tp->xExt;

		while (bitTEST(tp->lpData, xBlend))
			xBlend = (xBlend + 1) % tp->xExt;

		bitSET(tp->lpData, xBlend);

		TransDIBlt(tp, tp->xDst + xBlend, tp->yDst, 1, tp->yExt, tp->xSrc + xBlend, tp->ySrc);
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupRotateHorizontal( LPTRANS tp )
/***********************************************************************/
{
	UINT speed = Speed(tp->Transition);

	tp->hStep = 1;
	tp->vStep = 1;
	tp->yMin	= 0;
	tp->nTicks = TICKS(speed == TRANS_FAST ? 50 : speed == TRANS_MEDIUM ? 100 : 200);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepRotateHorizontal( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone;
	int nSlices = tp->nIteration + 4;
	int cySlice = ((tp->yExt / 2)) / nSlices;

	for (int i = 0; i < ((int)tp->nIteration + 4); --nSlices, ++i)
	{
		StretchBlt(tp->dcDst,
			tp->xDst + ((i + 1) * tp->hStep),
			tp->yDst + ((nSlices - 1) * cySlice) + ((i + 1) * tp->vStep),
			tp->xExt - (((i + 1) * 2) * tp->hStep),
			cySlice - tp->vStep,
			tp->dcDst,
			tp->xDst,
			tp->yDst + ((nSlices - 1) * cySlice),
			tp->xExt,
			cySlice,
			SRCCOPY);

		TransDIBlt(tp,
			tp->xDst,
			tp->yDst + ((nSlices - 1) * cySlice),
			(i + 1) * tp->hStep,
			cySlice,
			0, 0, BLACKNESS);

		TransDIBlt(tp,
			tp->xDst + tp->xExt - ((i + 1) * tp->hStep),
			tp->yDst + ((nSlices - 1) * cySlice),
			(i + 1) * tp->hStep,
			cySlice,
			0, 0, BLACKNESS);

		StretchBlt(tp->dcDst,
			tp->xDst - ((i + 1) * tp->hStep),
			tp->yDst + (tp->yExt / 2) + (i * cySlice) - (i * tp->vStep),
			tp->xExt + (((i + 1) * 3) * tp->hStep),
			cySlice + tp->vStep,
			tp->dcDst,
			tp->xDst,
			tp->yDst + (tp->yExt / 2) + (i * cySlice),
			tp->xExt,
			cySlice,
			SRCCOPY);
	}

	tp->yMin += (tp->nIteration + 4) * tp->vStep;

	bDone = (tp->yMin >= (tp->yExt / 2));

	if (!bDone)
	{
		TransDIBlt(tp,
			tp->xDst,
			tp->yDst,
			tp->xExt,
			tp->yMin,
			0, 0, BLACKNESS);

		TransDIBlt(tp,
			tp->xDst,
			tp->yDst + tp->yExt - tp->yMin,
			tp->xExt,
			tp->yMin,
			0, 0, BLACKNESS);
	}
	else
	{
		TransDIBlt(tp,
			tp->xDst,
			tp->yDst,
			tp->xExt,
			tp->yExt,
			0, 0, BLACKNESS);
	}

	return(bDone);
}

/***********************************************************************/
static BOOL setupRotateVertical( LPTRANS tp )
/***********************************************************************/
{
	return(FALSE);
}

/***********************************************************************/
static BOOL stepRotateVertical( LPTRANS tp )
/***********************************************************************/
{
	return(TRUE);
}

/***********************************************************************/
static BOOL setupSpiralClockwiseIn( LPTRANS tp )
/***********************************************************************/
{
	tp->xBox	= STEPSIZE((tp->xExt + 11) / 12);
	tp->xBox &= 0xfffe; // insure even
	tp->xBox	= max(tp->xBox, 16);
	tp->yBox	= (int)(((long)tp->xBox * tp->yExt) / (long)tp->xExt);
	tp->xPos	= -tp->xBox;
	tp->yPos	= 0;
	tp->hStep = tp->xBox;
	tp->vStep = 0;
	tp->xMin	= 0;
	tp->yMin	= tp->yBox;
	tp->xMax	= tp->xExt;
	tp->yMax	= tp->yExt;

	tp->nTicks = SPEED(1,2,5,10,15);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepSpiralClockwiseIn( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	if (tp->hStep > 0)
	{
		tp->xPos += tp->hStep;

		if (tp->xPos >= tp->xMax)
		{
			tp->xPos	= tp->xMax - tp->xBox;
			tp->yPos += tp->yBox;
			tp->hStep = 0;
			tp->vStep = tp->yBox;

			tp->xMax -= tp->xBox;
		}
	}
	else if (tp->hStep < 0)
	{
		tp->xPos += tp->hStep;

		if ((tp->xPos + tp->xBox) <= tp->xMin)
		{
			tp->xPos	= tp->xMin;
			tp->yPos -= tp->yBox;
			tp->hStep = 0;
			tp->vStep = -tp->yBox;

			tp->xMin += tp->xBox;
		}
	}
	else if (tp->vStep > 0)
	{
		tp->yPos += tp->vStep;

		if (tp->yPos >= tp->yMax)
		{
			tp->yPos	= tp->yMax - tp->yBox;
			tp->xPos -= tp->xBox;
			tp->hStep = -tp->xBox;
			tp->vStep = 0;

			tp->yMax -= tp->yBox;
		}
	}
	else if (tp->vStep < 0)
	{
		tp->yPos += tp->vStep;

		if ((tp->yPos + tp->yBox) < tp->yMin)
		{
			tp->yPos	= tp->yMin;
			tp->xPos += tp->xBox;
			tp->hStep = tp->xBox;
			tp->vStep = 0;

			tp->yMin += tp->yBox;
		}

		if (tp->yMin >= tp->yMax)
			bDone = TRUE;
	}

	TransDIBlt(tp, tp->xDst + tp->xPos, tp->yDst + tp->yPos, tp->xBox, tp->yBox, tp->xSrc + tp->xPos, tp->ySrc + tp->yPos);

	return(bDone);
}

/***********************************************************************/
static BOOL setupSpiralClockwiseOut( LPTRANS tp )
/***********************************************************************/
{
	tp->xBox	= STEPSIZE((tp->xExt + 11) / 12);
	tp->xBox &= 0xfffe; // insure even
	tp->xBox	= max(tp->xBox, 16);
	tp->yBox	= (int)(((long)tp->xBox * tp->yExt) / (long)tp->xExt);
	tp->xPos	= (tp->xExt / 2) - (tp->xBox / 2) - tp->xBox;
	tp->yPos	= (tp->yExt / 2) - (tp->yBox / 2);
	tp->hStep = tp->xBox;
	tp->vStep = 0;
	tp->xMin	= tp->xPos;
	tp->yMin	= tp->yPos - tp->yBox;
	tp->xMax	= tp->xPos + (tp->xBox * 2);
	tp->yMax	= tp->yPos + tp->yBox;

	tp->nTicks = SPEED(1,2,5,10,15);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepSpiralClockwiseOut( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	if (tp->hStep > 0)
	{
		tp->xPos += tp->hStep;

		if (tp->xPos > tp->xMax)
		{
			tp->xPos	= tp->xMax;
			tp->yPos += tp->yBox;
			tp->hStep = 0;
			tp->vStep = tp->yBox;

			tp->xMax += tp->xBox;

			if ((tp->xMax - tp->xBox) >= tp->xExt)
				bDone = TRUE;
		}
	}
	else if (tp->hStep < 0)
	{
		tp->xPos += tp->hStep;

		if (tp->xPos < tp->xMin)
		{
			tp->xPos	= tp->xMin;
			tp->yPos -= tp->yBox;
			tp->hStep = 0;
			tp->vStep = -tp->yBox;

			tp->xMin -= tp->xBox;
		}
	}
	else if (tp->vStep > 0)
	{
		tp->yPos += tp->vStep;

		if (tp->yPos > tp->yMax)
		{
			tp->yPos	= tp->yMax;
			tp->xPos -= tp->xBox;
			tp->hStep = -tp->xBox;
			tp->vStep = 0;

			tp->yMax += tp->yBox;
		}
	}
	else if (tp->vStep < 0)
	{
		tp->yPos += tp->vStep;

		if (tp->yPos < tp->yMin)
		{
			tp->yPos	= tp->yMin;
			tp->xPos += tp->xBox;
			tp->hStep = tp->xBox;
			tp->vStep = 0;

			tp->yMin -= tp->yBox;
		}
	}

	TransDIBlt(tp, tp->xDst + tp->xPos, tp->yDst + tp->yPos, tp->xBox, tp->yBox, tp->xSrc + tp->xPos, tp->ySrc + tp->yPos);

	return(bDone);
}

/***********************************************************************/
static BOOL setupSpiralCounterclockwiseIn( LPTRANS tp )
/***********************************************************************/
{
	tp->xBox	= STEPSIZE((tp->xExt + 11) / 12);
	tp->xBox &= 0xfffe; // insure even
	tp->xBox	= max(tp->xBox, 16);
	tp->yBox	= (int)(((long)tp->xBox * tp->yExt) / (long)tp->xExt);
	tp->xPos	= tp->xExt;
	tp->yPos	= 0;
	tp->hStep = -tp->xBox;
	tp->vStep = 0;
	tp->xMin	= 0;
	tp->yMin	= tp->yBox;
	tp->xMax	= tp->xExt;
	tp->yMax	= tp->yExt;

	tp->nTicks = SPEED(1,2,5,10,15);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepSpiralCounterclockwiseIn( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	if (tp->hStep > 0)
	{
		tp->xPos += tp->hStep;

		if (tp->xPos >= tp->xMax)
		{
			tp->xPos	= tp->xMax - tp->xBox;
			tp->yPos += tp->yBox;
			tp->hStep = 0;
			tp->vStep = -tp->yBox;

			tp->xMax -= tp->xBox;
		}
	}
	else if (tp->hStep < 0)
	{
		tp->xPos += tp->hStep;

		if ((tp->xPos + tp->xBox) <= tp->xMin)
		{
			tp->xPos	= tp->xMin;
			tp->yPos -= tp->yBox;
			tp->hStep = 0;
			tp->vStep = tp->yBox;

			tp->xMin += tp->xBox;
		}
	}
	else if (tp->vStep > 0)
	{
		tp->yPos += tp->vStep;

		if (tp->yPos >= tp->yMax)
		{
			tp->yPos	= tp->yMax - tp->yBox;
			tp->xPos -= tp->xBox;
			tp->hStep = tp->xBox;
			tp->vStep = 0;

			tp->yMax -= tp->yBox;
		}
	}
	else if (tp->vStep < 0)
	{
		tp->yPos += tp->vStep;

		if ((tp->yPos + tp->yBox) < tp->yMin)
		{
			tp->yPos	= tp->yMin;
			tp->xPos += tp->xBox;
			tp->hStep = -tp->xBox;
			tp->vStep = 0;

			tp->yMin += tp->yBox;
		}

		if ((tp->xMin >= tp->xMax) && (tp->yMin >= tp->yMax))
			bDone = TRUE;
	}

	TransDIBlt(tp, tp->xDst + tp->xPos, tp->yDst + tp->yPos, tp->xBox, tp->yBox, tp->xSrc + tp->xPos, tp->ySrc + tp->yPos);

	return(bDone);
}

/***********************************************************************/
static BOOL setupSpiralCounterclockwiseOut( LPTRANS tp )
/***********************************************************************/
{
	tp->xBox	= STEPSIZE((tp->xExt + 11) / 12);
	tp->xBox &= 0xfffe; // insure even
	tp->xBox	= max(tp->xBox, 16);
	tp->yBox	= (int)(((long)tp->xBox * tp->yExt) / (long)tp->xExt);
	tp->xPos	= (tp->xExt / 2) - (tp->xBox / 2) + tp->xBox;
	tp->yPos	= (tp->yExt / 2) - (tp->yBox / 2);
	tp->hStep = -tp->xBox;
	tp->vStep = 0;
	tp->xMin	= tp->xPos - (tp->xBox * 2);
	tp->yMin	= tp->yPos - tp->yBox;
	tp->xMax	= tp->xPos;
	tp->yMax	= tp->yPos + tp->yBox;

	tp->nTicks = SPEED(1,2,5,10,15);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepSpiralCounterclockwiseOut( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	if (tp->hStep > 0)
	{
		tp->xPos += tp->hStep;

		if (tp->xPos > tp->xMax)
		{
			tp->xPos	= tp->xMax;
			tp->yPos -= tp->yBox;
			tp->hStep = 0;
			tp->vStep = -tp->yBox;

			tp->xMax += tp->xBox;
		}
	}
	else if (tp->hStep < 0)
	{
		tp->xPos += tp->hStep;

		if (tp->xPos < tp->xMin)
		{
			tp->xPos	= tp->xMin;
			tp->yPos += tp->yBox;
			tp->hStep = 0;
			tp->vStep = tp->yBox;

			tp->xMin -= tp->xBox;

			if ((tp->xMin + tp->xBox) < 0)
				if ((tp->yMin + tp->yBox) < 0)
					bDone = TRUE;
		}
	}
	else if (tp->vStep > 0)
	{
		tp->yPos += tp->vStep;

		if (tp->yPos > tp->yMax)
		{
			tp->yPos	= tp->yMax;
			tp->xPos += tp->xBox;
			tp->hStep = tp->xBox;
			tp->vStep = 0;

			tp->yMax += tp->yBox;
		}
	}
	else if (tp->vStep < 0)
	{
		tp->yPos += tp->vStep;

		if (tp->yPos < tp->yMin)
		{
			tp->yPos	= tp->yMin;
			tp->xPos -= tp->xBox;
			tp->hStep = -tp->xBox;
			tp->vStep = 0;

			tp->yMin -= tp->yBox;
		}
	}

	TransDIBlt(tp, tp->xDst + tp->xPos, tp->yDst + tp->yPos, tp->xBox, tp->yBox, tp->xSrc + tp->xPos, tp->ySrc + tp->yPos);

	return(bDone);
}

/***********************************************************************/
static BOOL setupSplitHorizontalIn( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE((tp->yExt / 2) / (SPEED(40,50,60,70,80)));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(2,5,10,14,20);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepSplitHorizontalIn( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int yTop = tp->nIteration * tp->hStep;
	int yBot = tp->yExt - (tp->nIteration * tp->hStep);

	if (yTop <= ((tp->yExt / 2) + tp->hStep) ||
		yBot >= ((tp->yExt / 2) - tp->hStep))
	{
		TransDIBlt(tp, tp->xDst, tp->yDst + yTop, tp->xExt, tp->hStep, tp->xSrc, tp->ySrc + yTop);

		TransDIBlt(tp, tp->xDst, tp->yDst + yBot, tp->xExt, tp->hStep, tp->xSrc, tp->ySrc + yBot);
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupSplitHorizontalOut( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE((tp->yExt / 2) / (SPEED(40,50,60,70,80)));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(2,5,10,14,20);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepSplitHorizontalOut( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int yTop = (tp->yExt / 2) - tp->nIteration * tp->hStep;
	int yBot = (tp->yExt / 2) + tp->nIteration * tp->hStep;

	if (yTop >= -(tp->hStep) || yBot <= (tp->yExt + tp->hStep))
	{
		TransDIBlt(tp, tp->xDst, tp->yDst + yTop, tp->xExt, tp->hStep, tp->xSrc, tp->ySrc + yTop);

		TransDIBlt(tp, tp->xDst, tp->yDst + yBot, tp->xExt, tp->hStep, tp->xSrc, tp->ySrc + yBot);
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupSplitVerticalIn( LPTRANS tp )
/***********************************************************************/
{
	tp->vStep = STEPSIZE((tp->xExt / 2) / (SPEED(40,50,60,70,80)));
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(2,5,10,14,20);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepSplitVerticalIn( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int xLeft = tp->nIteration * tp->vStep;
	int xRight = tp->xExt - (tp->nIteration * tp->vStep);

	if (xLeft <= ((tp->xExt / 2) + tp->vStep) ||
		xRight >= ((tp->xExt / 2) - tp->vStep))
	{
		TransDIBlt(tp, tp->xDst + xLeft, tp->yDst, tp->vStep, tp->yExt, tp->xSrc + xLeft, tp->ySrc);

		TransDIBlt(tp, tp->xDst + xRight, tp->yDst, tp->vStep, tp->yExt, tp->xSrc + xRight, tp->ySrc);
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupSplitVerticalOut( LPTRANS tp )
/***********************************************************************/
{
	tp->vStep = STEPSIZE((tp->xExt / 2) / (SPEED(40,50,60,70,80)));
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(2,5,10,14,20);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepSplitVerticalOut( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int xLeft = (tp->xExt / 2) - tp->nIteration * tp->vStep;
	int xRight = (tp->xExt / 2) + tp->nIteration * tp->vStep;

	if (xLeft >= -(tp->vStep) || xRight <= (tp->xExt + tp->vStep))
	{
		TransDIBlt(tp, tp->xDst + xLeft, tp->yDst, tp->vStep, tp->yExt, tp->xSrc + xLeft, tp->ySrc);

		TransDIBlt(tp, tp->xDst + xRight, tp->yDst, tp->vStep, tp->yExt, tp->xSrc + xRight, tp->ySrc);
	}
	else
		bDone = TRUE;

	return(bDone);
}

/***********************************************************************/
static BOOL setupUncoverDown( LPTRANS tp )
/***********************************************************************/
{
	tp->vStep = STEPSIZE(tp->yExt / SPEED(20,28,40,52,64));
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);
	tp->dwTotalIterations = (tp->yExt + tp->vStep - 1) / tp->vStep;
	return(TRUE);
}

/***********************************************************************/
static BOOL stepUncoverDown( LPTRANS tp )
/***********************************************************************/
{
	int yHeight = tp->nIteration * tp->vStep;
	BOOL bDone	= FALSE;

	if (yHeight > tp->yExt)
	{
		yHeight = tp->yExt;
		bDone	= TRUE;
	}

	TransDCBlt(tp, tp->xDst, tp->yDst + yHeight, tp->xExt, tp->yExt - yHeight,
		tp->xDst, tp->yDst + yHeight - tp->vStep, SRCCOPY);

	TransDIBlt(tp, tp->xDst, tp->yDst, tp->xExt, yHeight, tp->xSrc, tp->ySrc);

	return(bDone);
}

/***********************************************************************/
static BOOL setupUncoverLeft( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->xExt / SPEED(20,28,40,52,64));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);
	tp->dwTotalIterations = (tp->xExt + tp->hStep - 1) / tp->hStep;
	return(TRUE);
}

/***********************************************************************/
static BOOL stepUncoverLeft( LPTRANS tp )
/***********************************************************************/
{
	int xWidth = tp->nIteration * tp->hStep;
	BOOL bDone = FALSE;

	if (xWidth > tp->xExt)
	{
		xWidth = tp->xExt;
		bDone = TRUE;
	}

	TransDCBlt(tp, tp->xDst, tp->yDst, tp->xExt - xWidth, tp->yExt,
		tp->xDst + tp->hStep, tp->yDst, SRCCOPY);

	TransDIBlt(tp, tp->xDst + (tp->xExt - xWidth), tp->yDst, xWidth, tp->yExt,
		tp->xSrc + (tp->xExt - xWidth), tp->ySrc);

	return(bDone);
}

/***********************************************************************/
static BOOL setupUncoverRight( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->xExt / SPEED(20,28,40,52,64));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);
	tp->dwTotalIterations = (tp->xExt + tp->hStep - 1) / tp->hStep;
	return(TRUE);
}

/***********************************************************************/
static BOOL stepUncoverRight( LPTRANS tp )
/***********************************************************************/
{
	int xWidth = tp->nIteration * tp->hStep;
	BOOL bDone = FALSE;

	if (xWidth > tp->xExt)
	{
		xWidth = tp->xExt;
		bDone = TRUE;
	}

	TransDCBlt(tp, tp->xDst + xWidth, tp->yDst, tp->xExt - xWidth, tp->yExt,
		tp->xDst + xWidth - tp->hStep, tp->yDst, SRCCOPY);

	TransDIBlt(tp, tp->xDst, tp->yDst, xWidth, tp->yExt, tp->xSrc, tp->ySrc);

	return(bDone);
}

/***********************************************************************/
static BOOL setupUncoverUp( LPTRANS tp )
/***********************************************************************/
{
	tp->vStep = STEPSIZE(tp->yExt / SPEED(16,24,36,48,60));
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);
	tp->dwTotalIterations = (tp->yExt + tp->vStep - 1) / tp->vStep;
	return(TRUE);
}

/***********************************************************************/
static BOOL stepUncoverUp( LPTRANS tp )
/***********************************************************************/
{
	int yHeight = tp->nIteration * tp->vStep;
	BOOL bDone	= FALSE;

	if (yHeight > tp->yExt)
	{
		yHeight = tp->yExt;
		bDone	= TRUE;
	}

	TransDCBlt(tp, tp->xDst, tp->yDst, tp->xExt, tp->yExt - yHeight,
		tp->xDst, tp->yDst + tp->vStep, SRCCOPY);

	TransDIBlt(tp, tp->xDst, tp->yDst + (tp->yExt - yHeight), tp->xExt, yHeight,
		tp->xSrc, tp->ySrc + (tp->yExt - yHeight));

	return(bDone);
}

/***********************************************************************/
static BOOL setupUncoverLeftDown( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->xExt / SPEED(20,28,40,52,64));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);
	tp->xBox	= 0;
	tp->yBox	= 0;

	return(TRUE);
}

/***********************************************************************/
void CALLBACK doUncoverLeftDown
/***********************************************************************/
(
int x,
int y,
LPTRANS lpTrans
)
{
	lpTrans->vStep -= 1;

	if (lpTrans->vStep == 0)
	{
		lpTrans->xPos = x;
		lpTrans->yPos = y;
	}
}

/***********************************************************************/
static BOOL stepUncoverLeftDown( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->vStep = (tp->nIteration + 1) * tp->hStep;

	LineDDA(0, 0, -(tp->xExt - 1), tp->yExt - 1,
	(LINEDDAPROC)doUncoverLeftDown, (LPARAM)(DWORD)tp);

	if (tp->vStep > 0)
	{
		tp->xPos = -tp->xExt;
		tp->yPos = tp->yExt;
		bDone	= TRUE;
	}

	TransDCBlt(tp, tp->xDst + tp->xPos, tp->yDst + tp->yPos, tp->xExt, tp->yExt,
		tp->xDst + tp->xBox, tp->yDst + tp->yBox);

	TransDIBlt(tp, tp->xDst, tp->yDst, tp->xExt, tp->yPos, tp->xSrc, tp->ySrc);

	TransDIBlt(tp, tp->xDst + tp->xPos + tp->xExt, tp->yDst, -tp->xPos, tp->yExt,
		tp->xSrc + tp->xPos + tp->xExt, tp->ySrc);

	tp->xBox = tp->xPos;
	tp->yBox = tp->yPos;

	return(bDone);
}

/***********************************************************************/
static BOOL setupUncoverLeftUp( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->xExt / SPEED(20,28,40,52,64));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);
	tp->xBox	= 0;
	tp->yBox	= 0;

	return(TRUE);
}

/***********************************************************************/
void CALLBACK doUncoverLeftUp
/***********************************************************************/
(
int x,
int y,
LPTRANS lpTrans
)
{
	lpTrans->vStep -= 1;

	if (lpTrans->vStep == 0)
	{
		lpTrans->xPos = x;
		lpTrans->yPos = y;
	}
}

/***********************************************************************/
static BOOL stepUncoverLeftUp( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->vStep = (tp->nIteration + 1) * tp->hStep;

	LineDDA(0, 0, -(tp->xExt - 1), -(tp->yExt - 1),
	(LINEDDAPROC)doUncoverLeftDown, (LPARAM)(DWORD)tp);

	if (tp->vStep > 0)
	{
		tp->xPos = -tp->xExt;
		tp->yPos = -tp->yExt;
		bDone	= TRUE;
	}

	TransDCBlt(tp, tp->xDst + tp->xPos, tp->yDst + tp->yPos, tp->xExt, tp->yExt,
		tp->xDst + tp->xBox, tp->yDst + tp->yBox);

	TransDIBlt(tp, tp->xDst, tp->yDst + (tp->yExt + tp->yPos), tp->xExt, -tp->yPos,
		tp->xSrc, tp->ySrc + (tp->yExt + tp->yPos));

	TransDIBlt(tp, tp->xDst + tp->xPos + tp->xExt, tp->yDst, -tp->xPos, tp->yExt,
		tp->xSrc + tp->xPos + tp->xExt, tp->ySrc);

	tp->xBox = tp->xPos;
	tp->yBox = tp->yPos;

	return(bDone);
}

/***********************************************************************/
static BOOL setupUncoverRightDown( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->xExt / SPEED(20,28,40,52,64));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);
	tp->xBox	= 0;
	tp->yBox	= 0;

	return(TRUE);
}

/***********************************************************************/
void CALLBACK doUncoverRightDown
/***********************************************************************/
(
int x,
int y,
LPTRANS lpTrans
)
{
	lpTrans->vStep -= 1;

	if (lpTrans->vStep == 0)
	{
		lpTrans->xPos = x;
		lpTrans->yPos = y;
	}
}

/***********************************************************************/
static BOOL stepUncoverRightDown( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->vStep = (tp->nIteration + 1) * tp->hStep;

	LineDDA(0, 0, tp->xExt - 1, tp->yExt - 1,
	(LINEDDAPROC)doUncoverLeftDown, (LPARAM)(DWORD)tp);

	if (tp->vStep > 0)
	{
		tp->xPos = tp->xExt;
		tp->yPos = tp->yExt;
		bDone	= TRUE;
	}

	TransDCBlt(tp, tp->xDst + tp->xPos, tp->yDst + tp->yPos, tp->xExt, tp->yExt,
		tp->xDst + tp->xBox, tp->yDst + tp->yBox);
	TransDIBlt(tp, tp->xDst, tp->yDst, tp->xExt, tp->yPos, tp->xSrc, tp->ySrc);
	TransDIBlt(tp, tp->xDst, tp->yDst, tp->xPos, tp->yExt, tp->xSrc, tp->ySrc);

	tp->xBox = tp->xPos;
	tp->yBox = tp->yPos;

	return(bDone);
}

/***********************************************************************/
static BOOL setupUncoverRightUp( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->xExt / SPEED(20,28,40,52,64));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);
	tp->xBox	= 0;
	tp->yBox	= 0;

	return(TRUE);
}

/***********************************************************************/
void CALLBACK doUncoverRightUp
/***********************************************************************/
(
int x,
int y,
LPTRANS lpTrans
)
{
	lpTrans->vStep -= 1;

	if (lpTrans->vStep == 0)
	{
		lpTrans->xPos = x;
		lpTrans->yPos = y;
	}
}

/***********************************************************************/
static BOOL stepUncoverRightUp( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->vStep = (tp->nIteration + 1) * tp->hStep;

	LineDDA(0, 0, tp->xExt - 1, -(tp->yExt - 1),
	(LINEDDAPROC)doUncoverLeftDown, (LPARAM)(DWORD)tp);

	if (tp->vStep > 0)
	{
		tp->xPos = tp->xExt;
		tp->yPos = -tp->yExt;
		bDone	= TRUE;
	}

	TransDCBlt(tp, tp->xDst + tp->xPos, tp->yDst + tp->yPos, tp->xExt, tp->yExt,
		tp->xDst + tp->xBox, tp->yDst + tp->yBox);

	TransDIBlt(tp, tp->xDst, tp->yDst + (tp->yExt + tp->yPos), tp->xExt, -tp->yPos,
		tp->xSrc, tp->ySrc + (tp->yExt + tp->yPos));

	TransDIBlt(tp, tp->xDst, tp->yDst, tp->xPos, tp->yExt, tp->xSrc, tp->ySrc);

	tp->xBox = tp->xPos;
	tp->yBox = tp->yPos;

	return(bDone);
}

/***********************************************************************/
static BOOL setupWaterfall( LPTRANS tp )
/***********************************************************************/
{
	int nStep = STEPSIZE(16);
	tp->vStep = 1;
	tp->nTicks = SPEED(0,2,3,4,6);
	tp->yBox	= tp->yExt / nStep;
	tp->xBox	= 0;
	tp->yBox	= max(nStep, tp->yBox);
	tp->yPos	= 0;
	tp->yMin	= 0;
	tp->yMax	= tp->yBox;

	return(TRUE);
}

/***********************************************************************/
static BOOL stepWaterfall( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	if (tp->yPos < tp->yMax)
	{
		int yOff = tp->yPos - tp->xBox;

		TransDIBlt(tp, tp->xDst, tp->yDst + yOff, tp->xExt, tp->vStep, tp->xSrc, tp->ySrc + yOff);

		if ((tp->yMax - tp->yBox) >= tp->yExt)
			if (yOff == tp->yMax - tp->yBox - 1)
				bDone = TRUE;

		tp->yPos += tp->yBox;
		tp->xBox += tp->vStep;
	}
	else
	{
		tp->yMin += tp->vStep;
		tp->yPos = tp->yMin;
		tp->xBox = 0;

		if ((tp->yMax - tp->yBox) < tp->yExt)
			tp->yMax += tp->yBox;
	}

	return(bDone);
}


#define MAX_WEAVES 12

/***********************************************************************/
static BOOL setupWeaveHorizontal( LPTRANS tp )
/***********************************************************************/
{
	tp->yBox	= (tp->yExt + (MAX_WEAVES - 1)) / MAX_WEAVES;
	tp->yBox	= max(tp->yBox, 8);
	tp->hStep = STEPSIZE(tp->xExt / SPEED(10,16,28,36,44));
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,2,15,20,20);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepWeaveHorizontal( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;
	int xWidth = (tp->nIteration + 1) * tp->hStep;

	if (xWidth > tp->xExt)
	{
		xWidth = tp->xExt;
		bDone = TRUE;
	}

	for (int Weave = 0; (Weave <= MAX_WEAVES); Weave += 1)
	{
		TransDIBlt(tp, tp->xDst, tp->yDst + (Weave * tp->yBox), xWidth, tp->yBox,
			tp->xSrc + (tp->xExt - xWidth), tp->ySrc + (Weave * tp->yBox));

		++Weave;

		TransDIBlt(tp, tp->xDst + (tp->xExt - xWidth), tp->yDst + (Weave * tp->yBox),
			xWidth, tp->yBox, tp->xSrc, tp->ySrc + (Weave * tp->yBox));
	}

	return(bDone);
}

/***********************************************************************/
static BOOL setupWeaveVertical( LPTRANS tp )
/***********************************************************************/
{
	tp->xBox	= (tp->xExt + (MAX_WEAVES - 1)) / MAX_WEAVES;
	tp->xBox	= max(tp->xBox, 8);
	tp->vStep = STEPSIZE(tp->yExt / SPEED(10,16,28,36,44));
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(1,2,15,20,20);

	return(TRUE);
}

/***********************************************************************/
static BOOL stepWeaveVertical( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone	= FALSE;
	int yHeight = (tp->nIteration + 1) * tp->vStep;

	if (yHeight > tp->yExt)
	{
		yHeight = tp->yExt;
		bDone	= TRUE;
	}

	for (int Weave = 0; (Weave <= MAX_WEAVES); Weave += 1)
	{
		TransDIBlt(tp, tp->xDst + (Weave * tp->xBox), tp->yDst, tp->xBox, yHeight,
			tp->xSrc + (Weave * tp->xBox), tp->ySrc + (tp->yExt - yHeight));

		++Weave;

		TransDIBlt(tp, tp->xDst + (Weave * tp->xBox), tp->yDst + (tp->yExt - yHeight),
			tp->xBox, yHeight, tp->xSrc + (Weave * tp->xBox), tp->ySrc);
	}

	return(bDone);
}

/***********************************************************************/
static BOOL setupWipeDown( LPTRANS tp )
/***********************************************************************/
{
	tp->vStep = STEPSIZE(tp->yExt / 100);
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(1,5,10,20,30);
	tp->dwTotalIterations = (tp->yExt + tp->vStep - 1) / tp->vStep;
	return(TRUE);
}

/***********************************************************************/
static BOOL stepWipeDown( LPTRANS tp )
/***********************************************************************/
{
	int y	= tp->nIteration * tp->vStep;
	BOOL bDone = (y >= tp->yExt);

	if (!bDone)
		TransDIBlt(tp, tp->xDst, tp->yDst + y, tp->xExt, tp->vStep, tp->xSrc, tp->ySrc + y);

	return(bDone);
}

/***********************************************************************/
static BOOL setupWipeLeft( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->xExt / 80);
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,5,15,30,40);
	tp->dwTotalIterations = (tp->xExt + tp->hStep - 1) / tp->hStep;
	return(TRUE);
}

/***********************************************************************/
static BOOL stepWipeLeft( LPTRANS tp )
/***********************************************************************/
{
	int x	= tp->xExt - (tp->nIteration * tp->hStep);
	BOOL bDone = (x < -(tp->hStep));

	if (!bDone)
	{
		TransDIBlt(tp, tp->xDst + x, tp->yDst, tp->hStep, tp->yExt, tp->xSrc + x, tp->ySrc);
	}

	return(bDone);
}

/***********************************************************************/
static BOOL setupWipeRight( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->xExt / 80);
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,5,15,30,40);
	tp->dwTotalIterations = (tp->xExt + tp->hStep - 1) / tp->hStep;
	return(TRUE);
}

/***********************************************************************/
static BOOL stepWipeRight( LPTRANS tp )
/***********************************************************************/
{
	int x	= tp->nIteration * tp->hStep;
	BOOL bDone = (x > (tp->xExt + tp->hStep));

	if (!bDone)
	{
		TransDIBlt(tp, tp->xDst + x, tp->yDst, tp->hStep, tp->yExt, tp->xSrc + x, tp->ySrc);
	}

	return(bDone);
}

/***********************************************************************/
static BOOL setupWipeUp( LPTRANS tp )
/***********************************************************************/
{
	tp->vStep = STEPSIZE(tp->yExt / 100);
	tp->vStep = max(tp->vStep, 1);
	tp->nTicks = SPEED(1,4,10,16,24);
	tp->dwTotalIterations = (tp->yExt + tp->vStep - 1) / tp->vStep;
	return(TRUE);
}

/***********************************************************************/
static BOOL stepWipeUp( LPTRANS tp )
/***********************************************************************/
{
	int y	= tp->yExt - (tp->nIteration * tp->vStep);
	BOOL bDone = (y < -(tp->vStep));

	if (!bDone)
	{
		TransDIBlt(tp, tp->xDst, tp->yDst + y, tp->xExt, tp->vStep, tp->xSrc, tp->ySrc + y);
	}

	return(bDone);
}

/***********************************************************************/
static BOOL setupWipeLeftDown( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->xExt / 80);
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,5,15,35,50);

	return(TRUE);
}

/***********************************************************************/
void CALLBACK doWipeLeftDown
/***********************************************************************/
(
int x,
int y,
LPTRANS lpTrans
)
{
	lpTrans->vStep -= 1;

	if (lpTrans->vStep == 0)
	{
		lpTrans->xPos = x;
		lpTrans->yPos = y;
	}
}

/***********************************************************************/
static BOOL stepWipeLeftDown( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->vStep = (tp->nIteration + 1) * tp->hStep;

	LineDDA(tp->xExt - 1, 0, 0, tp->yExt - 1,
	(LINEDDAPROC)doWipeLeftDown, (LPARAM)(DWORD)tp);

	if (tp->vStep > 0)
	{
		tp->xPos = 0;
		tp->yPos = tp->yExt;
		bDone	= TRUE;
		tp->hStep *= 2;
	}

	TransDIBlt(tp, tp->xDst + tp->xPos, tp->yDst, tp->hStep, tp->yPos, tp->xSrc + tp->xPos, tp->ySrc);

	TransDIBlt(tp, tp->xDst + tp->xPos, tp->yDst + tp->yPos - tp->hStep, tp->xExt, tp->hStep,
		tp->xSrc + tp->xPos, tp->ySrc + tp->yPos - tp->hStep);

	return(bDone);
}

/***********************************************************************/
static BOOL setupWipeLeftUp( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->xExt / 80);
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,5,15,35,50);

	return(TRUE);
}

/***********************************************************************/
void CALLBACK doWipeLeftUp
/***********************************************************************/
(
int x,
int y,
LPTRANS lpTrans
)
{
	lpTrans->vStep -= 1;

	if (lpTrans->vStep == 0)
	{
		lpTrans->xPos = x;
		lpTrans->yPos = y;
	}
}

/***********************************************************************/
static BOOL stepWipeLeftUp( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->vStep = (tp->nIteration + 1) * tp->hStep;

	LineDDA(tp->xExt - 1, tp->yExt - 1, 0, 0,
	(LINEDDAPROC)doWipeLeftUp, (LPARAM)(DWORD)tp);

	if (tp->vStep > 0)
	{
		tp->xPos = 0;
		tp->yPos = 0;
		bDone	= TRUE;
		tp->hStep *= 2;
	}

	TransDIBlt(tp, tp->xDst + tp->xPos, tp->yDst + tp->yPos, tp->xExt, tp->hStep,
		tp->xSrc + tp->xPos, tp->ySrc + tp->yPos);

	TransDIBlt(tp, tp->xDst + tp->xPos, tp->yDst + tp->yPos, tp->hStep, tp->yExt,
		tp->xSrc + tp->xPos, tp->ySrc + tp->yPos);

	return(bDone);
}

/***********************************************************************/
static BOOL setupWipeRightDown( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->xExt / 80);
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,5,15,35,50);

	return(TRUE);
}

/***********************************************************************/
void CALLBACK doWipeRightDown
/***********************************************************************/
(
int x,
int y,
LPTRANS lpTrans
)
{
	lpTrans->vStep -= 1;

	if (lpTrans->vStep == 0)
	{
		lpTrans->xPos = x;
		lpTrans->yPos = y;
	}
}

/***********************************************************************/
static BOOL stepWipeRightDown( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->vStep = (tp->nIteration + 1) * tp->hStep;

	LineDDA(0, 0, tp->xExt - 1, tp->yExt - 1,
	(LINEDDAPROC)doWipeRightDown, (LPARAM)(DWORD)tp);

	if (tp->vStep > 0)
	{
		tp->xPos = tp->xExt;
		tp->yPos = tp->yExt;
		bDone	= TRUE;
		tp->hStep *= 2;
	}

	TransDIBlt(tp, tp->xDst, tp->yDst + tp->yPos - tp->hStep, tp->xPos, tp->hStep,
		tp->xSrc, tp->ySrc + tp->yPos - tp->hStep);

	TransDIBlt(tp, tp->xDst + tp->xPos - tp->hStep, tp->yDst, tp->hStep, tp->yPos,
		tp->xSrc + tp->xPos - tp->hStep, tp->ySrc);

	return(bDone);
}

/***********************************************************************/
static BOOL setupWipeRightUp( LPTRANS tp )
/***********************************************************************/
{
	tp->hStep = STEPSIZE(tp->xExt / 80);
	tp->hStep = max(tp->hStep, 1);
	tp->nTicks = SPEED(1,5,15,35,50);

	return(TRUE);
}

/***********************************************************************/
void CALLBACK doWipeRightUp
/***********************************************************************/
(
int x,
int y,
LPTRANS lpTrans
)
{
	lpTrans->vStep -= 1;

	if (lpTrans->vStep == 0)
	{
		lpTrans->xPos = x;
		lpTrans->yPos = y;
	}
}

/***********************************************************************/
static BOOL stepWipeRightUp( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone = FALSE;

	tp->vStep = (tp->nIteration + 1) * tp->hStep;

	LineDDA(0, tp->yExt - 1, tp->xExt - 1, 0,
	(LINEDDAPROC)doWipeRightUp, (LPARAM)(DWORD)tp);

	if (tp->vStep > 0)
	{
		tp->xPos = tp->xExt;
		tp->yPos = 0;
		bDone	= TRUE;
		tp->hStep *= 2;
	}

	TransDIBlt(tp, tp->xDst, tp->yDst + tp->yPos, tp->xPos, tp->hStep, tp->xSrc, tp->ySrc + tp->yPos);

	TransDIBlt(tp, tp->xDst + tp->xPos - tp->hStep, tp->yDst + tp->yPos, tp->hStep,
		tp->yExt - tp->yPos, tp->xSrc + tp->xPos - tp->hStep, tp->ySrc + tp->yPos);

	return(bDone);
}

/***********************************************************************/
static void TransitionCleanup( LPTRANS lpTrans )
/***********************************************************************/
{
	if (lpTrans->lpData && lpTrans->hData)
		GlobalUnlock(lpTrans->hData);
	if (lpTrans->hData)
		GlobalFree(lpTrans->hData);
	lpTrans->hData = NULL;
	lpTrans->lpData = NULL;
}

/***********************************************************************/
static BOOL TransitionDoStep( LPTRANS tp )
/***********************************************************************/
{ // Do one step of the transition.
	BOOL bDone = TRUE;

	switch (Effect(tp->Transition))
	{
		case TRANS_REPLACE:
			TransDIBlt(tp, tp->xDst, tp->yDst, tp->xExt, tp->yExt, tp->xSrc, tp->ySrc, SRCCOPY);
			break;

		case TRANS_BLINDS_HORIZONTAL:
			bDone = stepBlindsHorizontal(tp);
			break;

		case TRANS_BLINDS_VERTICAL:
			bDone = stepBlindsVertical(tp);
			break;

		case TRANS_BOX_IN:
			bDone = stepBoxIn(tp);
			break;

		case TRANS_BOX_OUT:
			bDone = stepBoxOut(tp);
			break;

		case TRANS_CHECKERBOARD_LEFT:
			bDone = stepCheckerboardLeft(tp);
			break;

		case TRANS_CHECKERBOARD_RIGHT:
			bDone = stepCheckerboardRight(tp);
			break;

		case TRANS_CHECKERBOARD_UP:
			bDone = stepCheckerboardUp(tp);
			break;

		case TRANS_CHECKERBOARD_DOWN:
			bDone = stepCheckerboardDown(tp);
			break;

		case TRANS_COVER_LEFT:
			bDone = stepCoverLeft(tp);
			break;

		case TRANS_COVER_RIGHT:
			bDone = stepCoverRight(tp);
			break;

		case TRANS_COVER_UP:
			bDone = stepCoverUp(tp);
			break;
	
		case TRANS_COVER_DOWN:
			bDone = stepCoverDown(tp);
			break;
	
		case TRANS_COVER_LEFTDOWN:
			bDone = stepCoverLeftDown(tp);
			break;
	
		case TRANS_COVER_LEFTUP:
			bDone = stepCoverLeftUp(tp);
			break;
	
		case TRANS_COVER_RIGHTDOWN:
			bDone = stepCoverRightDown(tp);
			break;

		case TRANS_COVER_RIGHTUP:
			bDone = stepCoverRightUp(tp);
			break;

		case TRANS_DISSOLVE:
			bDone = stepDissolve(tp);
			break;
	
		case TRANS_FADETHROUGHBLACK:
			bDone = stepFadeThroughBlack(tp);
			break;
	
		case TRANS_STRETCH_HORIZONTAL_OUT:
			bDone = stepStretchHorizontalOut(tp);
			break;
	
		case TRANS_STRETCH_VERTICAL_OUT:
			bDone = stepStretchVerticalOut(tp);
			break;
	
		case TRANS_STRETCH_LEFT:
			bDone = stepStretchLeft(tp);
			break;
	
		case TRANS_STRETCH_RIGHT:
			bDone = stepStretchRight(tp);
			break;
	
		case TRANS_STRETCH_UP:
			bDone = stepStretchUp(tp);
			break;
	
		case TRANS_STRETCH_DOWN:
			bDone = stepStretchDown(tp);
			break;
	
		case TRANS_RAIN_LEFT:
			bDone = stepRainLeft(tp);
			break;
	
		case TRANS_RAIN_RIGHT:
			bDone = stepRainRight(tp);
			break;
	
		case TRANS_RAIN_UP:
			bDone = stepRainUp(tp);
			break;
	
		case TRANS_RAIN_DOWN:
			bDone = stepRainDown(tp);
			break;
	
		case TRANS_RANDOMBARS_VERTICAL:
			bDone = stepRandomBarsVertical(tp);
			break;
	
		case TRANS_RANDOMBARS_HORIZONTAL:
			bDone = stepRandomBarsHorizontal(tp);
			break;
	
		case TRANS_ROTATE_VERTICAL:
			bDone = stepRotateVertical(tp);
			break;
	
		case TRANS_ROTATE_HORIZONTAL:
			bDone = stepRotateHorizontal(tp);
			break;

		case TRANS_SPIRAL_CW_OUT:
			bDone = stepSpiralClockwiseOut(tp);
			break;
	
		case TRANS_SPIRAL_CCW_IN:
			bDone = stepSpiralCounterclockwiseIn(tp);
			break;
	
		case TRANS_SPIRAL_CCW_OUT:
			bDone = stepSpiralCounterclockwiseOut(tp);
			break;
	
		case TRANS_SPIRAL_CW_IN:
			bDone = stepSpiralClockwiseIn(tp);
			break;
	
		case TRANS_SPLIT_HORIZONTAL_IN:
			bDone = stepSplitHorizontalIn(tp);
			break;
	
		case TRANS_SPLIT_VERTICAL_IN:
			bDone = stepSplitVerticalIn(tp);
			break;
	
		case TRANS_SPLIT_VERTICAL_OUT:
			bDone = stepSplitVerticalOut(tp);
			break;

		case TRANS_SPLIT_HORIZONTAL_OUT:
			bDone = stepSplitHorizontalOut(tp);
			break;
	
		case TRANS_UNCOVER_LEFT:
			bDone = stepUncoverLeft(tp);
			break;
	
		case TRANS_UNCOVER_RIGHT:
			bDone = stepUncoverRight(tp);
			break;
	
		case TRANS_UNCOVER_UP:
			bDone = stepUncoverUp(tp);
			break;
	
		case TRANS_UNCOVER_DOWN:
			bDone = stepUncoverDown(tp);
			break;
	
		case TRANS_UNCOVER_LEFTDOWN:
			bDone = stepUncoverLeftDown(tp);
			break;
	
		case TRANS_UNCOVER_LEFTUP:
			bDone = stepUncoverLeftUp(tp);
			break;
	
		case TRANS_UNCOVER_RIGHTDOWN:
			bDone = stepUncoverRightDown(tp);
			break;
	
		case TRANS_UNCOVER_RIGHTUP:
			bDone = stepUncoverRightUp(tp);
			break;
	
		case TRANS_WATERFALL:
			bDone = stepWaterfall(tp);
			break;
	
		case TRANS_WEAVE_VERTICAL:
			bDone = stepWeaveVertical(tp);
			break;
	
		case TRANS_WEAVE_HORIZONTAL:
			bDone = stepWeaveHorizontal(tp);
			break;
	
		case TRANS_WIPE_LEFT:
			bDone = stepWipeLeft(tp);
			break;
	
		case TRANS_WIPE_RIGHT:
			bDone = stepWipeRight(tp);
			break;
	
		case TRANS_WIPE_UP:
			bDone = stepWipeUp(tp);
			break;
	
		case TRANS_WIPE_DOWN:
			bDone = stepWipeDown(tp);
			break;
	
		case TRANS_WIPE_LEFTDOWN:
			bDone = stepWipeLeftDown(tp);
			break;

		case TRANS_WIPE_LEFTUP:
			bDone = stepWipeLeftUp(tp);
			break;
	
		case TRANS_WIPE_RIGHTDOWN:
			bDone = stepWipeRightDown(tp);
			break;
	
		case TRANS_WIPE_RIGHTUP:
			bDone = stepWipeRightUp(tp);
			break;
	
		case TRANS_XRAY:
			bDone = stepXRay(tp);
			break;
	}

	if (tp->lpCallback)
		(*tp->lpCallback)(tp->dwIteration, tp->dwTotalIterations, tp->dwCallbackData);

	if ( !bDone )
	{
		tp->nIteration += 1;
		tp->dwIteration += 1;
		tp->dwStepTime += tp->dwTimePerStep;
	}

	return(bDone);
}

/***********************************************************************/
static BOOL TransitionDoSetup( LPTRANS tp )
/***********************************************************************/
{
	BOOL bDone;

	tp->nIteration = 0;
	tp->dwIteration = 0;

	switch (Effect(tp->Transition))
	{
		case TRANS_REPLACE:
			TransDIBlt(tp, tp->xDst, tp->yDst, tp->xExt, tp->yExt, tp->xSrc, tp->ySrc, SRCCOPY);
			break;

		case TRANS_BLINDS_HORIZONTAL:
			bDone = setupBlindsHorizontal(tp);
			break;

		case TRANS_BLINDS_VERTICAL:
			bDone = setupBlindsVertical(tp);
			break;

		case TRANS_BOX_IN:
			bDone = setupBoxIn(tp);
			break;

		case TRANS_BOX_OUT:
			bDone = setupBoxOut(tp);
			break;

		case TRANS_CHECKERBOARD_LEFT:
			bDone = setupCheckerboardLeft(tp);
			break;

		case TRANS_CHECKERBOARD_RIGHT:
			bDone = setupCheckerboardRight(tp);
			break;

		case TRANS_CHECKERBOARD_UP:
			bDone = setupCheckerboardUp(tp);
			break;

		case TRANS_CHECKERBOARD_DOWN:
			bDone = setupCheckerboardDown(tp);
			break;

		case TRANS_COVER_LEFT:
			bDone = setupCoverLeft(tp);
			break;

		case TRANS_COVER_RIGHT:
			bDone = setupCoverRight(tp);
			break;

		case TRANS_COVER_UP:
			bDone = setupCoverUp(tp);
			break;
	
		case TRANS_COVER_DOWN:
			bDone = setupCoverDown(tp);
			break;
	
		case TRANS_COVER_LEFTDOWN:
			bDone = setupCoverLeftDown(tp);
			break;
	
		case TRANS_COVER_LEFTUP:
			bDone = setupCoverLeftUp(tp);
			break;
	
		case TRANS_COVER_RIGHTDOWN:
			bDone = setupCoverRightDown(tp);
			break;

		case TRANS_COVER_RIGHTUP:
			bDone = setupCoverRightUp(tp);
			break;

		case TRANS_DISSOLVE:
			bDone = setupDissolve(tp);
			break;
	
		case TRANS_FADETHROUGHBLACK:
			bDone = setupFadeThroughBlack(tp);
			break;
	
		case TRANS_STRETCH_HORIZONTAL_OUT:
			bDone = setupStretchHorizontalOut(tp);
			break;
	
		case TRANS_STRETCH_VERTICAL_OUT:
			bDone = setupStretchVerticalOut(tp);
			break;
	
		case TRANS_STRETCH_LEFT:
			bDone = setupStretchLeft(tp);
			break;
	
		case TRANS_STRETCH_RIGHT:
			bDone = setupStretchRight(tp);
			break;
	
		case TRANS_STRETCH_UP:
			bDone = setupStretchUp(tp);
			break;
	
		case TRANS_STRETCH_DOWN:
			bDone = setupStretchDown(tp);
			break;
	
		case TRANS_RAIN_LEFT:
			bDone = setupRainLeft(tp);
			break;
	
		case TRANS_RAIN_RIGHT:
			bDone = setupRainRight(tp);
			break;
	
		case TRANS_RAIN_UP:
			bDone = setupRainUp(tp);
			break;
	
		case TRANS_RAIN_DOWN:
			bDone = setupRainDown(tp);
			break;
	
		case TRANS_RANDOMBARS_VERTICAL:
			bDone = setupRandomBarsVertical(tp);
			break;
	
		case TRANS_RANDOMBARS_HORIZONTAL:
			bDone = setupRandomBarsHorizontal(tp);
			break;
	
		case TRANS_ROTATE_VERTICAL:
			bDone = setupRotateVertical(tp);
			break;
	
		case TRANS_ROTATE_HORIZONTAL:
			bDone = setupRotateHorizontal(tp);
			break;

		case TRANS_SPIRAL_CW_OUT:
			bDone = setupSpiralClockwiseOut(tp);
			break;
	
		case TRANS_SPIRAL_CCW_IN:
			bDone = setupSpiralCounterclockwiseIn(tp);
			break;
	
		case TRANS_SPIRAL_CCW_OUT:
			bDone = setupSpiralCounterclockwiseOut(tp);
			break;
	
		case TRANS_SPIRAL_CW_IN:
			bDone = setupSpiralClockwiseIn(tp);
			break;
	
		case TRANS_SPLIT_HORIZONTAL_IN:
			bDone = setupSplitHorizontalIn(tp);
			break;
	
		case TRANS_SPLIT_VERTICAL_IN:
			bDone = setupSplitVerticalIn(tp);
			break;
	
		case TRANS_SPLIT_VERTICAL_OUT:
			bDone = setupSplitVerticalOut(tp);
			break;

		case TRANS_SPLIT_HORIZONTAL_OUT:
			bDone = setupSplitHorizontalOut(tp);
			break;
	
		case TRANS_UNCOVER_LEFT:
			bDone = setupUncoverLeft(tp);
			break;
	
		case TRANS_UNCOVER_RIGHT:
			bDone = setupUncoverRight(tp);
			break;
	
		case TRANS_UNCOVER_UP:
			bDone = setupUncoverUp(tp);
			break;
	
		case TRANS_UNCOVER_DOWN:
			bDone = setupUncoverDown(tp);
			break;
	
		case TRANS_UNCOVER_LEFTDOWN:
			bDone = setupUncoverLeftDown(tp);
			break;
	
		case TRANS_UNCOVER_LEFTUP:
			bDone = setupUncoverLeftUp(tp);
			break;
	
		case TRANS_UNCOVER_RIGHTDOWN:
			bDone = setupUncoverRightDown(tp);
			break;
	
		case TRANS_UNCOVER_RIGHTUP:
			bDone = setupUncoverRightUp(tp);
			break;
	
		case TRANS_WATERFALL:
			bDone = setupWaterfall(tp);
			break;
	
		case TRANS_WEAVE_VERTICAL:
			bDone = setupWeaveVertical(tp);
			break;
	
		case TRANS_WEAVE_HORIZONTAL:
			bDone = setupWeaveHorizontal(tp);
			break;
	
		case TRANS_WIPE_LEFT:
			bDone = setupWipeLeft(tp);
			break;
	
		case TRANS_WIPE_RIGHT:
			bDone = setupWipeRight(tp);
			break;
	
		case TRANS_WIPE_UP:
			bDone = setupWipeUp(tp);
			break;
	
		case TRANS_WIPE_DOWN:
			bDone = setupWipeDown(tp);
			break;
	
		case TRANS_WIPE_LEFTDOWN:
			bDone = setupWipeLeftDown(tp);
			break;

		case TRANS_WIPE_LEFTUP:
			bDone = setupWipeLeftUp(tp);
			break;
	
		case TRANS_WIPE_RIGHTDOWN:
			bDone = setupWipeRightDown(tp);
			break;
	
		case TRANS_WIPE_RIGHTUP:
			bDone = setupWipeRightUp(tp);
			break;
	
		case TRANS_XRAY:
			bDone = setupXRay(tp);
			break;

		default: // default to replace
			bDone = FALSE;
			break;
	}

	if (!bDone)
	{
		tp->Transition = TRANS_REPLACE;
		bDone = TRUE;
	}

	// get initial start time
	if (!tp->dwTotalIterations) // make sure somebody setup up iterations
		tp->dwTotalTime = 0;
	else
	{
		if (tp->dwTotalTime)
		{
			tp->dwLastTime = timeGetTime();
			tp->dwStepTime = tp->dwLastTime * 100L;
			tp->dwTimePerStep = (tp->dwTotalTime*100L) / tp->dwTotalIterations;
		}
	}

	return(bDone);
}

/***********************************************************************/
static BOOL TransitionSetup( LPTRANSPARMS lpTransParms,	LPTRANS lpTrans )
/***********************************************************************/
{
	RECT rect;

	_fmemset((LPBYTE)lpTrans, 0, sizeof(TRANS));
	lpTrans->iClock = -1;

	if (GetClipBox(lpTransParms->hDstDC, &rect) == ERROR)
		return(FALSE);

	if ( !lpTransParms->srcWidth )
		return( FALSE );

	if ( !lpTransParms->srcHeight )
		return( FALSE );

	if ( !lpTransParms->dstWidth )
	{
		lpTransParms->dstLeft = rect.left;
		lpTransParms->dstWidth = RectWidth(&rect);
	}

	if ( !lpTransParms->dstHeight )
	{
		lpTransParms->dstTop = rect.top;
		lpTransParms->dstHeight = RectHeight(&rect);
	}

	// Handle an implied stretch
	if ((lpTransParms->srcWidth != lpTransParms->dstWidth) ||
		(lpTransParms->srcHeight != lpTransParms->dstHeight))
	{
		lpTransParms->srcWidth = lpTransParms->dstWidth = min(lpTransParms->srcWidth, lpTransParms->dstWidth);
		lpTransParms->srcHeight = lpTransParms->dstHeight = min(lpTransParms->srcHeight, lpTransParms->dstHeight);
	}

	// Initialize the parameter block
	lpTrans->lpSrcDib	= lpTransParms->lpSrcDib;
	lpTrans->dcSrc		= lpTransParms->hSrcDC;
	lpTrans->dcDst		= lpTransParms->hDstDC;
	lpTrans->Transition = MAKETRANSITION(lpTransParms->Transition, TRANS_VERYFAST);
	lpTrans->xDst		= lpTransParms->dstLeft;
	lpTrans->yDst		= lpTransParms->dstTop;
	lpTrans->xSrc		= lpTransParms->srcLeft;
	lpTrans->ySrc		= lpTransParms->srcTop;
	lpTrans->xExt		= lpTransParms->srcWidth;
	lpTrans->yExt		= lpTransParms->srcHeight;
	lpTrans->iClock		= 0;
	lpTrans->nTicksOverride		= lpTransParms->nTicks;
	lpTrans->nStepSizeOverride	= lpTransParms->nStepSize;
	lpTrans->dwTotalTime = lpTransParms->dwTotalTime;
	lpTrans->lpCallback = lpTransParms->lpCallback;
	lpTrans->dwCallbackData = lpTransParms->dwCallbackData;
	return( TransitionDoSetup(lpTrans) );
}

/***********************************************************************/
BOOL TransitionStep( LPTRANS lpTrans )
/***********************************************************************/
{
	DWORD dwTime;

//	if (ESCAPE)
//		return(NO);

	if (lpTrans->dwTotalTime)
	{
		dwTime = timeGetTime() * 100L;
		if (lpTrans->dwStepTime <= dwTime)
		{
			if (TransitionDoStep(lpTrans))
			{ // Done
				return( NO );
			}
		}
	}
	else
	{
		if (lpTrans->iClock == 0)
		{
			if (TransitionDoStep(lpTrans))
			{ // Done
				lpTrans->iClock = -1;
				return( NO );
			}
			else
				lpTrans->iClock = lpTrans->nTicks;
		}

		if (lpTrans->iClock > 0)
		{
			dwTime = timeGetTime();
			if ((dwTime - lpTrans->dwLastTime) >= ((DWORD)lpTrans->iClock))
			{
				lpTrans->dwLastTime = dwTime;
				lpTrans->iClock = 0;
			}
		}
	}

	return( YES );
}

/***********************************************************************/
void TransitionTimer(UINT nTimerID, UINT wMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
/***********************************************************************/
{
	TransitionStep( (LPTRANS)dwUser );
}

static int m_TimerID; // id of scheduler timer

/***********************************************************************/
BOOL BeginTimer( LPTRANS lpTrans )
/***********************************************************************/
{
	if (timeBeginPeriod(CLOCK) == 0)
	{
		m_TimerID = timeSetEvent (CLOCK, 1, (LPTIMECALLBACK)TransitionTimer, (DWORD)(lpTrans),
								TIME_PERIODIC);
	}

	return(m_TimerID != 0);
}

/***********************************************************************/
BOOL KillTimer(void)
/***********************************************************************/
{
	if (m_TimerID)
	{
		timeKillEvent(m_TimerID);
		timeEndPeriod(CLOCK);
	}

	m_TimerID = 0;

	return(TRUE);
}

/***********************************************************************/
void SetTransitionTiming( LPTRANSPARMS lpTransParms, int nTicks, int nStepSize, DWORD dwTotalTime)
/***********************************************************************/
{
	lpTransParms->nTicks = nTicks;
	lpTransParms->nStepSize = nStepSize;
	lpTransParms->dwTotalTime = dwTotalTime;
}

/***********************************************************************/
void SetTransitionCallback( LPTRANSPARMS lpTransParms, TRANSCALLBACK lpCallback, DWORD dwCallbackData)
/***********************************************************************/
{
	lpTransParms->lpCallback = lpCallback;
	lpTransParms->dwCallbackData = dwCallbackData;
}

/***********************************************************************/
void SetTransitionParms( LPTRANSPARMS lpTransParms, UINT Transition, HDC hDstDC,
	int dstLeft, int dstTop, int dstWidth, int dstHeight,
	int srcLeft, int srcTop, int srcWidth, int srcHeight,
	PDIB lpSrcDib, HDC hSrcDC)
/***********************************************************************/
{
	lpTransParms->Transition = Transition;
	lpTransParms->hDstDC = hDstDC;
	lpTransParms->dstLeft = dstLeft;
	lpTransParms->dstTop = dstTop;
	lpTransParms->dstWidth = dstWidth;
	lpTransParms->dstHeight = dstHeight;
	lpTransParms->srcLeft = srcLeft;
	lpTransParms->srcTop = srcTop;
	lpTransParms->srcWidth = srcWidth;
	lpTransParms->srcHeight = srcHeight;
	lpTransParms->lpSrcDib = lpSrcDib;
	lpTransParms->hSrcDC = hSrcDC;
	SetTransitionTiming(lpTransParms);
	SetTransitionCallback(lpTransParms);
}

/***********************************************************************/
void TransitionBlt( LPTRANSPARMS lpTransParms )
/***********************************************************************/
{
	TRANS trans;

	//DWORD dwTime = timeGetTime();
	if ( !TransitionSetup( lpTransParms, &trans ) )
		return;

	while ( TransitionStep( &trans ) )
		; //Delay(10);
	//dwTime = timeGetTime() - dwTime;
	//Print("actual time = %ld requested time = %ld", dwTime, dwTotalTime);
	TransitionCleanup( &trans );
}

/***********************************************************************/
void TransitionBlt( UINT Transition, HDC hDstDC, int iDstLeft, int iDstTop, int iDstWidth, int iDstHeight,
	int iSrcLeft, int iSrcTop, int iSrcWidth, int iSrcHeight, PDIB lpSrcDib, HDC hSrcDC,
	int nTicks, int nStepSize, DWORD dwTotalTime )
/***********************************************************************/
{
	TRANSPARMS parms;

	SetTransitionParms(&parms, Transition, hDstDC,
						iDstLeft, iDstTop, iDstWidth, iDstHeight,
						iSrcLeft, iSrcTop, iSrcWidth, iSrcHeight,
						lpSrcDib, hSrcDC);
	parms.nTicks = nTicks;
	parms.nStepSize = nStepSize;
	parms.dwTotalTime = dwTotalTime;

	TransitionBlt(&parms);
}


