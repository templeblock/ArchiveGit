#include <windows.h>
#define MAIN
#include "proto.h"
#include "control.h"
#include "commonid.h"
#include "cllsn.h"
#include "cllsndef.h"
#include "options.h"
#include "credits.h"
#include "rcontrol.p"
#include "mmiojunk.p"

#ifdef _DEBUG
	#include "author.p"		// Video shot authoring
	#include "wdebug.p"		// Weapons authoring and debugging
#endif

// Local function prototypes

// The resume scene
void OnResumeSceneCommand( HWND hWnd, int id, HWND hControl, UINT codeNotify );
BOOL OnResumeSceneInit( HWND hWnd, HWND hWndControl, LPARAM lParam );
void ResumeGame( HWND hWnd, int iGameNumber );
void SaveGame( HWND hWnd, int iGameNumber );
void SaveGameTitles( HWND hWnd );

// The save game scene
BOOL OnSaveGameSceneInit( HWND hWnd, HWND hWndControl, LPARAM lParam );
void OnSaveGameSceneCommand( HWND hWnd, int id, HWND hControl, UINT codeNotify );

//************************************************************************
BOOL PlaySceneInit( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{ // Entering the game's main play scene
	if ( App.pGame )
		delete App.pGame;
	App.pGame = new CGame(hWnd);
	if ( !App.pGame )
		return( NO );
	return( YES );
}

//************************************************************************
void PlaySceneCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{ // Commands from the game's main play scene
	if (App.pGame)
		App.pGame->OnCommand(hWnd, id, hControl, codeNotify);
}

//************************************************************************
void PlaySceneClose( HWND hWnd )
//************************************************************************
{ // Closing the game's main play scene
	if (App.pGame)
		delete App.pGame;
	App.pGame = NULL;
}

//************************************************************************
BOOL TestSceneInit( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{ // Entering the game's test scene

	if ( App.pGame )
		delete App.pGame;
	App.pGame = new CGame(hWnd);
	if ( !App.pGame )
		return( NO );

	// Initialize weapons system
	App.pGame->Weapons.SetCurWeapon(3); // Missiles
	SendMessage( hWnd, WM_COMMAND, '1' + 3, 0L );

	/////// DO NOT REMOVE THIS CODE. IT IS ESSENTIAL FOR AUTHORING THE
	/////// ENEMY SHOTS.
	App.pGame->Weapons.CurWeapon()->OnVideoOpen();
	VideoClearObserver(App.pGame->Weapons.CurWeapon());
	App.pGame->Weapons.CurWeapon()->Enable(TRUE);
	App.pGame->Weapons.SetAmmo(3, 1000);
	VideoSetObserver(App.pGame->Weapons.CurWeapon());
	///////

	return( YES );
}

//************************************************************************
void TestSceneCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{ // Commands from the game's main play scene
	if (!App.pGame)
		return;

	UINT loWord = (UINT)hControl;
	UINT hiWord = codeNotify;

	switch (id)
	{
	case IDC_SHOTSDEBUG:
	{
		#ifdef _DEBUG
			App.pGame->lpVideo = (LPVIDEO)GetWindowLong( App.pGame->hVideo, GWL_DATAPTR );
			AuthorStart(GetParent(hWnd), App.pGame->lpVideo);
		#endif
		break;
	}

	case IDC_WEAPONDEBUG:
	{
		#ifdef _DEBUG
			App.pGame->Weapons.SetCurWeapon(3); // Missiles
			App.pGame->lpVideo = (LPVIDEO)GetWindowLong( App.pGame->hVideo, GWL_DATAPTR );
			WeaponDebugStart(GetParent(hWnd), App.pGame->Weapons.CurWeapon(), App.pGame->lpVideo);
		#endif
		break;
	}

    case IDC_EXIT:
    {
		#ifdef _DEBUG
			AuthorEnd();
			WeaponDebugEnd();
		#endif

		App.GotoScene (hWnd, 2);
		break;
    }

    case IDC_CLOSE:
    {
		//FORWARD_WM_COMMAND( hWnd, IDC_EXIT, hWnd, 0, SendMessage );
		App.GotoScene( hWnd, 2/*iScene*/ );
		break;
    }
    
		default:
			break;
	}
}


/************************************************************************/
BOOL CCollisionScene::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{ // Parcel out init's to each scene that needs it
	CScene::OnInitDialog(hWnd, hWndFocus, lParam);
	switch (m_nSceneNo)
	{
		// The logo scene
		case 1:
		{
			#ifdef _DEBUG
				//// FOR DEVELOPMENT (EASIER TO SEE DEBUG OUTPUT)
				int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
				int h = 480;
				h += (2*GetSystemMetrics(SM_CYSIZEFRAME));
				h += GetSystemMetrics(SM_CYCAPTION);
				int w = 640;
				w += (2*GetSystemMetrics(SM_CXSIZEFRAME));
				int x=0;
				if (nCxScreen > w)
					x = (nCxScreen - w) / 2;
				// Set collision window into upper left corner
				MoveWindow(GetParent(hWnd), x, 0, w, h, TRUE);
				MoveWindow(hWnd, 0, 0, 640, 480, TRUE);
				//// END SIZE CODE
			#endif

			HWND hVideo = GetDlgItem( hWnd, IDC_VIDEO );
			App.SetKeyMapEntry( hVideo, VK_ESCAPE, WM_COMMAND, IDOK, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hVideo, VK_SPACE, WM_COMMAND, IDOK, 0L, YES/*fOnDown*/);
			App.SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDOK, 0L );
			App.SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDOK, 0L );
			App.SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDOK, 0L );
			App.SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDOK, 0L );
			App.SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDOK, 0L );
			App.SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDOK, 0L );
			App.SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDOK, 0L );
			App.SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDOK, 0L );
			Radar_Register( App.GetInstance() );
			return( TRUE );
		}

		// The main menu scene
		case 2:
		{
			App.SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_START, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_QUIT, 0L, YES/*fOnDown*/);
			App.SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDC_START, 0L );
			App.SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDC_RESUME, 0L );
			App.SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDC_QUIT, 0L );
			App.SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDC_OPTIONS, 0L );
			App.SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDC_START, 0L );
			App.SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDC_RESUME, 0L );
			App.SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDC_OPTIONS, 0L );
			App.SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDC_QUIT, 0L );
			return( TRUE );
		}

		// The play game screen
		case 20:
		{
			PlaySceneInit( hWnd, hWndFocus, lParam );
			if (!App.pGame)
				return( TRUE );

			App.SetKeyMapEntry( App.pGame->hVideo, VK_PAUSE, NULL/*WM_KEYDOWN*/, VK_PAUSE, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( App.pGame->hVideo, VK_LEFT,  NULL/*WM_KEYDOWN*/, VK_LEFT,  0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( App.pGame->hVideo, VK_RIGHT, NULL/*WM_KEYDOWN*/, VK_RIGHT, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( App.pGame->hVideo, VK_UP,    NULL/*WM_KEYDOWN*/, VK_UP,    0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( App.pGame->hVideo, VK_DOWN,  NULL/*WM_KEYDOWN*/, VK_DOWN,  0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( App.pGame->hVideo, VK_HOME,  NULL/*WM_KEYDOWN*/, VK_HOME,  0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( App.pGame->hVideo, VK_SPACE, NULL/*WM_KEYDOWN*/, VK_SPACE, 0L, YES/*fOnDown*/);

			App.SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_EXIT, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, 'F', WM_COMMAND, IDC_WEAPONFIRE, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, 'W', WM_COMMAND, 'W', 0L, YES/*fOnDown*/);	// Weapon grid on
			App.SetKeyMapEntry( hWnd, 'Q', WM_COMMAND, 'Q', 0L, YES/*fOnDown*/);	// Energy Cheat
			App.SetKeyMapEntry( hWnd, VK_BACK, WM_COMMAND, VK_BACK, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, 'E', WM_COMMAND, 'E', 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, 'R', WM_COMMAND, 'R', 0L, YES/*fOnDown*/);

			// Temporary test (mc)
			App.SetKeyMapEntry( hWnd, 'L', WM_COMMAND, 'L', 0L, YES);
			App.SetKeyMapEntry( hWnd, 'S', WM_COMMAND, 'S', 0L, YES);
			App.SetKeyMapEntry( hWnd, 'C', WM_COMMAND, 'C', 0L, YES);
			App.SetKeyMapEntry( hWnd, 'G', WM_COMMAND, 'G', 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, 'D', WM_COMMAND, 'D', 0L, YES/*fOnDown*/);

			App.SetKeyMapEntry( hWnd, VK_TAB, WM_COMMAND, VK_TAB, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_F1, WM_COMMAND, VK_F1, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_F2, WM_COMMAND, VK_F2, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_F3, WM_COMMAND, VK_F3, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_F4, WM_COMMAND, VK_F4, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_F5, WM_COMMAND, VK_F5, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_F6, WM_COMMAND, VK_F6, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_F7, WM_COMMAND, VK_F7, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_F8, WM_COMMAND, VK_F8, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_F9, WM_COMMAND, VK_F9, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_F10,WM_COMMAND, VK_F10, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_F11,WM_COMMAND, VK_F11, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_F12,WM_COMMAND, VK_F12, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, 'T',   WM_COMMAND, 'T', 0L, YES/*fOnDown*/);
			//debug (mc)
			App.SetKeyMapEntry( hWnd, 'B', WM_COMMAND, 'B', 0L, YES/*fOnDown*/);

			App.SetJoyMapEntry( 1,        hWnd, WM_COMMAND, IDC_WEAPONFIRE, 0L );
			App.SetJoyMapEntry( 2,        hWnd, WM_COMMAND, VK_TAB, 0L );
			App.SetJoyMapEntry( 3,        App.pGame->hVideo, WM_KEYDOWN, VK_PAUSE, 0L );
			App.SetJoyMapEntry( 4,        App.pGame->hVideo, WM_KEYDOWN, VK_SPACE, 0L );
			App.SetJoyMapEntry( VK_LEFT,  App.pGame->hVideo, WM_KEYDOWN, VK_LEFT, 0L );
			App.SetJoyMapEntry( VK_UP,    App.pGame->hVideo, WM_KEYDOWN, VK_UP, 0L );
			App.SetJoyMapEntry( VK_RIGHT, App.pGame->hVideo, WM_KEYDOWN, VK_RIGHT, 0L );
			App.SetJoyMapEntry( VK_DOWN,  App.pGame->hVideo, WM_KEYDOWN, VK_DOWN, 0L );

			App.SetKeyMapEntry( hWnd, '1', WM_COMMAND, '1', 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, '2', WM_COMMAND, '2', 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, '3', WM_COMMAND, '3', 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, '4', WM_COMMAND, '4', 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, '5', WM_COMMAND, '5', 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, '6', WM_COMMAND, '6', 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, '7', WM_COMMAND, '7', 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, '8', WM_COMMAND, '8', 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, '9', WM_COMMAND, '9', 0L, YES/*fOnDown*/);

			App.SetKeyMapEntry( hWnd, VK_CONTROL, WM_COMMAND, 'F', 0L, YES );
			return( TRUE );
		}

		// The test screen
		case 25:
		{
			TestSceneInit( hWnd, hWndFocus, lParam );
			if (!App.pGame)
				return( TRUE );

			App.SetKeyMapEntry( App.pGame->hVideo, VK_PAUSE, NULL/*WM_KEYDOWN*/, VK_PAUSE, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( App.pGame->hVideo, VK_NEXT,  NULL/*WM_KEYDOWN*/, VK_SPACE, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( App.pGame->hVideo, VK_PRIOR, NULL/*WM_KEYDOWN*/, VK_SPACE, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_EXIT, 0L, YES/*fOnDown*/);
			return( TRUE );
		}

		// The resume game scene
		case 30:
		{
			App.SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_CANCEL, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_CANCEL, 0L, YES/*fOnDown*/);
			App.SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			return( OnResumeSceneInit( hWnd, hWndFocus, lParam ) );
		}

		// The options scene
		case 40:
		{
			App.SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_RETURN, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_RETURN, 0L, YES/*fOnDown*/);
			App.SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDC_RETURN, 0L );
			return( OptionsSceneInit( hWnd, hWndFocus, lParam ) );
		}

		// The credits scene
		case 50:
		{
			App.SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_RETURN, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_RETURN, 0L, YES/*fOnDown*/);
			App.SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			App.SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDC_RETURN, 0L );
			return( TRUE );
		}

		// The quit scene
		case 60:
		{
			App.SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_YES, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_NOT, 0L, YES/*fOnDown*/);
			App.SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDC_YES, 0L );
			App.SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDC_YES, 0L );
			App.SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDC_NOT, 0L );
			App.SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDC_NOT, 0L );
			App.SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDC_YES, 0L );
			App.SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDC_YES, 0L );
			App.SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDC_NOT, 0L );
			App.SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDC_NOT, 0L );
			return( TRUE );
		}

		// The save game scene
		case 70:
		{
			App.SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_CANCEL, 0L, YES/*fOnDown*/);
			App.SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_CANCEL, 0L, YES/*fOnDown*/);
			App.SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			App.SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			return( OnSaveGameSceneInit( hWnd, hWndFocus, lParam ) );
		}

		default:
			break;;
	}
	return( FALSE );
}

//************************************************************************
void CCollisionScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{ // Parcel out commands to each scene
	UINT loWord = (UINT)hControl;
	UINT hiWord = codeNotify;

	switch (m_nSceneNo)
	{
		// The Intro screen
		case 1:
		{
			App.GotoScene( hWnd, 2 );
			break;
		}

		// The main screen
		case 2:
		{
			switch ( id )
			{
				case IDC_START:
				{
					// Get the zoom factor
					int iZoomFactor = (int)GetPrivateProfileInt (OPTIONS_SECTION, "ZoomFactor", 0, INI_FILE);
					Video_SetDefaults( iZoomFactor, 0/*lResumeShot*/,0, YES/*bUseMCIDrawProc*/ );
					App.GotoScene (hWnd, 20);
					break;
				}
				case IDC_RESUME:
				{
					App.GotoScene (hWnd, 30);
					break;
				}
				case IDC_CREDITS:
				{
					App.GotoScene (hWnd, 50);
					break;
				}
				case IDC_OPTIONS:
				{
					App.bOptionsfromGame = FALSE;
					App.GotoScene (hWnd, 40);
					break;
				}
				case IDC_QUIT:
				{
					App.GotoScene (hWnd, 60);
					break;
				}
			}
			break;
		}

		// The game play scene
		case 20:
		{
			PlaySceneCommand( hWnd, id, hControl, codeNotify );
			break;
		}

		// The test scene
		case 25:
		{
			TestSceneCommand( hWnd, id, hControl, codeNotify );
			break;
		}

		// The resume game scene
		case 30:
		{
			OnResumeSceneCommand (hWnd, id, hControl, codeNotify);
			break;
		}

		// The options scene
		case 40:
		{
			OptionsSceneCommand( hWnd, id, hControl, codeNotify );
			break;
		}

		// The credits scene
		case 50:
		{
			switch ( id )
			{
				case IDC_RETURN:
				{ // Return to main screen
					App.GotoScene (hWnd, 2);
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
					App.GotoScene (hWnd, 0);
					break;
				}
				case IDC_NOT:
				{ // Return to main screen
					App.GotoScene (hWnd, 2);
					break;
				}
			}
			break;
		}

		// The save game scene
		case 70:
		{
			OnSaveGameSceneCommand (hWnd, id, hControl, codeNotify);
			break;
		}
	}
}

//***********************************************************************
void CCollisionScene::OnClose(HWND hWnd)
//***********************************************************************
{ // Parcel out the close to each scene
	switch (m_nSceneNo)
	{
		case 20: // play - test animation
		{
			PlaySceneClose( hWnd );
			break;
		}
		case 25: // play - test animation
		{
			PlaySceneClose( hWnd );
			break;
		}
	}

	CScene::OnClose( hWnd );
}

/***********************************************************************/
void CCollisionScene::OnTimer(HWND hWnd, UINT id)
/***********************************************************************/
{ // Timer went off, update opponent locations
	CScene::OnTimer( hWnd, id );

	switch (m_nSceneNo)
	{
		case 20: // play - test animation
		{
			if (App.pGame)
				App.pGame->OnTimer(hWnd, id);
		}

		case 25: // play - test animation
		{
			break;
		}
	}
}

//************************************************************************
BOOL OnResumeSceneInit (HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	// Get current titles from the INI file
	GetPrivateProfileString (OPTIONS_SECTION, "Game1Title", "", App.szGame1Title, TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME1_NAME, App.szGame1Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game2Title", "", App.szGame2Title, TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME2_NAME, App.szGame2Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game3Title", "", App.szGame3Title, TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME3_NAME, App.szGame3Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game4Title", "", App.szGame4Title, TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME4_NAME, App.szGame4Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game5Title", "", App.szGame5Title, TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME5_NAME, App.szGame5Title);

	return (TRUE);
}

//************************************************************************
void OnResumeSceneCommand (HWND hWnd, int id, HWND hControl, UINT codeNotify)
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
			App.GotoScene (hWnd, 2);
			break;
		}

		default:
			break;
	}
}

//***********************************************************************
void SaveGameTitles (HWND hWnd)
//***********************************************************************
{
	GetDlgItemText (hWnd, IDC_GAME1_NAME, App.szGame1Title, TITLE_SIZE);
	WritePrivateProfileString (OPTIONS_SECTION, "Game1Title", App.szGame1Title, INI_FILE);

	GetDlgItemText (hWnd, IDC_GAME2_NAME, App.szGame2Title, TITLE_SIZE);
	WritePrivateProfileString (OPTIONS_SECTION, "Game2Title", App.szGame2Title, INI_FILE);

	GetDlgItemText (hWnd, IDC_GAME3_NAME, App.szGame3Title, TITLE_SIZE);
	WritePrivateProfileString (OPTIONS_SECTION, "Game3Title", App.szGame3Title, INI_FILE);

	GetDlgItemText (hWnd, IDC_GAME4_NAME, App.szGame4Title, TITLE_SIZE);
	WritePrivateProfileString (OPTIONS_SECTION, "Game4Title", App.szGame4Title, INI_FILE);

	GetDlgItemText (hWnd, IDC_GAME5_NAME, App.szGame5Title, TITLE_SIZE);
	WritePrivateProfileString (OPTIONS_SECTION, "Game5Title", App.szGame5Title, INI_FILE);
}

//***********************************************************************
void SaveGame( HWND hWnd, int iGameNumber )
//***********************************************************************
{
	if (!App.pGame)
		return;

	char szKey[32];
	char szValue[32];

	// Save the shot number
	wsprintf (szKey, "Game%dShot", iGameNumber);
	wsprintf (szValue, "%ld", App.pGame->lSaveShot);
	WritePrivateProfileString (OPTIONS_SECTION, szKey, szValue, INI_FILE);

	// Here we need to save the level number information
	// and the collectibles that we have obtained.  On restore
	// the player will be placed in the beginning of that level but with the
	// collectible already gained.
	int iLevel = App.pGame->lpWorld->GetLevel();
	// Save the levell here

	// Save all of the collectibles
	App.pGame->Collect.SaveAll();

	// Save the other values here...
}

//***********************************************************************
void ResumeGame( HWND hWnd, int iGameNumber )
//***********************************************************************
{
	char szBuffer[64];

	// Restore other values here...

	// Get the saved shot number
	wsprintf (szBuffer, "Game%dShot", iGameNumber);
	SHOTID lResumeShot = (int)GetPrivateProfileInt (OPTIONS_SECTION, szBuffer, 0, INI_FILE);

	// Get the default zoom factor
	int iZoomFactor = (int)GetPrivateProfileInt (OPTIONS_SECTION, "ZoomFactor", 0, INI_FILE);
	Video_SetDefaults( iZoomFactor, 0/*lResumeShot*/,0, YES/*bUseMCIDrawProc*/ );

	// Resume the game
	App.GotoScene (hWnd, 20);
}

//************************************************************************
BOOL OnSaveGameSceneInit (HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	// Get current titles from the INI file
	GetPrivateProfileString (OPTIONS_SECTION, "Game1Title", "", App.szGame1Title,
	TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME1_NAME, App.szGame1Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game2Title", "", App.szGame2Title,
	TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME2_NAME, App.szGame2Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game3Title", "", App.szGame3Title,
	TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME3_NAME, App.szGame3Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game4Title", "", App.szGame4Title,
	TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME4_NAME, App.szGame4Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game5Title", "", App.szGame5Title,
	TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME5_NAME, App.szGame5Title);

	return (TRUE);
}

//************************************************************************
void OnSaveGameSceneCommand (HWND hWnd, int id, HWND hControl, UINT codeNotify)
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
			//  SendMessage( hWnd, DM_SETDEFID, IDC_GAME1 + (id - IDC_GAME1_NAME), 0L );
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
			SaveGame(hWnd, iGame);
			SaveGameTitles(hWnd);
			ResumeGame( hWnd, 0 );
			break;
		}

		case IDC_CANCEL:
		{
			if ( hControl != GetFocus() )
				break;
			ResumeGame( hWnd, 0 );
			break;
		}

		default:
			break;
	}
}

//***********************************************************************
CApp::CApp()
//***********************************************************************
{
	App.bOptionsfromGame	 = FALSE;
	App.bOptionTransitions	 = TRUE;	// Transitions
	App.bOptionAutoTarget	 = TRUE;	// Auto Target
	App.bOptionMusic		 = TRUE;	// Music
	App.bOptionBarricades	 = TRUE;	// Barricades
	App.bOptionTaunts		 = FALSE;	// Gatekeeper background audio
	App.bOptionFireBack		 = TRUE;	// Bullet holes (enemy firing back)
	App.bOptionScript		 = FALSE;	// Should we play a scripted list of shots?
	App.bMusicState			 = TRUE;

	// Cheats - Set to TRUE to cheat.
	#ifdef _DEBUG
	App.bEnergyCheat		 = FALSE;
	App.bShowGrid			 = FALSE;
	#else
	App.bEnergyCheat		 = FALSE;
	App.bShowGrid			 = FALSE;
	#endif

	// Any command message moves us along
	int iZoomFactor = (int)GetPrivateProfileInt (OPTIONS_SECTION, "ZoomFactor", 0, INI_FILE);
	Video_SetDefaults( iZoomFactor, 0/*lResumeShot*/,0, YES/*bUseMCIDrawProc*/ );

	MMIOJunkInstall();
	pGame = NULL;
}

//***********************************************************************
CApp::~CApp()
//***********************************************************************
{
	if (pGame)
		delete pGame;

	MMIOJunkRemove();
}


//***********************************************************************
BOOL CApp::GotoScene( HWND hWndPreviousScene, int iScene )
//***********************************************************************
{ // Standard fare for most titles
	if ( !iScene )
	{
		PostMessage( GetMainWnd(), WM_CLOSE, 0, 0L );
		return( TRUE );
	}

	HourGlass( YES );

	ResetKeyMap();
	ResetJoyMap();

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
		lpScene = (LPSCENE)new CCollisionScene();

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

