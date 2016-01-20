//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _CURSORS_H_
#define _CURSORS_H_


// the number of levels of clock possible.
#define MAX_CLOCK_DEPTH 25

typedef struct _clockcycle{
	int from;			// the (local) percent that the pass starts at.
	int to;				// the (local) percent that the pass ends at.
	BOOL bPercentSet;	// were the above values just set.
	int pass;			// the current pass.
	int nPasses;   		// the total number of passes pass.
	int msgID;			// the message id for the pass.
} CLOCKCYCLE, FAR *LPCLOCKCYCLE;

typedef void ( FAR *LPPROGRESSUPDATEPROC )( struct _progress FAR *lpProgress );

typedef struct _progress{
	int		cycle;
	int 	idProgressMsg;
	ULONG	ClockFrom;
	ULONG	ClockTo;
	ULONG	ClockTotal;
	int		Percentage;
	int		clock;
	int 	last_clock;
	BOOL	bEscapable;
	BOOL	bEscape;
	BOOL	fNewCycle;
	BOOL	bChanged;
	HWND 	hOwner;
	BOOL 	bDisplayClock; 	// for beachball
	LPPROGRESSUPDATEPROC lpProgressUpdateProc; // the plug-in progress proc
	CLOCKCYCLE	Cycles[MAX_CLOCK_DEPTH];
	} PROGRESS, FAR *LPPROGRESS;

typedef struct _cursordata
	{
	long	LastTime;
	short	times;
	BOOL	bCursorEnable;
	HCURSOR hCursor;
	} CURSORDATA, FAR *LPCURSORDATA;


BOOL AstralCursorEnable(BOOL bEnable);
VOID AstralCursor( LPCTSTR lpCursor );
BOOL AstralClockCursor( int part, int whole, BOOL bEscapable );
VOID AstralBeachCursor(ITEMID idCurs);
void ProgressInit(LPPROGRESS lpProgress);
LPPROGRESSUPDATEPROC ProgressSetUpdateProc(LPPROGRESSUPDATEPROC lpUpdateProc = NULL);
void ProgressBegin(int nNumPassesParam, ITEMID idProgressMsgParam, HWND hOwner = NULL, LPPROGRESSUPDATEPROC lpUpdateProc = NULL, BOOL bDisplayClock=TRUE);
void ProgressSetupPass(int nPassPercentParam, ITEMID idProgressMsgParam);
void ProgressEnd(BOOL bDisplayClock=TRUE);
void ProgressUpdate(LPPROGRESS lpProgress );
void CursorDataInit(LPCURSORDATA lpCursorData);
void SetWindowCursor(ITEMID idCursor, HCURSOR hCursor = NULL);
void SetWindowCursorHandle(HCURSOR hNewCursor);
VOID SetClockCursor(LPPROGRESS lpProgress, BOOL fOnlyIfChanged);

#endif // _CURSORS_H_

