/******************* ( Animation Construction Kit 3D ) ******************/
/*			     Global Data				     																		*/
/* CopyRight (c) 1993	   Author: Lary Myers				     									*/
/* CopyRight (c) 1994/5  John Lundy															     		*/
/* Code enhanced, modified, fixed and adapted to Windows by John Lundy	*/
/************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include "ack3d.h"
#include "ackeng.h"

/*****************************************************************************
    The four variables below are used by ACKRAY and ACKVIEW and are somewhat
misleading. They are held over from older versions of the engine where the
accuracy of the ray-casting routine was still being determined. What happens
is that LastX1 was a long and iLastX use to be an integer. It was later
determined that they both needed to be longs to give better accuracy. The xRay
routine use to set LastY1 to the Y value of an intersection and iLastX to the
X value of the intersection with a wall. The yRay routine would set LastX1 and
iLastY to the wall intersection.
******************************************************************************/
	int		    	LastX1;
	int		    	LastY1;
	int		    	iLastX;
	int		    	iLastY;

	//	Viewport size with default values
	int					ViewWidth=DEF_VIEW_WIDTH;				//	Width
	int					ViewHeight=DEF_VIEW_HEIGHT;			//	Height
	int					ViewX=DEF_VIEW_X;								//	First col of viewport
	int					ViewX1=DEF_VIEW_X1;							//	Last col of viewport
	int					ViewXS=DEF_VIEW_XS;							//	Width, zero based
	int					ViewY=DEF_VIEW_Y;								//	First row of viewport
	int					ViewY1=DEF_VIEW_Y1;							//	Last row of viewport
	int					ViewYS=DEF_VIEW_YS;							//	Height, zero based
	int					BytesPerRow=DEF_BYTES_PER_ROW;
	int					DWordsPerRow=DEF_DWORDS_PER_ROW;

	int	    		MaxDistance;    /* Max dist achieved on last ray-cast */

	int	    		ErrorCode;	    /* Global error set by various routines */

	int	    		xMapPosn;	    /* Map posn of intersect by xRay routine */
	int	    		yMapPosn;	    /* Map posn of intersect by yRay routine */

	HGLOBAL			hGrid;
	HGLOBAL			hObjGrid;
	
	UINT  far   *Grid;	    /* Contains the wall map array */
	UINT  far   *ObjGrid;	/* Holds object array when map read in */

	SPECIALCODE SpecialCodes[MAX_UPDOWN+1]; /* Special map codes */

	int	    		TotalSpecial;

	int	    		DistanceTable[MAX_DISTANCE+1];  /* Used in ray calc's */
	
	HGLOBAL			hAdjustTable;
	long  far   *AdjustTable;		/* Used in object calc's */

	WALLARRAY   Walls[DEF_VIEW_WIDTH];  	/* Walls in current POV */

	FLOORARRAY	xFloors[GRID_WIDTH];	// X Floors in current Column
	FLOORARRAY	yFloors[GRID_HEIGHT];	// Y Floors in current Column
	FLOORARRAY	Floors[GRID_WIDTH+GRID_HEIGHT];		// Floors in current Column
	int					xTiles;
	int					yTiles;
	int					Tiles;

	int	    		TotalSecret;
	int	    		ViewColumn;

	HGLOBAL			hSinTable;
	HGLOBAL			hCosTable;

	long far    *SinTable;
	long far    *CosTable;

	HGLOBAL			hLongTanTable;
	HGLOBAL			hLongInvTanTable;
	HGLOBAL			hInvCosTable;
	HGLOBAL			hInvSinTable;
	HGLOBAL			hArcCosTable;
	HGLOBAL			hArcSinTable;
	HGLOBAL			hViewCosTable;

	long far    *LongTanTable;
	long far    *LongInvTanTable;
	long far    *InvCosTable;
	long far    *InvSinTable;
	int  far    *ArcCosTable;
	int  far    *ArcSinTable;
	long far    *ViewCosTable;
	
	HGLOBAL			hxNextTable;
	HGLOBAL			hyNextTable;

	long far    *xNextTable;    /* Pre-calc'd amount of next X posn */
	long far    *yNextTable;    /* Pre-calc'd amount of next Y posn */

	HGLOBAL			hDeflectTable;
	int far			*DeflectTable;

	UCHAR far		PalTable[PAL_SIZE]; /* 16 solid colors, 32 zones */

	int	    		LastMapPosn;    /* Set for the calling application */
	int	    		LastObjectHit;  /* Set for the calling application */
	int					LastWallHit;
	int					LastObjectDistance;
	int					LastDoorDetail;
	int	    		TotalObjects;

	int					GoreObject=-1;

/* The following object variables are used when objects are found in the	*/
/* current POV. They are built for later display of the objects.			*/
	ORDERARRAY	ObjectOrder[MAX_OBJECTS];

//	Magic Bitmap-to-Screen Paint arrays
	HGLOBAL			hBmScrn;
	MAGICARRAY	pBmScrn[MAX_DISTANCE];

//	Floor and Ceiling builder arrays
	int far			BS_Floor[MAX_DISTANCE];
	int far			BS_Ceiling[MAX_DISTANCE];

//	Other miscellaneous data structures
	WEAPON far	Weapons[MAX_WEAPONS]=
			//	 Name							MaxAmmo	Range	Spread				Rounds	Used	Damage	Delay	Recoil
				{"Fists",						-1,			64,		INT_ANGLE_20,	1,			-2,		750,		3,		6,
				 "Pistol",					250,		500,	INT_ANGLE_10,	1,			1,		500,		3,		4,
				 "Shotgun",					50,			500,	INT_ANGLE_15,	6,			1,		250,		6,		12,	//	Damage per round (5 Pellets)
				 "Machine Gun",			250,		1000,	INT_ANGLE_5,	5,			5,		500,		4,		8,	//	Damage per round (5 Bullets)
				 "Rocket Launcher",	50,			1000,	INT_ANGLE_5,	1,			1,		5000,		6,		24,
				 ""};
				 //		MaxAmmo					Maximum ammunition that can be carried for waepon
				 //		Range						Effective distance of the projectile
				 //		Spread					How wide the spread of the weapon's projectile effective damage
				 //		Rounds					Projectiles propelled when the weapon is fired
				 //		Used						Amount of ammo used each time the trigger is fired
				 //		Damage					Amount of damage performed by each projectile (varies with distance to target)
				 //		Delay						Delay forced between each firing of the weapon
				 //		Recoil					Kick of the weapon imparted to the firing Object
	int					MaxWeaponsInUse=5;

	HGLOBAL far	hbMaps[MAX_WALLBMPS];
  UCHAR far*	far bMaps[MAX_WALLBMPS]; /* Pointers to wall bitmaps */
  HGLOBAL far	hoMaps[MAX_OBJBMPS];
  UCHAR far*	far oMaps[MAX_OBJBMPS];  /* Pointers to object bitmaps */

  UCHAR far		*ScreenBuffer;	    /* Buffer for screen */
  HGLOBAL			hBkgdBuffer;
  UCHAR far		*BkgdBuffer;	    /* Buffer for ceiling/floor */

	ANIMATION far		*ObjList[MAX_OBJBMPS];
