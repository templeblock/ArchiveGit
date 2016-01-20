//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

static BOOL ConstrainingX, ConstrainingY;

/************************************************************************/
BOOL ConstrainXY( LPINT lpX, LPINT lpY, BOOL fDown, BOOL fButtonDown,
                     BOOL fConstrainMode )
/************************************************************************/
{
BOOL fMove;
int dx, dy, x, y;
static BOOL ConstrainLooking;
static POINT Constrain, LastPos;

x = *lpX;
y = *lpY;
if (fButtonDown)
	{
	ConstrainingX = ConstrainingY = FALSE;
	ConstrainLooking = fConstrainMode;
	Constrain.x = x;
	Constrain.y = y;
	}
else
if (!fDown)
	{
	ConstrainLooking = FALSE;
	ConstrainingX = ConstrainingY = FALSE;
	}
else
if (ConstrainLooking)
	{
	dx = abs(Constrain.x - x);
	dy = abs(Constrain.y - y);
	if (dx > 1 || dy > 1)
		{
		ConstrainLooking = FALSE;
		if (dx > dy)
			ConstrainingY = TRUE;
		else
			ConstrainingX = TRUE;
		}
	else
		{
		x = Constrain.x;
		y = Constrain.y;
		}
	}
if (ConstrainingX)
	x = Constrain.x;
if (ConstrainingY)
	y = Constrain.y;
*lpX = x;
*lpY = y;
fMove = x != LastPos.x || y != LastPos.y;
LastPos.x = x;
LastPos.y = y;
return(fMove);
}

/************************************************************************/
BOOL ConstrainX()
/************************************************************************/
{
return(ConstrainingX);
}

/************************************************************************/
BOOL ConstrainY()
/************************************************************************/
{
return(ConstrainingY);
}

