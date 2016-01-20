//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _FILEOPT_H_
#define _FILEOPT_H_

#include "mgxbm.h"

// import dlg macros
#define LINE_ART        215
#define GRAY_SCALE16    216
#define GRAY_SCALE256   217
#define COLOR_16        219
#define COLOR_256       221
#define COLOR_24BIT     222

BOOL SetupSaveOptions(int idnFileType, BOOL bAlways, LPIMAGE lpImage = NULL);
BOOL GetSaveOptions(int idnFileType, LPPVOID lppOptions);
BOOL SetupImportOptions( int idFileType );

#endif // _FILEOPT_H_


