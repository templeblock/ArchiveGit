// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// routines to handle the image browsers thumbnail viewer

BOOL HandleThumbPane( HWND hDlg, WORD msg, LPARAM lParam );
void MeasureThumb(HWND hDlg, LPMEASUREITEMSTRUCT lpMeasure );
void DrawThumb(HWND hDlg,LPDRAWITEMSTRUCT lpDraw );
