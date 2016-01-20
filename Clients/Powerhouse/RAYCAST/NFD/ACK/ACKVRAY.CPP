/******************* ( Animation Construction Kit 3D ) ******************/
/*			  Ray Casting Routines				     															*/
/* CopyRight (c) 1993	   Author: Lary Myers				     									*/
/* CopyRight (c) 1994/5	 John Lundy																			*/
/* Code enhanced, modified, fixed and adapted to Windows by John Lundy	*/
/************************************************************************/


/****************************************************************************
** This routine cast rays along the X walls of the map to determine when		*
** a wall is struck. A zero returned indicates no wall was found, else the	*
** bitmap number of the wall is returned.				   													*
** If a wall is found the variables LastY1 and iLastX are set to the map		*
** coordinates of the wall and xMapPosn is the actual map coordinate.				*
****************************************************************************/
#if defined(_DEBUG) || defined(_PROFILER)
#ifdef	_PROFILER
	#pragma message("...Profiler code is included...")
#endif
UINT xVRay(UINT *xGrid,int x,int y,int angle)
#else
inline UINT xVRay(UINT *xGrid,int x,int y,int angle)
#endif
{
union
{	struct
	{	UCHAR	ColorLow;
		UCHAR	ColorHigh;
	};
	UINT	Color;
};
int		MapPosn;
int		xPos,xNext;
union
{   struct
	{	int		yPosLow;
  	int		yPosHigh;
  };
  long	yPos;
};
long	yNext;

if (angle == INT_ANGLE_45)
	angle = INT_ANGLE_45;

xPos = x & 0xFC0;							/* Get upper left corner of square */

if (angle > INT_ANGLE_270 || angle < INT_ANGLE_90)
{
	xPos  += GRID_SIZE;					/* Looking to the right */
	xNext = GRID_SIZE;					/* Positive direction	*/
}
else
{
	xNext = -GRID_SIZE;					/* Looking to the left */
}
yNext = yNextTable[angle];		/* PreCalc'd value of 64 * Tan(angle) */

/* Calculate the Y coordinate for the current square */
yPos = ((long)(xPos - x) * LongTanTable[angle]);
yPosHigh += y;

while (yPos >= 0 && yPos <= GRID_YMAXLONG)
{
	MapPosn = ((yPosHigh & 0xFC0) + (xPos >> 6));
	//	Generate a list of all X tiles we cross
	xFloors[xTiles].Number = xPos > x ? MapPosn : MapPosn - 1;
	xFloors[xTiles].xPos = xPos;
	xFloors[xTiles++].yPos = yPosHigh;

	/* Check to see if a wall is being struck by the ray */
	if ((Color = xGrid[MapPosn]) != 0)
	{
		xMapPosn = MapPosn;	    		/* Hold onto the map location */
		if (ColorLow >= DOOR_XCODE)	/* Is this a door? */
		{
			xPos += (xNext >> 1);			/* Adjust the X,Y values so */
			yPos += (yNext >> 1);			/* the door is halfway in sq.*/
			xFloors[xTiles].Number = xPos > x ? MapPosn : MapPosn - 1;
			xFloors[xTiles].xPos = xPos;
			xFloors[xTiles++].yPos = yPosHigh;
		}
		iLastX = xPos;							/* Last location checked */
		LastY1 = yPosHigh;					/* with something in it */
		return(Color);
	}
	xPos += xNext;	/* Next X coordinate (fixed at 64 or -64) */
	yPos += yNext;	/* Next calculated Y coord for a delta of X */
}

return(0);		/* Return that no wall was found */
}

/****************************************************************************
** This routine cast rays along the Y walls of the map to determine when    *
** a wall is struck. A zero returned indicates no wall was found, else the  *
** bitmap number of the wall is returned.				   				    							*
** If a wall is found the variables LastX1 and iLastY are set to the map    *
** coordinates of the wall and xMapPosn is the actual map coordinate.	    	*
****************************************************************************/
#if defined(_DEBUG) || defined(_PROFILER)
#ifdef	_PROFILER
	#pragma message("...Profiler code is included...")
#endif
UINT yVRay(UINT *yGrid,int x,int y,int angle)
#else
inline UINT yVRay(UINT *yGrid,int x,int y,int angle)
#endif
{
union
{	struct
	{	UCHAR	ColorLow;
		UCHAR	ColorHigh;
	};
	UINT	Color;
};
int		MapPosn;
int		yPos,yNext;
union
{   struct
	{	int		xPosLow;
  	int		xPosHigh;
  };
  long	xPos;
};
long	xNext;

if (angle == INT_ANGLE_45)
	angle = INT_ANGLE_45;

yPos = y & 0xFC0;							/* Same as div 64 then mul 64	*/

if (angle < INT_ANGLE_180)
{
	yPos  += GRID_SIZE;					/* Looking down	*/
	yNext = GRID_SIZE;					/* Positive direction */
}
else
{
	yNext = -GRID_SIZE;					/* Looking up	*/
}
xNext = xNextTable[angle];		/* Pre-calc'd value of 64 / tan(angle) */

/* Calculate the X coordinate for the current square */
xPos = ((long)(yPos - y) * LongInvTanTable[angle]);
xPosHigh += x;

while (xPos >= 0 && xPos <= GRID_XMAXLONG)
{
	MapPosn = ((yPos & 0xFC0) + (xPosHigh >> 6));
	//	Generate a list of all Y tiles we cross
	yFloors[yTiles].Number = yPos > y ? MapPosn : MapPosn - GRID_SIZE;
	yFloors[yTiles].xPos = xPosHigh;
	yFloors[yTiles++].yPos = yPos;

	/** Check for a wall being struck **/
	if ((Color = yGrid[MapPosn]) != 0)
	{
		yMapPosn = MapPosn;					/* Hold onto map position */
		if (ColorLow >= DOOR_YCODE)	/* Is this a door? */
		{
			yPos += (yNext >> 1);			// Adjust coordinates so door is
			xPos += (xNext >> 1);			// Halfway into wall
			yFloors[yTiles].Number = yPos > y ? MapPosn : MapPosn - GRID_SIZE;
			yFloors[yTiles].xPos = xPosHigh;
			yFloors[yTiles++].yPos = yPos;
		}
		iLastY = yPos;
		LastX1 = xPosHigh;
		return(Color);
	}
	yPos += yNext;		/* Next fixed value of 64 or -64 */
	xPos += xNext;		/* Next calculated X value for delta Y */
}

return(0);		/* Return here if no Y wall is found */
}
