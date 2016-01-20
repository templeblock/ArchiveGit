// ****************************************************************************
//
//  File Name:			GradientFill.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Dunlap, R. Hood, M. Houle
//
//  Description:		Implementation of the RGradientFill class
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/GradientFill.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "GradientFill.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "RenderToScreen.h"

//
// with DEBUG_SINGLE_STEP defined (also TPSDEBUG), gradients will pause before
// being drawn, waiting for the user to press either the control key or the
// shift key.  If the control key is pressed, then the gradient will just be
// drawn.  if the shift key is pressed, then drawing will pause after each
// color band, waiting for either the control or the shift key to be pressed.
// if the shift key is pressed, then the band will be rendered again (passed
// through the clipper) with debug output turned on.
//
//#define	DEBUG_SINGLE_STEP

//
// if FIX_BURST_ANGLE_AT_LONGEST is TRUE, then square and circular burst gradients will
// adjust their angle to 0 or 90 degrees (whichever direction has the larger dimension
// in the new bounding rect) when the bounding rectangle is set.  Otherwise the angle
// will not be adjusted...
//
// if FIX_BURST_LENGTH_AT_LONGEST is TRUE, then the vector length is fixed as
// above, but the angle is not...
//
// NOTE: ONLY ONE OF THESE (or neither) MUST BE DEFINED
#define	FIX_BURST_ANGLE_AT_LONGEST		FALSE
#define	FIX_BURST_LENGTH_AT_LONGEST	TRUE
#if FIX_BURST_ANGLE_AT_LONGEST
#if FIX_BURST_LENGTH_AT_LONGEST
#error "Both FIX_BURST_LENGTH_AT_LONGEST and FIX_BURST_ANGLE_AT_LONGEST are TRUE"
#endif
#endif

#ifdef IMAGER

	#include "GlobalBuffer.h"

	inline YPercentage 	GetGradientDisplayPrecision()	{ return( 0.98 ); }
	inline YPercentage 	GetGradientPrintPrecision() 	{ return( 1.0 ); }
	inline YIntDimension	GetMaxGradientDPI() 				{ return( 72 ); }

#else

	#include "ApplicationGlobals.h"

#endif

#include	"DcDrawingSurface.h"
#include "Path.h"
#include "GradientFill.h"
#include	"ClipPolygon.h"

//
// See if we should abort drawing the gradient
inline BOOLEAN QueryAbortGradient( RDrawingSurface& ds )
{
	//	Spin the cursor to be nice...
	RCursor( ).SpinCursor( );
	//	Should we abort the print job.
	return BOOLEAN( ds.IsPrintingCanceled() );
}


//
//	Helper class for color cleanup...
class RFillColorRestorer
{
public :
	RFillColorRestorer( RDrawingSurface& ds ) : m_ds( ds ), m_Color( ds.GetFillColor( ) ) { ; }
	~RFillColorRestorer( )	{ m_ds.SetFillColor( m_Color ); }
private :
	RDrawingSurface&	m_ds;
	RColor				m_Color;
} ;

#ifdef TPSDEBUG
#ifdef DEBUG_SINGLE_STEP
static BOOLEAN	fskipdump	= TRUE;
#endif
#endif

// ****************************************************************************
//
//  RGradientParams::Write( RArchive& archive )
//
//  @MFunc Writes this gradients data to the specified archive
//
//  @RDesc nothing
//
void RGradientParams::Write( RArchive& archive ) const
	{
	archive << static_cast<uWORD>( m_eType );
	archive << m_ptOrigin;
	archive << m_angVector;
	archive << m_rdlenVector;
	archive << m_pctposHilite;
	archive << m_angHilite;
	}

// ****************************************************************************
//
//  RGradientParams::Read( RArchive& archive )
//
//  @MFunc Reads this gradients data from the specified archive
//
//  @RDesc nothing
//
void RGradientParams::Read( RArchive& archive )
	{
	uWORD uwTemp;

	archive >> uwTemp;
	m_eType = static_cast<EGradientType>( uwTemp );

	archive >> m_ptOrigin;
	archive >> m_angVector;
	archive >> m_rdlenVector;
	archive >> m_pctposHilite;
	archive >> m_angHilite;
	}

inline YPercentage ReducePrecision( YPercentage pctValue, YPercentage pctPrecision )
{
	//
	// at 100% (pctPrecision==1.0), we simply return the value...
	if( AreFloatsEqual(pctPrecision,YPercentage(1.0)) )
		return pctValue;
	//
	// at less than 100%, the number of steps in 100% is computed by (1.0/(1.0-pctPrecision))
	// giving a step size of (1.0-pctPrecision)...
	YFloatType	dStepSize	= YFloatType(1.0 - pctPrecision);
	//
	// Round the percentage to a step position
	return YPercentage( dStepSize * int(pctValue/dStepSize) );
}


inline YPercentage MixPercentages( YPercentage pctValue, YPercentage pctMidPoint, YPercentage pctPrecision )
{
	YPercentage	pctMix;
	//
	// Reduce the precision of the subramp position
	pctValue	= ReducePrecision( pctValue, pctPrecision );
	double		base	= pctMidPoint;
	double		p		= log(0.5)/log(base);
	pctMix				= YPercentage(pow((double)pctValue,(double)p));
	return pctMix;
}

inline BOOLEAN IsFloatLess( YFloatType a, YFloatType b, YFloatType delta=0.001 )
{
	return BOOLEAN( (b-a) > delta );
}

inline BOOLEAN IsFloatBetween( YFloatType a, YFloatType b, YFloatType v, YFloatType delta=0.001 )
{
	if( IsFloatLess(a,v,delta) && IsFloatLess(b,v,delta) ) return FALSE;
	if( IsFloatLess(v,a,delta) && IsFloatLess(v,b,delta) ) return FALSE;
	return TRUE;
}

static YPointCount IntersectVertical( const RRealPoint& p0, const RRealPoint& p1, YRealCoordinate x, RRealPoint& pt,
													int& fIntersectFlag, int fIntersectBit )
{
	//
	// Check for the target X being within the linesegment...
	if( !IsFloatBetween(p0.m_x,p1.m_x,x) )	return 0;
	if( AreFloatsEqual(p0.m_x,p1.m_x) )	return 0;

	//
	// X is on the line, and the line's not vertical, so compute the intersection point
	RRealSize	szDelta	= p1 - p0;
	RRealPoint	ptTmp;
	ptTmp.m_x	= YRealCoordinate(x);
	ptTmp.m_y	= p0.m_y + (szDelta.m_dy * (ptTmp.m_x-p0.m_x) / szDelta.m_dx);
	pt	= ptTmp;	// Rounds the coordinates!
	fIntersectFlag |= fIntersectBit;
	return 1;
}

const int	kIntersectLeft		= 0x01;
const int	kIntersectRight	= 0x02;
const int	kIntersectTop		= 0x04;
const int	kIntersectBottom	= 0x08;
const int	kIntersectLTR		= (kIntersectLeft|kIntersectTop|kIntersectRight);
const	int	kIntersectLTB		= (kIntersectLeft|kIntersectTop|kIntersectBottom);
const	int	kIntersectLRB		= (kIntersectLeft|kIntersectRight|kIntersectBottom);
const int	kIntersectTRB		= (kIntersectTop|kIntersectRight|kIntersectBottom);

static YPointCount Intersect( const RRealVectorRect& rc, YRealCoordinate x, RRealPoint* pPoly, int& fEdges )
{
	YPointCount	n	= 0;
	fEdges	= 0;
	n += IntersectVertical( rc.m_BottomLeft, rc.m_TopLeft, x, pPoly[n], fEdges, kIntersectLeft );
	n += IntersectVertical( rc.m_TopLeft, rc.m_TopRight, x, pPoly[n], fEdges, kIntersectTop );
	n += IntersectVertical( rc.m_TopRight, rc.m_BottomRight, x, pPoly[n], fEdges, kIntersectRight );
	n += IntersectVertical( rc.m_BottomRight, rc.m_BottomLeft, x, pPoly[n], fEdges, kIntersectBottom );

	//	Assert changed because there could be a case where the vector rect only intersects the
	//	vertical at a end point (one edge)
	TpsAssert( n>0, "MUST initially intersect at least two edges!" );
	if( n>2 )
		{
		//
		// intersected too many edges.  The only way this should be possible is
		// if (due to floating point inaccuracy followed by rounding) we have
		// duplicate points.  We now attempt to remove the duplicate...
		switch( fEdges )
			{
			case kIntersectLTR:
				//
				// intersects left, top, and right.  the rectangle must be
				// very close to horizontal, with the top intersection caused
				// by roundoff error...
				pPoly[1] = pPoly[2];
				break;
			case kIntersectLTB:
				//
				// intersects left, top, and bottom.  the rectangle must be
				// very close to vertical, with the left intersection caused
				// by roundoff error...
				pPoly[0] = pPoly[1];
				pPoly[1] = pPoly[2];
				break;
			case kIntersectLRB:
				//
				// intersects left, right, and bottom.  the rectangle must be
				// very close to horizontal, with the bottom intersection caused
				// by roundoff error...
				break;
			case kIntersectTRB:
				//
				// intersects top, right, and bottom.  the rectangle must be
				// very close to vertical, with the right intersection caused
				// by roundoff error...
				pPoly[1] = pPoly[2];
				break;
			default:
				TpsAssertAlways( "Intersected (what should have been) an impossible combination of edges" );
				break;
			}
		n = 2;
		}

	//
	// Order the points so that the first has a smaller Y coordinate...
	if( pPoly[0].m_y > pPoly[1].m_y )
		{
		RRealPoint tmp	= pPoly[0];
		pPoly[0]			= pPoly[1];
		pPoly[1]			= tmp;
		}
	return n;
}

// ****************************************************************************
//
//  RGradientRampPoint::operator==( RGradientRampPoint& rhs )
//
//  @MFunc Compares this ramp point to the given ramp point
//
//  @RDesc TRUE if they are the same else FALSE
//
BOOLEAN RGradientRampPoint::operator==(
			const RGradientRampPoint&	rhs )	// @Parm reference to the ramp point to compare with
			const
{
	return BOOLEAN( m_color == rhs.m_color &&
						 AreFloatsEqual(m_pctMidPoint,rhs.m_pctMidPoint) &&
						 AreFloatsEqual(m_pctRampPoint,rhs.m_pctRampPoint) );
}

// ****************************************************************************
//
//  RGradientRampPoint::Write( RArchive& archive )
//
//  @MFunc Writes this ramp points data to the specified archive
//
//  @RDesc nothing
//
void RGradientRampPoint::Write( RArchive& archive ) const
	{
	archive << m_color;
	archive << m_pctMidPoint;
	archive << m_pctRampPoint;
	}

// ****************************************************************************
//
//  RGradientRampPoint::Read( RArchive& archive )
//
//  @MFunc Reads this ramp points data from the specified archive
//
//  @RDesc nothing
//
void RGradientRampPoint::Read( RArchive& archive )
	{
	archive >> m_color;
	archive >> m_pctMidPoint;
	archive >> m_pctRampPoint;
	}

//****************************************************************************
//
// RGradientRamp::RGradientRamp()
//
// @MFunc default ctor
//
// @RDesc nothing
//
RGradientRamp::RGradientRamp() :
	m_ePreferredType( kUndefined )
{
}


//****************************************************************************
//
// RGradientRamp::RGradientRamp()
//
// @MFunc ctor with static array
//
// @RDesc nothing
//
RGradientRamp::RGradientRamp( EGradientType eType, RGradientRampPoint* pRampPoints, int nRampPoints ) :
	m_ePreferredType( eType )
{
	while( nRampPoints-- )
		{
		Add( *pRampPoints );
		++pRampPoints;
		}
}

// ****************************************************************************
//
//  RGradientRamp::operator==( RGradientRamp& rhs )
//
//  @MFunc Compares this ramp to the given ramp
//
//  @RDesc TRUE if they are the same else FALSE
//
BOOLEAN RGradientRamp::operator==(
			const RGradientRamp&	rhs )	// @Parm reference to the ramp to compare with
			const
{
	if( m_ePreferredType != rhs.m_ePreferredType )
		return FALSE;

	if( m_arRampPoints.Count() != rhs.m_arRampPoints.Count() )
		return FALSE;
	YGradientRampPointArrayIterator	itr0	= m_arRampPoints.Start();
	YGradientRampPointArrayIterator	itr1	= rhs.m_arRampPoints.Start();
	while( itr0 != m_arRampPoints.End() )
		{
		if( (*itr0) != (*itr1) )
			return FALSE;
		++itr0;
		++itr1;
		}
	return TRUE;
}

// ****************************************************************************
//
//  RGradientRamp::Write( RArchive& archive )
//
//  @MFunc Writes this ramps data to the specified archive
//
//  @RDesc nothing
//
void RGradientRamp::Write( RArchive& archive ) const
	{
	archive << static_cast<uWORD>( m_ePreferredType );

	archive << static_cast<uLONG>( m_arRampPoints.Count( ) );

	YGradientRampPointArrayIterator iterator = m_arRampPoints.Start( );
	for( ; iterator != m_arRampPoints.End( ); ++iterator )
		( *iterator ).Write( archive );
	}

// ****************************************************************************
//
//  RGradientRamp::Read( RArchive& archive )
//
//  @MFunc Reads this ramps data from the specified archive
//
//  @RDesc nothing
//
void RGradientRamp::Read( RArchive& archive )
	{
	uWORD uwTemp;

	archive >> uwTemp;
	m_ePreferredType = static_cast<EGradientType>( uwTemp );

	uLONG numPoints;
	archive >> numPoints;

	m_arRampPoints.Empty( );

	for( uLONG i = 0; i < numPoints; ++i )
		{
		RGradientRampPoint rampPoint;
		rampPoint.Read( archive );
		m_arRampPoints.InsertAtEnd( rampPoint );
		}
	}

//****************************************************************************
//
// void RGradientRamp::Add( RGradientRampPoint& ramppt )
//
// @MFunc Add a ramp point to the gradient ramp
//
// @RDesc nothing
//
void RGradientRamp::Add( RGradientRampPoint& ramppt )
{
	//
	// We maintain the ramp color stops in order...
	YGradientRampPointArrayIterator	itr	= m_arRampPoints.Start();
	while( itr != m_arRampPoints.End() )
		{
		if( (*itr).m_pctRampPoint >= ramppt.m_pctRampPoint )
			break;
		++itr;
		}
	TpsAssert( itr==m_arRampPoints.End() || (*itr).m_pctRampPoint!=ramppt.m_pctRampPoint, "overlapping color stops!" );
	//
	// itr is the location to insert at...
	m_arRampPoints.InsertAt( itr, ramppt );
}


//****************************************************************************
//
// RColor RGradientRamp::GetColor( YPercentage pctRampPos, YPercentage pctPrecision ) const
//
// @MFunc Get the color on this ramp at the given percent position and percent precision
//
// @RDesc the color at the given percent location
//
RSolidColor RGradientRamp::GetColor(
			YPercentage pctRampPos,				// @Parm the percent position (0.0-1.0) in the ramp to get the color of
			YPercentage	pctPrecision ) const	// @Parm the percent precision
{
	//
	// Find the ramp points bounding the requested ramp position...
	YGradientRampPointArrayIterator	RampFirst	= m_arRampPoints.Start();
	YGradientRampPointArrayIterator	RampLast		= m_arRampPoints.End();
	YGradientRampPointArrayIterator	RampStart	= RampFirst-1;
	YGradientRampPointArrayIterator	RampEnd		= RampFirst;

	TpsAssert( RampFirst != RampLast, "Gradient ramp with no colors!" );
//	TpsAssert( RampFirst != (RampLast-1), "Gradient ramp with only one color!" );

	while( RampEnd < RampLast && pctRampPos >= (*RampEnd).m_pctRampPoint )
		{
		//
		// It could be a dummy at the end.  This is true if it's ramp position is 100%
		// and the previous's midpoint is 100%...
		if( AreFloatsEqual((*RampEnd).m_pctRampPoint,pctRampPos) && RampStart>=RampFirst )
			{
			//
			// the requested point falls exactly at the ramp point.  If the
			// midpoint of the segment that this ends at is at 100%, then
			// we're done.  We accept that
			if( AreFloatsEqual((*RampStart).m_pctMidPoint,YPercentage(1.0)) )
				break;
			}
		// 
		RampStart = RampEnd;
		++RampEnd;
		}

	if( RampStart < RampFirst )
		{
		//
		// the requested color point is before the first ramp point, so
		// the color is that of the first ramp point...
		return (*RampEnd).m_color;
		}
	if( RampEnd == RampLast )
		{
		//
		// the requested color is after the last ramp point, so
		// the color is that of the last ramp point...
		return (*RampStart).m_color;
		}

	//
	// The requested ramp position is somewhere between RampStart and RampEnd...
	// First, determine exactly where between the endpoints of the subramp the
	// requested ramp position is...
	YPercentage	pctSubRampPos	= (pctRampPos-(*RampStart).m_pctRampPoint) /
											((*RampEnd).m_pctRampPoint - (*RampStart).m_pctRampPoint);
	//
	// dSubRampPos is the percent position of the requested ramp position in the subramp.
	// Now we need to determine the mix...
	YPercentage	pctMix	= MixPercentages( pctSubRampPos, (*RampStart).m_pctMidPoint, pctPrecision );
		
	//
	// Now that we have the mix ratio, all we need do is return the
	// mixed color values...
	return RSolidColor( (*RampStart).m_color, (*RampEnd).m_color, pctMix );
}

//****************************************************************************
//
// void RGradientRamp::RemoveAllPts( )
//
// @MFunc Remove all ramp points from the gradient ramp
//
// @RDesc nothing
//
void RGradientRamp::RemoveAllPts( )
{
	m_arRampPoints.Empty() ;
}

//****************************************************************************
//
// RGradientRamp::operator=( const RGradientRamp& rhs )
//
// @MFunc Copy the data from the given RGradientRamp into this one
//
// @RDesc const reference to this RGradientRamp
//
const RGradientRamp& RGradientRamp::operator=( const RGradientRamp& rhs )
{
	m_ePreferredType = rhs.m_ePreferredType ;
	
	m_arRampPoints.Empty() ;
	YGradientRampPointArrayIterator iterator = rhs.m_arRampPoints.Start( );
	
	for( ; iterator != rhs.m_arRampPoints.End( ); ++iterator )
	{
		m_arRampPoints.InsertAtEnd( *iterator ) ;
	}

	return *this ;
}

//****************************************************************************
//
// RGradientFill::RGradientFill()
//
// @MFunc Construct a gradient fill object
//
// @Syntax RGradientFill()
// @Syntax RGradientFill( rSrcFill )
// @Syntax RGradientFill( rSrcParams )
// @Syntax RGradientFill( eType, pRamp, ptOrigin, angVector, rdlenVector, transform, pctposHilite, angHilite )
//
// @Parm const RGradientFill&	| rSrcFill		| Reference to an RGradientFill to copy data from
// @Parm EGradientType			| eType			| The type for this gradient
// @Parm const RGradientRamp*	| pRamp			| Pointer to the color ramp for this gradient
// @Parm const RIntPoint&		| ptOrigin		| Origin of the gradient vector
// @Parm YAngle					| angVector		| Angle of the gradient vector
// @Parm YRealDimension			| rdlenVector	| Length of the gradient vector
// @Parm const R2dTransform&	| transform		| Transformation matrix for this gradient
// @Parm YPercentage				| pctposHilite	| Position of the gradient hilite as a percentage of the gradient vector length
// @Parm YAngle					| angHilite		| Angle of the gradient hilite vector
//
RGradientFill::RGradientFill( ) :
		m_eGradientType			( kUndefined	),
		m_pRamp						( NULL			),
		m_ptGradientOrigin		( 0, 0			),
		m_angGradientVector		( 0.0				),
		m_rdlenGradientVector	( 0.0				),
		m_pctposHilite				( 0.0				),
		m_angHilite					( 0.0				)
{
	//
	// transformation matrix initializes to identity by default...
}

// @Parm RGradientParams&		| params			| Paramaters that define a gradient
//
RGradientFill::RGradientFill( 
			const RGradientParams& params ) :
		m_pRamp						( NULL						),
		m_eGradientType			( params.m_eType			),
		m_ptGradientOrigin		( params.m_ptOrigin		),
		m_angGradientVector		( params.m_angVector		),
		m_rdlenGradientVector	( params.m_rdlenVector	),
		m_pctposHilite				( params.m_pctposHilite	),
		m_angHilite					( params.m_angHilite		)
{
	//
	// transformation matrix initializes to identity by default...

	//	If we will be drawing this gradient then make a ramp for it.
	//	Define default ramp points.
	static RGradientRampPoint	RampPoints[] =
		{
		//						  color,						midpoint,				ramppoint
		//--------------------------------------------------------------------------
		RGradientRampPoint( RSolidColor(kBlue),	YPercentage(0.00),	YPercentage(0.0000) ),
		RGradientRampPoint( RSolidColor(kRed),		YPercentage(0.50),	YPercentage(0.1666) ),
		RGradientRampPoint( RSolidColor(kGreen),	YPercentage(0.50),	YPercentage(0.5000) ),
		RGradientRampPoint( RSolidColor(kBlue),	YPercentage(1.00),	YPercentage(0.8666) ),
		RGradientRampPoint( RSolidColor(kRed),		YPercentage(0.00),	YPercentage(1.0000) )
		};
		
		//	Use default ramp points to create a ramp.
		m_pRamp = new RGradientRamp( m_eGradientType, RampPoints, sizeof(RampPoints)/sizeof(RampPoints[0]) );
}

RGradientFill::RGradientFill(
	const RGradientFill &rhs ) :	// @ Parm reference to an RGradientFill to copy data from
		m_eGradientType			( rhs.m_eGradientType			),
		m_pRamp						( rhs.m_pRamp						),
		m_ptGradientOrigin		( rhs.m_ptGradientOrigin		),
		m_angGradientVector		( rhs.m_angGradientVector		),
		m_rdlenGradientVector	( rhs.m_rdlenGradientVector	),
		m_BaseTransform			( rhs.m_BaseTransform			),
		m_Transform					( rhs.m_Transform					),
		m_pctposHilite				( rhs.m_pctposHilite				),
		m_angHilite					( rhs.m_angHilite					)
{
}

RGradientFill::RGradientFill(
	EGradientType			eType,			// @ Parm The type for this gradient
	const RGradientRamp* pRamp,			// @ Parm Pointer to the color ramp for this gradient
	const RIntPoint&		ptOrigin,		// @ Parm Origin of the gradient vector
	YAngle					angVector,		// @ Parm Angle of the gradient vector
	YRealDimension			rdlenVector,	// @ Parm Length of the gradient vector
	const R2dTransform&	transform,		// @ Parm Transformation matrix for this gradient
	YPercentage				pctposHilite,	// @ Parm Position of the gradient hilite as a percentage of the gradient vector length
	YAngle					angHilite ) :	// @ Parm Angle of the gradient hilite vector
		m_eGradientType			( eType			),
		m_pRamp						( pRamp			),
		m_ptGradientOrigin		( ptOrigin		),
		m_angGradientVector		( angVector		),
		m_rdlenGradientVector	( rdlenVector	),
		m_BaseTransform			( transform		),
		m_Transform					( transform		),
		m_pctposHilite				( pctposHilite	),
		m_angHilite					( angHilite		)
{
}

//****************************************************************************
//
// RGradientFill::~RGradientFill( )
//
// @MFunc Destroy this RGradientFill's data
//
// @RDesc nothing
//
RGradientFill::~RGradientFill( )
{
}

//****************************************************************************
//
// RGradientFill::operator=( const RGradientFill& rhs )
//
// @MFunc Copy the data from the given RGradientFill into this one
//
// @RDesc const reference to this RGradientFill
//
const RGradientFill& RGradientFill::operator=( const RGradientFill& rhs )	// @Parm reference to the RGradientFill to copy data from
{
	if (this != &rhs)
		{
		m_eGradientType			= rhs.m_eGradientType;
		m_pRamp						= rhs.m_pRamp;
		m_ptGradientOrigin		= rhs.m_ptGradientOrigin;
		m_angGradientVector		= rhs.m_angGradientVector;
		m_rdlenGradientVector	= rhs.m_rdlenGradientVector;
		m_BaseTransform			= rhs.m_BaseTransform;
		m_Transform					= rhs.m_Transform;
		m_pctposHilite				= rhs.m_pctposHilite;
		m_angHilite					= rhs.m_angHilite;
		}
	return *this;
}

// ****************************************************************************
//
//  RGradientFill::operator==( RGradientFill& rhs )
//
//  @MFunc Compares this ramp point to the given ramp point
//
//  @RDesc TRUE if they are the same else FALSE
//
BOOLEAN RGradientFill::operator==(
			const RGradientFill&	rhs )	// @Parm reference to the gradient to compare with
			const
{
	return BOOLEAN((m_eGradientType == rhs.m_eGradientType) &&
						(m_ptGradientOrigin == rhs.m_ptGradientOrigin) &&
						(AreFloatsEqual(m_angGradientVector,rhs.m_angGradientVector)) &&
						(AreFloatsEqual(m_rdlenGradientVector,rhs.m_rdlenGradientVector)) &&
						(AreFloatsEqual(m_pctposHilite,rhs.m_pctposHilite)) &&
						(AreFloatsEqual(m_angHilite,rhs.m_angHilite)) &&
						(m_Transform==rhs.m_Transform) &&
						((*m_pRamp) == (*rhs.m_pRamp)) );
}

// ****************************************************************************
//
//  RGradientFill::Write( RArchive& archive )
//
//  @MFunc Writes this gradients data to the specified archive
//
//  @RDesc nothing
//
void RGradientFill::Write( RArchive& archive ) const
	{
	archive << static_cast<uWORD>( m_eGradientType );
	archive << m_ptGradientOrigin;
	archive << m_angGradientVector;
	archive << m_rdlenGradientVector;
	archive << m_BaseTransform;
	archive << m_Transform;
	archive << m_pctposHilite;
	archive << m_angHilite;
	}

template<class pointin, class pointout>
YPointCount CreateEllipsePolyBezier( const RVectorRect<pointin>& rc, RPoint<pointout>* pptBez )
	{
	//
	// The following magic number (kOffsetFactor) was arrived at empirically.  It provides for
	// the best possible match between the polybezier output and the Windows GDI Ellipse() output
	// using a normal rectangle...
	const	YRealDimension	kScaleFactor( 554.0/1000.0 );
	const RRealSize		kControlPointScale( kScaleFactor, kScaleFactor );
	//
	// Intersections with the rectangle...
	midpoint( pptBez[0],	rc.m_BottomLeft,	rc.m_TopLeft );
	midpoint( pptBez[3],	rc.m_TopLeft,		rc.m_TopRight );
	midpoint( pptBez[6],	rc.m_TopRight,		rc.m_BottomRight );
	midpoint( pptBez[9],	rc.m_BottomRight,	rc.m_BottomLeft );
	pptBez[12]	= pptBez[0];
	//
	// Control points
	pptBez[ 1]	= pptBez[ 0] + (rc.m_TopLeft-pptBez[ 0]).Scale( kControlPointScale );
	pptBez[ 2]	= pptBez[ 3] + (rc.m_TopLeft-pptBez[ 3]).Scale( kControlPointScale );
	pptBez[ 4]	= pptBez[ 3] + (rc.m_TopRight-pptBez[ 3]).Scale( kControlPointScale );
	pptBez[ 5]	= pptBez[ 6] + (rc.m_TopRight-pptBez[ 6]).Scale( kControlPointScale );
	pptBez[ 7]	= pptBez[ 6] + (rc.m_BottomRight-pptBez[ 6]).Scale( kControlPointScale );
	pptBez[ 8]	= pptBez[ 9] + (rc.m_BottomRight-pptBez[ 9]).Scale( kControlPointScale );
	pptBez[10]	= pptBez[ 9] + (rc.m_BottomLeft-pptBez[ 9]).Scale( kControlPointScale );
	pptBez[11]	= pptBez[12] + (rc.m_BottomLeft-pptBez[12]).Scale( kControlPointScale );

	return 13;
	}

// ****************************************************************************
//
//  RGradientFill::Read( RArchive& archive )
//
//  @MFunc Reads this gradients data from the specified archive
//
//  @RDesc nothing
//
void RGradientFill::Read( RArchive& archive )
	{
	uWORD uwTemp;

	archive >> uwTemp;
	m_eGradientType = static_cast<EGradientType>( uwTemp );

	archive >> m_ptGradientOrigin;
	archive >> m_angGradientVector;
	archive >> m_rdlenGradientVector;
	archive >> m_BaseTransform;
	archive >> m_Transform;
	archive >> m_pctposHilite;
	archive >> m_angHilite;
	}

inline void FillGradientPolygon( RDrawingSurface& ds, RClipPolygon* pClip, const RRealPoint* pPoints, YPointCount nPoints )
{
	YPointCount*	pcntOut;
	RIntPoint*		pptOut;
	uLONG				nBufSize;
	YPointCount		nOutPoints;
	YPolygonCount	nOutPolys	= 0;
	uBYTE*			pBuffer		= NULL;

	try
		{
		if( !pClip )
			{
			nBufSize				= nPoints * sizeof(RIntPoint);
			pBuffer				= ::GetGlobalBuffer( nBufSize );
			pptOut				= reinterpret_cast<RIntPoint*>( pBuffer );
			nOutPoints			= nPoints;
			RIntPoint*	ppt	= pptOut;
			while( nPoints-- )
				{
				*ppt	= RIntPoint(*pPoints);
				++ppt;
				++pPoints;
				}
			nOutPolys	= 1;
			pcntOut		= &nOutPoints;
			}
		else
			{
#ifdef TPSDEBUG
			int	fdump = 0;
againfordump:
			clipdebugindent = ( fdump )? 0 : -1;
			try{
#endif
			RClipPolygon		subject( pPoints, nPoints );
			RPolygonClipper	clipper( *pClip, subject );
#ifdef TPSDEBUG
			if( fdump )
				{
				STARTFUNC( ("Data Dump\n") );
				STARTFUNC( ("Clip Polygon\n") );
				if( pClip )
					pClip->Dump();
				ENDFUNC( ("\n") );
				STARTFUNC( ("Subject Polygon\n") );
				subject.Dump();
				ENDFUNC( ("\n") );
				ENDFUNC( ("\n") );
		//		clipdebugindent = -1;
				}
#endif
			clipper.GeneratePolygons( RPolygonClipper::kLogicalAnd, nOutPolys, nOutPoints );
			if( nOutPolys > 0 )
				{
				//
				// Get a global buffer to hold the clipped points...
				nBufSize		= nOutPolys*sizeof(YPointCount) + nOutPoints*sizeof(RIntPoint);
				pBuffer		= ::GetGlobalBuffer( nBufSize );
				pcntOut		= reinterpret_cast<YPointCount*>( pBuffer );
				pptOut		= reinterpret_cast<RIntPoint*>( pcntOut+nOutPolys );
				nOutPolys	= clipper.StorePolygons( pptOut, pcntOut );
				}
#ifdef TPSDEBUG
			clipdebugindent = -1;
#ifdef DEBUG_SINGLE_STEP
			GdiFlush();
#endif
			}catch(...){
				if( !fdump )
					{
					fdump = 1;
					if( pBuffer )
						::ReleaseGlobalBuffer( pBuffer );
					pBuffer	= NULL;
					goto againfordump;
					}
				throw;
			}
#ifdef DEBUG_SINGLE_STEP
			if( !fdump && !fskipdump )
				{
				CONTINUEFUNC( ("Clipped poly rendered, press ctrl to skip dump, shift to dump data\n") );
				while( ::GetAsyncKeyState(VK_CONTROL)>=0 )
					{
					if( ::GetAsyncKeyState(VK_SHIFT)<0 )
						{
						fdump = 1;
						while( ::GetAsyncKeyState(VK_SHIFT)<0 );
						goto againfordump;
						}
					}
				while( ::GetAsyncKeyState(VK_CONTROL)<0 );
				}
#endif
#endif
			}
		}
	catch( ... )
		{
		if( pBuffer )
			::ReleaseGlobalBuffer( pBuffer );
		throw;
		}
	if( nOutPolys>0 )
		ds.FillPolyPolygon( pptOut, pcntOut, nOutPolys );
	if( pBuffer )
		::ReleaseGlobalBuffer( pBuffer );
	}

inline void FillGradientRectangle( RDrawingSurface& ds, RClipPolygon* pClip, const RIntRect& rc )
{
	if( !pClip )
		ds.FillRectangle( rc );
	else
		{
		RRealPoint	ptTmp[4];
		CreateRectPoly( rc, &ptTmp[ 0 ] );
		FillGradientPolygon( ds, pClip, ptTmp, 4 );
		}
}

inline void FillGradientRectangle( RDrawingSurface& ds, RClipPolygon* pClip, const RRealVectorRect& rc, const R2dTransform& xform )
{
	if( !pClip )
		ds.FillVectorRect( rc, xform );
	else
		{
		RRealPoint	ptTmp[4];
		CreateRectPoly( rc, xform, &ptTmp[ 0 ] );
		FillGradientPolygon( ds, pClip, ptTmp, 4 );
		}
}

inline void FillGradientEllipse( RDrawingSurface& ds, RClipPolygon* pClip, const RRealRect& rc, const R2dTransform& xform )
{
	if( !pClip )
		ds.FillEllipse( rc, xform );
	else
		{
		RRealPoint			ptBez[13];
		RRealVectorRect	vrc( rc );
		YPointCount			nPoints	= CreateEllipsePolyBezier( vrc * xform, &ptBez[ 0 ] );
		YPointCount			numExpanded;
		RRealPoint*			pptExpanded	= ds.ExpandPolyBezier( *ptBez, ptBez + 1, 12, numExpanded );
		FillGradientPolygon( ds, pClip, pptExpanded, numExpanded );
		::ReleaseGlobalBuffer( (uBYTE*)pptExpanded );
		}
}


inline void DrawSegment( RDrawingSurface& ds, RClipPolygon* pClip, const RColor& clr, const RRealPoint& ptCenter, const RRealPoint& pt0, const RRealPoint& pt1 )
{
	RRealPoint	Poly[3];
	Poly[0]	= ptCenter;
	Poly[1]	= pt0;
	Poly[2]	= pt1;
//	RColor	oldColor	= ds.GetFillColor();
	ds.SetFillColor( clr );
	FillGradientPolygon( ds, pClip, Poly, YPointCount(3) );
}

inline YPercentage ComputeAngularPercentage( YRealDimension xDelta, YRealDimension yDelta, YAngle angZero )
{
	YAngle	dAng	= YAngle(atan2(yDelta,xDelta)) - angZero;
	while( dAng < 0.0 ) dAng += k2PI;
	return YPercentage( dAng / k2PI );
}

inline YPercentage ComputeLinearPercentage( YRealDimension nCurrent, YRealDimension nLength )
{
	YPercentage	pct	= nCurrent / nLength;
	pct	= ::Min( pct, YPercentage(1.0) );
	return ::Max( pct, YPercentage(0.0) );
}

//****************************************************************************
//
// RGradientFill::GetAdjustedAngle( YAngle angGradient, const R2dTransform& xform )
//
// @MFunc Return the angle that is to be applied to the gradient
//
// @RDesc nothing
//
YAngle GetAdjustedAngle( YAngle angGradient, const R2dTransform& xform )
{
	R2dTransform	x;
	x.PreRotateAboutOrigin( angGradient );
	RRealPoint	ptOrg( 0, 0 );
	RRealPoint	ptVec( RRealPoint(1000, 0)*x );

	YAngle			ang;
	YRealDimension	xScale;
	YRealDimension	yScale;
	BOOLEAN			fFlipped = xform.Decompose(ang,xScale,yScale);
	xScale = -xScale;	//	force an X Scale
	if ( fFlipped )
		yScale = -yScale;
	R2dTransform	s;
	s.PreScale( (xScale<0)?-1.0:1.0, (yScale<0)?-1.0:1.0 );
	ptVec	*= s;
	angGradient	= YAngle( atan2(ptVec.m_y,ptVec.m_x) );
	return angGradient;
}

//****************************************************************************
//
// RGradientFill::SetClipPolygonPoints( RClipPolygon& clipPoly, const RIntPoint* pptClip, const YPointCount* pcntClip, YPolygonCount nClip )
//
// @MFunc Fills in the clipping polygon to be used for rendering the gradient
//
// @RDesc Nothing
//
void SetClipPolygonPoints( RClipPolygon&			clipPoly,		// @Parm polygon to append points into
											 const RIntPoint*		pptClip,			// @Parm pointer to clip polypolygon points
											 const YPointCount*	pcntClip,		// @Parm pointer to clip polypolygon counts
											 YPolygonCount	nClip)			// @Parm number of sub-polygons in the clip polypolygon
{
#define	REDUCE_POLYPOINTS	1
#ifdef	REDUCE_POLYPOINTS
	//
	//	only do something if we have points or a point count
	if ( pptClip && pcntClip )
	{
		int				i, j;
		YPolygonCount	polyCount			= 0;
		int				nPoints				= 0;
		int				nClipPoints			= 0;
		RIntPoint*		pPoints				= NULL;
		YPointCount*	pSubPolyPtCount	= NULL;
		//
		//	Counter number of points and allocate the memory
		for ( i = 0; i < nClip; ++i )
			nClipPoints += pcntClip[i];
		pPoints				= reinterpret_cast<RIntPoint*>( ::GetGlobalBuffer( nClipPoints * sizeof(RIntPoint)) );
		pSubPolyPtCount	= new YPointCount[ nClip ];
		//
		//	Copy the points and reduce if possible (reduction is essential to making
		//		the clipper be able to work at its optimal speed and with the greatest accuracy).
		for ( i = 0; i < nClip; ++i )
		{
			//	Start with the first point in this sub polygon
			pSubPolyPtCount[ polyCount ] = 1;
			//
			pPoints[ nPoints ] = *pptClip;
			RIntPoint*	pptPrev	= &( pPoints[nPoints] );
			++nPoints;
			//	Remember current point (as previous) and advance to second point
			++pptClip;
			for ( j = 1; j < pcntClip[i]-1; ++j )
			{
				//
				//	See if the point should be kept or tossed based on the previous and next
				BOOLEAN				fRedundant	= FALSE;
				const RIntPoint*	pptNext		= pptClip + 1;
				//
				//	If it is the same as the previous point - redundant...
				if ( *pptPrev == *pptClip )
					fRedundant = TRUE;
				//	If points on the same vertical - redundant
				else if ( (pptPrev->m_y==pptClip->m_y) && (pptClip->m_y == pptNext->m_y) )
					fRedundant = TRUE;
				//	If points on the same horizontal - redundant
				else if ( (pptPrev->m_x==pptClip->m_x) && (pptClip->m_x == pptNext->m_x) )
					fRedundant = TRUE;
				//	If points are parallel - redundant
				else if ( AreParallel( *pptClip, *pptPrev, *pptNext ) )
					fRedundant = TRUE;

				//
				//	If point is not redundant, copy it over
				if ( !fRedundant )
				{
					pPoints[ nPoints++ ] = *pptClip;
					++pptPrev;
					++pSubPolyPtCount[ polyCount ];
				}

				//	Advance to next point
				++pptClip;
			}

			//	Always put the last point into the polygon
			pPoints[ nPoints++ ] = *pptClip;
			++pptClip;
			++pSubPolyPtCount[ polyCount ];

			//
			//	Make sure we have enough points to form a polygon,
			//		If not, remove the points from the count,
			//		If so, bump the polyCount
			if ( pSubPolyPtCount[ polyCount ] < 3 )
				nPoints	-= pSubPolyPtCount[ polyCount ];
			else
				++polyCount;
		}

		//
		//	Append points	- if any exist
		if ( polyCount > 0 )
			clipPoly.AppendPolyPolygon( pPoints, pSubPolyPtCount, polyCount );

		//
		//	Free memory
		::ReleaseGlobalBuffer( reinterpret_cast<uBYTE*>(pPoints) );
		delete[] pSubPolyPtCount;

	#ifdef	_WINDOWS
	#ifdef	TPSDEBUG
		if ( nPoints < nClipPoints )
			OutputDebugString( "Polygon reduced!\n" );
	#endif	TPSDEBUG
	#endif	//	_WINDOWS
	}
#else 	// REDUCE_POLYPOINTS
	clipPoly.AppendPolyPolygon(pptClip, pcntClip, nClip ); 
#endif	//	REDUCE_POLYPOINTS

}

//****************************************************************************
//
// RGradientFill::DrawGradient( RDrawingSurface& ds, const RIntRect& rcBounds, const R2dTransform& transform, YPercentage pctPrecision )
//
// @MFunc Fill the given rectangle in the given drawing surface with this gradient
//
// @RDesc nothing
//
void RGradientFill::DrawGradient( RDrawingSurface&		ds,				// @Parm reference to the drawing surface to draw into
											 const RIntRect&		rcBounds,		// @Parm reference to the required area in the surface
											 const R2dTransform&	transform,		// @Parm reference to additional transform to be applied to gradient
											 YPercentage			pctPrecision )	// @Parm percentage of color bands that should be drawn
											 const
{
	DrawGradient( ds, rcBounds, NULL, NULL, 0, transform, pctPrecision );
}

void RGradientFill::DrawGradient( RDrawingSurface&		ds,				// @Parm reference to the drawing surface to draw into
											 const RIntRect&		rcBounds,		// @Parm reference to the required area in the surface
											 const RIntPoint*		pptClip,			// @Parm pointer to clip polypolygon points
											 const YPointCount*	pcntClip,		// @Parm pointer to clip polypolygon counts
											 YPolygonCount			nClip,			// @Parm number of sub-polygons in the clip polypolygon
											 const R2dTransform&	transform,		// @Parm reference to additional transform to be applied to gradient
											 YPercentage			pctPrecision )	// @Parm percentage of color bands that should be drawn
											 const
{
	// Save the DS state
	RDrawingSurfaceState	curState( &ds );

	// Intersect the bounds rect with the clip region
	ds.IntersectClipRect( rcBounds );

#ifdef TPSDEBUG
#ifdef DEBUG_SINGLE_STEP
	fskipdump = TRUE;
	while( ::GetAsyncKeyState(VK_CONTROL)>=0 )
		{
		if( ::GetAsyncKeyState(VK_SHIFT)<0 )
			{
			fskipdump = FALSE;
			while( ::GetAsyncKeyState(VK_SHIFT)<0 );
			break;
			}
		}
	while( ::GetAsyncKeyState(VK_CONTROL)<0 );
#endif
#endif
	//
	// setup the clip polygon
	RClipPolygon	clipPolygon;
	SetClipPolygonPoints( clipPolygon, pptClip, pcntClip, nClip );
	RClipPolygon*	pClipPolygon	= (pptClip? &clipPolygon : NULL);
#ifdef TPSDEBUG
#ifdef DEBUG_SINGLE_STEP
	if( !fskipdump && pClipPolygon )
		{
		STARTFUNC( ("Clip polygon\n") );
		pClipPolygon->Dump();
		ENDFUNC( ("\n") );
		}
#endif
#endif
	//
	// a gradient can be:
	//		Fixed (start point, end point, everything beyond the edges is the color of the edges)
	//		Linear (not fixed, but it is linear in a given direction)
	//		Radial (not supported yet, but the colors cycle through 360 degrees)
	//		Burst (circular, with start at center, end at radius)

	//
	// We need to remember the old drawingsurface color!
	RFillColorRestorer	restorer( ds );

	//
	// Remember: FillRectangle fills the INTERIOR of the rectangle, so we need to
	//           inflate the bounds in order to include all of it...
	RIntRect	rcAdjustedBounds( rcBounds );
	rcAdjustedBounds.Inflate( RIntSize(1,1) );
	//
	// Build the combined transformation
	R2dTransform	xformGradient	= m_Transform * transform;
	//
	// Get the gradient vector...
	R2dTransform	xformRotate;
	xformRotate.PreRotateAboutOrigin( GetGradientAngle() );
	RRealSize		szVector( GetGradientLength(), 0.0 );
	RRealPoint		ptOrigin( GetGradientOrigin() );
	RRealPoint		ptVector = ptOrigin + (szVector*xformRotate);
	//
	// Transform the vector...
	ptOrigin	*= xformGradient;
	ptVector	*= xformGradient;
	//
	// Get the new length...
	YRealDimension	rdlenGradient	= ptVector.Distance( ptOrigin );

	//
	// Compute the smallest band size.
	RRealSize		deviceDPI( ds.GetDPI() );
	YRealDimension	nDeviceDPI( YRealDimension(::Max(deviceDPI.m_dx,deviceDPI.m_dy)) );
	YRealDimension	nMaxGradientDPI( YRealDimension(::GetMaxGradientDPI()) );
	YRealDimension	nMinimumBand( YRealDimension((nDeviceDPI>nMaxGradientDPI)? nDeviceDPI/nMaxGradientDPI : 1.0) );
	//
	// Draw the gradient colors into the bitmap
	switch( m_eGradientType )
		{
		case kLinear:
			{
			//
			// Gradient is a colored band from m_ptGradientOrigin that is m_rdlenGradientVector
			// units long, and heading in the m_angGradientVector direction, with all points prior
			// to m_StartPoint being the initial pure color, and all points past m_EndPoint being
			// the final pure color...

			//
			// Compute the implied hilite vector (untransformed gradient
			// vector rotated 90 degrees)
			YAngle	angTmp( GetAdjustedAngle(GetGradientAngle(),m_Transform) - (kPI/2.0) );
			R2dTransform	xformTmp;
			xformTmp.PreRotateAboutOrigin( angTmp );
			RRealSize		szHilite( szVector );
			RRealPoint		ptHilite( (RRealPoint(GetGradientOrigin()) + (szHilite*xformTmp)) * xformGradient );

			//
			// Get the resultant vector angles...
			szHilite	= ptHilite - ptOrigin;
			YAngle	angHilite	= YAngle( atan2(szHilite.m_dy,szHilite.m_dx) );
			//
			// Create the intermediate transformation matrices...
			R2dTransform	RotateHiliteTo90;
			R2dTransform	Rotate90ToHilite;
			YAngle			angHiliteTo90( angHilite + (kPI/2.0) );
			RotateHiliteTo90.PreRotateAboutPoint( ptOrigin.m_x, ptOrigin.m_y, angHiliteTo90 );
			Rotate90ToHilite.PreRotateAboutPoint( ptOrigin.m_x, ptOrigin.m_y, -angHiliteTo90 );

			//
			// Rotate the bounding rect such that the color bands will be vertical...
			RRealVectorRect	rc( RRealVectorRect(RRealRect(rcAdjustedBounds)) * RotateHiliteTo90 );
			//
			// Rotate the vector endpoint...
			ptVector	*= RotateHiliteTo90;
			//
			// Get the gradient end boundaries...
			YRealCoordinate	xMinGrad	= ::Min( ptOrigin.m_x, ptVector.m_x );
			YRealCoordinate	xMaxGrad	= ::Max( ptOrigin.m_x, ptVector.m_x );
			YRealDimension		lenGradient	= (xMaxGrad-xMinGrad);
			//
			// Get the rectangle boundaries...
			YRealCoordinate	xMinEdge	= rc.m_TransformedBoundingRect.m_Left;
			YRealCoordinate	xMaxEdge	= rc.m_TransformedBoundingRect.m_Right;
			//
			// Do the simple fills if possible...
			if( xMinEdge > xMaxGrad )
				{
				//
				// Fill with start color and we're done...
				ds.SetFillColor( m_pRamp->GetColor(YPercentage(0.0),pctPrecision) );
				FillGradientRectangle( ds, pClipPolygon, rcBounds );
				return;
				}
			if( xMaxEdge < xMinGrad )
				{
				//
				// Fill with end color and we're done...
				ds.SetFillColor( m_pRamp->GetColor(YPercentage(1.0),pctPrecision) );
				FillGradientRectangle( ds, pClipPolygon, rcBounds );
				return;
				}

			//
			// Okay, it's not really simple...
			// We know that vertical lines between xMinEdge and xMaxEdge all intersect
			// the rectangle, but may or may not intersect the gradient vector.
			// Setup the initial edge intersections and color...
			RRealPoint		Poly[ 10 ];
			int				fPrevEdges;
			int				fPolyEdges	= 0;
			int				idxLeft	= (sizeof(Poly)/sizeof(Poly[0]))/2-1;
			YPointCount		nPoly		= Intersect( rc, xMinEdge, Poly+idxLeft, fPrevEdges );
			RColor			clr( m_pRamp->GetColor(ComputeLinearPercentage(xMinEdge-xMinGrad,lenGradient),pctPrecision) );
			//
			// Loop across the rectangle...
			RRealPoint		Prev[2];
			RRealPoint		Next[ 4 ];
			YRealDimension	nBandWidth( 0.0 );

			Next[0]	= Poly[idxLeft];
			Next[1]	= Poly[idxLeft+nPoly-1];
			while( (xMinEdge+=YRealCoordinate(1.0)) < xMaxEdge )
				{
				//
				// There will always be 2 intersection points, but we know that it's impossible to have
				// more than 4, so we make sure we don't crash.  Intersect asserts 2 points...
				int		fNextEdges;
				Prev[0]	= Next[0];
				Prev[1]	= Next[1];
				Intersect( rc, xMinEdge, Next, fNextEdges );
				nBandWidth	+= YRealDimension(1.0);
				//
				// Stuff the ramp intersections into the polygon...
				if( fNextEdges!=fPolyEdges || fPolyEdges!=fPrevEdges )
					{
					//
					// Next ramp is on different edges, or the previous edge was
					// a transition from one pair of edges to another.  either way,
					// we need to keep the current points and add the new ones...
					--idxLeft;
					nPoly	+= YPointCount(2);
					//
					// reset the edges...
					fPrevEdges	= fPolyEdges;
					fPolyEdges	= fNextEdges;
					}
				//
				// Stuff the new ramp intersection points into the polygon...
				Poly[idxLeft]				= Next[0];
				Poly[idxLeft+nPoly-1]	= Next[1];
				//
				// Get the color at this ramp position...
				RColor		clrNew( m_pRamp->GetColor(ComputeLinearPercentage(xMinEdge-xMinGrad,lenGradient),pctPrecision) );
				//
				// If it's different, we need to draw and reset...
				if( clrNew != clr && nBandWidth>=nMinimumBand || nPoly==(sizeof(Poly)/sizeof(Poly[0])))
					{
					//
					// Different colors, so draw.
					//
					// Make sure there's something to fill...
					if( nPoly>2 )
						{
						//
						// Untransform the polygon...
						for( YPointCount nPts=nPoly ; nPts-- ; )
							{
							Poly[idxLeft+nPts] *= Rotate90ToHilite;
							}
						//
						// Now we can draw it...
						ds.SetFillColor( clr );
						FillGradientPolygon( ds, pClipPolygon, Poly+idxLeft, nPoly );

#ifdef	TPSDEBUG
static char fImageTmp = false;
if ( fImageTmp )
	RenderToScreen( ds, rcBounds );
#endif	//	TPSDEBUG
						}
					//
					// And finally, reset for the next ramp color...
					idxLeft						= (sizeof(Poly)/sizeof(Poly[0]))/2-1;
					nPoly							= YPointCount(2);
					Poly[idxLeft]				= Prev[0];
					Poly[idxLeft+nPoly-1]	= Prev[1];
					fPrevEdges					= fPolyEdges;
					fPolyEdges					= 0;
					clr							= clrNew;
					nBandWidth					-= nMinimumBand;
					}
				if( QueryAbortGradient(ds) )
					return;
				}// while(x<maxx)
			//
			// Done, all we need do is make sure we've finished filling...
			if( nPoly > 2 )
				{
				for( YPointCount nPts=nPoly ; nPts-- ; )
					{
					Poly[idxLeft+nPts] *= Rotate90ToHilite;
					}
				ds.SetFillColor( clr );
				FillGradientPolygon( ds, pClipPolygon, Poly+idxLeft, nPoly );
				}
			break;
			}
		case kSquareBurst:
		case kCircularBurst:
			{
			//
			// First, clear the bitmap to the outside color...
			RColor	clr( m_pRamp->GetColor(YPercentage(1.0),YPercentage(1.0)) );
			ds.SetFillColor( clr );
			FillGradientRectangle( ds, pClipPolygon, rcAdjustedBounds );
			//
			// Now, generate the color burst bands.
			// First, create the outer rectangle...
			RRealRect	rcUntransformedGradient( RRealPoint( GetGradientOrigin() ), RRealPoint( GetGradientOrigin() ) );
			rcUntransformedGradient.Inflate( RRealSize(GetGradientLength(),GetGradientLength()) );
			//
			// Use the radius for color band determination...
			YIntDimension	nDeviceRadius = ::Round( rdlenGradient );
			//
			// Grab the untransformed gradient origin for use in the transforms...
			RRealPoint		ptUntransformedOrigin( GetGradientOrigin() );
			//
			// Now we have the radius of the outer burst in device units.  This is the maximum resolution
			// that the gradient can have, so we use each radius as a potential band...
			// clr is the color of the outside band.
			// loop until we've filled all bands...
			//
			// Minimum band size is nMinimumBand.  We need to divide that up in the
			// radius...
			for( YRealDimension nRadius=YRealDimension(nDeviceRadius-1) ;
					nRadius>=0.0 ;
					nRadius-=nMinimumBand )
				{
				YPercentage	pctPos	= ComputeLinearPercentage( YRealDimension(nRadius), YRealDimension(nDeviceRadius) );
				RColor	clrNew( m_pRamp->GetColor(pctPos,pctPrecision) );
				if( clrNew != clr )
					{
					//
					// the color changed, so fill with the new color inside the current radius...
					// First, create the transform matrix
					RRealSize		szOffset			= GetHiliteOffset( YPercentage(1.0-pctPos) );
					R2dTransform	xform;
					if( m_eGradientType==kSquareBurst )
						xform.PreRotateAboutPoint( ptUntransformedOrigin.m_x, ptUntransformedOrigin.m_y, GetGradientAngle() );
					xform.PostScaleAboutPoint( ptUntransformedOrigin.m_x, ptUntransformedOrigin.m_y, YScaleFactor(pctPos), YScaleFactor(pctPos) );
					xform.PostTranslate( szOffset.m_dx, szOffset.m_dy );
#if	0 //
		  //	This was an attempt to stop rendering a gradient when the polygon bounds 
		  //	no longer intersects the color swash bounds.
		  //  It did not improve speed drastically so I am commenting it out to
		  //	avoid any potential errors and/or bugs it might introduce.

					//	If we have a clip polygon, see if the rcBounds of the polygon
					//	intersects the color swash area.  If it does not, we are done this polygon
					RRealVectorRect	rcToFill = (RRealVectorRect(rcUntransformedGradient)*xform) * xformGradient;
					if ( pClipPolygon )
						{
						//	If the square area to fill does not intersect the polybounds,
						//		we are done with this poly. All swashes have been rendered
						if ( !rcToFill.m_TransformedBoundingRect.IsIntersecting( rcBounds ) )
							return;
	#if	0 //	This was another attempt to reduce the amount of calls to
			  //	either FillGradientEllipse or FillGradientRectangle by catching
			  //	when the area will be completely redrawn by other swash

						//	If the square area to fill is completely inside the area of
						//	1/2 of the bounds, assume it will be overdrawn
						//	by another swash,  so just continue.
						const	YRealDimension	kInsideBoundsAdjustment( 1. / 2. );
						rcToFill.ScaleAboutPoint( rcToFill.GetCenterPoint(), RRealSize(kInsideBoundsAdjustment, kInsideBoundsAdjustment));
						RRealRect rcTest = rcToFill.m_TransformedBoundingRect;
						if ( (rcTest.m_Left<=rcBounds.m_Left) && (rcTest.m_Top<=rcBounds.m_Top) &&
							  (rcTest.m_Right>=rcBounds.m_Right) && (rcTest.m_Bottom>=rcBounds.m_Bottom) )
							continue;
	#endif	//	0
						}
#endif	//	0

					//
					// Now fill it...
					clr = clrNew;
					ds.SetFillColor( clr );
					if( m_eGradientType==kCircularBurst )
						FillGradientEllipse( ds, pClipPolygon, RRealRect(rcUntransformedGradient)*xform, xformGradient );
					else
						FillGradientRectangle( ds, pClipPolygon, RRealVectorRect(rcUntransformedGradient)*xform, xformGradient );
					}
				if( QueryAbortGradient(ds) )
					return;
				}
			break;
			}
		case kRadial:
			{
			//
			// Gradient is arc-segment bands with the center at m_StartPoint, the direction of the
			// initial color (ramp position 0.0) is along the vector from m_StartPoint to m_EndPoint,
			// and ramp position 1.0 is at 360 degrees (2pi).  colors advance in the counter-clockwise
			// direction.

			//
			// vector length in gradient is irrelevant, so we extend the precision...
			szVector	= RRealSize( 1000.0, 0.0 ) * xformRotate;
			ptVector	= RRealPoint( (RRealPoint( GetGradientOrigin() ) + RRealSize( szVector ) ) * xformGradient );

			//
			// Get the resultant gradient vector angle...
			szVector	= ptVector - ptOrigin;
			YAngle	angGradient	= YAngle( atan2(szVector.m_dy,szVector.m_dx) );

			//
			// Now, generate the color burst bands...
			YIntCoordinate	x			= rcAdjustedBounds.m_Left;
			YIntCoordinate	y			= rcAdjustedBounds.m_Top;
			YRealDimension	yDelta	= y - ptOrigin.m_y;
			YRealDimension	xDelta	= x - ptOrigin.m_x;
			RColor			clr( m_pRamp->GetColor( ComputeAngularPercentage(xDelta,yDelta,angGradient), pctPrecision) );

			RRealPoint		pt0( rcAdjustedBounds.m_Left, rcAdjustedBounds.m_Top );
			RRealPoint		pt1;
			YRealDimension	nBandWidth( 0.0 );
			//
			// Draw the segments from the origin to the top edge of the bounding rectangle...
			while( x<rcAdjustedBounds.m_Right )
				{
				RColor	clrNew( m_pRamp->GetColor( ComputeAngularPercentage(xDelta,yDelta,angGradient), pctPrecision) );
				nBandWidth	+= YRealDimension(1.0);
				if( clrNew != clr && nBandWidth>=nMinimumBand )
					{
					//
					// Draw the segment...
					DrawSegment( ds, pClipPolygon, clr, ptOrigin, pt0, pt1=RRealPoint(x,y) );
					pt0			= pt1;
					clr			= clrNew;
					nBandWidth	-= nMinimumBand;
					}
				++x;
				xDelta += YRealDimension(1);
				if( QueryAbortGradient(ds) )
					return;
				}
			DrawSegment( ds, pClipPolygon, clr, ptOrigin, pt0, pt1=RRealPoint(x,y) );
			pt0	= pt1;
			--x;

			//
			// Draw the segments from the origin to the right edge of the bounding rectangle...
			nBandWidth	= 0.0;
			while( y<rcAdjustedBounds.m_Bottom )
				{
				RColor	clrNew( m_pRamp->GetColor(ComputeAngularPercentage(xDelta,yDelta,angGradient), pctPrecision) );
				nBandWidth	+= YRealDimension(1.0);
				if( clrNew != clr && nBandWidth>=nMinimumBand )
					{
					//
					// Draw the segment...
					DrawSegment( ds, pClipPolygon, clr, ptOrigin, pt0, pt1=RRealPoint(x,y) );
					pt0			= pt1;
					clr			= clrNew;
					nBandWidth	-= nMinimumBand;
					}
				++y;
				yDelta += YRealDimension(1);
				if( QueryAbortGradient(ds) )
					return;
				}
			DrawSegment( ds, pClipPolygon, clr, ptOrigin, pt0, pt1=RRealPoint(x,y) );
			pt0	= pt1;
			--y;

			//
			// Draw the segments from the origin to the bottom edge of the bounding rectangle...
			nBandWidth	= 0.0;
			while( x>=rcAdjustedBounds.m_Left )
				{
				RColor	clrNew( m_pRamp->GetColor(ComputeAngularPercentage(xDelta,yDelta,angGradient), pctPrecision) );
				nBandWidth	+= YRealDimension(1.0);
				if( clrNew != clr && nBandWidth>=nMinimumBand )
					{
					//
					// Draw the segment...
					DrawSegment( ds, pClipPolygon, clr, ptOrigin, pt0, pt1=RRealPoint(x,y) );
					pt0			= pt1;
					clr			= clrNew;
					nBandWidth	-= nMinimumBand;
					}
				--x;
				xDelta -= YRealDimension(1);
				if( QueryAbortGradient(ds) )
					return;
				}
			DrawSegment( ds, pClipPolygon, clr, ptOrigin, pt0, pt1=RRealPoint(x,y) );
			pt0	= pt1;
			++x;

			//
			// Draw the segments from the origin to the left edge of the bounding rectangle...
			nBandWidth	= 0.0;
			while( y>=rcAdjustedBounds.m_Top )
				{
				RColor	clrNew( m_pRamp->GetColor(ComputeAngularPercentage(xDelta,yDelta,angGradient), pctPrecision) );
				nBandWidth	+= YRealDimension(1.0);
				if( clrNew != clr && nBandWidth>=nMinimumBand )
					{
					//
					// Draw the segment...
					DrawSegment( ds, pClipPolygon, clr, ptOrigin, pt0, pt1=RRealPoint(x,y) );
					pt0			= pt1;
					clr			= clrNew;
					nBandWidth	-= nMinimumBand;
					}
				--y;
				yDelta -= YRealDimension(1);
				if( QueryAbortGradient(ds) )
					return;
				}
			DrawSegment( ds, pClipPolygon, clr, ptOrigin, pt0, pt1=RRealPoint(x,y) );
			break;
			}
		}
	//
	// restore the color to the drawing surface is done by the stack class RFillColorRestorer
}


inline int CompareFloats( YFloatType a, YFloatType b )
{
	if( ::AreFloatsEqual(a,b) )
		return 0;
	if( a < b )
		return -1;
	return 1;
}

//****************************************************************************
//
// RGradientFill::SetBoundingRect( const RIntRect& rcBounds )
//
// @MFunc Set the target bounding rect for the gradient (resets params)
//
// @RDesc nothing
//
void RGradientFill::SetBoundingRect(
			const RIntRect&	rcBounds )	// @Parm the target bounding rectangle
{
	//
	// Assuming renaissance coordinates:
	// top > bottom, left < right
	YRealDimension	rdLength = 0;
	YAngle			angle( GetGradientAngle() );
	RRealRect		rcTmp( rcBounds );
	BOOLEAN			fFlipped( BOOLEAN(rcTmp.m_Top>rcTmp.m_Bottom) );
	if( fFlipped )
		{
		//
		// gradient angle is relative to flipped cartesian coordinates, so we need to negate it...
		angle = -angle;
		YRealDimension	nTmp	= rcTmp.m_Top;
		rcTmp.m_Top				= rcTmp.m_Bottom;
		rcTmp.m_Bottom			= nTmp;
		}
#if FIX_BURST_ANGLE_AT_LONGEST
	if( m_eGradientType==kCircularBurst || m_eGradientType==kSquareBurst )
		{
		//
		// then we fix the angle at 0 or 90 degrees, whichever is longest...
		if( ::Abs(rcBounds.Width()) < ::Abs(rcBounds.Height()) )
			{
			//
			// it's taller, so fix it to 90
			angle	= YAngle(kPI/2);
			}
		else
			{
			//
			// it's wider (or square), so fix it to 0
			angle	= YAngle(0);
			}
		SetGradientAngle( angle );
		}
#elif FIX_BURST_LENGTH_AT_LONGEST
	BOOLEAN	fFixedLength	= FALSE;
	if( m_eGradientType==kCircularBurst || m_eGradientType==kSquareBurst )
		{
		rdLength			= YRealDimension(YRealDimension(::Max(::Abs(rcBounds.Width()),::Abs(rcBounds.Height()))) / 2);
		fFixedLength	= TRUE;
		}
#endif
	double	cosa( ::cos(angle) );
	double	sina( ::sin(angle) );
	int	nDirX( ::CompareFloats(cosa,0.0) );
	int	nDirY( ::CompareFloats(sina,0.0) );
	//
	// Determine the ramp origin and the point opposite that point...
	BOOLEAN		fLinear( BOOLEAN(GetType()==kLinear) );
	RRealPoint	ptOrigin( rcTmp.GetCenterPoint() );
	RRealPoint	ptThrough;
	if( nDirX < 0 )
		{
		if( fLinear ) ptOrigin.m_x	= rcTmp.m_Right;
		ptThrough.m_x	= rcTmp.m_Left;
		}
	else if( nDirX > 0 )
		{
		if( fLinear ) ptOrigin.m_x	= rcTmp.m_Left;
		ptThrough.m_x	= rcTmp.m_Right;
		}
	else
		{
		ptThrough.m_x	= ptOrigin.m_x;
		}
	if( nDirY < 0 )
		{
		if( fLinear ) ptOrigin.m_y	= rcTmp.m_Top;
		ptThrough.m_y	= rcTmp.m_Bottom;
		}
	else if( nDirY > 0 )
		{
		if( fLinear ) ptOrigin.m_y	= rcTmp.m_Bottom;
		ptThrough.m_y	= rcTmp.m_Top;
		}
	else
		{
		ptThrough.m_y	= ptOrigin.m_y;
		}

	//
	// Compute another point on the line perpendicular to the RAMP and passing
	// through ptThrough...
#if FIX_BURST_LENGTH_AT_LONGEST
	if( !fFixedLength )
#endif
	{
		RRealSize	perpvector( YRealDimension(100.0*sina), YRealDimension(100.0*cosa) );
		RRealPoint	ptLine( ptThrough + perpvector );
		rdLength	= ptOrigin.DistanceToLine( ptThrough, ptLine );
	}
	//
	// Set the new gradient length...
	SetGradientLength( rdLength );
	//
	// Set the new gradient origin...
	SetGradientOrigin( ptOrigin );
	//
	// Reset the transformation matrix.
	R2dTransform	baseTransform;	// identity...
	if( fFlipped )
		{
		//
		// we're working in flipped coordinates, so in order to unflip them we need
		// to flip the color space...
		baseTransform.PreTranslate( 0.0, YRealDimension(rcTmp.Height()) );
		baseTransform.PreScale( 1.0, -1.0 );
		}
	//
	// Set the new base transformation matrix...
	SetBaseTransform( baseTransform );
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RGradientFill::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGradientFill::Validate( ) const
{
	RObject::Validate( );
	TpsAssertIsObject( RGradientFill, this );
	switch( m_eGradientType )
		{
		case kCircularBurst:
		case kSquareBurst:
		case kLinear:
		case kRadial:
			break;
		default:
			TpsAssert( m_eGradientType==kUndefined, "Invalid gradient type" );
			break;
		}
	TpsAssert( m_pctposHilite>=0.0 && m_pctposHilite<=1.0, "Invalid hilite position" );
}

#endif	//	TPSDEBUG