/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1991 - 1992.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                   mgxdde.c

******************************************************************************
******************************************************************************

  Description:  DDE functions.
 Date Created:  02/20/91
       Author:  Kevin McFarland

*****************************************************************************/

#ifdef BORLANDC
#pragma option -zC_DDE
#endif

#include <windows.h>
#include <dde.h>

#include "data.h"
#include "routines.h"
#include "id.h"

//#include "formatst.h"
//#include "mgxdde.h"

/********************************* Constants ********************************/

#ifdef DEBUG
    #define Debug(Statement)    {Statement;}
    #define LOCAL
#else
    #define Debug(Statement)
    #define LOCAL               static
#endif

/****************************************************************************/

#define MAXSTR 256

/******************************** Local Data ********************************/

LOCAL BOOL  bAck         = FALSE;
LOCAL BOOL  bExecute     = FALSE;
LOCAL BOOL  bInitiate    = FALSE;
LOCAL BOOL  bTerminated  = FALSE;
LOCAL BOOL  bTerminating = FALSE;

LOCAL HWND  hDDE         = NULL;
LOCAL HWND  hServer      = NULL;

LOCAL int   nBufferMax   = 0;
LOCAL LPSTR lpDataBuffer = NULL;

/****************************** Local Functions *****************************/

LOCAL HWND NEAR PASCAL send_dde_initiate (ATOM,ATOM);
LOCAL void NEAR PASCAL dde_wait ();

/****************************************************************************/

LOCAL HWND NEAR PASCAL send_dde_initiate (aApp,aTopic)
ATOM aApp;
ATOM aTopic;
{
    bInitiate = TRUE;
    SendMessage ((HWND)-1,WM_DDE_INITIATE,(WORD)hDDE,MAKELONG (aApp,aTopic));
    bInitiate = FALSE;
    return hServer;
}

/****************************************************************************/

LOCAL void NEAR PASCAL dde_wait ()
{
    MSG  msg;

    LockData (0);
    while (PeekMessage (&msg,NULL,NULL,NULL,PM_REMOVE)) 
    {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
    UnlockData (0);
}

/****************************** Public Functions ****************************/

BOOL FAR PASCAL dde_exec (hWnd,lpCmd)
HWND  hWnd;
LPSTR lpCmd;
{
    HANDLE hMem;
    LPSTR  lpMem;

    hMem = GlobalAlloc (GMEM_MOVEABLE | GMEM_SHARE,(LONG) lstrlen (lpCmd) + 1);
    if (hMem && (lpMem = GlobalLock (hMem)))
    {
        lstrcpy (lpMem,lpCmd);
        GlobalUnlock (hMem);
        bAck = FALSE;
        bExecute = TRUE;
        PostMessage (hWnd,WM_DDE_EXECUTE,(WORD)hDDE,MAKELONG (NULL,hMem));
        dde_wait ();
        bExecute = FALSE;
    }
    return bAck;
}

/****************************************************************************/

HWND FAR PASCAL dde_initiate (lpApp,lpTopic)
LPSTR lpApp;
LPSTR lpTopic;
{
    ATOM aApp   = GlobalAddAtom (lpApp);
    ATOM aTopic = GlobalAddAtom (lpTopic);
    HWND hWnd   = send_dde_initiate (aApp,aTopic);

    if (!hWnd) 
    {
        if (WinExec (lpApp,SW_SHOWNORMAL))
            hWnd = send_dde_initiate (aApp,aTopic);
    }
    GlobalDeleteAtom (aApp);
    GlobalDeleteAtom (aTopic);
    return hWnd;
}

/****************************************************************************/

BOOL FAR PASCAL dde_request (hWnd,lpTopic,lpBuffer,nMaxChars)
HWND  hWnd;
LPSTR lpTopic;
LPSTR lpBuffer;
int   nMaxChars;
{
    ATOM aTopic = GlobalAddAtom (lpTopic);

    bAck = FALSE;
    lpDataBuffer = lpBuffer;
    nBufferMax = nMaxChars;
    if (!PostMessage (hWnd,WM_DDE_REQUEST,(WORD)hDDE,MAKELONG (CF_TEXT,aTopic)))
        GlobalDeleteAtom (aTopic);
    while (lpDataBuffer)
        dde_wait ();
    return bAck;
}

/****************************************************************************/

BOOL FAR PASCAL dde_terminate (hWnd)
HWND hWnd;
{
    bTerminating = TRUE;
    bTerminated  = FALSE;

    PostMessage (hWnd,WM_DDE_TERMINATE,(WORD)hDDE,0L);
    dde_wait ();
    bTerminating = FALSE;
    if (hWnd == hServer)
        hServer = NULL;
    return bTerminated;
}

/****************************************************************************/

BOOL FAR PASCAL create_dde_window (hInst)
/*  Registers, if first instance, and creates DDE window. The function 
    returns the wndow handle if successful or NULL otherwse. */
HANDLE hInst;
{
    BOOL bResult;
    WNDCLASS wcDDE;

    if (!hDDE)
    {
        wcDDE.hCursor       = NULL;
        wcDDE.hIcon         = NULL;
        wcDDE.lpszMenuName  = NULL;
        wcDDE.lpszClassName = "MGXDDECLASS";
        wcDDE.hbrBackground = NULL;
        wcDDE.hInstance     = hInst;
        wcDDE.style         = 0;
        wcDDE.lpfnWndProc   = (WNDPROC)dde_window_proc;
        wcDDE.cbClsExtra    = 0;
        wcDDE.cbWndExtra    = 0;
        if (RegisterClass (&wcDDE))
            hDDE = CreateWindow ("MGXDDECLASS",NULL,0L,0,0,0,0,(HWND)NULL,
                    (HMENU)NULL,(HANDLE)hInst,(LPSTR)NULL);
        else
            hDDE = NULL;
    }
    return (BOOL)hDDE;
}

/****************************************************************************/

VOID FAR PASCAL destroy_dde_window (hInst)
HANDLE hInst;
{
    if (hDDE)
    {
        DestroyWindow (hDDE);
        UnregisterClass ("MGXDDECLASS",hInst);
        hDDE = NULL;
    }
}

/****************************************************************************/

LONG FAR PASCAL dde_window_proc (hWnd,wMessage,wParam,lParam)
HWND hWnd;
WORD wMessage;
WORD wParam;
LONG lParam;
{
    BOOL    bHandled = TRUE;
    HANDLE  hData;
    LONG    lResult  = 0L;

    switch (wMessage)
    {
    case WM_DDE_ACK:
        if (bExecute)
        {
            hData = (HANDLE) HIWORD (lParam);
            if (hData)
                GlobalFree (hData);
        }
        else
            GlobalDeleteAtom (HIWORD (lParam));
        if (bInitiate) 
        {
            GlobalDeleteAtom (LOWORD (lParam));
            hServer = (HWND)wParam;
            lResult = 1L;
            bAck = TRUE;
        } 
        else 
            bAck = (LOWORD (lParam) & 0x8000);
        if (!bAck)  // prevent endless loop when waiting for WM_DDE_DATA!
            lpDataBuffer = NULL;
        break;
    case WM_DDE_DATA:
        hData = (HANDLE) LOWORD (lParam);
        if (hData)
        {
            ATOM aItem = HIWORD (lParam);
            BOOL bRelease;
            DDEDATA far *lpData;

            lpData = (DDEDATA far *) GlobalLock (hData);
            if (!lpData || lpData->cfFormat != CF_TEXT)
                PostMessage ((HWND)wParam,WM_DDE_ACK,(WORD)hDDE,MAKELONG(0,aItem));
            else if (lpDataBuffer)
                wsprintf (lpDataBuffer,(LPSTR)lpData->Value);
//                FormatStr (lpDataBuffer,nBufferMax,(LPSTR)lpData->Value);
            if (lpData->fAckReq)
                PostMessage ((HWND)wParam,WM_DDE_ACK,(WORD)hDDE,MAKELONG(0x8000,aItem));
            else
                GlobalDeleteAtom (aItem);
            bRelease = lpData->fRelease;
            GlobalUnlock (hData);
            if (bRelease)
                GlobalFree (hData);
        }
        lpDataBuffer = NULL;
        break;
    case WM_DDE_TERMINATE:
        if (!bTerminating)
            PostMessage ((HWND)wParam,WM_DDE_TERMINATE,(WORD)hDDE,0L);
        bTerminated = TRUE;
        break;
    default:
        bHandled = FALSE;
    }
    if (!bHandled)
        lResult = DefWindowProc (hWnd,wMessage,wParam,lParam);    
    return lResult;
}

/****************************************************************************/

