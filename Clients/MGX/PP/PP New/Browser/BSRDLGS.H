//
// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081


// for bsrdlgs.c

typedef struct _choosealbum {
	FNAME szAlbum;				// The album choosen
	LPTSTR lpChooseTitle;		// FBC(fill before calling), Title of dialog (or NULL)
	LPTSTR lpChooseText;		// FBC, Text before combo box (or NULL)
	LPTSTR lpSelectedAlbum;		// FBC, the initial album to select (or NULL)
	LPTSTR lpSkipAlbum;			// FBC, the album which can NOT be selected (or NULL)
	} CHOOSEALBUM;
typedef CHOOSEALBUM FAR *LPCHOOSEALBUM;


extern LPCHOOSEALBUM lpChooseAlbum;

BOOL WINPROC DlgBsrPrefProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgBsrFileInfoProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgBsrAlbInfoProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgChooseAlbumProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
								   
int WINPROC DlgCopyFileProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
int WINPROC DlgCopyFilesProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
int WINPROC DlgRenameFileProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
int WINPROC DlgDeleteVirtualProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
int WINPROC DlgCleanupThumbsProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
int WINPROC DlgPrintThumbsProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgPrintThumbsProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
int WINPROC DlgMoveFileProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
int WINPROC DlgMoveFilesProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
int WINPROC DlgDeleteFileProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
int WINPROC DlgDeleteFilesProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

BOOL FileInfo_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
void FileInfo_OnClose(HWND hDlg);
void FileInfo_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);
void FileInfo_OnControlEnable(HWND hDlg, int fFlags);

BOOL CopyFileDialog(HWND hDlg, LPTSTR lpSrcString, LPTSTR lpDstString);
BOOL MoveFileDialog(HWND hDlg, LPTSTR lpSrcString, LPTSTR lpDstString);
BOOL CopyFilesDialog(HWND hDlg, LPTSTR lpDstString);
BOOL MoveFilesDialog(HWND hDlg, LPTSTR lpDstString);
BOOL RenameFileDialog(HWND hDlg, LPTSTR lpSrcString, LPTSTR lpDstString);
BOOL DeleteVirtualDialog(HWND hDlg, LPTSTR lpAlbString);

int GetDialogString(HWND hParent, LPTSTR lpInitString, LPTSTR lpSrcString, LPTSTR lpDstString, int idDlg);
