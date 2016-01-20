/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                    rooms.c

******************************************************************************
******************************************************************************

  Description:  Room dialog functions.
 Date Created:  01/11/94
       Author:  Michael P. Dagate

*****************************************************************************/

#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern	HINSTANCE	hInstAstral;
extern	HWND		hWndAstral;
extern	HINSTANCE	hCurrentDLL;
extern	BOOL		bPaintAppActive;

//#ifdef CDBUILD
//extern	BOOL		bFunkyOpening;
//#endif

#define PRIVATE_INI "CRAYOLA.INI"

/******************************** Private Data ******************************/

static	BOOL	bRegistered;
HBITMAP			hRoomBitmap;	// global so DLL's can use this for animations
static	HDC		hRoomDC;
static	int		iJoke;

/****************************** Public Functions ****************************/

void FAR PASCAL EXPORT GoPaintApp (HWND hDlg,LPSTR lpParams)
{
    BOOL bPrint;

    PostMessage (hDlg,WM_CLOSE,0,0L);
    HandleCommandLine (hWndAstral,lpParams,&bPrint);
}

/****************************************************************************/

void FAR PASCAL LoadRoomBitmap (UINT idRoom)
{
    AstralCursor (IDC_WAIT);

    // Unload previous room:
    //=======================
    if (hRoomDC)
    {
        DeleteDC (hRoomDC);
        hRoomDC = 0;
    }
    if (hRoomBitmap)
    {
        DeleteObject (hRoomBitmap);
        hRoomBitmap = 0;
    }
    *szPreview = 0;

    // Load new room:
    //================
    if (idRoom)
    {
        LPFRAME lpFrame = NULL;
        char szFile[MAX_PATH];
        WORD DataType;

        // load room image:
        PathCat (szFile,Control.ProgHome,GetString (idRoom,NULL));
        if (Exists (szFile))
            lpFrame = tifrdr (szFile,-1,(LPINT)&DataType,TRUE);
        if (!lpFrame)
        {
            PathCat (szFile,Control.PouchPath,GetString (idRoom,NULL));
            lpFrame = tifrdr (szFile,-1,(LPINT)&DataType,TRUE);
        }
        if (lpFrame)
        {
            hRoomBitmap = FrameToBitmap (lpFrame,NULL);
            FrameClose (lpFrame);
            if (hRoomBitmap)
            	hRoomDC = CreateCompatibleDC (0);
        }
    }
    AstralCursor (NULL);
}

HBITMAP FAR PASCAL GetRoomBitmap()
{
	return hRoomBitmap;
}

/****************************************************************************/

BOOL FAR PASCAL PaintRoomBitmap (HWND hWnd)
{
    BITMAP bm;
    BOOL   bHandled = FALSE;
    HDC    hDC;
    int    x,y;
    RECT   rBitmap;
    RECT   rSrc,rDst;
    PAINTSTRUCT ps;
	HBITMAP hOld;

    HintLine (0);
    if (hRoomBitmap && hRoomDC)
    {
		// Only select in hRoomBitmap when needed, It is selected elsewhere.
		hOld = (HBITMAP)SelectObject (hRoomDC,hRoomBitmap);
        hDC = BeginPaint (hWnd,&ps);
        rSrc = ps.rcPaint;
        MapWindowPoints (hWnd,hWnd,(LPPOINT)&rSrc,2);
        x = ps.rcPaint.left - rSrc.left;
        y = ps.rcPaint.top - rSrc.top;
        GetObject (hRoomBitmap,sizeof(BITMAP),(LPSTR)&bm);
        SetRect (&rBitmap,0,0,bm.bmWidth - 1,bm.bmHeight - 1);
        if (IntersectRect (&rSrc,&rSrc,&rBitmap))
        {
            rDst = rSrc;
            OffsetRect (&rDst,x,y);
            BitBlt (hDC,rDst.left,rDst.top,RectWidth (&rSrc),RectHeight (&rSrc),
                hRoomDC,rSrc.left,rSrc.top,SRCCOPY);
        }
        EndPaint (hWnd,&ps);
		SelectObject (hRoomDC,hOld);
        bHandled = TRUE;
    }
    return bHandled;
}

/****************************************************************************/

BOOL FAR PASCAL OwnerDrawBitmap (HWND hWnd,LPDRAWITEMSTRUCT lpItem)
//
// ATTENTION:
//
// Owner draw functionality is used here to allow the parent to paint into
// a child's dc before he paints himself. This is done for special controls
// that simulate transparency. This is NOT standard Windows behavior.
//
{
    BOOL bHandled = FALSE;

    HintLine (0);
    if (hRoomBitmap && hRoomDC)
    {
        BITMAP bm;
        RECT rBitmap,rSrc;
		HBITMAP hOld;

		// Only select in hRoomBitmap when needed, It is selected elsewhere.
		hOld = (HBITMAP)SelectObject (hRoomDC,hRoomBitmap);
        rSrc = lpItem->rcItem;
        MapWindowPoints (lpItem->hwndItem,hWnd,(LPPOINT)&rSrc,2);
        GetObject (hRoomBitmap,sizeof(BITMAP),(LPSTR)&bm);
        SetRect (&rBitmap,0,0,bm.bmWidth - 1,bm.bmHeight - 1);
        if (IntersectRect (&rSrc,&rSrc,&rBitmap))
        {
            BitBlt (lpItem->hDC,0,0,RectWidth (&rSrc),RectHeight (&rSrc),
                hRoomDC,rSrc.left,rSrc.top,SRCCOPY);
        }
        bHandled = TRUE;
		SelectObject (hRoomDC,hOld);
    }
    return bHandled;
}



void RepositionDialogControls(HWND hWnd)
{
// The reason for this is that we want to type PIXEL coordinates into the RC
// file.  Windows converts these to pixels, we convert them back to the
// values typed into the RC statement.

// Reposition all the controls in a dialog box to use 1:1 pixel mapping.
// All units specified in the dialog template in the resource file will
// be interpreted as PIXELS not DIALOG UNITS.

	HWND	hChild;
	RECT	rChild;
	DWORD	dwUnits;
	WORD	wBaseWidth;
	WORD	wBaseHeight;

	if (!hWnd) return;
	
	dwUnits = GetDialogBaseUnits();
	wBaseWidth  = LOWORD(dwUnits);
	wBaseHeight = HIWORD(dwUnits);
	
	hChild = GetWindow(hWnd, GW_CHILD);
	while (hChild)
	{
		GetWindowRect(hChild, &rChild);
		ScreenToClient(hWnd, (LPPOINT)&rChild.left);
		ScreenToClient(hWnd, (LPPOINT)&rChild.right);
		SetRect(&rChild,
			(rChild.left   * 4) / wBaseWidth,
			(rChild.top    * 8) / wBaseHeight,
			(rChild.right  * 4) / wBaseWidth,
			(rChild.bottom * 8) / wBaseHeight);
			
		SetWindowPos(hChild,
			NULL,
			rChild.left,
			rChild.top,
			rChild.right  - rChild.left,
			rChild.bottom - rChild.top,
			SWP_NOZORDER);
		
		// Go on to the next child
		hChild = GetWindow(hChild, GW_HWNDNEXT);
	}
}

void RepositionDialogBox(HWND hChild)
{
// The reason for this is that we want to type PIXEL coordinates into the RC
// file.  Windows converts these to pixels, we convert them back to the
// values typed into the RC statement.

// Reposition all the controls in a dialog box to use 1:1 pixel mapping.
// All units specified in the dialog template in the resource file will
// be interpreted as PIXELS not DIALOG UNITS.

	HWND	hParent;
	RECT	rChild;
	DWORD	dwUnits;
	WORD	wBaseWidth;
	WORD	wBaseHeight;

	if (!hChild) return;
	hParent = GetParent(hChild);
	if (!hParent) return;
	
	dwUnits = GetDialogBaseUnits();
	wBaseWidth  = LOWORD(dwUnits);
	wBaseHeight = HIWORD(dwUnits);
	
	GetWindowRect(hChild, &rChild);
	ScreenToClient(hParent, (LPPOINT)&rChild.left);
	ScreenToClient(hParent, (LPPOINT)&rChild.right);
	SetRect(&rChild,
		(rChild.left   * 4) / wBaseWidth,
		(rChild.top    * 8) / wBaseHeight,
		(rChild.right  * 4) / wBaseWidth,
		(rChild.bottom * 8) / wBaseHeight);
		
	SetWindowPos(hChild,
		NULL,
		rChild.left,
		rChild.top,
		rChild.right  - rChild.left,
		rChild.bottom - rChild.top,
		SWP_NOZORDER);
}


/****************************************************************************/

BOOL FAR PASCAL DefRoomProc (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    HWND hCtl;
    int  x,y;
    RECT rApp,rWnd;
    UINT uiFlag;

    switch (wMsg)
    {
    case WM_INITDIALOG:
		RepositionDialogControls(hWnd);
		RepositionDialogBox(hWnd);
		
        DisableSavebits (hWnd);
        LoadRoomBitmap (idCurrentRoom);

        // center dialog:
        //================
        GetClientRect (hWndAstral,&rApp);
        GetWindowRect (hWnd,&rWnd);
        x = (RectWidth (&rApp) - 640) / 2;
        y = (RectHeight (&rApp) - 480) / 2;
        uiFlag = IsWindowVisible (hWnd) ? 0 : SWP_NOREDRAW;
        SetWindowPos (hWnd, 0, x, y, 640, 480,
            uiFlag | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        SendMessage (hWnd,WM_COMMAND,HS_COVER,0L);
        break;
    case WM_CTLCOLOR:
        bHandled = (BOOL) SetControlColors ((HDC)wParam,hWnd,
            (HWND)LOWORD (lParam),HIWORD (lParam));
        break;
    case WM_ERASEBKGND:
        break;
    case WM_PAINT:
        bHandled = PaintRoomBitmap (hWnd);
        break;
    case WM_DRAWITEM:
        bHandled = OwnerDrawBitmap (hWnd,(LPDRAWITEMSTRUCT)lParam);
        break;
    case WM_COMMAND:
        switch (wParam)
        {
		case IDCANCEL:
        case HS_BACKART:
            GoRoom (hInstAstral, RM_ARTROOM, FALSE);
            break;
        case HS_BACKBOOKS:
            GoRoom (hInstAstral, RM_BOOKS, FALSE);
            break;
        case HS_COVER:
            hCtl = GetDlgItem (hWnd,HS_COLORME);
            if (hCtl)
                ShowWindow (hCtl,SW_HIDE);
            hCtl = GetDlgItem (hWnd,HS_SHOWME);
            if (hCtl)
                ShowWindow (hCtl,SW_HIDE);
            break;
        case HS_GALLERY:
            GoRoom (hInstAstral, RM_GALLERY, FALSE);
            break;
        case HS_HORN:
            SoundToggle();
            HintLine (wParam);
            break;
        case HS_TAB1:
        case HS_TAB2:
        case HS_TAB3:
        case HS_TAB4:
        case HS_TAB5:
        case HS_TAB6:
        case HS_TAB7:
        case HS_TAB8:
            hCtl = GetDlgItem (hWnd,HS_COLORME);
            if (hCtl)
                ShowWindow (hCtl,SW_SHOW);
            hCtl = GetDlgItem (hWnd,HS_SHOWME);
            if (hCtl)
                ShowWindow (hCtl,SW_SHOW);
            break;
        }
        break;
#ifndef SHIPBUILD
    case WM_RBUTTONDOWN:
        {
            char szMessage[256];

            if (wParam & (MK_CONTROL | MK_SHIFT))
            {
                wsprintf (szMessage,"x = %u\ny = %u",LOWORD (lParam),HIWORD (lParam));
                MessageBox (hWnd,szMessage,"Yo!",MB_OK);
            }
        }
        break;
#endif
    case WM_CLOSE:
        AstralDlgEnd (hWnd,TRUE|2);
        break;
    default:
        bHandled = FALSE;
        break;
    }
    return bHandled;
}

/****************************************************************************/

BOOL WINPROC EXPORT DlgRoomOpening (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;

    if (wMsg == WM_INITDIALOG)
    {
        DefRoomProc (hWnd,wMsg,wParam,lParam);
    }
    else if (wMsg == WM_CLOSE)
    {
        AstralDlgEnd (hWnd,TRUE|2);
    }
    else if (wMsg == WM_COMMAND && wParam == HS_OPENING)
    {
        GoRoom (hInstAstral, RM_WHOAREYOU, FALSE);
    }
    else
        bHandled = FALSE;
    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}

/****************************************************************************/

BOOL WINPROC EXPORT DlgJokePhone (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    LPSTR lpString;
    static BOOL bPunchLine;
    static HBITMAP hSaveBits;

    if (wMsg == WM_INITDIALOG)
    {
        HWND hCtl = GetDlgItem (hWnd,IDC_STATUSTEXT);
		CenterPopup (hWnd);
        bPunchLine = FALSE;
        if (iJoke > 51000 || !AstralStr (iJoke + 1000,&lpString))
            bPunchLine = TRUE;
        if (AstralStr (iJoke,&lpString))
            SetWindowText (hCtl,lpString);
        if (hSaveBits)
        {
            DeleteObject (hSaveBits);
            hSaveBits = 0;
        }
        SoundStartID (iJoke,FALSE,0);
    }
    else if (wMsg == WM_COMMAND)
    {
        AstralDlgEnd (hWnd,YES|2);
        if (!bPunchLine)
        {
            iJoke += 1000;
            if (AstralStr (iJoke,&lpString))
			{
                AstralDlg (NO, hInstAstral, GetParent (hWnd), IDD_STATUS,
				  (DLGPROC)DlgJokePhone);
			}
        }
        SoundStartResource( "Laugh", NO/*bLoop*/, 0/*hInstance*/ );
    }
    else if (wMsg == WM_ERASEBKGND)
    {
        HBITMAP hBitmap;
        HDC hDC,hMemDC;
        RECT rect,rWnd;

        hDC = GetDC (HWND_DESKTOP);
        GetClientRect (hWnd,&rWnd);
        if (hSaveBits)
        {
            // restore background
            hMemDC = CreateCompatibleDC (hDC);
            hBitmap = (HBITMAP)SelectObject (hMemDC,hSaveBits);
            rect = rWnd;
            MapWindowPoints (hWnd,HWND_DESKTOP,(LPPOINT)&rect,2);
            BitBlt (hDC,rect.left,rect.top,rWnd.right,rWnd.bottom,
                hMemDC,0,0,SRCCOPY);
            SelectObject (hMemDC,hBitmap);
            DeleteDC (hMemDC);
        }
        else
        {
            // save background
            hMemDC = CreateCompatibleDC (hDC);
            hSaveBits = CreateCompatibleBitmap (hDC,rWnd.right,rWnd.bottom);
            hBitmap = (HBITMAP)SelectObject (hMemDC,hSaveBits);
            rect = rWnd;
            MapWindowPoints (hWnd,HWND_DESKTOP,(LPPOINT)&rect,2);
            BitBlt (hMemDC,0,0,rWnd.right,rWnd.bottom,
                hDC,rect.left,rect.top,SRCCOPY);
            SelectObject (hMemDC,hBitmap);
            DeleteDC (hMemDC);
        }
        ReleaseDC (HWND_DESKTOP,hDC);
    }
    else if (wMsg == WM_DESTROY)
    {
        if (hSaveBits)
        {
            DeleteObject (hSaveBits);
            hSaveBits = 0;
        }
        bHandled = FALSE;
    }
    else
        bHandled = FALSE;
    return bHandled;
}

/****************************************************************************/

BOOL WINPROC EXPORT DlgRoomArtRoom (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
	HWND	hCtrl;
    BOOL	bHandled = TRUE;
    LPSTR	lpString;
	char	szBuff[MAX_STR_LEN];
	char	szPath[MAX_FNAME_LEN];

    if (wMsg == WM_COMMAND)
    {
        switch (wParam)
        {
        case HS_BADGE:
            GoRoom (hInstAstral, RM_BADGES, FALSE);
            break;
        case HS_BOOKS:
            GoRoom (hInstAstral, RM_BOOKS, FALSE);
            break;
        case HS_BURGER:
        case HS_PLACEMAT:
            GoRoom (hInstAstral, RM_PLACEMATS, FALSE);
            break;
        case HS_CAKE:
            GoRoom (hInstAstral, RM_DRAGON, FALSE);
            break;
        case HS_CERTIFICATE:
            GoRoom (hInstAstral, RM_CERTIFICATES, FALSE);
            break;
        case HS_CLOWN:
            GoRoom (hInstAstral, RM_CIRCUS, FALSE);
            break;
        case HS_EASEL:
            GoPaintApp (hWnd,NULL);
            break;
        case HS_EXIT:
            SoundStartID (IDM_EXIT,FALSE,0);
            GoRoom (hInstAstral, 0, FALSE);
            PostMessage (hWndAstral,WM_CLOSE,0,0L);
            Delay (1600);
            break;
        case HS_MAZE:
            GoRoom (hInstAstral, RM_MAZES, FALSE);
            break;
        case HS_MONKEY:
            GoRoom (hInstAstral, RM_JUNGLE, FALSE);
            break;
        case HS_MONSTER:
            GoRoom (hInstAstral, RM_MONSTER, FALSE);
            break;
        case HS_PAPER:
            GoRoom (hInstAstral, RM_PAPER, FALSE);
            break;
        case HS_PUZZLE:
            GoRoom (hInstAstral, RM_PUZZLESTART, FALSE);
            break;
        case HS_ROCKET:
            break;
        case HS_SHAPES:
            GoRoom (hInstAstral, RM_SHAPES, FALSE);
            break;
        case HS_SLIDESHOW:
            GoRoom (hInstAstral, RM_SLIDE, FALSE);
            break;
        case HS_SPACESHIP:
            GoRoom (hInstAstral, RM_SPACEBALL, FALSE);
            break;
        case HS_TOASTER:
            GoRoom (hInstAstral, RM_WRONG, FALSE);
            break;
        case HS_VEGGIES:
            GoRoom (hInstAstral, RM_SUBSETS, FALSE);
            break;
        case IDC_CAT_SCARED:
        case IDC_CAT_WALK:
			hCtrl = GetDlgItem(hWnd, (wParam == IDC_CAT_WALK) ? IDC_CAT_WALK : IDC_CAT_SCARED);
			if (hCtrl) 
			{
				// EnableWindow must be done to avoid menu problem.
				EnableWindow(hCtrl, FALSE);
				ShowWindow(hCtrl, SW_HIDE);
			}
			hCtrl = GetDlgItem(hWnd, (wParam == IDC_CAT_WALK) ? IDC_CAT_SCARED : IDC_CAT_WALK);
			if (hCtrl) 
			{
				EnableWindow(hCtrl, TRUE);
				ShowWindow(hCtrl, SW_SHOW);
			}
            break;
        case HS_TOUR:
            Control.Hints = !Control.Hints;
            PutDefInt (Control.Hints,Control.Hints);
            HintLine (wParam);
            break;
        case HS_JOKEPHONE:
//			SoundStartResource( "Phone", NO/*bLoop*/, 0/*hInstance*/ );
			if ( !iJoke )
	            srand((unsigned)time(NULL));
			iJoke = 50001 + (rand() % 124);
            if (AstralStr (iJoke,&lpString))
                AstralDlg( NO|2, hInstAstral, hWnd, IDD_STATUS, (DLGPROC)DlgJokePhone );
            break;

		case IDC_ADDONROOM:
            GoRoom (hInstAstral, RM_ADDON, FALSE);
			bHandled = TRUE;
			break;

        default:
            bHandled = FALSE;
        }
    }
    else if (wMsg == WM_INITDIALOG)
    {
		hCtrl = GetDlgItem(hWnd, IDC_CAT_SCARED);
		if (hCtrl)
			ShowWindow(hCtrl, SW_HIDE);
		bHandled = FALSE;

		// see if any Add-ons exist
		memset (szBuff, 0, MAX_STR_LEN);
		memset (szPath, 0, MAX_FNAME_LEN);
//		lstrcpy (szPath, Control.ProgHome);
//#ifdef STUDIO
//		lstrcat (szPath, "files\\studio.ini");
//#else
//		lstrcat (szPath, "files\\adventur.ini");
//#endif
//[AddOns] section moved to crayola.ini

		GetPrivateProfileString ("AddOns", NULL, "", szBuff, MAX_STR_LEN,
		  PRIVATE_INI);
		if (szBuff[0] != 0)
		{
			// enable the Add-On Room button
    		ShowWindow (GetDlgItem(hWnd, IDC_ADDONROOM), SW_SHOW);
		}
		else
		{
			// disable the Add-On Room button
    		ShowWindow (GetDlgItem(hWnd, IDC_ADDONROOM), SW_HIDE);
		}
	}
    else
        bHandled = FALSE;
    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}

/****************************************************************************/

BOOL WINPROC EXPORT DlgRoomBadges (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    char szFile[MAX_PATH];
    char szTemp[MAX_PATH];

    if (wMsg == WM_COMMAND && wParam >= HS_BADGE1 && wParam <= HS_BADGE12)
    {
        int i = wParam - HS_BADGE1 + 1;

        wsprintf (szTemp,GetString (IDS_BADGE,NULL),i);
        PathCat (szFile,Control.PouchPath,szTemp);
        GoPaintApp (hWnd,szFile);
    }
    else
        bHandled = FALSE;
    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}

/****************************************************************************/

BOOL WINPROC EXPORT DlgRoomBooks (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;

    if (wMsg == WM_COMMAND)
    {
        switch (wParam)
        {
        case HS_BOOK1:
            GoRoom (hInstAstral, RM_EUROPE, FALSE);
            break;
        case HS_BOOK2:
            GoRoom (hInstAstral, RM_FASHION, FALSE);
            break;
        case HS_BOOK3:
            GoRoom (hInstAstral, RM_BOARDWALK, FALSE);
            break;
        case HS_BOOK4:
            GoRoom (hInstAstral, RM_JOKES, FALSE);
            break;
        case HS_BOOK5:
            GoRoom (hInstAstral, RM_MAILMAZE, FALSE);
            break;
        case HS_BOOK6:
            GoRoom (hInstAstral, RM_SAFARI, FALSE);
            break;
        case HS_BOOK7:
            GoRoom (hInstAstral, RM_VEHICLE, FALSE);
            break;
        case HS_BOOK8:
            GoRoom (hInstAstral, RM_WORLD, FALSE);
            break;
        default:
            bHandled = FALSE;
        }
    }
    else
        bHandled = FALSE;
    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}

/****************************************************************************/

BOOL WINPROC EXPORT DlgRoomCertificates (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    char szFile[MAX_PATH];
    char szTemp[MAX_PATH];

    if (wMsg == WM_COMMAND && wParam >= HS_CERTIFICATE1 && wParam <= HS_CERTIFICATE8)
    {
        int i = wParam - HS_CERTIFICATE1 + 1;

        wsprintf (szTemp,GetString (IDS_CERTIFICATE,NULL),i);
        PathCat (szFile,Control.PouchPath,szTemp);
        GoPaintApp (hWnd,szFile);
    }
    else
        bHandled = FALSE;
    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}

/****************************************************************************/

BOOL WINPROC EXPORT DlgRoomPaper (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    char szFile[MAX_PATH];
    char szTemp[MAX_PATH];

    if (wMsg == WM_COMMAND && wParam >= HS_PAPER1 && wParam <= HS_PAPER10)
    {
        int i = wParam - HS_PAPER1 + 1;

        wsprintf (szTemp,GetString (IDS_PAPER,NULL),i);
        PathCat (szFile,Control.PouchPath,szTemp);
        GoPaintApp (hWnd,szFile);
    }
    else
        bHandled = FALSE;
    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}

/****************************************************************************/

BOOL WINPROC EXPORT DlgRoomPlacemats (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    char szFile[MAX_PATH];
    char szTemp[MAX_PATH];

    if (wMsg == WM_COMMAND && wParam >= HS_PLACEMAT1 && wParam <= HS_PLACEMAT8)
    {
        int i = wParam - HS_PLACEMAT1 + 1;

        if (CheckRegistration (TRUE))
        {        
            wsprintf (szTemp,GetString (IDS_PLACEMAT,NULL),i);
            PathCat (szFile,Control.PouchPath,szTemp);
            GoPaintApp (hWnd,szFile);
        }
    }
    else
        bHandled = FALSE;
    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}

/****************************************************************************/

BOOL WINPROC EXPORT DlgRoomPreview (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    char szTemp[MAX_PATH];
    HWND hCtl;
    LPOBJECT lpObject;

    switch (wMsg)
    {
    case WM_COMMAND:
        switch (wParam)
        {
        case HS_COVER:
			ShowCoverAnimatedButtons(hWnd, TRUE);
            hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
            ShowWindow (hCtl,SW_HIDE);
            hCtl = GetDlgItem (hWnd,
                idCurrentRoom == RM_EUROPE  ? HS_LOGO_EUROPE  :
                idCurrentRoom == RM_JUNGLE  ? HS_LOGO_JUNGLE  :
                idCurrentRoom == RM_SAFARI  ? HS_LOGO_SAFARI  :
                idCurrentRoom == RM_CIRCUS  ? HS_LOGO_CIRCUS  :
				0);
            if (hCtl)
                ShowWindow (hCtl,SW_SHOW);
            hCtl = GetDlgItem (hWnd,
                idCurrentRoom == RM_EUROPE  ? HS_XTRA_EUROPE  :
                idCurrentRoom == RM_JUNGLE  ? HS_XTRA_JUNGLE  :
                idCurrentRoom == RM_SAFARI  ? HS_XTRA_SAFARI  :
                idCurrentRoom == RM_CIRCUS  ? HS_XTRA_CIRCUS  :
                0);
            if (hCtl)
                ShowWindow (hCtl,SW_HIDE);
            bHandled = FALSE;
            break;
        case HS_TAB1:
        case HS_TAB2:
        case HS_TAB3:
        case HS_TAB4:
        case HS_TAB5:
        case HS_TAB6:
        case HS_TAB7:
        case HS_TAB8:
			ShowCoverAnimatedButtons(hWnd, FALSE);
            wsprintf (szTemp,GetString (
                idCurrentRoom == RM_EUROPE  ? IDS_EUROPE  :
                idCurrentRoom == RM_JUNGLE  ? IDS_JUNGLE  :
                idCurrentRoom == RM_SAFARI  ? IDS_SAFARI  :
                idCurrentRoom == RM_CIRCUS  ? IDS_CIRCUS  :
                0,NULL),wParam - HS_TAB1 + 1);
            PathCat (szPreview,Control.PouchPath,szTemp);
            AstralImageLoad (IDN_ART,szPreview,MAYBE,NO);
            if (lpImage && (lpObject = ImgGetBase (lpImage)))
            {
                hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
                SetWindowLong (hCtl,GWL_IMAGE,(long)lpObject);
		        SetWindowLong (hCtl,GWL_IMAGE_TOOLPROC,(long)AnimateProc);
                ShowWindow (hCtl,SW_SHOW);
                InvalidateRect (hCtl,NULL,TRUE);
                hCtl = GetDlgItem (hWnd,
                    idCurrentRoom == RM_EUROPE  ? HS_LOGO_EUROPE  :
                    idCurrentRoom == RM_JUNGLE  ? HS_LOGO_JUNGLE  :
                    idCurrentRoom == RM_SAFARI  ? HS_LOGO_SAFARI  :
                    idCurrentRoom == RM_CIRCUS  ? HS_LOGO_CIRCUS  :
                    0);
                if (hCtl)
                    ShowWindow (hCtl,SW_HIDE);
                hCtl = GetDlgItem (hWnd,
                    idCurrentRoom == RM_EUROPE  ? HS_XTRA_EUROPE  :
                    idCurrentRoom == RM_JUNGLE  ? HS_XTRA_JUNGLE  :
                    idCurrentRoom == RM_SAFARI  ? HS_XTRA_SAFARI  :
                    idCurrentRoom == RM_CIRCUS  ? HS_XTRA_CIRCUS  :
                    0);
                if (hCtl)
                    ShowWindow (hCtl,SW_SHOW);
                bHandled = FALSE;
            }
            break;
        case HS_COLORME:
            GoPaintApp (hWnd,szPreview);
            break;
        default:
            bHandled = FALSE;
        }
        break;
    default:
        bHandled = FALSE;
    }
    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}

/****************************************************************************/


static DLGPROC RoomProc[] = {
    (DLGPROC)DlgRoomOpening      ,           // RM_OPENING
    (DLGPROC)DlgRoomWhoAreYou    ,           // RM_WHOAREYOU
    (DLGPROC)DlgRoomArtRoom      ,           // RM_ARTROOM
    (DLGPROC)DlgRoomBadges       ,           // RM_BADGES
    (DLGPROC)DlgRoomHidden       ,           // RM_BOARDWALK
    (DLGPROC)DlgRoomBooks        ,           // RM_BOOKS
    (DLGPROC)DlgRoomCertificates ,           // RM_CERTIFICATES
    (DLGPROC)DlgRoomDot2Dot      ,           // RM_CIRCUS
    (DLGPROC)DlgRoomHidden       ,           // RM_DRAGON
    (DLGPROC)DlgRoomSpotDot      ,           // RM_EUROPE
    (DLGPROC)DlgRoomMixup        ,           // RM_FASHION
    (DLGPROC)DlgRoomGallery      ,           // RM_GALLERY
    (DLGPROC)DlgRoomJokes        ,           // RM_JOKES
    (DLGPROC)DlgRoomPreview      ,           // RM_JUNGLE
    (DLGPROC)DlgRoomMazes        ,           // RM_MAILMAZE
    (DLGPROC)DlgRoomMazes        ,           // RM_MAZES
    (DLGPROC)DlgRoomMixup        ,           // RM_MONSTER
    (DLGPROC)DlgRoomPaper        ,           // RM_PAPER
    (DLGPROC)DlgRoomPlacemats    ,           // RM_PLACEMATS
    (DLGPROC)DlgRoomPuzzle       ,           // RM_PUZZLE
    (DLGPROC)DlgRoomPreview      ,           // RM_SAFARI
    (DLGPROC)DlgRoomShapes       ,           // RM_SHAPES
    (DLGPROC)DlgRoomSpotDot      ,           // RM_SPACEBALL
    (DLGPROC)DlgRoomSubsets      ,           // RM_SUBSETS
    (DLGPROC)DlgRoomMixup        ,           // RM_VEHICLE
    (DLGPROC)DlgRoomWorld        ,           // RM_WORLD
    (DLGPROC)DlgRoomPuzzleStart  ,           // RM_PUZZLESTART
    (DLGPROC)DlgRoomSlideshow    ,           // RM_SLIDE
    (DLGPROC)DlgRoomSlidePlay    ,           // RM_SLIDEPLAY
    (DLGPROC)DlgRoomWrong        ,           // RM_WRONG
    (DLGPROC)DlgRoomAddOn                    // RM_ADDON
};

/****************************************************************************/


void FAR PASCAL EXPORT GoRoom (HINSTANCE hInst, ITEMID idNewRoom,
  BOOL bDLLRoom)
{
	static	DLGPROC	DlgAddonProc = NULL;

    if (idCurrentRoom && !IntermissionGetIsRunning())
        IntermissionStart(TRUE);        // start a small animation while changing rooms

    idPreviousRoom = idCurrentRoom;
    if (idNewRoom == -1)
        idNewRoom = idCurrentRoom ? idCurrentRoom :
//#ifdef CDBUILD	
//		  (bFunkyOpening ? RM_WHOAREYOU : RM_OPENING);
//#else
		  (Control.bNoTheme ? RM_WHOAREYOU : RM_OPENING);
//#endif
    if (idCurrentRoom)
    {
        HWND hDlg = AstralDlgGet (idCurrentRoom);

        if (hDlg)
            PostMessage (hDlg, WM_CLOSE, 0, 0L);
        idCurrentRoom = 0;
    }
	// WARNING ! Only the addon.c module can call GoRoom with bDLLRoom true
	// since it has to find and load the library first.
	if (bDLLRoom)
	{
		if (DlgAddonProc != NULL)
			FreeProcInstance ((FARPROC)DlgAddonProc);

        idCurrentRoom = idNewRoom;
		bPaintAppActive = FALSE;	// set FALSE before calling DLL dialog
		if ( (DlgAddonProc =
		  (DLGPROC)GetProcAddress (hInst, MAKEINTRESOURCE(2) )) )
		{
			AstralDlg (YES|2, hInst, hWndAstral, idNewRoom, DlgAddonProc);
		}
	}
    else if (idNewRoom >= RM_FIRST && idNewRoom <= RM_LAST)
    {
        int i = (int)idNewRoom - RM_FIRST;
        idCurrentRoom = idNewRoom;

   	    AstralDlg (YES|2, hInst, hWndAstral, idCurrentRoom, RoomProc[i]);
		bPaintAppActive = FALSE;
    }
}

/****************************************************************************/

