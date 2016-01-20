//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _IMGCMD_H_
#define _IMGCMD_H_

BOOL SaveMaskImage(LPIMAGE lpImage, LPSAVEMASK_PARMS lpParms);
BOOL CopyToClipboard(LPIMAGE lpImage, LPCMD_PARMS lpParms);
BOOL CutImage(LPIMAGE lpImage, LPTINTFILL_PARMS lpParms);
BOOL AlphaEditImage(LPIMAGE lpImage, LPCMD_PARMS lpParms);
BOOL CombineObjects(LPIMAGE lpImage, LPCMD_PARMS lpParms);
BOOL CombineSelObjects(LPIMAGE lpImage, LPCMD_PARMS lpParms);
BOOL CopyToFile(LPIMAGE lpImage, LPCOPYTOFILE_PARMS lpParms);
BOOL CropMaskImage( LPIMAGE lpImage, LPCMD_PARMS lpParms);
BOOL CropImage( LPIMAGE lpImage, LPCROP_PARMS lpParms );
BOOL DoCloseImage( LPIMAGE lpImage, LPCMD_PARMS lpParms);
BOOL CreateClipImage( LPIMAGE lpImage, LPCMD_PARMS lpParms);
BOOL EditUndoImage(LPIMAGE lpImage, LPCMD_PARMS lpParms);
BOOL MaskUndoImage(LPIMAGE lpImage, LPCMD_PARMS lpParms);
BOOL DiscardUndo(LPIMAGE lpImage, LPDISCARDUNDO_PARMS lpParms);
BOOL ImgEditApply( LPIMAGE lpImage, LPCMD_PARMS lpParms );
#endif // _IMGCMD_H_

