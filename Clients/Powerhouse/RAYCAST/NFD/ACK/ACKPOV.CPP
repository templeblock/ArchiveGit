/******************* ( Animation Construction Kit 3D ) ******************/
/*			 Point of View Routines																					*/
/* CopyRight (c) 1993	  Author: Lary Myers															*/
/* CopyRight (c) 1994/5	John Lundy																			*/
/* Code enhanced, modified, fixed and adapted to Windows by John Lundy	*/
/************************************************************************/

#include "stdafx.h"
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"
#include "acksqr.cpp"		//	inline functions

#ifndef	_DEBUG
#include "ackray.cpp"		//	Inline functions
#else
UINT xRay(UINT *xGrid,int x,int y,int angle);
UINT yRay(UINT *yGrid,int x,int y,int angle);
#endif
long	long_divide(long denom,long divisor);

extern int			ViewLowRes;

#define	sgn(a)	(((a) >= 0) ? TRUE : FALSE)

/****************************************************************************
** Normally an internal call made by AckMovePOV() to determine if a hit	   **
** has occurred with an object. The application can make this call if	   **
** desired at other times than moving the POV. If POV_OBJECT is returned   **
** then the app can call AckGetObjectHit() to retrieve the number of the   **
** object hit.															   **
****************************************************************************/
int AckCheckObjPosn(ACKENG *ae,int ObjIndex,int xPlayer,int yPlayer,int PlayerAngle,int *Amount)
{
int	 	i,result;
int		MinDistance,distance,CheckDist;
union
{	struct
	{	int		deltaxLow;
		int		deltaxHigh;
	};
	long	deltax;
};
long 	NewX,NewY;
long	SinValue,CosValue;
OBJECTS	far *oPtr;

result = POV_NOTHING;
MinDistance = MAX_DISTANCE;
CheckDist = 32 + *Amount;
SinValue = SinTable[PlayerAngle];
CosValue = CosTable[PlayerAngle];

for (i = 0; i < ae->MaxObjects; i++)
{
	//	Don't do ourselves
	if (i == ObjIndex) continue;

	oPtr = &ae->ObjList[i];

	//	Is the Object inactive or passable (can walk through)
	if (!oPtr->Active || oPtr->Flags & OF_PASSABLE) continue;
    
	if (oPtr->Flags & OF_ONE_SHOT && oPtr->Enemy == ObjIndex) continue;
	if (ae->ObjList[ObjIndex].Flags & OF_ONE_SHOT && ae->ObjList[ObjIndex].Enemy == i) continue;

	NewX = oPtr->x - xPlayer;
	if (labs(NewX) >= CheckDist) continue;
	NewY = oPtr->y - yPlayer;
	if (labs(NewY) >= CheckDist) continue;

	deltax = (NewX * CosValue) + (NewY * SinValue);
	if (deltaxHigh < 0) continue;

	distance = (int)long_sqrt((NewX * NewX) + (NewY * NewY));

	if (distance >= CheckDist) continue;
	if (distance < MinDistance)
	{
		LastObjectHit = i;
		MinDistance = distance;
		result = POV_OBJECT;
	}
}
if (result != POV_NOTHING)
{
	if (LastObjectHit == PLAYER_OBJECT) result = POV_PLAYER;
	if (!(ae->ObjList[LastObjectHit].Flags & (OF_CAN_PICKUP|OF_FORCE_FIELD)))
	{
		*Amount = *Amount - (CheckDist - MinDistance);
	}
}

return(result);
}

/****************************************************************************
**	This routine is used by the Player and many Objects while shooting.		*
**	If casts an X and a Y ray checks if any actor is hit given the Spread	*
**	(angle) of the weapon. It returns the distance to the actor so the main	*
**	routine can calculate damage based on distance.							*
****************************************************************************/
int AckCheckObjHit(ACKENG *ae,int ObjIndex,int xPlayer,int yPlayer,int PlayerAngle,int Spread,int *Distance)
{
int	    ObjectNumber,ObjectCode;
int	    xBitmap,yBitmap;
int			NewAngle;

long	NewX,NewY;
union
{	struct
	{	int		deltaxLow;
		int		deltaxHigh;
	};
	long	deltax;
};
long	deltay;
long	distance,ObjectDistance;
long	xd,yd;
union
{	struct
	{	int		xDistanceLow;
		int		xDistanceHigh;
	};
	long	xDistance;
};
union
{	struct
	{	int		yDistanceLow;
		int		yDistanceHigh;
	};
	long	yDistance;
};
long	SinValue,CosValue;
OBJECTS	far *oPtr;

SinValue = SinTable[PlayerAngle];
CosValue = CosTable[PlayerAngle];

ObjectDistance = 0x7FFFFFFF;	/* Set to a ridiculous value */
ObjectCode     = POV_NOTHING;	/* Nothing hit */

/* Check to see if an Object is being struck by the ray */
for (ObjectNumber=0 ; ObjectNumber < ae->MaxObjects ; ObjectNumber++)
{
	//	Don't want to look at ourselves	
	if (ObjectNumber == ObjIndex) continue;

	oPtr = &ae->ObjList[ObjectNumber];

	//	Must be an Active Object
	if (!oPtr->Active || oPtr->Flags & (OF_PASSABLE|OF_CAN_PICKUP|OF_STATIC|OF_FORCE_FIELD))
		continue;

	if (oPtr->Flags & OF_ONE_SHOT && oPtr->Enemy == ObjIndex) continue;

	NewX = oPtr->x - xPlayer;
	NewY = oPtr->y - yPlayer;

	deltax = (NewX * CosValue) + (NewY * SinValue);
	if (deltaxHigh < 0) continue;

	distance = (long)long_sqrt((NewX * NewX) + (NewY * NewY));

	if (labs(NewX) > labs(NewY))
		deltay = ArcSinTable[long_divide(labs(NewY) << 8,distance)];
	else
		deltay = ArcCosTable[long_divide(labs(NewX) << 8,distance)];
	if (sgn(NewX) != sgn(NewY)) deltay = -deltay;

	if (NewY >= 0)
	{	if (NewX >= 0)
		{	if (PlayerAngle <= INT_ANGLE_180)
				deltay -= PlayerAngle;
			else
			{
				deltay = INT_ANGLE_360 - deltay - PlayerAngle;
				if (deltay == INT_ANGLE_360) deltay = 0;
			}
		}
		else
			deltay = INT_ANGLE_180 + deltay - PlayerAngle;
	}
	else
	{   if (NewX >= 0)
		{	if (PlayerAngle <= INT_ANGLE_180)
				deltay -= PlayerAngle;
			else
			{
				deltay = INT_ANGLE_360 + deltay - PlayerAngle;
				if (deltay == INT_ANGLE_360) deltay = 0;
			}
		}
		else
			deltay = INT_ANGLE_180 + deltay - PlayerAngle;
	}

	NewAngle = PlayerAngle + (int)deltay;
	if (NewAngle < 0) NewAngle += INT_ANGLE_360;
	else
	if (NewAngle >= INT_ANGLE_360) NewAngle -= INT_ANGLE_360;

	if (ae->ObjList[ObjIndex].mPos != oPtr->mPos)
	{
		xDistance = yDistance = 0x7FFFFFFF;
		if (NewAngle != INT_ANGLE_90 && NewAngle != INT_ANGLE_270)
		{
			xBitmap = xRay(ae->xGrid,xPlayer,yPlayer,NewAngle);
	    if (xBitmap)
			{
				if ((xBitmap & 0xFF) < DOOR_XCODE)
				{
					xd = iLastX - xPlayer;
				}
				else
				{
					if (NewAngle < INT_ANGLE_90 || NewAngle > INT_ANGLE_270)
						xd = (iLastX & 0xFC0) - xPlayer;
					else
						xd = ((iLastX + GRID_WIDTH) & 0xFC0) - xPlayer;
				}
				/* Use the delta X to determine the distance to the wall */
				xDistance = xd * InvCosTable[NewAngle];
			}
		}
		if (NewAngle != 0 && NewAngle != INT_ANGLE_180)
		{
	    yBitmap = yRay(ae->yGrid,xPlayer,yPlayer,NewAngle);
	    if (yBitmap)
			{
				if ((yBitmap & 0xFF) < DOOR_XCODE)
				{
					yd = iLastY - yPlayer;
				}
				else
				{
					if (NewAngle < INT_ANGLE_180)
						yd = (iLastY & 0xFC0) - yPlayer;
					else
						yd = ((iLastY + GRID_HEIGHT) & 0xFC0) - yPlayer;
				}
				/* Use the delta Y to determine distance to the wall */
				yDistance = yd * InvSinTable[NewAngle];
			}
		}
		if (xDistanceHigh < distance || yDistanceHigh < distance) continue;
	}

	//	Object and Player can see each other
	oPtr->Flags |= OF_SEEN_PLAYER;
	
	//	Convert shooting spread to distance relationship	
	Spread = (int)(((long)Spread * (long)DistanceTable[distance]) >> (6 - ViewLowRes));
	if ((deltay < -Spread) || (deltay > Spread)) continue;

	if (distance < ObjectDistance)
	{
		ObjectDistance = distance;
		LastObjectHit = ObjectNumber;
		ObjectCode = POV_OBJECT;
	}
}
if (ObjectCode == POV_OBJECT)
{
	if (LastObjectHit == PLAYER_OBJECT) ObjectCode = POV_PLAYER;
	ae->ObjList[LastObjectHit].Enemy = ObjIndex;
	*Distance = (int)ObjectDistance;
}
return(ObjectCode);
}

/****************************************************************************
****************************************************************************/
int AckObjectAngle(ACKENG *ae,int ObjIndex,int xObject,int yObject)
{
long	NewX,NewY;
long	deltay;
long	distance;

NewX = ae->ObjList[ObjIndex].x - xObject;
NewY = ae->ObjList[ObjIndex].y - yObject;

distance = (long)long_sqrt((NewX * NewX) + (NewY * NewY));

if (labs(NewX) > labs(NewY))
	deltay = ArcSinTable[long_divide(labs(NewY) << 8,distance)];
else
	deltay = ArcCosTable[long_divide(labs(NewX) << 8,distance)];
if (sgn(NewX) != sgn(NewY)) deltay = -deltay;

if (NewY >= 0)
{	if (NewX < 0)
		deltay = INT_ANGLE_180 + deltay;
}
else
{	if (NewX >= 0)
	{
		deltay = INT_ANGLE_360 + deltay;
		if (deltay == INT_ANGLE_360) deltay = 0;
	}
	else
		deltay = INT_ANGLE_180 + deltay;
}
return((int)deltay);
}

/****************************************************************************
**	This routine is used by each Object while viewing and shooting.			*
**	If casts an X and a Y ray checks if the Player is hit given the Spread	*
**	(angle) of the weapon. It returns the distance to the Player so the		*
**	main routine can calculate damage based on distance.					*
****************************************************************************/
int AckCheckObjView(ACKENG *ae,int ObjIndex,int Enemy,int xObject,int yObject,int ObjectAngle,int Spread)
{
int		xBitmap,yBitmap;
long	NewX,NewY;
long	xd,yd;
union
{	struct
	{	int		deltaxLow;
		int		deltaxHigh;
	};
	long	deltax;
};
long	deltay;
long	distance;
union
{	struct
	{	int		xDistanceLow;
		int		xDistanceHigh;
	};
	long	xDistance;
};
union
{	struct
	{	int		yDistanceLow;
		int		yDistanceHigh;
	};
	long	yDistance;
};
long	SinValue,CosValue;

SinValue = SinTable[ObjectAngle];
CosValue = CosTable[ObjectAngle];

do
{
	NewX = ae->ObjList[Enemy].x - xObject;
	NewY = ae->ObjList[Enemy].y - yObject;

	deltax = (NewX * CosValue) + (NewY * SinValue);
	if (deltaxHigh < 0) continue;

	distance = (long)long_sqrt((NewX * NewX) + (NewY * NewY));

	if (labs(NewX) > labs(NewY))
		deltay = ArcSinTable[long_divide(labs(NewY) << 8,distance)];
	else
		deltay = ArcCosTable[long_divide(labs(NewX) << 8,distance)];
	if (sgn(NewX) != sgn(NewY)) deltay = -deltay;

	if (NewY >= 0)
	{	if (NewX >= 0)
		{	if (ObjectAngle <= INT_ANGLE_180)
				deltay -= ObjectAngle;
			else
			{
				deltay = INT_ANGLE_360 - deltay - ObjectAngle;
				if (deltay == INT_ANGLE_360) deltay = 0;
			}
		}
		else
			deltay = INT_ANGLE_180 + deltay - ObjectAngle;
	}
	else
	{	if (NewX >= 0)
		{	if (ObjectAngle <= INT_ANGLE_180)
				deltay -= ObjectAngle;
			else
			{
				deltay = INT_ANGLE_360 + deltay - ObjectAngle;
				if (deltay == INT_ANGLE_360) deltay = 0;
			}
		}
		else
			deltay = INT_ANGLE_180 + deltay - ObjectAngle;
	}

	if ((deltay < -Spread) || (deltay > Spread)) continue;

	ObjectAngle += (int)deltay;
	if (ObjectAngle < 0) ObjectAngle += INT_ANGLE_360;
	else
	if (ObjectAngle >= INT_ANGLE_360) ObjectAngle -= INT_ANGLE_360;

	if (ae->ObjList[Enemy].mPos != ae->ObjList[ObjIndex].mPos)
	{
		xDistance = yDistance = 0x7FFFFFFF;
		if (ObjectAngle != INT_ANGLE_90 && ObjectAngle != INT_ANGLE_270)
		{
	    xBitmap = xRay(ae->xGrid,xObject,yObject,ObjectAngle);
    	if (xBitmap)
			{
				if ((xBitmap & 0xFF) < DOOR_XCODE)
				{
					xd = iLastX - xObject;
				}
				else
				{
					if (ObjectAngle < INT_ANGLE_90 || ObjectAngle > INT_ANGLE_270)
						xd = (iLastX & 0xFC0) - xObject;
					else
						xd = ((iLastX + GRID_WIDTH) & 0xFC0) - xObject;
				}
				/* Use the delta X to determine the distance to the wall */
				xDistance = xd * InvCosTable[ObjectAngle];
			}
		}
		if (ObjectAngle != 0 && ObjectAngle != INT_ANGLE_180)
		{
    	yBitmap = yRay(ae->yGrid,xObject,yObject,ObjectAngle);
    	if (yBitmap)
			{
				if ((yBitmap & 0xFF) < DOOR_XCODE)
				{
					yd = iLastY - yObject;
				}
				else
				{
					if (ObjectAngle < INT_ANGLE_180)
						yd = (iLastY & 0xFC0) - yObject;
					else
						yd = ((iLastY + GRID_HEIGHT) & 0xFC0) - yObject;
				}
				/* Use the delta Y to determine distance to the wall */
				yDistance = yd * InvSinTable[ObjectAngle];
			}
		}
		if (xDistanceHigh < distance || yDistanceHigh < distance) continue;
	}

	return(POV_PLAYER);
}
while(0);
return(POV_NOTHING);
}

/****************************************************************************
** Similiar to the AckMovePOV() above except ignores collision with the	   **
** same object being moved, and also checks for a collision with the	   **
** player. Angle should be the direction to move the object, Amount is the **
** map unit distance the object is to be moved.							   **
****************************************************************************/
int AckMoveObjectPOV(ACKENG *ae,int ObjIndex,int Angle,int Amount)
{
//	To keep the highest accuracy during calculations concerning
//	the next position, we maintain all digits during calculations.
//	However, we still only pass the most significant digits to any
//	subordinate routines.  To do this, we create this union/struct
//	combination; both here and in the OBJECTS structure (ACK3D.H).
union
{	struct
	{	int		xlow;
		int	    x1;
	};
	long	xl1;
};
union
{	struct
	{	int		ylow;
		int		y1;
	};
	long	yl1;
};
int		HitResult,NewResult,NewAngle,NewAmount=Amount;
OBJECTS	far *oPtr;

oPtr = &ae->ObjList[ObjIndex];

xl1 = oPtr->xl + (CosTable[Angle] * Amount);
yl1 = oPtr->yl + (SinTable[Angle] * Amount);

HitResult = AckCheckHit(ae,oPtr->x,oPtr->y,Angle,&NewAmount,ObjList[ObjIndex]->CheckDist);
if (!HitResult)
{
	NewResult = AckCheckObjPosn(ae,ObjIndex,oPtr->x,oPtr->y,Angle,&NewAmount);
  if (NewResult)
  {
  	if (NewAmount < Amount)
  	{
			if (!(oPtr->Flags & OF_ONE_SHOT))
			{
				xl1 = oPtr->xl + (CosTable[Angle] * NewAmount);
				yl1 = oPtr->yl + (SinTable[Angle] * NewAmount);
			}
			HitResult = NewResult;
    }
  	else
  	{
			if (ae->ObjList[LastObjectHit].Flags & (OF_CAN_PICKUP|OF_FORCE_FIELD)) HitResult = POV_OBJECT;
		}
   	oPtr->xl = xl1;
   	oPtr->yl = yl1;
		oPtr->mPos = (y1 & 0xFC0) + (x1 >> 6);
    return(HitResult);
  }
	if (oPtr->Flags & OF_ONE_SHOT)
	{
   	oPtr->xl = xl1;
   	oPtr->yl = yl1;
		oPtr->mPos = (y1 & 0xFC0) + (x1 >> 6);
		return(HitResult);
	}
}
else
if (oPtr->Flags & OF_ONE_SHOT)
{
	oPtr->xl = oPtr->xl + (CosTable[Angle] * NewAmount);
	oPtr->yl = oPtr->yl + (SinTable[Angle] * NewAmount);
	oPtr->mPos = (oPtr->y & 0xFC0) + (oPtr->x >> 6);
	return(HitResult);
}

NewAngle = DeflectTable[Angle];

if (HitResult == POV_XWALL || HitResult == POV_DOORX)
{	//	If we hit an XWALL, modify X based upon how far we can go at Angle
	xl1 = oPtr->xl + (CosTable[Angle] * NewAmount);	//	keep all bits
}
else
if (HitResult == POV_YWALL || HitResult == POV_DOORY)
{	//	If we hit a YWALL, modify Y based upon how far we can go at Angle
	yl1 = oPtr->yl + (SinTable[Angle] * NewAmount);	//	for accuracy
}
Amount = NewAmount = 0;

//	Check if something is beside us; like a wall or object. Ex: if we are
//	"sliding" down a wall, we have to see if we hit another wall or an object
//	in the opposite plain. This keeps us from walking through things.
NewResult = AckCheckHit(ae,x1,y1,NewAngle,&NewAmount,ObjList[ObjIndex]->CheckDist);
if (!NewResult)
{
	//	Check if there is an Object in the location/direction we want to move
	NewResult = AckCheckObjPosn(ae,ObjIndex,x1,y1,NewAngle,&NewAmount);
  if (NewResult)
  {
		if (NewAmount < Amount)
		{
			xl1 += (CosTable[NewAngle] * NewAmount);	//	keep all bits
			yl1 += (SinTable[NewAngle] * NewAmount);	//	for accuracy
			HitResult = NewResult;
    }
  	else
  	{
			if (ae->ObjList[LastObjectHit].Flags & (OF_CAN_PICKUP|OF_FORCE_FIELD)) HitResult = POV_OBJECT;
		}
	}
	oPtr->xl = xl1;
	oPtr->yl = yl1;
	oPtr->mPos = (y1 & 0xFC0) + (x1 >> 6);
  return(HitResult);
}

if (NewAmount < Amount)
{
	if (NewResult == POV_XWALL || NewResult == POV_DOORX)
	{
		xl1 += (CosTable[NewAngle] * NewAmount);	//	keep all bits
	}
	else
	if (NewResult == POV_YWALL || NewResult == POV_DOORY)
	{
		yl1 += (SinTable[NewAngle] * NewAmount);	//	for accuracy
	}
}

oPtr->xl = xl1;
oPtr->yl = yl1;
oPtr->mPos = (y1 & 0xFC0) + (x1 >> 6);
return(HitResult ? HitResult : NewResult);
}

/****************************************************************************
** Internal call used by AckMovePOV() and AckMoveObjectPOV() to determine  **
** if a wall was hit. This routine does NOT check for hits with objects.   **
** ViewAngle is the angle to check against (usually the angle the POV is   **
** facing, but could also be 180 degrees from the facing angle to see if   **
** the POV hits something while backing up).							   **
****************************************************************************/
int AckCheckHit(ACKENG *ae,int xPlayer,int yPlayer,int ViewAngle,int *Amount,int CheckDist)
{
int		BitmapNumber,yBitmap;
int		WallCode;
long	WallDistance,xd,yd,yDistance;
long	DoorDist=0;

/* Initial minimum distance to look for */
CheckDist    += *Amount;
BitmapNumber = 0;				    /* Initialize to no bitmap found */
WallCode     = POV_NOTHING;
WallDistance = 0x7FFFFFFF;	/* Set to a ridiculous value */

/* Don't allow one of these angles, causes either the X or Y ray to not be */
/* cast which gives a false reading about an obstacle.
if (ViewAngle == INT_ANGLE_45 ||
    ViewAngle == INT_ANGLE_135 ||
    ViewAngle == INT_ANGLE_225 ||
    ViewAngle == INT_ANGLE_315)
    ViewAngle++;
*/
/* Don't cast an X ray if no chance of striking a X wall */
if (ViewAngle != INT_ANGLE_90 && ViewAngle != INT_ANGLE_270)
{
	BitmapNumber = xRay(ae->xGrid,xPlayer,yPlayer,ViewAngle);
	if (BitmapNumber)
	{
		if ((BitmapNumber & 0xFF) < DOOR_XCODE)
		{
			xd = iLastX - xPlayer;
			if (BitmapNumber & WALL_SECRET_DOOR)
				WallCode = POV_DOORX;
			else
				WallCode = POV_XWALL;
		}
		else
		{
			if (ViewAngle < INT_ANGLE_90 || ViewAngle > INT_ANGLE_270)
				xd = (iLastX & 0xFC0) - xPlayer;
			else
				xd = ((iLastX + GRID_WIDTH) & 0xFC0) - xPlayer;
			WallCode = POV_DOORX_REG;
		}
		/* Use the delta X to determine the distance to the wall */
		WallDistance = xd * InvCosTable[ViewAngle];
		LastMapPosn = xMapPosn;
	}
}

/* Don't cast a Y ray if impossible to strike a Y wall */
if (ViewAngle != 0 && ViewAngle != INT_ANGLE_180)
{
	yBitmap = yRay(ae->yGrid,xPlayer,yPlayer,ViewAngle);
	if (yBitmap)
	{
		if ((yBitmap & 0xFF) < DOOR_XCODE)
		{
			yd = iLastY - yPlayer;
		}
		else
		{
			if (ViewAngle < INT_ANGLE_180)
				yd = (iLastY & 0xFC0) - yPlayer;
			else
				yd = ((iLastY + GRID_HEIGHT) & 0xFC0) - yPlayer;
		}
		/* Use the delta Y to determine distance to the wall */
		yDistance = yd * InvSinTable[ViewAngle];
		/* If Y wall closer than X wall then use Y wall data */
		if (yDistance < WallDistance)
		{
			WallDistance = yDistance;
			/* Indicate the wall struck was a Y wall */
			BitmapNumber = yBitmap;
			if ((BitmapNumber & 0xFF) < DOOR_XCODE)
			{
				if (BitmapNumber & WALL_SECRET_DOOR)
					WallCode = POV_DOORY;
				else
					WallCode = POV_YWALL;
			}
			else
				WallCode = POV_DOORY_REG;
			LastMapPosn = yMapPosn;
		}
	}
}

/* Since doors appear in the middle of the wall, adjust the minimum distance */
/* to it. This handles walking up close to a door.	*/
if ((BitmapNumber & 0xFF) >= DOOR_XCODE)
{
	DoorDist = 32L;
}

if (WallCode)
{
	/* Adjust the distance based on the center of the screen */
	//    WallDistance = ((WallDistance >> FP_SHIFT) * ViewCosTable[160]) >> FP_SHIFT;
	WallDistance = WallDistance >> FP_SHIFT;

	/* If the wall or object is further than the minimum distance, we can */
	/* continue moving in this direction. */
	if (WallDistance > (CheckDist + DoorDist)) WallCode = POV_NOTHING;
}
if (WallCode)
{
	if (WallDistance < CheckDist)
	{
		if (WallCode == POV_DOORX_REG) WallCode = POV_DOORX;
		else
		if (WallCode == POV_DOORY_REG) WallCode = POV_DOORY;
		
		*Amount = *Amount - (int)(CheckDist - WallDistance);
	}
}

return(WallCode);
}

/****************************************************************************
** This routine can be called by the application to automatically update   **
** any objects that have multiple bitmaps to display. (Note: This is	   **
** different than objects that have multiple sides). Any objects that need **
** a new bitmap displayed will be updated by this routine.				   **
****************************************************************************/
void AckCheckObjectMovement(ACKENG *ae)
{
int	    i,sn,cn,mx,bc;
ANIMATION	*ObjPtr;

for (i = 1; i < ae->MaxObjects; i++)
{
  if (!ae->ObjList[i].Active) continue;
  if (!ae->ObjList[i].Flags & OF_ANIMATE) continue;

	ObjPtr = ObjList[ae->ObjList[i].bmNum];

 	bc = ObjPtr->bmCnt + 1;
	if (bc > ObjPtr->bmDsp[ObjPtr->CurNum])
	{
	  if (ae->ObjList[i].Flags & OF_SEEN_PLAYER)
   	{
   		sn = ObjPtr->MovNum;
   		mx = ObjPtr->MovMax;
   	}
   	else
   	{
   		sn = ObjPtr->IdlNum;
   		mx = ObjPtr->IdlMax;
   	}

    cn = ObjPtr->CurNum + 1;
    if (cn > mx) cn = sn;
		ObjPtr->CurNum = cn;
		ObjPtr->bmCnt = 0;
	}
	else
		ObjPtr->bmCnt = bc;
}
}
