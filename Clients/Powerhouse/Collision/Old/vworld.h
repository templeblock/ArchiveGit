#ifndef _VideoWorld_h
#define _VideoWorld_h

// Word types are characterized by the following criteria:
// Where the shot begins - 1 to 12 on the clock
// Your direction, or where you are headed in the shot - 1 to 12 on the clock
// Where you end up - 1 to 12 on the clock, or a description (> 12)
// Where the opponent starts
// Where the oppenent ends up

#define MASK_SHOT_BEGIN		0xf0000000
#define MASK_DIRECTION		0x0f000000
#define MASK_SHOT_END		0x00f00000

//#define MASK_

// TEMPORARY
#define WORLD_1		1

class CWeapon;

// World types

struct WPOINT
{
	int x;
	int z;
};

struct WALL
{
	WPOINT *p1;
	WPOINT *p2;
};

struct WORLDPARAMS
{
	int  nLength;			// Length of world
	long lStartMotion;		// Frame in the shot where the motion starts (0 based offset)
	int  nStartPercent;		// Percentage into the world where the motion starts
	long lStopMotion;		// Frame in the shot where the motion stops (0 based offset)
	int  nStopPercent;		// Percentage into the world where the motion stops
};

static const int MaxWorldTypes = 9;
static 	WORLDPARAMS WParams[MaxWorldTypes] =
{
	{320, 0, 0, 31, 50},		// 1
	{320, 0, 0, 43, 100},		// 2
	{320, 0, 0, 38, 50},		// 3
	{320, 0, 0, 24, 50},		// 4
	{320, 0, 0, 62, 100},		// 5
	{320, 0, 0, 59, 100},		// 6
	{320, 0, 0, 43, 60},		// 7
	{320, 0, 0, 24, 60},		// 8
	{320, 0, 0, 27, 60}			// 9
};

class VideoWorld
{

public:
	VideoWorld();
	~VideoWorld();
	void SetHexagonWorld(int nLength);

	BOOL HitWall(int x, int z);

	int GetLength();
	void SetLength(int nLength);

	void SetWorld(long lWorldType, CWeapon *pWeapon);

	long CarStart(void);
	long CarStop(void);
	double CarStep(void);

	long GetJumpFrameOffset(long lFromShotWorldType, long lFromStartFrame,
							long lFromCurFrame, long lToShotWorldType);

	void SetTestWorld(long lWorldType);
	void SetEnemyTest(BOOL bOn);

protected:

	int nNumPoints;
	WPOINT *pPoints;

	// The walls are defined clock-wise from the left of our starting position.  We are always inside.
	int nNumWalls;			// Number of walls
	WALL *pWalls;

	long   lCarStart;	// Start frame of the world (when the car actually starts moving)
	long   lCarStop;	// End frame of the world (when the car stops moving)
	double dCarStep;		// Number of positions car moves per frame

	long nLength;		// Length from one end of the world to another 
	long nWidth;	    // Width (left to right)
	long yGround;		// Where the ground appears to be

	long lTestWorldType;
	BOOL bEnemyTestOn;
};

inline
int VideoWorld::GetLength()
{
	return nLength;
}

inline
void VideoWorld::SetLength(int nWorldLength)
{
	nLength = nWorldLength;
}

inline
long VideoWorld::CarStart(void)
{
	return lCarStart;
}

inline 
long VideoWorld::CarStop(void)
{
	return lCarStop;
}

inline 
double VideoWorld::CarStep(void)
{
	return dCarStep;
}

inline
void VideoWorld::SetTestWorld(long lWorldType)
{
	lTestWorldType = lWorldType;
}

inline
void VideoWorld::SetEnemyTest(BOOL bOn)
{
	bEnemyTestOn = bOn;
}

#endif