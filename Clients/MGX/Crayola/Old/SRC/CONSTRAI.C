//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

typedef struct _grid
	{
	BOOL bOn;
	int xGrid, dxGrid, hxPosGrid, hxNegGrid;
	int yGrid, dyGrid, hyPosGrid, hyNegGrid;
} GRID;

static GRID G;
static GRID GSaved;

/************************************************************************/
void Grid( BOOL bOn, int x, int y, int dx, int dy )
/************************************************************************/
{
G.bOn  = bOn;
G.xGrid  = x;
G.yGrid  = y;
G.dxGrid = dx;
G.dyGrid = dy;
G.hxPosGrid = (dx / 2);
G.hxNegGrid = dx - G.hxPosGrid;
G.hyPosGrid = (dy / 2);
G.hyNegGrid = dy - G.hyPosGrid;
}

/************************************************************************/
void GridSave( void )
/************************************************************************/
{
GSaved = G;
}

/************************************************************************/
void GridRestore( void )
/************************************************************************/
{
G = GSaved;
}

/************************************************************************/
void GridPoint( LPPOINT lpPoint )
/************************************************************************/
{
int x, y;

if ( !G.bOn )
	return;

// Subtract the grid offset, then add half for rounding, then
// Divide by the grid size, then multiply by the grid size, then
// Add back the grid offset
x = lpPoint->x - G.xGrid;
if ( x > 0 )
		lpPoint->x = G.xGrid + G.dxGrid * ( (x + G.hxPosGrid) / G.dxGrid );
else	lpPoint->x = G.xGrid + G.dxGrid * ( (x - G.hxNegGrid) / G.dxGrid );
y = lpPoint->y - G.yGrid;
if ( y > 0 )
		lpPoint->y = G.yGrid + G.dyGrid * ( (y + G.hyPosGrid) / G.dyGrid );
else	lpPoint->y = G.yGrid + G.dyGrid * ( (y - G.hyNegGrid) / G.dyGrid );
}

static BOOL ConstrainingX, ConstrainingY;

/************************************************************************/
BOOL ConstrainXY(
/************************************************************************/
LPLONG 	lpMousePos,
BOOL 	fDown,
UINT 	msg,
BOOL 	fConstrainMode)
{
BOOL fMove;
LPARAM lParam;
int dx, dy, x, y;
static BOOL ConstrainLooking;
static POINT Constrain, LastPos;

GridPoint( (LPPOINT)lpMousePos );

lParam = *lpMousePos;
x = LOWORD(lParam);
y = HIWORD(lParam);
if (msg == WM_LBUTTONDOWN)
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
*lpMousePos = MAKELONG(x, y);
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