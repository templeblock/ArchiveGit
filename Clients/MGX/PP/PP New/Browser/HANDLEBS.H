// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081


//  routines to handle browser actions


// 	Menu routines
BOOL CreateDir(HWND hDlg);
BOOL RenameDir(HWND hDlg);
BOOL DeleteDir(HWND hDlg);
BOOL MoveFilesToAlb(HWND hDlg);
BOOL DeleteFiles(HWND hDlg);
BOOL RemoveFilesFromAlb(HWND hDlg);
BOOL CopyFiles(HWND hDlg);
BOOL MoveFiles(HWND hDlg);
BOOL RenameFiles(HWND hDlg);
BOOL GoToFile(HWND hDlg);
BOOL NewAlbum(HWND hDlg, BOOL bFill);
BOOL RenameAlbum(HWND hDlg, LPTSTR lpAlbum, BOOL bFill);
BOOL DeleteAlbum(HWND hDlg, LPTSTR lpAlbum, BOOL bFill);
BOOL SelectFiles(HWND hDlg, BOOL bAll);
BOOL InvertSelectFiles(HWND hDlg);
void Search(HWND hDlg);
BOOL CleanupThumbs(HWND hDlg);
//	Action routines
BOOL BrowserDlgBegin(HWND hDlg);
void BrowserDlgEnd(HWND hDlg);
void SetupItems(HWND hDlg);
void FillItems(HWND hDlg, BOOL bMain, BOOL bMaintainFileSelect)		 ;
void ClearItems(HWND hDlg);
void ShowAlbumWindow(HWND hDlg, BOOL bViewAlbums);
void HandleOK(HWND hDlg);
void SetUpSearchSpec(HWND hDlg, LPTSTR lpPath);
BOOL SetDirectoryFromDef(HWND hDlg);
void UpdateFileThumbs(HWND hDlg, WORD level);
void DeleteFileThumbs(HWND hDlg);
BOOL ChangeDir(HWND hDlg, LPTSTR lpDir, BOOL bUpdateFileSpec);
BOOL CopyThumbnail(LPFILELOC lpSrcFile, LPFILELOC lpDstFile, ULONG time);
LPCTHMNAIL GetCDThumb(LPFILELOC lpFile);
LFIXED SetupPrintThumbs(LPLFIXED lpwidth, LPLFIXED lpheight);
BOOL PrintThumbs(HWND hDlg, BOOL bSelectedOnly);





