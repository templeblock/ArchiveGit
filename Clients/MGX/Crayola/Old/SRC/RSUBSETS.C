/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                 rsubsets.c

******************************************************************************
******************************************************************************

  Description:  Sets and Subsets Dialog Proc
 Date Created:  03/07/94
       Author:  Stephen Suggs

*****************************************************************************/

#include <windows.h>
#include <math.h>
#include <ctype.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "animate.h"

extern HINSTANCE hInstAstral;
extern HWND   hWndAstral;

static HWND hSubSetWnd;
static bool bShowState;
static int SubsetsProc( HWND hWindow, LPARAM lParam, UINT msg );
static void ShowAllObjects(HWND hWnd, bool bShow);
static void DeleteAllObjects();
static void AnimateFirstObject(HWND hWnd);
static void PlayAnimationObject(HWND hImageCtrl, LPOBJECT lpObject);
static LPOBJECT ImgFindObjectRect(LPIMAGE lpImage, int x, int y, BOOL fBottomUp,
					BOOL fExcludeBase);

/****************************************************************************/

BOOL WINPROC EXPORT DlgRoomSubsets (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    char    szTemp[MAX_PATH];
    HWND    hCtl;
    BOOL    bHandled = TRUE;

    switch (wMsg)
    {
	case WM_INITDIALOG:
		hSubSetWnd = hWnd;
		DefRoomProc (hWnd, wMsg, wParam, lParam);
		bHandled = TRUE;
		break;

    case WM_COMMAND:
        switch (wParam)
        {
        case HS_COVER:
			ShowCoverBitmaps (hWnd);
            hCtl = GetDlgItem (hWnd, IDC_HIDDEN_IMAGE);
            ShowWindow (hCtl,SW_HIDE);
            hCtl = GetDlgItem (hWnd, HS_LOGO_SUBSETS);
            if (hCtl)
                ShowWindow (hCtl,SW_SHOW);
            hCtl = GetDlgItem (hWnd, HS_XTRA_SUBSETS);
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
			HideCoverBitmaps (hWnd);
            wsprintf (szTemp, GetString (IDS_SUBSETS, NULL), wParam - HS_TAB1 + 1);
            PathCat (szPreview, Control.PouchPath, szTemp);
            AstralImageLoad (IDN_ART, szPreview, MAYBE, NO);
            if (lpImage)
            {
                LPOBJECT lpObject;

                lpObject = ImgGetBase (lpImage);
                hCtl = GetDlgItem (hWnd,IDC_HIDDEN_IMAGE);
                SetWindowLong (hCtl,GWL_IMAGE,(long)lpObject);
	            SetWindowLong(hCtl, GWL_IMAGE_TOOLPROC,
	                (long)SubsetsProc);
                ShowWindow (hCtl,SW_SHOW);
                InvalidateRect (hCtl,NULL,TRUE);
                hCtl = GetDlgItem (hWnd, HS_LOGO_SUBSETS);
                if (hCtl)
                    ShowWindow (hCtl,SW_HIDE);
                hCtl = GetDlgItem (hWnd, HS_XTRA_SUBSETS);
                if (hCtl)
                    ShowWindow (hCtl,SW_SHOW);
                bHandled = FALSE;
            }
			SoundStartID(HS_XTRA_SUBSETS, FALSE, 0);
            break;

        case HS_COLORME:
			DeleteAllObjects();
            GoPaintApp (hWnd,szPreview);
            break;

		case HS_SHOWME:
			AnimateFirstObject(hWnd);
			break;                    

        case IDC_SS_COW:
        case IDC_SS_W1:
			PlayCoverAnimation (hWnd, 152, 94, IDC_SS_COW, IDC_SS_W1, 408, 313);
			bHandled = TRUE;
            break;

        case IDC_SS_PINEAPPLE:
        case IDC_SS_H1:
			PlayCoverAnimation (hWnd, 202, 94, IDC_SS_PINEAPPLE, IDC_SS_H1, 28, 77);
			bHandled = TRUE;
            break;

        case IDC_SS_GFISH:
        case IDC_SS_A1:
			PlayCoverAnimation (hWnd, 245, 94, IDC_SS_GFISH, IDC_SS_A1, 140, 315);
			bHandled = TRUE;
            break;
        
        case IDC_SS_CARROT:
        case IDC_SS_T1:
			PlayCoverAnimation (hWnd, 287, 95, IDC_SS_CARROT, IDC_SS_T1, 89, 179);
			bHandled = TRUE;
            break;
        
        case IDC_SS_TOUCAN1:
        case IDC_SS_S1:
			PlayCoverAnimation (hWnd, 338, 94, IDC_SS_TOUCAN1, IDC_SS_S1, 405, 249);
			bHandled = TRUE;
            break;
        
        case IDC_SS_SAW:
        case IDC_SS_I:
			PlayCoverAnimation (hWnd, 205, 149, IDC_SS_SAW, IDC_SS_I, 294, 305);
			bHandled = TRUE;
            break;
        
        case IDC_SS_ELEPHANT:
        case IDC_SS_N:
			PlayCoverAnimation (hWnd, 246, 149, IDC_SS_ELEPHANT, IDC_SS_N, 214, 315);
			bHandled = TRUE;
            break;
        
        case IDC_SS_FOOTBALL:
        case IDC_SS_QUEST1:
			PlayCoverAnimation (hWnd, 291, 149, IDC_SS_FOOTBALL, IDC_SS_QUEST1, 423, 209);
			bHandled = TRUE;
            break;
        
        case IDC_SS_GIRAFFE:
        case IDC_SS_W2:
			PlayCoverAnimation (hWnd, 153, 203, IDC_SS_GIRAFFE, IDC_SS_W2, 360, 90);
			bHandled = TRUE;
            break;
        
        case IDC_SS_PIG:
        case IDC_SS_H2:
			PlayCoverAnimation (hWnd, 204, 203, IDC_SS_PIG, IDC_SS_H2, 46, 316);
			bHandled = TRUE;
            break;
        
        case IDC_SS_PAROT1:
        case IDC_SS_A2:
			PlayCoverAnimation (hWnd, 245, 203, IDC_SS_PAROT1, IDC_SS_A2, 29, 150);
			bHandled = TRUE;
            break;
        
        case IDC_SS_BUTFLY:
        case IDC_SS_T2:
			PlayCoverAnimation (hWnd, 289, 203, IDC_SS_BUTFLY, IDC_SS_T2, 441, 97);
			bHandled = TRUE;
            break;
        
        case IDC_SS_CLOWN:
        case IDC_SS_S2:
			PlayCoverAnimation (hWnd, 338, 203, IDC_SS_CLOWN, IDC_SS_S2, 61, 25);
			bHandled = TRUE;
            break;
        
        case IDC_SS_OCTOPUS:
        case IDC_SS_O:
			PlayCoverAnimation (hWnd, 183, 256, IDC_SS_OCTOPUS, IDC_SS_O, 361, 25);
			bHandled = TRUE;
            break;
        
        case IDC_SS_PAN:
        case IDC_SS_U:
			PlayCoverAnimation (hWnd, 225, 257, IDC_SS_PAN, IDC_SS_U, 85, 132);
			bHandled = TRUE;
            break;

        case IDC_SS_APPLE:
        case IDC_SS_T3:
			PlayCoverAnimation (hWnd, 269, 258, IDC_SS_APPLE, IDC_SS_T3, 368, 314);
			bHandled = TRUE;
            break;

        case IDC_SS_CRAB1:
        case IDC_SS_QUEST2:
			PlayCoverAnimation (hWnd, 315, 257, IDC_SS_CRAB1, IDC_SS_QUEST2, 31, 255);
			bHandled = TRUE;
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
static void PlayCoverAnimation (HWND hWnd, int x, int y, int iID1, int iID2,
  int x2, int y2)
{
    RECT    rControl, rLetter;
    HWND    hControl, hLetter;
    STRING  szString;

    hControl = GetDlgItem (hWnd, iID1);
    hLetter = GetDlgItem (hWnd, iID2);

	if (hControl == NULL || hLetter == NULL)
		return;

    GetWindowRect (hControl, &rControl);
    GetWindowRect (hLetter, &rLetter);

	// hide the letter
	ShowWindow (hLetter, SW_HIDE);
	// hide the character
	ShowWindow (hControl, SW_HIDE);
	UpdateWindow (hWnd);

	// animate character to the letter position
	wsprintf( szString, "SETSPEED 37\rJUMP %d,%d\rMOVE %d,%d\r",
	  x2, y2, x, y);
	AnimateDibID (hWnd, NULL, hInstAstral, iID1, NULL, szString);

	// reposition the letter
	hControl = GetDlgItem (hWnd, iID2);
	SetWindowPos (hControl, (HWND)NULL, x, y, RectWidth (&rLetter),
	  RectHeight (&rLetter), SWP_SHOWWINDOW);

	UpdateWindow (hWnd);

	// animate the character back to original position
	wsprintf( szString, "SETSPEED 37\rJUMP %d,%d\rMOVE %d,%d\r",
	  x, y, x2, y2);
	AnimateDibID (hWnd, NULL, hInstAstral, iID1, NULL, szString);

	ShowWindow (GetDlgItem (hWnd, iID1), SW_SHOW);
}


/****************************************************************************/

static void ShowCoverBitmaps (HWND hWnd)
{
	int iID;

	for (iID = IDC_SS_CLOWN ; iID <= IDC_SS_QUEST2 ; iID++)
		ShowWindow (GetDlgItem (hWnd, iID), SW_SHOW);
}						   


/****************************************************************************/

static void HideCoverBitmaps (HWND hWnd)
{
	int iID;

	for (iID = IDC_SS_CLOWN ; iID <= IDC_SS_QUEST2 ; iID++)
		ShowWindow (GetDlgItem (hWnd, iID), SW_HIDE);
}


int SubsetsProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
    int         x, y;
    LPOBJECT    lpObject;
    HWND        hImageCtrl;

    x = LOWORD(lParam);
    y = HIWORD(lParam);
    switch (msg)
    {
        case WM_CREATE:	// The first mouse down message
    	case WM_LBUTTONDOWN:
            lpObject = ImgFindObjectRect(lpImage, x, y, TRUE, TRUE);
    		if (lpObject && !lpObject->fHidden)
    		{				 
                hImageCtrl = GetDlgItem(hSubSetWnd, IDC_HIDDEN_IMAGE);
                if (lpObject->ObjectDataID == OBJECT_DATA_STOCK_ANIMATION)
                {
            		SoundStartResource("GOODANSWER1", FALSE, 0);
					PlayAnimationObject(hWindow, lpObject);
					lpObject->fHidden = TRUE;
					UpdateImage(&lpObject->rObject, YES);
                }
            }
			else
				SoundStartResource("WRONGANSWER3", FALSE, 0);
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


void ShowAllObjects(HWND hWnd, bool bShow)
{
	LPOBJECT lpObject = NULL;
	while (lpObject = ImgGetNextObject(lpImage, lpObject, FALSE, NO))
        lpObject->fHidden = bShow;
    AstralControlPaint(hWnd, IDC_HIDDEN_IMAGE);
}


void DeleteAllObjects()
{
	LPOBJECT lpObject = NULL;
	LPOBJECT lpBase = NULL;

	lpBase = ImgGetBase(lpImage);
	while ((lpObject = ImgGetNextObject(lpImage, FALSE, FALSE, NO)) &&
		   lpObject != lpBase)
        ImgDeleteObject(lpImage, lpObject );
}




void AnimateFirstObject(HWND hWnd)
{
// Find the first object in the list and play it.
	LPOBJECT lpObject = NULL;
	HWND hImageCtrl = GetDlgItem(hWnd, IDC_HIDDEN_IMAGE);
	while (lpObject = ImgGetNextObject(lpImage, lpObject, FALSE, NO))
	{
    	if (lpObject->ObjectDataID == OBJECT_DATA_STOCK_ANIMATION)
        {
			PlayAnimationObject(hImageCtrl, lpObject);
			break;
        }
	}
}


void PlayAnimationObject(HWND hImageCtrl, LPOBJECT lpObject)
{
	STRING 		szString;
	FNAME 		szFileName;
	RECT		rUpdate;
	HINSTANCE 	hToy = 0;
	int 		hAObject = 0;
	LPAOBJECT 	Asession = NULL; // The session handle
	if (!lpObject) return;
	
	wsprintf( szString, "%04d", (int)(WORD)lpObject->dwObjectData );
    if ( GetToyFileName( szString, szFileName ) )
    {
    	if (!(Asession = ACreateNewAnimator()))
    		return;

    	if ((hToy = LoadLibrary(szFileName)) < (HINSTANCE)HINSTANCE_ERROR)
    		return;
			
		hAObject = LoadToy(
			Asession, 
			NULL, 
			1, 
			lpObject->rObject.left, 
			lpObject->rObject.top, 
			hToy);

		rUpdate = lpObject->rObject;
		InflateRect(&rUpdate, 2, 2);	// cover the entire object
        lpObject->fHidden = TRUE;
        lpObject->fChanged = TRUE;
		InvalidateRect(hImageCtrl, &rUpdate, TRUE);
		UpdateWindow(hImageCtrl); 
        AnimateToy (hImageCtrl,lpObject->rObject.left,
                    lpObject->rObject.top,szFileName,TRUE,
                    hToy, hAObject, Asession);
    }
}

/************************************************************************/
LPOBJECT ImgFindObjectRect(LPIMAGE lpImage, int x, int y, BOOL fBottomUp,
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
	if (PtInRect(&lpObject->rObject, pt))
		{
		return(lpObject);
		}
	}
return(NULL);
}





