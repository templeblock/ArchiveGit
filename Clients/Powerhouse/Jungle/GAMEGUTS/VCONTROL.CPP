#include "windows.h"
#include "mmsystem.h"
#include "proto.h"
#include "control.h"
#include "vtable.p"
#include "commonid.h"
#include "limits.h"

#define SHIFT ( GetAsyncKeyState( VK_SHIFT )<0 )
#define CONTROL ( GetAsyncKeyState( VK_CONTROL )<0 )
#define ESCAPE ( GetAsyncKeyState( VK_ESCAPE )<0 )
#define A_SPECIAL 1073741824 // same as 0x40000000L

// Static data
static BOOL bTrack;
static int iDefaultZoomFactor;
static SHOTID lDefaultShot;
static BOOL bShortenHotspots;

/***********************************************************************/
void Video_SetDefaults( int iZoomFactor, SHOTID lShot, BOOL bShortHotspots )
/***********************************************************************/
{
	if ( iZoomFactor )
		iDefaultZoomFactor = iZoomFactor;
	if ( lShot )
		lDefaultShot = lShot;
	bShortenHotspots = bShortHotspots;
}

/***********************************************************************/
static LPSHOT Video_GetShotPtr( LPVIDEO lpVideo, SHOTID lShot )
/***********************************************************************/
{
	if ( !lpVideo || !lpVideo->lpAllShots )
		return( NULL );

	lShot--; // The shot table treats shots as 1-n; we want 0-(n-1)
	if ( lShot < 0 || lShot >= lpVideo->iNumShots )
	{
		Print( "Bad shot number %d", lShot );
		lShot = 0;
	}

	return( &lpVideo->lpAllShots[ lShot ] );
}

/***********************************************************************/
LPSHOT Video_GetShot( LPVIDEO lpVideo, SHOTID lShot, long lCount )
/***********************************************************************/
{
	if ( !lpVideo || !lpVideo->lpAllShots )
		return( NULL );

	long lPrevFrame = 0;
	SHOTID lCurrentShot = lpVideo->lCurrentShot;

Repeat:
	if ( lShot == SHOT_PREV )
	{
		lShot = lpVideo->lPrevShot;
		lPrevFrame = lpVideo->lPrevFrame;
		goto Repeat;
	}
	else
	if ( lShot == SHOT_TEST )
	{
		if ( lCount < 0 || lCount > MAX_TESTVALUES )
			lCount = 0;
		long lTestValue = lpVideo->lTestValue[lCount];

		LPSHOT lpShot = Video_GetShotPtr( lpVideo, lCurrentShot );
		if ( lTestValue > 0 )
		{
			lShot = lpShot->lUpShot;
			lCount = lpShot->lUpCount;
		}
		else
		if ( lTestValue == 0 )
		{
			lShot = lpShot->lHomeShot;
			lCount = lpShot->lHomeCount;
		}
		else
		if ( lTestValue < 0 )
		{
			lShot = lpShot->lDownShot;
			lCount = lpShot->lDownCount;
		}
		goto Repeat;
	}

	if ( lCount > 1 )
		lShot += GetRandomNumber( (WORD)lCount );

	LPSHOT lpShot = Video_GetShotPtr( lpVideo, lShot );
	if ( !lpShot->lStartFrame && !lpShot->lEndFrame )
	{ // an alias (stub) shot; process the lEndShot as the alias
		lShot = lpShot->lEndShot;
		lCount = lpShot->lEndCount;
		lCurrentShot = lpShot->lShotID;
		goto Repeat;
	}

	if ( lPrevFrame )
	{
		if ( (lPrevFrame >= lpShot->lStartFrame) && (lPrevFrame <= lpShot->lEndFrame) )
			lpShot->lIndentFrames = lPrevFrame - lpShot->lStartFrame;
	}

	return( lpShot );
}

/***********************************************************************/
static void Video_SetJumpTimer( HWND hWindow, int iSeconds, SHOTID lShot, long lCount )
/***********************************************************************/
{
	LPVIDEO lpVideo;

	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;

	if ( !lpVideo->idLoopTimer )
		lpVideo->idLoopTimer = 0xBEEF;
	else
	{ // already have a timer set
		if ( (lpVideo->lTimerCurrentShot == lpVideo->lCurrentShot) &&
			 (lpVideo->lTimerJumpShot	 == lShot ) &&
			 (lpVideo->lTimerJumpCount	 == lCount ) )
				return; // trying to set up the same jump timer
		KillTimer( hWindow, lpVideo->idLoopTimer );
		lpVideo->idLoopTimer++;
	}

	lpVideo->lTimerCurrentShot = lpVideo->lCurrentShot;
	lpVideo->lTimerJumpShot	   = lShot;
	lpVideo->lTimerJumpCount   = lCount;
	SetTimer( hWindow, (WORD)lpVideo->idLoopTimer, 1000*iSeconds, NULL );
}

/***********************************************************************/
LPSHOT Video_FindSubShot( LPVIDEO lpVideo, SHOTID lShot, long lCount, long lFrame )
/***********************************************************************/
{
	if ( !lpVideo )
		return( NULL );

	if ( !lFrame )
		if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
			return( NULL );

	long lStartFrame, lEndFrame;
	if ( !lCount ) lCount = 1;
	for ( int i=0; i<lCount; i++ )
	{
		LPSHOT lpShot;
		if ( !(lpShot = Video_GetShotPtr( lpVideo, lShot+i )) )
			continue;
		lStartFrame = lpShot->lStartFrame;
		lEndFrame = lpShot->lEndFrame;
		if ( bShortenHotspots && (lpShot->lFlags & A_SPECIAL) )
		{
			long l = (lEndFrame - lStartFrame) / 4;
			lStartFrame += l;
			lEndFrame -= l;
		}
		if ( lFrame < lStartFrame || lFrame > lEndFrame )
			continue;
		// The current frame is in this shot's range
		return( Video_GetShot( lpVideo, lShot+i ) );
	}

	return( NULL );
}

/***********************************************************************/
LPSHOT Video_FindShot( HWND hWindow, SHOTID lShot, long lCount )
/***********************************************************************/
{
	LPVIDEO lpVideo;
	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return( NULL );

	if ( !lCount ) lCount = 1;
	for ( int i=0; i<lCount; i++ )
	{
		LPSHOT lpShot;
		if ( !(lpShot = Video_GetShotPtr( lpVideo, lShot+i )) )
			continue;
		// The current frame is in this shot's range
		return( Video_GetShot( lpVideo, lShot+i ) );
	}

	return( NULL );
}

/***********************************************************************/
LPSHOT Video_FindShotFlag( HWND hWindow, BYTE cFlagNum )
/***********************************************************************/
{
	LPVIDEO lpVideo;
	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return( NULL );

	// Loop through the shots, and find a shot with the passed flag on
	DWORD dwFlagBit = FLAGBIT(cFlagNum);
	for ( long idx = 1; idx <= lpVideo->iNumShots; idx++ )
	{ // The shot table entries treats shots as 1-n, not 0-(n-1)
		LPSHOT lpShot;

		if ( !(lpShot = Video_GetShotPtr( lpVideo, idx )) )
			continue;

		if ( lpShot->lFlags & dwFlagBit )
			return ( Video_GetShot( lpVideo, idx ) );
	}

	return( NULL );
}

/***********************************************************************/
LPSHOT Video_FindShotName( HWND hWindow, LPSTR lpString )
/***********************************************************************/
{
	LPVIDEO lpVideo;
	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return( NULL );

	ATOM atom;
	if ( !(atom = FindAtom(lpString)) )
		return( NULL );

	// Loop through the shots, and find a shot that matches the passed string (now an atom)
	for ( long idx = 1; idx <= lpVideo->iNumShots; idx++ )
	{ // The shot table entries treats shots as 1-n, not 0-(n-1)
		LPSHOT lpShot;

		if ( !(lpShot = Video_GetShotPtr( lpVideo, idx )) )
			continue;

		if ( lpShot->aShotID == atom )
			return ( Video_GetShot( lpVideo, idx ) );
	}

	return( NULL );
}

/***********************************************************************/
static SHOTID Video_ShotCheck( SHOTID lShot )
/***********************************************************************/
{
	if (!lShot || 
		lShot == SHOT_PREV ||
		lShot == SHOT_TEST ||
		lShot == SHOT_STOP)
		return( lShot );

	if ( lShot < 0 )
		return( -lShot );

	STRING szString;
	GetAtomName((ATOM)lShot, szString, sizeof(STRING) );
	Print("Can't find shot %s", (LPTR)szString );
	return( 0 );
}

/***********************************************************************/
static long Video_RandomizeHotspot( long lHotspot )
/***********************************************************************/
{
		if ( lHotspot <= 0 )
			return( 0 );

		char szBuff[5];
		wsprintf (szBuff, "%04ld", lHotspot);
		szBuff[4] = '\0';

		int iLow, iMid, iHigh;
		iLow  = (int)(szBuff[0] - '0');
		iHigh = (int)(szBuff[1] - '0');
		iMid = iLow + GetRandomNumber( iHigh - iLow + 1 );
		if ( iMid < 1 ) iMid = 1;
		if ( iMid > 8 ) iMid = 8;
		szBuff[0] = '0' + (iMid - 1);
		szBuff[1] = '0' + (iMid + 1);

		iLow  = (int)(szBuff[2] - '0');
		iHigh = (int)(szBuff[3] - '0');
		iMid = iLow + GetRandomNumber( iHigh - iLow + 1 );
		if ( iMid < 1 ) iMid = 1;
		if ( iMid > 8 ) iMid = 8;
		szBuff[2] = '0' + (iMid - 1);
		szBuff[3] = '0' + (iMid + 1);
		return( latol( szBuff ) );
}

// Re-map the video table to make all entries and indices sequential
/***********************************************************************/
static void Video_RemapShotTable(HWND hWindow)
/***********************************************************************/
{
	LPVIDEO lpVideo;
	LPSHOT lpShot, lpShot2;
	SHOTID lCurShot;
	long idx, idx2;

	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;

	// Loop through the shots, and find a shot index to map each the shot id's to
	// Stuff in a negative value, so we know its been changed
	for ( idx = 1; idx <= lpVideo->iNumShots; idx++ )
	{ // The shot table entries treats shots as 1-n, not 0-(n-1)
		if ( !(lpShot = Video_GetShotPtr( lpVideo, idx )) )
			continue;

		for ( idx2 = 1; idx2 <= lpVideo->iNumShots; idx2++ )
		{
			if ( !(lpShot2 = Video_GetShotPtr( lpVideo, idx2 )) )
				continue;

			lCurShot = lpShot2->lShotID;
			if ( lpShot->lEndShot	== lCurShot )	lpShot->lEndShot   = -idx2;
			if ( lpShot->lLeftShot	== lCurShot )	lpShot->lLeftShot  = -idx2;
			if ( lpShot->lUpShot	== lCurShot )	lpShot->lUpShot	   = -idx2;
			if ( lpShot->lRightShot == lCurShot )	lpShot->lRightShot = -idx2;
			if ( lpShot->lDownShot	== lCurShot )	lpShot->lDownShot  = -idx2;
			if ( lpShot->lHomeShot	== lCurShot )	lpShot->lHomeShot  = -idx2;
		}

		if ( lpShot->lFlags & A_SPECIAL )
		{
			lpShot->lLeftHotspot  = Video_RandomizeHotspot( lpShot->lLeftHotspot );
			lpShot->lRightHotspot = Video_RandomizeHotspot( lpShot->lRightHotspot );
			lpShot->lUpHotspot    = Video_RandomizeHotspot( lpShot->lUpHotspot );
			lpShot->lDownHotspot  = Video_RandomizeHotspot( lpShot->lDownHotspot );
			lpShot->lHomeHotspot  = Video_RandomizeHotspot( lpShot->lHomeHotspot );
		}

		lpShot->bGoesPrev = (
			(lpShot->lEndShot   == SHOT_PREV) ||
			(lpShot->lLeftShot  == SHOT_PREV) ||
			(lpShot->lRightShot == SHOT_PREV) ||
			(lpShot->lUpShot    == SHOT_PREV) ||
			(lpShot->lDownShot  == SHOT_PREV) ||
			(lpShot->lHomeShot  == SHOT_PREV) );
	}

	// Go back and change each shot's id to an index, and check for errors
	for ( idx = 1; idx <= lpVideo->iNumShots; idx++ )
	{ // The shot table entries treats shots as 1-n, not 0-(n-1)
		if ( !(lpShot = Video_GetShotPtr( lpVideo, idx )) )
			continue;

		lpShot->lShotID = idx;

		lpShot->lEndShot   = Video_ShotCheck( lpShot->lEndShot );
		lpShot->lLeftShot  = Video_ShotCheck( lpShot->lLeftShot );
		lpShot->lUpShot	   = Video_ShotCheck( lpShot->lUpShot );
		lpShot->lRightShot = Video_ShotCheck( lpShot->lRightShot );
		lpShot->lDownShot  = Video_ShotCheck( lpShot->lDownShot );
		lpShot->lHomeShot  = Video_ShotCheck( lpShot->lHomeShot );
	}
}

/***********************************************************************/
static void Video_Close( HWND hWindow, LPVIDEO lpVideo )
/***********************************************************************/
{
	if ( !lpVideo )
		return;

	if ( lpVideo->hDevice )
	{
		MCIStop( lpVideo->hDevice, YES/*bWait*/ );
		// The MCIClose() line below causes 2 mmtask errors: even Media Player gets them
		// mmtask: Invalid global handle 0x0000
		// mmtask: CreateDC error
		HMCI hDevice = lpVideo->hDevice;
		lpVideo->hDevice = NULL;
		MCIClose( hDevice );
//		InvalidateRect( hWindow, NULL, TRUE );
//		UpdateWindow( hWindow );
	}
	lpVideo->wDisk = 0;
}

/***********************************************************************/
static BOOL Video_Open( HWND hWindow, LPVIDEO lpVideo, WORD wDisk, long lStartFrame )
/***********************************************************************/
{
	if ( !lpVideo )
		return( NO );

	HourGlass( YES );

	// Cleanup any open video file
	if ( lpVideo->lpSwitches )
	{
		FreeUp( (LPTR)lpVideo->lpSwitches );
		lpVideo->lpSwitches = NULL;
	}
	if ( lpVideo->idLoopTimer )
	{
		KillTimer( hWindow, lpVideo->idLoopTimer );
		lpVideo->idLoopTimer = NULL;
	}
	if ( lpVideo->hDevice )
	{
		Video_Close( hWindow, lpVideo );
		lpVideo->hDevice = NULL;
	}

	FNAME szFileName;
	GetWindowText( hWindow, szFileName, sizeof(STRING) );
	GetStringParm( szFileName, 0/*nIndex*/, ',', szFileName );
	lpVideo->lpSwitches = ExtractSwitches( szFileName );
	
	// Replace any %d in the file name with the disk number,
	// and flag whether we need to check disk labels
	wsprintf( szFileName, szFileName, wDisk );
		
	BOOL bFullScreen = (BOOL)GetSwitchValue( 'f', lpVideo->lpSwitches );
	VideoInit( NO/*ZoomBy2*/, bFullScreen );
	if ( !lpVideo->iZoomFactor )
	{
		if ( iDefaultZoomFactor )
			lpVideo->iZoomFactor = iDefaultZoomFactor;
		else
		{
			lpVideo->iZoomFactor = (int)GetSwitchValue( 'z', lpVideo->lpSwitches );
			if ( !lpVideo->iZoomFactor )
				lpVideo->iZoomFactor = 1;
		}
	}
	
	STRING szAlias;
	wsprintf( szAlias, "A%d%ld", wDisk, (LPSTR)hWindow );
	if ( !(lpVideo->hDevice = MCIOpen( GetApp()->m_hDeviceAVI, szFileName, szAlias, hWindow, lStartFrame )) )
	{
		HourGlass( NO );
		return( NO );
	}

	lpVideo->lFrames = MCIGetLength( lpVideo->hDevice );
	lpVideo->wDisk = wDisk;

	//MCISetVideoDrawProc( lpVideo->hDevice, NULL/*lpDrawDibProc*/, NULL/*lpDrawDCProc*/ );
	MCISetTimeFormat( lpVideo->hDevice, MCI_FORMAT_FRAMES );

	// force the control to take on the size of the video file
	RECT rect;
	GetClientRect( hWindow, &rect );
	MapWindowPoints( hWindow, GetParent(hWindow), (LPPOINT)&rect, 2 );
	SetWindowPos( hWindow, NULL, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
		SWP_NOZORDER | SWP_NOACTIVATE );

	HourGlass( NO );
	return( YES );
}

/***********************************************************************/
static void Video_ProcessEvent( HWND hWindow, LPVIDEO lpVideo, int iEventCode, int iEventToPassAlong = 0 )
/***********************************************************************/
{ // Check to see what the video action should be, based on the event code
	SHOTID lShot, lTimerShot;
	long lCount, lTimerCount;
	int iFrames, iSeconds;
	LPSHOT lpShot;

	if ( !(lpShot = Video_GetShotPtr( lpVideo, lpVideo->lCurrentShot )) )
		return;

	iFrames = 0;
	iSeconds = 0;

	switch ( iEventCode )
	{
		case EVENT_END: // the "END" event
			if ( !lpShot->lEndLoop )
			{
				lShot  = lpShot->lEndShot;
				lCount = lpShot->lEndCount;
			}
			else
			{
				// (LOWORD(lLoop) = number of seconds to loop; 0 == forever)
				// (HIWORD(lLoop) = number of frames to loop; 0 == all frames)
				iFrames	 = HIWORD(lpShot->lEndLoop);
				if ( iSeconds = LOWORD(lpShot->lEndLoop) )
				{
					lTimerShot = lpShot->lEndShot;
					lTimerCount = lpShot->lEndCount;
				}
				lShot  = lpShot->lShotID;
				lCount = 1;
			}
			break;
		case EVENT_LEFT: // the "LEFT" event
			if ( lpShot->bLeftIsSubShot )
			{
				if ( !(lpShot = Video_FindSubShot( lpVideo,
					lpShot->lLeftShot, lpShot->lLeftCount, 0 )) )
						return;
			}
			lShot  = lpShot->lLeftShot;
			lCount = lpShot->lLeftCount;
			break;
		case EVENT_UP: // the "UP" event
			if ( lpShot->bUpIsSubShot )
			{
				if ( !(lpShot = Video_FindSubShot( lpVideo,
					lpShot->lUpShot, lpShot->lUpCount, 0 )) )
						return;
			}
			lShot  = lpShot->lUpShot;
			lCount = lpShot->lUpCount;
			break;
		case EVENT_RIGHT: // the "RIGHT" event
			if ( lpShot->bRightIsSubShot )
			{
				if ( !(lpShot = Video_FindSubShot( lpVideo,
					lpShot->lRightShot, lpShot->lRightCount, 0 )) )
						return;
			}
			lShot  = lpShot->lRightShot;
			lCount = lpShot->lRightCount;
			break;
		case EVENT_DOWN: // the "DOWN" event
			if ( lpShot->bDownIsSubShot )
			{
				if ( !(lpShot = Video_FindSubShot( lpVideo,
					lpShot->lDownShot, lpShot->lDownCount, 0 )) )
						return;
			}
			lShot  = lpShot->lDownShot;
			lCount = lpShot->lDownCount;
			break;
		case EVENT_HOME: // the "HOME" event
			if ( lpShot->bHomeIsSubShot )
			{
				if ( !(lpShot = Video_FindSubShot( lpVideo,
					lpShot->lHomeShot, lpShot->lHomeCount, 0 )) )
						return;
			}
			lShot  = lpShot->lHomeShot;
			lCount = lpShot->lHomeCount;
			break;
		default:
			return;
	}

	if ( !lShot )
	{
		MessageBeep(0);
		return;
	}

	if ( iEventToPassAlong )
		iEventCode = iEventToPassAlong;
	if (lShot == SHOT_STOP)
		// send a NULL for lppShot to indicate stop
		FORWARD_WM_COMMAND( GetParent(hWindow), GET_WINDOW_ID(hWindow),
			0, 0, SendMessage );
	else
		Video_GotoShot( hWindow, lpVideo, lShot, lCount, iFrames, iEventCode );
	if ( iSeconds ) // if a timer (in seconds) is specified...
		Video_SetJumpTimer( hWindow, iSeconds, lTimerShot, lTimerCount );
}

/***********************************************************************/
static void Video_ShotEnableHotspots( LPVIDEO lpVideo, LPSHOT lpShot )
/***********************************************************************/
{
	if ( !lpShot )
		return;

	lpShot->fHotspotDisabled = 0;

	if ( !lpVideo )
		return;

	LPSHOT lpSubShot;

	// Try the left event
	if ( lpShot->bLeftIsSubShot )
	{
		long lCount = lpShot->lLeftCount;
		if ( !lCount ) lCount = 1;
		for ( int i=0; i<lCount; i++ )
		{
			if ( lpSubShot = Video_GetShotPtr( lpVideo, lpShot->lLeftShot+i ) )
				lpSubShot->fHotspotDisabled &= (~1);
		}
	}

	// Try the right event
	if ( lpShot->bRightIsSubShot )
	{
		long lCount = lpShot->lRightCount;
		if ( !lCount ) lCount = 1;
		for ( int i=0; i<lCount; i++ )
		{
			if ( lpSubShot = Video_GetShotPtr( lpVideo, lpShot->lRightShot+i ) )
				lpSubShot->fHotspotDisabled &= (~2);
		}
	}

	// Try the up event
	if ( lpShot->bUpIsSubShot )
	{
		long lCount = lpShot->lUpCount;
		if ( !lCount ) lCount = 1;
		for ( int i=0; i<lCount; i++ )
		{
			if ( lpSubShot = Video_GetShotPtr( lpVideo, lpShot->lUpShot+i ) )
				lpSubShot->fHotspotDisabled &= (~4);
		}
	}

	// Try the down event
	if ( lpShot->bDownIsSubShot )
	{
		long lCount = lpShot->lDownCount;
		if ( !lCount ) lCount = 1;
		for ( int i=0; i<lCount; i++ )
		{
			if ( lpSubShot = Video_GetShotPtr( lpVideo, lpShot->lDownShot+i ) )
				lpSubShot->fHotspotDisabled &= (~8);
		}
	}

	// Try the home event
	if ( lpShot->bHomeIsSubShot )
	{
		long lCount = lpShot->lHomeCount;
		if ( !lCount ) lCount = 1;
		for ( int i=0; i<lCount; i++ )
		{
			if ( lpSubShot = Video_GetShotPtr( lpVideo, lpShot->lHomeShot+i ) )
				lpSubShot->fHotspotDisabled &= (~16);
		}
	}
}

/***********************************************************************/
void Video_GotoShot( HWND hWindow, LPVIDEO lpVideo, SHOTID lShot, long lCount, int iFrames, int iEventCode )
/***********************************************************************/
{
	LPSHOT lpShot;

	if ( !(lpShot = Video_GetShot( lpVideo, lShot, lCount )) )
	{
		MCIStop( lpVideo->hDevice, YES/*bWait*/ );
		MCIPlay( lpVideo->hDevice, hWindow/*Notify Myself*/ );
		return;
	}

	if ( lpShot->wDisk != lpVideo->wDisk )
		if ( !Video_Open( hWindow, lpVideo, lpShot->wDisk, lpShot->lStartFrame ) )
			return;

	if ( lpShot->lShotID != lpVideo->lCurrentShot || !iEventCode )
	{ // if the shot is changing, or a NULL event code...

		// Send the notification message if the shot is changing
		lpShot->iLastEvent = iEventCode;
		LPPSHOT lppShot = &lpShot;
		FORWARD_WM_COMMAND( GetParent(hWindow), GET_WINDOW_ID(hWindow),
			LOWORD(lppShot), HIWORD(lppShot), SendMessage );
		if ( !(*lppShot) ) // if the application rejects this shot...
			return; // get out, and whatever is playing keeps playing

		// Set the previous shot variables, unless we could cause an infinite loop
		if ( lpShot->bGoesPrev )
		{
			LPSHOT lpCurrentShot = Video_GetShotPtr( lpVideo, lpVideo->lCurrentShot );
			if ( lpCurrentShot && !lpCurrentShot->bGoesPrev )
			{
				lpVideo->lPrevFrame = MCIGetPosition( lpVideo->hDevice );
				lpVideo->lPrevShot = lpVideo->lCurrentShot;
			}
		}
			
		lpVideo->lCurrentShot = lpShot->lShotID;
	}

	// Calculate the lFrom and lTo frames
	long lTo = lpShot->lEndFrame;
	long lFrom = lpShot->lStartFrame;
	if ( iFrames > 0 )
		lFrom = max( lTo - iFrames, lFrom );

	if ( lpShot->lIndentFrames )
	{ // If indention is being used...
		lFrom += lpShot->lIndentFrames;
		lpShot->lIndentFrames = 0;
		if ( lFrom > lTo )
			lFrom = lTo;
	}
	else // Clear any disabled hotspot flags
		Video_ShotEnableHotspots( lpVideo, lpShot );

	MCIStop( lpVideo->hDevice, YES/*bWait*/ );
	MCIPlay( lpVideo->hDevice, hWindow/*Notify Myself*/, lFrom, lTo );
	lpVideo->lLastShotToPlay = lpVideo->lCurrentShot;
}

/***********************************************************************/
static BOOL Video_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	BOOL fRemap;
	LPVIDEO lpVideo;

	if ( !(lpVideo = (LPVIDEO)AllocX( sizeof(VIDEO), GMEM_ZEROINIT|GMEM_SHARE )) )
		return( FALSE );

	SetWindowLong( hWindow, GWL_DATAPTR, (long)lpVideo );

	ITEMID id;
	if ( !(id = GetWindowWord( hWindow, GWW_ICONID )) )
		id = GET_WINDOW_ID( hWindow );
	lpVideo->lpAllShots = Video_GetShotData( id, &lpVideo->iNumShots, &fRemap );
	lpVideo->lCurrentShot = lpVideo->lPrevShot = (lDefaultShot ? lDefaultShot : 1 );
	lDefaultShot = 0;
	lpVideo->lPrevFrame = 0;

	// Remap the shot table to make all entries sequential
	if (fRemap)
		Video_RemapShotTable( hWindow );
	Video_CheckDumpShotTable(id, lpVideo->lpAllShots, lpVideo->iNumShots);

	LPSHOT lpShot = Video_GetShotPtr( lpVideo, lpVideo->lCurrentShot );
	return( Video_Open( hWindow, lpVideo, (lpShot ? lpShot->wDisk : 1), (lpShot ? lpShot->lStartFrame : 0) ) );
}

/***********************************************************************/
static BOOL Video_OnSizeOrPosChange( HWND hWindow, LPWINDOWPOS lpWindowPos )
/***********************************************************************/
{
	int cx, cy;

	if ( lpWindowPos->flags & SWP_NOSIZE )
		return( FORWARD_WM_WINDOWPOSCHANGING( hWindow, lpWindowPos, DefWindowProc ) );

	LPVIDEO lpVideo;
	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return( YES );
	if ( !lpVideo->hDevice )
		return( YES );

	RECT SrcRect;
	MCIGetSrcRect( lpVideo->hDevice, &SrcRect );
	MCIClip( lpVideo->hDevice, &SrcRect, NULL/*&DstRect*/, lpVideo->iZoomFactor );

	if ( lpVideo->iZoomFactor > 0 )
	{
		cx = RectWidth(&SrcRect)  * lpVideo->iZoomFactor;
		cy = RectHeight(&SrcRect) * lpVideo->iZoomFactor;
	}
	else
	{
		cx = RectWidth(&SrcRect)  / (-lpVideo->iZoomFactor);
		cy = RectHeight(&SrcRect) / (-lpVideo->iZoomFactor);
	}

	if ( lpWindowPos->cx == cx && lpWindowPos->cy == cy )
		return( YES );

	RECT rect;
	GetWindowRect( hWindow, &rect );
	POINT pt;
	pt.x = (rect.right + rect.left - cx)/2;
	pt.y = (rect.bottom + rect.top - cy)/2;
	pt.x &= (~3); // For it to live on a 4 pixel (32 bit) boundary
	MapWindowPoints( NULL, GetParent(hWindow), &pt, 1 ); // Map screen to parent
	lpWindowPos->x	= pt.x;
	lpWindowPos->y	= pt.y;
	lpWindowPos->cx = cx;
	lpWindowPos->cy = cy;
	return( NO );
}

/***********************************************************************/
static void Video_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	LPVIDEO lpVideo;

	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;

	if ( lpVideo->lpSwitches )
	{
		FreeUp( (LPTR)lpVideo->lpSwitches );
		lpVideo->lpSwitches = NULL;
	}
	if ( lpVideo->idLoopTimer )
	{
		KillTimer( hWindow, lpVideo->idLoopTimer );
		lpVideo->idLoopTimer = NULL;
	}
	Video_DrawProcInstall( hWindow, NULL/*lpSetProc*/, NULL/*lpDrawProc*/, 33/*wTimeDelay*/ );
	Video_Close( hWindow, lpVideo );
	if ( lpVideo->lpAllShots )
		FreeUp( lpVideo->lpAllShots );
	FreeUp( lpVideo );
	if ( bTrack )
	{
		ReleaseCapture();
		bTrack = NO;
	}
}

/***********************************************************************/
static void Video_OnTimer(HWND hWindow, UINT id)
/***********************************************************************/
{ // Timer went off, so jump to the desired shot
	LPVIDEO lpVideo;
	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;

	if ( Video_DrawProcDraw( hWindow, id  ) )
		return;
	
	if ( lpVideo->idLoopTimer != id )
		return;

	KillTimer( hWindow, lpVideo->idLoopTimer );
	lpVideo->idLoopTimer = NULL;

	// Make sure it's still at the same shot
	if ( lpVideo->lTimerCurrentShot != lpVideo->lCurrentShot )
		return;

	// Jump to the shot we squirreled away earlier
	Video_GotoShot( hWindow, lpVideo, lpVideo->lTimerJumpShot,
		lpVideo->lTimerJumpCount, 0/*iFrames*/, EVENT_TIMEOUT );
}

/***********************************************************************/
static BOOL PtInEventHotspot( long lHotspot, POINT pt )
/***********************************************************************/
{
	if ( !lHotspot )
		return( NO );

	BOOL bNegative = (lHotspot < 0);
	if ( bNegative )
		lHotspot *= (-1);
	RECT rHotspot;
	int i = (int)lHotspot / 100;
	int j = i / 10;
	rHotspot.left = j;
	rHotspot.right = i - (j*10) + 1;
	i = (int)lHotspot - (i*100);
	j = i / 10;
	rHotspot.top = j;
	rHotspot.bottom = i - (j*10) + 1;
	if ( bNegative )
		return( !PtInRect( &rHotspot, pt ) );
	else
		return( PtInRect( &rHotspot, pt ) );
}

/***********************************************************************/
static int PtInHotspot( LPVIDEO lpVideo, int x, int y, BOOL bDisable )
/***********************************************************************/
{
	if ( !lpVideo )
		return( NULL );

	LPSHOT lpShotOrig;
	if ( !(lpShotOrig = Video_GetShotPtr( lpVideo, lpVideo->lCurrentShot )) )
		return( NULL );

	POINT pt;
	pt.x = x;
	pt.y = y;

	LPSHOT lpShot;
	long lFrame = 0;

	//TryLeft: // Try the left event
	lpShot = lpShotOrig;
	if ( lpShot->bLeftIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryRight;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lLeftShot, lpShot->lLeftCount, lFrame )) )
				goto TryRight;
	}
	if ( !(lpShot->fHotspotDisabled & 1) && PtInEventHotspot( lpShot->lLeftHotspot, pt ) )
	{
		if ( bDisable && (lpShot->lFlags & A_SPECIAL) )
			lpShot->fHotspotDisabled |= 1;
		return( EVENT_LEFT );
	}

	TryRight: // Try the right event
	lpShot = lpShotOrig;
	if ( lpShot->bRightIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryUp;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lRightShot, lpShot->lRightCount, lFrame )) )
				goto TryUp;
	}
	if ( !(lpShot->fHotspotDisabled & 2) && PtInEventHotspot( lpShot->lRightHotspot, pt ) )
	{
		if ( bDisable && (lpShot->lFlags & A_SPECIAL) )
			lpShot->fHotspotDisabled |= 2;
		return( EVENT_RIGHT );
	}

	TryUp: // Try the up event
	lpShot = lpShotOrig;
	if ( lpShot->bUpIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryDown;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lUpShot, lpShot->lUpCount, lFrame )) )
				goto TryDown;
	}
	if ( !(lpShot->fHotspotDisabled & 4) && PtInEventHotspot( lpShot->lUpHotspot, pt ) )
	{
		if ( bDisable && (lpShot->lFlags & A_SPECIAL) )
			lpShot->fHotspotDisabled |= 4;
		return( EVENT_UP );
	}

	TryDown: // Try the down event
	lpShot = lpShotOrig;
	if ( lpShot->bDownIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryHome;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lDownShot, lpShot->lDownCount, lFrame )) )
				goto TryHome;
	}
	if ( !(lpShot->fHotspotDisabled & 8) && PtInEventHotspot( lpShot->lDownHotspot, pt ) )
	{
		if ( bDisable && (lpShot->lFlags & A_SPECIAL) )
			lpShot->fHotspotDisabled |= 8;
		return( EVENT_DOWN );
	}

	TryHome: // Try the home event
	lpShot = lpShotOrig;
	if ( lpShot->bHomeIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryDone;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lHomeShot, lpShot->lHomeCount, lFrame )) )
				goto TryDone;
	}
	if ( !(lpShot->fHotspotDisabled & 16) && PtInEventHotspot( lpShot->lHomeHotspot, pt ) )
	{
		if ( bDisable && (lpShot->lFlags & A_SPECIAL) )
			lpShot->fHotspotDisabled |= 16;
		return( EVENT_HOME );
	}

	TryDone:
	return( NULL );
}

/***********************************************************************/
static BOOL Video_OnSetCursor(HWND hWindow, HWND hWndCursor, UINT codeHitTest, UINT msg)
/***********************************************************************/
{
	LPVIDEO lpVideo;
	RECT rWindow;
	POINT pt;
	int x, y;

	if ( hWndCursor != hWindow ) // not in our window
		return( FALSE );

	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return( FALSE );

	GetCursorPos( &pt );
	ScreenToClient( hWindow, &pt );
	GetClientRect( hWindow, &rWindow );
	x = (pt.x * 10) / RectWidth( &rWindow );
	y = (pt.y * 10) / RectHeight( &rWindow );

	if ( PtInHotspot( lpVideo, x, y, NO ) )
		{
		SetCursor( LoadCursor( GetApp()->GetInstance(), MAKEINTRESOURCE(IDC_HAND_POINT_CURSOR) ) );
		return( TRUE );
		}

	return( FORWARD_WM_SETCURSOR(hWindow, hWndCursor, codeHitTest, msg, DefWindowProc) );
}

/***********************************************************************/
static BOOL Video_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
	return( TRUE ); // handle ERASEBKGND and do nothing; PAINT covers everything
}

#ifdef _DEBUG
/***********************************************************************/
static UINT Video_OnNCHitTest(HWND hWindow, int x, int y)
/***********************************************************************/
{
	UINT uReturn = FORWARD_WM_NCHITTEST(hWindow, x, y, DefWindowProc);
	if ( SHIFT && (uReturn == HTCLIENT) )
		uReturn = HTCAPTION;
	return( uReturn );
}
#endif

/***********************************************************************/
static void Video_OnMove(HWND hWindow, int x, int y)
/***********************************************************************/
{
	InvalidateRect( hWindow, NULL, FALSE );
}

/***********************************************************************/
static void Video_OnPaint(HWND hWindow)
/***********************************************************************/
{
	LPVIDEO lpVideo;
	HDC hDC;
	PAINTSTRUCT ps;

	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;
	hDC = BeginPaint( hWindow, &ps );
	MCIUpdate( lpVideo->hDevice, hDC );
	EndPaint( hWindow, &ps );
	if ( !lpVideo->bAutoplayUsed )
	{
		lpVideo->bAutoplayUsed = YES;
		if ( GetSwitchValue( 'a', lpVideo->lpSwitches ) )
			FORWARD_WM_KEYDOWN( hWindow, 'P', 0/*cRepeat*/, 0/*flags*/, SendMessage);
		Video_DrawProcInstall( hWindow, VDP_HotspotSet, VDP_HotspotDraw, 33 );
	}
}

/***********************************************************************/
static void Video_OnKey(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
	BOOL bReturn;
	LPVIDEO lpVideo;
	static int iSpeed;

	if ( StyleOn( hWindow, WS_NOTENABLED ) )
		return;

	#ifndef _DEBUG
	if ( flags & KF_REPEAT )
		return;
	#endif

	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;

	switch ( vk )
	{
		case 'P':
		{ // Play
			Video_GotoShot( hWindow, lpVideo, lpVideo->lCurrentShot,
				1/*lCount*/, 0/*iFrames*/, 0/*iEventCode*/ );
			break;
		}

		case 'S':
		{ // Stop
			if (lpVideo->idLoopTimer)
			{
				KillTimer (hWindow, lpVideo->idLoopTimer);
				lpVideo->idLoopTimer = NULL;
			}
			bReturn = MCIStop( lpVideo->hDevice, YES/*bWait*/ );
			break;
		}

		case VK_PAUSE:
		{
			DWORD dwMode = MCIGetMode( lpVideo->hDevice );
			if ( dwMode == MCI_MODE_PAUSE )
				bReturn = MCIResume( lpVideo->hDevice );
			else
			if ( dwMode == MCI_MODE_PLAY )
				bReturn = MCIPause( lpVideo->hDevice );
			break;
		}

		case VK_SPACE:
		{ // Process the END event to skip the current shot
			Video_ProcessEvent( hWindow, lpVideo, EVENT_END );
			break;
		}

		case VK_LEFT:
		{ // Process the LEFT event
			Video_ProcessEvent( hWindow, lpVideo, EVENT_LEFT );
			break;
		}

		case VK_UP:			
		{ // Process the UP event
			Video_ProcessEvent( hWindow, lpVideo, EVENT_UP );
			break;
		}
		
		case VK_RIGHT:
		{ // Process the RIGHT event
			Video_ProcessEvent( hWindow, lpVideo, EVENT_RIGHT );
			break;
		}

		case VK_DOWN:
		{ // Process the DOWN event
			Video_ProcessEvent( hWindow, lpVideo, EVENT_DOWN );
			break;
		}

		case VK_HOME:
		{ // Process the DOWN event
			Video_ProcessEvent( hWindow, lpVideo, EVENT_HOME );
			break;
		}

		#ifdef _DEBUG
		case '1':
		{
			bReturn = MCISetAudioOnOff( lpVideo->hDevice, OFF );
			break;
		}

		case '2':
		{
			bReturn = MCISetAudioOnOff( lpVideo->hDevice, ON );
			break;
		}

		case '3':
		{
			bReturn = MCISetVideoOnOff( lpVideo->hDevice, OFF );
			break;
		}

		case '4':
		{
			bReturn = MCISetVideoOnOff( lpVideo->hDevice, ON );
			break;
		}

		case 'G':	// Turn grid draw proc on
		{
			Video_DrawProcInstall( hWindow, VDP_GridSet, VDP_GridDraw, 33 );
			break;
		}

		case 'H':	// Turn hotspot draw proc on
		{
			Video_DrawProcInstall( hWindow, VDP_HotspotSet, VDP_HotspotDraw, 33 );
			break;
		}

		case 'N':	// Turn draw proc off
		{
			Video_DrawProcInstall( hWindow, NULL, NULL, 33 );
			break;
		}

//		case VK_HOME:
//		{
//			bReturn = MCISeekHome( lpVideo->hDevice );
//			break;
//		}

		case VK_END:
		{
			LPSHOT lpShot = Video_GetShotPtr( lpVideo, lpVideo->lCurrentShot );
			if ( lpShot )
				bReturn = MCISeek( lpVideo->hDevice, lpShot->lEndFrame );
			else
				bReturn = MCISeekEnd( lpVideo->hDevice );
			break;
		}

		case VK_NEXT:
		{
			bReturn = MCIStepForward( lpVideo->hDevice, 1 );
			break;
		}

		case VK_PRIOR:
		{
			bReturn = MCIStepReverse( lpVideo->hDevice );
			break;
		}

		case VK_MULTIPLY:
		{
			iSpeed = 0;
			bReturn = MCISetVideoSpeed( lpVideo->hDevice, 1000L + iSpeed );
			break;
		}

		case VK_ADD:
		{
			if ( iSpeed >= 1000 )
				break;
			iSpeed += 100;
			bReturn = MCISetVideoSpeed( lpVideo->hDevice, 1000L + iSpeed );
			break;
		}

		case VK_SUBTRACT:
		{
			if ( iSpeed <= -1000 )
				break;
			iSpeed -= 100;
			bReturn = MCISetVideoSpeed( lpVideo->hDevice, 1000L + iSpeed );
			break;
		}
		#endif

		default:
			break;
	}

	#ifdef _DEBUG // For putting the current frame number into a text box
		STRING szString;
		wsprintf( szString, "%ld", MCIGetPosition( lpVideo->hDevice ) );
		SetDlgItemText( GetParent(hWindow), 120/*IDC_VIDEO_SHOTFRAME*/, szString );
	#endif
}

/***********************************************************************/
static void Video_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	LPVIDEO lpVideo;
	RECT rWindow;

	if ( SHIFT )
		return;
	if ( bTrack )
		return;
	SetCapture( hWindow ); bTrack = TRUE;

	#ifdef _DEBUG
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	#endif

	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;

	if ( !lpVideo->lpAllShots ) // if no jump table...
	{
		FORWARD_WM_COMMAND( GetParent(hWindow), GET_WINDOW_ID(hWindow), 0, 0, SendMessage );
		return;
	}

	// Process the proper event based on the "hotspot" that was clicked
	GetClientRect( hWindow, &rWindow );
	x = (x * 10) / RectWidth( &rWindow );
	y = (y * 10) / RectHeight( &rWindow );

	int iEventCode = PtInHotspot( lpVideo, x, y, YES );
	if ( iEventCode )
		Video_ProcessEvent( hWindow, lpVideo, iEventCode );
}

/***********************************************************************/
static void Video_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( !bTrack )
		return;
	ReleaseCapture(); bTrack = FALSE;
}

#ifdef _DEBUG
/***********************************************************************/
static void Video_OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( CONTROL )
		PrintWindowCoordinates( hWindow );
}
#endif

/***********************************************************************/
static void Video_OnSetState(HWND hWindow, BOOL fState)
/***********************************************************************/
{
	SetWindowWord( hWindow, GWW_STATE, fState );
}

/***********************************************************************/
static UINT Video_OnMCINotify(HWND hWindow, UINT codeNotify, HMCI hDevice)
/***********************************************************************/
{
	LPVIDEO lpVideo;
	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return(FALSE);

	// Something's wrong with the video notify
	if ( hDevice != lpVideo->hDevice )
		return( FALSE );

	switch( codeNotify )
	{
		case MCI_NOTIFY_SUCCESSFUL: // wParam == 1
		{ // successful msg sent when a shot is played to completion
			// Process the END event
			if ( lpVideo->lLastShotToPlay == lpVideo->lCurrentShot )
				Video_ProcessEvent( hWindow, lpVideo, EVENT_END );
			return( FALSE );
		}

		case MCI_NOTIFY_ABORTED:	// wParam == 4
		{ // aborted msg sent during playback, if we get a
		  // seek, step, stop, or full screen playback escape command.
		  // continue playing after a seek or step, unless
		  // escaping from a full screen playback
			return( FALSE );
		}

		case MCI_NOTIFY_SUPERSEDED: // wParam == 2
		{ // superseded msg sent during playback if we get a new play command
			return( FALSE );
		}

		case MCI_NOTIFY_FAILURE:	// wParam == 8
		{ // superseded msg sent during playback if we get a new play command
			return( FALSE );
		}
	}
	return(FALSE);
}

/***********************************************************************/
long WINPROC EXPORT VideoControl(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_CREATE, Video_OnCreate);
		HANDLE_MSG(hWindow, WM_WINDOWPOSCHANGING, Video_OnSizeOrPosChange);
		HANDLE_MSG(hWindow, WM_DESTROY, Video_OnDestroy);
		HANDLE_MSG(hWindow, WM_TIMER, Video_OnTimer);
		HANDLE_MSG(hWindow, WM_SETCURSOR, Video_OnSetCursor);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Video_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_PAINT, Video_OnPaint);
		HANDLE_MSG(hWindow, WM_KEYDOWN, Video_OnKey);
		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Video_OnLButtonDown);
		HANDLE_MSG(hWindow, WM_LBUTTONUP, Video_OnLButtonUp);
		#ifdef _DEBUG
		HANDLE_MSG(hWindow, WM_RBUTTONUP, Video_OnRButtonUp);
		HANDLE_MSG(hWindow, WM_NCHITTEST, Video_OnNCHitTest);
		#endif
		HANDLE_MSG(hWindow, BM_SETSTATE, Video_OnSetState);
		HANDLE_MSG(hWindow, BM_SETCHECK, Video_OnSetState);	 
		#ifdef WIN32 // needed for Windows 95 cause it still sends some 16-bit messages
		HANDLE_MSG(hWindow, BM_SETSTATE16, Video_OnSetState);
		HANDLE_MSG(hWindow, BM_SETCHECK16, Video_OnSetState);
		#endif
		HANDLE_MSG(hWindow, MM_MCINOTIFY, Video_OnMCINotify);

		default:
		return DefWindowProc( hWindow, message, wParam, lParam );
	}
}
