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
//      avihede.h -- extern variable declarations

extern PMOVIEINFO pMovieInfo;
extern HANDLE hInst;               /* current instance              */
extern BOOL bModified;
extern BOOL bFileLoaded;
extern BOOL bShowValidRectsOnly;
extern BOOL bModified;
extern UINT uDoubleClickTime;
extern HWND hwndDlg;                // handle of FrameDlg
extern HWND hwndSelectDlg;
extern int PixelVary;
extern char szBaseWindowTitle[];
extern char szWindowTitle[80];
extern char szMovieInfo[];
extern RECT rcwndSelectDlg;
extern RECT rcwndDlg;

