//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _SLIDER_H_
#define _SLIDER_H_
#include "ctlutil.h"

#define  SS_VERTSLIDER      0L          // simple slider styles
#define  SS_HORZSLIDER      0x0001L
#define  SS_TICKSLEFT       0L
#define  SS_TICKSRIGHT      0x0020L
#define  SS_TICKSTOP        0L
#define  SS_TICKSBOTTOM     0x0020L

#define  SS_NOSNAP          0L          // commen slider styles
#define  SS_SNAP            0x0002L
#define  SS_SNAPDRAG        0x0004L
#define  SS_CLICKSTEP       0x0008L
#define  SS_LEFTTORIGHT     0L
#define  SS_RIGHTTOLEFT     0x0010L
#define  SS_TOPTOBOTTOM     0L
#define  SS_BOTTOMTOTOP     0x0010L
#define  SS_OWNERDRAW       0x4000L
#define  SS_NOMOUSEFOCUS    0x8000L

typedef struct SSliderInfo
{
    POINT ptPosition;
    RECT rPosRange;
    int Value;
    int Min;    
    int Max;
    int Step;
    int BigTickInc;
    int SmallTickInc;
    int ThumbWidth;
    int ThumbHeight;
    BOOL bStdThumb;
    int GrooveThickness;
    long lStyle;
    BOOL bCaptured;
    BOOL bFocus;
    BOOL bStepping;
} SLIDERINFO, FAR *LPSLIDERINFO;

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

LONG WINPROC Slider_Control(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );

#endif // _SLIDER_H_
