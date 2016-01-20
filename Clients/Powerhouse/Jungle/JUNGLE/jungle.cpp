// DOWN is the correct answer for the voice puzzles; UP is incorrect
#include <windows.h>
#define MAIN
#include <mmsystem.h>
#include "proto.h"
#include "sound.h"
#include "jungle.h"
#include "vreco.h"
#include "control.h"
#include "credits.h"
#include "randseq.h"

// INI file stuff
#define INI_FILE			"JUNGLE.INI"
#define OPTIONS_SECTION		"Options"

// Test file for setting volume
#define TEST_WAVE			"goodbye.wav"
#define TEST_MIDI			"jungle.mid"

// Idle constants
#define IDLE_DELAY			10000 // 10 seconds

// Locals
static BOOL	bVoiceRecoOn;			// Flag to indicate if voice reco is active or not
static BOOL bVoiceRecoInit;
static long	m_lInstinctMeter;
static long	m_lResumeInstinctMeter;
static SHOTID m_lSaveShot;
static BOOL	m_bOptionsfromGame;
static int iPuzzleShown, iPuzzleDisk;

// Control table for decision point bitmaps
static CONTINFO m_ContInfo[] =
{
//	Shot	Left			Up				Right			Down			Home
//	Number	ID				ID				ID				ID				ID
	5,		IDC_DP5_L,		IDC_DP5_U,		IDC_DP5_R,		0,				0,
	7,		IDC_DP7_L,		0,				IDC_DP7_R,		0,				0,
	8,		IDC_DP8_L,		0,				IDC_DP8_R,		0,				0,
	10,		IDC_DP10_L,		IDC_DP10_U,		IDC_DP10_R,		0,				0,
	13,		IDC_DP13_L,		IDC_DP13_U,		IDC_DP13_R,		IDC_DP13_D,		IDC_DP13_H,
	25,		IDC_DP25_L,		IDC_DP25_U,		IDC_DP25_R,		0,				0,
	32,		IDC_DP32_L,		IDC_DP32_U,		IDC_DP32_R,		0,				0,
	43,		IDC_DP43_L,		0,				IDC_DP43_R,		0,				0,
	44,		IDC_DP44_L,		0,				IDC_DP44_R,		0,				0,
	45,		IDC_DP45_L,		0,				IDC_DP45_R,		0,				0,
	48,		IDC_DP48_L,		0,				IDC_DP48_R,		0,				0,
	49,		IDC_DP49_L,		0,				IDC_DP49_R,		0,				0,
	57,		IDC_DP57_L,		0,				IDC_DP57_R,		0,				0,
	59,		IDC_DP59_L,		IDC_DP59_U,		IDC_DP59_R,		0,				0,
	60,		IDC_DP60_L,		0,				IDC_DP60_R,		0,				0,
	69,		IDC_DP69_L,		IDC_DP69_U,		IDC_DP69_R,		IDC_DP69_D,		0,
	71,		IDC_DP71_L,		0,				IDC_DP71_R,		0,				0,
	76,		IDC_DP76_L,		0,				IDC_DP76_R,		0,				0,
	77,		IDC_DP77_L,		0,				IDC_DP77_R,		0,				0,
	79,		IDC_DP79_L,		0,				IDC_DP79_R,		0,				0,
	82,		IDC_DP82_L,		0,				IDC_DP82_R,		0,				0,
	101,	IDC_DP101_L,	IDC_DP101_U,	IDC_DP101_R,	IDC_DP101_D,	IDC_DP101_H,
	};

// Start or stop a midi file
//************************************************************************
void StartMidi(BOOL bTurnOn, LPSTR lpFile)
//************************************************************************
{
	static HMCI hPlayMidi;

	if ( !bTurnOn )
	{ // turn the sound off
		if ( hPlayMidi )
		{ // If open, close it
			MCIClose( hPlayMidi );
			hPlayMidi = NULL;
		}
	}
	else
	{ // turn the sound on
		if ( hPlayMidi )
			return; // If already playing, return

		if ( hPlayMidi = MCIOpen(GetApp()->m_hDeviceMID, lpFile, NULL/*lpAlias*/) )
		{
			if ( MCIPlay( hPlayMidi, GetApp()->GetMainWnd() ) )
				MCILoop( hPlayMidi, TRUE );
			else
			{
				MCIClose( hPlayMidi );
				hPlayMidi = NULL;
			}
		}
	}
}

// Start or stop a wave file
//************************************************************************
void StartWave(BOOL bTurnOn, LPSTR lpFile)
//************************************************************************
{
	static HMCI hPlayWave;

	if ( !bTurnOn )
	{ // turn the sound off
		if ( hPlayWave )
		{ // If open, close it
			MCIClose( hPlayWave );
			hPlayWave = NULL;
		}
	}
	else
	{ // turn the sound on
		if ( hPlayWave )
			return; // If already playing, return

		if ( hPlayWave = MCIOpen(GetApp()->m_hDeviceWAV, lpFile, NULL/*lpAlias*/) )
		{
			if ( MCIPlay( hPlayWave, GetApp()->GetMainWnd() ) )
				MCILoop( hPlayWave, TRUE );
			else
			{
				MCIClose( hPlayWave );
				hPlayWave = NULL;
			}
		}
	}
}
 
//************************************************************************
CJungleScene::CJungleScene()
//************************************************************************
{
	m_dwIdleStartTime = 0;
	m_dwIdleDelay = 0;
	m_idIdleItem = 0; // reset to the generic idle message
	m_iWrongSound = GetRandomNumber(NUM_WRONG_SOUNDS);
	m_iRightSound = GetRandomNumber(NUM_RIGHT_SOUNDS);
	m_iSuccessSound = GetRandomNumber(NUM_SUCCESS_SOUNDS);
}

//************************************************************************
BOOL CJungleScene::OnGameSceneInit( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	// Initialize the instinct meter
	if ( m_lResumeInstinctMeter )
	{
		m_lInstinctMeter = m_lResumeInstinctMeter;
		m_lResumeInstinctMeter = 0;
	}
	else
		m_lInstinctMeter = DEFAULT_INSTINCT;

	// Set the iniital instinct meter value
	SendMessage( GetDlgItem(hWnd, IDC_INSTINCT_METER), SM_SETPOSITION, 0, m_lInstinctMeter );

	// Hide the scene puzzle buttons
	ShowPuzzleButtons(hWnd, NULL, FALSE );

	// Hide the "event" buttons
	ShowWindow( GetDlgItem(hWnd, IDC_LEFT),	 SW_HIDE );
	ShowWindow( GetDlgItem(hWnd, IDC_UP),	 SW_HIDE );
	ShowWindow( GetDlgItem(hWnd, IDC_RIGHT), SW_HIDE );
	ShowWindow( GetDlgItem(hWnd, IDC_DOWN),	 SW_HIDE );
	ShowWindow( GetDlgItem(hWnd, IDC_HOME),	 SW_HIDE );

	// Hide the combination digit controls
	DisplayComboControls( hWnd, NULL );

	HWND hVideo = GetDlgItem( hWnd, IDC_VIDEO_GAME );
	GetApp()->SetJoyMessageMap( 3,		hVideo, WM_KEYDOWN, VK_PAUSE, 0L );
	GetApp()->SetJoyMessageMap( 1,		  hWnd, WM_COMMAND, IDC_EXIT, 0L );
	GetApp()->SetJoyMessageMap( 2,		  hWnd, WM_COMMAND, IDC_OPTIONS, 0L );
	GetApp()->SetJoyMessageMap( VK_LEFT,  hWnd, WM_COMMAND, IDC_LEFT, 0L );
	GetApp()->SetJoyMessageMap( VK_UP,	  hWnd, WM_COMMAND, IDC_UP, 0L );
	GetApp()->SetJoyMessageMap( VK_RIGHT, hWnd, WM_COMMAND, IDC_RIGHT, 0L );
	GetApp()->SetJoyMessageMap( VK_DOWN,  hWnd, WM_COMMAND, IDC_DOWN, 0L );
	GetApp()->SetKeyMapEntry( hVideo, VK_PAUSE, WM_KEYDOWN, VK_PAUSE, 0L, YES/*fOnDown*/);
	GetApp()->SetKeyMapEntry( hVideo, VK_LEFT,	WM_KEYDOWN, VK_LEFT,  0L, YES/*fOnDown*/);
	GetApp()->SetKeyMapEntry( hVideo, VK_RIGHT, WM_KEYDOWN, VK_RIGHT, 0L, YES/*fOnDown*/);
	GetApp()->SetKeyMapEntry( hVideo, VK_UP,	WM_KEYDOWN, VK_UP,	  0L, YES/*fOnDown*/);
	GetApp()->SetKeyMapEntry( hVideo, VK_DOWN,	WM_KEYDOWN, VK_DOWN,  0L, YES/*fOnDown*/);
	GetApp()->SetKeyMapEntry( hVideo, VK_HOME,	WM_KEYDOWN, VK_HOME,  0L, YES/*fOnDown*/);
	GetApp()->SetKeyMapEntry( hVideo, VK_SPACE, WM_KEYDOWN, VK_SPACE, 0L, YES/*fOnDown*/);

	return(YES);
}

//***********************************************************************
void CJungleScene::RandomizePuzzleButtons( HWND hWnd )
//***********************************************************************
{
	#define NUM_PUZZLE_BUTTONS 12

	int iSeed = GetRandomNumber( NUM_PUZZLE_BUTTONS );
	CRandomSequence Sequence( NUM_PUZZLE_BUTTONS, iSeed );
	for ( int idx = 0; idx < NUM_PUZZLE_BUTTONS; idx++ )
	{
		int iNewIdx = (int)Sequence.GetNextNumber();
		if ( iNewIdx >= NUM_PUZZLE_BUTTONS )
			iNewIdx = NUM_PUZZLE_BUTTONS - 1;
		if ( iNewIdx < 0 )
			iNewIdx = 0;
		SetWindowWord( GetDlgItem(hWnd, IDC_PUZ_BTN1  + (2 * idx)), GWW_ICONID,
										IDC_PUZ_BTN1  + (2 * iNewIdx) );
	}
}

//************************************************************************
void CJungleScene::DisplayPuzzleButtons( HWND hWnd, LPSHOT lpShot )
//************************************************************************
{
	if ( !lpShot)
		return;

	// If this is a puzzle shot...
	if ( lpShot->lFlags & A_PUZZLE )
	{
		// show the puzzle buttons
		if ( !m_bPuzzleButtonsOn )
		{
			m_dwIdleStartTime = 0;
			m_dwIdleDelay = 0;
			m_idIdleItem = 0; // reset to the generic idle message
			// randomize puzzle buttons
			RandomizePuzzleButtons(hWnd);
			ShowPuzzleButtons(hWnd, lpShot, TRUE);
		}
	}
	else // if not a puzzle shot...
	{ // hide the puzzle buttons
		if ( m_bPuzzleButtonsOn )
			ShowPuzzleButtons(hWnd, lpShot, FALSE);
	}
}

//***********************************************************************
void CJungleScene::ShowPuzzleButtons( HWND hWnd, LPSHOT lpShot, BOOL bShow )
//***********************************************************************
{
	if ( !bShow )
	{
		// Hide all the puzzle buttons
		for (int idx = 0; idx < 12; idx++)
		{
			HWND hControl = GetDlgItem(hWnd, IDC_PUZ_BTN1 + 2*idx);
			ShowWindow( hControl, SW_HIDE );
		}

		// Hide the progress meter
		ShowWindow( GetDlgItem(hWnd, IDC_PUZ_PROGRESS), SW_HIDE );
	}
	else
	{
		int iShotNum;
		if ( !lpShot )
			iShotNum = 0;
		else // Extract the shot number
			iShotNum = (int)(lpShot->lFlags & A_VALUE_BITS);

		ITEMID idProgress = NULL;
		iPuzzleShown = 0;

		switch ( iShotNum )
		{
			case 3:
			{
				ShowPuzzle3Buttons( hWnd, bShow );
				iPuzzleShown = 3; // Puzzle 3 - Matching game
				iPuzzleDisk = 1;
				idProgress = IDC_VP3;
				break;
			}
			case 18:
			{
				ShowPuzzle4Buttons( hWnd, bShow );
				iPuzzleShown = 4; // Puzzle 4 - Whak-a-mole
				iPuzzleDisk = 1;
				idProgress = IDC_VP18;
				break;
			}
			case 22:
			{
				// Set the progress meter's bitmap
				ShowPuzzle5Buttons( hWnd, bShow );
				iPuzzleShown = 5; // Puzzle 5 - Simon
				iPuzzleDisk = 1;
				idProgress = IDC_VP22;
				break;
			}
			case 28:
			{
				ShowPuzzle3Buttons( hWnd, bShow );
				iPuzzleShown = 3; // Puzzle 3 - Matching game
				iPuzzleDisk = 1;
				idProgress = IDC_VP28;
				break;
			}
			case 30:
			{
				ShowPuzzle2Buttons( hWnd, bShow );
				iPuzzleShown = 2; // Puzzle 2 - Blocker - Seventh Guest
				iPuzzleDisk = 1;
				idProgress = IDC_VP30;
				break;
			}
			case 46:
			{
				ShowPuzzle4Buttons( hWnd, bShow );
				iPuzzleShown = 4; // Puzzle 4 - Whak-a-mole
				iPuzzleDisk = 2;
				idProgress = IDC_VP46;
				break;
			}
			case 50:
			{
				ShowPuzzle5Buttons( hWnd, bShow );
				iPuzzleShown = 5; // Puzzle 5 - Simon
				iPuzzleDisk = 2;
				idProgress = IDC_VP50;
				break;
			}
			case 52:
			{
				ShowPuzzle4Buttons( hWnd, bShow );
				iPuzzleShown = 4; // Puzzle 4 - Whak-a-mole
				iPuzzleDisk = 2;
				idProgress = IDC_VP52;
				break;
			}
			case 53:
			{
				ShowPuzzle3Buttons( hWnd, bShow );
				iPuzzleShown = 3; // Puzzle 3 - Matching game
				iPuzzleDisk = 2;
				idProgress = IDC_VP53;
				break;
			}
			case 55:
			{
				ShowPuzzle2Buttons( hWnd, bShow );
				iPuzzleShown = 2; // Puzzle 2 - Blocker - Seventh Guest
				iPuzzleDisk = 2;
				idProgress = IDC_VP55;
				break;
			}
			default:
			{
				ShowPuzzle2Buttons( hWnd, NO );
				break;
			}
		}

		// Set the progress meter's bitmap
		if ( idProgress )
			SetWindowWord( GetDlgItem(hWnd, IDC_PUZ_PROGRESS), GWW_ICONID, idProgress );
		// Reset progress meter
		SendDlgItemMessage(hWnd, IDC_PUZ_PROGRESS, SM_SETPOSITION, 0, 0L);
		// Show the progress meter
		ShowWindow( GetDlgItem(hWnd, IDC_PUZ_PROGRESS), SW_SHOW );
	}

	// Show or hide the talk button
	ShowWindow( GetDlgItem(hWnd, IDC_TALK_BUTTON), ((bShow & bVoiceRecoInit) ? SW_SHOW : SW_HIDE) );

	UpdateWindow( hWnd );
	m_bPuzzleButtonsOn = bShow;
}

//***********************************************************************
void CJungleScene::HandlePuzzleButtons( HWND hWnd, ITEMID id )
//***********************************************************************
{
	switch( iPuzzleShown )
	{
		case 2: // Puzzle 2 - Blocker - Seventh Guest
		{
			HandlePuzzle2Buttons( hWnd, id );
			break;
		}
		case 3: // Puzzle 3 - Matching game
		{
			HandlePuzzle3Buttons( hWnd, id );
			break;
		}
		case 4: // Puzzle 4 - Whak-a-mole
		{
			HandlePuzzle4Buttons( hWnd, id );
			break;
		}
		case 5: // Puzzle 5 - Simon
		default:
		{
			HandlePuzzle5Buttons( hWnd, id );
			break;
		}
	}
}

// Recopy the clean background bitmap
//***********************************************************************
void CJungleScene::ClearBackground(HWND hWnd)
//***********************************************************************
{
	UpdateWindow( hWnd );

	LPSCENE lpScene = CScene::GetScene(hWnd);
	if ( !lpScene)
		return;
	LPOFFSCREEN lpOffScreen = lpScene->GetOffScreen();
	if (!lpOffScreen)
		return;

	RECT rect;
	SetRect( &rect, 100, 357, 540, 480 );
	lpOffScreen->CopyBits( &rect );
}

//***********************************************************************
void CJungleScene::DisplayDecisionButtons(HWND hWnd, LPSHOT lpShot)
//***********************************************************************
{
	if ( !lpShot )
		return;

	// Is this a decision point shot?
	BOOL bDecision = (BOOL)(lpShot->lFlags & A_DECISION);
	// Extract the shot number
	int iShotNum = (int)(lpShot->lFlags & A_VALUE_BITS);
	// How many items in the list?
	int nItems = sizeof(m_ContInfo) / sizeof(CONTINFO);
	// Get the index of the item matching the shot
	for ( int idx = 0; idx < nItems; idx++ )
	{
		if ( iShotNum == m_ContInfo[idx].iShotNum )
			break;
	}

	if ( !bDecision || idx == nItems )
	{ // Couldn't find the shot in the list
		ShowWindow( GetDlgItem(hWnd, IDC_LEFT),	 SW_HIDE );
		ShowWindow( GetDlgItem(hWnd, IDC_UP),	 SW_HIDE );
		ShowWindow( GetDlgItem(hWnd, IDC_RIGHT), SW_HIDE );
		ShowWindow( GetDlgItem(hWnd, IDC_DOWN),	 SW_HIDE );
		ShowWindow( GetDlgItem(hWnd, IDC_HOME),	 SW_HIDE );
		UpdateWindow( hWnd );
		return;
	}

	HWND hControl;
	ITEMID id;
	static int iPositions[5][5] = {
		276,	  0,	  0,	  0,	  0,
		224,	329,	  0,	  0,	  0,
		168,	276,	384,	  0,	  0,
		119,	224,	329,	434,	  0,
		123,	204,	285,	366,	447,
	};
	
	int iButtonCount = 0;
	int iButtonRow =
		(m_ContInfo[idx].idLeft	 != 0) +
		(m_ContInfo[idx].idUp	 != 0) +
		(m_ContInfo[idx].idRight != 0) +
		(m_ContInfo[idx].idDown	 != 0) +
		(m_ContInfo[idx].idHome	 != 0) - 1;

	hControl = GetDlgItem( hWnd, IDC_LEFT );
	id = m_ContInfo[idx].idLeft;
	if ( !id )
		ShowWindow( hControl, SW_HIDE );
	else
	{
		SetWindowWord( hControl, GWW_ICONID, id );
		RECT rect;
		GetClientRect( hControl, &rect );
		MapWindowPoints( hControl, GetParent(hControl), (LPPOINT)&rect, 2 );
		SetWindowPos( hControl, NULL, iPositions[iButtonRow][iButtonCount], rect.top, 0, 0,
			SWP_NOZORDER | SWP_NOACTIVATE );
		ShowWindow( hControl, SW_SHOW );
		iButtonCount++;
	}
	
	hControl = GetDlgItem( hWnd, IDC_UP );
	id = m_ContInfo[idx].idUp;
	if ( !id )
		ShowWindow( hControl, SW_HIDE );
	else
	{
		SetWindowWord( hControl, GWW_ICONID, id );
		RECT rect;
		GetClientRect( hControl, &rect );
		MapWindowPoints( hControl, GetParent(hControl), (LPPOINT)&rect, 2 );
		SetWindowPos( hControl, NULL, iPositions[iButtonRow][iButtonCount], rect.top, 0, 0,
			SWP_NOZORDER | SWP_NOACTIVATE );
		ShowWindow( hControl, SW_SHOW );
		iButtonCount++;
	}
	
	hControl = GetDlgItem( hWnd, IDC_RIGHT );
	id = m_ContInfo[idx].idRight;
	if ( !id )
		ShowWindow( hControl, SW_HIDE );
	else
	{
		SetWindowWord( hControl, GWW_ICONID, id );
		RECT rect;
		GetClientRect( hControl, &rect );
		MapWindowPoints( hControl, GetParent(hControl), (LPPOINT)&rect, 2 );
		SetWindowPos( hControl, NULL, iPositions[iButtonRow][iButtonCount], rect.top, 0, 0,
			SWP_NOZORDER | SWP_NOACTIVATE );
		ShowWindow( hControl, SW_SHOW );
		iButtonCount++;
	}
	
	hControl = GetDlgItem( hWnd, IDC_DOWN );
	id = m_ContInfo[idx].idDown;
	if ( !id )
		ShowWindow( hControl, SW_HIDE );
	else
	{
		SetWindowWord( hControl, GWW_ICONID, id );
		RECT rect;
		GetClientRect( hControl, &rect );
		MapWindowPoints( hControl, GetParent(hControl), (LPPOINT)&rect, 2 );
		SetWindowPos( hControl, NULL, iPositions[iButtonRow][iButtonCount], rect.top, 0, 0,
			SWP_NOZORDER | SWP_NOACTIVATE );
		ShowWindow( hControl, SW_SHOW );
		iButtonCount++;
	}
	
	hControl = GetDlgItem( hWnd, IDC_HOME );
	id = m_ContInfo[idx].idHome;
	if ( !id )
		ShowWindow( hControl, SW_HIDE );
	else
	{
		SetWindowWord( hControl, GWW_ICONID, id );
		RECT rect;
		GetClientRect( hControl, &rect );
		MapWindowPoints( hControl, GetParent(hControl), (LPPOINT)&rect, 2 );
		SetWindowPos( hControl, NULL, iPositions[iButtonRow][iButtonCount], rect.top, 0, 0,
			SWP_NOZORDER | SWP_NOACTIVATE );
		ShowWindow( hControl, SW_SHOW );
		iButtonCount++;
	}

	UpdateWindow( hWnd );
}

//***********************************************************************
void CJungleScene::DisplayComboControls( HWND hWnd, LPSHOT lpShot )
//***********************************************************************
{
	BOOL bShow;
	static BOOL bOnScreen;

	if ( !lpShot )
		bShow = NO;
	else
	{
		bShow = ((lpShot->lFlags & A_COMBINATION) != 0);
		if ( bShow == bOnScreen )
			return;
	}

	bOnScreen = bShow;
	int iShow = bShow ? SW_SHOW : SW_HIDE;
	// Clear out the numbers
	if ( bShow )
	{
		SetWindowWord( GetDlgItem(hWnd, IDC_COMBO_DIGIT1), GWW_ICONID, IDC_COMBO1 );
		SetWindowWord( GetDlgItem(hWnd, IDC_COMBO_DIGIT2), GWW_ICONID, IDC_COMBO1 );
		SetWindowWord( GetDlgItem(hWnd, IDC_COMBO_DIGIT3), GWW_ICONID, IDC_COMBO1 );
		SendMessage( GetDlgItem(hWnd, IDC_COMBO_DIGIT1), MB_SETSTATE, 0, 0L );
		SendMessage( GetDlgItem(hWnd, IDC_COMBO_DIGIT2), MB_SETSTATE, 0, 0L );
		SendMessage( GetDlgItem(hWnd, IDC_COMBO_DIGIT3), MB_SETSTATE, 0, 0L );
	}
	// Show/hide the combination digit controls
	ShowWindow(  GetDlgItem(hWnd, IDC_COMBO_DIGIT1), iShow);
	ShowWindow(  GetDlgItem(hWnd, IDC_COMBO_DIGIT2), iShow);
	ShowWindow(  GetDlgItem(hWnd, IDC_COMBO_DIGIT3), iShow);
//	ShowWindow(  GetDlgItem(hWnd, IDC_COMBO_TRYIT),  iShow);
//	ShowWindow(  GetDlgItem(hWnd, IDC_COMBO_GIVEUP), iShow);
	UpdateWindow( hWnd );
}

//************************************************************************
void CJungleScene::OnGameSceneCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	// turn off idle handling
	m_dwIdleDelay = 0; // deactivate the idle timeout

	switch (id)
	{
		#ifdef _DEBUG
		case IDC_VIDEO_GOTO:
		{
			if ( codeNotify != EN_CHANGE )
				break;
			HWND hVideo = GetDlgItem( hWnd, IDC_VIDEO_GAME );
			if ( !hVideo )
				break;
			STRING szString;
			GetDlgItemText( hWnd, IDC_VIDEO_GOTO, szString, sizeof(STRING) );
			LPSHOT lpShot = Video_FindShotName( hVideo, szString );
			if ( !lpShot )
				break;
			LPVIDEO lpVideo = (LPVIDEO)GetWindowLong(hVideo, GWL_DATAPTR);
			if ( !lpVideo )
				break;
			Video_GotoShot( hVideo, lpVideo, lpShot->lShotID, 1/*lCount*/, 0/*iFrames*/, 0 );
			break;
		}
		#endif // _DEBUG

		case IDC_ZOOM:
		{
			HWND hVideo = GetDlgItem( hWnd, IDC_VIDEO_GAME );
			if ( !hVideo )
				break;
			LPVIDEO lpVideo = (LPVIDEO)GetWindowLong(hVideo, GWL_DATAPTR);
			if ( !lpVideo )
				break;
//			// For Thumbnail size...
//			if ( lpVideo->iZoomFactor == 1 )
//				lpVideo->iZoomFactor = -2;
//			else
			if ( lpVideo->iZoomFactor == 2 )
				lpVideo->iZoomFactor = 1;
			else
				lpVideo->iZoomFactor = 2;

			// force the control to take on the size of the video file
			RECT rect;
			GetClientRect( hVideo, &rect );
			MapWindowPoints( hVideo, GetParent(hVideo), (LPPOINT)&rect, 2 );
			SetWindowPos( hVideo, NULL, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
				SWP_NOZORDER | SWP_NOACTIVATE );
			break;
		}

		case IDC_EXIT:
		case IDC_OPTIONS:
		case IDC_SAVE:
		{
			// Remember where we were
			LPVIDEO lpVideo = (LPVIDEO)GetWindowLong( GetDlgItem(hWnd, IDC_VIDEO_GAME), GWL_DATAPTR );
			if (lpVideo)
			{
				LPSHOT lpShot = Video_GetShot( lpVideo, lpVideo->lCurrentShot );
				if ( (lpShot->lEndShot == SHOT_PREV) || (lpShot->lFlags & A_HELPER_CLIP) )
					m_lSaveShot = lpVideo->lPrevShot;
				else
					m_lSaveShot = lpVideo->lCurrentShot;
			}
			else
				m_lSaveShot = 0;

			// Save the zoom factor
			if (lpVideo)
			{
				char szBuffer[2];
				szBuffer[0] = '0' + lpVideo->iZoomFactor;
				szBuffer[1] = 0;
				WritePrivateProfileString(OPTIONS_SECTION, "ZoomFactor", szBuffer, INI_FILE);
			}

			SaveGame(hWnd, 0);
			if ( id == IDC_EXIT )
				GetApp()->GotoScene(hWnd, 2);
			else
			if ( id == IDC_OPTIONS )
			{
				m_bOptionsfromGame = TRUE;
				GetApp()->GotoScene(hWnd, 40);
			}
			else
			if ( id == IDC_SAVE )
				GetApp()->GotoScene(hWnd, 70);
			break;
		}

		case IDC_VIDEO_GAME:
		{
			static long lLastFlags;
			LPPSHOT lppNewShot = (LPPSHOT)MAKELPARAM( (UINT)hControl, (UINT)codeNotify );
			if (!lppNewShot) // must be a stop
			{
				if ( lLastFlags & A_GAME_OVER )
				{
					GetApp()->GotoScene(hWnd, 50); // play the credits
					break;
				}
				if ( lLastFlags & A_COMBINATION )
					SetIdleTimeout(IDLE_DELAY, 1);
				else
					SetIdleTimeout(IDLE_DELAY, 0);
				break;
			}

			LPSHOT lpShot = *lppNewShot;
			if (!lpShot)
				break;

			if ( lpShot->lFlags & A_IGNORE )
			{
				*lppNewShot = NULL; // reject the shot
				break;
			}

			if ( lpShot->lFlags & A_ADD_IQ )
				ChangeInstinctMeter(hWnd, GOOD_RESPONSE);
			else
			if ( lpShot->lFlags & A_ADD_IQ2 )
				ChangeInstinctMeter(hWnd, GOOD_RESPONSE*2);
			else
			if ( lpShot->lFlags & A_SUB_IQ )
				ChangeInstinctMeter(hWnd, BAD_RESPONSE);
			else
			if ( lpShot->lFlags & A_SUB_IQ2 )
				ChangeInstinctMeter(hWnd, BAD_RESPONSE*2);

			LPVIDEO lpVideo = (LPVIDEO)GetWindowLong( GetDlgItem(hWnd, IDC_VIDEO_GAME), GWL_DATAPTR );
			if ( lpVideo )
			{
				if ( lpShot->lFlags & A_TRYIT )
				{
					// Get the 3-digit combination the player dialed in
					int iDigit1 = (int)SendMessage( GetDlgItem(hWnd, IDC_COMBO_DIGIT1), MB_GETSTATE, 0, 0L);
					int iDigit2 = (int)SendMessage( GetDlgItem(hWnd, IDC_COMBO_DIGIT2), MB_GETSTATE, 0, 0L);
					int iDigit3 = (int)SendMessage( GetDlgItem(hWnd, IDC_COMBO_DIGIT3), MB_GETSTATE, 0, 0L);
		
					HWND hVideo = GetDlgItem( hWnd, IDC_VIDEO_GAME );
					if ( !hVideo )
						break;
					// If combination is 30-15-30, take path as is, otherwise redirect it
					if (iDigit1 == 30 && iDigit2 == 15 && iDigit3 == 30)
					{
						lpVideo->lTestValue[TV_GOT_KEY] = YES;
					}
					else
					{
						*lppNewShot = NULL; // reject the shot
						FORWARD_WM_KEYDOWN( hVideo, VK_DOWN, 0/*cRepeat*/, 0/*flags*/, SendMessage);
						break;
					}
				}
	
				if ( lpShot->lFlags & A_SEEN_CROWN_ROOM )
					lpVideo->lTestValue[TV_SEEN_CROWN_ROOM] = YES;
	
				lpVideo->lTestValue[TV_INSTINCT_AT_8E]  = (m_lInstinctMeter >= 10000L);
				lpVideo->lTestValue[TV_INSTINCT_AT_27B] = (m_lInstinctMeter >= 50000L);
				lpVideo->lTestValue[TV_INSTINCT_AT_82B] = (m_lInstinctMeter >= 10000L);
				lpVideo->lTestValue[TV_HIGHEST_SCORE]   = (m_lInstinctMeter >= 90000L);
				lpVideo->lTestValue[TV_HIGH_SCORE]      = (m_lInstinctMeter >= 80000L);

				#ifdef _DEBUG
				STRING szString;
				GetAtomName( lpShot->aShotID, szString, sizeof(STRING) );
				SetDlgItemText( hWnd, IDC_VIDEO_SHOTNAME, szString );
				#endif
			}

			// Put up any necessary buttons
			if ( lpShot->lFlags != lLastFlags )
			{
				ClearBackground(hWnd);
				lLastFlags = lpShot->lFlags;
			}

			DisplayComboControls(hWnd, lpShot);
			DisplayDecisionButtons(hWnd, lpShot);
			DisplayPuzzleButtons(hWnd, lpShot);
			break;
		}

		// This button is only visible for shots with the voice reco flag
		case IDC_TALK_BUTTON:
		{
			FORWARD_WM_KEYDOWN(GetDlgItem( hWnd, IDC_VIDEO_GAME ), 'S',
				0/*cRepeat*/, 0/*flags*/, SendMessage);
			if (bVoiceRecoOn)
				VoiceRecoStop(hWnd);
			else
				VoiceRecoStart(hWnd);
			break;
		}

		// Handles all voice reco. messages returning coded reponses
		case IDC_VRECO_MSG:
		{
			VoiceRecoProcess(hWnd, codeNotify);
			break;
		}

		case IDC_LEFT:
		{
			VoiceRecoStop(hWnd);
			FORWARD_WM_KEYDOWN(GetDlgItem( hWnd, IDC_VIDEO_GAME ), VK_LEFT,
				0/*cRepeat*/, 0/*flags*/, SendMessage);
			break;
		}

		case IDC_UP:
		{
			VoiceRecoStop(hWnd);
			FORWARD_WM_KEYDOWN(GetDlgItem( hWnd, IDC_VIDEO_GAME ), VK_UP,
				0/*cRepeat*/, 0/*flags*/, SendMessage);
			break;
		}

		case IDC_RIGHT:
		{
			VoiceRecoStop(hWnd);
			FORWARD_WM_KEYDOWN(GetDlgItem( hWnd, IDC_VIDEO_GAME ), VK_RIGHT,
				0/*cRepeat*/, 0/*flags*/, SendMessage);
			break;
		}

		case IDC_DOWN:
		{
			VoiceRecoStop(hWnd);
			FORWARD_WM_KEYDOWN(GetDlgItem( hWnd, IDC_VIDEO_GAME ), VK_DOWN,
				0/*cRepeat*/, 0/*flags*/, SendMessage);
			break;
		}

		case IDC_HOME:
		{
			VoiceRecoStop(hWnd);
			FORWARD_WM_KEYDOWN(GetDlgItem( hWnd, IDC_VIDEO_GAME ), VK_HOME,
				0/*cRepeat*/, 0/*flags*/, SendMessage);
			break;
		}

		// Process the puzzle buttons
		case IDC_PUZ_BTN1:
		case IDC_PUZ_BTN2:
		case IDC_PUZ_BTN3:
		case IDC_PUZ_BTN4:
		case IDC_PUZ_BTN5:
		case IDC_PUZ_BTN6:
		case IDC_PUZ_BTN7:
		case IDC_PUZ_BTN8:
		case IDC_PUZ_BTN9:
		case IDC_PUZ_BTN10:
		case IDC_PUZ_BTN11:
		case IDC_PUZ_BTN12:
		{
			VoiceRecoStop(hWnd);
			SetIdleTimeout(IDLE_DELAY, id);
			FORWARD_WM_KEYDOWN(GetDlgItem( hWnd, IDC_VIDEO_GAME ), 'S',
				0/*cRepeat*/, 0/*flags*/, SendMessage);
			HandlePuzzleButtons( hWnd, id );
			break;
		}

		// Process the combination lock buttons
		case IDC_COMBO_DIGIT1:
		case IDC_COMBO_DIGIT2:
		case IDC_COMBO_DIGIT3:
		{
			SetIdleTimeout(IDLE_DELAY, 1);
			iPuzzleDisk = 2;
			break;
		}

		default:
			break;
	}
}

//************************************************************************
void CJungleScene::ChangeInstinctMeter(HWND hWnd, int iAdjustByValue)
//************************************************************************
{
	m_lInstinctMeter += iAdjustByValue;
	if (m_lInstinctMeter > METER_MAX)
		m_lInstinctMeter = METER_MAX;
	else
	if (m_lInstinctMeter < 0)
		m_lInstinctMeter = 0;
	// Set the instinct meter value
	SendMessage( GetDlgItem(hWnd, IDC_INSTINCT_METER), SM_SETPOSITION, 0, m_lInstinctMeter );
}

/************************************************************************/
void CJungleScene::VoiceRecoStart(HWND hWnd)
/************************************************************************/
{
	if ( !bVoiceRecoInit )
		return;
	if ( GetSound() )
		GetSound()->StopAndFree();
	if ( bVoiceRecoOn = VoiceRecoOn( App.GetMainWnd() ) )
	{
		// Push the talk button in
		SendMessage( GetDlgItem(hWnd, IDC_TALK_BUTTON), SM_SETSTATE, 1, 0L);
	}
	else
	{
		VoiceRecoOff();
		// Pop the Talk button back out
		SendMessage( GetDlgItem(hWnd, IDC_TALK_BUTTON), SM_SETSTATE, 0, 0L);
	}
}

/************************************************************************/
void CJungleScene::VoiceRecoStop(HWND hWnd)
/************************************************************************/
{
	if ( !bVoiceRecoInit )
		return;
	bVoiceRecoOn = FALSE;
	VoiceRecoOff();
	// Pop the Talk button back out
	SendMessage( GetDlgItem(hWnd, IDC_TALK_BUTTON), SM_SETSTATE, 0, 0L);
}

// Table of voice reco responses
typedef struct _vrecoinfo
{
	int		iShotNum;
	UINT	uCode;
} VRECOINFO;


static VRECOINFO m_PuzzleInfo[] =
{
//	Shot	VRECO
//	Number	uCode
	 3,		6, // KA-KEE
	18,		4, // JA-TA
	22,		5, // KA-CHEE-KEE
	28,		5, // KA-CHEE-KEE
	30,		0, // AW-OW-OW
	46,		3, // JA-KEE-TA
	50,		6, // KA-KEE-TA
	52,		4, // JA-TA
	53,		1, // CHA-KEE-CHA
	55,		2, // CHEE-KA-CHEE
};

//************************************************************************
void CJungleScene::VoiceRecoProcess(HWND hWnd, UINT uCode)
//************************************************************************
{
	VoiceRecoStop(hWnd);

	LPVIDEO lpVideo = (LPVIDEO)GetWindowLong( GetDlgItem(hWnd, IDC_VIDEO_GAME), GWL_DATAPTR );
	if ( !lpVideo)
		return;
	if ( lpVideo->lCurrentShot <= 0 )
		return;
	LPSHOT lpShot = &lpVideo->lpAllShots[lpVideo->lCurrentShot - 1];
	if ( !lpShot)
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

	BOOL bCorrect = (uCode == m_PuzzleInfo[idx].uCode);
	if ( bCorrect )
		FORWARD_WM_KEYDOWN(GetDlgItem(hWnd, IDC_VIDEO_GAME), VK_DOWN, 0 /*cRepeat*/, 0 /*flags*/, SendMessage);
	else
		FORWARD_WM_KEYDOWN(GetDlgItem(hWnd, IDC_VIDEO_GAME), VK_UP, 0 /*cRepeat*/, 0 /*flags*/, SendMessage);
}

/************************************************************************/
BOOL CJungleScene::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	CScene::OnInitDialog(hWnd, hWndFocus, lParam);
	switch (m_nSceneNo)
	{
		// The main menu scene
		case 2:
		{
			GetApp()->SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_START, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_EXIT, 0L, YES/*fOnDown*/);
			GetApp()->SetJoyMessageMap( 1, hWnd, WM_COMMAND, IDC_START, 0L );
			return( TRUE );
		}
		// The play game screen
		case 20:
		{
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_EXIT, 0L, YES/*fOnDown*/);
			return(OnGameSceneInit(hWnd, hWndFocus, lParam) );
		}
		// The resume game scene
		case 30:
		{
			GetApp()->SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_CANCEL, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_CANCEL, 0L, YES/*fOnDown*/);
			return(OnResumeSceneInit(hWnd, hWndFocus, lParam) );
		}
		// The options scene
		case 40:
		{
			GetApp()->SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_RETURN, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_RETURN, 0L, YES/*fOnDown*/);
			return(OnOptionsSceneInit(hWnd, hWndFocus, lParam) );
		}
		// The credits scene
		case 50:
		{
			GetApp()->SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_RETURN, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_RETURN, 0L, YES/*fOnDown*/);
			return( TRUE );
		}
		// The quit scene
		case 60:
		{
			GetApp()->SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_YES, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_NOT, 0L, YES/*fOnDown*/);
			return( TRUE );
		}
		// The save game scene
		case 70:
		{
			GetApp()->SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_CANCEL, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_CANCEL, 0L, YES/*fOnDown*/);
			return(OnSaveGameSceneInit(hWnd, hWndFocus, lParam) );
		}
		default:
		{
			return(FALSE);
		}
	}
	return(FALSE);
}

//************************************************************************
void CJungleScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (m_nSceneNo)
	{
		// The main screen
		case 2:
		{
			switch ( id )
			{
				case IDC_START:
				{
					// Get the zoom factor
					int iZoomFactor = (int)GetPrivateProfileInt(OPTIONS_SECTION, "ZoomFactor", 0, INI_FILE);
					BOOL bExpert = ((LPJUNGLEAPP)GetApp())->GetExpert();
					Video_SetDefaults( iZoomFactor, 0/*lResumeShot*/, bExpert/*bShortHotspots*/ );
					GetApp()->GotoScene(hWnd, 20);
					break;
				}
				case IDC_RESUME:
				{
					GetApp()->GotoScene(hWnd, 30);
					break;
				}
				case IDC_CREDITS:
				{
					GetApp()->GotoScene(hWnd, 50);
					break;
				}
				case IDC_OPTIONS:
				{
					m_bOptionsfromGame = FALSE;
					GetApp()->GotoScene(hWnd, 40);
					break;
				}
				case IDC_EXIT:
				{
					GetApp()->GotoScene(hWnd, 60);
					break;
				}
			}
			break;
		}

		// The game play scene
		case 20:
		{
			OnGameSceneCommand(hWnd, id, hControl, codeNotify);
			break;
		}

		// The resume game scene
		case 30:
		{
			OnResumeSceneCommand(hWnd, id, hControl, codeNotify);
			break;
		}

		// The options scene
		case 40:
		{
			OnOptionsSceneCommand(hWnd, id, hControl, codeNotify);
			break;
		}

		// The credits scene
		case 50:
		{
			switch ( id )
			{
				case IDC_RETURN:
				{ // Return to main screen
					GetApp()->GotoScene(hWnd, 2);
					break;
				}
			}
			break;
		}

		// The quit scene
		case 60:
		{
			switch ( id )
			{
				case IDC_YES:
				{ // Exit the program
					GetApp()->GotoScene(hWnd, 0);
					break;
				}
				case IDC_NOT:
				{ // Return to main screen
					GetApp()->GotoScene(hWnd, 2);
					break;
				}
			}
			break;
		}

		// The save game scene
		case 70:
		{
			OnSaveGameSceneCommand(hWnd, id, hControl, codeNotify);
			break;
		}

		default:
		{
			break;
		}
	}
}

/***********************************************************************/
void CJungleScene::OnClose(HWND hWindow)
/***********************************************************************/
{
	CScene::OnClose(hWindow);
}

/***********************************************************************/
void CJungleScene::OnTimer(HWND hWindow, UINT id)
/***********************************************************************/
{
	CScene::OnTimer(hWindow, id);
	// piggy back off of animation timer
	if ( id != ANIMATOR_TIMER_ID )
			return;

	if (m_dwIdleDelay)
	{
		if ((timeGetTime() - m_dwIdleStartTime) >= m_dwIdleDelay)
		{
			m_dwIdleDelay = 0; // deactivate the idle timeout
			// too much idle time, the user needs some help
			HandleIdleExpired();
		}
	}
	switch( iPuzzleShown )
	{
		case 4: // Puzzle 4 - Whak-a-mole
		{
			HandlePuzzle4Timer( hWindow );
			break;
		}
	}
}

//************************************************************************
BOOL CJungleScene::OnOptionsSceneInit(HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	// Init the "VoiceReco" button
	SendMessage( GetDlgItem(hWnd, IDC_VOICE), SM_SETSTATE, bVoiceRecoInit, 0L );

	// Init the "Expert" button
	BOOL bExpert = ((LPJUNGLEAPP)GetApp())->GetExpert();
	SendMessage( GetDlgItem(hWnd, IDC_EXPERT), SM_SETSTATE, bExpert, 0L );

	StartMidi(NO, NULL);
	StartWave(NO, NULL);

	// Init. the midi and wave volume slider positions
	// Volume range of each channel -> 0x0000 to 0xffff, right channel volume is
	// in the high word, left channel is in the low word, if high word is zero,
	// the device is mono, so we'll just use the left value and assume they're the same
	// Midi volume
	DWORD dwVol = MCIGetVolume((LPSTR)MCI_DEVTYPE_SEQUENCER);
	WORD wLeft = LOWORD(dwVol);
	long lPosition = (100L * wLeft) / 65535L;
	SendDlgItemMessage( hWnd, IDC_SLIDER_MIDIVOL, SM_SETPOSITION, 0, lPosition );

	// Wave volume
	dwVol = MCIGetVolume((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO);
	wLeft = LOWORD(dwVol);
	lPosition = (100L * wLeft) / 65535L;
	SendDlgItemMessage( hWnd, IDC_SLIDER_WAVVOL, SM_SETPOSITION, 0, lPosition );

	return(TRUE);
}

// Options scene command handler
//************************************************************************
void CJungleScene::OnOptionsSceneCommand( HWND hWnd, int id, HWND loWord, UINT hiWord )
//************************************************************************
{
	LPSCENE lpScene = NULL;
	LPOFFSCREEN lpOffScreen = NULL;

	switch (id)
	{
		case IDC_RETURN:
		{
			char szBuffer[2];

			// make sure midi and wave are stopped
			StartMidi(NO, NULL);
			StartWave(NO, NULL);

			// Save the expert setting
			szBuffer[0] = '0' + ((LPJUNGLEAPP)GetApp())->GetExpert();
			szBuffer[1] = 0;
			WritePrivateProfileString(OPTIONS_SECTION, "Expert", szBuffer, INI_FILE);

			// Check on voice reco
			BOOL bUseVoiceReco = GetWindowWord( GetDlgItem(hWnd, IDC_VOICE), GWW_STATE );
			if ( bUseVoiceReco && !bVoiceRecoInit )
			{ // If we should use voice reco and its not on, init it
				bVoiceRecoInit = VoiceRecoInit(App.GetMainWnd());
			}
			else
			if ( !bUseVoiceReco && bVoiceRecoInit )
			{ // If we should NOT use voice reco and it IS on, close it
				VoiceRecoClose();
				bVoiceRecoInit = NO;
			}

			// Save the voice reco setting
			szBuffer[0] = '0' + bVoiceRecoInit;
			szBuffer[1] = 0;
			WritePrivateProfileString(OPTIONS_SECTION, "VoiceReco", szBuffer, INI_FILE);

			if (m_bOptionsfromGame)
				ResumeGame( hWnd, 0 );
			else
				GetApp()->GotoScene(hWnd, 2); // Return to main screen
			break;
		}

		case IDC_SLIDER_MIDIVOL:
		{
			// Start and stop the looping sound
			StartMidi( !loWord, TEST_MIDI ); // loWord is 1, on slider up
			WORD wVolume = (WORD)(( 65535L * (DWORD)hiWord ) / 100L);
			MCISetVolume((LPSTR)MCI_DEVTYPE_SEQUENCER, wVolume, wVolume);
			break;
		}
      
		case IDC_SLIDER_WAVVOL:
		{
			// Start and stop the looping sound
			StartWave( !loWord, TEST_WAVE ); // loWord is 1, on slider up
			WORD wVolume = (WORD)(( 65535L * (DWORD)hiWord ) / 100L);
			MCISetVolume((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO, wVolume, wVolume);
			break;
		}
      
		case IDC_VOICE:
		{
			BOOL bUseVoiceReco = !GetWindowWord( GetDlgItem(hWnd, IDC_VOICE), GWW_STATE );
			SendMessage( GetDlgItem(hWnd, IDC_VOICE), SM_SETSTATE, bUseVoiceReco, 0L );
			break;
		}
	  
		case IDC_EXPERT:
		{
			BOOL bExpert = !GetWindowWord( GetDlgItem(hWnd, IDC_EXPERT), GWW_STATE );
			SendMessage( GetDlgItem(hWnd, IDC_EXPERT), SM_SETSTATE, bExpert, 0L );
			((LPJUNGLEAPP)GetApp())->SetExpert( bExpert );
			break;
		}
	  
		default:
			break;
	}
}

//************************************************************************
BOOL CJungleScene::OnResumeSceneInit(HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	STRING szTitle;

	// Get current titles from the INI file
	GetPrivateProfileString(OPTIONS_SECTION, "Game1Title", "", szTitle, sizeof(STRING), INI_FILE);
	SetDlgItemText(hWnd, IDC_GAME1_NAME, szTitle);

	GetPrivateProfileString(OPTIONS_SECTION, "Game2Title", "", szTitle, sizeof(STRING), INI_FILE);
	SetDlgItemText(hWnd, IDC_GAME2_NAME, szTitle);

	GetPrivateProfileString(OPTIONS_SECTION, "Game3Title", "", szTitle, sizeof(STRING), INI_FILE);
	SetDlgItemText(hWnd, IDC_GAME3_NAME, szTitle);

	GetPrivateProfileString(OPTIONS_SECTION, "Game4Title", "", szTitle, sizeof(STRING), INI_FILE);
	SetDlgItemText(hWnd, IDC_GAME4_NAME, szTitle);

	GetPrivateProfileString(OPTIONS_SECTION, "Game5Title", "", szTitle, sizeof(STRING), INI_FILE);
	SetDlgItemText(hWnd, IDC_GAME5_NAME, szTitle);

	return(TRUE);
}

//************************************************************************
void CJungleScene::OnResumeSceneCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_GAME0:
		case IDC_GAME1:
		case IDC_GAME2:
		case IDC_GAME3:
		case IDC_GAME4:
		case IDC_GAME5:
		{
			int iGame = (id - IDC_GAME0)/2;
			ResumeGame( hWnd, iGame );
			break;
		}

		case IDC_CANCEL:
		{
			// Return to main screen
			GetApp()->GotoScene(hWnd, 2);
			break;
		}

		default:
			break;
	}
}

//***********************************************************************
void CJungleScene::SaveGameTitles( HWND hWnd, int iGameNumber )
//***********************************************************************
{
	STRING szTitle;

	if ( iGameNumber < 1 || iGameNumber > 5 )
		return;

	GetDlgItemText(hWnd, IDC_GAME1_NAME + iGameNumber - 1, szTitle, sizeof(STRING));
	STRING szUnused;
	wsprintf( szUnused, "Unused Entry #%d", iGameNumber );
	if ( !lstrcmpi( szTitle, szUnused ) )
		wsprintf( szTitle, "My Saved Game #%d", iGameNumber );
	STRING szValueName;
	wsprintf( szValueName, "Game%dTitle", iGameNumber );
	WritePrivateProfileString(OPTIONS_SECTION, szValueName, szTitle, INI_FILE);
}

//***********************************************************************
void CJungleScene::SaveGame( HWND hWnd, int iGameNumber )
//***********************************************************************
{
	char szKey[32];

	// Save the shot number
	wsprintf(szKey, "Game%dShot", iGameNumber);
	WritePrivateProfileLong(OPTIONS_SECTION, szKey, m_lSaveShot, INI_FILE);

	// Save the instinct meter value
	wsprintf(szKey, "Game%dInstinct", iGameNumber);
	WritePrivateProfileLong(OPTIONS_SECTION, szKey, m_lInstinctMeter, INI_FILE);
}

//***********************************************************************
void CJungleScene::ResumeGame( HWND hWnd, int iGameNumber )
//***********************************************************************
{
	char szBuffer[64];

	// Get the saved instinct meter value
	wsprintf(szBuffer, "Game%dInstinct", iGameNumber);
	m_lResumeInstinctMeter = GetPrivateProfileLong(OPTIONS_SECTION, szBuffer, DEFAULT_INSTINCT, INI_FILE);

	// Get the saved shot number
	wsprintf(szBuffer, "Game%dShot", iGameNumber);
	SHOTID lResumeShot = GetPrivateProfileLong(OPTIONS_SECTION, szBuffer, 0, INI_FILE);

	// Get the default zoom factor
	int iZoomFactor = (int)GetPrivateProfileInt(OPTIONS_SECTION, "ZoomFactor", 0, INI_FILE);
	BOOL bExpert = ((LPJUNGLEAPP)GetApp())->GetExpert();
	Video_SetDefaults( iZoomFactor, lResumeShot, bExpert/*bShortHotspots*/ );

	// Resume the game
	GetApp()->GotoScene(hWnd, 20);
}

//************************************************************************
BOOL CJungleScene::OnSaveGameSceneInit(HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	STRING szTitle;

	// Get current titles from the INI file
	GetPrivateProfileString(OPTIONS_SECTION, "Game1Title", "", szTitle, sizeof(STRING), INI_FILE);
	if ( !szTitle[0] )
		lstrcpy( szTitle, "Unused Entry #1" );
	SetDlgItemText(hWnd, IDC_GAME1_NAME, szTitle);

	GetPrivateProfileString(OPTIONS_SECTION, "Game2Title", "", szTitle, sizeof(STRING), INI_FILE);
	if ( !szTitle[0] )
		lstrcpy( szTitle, "Unused Entry #2" );
	SetDlgItemText(hWnd, IDC_GAME2_NAME, szTitle);

	GetPrivateProfileString(OPTIONS_SECTION, "Game3Title", "", szTitle, sizeof(STRING), INI_FILE);
	if ( !szTitle[0] )
		lstrcpy( szTitle, "Unused Entry #3" );
	SetDlgItemText(hWnd, IDC_GAME3_NAME, szTitle);

	GetPrivateProfileString(OPTIONS_SECTION, "Game4Title", "", szTitle, sizeof(STRING), INI_FILE);
	if ( !szTitle[0] )
		lstrcpy( szTitle, "Unused Entry #4" );
	SetDlgItemText(hWnd, IDC_GAME4_NAME, szTitle);

	GetPrivateProfileString(OPTIONS_SECTION, "Game5Title", "", szTitle, sizeof(STRING), INI_FILE);
	if ( !szTitle[0] )
		lstrcpy( szTitle, "Unused Entry #5" );
	SetDlgItemText(hWnd, IDC_GAME5_NAME, szTitle);

	return(TRUE);
}

//************************************************************************
void CJungleScene::OnSaveGameSceneCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_GAME1_NAME:
		case IDC_GAME2_NAME:
		case IDC_GAME3_NAME:
		case IDC_GAME4_NAME:
		case IDC_GAME5_NAME:
		{
			// this doesn't appear to work as described
			//if ( codeNotify == EN_CHANGE )
			//	SendMessage( hWnd, DM_SETDEFID, IDC_GAME1 + (id - IDC_GAME1_NAME), 0L );
			break;
		}

		case IDC_GAME0:
		case IDC_GAME1:
		case IDC_GAME2:
		case IDC_GAME3:
		case IDC_GAME4:
		case IDC_GAME5:
		{
			int iGame = (id - IDC_GAME0)/2;
			SaveGame( hWnd, iGame );
			SaveGameTitles( hWnd, iGame );
			ResumeGame( hWnd, 0 );
			break;
		}

		case IDC_CANCEL:
		{
			HWND hFocus = GetFocus();
			if ( hControl == hFocus )
			{
				ResumeGame( hWnd, 0 );
				break;
			}

			// Enter key was pressed
			ITEMID id1 = NULL;
			if ( hFocus == GetDlgItem( hWnd, IDC_GAME1_NAME ) ) id1 = IDC_GAME1; else
			if ( hFocus == GetDlgItem( hWnd, IDC_GAME2_NAME ) ) id1 = IDC_GAME2; else
			if ( hFocus == GetDlgItem( hWnd, IDC_GAME3_NAME ) ) id1 = IDC_GAME3; else
			if ( hFocus == GetDlgItem( hWnd, IDC_GAME4_NAME ) ) id1 = IDC_GAME4; else
			if ( hFocus == GetDlgItem( hWnd, IDC_GAME5_NAME ) ) id1 = IDC_GAME5;
			if ( id1 )
			{
				SendMessage( GetDlgItem(hWnd, id1), SM_SETSTATE, 1, 0L);
				int iGame = (id1 - IDC_GAME0)/2;
				SaveGame( hWnd, iGame );
				SaveGameTitles( hWnd, iGame );
				ResumeGame( hWnd, 0 );
			}
			break;
		}

		default:
			break;
	}
}

//************************************************************************
void CJungleScene::SetIdleTimeout(DWORD dwIdleDelay, ITEMID idIdleItem)
//************************************************************************
{
	m_dwIdleStartTime = timeGetTime();
	m_dwIdleDelay = dwIdleDelay;
	if ( idIdleItem )
		m_idIdleItem = idIdleItem;
}

//************************************************************************
void CJungleScene::GotoHelperShot( HWND hWnd, LPSTR lpString )
//************************************************************************
{
	VoiceRecoStop( hWnd );
	if ( !lpString )
		return;
	HWND hVideo = GetDlgItem( hWnd, IDC_VIDEO_GAME );
	if ( !hVideo )
		return;
	LPVIDEO lpVideo = (LPVIDEO)GetWindowLong( hVideo, GWL_DATAPTR );
	if ( !lpVideo )
		return;
	LPSHOT lpCurrentShot = Video_FindShot( hVideo, lpVideo->lCurrentShot, 1/*lCount*/ );
	if ( !lpCurrentShot )
		return;

	// If helper clips are exhasuted, supercede the shot name with the "No more time" shot
	STRING szNoTime;
	if ( lpCurrentShot->lFlags & A_HELPER_CLIP_DONE )
	{

		wsprintf( szNoTime, "/Hlp%dNoTime", lpVideo->wDisk );
		lpString = szNoTime;
	}

	LPSHOT lpShot = Video_FindShotName( hVideo, lpString );
	if ( !lpShot )
		return;

	// Clone the current shot to inherit his/her behavior
	lpShot->lEndShot		 = lpCurrentShot->lUpShot;
	lpShot->lEndCount		 = lpCurrentShot->lEndCount;
	lpShot->lEndLoop		 = lpCurrentShot->lEndLoop;
	lpShot->bLeftIsSubShot	 = lpCurrentShot->bLeftIsSubShot;
	lpShot->lLeftShot		 = lpCurrentShot->lLeftShot;
	lpShot->lLeftCount		 = lpCurrentShot->lLeftCount;
	lpShot->lLeftHotspot	 = 0;
	lpShot->bUpIsSubShot	 = lpCurrentShot->bUpIsSubShot;
	lpShot->lUpShot			 = lpCurrentShot->lUpShot;
	lpShot->lUpCount		 = lpCurrentShot->lUpCount;
	lpShot->lUpHotspot		 = 0;
	lpShot->bRightIsSubShot	 = lpCurrentShot->bRightIsSubShot;
	lpShot->lRightShot		 = lpCurrentShot->lRightShot;
	lpShot->lRightCount		 = lpCurrentShot->lRightCount;
	lpShot->lRightHotspot	 = 0;
	lpShot->bDownIsSubShot	 = lpCurrentShot->bDownIsSubShot;
	lpShot->lDownShot		 = lpCurrentShot->lDownShot;
	lpShot->lDownCount		 = lpCurrentShot->lDownCount;
	lpShot->lDownHotspot	 = 0;
	lpShot->bHomeIsSubShot	 = lpCurrentShot->bHomeIsSubShot;
	lpShot->lHomeShot		 = lpCurrentShot->lHomeShot;
	lpShot->lHomeCount		 = lpCurrentShot->lHomeCount;
	lpShot->lHomeHotspot	 = 0;
	lpShot->lFlags			 = lpCurrentShot->lFlags | A_HELPER_CLIP;
	Video_GotoShot( hVideo, lpVideo, lpShot->lShotID, 1/*lCount*/, 0/*iFrames*/, 0 );
}

//************************************************************************
void CJungleScene::HandleIdleExpired()
//************************************************************************
{
	switch (m_idIdleItem)
	{
		STRING szShotName;

		// Idle expired after video STOP
		case 0:
		{
			wsprintf( szShotName, "/Hlp%dIdle", iPuzzleDisk );
			GotoHelperShot( GetWindow(), szShotName );
			break;
		}

		// Idle expired after video safe combination sequence
		case 1:
		{
			GotoHelperShot( GetWindow(), "/Hlp2Safe" );
			break;
		}

		// Idle expired after puzzle button hit
		case IDC_PUZ_BTN1:
		case IDC_PUZ_BTN2:
		case IDC_PUZ_BTN3:
		case IDC_PUZ_BTN4:
		case IDC_PUZ_BTN5:
		case IDC_PUZ_BTN6:
		case IDC_PUZ_BTN7:
		case IDC_PUZ_BTN8:
		case IDC_PUZ_BTN9:
		case IDC_PUZ_BTN10:
		case IDC_PUZ_BTN11:
		case IDC_PUZ_BTN12:
		{
			m_idIdleItem = 0; // reset to the generic idle message
			switch( iPuzzleShown )
			{
				case 2: // Puzzle 2 - Blocker - Seventh Guest
				{
					wsprintf( szShotName, "/Hlp%dBlokr", iPuzzleDisk );
					GotoHelperShot( GetWindow(), szShotName );
					break;
				}
				case 3: // Puzzle 3 - Matching game
				{
					wsprintf( szShotName, "/Hlp%dMatch", iPuzzleDisk );
					GotoHelperShot( GetWindow(), szShotName );
					break;
				}
				case 4: // Puzzle 4 - Whak-a-mole
				{
					wsprintf( szShotName, "/Hlp%dWhak", iPuzzleDisk );
					GotoHelperShot( GetWindow(), szShotName );
					break;
				}
				case 5: // Puzzle 5 - Simon
				{
					wsprintf( szShotName, "/Hlp%dSimon", iPuzzleDisk );
					GotoHelperShot( GetWindow(), szShotName );
					break;
				}
			}
			break;
		}

		// Idle expired after talk button hit
		case IDC_TALK_BUTTON:
		{
			m_idIdleItem = 0; // reset to the generic idle message
			wsprintf( szShotName, "/Hlp%dTalk", iPuzzleDisk );
			GotoHelperShot( GetWindow(), szShotName );
			break;
		}
		default:
			break;
	}
}

//************************************************************************
void CJungleScene::PlaySound(LPCTSTR lpSoundName, int nIndex, BOOL bWait)
//************************************************************************
{
	STRING szResource;

	wsprintf(szResource, "%s%d", lpSoundName, nIndex);
	if (GetSound())
	 	GetSound()->StartResource( szResource, NO/*bLoop*/, -1/*iChannel*/, GetApp()->GetInstance(), bWait, NULL/*hWndNotify*/ );
}

//************************************************************************
void CJungleScene::PlayWrongSound()
//************************************************************************
{
	PlaySound( WRONG_SOUND_NAME, m_iWrongSound, NO );
	++m_iWrongSound;
	if (m_iWrongSound >= NUM_WRONG_SOUNDS)
		m_iWrongSound = 0;
}

//************************************************************************
void CJungleScene::PlayRightSound()
//************************************************************************
{
	PlaySound( RIGHT_SOUND_NAME, m_iRightSound, NO );
	++m_iRightSound;
	if (m_iRightSound >= NUM_RIGHT_SOUNDS)
		m_iRightSound = 0;
}

//************************************************************************
void CJungleScene::PlaySuccessSound()
//************************************************************************
{
	// Play the sound and wait till its done (to avoid video w/o audio)
	PlaySound( SUCCESS_SOUND_NAME, m_iSuccessSound, YES );
	++m_iSuccessSound;
	if (m_iSuccessSound >= NUM_SUCCESS_SOUNDS)
		m_iSuccessSound = 0;
}

//***********************************************************************
BOOL CJungleApp::GotoScene(HWND hWndPreviousScene, int iScene)
//***********************************************************************
{
	if ( !iScene )
	{
		PostMessage( GetMainWnd(), WM_CLOSE, 0, 0L );
		return( TRUE );
	}

	HourGlass( YES );

	ResetKeyMap();

	if ( !hWndPreviousScene )
	{
		LPSCENE lpCurrentScene = GetCurrentScene();
		if ( lpCurrentScene )
			hWndPreviousScene = lpCurrentScene->GetWindow();
	}
	if ( hWndPreviousScene )
		SendMessage( hWndPreviousScene, WM_CLOSE, 0, 0L );
		  
	LPSCENE lpScene;
	if ( iScene == 50 )
		lpScene = (LPSCENE)new CCreditsScene(2);
	else
		lpScene = (LPSCENE)new CJungleScene();

	SetCurrentScene( lpScene );

	if (!lpScene)
	{	
		HourGlass( NO );
		return(FALSE);
	}

	if (!lpScene->Create(YES, App.GetInstance(), App.GetMainWnd(), iScene))
	{
		HourGlass( NO );
		return(FALSE);
	}

	HourGlass( NO );
	return( TRUE );
}

//***********************************************************************
BOOL CJungleApp::InitInstance()
//***********************************************************************
{
	// Call base class initialization
	CPHApp::InitInstance();

	// Init the "Expert" setting from the .INI file
	m_bExpert = (BOOL)GetPrivateProfileInt(OPTIONS_SECTION, "Expert", YES, INI_FILE);

	// Get the voice reco setting from the .INI file
	if ( GetPrivateProfileInt(OPTIONS_SECTION, "VoiceReco", YES, INI_FILE) )
	{ // Get the ICSS system up and running
		if ( !(bVoiceRecoInit = VoiceRecoInit(m_hMainWnd)) )
		{ // If error in initialization, don't try to use it
		}
	}

	return TRUE;
}

//***********************************************************************
void CJungleApp::OnClose( HWND hWnd )
//***********************************************************************
{
	LPSCENE lpScene = GetCurrentScene();
	if ( lpScene )
	{
		if ( lpScene->GetSceneNo() != 60 )
		{
			GetApp()->GotoScene( NULL, 60 ); // go to the quit scene
			return;
		}
	}

	VoiceRecoClose();

	// Call base class function
	CPHApp::OnClose( hWnd );
}
