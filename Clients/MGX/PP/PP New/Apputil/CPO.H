//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _CPO_H_
#define _CPO_H_

#include "object.h"

BOOL CreateOptimizedPalette(LPOBJECT lpObject, LPFRAME lpFrame,
							LPRGB lpRGBmap, LPINT lpMaxColors,
							BOOL bOptimize,
							STATUSCALLBACKPROC lpStatusCallback);
BOOL ReducePalette(LPRGB lpRGBmap, int nColors, LPRGB lpNewRGBmap, int nNewColors);
LPVOID OptimizeBegin( LPOBJECT lpObject, LPFRAME lpFrame, LPRGB RGBmap, int nColors, STATUSCALLBACKPROC lpStatusCallback, BOOL bReduction, BOOL bOptimize, BOOL bScatter, BOOL bDither, int dx);
LPVOID OptimizeBegin( LPOBJECT lpObject, LPFRAME lpFrame, LPRGB RGBmap, int nColors, STATUSCALLBACKPROC lpStatusCallback, BOOL bScatter, BOOL bDither, int dx);
void OptimizeData(LPVOID lpCpo, int x, int y, int iCount, LPTR lpIn, LPTR lpOut, int depth);
int OptimizeGetNumColor(LPVOID lpCpo);
void OptimizeEnd(LPVOID lpCpo);
int  CountColors( LPOBJECT lpObject, LPFRAME lpFrame, LPRGB lpRGBmap, LPRGB lpRGBData, int iLength, LPROC lpStatusCallback);

#endif // _CPO_H_

