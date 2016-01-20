// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "ctlutil.h"
#include "slider.h"

#ifdef DLGEDIT
void Control_GetClassInfo(LPSTR lpClassName, LPWORD lpWidth, LPWORD lpHeight, LPDWORD lpStyle)
{
lstrcpy(lpClassName, MGXCTL_SLIDER);
*lpWidth = 40;
*lpHeight = 10;
*lpStyle = SS_HORZSLIDER;
}
BOOL Control_RegisterClass(HINSTANCE hInstance, UINT ClassScope)
{
return(Slider_RegisterClass(hInstance, ClassScope));
}
#define Slider_Control DlgEdit_Control
extern "C"{
LONG WINPROC Slider_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
}
#else
LONG WINPROC Slider_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
#endif

/***********************************************************************/
/***********************************************************************/
// SLIDER CONTROL
/***********************************************************************/
/***********************************************************************/

//************************************************************************
BOOL InitSlide( HWND hDlg, ITEMID idItem, int iValue, int iMin, int iMax )
//************************************************************************
{
SendDlgItemMessage( hDlg, idItem, SM_SETRANGE, 0, MAKELONG(iMin,iMax) );
return( SendDlgItemMessage( hDlg, idItem, SM_SETVALUE, iValue, 0L ) );
}


//************************************************************************
BOOL SetSlide( HWND hDlg, ITEMID idItem, int iValue )
//************************************************************************
{
return( SendDlgItemMessage( hDlg, idItem, SM_SETVALUE, iValue, 0L ) );
}


//************************************************************************
int HandleSlide( HWND hDlg, ITEMID id, UINT codeNotify, LPINT lpBoolCompleted )
//************************************************************************
{
if ( lpBoolCompleted )
	*lpBoolCompleted == ( codeNotify == SN_VALUESELECTED );
return( SendDlgItemMessage( hDlg, id, SM_GETVALUE, 0, 0L ) );
}

#define WINDOW_EXTRA (GWW_START+0)

/* LONG Slider_OnUserMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) */
#define HANDLE_SM_GETRANGE(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)SM_GETRANGE, wParam, lParam))
#define HANDLE_SM_GETTHUMBSIZE(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)SM_GETTHUMBSIZE, wParam, lParam))
#define HANDLE_SM_GETTICKS(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)SM_GETTICKS, wParam, lParam))
#define HANDLE_SM_GETVALUE(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)SM_GETVALUE, wParam, lParam))
#define HANDLE_SM_SETRANGE(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)SM_SETRANGE, wParam, lParam))
#define HANDLE_SM_SETSTEP(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)SM_SETSTEP, wParam, lParam))
#define HANDLE_SM_SETTHUMBSIZE(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)SM_SETTHUMBSIZE, wParam, lParam))
#define HANDLE_SM_SETTICKS(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)SM_SETTICKS, wParam, lParam))
#define HANDLE_SM_SETVALUE(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)SM_SETVALUE, wParam, lParam))

LOCAL BOOL Slider_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct);
LOCAL void Slider_OnDestroy(HWND hWindow);
LOCAL BOOL Slider_OnEraseBkgnd(HWND hWindow, HDC hDC);
LOCAL void Slider_OnPaint(HWND hWindow);
LOCAL UINT Slider_OnGetDlgCode(HWND hWindow, LPMSG lpMsg);
LOCAL void Slider_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Slider_OnKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Slider_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Slider_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Slider_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Slider_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Slider_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Slider_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Slider_OnSize(HWND hWindow, UINT state, int cx, int cy);
LOCAL void Slider_OnTimer(HWND hWindow, UINT id);
LOCAL long Slider_OnUserMsg (HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
LOCAL void Slider_InitInfo (HWND hWindow, LPSLIDERINFO lpInfo, BOOL bUpdate);
LOCAL LPSLIDERINFO Slider_GetInfo(HWND hWindow);
LOCAL void Slider_Draw(HWND hWindow, HDC hDC);
LOCAL void Slider_DrawBackground (HWND hWindow, HDC hDC, LPSLIDERINFO lpInfo);
LOCAL void Slider_DrawTicks (HWND hWindow, HDC hDC, LPSLIDERINFO lpInfo);
LOCAL void Slider_DrawThumb (HWND hWindow, HDC hDC, LPSLIDERINFO lpInfo);
LOCAL void Slider_PointFromValue (HWND hWindow,int  Value,LPPOINT lpPosition,LPSLIDERINFO lpInfo);
LOCAL int Slider_ValueFromPoint (HWND hWindow, POINT ptPosition,LPSLIDERINFO lpInfo);
LOCAL void Slider_ClipPoint (LPPOINT lpPosition, LPSLIDERINFO lpInfo);
LOCAL BOOL Slider_DecrementValue ( LPSLIDERINFO lpInfo);
LOCAL BOOL Slider_IncrementValue (LPSLIDERINFO lpInfo);
LOCAL BOOL Slider_IsOnThumb (POINT ptPosition,LPSLIDERINFO lpInfo);

static int BitmapWidth = 0;
static int BitmapHeight = 0;
static HBITMAP hBitmap = NULL;
static int nControls = 0;

BOOL Slider_RegisterClass (
HINSTANCE hInstance, UINT ClassScope)
{
	WNDCLASS WndClass;

	/* Setup and define the window class labeled frame control */
	WndClass.hIcon		= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= MGXCTL_SLIDER;
	WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
	WndClass.hbrBackground	= (HBRUSH)GetStockObject(LTGRAY_BRUSH);
//	WndClass.hbrBackground	= NULL;
	WndClass.hInstance	= hInstance;
	WndClass.style		= CS_DBLCLKS;
//	WndClass.style		= CS_DBLCLKS|ClassScope;
	WndClass.lpfnWndProc	= Slider_Control;
	WndClass.cbClsExtra	= 0;
	WndClass.cbWndExtra	= WINDOW_EXTRA;

	return(RegisterClass( &WndClass ));
}

/***********************************************************************/
#ifdef DLGEDIT
extern "C" {
#endif
LONG WINPROC EXPORT Slider_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_CREATE, Slider_OnCreate);
	HANDLE_MSG(hWindow, WM_DESTROY, Slider_OnDestroy);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Slider_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_PAINT, Slider_OnPaint);
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Slider_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_KEYDOWN, Slider_OnKeyDown);
	HANDLE_MSG(hWindow, WM_KEYUP, Slider_OnKeyUp);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, Slider_OnKillFocus);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Slider_OnLButtonDblClk);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Slider_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, Slider_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Slider_OnMouseMove);
	HANDLE_MSG(hWindow, WM_SETFOCUS, Slider_OnSetFocus);
	HANDLE_MSG(hWindow, WM_SIZE, Slider_OnSize);
	HANDLE_MSG(hWindow, WM_TIMER, Slider_OnTimer);
	HANDLE_MSG(hWindow, SM_GETRANGE, Slider_OnUserMsg);
	HANDLE_MSG(hWindow, SM_GETTHUMBSIZE, Slider_OnUserMsg);
	HANDLE_MSG(hWindow, SM_GETTICKS, Slider_OnUserMsg);
	HANDLE_MSG(hWindow, SM_GETVALUE, Slider_OnUserMsg);
	HANDLE_MSG(hWindow, SM_SETRANGE, Slider_OnUserMsg);
	HANDLE_MSG(hWindow, SM_SETSTEP, Slider_OnUserMsg);
	HANDLE_MSG(hWindow, SM_SETTHUMBSIZE, Slider_OnUserMsg);
	HANDLE_MSG(hWindow, SM_SETTICKS, Slider_OnUserMsg);
	HANDLE_MSG(hWindow, SM_SETVALUE, Slider_OnUserMsg);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}
#ifdef DLGEDIT
}
#endif

/***********************************************************************/
LOCAL BOOL Slider_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
    LPSLIDERINFO lpInfo = (LPSLIDERINFO)Ctl_Alloc((long)sizeof(SLIDERINFO));

    nControls++;
    if (lpInfo)
    {
        Slider_InitInfo (hWindow,lpInfo,FALSE);
    }
	 SetWindowLong(hWindow, GWL_CTLDATAPTR, (long)lpInfo);
	 return(TRUE);
}

/***********************************************************************/
LOCAL void Slider_OnDestroy(HWND hWindow)
/***********************************************************************/
{
    LPSLIDERINFO lpInfo = Slider_GetInfo (hWindow);

    if (--nControls == 0 && hBitmap)
    {
        DeleteObject (hBitmap);
        BitmapWidth = 0; 
        BitmapHeight = 0;
        hBitmap = NULL;
    }
	if (lpInfo)
	{
		Ctl_FreeUp(lpInfo);
	}
}

/***********************************************************************/
LOCAL BOOL Slider_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
return(TRUE);
}

/***********************************************************************/
LOCAL void Slider_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC hDC, hMemDC;
    RECT	rc;
	HBITMAP	hOldBitmap;

	hDC = BeginPaint( hWindow, &ps );
    if (IsWindowVisible (hWindow) && hBitmap)
    { 
        hMemDC = CreateCompatibleDC (hDC);

        if (hMemDC)
        {
            hOldBitmap = (HBITMAP)SelectObject (hMemDC,hBitmap);

            Slider_Draw (hWindow, hMemDC);
            GetClientRect (hWindow,&rc);
            BitBlt (hDC,0,0,rc.right,rc.bottom,hMemDC,0,0,SRCCOPY);
            SelectObject (hMemDC,hOldBitmap);
            DeleteDC (hMemDC);  
        }
    }

	EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL UINT Slider_OnGetDlgCode(HWND hWindow, LPMSG lpMsg)
/***********************************************************************/
{
return( DLGC_WANTARROWS );
}

/***********************************************************************/
LOCAL void Slider_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
    LPSLIDERINFO lpInfo = Slider_GetInfo (hWindow);

    if (lpInfo)
    {
        int OldValue = lpInfo->Value;

        if (lpInfo->lStyle & SS_HORZSLIDER)
        {

            if ((vk == VK_LEFT && !(lpInfo->lStyle & SS_RIGHTTOLEFT)) ||
                (vk == VK_RIGHT && lpInfo->lStyle & SS_RIGHTTOLEFT))
                Slider_DecrementValue (lpInfo);
            else if ((vk == VK_RIGHT && !(lpInfo->lStyle & SS_RIGHTTOLEFT)) ||
                (vk == VK_LEFT && lpInfo->lStyle & SS_RIGHTTOLEFT))
                Slider_IncrementValue (lpInfo);
        }
        else
        {
            if ((vk == VK_UP && !(lpInfo->lStyle & SS_RIGHTTOLEFT)) ||
                (vk == VK_DOWN && lpInfo->lStyle & SS_BOTTOMTOTOP))
               Slider_DecrementValue (lpInfo);
            else if ((vk == VK_DOWN && !(lpInfo->lStyle & SS_RIGHTTOLEFT)) ||
                (vk == VK_UP && lpInfo->lStyle & SS_BOTTOMTOTOP))
                Slider_IncrementValue (lpInfo);
        }
        if (vk == VK_NEXT)
            Slider_DecrementValue (lpInfo);
        else if (vk == VK_PRIOR)
            Slider_IncrementValue (lpInfo);
        else if (vk == VK_END)
            lpInfo->Value = lpInfo->Max;
        else if (vk == VK_HOME)
            lpInfo->Value = lpInfo->Min;
        Slider_PointFromValue (hWindow,lpInfo->Value,&lpInfo->ptPosition,lpInfo);
        if (OldValue != lpInfo->Value)
        {
		 	FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
								hWindow, SN_VALUECHANGING, SendMessage);
            InvalidateRect (hWindow,NULL,FALSE);
            UpdateWindow (hWindow);
        }
    }
}

/***********************************************************************/
LOCAL void Slider_OnKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
    LPSLIDERINFO lpInfo = Slider_GetInfo (hWindow);

    if (lpInfo)
    {
        if (((lpInfo->lStyle & SS_HORZSLIDER) ? 
            (vk == VK_LEFT || vk == VK_RIGHT) :
            (vk == VK_UP || vk == VK_DOWN)) ||
            vk == VK_NEXT || vk == VK_PRIOR ||
            vk == VK_END  || vk == VK_HOME)
		 	FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
								hWindow, SN_VALUESELECTED, SendMessage);
    }
}

/***********************************************************************/
LOCAL void Slider_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
    LPSLIDERINFO lpInfo = Slider_GetInfo (hWindow);

    if (lpInfo)
    {
        if (lpInfo->bCaptured)
        {
            ReleaseCapture ();
            lpInfo->bCaptured = FALSE;
        }
        lpInfo->bFocus = FALSE;
        InvalidateRect (hWindow,NULL,FALSE);
    }
}

/***********************************************************************/
LOCAL void Slider_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
 	FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
						hWindow, CTLN_DOUBLECLICKED, SendMessage);
}

/***********************************************************************/
LOCAL void Slider_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
 	POINT ptPosition;
	int Value;
    LPSLIDERINFO lpInfo = Slider_GetInfo (hWindow);

	ptPosition.x = x; ptPosition.y = y;

    if (lpInfo)
    {
        BOOL bPaint = FALSE;

        SetCapture (hWindow);
        lpInfo->bCaptured = TRUE;
        if (!(lpInfo->lStyle & SS_NOMOUSEFOCUS))
            SetFocus (hWindow);
        if (!Slider_IsOnThumb (ptPosition,lpInfo) && (lpInfo->lStyle & SS_CLICKSTEP))
        {
           Value = Slider_ValueFromPoint (hWindow,ptPosition,lpInfo);

           lpInfo->bStepping = TRUE;
           if (Value < lpInfo->Value)
               bPaint = Slider_DecrementValue (lpInfo);
           else if (Value > lpInfo->Value)
               bPaint = Slider_IncrementValue (lpInfo);
           Slider_PointFromValue (hWindow,lpInfo->Value,&lpInfo->ptPosition,lpInfo);
           SetTimer (hWindow,1,150,NULL);
        }
        else
        {
            Value = lpInfo->Value;

            bPaint = TRUE;
            Slider_ClipPoint (&ptPosition,lpInfo);
            lpInfo->Value = Slider_ValueFromPoint (hWindow,ptPosition,lpInfo);
            if (lpInfo->lStyle & SS_SNAPDRAG)
            {
                Slider_PointFromValue (hWindow,lpInfo->Value,&lpInfo->ptPosition,lpInfo);
                bPaint = lpInfo->Value != Value;
            }
            else if (lpInfo->lStyle & SS_HORZSLIDER &&
                ptPosition.x != lpInfo->ptPosition.x)
                lpInfo->ptPosition.x = ptPosition.x;
            else if (ptPosition.y != lpInfo->ptPosition.y)
                lpInfo->ptPosition.y = ptPosition.y;
            else
                bPaint = FALSE;
        }
        if (bPaint)
        {
		 	FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
								hWindow, SN_VALUECHANGING, SendMessage);
            InvalidateRect (hWindow,NULL,FALSE);
            UpdateWindow (hWindow);
        }
    }
}

/***********************************************************************/
LOCAL void Slider_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
 	POINT	ptPosition;
	BOOL	bPaint;
    LPSLIDERINFO lpInfo = Slider_GetInfo (hWindow);

	ptPosition.x = x; ptPosition.y = y;

    if (lpInfo && lpInfo->bCaptured)
    {
        ReleaseCapture ();
        lpInfo->bCaptured = FALSE;
        if (lpInfo->bStepping)
        {
            lpInfo->bStepping = FALSE;
            KillTimer (hWindow,1);
        }
        else
        {
            bPaint = TRUE;

            Slider_ClipPoint (&ptPosition,lpInfo);
            lpInfo->Value = Slider_ValueFromPoint (hWindow,ptPosition,lpInfo);
            if (lpInfo->lStyle & SS_SNAP ||
                lpInfo->lStyle & SS_SNAPDRAG)
                Slider_PointFromValue (hWindow,lpInfo->Value,&lpInfo->ptPosition,lpInfo);
            else if (lpInfo->lStyle & SS_HORZSLIDER &&
                ptPosition.x != lpInfo->ptPosition.x)
                lpInfo->ptPosition.x = ptPosition.x;
            else if (ptPosition.y != lpInfo->ptPosition.y)
                lpInfo->ptPosition.y = ptPosition.y;
            else
                bPaint = FALSE;
            if (bPaint)
            {
                InvalidateRect (hWindow,NULL,FALSE);
                UpdateWindow (hWindow);
            }
        }
	 	FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
							hWindow, SN_VALUESELECTED, SendMessage);
    }
}

/***********************************************************************/
LOCAL void Slider_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
 	POINT	ptPosition;
	int		Value;
	BOOL	bPaint;
    LPSLIDERINFO lpInfo = Slider_GetInfo (hWindow);

	ptPosition.x = x; ptPosition.y = y;

    if (lpInfo && lpInfo->bCaptured)
    {
        bPaint = FALSE;

        if (!lpInfo->bStepping)
        {
            Value = lpInfo->Value;

            bPaint = TRUE;
            Slider_ClipPoint (&ptPosition,lpInfo);
            lpInfo->Value = Slider_ValueFromPoint (hWindow,ptPosition,lpInfo);
            if (lpInfo->lStyle & SS_SNAPDRAG)
            {
                Slider_PointFromValue (hWindow,lpInfo->Value,&lpInfo->ptPosition,lpInfo);
                bPaint = lpInfo->Value != Value;
            }
            else if (lpInfo->lStyle & SS_HORZSLIDER &&
                ptPosition.x != lpInfo->ptPosition.x)
                lpInfo->ptPosition.x = ptPosition.x;
            else if (ptPosition.y != lpInfo->ptPosition.y)
                lpInfo->ptPosition.y = ptPosition.y;
            else
                bPaint = FALSE;
        }
        if (bPaint)
        {
		 	FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
								hWindow, SN_VALUECHANGING, SendMessage);
            InvalidateRect (hWindow,NULL,FALSE);
            UpdateWindow (hWindow);
        }
    }
}

/***********************************************************************/
LOCAL void Slider_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
    LPSLIDERINFO lpInfo = Slider_GetInfo (hWindow);

    if (lpInfo)
    {
        lpInfo->bFocus = TRUE;
        InvalidateRect (hWindow,NULL,FALSE);
    }
}

/***********************************************************************/
LOCAL void Slider_OnSize(HWND hWindow, UINT state, int cx, int cy)
/***********************************************************************/
{
    LPSLIDERINFO lpInfo;

    HDC hDC = GetDC (hWindow);
    
    if (hDC && (cx > BitmapWidth || cy > BitmapHeight))
    {
        if (cx > BitmapWidth)
            BitmapWidth = cx;
        if (cy > BitmapHeight)
            BitmapHeight = cy;
        if (hBitmap)
            DeleteObject (hBitmap);
        hBitmap = CreateCompatibleBitmap (hDC,BitmapWidth,BitmapHeight);
        InvalidateRect (hWindow,NULL,FALSE);
        UpdateWindow (hWindow); 
    }   
    if (hDC)
        ReleaseDC (hWindow,hDC);
    lpInfo = Slider_GetInfo (hWindow);
    if (lpInfo)
        Slider_InitInfo (hWindow, lpInfo,TRUE);
	 FORWARD_WM_SIZE(hWindow, state, cx, cy, Control_DefProc);
}

/***********************************************************************/
LOCAL void Slider_OnTimer(HWND hWindow, UINT id)
/***********************************************************************/
{
	POINT	ptPosition;
	int		Value;
	BOOL	bPaint;
    LPSLIDERINFO lpInfo = Slider_GetInfo (hWindow);

    if (lpInfo)
    {
        bPaint = FALSE;

        GetCursorPos (&ptPosition);
        ScreenToClient (hWindow,&ptPosition);
        Value = Slider_ValueFromPoint (hWindow,ptPosition,lpInfo);
        if (Value < lpInfo->Value)
            bPaint = Slider_DecrementValue (lpInfo);
        else if (Value > lpInfo->Value)
            bPaint = Slider_IncrementValue (lpInfo);
        Slider_PointFromValue (hWindow,lpInfo->Value,&lpInfo->ptPosition,lpInfo);
        if (bPaint)
        {
		 	FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
								hWindow, SN_VALUECHANGING, SendMessage);
            InvalidateRect (hWindow,NULL,FALSE);
            UpdateWindow (hWindow);
        }
    }
}

/***********************************************************************/
LOCAL long Slider_OnUserMsg (HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
    long lResult = 0L;
    LPSLIDERINFO lpInfo = Slider_GetInfo (hWindow);

    if (lpInfo)
    {
        switch (msg)
        {
            case SM_GETRANGE:
                lResult = MAKELONG (lpInfo->Min,lpInfo->Max);
                break;
            case SM_GETTHUMBSIZE:
                lResult = MAKELONG (lpInfo->ThumbWidth,lpInfo->ThumbHeight);
                break;
            case SM_GETTICKS:
                lResult = MAKELONG (lpInfo->BigTickInc,lpInfo->SmallTickInc);
                break;
            case SM_GETVALUE:
                lResult = (long)lpInfo->Value;
//                lResult = (INT32)lpInfo->Value;
                break;
            case SM_SETRANGE:
                lpInfo->Min = (int)LOWORD (lParam);
                lpInfo->Max = (int)HIWORD (lParam);
//                lpInfo->Min = (INT16)LOWORD (lParam);
//                lpInfo->Max = (INT16)HIWORD (lParam);
                Slider_PointFromValue (hWindow,lpInfo->Value,&lpInfo->ptPosition,lpInfo);
                InvalidateRect (hWindow,NULL,FALSE);
                break;
            case SM_SETSTEP:
                lpInfo->Step = (int)wParam;
//                lpInfo->Step = (INT16)wParam;
                Slider_PointFromValue (hWindow,lpInfo->Value,&lpInfo->ptPosition,lpInfo);
                InvalidateRect (hWindow,NULL,FALSE);
                break;
            case SM_SETTHUMBSIZE:
                lpInfo->ThumbWidth = (int)LOWORD (lParam);
//                lpInfo->ThumbWidth = (INT16)LOWORD (lParam);
                lpInfo->ThumbHeight = (int)HIWORD (lParam);
//                lpInfo->ThumbHeight = (INT16)HIWORD (lParam);
                lpInfo->bStdThumb = (lParam == 0L);
                Slider_InitInfo (hWindow, lpInfo,TRUE);
                InvalidateRect (hWindow,NULL,FALSE);
                break;
            case SM_SETTICKS:
                lpInfo->BigTickInc = (int)LOWORD (lParam);
//                lpInfo->BigTickInc = (INT16)LOWORD (lParam);
                lpInfo->SmallTickInc = (int)HIWORD (lParam);
//                lpInfo->SmallTickInc = (INT16)HIWORD (lParam);
                InvalidateRect (hWindow,NULL,FALSE);
                break;
            case SM_SETVALUE:
                lpInfo->Value = (int)wParam;
//                lpInfo->Value = (INT16)wParam;
                Slider_PointFromValue (hWindow,lpInfo->Value,&lpInfo->ptPosition,lpInfo);
                InvalidateRect (hWindow,NULL,FALSE);
                break;
            default:
			 	  break;
        }
    }
    return lResult;
}

/***********************************************************************/
LOCAL void Slider_InitInfo (
HWND hWindow,
LPSLIDERINFO lpInfo,
BOOL bUpdate)
/***********************************************************************/
{
    RECT rClient;

    if (!bUpdate)
    {
        lpInfo->Value = 0;
        lpInfo->Min = 0;
        lpInfo->Max = 100;
        lpInfo->Step = 1;
        lpInfo->BigTickInc = 5;
        lpInfo->SmallTickInc = 0;
        lpInfo->bCaptured = FALSE;
        lpInfo->bStepping = FALSE;
        lpInfo->bFocus = FALSE;
        lpInfo->bStdThumb = TRUE;
    }
    lpInfo->GrooveThickness = 2;
    lpInfo->lStyle = GetWindowLong (hWindow,GWL_STYLE);
    GetClientRect (hWindow,&rClient);
    if (lpInfo->lStyle & SS_HORZSLIDER)
    {
        if (lpInfo->bStdThumb)
        {
            lpInfo->ThumbWidth = 8;
            lpInfo->ThumbHeight = 16;
        }
        lpInfo->rPosRange.left = rClient.left + (lpInfo->ThumbWidth+1)/2;
        lpInfo->rPosRange.right = rClient.right - (lpInfo->ThumbWidth+1)/2;
        lpInfo->rPosRange.top = rClient.bottom/2;
        lpInfo->rPosRange.bottom = rClient.bottom/2;
    }
    else
    {
        if (lpInfo->bStdThumb)
        {
            lpInfo->ThumbWidth = 8;
            lpInfo->ThumbHeight = 16;
        }
        lpInfo->ThumbWidth = 16;
        lpInfo->ThumbHeight = 8;
        lpInfo->rPosRange.left = rClient.right/2;
        lpInfo->rPosRange.right = rClient.right/2;
        lpInfo->rPosRange.top = rClient.top + (lpInfo->ThumbHeight+1)/2;
        lpInfo->rPosRange.bottom = rClient.bottom - (lpInfo->ThumbHeight+1)/2;
    }
    Slider_PointFromValue (hWindow,lpInfo->Value,&lpInfo->ptPosition,lpInfo);
}

/***********************************************************************/
LOCAL LPSLIDERINFO Slider_GetInfo(HWND hWindow)
/***********************************************************************/
{
return((LPSLIDERINFO)GetWindowLong(hWindow, GWL_CTLDATAPTR));
}

/***********************************************************************/
LOCAL void Slider_Draw(HWND hWindow, HDC hDC)
/***********************************************************************/
{
	LPSLIDERINFO lpInfo;
	DRAWITEMSTRUCT diData;
	RECT rClient;

	lpInfo = Slider_GetInfo (hWindow);

	if (lpInfo)
	{
    	if (lpInfo->lStyle & SS_OWNERDRAW)
    	{
        	GetClientRect (hWindow,&rClient);
        	diData.CtlType = 0;
        	diData.CtlID = GetDlgCtrlID(hWindow);
        	diData.itemID = 0;
        	diData.itemAction = ODA_DRAWENTIRE;
        	diData.itemState = (lpInfo->bFocus ? ODS_FOCUS : 0);
        	diData.hwndItem = hWindow;
        	diData.hDC = hDC;
        	diData.rcItem = lpInfo->rPosRange;
        	diData.itemData = MAKELONG (lpInfo->ptPosition.x,
            	lpInfo->ptPosition.y);
        	if (lpInfo->lStyle & SS_HORZSLIDER)
        	{
            	diData.rcItem.top = rClient.top;
            	diData.rcItem.bottom = rClient.bottom - rClient.top;
        	}
        	else
        	{
            	diData.rcItem.left = rClient.left;
            	diData.rcItem.right = rClient.right - rClient.left;
        	}
			FORWARD_WM_DRAWITEM(GetParent(hWindow), &diData, SendMessage);
    	}
    	else
    	{
        	Slider_DrawBackground (hWindow,hDC,lpInfo);
        	Slider_DrawTicks (hWindow,hDC,lpInfo);
        	Slider_DrawThumb (hWindow,hDC,lpInfo);
    	}
	}
}

/***********************************************************************/
LOCAL void Slider_DrawBackground (HWND hWindow, HDC hDC, LPSLIDERINFO lpInfo)
/***********************************************************************/
{
    RECT rClient;
    HBRUSH hBrush;
    RECT rSlider = lpInfo->rPosRange;

    GetClientRect (hWindow,&rClient);
//	Ctl_EraseBackground( hWindow, hDC, &rClient, CTLCOLOR_BTN);
// this makes the slider background color always light gray
	hBrush = (HBRUSH)GetStockObject (LTGRAY_BRUSH);
	FillRect (hDC, &rClient, hBrush);

    if (lpInfo->lStyle & SS_HORZSLIDER)
    {
        rSlider.top = (rClient.bottom / 2) - (lpInfo->GrooveThickness / 2);
        rSlider.bottom = rSlider.top + lpInfo->GrooveThickness;
    }
    else
    {
        rSlider.left = (rClient.right / 2) - (lpInfo->GrooveThickness / 2);
        rSlider.right = rSlider.left + lpInfo->GrooveThickness;
    }
    InflateRect (&rSlider,1,1);
    Ctl_DrawBevel (hDC, Ctl_GetBtnShadowColor(),
	 					Ctl_GetBtnHilightColor(),
        rSlider,1,DB_IN|DB_RECT);
    InflateRect (&rSlider,-1,-1);
    if (lpInfo->lStyle & SS_RIGHTTOLEFT)
        hBrush = CreateSolidBrush (RGB(0, 0, 0));
    else
        hBrush = CreateSolidBrush (RGB(255, 0, 0));
    FillRect (hDC,&rSlider,hBrush);
    DeleteObject (hBrush);
    if (lpInfo->lStyle & SS_HORZSLIDER)
        rSlider.left = lpInfo->ptPosition.x;
    else
        rSlider.top = lpInfo->ptPosition.y;
    if (lpInfo->lStyle & SS_RIGHTTOLEFT)
        hBrush = CreateSolidBrush (RGB(255, 0, 0));
    else
        hBrush = CreateSolidBrush (RGB(0, 0, 0));
    FillRect (hDC,&rSlider,hBrush);
    DeleteObject (hBrush);
}

/***********************************************************************/
LOCAL void Slider_DrawTicks (HWND hWindow, HDC hDC, LPSLIDERINFO lpInfo)
/***********************************************************************/
{
    HPEN hPen;
    HPEN hOldPen;
    int i;
    POINT ptTick;
    int Offset;
    int Length;

    hPen = CreatePen (PS_SOLID,1,Ctl_GetBtnShadowColor());
    hOldPen = (HPEN)SelectObject (hDC,hPen);

	 if ((lpInfo->lStyle & SS_HORZSLIDER) ?
        (lpInfo->lStyle & SS_TICKSBOTTOM) :
        (lpInfo->lStyle & SS_TICKSRIGHT))
        Offset = lpInfo->GrooveThickness / 2 + 3;
    else
        Offset = - lpInfo->GrooveThickness / 2 - 3;
    if (lpInfo->lStyle & SS_HORZSLIDER)
    {
        Length = lpInfo->ThumbHeight / 2;
        if (!(lpInfo->lStyle & SS_TICKSBOTTOM))
            Length = -Length;
    }
    else
    {
        Length = lpInfo->ThumbWidth / 2;
        if (!(lpInfo->lStyle & SS_TICKSRIGHT))
            Length = -Length;
    }
    if (lpInfo->BigTickInc)
    {
        if (lpInfo->lStyle & SS_HORZSLIDER)
        {
            for (i=lpInfo->Min;i<=lpInfo->Max;i+=lpInfo->BigTickInc)
            {
                Slider_PointFromValue (hWindow,i,&ptTick,lpInfo);
                MoveToEx (hDC,ptTick.x,ptTick.y + Offset, NULL);
                LineTo (hDC,ptTick.x,ptTick.y + Offset + Length);
            }
        }
        else
        {
            for (i=lpInfo->Min;i<=lpInfo->Max;i+=lpInfo->BigTickInc)
            {
                Slider_PointFromValue (hWindow,i,&ptTick,lpInfo);
                MoveToEx (hDC,ptTick.x + Offset,ptTick.y, NULL);
                LineTo (hDC,ptTick.x + Offset + Length,ptTick.y);
            }
        }
    }
    Length /= 2;
    if (lpInfo->SmallTickInc)
    {
        if (lpInfo->lStyle & SS_HORZSLIDER)
        {
            for (i=lpInfo->Min;i<=lpInfo->Max;i+=lpInfo->SmallTickInc)
            {
                Slider_PointFromValue (hWindow,i,&ptTick,lpInfo);
                MoveToEx (hDC,ptTick.x,ptTick.y + Offset, NULL);
                LineTo (hDC,ptTick.x,ptTick.y + Offset + Length);
            }
        }
        else
        {
            for (i=lpInfo->Min;i<=lpInfo->Max;i+=lpInfo->SmallTickInc)
            {
                Slider_PointFromValue (hWindow,i,&ptTick,lpInfo);
                MoveToEx (hDC,ptTick.x + Offset,ptTick.y, NULL);
                LineTo (hDC,ptTick.x + Offset + Length,ptTick.y);
            }
        }
    }
    SelectObject (hDC,hOldPen);
    DeleteObject (hPen);
}

/***********************************************************************/
LOCAL void Slider_DrawThumb (HWND hWindow, HDC hDC, LPSLIDERINFO lpInfo)
/***********************************************************************/
{
    RECT rButton;
    WORD wBtnStyle = BMS_STDBEVEL|BMS_ROUNDCORNERS|BMS_THINBEVEL;

    rButton.left = lpInfo->ptPosition.x - lpInfo->ThumbWidth/2;
    rButton.top = lpInfo->ptPosition.y - lpInfo->ThumbHeight/2;
    rButton.bottom = rButton.top + lpInfo->ThumbHeight;
    rButton.right = rButton.left + lpInfo->ThumbWidth;
    if (lpInfo->bFocus)
        wBtnStyle &= ~BMS_THINBEVEL;

	Ctl_DrawButton(hWindow, hDC, &rButton, wBtnStyle, BMS_UP);
}

/***********************************************************************/
LOCAL void Slider_PointFromValue (
HWND hWindow,
int  Value,
LPPOINT lpPosition,
LPSLIDERINFO lpInfo)
/***********************************************************************/
{
    RECT rClient;
    long nSteps = (long)(lpInfo->Max - lpInfo->Min) / (long)lpInfo->Step;

    if (Value > lpInfo->Max)
        Value = lpInfo->Max;
    if (Value < lpInfo->Min)
        Value = lpInfo->Min;
    Value = (Value - lpInfo->Min) / lpInfo->Step;
    GetClientRect (hWindow,&rClient);
    if (lpInfo->lStyle & SS_HORZSLIDER)
    {
        lpPosition->y = lpInfo->rPosRange.top;
        lpPosition->x = (int) (
            lpInfo->rPosRange.left +
            ((long)Value * (long)WRECT (lpInfo->rPosRange) + nSteps/2) / nSteps);
        if (lpInfo->lStyle & SS_RIGHTTOLEFT)
            lpPosition->x = lpInfo->rPosRange.right - lpPosition->x +
                lpInfo->rPosRange.left;
    }
    else
    {
        lpPosition->x = lpInfo->rPosRange.left;
        lpPosition->y = (int) (
            lpInfo->rPosRange.top +
            ((long)Value * (long)HRECT (lpInfo->rPosRange) + nSteps/2) / nSteps);
        if (lpInfo->lStyle & SS_BOTTOMTOTOP)
            lpPosition->y = lpInfo->rPosRange.bottom - lpPosition->y +
                lpInfo->rPosRange.top;
    }
}

/***********************************************************************/
LOCAL int Slider_ValueFromPoint (
HWND hWindow,
POINT ptPosition,
LPSLIDERINFO lpInfo)
/***********************************************************************/
{
    RECT rClient;
    int Value;
    long nSteps = (long)(lpInfo->Max - lpInfo->Min) / (long)lpInfo->Step;

    GetClientRect (hWindow,&rClient);
    Slider_ClipPoint (&ptPosition,lpInfo);
    if (lpInfo->lStyle & SS_HORZSLIDER)
        Value = (int)(
            ((long)(ptPosition.x - (long)lpInfo->rPosRange.left) * nSteps +
            (long)WRECT (lpInfo->rPosRange) / 2) /
            (long)WRECT (lpInfo->rPosRange) * lpInfo->Step) + lpInfo->Min;
    else
        Value = (int)(
            ((long)(ptPosition.y - (long)lpInfo->rPosRange.top) * nSteps +
            (long)(long)HRECT (lpInfo->rPosRange) / 2) /
            (long)(long)HRECT (lpInfo->rPosRange) * lpInfo->Step) + lpInfo->Min;
    if (lpInfo->lStyle & SS_RIGHTTOLEFT) // or SS_BOTTTOMTOTOP
        Value = lpInfo->Max - Value + lpInfo->Min;
    return Value;
}

/***********************************************************************/
LOCAL void Slider_ClipPoint (
LPPOINT lpPosition,
LPSLIDERINFO lpInfo)
/***********************************************************************/
{
    if (lpPosition->x < lpInfo->rPosRange.left)
        lpPosition->x = lpInfo->rPosRange.left;
    if (lpPosition->x > lpInfo->rPosRange.right)
        lpPosition->x = lpInfo->rPosRange.right;
    if (lpPosition->y < lpInfo->rPosRange.top)
        lpPosition->y = lpInfo->rPosRange.top;
    if (lpPosition->y > lpInfo->rPosRange.bottom)
        lpPosition->y = lpInfo->rPosRange.bottom;
}

/***********************************************************************/
LOCAL BOOL Slider_DecrementValue (
 LPSLIDERINFO lpInfo)
/***********************************************************************/
{
    if (lpInfo->Value >= (lpInfo->Min + lpInfo->Step))
    {
        lpInfo->Value -= lpInfo->Step;
        return TRUE;
    }
    else
        return FALSE;
}

/***********************************************************************/
LOCAL BOOL Slider_IncrementValue (
LPSLIDERINFO lpInfo)
/***********************************************************************/
{
    if (lpInfo->Value <= (lpInfo->Max - lpInfo->Step))
    {
        lpInfo->Value += lpInfo->Step;
        return TRUE;
    }
    else
        return FALSE;
}

/***********************************************************************/
LOCAL BOOL Slider_IsOnThumb (
POINT ptPosition,
LPSLIDERINFO lpInfo)
/***********************************************************************/
{
    RECT rThumb;

    rThumb.left = lpInfo->ptPosition.x - (lpInfo->ThumbWidth + 1)/2;
    rThumb.right = rThumb.left + lpInfo->ThumbWidth;
    rThumb.top = lpInfo->ptPosition.y - (lpInfo->ThumbHeight + 1)/2;
    rThumb.bottom = rThumb.top + lpInfo->ThumbHeight;
    return PtInRect (&rThumb,ptPosition);
}
