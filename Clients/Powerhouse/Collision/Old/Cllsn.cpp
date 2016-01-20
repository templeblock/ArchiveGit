#include <windows.h>
#define MAIN
#include "proto.h"
#include "control.h"
#include "commonid.h"
#include "cllsn.h"
#include "weapon.h"
#include "options.h"
#include "worlds.h"
#include "credits.h"
#include "rcontrol.p"
#include "infpanel.h" 
#include "vworld.h"
#include "videofx.h"
#include "vsndmix.h"

// INI file stuff
#define INI_FILE			"CLLSN.INI"
#define OPTIONS_SECTION		"Options"

#define ROUNDS_PER_CLIP		50
#define AMMO_MGUN			2500
#define AMMO_ROCKETS		250
#define AMMO_MINES			250

#define VIDEO_TIMER			0xBABE

// Globals
BOOL m_bOptionsfromGame;
BOOL bOption1 = TRUE;	// Auto - Targeting
BOOL bOption2 = TRUE;	// HUD Scrolling
BOOL bOption3 = TRUE;	// Animations
BOOL bOption4 = FALSE;
BOOL bOption5 = FALSE;
BOOL bOption6 = FALSE;
BOOL bOption7 = FALSE;
BOOL bOption8 = FALSE;

// video world
VideoWorld VWorld;

// generic video soundmix object
VSndMix MusicOne;
//VSndMix MusicTwo;

// Locals
static SHOTID m_lSaveShot;
static long lEnergy, lHealthSpine;
static LPSHOT lpLastShot;
static HWND hVideo;
static HWND hRadar;
static LPVIDEO lpVideo;
static LPWORLD lpWorld;
static LPCOLLINFO lpCollect;
static CWeapon *pWeapon;
static CRocketLauncher RocketLauncher;
static CLaserBomb LaserBomb;
static CMachineGun MGun;
static bShowGrid;
static int nMusicState = 0;

//Barricade crashed flag
static BOOL bBarricadeBroken = FALSE;

// Weapon load counting
static int iCurrentWeapon;
static int iAmmoRounds;
static int iRockets;
static int iMines;
static int iSpineKillers;

// Kill tracking
static int iKills;

// Local Animation Tracking Variables
static BOOL bInProgress;
static int iCurrentFrame;

//Animation testing
static BOOL bFiring;
static int iFireFrame;

static BOOL bLoopFrame = FALSE;
static int iLoopFrame = 0;	

#define LAST_FIRE_FRAME		3

// HUD Scrolling mechanism
static BOOL bScrolling;
static int iScrollDir;
static int iTotalScrolled;

#define LAST_MINEFRAME		11
#define LAST_MGUNFRAME		15	
#define LAST_LASERFRAME		16
#define LAST_ROCKETFRAME	15

static WORD wLevelFlags = 0x0;
static WORD wCollectibles = 0x0;

// Save/Resume scene stuff
char m_szGame1Title[TITLE_SIZE + 1];
char m_szGame2Title[TITLE_SIZE + 1];
char m_szGame3Title[TITLE_SIZE + 1];
char m_szGame4Title[TITLE_SIZE + 1];
char m_szGame5Title[TITLE_SIZE + 1];
STRING m_szGame1FilePath;
STRING m_szGame2FilePath;
STRING m_szGame3FilePath;
STRING m_szGame4FilePath;
STRING m_szGame5FilePath;

// Local function prototypes
static void InitWeapons(HWND hWnd);
static LPSHOT GetNewCellShot(HWND hWnd, HWND hVideo );
static void NewDirection(HWND hWnd, int iEvent );
static void SetupHUD(HWND hWnd, int iScroll);
static BOOL AnimateWeaponLoad(HWND hWnd);
static BOOL AnimateWeaponFire(HWND hWnd);
static BOOL AnimateHUDScroll(HWND hWnd);
static void SetAmmoDigitDisplay(HWND hWnd, int iValue);
static void SetKillDigitDisplay(HWND hWnd, int iValue);
static void SetHealthDigitDisplay(HWND hWnd, long lValue);
static BOOL ManageGateway(HWND hWnd);
static BOOL ManageEntry(HWND hWnd);
static BOOL AnimateLoop(HWND hWnd);

// The Resume scene
void OnResumeSceneCommand( HWND hWnd, int id, HWND hControl, UINT codeNotify );
BOOL OnResumeSceneInit( HWND hWnd, HWND hWndControl, LPARAM lParam );
void ResumeGame( HWND hWnd, int iGameNumber );
void SaveGame( HWND hWnd, int iGameNumber );
void SaveGameTitles( HWND hWnd );

// The save game scene
BOOL OnSaveGameSceneInit( HWND hWnd, HWND hWndControl, LPARAM lParam );
void OnSaveGameSceneCommand( HWND hWnd, int id, HWND hControl, UINT codeNotify );

//************************************************************************
BOOL CALLBACK CollisionProcessEvent( HWND hWindow, LPVIDEO lpVideo, int iEventCode )
//************************************************************************
{
	static LPVIDEOFX pVideoFx;

	if ( !pVideoFx && !(pVideoFx = new VideoFx()) )
		return( NO );

	return( pVideoFx->DoFx( hWindow, iEventCode ) );
}

//************************************************************************
BOOL PlaySceneInit( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{ // Entering the game's main play scene

	hVideo = GetDlgItem( hWnd, IDC_VIDEO );
	lpVideo = (LPVIDEO)GetWindowLong( hVideo, GWL_DATAPTR );
	hRadar = GetDlgItem( hWnd, IDC_RADAR );
	lpWorld = (LPWORLD)GetWindowLong( hRadar, GWL_DATAPTR );

	// Install the event proc for turn effects
	if ( lpVideo )
		lpVideo->lpEventProc = CollisionProcessEvent;

	MusicOne.Reset();

	VideoSetTimer( hWnd, VIDEO_TIMER );
   
	// Initialize weapons system
	InitWeapons(hWnd);

	MusicOne.Load(IDC_MUSIC1);
	//MusicTwo.Load(IDC_MUSIC2);

	ShowWindow(GetDlgItem(hWnd, IDC_MSL_LOOP1), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_MNE_LOOP1), SW_HIDE);

	// Initialize key static data
	iCurrentWeapon = 0;
	iAmmoRounds = AMMO_MGUN;
	iRockets = AMMO_ROCKETS;
	iMines = AMMO_MINES;
	iSpineKillers = 0;
	iKills = 0;

	// Temporary - will be moved
	SendDlgItemMessage( hWnd, IDC_KILLED, SM_SETICONID, IDC_CANNED,0L );
	RedrawWindow(GetDlgItem(hWnd, IDC_WEAPON), NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
	//Temoporary

	// Initilialize Collectibles Data Structure
	lpCollect = new CCollInfo;
	int iLevel = lpWorld->GetLevel();
	lpCollect->SetBitmapIDs(iLevel);

	// Call OnVideoOpen() since the video is already open and the weapons system has not had a chance to 
	// get this message yet.
	pWeapon->OnVideoOpen();

	//TEMPORARY! TEST FOR 8-bit display
	HDC hDC = GetDC(hWnd);
	int iRes = GetDeviceCaps(hDC, NUMCOLORS);
	if (iRes != 20)
	{
		MessageBox(hWnd, "NOTE: You must be running in 8-bit, 256 color mode for the weapons to work properly. (TEMPORARY)", "Note", 0);
	}
	ReleaseDC(hWnd, hDC);

	SendMessage( hWnd, WM_COMMAND, IDC_DISPLAY_PG1, 0L );

	lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy = METER_MAX/2 );
	SetHealthDigitDisplay(hWnd, lEnergy);
	
	SetKillDigitDisplay(hWnd, iKills);

	lpLastShot = NULL;

	PostMessage( hWnd, WM_COMMAND, 'N', 0L );
	return( YES );
}

//************************************************************************
BOOL TestSceneInit( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{ // Entering the game's test scene

	hVideo = GetDlgItem( hWnd, IDC_VIDEO );
	lpVideo = (LPVIDEO)GetWindowLong( hVideo, GWL_DATAPTR );

	// Install the event proc for turn effects
	if ( lpVideo )
		lpVideo->lpEventProc = CollisionProcessEvent;

	VideoSetTimer( hWnd, VIDEO_TIMER );

	lpLastShot = NULL;

	return( YES );
}

//************************************************************************
static BOOL GetShotVector( LPPOINT lpPoint1, LPPOINT lpPoint2, long lFlags )
//************************************************************************
{
	if ( !lpPoint1 || !lpPoint2 )
		return( NO );

	if ( !(lFlags & (FLAGBIT(F_HX) | FLAGBIT(F_UJ))) )
		return( NO );

	POINT pt1, pt2;
	if ( lFlags & FLAGBIT(F_POS_BEG) )
	{
		pt1.x = 0;		pt1.y = 0;
		pt2.x = 0;		pt2.y = 500;
	}
	else
	if ( lFlags & FLAGBIT(F_POS_MID) )
	{
		pt1.x = 0;		pt1.y = 500;
		pt2.x = 0;		pt2.y = 500;
	}
	else
	if ( lFlags & FLAGBIT(F_POS_END) )
	{
		pt1.x = 0;		pt1.y = 500;
		pt2.x = 0;		pt2.y = 1000;
	}
	else
	if ( lFlags & FLAGBIT(F_POS_FULL) )
	{
		pt1.x = 0;		pt1.y = 0;
		pt2.x = 0;		pt2.y = 1000;
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
		pt1.x = 300;	pt1.y = 0;
		pt2.x = 300;	pt2.y = 0;
	}
	else
	if ( lFlags & FLAGBIT(F_POS_VEER) )
	{
		pt1.x = 0;		pt1.y = 0;
		pt2.x = -100;	pt2.y = 1000;
	}
	else
		return( NO );

	*lpPoint1 = pt1;
	*lpPoint2 = pt2;
	return( YES );
}

static long lShortDistance;
static POINT ptShortDistance;
//************************************************************************
void CALLBACK DistanceDDA( int x, int y, LPARAM lpData )
//************************************************************************
{
	LPPOINT lp = (LPPOINT)lpData;
	if ( !lp )
		return;
	long x2 = (x - lp->x); x2 *= x2;
	long y2 = (y - lp->y); y2 *= y2;
	long l = x2 + y2;
	if ( l > lShortDistance )
		return;
	lShortDistance = l;
	ptShortDistance.y = x;
	ptShortDistance.y = y;
}

//************************************************************************
static BOOL ShortestDistance( LPPOINT lpBeg, LPPOINT lpEnd, LPPOINT lpIn, LPPOINT lpOut )
//************************************************************************
{ // Compute a point on passed vector that is at the shortest distance to the passed point
	if ( !lpBeg || !lpEnd || !lpIn || !lpOut )
		return( NO );
	lShortDistance = 999999999;
	ptShortDistance = *lpBeg;
	LineDDA( lpBeg->x, lpBeg->y, lpEnd->x, lpEnd->y, DistanceDDA, (LPARAM)lpIn );
	*lpOut = ptShortDistance;
	return( YES );
}

//************************************************************************
static BOOL IndentShot( HWND hWnd, LPSHOT lpNewShot, LPSHOT lpShot )
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
	
	// Compute how far into the current shot we are
	long lFramesIn = MCIGetPosition(lpVideo->hDevice);
	if ( lFramesIn < lpShot->lStartFrame || lFramesIn > lpShot->lEndFrame )
		return( YES );
	lFramesIn -= lpShot->lStartFrame;

	// Compute the length of the current shot
	long lLength = lpShot->lEndFrame - lpShot->lStartFrame + 1;
	if ( lLength <= 0 )
		return( YES );

	// Reverse the indent amount if turning around
	if ( lpNewShot->iLastEvent == EVENT_UP || lpNewShot->iLastEvent == EVENT_DOWN )
		lFramesIn = lLength - lFramesIn;

	// Get the player's travel vector
	POINT pt1, pt2;
	if ( !GetShotVector( &pt1, &pt2, lpShot->lFlags ) )
		return( YES );

	// Compute the player's point on the track
	LFIXED fScale = fget( lFramesIn, lLength );
	if ( fScale < 0 || fScale > UNITY )
		return( YES );
	POINT pt;
	pt.x = pt1.x + fmul( pt2.x - pt1.x, fScale );
	pt.y = pt1.y + fmul( pt2.y - pt1.y, fScale );

	// Get the player's NEW travel vector
	if ( !GetShotVector( &pt1, &pt2, lpNewShot->lFlags ) )
		return( YES );

	// Compute the point on the new travel vector that is at the shortest distance
	ShortestDistance( &pt1, &pt2, &pt, &pt/*lpOutPoint*/ );

	// Compute the indentation percentage
	int dx = pt1.x - pt2.x;
	int dy = pt1.y - pt2.y;
	if ( abs(dx) > abs(dy) )
		fScale = fget( (pt1.x - pt.x), dx );
	else
		fScale = fget( (pt1.y - pt.y), dy );
	if ( fScale < 0 || fScale > UNITY )
		return( YES );

	// Compute the length of the NEW shot
	long lNewLength = lpNewShot->lEndFrame - lpNewShot->lStartFrame + 1;
	if ( lNewLength <= 0 )
		return( YES );

	// Compute the indentation
	long lFrameOffset = fmul( lNewLength, fScale );

	lpNewShot->lIndentFrames = lFrameOffset;
	lpNewShot->lIndentFrames &= (~3); // put it on a key frame boundary
	return( YES );
}

#ifdef UNUSED
//************************************************************************
static BOOL IndentShot( HWND hWnd, LPSHOT lpNewShot, LPSHOT lpShot )
//************************************************************************
{
	if ( !lpNewShot || !lpShot || !lpVideo )
		return( YES );

	// If the shot change is not the result of a turn, process the shot normally
	if ( lpNewShot->iLastEvent == EVENT_END )
		return( YES );

	// If both shots aren't flagged to be indentable, get out and play the shot normally
	if ( !lpNewShot->lValue || !lpShot->lValue)
		return( YES );
	
	// Figure out how far into the current shot we are
	long lFramesIn = MCIGetPosition(lpVideo->hDevice);
	if ( lFramesIn < lpShot->lStartFrame || lFramesIn > lpShot->lEndFrame )
		return( YES );

	// First see if we can get the indent from the video world object
	long lFrameOffset = VWorld.GetJumpFrameOffset(lpShot->lValue, lpShot->lStartFrame,
						lFramesIn, lpNewShot->lValue);

	// If no offset from vworld, use old method to figure what frame to jump to
	if (lFrameOffset == -1)
	{
		// Compute the length of the current shot
		long lLength = lpShot->lEndFrame - lpShot->lStartFrame + 1;
		if ( !lLength )
			return( YES );

		lFramesIn -= lpShot->lStartFrame;
		if ( lFramesIn > lLength )
			lFramesIn = lLength;

		// Reverse the indent amount if turning around
		if ( lpNewShot->iLastEvent == EVENT_UP || lpNewShot->iLastEvent == EVENT_DOWN )
			lFramesIn = lLength - lFramesIn;

		// Compute the length of the new shot
		long lNewLength = lpNewShot->lEndFrame - lpNewShot->lStartFrame + 1;

		// Calculate the indentation
		lFrameOffset = (lNewLength * lFramesIn) / lLength;
	}

	lpNewShot->lIndentFrames = lFrameOffset;
	lpNewShot->lIndentFrames &= (~3); // put it on a key frame boundary
	return( YES );
}
#endif

//************************************************************************
void PlaySceneCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{ // Commands from the game's main play scene
	UINT loWord = (UINT)hControl;
	UINT hiWord = codeNotify;

	switch (id)
	{
	#ifdef WDEBUG
	case IDC_GETVIEW:
	{
		int nData;

		if ( codeNotify != BN_CLICKED )
			break;

		BOOL bTranslated;
		nData = GetDlgItemInt(hWnd, IDC_VIEWDISTANCE, &bTranslated, FALSE);
		if (bTranslated)
		{
			pWeapon->SetViewDistance(nData);
		}

		nData = GetDlgItemInt(hWnd, IDC_XPIXELS, &bTranslated, FALSE);
		if (bTranslated)
		{
			pWeapon->SetXPixels(nData);
		}
		nData = GetDlgItemInt(hWnd, IDC_YPIXELS, &bTranslated, FALSE);
		if (bTranslated)
		{
			pWeapon->SetYPixels(nData);
		}
		nData = GetDlgItemInt(hWnd, IDC_ZPIXELS, &bTranslated, FALSE);
		if (bTranslated)
		{
			pWeapon->SetZPixels(nData);
		}
		nData = GetDlgItemInt(hWnd, IDC_YTEST, &bTranslated, TRUE);
		if (bTranslated)
		{
			pWeapon->SetYTest(nData);
		}
		nData = GetDlgItemInt(hWnd, IDC_GRIDLENGTH, &bTranslated, FALSE);
		if (bTranslated)
		{
			pWeapon->SetGridLength(nData);
		}
		nData = GetDlgItemInt(hWnd, IDC_GRIDWIDTH, &bTranslated, FALSE);
		if (bTranslated)
		{
			pWeapon->SetGridWidth(nData);
		}
		nData = GetDlgItemInt(hWnd, IDC_SIGHTVAR, &bTranslated, FALSE);
		if (bTranslated)
		{
			pWeapon->SetSightVar(nData);
		}
		nData = GetDlgItemInt(hWnd, IDC_EXPSCALEFACTOR, &bTranslated, FALSE);
		if (bTranslated)
		{
			pWeapon->SetExpScaleFactor(nData);
		}
	}
	break;
	#endif // WDEBUG		

    case VK_TAB:
	case 'N'://IDC_WEAPON:
	{	
		ShowWindow(GetDlgItem(hWnd, IDC_MNE_LOOP1), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd, IDC_MSL_LOOP1), SW_HIDE);
		if (iCurrentWeapon == 3)
			iCurrentWeapon = 0;
		else 
			iCurrentWeapon++;

		SendDlgItemMessage( hWnd, IDC_WEAPON, SM_SETICONID, IDC_WEAPON + iCurrentWeapon + 1, 0L );
		RedrawWindow(GetDlgItem(hWnd, IDC_WEAPON), NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
		KillTimer(hWnd, IDC_LOAD_TIMER);
		SetTimer(hWnd, IDC_LOAD_TIMER, 500, NULL);
		//PostMessage( hWnd, WM_COMMAND, 'M', 0L );
		break;
	}

	case 'A':
		MusicOne.DebugInfo();
		break;

	case 'S':
		switch(nMusicState)
		{
		case 0:
			MusicOne.Play(TRUE, 2);
			nMusicState = 1;
			break;

		case 1:
			MusicOne.Stop();
			nMusicState = 0;
			break;

		//case 2:
		//	MusicTwo.Play(TRUE, 2);
		//	nMusicState = 3;
		//	break;

		//case 3:
		//	MusicTwo.Stop();
		//	nMusicState = 0;
		//	break;
		}
		break;

	case 'M'://IDC_WEAPON:
	{
		SendDlgItemMessage( hWnd, IDC_AMMOTYPE, SM_SETICONID, IDC_AMMOTYPE + iCurrentWeapon + 1, 0L );
		RedrawWindow(GetDlgItem(hWnd, IDC_AMMOTYPE), NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
		bInProgress = TRUE;
		iCurrentFrame = 0;		

		// Go ahead and clear observer, since we are drawing only one weapon missile at a time
		VideoClearObserver(pWeapon);

		if (iCurrentWeapon == 1)
		{
			pWeapon = &MGun;
			SetAmmoDigitDisplay(hWnd, iAmmoRounds/ROUNDS_PER_CLIP);
		}
		else
		if (iCurrentWeapon == 3)
		{
			pWeapon = &RocketLauncher;
			SetAmmoDigitDisplay(hWnd, iRockets);
		}
		else
		if (iCurrentWeapon == 2)
		{
			pWeapon = &LaserBomb;
			SetAmmoDigitDisplay(hWnd, iMines);
		}
		else
		if (iCurrentWeapon == 0)
		{
			pWeapon = &LaserBomb;
			SetAmmoDigitDisplay(hWnd, -1);
		}
		else
			pWeapon = &RocketLauncher;
			//SetAmmoDigitDisplay(hWnd, -1);

		// Set new video observer
		VideoSetObserver(pWeapon);

		// Make sure the weapons info get's updated
		pWeapon->OnVideoOpen();
		break;
	}

	case IDC_DISPLAY_PG1:
	case IDC_DISPLAY_PG2:
	case IDC_DISPLAY_PG3:
	{
		if ( SendDlgItemMessage(hWnd, id, SM_GETSTATE, 0, 0L) == 1 )
			break;

		SendDlgItemMessage(hWnd, IDC_DISPLAY_PG1, SM_SETSTATE, (id == IDC_DISPLAY_PG1), 0L);
		SendDlgItemMessage(hWnd, IDC_DISPLAY_PG2, SM_SETSTATE, (id == IDC_DISPLAY_PG2), 0L);
		SendDlgItemMessage(hWnd, IDC_DISPLAY_PG3, SM_SETSTATE, (id == IDC_DISPLAY_PG3), 0L);

		// Set New Icons
		if (id != IDC_DISPLAY_PG1 )
		{
			ITEMID idIcon;
			if ( id == IDC_DISPLAY_PG2)
				idIcon = IDC_LEVEL1_OPP1;
			else
			if ( id == IDC_DISPLAY_PG3 )
				idIcon = IDC_LEVEL1_HINT3;
			else
				idIcon = NULL;
			for ( int i=0; i<9; i++ )
				SendDlgItemMessage( hWnd, IDC_DISPLAY1+i, SM_SETICONID, idIcon+i, 0L );
		}
		else
		{
			ITEMID idIcon;
			for ( int i=0; i<9; i++ )
			{
				if (lpCollect->ctColl[i].bCollected)
					idIcon = lpCollect->ctColl[i].iBitmapID;
				else
					idIcon = NULL;
				SendDlgItemMessage( hWnd, IDC_DISPLAY1+i, SM_SETICONID, idIcon, 0L );
			}
	
		}

		//Redraw the changed area
		RECT rctSquare1;
		GetWindowRect(GetDlgItem(hWnd, IDC_DISPLAY1) , &rctSquare1);
		RECT rctSquare2;
		GetWindowRect(GetDlgItem(hWnd, IDC_DISPLAY9) , &rctSquare2);
		RECT rctUnion;
		rctUnion.top = rctSquare1.top;
		rctUnion.left = rctSquare1.left;
		rctUnion.bottom = rctSquare2.bottom;
		rctUnion.right = rctSquare2.right;
		MapWindowPoints(NULL, hWnd, (LPPOINT)&rctUnion, 2);
		InvalidateRect(hWnd, &rctUnion , FALSE);		
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
		MusicOne.Reset();

		// Remember where we were
		if (lpVideo)
			m_lSaveShot = lpVideo->lCurrentShot;
		else
			m_lSaveShot = 0;

		// Save the zoom factor
		if (lpVideo)
		{
			char szBuffer[2];
			szBuffer[0] = '0' + lpVideo->iZoomFactor;
			szBuffer[1] = 0;
			WritePrivateProfileString (OPTIONS_SECTION, "ZoomFactor", szBuffer, INI_FILE);
		}

		SaveGame(hWnd, 0);
		if ( id == IDC_EXIT )
			GetApp()->GotoScene (hWnd, 2);
		else
		if ( id == IDC_OPTIONS )
		{
			m_bOptionsfromGame = TRUE;
			GetApp()->GotoScene (hWnd, 40);
		}
		else
		if ( id == IDC_SAVE )
			GetApp()->GotoScene (hWnd, 70);
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
		if ( lpWorld )
			lpWorld->LoadLevel( id - VK_F1 + 1 );
		InvalidateRect(hRadar, NULL, TRUE); 
		break;
	}

//	case 'T':
//	{ // turn the navigation effects on and off
//		bEffects = !bEffects;
//		break;
//	}

	//debug (mc)
	case 'B':
	{
		pWeapon->BigExplode();
		break;
	}

	case '=':
	{
		bShowGrid = ~bShowGrid;
		pWeapon->SetShowGrid(bShowGrid);
		break;
	}

    case IDC_WEAPON:
    { // The weapon fire key was pressed

		// Exit before firing if no ammo
		if ( iCurrentWeapon == 0)
		{			
			if (!lEnergy)
				break;
		}	
		if ( iCurrentWeapon == 1)
		{
			if (!iAmmoRounds)
				break;
			bFiring = TRUE;
			iFireFrame = 1;
		}
		if ( iCurrentWeapon == 2)
		{
			if (!iMines)
				break;
		}
		if ( iCurrentWeapon == 3)
		{
			if (!iRockets)
				break;
		}

		// Fire!!!
		UINT flags = hiWord;
		BOOL bFired = ( flags & KF_REPEAT ? pWeapon->Repeat() : pWeapon->Start() );
		if ( !bFired )
			break;

		// Decrement appropriate weapon stores
		if ( iCurrentWeapon == 0)
		{			
			lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= METER_MAX/100 );
			SetHealthDigitDisplay(hWnd, lEnergy);
		}	
		if ( iCurrentWeapon == 1)
		{
			iAmmoRounds--;
			SetAmmoDigitDisplay(hWnd, iAmmoRounds/ROUNDS_PER_CLIP);
		}
		if ( iCurrentWeapon == 2)
		{
			iMines--;
			SetAmmoDigitDisplay(hWnd, iMines);
		}
		if ( iCurrentWeapon == 3)
		{
			iRockets--;
			SetAmmoDigitDisplay(hWnd, iRockets);
		}
		break;
    }

	//#ifdef UNUSED
	case 'D':
	{
		if (bOption2 == FALSE)
		pWeapon->MoveSightLeft();
		break;
	}

	case 'G':
	{
		if (bOption2 == FALSE)
			pWeapon->MoveSightRight();
		break;
	}
	//#endif // UNUSED

    case IDC_CLOSE:
    {
		//FORWARD_WM_COMMAND( hWnd, IDC_EXIT, hWnd, 0, SendMessage );
		GetApp()->GotoScene( hWnd, 2/*iScene*/ );
		break;
    }
    
    case IDC_VIDEO:
    {
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

  			LPSHOT lpShot = GetNewCellShot( hWnd, hVideo );
			if ( lpShot )
			{ // force the shot change
				*lppNewShot = lpShot;
				lpNewShot = lpShot;
				// Inform HUD that we are changing cells
				SetupHUD(hWnd, 0/*iScroll*/);
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
			lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/5) );
			SetHealthDigitDisplay(hWnd, lEnergy);
		}
		
		if ( lpNewShot->iLastEvent == EVENT_HOME)
		{
			iKills++;
			SetKillDigitDisplay(hWnd, iKills);
		}

		// See if we have to leave the game
		if ( lEnergy <= 0 )
		{ // We lose!!!

			MusicOne.Stop();
			MusicOne.Reset();  // 

			LPSHOT lpShot;
			if ( lpShot = Video_FindShotFlag( hVideo, F_YOU_LOSE ) )
				*lppNewShot = lpShot; // force the shot change
			lEnergy = 1; // so we don't hit this again
		}

		// Are we done?
		if ( lpLastShot->lFlags & FLAGBIT(F_EXIT) )
		{
			*lppNewShot = NULL; // don't allow the new shot to be started
			//FORWARD_WM_COMMAND( hWnd, IDC_EXIT, hWnd, 0, SendMessage );
			GetApp()->GotoScene( hWnd, 2/*iScene*/ );
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
			NewDirection( hWnd, EVENT_DOWN );
		else
			NewDirection( hWnd, lpNewShot->iLastEvent );

		pWeapon->SetShot( lpNewShot );

		lpLastShot = lpNewShot;
		break;
	}

		default:
			break;
	}
}


//************************************************************************
void TestSceneCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{ // Commands from the game's main play scene
	static BOOL bProcessEndEvents;
	static int iFlag;
	static STRING szShot;

	UINT loWord = (UINT)hControl;
	UINT hiWord = codeNotify;

	switch (id)
	{
	case IDC_VIDEO_REPLAY:
	{
		FORWARD_WM_KEYDOWN( hVideo, 'P', 0/*cRepeat*/, 0/*flags*/, SendMessage);
		break;
	}

	case IDC_VIDEO_SHOT:
	{
		if ( codeNotify != EN_CHANGE )
			break;
		GetDlgItemText( hWnd, id, szShot, sizeof(STRING) );
		break;
	}

	case IDC_VIDEO_FLAG:
	{
		if ( codeNotify != EN_CHANGE )
			break;
		BOOL Bool;
		iFlag = GetDlgItemInt( hWnd, id, &Bool, NO );
		break;
	}

	case IDC_VIDEO_GOTOSHOT:
	{
		LPSHOT lpShot = Video_FindShotName( hVideo, szShot );
		if ( !lpShot )
			break;
		if ( !lpVideo )
			break;
		Video_GotoShot( hVideo, lpVideo, lpShot->lShotID, 1/*lCount*/, 0/*iFrames*/, EVENT_TIMEOUT );
		break;
	}

	case IDC_VIDEO_GOTOFLAG:
	{
		LPSHOT lpShot = Video_FindShotFlag( hVideo, iFlag );
		if ( !lpShot )
			break;
		if ( !lpVideo )
			break;
		Video_GotoShot( hVideo, lpVideo, lpShot->lShotID, 1/*lCount*/, 0/*iFrames*/, EVENT_TIMEOUT );
		break;
	}

	case IDC_VIDEO_ENDS:
	{
		bProcessEndEvents = !bProcessEndEvents;
		CheckDlgButton( hWnd, id, bProcessEndEvents );
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
    {
		GetApp()->GotoScene (hWnd, 2);
		break;
    }

    case IDC_CLOSE:
    {
		//FORWARD_WM_COMMAND( hWnd, IDC_EXIT, hWnd, 0, SendMessage );
		GetApp()->GotoScene( hWnd, 2/*iScene*/ );
		break;
    }
    
    case IDC_VIDEO:
    {
		LPPSHOT lppNewShot = (LPPSHOT)MAKELPARAM( loWord, hiWord );
		if ( !lppNewShot )
			break; // its a stop code

		LPSHOT lpNewShot = *lppNewShot;
		if ( !lpNewShot )
			break;

		if ( !bProcessEndEvents && (lpNewShot->iLastEvent == EVENT_END) )
		{ // if triggered by the end event and we are going to ignore them...
			*lppNewShot = NULL; // don't allow the new shot to be started
			break;
		}

		// Are we entering a new cell?
		if ( lpNewShot->lFlags & FLAGBIT(F_NEWCELL) )
		{
  			LPSHOT lpShot = GetNewCellShot( hWnd, hVideo );
			if ( lpShot )
			{ // force the shot change
				*lppNewShot = lpShot;
				lpNewShot = lpShot;
			}
		}

		if ( !lpLastShot )
		{
			lpLastShot = lpNewShot;
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

		lpLastShot = lpNewShot;

		if ( lpVideo )
		{
			STRING szString;
			GetAtomName( lpNewShot->aShotID, szString, sizeof(STRING) );
			SetDlgItemText( hWnd, IDC_VIDEO_SHOTNAME, szString );
			SetDlgItemInt( hWnd, IDC_VIDEO_SHOTLEN,
				lpNewShot->lEndFrame - lpNewShot->lStartFrame, NO );
		}

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
			GetApp()->SetKeyMapEntry( hVideo, VK_ESCAPE, WM_COMMAND, IDOK, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_SPACE, WM_COMMAND, IDOK, 0L, YES/*fOnDown*/);
			GetApp()->SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDOK, 0L );
			GetApp()->SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDOK, 0L );
			GetApp()->SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDOK, 0L );
			GetApp()->SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDOK, 0L );
			GetApp()->SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDOK, 0L );
			GetApp()->SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDOK, 0L );
			GetApp()->SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDOK, 0L );
			GetApp()->SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDOK, 0L );
			Radar_Register( GetApp()->GetInstance() );
			return( TRUE );
		}

		// The main menu scene
		case 2:
		{
			GetApp()->SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_START, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_QUIT, 0L, YES/*fOnDown*/);
			GetApp()->SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDC_START, 0L );
			GetApp()->SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDC_RESUME, 0L );
			GetApp()->SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDC_QUIT, 0L );
			GetApp()->SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDC_OPTIONS, 0L );
			GetApp()->SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDC_START, 0L );
			GetApp()->SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDC_RESUME, 0L );
			GetApp()->SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDC_OPTIONS, 0L );
			GetApp()->SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDC_QUIT, 0L );

			// Disable weapon system from observing video changes
			VideoClearObserver(pWeapon);

			hVideo = GetDlgItem( hWnd, IDC_VIDEO );
			lpVideo = (LPVIDEO)GetWindowLong( hVideo, GWL_DATAPTR );
			return( TRUE );
		}

		// The play game screen
		case 20:
		{
			PlaySceneInit( hWnd, hWndFocus, lParam );

			GetApp()->SetKeyMapEntry( hVideo, VK_PAUSE, NULL/*WM_KEYDOWN*/, VK_PAUSE, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_LEFT,  NULL/*WM_KEYDOWN*/, VK_LEFT,  0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_RIGHT, NULL/*WM_KEYDOWN*/, VK_RIGHT, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_UP,    NULL/*WM_KEYDOWN*/, VK_UP,    0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_DOWN,  NULL/*WM_KEYDOWN*/, VK_DOWN,  0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_HOME,  NULL/*WM_KEYDOWN*/, VK_HOME,  0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_SPACE, NULL/*WM_KEYDOWN*/, VK_SPACE, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_EXIT, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, 'F', WM_COMMAND, IDC_WEAPON, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, '=', WM_COMMAND, '=', 0L, YES/*fOnDown*/);

			// Temporary test (mc)
			GetApp()->SetKeyMapEntry( hWnd, 'S', WM_COMMAND, 'S', 0L, YES);
			GetApp()->SetKeyMapEntry( hWnd, 'A', WM_COMMAND, 'A', 0L, YES);

		//#ifdef UNUSED
			GetApp()->SetKeyMapEntry( hWnd, 'G', WM_COMMAND, 'G', 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, 'D', WM_COMMAND, 'D', 0L, YES/*fOnDown*/);
		//#endif
			GetApp()->SetKeyMapEntry( hWnd, 'M', WM_COMMAND, 'M', 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, 'N', WM_COMMAND, 'N', 0L, YES/*fOnDown*/);

			GetApp()->SetKeyMapEntry( hWnd, VK_TAB, WM_COMMAND, VK_TAB, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_F1, WM_COMMAND, VK_F1, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_F2, WM_COMMAND, VK_F2, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_F3, WM_COMMAND, VK_F3, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_F4, WM_COMMAND, VK_F4, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_F5, WM_COMMAND, VK_F5, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_F6, WM_COMMAND, VK_F6, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_F7, WM_COMMAND, VK_F7, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_F8, WM_COMMAND, VK_F8, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_F9, WM_COMMAND, VK_F9, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_F10,WM_COMMAND, VK_F10, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_F11,WM_COMMAND, VK_F11, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_F12,WM_COMMAND, VK_F12, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, 'T',   WM_COMMAND, 'T', 0L, YES/*fOnDown*/);
			//debug (mc)
			GetApp()->SetKeyMapEntry( hWnd, 'B', WM_COMMAND, 'B', 0L, YES/*fOnDown*/);

			GetApp()->SetJoyMapEntry( 1,        hWnd, WM_COMMAND, IDC_WEAPON, 0L );
			GetApp()->SetJoyMapEntry( 2,        hWnd, WM_COMMAND, VK_TAB, 0L );
			GetApp()->SetJoyMapEntry( 3,        hVideo, WM_KEYDOWN, VK_PAUSE, 0L );
			GetApp()->SetJoyMapEntry( 4,        hVideo, WM_KEYDOWN, VK_SPACE, 0L );
			GetApp()->SetJoyMapEntry( VK_LEFT,  hVideo, WM_KEYDOWN, VK_LEFT, 0L );
			GetApp()->SetJoyMapEntry( VK_UP,    hVideo, WM_KEYDOWN, VK_UP, 0L );
			GetApp()->SetJoyMapEntry( VK_RIGHT, hVideo, WM_KEYDOWN, VK_RIGHT, 0L );
			GetApp()->SetJoyMapEntry( VK_DOWN,  hVideo, WM_KEYDOWN, VK_DOWN, 0L );
			return( TRUE );
		}

		// The test screen
		case 25:
		{
			TestSceneInit( hWnd, hWndFocus, lParam );

			GetApp()->SetKeyMapEntry( hVideo, VK_PAUSE, NULL/*WM_KEYDOWN*/, VK_PAUSE, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_LEFT,  NULL/*WM_KEYDOWN*/, VK_LEFT,  0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_RIGHT, NULL/*WM_KEYDOWN*/, VK_RIGHT, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_UP,    NULL/*WM_KEYDOWN*/, VK_UP,    0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_DOWN,  NULL/*WM_KEYDOWN*/, VK_DOWN,  0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_HOME,  NULL/*WM_KEYDOWN*/, VK_HOME,  0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_SPACE, NULL/*WM_KEYDOWN*/, VK_SPACE, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_NEXT,  NULL/*WM_KEYDOWN*/, VK_SPACE, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hVideo, VK_PRIOR, NULL/*WM_KEYDOWN*/, VK_SPACE, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_EXIT, 0L, YES/*fOnDown*/);
			return( TRUE );
		}

		// The resume game scene
		case 30:
		{
			GetApp()->SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_CANCEL, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_CANCEL, 0L, YES/*fOnDown*/);
			GetApp()->SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			return( OnResumeSceneInit( hWnd, hWndFocus, lParam ) );
		}

		// The options scene
		case 40:
		{
			GetApp()->SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_RETURN, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_RETURN, 0L, YES/*fOnDown*/);
			GetApp()->SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDC_RETURN, 0L );
			return( OptionsSceneInit( hWnd, hWndFocus, lParam ) );
		}

		// The credits scene
		case 50:
		{
			GetApp()->SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_RETURN, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_RETURN, 0L, YES/*fOnDown*/);
			GetApp()->SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDC_RETURN, 0L );
			GetApp()->SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDC_RETURN, 0L );
			return( TRUE );
		}

		// The quit scene
		case 60:
		{
			GetApp()->SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_YES, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_NOT, 0L, YES/*fOnDown*/);
			GetApp()->SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDC_YES, 0L );
			GetApp()->SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDC_YES, 0L );
			GetApp()->SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDC_NOT, 0L );
			GetApp()->SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDC_NOT, 0L );
			GetApp()->SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDC_YES, 0L );
			GetApp()->SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDC_YES, 0L );
			GetApp()->SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDC_NOT, 0L );
			GetApp()->SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDC_NOT, 0L );
			return( TRUE );
		}

		// The save game scene
		case 70:
		{
			GetApp()->SetKeyMapEntry( hWnd, VK_RETURN, WM_COMMAND, IDC_CANCEL, 0L, YES/*fOnDown*/);
			GetApp()->SetKeyMapEntry( hWnd, VK_ESCAPE, WM_COMMAND, IDC_CANCEL, 0L, YES/*fOnDown*/);
			GetApp()->SetJoyMapEntry( 1, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( 2, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( 3, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( 4, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( VK_LEFT,  hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( VK_UP,    hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( VK_RIGHT, hWnd, WM_COMMAND, IDC_CANCEL, 0L );
			GetApp()->SetJoyMapEntry( VK_DOWN,  hWnd, WM_COMMAND, IDC_CANCEL, 0L );
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
			// Any command message moves us along
			GetApp()->GotoScene( hWnd, 2 );
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
					GetApp()->GotoScene (hWnd, 20);
					break;
				}
				case IDC_RESUME:
				{
					GetApp()->GotoScene (hWnd, 30);
					break;
				}
				case IDC_CREDITS:
				{
					GetApp()->GotoScene (hWnd, 50);
					break;
				}
				case IDC_OPTIONS:
				{
					m_bOptionsfromGame = FALSE;
					GetApp()->GotoScene (hWnd, 40);
					break;
				}
				case IDC_QUIT:
				{
					GetApp()->GotoScene (hWnd, 60);
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
					GetApp()->GotoScene (hWnd, 2);
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
					GetApp()->GotoScene (hWnd, 0);
					break;
				}
				case IDC_NOT:
				{ // Return to main screen
					GetApp()->GotoScene (hWnd, 2);
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
	CScene::OnClose( hWnd );
	switch (m_nSceneNo)
	{
		case 20: // play - test animation
		{
			VideoKillTimer( hWnd, VIDEO_TIMER );
			RocketLauncher.Close();
			LaserBomb.Close();
			MGun.Close();
			break;
		}
		case 25: // play - test animation
		{
			VideoKillTimer( hWnd, VIDEO_TIMER );
			break;
		}
	}

	MusicOne.Stop();
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
			//Timer to load the selected weapon after 0.5 seconds of not selecting a weapon.
			if (id == (UINT)IDC_LOAD_TIMER)
			{
				KillTimer(hWnd, IDC_LOAD_TIMER);
				PostMessage( hWnd, WM_COMMAND, 'M', 0L );
				return;
			}

			if ( id == VIDEO_TIMER )
				return;
			static int iCount;
			if ( ++iCount == 15 )
			{ // update the frame number 15/iCount times a second
				if ( lpWorld )
					lpWorld->MoveOpponents();	
				iCount = 0;
			}

			// check to see if weapon selection animation needs to be advanced	
			//ShowWindow(GetDlgItem(hWnd, IDC_MSL_LOOP1), SW_HIDE);
			//ShowWindow(GetDlgItem(hWnd, IDC_MSL_LOOP1), SW_HIDE);
			AnimateWeaponLoad(hWnd);
			AnimateWeaponFire(hWnd);
			AnimateHUDScroll(hWnd);
	
			// Animate Weapon looping such as the rocket fire and lights on the mines
			if (bLoopFrame)
			{
				AnimateLoop(hWnd);
				bLoopFrame = FALSE;
			}		
			else
				bLoopFrame = TRUE;
		}

		case 25: // play - test animation
		{
			if ( id == VIDEO_TIMER )
				return;
			static int iCount;
			if ( ++iCount == 1 )
			{ // update the frame number 15/iCount times a second
				STRING szString;
				wsprintf( szString, "%ld", MCIGetPosition( lpVideo->hDevice ) );
				SetDlgItemText( hWnd, IDC_VIDEO_SHOTFRAME, szString );
				iCount = 0;
			}
			break;
		}
	}
}

//************************************************************************
BOOL OnResumeSceneInit (HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	// Get current titles from the INI file
	GetPrivateProfileString (OPTIONS_SECTION, "Game1Title", "", m_szGame1Title, TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME1_NAME, m_szGame1Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game2Title", "", m_szGame2Title, TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME2_NAME, m_szGame2Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game3Title", "", m_szGame3Title, TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME3_NAME, m_szGame3Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game4Title", "", m_szGame4Title, TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME4_NAME, m_szGame4Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game5Title", "", m_szGame5Title, TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME5_NAME, m_szGame5Title);

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
			GetApp()->GotoScene (hWnd, 2);
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
	GetDlgItemText (hWnd, IDC_GAME1_NAME, m_szGame1Title, TITLE_SIZE);
	WritePrivateProfileString (OPTIONS_SECTION, "Game1Title", m_szGame1Title, INI_FILE);

	GetDlgItemText (hWnd, IDC_GAME2_NAME, m_szGame2Title, TITLE_SIZE);
	WritePrivateProfileString (OPTIONS_SECTION, "Game2Title", m_szGame2Title, INI_FILE);

	GetDlgItemText (hWnd, IDC_GAME3_NAME, m_szGame3Title, TITLE_SIZE);
	WritePrivateProfileString (OPTIONS_SECTION, "Game3Title", m_szGame3Title, INI_FILE);

	GetDlgItemText (hWnd, IDC_GAME4_NAME, m_szGame4Title, TITLE_SIZE);
	WritePrivateProfileString (OPTIONS_SECTION, "Game4Title", m_szGame4Title, INI_FILE);

	GetDlgItemText (hWnd, IDC_GAME5_NAME, m_szGame5Title, TITLE_SIZE);
	WritePrivateProfileString (OPTIONS_SECTION, "Game5Title", m_szGame5Title, INI_FILE);
}

//***********************************************************************
void SaveGame( HWND hWnd, int iGameNumber )
//***********************************************************************
{
	char szKey[32];
	char szValue[32];
	WORD wFlags;  // Level information flags
	int iSaveLevel;

	// Save the shot number
	wsprintf (szKey, "Game%dShot", iGameNumber);
	wsprintf (szValue, "%ld", m_lSaveShot);
	// Here we need to save the level number information
	// and the collectibles that we have obtained.  On restore
	// the player will be placed in the beginning of that level but with the
	// collectible already gained.
	iSaveLevel = lpWorld->GetLevel();
	for (int iLoop = 15; iLoop >= 0; iLoop--)
	{
		if (lpCollect->IsCollected(iLoop))
		{
			wFlags || 0x01;
		}
		if (iLoop > 0)
			wFlags << 1;
	}


	WritePrivateProfileString (OPTIONS_SECTION, szKey, szValue, INI_FILE);

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
	Video_SetDefaults( iZoomFactor, lResumeShot,0 );

	// Resume the game
	GetApp()->GotoScene (hWnd, 20);
}

//************************************************************************
BOOL OnSaveGameSceneInit (HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	// Get current titles from the INI file
	GetPrivateProfileString (OPTIONS_SECTION, "Game1Title", "", m_szGame1Title,
	TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME1_NAME, m_szGame1Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game2Title", "", m_szGame2Title,
	TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME2_NAME, m_szGame2Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game3Title", "", m_szGame3Title,
	TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME3_NAME, m_szGame3Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game4Title", "", m_szGame4Title,
	TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME4_NAME, m_szGame4Title);

	GetPrivateProfileString (OPTIONS_SECTION, "Game5Title", "", m_szGame5Title,
	TITLE_SIZE, INI_FILE);
	SetDlgItemText (hWnd, IDC_GAME5_NAME, m_szGame5Title);

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
BOOL CCollisionApp::GotoScene( HWND hWndPreviousScene, int iScene )
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


/***********************************************************************/
static LPSHOT GetNewCellShot( HWND hWnd, HWND hVideo )
/***********************************************************************/
{
	if ( !lpWorld )
		return( NULL );
  
	
	CELLINFO ciCellInfo;
	lpWorld->GetCellData( &ciCellInfo );
	int iDir = lpWorld->GetPlayerDir();

	// Pick the shot to play
	char cType;
	STRING szShot;
	int i = 0;
	
	if	((((iDir == 2) && (ciCellInfo.szPortType_02 == 'B'))||
		((iDir == 2) && (ciCellInfo.szPortType_02 == 'b'))||
		((iDir == 4) && (ciCellInfo.szPortType_04 == 'B'))||
		((iDir == 4) && (ciCellInfo.szPortType_04 == 'b'))||
		((iDir == 6) && (ciCellInfo.szPortType_06 == 'B'))||
		((iDir == 6) && (ciCellInfo.szPortType_06 == 'b'))||
		((iDir == 8) && (ciCellInfo.szPortType_08 == 'B'))||
		((iDir == 8) && (ciCellInfo.szPortType_08 == 'b'))||
		((iDir == 10) && (ciCellInfo.szPortType_10 == 'B'))||
		((iDir == 10) && (ciCellInfo.szPortType_10 == 'b'))||
		((iDir == 12) && (ciCellInfo.szPortType_12 == 'B'))||
		((iDir == 12) && (ciCellInfo.szPortType_12 == 'b'))) && (bBarricadeBroken == FALSE))
	{
		switch (iDir)
		{
			case 2:
			{
				if (ciCellInfo.szPortType_02 == 'B')
				{
					if ((lEnergy / 100000.0) >= 0.5)
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'C';
						bBarricadeBroken = TRUE;
						lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/10));
						SetHealthDigitDisplay(hWnd, lEnergy);
					}
					else
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'B';
						bBarricadeBroken = TRUE;
					}
				}
				if (ciCellInfo.szPortType_02 == 'b')
				{
					if ((lEnergy / 100000.0) >= 0.3)
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'C';
						bBarricadeBroken = TRUE;
						lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/20));
						SetHealthDigitDisplay(hWnd, lEnergy);
					}
					else
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'B';
						bBarricadeBroken = TRUE;
					}
				}
				goto Terminate;
				break;
			}
			case 4:
			{
				if (ciCellInfo.szPortType_04 == 'B')
				{
					if ((lEnergy / 100000.0) >= 0.5)
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'C';
						bBarricadeBroken = TRUE;
						lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/10));
						SetHealthDigitDisplay(hWnd, lEnergy);
					}
					else
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'B';
						bBarricadeBroken = TRUE;
					}
				}
				if (ciCellInfo.szPortType_04 == 'b')
				{
					if ((lEnergy / 100000.0) >= 0.3)
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'C';
						bBarricadeBroken = TRUE;
						lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/20));
						SetHealthDigitDisplay(hWnd, lEnergy);
					}
					else
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'B';
						bBarricadeBroken = TRUE;
					}
				}
				goto Terminate;
				break;
			}
			case 6:
			{
				if (ciCellInfo.szPortType_06 == 'B')
				{
					if ((lEnergy / 100000.0) >= 0.5)
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'C';
						bBarricadeBroken = TRUE;
						lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/10));
						SetHealthDigitDisplay(hWnd, lEnergy);
					}
					else
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'B';
						bBarricadeBroken = TRUE;
					}
				}
				if (ciCellInfo.szPortType_06 == 'b')
				{
					if ((lEnergy / 100000.0) >= 0.3)
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'C';
						bBarricadeBroken = TRUE;
						lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/20));
						SetHealthDigitDisplay(hWnd, lEnergy);
					}
					else
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'B';
						bBarricadeBroken = TRUE;
					}
				}
				goto Terminate;
				break;
			}
			case 8:
			{
				if (ciCellInfo.szPortType_08 == 'B')
				{
					if ((lEnergy / 100000.0) >= 0.5)
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'C';
						bBarricadeBroken = TRUE;
						lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/10));
						SetHealthDigitDisplay(hWnd, lEnergy);
					}
					else
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'B';
						bBarricadeBroken = TRUE;
					}
				}
				if (ciCellInfo.szPortType_08 == 'b')
				{
					if ((lEnergy / 100000.0) >= 0.3)
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'C';
						bBarricadeBroken = TRUE;
						lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/20));
						SetHealthDigitDisplay(hWnd, lEnergy);
					}
					else
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'B';
						bBarricadeBroken = TRUE;
					}
				}
				goto Terminate;
				break;
			}
			case 10:
			{
				if (ciCellInfo.szPortType_10 == 'B')
				{
					if ((lEnergy / 100000.0) >= 0.5)
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'C';
						bBarricadeBroken = TRUE;
						lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/10));
						SetHealthDigitDisplay(hWnd, lEnergy);
					}
					else
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'B';
						bBarricadeBroken = TRUE;
					}
				}
				if (ciCellInfo.szPortType_10 == 'b')
				{
					if ((lEnergy / 100000.0) >= 0.3)
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'C';
						bBarricadeBroken = TRUE;
						lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/20));
						SetHealthDigitDisplay(hWnd, lEnergy);

					}
					else
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'B';
						bBarricadeBroken = TRUE;
					}
				}
				goto Terminate;
				break;
			}
			case 12:
			{
				if (ciCellInfo.szPortType_12 == 'B')
				{
					if ((lEnergy / 100000.0) >= 0.5)
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'C';
						bBarricadeBroken = TRUE;
						lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/10));
						SetHealthDigitDisplay(hWnd, lEnergy);
					}
					else
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'B';
						bBarricadeBroken = TRUE;
					}
				}
				if (ciCellInfo.szPortType_12 == 'b')
				{
					if ((lEnergy / 100000.0) >= 0.3)
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'C';
						bBarricadeBroken = TRUE;
						lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy -= (METER_MAX/20));
						SetHealthDigitDisplay(hWnd, lEnergy);
					}
					else
					{
						szShot[i++] = '/';
						szShot[i++] = 'B';
						szShot[i++] = 'B';
						bBarricadeBroken = TRUE;
					}
				}
				goto Terminate;
				break;
			}
		}	
	}

	//We broke the barricade so go to the next shot.
	bBarricadeBroken = FALSE;
	// IF a barricade shot is not needed, ask
	// the radar to put us in the next cell
	// and return what kind of cell it is.
	lpWorld->NewCellLocation( &ciCellInfo );
	cType = ciCellInfo.szCellType;
	
	// 0th byte:
	// The slash
	szShot[i++] = '/';

	// 1st byte:
	// The cell type
	switch ( cType )
	{
		case 'H': // hex
		case 'U': // union jack
		case 'B': // black hole
			szShot[i++] = cType;
			break;
		case '`': // diag 10-4 tunnel
		case '|': // vert tunnel
		case '/': // diag 2-8 tunnel
			szShot[i++] = 'T';
			break;
		case 'C': // cul-de-sac
			szShot[i++] = 'C';
			szShot[i++] = lpWorld->GetCulDeSacType( &ciCellInfo, iDir );
			if (lpWorld->GetCulDeSacType( &ciCellInfo, iDir ) == 'X')
			{
				//Determine is we have met the criteria to move to the
				//next level and change the map if so.
				ManageGateway(hWnd);
			}
			if (lpWorld->GetCulDeSacType( &ciCellInfo, iDir ) == 'N')
			{
				//Determine is we have met the criteria to move to the
				//next level and change the map if so.
				ManageEntry(hWnd);
			}
			goto Terminate;
			break;
		default:
			return( NULL );
	}

	// 2nd byte:
	// The look
	if ( ciCellInfo.szLookType )
		szShot[i++] = ciCellInfo.szLookType;
	else
		szShot[i++] = '0';

	// Only shots with with portals and opponents need continue
	if ( cType != 'H' && cType != 'U' )
		goto Terminate;

	// 3nd and 4th bytes:
	// The portal type
	if ( cType == 'H' )
	{
		switch ( iDir )
		{
			case 4:
			case 8:
			case 12:
				szShot[i++] = 'Y';
				szShot[i++] = '_';
				break;
			case 1:
			case 5:
			case 9:
				szShot[i++] = 'Y';
				szShot[i++] = 'G';
				break;
			case 2:
			case 6:
			case 10:
				szShot[i++] = 'G';
				szShot[i++] = '_';
				break;
			case 3:
			case 7:
			case 11:
				szShot[i++] = 'G';
				szShot[i++] = 'Y';
				break;
			default:
				szShot[i++] = '_';
				szShot[i++] = '_';
				break;
		}
	}
	else
	if ( cType == 'U' )
	{
		switch ( iDir )
		{
			case 3:
				szShot[i++] = 'X';
				szShot[i++] = 'G';
				break;
			case 4:
			case 10:
				szShot[i++] = 'G';
				szShot[i++] = '_';
				break;
			case 5:
			case 11:
				szShot[i++] = 'G';
				szShot[i++] = 'Y';
				break;
			case 6:
			case 12:
				szShot[i++] = 'Y';
				szShot[i++] = '_';
				break;
			case 1:
			case 7:
				szShot[i++] = 'Y';
				szShot[i++] = 'R';
				break;
			case 2:
			case 8:
				szShot[i++] = 'R';
				szShot[i++] = '_';
				break;
			case 9:
				szShot[i++] = 'R';
				szShot[i++] = 'X';
				break;
			default:
				szShot[i++] = '_';
				szShot[i++] = '_';
				break;
		}
	}

	// 5th byte:
	// The 10 o'clock opponent
	szShot[5] = '\0';

	// 6th byte:
	// The 12 o'clock opponent
	szShot[6] = '\0';

	// 7th byte:
	// The  2 o'clock opponent
	szShot[7] = '\0';

	Terminate:
	// The final byte:
	// Null terminate
	szShot[i++] = '\0';

	return( Video_FindShotPrefix( hVideo, szShot ) );
}

/***********************************************************************/
static void NewDirection( HWND hWnd, int iEvent )
/***********************************************************************/
{ // Inform the radar that we are changing directions
	if ( !iEvent )
		return;

	if ( !lpWorld )
		return;

	int iDirection = lpWorld->GetPlayerDir();

	switch ( iEvent )
	{
		case EVENT_LEFT:
		{
			if ( --iDirection < 1 )
				iDirection += 12;
			//if ( bEffects )
			//if ( bOption3 )
			//	VidFx.DoFx( QuickTurnLeftFx );			
			// Inform the radar that we are changing directions
			lpWorld->SetPlayerDir( iDirection );
			SetupHUD(hWnd, 1/*iScroll*/ );
			break;	
		}		
		case EVENT_RIGHT:
		{
			if ( ++iDirection > 12 )
				iDirection -= 12;
			//if ( bEffects )
			//if ( bOption3 )
			//	VidFx.DoFx( QuickTurnRightFx );
			// Inform the radar that we are changing directions
			lpWorld->SetPlayerDir( iDirection );
			SetupHUD(hWnd, -1/*iScroll*/ );
			break;
		}      
		case EVENT_DOWN:
		{
			if ( (iDirection-=6) < 1 )
				iDirection += 12;
			//if ( bEffects )
			//if ( bOption3 )
			//	VidFx.DoFx( ReverseFx );
			// Inform the radar that we are changing directions
			lpWorld->SetPlayerDir( iDirection );
			SetupHUD(hWnd, 0/*iScroll*/);
			break;
		}      
		default:
			return;
	}
}

/***********************************************************************/
static void InitWeapons(HWND hWnd)
/***********************************************************************/
{
	pWeapon = &RocketLauncher;

	// Set weapon system to observe video changes.
	VideoSetObserver(pWeapon);

	// Open all the weapons
	MGun.Open(hWnd);
	RocketLauncher.Open(hWnd);
	LaserBomb.Open(hWnd);

	pWeapon->SetShowGrid(bShowGrid);
}

/***********************************************************************/
static void SetHUDColor( HWND hWnd, int iPlayerDir, char cCellType, BOOL bDisplay )
/***********************************************************************/
{
	COLORREF crVisorBlank	= RGB(0,0,0);
	COLORREF crVisorRed		= RGB(255,0,0);
	COLORREF crVisorGreen	= RGB(0,255,0);
	COLORREF crVisorYellow	= RGB(255,255,0);

	COLORREF iLeft = crVisorBlank;
	COLORREF iLeftCenter = crVisorBlank;
	COLORREF iCenter = crVisorBlank;
	COLORREF iRightCenter = crVisorBlank;
	COLORREF iRight = crVisorBlank;

	switch( cCellType )
	{
		case 'U':
		{
			switch (iPlayerDir)
			{
				case 1:
				case 7:
				{
					iLeft = crVisorBlank;
					iLeftCenter = crVisorYellow;
					iCenter = crVisorBlank;
					iRightCenter = crVisorRed;
					iRight = crVisorBlank;
					break;
				}
				case 2:				
				case 8:
				{
					iLeft = crVisorYellow;
					iLeftCenter = crVisorBlank;
					iCenter = crVisorRed;
					iRightCenter = crVisorBlank;
					iRight = crVisorGreen;
					break;
				}
				case 3:
				case 9:
				{
					iLeft = crVisorBlank;
					iLeftCenter = crVisorRed;
					iCenter = crVisorBlank;
					iRightCenter = crVisorGreen;
					iRight = crVisorBlank;
					break;
				}
				case 4:
				case 10:
				{
					iLeft = crVisorRed;
					iLeftCenter = crVisorBlank;
					iCenter = crVisorGreen;
					iRightCenter = crVisorBlank;
					iRight = crVisorYellow;
					break;
				}
				case 5:
				case 11:
				{
					iLeft = crVisorBlank;
					iLeftCenter = crVisorGreen;
					iCenter = crVisorBlank;
					iRightCenter = crVisorYellow;
					iRight = crVisorBlank;
					break;
				}
				case 6:			
				case 12:
				{
					iLeft = crVisorGreen;
					iLeftCenter = crVisorBlank;
					iCenter = crVisorYellow;
					iRightCenter = crVisorBlank;
					iRight = crVisorRed;
					break;
				}
			}
			break;
		}
		case 'H':
		{
			switch (iPlayerDir)
			{
				case 1:
				case 5:
				case 9:
				{
					iLeft = crVisorBlank;
					iLeftCenter = crVisorYellow;
					iCenter = crVisorBlank;
					iRightCenter = crVisorGreen;
					iRight = crVisorBlank;
					break;
				}
				case 2:
				case 6:
				case 10:
				{
					iLeft = crVisorYellow;
					iLeftCenter = crVisorBlank;
					iCenter = crVisorGreen;
					iRightCenter = crVisorBlank;
					iRight = crVisorYellow;
					break;
				}
				case 3:
				case 7:
				case 11:
				{
					iLeft = crVisorBlank;
					iLeftCenter = crVisorGreen;
					iCenter = crVisorBlank;
					iRightCenter = crVisorYellow;
					iRight = crVisorBlank;
					break;
				}
				case 4:
				case 8:
				case 12:
				{
					iLeft = crVisorGreen;
					iLeftCenter = crVisorBlank;
					iCenter = crVisorYellow;
					iRightCenter = crVisorBlank;
					iRight = crVisorGreen;
					break;
				}		
			}
			break;
		}
		default:
		{
			iLeft = crVisorBlank;
			iLeftCenter = crVisorBlank;
			iCenter = crVisorBlank;
			iRightCenter = crVisorBlank;
			iRight = crVisorBlank;
			break;
		}			
	}
			
	SendDlgItemMessage(hWnd, IDC_HUDHOT_L1, BM_SETCOLOR, iLeft, (LPARAM)!bDisplay);
	SendDlgItemMessage(hWnd, IDC_HUDHOT_LC1, BM_SETCOLOR, iLeftCenter, (LPARAM)!bDisplay);
	SendDlgItemMessage(hWnd, IDC_HUDHOT_C1, BM_SETCOLOR, iCenter, (LPARAM)!bDisplay);
	SendDlgItemMessage(hWnd, IDC_HUDHOT_RC1, BM_SETCOLOR, iRightCenter, (LPARAM)!bDisplay);
	SendDlgItemMessage(hWnd, IDC_HUDHOT_R1, BM_SETCOLOR, iRight, (LPARAM)!bDisplay);
	
	// QUESTION: Why are these invalid???? (mc)
	SendDlgItemMessage(hWnd, IDC_MINELITE1, BM_SETCOLOR, RGB(255,0,0), (LPARAM)!bDisplay);
	SendDlgItemMessage(hWnd, IDC_MINELITE2, BM_SETCOLOR, RGB(255,0,0), (LPARAM)!bDisplay);

}

/***********************************************************************/
static BOOL PassThruPortal( HWND hWnd, char cTest )
/***********************************************************************/
{ // return whether or not to zap the code
	// Check for collectables
	if (cTest >= '1' && cTest <= '9' )
	{
		int iCollectible = cTest - '0';
		if ( !lpCollect->IsCollected(iCollectible) )
		{
			lpCollect->SetCollected(iCollectible);
			if (SendDlgItemMessage(hWnd, IDC_DISPLAY_PG1, SM_GETSTATE, 0, 0L) == 1)
			{
				int iLevel = lpWorld->GetLevel();
				ITEMID idNewIcon = (IDC_LEVEL_BASE + ((iLevel - 1) * 100) + (10 + iCollectible));
				HWND hItem = GetDlgItem(hWnd, IDC_DISPLAY1 + (iCollectible - 1));				
				SendMessage( hItem, SM_SETICONID, idNewIcon, 0L );
				InvalidateRect(hItem, NULL, TRUE);						
			}
		}
		return( NO ); // the zap code
	}	

	// Check for energy
	if (cTest == 'E')
	{
		lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy += (METER_MAX/10) );
		SetHealthDigitDisplay(hWnd, lEnergy);
		return( YES ); // the zap code
	}
	if (cTest == 'e')
	{
		lEnergy = Meter_Set( hWnd, IDC_ENERGY, lEnergy += (METER_MAX/20) );
		SetHealthDigitDisplay(hWnd, lEnergy);
		return( YES ); // the zap code
	}

	// Check for ammo
	if (cTest == 'G')
	{
		iAmmoRounds += ((AMMO_MGUN/5) * ROUNDS_PER_CLIP);
		if ( iAmmoRounds > AMMO_MGUN ) iAmmoRounds = AMMO_MGUN;
		SetAmmoDigitDisplay(hWnd, iAmmoRounds/ROUNDS_PER_CLIP);
		return( YES ); // the zap code
	}
	if (cTest == 'g')
	{
		iAmmoRounds += ((AMMO_MGUN/10) * ROUNDS_PER_CLIP);
		if ( iAmmoRounds > AMMO_MGUN ) iAmmoRounds = AMMO_MGUN;
		SetAmmoDigitDisplay(hWnd, iAmmoRounds/ROUNDS_PER_CLIP);
		return( YES ); // the zap code
	}
	if (cTest == 'R')
	{
		iRockets += (AMMO_ROCKETS/5);
		if ( iRockets > AMMO_ROCKETS ) iRockets = AMMO_ROCKETS;
		SetAmmoDigitDisplay(hWnd, iRockets);
		return( YES ); // the zap code
	}
	if (cTest == 'r')
	{
		iRockets += (AMMO_ROCKETS/10);
		if ( iRockets > AMMO_ROCKETS ) iRockets = AMMO_ROCKETS;
		SetAmmoDigitDisplay(hWnd, iRockets);
		return( YES ); // the zap code
	}
	if (cTest == 'S')
	{
		iMines += (AMMO_MINES/5);
		if ( iMines > AMMO_MINES ) iMines = AMMO_MINES;
		SetAmmoDigitDisplay(hWnd, iMines);
		return( YES ); // the zap code
	}
	if (cTest == 's')
	{
		iMines += (AMMO_MINES/10);
		if ( iMines > AMMO_MINES ) iMines = AMMO_MINES;
		SetAmmoDigitDisplay(hWnd, iMines);
		return( YES ); // the zap code
	}

	return( NO );
}

/***********************************************************************/
static void SetupHUD(HWND hWnd, int iScroll)
/***********************************************************************/
{
	if ( !lpWorld )
		return;	

	// If not supposed to scroll, and just redraw HUD
	if ( !bOption3 )
		iScroll = 0;

	BOOL bDisplay;
	if ( !iScroll )
	{
		if ( bScrolling )
			bDisplay = NO;
		else
			bDisplay = YES;
	}
	else
	{
		iScrollDir = iScroll;
		iTotalScrolled = 0;
		bScrolling = YES;
		bDisplay = NO;
	}

	char cHud1[5];
	char cHud2[5];
	CELLINFO ciNewCell;
	lpWorld->GetCellData(&ciNewCell);
	int iDir = lpWorld->GetPlayerDir();
	switch ( iDir )
	{
		case 1:
		{
			cHud1[0] = NULL;
			cHud1[1] = ciNewCell.szPortType_12;
			cHud1[2] = NULL;
			cHud1[3] = ciNewCell.szPortType_02;
			cHud1[4] = NULL;
			cHud2[0] = NULL;
			cHud2[1] = ciNewCell.szOppoType_12;
			cHud2[2] = NULL;
			cHud2[3] = ciNewCell.szOppoType_02;
			cHud2[4] = NULL;
			break;
		}
		case 2:
		{
			cHud1[0] = ciNewCell.szPortType_12;
			cHud1[1] = NULL;
			cHud1[2] = ciNewCell.szPortType_02;
			cHud1[3] = NULL;
			cHud1[4] = ciNewCell.szPortType_04;
			cHud2[0] = ciNewCell.szOppoType_12;
			cHud2[1] = NULL;
			cHud2[2] = ciNewCell.szOppoType_02;
			cHud2[3] = NULL;
			cHud2[4] = ciNewCell.szOppoType_04;

			//Handle entrance to collectible cul-de-sac
			if ( PassThruPortal( hWnd, ciNewCell.szPortType_08 ) )
			{
				POINT pt = ciNewCell.ptPlayerLoc;
				lpWorld->ZapCellUnitCode( pt.x, pt.y, -2, 1, 'M' );
			}
			break;
		}
		case 3:
		{
			cHud1[0] = NULL;
			cHud1[1] = ciNewCell.szPortType_02;
			cHud1[2] = NULL;
			cHud1[3] = ciNewCell.szPortType_04;
			cHud1[4] = NULL;
			cHud2[0] = NULL;
			cHud2[1] = ciNewCell.szOppoType_02;
			cHud2[2] = NULL;
			cHud2[3] = ciNewCell.szOppoType_04;
			cHud2[4] = NULL;
			break;
		}
		case 4:
		{
			cHud1[0] = ciNewCell.szPortType_02;
			cHud1[1] = NULL;
			cHud1[2] = ciNewCell.szPortType_04;
			cHud1[3] = NULL;
			cHud1[4] = ciNewCell.szPortType_06;
			cHud2[0] = ciNewCell.szOppoType_02;
			cHud2[1] = NULL;
			cHud2[2] = ciNewCell.szOppoType_04;
			cHud2[3] = NULL;
			cHud2[4] = ciNewCell.szOppoType_06;

			//Handle entrance to collectible cul-de-sac
			if ( PassThruPortal( hWnd, ciNewCell.szPortType_10 ) )
			{
				POINT pt = ciNewCell.ptPlayerLoc;
				lpWorld->ZapCellUnitCode( pt.x, pt.y, -2, -1, 'M' );
			}
			break;
		}
		case 5:
		{
			cHud1[0] = NULL;
			cHud1[1] = ciNewCell.szPortType_04;
			cHud1[2] = NULL;
			cHud1[3] = ciNewCell.szPortType_06;
			cHud1[4] = NULL;
			cHud2[0] = NULL;
			cHud2[1] = ciNewCell.szOppoType_04;
			cHud2[2] = NULL;
			cHud2[3] = ciNewCell.szOppoType_06;
			cHud2[4] = NULL;
			break;
		}
		case 6:
		{
			cHud1[0] = ciNewCell.szPortType_04;
			cHud1[1] = NULL;
			cHud1[2] = ciNewCell.szPortType_06;
			cHud1[3] = NULL;
			cHud1[4] = ciNewCell.szPortType_08;
			cHud2[0] = ciNewCell.szOppoType_04;
			cHud2[1] = NULL;
			cHud2[2] = ciNewCell.szOppoType_06;
			cHud2[3] = NULL;
			cHud2[4] = ciNewCell.szOppoType_08;

			//Handle entrance to collectible cul-de-sac
			if ( PassThruPortal( hWnd, ciNewCell.szPortType_12 ) )
			{
				POINT pt = ciNewCell.ptPlayerLoc;
				lpWorld->ZapCellUnitCode( pt.x, pt.y, 0, -2, 'M' );
			}
			break;
		}
		case 7:
		{
			cHud1[0] = NULL;
			cHud1[1] = ciNewCell.szPortType_06;
			cHud1[2] = NULL;
			cHud1[3] = ciNewCell.szPortType_08;
			cHud1[4] = NULL;
			cHud2[0] = NULL;
			cHud2[1] = ciNewCell.szOppoType_06;
			cHud2[2] = NULL;
			cHud2[3] = ciNewCell.szOppoType_08;
			cHud2[4] = NULL;
			break;
		}
		case 8:
		{
			cHud1[0] = ciNewCell.szPortType_06;
			cHud1[1] = NULL;
			cHud1[2] = ciNewCell.szPortType_08;
			cHud1[3] = NULL;
			cHud1[4] = ciNewCell.szPortType_10;
			cHud2[0] = ciNewCell.szOppoType_06;
			cHud2[1] = NULL;
			cHud2[2] = ciNewCell.szOppoType_08;
			cHud2[3] = NULL;
			cHud2[4] = ciNewCell.szOppoType_10;

			//Handle entrance to collectible cul-de-sac
			if ( PassThruPortal( hWnd, ciNewCell.szPortType_02 ) )
			{
				POINT pt = ciNewCell.ptPlayerLoc;
				lpWorld->ZapCellUnitCode( pt.x, pt.y, 2, -1, 'M' );
			}
			break;
		}
		case 9:
		{
			cHud1[0] = NULL;
			cHud1[1] = ciNewCell.szPortType_08;
			cHud1[2] = NULL;
			cHud1[3] = ciNewCell.szPortType_10;
			cHud1[4] = NULL;
			cHud2[0] = NULL;
			cHud2[1] = ciNewCell.szOppoType_08;
			cHud2[2] = NULL;
			cHud2[3] = ciNewCell.szOppoType_10;
			cHud2[4] = NULL;
			break;
		}
		case 10:
		{
			cHud1[0] = ciNewCell.szPortType_08;
			cHud1[1] = NULL;
			cHud1[2] = ciNewCell.szPortType_10;
			cHud1[3] = NULL;
			cHud1[4] = ciNewCell.szPortType_12;
			cHud2[0] = ciNewCell.szOppoType_08;
			cHud2[1] = NULL;
			cHud2[2] = ciNewCell.szOppoType_10;
			cHud2[3] = NULL;
			cHud2[4] = ciNewCell.szOppoType_12;

			//Handle entrance to collectible cul-de-sac
			if ( PassThruPortal( hWnd, ciNewCell.szPortType_04 ) )
			{
				POINT pt = ciNewCell.ptPlayerLoc;
				lpWorld->ZapCellUnitCode( pt.x, pt.y, 2, 1, 'M' );
			}
			break;
		}
		case 11:
		{
			cHud1[0] = NULL;
			cHud1[1] = ciNewCell.szPortType_10;
			cHud1[2] = NULL;
			cHud1[3] = ciNewCell.szPortType_12;
			cHud1[4] = NULL;
			cHud2[0] = NULL;
			cHud2[1] = ciNewCell.szOppoType_10;
			cHud2[2] = NULL;
			cHud2[3] = ciNewCell.szOppoType_12;
			cHud2[4] = NULL;
			break;
		}
		case 12:
		{
			cHud1[0] = ciNewCell.szPortType_10;
			cHud1[1] = NULL;
			cHud1[2] = ciNewCell.szPortType_12;
			cHud1[3] = NULL;
			cHud1[4] = ciNewCell.szPortType_02;
			cHud2[0] = ciNewCell.szOppoType_10;
			cHud2[1] = NULL;
			cHud2[2] = ciNewCell.szOppoType_12;
			cHud2[3] = NULL;
			cHud2[4] = ciNewCell.szOppoType_02;

			//Handle entrance to collectible cul-de-sac
			if ( PassThruPortal( hWnd, ciNewCell.szPortType_06 ) )
			{
				POINT pt = ciNewCell.ptPlayerLoc;
				lpWorld->ZapCellUnitCode( pt.x, pt.y, 0, 2, 'M' );
			}
			break;
		}
	}

	int iLevel = lpWorld->GetLevel();
	for ( int i=0; i<5; i++ )
	{
		ITEMID idIcon1;
		ITEMID idIcon2;

		if ( cHud1[i] == 'E' )	idIcon1 = IDC_HUDENR_S;	else
		if ( cHud1[i] == 'e' )	idIcon1 = IDC_HUDENR_W;	else
		if ( cHud1[i] == 'B' )	idIcon1 = IDC_HUDBAR_S;	else
		if ( cHud1[i] == 'b' )	idIcon1 = IDC_HUDBAR_W;	else
		if ( cHud1[i] == 'G' )	idIcon1 = IDC_HUDAMMO;	else
		if ( cHud1[i] == 'g' )	idIcon1 = IDC_HUDAMMO;	else
		if ( cHud1[i] == 'R' )	idIcon1 = IDC_HUDROCK;	else
		if ( cHud1[i] == 'r' )	idIcon1 = IDC_HUDROCK;	else
		if ( cHud1[i] == 'S' )	idIcon1 = IDC_HUDMINE;	else
		if ( cHud1[i] == 's' )	idIcon1 = IDC_HUDMINE;	else
		if ( cHud1[i] == 'N' )	idIcon1 = IDC_HUDENTRY;	else
		if ( cHud1[i] == 'n' )	idIcon1 = IDC_HUDENTRY;	else
		if ( cHud1[i] == 'X' )	idIcon1 = IDC_HUDEXIT;	else
		if ( cHud1[i] == 'x' )	idIcon1 = IDC_HUDEXIT;	else
		if ( cHud1[i] >= '1' && cHud1[i] <= '9' )
			idIcon1 = (IDC_LEVEL_BASE + ((iLevel - 1) * 100) + (cHud1[i] - '0') + 10);
		else
			idIcon1 = NULL;

		if ( cHud2[i] >= '1' && cHud2[i] <= '9' )
			idIcon2 = (IDC_LEVEL_BASE + ((iLevel - 1) * 100) + (cHud2[i] - '0'));
		else
			idIcon2 = NULL;

		SendDlgItemMessage( hWnd, IDC_HUD1+i, SM_SETICONID, idIcon1, !bDisplay );
		SendDlgItemMessage( hWnd, IDC_HUDOPP1+i, SM_SETICONID, idIcon2, !bDisplay );
	}

	SetHUDColor( hWnd, iDir, ciNewCell.szCellType, bDisplay );
}

/***********************************************************************/
static BOOL AnimateWeaponLoad(HWND hWnd)
/***********************************************************************/
{
	//	Function to advance the weapon animation one frame from its current
	//	state if the in progress variable is true.  This function will return
	//	the following values:
	//	0 - No action taken, animation control is in a final state
	//	1 - Animation was advanced one frame, task is not complete.

	// Kick out because nothing needs to be done
	if ( !bInProgress )
		return( NO );
	
	ITEMID idFrame;	
	if ( iCurrentWeapon == 0 )	idFrame = IDC_LOADLSR1;	else
	if ( iCurrentWeapon == 1 )	idFrame = IDC_LOADMGN1;	else
	if ( iCurrentWeapon == 2 )	idFrame = IDC_LOADMNE1;	else
	if ( iCurrentWeapon == 3 )	idFrame = IDC_LOADMSL1;	else return( NO );

	if (!bOption3)
	{
		switch(iCurrentWeapon)
		{
			case 0:
			{
				iCurrentFrame = (int)LAST_LASERFRAME;				
				break;
			}
			case 1:
			{
				iCurrentFrame = (int)LAST_MGUNFRAME;				
				break;
			}
			case 2:
			{
				iCurrentFrame = (int)LAST_MINEFRAME;				
				break;
			}
			case 3:
			{
				iCurrentFrame = (int)LAST_ROCKETFRAME;		
				break;
			}
			default:
			{
				break;
			}
		}
		SendDlgItemMessage( hWnd, IDC_WEAPON, SM_SETICONID, idFrame + iCurrentFrame, 0L );
		RedrawWindow(GetDlgItem(hWnd, IDC_WEAPON), NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
	}


	if (((iCurrentWeapon == 0 ) && (iCurrentFrame == LAST_LASERFRAME)) || 
		((iCurrentWeapon == 1 ) && (iCurrentFrame == LAST_MGUNFRAME)) ||
		((iCurrentWeapon == 2 ) && (iCurrentFrame == LAST_MINEFRAME)) ||
		((iCurrentWeapon == 3 ) && (iCurrentFrame == LAST_ROCKETFRAME)))
	{
		if ((iCurrentWeapon == 2) && (bOption3))
			ShowWindow(GetDlgItem(hWnd, IDC_MNE_LOOP1), SW_SHOW);		
		if ((iCurrentWeapon == 3) && (bOption3))
			ShowWindow(GetDlgItem(hWnd, IDC_MSL_LOOP1), SW_SHOW);
		bInProgress = FALSE;
		iCurrentFrame = 0;
		return( NO );
	}

	SendDlgItemMessage( hWnd, IDC_WEAPON, SM_SETICONID, idFrame + iCurrentFrame, 0L );
	iCurrentFrame++;
	RedrawWindow(GetDlgItem(hWnd, IDC_WEAPON), NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
	return( YES );
}

/***********************************************************************/
static BOOL AnimateWeaponFire(HWND hWnd)
/***********************************************************************/
{
	// Kick out because nothing needs to be done
	if ( !bFiring )
		return( NO );
	if (iFireFrame > LAST_FIRE_FRAME)
	{					
		bFiring = FALSE;
		iFireFrame = 0;				
		return( NO );
	}
	if (!bOption3)
		return( NO );
	ITEMID idFrame;
	idFrame = IDC_FIRE_MGN1 + (iFireFrame - 1);		
	SendDlgItemMessage( hWnd, IDC_WEAPON, SM_SETICONID, idFrame, 0L );
	RedrawWindow(GetDlgItem(hWnd, IDC_WEAPON), NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
	iFireFrame++;
	return( YES );
}

/***********************************************************************/
CWeapon *GetCurrentWeapon()
/***********************************************************************/
{
	return pWeapon;
}

/***********************************************************************/
static void SetAmmoDigitDisplay(HWND hWnd, int iValue)
/***********************************************************************/
{
	//Function to set the digit display to a value.
	//Digit display is two digits between 0 and 9 giving a 
	//maximum value of 99 clips, rockets or mines
	// - 1 sets both digits to blank
	if (iValue < -1)
		iValue = -1; //Value out of range

	if (iValue > 99)
		iValue = 99; //Value out of range

	ITEMID idTensDigit, idOnesDigit;

	if (iValue == -1)
	{
		idTensDigit = IDC_NODIGIT;
		idOnesDigit = IDC_NODIGIT;
	}
	else
	{
		short iTens = (int)(iValue / 10);
		if (!iTens)
			idTensDigit = IDC_NODIGIT;
		else
			idTensDigit = IDC_RDIGIT0 + iTens;
		short iOnes = iValue - (iTens * 10);
		idOnesDigit = IDC_RDIGIT0 + iOnes;
	}

	SendDlgItemMessage( hWnd, IDC_DIGITA, SM_SETICONID, idTensDigit, 0L );
	SendDlgItemMessage( hWnd, IDC_DIGITB, SM_SETICONID, idOnesDigit, 0L );
	RedrawWindow(GetDlgItem(hWnd, IDC_DIGITA), NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(hWnd, IDC_DIGITB), NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
}

/***********************************************************************/
static void SetKillDigitDisplay(HWND hWnd, int iValue)
/***********************************************************************/
{
	//Function to set the digit display to a value.
	//Digit display is two digits between 0 and 9 giving a 
	//maximum value of 99 clips, rockets or mines
	// - 1 sets both digits to blank
	if (iValue < -1)
		iValue = -1; //Value out of range

	if (iValue > 99)
		iValue = 99; //Value out of range

	ITEMID idTensDigit, idOnesDigit;

	if (iValue == -1)
	{
		idTensDigit = IDC_NODIGIT;
		idOnesDigit = IDC_NODIGIT;
	}
	else
	{
		short iTens = (int)(iValue / 10);
		if (!iTens)
			idTensDigit = IDC_NODIGIT;
		else
			idTensDigit = IDC_YDIGIT0 + iTens;
		short iOnes = iValue - (iTens * 10);
		idOnesDigit = IDC_YDIGIT0 + iOnes;
	}

	SendDlgItemMessage( hWnd, IDC_KILLDIGITA, SM_SETICONID, idTensDigit, 0L );
	SendDlgItemMessage( hWnd, IDC_KILLDIGITB, SM_SETICONID, idOnesDigit, 0L );
	RedrawWindow(GetDlgItem(hWnd, IDC_KILLDIGITA), NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(hWnd, IDC_KILLDIGITB), NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
}

/***********************************************************************/
static void SetHealthDigitDisplay(HWND hWnd, long lValue)
/***********************************************************************/
{
	// incoming value is 0-100,000 based - convert to 0-100
	int iValue = (lValue + 500) / 1000;

	//Function to set the digit display to a value.
	//Digit display is two digits between 0 and 9 giving a 
	//maximum value of 99 clips, rockets or mines
	// - 1 sets both digits to blank
	if (iValue < -1)
		iValue = -1; //Value out of range

	if (iValue > 99)
		iValue = 99; //Value out of range

	ITEMID idTensDigit, idOnesDigit;

	if (iValue == -1)
	{
		idTensDigit = IDC_NODIGIT;
		idOnesDigit = IDC_NODIGIT;
	}
	else
	{
		short iTens = (int)(iValue / 10);
		if (!iTens)
			idTensDigit = IDC_NODIGIT;
		else
			idTensDigit = IDC_GDIGIT0 + iTens;
		short iOnes = iValue - (iTens * 10);
		idOnesDigit = IDC_GDIGIT0 + iOnes;
	}

	SendDlgItemMessage( hWnd, IDC_HEALTHDIGA, SM_SETICONID, idTensDigit, 0L );
	SendDlgItemMessage( hWnd, IDC_HEALTHDIGB, SM_SETICONID, idOnesDigit, 0L );
	RedrawWindow(GetDlgItem(hWnd, IDC_HEALTHDIGA), NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(hWnd, IDC_HEALTHDIGB), NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
}

/***********************************************************************/
static BOOL AnimateHUDScroll(HWND hWnd)
/***********************************************************************/
{		
	#define SCROLL_STEP 31
	#define SCROLL_WIDTH 121

	if (!bScrolling)
		return(FALSE);
	if (!iScrollDir)
		return(FALSE);
	if (!hWnd)
		return(FALSE);

	static RECT rctScroll;
	if ( IsRectEmpty( &rctScroll ) )
	{ // initialize it...
		RECT rctLeft;
		RECT rctRight;
		GetClientRect(GetDlgItem(hWnd, IDC_HUD1), &rctLeft);
		MapWindowPoints(GetDlgItem(hWnd, IDC_HUD1), hWnd, (LPPOINT)&rctLeft, 2);
		GetClientRect(GetDlgItem(hWnd, IDC_HUDOPP5), &rctRight);
		MapWindowPoints(GetDlgItem(hWnd, IDC_HUDOPP5), hWnd, (LPPOINT)&rctRight, 2);
		UnionRect( &rctScroll, &rctLeft, &rctRight );
		rctScroll.bottom += 4; // Catch the portal highlight bars as well
	}

	int dx = (SCROLL_STEP * iScrollDir);
	int iTotal = iTotalScrolled + dx;
	BOOL bLastCall;
	if ( iTotal >= SCROLL_WIDTH )
	{
		dx -= (iTotal - SCROLL_WIDTH);
		bLastCall = YES;
	}
	else
	if ( iTotal <= -SCROLL_WIDTH )
	{
		dx -= (SCROLL_WIDTH + iTotal);
		bLastCall = YES;
	}
	else
		bLastCall = NO;

	iTotalScrolled += dx;

	HDC hDC = GetDC(hWnd);
	if (!hDC)
		return(FALSE);
	BOOL b = ScrollDC( hDC, dx, 0, &rctScroll, &rctScroll, NULL/*hScrollRgn*/, NULL/*rctUpdate*/);
	ReleaseDC(hWnd, hDC);	

	if ( bLastCall )
	{
		RECT rctUpdate = rctScroll;
		if (iTotalScrolled > 0) // if going left...
			rctUpdate.right = rctScroll.left + iTotalScrolled;
		else
			rctUpdate.left = rctScroll.right + iTotalScrolled;
		InvalidateRect( hWnd, &rctUpdate, FALSE );
		UpdateWindow( hWnd );
		bScrolling = NO;		 
	}

	return(TRUE);
}			

/***********************************************************************/
static BOOL AnimateLoop(HWND hWnd)
/***********************************************************************/
{
	if (bInProgress || bFiring)
	{		
		return(FALSE);
	}
	switch (iCurrentWeapon)
	{
		case 0:
		{
			break;
		}
		case 1:
		{
			break;
		}
		case 2:
		{
			if (bOption3)
				SendDlgItemMessage( hWnd, IDC_MNE_LOOP1, SM_SETSTATE, iLoopFrame,0L );			
			if (iLoopFrame == 6) 
				iLoopFrame = 0;
			else
				iLoopFrame++;
			break;
		}
		case 3:
		{			
			if (bOption3)
				SendDlgItemMessage( hWnd, IDC_MSL_LOOP1, SM_SETSTATE, iLoopFrame,0L );			
			if (iLoopFrame == 6) 
				iLoopFrame = 0;
			else
				iLoopFrame++;

			break;
		}

	}
	return(TRUE);
}

/***********************************************************************/
static BOOL ManageGateway(HWND hWnd)
/***********************************************************************/
{
	int iCurrentLevel = lpWorld->GetLevel();

	// Determine if criteria is met

	BOOL bNextLevel = FALSE;
	switch (iCurrentLevel)
	{
		case 1:
		{		
			if(lpCollect->IsCollected(1))
			{
				bNextLevel = TRUE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_2_START;
			}
			else
			{
				bNextLevel = FALSE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_1_GOBACK;
			}
			break;
		}
		case 2:
		{
			if(lpCollect->IsCollected(1))
			{
				bNextLevel = TRUE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_3_START;
			}
			else
			{
				bNextLevel = FALSE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_2_GOBACK;
			}
			break;
		}
		case 3:
		{
			if(lpCollect->IsCollected(1))
			{
				bNextLevel = TRUE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_4_START;
			}
			else
			{
				bNextLevel = FALSE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_3_GOBACK;
			}
			break;
		}
		case 4:
		{
			if(lpCollect->IsCollected(1))
			{
				bNextLevel = TRUE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_5_START;
			}
			else
			{
				bNextLevel = FALSE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_4_GOBACK;
			}
			break;
		}
		case 5:
		{
			if(lpCollect->IsCollected(1))
			{
				bNextLevel = TRUE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_6_START;
			}
			else
			{
				bNextLevel = FALSE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_5_GOBACK;
			}
			break;
		}
		case 6:
		{
			if(lpCollect->IsCollected(1))
			{
				bNextLevel = TRUE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_7_START;
			}
			else
			{
				bNextLevel = FALSE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_6_GOBACK;
			}
			break;
		}
		case 7:
		{
			if(lpCollect->IsCollected(1))
			{
				bNextLevel = TRUE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_8_START;
			}
			else
			{
				bNextLevel = FALSE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_7_GOBACK;
			}
			break;
		}
		case 8:
		{
			if(lpCollect->IsCollected(1))
			{
				bNextLevel = TRUE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_9_START;
			}
			else
			{
				bNextLevel = FALSE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_8_GOBACK;
			}
			break;
		}
		case 9:
		{
			if(lpCollect->IsCollected(1))
			{
				bNextLevel = TRUE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_10_START;
			}
			else
			{
				bNextLevel = FALSE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_9_GOBACK;
			}
			break;
		}
		case 10:
		{
			if(lpCollect->IsCollected(1))
			{
				bNextLevel = TRUE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_11_START;
			}
			else
			{
				bNextLevel = FALSE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_10_GOBACK;
			}
			break;
		}
		case 11:
		{
			if(lpCollect->IsCollected(1))
			{
				bNextLevel = TRUE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_12_START;
			}
			else
			{
				bNextLevel = FALSE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_11_GOBACK;
			}
			break;
		}
		case 12:
		{
			if(lpCollect->IsCollected(1))
			{
				bNextLevel = TRUE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = ENDGAME_MOVIE;
			}
			else
			{
				bNextLevel = FALSE;
				lpVideo->lRegisterValue[VR_LEVEL_SHOT - 1] = LEVEL_12_GOBACK;
			}
			break;
		}
	}					

	if (bNextLevel)
	{
		lpWorld->LoadLevel(iCurrentLevel + 1);
		lpWorld->SetPlayerDir(12);				
		lpVideo->lRegisterValue[VR_CURR_LEVEL - 1] = iCurrentLevel + 1;
 		for (int iLoop = 0; iLoop < 9; iLoop++)
			lpCollect->SetUnCollected(iLoop);
		return (TRUE);
	}
	else
		lpVideo->lRegisterValue[VR_CURR_LEVEL - 1] = iCurrentLevel;

	return (FALSE);
}

/***********************************************************************/
static BOOL ManageEntry(HWND hWnd)
/***********************************************************************/
{
	lpVideo->lRegisterValue[VR_CURR_LEVEL - 1] = lpWorld->GetLevel() - 1;
	return(TRUE);
}

//************************************************************************
//static void ReverseInterface( HWND hWindow, BOOL bNewReverseFlag )
//************************************************************************
//{
//	if ( bNewReverseFlag == bReverse )
//		return;
//	bReverse = bNewReverseFlag; // track whether we are driving in reverse
//
//	if ( !lpVideo )
//		return;
//
//	MCIStop( lpVideo->hDevice, NO );
//	if ( bReverse )
//	{
//		RECT SrcRect, DstRect;
//		GetClientRect( hVideo, &DstRect );
//		InvalidateRect( hWindow, &DstRect, FALSE );
//		// Dest size is 584 x 180; Src size is 292 x 90; video is 316 x 160
//		SetRect( &SrcRect, 12, 20, 12+295, 20+90 );
//		DstRect = SrcRect;
//		ScaleRect( &DstRect, 2 );
//		OffsetRect( &DstRect, -DstRect.left, -DstRect.top );
//		OffsetRect( &DstRect, 56, 96 );
//		MCIClip( lpVideo->hDevice, &SrcRect, &DstRect, 0/*iScale*/ );
//	}
//	else
//	{
//		RECT DstRect; GetClientRect( hVideo, &DstRect );
//		MCIClip( lpVideo->hDevice, NULL/*&SrcRect*/, &DstRect, 2/*iScale*/ );
//	}
//}
