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

/****************************************************************************

    PROGRAM: AVIHED

    PURPOSE:    AVI Hotspot Editor
                Copyright (c) 1993 Microsoft Corp.
                Written by David Feinleib: August, 1993
    
    MODIFIED	September/October 1993 by Wayne Radinsky:
    			Separated source from hotspot app, hotspot viewer DLL,
    			and hotspot VBX.  Converted hotspot data from INI file
    			to binary HOT file.

****************************************************************************/

/*
	main.c:
		WinMain -- Calls initialization function, processes message loop
		InitApplication -- Initializes window data and registers
			window class
		InitInstance -- Saves instance handle and creates main window
		MainWndProc -- Processes messages
		SaveWindowRect -- Write specified rect to win.ini 
		RestoreWindowRect -- Retreives main window position from win.ini
		RestoreDlgRect -- same thing but with dialogs...
		RestoreSelectDlgRect
*/		
		

#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <viewer.h>
#include <string.h>
#include <commdlg.h>
#include <stdlib.h>

#include "hotspot.h"
#include "avihed.h"            /* specific to this program          */

PMOVIEINFO pMovieInfo;
HANDLE  hInst;               /* current instance              */
UINT    uDoubleClickTime = 0;
BOOL    bShowValidRectsOnly = TRUE;
BOOL    bModified = FALSE;
BOOL    bFileLoaded = FALSE;
char    szBaseWindowTitle[] = "AVI Hotspot Editor";
char    szWindowTitle[80];
int     PixelVary = 3;
HWND    hwndDlg;               // handle of FrameDlg
HWND    hwndSelectDlg;         // handle of SelectDlg
RECT    rcwndDlg;
RECT    rcwndSelectDlg;


/*
	WinMain -- calls InitApplication() and initAVI(), initializes
		some global variables, then translates and dispatches messages
		until WM_QUIT message breaks the loop.		
*/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;                /* current instance         */
HANDLE hPrevInstance;            /* previous instance        */
LPSTR lpCmdLine;                 /* command line             */
int nCmdShow;                    /* show-window type (open/icon) */
{
    MSG msg;                     /* message              */

    if (!hPrevInstance)          /* Other instances of app running? */
    if (!InitApplication(hInstance)) /* Initialize shared things */
        return (FALSE);      /* Exits if unable to initialize     */

    /* Perform initializations that apply to a specific instance */

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    /* Acquire and dispatch messages until a WM_QUIT message is received. */
    
    uDoubleClickTime = GetDoubleClickTime();
    PixelVary = GetProfileInt(szBaseWindowTitle, "PixelVary", 3);
    
    initAVI();
    
    while (GetMessage(&msg,    /* message structure              */
        NULL,          /* handle of window receiving the message */
        NULL,          /* lowest message to examine          */
        NULL))         /* highest message to examine         */
    {
	    TranslateMessage(&msg);    /* Translates virtual key codes       */
	    DispatchMessage(&msg);     /* Dispatches message to window       */
    }
    
    termAVI();
    
    return (msg.wParam);       /* Returns the value from PostQuitMessage */
}


/* 
	InitApplication -- registers window class.  This gives Windows a
		pointer to MainWndProc.
*/
BOOL InitApplication(hInstance)
HANDLE hInstance;                  /* current instance       */
{
    WNDCLASS  wc;

    /* Fill in window class structure with parameters that describe the       */
    /* main window.                                                           */

    wc.style = CS_DBLCLKS;                    /* Class style(s).                    */
    wc.lpfnWndProc = MainWndProc;       /* Function to retrieve messages for  */
                                        /* windows of this class.             */
    wc.cbClsExtra = 0;                  /* No per-class extra data.           */
    wc.cbWndExtra = 0;                  /* No per-window extra data.          */
    wc.hInstance = hInstance;           /* Application that owns the class.   */
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName =  "AvihedMenu";   /* Name of menu resource in .RC file. */
    wc.lpszClassName = "AvihedWClass"; /* Name used in call to CreateWindow. */

    /* Register the window class and return success/failure code. */

    return (RegisterClass(&wc));

}


/*
	InitInstance() -- creates main application window, putting it at
		the same coordiantes (via RestoreWindowRect()) if possible.
*/

BOOL InitInstance(hInstance, nCmdShow)
    HANDLE          hInstance;          /* Current instance identifier.       */
    int             nCmdShow;           /* Param for first ShowWindow() call. */
{
    HWND            hWnd;               /* Main window handle.                */
    RECT            rc;

    /* Save the instance handle in static variable, which will be used in  */
    /* many subsequence calls from this application to Windows.            */
    hInst = hInstance;

    /* Create a main window for this application instance.  */
    
    if (RestoreWindowRect(&rc))
        hWnd = CreateWindow(
        "AvihedWClass",                 /* See RegisterClass() call.          */
        szBaseWindowTitle,              /* Text for window title bar.         */
        WS_OVERLAPPEDWINDOW,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        NULL,                           /* Overlapped windows have no parent. */
        NULL,                           /* Use the window class menu.         */
        hInstance,                      /* This instance owns this window.    */
        NULL                            /* Pointer not needed.                */
        );
    
    else    
        hWnd = CreateWindow(
            "AvihedWClass",                 /* See RegisterClass() call.          */
            szBaseWindowTitle,              /* Text for window title bar.         */
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,                  /* Default horizontal position.       */
            CW_USEDEFAULT,                  /* Default vertical position.         */
            CW_USEDEFAULT,                  /* Default width.                     */
            CW_USEDEFAULT,                  /* Default height.                    */
            NULL,                           /* Overlapped windows have no parent. */
            NULL,                           /* Use the window class menu.         */
            hInstance,                      /* This instance owns this window.    */
            NULL                            /* Pointer not needed.                */
            );

    /* If window could not be created, return "failure" */

    if (!hWnd)
        return (FALSE);

    /* Make the window visible; update its client area; and return "success" */

    if (GetProfileInt(szBaseWindowTitle, "Maximized", 0))
        ShowWindow(hWnd, SW_SHOWMAXIMIZED);
    else
        ShowWindow(hWnd, nCmdShow);  /* Show the window                        */
    UpdateWindow(hWnd);          /* Sends WM_PAINT message                 */
    return (TRUE);               /* Returns the value from PostQuitMessage */

}

/*
	MainWndProc --
		processes messages.  see comments by message handlers
*/

long CALLBACK __export MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;                      /* window handle                 */
UINT message;                   /* type of message               */
WPARAM wParam;                  /* additional information        */
LPARAM lParam;                  /* additional information        */
{
    
    static DLGPROC dlgproc;    
    static DLGPROC dlgproc2;    
    static RECT rcMain;
    static RECT rcDlg;
    POINT pt;
    BOOL fFlag = FALSE;
    int xDlg;
    static POINT ptOld;
    
    switch (message)
    {
		// WM_CREATE -- makes two dialog boxes, using coordiantes from the
		// app's INI file if possible, otherwise computing some, and
		// uses MoveWindow to put them where they go.
        case WM_CREATE:
            dlgproc = (DLGPROC) MakeProcInstance(FrameDlg, hInst);
            hwndDlg = CreateDialog(hInst, MAKEINTRESOURCE(FRAMEDLG), hWnd, dlgproc);
            
            dlgproc2 = (DLGPROC) MakeProcInstance(SelectDlg, hInst);
            hwndSelectDlg = CreateDialog(hInst, MAKEINTRESOURCE(SELECTDLG), hWnd, dlgproc2);
            
            if (!RestoreDlgRect(&rcwndDlg))
            {                
                // move the frame dlg            
                GetClientRect(hWnd, &rcMain);
                pt.x = rcMain.left;
                pt.y = rcMain.top;
                ClientToScreen(hWnd, &pt);
                rcMain.left = pt.x;
                rcMain.top = pt.y;
            
                pt.x = rcMain.right;
                pt.y = rcMain.bottom;
                ClientToScreen(hWnd, &pt);
                rcMain.right = pt.x;
                rcMain.bottom = pt.y;
                        
                GetWindowRect(hwndDlg, &rcDlg);                
                MoveWindow(hwndDlg, rcMain.right - (rcDlg.right - rcDlg.left),
                             rcMain.top, 
                             rcDlg.right - rcDlg.left,
                             rcDlg.bottom - rcDlg.top, TRUE);
            }
            else
                MoveWindow(hwndDlg, rcwndDlg.left, rcwndDlg.top, rcwndDlg.right - rcwndDlg.left,
                                         rcwndDlg.bottom - rcwndDlg.top, TRUE);

            if (!RestoreSelectDlgRect(&rcwndSelectDlg))
			{
                             
                xDlg = rcMain.right - (rcDlg.right - rcDlg.left);

                // move the select dlg                             
                GetClientRect(hWnd, &rcMain);
                pt.x = rcMain.left;
                pt.y = rcMain.top;
                ClientToScreen(hWnd, &pt);
                rcMain.left = pt.x;
                rcMain.top = pt.y;
            
                pt.x = rcMain.right;
                pt.y = rcMain.bottom;
                ClientToScreen(hWnd, &pt);
                rcMain.right = pt.x;
                rcMain.bottom = pt.y;
                        
                GetWindowRect(hwndSelectDlg, &rcDlg);
                MoveWindow(hwndSelectDlg, xDlg - 2 - (rcDlg.right - rcDlg.left),
                             rcMain.top, 
                             rcDlg.right - rcDlg.left,
                             rcDlg.bottom - rcDlg.top, TRUE);                                         
			}
            else
                MoveWindow(hwndSelectDlg, rcwndSelectDlg.left, 
                                            rcwndSelectDlg.top, 
                                                rcwndSelectDlg.right - rcwndSelectDlg.left, 
                                                    rcwndSelectDlg.bottom - rcwndSelectDlg.top,
                                                     TRUE);
            return TRUE;

		// WM_LBUTTONDOWN -- if they clicked in a hotspot, remember the
		// coordinate in ptOld, otherwise set ptOld to -1's.
        case WM_LBUTTONDOWN:
            if (!pMovieInfo)
                break;            
                
            if (2 == (OnButtonDown(pMovieInfo, message, wParam, lParam)))
			{
				ptOld = MAKEPOINT(lParam);
			}                                
            else 
                ptOld.y = ptOld.x = -1;                                
            break;            

        // WM_LBUTTONDBLCLK -- if they dbl clicked, OnButtonDblClk will
        // do the work...
        case WM_LBUTTONDBLCLK:
            if (!pMovieInfo)
                break;            
            OnButtonDblClk(pMovieInfo, message, wParam, lParam);
            break;
            
		// WM_KEYUP --  if they hit delete, delete current ptOld's hotspot.
        case WM_KEYUP:
            if (!pMovieInfo)
                break;            
            if (wParam == VK_DELETE)
            {
                RECT rc;
                if (ptOld.x != -1 && ptOld.y != -1)
                {
                    CopyRect(&rc, &HotspotFromPoint(pMovieInfo, ptOld)->rc);
                    DeleteHotspot(pMovieInfo, 
                            HotspotFromPoint(pMovieInfo, ptOld));                                                                                        
                    ptOld.x = ptOld.y = -1;
                    bModified = TRUE;
                    InvalidateRect(hWnd, &rc, TRUE);
                    UpdateWindow(hWnd);                    
                    ShowSelectInfo((RECT FAR *)NULL);
                }
            }
            break;

		// WM_SIZE -- if they minimize, remember our coordinates in
		// rcMain, rcwndDlg and rcwndSelectDlg
        case WM_SIZE:            
            if (wParam != SIZE_MINIMIZED)
            {                                    
                GetWindowRect(hWnd, &rcMain);
                if (hwndDlg)
                    GetWindowRect(hwndDlg, &rcwndDlg);
                if (hwndSelectDlg)                    
                    GetWindowRect(hwndSelectDlg, &rcwndSelectDlg);
            }                            
            return 0L;

		// WM_PAINT -- use MCI_UPDATE to refresh the AVI window, then
		// call DrawRects to draw the hotspots on the picture.
        case WM_PAINT:
            {
            HDC hDC;
            PAINTSTRUCT ps;
            MCI_DGV_UPDATE_PARMS mciUpdate;
            
            hDC = BeginPaint(hWnd, &ps);
            if (pMovieInfo)
            {
                mciUpdate.hDC = hDC;
                mciUpdate.rc = ps.rcPaint;
                mciUpdate.wReserved0 = 0;
                mciUpdate.dwCallback = NULL;
                mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_UPDATE, 
                            MCI_WAIT | MCI_DGV_RECT | MCI_DGV_UPDATE_PAINT | MCI_DGV_UPDATE_HDC,
                            (DWORD)(LPMCI_DGV_UPDATE_PARMS)&mciUpdate);
                                            
                if (pMovieInfo->pHotspotList && pMovieInfo->hwndMovie
                                && !pMovieInfo->fPlaying)
                    DrawRects(hDC, pMovieInfo);            
            }
            EndPaint(hWnd, &ps);                            
            }
            return TRUE;
                            
		// WM_MOVE -- If not minimized, remember where we got moved to,
        case WM_MOVE:                        
            if (!IsIconic(hWnd))
                {                                    
                GetWindowRect(hWnd, &rcMain);
                if (hwndDlg)
                    GetWindowRect(hwndDlg, &rcwndDlg);
                if (hwndSelectDlg)                    
                    GetWindowRect(hwndSelectDlg, &rcwndSelectDlg);                                    
                }            
            return 0L;
            
		// check/uncheck menu items depending on state  of related flags
        case WM_INITMENU:
            CheckMenuItem(wParam, IDM_SHOWFRAMEHOTSPOTS,
                (bShowValidRectsOnly ? MF_CHECKED : MF_UNCHECKED));

            if (pMovieInfo)
                {
                EnableMenuItem(wParam, IDM_PRINT,
                    pMovieInfo->pHotspotList ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem(wParam, IDM_SAVE,
                    (pMovieInfo->pHotspotList && bModified) ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem(wParam, IDM_SAVEAS,
                    pMovieInfo->pHotspotList ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem(wParam, IDM_DELETE,
                    pMovieInfo->pHotspotList ? MF_ENABLED : MF_GRAYED);                    
                }
            else
                {
                EnableMenuItem(wParam, IDM_PRINT, MF_GRAYED);
                EnableMenuItem(wParam, IDM_SAVE, MF_GRAYED);
                EnableMenuItem(wParam, IDM_SAVEAS, MF_GRAYED);
                EnableMenuItem(wParam, IDM_DELETE, MF_GRAYED);                
                }
            break;

        case WM_COMMAND:       /* message: command from application menu */
            switch (wParam)
                {
                case IDM_ABOUT:
                    DialogBox(hInst,        /* current instance          */
                        "AboutBox",         /* resource to use           */
                        hWnd,               /* parent handle             */
                        About);             /* About() instance address  */                
                    break;
                        
                case IDM_OPEN:
                    FileOpen(hWnd);
                    break;
                    
                case IDM_SAVEAS:
                    fFlag = TRUE;
                case IDM_SAVE:
                    if (pMovieInfo)
                        {
                        if (pMovieInfo->pHotspotList)
                            FileSave(hWnd, fFlag);
                        }
                    break;
                    
                case IDM_EXIT:                    
                    PostMessage(hWnd, WM_CLOSE, 0, 0L);
                    break;
                    
                case IDM_SHOWFRAMEHOTSPOTS:
                    bShowValidRectsOnly ^= 1;
                    if (pMovieInfo)
                        {
                        if (pMovieInfo->hwndMovie)
                            {
                            InvalidateRect(pMovieInfo->hwndMovie, NULL, TRUE);
                            UpdateWindow(pMovieInfo->hwndMovie);
                            }
                        }
                    break;
                    
                case IDM_REDRAW:
                    if (pMovieInfo)
                        {
                        if (pMovieInfo->hwndMovie)
                            {
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);
                            }
                        }                    
                    break;                    
                    
                case IDM_DELETE:
                    if (pMovieInfo)
                        {
                        if (pMovieInfo->hwndMovie)
                            {
                            PostMessage(pMovieInfo->hwndMovie, WM_KEYUP, VK_DELETE, 0L);
                            }
                        }
                    break;                    
                    
                default:
                    return (DefWindowProc(hWnd, message, wParam, lParam));                
                }
            break;            

		// WM_CLOSE -- prompt user to save if necessary.            
        case WM_CLOSE:
            if (bModified)
                {
                int fSave = IDNO;
                fSave = MessageBox(hWnd, "Save Changes?",                                    
                                   szBaseWindowTitle, MB_YESNOCANCEL | MB_ICONQUESTION);
                if (fSave == IDCANCEL)
                    return 0L;
                if (fSave == IDYES)
                    {
                    if (pMovieInfo)
                        {
                        if (pMovieInfo->pHotspotList)
                            FileSave(hWnd, FALSE);
                        }                
                    }
                if (fSave == IDNO)
                    ;                                                               
                }
            DestroyWindow(hWnd);        
            return 0L;            
            
        case MM_MCINOTIFY:
             /* This is where we check the status of an AVI  */
            /* movie that might have been playing.  We do   */
            /* the play with MCI_NOTIFY on so we should get */
            /* a MCI_NOTIFY_SUCCESSFUL if the play      */
            /* completes on it's own.           */
            switch(wParam){         
                case MCI_NOTIFY_FAILURE:
                case MCI_NOTIFY_ABORTED:
                case MCI_NOTIFY_SUPERSEDED:
                case MCI_NOTIFY_SUCCESSFUL:
                    {
                    char szBuf[129];
                    
                    pMovieInfo->fPlaying = FALSE;
                    pMovieInfo->dwCurrentFrame = GetMovieFrame(pMovieInfo);
                    wsprintf(szBuf, "%ld of %ld", pMovieInfo->dwCurrentFrame,
                                                      pMovieInfo->dwMovieLength);
                    SetDlgItemText(hwndDlg, ID_CURRENTFRAME, szBuf);                        
                    SetDlgItemText(hwndDlg, ID_PLAY, "Play");
                    EnableWindow(GetDlgItem(hwndDlg, ID_FORWARD), TRUE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_REVERSE), TRUE);                    
                    InvalidateRect(hWnd, NULL, FALSE);
                    UpdateWindow(hWnd);
                    }
                    return 0;
            }
            break;

		// we're going to die!
        case WM_DESTROY:          /* message: window being destroyed */            
            SaveWindowRect(hWnd, rcMain);            
            DestroyWindow(hwndDlg);             // get rid of modeless dialog
            FreeProcInstance(dlgproc);
            DestroyWindow(hwndSelectDlg);             // get rid of modeless dialog
            FreeProcInstance(dlgproc2);
            if (pMovieInfo)
                {
                fileCloseMovie(pMovieInfo, FALSE);
                DeleteHotspotList(pMovieInfo);            
                FREE(pMovieInfo);
                }
            termAVI();            
            PostQuitMessage(0);
            break;

        default:                  /* Passes it on if unproccessed    */
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}


/*

	SaveWindowRect -- Write specified rect to win.ini.  Uses hWnd
		to determine minimize/maximize status of window, and saves
		that as well
*/

void SaveWindowRect(HWND hWnd, RECT rc)
{
    char szBuf[129];
    
    if (rc.left < 0)
        rc.left = 0;
    if (rc.top < 0)
        rc.top = 0;            
    wsprintf(szBuf, "%d,%d,%d,%d", rc.left, rc.top, rc.right, rc.bottom);    
    WriteProfileString(szBaseWindowTitle, "Window", szBuf);
    
    if (IsZoomed(hWnd))
        WriteProfileString(szBaseWindowTitle, "Maximized", "1");
    else
        WriteProfileString(szBaseWindowTitle, "Maximized", "0");        

    wsprintf(szBuf, "%d,%d,%d,%d", rcwndDlg.left, rcwndDlg.top, rcwndDlg.right, rcwndDlg.bottom);    
    WriteProfileString(szBaseWindowTitle, "wndDlg", szBuf);
    
    wsprintf(szBuf, "%d,%d,%d,%d", rcwndSelectDlg.left, rcwndSelectDlg.top, 
                                    rcwndSelectDlg.right, rcwndSelectDlg.bottom);
    WriteProfileString(szBaseWindowTitle, "wndSelectDlg", szBuf);

    wsprintf(szBuf, "%d", PixelVary);    
    WriteProfileString(szBaseWindowTitle, "PixelVary", szBuf);
    
}

/*
	RestoreWindowRect -- Retreives main window position from win.ini
		(note this doesn't actually change the window position)
*/
BOOL RestoreWindowRect(RECT FAR *pRect)
{
    char szBuf[129];
    LPSTR token;

    if (!pRect)
        return FALSE;

    if (GetProfileString(szBaseWindowTitle, "Window", "", szBuf, 128))
        {
        token = FindToken(szBuf, ',');        
        pRect->left = StrToInt(token);
        
        token = FindToken(NULL, ',');        
        pRect->top = StrToInt(token);
        
        token = FindToken(NULL, ',');        
        pRect->right = StrToInt(token);
        
        token = FindToken(NULL, ',');        
        pRect->bottom = StrToInt(token);            
        return TRUE;
        }
    else
        return FALSE;
}        


BOOL RestoreDlgRect(RECT FAR *pRect)
{
    char szBuf[129];
    LPSTR token;

    if (!pRect)
        return FALSE;

    if (GetProfileString(szBaseWindowTitle, "wndDlg", "", szBuf, 128))
        {
        token = FindToken(szBuf, ',');        
        pRect->left = StrToInt(token);
        
        token = FindToken(NULL, ',');        
        pRect->top = StrToInt(token);
        
        token = FindToken(NULL, ',');        
        pRect->right = StrToInt(token);
        
        token = FindToken(NULL, ',');        
        pRect->bottom = StrToInt(token);            
        return TRUE;
        }
    else
        return FALSE;

}

        
// Same as RestoreRect, basically...
BOOL RestoreSelectDlgRect(RECT FAR *pRect)
{
    char szBuf[129];
    LPSTR token;

    if (!pRect)
        return FALSE;

    if (GetProfileString(szBaseWindowTitle, "wndSelectDlg", "", szBuf, 128))
        {
        token = FindToken(szBuf, ',');        
        pRect->left = StrToInt(token);
        
        token = FindToken(NULL, ',');        
        pRect->top = StrToInt(token);
        
        token = FindToken(NULL, ',');        
        pRect->right = StrToInt(token);
        
        token = FindToken(NULL, ',');        
        pRect->bottom = StrToInt(token);            
        return TRUE;
        }
    else
        return FALSE;
}
        


