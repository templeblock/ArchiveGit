#ifndef Weapon_h
#define Weapon_h

#include "vidobsrv.h"
#include "Vworld.h"
#include "vsndmix.h"

const int NumSparks = 1;
const int NumExplosions = 10;
const int NumMissiles = 50;
const int NumEnemies = 3;

const int SightWidth = 8;

enum WeaponType {
	GunWeapon,
	RocketWeapon
};

typedef struct Pos_type
{
	int x;
	int y;
	int z;
} POS;

// sparks
typedef struct spark_typ
        {
        int x;                   // x position
        int y;                   // y position
        int xv;                  // x velocity
        int yv;                  // y velocity
        int bActive;              // the state of the spark
        int counter_1;           // used for counting
        int threshold_1;         // the counters threshold
        int threshold_2;
        } SPARK;

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
        int  bActive;            // the state of the particle
		WeaponType Type;		 // type of weapon
		int nFrames;			 // number of frames
		int nCurFrame;			 // current frame of missile
        } MISSILE;

// weapon position
typedef struct {			
	int x;					// Right and left
	int y;					// up and down
	int z;					// Forward and backward (y is up and down)
	double nFrameDist;		// Distance to move each frame (in feet)
} WEAPONPOS;

typedef struct {
	int  nFrameDist;		// Distance the enemy travels per frame
	WORD wTraveled;			// Distance the enemy has traveled
	int StartX;				// Starting x coord
	int StartZ;				// Starting z coord
	int EndX;				// Ending x coord
	int EndZ;				// Ending z coord
	int x;					// Current x coord
	int y;					// Current y coord
	int z;					// Current z coord
	int Width;				// Width of enemy (x axis)
	int Height;				// Height (y axis)
	int Depth;				// Depth (z axis)
	double dDistance;		// Total distance the enemy will travel
} ENEMY;

class CWeapon : public VideoObserver {

public:

	CWeapon();

	virtual void Open( HWND hSceneWindow );

	BOOL Start( void );
	virtual BOOL Fire( BOOL bRepeat );
	BOOL Repeat( void );
	void End( void );
	void SetType(WeaponType Weapon);
	void Close( void );

	virtual void Draw( HDC hDC, HPEN hPen1, HPEN hPen2 );

	void SetMissileParams(WORD wRange, double dSpeed, int nNumFrames);

	virtual void MoveSightLeft();
	virtual void MoveSightRight();
	virtual void SightAutoMove();

	virtual void OnVideoOpen();
	virtual void OnVideoClose();
	virtual void OnVideoDraw(LPBITMAPINFOHEADER lpFormat, LPTR lpData);

	void SetShowGrid(BOOL bShow);
	void SetWorld(int nLength, int nWidth, int nYGround);
	void SetEnemy(int i, int nFrameDist, int nStartX, int nStartY, int nStartZ, 
		int nEndX, int nEndZ, int nWidth, int nHeight, int nDepth);

	void SetShot(LPSHOT lpShot);

	void Enable(BOOL bEnable);
	virtual void BigExplode() {}

protected:
	void StartSpark(int x, int y);
	virtual void StartExplosion(int x, int y, int z, int nType);
	virtual void StartMissile(void);

	virtual void MoveMissiles(void);

	void SetWeaponPosition(int x, int y, int z);
	void SetSightParams(int nLeft, int nRight);

	void MoveSparks(void);
	void MoveExplosions(void);
	void DrawExplosion(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);

	void DrawSprite(int nPercent, int x, int y, PDIB pSprite, LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	void DrawSight(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);

	void UpdatePosition();

	int  GetSightWorldX();
	virtual BOOL HitEnemy(MISSILE *pMissile);
	void CheckSound();
	HMCI OpenSound( LPSTR lpFile, LPSTR lpszAlias );
	void PlaySound( HMCI hWave );
	void StopSounds();
	void CloseSound( HMCI hWave );
	void CloseAllSounds();

// Statics
protected:
	static LPVIDEO lpVideo;			// Pointer to associated video
	static VideoWorld  *pWorld;		// World of the weapons
	static int		nWorldRef;		// Reference to world (how many objects reference it)
	static int		nViewDistance;	// Distance from viewer to screen (in pixels)
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
	static BOOL		bShowGrid;			// Show world grid

	static BOOL     bEnabled;			// Weapons enabled or not

protected:
	HWND hSceneWnd;

	SPARK		Sparks[NumSparks];
	int			nActiveSparks;

	EXPLOSION	Explosions[NumExplosions];
	int			nActiveExplosions;
	int			nNumExpMissFrames;
	int			nNumExpHitFrames;
	int			nMinExpScale;

	WEAPONPOS	WeaponPos;

	MISSILE		Missiles[NumMissiles];
	int			nActiveMissiles;

	ENEMY		Enemy[NumEnemies];

	int			nSightWidth;
	int			nSightLeft;
	int			nSightRight;
	int			nSightX;
	int			nSightZ;
	int			nSightDelta;
	UINT        uSightColor;
	UINT		uSightBorderColor;
	UINT        uSightFiringColor;

	long lStartFrame;
	long lEndFrame;

	VSndMix *pRocketSnd;
	VSndMix *pExpSmallSnd;
	VSndMix *pExpBigSnd;

	HMCI     hRocketWave;				// Weapons sounds
	HMCI     hExpSmallWave;
	HMCI     hExpBigWave;	

	BOOL    bSoundTrackOff;

	BOOL    bStartVideoInterim;

public:
	void SetViewDistance(int nViewDistance);
	void SetXPixels(int nPixels);
	void SetYPixels(int nPixels);
	void SetZPixels(int nPixels);
	void SetGroundY(int nPos);
	void SetGridLength(int nLength);
	void SetGridWidth(int nWidth);
	void SetExpScaleFactor(int nFactor);

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
	void DrawEnemy(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
};


class CRocketLauncher : public CWeapon {
public:
	CRocketLauncher();
	~CRocketLauncher();

	virtual void Open( HWND hSceneWindow );
	virtual void MoveMissiles(void);
	virtual void Draw(HDC hDC, HPEN hPen1, HPEN hPen2);

	virtual void OnVideoDraw(LPBITMAPINFOHEADER lpFormat, LPTR lpData);
	virtual BOOL Fire( BOOL bRepeat );
	virtual void BigExplode();

protected:
	virtual void StartExplosion(int x, int y, int z, int nType);

	PDIB pdibMissile[6];
	PDIB pdibExplode[4];

	PDIB pdibHitExplode[8];

};

class CLaserBomb : public CRocketLauncher {
public:
	virtual void Open( HWND hSceneWindow );
};

class CMachineGun : public CRocketLauncher {
public:
	virtual void Open( HWND hSceneWindow );
};

#endif
