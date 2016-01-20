//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _CMDUNDO_H_
#define _CMDUNDO_H_

typedef	struct
	{
	EXTNAME szExtName;
	LIST 	List;
	} MACROEDIT, FAR *LPMACROEDIT;

BOOL WINPROC EXPORT DlgCmdListUndoProc( HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam );
BOOL WINPROC EXPORT DlgCmdListRedoProc( HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam );
BOOL WINPROC EXPORT DlgCmdListEditProc( HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam );
BOOL WINPROC EXPORT DlgMacroEditProc( HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam );

#endif // _CMDUNDO_H_
