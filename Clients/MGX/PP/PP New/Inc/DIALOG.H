//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
#ifndef _DIALOG_H_
#define _DIALOG_H_

#define Dialog_DefProc DefWindowProc

BOOL Dialog_HandleDefaults(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam, ITEMID idDialog);
BOOL Dialog_OnSetCursor(HWND hDlg, HWND hWndCursor, UINT codeHitTest, UINT msg);
void Dialog_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem);
void Dialog_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem);
void Dialog_OnClose(HWND hDlg);
void Dialog_OnMenuSelect(HWND hDlg, HMENU hmenu, int item, HMENU hmenuPopup, UINT flags);
BOOL Dialog_OnQueryNewPalette(HWND hDlg);
void Dialog_OnPaletteChanged(HWND hDlg, HWND hwndPaletteChange);
HBRUSH Dialog_OnCtlColor(HWND hDlg, HDC hDC, HWND hControl, int wType);
BOOL Dialog_OnCreate(HWND hDlg, CREATESTRUCT FAR* lpCreateStruct);
void Dialog_OnDestroy(HWND hDlg);
void Dialog_OnNotify(HWND hDlg, int idFrom, NMHDR FAR* pnmhdr);
void Dialog_OnScroll(HWND hDlg, HWND hControl, UINT uCode, int iPos);

#endif // _DIALOG_H_
