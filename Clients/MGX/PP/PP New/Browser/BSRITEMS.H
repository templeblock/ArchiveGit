//							  
// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081


// for bsritems.c

 // routines to handle items in the browser


void FillDrivesCombo(HWND hDlg);
BOOL FillDirectoryList(HWND hDlg);
void FillFileList(HWND hDlg, BOOL bMaintainSelect);
void FillAlbList(HWND hDlg);
void FillFileType(HWND hDlg);
void FillDataType(HWND hDlg);
void FillOpenMode(HWND hDlg);
void EnableDataTypes(HWND hDlg);
void EnableCompress(HWND hDlg);
void EnableOpenSaveButton(HWND hDlg);
void EnableInfoButton(HWND hDlg);
void SetPathText(HWND hDlg);
void SetDriveSizeText(HWND hDlg);
void SetFileNameText(HWND hDlg, LPTSTR lpString, BOOL bSaveSel);
BOOL SetFileListInfo(HWND hDlg);
BOOL HandleDrives(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify);
BOOL HandleDirectories(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify);
BOOL HandleAlbumList(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify);
BOOL HandleAlbumCombo(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify);
BOOL ChangeFileSel(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL HandleFileList(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify);
BOOL HandleFileType(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify);
BOOL HandleDataType(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify);
BOOL HandleOpenMode(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify);
BOOL HandleFileName(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify);
BOOL FileListMeasure(HWND hDlg, LPMEASUREITEMSTRUCT lpMeasure);
BOOL FileListDraw(HWND hDlg, LPDRAWITEMSTRUCT lpDraw);
