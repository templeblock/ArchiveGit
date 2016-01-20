/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                   rworld.c

******************************************************************************
******************************************************************************

  Description:  Room dialog functions.
 Date Created:  03/08/94
       Author:  Michael P. Dagate

*****************************************************************************/

#include <windows.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern HINSTANCE hInstAstral;
extern HWND   hWndAstral;

/******************************** Private Data ******************************/

#define MAX_ANSWERS 4

static LPFRAME  lpCorrectAnswer;
static LPOBJECT lpAnswer[MAX_ANSWERS];
static RECT     rAnswer[MAX_ANSWERS];

/****************************** Private Functions ***************************/

static void NEAR PASCAL Randomizer (int nIndex,LPOBJECT lpObject)
{
    BOOL bRandomized = FALSE;
    int  i;

    // seed the randomizer
    if (!nIndex)
    {
        srand ((unsigned)time(NULL));
        lpCorrectAnswer = PixmapFrame (&lpObject->Pixmap,PMT_EDIT);
    }

    // try to select a random offset into answer table
    for (i = 0; i < 100; i++)
    {
        nIndex = (rand() % MAX_ANSWERS);
        if (!lpAnswer[nIndex])
        {
            lpAnswer[nIndex] = lpObject;
            bRandomized = TRUE;
            break;
        }
    }

    // if randomizer failed, try a sequential selection
    if (!bRandomized)
    {
        for (i = 0; i < MAX_ANSWERS; i++)
        {
            if (!lpAnswer[i])
            {
                lpAnswer[i] = lpObject;
                break;
            }
        }
    }
}

/****************************************************************************/

BOOL FAR WorldAnswerProc (HWND hWnd,LONG lParam,WORD wMsg)
{
    if (wMsg == WM_LBUTTONDOWN)
    {
        LPFRAME lpFrame = (LPFRAME) GetWindowLong (hWnd,GWL_IMAGE);

        if (lpFrame == lpCorrectAnswer)
        {
            SoundStartResource("GOODANSWER3", FALSE, 0);
            SendMessage (GetParent (hWnd),WM_COMMAND,HS_SHOWME,0L);
        }
        else
            SoundStartResource("WRONGANSWER2", FALSE, 0);
    }
    return TRUE;
}

/****************************** Public Functions ****************************/

BOOL WINPROC EXPORT DlgRoomWorld (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    char szTemp[MAX_PATH];
    HWND hCtl;
    int  i;
    LPFRAME lpFrame;
    LPOBJECT lpBase;
    LPOBJECT lpObject;
    static BOOL bInit = FALSE;

    if (wMsg == WM_COMMAND)
    {
        switch (wParam)
        {
        case HS_COVER:
			StopAnimation();
            hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
            ShowWindow (hCtl,SW_HIDE);
            hCtl = GetDlgItem (hWnd,HS_LOGO_WORLD);
            ShowWindow (hCtl,SW_SHOW);
            hCtl = GetDlgItem (hWnd,HS_XTRA_WORLD);
            ShowWindow (hCtl,SW_HIDE);
            for (i = 0; i < MAX_ANSWERS; i++)
            {
                hCtl = GetDlgItem (hWnd,IDC_WORLD_ANSWER1 + i);
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
            if (!bInit)
            {
                for (i = 0; i < MAX_ANSWERS; i++)
                {
                    hCtl = GetDlgItem (hWnd,IDC_WORLD_ANSWER1 + i);
                    GetWindowRect (hCtl,&rAnswer[i]);
                    MapWindowPoints (NULL,hWnd,(LPPOINT)&rAnswer[i],2);
                }
                bInit = TRUE;
            }
            for (i = 0; i < MAX_ANSWERS; i++)
                lpAnswer[i] = NULL;
            wsprintf (szTemp,GetString (IDS_WORLD,NULL),wParam - HS_TAB1 + 1);
            PathCat (szPreview,Control.PouchPath,szTemp);
            AstralImageLoad (IDN_ART,szPreview,MAYBE,NO);
            if (lpImage)
            {
                i = 0;
                lpObject = lpBase = ImgGetBase (lpImage);
                while (lpObject = ImgGetNextObject (lpImage,lpObject,YES,NO))
                {
                    if (i < MAX_ANSWERS)
                        Randomizer (i,lpObject);
                    lpObject->fHidden = TRUE;
                    i++;
                }
                hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
                SetWindowLong (hCtl,GWL_IMAGE,(long)lpBase);
                SetWindowLong (hCtl,GWL_IMAGE_TOOLPROC,(long)AnimateProc);
                ShowWindow (hCtl,SW_SHOW);
                InvalidateRect (hCtl,NULL,TRUE);
                for (i = 0; i < MAX_ANSWERS; i++)
                {
                    if (lpAnswer[i])
                    {
                        hCtl = GetDlgItem (hWnd,IDC_WORLD_ANSWER1 + i);
                        lpFrame = PixmapFrame (&lpAnswer[i]->Pixmap,PMT_EDIT);
                        SetWindowLong (hCtl,GWL_IMAGE,(long)lpFrame);
                        SetWindowLong (hCtl,GWL_IMAGE_TOOLPROC,
                            (long)WorldAnswerProc);
                        ShowWindow (hCtl,SW_SHOW);
                        InvalidateRect (hCtl,NULL,TRUE);
                    }
                }
                hCtl = GetDlgItem (hWnd,HS_LOGO_WORLD);
                ShowWindow (hCtl,SW_HIDE);
                hCtl = GetDlgItem (hWnd,HS_XTRA_WORLD);
                ShowWindow (hCtl,SW_SHOW);
                InvalidateRect (hCtl,NULL,TRUE);
                bHandled = FALSE;
            }
			SoundStartID(HS_XTRA_WORLD, FALSE, 0);
            break;
        case HS_COLORME:
			StopAnimation();
            for (i = 0; i < MAX_ANSWERS; i++)
            {
                if (lpAnswer[i])
                {
                    ImgDeleteObject (lpImage,lpAnswer[i]);
                    lpAnswer[i] = NULL;
                }
            }
            GoPaintApp (hWnd,szPreview);
            break;
        case HS_SHOWME:
            if (lpImage)
            {
				HWND hImageCtrl;
                lpObject = ImgGetBase (lpImage);
                while (lpObject = ImgGetNextObject (lpImage,lpObject,YES,NO))
                    lpObject->fHidden = FALSE;
                hCtl = hImageCtrl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
                ShowWindow (hCtl,SW_SHOW);
                InvalidateRect (hCtl,NULL,TRUE);
                for (i = 0; i < MAX_ANSWERS; i++)
                {
                    hCtl = GetDlgItem (hWnd,IDC_WORLD_ANSWER1 + i);
                    SetWindowLong (hCtl,GWL_IMAGE,0L);
                    ShowWindow (hCtl,SW_HIDE);
                    InvalidateRect (hCtl,NULL,TRUE);
					UpdateWindow(hCtl);
                    if (lpAnswer[i])
                    {
                        lpFrame = PixmapFrame (&lpAnswer[i]->Pixmap,PMT_EDIT);
                        if (lpFrame != lpCorrectAnswer)
                            ImgDeleteObject (lpImage,lpAnswer[i]);
                        lpAnswer[i] = NULL;
                    }
                }
				UpdateWindow(hImageCtrl);
				StartAnimation(hImageCtrl);
            }
            hCtl = GetDlgItem (hWnd,HS_XTRA_WORLD);
            ShowWindow (hCtl,SW_HIDE);
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

