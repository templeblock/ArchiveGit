/*
// $Workfile: PALETTE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1997 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/PALETTE.H $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:56   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:26   Fred
// Initial revision.
*/

#ifndef __PALETTE_H__
#define __PALETTE_H__

#include "stdafx.h"
#include "palette.h"

extern CPalette *pOurPal;
extern const RGBQUAD* GetIdentityPalette(void);
extern CPalette* CreateIdentityPalette(void);
extern void initialize_palette(void);
extern void free_palette(void);

#endif
