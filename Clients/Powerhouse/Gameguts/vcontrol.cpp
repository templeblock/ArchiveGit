#include "windows.h"
#include "mmsystem.h"
#include "proto.h"
#include "control.h"
#include "vtable.p"
#include "commonid.h"
#include "limits.h"
#include "macros.h"

#define SHIFT ( GetAsyncKeyState( VK_SHIFT )<0 )
#define CONTROL ( GetAsyncKeyState( VK_CONTROL )<0 )
#define ID_VIDEO_TIMER 0xBEEF

// Static data
static BOOL bTrack;
static int iDefaultZoomFactor;
static SHOTID lDefaultShot;
static int bDefaultUseMCIDrawProc;
static BOOL bShortenHotspots;

static long g_lJumpFrame = -1L;	// Indicates that a seek is a jump seek 
								// This is used so that low-level mmio
								// can figure out whether or not to
								// buffer up audio data.

static void Video_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
static void Video_OnRButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
static void Video_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);

static LPLBUTTONHOOK lpLButtonDown = Video_OnLButtonDown;
static LPRBUTTONHOOK lpRButtonDown = Video_OnRButtonDown;
static LPLUBUTTONHOOK lpLButtonUp = Video_OnLButtonUp;

/***********************************************************************/
BOOL Video_Register( HINSTANCE hInstance )
/***********************************************************************/
{
	WNDCLASS WndClass;
	WndClass.hCursor		= LoadCursor( hInstance, MAKEINTRESOURCE(IDC_HAND_CLOSED_CURSOR) );
	WndClass.hIcon			= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPSTR)"video";
	WndClass.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	WndClass.hInstance		= hInstance;
	WndClass.style			= CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	WndClass.lpfnWndProc	= VideoControl;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 16;

	return( RegisterClass( &WndClass ) );
}

/***********************************************************************/
void Video_SetDefaults( int iZoomFactor, SHOTID lShot, BOOL bShortHotspots, BOOL bUseMCIDrawProc )
/***********************************************************************/
{
	if ( iZoomFactor )
		iDefaultZoomFactor = iZoomFactor;
	if ( lShot )
		lDefaultShot = lShot;
	bShortenHotspots = bShortHotspots;
	bDefaultUseMCIDrawProc = bUseMCIDrawProc;
}

/***********************************************************************/
static void Video_SetPromptTimer( HWND hWindow, int iSeconds, SHOTID lShot )
/***********************************************************************/
{
	LPVIDEO lpVideo;

	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;

	if ( lpVideo->idPromptTimer )	// Already have a timer set
		KillTimer(hWindow, lpVideo->idPromptTimer);

	lpVideo->idPromptTimer = 0x0FED;
	lpVideo->lPromptShot = lShot;
	lpVideo->dwPromptStartTime = GetTickCount();
	lpVideo->dwPromptEndTime = lpVideo->dwPromptStartTime + (1000L * iSeconds);
	SetTimer( hWindow, (WORD)lpVideo->idPromptTimer, 500, NULL );
}

/***********************************************************************/
static LPSHOT Video_GetShotPtr( LPVIDEO lpVideo, SHOTID lShot )
/***********************************************************************/
{
	if ( !lpVideo || !lpVideo->lpAllShots )
		return( NULL );

	if ( lShot == SHOT_PREV  ||
		 lShot == SHOT_PREV2 ||
		 lShot == SHOT_TEST  ||
		 lShot == SHOT_NEXT  ||
		 lShot == SHOT_THIS  ||
		 lShot == SHOT_CONT  ||
		 lShot == SHOT_STOP )
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
void Video_SetPromptTimer(LPVIDEO lpVideo, LPSHOT lpShot)
/***********************************************************************/
// Set the Prompt timer if the lPromptTime is non-zero, else disarm the timer
{
	int PromptTime = (int)lpShot->lPromptTime;
	HWND hWnd = lpVideo->hWnd;

	if (PromptTime > 0)
		Video_SetPromptTimer(hWnd, PromptTime, lpShot->lPromptShot);
	else
	if (lpShot->lPromptShot == SHOT_STOP)
	{
		if (lpVideo->idPromptTimer != NULL)
			KillTimer(hWnd, lpVideo->idPromptTimer);

		lpVideo->idPromptTimer = NULL;
		lpVideo->lPromptShot = NULL;
	}
}
	

/***********************************************************************/
void Video_SetRegister(LPVIDEO lpVideo, LPSHOT lpShot)
/***********************************************************************/
// Set the register if lRegNum > 0                                          
{
	long Reg = lpShot->lRegNum;	
	if (Reg > 0)  {
		long Value = lpShot->lRegValue;
		
		if (Value == 9999999)		// A ++ command was entered
			++lpVideo->lRegisterValue[Reg-1];
		else
		if (Value == -9999999)		// A -- command was entered
			--lpVideo->lRegisterValue[Reg-1];
		else		
			lpVideo->lRegisterValue[Reg-1] = Value;
	}	 
}


/***********************************************************************/
LPSHOT Video_GetShot( LPVIDEO lpVideo, SHOTID lNextShot, long lCount )
/***********************************************************************/
{
	if ( !lpVideo || !lpVideo->lpAllShots )
		return( NULL );

	long lPrevFrame = 0;
	long lIndexReg = 0;
	SHOTID lCurrentShot = lpVideo->lCurrentShot;

Repeat:

	if ( lCount < 0 )
	{
		lIndexReg = -lCount;
		lCount = 0;
	}
	
	if ( lNextShot == SHOT_STOP )
	{
		return( NULL );
	}
	else
	if ( lNextShot == SHOT_PREV )
	{
		lNextShot = lpVideo->lPrevShot;
		lCount = 0;
		lIndexReg = 0;
		lPrevFrame = lpVideo->lPrevFrame;
		goto Repeat;
	}
	else
	if ( lNextShot == SHOT_PREV2 )		// Backup to the shot BEFORE previous shot 
	{                                   // Used for returns from prompts
		lNextShot = lpVideo->lPrevShot - 1;
		lCount = 0;
		lIndexReg = 0;
		lPrevFrame = 0;
		goto Repeat;
	}
	else
	if ( lNextShot == SHOT_NEXT )
	{
		lCurrentShot++;
		lNextShot = lCurrentShot;
		lCount = 0;
		lIndexReg = 0;
		goto Repeat;
	}
	else
	if ( lNextShot == SHOT_THIS )
	{
		lNextShot = lCurrentShot;
		lCount = 0;
		lIndexReg = 0;
		goto Repeat;
	}
	if ( lNextShot == SHOT_CONT )	// Continue where we left off.
	{
		lNextShot = lCurrentShot;
		lPrevFrame = MCIGetPosition( lpVideo->hDevice );
		goto Repeat;
	}
	else
	if ( lNextShot == SHOT_TEST )
	{
		LPSHOT lpCurrentShot = Video_GetShotPtr( lpVideo, lCurrentShot );
		if ( !lpCurrentShot )
			return( NULL );

		// Check the Compare Register
		long lTestValue = 0;
		long lCompReg = lpCurrentShot->lCompReg;
		if (lCompReg != 0)  {
			long lCompRegValue = lpVideo->lRegisterValue[lCompReg-1];
			long lCompValue = lpCurrentShot->lCompValue;
			
			if (lCompRegValue >  lCompValue)  lTestValue =  1;						
			if (lCompRegValue == lCompValue)  lTestValue =  0;						
			if (lCompRegValue <  lCompValue)  lTestValue = -1;
		}			
			
		if ( lTestValue > 0 )
		{
			lNextShot = lpCurrentShot->lUpShot;
			lCount = lpCurrentShot->lUpCount;
			lIndexReg = 0;
		}
		else
		if ( lTestValue == 0 )
		{
			lNextShot = lpCurrentShot->lHomeShot;
			lCount = lpCurrentShot->lHomeCount;
			lIndexReg = 0;
		}
		else
		if ( lTestValue < 0 )
		{
			lNextShot = lpCurrentShot->lDownShot;
			lCount = lpCurrentShot->lDownCount;
			lIndexReg = 0;
		}
		
		goto Repeat;
	}
    
	// 'lNextShot' should be a real shot by now!
	if ( lCount > 1 )
	{
        static long lLastCount = 99999;

		// If count has changed, reset the random sequence        
		if (lCount != lLastCount)  {
			lLastCount = lCount;
			SetRandomSequence((WORD)lCount);
		}
					
		lNextShot += GetNextRandom();
	}
    else	// No 'count' so try for an 'index'	
	if ( lIndexReg )
	{
		if ( (lIndexReg > 0) && (lIndexReg <= MAX_REGISTERS))  {
			long lShotOffset = lpVideo->lRegisterValue[lIndexReg-1];

			if (lShotOffset < 0)  lShotOffset = 0;
						
			// Now add the 'index' offset
			lNextShot += lShotOffset;
		}	
	}
			
	// Finally we get a pointer to the next shot
	LPSHOT lpNextShot = Video_GetShotPtr( lpVideo, lNextShot );
	if ( !lpNextShot )
		return( NULL );

	Video_SetPromptTimer(lpVideo, lpNextShot);
	Video_SetRegister(lpVideo, lpNextShot);
	
	if ( !lpNextShot->lStartFrame && !lpNextShot->lEndFrame && !lpNextShot->wDisk)
	{ // an alias (stub) shot; process the lEndShot as the alias
		lCurrentShot = lNextShot;

		lNextShot = lpNextShot->lEndShot;
		lCount = lpNextShot->lEndCount;
		lIndexReg = lpNextShot->lIndexReg;

		goto Repeat;
	}

	if ( lPrevFrame )
	{
		if ( (lPrevFrame >= lpNextShot->lStartFrame)
		  && (lPrevFrame <= lpNextShot->lEndFrame) )
			lpNextShot->lIndentFrames = lPrevFrame - lpNextShot->lStartFrame;
	}

	return( lpNextShot );
}


/***********************************************************************/
static void Video_SetJumpTimer( HWND hWindow, int iSeconds, SHOTID lShot, long lCount )
/***********************************************************************/
{
	LPVIDEO lpVideo;

	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;

	if ( !lpVideo->idLoopTimer )
		lpVideo->idLoopTimer = ID_VIDEO_TIMER;
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
		if ( bShortenHotspots && (lpShot->lFlags & FLAGBIT(A_SPECIAL)) )
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
	LPSHOT lpShot;
	for ( long idx = lpVideo->lCurrentShot+1; idx <= lpVideo->iNumShots; idx++ )
	{ // The shot table entries treats shots as 1-n, not 0-(n-1)
		if ( !(lpShot = Video_GetShotPtr( lpVideo, idx )) )
			continue;
		if ( lpShot->lFlags & dwFlagBit )
			return ( Video_GetShot( lpVideo, idx ) );
	}
	for ( idx = 1; idx <= lpVideo->lCurrentShot; idx++ )
	{ // The shot table entries treats shots as 1-n, not 0-(n-1)
		if ( !(lpShot = Video_GetShotPtr( lpVideo, idx )) )
			continue;
		if ( lpShot->lFlags & dwFlagBit )
			return ( Video_GetShot( lpVideo, idx ) );
	}

	return( NULL );
}

/***********************************************************************/
LPSHOT Video_FindShotFlags( HWND hWindow, DWORD dwFlagBits )
/***********************************************************************/
{
	LPVIDEO lpVideo;
	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return( NULL );

	// Loop through the shots, and find a shot with the passed flags on
	for ( long idx = 1; idx <= lpVideo->iNumShots; idx++ )
	{ // The shot table entries treats shots as 1-n, not 0-(n-1)
		LPSHOT lpShot;

		if ( !(lpShot = Video_GetShotPtr( lpVideo, idx )) )
			continue;

		if ( (lpShot->lFlags & dwFlagBits) == dwFlagBits )
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
LPSHOT Video_FindShotPrefix( HWND hWindow, LPSTR lpString )
/***********************************************************************/
{
	LPVIDEO lpVideo;
	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return( NULL );

	// Loop through the shots, and find a shot that matches the passed string
	for ( long idx = 1; idx <= lpVideo->iNumShots; idx++ )
	{ // The shot table entries treats shots as 1-n, not 0-(n-1)
		LPSHOT lpShot;

		if ( !(lpShot = Video_GetShotPtr( lpVideo, idx )) )
			continue;

		if ( FindPrefix( lpShot->szShotName, lpString ) )
			return ( Video_GetShot( lpVideo, idx ) );
	}

	return( NULL );
}

/***********************************************************************/
static SHOTID Video_ShotCheck( SHOTID lShot )
/***********************************************************************/
{
	if ( !lShot )
		return( lShot );
	if ( lShot == SHOT_PREV  ||
		 lShot == SHOT_PREV2 ||
		 lShot == SHOT_TEST  ||
		 lShot == SHOT_NEXT  ||
		 lShot == SHOT_THIS  ||
		 lShot == SHOT_CONT  ||
		 lShot == SHOT_STOP )
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

	char szBuff[12];
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
void Video_RemapShotTable(HWND hWindow)
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
			
			if (lCurShot == 0)
				continue;
				
			if ( lpShot->lEndShot	== lCurShot )	lpShot->lEndShot   = -idx2;
			if ( lpShot->lLeftShot	== lCurShot )	lpShot->lLeftShot  = -idx2;
			if ( lpShot->lUpShot	== lCurShot )	lpShot->lUpShot	   = -idx2;
			if ( lpShot->lRightShot == lCurShot )	lpShot->lRightShot = -idx2;
			if ( lpShot->lDownShot	== lCurShot )	lpShot->lDownShot  = -idx2;
			if ( lpShot->lHomeShot	== lCurShot )	lpShot->lHomeShot  = -idx2;
			if ( lpShot->lInsShot	== lCurShot )	lpShot->lInsShot   = -idx2;
			if ( lpShot->lDelShot	== lCurShot )	lpShot->lDelShot   = -idx2;

			if ( lpShot->lPromptShot == lCurShot )	lpShot->lPromptShot   = -idx2;
		}

/*		if ( lpShot->lFlags & FLAGBIT(A_SPECIAL) )
		{
			lpShot->lLeftHotspot  = Video_RandomizeHotspot( lpShot->lLeftHotspot );
			lpShot->lRightHotspot = Video_RandomizeHotspot( lpShot->lRightHotspot );
			lpShot->lUpHotspot    = Video_RandomizeHotspot( lpShot->lUpHotspot );
			lpShot->lDownHotspot  = Video_RandomizeHotspot( lpShot->lDownHotspot );
			lpShot->lHomeHotspot  = Video_RandomizeHotspot( lpShot->lHomeHotspot );
			lpShot->lInsHotspot   = Video_RandomizeHotspot( lpShot->lInsHotspot );
			lpShot->lDelHotspot	  = Video_RandomizeHotspot( lpShot->lDelHotspot );
		}
*/
		lpShot->bGoesPrev = (
			(lpShot->lEndShot   == SHOT_PREV2) ||
			(lpShot->lEndShot   == SHOT_PREV)  ||
			(lpShot->lLeftShot  == SHOT_PREV)  ||
			(lpShot->lRightShot == SHOT_PREV)  ||
			(lpShot->lUpShot    == SHOT_PREV)  ||
			(lpShot->lDownShot  == SHOT_PREV)  ||
			(lpShot->lHomeShot  == SHOT_PREV)  ||
			(lpShot->lInsShot   == SHOT_PREV)  ||
			(lpShot->lDelShot   == SHOT_PREV) );
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
		lpShot->lInsShot   = Video_ShotCheck( lpShot->lInsShot );
		lpShot->lDelShot   = Video_ShotCheck( lpShot->lDelShot );

		lpShot->lPromptShot = Video_ShotCheck( lpShot->lPromptShot );
	}
}

/***********************************************************************/
void Video_Close( HWND hWindow, LPVIDEO lpVideo )
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
BOOL Video_Open( HWND hWindow, LPVIDEO lpVideo, WORD wDisk, long lStartFrame )
/***********************************************************************/
{
	if ( !lpVideo )
		return( NO );

	if ( !wDisk )
		wDisk = 1;

	FNAME szFileName;
	GetWindowText( hWindow, szFileName, sizeof(STRING) );
	GetStringParm( szFileName, 0/*nIndex*/, ',', szFileName );
	LPLONG lpSwitches = ExtractSwitches( szFileName );
	
	// Replace any %d in the file name with the disk number,
	// and flag whether we need to check disk labels
	wsprintf( szFileName, szFileName, wDisk );
		
	FNAME szExpFileName;
	if ( !GetApp()->FindContent( szFileName, szExpFileName, YES ) )
	{
		FreeUp( (LPTR)lpSwitches );
		return FALSE;
	}

	HourGlass( YES );

	lpVideo->hWnd = hWindow;
	lpVideo->lpSwitches = lpSwitches;
    
	// Cleanup any open video file
	if ( lpVideo->idLoopTimer )
	{
		KillTimer( hWindow, lpVideo->idLoopTimer );
		lpVideo->idLoopTimer = NULL;
	}
	if ( lpVideo->idPromptTimer )
	{
		KillTimer( hWindow, lpVideo->idPromptTimer );
		lpVideo->idPromptTimer = NULL;
		lpVideo->lPromptShot = NULL;
	}
	if ( lpVideo->hDevice )
	{
		Video_Close( hWindow, lpVideo );
		lpVideo->hDevice = NULL;
	}

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
	if ( !(lpVideo->hDevice = MCIOpen( GetApp()->m_hDeviceAVI, szExpFileName, szAlias, hWindow, 
		lStartFrame, "avivideo" )) )
	{
		HourGlass( NO );
		return( NO );
	}

	lpVideo->lFrames = MCIGetLength( lpVideo->hDevice );
	lpVideo->wDisk = wDisk;

	MCISetTimeFormat( lpVideo->hDevice, MCI_FORMAT_FRAMES );
	if ( bDefaultUseMCIDrawProc )
		MCISetVideoDrawProc( hWindow, lpVideo->hDevice );

    // If necessary, tell the video to re-size itself based on the ZoomFactor
	if ( lpVideo->bCenterSaved )
		SetWindowPos( hWindow, NULL, 0,0, 0,0, SWP_NOZORDER | SWP_NOACTIVATE );
	
	HourGlass( NO );
	return( YES );
}

/***********************************************************************/
void Video_ProcessEvent( HWND hWindow, LPVIDEO lpVideo, int iEventCode )
/***********************************************************************/
{ // Check to see what the video action should be, based on the event code
	SHOTID lShot, lTimerShot;
	long lCount, lTimerCount;
	int iFrames, iSeconds;
	LPSHOT lpShot;

	if ( !(lpShot = Video_GetShotPtr( lpVideo, lpVideo->lCurrentShot )) )
		return;

	// Send the event code to the parent in case it wants to know that 
	// an event occurred
	if (lpVideo->bNotifyParent == TRUE)
		SendMessage(GetParent(hWindow), WM_VIDEO_MSG, (WORD)iEventCode, (LPARAM)lpShot);
	
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
		case EVENT_INS: // the "INS" event
			if ( lpShot->bInsIsSubShot )
			{
				if ( !(lpShot = Video_FindSubShot( lpVideo,
					lpShot->lInsShot, lpShot->lInsCount, 0 )) )
						return;
			}
			lShot  = lpShot->lInsShot;
			lCount = lpShot->lInsCount;
			break;
		case EVENT_DEL: // the "DEL" event
			if ( lpShot->bDelIsSubShot )
			{
				if ( !(lpShot = Video_FindSubShot( lpVideo,
					lpShot->lDelShot, lpShot->lDelCount, 0 )) )
						return;
			}
			lShot  = lpShot->lDelShot;
			lCount = lpShot->lDelCount;
			break;
		default:
			return;
	}

	if ( !lShot )
	{
		MessageBeep(0);
		return;
	}

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
	
	// Try the ins event
	if ( lpShot->bInsIsSubShot )
	{
		long lCount = lpShot->lInsCount;
		if ( !lCount ) lCount = 1;
		for ( int i=0; i<lCount; i++ )
		{
			if ( lpSubShot = Video_GetShotPtr( lpVideo, lpShot->lInsShot+i ) )
				lpSubShot->fHotspotDisabled &= (~32);
		}
	}
	
	// Try the del event
	if ( lpShot->bDelIsSubShot )
	{
		long lCount = lpShot->lDelCount;
		if ( !lCount ) lCount = 1;
		for ( int i=0; i<lCount; i++ )
		{
			if ( lpSubShot = Video_GetShotPtr( lpVideo, lpShot->lDelShot+i ) )
				lpSubShot->fHotspotDisabled &= (~64);
		}
	}
}

/***********************************************************************/
void Video_GotoShot( HWND hWindow, LPVIDEO lpVideo, SHOTID lShot, long lCount, int iFrames, int iEventCode )
/***********************************************************************/
{
	if ( !lpVideo )
		return;
		
	if ( !lpVideo->lpAllShots )
	{ // if no jump table, start it playing
		MCIStop( lpVideo->hDevice, YES/*bWait*/ );
		g_lJumpFrame = 0;
		MCIPlay( lpVideo->hDevice, hWindow/*Notify Myself*/ );
		g_lJumpFrame = -1;
		return;
	}

	LPSHOT lpShot;
	if ( !(lpShot = Video_GetShot( lpVideo, lShot, lCount )) )
	{ // stop if no shot returned
		MCIStop( lpVideo->hDevice, YES/*bWait*/ );
		// send a NULL for lppShot to indicate stop
		FORWARD_WM_COMMAND( GetParent(hWindow), GET_WINDOW_ID(hWindow),
			0, 0, SendMessage );
		return;
	}

	// Check to see if we need to switch disks
	if ( lpShot->wDisk && (lpShot->wDisk != lpVideo->wDisk) )
	{
		if ( !Video_Open( hWindow, lpVideo, lpShot->wDisk, lpShot->lStartFrame ) )
		{ // resort to playing the current shot
			lpShot = Video_GetShot( lpVideo, lpVideo->lLastShot, lpVideo->wDisk );
			if ( !lpShot )
				return;
		}
	}

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
		lpVideo->lLastShot = lpVideo->lCurrentShot;
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

// JMM - did have timing problems without the MCIStop(),
// but I'll try the setting lLastShotToPlay to NULL instead
	MCIStop( lpVideo->hDevice, YES/*bWait*/ );
//	MCIPause( lpVideo->hDevice );
	lpVideo->lLastShotToPlay = NULL;
	g_lJumpFrame = lFrom;
	MCIPlay( lpVideo->hDevice, hWindow/*Notify Myself*/, lFrom, lTo );
	g_lJumpFrame = -1;
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
	Video_DumpShotTable(id, lpVideo->lpAllShots, lpVideo->iNumShots);

	LPSHOT lpShot = Video_GetShotPtr( lpVideo, lpVideo->lCurrentShot );
	int wDisk = (lpShot && lpShot->wDisk ? lpShot->wDisk : 1);
	if ( !Video_Open( hWindow, lpVideo, wDisk, (lpShot ? lpShot->lStartFrame : 0) ) )
		GetApp()->GotoScene(GetParent(hWindow), 0); // Exit the program
	return( TRUE );
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

    
	// Save the original x,y position of the control.
	if ( !lpVideo->bCenterSaved )
	{
		lpVideo->iCenterX = lpWindowPos->x;
		lpVideo->iCenterY = lpWindowPos->y;
		lpVideo->bCenterSaved = TRUE;
	}
	
	POINT pt;
	pt.x = lpVideo->iCenterX - (cx / 2);
	pt.y = lpVideo->iCenterY - (cy / 2);
	pt.x &= (~3); // For it to live on a 4 pixel (32 bit) boundary
	
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
	if ( lpVideo->idPromptTimer )
	{
		KillTimer( hWindow, lpVideo->idPromptTimer );
		lpVideo->idPromptTimer = NULL;
		lpVideo->lPromptShot = NULL;
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
	
	if ( id == lpVideo->idLoopTimer )
	{
		KillTimer( hWindow, lpVideo->idLoopTimer );
		lpVideo->idLoopTimer = NULL;

		// Make sure it's still at the same shot
		if ( lpVideo->lTimerCurrentShot != lpVideo->lCurrentShot )
			return;

		// Jump to the shot we squirreled away earlier
		Video_GotoShot( hWindow, lpVideo, lpVideo->lTimerJumpShot,
			lpVideo->lTimerJumpCount, 0/*iFrames*/, EVENT_TIMEOUT );
	}
	else if (id == lpVideo->idPromptTimer )  {
		if (lpVideo->lPromptShot != NULL)  {
			if (GetTickCount() >= lpVideo->dwPromptEndTime)  {
				KillTimer(hWindow, lpVideo->idPromptTimer);
				
//				Debug("Delta = %ld", GetTickCount() - lpVideo->dwPromptStartTime);
				
				lpVideo->idPromptTimer = NULL;
				lpVideo->dwPromptStartTime = 0;
				lpVideo->dwPromptEndTime = 0;
	
				Video_GotoShot( hWindow, lpVideo, lpVideo->lPromptShot,
					1/*iCount*/, 0/*iFrames*/, NULL/*EventCode*/ );
	
				lpVideo->lPromptShot = NULL;
			}	
		}
	}
}


/***********************************************************************/
static BOOL PtInEventHotspot( long lHotspot, POINT pt )
/***********************************************************************/
{
	if ( !lHotspot )
		return NO;
		
	BOOL bNegative = (lHotspot < 0);
	if ( bNegative )
		lHotspot = -lHotspot;
		
	RECT rHotspot;
	int i = (int)lHotspot;	// Hotspot form is x1y1x2y2 e.g. 0012
	
	rHotspot.bottom =  (i % 10) + 1;
	rHotspot.right	= ((i/10) % 10) + 1;
	rHotspot.top	=  (i/100) % 10;
	rHotspot.left	=  (i/1000) % 10;
	
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
		if ( bDisable && (lpShot->lFlags & FLAGBIT(A_SPECIAL)) )
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
		if ( bDisable && (lpShot->lFlags & FLAGBIT(A_SPECIAL)) )
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
		if ( bDisable && (lpShot->lFlags & FLAGBIT(A_SPECIAL)) )
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
		if ( bDisable && (lpShot->lFlags & FLAGBIT(A_SPECIAL)) )
			lpShot->fHotspotDisabled |= 8;
		return( EVENT_DOWN );
	}

	TryHome: // Try the home event
	lpShot = lpShotOrig;
	if ( lpShot->bHomeIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryIns;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lHomeShot, lpShot->lHomeCount, lFrame )) )
				goto TryIns;
	}
	if ( !(lpShot->fHotspotDisabled & 16) && PtInEventHotspot( lpShot->lHomeHotspot, pt ) )
	{
		if ( bDisable && (lpShot->lFlags & FLAGBIT(A_SPECIAL)) )
			lpShot->fHotspotDisabled |= 16;
		return( EVENT_HOME );
	}

	TryIns: // Try the ins event
	lpShot = lpShotOrig;
	if ( lpShot->bInsIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryDel;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lInsShot, lpShot->lInsCount, lFrame )) )
				goto TryDel;
	}
	if ( !(lpShot->fHotspotDisabled & 32) && PtInEventHotspot( lpShot->lInsHotspot, pt ) )
	{
		if ( bDisable && (lpShot->lFlags & FLAGBIT(A_SPECIAL)) )
			lpShot->fHotspotDisabled |= 32;
		return( EVENT_INS );
	}

	TryDel: // Try the del event
	lpShot = lpShotOrig;
	if ( lpShot->bDelIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryDone;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lDelShot, lpShot->lDelCount, lFrame )) )
				goto TryDone;
	}
	if ( !(lpShot->fHotspotDisabled & 64) && PtInEventHotspot( lpShot->lDelHotspot, pt ) )
	{
		if ( bDisable && (lpShot->lFlags & FLAGBIT(A_SPECIAL)) )
			lpShot->fHotspotDisabled |= 64;
		return( EVENT_DEL );
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
 	if ( lpVideo->bAutoplayUsed )
		MCIUpdate( lpVideo->hDevice, hDC );
	EndPaint( hWindow, &ps );

	if ( !lpVideo->bAutoplayUsed )
	{
		lpVideo->bAutoplayUsed = YES;
		if ( GetSwitchValue( 'a', lpVideo->lpSwitches ) )
			FORWARD_WM_KEYDOWN( hWindow, 'P', 0/*cRepeat*/, 0/*flags*/, SendMessage);
		if ( !bDefaultUseMCIDrawProc )
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
				1/*lCount*/, 0/*iFrames*/, EVENT_TIMEOUT/*iEventCode*/ );
			break;
		}

		case 'S':
		{ // Stop
			if (lpVideo->idLoopTimer)
			{
				KillTimer (hWindow, lpVideo->idLoopTimer);
				lpVideo->idLoopTimer = NULL;
			}

			if ( lpVideo->idPromptTimer )
			{
				KillTimer( hWindow, lpVideo->idPromptTimer );
				lpVideo->idPromptTimer = NULL;
				lpVideo->lPromptShot = NULL;
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
			if ( !lpVideo->lpEventProc || !(*lpVideo->lpEventProc)( hWindow, lpVideo, EVENT_END ) )
				Video_ProcessEvent( hWindow, lpVideo, EVENT_END );
			break;
		}

		case VK_LEFT:
		{ // Process the LEFT event
			if ( !lpVideo->lpEventProc || !(*lpVideo->lpEventProc)( hWindow, lpVideo, EVENT_LEFT ) )
				Video_ProcessEvent( hWindow, lpVideo, EVENT_LEFT );
			break;
		}

		case VK_UP:			
		{ // Process the UP event
			if ( !lpVideo->lpEventProc || !(*lpVideo->lpEventProc)( hWindow, lpVideo, EVENT_UP ) )
				Video_ProcessEvent( hWindow, lpVideo, EVENT_UP );
			break;
		}
		
		case VK_RIGHT:
		{ // Process the RIGHT event
			if ( !lpVideo->lpEventProc || !(*lpVideo->lpEventProc)( hWindow, lpVideo, EVENT_RIGHT ) )
				Video_ProcessEvent( hWindow, lpVideo, EVENT_RIGHT );
			break;
		}

		case VK_DOWN:
		{ // Process the DOWN event
			if ( !lpVideo->lpEventProc || !(*lpVideo->lpEventProc)( hWindow, lpVideo, EVENT_DOWN ) )
				Video_ProcessEvent( hWindow, lpVideo, EVENT_DOWN );
			break;
		}

		case VK_HOME:
		{ // Process the HOME event
			if ( !lpVideo->lpEventProc || !(*lpVideo->lpEventProc)( hWindow, lpVideo, EVENT_HOME ) )
				Video_ProcessEvent( hWindow, lpVideo, EVENT_HOME );
			break;
		}

		case VK_INSERT:
		{ // Process the INSERT event
			if ( !lpVideo->lpEventProc || !(*lpVideo->lpEventProc)( hWindow, lpVideo, EVENT_INS ) )
				Video_ProcessEvent( hWindow, lpVideo, EVENT_INS );
			break;
		}

		case VK_DELETE:
		{ // Process the DELETE event
			if ( !lpVideo->lpEventProc || !(*lpVideo->lpEventProc)( hWindow, lpVideo, EVENT_DEL ) )
				Video_ProcessEvent( hWindow, lpVideo, EVENT_DEL );
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

		case VK_END:
		{
			static BOOL bHome;
			LPSHOT lpShot;
			BOOL bOption = (SHIFT|CONTROL);
			bHome = !bHome;
			if ( (bHome != bOption) )
			{ // advance to the next shot
				do
				{
				lpVideo->lCurrentShot += (bOption ? -1 : +1);
				lpShot = Video_GetShotPtr( lpVideo, lpVideo->lCurrentShot );
				}
				while ( lpShot && !lpShot->lStartFrame && !lpShot->lEndFrame );
			}

			lpShot = Video_GetShotPtr( lpVideo, lpVideo->lCurrentShot );
			if ( !lpShot )
				break;

			STRING szString;
			long lFrame;
			if ( bHome )
			{
				lFrame = lpShot->lStartFrame;
				wsprintf( szString, "%ld IN", lFrame );
				if ( (lFrame & 3) && (lpShot->lStartFrame != lpShot->lEndFrame)) // if not on a key frame boundary...
					MessageBeep(0);
			}
			else
			{
				lFrame = lpShot->lEndFrame;
				wsprintf( szString, "%ld OUT", lFrame );
			}

			g_lJumpFrame = lFrame;
			bReturn = MCISeek( lpVideo->hDevice, lFrame );
			g_lJumpFrame = -1;

			SetDlgItemText( GetParent(hWindow), 120/*IDC_VIDEO_SHOTFRAME*/, szString );
			GetAtomName( lpShot->aShotID, szString, sizeof(STRING) );
			SetDlgItemText( GetParent(hWindow), 130/*IDC_VIDEO_SHOTNAME*/, szString );
			return; // not a break to avoid the SHOTFRAME from being reset
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
		#endif // _DEBUG

		default:
			break;
	}
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
	{
		if ( !lpVideo->lpEventProc || !(*lpVideo->lpEventProc)( hWindow, lpVideo, iEventCode ) )
			Video_ProcessEvent( hWindow, lpVideo, iEventCode );
	}
}


/***********************************************************************/
static void Video_OnRButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
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
			{
				if ( !lpVideo->lpEventProc || !(*lpVideo->lpEventProc)( hWindow, lpVideo, EVENT_END ) )
					Video_ProcessEvent( hWindow, lpVideo, EVENT_END );
			}
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
		case MCI_NOTIFY_FAILURE:	// wParam == 8
		{ // superseded msg sent during playback if we get a new play command
			return( FALSE );
		}
	}
	return(FALSE);
}

/***********************************************************************/
void Video_OnSoundOnOff( HWND hWindow, BOOL bOn )
/***********************************************************************/
{
	LPVIDEO lpVideo;

	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;

	MCISetAudioOnOff( lpVideo->hDevice, bOn );
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
		//HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Video_OnLButtonDown);
		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, lpLButtonDown);
		HANDLE_MSG(hWindow, WM_RBUTTONDOWN, lpRButtonDown);

		HANDLE_MSG(hWindow, WM_LBUTTONUP, lpLButtonUp);
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

		case (WM_VIDEOSOUND_ONOFF):
		{
			Video_OnSoundOnOff(hWindow, (BOOL)wParam);
			return 1;
		}

		// This is used to call Video_ProcessEvent through a SendMessage or PostMessage.
		// Sometimes we need to post an event from within the video draw proc since
		// we can't make mci calls directly from within the proc.
		case (WM_VIDEO_EVENT):
		{
			Video_ProcessEvent(hWindow, 
				(LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR ), wParam);

			// Special 'post' command for use when we need to know after the
			// video has been started
			FORWARD_WM_COMMAND( GetParent(hWindow), GET_WINDOW_ID(hWindow),
				0, wParam, SendMessage );

			return 1;
		}

		case (WM_VIDEO_GOTOSHOT):
		{
			LPVIDEO lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR );
			LPSHOT  lpShot = (LPSHOT)lParam;
			Video_GotoShot(hWindow, lpVideo, lpShot->lShotID, 1, 0, lpShot->iLastEvent);
			return 1;
		}

		default:
		return DefWindowProc( hWindow, message, wParam, lParam );
	}
}


/***********************************************************************/
void Video_HookMouseButtons( LPLBUTTONHOOK lpLeftDProc,
							 LPRBUTTONHOOK lpRightProc,
							 LPLUBUTTONHOOK lpLeftUProc)
/***********************************************************************/
{
	if (lpLeftDProc)
		lpLButtonDown = lpLeftDProc;
	if (lpRightProc)
		lpRButtonDown = lpRightProc;
	if (lpLeftUProc)
		lpLButtonUp = lpLeftUProc;
}

/***********************************************************************/
void Video_UnhookMouseButtons()
/***********************************************************************/
{
	lpLButtonDown = Video_OnLButtonDown;
	lpRButtonDown = Video_OnRButtonDown;
	lpLButtonUp = Video_OnLButtonUp;
}

/***********************************************************************/
long Video_JumpFrame(void)
/***********************************************************************/
{
	return g_lJumpFrame;
}

/***********************************************************************/
LPSHOT Video_GetCurrentShot( LPVIDEO lpVideo )
/***********************************************************************/
{
	if ( !lpVideo )
		return NULL;

	return Video_GetShotPtr( lpVideo, lpVideo->lCurrentShot );
}
