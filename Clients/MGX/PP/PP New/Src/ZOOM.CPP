// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "ppafx.h"
#include "svrview.h"
#include "ipframe.h"
#include <limits.h>
#include "id.h"
#include "ctxhelp.h"

// Mini Caption Bar Constants
#define TITLE_HEIGHT	    9
#define BORDER_WIDTH	    1
#define SYS_MENU_WIDTH	    12
#define SYS_COOKIE_OFFSET	3
#define SYS_COOKIE_WIDTH	7
#define SYS_COOKIE_HEIGHT	4
#define COLOR_SYSMENU		RGB(192, 192, 192)
#define COLOR_SYSSHADOW		RGB(128, 128, 128)
#define COLOR_COOKIEFILL	RGB(255, 255, 255)
#define COLOR_BORDER	    RGB(0,   0,   0)

static zoomdepth = 0;

#define GetZoomOwner(hZoomWindow) \
	(HWND)GetWindowLong(hZoomWindow, GWL_ZOOMOWNER)
#define SetZoomOwner(hZoomWindow, hOwner) \
	SetWindowLong(hZoomWindow, GWL_ZOOMOWNER, (long)hOwner)

// Static prototypes
static void StuffZoomDisplay(HWND hWnd, LPDISPLAY lpDisp);
static void EraseBox(HWND hWnd, HDC hDC);
static void DrawBox(HWND hWnd, HDC hDC);
static void InvertBox(HDC hDC, LPRECT lpBox);
static BOOL FileRectChanged(HWND hWnd);

class CUpdateBoxHook : public CHookHandler
{
public:
    CUpdateBoxHook() : CHookHandler() {};
    virtual void ImageHookProc(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc,
            LPTR lpDst, LPTR lpMsk, LPMASK lpMask) { /*no implementation*/ };
    virtual void DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect); 
protected:
private:
};
CUpdateBoxHook UpdateBoxHook;


/***********************************************************************/
/***********************************************************************/
// IMAGE DOCUMENT IMPLEMENTATION
/***********************************************************************/
/***********************************************************************/

LOCAL BOOL ZoomBox_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
LOCAL void ZoomBox_OnDestroy(HWND hWnd);
LOCAL void ZoomBox_OnClose(HWND hWnd);
LOCAL void ZoomBox_OnPaint(HWND hWnd);
LOCAL void ZoomBox_OnPaletteChanged(HWND hWnd, HWND hwndPaletteChange);
LOCAL void ZoomBox_OnMove(HWND hWnd, int x, int y);
LOCAL void ZoomBox_OnSize(HWND hWnd, UINT state, int cx, int cy);
LOCAL void ZoomBox_OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void ZoomBox_OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void ZoomBox_OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags);
LOCAL void ZoomBox_OnLButtonDblClk(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void ZoomBox_OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags);
LOCAL BOOL ZoomBox_OnSetCursor(HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg);
LOCAL BOOL ZoomBox_OnWindowPosChanging(HWND hWnd, LPWINDOWPOS lpwpos);
LOCAL void ZoomBox_OnGetMinMaxInfo(HWND hWnd, LPMINMAXINFO lpMinMaxInfo);

// Message For Processing the Mini Captions on Zoom Window
#ifdef MINICAPTION
LOCAL UINT ZoomBox_NcHitTest(HWND hWnd,int x, int y);
LOCAL UINT ZoomBox_NcCalcSize(HWND hwnd, BOOL fCalcValidRects, NCCALCSIZE_PARAMS FAR* lpcsp);
LOCAL BOOL ZoomBox_NcActivate(HWND hwnd, BOOL fActive, HWND hwndActDeact, BOOL fMinimized);
LOCAL void ZoomBox_NcLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest);
LOCAL void ZoomBox_NcPaint(HWND hwnd, HRGN hrgn);
#endif // MINICAPTION

#define ZoomBox_DefProc DefWindowProc

static long lAspectX, lAspectY;
static POINT StartPoint;
static RECT SelectRect;
static BOOL fCapture, fSelection;
static BOOL fMarquee;
static HWND hOldFocus;

/***********************************************************************/
LONG WINPROC EXPORT WndZoomBoxProc(HWND hWnd, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_CONTROL_COLORS(hWnd, Dialog_OnCtlColor);

	HANDLE_MSG(hWnd, WM_CREATE, ZoomBox_OnCreate);
	HANDLE_MSG(hWnd, WM_DESTROY, ZoomBox_OnDestroy);
	HANDLE_MSG(hWnd, WM_CLOSE, ZoomBox_OnClose);
	HANDLE_MSG(hWnd, WM_PAINT, ZoomBox_OnPaint);
	HANDLE_MSG(hWnd, WM_PALETTECHANGED, ZoomBox_OnPaletteChanged);
	HANDLE_MSG(hWnd, WM_MOVE, ZoomBox_OnMove);
	HANDLE_MSG(hWnd, WM_SIZE, ZoomBox_OnSize);
	HANDLE_MSG(hWnd, WM_KEYUP, ZoomBox_OnKeyUp);
	HANDLE_MSG(hWnd, WM_LBUTTONDOWN, ZoomBox_OnLButtonDown);
	HANDLE_MSG(hWnd, WM_LBUTTONUP, ZoomBox_OnLButtonUp);
	HANDLE_MSG(hWnd, WM_LBUTTONDBLCLK, ZoomBox_OnLButtonDblClk);
	HANDLE_MSG(hWnd, WM_MOUSEMOVE, ZoomBox_OnMouseMove);
	HANDLE_MSG(hWnd, WM_SETCURSOR, ZoomBox_OnSetCursor);
	HANDLE_MSG(hWnd, WM_WINDOWPOSCHANGING, ZoomBox_OnWindowPosChanging);
	HANDLE_MSG(hWnd, WM_GETMINMAXINFO, ZoomBox_OnGetMinMaxInfo);

#ifdef MINICAPTION
	// Message For Processing the Mini Captions on Zoom Window
	HANDLE_MSG(hWnd, WM_NCHITTEST ,ZoomBox_NcHitTest);
	HANDLE_MSG(hWnd, WM_NCCALCSIZE ,ZoomBox_NcCalcSize);
	HANDLE_MSG(hWnd, WM_NCACTIVATE ,ZoomBox_NcActivate);
	HANDLE_MSG(hWnd, WM_NCLBUTTONDOWN ,ZoomBox_NcLButtonDown);
	HANDLE_MSG(hWnd, WM_NCPAINT ,ZoomBox_NcPaint);
#endif // MINICAPTION

	default:
	return ZoomBox_DefProc( hWnd, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL BOOL ZoomBox_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
LPDISPLAY lpMyDisplay;

hZoomWindow = hWnd;
if ( !(lpMyDisplay = (LPDISPLAY)Alloc((long)sizeof(DISPLAY))) )
	return( (BOOL)-1 );
StuffZoomDisplay(hWnd, lpMyDisplay);
SetImagePtr (hWnd, NULL);
SetDisplayPtr (hWnd, lpMyDisplay);
SetWindowWord( hWnd, GWW_ZOOMBOXLEFT, 0);	// postion in pixels
SetWindowWord( hWnd, GWW_ZOOMBOXTOP, 0);	// ''
SetWindowWord( hWnd, GWW_ZOOMBOXRIGHT, 0);	// ''
SetWindowWord( hWnd, GWW_ZOOMBOXBOTTOM, 0);	// ''
SetDisplayHook(hWnd, &UpdateBoxHook);
return(TRUE);
}

/***********************************************************************/
LOCAL void ZoomBox_OnDestroy(HWND hWnd)
/***********************************************************************/
{
LPDISPLAY lpMyDisplay;

if (lpMyDisplay = (LPDISPLAY)GetDisplayPtr(hWnd) )
	FreeUp((LPTR)lpMyDisplay);
hZoomWindow = NULL;
if (Tool.hRibbon)
	SendMessage( Tool.hRibbon, WM_DOCACTIVATED, 0, 1L );
}

/***********************************************************************/
LOCAL void ZoomBox_OnClose(HWND hWnd)
/***********************************************************************/
{
if(hWnd == hZoomWindow)
	{
	hZoomWindow = NULL;
	Window.fHasZoom = FALSE;
	}
DestroyWindow(hWnd);
//hDlg = AstralDlgGet(IDD_VIEWING);
//CheckDlgButton(hDlg, IDC_CUSTOMVIEW, Window.fHasZoom);
}

/***********************************************************************/
LOCAL void ZoomBox_OnPaint(HWND hWnd)
/***********************************************************************/
{
if ( !SuperPaint( hWnd ) )
	ZoomPaint(hWnd);
}

/***********************************************************************/
LOCAL void ZoomBox_OnPaletteChanged(HWND hWnd, HWND hwndPaletteChange)
/***********************************************************************/
{
InvalidateRect(hWnd, NULL, FALSE);
}

/***********************************************************************/
LOCAL void ZoomBox_OnMove(HWND hWnd, int x, int y)
/***********************************************************************/
{
SetZoomDefPos(x, y);
FORWARD_WM_MOVE(hWnd, x, y, ZoomBox_DefProc);
}

/***********************************************************************/
LOCAL void ZoomBox_OnSize(HWND hWnd, UINT state, int cx, int cy)
/***********************************************************************/
{
ZoomMainViewChanged(NULL);
SetZoomDefSize(cx, cy);
}

/***********************************************************************/
LOCAL void ZoomBox_OnKeyUp(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
if ( vk != VK_ESCAPE )
	return;
if ( !fCapture )
	return;
ReleaseCapture(); fCapture = FALSE;
if (fSelection)
	EndSelection( hWnd, NULL, &SelectRect, SL_BOX | SL_NOLIMIT, YES );
SetFocus(hOldFocus);
EnableMarquee(fMarquee);
}

/***********************************************************************/
LOCAL void ZoomBox_OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
RECT rWin;
HWND hOwner;

if ( fCapture )
	return;
SetCapture( hWnd ); fCapture = TRUE;
hOldFocus= GetFocus();
SetFocus(hWnd);
fMarquee = EnableMarquee(FALSE);
if (!IsOnImage(hWnd, x, y))
	return;
StartPoint.x = x;
StartPoint.y = y;
fSelection = NO;
if ( hOwner = GetZoomOwner(hWnd) )
	{
	GetClientRect(hOwner, &rWin);
	lAspectY = rWin.bottom - rWin.top ;
	lAspectX = rWin.right - rWin.left ;
	}
else
	{
	lAspectY = 1L;
	lAspectX = 1L;
	}
}

/***********************************************************************/
LOCAL void ZoomBox_OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
/***********************************************************************/
{
BOOL fGotaRect;
LPDISPLAY lpMyDisplay;
RECT ClientRect;
HWND hOwner;

if ( !fCapture )
	return;
ReleaseCapture(); fCapture = FALSE;
if (fSelection)
	{
	EndSelection( hWnd, NULL, &SelectRect, SL_BOX|SL_NOLIMIT, YES );
	if (abs(SelectRect.right-SelectRect.left) <= SMALL_MOVEMENT &&
			abs(SelectRect.bottom-SelectRect.top) <= SMALL_MOVEMENT)
		fGotaRect = NO;
	else
		fGotaRect = YES;
	}
else
	{
	fGotaRect = NO;
	SelectRect.left = SelectRect.right = StartPoint.x;
	SelectRect.top = SelectRect.bottom = StartPoint.y;
	Display2File( hWnd, (LPINT)&SelectRect.left, (LPINT)&SelectRect.top );
	Display2File( hWnd, (LPINT)&SelectRect.right, (LPINT)&SelectRect.bottom );
	}

// clip to file rect
lpMyDisplay = GetDisplayPtr(hWnd);
ClientRect = lpMyDisplay->FileRect;
AstralIntersectRect( &SelectRect, &SelectRect, &ClientRect );
SetFocus(hOldFocus);
EnableMarquee(fMarquee);
hOwner = GetZoomOwner(hWnd);
if (!hOwner)
	return;
if (IsIconic(hOwner))
	return;
SaveLastView(hOwner);
if ( fGotaRect )
	ViewRect(hOwner, &SelectRect, NO);
else
	{
	Zoom( hOwner, SelectRect.left, SelectRect.top,
		( SHIFT ? -100 : ( CONTROL ? +100 : 0 ) ), YES,NO);
	}
}

/***********************************************************************/
LOCAL void ZoomBox_OnLButtonDblClk(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
HWND hOwner;

hOwner = GetZoomOwner(hWnd);
if (!hOwner)
	return;
if (!IsIconic(hOwner))
	ViewAll(hOwner);
}

/***********************************************************************/
LOCAL void ZoomBox_OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
/***********************************************************************/
{
BOOL fRButton;
POINT pt;

if ( !fCapture )
	return;

fRButton = keyFlags & MK_RBUTTON;
if (!fSelection)
	{
	if (!IsOnImage(hWnd, x, y))
		return;
	if (fSelection = (abs(StartPoint.x-x) > SMALL_MOVEMENT ||
		 abs(StartPoint.y-y) > SMALL_MOVEMENT))
		{
		AstralUpdateWindow(hWnd);
		AstralSetRectEmpty( &SelectRect );
		StartSelection( hWnd, NULL, &SelectRect,
			SL_BOX|SL_SPECIAL|SL_NOLIMIT, StartPoint, 0L );
		}
	}
if (fSelection)
	{
	pt.x = x; pt.y = y;
	UpdateSelection( hWnd, NULL, &SelectRect, SL_BOX|SL_NOLIMIT,
		pt, CONSTRAINASPECT, lAspectX, lAspectY, MOVEKEY||fRButton,FROMCENTER);
	}
}

/************************************************************************/
LOCAL BOOL ZoomBox_OnSetCursor(HWND hWnd, HWND hWndCursor, UINT codeHitTest,
							UINT msg)
/************************************************************************/
{
    CPoint pt;
    HWND hChild;

    // check for disabled control
    GetCursorPos( &pt );
    if( hChild = MyChildWindowFromPoint( hWndCursor, pt ))
    {
        hWndCursor = hChild;
    }

    if ( SetupCursor( hWndCursor, codeHitTest, IDD_ZOOMBOX ))
	    return( TRUE );

    return FORWARD_WM_SETCURSOR(hWnd, hWndCursor, codeHitTest, msg,
							    ZoomBox_DefProc);
}

/***********************************************************************/
LOCAL BOOL ZoomBox_OnWindowPosChanging(HWND hWnd, LPWINDOWPOS lpWinPos)
/***********************************************************************/
{
HWND hOwner;
int w, h, x, y;

hOwner = GetZoomOwner(hWnd);
if (!hOwner)
	return(0);
w = lpWinPos->cx;
h = lpWinPos->cy;
x = lpWinPos->x;
y = lpWinPos->y;
ZoomBoundRect(&x,&y,&w,&h);
lpWinPos->cx = w;
lpWinPos->cy = h;
lpWinPos->x  = x;
lpWinPos->y  = y;
lpWinPos->flags = lpWinPos->flags | SWP_DRAWFRAME; // |SWP_NOZORDER;
return(0);
}

/***********************************************************************/
LOCAL void ZoomBox_OnGetMinMaxInfo(HWND hWnd, LPMINMAXINFO lpMinMaxInfo)
/***********************************************************************/
{
ZoomBoundSize(lpMinMaxInfo);
}

/***********************************************************************/
BOOL NewZoomWindow(HWND hParentWnd)
/***********************************************************************/
{
DWORD dwExtStyle = 0;
if (hParentWnd == NULL)
	return (FALSE);

if (hZoomWindow)				 	// should never happen
	return(TRUE);

LPTSTR lpCaptionStr;
if (!AstralStr(IDS_CAPTIONQZOOM,&lpCaptionStr))
	lpCaptionStr = _T("QuickZoom");

if (Control.fNewLook)
	dwExtStyle |= WS_EX_SMCAPTION;

hZoomWindow = CreateWindowEx(dwExtStyle,
			   _T("ppzoom"),
			   lpCaptionStr,
			   WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | 
			   WS_BORDER | WS_CHILD | WS_CLIPSIBLINGS,
			   100,
			   100,
			   200,
			   200,
			   hParentWnd,
			   NULL,
               PictPubApp.GetResourceHandle(),
			   NULL);


if (!hZoomWindow)
	return(FALSE);

SetWindowPos( hZoomWindow,
	0,
	0, 0, /* Same location */
	0, 0, /* Same size */
	SWP_NOMOVE | SWP_NOSIZE /* Flags */ );

DelUnusedSysMenuItems(hZoomWindow, FALSE, TRUE);
return(TRUE);
}

/***********************************************************************/
void StuffZoomDisplay( HWND hWnd, LPDISPLAY lpDisp )
/***********************************************************************/
{
clr ((LPTR)lpDisp, sizeof(DISPLAY));
lpDisp->ResizeType = SIZE_RESTORED;
}

//************************************************************************
//	HookZoom
//		hooks the zoom window into the current image.
//		If there is no zoom window one is created.
//		If there is No acive window, or no active Image then
//			it destroys itself;
//		Returns true if zoom window is successfuly hooked.
//************************************************************************/
BOOL HookZoom()
//************************************************************************/
{
	LPIMAGE lpNewImage;
	BOOL fHide;
	CServerView *pView = PictPubApp.GetActiveView();
 	HWND hZoomParentWnd = PictPubApp.Get_hWndAstral();;

	if (!Window.fHasZoom)
		return (FALSE);

	if (pView && pView->GetDocument() && pView->GetDocument()->m_lpImage)
	{
		lpNewImage = pView->GetDocument()->m_lpImage;
    	if (pView->GetDocument()->IsInPlaceActive())
			hZoomParentWnd = ((CInPlaceFrame*)pView->GetParentFrame())->GetClientFrameWnd();
	}
	else
		lpNewImage = NULL;

	if (!hZoomWindow && pView->GetSafeHwnd()) // try to create zoom window
	{
		if (!NewZoomWindow( hZoomParentWnd ))
		{
			Window.fHasZoom = FALSE;
			return (FALSE);
		}
	}

	if (pView->GetSafeHwnd() == NULL && hZoomWindow) // Must destroy itself
	{
		DestroyWindow(hZoomWindow);
		hZoomWindow = NULL;
		return(FALSE);
	}

	if (pView->GetSafeHwnd() == GetZoomOwner(hZoomWindow))
		return(TRUE);

 	fHide = (pView->GetSafeHwnd() != PictPubApp.Get_hActiveWindow());
	ShowWindow(hZoomWindow, SW_HIDE);

	SetZoomOwner(hZoomWindow, pView->GetSafeHwnd());

    SetImagePtr (hZoomWindow, lpNewImage);

	ZoomSizeToDef(); // recompute size & pos
	ComputeZoomDisp();

	if (!fHide && !IsWindowVisible(hZoomWindow))
		 ShowWindow(hZoomWindow, SW_SHOWNA);

	return (TRUE);
}


/************************************************************************
	UnhookZoom
		Unhooks the zoom window from the specified window.
		if this was its owner then zoom window destroys itself..
************************************************************************/

void UnhookZoom(HWND hWnd)
{
HWND hOwner, hWndMe;

if (!hZoomWindow)
	return;
hOwner = GetZoomOwner(hZoomWindow);
if (hOwner != hWnd || !hOwner)
	return;
hWndMe = hZoomWindow;
DestroyWindow(hWndMe);		// will set hZoomWindow = NULL
hZoomWindow = NULL;
}

//************************************************************************
//  ZoomSizeToDef
//		  Computes and sets size and position on the screen
//				for the new zoom window	according to the defaults
//***********************************************************************/
void ZoomSizeToDef()
{
	LPIMAGE lpMyImage;
	RECT rOwner, rArea, rParent;
	HWND hWndOwner,hParent;
	int x,y,height, width;
	int borderW, borderH;
	STRING szString;

	if (!hZoomWindow)
		return;
    lpMyImage = (LPIMAGE)GetImagePtr (hZoomWindow );
	hWndOwner = GetZoomOwner(hZoomWindow);
	hParent = PictPubApp.Get_hWndAstral();
	if (!hWndOwner || !hParent)
		return;
	if (hZoomWindow && lpMyImage )
	{
		x = View.rZoomRect.left;
		y = View.rZoomRect.top;
		width  = View.rZoomRect.right - View.rZoomRect.left;
		height = View.rZoomRect.bottom - View.rZoomRect.top;
		if (height == 0 || width == 0)
		{ // no previous values
			// use saved values if possible
			GetDefaultString( _T("sizeZoom"), _T("0,0"), szString, sizeof(STRING) );
			AsciiToInt2( szString, &width, &height);
			if (width+height != 0)
			{
				MoveWindow (hZoomWindow,x,y,width, height, FALSE);
				if (RestorePopup( hZoomWindow, _T("xyZoom"), NULL ))
				{
					// Restore popup will change View.rZoomRect
					x = View.rZoomRect.left;
					y = View.rZoomRect.top;
					MoveWindow (hZoomWindow,x,y,width, height, FALSE);
					return;
				}
			}
			// saved values failed, now use defaults
			GetWindowRect(hZoomWindow, &rArea);
			borderW = rArea.right - rArea.left;
			borderH = rArea.bottom - rArea.top;
			GetClientRect(hZoomWindow, &rArea);
			borderW -= rArea.right - rArea.left;
			borderH -= rArea.bottom - rArea.top;
			GetClientRect(hWndOwner, &rOwner);
			GetClientRect(hParent, &rParent);
			height = FMUL(RectHeight(&rOwner),
				View.ZoomDefaultSize)+borderH;
			width = FMUL(RectWidth(&rOwner),
				View.ZoomDefaultSize)+borderW;
			x = rParent.right-width;
			y = rParent.top + borderH;
			if (IsIconic(hWndOwner))
			{
				height = (rOwner.bottom - rOwner.top)+borderH;
				width = (rOwner.right - rOwner.left)+borderW;
			}
		}
		MoveWindow (hZoomWindow,x,y,width, height, FALSE);
 	}
}


/************************************************************************
  ComputeZoomDisp
		Computes and sets values for the lpDisplay.
************************************************************************/
void ComputeZoomDisp()
{
LPDISPLAY lpMyDisplay;
RECT area;
int fileH, fileW, dispH, dispW;
int x, y;
LPIMAGE lpMyImage;


if (!hZoomWindow)
	return;
lpMyDisplay = (LPDISPLAY)GetDisplayPtr(hZoomWindow);
lpMyImage = (LPIMAGE)GetImagePtr (hZoomWindow );
if (!lpMyDisplay)
	return;
GetClientRect(hZoomWindow, &area);

dispH = area.bottom - area.top;
dispW = area.right - area.left;

x = 0;
y = 0;

// set lpDisplay
lpMyDisplay->DispRect.left = x;
lpMyDisplay->DispRect.right = x+dispW-1;
lpMyDisplay->DispRect.top =y ;
lpMyDisplay->DispRect.bottom = y+dispH-1;

fileW = lpMyImage->npix;
fileH = lpMyImage->nlin;

lpMyDisplay->FileRect.left   = x;
lpMyDisplay->FileRect.top	 = y;
lpMyDisplay->FileRect.right  = x + fileW - 1;
lpMyDisplay->FileRect.bottom = y + fileH - 1;

lpMyDisplay->FileRate = FGET(fileW, dispW);

BoundRect( &lpMyDisplay->FileRect, 0, 0, lpMyImage->npix-1, lpMyImage->nlin-1 );
}


/************************************************************************
	SetZoomDefPos
		sets the default starting position of the zoom window.
************************************************************************/
void SetZoomDefPos(int xPos, int yPos)
{
HWND hOwner;
int borderH, borderW;
RECT rArea, rWindow;
POINT pt;

hOwner = GetZoomOwner(hZoomWindow);
if (!IsDoc(hOwner))
	return;
//if (!IsWindowVisible(hOwner))
//	return;
GetWindowRect(hZoomWindow, &rWindow);
GetClientRect(hZoomWindow, &rArea);
pt.x = rArea.left;
pt.y = rArea.top;
ClientToScreen(hZoomWindow, &pt);
borderW = pt.x - rWindow.left;			// just left border
borderH = pt.y - rWindow.top;			// just top border
xPos -= borderW;
yPos -= borderH;

View.rZoomRect.right += xPos - View.rZoomRect.left;
View.rZoomRect.bottom += yPos - View.rZoomRect.top;
View.rZoomRect.left = xPos;
View.rZoomRect.top = yPos;
}

//***********************************************************************
//	SetZoomDefSize
//		sets the default starting size of the zoom window.
//************************************************************************
void SetZoomDefSize(int width, int height)
{
HWND hOwner;
int borderH, borderW;
//int oldWidth, oldHeight;
RECT rArea;

hOwner = GetZoomOwner(hZoomWindow);
if (!IsDoc(hOwner))
	return;
//if (!IsWindowVisible(hOwner))
//	return;
GetWindowRect(hZoomWindow, &rArea);
borderW = rArea.right - rArea.left;
borderH = rArea.bottom - rArea.top;
GetClientRect(hZoomWindow, &rArea);
borderW -= rArea.right - rArea.left;
borderH -= rArea.bottom - rArea.top;
width += borderW;
height += borderH;
//oldWidth = View.rZoomRect.right - View.rZoomRect.left;
//oldHeight = View.rZoomRect.bottom - View.rZoomRect.top;

// always a square
width = Max(height, width);
View.rZoomRect.right = width + View.rZoomRect.left;
View.rZoomRect.bottom = width + View.rZoomRect.top;
}

//***************************************************************************
// A major change in an owner window
// hWnd is the window which changed: NULL for Owner
//***************************************************************************
void ZoomMainViewChanged(HWND hWnd)
//***************************************************************************
{
RECT rUpdate;
HWND hRealOwner;
//BOOL bWinChanged;
//HDC hDC;
//HWND hOldWnd;

if (!hZoomWindow)
	return;
hRealOwner=GetZoomOwner(hZoomWindow);
if (hWnd && hRealOwner != hWnd)
	return;
if (!hRealOwner || IsIconic(hRealOwner))
	return;
	ResizeMoveZoom();
//if (bWinChanged || hWnd == NULL || FileRectChanged(hZoomWindow))
//	{ 										 // Redraw everything
	ComputeZoomDisp();
	GetClientRect(hZoomWindow, &rUpdate);
	InvalidateRect(hZoomWindow, &rUpdate, FALSE);
//	AstralUpdateWindow(hZoomWindow);
//	}
// else
//	{										// Update box
//	SetCurView(hZoomWindow, &hOldWnd);
//	hDC = GetDC(hZoomWindow);
//	EraseBox(hZoomWindow, hDC);
//	DrawBox(hZoomWindow, hDC);
//	ReleaseDC(hZoomWindow, hDC);
//	SetCurView(hOldWnd, NULL);
//	}
}


/***************************************************************************
	ResizeMoveZoom
			checks if the current zoom window size and position are
				 compatable with the	owner window and the image size.
			Returns true if size or position was CHANGED.
			May change size of window, but does not redraw.
***************************************************************************/
BOOL ResizeMoveZoom()
{
LPIMAGE lpMyImage;
HWND	hParent;
RECT 	rZoom;
POINT ptZoomLT;
int 	x,y,h,w,oldheight, oldwidth;
BOOL bRet;


if (!hZoomWindow)
	return(FALSE);

lpMyImage = (LPIMAGE)GetImagePtr (hZoomWindow );
hParent = PictPubApp.Get_hWndAstral();
if (!hParent || !lpMyImage)
	return(FALSE);

GetWindowRect(hZoomWindow, &rZoom);
w = oldwidth = rZoom.right - rZoom.left;
h = oldheight = rZoom.bottom - rZoom.top;
ptZoomLT.x = rZoom.left;
ptZoomLT.y = rZoom.top;
ScreenToClient(hParent, &ptZoomLT);	 //find upper left in owner coords
x = ptZoomLT.x;
y = ptZoomLT.y;

ZoomBoundRect(&x, &y, &w, &h);

if (zoomdepth > 6)
	return(FALSE);
zoomdepth++;
if ( w != oldwidth || h != oldheight ||
		x != ptZoomLT.x ||y != ptZoomLT.y)
	{
	bRet = MoveWindow(hZoomWindow,x,y,oldwidth,oldheight, TRUE);
	//SendMessage(hZoomWindow, WM_NCPAINT, 0, 0L);
	}
zoomdepth--;
return (FALSE);
}


/***************************************************************************
	ZoomBoundRect
		Takes sugested position and size for the zoom window in owner coords.
		Will set the inputs to allowed values.
***************************************************************************/
void ZoomBoundRect(int *x, int *y, int *width, int *height)
{
	LPIMAGE lpMyImage;
	RECT 	rZoom;
	int borderW, borderH, oldwidth, oldheight;
	LFIXED fRatio, fPreposedRatio;

	if (!hZoomWindow)
		return;

    lpMyImage = (LPIMAGE)GetImagePtr (hZoomWindow );
	if (!lpMyImage)
		return;
	// compute size or border
	GetWindowRect(hZoomWindow, &rZoom);
	oldwidth = rZoom.right - rZoom.left;
	oldheight = rZoom.bottom - rZoom.top;
	GetClientRect(hZoomWindow, &rZoom);
	borderW = oldwidth -(rZoom.right - rZoom.left);
	borderH = oldheight -(rZoom.bottom - rZoom.top);

//	GetClientRect(hOwner, &rOwner);
	// be sure the zoom widow is not larger than window
//	*width = Min(*width, rOwner.right - rOwner.left);
//	*height = Min(*height, rOwner.bottom - rOwner.top);

	// be sure aspect ratios are the same
	fPreposedRatio = FGET(*width-borderW, *height-borderH);
	fRatio = FGET(lpMyImage->npix, lpMyImage->nlin);

	// is it good enough?
	oldwidth = WHOLE(fRatio*(*height-borderH));
	if (oldwidth == (*width - borderW))
		return;
	
	if (fPreposedRatio > fRatio)
		{											// width too big
		*width = FMUL(*height-borderH, fRatio);
		*width += borderW;
		}
	else if (fPreposedRatio < fRatio)
		{												// height top big
		*height = FMUL(*width-borderW,
					FGET(lpMyImage->nlin, lpMyImage->npix));
		*height += borderH;
		}

//	*x = Bound (*x, -*width/2, rOwner.right-*width/2);
//	*y = Bound (*y, -*height/2, rOwner.bottom-*height/2);
	return;
}



/***************************************************************************
	ZoomBoundSize
			limits size of the zoom window.
			called when window gets a WM_GETMINMAXINFO msg.
			does not do any actual sizing.
****************************************************************************/
/***************************************************************************/
void ZoomBoundSize( MINMAXINFO FAR *lpInfo )
/***************************************************************************/
{
LPIMAGE lpMyImage;
RECT rArea;
int borderW, borderH;

if (!hZoomWindow)
	return;
lpMyImage = (LPIMAGE)GetImagePtr (hZoomWindow );

if (!lpMyImage)
	return;
GetWindowRect(hZoomWindow, &rArea);
borderW = rArea.right - rArea.left;
borderH = rArea.bottom - rArea.top;
GetClientRect(hZoomWindow, &rArea);
borderW -= rArea.right - rArea.left;
borderH -= rArea.bottom - rArea.top;
lpInfo->ptMinTrackSize.x = Min(32+borderW, lpMyImage->npix+borderW);
lpInfo->ptMinTrackSize.y = Min(32+borderH, lpMyImage->nlin+borderH);
return;
}


/************************************************************************/
BOOL ZoomPaint( HWND hWnd )
/************************************************************************/
{
PAINTSTRUCT ps;
HDC hDC;
RECT rPaint;
LPDISPLAY lpDisplay;
LPIMAGE lpImage;

hDC = BeginPaint(hWnd, &ps);
lpDisplay = (LPDISPLAY)GetDisplayPtr(hWnd);
lpImage = (LPIMAGE)GetImagePtr (hWnd);
if (lpImage && lpDisplay)
	{
	rPaint = ps.rcPaint;
	AstralToWindowsRect(&rPaint);
	lpImage->Paint(hWnd,hDC,&rPaint,NULL); 		// will paint the invert box
	AstralSetRectEmpty(&lpDisplay->UpdateRect);
	}
EndPaint(hWnd, &ps);
return (TRUE);

}

#ifdef MINICAPTION
void CalcSysMenuRect(HWND hWnd, BOOL bClient, CRect &rMenu)
{
	CRect r;

	GetWindowRect(hWnd,&r);
	if (bClient)
		r.OffsetRect(-r.left, -r.top);
	rMenu = r;
	rMenu.bottom = rMenu.top  + TITLE_HEIGHT;
	rMenu.right  = rMenu.left + SYS_MENU_WIDTH;
}
#endif //MINICAPTION

#ifdef MINICAPTION
void CalcTitleRect(HWND hWnd, BOOL bClient, CRect &rTitle)
{
	CRect r;

	GetWindowRect(hWnd,&r);
	if (bClient)
		r.OffsetRect(-r.left, -r.top);
	rTitle = r;
	rTitle.bottom = rTitle.top + TITLE_HEIGHT;
}
#endif //MINICAPTION

#ifdef MINICAPTION
/************************************************************************/
UINT ZoomBox_NcHitTest(HWND hWnd,int x, int y)
/************************************************************************/
{
	CRect    r;
	CPoint pt(x,y);

	CalcSysMenuRect(hWnd,FALSE, r);
	if (r.PtInRect(pt))
		return HTSYSMENU;

	return DefWindowProc(hWnd, WM_NCHITTEST, (WPARAM)0, 
		(LPARAM)MAKELPARAM((int)(x), (int)(y)));
}
#endif //MINICAPTION

#ifdef MINICAPTION
/************************************************************************/
UINT ZoomBox_NcCalcSize(HWND hwnd, BOOL fCalcValidRects, NCCALCSIZE_PARAMS FAR* lpcsp)
/************************************************************************/
{
	if (fCalcValidRects)
	{
		lpcsp->rgrc[0].top    += TITLE_HEIGHT;
		lpcsp->rgrc[0].left   += BORDER_WIDTH;
		lpcsp->rgrc[0].right  -= BORDER_WIDTH;
		lpcsp->rgrc[0].bottom -= BORDER_WIDTH;
		return (0);
	}

	return 1;
}
#endif //MINICAPTION

#ifdef MINICAPTION
/************************************************************************/
BOOL ZoomBox_NcActivate(HWND hwnd, BOOL fActive, HWND hwndActDeact, BOOL fMinimized)
/************************************************************************/
{
   FORWARD_WM_NCPAINT(hwnd, NULL/*hrgn*/, ::SendMessage);
   return(TRUE);
}
#endif //MINICAPTION

#ifdef MINICAPTION
/************************************************************************/
void ZoomBox_NcLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest)
/************************************************************************/
{
	if (codeHitTest == HTSYSMENU)
	{
		return; // do nothing
	}
	DefWindowProc(hwnd, fDoubleClick ? WM_NCLBUTTONDBLCLK : WM_NCLBUTTONDOWN, 
		(WPARAM)(UINT)codeHitTest, MAKELPARAM((x), (y)));
}
#endif //MINICAPTION

#ifdef MINICAPTION
/************************************************************************/
void ZoomBox_NcPaint(HWND hwnd, HRGN hrgn)
/************************************************************************/
{
	CRect  r, rTitle, rMenu, rCaption, rCookie;
	TCHAR Caption[80];


	GetWindowRect(hwnd,&r);
	r.OffsetRect(-r.left, -r.top);
	CalcTitleRect(hwnd,TRUE, rTitle);
	CalcSysMenuRect(hwnd,TRUE, rMenu);
	rCookie.SetRect(
		rMenu.left + SYS_COOKIE_OFFSET + 1,
		rMenu.top  + SYS_COOKIE_OFFSET + 1,
		rMenu.left + SYS_COOKIE_OFFSET + SYS_COOKIE_WIDTH,
		rMenu.top  + SYS_COOKIE_OFFSET + SYS_COOKIE_HEIGHT);

	// Exclude the Caption Bar area and Have default draw
    HDC hDC = ::GetDCEx(hwnd, hrgn,
	    DCX_USESTYLE | DCX_WINDOW | DCX_INTERSECTRGN | DCX_LOCKWINDOWUPDATE);
	int level = ::SaveDC(hDC);
	CRgn rgn1;
	CRgn rgn2;
	rCaption.UnionRect((LPRECT)&rTitle,(LPRECT)&rMenu);
	::ClientToScreen(hwnd,(LPPOINT)&rCaption);
	::ClientToScreen(hwnd,(LPPOINT)&rCaption.right);
	rgn1.FromHandle(hrgn);
	rgn2.CreateRectRgn(rCaption.left,rCaption.top,rCaption.right,rCaption.bottom);
	int ret = rgn2.CombineRgn(&rgn1,&rgn2,RGN_DIFF);
/*
ERROR		    0
NULLREGION	    1
SIMPLEREGION	    2
COMPLEXREGION	    3
*/
	DefWindowProc(hwnd, WM_NCPAINT, (WPARAM)(HRGN)(rgn2.GetSafeHandle()),
		(LPARAM)0L); 
	::RestoreDC(hDC,level);
	::ReleaseDC(hwnd,hDC);


	// Draw the title bar, sys menu cookie, and border	
	CWnd *pWnd = CWnd::FromHandle(hwnd);
	CDC *pDC = pWnd->GetWindowDC(); 
	if (!pDC)
    	return;
	
	CBrush TitleBrush(GetSysColor(COLOR_ACTIVECAPTION));
	CBrush FrameBrush(COLOR_BORDER);
	CBrush MenuBrush(COLOR_SYSMENU);
	CBrush CkBrush(COLOR_COOKIEFILL);
	CBrush CkBrushShadow(COLOR_SYSSHADOW);
	
	pDC->FillRect(rTitle, &TitleBrush);
	pDC->FillRect(rMenu,  &MenuBrush);
	
	pDC->FrameRect(rTitle,  &FrameBrush);
	pDC->FrameRect(r,       &FrameBrush);
	pDC->FrameRect(rMenu,   &FrameBrush);
	pDC->FrameRect(rCookie, &CkBrushShadow);
	rCookie.OffsetRect(-1, -1);
	pDC->FillRect(rCookie,  &CkBrush);
	pDC->FrameRect(rCookie, &FrameBrush);

	CFont fnt;
	int FntHeight = 12;
	if (fnt.CreateFont(FntHeight,0,0,0,FW_NORMAL,FALSE,FALSE,0,0,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE|TMPF_TRUETYPE,NULL))
	{
		pDC->SelectObject(&fnt);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(GetSysColor(COLOR_CAPTIONTEXT));
		if (::GetWindowText(hwnd,Caption,80) == 0)
			lstrcpy(Caption,_T("QuickZoom"));
		pDC->DrawText((LPCSTR)Caption,-1,&rTitle,DT_CENTER|DT_VCENTER);
		fnt.DeleteObject();
	}
	
	pWnd->ReleaseDC(pDC);
}
#endif //MINICAPTION

/************************************************************************/

/************************************************************************/
static void EraseBox( HWND hWnd, HDC hDC )
/************************************************************************/
{
RECT rInvert;

rInvert.left   = GetWindowWord(hWnd, GWW_ZOOMBOXLEFT);
rInvert.top    = GetWindowWord(hWnd, GWW_ZOOMBOXTOP);
rInvert.right  = GetWindowWord(hWnd, GWW_ZOOMBOXRIGHT);
rInvert.bottom = GetWindowWord(hWnd, GWW_ZOOMBOXBOTTOM);

// invert the box
InvertBox(hDC, &rInvert);

SetWindowWord( hWnd, GWW_ZOOMBOXLEFT, 0);
SetWindowWord( hWnd, GWW_ZOOMBOXTOP, 0);
SetWindowWord( hWnd, GWW_ZOOMBOXRIGHT, 0);
SetWindowWord( hWnd, GWW_ZOOMBOXBOTTOM, 0);
}

/************************************************************************/
static void DrawBox( HWND hWnd, HDC hDC )  // Assumes zoom box is current view!
/************************************************************************/
{
RECT rFile, rDraw;
HWND hOwner;
LPDISPLAY lpOwnerDisp, lpMyDisp;

hOwner = GetZoomOwner(hWnd);
if (!hOwner)
	return;
lpOwnerDisp = (LPDISPLAY)GetDisplayPtr(hOwner);
lpMyDisp = (LPDISPLAY)GetDisplayPtr(hWnd);
if (!lpOwnerDisp || !lpMyDisp)
	return;

//rDisp = lpMyDisp->DispRect;
rFile = lpMyDisp->FileRect;

// get owners file rect and convert to our file rect

rDraw.left = lpOwnerDisp->FileRect.left-rFile.left;
rDraw.top= lpOwnerDisp->FileRect.top-rFile.top;
rDraw.right= lpOwnerDisp->FileRect.right-rFile.left;
rDraw.bottom= lpOwnerDisp->FileRect.bottom-rFile.top;


// convert into our display rect
File2DispRect(hWnd, &rDraw, &rDraw);

// limit to display rect
IntersectRect(&rDraw, &rDraw, &(lpMyDisp->DispRect));

//rDraw.left++;
//rDraw.top++;

//old way:fRate = FGET(rDisp.right-rDisp.left, rFile.right-rFile.left);
//rDraw.left = FMUL(rDraw.left,fRate);
//rDraw.right = FMUL(rDraw.right,fRate);
//fRate = FGET(rDisp.bottom-rDisp.top, rFile.bottom-rFile.top);
//rDraw.top = FMUL(rDraw.top,fRate);
//rDraw.bottom = FMUL(rDraw.bottom,fRate);

// Invert the box
InvertBox(hDC, &rDraw);

// remeber box
SetWindowWord( hWnd, GWW_ZOOMBOXLEFT, (WORD)rDraw.left);
SetWindowWord( hWnd, GWW_ZOOMBOXTOP, (WORD)rDraw.top);
SetWindowWord( hWnd, GWW_ZOOMBOXRIGHT, (WORD)rDraw.right);
SetWindowWord( hWnd, GWW_ZOOMBOXBOTTOM, (WORD)rDraw.bottom);
}


/************************************************************************/
static void InvertBox(HDC hDC, LPRECT lpBox)
/************************************************************************/
{
int OldROP;

// invert the box
OldROP = SetROP2(hDC, R2_NOT);
MoveTo( hDC, lpBox->left, lpBox->top );
LineTo( hDC, lpBox->right, lpBox->top );
LineTo( hDC, lpBox->right, lpBox->bottom );
LineTo( hDC, lpBox->left, lpBox->bottom );
LineTo( hDC, lpBox->left, lpBox->top );
// double box
MoveTo( hDC, lpBox->left+1, lpBox->top+1 );
LineTo( hDC, lpBox->right-1, lpBox->top+1 );
LineTo( hDC, lpBox->right-1, lpBox->bottom-1 );
LineTo( hDC, lpBox->left+1, lpBox->bottom-1 );
LineTo( hDC, lpBox->left+1, lpBox->top+1 );
SetROP2(hDC, OldROP);
}


/************************************************************************
 	Floating Graphics Proc
	redraws part of invert box
************************************************************************/

/************************************************************************/
void CUpdateBoxHook::DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect)
/************************************************************************/
{
HRGN hrgnClip;
RECT rOld;

GetClipBox(hDC, &rOld);
if (lpRect)
	hrgnClip = CreateRectRgn(lpRect->left,
							lpRect->top,
							lpRect->right+1,
							lpRect->bottom+1);
else
	hrgnClip = NULL;						// select entire image
SelectClipRgn(hDC, hrgnClip);
DrawBox(hWnd, hDC);
DeleteObject(hrgnClip);
hrgnClip = CreateRectRgnIndirect(&rOld);
SelectClipRgn(hDC, hrgnClip);
DeleteObject(hrgnClip);
}


// Returns true if the file rect is not the Whole image
/************************************************************************/
static BOOL FileRectChanged(HWND hWnd)
/************************************************************************/
{
LPDISPLAY lpMyDisplay;
int fileW, fileH;
LPIMAGE lpMyImage;

lpMyDisplay = (LPDISPLAY)GetDisplayPtr(hZoomWindow);
lpMyImage = (LPIMAGE)GetImagePtr (hZoomWindow );

fileW = lpMyDisplay->FileRect.right - lpMyDisplay->FileRect.left + 1;
fileH = lpMyDisplay->FileRect.bottom - lpMyDisplay->FileRect.top + 1;

if (fileW != lpMyImage->npix || fileH != lpMyImage->nlin)
	return(TRUE);
return(FALSE);
}
