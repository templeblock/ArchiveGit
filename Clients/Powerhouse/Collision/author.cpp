#include <string.h>
#include <fstream.h>
#include <limits.h>

#include "proto.h"
#include "res_author.h"
#include "author.p"
#include "cllsn.h"
#include "commctrl.h"
#include "cllsnid.h"
#include "cllsndef.h"
#include "author.h"
#include "vcontrol.p"
#include "vtable.h"
#include "control.h"

const int nMaxIn = 120;
char *g_szLook = "0";	// Change to hard code looks in the authoring dialogs

//#define COMMENT(n) &lpComments[(n)*nMaxIn]
#define COMMENT(n) lpOrig[n].szDescr
#define OUTS(sz) if ((sz)[0]) out << (sz) << '\t'; else out << " \t";

static HWND hWndDialog = NULL;
static HWND hSceneWindow = NULL;
static LPVIDEO lpVideo = NULL;
static LPSHOT lpCurShot = NULL;
static BOOL bDragging = FALSE;
static BOOL bTestEvent = FALSE;
static BOOL bTestEvent2 = FALSE;

static long lTestEventFrom = 0;	// Test event from frame
static long lTestEventTo = 0;	// Test event to frame
static HTREEITEM *phItems = NULL;
static STRING szError;
static EXCEL_ORIG *lpOrig = NULL;	// Original excel spreadsheet information
static long lCurrFrame = 0;			// Current frame (used for testing event shots)

static BOOL bGrabEventOn = FALSE;	// Grab event flag
static BOOL bEventShotDirty = FALSE;

BOOL AuthorOnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
void AuthorOnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
BOOL WINPROC EXPORT AuthorDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL AuthorOnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL AuthorOnSetCheck(int id);
void LoadHexLooks(HWND hTree, TV_INSERTSTRUCT *pItem, char *pszKey, LPVIDEO lpVideo);
void LoadHexPortals(HWND hTree, TV_INSERTSTRUCT *pItem, char *pszKey, LPVIDEO lpVideo);
void LoadShotNames(HWND hTree, TV_INSERTSTRUCT *pItem, char *pszKey, LPVIDEO lpVideo);
void SetCurrentShotFromTree(NM_TREEVIEW FAR *pView);
BOOL SetCurrentShotFromName(LPSTR lpszName);
void SetEventShot(int iCurSel);
void SetFlags(void);
void LoadUpdate(void);
void ClearEditFlag(void);
void LoadOtherShots(HWND hTree, TV_INSERTSTRUCT *pItem, LPVIDEO lpVideo);
void PlayTest(void);
void ExportAlias(void);
void ExportShots(void);
void SaveRecValues(HWND hWnd);
char *GetValidShotName(void);
BOOL BaseShotNameEqual( char *pszName1, char *pszName2 );
void CopyShotNameChanges(char *pszSrc, char *pszDest);
void SetShotName(HWND hWndDialog, char *pszText);
SHOTID MakeShotID(char *lpString);
void LoadAliasFromText(char *szFileName, LPSHOT *lpShots, int *piNumShots);
void LoadShotsFromText(char *szFileName, LPSHOT *lpShots, int *piNumShots);
void LoadTableFromText(void);
void SaveEventShot(void);
void DumpOriginalRecs(int id);
void LoadOriginalRecs(int id);
void LoadUJGates(HWND hTree, TV_INSERTSTRUCT *pItem, char *pszKey, LPVIDEO lpVideo);
BOOL CellShot(char *psz);
void SetNameChange(HWND hTree, LPSHOT lpShot, char *pszName);
void SetRecFlags(LPSHOT lpShot);
void SetCurrentFrame(void);
SHOTID FindShot(char *psz);

/************************************************************************/
void AuthorStart(HWND hWnd, LPVIDEO lpVid)
/************************************************************************/
{
	if (hWndDialog)
		return;

	hSceneWindow = hWnd;
	lpVideo = lpVid;
	hWndDialog = DialogStart( YES, GetWindowInstance(hWnd), hWnd, IDD_AUTHOR_DIALOG, AuthorDlgProc);
	DialogShow(IDD_AUTHOR_DIALOG);
}

/************************************************************************/
void AuthorEnd(void)
/************************************************************************/
{
	if (hWndDialog)
	{
		if (phItems)
			FreeUp(phItems);

		DialogEnd( hWndDialog, IDOK);
		hWndDialog = NULL;
	}
}


/************************************************************************/
BOOL AuthorOnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	ClearEditFlag();

	StartLoadTree(hWnd);

	// Initialize Event combo
	SendDlgItemMessage(hWnd, IDC_EVENTS, CB_ADDSTRING, 0, (LPARAM)(char *)"End");
	SendDlgItemMessage(hWnd, IDC_EVENTS, CB_ADDSTRING, 0, (LPARAM)(char *)"Left");
	SendDlgItemMessage(hWnd, IDC_EVENTS, CB_ADDSTRING, 0, (LPARAM)(char *)"Right");
	SendDlgItemMessage(hWnd, IDC_EVENTS, CB_ADDSTRING, 0, (LPARAM)(char *)"Down");

	// Init Cell type combo
	SendDlgItemMessage(hWnd, IDC_CELLTYPE, CB_ADDSTRING, 0, (LPARAM)(char *)"Hex");
	SendDlgItemMessage(hWnd, IDC_CELLTYPE, CB_ADDSTRING, 0, (LPARAM)(char *)"Union Jack");
	SendDlgItemMessage(hWnd, IDC_CELLTYPE, CB_ADDSTRING, 0, (LPARAM)(char *)"Other");

	// Init Shot type combo
	SendDlgItemMessage(hWnd, IDC_SHOTTYPE, CB_ADDSTRING, 0, (LPARAM)(char *)"None");
	SendDlgItemMessage(hWnd, IDC_SHOTTYPE, CB_ADDSTRING, 0, (LPARAM)(char *)"F_SPINE_HIT");
	SendDlgItemMessage(hWnd, IDC_SHOTTYPE, CB_ADDSTRING, 0, (LPARAM)(char *)"F_SPINE_DEAD");
	SendDlgItemMessage(hWnd, IDC_SHOTTYPE, CB_ADDSTRING, 0, (LPARAM)(char *)"F_NEWCELL");
	SendDlgItemMessage(hWnd, IDC_SHOTTYPE, CB_ADDSTRING, 0, (LPARAM)(char *)"F_YOU_LOSE");
	SendDlgItemMessage(hWnd, IDC_SHOTTYPE, CB_ADDSTRING, 0, (LPARAM)(char *)"F_YOU_WIN");
	SendDlgItemMessage(hWnd, IDC_SHOTTYPE, CB_ADDSTRING, 0, (LPARAM)(char *)"F_EXIT");
	SendDlgItemMessage(hWnd, IDC_SHOTTYPE, CB_ADDSTRING, 0, (LPARAM)(char *)"F_HIT");
	SendDlgItemMessage(hWnd, IDC_SHOTTYPE, CB_ADDSTRING, 0, (LPARAM)(char *)"F_AUTOREVERSE");
	SendDlgItemMessage(hWnd, IDC_SHOTTYPE, CB_ADDSTRING, 0, (LPARAM)(char *)"F_LIVE_DIE");

	// Init shot cell position combo
	SendDlgItemMessage(hWnd, IDC_CELLPOSITION, CB_ADDSTRING, 0, (LPARAM)(char *)"None");
	SendDlgItemMessage(hWnd, IDC_CELLPOSITION, CB_ADDSTRING, 0, (LPARAM)(char *)"F_POS_BEG");
	SendDlgItemMessage(hWnd, IDC_CELLPOSITION, CB_ADDSTRING, 0, (LPARAM)(char *)"F_POS_MID");
	SendDlgItemMessage(hWnd, IDC_CELLPOSITION, CB_ADDSTRING, 0, (LPARAM)(char *)"F_POS_END");
	SendDlgItemMessage(hWnd, IDC_CELLPOSITION, CB_ADDSTRING, 0, (LPARAM)(char *)"F_POS_FULL");
	SendDlgItemMessage(hWnd, IDC_CELLPOSITION, CB_ADDSTRING, 0, (LPARAM)(char *)"F_POS_FULLC");
	SendDlgItemMessage(hWnd, IDC_CELLPOSITION, CB_ADDSTRING, 0, (LPARAM)(char *)"F_POS_WALL");
	SendDlgItemMessage(hWnd, IDC_CELLPOSITION, CB_ADDSTRING, 0, (LPARAM)(char *)"F_POS_VEER");
	SendDlgItemMessage(hWnd, IDC_CELLPOSITION, CB_ADDSTRING, 0, (LPARAM)(char *)"F_POS_BEG_3QTR");

	// Temporary vars to test list
	SendDlgItemMessage(hWnd, IDC_SHOTHOLD, LB_ADDSTRING, 0, (LPARAM)(char *)"Test 1");
	SendDlgItemMessage(hWnd, IDC_SHOTHOLD, LB_ADDSTRING, 0, (LPARAM)(char *)"Test 2");
	SendDlgItemMessage(hWnd, IDC_SHOTHOLD, LB_ADDSTRING, 0, (LPARAM)(char *)"Test 3");

	MCIStop( lpVideo->hDevice, YES/*bWait*/ );

	// Load original records (if any)

	ITEMID id;
	if ( !(id = GetWindowWord( lpVideo->hWnd, GWW_ICONID )) )
		id = GET_WINDOW_ID( lpVideo->hWnd );

	// Save other elements, comments, etc.
	LoadOriginalRecs(id);

	// Size everything and lay it out for maximum use of space
	int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
	int nCyScreen = GetSystemMetrics(SM_CYFULLSCREEN);

	RECT Rect;
	GetWindowRect(hWnd, &Rect);

	// Set collision window into upper left corner
	MoveWindow(GetParent(hSceneWindow), 0, 0, 640, 480, TRUE);
	MoveWindow(hSceneWindow, 0, 0, 640, 480, TRUE);

	// Set author dialog in the lower right corner
	MoveWindow(hWnd, nCxScreen - (Rect.right - Rect.left), nCyScreen - (Rect.bottom - Rect.top),
		Rect.right - Rect.left, Rect.bottom - Rect.top, TRUE);

	return TRUE;
}

/************************************************************************/
void AuthorOnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	if (id == IDC_EVENTS)
	{
		if (codeNotify == CBN_SELENDOK)
		{
			int iCurSel = SendMessage(hControl, CB_GETCURSEL, 0, 0);
			if (iCurSel != CB_ERR)
			{
				SetEventShot(iCurSel);
			}
		}
		else if (codeNotify == CBN_SELCHANGE)
		{
			if (bEventShotDirty)
			{
				int iRet = MessageBox(hWndDialog, "Save current event shot?", "Event changed", MB_YESNO);
				if (iRet == IDYES)
				{
					SaveEventShot();
				}
			}
		}
	}
	else if (id == IDC_SAVEEVENT)
	{
		SaveEventShot();
	}
	else if (id == IDC_SAVETABLE)
	{
		ITEMID id;
		if ( !(id = GetWindowWord( lpVideo->hWnd, GWW_ICONID )) )
			id = GET_WINDOW_ID( lpVideo->hWnd );
		Video_DumpShots(id, lpVideo->lpAllShots, lpVideo->iNumShots);

		// Save other elements, comments, etc.
		DumpOriginalRecs(id);

	}
	else if (id == IDC_GRABMODE)
	{
		static HTREEITEM hItem;
		HWND hTree = GetDlgItem(hWndDialog, IDC_SHOTLIST);
		if (!bGrabEventOn)
		{
			// Save current item to restore later
			hItem =  TreeView_GetSelection(hTree);
			SendMessage(hControl, WM_SETTEXT, 0, (LPARAM)"GRAB ON!");
		}
		else
		{
			// Restore tree view to current item
			TreeView_SelectItem(hTree, hItem);
			SendMessage(hControl, WM_SETTEXT, 0, (LPARAM)"Grab off");
		}

		bGrabEventOn = !bGrabEventOn;
	}

	else if (id == IDC_GRAB)
	{
		if (bGrabEventOn)
		{
			HWND hTree = GetDlgItem(hWndDialog, IDC_SHOTLIST);
			HTREEITEM hItem = TreeView_GetSelection(hTree);
			char szText[80];
			TV_ITEM Item;

			MCIStop( lpVideo->hDevice, YES/*bWait*/ );
			Item.hItem = hItem;
			Item.mask = TVIF_TEXT;
			Item.pszText = szText;
			Item.cchTextMax = 80;

			BOOL bRet = TreeView_GetItem(hTree, &Item);	
			if (bRet)
			{
				SendDlgItemMessage(hWndDialog, IDC_EVENTSHOT, WM_SETTEXT, 0, (LPARAM)szText);
			}
		}
	}
	else if (id == IDC_CELLTYPE)
	{
		if (codeNotify == CBN_SELENDOK)
		{
			if (lpCurShot)
			{
				int iCurSel = SendMessage(hControl, CB_GETCURSEL, 0, 0);
				if (iCurSel != CB_ERR)
				{
					if (iCurSel == 0)
					{
						// Turn on hex
						lpCurShot->lFlags |= FLAGBIT(F_HX);

						// Turn off others
						lpCurShot->lFlags &= ~(FLAGBIT(F_UJ));
					}
					else if (iCurSel == 1)
					{
						// Turn on union jack
						lpCurShot->lFlags |= FLAGBIT(F_UJ);

						// Turn off others
						lpCurShot->lFlags &= ~(FLAGBIT(F_HX));
					}
					else
					{
						// Turn off cell types
						lpCurShot->lFlags &= ~(FLAGBIT(F_HX));
						lpCurShot->lFlags &= ~(FLAGBIT(F_UJ));
					}
				}
			}
		}
	}
	else if (id == IDC_SHOTTYPE)
	{

		if (lpCurShot)
		{
			int iCurSel = SendMessage(hControl, CB_GETCURSEL, 0, 0);
			if (iCurSel != CB_ERR)
			{
				// Clear out current shottype flags
				lpCurShot->lFlags &= ~FMASK_SHOTTYPES;

				// Set current shot type flag
				int iBit = iCurSel + FSTART_SHOTTYPES - 1;
				lpCurShot->lFlags |= FLAGBIT(iBit);
			}
		}
	}
 
	else if (id == IDC_CELLPOSITION)
	{
		if (lpCurShot)
		{
			int iCurSel = SendMessage(hControl, CB_GETCURSEL, 0, 0);
			if (iCurSel != CB_ERR)
			{
				// Clear out current shottype flags
				lpCurShot->lFlags &= ~FMASK_CELLPOSITIONS;

				// Set current shot type flag
				int iBit = iCurSel + FSTART_CELLPOSITIONS - 1;
				lpCurShot->lFlags |= FLAGBIT(iBit);
			}
		}
	}
	else if (id == IDC_PLAY)
	{
		BOOL bReturn;
		DWORD dwMode = MCIGetMode( lpVideo->hDevice );
		if ( dwMode == MCI_MODE_PAUSE )
			bReturn = MCIResume( lpVideo->hDevice );
		else if ( dwMode == MCI_MODE_PLAY )
		{
			bReturn = MCIPause( lpVideo->hDevice );
			SetCurrentFrame();
			if (!bTestEvent && !bTestEvent2 && !bGrabEventOn)
			{
				// Save current frame for testing
				lCurrFrame = MCIGetPosition( lpVideo->hDevice );
			}

		}
		else
		{
			if (!bGrabEventOn)
			{
				// Play current shot
				BOOL bTranslated;
				long lFrom = GetDlgItemInt(hWnd, IDC_STARTFRAME, &bTranslated, FALSE);
				long lTo = GetDlgItemInt(hWnd, IDC_ENDFRAME, &bTranslated, FALSE);
				MCIPlay( lpVideo->hDevice, hWnd, lFrom, lTo );
			}

			// Else play 50 frames of whatever is up
			else
			{
				long lFrom = MCIGetPosition( lpVideo->hDevice );
				long lTo = lFrom + 50;
				MCIPlay(lpVideo->hDevice, hWnd, lFrom, lTo);
			}
		}
  	}
	else if (id == IDC_STEPNEXT)
	{
		MCIStepForward( lpVideo->hDevice, 1 );
		SetCurrentFrame();
	}
	else if (id == IDC_STEPPREV)
	{
		MCIStepReverse( lpVideo->hDevice );
		SetCurrentFrame();
	}
	else if (id == IDC_UPDATE)
	{
		LoadUpdate();
	}
	else if (id == IDC_TEST)
	{
		PlayTest();

	}
	else if (id == IDC_EXPORT)
	{
		ExportAlias();
		ExportShots();
	}

	else if (id == IDC_SETNAME)
	{
		//SetName(hWnd);
	}
	else if (id == IDC_SHOTSAVE)
	{
		SaveRecValues(hWnd);
		SaveEventShot();
	}
	else if (id == IDC_LOAD)
	{
		LoadTableFromText();
	}
	else if (id >= IDC_FIRST_LOOK && id <= IDC_LAST_LOOK)
	{
		AuthorOnSetCheck(id);
	}
	else if (id == IDOK)
	{
	}
	else if (id == IDC_SETSTARTRANGE)
	{
		long lFrame = MCIGetPosition( lpVideo->hDevice );
		lFrame -= lpCurShot->lStartFrame;
		SetDlgItemInt(hWndDialog, IDC_RANGESTART, (int)lFrame, TRUE);
	}
	else if (id == IDC_SETENDRANGE)
	{
		long lFrame = MCIGetPosition( lpVideo->hDevice );
		lFrame -= lpCurShot->lStartFrame;
		SetDlgItemInt(hWndDialog, IDC_RANGEEND,   (int)lFrame, TRUE);
	}

	/*
	else if (id == IDC_JUMPSHOT)
	{
		STRING szString;

		HWND hVideo = GetDlgItem( hSceneWindow, IDC_VIDEO );
		if (!hVideo)
			return;

		LPVIDEO lpVideo = (LPVIDEO)GetWindowLong(hVideo, GWL_DATAPTR);
		if (!lpVideo)
			return;

		GetDlgItemText(hWnd, IDC_SHOTNAME, szString, sizeof(szString));
  
		LPSHOT lpShot = Video_FindShotName( hVideo, szString );
		if ( !lpShot )
			return;

		Video_GotoShot( hVideo, lpVideo, lpShot->lShotID, 1, 0, 0 );
	}
*/

}

/***********************************************************************/
void AuthorOnNotify(HWND hWnd, int iCtrl, NMHDR FAR* pHdr)
/***********************************************************************/
{
	if (iCtrl == IDC_SHOTLIST)
	{
		NM_TREEVIEW *pNMView = (NM_TREEVIEW *)pHdr;

		// If there was a selection change
		if (pNMView->hdr.code == TVN_SELCHANGED)
		{
			// If not in grab mode
			if (!bGrabEventOn)
			{
				SetCurrentShotFromTree(pNMView);
				BOOL bTranslated;
				long lFrom = GetDlgItemInt(hWnd, IDC_STARTFRAME, &bTranslated, FALSE);
				long lTo = GetDlgItemInt(hWnd, IDC_ENDFRAME, &bTranslated, FALSE);
				// Show the 5th frame (get past the fog in most cases)
				if (lTo > (lFrom + 5))
					lTo = 5;
				else
					lTo = 0;
				MCIPlay(lpVideo->hDevice, hWnd, lFrom, lFrom+lTo);
			}

			// In grab mode
			else
			{
				// Get the current item 
				char szText[80];
				pNMView->itemNew.mask = TVIF_TEXT;
				pNMView->itemNew.pszText = szText;
				pNMView->itemNew.cchTextMax = 80;

				BOOL bRet = TreeView_GetItem(pNMView->hdr.hwndFrom, &pNMView->itemNew);	 
				if (bRet)
				{
					// Get shot
					LPSHOT lpShot = Video_FindShotName( lpVideo->hWnd, szText );
					if (lpShot)
					{
						// Play frames
						long lTo = lpShot->lStartFrame + 5;
						if (lTo > lpShot->lEndFrame)
							lTo = lpShot->lEndFrame;
						MCIPlay(lpVideo->hDevice, hWnd, lpShot->lStartFrame, lTo);
					}
				}
			}
		}
		else if (pNMView->hdr.code == TVN_BEGINDRAG)
		{
			bDragging = TRUE;
		}
	}

	else if (iCtrl == IDC_EVENTS)
	{
		if (pHdr->code == CBN_SELCHANGE)
		{
			int i=0;
		}
	}
	else if (iCtrl == IDC_EVENTSHOT)
	{
		bEventShotDirty = TRUE;
	}
}

/***********************************************************************/
BOOL AuthorOnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{			  
	return (FALSE);
}

/***********************************************************************/
BOOL AuthorOnSetCheck(int id)
/***********************************************************************/
{

	if (id == IDC_NORMAL || id == IDC_HELL || id == IDC_ICE)
	{
		char szName[80];
		char cLook = '0';

		GetDlgItemText(hWndDialog, IDC_SHOT, szName, sizeof(szName));

		if (id == IDC_HELL)
			cLook += 1;
		else if (id == IDC_ICE)
			cLook += 2;

		szName[2] = cLook;
		if (SetCurrentShotFromName(szName))
		{
			if (id != IDC_NORMAL)
				EnableWindow(GetDlgItem(hWndDialog, IDC_SHOTSAVE), FALSE);
			else
				EnableWindow(GetDlgItem(hWndDialog, IDC_SHOTSAVE), TRUE);
		}
	}
	return TRUE;
}

/***********************************************************************/
UINT AuthorOnMCINotify(HWND hwnd, UINT notifyCode, HMCI hDevice)
/***********************************************************************/
{
	if (notifyCode == MCI_NOTIFY_SUCCESSFUL)
	{
		SetCurrentFrame();

		// If we are in the middle of testing an event...
		if (bTestEvent)
		{
			bTestEvent = FALSE;
			bTestEvent2 = TRUE;
			// Play next sequence in the test
			MCIPlay( lpVideo->hDevice, hWndDialog, lTestEventFrom, lTestEventTo );

		}
		// Second half of test is done playing
		else if (bTestEvent2)
		{
			bTestEvent2 = FALSE;
		}
		else if (!bGrabEventOn)
		{
			// Save current frame for testing
			lCurrFrame = MCIGetPosition( lpVideo->hDevice );
		}
	}

	return 1;
}

/***********************************************************************/
BOOL WINPROC EXPORT AuthorDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGMSG(hDlg, WM_INITDIALOG, AuthorOnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, AuthorOnCommand);
		HANDLE_DLGMSG(hDlg, WM_NOTIFY, AuthorOnNotify);
		HANDLE_MSG(hDlg, MM_MCINOTIFY, AuthorOnMCINotify);
		//HANDLE_BM_SETCHECK(hDlg, wParam, lParam, AuthorOnSetCheck);
	default:
			return(AuthorOnMessage(hDlg, msg, wParam, lParam));
		break;
	}
	return(FALSE);
}

/***********************************************************************/
void StartLoadTree(HWND hWnd)
/***********************************************************************/
{
	HWND hTree;

		// Create HTREE Array for editing purposes
	if (phItems)
		FreeUp(phItems);

	phItems = (HTREEITEM *)AllocX( sizeof(HTREEITEM) * (lpVideo->iNumShots+1), GMEM_ZEROINIT);
	if (!phItems)
	{
		MessageBox(hWndDialog, "Could not allocate htreeitem arrach.", "Error", MB_ICONERROR);
	}

	hTree = GetDlgItem(hWnd, IDC_SHOTLIST);

	LoadTree(hTree, lpVideo);
}

/***********************************************************************/
void LoadTree(HWND hTree, LPVIDEO lpVideo)
/***********************************************************************/
{
	TV_INSERTSTRUCT TreeItem;
	char szKey[80];

		// Create HTREE Array for editing purposes
	if (phItems)
		FreeUp(phItems);

	phItems = (HTREEITEM *)AllocX( sizeof(HTREEITEM) * (lpVideo->iNumShots+1), GMEM_ZEROINIT);
	if (!phItems)
	{
		MessageBox(hWndDialog, "Could not allocate htreeitem arrach.", "Error", MB_ICONERROR);
	}

	TreeItem.hParent = NULL;
	TreeItem.hInsertAfter = TVI_LAST;
	TreeItem.item.mask = TVIF_TEXT;
  

	// Setup shot list hierarchy

	// Hex Cells
	TreeItem.item.pszText = "Hex Cells";
	TreeItem.item.cchTextMax = strlen(TreeItem.item.pszText);
	TreeItem.hParent = TreeView_InsertItem(hTree, &TreeItem);

	strcpy(szKey, "/H");
	LoadHexLooks(hTree, &TreeItem, szKey, lpVideo);

	// Union Jacks
	TreeItem.hParent = NULL;
	TreeItem.item.pszText = "Union Jacks";
	TreeItem.item.cchTextMax = strlen(TreeItem.item.pszText);
	TreeItem.hParent = TreeView_InsertItem(hTree, &TreeItem);

	strcpy(szKey, "/U");
	strcat(szKey, g_szLook);
	LoadUJGates(hTree, &TreeItem, szKey, lpVideo);
	//LoadShotNames(hTree, &TreeItem, szKey);

	// Safe Cells
	TreeItem.hParent = NULL;
	TreeItem.item.pszText = "Safe Cells";
	TreeItem.item.cchTextMax = strlen(TreeItem.item.pszText);
	TreeItem.hParent = TreeView_InsertItem(hTree, &TreeItem);

	strcpy(szKey, "/S");
	LoadShotNames(hTree, &TreeItem, szKey, lpVideo);

	// Tunnels
	TreeItem.hParent = NULL;
	TreeItem.item.pszText = "Tunnels";
	TreeItem.item.cchTextMax = strlen(TreeItem.item.pszText);
	TreeItem.hParent = TreeView_InsertItem(hTree, &TreeItem);

	strcpy(szKey, "/T");
	LoadShotNames(hTree, &TreeItem, szKey, lpVideo);

	// Barricades
	TreeItem.hParent = NULL;
	TreeItem.item.pszText = "Barricades";
	TreeItem.item.cchTextMax = strlen(TreeItem.item.pszText);
	TreeItem.hParent = TreeView_InsertItem(hTree, &TreeItem);

	strcpy(szKey, "/B");
	LoadShotNames(hTree, &TreeItem, szKey, lpVideo);

	// Cul-de-sacs
	TreeItem.hParent = NULL;
	TreeItem.item.pszText = "Cul-de-sacs";
	TreeItem.item.cchTextMax = strlen(TreeItem.item.pszText);
	TreeItem.hParent = TreeView_InsertItem(hTree, &TreeItem);

	strcpy(szKey, "/C");
	LoadShotNames(hTree, &TreeItem, szKey, lpVideo);

	//LoadCulDeSacs(

	// Player Death scenes
	TreeItem.hParent = NULL;
	TreeItem.item.pszText = "Death scenes";
	TreeItem.item.cchTextMax = strlen(TreeItem.item.pszText);
	TreeItem.hParent = TreeView_InsertItem(hTree, &TreeItem);

	strcpy(szKey, "/D");
	LoadShotNames(hTree, &TreeItem, szKey, lpVideo);

	// Other shots
	TreeItem.hParent = NULL;
	TreeItem.item.pszText = "Other";
	TreeItem.item.cchTextMax = strlen(TreeItem.item.pszText);
	TreeItem.hParent = TreeView_InsertItem(hTree, &TreeItem);
	LoadOtherShots(hTree, &TreeItem, lpVideo);
}

/***********************************************************************/
void LoadHexLooks(HWND hTree, TV_INSERTSTRUCT *pItem, char *pszKey, LPVIDEO lpVideo)
/***********************************************************************/
{
	HTREEITEM hParent = pItem->hParent;	// Make copy of parent
	char szKey[80];

	// Normal
	//pItem->item.pszText = "Normal Look";
	//pItem->item.cchTextMax = strlen(pItem->item.pszText);
	//pItem->hParent = TreeView_InsertItem(hTree, pItem);

	strcpy(szKey, pszKey);
	strcat(szKey, g_szLook);
	LoadHexPortals(hTree, pItem, szKey, lpVideo);

	/*** Note:	Only load the first look, the other looks will be accessed
				from the dialog using the normal look name
	// Hell
	pItem->item.pszText = "Hell";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);

	strcpy(szKey, pszKey);
	strcat(szKey, "1");
	LoadHexPortals(hTree, pItem, szKey);


	// Ice
	pItem->item.pszText = "Ice";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);

	strcpy(szKey, pszKey);
	strcat(szKey, "2");
	LoadHexPortals(hTree, pItem, szKey);
	***/	
	/***
	pItem->item.pszText = "Look 3";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);

	strcpy(szKey, pszKey);
	strcat(szKey, "3");
	LoadHexPortals(hTree, pItem, szKey);

	// Look 4
	pItem->item.pszText = "Look 4";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);

	strcpy(szKey, pszKey);
	strcat(szKey, "4");
	LoadHexPortals(hTree, pItem, szKey);

	// Look 5
	pItem->item.pszText = "Look 5";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);

	strcpy(szKey, pszKey);
	strcat(szKey, "5");
	LoadHexPortals(hTree, pItem, szKey);
	***/

}

/***********************************************************************/
void LoadHexPortals(HWND hTree, TV_INSERTSTRUCT *pItem, char *pszKey, LPVIDEO lpVideo)
/***********************************************************************/
{
	HTREEITEM hParent = pItem->hParent;	// Make copy of parent
	char szKey[80];

	pItem->item.pszText = "Green Portal";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);
	strcpy(szKey, pszKey);
	strcat(szKey, "1");
	LoadShotNames(hTree, pItem, szKey, lpVideo);

	pItem->item.pszText = "Green - Yellow Portal";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);
	strcpy(szKey, pszKey);
	strcat(szKey, "2");
	LoadShotNames(hTree, pItem, szKey, lpVideo);

	pItem->item.pszText = "Yellow Portal";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);
	strcpy(szKey, pszKey);
	strcat(szKey, "3");
	LoadShotNames(hTree, pItem, szKey, lpVideo);

	pItem->item.pszText = "Yellow - Green Portal";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);
	strcpy(szKey, pszKey);
	strcat(szKey, "4");
	LoadShotNames(hTree, pItem, szKey, lpVideo);

}

/***********************************************************************/
void LoadUJGates(HWND hTree, TV_INSERTSTRUCT *pItem, char *pszKey, LPVIDEO lpVideo)
/***********************************************************************/
{
	HTREEITEM hParent = pItem->hParent;	// Make copy of parent
	char szKey[80];

	pItem->item.pszText = "between red and green (green on top)";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);
	strcpy(szKey, pszKey);
	strcat(szKey, "1");
	LoadShotNames(hTree, pItem, szKey, lpVideo);

	pItem->item.pszText = "Green gate";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);
	strcpy(szKey, pszKey);
	strcat(szKey, "2");
	LoadShotNames(hTree, pItem, szKey, lpVideo);

	pItem->item.pszText = "between Green and Yellow";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);
	strcpy(szKey, pszKey);
	strcat(szKey, "3");
	LoadShotNames(hTree, pItem, szKey, lpVideo);

	pItem->item.pszText = "Yellow Gate";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);
	strcpy(szKey, pszKey);
	strcat(szKey, "4");
	LoadShotNames(hTree, pItem, szKey, lpVideo);

	pItem->item.pszText = "between Yellow and Red";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);
	strcpy(szKey, pszKey);
	strcat(szKey, "5");

	LoadShotNames(hTree, pItem, szKey, lpVideo);
	pItem->item.pszText = "Red Gate";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);
	strcpy(szKey, pszKey);
	strcat(szKey, "6");

	LoadShotNames(hTree, pItem, szKey, lpVideo);
	pItem->item.pszText = "between red and green (red on top)";
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	pItem->hParent = TreeView_InsertItem(hTree, pItem);
	strcpy(szKey, pszKey);
	strcat(szKey, "7");
	LoadShotNames(hTree, pItem, szKey, lpVideo);
}

void LoadShotNames(HWND hTree, TV_INSERTSTRUCT *pItem, char *pszKey, LPVIDEO lpVideo)
{
	HTREEITEM hParent = pItem->hParent;	// Make copy of parent
	char szKey[80];
	int iNum = lpVideo->iNumShots;
	int i;
	int iLen = strlen(pszKey);
	LPSHOT lpShots = lpVideo->lpAllShots;

	// Make copy of key
	strcpy(szKey, pszKey);

	// Loop through all the shot names
	for( i=0; i<iNum; i++ )
	{
		// See what names match the key
		if (strncmp(pszKey, lpShots[i].szShotName, iLen) == 0)
		{
			// Put them in the tree
			pItem->item.pszText = lpShots[i].szShotName;
			pItem->item.cchTextMax = strlen(lpShots[i].szShotName);
			phItems[lpShots[i].lShotID] = TreeView_InsertItem(hTree, pItem);
			lpShots[i].fEditFlag |= EDIT_FLAG_INTREE;
		}
	}
}

void SetCurrentShotFromTree(NM_TREEVIEW FAR *pView)
{
	LPSHOT lpShot;
	char szText[80];

	MCIStop( lpVideo->hDevice, YES/*bWait*/ );
	pView->itemNew.mask = TVIF_TEXT;
	pView->itemNew.pszText = szText;
	pView->itemNew.cchTextMax = 80;

	BOOL bRet = TreeView_GetItem(pView->hdr.hwndFrom, &pView->itemNew);	
 
	// Get shot
	if (bRet)
	{
		lpShot = Video_FindShotName( lpVideo->hWnd, pView->itemNew.pszText );
		if (lpShot)
		{
			// Set shot name
			SetShotName(hWndDialog, pView->itemNew.pszText);

			// Set event data
			// IDC_EVENTS - combo box
			SendDlgItemMessage(hWndDialog, IDC_EVENTS, CB_SETCURSEL, 0, 0);
	
			// IDC_EVENTSHOT - edittext

			// Set start frame
			SetDlgItemInt(hWndDialog, IDC_STARTFRAME, lpShot->lStartFrame, FALSE); 

			// Set end frame
			SetDlgItemInt(hWndDialog, IDC_ENDFRAME, lpShot->lEndFrame, FALSE);

			// Set comment
			SetDlgItemText(hWndDialog, IDC_DESCRIPTION, COMMENT(lpShot->lShotID-1));

			// Set look button to normal
			SendDlgItemMessage(hWndDialog, IDC_NORMAL, BM_SETCHECK, BST_CHECKED, 0);
			EnableWindow(GetDlgItem(hWndDialog, IDC_SHOTSAVE), TRUE);

			// Set event ranges
			SetDlgItemInt(hWndDialog, IDC_RANGESTART, lpShot->EventRange.nStart, TRUE);
			SetDlgItemInt(hWndDialog, IDC_RANGEEND,   lpShot->EventRange.nEnd,   TRUE);

		}
	}

	// Set current shot
	lpCurShot = lpShot;
	if (lpShot)
		Video_GotoShot( lpVideo->hWnd, lpVideo, lpShot->lShotID, 1/*lCount*/, 0/*iFrames*/, 0 );

	SetEventShot(0);

	SetFlags();
}

BOOL SetCurrentShotFromName(LPSTR lpszName)
{
	LPSHOT lpShot = Video_FindShotName( lpVideo->hWnd, lpszName );
	if (lpShot)
	{
		HWND hWnd;

		// Set shot name
		hWnd = GetDlgItem(hWndDialog, IDC_SHOT);
		SetWindowText(hWnd, lpszName);

		// Set event data
		// IDC_EVENTS - combo box
		SendDlgItemMessage(hWnd, IDC_EVENTS, CB_SETCURSEL, 0, 0);

		// IDC_EVENTSHOT - edittext

		// Set start frame
		SetDlgItemInt(hWndDialog, IDC_STARTFRAME, lpShot->lStartFrame, FALSE); 

		// Set end frame
		SetDlgItemInt(hWndDialog, IDC_ENDFRAME, lpShot->lEndFrame, FALSE);

		// Set comment
		SetDlgItemText(hWndDialog, IDC_DESCRIPTION, COMMENT(lpShot->lShotID-1));

		// Set current shot
		lpCurShot = lpShot;
		if (lpShot)
			Video_GotoShot( lpVideo->hWnd, lpVideo, lpShot->lShotID, 1/*lCount*/, 0/*iFrames*/, 0 );

		SetEventShot(0);

		SetFlags();

		// Set event ranges
		SetDlgItemInt(hWndDialog, IDC_RANGESTART, lpShot->EventRange.nStart, TRUE);
		SetDlgItemInt(hWndDialog, IDC_RANGEEND,   lpShot->EventRange.nEnd,   TRUE);

		long lFrom = lpShot->lStartFrame;
		long lTo = lpShot->lEndFrame; 
		// Show the 5th frame (get past the fog in most cases)
		if (lTo > (lFrom + 5))
			lTo = 5;
		else
			lTo = 0;
		MCIPlay(lpVideo->hDevice, hWnd, lFrom, lFrom+lTo);
		return TRUE;
	}
	return FALSE;
}

void SetEventShot(int iCurSel)
{
	SHOTID lShotId;

	if (!lpCurShot)
		return;

	switch(iCurSel)
	{

	// End
	case 0:
		lShotId = lpCurShot->lEndShot;
		break;

	// Left
	case 1:
		lShotId = lpCurShot->lLeftShot;
		break;

	// Right
	case 2:
		lShotId = lpCurShot->lRightShot;
		break;

	// Down
	case 3:
		lShotId = lpCurShot->lDownShot;
		break;
	}

	--lShotId;

	if ( lShotId < 0 || lShotId > lpVideo->iNumShots )
		SendDlgItemMessage(hWndDialog, IDC_EVENTSHOT, WM_SETTEXT, 0, (LPARAM)(char *)"");
	else
	{
		if ( lShotId == (lpCurShot->lShotID-1))
			SendDlgItemMessage(hWndDialog, IDC_EVENTSHOT, WM_SETTEXT, 0, 
				(LPARAM)(char *)"/this");
		else
			SendDlgItemMessage(hWndDialog, IDC_EVENTSHOT, WM_SETTEXT, 0, 
				(LPARAM)(char *)lpVideo->lpAllShots[lShotId].szShotName);

	}

}

void SaveEventShot(void)
{

	// We at least tried to save, so set dirty to false
	bEventShotDirty = FALSE;

	if (!lpCurShot)
		return;

	int iCurSel = SendDlgItemMessage(hWndDialog, IDC_EVENTS, CB_GETCURSEL, 0, 0);
	if (iCurSel == CB_ERR)
		return;

	// Get text for the event shot
	char szEvent[120];
	SendDlgItemMessage(hWndDialog, IDC_EVENTSHOT, WM_GETTEXT, 119, (LPARAM)szEvent);

	switch(iCurSel)
	{

	// End
	case 0:
		if ( !lstrcmpi( szEvent, "/this" ) )
			lpCurShot->lEndShot = lpCurShot->lShotID;
		else	
			lpCurShot->lEndShot = FindShot(szEvent);
		break;

	// Left
	case 1:
		lpCurShot->lLeftShot  = FindShot(szEvent);
		break;

	// Right
	case 2:
		lpCurShot->lRightShot  = FindShot(szEvent);
		break;

	// Down
	case 3:
		lpCurShot->lDownShot  = FindShot(szEvent);
		break;
	}
}

void SetFlags(void)
{
	if (!lpCurShot)
		return;
	
	int i;
	long lFlags = lpCurShot->lFlags;

	// Set cell type
	if (lFlags & FLAGBIT(F_HX))
		i=0;
	else if (lFlags & FLAGBIT(F_UJ))
		i=1;
	else
		i=2;

	SendDlgItemMessage(hWndDialog, IDC_CELLTYPE, CB_SETCURSEL, i, 0);

	// Set shot type
	if (lFlags & FLAGBIT(F_SPINE_HIT))
		i = 1;
	else if (lFlags & FLAGBIT(F_SPINE_DEAD))
		i = 2;
	else if (lFlags & FLAGBIT(F_NEWCELL))
		i = 3;
	else if (lFlags & FLAGBIT(F_YOU_LOSE))
		i = 4;
	else if (lFlags & FLAGBIT(F_YOU_WIN))
		i = 5;
	else if (lFlags & FLAGBIT(F_EXIT))
		i = 6;
	else if (lFlags & FLAGBIT(F_HIT))
		i = 7;
	else if (lFlags & FLAGBIT(F_AUTOREVERSE))
		i = 8;
	else if (lFlags & FLAGBIT(F_LIVE_DIE))
		i = 9;
	else
		i = 0;

	SendDlgItemMessage(hWndDialog, IDC_SHOTTYPE, CB_SETCURSEL, i, 0);

	// Set shot cell position
	if (lFlags & FLAGBIT(F_POS_BEG))
		i=1;
	else if (lFlags & FLAGBIT(F_POS_MID))
		i=2;
	else if (lFlags & FLAGBIT(F_POS_END))
		i=3;
	else if (lFlags & FLAGBIT(F_POS_FULL))
		i=4;
	else if (lFlags & FLAGBIT(F_POS_FULLC))
		i=5;
	else if (lFlags & FLAGBIT(F_POS_WALL))
		i=6;
	else if (lFlags & FLAGBIT(F_POS_VEER))
		i=7;
	else if (lFlags & FLAGBIT(F_POS_BEG_3QTR))
		i=8;
	else
		i=0;

	SendDlgItemMessage(hWndDialog, IDC_CELLPOSITION, CB_SETCURSEL, i, 0);
}

void LoadUpdate(void)
{
	OPENFILENAME ofn;
	_fmemset(&ofn, 0, sizeof(OPENFILENAME));
	int iTableSize = 0;
	STRING szFileName;
	EXCEL_ORIG Rec;
	char szTemp[nMaxIn];

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWndDialog;
	//ofn.lpstrFilter = szFilter;
	//ofn.nFilterIndex = 1;
	szFileName[0] = NULL;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	//ofn.lpstrInitialDir = lpPathName;
	ofn.lpstrTitle = "Load Exported Excel Update File (tab seperated)"; // title of the dialog - "Open" by default
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (!GetOpenFileName(&ofn))
	{
		DWORD dwRet = CommDlgExtendedError();
		return;
	}

	ifstream in;

	in.open(szFileName);

	if (in.bad())
		return;

	char *psz1;
	char *psz2;
	int   nCmpStart = 0;
	LPSHOT lpShot;

	int iNextShot = lpVideo->iNumShots;
	int iNumShots = iNextShot;
	const int iAllocBlock = 16;

	// Allocate initial comment array
	if (lpOrig)
		FreeUp(lpOrig);
	lpOrig = (EXCEL_ORIG *)AllocX( sizeof(EXCEL_ORIG) * lpVideo->iNumShots, GMEM_ZEROINIT);
	if (lpOrig == NULL)
	{
		MessageBox(hWndDialog, "Could not allocate memory for original excel data.", "Error", MB_ICONERROR);
		return;
	}

	// Remove first line
	char szLine[1024];
	in.getline(szLine, 1024);

	// while there are more records
	while( !in.eof() )
	{
		// Get the shot name
		in.eatwhite();
		in.get( Rec.szNewName, 80, '\t');
		in.ignore(nMaxIn, '\t');
		//OutputDebugString(szName);
		//OutputDebugString("\n");

		in.get(Rec.szOldName, 40, '\t');
		in.ignore(nMaxIn, '\t');

		in.get(Rec.szF3, 20, '\t');
		in.ignore(nMaxIn, '\t');

		// Get the description text
		//in.eatwhite();
		//if (in.peek() != '\t')
		in.get(Rec.szDescr, 120, '\t');
		in.ignore(nMaxIn, '\t');

		in.get(Rec.szF5, 30, '\t');
		in.ignore(nMaxIn, '\t');
		in.get(Rec.szF6, 15, '\t');
		in.ignore(nMaxIn, '\t');
		in.get(Rec.szF7, 15, '\t');
		in.ignore(nMaxIn, '\t');
		in.get(Rec.szF8, 15, '\t');
		in.ignore(nMaxIn, '\t');
		in.get(Rec.szF9, 15, '\t');
		in.ignore(nMaxIn, '\t');
		in.get(Rec.szF10, 15, '\t');
		in.ignore(nMaxIn, '\t');
		in.get(Rec.szF11, 15, '\t');
		in.ignore(nMaxIn, '\t');

		// The start frame and the end frame
		in.eatwhite();
		in.get( szTemp, nMaxIn, '\t');
		in.ignore(nMaxIn, '\t');

		Rec.lStartFrame = atol(szTemp);

		in.eatwhite();
		in.get( szTemp, nMaxIn, '\t');
		in.ignore(nMaxIn, '\n');

		Rec.lEndFrame = atol(szTemp);

		// Look for it in the table

		// If hex cell or union jack
		for(int i=0; i<lpVideo->iNumShots; i++)
		{
			lpShot = &lpVideo->lpAllShots[i];

			// If hex cell or union jack
			if ((Rec.szNewName[1] == 'H' && lpShot->szShotName[1] == 'H') ||
				(Rec.szNewName[1] == 'U' && lpShot->szShotName[1] == 'U'))
			{
				// NOTE: For this first sweep we will look for the old name and, if found, 
				// convert it to the new name
				if (Rec.szNewName[2] == lpShot->szShotName[2])
				{
					psz1 = &Rec.szNewName[12];
					if (strlen(lpShot->szShotName) < 8)
						continue;

					psz2 = &lpShot->szShotName[6];
					if (stricmp(psz1, psz2) == 0 )
					{
						// FOUND!!!

						//char szOut[100];
						//wsprintf(szOut, "<%s> --> <%s>\n", lpShot->szShotName, Rec.szNewName);
						//OutputDebugString(szOut);

						// Delete the old name atom
						DeleteAtom(lpShot->aShotID);

						// Replace the name
						strncpy( lpShot->szShotName, Rec.szNewName, MAX_SHOTNAME_LEN-1);
						lpShot->szShotName[MAX_SHOTNAME_LEN-1] = NULL;

						// Create a new name atom
						lpShot->aShotID = AddAtom(lpShot->szShotName);

						// Copy record
						memcpy( &lpOrig[i], &Rec, sizeof(EXCEL_ORIG) );
						break;
					}
				}
			}
		}

		// If not in the list then add it to the table	
		if (i == lpVideo->iNumShots)
		{
			// Make sure there is a slot avaible
			if (iNextShot == iNumShots)
			{
				iNumShots += iAllocBlock;
				lpVideo->lpAllShots = (LPSHOT)AllocExtend( (LPTR)lpVideo->lpAllShots, 
					sizeof(SHOT) * iNumShots);
				if (lpVideo->lpAllShots == NULL)
				{
					MessageBox(hWndDialog, "Could not re-allocate shot table!", "Error", MB_ICONERROR);
					goto ErrorCleanUp;
				}
				lpOrig = (EXCEL_ORIG *)AllocExtend((LPTR)lpOrig, sizeof(EXCEL_ORIG) * iNumShots);
			}

			// Add the record
			lpShot = &lpVideo->lpAllShots[iNextShot];

			// Copy record
			memcpy( &lpOrig[i], &Rec, sizeof(EXCEL_ORIG) );
	
			strncpy( lpShot->szShotName, Rec.szNewName, MAX_SHOTNAME_LEN-1);
			lpShot->lShotID = iNextShot+1;
			lpShot->aShotID = AddAtom(lpShot->szShotName);
			lpShot->lStartFrame = Rec.lStartFrame;
			lpShot->lEndFrame = Rec.lEndFrame;

			// Position index for next entry
			++iNextShot;
		}
	}

	// Resize the shot table to the exact size
	if (iNextShot < iNumShots)
	{
		lpVideo->lpAllShots = (LPSHOT)AllocExtend( (LPTR)lpVideo->lpAllShots, 
			sizeof(SHOT) * iNextShot);
		if (lpVideo->lpAllShots == NULL)
		{
			MessageBox(hWndDialog, "Could not re-allocate shot table!", "Error", MB_ICONERROR);
			goto ErrorCleanUp;
		}
		lpVideo->iNumShots = iNextShot;

		lpOrig = (EXCEL_ORIG *)AllocExtend( (LPTR)lpOrig, sizeof(EXCEL_ORIG) * iNextShot );
	}

ErrorCleanUp:

	in.close();

	// Clear the list and rebuild it off the new table
	HWND hTree = GetDlgItem(hWndDialog, IDC_SHOTLIST);
	if (hTree)
	{
		TreeView_DeleteAllItems(hTree);
		StartLoadTree(hWndDialog);
	}
}


void LoadTableFromText(void)
{
	OPENFILENAME ofn;
	_fmemset(&ofn, 0, sizeof(OPENFILENAME));
	int iTableSize = 0;
	STRING szFileName;

	// Disable video for the moment
	Video_Close( lpVideo->hWnd, lpVideo );

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWndDialog;
	//ofn.lpstrFilter = szFilter;
	//ofn.nFilterIndex = 1;
	szFileName[0] = NULL;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	//ofn.lpstrInitialDir = lpPathName;
	ofn.lpstrTitle = "Load Exported Excel Alias Text File (tab seperated)"; // title of the dialog - "Open" by default
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (!GetOpenFileName(&ofn))
	{
		DWORD dwRet = CommDlgExtendedError();
		return;
	}

	// Get rid of old shot table
	if (lpVideo->lpAllShots)
		FreeUp(lpVideo->lpAllShots);

	// Allocate new shot table
	lpVideo->iNumShots = 0;
	lpVideo->lpAllShots = NULL;

	int iNumShots = 300;
	LPSHOT lpShots = (LPSHOT)AllocX( iNumShots * sizeof(SHOT), GMEM_ZEROINIT);

	LoadAliasFromText(szFileName, &lpShots, &iNumShots);

	szFileName[0] = NULL;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	//ofn.lpstrInitialDir = lpPathName;
	ofn.lpstrTitle = "Load Exported Excel Shot Text File (tab seperated)"; // title of the dialog - "Open" by default
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (!GetOpenFileName(&ofn))
	{
		DWORD dwRet = CommDlgExtendedError();
		return;
	}

	LoadShotsFromText(szFileName, &lpShots, &iNumShots);

	lpVideo->lpAllShots = lpShots;
	lpVideo->iNumShots = iNumShots;

	Video_RemapShotTable(lpVideo->hWnd);

	Video_Open( lpVideo->hWnd, lpVideo, 1, 0);

	// Clear the list and rebuild it off the new table
	HWND hTree = GetDlgItem(hWndDialog, IDC_SHOTLIST);
	if (hTree)
	{
		TreeView_DeleteAllItems(hTree);
		StartLoadTree(hWndDialog);
	}

	return;

}

void LoadAliasFromText(char *szFileName, LPSHOT *lpShots, int *piNumShots)
{
	int iTableSize = 0;
	char szTemp[nMaxIn];
	LPSHOT lpShot;

	int nCount = 0;
	int nNumSlots = *piNumShots;

	ifstream in;

	in.open(szFileName);

	if (in.bad())
		return;

	// Skip first line
	in.ignore(nMaxIn, '\t');
	in.ignore(nMaxIn, '\t');
	in.ignore(nMaxIn, '\t');
	in.ignore(nMaxIn, '\n');

	//out << "Alias\t" << "Shot\t" << "Count\t" << "Index\t\n";

	while ( !in.eof() )
	{
		lpShot = &((*lpShots)[nCount]);

		// shot name
		in.eatwhite();
		in.get(lpShot->szShotName, MAX_SHOTNAME_LEN, '\t');
		in.ignore(nMaxIn, '\t');
		
		// shot id
		//lpShot->lShotID = Count+1;

		// atom
		lpShot->aShotID = (ATOM)MakeShotID(lpShot->szShotName);
		lpShot->lShotID = (SHOTID)lpShot->aShotID;
	
		// Alias
		in.get(szTemp, sizeof(szTemp), '\t');
		in.ignore(nMaxIn, '\t');	
		lpShot->lEndShot = MakeShotID(szTemp);

		// count
		in.get(szTemp, sizeof(szTemp), '\t');
		in.ignore(nMaxIn, '\t');
		lpShot->lEndCount = atol(szTemp);

		// Index
		in.get(szTemp, sizeof(szTemp), '\n');
		in.ignore(nMaxIn, '\n');
		lpShot->lRegNum = atol(szTemp);

		++nCount;

		if (nCount == nNumSlots)
		{

			nNumSlots += 50;

			*lpShots = (LPSHOT)AllocExtend( (LPTR)*lpShots, 
				sizeof(SHOT) * nNumSlots);

			if (*lpShots == NULL)
			{
				MessageBox(hWndDialog, "Could not re-allocate shot table!", "Error", MB_ICONERROR);
				goto ErrorCleanUp;
			}
		}
	}

	*piNumShots = nCount;

	*lpShots = (LPSHOT)AllocExtend( (LPTR)*lpShots, 
		sizeof(SHOT) * nCount);

	if (*lpShots == NULL)
	{
		MessageBox(hWndDialog, "Could not re-allocate shot table!", "Error", MB_ICONERROR);
		goto ErrorCleanUp;
	}

	ErrorCleanUp:

	in.close();
}


void LoadShotsFromText(char *szFileName, LPSHOT *lpShots, int *piNumShots)
{
	int iTableSize = 0;
	EXCEL_ORIG Rec;
	char szTemp[nMaxIn];

	ifstream in;

	in.open(szFileName);

	if (in.bad())
		return;

	int   nCmpStart = 0;
	LPSHOT lpShot;

	int iNextShot = *piNumShots;
	int iNumShots = iNextShot;
	const int iAllocBlock = 50;

	// Allocate initial comment array
	if (lpOrig)
		FreeUp(lpOrig);
	lpOrig = (EXCEL_ORIG *)AllocX( sizeof(EXCEL_ORIG) * *piNumShots, GMEM_ZEROINIT);
	if (lpOrig == NULL)
	{
		MessageBox(hWndDialog, "Could not allocate memory for original excel data.", "Error", MB_ICONERROR);
		return;
	}

	// Remove first line
	char szLine[512];
	in.getline(szLine, 512);

	// while there are more records
	while( !in.eof() )
	{

		// Make sure there is a slot avaible
		if (iNextShot == iNumShots)
		{
			iNumShots += iAllocBlock;
			*lpShots = (LPSHOT)AllocExtend( (LPTR)*lpShots, 
				sizeof(SHOT) * iNumShots);
			if (*lpShots == NULL)
			{
				MessageBox(hWndDialog, "Could not re-allocate shot table!", "Error", MB_ICONERROR);
				goto ErrorCleanUp;
			}
			lpOrig = (EXCEL_ORIG *)AllocExtend((LPTR)lpOrig, sizeof(EXCEL_ORIG) * iNumShots);
		}

		lpShot = &((*lpShots)[iNextShot]);

		// Get the shot name
		in.eatwhite();
		in.get( Rec.szNewName, 80, '\t');
		in.ignore(nMaxIn, '\t');
		//OutputDebugString(szName);
		//OutputDebugString("\n");

		in.get(Rec.szOldName, 40, '\t');
		in.ignore(nMaxIn, '\t');

		in.get(Rec.szF3, 20, '\t');
		in.ignore(nMaxIn, '\t');

		// Get the description text
		//in.eatwhite();
		//if (in.peek() != '\t')
		in.get(Rec.szDescr, 120, '\t');
		in.ignore(nMaxIn, '\t');

		in.get(Rec.szF5, 30, '\t');
		in.ignore(nMaxIn, '\t');
		in.get(Rec.szF6, 15, '\t');
		in.ignore(nMaxIn, '\t');
		in.get(Rec.szF7, 15, '\t');
		in.ignore(nMaxIn, '\t');
		in.get(Rec.szF8, 15, '\t');
		in.ignore(nMaxIn, '\t');
		in.get(Rec.szF9, 15, '\t');
		in.ignore(nMaxIn, '\t');
		in.get(Rec.szF10, 15, '\t');
		in.ignore(nMaxIn, '\t');
		in.get(Rec.szF11, 15, '\t');
		in.ignore(nMaxIn, '\t');

		// The start frame and the end frame
		in.eatwhite();
		in.get( szTemp, nMaxIn, '\t');
		in.ignore(nMaxIn, '\t');

		Rec.lStartFrame = atol(szTemp);

		in.eatwhite();
		in.get( szTemp, nMaxIn, '\t');
		in.ignore(nMaxIn, '\t');

		Rec.lEndFrame = atol(szTemp);

		// Copy original excel data record
		memcpy( &lpOrig[iNextShot], &Rec, sizeof(EXCEL_ORIG) );

		strncpy( lpShot->szShotName, Rec.szNewName, MAX_SHOTNAME_LEN-1);
		//lpShot->lShotID = iNextShot+1;
		lpShot->aShotID = AddAtom(lpShot->szShotName);
		lpShot->lShotID = (SHOTID)lpShot->aShotID;
		lpShot->lStartFrame = Rec.lStartFrame;
		lpShot->lEndFrame = Rec.lEndFrame;

		// Disk number
		in.get( szTemp, nMaxIn, '\t');
		in.ignore(nMaxIn, '\t');
		lpShot->wDisk = atoi(szTemp);

		// End
		in.get( szTemp, nMaxIn, '\t');
		in.ignore(nMaxIn, '\t');
		if (szTemp[0])
		{
		if ( !lstrcmpi( szTemp, "/this" ) )
			lpShot->lEndShot = lpShot->lShotID;
		else
			lpShot->lEndShot = MakeShotID(szTemp);
		}

		// Left
		in.get( szTemp, nMaxIn, '\t');
		in.ignore(nMaxIn, '\t');
		if (szTemp[0])
			lpShot->lLeftShot = MakeShotID(szTemp);

		// Right 
		in.get( szTemp, nMaxIn, '\t');
		in.ignore(nMaxIn, '\t');
		if (szTemp[0])
			lpShot->lRightShot = MakeShotID(szTemp);

		// Down
		in.get( szTemp, nMaxIn, '\t');
		in.ignore(nMaxIn, '\t');
		if (szTemp[0])
			lpShot->lDownShot = MakeShotID(szTemp);

		// Home
		in.get( szTemp, nMaxIn, '\t');
		in.ignore(nMaxIn, '\t');
		if (szTemp[0])
			lpShot->lHomeShot = MakeShotID(szTemp);

		// Flags
		in.get( szTemp, nMaxIn, '\t');
		in.ignore(nMaxIn, '\t');
		if (szTemp[0])
			lpShot->lFlags  = atol(szTemp);
		
		// Value
		in.get( szTemp, nMaxIn, '\t');
		in.ignore(nMaxIn, '\t');
		if (szTemp[0])
			lpShot->lValue  = atol(szTemp);

		// Event range start frame
		in.get( szTemp, nMaxIn, '\t');
		in.ignore(nMaxIn, '\t');
		if (szTemp[0])
			lpShot->EventRange.nStart = atoi(szTemp);

		// Event range end frame
		in.get( szTemp, nMaxIn, '\n');
		if (szTemp[0])
			lpShot->EventRange.nEnd = atoi(szTemp);

		// Position index for next entry
		++iNextShot;
	}

	// Resize the shot table to the exact size
	if (iNextShot < iNumShots)
	{
		*lpShots = (LPSHOT)AllocExtend( (LPTR)*lpShots, 
			sizeof(SHOT) * iNextShot);
		if (*lpShots == NULL)
		{
			MessageBox(hWndDialog, "Could not re-allocate shot table!", "Error", MB_ICONERROR);
			goto ErrorCleanUp;
		}
		*piNumShots = iNextShot;

		lpOrig = (EXCEL_ORIG *)AllocExtend( (LPTR)lpOrig, sizeof(EXCEL_ORIG) * iNextShot );
	}

ErrorCleanUp:

	in.close();
}

void ReplaceShotId( SHOTID NewId, SHOTID OldId )
{
	for( int i=0; i<lpVideo->iNumShots; i++ )
	{
		
	}
}


void ClearEditFlag(void)
{
	for (int i=0; i<lpVideo->iNumShots; i++)
	{
		lpVideo->lpAllShots[i].fEditFlag = EDIT_FLAG_NOUSED;
	}
}

void LoadOtherShots(HWND hTree, TV_INSERTSTRUCT *pItem, LPVIDEO lpVideo)
{
	int iNum = lpVideo->iNumShots;
	int i;
	LPSHOT lpShots = lpVideo->lpAllShots;

	for( i=0; i<iNum; i++ )
	{
		if (!(lpShots[i].fEditFlag & EDIT_FLAG_INTREE))
		{
			pItem->item.pszText = lpShots[i].szShotName;
			pItem->item.cchTextMax = strlen(lpShots[i].szShotName);
			phItems[lpShots[i].lShotID] = TreeView_InsertItem(hTree, pItem);
			lpShots[i].fEditFlag |= EDIT_FLAG_INTREE;
		}
	}
}

void PlayTest(void)
{
	char szName[80];

	if (!lpCurShot)
		return;

	// Get test shot
	GetDlgItemText(hWndDialog, IDC_EVENTSHOT, szName, sizeof(szName));

	LPSHOT lpNewShot = Video_FindShotName( lpVideo->hWnd, szName );
	if (lpNewShot)
	{
		int iCurSel = SendDlgItemMessage(hWndDialog, IDC_EVENTS, CB_GETCURSEL, 0, 0);
		if (iCurSel == 0)
			lpNewShot->iLastEvent = EVENT_END;
		else if (iCurSel == 1)
			lpNewShot->iLastEvent = EVENT_LEFT;
		else if (iCurSel == 2)
			lpNewShot->iLastEvent = EVENT_RIGHT;
		else if (iCurSel == 3)
			lpNewShot->iLastEvent = EVENT_DOWN;

		if (App.pGame)
			App.pGame->IndentShot( lpVideo->hWnd, lpNewShot, lpCurShot );

		// Backup up 10 frames of current shot and play to the current position
		long lTo = lCurrFrame;
		long lFrom = lTo - 10;
		if (lFrom < lpCurShot->lStartFrame)
			lFrom = lpCurShot->lStartFrame;

		// Set up the event frames
		lTestEventFrom = lpNewShot->lStartFrame + lpNewShot->lIndentFrames;
		lTestEventTo = lTestEventFrom + 10;
		if (lTestEventTo > lpNewShot->lEndFrame)
			lTestEventTo = lpNewShot->lEndFrame;

		// Play first part of test sequence, second part will be done in the
		// AuthorOnMCINotify()
		bTestEvent = TRUE;
		MCIPlay( lpVideo->hDevice, hWndDialog, lFrom, lTo );
	}
}

/***********************************************************************/
BOOL IsAlias(LPSHOT lpShot)
/***********************************************************************/
{
	if ( !lpShot->lStartFrame && !lpShot->lEndFrame && !lpShot->wDisk)
		return TRUE;
	else 
		return FALSE;

}

/***********************************************************************/
void ExportAlias(void)
/***********************************************************************/
{
	LPSHOT lpShot;
	LPSHOT lpAliasShot;

	OPENFILENAME ofn;
	_fmemset(&ofn, 0, sizeof(OPENFILENAME));
	const int nMaxIn = 120;
	STRING szFileName;


	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWndDialog;
	//ofn.lpstrFilter = szFilter;
	//ofn.nFilterIndex = 1;
	szFileName[0] = NULL;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	//ofn.lpstrInitialDir = lpPathName;
	ofn.lpstrTitle = "Export Alias in Tabbed format for Excel"; // title of the dialog - "Open" by default
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	if (!GetOpenFileName(&ofn))
	{
		DWORD dwRet = CommDlgExtendedError();
		return;
	}

	ofstream out;

	out.open(szFileName, ios::trunc);

	if (out.bad())
		return;

	out << "Alias\t" << "Shot\t" << "Count\t" << "Index\t\n";

	for( int i=0; i < lpVideo->iNumShots; i++ )
	{
		lpShot = &lpVideo->lpAllShots[i];

		if ( IsAlias(lpShot) )
		{
			// Output shot name
			out << lpShot->szShotName << '\t';

			// Get aliased name
			if ( lpShot->lEndShot > 0 && lpShot->lEndShot < lpVideo->iNumShots )
			{
				lpAliasShot = &lpVideo->lpAllShots[lpShot->lEndShot-1];
				out << lpAliasShot->szShotName;
			}
			else
				out << "invalid alias index";

			out << '\t';

			//out << lpAliasShot->lEndCount << '\t';
			//out << lpAliasShot->lRegNum << "\n";

			out << lpShot->lEndCount << '\t';
			out << lpShot->lRegNum << "\n";
		}
	}

	out.close();
}

BOOL ValidShot(long lNum)
{
	if ( lNum > 0 && lNum < lpVideo->iNumShots )
		return TRUE;
	else
		return FALSE;
}

/***********************************************************************/
void ExportShots(void)
/***********************************************************************/
{
	LPSHOT lpShot;

	OPENFILENAME ofn;
	_fmemset(&ofn, 0, sizeof(OPENFILENAME));
	const int nMaxIn = 120;
	STRING szFileName;


	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWndDialog;
	//ofn.lpstrFilter = szFilter;
	//ofn.nFilterIndex = 1;
	szFileName[0] = NULL;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	//ofn.lpstrInitialDir = lpPathName;
	ofn.lpstrTitle = "Export Normal Shots in Tabbed format for Excel"; // title of the dialog - "Open" by default
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	if (!GetOpenFileName(&ofn))
	{
		DWORD dwRet = CommDlgExtendedError();
		return;
	}

	ofstream out;

	out.open(szFileName, ios::trunc);

	if (out.bad())
		return;

	out << "Collision Shot Name\tShot ID\tExtra Shot ID's from LA\tDescription\tFilename\t";
	out << "Premier Start Frame\tPremier Full Length\tPremier Slate Frames\tPremiere Usable Frames\t";
	out << "Premier Pad Frames\tPremiere Length Check\tTable Start Frame\tTable End Frame\t";
	out << "Disk\tEnd\tLeft\tRight\tDown\tHome\tFlags\tValue\tEvent Range Start\tEvent Range End\n";

	for( int i=0; i < lpVideo->iNumShots; i++ )
	{
		lpShot = &lpVideo->lpAllShots[i];

		if ( !IsAlias(lpShot) )
		{
			// Output shot name
			out << lpShot->szShotName << '\t';

			// Orignal excel cells
			OUTS(lpOrig[i].szOldName);
			OUTS(lpOrig[i].szF3);
			OUTS(lpOrig[i].szDescr);
			OUTS(lpOrig[i].szF5);
			OUTS(lpOrig[i].szF6);
			OUTS(lpOrig[i].szF7);
			OUTS(lpOrig[i].szF8);
			OUTS(lpOrig[i].szF9);
			OUTS(lpOrig[i].szF10);
			OUTS(lpOrig[i].szF11);

			// Start Frame
			out << lpShot->lStartFrame << '\t';

			// End Frame
			out << lpShot->lEndFrame << '\t';

			// Disk number
			out << lpShot->wDisk << '\t';

			// End
			if (ValidShot(lpShot->lEndShot-1))
			{
				if ( lpShot->lEndShot == lpShot->lShotID )
					out << "/this";
				else
					out << lpVideo->lpAllShots[lpShot->lEndShot-1].szShotName;
			}
			out << '\t';

			// Left
			if (ValidShot(lpShot->lLeftShot-1))
				out << lpVideo->lpAllShots[lpShot->lLeftShot-1].szShotName;
			out << '\t';

			// Right 
			if (ValidShot(lpShot->lRightShot-1))
				out << lpVideo->lpAllShots[lpShot->lRightShot-1].szShotName;
			out << '\t';

			// Down
			if (ValidShot(lpShot->lDownShot-1))
				out << lpVideo->lpAllShots[lpShot->lDownShot-1].szShotName;
			out << '\t';

			// Home
			if (ValidShot(lpShot->lHomeShot-1))
				out << lpVideo->lpAllShots[lpShot->lHomeShot-1].szShotName;
			out << '\t';

			// Flags
			out << lpShot->lFlags << '\t';
			
			// Value
			out << lpShot->lValue << '\t';

			// Event range start frame
			out << lpShot->EventRange.nStart << '\t';
			//out << "-1" << '\t';

			// Event range end frame
			out << lpShot->EventRange.nEnd << "\n";
			//out << "-1" << "\n";

		}
	}

	out.close();
}

BOOL CellShot(char *psz)
{
	int iLen = strlen(psz);
	if (iLen < 11)
		return FALSE;

	if (psz[1] != 'H' && psz[1] != 'U')
		return FALSE;

	if (psz[2] >= '1' && psz[2] <= '7')
		return TRUE;
	else
		return FALSE;
}

void SaveRecValues(HWND hWnd)
{
	STRING szString;
	STRING szShotName;
	char *pszName;
	int iLen;
	LPSHOT lpShot;
	HWND hTree = GetDlgItem(hWnd, IDC_SHOTLIST);
	char szDescr[nMaxIn];
		
	// Check for valid shot name
	if (!(pszName = GetValidShotName()))
	{
		char sz[100];
		wsprintf( sz, "Bad shot name <%s>. Reason: %s", pszName, szError);
		MessageBox(hWnd, sz, "Error", MB_ICONERROR);
		return;
	}

	strcpy(szString, pszName);

	iLen = strlen(szString);

	SetDlgItemText(hWnd, IDC_SHOT, szString);
	GetDlgItemText(hWnd, IDC_DESCRIPTION, szDescr, nMaxIn);

	BOOL bTranslated;
	long lStartFrame = GetDlgItemInt(hWnd, IDC_STARTFRAME, &bTranslated, FALSE);
	long lEndFrame = GetDlgItemInt(hWnd, IDC_ENDFRAME, &bTranslated, FALSE);

	// If not hex or union jack shot
	if (CellShot(szString))
	{
		// Set the frames (only do it for the current shot since look shots will
		// have different frame offsets
		lpCurShot->lStartFrame = lStartFrame;
		lpCurShot->lEndFrame = lEndFrame;

		// Loop through all shots and update shots with like names which includes
		// gate, looks, loops, and ends
		for(int i=0; i<lpVideo->iNumShots; i++)
		{
			lpShot = &lpVideo->lpAllShots[i];

			// If the base shot names are equal
			if ( BaseShotNameEqual( lpShot->szShotName, szString ) )
			{
				// Copy the changes (usually opponent stuff)
				CopyShotNameChanges( szShotName, szString );

				SetNameChange( hTree, lpShot, szShotName );

				SetRecFlags( lpShot );

				// Replicate comments as well (I think this makes sense...)
				strcpy( COMMENT(i), szDescr );

			}
		}
	}

	// Else not a hex or union jack
	else
	{
		// Change name
		SetNameChange( hTree, lpCurShot, szString);

		// Change description
		strcpy( COMMENT(lpCurShot->lShotID-1), szDescr );

		SetRecFlags( lpCurShot );

		// Set the frames
		lpCurShot->lStartFrame = lStartFrame;
		lpCurShot->lEndFrame = lEndFrame;
	}

	lpCurShot->EventRange.nStart = GetDlgItemInt(hWndDialog, IDC_RANGESTART, 
		&bTranslated, FALSE);
	lpCurShot->EventRange.nEnd = GetDlgItemInt(hWndDialog, IDC_RANGEEND,   
		&bTranslated, FALSE);


}

void SetRecFlags(LPSHOT lpShot)
{
	
	int i;
	long lFlags = 0;

	// Set cell type
	i = SendDlgItemMessage(hWndDialog, IDC_CELLTYPE, CB_GETCURSEL, 0, 0);
	if (i == 0)
		lFlags |= FLAGBIT(F_HX);
	else if (i == 1)
		lFlags |= FLAGBIT(F_UJ);

	// Set shot type
	i = SendDlgItemMessage(hWndDialog, IDC_SHOTTYPE, CB_GETCURSEL, 0, 0);
	i += (F_TYPE_START - 1); // Subtract 1 because first item is none
	if ( i > F_TYPE_LAST || i < F_TYPE_START )
		i = 0;
	lFlags |= FLAGBIT(i);

	// Set shot cell position
	i = SendDlgItemMessage(hWndDialog, IDC_CELLPOSITION, CB_GETCURSEL, 0, 0);
	i += (F_POS_BEG - 1); // Subtract 1 because first item is none
	if ( i < F_POS_BEG || i > F_POS_LAST)
		i = 0;
	lFlags |= FLAGBIT(i);
}

void SetNameChange(HWND hTree, LPSHOT lpShot, char *pszName)
{
	TV_ITEM Item;

	strcpy( lpShot->szShotName, pszName);

	// Make a new atom
	DeleteAtom( lpShot->aShotID);
	lpShot->aShotID = AddAtom(lpShot->szShotName);

	int id = lpShot->lShotID;
	// Change tree entry with new name
	if (phItems[id])
	{
		Item.mask = TVIF_HANDLE | TVIF_TEXT;
		Item.hItem = phItems[id];
		Item.pszText = lpShot->szShotName;
		Item.cchTextMax = strlen(lpShot->szShotName);

		TreeView_SetItem(hTree, &Item);
	}
}

char *GetValidShotName(void)
{
	static char szShotName[100];
	char szChar[3];
	char *pszName;
	char cCellType;
	int iLen;

	GetDlgItemText(hWndDialog, IDC_SHOT, szShotName, 99);
	iLen = strlen(szShotName);

	pszName = szShotName;

	// Skip slash
	++pszName;

	// Hex or union jack
	cCellType = *pszName;
	if ((cCellType == 'H' || cCellType == 'U') && iLen >= 11)
	{
		szChar[1] = NULL;

		// Skip type and look
		pszName += 2;

		// Check player gate pos
		GetDlgItemText(hWndDialog, IDC_PLAYERGATE, szChar, 2);

		if (cCellType == 'H')
		{
			if( szChar[0] < '1' || szChar[0] > '4' )
			{
				wsprintf( szError, "Bad player position of %s.", szChar);
				SetFocus(GetDlgItem(hWndDialog, IDC_PLAYERGATE));
				return NULL;
			}
		}
		else
		{
			if( szChar[0] < '1' || szChar[0] > '7' )
			{
				wsprintf( szError, "Bad player position of %s.", szChar);
				SetFocus(GetDlgItem(hWndDialog, IDC_PLAYERGATE));
				return NULL;
			}
		}

		*pszName = szChar[0];
		++pszName;
		
		// Check opponent 1 type
		GetDlgItemText(hWndDialog, IDC_OPP1TYPE, szChar, 2);
		if (szChar[0] != 'W' && szChar[0] != 'S' && 
			szChar[0] != 'B' && szChar[0] != '_')
		{
			wsprintf(szError, "Bad opponent type of %s.", szChar);
			SetFocus(GetDlgItem(hWndDialog, IDC_OPP1TYPE));
			return NULL;
		}
		*pszName = szChar[0];
		++pszName;

		// Check opponent 1 position
		GetDlgItemText(hWndDialog, IDC_OPP1POS, szChar, 2);
		if (szChar[0] < '1' || szChar[0] > '9')
		{
			if (szChar[0] != 'A' && szChar[0] != 'B' && 
				szChar[0] != 'C' && szChar[0] != '_')
			{
				wsprintf(szError, "Bad opponent position of %s.", szChar);
			SetFocus(GetDlgItem(hWndDialog, IDC_OPP1POS));
				return NULL;
			}
		}
		*pszName = szChar[0];
		++pszName;

		// Check opponent 2 type
		GetDlgItemText(hWndDialog, IDC_OPP2TYPE, szChar, 2);
		if (szChar[0] != 'W' && szChar[0] != 'S' && 
			szChar[0] != 'B' && szChar[0] != '_')
		{
			wsprintf(szError, "Bad opponent type of %s.", szChar);
			SetFocus(GetDlgItem(hWndDialog, IDC_OPP2TYPE));
			return NULL;
		}
		*pszName = szChar[0];
		++pszName;

		// Check opponent 2 position
		GetDlgItemText(hWndDialog, IDC_OPP2POS, szChar, 2);
		if (szChar[0] < '1' || szChar[0] > '9')
		{
			if (szChar[0] != 'A' && szChar[0] != 'B' && 
				szChar[0] != 'C' && szChar[0] != '_')
			{
				wsprintf(szError, "Bad opponent position of %s.", szChar);
				SetFocus(GetDlgItem(hWndDialog, IDC_OPP2POS));
				return NULL;
			}
		}
		*pszName = szChar[0];
		++pszName;

		// Check opponent 3 type
		GetDlgItemText(hWndDialog, IDC_OPP3TYPE, szChar, 2);
		if (szChar[0] != 'W' && szChar[0] != 'S' && 
			szChar[0] != 'B' && szChar[0] != '_')
		{
			wsprintf(szError, "Bad opponent type of %s.", szChar);
			SetFocus(GetDlgItem(hWndDialog, IDC_OPP3TYPE));
			return NULL;
		}
		*pszName = szChar[0];
		++pszName;

		// Check opponent 3 position
		GetDlgItemText(hWndDialog, IDC_OPP3POS, szChar, 2);
		if (szChar[0] < '1' || szChar[0] > '9')
		{
			if (szChar[0] != 'A' && szChar[0] != 'B' && 
				szChar[0] != 'C' && szChar[0] != '_')
			{
				wsprintf(szError, "Bad opponent position of %s.", szChar);
				SetFocus(GetDlgItem(hWndDialog, IDC_OPP3POS));
				return NULL;
			}
		}
		*pszName = szChar[0];
		++pszName;

		// Check special
		GetDlgItemText(hWndDialog, IDC_SPECIAL, szChar, 2);

		//
		// NOTE: Add check special code here later
		//

		*pszName = szChar[0];
		++pszName;

	}

	return szShotName;
}

BOOL BaseShotNameEqual( char *pszName1, char *pszName2 )
{
	int i = 1;

	int iLen1 = strlen(pszName1);
	int iLen2 = strlen(pszName2);

	if (pszName1[i] == pszName2[i])
	{
		// increment counter to go to the base shot name
		i = 11;
		if (iLen1 <= i || iLen2 <= i)
			return FALSE;

		pszName1 += i;
		pszName2 += i;

		iLen1 = strlen(pszName1);
		iLen2 = strlen(pszName2);

		int iCmpLen;
		if (iLen1 < iLen2)
			iCmpLen = iLen1;
		else
			iCmpLen = iLen2;

		if (strncmp(pszName1, pszName2, iCmpLen) == 0)
			return TRUE;
	}

	return FALSE;
}

void CopyShotNameChanges(char *pszDest, char *pszSrc)
{
	int i = 0;
	int iLen1 = strlen(pszDest);
	int iLen2 = strlen(pszSrc);
	int iCpyLen = 8;

	if ( iLen1 < 11 || iLen2 < 11 )
		return;

	pszDest += 3;
	pszSrc += 3;

	strncpy( pszDest, pszSrc, 8 );
}

void SetShotName(HWND hWndDialog, char *pszText)
{
	HWND hWnd = GetDlgItem(hWndDialog, IDC_SHOT);
	SetWindowText(hWnd, pszText);
	int iLen = strlen(pszText);

	// Get handles to all of the edit windows
	HWND hWndPlayerGate = GetDlgItem(hWndDialog, IDC_PLAYERGATE);
	HWND hWndOpp1Type = GetDlgItem(hWndDialog, IDC_OPP1TYPE);
	HWND hWndOpp1Pos = GetDlgItem(hWndDialog, IDC_OPP1POS);
	HWND hWndOpp2Type = GetDlgItem(hWndDialog, IDC_OPP2TYPE);
	HWND hWndOpp2Pos = GetDlgItem(hWndDialog, IDC_OPP2POS);
	HWND hWndOpp3Type = GetDlgItem(hWndDialog, IDC_OPP3TYPE);
	HWND hWndOpp3Pos = GetDlgItem(hWndDialog, IDC_OPP3POS);
	HWND hWndSpecial = GetDlgItem(hWndDialog, IDC_SPECIAL);

	// skip slash in name
	++pszText;
	
	// Make sure hex or union jack
	if ((*pszText == 'H' || *pszText == 'U') && iLen >= 11)
	{
		char szChar[2];

		// Skip cell type
		++pszText;

		szChar[1] = NULL;

		++pszText;
		szChar[0] = *pszText;
		SetWindowText(hWndPlayerGate, szChar);

		++pszText;
		szChar[0] = *pszText;
		SetWindowText(hWndOpp1Type, szChar);

		++pszText;
		szChar[0] = *pszText;
		SetWindowText(hWndOpp1Pos, szChar);

		++pszText;
		szChar[0] = *pszText;
		SetWindowText(hWndOpp2Type, szChar);

		++pszText;
		szChar[0] = *pszText;
		SetWindowText(hWndOpp2Pos, szChar);

		++pszText;
		szChar[0] = *pszText;
		SetWindowText(hWndOpp3Type, szChar);

		++pszText;
		szChar[0] = *pszText;
		SetWindowText(hWndOpp3Pos, szChar);

		++pszText;
		szChar[0] = *pszText;
		SetWindowText(hWndSpecial, szChar);
	}
	else
	{
		SetWindowText(hWndPlayerGate, "");
		SetWindowText(hWndOpp1Type, "");
		SetWindowText(hWndOpp1Pos, "");
		SetWindowText(hWndOpp2Type, "");
		SetWindowText(hWndOpp2Pos, "");
		SetWindowText(hWndOpp3Type, "");
		SetWindowText(hWndOpp3Pos, "");
		SetWindowText(hWndSpecial, "");
	}
}

SHOTID MakeShotID(char *lpString)
{
	ATOM atom;

	if ( atom = FindAtom(lpString) )
	{
		return( atom );
	}
	return( (SHOTID)AddAtom(lpString) );
}

void DumpOriginalRecs(int id)
{

	if (!lpOrig)
		return;

	HPTR hpData;
	UINT uBytes;
	FNAME szFileName;

	GetModuleFileName( GetApp()->GetInstance(), szFileName, MAX_FNAME_LEN );
	StripFile( szFileName );

	STRING szName;
	wsprintf( szName, "orig%03d.tbl", id );
	lstrcat( szFileName, szName );

	HFILE hFile = _lcreat(szFileName, 0);
	if ( hFile == HFILE_ERROR )
		return;

	hpData = (HPTR)lpOrig;

	DWORD dwSize = (DWORD)sizeof(EXCEL_ORIG) * (DWORD)lpVideo->iNumShots;
	while (dwSize)
	{
		uBytes = INT_MAX;
		if (uBytes > dwSize)
			uBytes = (UINT)dwSize;
		if (_lwrite(hFile, (LPCSTR)hpData, uBytes) == HFILE_ERROR)
		{
			Print("Error write to file: '%s'", szFileName);
			_lclose(hFile);
			return;
		}
		dwSize -= (DWORD)uBytes;
		hpData += uBytes;
	}
	_lclose(hFile);
}


void LoadOriginalRecs(int id)
{
	if (lpOrig)
		FreeUp(lpOrig);

	HPTR hpData;
	UINT uBytes;
	FNAME szFileName;

	GetModuleFileName( GetApp()->GetInstance(), szFileName, MAX_FNAME_LEN );
	StripFile( szFileName );

	STRING szName;
	wsprintf( szName, "orig%03d.tbl", id );
	lstrcat( szFileName, szName );

	HFILE hFile = _lopen(szFileName, 0);
	if ( hFile == HFILE_ERROR )
		return;

	lpOrig = (EXCEL_ORIG *)AllocX( sizeof(EXCEL_ORIG) * lpVideo->iNumShots, GMEM_ZEROINIT);
	if (lpOrig == NULL)
	{
		MessageBox(hWndDialog, "Could not allocate memory for original excel data.", "Error", MB_ICONERROR);
		return;
	}

	hpData = (HPTR)lpOrig;

	DWORD dwSize = (DWORD)sizeof(EXCEL_ORIG) * (DWORD)lpVideo->iNumShots;
	while (dwSize)
	{
		uBytes = INT_MAX;
		if (uBytes > dwSize)
			uBytes = (UINT)dwSize;
		if (_lread(hFile, (LPVOID)hpData, uBytes) == HFILE_ERROR)
		{
			Print("Error reading file: '%s'", szFileName);
			FreeUp(lpOrig);
			lpOrig = NULL;
			_lclose(hFile);
			return;
		}
		dwSize -= (DWORD)uBytes;
		hpData += uBytes;
	}
	_lclose(hFile);

}


void SetCurrentFrame(void)
{
	STRING szString;
	wsprintf( szString, "%ld", MCIGetPosition( lpVideo->hDevice ) );
	SetDlgItemText( hWndDialog, IDC_SHOTFRAME, szString );
}

SHOTID FindShot(char *psz)
{
	ATOM aId = FindAtom(psz);
	if (!aId)
		return NULL;

	for (int i=0; i<lpVideo->iNumShots; i++)
	{
		if (lpVideo->lpAllShots[i].aShotID == aId)
			return i+1;
	}
	return NULL;
}		