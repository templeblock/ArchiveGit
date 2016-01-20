#ifndef _Weapon_h
#define _Weapon_h

#include "vidobsrv.h"
#include "Vworld.h"
#include "DirSnd.h"
#include "Data3D.h"
#include "Cells.h"

const int CMND_CARCRASH = 1;
const int CMND_WALLCRASH = 2;

const int CMND_ACTION = 99;
const int CMND_SHOOT = 100;

const int ACTIVE_NO	= 0;
const int ACTIVE_NORMAL = 1;
const int ACTIVE_HIT = 2;

const int NumExplosions = 10;
const int NumMissiles = 8;

const int SightWidth = 8;

const int MaxCracks = 20;
const int MaxFF = 4;
const int MaxExp = 12;
const int MaxMiss = 4;
const int MaxScoreLen = 20;

const int MaxWeapon = 9;

const int LEROI_MAX_POWER = 2000;

enum WeaponType {
	GunWeapon,
	MissileWeapon,
	BombWeapon,
	JackWeapon,
	HoverWeapon,
	AerialWeapon
};

typedef struct Pos_type
{
	int x;
	int y;
	int z;
} POS;

// Bullet cracks
typedef struct BCrackType
{
	int x;
	int y;
	int nType;
} BCRACK;


// explosions
typedef struct explosion_typ
{
	int x;                   // x position
	int y;                   // y position
	int z;					 // z position
	int xv;                  // x velocity
	int yv;                  // y velocity
	int nActive;             // the state of the particle
	double dDistance;		 // distance of the explosion 
	int nType;
	int nFrames;			 // Number of frames in the explosion
	int nCurFrame;			 // Current frame in the explosion
	PDIB *pdibFrames;		 // Bitmaps of explosion
} EXPLOSION;

// missiles
typedef struct missile_typ
{
	int  x;                  // x position (left - right)
	int  y;                  // y position (up - down)
	int  z;					 // z postion  (forward - backward)
	int  nSightX;			 // x position of the sight when missile fired
	WORD wRange;			 // Range of the missile (in feet)
	WORD wTraveled; 		 // Distance the missile has traveled (in feet)
	double dSpeed;    		 // Speed of the missile (in feet per milliseconds)
	int  nActive;            // the state of the particle
	WeaponType Type;		 // type of weapon
	int nFrames;			 // number of frames
	int nCurFrame;			 // current frame of missile
	int nEnemyIndex;		 // index of enemy hit
	int xStep;				 // x increment per frame
	int zStep;				 // z increment per frame
	BOOL bBehind;			 // Behind player?
} MISSILE;

// weapon position
typedef struct {			
	int x;					// Right and left
	int y;					// up and down
	int z;					// Forward and backward (y is up and down)
	double nFrameDist;		// Distance to move each frame (in feet)
} WEAPONPOS;

// enemy shot spacing
typedef struct {
	int nCrackWait;		// Frames to wait for next crack
	int nShotWait;		// Frames to wait for next shot
} ENEMYSHOTSPACE;

class CRocketLauncher;
class CWeaponSystem;

class CWeapon : public VideoObserver
{
friend CWeaponSystem;
public:

	CWeapon();
	~CWeapon();

	void InitStaticObjects(void);
	void FreeStaticObjects(void);

	virtual void Open( HWND hSceneWindow );

	BOOL Start( void );
	virtual BOOL Fire( BOOL bRepeat, BOOL bGetMouse = FALSE );
	BOOL Repeat( void );
	void SetType(WeaponType Weapon);
	void NewCell(char cCellType);

	void SetMissileParams(WORD wRange, double dSpeed, int nNumFrames, WeaponType Type);

	virtual void MoveSightLeft();
	virtual void MoveSightRight();
	virtual void SightAutoMove();
	virtual void MoveSight(int x);

	virtual void OnVideoOpen();
	virtual void OnVideoClose();
	virtual void OnVideoDraw(LPBITMAPINFOHEADER lpFormat, LPTR lpData);

	void SetShowGrid(BOOL bShow);
	//void SetWorld(int nLength, int nWidth, int nYGround);
	void SetWorld(int _nNumWalls, WALL *_pWalls, int nYGround);
	//void SetEnemy(int i, int nFrameDist, int nStartX, int nStartY, int nStartZ, 
	//	int nEndX, int nEndZ, int nWidth, int nHeight, int nDepth);
	void ClearEnemies();

	void SetShot(LPSHOT lpShot);

	void Enable(BOOL bEnable);
	BOOL Enabled(void) { return bEnabled; }
	virtual void BigExplode() {}

	void IncSightSpeed();
	void ResetSightSpeed();

	void SetUseDebug( BOOL _bUseDebug ) {bUseDebug = _bUseDebug; }

	static void InitBulletCracks();
	static void AddBulletCrack(BOOL bSnd);
	static void ClearBulletCracks();

	long GetScore(void) {return m_lScore;}
	void SetScore(long lScore) {m_lScore = lScore;}

	BOOL IsOpen(void) { return (BOOL)hSceneWnd; } 
	void SetScene(HWND hWnd) { hSceneWnd = hWnd; }

	void AdjustRight(void);
	void AdjustLeft(void);
	void AdjustReverse(void);

	void SetSystem(CWeaponSystem *pSystem) {m_pSystem = pSystem;}
	void ResetText(void);

	int GetCurCrashDamage(void) {return m_nCurCrashDamage;}
	void DoCrashEffect(void);
	void StopFire() {m_nRepeat = 0;}
	void StartEffect(int nMills, int nColIndex);

protected:
	virtual void StartExplosion(int x, int y, int z, int nType);
	virtual void StartMissile(void);

	virtual void MoveMissiles(void);

	void SetWeaponPosition(int x, int y, int z);
	void SetSightParams(int nLeft, int nRight);

	void MoveExplosions(void);
	void DrawExplosion(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	virtual void DrawExplosions(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits) = 0;

	//void DrawSprite(int nPercent, int x, int y, PDIB pSprite, LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	void DrawSight(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);

	void UpdatePosition();

	int  GetSightWorldX();
	virtual int HitEnemy(MISSILE *pMissile);
	virtual int HitPlayer(MISSILE *pMissile);

	void DrawBulletCracks(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);

	BOOL TestEnemyFiring();
	//void StartLightening(int xSrc, int ySrc, int zSrc, int xDst, int yDst, int zDst);
	//void DrawLightening(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	//void StopLightening(void);
	void DrawWall(WALL *pWall, LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	BOOL HitWall(int x, int z);
	static void StartDrawScore(long lScore, int nFramesOn);
	static void DrawScore(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	int MissilesActive(void);
	static void DrawStrengths(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);

	void StartEnemyMissile(WeaponType Type, int x, int y, int z);
	void MoveEnemyMissiles(void);
	int GetCrashDamage(void);
	void StartBlackOut(int nMills);
	void StartWhiteOut(int nMills);
	void StartStrobe(int nMills);
	void StartElectric(int nFrames, int nElectricColor);
	void BlackOut(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	void WhiteOut(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	void Effect(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	void Strobe(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	void Electric(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);

// Statics
protected:
//	static VideoWorld  *pWorld;			// World of the weapons
	static LPVIDEO	lpVideo;			// Pointer to associated video
	static int		nWorldRef;			// Reference to world (how many objects reference it)
	static int		nViewDistance;		// Distance from viewer to screen (in pixels)
	static int		nXPixels;
	static int		nYPixels;
	static int		nZPixels;
	static long		lCurrentFrame;		// Current video frame
	static int		nExpScaleFactor;	// Explosion scale factor
	static int		yGround;			// y coord of where the ground appears to be
	static int		nWorldLength;		// Length from wall to wall on the z axis (straight ahead)
	static int		nWorldWidth;		// Width from wall to wall on the x axis (left to right)
	static int		nMissileRange;		// Range of missile
	static UINT		uEnemyColor;		// Color of enemy rectangle (if vector drawn)
	static UINT		uEnemyBoxColor;		// Color of enemy box
	static int		nActiveEnemies;		// Number of enemies
	static int      nLastActiveEnemies; // Number of last enemies
	static BOOL		bShowGrid;			// Show world grid

	static BOOL     bEnabled;			// Weapons enabled or not
	static int		nNumCracks;			// Number of bullet cracks
	static BCRACK	Cracks[MaxCracks];	// List of bullet cracks
	static PDIB		pdibCracks[2];		// Bullet crack bitmaps
	static CDirSnd *pShotSnd;			// Opponent shot snd
	static CDirSnd *pCrackSnd;
	static PDIB		pdibHitFF[MaxFF];	// Force field
	static PDIB		pdibExp[MaxExp];
	static PDIB		pdibMiss[MaxMiss];

	static long	m_lScore;				// Score to be drawn
	static int  m_nScFramesOn;			// # of frames the score stays on
	static int  m_nScStartX;			// Score start x position
	static int  m_nScWidth;				// Score digit width
	static int  m_nScoreLen;			// Score length
	static char m_szScore[MaxScoreLen+1];  // Score bytes
	static PDIB m_pdibScore[MaxScoreLen];  // Pointers to bitmap socre digits
	static PDIB m_pdibDigits[10];		// Source bitmap digits

	static CDirSnd *m_pCarSnds[3];		// Opponent car sounds
	static BOOL m_bOppsPossible;		// New opponents possible flag
	static CDirSnd *m_pCarCrashSnd;	
	static CDirSnd *m_pWallCrashSnd;

	static int  m_nCrackCount;
	static int  m_nWaitJump;			// Counter for waiting to jump to shot

	static HWND hSceneWnd;
	static int  m_nPlayerSaveZ;

	static BOOL m_bTextOn;

	//static BOOL m_bLighteningOn;
	//static int m_xSrc;
	//static int m_ySrc;
	//static int m_zSrc;
	//static int m_xDst;
	//static int m_yDst;
	//static int m_zDst;

	static MISSILE EnemyMissiles[NumMissiles];

	static CRocketLauncher *m_pSpineKiller; // Spine killer weapon

	static CWeaponSystem *m_pSystem;
	static int m_nCurCrashDamage;
	static int   m_nBlackOutDur;
	static DWORD m_dwBlackOutStart;
	static int   m_nWhiteOutDur;
	static DWORD m_dwWhiteOutStart;
	static int   m_nEffectDur;
	static DWORD m_dwEffectStart;
	static int   m_nEffectColor;
	static BOOL  m_bStrobe;
	static BOOL  m_bStrobeBlack;
	static int   m_nElectricFrames;
	static int   m_nElectricColor;
	static int   m_nColorCycle;

protected:
	EXPLOSION	Explosions[NumExplosions];
	int			nActiveExplosions;
	int			nNumExpMissFrames;
	int			nNumExpHitFrames;
	int			nMinExpScale;

	WEAPONPOS	WeaponPos;

	MISSILE		Missiles[NumMissiles];
	//int		nActiveMissiles;

	ENEMY		Enemy[nMaxEnemies];
	ENEMYSHOTSPACE EnemyShotSpace[nMaxEnemies];

	int			nSightWidth;
	int			nSightLeft;
	int			nSightRight;
	int			nSightX;
	int			nSightZ;
	int			nSightDelta;
	double		dSightSpeed;
	UINT        uSightColor;
	UINT		uSightBorderColor;
	UINT        uSightFiringColor;

	long lStartFrame;
	long lEndFrame;

	CDirSnd *pRocketSnd;
	CDirSnd *pExpSmallSnd;
	CDirSnd *pExpBigSnd;
	CDirSnd *pAttackSnd;

	BOOL    bSoundTrackOff;

	int     nNumWalls;
	WALL    Walls[nMaxWalls];

	BOOL	bUseDebug;
	WeaponType Type;		 // type of weapon

	int     m_nPower;	// Power of the weapon
	BOOL    m_bSightOff;

	CCell *m_pCell;
	int    m_iDir;

	int   m_nRepeat;

public:
	void SetViewDistance(int nViewDistance);
	void SetXPixels(int nPixels);
	void SetYPixels(int nPixels);
	void SetZPixels(int nPixels);
	void SetGroundY(int nPos);
	void SetGridLength(int nLength);
	void SetGridWidth(int nWidth);
	void SetExpScaleFactor(int nFactor);
	void StopSounds(void);

protected:
	UINT		uGridColor;
	void DrawHorizon(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	void Draw3DGrid(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	void DrawPoint(int x, int y, LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	POINT Point3DToPointDib(int x, int y, int z);
	void DrawDibLine(int x1, int y1, int x2, int y2, LPBITMAPINFOHEADER lpbi, LPTR lpDestBits, UINT uColor);

	WORD ClipCode(int x, int y, LPBITMAPINFOHEADER lpbi);
	BOOL ClipLine(int *px1, int *py1, int *px2, int *py2, LPBITMAPINFOHEADER lpbi);

	void MoveEnemies();
	virtual void DrawMissiles(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits) = 0;
	virtual void DrawEnemy(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	BOOL BehindEnemy(int z, POINT Point);
};


class CRocketLauncher : public CWeapon {
public:
	CRocketLauncher();
	~CRocketLauncher();

	virtual void Open( HWND hSceneWindow );
	virtual void MoveMissiles(void);

	virtual void OnVideoDraw(LPBITMAPINFOHEADER lpFormat, LPTR lpData);
	virtual BOOL Fire( BOOL bRepeat, BOOL bGetMouse = FALSE );
	virtual void BigExplode();

protected:
	virtual void StartExplosion(int x, int y, int z, int nType);
	int LoadBitmaps( ITEMID idFire );
	void DrawMissiles(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	void DrawEnemyMissiles(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	void DrawExplosions(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);

	PDIB pdibMissile[6];
};

class CScrapGun : public CRocketLauncher {
public:
	virtual void Open( HWND hSceneWindow );
};

class CMachineGun : public CRocketLauncher {
public:
	virtual void Open( HWND hSceneWindow );
};

class CLaser : public CRocketLauncher {
public:
	virtual void Open( HWND hSceneWindow );
};

class CMissile : public CRocketLauncher {
public:
	virtual void Open( HWND hSceneWindow );
};

class CMine : public CRocketLauncher {
public:
	virtual void Open( HWND hSceneWindow );
};

class CBomb : public CRocketLauncher {
public:
	~CBomb();
	virtual void Open( HWND hSceneWindow );
};


class CJack: public CRocketLauncher {
public:
	virtual void Open( HWND hSceneWindow );
	virtual void MoveMissiles(void);
};

class CLeroy: public CRocketLauncher {
public:
	virtual void Open( HWND hSceneWindow );
	virtual void MoveMissiles(void);
};

class CThumper : public CRocketLauncher {
public:
	virtual void Open( HWND hSceneWindow );
	virtual void MoveMissiles(void);
};

void DrawSprite(int nPercent, int x, int y, PDIB pSprite, LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);

void RotateRight(int *px, int *py);
void RotateLeft(int *px, int *py);


class CWeaponSystem
{
public:
	CWeaponSystem();
	~CWeaponSystem();

	void Init(HWND hSceneWindow);
	void Destroy();

	void SetAmmo(int nIndex, int nAmount);
	void AddAmmo(int nIndex, int nAmount);
	void DecAmmo(int nIndex, int nAmount);
	int  GetAmmo(int nIndex);

	void SetCurWeapon(int nIndex);
	int  GetCurWeapon(void) {return nCurWeapon;}

	CWeapon *CurWeapon(void) {return pWeapon[nCurWeapon];}

	void MoveMissiles(void);
	void MoveExplosions(void);
	void DrawMissiles(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	void DrawExplosions(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);


private:
	CWeapon *pWeapon[MaxWeapon];
	int Ammo[MaxWeapon];
	int nCount;
	int nCurWeapon;
};

#endif
