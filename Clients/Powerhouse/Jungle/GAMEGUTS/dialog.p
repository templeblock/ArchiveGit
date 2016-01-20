// Source file: dialog.cpp
BOOL DialogControlRect( HWND hWindow, ITEMID idControl, LPRECT lpRect);
BOOL DialogControlPaint( HWND hWindow, ITEMID idControl);
BOOL DialogControlRepaint( HWND hWindow, ITEMID idControl);
BOOL ControlEnable( HWND hWindow, ITEMID idControl, BOOL bGray);
HWND DialogStart( int iModeless, HINSTANCE hInstance, HWND hWndParent, ITEMID idDlgIn, DLGPROC lpDlgControl );
int DialogEnd( HWND hDlg, int iResult);
void DialogEndAll( void );
BOOL DialogMsg( LPMSG lpMsg );
BOOL DialogClear( HWND hDlg );
void DialogRemove( HWND hDlg );
HWND DialogGet( ITEMID idDlg );
ITEMID DialogGetID( HWND hDlg );
DLGPROC DialogGetProc( HWND hDlg );
BOOL DialogCheck( HWND hDlg );
BOOL DialogSet( ITEMID idDlg, HWND hDlg, DLGPROC lpProc );
void DialogShow( ITEMID idDlg );
void DialogHide( ITEMID idDlg );

