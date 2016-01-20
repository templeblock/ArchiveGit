/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993, 1994  Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/


#ifdef WIN32
#define EXPORT
#else
#define EXPORT __export
#endif

BOOL EXPORT CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT EXPORT CALLBACK MixTestWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/* void Cls_OnMM_WOM_DONE(HWND hwnd, int iChannel, LPMIXWAVE lpMixWave); */
#define HANDLE_MM_WOM_DONE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(wParam), (LPMIXWAVE)(lParam)), 0L)
#define FORWARD_MM_WOM_DONE(hwnd, bCommand, lpNCB, fn) \
    (void)(fn)((hwnd), WM_MM_WOM_DONE, (WPARAM)(iChannel),(LPARAM) lpMixWave)
