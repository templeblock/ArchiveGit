//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include <process.h>
#include "cmdparms.h"
#include "id.h"
#include "paledit.h"
#include "cmdtable.h"
#include "cmdmacro.h"
#include "macro.h"
#include "svrview.h"
#include "id.h"

LOCAL BOOL		ExecuteCommandsEx(LPCMDLIST lpCmdList, BOOL fThisThread);
LOCAL void		CollapseCommandList(LPCMDLIST lpCmdList);
LOCAL BOOL		DispatchCommands(LPCMDLIST lpCmdList);
LOCAL BOOL		AddPacket(LPCMDLIST lpCmdList, ITEMID idCommand, LPVOID lpParms, BOOL fCopyParms);
LOCAL void		MergeLoadPacketList(LPLIST lpPacketList, LPLIST lpLoadPacketList);
LOCAL void		SetLoadOptions(LPCMDPKT lpCmdPkt, LPVOID lpOptions);

/***********************************************************************/
BOOL IsCmdEnabled(ITEMID idCommand, CServerView *pView, LPCMD_PARMS lpParms,
				  BOOL fCheckForBusy, CImage *pImage)
/***********************************************************************/
{
	int 	i;

	if (pView)
		pImage = ((CServerDoc *)pView->GetDocument())->m_lpImage;

	if (!pImage)
		return(FALSE);

	// do an initial check to see if the image is busy
	// we don't do this check if we are calling from inside
	// the command list processor
	if (fCheckForBusy)
	{
		if (pImage->IsBusy())
			return(FALSE);
	}

	// if a null command id is passed in we don't do any command
	// verification
	if (!idCommand)
		return(TRUE);

	// get index into command description table
	if ((i = GetCommandIndex(idCommand)) < 0)
	{
		Message(IDS_CMDOUTOFRANGE, idCommand);
		return(FALSE);
	}

	// see if we are not a retail build and command is
	// only allowed in retail version - LE disabling
	if (!Control.Retail && Commands[i].fRetailOnly)
		return(FALSE);

	// if we have a view and we are in-place,
	// do the in-place enabled check first
	if (pView)
	{
		CServerDoc* pDoc = pView->GetDocument();
		if (pDoc && pDoc->IsInPlaceActive())
		{
			if (!Commands[i].fEnabledInPlace)
				return(FALSE);
		}
	}
	BOOL fEnabled = (*Commands[i].lpEnableProc)(pImage, pView, lpParms);
	return(fEnabled);
}

/***********************************************************************/
BOOL ProcessLoadCommand(ITEMID idCommand, LPVOID lpParms)
/***********************************************************************/
{
LPCMDLIST lpCmdList;
				
// create a command list for the load command
if (!(lpCmdList = AddLoadCommand(idCommand, lpParms)))
	return(FALSE);
return(ExecuteCommands(lpCmdList));
}

/***********************************************************************/
void  MarkImageDirty(LPCMDLIST lpCmdList, ITEMID idCommand)
/***********************************************************************/
   {
   COMMAND_TYPE  Type = GetCommandType(idCommand);

   if ((Type == CT_EDIT || Type == CT_EDITOBJ) && lpCmdList->ThreadData.lpImage)
	   SetImgChanged(lpCmdList->ThreadData.lpImage, TRUE);	
   }

/***********************************************************************/
BOOL ProcessCommand(LPCMDLIST lpCmdList, ITEMID idCommand, LPVOID lpParms)
/***********************************************************************/
{
BOOL  bRet;
// create a command list for the load command
if (!AddCommand(lpCmdList, idCommand, lpParms))
	return(FALSE);
bRet = ExecuteCommands(lpCmdList);
MarkImageDirty(lpCmdList, idCommand);
return(bRet);
}

/***********************************************************************/
BOOL PostCommand(LPCMDLIST lpCmdList, ITEMID idCommand, LPVOID lpParms)
/***********************************************************************/
{
	BOOL bRet = AddCommandEx(lpCmdList, idCommand, lpParms, YES, YES);
		
   MarkImageDirty(lpCmdList, idCommand);
	lpCmdList->UpdateType = UT_NONE;
	FORWARD_WM_PROCESSINGDONE(PictPubApp.Get_hWndAstral(FALSE), lpCmdList,
								SendMessage);
	return(bRet);
}

/***********************************************************************/
BOOL PostProgCommand(ITEMID idCommand, LPVOID lpParms)
/***********************************************************************/
{
LPCMDPKT lpCmdPkt;

if (IsMacroRecording())
	{
	lpCmdPkt = CreatePacket(idCommand, lpParms, YES);
	if (!lpCmdPkt)
		return(FALSE);
	MacroWritePacket(lpCmdPkt);
	FreeUpPacket(lpCmdPkt);
	}
return(TRUE);
}

/***********************************************************************/
BOOL CallCommand(LPCMDLIST lpCmdList, ITEMID idCommand, LPVOID lpParms)
/***********************************************************************/
{
// create a command list for the load command
if (!AddCommand(lpCmdList, idCommand, lpParms))
	return(FALSE);
return(ExecuteCommandsEx(lpCmdList, YES));
}

/***********************************************************************/
LPCMDLIST AddLoadCommand(ITEMID idCommand, LPVOID lpParms)
/***********************************************************************/
{
return(AddLoadCommandEx(idCommand, lpParms, YES));
}

/***********************************************************************/
LPCMDLIST CreateCommandList()
/***********************************************************************/
{
LPCMDLIST lpCmdList;

// Allocate memory for the command list
if ( !(lpCmdList = (LPCMDLIST)Alloc( (long)sizeof(CMDLIST) )) )
	{
	Message(IDS_EMEMALLOC);
	return(NULL);
	}
// Clear memory for the command list
clr( (LPTR)lpCmdList, sizeof(CMDLIST) );

// create command processing thread if we are using threading
#ifdef WIN32
if (Control.UseThreading)
	CreateCmdThread(lpCmdList);
#endif

// setup unique image identifier for this command list
lpCmdList->dwImageID = ++Control.dwImageID;

lpCmdList->fSaveCommands = Control.DoCommandList;

return(lpCmdList);
}

/***********************************************************************/
LPCMDLIST AddLoadCommandEx(ITEMID idCommand, LPVOID lpParms,
					BOOL fCopyParms)
/***********************************************************************/
{
LPCMDLIST lpCmdList;
int i;

// get index into command description table
if ((i = GetCommandIndex(idCommand)) < 0)
	{
	Message(IDS_CMDOUTOFRANGE, idCommand);
	return(FALSE);
	}

// Make sure the command is a CT_LOAD which
// is the type for all commands that create images
if (Commands[i].Type != CT_LOAD)
	{
	Message(IDS_BADCMDTYPE);
	return(FALSE);
	}

// Create a command list
if ( !(lpCmdList = CreateCommandList()) )
	return(NULL);

// create and link in the new packet
if (!AddPacket(lpCmdList, idCommand, lpParms, fCopyParms))
	{
	DestroyCommandList(lpCmdList);
	return(NULL);
	}

// return the command list
return(lpCmdList);
}

/***********************************************************************/
BOOL AddCommand(LPCMDLIST lpCmdList, ITEMID idCommand, LPVOID lpParms)
/***********************************************************************/
{
return(AddCommandEx(lpCmdList, idCommand, lpParms, YES, NO));
}

/***********************************************************************/
BOOL AddCommandEx(LPCMDLIST lpCmdList, ITEMID idCommand, LPVOID lpParms,
					BOOL fCopyParms, BOOL fPostCommand)
/***********************************************************************/
{
int i;

// see if we have a valid command list pointer
ASSERT(lpCmdList != NULL);

if (!lpCmdList)
	return(FALSE);

// get index into command description table
if ((i = GetCommandIndex(idCommand)) < 0)
	{
	Message(IDS_CMDOUTOFRANGE, idCommand);
	return(FALSE);
	}

#ifdef WIN32
// if we are using threading for this command list see
// if we are already busy and if so return error
if (lpCmdList->ThreadData.ThreadInfo.hCmdEvent)
	{
	if (WaitForSingleObject(lpCmdList->ThreadData.ThreadInfo.hCmdEvent, 0) == WAIT_OBJECT_0)
		{
        Message( IDS_BUSYTHREAD );
		return(FALSE);
		}
	}
#endif

// create the command packet
if (AddPacket(lpCmdList, idCommand, lpParms, fCopyParms))
	{
	// if this is a post command (i.e.) the function has already
	// been done and this command describes it post-haste
	if (fPostCommand)
		FreeUpCommandList(lpCmdList);
	return(TRUE);
	}
return(FALSE);
}

/***********************************************************************/
BOOL ExecuteCommands(LPCMDLIST lpCmdList)
/***********************************************************************/
{
return(ExecuteCommandsEx(lpCmdList, NO));
}

/***********************************************************************/
BOOL PlaybackCommands(LPCMDLIST lpCmdList, ITEMID idProgressDlg)
/***********************************************************************/
{
BOOL fDisabled;
MACROSETUP Setup;
HWND hDlg = NULL;

// if not in a threading environment and caller wants us
// to display a progress dialog, then set it up and do it
if (!Control.UseThreading)
	{
	// disable app during playback
	fDisabled = EnableWindow(PictPubApp.Get_hWndAstral(), FALSE);
	if (idProgressDlg)
		{
		Setup.iTotal = ListGetCount(&lpCmdList->PacketList);
		Setup.idDialog = idProgressDlg;
		Setup.lpFileName = NULL;
		if (lpCmdList->ThreadData.lpImage)
			Setup.lpFileName = lpCmdList->ThreadData.lpImage->CurFile;
		else
			Setup.lpFileName = GetLoadFileName((LPCMDPKT)ListGetHead(&lpCmdList->PacketList));
	    hDlg = AstralDlgParam(	DLG_MODELESS_MULTIPLE,
								PictPubApp.GetResourceHandle(),
								PictPubApp.Get_hWndAstral(),
								idProgressDlg,
								DlgMacroStatusProc,
								(LPARAM)(LPVOID)&Setup );
		if (hDlg)
			UpdateWindow(hDlg);
		}
	}
// don't turn off undos when playing back commands
//if (lpCmdList->ThreadData.lpImage)
//	lpCmdList->ThreadData.lpImage->fDisableUndos = NO;
// indicate we are in playback mode
lpCmdList->fPlayback = YES;
// play back the commands
BOOL fRet = ExecuteCommandsEx(lpCmdList, NO);
// reenable app after playback
if (!Control.UseThreading)
	EnableWindow(PictPubApp.Get_hWndAstral(), !fDisabled);
// if we have a progress dialog, nuke it
if (hDlg)
	AstralDlgDestroy(hDlg);

return(fRet);
}

/***********************************************************************/
LPIMAGE BuildImage(LPIMAGE lpImage, LPLIST lpPacketList, int PhotoCDResOverride)
/***********************************************************************/
{
	HWND hDlg = NULL;
	BOOL fRet = FALSE;
	LPCMDLIST lpCmdList;
	MACROSETUP Setup;
	BOOL fDisabled;

	if (lpImage)
		FixExportFileNames(lpImage->CurFile, lpPacketList);

	// if not in a threading environment and caller wants us
	// to display a progress dialog, then set it up and do it
	if (!Control.UseThreading)
	{
		// disable app during playback
		fDisabled = EnableWindow(PictPubApp.Get_hWndAstral(), FALSE);
		Setup.iTotal = ListGetCount(lpPacketList);
		Setup.idDialog = IDD_BUILD_STATUS;
		Setup.lpFileName = NULL;
		if (lpImage)
			Setup.lpFileName = lpImage->CurFile;
		else
			Setup.lpFileName = GetLoadFileName((LPCMDPKT)ListGetHead(lpPacketList));
		hDlg = AstralDlgParam( DLG_MODELESS_MULTIPLE,
					PictPubApp.GetResourceHandle(),
					PictPubApp.Get_hWndAstral(), IDD_BUILD_STATUS,
					DlgMacroStatusProc,
					(LPARAM)(LPVOID)&Setup );
		if (hDlg)
			UpdateWindow(hDlg);
	}

	if (lpImage)
		lpCmdList = lpImage->lpCmdList;
	else
		lpCmdList = CreateCommandList();
	if (lpCmdList)
	{
		// don't turn off undos when playing back commands
		//lpImage->fDisableUndos = NO;
		// indicate we are in playback mode
		lpCmdList->PhotoCDResOverride = PhotoCDResOverride;
		lpCmdList->fPlayback = YES;
		// play back the commands
		fRet = DispatchPacketList(lpCmdList, lpPacketList, FALSE);
	}
	else
		Message(IDS_EMEMALLOC);

	// reenable app after playback
	if (!Control.UseThreading)
		EnableWindow(PictPubApp.Get_hWndAstral(), !fDisabled);
	// if we have a progress dialog, nuke it
	if (hDlg)
		AstralDlgDestroy(hDlg);

	if (fRet)
		return(lpCmdList->ThreadData.lpImage);
	else
	{
		if (!lpImage && lpCmdList->ThreadData.lpImage)
			DestroyImage(lpCmdList->ThreadData.lpImage);
		return(NULL);
	}
}

/***********************************************************************/
void FlushCommands(LPCMDLIST lpCmdList)
/***********************************************************************/
{
#ifdef WIN32
MSG msg;
BOOL fEnabled;

if (lpCmdList &&
	WaitForSingleObject(lpCmdList->ThreadData.ThreadInfo.hCmdEvent, 0) == WAIT_OBJECT_0)
	{
	fEnabled = IsWindowEnabled(PictPubApp.Get_hWndAstral());
	if (fEnabled)
		EnableWindow(PictPubApp.Get_hWndAstral(), FALSE);
	while (WaitForSingleObject(lpCmdList->ThreadData.ThreadInfo.hCmdEvent, 0) == WAIT_OBJECT_0)
		{
		// wait for thread to send it's WM_PROCESSINGDONE
		MsgWaitForMultipleObjects(0, NULL, FALSE, INFINITE, QS_SENDMESSAGE);
		// flush the queue and event should be ready
		PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE);
		}
	if (fEnabled)
		EnableWindow(PictPubApp.Get_hWndAstral(), TRUE);
	}
#endif
}			  

#ifdef WIN32
/***********************************************************************/
BOOL CreateCmdThread(LPCMDLIST lpCmdList)
/***********************************************************************/
{
	lpCmdList->ThreadData.ThreadInfo.hCmdEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!lpCmdList->ThreadData.ThreadInfo.hCmdEvent)
	{
    	Message( IDS_BADTHREADSETUP );
		return(FALSE);
	}
	TRY
	{
		lpCmdList->ThreadData.ThreadInfo.pThread = AfxBeginThread(CmdThread, (LPVOID)lpCmdList, THREAD_PRIORITY_BELOW_NORMAL);
	}
	CATCH_ALL(e)
	{
		lpCmdList->ThreadData.ThreadInfo.pThread = NULL;
	}
	END_CATCH_ALL

	if (!lpCmdList->ThreadData.ThreadInfo.pThread)
	{
    	Message( IDS_BADTHREADSETUP );
		return(FALSE);
	}
	return(TRUE);
}

/***********************************************************************/
void KillCmdThread(LPCMDLIST lpCmdList)
/***********************************************************************/
{

	CWinThread *pThread = lpCmdList->ThreadData.ThreadInfo.pThread;
	HANDLE hThread = pThread->m_hThread;
	lpCmdList->ThreadData.Progress.bEscape = YES;
	FlushCommands(lpCmdList);
	ExecuteCommands(lpCmdList);
	if (!lpCmdList->fInOnProcessingDone)
	{
		DWORD dwExit;
		if (GetExitCodeThread(hThread, &dwExit))
		{
			while (dwExit == STILL_ACTIVE)
			{
				Sleep(10);
				if (!GetExitCodeThread(hThread, &dwExit))
					break;
			}
		}
	}
	CloseHandle(lpCmdList->ThreadData.ThreadInfo.hCmdEvent);
}
#endif

/***********************************************************************/
BOOL SetSaveCommandMode(LPCMDLIST lpCmdList, BOOL fSaveCommands)
/***********************************************************************/
{
#ifdef WIN32
// if we are using threading for this command list see
// if we are already busy and if so return error
if (lpCmdList->ThreadData.ThreadInfo.hCmdEvent)
	{
	if (WaitForSingleObject(lpCmdList->ThreadData.ThreadInfo.hCmdEvent, 0) == WAIT_OBJECT_0)
		return(FALSE);
	}
#endif
lpCmdList->fSaveCommands = fSaveCommands;
if (!lpCmdList->fSaveCommands && !ListIsEmpty(&lpCmdList->UndoPacketList))
	DestroyPacketList(&lpCmdList->UndoPacketList);
return(TRUE);
}

#ifdef WIN32
//***********************************************************************
//	returns the thread data pointer for this thread
//***********************************************************************
LPTHREADDATA GetThreadData()
//***********************************************************************
{
return((LPTHREADDATA)TlsGetValue(Control.ThreadDataTlsIndex));
}
#endif

/***********************************************************************/
BOOL CommandListUndo(LPCMDLIST lpCmdList)
/***********************************************************************/
{
	BOOL		fHadZoom;
	FNAME		szFileName, szDocName, szTitle;
	ITEMID		idFileType;
	LPCMDLIST 	lpNewCmdList;
	CImage 		*pImage;
	CServerDoc 	*pActiveDoc = NULL;
	
	pImage = lpCmdList->ThreadData.lpImage;
	if (!pImage)
		return(FALSE);
	CServerDoc *pDoc = PictPubApp.GetDocument(pImage);
	if (!pDoc)
		return(FALSE);

	lpNewCmdList = CreateCommandList();
	if (!lpNewCmdList)
		return(FALSE);

	// do we have a zoom box which will need to be restored?
    CMDIChildWnd *pActiveWnd = ((CMDIFrameWnd*)PictPubApp.m_pMainWnd)->MDIGetActive();  
	
    if (pActiveWnd) 
    	pActiveDoc = (CServerDoc *)pActiveWnd->GetActiveDocument(); 
	
	fHadZoom = (Window.fHasZoom && hZoomWindow && (pActiveDoc == pDoc));
	
	// save file name and file type of this image
	lstrcpy(szDocName, pDoc->GetPathName());
	lstrcpy(szTitle, pDoc->GetTitle());
	lstrcpy(szFileName, pImage->CurFile);
	
	// FIX - how do we get this to new image?
	idFileType = pImage->FileType;

	lpNewCmdList->PacketList = lpCmdList->UndoPacketList;
	ListInit(&lpCmdList->UndoPacketList);
	lpNewCmdList->RedoPacketList = lpCmdList->RedoPacketList;
	ListInit(&lpCmdList->RedoPacketList);
	lpNewCmdList->fInsertMode = lpCmdList->fInsertMode;
	lpNewCmdList->iInsertIndex = lpCmdList->iInsertIndex;
	lpNewCmdList->dwImageID = lpCmdList->dwImageID;
	lpNewCmdList->PhotoCDResOverride = lpCmdList->PhotoCDResOverride;
	// if this image was opened with override on then we need
	// to adjust the resolution in the load packet to be the
	// same as it was for the original load.  Otherwise, the
	// override will happen again to the new resolution.  This
	// would have the effect of continually reducing or increasing
	// the resolution every time a command list undo is performed.
	if (lpNewCmdList->PhotoCDResOverride)
	{
		LPCMDPKT lpCmdPkt = (LPCMDPKT)ListGetHead(&lpNewCmdList->PacketList);
		if (lpCmdPkt )
		{
			if (lpCmdPkt->idCommand == IDS_CMD_LOADFILE && 
				GetLoadFileType(lpCmdPkt) == IDN_PHOTOCD)
			{
				LPLOADFILE_PARMS lpParms = (LPLOADFILE_PARMS)lpCmdPkt->lpParms;
				LPPCDOPTIONS lpOptions = (LPPCDOPTIONS)lpParms->lpOptions;
				lpOptions->PhotoCDRes -= lpNewCmdList->PhotoCDResOverride;
			}
		}
	}

	// unplugin the lpImage from our document
	pDoc->m_lpImage = NULL;
	// wack the current pImage
	delete lpCmdList->ThreadData.lpImage;

	// attach our document to this new command list
	lpNewCmdList->pDocument = pDoc;
	pDoc->SetCmdList(lpNewCmdList);
	lpNewCmdList->fCmdListEdit = TRUE;
	PlaybackCommands(lpNewCmdList, IDD_CMDLIST_STATUS);

	#ifdef UNUSED // FIX
	pImage = lpCmdList->ThreadData.lpImage;
	// if we successfully created a new image, then
	// set our filename and file type
	if (pImage)
	{
		pImage->fChanged = TRUE;
		CServerDoc *pDoc = PictPubApp.GetDocument(pImage);
		if (pDoc)
		{
			// setup tile etc. (file name needs retoring for fastbits
			pDoc->SetModifiedFlag();
			pDoc->SetPathName(szDocName);
			pDoc->SetTitle(szTitle);
			lstrcpy(pImage->CurFile, szFileName);
			pImage->FileType = idFileType;
			// bring zoom back up
			pActiveWnd = ((CMDIFrameWnd*)PictPubApp.m_pMainWnd)->MDIGetActive();  
		    if (pActiveWnd) 
		    	pActiveDoc = (CServerDoc *)pActiveWnd->GetActiveDocument(); 
			if (fHadZoom && !hZoomWindow && (pActiveDoc == pDoc))
			{
				Window.fHasZoom = TRUE;
		        Window.fHasZoom = HookZoom();
		        if ( Tool.hRibbon )
        			::SendMessage( Tool.hRibbon, WM_DOCACTIVATED, 0, 1L );
			    if ( hWnd = AstralDlgGet(IDD_VIEWING) )
			        ::CheckDlgButton(hWnd, IDC_CUSTOMVIEW, Window.fHasZoom);
			}
		}
	}
	#endif
	return(TRUE);
}

/***********************************************************************/
BOOL CommandListRedo(LPCMDLIST lpCmdList)
/***********************************************************************/
{
lpCmdList->fCmdListEdit = TRUE;
PlaybackCommands(lpCmdList, IDD_CMDLIST_STATUS);
return(TRUE);
}

/***********************************************************************/
BOOL WriteCommandList(LPCMDLIST lpCmdList, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
return(WritePacketList((LPCMDPKT)ListGetHead(&lpCmdList->PacketList), fh));
}

/***********************************************************************/
BOOL WritePacketList(LPCMDPKT lpCmdPkt, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
BOOL fRet = TRUE;
LPSTR lpFormat, lpString;

lpFormat = (LPSTR)Alloc(MAX_CMD_LEN);
lpString = (LPSTR)Alloc(MAX_CMD_LEN);
if (!lpString || !lpFormat)
	{
	Message(IDS_EMEMALLOC);
	if (lpFormat)
		FreeUp(lpFormat);
	if (lpString)
		FreeUp(lpString);
	return(FALSE);
	}
while (lpCmdPkt)
	{
	if (!(fRet = WritePacket(lpCmdPkt, fh, lpFormat, lpString)))
		break;
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	}
FreeUp(lpFormat);
FreeUp(lpString);
return(fRet);
}

/***********************************************************************/
void DestroyCommandList(LPCMDLIST lpCmdList)
/***********************************************************************/
{
	// we must get rid of image's processing thread if
	// this destroy will get rid of the image
	// can't do it while the thread is busy
	#ifdef WIN32
	if (Control.UseThreading && lpCmdList)
	{
		// kill the image's processing thread
		KillCmdThread(lpCmdList);
	}
	#endif

	DestroyPacketList(&lpCmdList->PacketList);
	DestroyPacketList(&lpCmdList->UndoPacketList);
	DestroyPacketList(&lpCmdList->RedoPacketList);
	FreeUp(lpCmdList);
}

/***********************************************************************/
void FreeUpCommandList(LPCMDLIST lpCmdList)
/***********************************************************************/
{
LPCMDPKT lpCmdPkt;
int i, nCount;
BOOL fRetailOnly, fPlayback, fSuccess;
BOOL fSaveInCmdList, fAnythingAddedToUndo;
COMMAND_TYPE Type;

// if we were in playback mode, find out and turn it off
lpCmdList->fCmdListEdit = NO;
if (fPlayback = lpCmdList->fPlayback)
	{
//	if (lpCmdList->ThreadData.lpImage)
//		lpCmdList->ThreadData.lpImage->fDisableUndos = Control.NoUndo;
	lpCmdList->fPlayback = NO;
	}

// Free up command packet list
lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->PacketList);
fAnythingAddedToUndo = FALSE;
while (lpCmdPkt)
	{
	ListUnlink(&lpCmdList->PacketList, lpCmdPkt);
	if ((i = GetCommandIndex(lpCmdPkt->idCommand)) >= 0)
		{
		fRetailOnly = Commands[i].fRetailOnly;
		fSaveInCmdList = Commands[i].fSaveInCmdList;
		Type = Commands[i].Type;
		}
	else
		{
		fRetailOnly = fSaveInCmdList = FALSE;
		Type = CT_EDIT;
		}

	// when in playback mode always save commands
	fSuccess = lpCmdPkt->fSuccess;
	if (fPlayback && !fSuccess)
	{
		fSuccess = TRUE;
		lpCmdPkt->fDisabled = TRUE;
	}

	// link the command packet into the save list
	// If and only if:
	// successful and
	// we are saving commands and
	// this is a type of command that should be saved and
	// this is not a load command and our save list is not empty or
	// this is a load and our save list is empty
	nCount = ListGetCount(&lpCmdList->UndoPacketList);
	if (fSuccess && fSaveInCmdList &&
		lpCmdList->fSaveCommands &&
		(!Control.NTBeta || !fRetailOnly) &&
		( (Type != CT_LOAD && nCount) ||
		  (Type == CT_LOAD && !nCount) ))
		{
		// link in the command packet to the list
		ListAddTail(&lpCmdList->UndoPacketList, lpCmdPkt);
		fAnythingAddedToUndo = TRUE;
		}
	else
		{
		FreeUpPacket(lpCmdPkt);
		}
	lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->PacketList);
	}
// wack the redo packet list
// in insert mode the redo list is used to hold packets after
// the insert point so don't wack it
if (!fPlayback && !lpCmdList->fInsertMode && fAnythingAddedToUndo)
	DestroyPacketList(&lpCmdList->RedoPacketList);
}

/***********************************************************************/
BOOL CopyPacketList(LPLIST lpSrcList, LPLIST lpDstList)
/***********************************************************************/
{
	return(CopyPacketList((LPCMDPKT)ListGetHead(lpSrcList), lpDstList));
}

/***********************************************************************/
BOOL CopyPacketList(LPCMDPKT lpCmdPkt, LPLIST lpDstList)
/***********************************************************************/
{
	LPCMDPKT lpNewPkt;

	ListInit(lpDstList);
	while (lpCmdPkt)
 	{
		lpNewPkt = CopyPacket(lpCmdPkt);
		if (!lpNewPkt)
 		{
			Message(IDS_EMEMALLOC);
			return(FALSE);
		}
		ListAddTail(lpDstList, lpNewPkt);
		lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	}
	return(TRUE);
}

/***********************************************************************/
void DestroyPacketList(LPLIST lpPktList)
/***********************************************************************/
{
LPCMDPKT lpCmdPkt;

lpCmdPkt = (LPCMDPKT)ListGetHead(lpPktList);
while (lpCmdPkt)
	{
	ListUnlink(lpPktList, lpCmdPkt);
	FreeUpPacket(lpCmdPkt);
	lpCmdPkt = (LPCMDPKT)ListGetHead(lpPktList);
	}
}

/***********************************************************************/
void FreeUpPacket(LPCMDPKT lpCmdPkt)
/***********************************************************************/
{
if (lpCmdPkt->lpParms)
	FreeUpParms(lpCmdPkt->idCommand, lpCmdPkt->lpParms);
FreeUp(lpCmdPkt);
}

/***********************************************************************/
LPVOID GetLoadCommandParms(LPCMDPKT lpCmdPkt, ITEMID idCommand)
/***********************************************************************/
{
return(lpCmdPkt->lpParms);
}

/***********************************************************************/
int GetCommandIndex(ITEMID idCommand)
/***********************************************************************/
{
// See if command is within range
if (idCommand < IDS_CMD_FIRST || idCommand > IDS_CMD_LAST)
	return(-1);
return(idCommand-IDS_CMD_FIRST);
}

/***********************************************************************/
ITEMID GetCommandId(int index)
/***********************************************************************/
{
return(index+IDS_CMD_FIRST);
}

/***********************************************************************/
COMMAND_TYPE GetCommandType(ITEMID idCommand)
/***********************************************************************/
{
int i;

if ((i = GetCommandIndex(idCommand)) < 0)
	return(CT_NONE);
return(Commands[i].Type);
}

/***********************************************************************/
size_t GetCommandParmsSize(ITEMID idCommand)
/***********************************************************************/
{
int i;

if ((i = GetCommandIndex(idCommand)) < 0)
	return(0);
return(Commands[i].SizeOfParms);
}

/***********************************************************************/
BOOL GetCommandSequence(ITEMID idCommand)
/***********************************************************************/
{
int i;

if ((i = GetCommandIndex(idCommand)) < 0)
	return(FALSE);
return(Commands[i].fSequence);
}

/***********************************************************************/
BOOL CheckForRebuild(LPCMDPKT lpCmdPkt, int iResWanted, int iResCurrent,
					int PhotoCDResOverride)
/***********************************************************************/
{
	// is requested res greater than current res
	// iResWanted == 0 means that the image is full res
	if (lpCmdPkt->idCommand == IDS_CMD_LOWRESLOAD)
	{
		if (Control.bNoHiResRebuild)
			return(FALSE);
		else
			return(!iResWanted || (iResWanted > iResCurrent));
	}
	else
	if (lpCmdPkt->idCommand == IDS_CMD_LOADFILE && PhotoCDResOverride)
	{
		LPLOADFILE_PARMS lpParms = (LPLOADFILE_PARMS)lpCmdPkt->lpParms;
		if (GetPPFileType(lpParms->szFileName) == IDN_PHOTOCD)
		{
			LPPCDOPTIONS lpOptions = (LPPCDOPTIONS)lpParms->lpOptions;
			return(DoPCDResOverride(lpOptions->PhotoCDRes, PhotoCDResOverride));
		}
	}
	return(FALSE);
}

/***********************************************************************/
BOOL IsVariableResLoadCmd(LPCMDPKT lpCmdPkt)
/***********************************************************************/
{
if (lpCmdPkt)
	{
	if (lpCmdPkt->idCommand == IDS_CMD_LOADFILE)
		{
		LPLOADFILE_PARMS lpParms = (LPLOADFILE_PARMS)lpCmdPkt->lpParms;
		return(GetPPFileType(lpParms->szFileName) == IDN_PHOTOCD);
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_LOWRESLOAD)
		{
		return(TRUE);
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_FASTBITS)
		{
		return(FALSE);
		}
	}
return(FALSE);
}

/***********************************************************************/
LPSTR GetLoadFileName(LPCMDPKT lpCmdPkt)
/***********************************************************************/
{
if (lpCmdPkt)
	{
	if (lpCmdPkt->idCommand == IDS_CMD_LOADFILE)
		{
		LPLOADFILE_PARMS lpParms = (LPLOADFILE_PARMS)lpCmdPkt->lpParms;
		return(lpParms->szFileName);
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_LOWRESLOAD)
		{
		LPLOWRESLOAD_PARMS lpParms = (LPLOWRESLOAD_PARMS)lpCmdPkt->lpParms;
		return(lpParms->szFileName);
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_FASTBITS)
		{
		LPFASTBITS_PARMS lpParms = (LPFASTBITS_PARMS)lpCmdPkt->lpParms;
		return(lpParms->szFileName);
		}
	}
return(NULL);
}

/***********************************************************************/
LPSTR GetLoadVolumeID(LPCMDPKT lpCmdPkt)
/***********************************************************************/
{
if (lpCmdPkt)
	{
	if (lpCmdPkt->idCommand == IDS_CMD_LOADFILE)
		{
		LPLOADFILE_PARMS lpParms = (LPLOADFILE_PARMS)lpCmdPkt->lpParms;
		return(lpParms->szVolumeID);
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_LOWRESLOAD)
		{
		LPLOWRESLOAD_PARMS lpParms = (LPLOWRESLOAD_PARMS)lpCmdPkt->lpParms;
		return(lpParms->szVolumeID);
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_FASTBITS)
		{
		LPFASTBITS_PARMS lpParms = (LPFASTBITS_PARMS)lpCmdPkt->lpParms;
		return(lpParms->szVolumeID);
		}
	}
return(NULL);
}

/***********************************************************************/
LPVOID GetLoadOptions(LPCMDPKT lpCmdPkt)
/***********************************************************************/
{
if (lpCmdPkt)
	{
	if (lpCmdPkt->idCommand == IDS_CMD_LOADFILE)
		{
		LPLOADFILE_PARMS lpParms = (LPLOADFILE_PARMS)lpCmdPkt->lpParms;
		return(lpParms->lpOptions);
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_LOWRESLOAD)
		{
		LPLOWRESLOAD_PARMS lpParms = (LPLOWRESLOAD_PARMS)lpCmdPkt->lpParms;
		return(lpParms->lpOptions);
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_FASTBITS)
		{
		LPFASTBITS_PARMS lpParms = (LPFASTBITS_PARMS)lpCmdPkt->lpParms;
		return(NULL);
		}
	}
return(NULL);
}

/***********************************************************************/
LPCMSINFO GetLoadCMSInfo(LPCMDPKT lpCmdPkt)
/***********************************************************************/
{
if (lpCmdPkt)
	{
	if (lpCmdPkt->idCommand == IDS_CMD_LOADFILE)
		{
		LPLOADFILE_PARMS lpParms = (LPLOADFILE_PARMS)lpCmdPkt->lpParms;
		return(&lpParms->cmsInfo);
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_LOWRESLOAD)
		{
		LPLOWRESLOAD_PARMS lpParms = (LPLOWRESLOAD_PARMS)lpCmdPkt->lpParms;
		return(&lpParms->cmsInfo);
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_FASTBITS)
		{
		LPFASTBITS_PARMS lpParms = (LPFASTBITS_PARMS)lpCmdPkt->lpParms;
		return(&lpParms->cmsInfo);
		}
	}
return(NULL);
}

/***********************************************************************/
void SetLoadCMSInfo(LPCMDPKT lpCmdPkt, LPCMSINFO lpCmsInfo)
/***********************************************************************/
{
if (lpCmdPkt)
	{
	if (lpCmdPkt->idCommand == IDS_CMD_LOADFILE)
		{
		LPLOADFILE_PARMS lpParms = (LPLOADFILE_PARMS)lpCmdPkt->lpParms;
		lpParms->cmsInfo = *lpCmsInfo;
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_LOWRESLOAD)
		{
		LPLOWRESLOAD_PARMS lpParms = (LPLOWRESLOAD_PARMS)lpCmdPkt->lpParms;
		lpParms->cmsInfo = *lpCmsInfo;
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_FASTBITS)
		{
		LPFASTBITS_PARMS lpParms = (LPFASTBITS_PARMS)lpCmdPkt->lpParms;
		lpParms->cmsInfo = *lpCmsInfo;
		}
	}
}

/***********************************************************************/
LOCAL void SetLoadOptions(LPCMDPKT lpCmdPkt, LPVOID lpOptions)
/***********************************************************************/
{
if (lpCmdPkt)
	{
	if (lpCmdPkt->idCommand == IDS_CMD_LOADFILE)
		{
		LPLOADFILE_PARMS lpParms = (LPLOADFILE_PARMS)lpCmdPkt->lpParms;
		lpParms->lpOptions = lpOptions;
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_LOWRESLOAD)
		{
		LPLOWRESLOAD_PARMS lpParms = (LPLOWRESLOAD_PARMS)lpCmdPkt->lpParms;
		lpParms->lpOptions = lpOptions;
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_FASTBITS)
		{
		LPFASTBITS_PARMS lpParms = (LPFASTBITS_PARMS)lpCmdPkt->lpParms;
		}
	}
}

/***********************************************************************/
ITEMID GetLoadFileType(LPCMDPKT lpCmdPkt)
/***********************************************************************/
{
if (lpCmdPkt)
	{
	if (lpCmdPkt->idCommand == IDS_CMD_LOADFILE)
		{
		LPLOADFILE_PARMS lpParms = (LPLOADFILE_PARMS)lpCmdPkt->lpParms;
		return(lpParms->idFileType);
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_LOWRESLOAD)
		{
		LPLOWRESLOAD_PARMS lpParms = (LPLOWRESLOAD_PARMS)lpCmdPkt->lpParms;
		return(lpParms->idFileType);
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_FASTBITS)
		{
//		LPFASTBITS_PARMS lpParms = (LPFASTBITS_PARMS)lpCmdPkt->lpParms;
		return(IDN_TIFF);
		}
	}
return(NULL);
}

/***********************************************************************/
void SetLoadFileName(LPCMDPKT lpCmdPkt, LPSTR lpFileName, ITEMID idFileType)
/***********************************************************************/
{
if (lpCmdPkt)
	{
	if (lpCmdPkt->idCommand == IDS_CMD_LOADFILE)
		{
		LPLOADFILE_PARMS lpParms = (LPLOADFILE_PARMS)lpCmdPkt->lpParms;
		lstrcpy(lpParms->szFileName, lpFileName);
		lpParms->idFileType = idFileType;
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_LOWRESLOAD)
		{
		LPLOWRESLOAD_PARMS lpParms = (LPLOWRESLOAD_PARMS)lpCmdPkt->lpParms;
		lstrcpy(lpParms->szFileName, lpFileName);
		lpParms->idFileType = idFileType;
		}
	else if (lpCmdPkt->idCommand == IDS_CMD_FASTBITS)
		{
		LPFASTBITS_PARMS lpParms = (LPFASTBITS_PARMS)lpCmdPkt->lpParms;
		lstrcpy(lpParms->szFileName, lpFileName);
		}
	}
}

/***********************************************************************/
BOOL WritePacket(LPCMDPKT lpCmdPkt, MACRO_FILE_HANDLE fh,
						LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
int i;
WRITE_FUNC func;

// never write out disabled packets
if (lpCmdPkt->fDisabled)
	return(TRUE);

if ((i = GetCommandIndex(lpCmdPkt->idCommand)) < 0)
	return(FALSE);
if (!AstralStrEx(lpCmdPkt->idCommand, lpFormat, MAX_CMD_LEN))
	return(FALSE);
if (Commands[i].lpWriteParmsProc)
	{
	if (fh != MACRO_FILE_HANDLE_INVALID && lpCmdPkt->lpParms)
		((LPCMD_PARMS)lpCmdPkt->lpParms)->Common.fParmsStored = TRUE;
	func = (*Commands[i].lpWriteParmsProc)(lpCmdPkt->lpParms, fh,
											lpFormat, lpString);

	}
else
	{
	func = WF_WRITE;
	lpString = lpFormat;
	}
if (func == WF_WRITE)
	{
	if (!WriteString(fh, lpString))
		return(FALSE);
	return(TRUE);
	}
else
	return(func != WF_ERROR);
}

/***********************************************************************/
LPCMDPKT ReadPacket(MACRO_FILE_HANDLE fh, LPSTR lpCommand, LPBOOL lpError)
/***********************************************************************/
{
LPVOID lpParms;
int i = -1;
LPCMDPKT lpCmdPkt;
ITEMID idCommand;

*lpError = FALSE;
while (i < 0)
	{
	if (!ReadLine(fh, lpCommand, MAX_CMD_LEN))
		return(NULL);
	i = ReadParms(fh, lpCommand, &lpParms);
	}
idCommand = GetCommandId(i);
lpCmdPkt = CreatePacket(idCommand, lpParms, NO);
if (!lpCmdPkt)
	{
	*lpError = TRUE;
	FreeUpParms(idCommand, lpParms);
	return(NULL);
	}
return(lpCmdPkt);
}

/***********************************************************************/
int ReadParms(MACRO_FILE_HANDLE fh, LPSTR lpCommand, LPPVOID lppParms)
/***********************************************************************/
{
char szFunc[256];
char szCmdFunc[256];
static STRING szSmear, szPaint;
int nCommands, i = -1;

*lppParms = NULL;
GetCommandName(lpCommand, szCmdFunc);

if (!szPaint[0])
	{
	// special case to see if it is a paint command
	AstralStrEx(IDS_CMD_PAINT, szPaint, sizeof(szPaint));
	AstralStrEx(IDS_CMD_SMEAR, szSmear, sizeof(szSmear));

	GetCommandName(szPaint, NULL);
	GetCommandName(szSmear, NULL);
	}

if (StringsEqual(szCmdFunc, szPaint))
	i = GetCommandIndex(IDS_CMD_PAINT);
else
if (StringsEqual(szCmdFunc, szSmear))
	i = GetCommandIndex(IDS_CMD_SMEAR);

if (i < 0)
	{
	nCommands = sizeof(Commands) / sizeof(CMDDESC);
	for (i = 0; i < nCommands; ++i)
		{
		if (!AstralStrEx(GetCommandId(i), szFunc, sizeof(szFunc)))
			continue;
		GetCommandName(szFunc, NULL);
		if (StringsEqual(szCmdFunc, szFunc))
			break;
		}
	if (i >= nCommands)
		return(-1);
	}


if (Commands[i].lpGetParmsProc)
	{
	*lppParms = (*Commands[i].lpGetParmsProc)(NULL, lpCommand, NULL, fh);
	if (!(*lppParms))
		{
		Message(IDS_EMEMALLOC);
		return(-1);
		}
	}
return(i);
}

#ifdef WIN32
/***********************************************************************/
UINT CmdThread(LPVOID lpParam)
/***********************************************************************/
{
LPCMDLIST lpCmdList;
DWORD dwWaitResult;
BOOL fDone = NO;

if (!SetupBrushBuffers(YES))
    {
    Message( IDS_EMEMALLOC );
    return(0);
    }
lpCmdList = (LPCMDLIST)lpParam;

// Thread specific Initialization for Color Management
CmsThreadInit();

// attach input state of this thread to the main app thread input state
AttachThreadInput(lpCmdList->ThreadData.ThreadInfo.pThread->m_nThreadID,
		GetWindowThreadProcessId(PictPubApp.Get_hWndAstral(), NULL), TRUE);

// Initialize the progress data 
ProgressInit(&lpCmdList->ThreadData.Progress);
CursorDataInit(&lpCmdList->ThreadData.CursorData);

// setup thread local storage of thread data
TlsSetValue(Control.ThreadDataTlsIndex, &lpCmdList->ThreadData);

while (!fDone)
	{
	// wait for command event that tells us to process lpCmdList
	dwWaitResult = WaitForSingleObject(
			lpCmdList->ThreadData.ThreadInfo.hCmdEvent,
			INFINITE);

	switch (dwWaitResult)
		{
		case WAIT_OBJECT_0:
			// check for command existence
			if (!ListIsEmpty(&lpCmdList->PacketList))
				{
				// dispatch commands in command list
				DispatchCommands(lpCmdList);
				// indicate we are no longer busy
				ResetEvent(lpCmdList->ThreadData.ThreadInfo.hCmdEvent);
				// tell main process we are done
				fDone = FORWARD_WM_PROCESSINGDONE(PictPubApp.Get_hWndAstral(FALSE), lpCmdList, SendMessage);
				}
			else
				{
				// indicate we are no longer busy
				ResetEvent(lpCmdList->ThreadData.ThreadInfo.hCmdEvent);
				// caller told us to process a command when none
				// exist, this is the signal (or hack) to terminate
				fDone = YES;
				}
			break;
		default:
            Message( IDS_WAITRESULT, dwWaitResult );
            break;
		}
	}

// Thread specific Termination for Color Management
CmsThreadTerm();

SetupBrushBuffers(NO);
return(0);
}
#endif

/***********************************************************************/
LPSTR GetCommandName(LPSTR lpCommand, LPSTR lpName)
/***********************************************************************/
{
int i;
LPSTR lpFunc, lp;

if (lp = lStrChr(lpCommand, ' '))
	{
	if (lpName)
		{
		i = 0;
		lpFunc = lpCommand;
		while (lpFunc != lp)
			{
			lpName[i] = *lpFunc++;
			++i;
			}
		lpName[i] = '\0';
		}
	else
		*lp = '\0';
	}
else
if (lpName)
	lstrcpy(lpName, lpCommand);
if (lpName)
	return(lpName);
else
	return(lpCommand);
}

/***********************************************************************/
BOOL GetCommandString(LPCMDPKT lpCmdPkt, LPSTR lpCommand, int iMaxStrLen,
						BOOL fNameOnly)
/***********************************************************************/
{
LPSTR lpFormat, lpString, lp;
int i;
WRITE_FUNC func;
BOOL fRet = FALSE;

*lpCommand = '\0';
lpFormat = (LPSTR)Alloc(MAX_CMD_LEN);
lpString = (LPSTR)Alloc(MAX_CMD_LEN);
if (!lpFormat || !lpString)
	{
	Message(IDS_EMEMALLOC);
	goto Exit;
	}

if ((i = GetCommandIndex(lpCmdPkt->idCommand)) < 0)
	goto Exit;
if (!AstralStrEx(lpCmdPkt->idCommand, lpFormat, MAX_CMD_LEN))
	goto Exit;

if (fNameOnly)
	{
	GetCommandName(lpFormat, lpCommand);
	fRet = TRUE;
	goto Exit;
	}

if (Commands[i].lpWriteParmsProc)
	{
	func = (*Commands[i].lpWriteParmsProc)(lpCmdPkt->lpParms, MACRO_FILE_HANDLE_INVALID,
											lpFormat, lpString);
	if (func == WF_ERROR)
		goto Exit;
	lp = lpString;
	}
else
	lp = lpFormat;

// zap the last character so we have a null terminator
lpCommand[iMaxStrLen-1] = '\0';
lStrCpyN( lpCommand, lp, iMaxStrLen-1 );
fRet = TRUE;

Exit:
if (lpFormat)
	FreeUp(lpFormat);
if (lpString)
	FreeUp(lpString);
return(fRet);
}

/***********************************************************************/
void FreeUpParms(ITEMID idCommand, LPVOID lpParms)
/***********************************************************************/
{
int i;
LONG lUsageCount;
LPCMD_PARMS lpCmdParms;

if (((i = GetCommandIndex(idCommand)) >= 0) && lpParms)
	{
	lpCmdParms = (LPCMD_PARMS)lpParms;
#ifdef WIN32
	lUsageCount = InterlockedDecrement(&lpCmdParms->Common.lUsageCount);
#else
	--lpCmdParms->Common.lUsageCount;
	lUsageCount = lpCmdParms->Common.lUsageCount;
#endif
	if (lUsageCount <= 0)
		{
		if (Commands[i].lpFreeParmsProc)
			(*Commands[i].lpFreeParmsProc)(lpParms);
		FreeUp(lpParms);
		}
	}
}

static LPCMDNOTIFYPROC lpCmdNotifyProc = NULL;
static LPABORTPROC lpAbortProc = NULL;
static BOOL fLogCommands = FALSE;
static CFile LogFile;

/***********************************************************************/
BOOL EnableLogging(BOOL fEnable)
/***********************************************************************/
{
	FNAME szFileName;

	fLogCommands = fEnable;
	TRY
	{
		if (fEnable)
		{
			lstrcpy(szFileName, Control.PouchPath);
			lstrcat(szFileName, "BATCH.LOG");
			if (!LogFile.Open(szFileName, CFile::modeCreate|CFile::modeWrite))
			{
				Message(IDS_EOPEN, (LPSTR)szFileName);
				return(FALSE);
			}
			SetMessageLogging(&LogFile);
		}
		else
		{
			LogFile.Close();
			SetMessageLogging(NULL);
		}
		return(TRUE);
	}
	CATCH_ALL(e)
	{
		return(FALSE);
	}
	END_CATCH_ALL
}

/***********************************************************************/
LPCMDNOTIFYPROC SetCmdNotifyProc(LPCMDNOTIFYPROC lpNotifyProc)
/***********************************************************************/
{
LPCMDNOTIFYPROC lpOldProc;

lpOldProc = lpCmdNotifyProc;
lpCmdNotifyProc = lpNotifyProc;
return(lpOldProc);
}

/***********************************************************************/
LPABORTPROC SetCmdAbortProc(LPABORTPROC lpProc)
/***********************************************************************/
{
LPABORTPROC lpOldProc;

lpOldProc = lpAbortProc;
lpAbortProc = lpProc;
return(lpOldProc);
}

/***********************************************************************/
LOCAL BOOL DispatchCommands(LPCMDLIST lpCmdList)
/***********************************************************************/
{
BOOL fRet;

fRet = DispatchPacketList(lpCmdList, &lpCmdList->PacketList, TRUE);
// copy packets from PacketList to UndoPacketList
FreeUpCommandList(lpCmdList);
return(fRet);
}

/***********************************************************************/
BOOL DispatchPacketList(LPCMDLIST lpCmdList, LPLIST lpPacketList,
							BOOL fUpdateReset)
/***********************************************************************/
{
	LPCMDPKT lpCmdPkt, lpNext;
	int i, nCount = 0;
	COMMAND_TYPE Type;
	LPCMD_PARMS lpParms;
	LIST PacketList;
	STRING szName;
	BOOL fSuccess = TRUE, fDisabled, fProgress = FALSE;

	lpCmdPkt = (LPCMDPKT)ListGetHead(lpPacketList);
	while (lpCmdPkt)
	{
		if ((i = GetCommandIndex(lpCmdPkt->idCommand)) >= 0)
		{
			if (Commands[i].fHasProgress)
				++nCount;
		}
		lpCmdPkt->fSuccess = FALSE;
		lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	}
	if (lpCmdList->fPlayback && nCount)
	{
		ProgressBegin(nCount, PROGRESS_ID(IDS_UNDOREBUILD)); 
		fProgress = TRUE;
	}

	lpCmdPkt = (LPCMDPKT)ListGetHead(lpPacketList);
	if (fUpdateReset)
	{
		lpCmdList->UpdateType = UT_NONE;
		AstralSetRectEmpty(&lpCmdList->rUpdateArea);
		ListInit(&lpCmdList->NewImageList);
	}
	ListInit(&PacketList);
	ListInit(&lpCmdList->LoadPacketList);
	while (lpCmdPkt && fSuccess)
	{
		if (lpAbortProc && (*lpAbortProc)())
			break;

		// unlink the current command and save
		ListUnlink(lpPacketList, lpCmdPkt);
		ListAddTail(&PacketList, lpCmdPkt);

		// if packet is disabled, skip over it
		if (lpCmdPkt->fDisabled)
 		{
			lpCmdPkt->fSuccess = TRUE;
			// if a notify proc is plugged in, call it with the
			// packet we are about to skip
			if (lpCmdNotifyProc)
				(*lpCmdNotifyProc)(lpCmdPkt);
			lpCmdPkt = (LPCMDPKT)ListGetHead(lpPacketList);
			continue;
 		}
		if ((i = GetCommandIndex(lpCmdPkt->idCommand)) < 0)
 		{
			fSuccess = FALSE;
			Message(IDS_CMDOUTOFRANGE, lpCmdPkt->idCommand);
			break;
 		}
		lpParms = (LPCMD_PARMS)lpCmdPkt->lpParms;
		if (lpParms)
 		{
			lpParms = (LPCMD_PARMS)(*Commands[i].lpGetParmsProc)(lpCmdList, NULL, lpParms, MACRO_FILE_HANDLE_INVALID);
			// start with success, so only functions that cause an
			// error have to do something extra, mostly to save coding changes
			lpParms->Common.StatusCode = SC_SUCCESS;
			lpParms->Common.idDirty = Commands[i].idDirty;
			lpParms->Common.UpdateType = UT_NONE;
			AstralSetRectEmpty(&lpParms->Common.rUpdateArea);
			ListInit(&lpParms->Common.NewImageList);
 		}

		// FIX - re-entrancy problem
		if (fLogCommands)
 		{
			GetCommandString(lpCmdPkt, szName, sizeof(szName), Control.UseShortCommandNames);
			LogMessage(&LogFile, szName);
 		}
		Type = GetCommandType(lpCmdPkt->idCommand);
		// only execute command if it is a load command or
		// we have have an image to work on
		fSuccess = fDisabled = FALSE;
		if (Type == CT_LOAD || lpCmdList->ThreadData.lpImage)
 		{
			//
			// image gets plugged into ThreadData in CImage contructor
			//
			if (lpCmdList->ThreadData.lpImage)
				fDisabled = !IsCmdEnabled(lpCmdPkt->idCommand, NULL, lpParms, FALSE, lpCmdList->ThreadData.lpImage);

			if (!fDisabled)
				fSuccess = (*Commands[i].lpCmdProc)(lpCmdList->ThreadData.lpImage, lpParms);
 		}
		lpCmdPkt->fSuccess = fSuccess;
		lpCmdList->UpdateType |= lpParms->Common.UpdateType;
		if (lpParms->Common.UpdateType & (UT_AREA|UT_ACTIVEAREA))
 		{
			AstralUnionRect(&lpCmdList->rUpdateArea,
							&lpCmdList->rUpdateArea,
							&lpParms->Common.rUpdateArea);
 		}
		if (lpParms->Common.UpdateType & UT_NEWIMAGE)
 		{
			LPNEWIMAGEINFO lpInfo;

			lpInfo = (LPNEWIMAGEINFO)ListGetHead(&lpParms->Common.NewImageList);
			while (lpInfo)
 			{
				ListUnlink(&lpParms->Common.NewImageList, lpInfo);
				ListAddTail(&lpCmdList->NewImageList, lpInfo);
				lpInfo = (LPNEWIMAGEINFO)ListGetHead(&lpParms->Common.NewImageList);
 			}
 		}
		// if a notify proc is plugged in, call it with the
		// packet we are about to process
		if (lpCmdNotifyProc)
			(*lpCmdNotifyProc)(lpCmdPkt);

		// see if the command that was just processed produced some
		// packet list information
		if (ListGetCount(&lpCmdList->LoadPacketList))
			MergeLoadPacketList(&PacketList, &lpCmdList->LoadPacketList);

		lpNext = (LPCMDPKT)ListGetHead(lpPacketList);

		// if last command was unsuccessful and we are in playback mode
		// and we have an image, then ask the user if he wants to continue
		if (!fSuccess && lpCmdList->fPlayback && lpCmdList->ThreadData.lpImage)
 		{
			// make sure the user hasn't cancelled before you ask
			// them if they want to continue, obviously they don't
			if (!lpAbortProc || !(*lpAbortProc)())
 			{
				ITEMID idMsg;

				if (fDisabled)
					idMsg = IDS_CMDDISABLED;
				else
					idMsg = IDS_CMDLISTFAILURE;
	  			GetCommandString(lpCmdPkt, szName, sizeof(szName), Control.UseShortCommandNames);
	  			if (AstralAffirm(idMsg, (LPSTR)szName))
	  				fSuccess = TRUE;
 			}
 		}
		lpCmdPkt = lpNext;
 	}

	// make sure any remainings commands get moved to PacketList
	// so they don't get lost
	while (lpCmdPkt = (LPCMDPKT)ListGetHead(lpPacketList))
 	{	
		// add any remaining commands to the returned packet list
		// this can happen on an abort or failure, you never want
		// to lose your command list
		ListUnlink(lpPacketList, lpCmdPkt);
		ListAddTail(&PacketList, lpCmdPkt);
 	}
	*lpPacketList = PacketList;
	if (fProgress)
		ProgressEnd();

	return(fSuccess);
}

/***********************************************************************/
LOCAL void MergeLoadPacketList(LPLIST lpPacketList, LPLIST lpLoadPacketList)
/***********************************************************************/
{
	LPCMDPKT lpNext, lpFileLoad, lpCurrentLoad;

	// the load command in the load packet list (which is the packet
	// list read in from the PP5) needs
	// to get converted to the type of the load command in
	// the current packet list. 

	// Get the load command from the load packet list for the PP5
	lpFileLoad = (LPCMDPKT)ListGetHead(lpLoadPacketList);
	if (!lpFileLoad)
		return;

	// the current load command should be the last command in
	// the current packet list
	lpCurrentLoad = (LPCMDPKT)ListGetTail(lpPacketList);

	// make sure they are both load commands
	if (lpCurrentLoad &&
		(GetCommandType(lpFileLoad->idCommand) == CT_LOAD) &&
		(GetCommandType(lpCurrentLoad->idCommand) == CT_LOAD) )
	{
		if (lpCurrentLoad->idCommand == IDS_CMD_LOADFILE)
		{
			LPLOADFILE_PARMS lpCurrentLoadParms = (LPLOADFILE_PARMS)lpCurrentLoad->lpParms;
			if (lpFileLoad->idCommand == IDS_CMD_LOWRESLOAD)
			{
				LPLOWRESLOAD_PARMS lpFileLoadParms = (LPLOWRESLOAD_PARMS)lpFileLoad->lpParms;

				lstrcpy(lpCurrentLoadParms->szFileName, lpFileLoadParms->szFileName);
				lstrcpy(lpCurrentLoadParms->szVolumeID, lpFileLoadParms->szVolumeID);
				lpCurrentLoadParms->idFileType = lpFileLoadParms->idFileType;
				// the packet from the command list should always have
				// the correct cmsInfo
				lpCurrentLoadParms->cmsInfo = lpFileLoadParms->cmsInfo;
				lpCurrentLoadParms->lpOptions = lpFileLoadParms->lpOptions;
				lpFileLoadParms->lpOptions = NULL;
			}
			else
			if (lpFileLoad->idCommand == IDS_CMD_LOADFILE)
			{
				LPLOADFILE_PARMS lpFileLoadParms = (LPLOADFILE_PARMS)lpFileLoad->lpParms;

				lstrcpy(lpCurrentLoadParms->szFileName, lpFileLoadParms->szFileName);
				lstrcpy(lpCurrentLoadParms->szVolumeID, lpFileLoadParms->szVolumeID);
				lpCurrentLoadParms->idFileType = lpFileLoadParms->idFileType;
				// the packet from the command list should always have
				// the correct cmsInfo
				lpCurrentLoadParms->cmsInfo = lpFileLoadParms->cmsInfo;
				lpCurrentLoadParms->lpOptions = lpFileLoadParms->lpOptions;
				lpFileLoadParms->lpOptions = NULL;
			}

		}
		else
		if (lpCurrentLoad->idCommand == IDS_CMD_LOWRESLOAD)
		{
			LPLOWRESLOAD_PARMS lpCurrentLoadParms = (LPLOWRESLOAD_PARMS)lpCurrentLoad->lpParms;
			if (lpFileLoad->idCommand == IDS_CMD_LOADFILE)
			{
				LPLOADFILE_PARMS lpFileLoadParms = (LPLOADFILE_PARMS)lpFileLoad->lpParms;

				lstrcpy(lpCurrentLoadParms->szFileName, lpFileLoadParms->szFileName);
				lstrcpy(lpCurrentLoadParms->szVolumeID, lpFileLoadParms->szVolumeID);
				lpCurrentLoadParms->idFileType = lpFileLoadParms->idFileType;
				// the packet from the command list should always have
				// the correct cmsInfo
				lpCurrentLoadParms->cmsInfo = lpFileLoadParms->cmsInfo;
				lpCurrentLoadParms->lpOptions = lpFileLoadParms->lpOptions;
				lpFileLoadParms->lpOptions = NULL;
			}
			else
			if (lpFileLoad->idCommand == IDS_CMD_LOWRESLOAD)
			{
				LPLOWRESLOAD_PARMS lpFileLoadParms = (LPLOWRESLOAD_PARMS)lpFileLoad->lpParms;

				lstrcpy(lpCurrentLoadParms->szFileName, lpFileLoadParms->szFileName);
				lstrcpy(lpCurrentLoadParms->szVolumeID, lpFileLoadParms->szVolumeID);
				lpCurrentLoadParms->idFileType = lpFileLoadParms->idFileType;
				// the packet from the command list should always have
				// the correct cmsInfo
				lpCurrentLoadParms->cmsInfo = lpFileLoadParms->cmsInfo;
				lpCurrentLoadParms->lpOptions = lpFileLoadParms->lpOptions;
				lpFileLoadParms->lpOptions = NULL;
			}
		}
		ListUnlink(lpLoadPacketList, lpFileLoad);
		FreeUpPacket(lpFileLoad);
	}

	lpNext = (LPCMDPKT)ListGetHead(lpLoadPacketList);
	while (lpNext)
	{
		ListUnlink(lpLoadPacketList, lpNext);
		ListAddTail(lpPacketList, lpNext);
		lpNext = (LPCMDPKT)ListGetHead(lpLoadPacketList);
	}
}

/***********************************************************************/
LOCAL BOOL ExecuteCommandsEx(LPCMDLIST lpCmdList, BOOL fThisThread)
/***********************************************************************/
{
BOOL fRet;

if (lpCmdList)
	{
	if (!ListIsEmpty(&lpCmdList->PacketList))
		{
		CollapseCommandList(lpCmdList);
		if (ListIsEmpty(&lpCmdList->PacketList))
			return(TRUE);
		}
	if (!lpCmdList->ThreadData.ThreadInfo.hCmdEvent)
		fThisThread = YES;
	if (fThisThread)
		{
		// dispatch the command directly
		fRet = DispatchCommands(lpCmdList);
		// tell main process we are done
		FORWARD_WM_PROCESSINGDONE(PictPubApp.Get_hWndAstral(FALSE), lpCmdList,
								SendMessage);
		return(fRet);
		}
	else
		{
		SetEvent(lpCmdList->ThreadData.ThreadInfo.hCmdEvent);
		return(TRUE);
		}
	}
else
	return(FALSE);
}

/***********************************************************************/
LOCAL void CollapseCommandList(LPCMDLIST lpCmdList)
/***********************************************************************/
{
LPCMDPKT lpCmdPkt, lpNext;
COMMAND_TYPE Type, NextType;

// pass through the list getting rid of undo pairs
lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->PacketList);
while (lpCmdPkt)
	{
	lpNext = (LPCMDPKT)ListGetNext(lpCmdPkt);
	if (!lpNext)
		break;

	Type = GetCommandType(lpCmdPkt->idCommand);
	NextType = GetCommandType(lpNext->idCommand);
	if ((Type == CT_EDITUNDO && NextType == CT_EDITUNDO) ||
		(Type == CT_MASKUNDO && NextType == CT_MASKUNDO) )
		{
		ListUnlink(&lpCmdList->PacketList, lpCmdPkt);
		FreeUpPacket(lpCmdPkt);
		lpCmdPkt = (LPCMDPKT)ListGetNext(lpNext);
		ListUnlink(&lpCmdList->PacketList, lpNext);
		FreeUpPacket(lpNext);
		}
	else
		lpCmdPkt = lpNext;
	}

// pass through list getting rid of commands that have been undone
// This really needs to find a CT_EDITUNDO or a CT_MASKUNDO and then
// look backwards throught the list looking for the last
// CT_EDIT or CT_MASK.  The search would have to stop when
// a CT_ACTIVATE is hit.
lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->PacketList);
while (lpCmdPkt)
	{
	lpNext = (LPCMDPKT)ListGetNext(lpCmdPkt);
	if (!lpNext)
		break;
	Type = GetCommandType(lpCmdPkt->idCommand);
	NextType = GetCommandType(lpNext->idCommand);
	if ((Type == CT_EDIT && NextType == CT_EDITUNDO) ||
		(Type == CT_MASK && NextType == CT_MASKUNDO) )
		{
		ListUnlink(&lpCmdList->PacketList, lpCmdPkt);
		FreeUpPacket(lpCmdPkt);
		lpCmdPkt = (LPCMDPKT)ListGetNext(lpNext);
		ListUnlink(&lpCmdList->PacketList, lpNext);
		FreeUpPacket(lpNext);
		}
	else
		lpCmdPkt = lpNext;
	}
}

/***********************************************************************/
LOCAL BOOL AddPacket(LPCMDLIST lpCmdList, ITEMID idCommand, LPVOID lpParms,
					BOOL fCopyParms)
/***********************************************************************/
{
LPCMDPKT lpCmdPkt;

lpCmdPkt = CreatePacket(idCommand, lpParms, fCopyParms);
if (!lpCmdPkt)
	return(FALSE);
ListAddTail(&lpCmdList->PacketList, lpCmdPkt);
if (IsMacroRecording())
	MacroWritePacket(lpCmdPkt);
return(TRUE);
}

/***********************************************************************/
LPCMDPKT CreatePacket(ITEMID idCommand, LPVOID lpParms,
					BOOL fCopyParms)
/***********************************************************************/
{
LPCMDPKT lpCmdPkt;
LPCMD_PARMS lpCmdParms;
int i;
size_t SizeOfParms;

// get index into command description table
if ((i = GetCommandIndex(idCommand)) < 0)
	{
	Message(IDS_CMDOUTOFRANGE, idCommand);
	return(NULL);
	}

if (!(lpCmdPkt = (LPCMDPKT)Alloc((long)sizeof(CMDPKT))))
	{
	Message(IDS_EMEMALLOC);
	return(NULL);
	}
clr((LPTR)lpCmdPkt, sizeof(CMDPKT));

lpCmdPkt->idCommand = idCommand;
lpCmdPkt->fSuccess = YES;
if (fCopyParms)
	{
	SizeOfParms = GetCommandParmsSize(idCommand);
	if (SizeOfParms)
		{
		if (!(lpCmdPkt->lpParms = Alloc(SizeOfParms)))
			{
			FreeUp(lpCmdPkt);
			Message(IDS_EMEMALLOC);
			return(NULL);
			}
		if (lpParms)
			copy((LPTR)lpParms, (LPTR)lpCmdPkt->lpParms, SizeOfParms);
		}
	}
else
	lpCmdPkt->lpParms = lpParms;
// if we have some parms, set a usage count for them
if (lpCmdPkt->lpParms)
	{
	lpCmdParms = (LPCMD_PARMS)lpCmdPkt->lpParms;
	lpCmdParms->Common.lUsageCount = 1;
	lpCmdParms->Common.fParmsStored = FALSE;
	}
return(lpCmdPkt);
}
				 
/***********************************************************************/
LPCMDPKT CopyPacket(LPCMDPKT lpCmdPkt)
/***********************************************************************/
{
LPCMDPKT lpNewPkt;
LPCMD_PARMS lpCmdParms;

if (!(lpNewPkt = (LPCMDPKT)Alloc((long)sizeof(CMDPKT))))
	{
	Message(IDS_EMEMALLOC);
	return(NULL);
	}
*lpNewPkt = *lpCmdPkt;
lpNewPkt->Link.lpNext = lpNewPkt->Link.lpPrev = NULL;
if (lpNewPkt->lpParms)
	{
	lpCmdParms = (LPCMD_PARMS)lpNewPkt->lpParms;
#ifdef WIN32
	InterlockedIncrement(&lpCmdParms->Common.lUsageCount);
#else
	++lpCmdParms->Common.lUsageCount;
#endif
	}
return(lpNewPkt);
}

/***********************************************************************/
BOOL PPFileCopy(LPTSTR lpSrcFile, LPTSTR lpDstFile)
/***********************************************************************/
{
   	SCODE sc = StgIsStorageFile(lpSrcFile);
	if (sc == S_OK)
 	{
		LPSTORAGE lpSrcStorage, lpDstStorage;

		// Open the Root Storage
		sc = StgOpenStorage(
			lpSrcFile, 
			NULL,
			STGM_READ|STGM_SHARE_EXCLUSIVE,
			NULL, 
			0, 
			&lpSrcStorage);

		if (FAILED(sc))
			return(FALSE);

		// Create the Root Storage
		sc = StgCreateDocfile(
			lpDstFile, 
			STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
			0, 
			&lpDstStorage);
		if (FAILED(sc))
		{
			ReleaseInterface((LPUNKNOWN *)&lpSrcStorage);
			return(FALSE);
		}
        sc = lpSrcStorage->CopyTo(NULL,NULL,NULL,lpDstStorage);
		ReleaseInterface((LPUNKNOWN *)&lpSrcStorage);
		ReleaseInterface((LPUNKNOWN *)&lpDstStorage);
		if (FAILED(sc))
		{
			DeleteFile(lpDstFile);
			return(FALSE);
		}
		else
			return(TRUE);
	}
	else
		return(FileCopy(lpSrcFile, lpDstFile));
}

/***********************************************************************/
BOOL ExportFile(LPSTR lpSrcFile, LPSTR lpDstDir, LPINT lpFileNum, LPINT lpCopies)
/***********************************************************************/
{
	FNAME szDstFile;
	LIST PacketList;
	BOOL fRet = FALSE;

	ListInit(&PacketList);

	// see if the file type is so that we can read the packet list
	ITEMID idFileType = GetPPFileType(lpSrcFile);
	if (idFileType == IDN_PP || idFileType == IDN_PPCOMPOUND)
	{
		LPCMDPKT lpCmdPkt;

		// read the packet list
		CReadImage read(lpSrcFile);
		if (!read.ReadPacketList())
			return(FALSE);

		// retrieve the packet list
		read.GetPacketList(&PacketList);

		// get first packet in list
		lpCmdPkt = (LPCMDPKT)ListGetHead(&PacketList);
		while (lpCmdPkt)
		{
			LPSTR lpFileName = NULL;

			if (GetCommandType(lpCmdPkt->idCommand) == CT_LOAD)
			{
				lpFileName = GetLoadFileName(lpCmdPkt);
				if (StringsEqual(lpFileName, lpSrcFile))
					lpFileName = NULL;
			}
			else
			if (lpCmdPkt->idCommand == IDS_CMD_PASTEFROM && idFileType == IDN_PP)
			{
				LPPASTEFROM_PARMS lpParms;

				lpParms = (LPPASTEFROM_PARMS)lpCmdPkt->lpParms;
				lpParms->szClipboard[0] = '\0';
				lpFileName = lpParms->szClipFileName;
			}
			else
			if (lpCmdPkt->idCommand == IDS_CMD_LOADMASK && idFileType == IDN_PP)
			{
				LPLOADMASK_PARMS lpParms;

				lpParms = (LPLOADMASK_PARMS)lpCmdPkt->lpParms;
				lpParms->szMask[0] = '\0';
				lpFileName = lpParms->szMaskFileName;
			}
			if (lpFileName)
			{
				if (!ExportFile(lpFileName, lpDstDir, lpFileNum, lpCopies))
				{
					DestroyPacketList(&PacketList);
					return(FALSE);
				}
				if (lpFileNum)
					SetExportFileName(lpFileName);
			}
			lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
		}
	}

	lstrcpy(szDstFile, lpDstDir);
	lstrcat(szDstFile, filename(lpSrcFile));
	if (lpFileNum)
	{
		*lpFileNum += 1;
		if (AstralClockCursor(*lpFileNum, *lpCopies, TRUE))
		{
			DestroyPacketList(&PacketList);
			return(FALSE);
		}
		if (fRet = PPFileCopy(lpSrcFile, szDstFile))
		{
			if (fRet && idFileType == IDN_PPCOMPOUND && ListGetCount(&PacketList))
			{
				LPSTORAGE pStorage;
				SCODE sc;

				// open new storage file
				sc = StgOpenStorage(
					szDstFile,
					NULL,
					STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
					0, 
					0, 
					&pStorage);

				fRet = sc == NOERROR;
				if (fRet)
					fRet = UpdateClipBits(pStorage, &PacketList, TRUE, TRUE);
				if (fRet)
					fRet = UpdateMaskBits(pStorage, &PacketList, TRUE, TRUE);
				ReleaseInterface((LPUNKNOWN *)&pStorage);
			}
			if (fRet && (idFileType == IDN_PP || idFileType == IDN_PPCOMPOUND) && 
                ListGetCount(&PacketList))
			{
				CWriteImage write(szDstFile);
				fRet = write.UpdatePacketList(&PacketList);
			}
			if (!fRet)
			{
				Message(IDS_ECOPYFILE, lpSrcFile, szDstFile);
				DeleteFile(szDstFile);
			}
		}
		else
		{
			Message(IDS_ECOPYFILE, lpSrcFile, szDstFile);
		}
	}
	else
	{
		*lpCopies += 1;
		fRet = TRUE;
	}
	DestroyPacketList(&PacketList);
	return(fRet);
}

/***********************************************************************/
void FixExportFileNames(LPSTR lpCurrentFile, LPLIST lpPacketList)
/***********************************************************************/
{
	// get first packet in list
	LPCMDPKT lpCmdPkt = (LPCMDPKT)ListGetHead(lpPacketList);
	while (lpCmdPkt)
	{
		LPSTR lpFileName = NULL;

		if (GetCommandType(lpCmdPkt->idCommand) == CT_LOAD)
			lpFileName = GetLoadFileName(lpCmdPkt);
		else
		if (lpCmdPkt->idCommand == IDS_CMD_PASTEFROM)
		{
			LPPASTEFROM_PARMS lpParms;

			lpParms = (LPPASTEFROM_PARMS)lpCmdPkt->lpParms;
			lpFileName = lpParms->szClipFileName;
		}
		else
		if (lpCmdPkt->idCommand == IDS_CMD_LOADMASK)
		{
			LPLOADMASK_PARMS lpParms;

			lpParms = (LPLOADMASK_PARMS)lpCmdPkt->lpParms;
			lpFileName = lpParms->szMaskFileName;
		}
		if (lpFileName)
			FixExportFileName(lpCurrentFile, lpFileName);
		lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	}
}

/***********************************************************************/
void FixExportFileName(LPSTR lpCurrentFile, LPSTR lpFileName)
/***********************************************************************/
{
	if (lpFileName[0] == '.')
	{
	   FNAME szNewName;
	
	   lstrcpy(szNewName, lpCurrentFile);
	   stripfile(szNewName);
	   lstrcat(szNewName, &lpFileName[2]);
	   lstrcpy(lpFileName, szNewName);
	}
}

/***********************************************************************/
void SetExportFileName(LPSTR lpFileName)
/***********************************************************************/
{
   FNAME szNewName;

   szNewName[0] = '.';
   szNewName[1] = '\\';
   szNewName[2] = '\0';
   lstrcat(szNewName, filename(lpFileName));
   lstrcpy(lpFileName, szNewName);
}




