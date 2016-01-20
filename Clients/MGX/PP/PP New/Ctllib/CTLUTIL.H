//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// CTLUTIL.H - Private utility functions for controls library
#ifndef _CTLUTIL_H_
#define _CTLUTIL_H_

#ifndef WIN32
#include <windows.h>
#endif

#include <windowsx.h>
#include "types.h"
#include "common.h"
#include "superblt.h"
#include "imgutil.h"

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

#ifndef WINDOWS95_HACK
#define BTNM_GETCHECK16	    (WM_USER+0)
#define BTNM_SETCHECK16	    (WM_USER+1)
#define BTNM_GETSTATE16	    (WM_USER+2)
#define BTNM_SETSTATE16	    (WM_USER+3)

#define HANDLE_BTNM_GETCHECK16(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)BTNM_GETCHECK, wParam, lParam))
#define HANDLE_BTNM_GETSTATE16(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)BTNM_GETSTATE, wParam, lParam))
#define HANDLE_BTNM_SETSTATE16(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)BTNM_SETSTATE, wParam, lParam))
#define HANDLE_BTNM_SETCHECK16(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (UINT)BTNM_SETCHECK, wParam, lParam))
#endif

void Ctl_SetNewLook(BOOL fNewLook);
BOOL Ctl_IsNewLook();
LPBLT Ctl_GetBltScreen();
void Ctl_SetBltScreen(LPBLT lpBltScreen);
LPTR Ctl_Alloc( long lCount );
LPTR Ctl_AllocX( long lCount, WORD wFlags );
void Ctl_FreeUp( LPVOID lpMemory );
BOOL Ctl_StyleOn( HWND hWnd, DWORD dwFlag );
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
void Ctl_SetIconInstance(HINSTANCE hInstIcon);
HINSTANCE Ctl_GetIconInstance();

#endif // _CTLUTIL_H_


