//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// CTLUTIL.H - Private utility functions for controls library
#ifndef _CTLUTIL_H_
#define _CTLUTIL_H_

#include <windows.h>
#include <windowsx.h>
#include "types.h"
#include "common.h"
#include "superlib.h"

/******************************
 Predefined colors
 ******************************/
#define RED         0x000000FFL
#define GREEN       0x0000FF00L
#define BLUE        0x00FF0000L
#define YELLOW      0x0000FFFFL
#define CYAN        0x00FFFF00L
#define MAGENTA     0x00FF00FFL
#define WHITE       0x00FFFFFFL
#define LIGHT_GRAY  0x00C0C0C0L
#define MEDIUM_GRAY 0x00808080L
#define DARK_GRAY   0x00555555L
#define BLACK       0x00000000L

#define CTLCOLOR_DEFAULT   -1L  // Pass into SETCOLOR msg as color. (use default color)
#define CTLCOLOR_ERROR     -2L  // Returned from color msgs if color not supportered

/* CButtonMaker states */
#define BMS_CURRENT         0   //  Push button states
#define BMS_UP              1   //  unslected
#define BMS_DOWN            2   //  pressed
#define BMS_SELECTED        3   //  selected

/******************************
 Draw Bevel Types
 ******************************/
#define DB_IN             0
#define DB_OUT            1
#define DB_RECT           2
#define DB_CIRCLE         4
#define DB_DIAMOND        8

/******************************
 Draw Push Button styles
 ******************************/
#define DP_DBLBORDER       1 
#define DP_CHAMFERFRAME    2
#define DP_THINBEVEL       4
#define DP_THICKBEVEL      8
#define DP_CALCRECT       16

/******************************
 Draw Push Button states
 ******************************/
#define PBSTATE_UP         0
#define PBSTATE_DOWN       1
#define PBSTATE_SELECTED   2

#define BTNM_CLICK          (WM_USER + 99)   // Forces a click on the btn ctl
#define BTNM_GETCHECK       BM_GETCHECK      // determines if btn is checked
#define BTNM_SETCHECK       BM_SETCHECK      // Sets or removes check from btn
#define BTNM_GETSTATE       BM_GETSTATE      // Determines if btn is pressed
#define BTNM_SETSTATE       BM_SETSTATE      // Sets pressed state of btn
#define BTNM_SETSTYLE       BM_SETSTYLE      // Sets control style of btn

#define WRECT(rect) ((rect).right - (rect).left)
#define HRECT(rect) ((rect).bottom - (rect).top)
#define NOTIFY_PARENT(hControl,wCode) \
    FORWARD_WM_COMMAND (GetParent (hControl), \
        GetDlgCtrlID (hControl), hControl, wCode, SendMessage)
#define HANDLE_BTNM_CLICK(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)BTNM_CLICK, wParam, lParam))
#define HANDLE_BTNM_GETCHECK(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)BTNM_GETCHECK, wParam, lParam))
#define HANDLE_BTNM_GETSTATE(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)BTNM_GETSTATE, wParam, lParam))
#define HANDLE_BTNM_SETSTATE(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)BTNM_SETSTATE, wParam, lParam))
#define HANDLE_BTNM_SETCHECK(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)BTNM_SETCHECK, wParam, lParam))

LPBLT Ctl_GetBltScreen();
void Ctl_SetBltScreen(LPBLT lpBltScreen);
BOOL Ctl_CreateBrushes( );
void Ctl_DestroyBrushes( );
HBRUSH Ctl_GetBtnShadowBrush();
HBRUSH Ctl_GetBtnFaceBrush();
HBRUSH Ctl_GetBtnHilightBrush();
HPEN Ctl_GetBtnShadowPen();
HPEN Ctl_GetBtnFacePen();
LPTR Ctl_Alloc( long lCount );
LPTR Ctl_AllocX( long lCount, WORD wFlags );
void Ctl_FreeUp( LPVOID lpMemory );
BOOL Ctl_StyleOn( HWND hWnd, DWORD dwFlag );
void Ctl_DrawButton (HWND hWindow,HDC hDC,LPRECT lpRect,WORD wStyle,WORD wState);
void  Ctl_DrawPushButton (HDC hDC, LPRECT lpRect, WORD wStyle, WORD wState, HBRUSH hBkground);
void Ctl_DrawBtnBevel (HDC hDC, RECT rRect, WORD wThickness);
void Ctl_DrawBevel (HDC hDC,COLORREF rgbDark,COLORREF rgbLight, RECT rRect, UINT wThickness, UINT wType);
void Ctl_DrawLed (HDC hDC, COLOR rgbColor, RECT rRect, BOOL bHilight);
void Ctl_DrawDiamond (HDC hDC, COLOR rgbColor, RECT rRect);
void Ctl_GetAvgCharSize (HFONT hFont, LPWORD lpwCharW, LPWORD lpwCharH);
void Ctl_DrawCheck (HDC hDC, COLOR rgbColor, RECT rRect);
void Ctl_EraseBackground( HWND hWnd, HDC hDC, LPRECT lpRect, WORD wType );
HBRUSH Ctl_GetBackgroundBrush( HWND hWindow, HDC hDC, WORD wType );
COLOR Ctl_GetDisabledBtnTextColor();
COLOR Ctl_GetBtnTextColor();
COLOR Ctl_GetBtnFaceColor();
COLOR Ctl_GetTextColor();
COLOR Ctl_GetBtnShadowColor();
COLOR Ctl_GetBtnHilightColor();
COLOR Ctl_GetBtnBorderColor();
void Ctl_SetLabelFont(int iLabelFont);
int Ctl_GetLabelFont();

#endif // _CTLUTIL_H_

