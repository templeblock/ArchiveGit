// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®FD1¯®PL1¯®TP0¯®BT0¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

#define DELAY 500L

typedef EVENTMSG FAR *LPEVENTMSG;

extern HANDLE hInstAstral;
extern HWND hWndAstral;

static FARPROC lpfnOldHook = NULL, lpfnMyHook = NULL;
static EVENTMSG EMsg;
static DWORD dwBaseTime;
static PgmMode = -1;
static FILE *fpMsg, *hFile;

/***********************************************************************/
DWORD FAR PASCAL Recorder( iCode, wParam, lpEM )
/***********************************************************************/
int	iCode;
WORD	wParam;
LPEVENTMSG lpEM;
{
static BOOL IsDown;

if ( iCode == HC_ACTION )
	{
	/* Keep track of the up/down state */
	if ( lpEM->message == WM_LBUTTONDOWN )
		IsDown |= 1;
	else
	if ( lpEM->message == WM_RBUTTONDOWN)
		IsDown |= 2;
	else
	if ( lpEM->message == WM_LBUTTONUP )
		IsDown &= 2;
	else
	if ( lpEM->message == WM_RBUTTONUP )
		IsDown &= 1;
	else
	if ( lpEM->message == WM_MOUSEMOVE )
	   if ( !IsDown ) /* Moves without mouse down are not stored away */
	      return( DefHookProc( iCode, wParam, (DWORD)lpEM, &lpfnOldHook ) );

	if ( !dwBaseTime )
		dwBaseTime = lpEM->time;

	lpEM->time -= dwBaseTime;
	WriteEvent( hFile, lpEM );
	lpEM->time += dwBaseTime;
	}
return( DefHookProc( iCode, wParam, (DWORD)lpEM, &lpfnOldHook ) );
}


/***********************************************************************/
BOOL InstallRecorder( pRecordFile )
/***********************************************************************/
PTR pRecordFile;
{
if ( hFile )
	fclose( hFile );
if ( !(hFile = fopen( pRecordFile, "wct" )) )
	return( FALSE );

PgmMode = WH_JOURNALRECORD;
dwBaseTime = 0; /* Zero out the base time; set it with the 1st message */
HookOn();
return( TRUE );
}


/***********************************************************************/
BOOL RemoveRecorder()
/***********************************************************************/
{
if ( PgmMode != WH_JOURNALRECORD )
	return( NO );

if ( hFile )
	{
	fclose( hFile );
	hFile = NULL;
	}

HookOff();
PgmMode = -1;
return( YES );
}


/***********************************************************************/
DWORD FAR PASCAL Player( iCode, wParam, lpEM )
/***********************************************************************/
int	iCode;
WORD	wParam;
LPEVENTMSG lpEM;
{
int i;
DWORD dwDelay;
static POINT cpt;
static int Moving;
#define SKIPCOUNT 4 /* Use 8 for a demo disk (faster) */

if ( iCode == HC_SKIP )
	{ /* Advance to the next event */
	if ( Moving ) /* haven't used the last event, so don't read a new one */
		return( 0L );

	if ( !EMsg.message && EMsg.paramH )
		{ // If the last message was a NULL message, put up the text
		if ( !DemoMessage() )
			{
			RemovePlayer();
			return( DefHookProc( iCode, wParam, (DWORD)lpEM,
				&lpfnOldHook));
			}
		}

	/* Get a new event from the file and store it in a static structure */
	if ( !ReadEvent( hFile, &EMsg ) )
		{
		RemovePlayer();
		return( DefHookProc( iCode, wParam, (DWORD)lpEM, &lpfnOldHook));
		}

	return( 0L );
	}

if ( iCode != HC_GETNEXT )
	return( DefHookProc( iCode, wParam, (DWORD)lpEM, &lpfnOldHook ) );

// Handle any automatic mouse movements
if ( EMsg.message > WM_MOUSEFIRST && EMsg.message <= WM_MOUSELAST )
	{ /* If this is a mouse message... */
	if ( !Moving ) // Start moving...
		GetCursorPos( &cpt );
	if ( cpt.x != EMsg.paramL || cpt.y != EMsg.paramH )
		{ /* Send a WM_MOUSEMOVE message to get closer */
		if ( !Moving )
			{
			BeginLine( cpt.x, cpt.y, EMsg.paramL, EMsg.paramH );
			Moving = YES;
			}
		i = SKIPCOUNT;
		while ( --i >= 0 )
			{ /* move several points at a time */
			GetNextPoint( &cpt.x, &cpt.y );
			if ( cpt.x == EMsg.paramL && cpt.y == EMsg.paramH )
				break;
			}
		lpEM->message = WM_MOUSEMOVE;
		lpEM->paramL = cpt.x;
		lpEM->paramH = cpt.y;
		lpEM->time = GetTickCount(); /* right now */
		return( 0L ); /* without delay */
		}
	}

/* Process the current event */
Moving = NO;
lpEM->message = EMsg.message;
lpEM->paramL = EMsg.paramL;
lpEM->paramH = EMsg.paramH;

dwDelay = GetTickCount() - dwBaseTime; /* Ticks since we started playing */
if ( dwDelay > EMsg.time )
	{ /* If it should have happened in past, */
	  /* bring it and future events up to date */
	dwBaseTime += (dwDelay-EMsg.time);
	dwDelay = EMsg.time;
	}
dwDelay = EMsg.time - dwDelay;
if ( dwDelay > DELAY )
	{ /* If it should have happened too far into */
	  /* the future process it a little bit sooner */
	dwBaseTime -= (dwDelay-DELAY);
	dwDelay = DELAY;
	}

lpEM->time = EMsg.time + dwBaseTime;
return( dwDelay );
}


/***********************************************************************/
BOOL InstallPlayer( pPlayFile )
/***********************************************************************/
PTR pPlayFile;
{
LPTR lp;
BYTE szMessageFile[MAX_FNAME_LEN];

lstrcpy( szMessageFile, pPlayFile );
if ( lp = strchr( szMessageFile, '.' ) )
	lstrcpy( lp, ".MSG" );
else	lstrcpy( szMessageFile, "DEMO.MSG" );

if ( hFile )
	fclose( hFile );
if ( !(hFile = fopen( pPlayFile, "r" )) )
	return( FALSE );
if ( fpMsg )
	fclose( fpMsg );
if ( !(fpMsg = fopen( szMessageFile, "r" )) )
	Print( "Couldn't open message file '%ls'", (LPTR)szMessageFile );

/* pre-read the first event message */
if ( !ReadEvent( hFile, &EMsg ) )
	{
	fclose( hFile );
	hFile = NULL;
	if ( fpMsg )
		fclose( fpMsg );
	fpMsg = NULL;
	return( FALSE );
	}

PgmMode = WH_JOURNALPLAYBACK;
dwBaseTime = GetTickCount(); /* Set the start time variable */
HookOn();
return( TRUE );
}


/***********************************************************************/
BOOL RemovePlayer()
/***********************************************************************/
{
if ( PgmMode != WH_JOURNALPLAYBACK )
	return( NO );
if ( hFile )
	{
	fclose( hFile );
	hFile = NULL;
	}
if ( fpMsg )
	{
	fclose( fpMsg );
	fpMsg = NULL;
	}
HookOff();
PgmMode = -1;
// For demo	SendMessage( hWndAstral, WM_CLOSE, 0, 0L);
return( YES );
}


/***********************************************************************/
static void HookOn()
/***********************************************************************/
{
if ( PgmMode != WH_JOURNALRECORD && PgmMode != WH_JOURNALPLAYBACK )
	return;

if ( lpfnMyHook == NULL ) /* Add the hook */
	{
	if ( PgmMode == WH_JOURNALPLAYBACK )
		lpfnMyHook = MakeProcInstance( (FARPROC)Player, hInstAstral );
	else
	if ( PgmMode == WH_JOURNALRECORD )
		lpfnMyHook = MakeProcInstance( (FARPROC)Recorder, hInstAstral);
	lpfnOldHook = SetWindowsHook( PgmMode, lpfnMyHook );
	}
}


/***********************************************************************/
static void HookOff()
/***********************************************************************/
{
if ( PgmMode != WH_JOURNALRECORD && PgmMode != WH_JOURNALPLAYBACK )
	return;

if ( lpfnMyHook != NULL ) /* Remove the hook */
	{
	UnhookWindowsHook( PgmMode, lpfnMyHook );
	FreeProcInstance( lpfnMyHook );
	lpfnMyHook = NULL;
	lpfnOldHook = NULL;
	}
}


/***********************************************************************/
BOOL DemoMessage()
/***********************************************************************/
{
int retc;
HWND hFocusWindow;
LPTR lp;

if ( PgmMode != WH_JOURNALPLAYBACK )
	return( YES );
if ( !fpMsg )
	return( YES );
lp = LineBuffer[3];
*lp = 0;
if ( fscanf( fpMsg, "%[^|]|", lp ) == EOF )
	return( YES );

if ( CAPSLOCK )
	return( YES );

/* Skip leading RETURNS */
while ( *lp && *lp <= ' ' )
	lp++;

hFocusWindow = GetFocus();
HookOff();
retc = MessageBox( hWndAstral, lp, "Picture Publisher Tutorial",
	MB_OKCANCEL | MB_TASKMODAL );
HookOn();
SetFocus( hFocusWindow );

if ( retc == IDCANCEL )
	return( NO );
return( YES );
}


static FIXED dx, dy, ax, ay;
static int px, py, ix, iy;

/************************************************************************/
void BeginLine( x1, y1, x2, y2 )
/************************************************************************/
int x1, y1, x2, y2;
{
int x, y;

if ( x1 > x2 )
	{ x = x1 - x2; ix = -1; }
else	{ x = x2 - x1; ix = 1; }

if ( y1 > y2 )
	{ y = y1 - y2; iy = -1; }
else	{ y = y2 - y1; iy = 1; }

if ( x < y )
	{ dy = UNITY; dx = FGET( x, y ); }
else	{ dx = UNITY; dy = FGET( y, x ); }

px = x1; ax = HALF;
py = y1; ay = HALF;
}


/************************************************************************/
void GetNextPoint( lpx, lpy )
/************************************************************************/
LPINT lpx, lpy;
{
if ( ( ax += dx ) > UNITY )
	{ ax -= UNITY; px += ix; }
if ( ( ay += dy ) > UNITY )
	{ ay -= UNITY; py += iy; }
*lpx = px;
*lpy = py;
}


/************************************************************************/
BOOL DemoRunning()
/************************************************************************/
{
return( PgmMode == WH_JOURNALRECORD || PgmMode == WH_JOURNALPLAYBACK );
}


/************************************************************************/
void Click()
/************************************************************************/
{
int f, d, nVoices;
//long l;

if ( !Control.Sound )
	return;
nVoices = OpenSound();
if ( nVoices == S_SERDVNA || nVoices == S_SEROFM ) /* if busy or no memory... */
	return;
if ( SetVoiceQueueSize( 1, 1000 ) )
	goto Exit;
f = 30;	/* set frequency 0-59 */
d = 1;	/* set duration in clock ticks */
/*
for ( l=1; l<65536L; l++ )
	{
	if ( SetVoiceSound( 1, l<<16, d ) )
		{
		Print("Didn't work at %ld", l);
		goto Exit;
		}
	StartSound();
	if ( WaitSoundState( S_QUEUEEMPTY ) )
		goto Exit;
	}
*/
if ( SetVoiceSound( 1, 440L+(f*2), d ) )
	goto Exit;
StartSound();
if ( WaitSoundState( S_QUEUEEMPTY ) )
	goto Exit;

Exit:
CloseSound();
StopSound();
}

#ifdef UNUSED
	/* SetVoiceEnvelope() SetSoundNoise() SyncAllVoices() */
	/* GetThresholdEvent() GetThresholdStatus() */
	/* CountVoiceNotes() SetVoiceThreshold() not used */
	OpenSound();
	q=SetVoiceQueueSize(1,1000);
	for (i=0; i < 60 ; i++)
	    {  /* change frequency */
	    for (j=0; j < 6 ; j++)
		{  /* change duration */
		Print("Set with %d,%ld,%d",q,440L*(i*2),j);
		SetVoiceSound(q,440L+(i*2),j);
		Print("Start");
		StartSound();
		Print("Wait");
		WaitSoundState(S_QUEUEEMPTY);
		}
	    }
	CloseSound();

	OpenSound();
	q=SetVoiceQueueSize(1,1000);
	SetVoiceAccent(1,100,255,0,0);
	SetVoiceNote(q,45,16,0); /* G# */
	StartSound();
	WaitSoundState(S_QUEUEEMPTY);
	CloseSound();
	StopSound();
#endif


/***********************************************************************/
BOOL FindAWindow( Point, lpString, iStringLen )
/***********************************************************************/
POINT Point;
LPTR lpString;
int iStringLen;
{
HWND hWnd, hLastWnd, hMatchWindow;
RECT Rect;

hWnd = hWndAstral;
hMatchWindow = NULL;
iStringLen--;

while ( hWnd )
	{
	GetWindowRect( hWnd, &Rect );
	if ( PtInRect( &Rect, Point ) /* && IsWindowVisible(hWnd)*/ )
		{ // Found a match, go deeper
		GetWindowText( hWnd, lpString, iStringLen );
//		Print("Found! Window %d (id=%d) sibling of %d - %ls",
//			hWnd, GetWindowWord( hWnd, GWW_ID ), hLastWnd,
//			lpString );
		hMatchWindow = hWnd;
		hLastWnd = hWnd;
		hWnd = GetWindow( hWnd, GW_CHILD );
		}
	else	{ // Not found, continue at the same level
		GetWindowText( hWnd, lpString, iStringLen );
//		Print("Window %d (id=%d) sibling of %d - %ls",
//			hWnd, GetWindowWord( hWnd, GWW_ID ), hLastWnd,
//			lpString );
		hLastWnd = hWnd;
		hWnd = GetWindow( hWnd, GW_HWNDNEXT );
		}
	}

if ( hMatchWindow )
	{
	GetWindowText( hMatchWindow, lpString, iStringLen );
	return( GetWindowWord( hMatchWindow, GWW_ID ) );
	}
else	return( NULL );
}


#ifdef UNUSED
/***********************************************************************/
int PrintAllChildren( hWnd, level )
/***********************************************************************/
HWND hWnd;
int level;
{
RECT Rect;
LPTR lp;
BYTE szString[MAX_IMAGE_LINE];
static FILE *fp;

if ( !level )
	{
	if ( !(fp = fopen( "pp.win", "wct" )) )
		return( FALSE );
	}
else	hWnd = GetWindow( hWnd, GW_CHILD );

lp = LineBuffer[0];
while ( hWnd )
	{
	GetWindowRect( hWnd, &Rect );
	clr( lp, MAX_STR_LEN );
	GetWindowText( hWnd, lp, MAX_STR_LEN-1 );
	wsprintf( szString, "%d: %ls (%d,%d)-(%d-%d)\r\n", level,
		lp, Rect.left, Rect.top, Rect.right, Rect.bottom );
	fprintf( fp, "%s", szString );
	PrintAllChildren( hWnd, level+1 );
	hWnd = GetWindow( hWnd, GW_HWNDNEXT );
	}

if ( !level )
	{
	fflush( fp );
	fclose( fp );
	}
}
#endif


/***********************************************************************/
static void WriteEvent( fp, lpEvent )
/***********************************************************************/
FILE *fp;
LPEVENTMSG lpEvent;
{
WORD c;
long l;
PTR ptr;
BYTE szString[MAX_STR_LEN];

switch( lpEvent->message )
	{
	case WM_KEYDOWN:
		ptr = "KD";
		break;
	case WM_KEYUP:
		ptr = "KU";
		break;
	case WM_SYSKEYDOWN:
		ptr = "SD";
		break;
	case WM_SYSKEYUP:
		ptr = "SU";
		break;
	case WM_MOUSEMOVE:
		ptr = "MO";
		break;
	case WM_LBUTTONDOWN:
		ptr = "LD";
		break;
	case WM_LBUTTONUP:
		ptr = "LU";
		break;
	case WM_LBUTTONDBLCLK:
		ptr = "L2";
		break;
	case WM_RBUTTONDOWN:
		ptr = "RD";
		break;
	case WM_RBUTTONUP:
		ptr = "RU";
		break;
	case WM_RBUTTONDBLCLK:
		ptr = "R2";
		break;
	default:
		ptr = itoa( lpEvent->message, szString, 10 );
	}

fprintf( fp, "%s (%d,%d) %lx",
	ptr, lpEvent->paramL, lpEvent->paramH, lpEvent->time );
if ( lpEvent->message == WM_KEYUP )
	{
	c = lpEvent->paramL;
	c = LOBYTE(c);
	if ( c > ' ' && c < 127 )
		fprintf( fp, "\t; %c\r\n", c );
	else	fprintf( fp, "\t; %s\r\n", itoa( c, szString, 10 ) );
	}
else
if ( lpEvent->message == WM_LBUTTONUP )
	{
	l = MAKELONG( lpEvent->paramL, lpEvent->paramH );
	clr( szString, sizeof(szString) );
	if ( FindAWindow( MAKEPOINT(l), szString, sizeof(szString) ) )
		fprintf( fp, "\t; %s\r\n", szString );
	else	fprintf( fp, "\r\n" );
	}
else	fprintf( fp, "\r\n" );
}


/***********************************************************************/
static BOOL ReadEvent( fp, lpEvent )
/***********************************************************************/
FILE *fp;
LPEVENTMSG lpEvent;
{
char szString[1024];
int type, cmd;
EVENTMSG Event;

if ( fscanf( fp, "%s (%d,%d) %lx%*[^\r\n]",
     szString, &Event.paramL, &Event.paramH, &Event.time ) == EOF )
	return( FALSE );

lpEvent->paramL = Event.paramL;
lpEvent->paramH = Event.paramH;
lpEvent->time   = Event.time;

type = *szString;
cmd = *(szString+1);
switch( type )
	{
	case 'M':
		lpEvent->message = WM_MOUSEMOVE;
		break;
	case 'K':
		switch( cmd )
		{
		case 'D':
			lpEvent->message = WM_KEYDOWN;
			break;
		case 'U':
			lpEvent->message = WM_KEYUP;
			Click();
			break;
		}
		break;
	case 'S':
		switch( cmd )
		{
		case 'D':
			lpEvent->message = WM_SYSKEYDOWN;
			break;
		case 'U':
			lpEvent->message = WM_SYSKEYUP;
			Click();
			break;
		}
		break;
	case 'L':
		switch( cmd )
		{
		case 'D':
			lpEvent->message = WM_LBUTTONDOWN;
			break;
		case 'U':
			lpEvent->message = WM_LBUTTONUP;
			Click();
			break;
		case '2':
			lpEvent->message = WM_LBUTTONDBLCLK;
			break;
		}
		break;
	case 'R':
		switch( cmd )
		{
		case 'D':
			lpEvent->message = WM_RBUTTONDOWN;
			break;
		case 'U':
			lpEvent->message = WM_RBUTTONUP;
			Click();
			break;
		case '2':
			lpEvent->message = WM_RBUTTONDBLCLK;
			break;
		}
		break;
	default:
		lpEvent->message = atoi( szString );
	}

return( YES );
}
