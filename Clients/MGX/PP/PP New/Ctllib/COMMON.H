// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _COMMON_H_
#define _COMMON_H_

#include "common.rh"

/***********************************************************************/
/***********************************************************************/
// COMMON CONTROL IMPLEMENTATION
/***********************************************************************/
/***********************************************************************/

/* All controls should have the first 4 words reserved */
/* either private data or a data pointer should follow */
#define GWW_STATE		0
#define GWL_FONT		(GWW_STATE + sizeof(WORD))
#define GWW_STATUS		(GWL_FONT + sizeof(long))
#define GWL_CTLDATAPTR	(GWW_STATUS + sizeof(WORD))
#define GWW_START		(GWL_CTLDATAPTR + sizeof(long))

/* Common Control Notifications */
#define CTLN_CLICKED        0           // matches BN_CLICKED
#define CTLN_DOUBLECLICKED  5           // matches BN_DOUBLECLICKED
#define CTLN_BROWSED        10
#define CTLN_UNBROWSED      11
#define CTLN_BUTTONDOWN     12

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
void Control_OnSetState(HWND hWindow, BOOL fState);
void Control_OnSetCheck(HWND hWindow, BOOL fCheck);
BOOL Control_OnGetCheck(HWND hWindow);

extern HDC Control_shDC;
extern BOOL Control_bTrack;

#endif // _COMMON_H_
