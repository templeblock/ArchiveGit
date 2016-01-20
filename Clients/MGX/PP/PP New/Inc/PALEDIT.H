//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _PALEDIT_H_
#define _PALEDIT_H_

BOOL PaletteEdit( LPIMAGE lpImage );
LPCOLORMAP PaletteEditor( LPCOLORMAP lpColorMap );
BOOL PaletteEditImage(LPIMAGE lpImage, LPPALETTEEDIT_PARMS lpParms);
BOOL WINPROC EXPORT DlgPaletteEditorProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC EXPORT DlgPaletteEditorLoadProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC EXPORT DlgPaletteEditorSaveProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

#endif // _PALEDIT_H_
