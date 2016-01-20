//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "svrview.h"
#include "id.h"

// Static prototypes
static void VertRulerPaint( HDC hDC, HWND hWindow );
static void PaintRuler( HDC hDC, HWND hWnd );
static void HorzRulerPaint( HDC hDC, HWND hWindow );
static void PaintRulerTick( HDC hDC, HWND hWnd, int x, int y );

#define RULER_STYLE(dwStyle) ((dwStyle) & 0x0F)
#define RULER_CLASS "ruler"
#define MAX_TICKS 16
#define MIN_LABEL_SPACE 30
#define MIN_LABEL_EXTRA 2
#define MIN_TICK_SPACE 3

static char szMinLabel[6] = "000";

typedef struct _ruler
	{
	int	iTicksPerUnit;
	int	iTickLength[MAX_TICKS];
	int	iTickSize;
	long	lUnitSize;
	long	lRulerSize;
	long	lUnitFactor;
	long	lStartValue;
	int	xPos;
	int	yPos;
	int	iDispStart;
} RULER;
typedef RULER far *LPRULER;

/***********************************************************************/
LOCAL void ShrinkClientRect(LPRECT lpRect)
/***********************************************************************/
{
	if (Control.fNewLook)
		InflateRect(lpRect, -1, -1);
}

/***********************************************************************/
LOCAL void GetRulerClientRect(HWND hWnd, LPRECT lpRect)
/***********************************************************************/
{
	GetDrawClientRect( hWnd, lpRect );
	ShrinkClientRect(lpRect);
}

/***********************************************************************/
LOCAL void GetRulerWindowRect(HWND hWnd, LPRECT lpRect)
/***********************************************************************/
{
	GetDrawClientRect( hWnd, lpRect );
}

/***********************************************************************/
void EnableRulers( BOOL bEnable )
/***********************************************************************/
{
POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
while (TemplatePos)
{
	CDocTemplate * pTemplate = PictPubApp.GetNextDocTemplate(TemplatePos);
	POSITION DocPos = pTemplate->GetFirstDocPosition();
	while (DocPos)
	{
	    CDocument *pDoc = pTemplate->GetNextDoc (DocPos);    
	    if (pDoc)
	    {
	        POSITION ViewPos = pDoc->GetFirstViewPosition();
	        while (ViewPos)
	        {
	            CServerView* pView = (CServerView*)pDoc->GetNextView( ViewPos );
	            if (pView && pView->m_lpDisplay)
	            {
	                HWND hWnd = pView->GetSafeHwnd();
	                LPDISPLAY lpDisplay = pView->m_lpDisplay;
	                if (lpDisplay->HasRulers && !bEnable)
	              	{
	                	DestroyRulers(hWnd);
	                	ComputeDispRect(hWnd);
	                	ComputeFileRect(hWnd, 0, 0, lpDisplay->FileRate );
	                	SetupRulers(hWnd);
	            	    InvalidateRect(hWnd, NULL, TRUE);
	            	}
	                else if (!lpDisplay->HasRulers && bEnable)
	             	{
	                	CreateRulers(hWnd);
	                	ComputeDispRect(hWnd);
	            	    ComputeFileRect(hWnd, 0, 0, lpDisplay->FileRate );
	                	SetupRulers(hWnd);
	                	InvalidateRect(hWnd, NULL, TRUE);
	            	}
	            }
	        }
	    }
	}
}
}

/***********************************************************************/
void UpdateRulers(HWND hWnd)
/***********************************************************************/
{
HWND hChildWnd;
STRING szClassName;
LPDISPLAY lpDisplay;

lpDisplay = (LPDISPLAY)GetDisplayPtr ( hWnd );
if (!lpDisplay->HasRulers)
	return;
hChildWnd = GetWindow(hWnd, GW_CHILD);
while (hChildWnd)
	{
	GetClassName(hChildWnd, szClassName, MAX_STR_LEN);
	if (StringsEqual(szClassName, RULER_CLASS))
		UpdateWindow(hChildWnd);
	hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
	}
}

/***********************************************************************/
int GetRulerSize()
/***********************************************************************/
{
LOGFONT font;

GetObject( (HFONT)GetStockObject(Window.iLabelFont), sizeof(LOGFONT),
	(LPSTR)&font);
return(font.lfHeight+7); // 7 = 1 plus max ticks
}

/***********************************************************************/
BOOL CreateRulers(HWND hWnd)
/***********************************************************************/
{
RECT ClientRect;
LPDISPLAY lpDisplay;

lpDisplay = (LPDISPLAY)GetDisplayPtr ( hWnd );
if (lpDisplay->HasRulers)
	return( TRUE );
GetClientRect(hWnd, &ClientRect);
if (!CreateWindow(RULER_CLASS,
			"",
			WS_CHILD|WS_DISABLED|WS_CLIPSIBLINGS|WS_VISIBLE|RS_HORZ,
			0,
			0,
			RectWidth(&ClientRect),
			GetRulerSize(),
			hWnd,
			NULL,
			PictPubApp.GetResourceHandle(),
			NULL))
	return(FALSE);
if (!CreateWindow(RULER_CLASS,
			"",
			WS_CHILD|WS_DISABLED|WS_CLIPSIBLINGS|WS_VISIBLE|RS_VERT,
			0,
			0,
			GetRulerSize(),
			RectHeight(&ClientRect),
			hWnd,
			NULL,
			PictPubApp.GetResourceHandle(),
			NULL))
	{
	DestroyRulers(hWnd);
	return(FALSE);
	}
lpDisplay->HasRulers = YES;
return(TRUE);
}

/***********************************************************************/
void DestroyRulers(HWND hWnd)
/***********************************************************************/
{
HWND hChildWnd, hDestroyWnd;
STRING szClassName;
LPDISPLAY lpDisplay;

lpDisplay = (LPDISPLAY)GetDisplayPtr ( hWnd );
if (!lpDisplay->HasRulers)
	return;

hChildWnd = GetWindow(hWnd, GW_CHILD);
while (hChildWnd)
	{
	GetClassName(hChildWnd, szClassName, MAX_STR_LEN);
	if (StringsEqual(szClassName, RULER_CLASS))
		{
		hDestroyWnd = hChildWnd;
		hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
		DestroyWindow(hDestroyWnd);
		}
	else
		hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
	}
lpDisplay->HasRulers = NO;
}

/***********************************************************************/
void SetupAllRulers()
/***********************************************************************/
{
POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
while (TemplatePos)
{
	CDocTemplate * pTemplate = PictPubApp.GetNextDocTemplate(TemplatePos);
	POSITION DocPos = pTemplate->GetFirstDocPosition();
	while (DocPos)
	{
	    CDocument *pDoc = pTemplate->GetNextDoc (DocPos);    
	    if (pDoc)
	    {
	        POSITION ViewPos = pDoc->GetFirstViewPosition();
	        while (ViewPos)
	        {
	            CServerView* pView = (CServerView*)pDoc->GetNextView( ViewPos );
	            if (pView && pView->m_lpDisplay)
	            {
	                HWND hWnd = pView->GetSafeHwnd();
	                LPDISPLAY lpDisplay = pView->m_lpDisplay;
	                if (lpDisplay->HasRulers)
	            		SetupRulers(hWnd);
	            }
	        }
	    }
	}
}
}

/***********************************************************************/
void SetupRulers(HWND hWnd)
/***********************************************************************/
{
HWND hChildWnd;
STRING szClassName;
DWORD dwStyle;
RECT ClientRect;
LONG lRW, lRH, lFW, lFH, lDW, lDH;
int ruler_size;
long lRes;
LPRULER lpData;
LPDISPLAY lpDisplay;
LPIMAGE lpImage;

lpImage = (LPIMAGE)GetImagePtr ( hWnd );
lpDisplay = (LPDISPLAY)GetDisplayPtr ( hWnd );
if (!lpDisplay->HasRulers)
	return;

lRes = FrameResolution( ImgGetBaseEditFrame(lpImage) );
hChildWnd = GetWindow(hWnd, GW_CHILD);
while (hChildWnd)
	{
	GetClassName(hChildWnd, szClassName, MAX_STR_LEN);
	if (StringsEqual(szClassName, RULER_CLASS) &&
	    (lpData = (LPRULER)GetWindowLong(hChildWnd, GWL_CTLDATAPTR)))
		{
		ruler_size = GetRulerSize();
		GetRulerClientRect(hChildWnd, &ClientRect);
		lRW = ClientRect.right-(ClientRect.left+ruler_size-1);
		lRH = ClientRect.bottom-(ClientRect.top+ruler_size-1);
		lFW = RectWidth(&lpDisplay->FileRect);
		lFH = RectHeight(&lpDisplay->FileRect);
		lDW = RectWidth(&lpDisplay->DispRect);
		lDH = RectHeight(&lpDisplay->DispRect);
		dwStyle = GetWindowLong(hChildWnd, GWL_STYLE);
		if (Control.Units == IDC_PREF_UNITMM)
			{
			lpData->iTicksPerUnit = 10;
			lpData->iTickLength[0] = 1;
			lpData->iTickLength[1] = 2;
			lpData->iTickLength[2] = 1;
			lpData->iTickLength[3] = 2;
			lpData->iTickLength[4] = 1;
			lpData->iTickLength[5] = 2;
			lpData->iTickLength[6] = 1;
			lpData->iTickLength[7] = 2;
			lpData->iTickLength[8] = 1;
			lpData->iTickLength[9] = -1;
			lpData->iTickSize = 3;
			lpData->lUnitSize = 100L;
			lpData->lUnitFactor = 10L;
			if (RULER_STYLE(dwStyle) == RS_HORZ)
				{
				lpData->iDispStart =
				    lpDisplay->DispRect.left;
				lpData->lRulerSize =
				    (lFW*254L*lRW)/(lRes*lDW);
				lpData->lStartValue =
				    ((long)lpDisplay->FileRect.left
				    * 254L)/lRes;
				}
			else if (RULER_STYLE(dwStyle) == RS_VERT)
				{
				lpData->iDispStart =
				    lpDisplay->DispRect.top;
				lpData->lRulerSize =
				    (lFH*254L*lRH)/(lRes*lDH);
				lpData->lStartValue =
				    ((long)lpDisplay->FileRect.top
				    * 254L)/lRes;
				}
			}
		else if (Control.Units == IDC_PREF_UNITPICAS)
			{
			lpData->iTicksPerUnit = 5;
			lpData->iTickLength[0] = 1;
			lpData->iTickLength[1] = 1;
			lpData->iTickLength[2] = 1;
			lpData->iTickLength[3] = 1;
			lpData->iTickLength[4] = -1;
			lpData->iTickSize = 6;
			lpData->lUnitSize = 50L;
			lpData->lUnitFactor = 10L;
			if (RULER_STYLE(dwStyle) == RS_HORZ)
				{
				lpData->iDispStart =
				    lpDisplay->DispRect.left;
				lpData->lRulerSize =
				  (lFW*723L*lRW)/(lRes*12L*lDW);
				lpData->lStartValue =
				    ((long)lpDisplay->FileRect.left
				    * 723L)/(lRes*12L);
				}
			else if (RULER_STYLE(dwStyle) == RS_VERT)
				{
				lpData->iDispStart =
				    lpDisplay->DispRect.top;
				lpData->lRulerSize =
				  (lFH*723L*lRH)/(lRes*12L*lDH);
				lpData->lStartValue =
				    ((long)lpDisplay->FileRect.top
				    * 723L)/(lRes*12L);
				}
			}
		else if (Control.Units == IDC_PREF_UNITCM)
			{
			lpData->iTicksPerUnit = 2;
			lpData->iTickLength[0] = 1;
			lpData->iTickLength[1] = -1;
			lpData->iTickSize = 6;
			lpData->lUnitSize = 100L;
			lpData->lUnitFactor = 100L;
			if (RULER_STYLE(dwStyle) == RS_HORZ)
				{
				lpData->iDispStart =
				    lpDisplay->DispRect.left;
				lpData->lRulerSize =
				    (lFW*254L*lRW)/(lRes*lDW);
				lpData->lStartValue =
				    ((long)lpDisplay->FileRect.left
				    * 254L)/lRes;
				}
			else if (RULER_STYLE(dwStyle) == RS_VERT)
				{
				lpData->iDispStart =
				    lpDisplay->DispRect.top;
				lpData->lRulerSize =
				    (lFH*254L*lRH)/(lRes*lDH);
				lpData->lStartValue =
				    ((long)lpDisplay->FileRect.top
				    * 254L)/lRes;
				}
			}
		else if (Control.Units == IDC_PREF_UNITINCHES)
			{
			lpData->iTicksPerUnit = 8;
			lpData->iTickLength[0] = 1;
			lpData->iTickLength[1] = 2;
			lpData->iTickLength[2] = 1;
			lpData->iTickLength[3] = 3;
			lpData->iTickLength[4] = 1;
			lpData->iTickLength[5] = 2;
			lpData->iTickLength[6] = 1;
			lpData->iTickLength[7] = -1;
			lpData->iTickSize = 2;
			lpData->lUnitSize = 1000L;
			lpData->lUnitFactor = 1000L;
			if (RULER_STYLE(dwStyle) == RS_HORZ)
				{
				lpData->iDispStart =
				    lpDisplay->DispRect.left;
				lpData->lRulerSize =
				  (lFW * 1000L*lRW)/(lRes*lDW);
				lpData->lStartValue =
				    ((long)lpDisplay->FileRect.left
				    * 1000L)/lRes;
				}
			else if (RULER_STYLE(dwStyle) == RS_VERT)
				{
				lpData->iDispStart =
				    lpDisplay->DispRect.top;
				lpData->lRulerSize =
				    (lFH * 1000L*lRH)/(lRes*lDH);
				lpData->lStartValue =
				    ((long)lpDisplay->FileRect.top
				    * 1000L)/lRes;
				}
			}
		else // if (Control.Units == IDC_PREF_UNITPIXELS)
			{
			lpData->iTicksPerUnit = 1;
			lpData->iTickLength[0] = -1;
			lpData->iTickSize = 6;
			lpData->lUnitSize = 50L;
			lpData->lUnitFactor = 1L;
			if (RULER_STYLE(dwStyle) == RS_HORZ)
				{
				lpData->iDispStart =
				    lpDisplay->DispRect.left;
				lpData->lRulerSize = (lFW*lRW)/lDW;
				lpData->lStartValue =
				    (long)lpDisplay->FileRect.left;
				}
			else if (RULER_STYLE(dwStyle) == RS_VERT)
				{
				lpData->iDispStart =
				    lpDisplay->DispRect.top;
				lpData->lRulerSize = (lFH*lRH)/lDH;
				lpData->lStartValue =
				    (long)lpDisplay->FileRect.top;
				}
			}
		InvalidateRect(hChildWnd, NULL, TRUE);
		}
	hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
	}
}

/***********************************************************************/
void SetRulerTicks(HWND hWnd, int x, int y)
/***********************************************************************/
{
HWND hChildWnd;
STRING szClassName;
LPDISPLAY lpDisplay;

lpDisplay = (LPDISPLAY)GetDisplayPtr ( hWnd );
if (!lpDisplay->HasRulers)
	return;

hChildWnd = GetWindow(hWnd, GW_CHILD);
while (hChildWnd)
	{
	GetClassName(hChildWnd, szClassName, MAX_STR_LEN);
	if (StringsEqual(szClassName, RULER_CLASS))
		FORWARD_WM_MOUSEMOVE(hChildWnd, x, y, 0, SendMessage);
	hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
	}
}

/***********************************************************************/
void SizeRulers(HWND hWnd)
/***********************************************************************/
{
HWND hChildWnd;
STRING szClassName;
DWORD dwStyle;
RECT ClientRect, RulerRect;
LPDISPLAY lpDisplay;

lpDisplay = (LPDISPLAY)GetDisplayPtr ( hWnd );
if (!lpDisplay->HasRulers)
	return;

hChildWnd = GetWindow(hWnd, GW_CHILD);
while (hChildWnd)
	{
	GetClassName(hChildWnd, szClassName, MAX_STR_LEN);
	if (StringsEqual(szClassName, RULER_CLASS))
		{
		dwStyle = GetWindowLong(hChildWnd, GWL_STYLE);
		GetClientRect(hWnd, &ClientRect);
		GetClientRect(hChildWnd, &RulerRect);
		if (RULER_STYLE(dwStyle) == RS_HORZ)
			{
			MoveWindow(hChildWnd, 0, 0, RectWidth(&ClientRect)-1, RectHeight(&RulerRect)-1, NO);
			}
		else if (RULER_STYLE(dwStyle) == RS_VERT)
			{
			MoveWindow(hChildWnd, 0, 0, RectWidth(&RulerRect)-1, RectHeight(&ClientRect)-1, NO);
			}
		}
	hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
	}
}

/***********************************************************************/
/***********************************************************************/
// RULER CONTROL
/***********************************************************************/
/***********************************************************************/

LOCAL BOOL Ruler_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct);
LOCAL void Ruler_OnPaint(HWND hWindow);
LOCAL void Ruler_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);

static RECT RulerRect;
static POINT RulerLastPoint;

/***********************************************************************/
LONG WINPROC EXPORT Ruler_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);

	HANDLE_MSG(hWindow, WM_CREATE, Ruler_OnCreate);
	HANDLE_MSG(hWindow, WM_PAINT, Ruler_OnPaint);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Ruler_OnMouseMove);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL BOOL Ruler_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
LPRULER lpData;

lpData = (LPRULER)Alloc((long)sizeof(RULER));
if (!lpData)
	return(FALSE);
clr((LPTR)lpData, sizeof(RULER));
SetWindowLong(hWindow, GWL_CTLDATAPTR, (long)lpData);
return(TRUE);
}

/***********************************************************************/
void SculptHorzRuler( HDC hDC, LPRECT lpRect)
/***********************************************************************/
{
	HGDIOBJ  hOldPen;
	RECT     Rect;
	POINT    p;
	int		iLeft;

	Rect = *lpRect;
	WindowsToAstralRect(&Rect);

	hOldPen = SelectObject( hDC, DrawTools.GetFramePen() );

	// draw lower right
	SelectObject(hDC, DrawTools.GetFramePen());

	iLeft = Rect.left + RectHeight(&Rect) - 2; 
	MoveToEx( hDC, Rect.right, Rect.top, &p);
	LineTo( hDC, Rect.right, Rect.bottom );
	LineTo( hDC, iLeft, Rect.bottom );

	// draw upper left
	SelectObject(hDC, DrawTools.GetBtnHilightPen());

	MoveToEx( hDC, Rect.left, Rect.bottom, &p);
	LineTo( hDC, Rect.left, Rect.top );
	LineTo( hDC, Rect.right, Rect.top );

	InflateRect( &Rect, -1, -1 );

	// draw lower right
	SelectObject(hDC, DrawTools.GetBtnShadowPen());

	MoveToEx( hDC, Rect.right, Rect.top, &p);
	LineTo( hDC, Rect.right, Rect.bottom );
	LineTo( hDC, iLeft, Rect.bottom );

	// draw upper left
	SelectObject(hDC, DrawTools.GetBtnFacePen());

	MoveToEx( hDC, Rect.left, Rect.bottom, &p);
	LineTo( hDC, Rect.left, Rect.top );
	LineTo( hDC, Rect.right, Rect.top );

	// draw two pixels of shadow color that need to be turned on
	// to turn the corner
	SelectObject(hDC, DrawTools.GetBtnShadowPen());
	MoveTo(hDC, iLeft, Rect.bottom+1);
	LineTo(hDC, iLeft, Rect.bottom-1);

	SelectObject( hDC, hOldPen );
}

/***********************************************************************/
LOCAL void Ruler_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
RECT ClientRect;
HDC hDC;
LPRULER lpData;

hDC = BeginPaint( hWindow, &ps );
lpData = (LPRULER)GetWindowLong(hWindow, GWL_CTLDATAPTR);
GetClientRect( hWindow, &ClientRect );
EraseControlBackground( hDC, hWindow, &ClientRect,
	CTLCOLOR_BTN );
if (Control.fNewLook)
	{
	DWORD dwStyle = GetWindowLong(hWindow, GWL_STYLE);
	if (dwStyle & RS_HORZ)
		SculptHorzRuler(hDC, &ClientRect);
	else
		DrawSculptedBox( hDC, &ClientRect, FALSE, FALSE, FALSE );
	}

PaintRuler(hDC, hWindow);
if (lpData->xPos && lpData->yPos)
	{
	PaintRulerTick(hDC, hWindow, lpData->xPos, lpData->yPos);
	}
EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Ruler_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
HDC hDC;
LPRULER lpData;

lpData = (LPRULER)GetWindowLong(hWindow, GWL_CTLDATAPTR);
hDC = GetDC(hWindow);
if (lpData->xPos && lpData->yPos)
	PaintRulerTick(hDC, hWindow, lpData->xPos, lpData->yPos);
lpData->xPos = x;
lpData->yPos = y;
if (lpData->xPos && lpData->yPos)
	PaintRulerTick(hDC, hWindow, lpData->xPos, lpData->yPos);
ReleaseDC(hWindow, hDC);
}

/***********************************************************************/
static void PaintRuler( HDC hDC, HWND hWnd )
/***********************************************************************/
{
DWORD dwStyle;

dwStyle = GetWindowLong(hWnd, GWL_STYLE);
if (dwStyle & RS_HORZ)
	HorzRulerPaint(hDC, hWnd);
else if (dwStyle & RS_VERT)
	VertRulerPaint(hDC, hWnd);
}


/***********************************************************************/
static VOID HorzRulerPaint( HDC hDC, HWND hWindow )
/***********************************************************************/
{
RECT ClientRect, WindowRect;
char text[256];
int iTicksPerUnit, iNewPerUnit, iTickSize, factor, iDispStart, iRulerWidth;
int ticklength[MAX_TICKS];
int x, y, num_ticks, index, i, bottom, top, num, min_label_space, right;
long lRulerSize, lUnitSize, lUnitFactor, lNewSize;
long lValue, lStartValue, lTickUnit, lBefore;
LFIXED rate, urate, offset;
HPEN hOldPen;
HFONT hOldFont;
TEXTMETRIC info;
SIZE Extent;
LPRULER lpData;

lpData = (LPRULER)GetWindowLong(hWindow, GWL_CTLDATAPTR);
GetRulerClientRect( hWindow, &ClientRect );
GetRulerWindowRect( hWindow, &WindowRect );

// retrieve the numbers of ticks per unit and bound
iTicksPerUnit = bound(lpData->iTicksPerUnit, 1, MAX_TICKS);

// retrieve the relative lengths of the tick marks
for (i = 0; i < iTicksPerUnit; ++i)
	ticklength[i] = lpData->iTickLength[i];

// retrieve all other parameters
iTickSize = lpData->iTickSize;
lUnitSize = lpData->lUnitSize;
lRulerSize = lpData->lRulerSize;
lUnitFactor = lpData->lUnitFactor;
lStartValue = lpData->lStartValue;

// start to the left of the first pixel
iDispStart = lpData->iDispStart - 1;

// draw bottom line of ruler and left start line
hOldPen = (HPEN)SelectObject(hDC, GetStockObject(BLACK_PEN));
if (!Control.fNewLook)
	{
	MoveTo(hDC, ClientRect.left, ClientRect.bottom);
	LineTo(hDC, ClientRect.right+1, ClientRect.bottom);
	}
right = WindowRect.left + (RectHeight(&WindowRect)-1);
// draw line for vertical ruler
if (Control.fNewLook)
	{
	MoveTo(hDC, ClientRect.left, WindowRect.bottom);
	LineTo(hDC, right-1, WindowRect.bottom); 
	}

ClientRect.left = right;
MoveTo(hDC, ClientRect.left, ClientRect.top);
if (!Control.fNewLook)
	LineTo(hDC, ClientRect.left, ClientRect.bottom+1);
else
	LineTo(hDC, ClientRect.left, ClientRect.bottom);

// use small label font and get metrics
hOldFont = (HFONT)SelectObject(hDC, GetStockObject(Window.iLabelFont) );
GetTextMetrics(hDC, &info);
GetTextExtentPoint(hDC, szMinLabel, lstrlen(szMinLabel), &Extent);
min_label_space = Extent.cy * lstrlen(szMinLabel);
min_label_space = max(min_label_space, Extent.cx);
min_label_space += MIN_LABEL_EXTRA;

// calculate the rate to advance to next unit label
iRulerWidth = RectWidth(&ClientRect);
urate = FGET((long)iRulerWidth*lUnitSize, lRulerSize);
if (urate <= 0L)
	{
	SelectObject(hDC, hOldFont);
	SelectObject(hDC, hOldPen);
	return;
	}

// check to make sure we have MIN_LABEL_SPACE pixels between labels
lNewSize = lUnitSize;
while (urate < FGET(min_label_space, 1))
	{
	urate = FGET((long)iRulerWidth*lNewSize*5L, lRulerSize);
	rate = urate/10;
	if (urate <= 0L || rate <= 0L)  // overflow?
		{
		urate = FGET((long)iRulerWidth*lNewSize, lRulerSize);
		break;
		}
	lNewSize *= 5L;
	iTicksPerUnit = 10;
	for (i = 0; i < 5; ++i)
		ticklength[i*2] = 1;	
	for (i = 0; i < 5; ++i)
		ticklength[(i*2)+1] = 2;
	ticklength[iTicksPerUnit-1] = -1;
	iTickSize = 3;
	}

// make sure we have MIN_TICK_SPACE pixels between ticks
lUnitSize = lNewSize;
rate = urate/iTicksPerUnit;
iNewPerUnit = iTicksPerUnit;
while (rate < FGET(MIN_TICK_SPACE, 1))
	{
	iNewPerUnit /= 2;
	if (iNewPerUnit == 0)
		{
		lUnitSize *= 2;
		urate = FGET((long)iRulerWidth*lUnitSize, lRulerSize);
		rate = urate/iTicksPerUnit;
		if (urate <= 0L || rate <= 0L)  // overflow?
			{
			if (iNewPerUnit)
				iNewPerUnit *= 2;
			else
				iNewPerUnit = 1;
			lUnitSize /= 2;
			urate = FGET((long)iRulerWidth*lUnitSize, lRulerSize);
			rate = urate/iNewPerUnit;
			break;
			}
		iNewPerUnit = iTicksPerUnit;
		}
	rate = urate/iNewPerUnit;
	}

// redistribute relative tick lengths for new number of ticks per unit
if (iNewPerUnit != iTicksPerUnit)
	{
	factor = iTicksPerUnit / iNewPerUnit;
	iTicksPerUnit = iNewPerUnit;
	for (i = 0; i < iTicksPerUnit; ++i)
		ticklength[i] = ticklength[(i*factor)+1];
	}

// start one half in so we don't half to round
offset = FGET(1, 2);

// calculate total number of ticks to draw
num_ticks = (((lRulerSize*(long)iTicksPerUnit)+(lUnitSize/2))/lUnitSize);

// back up the start value to the beginning of the ruler
lBefore = iDispStart - ClientRect.left;
lStartValue -= ((lBefore * lRulerSize)+((long)iRulerWidth/2L)) / (long)iRulerWidth;

// calculate how much each tick is worth in our units, whatever they are
lTickUnit = lUnitSize / iTicksPerUnit;

// start these values for the 0 point on the ruler
lValue = 0;
i = iTicksPerUnit - 1;

// Now we now to found out what the value is at the first
// tick mark to be drawn
if (lStartValue >= 0)  // positive start value - we only go to the right
	{
	while (lValue < lStartValue)
		{
		i++;
		lValue += lTickUnit;
		}
	// at this point we have the value at the first tick to be
	// drawn.  This point may be exactly at the start of the ruler
	// in which case it will draw over the line already drawn
	// at the start of the ruler.
	}
else
	{
	while (lValue >= lStartValue)
		{
		i--;
		lValue -= lTickUnit;
		}
	// at this point we have passed the start of the ruler
	// in the negative direction
	// we must come back one to get the value of the first tick
	// to be drawn.
	lValue += lTickUnit;
	i++;
	}
if (i < 0)
	i = iTicksPerUnit - (abs(i) % iTicksPerUnit);
else
	i = i % iTicksPerUnit;

// i should now contain the tick number of the first tick
// to be drawn. 0...iTicksPerUnit-1
// advance some percentage of the rate in for first tick
offset += (rate*(lValue-lStartValue))/lTickUnit;

// adjust lValue for first value to be displayed
if (lValue >= 0)
	lValue += (lUnitSize-1);
lValue = (lValue/lUnitSize)*lUnitSize;

// bump the total number of ticks to include starting in 'i' ticks
num_ticks += i;

bottom = ClientRect.bottom;
for (; i < num_ticks; ++i)
    {
    x = ClientRect.left + WHOLE(offset);
    offset += rate;
    index = i % iTicksPerUnit;
    if (ticklength[index] < 0)
	top = ClientRect.top;
    else
	top  = bottom - (ticklength[index]*iTickSize);
    MoveTo(hDC, x, top);
    LineTo(hDC, x, bottom);
    if (index == (iTicksPerUnit-1))
	{
	if (lValue < 0)
		num = (lValue-lUnitFactor/2L)/lUnitFactor;
	else
		num = (lValue+lUnitFactor/2L)/lUnitFactor;
	num = lValue/lUnitFactor;
	wsprintf(text, "%d", abs(num));
	GetTextExtentPoint(hDC, text, lstrlen(text), &Extent);
//	x = x - Extent.cx - 1;
	x = x + 1;
//	y = ClientRect.top + ((top-ClientRect.top-info.tmAscent)/2);
	y = ClientRect.top;
        ColorText(hDC, x, y,
	  text, lstrlen(text), GetSysColor(COLOR_WINDOWTEXT));
	lValue += lUnitSize;
	}
    }
SelectObject(hDC, hOldPen);
SelectObject(hDC, hOldFont);
}


/***********************************************************************/
static VOID VertRulerPaint( HDC hDC, HWND hWindow )
/***********************************************************************/
{
RECT ClientRect, WindowRect;
char text[256], achar[10];
int iTicksPerUnit, iNewPerUnit, iTickSize, factor, iDispStart, iRulerHeight;
int x, y, num_ticks, index, i, right, left, j, num, min_label_space;
int ticklength[MAX_TICKS];
long lRulerSize, lUnitSize, lUnitFactor, lNewSize;
long lValue, lStartValue, lTickUnit, lBefore;
LFIXED rate, urate, offset;
HPEN hOldPen;
HFONT hOldFont;
TEXTMETRIC info;
SIZE Extent;
LPRULER lpData;

lpData = (LPRULER)GetWindowLong(hWindow, GWL_CTLDATAPTR);
GetRulerClientRect( hWindow, &ClientRect );
GetRulerWindowRect( hWindow, &WindowRect );

// retrieve the numbers of ticks per unit and bound
iTicksPerUnit = bound(lpData->iTicksPerUnit, 1, MAX_TICKS);

// retrieve the relative lengths of the tick marks
for (i = 0; i < iTicksPerUnit; ++i)
	ticklength[i] = lpData->iTickLength[i];

// retrieve all other parameters
iTickSize = lpData->iTickSize;
lUnitSize = lpData->lUnitSize;
lRulerSize = lpData->lRulerSize;
lUnitFactor = lpData->lUnitFactor;
lStartValue = lpData->lStartValue;

// start to the left of the first pixel
iDispStart = lpData->iDispStart - 1;

// draw right line of ruler and top start line
hOldPen = (HPEN)SelectObject(hDC, GetStockObject(BLACK_PEN));
if (!Control.fNewLook)
	{
	MoveTo(hDC, ClientRect.right, ClientRect.top);
	LineTo(hDC, ClientRect.right, ClientRect.bottom+1);
	}
ClientRect.top = WindowRect.top + (RectWidth(&WindowRect)-1);
MoveTo(hDC, ClientRect.left, ClientRect.top);
LineTo(hDC, ClientRect.right+1, ClientRect.top);

// use small label font and get metrics
hOldFont = (HFONT)SelectObject(hDC, GetStockObject(Window.iLabelFont) );
GetTextMetrics(hDC, &info);
GetTextExtentPoint(hDC, szMinLabel, lstrlen(szMinLabel), &Extent);
min_label_space = Extent.cy * lstrlen(szMinLabel);
min_label_space = max(min_label_space, Extent.cx);
min_label_space += MIN_LABEL_EXTRA;

// calculate the rate to advance to next unit label
iRulerHeight = RectHeight(&ClientRect);
urate = FGET((long)iRulerHeight*lUnitSize, lRulerSize);
if (urate <= 0L)
	{
	SelectObject(hDC, hOldFont);
	SelectObject(hDC, hOldPen);
	return;
	}

// check to make sure we have MIN_LABEL_SPACE pixels between labels
lNewSize = lUnitSize;
while (urate < FGET(min_label_space, 1))
	{
	urate = FGET((long)iRulerHeight*lNewSize*5L, lRulerSize);
	rate = urate/10;
	if (urate <= 0L || rate <= 0L)  // overflow?
		{
		urate = FGET((long)iRulerHeight*lNewSize, lRulerSize);
		break;
		}
	lNewSize *= 5L;
	iTicksPerUnit = 10;
	for (i = 0; i < 5; ++i)
		ticklength[i*2] = 1;	
	for (i = 0; i < 5; ++i)
		ticklength[(i*2)+1] = 2;
	ticklength[iTicksPerUnit-1] = -1;
	iTickSize = 3;
	}

// make sure we have MIN_TICK_SPACE pixels between ticks
lUnitSize = lNewSize;
rate = urate/iTicksPerUnit;
iNewPerUnit = iTicksPerUnit;
while (rate < FGET(MIN_TICK_SPACE, 1))
	{
	iNewPerUnit /= 2;
	if (iNewPerUnit == 0)
		{
		lUnitSize *= 2;
		urate = FGET((long)iRulerHeight*lUnitSize, lRulerSize);
		rate = urate / iTicksPerUnit;
		if (urate <= 0L || rate <= 0L)  // overflow?
			{
			if (iNewPerUnit)
				iNewPerUnit *= 2;
			else
				iNewPerUnit = 1;
			lUnitSize /= 2;
			urate = FGET((long)iRulerHeight*lUnitSize, lRulerSize);
			rate = urate / iNewPerUnit;
			break;
			}
		iNewPerUnit = iTicksPerUnit;
		}
	rate = urate/iNewPerUnit;
	}

// redistribute relative tick lengths for new number of ticks per unit
if (iNewPerUnit != iTicksPerUnit)
	{
	factor = iTicksPerUnit / iNewPerUnit;
	iTicksPerUnit = iNewPerUnit;
	for (i = 0; i < iTicksPerUnit; ++i)
		ticklength[i] = ticklength[(i*factor)+1];
	}

// start one half in so we don't half to round
offset = FGET(1, 2);

// calculate total number of ticks to draw
num_ticks = (((lRulerSize*(long)iTicksPerUnit)+(lUnitSize/2))/lUnitSize);

// back up the start value to the beginning of the ruler
lBefore = iDispStart - ClientRect.top;
lStartValue -= ((lBefore * lRulerSize)+((long)iRulerHeight/2L)) / (long)iRulerHeight;

// calculate how much each tick is worth in our units, whatever they are
lTickUnit = lUnitSize / iTicksPerUnit;

// start these values for the 0 point on the ruler
lValue = 0;
i = iTicksPerUnit - 1;

// Now we now to found out what the value is at the first
// tick mark to be drawn
if (lStartValue >= 0)  // positive start value - we only go to the right
	{
	while (lValue < lStartValue)
		{
		i++;
		lValue += lTickUnit;
		}
	// at this point we have the value at the first tick to be
	// drawn.  This point may be exactly at the start of the ruler
	// in which case it will draw over the line already drawn
	// at the start of the ruler.
	}
else
	{
	while (lValue >= lStartValue)
		{
		i--;
		lValue -= lTickUnit;
		}
	// at this point we have passed the start of the ruler
	// in the negative direction
	// we must come back one to get the value of the first tick
	// to be drawn.
	lValue += lTickUnit;
	i++;
	}
if (i < 0)
	i = iTicksPerUnit - (abs(i) % iTicksPerUnit);
else
	i = i % iTicksPerUnit;

// i should now contain the tick number of the first tick
// to be drawn. 0...iTicksPerUnit-1
// advance some percentage of the rate in for first tick
offset += (rate*(lValue-lStartValue))/lTickUnit;

// adjust lValue for first value to be displayed
if (lValue >= 0)
	lValue += (lUnitSize-1);
lValue = (lValue/lUnitSize)*lUnitSize;

// bump the total number of ticks to include starting in 'i' ticks
num_ticks += i;

right = ClientRect.right;
for (; i < num_ticks; ++i)
    {
    y = ClientRect.top + WHOLE(offset);
    offset += rate;
    index = i % iTicksPerUnit;
    if (ticklength[index] < 0)
	left = ClientRect.left;
    else
	left = right - (ticklength[index]*iTickSize);
    MoveTo(hDC, left, y);
    LineTo(hDC, right, y);
    if (index == (iTicksPerUnit-1))
	{
	if (lValue < 0)
		num = (lValue-lUnitFactor/2L)/lUnitFactor;
	else
		num = (lValue+lUnitFactor/2L)/lUnitFactor;
	wsprintf(text, "%d", abs(num));
	GetTextExtentPoint(hDC, text, lstrlen(text), &Extent);
//	x = left - Extent.cx -3;
//	if (x <= ClientRect.left)
//	    x = ClientRect.left+1;
	x = ClientRect.left + 2;
//	y -= ((info.tmAscent+1));
	y += 0;
	achar[1] = '\0';
	for (j = 0; j < lstrlen(text); ++j)
		{
		achar[0] = text[j];
        	ColorText(hDC, x, y,
	  achar, 1, GetSysColor(COLOR_WINDOWTEXT));
		y += (info.tmAscent-1);
		}
	lValue += lUnitSize;
	}
    }
SelectObject(hDC, hOldPen);
SelectObject(hDC, hOldFont);
}


/***********************************************************************/
static void PaintRulerTick( HDC hDC, HWND hWnd, int x, int y )
/***********************************************************************/
{
DWORD dwStyle;
RECT ClientRect;
HPEN hOldPen;
short OldROP;

GetRulerClientRect(hWnd, &ClientRect);
OldROP = SetROP2(hDC, R2_NOTXORPEN);
hOldPen = (HPEN)SelectObject(hDC, GetStockObject(BLACK_PEN));
dwStyle = GetWindowLong(hWnd, GWL_STYLE);
if (dwStyle & RS_HORZ)
	{
	ClientRect.left += (RectHeight(&ClientRect)-1);
	if (x > ClientRect.left && y > ClientRect.bottom)
		{
		MoveTo(hDC, x, ClientRect.top);
		LineTo(hDC, x, ClientRect.bottom);
		}
	}
else if (dwStyle & RS_VERT)
	{
	ClientRect.top += (RectWidth(&ClientRect)-1);
	if (y > ClientRect.top && x > ClientRect.right)
		{
		MoveTo(hDC, ClientRect.left, y);
		LineTo(hDC, ClientRect.right, y);
		}
	}
SelectObject(hDC, hOldPen);
SetROP2(hDC, OldROP);
}


