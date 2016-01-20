//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "macro.h"

static char szPercent[10];

// Static prototypes
static BOOL HookOn( int iMode );
static void HookOff( void );
static BOOL DemoMessage( void );
static void BeginLine( int x1, int y1, int x2, int y2 );
static void GetNextPoint( LPINT lpx, LPINT lpy );
static BOOL FindAWindow( POINT Point, LPSTR lpString, int iStringLen );
static void WriteEvent( FILE * fp, LPEVENTMSG lpEvent );
static BOOL ReadEvent( FILE * fp, LPEVENTMSG lpEvent );

#define DELAY 500L

// LOCAL prototypes
LOCAL BOOL MacroPlay_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void MacroPlay_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgMacroPlayProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, MacroPlay_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, MacroPlay_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MACROPLAY);
	}
}

/************************************************************************/
LOCAL BOOL MacroPlay_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Macro, IDN_MACRO );
lstrcpy( Names.Saved, Names.Macro );
if (GetDlgItem(hDlg,IDC_MACRO_COUNT))
	InitDlgItemSpin( hDlg, IDC_MACRO_COUNT, 1, NO, 1, 99 );
return(TRUE);
}

/************************************************************************/
LOCAL void MacroPlay_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;

switch (id)
	{
	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		Names.Macro, IDN_MACRO, codeNotify );
	break;

	case IDC_EXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Macro,
		IDN_MACRO, id, NO );
	break;

	case IDOK:
	if (GetDlgItem(hDlg,IDC_MACRO_COUNT))
		Macro.nRepeat = GetDlgItemSpin( hDlg, IDC_MACRO_COUNT, &Bool, NO );
	else
		Macro.nRepeat = 1;
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	lstrcpy( Names.Macro, Names.Saved );
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL MacroRecord_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void MacroRecord_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgMacroRecordProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, MacroRecord_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, MacroRecord_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MACRORECORD);
	}
}

/************************************************************************/
LOCAL BOOL MacroRecord_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Macro, IDN_MACRO );
lstrcpy( Names.Saved, Names.Macro );
return(TRUE);
}

/************************************************************************/
LOCAL void MacroRecord_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
FNAME szFileName;

switch (id)
	{
	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		Names.Macro, IDN_MACRO, codeNotify );
	break;

	case IDC_EXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Macro,
		IDN_MACRO, id, NO );
	break;

	case IDOK:
	if ( LookupExtFile(Names.Macro, szFileName, IDN_MACRO) )
		{
		if (AstralOKCancel(IDS_OVERWRITEEXTFILE,
			(LPSTR)Names.Macro) == IDCANCEL)
			break;
		}
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	lstrcpy( Names.Macro, Names.Saved );
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL BatchMacro_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void BatchMacro_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void BatchMacro_OnBatchAddFiles(HWND hDlg);
LOCAL void BatchMacro_OnBatchAddMacros(HWND hDlg);
LOCAL void BatchMacro_OnBatchQueue(HWND hDlg, UINT codeNotify);
LOCAL void BatchMacro_OnBatchDelete(HWND hDlg);
LOCAL LPQUEUEITEM BatchMacro_GetSelectedItem(HWND hDlg, int nItem);
LOCAL int BatchMacro_AddFileName(HWND hDlg, ITEMID idControl, LPSTR lpPathName);
LOCAL void BatchMacro_FillMacroList(HWND hDlg);
LOCAL BOOL BatchMacro_ListsDiffer(LPLIST lpList1, LPLIST lpList2);
LOCAL void BatchMacro_Enable(HWND hDlg);

LPLIST lpQueueList;

/***********************************************************************/
BOOL WINPROC EXPORT DlgBatchMacroProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, BatchMacro_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, BatchMacro_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MACRO_BATCH);
	}
}

/************************************************************************/
LOCAL BOOL BatchMacro_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
lpQueueList = (LPLIST)lParam;
ListInit(lpQueueList);
BatchMacro_Enable(hDlg);
return(TRUE);
}

/************************************************************************/
LOCAL void BatchMacro_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPQUEUEITEM lpQueueItem;

switch (id)
	{
   case IDC_BATCH_DELETE:
	BatchMacro_OnBatchDelete(hDlg);
   break;
	case IDC_BATCH_QUEUE:
	BatchMacro_OnBatchQueue(hDlg, codeNotify);
	break;

	case IDC_BATCH_ADDFILES:
	BatchMacro_OnBatchAddFiles(hDlg);
	break;

	case IDC_BATCH_ADDMACROS:
	BatchMacro_OnBatchAddMacros(hDlg);
	break;
	
	case IDC_BATCH_OPTIONS:
	AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, 
    			IDD_BATCH_OPTIONS, DlgBatchOptionsProc);
	break;

	case IDC_BATCH_PLAY:
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	lpQueueItem = (LPQUEUEITEM)ListGetHead(lpQueueList);
	while (lpQueueItem)
	{
		DestroyPtrList(&lpQueueItem->MacroList);
	 	lpQueueItem = (LPQUEUEITEM)ListGetNext(lpQueueItem);
	}
	DestroyPtrList(lpQueueList);
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

/***********************************************************************/
LOCAL void BatchMacro_OnBatchAddFiles(HWND hDlg)
/***********************************************************************/
{
    OPENSTATE   info;
    int         i, index, iCount;
    FNAME       szName;
    LPSTR       lpString;
	LPQUEUEITEM	lpItem;
	HWND 		hControl;

	hControl = GetDlgItem(hDlg, IDC_BATCH_QUEUE);
	ASSERT(hControl);

	// call the browser to select some image files
	info.bImported = FALSE;
    info.bSaving = FALSE;
    info.szFileSpec[0] = info.szAlbum[0] = info.szPath[0] = '\0';
    info.idFileType = 0;
    info.wFileAttr = -1;
    info.idDataType = IDC_SAVELA;
    AstralStr(IDS_BATCHOPEN_TITLE, &lpString);
    lstrcpy(info.szTitle, lpString);
    AstralStr(IDS_BATCHOPEN_BUTTON, &lpString);
    lstrcpy(info.szOK, lpString);
    if ( !(DoOpenDlg( hDlg, &info)))
        return;

	iCount = ListBox_GetCount(hControl);
	if (iCount)
		ListBox_SelItemRange(hControl, FALSE, 0, iCount-1);

    for (i=0; i<info.numFiles;i++)
    {
        lstrcpy(szName, info.lpFileList[i]);
        if (!PromptForVolume(szName, &info.lpFileListVolumes[i], YES))
            continue;
		lpItem = (LPQUEUEITEM)Alloc(sizeof(QUEUEITEM));
		if (!lpItem)
		{
			Message(IDS_EMEMALLOC);
			break;
		}
		clr((LPTR)lpItem, sizeof(QUEUEITEM));
		lstrcpy(lpItem->szFileName, szName);
		lpItem->cmsInfo = info.cmsInfo;
		ListAddTail(lpQueueList, lpItem);
		index = BatchMacro_AddFileName(hDlg, IDC_BATCH_QUEUE, lpItem->szFileName);
		ListBox_SetSel(hControl, TRUE, index);
    }
    FreeUp(info.lpFileList);
	BatchMacro_FillMacroList(hDlg);
	BatchMacro_Enable(hDlg);
}
		
/***********************************************************************/
LOCAL void BatchMacro_OnBatchAddMacros(HWND hDlg)
/***********************************************************************/
{
	LPMACROITEM lpMacroItem;
	LPQUEUEITEM lpQueueItem;
	int iCount, i;
	LPINT lpSelItems;
	HWND hControl;

	hControl = GetDlgItem(hDlg, IDC_BATCH_QUEUE);
	ASSERT(hControl);

	// get the current selection
	iCount = ListBox_GetSelCount(hControl);
	if (iCount <= 0)
		return;

	lpSelItems = (LPINT)Alloc(sizeof(int)*iCount);
	if (!lpSelItems)
	{
		Message(IDS_EMEMALLOC);
		return;
	}
	ListBox_GetSelItems(hControl, iCount, lpSelItems);

	if (AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, 
    			IDD_MACRO_LOAD, DlgMacroPlayProc))
	{
		for (i = 0; i < iCount; ++i)
		{
			
			lpQueueItem = BatchMacro_GetSelectedItem(hDlg, lpSelItems[i]);
			if (!lpQueueItem)
				continue;

			lpMacroItem = (LPMACROITEM)Alloc(sizeof(MACROITEM));
			if (!lpMacroItem)
			{
				Message(IDS_EMEMALLOC);
				return;
			}
			clr((LPTR)lpMacroItem, sizeof(MACROITEM));
			lstrcpy(lpMacroItem->szMacro, Names.Macro);
			ListAddTail(&lpQueueItem->MacroList, lpMacroItem);
		}
//		iCount = ListBox_GetCount(hControl);
//		if (iCount)
//			ListBox_SelItemRange(hControl, FALSE, 0, iCount-1);
//		ListBox_SetSel(hControl, TRUE, lpSelItems[0]);
		BatchMacro_FillMacroList(hDlg);
		BatchMacro_Enable(hDlg);
	}
	FreeUp(lpSelItems);
}

/***********************************************************************/
LOCAL void BatchMacro_OnBatchDelete(HWND hDlg)
/***********************************************************************/
   {
	LPMACROITEM lpMacroItem;
	LPQUEUEITEM lpQueueItem;
	HWND 		   hFilesList = GetDlgItem(hDlg, IDC_BATCH_QUEUE);
	HWND 		   hMacroList = GetDlgItem(hDlg, IDC_BATCH_MACROLIST);
	int         FileCountSel = ListBox_GetSelCount(hFilesList);
   int         MacroSel    = ListBox_GetCurSel(hMacroList);
   int         i,j;
	LPINT       lpSelItems;

	if (FileCountSel <= 0)
	   return;

	lpSelItems = (LPINT)Alloc(sizeof(int)*FileCountSel);
	if (!lpSelItems)
	   {
	   Message(IDS_EMEMALLOC);
	   return;
	   }
	ListBox_GetSelItems(hFilesList, FileCountSel, lpSelItems);
   if (MacroSel != -1)     // nothing selected then delete from IDC_NATCH_QUEUE
      {
		for (i = FileCountSel-1; i >= 0 ; i--)
		   {
			lpQueueItem = BatchMacro_GetSelectedItem(hDlg, lpSelItems[i]);
			if (!lpQueueItem)
				continue;
	      lpMacroItem = (LPMACROITEM)ListGetHead(&lpQueueItem->MacroList);
         j = MacroSel;
	      while (lpMacroItem && j)
	         {
		      lpMacroItem = (LPMACROITEM)ListGetNext(lpMacroItem);
		      j--;
	         }
         ListUnlink(&lpQueueItem->MacroList, lpMacroItem);
	      FreeUp(lpMacroItem);
         }
      }
   else
      {
		for (i = FileCountSel-1; i >= 0 ; i--)
		   {
			lpQueueItem = BatchMacro_GetSelectedItem(hDlg, lpSelItems[i]);
			if (!lpQueueItem)
				continue;
         ListUnlink(lpQueueList, lpQueueItem);
		   DestroyPtrList(&lpQueueItem->MacroList);
	      FreeUp(lpQueueItem);
         }
	   ListBox_ResetContent(hFilesList);
	   lpQueueItem = (LPQUEUEITEM)ListGetHead(lpQueueList);
	   while (lpQueueItem)
	      {
	      BatchMacro_AddFileName(hDlg, IDC_BATCH_QUEUE, lpQueueItem->szFileName);
	 	   lpQueueItem = (LPQUEUEITEM)ListGetNext(lpQueueItem);
	      }
      }

   FreeUp(lpSelItems);
	BatchMacro_FillMacroList(hDlg);
	BatchMacro_Enable(hDlg);
   }

/***********************************************************************/
LOCAL void BatchMacro_OnBatchQueue(HWND hDlg, UINT codeNotify)
/***********************************************************************/
{
	if (codeNotify == LBN_SELCHANGE)
	{
		BatchMacro_FillMacroList(hDlg);
		BatchMacro_Enable(hDlg);
	}
}

/***********************************************************************/
LOCAL LPQUEUEITEM BatchMacro_GetSelectedItem(HWND hDlg, int nItem)
/***********************************************************************/
{
	int iCurSel, iCount;
	LPQUEUEITEM lpItem;
	HWND hControl;

	hControl = GetDlgItem(hDlg, IDC_BATCH_QUEUE);
	ASSERT(hControl);

	// get the current selection
	iCount = ListBox_GetSelCount(hControl);
	if (nItem < 0)
	{
		if (iCount != 1)
			return(NULL);

		ListBox_GetSelItems(hControl, 1, &iCurSel);
	}
	else
		iCurSel = nItem;

	// look thru list to get the proper file list
	lpItem = (LPQUEUEITEM)ListGetHead(lpQueueList);
	while (lpItem && iCurSel)
	{
		lpItem = (LPQUEUEITEM)ListGetNext(lpItem);
		--iCurSel;
	}
	ASSERT(lpItem);

	return(lpItem);
}

/***********************************************************************/
LOCAL int BatchMacro_AddFileName(HWND hDlg, ITEMID idControl, LPSTR lpPathName)
/***********************************************************************/
{
	HWND	hControl;

	hControl = GetDlgItem(hDlg, idControl);
	ASSERT(hControl);

	return(ListBox_AddString(hControl, lpPathName));
}

/***********************************************************************/
LOCAL void BatchMacro_FillMacroList(HWND hDlg)
/***********************************************************************/
{
	LPQUEUEITEM lpFirstItem, lpNextItem;
	LPMACROITEM lpItem;
	LPINT lpSelItems;
	int iCount, i;

	HWND hControl = GetDlgItem(hDlg, IDC_BATCH_MACROLIST);
	ASSERT(hControl);
	HWND hQueue = GetDlgItem(hDlg, IDC_BATCH_QUEUE);
	ASSERT(hQueue);

	ListBox_ResetContent(hControl);

	// see if all the selected items have the
	// same macro list
	iCount = ListBox_GetSelCount(hQueue);
	if (iCount <= 0)
		return;

	lpSelItems = (LPINT)Alloc(sizeof(int)*iCount);
	if (!lpSelItems)
	{
		Message(IDS_EMEMALLOC);
		return;
	}
	ListBox_GetSelItems(hQueue, iCount, lpSelItems);

	lpFirstItem = BatchMacro_GetSelectedItem(hDlg, lpSelItems[0]);
	if (!lpFirstItem)
	{
		FreeUp(lpSelItems);
		return;
	}

	for (i = 1; i < iCount; ++i)
	{
		lpNextItem = BatchMacro_GetSelectedItem(hDlg, lpSelItems[i]);
		if (!lpNextItem)
			continue;
		if (BatchMacro_ListsDiffer(	&lpFirstItem->MacroList,
									&lpNextItem->MacroList))
		{
			FreeUp(lpSelItems);
			return;
		}
	}

	FreeUp(lpSelItems);

	lpItem = (LPMACROITEM)ListGetHead(&lpFirstItem->MacroList);
	while (lpItem)
	{
		ListBox_AddString(hControl, lpItem->szMacro);
		lpItem = (LPMACROITEM)ListGetNext(lpItem);
	}
}

/***********************************************************************/
LOCAL BOOL BatchMacro_ListsDiffer(LPLIST lpList1, LPLIST lpList2)
/***********************************************************************/
{
	LPMACROITEM lpItem1, lpItem2;

	if (ListGetCount(lpList1) != ListGetCount(lpList2))
		return(TRUE);
	lpItem1 = (LPMACROITEM)ListGetHead(lpList1);
	lpItem2 = (LPMACROITEM)ListGetHead(lpList2);
	while (lpItem1 && lpItem2)
	{
		if (!StringsEqual(lpItem1->szMacro, lpItem2->szMacro))
			return(TRUE);
		lpItem1 = (LPMACROITEM)ListGetNext(lpItem1);
		lpItem2 = (LPMACROITEM)ListGetNext(lpItem2);
	}
	return(FALSE);
}

/***********************************************************************/
LOCAL void BatchMacro_Enable(HWND hDlg)
/***********************************************************************/
{
	int iCount;
	BOOL fEnable;

	iCount = ListBox_GetSelCount(GetDlgItem(hDlg, IDC_BATCH_QUEUE));
	ControlEnable(hDlg, IDC_BATCH_ADDMACROS, iCount > 0);

	// in order to enable play there must be:
	// 1. at least one image in the queue
	fEnable = ListGetCount(lpQueueList) > 0;
	ControlEnable(hDlg, IDC_BATCH_PLAY, fEnable);
}

// LOCAL prototypes
LOCAL BOOL BatchOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void BatchOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL int BatchOptions_AddToCombo(LPSTR lpAlbum, long lData);

static ITEMID FileType_Table[] =
	{
	IDT_PPCOMPOUND,		
	IDT_PP,		
	IDT_TIFF,	
	IDT_TARGA, 	
	IDT_GIF, 	
	IDT_BMP, 	
	IDT_DIB, 	
	IDT_PCX, 
	IDT_EPS,
	IDT_DCS,
	IDT_JPEG, 	
	IDT_SCITEX, 
	IDT_PP4
	};

/***********************************************************************/
BOOL WINPROC EXPORT DlgBatchOptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, BatchOptions_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, BatchOptions_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_BATCH_OPTIONS);
	}
}

/************************************************************************/
LOCAL BOOL BatchOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );

CheckDlgButton(hDlg, IDC_CLOSEWHENDONE, Macro.fBatchClose);
CheckComboItem(hDlg, IDC_BATCHSAVE, IDC_BATCHSAVEFIRST,
				IDC_BATCHSAVELAST, Macro.BatchSave);

MakeFullPath(Macro.BatchDir);
if (!CheckDir(Macro.BatchDir))
	Macro.BatchDir[0] = '\0';
FixPath(Macro.BatchDir);

SetDlgItemText(hDlg, IDC_DIRNAME, Macro.BatchDir);
ShowPane(hDlg, IDC_PANE0, IDC_PANE1, Macro.BatchSave == IDC_SAVETOALBUM);
ShowPane(hDlg, IDC_PANE1, IDC_PANE2, Macro.BatchSave == IDC_SAVETODIR);
ShowPane(hDlg, IDC_PANE2, IDC_PANE3,
	Macro.BatchSave == IDC_SAVETOALBUM || Macro.BatchSave == IDC_SAVETODIR);
NonBSREnumAlbum(BatchOptions_AddToCombo, (long)GetDlgItem(hDlg, IDC_ALBUMLIST));
ComboBox_SelectString(GetDlgItem(hDlg, IDC_ALBUMLIST), -1, Macro.BatchAlbum);

CheckDlgButton(hDlg, IDC_BATCHCHANGETYPE, Macro.fBatchChangeType);

TableCheckComboItem(hDlg, IDC_BATCHFILETYPE, FileType_Table,
		    sizeof(FileType_Table)/sizeof(ITEMID),
		    LocalFileType(Macro.BatchFileType),
		    FALSE);

ControlEnable(hDlg, IDC_BATCHFILETYPE, Macro.fBatchChangeType);

return(TRUE);
}

/************************************************************************/
LOCAL int BatchOptions_AddToCombo(LPSTR lpAlbum, long lData)
/************************************************************************/
{
	HWND hControl = (HWND)lData;
	
	ComboBox_AddString(hControl, lpAlbum);

	return(TRUE);
}

/************************************************************************/
LOCAL void BatchOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;
int iSel;
FNAME szDirName;

switch (id)
	{
	case IDC_ALBUMLIST:
	if (codeNotify != CBN_SELCHANGE)
		break;
	iSel = ComboBox_GetCurSel(GetDlgItem(hDlg, id));
	ComboBox_GetLBText(GetDlgItem(hDlg, id), iSel, Macro.BatchAlbum);
	break;

	case IDC_CLOSEWHENDONE:
	Macro.fBatchClose = !Macro.fBatchClose;
	CheckDlgButton(hDlg, IDC_CLOSEWHENDONE, Macro.fBatchClose);
	break;

	case IDC_BATCHSAVE:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Macro.BatchSave = id;
	ShowPane(hDlg, IDC_PANE0, IDC_PANE1, Macro.BatchSave == IDC_SAVETOALBUM);
	ShowPane(hDlg, IDC_PANE1, IDC_PANE2, Macro.BatchSave == IDC_SAVETODIR);
	ShowPane(hDlg, IDC_PANE2, IDC_PANE3,
		Macro.BatchSave == IDC_SAVETOALBUM || Macro.BatchSave == IDC_SAVETODIR);
	break;

	case IDC_BATCHCHANGETYPE:
	Macro.fBatchChangeType = !Macro.fBatchChangeType;
	CheckDlgButton(hDlg, IDC_BATCHCHANGETYPE, Macro.fBatchChangeType);
	ControlEnable(hDlg, IDC_BATCHFILETYPE, Macro.fBatchChangeType);
	break;

	case IDC_BATCHFILETYPE:
	if (! (id = TableHandleCombo(hDlg, FileType_Table,
					    sizeof(FileType_Table)/sizeof(ITEMID),
					    id, codeNotify)))
            break;
	Macro.BatchFileType = AppFileType(id);
	break;

	case IDOK:
	GetDlgItemText( hDlg, IDC_DIRNAME, szDirName, sizeof(szDirName));
	MakeFullPath(szDirName);
 	if (!CheckDir(szDirName))
	{
		if (Macro.BatchSave == IDC_SAVETODIR)
		{
			Message(IDS_EINVALIDDIR, (LPSTR)szDirName);
			break;
		}
	}
	else
	{
		lstrcpy(Macro.BatchDir, szDirName);
		FixPath( AnsiLower( Macro.BatchDir ) );
	}
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, FALSE );
	break;

	default:
	break;
	}
}

// LOCAL prototypes
LOCAL BOOL MacroStatus_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void MacroStatus_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void MacroStatus_OnDestroy(HWND hDlg);
LOCAL void MacroStatus_ProgressProc(LPPROGRESS lpProgress);
LOCAL void MacroStatus_CmdNotifyProc(LPCMDPKT lpCmdPkt);
LOCAL BOOL MacroStatus_AbortCheck();
LOCAL void MacroStatus_Enable(HWND hDlg);

typedef struct
	{
	int iCount;
	int iTotal;
	BOOL fAbort;
	BOOL fPaused;
	ITEMID idOldDialog;
	HWND hOldDlg;
	ITEMID idLastProgressMsg;
	LPPROGRESSUPDATEPROC OldProgressUpdateProc;
	LPCMDNOTIFYPROC OldCmdNotifyProc;
	LPABORTPROC OldAbortProc;
	} STATUSINFO, FAR *LPSTATUSINFO;

static HWND MacroStatus_hDlg;
static ITEMID MacroStatus_idDialog;

/***********************************************************************/
BOOL WINPROC EXPORT DlgMacroStatusProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, MacroStatus_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, MacroStatus_OnCommand);
	MONITOR_DLGMSG(hDlg, WM_DESTROY, MacroStatus_OnDestroy);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, MacroStatus_idDialog);
	}
}

/************************************************************************/
LOCAL BOOL MacroStatus_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPSTATUSINFO lpInfo = (LPSTATUSINFO)Alloc(sizeof(STATUSINFO));
if (!lpInfo)
	{
	AstralDlgEnd(hDlg, TRUE /* destroy */);
	return(TRUE);
	}

SetWindowLong(hDlg, DWL_USER, (long)lpInfo);

CenterPopup( hDlg );

LPMACROSETUP lpSetup = (LPMACROSETUP)lParam;
if (lpSetup->lpFileName)
	SetDlgItemText(hDlg, IDC_MACRO_FILENAME, lpSetup->lpFileName);
lpInfo->iTotal = lpSetup->iTotal;
lpInfo->iCount = 0;
lpInfo->fAbort = FALSE;
lpInfo->fPaused = NO;
lpInfo->idLastProgressMsg = 0;
if (!szPercent[0])
	{
	if (!AstralStrEx(IDS_PERCENT, szPercent, sizeof(szPercent)))
		szPercent[0] = '%';
	}
lpInfo->hOldDlg = MacroStatus_hDlg;
lpInfo->idOldDialog = MacroStatus_idDialog;
lpInfo->OldProgressUpdateProc = ProgressSetUpdateProc(MacroStatus_ProgressProc);
lpInfo->OldAbortProc = SetCmdAbortProc(MacroStatus_AbortCheck);
lpInfo->OldCmdNotifyProc = SetCmdNotifyProc(MacroStatus_CmdNotifyProc);

MacroStatus_hDlg = hDlg;
MacroStatus_idDialog = lpSetup->idDialog;

MacroStatus_Enable(hDlg);

return(TRUE);
}

/************************************************************************/
LOCAL void MacroStatus_OnDestroy(HWND hDlg)
/************************************************************************/
{
LPSTATUSINFO lpInfo = (LPSTATUSINFO)GetWindowLong(hDlg, DWL_USER);
if (lpInfo)
	{
	SetCmdNotifyProc(lpInfo->OldCmdNotifyProc);
	MacroSetAbortProc(lpInfo->OldAbortProc);
	ProgressSetUpdateProc(lpInfo->OldProgressUpdateProc);
	MacroStatus_hDlg = lpInfo->hOldDlg;
	MacroStatus_idDialog = lpInfo->idOldDialog;
	FreeUp(lpInfo);
	}
}

/************************************************************************/
LOCAL void MacroStatus_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
MSG msg;

LPSTATUSINFO lpInfo = (LPSTATUSINFO)GetWindowLong(hDlg, DWL_USER);
if (!lpInfo)
	return;
switch (id)
	{
	case IDC_MACRO_PAUSE:
		lpInfo->fPaused = YES;
		MacroStatus_Enable(hDlg);
		while (lpInfo->fPaused && GetMessage(&msg, MacroStatus_hDlg, 0, 0))
			{
			if ( !IsDialogMessage(hDlg, &msg) )
				{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				}
			}
	break;
	case IDC_MACRO_RESUME:
		lpInfo->fPaused = NO;
		MacroStatus_Enable(hDlg);
	break;
	case IDC_MACRO_STOP:
		lpInfo->fPaused = NO;
		lpInfo->fAbort = TRUE;
		MacroStatus_Enable(hDlg);
	break;
	default:
	break;
	}
}

/************************************************************************/
LOCAL BOOL MacroStatus_AbortCheck()
/************************************************************************/
{
LPSTATUSINFO lpInfo = (LPSTATUSINFO)GetWindowLong(MacroStatus_hDlg, DWL_USER);
if (!lpInfo)
	return(FALSE);
return(lpInfo->fAbort);
}

/************************************************************************/
LOCAL void MacroStatus_CmdNotifyProc(LPCMDPKT lpCmdPkt)
/************************************************************************/
{
STRING szString;
int iPercent;

LPSTATUSINFO lpInfo = (LPSTATUSINFO)GetWindowLong(MacroStatus_hDlg, DWL_USER);
if (!lpInfo)
	return;
++lpInfo->iCount;
iPercent = ((long)lpInfo->iCount * 100L) / (long)lpInfo->iTotal;
Int2Ascii(szString, iPercent);
lstrcat(szString, szPercent);
SetDlgItemText(MacroStatus_hDlg, IDC_MACRO_PERCENT, szString);
}

/************************************************************************/
LOCAL void MacroStatus_ProgressProc(LPPROGRESS lpProgress)
/************************************************************************/
{
STRING	szString;
ITEMID idProgressMsg;
MSG msg;

LPSTATUSINFO lpInfo = (LPSTATUSINFO)GetWindowLong(MacroStatus_hDlg, DWL_USER);
if (!lpInfo)
	return;

if (lpInfo->fAbort)
	lpProgress->bEscape = TRUE;

if (lpProgress->bChanged)
	{
	if (lpProgress->idProgressMsg)
		idProgressMsg = lpProgress->idProgressMsg - IDS_PROGFIRST + IDS_TASKFIRST; 
	else
	if (lpProgress->bEscapable)
		idProgressMsg = TASK_ID(IDS_UNDO1);
	else
		idProgressMsg = TASK_ID(IDS_UNDO2);
	if (idProgressMsg != lpInfo->idLastProgressMsg)
		{
		if ( !AstralStrEx( idProgressMsg, szString, sizeof(szString) ) )
			szString[0] = '\0';
		SetDlgItemText(MacroStatus_hDlg, IDC_TASK_NAME, szString);
		lpInfo->idLastProgressMsg = idProgressMsg;
		}
		
	if (lpProgress->bDisplayClock)
	{
		Int2Ascii(szString, lpProgress->Percentage);
		lstrcat(szString, szPercent);
		SetDlgItemText(MacroStatus_hDlg, IDC_TASK_PERCENT, szString);
	}
	else
		SetDlgItemText(MacroStatus_hDlg, IDC_TASK_PERCENT, (LPSTR)"");
	
	}
while ( !lpInfo->fAbort && PeekMessage(&msg, MacroStatus_hDlg, 0, 0, PM_REMOVE) )
	{
	if ( !MacroStatus_hDlg || !IsDialogMessage(MacroStatus_hDlg, &msg) )
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
	}
}

/************************************************************************/
LOCAL void MacroStatus_Enable(HWND hDlg)
/************************************************************************/
{
LPSTATUSINFO lpInfo = (LPSTATUSINFO)GetWindowLong(hDlg, DWL_USER);
if (!lpInfo)
	return;
ControlEnable(hDlg, IDC_MACRO_PAUSE, !lpInfo->fPaused && !lpInfo->fAbort);
ControlEnable(hDlg, IDC_MACRO_RESUME, lpInfo->fPaused && !lpInfo->fAbort);
ControlEnable(hDlg, IDC_MACRO_STOP, !lpInfo->fAbort);
}

/***********************************************************************/
BOOL PlayJournalMacro(LPSTR lpFileName)
/***********************************************************************/
{
if ( !InstallPlayer( lpFileName ) )
	return(FALSE);
while ( IsJournalPlaying() )
    PictPubApp.PumpMessage();
return(TRUE);
}

/***********************************************************************/
BOOL RecordJournalMacro(LPSTR lpFileName)
/***********************************************************************/
{
if ( !InstallRecorder( lpFileName ) )
	return(FALSE);
if (!AddExtFile(Names.Macro, lpFileName, IDN_MACRO))
	{
	RemoveRecorder();
	FileDelete( lpFileName );
	return(FALSE);
	}
return(TRUE);
}

static HOOKPROC lpfnHook;
static HHOOK hHook;
static EVENTMSG EMsg;
static DWORD dwBaseTime;
static int PgmMode = -1;
static FILE *fpMsg, *hFile;
static BOOL fOldEnable;
static int iRecorderDown;

/***********************************************************************/
LRESULT CALLBACK EXPORT Recorder( int iCode, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
LPEVENTMSG lpEM;

lpEM = (LPEVENTMSG)lParam;
if ( iCode == HC_ACTION )
	{
	/* Keep track of the up/down state */
	if ( lpEM->message == WM_LBUTTONDOWN )
		iRecorderDown |= 1;
	else
	if ( lpEM->message == WM_RBUTTONDOWN)
		iRecorderDown |= 2;
	else
	if ( lpEM->message == WM_LBUTTONUP )
		iRecorderDown &= 2;
	else
	if ( lpEM->message == WM_RBUTTONUP )
		iRecorderDown &= 1;
	else
	if ( lpEM->message == WM_MOUSEMOVE )
		if ( !iRecorderDown ) // Moves without mouse down are not stored away
			return( CallNextHookEx( hHook, iCode, wParam, (DWORD)lpEM ) );

	if ( !dwBaseTime )
		dwBaseTime = lpEM->time;

	lpEM->time -= dwBaseTime;
	WriteEvent( hFile, lpEM );
	lpEM->time += dwBaseTime;
	}
return( CallNextHookEx( hHook, iCode, wParam, (DWORD)lpEM ) );
}


/***********************************************************************/
BOOL InstallRecorder( LPSTR lpRecordFile )
/***********************************************************************/
{
LPSTR lp;
FNAME szRecordFile;
STRING szString;
FNAME	 OEMName;

lstrcpy(szRecordFile, lpRecordFile);
if ( hFile )
	fclose( hFile );
AnsiToOem(szRecordFile, OEMName);
if ( !(hFile = fopen( OEMName, "wct" )) )
	return( FALSE );

if ( AstralStr( IDS_APPNAME, &lp ) )
	{
	lstrcpy( szString, lp );
	if ( AstralStr( IDS_MACRORECORD, &lp ) )
		{
		lstrcat( szString, lp );
		SetWindowText( PictPubApp.Get_hWndAstral(), szString );
		}
	}

dwBaseTime = 0; // Zero out the base time; set it with the 1st message
iRecorderDown = 0; // Zero out the mouse down flags
if (!HookOn( WH_JOURNALRECORD ))
	RemoveRecorder();
return( TRUE );
}


/***********************************************************************/
BOOL RemoveRecorder()
/***********************************************************************/
{
LPSTR lp;

if ( PgmMode != WH_JOURNALRECORD )
	return( NO );

if ( hFile )
	{
	fclose( hFile );
	hFile = NULL;
	}

if ( AstralStr( IDS_APPNAME, &lp ) )
	SetWindowText( PictPubApp.Get_hWndAstral(), lp );

HookOff();
return( YES );
}


/***********************************************************************/
LRESULT CALLBACK EXPORT Player( int iCode, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
int i;
DWORD dwDelay;
static POINT cpt;
static int Moving;
#define SKIPCOUNT 8 // Use 4 for a tutorial (slower)
LPEVENTMSG lpEM;

lpEM = (LPEVENTMSG)lParam;

if ( iCode == HC_SKIP )
	{ /* Advance to the next event */
	if ( Moving ) /* haven't used the last event, so don't read a new one */
		return( 0L );

	if ( !EMsg.message && EMsg.paramH )
		{ // If the last message was a NULL message, put up the text
		if ( !DemoMessage() )
			{
			RemovePlayer();
			return( CallNextHookEx( hHook, iCode, wParam, (DWORD)lpEM ) );
			}
		}

	/* Get a new event from the file and store it in a static structure */
	if ( !ReadEvent( hFile, &EMsg ) )
		{
		RemovePlayer();
		return( CallNextHookEx( hHook, iCode, wParam, (DWORD)lpEM ) );
		}

	return( 0L );
	}

if ( iCode != HC_GETNEXT )
	return( CallNextHookEx( hHook, iCode, wParam, (DWORD)lpEM ) );

// Handle any automatic mouse movements
if ( EMsg.message > WM_MOUSEFIRST && EMsg.message <= WM_MOUSELAST )
	{ /* If this is a mouse message... */
	if ( !Moving ) // Start moving...
		GetCursorPos( &cpt );
	if ( cpt.x != EMsg.paramL || cpt.y != EMsg.paramH )
		{ /* Send a WM_MOUSEMOVE message to get closer */
		if ( !Moving )
			{
			BeginLine( cpt.x, cpt.y, EMsg.paramL, EMsg.paramH );
			Moving = YES;
			}
		i = SKIPCOUNT;
		while ( --i >= 0 )
			{ /* move several points at a time */
			GetNextPoint( (LPINT)&cpt.x, (LPINT)&cpt.y );
			if ( cpt.x == EMsg.paramL && cpt.y == EMsg.paramH )
				break;
			}
		lpEM->message = WM_MOUSEMOVE;
		lpEM->paramL = cpt.x;
		lpEM->paramH = cpt.y;
		lpEM->time = GetTickCount(); /* right now */
		return( 0L ); /* without delay */
		}
	}

/* Process the current event */
Moving = NO;
lpEM->message = EMsg.message;
lpEM->paramL = EMsg.paramL;
lpEM->paramH = EMsg.paramH;

dwDelay = GetTickCount() - dwBaseTime; /* Ticks since we started playing */
if ( dwDelay > EMsg.time )
	{ /* If it should have happened in past, */
	  /* bring it and future events up to date */
	dwBaseTime += (dwDelay-EMsg.time);
	dwDelay = EMsg.time;
	}
dwDelay = EMsg.time - dwDelay;
if ( dwDelay > DELAY )
	{ /* If it should have happened too far into */
	  /* the future process it a little bit sooner */
	dwBaseTime -= (dwDelay-DELAY);
	dwDelay = DELAY;
	}

lpEM->time = EMsg.time + dwBaseTime;
return( dwDelay );
}


/***********************************************************************/
BOOL InstallPlayer( LPSTR lpPlayFile )
/***********************************************************************/
{
LPSTR lp;
char szMessageFile[MAX_FNAME_LEN];
STRING szString;
FNAME szPlayFile;

lstrcpy(szPlayFile, lpPlayFile);
lstrcpy( szMessageFile, szPlayFile );
if ( lp = lStrChr( szMessageFile, '.' ) )
		lstrcpy( lp, ".MSG" );
else	lstrcpy( szMessageFile, "DEMO.MSG" );

if ( hFile )
	fclose( hFile );
AnsiToOem(szPlayFile, szPlayFile);
if ( !(hFile = fopen( szPlayFile, "r" )) )
	return( FALSE );
if ( fpMsg )
	fclose( fpMsg );
fpMsg = fopen( szMessageFile, "r" );

/* pre-read the first event message */
if ( !ReadEvent( hFile, &EMsg ) )
	{
	fclose( hFile );
	hFile = NULL;
	if ( fpMsg )
		fclose( fpMsg );
	fpMsg = NULL;
	return( FALSE );
	}

if ( AstralStr( IDS_APPNAME, &lp ) )
	{
	lstrcpy( szString, lp );
	if ( AstralStr( IDS_MACROPLAY, &lp ) )
		{
		lstrcat( szString, lp );
		SetWindowText( PictPubApp.Get_hWndAstral(), szString );
		}
	}

dwBaseTime = GetTickCount(); /* Set the start time variable */
//fOldEnable = EnablePaintAbort( NO );
if (!HookOn( WH_JOURNALPLAYBACK ))
	RemovePlayer();
return( TRUE );
}


/***********************************************************************/
BOOL RemovePlayer()
/***********************************************************************/
{
LPSTR lp;

if ( PgmMode != WH_JOURNALPLAYBACK )
	return( NO );

if ( hFile )
	{
	fclose( hFile );
	hFile = NULL;
	}

if ( fpMsg )
	{
	fclose( fpMsg );
	fpMsg = NULL;
	}

if ( AstralStr( IDS_APPNAME, &lp ) )
	SetWindowText( PictPubApp.Get_hWndAstral(), lp );

// For demo	SendMessage( PictPubApp.Get_hWndAstral(), WM_CLOSE, 0, 0L);
//EnablePaintAbort( fOldEnable );
HookOff();
return( YES );
}


/***********************************************************************/
static BOOL HookOn( int iMode )
/***********************************************************************/
{
if ( iMode != WH_JOURNALRECORD && iMode != WH_JOURNALPLAYBACK )
	return(FALSE);
if ( lpfnHook ) // Already have a hook
	return(FALSE);

if ( iMode == WH_JOURNALPLAYBACK )
	lpfnHook = (HOOKPROC)MakeProcInstance( (FARPROC)Player, PictPubApp.Get_hInstAstral() );
else
if ( iMode == WH_JOURNALRECORD )
	lpfnHook = (HOOKPROC)MakeProcInstance( (FARPROC)Recorder, PictPubApp.Get_hInstAstral() );
hHook = SetWindowsHookEx( iMode, lpfnHook, PictPubApp.Get_hInstAstral(), 0 );
PgmMode = iMode;
return(hHook != NULL);
}


/***********************************************************************/
static void HookOff( void )
/***********************************************************************/
{
if ( hHook )
	{
	UnhookWindowsHookEx( hHook );
	hHook = NULL;
	}
if ( lpfnHook )
	{
	FreeProcInstance( (FARPROC)lpfnHook );
	lpfnHook = NULL;
	}
PgmMode = -1;
}


/***********************************************************************/
static BOOL DemoMessage()
/***********************************************************************/
{
int retc;
HWND hFocusWindow;
LPSTR lp;

if ( PgmMode != WH_JOURNALPLAYBACK )
	return( YES );
if ( !fpMsg )
	return( YES );
lp = (LPSTR)LineBuffer[3];
*lp = 0;
if ( fscanf( fpMsg, "%[^|]|", lp ) == EOF )
	return( YES );

if ( CAPSLOCK )
	return( YES );

/* Skip leading RETURNS */
while ( *lp && ((BYTE)*lp) <= ' ' )
	lp++;

hFocusWindow = GetFocus();
HookOff();
retc = MessageBox( PictPubApp.Get_hWndAstral(), lp, "Picture Publisher Tutorial",
	MB_OKCANCEL | MB_TASKMODAL );
HookOn( WH_JOURNALPLAYBACK );
SetFocus( hFocusWindow );

if ( retc == IDCANCEL )
	return( NO );
return( YES );
}


static LFIXED dx, dy, ax, ay;
static int px, py, ix, iy;

/************************************************************************/
static void BeginLine( int x1, int y1, int x2, int y2 )
/************************************************************************/
{
int x, y;

if ( x1 > x2 )
	{ x = x1 - x2; ix = -1; }
else	{ x = x2 - x1; ix = 1; }

if ( y1 > y2 )
	{ y = y1 - y2; iy = -1; }
else	{ y = y2 - y1; iy = 1; }

if ( x < y )
	{ dy = FUNITY; dx = FGET( x, y ); }
else	{ dx = FUNITY; dy = FGET( y, x ); }

px = x1; ax = .5;
py = y1; ay = .5;
}


/************************************************************************/
static void GetNextPoint( LPINT lpx, LPINT lpy )
/************************************************************************/
{
if ( ( ax += dx ) > FUNITY )
	{ ax -= FUNITY; px += ix; }
if ( ( ay += dy ) > FUNITY )
	{ ay -= FUNITY; py += iy; }
*lpx = px;
*lpy = py;
}

/************************************************************************/
BOOL IsJournalRecording()
/************************************************************************/
{
return( PgmMode == WH_JOURNALRECORD );
}


/************************************************************************/
BOOL IsJournalPlaying()
/************************************************************************/
{
return( PgmMode == WH_JOURNALPLAYBACK );
}

/***********************************************************************/
static BOOL FindAWindow( POINT Point, LPSTR lpString, int iStringLen )
/***********************************************************************/
{
HWND hWnd, hMatchWindow;

if ( !(hWnd = WindowFromPoint( Point )) )
	return( NO );
MapWindowPoints( NULL, hWnd, &Point, 1 );
while ( hMatchWindow = ChildWindowFromPoint( hWnd, Point ) )
	{
	if ( hMatchWindow == hWnd )
		break;
	MapWindowPoints( hWnd, hMatchWindow, &Point, 1 );
	hWnd = hMatchWindow;
	}

GetWindowText( hWnd, lpString, iStringLen );
return( YES );
}


/***********************************************************************/
static void WriteEvent( FILE *fp, LPEVENTMSG lpEvent )
/***********************************************************************/
{
WORD c;
P_STR ptr;
char szString[MAX_STR_LEN];

switch( lpEvent->message )
	{
	case WM_KEYDOWN:
		ptr = "KD";
		break;
	case WM_KEYUP:
		ptr = "KU";
		break;
	case WM_SYSKEYDOWN:
		ptr = "SD";
		break;
	case WM_SYSKEYUP:
		ptr = "SU";
		break;
	case WM_MOUSEMOVE:
		ptr = "MO";
		break;
	case WM_LBUTTONDOWN:
		ptr = "LD";
		break;
	case WM_LBUTTONUP:
		ptr = "LU";
		break;
	case WM_LBUTTONDBLCLK:
		ptr = "L2";
		break;
	case WM_RBUTTONDOWN:
		ptr = "RD";
		break;
	case WM_RBUTTONUP:
		ptr = "RU";
		break;
	case WM_RBUTTONDBLCLK:
		ptr = "R2";
		break;
	default:
		ptr = itoa( lpEvent->message, szString, 10 );
	}

fprintf( fp, "%s (%d,%d) %lx", ptr, lpEvent->paramL, lpEvent->paramH,
	lpEvent->time );
if ( lpEvent->message == WM_KEYUP )
	{
	c = lpEvent->paramL;
	c = LOBYTE(c);
	if ( c > ' ' && c < 127 )
			fprintf( fp, "\t; %c\r\n", c );
	else	fprintf( fp, "\t; %s\r\n", itoa( c, szString, 10 ) );
	}
//else
//if ( lpEvent->message == WM_LBUTTONUP )
//	{
//	l = MAKELONG( lpEvent->paramL, lpEvent->paramH );
//	clr( szString, sizeof(szString) );
//	if ( FindAWindow( MAKEPOINT(l), szString, sizeof(szString) ) )
//			fprintf( fp, "\t; %s\r\n", szString );
//	else	fprintf( fp, "\r\n" );
//	}
else
	fprintf( fp, "\r\n" );
}


/***********************************************************************/
static BOOL ReadEvent( FILE *fp, LPEVENTMSG lpEvent )
/***********************************************************************/
{
char szString[1024];
int type, cmd;
EVENTMSG Event;

if ( fscanf( fp, "%s (%d,%d) %lx%*[^\r\n]",
     szString, &Event.paramL, &Event.paramH, &Event.time ) == EOF )
	return( FALSE );

lpEvent->paramL = Event.paramL;
lpEvent->paramH = Event.paramH;
lpEvent->time   = Event.time;

type = *szString;
cmd = *(szString+1);
switch( type )
	{
	case 'M':
		lpEvent->message = WM_MOUSEMOVE;
		break;
	case 'K':
		switch( cmd )
		{
		case 'D':
			lpEvent->message = WM_KEYDOWN;
			break;
		case 'U':
			lpEvent->message = WM_KEYUP;
			break;
		}
		break;
	case 'S':
		switch( cmd )
		{
		case 'D':
			lpEvent->message = WM_SYSKEYDOWN;
			break;
		case 'U':
			lpEvent->message = WM_SYSKEYUP;
			break;
		}
		break;
	case 'L':
		switch( cmd )
		{
		case 'D':
			lpEvent->message = WM_LBUTTONDOWN;
			break;
		case 'U':
			lpEvent->message = WM_LBUTTONUP;
			break;
		case '2':
			lpEvent->message = WM_LBUTTONDBLCLK;
			break;
		}
		break;
	case 'R':
		switch( cmd )
		{
		case 'D':
			lpEvent->message = WM_RBUTTONDOWN;
			break;
		case 'U':
			lpEvent->message = WM_RBUTTONUP;
			break;
		case '2':
			lpEvent->message = WM_RBUTTONDBLCLK;
			break;
		}
		break;
	default:
		lpEvent->message = atoi( szString );
	}

return( YES );
}

// LOCAL prototypes
LOCAL BOOL Package_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Package_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

static LPSTR lpDirName;

/***********************************************************************/
BOOL WINPROC EXPORT DlgPackageProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Package_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Package_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PACKAGEPP5);
	}
}

/************************************************************************/
LOCAL BOOL Package_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );

lpDirName = (LPSTR)lParam;
SetDlgItemText(hDlg, IDC_PACKAGEPATH, lpDirName);

return(TRUE);
}

/************************************************************************/
LOCAL void Package_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;
FNAME szDirName;

switch (id)
	{
	case IDOK:
	GetDlgItemText(hDlg, IDC_PACKAGEPATH, szDirName, sizeof(szDirName));
	MakeFullPath(szDirName);
 	if (!CheckDir(szDirName))
	{
		Message(IDS_EINVALIDDIR, (LPSTR)szDirName);
		break;
	}
	lstrcpy(lpDirName, szDirName);
	FixPath(lpDirName);
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	lstrcpy( Names.Macro, Names.Saved );
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}




