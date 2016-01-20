/*®PL1¯®FD1¯®BT0¯®TP0¯*/
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "macros.h"
#include "svrview.h"
#include "id.h"

extern BOOL       fAppActive;
extern BOOL       fCursorInActiveWindow;

static LPPROGRESS lpTheProgress = NULL;
static LPCURSORDATA lpTheCursorData = NULL;

LOCAL void SetupClockTotals(LPPROGRESS lpProgress);
LOCAL int GetProgressMsg(LPPROGRESS lpProgress);
LOCAL LPPROGRESS GetProgressPtr();
LOCAL LPCURSORDATA GetCursorDataPtr();
LOCAL BOOL IsCursorEnabled();
LOCAL void ProgressPass();
#ifdef WIN32
LOCAL BOOL ThreadForActiveImage();
#endif

/***********************************************************************/
// 	This routines initializes the progress memory structure.
/***********************************************************************/
void ProgressInit(LPPROGRESS lpProgress)
/***********************************************************************/
{
	if (lpProgress)
	{
		lpProgress->cycle = -1;
		lpProgress->idProgressMsg = 0;
		lpProgress->last_clock = -1;
	   	lpProgress->bDisplayClock = TRUE;
	}
}

/***********************************************************************/
void CursorDataInit(LPCURSORDATA lpCursorData)
/***********************************************************************/
{
	if (lpCursorData)
	{
		lpCursorData->bCursorEnable = YES;
	}
}

/***********************************************************************/
LPPROGRESSUPDATEPROC ProgressSetUpdateProc(LPPROGRESSUPDATEPROC lpUpdateProc)
/***********************************************************************/
{
	LPPROGRESS lpProgress;
	LPPROGRESSUPDATEPROC lpOldProc;

	if (!(lpProgress = GetProgressPtr()))
		return(NULL);
	lpOldProc = lpProgress->lpProgressUpdateProc;
	if (lpUpdateProc)
		lpProgress->lpProgressUpdateProc = lpUpdateProc;
	else
		lpProgress->lpProgressUpdateProc = ProgressUpdate;
	return(lpOldProc);
}

/***********************************************************************/
// 	This routine begins a clock cycle.
//	If there is already a cycle in progress then it will be a subcycle.
//	If hWnd != NULL then the cursor cues off the window rather than 
//		the active image. (only if in the outer loop)
/***********************************************************************/
void ProgressBegin(int nNumPassesParam, int idProgressMsgParam,
					HWND hOwner, LPPROGRESSUPDATEPROC lpUpdateProc,  
					BOOL bDisplayClock/*TRUE*/)
/***********************************************************************/
{
	LPPROGRESS lpProgress;
	int cycle;

	if ( !IsCursorEnabled() )
		return;

	if (!(lpProgress = GetProgressPtr()))
		return;

	if (lpProgress->cycle < 0)
		{
		lpProgress->clock = -2;
		lpProgress->last_clock = -1;
		lpProgress->Percentage = 0;
		lpProgress->bEscapable = NO;
		lpProgress->bEscape = NO;
		lpProgress->hOwner = hOwner;
		if (!lpProgress->lpProgressUpdateProc)
			lpProgress->lpProgressUpdateProc = ProgressUpdate;
			
//		if (lpUpdateProc)
//			lpProgress->lpProgressUpdateProc = lpUpdateProc;
//		else
//			lpProgress->lpProgressUpdateProc = ProgressUpdate;
		if (Control.UseThreading)
			{
			AddTaskItem(GetThreadData());
			if (ThreadForActiveImage())
				AstralCursor(IDC_WAIT);
			else
				{
				LPCURSORDATA lpCursorData = GetCursorDataPtr();
				if (lpCursorData)
					lpCursorData->hCursor = GetCursor();
				}
				
				
			}
		else
			AstralCursor(IDC_WAIT);
		}
	lpProgress->idProgressMsg = idProgressMsgParam;

	// previous cycle moves ahead one step
	ProgressPass();
	lpProgress->cycle++;

	ASSERT(lpProgress->cycle < MAX_CLOCK_DEPTH);

	if (lpProgress->cycle >= MAX_CLOCK_DEPTH)
		lpProgress->cycle--; 
	if (!nNumPassesParam)
		nNumPassesParam = 1;
	cycle = lpProgress->cycle;
	lpProgress->Cycles[cycle].from = 0;
	lpProgress->Cycles[cycle].to = 100/nNumPassesParam;
	lpProgress->Cycles[cycle].nPasses = nNumPassesParam;
	lpProgress->Cycles[cycle].bPercentSet = TRUE;
	lpProgress->Cycles[cycle].pass = 0;
	lpProgress->Cycles[cycle].msgID = idProgressMsgParam;
	SetupClockTotals(lpProgress);
	lpProgress->idProgressMsg = GetProgressMsg(lpProgress);
 	lpProgress->bDisplayClock = bDisplayClock;
	lpProgress->bChanged = TRUE;

	if (lpProgress->idProgressMsg)
		(*lpProgress->lpProgressUpdateProc)(lpProgress);
}

//***********************************************************************/
// 	This routine sets the percent and message for the current pass
//		within the current cycle.
//	If nPassPercentParam==0 then only the message is changed.
//	if nPassPercentParam==-1 then the remaining space is split up evenly.
/***********************************************************************/
void ProgressSetupPass(int nPassPercentParam, int idProgressMsgParam)
/***********************************************************************/
{
	LPPROGRESS lpProgress;
	int cycle;

	if (!(lpProgress = GetProgressPtr()))
		return;

	lpProgress->fNewCycle = YES;
	cycle = lpProgress->cycle;
	if (nPassPercentParam && cycle>=0)
	{
		if (nPassPercentParam>=0 || !lpProgress->Cycles[cycle].bPercentSet)
		{
			if (!lpProgress->Cycles[cycle].bPercentSet)
 	  			lpProgress->Cycles[cycle].from = lpProgress->Cycles[cycle].to;
			lpProgress->Cycles[cycle].bPercentSet = TRUE;
			if (nPassPercentParam<0)
			{
				nPassPercentParam = lpProgress->Cycles[cycle].nPasses-lpProgress->Cycles[cycle].pass;
				lpProgress->Cycles[cycle].bPercentSet = FALSE;
				if (nPassPercentParam)
					nPassPercentParam = (100-lpProgress->Cycles[cycle].to)/nPassPercentParam;
			}
			lpProgress->Cycles[cycle].to = lpProgress->Cycles[cycle].from+nPassPercentParam;
			if (lpProgress->Cycles[cycle].to > 100)
				lpProgress->Cycles[cycle].to = 100;
			lpProgress->Cycles[cycle].pass++;
			SetupClockTotals(lpProgress);
		}
		else 
			lpProgress->Cycles[cycle].bPercentSet = FALSE;
		
	}
	if (idProgressMsgParam)
	{
		if (lpProgress->Cycles && cycle>=0)
			lpProgress->Cycles[cycle].msgID = idProgressMsgParam;
	}
	lpProgress->idProgressMsg = GetProgressMsg(lpProgress);
}

//***********************************************************************/
/***********************************************************************/
LOCAL void ProgressPass()
/***********************************************************************/
{
	LPPROGRESS lpProgress;
	int cycle;
	int nPassPercentParam;

	if (!(lpProgress = GetProgressPtr()))
		return;

	lpProgress->fNewCycle = YES;
	cycle = lpProgress->cycle;
	if (cycle>=0)
	{
		if (!lpProgress->Cycles[cycle].bPercentSet)
		{
			lpProgress->Cycles[cycle].bPercentSet = TRUE;
			nPassPercentParam = lpProgress->Cycles[cycle].nPasses-lpProgress->Cycles[cycle].pass-1;
			lpProgress->Cycles[cycle].bPercentSet = FALSE;
			if (nPassPercentParam)
				nPassPercentParam = (100-lpProgress->Cycles[cycle].to)/nPassPercentParam;
			if (!lpProgress->Cycles[cycle].bPercentSet)
				lpProgress->Cycles[cycle].from = lpProgress->Cycles[cycle].to;
			lpProgress->Cycles[cycle].to = lpProgress->Cycles[cycle].from+nPassPercentParam;
			if (lpProgress->Cycles[cycle].to > 100)
				lpProgress->Cycles[cycle].to = 100;
			lpProgress->Cycles[cycle].pass++;
			SetupClockTotals(lpProgress);
		}
		else 
			lpProgress->Cycles[cycle].bPercentSet = FALSE;
		
	}
	lpProgress->idProgressMsg = GetProgressMsg(lpProgress);
}

//***********************************************************************/
// 	This routine ends a clock lpProgress->cycle.
//	If there is already a lpProgress->cycle in progress then it will just push
//		up one level to the higher lpProgress->cycle.
//***********************************************************************/
void ProgressEnd( BOOL bDisplayClock/*TRUE*/)
/***********************************************************************/
{
	LPPROGRESS lpProgress;

	if ( !IsCursorEnabled() )
		return;
	if (!(lpProgress = GetProgressPtr()))
		return;
	ASSERT(lpProgress->cycle >= 0);

	if (lpProgress->cycle < 0)
		return;

	lpProgress->cycle--;
	if (lpProgress->cycle<0)
	{
//		FreeUp((LPTR)lpProgress->Cycles);
//		lpProgress->Cycles = NULL;
#ifdef WIN32
		if (Control.UseThreading)
			{
			DeleteTaskItem(GetThreadData());
			if (ThreadForActiveImage())
				{
				AstralCursor(NULL);
				if (AstralDlgGet(IDD_TASKMANAGER) == NULL)
					MessageStatus(0);
				}
			else
				{
				LPCURSORDATA lpCursorData = GetCursorDataPtr();
				if (lpCursorData)
					lpCursorData->hCursor = NULL;
				}
			}
#else
		AstralCursor(NULL);
		MessageStatus(0);
#endif
		lpProgress->idProgressMsg = 0;
		return;
	}
	lpProgress->idProgressMsg = GetProgressMsg(lpProgress);
 	lpProgress->bDisplayClock = bDisplayClock;
}

/***********************************************************************/
BOOL AstralCursorEnable( BOOL bEnable )
/***********************************************************************/
{
BOOL	bOldEnable;
LPCURSORDATA lpCursorData;

if (!(lpCursorData = GetCursorDataPtr()))
	return(YES);
bOldEnable = lpCursorData->bCursorEnable;
lpCursorData->bCursorEnable = bEnable;
return(bOldEnable);
}

/***********************************************************************/
VOID AstralCursor( LPCSTR lpCursor )
/***********************************************************************/
/* Displays lpCursor - if 0 is passed, the previous cursor is displayed */
/***********************************************************************/
{
LPCURSORDATA lpCursorData;

#ifndef SILENT
if ( !fAppActive && !PictPubApp.IsActiveDocInPlaceActive())
	return;
#endif

if (!(lpCursorData = GetCursorDataPtr()))
	return;
if ( !lpCursorData->bCursorEnable)
	return;

// Display the cursor identified by the id
/* If the caller didn't pass a cursor handle, use the previous one */
if ( !lpCursor )
	{
	if ( lpCursorData->hCursor != 0 )
		{
		SetCursor( lpCursorData->hCursor );
		lpCursorData->hCursor = NULL;
		}
	}
else	
	lpCursorData->hCursor = SetCursor( LoadCursor( NULL, lpCursor ) );
}

/***********************************************************************/
BOOL FAR AstralClockCursor( int part, int whole, BOOL bEscapable )
/***********************************************************************/
{
	int Percentage;
	long lWhole, lPart;
	BOOL bSimple;
	LPPROGRESS lpProgress;
	
#ifdef SILENT
	return( bEscapable && CANCEL );
#else

	// if the apps not active get out
	if ( !fAppActive && !PictPubApp.IsActiveDocInPlaceActive())
		return( FALSE );
	
	// get pointer to progress data
	if (!(lpProgress = GetProgressPtr()))
		return(FALSE);

	// if escapable and task manager told us to escape
	if (bEscapable && lpProgress->bEscape)
		return(TRUE);

	// if escapable and the user hit the escape key
	if ( bEscapable && CANCEL )
		return( TRUE );
	
	// see if cursors are turned off
	if ( !IsCursorEnabled() )
		return(FALSE);

	// This would be nicer if we could set escapable in
	// ProgressBegin(), however lots of code would
	// have to change
	lpProgress->bEscapable = bEscapable;

	// calculate Percentage and clock
	bSimple = (lpProgress->cycle < 0);
	if (bSimple)
	{
		Percentage = FMUL( 100, FGET( part, whole ) );
	}
	else
	{
		if (!part && !lpProgress->Cycles[lpProgress->cycle].bPercentSet)
			ProgressPass();
		lWhole = lpProgress->ClockTotal;
		if (!whole)
			lPart = 0L;
		else
			lPart = lpProgress->ClockFrom + ((lpProgress->ClockTo-lpProgress->ClockFrom)*part)/whole;
		Percentage = ((100*lPart)+(lWhole>>1))/lWhole;
	}

	lpProgress->bChanged = (Percentage != lpProgress->Percentage);
	lpProgress->Percentage = Percentage;

	(*lpProgress->lpProgressUpdateProc)(lpProgress);

	return( FALSE );
#endif
}

/***********************************************************************/
void ProgressUpdate(LPPROGRESS lpProgress)
/***********************************************************************/
{
	BOOL bTaskManager = NO;

#ifdef WIN32
  	// call the task manager
	if (Control.UseThreading)
	{
		// only call task manager when percentage changes
		if (lpProgress->bChanged)
			ModifyTaskItem(GetThreadData());
	}
	lpProgress->fNewCycle = NO;
	// if this progress doesn't have an image yet or the image
	// is not the active image, get out
	if (Control.UseThreading && !ThreadForActiveImage())
 		return;
	bTaskManager = AstralDlgGet(IDD_TASKMANAGER) != NULL;
#else
	lpProgress->fNewCycle = NO;
#endif

	if (!bTaskManager && lpProgress->bChanged)
	{
		if (lpProgress->idProgressMsg)
			MessageStatus( lpProgress->idProgressMsg, lpProgress->Percentage);
		else
			MessageStatus( (lpProgress->bEscapable ? PROGRESS_ID(IDS_UNDO1) : PROGRESS_ID(IDS_UNDO2)),
				lpProgress->Percentage );
	}
	if (lpProgress->bDisplayClock)
		SetClockCursor(lpProgress, TRUE);
}

/***********************************************************************/
VOID SetClockCursor(LPPROGRESS lpProgress, BOOL fOnlyIfChanged)
/***********************************************************************/
{
int idCursor, clock;

if (!lpProgress)
	return;
clock = FMUL( 12, FGET( lpProgress->Percentage, 100 ) );
if (clock < 0)
	return;
if ( clock == lpProgress->last_clock && fOnlyIfChanged)
	return;
lpProgress->last_clock = clock;

switch( clock )
 {
    case 0:  idCursor = ID_CLOCK12; break;
    case 1:  idCursor = ID_CLOCK01; break;
    case 2:  idCursor = ID_CLOCK02; break;
    case 3:  idCursor = ID_CLOCK03; break;
    case 4:  idCursor = ID_CLOCK04; break;
    case 5:  idCursor = ID_CLOCK05; break;
    case 6:  idCursor = ID_CLOCK06; break;
    case 7:  idCursor = ID_CLOCK07; break;
    case 8:  idCursor = ID_CLOCK08; break;
    case 9:  idCursor = ID_CLOCK09; break;
    case 10: idCursor = ID_CLOCK10; break;
    case 11: idCursor = ID_CLOCK11; break;
    case 12: idCursor = ID_CLOCK12; break;
    default: idCursor = ID_CLOCK12; break;
 }
// Set the new cursor and show it
SetCursor( LoadCursor( PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(idCursor) ) );
}

/***********************************************************************/
VOID AstralBeachCursor(int idCurs)
/***********************************************************************/
{
int            idCursor;
long           CurrentTime;
LPCURSORDATA   lpCursorData;

if (!(lpCursorData = GetCursorDataPtr()))
	return;

#ifndef SILENT
if ( !fAppActive && !PictPubApp.IsActiveDocInPlaceActive())
	return;
#endif

if ( !lpCursorData->bCursorEnable)
	return;

CurrentTime = GetCurrentTime();
if ((CurrentTime - lpCursorData->LastTime) > 150L)
	{
	lpCursorData->LastTime = CurrentTime;
	idCursor = idCurs + (lpCursorData->times++ % 4);
	SetCursor( LoadCursor( PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(idCursor) ) );
	}
}


//***********************************************************************
//	setup  ClockFrom, ClockTo & ClockTotal.
//***********************************************************************
LOCAL void SetupClockTotals(LPPROGRESS lpProgress)
//***********************************************************************
{
	int i;
	unsigned long value;
	
	value = lpProgress->ClockTotal = 0x00010000;
	lpProgress->ClockFrom = 0; 
	lpProgress->ClockTo = lpProgress->ClockTotal;
 	if (!lpProgress->Cycles || lpProgress->cycle<0)
		return;
	for (i=0; i<=lpProgress->cycle; i++)
	{
		lpProgress->ClockFrom += (lpProgress->Cycles[i].from*value)/100;
		lpProgress->ClockTo = lpProgress->ClockFrom+((lpProgress->Cycles[i].to-lpProgress->Cycles[i].from)*value)/100;
		value = lpProgress->ClockTo-lpProgress->ClockFrom;
		if (value<=0)
		{
			lpProgress->ClockTo = lpProgress->ClockFrom;
			break;
		}
	}
	return;
}

//***********************************************************************
//	returns the message id for the message that should be displayed.
//	returns 0 for the default.
//***********************************************************************
LOCAL int GetProgressMsg(LPPROGRESS lpProgress)
//***********************************************************************
{
	int i, id;
	
	id = 0;
 
 	if (!lpProgress->Cycles || lpProgress->cycle<0)
		return(lpProgress->idProgressMsg);	

	for (i=0; i<=lpProgress->cycle; i++)
//	for (i=lpProgress->cycle; i>=0; i--)
		if (id=lpProgress->Cycles[i].msgID)
			break;
	return(id);
}

//***********************************************************************
//	returns whether or not cursors are enabled
//***********************************************************************
LOCAL BOOL IsCursorEnabled()
//***********************************************************************
{
LPCURSORDATA lpCursorData;

if (!(lpCursorData = GetCursorDataPtr()))
	return(TRUE);
return(lpCursorData->bCursorEnable);
}

//***********************************************************************
//	returns the progress pointer for the progress functions to use.
//***********************************************************************
LOCAL LPPROGRESS GetProgressPtr()
//***********************************************************************
{
#ifdef WIN32
LPTHREADDATA lpThreadData;
if (Control.UseThreading)
	{
	lpThreadData = GetThreadData();
	if (lpThreadData)
		return(&lpThreadData->Progress);
	else
		return(NULL);
	}
#endif
if (!lpTheProgress)
	{
	lpTheProgress = (LPPROGRESS)Alloc(sizeof(PROGRESS));
	if (lpTheProgress)
		{
		clr((LPTR)lpTheProgress, sizeof(PROGRESS));
		ProgressInit(lpTheProgress);
		}
	}
return(lpTheProgress);
}

//***********************************************************************
//	returns the progress pointer for the progress functions to use.
//***********************************************************************
LOCAL LPCURSORDATA GetCursorDataPtr()
//***********************************************************************
{
#ifdef WIN32
LPTHREADDATA lpThreadData;
if (Control.UseThreading)
	{
	lpThreadData = GetThreadData();
	if (lpThreadData)
		return(&lpThreadData->CursorData);
	else
		return(NULL);
	}
#endif
if (!lpTheCursorData)
	{
	lpTheCursorData = (LPCURSORDATA)Alloc(sizeof(CURSORDATA));
	if (lpTheCursorData)
		{
		clr((LPTR)lpTheCursorData, sizeof(CURSORDATA));
		CursorDataInit(lpTheCursorData);
		}
	}
return(lpTheCursorData);
}

#ifdef WIN32
//***********************************************************************
//	returns whether this thread is for the active image.
//***********************************************************************
LOCAL BOOL ThreadForActiveImage()
//***********************************************************************
{
LPTHREADDATA lpThreadData;
LPIMAGE lpImage;

if (GetCurrentThreadId() == PictPubApp.GetMainThreadId())
	return(TRUE);

if (!(lpThreadData = GetThreadData()))
	return(FALSE);
// Handle modal dialog case
if (lpThreadData->Progress.hOwner)
{
	HWND hActive;
//	POINT point; 
//	RECT rect;

	hActive = GetActiveWindow();
	if (hActive != lpThreadData->Progress.hOwner)
		return(FALSE);
//	if (!GetCursorPos(&point))
//		return(FALSE);
//	if (!GetWindowRect(hActive, &rect))
//		return(FALSE);
//	return(PtInRect(&rect, point));
	return(TRUE);
}

if (fCursorInActiveWindow)
	{	    
	CServerView *pView = PictPubApp.GetActiveView();
	return(pView && (pView->GetDocument() == lpThreadData->pDocument));
	}
else
	return(FALSE);
}
#endif

//***********************************************************************
//	Sets the current image's window's cursor to the cursor given.
//***********************************************************************
void SetWindowCursor(ITEMID idCursor, HCURSOR hCursor)
//***********************************************************************
{
	HCURSOR hOldCursor;
	POINT CursorPos;
	HWND hActiveWnd;
	
	hActiveWnd = GetActiveDoc();
	hOldCursor = Window.hCursor;
	
	if (hCursor)
		Window.hCursor = hCursor;
	else
	{
		if (idCursor > LAST_CURSOR)
	     	Window.hCursor = LoadCursor( NULL, MAKEINTRESOURCE(idCursor) );
		else Window.hCursor = LoadCursor( PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(idCursor) );
	}
	if ( hActiveWnd )
	{
		SetClassCursor( hActiveWnd, Window.hCursor );
		if (hOldCursor != Window.hCursor)
		{
			GetCursorPos( &CursorPos );
			ScreenToClient(hActiveWnd, &CursorPos );
			if (IsOnImage( hActiveWnd, CursorPos.x, CursorPos.y ) )
				SetCursor(Window.hCursor);
		}
	}
}												  




