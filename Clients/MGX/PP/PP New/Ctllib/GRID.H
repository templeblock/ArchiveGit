//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _GRID_H_
#define _GRID_H_
#include "framelib.h"
#include "grid.rh"
#ifdef CTLLIB
BOOL Grid_RegisterClass (HINSTANCE hInstance, UINT ClassScope);
BOOL Grid_UnRegisterClass (HINSTANCE hInstance);
#endif

void Grid_Init(HWND hWindow, LPCOLORMAP lpColorMap, int iSelect);
LPCOLORMAP Grid_GetColorMap(HWND hWindow);
int Grid_GetIndex(HWND hWindow);

#endif // _GRID_H_

