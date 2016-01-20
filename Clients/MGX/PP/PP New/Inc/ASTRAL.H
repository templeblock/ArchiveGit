//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
#ifndef _ASTRAL_H_
#define _ASTRAL_H_

#include "image.h"

int AstralGetMessage( LPMSG lpMsg );
void AstralProcessMessage( LPMSG lpMsg );
long WINPROC AstralWndProc( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
BOOL SetupCursor( HWND hWnd, UINT HitCode, ITEMID idWnd );
void CloseImage( BOOL fCloseAll, LPIMAGE lpCloseImage, HWND hWnd, BOOL fRegenrating = FALSE);
BOOL AnotherView(LPIMAGE lpTheImage);
LPTSTR GetUntitledName( LPTSTR szUntitled );
BOOL HandleQueryNewPalette(HWND hWnd, BOOL fSpecialPal, BOOL fColorPal);
BOOL IsOurWindow(HWND hWnd);
BOOL WackWindows( HINSTANCE hInst );
BOOL CALLBACK CallBkProc( HWND hWnd, LPARAM lParam );
BOOL CALLBACK CallBkProc1( HWND hWnd, LPARAM lParam );
HMENU AstralGetSubMenu(HMENU hMenu, int nPos);
void Astral_OnMenuSelect(HWND hWindow, HMENU hMenu, int item, HMENU hMenuPopup, UINT flags);
void ShowHideMenu( WORD idMenu, int fShow, HMENU hMenu);
void SetShowHideMenus( WORD idMenu, WORD idDialogBox, HMENU hMenu=NULL);
void OnUpdateShowHideMenu( WORD idMenu, BOOL fShow, CCmdUI* pCmdUI );
void OnUpdateSetShowHideMenus( WORD idMenu, WORD idDialogBox, CCmdUI* pCmdUI );

#endif // _ASTRAL_H_


