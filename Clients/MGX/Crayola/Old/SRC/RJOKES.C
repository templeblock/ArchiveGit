/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                   rjokes.c

******************************************************************************
******************************************************************************

  Description:  Room dialog functions.
 Date Created:  01/11/94
       Author:  Michael P. Dagate

*****************************************************************************/

#include <windows.h>
#include <math.h>
#include "id.h"
#include "data.h"
#include "routines.h"

/******************************** Private Data ******************************/

static HMGXBRUSH hBrush;
static BOOL bBrushCursor;

/****************************** Private Functions ***************************/

/************************************************************************/
static void SetupPreviewCursor(HWND hControl, BOOL fInit)
/************************************************************************/
{
static HCURSOR hOldCursor;

if (!hControl)
	return;

if (fInit)
	{
	if ( !hOldCursor )
		hOldCursor = (HCURSOR)GET_CLASS_CURSOR( hControl );
	SET_CLASS_CURSOR( hControl, (WORD)Window.hNullCursor );
	bBrushCursor = YES;
	}
else
	{
	if ( hOldCursor )
		SET_CLASS_CURSOR( hControl, (WORD)hOldCursor );
	hOldCursor = NULL;
	bBrushCursor = NO;
	}
}

/************************************************************************/
static void WinBrush( HWND hWnd, HDC hDC, LPRECT lpRect, int flag,
						int fTickMarks, int shape, LFIXED DispRate )
/************************************************************************/
{
int size, size2, size3, y1, y2, x1, x2, dx, dy;
HBRUSH hOldBrush;
short OldROP;

if ( !hDC )
	return;

if (shape == IDC_BRUSHCUSTOM)
	shape = IDC_BRUSHSQUARE;

OldROP = SetROP2(hDC, R2_NOT);
x1 = lpRect->left; y1 = lpRect->top;
x2 = lpRect->right; y2 = lpRect->bottom;
if ( shape == IDC_BRUSHCIRCLE )
	{
	hOldBrush = (HBRUSH)SelectObject( hDC, GetStockObject(NULL_BRUSH) );
	Ellipse( hDC, x1, y1, x2, y2 );
	SelectObject( hDC, hOldBrush );
	}
else
if ( shape == IDC_BRUSHSQUARE  ||
     shape == IDC_BRUSHHORIZONTAL ||
     shape == IDC_BRUSHVERTICAL)
	{
	MoveToEx( hDC, x1, y1, NULL );
	LineTo( hDC, x2, y1 );
	LineTo( hDC, x2, y2 );
	LineTo( hDC, x1, y2 );
	LineTo( hDC, x1, y1 );
	}
else
if ( shape == IDC_BRUSHSLASH )
	{
	size2 = FMUL(2, DispRate);
	size2 = (size2+2)/2;
	size3 = FMUL(3, DispRate);
	size3 = (size3+2)/2;
	MoveToEx( hDC, x2-size2, y1, NULL );
	LineTo( hDC, x2-size2+size3, y1+size3 );
	LineTo( hDC, x1+size2, y2+size2 );
	LineTo( hDC, x1+size2-size3, y2+size2-size3 );
	LineTo( hDC, x2-size2, y1 );
	}
else
if ( shape == IDC_BRUSHBACKSLASH )
	{
	size2 = FMUL(2, DispRate);
	size2 = (size2+2)/2;
	size3 = FMUL(3, DispRate);
	size3 = (size3+2)/2;
	MoveToEx( hDC, x1+size2, y1, NULL );
	LineTo( hDC, x1+size2-size3, y1+size3 );
	LineTo( hDC, x2-size2, y2+size2 );
	LineTo( hDC, x2-size2+size3, y2+size2-size3 );
	LineTo( hDC, x1+size2, y1 );
	}

if ( !fTickMarks )
	{
	SetROP2(hDC, OldROP);
	return;
	}

dx = 1;
dy = 1;
if (shape == IDC_BRUSHCIRCLE)
	{
	size = x2 - x1 + 1;
	dx = size - ((size * 100) / 141);
	size = y2 - y1 + 1;
	dy = size - ((size * 100) / 141);
	if (dx & 1)
		++dx;
	if (dy & 1)
		++dy;
	dx /= 2;
	dy /= 2;
	}
x1 += dy; y1 += dy;
x2 -= dy; y2 -= dy;
MoveToEx( hDC, x1, y1, NULL );
LineTo( hDC, x2, y2 );
MoveToEx( hDC, x2, y1, NULL );
LineTo( hDC, x1, y2 );

SetROP2(hDC, OldROP);
}

/************************************************************************/
static BOOL DisplayPreviewBrush(HWND hWnd, int x, int y, BOOL on )
/************************************************************************/
{
HDC hDC;
static RECT onRect, on2Rect;
static int bx, by;
static BOOL bBrushOn;
static HWND hBrushWnd;
static int ShapeOn;
static int SizeOn;
int HalfSize;

// make sure we are in a valid state
if ( on == bBrushOn || !hBrush)
	return(FALSE);

if (on)
    {
	// special case to redisplay brush at same location it was turned off
    if (x == 32767)
		{
		// get location that the brush was turned off at
		x = bx;
		y = by;
		}
	// save window we turned brush on in
    hBrushWnd = hWnd;
    }
else
    {
    hWnd = hBrushWnd;
    hBrushWnd = 0;
    }
// do we have a window to use?
if (!hWnd)
    return(FALSE);

// save last on state
bBrushOn = on;
if ( on )
	{
	// save current mouse location for turning off brush
	bx = x;
	by = y;

	ShapeOn = GetMgxBrushShape(hBrush);
	SizeOn = GetMgxBrushSize(hBrush);
	HalfSize = SizeOn/2;

	// setup rectangle in file coordinates for brush
	if (ShapeOn == IDC_BRUSHCUSTOM ||
	    ShapeOn == IDC_BRUSHSQUARE ||
	    ShapeOn == IDC_BRUSHCIRCLE ||
	    ShapeOn == IDC_BRUSHSLASH ||
	    ShapeOn == IDC_BRUSHBACKSLASH)
		{
		onRect.left = x - HalfSize;
		onRect.right = onRect.left + SizeOn - 1;
		onRect.top = y - HalfSize;
		onRect.bottom = onRect.top + SizeOn - 1;
		}
	else if (ShapeOn == IDC_BRUSHHORIZONTAL)
		{
		onRect.left = x - HalfSize;
		onRect.right = onRect.left + SizeOn - 1;
		onRect.top = onRect.bottom = y;
		}
	else if (ShapeOn == IDC_BRUSHVERTICAL)
		{
		onRect.left = onRect.right = x;
		onRect.top = y - HalfSize;
		onRect.bottom = onRect.top + SizeOn - 1;
		}
	}
hDC = GetDC(hWnd);
WinBrush( hWnd, hDC, &onRect, bBrushOn, NO, ShapeOn, FONE );
ReleaseDC(hWnd, hDC);
return(TRUE);
}

/************************************************************************/
static void UpdateBrushPreview(HWND hWnd, LPRECT lpUpdateRect, BOOL fUpdateCursor)
/************************************************************************/
{
int x, y;
BOOL fTurnOn;
LPOBJECT lpObject;
RECT rect;

if (fUpdateCursor)
	{
	DisplayPreviewBrush(hWnd, 0, 0, OFF);
	x = lpUpdateRect->left;
	y = lpUpdateRect->top;
	DisplayPreviewBrush(hWnd, x, y, ON);
	}
//else if (lpFrame = ImgGetBaseEditFrame (lpImage))
//	{
//	fTurnOn = DisplayPreviewBrush(hWnd, 0, 0, OFF);
//	hDC = GetDC(hWnd);
//	rSrc.left = rSrc.top = 0;
//	rSrc.right = FrameXSize(lpFrame)-1;
//	rSrc.bottom = FrameYSize(lpFrame)-1;
//	GetClientRect(hWnd, &rDst);
//	WindowsToAstralRect(&rDst);
//	DrawFrame(lpFrame, hDC, &rSrc, &rDst, lpUpdateRect);
//	ReleaseDC(hWnd, hDC);
//	if (fTurnOn)
//		DisplayPreviewBrush(hWnd, 32767, 32767, ON);
//	}
else if (lpObject = (LPOBJECT) GetWindowLong (hWnd,GWL_IMAGE))
    {
	fTurnOn = DisplayPreviewBrush(hWnd, 0, 0, OFF);
    rect = *lpUpdateRect;
    rect.right++;
    rect.bottom++;
    InvalidateRect (hWnd,&rect,FALSE);
    UpdateWindow (hWnd);
	if (fTurnOn)
		DisplayPreviewBrush(hWnd, 32767, 32767, ON);
    }
}

/****************************************************************************/

BOOL FAR JokesToolProc (HWND hWnd,LONG lParam,WORD wMsg)
{
    static BOOL bTrack = FALSE;
    int x,y,fx,fy;
    LPALPHA lpAlpha;
    LPOBJECT lpObject;

    switch (wMsg)
    {
    case WM_ACTIVATE:
        DisplayBrush (hWnd,32767,32767,lParam ? ON : OFF);
        break;
    case WM_LBUTTONDOWN:
		if ( !bBrushCursor )
			break;
        if (bTrack || !hBrush ||
            !(lpObject = ImgGetNextObject (lpImage,NULL,NO,NO)) ||
            !(lpAlpha = lpObject->lpAlpha) ||
            !ImgEditInit (lpImage,ET_OBJECT,UT_ALPHA,lpObject))
            break;
        SetMgxBrushOpacity (hBrush,255);
        if (!MgxBrushBeginEx (hWnd,&lpAlpha->Pixmap,0,0,
            hBrush,YES,YES,UpdateBrushPreview,NULL))
            break;
        ImgEditedObject (lpImage,lpObject,IDS_UNDOPAINTMASK,NULL);
        bTrack = MgxBrushActive();
        SoundStartResource( "Happy01", YES/*bLoop*/, 0/*hInstance*/ );
        // fall through!
    case WM_MOUSEMOVE:
		if ( !bBrushCursor )
			break;
        fx = x = LOWORD (lParam);
        fy = y = HIWORD (lParam);
        Display2File (&fx,&fy);
        DisplayPreviewBrush (hWnd,x,y,OFF);
        if (bTrack)
            MgxBrushStroke (fx,fy,fx,fy,x,y);
        else
            SoundStop();
        DisplayPreviewBrush (hWnd,x,y,ON);
        break;
    case WM_DESTROY:
    case WM_LBUTTONUP:
        MgxBrushEnd (NO);
        SoundStop();
        bTrack = FALSE;
        break;
    case WM_LBUTTONDBLCLK:
        break;
    }
    return TRUE;
}

/****************************** Public Functions ****************************/

BOOL WINPROC EXPORT DlgRoomJokes (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    COLORINFO ColorStruct;
    BOOL bHandled = TRUE;
    char szTemp[MAX_PATH];
    HWND hCtl;
    LPOBJECT lpObject;

    switch (wMsg)
    {
    case WM_COMMAND:
        switch (wParam)
        {
        case HS_COVER:
			StopAnimation();
            hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
            ShowWindow (hCtl,SW_HIDE);
            hCtl = GetDlgItem (hWnd,HS_LOGO_JOKES);
            ShowWindow (hCtl,SW_SHOW);
            hCtl = GetDlgItem (hWnd,HS_XTRA_JOKES);
            ShowWindow (hCtl,SW_HIDE);
            bHandled = FALSE;
            break;
        case HS_TAB1:
        case HS_TAB2:
        case HS_TAB3:
        case HS_TAB4:
        case HS_TAB5:
        case HS_TAB6:
        case HS_TAB7:
        case HS_TAB8:
			StopAnimation();
            wsprintf (szTemp,GetString (IDS_JOKES,NULL),wParam - HS_TAB1 + 1);
            PathCat (szPreview,Control.PouchPath,szTemp);
            AstralImageLoad (IDN_ART,szPreview,MAYBE,NO);
            if (lpImage && (lpObject = ImgGetBase (lpImage)))
            {
                hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
                SetWindowLong (hCtl,GWL_IMAGE,(long)lpObject);
                SetWindowLong (hCtl,GWL_IMAGE_TOOLPROC,(long)JokesToolProc);
				SetupPreviewCursor (hCtl,YES);
                ShowWindow (hCtl,SW_SHOW);
                InvalidateRect (hCtl,NULL,TRUE);
                UpdateWindow (hCtl);
                hCtl = GetDlgItem (hWnd,HS_LOGO_JOKES);
                ShowWindow (hCtl,SW_HIDE);
                hCtl = GetDlgItem (hWnd,HS_XTRA_JOKES);
                ShowWindow (hCtl,SW_SHOW);
                bHandled = FALSE;
            }
			SoundStartID(HS_XTRA_JOKES, FALSE, 0);
            break;
        case HS_COLORME:
			StopAnimation();
            if (lpImage && (lpObject = ImgGetNextObject (lpImage,NULL,NO,NO)))
                ImgDeleteObject (lpImage,lpObject);
            GoPaintApp (hWnd,szPreview);
            break;
        case HS_SHOWME:
			SoundStartResource( "Happy01", NO/*bLoop*/, 0/*hInstance*/ );
            if (lpImage && (lpObject = ImgGetNextObject (lpImage,NULL,NO,NO)))
            {
                lpObject->fHidden = !lpObject->fHidden;
                hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
		        SetupPreviewCursor (hCtl,NO);
                ShowWindow (hCtl,SW_SHOW);
                InvalidateRect (hCtl,NULL,TRUE);
				UpdateWindow(hCtl);
				StartAnimation(hCtl);
            }
            break;
        default:
            bHandled = FALSE;
        }
        break;
    case WM_SETCURSOR:
        hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
        if (hCtl != (HWND)wParam)
            DisplayPreviewBrush (hCtl,0,0,OFF);
        bHandled = SetupCursor (wParam,lParam,0);
        break;
    case WM_INITDIALOG:
		hBrush = (HMGXBRUSH)CreateDefaultBrush( hBrush );
        ColorStruct.ColorSpace = CS_GRAY;
        ColorStruct.gray = 0;
		SetMgxBrushColor( hBrush, &ColorStruct );
        SetMgxBrushSize (hBrush,48);
        bHandled = FALSE;
        break;
    case WM_DESTROY:
		StopAnimation();
		if ( hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE) )
			SetupPreviewCursor (hCtl,NO);
        if (hBrush)
        {
            DestroyMgxBrush (hBrush);
            hBrush = NULL;
        }
        bHandled = FALSE;
        break;
    default:
        bHandled = FALSE;
    }
    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}

/****************************************************************************/

