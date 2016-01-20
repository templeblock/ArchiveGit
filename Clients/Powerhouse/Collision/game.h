#ifndef _game_h_
#define _game_h_

#include "worlds.h"
#include "weapon.h"
#include "collect.h"
#include "vworld.h"
#include "videofx.h"
#include "dirsnd.h"
#include "levels.h"
#include "ambientSnd.h"
#include "score.h"

#define DAMAGE_WALL_CRASH 1000	// Wall crash damage to car (sub from energy)

// Define how much ammo we start the game with
#define ROUNDS_PER_CLIP		1
#define AMMO_MGUN			(500 * ROUNDS_PER_CLIP)
#define AMMO_MISSILES		50
#define AMMO_MINES			50
#define AMMO_BOMBS			50
#define AMMO_JACKS			50
#define AMMO_LEROYS         50
#define AMMO_THUMPER        50
#define FUEL_DEC			 5		// Fuel consumption per frame

#define REVERSE_DIR(i) i+=6; if (i < 1) i+=12

class CGame;

class CGame
{
public:
	// Constructor
	CGame(HWND hWnd);
	// Destructor
	~CGame();

	// Methods
	void Init(void);
	void Free(void);
	void OnTimer(HWND hWnd, UINT id);
	BOOL OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);

private:
	LPSHOT GetNewCellShot(HWND hWnd, HWND hVideo );
	LPSHOT NewGetNewCellShot(HWND hWnd, LPSTR lpShotName);
	BOOL ManageGateway(HWND hWnd);
	BOOL ManageEntry(HWND hWnd);
	char LevelChar(int iLevel);
	BOOL SpeakingShot(char c);
public:
	BOOL Turn(HWND hWnd, int iEventCode);
	BOOL IndentShot( HWND hWnd, LPSHOT lpNewShot, LPSHOT lpShot );
private:
	BOOL GetShotVector( LPPOINT lpPoint1, LPPOINT lpPoint2, long lFlags );
	BOOL ShortestDistance( LPPOINT lpBeg, LPPOINT lpEnd, LPPOINT lpIn, LPPOINT lpOut, LPDOUBLE lpRate );
	void NewDirection(HWND hWnd, int iEvent, char cSpecial );
	void SetupHUD(HWND hWnd, BOOL bDisplay);
	BOOL PassThruPortal( HWND hWnd, CCell * pCell, int iPortal );
	void SetWeaponDisplay(HWND hWnd, int iWeapon);
	void SetWeaponDigitDisplay(HWND hWnd, int iValue);
	void SetKillDisplay(HWND hWnd, int iValue, BOOL bForcePaint);
	void SetHealthDigitDisplay(HWND hWnd, long lValue);
	void LoadCustomLevel(HWND hWnd);
	void InitWeapons(HWND hWnd);
public:
	BOOL LoadLevelFile(HWND hWnd, LPSTR lpFileName, int iLevel);
	void FireWeapon(HWND hWnd, WORD wFlags);
	BOOL SetAmmo(HWND hWnd, char cCode);
	void HitHellion(void);
	void HitThwart(void);
	void HitViralion(void);
	void HitMedicator(void);
	void HitDraniac(void);
	void HitKiller(void);
	void HitSlamatron(void);
	void HitPsychotech(void);
	void ZapCurrentPortal(void);

private:
	void BlowupRadar(BOOL bBlowup);
	void ShowLevel6Clue(HWND hWnd);

private:
	HWND m_hWnd;

	// Game state variables
public:
	long lEnergy;
private:
	long lHealthSpine;
	BOOL bDieing;
	BOOL bInGate;				// Flag set when player is in a gate
	BOOL m_bLowFuelWarn;		// Set when low fuel warning is on
	BOOL m_bFire;				// auto fire flag

	// Video
public:
	HWND hVideo;
	LPVIDEO lpVideo;
	SHOTID lSaveShot;
	VideoWorld VWorld;			// video world
	VideoFx * pVidFx;			// Turning effects

private:
	LPSHOT lpLastShot;

	// Sounds
	CDirSnd MusicOne;			// Main music track
	CDirSnd Portal;				// Through portal
	CDirSnd WallRub;			// Car turning rubbing into wall
	CDirSnd SwitchSnd;
	CDirSnd PlayCar;			// Main driving sound for players car
	//CAmbientSnd AmbSnds;

	// Kills and score
	int iKills;
	CScore Score;

	// "World" cell map of the level
public:
	CWorld World;
	CWorld * lpWorld;

private:
	// Radar
	HWND hRadar;
	BOOL bRadarFull;			// Radar full size flag
	RECT RadarRect;				// Radar rectangle
	BOOL m_bScoreUp;

public:
	// Levels and collectibles
	CCollectibles Collect;
	CLevelState Levels;

private:
	DWORD m_dwVirusStart;
	long m_lVirusDur;

	// Weapons
public:
	CWeaponSystem Weapons;
};

#endif // _game_h_
