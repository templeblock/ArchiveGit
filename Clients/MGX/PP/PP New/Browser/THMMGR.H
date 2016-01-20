// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#ifndef __thmmgr
#define __thmmgr

// routines to manage thumbnails (Image Previews and Description) Files

#include "bsrtypes.h"

// BASE Functions 

LPTHMNAIL	THMOpen( LPTHMNAIL lpThm, LPENTRY lpEntry);
void		THMClose(LPTHMNAIL lpThm);
LPTHMNAIL	THMCreate( LPTHMNAIL lpThm, LPENTRY lpEntry, BOOL bLocal);
LPFRAME		THMGetFrame(LPTHMNAIL lpThm);
BOOL		THMPutFrame(LPTHMNAIL lpThm, LPFRAME lpFrame, int width, int height, LONG dataType, LONG fileType, LONG time);
BOOL		THMDeleteFrame(LPTHMNAIL lpThm);
LPDESC		THMGetDescription(LPTHMNAIL lpThm);
BOOL		THMPutDescription(LPTHMNAIL lpThm, LPDESC lpDescription);
BOOL		THMDeleteDescription(LPTHMNAIL lpThm);
LPTHMINFO	THMGetInfo(LPTHMNAIL lpThm);
BOOL 		THMReCreateEasyInfo(LPTHMNAIL lpThm, LPFILELOC lpFile, BOOL bAlways);
BOOL 		THMReCreateFrame(LPTHMNAIL lpThm, LPFILELOC lpFile, BOOL bAlways);
BOOL 		THMCopyThumb(LPTHMNAIL lpDstThm, LPTHMNAIL lpSrcThm, ULONG time);
BOOL		THMValidate(LPTHMNAIL lpThm, LPFILELOC lpFile, WORD level);

LPTHMNAIL 	ReadPCDThumb(LPTHMNAIL lpThm, LPTSTR lpFileName);
LPTHMNAIL	ReadOldThumb(LPTHMNAIL lpThm, LPTSTR lpFileName, LPTSTR lpThumbDir);

#endif

