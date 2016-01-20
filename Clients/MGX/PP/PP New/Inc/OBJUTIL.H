//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _OBJUTIL_H_
#define _OBJUTIL_H_

#include "object.h"

LPOBJECT ObjDuplicateObject(LPOBJECT lpObject);
LPOBJECT ObjDuplicateObject(LPOBJECT lpObject, FRMTYPEINFO DstTypeInfo,
						BOOL bDoCMS = FALSE, LPFRMTYPEINFO lpSrcTypeInfo = NULL);

#endif _OBJUTIL_H_
