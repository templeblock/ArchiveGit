//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _CONTROLS_H_
#define _CONTROLS_H_
long WINPROC Lever_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
long WINPROC Icon_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
long WINPROC LabeledFrame_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
long WINPROC VisualFrame_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
long WINPROC Color_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
long WINPROC Color2_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
long WINPROC Hue_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
long WINPROC Histo_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
long WINPROC TextBlock_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
BOOL TextBlock_GetData( int id, LPTSTR lpData, int iSize );
long WINPROC Panel_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
long WINPROC StaticText_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
long WINPROC Image_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
void Image_SetFrame(HWND hControl, LPFRAME lpFrame);
void DrawColorPatch( HDC hDC, LPRECT lpRect, LPRGB lpRGB, BOOL bSuperBlt );
void SetControlValue( HWND hDlg, int nIDDlgItem, int nIndex, long lValue, BOOL fNotify );
long GetControlValue( HWND hDlg, int nIDDlgItem, int nIndex );
void DrawImageControl( HDC hDC, HWND hWnd, LPFRAME lpFrame, LPRECT lpDestRect, int dx, int dy, BOOL fShadows, LPVOID lpCmsXform = NULL);
void ComputeImageControlRect( HWND hWnd, LPRECT lpRect, int iImageWidth, int iImageHeight, BOOL fShadows);
void EraseControlBackground( HDC hDC, HWND hWnd, LPRECT lpRect, WORD wType );
HBRUSH GetBackgroundBrush( HDC hDC, HWND hWnd, WORD wType);
HBRUSH SelectBackgroundBrush( HDC hDC, HWND hWnd, WORD wType );

/***********************************************************************/
/***********************************************************************/
// COMMON CONTROL IMPLEMENTATION
/***********************************************************************/
/***********************************************************************/
#define Control_DefProc DefWindowProc

UINT Control_OnGetDlgCode(HWND hWindow, LPMSG lpmsg);
BOOL Control_OnEraseBkgnd(HWND hWindow, HDC hDC);
void Control_OnEnable(HWND hWindow, BOOL fEnable);
void Control_OnSetText(HWND hWindow, LPCTSTR lpszText);
void Control_OnDestroy(HWND hWindow);
void Control_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
void Control_OnSetFont(HWND hWindow, HFONT hFont, BOOL fRedraw);
void Control_OnPaletteChanged(HWND hWindow, HWND hwndPaletteChange);
void Control_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
void Control_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
void Control_HandleFocus(HWND hWindow, BOOL fSetFocus);

extern HDC Control_shDC;
extern BOOL Control_bTrack;

#endif // _CONTROLS_H_

