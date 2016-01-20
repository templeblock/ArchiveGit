//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "ppafx.h"
#include "svrview.h"
#include "ipframe.h"
#include "olelistb.h"
#include "bsrtypes.h"

struct PFFSTRUCT
{
	PFFSTRUCT(COlelistBox *pLB,HWND hOwnerWnd=NULL)
		{
			ASSERT(pLB);
			pListBox = pLB;
			hRealOwner = hOwnerWnd;
		};

	HWND hRealOwner;
   	COlelistBox *pListBox;
};


#define PASTE_FILELISTID (ExtBrowser.PasteViewThumbnails ? IDC_BSR_THUMBS : IDC_BSR_FILELIST)

// LOCAL prototypes

LOCAL BOOL PasteFromFile_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL BOOL PasteFromFile_OnDestroy(HWND hDlg);
LOCAL void PasteFromFile_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR *lpDrawItem);
LOCAL void PasteFromFile_OnMeasureItem(HWND hDlg, LPMEASUREITEMSTRUCT lpMeasureItem);
LOCAL void PasteFromFile_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

BOOL WINPROC EXPORT DlgPasteFromFileInfoProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC EXPORT DlgPasteFromPrefProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

LOCAL BOOL do_paste_from_file(LPSTR PasteImageFile, LPSTR PasteMaskFile, LPSTR Clipboard);
LOCAL void init_paste(LPOBJECT lpObject, LPIMAGE lpImage);
static void enable_info_button(HWND hDlg);
static void enable_open_button(HWND hDlg);
static void set_files_text(HWND hDlg);

// callback for extended browser
LOCAL LPFRAME get_ext_preview(LPSTR lpExtName, WORD wType,  LPPREVIEWINFO lpInfo,  LPINT bCompatible);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPasteFromFileProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
	// handle stuff in the dialog
	switch (msg)
	{
		
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, PasteFromFile_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, PasteFromFile_OnMeasureItem);
		HANDLE_DLGMSG(hDlg, WM_DRAWITEM, PasteFromFile_OnDrawItem);
		HANDLE_DLGMSG(hDlg, WM_COMMAND, PasteFromFile_OnCommand);
		HANDLE_DLGMSG(hDlg, WM_DESTROY, PasteFromFile_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_PALETTECHANGED, ExtBsr_OnPaletteChanged);
	
		default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PASTEFROMFILE);
	}
}

/************************************************************************/
LOCAL BOOL PasteFromFile_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	EXTSTATE ExtState;
 	RECT     r;
   	HWND hListBox;
   	COlelistBox *pListBox;
			
	RestorePopup( hDlg, "xyClipBrowser", RightPopup );
	hListBox = GetDlgItem(hDlg,PASTE_FILELISTID);
	if (hListBox)
	   {
      DWORD Style;
      if (ExtBrowser.PasteViewThumbnails)
         {
         Style = LBS_NOTIFY | LBS_SORT | LBS_MULTIPLESEL | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | 
                 LBS_NOINTEGRALHEIGHT | LBS_MULTICOLUMN | LBS_EXTENDEDSEL |
                 WS_BORDER | WS_HSCROLL | LBS_DISABLENOSCROLL | WS_CHILD | WS_VISIBLE;
 		 pListBox = new COlelistBox;
         }
      else
         {
         Style = LBS_STANDARD | LBS_MULTIPLESEL | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_EXTENDEDSEL | WS_CHILD | WS_VISIBLE | WS_TABSTOP;
 		 pListBox = new COlelistBox;
         }
   	GetWindowRect(hListBox, &r);
   	MapWindowPoints(NULL, hDlg, (LPPOINT)&r, 2);
   	DestroyWindow(hListBox);
   	pListBox->Create(Style, r, CWnd::FromHandle(hDlg), PASTE_FILELISTID);
	   }
	
	if (!ExtBsr_OnInitDialog(hDlg, hWndFocus, lParam))
		return(FALSE);
	lstrcpy( Names.Saved, Names.Clipboard );
	
	// setup Exteneded browser handling
	ExtState.wExtType = IDN_CLIPBOARD;
	ExtState.lpGetFileProc = GetExtFileName_Bsr;
	ExtState.lpGetPreview = get_ext_preview;
	ExtState.bMultiListBox = TRUE;
	ExtState.ViewThumbnails = ExtBrowser.PasteViewThumbnails;
	ExtState.AutoThumbnails = ExtBrowser.PasteAutoThumbnails;
	PFFSTRUCT *pPF = new PFFSTRUCT(pListBox,(HWND)lParam); 
	ExtBsr_Init(hDlg, &ExtState, (void FAR*)pPF);
	
	// fill file list 
	InitExtName( hDlg, PASTE_FILELISTID, IDC_FILENAME, Names.Clipboard,
		IDN_CLIPBOARD );
	return(TRUE);
}

LOCAL BOOL PasteFromFile_OnDestroy(HWND hDlg)
{
	LPBSRSTATE lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (lpBsr && lpBsr->OpenState.lpExtUserData)
	{
		if (((PFFSTRUCT*)lpBsr->OpenState.lpExtUserData)->pListBox)	   
			delete ((PFFSTRUCT*)lpBsr->OpenState.lpExtUserData)->pListBox;		
		delete (PFFSTRUCT*)lpBsr->OpenState.lpExtUserData;
		lpBsr->OpenState.lpExtUserData = NULL;
	}
	SetWindowLong(hDlg, DWL_USER, (LONG)lpBsr);

	return ExtBsr_OnDestroy(hDlg);
}

/************************************************************************/
LOCAL void PasteFromFile_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	int num,i;
	FNAME *lpResult;
	FNAME szFileName;
	BOOL bThumb, bool;
	
	// default handling of extended browser
	if (!ExtBsr_OnCommand(hDlg, id, hControl, codeNotify))
		return;
	
	switch (id)
	{
		case IDC_OK:
		case IDOK:
			lpResult = ExtBsr_GetSelection(hDlg, &num);
			if (!lpResult || num < 1)
				break;
			lstrcpy(Names.Clipboard, lpResult[0]);
			if (!LookupExtFile(Names.Clipboard, Names.PasteImageFile, IDN_CLIPBOARD))
				break;
			if (!LookupExtFileN(Names.Clipboard, Names.PasteMaskFile, IDN_MASK, NO))
				Names.PasteMaskFile[0] = 0;
			do_paste_from_file(Names.PasteImageFile, Names.PasteMaskFile, Names.Clipboard);
		break;
	
		case IDCANCEL:
		{
			LPBSRSTATE lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
			if (lpBsr && lpBsr->OpenState.lpExtUserData)
			{
				if (((PFFSTRUCT*)lpBsr->OpenState.lpExtUserData)->hRealOwner)
					SendMessage(((PFFSTRUCT*)lpBsr->OpenState.lpExtUserData)->
						hRealOwner,WM_PASTEFROMFILECLOSED,(WPARAM)hDlg,0L);
			}
			lstrcpy( Names.Clipboard, Names.Saved );
			AstralDlgEnd( hDlg, TRUE );
		}
		break;
	
		case IDC_EXTMANAGE:
		case IDC_PASTEEXTMANAGE:
			lpResult = ExtBsr_GetSelection(hDlg, &num);
			SetDlgItemText(hDlg, IDC_FILENAME, "");
			if (lpResult && num == 1)
			{
				if (LookupExtFileN( lpResult[0], szFileName, IDN_CLIPBOARD, NO ) )
					SetDlgItemText(hDlg, IDC_FILENAME, filename(Lowercase(szFileName)));
			}
			if (lpResult)
				FreeUp(lpResult);
			// use proper menu to keep delete from being grayed in multiple select
			i = (num > 1) ? IDC_PASTEEXTMANAGE_MULT:IDC_PASTEEXTMANAGE;
			PopupMenuEx( hDlg, id, i, IDC_FILENAME );
		break;
	
		case IDC_ADDEXT:
			ExtNameManager( hDlg, PASTE_FILELISTID, IDC_FILENAME,
				Names.Clipboard, IDN_CLIPBOARD, id, NO );
			set_files_text(hDlg);
			enable_info_button(hDlg);
			enable_open_button(hDlg);
		break;
		
		case IDC_RENAMEEXT:
			lpResult = ExtBsr_GetSelection(hDlg, &num);
			if (!lpResult)
				break;
			if (num != 1)
			{
				FreeUp(lpResult);
				break;
			}
			lstrcpy(Names.Clipboard, lpResult[0]);
			ExtNameManager( hDlg, PASTE_FILELISTID, IDC_FILENAME,
				Names.Clipboard, IDN_CLIPBOARD, id, NO );
			FreeUp(lpResult);
			set_files_text(hDlg);
			enable_info_button(hDlg);
			enable_open_button(hDlg);
		break;
		
		case IDC_DELETEEXT:
		case IDC_PASTE_DELETEEXT:
			// multi delete allowed
			lpResult = ExtBsr_GetSelection(hDlg, &num);
			if (!lpResult)
				break;
			if (num < 1)
			{
				FreeUp(lpResult);
				break;
			}
			bool = TRUE;
			AstralCursor(IDC_WAIT);
			for (i=0;i<num;i++)
			{
				if ( !LookupExtFileN( lpResult[i], szFileName, IDN_CLIPBOARD, NO ) )
					lstrcpy(szFileName, (LPSTR)"No File");
				if (bool)
				{
					id = AstralYesNoYesToAll(hDlg, i<num-1, IDD_OKDEL_PASTEFROM, 
							IDS_DELETEEXTFILE_MULTI, (LPSTR)"Clipboard", lpResult[i], Lowercase(szFileName));
					if (id == IDC_BSR_NO)
						continue;
					else if (id == IDC_YESTOALL)
						bool = FALSE;
					else if (id != IDOK)
						break;
					// the dialog can change cursors
					AstralCursor(IDC_WAIT);
				}
				if ( DeleteExtFile( lpResult[i], IDN_CLIPBOARD ) )
					DeleteExtFile( lpResult[i], IDN_MASK );
			}
			InitExtNameN( hDlg, PASTE_FILELISTID, IDC_FILENAME, lpResult[0], IDN_CLIPBOARD, NO, NO );
			FreeUp(lpResult);
			set_files_text(hDlg);
			enable_info_button(hDlg);
			enable_open_button(hDlg);
			AstralCursor(NULL);
		break;
		
		case IDC_STATUSINFO:
			EnableWindow(hDlg, FALSE);
			AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_EXT_INFO, DlgPasteFromFileInfoProc );
			EnableWindow(hDlg, TRUE);
			SetFocus(hDlg);
		break;
		case IDC_OPTIONS:
			bThumb = ExtBrowser.PasteViewThumbnails;
			if (!AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_PASTEFROM_OPTIONS, DlgPasteFromPrefProc ))
				break;
			if (ExtBrowser.PasteViewThumbnails != bThumb)
			{
				AstralDlgEnd( hDlg, TRUE );
     			AstralDlg( YES, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), ExtBrowser.PasteViewThumbnails ? IDD_PASTEFROMFILE_THUMB:IDD_PASTEFROMFILE,
			         DlgPasteFromFileProc);
			}
		break;
	   	default:
		break;
	}
}


//************************************************************************
LOCAL void PasteFromFile_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR *lpDrawItem)
//************************************************************************
{
	if (ExtBsr_OnDrawItem(hDlg, lpDrawItem))
		Dialog_OnDrawItem(hDlg, lpDrawItem);
}

//************************************************************************
LOCAL void PasteFromFile_OnMeasureItem(HWND hDlg, LPMEASUREITEMSTRUCT lpMeasureItem)
//************************************************************************
{
	if(ExtBsr_OnMeasureItem(hDlg, lpMeasureItem))
		Dialog_OnMeasureItem(hDlg, lpMeasureItem);
}



//************************************************************************
// PARAMETERS:
//				hDlg- The dialog or NULL.  If NULL then it will find the dialog
// DESCRIPTION:
//		Call when paste from extended names list may have changed.
//		Updates the list in the PasteFrom Dialog.
// RETURNS:
//		True if the dialog was valid and the list was updated.
//************************************************************************
BOOL UpdatePasteFromFile(HWND hDlg)
//************************************************************************
{
	if (!hDlg)
	{
		hDlg = AstralDlgGet(ExtBrowser.PasteViewThumbnails ? IDD_PASTEFROMFILE_THUMB:IDD_PASTEFROMFILE);
		if (!hDlg)
			return(FALSE);
	}
	InitExtName( hDlg, PASTE_FILELISTID, IDC_FILENAME, Names.Clipboard,
		IDN_CLIPBOARD );
	set_files_text(hDlg);
	enable_info_button(hDlg);
	enable_open_button(hDlg);
	return(TRUE);
}


LOCAL void PasteFromFileInfo_OnControlEnable(HWND hDlg, int fFlags);

//************************************************************************
BOOL WINPROC EXPORT DlgPasteFromFileInfoProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
//************************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, PasteFromFileInfo_OnControlEnable);
	    default:
			return ExtBsrInfo_HandleDefaults(hDlg, msg, wParam, lParam, IDD_EXT_INFO);
	}  
		
}

//************************************************************************
//	Handle change in file
//************************************************************************
LOCAL void PasteFromFileInfo_OnControlEnable(HWND hDlg, int fFlags)
//************************************************************************
{
	FNAME szName, szFile, szMask;
	char	szPtName[256];
	
	// fill defaults
	ExtBsrInfo_OnControlEnable(hDlg, fFlags);
	
	if (!ExtBsrInfo_GetCurrentName(hDlg, szName))
		return;
	// fill specific parts
	SetDlgItemText(hDlg, IDC_INFO_EXTNAME, szName);
	if (!LookupExtFileN(szName, szFile, IDN_CLIPBOARD, NO))
		szFile[0] = '\0';
	else if (LookupExtFileN(szName, szMask, IDN_MASK, NO))
	{
		lstrcat (szFile, (LPSTR)", ");
		lstrcat (szFile, szMask);
	}
   SetDlgItemText( hDlg, IDC_INFO_COLORMANAGED,
		CmsGetPTNameFromFile( szFile, szPtName ) );

	SetDlgItemText(hDlg, IDC_INFO_SMALLFILENAME, szFile);
}



LOCAL BOOL PasteFromPref_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void PasteFromPref_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

typedef struct _pastefrompref{
	BOOL PasteAutoThumbnails;
	BOOL PasteThumbCreate;		
	BOOL PasteViewThumbnails;
} PASTEFROMPREF, FAR *LPPASTEFROMPREF;


//************************************************************************
BOOL WINPROC EXPORT DlgPasteFromPrefProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
//************************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, PasteFromPref_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, PasteFromPref_OnCommand);
	    default:	  
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PASTEFROM_OPTIONS);
	}
}



//************************************************************************
LOCAL BOOL PasteFromPref_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	LPPASTEFROMPREF lpData;
	
	CenterPopup(hDlg);	
	lpData = (LPPASTEFROMPREF)Alloc(sizeof(PASTEFROMPREF));
	if (!lpData)
		return(FALSE);
 	SetWindowLong(hDlg, DWL_USER, (LONG)lpData);
	
	// copy to local storage
	lpData->PasteAutoThumbnails = ExtBrowser.PasteAutoThumbnails;;		
	lpData->PasteThumbCreate = ExtBrowser.PasteThumbCreate;		
	lpData->PasteViewThumbnails= ExtBrowser.PasteViewThumbnails;
	
	// init items
	CheckDlgButton(hDlg,IDC_BSR_AUTOTHM,lpData->PasteAutoThumbnails);
	CheckDlgButton(hDlg,IDC_BSR_CREATE_ON_SAVE,lpData->PasteThumbCreate);
	CheckDlgButton(hDlg,IDM_BSR_VIEWTHUMBS,lpData->PasteViewThumbnails);
	return(TRUE);
}


//************************************************************************
LOCAL void PasteFromPref_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify) 
//************************************************************************
{
	LPPASTEFROMPREF lpData;

	// get the local state variables for the dialog.
 	lpData = (LPPASTEFROMPREF)GetWindowLong(hDlg, DWL_USER);
	if (!lpData)	return;

	switch(id)
	{
		case IDC_BSR_AUTOTHM:	   
			lpData->PasteAutoThumbnails= !lpData->PasteAutoThumbnails;
			CheckDlgButton(hDlg,id,lpData->PasteAutoThumbnails);
		break;
		case IDC_BSR_CREATE_ON_SAVE:	   
			lpData->PasteThumbCreate= !lpData->PasteThumbCreate;
			CheckDlgButton(hDlg,id,lpData->PasteThumbCreate);
		break;
		case IDM_BSR_VIEWTHUMBS:	   
			lpData->PasteViewThumbnails= !lpData->PasteViewThumbnails;
			CheckDlgButton(hDlg,id,lpData->PasteViewThumbnails);
		break;
		case IDOK:
			ExtBrowser.PasteAutoThumbnails = lpData->PasteAutoThumbnails;
			ExtBrowser.PasteThumbCreate = lpData->PasteThumbCreate;		
			ExtBrowser.PasteViewThumbnails = lpData->PasteViewThumbnails;
			AstralDlgEnd(hDlg, TRUE);
		break;
		case IDCANCEL:
			AstralDlgEnd(hDlg, FALSE);
		break;
		default:
		break;
	}  // switch command
}


//************************************************************************
// 		get_ext_preview
// PARAMETERS:
//					LPSTR lpFileName- The name of file to get.
//					WORD wType- The extended type.
//					LPPREVIEWINFO lpInfo - filled by callback with all available information.
//					LPINT bCompatible- callback fills with false if further default processing should occur.
// DESCRIPTION:
// 			callback for extended browser
//			Gets a preview frame for the given extended name file.
// RETURNS:
//			A frame containing the preview, or NULL.
//************************************************************************
LOCAL LPFRAME get_ext_preview(LPSTR lpFileName, WORD wType,  LPPREVIEWINFO lpInfo,  LPINT lpbCompatible)
{
    LPOBJECT lpObject;
	OBJECTLIST ObjList;
	int	idFileType;
	RECT	Rect;
	COLORINFO Color;
	LPFRAME lpOutFrame = NULL;
	
	*lpbCompatible = TRUE;
	idFileType = GetPPFileType(lpFileName);
	if (idFileType != IDN_PP)
	{
		*lpbCompatible = FALSE;
		return(NULL);
	}
		
	// get frame directly
	CReadImage Read(lpFileName, idFileType);
	Rect.left = Rect.top =  0;
	Rect.right = lpInfo->RequestedWidth-1;
	Rect.bottom = lpInfo->RequestedHeight-1;
	Read.SetSampling(&Rect);
	if (Read.ReadObjectList(&ObjList))
	{
			lpObject = ObjList.lpHead; 
			// get frame to write
			Color.gray = 0xC0;
			SetColorInfo(&Color, &Color, CS_GRAY);
			lpOutFrame = ImgCopyMaskedFrame(NULL,lpObject,NULL,NO,NO,YES,&Color);
			ObjDeleteObjects(&ObjList, ST_ALL);
	}
	
	lpInfo->idFileType = Read.GetFileType();	// file type of original file
	lpInfo->idDataType = Read.GetDataType();	// data type of original file
	lpInfo->fileXSize = Read.GetFileWidth();	// size of original file
	lpInfo->fileYSize = Read.GetFileHeight();	// size of original file
	lpInfo->fileRes = Read.GetFileResolution();	// Resolution of original file
	
	return(lpOutFrame);
}


//************************************************************************
//
//************************************************************************
static BOOL do_paste_from_file(LPSTR PasteImageFile, LPSTR PasteMaskFile, LPSTR Clipboard)
//************************************************************************
{
    CServerDoc* pDoc;
    LPIMAGE lpImage;
	LPOBJECT lpObject;
	CServerView* pView;
	FRMTYPEINFO SrcTypeInfo;

	FrameSetTypeInfo(&SrcTypeInfo, FDT_NONE);
	
	pView = PictPubApp.GetActiveView();
	if (!pView)
	{
		// paste as new image
   		 ProgressBegin(1, PROGRESS_ID(IDS_UNDOPASTEFROMFILE));
		 lpObject = ImgCreateClipInObject(NULL, PasteImageFile, PasteMaskFile);
		 if (lpObject)
		 {
			FRMTYPEINFO TypeInfo;
			ITEMID      DataType;   
			ITEMID      FileType;
			
		 	FileType = GetPPFileType(PasteImageFile);
			FrameGetTypeInfo(ObjGetEditFrame(lpObject),&TypeInfo);
			DataType  = GetSaveDataType(TypeInfo);
		 	lpImage = CreateImage (NULL,NULL,lpObject,NULL, 
		 			FileType,DataType,IMG_DOCUMENT,NULL);
			if (lpImage)
			{
	    	    CServerDoc *pDoc;
	    	    CWnd       *pWnd;
				
	    	    pDoc = (CServerDoc *)PictPubApp.OpenDocumentFile(
					(LPSTR)lpImage->CurFile,lpImage);
	    		if (!pDoc)
	    	    {
			    	DestroyImage(lpImage);
			  		ProgressEnd();
					return (FALSE);
	    	    }
			}
			else 
			{
				ObjFreeUp(lpObject);
			}
		}
  		ProgressEnd();
		return (TRUE);
	}
					
    pDoc = pView->GetDocument();
	if (!pDoc)
		return(FALSE);	//***ERROR
    lpImage = pDoc->m_lpImage;
	if (!lpImage)
		return(FALSE);	//***ERROR

    DeactivateTool();

    ProgressBegin(1, PROGRESS_ID(IDS_UNDOPASTEFROMFILE));
    RemoveObjectMarquee(lpImage);
    lpObject = ImgCreateClipInObject( lpImage,
            PasteImageFile, PasteMaskFile, Control.CMSEnabled, &SrcTypeInfo);
	ProgressEnd();

	if (lpObject)
	{
		init_paste(lpObject, lpImage);
        TransformObjectsStart(lpImage, PasteFromEndProc, IDC_OBJECTTRANSFORM,
			YES, IDS_UNDOPASTE, YES, Clipboard, 0, TRUE, &SrcTypeInfo);
		return(TRUE);
	}
	return(FALSE);
}

LOCAL void init_paste(LPOBJECT lpObject, LPIMAGE lpImage)
{
	RECT rImage, rObjects;
	
	int xData = RectWidth(&lpObject->rObject);
	int yData = RectHeight(&lpObject->rObject);

	// get location rect to center to object on
	ImgGetInfo(lpImage, (LPINT)&rImage.right, (LPINT)&rImage.bottom, NULL, NULL);
	--rImage.right; --rImage.bottom;
	rImage.top = rImage.left = 0;

	// center the object in the location rectangle
	int x = ( rImage.left + rImage.right - xData ) / 2;
	int y = ( rImage.top + rImage.bottom - yData ) / 2;
	if (x < rImage.left)
		x = rImage.left;
	if (y < rImage.top)
		y = rImage.top;
	OffsetRect(&lpObject->rObject, x, y);

	RemoveObjectMarquee(lpImage);
	lpObject->StorageType = ST_TEMPORARY;
	// save the selection state in case of cancel
	SaveSelectedObjects(lpImage);
	ImgAddNewObject(lpImage, lpObject);
	ImgGetSelObjectRect(lpImage, &rObjects, YES);
	UpdateImage(lpImage, &rObjects, YES);
}


//**************************************************************************/
//	Enables/Disables the OK button based on number of files selected
//**************************************************************************/
static void enable_open_button(HWND hDlg)
//**************************************************************************/
{
	HWND hFileCombo;
	int num;
	BOOL bEnable;
	FNAME text;

	hFileCombo = GetDlgItem(hDlg, PASTE_FILELISTID);
	if (!hFileCombo)
		return;
	num = ListBox_GetSelCount(hFileCombo);
	bEnable = (num == 1);
	ControlEnable(hDlg, IDC_OK, bEnable);
}

//**************************************************************************/
//	Enables/Disables the info button based whether any files/albums
//		are selected
//**************************************************************************/
static void enable_info_button(HWND hDlg)
//**************************************************************************/
{
	int num;
	BOOL bEnable;
	HWND hFileCombo;

	hFileCombo = GetDlgItem(hDlg, PASTE_FILELISTID);
	if (!hFileCombo)
		return;
	num = ListBox_GetSelCount(hFileCombo);
	bEnable = (num != 0 && num != LB_ERR);
	ControlEnable(hDlg, IDC_STATUSINFO, bEnable);
}

//**************************************************************************/
// call when list box changed NOT by user to update file names
//**************************************************************************/
static void set_files_text(HWND hDlg)
//**************************************************************************/
{
	// fake a change to the listbox
	FORWARD_WM_COMMAND(hDlg, PASTE_FILELISTID, GetDlgItem(hDlg,PASTE_FILELISTID),
					LBN_SELCHANGE, PostMessage);
}




