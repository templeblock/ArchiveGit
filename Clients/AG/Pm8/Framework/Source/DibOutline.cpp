// ****************************************************************************
//
//  File Name:			DibOutline.cpp
//
//  Project:			TPS Libraries
//
//  Author:				Greg Brown, Bruce Rosenblum
//
//  Description:		Advanced DIB manipulation functions
//
//  Portability:		Platform independent, 32 bit systems only
//
//  Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Turning Point Software         
//
//  Copyright (C) 1996 Turning Point Software, Inc. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/DibOutline.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "DibOutline.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "PixelGrid.h"

//	Directions for walking outline of bitmap
const uBYTE EAST = 0;
const uBYTE N_EAST = 1;
const uBYTE NORTH = 2;
const uBYTE N_WEST = 3;
const uBYTE WEST = 4;
const uBYTE S_WEST = 5;
const uBYTE SOUTH = 6;
const uBYTE S_EAST = 7;
const uBYTE NULL_DIRECTION = 8;

//	local prototypes
static BOOLEAN FollowOutline (const RIntPoint& StartPoint,
						  uBYTE InitialDirection,
						  const DIBINFO& Dib,
						  CPixelGrid& PixelGrid,
						  RPolyPolygon& rPolyPoly);

static uBYTE FindNextPoint (const RIntPoint& LastPoint, 
								 RIntPoint& NextPoint,
								 uBYTE Direction,
								 const DIBINFO& Dib);

#ifdef	TPSDEBUG
static void _AssertDIBInfo (const DIBINFO& Dib);
#else
#define	_AssertDIBInfo(x)	{}
#endif	/* TPSDEBUG */

// ****************************************************************************
//
//  Function Name:	OutlineDibShapes()
//
//  Description:		Finds outline of all shapes defined by non-
//										transparent pixels within a given DIB.
//										Returns points in a RPolyPolygon structure.
//
//  Returns:			Boolean success (TRUE) or failure (FALSE)
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN OutlineDibShapes (const DIBINFO& Dib, RPolyPolygon& rPolyPoly)
{	
	_AssertDIBInfo(Dib);

	sLONG			DibWidth = Dib.Width;
	sLONG			DibHeight = Dib.Height;
	RIntPoint	StartPoint;
	CPixelGrid	PixelGrid;
	
	try
	{
		// Initialize pixel grid
		PixelGrid.Initialize (DibWidth, DibHeight);
		
		sLONG MinX = DibWidth - 1;
		sLONG MaxX = 0;
		sLONG MinY = -1;	
		sLONG MaxY;
		
		// Scan horizontally
		for (StartPoint.m_y = 0; StartPoint.m_y < DibHeight; StartPoint.m_y++)
		{			
			if (FindFirstEdge (StartPoint, Dib, LEFT, 0))
			{
				if (MinY < 0) MinY = StartPoint.m_y;
				MaxY = StartPoint.m_y;
				if (StartPoint.m_x < MinX) MinX = StartPoint.m_x;
				FollowOutline (StartPoint, N_WEST, Dib, PixelGrid, rPolyPoly);
				if (FindFirstEdge (StartPoint, Dib, RIGHT, (DibWidth - 1)))
				{
					if (StartPoint.m_x > MaxX) MaxX = StartPoint.m_x;
					FollowOutline (StartPoint, S_EAST, Dib, PixelGrid, rPolyPoly);
				}
			}				
		}

		// Scan vertically
		for (StartPoint.m_x = MinX; StartPoint.m_x < MaxX; StartPoint.m_x++)
		{			
			if (FindFirstEdge (StartPoint, Dib, TOP, MaxY))
			{
				FollowOutline (StartPoint, N_EAST, Dib, PixelGrid, rPolyPoly);
				if (FindFirstEdge (StartPoint, Dib, BOTTOM, MinY))
				{
					FollowOutline (StartPoint, S_WEST, Dib, PixelGrid, rPolyPoly);
				}
			}		
		}
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}


// ****************************************************************************
//
//  Function Name:	FollowOutline()
//
//  Description:		Follow the outline from a starting point in a DIB.
//
//  Returns:			TRUE if points added to Polygon, FALSE if not
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
static BOOLEAN FollowOutline (const RIntPoint& StartPoint,
						  uBYTE InitialDirection,
						  const DIBINFO& Dib,
						  CPixelGrid& PixelGrid,
						  RPolyPolygon& rPolyPoly)
{

	uBYTE			Direction, PreviousDirection;
	RIntPoint	LastPoint, NextPoint;		

	if (PixelGrid.IsPointDirty (StartPoint))
	{
		// This point has already been used
		return FALSE;
	}

	Direction = InitialDirection;
	PreviousDirection = NULL_DIRECTION;
	LastPoint = StartPoint;
	rPolyPoly.NewPolygon();	
	do
	{
		Direction = FindNextPoint (LastPoint, NextPoint, Direction, Dib);
		
		if (Direction == NULL_DIRECTION)
		{
			// There is only one pixel in this shape									
			rPolyPoly.AddPoint (LastPoint);			
		}
		else if (Direction != PreviousDirection)
		{				
			rPolyPoly.AddPoint (LastPoint);
		}

		PreviousDirection = Direction;
		// Adjust direction
		if (Direction & 0x01)
		{
			// Direction is odd
			Direction = (uBYTE)((Direction + 2) & 0x07);
		}
		else
		{
			// Direction is even
			Direction = (uBYTE)((Direction + 1) & 0x07);
		}

		LastPoint	= NextPoint;
		
		// Must add all points traversed to pixel grid so they are not
		// considered as part of another shape			
		PixelGrid.MarkPoint (LastPoint);		
	}
	while ( LastPoint != StartPoint );
	
	// Make sure that start point is also end point of polyline
	rPolyPoly.AddPoint (LastPoint);
	//	Notify class this polygon is done
	rPolyPoly.EndPolygon();
	
	return TRUE;
}

	
// ****************************************************************************
//
//  Function Name:	FindNextPoint()
//
//  Description:		Find next point while following outline based on direction.
//
//  Returns:			DIRECTION of next point (see enums at top of file)
//
//  Exceptions:		none
//
// ****************************************************************************
static uBYTE FindNextPoint (const RIntPoint& LastPoint, RIntPoint& NextPoint,
									uBYTE Direction, const DIBINFO& Dib)
{		
	sLONG DibWidth = Dib.Width;
	sLONG DibHeight = Dib.Height;
	BOOLEAN PointFound = FALSE;	
	sLONG PixelIndex;
	sLONG ThisRowIndex, NextRowIndex, PrevRowIndex;
	sLONG ThisColIndex, NextColIndex, PrevColIndex;
	uBYTE LoopCount = 0;
	uLONG BytesPerRow = Dib.BytesPerRow; //ByteAlign(DibWidth, Dib.BytesPerPixel);
		
	ThisRowIndex = LastPoint.m_y * BytesPerRow;
	NextRowIndex = ThisRowIndex + BytesPerRow;
	PrevRowIndex = ThisRowIndex - BytesPerRow;

	ThisColIndex = LastPoint.m_x * Dib.BytesPerPixel;
	NextColIndex = ThisColIndex + Dib.BytesPerPixel;
	PrevColIndex = ThisColIndex - Dib.BytesPerPixel;
	
	while (!PointFound)
	{
		switch (Direction)
		{
		case EAST:
			NextPoint.m_x = LastPoint.m_x + 1;
			NextPoint.m_y = LastPoint.m_y;			
			PixelIndex = ThisRowIndex + NextColIndex;
			break;

		case N_EAST:
			NextPoint.m_x = LastPoint.m_x + 1;
			NextPoint.m_y = LastPoint.m_y + 1;			
			PixelIndex = NextRowIndex + NextColIndex;
			break;

		case NORTH:
			NextPoint.m_x = LastPoint.m_x;
			NextPoint.m_y = LastPoint.m_y + 1;			
			PixelIndex = NextRowIndex + ThisColIndex;
			break;

		case N_WEST:
			NextPoint.m_x = LastPoint.m_x - 1;
			NextPoint.m_y = LastPoint.m_y + 1;			
			PixelIndex = NextRowIndex + PrevColIndex;
			break;

		case WEST:
			NextPoint.m_x = LastPoint.m_x - 1;
			NextPoint.m_y = LastPoint.m_y;			
			PixelIndex = ThisRowIndex + PrevColIndex;
			break;

		case S_WEST:
			NextPoint.m_x = LastPoint.m_x - 1;
			NextPoint.m_y = LastPoint.m_y - 1;
			PixelIndex = PrevRowIndex + PrevColIndex;
			break;

		case SOUTH:
			NextPoint.m_x = LastPoint.m_x;
			NextPoint.m_y = LastPoint.m_y - 1;			
			PixelIndex = PrevRowIndex + ThisColIndex;
			break;

		case S_EAST:
			NextPoint.m_x = LastPoint.m_x + 1;
			NextPoint.m_y = LastPoint.m_y - 1;
			PixelIndex = PrevRowIndex + NextColIndex;
			break;
		
		default:
			TpsAssertAlways("Bad case statement value");
			break;
		
		}

		// If the point falls outside of the bitmap, change direction appropriately
		if ((NextPoint.m_x < 0) 
			|| (NextPoint.m_x > (sLONG) DibWidth - 1) 
			|| (NextPoint.m_y < 0) 
			|| (NextPoint.m_y > (sLONG) DibHeight - 1))
		{
			Direction = (uBYTE)((--Direction) & 0x07);
		}
		else
		{
			LoopCount++;		
			if (LoopCount > S_EAST)
			{
				// Only 1 pixel in this shape
				NextPoint = LastPoint;
				return NULL_DIRECTION;
			}

			//PixelIndex = NextPoint.y * BytesPerRow + NextPoint.x * Dib.BytesPerPixel;			
			switch (Dib.BytesPerPixel)
			{
			case 1:
				if (Dib.pBits[PixelIndex] == (uBYTE) Dib.TransparentColor)
				{
					Direction = (uBYTE)((--Direction) & 0x07);			
				}
				else
				{
					PointFound = TRUE;
				}
				break;

			case 2:
				if (*((uWORD*) &Dib.pBits[PixelIndex]) == (uWORD) Dib.TransparentColor)
				{	
					Direction = (uBYTE)((--Direction) & 0x07);			
				}
				else
				{
					PointFound = TRUE;
				}
				break;
			
			case 3:	
				if ((*((uLONG*) &Dib.pBits[PixelIndex]) & 0x00FFFFFF) == Dib.TransparentColor)				
				{				
					Direction = (uBYTE)((--Direction) & 0x07);			
				}
				else
				{
					PointFound = TRUE;
				}					
				break;

			case 4:
				if (*((uLONG*) &Dib.pBits[PixelIndex]) == Dib.TransparentColor)
				{						
					Direction = (uBYTE)((--Direction) & 0x07);			
				}
				else
				{
					PointFound = TRUE;
				}
				break;

			default:
				TpsAssertAlways("Bad case statement value");
			}			
		}
	}
	return Direction;
}

// ****************************************************************************
//
//  Function Name:	FindFirstEdge()
//
//  Description:		Find first non-transparent pixel in a DIB from ORIENTATION.
//
//  Returns:			TRUE if found, return in StartPoint
//
//  Exceptions:		none
//
// ****************************************************************************
BOOLEAN FindFirstEdge (RIntPoint& StartPoint, 
						  const DIBINFO& Dib, 
						  const ORIENTATION FromWhere,
						  const sLONG StartAt)
{
	sLONG DibWidth = Dib.Width;
	sLONG DibHeight = Dib.Height;	
	sLONG Index;	
	uLONG BytesPerRow = Dib.BytesPerRow;

	_AssertDIBInfo(Dib);

	switch (FromWhere)
	{
	case LEFT:		
		Index = StartPoint.m_y * BytesPerRow + StartAt * Dib.BytesPerPixel;
		for (StartPoint.m_x = 0; StartPoint.m_x < DibWidth; StartPoint.m_x++)
		{
			switch (Dib.BytesPerPixel)
			{
			case 1:
				if (Dib.pBits[Index] != (uBYTE) Dib.TransparentColor) return TRUE;	
				break;

			case 2:
				if (*((uWORD*) &Dib.pBits[Index]) != (uWORD) Dib.TransparentColor) return TRUE;
				break;	
			
			case 3:	
				if ((*((uLONG*) &Dib.pBits[Index]) & 0x00FFFFFF) != Dib.TransparentColor) return TRUE;				
				break;

			case 4:
				if (*((uLONG*) &Dib.pBits[Index]) != Dib.TransparentColor) return TRUE;
				break;

			default:
				TpsAssertAlways("Bad case statement value");
			}
			Index += Dib.BytesPerPixel;
		}
		break;

	case RIGHT:
		Index = StartPoint.m_y * BytesPerRow + StartAt * Dib.BytesPerPixel;
		for (StartPoint.m_x = DibWidth - 1; StartPoint.m_x >= 0; StartPoint.m_x--)
		{
			switch (Dib.BytesPerPixel)
			{
			case 1:
				if (Dib.pBits[Index] != (uBYTE) Dib.TransparentColor) return TRUE;	
				break;

			case 2:
				if (*((uWORD*) &Dib.pBits[Index]) != (uWORD) Dib.TransparentColor) return TRUE;
				break;	
			
			case 3:	
				if ((*((uLONG*) &Dib.pBits[Index]) & 0x00FFFFFF) != Dib.TransparentColor) return TRUE;
				break;

			case 4:
				if (*((uLONG*) &Dib.pBits[Index]) != Dib.TransparentColor) return TRUE;
				break;

			default:
				TpsAssertAlways("Bad case statement value");
			}
			Index -= Dib.BytesPerPixel;
		}
		break;

	case TOP:
		Index = (StartAt) * BytesPerRow + StartPoint.m_x * Dib.BytesPerPixel;
		for (StartPoint.m_y = StartAt; StartPoint.m_y >= 0; StartPoint.m_y--)
		{
			switch (Dib.BytesPerPixel)
			{
			case 1:
				if (Dib.pBits[Index] != (uBYTE) Dib.TransparentColor) return TRUE;	
				break;

			case 2:
				if (*((uWORD*) &Dib.pBits[Index]) != (uWORD) Dib.TransparentColor) return TRUE;
				break;	
			
			case 3:	
				if ((*((uLONG*) &Dib.pBits[Index]) & 0x00FFFFFF) != Dib.TransparentColor) return TRUE;
				break;

			case 4:
				if (*((uLONG*) &Dib.pBits[Index]) != Dib.TransparentColor) return TRUE;
				break;

			default:
				TpsAssertAlways("Bad case statement value");
			}
			Index -= BytesPerRow;
		}
		break;

	case BOTTOM:
		Index = (StartAt) * BytesPerRow + StartPoint.m_x * Dib.BytesPerPixel;
		for (StartPoint.m_y = StartAt; StartPoint.m_y < DibHeight; StartPoint.m_y++)
		{
			switch (Dib.BytesPerPixel)
			{
			case 1:
				if (Dib.pBits[Index] != (uBYTE) Dib.TransparentColor) return TRUE;	
				break;

			case 2:
				if (*((uWORD*) &Dib.pBits[Index]) != (uWORD) Dib.TransparentColor) return TRUE;
				break;	
			
			case 3:	
				if ((*((uLONG*) &Dib.pBits[Index]) & 0x00FFFFFF) != Dib.TransparentColor) return TRUE;
				break;

			case 4:
				if (*((uLONG*) &Dib.pBits[Index]) != Dib.TransparentColor) return TRUE;
				break;

			default:
				TpsAssertAlways("Bad case statement value");
			}
			Index += BytesPerRow;
		}
		break;
	}

	return FALSE;
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	_AssertDIBInfo()
//
//  Description:		Validate information in DIBINFO structure.
//
//  Returns:			none
//
//  Exceptions:		none
//
// ****************************************************************************
static void _AssertDIBInfo (const DIBINFO& Dib)
{
	TpsAssert(Dib.pBits != NULL, "Bad DIB pointer");
	TpsAssert(Dib.Width > 0 &&  Dib.Width < 0x3FFF, "Bad DIB width");
	TpsAssert(Dib.Height > 0 &&  Dib.Height < 0x3FFF, "Bad DIB height");
	TpsAssert(Dib.BytesPerPixel > 0 && Dib.BytesPerPixel <= 4, "Bad DIB bytes/pixel");
	TpsAssert(Dib.BytesPerRow > 0 && Dib.BytesPerRow < 0x7FFF, "Bad DIB bytes/row");
}

#endif	/* TPSDEBUG */

