#include <string.h>

#include "proto.h"
#include "Author/Authres.h"
#include "Author.p"
#include "commctrl.h"
#include "vtable.h"
#include "cllsnid.h"

// global video world
static int iLevel;

static HWND hWndDialog = NULL;
static HWND hSceneWindow = NULL;
static LPVIDEO lpVideo = NULL;
static LPSHOT lpCurShot = NULL;
static BOOL bDragging = FALSE;

static char *pszPrefix = "SPT"; // Shot Pool Tree atom prefix
static char szName[80]; // Name used in atoms

//BOOL RecManOnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
//void RecManOnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
BOOL WINPROC EXPORT RecManDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
//BOOL RecManOnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

void PrintName(int iLevel, char *pszName)
{
	char szTabs[] = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	char szOut[100];

	szTabs[iLevel] = NULL;
	wsprintf(szOut, "%s%s\n", szTabs, pszName);
	OutputDebugString(szOut);

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
BOOL IsIndex(LPSHOT lpShot)
/***********************************************************************/
{
	if ( lpShot->lStartFrame < 0 )
		return TRUE;
	else 
		return FALSE;

}

/***********************************************************************/
void TraverseShotAlias(HWND hTree, HTREEITEM hParent, TV_INSERTSTRUCT *pItem, LPSHOT lpShot)
/***********************************************************************/
{
	HTREEITEM hNewParent;
	HTREEITEM hCurParent;

	int iIndex = lpShot->lEndShot-1;

	// If invalid shot index
	if (iIndex >= lpVideo->iNumShots)
	{
		char szError[80];
		wsprintf(szError, "Invalid lEndShot of %ld in alias <%s>", lpShot->lEndShot, lpShot->szShotName);
		MessageBox(lpVideo->hWnd, szError, "Error", MB_ICONERROR);
		return;
	}
	// Valid shot index, get the new shot
	else if (iIndex >= 0)
	{
		// Get aliased shot
		lpShot = &lpVideo->lpAllShots[ iIndex ];
	}

	// else, we have a normal shot - not an alias shot
	
	// See if shot already referenced (shouldn't be)
	strcpy(szName, pszPrefix);
	strcat(szName, lpShot->szShotName);
	if (FindAtom(szName))
	{
		char szError[80];
		wsprintf(szError, "Shot <%s> already referenced.\n", lpShot->szShotName);
		OutputDebugString(szError);
		//MessageBox(lpVideo->hWnd, szError, "Error", MB_ICONERROR);
		return;
	}

	// Add the name to the list
	AddAtom(szName);
	pItem->item.pszText = lpShot->szShotName;
	//if (IsAlias(lpShot))
	//{
	//	strcat(pItem->item.pszText, " (alias)");
	//}
	//else if (IsIndex(lpShot))
	//{
	//	char szStr[100];
	//	wsprintf(szStr, " (register %ld)", -lpShot->lEndCount);
	//	strcat(pItem->item.pszText, szStr);
	//}
	pItem->item.cchTextMax = strlen(pItem->item.pszText);
	pItem->hParent = hParent;
	hCurParent = TreeView_InsertItem(hTree, pItem);

	PrintName(iLevel, pItem->item.pszText);

	// If this does not have an end count then it could be 
	// a normal alias
	if (!lpShot->lEndCount)
	{
		// Is this an alias?
		if (IsAlias(lpShot))
		{
			++iLevel;
			TraverseShotAlias(hTree, hCurParent, pItem, lpShot);
			--iLevel;
		}
	}

	// Else this alias is the head of a pool of shots
	else
	{
		int iEnd = iIndex + lpShot->lEndCount;

		++iLevel;

		// There is a count so loop through the pool
		for( int i=iIndex+1; i <= iEnd; i++)
		{
			lpShot = &lpVideo->lpAllShots[ i ];

			// See if shot already referenced (shouldn't be)
			strcpy(szName, pszPrefix);
			strcat(szName, lpShot->szShotName);
			if (FindAtom(szName))
			{
				char szError[80];
				wsprintf(szError, "Shot <%s> already referenced.\n", lpShot->szShotName);
				OutputDebugString(szError);
				//MessageBox(lpVideo->hWnd, szError, "Error", MB_ICONERROR);
				continue;
			}

			// Add the name to the list
			AddAtom(szName);
			pItem->item.pszText = lpShot->szShotName;
			//if (IsAlias(lpShot))
			//{
			//	strcat(pItem->item.pszText, " (alias)");
			//}
			//else if (IsIndex(lpShot))
			//{
			//	char szStr[100];
			//	wsprintf(szStr, " (register %ld)", -lpShot->lEndCount);
			//	strcat(pItem->item.pszText, szStr);
			//}
			pItem->item.cchTextMax = strlen(pItem->item.pszText);
			pItem->hParent = hCurParent;
			hNewParent = TreeView_InsertItem(hTree, pItem);
			PrintName(iLevel, pItem->item.pszText);

			// Is this an alias?
			if (IsAlias(lpShot))
			{
				++iLevel;
				TraverseShotAlias(hTree, hNewParent, pItem, lpShot);
				--iLevel;
			}
		}
		--iLevel;
	}
}

/***********************************************************************/
void LoadShotPools(HWND hWnd)
/***********************************************************************/
{
	int iNum = lpVideo->iNumShots;
	int i;
	LPSHOT lpShots = lpVideo->lpAllShots;
	TV_INSERTSTRUCT TreeItem;
	HWND hTree;
	HTREEITEM hParent;

	TreeItem.hParent = NULL;
	TreeItem.hInsertAfter = TVI_LAST;
	TreeItem.item.mask = TVIF_TEXT;
  
	hTree = GetDlgItem(hWnd, IDC_SHOTPOOLS);

	iLevel = 0;

	// Loop through the shots
	for( i=0; i<iNum; i++ )
	{
		// Make sure shot record has not already been referenced
		strcpy(szName, pszPrefix);
		strcat(szName, lpShots[i].szShotName);
		if (FindAtom(szName))
			continue;

		// Add entry to the tree
		AddAtom(szName);
		TreeItem.item.pszText = lpShots[i].szShotName;
		//if (IsAlias(&lpShots[i]))
		//{
		//	strcat(TreeItem.item.pszText, " (alias)");
		//}
		//else if (IsIndex(&lpShots[i]))
		//{
		//	char szStr[100];
		//	wsprintf(szStr, " (register %ld)", -lpShots[i].lEndCount);
		//	strcat(TreeItem.item.pszText, szStr);
		//}
		TreeItem.item.cchTextMax = strlen(TreeItem.item.pszText);
		TreeItem.hParent = NULL;
		hParent = TreeView_InsertItem(hTree, &TreeItem);

		PrintName(iLevel, TreeItem.item.pszText);
	
		// If this shot is an alias then traverse the alias
		if (IsAlias(&lpShots[i]))
		{
			++iLevel;
			TraverseShotAlias(hTree, hParent, &TreeItem, &lpShots[i]);
			--iLevel;
		}

	}

	// Delete the temporary atoms
	ATOM aName;
	for( i=0; i<iNum; i++ )
	{
		strcpy(szName, pszPrefix);
		strcat(szName, lpShots[i].szShotName);
		aName = FindAtom(szName);
		if (aName)
			DeleteAtom(aName);
		else
		{
			char szDebug[80];
			wsprintf(szDebug, "Warning: <%s> not referenced!\n", lpShots[i].szShotName);
			OutputDebugString(szDebug);
		}
	}
}



/************************************************************************/
void RecManStart(HWND hWnd, LPVIDEO lpVid)
/************************************************************************/
{
	hSceneWindow = hWnd;
	lpVideo = lpVid;
	hWndDialog = DialogStart( YES, GetWindowInstance(hWnd), hWnd, IDD_RECMANAGER, RecManDlgProc);
	DialogShow(IDD_RECMANAGER);
}

/************************************************************************/
void RecManEnd(void)
/************************************************************************/
{
	if (hWndDialog)
		DialogEnd( hWndDialog, IDOK);
}


/************************************************************************/
BOOL RecManOnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	LoadShotPools(hWnd);

	MCIStop( lpVideo->hDevice, YES/*bWait*/ );

	return TRUE;
}

/************************************************************************/
void RecManOnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	if (id == IDOK)
	{
	}
}

/***********************************************************************/
void RecManOnNotify(HWND hWnd, int iCtrl, NMHDR FAR* pHdr)
/***********************************************************************/
{
	if (iCtrl == IDC_SHOTPOOLS)
	{
		NM_TREEVIEW *pNMView = (NM_TREEVIEW *)pHdr;

		// If there was a selection change
		if (pNMView->hdr.code == TVN_SELCHANGED)
		{
			//SetCurrentShot(pNMView);
		}
		else if (pNMView->hdr.code == TVN_BEGINDRAG)
		{
			bDragging = TRUE;
		}
	}

	//else if (iCtrl == IDC_EVENTS)
	//{
	//	if (pHdr->code == CBN_SELCHANGE)
	//	{
	//		int i=0;
	//	}
	//}
}

/***********************************************************************/
BOOL RecManOnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{			  
	return (FALSE);
}

/***********************************************************************/
BOOL WINPROC EXPORT RecManDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGMSG(hDlg, WM_INITDIALOG, RecManOnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, RecManOnCommand);
		HANDLE_DLGMSG(hDlg, WM_NOTIFY, RecManOnNotify);
		default:
			return(RecManOnMessage(hDlg, msg, wParam, lParam));
		break;
	}
	return(FALSE);
}


