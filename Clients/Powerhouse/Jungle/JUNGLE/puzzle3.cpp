#include <windows.h>
#include "proto.h"
#include "jungle.h"
#include "control.h"

// The Matching Game

static BOOL bExpert;
static int iMatches[12];
static ITEMID idInProgress;
static int nMatches;

//***********************************************************************
void CJungleScene::ShowPuzzle3Buttons( HWND hWnd, BOOL bShow )
//***********************************************************************
{
	static int iPosX[12] =
	{ 211, 264, 317, 370, 423, 476, 211, 264, 317, 370, 423, 476 };
	static int iPosY[12] =
	{ 364, 364, 364, 364, 364, 364, 408, 408, 408, 408, 408, 408 };

	bExpert = ((LPJUNGLEAPP)GetApp())->GetExpert();

	for (int idx = 0; idx < 12; idx++)
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

	idInProgress = NULL;
	nMatches = 0;
}

//***********************************************************************
void CJungleScene::HandlePuzzle3Buttons( HWND hWnd, ITEMID id )
//***********************************************************************
{
	// If the button is already exposed...
	int iState = GetWindowWord( GetDlgItem(hWnd, id), GWW_STATE );
	if ( iState )
		{ // Cover the button (state = 0), and get out
		SendMessage( GetDlgItem(hWnd, id), SM_SETSTATE, 0, 0L);
		idInProgress = NULL;
		return;
		}
	// Not exposed yet

	// If this is the first button to expose (not in progress)...
	if ( !idInProgress )
		{ // Expose the button (state = 1), and get out
		SendMessage( GetDlgItem(hWnd, id), SM_SETSTATE, 1, 0L);
		idInProgress = id;
		return;
		}

	// Delay a bit so we can see the second icon exposed
	Delay( 500L );

	// If the button is not exposed, and we are in progress...
	int id1 = GetWindowWord( GetDlgItem(hWnd, id), GWW_ICONID );
	if ( !id1 )
		id1 = id;
	int id2 = GetWindowWord( GetDlgItem(hWnd, idInProgress), GWW_ICONID );
	if ( !id2 )
		id2 = idInProgress;

	// If they are not a match...
	if ( (id1 + id2) != (IDC_PUZ_BTN1 + IDC_PUZ_BTN12) )
	{ // Cover both buttons (state = 0), and get out
		SendMessage( GetDlgItem(hWnd, id), SM_SETSTATE, 0, 0L);
		SendMessage( GetDlgItem(hWnd, idInProgress), SM_SETSTATE, 0, 0L);
		idInProgress = NULL;
		PlayWrongSound();
		return;
	}

	// Since we have a match, hide both icons
	ClearBackground( hWnd );
	ShowWindow( GetDlgItem(hWnd, id), SW_HIDE );
	ShowWindow( GetDlgItem(hWnd, idInProgress), SW_HIDE );
	UpdateWindow( hWnd );
	idInProgress = NULL;
	PlayRightSound();

	// If we are not done yet...
	if ( ++nMatches < 6)
	{ // Set progress meter, and get out
		long lProgress = (METER_MAX * (nMatches+1)) / (6+2);
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
