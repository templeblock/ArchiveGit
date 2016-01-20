//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef NO_MFC
#ifndef _SLIDER_H_
#define _SLIDER_H_

#include "afxcmn.h"

#define SN_VALUECHANGING    1          // dragging of slider thumb.
#define SN_VALUESELECTED    2          // clicked slider.

BOOL InitSlide( HWND hDlg, ITEMID idItem, int iValue, int iMin, int iMax );
BOOL SetSlide( HWND hDlg, ITEMID idItem, int iValue );
int HandleSlide( HWND hDlg, ITEMID id, UINT codeNotify, LPINT lpBoolCompleted );
void SliderOnScroll( HWND hWindow, UINT uCode, int iPos );

#endif // _SLIDER_H_
#endif // NO_MFC
