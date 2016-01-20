/*®PL1¯®FD1¯®BT0¯®TP0¯*/
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "types.h"
#include "routines.h"
#include "macros.h"
#include "id.h"

// From routines.h
WORD MessageStatus( int idStr, ... );

// the number of levels of clock possible.
#define MAX_CLOCK_DEPTH 10		

typedef struct _clockcycle{
	int from;			// the (local) percent that the pass starts at.
	int to;				// the (local) percent that the pass ends at.
	BOOL bPercentSet;	// were the above values just set.
	int pass;			// the current pass.
	int nPasses;   		// the total number of passes pass.
	int msgID;			// the message id for the pass.
} CLOCKCYCLE;
typedef CLOCKCYCLE _far *LPCLOCKCYCLE;

extern BOOL fAppActive;
extern HINSTANCE hInstAstral;
extern HWND hWndAstral;

static BOOL bCursorEnable = YES;
static LPCLOCKCYCLE lpCycles = NULL;
static int  cycle = -1;				// The depth in the clock pass list
static int idProgressMsg = 0;
static unsigned long ClockFrom, ClockTo, ClockTotal;

static void SetupClockTotals(void);
static int GetProgressMsg(void);

/***********************************************************************/
// 	This routine begins a clock cycle.
//	If there is already a cycle in progress then it will be a subcycle.
/***********************************************************************/
void ProgressBegin(int nNumPassesParam, int idProgressMsgParam)
/***********************************************************************/
{
	if ( !bCursorEnable)
		return;
	if (cycle < 0)
		AstralCursor(IDC_WAIT);
	if (!lpCycles)
	{
		lpCycles = (LPCLOCKCYCLE)Alloc(MAX_CLOCK_DEPTH*sizeof(CLOCKCYCLE));
	}
	idProgressMsg = idProgressMsgParam;
	if (!lpCycles)
	{
		SetupClockTotals();
		return;
	}
	// previous cycle moves ahead one step
	ProgressSetupPass(-1,0);
	cycle++;
	if (cycle >= MAX_CLOCK_DEPTH)
		cycle--;
	if (cycle < 0)
		cycle = 0;
	if (!nNumPassesParam)
		nNumPassesParam = 1;
	lpCycles[cycle].from = 0;
	lpCycles[cycle].to = 100/nNumPassesParam;
	lpCycles[cycle].nPasses = nNumPassesParam;
	lpCycles[cycle].bPercentSet = TRUE;
	lpCycles[cycle].pass = 0;
	lpCycles[cycle].msgID = idProgressMsgParam;
	SetupClockTotals();
	idProgressMsg = GetProgressMsg();
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
	if ( !bCursorEnable)
		return;
	if (nPassPercentParam && lpCycles && cycle>=0)
	{
		if (nPassPercentParam>=0 || !lpCycles[cycle].bPercentSet)
		{
			lpCycles[cycle].bPercentSet = TRUE;
			if (nPassPercentParam<0)
			{
				nPassPercentParam = lpCycles[cycle].nPasses-lpCycles[cycle].pass-1;
				lpCycles[cycle].bPercentSet = FALSE;
			}
			if (nPassPercentParam)
				nPassPercentParam = (100-lpCycles[cycle].to)/nPassPercentParam;
			if (!lpCycles[cycle].bPercentSet)
				lpCycles[cycle].from = lpCycles[cycle].to;
			lpCycles[cycle].to = lpCycles[cycle].from+nPassPercentParam;
			if (lpCycles[cycle].to > 100)
				lpCycles[cycle].to = 100;
			lpCycles[cycle].pass++;
			SetupClockTotals();
		}
		else 
			lpCycles[cycle].bPercentSet = FALSE;
		
	}
	if (idProgressMsgParam)
	{
		if (lpCycles && cycle>=0)
			lpCycles[cycle].msgID = idProgressMsgParam;
	}
	idProgressMsg = GetProgressMsg();
}

//***********************************************************************/
// 	This routine ends a clock cycle.
//	If there is already a cycle in progress then it will just push
//		up one level to the higher cycle.
//***********************************************************************/
void ProgressEnd()
/***********************************************************************/
{
	if ( !bCursorEnable)
		return;
	cycle--;
	if (cycle<0)
	{
		FreeUp((LPTR)lpCycles);
		lpCycles = NULL;
		AstralCursor(NULL);
		MessageStatus(NULL);
		idProgressMsg = 0;
		return;
	}
	idProgressMsg = GetProgressMsg();
}

/***********************************************************************/
BOOL AstralCursorEnable(BOOL bEnable)
/***********************************************************************/
{
BOOL	bOldEnable;

bOldEnable = bCursorEnable;
bCursorEnable = bEnable;
return(bOldEnable);
}

/***********************************************************************/
VOID AstralCursor(LPCSTR lpCursor)
/***********************************************************************/
/* Displays lpCursor - if 0 is passed, the previous cursor is displayed */
/***********************************************************************/
{
static HCURSOR hCursor = 0;

#ifndef SILENT
if ( !fAppActive )
	return;
#endif

if ( !bCursorEnable )
	return;

// Display the cursor identified by the id
/* If the caller didn't pass a cursor handle, use the previous one */
if ( !lpCursor )
	{
	if ( hCursor != 0 )
		SetCursor( hCursor );

    // the following block of code was added to flush click-ahead messages
    {
    MSG msg,last_upmsg;

    last_upmsg.message = 0;
    while (PeekMessage (&msg,hWndAstral,WM_LBUTTONDOWN,WM_MOUSELAST,PM_REMOVE))
        {
        if (msg.message == WM_LBUTTONUP)
            last_upmsg = msg;   // preserve one WM_LBUTTONUP message
        }
    if (last_upmsg.message)
        PostMessage (last_upmsg.hwnd,last_upmsg.message,last_upmsg.wParam,
            last_upmsg.lParam);
    }
	}
else
    hCursor = (lpCursor == IDC_WAIT) ? GetCursor() :
        SetCursor( LoadCursor( NULL, lpCursor ) );
}


/***********************************************************************/
BOOL AstralClockCursor(int part, int whole, BOOL bEscapable )
/***********************************************************************/
{
	int idCursor, clock, Percentage;
	long lWhole, lPart;
	BOOL bSimple;
	
	static int last_clock = -1;
	
#ifndef NOTCRAYOLA
	bEscapable = NO; // don't allow this feature at all
#endif

#ifdef SILENT
	return( bEscapable && (CANCEL && !ALT && !CONTROL) );
#else
	
	if ( !bCursorEnable)
		return(FALSE);
	
	if ( !fAppActive )
		return( FALSE );
	
	if ( bEscapable && (CANCEL && !ALT && !CONTROL) )
		return( TRUE );
	
	bSimple = (cycle < 0 ||	!lpCycles);
	if (bSimple)
	{
		Percentage = FMUL( 100, FGET( part, whole ) );
		clock = FMUL( 12, FGET( part, whole ) );
	}
	else
	{
		if (!part && !lpCycles[cycle].bPercentSet)
			ProgressSetupPass(-1,0);
		lWhole = ClockTotal;
		if (!whole)
			lPart = 0L;
		else
			lPart = ClockFrom + ((ClockTo-ClockFrom)*part)/whole;
		Percentage = ((100*lPart)+(lWhole>>1))/lWhole;
		clock = ((12 * lPart)+(lWhole>>1))/lWhole;
	}
	
	if (idProgressMsg)
		MessageStatus( idProgressMsg, Percentage);
	else if ( part+1 >= whole )
		MessageStatus( NULL );
	else
		MessageStatus( (bEscapable ? IDS_PROGRESS1 : IDS_PROGRESS2),
			Percentage );
	
    IntermissionPlay();     // allow a small animation to play while waiting
	if ( clock == last_clock )
		return( FALSE );
	last_clock = clock;
	
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
	SetCursor( LoadCursor( hInstAstral, MAKEINTRESOURCE(idCursor) ) );
	return( FALSE );
#endif
}

/***********************************************************************/
VOID AstralBeachCursor(int idCurs)
/***********************************************************************/
{
static long LastTime;
static short times;
int idCursor;
long CurrentTime;

#ifndef SILENT
if ( !fAppActive )
	return;
#endif

if ( !bCursorEnable)
	return;

CurrentTime = GetCurrentTime();
if ((CurrentTime - LastTime) > 150L)
	{
	LastTime = CurrentTime;
	idCursor = idCurs + (times++ % 4);
	SetCursor( LoadCursor( hInstAstral, MAKEINTRESOURCE(idCursor) ) );
	}
}


//***********************************************************************
//	setup  ClockFrom, ClockTo & ClockTotal.
//***********************************************************************
static void SetupClockTotals(void)
//***********************************************************************
{
	int i;
	unsigned long value;
	
	value = ClockTotal = 0x00010000;
	ClockFrom = 0; 
	ClockTo = ClockTotal;
 	if (!lpCycles || cycle<0)
		return;
	for (i=0; i<=cycle; i++)
	{
		ClockFrom += (lpCycles[i].from*value)/100;
		ClockTo = ClockFrom+((lpCycles[i].to-lpCycles[i].from)*value)/100;
		value = ClockTo-ClockFrom;
		if (value<=0)
		{
			ClockTo = ClockFrom;
			break;
		}
	}
	return;
}

//***********************************************************************
//	returns the message id for the message that should be displayed.
//	returns 0 for the default.
//***********************************************************************
static int GetProgressMsg(void)
//***********************************************************************
{
	int i, id;
	
	id = 0;
 
 	if (!lpCycles || cycle<0)
		return(idProgressMsg);	

	for (i=cycle; i>=0; i--)
		if (id=lpCycles[i].msgID)
			break;
	return(id);
}

//***********************************************************************
//	Sets the current image's window's cursor to the cursor given.
//***********************************************************************
void SetWindowCursor(WORD idCursor)
//***********************************************************************
{
	HCURSOR hOldCursor;
	POINT CursorPos;
	
	hOldCursor = Window.hCursor;
	
	if (idCursor > LAST_CURSOR)
	     Window.hCursor = LoadCursor( NULL, MAKEINTRESOURCE(idCursor) );
	else Window.hCursor = LoadCursor( hInstAstral, MAKEINTRESOURCE(idCursor) );
	if ( lpImage && lpImage->hWnd )
	{
		SET_CLASS_CURSOR( lpImage->hWnd, Window.hCursor );
		if (hOldCursor != Window.hCursor)
		{
			if ( GetCapture() ==  lpImage->hWnd )
				SetCursor(Window.hCursor);
			else
			{
				GetCursorPos( &CursorPos );
				ScreenToClient(lpImage->hWnd, &CursorPos );
				if (ONIMAGE( CursorPos.x, CursorPos.y ) )
					SetCursor(Window.hCursor);
			}
		}
	}
}
