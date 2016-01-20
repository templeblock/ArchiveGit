//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _CMDMACRO_H_
#define _CMDMACRO_H_

typedef struct _queueitem
{
	LISTLINK	Link;
	FNAME		szFileName;
	CMSINFO		cmsInfo;
	int			PhotoCDResOverride;
	LOADFILE_PARMS parms;
	LIST		MacroList;
	LIST		PacketList;
} QUEUEITEM, FAR *LPQUEUEITEM;

typedef struct _macroitem
{
	LISTLINK	Link;
	EXTNAME		szMacro;
} MACROITEM, FAR *LPMACROITEM;

BOOL PlayMacro(LPCMDLIST lpCmdList, LPTSTR lpFileName, int nRepeat, BOOL fSequenceAll, LPLIST lpMacroList = NULL, HWND hParent = NULL, int PhotoCDResOverride = 0, LPTSTR lpMacroName = NULL);
void PlayBatchMacro(LPLIST lpQueueList);
BOOL RecordMacro(LPTSTR lpFileName);
BOOL ReadMacro(LPTSTR lpFileName, LPLIST lpMacroList, BOOL fReadFirstPacketOnly = FALSE);
BOOL ReadMacro(HPTR hpBuf, LPLIST lpMacroList, BOOL fReadFirstPacketOnly = FALSE);
BOOL StopMacro(LPTSTR lpFileName);
LPABORTPROC MacroSetAbortProc(LPABORTPROC lpAbortProc = NULL);
BOOL MacroAnyLoadCommands(LPTSTR lpFileName);
BOOL MacroAnyLoadCommands(LPLIST lpPacketList);
BOOL IsPlaying( void );
BOOL IsRecording( void );
BOOL IsMacroRecording();
BOOL IsMacroPlaying();
BOOL MacroWritePacket(LPCMDPKT lpCmdPkt);
BOOL ConvertLowResLoad(LPLIST lpPktList);
void DestroyPtrList(LPLIST lpList);

#endif // _CMDMACRO_H_


