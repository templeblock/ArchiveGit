// Source file: ..\src\wintools.c
typedef void (FAR *LPPOPUPPROC)(HWND hWnd);

void SetSpinBuddy(HWND hDlg, int idCtl);
void SetSmallCaption(HWND hWindow);
int AstralHint( int idString, LPPSTR lppString );
int AstralHintEx( int idString, LPTSTR lpBuffer, int nBufLen);
int AstralStr( int idString, LPPSTR lppString );
int AstralStrEx( int idString, LPTSTR lpBuffer, int nBufLen);
BOOL StringsEqualID( LPTSTR lpString1, ITEMID idString2 );
void GetWinXY(HWND hWnd, LPINT lpX, LPINT lpY);
void MoveIntoParentView( HWND hWnd );
void CenterPopup( HWND hWnd );
void GetScreenCenter(HWND hWnd, LPPOINT ppt, int dx, int dy);
void OffsetWindow( HWND hWnd, int dx, int dy, BOOL bRedraw );
void RightPopup( HWND hWnd );
void LeftPopup( HWND hWnd );
void SavePopup( HWND hWnd, LPTSTR lpKeyName );
void SaveFullPopup( HWND hWnd, LPTSTR lpKeyPosName, LPTSTR lpKeySizeName);
BOOL RestorePopup( HWND hWnd, LPTSTR lpKeyName, LPPOPUPPROC lpDefaultRoutine );
BOOL RestoreFullPopup( HWND hWnd, LPTSTR lpKeyPosName, LPTSTR lpKeySizeName, LPPOPUPPROC lpDefaultRoutine );
void DisableSavebits( HWND hWnd );
void EnableSavebits( HWND hWnd );
void SetClientRect( HWND hWnd, LPRECT lpRect );
void GetDrawClientRect( HWND hWnd, LPRECT lpRect );
void GetRealClientRect( HWND hWnd, LPRECT lpRect );
void GetScrollBarSize( LPINT lpCx, LPINT lpCy );
void SetFullClientRect( HWND hWnd, BOOL fFull );
VOID AstralMenuCheck( int menuID, int * oldmenuID );
int SetUnitResolution( int res );
void Dline( HDC hDC, int x1, int y1, int x2, int y2, BOOL flag );
void DrawLine( HDC hDC, int x1, int y1, int x2, int y2, HPEN hPen );
void Drect( HDC hDC, LPRECT lpRect, BOOL flag );
void DrawRect( HDC hDC, LPRECT lpRect, HPEN hPen );
void Dbox( HDC hDC, LPRECT lpRect, BOOL flag );
void Dellipse( HDC hDC, LPRECT lpRect, BOOL flag );
VOID SetKeyState( int nVirtKey, BOOL fDown, BOOL fToggle );
void ShowControl( HWND hDlg, int idControl1, int idControl2, BOOL bShow, BOOL bDefer );
void ShowPane( HWND hDlg, int idMarker1, int idMarker2, BOOL bShow );
void MovePane( HWND hDlg, ITEMID idControl1, ITEMID idControl2, int dx, int dy);
void ShowDlgItems( HWND hDlg, int idControl1, int idControl2, BOOL bShow, BOOL bDefer );
void ShowDlgItem( HWND hDlg, ITEMID idControl, BOOL bShow );
void ControlEnableGroup( HWND hDlg, int idControl1, int idControl2, BOOL bEnable );
VOID SetAllDlgItemsText(HWND hDlg, int id, LPTSTR lpText);
HWND CopyWindow( HWND hWnd );
BOOL EnableOverlappedWindow( HWND hWnd, BOOL bEnable );
HANDLE AstralLoadLibrary(LPTSTR lpLibraryName);
void Delay( long lTicks );
BOOL DelayIfMouseDown( long lTicks );
void SendMessageToChildren(HWND hParent, UINT msg, WPARAM wParam, LPARAM lParam);
void DrawBevel( HDC hDC, LPRECT lpRect, BOOL bBevelin, int iWidth );
void DrawCorners( HDC hDC, LPRECT lpRect, BOOL bBevelin );
void SetClipDC( HDC hDC, LPRECT lpRect );
BOOL SelectClipRect(HDC hDC, LPRECT lpNewRect, LPRECT lpOldRect );
void DelUnusedSysMenuItems(HWND hWnd, BOOL fGrayItems, BOOL fSize);
void ShowExtra(HWND hParent, int idFirst, int idLast, int idShow);
HWND GetSafeChildWindow(HWND hParent, int idControl);
HBRUSH SetClassBackground(HWND hWnd, HBRUSH hBrush);
HCURSOR SetClassCursor(HWND hWnd, HCURSOR hCursor);
int GetClassExtra(HWND hWnd);
void ShowTime(LPTSTR lpString, BOOL bInit);
#ifdef WIN32
void MoveTo(HDC hDC, int x, int y);
#endif
HBITMAP MgxLoadBitmap(HINSTANCE hInst, LPCTSTR lpszResourceName);
void MgxDrawBitmap( HDC hDC, int x, int y, HBITMAP hBitmap, BOOL fEnabled );

