//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include <time.h>
#include "id.h"

#ifdef WIN32
extern HANDLE hInstAstral;

typedef struct _taskitem
	{
	LPTHREADDATA 	lpThreadData;
	struct _taskitem FAR *next;
	int				Color;
	} TASKITEM, FAR *LPTASKITEM;

typedef enum
	{
	TM_ADD,
	TM_DELETE,
	TM_MODIFY
	} TM_OPCODE;

// LOCAL prototypes
LOCAL BOOL TaskManager_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void TaskManager_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void TaskManager_OnClose(HWND hDlg);
LOCAL void TaskManager_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem);
LOCAL void TaskManager_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem);
LOCAL void TaskManager_EnableButtons(HWND hDlg);
LOCAL void TaskManager_GetSizes(HWND hCtl, HDC hInDC, LPRECT lpClient,
				LPINT lpDx1, LPINT lpDx2, LPINT lpDx3);
LOCAL BOOL TaskManager_TaskManage(HWND hDlg, LPTHREADDATA lpThreadData, TM_OPCODE op);


LOCAL BOOL GetRectFromList(LPTHREADDATA lpThreadData, LPRECT rItem);
LOCAL BOOL RemoveFromList(LPTHREADDATA lpThreadData);
LOCAL BOOL AddToList(LPTHREADDATA lpThreadData);
LOCAL int FindInList(LPTHREADDATA lpThreadData);
LOCAL LPTHREADDATA GetFromList(int pos);
LOCAL int GetColorFromList(int pos);
LOCAL FreeList();

static HWND hTaskList = NULL;
static HWND hTaskManager = NULL;
static int nItems, dx3;
static char szPercent[10];
static LPTASKITEM lpListHead = NULL;
static int itemSel = LB_ERR;	// current list box selection
static int bHasFocus = FALSE;
static int itemHeight = 0;
static int itemWidth = 0;
static HBRUSH hBoxBrush = NULL;
static HBRUSH hHighBrush = NULL;

/***********************************************************************/
BOOL WINPROC EXPORT DlgTaskManagerProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, TaskManager_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, TaskManager_OnCommand);

	HANDLE_DLGMSG(hDlg, WM_CLOSE, TaskManager_OnClose);
	HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, TaskManager_OnMeasureItem);
	HANDLE_DLGMSG(hDlg, WM_DRAWITEM, TaskManager_OnDrawItem);
	HANDLE_DLGMSG(hDlg, WM_TASKMANAGE, TaskManager_TaskManage);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_TASKMANAGER);
	}
}

/************************************************************************/
LOCAL BOOL TaskManager_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
if (Control.fNewLook)
	SetSmallCaption(hDlg);
RestorePopup( hDlg, "xyTaskMan", RightPopup );
hTaskList = GetDlgItem(hDlg, IDC_TASKLIST);
hTaskManager = hDlg;	  
lpListHead = NULL;
itemSel = LB_ERR;
bHasFocus = FALSE;

hBoxBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW ));
hHighBrush = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT) );

/* this outside loop steps through each document window */
POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
while (TemplatePos)
{
	CDocTemplate * pTemplate = PictPubApp.GetNextDocTemplate(TemplatePos);
	POSITION DocPos = pTemplate->GetFirstDocPosition();
	int i = -1;
	while (DocPos)
	{
	    i++;
	    CServerDoc *pDoc = (CServerDoc *)pTemplate->GetNextDoc (DocPos);    
	    if (pDoc)
	    {
			if (pDoc->GetImage())
			{
				CImage *pImage = pDoc->GetImage();
	    		if (pImage->IsBusy())
	        		AddTaskItem(&pImage->lpCmdList->ThreadData);
			}
			else
			if (pDoc->GetCmdList())
			{
				LPCMDLIST lpCmdList = pDoc->GetCmdList();
				if (lpCmdList->ThreadData.ThreadInfo.hCmdEvent &&
					(WaitForSingleObject(lpCmdList->ThreadData.ThreadInfo.hCmdEvent, 0) == WAIT_OBJECT_0) )
	        		AddTaskItem(&lpCmdList->ThreadData);
			}
	    }
	}
}

TaskManager_EnableButtons(hDlg);

return(TRUE);
}

/************************************************************************/
LOCAL void TaskManager_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPTHREADDATA lpThreadData;

switch (id)
	{
	case IDC_TASKSTOP:
	if (itemSel == LB_ERR)
		break;
	lpThreadData = GetFromList(itemSel);
	if (!lpThreadData)
		break;
	if (lpThreadData->ThreadInfo.fSuspended)
		{
		lpThreadData->ThreadInfo.pThread->ResumeThread();
		lpThreadData->ThreadInfo.fSuspended = NO;
		}
	lpThreadData->Progress.bEscape = YES;
	break;
		
	case IDC_TASKSUSPEND:
	if (itemSel == LB_ERR)
		break;
	lpThreadData = GetFromList(itemSel);
	if (!lpThreadData)
		break;
	if (lpThreadData->ThreadInfo.fSuspended)
		break;
	// don't suspend this thread
	if (lpThreadData->ThreadInfo.pThread->m_nThreadID != GetCurrentThreadId())
		{
		lpThreadData->ThreadInfo.pThread->SuspendThread();
		lpThreadData->ThreadInfo.fSuspended = YES;
		}
	TaskManager_EnableButtons(hDlg);
	break;
		
	case IDC_TASKRESUME:
	if (itemSel == LB_ERR)
		break;
	lpThreadData = GetFromList(itemSel);
	if (!lpThreadData)
		break;
	if (!lpThreadData->ThreadInfo.fSuspended)
		break;
	lpThreadData->ThreadInfo.pThread->ResumeThread();
	lpThreadData->ThreadInfo.fSuspended = NO;
	TaskManager_EnableButtons(hDlg);
	break;
		
	case IDC_TASKLIST:
		switch (codeNotify)
			{
			case LBN_SELCHANGE:
			case LBN_SELCANCEL:
			if (hTaskList)
				itemSel = ListBox_GetCurSel(hTaskList);
			TaskManager_EnableButtons(hDlg);
			break;

			case LBN_SETFOCUS:
			bHasFocus = TRUE;
			break;
			
			case LBN_KILLFOCUS:
			bHasFocus = FALSE;
			break;
			
			default:
			break;
			}

   default:
	break;
   }
}

/************************************************************************/
LOCAL void TaskManager_EnableButtons(HWND hDlg)
/************************************************************************/
{
LPTHREADDATA lpThreadData = NULL;

if (itemSel != LB_ERR)
	lpThreadData = GetFromList(itemSel);


// enable or disable buttons here
ControlEnable(hDlg, IDC_TASKSTOP, lpThreadData &&
			lpThreadData->Progress.bEscapable);
ControlEnable(hDlg, IDC_TASKSUSPEND, lpThreadData &&
			!lpThreadData->ThreadInfo.fSuspended);
ControlEnable(hDlg, IDC_TASKRESUME, lpThreadData &&
			lpThreadData->ThreadInfo.fSuspended);
}

/************************************************************************/
LOCAL void TaskManager_OnClose(HWND hDlg)
/************************************************************************/
{
FreeList();
if (hBoxBrush)
	DeleteObject( hBoxBrush );
if (hHighBrush)
	DeleteObject( hHighBrush );
hTaskList = NULL;
hTaskManager = NULL;
AstralDlgEnd(hDlg, TRUE /* destroy */);
}

/************************************************************************/
LOCAL void TaskManager_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem)
/************************************************************************/
{
	HWND 	hControl;
	HDC 	hDC;
	TEXTMETRIC tm;
	int Height, ComboHeight;
	HFONT     hFont, hOldFont;

#ifndef DLL
    ComboHeight = Control.ComboHeight;
#else
    ComboHeight = 20;
#endif

	if ( lpMeasureItem->itemID == -1 ) // the always visible part
		lpMeasureItem->itemHeight = ComboHeight - 7; // Normally 21-7=14
	else
	{
		hControl = GetDlgItem( hDlg, lpMeasureItem->CtlID );
		hFont = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
		hDC = GetDC(hControl);
		hOldFont = (HFONT)SelectObject(hDC, hFont);
		GetTextMetrics(hDC, &tm);
		SelectObject(hDC, hOldFont);
		Height = tm.tmHeight;
		ReleaseDC(hControl, hDC);
		lpMeasureItem->itemHeight = Height; // Normally 17+3=20
	}
	itemHeight = lpMeasureItem->itemHeight;
}

/************************************************************************/
LOCAL void TaskManager_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem)
/************************************************************************/
{
	RECT ClientRect, rUpdate, rDumb;
	RECT NotDone,Done;
	HWND hControl;
	HDC hDC;
	BOOL bFocus, bHilighted, bGrayed;
	STRING Str1;
	STRING Str2;
	ITEMID idMsg;
	COLORREF TextColor, BkgColor;
	HPEN hOldPen;
	LPTHREADDATA lpThreadData;
	int ret, dx1, dx2, dx3, dx, Perc, len;

	// Check state (ODS_??) and not action (ODA_??)
#define OD_GRAYED (ODS_GRAYED | ODS_DISABLED)
#define OD_SELECTED (ODS_SELECTED | ODS_CHECKED)
#define INDENT_SPACES 5

	if ( lpDrawItem->itemID == -1 )
		return;

	// Check to see if its focus draw only
	if ( lpDrawItem->itemAction == ODA_FOCUS )
	{
		DrawFocusRect( lpDrawItem->hDC, &lpDrawItem->rcItem );
		return;
	}

	hDC = lpDrawItem->hDC;
	ClientRect = lpDrawItem->rcItem;
	hControl = lpDrawItem->hwndItem;

	bGrayed = ((lpDrawItem->itemState & OD_GRAYED) ||
			(lpDrawItem->hwndItem && !IsWindowEnabled( lpDrawItem->hwndItem )));
	bHilighted = ( (lpDrawItem->itemState & OD_SELECTED) && !bGrayed );
	bFocus = ( (lpDrawItem->itemState & ODS_FOCUS) && !bGrayed );

	// get the update region
	ret = GetClipBox(hDC, &rUpdate);
	if (ret == NULLREGION)
		return;
	else if (ret == ERROR)
		rUpdate = ClientRect;

	if (!IntersectRect(&rUpdate, &ClientRect, &rUpdate))
		return;

	lpThreadData = GetFromList(lpDrawItem->itemID);
	if (!lpThreadData)
		return;

	// Draw the box interior
	FillRect( hDC, &rUpdate,  bHilighted ? hHighBrush :hBoxBrush );

	len = GetColorFromList(lpDrawItem->itemID);
	if (len == 0)
		TextColor = RGB(0,0,255);
	else if (len == 1)
		TextColor = RGB(255,0,0);
	else
		TextColor = RGB(0,255,0);
	BkgColor = RGB(255,255,255);

	// draw a line below every entry
	hOldPen = (HPEN)SelectObject(hDC, GetStockObject(BLACK_PEN));

	// make it so the percentage isn't jammed up against the side
	TaskManager_GetSizes(hControl, hDC, &ClientRect, &dx1, &dx2, &dx3);

	// Display the thread name
//	ClientRect.right = ClientRect.left + dx1;
	Str1[0] = 0;
	if (IntersectRect(&rDumb, &ClientRect, &rUpdate))
	{
		lstrcpy(Str1, lpThreadData->ThreadInfo.szThreadName);
		if (Control.WinVer == WV_WINDOWS95)
			stripext(Str1);
		AnsiLower(Str1);
	}	

	// Display the activity
	if (IntersectRect(&rDumb, &ClientRect, &rUpdate))
	{
		idMsg = lpThreadData->Progress.idProgressMsg - IDS_PROGFIRST + IDS_TASKFIRST;
		if (!AstralStrEx(idMsg, Str2, sizeof(Str2)))
      		AstralStrEx(0, Str2, sizeof(Str2));
		lstrcat(Str1, " - ");
		lstrcat(Str1, Str2);
	}	

	// Display progress percentage

	dx = ClientRect.right - ClientRect.left;
	Perc = (WORD)((DWORD)lpThreadData->Progress.Percentage * dx / 100);
#ifdef UNUSED
	if (IntersectRect(&rDumb, &ClientRect, &rUpdate))
	{
   		Perc = (WORD)((DWORD)lpThreadData->Progress.Percentage * dx / 100);
		if (!szPercent[0])
		{
			if (!AstralStrEx(IDS_PERCENT, szPercent, sizeof(szPercent)))
				szPercent[0] = '%';
		}
		Int2Ascii(Str2, lpThreadData->Progress.Percentage);
		lstrcat(Str2, szPercent);
		lstrcat(Str1, " (");
		lstrcat(Str1, Str2);
		lstrcat(Str1, ")");
	}	
#endif
	Done = NotDone = ClientRect;
	NotDone.left += Perc;
    Done.right = NotDone.left;
	len = lstrlen(Str1);

   	SetBkColor(hDC, TextColor);
   	SetTextColor(hDC, BkgColor);
   	ExtTextOut(hDC, ClientRect.left,ClientRect.top,
                  ETO_OPAQUE | ETO_CLIPPED, &Done, Str1, len, NULL);
	GdiFlush();

  	//  Draw the un-finished (ie the percent not-done) side of box.
	SetTextColor(hDC, TextColor);
	SetBkColor(hDC, BkgColor);
   	ExtTextOut(hDC, ClientRect.left,ClientRect.top,
                  ETO_OPAQUE | ETO_CLIPPED, &NotDone, Str1, len, NULL);
	GdiFlush();

	SelectObject(hDC, hOldPen);
	if ( bFocus )
		DrawFocusRect( lpDrawItem->hDC, &lpDrawItem->rcItem );
}

/************************************************************************/
LOCAL void TaskManager_GetSizes(HWND hCtl, HDC hInDC, LPRECT lpClient,
				LPINT lpDx1, LPINT lpDx2, LPINT lpDx3)
/************************************************************************/
{
STRING szString;
SIZE ext;
HDC hDC;
int temp;

// Calculate the size of the area need for the percentage
if (!dx3)
	{
	if (hInDC)
		hDC = hInDC;
	else
		hDC = GetDC(hCtl);

	if (!szPercent[0])
		{
		if (!AstralStrEx(IDS_PERCENT, szPercent, sizeof(szPercent)))
			szPercent[0] = '%';
		}
	Int2Ascii(szString, 999);
	lstrcat(szString, szPercent);
	GetTextExtentPoint(hDC, szString, lstrlen(szString), &ext);
	dx3 = ext.cx;
	if (!hInDC)
		ReleaseDC(hCtl, hDC);
	}
// reduce so percentage isn't jammed up against the side
lpClient->right -= 1;
temp = lpClient->right - lpClient->left - dx3;
*lpDx1 = (temp / 2);
*lpDx2 = temp - *lpDx1;
*lpDx3 = dx3;
}

/************************************************************************/
LOCAL BOOL TaskManager_TaskManage(HWND hDlg, LPTHREADDATA lpThreadData, TM_OPCODE op)
/************************************************************************/
{
int dx1, dx2, dx3;
RECT rItem;
BOOL fRet = FALSE;
DRAWITEMSTRUCT dis;
HFONT     hFont, hOldFont;

if (!hTaskList)
	return(fRet);

// Would be critical if it was ever called by another thread
//EnterCriticalSection(&Control.TaskManCriticalSection);
switch (op)
	{
	case TM_ADD:
		AddToList(lpThreadData);
		itemSel = ListBox_GetCurSel(hTaskList);
		itemWidth = 0; 	// width of items may change due to scroll bars
		fRet = TRUE;
		break;

	case TM_DELETE:
		if (!lpListHead)
			break;
		RemoveFromList(lpThreadData);
		itemSel = ListBox_GetCurSel(hTaskList);
		TaskManager_EnableButtons(AstralDlgGet(IDD_TASKMANAGER));
		itemWidth = 0; 	// width of items may change due to scroll bars
		break;

	case TM_MODIFY:

		if (!lpListHead)
			break;
		if (!GetRectFromList(lpThreadData, &rItem))
			break;

		dis.CtlType = ODT_LISTBOX;
		dis.CtlID = IDC_TASKLIST;
		dis.itemID = FindInList(lpThreadData);
		dis.itemAction = 0;
		dis.itemState = ((itemSel == dis.itemID) ?
						(ODS_SELECTED |	(bHasFocus ? ODS_FOCUS : 0)) :
						0);
		dis.hwndItem = hTaskList;
		dis.hDC = GetDC(hTaskList);
		hFont = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
		hOldFont = (HFONT)SelectObject(dis.hDC, hFont);
		dis.rcItem = rItem;
		dis.itemData = (DWORD)lpThreadData;

		TaskManager_GetSizes(hTaskList, NULL, &rItem, &dx1, &dx2, &dx3);
		rItem.left += dx1;
		if (!lpThreadData->Progress.fNewCycle)
			rItem.left += dx2;
//		InvalidateRect(hTaskList, &rItem, TRUE);
		
		// call instead of InvalidateRect
//		IntersectClipRect(dis.hDC, rItem.left, rItem.top, rItem.right, rItem.bottom);
		TaskManager_OnDrawItem(hDlg, &dis);
		SelectObject(dis.hDC, hOldFont);
		ReleaseDC(hTaskList, dis.hDC);

		fRet = TRUE;
		break;

	default:
		break;
	}
//LeaveCriticalSection(&Control.TaskManCriticalSection);
return(fRet);
}

//************************************************************************
LOCAL BOOL AddToList(LPTHREADDATA lpThreadData)
//************************************************************************
{
	DWORD ret;
	LPTASKITEM lpItem, lpList;

	if (FindInList(lpThreadData) >= 0)
		return(TRUE);

	ret = ListBox_AddItemData(hTaskList, lpThreadData);
	if (ret == LB_ERR|| ret == LB_ERRSPACE)
		return(FALSE);
	if (ret != nItems)
			return(FALSE);
	// get the new stroage
	lpItem = (LPTASKITEM)Alloc(sizeof(LPTASKITEM));
	if (!lpItem)
		return(FALSE);
	lpItem->next = NULL;
	lpItem->lpThreadData = lpThreadData;
	lpItem->Color = 0;
	if (!lpListHead)
		lpListHead = lpItem;
	else
	{
		lpList = lpListHead;
		while (lpList->next)
		  	lpList = lpList->next;
		lpList->next = lpItem;
		lpItem->Color = (lpList->Color + 1) % 3;
	}
	nItems++;
	return(TRUE);
}

//************************************************************************
LOCAL BOOL RemoveFromList(LPTHREADDATA lpThreadData)
//************************************************************************
{
	DWORD ret;
	int i;
	LPTASKITEM lpItem, lpList;

	i = FindInList(lpThreadData);
	if (i < 0)
		return(FALSE);
	ret = ListBox_DeleteString(hTaskList, i);
	itemWidth = 0; 	// width of items may change due to scroll bars
	if (ret == LB_ERR)
		return(FALSE);
	lpList = lpListHead;
	if (lpListHead->lpThreadData == lpThreadData)
	{
		lpListHead = lpListHead->next; 
		FreeUp(lpList);
		--nItems;
		return(TRUE);
	}
	while(lpItem = lpList->next)
	{
		if (lpItem->lpThreadData == lpThreadData)
		{
			lpList->next = lpItem->next;
			FreeUp(lpItem);
			--nItems;
			return(TRUE);
		}
		lpList = lpItem;
	}
	return(FALSE);
}

//************************************************************************
LOCAL BOOL GetRectFromList(LPTHREADDATA lpThreadData, LPRECT lpRect)
//************************************************************************
{
	int i, top;

	i = FindInList(lpThreadData);
	if (i<0)
		return(FALSE);
	top = ListBox_GetTopIndex(hTaskList);
	if (i<top)
		return(FALSE);
	// avoid getting the item rect from list box
	if (!itemWidth)
	{
		if (ListBox_GetItemRect(hTaskList, i, lpRect) == LB_ERR)
			return(FALSE);
		itemWidth = RectWidth(lpRect);
	}

	i -= top;
	lpRect->top = itemHeight * i;
	lpRect->bottom = lpRect->top + itemHeight;
	lpRect->left = 0;
	lpRect->right = itemWidth-1;
	return(TRUE);
}

//************************************************************************
LOCAL int FindInList(LPTHREADDATA lpThreadData)
//************************************************************************
{
 	LPTASKITEM lpList;
	int i;

	lpList = lpListHead;
	i = 0;
	while(lpList)
	{
		if (lpList->lpThreadData == lpThreadData)
			return(i);
		lpList = lpList->next;
		i++;
	}
	return(-1);
}

//************************************************************************
LOCAL int GetColorFromList(int pos)
//************************************************************************
{
 	LPTASKITEM lpList;

	lpList = lpListHead;
	while(pos-- > 0)
	{
		if (!lpList)
			return(NULL);
		lpList = lpList->next;
	}
	if (lpList)
		return(lpList->Color);
	else
		return(0);
}

//************************************************************************
LOCAL LPTHREADDATA GetFromList(int pos)
//************************************************************************
{
 	LPTASKITEM lpList;

	lpList = lpListHead;
	while(pos-- > 0)
	{
		if (!lpList)
			return(NULL);
		lpList = lpList->next;
	}
	if (lpList)
		return(lpList->lpThreadData);
	else
		return(NULL);
}

//************************************************************************
LOCAL FreeList()
//************************************************************************
{
 	LPTASKITEM lpList, lpItem;

	lpList = lpListHead;
	lpListHead = NULL;
	nItems = 0;
	while (lpList)
	{
		lpItem = lpList;
	 	lpList = lpList->next;
		FreeUp((LPTR)lpItem);
	}
	return(TRUE);
}

//************************************************************************
BOOL AddTaskItem(LPTHREADDATA lpThreadData)
/************************************************************************/
{
 	if (!hTaskManager)
		return(FALSE);
	FORWARD_WM_TASKMANAGE(hTaskManager, TM_ADD, lpThreadData, SendMessage);
	return(TRUE);
}

/************************************************************************/
BOOL ModifyTaskItem(LPTHREADDATA lpThreadData)
/************************************************************************/
{
 	if (!hTaskManager)
		return(FALSE);
	if (GetCurrentThreadId() != PictPubApp.GetMainThreadId())
		FORWARD_WM_TASKMANAGE(hTaskManager, TM_MODIFY, lpThreadData, PostMessage);
	else
		FORWARD_WM_TASKMANAGE(hTaskManager, TM_MODIFY, lpThreadData, SendMessage);
	return(TRUE);
}

/************************************************************************/
BOOL DeleteTaskItem(LPTHREADDATA lpThreadData)
/************************************************************************/
{
//	HWND hTask;
//	if (!(hTask = AstralDlgGet(IDD_TASKMANAGER)))

 	if (!hTaskManager)
		return(FALSE);
	FORWARD_WM_TASKMANAGE(hTaskManager, TM_DELETE, lpThreadData, SendMessage);
	return(TRUE);
}
#endif

