// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef __extbsr
#define __extbsr

// This .h file should provides API for extened brower
// The Extended browser provides list box and thumbnail magament


typedef struct _extstate {
	WORD 	wExtType;				// the extended type
	LPGETFILEPROC lpGetFileProc;	// to get file name from ext name
	LPGETPREVIEWPROC lpGetPreview;	// to get preview frame from ext name
	BOOL bMultiListBox;				// is it a mult select list box?
	BOOL ViewThumbnails;			
	BOOL AutoThumbnails;			// auto create thumbnails
} EXTSTATE, FAR *LPEXTSTATE;


BOOL ExtBsr_Init( HWND hDlg, LPEXTSTATE lpExtState, void FAR *lpExtUserData = NULL);
void ExtBsr_Done( HWND hDlg);
FNAME *ExtBsr_GetSelection( HWND hDlg, LPINT lpNum);

// for default processing in extended browser
BOOL ExtBsr_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
BOOL ExtBsr_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR *lpDrawItem);
BOOL ExtBsr_OnMeasureItem(HWND hDlg, LPMEASUREITEMSTRUCT lpMeasureItem);
BOOL ExtBsr_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
BOOL ExtBsr_OnDestroy(HWND hDlg);

// to handle info dialog
LPTSTR ExtBsrInfo_GetCurrentName(HWND hDlg, LPTSTR lpName);
BOOL WINPROC ExtBsrInfo_HandleDefaults( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam, ITEMID idDialog);
BOOL ExtBsrInfo_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) ;
void ExtBsr_OnPaletteChanged(HWND hDlg, HWND hwndPaletteChange);
void ExtBsrInfo_OnClose(HWND hDlg) ;
void ExtBsrInfo_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify) ;
void ExtBsrInfo_OnControlEnable(HWND hDlg, int fFlags) ;

#endif // __extbsr

