/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/
#include "resource.h"

#define MAXFILENAME 256          /* maximum length of file pathname      */

// main.c --

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
void SaveWindowRect(HWND hWnd, RECT rc);
BOOL RestoreWindowRect(RECT FAR *pRect);
BOOL RestoreDlgRect(RECT FAR *pRect);
BOOL RestoreSelectDlgRect(RECT FAR *pRect);

long CALLBACK __export MainWndProc(HWND, UINT, WPARAM, LPARAM);

// sbcls.c --

void ShowSelectInfo(RECT FAR *prcRect);
void DrawSelect( HDC hdc, RECT FAR *prcClip, BOOL fDraw);
void PASCAL NormalizeRect (RECT *prc);
void TrackMouse (HWND hwnd, POINT pt, RECT *prcClip);
BOOL Overlap(PMOVIEINFO pMovieInfo, RECT rc, PHOTSPOT pThisHotspot);
BOOL SelectHotspot(PMOVIEINFO pMovieInfo, PHOTSPOT pHotspot);
BOOL PointOnLine(POINT pt, int x1, int y1, int x2, int y2);
BOOL SizeOK(RECT rc);
BOOL MoveHotspot(PMOVIEINFO pMovieInfo, PHOTSPOT pHotspot, POINT pt);
PHOTSPOT HotspotFromPoint(PMOVIEINFO pMovieInfo, POINT pt);
int OnButtonDown(PMOVIEINFO pMovieInfo, USHORT msg, USHORT wp, LONG lp);
int OnButtonDblClk(PMOVIEINFO pMovieInfo, USHORT msg, unsigned wp, LONG lp);
void DrawRects(HDC hDC, PMOVIEINFO pMovieInfo);
LONG FAR PASCAL __export SbClsProc (HWND hwnd, USHORT msg, USHORT wp, LONG lp);

// dlg.c --

BOOL __export CALLBACK FrameDlg(HWND hDlg, unsigned message, WORD wParam, LONG lParam);
BOOL __export CALLBACK SelectDlg(HWND hDlg, unsigned message, WORD wParam, LONG lParam);
BOOL __export CALLBACK HotspotDlg(HWND hDlg, unsigned message, WORD wParam, LONG lParam);
BOOL __export CALLBACK About(HWND, unsigned, WORD, LONG);


//  file.c --

BOOL FileOpen(HWND hWnd);
BOOL FileSave(HWND hWnd, BOOL bGetFileName);

