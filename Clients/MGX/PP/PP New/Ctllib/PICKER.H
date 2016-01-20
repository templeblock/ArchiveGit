//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _PICKER_H_
#define _PICKER_H_
#include "superblt.h"
#include "picker.rh"
#ifdef CTLLIB
BOOL PalPicker_RegisterClass (HINSTANCE hInstance, UINT ClassScope);
BOOL PalPicker_UnRegisterClass (HINSTANCE hInstance);
#endif

void PalPicker_Init(HWND hWindow, LPCOLORMAP lpColorMap, int iSelect);
LPCOLORMAP PalPicker_GetColorMap(HWND hWindow);
int PalPicker_GetIndex(HWND hWindow);

#endif // _PICKER_H_

