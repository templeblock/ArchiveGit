/****************************************************************************

	File:		SCHRECT.C


	$Header: /Projects/Toolbox/ct/SCHRECT.CPP 2 	5/30/97 8:45a Wmanis $

	Contains:	
		This file duplicates in high res rectangles the 
			'Calculations on Rectangles' described in Inside MAC I-174


	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
****************************************************************************/

#include "sctypes.h"


#if defined( _MSC_VER )
	#pragma warning(disable:4244)		// disable - int conversion
#endif

/* ==================================================================== */
/* ==================================================================== */
/* ====================    CMUPOINT    =============================== */
/* ==================================================================== */
/* ==================================================================== */

void scMuPoint::FourthToThird( MicroPoint w )
{
	MicroPoint	xPrime,
				yPrime;

	scAssert( x != kInvalMP || y != kInvalMP );

	xPrime	= y;
	yPrime	= w - x;
	x		= xPrime;
	y		= yPrime;
}

/* ====================================================================== */


void scMuPoint::ThirdToFourth( MicroPoint w )
{
	MicroPoint	xPrime,
				yPrime;

	scAssert( x != kInvalMP || y != kInvalMP );

	xPrime	= w - y;
	yPrime	= x;
	x		= xPrime;
	y		= yPrime;
}

/* ====================================================================== */


/* ==================================================================== */
/* ==================================================================== */
/* ==================== 	CEXRECT 	 ============================== */
/* ==================================================================== */
/* ==================================================================== */


#if SCDEBUG > 1

scChar* scXRect::DebugStr( scChar* buf, int factor ) const
{
#if defined(SCWINDOWS)
	wsprintf( buf, scString( "(%d, %d, %d, %d)" ), x1 / factor, y1 / factor, x2 / factor, y2 / factor );
#else
	sprintf( buf, scString( "(%d, %d, %d, %d)" ), x1 / factor, y1 / factor, x2 / factor, y2 / factor );
#endif
	return buf;
}

#endif

/* ==================================================================== */

Bool scXRect::Valid( eCoordSystem coordSys ) const
{
	switch ( coordSys ) {
		case eFirstQuad:
			return x1 <= x2 && y1 >= y2;
		case eSecondQuad:
			return x1 >= x2 && y1 >= y2;
		case eThirdQuad:
			return x1 >= x2 && y1 <= y2;
		case eFourthQuad:
			return x1 <= x2 && y1 <= y2;
	}
	return false;
}

/* ==================================================================== */

void scXRect::Scale( TenThousandth factor )
{
#if SCDEBUG>2
	scAssert( Valid() );
#endif

	x1 = scRoundMP( (REAL)x1 * factor / 10000.0 );
	x2 = scRoundMP( (REAL)x2 * factor / 10000.0 );
	y1 = scRoundMP( (REAL)y1 * factor / 10000.0 );
	y2 = scRoundMP( (REAL)y2 * factor / 10000.0 );
}

/* ==================================================================== */

void scXRect::Scale( REAL factor )
{
#if SCDEBUG>2
	scAssert( Valid() );
#endif

	x1 = scRoundMP( (REAL)x1 * factor );
	x2 = scRoundMP( (REAL)x2 * factor );
	y1 = scRoundMP( (REAL)y1 * factor );
	y2 = scRoundMP( (REAL)y2 * factor );
}

/* ==================================================================== */

void scXRect::FourthToThird( MicroPoint w )
{
	scMuPoint	pt1,
				pt2;

#if SCDEBUG>2
	scAssert( Valid() );
#endif

	pt1.x = x2;
	pt1.y = y1;

	pt1.FourthToThird( w );

	pt2.x = x1;
	pt2.y = y2;

	pt2.FourthToThird( w );

	x1 = pt1.x;
	y1 = pt1.y;

	x2 = pt2.x;
	y2 = pt2.y;

#if SCDEBUG>2
	scAssert( Valid() );
#endif
}

/* ==================================================================== */

void scXRect::ThirdToFourth( MicroPoint w )
{
	scMuPoint	pt1,
				pt2;

#if SCDEBUG>2
	scAssert( Valid() );
#endif

	pt1.x = x1;
	pt1.y = y2;

	pt1.ThirdToFourth( w );

	pt2.x = x2;
	pt2.y = y1;

	pt2.ThirdToFourth( w );

	x1 = pt1.x;
	y1 = pt1.y;

	x2 = pt2.x;
	y2 = pt2.y;

#if SCDEBUG>2
	scAssert( Valid() );
#endif
}

/* ==================================================================== */

void scXRect::FirstToFourth( MicroPoint d )
{
#if SCDEBUG>2
	scAssert( Valid() );
#endif

	y1 = -y1;
	y2 = -y2;

#if SCDEBUG>2
	scAssert( Valid() );
#endif
	
}

/* ==================================================================== */

void scXRect::FourthToFirst( MicroPoint d )
{
#if SCDEBUG>2
	scAssert( Valid() );
#endif

	y1 -= d;
	y2 -= d;
	
#if SCDEBUG>2
	scAssert( Valid() );
#endif	
}

/* ==================================================================== */

/* ==================================================================== */
/* ==================================================================== */
/* ==================== 	CRLURECT	  ============================= */
/* ==================================================================== */
/* ==================================================================== */


scRLURect::scRLURect( )
{
		// in an attemp to insure that we can freely convert
		// back and forth between these we do the following text
	scAssert( sizeof( scRLURect ) == sizeof( RLU ) * 4 );

	Invalidate();
}

/* ====================================================================== */

scRLURect::scRLURect( const scRLURect& rlurect )
{
	rluLeft 	= rlurect.rluLeft;
	rluTop		= rlurect.rluTop;
	rluRight	= rlurect.rluRight;
	rluBottom	= rlurect.rluBottom;
}

/* ====================================================================== */

void scRLURect::Set( RLU left, RLU top, RLU right, RLU bottom )
{
	rluLeft 	= left;
	rluTop		= top;
	rluRight	= right;
	rluBottom	= bottom;
}

/* ====================================================================== */

Bool	scRLURect::Valid( eCoordSystem coordSys ) const
{
	switch ( coordSys ) {
		case eFirstQuad:
			return rluLeft <= rluRight && rluTop >= rluBottom;
		case eSecondQuad:
			return rluLeft >= rluRight && rluTop >= rluBottom;
		case eThirdQuad:
			return rluLeft >= rluRight && rluTop <= rluBottom;
		case eFourthQuad:
			return rluLeft <= rluRight && rluTop <= rluBottom;
	}
	return false;
}

/* ====================================================================== */

void scRLURect::Invalidate( )
{
	Set( SHRT_MAX, SHRT_MAX, SHRT_MIN, SHRT_MIN );
}

/* ====================================================================== */

void scRLURect::Translate( RLU h, RLU v )
{
	rluRight	= rluRight	+ h;
	rluLeft 	= rluLeft	+ h;
	rluTop		= rluTop	+ v;
	rluBottom	= rluBottom + v;
}

/* ====================================================================== */

void	scRLURect::FirstToFourth( RLU )
{
	rluTop		= -rluTop;
	rluBottom	= -rluBottom;
}
	
/* ==================================================================== */

void scRLURect::FourthToFirst( RLU )
{
	rluTop		= -rluTop;
	rluBottom	= -rluBottom;
}

/* ==================================================================== */

void	scRLURect::RLURomanBaseLineToCenter( void )
{
	rluRight	= (rluRight - rluLeft)/2;
	rluLeft 	= 0 - rluRight;

		//use bottom as temp variable to save height
	rluBottom	= rluTop - rluBottom;
	rluTop		= scBaseRLUsystem - ( rluTop + RLU_BASEfmBottom);
	rluBottom	= rluTop + rluBottom;	//Bottom has character height
}

/* ====================================================================== */

void	scRLURect::RLURomanBaseLineToLeft( void )
{
	rluRight	= (rluRight - rluLeft);
	rluLeft 	= 0;

		//use bottom as temp variable to save height
	rluBottom	= rluTop - rluBottom;
	rluTop		= scBaseRLUsystem - ( rluTop + RLU_BASEfmBottom);
	rluBottom	= rluTop + rluBottom;	//Bottom has character height
}

/* ====================================================================== */

void	scRLURect::RLURomanBaseLineToRight( void )
{
	rluLeft 	= 0 - (rluRight - rluLeft);
	rluRight	= 0;
	rluBottom	= rluTop - rluBottom;	//use bottom as temp variable to save height
	rluTop		= scBaseRLUsystem - ( rluTop + RLU_BASEfmBottom);
	rluBottom	= rluTop + rluBottom;	//Bottom has character height
}

/* ====================================================================== */

void	scRLURect::RLURomanBaseLineToTop( void )
{
	Translate( 0, -RLU_BASEfmTop );
}

/* ====================================================================== */

void	scRLURect::RLURomanBaseLineToMiddle( void )
{
	Translate( 0, -RLU_BASEfmTop/2 );
}

/* ====================================================================== */

void	scRLURect::RLURomanBaseLineToBottom( void )
{
	Translate( 0, RLU_BASEfmBottom );
}

/* ====================================================================== */
#if 0
void RectTest( )
{

	scXRect xrect( 100, 100, 200, 200 );
	scMuPoint	pt1;

	scMuPoint	pt2;

	pt1.x = 20;
	pt1.y = 80;

	pt2 = pt1;

	xrect.FourthToThird( 1000 );
	xrect.ThirdToFourth( 1000 );

	pt1.FourthToThird( 1000 );
	pt1.ThirdToFourth( 1000 );


	pt1.FourthToThird( 100 );
	pt1.ThirdToFourth( 100 );


	pt1.FourthToThird( 200 );
	pt1.ThirdToFourth( 200 );


	pt1.FourthToThird( 500 );
	pt1.ThirdToFourth( 500 );



	scAssert( pt1 == pt2 );
}
#endif

/* ====================================================================== */
