#ifndef _DATA3D_H
#define _DATA3D_H

const int nMaxEnemies = 3;				// Maximum # of enemies
const int nMaxWalls = 5;				// Maximum # of walls
const int nMaxFrame = 500;				// Maximum # of frames per shot

typedef struct {
	int x;					// Current x coord
	int y;					// Current y coord
	int z;					// Current z coord
	int w;					// Width of enemy (x axis)
	int h;					// Height (y axis)
	int d;					// Depth (z axis)
	int nShotCount;			// Number of shots left to shoot
	double dDistance;
} ENEMY;

typedef struct {			
	int x;					// Current x coord
	int y;					// Current y coord
	int z;					// Current z coord
	int w;					// Width of enemy (x axis)
	int h;					// Height (y axis)
	int d;					// Depth (z axis)
} SENEMY;					// Short enemy (saved in AVI file)

struct WPOINT
{
	int x;
	int z;
};

struct WALL
{
	WPOINT p1;
	WPOINT p2;
	double L2;		// Length squared
};

struct SWALL		// Short wall (saved in AVI file)
{
	WPOINT p1;
	WPOINT p2;
};

#define FRTYPE_TWEEN 0
#define FRTYPE_PLAYERZKEY	0x0001
#define FRTYPE_WALLKEY		0x0002
#define FRTYPE_ENEMYKEY		0x0004

typedef struct				
{
	WORD fType;					// Tween frame or key frame?
	int  nCommand;
	int  nPlayerZ;
	int  nNumWalls;
	int  yGround;
	WALL Walls[nMaxWalls];
	int  nNumEnemy;
	ENEMY Enemy[nMaxEnemies];
} FRAMEINFO;					// Info for each frame


#endif