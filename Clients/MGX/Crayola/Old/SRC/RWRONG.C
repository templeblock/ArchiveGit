/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                    wrong.c

******************************************************************************
******************************************************************************

  Description:  What's Wrong Room Dialog Proc
 Date Created:  03/04/94
       Author:  Stephen Suggs

*****************************************************************************/

#include <windows.h>
#include <math.h>
#include <ctype.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern HINSTANCE hInstAstral;
extern HWND   hWndAstral;

static HWND hWrongWnd;
#define MAX_PAIRS 30
static BOOL WrongPairs[MAX_PAIRS];
static int nPairs = 0;

static void ShowControls(HWND hWnd, bool bCover);
static int WrongProc( HWND hWindow, LPARAM lParam, UINT msg );
static LPOBJECT ImgFindVisibleObject(LPIMAGE lpImage, int x, int y, BOOL fBottomUp,
					BOOL fExcludeBase);
static void HideAlternateObjects();					
static void UnHideAlternateObjects();					
static void AlternateObjects(LPOBJECT lpObject, RECT* lpUpdateRect);
static void ClearWrongPairs(BOOL bSet);
static int  GetPairPos(LPOBJECT lpFindObject);
static bool CheckFinished();
/****************************************************************************/

BOOL WINPROC EXPORT DlgRoomWrong (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    char szTemp[MAX_PATH];
    HWND hCtl;

    switch (wMsg)
    {
	case WM_INITDIALOG:
		hWrongWnd = hWnd;
		DefRoomProc (hWnd, wMsg, wParam, lParam);
		// hide the alternate set
		ShowControls(hWnd, TRUE);
		SetInitialControls (hWnd);
		bHandled = TRUE;
		break;

    case WM_COMMAND:
        switch (wParam)
        {
        case HS_COVER:
			ShowControls(hWnd, TRUE);
            bHandled = FALSE;
			SetInitialControls (hWnd);
            break;

        case HS_TAB1:
        case HS_TAB2:
        case HS_TAB3:
        case HS_TAB4:
        case HS_TAB5:
        case HS_TAB6:
        case HS_TAB7:
        case HS_TAB8:
			HideAllControls (hWnd);
            wsprintf (szTemp, GetString (IDS_WRONG, NULL), wParam - HS_TAB1 + 1);
            PathCat (szPreview,Control.PouchPath, szTemp);
            AstralImageLoad (IDN_ART,szPreview, MAYBE, NO);
            if (lpImage)
            {
                LPOBJECT lpObject;

				ClearWrongPairs(FALSE);
				HideAlternateObjects();			// Hide every other one
                lpObject = ImgGetBase (lpImage);
                hCtl = GetDlgItem (hWnd, IDC_HIDDEN_IMAGE);
                SetWindowLong (hCtl, GWL_IMAGE, (long)lpObject);
            	SetWindowLong(hCtl, GWL_IMAGE_TOOLPROC,
                	(long)WrongProc);
                ShowWindow (hCtl, SW_SHOW);
				ShowControls(hWnd, FALSE);
                InvalidateRect(hCtl, NULL, TRUE);
            }
			SoundStartID(HS_XTRA_WRONG, FALSE, 0);
			bHandled = TRUE;
            break;

        case HS_COLORME:
			ImgCombineObjects(lpImage, TRUE, FALSE, TRUE);
            GoPaintApp(hWnd,szPreview);
            break;

        case HS_SHOWME:
			ClearWrongPairs(TRUE);
			UnHideAlternateObjects();
            hCtl = GetDlgItem (hWnd, IDC_HIDDEN_IMAGE);
            InvalidateRect(hCtl, NULL, TRUE);
			bHandled = TRUE;
            break;
			

		case HS_WRONG_APPLES:
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_APPLES), SW_HIDE);
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_LBULBS), SW_SHOW);
		bHandled = TRUE;
		break;

		case HS_WRONG_LBULBS:
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_LBULBS), SW_HIDE);
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_APPLES), SW_SHOW);
		bHandled = TRUE;
		break;

		case HS_WRONG_PIZZA:
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_PIZZA), SW_HIDE);
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_NUT), SW_SHOW);
		bHandled = TRUE;
		break;

		case HS_WRONG_NUT:
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_NUT), SW_HIDE);
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_PIZZA), SW_SHOW);
		bHandled = TRUE;
		break;

		case HS_WRONG_FISH:
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_FISH), SW_HIDE);
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_BEAR), SW_SHOW);
		bHandled = TRUE;
		break;

		case HS_WRONG_BEAR:
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_BEAR), SW_HIDE);
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_FISH), SW_SHOW);
		bHandled = TRUE;
		break;

		case HS_WRONG_PINE:
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_PINE), SW_HIDE);
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_PALM), SW_SHOW);
		bHandled = TRUE;
		break;

		case HS_WRONG_PALM:
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_PALM), SW_HIDE);
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_PINE), SW_SHOW);
		bHandled = TRUE;
		break;

		case HS_WRONG_DUCK1:
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_DUCK1), SW_HIDE);
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_DUCK2), SW_SHOW);
		bHandled = TRUE;
		break;

		case HS_WRONG_DUCK2:
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_DUCK2), SW_HIDE);
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_DUCK1), SW_SHOW);
		bHandled = TRUE;
		break;

		case HS_WRONG_PIG:
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_PIG), SW_HIDE);
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_BIRD), SW_SHOW);
		bHandled = TRUE;
		break;

		case HS_WRONG_BIRD:
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_BIRD), SW_HIDE);
		ShowWindow (GetDlgItem(hWnd, HS_WRONG_PIG), SW_SHOW);
		bHandled = TRUE;
		break;

        default:
            bHandled = FALSE;
        }
        break;

    default:
        bHandled = FALSE;
    }

    if (! bHandled)
        bHandled = DefRoomProc (hWnd, wMsg, wParam, lParam);

    return bHandled;
}



int WrongProc(
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
    int         x, y;
    LPOBJECT    lpObject;
    HWND        hImageCtrl;
    RECT        rUpdate;
	int			nPos;

    x = LOWORD(lParam);
    y = HIWORD(lParam);
    switch (msg)
    {
        case WM_CREATE:	// The first mouse down message
    	case WM_LBUTTONDOWN:
            lpObject = ImgFindVisibleObject(lpImage, x, y, TRUE, TRUE);
    		if (lpObject)
    		{
				SoundStartResource("ICONS", FALSE, 0);
				AlternateObjects(lpObject, &rUpdate);
                hImageCtrl = GetDlgItem(hWrongWnd, IDC_HIDDEN_IMAGE);
                InvalidateRect(hImageCtrl, &rUpdate, FALSE);
				nPos = GetPairPos(lpObject);
				if (nPos >= 0 && nPos < nPairs)
					WrongPairs[nPos] = !WrongPairs[nPos];
				if (WrongPairs[nPos])
					 SoundStartResource("GOODANSWER1",  FALSE, 0);
				else SoundStartResource("WRONGANSWER3", FALSE, 0);
				if (CheckFinished())
					SoundStartResource("GOODANSWER3", FALSE, 0);
            }
    		break;

    	case WM_SETCURSOR:
    		return TRUE;

        case WM_LBUTTONUP:
    		break;

        case WM_MOUSEMOVE:	// sent when ToolActive is on
    		break;
    }
    return TRUE;
}



void AlternateObjects(LPOBJECT lpObject, RECT* lpUpdateRect)
{
/*
	Objects are toggled on and off in pairs.  Objects 1/2, 3/4, 5/6 ...
	When a mouse down occurs, locate the object being clicked on and turn it off,
	while turning its pair on.  They will toggle.
*/	
	bool bBottomUp;	
	RECT rUpdate, rUpdate1;
	LPOBJECT lpNext; 
	LPOBJECT lpBase = NULL;
	bBottomUp = lpObject->fHidden;

	lpBase = ImgGetBase(lpImage);
	lpNext = ImgGetNextObject(lpImage, lpObject, bBottomUp, FALSE);
	lpObject->fHidden = !lpObject->fHidden;
	ImgChangeSelection(lpImage, lpObject, TRUE, FALSE);
	ImgAdjustLayerSelObj(lpImage, bBottomUp, TRUE);
	ImgChangeSelection(lpImage, lpObject, FALSE, FALSE);
	rUpdate = lpObject->rObject;

	if (lpNext)
	{
		lpNext->fHidden = !lpNext->fHidden;
		rUpdate1 = lpNext->rObject;
		UnionRect(&rUpdate, &rUpdate, &rUpdate1);
	}
	
	InflateRect(&rUpdate, 2, 2);
	CopyRect(lpUpdateRect, &rUpdate);
}




/****************************************************************************/

static void SetInitialControls (HWND hWnd)
{
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_APPLES), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_LBULBS), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, HS_WRONG_NUT), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_PIZZA), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, HS_WRONG_PINE), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_PALM), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, HS_WRONG_BEAR), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_FISH), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, HS_WRONG_DUCK2), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_DUCK1), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, HS_WRONG_BIRD), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_PIG), SW_SHOW);
}


/****************************************************************************/

static void HideAllControls (HWND hWnd)
{
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_APPLES), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_LBULBS), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_NUT), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_PIZZA), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_BEAR), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_FISH), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_PINE), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_PALM), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_DUCK1), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_DUCK2), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_BIRD), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_WRONG_PIG), SW_HIDE);
}

/************************************************************************/
LPOBJECT ImgFindVisibleObject(LPIMAGE lpImage, int x, int y, BOOL fBottomUp,
					BOOL fExcludeBase)
/************************************************************************/
{
POINT pt;
LPOBJECT lpObject = NULL;
LPOBJECT lpBase;

if (!lpImage)
	return(NULL);
pt.x = x;
pt.y = y;
lpBase = ImgGetBase(lpImage);
while (lpObject = ImgGetNextObject(lpImage, lpObject, fBottomUp, NO))
	{
	if (fExcludeBase && lpObject == lpBase)
		continue;
	if (!lpObject->fHidden && PtInRect(&lpObject->rObject, pt))
		return lpObject;
	}
return(NULL);
}


void HideAlternateObjects()
{
// Mark the objects as they are being hidden.  This way they can be reversed later.
// When the SHOWME button is pressed, we want to show the opposite objects.
	LPOBJECT lpObject = NULL;
	LPOBJECT lpBase   = NULL;
	BOOL bHide = FALSE;
	
	nPairs = 0;
	
	if (!lpImage) return;
	lpBase = ImgGetBase(lpImage);
	while (lpObject = ImgGetNextObject(lpImage, lpObject, FALSE, NO))
	{
		if (lpObject != lpBase)
		{
			if (!bHide)
				nPairs++;
			lpObject->fHidden = bHide;
			lpObject->dwObjectData = bHide;		// flag to unhide when SHOWME is pressed
			bHide = !bHide;
		}
	}
}


void UnHideAlternateObjects()
{
// If any of the objects have not been swapped then swap them.
// The SHOWME button reveals the correct objects.
	RECT rUpdate;
	LPOBJECT lpObject = NULL;
	LPOBJECT lpBase   = NULL;
	LPOBJECT lpNext   = NULL;
	
	if (!lpImage) return;
	lpBase = ImgGetBase(lpImage);

	lpObject = NULL;
	while (lpObject = ImgGetNextObject(lpImage, lpObject, FALSE, NO))
	{
		if (lpObject == lpBase) break;
		if (!lpObject->dwObjectData)
		{
			// "next" will be "prev" once AlternateObjects() is called. 
			lpNext = ImgGetNextObject(lpImage, lpObject, FALSE, NO);
			AlternateObjects(lpObject, &rUpdate);
			lpObject = lpNext;
		}
		lpObject = ImgGetNextObject(lpImage, lpObject, FALSE, NO);
		if (lpObject == lpBase) break;
		if (!lpObject) break;
	}
}



void ShowControls(HWND hWnd, bool bCover)
{
    HWND hCtrl;
    
    hCtrl = GetDlgItem(hWnd, IDC_HIDDEN_IMAGE);
    if (hCtrl)
    	ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);

    hCtrl = GetDlgItem(hWnd, HS_COLORME);
    if (hCtrl)
    	ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);

    hCtrl = GetDlgItem(hWnd, HS_SHOWME);
    if (hCtrl)
    	ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);

	hCtrl = GetDlgItem (hWnd, HS_LOGO_WRONG);
	if (hCtrl)
	    ShowWindow(hCtrl, bCover ? SW_SHOW : SW_HIDE);

	hCtrl = GetDlgItem (hWnd, HS_XTRA_WRONG);
	if (hCtrl)
	    ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);
}

void ClearWrongPairs(BOOL bSet)
{
	int i;
	for (i=0; i<MAX_PAIRS; i++)
		WrongPairs[i] = bSet;	 
}


bool CheckFinished()
{
	int i;
	for (i=0; i<nPairs; i++)
		if (!WrongPairs[i])
			return FALSE;
	return TRUE;	 
}


int GetPairPos(LPOBJECT lpFindObject)
{
// Find the pair that this object represents.
	LPOBJECT lpObject = NULL;
	LPOBJECT lpBase   = NULL;
	int nPair = 0;
	int nPos = 0;
	
	if (!lpImage) return -1;
	lpBase = ImgGetBase(lpImage);
	while (lpObject = ImgGetNextObject(lpImage, lpObject, FALSE, NO))
	{
		if (lpObject != lpBase)
		{
			if (lpObject == lpFindObject)
				return nPos;
				
			if (nPair)
				nPos++;
			nPair = !nPair;
		}
	}
	return -1;
}




