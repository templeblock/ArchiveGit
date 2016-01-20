#include <windows.h>
#include <mmsystem.h>
#include "proto.h"
#include "jungle.h"
#include "control.h"

// The "Whack-a-monk" puzzle

static BOOL bExpert;
static int iExposed;
static int nMatches;
static long lTimerStart;
static long lTimerDelay;

//***********************************************************************
void CJungleScene::ShowPuzzle4Buttons( HWND hWnd, BOOL bShow )
//***********************************************************************
{
	static int iPosX[12] =
	{ 240, 293, 346, 240, 293, 346, 240, 293, 346,   0,   0,   0 };
	static int iPosY[12] =
	{ 357, 357, 357, 398, 398, 398, 439, 439, 439,   0,   0,   0 };

	bExpert = ((LPJUNGLEAPP)GetApp())->GetExpert();

	for (int idx = 0; idx < 9; idx++)
	{
		HWND hControl = GetDlgItem(hWnd, IDC_PUZ_BTN1 + 2*idx);
		SetWindowWord( hControl, GWW_STATE, 0 );
		if ( bShow )
		{
			SetWindowPos( hControl, NULL, iPosX[idx], iPosY[idx], 0, 0,
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE );
			ShowWindow( hControl, SW_SHOW );
		}
		else
			ShowWindow( hControl, SW_HIDE );
	}

	for (idx = 9; idx < 12; idx++)
	{
		HWND hControl = GetDlgItem(hWnd, IDC_PUZ_BTN1 + 2*idx);
		ShowWindow( hControl, SW_HIDE );
	}

	iExposed = 0;
	nMatches = 0;
	lTimerStart = 0;
	lTimerDelay = 0;
}

//***********************************************************************
void CJungleScene::HandlePuzzle4Buttons( HWND hWnd, ITEMID id )
//***********************************************************************
{
	int iState = GetWindowWord( GetDlgItem(hWnd, id), GWW_STATE );
	if ( !iState )
	{ // Not exposed, so you lose; unhide and cover them all, and get out
		PlayWrongSound();
		nMatches = 0;
		// Zero out the progress meter
		SendMessage( GetDlgItem(hWnd, IDC_PUZ_PROGRESS), SM_SETPOSITION, 0, 0L );
		for ( int i = 0; i < 9; i++ )
		{
			id = IDC_PUZ_BTN1 + 2*i;
			// Cover the exposed button (state = 0)
			SendMessage( GetDlgItem(hWnd, id), SM_SETSTATE, 0, 0L);
			ShowWindow( GetDlgItem(hWnd, id), SW_SHOW );
		}
		return;
	}

	// The button is exposed, you got a hit!
	PlayRightSound();

	// Hide the button
	ClearBackground( hWnd );
	ShowWindow( GetDlgItem(hWnd, id), SW_HIDE );
	UpdateWindow( hWnd );

	// If we are not done yet...
	if ( ++nMatches < 9)
	{ // Set progress meter, and get out
		long lProgress = (METER_MAX * (nMatches+1)) / (9+2);
		SendMessage( GetDlgItem(hWnd, IDC_PUZ_PROGRESS), SM_SETPOSITION, 0, lProgress );
		return;
	}

	// Set progress meter complete to be complete, and get out
	SendMessage( GetDlgItem(hWnd, IDC_PUZ_PROGRESS), SM_SETPOSITION, 0, METER_MAX );

	PlaySuccessSound();

	// Simulate an EVENT_DOWN (good response) to the video control
	FORWARD_WM_KEYDOWN (GetDlgItem(hWnd, IDC_VIDEO_GAME), VK_DOWN,
		0/*cRepeat*/, 0/*flags*/, SendMessage);
}

//***********************************************************************
void CJungleScene::HandlePuzzle4Timer( HWND hWnd )
//***********************************************************************
{
	if ( (long)timeGetTime() - lTimerStart < lTimerDelay )
		return;
	// Reset the timer
	lTimerStart = timeGetTime();
	lTimerDelay = (bExpert ? 750 : 1000);

	ITEMID id;
	if ( iExposed )
	{ // Cover the exposed button (state = 0)
		id = IDC_PUZ_BTN1 + 2*(iExposed - 1);
		SendMessage( GetDlgItem(hWnd, id), SM_SETSTATE, 0, 0L);
	}

	// Find a new button to expose
	int iLastExposed = iExposed;
	iExposed = 1 + GetRandomNumber(9);
	for ( int i = 0; i < 9; i++ )
	{
		if ( iExposed != iLastExposed )
		{
			id = IDC_PUZ_BTN1 + 2*(iExposed - 1);
			if ( IsWindowVisible( GetDlgItem(hWnd, id) ) )
				break;
		}
		// Try the next one
		if ( ++iExposed > 9 )
			iExposed = 1;
	}

	if ( !IsWindowVisible( GetDlgItem(hWnd, id) ) )
		return;

	// Expose a new button (state = 1)
	SendMessage( GetDlgItem(hWnd, id), SM_SETSTATE, 1, 0L);
}
