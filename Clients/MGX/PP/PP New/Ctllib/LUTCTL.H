// (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// lutctl.h for look-up-table control
#ifndef _LUTCTL_H_
#define _LUTCTL_H_

#include "lutctl.rh"

#ifdef CTLLIB
BOOL Lut_RegisterClass (HINSTANCE hInstance, UINT ClassScope);
BOOL Lut_UnRegisterClass (HINSTANCE hInstance);
#endif
long WINPROC Lut_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
BOOL Lut_Init(HWND hWindow, int gridX, int gridY);
BOOL Lut_SetLut(HWND hWindow, int nLuts, LPTR lpLut);
BOOL Lut_SetColor(HWND hWindow, int iLutIndex, LPRGB lpRGB);

#endif // _LUTCTL_H_


