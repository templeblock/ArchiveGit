//
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern HINSTANCE hInstAstral;

#ifndef SHARPSMOOTHPROC
typedef void ( FAR *SHARPSMOOTHPROC )( int, LPTR, LPTR, LPTR, LPTR );
#endif

/************************************************************************/
/* Constants								*/
/************************************************************************/
#define FADE_LEN_MULT 5

typedef struct _brushrect
	{
	WORD	xTile;
	WORD	yTile;
	RECT	rTile;
	RECT	rEdit;
	BOOL	fEmpty;
	} BRUSHRECT, *PBRUSHRECT;

typedef struct _brushpt
	{
	int	x;
	int	y;
	BYTE	Pressure;
	} BRUSHPT, far *LPBRUSHPT;

/************************************************************************/
/* Local Functions							*/
/************************************************************************/
static void NewDoDDABrushStroke(int sx, int sy, int dx, int dy);
static void DoBrushStroke(int sx, int sy, int dx, int dy);
static void SetupOverlap();
static void NewProcessRectList(int x, int y);
static void BltRect(PRECT lpRect, LPTR lpBrushMask);
static void NewBltRect(PRECT lpRect);
static void BltToFrame( int x, int y, int dh, int dv, LPTR lpData, LPRECT lpRect, BOOL fUpdateDisplay);
static void BltToDisplay(LPRECT lpRect);
static void SetupDataProcs(int Depth);
static void SetupBrushProcs(LPMGXBRUSH lpBrush, BOOL fBegin);
static void SetupBrush(LPMGXBRUSH lpBrush);
static void SetBrushOpacity(LPTR lpSrc, LPTR lpDst, WORD Opacity, int iCount);
static void SetupBrushTimer(LPMGXBRUSH lpBrush, BOOL fBegin);
static BOOL AllocBrushBuffers(LPMGXBRUSH lpBrush);
static void FreeUpBrushBuffers( void );
static void SetupFade(LPMGXBRUSH lpBrush);
static void ResetBrush(LPMGXBRUSH lpBrush, BOOL fNoBrushDisplay);
static void SetupSourceColor(LPMGXBRUSH lpBrush);
static void SetupMaskState( void );
static void SetupPressurePen( void );
static BOOL SetupFrames(LPMGXBRUSH lpBrush, BOOL fBegin);
static void GetMouseMoves();
static void ProcessBrushStrokes();
static BOOL SetupOSFrame(BOOL fBegin, LPRECT lpStrokeRect, LPRECT lpMaxRect);
static BOOL AllocOSBrushBuffers(LPMGXBRUSH lpBrush);
static void FreeUpOSBrushBuffers();
static void SetupOSBrushProcs(LPMGXBRUSH lpBrush);
static void DoDDAOSBrushStroke(int dx, int dy);
static void DoOSBrushStroke(int dx, int dy);
static void ProcessOSBrushStroke(int x, int y);
static void	OSMergeMaskLine(LPTR lpDst, int iCount, LPTR lpSrc, int iMaxValue);
static void OSMergeMask(LPFRAME lpMskFrame, LPTR lpSrc, LPRECT lpSrcRect,
					 int SrcWidth, LPRECT lpSectRect, int iMaxValue);
LPTR SolidColorSource(int x, int y, int dx, int dy, int State);
LPTR CopyImageSource(int x, int y, int dx, int dy, int State);
LPTR SharpenImageSource(int x, int y, int dx, int dy, int State);
LPTR SmoothImageSource(int x, int y, int dx, int dy, int State);
LPTR LightenImageSource(int x, int y, int dx, int dy, int State);
LPTR DarkenImageSource(int x, int y, int dx, int dy, int State);
LPTR SmearImageSource(int x, int y, int dx, int dy, int State);
static void CombineData32(LPCMYK, LPRECT, int, LPCMYK, LPRECT, int, LPRECT, WORD);
static void CombineData24(LPRGB, LPRECT, int, LPRGB, LPRECT, int, LPRECT, WORD);
static void CombineData8(LPTR, LPRECT, int, LPTR, LPRECT, int, LPRECT, WORD);
static void Colorize32(LPCMYK, LPCMYK, int, LPCMYK, WORD);
static void Colorize24(LPRGB, LPRGB, int, LPRGB, WORD);
static void Colorize8(LPTR, LPTR, int, LPTR, WORD);
#ifdef C_CODE
static void MapLut(LPTR lpSrc, LPTR lpDst, LPTR lpLut, WORD wCount);
static void MapLutInvert(LPTR lpSrc, LPTR lpDst, LPTR lpLut, WORD wCount);
#else
#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */
extern void MapLut(LPTR lpSrc, LPTR lpDst, LPTR lpLut, WORD wCount);
extern void MapLutInvert(LPTR lpSrc, LPTR lpDst, LPTR lpLut, WORD wCount);
#ifdef __cplusplus
}
#endif	// __cplusplus
#endif	// C_CODE
static void FillInBuffer(LPTR lpBuf, int w, int h, LPRECT lpRect);

/************************************************************************/
/* Static variables							*/
/************************************************************************/
static HWND		hWnd;		// the window containing the image
static LPFRAME lpMskFrame;
static PIXMAP	dstPixmap;
static LPPIXMAP	lpDstPixmap;
static LPPIXMAP	lpOrigDstPixmap;
static PIXMAP_TYPE pmDstType;
static LPPIXMAP	lpSrcPixmap;
static PIXMAP_TYPE pmSrcType;
static LPPIXMAP	lpMskPixmap;
static PIXMAP_TYPE pmMskType;
static PIXMAP	srcPixmap;
static PIXMAP	mskPixmap;
static LPMASK	lpMask;
static int		iBltFrameDepth;
static int		iBltFrameXsize;
static int		iBltFrameYsize;
static int		xDstOffset;
static int		yDstOffset;
static int		xSrcOffset;
static int		ySrcOffset;
static FRMDATATYPE iBltFrameType;
static WORD		Spacing;
static BOOL		fMaskEditing;
static BOOL		fNotAnImage;
static BOOL		fUpdateMarquee;
static BOOL		fUsingMouse;	// do any display functions?
static BOOL		UsingMask;	// Using any mask?
static BOOL		fProcessing;
static BOOL		fUsePressure;	// Pressure sensitive pen present
static BOOL 	fPressureSize; 	// Adjust brush size with pressure
static BOOL		fPressureOpacity; // Adjust opacity with pressure
static BOOL		fFirstStroke;	// First stroke?
static BOOL		fCheckLastStroke; // Check for overlap of last stroke
static BOOL		fHorzStrips;
static BOOL		fRightBottom;	// Direction of brush stroke
static BOOL		fRight;
static BOOL		fBottom;
static BOOL		fDone;		// Done processing rects?
static BOOL		fStrokeProcessed; // has a stroke been processed?
static BOOL		fDoBuffering;
static BOOL		fCreatedUndo;
static BOOL		fNoOverlap;
static BOOL		fSmoothness;
static BOOL		fUpdateDisplay;
static BOOL		fDoingMacro;
static BOOL		fUsingDstLast;
static LFIXED	FadeOut;	// Fade In/Out value
static LFIXED	FadeBrushSize;	// Current brush size for Fade
static LFIXED	FadeOpacity;	// Fade value for opacity
static LFIXED	FadeOpacityValue; // Current opacity for Fade
static int		BrushFull;	// Brush Size
static int		BrushHalf;	// Half Brush Size
static int		bh;		// brush buffer size horizontally
static int		bv;		// brush buffer size vertically
static WORD		bcount;		// brush buffer size in bytes
static int		brush_x;	// Brush x-position
static int		brush_y;	// Brush y-position
static int		sx_delta;	// x-delta between source and dest
static int		sy_delta;	// y-delta between source and dest
static int		start_dx;	// Destination starting x
static int		start_dy;	// Destination starting y
static int		end_dx;		// Destination ending x
static int		end_dy;		// Destination ending y
static int		BrushSize;	// Original Brush Size
static int		Opacity;	// Original Brush Opacity
static int		PenPressure;	// pressure pen pressure
static int		LastOpacity;	// Original Brush Opacity
static int 		MaxOpacity;
static int		StylePressure;	// Original Brush Pressure
static int		State;		// Current State
static LPTR		lpColor;	// Source color buffer pointer
static LPTR		lpSrcData;  // Source data buffer pointer
static LPTR		lpData;		// Source data buffer pointer
static LPTR		lpSrc;		// Brush source buffer pointer
static LPTR		lpDst;		// Destination buffer pointer
static LPTR		lpDstLast;		// Destination buffer pointer
static LPTR		lpMsk;		// Mask buffer pointer
static LPTR		lpBM;		// BrushMask pointer
static LPTR		lpMsk1;		// Primary buffer for building up mask
static LPINT	lpErr;		// Error buffer for scatter style
static void	   ( *lpMaskProc )( int, int, int, int, HPTR, HPTR, LPMASK );	// Proc to load mask data
static void	   ( FAR *lpProcessProc )( LPTR, LPTR, LPTR, int );	// Proc to process image data
static void	   ( FAR *lpHugeProcessProc )( HPTR, HPTR, HPTR, DWORD );	// Proc to process image data
static BOOL    ( *lpGetSourceData )( LPPIXMAP, PIXMAP_TYPE, int, int, int, int,
                                    LPTR, LPRECT, BOOL, BOOL );  // Proc to get source data
static LPTR    ( *lpCreateBrushSource )( int, int, int, int, int );  // Proc to create source data
static LPBRUSHUPDATEPROC lpUpdateProc; // Proc for update display
static LPSRCPOSPROC lpSrcPosProc; // Proc for update display
static void    ( *StrokeProc )( int, int, int, int );	// Proc to handle stroking
static void    ( *OSStrokeProc )( int, int );	      // Proc to handle stroking
static void    ( *StylizeProc )( LPTR, LPTR, LPINT, int, int, int );	// Proc to handle brush style
static RECT		rMax;		// Max area stroke can affect
static RECT		rOffScreen;
static RECT		rUpdate;
static LPMGXBRUSH lpBrush;	// The brush description
static WORD		idTimer;	// timer indentifier
static DWORD	dwLastNoMoveTime; // time last stroke finished
static DWORD	dwMoveTime;
static DWORD	dwLastTime;
static DWORD	dwBrushTimer;
static BRUSHRECT	bRects[4];
static int lr[4] = {0, 1, 3, 2};
static int ur[4] = {3, 0, 2, 1};
static int ul[4] = {2, 1, 3, 0};
static int ll[4] = {1, 0, 2, 3};
static int * pPtTable;

static LPBRUSHPT  lpMoveBuf;
static LPBRUSHPT	lpMoveEnd;
static LPBRUSHPT  lpMoveIn;
static LPBRUSHPT  lpMoveOut;
static BOOL fQuit;
static int last_x_processed;
static int last_y_processed;
static MAP Map;

/************************************************************************/
BOOL SetupBrushBuffers(BOOL fInit)
/************************************************************************/
{
if (fInit)
	{
	if (!SetupBrushMaskBuffers(fInit))
		return(FALSE);
	lpMoveBuf = (LPBRUSHPT)Alloc(4096L * sizeof(BRUSHPT));
	if (!lpMoveBuf)
		{
		SetupBrushMaskBuffers(NO);
		return(FALSE);
		}
	lpMoveEnd = lpMoveBuf + 4096L;
	}
else
	{
	SetupBrushMaskBuffers(fInit);
	if (lpMoveBuf)
		{
		FreeUp((LPTR)lpMoveBuf);
		lpMoveBuf = NULL;
		}
	}
	return TRUE;
}

/************************************************************************/
BOOL MgxBrushBegin(HWND hWndIn, LPPIXMAP lpPixmapIn,
					int xOffset, int yOffset,
					HMGXBRUSH hBrush)

/************************************************************************/
{
return(MgxBrushBeginEx(hWndIn, lpPixmapIn, xOffset, yOffset, hBrush,
					YES, NO, NULL, NULL));
}

/************************************************************************/
BOOL MgxBrushBeginEx(HWND hWndIn, LPPIXMAP lpPixmapIn,
					int xOffset, int yOffset,
					HMGXBRUSH hBrush, BOOL fUsingMouseIn,
					BOOL fNotAnImageSetting,
					LPBRUSHUPDATEPROC lpUpdateProcIn,
					LPSRCPOSPROC lpSrcPosProcIn)
/************************************************************************/
{
// Call End function to make sure things get reset
MgxBrushEnd(NO);

// Convert brush handle to brush pointer for our use
lpBrush = (LPMGXBRUSH)hBrush;

// Save passed in values for later use
hWnd = hWndIn;
lpOrigDstPixmap = lpPixmapIn;
fUsingMouse = fUsingMouseIn;
xDstOffset = xSrcOffset = xOffset;
yDstOffset = ySrcOffset = yOffset;
lpUpdateProc = lpUpdateProcIn;
lpSrcPosProc = lpSrcPosProcIn;
fNotAnImage = fNotAnImageSetting;
fUsingMouse = fUsingMouseIn;
fUpdateDisplay = fUsingMouse && !fNotAnImage;
fDoingMacro = NO;
if ((fDoingMacro || !lpBrush->Smoothness))
	fUsingMouse = NO;
fDoBuffering = NO;

// setup overlap
SetupOverlap();

// Make sure there are no outstanding WM_PAINT's to avoid strange display
if (fNotAnImage)
	UpdateWindow(hWnd);
else
	AstralUpdateWindow(hWnd);

if (fUpdateDisplay)
	{
 	if (!ImgPaintSetup(lpImage, NO))
		{
		lpBrush = NULL;
		return(FALSE);
		}
	AstralSetRectEmpty(&rUpdate);
	}

// Set frame mode to free oldest lines first instead of newest
// Assumption is that your typically brushing in the same area
if (!SetupFrames(lpBrush, YES))
	{
	lpBrush = NULL;
	return(FALSE);
	}

// Setup Fade for brush size and brush opacity
SetupFade(lpBrush);

// Allocate all memory needed for brushing
if (!AllocBrushBuffers(lpBrush))
	{
	lpBrush = NULL;
	return(FALSE);
	}

// Setup the Depth and Brush dependent procs
SetupDataProcs(iBltFrameDepth);
SetupBrushProcs(lpBrush, YES);

// Load up the source color
SetupSourceColor(lpBrush);

// Setup mask state, geometrical mask and color shield
SetupMaskState();

// Initialize pressure sensitive pen settings
SetupPressurePen();

// Setup the brush mask
SetupBrush(lpBrush);

// Setup brush timer
SetupBrushTimer(lpBrush, YES);

// Indicate the first stroke is coming
fFirstStroke = YES;

SetRect(&rMax, 0, 0, iBltFrameXsize-1, iBltFrameYsize-1);
if (fUsingMouse)
	{
	ProcessBrushStrokes();
	MgxBrushEnd(NO);
	}

return(TRUE);
}

/************************************************************************/
void MgxBrushStroke(int sx, int sy, int dx, int dy,
			int curs_x, int curs_y)
/************************************************************************/
{
int i;

if (!lpBrush)
	return;

if (fUpdateDisplay && !fUsingMouse)
	{
	ImgPaintSetup(lpImage, NO);
	brush_x = curs_x;
	brush_y = curs_y;
 	if (fUsePressure)
 		PenPressure = WacPressure();
	if ( Window.hCursor == Window.hNullCursor )
		DisplayBrush(0, 0, 0, OFF);
	}
if (lpSrcPosProc)
	{
	(*lpSrcPosProc)(dx, dy, lpBrush->Size, &sx, &sy);
	}

sx -= xSrcOffset;
sy -= ySrcOffset;
dx -= xDstOffset;
dy -= yDstOffset;
// WHAT ??? 6 parameters??? (*StrokeProc)(sx, sy, dx, dy, curs_x, curs_y);
(*StrokeProc)(sx, sy, dx, dy);

if (!fUsingMouse)
	{
	for (i = 0; i < 4; ++i)
		{
		if (bRects[i].fEmpty)
			continue;
		NewBltRect(&bRects[i].rEdit);
		bRects[i].fEmpty = YES;
		}
	if (fUpdateDisplay && Window.hCursor == Window.hNullCursor )
		DisplayBrush(hWnd, brush_x, brush_y, ON);
	}
}

/************************************************************************/
BOOL MgxBrushActive()
/************************************************************************/
{
return(lpBrush != NULL);
}

/************************************************************************/
void MgxBrushEnd(BOOL fNoBrushDisplay)
/************************************************************************/
{
int i;

if (!lpBrush)
	return;

for (i = 0; i < 4; ++i)
	{
	if (bRects[i].fEmpty)
		continue;
	NewBltRect(&bRects[i].rEdit);
	bRects[i].fEmpty = YES;
	}
ResetBrush(lpBrush, fNoBrushDisplay);
SetupBrushProcs(lpBrush, NO);
SetupFrames(lpBrush, NO);
SetupBrushTimer(lpBrush, NO);
FreeUpBrushBuffers();
lpBrush = NULL;
if (fUpdateDisplay && !AstralIsRectEmpty(&rUpdate))
	UpdateImageEx(lpImage, &rUpdate, YES, NO, YES);
}

/************************************************************************/
static void ProcessBrushStrokes()
/************************************************************************/
{
int fx, fy, i;
DWORD pos, dwNewTime;
POINT pt;

fDoBuffering = YES;
if (lpBrush->Smoothness)
	{
	fSmoothness = YES;
	dwMoveTime = (((100L-(long)lpBrush->Smoothness) * 500) + 50) / 100;
	}
else
	fSmoothness = NO;
dwLastNoMoveTime = 0;
dwLastTime = GetTickCount();
pos = GetMessagePos();
pt.x = LOWORD(pos);
pt.y = HIWORD(pos);
ScreenToClient(hWnd, &pt);
brush_x = pt.x;
brush_y = pt.y;
fx = pt.x;
fy = pt.y;
if (!fNotAnImage)
	Display2File(&fx, &fy);
fProcessing = YES;
MgxBrushStroke(fx, fy, fx, fy, 0, 0);
fProcessing = NO;
fQuit = FALSE;
while (!fQuit)
	{
	GetMouseMoves();
	if (lpMoveOut < lpMoveIn)
		{
		fx = lpMoveOut->x;
		fy = lpMoveOut->y;
		PenPressure = lpMoveOut->Pressure;
		++lpMoveOut;
		fProcessing = YES;
		MgxBrushStroke(fx, fy, fx, fy, 0, 0);
		fProcessing = NO;
		}
	if (!LBUTTON)
		{
		fQuit = TRUE;
		while (lpMoveOut < lpMoveIn)
			{
			GetMouseMoves();
			fx = lpMoveOut->x;
			fy = lpMoveOut->y;
			PenPressure = lpMoveOut->Pressure;
			++lpMoveOut;
			fProcessing = YES;
			MgxBrushStroke(fx, fy, fx, fy, 0, 0);
			fProcessing = NO;
			}
		}
	// if we have caught up to the user:
	// 1. reset buffer pointers
	// 2. output any pending rectangles
	if (lpMoveOut >= lpMoveIn)
		{
		if (lpMoveOut >= lpMoveIn)
			{
			lpMoveIn = lpMoveBuf;
			lpMoveOut = lpMoveBuf;
			}
		fProcessing = YES;
		for (i = 0; i < 4; ++i)
			{
			if (bRects[i].fEmpty)
				continue;
			NewBltRect(&bRects[i].rEdit);
			bRects[i].fEmpty = YES;
			}
		fProcessing = NO;
		if (!fQuit)
			{
			GetMouseMoves();
			if (dwBrushTimer && (lpMoveOut >= lpMoveIn))
				{
				dwNewTime = GetTickCount();
				if ((dwNewTime - dwLastNoMoveTime) >= dwBrushTimer)
					{
					GetCursorPos(&pt);
					ScreenToClient(hWnd, &pt);
					fx = pt.x;
					fy = pt.y;
					if (fUsePressure)
						PenPressure = WacPressure();
					if (!fNotAnImage)
						Display2File(&fx, &fy);
					fFirstStroke = YES;
					fProcessing = YES;
					MgxBrushStroke(fx, fy, fx, fy, 0, 0);
					fProcessing = NO;
					dwLastNoMoveTime = dwNewTime;
					}
				}
			}
		}
	}
fDoBuffering = NO;
}

/************************************************************************/
static void GetMouseMoves()
/************************************************************************/
{
DWORD dwNewTime;
POINT pt, aspt;
RECT rUpdate;

if (!fDoBuffering)
	return;

if (!fSmoothness)
	{
	if (fProcessing)
		return;
//	if (lpMoveOut < lpMoveIn) // strokes still to process?
//		{
//		Print("Tell Ted you got too many strokes message");
//		return;
//		}
	}
else
if (dwMoveTime)
	{
	dwNewTime = GetTickCount();
	if ((dwNewTime - dwLastTime) < dwMoveTime)
		return;
	dwLastTime = dwNewTime;
	}

GetCursorPos(&pt);
ScreenToClient(hWnd, &pt);
if (pt.x != brush_x || pt.y != brush_y)
	{
	brush_x = pt.x;
	brush_y = pt.y;
	if (!fQuit && lpMoveIn < lpMoveEnd)
		{
		aspt = pt;
		if (!fNotAnImage)
			Display2File((LPINT)&pt.x, (LPINT)&pt.y);
		lpMoveIn->x = pt.x;
		lpMoveIn->y = pt.y;
		if (fUsePressure)
			lpMoveIn->Pressure = WacPressure();
		++lpMoveIn;
		if (!fNotAnImage && AutoScroll(hWnd, aspt.x, aspt.y))
			{
			ImgPaintSetup(lpImage, NO);
			if (!fNotAnImage)
				File2Display((LPINT)&pt.x, (LPINT)&pt.y);
			brush_x = pt.x;
			brush_y = pt.y;
			ClientToScreen(hWnd, &pt);
			SetCursorPos(pt.x, pt.y);
			}
		}
	if (lpUpdateProc)
	  	{
	  	rUpdate.left = brush_x;
	  	rUpdate.top = brush_y;
	  	(*lpUpdateProc)(hWnd, &rUpdate, YES);
	  	}
//	else
//	if (fUpdateDisplay)
//		{
//		DisplayBrush(0, 0, 0, OFF);
//		DisplayBrush(hWnd, brush_x, brush_y, ON);
//		}
	}
}

/************************************************************************/
LPMGXBRUSH CopyMgxBrush(HMGXBRUSH hBrush)
/************************************************************************/
{
LPMGXBRUSH lpBrush;

if (!hBrush)
	return(NULL);

// Allocate memory for brush
if (!(lpBrush = (LPMGXBRUSH)Alloc((long)sizeof(MGXBRUSH))))
	{
	Message(IDS_EMEMALLOC);
	return(NULL);
	}
copy((LPTR)hBrush, (LPTR)lpBrush, sizeof(MGXBRUSH));

// Check for pointers which are not allowed to copy
lpBrush->SourceFrame = NULL;

return(lpBrush);
}

/************************************************************************/
HMGXBRUSH CreateDefaultBrush( HMGXBRUSH hBrush )
/************************************************************************/
{
LPMGXBRUSH lpBrush;

if (lpImage)
	DisplayBrush(0, 0, 0, OFF);
if ( hBrush )
	DestroyMgxBrush( hBrush );
// Allocate memory for brush
if (!(lpBrush = (LPMGXBRUSH)Alloc((long)sizeof(MGXBRUSH))))
	{
	Message(IDS_EMEMALLOC);
	return(NULL);
	}
clr((LPTR)lpBrush, sizeof(MGXBRUSH));

lpBrush->SourceColor = SC_ACTIVE_COLOR;
lpBrush->SourceFrame = NULL;
lpBrush->ColorStruct.gray = 0;
lpBrush->ColorStruct.ColorSpace = CS_GRAY;
SetColorInfo(&lpBrush->ColorStruct, &lpBrush->ColorStruct,
			lpBrush->ColorStruct.ColorSpace);
lpBrush->SourceData = SD_NONE;
lpBrush->SourceFlipMode = SFM_NO_FLIP;
lpBrush->SourceFlipHorz = lpBrush->SourceFlipVert = NO;
lpBrush->SourceFunction = SF_SOLID_COLOR;
lpBrush->StrokeMethod = SM_CONNECTED;
lpBrush->Shape = IDC_BRUSHCIRCLE;
lpBrush->ShapeName[0] = '\0';
lpBrush->Style = IDC_BRUSHSOLID;
lpBrush->Size = 20;
lpBrush->Pressure = 0;
lpBrush->Feather = 0;
lpBrush->FadeSize = 0;
lpBrush->FadeOpacity = 0;
lpBrush->Opacity = 255;
lpBrush->OpacityScale = 100;
lpBrush->MergeMode = MM_NORMAL;
lpBrush->Timer = 0;
lpBrush->StylePressure = 50;
lpBrush->ReadOnlyMask = 0;
lpBrush->Weight = 0;
lpBrush->szSourceExtName[0] = '\0';
lpBrush->szSource[0] = '\0';
lpBrush->xSrcOffset = 0;
lpBrush->ySrcOffset = 0;
lpBrush->Spacing = 25;
lpBrush->fMirrorMode = NO;
lpBrush->Smoothness = 0;
lpBrush->NoOverlap = NO;

if (lpImage && Window.hCursor == Window.hNullCursor)
	DisplayBrush(lpImage->hWnd, 32767, 32767, ON);

return( (HMGXBRUSH)lpBrush );
}

/************************************************************************/
void DestroyMgxBrush(HMGXBRUSH hBrush)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (!hBrush)
	return;
lpBrush = (LPMGXBRUSH)hBrush;
if (lpBrush->SourceFrame)
	FrameClose(lpBrush->SourceFrame);
FreeUp((LPTR)hBrush);
}

/************************************************************************/
void SetMgxBrushSource(HMGXBRUSH hBrush, LPFRAME lpFrame,
						int xSrcOffset, int ySrcOffset)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (!(lpBrush->ReadOnlyMask & RO_SOURCE_DATA))
		{
		lpBrush->SourceFrame = lpFrame;
		lpBrush->xSrcOffset = xSrcOffset;
		lpBrush->ySrcOffset = ySrcOffset;
		}
	}
}

/************************************************************************/
int GetMgxBrushSource(HMGXBRUSH hBrush)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	return(lpBrush->SourceData);
	}
return(SD_CURRENT_DEST);
}

/************************************************************************/
void SetMgxBrushSize(HMGXBRUSH hBrush, int BrushSize)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (!(lpBrush->ReadOnlyMask & RO_SIZE))
		lpBrush->Size = BrushSize;
	}
}

/************************************************************************/
int SetMgxBrushStyle(HMGXBRUSH hBrush, int BrushStyle)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int OldStyle = IDC_BRUSHSOLID;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	OldStyle = lpBrush->Style;
	lpBrush->Style = BrushStyle;
	}
return(OldStyle);
}

/************************************************************************/
int SetMgxStylePressure(HMGXBRUSH hBrush, int StylePressure)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int OldPressure = 255;
if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	OldPressure = lpBrush->StylePressure;
	lpBrush->StylePressure = StylePressure;
	}
return(OldPressure);
}

/************************************************************************/
void SetMgxBrushFeather(HMGXBRUSH hBrush, int Feather)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (!(lpBrush->ReadOnlyMask & RO_FEATHER))
		lpBrush->Feather = Feather;
	}
}

/************************************************************************/
BOOL SetMgxBrushShape(HMGXBRUSH hBrush, int BrushShape, LPSTR lpShapeName)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int OldShape;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (!(lpBrush->ReadOnlyMask & RO_SHAPE))
		{
		OldShape = lpBrush->Shape;
		lpBrush->Shape = BrushShape;
		if (BrushShape == IDC_BRUSHCUSTOM)
			{
			if (SetupCustomBrushMask(lpBrush, lpShapeName))
				lstrcpy(lpBrush->ShapeName, lpShapeName);
			else
				{
				lpBrush->Shape = OldShape;
				if (OldShape == IDC_BRUSHCUSTOM)
					SetupCustomBrushMask(lpBrush, lpBrush->ShapeName);
				return(FALSE);
				}
			}
		return(TRUE);
		}
	}
return(FALSE);
}

/************************************************************************/
int SetMgxBrushOpacity(HMGXBRUSH hBrush, int Opacity)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int OldOpacity = 255;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	OldOpacity = lpBrush->Opacity;
	if (!(lpBrush->ReadOnlyMask & RO_OPACITY))
		lpBrush->Opacity = Opacity;
	}
return(OldOpacity);
}

/************************************************************************/
int SetMgxBrushOpacityScale(HMGXBRUSH hBrush, int OpacityScale)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int OldScale = 0;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	OldScale = lpBrush->OpacityScale;
	lpBrush->OpacityScale = OpacityScale;
	}
return(OldScale);
}

/************************************************************************/
void SetMgxBrushColor(HMGXBRUSH hBrush, LPCOLORINFO lpColorStruct)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	lpBrush->SourceColor = SC_RGB_COLOR;
	lpBrush->SourceFunction = SF_SOLID_COLOR;
	lpBrush->ColorStruct = *lpColorStruct;
	SetColorInfo(&lpBrush->ColorStruct, &lpBrush->ColorStruct,
			lpBrush->ColorStruct.ColorSpace);
	}
}

/************************************************************************/
void SetMgxBrushFlipMode(HMGXBRUSH hBrush, int FlipMode, BOOL FlipHorz, BOOL FlipVert)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (!(lpBrush->ReadOnlyMask & RO_FLIPMODE))
		{
		lpBrush->SourceFlipMode = FlipMode;
		lpBrush->SourceFlipHorz = FlipHorz;
		lpBrush->SourceFlipVert = FlipVert;
		}
	}
}

/************************************************************************/
void SetMgxBrushPressure(HMGXBRUSH hBrush, int Pressure)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (!(lpBrush->ReadOnlyMask & RO_PRESSURE))
		lpBrush->Pressure = Pressure;
	}
}

/************************************************************************/
MERGE_MODE SetMgxBrushMergeMode(HMGXBRUSH hBrush, MERGE_MODE MergeMode)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
MERGE_MODE oldMode = MM_NORMAL;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	oldMode = lpBrush->MergeMode;
	lpBrush->MergeMode = MergeMode;
	}
return(oldMode);
}

/************************************************************************/
int SetMgxBrushStrokeMethod(HMGXBRUSH hBrush, int idStrokeMethod)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int idOldMethod = 0;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	idOldMethod = lpBrush->StrokeMethod;
	lpBrush->StrokeMethod = idStrokeMethod;
	}
return(idOldMethod);
}


/************************************************************************/
void SetMgxBrushOverlap(HMGXBRUSH hBrush, BOOL bOverlap)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	lpBrush->NoOverlap = !bOverlap;
	}
}

/************************************************************************/
int GetMgxBrushSize(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Size);
else
	return(0);
}

/************************************************************************/
int GetMgxStylePressure(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->StylePressure);
else
	return(255);
}

/************************************************************************/
int GetMgxBrushStyle(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Style);
else
	return(IDC_BRUSHSOLID);
}


/************************************************************************/
int GetMgxBrushFeather(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Feather);
else
	return(0);
}

/************************************************************************/
int GetMgxBrushShape(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Shape);
else
	return(IDC_BRUSHCIRCLE);
}



/************************************************************************/
int GetMgxBrushOpacity(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Opacity);
return(255);
}

/************************************************************************/
int GetMgxBrushOpacityScale(HMGXBRUSH hBrush)
/************************************************************************/
{

if (hBrush)
	return(((LPMGXBRUSH)hBrush)->OpacityScale);
return(0);
}


/************************************************************************/
int GetMgxBrushFlipMode(HMGXBRUSH hBrush, LPINT FlipHorz, LPINT FlipVert)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	*FlipHorz = lpBrush->SourceFlipHorz;
	*FlipVert = lpBrush->SourceFlipVert;
	return(lpBrush->SourceFlipMode);
	}
return(SFM_NO_FLIP);
}

/************************************************************************/
int GetMgxBrushPressure(HMGXBRUSH hBrush)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	return(lpBrush->Pressure);
	}
return(0);
}

/************************************************************************/
MERGE_MODE GetMgxBrushMergeMode(HMGXBRUSH hBrush, LPINT lpReadOnly)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (lpReadOnly)
		*lpReadOnly = (lpBrush->ReadOnlyMask & RO_MERGEMODE) != 0;
	return(lpBrush->MergeMode);
	}
else
	{
	if (lpReadOnly)
		*lpReadOnly = NO;
	return(MM_NORMAL);
	}
}

/************************************************************************/
void MgxBrushMirror(HMGXBRUSH hBrush, LPRECT lpSrcRect, LPRECT lpDstRect,
			int npix, int nlin)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (!hBrush)
	return;
lpBrush = (LPMGXBRUSH)hBrush;
if (lpBrush->SourceFlipMode != SFM_MIRROR)
	return;
MgxMirrorRect(lpSrcRect, lpDstRect, npix, nlin, lpBrush->SourceFlipHorz,
		lpBrush->SourceFlipVert);
}

/************************************************************************/
void MgxMirrorRect(LPRECT lpSrcRect, LPRECT lpDstRect,
			int npix, int nlin, BOOL FlipHorz, BOOL FlipVert)
/************************************************************************/
{
int top, left;

if (FlipVert)
	{
	top = lpSrcRect->top;
	lpDstRect->top = (nlin-1) - lpSrcRect->bottom;
	lpDstRect->bottom = (nlin-1) - top;
	}
else
	{
	lpDstRect->top = lpSrcRect->top;
	lpDstRect->bottom = lpSrcRect->bottom;
	}
if (FlipHorz)
	{
	left = lpSrcRect->left;
	lpDstRect->left = (npix-1) - lpSrcRect->right;
	lpDstRect->right = (npix-1) - left;
	}
else
	{
	lpDstRect->left = lpSrcRect->left;
	lpDstRect->right = lpSrcRect->right;
	}
}

/************************************************************************/
static void NewDoDDABrushStroke(int sx, int sy, int dx, int dy)
/************************************************************************/
{
int deltax, deltay, i;

// Initialize for first stroke that we get
if (fFirstStroke)
	{
	sx_delta = sx - dx; // setup deltas between source and dest
	sy_delta = sy - dy;
	start_dx = dx;		 // setup initial starting point
	start_dy = dy;
	last_x_processed = 32767;
	last_y_processed = 32767;
	fFirstStroke = FALSE; // no longer first stroke
	fStrokeProcessed = FALSE;
	State |= STATE_FIRSTSTROKE;
	for (i = 0; i < 4; ++i)
		bRects[i].fEmpty = YES;
	}
else if (dx == start_dx && dy == start_dy)
	return; // point has moved - don't do anything

// Set state for a new brush stroke
State |= STATE_NEWSTROKE;

// setup new ending point - start point was the last point
end_dx = dx;
end_dy = dy;

deltax = abs(end_dx-start_dx);
deltay = abs(end_dy-start_dy);

// Save direction of stroke based on type of strips being processed
fRight = start_dx <= end_dx;
fBottom = start_dy <= end_dy;

if (fRight && fBottom)
	pPtTable = lr;
else if (fRight)
	pPtTable = ur;
else if (fBottom)
	pPtTable = ll;
else
	pPtTable = ul;

// Setup pressure pen and fade settings for this stroke
if (fUsePressure)
	PressureSetup(max(deltax, deltay));

if (deltax || deltay)
	{
	if (deltax > 1 || deltay > 1)
		ddaline(start_dx, start_dy, end_dx, end_dy, (LPPOINTFUNC)NewProcessRectList);
	else
		{
		NewProcessRectList(start_dx, start_dy);
		NewProcessRectList(end_dx, end_dy);
		}
	}
else
	NewProcessRectList(start_dx, start_dy);

fStrokeProcessed = TRUE;

// next strokes start becomes this strokes end
start_dx = end_dx;
start_dy = end_dy;
}

/************************************************************************/
static void DoBrushStroke(int sx, int sy, int dx, int dy)
/************************************************************************/
{
RECT rBrush;

if (!BrushFull)
	return;

sx_delta = sx - dx; // setup deltas between source and dest
sy_delta = sy - dy;
rBrush.left = dx - BrushHalf;
rBrush.right = rBrush.left + BrushFull - 1;
rBrush.top = dy - BrushHalf;
rBrush.bottom = rBrush.top + BrushFull - 1;
BltRect(&rBrush, lpBM);
BltToDisplay( &rBrush );
FadeOutBrush(YES);
}

/************************************************************************/
static void NewProcessRectList(int x, int y)
{
RECT bRect, iRect, rSect;
BOOL fOnImage, fAnyEmpty, fAllEmpty;
int i, j, empty, pt, pi;
WORD	xTile, yTile;
POINT pts[4];
PBRUSHRECT pbRect;

if (abs(last_x_processed-x) < Spacing &&
	abs(last_y_processed-y) < Spacing)
	{
	FadeOutBrush(YES);
	return;
	}

last_x_processed = x;
last_y_processed = y;
if (!BrushFull)
	{
	// Adjust fade values
	FadeOutBrush(YES);
	return;
	}

// get rectangle this brush stroke encompasses
bRect.top = y - BrushHalf;
bRect.bottom = bRect.top + BrushFull - 1;
bRect.left = x - BrushHalf;
bRect.right = bRect.left + BrushFull - 1;

// merge brush in with current brush mask
if (fOnImage = AstralIntersectRect(&iRect, &rMax, &bRect))
	{
	if (fNoOverlap)
		OSMergeMask(lpMskFrame, lpBM, &bRect, BrushFull, &iRect, MaxOpacity);
	else
		BltRect(&bRect, lpBM);
	}

// See where this brush intersects and output any rectangles that
// the brush no longer intersects
fAnyEmpty = NO;
fAllEmpty = YES;
for (i = 0; i < 4; ++i)
	{
	if (bRects[i].fEmpty)
		{
		fAnyEmpty = YES;
		continue;
		}
	if (fOnImage && AstralIntersectRect(&rSect, &bRects[i].rTile, &iRect))
		{
		fAllEmpty = NO;
		AstralUnionRect(&bRects[i].rEdit, &bRects[i].rEdit, &rSect);
		}
	else
		{
		// if we have some previous rects, put them out
//		if (!fAllEmpty && !fNoOverlap)
//	  		{
//			MessageBeep(0);
//			for (j = 0; j < i; ++j)
//			 	{
//			 	NewBltRect(&bRects[j].rEdit);
//			 	bRects[j].fEmpty = YES;
//			 	}
//			}
		NewBltRect(&bRects[i].rEdit);
		bRects[i].fEmpty = YES;
		fAnyEmpty = YES;
		}
	}

// move all rects to front of list
if (!fAllEmpty && fAnyEmpty)
	{
	for (i = 0; i < 4; ++i)
		{
		if (!bRects[i].fEmpty)
			{
			for (j = 0; j < i; ++j)
				{
				if (bRects[j].fEmpty)
					{
					bRects[j] = bRects[i];
					bRects[i].fEmpty = YES;
					break;
					}
				}
			}
		}
	}

// if not on image we are all done
if (!fOnImage || !fAnyEmpty)
	{
	// Adjust fade values
	FadeOutBrush(YES);
	return;
	}

pts[0].x = iRect.left;
pts[0].y = iRect.top;
pts[1].x = iRect.right;
pts[1].y = iRect.top;
pts[2].x = iRect.right;
pts[2].y = iRect.bottom;
pts[3].x = iRect.left;
pts[3].y = iRect.bottom;
for (pt = 0; pt < 4; ++pt)
	{
	pi = pPtTable[pt];
	xTile = pts[pi].x / bh;
	yTile = pts[pi].y / bv;
	empty = -1;
	for (i = 0; i < 4; ++i)
		{
		if (bRects[i].fEmpty)
			{
			if (empty < 0)
				empty = i;
			continue;
			}
		if (bRects[i].xTile == xTile && bRects[i].yTile == yTile)
			break;
		}
	if (i >= 4) // not found in rect list
		{
		if (empty >= 0)
			{
			pbRect = &bRects[empty];
			pbRect->xTile = xTile;
			pbRect->yTile = yTile;
			pbRect->rTile.left = xTile * bh;
			pbRect->rTile.right = pbRect->rTile.left + bh - 1;
			pbRect->rTile.top = yTile * bv;
			pbRect->rTile.bottom = pbRect->rTile.top + bv - 1;
			AstralIntersectRect(&pbRect->rEdit, &pbRect->rTile,
								 &iRect);
			pbRect->fEmpty = NO;
			}
		else
			Print("Tell Ted you got empty rectangle message");
		}
	}

// Adjust fade values
FadeOutBrush(YES);
}

/************************************************************************/
static void BltRect(PRECT lpRect, LPTR lpBrushMask)
{
RECT rDest, rSource, rOutput;
int dx, dy, w, h, sx, sy;
WORD bufsize;
LPTR lpBrushSrc, lpTmp, lpMskData;

// get destination x,y,width, and height of rect to be blt'ed
dx = lpRect->left;
dy = lpRect->top;
w = RectWidth(lpRect);
h = RectHeight(lpRect);

// get total number of pixels in rect to be blt'ed
bufsize = w * h;

// Load the destination buffer width destination data
if (!LoadSourceData(lpDstPixmap, pmDstType, dx, dy, w, h, lpDst,
				(LPRECT)&rDest, NO, NO))
	return;

// load source data
if (lpSrcPixmap)
	{
	sx = dx + sx_delta; // get location of source
	sy = dy + sy_delta;
	if (!(*lpGetSourceData)(lpSrcPixmap, pmSrcType, sx, sy, w, h, lpData,
			(LPRECT)&rSource, lpBrush->SourceFlipHorz, lpBrush->SourceFlipVert))
		return;
	lpSrcData = lpData;
	}
else
	{
	rSource = rDest;
	lpSrcData = lpDst;
	}

// call function to create the brush source from source data and source color
lpBrushSrc = (*lpCreateBrushSource)(dx, dy, w, h, State);

// Reset state
State = 0;

// If an image mask is being used, combine it with the Brush Mask
if (UsingMask)
	{
	// load image mask
	(*lpMaskProc)( dx+xDstOffset, dy+yDstOffset, w, h, lpMsk, lpDst, lpMask); 
	copyifset( lpBrushMask, lpMsk, bufsize );
	lpMskData = lpMsk;
	}
else if (!StylizeProc)
	lpMskData = lpBrushMask;
else
	{
	copy(lpBrushMask, lpMsk, bufsize);
	lpMskData = lpMsk;
	}

// Do style modifications to the mask, such as scatter
if (StylizeProc)
	(*StylizeProc)(lpMskData, lpMskData, lpErr, w, h, lpBrush->StylePressure);
//static void    ( *StylizeProc )( HPTR, HPTR, LPINT16, int, int, int );	// Proc to handle brush style

// Process the data by combining destination, source, and mask
(*lpProcessProc)(lpDst, lpBrushSrc, lpMskData, bufsize);

// Blt the new data to the frame
if (AstralIntersectRect(&rOutput, &rSource, &rDest))
	BltToFrame(dx, dy, w, h, lpDst, &rOutput, NO);

// swap last pointer and dest pointer
if (fUsingDstLast)
	{
	lpTmp = lpDst;
	lpDst = lpDstLast;
	lpDstLast = lpTmp;
	if (RectWidth(&rDest) != w || RectHeight(&rDest) != h)
		FillInBuffer(lpDstLast, w, h, &rDest);
	}
}

/************************************************************************/
static void FillInBuffer(LPTR lpBuf, int w, int h, LPRECT lpRect)
/************************************************************************/
{
int dx, dy;
WORD width;
LPTR lpLine, lpSrcPix;
LPFRAME lpFrame;
RECT rBuf;
long lPixel;

rBuf = *lpRect;
lpFrame = lpDstPixmap->EditFrame;
width = w * iBltFrameDepth;
dx = rBuf.left;	
if (dx)
	{
	dy = RectHeight(&rBuf);
	lpLine = lpBuf + (width * rBuf.top);
	while (--dy >= 0)
		{
		lpSrcPix = lpLine + (rBuf.left*iBltFrameDepth);
		frame_getpixel(lpSrcPix, iBltFrameDepth, &lPixel);	
		FrameSetPixel( lpFrame, lPixel, lpLine, dx);
		lpLine += width;
		}
	}
dx = w - rBuf.right - 1;	
if (dx)
	{
	dy = RectHeight(&rBuf);
	lpSrcPix = lpBuf + (width * rBuf.top);
	lpSrcPix += (rBuf.right * iBltFrameDepth);
	lpLine = lpSrcPix + iBltFrameDepth;
	while (--dy >= 0)
		{
		frame_getpixel(lpSrcPix, iBltFrameDepth, &lPixel);	
		FrameSetPixel( lpFrame, lPixel, lpLine, dx);
		lpSrcPix += width;
		lpLine += width;
		}
	}
dy = rBuf.top;	
if (dy)
	{
	lpLine = lpBuf;
	lpSrcPix = lpBuf + (width * rBuf.top);
	while (--dy >= 0)
		{
		copy(lpSrcPix, lpLine, width);
		lpLine += width;
		}
	}
dy = h - rBuf.bottom - 1;	
if (dy)
	{
	lpSrcPix = lpBuf + (width * rBuf.bottom);
	lpLine = lpSrcPix + width;
	while (--dy >= 0)
		{
		copy(lpSrcPix, lpLine, width);
		lpLine += width;
		}
	}
}

/************************************************************************/
static void NewBltRect(PRECT lpRect)
/************************************************************************/
{
RECT rDest, rSource, rOutput;
int dx, dy, w, h, sx, sy;
WORD bufsize;
LPTR lpBrushSrc;
LPTR lpBrushMask, lpTmp, lpMskData;
static BOOL fTemp;

if (!fNoOverlap)
	{
	BltToDisplay(lpRect);
	return;
	}

// get destination x,y,width, and height of rect to be blt'ed
dx = lpRect->left;
dy = lpRect->top;
w = RectWidth(lpRect);
h = RectHeight(lpRect);
if (w == bh || h == bv)
	{
	fTemp = YES;
	}

// get total number of pixels in rect to be blt'ed
bufsize = w * h;

GetMouseMoves();
// Load the destination buffer width destination data
if (!LoadSourceData(lpDstPixmap, pmDstType, dx, dy, w, h, lpDst,
				(LPRECT)&rDest, NO, NO))
	return;
GetMouseMoves();
if (!LoadSourceData(lpMskPixmap, pmMskType, dx, dy, w, h, lpMsk1,
				 (LPRECT)&rDest, NO, NO))
	return;
lpBrushMask = lpMsk1;

// load source data
if (lpSrcPixmap)
	{
	sx = dx + sx_delta; // get location of source
	sy = dy + sy_delta;
	GetMouseMoves();
	if (!(*lpGetSourceData)(lpSrcPixmap, pmSrcType, sx, sy, w, h, lpData,
			(LPRECT)&rSource, lpBrush->SourceFlipHorz, lpBrush->SourceFlipVert))
		return;
	lpSrcData = lpData;
	}
else
	{
	rSource = rDest;
	lpSrcData = lpDst;
	}

GetMouseMoves();
// call function to create the brush source from source data and source color
lpBrushSrc = (*lpCreateBrushSource)(dx, dy, w, h, State);

// Reset state
State = 0;

// If an image mask is being used, combine it with the Brush Mask
GetMouseMoves();
if (UsingMask)
	{
	// load image mask
	(*lpMaskProc)( dx+xDstOffset, dy+yDstOffset, w, h, lpMsk, lpDst, lpMask); 
	copyifset( lpBrushMask, lpMsk, bufsize );
	lpMskData = lpMsk;
	}
else if (!StylizeProc)
	lpMskData = lpBrushMask;
else
	{
	copy(lpBrushMask, lpMsk, bufsize);
	lpMskData = lpMsk;
	}

// Do style modifications to the mask, such as scatter
if (StylizeProc)
	{
	GetMouseMoves();
	(*StylizeProc)(lpMskData, lpMskData, lpErr, w, h, lpBrush->StylePressure);
	}

// Process the data by combining destination, source, and mask
GetMouseMoves();
(*lpProcessProc)(lpDst, lpBrushSrc, lpMskData, bufsize);

// Blt the new data to the frame
if (AstralIntersectRect(&rOutput, &rSource, &rDest))
	{
	GetMouseMoves();
	BltToFrame(dx, dy, w, h, lpDst, &rOutput, YES);
	}

GetMouseMoves();

if (fUsingDstLast)
	{
	// swap last pointer and dest pointer
	lpTmp = lpDst;
	lpDst = lpDstLast;
	lpDstLast = lpTmp;
	if (RectWidth(&rDest) != w || RectHeight(&rDest) != h)
		FillInBuffer(lpDstLast, w, h, &rDest);
	}
}

/************************************************************************/
static void BltToFrame( int x, int y, int dh, int dv, LPTR lpData,
						LPRECT lpRect, BOOL fUpdateDisplay)
/************************************************************************/
{
int sy;
WORD bwidth, width;
LPTR lp;
RECT StrokeRect;

StrokeRect = *lpRect;

// get width of buffer in bytes
bwidth = dh*iBltFrameDepth;

// advance in the number of lines and pixels
lpData += ((WORD)StrokeRect.top * bwidth) +
			((WORD)StrokeRect.left * iBltFrameDepth);
// get width of area to be blt'ed
width = (StrokeRect.right-StrokeRect.left+1) * iBltFrameDepth;

// offset stroked area from buffer to frame
StrokeRect.left += x;
StrokeRect.right += x;
StrokeRect.top += y;
StrokeRect.bottom += y;
if (fNoOverlap)
	{
	for (sy = StrokeRect.top; sy <= StrokeRect.bottom; ++sy)
		{
		PixmapCheck(lpOrigDstPixmap, PMT_EDIT, StrokeRect.left, sy, YES);
		if ((lp = PixmapPtr(lpDstPixmap, PMT_EDIT, StrokeRect.left, sy, YES)))
			copy(lpData, lp, width);
		lpData += bwidth;
		}
	}
else
	{
	for (sy = StrokeRect.top; sy <= StrokeRect.bottom; ++sy)
		{
		if ((lp = PixmapPtr(lpDstPixmap, PMT_EDIT, StrokeRect.left, sy, YES)))
			copy(lpData, lp, width);
		lpData += bwidth;
		}
	}
GetMouseMoves();

PixmapAddUndoArea(lpOrigDstPixmap, &StrokeRect);
if (fUpdateDisplay)
	BltToDisplay(&StrokeRect);
}

/************************************************************************/
static void BltToDisplay(LPRECT lpRect)
/************************************************************************/
{
RECT StrokeRect, DispRect;

StrokeRect = *lpRect;
if (lpUpdateProc)
	{
	(*lpUpdateProc)(hWnd, &StrokeRect, NO);
	}
else
if (fUpdateDisplay)
	{
	OffsetRect(&StrokeRect, xDstOffset, yDstOffset);
	if (fMaskEditing && lpMask)
		AstralUnionRect(&lpMask->rMask, &lpMask->rMask, &StrokeRect);
	AstralUnionRect(&rUpdate, &rUpdate, &StrokeRect);
	File2DispRect(&StrokeRect, &DispRect);
	ImgPaintRaster(lpImage, Window.hDC, &DispRect, NULL);
	if (fUpdateMarquee)
		UpdateMarquee(&StrokeRect, NO);
	}
}

/************************************************************************/
static void SetupOverlap()
/************************************************************************/
{
fNoOverlap = lpBrush->NoOverlap;
// certain brush functions must have overap setup particular ways
if (lpBrush->SourceFunction == SF_SMEAR_IMAGE)
	fNoOverlap = NO;
else
if (lpBrush->SourceFunction == SF_LIGHTEN_IMAGE ||
	lpBrush->SourceFunction == SF_DARKEN_IMAGE ||
	lpBrush->SourceFunction == SF_SHARPEN_IMAGE ||
	lpBrush->SourceFunction == SF_SMOOTH_IMAGE)
	fNoOverlap = YES;
}

/************************************************************************/
static void SetupDataProcs(int Depth)
/************************************************************************/
{
if (Depth == 3)
	{
	lpMaskProc = ( void ( * )( int, int, int, int, HPTR, HPTR, LPMASK ) )mload24;
// OLD	lpMaskProc = mload24;
	}
else
if (Depth == 4)
	{
	lpMaskProc = ( void ( * )( int, int, int, int, HPTR, HPTR, LPMASK ) )mload32;
// OLD	lpMaskProc = mload32;
	}
else
	{
	lpMaskProc = ( void ( * )( int, int, int, int, HPTR, HPTR, LPMASK ) )mload8;
// OLD	lpMaskProc = mload8;
	}
lpProcessProc = GetProcessProc(lpBrush->MergeMode, Depth);
}

/************************************************************************/
static void SetupBrushProcs(LPMGXBRUSH lpBrush, BOOL fBegin)
/************************************************************************/
{
if (!fBegin)
	{
	(*lpCreateBrushSource)(	0, 0, 0, 0, STATE_END);
	return;
	}
fUsingDstLast = NO;
if (lpBrush->SourceFlipMode == SFM_MIRROR)
	lpGetSourceData = LoadMirroredData;
else if (lpBrush->SourceFlipMode == SFM_TILE)
	lpGetSourceData = LoadTiledData;
else
	lpGetSourceData = LoadSourceData;

if (lpBrush->SourceFunction == SF_COPY_IMAGE)
	lpCreateBrushSource = CopyImageSource;
else if (lpBrush->SourceFunction == SF_SHARPEN_IMAGE)
	lpCreateBrushSource = SharpenImageSource;
else if (lpBrush->SourceFunction == SF_SMOOTH_IMAGE)
	lpCreateBrushSource = SmoothImageSource;
else if (lpBrush->SourceFunction == SF_LIGHTEN_IMAGE)
	lpCreateBrushSource = LightenImageSource;
else if (lpBrush->SourceFunction == SF_DARKEN_IMAGE)
	lpCreateBrushSource = DarkenImageSource;
else if (lpBrush->SourceFunction == SF_SMEAR_IMAGE)
	{
	fUsingDstLast = YES;
	lpCreateBrushSource = SmearImageSource;
	}
else
	lpCreateBrushSource = SolidColorSource;
(*lpCreateBrushSource)(	0, 0, 0, 0, STATE_BEGIN);

if (lpBrush->StrokeMethod == SM_CONNECTED)
	StrokeProc = NewDoDDABrushStroke;
else
	StrokeProc = DoBrushStroke;
if (lpBrush->Style == IDC_BRUSHSCATTER)
	StylizeProc = ScatterMask;
else
	StylizeProc = NULL;
}

/************************************************************************/
static void SetupBrush(LPMGXBRUSH lpBrush)
/************************************************************************/
{
SetupBrushMask(lpBrush);
BrushFull = lpBrush->Size;
BrushHalf = BrushFull / 2;
Spacing = (((WORD)lpBrush->Spacing * (WORD)BrushFull) + 50)/100;
if (!Spacing)
	Spacing = 1;
LastOpacity = ((lpBrush->Opacity * lpBrush->OpacityScale) + 50) / 100;

SetBrushOpacity(lpBrush->BrushMask, lpBM, LastOpacity, BrushFull*BrushFull);
}

/************************************************************************/
static void SetBrushOpacity(LPTR lpSrc, LPTR lpDst, WORD Opacity,
			int iCount)
/************************************************************************/
{
#ifdef C_CODE
while (--iCount >= 0)
	*lpDst++ = ((WORD)*lpSrc++ * Opacity)/255;
#else
__asm	{
	push	ds
	push	si
	push	di
	pushf
	lds	si, lpSrc
	les	di, lpDst
	mov	cx, iCount
	or	cx,cx
	je	SBO_END
	mov	bx,Opacity
	cld
	cmp	bx,127
	jbe	SBO_1
	inc	bx
SBO_1:
	lodsb
	sub	ah,ah
	mul	bx
	mov	es:[di],ah
	inc	di
	loop	SBO_1
SBO_END:
	popf
	pop	di
	pop	si
	pop	ds
	}
#endif
}

/************************************************************************/
static void SetupBrushTimer(LPMGXBRUSH lpBrush, BOOL fBegin)
/************************************************************************/
{
dwBrushTimer = lpBrush->Timer;
}

#ifdef UNUSED
/************************************************************************/
static void SetupBrushTimer(LPMGXBRUSH lpBrush, BOOL fBegin)
/************************************************************************/
{
static TIMERPROC lpTimerProc;
int	interval;

if (lpBrush->Timer <= 0)
	return;
if (fBegin)
	{
	interval = lpBrush->Timer/4;
	if (!interval)
		interval = 1;
	lpTimerProc = MakeProcInstance((FARPROC)BrushTimerProc, hInstAstral);
	idTimer = SetTimer(NULL, 0, interval, lpTimerProc);
	}
else
	{
	if (idTimer)
		KillTimer(NULL, idTimer);
	idTimer = 0;
	FreeProcInstance(lpTimerProc);
	}
}

/***********************************************************************/
void CALLBACK BrushTimerProc(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
{
int	SaveOpacity, SavePressure;

if (!lpBrush)
	return;
if (fFirstStroke)
	return;

if ((lParam - lLastStrokeTime) >= lpBrush->Timer)
	{
	fFirstStroke = YES;
	SaveOpacity = lpBrush->Opacity;
	SavePressure = lpBrush->StylePressure;
	lpBrush->Opacity = Opacity;
	lpBrush->StylePressure = StylePressure;
	FadeOpacity = 0L;
	SetupBrush(lpBrush);
	MgxBrushStroke(start_dx+sx_delta, start_dy+sy_delta, start_dx, start_dy,
		brush_x, brush_y);
	lpBrush->Opacity = SaveOpacity;
	lpBrush->StylePressure = SavePressure;
	SetupBrush(lpBrush);
	}
}
#endif

/************************************************************************/
static BOOL AllocBrushBuffers(LPMGXBRUSH lpBrush)
/************************************************************************/
{
long	lSize;

// "NULL" out all buffers
lpColor = lpData = lpSrc = lpDst = lpDstLast = lpMsk = NULL;
lpErr = NULL;
lpMsk1 = NULL;
lpBM = NULL;

// Calculate size of buffers to be used
// double-wide strips seem to be fastest for the most brush sizes

// setup buffers for building brush mask

// 80 is max to keep buffer size within 32K
if (fNoOverlap)
	{
	if (lpBrush->Size <= 30)
		bh = lpBrush->Size * 2;
	else
		bh = lpBrush->Size + (lpBrush->Size/2);
	}
else
	bh = lpBrush->Size;

if (bh < 10)
	bh = 10;
// make sure we fit in 64K
if (iBltFrameDepth <= 1)
	bh = min(bh, 255);
else
if (iBltFrameDepth == 3)
	bh = min(bh, 147);
else
	bh = min(bh, 127);
bv = bh;

bcount = bh * bv;

// Allocate memory for buffers used for all style brushes
lSize = iBltFrameDepth;
if (lSize == 0) lSize = 1;
lSize *= (long)bcount;
if (!(lpColor = Alloc( lSize )))
	goto MemError;
if (!(lpSrc = Alloc( lSize )))
	goto MemError;
if (!(lpDst = Alloc( lSize )))
	goto MemError;
if (!(lpMsk = Alloc( (long)bcount)))
	goto MemError;

if (lpBrush->SourceFunction == SF_SMEAR_IMAGE)
	{
	if (!(lpDstLast = Alloc( lSize )))
		goto MemError;
	}

// Allocate a source data buffer if there is a source frame
if (lpSrcPixmap)
	if (!(lpData = Alloc( lSize )))
		goto MemError;

// Allocate an error buffer for scatter style
if (lpBrush->Style == IDC_BRUSHSCATTER &&
	!(lpErr = (LPINT)Alloc((long)(bh+1)*3L*2L)))
	goto MemError;

// Allocate brushes needed for doing DDA strokes, save under buffers, etc.
if (lpBrush->StrokeMethod == SM_CONNECTED)
	{
	if (!(lpMsk1 = Alloc((long)bcount)))
		goto MemError;
	}

if (!(lpBM = Alloc( (long)sizeof(lpBrush->BrushMask) )))
	goto MemError;

if (!lpMoveBuf)
	goto MemError;
lpMoveIn = lpMoveBuf;
lpMoveOut = lpMoveBuf;

return(TRUE);

MemError:
Message(IDS_EMEMALLOC);
// freeup buffers
FreeUpBrushBuffers();
return(FALSE);
}

/************************************************************************/
static void FreeUpBrushBuffers()
/************************************************************************/
{
FreeUp( lpColor );
FreeUp( lpData );
FreeUp( lpSrc );
FreeUp( lpDst );
FreeUp( lpDstLast );
FreeUp( lpMsk );
FreeUp((LPTR)lpErr);
FreeUp( lpMsk1 );
FreeUp( lpBM );
}

/************************************************************************/
static void SetupFade(LPMGXBRUSH lpBrush)
/************************************************************************/
{
int		Delta;

// Setup the Fade In/Out values
if ( lpBrush->Shape == IDC_BRUSHCUSTOM || lpBrush->FadeSize == 0)
	FadeOut = 0L;
else	{
	Delta = ( lpBrush->FadeSize > 0 ? lpBrush->Size :
		  MAX_BRUSH_SIZE - lpBrush->Size + 1 );
	FadeOut = FGET( Delta, (100-abs(lpBrush->FadeSize))*FADE_LEN_MULT);
	if (lpBrush->FadeSize < 0)
		FadeOut = -FadeOut;
	}
BrushSize = lpBrush->Size;
FadeBrushSize = TOFIXED(BrushSize);

// Setup values for fading opacity
FadeOpacity = 0L;
Opacity = lpBrush->Opacity;
FadeOpacityValue = TOFIXED(Opacity);

MaxOpacity = ((lpBrush->Opacity * lpBrush->OpacityScale) + 50) / 100;

// Setup pressure values
StylePressure = lpBrush->StylePressure;
}

/************************************************************************/
static void ResetBrush(LPMGXBRUSH lpBrush, BOOL fNoBrushDisplay)
/************************************************************************/
{
if (FadeOut || FadeOpacity || fUsePressure)
	{
	if (fUpdateDisplay && !fNoBrushDisplay)
		DisplayBrush(0, 0, 0, OFF);
	lpBrush->Size = BrushSize;
	lpBrush->Opacity = Opacity;
	lpBrush->StylePressure = StylePressure;
	SetupBrush(lpBrush);
	if (fUpdateDisplay && !fNoBrushDisplay)
		DisplayBrush(hWnd, brush_x, brush_y, ON);
	}
}

/************************************************************************/
static void SetupSourceColor(LPMGXBRUSH lpBrush)
/************************************************************************/
{
if (lpBrush->SourceColor == SC_ACTIVE_COLOR)
	GetActiveColorFromType(iBltFrameType, &lpBrush->ColorStruct);
else if (lpBrush->SourceColor == SC_ALTERNATE_COLOR)
	GetAlternateColorFromType(iBltFrameType, &lpBrush->ColorStruct);
LoadColor(PixmapFrame(lpDstPixmap, PMT_EDIT), lpColor, bcount,
			&lpBrush->ColorStruct);
}

/************************************************************************/
static void SetupMaskState()
/************************************************************************/
{
if (fNotAnImage)
	{
	lpMask = NULL;
	UsingMask = NO;
	fUpdateMarquee = NO;
	}
else
	{
	lpMask = ImgGetMask(lpImage);
	fMaskEditing = lpMask && (lpDstPixmap == &lpMask->Pixmap);
	if (fMaskEditing)
		{
		UsingMask = FALSE;
		fUpdateMarquee = ImgGetMarqueeMask(lpImage) != NULL;
		}
	else
		{
		UsingMask = (lpMask != NULL) || (ColorMask.Mask && ColorMask.On);
		fUpdateMarquee = NO;
		}
	}
}

/************************************************************************/
static void SetupPressurePen()
/************************************************************************/
{
fUsePressure = NO;
if (Control.UseWacom && (fUsingMouse || fUpdateDisplay) && !fDoingMacro)
	PressureInit();
}

/************************************************************************/
static BOOL SetupFrames(LPMGXBRUSH lpBrush, BOOL fBegin)
/************************************************************************/
{
int	mode, iDataType;

if (fBegin) // called from MgxBrushBegin
	{
	lpDstPixmap = lpOrigDstPixmap;
	mode = RELEASE_OLDEST; // faster mode for retouching
	PixmapGetInfo(lpDstPixmap, PMT_EDIT, &iBltFrameXsize, &iBltFrameYsize,
					&iBltFrameDepth, &iBltFrameType);
	pmDstType = PMT_EDIT;
	lpMskFrame = NULL;
	fCreatedUndo = NO;
	if (fNoOverlap)
		{
		lpMskFrame = FrameOpen(FDT_GRAYSCALE, iBltFrameXsize, iBltFrameYsize, 100);
		if (!lpMskFrame)
			{
			Message(IDS_EMEMALLOC);
			return(FALSE);
			}
		FrameSetBackground(lpMskFrame, 0);

		lpMskPixmap = &mskPixmap;
		PixmapSetup(lpMskPixmap, lpMskFrame, YES);
		pmMskType = PMT_EDIT;

		if (Control.UseApply || !lpDstPixmap->UndoFrame ||
			lpBrush->SourceData == SD_CURRENT_UNDO)
			{
			PixmapSetup(&dstPixmap, lpDstPixmap->EditFrame, NO);
			if (!PixmapInitUndo(&dstPixmap))
				{
				FrameClose(lpMskFrame);
				Message(IDS_EMEMALLOC);
				}
			fCreatedUndo = YES;
			lpDstPixmap = &dstPixmap;
			}
		pmDstType = PMT_UNDO;
		}
	lpSrcPixmap = NULL;
	if (lpBrush->SourceData == SD_FILE)
		{
		if (!lpBrush->SourceFrame || (lpBrush->SourceFrame &&
			FrameDepth(lpBrush->SourceFrame) != iBltFrameDepth))
			{
			if (lpBrush->SourceFrame)
				FrameClose(lpBrush->SourceFrame);
			if (!(lpBrush->SourceFrame = AstralFrameLoad(lpBrush->szSource,
						iBltFrameDepth, &iDataType, NULL)))
				{
				Message(IDS_BRUSHINVALID, (LPTR)lpBrush->szSource);
				return(FALSE);
				}
			}
		if (lpBrush->SourceFrame &&
			FrameDepth(lpBrush->SourceFrame) != iBltFrameDepth)
			{
			Message(IDS_BRUSHINCOMPATIBLE);
			return(FALSE);
			}
		lpSrcPixmap = &srcPixmap;
		PixmapSetup(lpSrcPixmap, lpBrush->SourceFrame, YES);
		pmSrcType = PMT_EDIT;
		}
	else if (lpBrush->SourceFrame)
		{
		lpSrcPixmap = &srcPixmap;
		PixmapSetup(lpSrcPixmap, lpBrush->SourceFrame, YES);
		pmSrcType = PMT_EDIT;
		xSrcOffset = lpBrush->xSrcOffset;
		ySrcOffset = lpBrush->ySrcOffset;
		}
	else if (lpBrush->SourceData == SD_CURRENT_UNDO)
		{
		if (!lpDstPixmap->UndoFrame || lpDstPixmap->fNewFrame)
			{
			Message(IDS_BRUSHNOUNDO);
			return(FALSE);
			}
		lpSrcPixmap = lpOrigDstPixmap;
		pmSrcType = PMT_UNDO;
		}
	else if (lpBrush->SourceData == SD_CURRENT_DEST)
		{
		lpSrcPixmap = lpDstPixmap;
		pmSrcType = pmDstType;
		}
	}
else // called from MgxBrushEnd
	{
	if (lpMskFrame)
		FrameClose(lpMskFrame);
	if (fCreatedUndo)
		{
		PixmapFreeUndo(lpDstPixmap);
		pmDstType = PMT_EDIT;
		}
	mode = RELEASE_NEWEST; // normal mode
	}

// Set mode for Blt frame
FrameMode(PixmapFrame(lpDstPixmap, PMT_EDIT), mode);

// Set mode for Source frame
if (lpSrcPixmap)
	FrameMode(PixmapFrame(lpSrcPixmap, pmSrcType), mode);

// Set mode for Destination frame
if (pmDstType != PMT_EDIT)
	FrameMode(PixmapFrame(lpDstPixmap, pmDstType), mode);

return(TRUE);
}

/************************************************************************/
void FadeBrush(BOOL fSetupMask)
{
int	iNewOpacity;

if (FadeOut)
	{
	lpBrush->Size = FMUL(1, FadeBrushSize);
	lpBrush->Size = bound(lpBrush->Size, 0, BrushSize);
	if (fSetupMask)
			SetupBrush(lpBrush);
	else
		{
		BrushFull = bound(lpBrush->Size, 0, BrushSize);
		BrushHalf = BrushFull / 2;
		}
	}
if (FadeOpacity)
	{
	lpBrush->Opacity = FMUL(1, FadeOpacityValue);
	lpBrush->Opacity = bound(lpBrush->Opacity, 0, Opacity);
	iNewOpacity = ((lpBrush->Opacity * lpBrush->OpacityScale) + 50) / 100;
	if (iNewOpacity != LastOpacity)
		{
		SetBrushOpacity(lpBrush->BrushMask, lpBM, iNewOpacity,
				 BrushFull*BrushFull);
		LastOpacity = iNewOpacity;
		}
	}
}

/************************************************************************/
void FadeOutBrush(BOOL fSetupMask)
{
if (FadeOut || FadeOpacity)
	{
	if (FadeOut)
		FadeBrushSize -= FadeOut;
	if (FadeOpacity && fSetupMask)
		FadeOpacityValue -= FadeOpacity;
	if (FadeBrushSize > 0)
		FadeBrush(fSetupMask);
	}
}

/************************************************************************/
void FadeInBrush(BOOL fSetupMask)
{
if (FadeOut || FadeOpacity)
	{
	FadeBrushSize += FadeOut;
	if (fSetupMask)
		FadeOpacityValue += FadeOpacity;
	FadeBrush(fSetupMask);
	}
}


/***********************************************************************/
void PressureInit()
/***********************************************************************/
{
int iPressure;

if (!(fUsePressure = WacInit()))
	return;
PenPressure = WacPressure();
iPressure = PenPressure;
fPressureSize = ((Control.UseWacom & 2) != 0) && FadeOut == 0L;
fPressureOpacity = ((Control.UseWacom & 1) != 0);
fUsePressure = fUsePressure &&	(fPressureSize || fPressureOpacity);
if (fUsePressure)
	{ 
	if (fPressureSize)
		{
		lpBrush->Size = (iPressure * BrushSize) / 100;
		FadeBrushSize = FGET(lpBrush->Size, 1);
		}
	if (fPressureOpacity)
		{
		lpBrush->Opacity = (iPressure * Opacity) / 100;
		FadeOpacityValue = FGET(lpBrush->Opacity, 1);
		}
	SetupBrush(lpBrush);
	}
}

/***********************************************************************/
void PressureSetup(int	delta)
{
int iPressure, iNewSize, iNewOpacity;

if (!fUsePressure)
	return;
if (fPressureSize)
	FadeOut = 0;
if (fPressureOpacity)
	FadeOpacity = 0;
if (!delta)
	return;
iPressure = PenPressure;
if (fPressureSize)
	{
	iNewSize = (iPressure * BrushSize) / 100;
	if (iNewSize != lpBrush->Size)
		FadeOut = FGET(lpBrush->Size-iNewSize, delta);
	}
if (fPressureOpacity)
	{
	iNewOpacity = (iPressure * Opacity) / 100;
//	dbg("iPressure = %d Opacity = %d iNewOpacity = %d",
//		iPressure, Opacity, iNewOpacity);
	if (iNewOpacity != lpBrush->Opacity)
		FadeOpacity = FGET(lpBrush->Opacity-iNewOpacity, delta);
	}
}

/************************************************************************/
BOOL MgxOSBrushBegin(HMGXBRUSH hBrush, LPRECT lpStrokeRect, LPRECT lpMaxRect)
/************************************************************************/
{
// Call End function to make sure things get reset
MgxOSBrushEnd(NULL);

// Convert brush handle to brush pointer for our use
lpBrush = (LPMGXBRUSH)hBrush;

// Set frame mode to free oldest lines first instead of newest
// Assumption is that your typically brushing in the same area
if (!SetupOSFrame(YES, lpStrokeRect, lpMaxRect))
	return(FALSE);

// Setup Fade for brush size and brush opacity
SetupFade(lpBrush);

// Allocate all memory needed for brushing
if (!AllocOSBrushBuffers(lpBrush))
	{
	lpBrush = NULL;
	return(FALSE);
	}

// setup brush dependent procs
SetupOSBrushProcs(lpBrush);

// Initialize pressure sensitive pen settings
fUsePressure = NO;

// Setup the brush mask
SetupBrush(lpBrush);

// Indicate the first stroke is coming
fFirstStroke = YES;

return(TRUE);
}

/************************************************************************/
//	Call to pickup the brush
/************************************************************************/
void MgxOSPickupBrush(void)
{
	fFirstStroke = YES;
}

/************************************************************************/
LPFRAME MgxOSBrushEnd(LPRECT lpRect)
/************************************************************************/
{
if (!lpBrush)
	return(NULL);

ResetBrush(lpBrush, YES);
FreeUpOSBrushBuffers();
SetupOSFrame(NO, NULL, NULL);
lpBrush = NULL;
if (lpRect)
	{
	OffsetRect(&rOffScreen, xDstOffset, yDstOffset);
	*lpRect = rOffScreen;
	}
else
	{
	if (lpMskFrame)
		{
		FrameClose(lpMskFrame);
		lpMskFrame = NULL;
		}
	}
return(lpMskFrame);
}

/************************************************************************/
void MgxOSBrushStroke(int dx, int dy)
/************************************************************************/
{
if (!lpBrush)
	return;

dx -= xDstOffset;
dy -= yDstOffset;
(*OSStrokeProc)(dx, dy);
}

/************************************************************************/
static BOOL SetupOSFrame(BOOL fBegin, LPRECT lpStrokeRect, LPRECT lpMaxRect)
/************************************************************************/
{
int	mode;

if (fBegin) // called from MgxBrushBegin
	{
	rOffScreen = *lpStrokeRect;
	if (lpBrush->FadeSize)
		InflateRect(&rOffScreen, MAX_BRUSH_SIZE, MAX_BRUSH_SIZE);
	else
		InflateRect(&rOffScreen, lpBrush->Size/2, lpBrush->Size/2);
	BoundRect(&rOffScreen,	lpMaxRect->left, lpMaxRect->top,
							lpMaxRect->right, lpMaxRect->bottom);

	xDstOffset = rOffScreen.left;
	yDstOffset = rOffScreen.top;
	OffsetRect(&rOffScreen, -xDstOffset, -yDstOffset);
	lpMskFrame = FrameOpen(FDT_GRAYSCALE, RectWidth(&rOffScreen),
							RectHeight(&rOffScreen), 100);
	if (!lpMskFrame)
		return(FALSE);

	FrameSetBackground(lpMskFrame, 0);
	mode = RELEASE_OLDEST; // faster mode for retouching
	}
else // called from MgxBrushEnd
	{
	mode = RELEASE_NEWEST; // normal mode
	}

// Set mode for Blt frame
FrameMode(lpMskFrame, mode);

return(TRUE);
}

/************************************************************************/
static BOOL AllocOSBrushBuffers(LPMGXBRUSH lpBrush)
/************************************************************************/
{
if (!(lpBM = Alloc( (long)sizeof(lpBrush->BrushMask) )))
	return(FALSE);
return(TRUE);
}

/************************************************************************/
static void FreeUpOSBrushBuffers()
/************************************************************************/
{
if (lpBM)
	{
	FreeUp(lpBM);
	lpBM = NULL;
	}
}

/************************************************************************/
static void SetupOSBrushProcs(LPMGXBRUSH lpBrush)
/************************************************************************/
{
if (lpBrush->StrokeMethod == SM_CONNECTED)
	OSStrokeProc = DoDDAOSBrushStroke;
else
	OSStrokeProc = DoOSBrushStroke;
}

/************************************************************************/
static void DoDDAOSBrushStroke(int dx, int dy)
/************************************************************************/
{
// Initialize for first stroke that we get
if (fFirstStroke)
	{
	fFirstStroke = FALSE;
	start_dx = dx;		 // setup initial starting point
	start_dy = dy;
	last_x_processed = 32767;
	last_y_processed = 32767;
	}
else if (dx == start_dx && dy == start_dy)
	return; // point has moved - don't do anything

// setup new ending point - start point was the last point
end_dx = dx;
end_dy = dy;

if (abs(end_dx-start_dx) || abs(end_dy-start_dy))
	ddaline(start_dx, start_dy, end_dx, end_dy, (LPPOINTFUNC)ProcessOSBrushStroke);
else
	ProcessOSBrushStroke(start_dx, start_dy);
start_dx = end_dx;	
start_dy = end_dy;	
}

/************************************************************************/
static void DoOSBrushStroke(int dx, int dy)
/************************************************************************/
{
ProcessOSBrushStroke(dx, dy);
}

/************************************************************************/
static void ProcessOSBrushStroke(int x, int y)
/************************************************************************/
{
RECT bRect, iRect;

if (abs(last_x_processed-x) < Spacing &&
	abs(last_y_processed-y) < Spacing)
	{
	FadeOutBrush(YES);
	return;
	}

last_x_processed = x;
last_y_processed = y;

if (BrushFull)
	{
	// get rectangle this brush stroke encompasses
	bRect.top = y - BrushHalf;
	bRect.bottom = bRect.top + BrushFull - 1;
	bRect.left = x - BrushHalf;
	bRect.right = bRect.left + BrushFull - 1;

	if (AstralIntersectRect(&iRect, &rOffScreen, &bRect))
		OSMergeMask(lpMskFrame, lpBM, &bRect, BrushFull, &iRect, MaxOpacity);
	}

// Adjust fade values
FadeOutBrush(YES);
}

/************************************************************************/
static void OSMergeMask(LPFRAME lpMskFrame, LPTR lpSrc, LPRECT lpSrcRect,
					 int SrcWidth, LPRECT lpSectRect, int iMaxValue)
/************************************************************************/
{
int dx, dy, iWidth, iHeight, x, y;
LPTR lpSrcBuf, lpDstBuf;

iWidth = RectWidth(lpSectRect);
iHeight = RectHeight(lpSectRect);
if (!iWidth || !iHeight)
	return;

dy = lpSectRect->top - lpSrcRect->top;
dx = lpSectRect->left - lpSrcRect->left;
lpSrcBuf = lpSrc + (SrcWidth * dy) + dx;

x = lpSectRect->left;
y = lpSectRect->top;
while (--iHeight >= 0)
	{
	if (lpDstBuf = FramePointer(lpMskFrame, x, y, YES))
		OSMergeMaskLine(lpDstBuf, iWidth, lpSrcBuf, iMaxValue);
	lpSrcBuf += SrcWidth;
	++y;
	}
}

/************************************************************************/
static void	OSMergeMaskLine(LPTR lpDst, int iCount, LPTR lpSrc, int iMaxValue)
/************************************************************************/
{
#ifdef C_CODE
{
WORD s, d, rnd;

rnd = iMaxValue >> 1;
while (--iCount >= 0)
	{
// Don;t trust the C code below.... ifdef broken
	s = (WORD)(*lpSrc++);
	d = (WORD)(*lpDst);
	d = ((((iMaxValue - s) * d) + rnd) / iMaxValue) + s;
	*lpDst++ = (BYTE)d;
	}
}
#else
__asm
	{
	push	ds
	pushf
	push	di
	push	si
	les     di,lpDst
	lds		si,lpSrc
	cld
	mov		cx,iMaxValue
	mov		dx,iCount
	jmp		MM_DOWIDTHLOOP

MM_WIDTHLOOP:
	lodsb				; al = new value
	or		al,al		; any value?
	je		MM_NONEWVALUE
; start cl = maxvalue; al = src value
	mov		bl,BYTE PTR es:[di]  ; bl = dst value
	cmp		al,bl
	jbe		MM_NONEWVALUE
	cmp		al,cl
	jbe		MM_STOREBYTE
	mov		al,cl
MM_STOREBYTE:
	stosb
	jmp		MM_DOWIDTHLOOP
MM_NONEWVALUE:
	inc		di
MM_DOWIDTHLOOP:
	dec		dx	; decrement iCount
	jns	MM_WIDTHLOOP

	pop	si
	pop di
	popf
	pop	ds
	}
#endif
}


/************************************************************************/
LPTR SolidColorSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
return(lpColor);
}

/************************************************************************/
LPTR CopyImageSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
return(lpSrcData);
}

/************************************************************************/
LPTR SharpenImageSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
int rx, iCount;
LPTR lpSLine1, lpSLine2, lpSLine3, lpSrcPtr;
int bcount;
RECT rBuf;
static SHARPSMOOTHPROC  lpSharpSmoothProc;
static LPTR lpBuf;

if (State & (STATE_END|STATE_BEGIN))
	{
	if (lpBuf)
		FreeUp(lpBuf);
	lpBuf = NULL;
	if (State & STATE_BEGIN)
		{
		lpBuf = Alloc((long)(bh+2)*(long)(bv+2)*(long)iBltFrameDepth);
		switch ( iBltFrameDepth )
			{
			case 3:
				lpSharpSmoothProc = (SHARPSMOOTHPROC)GetSharpSmooth24(IDC_SHARP, lpBrush->Pressure);
			break;

			case 4:
				lpSharpSmoothProc = (SHARPSMOOTHPROC)GetSharpSmooth32(IDC_SHARP, lpBrush->Pressure);
			break;

			case 0:
			case 1:
			default:
				lpSharpSmoothProc = (SHARPSMOOTHPROC)GetSharpSmooth8( IDC_SHARP, lpBrush->Pressure);
			break;
			}
		}
	return(NULL);
	}

if (!lpBuf)
	return(lpSrcData);
if (!LoadSourceData(lpDstPixmap, pmDstType, x-1, y-1, dx+2, dy+2, lpBuf,
						&rBuf, NO, NO))
	return(lpSrcData);

bcount = (dx+2) * iBltFrameDepth;
//dx -= 2;
//dy -= 2;
//if (dx <= 0 || dy <= 0)
//	return(lpSrcData);
lpSrcPtr = lpSrc;
lpSLine1 = lpBuf;
lpSLine2 = lpSLine1 + bcount;
lpSLine3 = lpSLine2 + bcount;
while (--dy >= 0)
	{
	rx = 1;
	iCount = dx;
	while (--iCount >= 0)
		{
		(*lpSharpSmoothProc)( rx, lpSLine1, lpSLine2, lpSLine3, lpSrcPtr);
		++rx;
		lpSrcPtr += iBltFrameDepth;
		}
	lpSLine1 = lpSLine2;
	lpSLine2 = lpSLine3;
	lpSLine3 = lpSLine2 + bcount;
	}
return(lpSrc);
}

/************************************************************************/
LPTR SmoothImageSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
int rx, iCount;
LPTR lpSLine1, lpSLine2, lpSLine3, lpSrcPtr;
int bcount;
RECT rBuf;
static SHARPSMOOTHPROC lpSharpSmoothProc;
static LPTR lpBuf;

if (State & (STATE_END|STATE_BEGIN))
	{
	if (lpBuf)
		FreeUp(lpBuf);
	lpBuf = NULL;
	if (State & STATE_BEGIN)
		{
		lpBuf = Alloc((long)(bh+2)*(long)(bv+2)*(long)iBltFrameDepth);
		switch ( iBltFrameDepth )
			{
			case 3:
				lpSharpSmoothProc = (SHARPSMOOTHPROC)GetSharpSmooth24(IDC_SMOOTH, lpBrush->Pressure);
			break;

			case 4:
				lpSharpSmoothProc = (SHARPSMOOTHPROC)GetSharpSmooth32(IDC_SMOOTH, lpBrush->Pressure);
			break;

			case 0:
			case 1:
			default:
				lpSharpSmoothProc = (SHARPSMOOTHPROC)GetSharpSmooth8( IDC_SMOOTH, lpBrush->Pressure);
			break;
			}
		}
	return(NULL);
	}

if (!lpBuf)
	return(lpSrcData);
if (!LoadSourceData(lpDstPixmap, pmDstType, x-1, y-1, dx+2, dy+2, lpBuf,
						&rBuf, NO, NO))
	return(lpSrcData);

bcount = (dx+2) * iBltFrameDepth;
lpSrcPtr = lpSrc;
lpSLine1 = lpBuf;
lpSLine2 = lpSLine1 + bcount;
lpSLine3 = lpSLine2 + bcount;
while (--dy >= 0)
	{
	rx = 1;
	iCount = dx;
	while (--iCount >= 0)
		{
		(*lpSharpSmoothProc)( rx, lpSLine1, lpSLine2, lpSLine3, lpSrcPtr);
		++rx;
		lpSrcPtr += iBltFrameDepth;
		}
	lpSLine1 = lpSLine2;
	lpSLine2 = lpSLine3;
	lpSLine3 = lpSLine2 + bcount;
	}
return(lpSrc);
}

/************************************************************************/
LPTR LightenImageSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
	WORD wCount;
	int iBand;
	LPTR lpLut;

	if (State & (STATE_END|STATE_BEGIN))
		{
		if (State & STATE_BEGIN)
			{
			ResetMap(&Map, MAPPOINTS, NO);
			iBand = bound(lpBrush->Weight, BAND_ALL, BAND_SHADOWS);
			Map.Brightness[iBand] = -TOPERCENT(lpBrush->Pressure);
			MakeMap(&Map);
			}
		return(NULL);
		}

	lpLut = Map.Lut;
	wCount = (WORD)dx * (WORD)dy;
	if (!wCount)
		return(lpSrcData);

	switch(iBltFrameDepth)
	{
		case 0 :
		case 1 :
			MapLut(lpDst, lpSrc, lpLut, wCount);
		break;

		case 3 :
			wCount *= 3;
			MapLut(lpDst, lpSrc, lpLut, wCount);
		break;

		case 4 :
			wCount *= 4;
			MapLutInvert(lpDst, lpSrc, lpLut, wCount);
		break;
	}

	return(lpSrc);
}

/************************************************************************/
LPTR DarkenImageSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
	WORD wCount;
	int iBand;
	LPTR lpLut;

	if (State & (STATE_END|STATE_BEGIN))
		{
		if (State & STATE_BEGIN)
			{
			ResetMap(&Map, MAPPOINTS, NO);
			iBand = bound(lpBrush->Weight, BAND_ALL, BAND_SHADOWS);
			Map.Brightness[iBand] = TOPERCENT(lpBrush->Pressure);
			MakeMap(&Map);
			}
		return(NULL);
		}

	lpLut = Map.Lut;
	wCount = (WORD)dx * (WORD)dy;
	if (!wCount)
		return(lpSrcData);

	switch(iBltFrameDepth)
	{
		case 0 :
		case 1 :
			MapLut(lpDst, lpSrc, lpLut, wCount);
		break;

		case 3 :
			wCount *= 3;
			MapLut(lpDst, lpSrc, lpLut, wCount);
		break;

		case 4 :
			wCount *= 4;
			MapLutInvert(lpDst, lpSrc, lpLut, wCount);
		break;
	}

	return(lpSrc);
}

/************************************************************************/
LPTR SmearImageSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
static WORD weight;

if (State & (STATE_END|STATE_BEGIN))
	return(NULL);

if (State & STATE_FIRSTSTROKE)
	{
	weight = lpBrush->Weight;
	if (weight > 127)
		++weight;
	if (weight)
		{
		if (iBltFrameDepth == 3)
			Colorize24((LPRGB)lpSrc, (LPRGB)lpDst, dx*dy, (LPRGB)lpColor, weight);
		else
		if (iBltFrameDepth == 4)
			Colorize32((LPCMYK)lpSrc, (LPCMYK)lpDst, dx*dy, (LPCMYK)lpColor, weight);
		else
			Colorize8(lpSrc, lpDst, dx*dy, lpColor, weight);
		return(lpSrc);
		}
	else
		return(lpSrcData);
	}
else
	{
	if (weight)
		{
		if (iBltFrameDepth == 3)
			Colorize24((LPRGB)lpDstLast, (LPRGB)lpDstLast, dx*dy, (LPRGB)lpColor, weight);
		else
		if (iBltFrameDepth == 4)
			Colorize32((LPCMYK)lpDstLast, (LPCMYK)lpDstLast, dx*dy, (LPCMYK)lpColor, weight);
		else
			Colorize8(lpDstLast, lpDstLast, dx*dy, lpColor, weight);
		}
	return(lpDstLast);
	}
}

/************************************************************************/
static void CombineData24(
/************************************************************************/
	LPRGB 	lpDst,
	LPRECT 	lpDstRect,
	int 	DstWidth,
	LPRGB 	lpSrc,
	LPRECT 	lpSrcRect,
	int 	SrcWidth,
	LPRECT 	lpSectRect,
	WORD 	sb)
{
int iWidth, iHeight;
WORD db;

#ifdef C_CODE
int dx, dy, iCount;
LPRGB lpSrcBuf, lpDstBuf, lpSrcTmp, lpDstTmp;
BYTE r, g, b;

db = 256 - sb;

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
		r = ((lpDstTmp->red*db)+(lpSrcTmp->red*sb))>>8;
		lpDstTmp->red = lpSrcTmp->red = r;
		g = ((lpDstTmp->green*db)+(lpSrcTmp->green*sb))>>8;
		lpDstTmp->green = lpSrcTmp->green = g;
		b = ((lpDstTmp->blue*db)+(lpSrcTmp->blue*sb))>>8;
		lpDstTmp->blue = lpSrcTmp->blue = b;
		lpSrcTmp++;
		lpDstTmp++;
		}
	lpSrcBuf += SrcWidth;
	lpDstBuf += DstWidth;
	}
#else
__asm
	{
	push	ds
	push	si
	push	di
	pushf

;	lpDst = 6
;	lpDstRect = 10
;	DstWidth = 14
;	lpSrc = 16
;	lpSrcRect = 20
;	SrcWidth = 24
;	lpSectRect = 28
;	sb = 26

	les	si,lpSectRect		;lpSectRect
	mov	ax,WORD PTR es:[si+4]	;lpSectRect->right
	sub	ax,WORD PTR es:[si]	;lpSectRect->right-lpSectRect->left
	inc	ax			;RectWidth(lpSectRect)
	mov	cx,ax
	add	ax,ax
	add	ax,cx
	mov	iWidth,ax		; save iWidth * Depth

	mov	ax,WORD PTR es:[si+6]	;lpSectRect->bottom
	sub	ax,WORD PTR es:[si+2]	;lpSectRect->bottom-lpSectRect->top
	inc	ax			;RectHeight(lpSectRect)
	mov	iHeight,ax		; save iHeight

	mov	ax,SrcWidth		; get SrcWidth
	add	ax,ax
	add	SrcWidth,ax		; save SrcWidth * Depth

	mov	ax,DstWidth		; DstWidth
	add	ax,ax
	add	DstWidth,ax		; save DstWidth * Depth

	mov	ax,WORD PTR es:[si+2]
	lds	bx,lpDstRect		;lpDstRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	mov	cx,WORD PTR es:[si]
	sub	cx,WORD PTR ds:[bx]	; cx = dx
	mov	dx,cx
	add	cx,cx
	add	cx,dx			; dx * Depth

	mul 	DstWidth		; dy * DstWidth
	add	ax,cx			; (dy * DstWidth) + dx
	add	ax,WORD PTR [bp+6]	;lpDst
	mov	dx,WORD PTR [bp+8]
	mov	di,ax			;lpDstBuf
	push	dx			; save dst segment

	mov	ax,WORD PTR es:[si+2]
	lds	bx,lpSrcRect		;lpSrcRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	mov	cx,WORD PTR es:[si]
	sub	cx,WORD PTR ds:[bx]	; cx = dx
	mov	dx,cx
	add	cx,cx
	add	cx,dx			; dx * Depth

	mul	SrcWidth		; dy * SrcWidth
	add	ax,cx			; (dy * SrcWidth) + dx
	add	ax,WORD PTR [bp+16]	;lpSrc
	mov	dx,WORD PTR [bp+18]
	mov	si,ax			;lpSrcBuf
	mov	ds,dx

	mov	ax,iWidth
	sub	DstWidth,ax		; DstWidth
	sub	SrcWidth,ax		; SrcWidth

	mov	ax,256
	sub	ax,sb
	mov	db,ax

	pop	es			; restore dst segment

	cld
	jmp	CD24_3
CD24_1:
	mov	cx,iWidth
CD24_2:	
	mov	al,BYTE PTR ds:[si]	; get the source
	sub	ah,ah			; unsigned word conversion
	mul	sb			; source * b

	mov	bx,ax			; bx = (source * sb)
	
	mov	al,BYTE PTR es:[di]	; get the destination
	sub	ah,ah			; unsigned word conversion
	mul	db			; (dest * db)
	
	add	ax,bx			; ax = (s * b) + (d * db)

	mov	BYTE PTR es:[di],ah	; store in destination
	inc	di
	mov	BYTE PTR ds:[si],ah	; store in source
	inc	si

	loop	CD24_2

	add	si,SrcWidth		;SrcWidth
	add	di,DstWidth		;DstWidth
CD24_3:
	dec	iHeight
	jns	CD24_1

	popf
	pop	di
	pop	si
	pop	ds
	}
#endif
}

/************************************************************************/

static void CombineData32(
	LPCMYK 			lpDst,
	LPRECT 			lpDstRect,
	int    			DstWidth,
	LPCMYK 			lpSrc,
	LPRECT 			lpSrcRect,
	int    			SrcWidth,
	LPRECT 			lpSectRect,
	register WORD 	sb)
{
	register WORD db;
	int iWidth, iHeight;

	int dx, dy, iCount;
	LPCMYK lpSrcBuf, lpDstBuf;
	LPTR   lpSrcB = (LPTR)lpSrc;
	LPTR   lpDstB = (LPTR)lpDst;

	db = 256 - sb;

	dy = lpSectRect->top  - lpDstRect->top;
	dx = lpSectRect->left - lpDstRect->left;
	lpDstBuf = lpDst + (DstWidth * dy) + dx;

	dy = lpSectRect->top  - lpSrcRect->top;
	dx = lpSectRect->left - lpSrcRect->left;
	lpSrcBuf = lpSrc + (SrcWidth * dy) + dx;

	iWidth  = RectWidth(lpSectRect);
	iHeight = RectHeight(lpSectRect);

	while (iHeight-- > 0)
	{
		lpSrcB = (LPTR)lpSrcBuf;
		lpDstB = (LPTR)lpDstBuf;
		iCount = iWidth * 4;

		while (iCount-- > 0)
		{
			*lpDstB = *lpSrcB = (((*lpDstB)*db)+((*lpSrcB)*sb))>>8;
			lpDstB++;
			lpSrcB++;
		}

		lpSrcBuf += SrcWidth;
		lpDstBuf += DstWidth;
	}
}

/************************************************************************/
static void CombineData8(
/************************************************************************/
	LPTR 		lpDst,
	LPRECT 		lpDstRect,
	int 		DstWidth,
	LPTR 		lpSrc,
	LPRECT 		lpSrcRect,
	int 		SrcWidth,
	LPRECT 		lpSectRect,
	WORD 		sb)
{
WORD db;
int iWidth, iHeight;

#ifdef C_CODE
int dx, dy, iCount;
LPTR lpSrcBuf, lpDstBuf, lpSrcTmp, lpDstTmp;
BYTE g;

db = 256 - sb;

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
		g = ((db*(*lpDstTmp))+(sb*(*lpSrcTmp)))>>8;
		*lpDstTmp++ = *lpSrcTmp++ = g;
		}
	lpSrcBuf += SrcWidth;
	lpDstBuf += DstWidth;
	}
#else
__asm
	{
	push	ds
	push	si
	push	di
	pushf

;	lpDst = 6
;	lpDstRect = 10
;	DstWidth = 14
;	lpSrc = 16
;	lpSrcRect = 20
;	SrcWidth = 24
;	lpSectRect = 28
;	sb = 26

	les	si,lpSectRect		;lpSectRect
	mov	ax,WORD PTR es:[si+4]	;lpSectRect->right
	sub	ax,WORD PTR es:[si]	;lpSectRect->right-lpSectRect->left
	inc	ax			;RectWidth(lpSectRect)
	mov	iWidth,ax

	mov	ax,WORD PTR es:[si+6]	;lpSectRect->bottom
	sub	ax,WORD PTR es:[si+2]	;lpSectRect->bottom-lpSectRect->top
	inc	ax			;RectHeight(lpSectRect)
	mov	iHeight,ax

	mov	ax,WORD PTR es:[si+2]
	lds	bx,lpDstRect		;lpDstRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	mov	cx,WORD PTR es:[si]
	sub	cx,WORD PTR ds:[bx]	; cx = dx

	mul 	DstWidth		; dy * DstWidth
	add	ax,cx			; (dy * DstWidth) + dx
	add	ax,WORD PTR [bp+6]	;lpDst
	mov	dx,WORD PTR [bp+8]
	mov	di,ax			;lpDstBuf
	push	dx			; save dst segment

	mov	ax,WORD PTR es:[si+2]
	lds	bx,lpSrcRect		;lpSrcRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	mov	cx,WORD PTR es:[si]
	sub	cx,WORD PTR ds:[bx]	; cx = dx

	mul	SrcWidth		; dy * SrcWidth
	add	ax,cx			; (dy * SrcWidth) + dx
	add	ax,WORD PTR [bp+16]	;lpSrc
	mov	dx,WORD PTR [bp+18]
	mov	si,ax			;lpSrcBuf
	mov	ds,dx

	mov	ax,iWidth
	sub	DstWidth,ax		; DstWidth
	sub	SrcWidth,ax		; SrcWidth

	mov	ax,256
	sub	ax,sb
	mov	db,ax

	pop	es			; restore dst segment

	cld
	jmp	CD8_3
CD8_1:
	mov	cx,iWidth
CD8_2:	
	mov	al,BYTE PTR ds:[si]	; get the source
	sub	ah,ah			; unsigned word conversion
	mul	sb			; source * b

	mov	bx,ax			; bx = (source * sb)
	
	mov	al,BYTE PTR es:[di]	; get the destination
	sub	ah,ah			; unsigned word conversion
	mul	db			; (dest * db)
	
	add	ax,bx			; ax = (s * b) + (d * db)

	mov	BYTE PTR es:[di],ah	; store in destination
	inc	di
	mov	BYTE PTR ds:[si],ah	; store in source
	inc	si

	loop	CD8_2

	add	si,SrcWidth		;SrcWidth
	add	di,DstWidth		;DstWidth
CD8_3:
	dec	iHeight
	jns	CD8_1
	
	popf
	pop	di
	pop	si
	pop	ds
	}
#endif
}

/************************************************************************/
void Colorize24(LPRGB lpDst, LPRGB lpSrc, int iCount, LPRGB lpColor, WORD sb)
/************************************************************************/
{
WORD	r, g, b;

#ifdef C_CODE
WORD	db;

db = 256 - sb;
r = sb*lpColor->red;
g = sb*lpColor->green;
b = sb*lpColor->blue;
while (--iCount >= 0)
	{
	lpDst->red = ((db*lpSrc->red)+r+128)>>8;
	lpDst->green = ((db*lpSrc->green)+g+128)>>8;
	lpDst->blue = ((db*lpSrc->blue)+b+128)>>8;
	++lpDst;
	++lpSrc;
	}
#else
__asm
	{
	push	ds
	push	si
	push	di
	pushf

	mov	cx,iCount
	or	cx,cx
	je Done
	lds	si,lpColor
	sub	ah,ah
	lodsb
	mul	sb
	mov	r,ax
	sub	ah,ah
	lodsb
	mul	sb
	mov	g,ax
	sub	ah,ah
	lodsb
	mul	sb
	mov	b,ax
	mov	bx,256
	sub	bx,sb
	les	di,lpDst
	lds	si,lpSrc
	cld
C24_1:
; Process Red
	sub	ah,ah
	lodsb
	mul	bx
	add	ax,r
	add	ax,128
	mov	al,ah
	stosb

; Process Green
	sub	ah,ah
	lodsb
	mul	bx
	add	ax,g
	add	ax,128
	mov	al,ah
	stosb

; Process Blue
	sub	ah,ah
	lodsb
	mul	bx
	add	ax,b
	add	ax,128
	mov	al,ah
	stosb

	loop	C24_1

Done:
	popf
	pop	di
	pop	si
	pop	ds
	}
#endif
}

/************************************************************************/

void Colorize32(
	LPCMYK lpDst,
	LPCMYK lpSrc,
	int    iCount,
	LPCMYK  lpColor,
	WORD sb)
{
	register WORD db, iCnt;
	WORD c, m, y, k;
	LPTR lpSrcB, lpDstB;

	lpSrcB = (LPTR)lpSrc;
	lpDstB = (LPTR)lpDst;

	iCnt = iCount;

	db = 256 - sb;
	c = sb*(lpColor->c);
	m = sb*(lpColor->m);
	y = sb*(lpColor->y);
	k = sb*(lpColor->k);

	while (--iCount >= 0)
	{
		*lpDstB++ = ((db * (*lpSrcB++)) + c + 128) >> 8;
		*lpDstB++ = ((db * (*lpSrcB++)) + m + 128) >> 8;
		*lpDstB++ = ((db * (*lpSrcB++)) + y + 128) >> 8;
		*lpDstB++ = ((db * (*lpSrcB++)) + k + 128) >> 8;
	}
}

/************************************************************************/
void Colorize8(LPTR lpDst, LPTR lpSrc, int iCount, LPTR lpColor, WORD sb)
/************************************************************************/
{
WORD	g;

#ifdef C_CODE
WORD	db;

db = 256 - sb;
g = sb * (*lpColor);
while (--iCount >= 0)
	*lpDst++ = ((db*(*lpSrc++))+g+128)>>8;
#else
__asm
	{
	push	ds
	push	si
	push	di
	pushf

	mov	cx,iCount
	or	cx,cx
	je 	Done

	lds	si,lpColor
	sub	ah,ah
	lodsb
	mul	sb
	mov	g,ax

	mov	bx,256
	sub	bx,sb
	les	di,lpDst
	lds	si,lpSrc
	cld
C8_1:
; Process Gray
	sub	ah,ah
	lodsb
	mul	bx
	add	ax,g
	add	ax,128
	mov	al,ah
	stosb

	loop	C8_1

Done:
	popf
	pop	di
	pop	si
	pop	ds
	}
#endif
}

#ifdef C_CODE
/************************************************************************/
static void MapLut(LPTR lpSrc, LPTR lpDst, LPTR lpLut, WORD wCount)
/************************************************************************/
{
if (!wCount)
	return;
while (wCount-- != 0)
	*lpDst++ = lpLut[*lpSrc++];
}

/************************************************************************/
static void MapLutInvert(LPTR lpSrc, LPTR lpDst, LPTR lpLut, WORD wCount)
/************************************************************************/
{
BYTE value;

if (!wCount)
	return;
while (wCount-- != 0)
	{
	value = *lpSrc++;
	value ^= 0xFF;
	value = lpLut[value];
	value ^= 0xFF;
	*lpDst++ = value;
	}
}

#endif

/************************************************************************/
int SetMgxBrushSourceFunction(HMGXBRUSH hBrush, int SourceFunction)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int OldFunction = SF_SOLID_COLOR;
if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	OldFunction = lpBrush->SourceFunction;
	lpBrush->SourceFunction = SourceFunction;
	}
return(OldFunction);
}

/************************************************************************/
int SetMgxBrushWeight( HMGXBRUSH hBrush, int Weight )
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int OldWeight = 0;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
    OldWeight = lpBrush->Weight;
	lpBrush->Weight = Weight;
	}
return OldWeight;
}
