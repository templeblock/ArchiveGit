/******************* ( Animation Construction Kit 3D ) ******************/
/*			     Door Routines				     																	*/
/*  CopyRight (c) 1993	   Author: Lary Myers				    						 		*/
/*  CopyRight (c) 1994/5	 John Lundy														     		*/
/* Code enhanced, modified, fixed and adapted to Windows by John Lundy	*/
/************************************************************************/

#include "stdafx.h"
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

/****************************************************************************
** Check all the doors to see what state they are in. If a door's column   **
** offset is non-zero then it is either opening or closing, so add in the  **
** speed of the door and check for fully open and fully closed conditions. **
****************************************************************************/
void CheckDoors(ACKENG *ae)
{
int		i,MapPosn,mPos,mPos1;
int		xPlayer,yPlayer;

xPlayer = ae->ObjList[PLAYER_OBJECT].x;
yPlayer = ae->ObjList[PLAYER_OBJECT].y;

for (i = 0 ; i < MAX_DOORS ; i++)
{
  if (ae->Door[i].ColOffset)
	{
		ae->Door[i].ColOffset += ae->Door[i].Speed;
		mPos = ae->Door[i].mPos;
		mPos1 = ae->Door[i].mPos1;

		/* Door is closing and is visible. Put old codes back to make non-passable */
		if (ae->Door[i].Speed < 0 && ae->Door[i].ColOffset < 65)
		{
    	MapPosn = (yPlayer & 0xFFC0) + (xPlayer >> 6);
    	if ((mPos < mPos1 && MapPosn == mPos) ||
    		(mPos1 < mPos && MapPosn == mPos1))
			{
				//	ae->Door[i].ColOffset -= ae->Door[i].Speed * 2;
				if (ae->Door[i].mCode & DOOR_TYPE_SLIDE)
				{
					if (ae->Door[i].ColOffset < 32)
						ae->Door[i].Speed = -ae->Door[i].Speed;
				}
				else	//	DOOR_TYPE_SPLIT
				{
					if (ae->Door[i].ColOffset < 16)
						ae->Door[i].Speed = -ae->Door[i].Speed;
				}
				continue;
			}

    	if (ae->Door[i].Type == DOOR_XCODE)
			{
				ae->xGrid[mPos]	 = ae->Door[i].mCode;
				ae->xGrid[mPos1] = ae->Door[i].mCode1;
			}
    	else
			{
				ae->yGrid[mPos]	 = ae->Door[i].mCode;
				ae->yGrid[mPos1] = ae->Door[i].mCode1;
			}

			/* Door is close enough to fully closed to get rid of the door from the array */
    	if (ae->Door[i].ColOffset < 3)
			{
				ae->Door[i].ColOffset = 0;
				ae->Door[i].mPos      = -1;
				ae->Door[i].mPos1     = -1;
				ae->Door[i].Flags     = 0;
			}
		}

   	/* Door is fully open, reverse the speed to begin closing */
		if (ae->Door[i].ColOffset > 0xA0)
    {
    	ae->Door[i].Speed = -ae->Door[i].Speed;
    	ae->Door[i].Flags &= ~DOOR_OPENING;
    	ae->Door[i].Flags |= DOOR_CLOSING;
    }
	}
}

}

/****************************************************************************
* Locate a door from its Map coordinate and return the index to the caller.	*
****************************************************************************/
int FindDoor(ACKENG *ae,int MapPosn)
{
int	    index;

for (index = 0 ; index < MAX_DOORS ; index++)
{
    if (MapPosn == ae->Door[index].mPos || MapPosn == ae->Door[index].mPos1)
		return(index);
}

return(-1);
}


/****************************************************************************
* Find an empty slot for a door. If the door already occupies a slot and		*
* it is in a non-closed state then return error.														*
****************************************************************************/
int FindDoorSlot(ACKENG *ae,int MapPosn)
{
int	    index;

index = FindDoor(ae,MapPosn);
if (index >= 0 && ae->Door[index].ColOffset)
    return(-1);

for (index = 0 ; index < MAX_DOORS ; index++)
{
    if (ae->Door[index].mPos == -1)
		return(index);
}

return(-1);
}

/****************************************************************************
* The application performs this call to see if the POV is close enough to		*
* a door to begin opening it. (A good time to make this call would be				*
* when the POV moves). If a door is activated a return code tells the				*
* application what kind of door it was (for purposes of sound, etc.)				*
****************************************************************************/
int AckCheckDoorOpen(ACKENG *ae,int xPlayer,int yPlayer,int PlayerAngle)
{
int	    i,j,DoorCode,Amount=0;

DoorCode = POV_NODOOR;	    /* Default - assumes no doors found */

i = AckCheckHit(ae,xPlayer,yPlayer,PlayerAngle,&Amount,32);

// Check doors along the X walls
if (i == POV_DOORX_REG || i == POV_DOORX)
{
  j = FindDoorSlot(ae,xMapPosn);
  if (j >= 0)
	{
		if (ae->xGrid[xMapPosn] & WALL_SECRET_DOOR)
			DoorCode = POV_XSECRETDOOR;
		else
			DoorCode = POV_XDOOR;

		LastMapPosn = ae->Door[j].mPos = xMapPosn;
		if (iLastX > xPlayer)
    	i = xMapPosn + 1;
		else
    	LastMapPosn = i = xMapPosn - 1;

		if (ae->xGrid[xMapPosn] & DOOR_LOCKED_MASK)
    {
    	ae->Door[j].mPos = -1;
    	LastDoorDetail = ae->xGrid[xMapPosn];
   		return(DoorCode | POV_DOORLOCKED);
    }

		ae->Door[j].mCode = ae->xGrid[xMapPosn];
		ae->Door[j].mCode1 = ae->xGrid[i];
		ae->Door[j].mPos1 = i;
		ae->Door[j].ColOffset = 1;
		ae->Door[j].Speed = ae->DoorSpeed;
		ae->Door[j].Type = DOOR_XCODE;
		ae->Door[j].Flags = DOOR_OPENING;
	}
}

// Check doors along the Y walls
if (i == POV_DOORY_REG || i == POV_DOORY)
{
  j = FindDoorSlot(ae,yMapPosn);   
  if (j >= 0)
	{
		if (ae->yGrid[yMapPosn] & WALL_SECRET_DOOR)
			DoorCode = POV_YSECRETDOOR;
		else
			DoorCode = POV_YDOOR;

		LastMapPosn = ae->Door[j].mPos = yMapPosn;
		if (iLastY > yPlayer)
	    i = yMapPosn + GRID_WIDTH;
		else
			LastMapPosn = i = yMapPosn - GRID_WIDTH;

		if (ae->yGrid[yMapPosn] & DOOR_LOCKED_MASK)
   	{
	    ae->Door[j].mPos = -1;
    	LastDoorDetail = ae->yGrid[yMapPosn];
   		return(DoorCode | POV_DOORLOCKED);
   	}

		ae->Door[j].mCode = ae->yGrid[yMapPosn];
		ae->Door[j].mCode1 = ae->yGrid[i];
		ae->Door[j].mPos1 = i;
		ae->Door[j].ColOffset = 1;
		ae->Door[j].Speed = ae->DoorSpeed;
		ae->Door[j].Type = DOOR_YCODE;
		ae->Door[j].Flags = DOOR_OPENING;
	}
}

return(DoorCode);
}
