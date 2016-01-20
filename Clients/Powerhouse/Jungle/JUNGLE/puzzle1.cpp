#include <windows.h>
#include "proto.h"
#include "jungle.h"
#include "control.h"

static int nMatches;

#define NUM_PUZZLE_BUTTONS	12
//***********************************************************************
void CJungleScene::ShowPuzzle1Buttons( HWND hWnd, BOOL bShow )
//***********************************************************************
{
	static int iPosX[12] =
	{ 211, 264, 317, 370, 423, 476, 211, 264, 317, 370, 423, 476 };
	static int iPosY[12] =
	{ 364, 364, 364, 364, 364, 364, 408, 408, 408, 408, 408, 408 };

	for (int idx = 0 ; idx < NUM_PUZZLE_BUTTONS ; idx++)
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

	nMatches = 0;
}

// Puzzle button syllable table for voice reco decision points
static VRECOINFO m_PuzzleInfo[] =
{
//	Shot	Number	VRECO	Syllable1		Syllable2		Syllable3
//	Number	syll's	uCode	Sound ID		Sound ID		Sound ID
	 3,		3,		3,		IDC_PUZ_BTN6,	IDC_PUZ_BTN2,	IDC_PUZ_BTN6,	0,	0,
	18,		3,		3,		IDC_PUZ_BTN6,	IDC_PUZ_BTN2,	IDC_PUZ_BTN6,	0,	0,
	22,		3,		3,		IDC_PUZ_BTN6,	IDC_PUZ_BTN2,	IDC_PUZ_BTN6,	0,	0,
	28,		3,		3,		IDC_PUZ_BTN6,	IDC_PUZ_BTN2,	IDC_PUZ_BTN6,	0,	0,
	30,		3,		3,		IDC_PUZ_BTN6,	IDC_PUZ_BTN2,	IDC_PUZ_BTN6,	0,	0,
	46,		3,		3,		IDC_PUZ_BTN6,	IDC_PUZ_BTN2,	IDC_PUZ_BTN6,	0,	0,
	50,		3,		3,		IDC_PUZ_BTN6,	IDC_PUZ_BTN2,	IDC_PUZ_BTN6,	0,	0,
	52,		3,		3,		IDC_PUZ_BTN6,	IDC_PUZ_BTN2,	IDC_PUZ_BTN6,	0,	0,
	53,		3,		3,		IDC_PUZ_BTN6,	IDC_PUZ_BTN2,	IDC_PUZ_BTN6,	0,	0,
	55,		3,		3,		IDC_PUZ_BTN6,	IDC_PUZ_BTN2,	IDC_PUZ_BTN6,	0,	0,
};

//***********************************************************************
void CJungleScene::HandlePuzzle1Buttons( HWND hWnd, ITEMID id )
//***********************************************************************
{
	int iSndID = GetWindowWord( GetDlgItem(hWnd, id), GWW_ICONID );
	if ( !iSndID )
		iSndID = id;

	// Get the current shot info from the video
	LPVIDEO lpVideo = (LPVIDEO)GetWindowLong( GetDlgItem(hWnd, IDC_VIDEO_GAME), GWL_DATAPTR );
	if ( !lpVideo)
		return;
	LPSHOT lpShot =  &lpVideo->lpAllShots [lpVideo->lCurrentShot - 1];
	if (!lpShot)
		return;

	// Extract the shot number
	int iShotNum = (int)(lpShot->lFlags & A_VALUE_BITS);
	// How many items in the list?
	int nItems = sizeof(m_PuzzleInfo) / sizeof(VRECOINFO);
	// Get the index of the item matching the shot
	for ( int idx = 0; idx < nItems; idx++ )
	{
		if ( iShotNum == m_PuzzleInfo[idx].iShotNum )
			break;
	}

	if ( idx == nItems )
		return; // Couldn't find the shot in the list

	int iNxtSyllable = m_PuzzleInfo[idx].iSyllables[nMatches];
	int iNumSyllables = m_PuzzleInfo[idx].sNumSyllables;

	// Does the button match the next syllable?
	if (iSndID == iNxtSyllable)
	{
//		int iState = GetWindowWord( GetDlgItem(hWnd, id), GWW_STATE );
//		SendMessage( GetDlgItem(hWnd, id), SM_SETSTATE, !iState, 0L);
		SendMessage( GetDlgItem(hWnd, id), SM_SETSTATE, 1, 0L);

		// Give them instinct points
		ChangeInstinctMeter (hWnd, GOOD_RESPONSE);
		// Set progress meter
		nMatches++;
		long lProgress = (METER_MAX * nMatches) / (long)iNumSyllables;
		SendMessage( GetDlgItem(hWnd, IDC_PUZ_PROGRESS), SM_SETPOSITION, 0, lProgress );
		// Is this the last syllable?
		if (nMatches >= iNumSyllables)
		{
			// Delay so the progress meter is shown
			Delay( 1500 );
			// Simulate an EVENT_DOWN (good response) to the video control
			FORWARD_WM_KEYDOWN (GetDlgItem(hWnd, IDC_VIDEO_GAME), VK_DOWN,
				0/*cRepeat*/, 0/*flags*/, SendMessage);
		}
		return;
	}

	// It is not a match

	// Take instinct points away
	ChangeInstinctMeter (hWnd, BAD_RESPONSE);

//	// Simulate a wrong button pressed
//	FORWARD_WM_KEYDOWN (GetDlgItem(hWnd, IDC_VIDEO_GAME), VK_UP,
//		0/*cRepeat*/, 0/*flags*/, SendMessage);
}
