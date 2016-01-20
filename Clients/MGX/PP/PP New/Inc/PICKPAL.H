//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _PICKPAL_H_
#define _PICKPAL_H_

BOOL PalettePicker( LPCOLORINFO lpColor, LPCOLORINFO lpAltColor, LPCOLORMAP lpColorMap );
BOOL WINPROC EXPORT DlgPalettePickerProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

#endif // _PICKPAL_H_
