#include <windows.h>
#include "proto.h"
#include "jungle.h"
#include "control.h"

// The seventh guest puzzle

static BOOL bExpert;
static int nExposed;

//***********************************************************************
static void ToggleState( HWND hWnd, ITEMID id )
//***********************************************************************
{
	int iState = GetWindowWord( GetDlgItem(hWnd, id), GWW_STATE );
	SendMessage( GetDlgItem(hWnd, id), SM_SETSTATE, !iState, 0L);
	if ( !iState )
		nExposed++;
	else
		nExposed--;
}

//***********************************************************************
void CJungleScene::ShowPuzzle2Buttons( HWND hWnd, BOOL bShow )
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

	nExposed = 0;
	static int iTryNum;
	if ( iTryNum == 0 )
	{
		if ( bExpert )
		{
			ToggleState( hWnd, IDC_PUZ_BTN1 );
			ToggleState( hWnd, IDC_PUZ_BTN5 );
			ToggleState( hWnd, IDC_PUZ_BTN9 );
		}
		else
		{
			ToggleState( hWnd, IDC_PUZ_BTN1 );
			ToggleState( hWnd, IDC_PUZ_BTN5 );
			ToggleState( hWnd, IDC_PUZ_BTN9 );
			ToggleState( hWnd, IDC_PUZ_BTN3 );
			ToggleState( hWnd, IDC_PUZ_BTN7 );
		}
	}
	else
	if ( iTryNum == 1 )
	{
		if ( bExpert )
		{
			ToggleState( hWnd, IDC_PUZ_BTN1 );
			ToggleState( hWnd, IDC_PUZ_BTN3 );
			ToggleState( hWnd, IDC_PUZ_BTN5 );
		}
		else
		{
			ToggleState( hWnd, IDC_PUZ_BTN5 );
		}
	}
	else
	if ( iTryNum == 2 )
	{
		if ( bExpert )
		{
			ToggleState( hWnd, IDC_PUZ_BTN5 );
			ToggleState( hWnd, IDC_PUZ_BTN8 );
		}
		else
		{
			ToggleState( hWnd, IDC_PUZ_BTN2 );
			ToggleState( hWnd, IDC_PUZ_BTN5 );
			ToggleState( hWnd, IDC_PUZ_BTN8 );
		}
	}
	if ( ++iTryNum > 2 ) iTryNum = 0;

	long lProgress = (METER_MAX * (nExposed+1)) / (9+2);
	SendMessage( GetDlgItem(hWnd, IDC_PUZ_PROGRESS), SM_SETPOSITION, 0, lProgress );
}

//***********************************************************************
void CJungleScene::HandlePuzzle2Buttons( HWND hWnd, ITEMID id )
//***********************************************************************
{
	// Buttons are arranged as follows:
	//	1 2 3
	//	4 5 6
	//	7 8 9

	switch ( id )
	{
		case IDC_PUZ_BTN1:
		{
			ToggleState( hWnd, IDC_PUZ_BTN1 );
			ToggleState( hWnd, IDC_PUZ_BTN2 );
			ToggleState( hWnd, IDC_PUZ_BTN5 );
			ToggleState( hWnd, IDC_PUZ_BTN4 );
			break;
		}
		case IDC_PUZ_BTN2:
		{
			ToggleState( hWnd, IDC_PUZ_BTN1 );
			ToggleState( hWnd, IDC_PUZ_BTN2 );
			ToggleState( hWnd, IDC_PUZ_BTN3 );
			break;
		}
		case IDC_PUZ_BTN3:
		{
			ToggleState( hWnd, IDC_PUZ_BTN3 );
			ToggleState( hWnd, IDC_PUZ_BTN6 );
			ToggleState( hWnd, IDC_PUZ_BTN5 );
			ToggleState( hWnd, IDC_PUZ_BTN2 );
			break;
		}
		case IDC_PUZ_BTN4:
		{
			ToggleState( hWnd, IDC_PUZ_BTN1 );
			ToggleState( hWnd, IDC_PUZ_BTN4 );
			ToggleState( hWnd, IDC_PUZ_BTN7 );
			break;
		}
		case IDC_PUZ_BTN5:
		{
			ToggleState( hWnd, IDC_PUZ_BTN2 );
			ToggleState( hWnd, IDC_PUZ_BTN5 );
			ToggleState( hWnd, IDC_PUZ_BTN8 );
			ToggleState( hWnd, IDC_PUZ_BTN4 );
			ToggleState( hWnd, IDC_PUZ_BTN6 );
			break;
		}
		case IDC_PUZ_BTN6:
		{
			ToggleState( hWnd, IDC_PUZ_BTN3 );
			ToggleState( hWnd, IDC_PUZ_BTN6 );
			ToggleState( hWnd, IDC_PUZ_BTN9 );
			break;
		}
		case IDC_PUZ_BTN7:
		{
			ToggleState( hWnd, IDC_PUZ_BTN7 );
			ToggleState( hWnd, IDC_PUZ_BTN4 );
			ToggleState( hWnd, IDC_PUZ_BTN5 );
			ToggleState( hWnd, IDC_PUZ_BTN8 );
			break;
		}
		case IDC_PUZ_BTN8:
		{
			ToggleState( hWnd, IDC_PUZ_BTN7 );
			ToggleState( hWnd, IDC_PUZ_BTN8 );
			ToggleState( hWnd, IDC_PUZ_BTN9 );
			break;
		}
		case IDC_PUZ_BTN9:
		{
			ToggleState( hWnd, IDC_PUZ_BTN9 );
			ToggleState( hWnd, IDC_PUZ_BTN8 );
			ToggleState( hWnd, IDC_PUZ_BTN5 );
			ToggleState( hWnd, IDC_PUZ_BTN6 );
			break;
		}
	}

	if ( nExposed != 9 )
	{
		// Set progress meter, and get out
		long lProgress = (METER_MAX * (nExposed+1)) / (9+2);
		SendMessage( GetDlgItem(hWnd, IDC_PUZ_PROGRESS), SM_SETPOSITION, 0, lProgress );
		return;
	}

	// Set progress meter to complete, and get out
	SendMessage( GetDlgItem(hWnd, IDC_PUZ_PROGRESS), SM_SETPOSITION, 0, METER_MAX );

	PlaySuccessSound();

	// Simulate an EVENT_DOWN (good response) to the video control
	FORWARD_WM_KEYDOWN (GetDlgItem(hWnd, IDC_VIDEO_GAME), VK_DOWN,
		0/*cRepeat*/, 0/*flags*/, SendMessage);
}
