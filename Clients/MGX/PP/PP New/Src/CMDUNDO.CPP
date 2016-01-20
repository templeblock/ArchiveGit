//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "cmdundo.h"
#include "macro.h"
#include "id.h"


// LOCAL prototypes
LOCAL void RemoveDisabledPackets(LPLIST lpList);
LOCAL BOOL	CmdList_OnSave(HWND hDlg, LPCMDPKT lpCmdPkt, CImage *pImage = NULL, LPTSTR lpExtName = NULL);
LOCAL int	CmdList_FillListBox(HWND hDlg, ITEMID idListBox, LPCMDPKT lpCmdPkt,
								BOOL fReverse);
LOCAL BOOL	CmdList_ListChanged(LPLIST lpOldList, LPLIST lpNewList, int nCount = -1);
LOCAL void 	CmdList_OnFullNames(HWND hDlg, ITEMID idListBox, LPCMDPKT lpCmdPkt);
LOCAL void CmdList_DrawItem( HWND hDlg, const DRAWITEMSTRUCT FAR *lpDraw, LPCMDPKT lpCmdPkt );
LOCAL BOOL CmdList_IsEnabled(LPCMDPKT lpCmdPkt, int nItem);

LOCAL BOOL 	CmdListUndo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void 	CmdListUndo_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void	CmdListUndo_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem);

static LPIMAGE CmdListUndo_lpImage;

/***********************************************************************/
BOOL WINPROC EXPORT DlgCmdListUndoProc( HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, CmdListUndo_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, CmdListUndo_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DRAWITEM, CmdListUndo_OnDrawItem);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CMDLIST_UNDO);
	}
}

/************************************************************************/
LOCAL void CmdListUndo_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem)
/************************************************************************/
{
LPCMDLIST lpCmdList = CmdListUndo_lpImage->lpCmdList;
LPCMDPKT lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->UndoPacketList);
if (lpCmdPkt)
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);

CmdList_DrawItem( hDlg, lpDrawItem, lpCmdPkt );
}

/************************************************************************/
LOCAL BOOL CmdListUndo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPCMDLIST lpCmdList;
LPCMDPKT lpCmdPkt;
HWND hListBox;
int nItems;

CenterPopup( hDlg );

CmdListUndo_lpImage = (LPIMAGE)lParam;
lpCmdList = CmdListUndo_lpImage->lpCmdList;
if (!lpCmdList)
	{
	AstralDlgEnd(hDlg, FALSE);
	return(TRUE);
	}

// fill the list box from the saved command list
lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->UndoPacketList);
if (lpCmdPkt)
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
nItems = CmdList_FillListBox(hDlg, IDC_CMDLIST_UNDO, lpCmdPkt, FALSE);
if (!nItems)
	{
	AstralDlgEnd(hDlg, FALSE);
	return(TRUE);
	}
hListBox = GetDlgItem(hDlg, IDC_CMDLIST_UNDO);
SetFocus(hListBox);
ListBox_SetTopIndex(hListBox, ListBox_GetCount(hListBox)-1);
ListBox_SetSel(hListBox, TRUE, ListBox_GetCount(hListBox)-1);
ListBox_SetCaretIndex(hListBox, ListBox_GetCount(hListBox)-1);
ListBox_SetSelectDir(hListBox, 1 /* towards bottom */);
SetDlgItemInt(hDlg, IDC_NUMTOUNDO, 1, NO);
ControlEnable(hDlg, IDOK, TRUE);
CheckDlgButton(hDlg, IDC_CMDLIST_FULLNAMES, !Control.UseShortCommandNames);

return(FALSE);
}

/************************************************************************/
LOCAL void CmdListUndo_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPCMDLIST lpCmdList;
LPCMDPKT lpCmdPkt;
HWND hListBox;
int iUndoCount;

switch (id)
	{
	case IDC_CMDLIST_UNDO:
		if (codeNotify == LBN_SELCHANGE) // selection change?
			{
			hListBox = GetDlgItem(hDlg, IDC_CMDLIST_UNDO);
			iUndoCount = ListBox_GetSelCount(hListBox);
			ControlEnable(hDlg, IDOK, iUndoCount > 0);
			SetDlgItemInt(hDlg, IDC_NUMTOUNDO, iUndoCount, NO);
			}
	break;

	case IDOK:
	iUndoCount = ListBox_GetSelCount(GetDlgItem(hDlg, IDC_CMDLIST_UNDO));
	if (iUndoCount)
		{
		lpCmdList = CmdListUndo_lpImage->lpCmdList;
		while (--iUndoCount >= 0)
			{
			lpCmdPkt = (LPCMDPKT)ListGetTail(&lpCmdList->UndoPacketList);
			ListUnlink(&lpCmdList->UndoPacketList, lpCmdPkt);
			ListAddHead(&lpCmdList->RedoPacketList, lpCmdPkt);
			}
		AstralDlgEnd( hDlg, TRUE );
		}
	else
		AstralDlgEnd( hDlg, FALSE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_SAVEUNDOLIST:
	// advance to second packet in undo command list
	lpCmdList = CmdListUndo_lpImage->lpCmdList;
	lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->UndoPacketList);
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	CmdList_OnSave(hDlg, lpCmdPkt, CmdListUndo_lpImage);
	break;

	case IDC_CMDLIST_FULLNAMES:
	lpCmdList = CmdListUndo_lpImage->lpCmdList;
	lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->UndoPacketList);
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	CmdList_OnFullNames(hDlg, IDC_CMDLIST_UNDO, lpCmdPkt);
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL CmdListRedo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void CmdListRedo_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void	CmdListRedo_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem);

static LPIMAGE CmdListRedo_lpImage;

/***********************************************************************/
BOOL WINPROC EXPORT DlgCmdListRedoProc( HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, CmdListRedo_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, CmdListRedo_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DRAWITEM, CmdListRedo_OnDrawItem);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CMDLIST_REDO);
	}
}

/************************************************************************/
LOCAL void CmdListRedo_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem)
/************************************************************************/
{
LPCMDLIST lpCmdList = CmdListRedo_lpImage->lpCmdList;
LPCMDPKT lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->RedoPacketList);

CmdList_DrawItem( hDlg, lpDrawItem, lpCmdPkt );
}

/************************************************************************/
LOCAL BOOL CmdListRedo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPCMDLIST lpCmdList;
LPCMDPKT lpCmdPkt;
HWND hListBox;
int nItems;

CenterPopup( hDlg );

CmdListRedo_lpImage = (LPIMAGE)lParam;
lpCmdList = CmdListRedo_lpImage->lpCmdList;
if (!lpCmdList)
	{
	AstralDlgEnd(hDlg, FALSE);
	return(TRUE);
	}

// fill the list box from the saved command list
lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->RedoPacketList);
nItems = CmdList_FillListBox(hDlg, IDC_CMDLIST_REDO, lpCmdPkt, FALSE);
if (!nItems)
	{
	AstralDlgEnd(hDlg, FALSE);
	return(TRUE);
	}
hListBox = GetDlgItem(hDlg, IDC_CMDLIST_REDO);
SetFocus(hListBox);
ListBox_SetSel(hListBox, TRUE, 0);
ListBox_SetCaretIndex(hListBox, 0);
ListBox_SetSelectDir(hListBox, -1 /* towards top */);
SetDlgItemInt(hDlg, IDC_NUMTOREDO, 1, NO);
ControlEnable(hDlg, IDOK, TRUE);
CheckDlgButton(hDlg, IDC_CMDLIST_FULLNAMES, !Control.UseShortCommandNames);
return(FALSE);
}

/************************************************************************/
LOCAL void CmdListRedo_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPCMDLIST lpCmdList;
LPCMDPKT lpCmdPkt;
LIST RedoList;
int iRedoCount;

switch (id)
	{
	case IDC_CMDLIST_REDO:
		if (codeNotify == LBN_SELCHANGE) // selection change?
			{
			iRedoCount = ListBox_GetSelCount(GetDlgItem(hDlg, IDC_CMDLIST_REDO));
			ControlEnable(hDlg, IDOK, iRedoCount > 0);
			SetDlgItemInt(hDlg, IDC_NUMTOREDO, iRedoCount, NO);
			}
	break;

	case IDOK:
	iRedoCount = ListBox_GetSelCount(GetDlgItem(hDlg, IDC_CMDLIST_REDO));
	if (iRedoCount)
		{
		ListInit(&RedoList);
		lpCmdList = CmdListRedo_lpImage->lpCmdList;
	   	lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->RedoPacketList);
		while (--iRedoCount >= 0)
			{
			ListUnlink(&lpCmdList->RedoPacketList, lpCmdPkt);
			ListAddTail(&RedoList, lpCmdPkt);
		   	lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->RedoPacketList);
			}
	   	lpCmdPkt = (LPCMDPKT)ListGetHead(&RedoList);
		ListAddTail(&lpCmdList->PacketList, lpCmdPkt);
		AstralDlgEnd( hDlg, TRUE );
		}
	else
		AstralDlgEnd( hDlg, FALSE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_SAVEREDOLIST:
	// get first packet in redo command list
	lpCmdList = CmdListRedo_lpImage->lpCmdList;
	lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->RedoPacketList);
	CmdList_OnSave(hDlg, lpCmdPkt, CmdListRedo_lpImage);
	break;

	case IDC_CMDLIST_FULLNAMES:
	// get first packet in redo command list
	lpCmdList = CmdListRedo_lpImage->lpCmdList;
	lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->RedoPacketList);
	CmdList_OnFullNames(hDlg, IDC_CMDLIST_REDO, lpCmdPkt);
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL CmdListEdit_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void CmdListEdit_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void CmdListEdit_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDraw);
LOCAL void CmdListEdit_Enable(HWND hDlg);
LOCAL BOOL CmdListEdit_OnOK(HWND hDlg);
LOCAL void CmdListEdit_OnListBox(HWND hDlg, UINT codeNotify);
LOCAL void CmdListEdit_OnDisable(HWND hDlg);
LOCAL void CmdListEdit_OnDelete(HWND hDlg);
LOCAL void CmdListEdit_OnInsert(HWND hDlg);
LOCAL BOOL CmdListEdit_AnyChanges();
LOCAL LPCMDPKT CmdListEdit_GetPacket(LPLIST lpPacketList, int nItem);
LOCAL BOOL CmdListEdit_IsEnabled(LPLIST lpPacketList, int nItem);
LOCAL int CmdListEdit_GetDisableState(HWND hDlg);

static LPIMAGE CmdListEdit_lpImage;
static LIST CmdListEdit_List;
static BOOL CmdListEdit_fInsertMode;
static int CmdListEdit_iInsertItem;
static int CmdListEdit_iInsertIndex;
static int CmdListEdit_nRedoItems;

/***********************************************************************/
BOOL WINPROC EXPORT DlgCmdListEditProc( HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, CmdListEdit_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, CmdListEdit_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DRAWITEM, CmdListEdit_OnDrawItem);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CMDLIST_EDIT);
	}
}

/************************************************************************/
LOCAL BOOL CmdListEdit_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPCMDLIST lpCmdList;
LPCMDPKT lpCmdPkt;
LIST RedoList;
HWND hListBox;
int nItems;

CenterPopup( hDlg );

CmdListEdit_lpImage = (LPIMAGE)lParam;
lpCmdList = CmdListEdit_lpImage->lpCmdList;
if (!lpCmdList)
	{
	AstralDlgEnd(hDlg, FALSE);
	return(TRUE);
	}

if (!CopyPacketList(&lpCmdList->UndoPacketList, &CmdListEdit_List))
	{
	AstralDlgEnd(hDlg, FALSE);
	return(TRUE);
	}

CmdListEdit_nRedoItems = 0;
// if in insert mode, move the redo list into the editing list
if (CmdListEdit_fInsertMode = lpCmdList->fInsertMode)
	{
	// make a copy of the redo list so we can cancel
	if (!CopyPacketList(&lpCmdList->RedoPacketList, &RedoList))
		{
		DestroyPacketList(&CmdListEdit_List);
		AstralDlgEnd(hDlg, FALSE);
		return(TRUE);
		}
	// move the redo list into the edit list
	lpCmdPkt = (LPCMDPKT)ListGetHead(&RedoList);
	while (lpCmdPkt)
		{
		ListUnlink(&RedoList, lpCmdPkt);
		ListAddTail(&CmdListEdit_List, lpCmdPkt);
		lpCmdPkt = (LPCMDPKT)ListGetHead(&RedoList);
		++CmdListEdit_nRedoItems;
		}
	// get list insert index
	CmdListEdit_iInsertIndex = lpCmdList->iInsertIndex;
	// same as list, cause we insert before in list box
	CmdListEdit_iInsertItem = CmdListEdit_iInsertIndex;
	}
else
	// can't have a redo list if not in insert mode!!!
	DestroyPacketList(&lpCmdList->RedoPacketList);
	
// fill the list box from the saved command list
lpCmdPkt = (LPCMDPKT)ListGetHead(&CmdListEdit_List);
if (lpCmdPkt)
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
nItems = CmdList_FillListBox(hDlg, IDC_CMDLIST_EDIT, lpCmdPkt, FALSE);
if (!nItems)
	{
	DestroyPacketList(&CmdListEdit_List);
	AstralDlgEnd(hDlg, FALSE);
	return(TRUE);
	}
hListBox = GetDlgItem(hDlg, IDC_CMDLIST_EDIT);
SetFocus(hListBox);
ListBox_SetTopIndex(hListBox, ListBox_GetCount(hListBox)-1);
ListBox_SetCaretIndex(hListBox, ListBox_GetCount(hListBox)-1);
ListBox_SetMoveCursor(hListBox,
					LoadCursor(PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(ID_DRAGMOVE)));
CmdListEdit_Enable(hDlg);
CheckDlgButton(hDlg, IDC_CMDLIST_FULLNAMES, !Control.UseShortCommandNames);
return(FALSE);
}

/************************************************************************/
LOCAL void CmdListEdit_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPCMDPKT lpCmdPkt;

switch (id)
	{
	case IDC_CMDLIST_EDIT:
	CmdListEdit_OnListBox(hDlg, codeNotify);
	break;

	case IDOK:
	if (CmdListEdit_OnOK(hDlg))
		{
		AstralDlgEnd( hDlg, TRUE );
		break;
		}
	// if no changes, fall thru to cancel
	case IDCANCEL:
	DestroyPacketList(&CmdListEdit_List);
	AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_CMDLIST_DISABLE:
	CmdListEdit_OnDisable(hDlg);
	break;

	case IDC_CMDLIST_DELETE:
	CmdListEdit_OnDelete(hDlg);
	break;

	case IDC_CMDLIST_INSERT:
	CmdListEdit_OnInsert(hDlg);
	break;

	case IDC_CMDLIST_SAVE:
	// advance to second packet in undo command list
	lpCmdPkt = (LPCMDPKT)ListGetHead(&CmdListEdit_List);
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	CmdList_OnSave(hDlg, lpCmdPkt, CmdListEdit_lpImage);
	break;

	case IDC_CMDLIST_FULLNAMES:
	lpCmdPkt = (LPCMDPKT)ListGetHead(&CmdListEdit_List);
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	CmdList_OnFullNames(hDlg, IDC_CMDLIST_EDIT, lpCmdPkt);
	break;

   default:
	break;
   }
}

/************************************************************************/
LOCAL BOOL CmdListEdit_OnOK(HWND hDlg)
/************************************************************************/
{
int iCount;
LPCMDLIST lpCmdList;
LPCMDPKT lpCmdPkt;
LIST TempList, RedoList;
BOOL fListChanged;

if (!CmdListEdit_AnyChanges())
	return(FALSE);

lpCmdList = CmdListEdit_lpImage->lpCmdList;

// if coming out of insert mode or going into insert mode,
// then move the redo items to a temporary redo list
if (CmdListEdit_fInsertMode || lpCmdList->fInsertMode)
	{
	ListInit(&RedoList);
	iCount = CmdListEdit_nRedoItems;
	while (--iCount >= 0)
		{
		lpCmdPkt = (LPCMDPKT)ListGetTail(&CmdListEdit_List);
		ListUnlink(&CmdListEdit_List, lpCmdPkt);
		ListAddHead(&RedoList, lpCmdPkt);
		}
	}

// see if the list has changed at all
fListChanged =	CmdList_ListChanged(&lpCmdList->UndoPacketList,	&CmdListEdit_List);

// swap in the new edited packet list
TempList = lpCmdList->UndoPacketList;
lpCmdList->UndoPacketList = CmdListEdit_List;
DestroyPacketList(&TempList);

// wack the redo list for good measure
DestroyPacketList(&lpCmdList->RedoPacketList);

// build up redo list for insert mode
if (CmdListEdit_fInsertMode)
	{
	// setup mode to insert
	lpCmdList->fInsertMode = TRUE;
	// move number of redo items into the redo list
	iCount = ListGetCount(&RedoList);
	while (--iCount >= 0)
		{
		lpCmdPkt = (LPCMDPKT)ListGetHead(&RedoList);
		ListUnlink(&RedoList, lpCmdPkt);
		ListAddTail(&lpCmdList->RedoPacketList, lpCmdPkt);
		}
	// turn on insert mode
	lpCmdList->iInsertIndex = CmdListEdit_iInsertIndex;
	}
else
// if we are turning insert mode off and the list has
// not changed, all we need to do is redo the commands after the insert
// otherwise put the commands into the undo list for processing
if (lpCmdList->fInsertMode)
	{
	lpCmdList->fInsertMode = FALSE;
	iCount = ListGetCount(&RedoList);
	while (--iCount >= 0)
		{
		lpCmdPkt = (LPCMDPKT)ListGetHead(&RedoList);
		ListUnlink(&RedoList, lpCmdPkt);
		if (fListChanged)
			ListAddTail(&lpCmdList->UndoPacketList, lpCmdPkt);
		else
			ListAddTail(&lpCmdList->PacketList, lpCmdPkt);
		}
	}
return(TRUE);
}

/************************************************************************/
LOCAL void CmdListEdit_OnListBox(HWND hDlg, UINT codeNotify)
/************************************************************************/
{
if (codeNotify == LBN_SELCHANGE) // selection change?
	CmdListEdit_Enable(hDlg);
else
if (codeNotify == LBN_MOVEITEM)
	{
	// user has move items in the list box
	LPCMDPKT lpMovePkt, lpDestPkt;
	int nOldPos, nNewPos;

	// get the old and new position of the items
	ListBox_GetMoveItem(GetDlgItem(hDlg, IDC_CMDLIST_EDIT), &nOldPos, &nNewPos);
	if (nOldPos != nNewPos)
		{
		lpMovePkt = CmdListEdit_GetPacket(&CmdListEdit_List, nOldPos);
		lpDestPkt = CmdListEdit_GetPacket(&CmdListEdit_List, nNewPos);
		if (lpMovePkt)
			{
			// unlink to packet to move
			ListUnlink(&CmdListEdit_List, lpMovePkt);
			if (lpDestPkt)
				{
			  	// always before after the dest
			  	ListInsertBefore(&CmdListEdit_List, lpDestPkt, lpMovePkt);
				}
			else
				{
				// if there is no dest packet that means the user is
				// try to move the item to the bottom of the list
				ListAddTail(&CmdListEdit_List, lpMovePkt);
				}
			CmdListEdit_Enable(hDlg);
			}
		}
	}
}

/************************************************************************/
LOCAL void CmdListEdit_OnDisable(HWND hDlg)
/************************************************************************/
{
int iSelCount, iSelected, i, iState;
LPINT lpSelItems;
LPCMDPKT lpCmdPkt;

iState = CmdListEdit_GetDisableState(hDlg);
if (iState < 0)
	return;
// flip the state
iState = !iState;

// get the selected items
iSelCount = ListBox_GetSelCount(GetDlgItem(hDlg, IDC_CMDLIST_EDIT));
if (iSelCount)
	{
	lpSelItems = (LPINT)Alloc(sizeof(int)*iSelCount);
	if (!lpSelItems)
		{
		Message(IDS_EMEMALLOC);
		return;
		}
	ListBox_GetSelItems(GetDlgItem(hDlg, IDC_CMDLIST_EDIT), iSelCount, lpSelItems);


	// now disable all items that are selected
	for (i = 0; i < iSelCount; ++i)
		{
		iSelected = lpSelItems[i];
		lpCmdPkt = CmdListEdit_GetPacket(&CmdListEdit_List, iSelected);
		if (lpCmdPkt)
			lpCmdPkt->fDisabled = iState;
		}
	FreeUp(lpSelItems);
	AstralControlPaint(hDlg, IDC_CMDLIST_EDIT);
	CmdListEdit_Enable(hDlg);
	}
}

/************************************************************************/
LOCAL void CmdListEdit_OnDelete(HWND hDlg)
/************************************************************************/
{
int iSelCount, iSelected, i, iOffset, iFirstRedo, iCount;
LPINT lpSelItems;
LPCMDPKT lpCmdPkt;

// get the selected items
iSelCount = ListBox_GetSelCount(GetDlgItem(hDlg, IDC_CMDLIST_EDIT));
lpSelItems = (LPINT)Alloc(sizeof(int)*iSelCount);
if (!lpSelItems)
	{
	Message(IDS_EMEMALLOC);
	return;
	}
ListBox_GetSelItems(GetDlgItem(hDlg, IDC_CMDLIST_EDIT), iSelCount, lpSelItems);

iFirstRedo = INT_MAX;
if (CmdListEdit_nRedoItems)
	{
	iCount = ListGetCount(&CmdListEdit_List) - 1/*don't include open command*/;
	iFirstRedo = iCount - CmdListEdit_nRedoItems;
	}
// now disable all items that are selected
iOffset = 0;
for (i = 0; i < iSelCount; ++i)
	{
	iSelected = lpSelItems[i] - iOffset;
	lpCmdPkt = CmdListEdit_GetPacket(&CmdListEdit_List, iSelected);
	if (lpCmdPkt)
		{
		ListUnlink(&CmdListEdit_List, lpCmdPkt);
		FreeUpPacket(lpCmdPkt);
		++iOffset;
		ListBox_DeleteString(GetDlgItem(hDlg, IDC_CMDLIST_EDIT), iSelected);
		if (iSelected >= iFirstRedo)
			--CmdListEdit_nRedoItems;
		--iFirstRedo;
		}
	}
FreeUp(lpSelItems);
CmdListEdit_Enable(hDlg);
}

/************************************************************************/
LOCAL void CmdListEdit_OnInsert(HWND hDlg)
/************************************************************************/
{
int iSelCount, nItems;

// toggle the insert mode
CmdListEdit_fInsertMode = !CmdListEdit_fInsertMode;

// turn on insert mode
if (CmdListEdit_fInsertMode)
	{
	// the select count better be 1
	iSelCount = ListBox_GetSelCount(GetDlgItem(hDlg, IDC_CMDLIST_EDIT));
	if (iSelCount != 1)
		{
		CmdListEdit_fInsertMode = NO;
		return;
		}

	// if turning on insert mode, then get the selected item for inserting
	ListBox_GetSelItems(GetDlgItem(hDlg, IDC_CMDLIST_EDIT), iSelCount,
						(LPINT)&CmdListEdit_iInsertItem);

	// number of items that will go into redo list
	// when new items get inserted
	nItems = ListGetCount(&CmdListEdit_List);
	CmdListEdit_iInsertIndex = CmdListEdit_iInsertItem;
	// subtract 1 because 1st packet is never included
	CmdListEdit_nRedoItems = nItems - CmdListEdit_iInsertIndex - 1;
	}

AstralControlPaint(hDlg, IDC_CMDLIST_EDIT);
CmdListEdit_Enable(hDlg);
}

/************************************************************************/
LOCAL int CmdListEdit_GetDisableState(HWND hDlg)
/************************************************************************/
{
int iSelCount, iSelected, i;
LPINT lpSelItems;
LPCMDPKT lpCmdPkt;
int iState = -1;

// get the selected items
iSelCount = ListBox_GetSelCount(GetDlgItem(hDlg, IDC_CMDLIST_EDIT));
if (iSelCount > 0)
	{
	lpSelItems = (LPINT)Alloc(sizeof(int)*iSelCount);
	if (!lpSelItems)
		{
		Message(IDS_EMEMALLOC);
		return(iState);
		}
	ListBox_GetSelItems(GetDlgItem(hDlg, IDC_CMDLIST_EDIT), iSelCount, lpSelItems);

	// now disable all items that are selected
	for (i = 0; i < iSelCount; ++i)
		{
		iSelected = lpSelItems[i];
		lpCmdPkt = CmdListEdit_GetPacket(&CmdListEdit_List, iSelected);
		if (lpCmdPkt)
			{
			if (iState < 0)
				iState = lpCmdPkt->fDisabled;
			else
			if (lpCmdPkt->fDisabled != iState)
				{
				iState = -1;
				break;
				}
			}
		}
	FreeUp(lpSelItems);
	}
return(iState);
}


/************************************************************************/
LOCAL void CmdListEdit_Enable(HWND hDlg)
/************************************************************************/
{
int iEditCount;
int iState;
BOOL fEnable = TRUE;
STRING szString;

iState = CmdListEdit_GetDisableState(hDlg);
if (iState < 0)
	{
	iState = NO;
	fEnable = FALSE;
	}
if (iState)
	GetDlgItemText(hDlg, IDC_CMDLIST_ENABLETEXT, szString, sizeof(szString));
else
	GetDlgItemText(hDlg, IDC_CMDLIST_DISABLETEXT, szString, sizeof(szString));
SetDlgItemText(hDlg, IDC_CMDLIST_DISABLE, szString);
ControlEnable(hDlg, IDC_CMDLIST_DISABLE, fEnable && !CmdListEdit_fInsertMode);

// get the number of items selected in list box
iEditCount = ListBox_GetSelCount(GetDlgItem(hDlg, IDC_CMDLIST_EDIT));

ControlEnable(hDlg, IDOK, CmdListEdit_AnyChanges());
ControlEnable(hDlg, IDC_CMDLIST_DELETE, iEditCount > 0 && !CmdListEdit_fInsertMode);
CheckDlgButton(hDlg, IDC_CMDLIST_INSERT, CmdListEdit_fInsertMode);
ControlEnable(hDlg, IDC_CMDLIST_INSERT, (iEditCount == 1) ||
				CmdListEdit_fInsertMode);
SetDlgItemInt(hDlg, IDC_CMDLIST_SELECTED, iEditCount, NO && !CmdListEdit_fInsertMode);
ListBox_MoveEnable(GetDlgItem(hDlg, IDC_CMDLIST_EDIT), !CmdListEdit_fInsertMode);
}

/************************************************************************/
LOCAL LPCMDPKT CmdListEdit_GetPacket(LPLIST lpPacketList, int nItem)
/************************************************************************/
{
LPCMDPKT lpCmdPkt;

lpCmdPkt = (LPCMDPKT)ListGetHead(lpPacketList);
// never include 1st packet
lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
while (--nItem >= 0 && lpCmdPkt)
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
// never return the first packet - the load command
if (lpCmdPkt == (LPCMDPKT)ListGetHead(lpPacketList))
	lpCmdPkt = NULL;
return(lpCmdPkt);
}

/************************************************************************/
LOCAL BOOL CmdListEdit_IsEnabled(LPLIST lpPacketList, int nItem)
/************************************************************************/
{
LPCMDPKT lpCmdPkt;

lpCmdPkt = CmdListEdit_GetPacket(lpPacketList, nItem);
if (lpCmdPkt)
	return(!lpCmdPkt->fDisabled);
else
	return(FALSE);
}

/************************************************************************/
LOCAL BOOL CmdList_IsEnabled(LPCMDPKT lpCmdPkt, int nItem)
/************************************************************************/
{
	if (!lpCmdPkt)
		return(FALSE);
	while (--nItem >= 0 && lpCmdPkt)
		lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	if (lpCmdPkt)
		return(!lpCmdPkt->fDisabled);
	else
		return(FALSE);
}

/************************************************************************/
LOCAL BOOL CmdList_ListChanged(LPLIST lpOldList, LPLIST lpNewList, int iCount)
/************************************************************************/
{
LPCMDPKT lpOldPkt, lpNewPkt;

if (iCount >= 0)
	{
	if (ListGetCount(lpOldList) < iCount ||
		ListGetCount(lpNewList) < iCount)
		return(TRUE);
	}
else
	{
	iCount = ListGetCount(lpOldList);
	if (iCount != ListGetCount(lpNewList))
		return(TRUE);
	}

lpOldPkt = (LPCMDPKT)ListGetHead(lpOldList);
lpNewPkt = (LPCMDPKT)ListGetHead(lpNewList);
while (lpOldPkt && lpNewPkt && iCount)
	{
	if (lpOldPkt->fDisabled != lpNewPkt->fDisabled)
		return(TRUE);
	if (lpOldPkt->lpParms != lpNewPkt->lpParms)
		return(TRUE);
	lpOldPkt = (LPCMDPKT)ListGetNext(lpOldPkt);
	lpNewPkt = (LPCMDPKT)ListGetNext(lpNewPkt);
	--iCount;
	}
return(FALSE);
}

/************************************************************************/
LOCAL int GetNumClipbits(CImage *pImage, LPCMDPKT lpFirstCmdPkt)
/************************************************************************/
{
	int nCopies = 0;

	if (pImage &&
		pImage->GetRootCompoundStorage() &&
		(pImage->FileType == IDN_PPCOMPOUND))
	{
		LPCMDPKT pCmdPkt = lpFirstCmdPkt;
		while (pCmdPkt)
		{
			if (pCmdPkt->idCommand == IDS_CMD_PASTEFROM)
			{
				LPPASTEFROM_PARMS pParms = (LPPASTEFROM_PARMS)pCmdPkt->lpParms;
				// is this a clipbit stored in storage?
				if (!lstrlen(pParms->szClipboard) &&
					IsStorageClipbit(pParms->szClipFileName) )
				{
					++nCopies;
				}
			}
			pCmdPkt = (LPCMDPKT)ListGetNext(pCmdPkt);
		}
	}
	return(nCopies);
}

/************************************************************************/
LOCAL int GetNumMaskbits(CImage *pImage, LPCMDPKT lpFirstCmdPkt)
/************************************************************************/
{
	int nCopies = 0;

	if (pImage &&
		pImage->GetRootCompoundStorage() &&
		(pImage->FileType == IDN_PPCOMPOUND))
	{
		LPCMDPKT pCmdPkt = lpFirstCmdPkt;
		while (pCmdPkt)
		{
			if (pCmdPkt->idCommand == IDS_CMD_LOADMASK)
			{
				LPLOADMASK_PARMS pParms = (LPLOADMASK_PARMS)pCmdPkt->lpParms;
				if (!lstrlen(pParms->szMask) &&
					IsStorageMaskbit(pParms->szMaskFileName))
				{
					++nCopies;
				}
			}
			pCmdPkt = (LPCMDPKT)ListGetNext(pCmdPkt);
		}
	}
	return(nCopies);
}

typedef struct
{
	LPCMDPKT			pCmdPkt;
	FNAME	 			szFileName;
} PACKETFILE;

/************************************************************************/
LOCAL void FreeUpPacketFileList(PACKETFILE *pList, BOOL fDelete, int nCopies)
/************************************************************************/
{
	for (int i = 0; i < nCopies; ++i)
	{
		LPTSTR lpFileName;
		LPCMDPKT pCmdPkt = pList[i].pCmdPkt;
		if (pCmdPkt->idCommand == IDS_CMD_PASTEFROM)
		{
			LPPASTEFROM_PARMS pParms = (LPPASTEFROM_PARMS)pCmdPkt->lpParms;
			lpFileName = pParms->szClipFileName;
		}
		else
		{
			LPLOADMASK_PARMS pParms = (LPLOADMASK_PARMS)pCmdPkt->lpParms;
			lpFileName = pParms->szMaskFileName;
		}
		if (fDelete)
			DeleteFile(lpFileName);
		lstrcpy(lpFileName, pList[i].szFileName);
	}
	FreeUp(pList);
}

/************************************************************************/
LOCAL int CreateClipbits(CImage *pImage, LPCMDPKT lpFirstCmdPkt, PACKETFILE *pList, int nIndex)
/************************************************************************/
{
	LPCMDPKT pCmdPkt = lpFirstCmdPkt;
	while (pCmdPkt)
	{
		if (pCmdPkt->idCommand == IDS_CMD_PASTEFROM)
		{
			LPPASTEFROM_PARMS pParms = (LPPASTEFROM_PARMS)pCmdPkt->lpParms;
			// use this to flag files we create
			// is this a clipbit stored in storage?
			if (!lstrlen(pParms->szClipboard) &&
				IsStorageClipbit(pParms->szClipFileName) )
			{
				EXTNAME szExtName;
				FNAME szFileName;

				// create a base name for the clipboard filename
				lstrcpy(szExtName, "clip");

				// create a filename to write to
				CreateFileName( szExtName, IDN_CLIPBOARD, szFileName);
				if (CopyFromStream(pParms->szClipFileName, szFileName, pImage->GetRootCompoundStorage()))
				{
					// setup name in storage - relative path
					pList[nIndex].pCmdPkt = pCmdPkt;
					lstrcpy(pList[nIndex].szFileName, pParms->szClipFileName);
					++nIndex;
					lstrcpy(pParms->szClipFileName, szFileName);
				}
				else
				{
					FreeUpPacketFileList(pList, TRUE, nIndex);
					return(-1);
				}
			}
		}
 		pCmdPkt = (LPCMDPKT)ListGetNext(pCmdPkt);
	}
	return(nIndex);
}

/************************************************************************/
LOCAL int CreateMaskbits(CImage *pImage, LPCMDPKT lpFirstCmdPkt, PACKETFILE *pList, int nIndex)
/************************************************************************/
{
	LPCMDPKT pCmdPkt = lpFirstCmdPkt;
	while (pCmdPkt)
	{
		if (pCmdPkt->idCommand == IDS_CMD_LOADMASK)
		{
			LPLOADMASK_PARMS pParms = (LPLOADMASK_PARMS)pCmdPkt->lpParms;
			// use this to flag files we create
			// is this a clipbit stored in storage?
			if (!lstrlen(pParms->szMask) &&
				IsStorageMaskbit(pParms->szMaskFileName) )
			{
				EXTNAME szExtName;
				FNAME szFileName;

				// create a base name for the clipboard filename
				lstrcpy(szExtName, "mask");

				// create a filename to write to
				CreateFileName( szExtName, IDN_MASK, szFileName);
				if (CopyFromStream(pParms->szMaskFileName, szFileName, pImage->GetRootCompoundStorage()))
				{
					// setup name in storage - relative path
					pList[nIndex].pCmdPkt = pCmdPkt;
					lstrcpy(pList[nIndex].szFileName, pParms->szMaskFileName);
					++nIndex;
					lstrcpy(pParms->szMaskFileName, szFileName);
				}
				else
				{
					FreeUpPacketFileList(pList, TRUE, nIndex);
					return(-1);
				}
			}
		}
 		pCmdPkt = (LPCMDPKT)ListGetNext(pCmdPkt);
	}
	return(nIndex);
}

/************************************************************************/
LOCAL BOOL CmdList_OnSave(HWND hDlg, LPCMDPKT lpFirstCmdPkt, CImage *pImage, LPTSTR lpExtName)
/************************************************************************/
{
	PACKETFILE *pList = NULL;
	MACRO_FILE_HANDLE fh;
	FNAME szFileName;
	int nNumMasks;
	int	nCopies = 0;

	// Get macro name from user
	if (!AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_MACROSAVE, DlgMacroRecordProc ))
		return(FALSE);

	nCopies = GetNumClipbits(pImage, lpFirstCmdPkt);
	nCopies += GetNumMaskbits(pImage, lpFirstCmdPkt);
	if (nCopies)
	{
		pList = (PACKETFILE *)Alloc(sizeof(PACKETFILE) * nCopies);
		if (!pList)
		{
			Message(IDS_EMEMALLOC);
			return(FALSE);
		}
		int nIndex = 0;
		nIndex = CreateClipbits(pImage, lpFirstCmdPkt, pList, nIndex);
		if (nIndex < 0)
			return(FALSE);
		nIndex = CreateMaskbits(pImage, lpFirstCmdPkt, pList, nIndex);
		if (nIndex < 0)
			return(FALSE);
	}
 	 
	// see if macro already exists
	if ( !LookupExtFile(Names.Macro, szFileName, IDN_MACRO) )
	{ // doesn't exist so create a file name for it
		if ( !CreateFileName( Names.Macro, IDN_MACRO, szFileName ) )
		{
			if (pList)
				FreeUpPacketFileList(pList, TRUE, nCopies);
			return(FALSE);
		}
	}

	if (!AddExtFile(Names.Macro, szFileName, IDN_MACRO))
	{
		if (pList)
			FreeUpPacketFileList(pList, TRUE, nCopies);
		FileDelete( szFileName );
		return(FALSE);
	}

	BOOL fRet = FALSE;
	#ifdef BUFFERED_IO
	AnsiToOem(szFileName, szFileName);
	fh = fopen(szFileName, "w+b");
	if (fh != NULL)
	#else
	fh = FileOpen(szFileName, FO_CREATE|FO_WRITE);
	if (fh != MACRO_FILE_HANDLE_INVALID)
	#endif
	{
		fRet = WritePacketList(lpFirstCmdPkt, fh);
		#ifdef BUFFERED_IO
		fclose(fh);
		#else
		FileClose(fh);
		#endif
		if (lpExtName)
			lstrcpy(lpExtName, Names.Macro);
	}
	else
	{
		Message(IDS_EOPEN, (LPTSTR)szFileName);
	}
	if (pList)
		FreeUpPacketFileList(pList, !fRet, nCopies);
	return(fRet);
}

/************************************************************************/
LOCAL int CmdList_FillListBox(HWND hDlg, ITEMID idListBox, LPCMDPKT lpCmdPkt,
								BOOL fReverse)
/************************************************************************/
{
int nItems;
STRING szName;
HWND hListBox;

hListBox = GetDlgItem(hDlg, idListBox);
if (!hListBox)
	return(0);
ListBox_ResetContent(hListBox);

nItems = 0;
while (lpCmdPkt)
	{
	GetCommandString(lpCmdPkt, szName, sizeof(szName), Control.UseShortCommandNames);
	if (fReverse)
		ListBox_InsertString(hListBox, 0, szName);
	else
		ListBox_AddString(hListBox, szName);
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	++nItems;
	}
return(nItems);
}

/************************************************************************/
LOCAL void CmdList_OnFullNames(HWND hDlg, ITEMID idListBox, LPCMDPKT lpCmdPkt)
/************************************************************************/
{
int iSelCount, i, iTopIndex, iCaretIndex;
LPINT lpSelItems;
HWND hListBox;

Control.UseShortCommandNames = !Control.UseShortCommandNames;
CheckDlgButton(hDlg, IDC_CMDLIST_FULLNAMES, !Control.UseShortCommandNames);

hListBox = GetDlgItem(hDlg, idListBox);
if (!hListBox)
	return;

// get the selected items
iSelCount = ListBox_GetSelCount(hListBox);
if (iSelCount)
	{
	lpSelItems = (LPINT)Alloc(sizeof(int)*iSelCount);
	if (!lpSelItems)
		{
		Message(IDS_EMEMALLOC);
		return;
		}
	ListBox_GetSelItems(hListBox, iSelCount, lpSelItems);
	}
SetWindowRedraw(hListBox, FALSE);
iTopIndex = ListBox_GetTopIndex(hListBox);
iCaretIndex = ListBox_GetCaretIndex(hListBox);
CmdList_FillListBox(hDlg, idListBox, lpCmdPkt, FALSE);
if (iSelCount)
	{
	for (i = 0; i < iSelCount; ++i)
		ListBox_SetSel(hListBox, TRUE, lpSelItems[i]);
	FreeUp(lpSelItems);
	}
ListBox_SetTopIndex(hListBox, iTopIndex);
ListBox_SetCaretIndex(hListBox, iCaretIndex);
SetWindowRedraw(hListBox, TRUE);
}

/***********************************************************************/
LOCAL void CmdList_DrawItem( HWND hDlg, const DRAWITEMSTRUCT FAR *lpDraw,
							LPCMDPKT lpCmdPkt)
/***********************************************************************/
{
int      x, y, i;
RECT     ClientRect;
HICON    hResource;
HWND     hControl;
HDC      hDC;
BOOL     bFocus, bHilighted, bGrayed;
STRING   szString, szTemp;
int      iOldMode;
COLORREF lOldColor;
HBRUSH   hBrush;
LPTSTR   lpString;
DWORD    dwStyle;
HFONT    hOldFont;

// Check state (ODS_??) and not action (ODA_??)
#define OD_GRAYED (ODS_GRAYED | ODS_DISABLED)
#define OD_SELECTED (ODS_SELECTED | ODS_CHECKED)
#define INDENT_SPACES 5

if ( lpDraw->itemID == -1 )
	return;

// Check to see if its focus draw only
if ( lpDraw->itemAction == ODA_FOCUS )
	{
	DrawFocusRect( lpDraw->hDC, &lpDraw->rcItem );
	return;
	}

hDC = lpDraw->hDC;
ClientRect = lpDraw->rcItem;
hControl = GetDlgItem( hDlg, lpDraw->CtlID );

dwStyle = GetWindowLong(hControl, GWL_STYLE);

if (lpDraw->CtlType == ODT_COMBOBOX)
	ComboBox_GetLBText(hControl, lpDraw->itemID, szString);
else
	ListBox_GetText(hControl, lpDraw->itemID, szString);

bGrayed = ( (lpDraw->itemState & OD_GRAYED) ||
	!CmdList_IsEnabled(lpCmdPkt, lpDraw->itemID) );

bHilighted = ( (lpDraw->itemState & OD_SELECTED) && !bGrayed );
bFocus = ( (lpDraw->itemState & ODS_FOCUS) && !bGrayed );

// Draw the box interior
hBrush = CreateSolidBrush( 
	GetSysColor( bHilighted ? COLOR_HIGHLIGHT : COLOR_WINDOW ) );
FillRect( hDC, &ClientRect, hBrush );
DeleteObject( hBrush );

lpString = SkipSpaces( szString );

if (lstrlen(lpString))
	{
	// Draw the text
	iOldMode = SetBkMode( hDC, TRANSPARENT );
	lOldColor = SetTextColor( hDC,
			GetSysColor( bHilighted ? COLOR_HIGHLIGHTTEXT :
		   	( bGrayed ? COLOR_BTNSHADOW : COLOR_WINDOWTEXT ) ) );

	if (dwStyle & CBS_THINFONT)
		hOldFont = (HFONT)SelectObject(hDC, GetStockObject(Window.iLabelFont));
	else
		hOldFont = NULL;
	DrawText( hDC, lpString, -1, &ClientRect,
		DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	if (hOldFont)
		SelectObject(hDC, hOldFont);
	SetBkMode( hDC, iOldMode );
	SetTextColor( hDC, lOldColor );
	}
if ( bFocus )
	DrawFocusRect( lpDraw->hDC, &lpDraw->rcItem );
}

/************************************************************************/
LOCAL void CmdListEdit_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDraw)
/************************************************************************/
{
RECT     ClientRect;
HWND     hControl;
HDC      hDC;
BOOL     bFocus, bHilighted, bGrayed;
STRING   szString;
int      iOldMode;
COLORREF lOldColor;
HBRUSH   hBrush;
LPTSTR    lpString;
DWORD    dwStyle;
HFONT	  hOldFont;
int		 nItems;

// Check state (ODS_??) and not action (ODA_??)
#define OD_GRAYED (ODS_GRAYED | ODS_DISABLED)
#define OD_SELECTED (ODS_SELECTED | ODS_CHECKED)
#define INDENT_SPACES 5

if ( lpDraw->itemID == -1 )
	return;

// Check to see if its focus draw only
if ( lpDraw->itemAction == ODA_FOCUS )
	{
	DrawFocusRect( lpDraw->hDC, &lpDraw->rcItem );
	return;
	}

hDC = lpDraw->hDC;
ClientRect = lpDraw->rcItem;
hControl = GetDlgItem( hDlg, lpDraw->CtlID );

dwStyle = GetWindowLong(hControl, GWL_STYLE);

if (lpDraw->CtlType == ODT_COMBOBOX)
	ComboBox_GetLBText(hControl, lpDraw->itemID, szString);
else
	ListBox_GetText(hControl, lpDraw->itemID, szString);

bGrayed = ( (lpDraw->itemState & OD_GRAYED) ||
	!CmdListEdit_IsEnabled(&CmdListEdit_List, lpDraw->itemID) );

nItems = ListBox_GetCount(hControl);
if (CmdListEdit_fInsertMode &&
	(lpDraw->itemID >= (nItems-CmdListEdit_nRedoItems)) )
	bGrayed = TRUE;
			
bHilighted = ( lpDraw->itemState & OD_SELECTED );
bFocus = ( lpDraw->itemState & ODS_FOCUS );

// Draw the box interior
hBrush = CreateSolidBrush( 
	GetSysColor( bHilighted ? COLOR_HIGHLIGHT : COLOR_WINDOW ) );
FillRect( hDC, &ClientRect, hBrush );
DeleteObject( hBrush );

lpString = SkipSpaces( szString );
//if (fType & OD_LOWERCASE)
//	AnsiLower(szString);

if (lstrlen(lpString))
	{
	// Draw the text
	iOldMode = SetBkMode( hDC, TRANSPARENT );
		
	if (!bGrayed &&
		CmdListEdit_fInsertMode &&
		((lpDraw->itemID >= CmdListEdit_iInsertItem) &&
		(lpDraw->itemID < (nItems-CmdListEdit_nRedoItems))) )
		{
		lOldColor = SetTextColor(hDC, RGB(255, 0, 0));
		}
	else
		{
		lOldColor = SetTextColor( hDC,
			GetSysColor( bHilighted ? COLOR_HIGHLIGHTTEXT  : COLOR_WINDOWTEXT  ) );
		}

	if (dwStyle & CBS_THINFONT)
		hOldFont = (HFONT)SelectObject(hDC, GetStockObject(Window.iLabelFont));
	else
		hOldFont = NULL;
	DrawText( hDC, lpString, -1, &ClientRect,
		DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	if (hOldFont)
		SelectObject(hDC, hOldFont);
	SetBkMode( hDC, iOldMode );
	SetTextColor( hDC, lOldColor );
	}

if ( bGrayed )
	MgxGrayArea(hDC, ClientRect.left, ClientRect.top,
			RectWidth(&ClientRect), RectHeight(&ClientRect) );

if ( bFocus )
	DrawFocusRect( lpDraw->hDC, &lpDraw->rcItem );
}

/************************************************************************/
LOCAL BOOL CmdListEdit_AnyChanges()
/************************************************************************/
{
LPCMDLIST lpCmdList;

// determine whether the list has been changed base on the following:
// 1. Insert Mode has been toggled or
//	2. Insert Mode is on and iInsertIndex has changed or
// 3. Insert Mode is on and nRedoItems has changed or
// 4. The list has changed - an item has been disabled or deleted
lpCmdList = CmdListEdit_lpImage->lpCmdList;
return(
	(lpCmdList->fInsertMode != CmdListEdit_fInsertMode) ||
	(CmdListEdit_fInsertMode && (lpCmdList->iInsertIndex != CmdListEdit_iInsertIndex)) ||
	(CmdListEdit_fInsertMode && (CmdListEdit_nRedoItems != ListGetCount(&lpCmdList->RedoPacketList))) ||
	CmdList_ListChanged(&lpCmdList->UndoPacketList,
		&CmdListEdit_List, ListGetCount(&lpCmdList->UndoPacketList)) );
}

// LOCAL prototypes
LOCAL BOOL MacroEdit_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void MacroEdit_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void MacroEdit_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDraw);
LOCAL void MacroEdit_Enable(HWND hDlg);
LOCAL BOOL MacroEdit_OnOK(HWND hDlg);
LOCAL void MacroEdit_OnListBox(HWND hDlg, UINT codeNotify);
LOCAL void MacroEdit_OnDisable(HWND hDlg);
LOCAL void MacroEdit_OnDelete(HWND hDlg);
LOCAL void MacroEdit_OnClose(HWND hDlg);
LOCAL LPCMDPKT MacroEdit_GetPacket(LPLIST lpPacketList, int nItem);
LOCAL BOOL MacroEdit_IsEnabled(LPLIST lpPacketList, int nItem);
LOCAL int MacroEdit_GetDisableState(HWND hDlg);
LOCAL BOOL MacroEdit_Load(HWND hDlg, LPTSTR lpMacroName);
LOCAL BOOL MacroEdit_CheckSave(HWND hDlg);
LOCAL BOOL MacriEdit_AnyChanges();
LOCAL int MacroEdit_ListGetCount(LPLIST lpList);


static LIST MacroEdit_List;
static LIST MacroEdit_OrigList;
static LPMACROEDIT MacroEdit_lpData;
static STRING MacroEdit_szTitle;

/***********************************************************************/
BOOL WINPROC EXPORT DlgMacroEditProc( HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, MacroEdit_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, MacroEdit_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DRAWITEM, MacroEdit_OnDrawItem);
	HANDLE_DLGMSG(hDlg, WM_CLOSE, MacroEdit_OnClose);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MACRO_EDIT);
	}
}

/************************************************************************/
LOCAL BOOL MacroEdit_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );

MacroEdit_lpData = (LPMACROEDIT)lParam;
ListInit(&MacroEdit_lpData->List);

// save the window title
GetWindowText(hDlg, MacroEdit_szTitle, sizeof(MacroEdit_szTitle));
// zap the list
ListInit(&MacroEdit_List);
ListInit(&MacroEdit_OrigList);
// we continue on, even if the load fails
MacroEdit_Load(hDlg, MacroEdit_lpData->szExtName);
// enable/disable controls
CheckDlgButton(hDlg, IDC_CMDLIST_FULLNAMES, !Control.UseShortCommandNames);
MacroEdit_Enable(hDlg);

return(TRUE);
}

/************************************************************************/
LOCAL void MacroEdit_OnClose(HWND hDlg)
/************************************************************************/
{
if (MacroEdit_CheckSave(hDlg))
	{
	DestroyPacketList(&MacroEdit_List);
	DestroyPacketList(&MacroEdit_OrigList);
	AstralDlgEnd( hDlg, FALSE );
	}
}

/************************************************************************/
LOCAL void MacroEdit_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPCMDPKT lpCmdPkt;
STRING szName;
LIST PlayList;

switch (id)
	{
	case IDC_CMDLIST_EDIT:
	MacroEdit_OnListBox(hDlg, codeNotify);
	break;

	case IDC_MACRO_PLAY:
#ifdef WIN32
	DestroyPacketList(&MacroEdit_OrigList);
	RemoveDisabledPackets(&MacroEdit_List);
	MacroEdit_lpData->List = MacroEdit_List;
	AstralDlgEnd( hDlg, TRUE );
#else
	if (CopyPacketList(&MacroEdit_List, &PlayList))
		{
		// toss disabled packets
		RemoveDisabledPackets(&PlayList);
		// play the macro
	    PlayMacro(NULL, NULL, 1, NO, &PlayList, hDlg);
		DestroyPacketList(&PlayList);
		}
#endif
	break;

	case IDC_CLOSE:
	FORWARD_WM_CLOSE(hDlg, SendMessage);
	break;

	case IDC_CMDLIST_DISABLE:
	MacroEdit_OnDisable(hDlg);
	break;

	case IDC_CMDLIST_DELETE:
	MacroEdit_OnDelete(hDlg);
	break;

	case IDC_MACRO_LOAD:
	if (!MacroEdit_CheckSave(hDlg))
		break;
    if (AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, 
        		IDD_MACRO_LOAD, DlgMacroPlayProc))
		{
		MacroEdit_Load(hDlg, Names.Macro);
		MacroEdit_Enable(hDlg);
		}
	break;

	case IDC_CMDLIST_SAVE:
	// advance to second packet in undo command list
	lpCmdPkt = (LPCMDPKT)ListGetHead(&MacroEdit_List);
	if (CmdList_OnSave(hDlg, lpCmdPkt, NULL, szName))
		{
		STRING szTitle;

		CopyPacketList(&MacroEdit_List, &MacroEdit_OrigList);
	 	MacroEdit_Enable(hDlg);
		lstrcpy(szTitle, MacroEdit_szTitle);
		lstrcat(szTitle, szName);
		SetWindowText(hDlg, szTitle);
		}
	break;

	case IDC_CMDLIST_FULLNAMES:
	lpCmdPkt = (LPCMDPKT)ListGetHead(&MacroEdit_List);
	CmdList_OnFullNames(hDlg, IDC_CMDLIST_EDIT, lpCmdPkt);
	break;

   default:
	break;
   }
}

/************************************************************************/
LOCAL void MacroEdit_OnListBox(HWND hDlg, UINT codeNotify)
/************************************************************************/
{
if (codeNotify == LBN_SELCHANGE) // selection change?
	MacroEdit_Enable(hDlg);
else
if (codeNotify == LBN_MOVEITEM)
	{
	// user has move items in the list box
	LPCMDPKT lpMovePkt, lpDestPkt;
	int nOldPos, nNewPos;

	// get the old and new position of the items
	ListBox_GetMoveItem(GetDlgItem(hDlg, IDC_CMDLIST_EDIT), &nOldPos, &nNewPos);
	if (nOldPos != nNewPos)
		{
		lpMovePkt = MacroEdit_GetPacket(&MacroEdit_List, nOldPos);
		lpDestPkt = MacroEdit_GetPacket(&MacroEdit_List, nNewPos);
		if (lpMovePkt)
			{
			// unlink to packet to move
			ListUnlink(&MacroEdit_List, lpMovePkt);
			if (lpDestPkt)
				{
			  	// always before after the dest
			  	ListInsertBefore(&MacroEdit_List, lpDestPkt, lpMovePkt);
				}
			else
				{
				// if there is no dest packet that means the user is
				// try to move the item to the bottom of the list
				ListAddTail(&MacroEdit_List, lpMovePkt);
				}
			MacroEdit_Enable(hDlg);
			}
		}
	}
}

/************************************************************************/
LOCAL void MacroEdit_OnDisable(HWND hDlg)
/************************************************************************/
{
int iSelCount, iSelected, i, iState;
LPINT lpSelItems;
LPCMDPKT lpCmdPkt;

iState = MacroEdit_GetDisableState(hDlg);
if (iState < 0)
	return;
// flip the state
iState = !iState;

// get the selected items
iSelCount = ListBox_GetSelCount(GetDlgItem(hDlg, IDC_CMDLIST_EDIT));
if (iSelCount)
	{
	lpSelItems = (LPINT)Alloc(sizeof(int)*iSelCount);
	if (!lpSelItems)
		{
		Message(IDS_EMEMALLOC);
		return;
		}
	ListBox_GetSelItems(GetDlgItem(hDlg, IDC_CMDLIST_EDIT), iSelCount, lpSelItems);


	// now disable all items that are selected
	for (i = 0; i < iSelCount; ++i)
		{
		iSelected = lpSelItems[i];
		lpCmdPkt = MacroEdit_GetPacket(&MacroEdit_List, iSelected);
		if (lpCmdPkt)
			lpCmdPkt->fDisabled = iState;
		}
	FreeUp(lpSelItems);
	AstralControlPaint(hDlg, IDC_CMDLIST_EDIT);
	MacroEdit_Enable(hDlg);
	}
}

/************************************************************************/
LOCAL void MacroEdit_OnDelete(HWND hDlg)
/************************************************************************/
{
int iSelCount, iSelected, i, iOffset;
LPINT lpSelItems;
LPCMDPKT lpCmdPkt;

// get the selected items
iSelCount = ListBox_GetSelCount(GetDlgItem(hDlg, IDC_CMDLIST_EDIT));
lpSelItems = (LPINT)Alloc(sizeof(int)*iSelCount);
if (!lpSelItems)
	{
	Message(IDS_EMEMALLOC);
	return;
	}
ListBox_GetSelItems(GetDlgItem(hDlg, IDC_CMDLIST_EDIT), iSelCount, lpSelItems);

// now disable all items that are selected
iOffset = 0;
for (i = 0; i < iSelCount; ++i)
	{
	iSelected = lpSelItems[i] - iOffset;
	lpCmdPkt = MacroEdit_GetPacket(&MacroEdit_List, iSelected);
	if (lpCmdPkt)
		{
		ListUnlink(&MacroEdit_List, lpCmdPkt);
		FreeUpPacket(lpCmdPkt);
		++iOffset;
		ListBox_DeleteString(GetDlgItem(hDlg, IDC_CMDLIST_EDIT), iSelected);
		}
	}
FreeUp(lpSelItems);
MacroEdit_Enable(hDlg);
}

/************************************************************************/
LOCAL int MacroEdit_GetDisableState(HWND hDlg)
/************************************************************************/
{
int iSelCount, iSelected, i;
LPINT lpSelItems;
LPCMDPKT lpCmdPkt;
int iState = -1;

// get the selected items
iSelCount = ListBox_GetSelCount(GetDlgItem(hDlg, IDC_CMDLIST_EDIT));
if (iSelCount > 0)
	{
	lpSelItems = (LPINT)Alloc(sizeof(int)*iSelCount);
	if (!lpSelItems)
		{
		Message(IDS_EMEMALLOC);
		return(iState);
		}
	ListBox_GetSelItems(GetDlgItem(hDlg, IDC_CMDLIST_EDIT), iSelCount, lpSelItems);

	// now disable all items that are selected
	for (i = 0; i < iSelCount; ++i)
		{
		iSelected = lpSelItems[i];
		lpCmdPkt = MacroEdit_GetPacket(&MacroEdit_List, iSelected);
		if (lpCmdPkt)
			{
			if (iState < 0)
				iState = lpCmdPkt->fDisabled;
			else
			if (lpCmdPkt->fDisabled != iState)
				{
				iState = -1;
				break;
				}
			}
		}
	FreeUp(lpSelItems);
	}
return(iState);
}

/************************************************************************/
LOCAL void MacroEdit_Enable(HWND hDlg)
/************************************************************************/
{
int iEditCount;
int iState;
BOOL fEnable = TRUE;
STRING szString;

iState = MacroEdit_GetDisableState(hDlg);
if (iState < 0)
	{
	iState = NO;
	fEnable = FALSE;
	}
if (iState)
	GetDlgItemText(hDlg, IDC_CMDLIST_ENABLETEXT, szString, sizeof(szString));
else
	GetDlgItemText(hDlg, IDC_CMDLIST_DISABLETEXT, szString, sizeof(szString));
SetDlgItemText(hDlg, IDC_CMDLIST_DISABLE, szString);
ControlEnable(hDlg, IDC_CMDLIST_DISABLE, fEnable);

// get the number of items selected in list box
iEditCount = ListBox_GetSelCount(GetDlgItem(hDlg, IDC_CMDLIST_EDIT));

ControlEnable(hDlg, IDC_MACRO_PLAY, MacroEdit_ListGetCount(&MacroEdit_List) > 0 && GetActiveImage());
ControlEnable(hDlg, IDC_CMDLIST_DELETE, iEditCount > 0);
SetDlgItemInt(hDlg, IDC_CMDLIST_SELECTED, iEditCount, NO);
ControlEnable(hDlg, IDC_CMDLIST_FULLNAMES, ListGetCount(&MacroEdit_List) > 0);
ControlEnable(hDlg, IDC_CMDLIST_SAVE, MacroEdit_ListGetCount(&MacroEdit_List) > 0);
}

/************************************************************************/
LOCAL LPCMDPKT MacroEdit_GetPacket(LPLIST lpPacketList, int nItem)
/************************************************************************/
{
LPCMDPKT lpCmdPkt;

lpCmdPkt = (LPCMDPKT)ListGetHead(lpPacketList);
while (--nItem >= 0 && lpCmdPkt)
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
return(lpCmdPkt);
}

/************************************************************************/
LOCAL BOOL MacroEdit_IsEnabled(LPLIST lpPacketList, int nItem)
/************************************************************************/
{
LPCMDPKT lpCmdPkt;

lpCmdPkt = MacroEdit_GetPacket(lpPacketList, nItem);
if (lpCmdPkt)
	return(!lpCmdPkt->fDisabled);
else
	return(FALSE);
}

/************************************************************************/
LOCAL void MacroEdit_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDraw)
/************************************************************************/
{
RECT     ClientRect;
HWND     hControl;
HDC      hDC;
BOOL     bFocus, bHilighted, bGrayed;
STRING   szString;
int      iOldMode;
COLORREF lOldColor;
HBRUSH   hBrush;
LPTSTR    lpString;
DWORD    dwStyle;
HFONT	  hOldFont;
int		 nItems;

// Check state (ODS_??) and not action (ODA_??)
#define OD_GRAYED (ODS_GRAYED | ODS_DISABLED)
#define OD_SELECTED (ODS_SELECTED | ODS_CHECKED)
#define INDENT_SPACES 5

if ( lpDraw->itemID == -1 )
	return;

// Check to see if its focus draw only
if ( lpDraw->itemAction == ODA_FOCUS )
	{
	DrawFocusRect( lpDraw->hDC, &lpDraw->rcItem );
	return;
	}

hDC = lpDraw->hDC;
ClientRect = lpDraw->rcItem;
hControl = GetDlgItem( hDlg, lpDraw->CtlID );

dwStyle = GetWindowLong(hControl, GWL_STYLE);

if (lpDraw->CtlType == ODT_COMBOBOX)
	ComboBox_GetLBText(hControl, lpDraw->itemID, szString);
else
	ListBox_GetText(hControl, lpDraw->itemID, szString);

bGrayed = ( (lpDraw->itemState & OD_GRAYED) ||
	!MacroEdit_IsEnabled(&MacroEdit_List, lpDraw->itemID) );

nItems = ListBox_GetCount(hControl);
			
bHilighted = ( lpDraw->itemState & OD_SELECTED );
bFocus = ( lpDraw->itemState & ODS_FOCUS );

// Draw the box interior
hBrush = CreateSolidBrush( 
	GetSysColor( bHilighted ? COLOR_HIGHLIGHT : COLOR_WINDOW ) );
FillRect( hDC, &ClientRect, hBrush );
DeleteObject( hBrush );

lpString = SkipSpaces( szString );
//if (fType & OD_LOWERCASE)
//	AnsiLower(szString);

if (lstrlen(lpString))
	{
	// Draw the text
	iOldMode = SetBkMode( hDC, TRANSPARENT );
		
	lOldColor = SetTextColor( hDC,
			GetSysColor( bHilighted ? COLOR_HIGHLIGHTTEXT  : COLOR_WINDOWTEXT  ) );

	if (dwStyle & CBS_THINFONT)
		hOldFont = (HFONT)SelectObject(hDC, GetStockObject(Window.iLabelFont));
	else
		hOldFont = NULL;
	DrawText( hDC, lpString, -1, &ClientRect,
		DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	if (hOldFont)
		SelectObject(hDC, hOldFont);
	SetBkMode( hDC, iOldMode );
	SetTextColor( hDC, lOldColor );
	}

if ( bGrayed )
	MgxGrayArea(hDC, ClientRect.left, ClientRect.top,
			RectWidth(&ClientRect), RectHeight(&ClientRect) );

if ( bFocus )
	DrawFocusRect( lpDraw->hDC, &lpDraw->rcItem );
}

/************************************************************************/
LOCAL BOOL MacroEdit_Load(HWND hDlg, LPTSTR lpMacroName)
/************************************************************************/
{
FNAME szFileName;
LPCMDPKT lpCmdPkt;
STRING szTitle;

if ( !LookupExtFile( lpMacroName, szFileName, IDN_MACRO ) )
	return(FALSE);

DestroyPacketList(&MacroEdit_List);
// read in the entire macro file
ReadMacro(szFileName, &MacroEdit_List);

// copy the list so we can see if it changed
CopyPacketList(&MacroEdit_List, &MacroEdit_OrigList);
lpCmdPkt = (LPCMDPKT)ListGetHead(&MacroEdit_List);
CmdList_FillListBox(hDlg, IDC_CMDLIST_EDIT, lpCmdPkt, FALSE);
lstrcpy(szTitle, MacroEdit_szTitle);
if (lpCmdPkt)
	lstrcat(szTitle, lpMacroName);
SetWindowText(hDlg, szTitle);
return(lpCmdPkt != NULL);
}


/************************************************************************/
LOCAL BOOL MacroEdit_AnyChanges()
/************************************************************************/
{
// determine whether the list has been changed base on the following:
// 1. The list has changed - an item has been disabled or deleted
return(CmdList_ListChanged(&MacroEdit_List, &MacroEdit_OrigList));
}


/************************************************************************/
LOCAL BOOL MacroEdit_CheckSave(HWND hDlg)
/************************************************************************/
{
STRING szName;

if (!MacroEdit_AnyChanges())
	return(TRUE);
GetWindowText(hDlg, szName, sizeof(szName));
return(AstralAffirm(IDS_MACROCHANGED, (LPTSTR)szName));
}

/************************************************************************/
LOCAL int MacroEdit_ListGetCount(LPLIST lpList)
/************************************************************************/
{
LPCMDPKT lpCmdPkt;
int iCount = 0;

lpCmdPkt = (LPCMDPKT)ListGetHead(lpList);
while (lpCmdPkt)
	{
	if (!lpCmdPkt->fDisabled)
		++iCount;
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	}
return(iCount);
}

/************************************************************************/
LOCAL void RemoveDisabledPackets(LPLIST lpList)
/************************************************************************/
{
LPCMDPKT lpCmdPkt, lpNext;

lpCmdPkt = (LPCMDPKT)ListGetHead(lpList);
while (lpCmdPkt)
	{
	lpNext = (LPCMDPKT)ListGetNext(lpCmdPkt);
	if (lpCmdPkt->fDisabled)
		{
		ListUnlink(lpList, lpCmdPkt);
		FreeUpPacket(lpCmdPkt);
		}
	lpCmdPkt = lpNext;
	}
}
