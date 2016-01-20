// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mgxbtn.h"

/****************************************************************************

                                     MGXBTN.C

 ****************************************************************************
 ****************************************************************************

 Name:         Micrografx 3D button class
 Description:  Provide 3D look to standard button control
 Date Created: 16 OCT 90
 Author:       Kevin McFarland and Kurt Delimon

 ****************************************************************************/

/********************************* Constants ********************************/

#define LOCAL static

#define RED         0x000000FFL
#define GREEN       0x0000FF00L
#define BLUE        0x00FF0000L
#define YELLOW      0x0000FFFFL
#define CYAN        0x00FFFF00L
#define MAGENTA     0x00FF00FFL
#define WHITE       0x00FFFFFFL
#define LIGHT_GRAY  0x00C0C0C0L
#define MEDIUM_GRAY 0x00808080L
#define DARK_GRAY   0x00404040L
#define BLACK       0x00000000L

#define RGBC        COLORREF    

#define GR_RADIOBTN_BOX   1
#define GR_RADIOBTN_TXT   2
#define GR_CHECKBOX_BOX   3
#define GR_CHECKBOX_TXT   4
#define GR_GROUPBOX_BOX   5
#define GR_GROUPBOX_TXT   6
#define GR_GROUPBOX_LIN   7
#define GR_PUSHBTN_BOX    8
#define GR_PUSHBTN_TXT    9

#define DB_IN             0
#define DB_OUT            1
#define DB_RECT           2
#define DB_CIRCLE         4

/*********************************** Macros *********************************/

#define SWAP(A,B) {A^=B; B^=A; A^=B;}

/*********************************** Types **********************************/
/********************************* Local Data *******************************/

LOCAL HBRUSH hBtnFillBrush;
LOCAL COLORREF rgbDark;
LOCAL COLORREF rgbLight;

/******************************* Exported Data ******************************/
/****************************** Local Functions *****************************/

LOCAL void   NEAR PASCAL draw_bevel (HDC,HPEN,HPEN,LPRECT,WORD,WORD);
LOCAL void   NEAR PASCAL draw_checkbox (HWND,HDC,WORD);
LOCAL void   NEAR PASCAL draw_groupbox (HWND,HDC,WORD);
LOCAL void   NEAR PASCAL draw_pushbtn (HWND,HDC,WORD);
LOCAL void   NEAR PASCAL draw_radiobtn (HWND,HDC,WORD);
LOCAL void   NEAR PASCAL get_avg_char_size (HWND,WORD FAR *,WORD FAR *);
LOCAL void   NEAR PASCAL get_btn_rect (HWND,WORD,WORD,LONG,LPRECT);
LOCAL HBRUSH NEAR PASCAL get_control_brush (HWND,HDC,WORD);
LOCAL LONG   NEAR PASCAL get_dlg_code (WORD);
LOCAL RGBC   NEAR PASCAL get_led_color (LONG);
LOCAL BOOL   NEAR PASCAL get_parentdc_clipbox (HWND,HDC,LPRECT);
LOCAL void   NEAR PASCAL kill_mgxbtn_focus (HWND,WORD);
LOCAL void   NEAR PASCAL notify_parent (HWND,WORD);
LOCAL void   NEAR PASCAL release_mgxbtn_capture (HWND,BOOL);
LOCAL void   NEAR PASCAL set_mgxbtn_capture (HWND,BOOL);
LOCAL void   NEAR PASCAL set_mgxbtn_check (HWND,LONG,WORD,WORD);
LOCAL void   NEAR PASCAL set_mgxbtn_focus (HWND,WORD);
LOCAL void   NEAR PASCAL set_mgxbtn_hilight (HWND,WORD,BOOL);
LOCAL void   NEAR PASCAL paint_mgxbtn (HWND,WORD,WORD);

LOCAL void NEAR PASCAL draw_bevel (hDC,hDarkPen,hLightPen,lpRect,wThickness,wType)
HDC    hDC;
HPEN   hDarkPen;
HPEN   hLightPen;
LPRECT lpRect;
WORD   wThickness;
WORD   wType;
{
    int i;
    
    if (wType & DB_OUT)
        SWAP(hDarkPen,hLightPen);

    if (wType & DB_CIRCLE)
    {
        for (i=0;i<wThickness;i++)
        {
            SelectObject (hDC,hDarkPen);
            Arc (hDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom,
                lpRect->right,lpRect->top,lpRect->left,lpRect->bottom);
            SelectObject (hDC,hLightPen);
            Arc (hDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom,
                lpRect->left,lpRect->bottom,lpRect->right,lpRect->top);
            InflateRect (lpRect,-1,-1);
        }
    }
    else //(wType & DB_RECT)
    {
        for (i=0;i<wThickness;i++)
        {
            SelectObject (hDC,hDarkPen);
            MoveTo (hDC,lpRect->left+i,lpRect->bottom-i);
            LineTo (hDC,lpRect->left+i,lpRect->top+i);    
            LineTo (hDC,lpRect->right-i,lpRect->top+i);    
            SelectObject (hDC,hLightPen);
            LineTo (hDC,lpRect->right-i,lpRect->bottom-i);    
            LineTo (hDC,lpRect->left+i,lpRect->bottom-i);    
        }
    }

}

LOCAL void NEAR PASCAL draw_checkbox (hControl,hDC,wState)
HWND hControl; 
HDC  hDC;
WORD wState;
{
    RECT     rBox, rText, rClient;
    HPEN     hDarkPen, hLightPen, hFocusPen, hOldPen;
    HBRUSH   hOldBrush, hLEDBrush;
    HFONT    hOldFont, hFont;
    int      OldBkMode, OldMapMode;
    LONG     lStyle = GetWindowLong (hControl,GWL_STYLE);
    COLORREF rgbOldTextColor;
    COLORREF rgbLED = get_led_color (lStyle);
    char     szBtnText[80];

    if ((hFont = GetWindowWord(hControl,GWW_FONT)))
	hOldFont = SelectObject(hDC, hFont);

    get_btn_rect (hControl,GR_CHECKBOX_BOX,wState,lStyle,&rBox);
    OldMapMode = SetMapMode (hDC,MM_TEXT);
    hDarkPen = CreatePen (PS_SOLID,1,rgbDark);
    hLightPen = CreatePen (PS_SOLID,1,rgbLight);
    hOldPen = SelectObject (hDC,hDarkPen);
    if (wState & BFHILIGHT)
    {
        draw_bevel (hDC,hDarkPen,hLightPen,&rBox,1,DB_RECT | DB_IN);
        InflateRect (&rBox,-3,-3);
        draw_bevel (hDC,hDarkPen,hLightPen,&rBox,1,DB_RECT | DB_IN);
    }
    else
    {
        draw_bevel (hDC,hDarkPen,hLightPen,&rBox,1,DB_RECT | DB_IN);
        InflateRect (&rBox,-3,-3);
        draw_bevel (hDC,hDarkPen,hLightPen,&rBox,1,DB_RECT | DB_OUT);
    }
    if (wState & BFCHECK)
    {
        InflateRect (&rBox,-1,-1);
        rBox.bottom++;
        rBox.right++;
        if ((wState & BFCHECK) == 1)
            hLEDBrush = CreateSolidBrush (rgbLED);
        else
            hLEDBrush = CreateSolidBrush (BLACK);
        hOldBrush = SelectObject (hDC,hLEDBrush);
        // remove MGXLIB off by one fix.
        // rBox.right--;
        // rBox.bottom--;
        FillRect (hDC,&rBox,hLEDBrush);
        //if (!(lStyle & WS_DISABLED))
        //{
        //    SelectObject (hDC,hLightPen);
        //    MoveTo (hDC,rBox.left+1,rBox.top+2);
        //    LineTo (hDC,rBox.left+1,rBox.top+1);    
        //    LineTo (hDC,rBox.left+3,rBox.top+1);    
        //}
        SelectObject (hDC,hOldBrush);
        DeleteObject (hLEDBrush);
    }
    if (GetWindowText (hControl,szBtnText,80))
    {
        get_btn_rect (hControl,GR_CHECKBOX_TXT,wState,lStyle,&rText);
        if (lStyle & WS_DISABLED)
            rgbOldTextColor = SetTextColor (hDC,MEDIUM_GRAY); 
        else 
            rgbOldTextColor = SetTextColor (hDC,BLACK); 
        OldBkMode = SetBkMode (hDC,TRANSPARENT); 
        DrawText (hDC,szBtnText,-1,&rText,DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        if (wState & BFFOCUS && (lStyle & WS_TABSTOP))
        {
            WORD StrWidth = (WORD)(GetTextExtent (hDC,szBtnText,
                                    lstrlen (szBtnText)));
            
//            rText.right = min (StrWidth+rText.left,rText.right);
//            InflateRect (&rText,1,1);
	    GetClientRect(hControl, &rClient);
	    rText.left -= 2;
	    rText.right = rClient.right - 1;
	    rText.top = rClient.top;
	    rText.bottom = rClient.bottom - 1;
	    draw_bevel (hDC,hDarkPen,hLightPen,&rText,1,DB_RECT | DB_IN);
            //hFocusPen = CreatePen (PS_DOT,1,BLACK);
            //SelectObject (hDC,hFocusPen);
            //hOldBrush = SelectObject (hDC,GetStockObject (NULL_BRUSH));
            //Rectangle (hDC,rText.left,rText.top,rText.right,rText.bottom);
            //SelectObject (hDC,hOldPen);
            //SelectObject (hDC,hOldBrush);
            //DeleteObject (hFocusPen);
        }
        SetBkMode (hDC,OldBkMode);
        SetTextColor (hDC,rgbOldTextColor); 
    }
    if (hFont)
	SelectObject(hDC, hOldFont);
    SetMapMode (hDC,OldMapMode);
    SelectObject (hDC,hOldPen);
    DeleteObject (hDarkPen);
    DeleteObject (hLightPen);
}    

LOCAL void NEAR PASCAL draw_groupbox (hControl,hDC,wState)
HWND hControl; 
HDC  hDC;
WORD wState;
{
    RECT     rBox, rText;
    HPEN     hDarkPen, hLightPen, hOldPen;
    int      OldBkMode, OldMapMode;
    LONG     lStyle = GetWindowLong (hControl,GWL_STYLE);
    COLORREF rgbOldTextColor;
    char     szBtnText[80];

    get_btn_rect (hControl,GR_GROUPBOX_BOX,wState,lStyle,&rBox);
    IntersectClipRect (hDC,0,0,rBox.right+1,rBox.bottom+1);
    OldMapMode = SetMapMode (hDC,MM_TEXT);
    hDarkPen = CreatePen (PS_SOLID,1,rgbDark);
    hLightPen = CreatePen (PS_SOLID,1,rgbLight);
    hOldPen = SelectObject (hDC,hDarkPen);
    draw_bevel (hDC,hDarkPen,hLightPen,&rBox,1,DB_RECT | DB_IN);
    if (GetWindowText (hControl,szBtnText,80))
    {
        get_btn_rect (hControl,GR_GROUPBOX_TXT,wState,lStyle,&rText);
        if (lStyle & WS_DISABLED)
            rgbOldTextColor = SetTextColor (hDC,MEDIUM_GRAY); 
        else 
            rgbOldTextColor = SetTextColor (hDC,BLACK); 
        OldBkMode = SetBkMode (hDC,TRANSPARENT); 
        DrawText (hDC,szBtnText,-1,&rText,DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        get_btn_rect (hControl,GR_GROUPBOX_LIN,wState,lStyle,&rText);
        SelectObject (hDC,hLightPen);
        MoveTo (hDC,rText.left,rText.top);
        LineTo (hDC,rText.right,rText.top);    
        SelectObject (hDC,hDarkPen);
        MoveTo (hDC,rText.right,rText.top+1);
        LineTo (hDC,rText.left,rText.top+1);    
        SetBkMode (hDC,OldBkMode);
        SetTextColor (hDC,rgbOldTextColor); 
    }
    SetMapMode (hDC,OldMapMode);
    SelectObject (hDC,hOldPen);
    DeleteObject (hDarkPen);
    DeleteObject (hLightPen);
}    
    

LOCAL void NEAR PASCAL draw_pushbtn (hControl,hDC,wState)
HWND hControl; 
HDC  hDC;
WORD wState;
{
    RECT     rBox, rText;
    HPEN     hDarkPen, hLightPen, hBorderPen, hOldPen;
    HBRUSH   hOldBrush;
    int      OldBkMode, OldMapMode;
    LONG     lStyle = GetWindowLong (hControl,GWL_STYLE);
    COLORREF rgbOldTextColor;
    COLORREF rgbLED = get_led_color (lStyle);
    char     szBtnText[80];

    get_btn_rect (hControl,GR_PUSHBTN_BOX,wState,lStyle,&rBox);
    OldMapMode = SetMapMode (hDC,MM_TEXT);
    hDarkPen = CreatePen (PS_SOLID,1,rgbDark);
    hLightPen = CreatePen (PS_SOLID,1,rgbLight);
    hBorderPen = CreatePen (PS_SOLID,1,BLACK);
    hOldPen = SelectObject (hDC,hDarkPen);
    if (wState & BFHILIGHT)
        draw_bevel (hDC,hDarkPen,hLightPen,&rBox,3,DB_RECT | DB_IN);
    else
        draw_bevel (hDC,hDarkPen,hLightPen,&rBox,3,DB_RECT | DB_OUT);
    InflateRect (&rBox,1,1);
    SelectObject (hDC,hBorderPen);
    SelectObject (hDC,GetStockObject (NULL_BRUSH));
    Rectangle (hDC,rBox.left,rBox.top,rBox.right+1,rBox.bottom+1);
    if (GetWindowText (hControl,szBtnText,80))
    {
        get_btn_rect (hControl,GR_PUSHBTN_TXT,wState,lStyle,&rText);
        OldBkMode = SetBkMode (hDC,TRANSPARENT); 
        if (wState & BFHILIGHT)
            SWAP (rgbLight,rgbDark);
        rgbOldTextColor = SetTextColor (hDC,rgbLight); 
        DrawText (hDC,szBtnText,-1,&rText,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SetTextColor (hDC,rgbDark); 
        OffsetRect (&rText,2,2);
        DrawText (hDC,szBtnText,-1,&rText,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        if (lStyle & WS_DISABLED)
            SetTextColor (hDC,MEDIUM_GRAY); 
        else 
        {
            if (wState & BFFOCUS)
               SetTextColor (hDC,rgbLED);
            else
               SetTextColor (hDC,BLACK);
        }
        OffsetRect (&rText,-1,-1);
        DrawText (hDC,szBtnText,-1,&rText,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SetBkMode (hDC,OldBkMode);
        SetTextColor (hDC,rgbOldTextColor); 
    }
    SetMapMode (hDC,OldMapMode);
    SelectObject (hDC,hOldPen);
    DeleteObject (hDarkPen);
    DeleteObject (hLightPen);
    DeleteObject (hBorderPen);
}    

LOCAL void NEAR PASCAL draw_radiobtn (hControl,hDC,wState)
HWND hControl; 
HDC  hDC;
WORD wState;
{
    RECT     rBox, rText, rClient;
    HPEN     hDarkPen, hLightPen, hBorderPen, hOldPen;
    HBRUSH   hOldBrush, hLEDBrush;
    int      OldBkMode, OldMapMode;
    LONG     lStyle = GetWindowLong (hControl,GWL_STYLE);
    COLORREF rgbOldTextColor;
    COLORREF rgbLED = get_led_color (lStyle);
    char     szBtnText[80];
    HFONT    hOldFont, hFont;
    
    if ((hFont = GetWindowWord(hControl,GWW_FONT)))
	hOldFont = SelectObject(hDC, hFont);

    get_btn_rect (hControl,GR_RADIOBTN_BOX,wState,lStyle,&rBox);
    OldMapMode = SetMapMode (hDC,MM_TEXT);
    hDarkPen = CreatePen (PS_SOLID,1,rgbDark);
    hLightPen = CreatePen (PS_SOLID,1,rgbLight);
    hOldPen = SelectObject (hDC,hDarkPen);
    if (!((wState & BFCHECK) || (wState & BFHILIGHT)))
        draw_bevel (hDC,hDarkPen,hLightPen,&rBox,1,DB_CIRCLE | DB_OUT);
    else
        draw_bevel (hDC,hDarkPen,hLightPen,&rBox,1,DB_CIRCLE | DB_IN);
    if (!((wState & BFHILIGHT) || !(wState & BFCHECK)))
    {
        InflateRect (&rBox,-2,-2);
        if ((wState & BFCHECK) == 1)
            hLEDBrush = CreateSolidBrush (rgbLED);
        else
            hLEDBrush = CreateSolidBrush (BLACK);
        hBorderPen = CreatePen (PS_SOLID,1,rgbLED);
        SelectObject (hDC,hBorderPen);
        hOldBrush = SelectObject (hDC,hLEDBrush);
        Ellipse (hDC,rBox.left,rBox.top,rBox.right,rBox.bottom);
        if (!(lStyle & WS_DISABLED))
        {
            InflateRect (&rBox,-2,-2);
            SelectObject (hDC,hLightPen);
            Arc (hDC,rBox.left,rBox.top,rBox.right,rBox.bottom,
                (rBox.left+rBox.right)/2,rBox.top,
                rBox.left,(rBox.top+rBox.bottom)/2);
        }
        SelectObject (hDC,hOldBrush);
        SelectObject (hDC,hOldPen);
        DeleteObject (hBorderPen);
        DeleteObject (hLEDBrush);
    }
    if (GetWindowText (hControl,szBtnText,80))
    {
        get_btn_rect (hControl,GR_RADIOBTN_TXT,wState,lStyle,&rText);
        if (lStyle & WS_DISABLED)
            rgbOldTextColor = SetTextColor (hDC,MEDIUM_GRAY); 
        else 
            rgbOldTextColor = SetTextColor (hDC,BLACK); 
        OldBkMode = SetBkMode (hDC,TRANSPARENT); 
        DrawText (hDC,szBtnText,-1,&rText,DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        if (wState & BFFOCUS)
        {
            WORD StrWidth = (WORD)(GetTextExtent (hDC,szBtnText,
                                    lstrlen (szBtnText)));
            
//            rText.right = min (StrWidth+rText.left,rText.right);
//            InflateRect (&rText,1,1);
	    GetClientRect(hControl, &rClient);
	    rText.left -= 2;
	    rText.right = rClient.right - 1;
	    rText.top = rClient.top;
	    rText.bottom = rClient.bottom - 1;
            draw_bevel (hDC,hDarkPen,hLightPen,&rText,1,DB_RECT | DB_IN);
            //hFocusPen = CreatePen (PS_DOT,1,BLACK);
            //SelectObject (hDC,hFocusPen);
            //hOldBrush = SelectObject (hDC,GetStockObject (NULL_BRUSH));
            //Rectangle (hDC,rText.left,rText.top,rText.right,rText.bottom);
            //SelectObject (hDC,hOldPen);
            //SelectObject (hDC,hOldBrush);
            //DeleteObject (hFocusPen);
        }
        SetTextColor (hDC,rgbOldTextColor); 
        SetBkMode (hDC,OldBkMode);
    }
    SetMapMode (hDC,OldMapMode);
    SelectObject (hDC,hOldPen);
    DeleteObject (hDarkPen);
    DeleteObject (hLightPen);
    if ( hFont )
	SelectObject( hDC, hOldFont );
}


LOCAL void NEAR PASCAL get_avg_char_size (hControl,lpwCharW,lpwCharH)
HWND hControl;
WORD FAR *lpwCharW;
WORD FAR *lpwCharH;
{
    TEXTMETRIC tm;
    HDC hDC = GetDC (hControl);
    
    GetTextMetrics (hDC,&tm);
    ReleaseDC (hControl,hDC);
    *lpwCharW = tm.tmAveCharWidth;
    *lpwCharH = tm.tmHeight+tm.tmExternalLeading;
}


LOCAL void NEAR PASCAL get_btn_rect (hControl,wType,wState,lStyle,lpRect)
HWND hControl;
WORD wType;
WORD wState;
LONG lStyle;
LPRECT lpRect;
{
    WORD CharW, CharH;
    int  BoxSize;
    RECT rTemp;

    get_avg_char_size (hControl,&CharW,&CharH);
    GetClientRect (hControl,lpRect);
    if (lpRect->bottom < 14)
        lpRect->bottom = 14;
    if (lpRect->right < 14)
        lpRect->right = 14;
    BoxSize = max(14,(5*CharH/6));
    //BoxSize = max(14,(3*CharH/4));
    //BoxSize = max(14,CharH);
    switch (wType)
    {
        case GR_CHECKBOX_BOX:
            //BoxSize = max(14,CharH);
            BoxSize = max(14,(5*CharH/6));
        case GR_RADIOBTN_BOX:
            if (lpRect->bottom >= BoxSize)
            {
                lpRect->top = (lpRect->bottom - BoxSize)/2;
                lpRect->bottom = lpRect->top + BoxSize;
            }
            if (lpRect->right >= BoxSize)
            {
                if (lStyle & BS_LEFTTEXT)
                    lpRect->left = lpRect->right - BoxSize;
                else
                    lpRect->right = lpRect->left + BoxSize;
            }
            break;
        case GR_RADIOBTN_TXT:
        case GR_CHECKBOX_TXT:
            if (lpRect->bottom >= CharH + 2)
            {
                lpRect->top = (lpRect->bottom - CharH)/2;
                lpRect->bottom = lpRect->top + CharH;
            }
            else 
            {
                lpRect->top ++;
                lpRect->bottom --;
            }    
            if (lpRect->right >= CharH)
            {
                if (lStyle & BS_LEFTTEXT)
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
            break;
        case GR_GROUPBOX_LIN:
            InflateRect (lpRect,-3-CharH/4,0);
            lpRect->top += 3 + 3*CharH/2;
            lpRect->bottom += lpRect->top + 2;
            break;
        case GR_GROUPBOX_TXT:
            InflateRect (lpRect,-3-CharH/4,-3-CharH/4);
            lpRect->bottom = lpRect->top + CharH;
            break;
        case GR_PUSHBTN_BOX:
            InflateRect (lpRect,-2,-2);
            break;
        case GR_PUSHBTN_TXT:
            InflateRect (lpRect,-5,-5);
            OffsetRect (lpRect,-1,-1);
            break;
        default:
            break;
    }
}

LOCAL HBRUSH NEAR PASCAL get_control_brush (hControl,hDC,wType)
HWND hControl;
HDC  hDC;
WORD wType;
{
    HWND    hParent;
    HBRUSH  hRetBrush = NULL;

    if ((hParent = GetParent (hControl)) == NULL)
        hParent = hControl;

    hRetBrush = (HBRUSH)SendMessage (hParent,WM_CTLCOLOR,(WORD)hDC,
                                    MAKELONG (hControl,wType));
    if (!hRetBrush)
    {
        SetBkColor (hDC,GetSysColor (COLOR_BTNFACE));
        SetTextColor (hDC,GetSysColor (COLOR_WINDOWTEXT));
        hRetBrush = GetStockObject (WHITE_BRUSH);
    }
    return (hRetBrush);
}

LOCAL LONG NEAR PASCAL get_dlg_code (wStyle)
WORD wStyle;
{
    LONG lResult;

    switch (wStyle) 
    {
        case BS_DEFPUSHBUTTON:
            lResult = DLGC_DEFPUSHBUTTON | DLGC_BUTTON;
            break;
        case BS_PUSHBUTTON:
        case BS_PUSHBOX:
            lResult = DLGC_UNDEFPUSHBUTTON | DLGC_BUTTON;
            break;
        case BS_AUTORADIOBUTTON:
        case BS_RADIOBUTTON:
            lResult = DLGC_RADIOBUTTON | DLGC_BUTTON;
            break;
        case BS_GROUPBOX:
            lResult = DLGC_STATIC;
            break;
        default:
            lResult = 0;
            break;
    }
    return lResult;
}

LOCAL COLORREF NEAR PASCAL get_led_color (lStyle)
LONG lStyle;
{
    COLORREF rgbResult;

    if (lStyle & BS_GREEN)
        rgbResult = GREEN;
    else if (lStyle & BS_BLUE)
        rgbResult = BLUE;
    else if (lStyle & BS_YELLOW)
        rgbResult = YELLOW;
    else if (lStyle & BS_CYAN)
        rgbResult = CYAN;
    else if (lStyle & BS_MAGENTA)
        rgbResult = MAGENTA;
    else
        rgbResult = RED;

    return (rgbResult);
}

LOCAL BOOL NEAR PASCAL get_parentdc_clipbox (hControl,hDC,lpClipRect)
HWND   hControl;
HDC    hDC;
LPRECT lpClipRect;
{
    RECT rClient;
    BOOL bResult = FALSE;

    if (GetClipBox (hDC,lpClipRect) != NULLREGION)
    {
        if (GetClassWord (hControl,GCW_STYLE) & CS_PARENTDC)
        {
            GetClientRect (hControl,&rClient);
            bResult = IntersectRect (lpClipRect,lpClipRect,&rClient);
        }
        else
            bResult = TRUE;
    }
    return (bResult);
}

LOCAL void NEAR PASCAL kill_mgxbtn_focus (hControl,wState)
HWND hControl;
WORD wState;
{
    if (wState & BFCAPTURED)
        release_mgxbtn_capture (hControl,TRUE);
    wState = GetWindowWord (hControl,GWW_STATE);
    wState &= ~BFFOCUS;
    SetWindowWord (hControl,GWW_STATE,wState);
    InvalidateRect (hControl,NULL,FALSE);
    UpdateWindow (hControl); 
}

LOCAL void NEAR PASCAL notify_parent (hControl,wCode)
HWND hControl;
WORD wCode;
{
    SendMessage (GetParent (hControl),WM_COMMAND,
                 GetWindowWord (hControl,GWW_ID), 
                 MAKELONG (hControl,wCode));
}

LOCAL void NEAR PASCAL release_mgxbtn_capture (hControl,bCheck)
HWND hControl;
BOOL bCheck;
{
    HWND hNextRadioBtn;
    WORD check;
    WORD wState = GetWindowWord (hControl,GWW_STATE);
    WORD wStyle = (WORD)(GetWindowLong (hControl,GWL_STYLE)) & 0xF;

    wState &= ~BFINCLICK;
    SetWindowWord (hControl,GWW_STATE,wState);

    if (wState & BFHILIGHT) 
    {
        if ((wStyle == BS_PUSHBOX) || (wStyle == BS_PUSHBUTTON) || 
            (wStyle == BS_DEFPUSHBUTTON) || (wStyle == BS_RADIOBUTTON) ||
            (wStyle == BS_CHECKBOX)) 
        {
            SendMessage (hControl,BM_SETSTATE,FALSE,0L);
            wState = GetWindowWord (hControl,GWW_STATE);
        }
        else
        {
            wState &= ~BFHILIGHT;
            SetWindowWord (hControl,GWW_STATE,wState);
        }

        if (bCheck) 
        {
            switch (wStyle) 
            {
                case BS_AUTORADIOBUTTON:
                    if (wState & BFCHECK)
                    {
                        InvalidateRect (hControl,NULL,FALSE);
                        UpdateWindow (hControl); 
                    }    
                    else
                    {
                        hNextRadioBtn = hControl;
                        do
                        {
                            if ((WORD)SendMessage (hNextRadioBtn,
                                WM_GETDLGCODE,0,0L) & DLGC_RADIOBUTTON)
                                SendMessage (hNextRadioBtn,BM_SETCHECK,
                                    (hControl == hNextRadioBtn),0L);
                            hNextRadioBtn = GetNextDlgGroupItem (
                                GetParent (hNextRadioBtn),hNextRadioBtn,FALSE);
                        }   while (hNextRadioBtn != hControl);
                    }
                    notify_parent (hControl,BN_CLICKED);
                    wState = GetWindowWord (hControl,GWW_STATE);
                    break;
                case BS_GROUPBOX:
                    break;
                case BS_AUTOCHECKBOX:
                case BS_AUTO3STATE:
                    check = (wState & BFCHECK) + 1;
                    if (check > (wStyle == BS_AUTO3STATE ? 2 : 1))
                        check = 0;
                    SendMessage (hControl,BM_SETCHECK,check,0L);
                    notify_parent (hControl,BN_CLICKED);
                    wState = GetWindowWord (hControl,GWW_STATE);
                    break;
                default:
                    notify_parent (hControl,BN_CLICKED);
                    wState = GetWindowWord (hControl,GWW_STATE);
                    break;
            }
        }
    }
    if (wState & BFCAPTURED) 
    {
        wState &= ~(BFCAPTURED | BFMOUSE);
        SetWindowWord (hControl,GWW_STATE,wState);
        ReleaseCapture ();
    }
}

LOCAL void NEAR PASCAL set_mgxbtn_capture (hControl,bMouseInitiated)
HWND hControl;
BOOL bMouseInitiated;
{
    WORD wState = GetWindowWord (hControl,GWW_STATE);

    if (bMouseInitiated)
        wState |= BFMOUSE;
    if (!(wState & BFCAPTURED)) 
    {
        wState = (wState | BFCAPTURED | BFINCLICK);
        SetWindowWord (hControl,GWW_STATE,wState);
        SetCapture (hControl);
        SetFocus (hControl);
        wState = GetWindowWord (hControl,GWW_STATE);
        //wState &= ~BFINCLICK;
    }
    SetWindowWord (hControl,GWW_STATE,wState);
}

LOCAL void NEAR PASCAL set_mgxbtn_check (hControl,lStyle,wState,wParam)
HWND hControl;
LONG lStyle;
WORD wState;
WORD wParam;
{
    WORD wStyle = (WORD)lStyle & 0xF;

    switch (wStyle) 
    {
        case BS_RADIOBUTTON:
        case BS_AUTORADIOBUTTON:
            if (wParam)
                SetWindowLong (hControl,GWL_STYLE,lStyle |= WS_TABSTOP);
            else
                SetWindowLong (hControl,GWL_STYLE,lStyle &= ~WS_TABSTOP);
            wParam = (wParam > 0 ? 1 : 0);
            break;
        case BS_CHECKBOX:
        case BS_AUTOCHECKBOX:
            wParam = (wParam > 0 ? 1 : 0);
            break;
        case BS_3STATE:
        case BS_AUTO3STATE:
            if (wParam > 2) wParam = 2;
        default:
            break;
    }
    if ((wState & BFCHECK) != wParam) 
    {
        wState = ((wState & ~BFCHECK) | wParam);
        SetWindowWord (hControl,GWW_STATE,wState);
        InvalidateRect (hControl,NULL,FALSE);
        UpdateWindow (hControl); 
    }
}

LOCAL void NEAR PASCAL set_mgxbtn_focus (hControl,wState)
HWND hControl;
WORD wState;
{
    WORD wStyle = (WORD)(GetWindowLong (hControl,GWL_STYLE)) & 0xF;

    wState |= BFFOCUS;
    SetWindowWord (hControl,GWW_STATE,wState);
    InvalidateRect (hControl,NULL,FALSE);
    UpdateWindow (hControl); 
    if (!(wState & BFINCLICK) && 
        (wStyle == BS_RADIOBUTTON || wStyle == BS_AUTORADIOBUTTON) &&
        !(wState & BFDONTCLICK) && !(wState & BFCHECK))
            notify_parent (hControl,BN_CLICKED);
}

LOCAL void NEAR PASCAL set_mgxbtn_hilight (hControl,wState,bHilight)
HWND hControl;
WORD wState;
BOOL bHilight;
{
    WORD wStyle = (WORD)(GetWindowLong (hControl,GWL_STYLE)) & 0xF;

    if (wStyle == BS_USERBUTTON)
    {
        notify_parent (hControl,(bHilight ? BN_HILITE : BN_UNHILITE));
        wState = GetWindowWord (hControl,GWW_STATE);
    }
    if (bHilight && (!(wState & BFHILIGHT)))
    {
        wState |= BFHILIGHT;
        SetWindowWord (hControl,GWW_STATE,wState);
        InvalidateRect (hControl,NULL,FALSE);
        UpdateWindow (hControl); 
    }
    else if (!bHilight && (wState & BFHILIGHT))
    {
        wState &= ~BFHILIGHT;
        SetWindowWord (hControl,GWW_STATE,wState);
        InvalidateRect (hControl,NULL,FALSE);
        UpdateWindow (hControl); 
    }
}

LOCAL void NEAR PASCAL paint_mgxbtn (hControl,wState,wStyle)
HWND hControl;
WORD wState;
WORD wStyle;
{
    PAINTSTRUCT ps;
    RECT rc;
    HBITMAP hBitmap, hOldBitmap;
    HBRUSH hOldBrush;
    HDC hMemDC;

    BeginPaint(hControl,&ps);

    if (ps.hdc)
    {
        GetClientRect (hControl,&rc);
        switch (wStyle) 
        {
            case BS_PUSHBUTTON:
            case BS_DEFPUSHBUTTON:
            case BS_PUSHBOX:
                hBtnFillBrush = get_control_brush (hControl,ps.hdc,CTLCOLOR_BTN);
                hMemDC = CreateCompatibleDC (ps.hdc);
                hBitmap = CreateCompatibleBitmap (ps.hdc,rc.right,rc.bottom);
                if (hBitmap && hMemDC)
                {
                    hOldBitmap = SelectObject (hMemDC,hBitmap);
                    hOldBrush = SelectObject (hMemDC,hBtnFillBrush);
                    PatBlt (hMemDC,0,0,rc.right,rc.bottom,PATCOPY);
                    draw_pushbtn (hControl,hMemDC,wState);
                    BitBlt (ps.hdc,0,0,rc.right,rc.bottom,hMemDC,0,0,SRCCOPY);
                    SelectObject (hMemDC,hOldBrush);
                    SelectObject (hMemDC,hOldBitmap);
                    DeleteObject (hBitmap);
                    DeleteDC (hMemDC);
                }
                break;
            case BS_RADIOBUTTON:
            case BS_AUTORADIOBUTTON:
                hBtnFillBrush = get_control_brush (hControl,ps.hdc,CTLCOLOR_BTN);
                hMemDC = CreateCompatibleDC (ps.hdc);
                hBitmap = CreateCompatibleBitmap (ps.hdc,rc.right,rc.bottom);
                if (hBitmap && hMemDC)
                {
                    hOldBitmap = SelectObject (hMemDC,hBitmap);
                    hOldBrush = SelectObject (hMemDC,hBtnFillBrush);
                    PatBlt (hMemDC,0,0,rc.right,rc.bottom,PATCOPY);
                    draw_radiobtn (hControl,hMemDC,wState);
                    BitBlt (ps.hdc,0,0,rc.right,rc.bottom,hMemDC,0,0,SRCCOPY);
                    SelectObject (hMemDC,hOldBrush);
                    SelectObject (hMemDC,hOldBitmap);
                    DeleteObject (hBitmap);
                    DeleteDC (hMemDC);
                }
                break;
            case BS_CHECKBOX:
            case BS_3STATE:
            case BS_AUTOCHECKBOX:
            case BS_AUTO3STATE:
                hBtnFillBrush = get_control_brush (hControl,ps.hdc,CTLCOLOR_BTN);
                hMemDC = CreateCompatibleDC (ps.hdc);
                hBitmap = CreateCompatibleBitmap (ps.hdc,rc.right,rc.bottom);
                if (hBitmap && hMemDC)
                {
                    hOldBitmap = SelectObject (hMemDC,hBitmap);
                    hOldBrush = SelectObject (hMemDC,hBtnFillBrush);
                    PatBlt (hMemDC,0,0,rc.right,rc.bottom,PATCOPY);
                    draw_checkbox (hControl,hMemDC,wState);
                    BitBlt (ps.hdc,0,0,rc.right,rc.bottom,hMemDC,0,0,SRCCOPY);
                    SelectObject (hMemDC,hOldBrush);
                    SelectObject (hMemDC,hOldBitmap);
                    DeleteObject (hBitmap);
                    DeleteDC (hMemDC);
                }
                break;
            case BS_USERBUTTON:
                notify_parent (hControl,BN_PAINT);
                wState = GetWindowWord (hControl,GWW_STATE);
                if (wState & BFHILIGHT)
                    notify_parent (hControl,BN_HILITE);
                if (wState & WS_DISABLED)
                    notify_parent (hControl,BN_DISABLE);
                break;
            case BS_GROUPBOX:
                draw_groupbox (hControl,ps.hdc,wState);
                break;
        }
    }
    EndPaint (hControl,&ps);
}

/****************************** Public Functions ****************************/

BOOL FAR PASCAL register_mgxbtn_class (hInst,bGlobal)
HANDLE hInst;
BOOL bGlobal;
{
    WNDCLASS   wcControl;
    BOOL       bResult;
    COLORREF   rgbMedium;
    HDC        hDC;

    LockData (NULL);
    hDC = GetDC (NULL);
    rgbDark = GetNearestColor (hDC,0x00555555);
    rgbMedium = GetNearestColor (hDC,0x00C0C0C0);
    rgbLight = WHITE;
    ReleaseDC (NULL,hDC);

    if (rgbMedium == rgbLight || 
        rgbLight  == rgbDark  ||
        rgbDark   == rgbMedium) 
    {
        GetClassInfo (NULL,"BUTTON",&wcControl);
        wcControl.lpszClassName = (LPSTR)"MGXBTN";
        wcControl.hInstance = hInst;
    }
    else
    {
        wcControl.lpszClassName = (LPSTR)"MGXBTN";
        wcControl.hInstance     = hInst;
        wcControl.lpfnWndProc   = MgxBtnWndFn;
        wcControl.hCursor       = LoadCursor (NULL,IDC_ARROW);
        wcControl.hIcon         = NULL;
        wcControl.lpszMenuName  = NULL;
        wcControl.cbClsExtra    = 0;
        wcControl.cbWndExtra    = BC_EXTRA;
        wcControl.style         = CS_DBLCLKS | CS_PARENTDC;
        if (bGlobal)
            wcControl.style |= CS_GLOBALCLASS;
    }
    wcControl.hbrBackground = NULL;
    bResult = RegisterClass (&wcControl);
    UnlockData (NULL);
    return bResult;
}

/***************************** Exported Functions ***************************/

LONG FAR PASCAL MgxBtnWndFn (hControl,wMsg,wParam,lParam)
/*  This function is responsible for processing all the messages
    which relate to the MGXBTN control window. The code is written 
    to avoid potential problems when re-entrancy by the use of extra 
    bytes associated with the window data structure. The LONG value 
    returned by this function is either a value  returned by the 
    internal handling of the message or by the default window procedure. */
HWND  hControl;
WORD  wMsg;
WORD  wParam;
LONG  lParam;
{
    LONG lResult = 0L;
    BOOL bHandled = TRUE;
    LONG lStyle = GetWindowLong (hControl,GWL_STYLE);
    WORD wStyle = (WORD)lStyle & 0xF;
    WORD wState = GetWindowWord (hControl,GWW_STATE);
    RECT rControl;

    LockData (NULL);
    switch (wMsg)
    {
    case WM_CREATE:
        SetWindowWord(hControl,GWW_STATE,0);
        break;
    case WM_SETFONT:
	SetWindowWord(hControl,GWW_FONT,wParam);
	if (lParam)
		{
		InvalidateRect(hControl, NULL, TRUE);
		UpdateWindow(hControl);
		}
	break;
    case WM_ERASEBKGND:
        lResult = 1L;
        break;
    case WM_NCHITTEST:
        if (wStyle == BS_GROUPBOX) 
            lResult = (LONG)HTTRANSPARENT;
        else
            bHandled = FALSE;
        break;
    case WM_SETFOCUS:
        set_mgxbtn_focus (hControl,wState);
        break;
    case WM_KILLFOCUS:
        kill_mgxbtn_focus (hControl,wState);
        break;
    case WM_LBUTTONDBLCLK:
        if (wStyle == BS_USERBUTTON || wStyle == BS_RADIOBUTTON)
            notify_parent (hControl,BN_DOUBLECLICKED);
        else
            SendMessage (hControl,WM_LBUTTONDOWN,wParam,lParam);
        break;
    case WM_LBUTTONUP:
        if (!(wState & BFMOUSE))
            break;
        release_mgxbtn_capture (hControl,TRUE);
        break;
    case WM_MOUSEMOVE:
        if (wState & BFCAPTURED)
        {
            GetClientRect (hControl,(LPRECT)&rControl);
            if (PtInRect ((LPRECT)&rControl,MAKEPOINT(lParam)))  
            {
                if (!(wState & BFHILIGHT))
                    SendMessage (hControl,BM_SETSTATE,TRUE,0L);
            }
            else if (wState & BFHILIGHT)
                SendMessage (hControl,BM_SETSTATE,FALSE,0L);
        }
        break;
    case WM_LBUTTONDOWN:
        set_mgxbtn_capture (hControl,TRUE);
        GetClientRect (hControl,(LPRECT)&rControl);
        SendMessage (hControl,BM_SETSTATE,
            PtInRect ((LPRECT)&rControl,MAKEPOINT (lParam)),0L);
        break;
    case WM_KEYDOWN:
	if (wParam == VK_DOWN || wParam == VK_UP)
		dbg("KEYDOWN - got UP or DOWN = %x", wParam);
        if (wState & BFMOUSE)
            break;
        if (wParam == VK_SPACE) 
        {
            if (wState & BFINCLICK)
                break;
            set_mgxbtn_capture (hControl,FALSE);
            SendMessage (hControl,BM_SETSTATE,TRUE,0L);
        } 
        else     
            release_mgxbtn_capture (hControl,FALSE);
        break;
    case WM_KEYUP:  // FALL THRU
	if (wParam == VK_DOWN || wParam == VK_UP)
		dbg("KEYDOWN - got UP or DOWN = %x", wParam);
    case WM_SYSKEYUP:
        if ((wState & BFMOUSE) || (wParam == VK_TAB))
        {   
            bHandled = FALSE;
            break;
        }    
        release_mgxbtn_capture (hControl,(wParam == VK_SPACE));
        if (wMsg == WM_SYSKEYUP)
            bHandled = FALSE;
        break;
    case WM_GETDLGCODE:
        lResult = get_dlg_code (wStyle);
        break;
    case WM_ENABLE:
        InvalidateRect (hControl,(LPRECT)NULL,TRUE);
        UpdateWindow (hControl);
        break;
    case WM_PAINT:
        paint_mgxbtn (hControl,wState,wStyle);
        break;
    case BM_CLICK:
        SendMessage (hControl,WM_LBUTTONDOWN,0,0L);
        SendMessage (hControl,WM_LBUTTONUP,0,0L);
        break;
    case BM_GETCHECK:       
        lResult = (LONG)(wState & BFCHECK);
        break;       
    case BM_GETSTATE:     
        lResult = (LONG)wState;
        break;
    case BM_SETCHECK:    
        set_mgxbtn_check (hControl,lStyle,wState,wParam);
        break;
    case BM_SETSTATE:
        set_mgxbtn_hilight (hControl,wState,(BOOL)wParam);
        break;
    case BM_SETSTYLE:
        {
            LONG lNewStyle = MAKELONG (wParam,HIWORD (lStyle));

            SetWindowLong (hControl,GWL_STYLE,lNewStyle);
            if (lParam || (wStyle != wParam))
            {
                InvalidateRect (hControl,(LPRECT)NULL,TRUE);
                UpdateWindow (hControl);
            }
        }
        break;
    default:
        bHandled = FALSE;
        break;
    }
    if (!bHandled)
        lResult = DefWindowProc (hControl,wMsg,wParam,lParam);
    UnlockData (NULL);
    return (lResult);
}

