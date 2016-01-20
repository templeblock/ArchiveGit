/******************************************************************************
 *
 * MFRAME.C - Minerva MDI frame window procedure
 *
 * Copyright (C) 1996, 1997 Pegasus Imaging Corporation
 *
 * This source code is provided to you as sample source code.  Pegasus Imaging
 * Corporation grants you the right to use this source code in any way you
 * wish. Pegasus Imaging Corporation assumes no responsibility and explicitly
 * disavows any responsibility for any and every use you make of this source
 * code.
 *
 * Global functions:
 *     FrameWndProc   -    window procedure for MDI frame window
 *
 * Local functions:
 *     ForwardToChildren - SendMessage to all MDI child windows of some message
 *                         received by frame
 *     OnQueryNewPalette - WM_QUERYNEWALETTE - realize the palette for the active
 *                         MDI child window
 *
 * Revision History:
 *   12-18-96  1.00.03  jrb  once-over and additional commenting
 *   11-11-96  1.00.00  jrb  created
 *
 ******************************************************************************/



#define STRICT
/* system includes */
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
#pragma warning(default:4115 4201 4214)
#include <string.h>
#pragma warning(disable:4001)
#include "ctl3d.h"

/* PIC includes */
#include "pic.h"

/* minerva includes */
#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "minerva.h"
#include "mframe.h"
#include "mchild.h"
#include "mmenu.h"



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static void ForwardToChildren(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static int OnQueryNewPalette(HWND hwndChild);



/******************************************************************************
 *  global functions
 ******************************************************************************/



void FrameWaitCursorOn(void)
{
    if ( nWaitCursor++ != 0 )
        return;    
    // really turning it on
    SetCursor(hWaitCursor);
}



void FrameWaitCursorOff(void)
{
     if ( nWaitCursor == 0 || --nWaitCursor != 0 )
          return;
     // really turning it on
     SetCursor(hArrowCursor);
}



/******************************************************************************
 *  LRESULT FrameWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
 * 
 *  window procedure for MDI frame window
 *
 *  parameters:
 *      hWnd - handle to the frame window
 *      uMsg - message id
 *      wParam, lParam - depends on uMsg
 *
 *  returns:  
 *      depends on uMsg, ordinarily 0 if we handle the message
 *
 *  notes:
 *      The active child forwards its WM_MDIACTIVATE message to the frame
 *      so the frame can set the foreground palette to that child's palette
 *
 *      The frame also sets the foreground palette to the active child's
 *      palette on WM_QUERYNEWPALETTE messages
 *
 *      The frame forwards WM_PALETTECHANGED messages to all the MDI children
 *      so they can realize their palettes
 ******************************************************************************/
WNDPROC pfnOldMDIClientWndProc = 0;
LRESULT CALLBACK MDIClientWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRet;

    lRet = (*pfnOldMDIClientWndProc)(hWnd, uMsg, wParam, lParam); 
    if ( uMsg == WM_MOUSEMOVE )
        {
        if ( nWaitCursor != 0 )
            SetCursor(hWaitCursor);
        else

            SetCursor(hArrowCursor);                
        }
    return ( lRet );
}


LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CLIENTCREATESTRUCT ccs;
    HWND hwndChild = 0;
    
    if ( IsWindow(hwndMDIClient) )
        hwndChild = ChildGetActive();

    switch ( uMsg )
        {
        case WM_CREATE:
            /* following is regular MDI incantation */
            ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), WINDOW_MENU_POS);
            ccs.idFirstChild = IDM_WINDOW_CHILDREN;
            hwndMDIClient = CreateWindow(
                "mdiclient",
                NULL,
                WS_CHILD | WS_CLIPCHILDREN,
                0,
                0,
                0,
                0,
                hWnd,
                0,
                hinstThis,
                &ccs);
            if ( hwndMDIClient != 0 )
                {
                ShowWindow(hwndMDIClient, SW_SHOW);
                /* subclass the hwndMDIClient window so we can change the cursor
                    to a wait cursor when desired */
                pfnOldMDIClientWndProc =
                    (WNDPROC)SetWindowLong(hwndMDIClient, GWL_WNDPROC, (LONG)MDIClientWndProc);
                }
            return ( 0 );
        
        case WM_INITMENUPOPUP:
            /* some popup menu is about to be displayed */
            MenuStatus((int)LOWORD(lParam), (HMENU)wParam);
            return ( 0 );
        
        case WM_COMMAND:
            /* if WM_COMMAND is not from a control (then it's from a menu item) */
            if ( LOWORD(lParam) == 0 && MenuOnCommand(LOWORD(wParam)) )
                return ( 0 );
            break;

        case WM_QUERYENDSESSION:
            return ( ChildQueryCloseAll() );

        case WM_CLOSE:
            if ( !ChildQueryCloseAll() )
                return ( 0 );
            break;                
            
        case WM_DESTROY:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_QUIT, 0);
            PostQuitMessage(0);
            return ( 0 );

        case WM_QUERYNEWPALETTE:
            /* set the system palette to the active child's palette */
            return ( OnQueryNewPalette(hwndChild) );

        case WM_MDIACTIVATE:
            /* set the system palette to the newly active child's palette */
            OnQueryNewPalette((HWND)wParam);
            return ( 0 );

        case WM_PALETTECHANGED:
            /* forwarded to children so they can each realize their background palettes */
            ForwardToChildren(hWnd, uMsg, wParam, lParam);
            return ( 0 );

        case WM_SYSCOLORCHANGE:
            Ctl3dColorChange();
            break;

        case WM_SYSCOMMAND:
            switch ( wParam & 0xfff0 )
                {
                case SC_MAXIMIZE:
                case SC_RESTORE:
                    /* since we effectively change our image origin when maximized or restored,
                        we have to _force_ the window repaint to include the entire window --
                        else win '95 (at least) optimizes away some of the regions */
                    DefFrameProc(hWnd, hwndMDIClient, uMsg, wParam, lParam);
                    if ( IsWindow(hwndChild) )
                        {
                        InvalidateRect(hwndChild, NULL, FALSE);
                        UpdateWindow(hwndChild);
                        }
                    return ( 0 );
                
                default:
                    break;                    
                }
            break;
            
        case WM_MOUSEMOVE:
            if ( nWaitCursor != 0 )
                SetCursor(hWaitCursor);
            else
                SetCursor(hArrowCursor);                
            break;
        
        default:
            break;
        }
    if ( !IsWindow(hwndMDIClient) )
        return ( DefFrameProc(hWnd, 0, uMsg, wParam, lParam) );
    return ( DefFrameProc(hWnd, hwndMDIClient, uMsg, wParam, lParam) );
}



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 *  void ForwardToChildren(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
 * 
 *  SendMessage to all MDI child windows of some message received by frame
 *
 *  parameters:
 *      hWnd - handle to the frame window
 *      uMsg - message id
 *      wParam, lParam - depends on uMsg
 *
 *  notes:
 *      I include the Enum... proc here because it's not called by anybody else
 *      and it's easier for me to understand if it's right here.
 ******************************************************************************/

static BOOL CALLBACK EnumChildProcForward(HWND hwnd, LPARAM lParam)
{
    char sz[sizeof(MDICHILD_CLASSNAME) + 1];
    MSG PICFAR* pMsg = (MSG PICFAR*)lParam;

    /* some of the children of the client are not actually our mdi child windows
        For example, if a window is iconized, the icon legend is a child of the
        client.  For that reason, we compare the enum'ed window class name with
        the one we established for mdi children to make sure it's one of ours */
    GetClassName(hwnd, sz, sizeof(sz));
    if ( lstrcmp(sz, MDICHILD_CLASSNAME) == 0 )
        SendMessage(hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
    return ( TRUE );        
}

static void ForwardToChildren(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MSG msg;

    msg.message = uMsg;
    msg.wParam  = wParam;
    msg.lParam  = lParam;

    EnumChildWindows(hWnd, (BOOL (CALLBACK*)(HWND, LPARAM))EnumChildProcForward, (LPARAM)(MSG PICFAR *)&msg);
}



/******************************************************************************
 *  OnQueryNewPalette(HWND hwnd)
 *
 *  WM_QUERYNEWALETTE - realize the palette for the active MDI child window
 *
 *  parameters:
 *      hWnd - handle to the active child window
 *
 *  returns:
 *      result of RealizePalette to return to windows (number of colors changed
 *      in setting the foreground palette)
 ******************************************************************************/
static int OnQueryNewPalette(HWND hwndChild)
{            
    int nChanged = 0;
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwndChild);
    HDC hdc;
    HPALETTE hOldPalette;

    if ( pInstance != 0 && pInstance->hPalette != NULL )
        {
        hdc = GetDC(hwndFrame);
        hOldPalette = SelectPalette(hdc, pInstance->hPalette, FALSE);
        nChanged = RealizePalette(hdc);
        SelectPalette(hdc, hOldPalette, FALSE);
        ReleaseDC(hwndFrame, hdc);
        }
    return ( nChanged );
}
