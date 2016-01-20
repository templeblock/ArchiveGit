#define TEST_WORLD

#include <windows.h>
#include "proto.h"
#include "VWorld.h"
#include "Weapon.h"

VideoWorld::VideoWorld() 
{
	nNumPoints = 0;
	nNumWalls = 0;
	pPoints = NULL;
	pWalls = NULL;
	nLength = 0;
	lTestWorldType = 0;
	bEnemyTestOn = FALSE;
}

VideoWorld::~VideoWorld()
{
	if (pPoints)
		FreeUp(pPoints);

	if (pWalls)
		FreeUp(pWalls);
}

BOOL VideoWorld::HitWall(int x, int z)
{
	//TEMPORARY SIMPLE TEST
	if ( z >= nLength )
	{
		//char debug[81];
		//wsprintf(debug, "hit wall z=%d, nLength=%d\n", z, nLength);
		//OutputDebugString(debug);
		return TRUE;
	}

	return FALSE;
}

void VideoWorld::SetHexagonWorld(int _nLength)
{
	double Tan30 = 0.5773502691896;		// Tangent 30 degrees
	double Cos60 = 0.5;					// Cosine 60 degrees

	nLength = _nLength;

	if (pPoints)
		FreeUp(pPoints);

	// Allocate world points for hexagon
	nNumPoints = 5;
	pPoints = (WPOINT *)Alloc( sizeof(WPOINT) * nNumPoints );

	// Some right triangle math
	int nHexSideLength =  (int)((Tan30 * nLength/2.0) * 2.0);
	int nCornerToCorner = nHexSideLength + (int)((Cos60 * nHexSideLength) * 2.0);

	// Starting point is the first one on our left as we enter the world
	pPoints[0].x = -1 * (nHexSideLength / 2);
	pPoints[0].z = 0;

	pPoints[1].x = -1 * (nCornerToCorner / 2);
	pPoints[1].z = nLength / 2;

	pPoints[2].x = pPoints[0].x;
	pPoints[2].z = nLength;

	pPoints[3].x = pPoints[2].x * -1;
	pPoints[3].z = pPoints[2].z;

	pPoints[4].x = pPoints[1].x * -1;
	pPoints[4].z = pPoints[1].z;
	
	pPoints[5].x = pPoints[0].x * -1;
	pPoints[5].z = pPoints[0].z;
	
	// Now create walls (don't need the one behind us)
	if (pWalls)
		FreeUp(pWalls);

	nNumWalls = 5;
	pWalls = (WALL *)Alloc( sizeof(WALL) * nNumWalls);

	pWalls[0].p1 = &pPoints[0];
	pWalls[0].p2 = &pPoints[1];

	pWalls[1].p1 = &pPoints[1];
	pWalls[1].p2 = &pPoints[2];

	pWalls[2].p1 = &pPoints[2];
	pWalls[2].p2 = &pPoints[3];

	pWalls[3].p1 = &pPoints[3];
	pWalls[3].p2 = &pPoints[4];

	pWalls[4].p1 = &pPoints[4];
	pWalls[4].p2 = &pPoints[5];
}

void VideoWorld::SetWorld(long lWorldType, CWeapon *pWeapon)
{
	BOOL bEnabled = TRUE;		// Start out assuming weapon is enabled

	#ifdef TEST_WORLD
		// Use the test value if set
		if (lTestWorldType)
		{
			lWorldType = lTestWorldType;
		}
	#endif

	int iIndex = lWorldType - 1;

	switch(lWorldType)
	{

    // Car heading for yellow portal, goes to middle and gets hit by car coming from 10 o'clock
	case 1:
		nLength = WParams[iIndex].nLength;				// Length from start to wall
		lCarStart = WParams[iIndex].lStartMotion;		// Relative car moving start frame
		lCarStop = WParams[iIndex].lStopMotion;			// Relative car stop frame

		// Figure the distance the car steps (moves) for one frame
		dCarStep = (double)(nLength/2) / (double)(lCarStop - lCarStart);

		// Set 3D world params
		pWeapon->SetWorld(nLength, 200, 3);

		// Set the enemy parameters for this shot
		if (!bEnemyTestOn)
			pWeapon->SetEnemy(0, 7, -214, 1, 130, 0, 170, 40, 11, 36);
		break;

	// Car goes straight through yellow portal
	case 2:
		nLength = WParams[iIndex].nLength;				// Length from start to wall
		lCarStart = WParams[iIndex].lStartMotion;		// Relative car moving start frame
		lCarStop = WParams[iIndex].lStopMotion;			// Relative car stop frame

		// Figure the distance the car steps (moves) for one frame
		dCarStep = (double)(nLength) / (double)(lCarStop - lCarStart);

		// Set 3D world params

		pWeapon->SetWorld(nLength, 200, 3);
		break;

	// Car heading straight for green portal, goes to middle and gets hit by car coming from
	// 2 o'clock
	case 3:
		nLength = WParams[iIndex].nLength;				// Length from start to wall
		lCarStart = WParams[iIndex].lStartMotion;		// Relative car moving start frame
		lCarStop = WParams[iIndex].lStopMotion;			// Relative car stop frame

		// Figure the distance the car steps (moves) for one frame
		dCarStep = (double)(nLength/2) / (double)(lCarStop - lCarStart);

		// Set 3D world params
		pWeapon->SetWorld(nLength, 200, 3);
		
		// Set the enemy parameters for this shot
		if (!bEnemyTestOn)
			pWeapon->SetEnemy(0, 7, 260, 3, 160, -20, 150, 40, 11, 50);
		break;

	// Car heading straight for green portal, goes to middle and gets hit by car coming from 12 o'clock
	case 4:
		nLength = WParams[iIndex].nLength;				// Length from start to wall
		lCarStart = WParams[iIndex].lStartMotion;		// Relative car moving start frame
		lCarStop = WParams[iIndex].lStopMotion;			// Relative car stop frame


		// Figure the distance the car steps (moves) for one frame
		dCarStep = (double)(nLength/2) / (double)(lCarStop - lCarStart);

		// Set 3D world params
		pWeapon->SetWorld(nLength, 200, 8);
		
		// Set the enemy parameters for this shot
		if (!bEnemyTestOn)
			pWeapon->SetEnemy(0, 4, 0, 5, 250, 0, 135, 70, 20, 36);
		break;

	case 5:
		nLength = WParams[iIndex].nLength;				// Length from start to wall
		lCarStart = WParams[iIndex].lStartMotion;		// Relative car moving start frame
		lCarStop = WParams[iIndex].lStopMotion;			// Relative car stop frame

		pWeapon->SetWorld(nLength, 200, 8);

		if (!bEnemyTestOn)
			pWeapon->SetEnemy(0, 1, -38, 3, 200, -20, 150, 40, 11, 40);
		break;

	case 6:
		nLength = WParams[iIndex].nLength;				// Length from start to wall
		lCarStart = WParams[iIndex].lStartMotion;		// Relative car moving start frame
		lCarStop = WParams[iIndex].lStopMotion;			// Relative car stop frame

		pWeapon->SetWorld(nLength, 200, 8);

		if (!bEnemyTestOn)
			pWeapon->SetEnemy(0, 1, 30, 4, 200, 30, 150, 40, 11, 40);
		break;

	case 7:
		nLength = WParams[iIndex].nLength;				// Length from start to wall
		lCarStart = WParams[iIndex].lStartMotion;		// Relative car moving start frame
		lCarStop = WParams[iIndex].lStopMotion;			// Relative car stop frame

		pWeapon->SetWorld(nLength, 200, 8);

		if (!bEnemyTestOn)
			pWeapon->SetEnemy(0, 7, 300, 1, 320, -140, 200, 80, 15, 50);
		break;

	case 8:
		nLength = WParams[iIndex].nLength;				// Length from start to wall
		lCarStart = WParams[iIndex].lStartMotion;		// Relative car moving start frame
		lCarStop = WParams[iIndex].lStopMotion;			// Relative car stop frame

		pWeapon->SetWorld(nLength, 200, 8);

		if (!bEnemyTestOn)
			pWeapon->SetEnemy(0, 4, -120, 3, 200, -15, 170, 40, 11, 40);
		break;

	case 9:
		nLength = WParams[iIndex].nLength;				// Length from start to wall
		lCarStart = WParams[iIndex].lStartMotion;		// Relative car moving start frame
		lCarStop = WParams[iIndex].lStopMotion;			// Relative car stop frame

		pWeapon->SetWorld(nLength, 200, 8);

		if (!bEnemyTestOn)
			pWeapon->SetEnemy(0, 7, -180, 0, 180, 20, 140, 60, 15, 40);
		break;

	default:
		nLength = 300;		// Length from start to wall
		lCarStart = 0;		// Relative car moving start frame
		lCarStop = 48;		// Relative car stop frame

		// Figure the distance the car steps (moves) for one frame
		dCarStep = (double)(nLength) / (double)(lCarStop - lCarStart);
	
		// We will disable the weapon if the type of world is unrecognized
		bEnabled = FALSE;
	}

	pWeapon->Enable(bEnabled);
}

long VideoWorld::GetJumpFrameOffset(long lFromShotWorldType, long lFromStartFrame,
									long lFromCurFrame, long lToShotWorldType)
{
	int nCurPercent;	// Current percentage distance into the from world
	WORLDPARAMS *wp1;
	WORLDPARAMS *wp2;
	int nNormStopPercent;
	long lNormStopFrame;

	// Adjust shot types to base 0
	--lFromShotWorldType;
	--lToShotWorldType;

	// If no world of the from or to type then just return offset of 0
	if (lFromShotWorldType >= MaxWorldTypes || lToShotWorldType >= MaxWorldTypes)
		return -1;
	
	wp1 = &WParams[lFromShotWorldType];
	wp2 = &WParams[lToShotWorldType];

	long lNormCurFrame = lFromCurFrame - lFromStartFrame;

	// If the current offset is before the start motion then percent is start percent
	if (lNormCurFrame <= wp1->lStartMotion)
	{
		nCurPercent = wp1->nStartPercent;
	}
	// If the current offset is after the stop motion then percent is the stop percent
	else if (lNormCurFrame >= wp1->lStopMotion)
	{
		nCurPercent = wp1->nStopPercent;
	}
	// else figure the current percent in the world
	else 
	{
		nNormStopPercent = wp1->nStopPercent - wp1->nStartPercent;
		lNormStopFrame = wp1->lStopMotion - wp1->lStartMotion;
		nCurPercent = (lNormCurFrame * nNormStopPercent / lNormStopFrame) + wp1->nStartPercent;
	}

	// Now figure the frame offset to jump to
	lNormStopFrame = wp2->lStopMotion - wp2->lStartMotion;
	nNormStopPercent = wp2->nStopPercent - wp2->nStartPercent;
	long lToOffset = nCurPercent * lNormStopFrame / nNormStopPercent;

	return lToOffset;

}