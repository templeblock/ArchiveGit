#include <windows.h>
#include "proto.h"
#include "commonid.h"
#include "cllsnid.h"
#include "cllsn.h"
#include "game.h"

#include "control.h"
#include "cllsndef.h"
#include "weapon.h"
#include "worlds.h"
#include "rcontrol.p"
#include "collect.h" 
#include "videofx.h"
#include "dirsnd.h"
#include "levels.h"
#include "AmbientSnd.h"
#include "Score.h"
#include "cells.h"
#include "amovie.h"

static BOOL bToggle = FALSE; // Temporary toggle flag for going between level 9 and 10

//#define WEAPONCHEAT

BOOL CALLBACK TurnEventCallback( HWND hWindow, LPVIDEO lpVideo, int iEventCode );
void OnVideoRButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnVideoLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnVideoLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);

/**********************************************************************************************/
CGame::CGame(HWND hWnd)
/**********************************************************************************************/
{
	if (!App.pGame)
		App.pGame = this;

	lpWorld = &World;

	m_hWnd = hWnd;
	pVidFx = NULL;
	Init();
}

/**********************************************************************************************/
CGame::~CGame()
/**********************************************************************************************/
{
	Free();
}

/**********************************************************************************************/
void CGame::Init(void)
/**********************************************************************************************/
{
	HWND hWnd = m_hWnd;

	bDieing = FALSE;
	m_bLowFuelWarn = FALSE;
	m_bScoreUp = TRUE;
	m_dwVirusStart = 0;
	m_lVirusDur = 0;

	hVideo = GetDlgItem( hWnd, IDC_VIDEO );
	lpVideo = (LPVIDEO)GetWindowLong( hVideo, GWL_DATAPTR );
	hRadar = GetDlgItem( hWnd, IDC_RADAR );
	lpWorld->Init(hRadar);
	bRadarFull = NO;
	if ( hRadar )
	{
		// Save the radar coordinates for future manipulation
		GetWindowRect(hRadar, &RadarRect);
		ScreenToClient(hWnd, (LPPOINT)&RadarRect.left);
		ScreenToClient(hWnd, (LPPOINT)&RadarRect.right);
	}

	// Install the event proc for turn effects
	if ( lpVideo )
		lpVideo->lpEventProc = TurnEventCallback;

	// Hook into the mouse on the video control
	Video_HookMouseButtons(OnVideoLButtonDown, OnVideoRButtonDown, OnVideoLButtonUp);

	MusicOne.Open(IDC_MUSIC1, App.GetInstance());
	Portal.Open(IDC_PORTALSND, App.GetInstance());
	WallRub.Open(IDC_NOTURNSND, App.GetInstance(), TRUE);
	SwitchSnd.Open(IDC_SWITCHSND, App.GetInstance());

	//PlayCar.Open(IDC_PLAYCAR, App.GetInstance());
	//PlayCar.SetVolume(-1500);

	// Initialize weapons system
	InitWeapons(hWnd);

	#ifdef WEAPONCHEAT
		Weapons.SetAmmo(0, -1); // scrap gun
		Weapons.SetAmmo(1, AMMO_MGUN); // machine gun;
		Weapons.SetAmmo(2, AMMO_MISSILES); // missiles
		Weapons.SetAmmo(3, -1); //  laser
		Weapons.SetAmmo(4, AMMO_MINES);
		Weapons.SetAmmo(5, AMMO_JACKS);
		Weapons.SetAmmo(6, AMMO_LEROYS);
		Weapons.SetAmmo(7, AMMO_BOMBS);
		Weapons.SetAmmo(8, AMMO_THUMPER);
	#else
		Weapons.SetAmmo(0, -1); // scrap gun
		Weapons.SetAmmo(1, 0); // machine gun;
		Weapons.SetAmmo(2, 0); // missiles
		Weapons.SetAmmo(3, -1); // laser
		Weapons.SetAmmo(4, 0);
		Weapons.SetAmmo(5, 0);
		Weapons.SetAmmo(6, 0);
		Weapons.SetAmmo(7, 0);
		Weapons.SetAmmo(8, 0);

	#endif

	Weapons.SetCurWeapon(0); // Scrap gun
	SendMessage( hWnd, WM_COMMAND, '1' + Weapons.GetCurWeapon(), 0L );

	// Initilialize Collectibles Class
	Collect.ReInit(hWnd);

	//TEMPORARY! TEST FOR 8-bit display (mc)
	HDC hDC = GetDC(hWnd);
	int iRes = GetDeviceCaps(hDC, NUMCOLORS);
	if (iRes != 20)
	{
		MessageBox(hWnd, "NOTE: You must be running in 8-bit, 256 color mode for the weapons to work properly. (TEMPORARY)", "Note", 0);
	}
	ReleaseDC(hWnd, hDC);

	lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy = METER_MAX );
	SetHealthDigitDisplay(hWnd, lEnergy);
	
	iKills = 0;
	SetKillDisplay(hWnd, iKills, YES/*bForcePaint*/);

	lpLastShot = NULL;

	// Play music
	App.bMusicState = ON;
	//if (App.bOptionMusic) // Start the music
	//	PostMessage( hWnd, WM_COMMAND, 'S', 0L );

	// Start at the beginning of the game
	Levels.SetLevel(1);

	pVidFx = new VideoFx(VFX_TYPE_TURN);

	//lpWorld->GetPlayer()->ChangeDirAbs(12);
	//lpWorld->GetPlayer()->SetNewShotCell();
	//CCell *pCell = lpWorld->GetPlayer()->GetCell();
	//pCell->SetVisible(TRUE);

	// Ensure a new suitable shot
	lpWorld->GetGroup()->BuildShot();

	bInGate = TRUE;

	Weapons.CurWeapon()->SetScore(0);

	MusicOne.CloseAll();
	MCISetAudioOnOff(lpVideo->hDevice, ON);

	// Tell video window to play now
	PostMessage( hVideo, WM_KEYDOWN, 'P', 0);
}

/**********************************************************************************************/
void CGame::Free(void)
/**********************************************************************************************/
{
	MusicOne.CloseAll();
	VideoClearAllObservers();
	VideoKillAllTimers();

	if ( lpVideo )
		Video_Close( lpVideo->hWnd, lpVideo );
}

//************************************************************************
void CGame::OnTimer(HWND hWnd, UINT id)
//************************************************************************
{
	// See if we are dying ..
	if (bDieing)
	{
		lEnergy = Meter_PostSet( hWnd, IDC_ENERGY, lEnergy - 200 );
		if (App.bEnergyCheat && lEnergy <= 1) lEnergy = Meter_PostSet( hWnd, IDC_ENERGY, lEnergy = METER_MAX );
		SetHealthDigitDisplay(hWnd, lEnergy);
		if (!pVidFx->TextUp())
		{
			pVidFx->TextOn(hWnd, "Danger: Engine burning up", RGB(255,0,0), 18, 4);
			m_bLowFuelWarn = TRUE;
		}

		Weapons.CurWeapon()->StartEffect(200, 249); // red

		if (lEnergy <= 0)
		{
			lEnergy = 0;
			bDieing = FALSE;

			// Post message for death scene
			PostMessage(hWnd, VK_SPACE, 0, 0);
		}
	}

	// Consume some fuel
	else
	{
		if (!bRadarFull)
		{
			lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= FUEL_DEC );
			if (App.bEnergyCheat && lEnergy <= 1) lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy = METER_MAX );
			SetHealthDigitDisplay(hWnd, lEnergy);
			if (lEnergy < 2000 && pVidFx)
			{
				if (!pVidFx->TextUp())
				{
					pVidFx->TextOn(hWnd, "Warning: Low Fuel", RGB(255,0,0), 18, 3);
					m_bLowFuelWarn = TRUE;
				}
			}
			else if (m_bLowFuelWarn && pVidFx && pVidFx->TextUp())
			{
				pVidFx->TextOff();
				m_bLowFuelWarn = FALSE;
			}
		}
	}

	static int iCount;
	if ( ++iCount == 15 )
	{ // update the frame number 15/iCount times a second

		// Move all opponents
		if (lpWorld->GetGroup()->MoveNormal())
			lpWorld->UpdateView();  

		// Update energy display
		lEnergy = Meter_PostSet(hWnd, IDC_ENERGY, lEnergy);
		if (App.bEnergyCheat && lEnergy <= 1) lEnergy = Meter_PostSet( hWnd, IDC_ENERGY, lEnergy = METER_MAX );
		SetHealthDigitDisplay(hWnd, lEnergy);

		iCount = 0;

		// Got a virus going?
		if (m_lVirusDur)
		{
			if ( (timeGetTime() - m_dwVirusStart) > (DWORD)m_lVirusDur)
			{
				m_lVirusDur = 0;
				lpWorld->SetVirus(NO);
			}
		}

	}

	// Adjust the possible timed lives of collectibles
	Collect.AdjustTimedLife(hWnd);
}

//************************************************************************
BOOL CGame::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	UINT loWord = (UINT)hControl;
	UINT hiWord = codeNotify;

	switch (id)
	{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			int iCurrentWeapon = id - '0';
			Weapons.SetCurWeapon(iCurrentWeapon-1);

			SetWeaponDisplay(hWnd, iCurrentWeapon);
			break;
		}

		case IDC_WEAPON:
		case VK_TAB: // Next weapon
		{	
			if (Weapons.GetCurWeapon() == 9)
				Weapons.SetCurWeapon(0);

			SendMessage( hWnd, WM_COMMAND, '1' + Weapons.GetCurWeapon() + 1, 0L );
			break;
		}

		case 'S':
		{
			if ( App.bMusicState == ON )
			{
				MusicOne.Stop();
				App.bMusicState = OFF;
			}
			else
			if ( App.bOptionMusic )
			{
				MusicOne.Play(TRUE);
				App.bMusicState = ON;
			}
			break;
		}

		case 'L':
		{ // Toggle the state of the "script" flag
			App.bOptionScript = !App.bOptionScript;
			MessageBeep(0);
			break;
		}

		case IDC_ZOOM:
		{
			if ( !lpVideo )
				break;

			if ( lpVideo->iZoomFactor == 2 )
				lpVideo->iZoomFactor = 1;
			else
				lpVideo->iZoomFactor = 2;
			SetWindowPos( hVideo, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE );
			break;
		}
    
		case IDC_EXIT:
		case IDC_OPTIONS:
		case IDC_SAVE:
		{
			MusicOne.Stop();

			// Remember where we were
			if (lpVideo)
				lSaveShot = lpVideo->lCurrentShot;
			else
				lSaveShot = 0;

			// Save the zoom factor
			if (lpVideo)
			{
				char szBuffer[2];
				szBuffer[0] = '0' + lpVideo->iZoomFactor;
				szBuffer[1] = 0;
				WritePrivateProfileString (OPTIONS_SECTION, "ZoomFactor", szBuffer, INI_FILE);
			}

			//SaveGame(hWnd, 0);
			if ( id == IDC_EXIT )
				App.GotoScene (hWnd, 2);
			else
			if ( id == IDC_OPTIONS )
			{
				App.bOptionsfromGame = TRUE;
				App.GotoScene (hWnd, 40);
			}
			else
			if ( id == IDC_SAVE )
				App.GotoScene (hWnd, 70);
			break;
		}

		case VK_F1:
		case VK_F2:
		case VK_F3:
		case VK_F4:
		case VK_F5:
		case VK_F6:
		case VK_F7:
		case VK_F8:
		case VK_F9:
		case VK_F10:
		case VK_F11:
		case VK_F12:
		{ // A level change key was pressed

			// TEMP FOR GETTING TO LEVEL 10
			if (id == VK_F9)
			{
				if (bToggle)
					id = VK_F10;

				bToggle = ~bToggle;
			}

			int iCurrentLevel = id - VK_F1 + 1;

			Weapons.CurWeapon()->Enable(FALSE);
			MusicOne.Stop();

			bInGate = TRUE;
			lpWorld->GetGroup()->Pause();

			// Stop the video to prevent problems when loading the new
			// map
			if (lpVideo->hDevice)
				MCIPause(lpVideo->hDevice);
			else
			if (lpVideo->pAMovie)
				lpVideo->pAMovie->Pause();

			MusicOne.CloseAll();
			MCISetAudioOnOff(lpVideo->hDevice, ON);

			FNAME szFileName;
			wsprintf(szFileName, "level%02d.txt", iCurrentLevel);
			LoadLevelFile(hWnd, szFileName, iCurrentLevel);

			char szShot[10];
			szShot[0] = '/';
			szShot[1] = 'C';
			szShot[2] = 'N';
			szShot[3] = LevelChar(iCurrentLevel);
			szShot[4] = NULL;

			LPSHOT lpShot = Video_FindShotPrefix(hVideo, szShot);
			if (lpShot)
			{
				Video_GotoShot( lpVideo->hWnd, lpVideo, lpShot->lShotID, 
					1/*lCount*/, 0/*iFrames*/, 0 );
			}

			Weapons.CurWeapon()->SetScore(0);

			break;
		}

		case 'C':
		{
			LoadCustomLevel(hWnd);
			break;
		}

		case 'T':
		{ // Toggle the state of the "transition/turn effects" flag
			App.bOptionTransitions = !App.bOptionTransitions;
			MessageBeep(0);
			break;
		}

		//debug (mc)
		case 'B':
		{
			static bSlow = FALSE;
			Weapons.CurWeapon()->BigExplode();
			bSlow = !bSlow;
			if (bSlow)
				MCISetVideoSpeed(lpVideo->hDevice, 70);
			else
				MCISetVideoSpeed(lpVideo->hDevice, 1000);
			break;
		}

		case 'W':
		{
			App.bShowGrid = !App.bShowGrid;
			Weapons.CurWeapon()->SetShowGrid(App.bShowGrid);
			break;
		}

		case 'Q':
		{
			App.bEnergyCheat = !App.bEnergyCheat;
			break;
		}

		case 'R':
		{
			bRadarFull = !bRadarFull;
			BlowupRadar( bRadarFull );
		}
		break;

		case 'E':
		{
			Weapons.CurWeapon()->ClearBulletCracks();
			lEnergy -= 200;
			break;
		}

		case VK_BACK:
		{
			MusicOne.CloseAll();
			MCISetAudioOnOff(lpVideo->hDevice, ON);

			LPSHOT lpShot;		
			if (lpShot = Video_FindShotName( hVideo, "/Death"))
				Video_GotoShot(lpVideo->hWnd, lpVideo, lpShot->lShotID,
					1/*lCount*/, 0/*iFrames*/, 0 );

			lEnergy = 1; // so we don't hit this again
			//if (App.bOptionTaunts)
			//	AmbSnds.Play(DeathSnd);

			Weapons.CurWeapon()->ClearBulletCracks();
			VideoClearObserver(Weapons.CurWeapon());
			break;
		}

		case IDC_WEAPONFIRE:
		{ // The weapon fire key was pressed
			FireWeapon(hWnd, hiWord);
			break;
		}

		case 'D':
		{
			Weapons.CurWeapon()->MoveSightLeft();
			break;
		}

		case 'G':
		{
			Weapons.CurWeapon()->MoveSightRight();
			break;
		}

		case IDC_VIDEO:
		{
			// See if this is our 'special' post notification
			if (loWord == 0)
			{
				// Implement turn adjustments to the weapon system
				if (hiWord == EVENT_LEFT)
					Weapons.CurWeapon()->AdjustLeft();
				else
				if (hiWord == EVENT_RIGHT)
					Weapons.CurWeapon()->AdjustRight();
				else
				if (hiWord == EVENT_DOWN)
					Weapons.CurWeapon()->AdjustReverse();

				break;
			}

			LPPSHOT lppNewShot = (LPPSHOT)MAKELPARAM( loWord, hiWord );
			if ( !lppNewShot )
				break; // its a stop code

			LPSHOT lpNewShot = *lppNewShot;
			if ( !lpNewShot )
				break;

			// Are we entering a new cell?
			if ( lpNewShot->lFlags & FLAGBIT(F_NEWCELL) )
			{
				//MusicOne.Reset();
				Portal.Play();
  				LPSHOT lpShot = GetNewCellShot( hWnd, hVideo );
				if ( lpShot )
				{ // force the shot change
					*lppNewShot = lpShot;

					// Pass the event on, since we may key off of it later
					lpShot->iLastEvent = lpNewShot->iLastEvent;
					lpNewShot = lpShot;
					// Inform HUD that we are changing cells
					SetupHUD(hWnd, YES/*bDisplay*/);
				}
			}

			if ( !lpLastShot )
			{
				lpLastShot = lpNewShot;
				break;
			}

			// If we hit spine...
			if ( lpNewShot->lFlags & FLAGBIT(F_SPINE_HIT) )
			{
				lHealthSpine -= (METER_MAX/5);
				if ( lHealthSpine <= 0 )
				{ // Spine is out of the game!!!
					LPSHOT lpShot;
					if ( lpShot = Video_FindShotFlag( hVideo, F_SPINE_DEAD ) )
						*lppNewShot = lpShot; // force the shot change
					break;
				}
			}

			// If we hit another car and loose energy...
			if ( lpNewShot->lFlags & FLAGBIT(F_HIT) )
			{
				int iDamage = Weapons.CurWeapon()->GetCurCrashDamage();
				if (iDamage > 0)
				{
					STRING szText;
					wsprintf(szText, "%d damage!", iDamage);
					pVidFx->TextOn(hWnd, szText, RGB(255,0,0), 18, 3000, 3);
					lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= iDamage );

					if (App.bEnergyCheat && lEnergy <= 1) lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy = METER_MAX );
					SetHealthDigitDisplay(hWnd, lEnergy);
				}
				//if (App.bOptionTaunts)
				//	AmbSnds.Play(TauntSnd);
			}
			
			// or up against the wall
			else if ( lpNewShot->lFlags & FLAGBIT(F_POS_WALL) )
			{
				lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/20) );
				if (App.bEnergyCheat && lEnergy <= 1) lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy = METER_MAX );
				SetHealthDigitDisplay(hWnd, lEnergy);
			}

			// See if we have to leave the game
			if ( lEnergy <= 0 && !(lpNewShot->lFlags & FLAGBIT(F_LIVE_DIE)) )
			{ // We lose!!!

				Score.Set(Weapons.CurWeapon()->GetScore());
				if (pVidFx)
				{
					STRING szScore;
					wsprintf(szScore, "Kills: %d, Score: %d", iKills, 
						Weapons.CurWeapon()->GetScore(), RGB(0,255,255), 24);
					pVidFx->TextOn(hWnd, szScore, RGB(255,255,255), 24, 0, 5);
					m_bScoreUp = TRUE;
				}
				//Score.Save();

				MusicOne.CloseAll();
				MCISetAudioOnOff(lpVideo->hDevice, ON);

				LPSHOT lpShot;		
				if (lpShot = Video_FindShotName( hVideo, "/Death"))
					*lppNewShot = lpShot; // force the shot change
				lEnergy = 1; // so we don't hit this again
				//if (App.bOptionTaunts)
				//	AmbSnds.Play(DeathSnd);

				Weapons.CurWeapon()->ClearBulletCracks();
				Weapons.CurWeapon()->Enable(FALSE);
				//VideoClearAllObservers();
			}

			// Are we done?
			if ( lpLastShot->lFlags & FLAGBIT(F_EXIT) )
			{
				Weapons.CurWeapon()->ClearBulletCracks();
				*lppNewShot = NULL; // don't allow the new shot to be started
				//FORWARD_WM_COMMAND( hWnd, IDC_EXIT, hWnd, 0, SendMessage );
				App.GotoScene( hWnd, 2/*iScene*/ );
				break;
			}

			if ( lpNewShot->iLastEvent != EVENT_TIMEOUT )
			{
				BOOL bShotFlag = IndentShot( hWnd, lpNewShot, lpLastShot );
				if ( !bShotFlag )
				{ // if the new shot is rejected by IndentShot...
					*lppNewShot = NULL; // don't allow the new shot to be started
					break;
				}
			}

			// Keep a running directional "compass"
			if ( lpNewShot->lFlags & FLAGBIT(F_AUTOREVERSE) )
				NewDirection( hWnd, EVENT_DOWN, 0 );
			else
			{
				// If union jack
				if (lpLastShot->szShotName[1] == 'U')
					NewDirection( hWnd, lpNewShot->iLastEvent, 'U' );
				else
					NewDirection( hWnd, lpNewShot->iLastEvent, 0 );
			}

			// See if we need to update the kill counter
			if ( lpNewShot->iLastEvent == EVENT_HOME)
			{
				iKills++;
				SetKillDisplay(hWnd, iKills, NO/*bForcePaint*/);

				//Moved this to opponents (mc)
				//Levels.SetState(LEVENT_KILL, (DWORD)&World);

				//if (App.bOptionTaunts)
				//	AmbSnds.Play(PraiseSnd);
			}

			Weapons.CurWeapon()->SetShot( lpNewShot );

			// If we were in a gate then we need to start the opponents moving
			if (bInGate)
			{
				lpWorld->GetGroup()->UnPause();
				bInGate = FALSE;
			}

			lpLastShot = lpNewShot;

			break;
		}

		case IDC_CLOSE:
		{
			//FORWARD_WM_COMMAND( hWnd, IDC_EXIT, hWnd, 0, SendMessage );
			App.GotoScene( hWnd, 2/*iScene*/ );
			break;
		}

		default:
		{
			return( FALSE );
		}
	}
    
	return( TRUE );
}

typedef struct {
	LPSTR	lpName;
	BOOL	bWeapons;
} SCRIPT;

// Scripted shot data for demos
static int iScriptedShot;
static SCRIPT ScriptedShots[] = {
	"/H01SA",			YES,
	"/U12WC",			YES,
	"/U02S2",			YES,
	"/H01SCW2",			YES,
	"/H13SC",			YES,
	"/BARCRASH",		NO,
	"/H23WC",			YES,
	"/U02S4",			YES,
	"/H03SC",			YES,
	"/U06WCWA",			YES,
	"/H11WC",			YES,
	"/H01S4",			YES,
	"/BARSTOP",			NO,
	"/H03WC",			YES,
	"/U06WA",			YES,
	"/H01W2",			YES,
	"/H21SC",			YES,
	"/U04SA",			YES,
	"/U04WC",			YES,
	"/H01W8",			YES,
	"/H03S2",			YES,
	"/U06WC",			YES,
	"/CW",				NO,
	"/Death_9_01",		NO
};

/***********************************************************************/
LPSHOT CGame::GetNewCellShot( HWND hWnd, HWND hVideo )
/***********************************************************************/
{
	lpWorld->GetPlayer()->FixDir();
	int iDir = lpWorld->GetPlayer()->GetDir();

	// Before moving ahead, see if we are about to go through a barricade
	// If barricades on and a barricade is not already broken
	STRING szShot;
	// Carry around a flag to indicate whether a barricade shot is playing
	static BOOL bBarricadeShotPlaying;
	if (App.bOptionBarricades && !bBarricadeShotPlaying && !App.bOptionScript)
	{
		CCell * pCell = lpWorld->GetPlayer()->GetCell();
		char cType = ( pCell ? pCell->PortalDir(iDir) : NULL );
		if (cType == 'B' || cType == 'b')
		{ // We hit a barricade
			int iPortionToLose = ( cType == 'B' ? 30 : 40 ); // lose 30 or 40 percent
			int iPortionToTest = ( cType == 'B' ? 2 : 3 ); // test for a half or a third
			lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/iPortionToLose));
			if (App.bEnergyCheat && lEnergy <= 1) lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy = METER_MAX );
			SetHealthDigitDisplay(hWnd, lEnergy);

			szShot[0] = '/';
			szShot[1] = 'B';

			if (lEnergy >= (METER_MAX/iPortionToTest))
			{
				szShot[2] = 'C';
				STRING szText;

				wsprintf(szText, "%d damage, need %d energy to break!", 
					(METER_MAX/iPortionToLose), ((iPortionToTest + 500) / 100) - 1);
				pVidFx->TextOn(hWnd, szText, RGB(255,0,0), 18, 3000, 3);
			}
			else
			{
				szShot[2] = 'B';
				STRING szText;

				wsprintf(szText, "%d damage!", (METER_MAX/iPortionToLose));
				pVidFx->TextOn(hWnd, szText, RGB(255,0,0), 18, 3000, 3);

			}
			szShot[3] = '\0';
			bBarricadeShotPlaying = TRUE;
			return( Video_FindShotPrefix( hVideo, szShot ) );
		}
	}

	// If we get this far, clear the barricade shot playing flag
	bBarricadeShotPlaying = FALSE;

	// We are now ready to move into the new cell
	lpWorld->GetPlayer()->StartTick(); // player enters cell
	lpWorld->GetPlayer()->Forward();
	lpWorld->CenterPlayerView(YES/*bScroll*/);

	// Time to generate a new shot name for the new cell location
	// Try the new naming mechanism, if it doesn't find anything then regress...
	if ( !App.bOptionScript )
	{
		iScriptedShot = 0; // reset the scripted shot index
		return( NewGetNewCellShot(hWnd, NULL) );
	}
	else
	{ // Use this code when running a canned script of shots
		// Clear the barricade shot playing flag
		int iShot = iScriptedShot;
		int nScriptedShots = sizeof(ScriptedShots) / sizeof(SCRIPT);
		if (++iScriptedShot >= nScriptedShots)
			iScriptedShot = 0;

		return( NewGetNewCellShot(hWnd, ScriptedShots[iShot].lpName) );
	}

	// Try the older methods
	CCell* pCell = lpWorld->GetPlayer()->GetCell();
	if ( !pCell )
		return( NULL );

	// 0th byte:
	// The slash
	szShot[0] = '/';
	int i = 1;

	// 1st byte:
	// The cell type
	char cType = pCell->Type();
	char cLook = pCell->Look();
	switch ( cType )
	{
		case 'B': // black hole
			szShot[i++] = cType;
			break;
		case '`': // diag 10-4 tunnel
		case '|': // vert tunnel
		case '/': // diag 2-8 tunnel
			szShot[i++] = 'T';
			break;
		case 'C': // cul-de-sac
		{
			char cCul = pCell->PortalDir(iDir);
			szShot[i++] = 'C';
			szShot[i++] = cCul;

			switch(cCul)
			{
			case 'X':
				ManageGateway(hWnd);
				break;
			case 'N':
				ManageEntry(hWnd);
				break;

			case 'A':   // Machine gun ammo
			case 'R':	// Missiles
			case 'S':	// Mines
			case 'K':	// Bombs
				break;

			// Empty cul-de-sac, do live/die wheel
			case 'M':
			{
				if (GetRandomNumber(2) == 1)
				{
					szShot[i-1] = 'D';
					lEnergy = Meter_Set( hWnd, IDC_ENERGY, 0);
					if (App.bEnergyCheat && lEnergy <= 1) lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy = METER_MAX );
					SetHealthDigitDisplay(hWnd, 0);
				}
				else
					szShot[i-1] = 'L';
				szShot[i] = '\0';
				break;
			}
			
			// Level specific collectibles
			default:
				char szLevel[5];
				int iLevel = lpWorld->GetLevel();
				_itoa(iLevel, szLevel, 10);
				szShot[i++] = '_';
				szShot[i] = '\0';
				strcat(szShot, szLevel);

				// Set the state for collecting this collectible
				Levels.SetState(cCul - '0', 0);
			}

			// Bypass the "look" byte
			goto Terminate;
		}
		default:
			return( NULL );
	}

	// 2nd byte:
	// The look
	if ( cLook )
	{
		szShot[i++] = cLook;

		// See if we need to start dieing...

		// We are in a hell cell and don't have an extinguisher...
		if (cLook == '1' && !Levels.HaveFireExtinguisher())
			bDieing = TRUE;
		else
			bDieing = FALSE;
	}
	else
		szShot[i++] = '0';

	Terminate:
	// The final byte:
	// Null terminate
	szShot[i++] = '\0';

	return( Video_FindShotPrefix( hVideo, szShot ) );
}

/***********************************************************************/
LPSHOT CGame::NewGetNewCellShot(HWND hWnd, LPSTR lpShotName)
/***********************************************************************/
{
	STRING szShot;

	lpWorld->GetPlayer()->FixDir();

	if (m_bScoreUp)
	{
		m_bScoreUp = FALSE;
		if (pVidFx)
			pVidFx->TextOff();
	}

	if ( lpShotName )
		lstrcpy( szShot, lpShotName );
	else
	{
		lpShotName = lpWorld->GetGroup()->GetCurShot();
		if (!lpShotName)
			return(NULL);

		szShot[0] = '/';
		szShot[1] = NULL;
		if (lpShotName[0] == NULL)
			lstrcat(szShot,"CM");
		else
			lstrcat(szShot, lpShotName);
	}

	// If a death
	if (szShot[1] == 'D')
	{
		Weapons.CurWeapon()->Enable(FALSE);

		// Turn music off
		if (App.bOptionMusic && App.bMusicState == ON)
			MusicOne.Stop();

		MusicOne.CloseAll();
		MCISetAudioOnOff(lpVideo->hDevice, ON);
	}
	else
	// If a cul-de-sac
	if (szShot[1] == 'C')
	{
		Weapons.CurWeapon()->Enable(FALSE);

		// Modify if gate shot
		if (szShot[2] == 'X')
		{
			// Turn music off
			if (App.bOptionMusic && App.bMusicState == ON)
				MusicOne.Stop();

			MusicOne.CloseAll();
			MCISetAudioOnOff(lpVideo->hDevice, ON);
			
			// Check for special level operations
			if (lpWorld->GetLevel() == 6)
			{
				ShowLevel6Clue(hWnd);
			}

			// If we are going to the next level
			if (ManageGateway(hWnd))
			{
				szShot[2] = 'N';
				szShot[3] = LevelChar(lpWorld->GetLevel());
				if (pVidFx)
				{
					STRING szScore;
					wsprintf(szScore, "Kills: %d, Score: %d", iKills, 
						Weapons.CurWeapon()->GetScore());
					pVidFx->TextOn(hWnd, szScore, RGB(255,255,255), 24, 0, 5);
					m_bScoreUp = TRUE;
				}

				// Restore energy between levels
				lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy = METER_MAX );
				SetHealthDigitDisplay(hWnd, lEnergy);
			}
			else
			{
				szShot[3] = LevelChar(lpWorld->GetLevel());

				// Enable weapon for level 10 to shoot the gatekeeper
				if (lpWorld->GetLevel() == 10)
					Weapons.CurWeapon()->Enable(TRUE);
			}

			szShot[4] = NULL;

			bInGate = TRUE;
			lpWorld->GetGroup()->Pause();
		}
			// If back in entrance gate
		else if (szShot[2] == 'N')
		{
			// Turn music off
			if (App.bOptionMusic && App.bMusicState == ON)
				MusicOne.Stop();

			MusicOne.CloseAll();
			MCISetAudioOnOff(lpVideo->hDevice, ON);

			szShot[3] = LevelChar(lpWorld->GetLevel());
			szShot[4] = NULL;		
		}
		// If 'speaking' shot
		else if ( SpeakingShot(szShot[2]) )
		{
			// Turn music off
			if (App.bOptionMusic && App.bMusicState == ON)
				MusicOne.Stop();

			MusicOne.CloseAll();
			MCISetAudioOnOff(lpVideo->hDevice, ON);

			// Check if collectable
			if (szShot[2] >= '1' && szShot[2] <= '9')
			{
				szShot[3] = LevelChar(lpWorld->GetLevel());
				szShot[4] = NULL;

				// Set the state for collecting this collectible
				Levels.SetState(LEVENT_GET_COLL, szShot[2] - '0');

				// special level operations
				if (lpWorld->GetLevel() == 2)
				{
					if (szShot[2] == '2')
						bDieing = FALSE;
				}
			}

			// Check if we need to set ammo
			if (SetAmmo(hWnd, szShot[2]))
			{
				// Zap the ammo cul-de-sac
				ZapCurrentPortal();
			}
		}

		// Check if special code
		else if ((BYTE)szShot[2] >= PORT_START &&
			(BYTE)szShot[2] <= PORT_END)
		{
			BYTE b = (BYTE)szShot[2];

			// If opponent generator switch off
			if (b >= PORT_SWITCH_GEN1OFF && b <= PORT_SWITCH_GEN5OFF)
			{
				int iGen = b - PORT_SWITCH_GEN1OFF + 1;

				MusicOne.CloseAll();
				MCISetAudioOnOff(lpVideo->hDevice, ON);

				// Turn off the opponent generator
				lpWorld->GetGroup()->OppGenOff(iGen);

				lstrcpy(szShot, "/SWOFF");
				SwitchSnd.Play();
				if (pVidFx)
				{
					STRING szText;
					wsprintf(szText, "Opponent Generator %d Off!", iGen);
					pVidFx->TextOn(hWnd, szText, RGB(255,255,0), 18, 10000, 4);
				}
			}	
			else if (b >= PORT_SWITCH_CELLSON1 && b <= PORT_SWITCH_CELLSON5)
			{
				int iGen = b - PORT_SWITCH_CELLSON1 + 1;

				MusicOne.CloseAll();
				MCISetAudioOnOff(lpVideo->hDevice, ON);

				lpWorld->SwitchCellsOn(lpWorld->GetLevel(), iGen);
				Levels.SetState(LEVENT_SWITCH_ON, 0);
				lstrcpy(szShot, "/SWON");
				SwitchSnd.Play();				
				if (pVidFx)
				{
					STRING szText;
					wsprintf(szText, "Cell area %d on!", iGen);
					pVidFx->TextOn(hWnd, szText, RGB(255,255,0), 18, 10000, 4);
				}
			}
		}
	}
	// If opponent generator
	else
	if (szShot[1] == 'O')
	{
		szShot[1] = 'C';
		szShot[2] = 'M';
		szShot[3] = NULL;
	}
	// If Radioactive
	else
	if (szShot[1] == 'R')
	{
		szShot[1] = 'H';
	}
	else
	{
		Weapons.CurWeapon()->Enable(TRUE);
		//if (pVidFx && pVidFx->TextUp())
		//	pVidFx->TextOff();

		if (App.bOptionMusic && App.bMusicState == ON && MusicOne.Stopped())
		{
			MCISetAudioOnOff(lpVideo->hDevice, OFF);
			MusicOne.ReloadAll();
			MusicOne.Play(TRUE);
		}
		//if (PlayCar.Stopped())
		//	PlayCar.Play(TRUE);
	}

	LPSHOT lpShot = Video_FindShotPrefix(hVideo, szShot);
	if (!lpShot)
	{
#ifdef _DEBUG
		OutputDebugString("Shot not found.\n");
#endif
		lstrcat(szShot, lpShotName);
		for(int i=4; i<11; i++)
			szShot[i] = '_';
		szShot[i] = NULL;
		lpShot = Video_FindShotPrefix(hVideo, szShot);
	}

	// Tell the weapons system that we are going to enter a new cell
	if (Weapons.CurWeapon())
		Weapons.CurWeapon()->NewCell( szShot[1] );

	// Manage portal stuff
	CCell *pCell = lpWorld->GetPlayer()->GetCell();
	if (pCell)
	{
		// get direction behind player
		int iDir = lpWorld->GetPlayer()->GetDir();
		iDir -= 6;
		if (iDir < 1)
			iDir += 12;

		PassThruPortal(hWnd, pCell, iDir );
	}

	return lpShot;
}

/***********************************************************************/
BOOL CGame::ManageGateway(HWND hWnd)
/***********************************************************************/
{
	if (Levels.HaveGateKey())
	{
		int iCurrentLevel = lpWorld->GetLevel() + 1;

		FNAME szFileName;
		wsprintf(szFileName, "level%02d.txt", iCurrentLevel);
		LoadLevelFile(hWnd, szFileName, iCurrentLevel);
		return (TRUE);
	}

	return FALSE;
}

/***********************************************************************/
BOOL CGame::ManageEntry(HWND hWnd)
/***********************************************************************/
{
	return(TRUE);
}

//************************************************************************
char CGame::LevelChar(int iLevel)
//************************************************************************
{
	// Add the level code
	char cLevel;
	if (iLevel < 10)
		cLevel = iLevel + '0';
	else
		cLevel = (iLevel - 10) + 'A';

	return cLevel;
}

/***********************************************************************/
BOOL CGame::SpeakingShot(char c)
/***********************************************************************/
{
	if (c == 'W' || c == 'G' || c == 'H' || (c >= '0' && c <= '9') || 
		c == 'A' || c == 'R' || c == 'h' || c == 'i' || c == 'j' || 
		c == 'k' || c == 'l' || c == 'm' || c == 'n' || c == 'S')
		return TRUE;

	return FALSE;
}

//************************************************************************
BOOL CGame::Turn(HWND hWnd, int iEventCode)
//************************************************************************
{
	if ( !pVidFx && !(pVidFx = new VideoFx(VFX_TYPE_TURN)) )
		return( NO );

	// Make sure it is legal to turn
	if (iEventCode == EVENT_LEFT || iEventCode == EVENT_RIGHT)
	{
		// See if we are in the event range to allow turning
		if (lpVideo)
		{
			LPSHOT lpShot = Video_GetCurrentShot(lpVideo);
			if ( lpShot && lpShot->EventRange.nStart != -1 )
			{
				long lFrame;
				if (lpVideo->hDevice)
					lFrame = MCIGetPosition(lpVideo->hDevice);
				else
				if (lpVideo->pAMovie)
					lFrame = lpVideo->pAMovie->GetPosition();

				lFrame -= lpShot->lStartFrame;

				// If not in event range
				if ( (lFrame < lpShot->EventRange.nStart) ||
					 (lFrame > lpShot->EventRange.nEnd) )
				{
#ifdef _DEBUG

					char debug[81];
					wsprintf(debug,"no turn lframe=%d, start=%d, end=%d\n", 
						(int)lFrame, lpShot->EventRange.nStart, lpShot->EventRange.nEnd);
					OutputDebugString(debug);
#endif

					STRING szText;

					wsprintf(szText, "%d damage!", DAMAGE_WALL_CRASH);
					pVidFx->TextOn(GetParent(hWnd), szText, RGB(255,0,0), 18, 3000, 3);

					lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= DAMAGE_WALL_CRASH );
					if (App.bEnergyCheat && lEnergy <= 1) 
						lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy = METER_MAX );
					SetHealthDigitDisplay(hWnd, lEnergy);

					// Set off wall screech sound
					if (iEventCode == EVENT_LEFT)
						WallRub.SetPosition(-10, 0, 0);
					else
						WallRub.SetPosition(10, 0, 0);

					WallRub.Play();

					return TRUE;
				}
			}
		}
	}

	LPSHOT lpNewShot = NULL;
	// We need to see if there is a car shot to turn to. If there are enganged opponents,
	// we need to see where they are at in relation to this turn. They may have to be
	// brought into view.
	
	if ( iEventCode == EVENT_LEFT ||
		 iEventCode == EVENT_RIGHT ||
		 iEventCode == EVENT_DOWN)
	{

		// Adjust the engaged opponents in relation to the turn we are doing
		int iAdjust;
		if (iEventCode == EVENT_LEFT)
			iAdjust = -1;
		else if (iEventCode == EVENT_RIGHT)
			iAdjust = 1;
		else if (iEventCode == EVENT_DOWN)
			iAdjust = 6;
		lpWorld->GetGroup()->AdjustEngaged(iAdjust);

		// Now get the new enaged car string
		char *pszCars = lpWorld->GetGroup()->GetEngagedCars();
		if (pszCars && pszCars[0] && lpVideo)
		{
			LPSHOT lpOldShot = Video_GetCurrentShot(lpVideo);
			STRING szShot;
			lstrcpyn(szShot, lpOldShot->szShotName, 5);
			lstrcat(szShot, pszCars);
			lstrcat(szShot, "_");

			Debug("szShot before=<%s>",szShot);

			// Adjust gate
			if (iEventCode == EVENT_LEFT)
				szShot[3] -= 1;
			else if (iEventCode == EVENT_DOWN)
				szShot[3] += 2;
			else
				szShot[3] += 1;

			// Check for wrap around
			if (szShot[1] == 'H') // hex
			{
				if (szShot[3] < '1')
					szShot[3] += 4;
				else if (szShot[3] > '4')
					szShot[3] -= 4;
			}
			else // union jack
			{
				if (szShot[3] < '1')
					szShot[3] += 7;
				else if (szShot[3] > '7')
					szShot[3] -= 7;

				if (szShot[3] == '1') // kludge, gate 1 doesn't exist (same as 7)
					szShot[3] = '7';
			}

			Debug("szShot b vidfind <%s>", szShot);
			lpNewShot = Video_FindShotPrefix(hVideo, szShot);
			Debug("szShot after <%s>", szShot);
			if (lpOldShot && lpNewShot)
			{
				lpNewShot->iLastEvent = iEventCode;
				//IndentShot(hWnd, lpNewShot, lpOldShot);

				//NewDirection( hWnd, iEventCode );

			}
		}
	}

	return( pVidFx->DoFx( hWnd, iEventCode, lpNewShot ) );
}

//************************************************************************
BOOL CGame::IndentShot( HWND hWnd, LPSHOT lpNewShot, LPSHOT lpShot )
//************************************************************************
{
	if ( !lpNewShot || !lpShot || !lpVideo )
		return( YES );

	// If the shot change is not the result of a turn, process the shot normally
	if ( lpNewShot->iLastEvent == EVENT_END )
		return( YES );

	// If both shots aren't flagged to be indentable, get out and play the shot normally
	if ( !(lpNewShot->lFlags & (FLAGBIT(F_HX) | FLAGBIT(F_UJ))) )
		return( YES );
	if ( !(lpShot->lFlags    & (FLAGBIT(F_HX) | FLAGBIT(F_UJ))) )
		return( YES );

	// If we are against the wall, get out and play the shot normally
	if ( (lpShot->lFlags & (FLAGBIT(F_POS_WALL))) )
		return( YES );
	
	long lFramesIn;
	if (lpVideo->hDevice)
		lFramesIn = MCIGetPosition(lpVideo->hDevice);
	else
	if (lpVideo->pAMovie)
		lFramesIn = lpVideo->pAMovie->GetPosition();

	if ( lFramesIn < lpShot->lStartFrame || lFramesIn > lpShot->lEndFrame )
		return( YES );

	lFramesIn -= lpShot->lStartFrame;

	long lLength;
	POINT pt1, pt2;

	//char debug[81];
	//OutputDebugString("---------\n");
	//wsprintf(debug, "From: <%s> frames in=%d, Range start=%d, end=%d\n", 
	//	lpShot->szShotName, (int)lFramesIn, lpShot->EventRange.nStart, lpShot->EventRange.nEnd);
	//OutputDebugString(debug);
	//wsprintf(debug, "To: <%s> range start=%d, end=%d\n", 
	//	lpNewShot->szShotName, lpNewShot->EventRange.nStart, lpNewShot->EventRange.nEnd);
	//OutputDebugString(debug);

	// If there is an event range for the old shot
	if (lFramesIn >= lpShot->EventRange.nStart && lFramesIn <= lpShot->EventRange.nEnd)
	{
		// Adjust frames in based on the event range
		lFramesIn -= lpShot->EventRange.nStart;

		// Compute the length based on the event range
		lLength = lpShot->EventRange.nEnd - lpShot->EventRange.nStart + 1;
		if (lLength <=0 )
			return( YES );

		// Reverse the indent amount if turning around
		if ( lpNewShot->iLastEvent == EVENT_UP || lpNewShot->iLastEvent == EVENT_DOWN )
			lFramesIn = lLength - lFramesIn;

		// Get the player's travel vector
		if ( !GetShotVector( &pt1, &pt2, lpShot->lFlags ) )
			return( YES );

		// Adjust the y values to full since we will be using the event ranges
		// to do scaling
		//pt1.y = 0;
		//pt2.y = 1000;
	}
	else
	{

#ifdef _DEBUG
		OutputDebugString("no lpshot event range\n");
#endif
		// Compute the length of the current shot
		lLength = lpShot->lEndFrame - lpShot->lStartFrame + 1;
		if ( lLength <= 0 )
			return( YES );

		// Reverse the indent amount if turning around
		if ( lpNewShot->iLastEvent == EVENT_UP || lpNewShot->iLastEvent == EVENT_DOWN )
			lFramesIn = lLength - lFramesIn;

		// Get the player's travel vector
		if ( !GetShotVector( &pt1, &pt2, lpShot->lFlags ) )
			return( YES );
	}

	// Compute the player's point on the track
	// Compute how far into the current shot we are
	double fScale = (double)lFramesIn / lLength;
	if ( fScale < 0 ) fScale = 0;
	if ( fScale > 1.0 ) fScale = 1.0;

	POINT pt;
	pt.x = pt1.x + (long)( (pt2.x - pt1.x) * fScale );
	pt.y = pt1.y + (long)( (pt2.y - pt1.y) * fScale );

	// Get the player's NEW travel vector
	if ( !GetShotVector( &pt1, &pt2, lpNewShot->lFlags ) )
		return( YES );

	// Compute the point on the new travel vector that is at the shortest distance
	ShortestDistance( &pt1, &pt2, &pt, &pt/*lpOutPoint*/, &fScale );

	if ( fScale < 0 ) fScale = 0;
	if ( fScale > 1.0 ) fScale = 1.0;

	long lNewLength;
	long lFrameOffset;

	// If using the event range
	if (lpNewShot->EventRange.nStart > -1 && lpNewShot->EventRange.nEnd > -1)
	{
		// Compute the length of the NEW shot
		lNewLength =lpNewShot->EventRange.nEnd - lpNewShot->EventRange.nStart + 1;
		if (lNewLength <= 0)
			return( YES );

		// Compute the indentation
		lFrameOffset = lpNewShot->EventRange.nStart + (long)( lNewLength * fScale );
	}
	else
	{
#ifdef _DEBUG
		OutputDebugString("no lpNewShot event range\n");
#endif
		// Compute the length of the NEW shot
		lNewLength = lpNewShot->lEndFrame - lpNewShot->lStartFrame + 1;
		if ( lNewLength <= 0 )
			return( YES );

		// Compute the indentation
		lFrameOffset = (long)( lNewLength * fScale );
	}

	lpNewShot->lIndentFrames = lFrameOffset;
	lpNewShot->lIndentFrames &= (~3); // put it on a key frame boundary

	//wsprintf(debug, "lIndentFrames = %d\n", (int)lFrameOffset);
	//OutputDebugString(debug);

	return( YES );
}

//************************************************************************
BOOL CGame::GetShotVector( LPPOINT lpPoint1, LPPOINT lpPoint2, long lFlags )
//************************************************************************
{
	if ( !lpPoint1 || !lpPoint2 )
		return( NO );

	if ( !(lFlags & (FLAGBIT(F_HX) | FLAGBIT(F_UJ))) )
		return( NO );

	POINT pt1, pt2;

	// Set Hex shot coordinates
	if ( lFlags & (FLAGBIT(F_HX)) )
	{
		if ( lFlags & FLAGBIT(F_POS_BEG) )
		{
			pt1.x =  0;		pt1.y = 0;
			pt2.x =  0;		pt2.y = 500;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_BEG_3QTR) )
		{
			pt1.x =  0;		pt1.y = 0;
			pt2.x =  0;		pt2.y = 900;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_MID) )
		{
			pt1.x =  0;		pt1.y = 500;
			pt2.x =  0;		pt2.y = 500;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_END) )
		{
			pt1.x =  0;		pt1.y = 500;
			pt2.x =  0;		pt2.y = 1000;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_FULL) )
		{
			pt1.x =  0;		pt1.y = 0;
			pt2.x =  0;		pt2.y = 1000;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_FULLC) )
		{
			pt1.x =  300;	pt1.y = 0;
			pt2.x = -300;	pt2.y = 1000;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_WALL) )
		{
			pt1.x =  300;	pt1.y = 0;
			pt2.x =  300;	pt2.y = 0;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_VEER) )
		{ // assume veer left
			pt1.x =  0;		pt1.y = 0;
			pt2.x = -300;	pt2.y = 1000;
		}
		else
			return( NO );
	}
	else
	// Set Union Jack shot coordinates
	if ( lFlags & (FLAGBIT(F_UJ)) )
	{
		if ( lFlags & FLAGBIT(F_POS_BEG) )
		{
			pt1.x =  0;		pt1.y = 0;
			pt2.x =  0;		pt2.y = 500;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_BEG_3QTR) )
		{
			pt1.x =  0;		pt1.y = 0;
			pt2.x =  0;		pt2.y = 900;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_MID) )
		{
			pt1.x =  0;		pt1.y = 500;
			pt2.x =  0;		pt2.y = 500;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_END) )
		{
			pt1.x =  0;		pt1.y = 500;
			pt2.x =  0;		pt2.y = 1000;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_FULL) )
		{
			pt1.x =  0;		pt1.y = 0;
			pt2.x =  0;		pt2.y = 1000;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_FULLC) )
		{
			pt1.x =  100;	pt1.y = 400;
			pt2.x = -100;	pt2.y = 600;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_WALL) )
		{
			pt1.x =  100;	pt1.y = 400;
			pt2.x =  100;	pt2.y = 400;
		}
		else
		if ( lFlags & FLAGBIT(F_POS_VEER) )
		{
			pt1.x =  0;		pt1.y = 0;
			pt2.x = -100;	pt2.y = 600;
		}
		else
			return( NO );
	}

	*lpPoint1 = pt1;
	*lpPoint2 = pt2;
	return( YES );
}

//************************************************************************
BOOL CGame::ShortestDistance( LPPOINT lpBeg, LPPOINT lpEnd, LPPOINT lpIn, LPPOINT lpOut, LPDOUBLE lpRate )
//************************************************************************
{ // Compute a point on passed vector that is at the shortest distance to the passed point
	if ( !lpBeg || !lpEnd || !lpIn || !lpOut || !lpRate )
		return( NO );

	long XI, YI;
	BOOL bRet = PointMappedToLine( lpBeg->x, lpBeg->y, lpEnd->x, lpEnd->y,
		lpIn->x, lpIn->y, &XI, &YI, NULL, lpRate );
	if ( bRet )
	{
		lpOut->x = XI;
		lpOut->y = YI;
	}

	return( bRet );
}

/***********************************************************************/
void CGame::NewDirection( HWND hWnd, int iEvent, char cSpecial )
/***********************************************************************/
{ // Inform the radar that we are changing directions
	if ( !iEvent )
		return;

	switch ( iEvent )
	{
		case EVENT_LEFT:
		{
			lpWorld->GetPlayer()->TurnLeft(cSpecial);
			//Weapons.CurWeapon()->AdjustLeft();
			SetupHUD(hWnd, YES/*bDisplay*/);
			lpWorld->UpdateView();
			break;	
		}		
		case EVENT_RIGHT:
		{
			lpWorld->GetPlayer()->TurnRight(cSpecial);
			//Weapons.CurWeapon()->AdjustRight();
			SetupHUD(hWnd, YES/*bDisplay*/);
			lpWorld->UpdateView();
			break;
		}      
		case EVENT_DOWN:
		{
			lpWorld->GetPlayer()->TurnBack();
			//Weapons.CurWeapon()->AdjustReverse();
			SetupHUD(hWnd, YES/*bDisplay*/);
			lpWorld->UpdateView();
			break;
		}      
		default:
			return;
	}
}

/***********************************************************************/
void CGame::SetupHUD(HWND hWnd, BOOL bDisplay)
/***********************************************************************/
{
	CCell* pCell = lpWorld->GetPlayer()->GetCell();
	if ( !pCell )
		return;

	int iDir = lpWorld->GetPlayer()->GetDir();

	// Handle collectible cul-de-sacs
	//PassThruPortal( hWnd, pCell, iDir );

	char cHud2[3];
	// If direction is odd
	if (iDir & 1)
	{
		cHud2[0] = pCell->OppDir(iDir-1);
		cHud2[1] = pCell->OppDir(iDir);
		cHud2[2] = pCell->OppDir(iDir-1);
	}
	else
	{
		cHud2[0] = pCell->OppDir(iDir-2);
		cHud2[1] = pCell->OppDir(iDir);
		cHud2[2] = pCell->OppDir(iDir+2 );
	}

	int iLevel = lpWorld->GetLevel();
	for ( int i=0; i<3; i++ )
	{
		ITEMID idIcon2;
		if ( cHud2[i] >= '1' && cHud2[i] <= '9' )
			idIcon2 = (IDC_LEVEL_BASE + ((iLevel - 1) * 100) + (cHud2[i] - '0'));
		else
			idIcon2 = NULL;
		SendDlgItemMessage( hWnd, IDC_HUDOPP1+i, SM_SETICONID, idIcon2, !bDisplay );
	}
}

/***********************************************************************/
BOOL CGame::PassThruPortal( HWND hWnd, CCell* pCell, int iDir )
/***********************************************************************/
{ // return whether or not the item was removed from the portal

	if (!pCell)
		return( NO );

	char cTest = pCell->PortalDir(iDir);

	// Check for collectibles
	if (cTest >= '1' && cTest <= '9' )
	{
		int iCollectible = cTest - '1';
		Collect.AddItem(hWnd, lpWorld->GetLevel(), iCollectible);
		//pCell->SetPortalDir(iDir, 'M');
		ZapCurrentPortal();
		return( YES ); // the item is removed
	}	

	// Check for energy
	if (cTest == 'E')
	{
		//if (App.bOptionTaunts)
		//	AmbSnds.Play(Energy2Snd);

		Weapons.CurWeapon()->ClearBulletCracks();
		lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy += (METER_MAX/10) );
		if (App.bEnergyCheat && lEnergy <= 1) lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy = METER_MAX );
		SetHealthDigitDisplay(hWnd, lEnergy);
		ZapCurrentPortal();
		//pCell->SetPortalDir(iDir, 'M');
		STRING szText;
		wsprintf(szText, "%d energy!", 	((lEnergy + 500) / 100) - 1);
		pVidFx->TextOn(hWnd, szText, RGB(255,0,0), 18, 3000, 3);
		return( YES ); // the item is removed
	}
	if (cTest == 'e')
	{
		//if (App.bOptionTaunts)
		//	AmbSnds.Play(Energy1Snd);

		Weapons.CurWeapon()->ClearBulletCracks();
		lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy += (METER_MAX/20) );
		if (App.bEnergyCheat && lEnergy <= 1) lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy = METER_MAX );
		SetHealthDigitDisplay(hWnd, lEnergy);
		ZapCurrentPortal();
		//pCell->SetPortalDir(iDir, 'M');
		STRING szText;
		wsprintf(szText, "%d energy!", 	((lEnergy + 500) / 100) - 1);
		pVidFx->TextOn(hWnd, szText, RGB(255,0,0), 18, 3000, 3);
		return( YES ); // the item is removed
	}

	return( NO ); // the item is NOT removed
}

/***********************************************************************/
BOOL CGame::SetAmmo(HWND hWnd, char cCode)
/***********************************************************************/
{
	int nAmmo = -1;
	int nSetWeapon = -1;
	int nMaxAmmo = 999;
	BOOL bRet = FALSE;

	switch(cCode)
	{

	// machine gun
	case 'A':
		nSetWeapon = 2;
		nAmmo = 100;
		break;

	// rockets
	case 'R':
		nSetWeapon = 3;
		nAmmo = 20;
		break;

	// ion cannon
	case 'k':
		nSetWeapon = 4;
		nAmmo = -1;
		break;

	// mines
	case 'h':
		nSetWeapon = 5;
		nAmmo = 20;
		break;

	// jacks
	case 'j':
		nSetWeapon = 6;
		nAmmo = 20;
		break;

	// leroi
	case 'm':
		nSetWeapon = 7;
		nAmmo = 1;
		break;

	// spine killer
	case 'S':
		nSetWeapon = 8;
		nAmmo = 1;
		Levels.SetState(LEVENT_GOT_SK, 0);
		break;

	// thumper
	case 'n':
		nSetWeapon = 9;
		nAmmo = 1;
		break;

	// dog rockets
	case 'i':
		nSetWeapon = 12;
		nAmmo = 20;
		break;

	// spreadfire
	case 'l':
		nSetWeapon = 13;
		nAmmo = 20;
		break;
	}

	// If a weapon is to be set
	if (nSetWeapon > -1)
	{
		Weapons.AddAmmo(nSetWeapon-1, nAmmo);
		if ( Weapons.GetAmmo(nSetWeapon) > nMaxAmmo) 
			Weapons.SetAmmo(nSetWeapon-1, nMaxAmmo);

		// Change weapons
		if (Weapons.GetCurWeapon() < (nSetWeapon-1))
			PostMessage( hWnd, WM_COMMAND, '0' + nSetWeapon, 0L );

		bRet = TRUE;
	}

	return bRet;
}

/***********************************************************************/
void CGame::SetWeaponDisplay(HWND hWnd, int iWeapon)
/***********************************************************************/
{
	if (iWeapon == 1)
		SetWeaponDigitDisplay(hWnd, -1);
	else
	if (iWeapon == 4)
		SetWeaponDigitDisplay(hWnd, -1);
	else
	if (iWeapon == 2)
		SetWeaponDigitDisplay(hWnd, Weapons.GetAmmo(iWeapon-1)/ROUNDS_PER_CLIP);
	else
		SetWeaponDigitDisplay(hWnd, Weapons.GetAmmo(iWeapon-1));

	SendDlgItemMessage( hWnd, IDC_WEAPON, SM_SETICONID, IDC_WEAPON + iWeapon, 0L );
	SendDlgItemMessage( hWnd, IDC_AMMOTYPE, SM_SETICONID, IDC_AMMOTYPE + iWeapon, 0L );
}

/***********************************************************************/
void CGame::SetWeaponDigitDisplay(HWND hWnd, int iValue)
/***********************************************************************/
{
	// Digit display is three digits between 0 and 999
	// Set to - 1 to set all digits blank
	if (iValue < -1)
		iValue = -1;	// Value out of range

	if (iValue > 999)
		iValue = 999;	// Value out of range

	ITEMID idHuns = IDC_NODIGIT;
	ITEMID idTens = IDC_NODIGIT;
	ITEMID idOnes = IDC_NODIGIT;

	int i;
	BOOL bGotDigit = NO;
	if (iValue >= 0)
	{
		if ( (i = (iValue / 100)) )
		{
			idHuns = IDC_DIGIT0 + i;
			iValue -= (i * 100);
			bGotDigit = YES;
		}
		if ( (i = (iValue / 10)) || bGotDigit )
		{
			idTens = IDC_DIGIT0 + i;
			iValue -= (i * 10);
		}
		idOnes = IDC_DIGIT0 + iValue;
	}

	SendDlgItemMessage( hWnd, IDC_AMMODIGITA, SM_SETICONID, idHuns, 0L );
	SendDlgItemMessage( hWnd, IDC_AMMODIGITB, SM_SETICONID, idTens, 0L );
	SendDlgItemMessage( hWnd, IDC_AMMODIGITC, SM_SETICONID, idOnes, 0L );
}

/***********************************************************************/
void CGame::SetKillDisplay(HWND hWnd, int iValue, BOOL bForcePaint)
/***********************************************************************/
{
	if (iValue < -1)
		iValue = 0;		// Value out of range

	if (iValue > 499)
		iValue = 499;	// Value out of range

	int i = iValue;
	int iHuns = (i / 100);	i -= (iHuns * 100);
	int iTens = (i / 10); 	i -= (iTens * 10);
	int iOnes = i;

	// Do the hundreds digit
	for (i=1; i<=9; i++)
	{
		ITEMID id = IDC_KILL100 + i - 1;
		HWND hControl = GetDlgItem(hWnd, id);
		if (!hControl)
			continue;
		BOOL bOn = IsWindowVisible(hControl);
		if ( (bOn || bForcePaint) && (i > iHuns) ) // if it's ON but it shouldn't be
			Bitmap_Show(hControl, NO/*bShow*/);
		else
		if ( (!bOn || bForcePaint) && (i <= iHuns) ) // if it's NOT ON but it should be
			Bitmap_Show(hControl, YES/*bShow*/);
	}

	// Do the tens digit
	for (i=1; i<=9; i++)
	{
		ITEMID id = IDC_KILL10 + i - 1;
		HWND hControl = GetDlgItem(hWnd, id);
		if (!hControl)
			continue;
		BOOL bOn = IsWindowVisible(hControl);
		if ( (bOn || bForcePaint) && (i > iTens) ) // if it's ON but it shouldn't be
			Bitmap_Show(hControl, NO/*bShow*/);
		else
		if ( (!bOn || bForcePaint) && (i <= iTens) ) // if it's NOT ON but it should be
			Bitmap_Show(hControl, YES/*bShow*/);
	}

	// Do the ones digit
	for (i=1; i<=9; i++)
	{
		ITEMID id = IDC_KILL1 + i - 1;
		HWND hControl = GetDlgItem(hWnd, id);
		if (!hControl)
			continue;
		BOOL bOn = IsWindowVisible(hControl);
		if ( (bOn || bForcePaint) && (i > iOnes) ) // if it's ON but it shouldn't be
			Bitmap_Show(hControl, NO/*bShow*/);
		else
		if ( (!bOn || bForcePaint) && (i <= iOnes) ) // if it's NOT ON but it should be
			Bitmap_Show(hControl, YES/*bShow*/);
	}
}

/***********************************************************************/
void CGame::SetHealthDigitDisplay(HWND hWnd, long lValue)
/***********************************************************************/
{
	// energy value is 0-100,000 based - convert to 0-999
	int iValue = ((lValue + 500) / 100) - 1;

	// Digit display is three digits between 0 and 999
	// Set to - 1 to set all digits blank
	if (iValue < -1)
		iValue = -1;	// Value out of range

	if (iValue > 999)
		iValue = 999;	// Value out of range

	ITEMID idHuns = IDC_NODIGIT;
	ITEMID idTens = IDC_NODIGIT;
	ITEMID idOnes = IDC_NODIGIT;

	int i;
	BOOL bGotDigit = NO;
	if (iValue >= 0)
	{
		if ( (i = (iValue / 100)) )
		{
			idHuns = IDC_DIGIT0 + i;
			iValue -= (i * 100);
			bGotDigit = YES;
		}
		if ( (i = (iValue / 10)) || bGotDigit )
		{
			idTens = IDC_DIGIT0 + i;
			iValue -= (i * 10);
		}
		idOnes = IDC_DIGIT0 + iValue;
	}

	SendDlgItemMessage( hWnd, IDC_ENERGYDIGITA, SM_SETICONID, idHuns, 0L );
	SendDlgItemMessage( hWnd, IDC_ENERGYDIGITB, SM_SETICONID, idTens, 0L );
	SendDlgItemMessage( hWnd, IDC_ENERGYDIGITC, SM_SETICONID, idOnes, 0L );
}

/***********************************************************************/
void CGame::LoadCustomLevel(HWND hWnd)
/***********************************************************************/
{
	//STRING szFilter;
	STRING szFileName;

	/***
	lstrcpy( szFilter, FileName(lpFileName) );
	lstrcat( szFilter, " (*" );
	lstrcat( szFilter, Extension(lpFileName) );
	lstrcat( szFilter, ")|*" );
	lstrcat( szFilter, Extension(lpFileName) );
	lstrcat( szFilter, "|All Files (*.*)|*.*|" );
	int iLength = lstrlen( szFilter );

	char  chReplace;
	chReplace = szFilter[iLength - 1]; // retrieve wildcard character
	for ( int i = 0; szFilter[i] != '\0'; i++ )
	{
		if (szFilter[i] == chReplace)
		   szFilter[i] = '\0';
	}

    ***/

	OPENFILENAME ofn;
	_fmemset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetFocus();
	//ofn.lpstrFilter = szFilter;
	//ofn.nFilterIndex = 1;
	szFileName[0] = NULL;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	//ofn.lpstrInitialDir = lpPathName;
	ofn.lpstrTitle = "Load Custom Map File"; // title of the dialog - "Open" by default
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (!GetOpenFileName(&ofn))
		return;

	// Use level 2 logic
	LoadLevelFile(hWnd, szFileName, 2/*iLevel*/);
}

//************************************************************************
BOOL CGame::LoadLevelFile(HWND hWnd, LPSTR lpFileName, int iLevel)
//************************************************************************
{
	FNAME szExpFileName;
	if ( !GetApp()->FindContent( lpFileName, szExpFileName ) )
		return( FALSE );

	ifstream in;
	in.open(szExpFileName);
	if (in.bad())
		return( FALSE );

	lpWorld->LoadLevel( in, iLevel );

	in.close();

	Levels.SetLevel(iLevel);
	Collect.ReInit(hWnd);
	lpWorld->GetPlayer()->SetNewShotCell();

	InvalidateRect(hRadar, NULL, TRUE); 
	return( TRUE );
}

/***********************************************************************/
void CGame::InitWeapons(HWND hWnd)
/***********************************************************************/
{
	Weapons.Init(hWnd);

	Weapons.SetCurWeapon(0); // Set scrap gun

	Weapons.CurWeapon()->SetShowGrid(App.bShowGrid);

	// Call OnVideoOpen() since the video is already open and the
	// weapons system has not had a chance to get this message yet.
	
	Weapons.CurWeapon()->OnVideoOpen();
	Weapons.CurWeapon()->Enable(FALSE);
}

//************************************************************************
void CGame::FireWeapon(HWND hWnd, WORD wFlags)
//************************************************************************
{
	// Exit before firing if no ammo

	int iCurrentWeapon = Weapons.GetCurWeapon() + 1;

	// Check laser
	if ( iCurrentWeapon == 4)
	{
		if (!lEnergy)
			return;
	}
	else

	if ( iCurrentWeapon >= 2 && iCurrentWeapon <= 8)
	{			
		if (!Weapons.GetAmmo(iCurrentWeapon-1))
			return;
	}	

	// Fire!!!
	BOOL bFired = ( wFlags & KF_REPEAT ? Weapons.CurWeapon()->Repeat() : Weapons.CurWeapon()->Start() );
	if ( !bFired )
		return;

	// Decrement appropriate weapon stores
	if ( iCurrentWeapon == 4)
	{
		lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= METER_MAX/100 );
		if (App.bEnergyCheat && lEnergy <= 1) lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy = METER_MAX );
		SetHealthDigitDisplay(hWnd, lEnergy);
	}
	else if (iCurrentWeapon >= 2 && iCurrentWeapon <= 8)
	{
		Weapons.DecAmmo(iCurrentWeapon-1, 1);

		int iAmmo = Weapons.GetAmmo(iCurrentWeapon-1);
		if (iCurrentWeapon == 2)
			iAmmo /= ROUNDS_PER_CLIP;

		SetWeaponDigitDisplay(hWnd, iAmmo);

		if (App.bEnergyCheat)
		{
			int iMax = 50;
			if ( iCurrentWeapon == 2)
				iMax = AMMO_MGUN;
			else if (iCurrentWeapon == 3)
				iMax = AMMO_MISSILES;
			else if (iCurrentWeapon == 4)
				iMax = AMMO_MINES;
			else if (iCurrentWeapon == 5)
				iMax = AMMO_BOMBS;
			else if (iCurrentWeapon == 6)
				iMax = AMMO_JACKS;
			else if (iCurrentWeapon == 7)
				iMax = AMMO_LEROYS;
			else if (iCurrentWeapon == 8)
				iMax = AMMO_THUMPER;
			Weapons.SetAmmo(iCurrentWeapon-1, iMax);
		}
	}
}

//************************************************************************
void CGame::BlowupRadar( BOOL bBlowup )
//************************************************************************
{
	HWND hWnd = GetParent(hRadar); 
	RECT Rect;
	GetClientRect(hWnd, &Rect);

	LPSCENE lpScene = CScene::GetScene(hWnd);
	if (!lpScene)
		return;

	LPOFFSCREEN lpOffScreen = lpScene->GetOffScreen();
	if (!lpOffScreen)
		return;

	HWND hCtrl;

	if (bBlowup)
	{
		// Save the radar coordinates for future manipulation
		GetWindowRect(hRadar, &RadarRect);
		ScreenToClient(hWnd, (LPPOINT)&RadarRect.left);
		ScreenToClient(hWnd, (LPPOINT)&RadarRect.right);

		if (lpVideo->hDevice)
			MCIPause(lpVideo->hDevice);
		else
		if (lpVideo->pAMovie)
			lpVideo->pAMovie->Pause();

		// Hide the controls
		hCtrl = GetDlgItem( hWnd, IDC_ENERGY );
		ShowWindow(hCtrl, SW_HIDE);
		hCtrl = GetDlgItem( hWnd, IDC_VIDEO );
		ShowWindow(hCtrl, SW_HIDE);
		hCtrl = GetDlgItem( hWnd, IDC_ENERGYDIGITA );
		ShowWindow(hCtrl, SW_HIDE);
		hCtrl = GetDlgItem( hWnd, IDC_ENERGYDIGITB );
		ShowWindow(hCtrl, SW_HIDE);
		hCtrl = GetDlgItem( hWnd, IDC_ENERGYDIGITC );
		ShowWindow(hCtrl, SW_HIDE);

		// Clear the offscreen with black
		HDC hDC = lpOffScreen->GetDC();
		if (hDC)
			FillRect( hDC, &Rect, (HBRUSH)GetStockObject(BLACK_BRUSH) );

		//SetWindowPos(hRadar, HWND_TOPMOST, Rect.left, Rect.top, 
		//	Rect.right - Rect.left, Rect.bottom - Rect.top, 0);
		MoveWindow(hRadar, Rect.left, Rect.top, 
			Rect.right - Rect.left,
			Rect.bottom - Rect.top, FALSE);

		InvalidateRect( hRadar, NULL, FALSE );
		UpdateWindow( hRadar );
	}
	else
	{
		// Restore the offscreen
		lpOffScreen->CopyBits( &Rect );

		//SetWindowPos(hRadar, HWND_NOTOPMOST, RadarRect.left, RadarRect.top, 
		//	RadarRect.right - RadarRect.left, RadarRect.bottom - RadarRect.top,
		//	0);
		MoveWindow(hRadar, RadarRect.left, RadarRect.top, 
			RadarRect.right - RadarRect.left,
			RadarRect.bottom - RadarRect.top, TRUE);

		// Show the controls
		hCtrl = GetDlgItem( hWnd, IDC_ENERGY );
		ShowWindow(hCtrl, SW_SHOW);
		hCtrl = GetDlgItem( hWnd, IDC_VIDEO );
		ShowWindow(hCtrl, SW_SHOW);
		hCtrl = GetDlgItem( hWnd, IDC_ENERGYDIGITA );
		ShowWindow(hCtrl, SW_SHOW);
		hCtrl = GetDlgItem( hWnd, IDC_ENERGYDIGITB );
		ShowWindow(hCtrl, SW_SHOW);
		hCtrl = GetDlgItem( hWnd, IDC_ENERGYDIGITC );
		ShowWindow(hCtrl, SW_SHOW);

		UpdateWindow( hWnd );

		if (lpVideo->hDevice)
			MCIResume(lpVideo->hDevice);
		else
		if (lpVideo->pAMovie)
			lpVideo->pAMovie->Resume();
	}
}

//************************************************************************
void CGame::HitHellion(void)
//************************************************************************
{
	STRING szText;

	if (!Levels.HaveFireExtinguisher())
	{
		bDieing = TRUE;
		wsprintf(szText, "You hit a Hellion and have no fire extinguisher!");
	}
	else
	{
		wsprintf(szText, "You hit a Hellion, using fire extinguisher...");
	}

	HWND hWnd = GetParent(hRadar); 
	pVidFx->TextOn(hWnd, szText, RGB(255,0,0), 18, 4000, 4);
}

//************************************************************************
void CGame::HitThwart(void)
//************************************************************************
{
	Levels.SetState(LEVENT_LOSE_ALL, 0);

	HWND hWnd = GetParent(hRadar); 
	if (pVidFx)
	{
		if (Levels.GetState() > 0)
			pVidFx->TextOn(hWnd,"Thwart took away your collectibles!", 
				RGB(255, 0, 0), 18, 5000, 5);
	}
}

//************************************************************************
void CGame::HitViralion(void)
//************************************************************************
{
	if (lpWorld)
	{
		lpWorld->SetVirus(YES);

		m_dwVirusStart = timeGetTime();
		m_lVirusDur = 50000;

		HWND hWnd = GetParent(hRadar);
		if (pVidFx)
			pVidFx->TextOn(hWnd, "Viralion infecting radar system!",
				RGB(255, 0, 0), 18, 4000, 5);

	}
}

//************************************************************************
void CGame::HitMedicator(void)
//************************************************************************
{
	if (lpWorld)
	{
		HWND hWnd = GetParent(hRadar);
		if (lpWorld->HaveVirus())
		{
			lpWorld->SetVirus(NO);
			m_lVirusDur = 0;
			if (pVidFx)
				pVidFx->TextOn(hWnd, "Medicator removing virus...",
					RGB(255, 255, 0), 18, 4000, 5);
		}
		else
		{
			lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy += (METER_MAX/10) );
			SetHealthDigitDisplay(hWnd, lEnergy);
			if (pVidFx)
			{
				STRING szText;
				wsprintf(szText,  "Medicator gives you %d energy.", (METER_MAX/10));
				pVidFx->TextOn(hWnd, szText, RGB(255, 255, 0), 18, 4000, 5);
			}
		}
	}

	// Other nice things the medicator does
	Weapons.CurWeapon()->ClearBulletCracks();

}

//************************************************************************
void CGame::HitDraniac(void)
//************************************************************************
{
	HWND hWnd = GetParent(hRadar);
	lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/10) );
	SetHealthDigitDisplay(hWnd, lEnergy);
	if (pVidFx)
	{
		STRING szText;
		wsprintf(szText,  "Draniac stole %d energy!", (METER_MAX/10));
		pVidFx->TextOn(hWnd, szText, RGB(255, 0, 0), 18, 4000, 5);
	}
}

//************************************************************************
void CGame::HitKiller(void)
//************************************************************************
{
	HWND hWnd = GetParent(hRadar);
	lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy = 5000 );
	SetHealthDigitDisplay(hWnd, lEnergy);
	if (pVidFx)
	{
		STRING szText;
		wsprintf(szText,  "You hit a killer! You are near death!", 5000);
		pVidFx->TextOn(hWnd, szText, RGB(255, 0, 0), 18, 4000, 5);
	}
}

//************************************************************************
void CGame::HitPsychotech(void)
//************************************************************************
{
	// Knock player into random cell

	// get player
	CPlayer *pPlayer = lpWorld->GetPlayer();

	// look for a place to put the player (random)

	// Get cells and count of cells
	CCell **pCells = App.pGame->lpWorld->GetAllCells();
	int nCount = App.pGame->lpWorld->GetCellMapWidth() *
		App.pGame->lpWorld->GetCellMapHeight();

	int nPlace;
	CCell *pCell;
	BOOL bFound = FALSE;

	// look for a place to put it
	while(!bFound)
	{
		nPlace = GetRandomNumber(nCount);
		pCell = pCells[nPlace];
		if ((pCell->Type() == 'H' || pCell->Type() == 'U') &&
			 pCell->IsVisible())
			bFound = TRUE;
	} // end while cell not found

	// Set the new player cell
	pPlayer->SetCell(pCell);
	pPlayer->SetXYCell(pCell->Getx(), pCell->Gety());

	// Set the player in the opposite direction
	pPlayer->TurnBack();

	// Generate end event, which causes a new cell video to play
	//PostMessage( hVideo, WM_KEYDOWN, VK_SPACE, 0 );
	LPSHOT lpShot = Video_FindShotPrefix(hVideo, "/Cell");
	PostMessage( hVideo, WM_VIDEO_GOTOSHOT, 0, (LPARAM)lpShot);

	HWND hWnd = GetParent(hRadar);
	if (pVidFx)
	{
		STRING szText;
		wsprintf(szText,  "Psychotech teleported you!", 5000);
		pVidFx->TextOn(hWnd, szText, RGB(255, 0, 0), 18, 4000, 5);
	}

	Weapons.CurWeapon()->StopSounds();
}

//************************************************************************
void CGame::HitSlamatron(void)
//************************************************************************
{
	// Knock player back a cell

	// get player
	CPlayer *pPlayer = lpWorld->GetPlayer();

	// get cell that player is in
	//CCell *pCell = pPlayer->GetCell();

	// get player direction
	//int iDir = lpWorld->GetPlayer()->GetDir();

	// reverse direction
	//iDir -= 6;
	//if (iDir < 1)
	//	iDir += 12;

	// get the next cell in that direction
	//POINT pnt = pPlayer->NextCellFromDir(pCell->Getx(), pCell->Gety(), iDir);
	//CCell *pNextCell = lpWorld->GetCell(pnt.x, pnt.y);

	// if cell is invalid, then use current cell
	//if (pNextCell == NULL)
	//	pNextCell = pCell;

	// Set the new player cell
	//pPlayer->SetCell(pNextCell);

	pPlayer->TurnBack();

	// Generate end event, which causes a new cell video to play
	//PostMessage( hVideo, WM_KEYDOWN, VK_SPACE, 0 );
	LPSHOT lpShot = Video_FindShotPrefix(hVideo, "/Cell");
	PostMessage( hVideo, WM_VIDEO_GOTOSHOT, 0, (LPARAM)lpShot);

	HWND hWnd = GetParent(hRadar);
	if (pVidFx)
	{
		STRING szText;
		wsprintf(szText,  "You have been slammed!", 5000);
		pVidFx->TextOn(hWnd, szText, RGB(255, 0, 0), 18, 4000, 5);
	}

	Weapons.CurWeapon()->StopSounds();
}

//************************************************************************
BOOL CALLBACK TurnEventCallback( HWND hWindow, LPVIDEO lpVideo, int iEventCode )
//************************************************************************
{
	// If effects are turned off then return
	if (!App.bOptionTransitions)
		return(NO);

	// If turn left or right check for sight key being
	// depressed
	//if (iEventCode == EVENT_LEFT || iEventCode == EVENT_RIGHT)
	//{
	//	short nState = GetKeyState(VK_CONTROL);

		// If control key is depressed we are moving
		// the weapon sight left
	//	if ( nState & 0x80000000 )
	//	{
	//		if (iEventCode == EVENT_LEFT)
	//			Weapons.CurWeapon()->MoveSightLeft();
	//		else
	//			Weapons.CurWeapon()->MoveSightRight();
    //
	//		return TRUE;
	//	}

	//}

	// If ctrl key was depressed
	//static BOOL bWasDepressed = FALSE;
	//if (bWasDepressed)
	//{
		// Reset the sight speed to normal
	//	Weapons.CurWeapon()->ResetSightSpeed();
	//	bWasDepressed = FALSE;
	//}

	return App.pGame->Turn( hWindow, iEventCode);
}

//************************************************************************
void OnVideoRButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	// Turn around?
	if (y >= 340)
		App.pGame->Turn(hWindow, EVENT_DOWN);

	// Turn left?
	else if (x < 320 && y < 340)
		App.pGame->Turn(hWindow, EVENT_LEFT);

	// Turn right?
	else if (x >= 320 && y < 340)
		App.pGame->Turn(hWindow, EVENT_RIGHT);
}

//************************************************************************
void OnVideoLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	WORD wFlags;
	int iCur = App.pGame->Weapons.GetCurWeapon();
	// if scrap gun or maching gun then make continious fire
	if (iCur == 0 || iCur == 2)
		wFlags = KF_REPEAT;
	else
		wFlags = 0;

	// Fire!
	App.pGame->Weapons.CurWeapon()->MoveSight(x);
	App.pGame->FireWeapon(GetParent(hWindow), KF_REPEAT); 
}

//************************************************************************
void OnVideoLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
//************************************************************************
{
	// stop firing
	App.pGame->Weapons.CurWeapon()->StopFire();
}

//************************************************************************
void CGame::ShowLevel6Clue(HWND hWnd)
//************************************************************************
{
	if (pVidFx)
	{
		CLevel *pLevel = Levels.GetLevelPtr();
		if (pLevel)
		{
			char *pszDstLets = "-- --- ---------";

			int nCollFlags = pLevel->GetUseData();

			// ok, figure out letters
			if (nCollFlags & 1)
			{
				pszDstLets[0] = 'I';
				pszDstLets[10] = 'I';
				pszDstLets[13] = 'I';
			}
			if (nCollFlags & (1 << 1))
			{
				pszDstLets[1] = 'N';
				pszDstLets[11] = 'N';
				pszDstLets[12] = 'N';
				pszDstLets[14] = 'N';
			}
			if (nCollFlags & (1 << 2))
				pszDstLets[3] = 'T';
			if (nCollFlags & (1 << 3))
				pszDstLets[4] = 'H';
			if (nCollFlags & (1 << 4))
			{
				pszDstLets[5] = 'E';
				pszDstLets[8] = 'E';
			}
			if (nCollFlags & (1 << 5))
				pszDstLets[7] = 'B';
			if (nCollFlags & (1 << 6))
			{
				pszDstLets[9] = 'G';
				pszDstLets[15] = 'G';
			}

			pVidFx->TextOn(hWnd, pszDstLets);
			m_bScoreUp = TRUE;
		}
	}
}

//************************************************************************
void CGame::ZapCurrentPortal(void)
//************************************************************************
{
	// Zaps the current portal code. The current portal is the portal
	// behind the player after the player has entered a cell.

	CPlayer *pPlayer = lpWorld->GetPlayer();
	if (!pPlayer)
		return;

	CCell* pCell = pPlayer->GetCell();
	if (!pCell)
		return;

	// get portal behind player
	int iOrigDir = pPlayer->GetDir();
	int iRevDir = iOrigDir;

	// reverse the direction
	REVERSE_DIR(iRevDir);

	// zap the portal
	pCell->ZapPortal(iRevDir);

	// Get the adjacent cell
	POINT pnt = pPlayer->NextCellFromDir(pCell->Getx(), pCell->Gety(), iRevDir);
	pCell = lpWorld->GetCell(pnt.x, pnt.y);

	// have to clear it from the adjacent cell too, since they share
	// the portal
	if (pCell)
		pCell->ZapPortal(iOrigDir);
}
