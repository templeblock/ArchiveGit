#include <windows.h>
#include "proto.h"
#include "jungle.h"
#include "control.h"

// The "Simon" puzzle

static BOOL bExpert;
static int nButtons;
static int iMasterSequence[9];
static int iLength;
static int nMatches;

//***********************************************************************
static void GenerateSequence( void )
//***********************************************************************
{
	// Initialize the variables
	for ( int i = 0; i < nButtons; i++ )
		iMasterSequence[i] = 0;	

	// Generate an order to play the buttons
	int iPosition = 0;
	while ( iPosition < nButtons )
	{
		// Find the next button to add to the sequence
		int iButton = GetRandomNumber(nButtons);
		// Check to see if we've used this number before
		for ( int i = 0; i < iPosition; i++ )
		{
			if ( iButton == iMasterSequence[i] )
				break; // this number is already used, so try another
		}
	
		if ( i == iPosition ) // it's a new button number
		{ // so add it to the sequence
			iMasterSequence[iPosition] = iButton;
			iPosition++;
		}
	}
}

//***********************************************************************
static void PlaySequence( HWND hWnd, int iPosition )
//***********************************************************************
{
	UpdateWindow( hWnd );
	for ( int i = 0; i < nButtons; i++ )
	{
		ITEMID id = IDC_PUZ_BTN1 + 2*i;
		// Cover all of the exposed buttons
		SendMessage( GetDlgItem(hWnd, id), SM_SETSTATE, 0, 0L);
		ShowWindow( GetDlgItem(hWnd, id), SW_SHOW );
	}
	for ( i = 0; i <= iPosition; i++ )
	{
		ITEMID id = IDC_PUZ_BTN1 + 2*iMasterSequence[i];
		// Expose the button
		SendMessage( GetDlgItem(hWnd, id), SM_SETSTATE, 1, 0L);
		Delay( bExpert ? 750 : 1000 );
	}
//	for ( i = 0; i <= iPosition; i++ )
//	{
//		ITEMID id = IDC_PUZ_BTN1 + 2*iMasterSequence[i];
//		// Cover the button
//		SendMessage( GetDlgItem(hWnd, id), SM_SETSTATE, 0, 0L);
//	}
	FlushMouseMessages();
}

//***********************************************************************
void CJungleScene::ShowPuzzle5Buttons( HWND hWnd, BOOL bShow )
//***********************************************************************
{
	static int iPosX[12] =
	{ 240, 293, 346, 240, 293, 346, 240, 293, 346,   0,   0,   0 };
	static int iPosY[12] =
	{ 357, 357, 357, 398, 398, 398, 439, 439, 439,   0,   0,   0 };

	bExpert = ((LPJUNGLEAPP)GetApp())->GetExpert();
	if ( bExpert )
		nButtons = 9;
	else
		nButtons = 6;

	for (int idx = 0; idx < nButtons; idx++)
	{
		HWND hControl = GetDlgItem(hWnd, IDC_PUZ_BTN1 + 2*idx);
		SetWindowWord( hControl, GWW_STATE, 0 );
		if ( bShow )
		{
			SetWindowPos( hControl, NULL, iPosX[idx], iPosY[idx] + (bExpert ? 0 : 20), 0, 0,
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE );
			ShowWindow( hControl, SW_SHOW );
		}
		else
			ShowWindow( hControl, SW_HIDE );
	}

	for (idx = nButtons; idx < 12; idx++)
	{
		HWND hControl = GetDlgItem(hWnd, IDC_PUZ_BTN1 + 2*idx);
		ShowWindow( hControl, SW_HIDE );
	}

	GenerateSequence();
	nMatches = 0;
	iLength = 0;
	PlaySequence( hWnd, iLength );
}

//***********************************************************************
void CJungleScene::HandlePuzzle5Buttons( HWND hWnd, ITEMID id )
//***********************************************************************
{
	int i = (id - IDC_PUZ_BTN1) / 2;
	if ( i != iMasterSequence[nMatches] )
	{ // Not the next in sequence, so you lose; unhide and cover them all, and get out
		PlayWrongSound();
		Delay( 1000 );
		// Zero out the progress meter
		SendMessage( GetDlgItem(hWnd, IDC_PUZ_PROGRESS), SM_SETPOSITION, 0, 0L );
		nMatches = 0;
		iLength = 0;
		PlaySequence( hWnd, iLength );
		return;
	}

	// The button is next in the sequence, you got a hit!
	// Expose the button (state = 0)
	ClearBackground( hWnd );
	ShowWindow( GetDlgItem(hWnd, id), SW_HIDE );
	UpdateWindow( hWnd );
//	SendMessage( GetDlgItem(hWnd, id), SM_SETSTATE, 0, 0L);

	// If we are not done with this length...
	if ( ++nMatches <= iLength )
		return;

	// We are done with this length...
	if ( ++iLength < nButtons )
	{ // Set progress meter, and get out
		PlayRightSound();
		Delay( 1000 );
		long lProgress = (METER_MAX * (iLength+1)) / (nButtons+2);
		SendMessage( GetDlgItem(hWnd, IDC_PUZ_PROGRESS), SM_SETPOSITION, 0, lProgress );
		nMatches = 0;
		PlaySequence( hWnd, iLength );
		return;
	}

	// You got all "nButtons", so you win!

	// Set progress meter complete to be complete, and get out
	SendMessage( GetDlgItem(hWnd, IDC_PUZ_PROGRESS), SM_SETPOSITION, 0, METER_MAX );

	PlaySuccessSound();

	// Simulate an EVENT_DOWN (good response) to the video control
	FORWARD_WM_KEYDOWN (GetDlgItem(hWnd, IDC_VIDEO_GAME), VK_DOWN,
		0/*cRepeat*/, 0/*flags*/, SendMessage);
}
