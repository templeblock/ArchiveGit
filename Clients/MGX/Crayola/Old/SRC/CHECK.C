/***************************************************************************

         (C) Copyright 1991 by Micrografx, Inc.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
            from Micrografx, 1303 Arapaho, Richardson, TX. 75081.

 ****************************************************************************
 ****************************************************************************

                                    CHECK.C

 ****************************************************************************
 ****************************************************************************

 Description:  MGX CheckBox Control
 Date Created: November 19, 1991
 Author:       Kevin McFarland
 Comments:

    Additional Messages:

    BTNM_CLICK
    BTNM_GETCHECK
    BTNM_SETCHECK
    BTNM_GETSTATE
    BTNM_SETSTATE
    CTLM_GETSCHEME
    CTLM_SETSCHEME
    CTLM_GETCOLOR
    CTLM_SETCOLOR
    CTLM_SETSTYLE
    CTLM_GETNOMINALSIZE

    Notifications:

    CTLN_CLICKED
    CTLN_DOUBLCLICKED

    Colors that you can change with the SET/GETCOLOR Message:

    SC_CHECK1  Color for checkbox check mark
    SC_CHECK2  Color for checkbox second check mark (3STATE)

 ****************************************************************************/

#include "ctlutil.h"
#include "check.h"

#ifdef DLGEDIT
void Control_GetClassInfo(LPSTR lpClassName, LPWORD lpWidth, LPWORD lpHeight, LPDWORD lpStyle)
{
lstrcpy(lpClassName, MGXCTL_CHECKBOX);
*lpWidth = 40;
*lpHeight = 12;
*lpStyle = 0;
}
BOOL Control_RegisterClass(HINSTANCE hInstance, UINT ClassScope)
{
return(Check_RegisterClass(hInstance, ClassScope));
}
#define Check_Control DlgEdit_Control
extern "C"{
LONG WINPROC Check_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
}
#else
LONG WINPROC Check_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
#endif

/********************************* Constants ********************************/

/* Window WORD/LONG Offsets */

#define GWL_HBITMAP         GWW_START
#define GWL_CHECK1COLOR     (GWL_HBITMAP + sizeof (long))
#define GWL_CHECK2COLOR     (GWL_CHECK1COLOR + sizeof (long))
#define WINDOW_EXTRA        (GWL_CHECK2COLOR + sizeof (long))

/* Bit Masks for State Word */

#define BFCHECK             0x0003      // 3 bit for check value 0-2
#define BFHILIGHT           0x0004      // pressed state
#define BFFOCUS             0x0008      // focus set
#define BFINCLICK           0x0010      // inside click code
#define BFCAPTURED          0x0020      // we have mouse capture
#define BFMOUSE             0x0040      // mouse-initiated
#define BFDONTCLICK         0x0080      // don't check on get focus

/* Bit Masks for Window Long */

#define MASK_TYPE           0x0007L     // Type Mask

/* Check box btn bitmap size */

#define BOX_WIDTH           14
#define BOX_HEIGHT          14

LOCAL BOOL Check_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct);
LOCAL void Check_OnDestroy(HWND hWindow);
LOCAL void Check_OnEnable(HWND hWindow, BOOL fEnable);
LOCAL BOOL Check_OnEraseBkgnd(HWND hWindow, HDC hDC);
LOCAL void Check_OnPaint(HWND hWindow);
LOCAL UINT Check_OnGetDlgCode(HWND hWindow, LPMSG lpMsg);
LOCAL HFONT Check_OnGetFont(HWND hWindow);
LOCAL void Check_OnSetFont(HWND hwndCtl, HFONT hfont, BOOL fRedraw);
LOCAL void Check_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Check_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Check_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Check_OnKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Check_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Check_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Check_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Check_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Check_OnSize(HWND hWindow, UINT state, int cx, int cy);
LOCAL void Check_OnSysKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL long Check_OnUserMsg (HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
LOCAL void	Check_OnClick (HWND,WPARAM,LPARAM);
LOCAL void	Check_OnGetCheck (HWND,LPLONG);
LOCAL void	Check_OnGetState (HWND,LPLONG);
LOCAL void	Check_OnSetCheck (HWND,WPARAM);
LOCAL void	Check_OnSetState (HWND,BOOL);
LOCAL void	Check_OwnerDrawNotify (HWND,HDC,UINT,UINT);
LOCAL void	Check_ReleaseCapture (HWND,BOOL);
LOCAL void	Check_SetCapture (HWND,BOOL);
LOCAL void	Check_DrawPushBtn (HWND,HDC,HBRUSH,UINT);
LOCAL void	Check_GetTextRect (HWND,long,LPRECT);
LOCAL void Check_DrawCheckBox (HWND,HDC,UINT);
LOCAL void Check_DrawCheckBoxBox (HWND,HDC,UINT,long,RECT);
LOCAL void Check_GetBoxRect (HWND,long,LPRECT);
LOCAL void Check_OwnerDrawNotify (HWND,HDC,UINT,UINT);
LOCAL void Check_ReleaseCapture (HWND,BOOL);
LOCAL void Check_SetCapture (HWND,BOOL);

static UINT nControls = 0;
static int cxBorder = 1;
static int cyBorder = 1;

BOOL Check_RegisterClass (
HINSTANCE hInstance, UINT ClassScope)
{
    WNDCLASS wcControl;
    BOOL     bResult;

    wcControl.lpszClassName = MGXCTL_CHECKBOX;
    wcControl.hInstance     = hInstance;
    wcControl.lpfnWndProc   = Check_Control;
    wcControl.hCursor       = LoadCursor (NULL,IDC_ARROW);
    wcControl.hIcon         = NULL;
    wcControl.lpszMenuName  = NULL;
    wcControl.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wcControl.cbClsExtra    = 0;
    wcControl.cbWndExtra    = WINDOW_EXTRA;
    wcControl.style         = ClassScope;
    bResult = RegisterClass (&wcControl);
    return (bResult);
}

/***********************************************************************/
#ifdef DLGEDIT
extern "C"{
#endif
LONG WINPROC EXPORT Check_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_CREATE, Check_OnCreate);
	HANDLE_MSG(hWindow, WM_DESTROY, Check_OnDestroy);
	HANDLE_MSG(hWindow, WM_ENABLE, Check_OnEnable);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Check_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_PAINT, Check_OnPaint);
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Check_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_GETFONT, Check_OnGetFont);
	HANDLE_MSG(hWindow, WM_SETFONT, Check_OnSetFont);
	HANDLE_MSG(hWindow, WM_SETFOCUS, Check_OnSetFocus);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, Check_OnKillFocus);
	HANDLE_MSG(hWindow, WM_KEYDOWN, Check_OnKeyDown);
	HANDLE_MSG(hWindow, WM_KEYUP, Check_OnKeyUp);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Check_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Check_OnLButtonDblClk);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, Check_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Check_OnMouseMove);
	HANDLE_MSG(hWindow, WM_SIZE, Check_OnSize);
	HANDLE_MSG(hWindow, WM_SYSKEYUP, Check_OnSysKeyUp);
	HANDLE_MSG(hWindow, BTNM_CLICK, Check_OnUserMsg);
	HANDLE_MSG(hWindow, BTNM_GETCHECK, Check_OnUserMsg);
	HANDLE_MSG(hWindow, BTNM_GETSTATE, Check_OnUserMsg);
	HANDLE_MSG(hWindow, BTNM_SETSTATE, Check_OnUserMsg);
	HANDLE_MSG(hWindow, BTNM_SETCHECK, Check_OnUserMsg);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}
#ifdef DLGEDIT
}
#endif

/***********************************************************************/
LOCAL BOOL Check_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
    HDC hDC = GetDC (hWindow);

    SetWindowWord (hWindow,GWW_STATE,0);
    SetWindowWord (hWindow,GWL_HBITMAP,0);
    SetWindowWord (hWindow,GWL_FONT,0);

    SetWindowLong (hWindow,GWL_CHECK1COLOR,CTLCOLOR_DEFAULT);
    SetWindowLong (hWindow,GWL_CHECK2COLOR,CTLCOLOR_DEFAULT);

    if (hDC)
    {
        if (nControls == (UINT) NULL)
        {
            cxBorder = GetSystemMetrics (SM_CXBORDER);
            cyBorder = GetSystemMetrics (SM_CYBORDER);
        }
        ReleaseDC (hWindow,hDC);
    }
    nControls++;
    return TRUE;
}

/***********************************************************************/
LOCAL void Check_OnDestroy(HWND hWindow)
/***********************************************************************/
{
    HBITMAP hBitmap = (HBITMAP) GetWindowLong (hWindow,GWL_HBITMAP);

    if (hBitmap)
        DeleteObject (hBitmap);
    nControls -= (nControls > 0) ? 1 : 0;
}

/***********************************************************************/
LOCAL void Check_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
    InvalidateRect (hWindow,NULL,TRUE);
    UpdateWindow (hWindow);
}

/***********************************************************************/
LOCAL BOOL Check_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
    return TRUE;
}

/***********************************************************************/
LOCAL void Check_OnPaint(HWND hWindow)
/***********************************************************************/
{
    PAINTSTRUCT ps;
    HBITMAP hOldBitmap, hBitmap = (HBITMAP) GetWindowLong (hWindow,GWL_HBITMAP);
    HDC hWindowDC = GetDC (hWindow);
    HDC hMemDC = CreateCompatibleDC (hWindowDC);

    ReleaseDC (hWindow,hWindowDC);
    if (IsWindowVisible (hWindow) && hMemDC && hBitmap &&
        (BeginPaint (hWindow,&ps) != NULL)) 
    { 
        RECT rc;
        WORD wState = GetWindowWord (hWindow,GWW_STATE);
        HBRUSH hOldBrush, hBtnFillBrush = Ctl_GetBackgroundBrush(hWindow, hWindowDC, CTLCOLOR_BTN);
        HFONT hOldFont, hNewFont = (HFONT) GetWindowLong (hWindow,GWL_FONT);
        long lType = GetWindowLong (hWindow,GWL_STYLE) & MASK_TYPE;
        
        GetClientRect (hWindow,&rc);
        hOldBitmap = (HBITMAP)SelectObject (hMemDC,hBitmap);
        hOldBrush = (HBRUSH)SelectObject (hMemDC,hBtnFillBrush);
        if (hNewFont) 
            hOldFont = (HFONT)SelectObject (hMemDC,hNewFont);
        PatBlt (hMemDC,0,0,rc.right,rc.bottom,PATCOPY);
        if (lType == CBS_OWNERDRAW)
        {
            Check_OwnerDrawNotify (hWindow,hMemDC,ODA_DRAWENTIRE,
                (wState & BFFOCUS ? ODS_FOCUS : 0) | 
                (wState & BFHILIGHT ? ODS_SELECTED : 0));
        }
        else if (lType == CBS_PUSHBTNCHECK || lType == CBS_PUSHBTNSQUARE)
            Check_DrawPushBtn (hWindow,hMemDC,hBtnFillBrush,wState);
        else
            Check_DrawCheckBox (hWindow,hMemDC,wState);
        BitBlt (ps.hdc,0,0,rc.right,rc.bottom,hMemDC,0,0,SRCCOPY);
        if (hNewFont) 
            SelectObject (hMemDC,hOldFont);
        SelectObject (hMemDC,hOldBrush);
        SelectObject (hMemDC,hOldBitmap);
//        DeleteObject (hBtnFillBrush);
        EndPaint (hWindow,&ps);
    }
    if (hMemDC)
        DeleteDC (hMemDC);
}

/***********************************************************************/
LOCAL UINT Check_OnGetDlgCode(HWND hWindow, LPMSG lpMsg)
/***********************************************************************/
{
	return(DLGC_BUTTON);
}

/***********************************************************************/
LOCAL HFONT Check_OnGetFont(HWND hWindow)
/***********************************************************************/
{
    return (HFONT)GetWindowLong (hWindow,GWL_FONT);
}

/***********************************************************************/
LOCAL void Check_OnSetFont(HWND hWindow, HFONT hFont, BOOL fRedraw)
/***********************************************************************/
{
    SetWindowLong (hWindow,GWL_FONT,(long)(LPTR)hFont);
    if (fRedraw)
    {
        InvalidateRect (hWindow,NULL,TRUE);
        UpdateWindow (hWindow);
    }
}

/***********************************************************************/
LOCAL void Check_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
    WORD wState = GetWindowWord (hWindow,GWW_STATE);
    
    if (!(wState & BFMOUSE))
    {
        if (vk == VK_SPACE) 
        {
            if (!(wState & BFINCLICK))
            {
                Check_SetCapture (hWindow,FALSE);
                SendMessage (hWindow,BTNM_SETSTATE,TRUE,0L);
            }
        } 
        else     
            Check_ReleaseCapture (hWindow,FALSE);
    }
}

/***********************************************************************/
LOCAL void Check_OnKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
    if ((GetWindowWord (hWindow,GWW_STATE) & BFMOUSE) ||
        (vk == VK_TAB))
		 FORWARD_WM_KEYUP(hWindow, vk, cRepeat, flags, Control_DefProc);
    else
    {
        Check_ReleaseCapture (hWindow,vk == VK_SPACE);
    }
}

/***********************************************************************/
LOCAL void Check_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
    WORD wState = GetWindowWord (hWindow,GWW_STATE);
    
    if (wState & BFCAPTURED)
        Check_ReleaseCapture (hWindow,TRUE);
    wState = GetWindowWord (hWindow,GWW_STATE);
    wState &= ~BFFOCUS;
    SetWindowWord (hWindow,GWW_STATE,wState);
    InvalidateRect (hWindow,NULL,FALSE);
}

/***********************************************************************/
LOCAL void Check_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
    WORD wState = GetWindowWord (hWindow,GWW_STATE);

    wState |= BFFOCUS;
    SetWindowWord (hWindow,GWW_STATE,wState);
    InvalidateRect (hWindow,NULL,FALSE);
}

/***********************************************************************/
LOCAL void Check_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
    RECT rControl;
	 POINT ptPosition;

	 ptPosition.x = x;
	 ptPosition.y = y;
    Check_SetCapture (hWindow,TRUE);
    GetClientRect (hWindow, &rControl);
    SendMessage (hWindow,BTNM_SETSTATE,
        PtInRect ((LPRECT)&rControl,ptPosition),0L);
}

/***********************************************************************/
LOCAL void Check_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
    if (GetWindowWord (hWindow,GWW_STATE) & BFMOUSE)
        Check_ReleaseCapture (hWindow,TRUE);
}

/***********************************************************************/
LOCAL void Check_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
    long lType = GetWindowLong (hWindow,GWL_STYLE) & MASK_TYPE;

    if (lType == CBS_OWNERDRAW)
        NOTIFY_PARENT (hWindow,CTLN_DOUBLECLICKED);
    else
        Check_OnClick (hWindow,0,0L);
}

/***********************************************************************/
LOCAL void Check_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
    WORD wState = GetWindowWord (hWindow,GWW_STATE);
	 POINT ptPosition;

	 ptPosition.x = x;
	 ptPosition.y = y;
    
    if (wState & BFMOUSE && wState & BFCAPTURED)
    {
        RECT rControl;
        
        GetClientRect (hWindow, &rControl);
        if (PtInRect (&rControl,ptPosition))  
        {
            if (!(wState & BFHILIGHT))
                SendMessage (hWindow,BTNM_SETSTATE,TRUE,0L);
        }
        else if (wState & BFHILIGHT)
            SendMessage (hWindow,BTNM_SETSTATE,FALSE,0L);
    }
}

/***********************************************************************/
LOCAL void Check_OnSize(HWND hWindow, UINT state, int cx, int cy)
/***********************************************************************/
{
    HDC hDC = GetDC (hWindow);
    
    if (hDC && cx > 0  && cy > 0)
    {
        HBITMAP hBitmap = (HBITMAP) GetWindowLong (hWindow,GWL_HBITMAP);

        if (hBitmap)
            DeleteObject (hBitmap);
        hBitmap = CreateCompatibleBitmap (hDC,cx,cy);
        SetWindowLong (hWindow,GWL_HBITMAP, (long)(LPTR)hBitmap);
        InvalidateRect (hWindow,NULL,FALSE);
        UpdateWindow (hWindow); 
    }   
    if (hDC)
        ReleaseDC (hWindow,hDC);
}

/***********************************************************************/
void Check_OnSysKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
    if (!((GetWindowWord (hWindow,GWW_STATE) & BFMOUSE) ||
        (vk == VK_TAB)))
        Check_ReleaseCapture (hWindow,vk == VK_SPACE);
}

/***********************************************************************/
LOCAL long Check_OnUserMsg (HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
long lResult = 0;

	switch (msg)
		{
        case BTNM_CLICK:
            Check_OnClick (hWindow,wParam,lParam);
            break;
        case BTNM_GETCHECK:       
            Check_OnGetCheck (hWindow,&lResult);
            break;
        case BTNM_GETSTATE:     
            Check_OnGetState (hWindow,&lResult);
            break;
        case BTNM_SETSTATE:
            Check_OnSetState (hWindow,wParam);
            break;
        case BTNM_SETCHECK:    
            Check_OnSetCheck (hWindow,wParam);
            break;
		 }
return(lResult);
}

LOCAL void  Check_OnClick (
HWND hWindow,
WPARAM wParam,
long lParam)
{
    SendMessage (hWindow,WM_LBUTTONDOWN,0,0L);
    SendMessage (hWindow,WM_LBUTTONUP,0,0L); 
	 Check_OnKeyDown(hWindow, 0, FALSE, 0, 0);
}

LOCAL void  Check_OnGetCheck (
HWND hWindow,
LPLONG lplResult)
{
    *lplResult = (long)(GetWindowWord (hWindow,GWW_STATE) & BFCHECK);
}

LOCAL void  Check_OnGetState (
HWND hWindow,
LPLONG lplResult)
{
    *lplResult = (long)GetWindowWord (hWindow,GWW_STATE);
}

LOCAL void  Check_OnSetCheck (
HWND hWindow,
WPARAM wCheck)
{
    WORD wState = GetWindowWord (hWindow,GWW_STATE);
    long lStyle = GetWindowLong (hWindow,GWL_STYLE);

    if (lStyle & CBS_3STATE)
    {
        if (wCheck > 2) 
            wCheck = 2;
    }
    else
        wCheck = (wCheck > 0 ? 1 : 0);
    if ((wState & BFCHECK) != wCheck) 
    {
        wState = ((wState & ~BFCHECK) | wCheck);
        SetWindowWord (hWindow,GWW_STATE,wState);
        InvalidateRect (hWindow,NULL,FALSE);
        UpdateWindow (hWindow); 
    }
}

LOCAL void  Check_OnSetState (
HWND hWindow,
BOOL bHilight)
{
    WORD wState = GetWindowWord (hWindow,GWW_STATE);

    if (bHilight && (!(wState & BFHILIGHT)))
    {
        wState |= BFHILIGHT;
        SetWindowWord (hWindow,GWW_STATE,wState);
        InvalidateRect (hWindow,NULL,FALSE);
        UpdateWindow (hWindow); 
    }
    else if (!bHilight && (wState & BFHILIGHT))
    {
        wState &= ~BFHILIGHT;
        SetWindowWord (hWindow,GWW_STATE,wState);
        InvalidateRect (hWindow,NULL,FALSE);
        UpdateWindow (hWindow); 
    }
}

LOCAL void Check_DrawCheckBox (
HWND hWindow,
HDC hDC,
UINT wState)
{
    RECT rBox, rText;
    int OldBkMode, OldMapMode;
    long lStyle = GetWindowLong (hWindow,GWL_STYLE);
    int iTextLen = GetWindowTextLength (hWindow);
    char far * lpText;

    Check_GetBoxRect (hWindow,lStyle,&rBox);
    OldMapMode = SetMapMode (hDC,MM_TEXT);
    Check_DrawCheckBoxBox (hWindow,hDC,wState,lStyle,rBox);
    if (iTextLen && (lpText = (LPSTR)Ctl_Alloc ((long)iTextLen+1)))
    {
        COLOR rgbOldTextColor;

        GetWindowText (hWindow,lpText,iTextLen+1);
        Check_GetTextRect (hWindow,lStyle,&rText);
        if (lStyle & WS_DISABLED)
            rgbOldTextColor = SetTextColor (hDC,Ctl_GetDisabledBtnTextColor());
        else
            rgbOldTextColor = SetTextColor (hDC,Ctl_GetBtnTextColor());
        OldBkMode = SetBkMode (hDC,TRANSPARENT);
        DrawText (hDC,lpText,-1,&rText,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
        if (wState & BFFOCUS)
        {
            SIZE TextExtents;
            UINT StrWidth;

            GetTextExtentPoint (hDC,lpText,lstrlen (lpText),&TextExtents);
            StrWidth = TextExtents.cx;
            rText.right = min ((int)StrWidth+rText.left,rText.right);
            InflateRect (&rText,2*cxBorder,cyBorder);
            if (lStyle & CBS_STDFOCUS)
            {
                COLOR rgbOldBkColor;

                rgbOldBkColor = SetBkColor (hDC,Ctl_GetBtnFaceColor());
                SetTextColor (hDC, Ctl_GetTextColor());
                DrawFocusRect (hDC,&rText);
                SetBkColor (hDC,rgbOldBkColor);
            }
            else
                Ctl_DrawBevel (hDC, Ctl_GetBtnShadowColor(),
                    Ctl_GetBtnHilightColor(),rText,1,DB_RECT|DB_IN);
        }
        SetBkMode (hDC,OldBkMode);
        SetTextColor (hDC,rgbOldTextColor);
        Ctl_FreeUp (lpText);
    }
    SetMapMode (hDC,OldMapMode);
}

LOCAL void Check_DrawCheckBoxBox (
HWND hWindow,
HDC    hDC,
UINT   wState,
long   lStyle,
RECT   rBox)
{
    long lType = lStyle & MASK_TYPE;

    if (wState & BFHILIGHT)
    {
        if (lType != CBS_CHECKBTN)
        {
            Ctl_DrawBevel (hDC, Ctl_GetBtnShadowColor(),
                   Ctl_GetBtnHilightColor(),rBox,1,DB_RECT|DB_IN);
            InflateRect (&rBox,-3*cxBorder,-3*cyBorder);
            if (lType == CBS_SQUAREBTN2)
	            Ctl_DrawBevel (hDC, Ctl_GetBtnShadowColor(),
                   Ctl_GetBtnHilightColor(),rBox,1,DB_RECT|DB_OUT);
            else if (lType == CBS_SQUAREBTN1)
	            Ctl_DrawBevel (hDC, Ctl_GetBtnShadowColor(),
   	                Ctl_GetBtnHilightColor(),rBox,1,DB_RECT|DB_IN);
        }
        else
            Ctl_DrawBevel (hDC, Ctl_GetBtnBorderColor(),
  	                Ctl_GetBtnBorderColor(),rBox,1,DB_RECT|DB_IN);

    }
    else
    {
        if (lType == CBS_SQUAREBTN2 || lType == CBS_CHECKBTN)
            Ctl_DrawBevel (hDC, Ctl_GetBtnShadowColor(),
                  Ctl_GetBtnHilightColor(),rBox,1,DB_RECT|DB_OUT);
        else if (lType == CBS_SQUAREBTN1)
            Ctl_DrawBevel (hDC, Ctl_GetBtnShadowColor(),
                  Ctl_GetBtnHilightColor(),rBox,1,DB_RECT|DB_IN);
        if (lType != CBS_CHECKBTN)
            InflateRect (&rBox,-3*cxBorder,-3*cyBorder);
        if (lType == CBS_SQUAREBTN2)
            Ctl_DrawBevel (hDC, Ctl_GetBtnShadowColor(),
                  Ctl_GetBtnHilightColor(),rBox,1,DB_RECT|DB_IN);
        else if (lType == CBS_SQUAREBTN1) 
            Ctl_DrawBevel (hDC, Ctl_GetBtnShadowColor(),
                  Ctl_GetBtnHilightColor(),rBox,1,DB_RECT|DB_OUT);
    }
    if (wState & BFCHECK)
    {
        HBRUSH hOldBrush, hCheckBrush;
        COLOR rgbCheck1 = (COLOR)GetWindowLong (hWindow,GWL_CHECK1COLOR);
        COLOR rgbCheck2 = (COLOR)GetWindowLong (hWindow,GWL_CHECK2COLOR);
        
        rgbCheck1 = RGB(255,255,0);
        rgbCheck2 = RGB(255,255,0);
        InflateRect (&rBox,-cxBorder,-cyBorder);
        if ((wState & BFCHECK) == 1)
            hCheckBrush = CreateSolidBrush (rgbCheck1);
        else
            hCheckBrush = CreateSolidBrush (rgbCheck2);
        hOldBrush = (HBRUSH)SelectObject (hDC,hCheckBrush);
        if (lType == CBS_CHECKBTN)
        {
            InflateRect (&rBox,-cxBorder,-cyBorder);
            if ((wState & BFCHECK) == 1)
                Ctl_DrawCheck (hDC,rgbCheck1,rBox);
            else
                Ctl_DrawCheck (hDC,rgbCheck2,rBox);
        }
        else
            FillRect (hDC,&rBox,hCheckBrush);
        SelectObject (hDC,hOldBrush);
        DeleteObject (hCheckBrush);
   }
}

LOCAL void Check_DrawPushBtn (
HWND hWindow,
HDC hDC,
HBRUSH hBkground,
UINT wState)
{
    RECT rButton, rInside;
   long lStyle = GetWindowLong (hWindow,GWL_STYLE);
    int OldMapMode = SetMapMode (hDC,MM_TEXT);
    int OldBkMode = SetBkMode (hDC,TRANSPARENT); 
    int iTextLen = GetWindowTextLength (hWindow);
    char far *lpText;

    GetClientRect (hWindow,&rButton);
    Ctl_DrawPushButton (hDC,&rButton,DP_THINBEVEL,((wState & BFHILIGHT) ? 
        PBSTATE_DOWN : PBSTATE_UP),hBkground);
    if (iTextLen && (lpText = (LPSTR)Ctl_Alloc ((long)iTextLen+1)))
    {
        COLOR rgbOldTextColor;
        COLOR rgbCheck1 = GetWindowLong (hWindow,GWL_CHECK1COLOR);
        COLOR rgbCheck2 = GetWindowLong (hWindow,GWL_CHECK2COLOR);
        SIZE StrExt;
        int xStr, yStr;
        UINT wBtnSize;
    
        if (rgbCheck1 == CTLCOLOR_DEFAULT)
            rgbCheck1 = RGB(255,255,0);
        if (rgbCheck2 == CTLCOLOR_DEFAULT)
            rgbCheck1 = RGB(255,255,0);
        GetWindowText (hWindow,lpText,iTextLen+1);
        GetTextExtentPoint (hDC,lpText,iTextLen,&StrExt);
        xStr = StrExt.cx;
        yStr = StrExt.cy;
        wBtnSize = 5*yStr/6;
        if (lStyle & WS_DISABLED)
            rgbOldTextColor = SetTextColor (hDC,Ctl_GetDisabledBtnTextColor());
        else
            rgbOldTextColor = SetTextColor (hDC,Ctl_GetBtnTextColor());
        // Account for Focus rect and space left and right
        InflateRect (&rButton,-2*cxBorder,-cyBorder);
        CopyRect (&rInside,&rButton);
        if (lStyle & CBS_LEFTTEXT)
        {
            rButton.right -= 3*yStr/4 + wBtnSize;
            rInside.right -= yStr/4;
            rInside.left = rInside.right - wBtnSize;
        }
        else
        {
            rButton.left += 3*yStr/4 + wBtnSize;
            rInside.left += yStr/4;
            rInside.right = rInside.left + wBtnSize;
        }
        rInside.top += ((rInside.bottom - rInside.top) - wBtnSize)/2;
        rInside.bottom = rInside.top + wBtnSize;
        if ((lStyle & MASK_TYPE) == CBS_PUSHBTNCHECK)
        {
            OffsetRect (&rInside,cxBorder,cyBorder);
            Ctl_DrawCheck (hDC,Ctl_GetBtnHilightColor(),rInside);
            OffsetRect (&rInside,-2*cxBorder,-2*cyBorder);
            Ctl_DrawCheck (hDC,Ctl_GetBtnShadowColor(),rInside);
            OffsetRect (&rInside,cxBorder,cyBorder);
            if ((wState & BFCHECK) == 1)
                Ctl_DrawCheck (hDC,rgbCheck1,rInside);
            else if ((wState & BFCHECK) == 2)
                Ctl_DrawCheck (hDC,rgbCheck2,rInside);
            else
                Ctl_DrawCheck (hDC,Ctl_GetBtnFaceColor(),rInside);
        }
        else
        {
            Ctl_DrawBevel (hDC, Ctl_GetBtnShadowColor(),
                   Ctl_GetBtnHilightColor(),rInside,1,DB_RECT|DB_IN);
            //InflateRect (&rInside,-2*cxBorder,-2*cyBorder);
            InflateRect (&rInside,-cxBorder,-cyBorder);
            if (wState & BFCHECK)
            {
                HBRUSH hOldBrush, hCheckBrush;
            
                if ((wState & BFCHECK) == 1)
                    hCheckBrush = CreateSolidBrush (rgbCheck1);
                else
                    hCheckBrush = CreateSolidBrush (rgbCheck2);
                hOldBrush = (HBRUSH)SelectObject (hDC,hCheckBrush);
                FillRect (hDC,&rInside,hCheckBrush);
                SelectObject (hDC,hOldBrush);
                DeleteObject (hCheckBrush);
            }
        }
        DrawText (hDC,lpText,-1,&rButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
        if (wState & BFFOCUS)
        {
            rButton.top += max ((rButton.bottom-rButton.top-yStr+1)/2,0);
            rButton.right = min (rButton.left+xStr,rButton.right);
            rButton.bottom = min (rButton.top+yStr,rButton.bottom);

            InflateRect (&rButton,cxBorder,cyBorder);
            if (lStyle & CBS_STDFOCUS)
            {
                COLOR rgbOldBkColor; 

                rgbOldBkColor = SetBkColor (hDC,Ctl_GetBtnFaceColor());
                SetTextColor (hDC,Ctl_GetBtnTextColor());
                DrawFocusRect (hDC,&rButton);
                SetBkColor (hDC,rgbOldBkColor);
            }
            else    
                Ctl_DrawBevel (hDC, Ctl_GetBtnShadowColor(),
                    Ctl_GetBtnHilightColor(),rButton,1,DB_RECT|DB_IN);
        }
        SetTextColor (hDC,rgbOldTextColor); 
        Ctl_FreeUp (lpText);
    }
    SetBkMode (hDC,OldBkMode);
    SetMapMode (hDC,OldMapMode);
}    

LOCAL void Check_GetBoxRect (
HWND hWindow,
long lStyle,
LPRECT lpRect)
{
    WORD CharW, CharH;
    int  BoxSize;

    GetClientRect (hWindow,lpRect);
    InflateRect (lpRect,-cxBorder,-cyBorder);
    Ctl_GetAvgCharSize ((HFONT)GetWindowLong (hWindow,GWL_FONT),&CharW,&CharH);
    if (lpRect->bottom < BOX_HEIGHT) 
        lpRect->bottom = BOX_HEIGHT;
    if (lpRect->right < BOX_WIDTH) 
        lpRect->right = BOX_WIDTH;
    if ((lStyle & MASK_TYPE) == CBS_CHECKBTN)
        BoxSize = max (BOX_HEIGHT,CharH);
    else
        BoxSize = max (BOX_HEIGHT,(5 * CharH / 6));
    if (lpRect->bottom >= BoxSize)
    {
        lpRect->top = (lpRect->bottom - BoxSize) / 2;
        lpRect->bottom = lpRect->top + BoxSize;
    }
    if (lpRect->right >= BoxSize)
    {
        if (lStyle & CBS_LEFTTEXT)
            lpRect->left = lpRect->right - BoxSize;
        else
            lpRect->right = lpRect->left + BoxSize;
    }
}

LOCAL void Check_GetTextRect (
HWND hWindow,
long lStyle,
LPRECT lpRect)
{
    WORD CharW, CharH;
    int  BoxSize;

    GetClientRect (hWindow,lpRect);
    InflateRect (lpRect,-cxBorder,-cyBorder);
    Ctl_GetAvgCharSize((HFONT) GetWindowLong (hWindow,GWL_FONT),&CharW,&CharH);
    if (lpRect->bottom < BOX_HEIGHT) 
        lpRect->bottom = BOX_HEIGHT;
    if (lpRect->right < BOX_WIDTH) 
        lpRect->right = BOX_WIDTH;
    BoxSize = max (BOX_HEIGHT,(5 * CharH / 6));
    if (lpRect->bottom >= (int)CharH + 2)
    {
        lpRect->top = (lpRect->bottom - CharH) / 2;
        lpRect->bottom = lpRect->top + CharH;
    }
    else 
    {
        lpRect->top++;
        lpRect->bottom--;
    }    
    if (lpRect->right >= (int)CharH)
    {
        if (lStyle & CBS_LEFTTEXT)
        {
            lpRect->right = lpRect->right - CharH/2 - BoxSize;
            lpRect->left = lpRect->left + 2;
        }
        else
        {
            lpRect->left = lpRect->left + CharH/2 + BoxSize;
            lpRect->right = lpRect->right - 2;
        }
    }
    else
    {
        lpRect->right = 0;
        lpRect->left = 0;
    }    
}

LOCAL VOID Check_OwnerDrawNotify (
HWND hWindow,
HDC hDC,
UINT wAction,
UINT wState)
{
    DRAWITEMSTRUCT drawItemStruct;
    UINT wControlId = GetDlgCtrlID(hWindow);

    drawItemStruct.CtlType    = ODT_BUTTON;
    drawItemStruct.CtlID      = wControlId;
    drawItemStruct.itemAction = wAction;
    drawItemStruct.hwndItem   = hWindow;
    drawItemStruct.hDC        = hDC;
    GetClientRect (hWindow,(LPRECT)(&drawItemStruct.rcItem));
    drawItemStruct.itemData   = 0L;
    drawItemStruct.itemState  = wState | 
        (IsWindowEnabled (hWindow) ? 0 : ODS_DISABLED);
    SendMessage (GetParent (hWindow),WM_DRAWITEM,wControlId, 
        (long)((LPDRAWITEMSTRUCT)&drawItemStruct));
}

LOCAL void Check_ReleaseCapture (
HWND hWindow,
BOOL bCheck)
{
    UINT wState = GetWindowWord (hWindow,GWW_STATE);
    long lStyle = GetWindowLong (hWindow,GWL_STYLE);

    wState &= ~BFINCLICK;
    SetWindowWord (hWindow,GWW_STATE,wState);
    if (wState & BFHILIGHT) 
    {
        SendMessage (hWindow,BTNM_SETSTATE,FALSE,0L);
        wState = GetWindowWord (hWindow,GWW_STATE);
        if (bCheck) 
        {
            if (lStyle & CBS_AUTO)
            {
                UINT check;
                
                check = (wState & BFCHECK) + 1;
                if ((int)check > (lStyle & CBS_3STATE ? 2 : 1))
                    check = 0;
                SendMessage (hWindow,BTNM_SETCHECK,check,0L);
            }
            NOTIFY_PARENT (hWindow,CTLN_CLICKED);
            wState = GetWindowWord (hWindow,GWW_STATE);
        }
    }
    if (wState & BFCAPTURED) 
    {
        wState &= ~(BFCAPTURED | BFMOUSE);
        SetWindowWord (hWindow,GWW_STATE,wState);
        ReleaseCapture ();
    }
}

LOCAL void Check_SetCapture (
HWND hWindow,
BOOL bMouseInitiated)
{
    WORD wState = GetWindowWord (hWindow,GWW_STATE);

    if (bMouseInitiated)
        wState |= BFMOUSE;
    if (!(wState & BFCAPTURED)) 
    {
        wState = (wState | BFCAPTURED | BFINCLICK);
        SetWindowWord (hWindow,GWW_STATE,wState);
        SetCapture (hWindow);
        SetFocus (hWindow);
        wState = GetWindowWord (hWindow,GWW_STATE);
    }
    SetWindowWord (hWindow,GWW_STATE,wState);
}


