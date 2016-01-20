#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include "proto.h"
#include "weapon.h"
#include "dib.h"
#include "cllsnid.h"
#include "cllsn.h"
#include "control.h"
#include "vworld.h"
#include "DirSnd.h"
#include "wdebug.p"
#include "line.p"
#include "mmiojunk.p"

const int nHexVanishY = 140; // Vanishing y for hex cell
const int nUJVanishY = 104;  // Vanishing y for union jack

static int nVanishY = nHexVanishY; 
static int nVanishX = 320;

// Statics

LPVIDEO CWeapon::lpVideo = NULL;			// Video pointer
int     CWeapon::nWorldRef = 0;				// # of references to world object (3D video coordinate world)
int     CWeapon::nViewDistance = 600;		// pixel distance of viewer from screen
int     CWeapon::nXPixels = 16;				// pixels per feet (approx)
int		CWeapon::nYPixels = 32;				// pixels per feet (approx)
int     CWeapon::nZPixels = 32;				// pixels per feet (approx)
long	CWeapon::lCurrentFrame = 0;			// Current video frame
int		CWeapon::nExpScaleFactor = 300;		// Explosion scale factor
int		CWeapon::yGround = 3;				// y coord of where the ground appears to be
int		CWeapon::nWorldLength = 1000;		// Arbitrary distance of the radar sight
int		CWeapon::nWorldWidth = 200;			// Width from wall to wall on the x axis (left to right)
int		CWeapon::nMissileRange = 300;		// Range of missile
UINT	CWeapon::uEnemyColor = 0;			// Color of enemy rectangle (if vector drawn)
UINT	CWeapon::uEnemyBoxColor = 0;		// Color of enemy box
int		CWeapon::nActiveEnemies = 0;		// Number of enemies
int     CWeapon::nLastActiveEnemies = 0;	// Number of last enemies
BOOL	CWeapon::bEnabled = FALSE;			// Weapons enabled or not
BOOL	CWeapon::bShowGrid = TRUE;			// Show world grid
HWND	CWeapon::hSceneWnd = NULL;			// Current scene window
BCRACK	CWeapon::Cracks[MaxCracks];			// List of bullet cracks
int		CWeapon::nNumCracks = 0;			// Number of bullet cracks
BOOL    CWeapon::m_bOppsPossible = TRUE;
int     CWeapon::m_nCrackCount = 0;
int     CWeapon::m_nWaitJump = 0;

// Score statics
long	CWeapon::m_lScore = 0;
int     CWeapon::m_nScFramesOn = 0;			// # of frames the score stays on
int     CWeapon::m_nScStartX = 0;			// Score start x position
int     CWeapon::m_nScWidth = 0;			// Score digit width
int     CWeapon::m_nScoreLen = 0;			// Score length
char	CWeapon::m_szScore[MaxScoreLen+1];  // Score bytes

// All static bitmap and sound objects
// Create all when first weapon object is created
// Delete all when last weapon object is deleted
PDIB	CWeapon::pdibCracks[2] = {NULL};	// Bullet crack bitmaps
PDIB    CWeapon::pdibHitFF[MaxFF] = {NULL}; // Opponent Force Field hit
PDIB    CWeapon::pdibExp[MaxExp] = {NULL};	// Generic explosions
PDIB    CWeapon::pdibMiss[MaxMiss] = {NULL};
PDIB    CWeapon::m_pdibScore[MaxScoreLen] = {NULL};	// Pointers to bitmap score digits
PDIB    CWeapon::m_pdibDigits[10] = {NULL};	// Source bitmap digits
CDirSnd *CWeapon::pShotSnd = NULL;			// Opponent shooting sound
CDirSnd *CWeapon::pCrackSnd = NULL;			// Car window crack sound
CDirSnd *CWeapon::m_pCarSnds[3] = {NULL};	// Opponent car sounds
CDirSnd *CWeapon::m_pCarCrashSnd = NULL;	
CDirSnd *CWeapon::m_pWallCrashSnd = NULL;
BOOL CWeapon::m_bTextOn = FALSE;
int CWeapon::m_nCurCrashDamage = 0;
int CWeapon::m_nBlackOutDur = 0;
int CWeapon::m_nWhiteOutDur = 0;
DWORD CWeapon::m_dwBlackOutStart = 0;
DWORD CWeapon::m_dwWhiteOutStart = 0;
int CWeapon::m_nEffectDur = 0;
DWORD CWeapon::m_dwEffectStart = 0;
int CWeapon:: m_nEffectColor = 0;
BOOL  CWeapon::m_bStrobe = FALSE;
BOOL  CWeapon::m_bStrobeBlack = FALSE;
int   CWeapon::m_nElectricFrames = 0;
int   CWeapon::m_nElectricColor = 113;
int   CWeapon::m_nColorCycle = 0;

MISSILE CWeapon::EnemyMissiles[NumMissiles];

int CWeapon::m_nPlayerSaveZ = 0;

CRocketLauncher *CWeapon::m_pSpineKiller = NULL;	// Spine killer weapon

CWeaponSystem *CWeapon::m_pSystem = NULL;	// Weapon system pointer

 const double Cos30 = 0.8660254037844;
 const double Sin30 = 0.5;

//BOOL CWeapon::m_bLighteningOn = FALSE;
//int  CWeapon::m_xSrc;
//int  CWeapon::m_ySrc;
//int  CWeapon::m_zSrc;
//int  CWeapon::m_xDst;
//int  CWeapon::m_yDst;
//int  CWeapon::m_zDst;

//***********************************************************************
CWeapon::CWeapon()
//***********************************************************************
{
	SetWeaponPosition( 0, 3, 0);   // Center
	//SetWeaponPosition( 8, 3, 0); // Right side
	SetSightParams(120, 520);

	bSoundTrackOff = FALSE;

	nActiveEnemies = 0;
	nLastActiveEnemies = 0;

	nNumWalls = 0;	// Number of walls in the world

	bUseDebug = FALSE;

	hSceneWnd = NULL;
	m_bTextOn = FALSE;

	m_nRepeat = 0;

	// Increment the weapon reference count
	if (++nWorldRef == 1)
		InitStaticObjects();
}

//***********************************************************************
CWeapon::~CWeapon()
//***********************************************************************
{
	// Decrement the weapon reference count
	if (--nWorldRef == 0)
		FreeStaticObjects();
}

//***********************************************************************
void CWeapon::InitStaticObjects(void)
//***********************************************************************
{
	// Load the static bitmaps
	int i;
	for (i=0; i<10; i++)
	{
		if (!m_pdibDigits[i])
			m_pdibDigits[i] = CDib::LoadDibFromResource(
				GetWindowInstance(hSceneWnd),
				MAKEINTRESOURCE(IDC_DIGIT0+i),
				GetApp()->m_hPal);
	}

	for (i=0; i<2; i++)
	{
		if (!pdibCracks[i])
			pdibCracks[i] = CDib::LoadDibFromResource( 
				GetWindowInstance(hSceneWnd), 
				MAKEINTRESOURCE(IDC_BCRACK1+i),
				GetApp()->m_hPal);
	}

	for (i=0; i<MaxFF; i++)
	{
		if (!pdibHitFF[i])
			pdibHitFF[i] = CDib::LoadDibFromResource( 
				GetWindowInstance(hSceneWnd), 
				MAKEINTRESOURCE(IDC_FFDONUT01+i),
				GetApp()->m_hPal);
	}

	for (i=0; i<MaxExp; i++)
	{
		if (!pdibExp[i])
			pdibExp[i] = CDib::LoadDibFromResource( 
				GetWindowInstance(hSceneWnd), 
				MAKEINTRESOURCE(IDC_EXP1FR00+i),
				GetApp()->m_hPal);
	}

	for (i=0; i<MaxMiss; i++)
	{
		if (!pdibMiss[i])
			pdibMiss[i] = CDib::LoadDibFromResource( 
				GetWindowInstance(hSceneWnd), 
				MAKEINTRESOURCE(IDC_EXPMISS00+i),
				GetApp()->m_hPal);
	}

	// Load all the static sounds
	if (!pShotSnd)
	{
		pShotSnd = new CDirSnd();
		pShotSnd->Open(IDC_SHOTSND, GetApp()->GetInstance(), TRUE);
	}
	
	if (!pCrackSnd)
	{
		pCrackSnd = new CDirSnd();
		pCrackSnd->Open(IDC_CRACKSND, GetApp()->GetInstance(), TRUE);
	}

	
	if (!m_pCarSnds[0])
	{
		m_pCarSnds[0] = new CDirSnd();
		m_pCarSnds[0]->Open(IDC_STRONGCAR, GetApp()->GetInstance(), TRUE);
		m_pCarSnds[0]->SetYRange(200);
		m_pCarSnds[0]->SetVolume(-500);
	}

	if (!m_pCarSnds[1])
	{
		m_pCarSnds[1] = new CDirSnd();
		m_pCarSnds[1]->Open(IDC_BOSSCAR, GetApp()->GetInstance(), TRUE);
		m_pCarSnds[1]->SetYRange(200);
		m_pCarSnds[1]->SetVolume(-500);
	}

	if (!m_pCarSnds[2])
	{
		m_pCarSnds[2] = new CDirSnd();
		m_pCarSnds[2]->Open(IDC_WEAKCAR, GetApp()->GetInstance(), TRUE);
		m_pCarSnds[2]->SetYRange(200);
		m_pCarSnds[2]->SetVolume(-500);
	}

	if (!m_pCarCrashSnd)
	{
		m_pCarCrashSnd = new CDirSnd();
		m_pCarCrashSnd->Open(IDC_CARCRASHSND, GetApp()->GetInstance());
	}

	if (!m_pWallCrashSnd)
	{
		m_pWallCrashSnd = new CDirSnd();
		m_pWallCrashSnd->Open(IDC_WALLCRASHSND, GetApp()->GetInstance());
	}

	for(i=0; i<NumMissiles; i++)
	{
		EnemyMissiles[i].nActive = ACTIVE_NO;
		EnemyMissiles[i].dSpeed = 10;
	}
}

//***********************************************************************
void CWeapon::FreeStaticObjects(void)
//***********************************************************************
{
	// Delete all of the static bitmaps and clear the pointers
	int i;
	for (i=0; i<MaxScoreLen; i++)
	{ // De-reference copies of the digits DIBs below...
		m_pdibScore[i] = NULL;
	}

	for (i=0; i<10; i++)
	{
		if (m_pdibDigits[i])
			delete m_pdibDigits[i];
		m_pdibDigits[i] = NULL;
	}

	for(i=0; i<2; i++)
	{
		if (pdibCracks[i])
			delete pdibCracks[i];
		pdibCracks[i] = NULL;
	}

	for(i=0; i<MaxFF; i++)
	{
		if (pdibHitFF[i])
			delete pdibHitFF[i];
		pdibHitFF[i] = NULL;
	}

	for(i=0; i<MaxExp; i++)
	{
		if (pdibExp[i])
			delete pdibExp[i];
		pdibExp[i] = NULL;
	}

	for(i=0; i<MaxMiss; i++)
	{
		if (pdibMiss[i])
			delete pdibMiss[i];
		pdibMiss[i] = NULL;
	}

	// Delete all of the static sounds and clear the pointers
	if (pShotSnd)
	{
		delete pShotSnd;
		pShotSnd = NULL;
	}

	if (pCrackSnd)
	{
		delete pCrackSnd;
		pCrackSnd = NULL;
	}

	if (m_pCarSnds[0])
	{
		if (m_pCarSnds[0])
			delete m_pCarSnds[0];
		m_pCarSnds[0] = NULL;
	}

	if (m_pCarSnds[1])
	{
		if (m_pCarSnds[1])
			delete m_pCarSnds[1];
		m_pCarSnds[1] = NULL;
	}

	if (m_pCarSnds[2])
	{
		if (m_pCarSnds[2])
			delete m_pCarSnds[2];
		m_pCarSnds[2] = NULL;
	}

	if (m_pCarCrashSnd)
	{
		if (m_pCarCrashSnd)
			delete m_pCarCrashSnd;
		m_pCarCrashSnd = NULL;
	}

	if (m_pWallCrashSnd)
	{
		if (m_pWallCrashSnd)
			delete m_pWallCrashSnd;
		m_pWallCrashSnd = NULL;
	}
}

//***********************************************************************
void CWeapon::Open( HWND hSceneWindow )
//***********************************************************************
{
	hSceneWnd = hSceneWindow;

	// It doesn't hurt to repeatedly call this static method. This was
	// the only good place to call it.  The constructor was too early.
	InitBulletCracks();

	for (int i=0; i<NumMissiles; i++)
	{
	    Missiles[i].nActive = ACTIVE_NO;
		Missiles[i].Type = GunWeapon;
		Missiles[i].wRange = 400;
		Missiles[i].dSpeed = 0.3;
		Missiles[i].nEnemyIndex = -1;
	}
	
	//nActiveMissiles = 0;
	nActiveExplosions = 0;
	for (i=0; i<NumExplosions; i++)
	    Explosions[i].nActive = 0;

	// Set sight color
	LPSCENE lpScene = CScene::GetScene(hSceneWindow);
	HPALETTE hPal = lpScene->GetPalette();

	uSightColor = GetNearestPaletteIndex( hPal, RGB(0, 0xff, 0) );
	uSightBorderColor = GetNearestPaletteIndex( hPal, RGB(0, 0, 0) );
	uSightFiringColor = GetNearestPaletteIndex( hPal, RGB(0xff, 0, 0) );
	uEnemyBoxColor = GetNearestPaletteIndex( hPal, RGB(255, 128, 64) );
	uEnemyColor = GetNearestPaletteIndex( hPal, RGB(255, 255, 0) );
	uGridColor = GetNearestPaletteIndex( hPal, RGB(99, 173, 148) );
}

//***********************************************************************
void CWeapon::NewCell(char cCellType)
//***********************************************************************
{
	m_bTextOn = FALSE;

	StopFire();

	m_bOppsPossible = TRUE;
	nActiveEnemies = 0;
	nLastActiveEnemies = 0;

	m_pCarSnds[0]->Stop();
	m_pCarSnds[1]->Stop();
	m_pCarSnds[2]->Stop();

	// Set vanishing horizon given the cell type
	if (cCellType == 'H')
		nVanishY = nHexVanishY;
	else
		nVanishY = nUJVanishY;

}

//***********************************************************************
BOOL CWeapon::Start( void )
//***********************************************************************
{
	return Fire( NO /*bRepeat*/ );
}

//***********************************************************************
BOOL CWeapon::Repeat( void )
//***********************************************************************
{
	return Fire( YES /*bRepeat*/ );
}

//***********************************************************************
void CWeapon::StartMissile(void)
//***********************************************************************
{ // this function scans through the missile array and tries to find one to fire

	if (!App.pGame)
		return;

	for ( int i=0; i<NumMissiles; i++ )
    {
		if ( Missiles[i].nActive != ACTIVE_NO)
			continue;

		if (Missiles[i].Type == AerialWeapon)
		{
			Missiles[i].x = 0;
			Missiles[i].y = 5;

			m_pCell = App.pGame->lpWorld->GetPlayer()->GetCell();
			m_iDir = App.pGame->lpWorld->GetPlayer()->GetDir();
		}
		else
		{
	        Missiles[i].x = WeaponPos.x;
			Missiles[i].y = 6;
		}

		// Set missile Z position in front of the player
        Missiles[i].z = WeaponPos.z + 10;
		//int nSightX = GetSightWorldX();

		//double dDistance = sqrt( pow(nSightX,2) + pow(nWorldLength, 2) );

		//double dRatio = Missiles[i].dSpeed / dDistance;

		//Missiles[i].xStep = (int)((double)nSightX * dRatio);
		//Missiles[i].zStep = (int)((double)(nWorldLength) * dRatio);

		//Debug("nSightX=%d, nWorldLength=%d, dDistance=%d, dSpeed=%d, xStep=%d, zStep=%d", nSightX, nWorldLength,
		//	(int)dDistance, (int)Missiles[i].dSpeed, Missiles[i].xStep, Missiles[i].zStep);
		Missiles[i].nSightX = GetSightWorldX();
		Missiles[i].wTraveled = 0;
		Missiles[i].nCurFrame = 0;
		Missiles[i].nEnemyIndex = -1;
		Missiles[i].dSpeed = abs(Missiles[i].dSpeed); // make sure missle is moving forward
		Missiles[i].bBehind = FALSE;

        // increment number of active missiles
        //nActiveMissiles++;

		// Activate sound for missile firing
		if (lpVideo)
		{
			HWND hWnd = GetDlgItem(hSceneWnd, IDC_VIDEO);

			// Turn off sound track
			//Video_OnSoundOnOff( hWnd, FALSE );
			//MCIPause(lpVideo->hDevice);
			bSoundTrackOff = TRUE;

			// Start rocket sound
			if (Type == JackWeapon || Type == HoverWeapon)
				pRocketSnd->Play(TRUE, TRUE);
			else
				pRocketSnd->Play(FALSE, TRUE);
		}

		// Mark the missile is active. Need to do this last because the video
		// proc may call MoveMissile() on the missile before we are through
		// preparing it.
        Missiles[i].nActive = ACTIVE_NORMAL;

		return;
	}
}

//***********************************************************************
void CWeapon::StartEnemyMissile(WeaponType Type, int x, int y, int z)
//***********************************************************************
{ // this function scans through the missile array and tries to find one to fire

	if (!App.pGame)
		return;

	for ( int i=0; i<NumMissiles; i++ )
    {
		if ( EnemyMissiles[i].nActive )
			continue;

        EnemyMissiles[i].nActive = YES;

	    EnemyMissiles[i].x = x;
		EnemyMissiles[i].y = 0;
		EnemyMissiles[i].z = z;
		EnemyMissiles[i].dSpeed = 25;

		// Compute player speed per frame
		int nLead = (WeaponPos.z - m_nPlayerSaveZ);
		double dDistance;
		int nTargetZ;

		// If player speed is less then 5 per frame
		// we want to shoot right at the player location
		if (nLead < 1)
		{
			// Shoot a little in front so we can see it
			nTargetZ = WeaponPos.z+5;
			//nTargetZ = 40;

			// Get the distance from the target location
			dDistance = sqrt( pow(x, 2) + pow(z - nTargetZ, 2) );
		}
		// Else find target in front of player
		else
		{
			// Ratio of the missile speed to the player speed
			double dSpeedRatio = EnemyMissiles[i].dSpeed / (double)nLead;

			 // Incrementing distance from the player pos
			double dInc = 30.0;
			nTargetZ = WeaponPos.z + (int)dInc;
			int nSaveTargetZ = nTargetZ;
			
			// Use normalized enemy z
			int ez = z - WeaponPos.z;

			// Get the distance from the new target
			dDistance = sqrt( pow(x, 2) + pow(ez - dInc, 2) );

			// Compute the distance ratio
			double dDistRatio = dDistance / dInc;

			// While the distance ratio is greater then the speed ratio
			while ((dDistRatio > dSpeedRatio) && dInc < 90)
			{
				// Move the target up 10 units
				dInc += 10;
				//nSaveTargetZ = nTargetZ;
				nTargetZ = WeaponPos.z + (int)dInc;

				// Compute the new distance to the target
				dDistance = sqrt( pow(x, 2) + pow(ez - dInc, 2) );

				// Compute the new distance ratio
				dDistRatio = dDistance / dInc;
			}

			//nTargetZ = nSaveTargetZ;
			dDistance = sqrt( pow(x, 2) + pow(z - nTargetZ, 2) );

		}


		// Compute missile travel ratio		
		double dRatio = EnemyMissiles[i].dSpeed / dDistance;

		//Debug("TargetZ = %d", nTargetZ);

		// Figure the x and z steps to take
		EnemyMissiles[i].xStep = (int)((double)(0 - x) * dRatio);
		EnemyMissiles[i].zStep = (int)((double)(nTargetZ - z) * dRatio);

		EnemyMissiles[i].wTraveled = 0;
		EnemyMissiles[i].nCurFrame = 0;
		EnemyMissiles[i].nFrames = 1;
		EnemyMissiles[i].nEnemyIndex = -1;
		EnemyMissiles[i].Type = MissileWeapon;

		//Debug("start e missile: xStep=%d, zStep=%d", EnemyMissiles[i].xStep, EnemyMissiles[i].zStep);

        // increment number of active missiles
        //nActiveMissiles++;

		// Activate sound for missile firing
		if (lpVideo)
		{
			HWND hWnd = GetDlgItem(hSceneWnd, IDC_VIDEO);

			// Turn off sound track
			//Video_OnSoundOnOff( hWnd, FALSE );
			//MCIPause(lpVideo->hDevice);
			//bSoundTrackOff = TRUE;

			// Start rocket sound
			if (Type == JackWeapon || Type == HoverWeapon)
				pRocketSnd->Play(TRUE, TRUE);
			else
				pRocketSnd->Play(FALSE, TRUE);
		}

		return;
	} // end for
}

//***********************************************************************
void CWeapon::MoveEnemyMissiles(void)
//***********************************************************************
{ // this function moves the missiles and does all the collision detection

	if (!App.pGame)
		return;

	for ( int i=0; i<NumMissiles; i++ )
    {
		int iIndex;

	    if ( !EnemyMissiles[i].nActive )
	    	continue;
		
		if (EnemyMissiles[i].nActive == ACTIVE_HIT)
		{
			EnemyMissiles[i].nActive = ACTIVE_NO;
			
			StartExplosion(EnemyMissiles[i].x, EnemyMissiles[i].y, EnemyMissiles[i].z, 1);
		}
		else
		{
			EnemyMissiles[i].x += EnemyMissiles[i].xStep;
			EnemyMissiles[i].z += EnemyMissiles[i].zStep;

			Debug("e missile x=%d, z=%d", EnemyMissiles[i].x, EnemyMissiles[i].z);

			EnemyMissiles[i].wTraveled += (WORD)(EnemyMissiles[i].dSpeed);

			// If hit player
			if ( (iIndex = HitPlayer( &EnemyMissiles[i] )) > -1 ) 
			{
				EnemyMissiles[i].nActive = ACTIVE_HIT;
			} // end if HitPlayer

			// else if out of range
			else if (EnemyMissiles[i].wTraveled > 300)
			{
				StartExplosion(EnemyMissiles[i].x, EnemyMissiles[i].y, EnemyMissiles[i].z, 0);
				EnemyMissiles[i].nActive = ACTIVE_NO;

			}
			// else missile has not hit anything
			else
			{
				// Adjust sound
				if (pRocketSnd)
					pRocketSnd->SetPosition(EnemyMissiles[i].x, EnemyMissiles[i].z, 0);
				
			}
		}
	} // end for loop
}


//***********************************************************************
void CWeapon::AdjustRight(void)
//***********************************************************************
{
	int i;
	for(i=0; i<NumMissiles; i++)
	{
		if (Missiles[i].nActive == ACTIVE_NO)
			continue;

		Debug("Right before x=%d, z=%d, sight x=%d, z=%d", Missiles[i].x, Missiles[i].z, Missiles[i].nSightX, nSightZ);

		RotateRight(&Missiles[i].x, &Missiles[i].z);
		int nTemp = nSightZ;
		RotateRight(&Missiles[i].nSightX, &nTemp);

		Debug("after x=%d, z=%d, sight x=%d, z=%d", Missiles[i].x, Missiles[i].z, Missiles[i].nSightX, nTemp);

		// Translate coordinates back
	}

	for(i=0; i<NumMissiles; i++)
	{
		if (EnemyMissiles[i].nActive == ACTIVE_NO)
			continue;

		RotateRight(&EnemyMissiles[i].x, &EnemyMissiles[i].z);
	}

	for(i=0; i<NumExplosions; i++)
	{
		if (Explosions[i].nActive == ACTIVE_NO)
			continue;

		RotateRight(&Explosions[i].x, &Explosions[i].z);
	}
}

//***********************************************************************
void CWeapon::AdjustLeft(void)
//***********************************************************************
{
	int i;
	for(i=0; i<NumMissiles; i++)
	{
		if (Missiles[i].nActive == ACTIVE_NO)
			continue;

		Debug("Left before x=%d, z=%d, sight x=%d, z=%d", Missiles[i].x, Missiles[i].z, Missiles[i].nSightX, nSightZ);

		RotateLeft(&Missiles[i].x, &Missiles[i].z);
		int nTemp = nSightZ;
		RotateLeft(&Missiles[i].nSightX, &nTemp);

		Debug("after x=%d, z=%d, sight x=%d, z=%d", Missiles[i].x, Missiles[i].z, Missiles[i].nSightX, nTemp);

		// Translate coordinates back
	}

	for(i=0; i<NumMissiles; i++)
	{
		if (EnemyMissiles[i].nActive == ACTIVE_NO)
			continue;

		RotateLeft(&EnemyMissiles[i].x, &EnemyMissiles[i].z);
	}

	for(i=0; i<NumExplosions; i++)
	{
		if (Explosions[i].nActive == ACTIVE_NO)
			continue;

		RotateLeft(&Explosions[i].x, &Explosions[i].z);
	}
}


//***********************************************************************
void CWeapon::AdjustReverse(void)
//***********************************************************************
{
	int i;
	for(i=0; i<NumMissiles; i++)
	{
		if (Missiles[i].nActive == ACTIVE_NO)
			continue;

		Debug("reverse before x=%d, z=%d", Missiles[i].x, Missiles[i].z);

		//RotateLeft(&Missiles[i].x, &Missiles[i].z);
		//int nTemp = nSightZ;
		//RotateLeft(&Missiles[i].nSightX, &nTemp);
		Missiles[i].x *= -1;
		Missiles[i].z *= -1;
		//Missiles[i].nSightX *= -1;
		Missiles[i].bBehind = !Missiles[i].bBehind;

		Debug("after x=%d, z=%d", Missiles[i].x, Missiles[i].z);

		// Translate coordinates back
	}

	for(i=0; i<NumMissiles; i++)
	{
		if (EnemyMissiles[i].nActive == ACTIVE_NO)
			continue;

		EnemyMissiles[i].x *= -1;
		EnemyMissiles[i].z *= -1;
		EnemyMissiles[i].xStep *= -1;
		EnemyMissiles[i].zStep *= -1;
	}

	for(i=0; i<NumExplosions; i++)
	{
		if (Explosions[i].nActive == ACTIVE_NO)
			continue;

		Explosions[i].x *= -1;
		Explosions[i].z *= -1;
	}

}

//***********************************************************************
int CWeapon::MissilesActive(void)
//***********************************************************************
{
	int iNum = 0;
	for(int i=0; i<NumMissiles; i++)
	{
		if (Missiles[i].nActive != ACTIVE_NO)
			++iNum;
	}

	return iNum;
}

//***********************************************************************
BOOL CWeapon::Fire( BOOL bRepeat, BOOL bGetMouse )
//***********************************************************************
{
	return FALSE;
}

//***********************************************************************
void CWeapon::StartExplosion(int x, int y, int z, int nType)
//***********************************************************************
{
}

//***********************************************************************
void CRocketLauncher::Open( HWND hSceneWindow )
//***********************************************************************
{ // override this virtual function is required
}

//***********************************************************************
int CRocketLauncher::LoadBitmaps( ITEMID idFire )
//***********************************************************************
{
	// Load firing bitmaps
	int nFireBitmaps = 0;
	int iCount = sizeof(pdibMissile)/sizeof(PDIB);
	for (int i=0; i<iCount; i++)
		pdibMissile[i] = NULL;
	if ( idFire )
	{
		for (i=0; i<iCount; i++)
		{
			pdibMissile[i] = CDib::LoadDibFromResource( GetApp()->GetInstance(),
				MAKEINTRESOURCE(idFire+i), GetApp()->m_hPal);
			if ( !pdibMissile[i] )
				break;
			nFireBitmaps++;
		}
	}

	return(nFireBitmaps);
}

//***********************************************************************
BOOL CRocketLauncher::Fire( BOOL bRepeat, BOOL bGetMouse )
//***********************************************************************
{
	if (!bEnabled)
		return FALSE;

	int iNum = MissilesActive();

	// If hover weapon, then turn off
	if (Type == HoverWeapon && iNum > 0)
	{
		m_bSightOff = FALSE;
		for(int i=0; i<NumMissiles; i++)
			Missiles[0].nActive = ACTIVE_NO;
		pRocketSnd->Stop();
		return FALSE;
	}

	// Else, normal weapon limit to two missiles
	//else if (iNum > 1)
	//	return FALSE;

	if (Type == HoverWeapon)
		m_bSightOff = TRUE;

	m_nRepeat = 1;

	if (bGetMouse)
	{
		POINT pnt;
		// get mouse cursor
		GetCursorPos(&pnt);

		HWND hVideo = GetDlgItem( hSceneWnd, IDC_VIDEO );
		if (hVideo)
		{
			// convert to video client coords
			ScreenToClient(hVideo, &pnt);
	
			// move the sight to the x position
			if (pnt.x > 0 && pnt.x < 640)
				MoveSight(pnt.x);
		}
	}

	StartMissile();

	return TRUE;
}

//***********************************************************************
void CRocketLauncher::StartExplosion(int x, int y, int z, int nType)
//***********************************************************************
{// this function stars a generic explosion

	// if we are extremely close, then set y to 0 so we will still see it
	if (z > (WeaponPos.z-5) && z < (WeaponPos.z+5))
		y = 0;

	for ( int i=0; i<NumExplosions; i++ )
    {
	    if ( Explosions[i].nActive )
	    	continue;

	    // set it up to use
	    Explosions[i].x = x;
		//Explosions[i].z = z	- WeaponPos.z;
		Explosions[i].z = z;
	    Explosions[i].y = y;
		Explosions[i].nType = nType;

		// Compute distance. 
		Explosions[i].dDistance = 
			sqrt( pow(Explosions[i].x - WeaponPos.x, 2) + pow(Explosions[i].z - WeaponPos.z, 2) );

		Explosions[i].nCurFrame = 0;

		// If hit explosion
		if (nType == 1)
		{
			Explosions[i].nFrames = nNumExpHitFrames;
			Explosions[i].pdibFrames = &pdibExp[0];

			if (pExpBigSnd)
			{
				if (Type != GunWeapon && Type != HoverWeapon)
					pRocketSnd->Stop();
				pExpBigSnd->SetPosition(Explosions[i].x, Explosions[i].z - WeaponPos.z, 0);
				pExpBigSnd->Play();
			}
		}
		// Force field explosion
		else
		if (nType == 2 && Type != GunWeapon)
		{
			Explosions[i].nFrames = MaxFF;
			Explosions[i].pdibFrames = &pdibHitFF[0];
		}
		else 
		{
			Explosions[i].nFrames = nNumExpMissFrames;
			Explosions[i].pdibFrames = &pdibMiss[0];

			if (pExpSmallSnd)
			{
				if (Type != GunWeapon && Type != HoverWeapon)
					pRocketSnd->Stop();
				pExpSmallSnd->SetPosition(Explosions[i].x, Explosions[i].z - WeaponPos.z, 0);
				pExpSmallSnd->Play();
			}
		}

		// Note: Make active one more then the number of frames since this number will be first decremented
		// before the first explosion gets drawn
	    Explosions[i].nActive = Explosions[i].nFrames + 1;

        // increment number of active explosions
        nActiveExplosions++;
	    return;
    }
}

//***********************************************************************
void CWeapon::DrawExplosion(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits ) 
//***********************************************************************
{
	for ( int i=0; i<NumExplosions; i++ )
	{
		if ( Explosions[i].nActive )
		{
			int nPercent;

			// if behind player
			if (Explosions[i].z < WeaponPos.z)
				continue;

			// sometimes a missile will land behind our point of view so we
			// need to make sure the explosion is seen
			if (Explosions[i].z < 5)
				Explosions[i].z = 5;

			POINT Point = Point3DToPointDib(Explosions[i].x, Explosions[i].y, Explosions[i].z - WeaponPos.z);
		
			//Debug("exp x=%d, y=%d, z=%d, px=%d, py=%d", Explosions[i].x, Explosions[i].y, Explosions[i].z, Point.x, Point.y);

			//if (nForcedSize)
			//	nPercent = nForcedSize;
			//else
			if (Type == GunWeapon && (Explosions[i].nType == 0 || Explosions[i].nType == 2))
				nPercent = 40;
			else
				nPercent = 370 - (int)((Explosions[i].dDistance * 370.0) / nExpScaleFactor) ;
			//if (nPercent > 100) {
			//	nPercent = 100;
			//}

			//Debug("exp x=%d, y=%d, dist=%d, percent=%d\n", 
			//	Point.x, Point.y, (int)Explosions[i].dDistance, nPercent);
			
			if (nPercent < nMinExpScale)
				nPercent = nMinExpScale;

			int nFrame;
			if (Explosions[i].nType == 1)
				nFrame = Explosions[i].nCurFrame * 3 + GetRandomNumber(3);
			else 
				nFrame = Explosions[i].nCurFrame;

			// If explosion is not behind enemy
			if (!BehindEnemy(Explosions[i].z, Point))
				DrawSprite(nPercent, Point.x, Point.y, 
					Explosions[i].pdibFrames[nFrame], 
					lpbi, lpDestBits);

			++Explosions[i].nCurFrame;
			if (Explosions[i].nCurFrame >= Explosions[i].nFrames)
				Explosions[i].nCurFrame = 0;
		}
	}
}

//***********************************************************************
void CWeapon::OnVideoOpen()
//***********************************************************************
{
	// Get new position information
	HWND hVideo = GetDlgItem( hSceneWnd, IDC_VIDEO );
	if (!hVideo)
		return;

	lpVideo = (LPVIDEO)GetWindowLong(hVideo, GWL_DATAPTR);
	if (!lpVideo)
		return;

	LPSHOT lpShot = Video_GetShot( lpVideo, lpVideo->lCurrentShot );

	SetShot( lpShot );
}

//***********************************************************************
void CWeapon::SetShot(LPSHOT lpShot)
//***********************************************************************
{
	if ( !lpShot )
		return;

	lStartFrame = lpShot->lStartFrame;
	lEndFrame = lpShot->lEndFrame;

	//Debug("start=%ld, end=%ld\r\n", lpShot->lStartFrame, lpShot->lEndFrame );
	//Debug("OnVideoOpen\n");

	// Set world if not in weapon debug mode
	//#ifndef _DEBUG
	//App.pGame->VWorld->SetWorld(lpShot->lValue, this);
	//#endif

	//WeaponPos.nFrameDist = App.pGame->VWorld->GetLength() / (lEndFrame - lStartFrame);
	//WeaponPos.nFrameDist = (double)nWorldLength / (double)(App.pGame->VWorld->CarStop() - App.pGame->VWorld->CarStart());

	//nActiveMissiles = 0;
}

//***********************************************************************
void CWeapon::Enable(BOOL bEnable)
//***********************************************************************
{
	bEnabled = bEnable;
}

//***********************************************************************
void CWeapon::OnVideoClose()
//***********************************************************************
{
}

//***********************************************************************
void CWeapon::UpdatePosition()
//***********************************************************************
{
	if (!lpVideo)
		return;

	int nCommand;

	if (bUseDebug)
	{
		lCurrentFrame = MCIGetPosition(lpVideo->hDevice);
		GetWDWalls(lCurrentFrame, &yGround, &nNumWalls, Walls);
		GetWDEnemy(lCurrentFrame, &nActiveEnemies, Enemy);
		GetWDCommand(lCurrentFrame, &nCommand);
		m_nPlayerSaveZ = WeaponPos.z;
		WeaponPos.z = GetWDPlayerZ(lCurrentFrame);
		//if (WeaponPos.z < 1)
		//	WeaponPos.z = m_nPlayerSaveZ;
	}
	else
	{
		GetWalls(&yGround, &nNumWalls, Walls);
		GetEnemy(&nActiveEnemies, Enemy);
		GetCommand(&nCommand);
		m_nPlayerSaveZ = WeaponPos.z;
		WeaponPos.z = GetPlayerZ();
		//if (WeaponPos.z < 1)
		//  	WeaponPos.z = m_nPlayerSaveZ;
		//Debug("player pos dif = %d", WeaponPos.z - m_nPlayerSaveZ);
		if (m_bOppsPossible)
		{
			for(int i=0; i<nActiveEnemies; i++)
			{
				EnemyShotSpace[i].nCrackWait = 2;
				EnemyShotSpace[i].nShotWait = 0;
			}
		}
	}

	if (nCommand)
	{
		if (nCommand == CMND_CARCRASH)
		{
			m_pCarCrashSnd->Play();

			// Set current car crash strength
			m_nCurCrashDamage = GetCrashDamage();

			// do crash effect (if any)
			DoCrashEffect();
		}
		
		else if (nCommand == CMND_WALLCRASH)
			m_pWallCrashSnd->Play();

		else if (nCommand == CMND_SHOOT)
			TestEnemyFiring();
	}

	// If start of enemy then start enemy car sound
	if (nActiveEnemies && (nLastActiveEnemies == 0) && m_bOppsPossible)
	{
		nLastActiveEnemies = nActiveEnemies;

		// Start enemy sounds
		for(int i=0; i<nActiveEnemies; i++)
		{
			if (i == 3)
				break;
			m_pCarSnds[i]->SetPosition(Enemy[i].x, Enemy[i].z - WeaponPos.z, 0);
			m_pCarSnds[i]->Play(TRUE, TRUE, FALSE);
		}

		m_bOppsPossible = FALSE;
	}
	else
	{
		for(int i=0; i<nActiveEnemies; i++)
		{
			if (i == 3)
				break; // whoops, should only be 3 enemies max

			int nZ = Enemy[i].z - WeaponPos.z;

			// Kludge to turn off enemy engine after pass an enemy
			if (nZ < -60)
			{
				m_pCarSnds[i]->Stop();
				break;
			}

			// Set enemy engine sound in 3d space
			m_pCarSnds[i]->SetPosition(Enemy[i].x, nZ, 0);
		}
	}

	/*
	WeaponPos.nFrameDist = App.pGame->VWorld->CarStep();

	if ( lCurrentFrame <= (lStartFrame + App.pGame->VWorld->CarStop()) && lCurrentFrame >= (App.pGame->VWorld->CarStart() + lStartFrame) )
	{
		WeaponPos.z = (int)((lCurrentFrame - lStartFrame) * WeaponPos.nFrameDist);
		//Debug("z=%d\n", WeaponPos.z);
	}
	*/

}

//***********************************************************************
void CWeapon::MoveMissiles(void)
//***********************************************************************
{ // this function moves the missiles and does all the collision detection
}

//***********************************************************************
void CWeapon::MoveExplosions(void)
//***********************************************************************
{ // this function steps the explosion thru the frames of animation

	if ( !nActiveExplosions )
		return;

	for ( int i=0; i<NumExplosions; i++ )
	{// test if explosion is alive
		if ( !Explosions[i].nActive )
	    	continue;
	
		--Explosions[i].nActive;
		if (Explosions[i].nActive == 0)
		{
			--nActiveExplosions;
		}
		//Sleep(1000);
    }
}


//***********************************************************************
void CWeapon::SetWeaponPosition(int x, int y, int z) 
//***********************************************************************
{
	WeaponPos.x = x;
	WeaponPos.y = y;
	WeaponPos.z = z;
}

//***********************************************************************
void CWeapon::SetSightParams(int nLeft, int nRight) 
//***********************************************************************
{
	nSightLeft = nLeft;
	nSightRight = nRight;
	nSightWidth = 40;
	nSightDelta = nSightWidth;
	dSightSpeed = 1;

	nSightRight -= nSightWidth;
	nSightX = nSightLeft + (nRight - nLeft) / 2;
	nSightZ = 1000;

}

//***********************************************************************
int CWeapon::GetSightWorldX()
//***********************************************************************
{
	// nSightX is the view x coordinate
	//int i = (nSightVar * nSightX) / 640;
	//i = i + -(nSightVar / 2);;

	// Starting with screen coordinate x (nSightX), so reverse process to 3D world x coord
	int nViewX = nSightX - 320;
	int nDistance = nWorldLength * nZPixels;
	int x = (nViewX * nDistance / nViewDistance) / nXPixels;

	//Debug("sight pos=%d, x=%d\n", nSightX, x);
	return x;
}

//***********************************************************************
void CWeapon::SetMissileParams(WORD wRange, double dSpeed, int nNumFrames,
							   WeaponType Type)
//***********************************************************************
{
	for (int i=0; i<NumMissiles; i++)
	{
		Missiles[i].wRange = wRange;
		Missiles[i].dSpeed = dSpeed;
		Missiles[i].nFrames = nNumFrames;
		Missiles[i].Type = Type;
	}
}

//***********************************************************************
void CWeapon::MoveSightLeft()
//***********************************************************************
{
	if (nSightX > nSightLeft)
	{
		if (nSightDelta > 0)
			nSightDelta *= -1;

		nSightX += (int)((double)nSightDelta * dSightSpeed);

		if (nSightX < nSightLeft)
			nSightX = nSightLeft;

	}
}

//***********************************************************************
void CWeapon::MoveSightRight() 
//***********************************************************************
{
	if (nSightX < nSightRight)
	{
		if (nSightDelta < 0)
			nSightDelta *= -1;

		nSightX += (int)((double)nSightDelta * dSightSpeed);
		if (nSightX > nSightRight)
			nSightX = nSightRight;
	}
}

//***********************************************************************
void CWeapon::MoveSight(int x)
//***********************************************************************
{
	if (x < nSightLeft)
		x = nSightLeft;
	else if (x > nSightRight)
		x = nSightRight;

	nSightX = x;
}

//***********************************************************************
void CWeapon::IncSightSpeed()
//***********************************************************************
{
	dSightSpeed += 0.5;
	if (dSightSpeed > 8.0)
		dSightSpeed = 8.0;
}
//***********************************************************************
void CWeapon::ResetSightSpeed()
//***********************************************************************
{
	dSightSpeed = 1.0;
}

//***********************************************************************
void CWeapon::SightAutoMove()
//***********************************************************************
{

#ifdef SIGHT_KEEP_MOVING
	if (MissilesActive())
		return;
#endif

	nSightX += nSightDelta;
	if (nSightX > nSightRight || nSightX < nSightLeft)
	{
		nSightDelta *= -1;
		nSightX += nSightDelta;
	}
}

//***********************************************************************
void CWeapon::OnVideoDraw(LPBITMAPINFOHEADER lpFormat, LPTR lpData)
//***********************************************************************
{
	MoveMissiles();
	MoveExplosions();

	int i, x, y;
	if ( MissilesActive() )
	{
		//SelectObject( hDrawDC, hPen2 );
		for ( i=0; i<NumMissiles; i++ )
	    {
			if ( Missiles[i].nActive )
		    {
				x = Missiles[i].x;
				y = Missiles[i].z;

//				pdibMissile->DCBlt( hDrawDC, x, y, pdibMissile->GetWidth(), pdibMissile->GetHeight(),
//						0, 0, pdibMissile->GetWidth(), pdibMissile->GetHeight() );

/*				WinGBitBlt( hDrawDC,
					x, y,
					rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
					lpOffScreen->GetDC(),
					rDstArea.left, rDstArea.top);
*/
				//Debug( "mis (%d,%d)", x, y );
//				MoveToEx( hDrawDC, x, y, NULL );
//				LineTo( hDrawDC, x,   y+10 );
//				LineTo( hDrawDC, x+1, y+1 );
//				LineTo( hDrawDC, x+1, y   );
//				LineTo( hDrawDC, x,   y   );
		    }
		}
	}

/****
	
	if ( nActiveExplosions )
	{
		SelectObject( hDrawDC, hPen1 );
		for ( i=0; i<NumExplosions; i++ )
	    {
		    if ( Explosions[i].bActive )
			{
				x = Explosions[i].x;
				y = Explosions[i].y;
				//Debug( "exp (%d,%d)", x, y );
				MoveToEx( hDrawDC, x, y, NULL );
				LineTo( hDrawDC, x,   y+3 );
//				LineTo( hDrawDC, x+1, y+1 );
//				LineTo( hDrawDC, x+1, y   );
//				LineTo( hDrawDC, x,   y   );
			}
		}
    }
****/

}

//***********************************************************************
CRocketLauncher::CRocketLauncher() : CWeapon()
//***********************************************************************
{
	Type = MissileWeapon;
	SetMissileParams(0, 0, 0, MissileWeapon);
	nNumExpMissFrames = 0;
	nNumExpHitFrames = 0;
	nMinExpScale = 0;

	m_bSightOff = FALSE;

	int i;
	for(i=0; i<6; i++)
	{
		pdibMissile[i] = NULL;
	}

	pRocketSnd = NULL;
	pExpSmallSnd = NULL;
	pExpBigSnd = NULL;
	pAttackSnd = NULL;

	SetSightParams(20, 620);
}

//***********************************************************************
void CWeapon::DrawSight(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	int i;
	UINT uColor;

	if (!bEnabled)
		return;

	int nDestByteWidth = (((lpbi->biBitCount * (int)lpbi->biWidth) + 31) & (~31) ) / 8;
	long lDestOffset = (nDestByteWidth * (lpbi->biHeight - 1 - nVanishY)) + nSightX;

	if ( MissilesActive() )
		uColor = uSightFiringColor;
	else
		uColor = uSightColor;


	int nStart1 = nSightWidth / 2;

	// Top border
	for( i=0; i<5; i++ )
	{
		*(lpDestBits + lDestOffset + i - nStart1) = uSightBorderColor;
		*(lpDestBits + lDestOffset - i + nStart1) = uSightBorderColor;
	}

	lDestOffset -= nDestByteWidth;

	// Top color
	for( i=0; i<5; i++ )
	{
		*(lpDestBits + lDestOffset + i - nStart1 + 1) = uColor;
		*(lpDestBits + lDestOffset - i + nStart1 - 1) = uColor;
	}

	lDestOffset -= nDestByteWidth;

	// Left and right top sides
	for( i=0; i<5; i++ )
	{
		*(lpDestBits + lDestOffset - nStart1) = uSightBorderColor;
		*(lpDestBits + lDestOffset - nStart1+1) = uColor;

		*(lpDestBits + lDestOffset + nStart1) = uSightBorderColor;
		*(lpDestBits + lDestOffset + nStart1-1) = uColor;

		lDestOffset -= nDestByteWidth;
	}

	lDestOffset -= nDestByteWidth;

	// Top middle vertical bar
	for( i=0; i<5; i++ )
	{
		*(lpDestBits + lDestOffset-1) = uSightBorderColor;
		*(lpDestBits + lDestOffset) = uColor;
		*(lpDestBits + lDestOffset+1) = uSightBorderColor;
		lDestOffset -= nDestByteWidth;
	}


	// Middle horizontal bar border
	for( i=0; i<10; i++ )
	{
		*(lpDestBits + lDestOffset + i - 5) = uSightBorderColor;
	}

	lDestOffset -= nDestByteWidth;

	// Middle horizontal bar color
	for( i=0; i<10; i++ )
	{
		*(lpDestBits + lDestOffset + i - 5) = uColor;
	}

	lDestOffset -= nDestByteWidth;

	// Bottom middle vertical bar
	for( i=0; i<5; i++ )
	{
		*(lpDestBits + lDestOffset-1) = uSightBorderColor;
		*(lpDestBits + lDestOffset) = uColor;
		*(lpDestBits + lDestOffset+1) = uSightBorderColor;
		lDestOffset -= nDestByteWidth;
	}

	lDestOffset -= nDestByteWidth;

	// Bottom left and right sides
	for( i=0; i<5; i++ )
	{
		*(lpDestBits + lDestOffset - nStart1) = uSightBorderColor;
		*(lpDestBits + lDestOffset - nStart1+1) = uColor;

		*(lpDestBits + lDestOffset + nStart1) = uSightBorderColor;
		*(lpDestBits + lDestOffset + nStart1-1) = uColor;

		lDestOffset -= nDestByteWidth;
	}

	// Bottom color
	for( i=0; i<5; i++ )
	{
		*(lpDestBits + lDestOffset + i - nStart1 + 1) = uColor;
		*(lpDestBits + lDestOffset - i + nStart1 - 1) = uColor;
	}

	lDestOffset -= nDestByteWidth;

	// Bottom border
	for( i=0; i<5; i++ )
	{
		*(lpDestBits + lDestOffset + i - nStart1) = uSightBorderColor;
		*(lpDestBits + lDestOffset - i + nStart1) = uSightBorderColor;
	}
}

//***********************************************************************
CRocketLauncher::~CRocketLauncher()
//***********************************************************************
{
	for (int i=0; i<6; i++)
	{
		if (pdibMissile[i])
			delete pdibMissile[i];
	}

	if (pRocketSnd)
		delete pRocketSnd;
	if (pExpSmallSnd)
		delete pExpSmallSnd;
	if (pExpBigSnd)
		delete pExpBigSnd;
	if (pAttackSnd)
		delete pAttackSnd;
}


//***********************************************************************
void CRocketLauncher::OnVideoDraw(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{

	UpdatePosition();

	if (App.bOptionAutoTarget)
		SightAutoMove();

	// NOTE: Don't move for now.
	//MoveEnemies();

	//TestEnemyFiring();

	m_pSystem->MoveMissiles();

	MoveEnemyMissiles();

	m_pSystem->MoveExplosions();

//	if (m_bLighteningOn)
//		DrawLightening(lpbi, lpDestBits);

	DrawHorizon(lpbi, lpDestBits);

	m_pSystem->DrawExplosions(lpbi, lpDestBits);

	m_pSystem->DrawMissiles(lpbi, lpDestBits);

	DrawEnemyMissiles(lpbi, lpDestBits);

	// Crash effects
	if (m_bStrobe)
	{
		Strobe(lpbi, lpDestBits);
	}
	else if (m_nBlackOutDur)
	{
		BlackOut(lpbi, lpDestBits);
	}
	else if (m_nWhiteOutDur)
	{
		WhiteOut(lpbi, lpDestBits);
	}
	else if (m_nEffectDur)
	{
		Effect(lpbi, lpDestBits);
	}
	else if (m_nElectricFrames)
	{
		Electric(lpbi, lpDestBits);
	}

	DrawBulletCracks(lpbi, lpDestBits);
	if (!m_bSightOff)
		DrawSight(lpbi, lpDestBits);

	if (m_nScFramesOn)
		DrawScore(lpbi, lpDestBits);
	if (nActiveEnemies)
		DrawStrengths(lpbi, lpDestBits);
}

//***********************************************************************
void CRocketLauncher::DrawExplosions(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	if ( nActiveExplosions )
	{
		DrawExplosion(lpbi, lpDestBits);

		if (m_nWaitJump == 1)
		{
			// Jump to shot with-out enemy
			PostMessage( GetDlgItem(hSceneWnd, IDC_VIDEO), WM_KEYDOWN, VK_HOME, 0 );
			m_nWaitJump = 0;
		}
		else 
			if (m_nWaitJump > 1)
				--m_nWaitJump;
    }
}

//***********************************************************************
void CRocketLauncher::MoveMissiles(void)
//***********************************************************************
{ // this function moves the missiles and does all the collision detection

	if (!App.pGame)
		return;

	int i;
	for ( i=0; i<NumMissiles; i++ )
    {
		int iIndex;

	    if ( Missiles[i].nActive == ACTIVE_NO)
	    	continue;
		// missile x and z are figured as follows:
		// RM = distance from car to sight = range of missile = 175
		// CP = Car Position (or weapon position) = (0, 50)
		// SP = Sight Position = (-60, 175)
		// MT = Missile Traveled = 30 feet
		// MPX = Missile Position X coordinate = MT/RM * SPX + CPX = 30/175 * -60 + 0 = -10
		// MPZ = Missile Position Z coordinate = MT/RM * SPZ + CPZ = 30/175 * 175 + 50 = 80
		// or 30 +50 = 80
		// MP = (-6, -18)

		// NOTE: Move ratio calculation to start missile and store result
		double dDistance;
		if (Missiles[i].bBehind)
			dDistance = nSightZ * -1 + WeaponPos.z;
		else
			dDistance = nSightZ - WeaponPos.z;
		if (dDistance == 0)
			dDistance = 1;
		double Ratio = Missiles[i].wTraveled / dDistance;

		{
			// If missile behind player
			if (Missiles[i].bBehind)
			{
				Missiles[i].x = (int)(Ratio * Missiles[i].nSightX + WeaponPos.x);
				Missiles[i].z = (int)(Ratio * nWorldLength + WeaponPos.z);
			}
			else
			{
				Missiles[i].x = (int)(Ratio * Missiles[i].nSightX + WeaponPos.x);
				Missiles[i].z = (int)(Ratio * nWorldLength + WeaponPos.z);
			}
			//Missiles[i].x += Missiles[i].xStep;
			//Missiles[i].z += Missiles[i].zStep;

			// If type bomb then figure new y position
			if ( Missiles[i].Type == JackWeapon)
			{
				// Find the percentage traveled along max distance
				double fScale = (double)Missiles[i].wTraveled / 150.0;
				int iScale = (int)(fScale * 10.0);
				//if (iScale > 3)
				//	iScale -= (iScale - 3);
				//if (iScale < 0)
				//	iScale = 0;

				//Missiles[i].y = 3 + (int)pow(iScale, 2) * -1;
				if (iScale & 1)
					Missiles[i].y = 2;
				else
					Missiles[i].y = -1;
			}
		}


		Missiles[i].wTraveled += (WORD)(Missiles[i].dSpeed);
		//Debug("%d] missle x=%d, y=%d, z=%d, wTraveled=%d, Weap z=%d, ", i, 
		//	Missiles[i].x, Missiles[i].y, Missiles[i].z, Missiles[i].wTraveled, WeaponPos.z);

#ifdef _DEBUG
		if (bUseDebug)
		{
			STRING szInfo;
			wsprintf(szInfo, " M x=%d, z=%d", Missiles[i].x, Missiles[i].z);
			SetWindowText(GetParent(hSceneWnd), szInfo);
		}
#endif

		// If hit wall
		if ( HitWall(Missiles[i].x, Missiles[i].z) )
		{
			Missiles[i].nActive = ACTIVE_NO;
			//--nActiveMissiles;

			// Start small explosion
			//StartExplosion(Missiles[i].x, Missiles[i].z, Missiles[i].wTraveled);
			// Kludge for machine gun - for now (mc)
			if (Type == GunWeapon)
				Missiles[i].z = WeaponPos.z + 100;

			StartExplosion(Missiles[i].x, Missiles[i].y, Missiles[i].z, 0);

			if (m_nRepeat)
				Fire(TRUE, TRUE);
		}
		// else test if hit enemy
		else if ( (iIndex = HitEnemy( &Missiles[i] )) > -1 ) 
		{
			Missiles[i].nActive = ACTIVE_NO;
			//--nActiveMissiles;

			//StartExplosion(Missiles[i].x, 1 , Missiles[i].z, 1);			
			// Tell the world that an opponent was hit by a rocket
			if (m_nWaitJump == 0)
			{
				int iHit = 0;
				if ((iHit = App.pGame->lpWorld->HitOpponent(iIndex, m_nPower)) > 0 || iHit == -1)
				{
					// Kill 'em all for now
					for(int i=0; i<nActiveEnemies; i++)
					{
						// Call hit opponent to make sure cars get removed from engaged list.
						// Use power of 9999 to make sure they get destroyed
						App.pGame->lpWorld->HitOpponent(i, 999);

						// Stop car sound
						m_pCarSnds[i]->Stop();

						// Big explosion!!!
						StartExplosion(Enemy[i].x, Enemy[i].y, Enemy[i].z, 1);
					}

					// Start drawing the score
					StartDrawScore(App.pGame->lpWorld->GetScore(), 30);

					m_nWaitJump = 1;

					//VideoColorEffect( RGB(255, 255, 255) );
					nActiveEnemies = 0;
					nLastActiveEnemies = 0;
					m_bOppsPossible = FALSE;
					pShotSnd->Stop();

					if (m_nRepeat)
						Fire(TRUE, TRUE);
				}
				else
				{
					// Force Field Explosion
					StartExplosion(Missiles[i].x, Missiles[i].y, Enemy[iIndex].z, 2);

					if (m_nRepeat)
						Fire(TRUE, TRUE);
				}
			}
		} // end if HitEnemy

		// else missile has not hit anything
		else
		{
			// Adjust sound
			if (pRocketSnd)
				pRocketSnd->SetPosition(Missiles[i].x, Missiles[i].z, 0);
			
		}
	} // end for loop
}

//***********************************************************************
void CRocketLauncher::BigExplode()
//***********************************************************************
{
	//pExpBigSnd->Play();
	StartExplosion(0, 0, WeaponPos.z+10, 1);
}

//***********************************************************************
int CWeapon::HitEnemy(MISSILE *pMissile)
//***********************************************************************
{
	int i;
	int HalfWidth;
	int HalfDepth;

	for(i=0; i<nActiveEnemies; i++)
	{
		// see if in x
		HalfWidth = Enemy[i].w / 2;

		//Debug("Missile x=%d, Enemy left=%d right=%d", pMissile->x, Enemy[i].x-HalfWidth, Enemy[i].x+HalfWidth);
		if (pMissile->x > (Enemy[i].x-HalfWidth) && pMissile->x < (Enemy[i].x+HalfWidth) )
		{
			// If gun weapon then it is a hit, else test z
			if (pMissile->Type == GunWeapon)
			{
				//Debug("Hit!");
				return i;
			}
			else
			{
				//  test if past z
				HalfDepth = Enemy[i].d / 2;
				if (pMissile->z > (Enemy[i].z-HalfDepth) )
				{
					//Debug("Hit!");
					// Hit
					return i;
				}
				else
				{
					//Debug("Miss: Missile z=%d, Enemy depth=%d", pMissile->z, Enemy[i].z-HalfDepth);
				}
			}
		}

		// Else check if the car is real close to us
		else
		{
			if ( (abs(WeaponPos.z - Enemy[i].z) < 20) &&
				(Enemy[i].x < 30 && Enemy[i].x > -30) )
			{
				//Debug("Hit");
				return i;
			}
			//Debug("Miss");
			//Debug("Wp z=%d, Enemy z=%d. x=%d", WeaponPos.z, Enemy[i].z, Enemy[i].x);
		}
	}

	return -1;
}

//***********************************************************************
int CWeapon::HitPlayer(MISSILE *pMissile)
//***********************************************************************
{
	int HalfWidth;
	int HalfDepth;

	// see if in x
	HalfWidth = 10;

	//Debug("Player z=%d, left=%d right=%d", WeaponPos.z, 0-HalfWidth, HalfWidth);
	// use weapon pos to figure player pos - player pos x is 0 though
	if (pMissile->x > (0-HalfWidth) && pMissile->x < (HalfWidth) )
	{
		// If gun weapon then it is a hit, else test z
		if (pMissile->Type == GunWeapon)
		{
			Debug("e gun Hit!");
			StartEffect(200, 249); // red
			return 1;
		}
		else
		{
			//  test if past z
			HalfDepth = 25;
			if (pMissile->z > (WeaponPos.z-HalfDepth) && pMissile->z < (WeaponPos.z+HalfDepth))
			{
				Debug("e Hit!");
				AddBulletCrack(TRUE);
				StartEffect(200, 249); // red
				App.pGame->lEnergy -= 300;

				// Hit
				return 1;
			}
			else
				Debug("e miss z: player half depth=%d", HalfDepth);
		}
	}

	return -1;
}

//***********************************************************************
void CMissile::Open( HWND hSceneWindow )
//***********************************************************************
{
	CWeapon::Open(hSceneWindow);

	Type = MissileWeapon;
	nMinExpScale = 30;
	m_nPower = 15;

	int nFireBitmaps = LoadBitmaps(IDC_ROCKET00);
	nNumExpHitFrames = 4;
	nNumExpMissFrames = 4;
	SetMissileParams(1000, 10, nFireBitmaps, Type);

	pRocketSnd = new CDirSnd();
	pRocketSnd->Open(IDC_ROCKETSND, GetApp()->GetInstance(), TRUE);

	pExpSmallSnd = new CDirSnd();
	pExpSmallSnd->Open(IDC_EXPLSMSND, GetApp()->GetInstance(), TRUE);

	pExpBigSnd = new CDirSnd();
	pExpBigSnd->Open(IDC_EXPLBGSND, GetApp()->GetInstance(), TRUE);
}

//***********************************************************************
void CLaser::Open( HWND hSceneWindow )
//***********************************************************************
{
	CWeapon::Open(hSceneWindow);

	Type = MissileWeapon;
	nMinExpScale = 30;
	m_nPower = 20;

	int nFireBitmaps = LoadBitmaps(IDC_LBURST00);
	SetMissileParams(1000, 15, nFireBitmaps, Type);
	nNumExpHitFrames = 4;
	nNumExpMissFrames = 4;

	pRocketSnd = new CDirSnd();
	pRocketSnd->Open(IDC_LASERBOMBSND, GetApp()->GetInstance(), TRUE);

	pExpSmallSnd = new CDirSnd();
	pExpSmallSnd->Open(IDC_LASEREXPSM, GetApp()->GetInstance(), TRUE);

	pExpBigSnd = new CDirSnd();
	pExpBigSnd->Open(IDC_LASEREXPBG, GetApp()->GetInstance(), TRUE);
}

//***********************************************************************
void CScrapGun::Open( HWND hSceneWindow )
//***********************************************************************
{
	CWeapon::Open(hSceneWindow);

	Type = GunWeapon;
	nMinExpScale = 30;
	m_nPower = 3;

	int nFireBitmaps = 0; //LoadBitmaps(NULL);
	SetMissileParams(1000, 100, nFireBitmaps, Type);
	nNumExpHitFrames = 4;
	nNumExpMissFrames = 4;

	pRocketSnd = new CDirSnd();
	pRocketSnd->Open(IDC_SCRAPGUNSND, GetApp()->GetInstance());

	pExpBigSnd = new CDirSnd();
	pExpBigSnd->Open(IDC_EXPLBGSND, GetApp()->GetInstance(), TRUE);
}


//***********************************************************************
void CMachineGun::Open( HWND hSceneWindow )
//***********************************************************************
{
	CWeapon::Open(hSceneWindow);

	Type = GunWeapon;
	nMinExpScale = 30;
	m_nPower = 5;

	int nFireBitmaps = 0; //LoadBitmaps(NULL);
	SetMissileParams(1000, 100, nFireBitmaps, Type);
	nNumExpHitFrames = 4;
	nNumExpMissFrames = 4;

	pRocketSnd = new CDirSnd();
	pRocketSnd->Open(IDC_MGUNSND, GetApp()->GetInstance());

	pExpBigSnd = new CDirSnd();
	pExpBigSnd->Open(IDC_EXPLBGSND, GetApp()->GetInstance(), TRUE);
}

//***********************************************************************
void CMine::Open( HWND hSceneWindow )
//***********************************************************************
{
	CWeapon::Open(hSceneWindow);

	Type = GunWeapon;
	nMinExpScale = 30;
	m_nPower = 200;

	int nFireBitmaps = 0; 
	SetMissileParams(1000, 8, nFireBitmaps, Type);

	nNumExpHitFrames = 4;
	nNumExpMissFrames = 4;

	pRocketSnd = new CDirSnd();
	pRocketSnd->Open(IDC_MINELAUNCHSND, GetApp()->GetInstance());

	pExpBigSnd = new CDirSnd();
	pExpBigSnd->Open(IDC_EXPLBGSND, GetApp()->GetInstance(), TRUE);
}

//***********************************************************************
void CBomb::Open( HWND hSceneWindow )
//***********************************************************************
{
	CWeapon::Open(hSceneWindow);

	Type = MissileWeapon;
	nMinExpScale = 30;
	m_nPower = 100;

	int nFireBitmaps = LoadBitmaps( IDC_SPINEKILL00 );
	SetMissileParams(1000, 20, nFireBitmaps, BombWeapon);

	m_pSpineKiller = this;

	nNumExpHitFrames = 4;
	nNumExpMissFrames = 4;

	pRocketSnd = new CDirSnd();
	pRocketSnd->Open(IDC_SPINELAUNCHSND, GetApp()->GetInstance(), TRUE);

	pExpSmallSnd = new CDirSnd();
	pExpSmallSnd->Open(IDC_EXPLSMSND, GetApp()->GetInstance(), TRUE);

	pExpBigSnd = new CDirSnd();
	pExpBigSnd->Open(IDC_EXPLBGSND, GetApp()->GetInstance(), TRUE);
}

//***********************************************************************
CBomb::~CBomb()
//***********************************************************************
{
	m_pSpineKiller = NULL;
}

//***********************************************************************
void CJack::Open( HWND hSceneWindow )
//***********************************************************************
{
	CWeapon::Open(hSceneWindow);

	Type = JackWeapon;
	nMinExpScale = 30;
	m_nPower = 50;

	int nFireBitmaps = LoadBitmaps( IDC_JACK00 );
	SetMissileParams(1000, 8, nFireBitmaps, JackWeapon);

	nNumExpHitFrames = 4;
	nNumExpMissFrames = 4;

	pRocketSnd = new CDirSnd();
	pRocketSnd->Open(IDC_JACKSND, GetApp()->GetInstance(), TRUE);

	pExpSmallSnd = new CDirSnd();
	pExpSmallSnd->Open(IDC_EXPLSMSND, GetApp()->GetInstance(), TRUE);

	pExpBigSnd = new CDirSnd();
	pExpBigSnd->Open(IDC_EXPLBGSND, GetApp()->GetInstance(), TRUE);

	pAttackSnd = new CDirSnd();
	pAttackSnd->Open(IDC_JACKATTSND, GetApp()->GetInstance(), TRUE);
}

//***********************************************************************
void CRocketLauncher::DrawMissiles(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	if ( MissilesActive() )
	{
		int i;
		for ( i=0; i<NumMissiles; i++ )
	    {
			if ( Missiles[i].nActive != ACTIVE_NO )
		    {
				int nScaleFactor = 275;
				//int nDist = Missiles[i].wTraveled;
				int nPercent;
				int nDist = (int)sqrt( pow(Missiles[i].x - WeaponPos.x, 2) + pow(Missiles[i].z - WeaponPos.z, 2) );

				POINT Point = Point3DToPointDib(Missiles[i].x, Missiles[i].y, Missiles[i].z - WeaponPos.z);

				//Debug("missile x=%d, y=%d, z=%d, px=%d, py=%d", Missiles[i].x, Missiles[i].y, Missiles[i].z, Point.x, Point.y);
				// Figure scale percentage
				nPercent = 100 - (int)(((float)(nDist)*100) / nScaleFactor) ;
				//nPercent = 200 - nDist;
				//if (nPercent > 100) {
				//	nPercent = 100;
				//}

				// If the missile is ridiculously small then get rid of if
				if (nPercent < 10)
				{
					if (Type != HoverWeapon && Type != AerialWeapon)
					{
						Missiles[i].nActive = ACTIVE_NO;
						// Start small explosion
						StartExplosion(Missiles[i].x, Missiles[i].y, Missiles[i].z, 0);
						if (m_nRepeat)
							Fire(TRUE, TRUE);
					}
					else
						nPercent = 10;
				}
				else
				{
					if (Missiles[i].nFrames)
					{
						// Only draw the missile if it is not behind an enemy car
						if (!BehindEnemy(Missiles[i].z, Point))
							DrawSprite(nPercent, Point.x, Point.y, pdibMissile[Missiles[i].nCurFrame], lpbi, lpDestBits);
					}

					if (Type != HoverWeapon)
					{
						++Missiles[i].nCurFrame;
						if (Missiles[i].nCurFrame >= Missiles[i].nFrames)
							Missiles[i].nCurFrame = 0;
					}
				}
		    }
		}
	}
}

//***********************************************************************
BOOL CWeapon::BehindEnemy(int z, POINT Point)
//***********************************************************************
{
	// Loop through enemies and see if point is hidden
	for(int i=0; i<nActiveEnemies; i++)
	{
		// If enemy behind player then continue
		if (Enemy[i].z < WeaponPos.z)
			continue;

		int HalfDepth = Enemy[i].d / 2;

		// If point in front of enemy then continue
		if ( z < (Enemy[i].z - HalfDepth) )
			continue;
		
		int HalfWidth = Enemy[i].w / 2;
		int HalfHeight = Enemy[i].h / 2;

		// Set enemy screen rectangle points
		POINT ul = Point3DToPointDib(Enemy[i].x - HalfWidth, Enemy[i].y - HalfHeight, Enemy[i].z - WeaponPos.z);
		POINT ur = Point3DToPointDib(Enemy[i].x + HalfWidth, Enemy[i].y - HalfHeight, Enemy[i].z - WeaponPos.z);
		POINT ll = Point3DToPointDib(Enemy[i].x - HalfWidth, Enemy[i].y + HalfHeight, Enemy[i].z - WeaponPos.z);

		// If point is in rectangle then it is behind the enemy
		if (Point.x > ul.x && Point.x < ur.x && Point.y > ul.y && Point.y < ll.y)
			return TRUE;
	}

	return FALSE;
}


//***********************************************************************
void CRocketLauncher::DrawEnemyMissiles(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	int i;
	int nPercent;

	for ( i=0; i<NumMissiles; i++ )
	{
		if ( EnemyMissiles[i].nActive )
		{
			int nScaleFactor = 275;
			int nDist = (int)sqrt( pow(EnemyMissiles[i].x, 2) + pow(EnemyMissiles[i].z - WeaponPos.z, 2) );

			POINT Point = Point3DToPointDib(EnemyMissiles[i].x, EnemyMissiles[i].y, EnemyMissiles[i].z - WeaponPos.z);

			//Debug("e draw missile x=%d, z=%d, px=%d, py=%d", EnemyMissiles[i].x, EnemyMissiles[i].z - WeaponPos.z, Point.x, Point.y);

			// Figure scale percentage
			nPercent = 130      - (int)(((float)(nDist)*100) / nScaleFactor) ;
			//nPercent = 200 - nDist;

			// If the missile is ridiculously small then don't show it
			if (nPercent > 10)
			{
				if (!m_pSpineKiller)
					return;

				//Debug("nPercent=%d", nPercent);
				if (EnemyMissiles[i].nFrames)
				{
					DrawSprite(nPercent, Point.x, Point.y, 
					m_pSpineKiller->pdibMissile[EnemyMissiles[i].nCurFrame], 
					lpbi, lpDestBits);
				}

				if (Type != HoverWeapon)
				{
					++EnemyMissiles[i].nCurFrame;
					if (EnemyMissiles[i].nCurFrame >= EnemyMissiles[i].nFrames)
						EnemyMissiles[i].nCurFrame = 0;
				}
			}
		}
	}
}

//***********************************************************************
void CWeapon::DrawHorizon(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	int i;
	
	if (!bEnabled)
		return;

	Draw3DGrid(lpbi, lpDestBits);

	return;

	int nDestByteWidth = (((lpbi->biBitCount * (int)lpbi->biWidth) + 31) & (~31) ) / 8;
	
	int nDestOffset = (lpbi->biHeight - nVanishY - 1) * nDestByteWidth - 1;
	
	for( i=0; i<lpbi->biWidth; i++ )
	{
		*(lpDestBits + nDestOffset + i) = uSightColor;
	}

	int nMid = lpbi->biWidth / 2;
	nDestOffset = nMid;

	for( i=0; i<lpbi->biHeight; i++ )
	{
		*(lpDestBits + nDestOffset) = uSightColor;
		nDestOffset += nDestByteWidth;
	}
}

//***********************************************************************
void CWeapon::Draw3DGrid(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	if (!bShowGrid)
		return;

	// For all the walls
	for(int i=0; i<nNumWalls; i++)
		DrawWall(&Walls[i], lpbi, lpDestBits);
	
	DrawEnemy(lpbi, lpDestBits);

	return;
}

//***********************************************************************
void CWeapon::DrawWall(WALL *pWall, LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	POINT p1, p2, p3, p4;
	int x, y, z;

	// lower left corner
	x = pWall->p1.x;
	y = yGround; 
	z = pWall->p1.z - WeaponPos.z;
	p1 = Point3DToPointDib(x, y, z);

	// upper left corner
	y = yGround - 12;
	p2 = Point3DToPointDib(x, y, z);

	// upper right corner
	x = pWall->p2.x;
	z = pWall->p2.z - WeaponPos.z;
	p3 = Point3DToPointDib(x, y, z);

	// lower right corner
	y = yGround;
	p4 = Point3DToPointDib(x, y, z);

	// Draw lines
	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);
	DrawDibLine(p2.x, p2.y, p3.x, p3.y, lpbi, lpDestBits, uGridColor);
	DrawDibLine(p3.x, p3.y, p4.x, p4.y, lpbi, lpDestBits, uGridColor);
	DrawDibLine(p4.x, p4.y, p1.x, p1.y, lpbi, lpDestBits, uGridColor);
}

//***********************************************************************
BOOL CWeapon::HitWall(int x, int z)
//***********************************************************************
{
	int zTest1, zTest2;
//	char debug[120];

	// For all walls
	for(int i=0; i<nNumWalls; i++)
	{
		// First, see if within x (assumes p1.x is less than p2.x)
		if ( x >= (Walls[i].p1.x) && x <= (Walls[i].p2.x) )
		{
			if (Walls[i].p1.z < Walls[i].p2.z)
			{
				zTest1 = Walls[i].p1.z;
				zTest2 = Walls[i].p2.z;
			}
			else
			{	
				zTest1 = Walls[i].p2.z;
				zTest2 = Walls[i].p1.z;
			}

			// See if within z or z + 20 (somewhat arbitrary wall depth
			if ( z >= zTest1 && z <= zTest2 + 20 )
			{
				// See if on or to the left of the line (pass the line)
				if (PointHitLine(Walls[i].L2, 
					Walls[i].p1.x, Walls[i].p1.z,
					Walls[i].p2.x, Walls[i].p2.z,
					x, z))
				{
#ifdef _DEBUG
					if (bUseDebug)
					{
						STRING szInfo;
						wsprintf(szInfo, " HitWall=%d, x=%d, z=%d", i, Missiles[i].x, Missiles[i].z);
						SetWindowText(GetParent(hSceneWnd), szInfo);
					}
#endif

					//Debug("Hit:x=%d, z=%d Wall=%d\n", x, z, i);
					return TRUE;
				}
				else
				{
					//Debug("No hit: x=%d, z=%d\n", x, z );
				}
			}
			else
			{
				//Debug("out z: x=%d, z=%d\n", x, z);
			}
		} // end test x
		//else
		//{
		//	Debug("out x: x=%d, z=%d\n", x, z);
		//}
	}

	return FALSE;
}

//***********************************************************************
void CWeapon::SetShowGrid(BOOL bShow)
//***********************************************************************
{
	bShowGrid = bShow;
}

//***********************************************************************
void CWeapon::SetWorld(int _nNumWalls, WALL *_pWalls, int nYGround)
//***********************************************************************
{
	//nWorldLength = nLength;
	//nWorldWidth = nWidth;
	nNumWalls = _nNumWalls;
	memcpy(Walls, _pWalls, sizeof(WALL) * nNumWalls);
	yGround = nYGround;

	// Compute L2 (length squared) for all walls
	for(int i=0; i<nNumWalls; i++)
	{
		Walls[i].L2 = LineLengthSquared( Walls[i].p1.x, Walls[i].p1.z,
			Walls[i].p2.x, Walls[i].p2.z );
	}
}

//***********************************************************************
//void CWeapon::SetEnemy(int i, int nFrameDist, int nStartX, int nStartY, int nStartZ, 
//					   int nEndX, int nEndZ, int nWidth, int nHeight, int nDepth)
//***********************************************************************
//{
//	nActiveEnemies = 1;

	//Enemy[i].nFrameDist = nFrameDist;
	//Enemy[i].StartX = nStartX;
	//Enemy[i].StartZ = nStartZ;
	//Enemy[i].EndX = nEndX;
	//Enemy[i].EndZ = nEndZ;
//	Enemy[i].x = x;
//	Enemy[i].z = z
//	Enemy[i].y = y;
//	Enemy[i].Width = nWidth;
//	Enemy[i].Height = nHeight;
//	Enemy[i].Depth = nDepth;
	//Enemy[i].dDistance = sqrt( pow(nEndX - nStartX, 2) + pow(nEndZ - nStartZ, 2) );
	//Enemy[i].wTraveled = 0;
//	Enemy[i].nShotCount = 4;
//}

//***********************************************************************
void CWeapon::ClearEnemies()
//***********************************************************************
{
	nActiveEnemies = 0;
}

//***********************************************************************
void CWeapon::MoveEnemies()
//***********************************************************************
{
	return;

	//if ( lCurrentFrame > (lStartFrame + App.pGame->VWorld->CarStop()) || lCurrentFrame < (lStartFrame + App.pGame->VWorld->CarStart()) )
	//	return;

	//int i;
	//for( i=0; i<nActiveEnemies; i++)
	//{
		// Adjust distance traveled (based on constant rate per frame)
	//	long lCurrentFrame = MCIGetPosition(lpVideo->hDevice);
		//long lTemp = App.pGame->VWorld->CarStart();
		//Enemy[i].wTraveled = (WORD)(Enemy[i].nFrameDist * (lCurrentFrame - lStartFrame - App.pGame->VWorld->CarStart()));

		// Compute new enemy position based on start, distance traveled, and distance to travel (dDistance)
	//	double dRatio = (double)(Enemy[i].dDistance - Enemy[i].wTraveled) / Enemy[i].dDistance;
	//	Enemy[i].x = (int)(dRatio * (double)Enemy[i].StartX) + Enemy[i].EndX;
	//	Enemy[i].z = (int)(dRatio * (double)Enemy[i].StartZ) + Enemy[i].EndZ;

	//Debug("enemy x=%d, z=%d\n", Enemy[i].x, Enemy[i].z);
	//}
	
}

//***********************************************************************
void CWeapon::DrawEnemy(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	int x1, y1, z1;
	int x2, y2, z2;
	int i = 0;
	POINT p1, p2, p3, p4;

	if (!nActiveEnemies)
		return;

	for (i=0; i<nActiveEnemies; i++)
	{
		z1 = z2 = (Enemy[i].z - (Enemy[i].d/2)) - WeaponPos.z;

		// bottom points
		x1 = Enemy[i].x + (Enemy[i].w / 2);
		x2 = Enemy[i].x - (Enemy[i].w / 2);
		y1 = y2 = Enemy[i].y + (Enemy[i].h / 2);
		p1 = Point3DToPointDib(x1, y1, z1);
		p2 = Point3DToPointDib(x2, y2, z2);

		// top points`
		y1 = y2 = Enemy[i].y - (Enemy[i].h / 2);
		p3 = Point3DToPointDib(x1, y1, z1);
		p4 = Point3DToPointDib(x2, y2, z2);

		// ** Front side
		// bottom
		DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uEnemyBoxColor);
		// left
		DrawDibLine(p2.x, p2.y, p4.x, p4.y, lpbi, lpDestBits, uEnemyBoxColor);
		// top
		DrawDibLine(p4.x, p4.y, p3.x, p3.y, lpbi, lpDestBits, uEnemyBoxColor);
		// right
		DrawDibLine(p3.x, p3.y, p1.x, p1.y, lpbi, lpDestBits, uEnemyBoxColor);

		// ** Back Side
		z1 = z2 = (Enemy[i].z + (Enemy[i].d/2)) - WeaponPos.z;
		y1 = y2 = Enemy[i].y + (Enemy[i].h / 2);
		p1 = Point3DToPointDib(x1, y1, z1);
		p2 = Point3DToPointDib(x2, y2, z2);
		y1 = y2 = Enemy[i].y - (Enemy[i].h / 2);
		p3 = Point3DToPointDib(x1, y1, z1);
		p4 = Point3DToPointDib(x2, y2, z2);

		// bottom
		DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uEnemyBoxColor);
		// left
		DrawDibLine(p2.x, p2.y, p4.x, p4.y, lpbi, lpDestBits, uEnemyBoxColor);
		// top
		DrawDibLine(p4.x, p4.y, p3.x, p3.y, lpbi, lpDestBits, uEnemyBoxColor);
		// right
		DrawDibLine(p3.x, p3.y, p1.x, p1.y, lpbi, lpDestBits, uEnemyBoxColor);

		// ** Left Side
		z1 = (Enemy[i].z - (Enemy[i].d/2)) - WeaponPos.z;
		z2 = (Enemy[i].z + (Enemy[i].d/2)) - WeaponPos.z;
		x1 = x2 = Enemy[i].x - (Enemy[i].w / 2);
		y1 = y2 = Enemy[i].y + (Enemy[i].h / 2);
		p1 = Point3DToPointDib(x1, y1, z1);
		p2 = Point3DToPointDib(x2, y2, z2);
		y1 = y2 = Enemy[i].y - (Enemy[i].h / 2);
		p3 = Point3DToPointDib(x1, y1, z1);
		p4 = Point3DToPointDib(x2, y2, z2);
		// top
		DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uEnemyBoxColor);
		// bottom
		DrawDibLine(p4.x, p4.y, p3.x, p3.y, lpbi, lpDestBits, uEnemyBoxColor);

		// ** Right Side
		z1 = (Enemy[i].z - (Enemy[i].d/2)) - WeaponPos.z;
		z2 = (Enemy[i].z + (Enemy[i].d/2)) - WeaponPos.z;
		x1 = x2 = Enemy[i].x + (Enemy[i].w / 2);
		y1 = y2 = Enemy[i].y + (Enemy[i].h / 2);
		p1 = Point3DToPointDib(x1, y1, z1);
		p2 = Point3DToPointDib(x2, y2, z2);
		y1 = y2 = Enemy[i].y - (Enemy[i].h / 2);
		p3 = Point3DToPointDib(x1, y1, z1);
		p4 = Point3DToPointDib(x2, y2, z2);
		// top
		DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uEnemyBoxColor);
		// bottom
		DrawDibLine(p4.x, p4.y, p3.x, p3.y, lpbi, lpDestBits, uEnemyBoxColor);

		
		// ** Middle Square
		z1 = z2 = Enemy[i].z - WeaponPos.z;

		// bottom points
		x1 = Enemy[i].x + (Enemy[i].w / 2);
		x2 = Enemy[i].x - (Enemy[i].w / 2);
		y1 = y2 = Enemy[i].y + (Enemy[i].h / 2);
		p1 = Point3DToPointDib(x1, y1, z1);
		p2 = Point3DToPointDib(x2, y2, z2);

		// top points
		y1 = y2 = Enemy[i].y - (Enemy[i].h / 2);
		p3 = Point3DToPointDib(x1, y1, z1);
		p4 = Point3DToPointDib(x2, y2, z2);

		// ** Front side
		// bottom
		DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uEnemyColor);
		// left
		DrawDibLine(p2.x, p2.y, p4.x, p4.y, lpbi, lpDestBits, uEnemyColor);
		// top
		DrawDibLine(p4.x, p4.y, p3.x, p3.y, lpbi, lpDestBits, uEnemyColor);
		// right
		DrawDibLine(p3.x, p3.y, p1.x, p1.y, lpbi, lpDestBits, uEnemyColor);
	}
}

//***********************************************************************
void CWeapon::DrawPoint(int x, int y, LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	int nDestByteWidth = (((lpbi->biBitCount * (int)lpbi->biWidth) + 31) & (~31) ) / 8;

	if (x < 0 || x > 640)
		return;

	if (y < 0 || y > 296)
		return;

	int nDestOffset = (lpbi->biHeight - y - 1) * nDestByteWidth - 1 + x;

	*(lpDestBits + nDestOffset) = uGridColor;

}

//***********************************************************************
POINT CWeapon::Point3DToPointDib(int x, int y, int z)
//***********************************************************************
{
	POINT Point;

	// Clip z
	if ( z < 1 )
		z = 1;

	// Convert feet to pixel space
	x *= nXPixels;	
	y *= nYPixels;
	z *= nZPixels;
	
	// Kludge - fix divide by 0 error
	if (z == 0)
		z = 1;

	// Transform point to 2D view
	int nViewX = nViewDistance * x / z;
	int nViewY = nViewDistance * y / z;

	// Offset coordinates to bitmap position (in the world the center is 0,0)
	Point.x = 320 + nViewX;
	Point.y = nVanishY + nViewY;

	//Debug("x=%d,y=%d,z=%d nViewX=%d, nViewY=%d, px=%d, py=%d\n", x, y, z, nViewX, nViewY, Point.x, Point.y);

	return Point;
}

//***********************************************************************
void CWeapon::DrawDibLine(int x1, int y1, int x2, int y2, LPBITMAPINFOHEADER lpbi, LPTR lpDestBits, UINT uColor)
//***********************************************************************
{
	// First clip
	if ( ClipLine(&x1, &y1, &x2, &y2, lpbi) == FALSE)
		return;	// Totally outside the bitmap

	int xUnit, yUnit;
	int nDestByteWidth = (((lpbi->biBitCount * (int)lpbi->biWidth) + 31) & (~31) ) / 8;
	int nMax = nDestByteWidth * lpbi->biHeight;

	long nDestOffset = (lpbi->biHeight - y1 - 1) * nDestByteWidth + x1;

	int yDiff = y2 - y1;

	if (yDiff < 0 )
	{
		yDiff = -yDiff;		    // Absolute value
		yUnit = nDestByteWidth;	// Negative y direction means we will be adding to the offset (bitmaps are bottom-up)
	}
	else
	{
		yUnit = -nDestByteWidth;
	}

	int xDiff = x2 - x1;

	if (xDiff < 0 )
	{
		xDiff = -xDiff;
		xUnit = -1;
	}
	else 
	{
		xUnit = 1;
	}

	int nErrTerm = 0;

	if (xDiff > yDiff)
	{
		int nLength=xDiff+1;

		for(int i=0; i<nLength; i++)
		{
			if (nDestOffset >= nMax || nDestOffset < 0)
				DebugBreak();

			*(lpDestBits + nDestOffset) = uColor;
			
			nDestOffset += xUnit;
			nErrTerm += yDiff;

			if (nErrTerm > xDiff)
			{
				nErrTerm -= xDiff;
				nDestOffset += yUnit;
			}
		}
	}
	else
	{
		int nLength = yDiff+1;

		for(int i=0; i<nLength; i++)
		{
			if (nDestOffset >= nMax || nDestOffset < 0)
				DebugBreak();

			*(lpDestBits + nDestOffset) = uColor;

			nDestOffset += yUnit;

			nErrTerm += xDiff;
			if (nErrTerm > 0)
			{
				nErrTerm -= yDiff;
				nDestOffset += xUnit;
			}
		}
	}
}

//***********************************************************************
WORD CWeapon::ClipCode(int x, int y, LPBITMAPINFOHEADER lpbi)
//***********************************************************************
{
	WORD wFlag = 0;				// Assume no intersections

	if (x < 0) 
		wFlag = 1;			// Crosses left edge

	if (x >= lpbi->biWidth) 
		wFlag |= 2;			// Crosses right edge

	if (y < 0)
		wFlag |= 4;			// Crosses top edge

	if (y >= lpbi->biHeight)
		wFlag |= 8;			// Crosses bottom edge

	return wFlag;
}

//***********************************************************************
BOOL CWeapon::ClipLine(int *px1, int *py1, int *px2, int *py2, LPBITMAPINFOHEADER lpbi)
//***********************************************************************
{
	WORD wFlag1 = ClipCode(*px1, *py1, lpbi);
	WORD wFlag2 = ClipCode(*px2, *py2, lpbi);

	// Repeat until no more clipping is necessary
	while (TRUE)
	{
		// Check for acceptance of both endpoints or rejection of both
		if ((wFlag1 | wFlag2) == 0)
		{
			if (*py1 == 420  || *py2 == 420)
				DebugBreak();
			return TRUE; // Both endpoints in bitmap
		}

		if (wFlag1 & wFlag2) 
			return FALSE; // Both endpoints outside of bitmap

		// Pick an endpoint to clip. To simplify clip code, always clip the 2nd endpoint.  If
		// 2nd endpoint is already inside bitmap, swap the two endpoints so that we're really 
		// clipping endpoint 1

		if (wFlag2 == 0)
		{
			// swap x coords
			int nTemp = *px1;
			*px1 = *px2;
			*px2 = nTemp;

			// swap y coords
			nTemp = *py1;
			*py1 = *py2;
			*py2 = nTemp;

			// swap codes
			WORD wFlag = wFlag1;
			wFlag1 = wFlag2;
			wFlag2 = wFlag;
		}

		// If either of two low bits of clipcode set, we'll clip agains
		// left or right this time.  Comput intersection with the appropriate side
		// and set new endpoint x,y.

		if (wFlag2 & 3)		// Clip agains left or right side?
		{
			int nEdgeX;

			if (wFlag2 & 1)
				nEdgeX = 0;					// clip left
			else
				nEdgeX = lpbi->biWidth-1;	// else clip right

			// compute slope of the line
			double Slope = (double)(*py2 - *py1) / (double)(*px2 - *px1);
			*py2 = *py1 + (int)(Slope * (nEdgeX - *px1));	// calc y-crossing & set y2
			*px2 = nEdgeX;								// set x2
		}

		// Else clip against top or bottom.  Comput intersection with the appropriate
		// side and set new endpoint x,y
		else
		{
			int nEdgeY;
			if (wFlag2 & 4)
				nEdgeY = 0;					// clip top
			else
				nEdgeY = lpbi->biHeight-1;	// clip bottom

			// compute slope of the line
			double Slope = (double)(*px2 - *px1) / (double)(*py2 - *py1);	

			*px2 = *px1 + (int)(Slope * (nEdgeY - *py1));	// calc x-crossing & set x2
			*py2 = nEdgeY;
		}
			

		wFlag2 = ClipCode(*px2, *py2, lpbi);
	}

	return FALSE;
}

//***********************************************************************
void CWeapon::SetViewDistance(int nDistance)
//***********************************************************************
{
	nViewDistance = nDistance;
}

//***********************************************************************
void CWeapon::SetXPixels(int nPixels)
//***********************************************************************
{
	nXPixels = nPixels;
}

//***********************************************************************
void CWeapon::SetYPixels(int nPixels)
//***********************************************************************
{
	nYPixels = nPixels;
}

//***********************************************************************
void CWeapon::SetZPixels(int nPixels)
//***********************************************************************
{
	nZPixels = nPixels;
}

//***********************************************************************
void CWeapon::SetGroundY(int nPos)
//***********************************************************************
{
	yGround = nPos;
}

//***********************************************************************
void CWeapon::SetGridLength(int nLength)
//***********************************************************************
{
	nWorldLength = nLength;
}

//***********************************************************************
void CWeapon::SetGridWidth(int nWidth)
//***********************************************************************
{
	nWorldWidth = nWidth;
}

//***********************************************************************
void CWeapon::SetExpScaleFactor(int nFactor)
//***********************************************************************
{
	nExpScaleFactor = nFactor;
}

//***********************************************************************
void CWeapon::InitBulletCracks()
//***********************************************************************
{
	int i, j, nIndex;
	nNumCracks = 0;
	RECT VidRect;
	int xDelta, yDelta;
	int xVariance, xVarMid, xVarRange;
	int yVariance, yVarMid, yVarRange;

	HWND hVideo = GetDlgItem( hSceneWnd, IDC_VIDEO );
	if (!hVideo)
		return;

	GetClientRect(hVideo, &VidRect);

	xDelta = (VidRect.right - VidRect.left) / 5;
	yDelta = (VidRect.bottom - VidRect.top) / 4;

	xVarRange = xDelta / 2;
	xVarMid = xVarRange / 2;
	yVarRange = yDelta / 2;
	yVarMid = yVarRange / 2;

	nIndex = 0;
	for(i=0; i<4; i++)
	{
		for(j=0; j<5; j++)
		{
			xVariance = GetRandomNumber(xVarRange) - xVarMid;
			yVariance = GetRandomNumber(yVarRange) - yVarMid;
			Cracks[nIndex].x = xDelta * j + xVariance;
			Cracks[nIndex].y = yDelta * i + yVariance;
			Cracks[nIndex].nType = 0;	// No crack for now
			++nIndex;
		}
	}
}

//***********************************************************************
void CWeapon::AddBulletCrack(BOOL bSnd)
//***********************************************************************
{

	if (m_nCrackCount > 6)
		return;

	++m_nCrackCount;

	int i, nIndex;
	int nRand = GetRandomNumber(MaxCracks-1);
	int nType = GetRandomNumber(2)+1;

	if (nType > 2)
		nType = 2;

	if (Cracks[nRand].nType == 0)
	{
		if (pCrackSnd)
			pCrackSnd->Play();
		//else
		Cracks[nRand].nType = nType;
		return;
	}
	else
	{
		i=0;
		nIndex = nRand + 1;
		while(i < (MaxCracks-1))
		{
			if (nIndex >= MaxCracks)
				nIndex = 0;

			if (Cracks[nIndex].nType == 0)
			{
				if (pCrackSnd)
					pCrackSnd->Play();
				else
					Cracks[nIndex].nType = nType;
				return;
			}
			++i;
			++nIndex;
		}
	}
}

//***********************************************************************
void CWeapon::ClearBulletCracks()
//***********************************************************************
{
	for(int i=0; i<MaxCracks; i++)
	{
		Cracks[i].nType = 0;
	}

	m_nCrackCount = 0;
}

//***********************************************************************
void CWeapon::DrawBulletCracks(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	for(int i=0; i<MaxCracks; i++)
	{
		if (Cracks[i].nType == 1)
		{
			DrawSprite(100, Cracks[i].x, Cracks[i].y, pdibCracks[0], lpbi, lpDestBits);
		}
		else
		if (Cracks[i].nType == 2)
		{
			DrawSprite(100, Cracks[i].x, Cracks[i].y, pdibCracks[1], lpbi, lpDestBits);
		}
	}

}

//***********************************************************************
void CWeapon::StartDrawScore(long lScore, int nFramesOn)
//***********************************************************************
{
	m_lScore = lScore;
	m_nScFramesOn = nFramesOn;

	_ltoa(lScore, m_szScore, 10 );

	int iLen = lstrlen(m_szScore);
	int idx;
	if (iLen >= MaxScoreLen)
		iLen = 0;
	int i;
	for(i=0; i<iLen; i++)
	{
		idx = m_szScore[i] - '0';
		if (idx >= 0 && idx <= 9)
			m_pdibScore[i] = m_pdibDigits[idx];
		else
			m_pdibScore[i] = m_pdibDigits[0];
	}

	m_nScoreLen = i;

	m_nScWidth = m_pdibDigits[0]->GetWidthBytes() + 2;

	m_nScStartX = 640 - 5 - (m_nScWidth * m_nScoreLen);
	if (m_nScStartX < 0)
		m_nScStartX = 0;
}

//***********************************************************************
void CWeapon::DrawScore(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	if (!m_nScFramesOn)
		return;

	int x = m_nScStartX;
	int y = 15;

	// For all the digits
	for(int i=0; i<m_nScoreLen; i++)
	{
		DrawSprite(100, x, y, m_pdibScore[i], lpbi, lpDestBits);
		x += m_nScWidth;
	}

	--m_nScFramesOn;
}

//***********************************************************************
void CWeapon::DrawStrengths(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	if (!App.pGame || m_bTextOn)
		return;

	VideoFx *pfx = App.pGame->pVidFx;

	if (nActiveEnemies && pfx && !pfx->TextUp())
	{
		STRING szStrength;

		int iStrength = App.pGame->lpWorld->GetGroup()->GetStrength(0);
		char *pszName = App.pGame->lpWorld->GetGroup()->GetName(0);
		char *pszUnknown = "unknown";

		if (nActiveEnemies == 1)
		{
			if (!pszName)
				pszName = pszUnknown;

			wsprintf(szStrength, "%s:%d", pszName, iStrength);
		}
		else if (nActiveEnemies == 2)
		{
			if (!pszName)
				pszName = pszUnknown;

			int iStrength2 = App.pGame->lpWorld->GetGroup()->GetStrength(1);
			char *pszName2 = App.pGame->lpWorld->GetGroup()->GetName(1);

			if (!pszName2)
				pszName2 = pszUnknown;

			wsprintf(szStrength, "%s:%d, %s:%d", pszName, iStrength, pszName2, iStrength2);
		}
		else if (nActiveEnemies == 3)
		{
			if (!pszName)
				pszName = pszUnknown;

			int iStrength2 = App.pGame->lpWorld->GetGroup()->GetStrength(1);
			char *pszName2 = App.pGame->lpWorld->GetGroup()->GetName(1);

			if (!pszName2)
				pszName2 = pszUnknown;

			int iStrength3 = App.pGame->lpWorld->GetGroup()->GetStrength(2);
			char *pszName3 = App.pGame->lpWorld->GetGroup()->GetName(2);

			if (!pszName3)
				pszName3 = pszUnknown;

			wsprintf(szStrength, "%s:%d, %s:%d, %s:%d", pszName, iStrength, 
				pszName2, iStrength2, pszName3, iStrength3);
		}

		m_bTextOn = TRUE;

		pfx->TextOn(hSceneWnd, szStrength, RGB(255, 255, 255), 18, 4000, 3);
	}

	return;

	if (nActiveEnemies)
	{
		int y = 270;
		int x;

		m_nScWidth = m_pdibDigits[0]->GetWidthBytes() + 2;

		if (nActiveEnemies == 1)
			x = 320 - m_nScWidth;
		else
		if (nActiveEnemies == 2)
			x = 320 - (m_nScWidth * 3);
		else
		if (nActiveEnemies == 3)
			x = 320 - (m_nScWidth * 5);

		m_nScWidth = m_pdibDigits[0]->GetWidthBytes() + 2;

		for(int i=0; i<nActiveEnemies; i++)
		{
			int iStrength = App.pGame->lpWorld->GetGroup()->GetStrength(i);
			if (iStrength < 0)
				continue;

			if (iStrength < 10)
			{
				DrawSprite(100, x, y, m_pdibDigits[iStrength], lpbi, lpDestBits);
				x += m_nScWidth;
				x += m_nScWidth;
			}
			else
			if (iStrength < 100)
			{
				int iTens = iStrength / 10;		
				DrawSprite(100, x, y, m_pdibDigits[iTens], lpbi, lpDestBits);
				x += m_nScWidth;

				int iOnes = iStrength - (iTens * 10);
				DrawSprite(100, x, y, m_pdibDigits[iOnes], lpbi, lpDestBits);
				x += m_nScWidth;
			}
		}
	}
}

//***********************************************************************
BOOL CWeapon::TestEnemyFiring()
//***********************************************************************
{
	if (!App.pGame)
		return FALSE;

	int i;

	if (!App.bOptionFireBack || !App.pGame->lpWorld)
		return FALSE;

	for(i=0; i<nActiveEnemies; i++)
	{
		//nDistance = Enemy[i].z - WeaponPos.z;

		// If the enemy is in shooting range
		//if (nDistance < 130 && nDistance > 90)		
		//{
		// If this is the type of enemy to shoot back
		int iClass;
		if (iClass = App.pGame->lpWorld->GetGroup()->EngagedShooter(i))
		{
			// If it is time for the enemy to fire his gun
			if (EnemyShotSpace[i].nShotWait == 0)
			{
				EnemyShotSpace[i].nShotWait = 0;

				if (iClass == OPPCLASS_CONDRONE || 
					iClass == OPPCLASS_DOGGER ||
					iClass == OPPCLASS_DREADNAUGHT)
				{
					if (pShotSnd)
						pShotSnd->Play();
					AddBulletCrack(TRUE);
					App.pGame->lEnergy -= 200;
					StartEffect(200, 249); //red

					// special case for dreadnaught
					if (iClass == OPPCLASS_DREADNAUGHT)
						StartEnemyMissile(MissileWeapon, 
							Enemy[i].x, Enemy[i].y, Enemy[i].z);
				}
				else
				{

					StartEnemyMissile(MissileWeapon, 
						Enemy[i].x, Enemy[i].y, Enemy[i].z);

				}
			}
			else
			{
				if (EnemyShotSpace[i].nShotWait > 0)
					--EnemyShotSpace[i].nShotWait;  

			}

			// If the enemy is firing his gun and it is time 
			// to add a crack
			if (pShotSnd && !pShotSnd->Stopped() && 
				EnemyShotSpace[i].nCrackWait == 0)
			{
				AddBulletCrack(TRUE);
				EnemyShotSpace[i].nCrackWait = 2;
			}
			else
			{
				if (EnemyShotSpace[i].nCrackWait > 0)
					--EnemyShotSpace[i].nCrackWait;
			}

			return TRUE;
		}
	}

	return FALSE;
}


//***********************************************************************
//void CWeapon::StartLightening(int xSrc, int ySrc, int zSrc, 
//							  int xDst, int yDst, int zDst)
//***********************************************************************
//{
//	m_bLighteningOn = TRUE;
//	m_xSrc = xSrc;
//	m_ySrc = ySrc;
//	m_zSrc = zSrc;
//	m_xDst = xDst;
//	m_yDst = yDst;
//	m_zDst = zDst;
//}

//***********************************************************************
//void CWeapon::DrawLightening(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
//{
//	if (!m_bLighteningOn)
//		return;
//
//	POINT p1 = Point3DToPointDib(m_xSrc, m_ySrc, m_zSrc);
//	POINT p2 = Point3DToPointDib(m_xDst, m_yDst, m_zDst);
//	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);
//
//}
//***********************************************************************
//void CWeapon::StopLightening(void)
//***********************************************************************
//{
//	m_bLighteningOn = FALSE;
//}

//***********************************************************************
void DrawSprite(int nPercent, int x, int y, PDIB pSprite, LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	if ( !pSprite || !lpbi || !lpDestBits )
		return;

	int nSrcX;
	int nSrcY;
	int nYerror;
	int nXerror;
	int nDestOffset;
	int nOldDestOffset;
	int nDestHeight;
	int nDestWidth;
	int nDestByteWidth;
	int nDestXPos;
	int nDestYPos;
	int nSrcOffset;
	int nOldSrcOffset;
	int nSrcHeight;
	int nSrcByteWidth;
	int nSrcPixWidth;
	BYTE TransColor;

	RECT SrcRect;
	RECT DestRect;
	RECT ClipRect;

	LPTR pSrc;
	LPTR pDest = lpDestBits;

	// Initialize
	nYerror			= 0;	// Vertical error term
	nXerror			= 0;	// Horizontal error term

	nDestHeight		= lpbi->biHeight;
	nDestWidth      = lpbi->biWidth;

	// Destination bitmap width in bytes (ULONG aligned)
	nDestByteWidth	= (((lpbi->biBitCount * (int)lpbi->biWidth) + 31) & (~31) ) / 8;
	nSrcHeight		= pSprite->GetHeight();
	nSrcByteWidth	= pSprite->GetWidthBytes();
	nSrcPixWidth	= pSprite->GetWidth();
	pSrc			= pSprite->GetPtr();
	nSrcOffset		= nSrcByteWidth * (nSrcHeight-1);	// Source bitmap offset

	// Figure upper left corner of sprite considering it might be scaled
	int nScaleWidth = (int)(nSrcPixWidth * ((double)nPercent / 100.0));
	int nScaleHeight = (int)(nSrcHeight * ((double)nPercent / 100.0));
	double dlx = nSrcPixWidth * (nPercent / 100.0) / 2.0;
	int lx = x - (int)(nScaleWidth / 2);
	int uy = y - (int)(nScaleHeight / 2);

	// Clip source rect if on edge of destination bitmap
	SetRect(&SrcRect,  lx, uy, lx+nScaleWidth, uy+nScaleHeight);
	SetRect(&DestRect, 0, 0, nDestWidth, nDestHeight);
	IntersectRect(&ClipRect, &DestRect, &SrcRect);

	// Check for off screen
	if (IsRectEmpty(&ClipRect))
	{
		return;
	}

	// Set clipped source starts and lengths
	int nSrcXStart;
	int nSrcYStart;
	int nSrcByteHeight = nSrcHeight;

	// if off the left of the screen
	if (lx < 0)
		nSrcXStart = (int)( ((double)abs(lx) / (double)nPercent) * 100.0 );
	else
		nSrcXStart = 0;

	// if off the right of the screen
	if ((lx + nScaleWidth) > nDestWidth)
		nSrcPixWidth -= (int)( ((double)(lx + nScaleWidth - nDestWidth) / (double)nPercent) * 100.0 );

	// if off the top of the screen
	if (uy < 0)
		nSrcYStart = (int)( ((double)abs(uy) / (double)nPercent) * 100.0 );
	else
		nSrcYStart = 0;

	// if off the bottom of the screen
	if ((uy + nScaleHeight) > nDestHeight)
		nSrcHeight -= (int)( ((double)(uy + nScaleHeight - nDestHeight) / (double)nPercent) * 100.0 );

	// Set clipped upper-left sprite coordinates
	lx = ClipRect.left;
	uy = ClipRect.top;

	nDestXPos = lx;
	nDestYPos = uy;

	// Destination bitmap offset
	nDestOffset = (nDestHeight - uy - 1) * nDestByteWidth + lx - 1;

	// Source bitmap offset
	nSrcOffset = nSrcByteWidth * (nSrcByteHeight-1-nSrcYStart) + nSrcXStart;	// Source bitmap offset

	TransColor = *pSrc;

	// For all of the rows in the source bitmap
	for(nSrcY=nSrcYStart; nSrcY<nSrcHeight; nSrcY++)
	{
		// Adjust the vertical error term
		nYerror += nPercent;

		// Draw the next row as many times as need be
		while(nYerror >= 100)
		{
			// Adjust the vertical error term
			nYerror -= 100;

			// Save destination and source bitmap offsets
			nOldDestOffset = nDestOffset;
			nOldSrcOffset = nSrcOffset;

			// Init horizontal error term
			nXerror = 0;

			// Set this to keep track of where we are in the destination row so we can clip the
			// right edge if we need to
			nDestXPos = lx;

			// For all the pixels in the row
			for(nSrcX=nSrcXStart; nSrcX<nSrcPixWidth; nSrcX++) 
			{

				// Adjust horz error term
				nXerror += nPercent;

				// Draw the next pixel as many times as need be
				while(nXerror >= 100) 
				{

					// Adjust horz error term
					nXerror -= 100;

					// Copy pixel from source to destination (if not transparent color)
					if (TransColor != *(pSrc + nSrcOffset))
					{
						*(pDest + nDestOffset) = *(pSrc + nSrcOffset);
					}

					// Next pixel in destination
					++nDestOffset;
					++nDestXPos;
					if (nDestXPos >= nDestWidth)
						goto StopDrawingX;
				}

				// Next pixel in source
				++nSrcOffset;
			}

StopDrawingX:

			// Next row on destination
			nDestOffset = nOldDestOffset - nDestByteWidth;

			// Bump destination row position and see if we need to clip
			++nDestYPos;
			if (nDestYPos >= nDestHeight)
			{
				// No more rows to draw (clipped)
				return;
			}

			// Repeat same row in source
			nSrcOffset = nOldSrcOffset;
		}

		// Next row in source
		nSrcOffset = nSrcOffset - nSrcByteWidth;
	}
}

//***********************************************************************
void RotateRight(int *px, int *py)
//***********************************************************************
{
	int iNewX = (int)(((double)*px * Cos30) - ((double)*py * Sin30));
	//int iNewY = (int)(((double)*py * Cos30) - ((double)*px * Sin30));
	int iNewY = (int)(((double)*py * Cos30) + ((double)*px * Sin30));

	*px = iNewX;
	*py = iNewY;
}

//***********************************************************************
void RotateLeft(int *px, int *py)
//***********************************************************************
{
	int iNewX = (int)((*px * Cos30) - (*py * -1 * Sin30));
	int iNewY = (int)((*py * Cos30) + (*px * -1 * Sin30));

	*px = iNewX;
	*py = iNewY;
}

//***********************************************************************
void CJack::MoveMissiles(void)
//***********************************************************************
{ // this function moves the missiles and does all the collision detection

	if (!App.pGame)
		return;

	int i;
	int iIndex;
	for ( i=0; i<NumMissiles; i++ )
    {

	    if ( Missiles[i].nActive == ACTIVE_NO)
	    	continue;

		// See if the jack is stuck to the car
		if (Missiles[i].nActive > 1)
		{
			--Missiles[i].nActive;
			Missiles[i].nCurFrame = 0; // Make stationary

			// Tell the world that an opponent was hit by a jack
			if (Missiles[i].nActive == 1)
			{
				Missiles[i].nActive = ACTIVE_NO;

				iIndex = Missiles[i].nEnemyIndex;


				if (m_nWaitJump == 0)
				{
					int iHit = 0;
					if ((iHit = App.pGame->lpWorld->HitOpponent(iIndex, m_nPower)) > 0 || iHit == -1)
					{
						// Start drawing the score
						StartDrawScore(App.pGame->lpWorld->GetScore(), 30);

						// Kill 'em all for noew
						for(int i=0; i<nActiveEnemies; i++)
						{
							// Stop car sound
							m_pCarSnds[i]->Stop();

							// Big explosion!!!
							StartExplosion(Enemy[i].x, 1, Enemy[i].z, 1);
						}

						m_nWaitJump = 1;

						//VideoColorEffect( RGB(255, 255, 255) );
						nActiveEnemies = 0;
						nLastActiveEnemies = 0;
						m_bOppsPossible = FALSE;
					}
					else
					{
						// Force Field Explosion
						StartExplosion(Missiles[i].x, 1, Enemy[iIndex].z, 2);
					}
				}
				else
				{
					int iIndex = Missiles[i].nEnemyIndex;
					if (iIndex > -1 )
					{
						Missiles[i].x = Enemy[iIndex].x;
						Missiles[i].z = Enemy[iIndex].z;
						Missiles[i].y = Enemy[iIndex].y;
					}
				}
			} // end if nactive > 1

			continue;
		}

		// missile x and z are figured as follows:
		// RM = distance from car to sight = range of missile = 175
		// CP = Car Position (or weapon position) = (0, 50)
		// SP = Sight Position = (-60, 175)
		// MT = Missile Traveled = 30 feet
		// MPX = Missile Position X coordinate = MT/RM * SPX + CPX = 30/175 * -60 + 0 = -10
		// MPZ = Missile Position Z coordinate = MT/RM * SPZ + CPZ = 30/175 * 175 + 50 = 80
		// or 30 +50 = 80
		// MP = (-6, -18)

		// NOTE: Move ratio calculation to start missile and store result
		double dDistance;
		if (Missiles[i].bBehind)
			dDistance = nSightZ * -1 + WeaponPos.z;
		else
			dDistance = nSightZ - WeaponPos.z;
		if (dDistance == 0)
			dDistance = 1;
		double Ratio = Missiles[i].wTraveled / dDistance;

		// If missile behind player
		if (Missiles[i].bBehind)
		{
			Missiles[i].x = (int)(Ratio * Missiles[i].nSightX + WeaponPos.x);
			Missiles[i].z = (int)(Ratio * nWorldLength + WeaponPos.z);
		}
		else
		{
			Missiles[i].x = (int)(Ratio * Missiles[i].nSightX + WeaponPos.x);
			Missiles[i].z = (int)(Ratio * nWorldLength + WeaponPos.z);
		}

		// Find the percentage traveled along max distance
		double fScale = (double)Missiles[i].wTraveled / 150.0;
		int iScale = (int)(fScale * 10.0);
		//if (iScale > 3)
		//	iScale -= (iScale - 3);
		//if (iScale < 0)
		//	iScale = 0;

		//Missiles[i].y = 3 + (int)pow(iScale, 2) * -1;
		if (iScale & 1)
			Missiles[i].y = -1;
		else
			Missiles[i].y = 2;

		//Debug("dDistance = %d, miss x=%d, y=%d, z=%d\n",  (int)dDistance, Missiles[i].x, Missiles[i].y, Missiles[i].z);

		Missiles[i].wTraveled += (WORD)(Missiles[i].dSpeed);

		// If hit wall
		if ( HitWall(Missiles[i].x, Missiles[i].z) )
		{
			Missiles[i].nActive = ACTIVE_NO;
			//--nActiveMissiles;

			// Start small explosion
			//StartExplosion(Missiles[i].x, Missiles[i].z, Missiles[i].wTraveled);
			// Kludge for machine gun - for now (mc)
			if (Type == GunWeapon)
				Missiles[i].z = WeaponPos.z + 100;

			StartExplosion(Missiles[i].x, Missiles[i].y, Missiles[i].z, 0);
		}

		// else test if hit enemy
		else if ( (iIndex = HitEnemy( &Missiles[i] )) > -1) 
		{
			
			// Stick to enemy
			Missiles[i].nActive = 3;
			Missiles[i].x = Enemy[iIndex].x;
			Missiles[i].z = Enemy[iIndex].z;
			Missiles[i].y = Enemy[iIndex].y;
			Missiles[i].nEnemyIndex = iIndex;

			pRocketSnd->Stop();
			pAttackSnd->Play();
			pShotSnd->Stop();

			//StartExplosion(Missiles[i].x, 1 , Missiles[i].z, 1);			
		} // end if HitEnemy

		// else missile has not hit anything
		else
		{
			// Adjust sound
			if (pRocketSnd)
				pRocketSnd->SetPosition(Missiles[i].x, Missiles[i].z, 0);		
		}
	} // end for loop
}


//***********************************************************************
void CLeroy::Open( HWND hSceneWindow )
//***********************************************************************
{
	CWeapon::Open(hSceneWindow);

	Type = HoverWeapon;
	nMinExpScale = 30;
	m_nPower = LEROI_MAX_POWER;

	int nFireBitmaps = LoadBitmaps( IDC_LEROY0 );
	SetMissileParams(1000, 10, nFireBitmaps, HoverWeapon);

	nNumExpHitFrames = 4;
	nNumExpMissFrames = 4;

	pRocketSnd = new CDirSnd();
	pRocketSnd->Open(IDC_LEROIFLOATSND, GetApp()->GetInstance(), TRUE);

	pExpSmallSnd = new CDirSnd();
	pExpSmallSnd->Open(IDC_EXPLSMSND, GetApp()->GetInstance(), TRUE);

	pExpBigSnd = new CDirSnd();
	pExpBigSnd->Open(IDC_EXPLBGSND, GetApp()->GetInstance(), TRUE);

	pAttackSnd = new CDirSnd();
	pAttackSnd->Open(IDC_LEROIATTACKSND, GetApp()->GetInstance(), TRUE);
}

//***********************************************************************
void CLeroy::MoveMissiles(void)
//***********************************************************************
{ // this function moves the missiles and does all the collision detection
	if (!App.pGame)
		return;

	static int iCnt = 0;
	static int iLeroyFrame = -1;
	static int iMove = 1;
	//static int iPath = 1;   
	static int iSaveZ = 0;
	static int iSaveX = 0;
	static int iPos = 0;	// His current position
	const int  iMaxPos = 16; // Max positions
	static POINT ScrPos[iMaxPos] = {{8,-2},{8,-2},{9,-3},{9,-3},{10,-4},{10,-4},{11,-3},{11,-3},{12,-2},{12,-2},{11,-2},{11,-2},{10,-2},{10,-2},{9,-2},{9,-2}};

	int i;
	for ( i=0; i<NumMissiles; i++ )
    {
		int iIndex;

	    if ( Missiles[i].nActive == ACTIVE_NO)
	    	continue;

		// Figure out Leroys next frame
		iLeroyFrame += iMove;
		if (iLeroyFrame >= Missiles[i].nFrames)
		{
			iMove = -1;
			iLeroyFrame = Missiles[i].nFrames - 1;
		}
		else if (iLeroyFrame < 0)
		{
			iMove = 1;
			iLeroyFrame = 0;
		}

		Missiles[i].nCurFrame = iLeroyFrame;

		// If hovering
		if (Missiles[i].nActive == 1)
		{
			if (nActiveEnemies)
			{
				Missiles[i].nActive = 2;
				pRocketSnd->Stop();
				pAttackSnd->Play();
				Missiles[i].z = WeaponPos.z + 10;
			}

			Missiles[i].x = ScrPos[iPos].x;
			Missiles[i].y = ScrPos[iPos].y;
			Missiles[i].z = WeaponPos.z+20;
			Missiles[i].wTraveled = 30;

			//iPos += iPath;
			++iPos;
			if (iPos >= iMaxPos)
				iPos = 0;
		}

		// Else if returning
		else if (Missiles[i].nActive == 3)
		{
			if (nActiveEnemies && iCnt == 0)
			{
				Missiles[i].nActive = 2;
				pRocketSnd->Stop();
				pAttackSnd->Play();
			}
			if (iCnt > 0)
				--iCnt;

			Missiles[i].z = Missiles[i].wTraveled + WeaponPos.z;

			// Note: It is ok to use sight z since it is always fixed at something like 1000
			double dDistance = iSaveZ - WeaponPos.z;
			if (dDistance < 1)
				dDistance = 1;
			double Ratio = (Missiles[i].z - WeaponPos.z) / dDistance;
			if (Ratio > 1)
				Ratio = 1;

			if (Enemy[0].x >= 0)
				Missiles[i].x = (int)(Ratio * (iSaveX - WeaponPos.x)) + WeaponPos.x;
			else
				Missiles[i].x = WeaponPos.x - (int)(Ratio * (WeaponPos.x - iSaveX));

			//Missiles[i].z = (int)(Ratio * nWorldLength + WeaponPos.z);
			
			//Debug("Ratio=%f",(float)Ratio);
			//Debug("wTraveled=%d, dDistance = %d, miss x=%d, y=%d, z=%d", Missiles[i].wTraveled,
			//	(int)dDistance, Missiles[i].x, Missiles[i].y, Missiles[i].z);
			//Debug("Enemy x=%d, y=%d, z=%d", Enemy[0].x, Enemy[0].y, Enemy[0].z);
			//Debug("weapn z=%d, weapn x=%d", WeaponPos.z, WeaponPos.x);

			Missiles[i].wTraveled -= (WORD)(Missiles[i].dSpeed);
			if (Missiles[i].wTraveled < 20)
				Missiles[i].nActive = 1;

		}
		// Else attacking
		else
		{
			//int ex=Enemy[0].x;
			//int ey=Enemy[0].y;
			//int ez=Enemy[0].z;

			// Convert enemy coordinate to screen for homing missile calculation
			//POINT pnt = Point3DToPointDib(ex, ey, ez);

			Missiles[i].z = Missiles[i].wTraveled + WeaponPos.z;

			// Note: It is ok to use sight z since it is always fixed at something like 1000
			double dDistance = Enemy[0].z - WeaponPos.z;
			if (dDistance < 1)
				dDistance = 1;
			double Ratio = (Missiles[i].z - WeaponPos.z) / dDistance;
			if (Ratio > 1)
				Ratio = 1;

			if (Enemy[0].x >= 0)
				Missiles[i].x = (int)(Ratio * (Enemy[0].x - WeaponPos.x)) + WeaponPos.x;
			else
				Missiles[i].x = WeaponPos.x - (int)(Ratio * (WeaponPos.x - Enemy[0].x));

			//Missiles[i].z = (int)(Ratio * nWorldLength + WeaponPos.z);
			
			//Debug("Ratio=%f",(float)Ratio);
			//Debug("wTraveled=%d, dDistance = %d, miss x=%d, y=%d, z=%d", Missiles[i].wTraveled,
			//	(int)dDistance, Missiles[i].x, Missiles[i].y, Missiles[i].z);
			//Debug("Enemy x=%d, y=%d, z=%d", Enemy[0].x, Enemy[0].y, Enemy[0].z);
			//Debug("weapn z=%d, weapn x=%d", WeaponPos.z, WeaponPos.x);

			Missiles[i].wTraveled += (WORD)(Missiles[i].dSpeed);

			// Make sure the missile distance is right
			if (Ratio > 0.9)
				Missiles[i].z = Enemy[0].z;

			// If hit wall
			if ( HitWall(Missiles[i].x, Missiles[i].z) )
			{
				//Missiles[i].nActive = NO;
				//--nActiveMissiles;

				// Start small explosion
				//StartExplosion(Missiles[i].x, Missiles[i].z, Missiles[i].wTraveled);
				// Kludge for machine gun - for now (mc)
				//if (Type == GunWeapon)
				//	Missiles[i].z = WeaponPos.z + 100;

				//StartExplosion(Missiles[i].x, Missiles[i].y, Missiles[i].z, 0);

				// Adjust LEROI's position since he is bumping into a wall
				if (Missiles[i].x > 0)
					Missiles[i].x -= 2;
				else
					Missiles[i].x += 2;
			}

			// else test if hit enemy
			else if ( (iIndex = HitEnemy( &Missiles[i] )) > -1 ) 
			{
				//if (iCnt == 6)
				//{
				//	Missiles[i].nActive = NO;
				//	iCnt = 0;
				//}
				//else

				//--nActiveMissiles;

				//StartExplosion(Missiles[i].x, 1 , Missiles[i].z, 1);			
				// Tell the world that an opponent was hit by a rocket
				int iStrength = 0;
				int iTemp;
				if (m_nWaitJump == 0)
				{
					int iHit = 0;

					// get strength now, in case we need it later
					iStrength = App.pGame->lpWorld->GetGroup()->GetStrength(iIndex);
					if ((iHit = App.pGame->lpWorld->HitOpponent(iIndex, m_nPower)) > 0 || iHit == -1)
					{
						// Start drawing the score
						StartDrawScore(App.pGame->lpWorld->GetScore(), 30);

						// Kill 'em all for now
						for(int i=0; i<nActiveEnemies; i++)
						{

							// accumluate strength							  
							iTemp = App.pGame->lpWorld->GetGroup()->GetStrength(i);
							if (iTemp > 0)
								iStrength += iTemp;

							// Stop car sound
							m_pCarSnds[i]->Stop();

							// Big explosion!!!
							StartExplosion(Enemy[i].x, 1, Enemy[i].z, 1);

						}

						m_nWaitJump = 1;

						// make sure shooting back stops
						pShotSnd->Stop();

						//VideoColorEffect( RGB(255, 255, 255) );
						nActiveEnemies = 0;
						nLastActiveEnemies = 0;
						m_bOppsPossible = FALSE;
					}
					else
					{
						// Force Field Explosion
						StartExplosion(Missiles[i].x, 1, Enemy[iIndex].z, 2);
					}
				}

				m_nPower -= iStrength;

				// If there is any power left in LEROI then keep him
				if (m_nPower > 0) 
				{
					Missiles[i].nActive = 3;
					iCnt = 6;
					pRocketSnd->Play(TRUE, TRUE, TRUE);
					iSaveZ = Missiles[i].z;
					iSaveX = Missiles[i].x;
				}
				// else this LEROI has been destroyed
				else
				{
					m_nPower = LEROI_MAX_POWER;
					Missiles[i].nActive = 0;
				}

			} // end if HitEnemy

			// else missile has not hit anything
			else
			{
				// Adjust sound
				if (pAttackSnd)
					pAttackSnd->SetPosition(Missiles[i].x, Missiles[i].z, 0);
				
		}
		}
	} // end for loop
}

//***********************************************************************
void CThumper::Open( HWND hSceneWindow )
//***********************************************************************
{
	CWeapon::Open(hSceneWindow);

	Type = AerialWeapon;
	nMinExpScale = 30;
	m_nPower = 10000; // doesn't matter (kills all in cell anyway)

	int nFireBitmaps = LoadBitmaps(IDC_THUMP0);
	nNumExpHitFrames = 4;
	nNumExpMissFrames = 4;
	SetMissileParams(1000, 15, nFireBitmaps, Type);

	pRocketSnd = new CDirSnd();
	pRocketSnd->Open(IDC_THUMPERLAUNCHSND, GetApp()->GetInstance(), TRUE);

	pExpBigSnd = new CDirSnd();
	pExpBigSnd->Open(IDC_THUMPEREXPSND, GetApp()->GetInstance(), TRUE);
}

//***********************************************************************
void CThumper::MoveMissiles(void)
//***********************************************************************
{ // this function moves the missiles and does all the collision detection
	if (!App.pGame)
		return;

	int i;
	for ( i=0; i<NumMissiles; i++ )
    {

	    if ( !Missiles[i].nActive )
	    	continue;

		// missile x and z are figured as follows:
		// RM = distance from car to sight = range of missile = 175
		// CP = Car Position (or weapon position) = (0, 50)
		// SP = Sight Position = (-60, 175)
		// MT = Missile Traveled = 30 feet
		// MPX = Missile Position X coordinate = MT/RM * SPX + CPX = 30/175 * -60 + 0 = -10
		// MPZ = Missile Position Z coordinate = MT/RM * SPZ + CPZ = 30/175 * 175 + 50 = 80
		// or 30 +50 = 80
		// MP = (-6, -18)

		// NOTE: Move ratio calculation to start missile and store result
		//double dDistance = nSightZ - WeaponPos.z;
		//if (dDistance < 1)
		//	dDistance = 1;
		//double Ratio = Missiles[i].wTraveled / dDistance;

		//Missiles[i].x = //(int)(Ratio * Missiles[i].nSightX + WeaponPos.x);
		if (Missiles[i].bBehind)
			Missiles[i].z =  WeaponPos.z - Missiles[i].wTraveled;
		else
			Missiles[i].z =  WeaponPos.z + Missiles[i].wTraveled;

		int iSub = 2 * (int)(10.0 * (double)Missiles[i].wTraveled / 500.0);
		Missiles[i].y -= (2 + iSub);
		//Debug("dDistance = %d, miss x=%d, y=%d, z=%d\n",  (int)dDistance, Missiles[i].x, Missiles[i].y, Missiles[i].z);

		Missiles[i].wTraveled += (WORD)(Missiles[i].dSpeed);

		// If missile has reached the end of its range
		if (Missiles[i].wTraveled > 900)
		{
			Missiles[i].nActive = NO;
			pExpBigSnd->Play();
			if (m_pCell)
			{
				App.pGame->lpWorld->GetGroup()->DestroyArea(m_pCell, m_iDir);
			}
		}

		// Check for firing too close to an enemy or a wall
		else if (Missiles[i].wTraveled < 50)
		{
			BOOL bDead = FALSE;

			if (nActiveEnemies)
			{
				int Z = 900;
				for(int i=0; i<nActiveEnemies; i++)
				{
					if (Enemy[i].z < Z)
						Z = Enemy[i].z;
				}

				Z = Z - WeaponPos.z;
				if (Z < 60 && Z > 0) 
					bDead = TRUE;
			}
			else if (HitWall(Missiles[i].x, Missiles[i].z))
				bDead = TRUE;

			if (bDead)
			{

				// We are dead...
				Missiles[i].nActive = NO;
				pExpBigSnd->Play();
				App.pGame->lEnergy = 0;
			}
		}
		// else missile has not hit anything
		else
		{
			// Adjust sound
			//if (pRocketSnd)
			//	pRocketSnd->SetPosition(Missiles[i].x, Missiles[i].z, 0);
			
		}
	} // end for loop
}

//***********************************************************************
CWeaponSystem::CWeaponSystem()
//***********************************************************************
{
	nCount = 0;
	nCurWeapon = 0;
}

//***********************************************************************
CWeaponSystem::~CWeaponSystem()
//***********************************************************************
{
	Destroy();
}

//***********************************************************************
void CWeaponSystem::Init(HWND hWnd)
//***********************************************************************
{
	// If already initialized
	if (nCount)
	{
		// Set the scene window
		for(int i=0; i<nCount; i++)
			pWeapon[i]->SetScene(hWnd);

		return;
	}
	// else, initialize weapons
	else
	{

		// Create weapons
		pWeapon[0] = new CScrapGun();
		pWeapon[1] = new CMachineGun();
		pWeapon[2] = new CMissile();
		pWeapon[3] = new CLaser();
		pWeapon[4] = new CMine();
		pWeapon[5] = new CJack();
		pWeapon[6] = new CLeroy();
		pWeapon[7] = new CBomb();
		pWeapon[8] = new CThumper();

		nCount = 9;

		// Open them
		for(int i=0; i<nCount; i++)
			pWeapon[i]->Open(hWnd);

	}

	pWeapon[0]->SetSystem(this);
}

//***********************************************************************
void CWeaponSystem::Destroy()
//***********************************************************************
{
	for(int i=0; i<nCount; i++)
		delete pWeapon[i];

	nCount = 0;
}

//***********************************************************************
void CWeaponSystem::SetAmmo(int nIndex, int nAmount)
//***********************************************************************
{
	if (nIndex < 0 || nIndex >= nCount)
		return;

	Ammo[nIndex] = nAmount;
}

//***********************************************************************
void CWeaponSystem::AddAmmo(int nIndex, int nAmount)
//***********************************************************************
{
	if (nIndex < 0 || nIndex >= nCount)
		return;

	Ammo[nIndex] += nAmount;
}

//***********************************************************************
int CWeaponSystem::GetAmmo(int nIndex)
//***********************************************************************
{
	if (nIndex < 0 || nIndex >= nCount)
		return 0;

	return Ammo[nIndex];
}

//***********************************************************************
void CWeaponSystem::DecAmmo(int nIndex, int nAmount)
//***********************************************************************
{
	if (nIndex < 0 || nIndex >= nCount)
		return;

	if (Ammo[nIndex] > 0)
		--Ammo[nIndex];
}


//***********************************************************************
void CWeaponSystem::MoveMissiles(void)
//***********************************************************************
{
	int i;
	for(i=0; i<nCount; i++)
		pWeapon[i]->MoveMissiles();
}

//***********************************************************************
void CWeaponSystem::MoveExplosions(void)
//***********************************************************************
{
	for(int i=0; i<nCount; i++)
		pWeapon[i]->MoveExplosions();
}

//***********************************************************************
void CWeaponSystem::DrawMissiles(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	for(int i=0; i<nCount; i++)
		pWeapon[i]->DrawMissiles(lpbi, lpDestBits);
}

//***********************************************************************
void CWeaponSystem::DrawExplosions(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	for(int i=0; i<nCount; i++)
		pWeapon[i]->DrawExplosions(lpbi, lpDestBits);
}

//***********************************************************************
void CWeaponSystem::SetCurWeapon(int nIndex)
//***********************************************************************
{
	if (nIndex < 0 || nIndex >= nCount)
		return;

	// Make sure weapon stops firing
	pWeapon[nCurWeapon]->StopFire();
	// Clear old video observer
	VideoClearObserver(pWeapon[nCurWeapon]);

	nCurWeapon = nIndex;

	// Set new video observer
	VideoSetObserver(pWeapon[nCurWeapon]);

}

//***********************************************************************
void CWeapon::ResetText(void)
//***********************************************************************
{
	VideoFx *pfx = App.pGame->pVidFx;
	if (pfx)
		pfx->TextOff();
}

//***********************************************************************
int CWeapon::GetCrashDamage(void)
//***********************************************************************
{
	COpponentGroup *pGroup = App.pGame->lpWorld->GetGroup();
	if (!pGroup)
		return 0;

	int iDamage = 0;

	// assume all enemies hit...
	for(int i=0; i<nActiveEnemies; i++)
		iDamage += pGroup->EngagedCrashStrength(i);

	return iDamage;
}

//***********************************************************************
void CWeapon::DoCrashEffect(void)
//***********************************************************************
{
	COpponentGroup *pGroup = App.pGame->lpWorld->GetGroup();
	if (!pGroup)
		return;

	int iClass = 0;
	EngageRec *pCur;
	COpp *pOpp;

	// assume all enemies hit...so get all effects
	for(int i=0; i<nActiveEnemies; i++)
	{
		pCur = pGroup->FindEngaged(i);
		if (!pCur)
			continue;

		pOpp = pCur->pOpp;
		if (!pOpp)
			continue;

		iClass = pOpp->GetClass();

		switch(iClass)
		{
		case OPPCLASS_DETONATOR:
			{
				// Kill 'em all for now
				for(int i=0; i<nActiveEnemies; i++)
				{
					// Call hit opponent to make sure cars get removed from engaged list.
					// Use power of 9999 to make sure they get destroyed
					App.pGame->lpWorld->HitOpponent(i, 999);
					// Stop car sound
					m_pCarSnds[i]->Stop();
					// Big explosion!!!
					StartExplosion(Enemy[i].x, Enemy[i].y, Enemy[i].z, 1);
					// Start drawing the score
					StartDrawScore(App.pGame->lpWorld->GetScore(), 30);

					m_nWaitJump = 1;
				}
				break;
			}
		case OPPCLASS_HELLION:
			App.pGame->HitHellion();
			break;

		case OPPCLASS_NIGHTRON:
			{
				VideoFx *pfx = App.pGame->pVidFx;
				if (pfx)
				{
					if (m_nWhiteOutDur)
					{
						pfx->TextOn(hSceneWnd, "Nightron cancels flash.", RGB(255, 255, 0), 18, 3000, 4);
						m_nWhiteOutDur = 0;
					}
					else
					{
						pfx->TextOn(hSceneWnd, "Nightron blackout!", RGB(255, 0, 0), 18, 5000, 4);
						StartBlackOut(20000);
					}
				}
			}
			break;

		case OPPCLASS_PARHELION:
			{
				VideoFx *pfx = App.pGame->pVidFx;
				if (pfx)
				{
					if (m_nBlackOutDur)
					{
						pfx->TextOn(hSceneWnd, "Parhelion cancels blackout.", RGB(255, 255, 0), 18, 3000, 4);
						m_nBlackOutDur = 0;
					}
					else
					{
						pfx->TextOn(hSceneWnd, "Parhelion flash!", RGB(255, 0, 0), 18, 5000, 4);
						StartWhiteOut(20000);
					}
				}
			}
			break;

		case OPPCLASS_THWART:
			App.pGame->HitThwart();
			break;

		case OPPCLASS_FLASHER:
			{
				StartStrobe(20000);
				break;
			}

		case OPPCLASS_ELECTROSHARK:
			{
				StartElectric(100, 113);
				break;
			}

		case OPPCLASS_VIRALION:
			StartElectric(3, 104);
			App.pGame->HitViralion();
			break;

		case OPPCLASS_MEDICATOR:
			App.pGame->HitMedicator();
			m_nCurCrashDamage = 0; // medicator does no damage
			break;

		case OPPCLASS_DRANIAC:
			StartEffect(10000, 0); // dim the screen
			App.pGame->HitDraniac();
			break;

		case OPPCLASS_KILLER:
			StartEffect(400, 1); // dark read
			App.pGame->HitKiller();
			break;

		case OPPCLASS_SLAMATRON:
			StartEffect(200, 5); // purple flash
			App.pGame->HitSlamatron(); 
			break;

		case OPPCLASS_PSYCHOTECH:
			StartElectric(20, 143); // light green electric
			App.pGame->HitPsychotech();
			break;

		case OPPCLASS_PAINULATOR:  // electric color cycle
			m_nColorCycle = 249;
			StartElectric(20, 249);
			break;

		case OPPCLASS_PARAGON: // strobe color cycle
			m_nColorCycle = 249;
			StartStrobe(20000);
			break;

		case OPPCLASS_SPINEIMMORTACON:
			break;

		case OPPCLASS_SPINE:
			break;

		default:
			StartEffect(200, 249);
		}
	}
}

//***********************************************************************
void CWeapon::StartBlackOut(int nMills)
//***********************************************************************
{
	m_nBlackOutDur = nMills;
	m_dwBlackOutStart = timeGetTime();
}

//***********************************************************************
void CWeapon::BlackOut(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	if ((int)(timeGetTime() - m_dwBlackOutStart) > m_nBlackOutDur)
	{
		m_nBlackOutDur = 0;
		return;
	}

	long lCount;
	//long lStart = timeGetTime();
	//long lTime;

	// Get the number of bytes to zero out (make black)
	lCount = lpbi->biHeight * (((lpbi->biBitCount * (int)lpbi->biWidth) + 31) & (~31) ) / 8;
	--lCount;

	// FAST - 2 to 3 milliseonds
	ZeroMemory((PVOID)lpDestBits, lCount);

	//lTime = timeGetTime() - lStart;

	//Debug("lTime=%ld", lTime);
}

//***********************************************************************
void CWeapon::StartWhiteOut(int nMills)
//***********************************************************************
{
	m_nWhiteOutDur = nMills;
	m_dwWhiteOutStart = timeGetTime();
}

//***********************************************************************
void CWeapon::WhiteOut(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	if ((int)(timeGetTime() - m_dwWhiteOutStart) > m_nWhiteOutDur)
	{
		m_nWhiteOutDur = 0;
		return;
	}

	long lCount;
	//long lStart = timeGetTime();
	//long lTime;

	// Get the number of bytes to zero out (make black)
	lCount = lpbi->biHeight * (((lpbi->biBitCount * (int)lpbi->biWidth) + 31) & (~31) ) / 8;
	--lCount;

	FillMemory ((PVOID)lpDestBits, lCount, 255);
	//lTime = timeGetTime() - lStart;

	//Debug("lTime=%ld", lTime);
}

//***********************************************************************
void CWeapon::StartEffect(int nMills, int nColIndex)
//***********************************************************************
{
	m_nEffectDur = nMills;
	m_dwEffectStart = timeGetTime();
	m_nEffectColor = nColIndex;
}

//***********************************************************************
void CWeapon::Effect(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	if ((int)(timeGetTime() - m_dwEffectStart) > m_nEffectDur)
	{
		m_nEffectDur = 0;
		return;
	}

	long lCount;
	//LARGE_INTEGER lStart; // = timeGetTime();
	//BOOL bRet = QueryPerformanceCounter(&lStart);
	//LARGE_INTEGER lEnd;
	//long lTime;
	//static lTotal = 0;
	//static lTimes = 0;

	//++lTimes;

	lCount = lpbi->biHeight * (((lpbi->biBitCount * (int)lpbi->biWidth) + 31) & (~31) ) / 8;
	--lCount;

	/** // Low time is 18 milliseconds, average is 32 mills
	
	long lIndex;
	for( lIndex=0; lIndex<lCount; lIndex+=2 )
	{
		*(lpDestBits+lIndex) = m_nEffectColor;
	}

	**/

	// Low time - 5 millisconds, average 18 mills
	// average ticks is 19671
	/***
	unsigned char color  = (unsigned char)m_nEffectColor;
	LPTR lpEndByte = lpDestBits + lCount - 1;

	__asm
	{
		mov ecx,	lpEndByte		;Last byte
		mov al,		color			;Color to set
		mov edx,	lpDestBits		;Destination byte pointer

setcolor:
		mov [edx], al				;Set destination byte to color
		add edx, 2					;add 2 to destination
		cmp edx, ecx				;Compare destination with last byte
		jl	setcolor				;Jump if destinations is less then last byte

	}
	***/

	/**
	// Low time - 5 milliseconds, average 16 mills
	char cColor = (char)m_nEffectColor;
	// make sure lcount is even
	lCount &= 0xfffffe;

	__asm
	{
		mov eax,	lCount			;Count of bytes
		mov ebx,    2
		mov cl,	    cColor;			;Color to set
		mov edx,	lpDestBits		;Destination byte pointer

setcolor:
		mov [edx+eax], cl			;Set destination byte to color
		sub eax, ebx				;Subtract 2 from count
		jnz	setcolor				;Jump if not zero

	}
	**/

	/** 3 mills, but AND does not work
	long lColor = m_nEffectColor;
	lColor |= ((long)m_nEffectColor << 15);
	lColor |= 0xf0f0;

	// make sure lcount is divisable by 2
	lCount &= 0xfffffe;

	__asm
	{
		mov eax,	lCount			;Count of bytes
		mov ebx,    4
		mov ecx,	lColor;			;Color to set
		mov edx,	lpDestBits		;Destination byte pointer

setcolor:
		and [edx+eax], ecx			;Set destination byte to color
		sub eax, ebx				;Subtract 4 from count
		jnz	setcolor				;Jump if not zero

	}
	**/
	/** low 4 mills
	long lColor = m_nEffectColor;
	lColor |= ((long)m_nEffectColor << 15);
	lColor |= 0xf0f0;

	// make sure lcount is divisable by 2
	lCount &= 0xfffffe;
	long lMask = 0xff00ff00;

	__asm
	{
		mov ebx,	lCount			;Count of bytes
		mov ecx,    lMask
		;mov ecx,	lColor;			;Color to set
		mov esi,	lpDestBits		;Destination byte pointer
		mov edi,    lpDestBits
		mov edx,	[esi]

setcolor:
		lodsd
		and eax, ebp
		or  eax, lColor
		stosd
		sub ebx, 4
		jnz	setcolor				;Jump if not zero

	}
	*/

	// Low time - 5 milliseconds, average 16 mills
	// avererage ticks = 19063
	
	char cColor = (char)m_nEffectColor;
	// make sure lcount is even
	lCount &= 0xfffffe;

	__asm
	{
		mov eax,	lCount			;Count of bytes
		mov ebx,    2
		mov cl,	    cColor;			;Color to set
		mov edx,	lpDestBits		;Destination byte pointer

setcolor:
		mov [edx+eax], cl			;Set destination byte to color
		sub eax, ebx				;Subtract 2 from count
		jnz	setcolor				;Jump if not zero

	}
	

	//bRet = QueryPerformanceCounter(&lEnd);
	//lTime = lEnd.LowPart - lStart.LowPart;

	//LARGE_INTEGER lFreq;
	//bRet = QueryPerformanceFrequency(&lFreq);
	//lTotal += lTime;

	//Debug("lTime=%ld, Avg.=%ld", lTime, lTotal/lTimes);
}

//***********************************************************************
void CWeapon::StartStrobe(int nMills)
//***********************************************************************
{
	m_bStrobe = TRUE;
	m_bStrobeBlack = TRUE;
	StartBlackOut(nMills);
}


//***********************************************************************
void CWeapon::Strobe(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	// If time to do a black frame
	if (m_bStrobeBlack)
	{
		if (m_nColorCycle)
		{
			// Get the number of bytes to zero out (make black)
			long lCount = lpbi->biHeight * (((lpbi->biBitCount * 
				(int)lpbi->biWidth) + 31) & (~31) ) / 8;
			--lCount;
			FillMemory ((PVOID)lpDestBits, lCount, m_nColorCycle);
			++m_nColorCycle;
			if (m_nColorCycle > 255)
				m_nColorCycle = 249;

			// Have to check blackout here since bypassing call
			if ((int)(timeGetTime() - m_dwBlackOutStart) > m_nBlackOutDur)
			{
				m_nBlackOutDur = 0;
				m_nColorCycle = 0;
			}
		}
		else
			BlackOut(lpbi, lpDestBits);

		m_bStrobeBlack = FALSE;
	}
	// else it is a normal frame
	else
		m_bStrobeBlack = TRUE;

	// If time is up then set strobe to false
	if (m_nBlackOutDur == 0)
		m_bStrobe = FALSE;

}

//***********************************************************************
void CWeapon::StartElectric(int nFrames, int nColor)
//***********************************************************************
{
	m_nElectricFrames = nFrames;
	m_nElectricColor = nColor;
}

//***********************************************************************
void CWeapon::Electric(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	--m_nElectricFrames;

	long lCount;
	//LARGE_INTEGER lStart; // = timeGetTime();
	//BOOL bRet = QueryPerformanceCounter(&lStart);
	//LARGE_INTEGER lEnd;
	//long lTime;
	//static lTotal = 0;
	//static lTimes = 0;

	//++lTimes;

	lCount = lpbi->biHeight * (((lpbi->biBitCount * (int)lpbi->biWidth) + 31) & (~31) ) / 8;
	--lCount;

	long lOffset = 0;
	int iLength;
	
	if (m_nColorCycle)
	{
		m_nElectricColor = m_nColorCycle;
		++m_nColorCycle;
		if (m_nColorCycle > 255)
			m_nColorCycle = 249;
	}

	while (lOffset < lCount)
	{
		iLength = GetRandomNumber(20) + 2;

		// Safety check
		if ((lOffset + iLength) > lCount)
			break;

		FillMemory((PVOID)(lpDestBits + lOffset), iLength, m_nElectricColor);

		lOffset += (iLength + GetRandomNumber(100) + 20);
	}

	if (m_nElectricFrames == 0)
		m_nColorCycle = 0;

	//bRet = QueryPerformanceCounter(&lEnd);
	//lTime = lEnd.LowPart - lStart.LowPart;

	//LARGE_INTEGER lFreq;
	//bRet = QueryPerformanceFrequency(&lFreq);
	//lTotal += lTime;

	//Debug("lTime=%ld, Avg.=%ld", lTime, lTotal/lTimes);
}

//***********************************************************************
void CWeapon::StopSounds(void)
//***********************************************************************
{
	for(int i=0; i<nActiveEnemies; i++)
		m_pCarSnds[i]->Stop();
}