//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _CMDENABL_H_
#define _CMDENABL_H_

BOOL AlwaysEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsObjectFunctionEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsNonBaseObjectFunctionEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsMultipleObjectFunctionEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsSizeEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsInvertEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsConvertEnabled(CImage *pImage, CServerView *pView, LPCONVERT_PARMS lpParms);
BOOL IsCutEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsMappingEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsTransformObjectsEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsMergeMaskEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsPasteEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsEffectEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsMaskFunctionEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsFeatherMaskEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsFeatherObjectEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsEditUndoEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsMaskUndoEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsPaletteEditEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsUnlockedObjectFunctionEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsLockedObjectFunctionEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsPSFilterEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsCropMaskEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsChromaEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsGroupEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsUngroupEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms);
BOOL IsSelectObjectsRelativeEnabled(CImage *pImage, CServerView *pView, LPSELECTOBJRELATIVE_PARMS lpParms);

#endif // _CMDENABL_H_

