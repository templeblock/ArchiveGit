//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _DLGMGR_H_
#define _DLGMGR_H_


// values for iModeless parameter to AstralDlgParam
#define DLG_MODAL				0
#define DLG_MODELESS_ONBOTTOM 	-1
#define DLG_MODELESS_ONTOP		1
#define DLG_MODELESS_MULTIPLE	2
#define DLG_MODELESS_TOPMOST	3

int AstralControlRect( HWND hWindow, int idControl, LPRECT lpRect );
int AstralControlPaint( HWND hWindow, int idControl );
int AstralControlRepaint( HWND hWindow, int idControl );
int ControlEnable( HWND hWindow, int idControl, BOOL bGray );
HWND AstralDlg( int iModeless, HINSTANCE hInstance, HWND hWndParent, int idDlgIn, DLGPROC lpDlgControl );
HWND AstralDlgParam( int iModeless, HINSTANCE hInstance, HWND hWndParent, int idDlgIn, DLGPROC lpDlgControl, LPARAM lParamInit );
int AstralDlgEnd( HWND hDlg, int iResult );
void AstralDlgDestroy( HWND hDlg );
void AstralDlgEndAll( void );
BOOL AstralDlgMsg( LPMSG lpMsg );
BOOL AstralDlgClear( HWND hDlg );
HWND AstralDlgGet( int idDlg);
int AstralDlgGetID( HWND hDlg );
BOOL AstralDlgCheck( HWND hDlg );
BOOL AstralDlgSet( int idDlg, HWND hDlg, BOOL bModalDlg = FALSE );
void AstralDlgShow( int idDlg );
void AstralDlgHide( int idDlg );

#endif // _DLGMGR_H_

