//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "cmdmacro.h"
#include "macro.h"
#include "id.h"
#include "svrview.h"
#include "svrdoc.h"
#include "bsrutil.h"

typedef enum
	{
	MM_NONE,
	MM_RECORD,
	MM_PLAY
	} MACRO_MODE;

LOCAL LPCMDLIST	ProgActivateWindow(LPACTIVATEWINDOW_PARMS lpParms);
LOCAL BOOL			NeedSequence(LPCMDLIST lpCmdList);
LOCAL void			CommandError(ITEMID idCommand);
LOCAL int			CountType(LPTSTR lpFileName, COMMAND_TYPE Type);
LOCAL int			CountType(LPLIST lpPktList, COMMAND_TYPE Type);
LOCAL BOOL			FindCommand(MACRO_FILE_HANDLE fh, ITEMID idCommand);
LOCAL BOOL			FindCommand(LPLIST lpPktList, ITEMID idCommand);
LOCAL BOOL			SetupMacro(LPQUEUEITEM lpQueueItem);
LOCAL BOOL 			CheckPCDSave(LPTSTR lpFileName);

static MACRO_MODE MacroMode = MM_NONE;
static FNAME szMacroFileName;
static MACRO_FILE_HANDLE MacroFileHandle = MACRO_FILE_HANDLE_INVALID;
static LPTSTR lpMacroFormat = NULL;
static LPTSTR lpMacroString = NULL;
static LPABORTPROC lpAbortProc;
static BOOL fPCDNotice = FALSE;

/***********************************************************************/
LPABORTPROC MacroSetAbortProc(LPABORTPROC lpProc)
/***********************************************************************/
{
LPABORTPROC lpOldProc;

lpOldProc = lpAbortProc;
lpAbortProc = lpProc;
SetCmdAbortProc(lpProc);
return(lpOldProc);
}

/***********************************************************************/
void PlayBatchMacro(LPLIST lpQueueList)
/***********************************************************************/
{
	LPQUEUEITEM 	lpQueueItem = NULL;
	LPMACROITEM 	lpMacroItem;
	FNAME 			szFileName;
	LIST 			TempList;
	BOOL			fError = TRUE;

	fPCDNotice = FALSE;
	while (TRUE)
	{
		if (lpQueueItem)
			lpQueueItem = (LPQUEUEITEM)ListGetNext(lpQueueItem);
		else
			lpQueueItem = (LPQUEUEITEM)ListGetHead(lpQueueList);

		if (!lpQueueItem)
		{
			fError = FALSE;
			break;
		}

		ListInit(&lpQueueItem->PacketList);

		// create a command list for the load command
		if (!CreateLoadFileParms(0, lpQueueItem->szFileName, TRUE,
 								&lpQueueItem->cmsInfo,
								&lpQueueItem->PhotoCDResOverride,
								&lpQueueItem->parms))
			continue;

		lpMacroItem = NULL;
		while (TRUE)
		{
			if (lpMacroItem)
				lpMacroItem = (LPMACROITEM)ListGetNext(lpMacroItem);
			else
				lpMacroItem = (LPMACROITEM)ListGetHead(&lpQueueItem->MacroList);
			if (!lpMacroItem)
				break;

			if ( !LookupExtFile( lpMacroItem->szMacro, szFileName, IDN_MACRO ) )
				continue;

			// read in the entire macro file for faster processing
			if (!ReadMacro(szFileName, &TempList))
				continue;
			if (MacroAnyLoadCommands(&TempList))
    		{
	    		Message(IDS_BADBATCHMACRO);
				DestroyPacketList(&TempList);
				continue;
		    }
			ListAddTail(&lpQueueItem->PacketList, ListGetHead(&TempList));
		}

		if (!SetupMacro(lpQueueItem))
			break;
	}
	if (!fError)
	{
		if (EnableLogging(TRUE))
		{
			lpQueueItem = NULL;
			while (!fError)
			{
				if (lpQueueItem)
					lpQueueItem = (LPQUEUEITEM)ListGetNext(lpQueueItem);
				else
					lpQueueItem = (LPQUEUEITEM)ListGetHead(lpQueueList);
				if (!lpQueueItem)
					break;
				if (!PlayMacro(NULL, NULL, 1, NO, &lpQueueItem->PacketList, NULL, lpQueueItem->PhotoCDResOverride))
					break;
			}
		 	EnableLogging(FALSE);
		}
	}

	lpQueueItem = NULL;
	while (TRUE)
	{
		if (lpQueueItem)
			lpQueueItem = (LPQUEUEITEM)ListGetNext(lpQueueItem);
		else
			lpQueueItem = (LPQUEUEITEM)ListGetHead(lpQueueList);
		if (!lpQueueItem)
			break;

		DestroyPacketList(&lpQueueItem->PacketList);
		DestroyPtrList(&lpQueueItem->MacroList);
	}
	DestroyPtrList(lpQueueList);
}

/***********************************************************************/
void DestroyPtrList(LPLIST lpList)
/***********************************************************************/
{
	LPTR lpNode;

	lpNode = (LPTR)ListGetHead(lpList);
	while (lpNode)
		{
		ListUnlink(lpList, lpNode);
		FreeUp(lpNode);
		lpNode = (LPTR)ListGetHead(lpList);
		}
}

/***********************************************************************/
LOCAL BOOL SetupMacro(LPQUEUEITEM lpQueueItem)
/***********************************************************************/
{
	LPCMDPKT		lpCmdPkt;
	FNAME		   	szFileName;

	lpCmdPkt = CreatePacket(IDS_CMD_LOADFILE, &lpQueueItem->parms, TRUE);
	if (!lpCmdPkt)
		goto MemError;
	ListAddHead(&lpQueueItem->PacketList, lpCmdPkt);

	// create the new filename
	// add a save command to the end if necessary
	if (Macro.BatchSave == IDC_SAVETOORIGINAL)
		lstrcpy(szFileName, lpQueueItem->szFileName);
	else
	if (Macro.BatchSave == IDC_SAVETODIR)
	{
		lstrcpy(szFileName, Macro.BatchDir);
		lstrcat(szFileName, filename(lpQueueItem->szFileName));
	}
	else
	if (Macro.BatchSave == IDC_SAVETOALBUM)
	{
		lstrcpy(szFileName, Browser.AlbumFileDir);
		lstrcat(szFileName, filename(lpQueueItem->szFileName));
	}

	// if we are changing the file type, then change the extension
	if (Macro.fBatchChangeType &&
		(Macro.BatchSave == IDC_SAVETODIR || Macro.BatchSave == IDC_SAVETOALBUM))
	{
		STRING szExt;

		LookupExtension(LocalFileType(Macro.BatchFileType), szExt);
		stripext(szFileName);
		lstrcat(szFileName, szExt);
	}

	// check to make sure we are not saving to PCD and overwrite warning
	if (Macro.BatchSave != IDC_NOSAVE)
	{
		if (!CheckPCDSave(szFileName))
			return(FALSE);
		if (Macro.BatchSave != IDC_SAVETOORIGINAL)
		{
			if (FileExists(szFileName))
			{
				if (AstralOKCancel (IDS_OVERWRITEIMAGE, (LPTSTR)szFileName) != IDOK)
					return(FALSE);
			}
		}

	}

	if (Macro.BatchSave == IDC_SAVETOALBUM)
	{
		// get a path name from the browser
		SAVETOALBUM_PARMS parms;

		lstrcpy(parms.szAlbum, Macro.BatchAlbum);
		lstrcpy(parms.szFileName, szFileName);
	  	parms.idFileType = 	GetPPFileType(parms.szFileName);
		parms.idDataType = 0;
		parms.EPSOptions = EPSOptions;
		parms.TIFFOptions = TIFFOptions;
		parms.TGAOptions = TGAOptions;
		parms.JPEGOptions = JPEGOptions;
		parms.PPFFOptions = PPFFOptions;
		parms.AVIOptions = AVIOptions;

		// create a command list for the load command
		lpCmdPkt = CreatePacket(IDS_CMD_SAVETOALBUM, &parms, TRUE);
		if (!lpCmdPkt)
			goto MemError;
		ListAddTail(&lpQueueItem->PacketList, lpCmdPkt);
	}
	else
	if (Macro.BatchSave != IDC_NOSAVE)
	{
		SAVEFILE_PARMS parms;

		lstrcpy(parms.szFileName, szFileName);
		parms.idFileType = 	GetPPFileType(parms.szFileName);
		parms.idDataType = 0;
		parms.EPSOptions = EPSOptions;
		parms.TIFFOptions = TIFFOptions;
		parms.TGAOptions = TGAOptions;
		parms.JPEGOptions = JPEGOptions;
		parms.PPFFOptions = PPFFOptions;
		parms.AVIOptions = AVIOptions;

		// create a command list for the load command
		lpCmdPkt = CreatePacket(IDS_CMD_SAVEFILE, &parms, TRUE);
		if (!lpCmdPkt)
			goto MemError;
		ListAddTail(&lpQueueItem->PacketList, lpCmdPkt);
	}
	if (Macro.fBatchClose)
	{
		CMD_PARMS parms;

		// create a command list for the load command
		lpCmdPkt = CreatePacket(IDS_CMD_CLOSE, &parms, TRUE);
		if (!lpCmdPkt)
			goto MemError;
		ListAddTail(&lpQueueItem->PacketList, lpCmdPkt);
	}
	return(TRUE);

MemError:
	Message(IDS_EMEMALLOC);
	return(FALSE);
}

/***********************************************************************/
LOCAL BOOL CheckPCDSave(LPTSTR lpFileName)
/***********************************************************************/
{
	FNAME		   	szPP5FileName;
	STRING			szExt;

	if (GetPPFileType(lpFileName) == IDN_PHOTOCD)
	{
		lstrcpy(szPP5FileName, lpFileName);
		stripext(szPP5FileName);
		AstralStrEx(IDN_PP, szExt, sizeof(szExt));
		//lstrcat(szPP5FileName, szExt+1);
		lstrcat(szPP5FileName, MyCharNext( szExt ));

		if (!fPCDNotice)
		{
			int retc = AstralOKCancel(IDS_SAVEPCDASPP5);
			fPCDNotice = TRUE;
			if (retc == IDCANCEL)
				return(FALSE);
		}
		lstrcpy(lpFileName, szPP5FileName);
	}
	return(TRUE);
}

/***********************************************************************/
BOOL PlayMacro(LPCMDLIST lpCmdList, LPTSTR lpFileName, int nRepeat,
				BOOL fSequenceAll, LPLIST lpMacroList, HWND hParent,
				int PhotoCDResOverride, LPTSTR lpMacroName)
/***********************************************************************/
{
LPIMAGE lpImage;
LPCMDLIST lpNewList;
int nActivates, iCount;
ITEMID idCommand;
STRING szString, szAppName;
BOOL fError;
BOOL fSequence, fCmdSequence, fCopyPackets;
LIST MacroList;
LPCMDPKT lpCmdPkt, lpNextPkt;
MACROSETUP Setup;
COMMAND_TYPE CommandType;
HWND hDlg = NULL;

lpAbortProc = NULL;
if (!hParent)
	hParent = PictPubApp.Get_hWndAstral();

// see if we need to copy our packets
fCopyPackets = nRepeat > 1;

if (!lpMacroList)
	{
	// read in the entire macro file for faster processing
	if (!ReadMacro(lpFileName, &MacroList))
		return(FALSE);
	lpMacroList = &MacroList;
	}

// count the number of activates in the macro file
// because it affects sequencing
nActivates = CountType(lpMacroList, CT_ACTIVATE);

// turn of macro play mode and tell the world
MacroMode = MM_PLAY;
if ( AstralStrEx( IDS_APPNAME, szAppName, sizeof(szAppName) ) )
	{
	if ( AstralStrEx( IDS_MACROPLAY, szString, sizeof(szString) ) )
		{
		lstrcat( szAppName, szString );
		SetWindowText( PictPubApp.Get_hWndAstral(), szAppName );
		}
	}

// reset untitled number so that if a macro is played it
// can deal with untitled images the same
Control.UntitledNo = 0;

// If no command list passed in to work on (Macro Batch Mode)
// then get command list for active image
if (!lpCmdList)
	{
	if (lpImage = GetActiveImage())
		lpCmdList = lpImage->lpCmdList;
	else
		lpCmdList = NULL;
	}

// See if the macro contains any low res loads and
// if so ask to user if he'd like to convert them
// to hi res loads and if so do the convert
if (FindCommand(lpMacroList, IDS_CMD_LOWRESLOAD))
	if (AstralAffirm(IDS_CONVERTLOWRES))
		if (!ConvertLowResLoad(lpMacroList))
			{
			DestroyPacketList(lpMacroList);
			return(FALSE);
			}

// disable all mouse and keyboard input during macro play
EnableWindow(hParent, FALSE);

// if not in a threading environment and caller wants us
// to display a progress dialog, then set it up and do it
if (!Control.UseThreading)
	{
	iCount = ListGetCount(lpMacroList) - nActivates;
	iCount *= nRepeat;
	Setup.iTotal = iCount;
	Setup.idDialog = IDD_MACRO_STATUS;
	Setup.lpFileName = NULL;
    hDlg = AstralDlgParam( YES, PictPubApp.GetResourceHandle(), hParent, IDD_MACRO_STATUS,
						DlgMacroStatusProc,
						(LPARAM)(LPVOID)&Setup );
	if (hDlg)
		{
		if (lpMacroName)
			{
			STRING szString;

			GetWindowText(hDlg, szString, sizeof(szString));
			lstrcat(szString, _T(" - "));
			lstrcat(szString, lpMacroName);
			SetWindowText(hDlg, szString);
			}

		UpdateWindow(hDlg);
		}
	}

// Repeat macro nRepeat number of times
fError = FALSE;
while (--nRepeat >= 0 && !fError)
	{
	if (lpAbortProc && (*lpAbortProc)())
		break;
	// back to beginning of macro file
	lpNextPkt = (LPCMDPKT)ListGetHead(lpMacroList);

	// initialize sequencing
	fSequence = fSequenceAll;

	while (!fError && lpNextPkt)
		{
		if (lpAbortProc && (*lpAbortProc)())
			{
			fError = TRUE;
			break;
			}
		// get the packet to work on
		if (fCopyPackets)
			{
			lpCmdPkt = CopyPacket(lpNextPkt);
			if (!lpCmdPkt)
				{
				fError = TRUE;
				break;
				}
			}
		else
			{
			ListUnlink(lpMacroList, lpNextPkt);
			lpCmdPkt = lpNextPkt;
			}

		// get command id and parms for this command
		idCommand = lpCmdPkt->idCommand;

		// Find out whether this command requires sequencing
		// set it here, so command can change it if needed before
		// we actually set fSequence
		fCmdSequence = GetCommandSequence(idCommand);
		CommandType = GetCommandType(idCommand);

		// Handle the different types of commands
		switch (CommandType)
			{
			case CT_LOAD:
				// create new command list for load
				lpNewList = CreateCommandList();
				if (lpNewList)
					{
					ListAddTail(&lpNewList->PacketList, lpCmdPkt);

					// if we already have a command list containing commands,
					// kick off the execution of those commands before we
					// switch to the new command list
					if (lpCmdList && !ListIsEmpty(&lpCmdList->PacketList))
						{
						PlaybackCommands(lpCmdList);
						// if some command in the command list affects
						// sequencing force the whole command list to
						// be processed
						if (fSequence)
							{
							FlushCommands(lpCmdList);
							fSequence = fSequenceAll;
							}
						}
					// setup new command list for us to work with
					lpCmdList = lpNewList;
					lpCmdList->PhotoCDResOverride = PhotoCDResOverride;

					// If there are any activates in this macro make sure a
					// command that creates an image processes immediately
					if (nActivates)
						{
						PlaybackCommands(lpCmdList);
						FlushCommands(lpCmdList);
						fSequence = fSequenceAll;
						fCmdSequence = NO; // already sequenced
						}
					}
				break;

			case CT_COPY:
				// Make sure we have a command list to work with
				if (!lpCmdList)
					{
				 	Message(IDS_NOIMAGETOWORKON);
					fError = TRUE;
					break;
					}
				// Just add this command to the command list
				ListAddTail(&lpCmdList->PacketList, lpCmdPkt);

				// If there are any activates in this macro make sure a
				// command that creates an image processes immediately
				if (nActivates)
					{
					PlaybackCommands(lpCmdList);
					FlushCommands(lpCmdList);
					fSequence = fSequenceAll;
					fCmdSequence = NO; // already sequenced
					}
				break;

			case CT_SAVE:
			case CT_EDIT:
			case CT_MASK:
			case CT_MODE:
			case CT_EDITUNDO:
			case CT_MASKUNDO:
			case CT_EDITOBJ:
			case CT_CLOSE:
				// Make sure we have a command list to work with
				if (!lpCmdList)
					{
				 	Message(IDS_NOIMAGETOWORKON);
					fError = TRUE;
					break;
					}
				if (CommandType == CT_SAVE)
					{
					PlaybackCommands(lpCmdList);
					if (!lpCmdList->ThreadData.lpImage)
						{
				 		Message(IDS_NOIMAGETOWORKON);
						fError = TRUE;
						break;
						}
					}

				// Just add this command to the command list
				ListAddTail(&lpCmdList->PacketList, lpCmdPkt);
				if (CommandType != CT_CLOSE)
					break;

			case CT_ACTIVATE:
				// Program or commands that affect window activation
				// are processed here
				switch (idCommand)
					{
					case IDS_CMD_CLOSE:
					case IDS_CMD_ACTIVATEWINDOW:
						{
						// if we already have a command list containing commands,
						// kick of the execution of those commands before we
						// switch to the new command list
						if (lpCmdList && !ListIsEmpty(&lpCmdList->PacketList))
							{
							PlaybackCommands(lpCmdList);
							// if some command in the command list affects
							// sequencing force the whole command list to
							// be processed
							if (fSequence)
								{
								FlushCommands(lpCmdList);
								fSequence = fSequenceAll;
								}
							}
						// if this was a close command, then wack the command list pointer
						if (idCommand == IDS_CMD_CLOSE)
							lpCmdList = NULL;
						else
							{
							// now process the activate and get a new command list
							lpNewList = ProgActivateWindow(
									(LPACTIVATEWINDOW_PARMS)lpCmdPkt->lpParms);
							// setup the new command list if we got one
							if (lpNewList)
								lpCmdList = lpNewList;
							else
								{
								CommandError(idCommand);
								fError = TRUE;
								}
							// activate don't go through command processing
							// so we have to free it up here
							FreeUpPacket(lpCmdPkt);
							}
						}
					break;

					default:
					break;
					}
				break;

			default:
				break;
			}
		// if command just handled requires sequencing
		// set sequencing flag
		if (fCmdSequence)
			fSequence = YES;
		if (fCopyPackets)
			// get next command packet in macro list
			lpNextPkt = (LPCMDPKT)ListGetNext(lpNextPkt);
		else
			// head of list will be next one if we're not copying
			lpNextPkt = (LPCMDPKT)ListGetHead(lpMacroList);
		}
	}
// get rid of macro list
DestroyPacketList(lpMacroList);

// if we already have a command list containing commands,
// kick off the execution of those commands
if (lpCmdList && !ListIsEmpty(&lpCmdList->PacketList))
	{
	PlaybackCommands(lpCmdList);
	if (fSequenceAll)
		FlushCommands(lpCmdList);
	}

// turn off the macro mode
EnableWindow(hParent, TRUE);
if ( AstralStrEx( IDS_APPNAME, szAppName, sizeof(szAppName) ) )
	SetWindowText( PictPubApp.Get_hWndAstral(), szAppName );
// if we have a progress dialog, nuke it
if (hDlg)
	AstralDlgEnd(hDlg, TRUE);
// if we are display the macro status for another modal dialog 
// make sure the main app stuff is still disabled
if (hParent != PictPubApp.Get_hWndAstral())
	{
	EnableOverlappedWindow( PictPubApp.Get_hWndAstral(), FALSE );
	EnableWindow(PictPubApp.Get_hWndAstral(), FALSE);
	}
MacroMode = MM_NONE;
return(TRUE);
}

/***********************************************************************/
BOOL ReadMacro(LPTSTR lpFileName, LPLIST lpMacroList, BOOL fReadFirstPacketOnly)
/***********************************************************************/
{
LPTSTR lpCommand;
MACRO_FILE_HANDLE fh;
LPCMDPKT lpCmdPkt;
BOOL fError;
FNAME OEMName;

// zero out the list
ListInit(lpMacroList);

// Open the macro file
#ifdef BUFFERED_IO
AnsiToOem(lpFileName, OEMName);
fh = fopen(OEMName, _T("rb"));
if (fh == NULL)
#else
fh = FileOpen(lpFileName, FO_READ);
if (fh == MACRO_FILE_HANDLE_INVALID)
#endif
	{
	Message(IDS_EOPEN, lpFileName);
	return(FALSE);
	}
// Allocate a buffer to read the commands into
if (!(lpCommand = (LPTSTR)Alloc(MAX_CMD_LEN)))
	{
#ifdef BUFFERED_IO
	fclose(fh);
#else
	FileClose(fh);
#endif
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}
AstralCursor(IDC_WAIT);
while (lpCmdPkt = ReadPacket(fh, lpCommand, &fError))
	{
	ListAddTail(lpMacroList, lpCmdPkt);
	if (fReadFirstPacketOnly)
		break;
	}
AstralCursor(NULL);
FreeUp(lpCommand);
#ifdef BUFFERED_IO
fclose(fh);
#else
FileClose(fh);
#endif
if (fError)
	{
	DestroyPacketList(lpMacroList);
	return(FALSE);
	}
return(TRUE);
}

#ifdef UNUSED
/***********************************************************************/
BOOL ReadMacro(HPTR hpBuf, LPLIST lpMacroList, BOOL fReadFirstPacketOnly)
/***********************************************************************/
{
LPTSTR lpCommand;
MACRO_FILE_HANDLE fh;
LPCMDPKT lpCmdPkt;
BOOL fError;

// zero out the list
ListInit(lpMacroList);

// Allocate a buffer to read the commands into
if (!(lpCommand = (LPTSTR)Alloc(MAX_CMD_LEN)))
	{
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}
while (lpCmdPkt = ReadPacket(&hpBuf, lpCommand, &fError))
	{
	ListAddTail(lpMacroList, lpCmdPkt);
	if (fReadFirstPacketOnly)
		break;
	}
FreeUp(lpCommand);
if (fError)
	{
	DestroyPacketList(lpMacroList);
	return(FALSE);
	}
return(TRUE);
}
#endif

/***********************************************************************/
BOOL StopMacro(LPTSTR lpFileName)
/***********************************************************************/
{
STRING szAppName;

if (MacroMode != MM_RECORD)
	return(FALSE);
#ifdef BUFFERED_IO
fclose(MacroFileHandle);
#else
FileClose(MacroFileHandle);
#endif
FreeUp(lpMacroFormat);
FreeUp(lpMacroString);
if (lpFileName)
	lstrcpy(lpFileName, szMacroFileName);
else
	FileDelete(szMacroFileName);
if ( AstralStrEx( IDS_APPNAME, szAppName, sizeof(szAppName) ) )
	SetWindowText( PictPubApp.Get_hWndAstral(), szAppName );
MacroMode = MM_NONE;
return(TRUE);
}

/***********************************************************************/
BOOL MacroAnyLoadCommands(LPTSTR lpFileName)
/***********************************************************************/
{
return(CountType(lpFileName, CT_LOAD) > 0);
}

/***********************************************************************/
BOOL MacroAnyLoadCommands(LPLIST lpPacketList)
/***********************************************************************/
{
return(CountType(lpPacketList, CT_LOAD) > 0);
}

/***********************************************************************/
BOOL RecordMacro(LPTSTR lpFileName)
/***********************************************************************/
{
STRING szString, szAppName;
FNAME  OEMName;

lpMacroFormat = (LPTSTR)Alloc(MAX_CMD_LEN);
lpMacroString = (LPTSTR)Alloc(MAX_CMD_LEN);
if (!lpMacroString || !lpMacroFormat)
	{
	Message(IDS_EMEMALLOC);
	if (lpMacroFormat)
		FreeUp(lpMacroFormat);
	if (lpMacroString)
		FreeUp(lpMacroString);
	return(FALSE);
	}
#ifdef BUFFERED_IO
AnsiToOem(lpFileName, OEMName);
MacroFileHandle = fopen(OEMName, _T("w+b"));
if (MacroFileHandle == NULL)
#else
MacroFileHandle = FileOpen(lpFileName, FO_CREATE|FO_WRITE);
if (MacroFileHandle == MACRO_FILE_HANDLE_INVALID)
#endif
	{
	FreeUp(lpMacroFormat);
	FreeUp(lpMacroString);
	return(FALSE);
	}
lstrcpy(szMacroFileName, lpFileName);

if ( AstralStrEx( IDS_APPNAME, szAppName, sizeof(szAppName) ) )
	{
	if ( AstralStrEx( IDS_MACRORECORD, szString, sizeof(szString) ) )
		{
		lstrcat( szAppName, szString );
		SetWindowText( PictPubApp.Get_hWndAstral(), szAppName );
		}
	}
Control.UntitledNo = 0;
MacroMode = MM_RECORD;
return(TRUE);
}

/************************************************************************/
BOOL MacroWritePacket(LPCMDPKT lpCmdPkt)
/************************************************************************/
{
if (!WritePacket(lpCmdPkt, MacroFileHandle, lpMacroFormat, lpMacroString))
	{
	Message(IDS_EWRITE, (LPTSTR)szMacroFileName);
	StopMacro(NULL);
	return(FALSE);
	}
return(TRUE);
}

/************************************************************************/
BOOL IsRecording()
/************************************************************************/
{
return( IsMacroRecording() || IsJournalRecording() );
}

/************************************************************************/
BOOL IsPlaying()
/************************************************************************/
{
return( IsMacroPlaying() || IsJournalPlaying() );
}

/************************************************************************/
BOOL IsMacroRecording()
/************************************************************************/
{
return(MacroMode == MM_RECORD);
}

/************************************************************************/
BOOL IsMacroPlaying()
/************************************************************************/
{
return(MacroMode == MM_PLAY);
}

/***********************************************************************/
LOCAL BOOL NeedSequence(LPCMDLIST lpCmdList)
/***********************************************************************/
{
LPCMDPKT lpCmdPkt;
int i;

lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->PacketList);
while (lpCmdPkt)
	{
	if ((i = GetCommandIndex(lpCmdPkt->idCommand)) >= 0)
		{
		if (GetCommandSequence(lpCmdPkt->idCommand))
			return(TRUE);
		}
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	}
return(FALSE);
}

/***********************************************************************/
LOCAL LPCMDLIST ProgActivateWindow(LPACTIVATEWINDOW_PARMS lpParms)
/***********************************************************************/
{
int iOffset, nDocs;
CWnd *pActiveWnd, *pNextWnd, *pChild;
CMDIFrameWnd *pMainWnd;
	
pNextWnd = NULL;

pMainWnd = (CMDIFrameWnd *)PictPubApp.m_pMainWnd;

// first look for the filename
CServerDoc *pDoc = PictPubApp.GetDocument(lpParms->szFileName);
if (pDoc)
	{
	// ignore if we are in place
	if (pDoc->IsInPlaceActive())
		return(NULL);

	POSITION ViewPos = pDoc->GetFirstViewPosition();
	if (ViewPos)
		{
	    CView* pView = pDoc->GetNextView( ViewPos );
		if (pView)
			pNextWnd = pView->GetParent();
		}
	}

// if not found, try using windows offsets
if (!pNextWnd && lpParms->iOffset)
	{
	nDocs = 0;
	pChild = pActiveWnd = pMainWnd->MDIGetActive();
	while (pChild)
		{
		++nDocs;
		pChild = pChild->GetWindow(GW_HWNDNEXT);
		}

	if (nDocs)
		{
		iOffset = lpParms->iOffset;
		pNextWnd = pActiveWnd;
		if (iOffset > 0)
			{
			if (iOffset >= nDocs)
				iOffset = nDocs - 1;
			while(pNextWnd && (--iOffset >= 0))
				pNextWnd = pNextWnd->GetWindow(GW_HWNDNEXT);
			}
		else
			{
			if (iOffset <= (-nDocs))
				iOffset = (-nDocs) + 1;
			while(pNextWnd && (++iOffset <= 0))
				pNextWnd = pNextWnd->GetWindow(GW_HWNDPREV);
			}
		}
	}

if (pNextWnd)
	{
	CPPMDIChildWnd *pMDIChild = (CPPMDIChildWnd *)pNextWnd;
	CServerView *pView = (CServerView*)pMDIChild->GetActiveView();
	if (pView)
		{
        CServerDoc *pDoc = pView->GetDocument();
       	ASSERT(pDoc && pDoc->m_lpImage);
		if (pDoc && pDoc->m_lpImage && !pDoc->IsInPlaceActive())
			{
			pMainWnd->MDIActivate(pNextWnd);
			return(pDoc->m_lpImage->lpCmdList);
			}
		}
	}
return(NULL);
}

/***********************************************************************/
LOCAL BOOL	FindCommand(LPLIST lpPktList, ITEMID idCommand)
/***********************************************************************/
{
LPCMDPKT lpCmdPkt;

lpCmdPkt = (LPCMDPKT)ListGetHead(lpPktList);
while (lpCmdPkt)
	{
	if (lpCmdPkt->idCommand == idCommand)
		return(TRUE);
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	}
return(FALSE);
}

/***********************************************************************/
LOCAL BOOL	FindCommand(MACRO_FILE_HANDLE fh, ITEMID idCommand)
/***********************************************************************/
{
LPVOID lpParms;
LPTSTR lpCommand;
int i;
BOOL found;

if (!(lpCommand = (LPTSTR)Alloc(MAX_CMD_LEN)))
	{
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}
found = FALSE;
while (ReadLine(fh, lpCommand, MAX_CMD_LEN))
	{
	i = ReadParms(fh, lpCommand, &lpParms);
	if (i < 0)
		continue;
	if (lpParms)
		FreeUpParms(GetCommandId(i), lpParms);
	if (GetCommandId(i) == idCommand)
		{
		found = TRUE;
		break;
		}
	}
FreeUp(lpCommand);
return(found);
}

/***********************************************************************/
LOCAL int CountType(LPLIST lpPktList, COMMAND_TYPE Type)
/***********************************************************************/
{
LPCMDPKT lpCmdPkt;
int nType = 0;

lpCmdPkt = (LPCMDPKT)ListGetHead(lpPktList);
while (lpCmdPkt)
	{
	if (GetCommandType(lpCmdPkt->idCommand) == Type)
		++nType;
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	}
return(nType);
}

/***********************************************************************/
LOCAL int CountType(LPTSTR lpFileName, COMMAND_TYPE Type)
/***********************************************************************/
{
LPVOID lpParms;
LPTSTR lpCommand;
int i;
int nType = 0;
MACRO_FILE_HANDLE fh;
FNAME       OEMName;

if (!(lpCommand = (LPTSTR)Alloc(MAX_CMD_LEN)))
	{
	Message(IDS_EMEMALLOC);
	return(-1);
	}

#ifdef BUFFERED_IO
AnsiToOem(lpFileName, OEMName);
fh = fopen(OEMName, _T("rb"));
if (fh == NULL)
#else
fh = FileOpen(lpFileName, FO_READ);
if (fh == MACRO_FILE_HANDLE_INVALID)
#endif
	{
	FreeUp((LPTR)lpCommand);
	Message(IDS_EOPEN, lpFileName);
	return(-1);
	}
while (ReadLine(fh, lpCommand, MAX_CMD_LEN))
	{
	i = ReadParms(fh, lpCommand, &lpParms);
	if (i < 0)
		continue;
	if (GetCommandType(GetCommandId(i)) == Type)
		++nType;
	if (lpParms)
		FreeUpParms(GetCommandId(i), lpParms);
	}
#ifdef BUFFERED_IO
fclose(fh);
#else
FileClose(fh);
#endif
FreeUp((LPTR)lpCommand);
return(nType);
}

/***********************************************************************/
LOCAL void CommandError(ITEMID idCommand)
/***********************************************************************/
{
TCHAR szCommand[MAX_CMD_LEN];
STRING szFormat;
STRING szName;

if (AstralStrEx(idCommand, szCommand, sizeof(szCommand)))
	GetCommandName(szCommand, szName);
else
	wsprintf(szName, "%d", idCommand);
AstralStrEx(IDS_COMMANDERROR, szFormat, sizeof(szFormat));
wsprintf(szCommand, szFormat, (LPTSTR)szName);
Print(szCommand);
}

/***********************************************************************/
BOOL ConvertLowResLoad(LPLIST lpPktList)
/***********************************************************************/
{
LPCMDPKT lpCmdPkt;

lpCmdPkt = (LPCMDPKT)ListGetHead(lpPktList);
while (lpCmdPkt)
	{
	if (lpCmdPkt->idCommand == IDS_CMD_LOWRESLOAD)
		{
		LPLOADFILE_PARMS lpLoadFileParms;
		LPLOWRESLOAD_PARMS lpLowResParms;

		lpLoadFileParms = (LPLOADFILE_PARMS)Alloc(sizeof(LOADFILE_PARMS));
		if (!lpLoadFileParms)
			{
			Message(IDS_EMEMALLOC);
			return(FALSE);
			}
		lpLowResParms = (LPLOWRESLOAD_PARMS)lpCmdPkt->lpParms;
		lstrcpy(lpLoadFileParms->szFileName, lpLowResParms->szFileName);
		lpLoadFileParms->idFileType = lpLowResParms->idFileType;
		lpLoadFileParms->cmsInfo = lpLowResParms->cmsInfo;

		FreeUpParms(lpCmdPkt->idCommand, lpCmdPkt->lpParms);
		lpCmdPkt->idCommand = IDS_CMD_LOADFILE;
		lpCmdPkt->lpParms = lpLoadFileParms;
		}
	lpCmdPkt = (LPCMDPKT)ListGetNext(lpCmdPkt);
	}
return(TRUE);
}



