// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#ifndef __cthmmgr
#define __cthmmgr

// routines to manage thumbnails (Image Previews and Description) Files
//	Performs caching for thmmgr.
// 	Do not mix calls to the two sets of functions.
 
#include "bsrtypes.h"

//**************************************************************************
// 	Call BEFORE ANY OTHER CTHM OPERATION!!!. 
//**************************************************************************
void CTHMInitCache();
//**************************************************************************
// 	Call AFTER DONE WITH ALL CTHM OPERATIONS!!!. 
//**************************************************************************
void CTHMEndCache(void);
//**************************************************************************

LPCTHMNAIL	CTHMOpen(LPENTRY lpEntry);
LPCTHMNAIL	CTHMOpenCached(LPFILELOC lpFile);
void		CTHMClose(LPCTHMNAIL lpCThm);
void		CTHMPurge(LPCTHMNAIL lpCThm);
LPCTHMNAIL	CTHMCreate(LPENTRY lpEntry, BOOL bLocal);
LPFRAME		CTHMGetFrame(LPCTHMNAIL lpCThm);
BOOL		CTHMPutFrame(LPCTHMNAIL lpCThm, LPFRAME lpFrame, int width, int height, LONG dataType, LONG fileType, LONG time);
BOOL		CTHMDeleteFrame(LPCTHMNAIL lpCThm);
LPDESC		CTHMGetDescription(LPCTHMNAIL lpCThm);
BOOL		CTHMPutDescription(LPCTHMNAIL lpCThm, LPDESC lpDescription);
BOOL		CTHMDeleteDescription(LPCTHMNAIL lpCThm);
BOOL 		CTHMReCreateEasyInfo(LPCTHMNAIL lpCThm, LPFILELOC lpFile, BOOL bAlways);
BOOL 		CTHMReCreateFrame(LPCTHMNAIL lpCThm, LPFILELOC lpFile, BOOL bAlways);
BOOL 		CTHMCopyThumb(LPCTHMNAIL lpDstCThm, LPCTHMNAIL lpSrcCThm, ULONG time);
int			CTHMValidate(LPCTHMNAIL lpCThm, LPFILELOC lpFile, WORD level);

LPCTHMNAIL 	CReadPCDThumb(LPTSTR lpFileName);
LPCTHMNAIL	CReadOldThumb(LPTSTR lpFileName, LPTSTR lpThumbDir);

#endif
