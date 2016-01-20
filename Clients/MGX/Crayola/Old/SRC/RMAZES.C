/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                   rmazes.c

******************************************************************************
******************************************************************************

  Description:  Room dialog functions.
 Date Created:  01/11/94
       Author:  Michael P. Dagate

*****************************************************************************/

#include <windows.h>
#include <math.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern BOOL   fAppActive;
extern HINSTANCE hInstAstral;
extern HWND   hWndAstral;

/******************************** Private Data ******************************/

// maximum line length possible in a 480x360 image plus a little
#define MAX_LINEPOINTS 640

static BOOL  bShowSolution;
static BOOL  bSolved;
static BOOL  bTrack;
static HPEN  hMazePen;
static int   nNumLinePoints;
static int   nNumTrailPoints;
static POINT MazeLinePoints[MAX_LINEPOINTS];
static POINT MazeTrailPoints[MAX_LINEPOINTS];

/****************************** Private Functions ***************************/

static void FAR MazeLineProc (int x,int y)
{
    if (nNumLinePoints < MAX_LINEPOINTS)
    {
        MazeLinePoints[nNumLinePoints].x = x;
        MazeLinePoints[nNumLinePoints].y = y;
        nNumLinePoints++;
    }
}

/****************************************************************************/

static void NEAR InvertLine (HWND hWnd,int x1,int y1,int x2,int y2)
{
    static BOOL bInverted = FALSE;
    static int  px1,py1,px2,py2;
    HDC hDC;

    hDC = GetDC (hWnd);
    if (bInverted)
    {
        bInverted = FALSE;
        if (x1 == px1 && y1 == py1)             // are start points equal?
            DrawLine (hDC,px1,py1,px2,py2,      // reverse previous line
                hMazePen ? hMazePen : (HPEN)GetStockObject (BLACK_PEN));
    }
    DrawLine (hDC,x1,y1,x2,y2,
        hMazePen ? hMazePen : (HPEN)GetStockObject (BLACK_PEN));
    ReleaseDC (hWnd,hDC);
    px1 = x1;   // save for future reference
    py1 = y1;
    px2 = x2;
    py2 = y2;
    bInverted = TRUE;
}

/****************************************************************************/

static BOOL NEAR PathIsClear (int x1,int y1,int x2,int y2)
{
    BOOL bClear = TRUE;
    int  i;
    LPFRAME lpFrame = ImgGetBaseEditFrame (lpImage);

    nNumLinePoints = 0;
    ddaline (x1,y1,x2,y2, (LPPOINTFUNC)MazeLineProc);
    for (i = 0; i < nNumLinePoints; i++)
    {
        RGBS Pixel;
        LPTR lpLine = FramePointer (lpFrame,
            MazeLinePoints[i].x,MazeLinePoints[i].y,FALSE);

        FrameGetRGB (lpFrame,lpLine,&Pixel,1);
        if (!Pixel.red && !Pixel.green && !Pixel.blue)
        {
            bClear = FALSE;
            break;
        }
    }
    return bClear;
}

/****************************************************************************/

static void MazePixelProc (LPTR lpSrc, LPTR lpDst, int dx, int iDepth)
{
    BYTE r,g,b;

    if (!iDepth)
        iDepth = 1;
    if (iDepth == 3 && !bShowSolution)
    {
        while (dx--)
        {
        	r = *lpSrc++;
        	g = *lpSrc++;
        	b = *lpSrc++;
            if (!(r & g) && b == 255)
                r = g = 255;
            *lpDst++ = r;
            *lpDst++ = g;
            *lpDst++ = b;
        }
    }
    else
        copy (lpSrc,lpDst,dx * iDepth);
}

/****************************************************************************/

static BOOL FAR MazeToolProc (HWND hWnd,LONG lParam,WORD wMsg)
{
    HDC     hDC;
    LPFRAME lpFrame;
    LPTR    lpLine;
    POINT   pt;
    RGBS    Pixel;
    static int x1,y1;

    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);
    switch (wMsg)
    {
    case WM_LBUTTONDOWN:
        if (!bTrack)
        {
            lpFrame = ImgGetBaseEditFrame (lpImage);
            lpLine = FramePointer (lpFrame,pt.x,pt.y,FALSE);
            FrameGetRGB (lpFrame,lpLine,&Pixel,1);
            if (Pixel.green && !(Pixel.red & Pixel.blue)) // start green pel
            {
//                SoundStartID (IDC_LINES,NO,0);
                SoundStartResource ("icons",NO,0);
                MazeTrailPoints[0].x = x1 = pt.x;
                MazeTrailPoints[0].y = y1 = pt.y;
                nNumTrailPoints = 1;
                if (bShowSolution || bSolved)
                {
                    bShowSolution = bSolved = FALSE;
                    InvalidateRect (hWnd,NULL,FALSE);
                    UpdateWindow (hWnd);
                }
                bTrack = TRUE;
            }
            else if (!AnimateProc(hWnd,lParam,wMsg))
                SoundStartResource ("WRONGANSWER1",NO,0);
        }
        break;
    case WM_MOUSEMOVE:
        if (!bTrack)
            break;
        if (!fAppActive || GetUpdateRect (hWnd,NULL,FALSE))
        {
            InvertLine (hWnd,x1,y1,x1,y1);
            break;
        }
        InvertLine (hWnd,x1,y1,pt.x,pt.y);
        if (!LBUTTON || (pt.x == x1 && pt.y == y1))
            break;
        // else button is down, so fall through to anchor a point
    case WM_LBUTTONUP:
        if (bTrack)
        {
            if (PathIsClear (x1,y1,pt.x,pt.y))
            {
                lpFrame = ImgGetBaseEditFrame (lpImage);
                lpLine = FramePointer (lpFrame,pt.x,pt.y,FALSE);
                FrameGetRGB (lpFrame,lpLine,&Pixel,1);
                if (Pixel.red && !(Pixel.green & Pixel.blue)) // end red pel
                {
                    SoundStartResource ("GOODANSWER1",NO,0);
					StartAnimation(hWnd);
                    bSolved = TRUE;
                    bTrack = FALSE;
                }
                else
                {
                    hDC = GetDC (hWnd);
                    DrawLineEx (hDC,x1,y1,pt.x,pt.y,
                        hMazePen ? hMazePen : (HPEN)GetStockObject (BLACK_PEN),FALSE);
                    ReleaseDC (hWnd,hDC);
                    x1 = pt.x;
                    y1 = pt.y;
                    if (nNumTrailPoints < MAX_LINEPOINTS)
                    {
                        MazeTrailPoints[nNumTrailPoints].x = x1;
                        MazeTrailPoints[nNumTrailPoints].y = y1;
                        nNumTrailPoints++;
                    }
                }
            }
            else
                SoundStartResource ("WRONGANSWER1",NO,0);
        }
        break;
    case WM_PAINT:
        if (bTrack)
        {
            int i;

            hDC = GetDC (hWnd);
            pt.x = MazeTrailPoints[0].x;
            pt.y = MazeTrailPoints[0].y;
            for (i = 1; i < nNumTrailPoints; i++)
            {
                DrawLineEx (hDC,pt.x,pt.y,
                    MazeTrailPoints[i].x,MazeTrailPoints[i].y,
                    hMazePen ? hMazePen : (HPEN)GetStockObject (BLACK_PEN),FALSE);
                pt.x = MazeTrailPoints[i].x;
                pt.y = MazeTrailPoints[i].y;
            }
            ReleaseDC (hWnd,hDC);
        }
        break;
    }
    return TRUE;
}

/****************************** Public Functions ****************************/

BOOL WINPROC EXPORT DlgRoomMazes (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    char szTemp[MAX_PATH];
    HWND hCtl;

    switch (wMsg)
    {
    case WM_COMMAND:
        switch (wParam)
        {
        case HS_COVER:
			StopAnimation();
			ShowCoverAnimatedButtons(hWnd, TRUE);
            hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
            ShowWindow (hCtl,SW_HIDE);
            if (idCurrentRoom == RM_MAILMAZE)
            {
                hCtl = GetDlgItem (hWnd,HS_LOGO_MAILMAZE);
                ShowWindow (hCtl,SW_SHOW);
                hCtl = GetDlgItem (hWnd,HS_XTRA_MAILMAZE);
                ShowWindow (hCtl,SW_HIDE);
                hCtl = GetDlgItem (hWnd,HS_RESTART);
                ShowWindow (hCtl,SW_HIDE);
            }
            else if (idCurrentRoom == RM_MAZES)
            {
                hCtl = GetDlgItem (hWnd,HS_LOGO_MAZES);
                ShowWindow (hCtl,SW_SHOW);
                hCtl = GetDlgItem (hWnd,HS_XTRA_MAZES);
                ShowWindow (hCtl,SW_HIDE);
                hCtl = GetDlgItem (hWnd,HS_RESTART);
                ShowWindow (hCtl,SW_HIDE);
            }
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
			StopAnimation();
			ShowCoverAnimatedButtons(hWnd, FALSE);
            bTrack = FALSE;
            if (idCurrentRoom == RM_MAILMAZE)
                wsprintf (szTemp,GetString (IDS_MAILMAZE,NULL),wParam - HS_TAB1 + 1);
            else if (idCurrentRoom == RM_MAZES)
                wsprintf (szTemp,GetString (IDS_MAZES,NULL),wParam - HS_TAB1 + 1);
            PathCat (szPreview,Control.PouchPath,szTemp);
            AstralImageLoad (IDN_ART,szPreview,MAYBE,NO);
            if (lpImage)
            {
                bShowSolution = bSolved = FALSE;
                ImgSetPixelProc (MazePixelProc);
                hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
                SetWindowLong (hCtl,GWL_IMAGE,(long) ImgGetBase (lpImage));
                SetWindowLong (hCtl,GWL_IMAGE_TOOLPROC,(long)MazeToolProc);
                ShowWindow (hCtl,SW_SHOW);
                InvalidateRect (hCtl,NULL,FALSE);
                if (idCurrentRoom == RM_MAILMAZE)
                {
                    hCtl = GetDlgItem (hWnd,HS_LOGO_MAILMAZE);
                    ShowWindow (hCtl,SW_HIDE);
                    hCtl = GetDlgItem (hWnd,HS_XTRA_MAILMAZE);
                    ShowWindow (hCtl,SW_SHOW);
                    hCtl = GetDlgItem (hWnd,HS_RESTART);
                    ShowWindow (hCtl,SW_SHOW);
                }
                else if (idCurrentRoom == RM_MAZES)
                {
                    hCtl = GetDlgItem (hWnd,HS_LOGO_MAZES);
                    ShowWindow (hCtl,SW_HIDE);
                    hCtl = GetDlgItem (hWnd,HS_XTRA_MAZES);
                    ShowWindow (hCtl,SW_SHOW);
                    hCtl = GetDlgItem (hWnd,HS_RESTART);
                    ShowWindow (hCtl,SW_SHOW);
                }
                bHandled = FALSE;
            }
#ifdef STUDIO			
				SoundStartID(HS_XTRA_MAILMAZE, FALSE, 0);
#else
				SoundStartID(HS_XTRA_MAZES, FALSE, 0);
#endif			
            break;
        case HS_COLORME:
			StopAnimation();
            GoPaintApp (hWnd,szPreview);
            break;
        case HS_SHOWME:
        case HS_RESTART:
            bTrack = FALSE;
            bShowSolution = (wParam == HS_SHOWME);
            hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
			StopAnimation();
            InvalidateRect (hCtl,NULL,FALSE);
            UpdateWindow (hCtl);
			if (wParam == HS_SHOWME)
				StartAnimation(hCtl);
            break;
        default:
            bHandled = FALSE;
        }
        break;
    case WM_INITDIALOG:
        hMazePen = CreatePen (PS_SOLID,3,RGB (0,0,255));
        bHandled = FALSE;
        break;
    case WM_DESTROY:
		StopAnimation();
        ImgSetPixelProc (NULL);
        if (hMazePen)
        {
            DeleteObject (hMazePen);
            hMazePen = 0;
        }
        bHandled = FALSE;
        break;
    default:
        bHandled = FALSE;
    }
    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}

/****************************************************************************/

