//****************************************************************************
//
// File Name:			HexColorGrid.cpp
//
// Project:				Renaissance
//
// Author:				John Fleischhauer
//
// Description:			RHexColorGrid definition
//
// Portability:			Windows
//
// Copyright (C) 1998 Broderbund Software, Inc.  All Rights Reserved.
//
//
// *****************************************************************************

//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME

#include "HexColorGrid.h"


RHexColorGrid::RHexColorGrid( int levels )
	: m_maxIndex( levels )
{
	Reset();
}	

// copy constructor
RHexColorGrid::RHexColorGrid( RHexColorGrid& src )
	: m_maxIndex( src.m_maxIndex )
{
	m_hexCoord[ kIndexRed ]   = src.m_hexCoord[ kIndexRed ]   ;
	m_hexCoord[ kIndexGreen ] = src.m_hexCoord[ kIndexGreen ] ;
	m_hexCoord[ kIndexBlue ]  = src.m_hexCoord[ kIndexBlue ]  ;
}	

RHexColorGrid::~RHexColorGrid()
{
}	

void RHexColorGrid::Reset()
{
	m_hexCoord[ kIndexRed ]   = 0;
	m_hexCoord[ kIndexGreen ] = 0;
	m_hexCoord[ kIndexBlue ]  = 0;
}	

const RHexColorGrid& RHexColorGrid::operator=( RHexColorGrid& src )
{
	m_maxIndex = src.m_maxIndex;
	m_hexCoord[ kIndexRed ]   = src.m_hexCoord[ kIndexRed ]   ;
	m_hexCoord[ kIndexGreen ] = src.m_hexCoord[ kIndexGreen ] ;
	m_hexCoord[ kIndexBlue ]  = src.m_hexCoord[ kIndexBlue ]  ;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//
//	Routine: void WalkHexGrid( HexIndexArray& hexIndex, eDirection direction )
//
//	Description: Given a current position on the hex color grid and a direction
//				 to move, calculate the new position.
//
//////////////////////////////////////////////////////////////////////////////
//
//	The following pseudocode examples demonstrate the algorithms which are
//	generalized in this routine.
//
//	Given a hexagonal grid of m_maxIndex levels (in this example, 5), with Red (R)
//	at 0 degrees, Yellow (Y) at 60 degrees, etc., positions in the grid are
//	designated by a triplet array [RED, GREEN, BLUE] as follows:
//
//                        (G)                                                    (Y)
//
//                           [0 5 0]  [1 5 0]  [2 5 0]  [3 5 0]  [4 5 0]  [5 5 0]
//
//                      [0 5 1]  [0 4 0]  [1 4 0]  [2 4 0]  [3 4 0]  [4 4 0]  [5 4 0]
//
//                 [0 5 2]  [0 4 1]  [0 3 0]  [1 3 0]  [2 3 0]  [3 3 0]  [4 3 0]  [5 3 0]
//
//             [0 5 3]  [0 4 2]  [0 3 1]  [0 2 0]  [1 2 0]  [2 2 0]  [3 2 0]  [4 2 0]  [5 2 0]
//
//         [0 5 4]  [0 4 3]  [0 3 2]  [0 2 1]  [0 1 0]  [1 1 0]  [2 1 0]  [3 1 0]  [4 1 0]  [5 1 0]
//
//(C)  [0 5 5]  [0 4 4]  [0 3 3]  [0 2 2]  [0 1 1]  [0 0 0]  [1 0 0]  [2 0 0]  [3 0 0]  [4 0 0]  [5 0 0]  (R)
//
//         [0 4 5]  [0 3 4]  [0 2 3]  [0 1 2]  [0 0 1]  [1 0 1]  [2 0 1]  [3 0 1]  [4 0 1]  [5 0 1]
//
//	           [0 3 5]  [0 2 4]  [0 1 3]  [0 0 2]  [1 0 2]  [2 0 2]  [3 0 2]  [4 0 2]  [5 0 2]
//
//                 [0 2 5]  [0 1 4]  [0 0 3]  [1 0 3]  [2 0 3]  [3 0 3]  [4 0 3]  [5 0 3]
//
//                     [0 1 5]  [0 0 4]  [1 0 4]  [2 0 4]  [3 0 4]  [4 0 4]  [5 0 4]
//
//                         [0 0 5]  [1 0 5]  [2 0 5]  [3 0 5]  [4 0 5]  [5 0 5]
//
//                     (B)                                                      (M)
//
//
//	Algorithm for movement in the direction of the primary RED, GREEN or BLUE:
//	Example 1: Movement in the direction of the primary RED (R):
//
//		if ( index[GREEN] == 0 || index[BLUE] == 0 )	// not in the Cyan "quadrant"
//		{
//			if ( index[RED] < m_maxIndex )				// not on the R-Y or the R-M facet
//			{
//				index[RED]++;							// bump red up one level
//			}
//			else
//			{
//				if ( index[GREEN] > 0 )					// on the RED-YELLOW facet
//				{
//					index[GREEN]--;						// decrease GREEN, to move toward RED
//				}
//				if ( index[BLUE] > 0 )					// on the RED-MAGENTA facet
//				{
//					index[BLUE]--;						// decrease BLUE, to move toward RED
//				}
//			}
//		}
//		else											// in the Cyan "quadrant"
//		{
//			index[GREEN]--;								// decrease	GREEN and BLUE
//			index[BLUE]--;
//		}
//
//
//	Algorithm for movement in the direction of the primary YELLOW, CYAN, or MAGENTA:
//	Example 2: Movement in the direction of the primary YELLOW:
//
//		if ( index[BLUE] > 0 )				// not in the Yellow "triad"
//		{
//			index[BLUE]--;					// decrease BLUE
//		}
//		else								// not on the RED-YELLOW or YELLOW-GREEN facet 
//		{									// decrease RED, GREEN or both
//			if ( index[RED] < m_maxIndex )
//			{
//				index[RED]++;
//			}
//			if ( index[GREEN] < m_maxIndex )
//			{
//				index[GREEN]++;
//			}
//		}
//
//////////////////////////////////////////////////////////////////////////////
//
void RHexColorGrid::NextPosition( EDirection direction )
{
	EHexCoordIndex color1, color2, color3;

	if ( direction == kDirectionNone )
	{
		return;
	}

	// a move in the direction of primary R, G or B
	if ( direction == kDirectionRed   ||
		 direction == kDirectionGreen ||
		 direction == kDirectionBlue   )
	{
		switch ( direction )
		{
			case kDirectionRed:
				color1 = kIndexRed;
				color2 = kIndexGreen;
				color3 = kIndexBlue;
				break;

			case kDirectionGreen:
				color1 = kIndexGreen;
				color2 = kIndexRed;
				color3 = kIndexBlue;
				break;

			case kDirectionBlue:
				color1 = kIndexBlue;
				color2 = kIndexRed;
				color3 = kIndexGreen;
				break;

		}

		// see function header for description
		if ( m_hexCoord[ color2 ] == 0 || m_hexCoord[ color3 ] == 0 )
		{
			if ( m_hexCoord[ color1 ] < m_maxIndex )
			{
				m_hexCoord[ color1 ]++;
			}
			else
			{
				if ( m_hexCoord[ color2 ] > 0 )
				{
					m_hexCoord[ color2 ]--;
				}
				if ( m_hexCoord[ color3 ] > 0 )
				{
					m_hexCoord[ color3 ]--;
				}
			}
		}
		else
		{
			m_hexCoord[ color2 ]--;
			m_hexCoord[ color3 ]--;
		}
	}
	else					// move in direction of primary Y, C or M
	{
		switch ( direction )
		{
			case kDirectionYellow:
				color1 = kIndexBlue;
				color2 = kIndexRed;
				color3 = kIndexGreen;
				break;

			case kDirectionCyan:
				color1 = kIndexRed;
				color2 = kIndexBlue;
				color3 = kIndexGreen;
				break;

			case kDirectionMagenta:
				color1 = kIndexGreen;
				color2 = kIndexRed;
				color3 = kIndexBlue;
				break;
		}

		// see function header for description
		if ( m_hexCoord[ color1 ] > 0 )
		{
			m_hexCoord[ color1 ]--;
		}
		else
		{
			if ( m_hexCoord[ color2 ] < m_maxIndex )
			{
				m_hexCoord[ color2 ]++;
			}
			if ( m_hexCoord[ color3 ] < m_maxIndex )
			{
				m_hexCoord[ color3 ]++;
			}
		}
	}
}	


//////////////////////////////////////////////////////////////////////////////
//
//	Routine: void CalculateAngleAndEffectFraction( HexIndexArray& hexIndex, double *pAngle, double *pEffectFraction )
//
//	Description: Given a position on the hex color grid, calculate the
//				 angle (hue) and the level of effect (0.0 - 1.0)
//
//////////////////////////////////////////////////////////////////////////////
//
//void RHexColorGrid::GetAngle( double *pAngle, double *pEffectFraction )
void RHexColorGrid::GetAngle( YHueAngle *pAngle, YFloatType *pEffectFraction )
{
	// get the max value to determine what level ("ring index") we are at
	UINT uRingIndex = max( max( m_hexCoord[ kIndexRed ], m_hexCoord[ kIndexGreen ] ), m_hexCoord[ kIndexBlue ] );

	if ( uRingIndex == 0 )
	{
		*pAngle = 0;
		*pEffectFraction = 0.0;
	}
	else
	{
		// divide by the max level to get our "effect fraction"
		*pEffectFraction = (YFloatType)uRingIndex / (YFloatType)m_maxIndex;

		YHueAngle dPrimaryAngle;
		UINT uNextIndex;
		UINT uPrevIndex;

		if ( m_hexCoord[ kIndexRed ] == uRingIndex )			// if we are "closest" to RED
		{
			dPrimaryAngle = kPrimaryAngle[ kDirectionRed ];		// 0 deg.
			uNextIndex    = m_hexCoord[ kIndexGreen ];			// "next" primary = GREEN
			uPrevIndex    = m_hexCoord[ kIndexBlue ];				// "previous" primary = BLUE
		}
		else if ( m_hexCoord[ kIndexGreen ] == uRingIndex )	// etc...
		{
			dPrimaryAngle = kPrimaryAngle[ kDirectionGreen ];	// 120 deg.
			uNextIndex    = m_hexCoord[ kIndexBlue ];
			uPrevIndex    = m_hexCoord[ kIndexRed ];
		}
		else
		{
			dPrimaryAngle = kPrimaryAngle[ kDirectionBlue ];	// 240 deg.
			uNextIndex    = m_hexCoord[ kIndexRed ];
			uPrevIndex    = m_hexCoord[ kIndexGreen ];
		}

		// now get the angle between our primary and the next or previous primary
		YHueAngle dSubAngle = 0;
		if ( uNextIndex > 0 )			// going toward the next primary
		{
			// the sub angle is 60 deg. x (the next primary's index level / max level)
			dSubAngle =  kAngleBetweenPrimaries * ((YFloatType)(uNextIndex) / (YFloatType)(uRingIndex));
		}
		else if ( uPrevIndex > 0 )
		{
			dSubAngle = -kAngleBetweenPrimaries * ((YFloatType)(uPrevIndex) / (YFloatType)(uRingIndex));
		}

		// add the primary angle and the subAngle to get the final angle
		//	example: given the position [3, 0, 4]:
		//				dPrimaryAngle = 240  (because BLUE == 4 == max)
		//	  			dSubAngle     =  60 deg. * ( 3 / 4 ) = 45 deg.
		//				*pAngle       = 240 deg. + 45 deg.   = 285 deg.
		//				
		*pAngle = dPrimaryAngle + dSubAngle;
		if ( *pAngle < 0 )
		{
			*pAngle += 360;
		}
	}
}	


#ifdef TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RHexColorGrid::Validate( )
//
//  Description:	Validates the object
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHexColorGrid::Validate( ) const
{
	RObject::Validate( );
	TpsAssertIsObject( RHexColorGrid, this );
}

#endif	//	TPSDEBUG


