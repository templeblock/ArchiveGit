//
// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "types.h"

// for bsrport.c

BOOL WINPROC DlgDirExportProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgDirImportProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgThumbMoveProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
int WINPROC DlgAlbumExportProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
int WINPROC DlgAlbumImportProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

