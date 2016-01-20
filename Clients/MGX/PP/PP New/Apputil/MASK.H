//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _MASK_H_
#define _MASK_H_

#include "imgutil.h"
#include "appdata.h"

// Shape Mode Constants
#define SHAPE_REVERSE	1
#define SHAPE_ADD		2
#define SHAPE_SUBTRACT	3
#define SHAPE_DEFAULT SHAPE_ADD

LPMASK MaskCreate( LPFRAME lpAlphaIn, int iMaxWidth, int iMaxHeight, BOOL fOn, BOOL fNoUndo, FRMDATATYPE type = FDT_GRAYSCALE);
BOOL MaskClose( LPMASK lpMask );
BOOL MaskRect( LPMASK lpMask, LPRECT lpRect );
BOOL MaskRectUpdate( LPMASK lpMask, LPRECT lpRect );
int IsMasked( LPMASK lpMask, int x, int y );
BOOL MaskLoad( LPMASK lpMask, int x, int y, int dx, LPTR lpM );
BOOL MaskLoaderHuge( LPMASK lpMask, int x, int y, int dx, HPTR lpM, BOOL bInvert, COMBINE_ROP ROP  );
BOOL MaskLoader( LPMASK lpMask, int x, int y, int iMaxWidth, LPTR lpM, BOOL bInvert, COMBINE_ROP ROP );
BOOL MaskRectUpdate( LPMASK lpMask, LPRECT lpRect );
BOOL MaskRectDwindle( LPMASK lpMask, LPRECT lpRect, LPFRAME lpAlphaFrame );
LPFRAME AlphaGetEditFrame(LPALPHA lpAlpha);
BOOL MaskSizeChange(LPMASK lpMask, int dx, int dy);
#endif // _MASK_H_


