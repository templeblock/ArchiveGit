#ifndef _Opponents_h
#define _Opponents_h

#include <windows.h>

// Some car classes that are special
#define OPPCLASS_GRUNT			1
#define OPPCLASS_DETONATOR		2
#define OPPCLASS_CONDRONE		3
#define OPPCLASS_NIGHTRON		4
#define OPPCLASS_HELLION		5
#define OPPCLASS_FLEA			6
#define OPPCLASS_PARHELION		7
#define OPPCLASS_THWART			8
#define OPPCLASS_FLASHER		9
#define OPPCLASS_DOGGER			10
#define OPPCLASS_ELECTROSHARK	11
#define OPPCLASS_MEDICATOR		12
#define OPPCLASS_VIRALION		13
#define OPPCLASS_DRANIAC		14
#define OPPCLASS_KILLER			15
#define OPPCLASS_REINDEER		16
#define OPPCLASS_RUDOLPH		17
#define OPPCLASS_SLAMATRON		18
#define OPPCLASS_DREADNAUGHT	19
#define OPPCLASS_PSYCHOTECH		20
#define OPPCLASS_PAINULATOR		21
#define OPPCLASS_PARAGON		22
#define OPPCLASS_SPINEIMMORTACON	23
#define OPPCLASS_SPINE			24

#define OPPNUM_REINDEER1 51
#define OPPNUM_REINDEER2 52
#define OPPNUM_REINDEER3 53
#define OPPNUM_REINDEER4 54
#define OPPNUM_REINDEER5 55
#define OPPNUM_REINDEER6 56
#define OPPNUM_REINDEER7 57
#define OPPNUM_REINDEER8 58
#define OPPNUM_REINDEER9 59

#define CRASHEFFECT_OPPEXPLODE 1

// Maximum number of opponents that can be engaged at any one time (on video)
const int MaxEngaged = 3;

const int OppNameMax = 40;		
const int OppDescMax = 120;
const int MaxPat = 20;
const int MaxShot = 50;
const int MaxShotCars = 20;

#define	TICKNORM 0
#define	TICKTURN 2
#define	TICKEND  4

class CCell;
class CCarShots;

class CCar;
typedef CCar * pCCar;

class CPlayer;
typedef CPlayer * pCPlayer;

class COpp;
typedef COpp * pCOpp;

class COpponentGroup;
typedef COpponentGroup * pCOpponentGroup;

#define CARSHOT_STATE_INSHOT 1

//typedef struct EngageRec;

typedef struct _EngageRec
{
  COpp *pOpp;			// Opponent in this engage rec
  _EngageRec *pPrev;	// Previous engage rec (circularly linked list like a clock)
  _EngageRec *pNext;	// Next engage rec
  int iDir;				// Direction of this engage rec
} EngageRec;

typedef struct _OppCarShot
{
	char  cType;				// Type of opponent (W, S, B)
	char  cGate;				// Gate code
	short nState;				// State of the shot car 0 or CARSHOT_STATE_INSHOT
	COpp *pOpp;					// Opponent
} OppCarShot;

typedef struct _OpponRec
{
	char  szName[OppNameMax];		// Opponent name
	char  szDescr[OppDescMax];		// Description of character
	char  cCarType;					// Video car look (W, S, or B)
	int   nClass;					// Car Class (1 - ???)
	int	  nStrength;				// Strength value
	int   nPoints;					// Points awarded when this opponent is killed
	int   nCollide;					// Collision behavior
	int   nSpeed;					// Speed (milliseconds per track slot)
	int   nProximityTolerance;		// Cell distance
	int   nStartDirection;			// Start direction
	char  szNormalDrivingPat[MaxPat];	
	char  szProximityDrivingPat[MaxPat];
	char  szNormalRestore[MaxPat];							
} OpponentRec;


//
// The CCar class moves a car in the virtual, hex-oriented world.  Cars move in 12
// different directions with the orientation or direction the same as a clock. 
//
class CCar
{
public:
	static void SetGroup(COpponentGroup *pGroup) {m_pGroup = pGroup;}
protected:
	static COpponentGroup *m_pGroup;

public:
	CCar();
	void  ChangeDirRel(short iDir);			// Change relative direction
	void  ChangeDirAbs(short iDir);			// Change absolute direction
	POINT GetCurPos(void);					// Get the current car position (used for painting
	POINT GetNewPos(void);	// Get a new car position based on a cell x,y
	virtual short IncTick(void);
	short GetTickType(void) {return m_iTrackTick;}
	void  StartTick(void);
	virtual BOOL EnterCell(void) = 0;
	POINT GetXYCell(void);
	void  SetLastDraw(POINT pnt) {pntLastDraw = pnt;}
	POINT GetLastDraw(void) {return pntLastDraw;}
	short GetDir(void) { return m_iDir; }
	POINT NextCellFromDir(int iCellx, int iCelly, int nDir);
	CCell *GetCell(void) { return m_pCell; }
	void SetCell(CCell *pCell) {m_pCell = pCell;}
	void SetHome(void) {m_iHomeDir=m_iDir; m_xHomeCell=m_xCell; m_yHomeCell=m_yCell;}
	short GetTick(void) {return m_iTrackTick;}
	BOOL  IsVisible(void);
	COpponentGroup *GetGroup(void) {return m_pGroup;}

protected:
	short m_iDir;				// Direction the car is moving 1-12. This also tells what
								// track the car is on. Opponents will only move on
								// tracks 2, 4, 6, 8, 10, and 12.
	short m_xCell;				// Horizontal cell coordinate
	short m_yCell;				// Vertical cell coordinate
	short m_iTrackTick;			// Position on a track with within a cell. 0 is the start,
								// 2 is the center, 4 is the end.
	BOOL  m_bStayOnTick;		// Flag for staying on a tick (stopping)
	int   m_xPixel;				// Horizontal pixel position on the screen
	int   m_yPixel;				// Vertical pixel position on the screen
	POINT pntLastDraw;			// Point where the car was last drawn

	CCell *m_pCell;				// Pointer to current cell the car is in

	short m_iHomeDir;			// Starting home direction
	short m_xHomeCell;			// home cell x position
	short m_yHomeCell;			// home cell y position
};


//
// CPlayer - Player car class
//
class CPlayer : public CCar
{
public:
	CPlayer();

public:
	void SetXYCell(int xCell, int yCell);
	void SetInGate(void);
	void FixDir(void);
	short IncTick(void);
	void Forward(void);
	void TurnLeft(char cSpecial);
	void TurnRight(char cSpecial);
	void TurnBack(void);
	BOOL EnterCell(void);
	CCell *SetNewShotCell(void);
};


//
// Driving mode values - used to determine how the car is driving
//
#define DRIVEMODE_NORMAL		0x0001	// Normal driving pattern used
#define DRIVEMODE_PROXIM		0x0002	// Proximity driving pattern used
#define DRIVEMODE_RESTORE		0x0003	// Proximity Restoration driving patten used
#define DRIVEMODE_RECOVER		0x0004	// Recovering from engagement with player

#define DRIVEMODE_STAY			0x0100	// Doesn't move, stays put
#define DRIVEMODE_POSITION		0x0200	// Positions outside shot cell
#define DRIVEMODE_ENGAGED		0x0300  // Engaged with player
#define DRIVEMODE_DISENGAGED	0x0400  // Disengaging from player
#define DRIVEMODE_DESTROYED     0x0500  // Opponent has been marked for destruction

#define DRIVEMODEMASK_TICK      0x00FF	// Used as a mask to see if the car should move to
										// the next tick on it's direction path

// Tests given drive mode to see if the car should move on the next
// tick on it's direction path
#define DRIVEMODETEST_MOVE(iMode) ((iMode) & DRIVEMODEMASK_TICK)

//
// COpp - Opponent car class
//
class COpp : public CCar
{
friend class COpponentGroup;

protected:
	short m_iRecIdx;		// Opponent record index
	DWORD m_dwLastMoveTime;	// Time the car last moved
	int   m_nVideoPos;		// Position in video from left to right. This value is used to find
							// this opponent when hit by a weapon.  The pEngaged[] array is searched.
	short m_iInstPos;		// Current instruction position
	char *m_pszDrivePat;    // Current driving pattern
	short m_iInstLen;		// Length of instruction string
	short m_iPoints;		// Points awarded when the opponent is killed
	short m_iStrength;		// Strength of opponent
	short m_iSpeed;			// Speed
	short m_iDriveMode;		// Driving mode 0=normal, 1=proximity, 2=restore
	short m_iLastDriveMode; // Last driving mode
	char  m_cCarType;		// Car type
	short m_iClass;			// Car class
	char  *m_pszName;		// Car class name

	int   m_x;				// X position in a cell in relation to player
	int   m_y;				// Y position
	int   m_z;				// Z position
	
	COpp *pNext;	// Next opponent
	COpp *pPrev;	// Previous opponent

public:
	COpp();
	
	short IncTick(void);
	void StartMove(void);				// Start move (initializes moving)
	virtual BOOL Move(DWORD dwTime);	// Move opponent

	int  GetPoints(void) { return m_iPoints; }	// Get points of this opponent
	void SetHit(short iAmount);					// Set Opponent hit (take away strength)
	void SetNextMoveInstruct(void);
	char GetMoveInstruct(void);
	BOOL ExecuteMoveInstruct(void);

	COpp *GetNext(void) { return pNext; }

	int  GetIndex(void) {return m_iRecIdx;}
	int  GetClass(void) {return m_iClass;}
	BOOL EnterCell(void);
	BOOL GetOut(void);
	COpp *Next(void) {return pNext;}
	BOOL CheckHome(void);
	void SetBestDir(int xSrcCell, int ySrcCell, int xDestCell, int yDestCell);
	void SetHomeDir(void);
	void SetCowardDir(void);
	void SetAttackDir(void);
	void SetClockwisePerimDir(void);
	void SetValidMoveDir(void);
	BOOL ValidMoveCell(CCell *pCell);
	void SetDriveMode(int iMode);
	short GetDriveMode(void) { return m_iDriveMode; }
	void SetPrevDriveMode(void);
	BOOL InProximity(void);
	BOOL MaxProxim(void);
	OppCarShot *CheckForShot(void);
	void MoveToShotCell(void);			// Move to shot cell, stoping at gate
	void SendIn(void);					// Send opponent into player cell
	void AdjustTick(void);		// Adjust tick - used to not pile cars on top of each other
	BOOL OnTick(int iTick, int xCell, int yCell); // Test if a car is on a tick in a cell
	BOOL JustDestroyed(void) {return m_iDriveMode == DRIVEMODE_DESTROYED;} 
	void FindNewHome(void);
};


//
// Opponent generator class
//
class COppGenerator
{
friend class COpponentGroup;

public:
	COppGenerator(CCell *pCell, DWORD dwFrequency, COpponentGroup *pGroup,
		int iFirstOpp, int iLastOpp);

	void Check(DWORD dwTime, COpponentGroup *pGroup);

	void On(void) {m_bOn = TRUE;}
	void Off(void) {m_bOn = FALSE;}

private:
	COppGenerator *m_pNext;

	DWORD m_dwFrequency;
	DWORD m_dwLastCheck;		// Last time it checked to see if it could 
								// generate an opponent
	CCell *m_pCell;				// Cell that the generator sits in
	BOOL   m_bOn;				// Is it on?
	int    m_iFirstOpp;			// First opponent to generate
	int    m_iLastOpp;			// Last opponent to gnerate
	int    m_iLastIndex;		// Last opponent index
};


//
// Opponent behind player
//
typedef struct _OppBehind
{
	char  cType;				// Type of opponent (W, S, B)
	char  cGate;				// Gate code
	short nState;				// State of the shot car 0 or CARSHOT_STATE_INSHOT
	COpp *pOpp;					// Opponent
} OppBehind;

//
// COpponentGroup - Class for opponents as a group
//
class COpponentGroup
{
friend class COpp;
friend class CPlayer;
friend class COppGenerator;

public:	
	COpponentGroup();
	~COpponentGroup();

	BOOL LoadRecs(char *pszFileName);		// Load opponent records
	COpp *CreateOpp(short iId, short iCellx, short iCelly);
	void  CreateGenerator(CCell *pCell, DWORD dwFrequency, int iFirstOpp, int iLastOpp);
	void  CheckGenerators(void);
	void  Pause(void);
	void  UnPause(void);
	BOOL  MoveNormal(void);
	void  ReleaseShotCars(void);
	char *BuildShot(void);
	char *GetShotCellPrefix(void);
	void  MoveShot(void);

	void DeleteOpps(void);
	void DeleteGenerators(void);
	COpp *GetFirst(void) { return m_pFirstOpp; } // Gets the first opponent in the linked list

	void SetShotCell(CCell *pCell) {m_pShotCell = pCell;}

	CCell *GetShotCell(void) { return m_pShotCell; }

	OpponentRec *Rec(int idx) 
		{ if (idx >= m_iNumRecs) return NULL; else return &m_pRecs[idx]; }

	BOOL AllDead(void) { return (BOOL)(!m_pFirstOpp || !m_pFirstOpp->Next()); }

	char *SetCurShot(void);
	char *GetCurShot(void) { return m_szCurShot; }
	void SendInTheClowns(void);

	void AddEngaged(COpp *pOpp);
	int  HitEngaged(int idx, int iDamage);
	int  EngagedCrashStrength(int idx);
	int  EngagedShooter(int idx);			// Is the engaged opp a shooter?
	void ReleaseEngaged(void);
	void DestroyOpp(COpp *pOpp);
	int  OppsInCell(CCell *pCell);
	void OppGenOff(int iGen);
	void AdjustEngaged(int iNum);
	EngageRec *FindEngaged(int idx);
	void ResetEngaged(void);
	void BuildEngagedCars(void);
	char *GetEngagedCars(void) {return m_szEngagedCars;}
	int  GetStrength(int idx);	// Get engaged opponents strength;
	char *GetName(int idx);		// Get engaged opponent name
	
	void DestroyArea(CCell *pPlayerCell, int iDir);
	void DestroyOpponents(CCell *pCell);
	void Resurrect(int nFirst, int nLast);
	void Clone(COpp *pOpp);
	int  CountOpps(void);
	
private:
	COpp *m_pFirstOpp;							// First Opponent in list
	COpp *m_pLastOpp;							// Last opponent in list

	COppGenerator *m_pFirstGen;					// First opponent generator

	// Opponents currently engaged (on video)
	//COpp *m_pEngaged[MaxEngaged];
	EngageRec *m_pEngaged;		// First engaged rec (always at the player's 12 o'clock)
	EngageRec m_EngageRecs[12];

	OpponentRec *m_pRecs;		// Opponent records
	short m_iNumRecs;			// Number of opponent records

	CCell *m_pShotCell;			// The 'predicted' shot cell based on player
								// direction
	char m_szShot[MaxShot];		// Current shot
	OppCarShot m_ShotCars[MaxShotCars];	// Current shot candidates
	int m_iCarShotCnt;			// Current shot candidate count

	CCarShots *m_pCarShots;		// Car shots

	char m_szCurShot[MaxShot];	// Current shot for video
	char m_szNextShot[MaxShot];	// Next potential shot based off of shot cell

	short m_iEngaged;			// Number of engaged opponents
	//COpp *m_Engaged[3];			// Engaged opponents

	BOOL m_bEmptyShot;			// Indicates an empty shot
	BOOL m_bPaused;				// Pause flag for moving opponents

	COpp *m_pBehind;			// Opponent behind player
	char m_szEngagedCars[10];	// Engaged car string 

	char *m_pszCarGates;		// car gate prefix pointer

};


#endif
