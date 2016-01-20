/***************************************************************************

         (C) Copyright 1992 by Micrografx, Inc.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
            from Micrografx, 1303 Arapaho, Richardson, TX. 75081.

 ****************************************************************************
 ****************************************************************************

                                    RADIO.C

 ****************************************************************************
 ****************************************************************************

 Description:  MGX Radio button Control
 Date Created: January 28, 1992
 Author:       Kevin McFarland
 Comments:

    Additional Messages:

    BTNM_CLICK
    BTNM_GETCHECK
    BTNM_SETCHECK
    BTNM_GETSTATE
    BTNM_SETSTATE

    Notifications:

    CTLN_CLICKED
    CTLN_DOUBLECLICKED

    Colors that you can change with the SET/GETCOLOR Message:

    SC_RADIO       Color of radio "button"

 ****************************************************************************/
#include "ctlutil.h"
#include "radio.h"

#ifdef DLGEDIT
void Control_GetClassInfo(LPTSTR lpClassName, LPWORD lpWidth, LPWORD lpHeight, LPDWORD lpStyle)
{
lstrcpy(lpClassName, MGXCTL_RADIOBTN);
*lpWidth = 40;
*lpHeight = 12;
*lpStyle = 0;
}
BOOL Control_RegisterClass(HINSTANCE hInstance, UINT ClassScope)
{
return(Radio_RegisterClass(hInstance, ClassScope));
}
#define Radio_Control DlgEdit_Control
extern "C"{
LONG WINPROC Radio_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
}
#else
LONG WINPROC Radio_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
#endif


/********************************* Constants ********************************/

/* Window WORD/LONG Offsets */
#define GWL_HBITMAP         GWW_START
#define GWL_RADIOCOLOR      (GWL_HBITMAP + sizeof (long))
#define GWL_PRVCLICK        (GWL_RADIOCOLOR + sizeof(long))
#define WINDOW_EXTRA        (GWL_PRVCLICK + sizeof (long))

#define new_mem Ctl_Alloc
#define del_mem Ctl_FreeUp


/* Bit Masks for State Word */

#define BFCHECK             0x0003      // 3 bit for check value 0-2
#define BFHILIGHT           0x0004      // pressed state
#define BFFOCUS             0x0008      // focus set
#define BFINCLICK           0x0010      // inside click code
#define BFCAPTURED          0x0020      // we have mouse capture
#define BFMOUSE             0x0040      // mouse-initiated
#define BFDONTCLICK         0x0080      // don't check on get focus

#define DUMMY_FLAG          0xFFFF      // Dummy flag used in BTNM_CLICK

/* Bit Masks for Window Long */

#define MASK_TYPE           0x0700L     // Type Mask

/* Radiobtn btn bitmap size */

#define BOX_WIDTH           14
#define BOX_HEIGHT          14


LOCAL BOOL Radio_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct);
LOCAL void Radio_OnDestroy(HWND hWindow);
LOCAL void Radio_OnEnable(HWND hWindow, BOOL fEnable);
LOCAL BOOL Radio_OnEraseBkgnd(HWND hWindow, HDC hDC);
LOCAL void Radio_OnPaint(HWND hWindow);
LOCAL UINT Radio_OnGetDlgCode(HWND hWindow, LPMSG lpMsg);
LOCAL HFONT Radio_OnGetFont(HWND hWindow);
LOCAL void Radio_OnSetFont(HWND hwndCtl, HFONT hfont, BOOL fRedraw);
LOCAL void Radio_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Radio_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Radio_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Radio_OnKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Radio_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Radio_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Radio_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Radio_OnSize(HWND hWindow, UINT state, int cx, int cy);
LOCAL void Radio_OnSysKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL long Radio_OnUserMsg (HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
LOCAL void Radio_DrawBtn (HWND,HDC,UINT);
LOCAL void Radio_DrawBtnBtn (HWND,HDC,UINT,long,RECT);
LOCAL void Radio_DrawPushBtn (HWND,HDC,HBRUSH,UINT);
LOCAL void Radio_GetBtnRect (HWND,long,RECT far *);
LOCAL void Radio_GetTextRect (HWND,long,RECT far *);
LOCAL void Radio_OwnerDrawNotify (HWND,HDC,UINT,UINT);
LOCAL void Radio_ReleaseCapture (HWND,BOOL);
LOCAL void Radio_SetCapture (HWND,BOOL);
LOCAL void Radio_OnClick (HWND);
LOCAL UINT Radio_OnGetCheck (HWND);
LOCAL BOOL Radio_OnGetState (HWND);
LOCAL void Radio_OnSetCheck (HWND,UINT);
LOCAL void Radio_OnSetState (HWND,BOOL);

static UINT nControls = 0;
static int cxBorder = 1;
static int cyBorder = 1;

BOOL Radio_RegisterClass (
HINSTANCE hInstance, UINT ClassScope)
{
    WNDCLASS wcControl;
    BOOL     bResult;

    wcControl.lpszClassName = (LPTSTR)MGXCTL_RADIOBTN;
    wcControl.hInstance     = hInstance;
    wcControl.lpfnWndProc   = (WNDPROC)Radio_Control;
    wcControl.hCursor       = LoadCursor (NULL,IDC_ARROW);
    wcControl.hIcon         = NULL;
    wcControl.lpszMenuName  = NULL;
    wcControl.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcControl.cbClsExtra    = 0;
    wcControl.cbWndExtra    = WINDOW_EXTRA;
    wcControl.style         = ClassScope;
    bResult = RegisterClass (&wcControl);
    return (bResult);
}

BOOL Radio_UnRegisterClass (HINSTANCE hInstance)
{
    return (UnregisterClass ((LPTSTR)MGXCTL_RADIOBTN, hInstance));
}

/***********************************************************************/
#ifdef DLGEDIT
extern "C" {
#endif
LONG WINPROC EXPORT Radio_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_CREATE, Radio_OnCreate);
	HANDLE_MSG(hWindow, WM_DESTROY, Radio_OnDestroy);
	HANDLE_MSG(hWindow, WM_ENABLE, Radio_OnEnable);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Radio_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_PAINT, Radio_OnPaint);
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Radio_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_GETFONT, Radio_OnGetFont);
	HANDLE_MSG(hWindow, WM_SETFONT, Radio_OnSetFont);
	HANDLE_MSG(hWindow, WM_SETFOCUS, Radio_OnSetFocus);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, Radio_OnKillFocus);
	HANDLE_MSG(hWindow, WM_KEYDOWN, Radio_OnKeyDown);
	HANDLE_MSG(hWindow, WM_KEYUP, Radio_OnKeyUp);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Radio_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, Radio_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Radio_OnMouseMove);
	HANDLE_MSG(hWindow, WM_SIZE, Radio_OnSize);
	HANDLE_MSG(hWindow, WM_SYSKEYUP, Radio_OnSysKeyUp);
	HANDLE_MSG(hWindow, BTNM_CLICK, Radio_OnUserMsg);
	HANDLE_MSG(hWindow, BTNM_GETCHECK, Radio_OnUserMsg);
	HANDLE_MSG(hWindow, BTNM_GETSTATE, Radio_OnUserMsg);
	HANDLE_MSG(hWindow, BTNM_SETSTATE, Radio_OnUserMsg);
	HANDLE_MSG(hWindow, BTNM_SETCHECK, Radio_OnUserMsg);
	#ifndef WINDOWS95_HACK
	HANDLE_MSG(hWindow, BTNM_GETCHECK16, Radio_OnUserMsg);
	HANDLE_MSG(hWindow, BTNM_GETSTATE16, Radio_OnUserMsg);
	HANDLE_MSG(hWindow, BTNM_SETSTATE16, Radio_OnUserMsg);
	HANDLE_MSG(hWindow, BTNM_SETCHECK16, Radio_OnUserMsg);
	#endif

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}
#ifdef DLGEDIT
}
#endif
/***********************************************************************/
LOCAL BOOL Radio_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
    HDC hDC = GetDC (hWindow);

    SetWindowWord (hWindow,GWW_STATE,0);
    SetWindowWord (hWindow,GWL_HBITMAP,0);
    SetWindowWord (hWindow,GWL_FONT,0);
    SetWindowLong (hWindow,GWL_RADIOCOLOR,CTLCOLOR_DEFAULT);
    SetWindowLong (hWindow,GWL_PRVCLICK, 0L);
    if (hDC)
    {
        if (nControls == 0)
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
LOCAL void Radio_OnDestroy(HWND hWindow)
/***********************************************************************/
{
    HBITMAP hBitmap = (HBITMAP)GetWindowLong (hWindow,GWL_HBITMAP);

    if (hBitmap)
        DeleteObject (hBitmap);
    nControls -= (nControls > 0) ? 1 : 0;
}

/***********************************************************************/
LOCAL void Radio_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
    InvalidateRect (hWindow,(RECT far *)NULL,TRUE);
    UpdateWindow (hWindow);
}

/***********************************************************************/
LOCAL BOOL Radio_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
	return(TRUE);
}

/***********************************************************************/
LOCAL void Radio_OnPaint(HWND hWindow)
/***********************************************************************/
{
    PAINTSTRUCT ps;
    HBITMAP hOldBitmap, hBitmap = (HBITMAP)GetWindowLong (hWindow,GWL_HBITMAP);
    HDC hWindowDC = GetDC (hWindow);
    HDC hMemDC = CreateCompatibleDC (hWindowDC);

    ReleaseDC (hWindow,hWindowDC);
    if (IsWindowVisible (hWindow) && hMemDC && hBitmap && 
	 (BeginPaint (hWindow,&ps) != NULL)) 
    { 
        RECT rc;
        UINT wState = GetWindowWord (hWindow,GWW_STATE);
        HBRUSH hOldBrush, hBtnFillBrush = Ctl_GetBackgroundBrush( hWindow, hWindowDC, CTLCOLOR_BTN );
        HFONT hOldFont, hNewFont = (HFONT)GetWindowLong (hWindow,GWL_FONT);
        long lType = GetWindowLong (hWindow,GWL_STYLE) & MASK_TYPE;
        
        GetClientRect (hWindow,&rc);
        hOldBitmap = (HBITMAP)SelectObject (hMemDC,hBitmap);
        hOldBrush = (HBRUSH)SelectObject (hMemDC,hBtnFillBrush);
        if (hNewFont) 
            hOldFont = (HFONT)SelectObject (hMemDC,hNewFont);
        PatBlt (hMemDC,0,0,rc.right,rc.bottom,PATCOPY);
        if (lType == RBS_OWNERDRAW)
        {
            Radio_OwnerDrawNotify (hWindow,hMemDC,ODA_DRAWENTIRE,
                (wState & BFFOCUS ? ODS_FOCUS : 0) | 
                (wState & BFHILIGHT ? ODS_SELECTED : 0));
        }
        else if (lType == RBS_PUSHBTNDMD || lType == RBS_PUSHBTNLED)
            Radio_DrawPushBtn (hWindow,hMemDC,hBtnFillBrush,wState);
        else
            Radio_DrawBtn (hWindow,hMemDC,wState);
        BitBlt (ps.hdc,0,0,rc.right,rc.bottom,hMemDC,0,0,SRCCOPY);
        if (hNewFont) 
            SelectObject (hMemDC,hOldFont);
        SelectObject (hMemDC,hOldBrush);
        SelectObject (hMemDC,hOldBitmap);
 //       DeleteObject (hBtnFillBrush);
        EndPaint (hWindow,&ps);
    }
    if (hMemDC)
        DeleteDC (hMemDC);
}

/***********************************************************************/
LOCAL UINT Radio_OnGetDlgCode(HWND hWindow, LPMSG lpMsg)
/***********************************************************************/
{
    return DLGC_RADIOBUTTON | DLGC_BUTTON;
}

/***********************************************************************/
LOCAL HFONT Radio_OnGetFont(HWND hWindow)
/***********************************************************************/
{
    return (HFONT)GetWindowLong (hWindow,GWL_FONT);
}

/***********************************************************************/
LOCAL void Radio_OnSetFont(HWND hWindow, HFONT hFont, BOOL fRedraw)
/***********************************************************************/
{
    SetWindowLong (hWindow,GWL_FONT,(long)hFont);
    if (fRedraw)
    {
        InvalidateRect (hWindow,NULL,TRUE);
        UpdateWindow (hWindow);
    }
}

/***********************************************************************/
LOCAL void Radio_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
    UINT wState = GetWindowWord (hWindow,GWW_STATE);
    
    if (!(wState & BFMOUSE))
    {
        if (vk == VK_SPACE) 
        {
            if (!(wState & BFINCLICK))
            {
                Radio_SetCapture (hWindow,FALSE);
                SendMessage (hWindow,BTNM_SETSTATE,TRUE,0L);
            }
        } 
        else     
            Radio_ReleaseCapture (hWindow,FALSE);
    }
}

/***********************************************************************/
LOCAL void Radio_OnKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
    if ((GetWindowWord (hWindow,GWW_STATE) & BFMOUSE) ||
        (vk == VK_TAB))
        return;
    else
    {
        Radio_ReleaseCapture (hWindow,vk == VK_SPACE);
        return;
    }
}

/***********************************************************************/
LOCAL void Radio_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
    UINT wState = GetWindowWord (hWindow,GWW_STATE);
    
    if (wState & BFCAPTURED)
        Radio_ReleaseCapture (hWindow,TRUE);
    wState = GetWindowWord (hWindow,GWW_STATE);
    wState &= ~BFFOCUS;
    SetWindowWord (hWindow,GWW_STATE,wState);
    InvalidateRect (hWindow,NULL,FALSE);
}

/***********************************************************************/
LOCAL void Radio_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
    RECT rControl;
    DWORD thisClick = GetTickCount();
    DWORD sinceLastClick = thisClick - GetWindowLong (hWindow,GWL_PRVCLICK);
	 POINT ptPosition;

	 ptPosition.x = x;
	 ptPosition.y = y;

	 /* HACK ALERT - dlgmgr calls BTNM_CLICK very fast during arrow tabbing 
       that cause unintended doublclicks. We use the dummy flag in the wparam
       to indicate the down event was sent by our click event.
       Fix made for ABC flowcharter,
    */

    if (keyFlags != DUMMY_FLAG &&              
        sinceLastClick <= GetDoubleClickTime ()) 
    {
        SetWindowLong(hWindow,GWL_PRVCLICK, 0L);  // so the third click is not interpreted
        NOTIFY_PARENT(hWindow,CTLN_DOUBLECLICKED);
    } 
    else 
    {
        SetWindowLong(hWindow,GWL_PRVCLICK,thisClick);
        Radio_SetCapture (hWindow,TRUE);
        GetClientRect (hWindow,(RECT far *)&rControl);
        SendMessage (hWindow,BTNM_SETSTATE,
		 PtInRect ((RECT far *)&rControl,ptPosition),0L);
    }   
}

/***********************************************************************/
LOCAL void Radio_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	 if (GetWindowWord (hWindow,GWW_STATE) & BFMOUSE)
        Radio_ReleaseCapture (hWindow,TRUE);
}

/***********************************************************************/
LOCAL void Radio_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	 POINT ptPosition;
    UINT wState = GetWindowWord (hWindow,GWW_STATE);

	 ptPosition.x = x;
	 ptPosition.y = y;

    
    if (wState & BFMOUSE && wState & BFCAPTURED)
    {
        RECT rControl;
        
        GetClientRect (hWindow,(RECT far *)&rControl);
        if (PtInRect ((RECT far *)&rControl,ptPosition))  
        {
            if (!(wState & BFHILIGHT))
                SendMessage (hWindow,BTNM_SETSTATE,TRUE,0L);
        }
        else if (wState & BFHILIGHT)
            SendMessage (hWindow,BTNM_SETSTATE,FALSE,0L);
    }
}

/***********************************************************************/
LOCAL void Radio_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
    WORD wState = GetWindowWord (hWindow,GWW_STATE);

    wState |= BFFOCUS;
    SetWindowWord (hWindow,GWW_STATE,wState);
    InvalidateRect (hWindow,NULL,FALSE);
    if (!(wState & BFDONTCLICK) && !(wState & BFINCLICK) && !(wState & BFCHECK))
        NOTIFY_PARENT (hWindow,CTLN_CLICKED);
}

/***********************************************************************/
LOCAL void Radio_OnSize(HWND hWindow, UINT state, int cx, int cy)
/***********************************************************************/
{
    HDC hDC = GetDC (hWindow);
    
    if (hDC && cx > 0  && cy > 0)
    {
        HBITMAP hBitmap = (HBITMAP)GetWindowLong (hWindow,GWL_HBITMAP);

        if (hBitmap)
            DeleteObject (hBitmap);
        hBitmap = CreateCompatibleBitmap (hDC,cx,cy);
        SetWindowLong (hWindow,GWL_HBITMAP,(long)hBitmap);
        InvalidateRect (hWindow,NULL,FALSE);
        UpdateWindow (hWindow); 
    }   
    if (hDC)
        ReleaseDC (hWindow,hDC);
}

/***********************************************************************/
void Radio_OnSysKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
    if (!((GetWindowWord (hWindow,GWW_STATE) & BFMOUSE) ||
        (vk == VK_TAB)))
        Radio_ReleaseCapture (hWindow,vk == VK_SPACE);
}

/***********************************************************************/
LOCAL long Radio_OnUserMsg (HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
	long lResult = 0;

	switch (msg)
		{
        case BTNM_CLICK:
            Radio_OnClick (hWindow);
            break;
        case BTNM_GETCHECK:       
            lResult = (long)Radio_OnGetCheck (hWindow);
            break;
        case BTNM_GETSTATE:     
            lResult = Radio_OnGetState (hWindow);
            break;
        case BTNM_SETSTATE:
            Radio_OnSetState (hWindow, (BOOL)wParam);
            break;
        case BTNM_SETCHECK:    
            Radio_OnSetCheck (hWindow, (BOOL)wParam);
            break;
		 }
	return(lResult);
}

LOCAL void   Radio_DrawBtn (
HWND hWindow,
HDC hDC,
UINT wState)
{
    RECT rBox, rText;
    int OldBkMode, OldMapMode;
    long lStyle = GetWindowLong (hWindow,GWL_STYLE);
    int iTextLen = GetWindowTextLength (hWindow);
    LPTSTR  lpText;

    Radio_GetBtnRect (hWindow,lStyle,&rBox);
    OldMapMode = SetMapMode (hDC,MM_TEXT);
    Radio_DrawBtnBtn (hWindow,hDC,wState,lStyle,rBox);
    if (iTextLen && (lpText = (LPTSTR)new_mem ((ULONG)iTextLen+1)))
    {
        ULONG rgbOldTextColor;

        GetWindowText (hWindow,lpText,iTextLen+1);
        Radio_GetTextRect (hWindow,lStyle,&rText);
        if (lStyle & WS_DISABLED)
            rgbOldTextColor = SetTextColor (hDC,Ctl_GetDisabledBtnTextColor());
        else
            rgbOldTextColor = SetTextColor (hDC,Ctl_GetBtnTextColor());
        OldBkMode = SetBkMode (hDC,TRANSPARENT);
        DrawText (hDC,lpText,-1,&rText,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
        if (wState & BFFOCUS)
        {
			SIZE StrSize;
			UINT StrWidth;
		 	GetTextExtentPoint (hDC,lpText,lstrlen (lpText), &StrSize);
			StrWidth = StrSize.cx;

            rText.right = min ((int)StrWidth+rText.left,rText.right);
            InflateRect (&rText,2*cxBorder,cyBorder);
            if (lStyle & RBS_STDFOCUS)
            {
                ULONG rgbOldBkColor;

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
        del_mem (lpText);
    }
    SetMapMode (hDC,OldMapMode);
}

LOCAL void   Radio_DrawBtnBtn (
HWND hWindow,
HDC hDC,
UINT wState,
long lStyle,
RECT rBox)
{
    long lType = lStyle & MASK_TYPE;

    if (wState & BFHILIGHT || wState & BFCHECK)
        Ctl_DrawBevel (hDC,
 	 				Ctl_GetBtnShadowColor(),
 	 				Ctl_GetBtnHilightColor(),
 			        rBox,1,DB_IN | (lType == RBS_LED ? DB_CIRCLE : DB_DIAMOND));
    else
        Ctl_DrawBevel (hDC,
 	 				Ctl_GetBtnShadowColor(),
 	 				Ctl_GetBtnHilightColor(),
            		rBox,1,DB_OUT | (lType == RBS_LED ? DB_CIRCLE : DB_DIAMOND));
    if (!((wState & BFHILIGHT) || !(wState & BFCHECK)))
    {
        ULONG rgbRadio = GetWindowLong (hWindow,GWL_RADIOCOLOR);

        if (rgbRadio == CTLCOLOR_DEFAULT)
            rgbRadio = RGB(0,255,0); // green
        InflateRect (&rBox,-3*cxBorder,-3*cyBorder);
        if (lType == RBS_LED)
            Ctl_DrawLed (hDC,rgbRadio,rBox,!(lStyle & WS_DISABLED));
        else //if(RBS_DIAMOND)
            Ctl_DrawDiamond (hDC,rgbRadio,rBox);
    }
}

LOCAL void   Radio_DrawPushBtn (
HWND hWindow,
HDC hDC,
HBRUSH hBkground,
UINT wState)
{
    RECT rButton, rInside;
    long lStyle = GetWindowLong (hWindow,GWL_STYLE);
    long lType = lStyle & MASK_TYPE;
    int OldMapMode = SetMapMode (hDC,MM_TEXT);
    int OldBkMode = SetBkMode (hDC,TRANSPARENT);
    int iTextLen = GetWindowTextLength (hWindow);
    LPTSTR lpText;

    GetClientRect (hWindow,&rButton);
    Ctl_DrawPushButton (hDC,&rButton,DP_THINBEVEL,((wState & BFHILIGHT) ? 
        PBSTATE_DOWN : PBSTATE_UP),hBkground);
    if (iTextLen && (lpText = (LPTSTR)new_mem ((ULONG)iTextLen+1)))
    {
        ULONG rgbOldTextColor;
        ULONG rgbRadio = GetWindowLong (hWindow,GWL_RADIOCOLOR);
		 SIZE StrExt;
        int xStr, yStr;
        UINT wBtnSize;
    
        if (rgbRadio == CTLCOLOR_DEFAULT)
            rgbRadio = RGB(0,255,0);
        GetWindowText (hWindow,lpText,iTextLen+1);
        GetTextExtentPoint (hDC,lpText,iTextLen, &StrExt);
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
        if (lStyle & RBS_LEFTTEXT)
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
        Ctl_DrawBevel (hDC,
			Ctl_GetBtnShadowColor(),
			Ctl_GetBtnHilightColor(),
            rInside,1,DB_IN | 
            (lType == RBS_PUSHBTNLED ? DB_CIRCLE : DB_DIAMOND));
        if ((wState & BFCHECK) == 1)
        {
            //InflateRect (&rInside,-2*cxBorder,-2*cyBorder);
            InflateRect (&rInside,-cxBorder,-cyBorder);
            if (lType == RBS_PUSHBTNLED)
                Ctl_DrawLed (hDC,rgbRadio,rInside,!(lStyle & WS_DISABLED));
            else
                Ctl_DrawDiamond (hDC,rgbRadio,rInside);
        }
        DrawText (hDC,lpText,-1,&rButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
        if (wState & BFFOCUS)
        {
            rButton.top += max ((rButton.bottom-rButton.top-yStr+1)/2,0);
            rButton.right = min (rButton.left+xStr,rButton.right);
            rButton.bottom = min (rButton.top+yStr,rButton.bottom);

            InflateRect (&rButton,cxBorder,cyBorder);
            if (lStyle & RBS_STDFOCUS)
            {
                ULONG rgbOldBkColor; 

                rgbOldBkColor = SetBkColor (hDC,Ctl_GetBtnFaceColor());
                SetTextColor (hDC,Ctl_GetBtnTextColor());
                DrawFocusRect (hDC,&rButton);
                SetBkColor (hDC,rgbOldBkColor);
            }
            else    
                Ctl_DrawBevel (hDC,
					Ctl_GetBtnShadowColor(),
					Ctl_GetBtnHilightColor(),
                   rButton,1,DB_RECT|DB_IN);
        }
        SetTextColor (hDC,rgbOldTextColor); 
        del_mem (lpText);
    }
    SetBkMode (hDC,OldBkMode);
    SetMapMode (hDC,OldMapMode);
}    

LOCAL void   Radio_GetBtnRect (
HWND hWindow,
long lStyle,
RECT far * lpRect)
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
    BoxSize = max (BOX_HEIGHT,(5 * CharH / 6));
    if (lpRect->bottom >= BoxSize)
    {
        lpRect->top = (lpRect->bottom - BoxSize) / 2;
        lpRect->bottom = lpRect->top + BoxSize;
    }
    if (lpRect->right >= BoxSize)
    {
        if (lStyle & RBS_LEFTTEXT)
            lpRect->left = lpRect->right - BoxSize;
        else
            lpRect->right = lpRect->left + BoxSize;
    }
}

LOCAL void   Radio_GetTextRect (
HWND hWindow,
long lStyle,
RECT far * lpRect)
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
        if (lStyle & RBS_LEFTTEXT)
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

LOCAL VOID   Radio_OwnerDrawNotify (
HWND hWindow,
HDC hDC,
UINT wAction,
UINT wState)
{
    DRAWITEMSTRUCT drawItemStruct;
    UINT wControlId = GetDlgCtrlID (hWindow);

    drawItemStruct.CtlType    = ODT_BUTTON;
    drawItemStruct.CtlID      = wControlId;
    drawItemStruct.itemAction = wAction;
    drawItemStruct.hwndItem   = hWindow;
    drawItemStruct.hDC        = hDC;
    GetClientRect (hWindow,(RECT far *)(&drawItemStruct.rcItem));
    drawItemStruct.itemData   = 0L;
    drawItemStruct.itemState  = wState | 
        (IsWindowEnabled (hWindow) ? 0 : ODS_DISABLED);
    SendMessage (GetParent (hWindow),WM_DRAWITEM,wControlId, 
        (long)((LPDRAWITEMSTRUCT)&drawItemStruct));
}

LOCAL void   Radio_ReleaseCapture (
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
            if (lStyle & RBS_AUTO)
            {
               if (wState & BFCHECK)
               {
                   InvalidateRect (hWindow,NULL,FALSE);
                   UpdateWindow (hWindow); 
               }    
               else
               {
                    HWND hNextRadioBtn = hWindow;  
                    do
                    {
                        if ((UINT)SendMessage (hNextRadioBtn,
                            WM_GETDLGCODE,0,0L) & DLGC_RADIOBUTTON)
                            SendMessage (hNextRadioBtn,BTNM_SETCHECK,
                                (hWindow == hNextRadioBtn),0L);
                        hNextRadioBtn = GetNextDlgGroupItem (
                            GetParent (hNextRadioBtn),hNextRadioBtn,FALSE);
                    }   while (hNextRadioBtn != hWindow);
               }
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

LOCAL void   Radio_SetCapture (
HWND hWindow,
BOOL bMouseInitiated)
{
    UINT wState = GetWindowWord (hWindow,GWW_STATE);

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

LOCAL void   Radio_OnClick (
HWND hWindow)
{
    SendMessage (hWindow,WM_LBUTTONDOWN,DUMMY_FLAG,0L);
    SendMessage (hWindow,WM_LBUTTONUP,DUMMY_FLAG,0L); 
	 Radio_OnKeyDown(hWindow, 0, FALSE, 0, 0);
}

LOCAL UINT   Radio_OnGetCheck (
HWND hWindow)
{
    return(UINT)(GetWindowWord (hWindow,GWW_STATE) & BFCHECK);
}

LOCAL BOOL   Radio_OnGetState (
HWND hWindow)
{
    return (BOOL) GetWindowWord (hWindow,GWW_STATE);
}

LOCAL void   Radio_OnSetCheck (
HWND hWindow,
UINT wCheck)
{
    UINT wState = GetWindowWord (hWindow,GWW_STATE);
    long lStyle = GetWindowLong (hWindow,GWL_STYLE);

    if (wCheck)
        SetWindowLong (hWindow,GWL_STYLE,lStyle |= WS_TABSTOP);
    else
        SetWindowLong (hWindow,GWL_STYLE,lStyle &= ~WS_TABSTOP);
    wCheck = (wCheck > 0 ? 1 : 0);
    if ((wState & BFCHECK) != wCheck) 
    {
        wState = ((wState & ~BFCHECK) | wCheck);
        SetWindowWord (hWindow,GWW_STATE,wState);
        InvalidateRect (hWindow,NULL,FALSE);
        UpdateWindow (hWindow); 
    }
}

LOCAL void   Radio_OnSetState (
HWND hWindow,
BOOL bHilight)
{
    UINT wState = GetWindowWord (hWindow,GWW_STATE);

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






