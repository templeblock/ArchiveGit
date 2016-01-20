//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _LISTBOX_H_
#define _LISTBOX_H_

#include "listbox.rh"

#define LBN_MOVEITEM	32000	// 16-bit value sufficiently high
								// not to interfere with MS
#ifdef CTLLIB
BOOL ListBox_RegisterClass (HINSTANCE hInstance, UINT ClassScope);
BOOL ListBox_UnRegisterClass (HINSTANCE hInstance);
#endif

// called in response to LBN_MOVEITEM notification
void ListBox_GetMoveItem(HWND hWindow, LPINT lpOldPos, LPINT lpNewPos);
void ListBox_MoveEnable(HWND hWindow, BOOL fEnable);
void ListBox_SetMoveCursor(HWND hWindow, HCURSOR hCursor);
void ListBox_SetSelectDir(HWND hWindow, int iSelectDir);

#endif // _LISTBOX_H_

