//Indeo video can be requested to use an external palette with either
//of the following MCI (Media Control Interface) commands:
//
//SETVIDEO <alias> PALETTE HANDLE TO <palette_handle>
//REALIZE <alias> BACKGROUND

#include <windows.h>
#include <mmsystem.h>
#include "digitalv.h"
#include "mciavi.h"
#include "proto.h"

//void CALLBACK EXPORT MCICallback( HDRVR h, UINT uMessage, DWORD dwUser, DWORD dwParam1, DWORD dwParam2 );

//****************************************************************************
//	This function opens an MCI global device and leaves it open.
//	The device type can either be a string like "waveaudio", or
//	an it can be an ID like (LPSTR)MCI_DEVTYPE_SEQUENCER (for MIDI).
//
//	Opening the driver once and leaving it open saves time.
//	You can then use this driver ID in the other MCI element (file) calls.
//	It saves time if you open an element without specifying a device to open.
//	When playing multiple elements (e.g., wavefiles), this excess time
//	can be costly.
//****************************************************************************
WORD MCIOpenDevice( LPSTR lpDeviceType )
//****************************************************************************
{
MCI_OPEN_PARMS mciopen;
DWORD dwRes;
DWORD dwFlags;

mciopen.wDeviceID = 0;
dwFlags = MCI_OPEN_TYPE | MCI_OPEN_SHAREABLE | MCI_WAIT;

// When specifying the device type as a constant 'ID',
// (e.g, (LPSTR)MCI_DEVTYPE_SEQUENCER, (LPTR)MCI_DEVTYPE_WAVEFORM_AUDIO)
// you must also include the MCI_OPEN_TYPE_ID flag.
// Otherwise use strings as in (LPTR)"avivideo"

mciopen.lpstrDeviceType = lpDeviceType;
if ( !HIWORD( lpDeviceType ) )
	dwFlags |= MCI_OPEN_TYPE_ID; 

//mciopen.dwCallback = (DWORD)MakeProcInstance( (FARPROC)MCICallback, App.GetInstance() );
//dwFlags |= CALLBACK_FUNCTION;
//dwFlags |= MCI_NOTIFY;
if ( (dwRes = mciSendCommand( 0, MCI_OPEN, dwFlags, (DWORD)(LPSTR)&mciopen )) )
	{
	MCIError(dwRes);
	return(FALSE);
	}

return( mciopen.wDeviceID );
}

//****************************************************************************
WORD MCIOpen( WORD wDeviceID, LPSTR lpFileName, LPSTR lpAlias, HWND hWndPlay, long lStartFrame )
//***************************************************************************
{
MCI_OPEN_PARMS mciopen;
DWORD dwRes;
DWORD dwFlags;

if ( !wDeviceID )
	return FALSE;

FNAME szExpFileName;
if ( !GetApp()->FindContent( lpFileName, szExpFileName ) )
	return FALSE;

lpFileName = szExpFileName;

mciopen.lpstrDeviceType = NULL;
mciopen.lpstrElementName = lpFileName;
dwFlags = MCI_OPEN_ELEMENT; /*| MCI_WAIT*/;

if ( lpAlias )
	{
	mciopen.lpstrAlias = lpAlias;
	dwFlags |= MCI_OPEN_ALIAS;
	}

//mciopen.dwCallback = (DWORD)MakeProcInstance( (FARPROC)MCICallback, App.GetInstance() );
//dwFlags |= CALLBACK_FUNCTION;
//dwFlags |= MCI_NOTIFY;
if ( (dwRes = mciSendCommand( wDeviceID, MCI_OPEN, dwFlags, (DWORD)(LPSTR)&mciopen )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

wDeviceID = mciopen.wDeviceID;

if ( hWndPlay )
	{
	// Seek to the start position before it draws into the window
	MCISeek( wDeviceID, lStartFrame );

	// Send a command to specify a playback window
 	MCI_DGV_WINDOW_PARMS mciWindow;
	mciWindow.hWnd = hWndPlay;
	if ( (dwRes = mciSendCommand( wDeviceID, MCI_WINDOW, MCI_DGV_WINDOW_HWND, (DWORD)(LPSTR)&mciWindow )) )
		{
		MCIError(dwRes);
		return FALSE;
		} 

//	// Send a command to NOT seek exactly
//	if ( (dwRes = mciSendCommand( wDeviceID, MCI_SET, MCI_DGV_SET_SEEK_EXACTLY, (DWORD)(LPSTR)NULL )) )
//		{
//		MCIError(dwRes);
//		return FALSE;
//		}
	}

return( wDeviceID );
}

/****************************************************************************
	This function takes a currently open device ID, a handle to a window
	that will be notified when playback is finished and calls the MCI_PLAY
	command.  The function immediately returns and does not wait for the AVI
	file to finish playing.  hWndNotify is the callback window which will
	be notified once the video is finished playing.
****************************************************************************/

//***************************************************************************
BOOL MCIPlay( WORD wDeviceID, HWND hWndNotify, long lFrom, long lTo, DWORD dwSpeed, BOOL bFullScreen, BOOL bRepeat )
//***************************************************************************
{
DWORD dwRes;
DWORD dwFlags;
MCI_DGV_PLAY_PARMS mciplay;

if ( !wDeviceID )
	return FALSE;
							 
if ( !lFrom && !lTo )
	{ // (lFrom=0, lTo=0) is the same as playing nothing
	lFrom = -1;
	lTo = -1;
	}

if ( bFullScreen )
	dwFlags = MCI_MCIAVI_PLAY_FULLSCREEN;
else
	dwFlags = MCI_MCIAVI_PLAY_WINDOW;

if (bRepeat)
	MCILoop( wDeviceID, YES, lFrom, lTo ); // dwFlags |= MCI_DGV_PLAY_REPEAT;

if ( dwSpeed )
	MCISetVideoSpeed( wDeviceID, dwSpeed );

if ( lFrom >= 0 )
	{
	mciplay.dwFrom = lFrom;
	dwFlags |= MCI_FROM;
	}

if ( lTo >= 0 )
	{
	mciplay.dwTo = lTo;
	dwFlags |= MCI_TO;
	}

if ( hWndNotify )
	{ // use a callback to notify the window
	mciplay.dwCallback = (DWORD)(LPTR)hWndNotify;
	dwFlags |= MCI_NOTIFY;
	}
else
	dwFlags |= MCI_WAIT;

// Also...
//dwFlags |= MCI_DGV_PLAY_REPEAT;
//dwFlags |= MCI_DGV_PLAY_REVERSE;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_PLAY, dwFlags, (DWORD)(LPSTR)&mciplay )) )
	{
//hide these from the users, the caller needs to handle this
//	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCIPause( WORD wDeviceID )
//***************************************************************************
{
DWORD dwRes;

if ( !wDeviceID )
	return FALSE;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_PAUSE, 0, (DWORD)(LPVOID)NULL )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCIResume( WORD wDeviceID )
//***************************************************************************
{
DWORD dwRes;

if ( !wDeviceID )
	return FALSE;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_RESUME, 0, (DWORD)(LPVOID)NULL )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//****************************************************************************
BOOL MCIStop( WORD wDeviceID, BOOL fWait )
//***************************************************************************
{
DWORD dwRes;
DWORD dwFlags = 0;

if ( !wDeviceID )
	return FALSE;

if ( fWait )
	dwFlags |= MCI_WAIT;
if ( (dwRes = mciSendCommand( wDeviceID, MCI_STOP, dwFlags, NULL )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCISetTimeFormat( WORD wDeviceID, DWORD dwFormat )
//***************************************************************************
{
DWORD dwRes;
MCI_SET_PARMS mciSet;

if ( !wDeviceID )
	return FALSE;

mciSet.dwTimeFormat = dwFormat;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPSTR)&mciSet )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCISetVideoPalette( WORD wDeviceID, HPALETTE hPal )
//***************************************************************************
{
DWORD dwRes;
MCI_DGV_SETVIDEO_PARMS mciSet;

if ( !wDeviceID )
	return FALSE;

mciSet.dwValue = (DWORD)(LPSTR)hPal;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_SETVIDEO, MCI_DGV_SETVIDEO_PALHANDLE, (DWORD)(LPSTR)&mciSet )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCISetVideoSpeed( WORD wDeviceID, DWORD dwSpeed )
//***************************************************************************
{
DWORD dwRes;
MCI_DGV_SET_PARMS mciSet;

if ( !wDeviceID )
	return FALSE;

mciSet.dwSpeed = dwSpeed;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_SET, MCI_DGV_SET_SPEED,
	(DWORD)(LPSTR)&mciSet )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCISetVideoOnOff( WORD wDeviceID, BOOL bFlag )
//***************************************************************************
{
DWORD dwRes;
MCI_DGV_SETVIDEO_PARMS mciSet;

if ( !wDeviceID )
	return FALSE;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_SETVIDEO,
	(bFlag ? MCI_SET_ON : MCI_SET_OFF), (DWORD)(LPSTR)&mciSet )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCISetAudioOnOff( WORD wDeviceID, BOOL bFlag )
//***************************************************************************
{
DWORD dwRes;
MCI_DGV_SETAUDIO_PARMS mciSet;

if ( !wDeviceID )
	return FALSE;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_SETAUDIO,
	(bFlag ? MCI_SET_ON : MCI_SET_OFF), (DWORD)(LPSTR)&mciSet )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCISetAudioStream( WORD wDeviceID, DWORD dwStream )
//***************************************************************************
{
DWORD dwRes;
MCI_DGV_SETAUDIO_PARMS mciSet;

if ( !wDeviceID )
	return FALSE;

mciSet.dwValue = dwStream;
mciSet.dwItem = MCI_DGV_SETAUDIO_STREAM;
if ( (dwRes = mciSendCommand( wDeviceID, MCI_SETAUDIO,
	MCI_DGV_SETAUDIO_ITEM|MCI_DGV_SETAUDIO_VALUE, (DWORD)(LPSTR)&mciSet )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCISeek( WORD wDeviceID, long lFrame )
//***************************************************************************
{
DWORD dwRes;
MCI_SEEK_PARMS mciSeek;

if ( !wDeviceID )
	return FALSE;

if ( lFrame < 0 )
	lFrame = 0;

mciSeek.dwTo = lFrame;
mciSeek.dwCallback = NULL;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_SEEK, MCI_TO, (DWORD)(LPSTR)&mciSeek )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCISeekHome( WORD wDeviceID )
//***************************************************************************
{
DWORD dwRes;

if ( !wDeviceID )
	return FALSE;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)NULL )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCISeekEnd( WORD wDeviceID )
//***************************************************************************
{
DWORD dwRes;

if ( !wDeviceID )
	return FALSE;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_SEEK, MCI_SEEK_TO_END, (DWORD)(LPVOID)NULL )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCIStepForward( WORD wDeviceID, int nFrames )
//***************************************************************************
{
DWORD dwRes;
MCI_DGV_STEP_PARMS mciStep;

if ( !wDeviceID )
	return FALSE;

mciStep.dwFrames = nFrames;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_STEP, MCI_DGV_STEP_FRAMES, (DWORD)(LPSTR)&mciStep )) )
	{
	// This function returns an error when you step past the end of the file
	// To avoid constant error messages, do not return the error string
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCIStepReverse( WORD wDeviceID )
//***************************************************************************
{
DWORD dwRes;

if ( !wDeviceID )
	return FALSE;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_STEP, MCI_DGV_STEP_REVERSE, (DWORD)(LPVOID)NULL )) )
	{
	// This function returns an error when you step past the end of the file
	// To avoid constant error messages, do not return the error string
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
BOOL MCIUpdate( WORD wDeviceID, HDC hDC )
//***************************************************************************
{
DWORD dwRes;
MCI_DGV_UPDATE_PARMS mciUpdate;

if ( !wDeviceID )
	return FALSE;

if ( !(mciUpdate.hDC = hDC) )
	return FALSE;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_UPDATE, MCI_DGV_UPDATE_HDC, (DWORD)(LPMCI_DGV_UPDATE_PARMS)&mciUpdate )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}

//***************************************************************************
DWORD MCIGetLength( WORD wDeviceID )
//***************************************************************************
{
DWORD dwRes;
MCI_STATUS_PARMS mciStatus;

if ( !wDeviceID )
	return FALSE;

mciStatus.dwTrack = 1;
mciStatus.dwCallback = NULL;
mciStatus.dwItem = MCI_STATUS_LENGTH;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_STATUS, (DWORD)MCI_STATUS_ITEM,
		(DWORD)(LPMCI_STATUS_PARMS)&mciStatus )) )
	{
	MCIError(dwRes);
	return 0L;
	}

return mciStatus.dwReturn;
}

//***************************************************************************
DWORD MCIGetMode( WORD wDeviceID )
//***************************************************************************
{
DWORD dwRes;
MCI_STATUS_PARMS mciStatus;

if ( !wDeviceID )
	return FALSE;

mciStatus.dwTrack = 1;
mciStatus.dwCallback = NULL;
mciStatus.dwItem = MCI_STATUS_MODE;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_STATUS, (DWORD)MCI_STATUS_ITEM,
		(DWORD)(LPMCI_STATUS_PARMS)&mciStatus )) )
	{
	MCIError(dwRes);
	return 0L;
	}

return mciStatus.dwReturn;
}

//***************************************************************************
DWORD MCIGetPosition( WORD wDeviceID )
//***************************************************************************
{
DWORD dwRes;
MCI_STATUS_PARMS mciStatus;

if ( !wDeviceID )
	return FALSE;

mciStatus.dwTrack = 1;
mciStatus.dwCallback = NULL;
mciStatus.dwItem = MCI_STATUS_POSITION;

if ( (dwRes = mciSendCommand(wDeviceID, MCI_STATUS, (DWORD)MCI_STATUS_ITEM,
		(DWORD)(LPMCI_STATUS_PARMS)&mciStatus )) )
	{
	MCIError(dwRes);
	return 0L;
	}

return mciStatus.dwReturn;
}

//***************************************************************************
BOOL MCIGetSrcRect( WORD wDeviceID, LPRECT lpRect )
//***************************************************************************
{
DWORD dwRes;
MCI_DGV_RECT_PARMS	mciWhere;

if ( !wDeviceID )
	return FALSE;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_WHERE, (DWORD)MCI_DGV_WHERE_SOURCE,
		(DWORD)(LPMCI_DGV_RECT_PARMS)&mciWhere )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

// Note that the rc is not a rect, but a left, top width and height
//*lpRect = mciWhere.rc;
lpRect->left = mciWhere.rc.left;
lpRect->top = mciWhere.rc.top;
lpRect->right = mciWhere.rc.right + mciWhere.rc.left;
lpRect->bottom = mciWhere.rc.bottom + mciWhere.rc.top;
return TRUE;
}

//***************************************************************************
BOOL MCIGetDstRect( WORD wDeviceID, LPRECT lpRect )
//***************************************************************************
{
DWORD dwRes;
MCI_DGV_RECT_PARMS	mciWhere;

if ( !wDeviceID )
	return FALSE;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_WHERE, (DWORD)MCI_DGV_WHERE_DESTINATION,
		(DWORD)(LPMCI_DGV_RECT_PARMS)&mciWhere )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

// Note that the rc is not a rect, but a left, top width and height
//*lpRect = mciWhere.rc;
lpRect->left = mciWhere.rc.left;
lpRect->top = mciWhere.rc.top;
lpRect->right = mciWhere.rc.right + mciWhere.rc.left;
lpRect->bottom = mciWhere.rc.bottom + mciWhere.rc.top;
return TRUE;
}

//****************************************************************************
BOOL MCIClose( WORD wDeviceID )
//***************************************************************************
{
DWORD dwRes;

if ( !wDeviceID )
	return FALSE;

MCILoop( wDeviceID, NO );

if ( (dwRes = mciSendCommand( wDeviceID, MCI_CLOSE, MCI_WAIT, NULL )) )
	{
	MCIError(dwRes);
	return(FALSE);
	}

return(TRUE);
}

//***************************************************************************
BOOL MCICloseAll( void )
//***************************************************************************
{
DWORD dwRes;

if ( (dwRes = mciSendCommand( MCI_ALL_DEVICE_ID, MCI_CLOSE, MCI_WAIT, NULL )) )
	{
	MCIError(dwRes);
	return(FALSE);
	}

return(TRUE);
}

//***************************************************************************
UINT MCIHandleNotify( HWND hWnd, UINT codeNotify, HMCI hDevice )
//***************************************************************************
{
switch( codeNotify )
	{
	case MCI_NOTIFY_SUCCESSFUL:
		{
		// Either close the device, or loop it
		long lFrom, lTo;
		if ( MCIIsLooped(hDevice, &lFrom, &lTo) )
			{
			MCIStop( hDevice, YES/*bWait*/ );
			MCISeek( hDevice, 0 );
			if ( !MCIPlay( hDevice, hWnd, lFrom, lTo ) )
				MCIClose( hDevice );
			}
		else
			MCIClose( hDevice );
		return FALSE;
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

return( FALSE );
}

typedef struct _loop
{
	WORD wDeviceID;
	long lFrom;
	long lTo;
	struct _loop far * lpPrev;
	struct _loop far * lpNext;
} LOOP, far * LPLOOP ;

static LPLOOP lpLoopList;

//***************************************************************************
void MCILoop( WORD wDeviceID, BOOL bOn, long lFrom, long lTo )
//***************************************************************************
{
// First see if its in the list
LPLOOP lpLoop = NULL;
LPLOOP lpLoopFind = lpLoopList;
while ( lpLoopFind )
	{
	if ( wDeviceID == lpLoopFind->wDeviceID )
		{ // found it
		lpLoop = lpLoopFind;
		break;
		}
	lpLoopFind = lpLoopFind->lpNext;
	}

if ( bOn )
	{ // turn on looping by adding the device to the loop list
	if ( lpLoop )
    	return; // already in the list
	if ( !(lpLoop = (LPLOOP)Alloc( sizeof(LPLOOP) )) )
		return;
	// add it to the head of the list
	lpLoop->wDeviceID = wDeviceID;
	lpLoop->lFrom = lFrom;
	lpLoop->lTo = lTo;
	lpLoop->lpPrev = NULL;
	lpLoop->lpNext = lpLoopList;
	if ( lpLoopList )
		lpLoopList->lpPrev = lpLoop;
	lpLoopList = lpLoop;
	}
else
	{ // turn off looping by removing the device from the loop list
	if ( !lpLoop )
    	return; // not in the list
	// remove it
	if ( lpLoop->lpPrev )
		lpLoop->lpPrev->lpNext = lpLoop->lpNext;
	else
		lpLoopList = lpLoop->lpNext;
	if ( lpLoop->lpNext )
		lpLoop->lpNext->lpPrev = lpLoop->lpPrev;
	FreeUp( (LPTR)lpLoop );
	}
}

//***************************************************************************
BOOL MCIIsLooped( WORD wDeviceID, LPLONG lpFrom, LPLONG lpTo )
//***************************************************************************
{
LPLOOP lpLoop;

lpLoop = lpLoopList;
while ( lpLoop )
	{
	if ( wDeviceID == lpLoop->wDeviceID )
		{
		if ( *lpFrom )
			 *lpFrom = lpLoop->lFrom;
		if ( *lpTo )
			 *lpTo = lpLoop->lTo;
		return( YES );
		}
	lpLoop = lpLoop->lpNext;
	}
return( NO );
}

#ifdef WIN32
	#define HMYMIDIOUT HMIDIOUT
	#define HMYWAVEOUT HWAVEOUT
#else
	#define HMYMIDIOUT UINT
	#define HMYWAVEOUT UINT
#endif

static WORD wWaveMin, wWaveMax, wMidiMin, wMidiMax, wAuxMin, wAuxMax;
static BOOL fCalcMinMax = TRUE;

//****************************************************************************
LOCAL void GetVolumeMinMax()
//****************************************************************************
{
	DWORD dwVol, dwOrigVol;
	DWORD dwDevice = 0;

	// only do it once
	if (!fCalcMinMax)
		return;
	fCalcMinMax = FALSE;

	//if (GetApp()->GetWinVersion() != WV_WIN31)
	//	dwDevice = WAVE_MAPPER; // only seems to work in WIN32

	// save wave volume
	waveOutGetVolume((HMYWAVEOUT)dwDevice, &dwOrigVol);
	// determine minimum wave volume setting
	waveOutSetVolume((HMYWAVEOUT)dwDevice, 0);
	waveOutGetVolume((HMYWAVEOUT)dwDevice, &dwVol);
	wWaveMin = LOWORD(dwVol);
	// determine maximum wave volume setting
	waveOutSetVolume((HMYWAVEOUT)dwDevice, 0xFFFFFFFF);
	waveOutGetVolume((HMYWAVEOUT)dwDevice, &dwVol);
	wWaveMax = LOWORD(dwVol);
	// restore wave volume
	waveOutSetVolume((HMYWAVEOUT)dwDevice, dwOrigVol);

	//if (GetApp()->GetWinVersion() != WV_WIN31)
	//	dwDevice = MIDI_MAPPER; // only seems to work in WIN32
	// save midi volume
	midiOutGetVolume((HMYMIDIOUT)dwDevice, &dwOrigVol);
	// determine minimum midi volume setting
	midiOutSetVolume((HMYMIDIOUT)dwDevice, 0);
	midiOutGetVolume((HMYMIDIOUT)dwDevice, &dwVol);
	wMidiMin = LOWORD(dwVol);
	// determine maximum midi volume setting
	midiOutSetVolume((HMYMIDIOUT)dwDevice, 0xFFFFFFFF);
	midiOutGetVolume((HMYMIDIOUT)dwDevice, &dwVol);
	wMidiMax = LOWORD(dwVol);
	// restore midi volume
	midiOutSetVolume((HMYMIDIOUT)dwDevice, dwOrigVol);

	//if (GetApp()->GetWinVersion() != WV_WIN31)
	//	dwDevice = AUX_MAPPER; // only seems to work in WIN32
	// save Aux volume
	auxGetVolume((UINT)dwDevice, &dwOrigVol);
	// determine minimum Aux volume setting
	auxSetVolume((UINT)dwDevice, 0);
	auxGetVolume((UINT)dwDevice, &dwVol);
	wAuxMin = LOWORD(dwVol);
	// determine maximum Aux volume setting
	auxSetVolume((UINT)dwDevice, 0xFFFFFFFF);
	auxGetVolume((UINT)dwDevice, &dwVol);
	wAuxMax = LOWORD(dwVol);
	// restore Aux volume
	auxSetVolume((UINT)dwDevice, dwOrigVol);
}

//****************************************************************************
LOCAL DWORD CalcGetVolume(DWORD dwVol, WORD wMin, WORD wMax)
//****************************************************************************
{
	DWORD dwRange, dwRound, dwReturn;
	WORD wLeft, wRight;
	// scale value from wMin-wMax range to 0-0xFFFF
	dwRange = wMax - wMin;
	if (dwRange)
	{
		dwRound = dwRange / 2;
		wLeft = LOWORD(dwVol) - wMin;
		wLeft = (WORD)((((DWORD)wLeft * 0xFFFFL)+dwRound)/dwRange);
		wRight = HIWORD(dwVol) - wMin;
		wRight = (WORD)((((DWORD)wRight * 0xFFFFL)+dwRound)/dwRange);
		dwReturn = MAKELONG(wLeft, wRight);
	}
	else
		dwReturn = 0;
	return(dwReturn);
}

#pragma optimize( "", off )

//****************************************************************************
LOCAL DWORD CalcSetVolume(DWORD dwVol, WORD wMin, WORD wMax)
//****************************************************************************
{
	DWORD dwReturn, dwTemp, dwRange;
	WORD wLeft, wRight;

	// scale value from 0-xFFFF range to wMin-wMax
	// this code is written step-by-step
	// because the optimizer kept screwing it up.
	dwRange = (DWORD)wMax - (DWORD)wMin;
	dwTemp = LOWORD(dwVol);
	dwTemp *= dwRange;
	dwTemp += 0x7FFFL;
	dwTemp /= 0xFFFFL;
	wLeft = (WORD)dwTemp;
	wLeft += wMin;

	dwTemp = HIWORD(dwVol);
	dwTemp *= dwRange;
	dwTemp += 0x7FFFL;
	dwTemp /= 0xFFFFL;
	wRight = (WORD)dwTemp;
	wRight += wMin;

	dwReturn = MAKELONG(wLeft, wRight);

	return(dwReturn);
}

#pragma optimize( "", on ) 

//****************************************************************************
DWORD MCIGetVolume( LPSTR lpDeviceType )
//****************************************************************************
{
GetVolumeMinMax();
DWORD dwVolume = 0;
// always start with the 0th device
DWORD dwDevice = 0;
if ( !HIWORD( lpDeviceType ) )
	{ // (e.g, (LPSTR)MCI_DEVTYPE_SEQUENCER, MCI_DEVTYPE_WAVEFORM_AUDIO, and MCI_DEVTYPE_CD_AUDIO
	HMCI hDevice = LOWORD( lpDeviceType );
	if ( hDevice == MCI_DEVTYPE_SEQUENCER )
		{
		//if (GetApp()->GetWinVersion() != WV_WIN31)
		//	dwDevice = MIDI_MAPPER; // only seems to work in WIN32
		midiOutGetVolume( (HMYMIDIOUT)dwDevice, &dwVolume );
		dwVolume = CalcGetVolume(dwVolume, wMidiMin, wMidiMax);
		}
	else
	if ( hDevice == MCI_DEVTYPE_WAVEFORM_AUDIO )
		{
		//if (GetApp()->GetWinVersion() != WV_WIN31)
		//	dwDevice = WAVE_MAPPER; // only seems to work in WIN32
		waveOutGetVolume( (HMYWAVEOUT)dwDevice, &dwVolume );
		dwVolume = CalcGetVolume(dwVolume, wWaveMin, wWaveMax);
		}
	else
	if ( hDevice == MCI_DEVTYPE_CD_AUDIO )
		{
		//if (GetApp()->GetWinVersion() != WV_WIN31)
		//	dwDevice = AUX_MAPPER; // only seems to work in WIN32
		auxGetVolume( (UINT)dwDevice, &dwVolume );
		dwVolume = CalcGetVolume(dwVolume, wAuxMin, wAuxMax);
		}
	}
else
	{ // (e.g., (LPSTR)"sequencer" and (LPSTR)"waveaudio"
	if ( *lpDeviceType = 's' ) // MCI_DEVTYPE_SEQUENCER
		{
		//if (GetApp()->GetWinVersion() != WV_WIN31)
		//	dwDevice = MIDI_MAPPER; // only seems to work in WIN32
		midiOutGetVolume( (HMYMIDIOUT)dwDevice, &dwVolume );
		dwVolume = CalcGetVolume(dwVolume, wMidiMin, wMidiMax);
		}
	else
	if ( *lpDeviceType = 'w' ) // MCI_DEVTYPE_WAVEFORM_AUDIO;
		{
		//if (GetApp()->GetWinVersion() != WV_WIN31)
		//	dwDevice = WAVE_MAPPER; // only seems to work in WIN32
		waveOutGetVolume( (HMYWAVEOUT)dwDevice, &dwVolume );
		dwVolume = CalcGetVolume(dwVolume, wWaveMin, wWaveMax);
		}
	else
	if ( *lpDeviceType = 'c' ) // MCI_DEVTYPE_CD_AUDIO;
		{
		//if (GetApp()->GetWinVersion() != WV_WIN31)
		//	dwDevice = AUX_MAPPER; // only seems to work in WIN32
		auxGetVolume( (UINT)dwDevice, &dwVolume );
		dwVolume = CalcGetVolume(dwVolume, wAuxMin, wAuxMax);
		}
	}

return( dwVolume );
}

//****************************************************************************
void MCISetVolume( LPSTR lpDeviceType, WORD wVolumeLeft, WORD wVolumeRight )
//****************************************************************************
{
GetVolumeMinMax();
DWORD dwVolume = MAKELONG( wVolumeLeft, wVolumeRight );
// always start with the 0th device
DWORD dwDevice = 0;
if ( !HIWORD( lpDeviceType ) )
	{ // (e.g, (LPSTR)MCI_DEVTYPE_SEQUENCER, MCI_DEVTYPE_WAVEFORM_AUDIO, and MCI_DEVTYPE_CD_AUDIO
	HMCI hDevice = LOWORD( lpDeviceType );
	if ( hDevice == MCI_DEVTYPE_SEQUENCER )
		{
		//if (GetApp()->GetWinVersion() != WV_WIN31)
		//	dwDevice = MIDI_MAPPER; // only seems to work in WIN32
		dwVolume = CalcSetVolume(dwVolume, wMidiMin, wMidiMax);
		midiOutSetVolume( (HMYMIDIOUT)dwDevice, dwVolume );
		}
	else
	if ( hDevice == MCI_DEVTYPE_WAVEFORM_AUDIO )
		{
		//if (GetApp()->GetWinVersion() != WV_WIN31)
		//	dwDevice = WAVE_MAPPER; // only seems to work in WIN32
		dwVolume = CalcSetVolume(dwVolume, wWaveMin, wWaveMax);
		waveOutSetVolume( (HMYWAVEOUT)dwDevice, dwVolume );
		}
	else
	if ( hDevice == MCI_DEVTYPE_CD_AUDIO )
		{
		//if (GetApp()->GetWinVersion() != WV_WIN31)
		//	dwDevice = AUX_MAPPER; // only seems to work in WIN32
		dwVolume = CalcSetVolume(dwVolume, wAuxMin, wAuxMax);
		auxSetVolume( (UINT)dwDevice, dwVolume );
		}
	}
else
	{ // (e.g., (LPSTR)"sequencer" and (LPSTR)"waveaudio"
	if ( *lpDeviceType = 's' ) // MCI_DEVTYPE_SEQUENCER
		{
		//if (GetApp()->GetWinVersion() != WV_WIN31)
		//	dwDevice = MIDI_MAPPER; // only seems to work in WIN32
		dwVolume = CalcSetVolume(dwVolume, wMidiMin, wMidiMax);
		midiOutSetVolume( (HMYMIDIOUT)dwDevice, dwVolume );
		}
	else
	if ( *lpDeviceType = 'w' ) // MCI_DEVTYPE_WAVEFORM_AUDIO;
		{
		//if (GetApp()->GetWinVersion() != WV_WIN31)
		//	dwDevice = WAVE_MAPPER; // only seems to work in WIN32
		dwVolume = CalcSetVolume(dwVolume, wWaveMin, wWaveMax);
		waveOutSetVolume( (HMYWAVEOUT)dwDevice, dwVolume );
		}
	else
	if ( *lpDeviceType = 'c' ) // MCI_DEVTYPE_CD_AUDIO;
		{
		//if (GetApp()->GetWinVersion() != WV_WIN31)
		//	dwDevice = AUX_MAPPER; // only seems to work in WIN32
		dwVolume = CalcSetVolume(dwVolume, wAuxMin, wAuxMax);
		auxSetVolume( (UINT)dwDevice, dwVolume );
		}
	}
}

//***************************************************************************
void MCIError( DWORD dwResult )
//***************************************************************************
{
char szString[256];

if ( dwResult == 257 ) // Invalid device ID (comes from too many MCIClose()'s
	return;
if ( mciGetErrorString( dwResult, szString, sizeof(szString)-1 ) )
	MessageBox( GetApp()->GetMainWnd(), szString, "MCI Error", MB_OK );
else
	MessageBox( GetApp()->GetMainWnd(), "Unknown MCI Error", "MCI Error", MB_OK );
}

//***************************************************************************
//	retrieves the number of waveform output devices present in the system.
//	first checks to see if MMSYSTEM is loaded.	So it works in 3.0 as well as 3.1
//***************************************************************************
int cNumAudio(void)
//***************************************************************************
{
PROCINT lpfnwaveOutGetNumDevs;
HINSTANCE hInstWave;
int nRet;
HANDLE hModule;
STRING szString;

if ( !(hModule = GetModuleHandle("mmsystem.dll")) )
	return( 0 );


if ( (hInstWave = PHLoadLibrary("mmsystem.dll")) == NULL)
	return( 0 );

if ( lpfnwaveOutGetNumDevs = (PROCINT)GetProcAddress( hInstWave, "waveOutGetNumDevs" ) )
	nRet = (*lpfnwaveOutGetNumDevs)();
else
	nRet = 0;

wsprintf( szString, "Number of wave devices = %d", nRet );
MessageBox( GetApp()->GetMainWnd(), szString, "MCI Status", MB_OK );

FreeLibrary(hInstWave);
return (nRet);
}

//***************************************************************************
BOOL MCISetDestination( WORD wDeviceID, int x, int y )
//***************************************************************************
{
	MCI_DGV_RECT_PARMS	mciRect;
	DWORD dwRes;

	if ( (dwRes = mciSendCommand(wDeviceID, MCI_WHERE, MCI_DGV_WHERE_DESTINATION,
			(DWORD)(LPMCI_DGV_RECT_PARMS)&mciRect)) )
	{
		MCIError(dwRes);
		return FALSE;
	}

	// Note that the rc is not a rect, but a top left, width and height
	mciRect.rc.left = x;
	mciRect.rc.top	= y;

	if ( (dwRes = mciSendCommand(wDeviceID, MCI_PUT, MCI_DGV_PUT_DESTINATION | MCI_DGV_RECT,
			(DWORD)(LPMCI_DGV_RECT_PARMS)&mciRect)) )
	{
		MCIError(dwRes);
		return FALSE;
	}

	return(TRUE);
}

//***************************************************************************
BOOL MCIClip( WORD wDeviceID, LPRECT lpSrcClipRect, LPRECT lpDstClipRect, int iScale )
//***************************************************************************
{
	MCI_DGV_RECT_PARMS	mciRect;
	DWORD dwRes;
	RECT SrcRect, DstRect;

	if ( !lpSrcClipRect && !lpDstClipRect )
		return( FALSE );

	if ( !lpSrcClipRect )
	{
		SrcRect = *lpDstClipRect;
		ScaleRect( &SrcRect, -iScale );
		lpSrcClipRect = &SrcRect;
	}
	
	if ( !lpDstClipRect )
	{
		DstRect = *lpSrcClipRect;
		ScaleRect( &DstRect, iScale );
		lpDstClipRect = &DstRect;
	}
	
	// Note that the rc is not a rect, but a left, top, width and height
//	mciRect.rc = *lpDstClipRect;
	mciRect.rc.left	  = lpDstClipRect->left;
	mciRect.rc.top	  = lpDstClipRect->top;
	mciRect.rc.right  = lpDstClipRect->right  - lpDstClipRect->left;
	mciRect.rc.bottom = lpDstClipRect->bottom - lpDstClipRect->top;

	if ( (dwRes = mciSendCommand(wDeviceID, MCI_PUT, MCI_DGV_PUT_DESTINATION | MCI_DGV_RECT, (DWORD)(LPMCI_DGV_RECT_PARMS)&mciRect)) )
	{
		MCIError(dwRes);
		return FALSE;
	}

	// Note that the rc is not a rect, but a left, top, width and height
//	mciRect.rc = *lpSrcClipRect;
	mciRect.rc.left	  = lpSrcClipRect->left;
	mciRect.rc.top	  = lpSrcClipRect->top;
	mciRect.rc.right  = lpSrcClipRect->right  - lpSrcClipRect->left;
	mciRect.rc.bottom = lpSrcClipRect->bottom - lpSrcClipRect->top;

	if ( (dwRes = mciSendCommand(wDeviceID, MCI_PUT, MCI_DGV_PUT_SOURCE | MCI_DGV_RECT, (DWORD)(LPMCI_DGV_RECT_PARMS)&mciRect)) )
	{
		MCIError(dwRes);
		return FALSE;
	}

	return(TRUE);
}

//DRVCALLBACK
//***************************************************************************
//void CALLBACK EXPORT MCICallback( HDRVR h, UINT uMessage, DWORD dwUser, DWORD dwParam1, DWORD dwParam2 )
//***************************************************************************
//{
//MessageBeep(0);
//}
