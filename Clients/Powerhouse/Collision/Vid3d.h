#ifndef _VID3D_H
#define _VID3D_H

#include <windows.h>
#include "proto.h"

//#include "vtable.h"

const int MaxEnemies = 3;

typedef struct {
	int x;					// Current x coord
	int y;					// Current y coord
	int z;					// Current z coord
	int nWidth;				// Width of enemy (x axis)
	int nHeight;			// Height (y axis)
	int nDepth;				// Depth (z axis)
} ENEMYBOX;

typedef struct {
	int nLength;		// Length from one end of the world to another 
	int nWidth;			// Length from one end of the world to another 
	int yGround;		// Where the ground appears to be
	int nNumEnemy;		// Number of enemies in current frame
	ENEMYBOX *pEnemy;	// Enemy box array for this frame
} FRAMEINFO3D;

class Vid3dFrame
{

// Public methods
public:
	Vid3dFrame(LPVIDEO lpVideo);
	~Vid3dFrame();

	BOOL Load(char *szFile);
	BOOL Save(char *szFile);

	BOOL SetFrame(long lFrame);	// Set 3d info for this frame

	inline int WorldLength();	// Get world length
	inline int WorldWidth();	// Get world width
	inline int WorldGround();	// Get the ground y value

	inline int NumEnemies();	// Get the number of enemies
	inline int EnemyX(int e);	// Get enemy x coord for enemy e
	inline int EnemyY(int e);	// Get enemy y coord for enemy e
	inline int EnemyZ(int e);	// Get enemy z coord for enemy e
	inline int EnemyW(int e);	// Get enemy width for enemy e
	inline int EnemyH(int e);	// Get enemy height for enemy e
	inline int EnemyD(int e);	// Get enemy depth for enemy 3

// Private variables
private:
	int nLength;			// Length from one end of the world to another 
	int nWidth;				// Width of world(left to right)
	int yGround;			// Where the ground appears to be

	int nNumEnemy;			// Number of enemies in current frame

	ENEMYBOX *pEnemy;		// Enemy box array for this frame

	LPVIDEO lpVid;			// Video pointer
	
	long lNumFrames;		// Number of frames;
	FRAMEINFO3D *pFrames;	// 3d frame info array
};

inline int Vid3dFrame::WorldLength() // Get world length
{
	return nLength;
}

inline int Vid3dFrame::WorldWidth()	// Get world width
{
	return nWidth;
}

inline int Vid3dFrame::WorldGround() // Get the ground y value
{
	return yGround;
}

inline int Vid3dFrame::NumEnemies()	// Get the number of enemies
{
	return nNumEnemy;
}

inline int Vid3dFrame::EnemyX(int e) // Get enemy x coord for enemy e
{
	if ( e >= nNumEnemy )
		return 0;

	return pEnemy[e].x;
}

inline int Vid3dFrame::EnemyY(int e) // Get enemy y coord for enemy e
{
	if ( e >= nNumEnemy )
		return 0;

	return pEnemy[e].y;
}

inline int Vid3dFrame::EnemyZ(int e) // Get enemy z coord for enemy e
{
	if ( e >= nNumEnemy )
		return 0;

	return pEnemy[e].z;
}

inline int Vid3dFrame::EnemyW(int e) // Get enemy width for enemy e
{
	if ( e >= nNumEnemy )
		return 0;

	return pEnemy[e].nWidth;
}

inline int Vid3dFrame::EnemyH(int e) // Get enemy height for enemy e
{
	if ( e >= nNumEnemy )
		return 0;

	return pEnemy[e].nHeight;
}

inline int Vid3dFrame::EnemyD(int e)	// Get enemy depth for enemy 3
{
	if ( e>= nNumEnemy )
		return 0;

	return pEnemy[e].nDepth;
}

#endif