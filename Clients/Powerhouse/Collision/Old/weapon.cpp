#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include "proto.h"
#include "weapon.h"
#include "dib.h"
#include "cllsnid.h"
#include "control.h"
#include "vworld.h"
#include "vsndmix.h"

// Imported globals
extern BOOL m_bOptionsfromGame;
extern BOOL bOption2;  //Auto Targeting toggle

#define SOUND_PLAYSOUND  // uncomment to use sndPlaysound with waves in memory

//#define WDEBUG
#ifdef WDEBUG
#include "wdebug.cpp"
#endif

// global video world
extern VideoWorld VWorld;

static int nVanishY = 120;
static int nVanishX = 320;
static int nAudioFrames = 0; // KLUDGE - only for now ... (don't ask)
static HMCI hLastWave = 0;

// Statics
LPVIDEO CWeapon::lpVideo = NULL;			// Video pointer
VideoWorld  *CWeapon::pWorld = NULL;		// 3D video coordinate world

int     CWeapon::nWorldRef = 0;				// # of references to world object (3D video coordinate world)
int     CWeapon::nViewDistance = 600;		// pixel distance of viewer from screen
int     CWeapon::nXPixels = 16;				// pixels per feet (approx)
int		CWeapon::nYPixels = 32;				// pixels per feet (approx)
int     CWeapon::nZPixels = 32;				// pixels per feet (approx)
long	CWeapon::lCurrentFrame = 0;			// Current video frame
int		CWeapon::nExpScaleFactor = 300;		// Explosion scale factor
int		CWeapon::yGround = 3;				// y coord of where the ground appears to be
int		CWeapon::nWorldLength = 300;		// Length from wall to wall on the z axis (straight ahead)
int		CWeapon::nWorldWidth = 200;			// Width from wall to wall on the x axis (left to right)
int		CWeapon::nMissileRange = 300;		// Range of missile
UINT	CWeapon::uEnemyColor = 0;			// Color of enemy rectangle (if vector drawn)
UINT	CWeapon::uEnemyBoxColor = 0;		// Color of enemy box
int		CWeapon::nActiveEnemies;			// Number of enemies
BOOL	CWeapon::bEnabled = FALSE;			// Weapons enabled or not
BOOL	CWeapon::bShowGrid = TRUE;			// Show world grid

//***********************************************************************
CWeapon::CWeapon()
//***********************************************************************
{
	SetWeaponPosition( 0, 3, 0);
	SetSightParams(120, 520);

	if (!pWorld)
	{
		pWorld = &VWorld;
	}

	// Inc world reference
	++nWorldRef;

	bSoundTrackOff = FALSE;
	bStartVideoInterim = FALSE;

	nActiveEnemies = 0;
}

//***********************************************************************
CWeapon::~CWeapon()
//***********************************************************************
{
	--nWorldRef;
	if (nWorldRef == 0)
	{
		//NOTE: Don't delete anymore, since we are pointing to a global object
		//if (pWorld)
		//	delete pWorld;
	}
	CloseAllSounds();
}

//***********************************************************************
void CWeapon::Open( HWND hSceneWindow )
//***********************************************************************
{
	int i;

	hSceneWnd = hSceneWindow;

	nActiveMissiles = 0;
	for (i=0; i<NumMissiles; i++) {
	    Missiles[i].bActive = 0;
		Missiles[i].Type = GunWeapon;
		Missiles[i].wRange = 400;
		Missiles[i].dSpeed = 0.3;
	}
	
	nActiveSparks = 0;
	for (i=0; i<NumSparks; i++)
	    Sparks[i].bActive = NO;
	
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
void CWeapon::Close( void )
//***********************************************************************
{
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
void CWeapon::End( void )
//***********************************************************************
{
}

//***********************************************************************
void CWeapon::StartMissile(void)
//***********************************************************************
{ // this function scans through the missile array and tries to find one to fire

	for ( int i=0; i<NumMissiles; i++ )
    {
		if ( Missiles[i].bActive )
			continue;

        Missiles[i].bActive = YES;

        Missiles[i].x = WeaponPos.x;
		Missiles[i].y = 3;
        Missiles[i].z = WeaponPos.z;
		Missiles[i].nSightX = GetSightWorldX();
		Missiles[i].wTraveled = 0;
		Missiles[i].nCurFrame = 0;

        // increment number of active missiles
        nActiveMissiles++;

		// Activate sound for missile firing
		if (lpVideo)
		{
			HWND hWnd = GetDlgItem(hSceneWnd, IDC_VIDEO);

			// Turn off sound track
//			Video_OnSoundOnOff( hWnd, FALSE );
				//MCIPause(lpVideo->hDevice);
			bSoundTrackOff = TRUE;

			// Start rocket sound
			//for(int i=0; i<100; i++)
			//{
			//pRocketSnd->StartResourceID( IDC_ROCKETSND, FALSE, 1, 
			//	GetWindowInstance(hSceneWnd), FALSE, hSceneWnd);
			//Sleep(300);
			//}
			//sndPlaySound("c:\\dev\\src\\cllsn\\rocket.wav", SND_ASYNC); 
			//pRocketSnd->StartFile("c:\\dev\\src\\cllsn\\rocket.wav", FALSE, 1, FALSE, hSceneWnd);
			#ifdef SOUND_PLAYSOUND
				pRocketSnd->Play();
			#else
				PlaySound(hRocketWave);
			#endif

		}

		return;
	}
}

//***********************************************************************
BOOL CWeapon::Fire( BOOL bRepeat )
//***********************************************************************
{
	//StartMissile();
	return FALSE;
}

//***********************************************************************
void CWeapon::StartSpark(int x, int y)
//***********************************************************************
{// this function scans through the spark array and tries to find one to start
	return;

	for ( int i=0; i<NumSparks; i++ )
    {
	    if ( Sparks[i].bActive )
	    	continue;

	    // set it up to use
        Sparks[i].bActive = YES;
        Sparks[i].x  = x + GetRandomNumber(4) - 2;
        Sparks[i].y  = y + GetRandomNumber(4) - 2;
        Sparks[i].xv = 0;
        Sparks[i].yv = 0 - GetRandomNumber(3);
	
        // select a random lifetime before bursting
        Sparks[i].counter_1   = 0;
        Sparks[i].threshold_1 = 25 + GetRandomNumber(10);
        Sparks[i].threshold_2 = 10 + GetRandomNumber(5);
	
        // increment number of active Sparks[
        nActiveSparks++;
        return;
	}
}

//***********************************************************************
void CWeapon::StartExplosion(int x, int y, int z, int nType)
//***********************************************************************
{
	return;
}

//***********************************************************************
BOOL CRocketLauncher::Fire( BOOL bRepeat )
//***********************************************************************
{
	if (!bEnabled)
		return FALSE;

	//if (nActiveMissiles)
	//	return FALSE;

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
		Explosions[i].z = z	- WeaponPos.z;;
	    Explosions[i].y = y;
		Explosions[i].nType = nType;

		// Compute distance. Note: Do not subtract WeaponPos.z since we alread did that...
		Explosions[i].dDistance = 
			sqrt( pow(Explosions[i].x - WeaponPos.x, 2) + pow(Explosions[i].z, 2) );

		// If hit explosion
		if (nType == 1)
		{
			Explosions[i].nFrames = nNumExpHitFrames;
			//pExpBigSnd->StartResourceID( IDC_EXPLBGSND, FALSE, 1, 
			//	GetWindowInstance(hSceneWnd), FALSE, hSceneWnd);
			//sndPlaySound("c:\\dev\\src\\cllsn\\expbig.wav", SND_ASYNC); 
			//PlaySound("c:\\dev\\src\\cllsn\\expbig.wav");
			#ifdef SOUND_PLAYSOUND
				pRocketSnd->Stop();
				pExpBigSnd->Play();
			#else
				PlaySound(hExpBigWave);
			#endif

			Explosions[i].nCurFrame = 0;
			Explosions[i].pdibFrames = &pdibHitExplode[0];
			//bStartVideoInterim = TRUE;
		}
		else
		{
			Explosions[i].nFrames = nNumExpMissFrames;
			//pExpSmallSnd->StartResourceID( IDC_EXPLSMSND, FALSE, 1, 
			//	GetWindowInstance(hSceneWnd), FALSE, hSceneWnd);
			//if (pExpSmallSnd)
			//sndPlaySound("c:\\dev\\src\\cllsn\\expsmall.wav", SND_ASYNC); 
			//PlaySound("c:\\dev\\src\\cllsn\\expsmall.wav");
			#ifdef SOUND_PLAYSOUND
				if (pExpSmallSnd)
				{
					pRocketSnd->Stop();
					pExpSmallSnd->Play();
				}
			#else
				PlaySound(hExpSmallWave);
			#endif

			Explosions[i].nCurFrame = 0;
			Explosions[i].pdibFrames = &pdibExplode[0];
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
void CWeapon::DrawExplosion(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	int i;
	int nPercent;

	for ( i=0; i<NumExplosions; i++ )
	{
		if ( Explosions[i].nActive )
		{

			POINT Point = Point3DToPointDib(Explosions[i].x, Explosions[i].y, Explosions[i].z);
		
			//if (nForcedSize)
			//	nPercent = nForcedSize;
			//else
				nPercent = 370 - (int)((Explosions[i].dDistance * 370.0) / nExpScaleFactor) ;
			//if (nPercent > 100) {
			//	nPercent = 100;
			//}

			//Debug("exp x=%d, y=%d, dist=%d, percent=%d\n", 
			//	Point.x, Point.y, (int)Explosions[i].dDistance, nPercent);
			
			if (nPercent < nMinExpScale)
				nPercent = nMinExpScale;

			DrawSprite(nPercent, Point.x, Point.y, Explosions[i].pdibFrames[Explosions[i].nCurFrame], 
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
	lStartFrame = lpShot->lStartFrame;
	lEndFrame = lpShot->lEndFrame;

	//Debug("start=%ld, end=%ld\r\n", lpShot->lStartFrame, lpShot->lEndFrame );
	//Debug("OnVideoOpen\n");

	// Set hexagon test world for now
	pWorld->SetWorld(lpShot->lValue, this);

	//WeaponPos.nFrameDist = pWorld->GetLength() / (lEndFrame - lStartFrame);
	//WeaponPos.nFrameDist = (double)nWorldLength / (double)(pWorld->CarStop() - pWorld->CarStart());

	nActiveMissiles = 0;

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
	//Debug("OnVideoClose\n");
}

//***********************************************************************
void CWeapon::UpdatePosition()
//***********************************************************************
{
	if (lpVideo == NULL)
		return;

	// Get current frame

	// Figure position based on current frame


	lCurrentFrame = MCIGetPosition(lpVideo->hDevice);

	//WeaponPos.nFrameDist = nWorldLength / (lEndFrame - lStartFrame);
	// WeaponPos.nFrameDist = (double)nWorldLength / (double)(lEndFrame - lStartFrame);
	WeaponPos.nFrameDist = pWorld->CarStep();

	if ( lCurrentFrame <= (lStartFrame + pWorld->CarStop()) && lCurrentFrame >= (pWorld->CarStart() + lStartFrame) )
	{
		WeaponPos.z = (int)((lCurrentFrame - lStartFrame) * WeaponPos.nFrameDist);
		//Debug("z=%d\n", WeaponPos.z);
	}
}

//***********************************************************************
void CWeapon::MoveMissiles(void)
//***********************************************************************
{ // this function moves the missiles and does all the collision detection

}

//***********************************************************************
void CWeapon::MoveSparks(void)
//***********************************************************************
{ // this function moves the Sparks[
	if ( !nActiveSparks )
		return;

	for (int i=0; i<NumSparks; i++ )
	{
	    if ( !Sparks[i].bActive )
	    	continue;
	
	    // move the spark if time to using y velocity
	    int spark_x = Sparks[i].x;
		int spark_y;
	
	    if (Sparks[i].counter_1 > Sparks[i].threshold_2)
	    	spark_y = (Sparks[i].y += Sparks[i].yv);
	    else
	    	spark_y = Sparks[i].y;
	
		// test if it's hit the edge of the screen or a wall
		if ( (spark_x >= 640/*SCREEN_WIDTH*/) || (spark_x <= 0) ||
	         (spark_y > (480/*SCREEN_HEIGHT*/-16)) || ( spark_y <=0) )
	    {
	    	Sparks[i].bActive = NO;
			nActiveSparks--;
	    }
		else // is it time to pop spark
	    if (++Sparks[i].counter_1 > Sparks[i].threshold_1)
		{ // kill spark and process next
	    	Sparks[i].bActive = NO;
			nActiveSparks--;
	    }
    }
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
void CWeapon::Draw( HDC hDrawDC, HPEN hPen1, HPEN hPen2 )
//***********************************************************************
{
	MoveMissiles();
	MoveSparks();
	MoveExplosions();

	int i, x, y;
	if ( nActiveMissiles )
	{
		SelectObject( hDrawDC, hPen2 );
		for ( i=0; i<NumMissiles; i++ )
	    {
			if ( Missiles[i].bActive )
		    {
				x = Missiles[i].x;
				y = Missiles[i].z;

				//Debug( "mis (%d,%d)", x, y );
				MoveToEx( hDrawDC, x, y, NULL );
				LineTo( hDrawDC, x,   y+1 );
//				LineTo( hDrawDC, x+1, y+1 );
//				LineTo( hDrawDC, x+1, y   );
//				LineTo( hDrawDC, x,   y   );
		    }
		}
	}

	if ( nActiveSparks )
	{
		SelectObject( hDrawDC, hPen1 );
		for ( i=0; i<NumSparks; i++ )
	    {
		    if ( Sparks[i].bActive )
			{
				x = Sparks[i].x;
				y = Sparks[i].y;
				//Debug( "spk (%d,%d)", x, y );
				MoveToEx( hDrawDC, x, y, NULL );
				LineTo( hDrawDC, x, y+1 );
			}
		}
    }
	
	if ( nActiveExplosions )
	{
		SelectObject( hDrawDC, hPen1 );
		for ( i=0; i<NumExplosions; i++ )
	    {
		    if ( Explosions[i].nActive )
			{
				x = Explosions[i].x;
				y = Explosions[i].y;
				//Debug( "exp (%d,%d)", x, y );
				MoveToEx( hDrawDC, x, y, NULL );
				LineTo( hDrawDC, x,   y+1 );
//				LineTo( hDrawDC, x+1, y+1 );
//				LineTo( hDrawDC, x+1, y   );
//				LineTo( hDrawDC, x,   y   );
			}
		}
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

	nSightRight -= nSightWidth;
	nSightX = nSightLeft;
	nSightZ = 300;

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
	int nDistance = pWorld->GetLength() * nZPixels;
	int x = (nViewX * nDistance / nViewDistance) / nXPixels;

	//Debug("sight pos=%d, x=%d\n", nSightX, x);
	return x;
}

//***********************************************************************
void CWeapon::SetMissileParams(WORD wRange, double dSpeed, int nNumFrames) {
//***********************************************************************
	int i;
	for (i=0; i<NumMissiles; i++) {
		Missiles[i].wRange = wRange;
		Missiles[i].dSpeed = dSpeed;
		Missiles[i].nFrames = nNumFrames;
	}
}

//***********************************************************************
void CWeapon::MoveSightLeft()
//***********************************************************************
{
	if (nSightX > nSightLeft)
	{
		nSightX -= nSightDelta;
	}
}

//***********************************************************************
void CWeapon::MoveSightRight() 
//***********************************************************************
{
	if (nSightX < nSightRight)
	{
		nSightX += nSightDelta;
	}
}

//***********************************************************************
void CWeapon::SightAutoMove()
//***********************************************************************
{
#ifdef SIGHT_KEEP_MOVING
	if (nActiveMissiles)
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
	MoveSparks();
	MoveExplosions();

	int i, x, y;
	if ( nActiveMissiles )
	{
		//SelectObject( hDrawDC, hPen2 );
		for ( i=0; i<NumMissiles; i++ )
	    {
			if ( Missiles[i].bActive )
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
	if ( nActiveSparks )
	{
		SelectObject( hDrawDC, hPen1 );
		for ( i=0; i<NumSparks; i++ )
	    {
		    if ( Sparks[i].bActive )
			{
				x = Sparks[i].x;
				y = Sparks[i].y;
				//Debug( "spk (%d,%d)", x, y );
				MoveToEx( hDrawDC, x, y, NULL );
				LineTo( hDrawDC, x, y+1 );
			}
		}
    }
	
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
HMCI CWeapon::OpenSound( LPSTR lpFile, LPSTR lpszAlias )
//***********************************************************************
{
	MCI_OPEN_PARMS mciopen;
	DWORD dwFlags;
	DWORD dwRes;
	FNAME szExpFileName;
	LPSTR lpFileName;

	if ( !GetApp()->FindContent( lpFile, szExpFileName ) )
		return NULL;

	lpFileName = szExpFileName;

	mciopen.lpstrDeviceType = NULL;
	mciopen.lpstrElementName = lpFileName;
    mciopen.dwCallback = NULL;
	if (lpszAlias)
		mciopen.lpstrAlias = lpszAlias;
	else
		mciopen.lpstrAlias = lpFile;
	dwFlags = MCI_OPEN_ELEMENT | MCI_OPEN_ALIAS;

	//if ( lpAlias )
	//	{
	//	mciopen.lpstrAlias = lpAlias;
	//	dwFlags |= MCI_OPEN_ALIAS;
	//	}

	//mciopen.dwCallback = (DWORD)MakeProcInstance( (FARPROC)MCICallback, App.GetInstance() );
	//dwFlags |= CALLBACK_FUNCTION;
	//dwFlags |= MCI_NOTIFY;
	if ( (dwRes = mciSendCommand(GetApp()->m_hDeviceWAV, MCI_OPEN, dwFlags, (DWORD)(LPSTR)&mciopen )) )
		{
		//MCIError(dwRes);
		return NULL;
		}

	return mciopen.wDeviceID;
}

//***********************************************************************
void CWeapon::PlaySound( HMCI hWave )
//***********************************************************************
{
	DWORD dwRes;
	DWORD dwFlags;
	MCI_PLAY_PARMS mciplay;

	if (hLastWave)
		dwRes = mciSendCommand( hLastWave, MCI_STOP, MCI_WAIT, NULL );

	//StopSounds();

	//if (hWave)
	//	MCIClose(hWave);

	//if ( hWave = MCIOpen(GetApp()->m_hDeviceWAV, lpFile, NULL/*lpAlias*/) )
	//{
	//	MCIPlay( hWave, GetApp()->GetMainWnd() );
	//}


	//if ( hWndNotify )
	//	{ // use a callback to notify the window
	//	mciplay.dwCallback = (DWORD)(LPTR)hWndNotify;
	//	dwFlags |= MCI_NOTIFY;
	//	}
	//else
	//	dwFlags |= MCI_WAIT;

	// Also...
	//dwFlags |= MCI_DGV_PLAY_REPEAT;
	//dwFlags |= MCI_DGV_PLAY_REVERSE;
	dwFlags = MCI_FROM | MCI_NOTIFY;
	mciplay.dwTo = 0;
	mciplay.dwFrom = 0;
	mciplay.dwCallback = (DWORD)(LPTR)hSceneWnd;


	if ( (dwRes = mciSendCommand( hWave, MCI_PLAY, dwFlags, (DWORD)(LPSTR)&mciplay )) )
	{
		hLastWave = NULL;
		//MCIError(dwRes);
		return;
	}

	hLastWave = hWave;
}

//***********************************************************************
void CWeapon::StopSounds()
//***********************************************************************
{
	DWORD dwRes;

	dwRes = mciSendCommand( hRocketWave, MCI_STOP, MCI_WAIT, NULL );
	dwRes = mciSendCommand( hExpSmallWave, MCI_STOP, MCI_WAIT, NULL );
	dwRes = mciSendCommand( hExpBigWave, MCI_STOP, MCI_WAIT, NULL );

	hLastWave = NULL;
}

//***********************************************************************
void CWeapon::CloseAllSounds()
//***********************************************************************
{
	CloseSound(hRocketWave);
	CloseSound(hExpSmallWave);
	CloseSound(hExpBigWave);

	hLastWave = NULL;
}

//***********************************************************************
void CWeapon::CloseSound( HMCI hWave )
//***********************************************************************
{
	DWORD dwRes;

	if (hWave)
	{
		if ( (dwRes = mciSendCommand( hWave, MCI_CLOSE, MCI_WAIT, NULL )) )
		{
			//MCIError(dwRes);
			Sleep(1);
			dwRes = mciSendCommand(hWave, MCI_CLOSE, MCI_WAIT, NULL);
		}
	}
}


//***********************************************************************
void CWeapon::CheckSound()
//***********************************************************************
{
	// If there are missiles or explosions in progress then don't check any further
	if ( nActiveMissiles || nActiveExplosions )
	{
		nAudioFrames = 12;
		return;
	}
	

	// If the sound track has been turned off
	if (bSoundTrackOff)
	{
		--nAudioFrames;

		if (nAudioFrames)
			return;

		// Make sure there are no sound fx playing
		// Note: Calling stop for one stops all...


		#ifdef SOUND_PLAYSOUND
			//pExpBigSnd->Stop();
		#else
			StopSounds();
		#endif

		// NOTE: We can not make mci calls to the video directly since we are probably
		// inside the draw proc at this point in time (MMTASK will freak).  So, we post 
		// a message to vcontrol instead.
//		PostMessage( GetDlgItem( hSceneWnd, IDC_VIDEO ), WM_VIDEOSOUND_ONOFF, ON, 0);

		bSoundTrackOff = FALSE;
	}
}

//***********************************************************************
void CWeapon::DrawSprite(int nPercent, int x, int y, PDIB pSprite, LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
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
	double dlx = nSrcPixWidth * (nPercent / 100.0) / 2.0;
	int lx = x - (int)((nSrcPixWidth * (nPercent / 100.0)) / 2.0);
	int uy = y - (int)((nSrcHeight * (nPercent / 100.0)) /2.0);

	// Clip source rect if on edge of destination bitmap
	SetRect(&SrcRect,  lx, uy, lx+nSrcPixWidth, uy+nSrcHeight);
	SetRect(&DestRect, 0, 0, nDestWidth, nDestHeight);
	IntersectRect(&ClipRect, &DestRect, &SrcRect);

	// Check for off screen
	if (IsRectEmpty(&ClipRect))
	{
		return;
	}

	// Set clipped upper-left sprite coordinates
	lx = ClipRect.left;
	uy = ClipRect.top;

	//if (uy == 0)
	//	DebugBreak();

	nDestXPos = lx;
	nDestYPos = uy;

	// Destination bitmap offset
	nDestOffset = (nDestHeight - uy - 1) * nDestByteWidth + lx - 1;

	TransColor = *(pSrc + nSrcOffset);

	// For all of the rows in the source bitmap
	for(nSrcY=0; nSrcY<nSrcHeight; nSrcY++)
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
			for(nSrcX=0; nSrcX<nSrcPixWidth; nSrcX++) 
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
		nSrcOffset -= nSrcByteWidth;
	}
}

//***********************************************************************
CRocketLauncher::CRocketLauncher() : CWeapon()
//***********************************************************************
{
	int i;
	for(i=0; i<6; i++)
	{
		pdibMissile[i] = NULL;
	}
	for(i=0; i<4; i++)
	{
		pdibExplode[i] = NULL;
	}
	for(i=0; i<8; i++)
	{
		pdibHitExplode[i] = NULL;
	}

	pRocketSnd = NULL;
	pExpSmallSnd = NULL;
	pExpBigSnd = NULL;

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

	if ( nActiveMissiles )
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
	#ifdef WDEBUG
	WeaponDebugEnd();
	#endif
	int i;
	for(i=0; i<6; i++)
	{
		if (pdibMissile[i])
			delete pdibMissile[i];
	}
	for(i=0; i<4; i++)
	{
		if (pdibExplode[i])
			delete pdibExplode[i];
	}
	for(i=0; i<8; i++)
	{
		if (pdibHitExplode[i])
			delete pdibHitExplode[i];
	}

	#ifdef SOUND_PLAYSOUND
		delete pRocketSnd;
		delete pExpSmallSnd;
		delete pExpBigSnd;
	#endif
}

//***********************************************************************
void CRocketLauncher::Open( HWND hSceneWindow )
//***********************************************************************
{
	CWeapon::Open(hSceneWindow);

	SetMissileParams(1000, 10, 6);
	nNumExpMissFrames = 4;
	nNumExpHitFrames = 7;
	nMinExpScale = 30;

	int i;

	// Load missile bitmaps
	for(i=0; i<6; i++ )
	{
		pdibMissile[i] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_ROCKET00+i), 
			GetApp()->m_hPal);
	}

	// Load miss explosion bitmaps
	for (i=0; i<4; i++)
	{
		pdibExplode[i] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_EXPLODE00+i), 
			GetApp()->m_hPal);
	}

	// Load hit explosion bitmaps
	for (i=0; i<8; i++)
	{
		pdibHitExplode[i] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_BIGEXP00+i),
			GetApp()->m_hPal);
	}

	#ifdef SOUND_PLAYSOUND
		pRocketSnd = new VSndMix();
		pRocketSnd->Load(IDC_ROCKETSND);
		pExpSmallSnd = new VSndMix();
		pExpSmallSnd->Load(IDC_EXPLSMSND);
		pExpBigSnd = new VSndMix();
		pExpBigSnd->Load(IDC_EXPLBGSND);
	#else
		CloseAllSounds();
		hRocketWave = OpenSound("rocket.wav", NULL);
		hExpSmallWave = OpenSound("expsmall.wav", NULL);
		hExpBigWave = OpenSound("expbig.wav", "big1");
	#endif

	#ifdef WDEBUG
		//WeaponDebugStart(hSceneWnd, this);
	#endif

}

//***********************************************************************
void CRocketLauncher::Draw( HDC hDrawDC, HPEN hPen1, HPEN hPen2 )
//***********************************************************************
{
	MoveMissiles();
	MoveSparks();
	MoveExplosions();

	int i, x, y;
	if ( nActiveMissiles )
	{
		SelectObject( hDrawDC, hPen2 );
		for ( i=0; i<NumMissiles; i++ )
	    {
			if ( Missiles[i].bActive )
		    {
				x = Missiles[i].x;
				y = Missiles[i].z;

				//pdibMissile->DCBlt( hDrawDC, x, y, pdibMissile->GetWidth(), pdibMissile->GetHeight(),
				//		0, 0, pdibMissile->GetWidth(), pdibMissile->GetHeight() );

		    }
		}
	}

	if ( nActiveSparks )
	{
		SelectObject( hDrawDC, hPen1 );
		for ( i=0; i<NumSparks; i++ )
	    {
		    if ( Sparks[i].bActive )
			{
				x = Sparks[i].x;
				y = Sparks[i].y;
				//Debug( "spk (%d,%d)", x, y );
				MoveToEx( hDrawDC, x, y, NULL );
				LineTo( hDrawDC, x, y+1 );
			}
		}
    }
	
	if ( nActiveExplosions )
	{
		SelectObject( hDrawDC, hPen1 );
		for ( i=0; i<NumExplosions; i++ )
	    {
		    if ( Explosions[i].nActive )
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
}


//***********************************************************************
void CRocketLauncher::OnVideoDraw(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***********************************************************************
{
	int nPercent;

	UpdatePosition();

	if (bOption2 == TRUE)
		SightAutoMove();

	MoveEnemies();
	MoveMissiles();
	MoveSparks();
	MoveExplosions();

	CheckSound();

	DrawSight(lpbi, lpDestBits);

	int i;

	DrawHorizon(lpbi, lpDestBits);

	if ( nActiveMissiles )
	{
		for ( i=0; i<NumMissiles; i++ )
	    {
			if ( Missiles[i].bActive )
		    {
				int nScaleFactor = 200;
				int nDist = Missiles[i].wTraveled;

				POINT Point = Point3DToPointDib(Missiles[i].x, Missiles[i].y, Missiles[i].z - WeaponPos.z);

				//Debug("missile x=%d, y=%d, z=%d, px=%d, py=%d\n", Missiles[i].x, Missiles[i].y, Missiles[i].z, Point.x, Point.y);
				// Figure scale percentage
				nPercent = 100 - (int)(((float)(nDist)*100) / nScaleFactor) ;
				if (nPercent > 100) {
					nPercent = 100;
				}

				if (Missiles[i].nFrames)
					DrawSprite(nPercent, Point.x, Point.y, pdibMissile[Missiles[i].nCurFrame], lpbi, lpDestBits);

				++Missiles[i].nCurFrame;
				if (Missiles[i].nCurFrame >= Missiles[i].nFrames)
					Missiles[i].nCurFrame = 0;
		    }
		}
	}

	if ( nActiveExplosions )
	{
		if (bStartVideoInterim)
		{
			//VideoStartInterim(1, 10, lpbi, lpDestBits);
			bStartVideoInterim = FALSE;
		}

		DrawExplosion(lpbi, lpDestBits);
    }

}

//***********************************************************************
void CRocketLauncher::MoveMissiles(void)
//***********************************************************************
{ // this function moves the missiles and does all the collision detection

	if ( !nActiveMissiles )
		return;

	// test if all missiles are active so that ready light on cannon can be
	// illuminated properly
	if (nActiveMissiles == NumMissiles)
	{// turn ready light red; no more missles can be fired
	}
	else
	{// turn ready light green; end else there is a missile ready
	}

	for ( int i=0; i<NumMissiles; i++ )
    {
	    if ( !Missiles[i].bActive )
	    	continue;

		// move the missile
		//int miss_x = (int)(/*(float)100 * */(float)Missiles[i].xv / (float)(Missiles[i].wTraveled /*+100*/));
		//if ((Missiles[i].x + miss_x)> 640)
		//	DebugBreak();


		//int miss_y = (Missiles[i].y += Missiles[i].yv);

		//miss_x =  20 * Missiles[i].x / Missiles[i].wTraveled;
		//int miss_x = (int)((float)100 * (float)x / (float)Missiles[i].wTraveled+100));
		//int miss_y =  (int)((float)100 * (float)180 / (float)(Missiles[i].wTraveled+100));
		
		//int miss_x = 0;
		//miss_y += nVanishY;
		
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
		double dDistance = nSightZ - WeaponPos.z;
		if (dDistance < 1)
			dDistance = 1;
		double Ratio = Missiles[i].wTraveled / dDistance;

		// For the first time missile is drawn, leave x 0 so it go from center
		//if ( Missiles[i].z <= WeaponPos.z )
		//{
		//	Missiles[i].x = 0;
		//}
		// Else, figure new x, z position based on distance
		//else
		{
			//Missiles[i].x = (int)(Ratio * GetSightWorldX() + WeaponPos.x);
			Missiles[i].x = (int)(Ratio * Missiles[i].nSightX + WeaponPos.x);
			Missiles[i].z = (int)(Ratio * pWorld->GetLength() + WeaponPos.z);
		}

		//Debug("dDistance = %d, miss x=%d, y=%d, z=%d\n",  (int)dDistance, Missiles[i].x, Missiles[i].y, Missiles[i].z);

		Missiles[i].wTraveled += (WORD)(Missiles[i].dSpeed);

		// If hit wall
		if ( pWorld->HitWall(Missiles[i].x, Missiles[i].z) )
		{
			Missiles[i].bActive = NO;
			--nActiveMissiles;

			// Start small explosion
			//StartExplosion(Missiles[i].x, Missiles[i].z, Missiles[i].wTraveled);
			StartExplosion(Missiles[i].x, Missiles[i].y, Missiles[i].z, 0);
		}

		// else test if hit enemy
		if ( HitEnemy( &Missiles[i] ) ) 
		{
			Missiles[i].bActive = NO;
			--nActiveMissiles;

			StartExplosion(Missiles[i].x, 1 , Missiles[i].z, 1);
			
			// Jump to shot with-out enemy
			PostMessage( GetDlgItem(hSceneWnd, IDC_VIDEO), WM_KEYDOWN, VK_HOME, 0 );

			//VideoColorEffect( RGB(255, 255, 255) );
			--nActiveEnemies;
		}
	}
}

//***********************************************************************
void CRocketLauncher::BigExplode()
//***********************************************************************
{
	//pExpBigSnd->Play();
	StartExplosion(WeaponPos.x, 0, WeaponPos.z, 1);
}

//***********************************************************************
BOOL CWeapon::HitEnemy(MISSILE *pMissile)
//***********************************************************************
{
	int i;
	int HalfWidth;
	int HalfDepth;

	for(i=0; i<nActiveEnemies; i++)
	{
		// see if in x
		HalfWidth = Enemy[i].Width / 2;
		if (pMissile->x > (Enemy[i].x-HalfWidth) && pMissile->x < (Enemy[i].x+HalfWidth) )
		{
		//  test if past z
			HalfDepth = Enemy[i].Depth / 2;
			if (pMissile->z > (Enemy[i].z-HalfDepth) )
			{
				// Hit
				return TRUE;
			}
		}
	}

	return FALSE;
}


//***********************************************************************
void CLaserBomb::Open( HWND hSceneWindow )
//***********************************************************************
{
	CWeapon::Open(hSceneWindow);

	SetMissileParams(1000, 15, 6);
	nNumExpMissFrames = 4;
	nNumExpHitFrames = 4;
	nMinExpScale = 30;

	// Laser burst
	pdibMissile[0] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LBURST00), 
		GetApp()->m_hPal);
	pdibMissile[1] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LBURST01), 
		GetApp()->m_hPal);
	pdibMissile[2] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LBURST02), 
		GetApp()->m_hPal);
	pdibMissile[3] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LBURST00), 
		GetApp()->m_hPal);
	pdibMissile[4] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LBURST01), 
		GetApp()->m_hPal);
	pdibMissile[5] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LBURST02), 
		GetApp()->m_hPal);
	
	// Load explosion bitmap
	pdibExplode[0] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LEXPLODE0), 
		GetApp()->m_hPal);
	pdibExplode[1] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LEXPLODE1), 
		GetApp()->m_hPal);
	pdibExplode[2] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LEXPLODE2), 
		GetApp()->m_hPal);
	pdibExplode[3] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LEXPLODE3), 
		GetApp()->m_hPal);

	pdibHitExplode[0] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LBIGEXP00), 
		GetApp()->m_hPal);
	pdibHitExplode[1] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LBIGEXP01), 
		GetApp()->m_hPal);
	pdibHitExplode[2] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LBIGEXP02), 
		GetApp()->m_hPal);
	pdibHitExplode[3] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_LBIGEXP03), 
		GetApp()->m_hPal);
	pdibHitExplode[4] = NULL;
	pdibHitExplode[5] = NULL;
	pdibHitExplode[6] = NULL;
	pdibHitExplode[7] = NULL;

	#ifdef SOUND_PLAYSOUND
		pRocketSnd = new VSndMix();
		pRocketSnd->Load(IDC_LASERBOMBSND);

		pExpSmallSnd = new VSndMix();
		pExpSmallSnd->Load(IDC_LASEREXPSM);
		pExpBigSnd = new VSndMix();
		pExpBigSnd->Load(IDC_LASEREXPBG);
	#else
		hRocketWave = OpenSound("lasburst.wav", NULL);
		hExpSmallWave = OpenSound("lexpbig.wav", NULL);
		hExpBigWave = OpenSound("lexpsmal.wav", NULL);
	#endif

	#ifdef WDEBUG
		//WeaponDebugStart(hSceneWnd, this);
	#endif

}

//***********************************************************************
void CMachineGun::Open( HWND hSceneWindow )
//***********************************************************************
{
	CWeapon::Open(hSceneWindow);

	SetMissileParams(1000, 100, 0);
	nNumExpMissFrames = 4;
	nNumExpHitFrames = 4;
	nMinExpScale = 30;

	for(int i=0; i<6; i++)
	{
		pdibMissile[i] = NULL;
	}

	// Load sparks bitmap
	pdibExplode[0] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_GUNSPARK00), 
		GetApp()->m_hPal);
	pdibExplode[1] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_GUNSPARK01), 
		GetApp()->m_hPal);
	pdibExplode[2] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_GUNSPARK02), 
		GetApp()->m_hPal);
	pdibExplode[3] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_GUNSPARK03), 
		GetApp()->m_hPal);

	pdibHitExplode[0] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_GBIGEXP00), 
		GetApp()->m_hPal);
	pdibHitExplode[1] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_GBIGEXP01), 
		GetApp()->m_hPal);
	pdibHitExplode[2] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_GBIGEXP02), 
		GetApp()->m_hPal);
	pdibHitExplode[3] = CDib::LoadDibFromResource( GetWindowInstance(hSceneWindow), MAKEINTRESOURCE(IDC_GBIGEXP03), 
		GetApp()->m_hPal);
	pdibHitExplode[4] = NULL;
	pdibHitExplode[5] = NULL;
	pdibHitExplode[6] = NULL;
	pdibHitExplode[7] = NULL;

	#ifdef SOUND_PLAYSOUND
		pRocketSnd = new VSndMix();
		pRocketSnd->Load(IDC_MGUNSND);

		pExpSmallSnd = NULL;
		pExpBigSnd = new VSndMix();
		pExpBigSnd->Load(IDC_EXPLBGSND);
	#else
		hRocketWave = OpenSound("mgun.wav", NULL);
		hExpSmallWave = NULL;
		hExpBigWave = OpenSound("expbig.wav", "big2");
	#endif

	#ifdef WDEBUG
		//WeaponDebugStart(hSceneWnd, this);
	#endif

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
	int x1, y1, z1;
	int x2, y2, z2;

	if (!bShowGrid)
		return;

	if (WeaponPos.z >= pWorld->GetLength())
		return;

	/***
    static int nTimes = 0;

	if (nTimes == 1)
		return;

	nTimes = 1;
	***/

	POINT p1, p2;

	// Draw a frame
	//DrawDibLine(0,0, lpbi->biWidth-1,0, lpbi, lpDestBits);
	//DrawDibLine(0, lpbi->biHeight-1, lpbi->biWidth-1, lpbi->biHeight-1, lpbi, lpDestBits);
	// Some weirdness here...maybe the compressor preserves the first column, because if I try and draw
	// in column 0, I don't see it!!! (believe me, I spent two hours on this)
	//DrawDibLine(1, 0, 1, lpbi->biHeight-1, lpbi, lpDestBits);
	//DrawDibLine(lpbi->biWidth-1, 0, lpbi->biWidth-1, lpbi->biHeight-1, lpbi, lpDestBits);

	/**** Draw vertical lines
	y1 = yGround; z1 = 1;
	y2 = yGround; z2 = nWorldLength - WeaponPos.z;
	***/
	int iLeft = nWorldWidth / 2 * -1;
	int iRight = nWorldWidth / 2;

	/***
	for(i=iLeft; i<=iRight; i+=10)
	{
		x2 = x1 = i;
		p1 = Point3DToPointDib(x1, y1, z1);
		p2 = Point3DToPointDib(x2, y2, z2);
		DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);
	}
	****/

	nWorldLength = pWorld->GetLength();
	// Draw back wall
	x1 = iRight; y1 = yGround; z1 = nWorldLength - WeaponPos.z;
	x2 = iLeft; y2 = yGround; z2 = nWorldLength - WeaponPos.z;
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);
	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);

	x1 = iLeft; y1 = yGround; z1 = nWorldLength - WeaponPos.z;
	x2 = iLeft; y2 = -12; z2 = nWorldLength - WeaponPos.z;
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);
	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);

	x1 = iLeft; y1 = -12; z1 = nWorldLength - WeaponPos.z;
	x2 = iRight; y2 = -12; z2 = nWorldLength - WeaponPos.z;
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);
	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);

	x1 = iRight; y1 = -12; z1 = nWorldLength - WeaponPos.z;
	x2 = iRight; y2 = yGround; z2 = nWorldLength - WeaponPos.z;
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);
	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);

	//**** Draw left wall ****
	x1 = iLeft; y1 = yGround; z1 = nWorldLength - WeaponPos.z;
	x2 = iLeft*2; y2 = yGround; z2 = nWorldLength - WeaponPos.z - (nWorldLength/2);
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);
	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);

	x1 = iLeft*2; y1 = yGround; z1 = nWorldLength - WeaponPos.z - (nWorldLength/2);
	x2 = iLeft*2; y2 = -12; z2 = nWorldLength - WeaponPos.z - (nWorldLength/2);
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);
	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);

	x1 = iLeft*2; y1 = -12; z1 = nWorldLength - WeaponPos.z - (nWorldLength/2);
	x2 = iLeft; y2 = -12; z2 = nWorldLength - WeaponPos.z;
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);
	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);

	// Draw right wall
	x1 = iRight*2; y1 = yGround; z1 = nWorldLength - WeaponPos.z - (nWorldLength/2);
	x2 = iRight; y2 = yGround; z2 = nWorldLength - WeaponPos.z;
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);
	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);

	x1 = iRight; y1 = -12; z1 = nWorldLength - WeaponPos.z;
	x2 = iRight*2; y2 = -12; z2 = nWorldLength - WeaponPos.z - (nWorldLength/2);
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);
	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);

	x1 = iRight*2; y1 = -12; z1 = nWorldLength - WeaponPos.z - (nWorldLength/2);
	x2 = iRight*2; y2 = yGround; z2 = nWorldLength - WeaponPos.z - (nWorldLength/2);
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);
	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);

	/**** Draw horizontal lines
	x1 = iLeft; y1 = yGround; 
	x2 = iRight; y2 = yGround;
	for(i=1; i<=nWorldLength-WeaponPos.z; i+=10)
	{
		z1 = z2 = i;
		p1 = Point3DToPointDib(x1, y1, z1);
		p2 = Point3DToPointDib(x2, y2, z2);
		DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uGridColor);
	}
	****/

	DrawEnemy(lpbi, lpDestBits);

	return;
}

//***********************************************************************
void CWeapon::SetShowGrid(BOOL bShow)
//***********************************************************************
{
	bShowGrid = bShow;
}

//***********************************************************************
void CWeapon::SetWorld(int nLength, int nWidth, int nYGround)
//***********************************************************************
{
	nWorldLength = nLength;
	nWorldWidth = nWidth;
	yGround = nYGround;
}

//***********************************************************************
void CWeapon::SetEnemy(int i, int nFrameDist, int nStartX, int nStartY, int nStartZ, 
					   int nEndX, int nEndZ, int nWidth, int nHeight, int nDepth)
//***********************************************************************
{
	nActiveEnemies = 1;

	Enemy[i].nFrameDist = nFrameDist;
	Enemy[i].StartX = nStartX;
	Enemy[i].StartZ = nStartZ;
	Enemy[i].EndX = nEndX;
	Enemy[i].EndZ = nEndZ;
	Enemy[i].x = nStartX;
	Enemy[i].y = nStartY;
	Enemy[i].z = nStartZ;
	Enemy[i].Width = nWidth;
	Enemy[i].Height = nHeight;
	Enemy[i].Depth = nDepth;
	Enemy[i].dDistance = sqrt( pow(nEndX - nStartX, 2) + pow(nEndZ - nStartZ, 2) );
	Enemy[i].wTraveled = 0;
}

//***********************************************************************
void CWeapon::MoveEnemies()
//***********************************************************************
{
	if ( lCurrentFrame > (lStartFrame + pWorld->CarStop()) || lCurrentFrame < (lStartFrame + pWorld->CarStart()) )
		return;

	int i;
	for( i=0; i<nActiveEnemies; i++)
	{
		// Adjust distance traveled (based on constant rate per frame)
		long lCurrentFrame = MCIGetPosition(lpVideo->hDevice);
		long lTemp = pWorld->CarStart();
		Enemy[i].wTraveled = (WORD)(Enemy[i].nFrameDist * (lCurrentFrame - lStartFrame - pWorld->CarStart()));

		// Compute new enemy position based on start, distance traveled, and distance to travel (dDistance)
		double dRatio = (double)(Enemy[i].dDistance - Enemy[i].wTraveled) / Enemy[i].dDistance;
		Enemy[i].x = (int)(dRatio * (double)Enemy[i].StartX) + Enemy[i].EndX;
		Enemy[i].z = (int)(dRatio * (double)Enemy[i].StartZ) + Enemy[i].EndZ;

	//Debug("enemy x=%d, z=%d\n", Enemy[i].x, Enemy[i].z);
	}
	
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

	z1 = z2 = (Enemy[i].z - (Enemy[i].Depth/2)) - WeaponPos.z;

	// bottom points
	x1 = Enemy[i].x + (Enemy[i].Width / 2);
	x2 = Enemy[i].x - (Enemy[i].Width / 2);
	y1 = y2 = Enemy[i].y + (Enemy[i].Height / 2);
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);

	// top points`
	y1 = y2 = Enemy[i].y - (Enemy[i].Height / 2);
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
	z1 = z2 = (Enemy[i].z + (Enemy[i].Depth/2)) - WeaponPos.z;
	y1 = y2 = Enemy[i].y + (Enemy[i].Height / 2);
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);
	y1 = y2 = Enemy[i].y - (Enemy[i].Height / 2);
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
	z1 = (Enemy[i].z - (Enemy[i].Depth/2)) - WeaponPos.z;
	z2 = (Enemy[i].z + (Enemy[i].Depth/2)) - WeaponPos.z;
	x1 = x2 = Enemy[i].x - (Enemy[i].Width / 2);
	y1 = y2 = Enemy[i].y + (Enemy[i].Height / 2);
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);
	y1 = y2 = Enemy[i].y - (Enemy[i].Height / 2);
	p3 = Point3DToPointDib(x1, y1, z1);
	p4 = Point3DToPointDib(x2, y2, z2);
	// top
	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uEnemyBoxColor);
	// bottom
	DrawDibLine(p4.x, p4.y, p3.x, p3.y, lpbi, lpDestBits, uEnemyBoxColor);

	// ** Right Side
	z1 = (Enemy[i].z - (Enemy[i].Depth/2)) - WeaponPos.z;
	z2 = (Enemy[i].z + (Enemy[i].Depth/2)) - WeaponPos.z;
	x1 = x2 = Enemy[i].x + (Enemy[i].Width / 2);
	y1 = y2 = Enemy[i].y + (Enemy[i].Height / 2);
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);
	y1 = y2 = Enemy[i].y - (Enemy[i].Height / 2);
	p3 = Point3DToPointDib(x1, y1, z1);
	p4 = Point3DToPointDib(x2, y2, z2);
	// top
	DrawDibLine(p1.x, p1.y, p2.x, p2.y, lpbi, lpDestBits, uEnemyBoxColor);
	// bottom
	DrawDibLine(p4.x, p4.y, p3.x, p3.y, lpbi, lpDestBits, uEnemyBoxColor);

	
	// ** Middle Square
	z1 = z2 = Enemy[i].z - WeaponPos.z;

	// bottom points
	x1 = Enemy[i].x + (Enemy[i].Width / 2);
	x2 = Enemy[i].x - (Enemy[i].Width / 2);
	y1 = y2 = Enemy[i].y + (Enemy[i].Height / 2);
	p1 = Point3DToPointDib(x1, y1, z1);
	p2 = Point3DToPointDib(x2, y2, z2);

	// top points
	y1 = y2 = Enemy[i].y - (Enemy[i].Height / 2);
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
	
	if (z == 0)
		z = 1;

	// Transform point to 2D view
	int nViewX = nViewDistance * x / z;
	int nViewY = nViewDistance * y / z;

	// Offset coordinates to bitmap position (in the world the center is 0,0)
	Point.x = 320 + nViewX;
	Point.y = nVanishY + nViewY;

	// KLUDGE (for now) (to fix centering of missiles) NOTE: This does not work...
	//if (Point.y > 296)
	//	Point.y = 280;

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
