// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

/*
******************************************************************************

                                     About

******************************************************************************
*/
extern HANDLE hInstAstral;

static STRING StringBuffer;

#define hModule hInstAstral
#define hMainWindow hWndAstral
#define BUFFER_SIZE MAX_STR_LEN
#define hShdwBrush Window.hShadowBrush
#define load_library LoadLibrary
#define IF_SHADOW_BRUSH(a,b) 0
#define lChrchr(ptr,chr) ( *(ptr) == (chr) ? 0 : (ptr) )
#define lChrlen lstrlen
#define ParmString(ptr1,format,ptr2,nBytes)
#define dialog_box(hWnd,word,null)
#define AlignWindow(hWnd,flags,hParentWnd)

/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */

#define NLIBS           6
#define CASTPAUSE       50
#define README_STEPS    12

/* ********************************** Macros ******************************** */
/* ********************************** Types ********************************* */

typedef DWORD (FAR PASCAL *VERSPROC) (LPSTR,short);

/* ******************************* Local Data ******************************* */
/* ****************************** Exported Data ***************************** */
/* ***************************** Local Functions **************************** */

LOCAL void  NEAR PASCAL blink_readme (HWND,BOOL);
LOCAL void  NEAR PASCAL display_cast (HWND,BOOL);
LOCAL short NEAR PASCAL get_version_string (PSTR,PSTR,short);
LOCAL void  NEAR PASCAL show_library_versions (HWND,HWND);
LOCAL void  NEAR PASCAL paint_app_icon (HWND);


LOCAL void NEAR PASCAL blink_readme (
//  Blink a window on and off by every time a number of steps in completed.
HWND hDlg,
BOOL bReset)
{
    static short nSteps;

    if (bReset)
        nSteps = 0;
    else if (++nSteps >= README_STEPS)
    {
        HWND hReadMe = GetDlgItem (hDlg,READ_ME);
        BOOL bVisible = IsWindowVisible (hReadMe);

        nSteps = 0;
        ShowWindow (hReadMe,bVisible ? SW_HIDE : SW_SHOWNA);
    }
}


LOCAL void NEAR PASCAL display_cast (
//  Display a scrolling list of the Cast, beginning with a new player each
//  time.
HWND hDlg,
BOOL bScroll)
{
    TEXTMETRIC SysCharSize;
    HWND hCastWnd = GetDlgItem (hDlg,ABOUT_CAST);
    HDC hDC = GetDC (hCastWnd);

    GetTextMetrics(hDC, &SysCharSize );

    if (hDC)
    {
        static short CastOfs = -1;
        RECT Area;
        PSTR pNext;
        PSTR pCastName;
        HRGN hUpdate;
        short SaveLevel;
        short WndHeight;
        short CastHeight = 0;
        short OldBkMode = SetBkMode (hDC,TRANSPARENT);
        DWORD OldTextColor = SetTextColor (hDC,GetSysColor (COLOR_WINDOWTEXT));
        HBRUSH hBrush = CreateSolidBrush (GetSysColor (COLOR_WINDOW));
	STRING StringBuffer;

        GetClientRect (hCastWnd,&Area);
        if (bScroll)
        {
            RECT Update;

            ScrollDC (hDC,0,-1,NULL,NULL,NULL,&Update);
            SaveLevel = SaveDC (hDC);
            SelectClipRgn (hDC,hUpdate = CreateRectRgn (Update.left,
                Update.top - 2,Update.right,Update.bottom));

        }
        GetWindowText (hCastWnd,pCastName = pNext = StringBuffer,BUFFER_SIZE);
        do
            CastHeight += SysCharSize.tmHeight;
        while (pNext = (PSTR)lChrchr (pNext + 1,'\n'));
        WndHeight = (Area.bottom / SysCharSize.tmHeight) * SysCharSize.tmHeight;
        if ((WndHeight - --CastOfs) >= CastHeight)
            CastOfs = WndHeight;
        Area.top = CastOfs;
        while (pCastName)
        {
            PSTR pNext = (PSTR)lChrchr (pCastName,'\n');

            if (pNext)
                *pNext++ = '\0';
            Area.bottom = Area.top + SysCharSize.tmHeight;
            FillRect (hDC,&Area,hBrush);
            DrawText (hDC,pCastName,-1,&Area,DT_CENTER | DT_NOPREFIX |
                DT_SINGLELINE);
            pCastName = pNext;
            if ((Area.top = Area.bottom) > WndHeight)
                Area.top -= CastHeight;
        }
        if (bScroll)
        {
            RestoreDC (hDC,SaveLevel);
            DeleteObject (hUpdate);
        }
        DeleteObject (hBrush);
        SetTextColor (hDC,OldTextColor);
        SetBkMode (hDC,OldBkMode);
        ReleaseDC (hCastWnd,hDC);
    }
}


LOCAL short NEAR PASCAL get_version_string (
//  Attempt to load the given module and execute its "GetVersionString"
//  function.  If not found, return the library name alone. In all cases, return
//  the number of characters in the string as the value of the function.
PSTR pLibName,
PSTR pVerStr,
short nBytes)
{
    HANDLE hLib = load_library (pLibName);

    if (hLib)
    {
        VERSPROC lpProc = (VERSPROC)GetProcAddress (hLib,"GetVersionString");

        if (lpProc)
            (*lpProc) (pVerStr,nBytes);
        else
            ParmString (pVerStr,"*%%*",pLibName,nBytes);
        FreeLibrary (hLib);
    }
    else
        ParmString (pVerStr,"**%%**",pLibName,nBytes);
    return (lChrlen (pVerStr));
}


LOCAL void NEAR PASCAL paint_app_icon (
//  Find and draw the app icon described by the window text for the given
//  control
HWND hCtl)
{
    HDC hDC = GetDC (hCtl);
    STRING StringBuffer;

    GetWindowText (hCtl,StringBuffer,BUFFER_SIZE);
    DrawIcon (hDC,0,0,LoadIcon (hModule,StringBuffer));
    ReleaseDC (hCtl,hDC);
}


#ifdef UNUSED
LOCAL void NEAR PASCAL show_library_versions (
//  This function is called when the given button control is clicked in the
//  given dialog. The version string is retrieved for each library used by the
//  app. These strings are displayed in a popup window which waits for the mouse
//  or keypress which initiated this call to be released.
HWND hDlg,
HWND hCtl)
{
    TEXTMETRIC SysCharSize;
    HDC hDC = GetDC (hDlg);

    if (hDC)
    {
        GetTextMetrics(HDC, &SysCharSize );
        static short LibIDs[NLIBS] =
        {
            MGXLIB_MAIN,
            MGXLIB_FONT,
            MGXLIB_VBM,
            MGXLIB_VM,
            MGXLIB_WMF,
            TRANSLATOR_LIBRARY
        };
        short LibCounts[NLIBS];
        char LibStrings[NLIBS][BUFFER_SIZE];
        HWND hWnd;
        short Idx;
        short Ext;
        short MaxExt = 0;
        short X = SysCharSize.tmMaxCharWidth;
        short Y = SysCharSize.tmHeight / 2;
        short W = 2 * (X + GetSystemMetrics (SM_CXBORDER));
        short H = 2 * GetSystemMetrics (SM_CYBORDER) +
                SysCharSize.tmHeight * (NLIBS + 1);

        for (Idx = 0; Idx < NLIBS; Idx++)
        {
            LibCounts[Idx] = get_version_string (get_string (LibIDs[Idx]),
                LibStrings[Idx],BUFFER_SIZE);
            Ext = LOSHORT (GetTextExtent (hDC,LibStrings[Idx],LibCounts[Idx]));
            MaxExt = max (Ext,MaxExt);
        }
        ReleaseDC (hDlg,hDC);
        if (hWnd = CreateWindow (BUTTON_CLASS,"",WS_BORDER | WS_POPUP |
            SS_WHITERECT,0,0,W + MaxExt,H,hDlg,NULL,hModule,NULL))
        {
            MSG Msg;

            AlignWindow (hWnd,AW_TOP | AW_LEFT | AW_BYCLIENT,hCtl);
            ShowWindow (hWnd,SW_SHOWNA);
            UpdateWindow (hWnd);
            if (hDC = GetDC (hWnd))
            {
                short OldBkMode = SetBkMode (hDC,TRANSPARENT);

                for (Idx = 0; Idx < NLIBS; Idx++, Y += SysCharSize.tmHeight)
                    TextOut (hDC,X,Y,LibStrings[Idx],LibCounts[Idx]);
                SetBkMode (hDC,OldBkMode);
                ReleaseDC (hWnd,hDC);
            }
            SendMessage (hCtl,BM_SETSTATE,FALSE,0L);
            SetCapture (hWnd);
            while (GetMessage (&Msg,NULL,0,0) &&
                !(Msg.message == WM_KEYUP || Msg.message == WM_LBUTTONUP))
                ;
            ReleaseCapture ();
            DestroyWindow (hWnd);
        }
    }
}

/* ***************************** Public Functions *************************** */
/* **************************** Exported Functions ************************** */

BOOL FAR PASCAL DlgAboutProc( hDlg, Message, wParm, lParm )
HWND hDlg;
unsigned Message;
WORD wParm;
long lParm;
{
    BOOL bHandled = TRUE;

    if (Message == WM_INITDIALOG)
    {
        AlignWindow (hDlg,AW_BYCLIENT,hMainWindow);
        EnableWindow (GetDlgItem (hDlg,ABOUT_CAST),FALSE);
        blink_readme (hDlg,TRUE);
    }
    else if (Message == WM_TIMER)
    {
        display_cast (hDlg,TRUE);
        blink_readme (hDlg,FALSE);
    }
    else if (Message == WM_CTLCOLOR)
        bHandled = IF_SHADOW_BRUSH (LOWORD (lParm),hShdwBrush);
    else if (Message == WM_COMMAND)
    {
        if (wParm == IDOK || wParm == IDCANCEL)
            EndDialog (hDlg,wParm == IDOK);
        else if (wParm == ABOUT_CAST && HIWORD (lParm) == BN_DISABLE)
        {
            display_cast (hDlg,FALSE);
            SetTimer (hDlg,NULL,CASTPAUSE,NULL);
        }
        else if (wParm == VERSION_ICON || WITHIN (wParm,BLURB_MIN,BLURB_MAX))
            if (HIWORD (lParm) == BN_HILITE)
                PostMessage (hDlg,WM_COMMAND,wParm,MAKELONG (LOWORD (lParm),
                    BN_CLICKED));
            else if (HIWORD (lParm) == BN_CLICKED)
                if (wParm == VERSION_ICON)
                    ;//show_library_versions (hDlg,LOWORD (lParm));
                else
                    dialog_box (hDlg,wParm,NULL);
            else if (HIWORD (lParm) == BN_PAINT)
                paint_app_icon (LOWORD (lParm));
    }
    else
        bHandled = FALSE;
    return (bHandled);
}
#endif
