//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _ICON_H_
#define _ICON_H_

#include "icon.rh"

#define GWW_ICONID	GWW_START

#ifdef CTLLIB
BOOL Icon_RegisterClass (HINSTANCE hInstance, UINT ClassScope);
BOOL Icon_UnRegisterClass (HINSTANCE hInstance);
#endif

#endif // _ICON_H_


