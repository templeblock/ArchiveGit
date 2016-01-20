/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                   rhidden.c

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

extern HINSTANCE hInstAstral;
extern HWND   hWndAstral;

/******************************** Private Data ******************************/

static LPOBJECT lpLegend;

/****************************** Private Functions ***************************/

void NEAR PASCAL RotateLegend (HWND hWnd,BOOL bIncludeVisibleObjects)
{
    HWND hCtl;
    LPOBJECT lpBase;
    LPOBJECT lpObject;
    LPOBJECT lpStart;

	if ( !lpImage )
		return;
    if ( !(lpBase = ImgGetBase (lpImage)) )
		return;
    if ( !(lpObject = lpStart = lpLegend ? lpLegend : lpBase) )
		return;
    if ((hCtl = GetDlgItem (hWnd,HS_XTRA_BOARDWALK)) ||
        (hCtl = GetDlgItem (hWnd,HS_XTRA_DRAGON)))
        InvalidateRect (hCtl,NULL,FALSE);
    hCtl = GetDlgItem (hWnd,IDC_HIDDEN_LEGEND);
    lpLegend = NULL;
    while (lpObject = ImgGetNextObject (lpImage,lpObject,YES,YES))
    {
        if (lpObject == lpStart)
            break;
        if (lpObject == lpBase)
            continue;
		if (lpObject->ObjectDataID == OBJECT_DATA_STOCK_ANIMATION)
			continue;
        if (bIncludeVisibleObjects || lpObject->fHidden)
        {
            lpLegend = lpObject;
            break;
        }
    }
    if (lpLegend)
    {
        LPFRAME lpFrame;

        if ( !(lpFrame = PixmapFrame (&lpLegend->Pixmap,PMT_EDIT)) )
			return;
        SetWindowLong (hCtl,GWL_IMAGE,(long)lpFrame);
        ShowWindow (hCtl,SW_SHOW);
        InvalidateRect (hCtl,NULL,TRUE);
    }
    else
    {
		SoundStartResource("GOODANSWER3", FALSE, 0);
        SetWindowLong (hCtl,GWL_IMAGE,0L);
        ShowWindow (hCtl,SW_HIDE);
        
        hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
		StartAnimation(hCtl);
    }
}

/****************************************************************************/

BOOL FAR HiddenToolProc (HWND hWnd,LONG lParam,WORD wMsg)
{
    static BOOL bTrack = FALSE;

    if (wMsg == WM_LBUTTONDOWN && !bTrack)
    {
        LPOBJECT lpObject;

        if (lpObject = ImgFindObject (lpImage,LOWORD(lParam),HIWORD(lParam),
            TRUE,TRUE))
        {
            RECT rect;

			SoundStartResource("GOODANSWER2", FALSE, 0);
            lpObject->fHidden = FALSE;
            File2DispRect (&lpObject->rObject,&rect);
            InvalidateRect (hWnd,AstralToWindowsRect (&rect),FALSE);
            RotateLegend (GetParent (hWnd),FALSE);
        }
		else
			SoundStartResource("WRONGANSWER3", FALSE, 0);
		AnimateProc(hWnd,lParam,wMsg);	
        bTrack = TRUE;
    }
    else if (wMsg == WM_LBUTTONUP && bTrack)
        bTrack = FALSE;
    return TRUE;
}

/****************************** Public Functions ****************************/

BOOL WINPROC EXPORT DlgRoomHidden (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    char szTemp[MAX_PATH];
    HWND hCtl;
    LPFRAME lpFrame;
    LPOBJECT lpBase;
    LPOBJECT lpObject;

    if (wMsg == WM_COMMAND)
    {
        switch (wParam)
        {
        case HS_COVER:
			StopAnimation();
            hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
            ShowWindow (hCtl,SW_HIDE);
            hCtl = GetDlgItem (hWnd,IDC_HIDDEN_LEGEND);
            ShowWindow (hCtl,SW_HIDE);
            if (idCurrentRoom == RM_BOARDWALK)
            {
                hCtl = GetDlgItem (hWnd,HS_LOGO_BOARDWALK);
                ShowWindow (hCtl,SW_SHOW);
                hCtl = GetDlgItem (hWnd,HS_XTRA_BOARDWALK);
                ShowWindow (hCtl,SW_HIDE);
            }
            else if (idCurrentRoom == RM_DRAGON)
            {
                hCtl = GetDlgItem (hWnd,HS_LOGO_DRAGON);
                ShowWindow (hCtl,SW_SHOW);
                hCtl = GetDlgItem (hWnd,HS_XTRA_DRAGON);
                ShowWindow (hCtl,SW_HIDE);
            }
			ShowCoverAnimatedButtons (hWnd, YES);
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
            lpLegend = NULL;
            if (idCurrentRoom == RM_BOARDWALK)
                wsprintf (szTemp,GetString (IDS_BOARDWALK,NULL),wParam - HS_TAB1 + 1);
            else if (idCurrentRoom == RM_DRAGON)
			{
                wsprintf (szTemp,GetString (IDS_DRAGON,NULL),wParam - HS_TAB1 + 1);
			}
			ShowCoverAnimatedButtons (hWnd, NO);
            PathCat (szPreview,Control.PouchPath,szTemp);
            AstralImageLoad (IDN_ART,szPreview,MAYBE,NO);
            if (lpImage)
            {
                lpObject = lpBase = ImgGetBase (lpImage);
                while (lpObject = ImgGetNextObject (lpImage,lpObject,YES,NO))
                {
                    if (!lpLegend)
                        lpLegend = lpObject;
                    lpObject->fHidden = TRUE;
                }
                hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
                SetWindowLong (hCtl,GWL_IMAGE,(long)lpBase);
                SetWindowLong (hCtl,GWL_IMAGE_TOOLPROC,(long)HiddenToolProc);
                ShowWindow (hCtl,SW_SHOW);
                InvalidateRect (hCtl,NULL,TRUE);
                if (lpLegend)
                {
                    hCtl = GetDlgItem (hWnd,IDC_HIDDEN_LEGEND);
                    lpFrame = PixmapFrame (&lpLegend->Pixmap,PMT_EDIT);
                    SetWindowLong (hCtl,GWL_IMAGE,(long)lpFrame);
                    ShowWindow (hCtl,SW_SHOW);
                    InvalidateRect (hCtl,NULL,TRUE);
                }
                if (idCurrentRoom == RM_BOARDWALK)
                {
                    hCtl = GetDlgItem (hWnd,HS_LOGO_BOARDWALK);
                    ShowWindow (hCtl,SW_HIDE);
                    hCtl = GetDlgItem (hWnd,HS_XTRA_BOARDWALK);
                    ShowWindow (hCtl,SW_SHOW);
                    InvalidateRect (hCtl,NULL,TRUE);
                }
                else if (idCurrentRoom == RM_DRAGON)
                {
                    hCtl = GetDlgItem (hWnd,HS_LOGO_DRAGON);
                    ShowWindow (hCtl,SW_HIDE);
                    hCtl = GetDlgItem (hWnd,HS_XTRA_DRAGON);
                    ShowWindow (hCtl,SW_SHOW);
                    InvalidateRect (hCtl,NULL,TRUE);
                }
                bHandled = FALSE;
            }
#ifdef STUDIO			
			SoundStartID(HS_XTRA_BOARDWALK, FALSE, 0);
#else			
			SoundStartID(HS_XTRA_DRAGON, FALSE, 0);
#endif			
            break;
        case HS_COLORME:
			StopAnimation();
            ImgCombineObjects (lpImage,YES,NO,YES);
            GoPaintApp (hWnd,szPreview);
            break;
        case HS_SHOWME:
            if (lpImage)
            {
                lpObject = ImgGetBase (lpImage);
                while (lpObject = ImgGetNextObject (lpImage,lpObject,YES,NO))
                    lpObject->fHidden = FALSE;
                hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
                ShowWindow (hCtl,SW_SHOW);
                InvalidateRect (hCtl,NULL,TRUE);
				StartAnimation(hCtl);
            }
            lpLegend = NULL;
            break;
        case HS_XTRA_BOARDWALK:
        case HS_XTRA_DRAGON:
            if (lpImage)
                RotateLegend (hWnd,TRUE);
            break;
        default:
            bHandled = FALSE;
        }
    }
	else if (wMsg == WM_DESTROY)
	{
		StopAnimation();
		bHandled = FALSE;
	}
    else
        bHandled = FALSE;
    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}

/****************************************************************************/

