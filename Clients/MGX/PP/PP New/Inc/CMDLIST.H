//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _CMDLIST_H_
#define _CMDLIST_H_

#include "image.h"
#include "cursors.h"
#include "list.h"

class CServerDoc;
class CServerView;

#define BUFFERED_IO

typedef DWORD UPDATE_TYPE;
typedef UPDATE_TYPE FAR *LPUPDATE_TYPE;
#ifdef BUFFERED_IO
#define MACRO_FILE_HANDLE_INVALID NULL
typedef FILE * MACRO_FILE_HANDLE;
#else
typedef FILE_HANDLE MACRO_FILE_HANDLE;
#define MACRO_FILE_HANDLE_INVALID FILE_HANDLE_INVALID
#endif

// update types
#define	UT_NONE			0L
#define	UT_AREA			1L
#define	UT_SIZE			2L
#define	UT_NEWIMAGE		4L
#define UT_DATATYPE		8L
#define UT_ACTIVEAREA	16L
#define UT_CLOSE		32L
#define UT_SAVE			64L

typedef BOOL ( FAR *LPABORTPROC )( );

typedef enum
	{
	CT_NONE,		// bad command type
	CT_LOAD,		// for loading an image (creating)
	CT_MODE,  		// for operating on the active image without editing 
	CT_EDIT,		// for editing the active image
	CT_MASK,		// for editint the mask of the active image
	CT_EDITUNDO, 	// an edit undo function
	CT_MASKUNDO, 	// a mask undo function
	CT_ACTIVATE, 	// an activate command
	CT_COPY,		// creates some form of copy/copies of the active image
	CT_EDITOBJ,     // command that operate on objects but not on bits,
                  	// e.g. move, select, layer, lock, group
	CT_CLOSE,		// a command that closes the image
	CT_SAVE			// an operation that saves the image
	} COMMAND_TYPE;

typedef struct _newimageinfo
	{
	LISTLINK	Link;
	class CImage *lpImage;	// the image associated with this thread
	} NEWIMAGEINFO, FAR *LPNEWIMAGEINFO;

#ifdef WIN32
typedef struct _threadinfo
	{
	CWinThread	*pThread;		// MFC thread object
	HANDLE		hCmdEvent;		// the thread command event
	STRING		szThreadName;	// the name of the thread
	BOOL		fSuspended;		// Is this thread suspended?
	} THREADINFO, FAR *LPTHREADINFO;
#endif

typedef struct _threaddata
	{
	class CImage *		lpImage;	// the image associated with this thread
	CServerDoc			*pDocument; // document pointer
#ifdef WIN32
	THREADINFO			ThreadInfo;	// thread specific information
	struct _progress	Progress;	// Progress data
	struct _cursordata	CursorData;	// Cursor statics
#endif
	} THREADDATA, FAR *LPTHREADDATA;

typedef struct _cmdpkt
	{
	LISTLINK	Link;
	ITEMID		idCommand;
	LPVOID	   	lpParms;			// paramaters
	BOOL		fSuccess;			// successful?
	BOOL		fDisabled;			// disabled?
	} CMDPKT, FAR *LPCMDPKT;


typedef void ( FAR *LPCMDNOTIFYPROC )( LPCMDPKT lpCmdPkt );

typedef struct _cmdlist
	{
	THREADDATA	ThreadData;		// Thread local storage
	LIST		PacketList; 	// packet list
	BOOL		fSaveCommands;	// special flag for saving commands
	LIST		UndoPacketList; // the undo packet list
	LIST		RedoPacketList; // the redo packet list
	UPDATE_TYPE	UpdateType;		// the type of update needed
	RECT		rUpdateArea;	// the area of update needed (UT_AREA)
	LIST		NewImageList;	// List of new images
	BOOL		fPlayback;		// we are playing back this command list
	BOOL		fInsertMode;	// are we in insert mode?
	int			iInsertIndex;	// where we are inserting after
	LPCMDNOTIFYPROC lpCmdNotifyProc; // command notify proc
	BOOL		fCmdListEdit;	// are we currently editing the command list
	LIST		LoadPacketList; // packet list read in
	int			PhotoCDResOverride; // special PhotoCD Res override setting
									// If this value is non-zero, then
									// all photo cd opens associated with
									// this images should use this resolution.
									// this also means that any command lists
									// created used in the build of this image
									// should also use this resolution.
	DWORD		dwImageID;		// Unique image id for this session
	LIST		InProcessList;	// commands currently being processed
	CServerDoc 	*pDocument;		// the document attached to this command list
	BOOL		fInOnProcessingDone; 	// are we currently in OnProcessingDone
									// this flag is used so we don't deadlock
									// waiting for a thread to die
	} CMDLIST, FAR *LPCMDLIST;

BOOL IsCmdEnabled(ITEMID idCommand, CServerView *pView, struct _cmd_parms * lpParms = NULL, BOOL fCheckForBusy = TRUE, CImage *pImage = NULL);
LPCMDNOTIFYPROC SetCmdNotifyProc(LPCMDNOTIFYPROC lpCmdNotifyProc = NULL);
LPABORTPROC SetCmdAbortProc(LPABORTPROC lpAbortProc = NULL);
UINT CmdThread(LPVOID lpParam);
BOOL CreateCmdThread(LPCMDLIST lpCmdList);
void KillCmdThread(LPCMDLIST lpCmdList);
BOOL ProcessLoadCommand(ITEMID idCommand, LPVOID lpParms);
BOOL ProcessCommand(LPCMDLIST lpCmdList, ITEMID idCommand, LPVOID lpParms);
BOOL CallCommand(LPCMDLIST lpCmdList, ITEMID idCommand, LPVOID lpParms);
BOOL PostCommand(LPCMDLIST lpCmdList, ITEMID idCommand, LPVOID lpParms);
BOOL AddCommand(LPCMDLIST lpCmdList, ITEMID idCommand, LPVOID lpParms);
BOOL AddCommandEx(LPCMDLIST lpCmdList, ITEMID idCommand, LPVOID lpParms, BOOL fCopyParms, BOOL fPostCommand);
BOOL DispatchPacketList(LPCMDLIST lpCmdList, LPLIST lpPacketList, BOOL fUpdateReset);
class CImage FAR* BuildImage(class CImage FAR *lpImage, LPLIST lpPacketList, int PhotoCDResOverride = 0);
BOOL PostProgCommand(ITEMID idCommand, LPVOID lpParms);
void DestroyCommandList(LPCMDLIST lpCmdList);
BOOL CopyPacketList(LPLIST lpSrcList, LPLIST lpDstList);
BOOL CopyPacketList(LPCMDPKT lpCmdPkt, LPLIST lpDstList);
LPCMDPKT CreatePacket(ITEMID idCommand, LPVOID lpParms, BOOL fCopyParms);
void DestroyPacketList(LPLIST lpPktList);
void FreeUpPacket(LPCMDPKT lpCmdPkt);
LPCMDPKT CopyPacket(LPCMDPKT lpCmdPkt);
LPCMDLIST AddLoadCommand(ITEMID idCommand, LPVOID lpParms);
LPCMDLIST AddLoadCommandEx(ITEMID idCommand, LPVOID lpParms,
					BOOL fCopyParms);
LPCMDLIST CreateCommandList();
BOOL ExecuteCommands(LPCMDLIST lpCmdList);
BOOL PlaybackCommands(LPCMDLIST lpCmdList, ITEMID idProgressDlg = 0);
void FlushCommands(LPCMDLIST lpCmdList);
BOOL CommandListUndo(LPCMDLIST lpCmdList);
BOOL CommandListRedo(LPCMDLIST lpCmdList);
BOOL SetSaveCommandMode(LPCMDLIST lpCmdList, BOOL fSaveCommands);
LPTHREADDATA GetThreadData();
void FreeUpCommandList(LPCMDLIST lpCmdList);
BOOL WriteCommandList(LPCMDLIST lpCmdList, MACRO_FILE_HANDLE fh);
BOOL WritePacketList(LPCMDPKT lpCmdPkt, MACRO_FILE_HANDLE fh);
int GetCommandIndex(ITEMID idCommand);
ITEMID	GetCommandId(int index);
COMMAND_TYPE GetCommandType(ITEMID idCommand);
size_t GetCommandParmsSize(ITEMID idCommand);
BOOL GetCommandSequence(ITEMID idCommand);
BOOL IsVariableResLoadCmd(LPCMDPKT lpCmdPkt);
BOOL CheckForRebuild(LPCMDPKT lpCmdPkt, int iResWanted, int iResCurrent, int PhotoCDResOverride);
LPTSTR GetLoadFileName(LPCMDPKT lpCmdPkt);
LPTSTR GetLoadVolumeID(LPCMDPKT lpCmdPkt);
ITEMID GetLoadFileType(LPCMDPKT lpCmdPkt);
LPVOID GetLoadOptions(LPCMDPKT lpCmdPkt);
LPCMSINFO GetLoadCMSInfo(LPCMDPKT lpCmdPkt);
void SetLoadCMSInfo(LPCMDPKT lpCmdPkt, LPCMSINFO lpCmsInfo);
void SetLoadFileName(LPCMDPKT lpCmdPkt, LPTSTR lpFileName, ITEMID idFileType);
BOOL WritePacket(LPCMDPKT lpCmdPkt, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
LPCMDPKT ReadPacket(MACRO_FILE_HANDLE fh, LPTSTR lpCommand, LPBOOL lpError);
int ReadParms(MACRO_FILE_HANDLE fh, LPTSTR lpCommand, LPPVOID lppParms);
LPTSTR 	GetCommandName(LPTSTR lpCommand, LPTSTR lpName);
BOOL GetCommandString(LPCMDPKT lpCmdPkt, LPTSTR lpCommand, int iMaxStrLen, BOOL fNameOnly);
void FreeUpParms(ITEMID idCommand, LPVOID lpParms);
BOOL ExportFile(LPTSTR lpSrcFile, LPTSTR lpDstDir, LPINT lpFileNum, LPINT lpCopies);
void FixExportFileNames(LPTSTR lpCurrentFile, LPLIST lpPacketList);
void FixExportFileName(LPTSTR lpCurrentFile, LPTSTR lpFileName);
void SetExportFileName(LPTSTR lpFileName);
BOOL EnableLogging(BOOL fEnable);
#endif // _CMDLIST_H_



