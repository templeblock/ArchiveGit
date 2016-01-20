// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _CTLLIB_H_
#define _CTLLIB_H_

#include "types.h"
#include "common.h"
#include "radio.h"
#include "check.h"
#include "slider.h"
#include "spin.h"
#include "icon.h"
#include "picker.h"
#include "grid.h"
#include "listbox.h"
#include "dlgitem.h"
#include "ctltools.h"
#include "stick.h"
#include "lutctl.h"
#include "mapctl.h"
#include "histoctl.h"
#include "gradctl.h"
#include "patch.h"
#include "tagctl.h"
#include "stext.h"

BOOL Ctl_Init( HINSTANCE hInstance, UINT ClassScope, int iLabelFont, LPBLT lpBltScreen, HINSTANCE hInstIcon = NULL, BOOL fNewLook = FALSE);
void Ctl_Cleanup( HINSTANCE hInstance );
BOOL Ctl_NoReg_Init( HINSTANCE hInstance, UINT ClassScope, int iLabelFont, LPBLT lpBltScreen, HINSTANCE hInstIcon = NULL, BOOL fNewLook = FALSE);
void Ctl_NoReg_Cleanup( HINSTANCE hInstance );

#endif // _CTLLIB_H_


































                                                                         
