//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _SLIDER_H_
#define _SLIDER_H_

#include "slider.rh"

#define SM_GETRANGE         (WM_USER + 650)
#define SM_GETVALUE         (WM_USER + 651)
#define SM_GETSTEP          (WM_USER + 652)
#define SM_GETTHUMBSIZE     (WM_USER + 653)
#define SM_GETTICKS         (WM_USER + 654)
#define SM_SETDATA          (WM_USER + 655)
#define SM_SETDRAWPROC      (WM_USER + 656)    
#define SM_SETRANGE         (WM_USER + 657)
#define SM_SETVALUE         (WM_USER + 658)
#define SM_SETSTEP          (WM_USER + 659)
#define SM_SETTHUMBSIZE     (WM_USER + 660)     
#define SM_SETTICKS         (WM_USER + 661)

#define SM_GETXRANGE        (WM_USER + 671)
#define SM_GETYRANGE        (WM_USER + 672)
#define SM_SETXRANGE        (WM_USER + 673)
#define SM_SETYRANGE        (WM_USER + 674)

#define SN_VALUECHANGING    1          // dragging of slider thumb.
#define SN_VALUESELECTED    2          // clicked slider.

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

#ifdef CTLLIB
BOOL Slider_RegisterClass (HINSTANCE hInstance, UINT ClassScope);
#endif
BOOL InitSlide( HWND hDlg, ITEMID idItem, int iValue, int iMin, int iMax );
BOOL SetSlide( HWND hDlg, ITEMID idItem, int iValue );
int HandleSlide( HWND hDlg, ITEMID id, UINT codeNotify, LPINT lpBoolCompleted );

#endif // _SLIDER_H_
