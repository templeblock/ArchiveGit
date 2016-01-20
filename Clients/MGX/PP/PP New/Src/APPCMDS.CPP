/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                appcmds.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "svrview.h"
#include "ctxhelp.h"
#include "cmdundo.h"
#include "macro.h"
#include "twain.h"
#include "dca_glue.h"
#include "dca_acq.h"
#include "id.h"
#include "objfloat.h"
#include "monitor.h"
#include "prefs.h"
#include "pptbars.h"
#include "calib.h"
#include "dlgscan.h"

#ifdef _DEBUG
#undef THIS_FILE
static TCHAR BASED_CODE THIS_FILE[] = __FILE__;
#endif

/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */
/* ********************************** Macros ******************************** */

/* ********************************** Types ********************************* */

/* ******************************* Local Data ******************************* */

afx_msg void CPictPubApp::OnHelp()
{
}

afx_msg void CPictPubApp::OnSwitchActiveColor()
{
SwapActiveAlternate();
}

afx_msg void CPictPubApp::OnRevertColorSwatch()
{
RevertColorSwatch();
}

afx_msg void CPictPubApp::OnUpdateAlwaysEnabled(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( TRUE );
}

afx_msg void CPictPubApp::OnUpdateNeedImage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( (GetActiveImage() != NULL) ? TRUE : FALSE );
}

extern void OnStatusShield(int id);
extern void OnStatusPalette(int id);
extern void OnStatusMask();
extern void OnStatusMaskEditAlpha(int id);
extern void OnViewSlime(int id);

afx_msg void CPictPubApp::OnStatusInfo()
{
	// this function should go in CServerView
	CServerView *pView = PictPubApp.GetActiveView();
	if (pView)
	{
	 	CServerDoc *pDocument = pView->GetDocument();  
		if (pDocument)
        	pDocument->DoSummaryInfo();
	}
}
	
afx_msg void CPictPubApp::OnViewSlime()
   {
   if (!GetActiveImage()) return; 
   ::OnViewSlime(IDC_VIEWSLIME);
   }

afx_msg void CPictPubApp::OnShield()
   {
   OnStatusShield(IDC_STATUSSHIELD);
   }

afx_msg void CPictPubApp::OnPalette()
    {
    OnStatusPalette(IDC_STATUSPALETTE);
    }

afx_msg void CPictPubApp::OnUpdatePalette (CCmdUI* pCmdUI)
	{
	// Update the ShowHide State of the Color Palette Menu Item
	HMENU hMenu;
	HWND hWnd = AstralDlgGet( IDD_PALETTE );
	BOOL Bool = ( hWnd && IsWindowVisible( hWnd ) );
	if (hMenu = ::GetMenu( PictPubApp.Get_hWndAstral() ))
		ShowHideMenu( IDC_STATUSPALETTE, !Bool, hMenu);
	pCmdUI->Enable( TRUE );
	}

afx_msg void CPictPubApp::OnStatusMask()
   {
   if (!GetActiveImage()) return; 
   OnStatusMask();
   }

afx_msg void CPictPubApp::OnEditAlpha()
{
    if( !Control.Retail )
        return;

    if (!GetActiveImage()) 
        return; 

    OnStatusMaskEditAlpha(IDC_MASKEDITALPHA);
}

afx_msg void CPictPubApp::OnAbout()
{
    /* Bring up the modal 'About' dialog box */
    AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), 
		IDD_ABOUT, DlgAboutProc);
}

afx_msg void CPictPubApp::OnPasteAsNew()
{
	LPOBJECT lpObject;
	FRMTYPEINFO TypeInfo;
   CDocument   *pDoc;

    DeactivateTool();

    // First put clipboard contents into a file(s)
	// see if we are the clipboard owner, if so we just need to
	// copy the data we need

	FrameSetTypeInfo(&TypeInfo, FDT_NONE);
	lpObject = GetClipboardObject();

	if (!lpObject)
		return;

   ProgressBegin(1, PROGRESS_ID(IDS_UNDO2));
	LPIMAGE lpNewImage = CreateImage(
					   		NULL,
					   		NULL,
					   		lpObject,
					   		NULL,
					   		Control.DefaultFileType,
					   		GetFrameFileDataType(ObjGetEditFrame(lpObject)),
							IMG_DOCUMENT,
							NULL);
	if (lpNewImage)
	{
	   	if (pDoc = PictPubApp.OpenDocumentFile((LPTSTR)lpNewImage->CurFile,
    								   	lpNewImage))         
            pDoc->SetModifiedFlag();
	}   
	ProgressEnd();
}

afx_msg void CPictPubApp::OnUpdatePasteAsNew(CCmdUI* pCmdUI)
{
	int state = MF_GRAYED;

	if ( AfxGetMainWnd()->OpenClipboard() )
	{
	    if ( IsClipboardFormatAvailable(CF_DIB) ||
		     IsClipboardFormatAvailable(CF_BITMAP) ||
		     IsClipboardFormatAvailable(Control.cfImagePP4) ||
	    	 IsClipboardFormatAvailable(Control.cfImage) )
	       	state = MF_ENABLED;
	    CloseClipboard();
	}
	pCmdUI->Enable( state == MF_ENABLED);
}

afx_msg void CPictPubApp::OnMacroPlay()
{
    if( !Control.Retail )
        return;

    int i;
	FNAME szFileName;

    if ( IsRecording() || IsPlaying() )
        return;
    if (!AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), 
                IDD_MACROPLAY, DlgMacroPlayProc))
        return;
    if ( !LookupExtFile( Names.Macro, szFileName, IDN_MACRO ) )
        return;
    if (Macro.fJournalMacro)
    {
        i = Macro.nRepeat;
        while (i--)
            PlayJournalMacro(szFileName);
    }
    else
    {
        PlayMacro(NULL, szFileName, Macro.nRepeat, NO, NULL, NULL, 0, Names.Macro);
    }
}

afx_msg void CPictPubApp::OnMacroPlayBatch()
{
    if( !Control.Retail )
        return;

    if ( !IsRecording() && !IsPlaying() )
	{
		LIST QueueList;

    	if ( AstralDlgParam( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_MACRO_BATCH,
						DlgBatchMacroProc, (LPARAM)&QueueList))
			PlayBatchMacro(&QueueList);
	}
}

afx_msg void CPictPubApp::OnMacroRecord()
{
    if( !Control.Retail )
        return;

    if ( IsRecording() || IsPlaying() )
        return;
    if (AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_MACRORECORD, DlgMacroRecordProc ))
    {
		FNAME szFileName;

        if ( !LookupExtFile( Names.Macro, szFileName, IDN_MACRO ) )
        {
            if ( !CreateFileName( Names.Macro, IDN_MACRO, szFileName ) )
                return;
        }
        if (Macro.fJournalMacro)
            RecordJournalMacro(szFileName);
        else
            RecordMacro(szFileName);
    }
}

// used for MacroPlay, MacroPlayBatch, and MacroPlayBatch
afx_msg void CPictPubApp::OnUpdateMacroPlay(CCmdUI* pCmdUI)
{
	int state = MF_GRAYED;

	if (!IsPlaying() && !IsRecording() )
		state = MF_ENABLED;

	pCmdUI->Enable( state == MF_ENABLED);
}

afx_msg void CPictPubApp::OnMacroStop()
{
    if( !Control.Retail )
        return;

    if ( IsRecording() )
    {
		FNAME szFileName;

        if (Macro.fJournalMacro)
            RemoveRecorder();
        else if (StopMacro(szFileName))
        {
            if (!AddExtFile(Names.Macro, szFileName, IDN_MACRO))
                FileDelete( szFileName );
        }
    }
}

afx_msg void CPictPubApp::OnUpdateMacroStop(CCmdUI* pCmdUI)
{
	int state = MF_GRAYED;

	if ( IsRecording() )
		state = MF_ENABLED;

	pCmdUI->Enable( state == MF_ENABLED);
}

afx_msg void CPictPubApp::OnExit()
{
    DeactivateTool();
    AfxGetMainWnd()->PostMessage( WM_CLOSE, 0, 0L);
}

afx_msg void CPictPubApp::OnMacroEdit()
{
    if( !Control.Retail )
        return;

    if (AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), 
                IDD_MACRO_LOAD, DlgMacroPlayProc))
	{
		MACROEDIT MacroEdit;

		lstrcpy(MacroEdit.szExtName, Names.Macro);
		if (AstralDlgParam( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_MACRO_EDIT,
                  DlgMacroEditProc, (LPARAM)(LPMACROEDIT)&MacroEdit ))
	        PlayMacro(NULL, NULL, 1, NO, &MacroEdit.List);
	}
}

afx_msg void CPictPubApp::OnUpdateExit(CCmdUI* pCmdUI)
{
	LPIMAGE lpImage = GetActiveImage();
	ITEMID id;
	LPTSTR lpString;

	// Set the Exit menu item based on whether a client owns the image
	id = ( ( lpImage && lpImage->fOwnedByClient ) ? IDS_UPDATEEXIT : IDS_EXIT );
	if ( pCmdUI->m_pMenu && AstralStr( id, &lpString ) )
		pCmdUI->SetText(lpString);
}

afx_msg void CPictPubApp::OnNew()
{
    DeactivateTool();
    /* Bring up the modal 'New' box */
    if (AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_NEW, DlgNewProc ))
    {
        NEW_PARMS parms;
    
        parms.Type = (FRMDATATYPE)Control.NewDepth;
        parms.Width = Control.NewWidth;
        parms.Height = Control.NewHeight;
        parms.Resolution = Control.NewResolution;
	    parms.Color = Edit.FillColor;
	    parms.CmsInfo = ColorManager.FileNew;
        ProcessLoadCommand(IDS_CMD_NEWIMAGE, &parms);
    }
}

afx_msg void CPictPubApp::OnOpen()
{
    DeactivateTool();
    /* Bring up the modal 'File Open' box */
    HandleOpenCommand(PictPubApp.Get_hWndAstral());
}

afx_msg void CPictPubApp::OnImport()
{
    if( !Control.Retail )
        return;

    DeactivateTool();
    /* Bring up the modal 'File Import' box */
    HandleImportCommand(PictPubApp.Get_hWndAstral());
}

afx_msg void CPictPubApp::OnRevert()
{
	FNAME szFileName;
	ITEMID idFileType;
	int PhotoCDResOverride;
	LPCMDLIST lpCmdList;

	CServerView *pView = PictPubApp.GetActiveView();
	if (!pView)
		return;


	LPIMAGE lpImage = pView->GetDocument()->m_lpImage;
	if (!lpImage)
		return;

    DeactivateTool();
    if ( lpImage->fUntitled ) // If the picture doesn't have a name, get out
        return;
    if ( ImgChanged(lpImage) )
        {
        /* Check to see if its OK to trash changes */
        if ( AstralOKCancel(IDS_OKTOREVERT,
                (LPTR)lpImage->CurFile) == IDCANCEL )
            return;
        }
    lstrcpy( szFileName, lpImage->CurFile );
    idFileType = lpImage->FileType;
    //fDoZoom = pView->GetParentFrame()->IsZoomed();
    //fFastBits = (lpImage->DocumentType == IMG_DOCPIECE);
    //fHasZoom = Window.fHasZoom;

	LPCMDPKT lpLoadPkt = (LPCMDPKT)ListGetHead(&lpImage->lpCmdList->UndoPacketList);
	if (lpLoadPkt)
		ListUnlink(&lpImage->lpCmdList->UndoPacketList, lpLoadPkt);

    CloseImage( NO, lpImage, NULL ); // Close the active image

	if (!lpLoadPkt)
	{
		LOADFILE_PARMS parms;
		if (!CreateLoadFileParms(idFileType, szFileName, TRUE,
							NULL, &PhotoCDResOverride, &parms))
			return;
		lpLoadPkt = CreatePacket(IDS_CMD_LOADFILE, &parms, TRUE);
		if (!lpLoadPkt)
			return;
	}

	// create a command list for the load command
	if (lpCmdList = CreateCommandList())
	{
		ListAddTail(&lpCmdList->PacketList, lpLoadPkt);
		lpCmdList->PhotoCDResOverride = PhotoCDResOverride;
		ExecuteCommands(lpCmdList);
	}
	else
		FreeUpPacket(lpLoadPkt);
		
	// does the code below really need to be called, shouldn't 
	// activations take care of it
	#ifdef UNUSED
    // restore zoom if needed
    if (fHasZoom && !Window.fHasZoom)
        {
		HWND hWnd;

        Window.fHasZoom = TRUE;
        Window.fHasZoom = HookZoom();
        if ( Tool.hRibbon )
            ::SendMessage( Tool.hRibbon, WM_DOCACTIVATED, 0, 1L );
        if ( hWnd = AstralDlgGet(IDD_VIEWING) )
            ::CheckDlgButton(hWnd, IDC_CUSTOMVIEW, Window.fHasZoom);
        }
	#endif

    // Don't use this because lpImage is not the same anymore
    // OleRevertServerDoc( lpImage->lhDoc );
}

afx_msg void CPictPubApp::OnUpdateRevert(CCmdUI* pCmdUI)
{
	CServerView *pView = PictPubApp.GetActiveView();
	BOOL fNotNormal;
	int state = MF_GRAYED;

	if (pView)
   	{
	   	LPIMAGE lpImage = GetActiveImage();
   		if (lpImage)
   		{
   			fNotNormal = (lpImage->DocumentType == IMG_DOCPIECE) ||
						 (lpImage->DocumentType == IMG_LOWRES);
			if (fNotNormal)
			{
				if (ListGetCount(&lpImage->lpCmdList->UndoPacketList))
					fNotNormal = FALSE;
			}
   			if ( !fNotNormal && !lpImage->fUntitled && pView->GetDocument()->IsModified() && !ImageIsBusy(lpImage))
      			state = MF_ENABLED;
		}				 
   	}
	pCmdUI->Enable( state == MF_ENABLED);
}

afx_msg void CPictPubApp::OnScanSetup()
{
    CScanSetupDlg dlg(CWnd::FromHandle(PictPubApp.Get_hWndAstral()));
    dlg.DoModal();
}

afx_msg void CPictPubApp::OnMonitorMap()
{
	CMonitorPropSheet	MonitorSheet(IDS_MONITORSETUP, AfxGetMainWnd(), IDD_MONITORSETUP);
	MonitorSheet.DoModal(); 
}							  

afx_msg void CPictPubApp::OnPrintSetup()
{
    /* Bring up the setup dialog box for the active printer */
    AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_PRINTSETUP,
        DlgPrintSetupProc);
}

afx_msg void CPictPubApp::OnCalibrateScan()
{
    if( !Control.Retail )
        return;

	ITEMID idReturn;

	idReturn = Scans.fScanCalVisual ? IDD_CALIBRATESCANVISUAL : IDD_CALIBRATESCAN;
	while (	idReturn == IDD_CALIBRATESCANVISUAL ||
			idReturn == IDD_CALIBRATESCAN)
	{
		if (idReturn == IDD_CALIBRATESCAN)
        {
            CCalibrateScanDlg *pDlg = 
                new CCalibrateScanDlg(GetActiveView(), idReturn,
                    CWnd::FromHandle(Get_hWndAstral()));
            if (pDlg)
            {
                idReturn = pDlg->DoModal();
                delete pDlg;
            }
            else
                idReturn = FALSE;
        }
        else
        {
            CScanVisualDlg *pDlg = 
                new CScanVisualDlg(GetActiveView(), idReturn,
                    CWnd::FromHandle(Get_hWndAstral()));
            if (pDlg)
            {
                idReturn = pDlg->DoModal();
                delete pDlg;
            }
            else
                idReturn = FALSE;
        }
	}
}

afx_msg void CPictPubApp::OnCalibratePrint()
{
    if( !Control.Retail )
        return;

	ITEMID idReturn;

	idReturn = Control.fPrintCalVisual ? IDD_CALIBRATEPRINTVISUAL : IDD_CALIBRATEPRINT;
	while (	idReturn == IDD_CALIBRATEPRINTVISUAL ||
			idReturn == IDD_CALIBRATEPRINT)
	{
		if (idReturn == IDD_CALIBRATEPRINT)
        {
            CCalibratePrintDlg *pDlg = new CCalibratePrintDlg(
                GetActiveView(), idReturn, CWnd::FromHandle(Get_hWndAstral()));
            if (pDlg)
            {
                idReturn = pDlg->DoModal();
                delete pDlg;
            }
            else
                idReturn = FALSE;
        }
        else
        {
            CPrintVisualDlg *pDlg = new CPrintVisualDlg(   
                GetActiveView(), idReturn, CWnd::FromHandle(Get_hWndAstral()));
            if (pDlg)
            {
                idReturn = pDlg->DoModal();
                delete pDlg;
            }
            else
                idReturn = FALSE;
        }
	}
}

afx_msg void CPictPubApp::OnScratchPad()
{
	int      iDepth, iNewDataType;
	int      pix, lin;
	int      sy;
	RGBS     rgb;
	long     pixel;
	LPTSTR    lpName;
	LPTR     lpBuffer = NULL;
	LPFRAME  lpFrame = NULL;
    TCHAR     szTitle[20];
    static   WORD uScratchPadCount = 0;

    DeactivateTool();
	ProgressBegin(1,0);
	iDepth = ( Control.IsScratchpadColor ? 3 : 1 );
	pix = Control.ScratchpadWidth;
	lin = Control.ScratchpadHeight;

	if ( !(lpFrame = FrameOpen( ( FRMDATATYPE )iDepth, pix, lin, 100 /*Resolution*/ )) )
    {
    	Message( IDS_ESCROPEN, (LPTR)Control.RamDisk );
    	goto ExitFalse;
    }

	if ( !(AllocLines( &lpBuffer, 1, pix, iDepth )) )
    	goto ExitFalse;

	rgb.red = rgb.green = rgb.blue = 255;

	FrameRGB2Pixel(lpFrame, &rgb, &pixel);

	FrameSetPixel(lpFrame, pixel, lpBuffer, pix);

	for ( sy=0; sy<lin; sy++ )
    {
    	if (AstralClockCursor( sy, lin, YES ))
        	goto ExitFalse;
    	if ( !FrameWrite( lpFrame, 0, sy, pix, lpBuffer, pix ) )
       	{
        	Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
        	goto ExitFalse;
       	}
   	}

	FreeUp( lpBuffer );
	lpBuffer = NULL;

	// Setup the new image and bring up the new image window 
    //
	if ( !AstralStr( IDS_SCRATCHPAD, &lpName ))
    	szTitle[0] = _T('\0');
    else
        wsprintf( szTitle,"%s%u", lpName, ++uScratchPadCount );

	switch(iDepth)
	{
    	case FDT_LINEART:
    	case FDT_GRAYSCALE :
        	iNewDataType = IDC_SAVECT; 
    	break;

    	case FDT_RGBCOLOR :
        	iNewDataType = IDC_SAVE24BITCOLOR; 
    	break;

    	case FDT_CMYKCOLOR :
        	iNewDataType = IDC_SAVE32BITCOLOR; 
    	break;
	}

	{
    	LPIMAGE lpNewImage = CreateImage(( LPTSTR )szTitle, lpFrame, NULL, 
       	    NULL, Control.DefaultFileType, iNewDataType, IMG_DOCUMENT, NULL);
    	if (!lpNewImage)
    	{
           	ProgressEnd();
           	return; 
    	}   
    	else
    	{
       	    if (!PictPubApp.NewDocumentFile(lpNewImage))                     
           	{
               	ProgressEnd();
               	return; 
           	}                      
           	else
       	    {
           	    lpNewImage->fUntitled = YES;
               	ProgressEnd();  
               	return;
       	    }
    	}   
	}

	ExitFalse:
	if(lpFrame)
    	FrameClose(lpFrame);
	if (lpBuffer)
    	FreeUp(lpBuffer);
	ProgressEnd();  
}

afx_msg void CPictPubApp::OnUpdateScratchPad(CCmdUI* pCmdUI)
{
	LPTSTR lpString;
	
	if ( pCmdUI->m_pMenu && AstralStr( IDS_CREATE_SCRATCHPAD, &lpString ) )
		pCmdUI->SetText(lpString);
	pCmdUI->Enable(  TRUE );
}

afx_msg void CPictPubApp::OnStatusShow()
{
	((CPPToolList*)OCToolList)->CreateStatus((CFrameWnd*)AfxGetMainWnd(), NULL);
}

afx_msg void CPictPubApp::OnUpdateStatusShow(CCmdUI* pCmdUI)
{
	HWND hWnd;
	LPTSTR lpString;
	
   	ITEMID id = IDS_SHOW_STATUS;
	if ( (hWnd = AstralDlgGet( IDD_STATUS )) && ::IsWindowVisible( hWnd ) )
		++id;
	if ( pCmdUI->m_pMenu && AstralStr( id, &lpString ) )
		pCmdUI->SetText(lpString);
	pCmdUI->Enable(  TRUE );
}

afx_msg void CPictPubApp::OnTaskManShow()
{
#ifdef WIN32
	HWND hWnd;

    if ( hWnd = AstralDlgGet( IDD_TASKMANAGER ) )
        FORWARD_WM_CLOSE(hWnd, SendMessage);
    else
        AstralDlg( YES, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_TASKMANAGER, DlgTaskManagerProc);
#endif
}

afx_msg void CPictPubApp::OnUpdateTaskManShow(CCmdUI* pCmdUI)
{
	LPTSTR lpString;
	HWND hWnd;
	
   	ITEMID id = IDS_SHOW_TASKMAN;
	if ( (hWnd = AstralDlgGet( IDD_TASKMANAGER )) && ::IsWindowVisible( hWnd ) )
		++id;
	if ( pCmdUI->m_pMenu && AstralStr( id, &lpString ) )
		pCmdUI->SetText(lpString);
#ifdef WIN32
	pCmdUI->Enable(  TRUE );
#else
	pCmdUI->Enable(  FALSE );
#endif
}

afx_msg void CPictPubApp::OnToolShow()
{
   PictPubApp.GetCustomBoxes()->CreateMain((CFrameWnd*)AfxGetMainWnd());
}

afx_msg void CPictPubApp::OnUpdateToolShow(CCmdUI* pCmdUI)
{
   HMENU hMenu;
   BOOL  fShow;

   hMenu = ::GetMenu(Get_hWndAstral());
   if (PictPubApp.GetCustomBoxes()->IsMainUp())
      fShow = NO;
   else
      fShow = YES;
   if( !pCmdUI->m_pOther )      // if not a toolbar button
       OnUpdateShowHideMenu( IDM_TOOLSHOW, fShow, pCmdUI );
   pCmdUI->Enable(TRUE);
}

afx_msg void CPictPubApp::OnObjManager()
{
    if( !Control.Retail )
        return;

	HWND hWnd;

    if ( hWnd = ObjFloat.GetSafeHwnd())
       FORWARD_WM_CLOSE(hWnd, ::SendMessage);
    else
       ObjFloat.Create();
}

afx_msg void CPictPubApp::OnUpdateObjManager(CCmdUI* pCmdUI)
{
    if( !pCmdUI->m_pOther )     // if not a toolbar button
	    OnUpdateSetShowHideMenus(IDM_OBJ_MANAGER, IDD_OBJFLOAT, pCmdUI );
    pCmdUI->Enable(TRUE);
}

afx_msg void CPictPubApp::OnArrangeIcons()
{
    ::SendMessage( PictPubApp.Get_hClientAstral(), WM_MDIICONARRANGE, 0, 0L );
}

afx_msg void CPictPubApp::OnCascade()
{
    ::SendMessage( PictPubApp.Get_hClientAstral(), WM_MDICASCADE, 0, 0L );
}

afx_msg void CPictPubApp::OnTile()
{
    ::SendMessage( PictPubApp.Get_hClientAstral(), WM_MDITILE, 0, 0L );
}

afx_msg void CPictPubApp::OnBHintsShow()
{
    Hints.fBubbleHintsOn = !Hints.fBubbleHintsOn;
    if( !Hints.fBubbleHintsOn )
    {
        HelpContext.SetHintState( UINT( 0 ));               // flush current ID
        HelpContext.BubbleHintsOn( NO );                    // reset states
        HelpContext.SetHintWindows( NULL, NULL, HT_OFF );   // turn off timer
    }
    else
    {
        HelpContext.BubbleHintsOn( YES );                   // reset states
        HelpContext.SetHintWindows( NULL, NULL, HINTS_ON ); // turn on timer
    }
}

afx_msg void CPictPubApp::OnUpdateBHintsShow(CCmdUI* pCmdUI)
{
    if( !pCmdUI->m_pOther )     // if not a toolbar button
        OnUpdateShowHideMenu( IDM_BHINTSSHOW, !Hints.fBubbleHintsOn, pCmdUI );
	pCmdUI->Enable(  TRUE );
}

afx_msg void CPictPubApp::OnSHintsShow()
{
    if(Hints.fStatusHintsOn)
        HintLine( 0 );          // flush hint from status bar
    Hints.fStatusHintsOn = !Hints.fStatusHintsOn;
}

afx_msg void CPictPubApp::OnUpdateSHintsShow(CCmdUI* pCmdUI)
{
	HWND     hWnd;
   BOOL     StatusVisible = TRUE;   

    if ( hWnd = AstralDlgGet( IDD_STATUS ) )
        {
        if ( !::IsWindowVisible( hWnd ) )
            StatusVisible = FALSE;
        }
    else
        StatusVisible = FALSE;
    if( !pCmdUI->m_pOther )     // if not a toolbar button   
        OnUpdateShowHideMenu( IDM_SHINTSSHOW, !Hints.fStatusHintsOn || !StatusVisible, pCmdUI );
	pCmdUI->Enable(StatusVisible);
}

afx_msg void CPictPubApp::OnWHintsShow()
{
    Hints.fHintWindowOn = !Hints.fHintWindowOn;
}

afx_msg void CPictPubApp::OnUpdateWHintsShow(CCmdUI* pCmdUI)
{
    if( !pCmdUI->m_pOther )     // if not a toolbar button   
        OnUpdateShowHideMenu( IDM_WHINTSSHOW, !Hints.fHintWindowOn, pCmdUI );
	pCmdUI->Enable( TRUE );
}

afx_msg void CPictPubApp::OnShowInfo()
{
	HWND hWnd;

    if ( hWnd = AstralDlgGet( IDD_INFOFLOAT ) )
    {
        if ( ::IsWindowVisible( hWnd ) )
            ::ShowWindow( hWnd, SW_HIDE );
        else
        if ( ::IsIconic( hWnd ) )
            ::ShowWindow( hWnd, SW_MINIMIZE );
        else
            ::ShowWindow( hWnd, SW_NORMAL );
    }
    else
        AstralDlg( YES, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_INFOFLOAT, DlgInfoFloatProc);
}

afx_msg void CPictPubApp::OnUpdateShowInfo(CCmdUI* pCmdUI)
{
	HWND hWnd;
	LPTSTR lpString;
	
   	ITEMID id = IDS_SHOW_INFO;
	if ( (hWnd = AstralDlgGet( IDD_INFOFLOAT )) && ::IsWindowVisible( hWnd ) )
		++id;
	if ( pCmdUI->m_pMenu && AstralStr( id, &lpString ) )
		pCmdUI->SetText(lpString);
	pCmdUI->Enable(  TRUE );
}

afx_msg void CPictPubApp::OnShowZoomBox()
{
	HWND hWnd;

    if (!Window.fHasZoom)
        {
        Window.fHasZoom = TRUE;
        Window.fHasZoom = HookZoom();
        if ( Tool.hRibbon )
            ::SendMessage( Tool.hRibbon, WM_DOCACTIVATED, 0, 1L );
        }
    else
        {
        if (hZoomWindow) // there is a zoom
            {
            ::DestroyWindow(hZoomWindow);
            hZoomWindow = NULL;
            }
        Window.fHasZoom = FALSE;
        if ( Tool.hRibbon )
            ::SendMessage( Tool.hRibbon, WM_DOCACTIVATED, 0, 1L );
        }
    if ( hWnd = AstralDlgGet(IDD_VIEWING) )
        ::CheckDlgButton(hWnd, IDC_CUSTOMVIEW, Window.fHasZoom);
}

afx_msg void CPictPubApp::OnUpdateShowZoomBox(CCmdUI* pCmdUI)
{
	LPTSTR lpString;
	LPIMAGE lpImage = GetActiveImage();
	
	ITEMID id = IDS_SHOWZOOMBOX;
	if (Window.fHasZoom && lpImage)
		++id;
	if ( pCmdUI->m_pMenu && AstralStr( id, &lpString ) )
		pCmdUI->SetText(lpString);
	pCmdUI->Enable(  lpImage != NULL );
}

afx_msg void CPictPubApp::OnCloseAll()
{
	BOOL bEndSession = FALSE;

    DeactivateTool();   

	// loop thru all templates types and then all open documents of the template
	// type attempting to close all docs down.  If one document refuses to close
	// abort close all operation
	POSITION pos = m_templateList.GetHeadPosition();
	while (pos != NULL)
	{
		CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetNext(pos);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CDocTemplate)));
		POSITION pos2 = pTemplate->GetFirstDocPosition();
		while (pos2 != NULL)
		{
			CDocument* pDoc = pTemplate->GetNextDoc(pos2);

		    if (!pDoc->SaveModified())
				return; // user canceled-out of close all

			if (bEndSession)
				pDoc->DisconnectViews();

			pDoc->OnCloseDocument();
		}
	}
}

afx_msg void CPictPubApp::OnSavePositions()
{
	RECT rect;
	int dx, dy;
	STRING szString;
	HWND hDlg;

    SavePopup( AstralDlgGet(IDD_SHIELDFLOAT), _T("xyShield") );
    SavePopup( AstralDlgGet(IDD_PALETTE), _T("xyPalette") );
    SavePopup( ObjFloat.GetSafeHwnd(), _T("xyObjects") );
    SavePopup( AstralDlgGet(IDD_INFOFLOAT), _T("xyInfo") );
    SavePopup( AstralDlgGet(IDD_TASKMANAGER), _T("xyTaskMan") );
	// there are 2 possible clip browsers
  	hDlg =  AstralDlgGet(IDD_PASTEFROMFILE);
	if (!hDlg)
	  	hDlg =  AstralDlgGet(IDD_PASTEFROMFILE_THUMB);
    SavePopup(hDlg, _T("xyClipBrowser") );
	
    SavePopup( hZoomWindow, _T("xyZoom"));
    if (hZoomWindow)
    {
        ::GetWindowRect( hZoomWindow, &rect );
        dx = RectWidth(&rect);
        dy = RectHeight(&rect);
        dx = Max(dx,dy);
        PutDefaultString(_T("sizeZoom"), Int2ToAscii( szString, dx, dx) );
    }
	HWND hActiveWnd = GetActiveDoc();
    if ( hActiveWnd )
        {
		::GetWindowRect( ::GetParent(hActiveWnd), &rect );
        ::ScreenToClient( ::GetParent(::GetParent(hActiveWnd)), (LPPOINT)&rect.left );
        PutDefaultInt( _T("xImage"), rect.left );
        PutDefaultInt( _T("yImage"), rect.top );
		// Update the current defaults
		Control.xImage=rect.left;
		Control.yImage=rect.top;        
	}
    ::GetWindowRect( PictPubApp.Get_hWndAstral(), &rect );
    PutDefaultString( _T("xyApp"), Int4ToAscii( szString,
        rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top ) );
}

afx_msg void CPictPubApp::OnSizeUp()
{
	on_brush_size(IDM_SIZEUP);
}

afx_msg void CPictPubApp::OnSizeDown()
{
	on_brush_size(IDM_SIZEDOWN);
}

void CPictPubApp::on_brush_size(int idCmd)
{
    if (!Retouch.hBrush)
        return;
    ::SetFocus( PictPubApp.Get_hWndAstral() ); // Take focus away from any controls
    if ( Retouch.BrushShape == IDC_BRUSHCUSTOM )
        return;
    if ( idCmd == IDM_SIZEUP )
        Retouch.BrushSize++;
    else    Retouch.BrushSize--;
    if ( Retouch.BrushSize < 1 || Retouch.BrushSize > MAX_BRUSH_SIZE )
        {
        Retouch.BrushSize = bound( Retouch.BrushSize, 1,MAX_BRUSH_SIZE);
        MessageBeep(0);
        return;
        }
	HWND hActiveWnd = GetActiveDoc();
    if (hActiveWnd)
        DisplayBrush(0, 0, 0, OFF);
    SetMgxBrushSize(Retouch.hBrush, Retouch.BrushSize);
    if (hActiveWnd && Window.hCursor == Window.hNullCursor)
        DisplayBrush(hActiveWnd, INT_MAX, INT_MAX, ON);
    if ( Tool.hRibbon )
        ::SetDlgItemInt( Tool.hRibbon, IDC_BRUSHSIZE, Retouch.BrushSize, NO );
}

afx_msg void CPictPubApp::OnShapeUp()
{
	on_brush_shape(IDM_SHAPEUP);
}

afx_msg void CPictPubApp::OnShapeDown()
{
	on_brush_shape(IDM_SHAPEDOWN);
}

afx_msg void CPictPubApp::on_brush_shape(int idCmd)
{
	ITEMID NewShape;

    if (!Retouch.hBrush)
        return;
    ::SetFocus( PictPubApp.Get_hWndAstral() ); // Take focus away from any controls
    NewShape = Retouch.BrushShape;
    if ( idCmd == IDM_SHAPEUP )
        NewShape++;
    else    NewShape--;
    if ( NewShape > IDC_BRUSHCUSTOM )
        NewShape = IDC_BRUSHCIRCLE;
    if ( NewShape < IDC_BRUSHCIRCLE )
        NewShape = IDC_BRUSHCUSTOM;
	LPIMAGE lpImage = GetActiveImage();
    if (lpImage)
        DisplayBrush(0, 0, 0, OFF);
    if (!SetMgxBrushShape(Retouch.hBrush, NewShape, Names.CustomBrush))
        {
        NewShape = IDC_BRUSHCIRCLE;
        SetMgxBrushShape(Retouch.hBrush, NewShape, Names.CustomBrush);
        }
    Retouch.BrushShape = NewShape;
	HWND hActiveWnd = GetActiveDoc();
    if (hActiveWnd && Window.hCursor == Window.hNullCursor)
        DisplayBrush(hActiveWnd, INT_MAX, INT_MAX, ON);
    if ( Tool.hRibbon )
        {
        CheckComboItem( Tool.hRibbon, IDC_BRUSHSHAPE, IDC_BRUSHCIRCLE,
            IDC_BRUSHCUSTOMNEW, Retouch.BrushShape );
        ::SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
        }
}

afx_msg void CPictPubApp::OnPref()
{
	CPreferencesPropSheet PrefSheet(IDS_PREFERENCES, AfxGetMainWnd(), IDD_PREF);
	PrefSheet.DoModal();
}

afx_msg void CPictPubApp::OnHelpContextMode()
{
    StartContextHelp();
}

afx_msg void CPictPubApp::OnHelpContext()
{
    /* Bring up the help system on the current tool */
    Help( HELP_CONTEXT, (long)Tool.id );
}

afx_msg void CPictPubApp::OnHelpIndex()
{
    /* Bring up the help system at the index */
    Help( HELP_CONTEXT, (long)IDM_HELP_INDEX );
//  Help( HELP_CONTEXT, -1L );
//  Help( HELP_INDEX, 0L );
}

afx_msg void CPictPubApp::OnHelpUsing()
{
    /* Bring up the help system at using Picture Publisher */
    Help( HELP_CONTEXT, (long)IDM_HELP_USING );
}

afx_msg void CPictPubApp::OnHelpReadMe()
{
    /* Bring up the help system at commands */
    Help( HELP_CONTEXT, (long)IDM_HELP_README );
}

afx_msg void CPictPubApp::OnHelpOnHelp()
{
    /* Bring up the help system with help on help */
//  Help( HELP_CONTEXT, (long)idCmd );
    Help( HELP_HELPONHELP, -1L );
}

afx_msg void CPictPubApp::OnNewWindow()
{   
    DeactivateTool();

	CMDIFrame* pAppFrame = (CMDIFrame*)AfxGetMainWnd();
    CPPMDIChildWnd* pActiveChild = (CPPMDIChildWnd*)pAppFrame->MDIGetActive();
    CServerDoc* pDocument;       
    
    if (pActiveChild == NULL ||
      (pDocument = (CServerDoc*)pActiveChild->GetActiveDocument()) == NULL)
    {
        TRACE0(_T("Warning: No active document for xxWindowNew command\n"));
        AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
        return;     // command failed
    }
    
    ASSERT(pDocument->m_lpImage);

    // otherwise we have a new frame !
    CPPMultiDocTemplate* pTemplate = 
        (CPPMultiDocTemplate*)pDocument->GetDocTemplate();
    ASSERT_VALID(pTemplate);
    CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDocument, pActiveChild);
    if (pFrame == NULL)
    {
        TRACE0(_T("Warning: failed to create new frame\n"));
        return;     // command failed
    }

    pTemplate->InitialUpdateFrame(pFrame, pDocument);
}

afx_msg void CPictPubApp::OnUpdateNewWindow(CCmdUI* pCmdUI)
{
	CMDIFrame* pAppFrame = (CMDIFrame*)AfxGetMainWnd();
    CPPMDIChildWnd* pActiveChild = (CPPMDIChildWnd*)pAppFrame->MDIGetActive();
    CServerDoc* pDocument = NULL;       
    
	if (pActiveChild)
		pDocument = (CServerDoc*)pActiveChild->GetActiveDocument();

	if (pActiveChild == NULL ||
		pDocument == NULL)
	    pCmdUI->Enable(	FALSE );
	else
	    pCmdUI->Enable(	TRUE );
}

afx_msg void CPictPubApp::OnAcquire()
{   
    DeactivateTool();
    DCAcquire();
}

afx_msg void CPictPubApp::OnUpdateAcquire(CCmdUI* pCmdUI)
{
	if (CheckTwainOK())
	    pCmdUI->Enable(	TRUE );
	else
	    pCmdUI->Enable(	FALSE );
		
}

afx_msg void CPictPubApp::OnSendMail()
{
	if (m_pMail)
	{
		if (!m_pMail->IsLoggedOn())
			m_pMail->Logon();

		if (m_pMail->IsLoggedOn())
			m_pMail->SendMail();
	}
}

afx_msg void CPictPubApp::OnUpdateSendMail(CCmdUI* pCmdUI)
{
    if (!m_bMailInitDone)
    {
    	/* Initialize Mail Enable */
    	m_pMail = new CMapiMail(::AfxGetMainWnd()->GetSafeHwnd(),
    		::AfxGetInstanceHandle());
    	if (m_pMail)
    		m_pMail->InitMail(FALSE); // initialize with NO autologon
        m_bMailInitDone = TRUE;
    }
	pCmdUI->Enable(m_pMail != NULL);
}

afx_msg void CPictPubApp::OnPasteFromFile()
{
	// bring up modless dialog
     AstralDlg( YES, GetResourceHandle(), Get_hWndAstral(),
	 	ExtBrowser.PasteViewThumbnails ? 
		IDD_PASTEFROMFILE_THUMB:IDD_PASTEFROMFILE,DlgPasteFromFileProc);

}

afx_msg void CPictPubApp::OnUpdatePasteFromFile(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}
