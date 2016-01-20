/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                mdiframe.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "dataitem.h"
#include "svrview.h"
#include "plugin.h"
#include "ctxhelp.h"
#include "twain.h"
#include "dca_acq.h"
#include "dca_glue.h"
#include "macro.h"
#include "id.h"
#include "custools.h"
#include "objfloat.h"
#include "ipframe.h"
#include "pptbars.h"
#include "tbardlgs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */
/* ********************************** Macros ******************************** */
/* ********************************** Externals ***************************** */
extern HINSTANCE	hInstStringLib;
extern HINSTANCE	hInstHintLib;   
extern HACCEL		hAccelTable;
extern BOOL			fAppActive;
extern RECT			rFrame;
extern void SaveImportSetup( void );

/* ********************************** Types ********************************* */
/* ******************************* Local Data ******************************* */
static HBRUSH hOldBGBrush;
static POINT ClosePoint;

RECT    rClient;

WPARAM MDIFrame_wParam;
LPARAM MDIFrame_lParam;

IMPLEMENT_DYNAMIC(CMDIFrame,CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMDIFrame,CMDIFrameWnd)
	ON_WM_SYSCOLORCHANGE()
	ON_WM_ACTIVATEAPP()
	ON_WM_CHANGECBCHAIN()
	ON_WM_CREATE()
	ON_WM_CLOSE()   
	ON_WM_DESTROY()
	ON_WM_DRAWCLIPBOARD()
	ON_WM_INITMENUPOPUP()
	ON_WM_ENDSESSION()
    ON_WM_QUERYENDSESSION()
	ON_COMMAND(IDM_TOOLBARS, OnToolbars)
	ON_UPDATE_COMMAND_UI(IDM_TOOLBARS, OnUpdateToolbars)
	ON_COMMAND(IDM_OCCustom, OnOCCustomize)
	ON_UPDATE_COMMAND_UI(IDM_OCCustom, OnUpdateOCCustomize)
END_MESSAGE_MAP()

// Prototypes 
LOCAL LONG Astral_DefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LOCAL void Astral_OnDropFiles(HWND hWindow, HDROP hDrop);
LOCAL void Astral_OnNCLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT codeHitTest) ;
LOCAL void Astral_OnSysCommand(HWND hWindow, UINT cmd, int x, int y);
LOCAL BOOL Astral_OnQueryNewPalette(HWND hWindow);
LOCAL void Astral_OnPaletteChanged(HWND hWindow, HWND hwndPaletteChange);
LOCAL BOOL Astral_OnSetCursor(HWND hWindow, HWND hwndCursor, UINT codeHitTest, UINT msg);
LOCAL void Astral_OnSize(HWND hWindow, UINT state, int cx, int cy);
LOCAL void Astral_OnCommand(HWND hWindow, UINT id, HWND hwndCtl, UINT codeNotify);
LOCAL void Astral_OnPaint(HWND hWindow);
LOCAL void Astral_OnXFERDone(HWND hWindow, LPVOID lpVoid, int DataType);
LOCAL BOOL Astral_OnProcessingDone(HWND hWindow, LPCMDLIST lpCmdList);
LOCAL void Astral_OnAddRecall(HWND hWindow, LPSTR lpFileName);
LOCAL int Astral_OnMouseActivate( HWND hWindow, HWND hwndTopLevel, UINT codeHitTest, UINT msg );
LOCAL int Astral_OnMessageBox(HWND hWindow, LPCTSTR lpszText, UINT nType);
LOCAL HWND Astral_OnGetMessageParent(HWND hWindow);
LOCAL BOOL Astral_OnPromptForFileName(HWND hWindow, LPTSTR lpFileName, LPTSTR lpVolumeID);
LOCAL void Astral_OnStatusMsg(HWND hWindow, LPCTSTR lpszText);
 
/***********************************************************************/
// CMDIFrame Methods
/***********************************************************************/

void CMDIFrame::OnToolbars() 
{
TBarDlgList		ToolBar;

ToolBar.DoModal();
}

void CMDIFrame::OnUpdateToolbars(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

OCCustom		*CustomDlg;
void CMDIFrame::OnOCCustomize() 
{

CustomDlg = new OCCustom;
CustomDlg->Create();
	
}

void CMDIFrame::OnUpdateOCCustomize(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CMDIFrame::RecalcLayout(BOOL bNotify)
{
CMDIFrameWnd::RecalcLayout(bNotify);
// override to prevent resizing based on control bars
}

CMDIFrame::CMDIFrame ()
{
    m_pListCustTools = new CPPToolList();	// load toolbars
    OCToolList = m_pListCustTools;

	// Clipboard Viewer stuff for tracking Paste operations
	m_hwndNextViewer = NULL;
	m_ClipboardChangeID = 0;
	m_fFrameClosing = FALSE;

	m_prevDropEffect = DROPEFFECT_NONE;
}
  
CMDIFrame::~CMDIFrame ()
{
    if (m_pListCustTools)
       delete m_pListCustTools;
	if (m_pDropTarget)
	{
		m_pDropTarget->Revoke();
		delete m_pDropTarget;
	}
}

int CMDIFrame::GetNumMDIChildWindows()
{
	int iCount = 0;

	HWND hChild = ::GetWindow(m_hWndMDIClient, GW_CHILD);
	while (hChild)
	{
		++iCount;
		hChild = ::GetWindow(hChild, GW_HWNDNEXT);
	}
	return(iCount);
}

//************************************************************************
//	Note: never called if app is closing due to windows closing
//************************************************************************
BOOL CMDIFrame::do_close()
{
    // turn off bubble hints timer
    HelpContext.SetHintWindows();   

    // Clean up the TWAIN Source Manager
    DCTerminate();

    // destroy pressure tablet context, if active
    TabletEnd();             

    /* all ok to close down */
    return(TRUE);
}   

void CMDIFrame::do_freeup()
{
    // turn off bubble hints timer
    HelpContext.SetHintWindows(); 
  
    // Clean up the TWAIN Source Manager
    DCTerminate();

    // destroy pressure tablet context, if active
    TabletEnd();             

    // save preferences at the last possible moment
    SaveMiscPreferences();

    SaveImportSetup();      // filter profile settings

    // unhook help
    EndContextHelp();

    /* Free up any image frames */
    if ( Scans.lpScanFrame )
        FrameClose( Scans.lpScanFrame );

    // Remove the Adobe plugins
    FreeUpPlugin();
    FreeUpAdobePlugin();

    // turn off help
    Help( HELP_QUIT, 0L );

}

LONG CMDIFrame::WindowProc (
UINT message, 
UINT wParam, 
LONG lParam)
{
    HWND hWindow = GetSafeHwnd ();

	if ((message == WM_COMMAND) && OnCommand(wParam,lParam))
		return(0L);

	MDIFrame_wParam = wParam;
	MDIFrame_lParam = lParam;
    switch ( message )
    {
        HANDLE_MSG(hWindow, WM_COMMAND, Astral_OnCommand);
        HANDLE_MSG(hWindow, WM_DROPFILES, Astral_OnDropFiles);
        HANDLE_MSG(hWindow, WM_NCLBUTTONDBLCLK, Astral_OnNCLButtonDown);
        HANDLE_MSG(hWindow, WM_SYSCOMMAND, Astral_OnSysCommand);
        HANDLE_MSG(hWindow, WM_QUERYNEWPALETTE, Astral_OnQueryNewPalette);
        HANDLE_MSG(hWindow, WM_PALETTECHANGED, Astral_OnPaletteChanged);
        HANDLE_MSG(hWindow, WM_SETCURSOR, Astral_OnSetCursor);
        HANDLE_MSG(hWindow, WM_MENUSELECT, Astral_OnMenuSelect);
//        HANDLE_MSG(hWindow, WM_SIZE, Astral_OnSize);
        HANDLE_MSG(hWindow, WM_PAINT, Astral_OnPaint);
        HANDLE_MSG(hWindow, PM_XFERDONE, Astral_OnXFERDone);
        HANDLE_MSG(hWindow, WM_PROCESSINGDONE, Astral_OnProcessingDone);
        HANDLE_MSG(hWindow, WM_MESSAGEBOX, Astral_OnMessageBox);
        HANDLE_MSG(hWindow, WM_GETMESSAGEPARENT, Astral_OnGetMessageParent);
        HANDLE_MSG(hWindow, WM_PROMPTFORFILENAME, Astral_OnPromptForFileName);
        HANDLE_MSG(hWindow, WM_ADDRECALL, Astral_OnAddRecall);
	    HANDLE_MSG(hWindow, WM_MOUSEACTIVATE, Astral_OnMouseActivate );
        HANDLE_MSG(hWindow, WM_STATUSMSG, Astral_OnStatusMsg);

    default:
        return(CMDIFrameWnd::WindowProc (message,wParam,lParam));
    }
}

void CMDIFrame::OnSysColorChange()
{
	CMDIFrameWnd::OnSysColorChange();
	PictPubApp.OnSysColorChange();
}

// when Windows session ends, close all documents
void CMDIFrame::OnEndSession(BOOL bEnding)
{
	if (bEnding && AfxGetApp()->m_pMainWnd == this)
	{
		AfxOleSetUserCtrl(TRUE);    // keeps from randomly shutting down
		AfxGetApp()->CloseAllDocuments(FALSE); // we should pass in true.. but it crashes some systems on close
		do_freeup();	// save prefs
	}
}

BOOL CMDIFrame::OnQueryEndSession()
{
    //----------------------------------------------------------------
    // Delete promised clipboard formats -- Windows is going down    
    //----------------------------------------------------------------
    COleDataSource *pDataSource = COleDataSource::GetClipboardOwner();
    if (pDataSource)
    {
        pDataSource->Empty();
        pDataSource->FlushClipboard();
    }
    return CMDIFrameWnd::OnQueryEndSession();
}

BOOL CMDIFrame::GetObjectInfo(COleDataObject* pDataObject,
    CSize* pSize, CSize* pOffset)
{
    ASSERT(pSize != NULL);

    // get object descriptor data
    HGLOBAL hObjDesc = pDataObject->GetGlobalData(_oleData.cfObjectDescriptor);
    if (hObjDesc == NULL)
    {
        if (pOffset != NULL)
            *pOffset = CSize(0, 0); // fill in defaults instead
        *pSize = CSize(0, 0);
        return FALSE;
    }
    ASSERT(hObjDesc != NULL);

    // otherwise, got CF_OBJECTDESCRIPTOR ok.  Lock it down and extract size.
    LPOBJECTDESCRIPTOR pObjDesc = (LPOBJECTDESCRIPTOR)GlobalLock(hObjDesc);
    ASSERT(pObjDesc != NULL);
    pSize->cx = (int)pObjDesc->sizel.cx;
    pSize->cy = (int)pObjDesc->sizel.cy;
    if (pOffset != NULL)
    {
        pOffset->cx = (int)pObjDesc->pointl.x;
        pOffset->cy = (int)pObjDesc->pointl.y;
    }
    GlobalUnlock(hObjDesc);
    GlobalFree(hObjDesc);

    // successfully retrieved pSize & pOffset info
    return TRUE;
}

DROPEFFECT CMDIFrame::OnDragEnter(
COleDataObject* pDataObject,
DWORD dwKeyState, 
CPoint point)
{
	ASSERT(m_prevDropEffect == DROPEFFECT_NONE);
	GetObjectInfo(pDataObject, &m_dragSize, &m_dragOffset);
	CClientDC dc(NULL);
	dc.HIMETRICtoDP(&m_dragSize);
	dc.HIMETRICtoDP(&m_dragOffset);
	return OnDragOver(pDataObject, dwKeyState, point);
}

void CheckifBrowserDlg(CPoint pt, DROPEFFECT &de)
   {
   RECT     r;
   HWND     BrowserDlg = AstralDlgGet(IDD_PASTEFROMFILE_THUMB);
   if (!BrowserDlg || !IsWindowVisible(BrowserDlg))
      BrowserDlg = AstralDlgGet(IDD_PASTEFROMFILE);
   if (BrowserDlg && IsWindowVisible(BrowserDlg))
      {
      GetWindowRect(BrowserDlg, &r);
      if (PtInRect(&r, pt))
         de = DROPEFFECT_NONE;
      }
   }

DROPEFFECT CMDIFrame::OnDragOver(
COleDataObject* pDataObject,
DWORD dwKeyState, 
CPoint point)
{
    DROPEFFECT de = DROPEFFECT_NONE;
	CWnd *pWnd = CWnd::FromHandle(m_hWndMDIClient);
    CPoint   pt;

	if (pDataObject->IsDataAvailable(_oleData.cfPPFileName) ||
		pDataObject->IsDataAvailable(Control.cfImage) ||
	    pDataObject->IsDataAvailable(CF_DIB) ||
	    pDataObject->IsDataAvailable(CF_BITMAP))
    {
    	ClientToScreen(&point);
        pt = point;
    	pWnd->ScreenToClient(&point);
    
    	point -= m_dragOffset;  // adjust target rect by original cursor offset
    
        if ((dwKeyState & MK_ALT) == MK_ALT)
            de = DROPEFFECT_MOVE;
        else
            de = DROPEFFECT_COPY;
    	if (point == m_dragPoint)
        	return de;
    }    

    CheckifBrowserDlg(pt, de);
	// otherwise, cursor has moved -- need to update the drag feedback
	CClientDC dc(pWnd);
	if (m_prevDropEffect != DROPEFFECT_NONE)
	{
	    // erase previous focus rect
	    dc.DrawFocusRect(CRect(m_dragPoint, m_dragSize));
	}

	m_prevDropEffect = de;

	if (m_prevDropEffect != DROPEFFECT_NONE)
	{
	    m_dragPoint = point;
	    dc.DrawFocusRect(CRect(point, m_dragSize));
	}

	return de;
}

BOOL CMDIFrame::OnDrop(
COleDataObject* pDataObject,
DROPEFFECT dropEffect, 
CPoint point)
{
	int cfAvailable = -1;
	STGMEDIUM StgMedium;
    FORMATETC FormatEtc;

    // clean up focus rect
	OnDragLeave();

	if (pDataObject->IsDataAvailable(_oleData.cfPPFileName))
    {
 		cfAvailable = _oleData.cfPPFileName;
        SETDefFormatEtc(FormatEtc,cfAvailable,TYMED_HGLOBAL);
   	}
	else
	if (pDataObject->IsDataAvailable(Control.cfImage5Ptr))
	{
 		cfAvailable = Control.cfImage5Ptr;
        SETDefFormatEtc(FormatEtc,cfAvailable,TYMED_HGLOBAL);
	}
	else if (pDataObject->IsDataAvailable(Control.cfImage))
	{
		cfAvailable = Control.cfImage;
	    SETDefFormatEtc(FormatEtc,Control.cfImage,TYMED_HGLOBAL);
	}
	else
	if (pDataObject->IsDataAvailable(CF_DIB))
	{
		cfAvailable = CF_DIB;
	    SETDefFormatEtc(FormatEtc,CF_DIB,TYMED_HGLOBAL);
	}
	else
	if (pDataObject->IsDataAvailable(CF_BITMAP))  
	{
		cfAvailable = CF_BITMAP;
	    SETDefFormatEtc(FormatEtc,CF_BITMAP,TYMED_GDI);
	}

	if (cfAvailable == -1)
		return FALSE;

   	DeactivateTool();
    ProgressBegin(1, PROGRESS_ID(IDS_UNDOPASTECLIP));

	if (pDataObject->GetData(cfAvailable,&StgMedium,&FormatEtc))
	{
		LPOBJECT lpObject = NULL;
        LPIMAGE lpImage = NULL;

    	// Other formats need to create the file, therefore we stuff the path
    	// strings first
        if (cfAvailable == _oleData.cfPPFileName)
        {
        	PPFILENAME *ppFileName = (PPFILENAME *)GlobalLock(StgMedium.hGlobal);
			FNAME 		szDataFile;
			FNAME 		szMaskFile;
			BOOL 		bExtName = FALSE;

			szMaskFile[0] ='\0';
			if (bExtName = ppFileName->bExtName)
			{
				if (LookupExtFileN(ppFileName->szFileName, szDataFile, IDN_CLIPBOARD, NO))
				{
					if (GetPPFileType(szDataFile) != IDN_PP)
						LookupExtFileN(ppFileName->szFileName, szMaskFile, 
							IDN_MASK, NO);
				}
				else
				{
					ProgressEnd();
			        GlobalUnlock(StgMedium.hGlobal);
					ReleaseStgMedium (&StgMedium);
					return (FALSE);
				}
			}
			else
			{
				lstrcpy(szDataFile, ppFileName->szFileName);
			}

		    lpObject = ImgCreateClipInObject(NULL, szDataFile, szMaskFile);
	        GlobalUnlock(StgMedium.hGlobal);
        }
		else
        if (cfAvailable == Control.cfImage5Ptr)
        {
        	LPDWORD lpImagePtr = (LPDWORD)GlobalLock(StgMedium.hGlobal);
			LPIMAGE lpSrcImage = (LPIMAGE)*lpImagePtr;
	        GlobalUnlock(StgMedium.hGlobal);
			lpObject = GetObjectFromInternalData(NULL,
											 	lpSrcImage,
											 	FALSE,
											 	NULL,
												NULL);
        }
		else
        {
			FormFileName (Names.PasteImageFile, IDN_CLIPBOARD,"WINCLIP.BMP");
	    	Names.PasteMaskFile[0] = '\0';
		    if (!PasteDropData(cfAvailable,StgMedium.hGlobal,Names.PasteImageFile,
		    	Names.PasteMaskFile))
			{		
            	ProgressEnd();
				ReleaseStgMedium (&StgMedium);
	 		    return(FALSE);
    	    }
			lpObject = GetObjectFromExternalData(NULL,
											 	 Names.PasteImageFile,
										 	 	 Names.PasteMaskFile,
										 	 	 FALSE,
										 	 	 NULL,
									  		 	 NULL);
        }
		ReleaseStgMedium (&StgMedium);

		if (lpObject)
		{
	        CServerDoc *pDoc;
	        CWnd       *pWnd;
			FRMTYPEINFO TypeInfo;
			ITEMID      DataType;   
			ITEMID      FileType;
			
		 	FileType = GetPPFileType(Names.PasteImageFile);
			FrameGetTypeInfo(ObjGetEditFrame(lpObject),&TypeInfo);
			DataType  = GetSaveDataType(TypeInfo);
		 	if (lpImage = CreateImage (NULL,NULL,lpObject,NULL, 
		 			FileType,DataType,IMG_DOCUMENT,NULL))
			{
		        pDoc = (CServerDoc *)PictPubApp.OpenDocumentFile(
					(LPSTR)lpImage->CurFile,lpImage);
		    	if (!pDoc)
		        {
			    	DestroyImage(lpImage);
			  		ProgressEnd();
					return (FALSE);
		        }
	
				// set the new window to dirty state.
				lpImage->fChanged = TRUE;
				PictPubApp.DocumentChanged(lpImage);				

				// position the window based on the cursor point
				CRect rClientRect;
				CRect rFrameRect;
	    	    pWnd = pDoc->GetActiveView()->GetParentFrame();
				pWnd->GetWindowRect((LPRECT)&rFrameRect);
				point.x -= (rFrameRect.Width()/2);
				point.y -= (rFrameRect.Height()/2) - GetSystemMetrics(SM_CYCAPTION);
				::GetClientRect(PictPubApp.Get_hClientAstral(),(LPRECT)&rClientRect);
				::ScreenToClient(PictPubApp.Get_hClientAstral(),(LPPOINT)&point);
		        pWnd->SetWindowPos(NULL, 
					max(point.x,rClientRect.left), 
					max(point.y,rClientRect.top), 
					0, 0, 
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);

		  		ProgressEnd();
				return (TRUE);
			}
		}
	}
   	
   	ProgressEnd();
    return (FALSE);
}

void CMDIFrame::OnDragLeave()
{
	CClientDC dc(CWnd::FromHandle(m_hWndMDIClient));
	if (m_prevDropEffect != DROPEFFECT_NONE)
	{
	    dc.DrawFocusRect(CRect(m_dragPoint,m_dragSize)); // erase previous focus rect
	    m_prevDropEffect = DROPEFFECT_NONE;
	}
}

afx_msg int CMDIFrame::OnCreate( 
LPCREATESTRUCT lpCreateStruct )
{
	// register ourselves as a Ole Drop Site
	m_pDropTarget = new CPPDropTarget();	
	if (m_pDropTarget)
		m_pDropTarget->Register(this);

	// notify that we will accept drop files from the likes of the filemanager
    DragAcceptFiles( TRUE );
    Window.hNullCursor = LoadCursor( PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(ID_NULL));

	// add ourselves to the clipboard viewer chain
    m_hwndNextViewer = SetClipboardViewer();

    int i = CFrameWnd::OnCreate (lpCreateStruct);
return(i);
}

afx_msg void CMDIFrame::OnDestroy()
{
     do_freeup();
    m_hWndMDIClient = NULL;

	// remove ourselves from the clipboard viewer chain
	if (m_hwndNextViewer != NULL)
	{
		ChangeClipboardChain(m_hwndNextViewer);
		m_hwndNextViewer = NULL;
	}

    CMDIFrameWnd::OnDestroy();
}

afx_msg void CMDIFrame::OnActivateApp( 
BOOL  bActive, 
HTASK  hTask )
{
	// If deactivating...
	if ( !bActive && fAppActive )
    { 
	    // turn off hints, if enabled 
	    if( Hints.fBubbleHintsOn )
	    {
	        HelpContext.SetHintState( "" );                     // flush current hint
	        HelpContext.SetHintWindows( NULL, NULL, HT_OFF );   // turn off timer
	        HelpContext.BubbleHintsOn( NO );                    // reset state
	    }
	    if( Hints.fStatusHintsOn )                                    // flush hint from status bar
	        HintLine( 0 );
	
	    // Fix the MDI client background
	    if ( Control.ScreenColor && hOldBGBrush )
        {
	        SetClassBackground(PictPubApp.Get_hClientAstral(), hOldBGBrush);
	        WackWindows( PictPubApp.Get_hInstAstral() );
        }

	    if ( InContextHelpMode() )
	        return;

	    if ( IsJournalPlaying() || IsJournalRecording() )
        {
    	    RemoveRecorder();
	        RemovePlayer();
	        MessageBeep(0);
        }
	    RestoreSystemPalette( GetSafeHwnd() );
	    fAppActive = FALSE;
    }

	CMDIFrameWnd::OnActivateApp(bActive,hTask);

	// If activating...
	if ( bActive && !fAppActive )
    { 

	    // Setup the MDI client background color
	    if ( Control.ScreenColor && Window.hBGBrush )
        {
    	    hOldBGBrush = SetClassBackground( PictPubApp.Get_hClientAstral(), Window.hBGBrush);
        	WackWindows( PictPubApp.Get_hInstAstral() );
        }

	    SaveSystemPalette(GetSafeHwnd());
    	fAppActive = TRUE;

	    // turn on hints, if enabled
	    if( Hints.fBubbleHintsOn )
	    {
	        HelpContext.BubbleHintsOn( YES );                           // reset state
	        HelpContext.SetHintWindows( NULL, NULL, HT_STATIC );        // turn on timer
	    }

		// Set the ToolBOX and Ribbons to the correct window 
		// (ie. Insitu or Normal)
		CWnd *pMainWnd = ::AfxGetMainWnd();
		if (pMainWnd)
		{
			// 08/15/94 Jake and Joe fix for global ribbon state
			// if (Tool.hRibbon)
			// 	CServerDoc::SetToolbar(PictPubApp.GetActiveView());
			CServerDoc::SetToolbar(NULL);

			CServerDoc::SetToolboxes(NULL,TRUE);
			// force MDIFrame to recompute the actual size of the ribbon
		    pMainWnd->SendMessage( WM_SIZE, 0, 0L );
		}
    }
}

afx_msg void CMDIFrame::OnChangeCbChain( HWND hWndRemove, HWND hWndAfter)
{
	if (hWndRemove == m_hwndNextViewer)
		m_hwndNextViewer = hWndAfter;
	else if (m_hwndNextViewer != NULL)
		FORWARD_WM_CHANGECBCHAIN(m_hwndNextViewer,hWndRemove,m_hwndNextViewer,::SendMessage);
}

afx_msg void CMDIFrame::OnDrawClipboard( )
{
	if (m_hwndNextViewer != NULL)
		FORWARD_WM_DRAWCLIPBOARD(m_hwndNextViewer, ::SendMessage);
	m_ClipboardChangeID++;
}

//************************************************************************
//	Note: never called if app is closing due to windows closing
//************************************************************************
afx_msg void CMDIFrame::OnClose()
{
	if (m_pListCustTools)
	{
       		delete m_pListCustTools;
	   	m_pListCustTools = NULL;
	}
	// if we are going to close down set the flag so that the 
	// childern documents will know why they are being asked to close
	if (AfxGetApp()->m_pMainWnd == this)
		m_fFrameClosing = TRUE;

    // what sucks is that we do our close stuff first, but
    // some of it we only want to do if the close is really
    // going to happen - the foundation class checks to make
    // sure we want to close all our modified documents and
    // the user can cancel the application close.  If that
    // happens, we have already closed our palette dialog
    // and saved some of our preferences
    CMDIFrameWnd::OnClose();

    if ( !do_close() )
        return;
	
	m_fFrameClosing = FALSE;
}                                             

afx_msg void CMDIFrame::OnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex,
										 BOOL bSysMenu )
{
	LPIMAGE lpImage;
	FRMDATATYPE Type;
	int state, i;
	FNAME szString;
	UINT id, nCount;
	BOOL fWeSetupMenu = FALSE;

	// we see if the menu being popped up is a menu we care about
	nCount = pPopupMenu->GetMenuItemCount();
	if (!bSysMenu && nCount)
	{
		// get the first item in the popup menu
		id = pPopupMenu->GetMenuItemID(0);
		if (id == IDM_NEW)
		{
   			pPopupMenu->EnableMenuItem( IDM_RECALLPOPUP, MF_BYPOSITION|
							(Control.RecallCount?MF_ENABLED:MF_GRAYED));
			
		}
		else
		if (id == IDM_RECALLIMAGE0)
		{
			FNAME szTitle;
			// Setup the recall list by zapping it and refilling it
			for (i = 1; i < MAX_RECALL; ++i)
    			pPopupMenu->DeleteMenu(IDM_RECALLIMAGE0+i, MF_BYCOMMAND);
			for ( i=0; i<Control.RecallCount; i++ )
   			{
    			id = IDM_RECALLIMAGE0 + i;
			lstrcpy(szTitle, Control.RecallImage[i]);
			if (Control.WinVer == WV_WINDOWS95)
    			    stripext(szTitle);
    			wsprintf( szString, "&%d. %s", i+1, szTitle);
    			if ( (int)pPopupMenu->EnableMenuItem( id, MF_ENABLED ) >= 0 )
        			pPopupMenu->ModifyMenu( id, MF_BYCOMMAND | MF_STRING,id,szString);
    			else
					pPopupMenu->AppendMenu( MF_BYCOMMAND | MF_STRING, id, szString );
   			}
			fWeSetupMenu = TRUE;
		}
		else
		if( id >= IDM_RUN0 && id <= IDM_RUN9 )
		{
   			state = MF_ENABLED | MF_BYPOSITION;
			for( id = 0; id < nCount; ++id )
				pPopupMenu->EnableMenuItem( id, state );
			fWeSetupMenu = TRUE;
		}
		else
		if( id >= PS_FIRST_ADOBEPLUGIN && id <= PS_LAST_ADOBEPLUGIN )
		{
			lpImage = GetActiveImage();
			if (lpImage)
				Type = FrameType( ImgGetBaseEditFrame( lpImage ));

			if ( ImageIsBusy(lpImage) || Type == FDT_PALETTECOLOR )
    			state = MF_GRAYED;
			else
    			state = MF_ENABLED;
			state |= MF_BYPOSITION;
			for (id = 0; id < nCount; ++id)
				pPopupMenu->EnableMenuItem( id, state );
			fWeSetupMenu = TRUE;
		}
	}

	// if we didn't set up the menu, let the framework do it
	if (!fWeSetupMenu)
	{
		UINT nIndex, nID;

		CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

		// do some post-processing of any sub-menus of the pPopupMenu
		// this is done to make accelerator keys work because in the
		// normal course of events sub-menus do not get updated when
		// an accelerator is being processing.  Only top-level
		// menus get updated (i.e.- File, Edit, etc.) This can cause the
		// command to not function properly.  For example, the menu
		// item may be disabled when it shouldn't be which would
		// cause the accelearator not to work. CMDIFrameWnd::OnInitMenuPopup
		// does not handle this correctly
		for (nIndex = 0; nIndex < nCount; ++nIndex)
		{
			nID = pPopupMenu->GetMenuItemID(nIndex);
			if (nID == (UINT)-1)
			{
				// possibly a popup menu
				CMenu *pSubMenu = pPopupMenu->GetSubMenu(nIndex);
				if (pSubMenu)
				{
					// initialize the popup menu
					OnInitMenuPopup(pSubMenu, nIndex, FALSE);
				}
			}
		}
	}
}

BOOL CMDIFrame::PreTranslateMessage( 
/*  
REMARKS:

    Override this function to filter window messages before they are dispatched to the Windows functions 
    TranslateMessage and DispatchMessage The default implementation performs accelerator-key 
    translation, so you must call the CWinApp::PreTranslateMessage member function in your overridden 
    version.

RETURN:

    Nonzero if the message was fully processed in PreTranslateMessage and should not be processed 
    further. Zero if the message should be processed in the normal way. 
*/
MSG* pMsg )
{
	if (CMDIFrameWnd::PreTranslateMessage(pMsg))
		return(TRUE);

    /* Check the TWAIN Source Manager */
    if ( ProcessTWMessage( (LPMSG)pMsg, AfxGetMainWnd()->GetSafeHwnd() ) )
        return(TRUE);
    
    /* If an accelerator was executed, go get another command */
    if (TranslateMDISysAccel (m_hWndMDIClient, (LPMSG)pMsg))
        return(TRUE);
    
    if ( TranslateAccelerator( AfxGetMainWnd()->GetSafeHwnd(), hAccelTable, (LPMSG)pMsg ) )
        return(TRUE);
    
    /* If some child window took the message, go get another one */
    if ( AstralDlgMsg( (LPMSG)pMsg ) )
        return(TRUE);
    
    return(FALSE); // FALSE if the message should be processed in the normal way
}


WNDPROC* CMDIFrame::GetSuperWndProcAddr()
{
    static WNDPROC NEAR pfnSuper;
    return &pfnSuper;
}


/***********************************************************************/
LOCAL LONG Astral_DefProc( HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
return(DefFrameProc(hWindow, PictPubApp.Get_hClientAstral(), msg, wParam, lParam));
}

/***********************************************************************/
LOCAL void Astral_OnAddRecall( HWND hWindow, LPSTR lpFileName )
/***********************************************************************/
{
NewAddRecallImageName(lpFileName);
}

/***********************************************************************/
LOCAL void Astral_OnDropFiles( HWND hWindow, HDROP hDrop )
/***********************************************************************/
{
int i;
FNAME szFileName;

//  DragQueryPoint( hDrop, &pt );
i = DragQueryFile( hDrop, 0xFFFF, NULL, 0 );
while ( --i >= 0 )
    {
    DragQueryFile( hDrop, i, szFileName, sizeof(szFileName));
    AstralImageLoadBG( 0, szFileName, YES );
    }
DragFinish( hDrop );
}

/***********************************************************************/
LOCAL void Astral_OnNCLButtonDown( HWND hWindow, BOOL fDoubleClick, int x, int y,
                                    UINT codeHitTest ) 
/***********************************************************************/
{
RECT Cookie;

if ( codeHitTest != HTMENU ||
        x != ClosePoint.x || y != ClosePoint.y )
    {
    FORWARD_WM_NCLBUTTONDOWN(hWindow, fDoubleClick, x, y, codeHitTest,
                            Astral_DefProc);
    return;
    }

GetWindowRect( hWindow, (LPRECT)&Cookie);

Cookie.top  += GetSystemMetrics(SM_CYCAPTION);
Cookie.left += GetSystemMetrics(SM_CXBORDER);

Cookie.right  = Cookie.left+GetSystemMetrics(SM_CYMENU);
Cookie.bottom = Cookie.top+GetSystemMetrics(SM_CYMENU);

/* Make sure we are in the system cookie area */
if ((ClosePoint.x >= Cookie.left &&
    ClosePoint.x <= Cookie.right) &&
    (ClosePoint.y >= Cookie.top  &&
    ClosePoint.y <= Cookie.bottom)) {
    /* Time to close the child window */
    SendMessage( hWindow, WM_COMMAND, IDM_CLOSE, 0L );
    }
FORWARD_WM_NCLBUTTONDOWN(hWindow, fDoubleClick, x, y, codeHitTest,
                        Astral_DefProc);
}

/***********************************************************************/
LOCAL void Astral_OnSysCommand( HWND hWindow, UINT cmd, int x, int y )
/***********************************************************************/
{
if ((cmd & 0x000F) == HTMENU)
    {
    ClosePoint.x = x;
    ClosePoint.y = y;
    }
FORWARD_WM_SYSCOMMAND(hWindow, cmd, x, y, Astral_DefProc);
}

/***********************************************************************/
LOCAL BOOL Astral_OnQueryNewPalette( HWND hWindow )
/***********************************************************************/
{
return(HandleQueryNewPalette(hWindow, NO, NO));
}

/***********************************************************************/
LOCAL void Astral_OnPaletteChanged( HWND hWindow, HWND hwndPaletteChange )
/***********************************************************************/
{
	if (IsOurWindow(hwndPaletteChange))
	{
		CServerView *pView = PictPubApp.GetActiveView();
		if (pView)
		{
			CWnd *pParent = pView->GetParentFrame();
			if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CInPlaceFrame)))
			{
		    	FORWARD_WM_PALETTECHANGED(pParent->GetSafeHwnd(), hwndPaletteChange, SendMessage);
				return;
			}
		}
    	FORWARD_WM_PALETTECHANGED(hWindow, hwndPaletteChange, SendMessageToChildren);
	}
}

/***********************************************************************/
LOCAL BOOL Astral_OnSetCursor( HWND hWindow, HWND hwndCursor,
                                UINT codeHitTest, UINT msg )
/***********************************************************************/
{
    if( SetupCursor( hwndCursor, codeHitTest, 0 ))
        return( TRUE );
    return FORWARD_WM_SETCURSOR(hWindow, hwndCursor, codeHitTest, msg,
                                Astral_DefProc);
}

/***********************************************************************/
void Astral_OnMenuSelect( HWND hWindow, HMENU hMenu, int item,
                                HMENU hMenuPopup, UINT flags )
/***********************************************************************/
{
    if(( int )flags == -1 )
        return;

    if( hMenuPopup )
    {
        while( GetSubMenu( hMenuPopup, 0 ))
            hMenuPopup = GetSubMenu( hMenuPopup, 0 );

        int id = GetMenuItemID( hMenuPopup, 0 );
        if( id > 0 )
        {
            id -= 1;
            HintLine( id );
        }
    }
    else
    if( hMenu )
        HintLine( item );
}

/***********************************************************************/
LOCAL void Astral_OnSize(HWND hWindow, UINT state, int cx, int cy)
/***********************************************************************/
{
HWND hWnd;
RECT rect;
int dyRibbon, dyStatus;

if ( state == SIZEICONIC )
    {
    FORWARD_WM_SIZE(hWindow, state, cx, cy, Astral_DefProc);
    return;
    }

// never reset our window sizes if iconic
if (IsIconic(hWindow))
    {
    if (IsRectEmpty(&rClient)) // if never set rectangle
        {
        GetClientRect( PictPubApp.Get_hClientAstral(), &rClient );
        if (IsRectEmpty(&rClient)) // only in Windows 3.0
            {
            rClient.left = rClient.top = 0;
            rClient.right = rFrame.right - rFrame.left - 1;
            rClient.bottom = rFrame.bottom - rFrame.top - 1;
            rClient.right -= 2 * GetSystemMetrics(SM_CXFRAME);
            rClient.bottom -= 2 * GetSystemMetrics(SM_CYFRAME);
            rClient.bottom -= GetSystemMetrics(SM_CYCAPTION);
            rClient.bottom -= GetSystemMetrics(SM_CYMENU);
            }
        if ( hWnd = AstralDlgGet(IDD_STATUS) )
            {
            GetClientRect(hWnd, &rect);
            rClient.bottom -= RectHeight(&rect);
            }
        if ( hWnd = AstralDlgGet(Tool.idRibbon) )
            {
            GetClientRect(hWnd, &rect);
            rClient.top += RectHeight(&rect);
            }
#ifdef ID_MAGIC
        if ( hWnd = AstralDlgGet(IDD_TOOLS) )
            {
            GetClientRect(hWnd, &rect);
            rClient.left += RectWidth(&rect);
            }
        if ( hWnd = AstralDlgGet(IDD_VIEWING) )
            {
            GetClientRect(hWnd, &rect);
            rClient.right -= RectWidth(&rect);
            }
#endif
        }
    return;
    }

GetClientRect( hWindow, &rClient );
ClientToScreen( hWindow, (LPPOINT)&rClient.left );
ClientToScreen( hWindow, (LPPOINT)&rClient.right );
dyRibbon = dyStatus = 0;
if ( hWnd = AstralDlgGet( Tool.idRibbon ) )
    {
    if (IsWindowVisible(hWnd))
        {
        GetWindowRect( hWnd, &rect );
        rect.right--; rect.bottom--;
        dyRibbon = rect.bottom - rClient.top + 1;
        rClient.top = rect.bottom + 1;
        }
	::InvalidateRect(hWnd,NULL,TRUE);
    }
if ( hWnd = AstralDlgGet( IDD_STATUS ) )
    {
    SendMessage( hWnd, WM_INITDIALOG, 0, 0L );
    if (IsWindowVisible(hWnd))
        {
        GetWindowRect( hWnd, &rect );
        rect.right--; rect.bottom--;
        dyStatus = rClient.bottom - rect.top + 1;
        rClient.bottom = rect.top - 1;
        }
	::InvalidateRect(hWnd,NULL,TRUE);
    }

ScreenToClient( hWindow, (LPPOINT)&rClient.left );
ScreenToClient( hWindow, (LPPOINT)&rClient.right );
MoveWindow( PictPubApp.Get_hClientAstral(), rClient.left, rClient.top,
    RectWidth(&rClient), RectHeight(&rClient), TRUE );
GetWindowRect( PictPubApp.Get_hClientAstral(), &rClient );
ScreenToClient( hWindow, (LPPOINT)&rClient.left );
ScreenToClient( hWindow, (LPPOINT)&rClient.right );
}

/***********************************************************************/
LOCAL void Astral_OnCommand(HWND hWindow, UINT id, HWND hwndCtl,
                            UINT codeNotify)
/***********************************************************************/
{
if (DoCommand( hWindow, id, hwndCtl, codeNotify ))
    return;
FORWARD_WM_COMMAND(hWindow, id, hwndCtl, codeNotify, Astral_DefProc);
}

/***********************************************************************/
LOCAL void Astral_OnPaint(HWND hWindow)
/***********************************************************************/
{   
HDC hDC;
PAINTSTRUCT ps;

hDC = BeginPaint( hWindow, &ps );
EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Astral_OnXFERDone(HWND hWindow, LPVOID lpVoid, int DataType) 
/***********************************************************************/
{
// This is for TWAIN Image Processing
OpenImageWindow( PictPubApp.Get_hInstAstral(), hWindow, lpVoid, DataType );
}

/***********************************************************************/
LOCAL void do_processingUpdate(CServerDoc *pDoc,CView *pView,
								BOOL bUpdateViews,RECT rUpdate)
/***********************************************************************/
{
	ASSERT(pDoc);
	ASSERT(pView);
	CServerView *pActiveView = PictPubApp.GetActiveView();
	RECT rDisp;

	// Update all views and items
	CUpdateArea UpdateArea(AstralToWindowsRect(&rUpdate));
	if (bUpdateViews)
	    pDoc->UpdateAllViews(NULL,0L, (CObject*)&UpdateArea);
	else
	if (pView && !AstralIsRectEmpty(&rUpdate))
	{
		File2DispRect(pView->GetSafeHwnd(), &rUpdate, &rDisp);
	    ::InvalidateRect(pView->GetSafeHwnd(), 	 
			AstralToWindowsRect(&rDisp), FALSE);
	}	
	pDoc->UpdateAllItems(NULL,0L,
		(CObject*)&UpdateArea,DVASPECT_CONTENT);

	// If this is the Active View and the Zoom Window is on it,
	// then update the Zoom as well to reflect the changes
	if ((pActiveView == pView) && 
		(Window.fHasZoom && hZoomWindow))
	{
	    File2DispRect(hZoomWindow, &rUpdate, 
			&rDisp);
	    ::InvalidateRect(hZoomWindow, 
			AstralToWindowsRect(&rDisp), FALSE);
	}
}

/***********************************************************************/
LOCAL int Astral_OnMessageBox(HWND hWindow, LPCTSTR lpszText, UINT nType)
/***********************************************************************/
{
	return (PPMessageBox(lpszText, nType));
}

/***********************************************************************/
LOCAL void Astral_OnStatusMsg(HWND hWindow, LPCTSTR lpszText)
/***********************************************************************/
{
	Status((LPTSTR)lpszText);
}

/***********************************************************************/
LOCAL HWND Astral_OnGetMessageParent(HWND hWindow)
/***********************************************************************/
{
	return (MessageParent());
}

/***********************************************************************/
LOCAL BOOL Astral_OnPromptForFileName(HWND hWindow, LPTSTR lpFileName, LPTSTR lpVolumeID)
/***********************************************************************/
{
	return (PromptForFileName(lpFileName, lpVolumeID));
}

/***********************************************************************/
LOCAL BOOL Astral_OnProcessingDone(HWND hWindow, LPCMDLIST lpCmdList)
/***********************************************************************/
{					 
	RECT rUpdate;
	HWND hWnd;
	UPDATE_TYPE UpdateType;
	BOOL fHandleUpdate;
	LPNEWIMAGEINFO lpInfo;
	CImage *pImage;
	CDocument *pNewDoc;
	int dx, dy;
	
	if (!lpCmdList)
	    return(TRUE /* kill thread */);
	
	// set flag to prevent deadlock when trying to kill thread
	lpCmdList->fInOnProcessingDone = TRUE;

	pImage = lpCmdList->ThreadData.lpImage;
	fHandleUpdate = TRUE;
	UpdateType = lpCmdList->UpdateType;

	// see if we have a command list without an image attached
	// this can occur when you get an error reading a file
	if (!pImage)
	{
		if (lpCmdList->pDocument)
			lpCmdList->pDocument->OnCloseDocument();
		DestroyCommandList(lpCmdList);
		return(TRUE /* kill thread */);
	}
	
	// handle a close, this means close this sucker right now!!!
	if (UpdateType & UT_CLOSE)
	{
		// if a document has been created for this image, close it
		// otherwise close the image
		CServerDoc *pDoc = PictPubApp.GetDocument(pImage);
		if (pDoc)
			pDoc->OnCloseDocument();
		else
			delete pImage;
		return(TRUE /* kill thread */);
	}
	
	BOOL fKillThread = FALSE;

 	if (UpdateType & UT_NEWIMAGE)
	{
	    lpInfo = (LPNEWIMAGEINFO)ListGetHead(&lpCmdList->NewImageList);
	    while (lpInfo)
	    {
	        // is one of the new images the current image
	        if (lpInfo->lpImage == pImage)
	            fHandleUpdate = FALSE;

			// do we have a document to attach to?
			if (lpCmdList->pDocument)
			{
				pNewDoc = lpCmdList->pDocument; // AttachImage wacks this pointer
				lpCmdList->pDocument->AttachImage(lpInfo->lpImage);
			}
			else
				pNewDoc = PictPubApp.OpenDocumentFile(lpInfo->lpImage->CurFile,	lpInfo->lpImage); 
			if (!pNewDoc)
	        {
	            if (lpInfo->lpImage == pImage)
				{
	                 delete pImage;
					 fKillThread = TRUE;
				}
	        }
	        ListUnlink(&lpCmdList->NewImageList, lpInfo);
	        FreeUp(lpInfo);
	        lpInfo = (LPNEWIMAGEINFO)ListGetHead(&lpCmdList->NewImageList);
	    }
	}

	CServerDoc *pDoc = PictPubApp.GetDocument(pImage);
	if (pDoc)
	{
		if (UpdateType & UT_SAVE)
			pDoc->OnSavedDocument();

		CServerView *pActiveView = pDoc->GetActiveView();
		if (fHandleUpdate)
		{
	    	if (UpdateType & UT_DATATYPE)
	    	{
	        	ImgInitDisplay(pImage);
				if (pActiveView)
	        		pActiveView->SetupImageDisplayPalette();
	    	}

	    	rUpdate = lpCmdList->rUpdateArea;
			POSITION ViewPos = pDoc->GetFirstViewPosition();
			while (ViewPos)
			{	
	       		CServerView* pView = (CServerView *)pDoc->GetNextView( ViewPos );
	       		if (pView)
				{
					if (pView->m_lpDisplay &&
						!AstralIsRectEmpty(&pView->m_lpDisplay->PaintRect))
					{
						AstralUnionRect(&rUpdate, &rUpdate,
										&pView->m_lpDisplay->FileRect);
						AstralSetRectEmpty(&pView->m_lpDisplay->PaintRect);
						UpdateType |= UT_AREA;
					}
				}
			}

			ImgGetInfo(pDoc->m_lpImage, &dx, &dy, NULL, NULL);
	    	if ((UpdateType & UT_DATATYPE) || (UpdateType & UT_SIZE))
			{
				if (UpdateType & UT_SIZE)
					UpdateImageSize(pImage);

				SetRect(&rUpdate, 0, 0, dx-1, dy-1);
				do_processingUpdate(pDoc,pActiveView,TRUE,rUpdate);
			}
	    	else if (UpdateType & UT_AREA)
	    	{
				do_processingUpdate(pDoc,pActiveView,TRUE,rUpdate);
	    	}
	    	else if (UpdateType & UT_ACTIVEAREA &&
					pActiveView == PictPubApp.GetActiveView())
	    	{
				do_processingUpdate(pDoc,pActiveView,FALSE,rUpdate);
	    	}
			else if (UpdateType == UT_NONE)
			{
				// Linked/embeded objects need notification anyway
				CUpdateArea UpdateArea(AstralToWindowsRect(&rUpdate));
				pDoc->UpdateAllItems(NULL,0L,
					(CObject*)&UpdateArea,DVASPECT_CONTENT);
			}
		}
	
		if (pActiveView == PictPubApp.GetActiveView())
		{
	    	SetupMiniViews(NULL, NO);
	    	UpdateStatusBar( YES, YES, YES, YES );
	    	if ( Tool.hRibbon )
	        	SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
		}
	
		// Set the Modified Flag and update all the views of the Active document     
		PictPubApp.DocumentChanged (pImage);
	}
	lpCmdList->fInOnProcessingDone = FALSE;
	return(fKillThread);
}
										   
/************************************************************************/
LOCAL int Astral_OnMouseActivate( HWND hWindow, HWND hwndTopLevel, UINT codeHitTest, UINT msg )
/************************************************************************/
{
    if( Hints.fBubbleHintsOn && HelpContext.BubbleHintsOn( HL_BUBBLE_QUERY ))     
    {
        HelpContext.SetHintState( "" );                     // flush current hint
        HelpContext.BubbleHintsOn( NO );                    // reset state
    }

    return FORWARD_WM_MOUSEACTIVATE( hWindow, hwndTopLevel, codeHitTest, msg,
                                     Astral_DefProc);
}
