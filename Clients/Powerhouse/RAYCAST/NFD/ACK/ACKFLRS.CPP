/******************* ( Animation Construction Kit 3D ) ******************/
/*			 Draw Floor Slice							     															*/
/* CopyRight (c) 1994/5	 John Lundy								     									*/
/************************************************************************/

#include "stdafx.h"
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

extern int		ViewUpDown, ViewUDCenter;

/****************************************************************************
* Draw a floor slice for the present column based upon data returned				*
* by the ray cast routines in the AckView routine														*
* Viewer angle from center of screen to bottom row is 18.75 degrees or 100	*
* screen units in high resolution mode (50 units in low resolution mode)		*
****************************************************************************/
void	FloorSlice(ACKENG *ae,int xPlayer,int yPlayer,int ViewAngle,int Column)
{
int			i, j;
UINT		offset;
long		xd, yd;
union
{	struct
	{	int		DistanceLow;
		int		DistanceHigh;
	};
	long	Distance;
};

for (i=0 ; i < xTiles ; i++)
{
	xd = xFloors[i].xPos - xPlayer;
	Distance = xd * InvCosTable[ViewAngle];
	if (DistanceHigh >= MAX_DISTANCE)
	{
		xTiles = i;
		break;	//	Everything else is farther away
	}
	if (DistanceHigh < 16) DistanceHigh = 16;
	Distance = DistanceHigh * ViewCosTable[Column];
	offset = BS_Floor[DistanceHigh];
	offset += (ViewUDCenter - ViewUpDown) / DistanceHigh;
	if ((int)offset >= ViewHeight) continue;
	offset = offset * BytesPerRow + Column;
	ScreenBuffer[offset] = 244;
}

for (j=0 ; j < yTiles; j++)
{
	yd = yFloors[j].yPos - yPlayer;
	Distance = yd * InvSinTable[ViewAngle];
	if (DistanceHigh >= MAX_DISTANCE)
	{
		yTiles = j;
		break;	//	Everything else is farther away
	}
	if (DistanceHigh < 16) DistanceHigh = 16;
	Distance = DistanceHigh * ViewCosTable[Column];
	offset = BS_Floor[DistanceHigh];
	offset += (ViewUDCenter - ViewUpDown) / DistanceHigh;
	if ((int)offset >= ViewHeight) continue;
	offset = offset * BytesPerRow + Column;
	ScreenBuffer[offset] = 244;
}
}
