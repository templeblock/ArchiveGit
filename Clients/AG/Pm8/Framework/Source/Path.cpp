// ****************************************************************************
//
//  File Name:			Path.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood
//
//  Description:		Definition of the RPath class
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/Path.cpp                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "Path.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifndef	IMAGER
#include "ApplicationGlobals.h"
#else
#include "GlobalBuffer.h"
#endif

#include "SingleGraphic.h"
#include "Bezier.h"
#include "QuadraticSpline.h"
#include "DrawingSurface.h"
#include "Storage.h"
#include "ChunkyStorage.h"

// defines
#ifdef	IMAGER
	extern	BOOLEAN  			 gfShowPathPoints;
	#define  RAH_TEST_CODE		 1
#endif	

// prototypes
void CalcSplineBoundingRect( const RIntPoint& q0, const RIntPoint& q1, const RIntPoint& q2, RIntRect& rc );


// ****************************************************************************
//
//  Function Name:	RPath::RPath( )
//
//  Description:		Copy constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPath::RPath( const RPath &rhs )
{
	*this = rhs;
}

// ****************************************************************************
//
//  Function Name:	RPath::RPath( )
//
//  Description:		Copy constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RPath::RPath( const RPath &rhs, const R2dTransform& transform )
{
	*this = rhs;
	//	Reinitialize the transform to identity so the given transform will do something
	m_LastTransform.MakeIdentity();
	//
	//	Transform the points to account for the 
	RIntPointArrayIterator	iTransIntPtsIterator		= m_TransformedIntPoints.Start(); 
	RRealPointArrayIterator	iTransRealPtsIterator	= m_TransformedRealPoints.Start(); 
	RRealPointArrayIterator	iOrigPtsIterator			= m_PathPoints.Start();
	while ( iTransIntPtsIterator != m_TransformedIntPoints.End() )
	{
		*iTransRealPtsIterator = *iOrigPtsIterator * transform;
		*iTransIntPtsIterator = *iTransRealPtsIterator;
		++iOrigPtsIterator;
		++iTransIntPtsIterator;
		++iTransRealPtsIterator;
	}
	m_SegmentLengths.Empty();

	//	Copy the transformed points the new points and reset LastTransform
	//	to be indentity again
	m_PathPoints = m_TransformedRealPoints;
	m_LastTransform.MakeIdentity();
	// recompute bounds and size
	CalcBoundingRect();
	m_PathSize = RIntSize( m_BoundingRect.m_Right, m_BoundingRect.m_Bottom );
}

// ****************************************************************************
//
//  Function Name:	RPath::operator =( )
//
//  Description:		Assignment operator.
//
//  Returns:			reference to this object
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RPath& RPath::operator=( const RPath& rhs )
{
	if ( this == &rhs )
		return *this;

	m_PathOperators			= rhs.m_PathOperators;
	m_PathPoints				= rhs.m_PathPoints;
	m_TransformedIntPoints	= rhs.m_TransformedIntPoints;
	m_TransformedRealPoints	= rhs.m_TransformedRealPoints;
	m_SegmentLengths			= rhs.m_SegmentLengths;
	m_OutlineCount				= rhs.m_OutlineCount;
	m_PathSize					= rhs.m_PathSize;
	m_BoundingRect				= rhs.m_BoundingRect;
	m_LastTransform			= rhs.m_LastTransform;

	return *this;
}

// ****************************************************************************
//
// Function Name:		RPath::Define( )
//
// Description:		Define the path based on a rectangle
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RPath::Define( const RIntRect& rect ) 
{
	return Define( RRealRect( rect ) );
}

// ****************************************************************************
//
// Function Name:		RPath::Define( )
//
// Description:		Define the path based on a rectangle
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RPath::Define( const RRealRect& rect ) 
{
	TpsAssert( ( m_PathOperators.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_PathPoints.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_TransformedIntPoints.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_TransformedRealPoints.Count() == 0 ), "Path already defined." );
	m_BoundingRect = rect;

	m_PathOperators.InsertAtEnd( kMoveTo );
	m_PathOperators.InsertAtEnd( kLine );
	m_PathOperators.InsertAtEnd( (EPathOperator)4 );
	m_PathOperators.InsertAtEnd( kClose );
	m_PathOperators.InsertAtEnd( kEnd );

	m_PathPoints.InsertAtEnd( m_BoundingRect.m_TopLeft );
	m_PathPoints.InsertAtEnd( RIntPoint( m_BoundingRect.m_Right, m_BoundingRect.m_Top ) );
	m_PathPoints.InsertAtEnd( m_BoundingRect.m_BottomRight );
	m_PathPoints.InsertAtEnd( RIntPoint( m_BoundingRect.m_Left, m_BoundingRect.m_Bottom ) );
	m_PathPoints.InsertAtEnd( m_BoundingRect.m_TopLeft );

	m_OutlineCount = 1;
	CopyPointArray( m_PathPoints, m_TransformedIntPoints );		// m_TransformedIntPoints = m_PathPoints;
	m_TransformedRealPoints = m_PathPoints;
	m_LastTransform.MakeIdentity();
	m_PathSize = RIntSize( m_BoundingRect.m_Right, m_BoundingRect.m_Bottom );
	TpsAssertValid( this );

	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RPath::Define( )
//
// Description:		Define the path based on a poly-polygon
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RPath::Define( const RIntPoint* pVertices, YPointCount* pNumVertices, YPolylineCount polyCount, BOOLEAN fClosed ) 
{
	TpsAssert( ( m_PathOperators.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_PathPoints.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_TransformedIntPoints.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_TransformedRealPoints.Count() == 0 ), "Path already defined." );

	return MergePoints( pVertices, pNumVertices, polyCount, fClosed );
}

// ****************************************************************************
//
// Function Name:		RPath::Define( )
//
// Description:		Define the path based on a poly-polygon
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RPath::DefineFromArcs( const RIntPoint* pArcPoints, YPointCount* pNumArcPoints, YPolylineCount polyCount )
{
	TpsAssert( ( m_PathOperators.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_PathPoints.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_TransformedIntPoints.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_TransformedRealPoints.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_OutlineCount == 0 ), "Undefined path has an outline count != 0" );

	BOOLEAN					fOkay = TRUE;

	while( fOkay && polyCount-- )
	{
		YPointCount		nArcPoints	= *pNumArcPoints++;
		YCounter			nBeziers		= 0;
		RIntPointArray	arTmp;

		TpsAssert( ( nArcPoints > 1 ), "Invalid number of arc points" );
		TpsAssert( ( ( nArcPoints & 0x01 ) != 0 ), "Invalid number of arc points" );

		--nArcPoints;
		while( fOkay && nArcPoints )
		{
			RRealPointArray	arPolyBezier;
			fOkay = CreatePolyBezierFromArc( pArcPoints[0], pArcPoints[1], pArcPoints[2], arPolyBezier );
			if( fOkay )
				{
				//
				// Append the points to the path point array...
				RRealPointArray::YIterator	itrStart = arPolyBezier.Start();
				//
				// If this is not the first arc, then it starts at the end of the last one, so we
				// don't want the first point...
				if( nBeziers )
					++itrStart;
				::Append( arTmp, itrStart, arPolyBezier.End() );
				//
				// Update the number of beziers added to the path...
				nBeziers += (arPolyBezier.Count()-1)/3;
			}
			pArcPoints += 2;
			nArcPoints -= 2;
		}
		if( fOkay )
		{
			//
			// Merge the path in...
			EPathOperatorArray	pathOps;
			pathOps.InsertAtEnd( kMoveTo );
			pathOps.InsertAtEnd( kBezier );
			pathOps.InsertAtEnd( (EPathOperator)nBeziers );
			pathOps.InsertAtEnd( kEnd );
			fOkay = MergePoints( pathOps, arTmp );
		}
	}
	return fOkay;
}

// ****************************************************************************
//
// Function Name:		RPath::Define( )
//
// Description:		Define the path based on a list of points and path operators
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RPath::Define( EPathOperator* pOperators, const RIntPoint* pPoints )
{
	TpsAssert( ( m_PathOperators.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_PathPoints.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_TransformedIntPoints.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_TransformedRealPoints.Count() == 0 ), "Path already defined." );
	TpsAssert( ( m_OutlineCount == 0 ), "Undefined path has an outline count != 0" );

	return MergePoints( pOperators, pPoints );
}


// ****************************************************************************
//
// Function Name:		RPath::IsDefined( )
//
// Description:		Return TRUE if the path is defined
//
// Returns:				True if the point count > 0
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RPath::IsDefined( ) const
{
	return static_cast<BOOLEAN>( ( GetPointCount() > 0 ) ? TRUE : FALSE );
}

// ****************************************************************************
//
// Function Name:		RPath::Append( )
//
// Description:		Append the given path data onto the end of this path
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RPath::Append( RPath& path, const R2dTransform& transform )
{
	//	Apply the given transform and then merge the paths
	path.ApplyTransform( transform );
	return MergePoints( path.m_PathOperators, path.m_TransformedIntPoints );
}

// ****************************************************************************
//
// Function Name:		RPath::MergePoints( )
//
// Description:		Merge the given points and operators into the current path
//							at the end.
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RPath::MergePoints( const RIntPoint* pVertices, YPointCount* pNumVertices, YPolylineCount polyCount, BOOLEAN fClosed )
{
const RIntPoint* 	pPoints			= pVertices;
int					numTotalPoints	= 0;

	TpsAssert( ( pPoints != NULL ), "Nil path point array." );

	//	If the last operator is an End, remove it...
	if (m_PathOperators.Count() > 0)
	{
		EPathOperatorArrayIterator	iter = m_PathOperators.End();
		--iter;
		TpsAssert( *iter == kEnd, "Merging points where the last operator was not End" );
		m_PathOperators.RemoveAt( iter );
	}

	for ( int j = 0; j < polyCount; ++j, ++pPoints )
	{
		YPointCount		pointCount		= 0;
		RIntPoint		polyStartPoint	= *pPoints;
		int				numVertices		= pNumVertices[j];

		numTotalPoints	+= numVertices;

		TpsAssert( ( numVertices >= 2 ), "Paths must have at least 2 points." );
		TpsAssert( ( numTotalPoints < 8189 ), "Paths must be less than 32k." );

		m_PathOperators.InsertAtEnd( kMoveTo );
		m_PathPoints.InsertAtEnd( *pPoints );
		for ( int i = 1; i < numVertices; i++ )
		{
			++pPoints;
			m_PathPoints.InsertAtEnd( *pPoints );
			++pointCount;
			if ( pointCount > 250 )
			{
				m_PathOperators.InsertAtEnd( kLine );
				m_PathOperators.InsertAtEnd( (EPathOperator)pointCount );
				pointCount = 0;
			}
		}
		if ( fClosed && *pPoints != polyStartPoint )
		{
			m_PathPoints.InsertAtEnd( polyStartPoint );
			pointCount++;
		}
		if ( pointCount > 0 )
		{
			m_PathOperators.InsertAtEnd( kLine );
			m_PathOperators.InsertAtEnd( (EPathOperator)pointCount );
		}
		if ( fClosed )
			m_PathOperators.InsertAtEnd( kClose );
	}

	m_PathOperators.InsertAtEnd( kEnd );

	m_OutlineCount = 1;
	CopyPointArray( m_PathPoints, m_TransformedIntPoints );		// m_TransformedIntPoints = m_PathPoints;
	m_TransformedRealPoints = m_PathPoints;
	m_LastTransform.MakeIdentity();
	CalcBoundingRect();
	m_PathSize = RIntSize( m_BoundingRect.m_Right, m_BoundingRect.m_Bottom );
	TpsAssertValid( this );

	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RPath::MergePoints( )
//
// Description:		Merge the given points and operators into the current path
//							at the end.
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RPath::MergePoints( const EPathOperator* pOperators, const RIntPoint* pPoints )
{
EPathOperator				pathOperator;
EPathOperator				lastOperator = kEnd;
YPointCount					pointCount;
YPathOperatorRepCount	repCount;
int							lastRepCount = 0;
int							i;
BOOLEAN 						fInObject = FALSE;

	TpsAssert( ( pOperators != NULL ), "Nil path operators array." );
	TpsAssert( ( pPoints != NULL ), "Nil path point array." );
	TpsAssert( ( *pOperators == kMoveTo ), "Paths must start with a moveto." );

	//	If the last operator is an End, remove it...
	if (m_PathOperators.Count() > 0)
	{
		EPathOperatorArrayIterator	iter = m_PathOperators.End();
		--iter;
		TpsAssert( *iter == kEnd, "Merging points where the last operator was not End" );
		m_PathOperators.RemoveAt( iter );
	}

	while( ( pathOperator = *pOperators++ ) != kEnd )
	{
		switch ( pathOperator )
		{
			case kMoveTo :
				if ( fInObject )									// if we are currently in an object, end it
				{
					m_OutlineCount++;
					TpsAssert( ( m_OutlineCount <= kMaxPathPolygons ), "Too many outlines!" );
				}
				else if ( lastOperator == kMoveTo )			// factor out concurrent moves
				{
					RRealPointArrayIterator	k = m_PathPoints.End();
					m_PathPoints.RemoveAt( --k );
				}
				if ( lastOperator != kMoveTo )
					m_PathOperators.InsertAtEnd( kMoveTo );
				m_PathPoints.InsertAtEnd( *pPoints++ );
				lastRepCount = 0;
				fInObject = FALSE;
				break;

			case kClose :
				if ( fInObject ) 
				{
					m_PathOperators.InsertAtEnd( kClose );
					m_OutlineCount++;
					TpsAssert( ( m_OutlineCount <= kMaxPathPolygons ), "Too many outlines!" );
				}
				lastRepCount = 0;
				fInObject = FALSE;
				break;

			case kLine :
				pointCount = (YPathOperatorRepCount)*pOperators++;
				if ( lastOperator == kLine && ( ( pointCount + lastRepCount ) < 250 ) )
				{
					EPathOperatorArrayIterator	l = m_PathOperators.End();
					lastRepCount = pointCount + lastRepCount;
					*(--l) = (EPathOperator)lastRepCount;
				}
				else
				{
					m_PathOperators.InsertAtEnd( kLine );
					m_PathOperators.InsertAtEnd( (EPathOperator)pointCount );
					lastRepCount = pointCount;
				}
				for ( i = 0; i < pointCount; i++ )
					m_PathPoints.InsertAtEnd( *pPoints++ );
				fInObject = TRUE;
				break;

			case kBezier :
				repCount = (YPathOperatorRepCount)*pOperators++;
				if ( lastOperator == kBezier && ( ( repCount + lastRepCount ) < 250 ) )
				{
					EPathOperatorArrayIterator	m = m_PathOperators.End();
					lastRepCount = repCount + lastRepCount;
					*(--m) = (EPathOperator)lastRepCount;
				}
				else
				{
					m_PathOperators.InsertAtEnd( kBezier );
					m_PathOperators.InsertAtEnd( (EPathOperator)repCount );
					lastRepCount = repCount;
				}
				for ( i = 0; i < ( repCount * 3 ); i++ )
					m_PathPoints.InsertAtEnd( *pPoints++ );
				fInObject = TRUE;
				break;

			case kQuadraticSpline :
				repCount	= (YPathOperatorRepCount)*pOperators++;
				if ( lastOperator == kQuadraticSpline && ( ( repCount + lastRepCount ) < 250 ) )
				{
					EPathOperatorArrayIterator	m = m_PathOperators.End();
					lastRepCount = repCount + lastRepCount;
					*(--m) = (EPathOperator)lastRepCount;
				}
				else
				{
					m_PathOperators.InsertAtEnd( kQuadraticSpline );
					m_PathOperators.InsertAtEnd( (EPathOperator)repCount );
					lastRepCount = repCount;
				}
				for ( i = 0; i < ( repCount * 2 ); i++ )
					m_PathPoints.InsertAtEnd( *pPoints++ );
				fInObject = TRUE;
				break;

			default :
				TpsAssertAlways( "Unrecognized Path operator." );
				lastRepCount = 0;
				break;
		}
		lastOperator = pathOperator;
	}
	if ( lastOperator == kMoveTo )
	{
		EPathOperatorArrayIterator	k2 = m_PathOperators.End();
		m_PathOperators.RemoveAt( --k2 );
		RRealPointArrayIterator	k3 = m_PathPoints.End();
		m_PathPoints.RemoveAt( --k3 );
	}
	m_PathOperators.InsertAtEnd( kEnd );
	if ( fInObject )
		m_OutlineCount++;

	CopyPointArray( m_PathPoints, m_TransformedIntPoints );		// m_TransformedIntPoints = m_PathPoints;
	m_TransformedRealPoints = m_PathPoints;
	m_LastTransform.MakeIdentity();
	CalcBoundingRect();
	m_PathSize = RIntSize( m_BoundingRect.m_Right, m_BoundingRect.m_Bottom );
	TpsAssertValid( this );

	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RPath::Undefine( )
//
// Description:		Undefine the path 
//
// Returns:				Nothing.
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPath::Undefine( ) 
{
	m_PathOperators.Empty();
	m_PathPoints.Empty();
	m_TransformedIntPoints.Empty();
	m_TransformedRealPoints.Empty();
	m_SegmentLengths.Empty();
	m_OutlineCount = 0;
	m_PathSize = RIntSize( 0, 0 );
	m_BoundingRect = RIntRect( 0, 0, 0, 0 );
	m_LastTransform.MakeIdentity();
}

// ****************************************************************************
//
// Function Name:		RPath::UndefineLastTransform( )
//
// Description:		Undefine the path 
//
// Returns:				Nothing.
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPath::UndefineLastTransform( ) 
{
	R2dTransform	newTransform;
	ApplyTransform( newTransform );
}

// ****************************************************************************
//
// Function Name:		RPath::UsesBeziers( )
//
// Description:		Checks to see if the path uses beziers
//
// Returns:				Boolean indicating bezier data in path.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RPath::UsesBeziers( ) const
{
EPathOperatorArrayIterator		iOps = m_PathOperators.Start(); 

	TpsAssertValid( this );
	// check to see if path contains bezier data
	while ( *iOps != kEnd )
	{
		switch ( *iOps )
		{
			case kMoveTo :
			case kClose :
				break;
			case kLine :
			case kQuadraticSpline :
				++iOps;
				break;
			case kBezier :
				return TRUE;
				break;
			default :
				TpsAssertAlways( "Unrecognized Path operator." );
				break;
		}
		++iOps;
	}
	return FALSE;
}

// ****************************************************************************
//
// Function Name:		RPath::UsesSplines( )
//
// Description:		Checks to see if the path uses quadradic splines
//
// Returns:				Boolean indicating spline data in path.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RPath::UsesSplines( ) const
{
EPathOperatorArrayIterator		iOps = m_PathOperators.Start(); 

	TpsAssertValid( this );
	// check to see if path contains spline data
	while ( *iOps != kEnd )
	{
		switch ( *iOps )
		{
			case kMoveTo :
			case kClose :
				break;
			case kLine :
			case kBezier :
				++iOps;
				break;
			case kQuadraticSpline :
				return TRUE;
				break;
			default :
				TpsAssertAlways( "Unrecognized Path operator." );
				break;
		}
		++iOps;
	}
	return FALSE;
}

// ****************************************************************************
//
// Function Name:		RPath::IsClosed( )
//
// Description:		Scans path operator data for close.
//
// Returns:				Boolean indicating path or poly path is closed.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RPath::IsClosed( const EPathOperator* pOperators ) 
{
BOOLEAN						fClosed = FALSE; 
BOOLEAN						fUnclosed = FALSE; 
EPathOperator				pathOperator;
EPathOperator				lastOperator = kEnd;
int							nrOutlines = 0;

	while ( ( pathOperator = *pOperators ) != kEnd )
	{
		switch ( pathOperator )
		{
			case kMoveTo :
				if ( lastOperator != kClose )
					if ( lastOperator != kMoveTo )
						if ( nrOutlines != 0 )
							fUnclosed = TRUE; 
				nrOutlines++;
				break;
			case kClose :
				fClosed = TRUE; 
				break;
			case kLine :
			case kBezier :
			case kQuadraticSpline :
				pOperators++;
				break;
			default :
				TpsAssertAlways( "Unrecognized Path operator." );
				break;
		}
		lastOperator = pathOperator;
		pOperators++;
	}
#ifdef	TPSDEBUG
	if ( nrOutlines > 1 )
	{
		TpsAssert( ( fClosed != fUnclosed ), "Mixing closed and unclosed paths in a poly-path is not allowed.");
	}
#endif
	return fClosed;
}

// ****************************************************************************
//
// Function Name:		RPath::Reverse( )
//
// Description:		Reverse path operators and points.
//
// Returns:				Nothing.
//
// Exceptions:			Memory
//
//
// ****************************************************************************
//
void RPath::Reverse( ) 
{
EPathOperatorArray				pathOperators = m_PathOperators;
RRealPointArray					pathPoints = m_PathPoints;
EPathOperatorArrayIterator		iOps = pathOperators.Start(); 
RRealPointArrayIterator			iPts = pathPoints.Start(); 
EPathOperator						pathOperator;
EPathOperator						lastOperator = kMoveTo;
YPointCount							pointCount;
YPathOperatorRepCount			repCount;
BOOLEAN								fMove = FALSE;

	try
	{
		// reinitialize path
		m_PathOperators.Empty();
		m_PathPoints.Empty();
		m_TransformedIntPoints.Empty();
		m_TransformedRealPoints.Empty();
		m_SegmentLengths.Empty();
		// repopulate path points in reverse order
		while ( iPts != pathPoints.End() )
		{
			m_PathPoints.InsertAtStart( *iPts );
			++iPts;
		}
		CopyPointArray( m_PathPoints, m_TransformedIntPoints );		// m_TransformedIntPoints = m_PathPoints;
		m_TransformedRealPoints = m_PathPoints;
		m_LastTransform.MakeIdentity();
		// repopulate path operators in reverse order
		m_PathOperators.InsertAtEnd( kEnd );
		while ( ( pathOperator = *iOps ) != kEnd )
		{
			++iOps;
			switch ( pathOperator )
			{
				case kMoveTo :
					if ( lastOperator != kMoveTo )
						fMove = TRUE;
					break;
	
				case kClose :
					{
						for ( EPathOperatorArrayIterator opz = m_PathOperators.Start(); opz != m_PathOperators.End(); ++opz )
						{
							if ( *opz == kMoveTo || *opz == kClose || *opz == kEnd )
							{
								m_PathOperators.InsertAt( opz, kClose );
								break;
							}
							else if ( *opz == kLine || *opz == kBezier || *opz == kQuadraticSpline )
								++opz;
						}
					}
					break;
	
				case kLine :
					pointCount = (YPathOperatorRepCount)*iOps;
					++iOps;
					m_PathOperators.InsertAtStart( (EPathOperator)pointCount );
					m_PathOperators.InsertAtStart( kLine );
					break;
	
				case kBezier :
					repCount = (YPathOperatorRepCount)*iOps;
					++iOps;
					m_PathOperators.InsertAtStart( (EPathOperator)repCount );
					m_PathOperators.InsertAtStart( kBezier );
					break;
	
				case kQuadraticSpline :
					repCount	= (YPathOperatorRepCount)*iOps;
					++iOps;
					m_PathOperators.InsertAtStart( (EPathOperator)repCount );
					m_PathOperators.InsertAtStart( kQuadraticSpline );
					break;
	
				default :
					TpsAssertAlways( "Unrecognized Path operator." );
					break;
			}
			if ( fMove )
				m_PathOperators.InsertAtStart( kMoveTo );
			fMove = FALSE;
			lastOperator = pathOperator;
		}
		m_PathOperators.InsertAtStart( kMoveTo );
		TpsAssertValid( this );
	}
	catch ( ... )
	{
		Undefine();
		throw;
	}
}

// ****************************************************************************
//
// Function Name:		RPath::ConvertLinesToBeziers( )
//
// Description:		Converts line segments to beziers.  Converts 
//							Horizontal lines if the kHorizontal flag is
//                   set, and vertical lines if the kVertical flag
//                   is set.
//
// Returns:				Nothing.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RPath::ConvertLinesToBeziers( uWORD flags )
{
	EPathOperatorArray				pathOperators = m_PathOperators ;
	EPathOperatorArrayIterator		iOps = pathOperators.Start(); 
	RRealPointArray					pathPoints = m_PathPoints;
	RRealPointArrayIterator			iPts = pathPoints.Start(); 
	EPathOperator						pathOperator;
	RRealPoint							ptLast( 0, 0 );

	try
	{
		// reinitialize path
		m_PathPoints.Empty();
		m_PathOperators.Empty();
		m_TransformedIntPoints.Empty();
		m_TransformedRealPoints.Empty();
		m_SegmentLengths.Empty();

		while ( ( pathOperator = *iOps++ ) != kEnd )
		{
			int repCount = 0;

			switch ( pathOperator )
			{
			case kMoveTo :
				m_PathOperators.InsertAtEnd( pathOperator ) ;
				m_PathPoints.InsertAtEnd( *iPts );
				ptLast = *iPts++;
				break ;

			case kBezier :
			case kQuadraticSpline :
				{
					repCount = (int) *iOps++;
					m_PathOperators.InsertAtEnd( pathOperator ) ;
					m_PathOperators.InsertAtEnd( (EPathOperator) repCount ) ;

					const kPointCount[] = { 1, 0, 1, 3, 0, 2 };
					int nPtCount = kPointCount[pathOperator] * repCount ;

					for ( int i = 0; i < nPtCount; i++ )
					{
						m_PathPoints.InsertAtEnd( *iPts );
						ptLast = *iPts;
						iPts++;
					}

					break;
				}

			case kLine:
			{
				// Get the number of points in the line
				int nCount = (int) *iOps++;

				// Set the path operator to an indeterminate
				// state, so that the operator will be written
				// out for the first line of first bezier.
				pathOperator = (EPathOperator) -1 ;

				for (int i = 0, nRepCount = 0; i < nCount; i++, nRepCount++)
				{
					RRealPoint  ptNew ;
					RRealPoint& point = *iPts++ ;

					BOOLEAN fHorzConvert = BOOLEAN(flags & kHorizontalLines) && BOOLEAN( point.m_y == ptLast.m_y ) ;
					BOOLEAN fVertConvert = BOOLEAN(flags & kVerticalLines)   && BOOLEAN( point.m_x == ptLast.m_x ) ;

					if (fHorzConvert || fVertConvert)
					{
						if (kBezier != pathOperator)
						{
							//
							// Changing operator types, so write rep count
							// of previous operation and then new operator 
							// into the stream.  Note, if nRepCount is 0, 
							// then we are starting the first operation so 
							// so there is no rep count to write.
							//

							if (nRepCount)
							{
								m_PathOperators.InsertAtEnd( (EPathOperator) nRepCount ) ;
							}

							m_PathOperators.InsertAtEnd( kBezier ) ;
							pathOperator = kBezier ;
							nRepCount = 0 ;
						}

						YRealDimension xLen = point.m_x - ptLast.m_x ;
						YRealDimension yLen = point.m_y - ptLast.m_y ;

						ptNew.m_x = ptLast.m_x + 0.3333 * xLen ;
						ptNew.m_y = ptLast.m_y + 0.3333 * yLen ;
						m_PathPoints.InsertAtEnd( ptNew );

						ptNew.m_x = ptLast.m_x + 0.6667 * xLen ;
						ptNew.m_y = ptLast.m_y + 0.6667 * yLen ;
						m_PathPoints.InsertAtEnd( ptNew );

						m_PathPoints.InsertAtEnd( point );
					}
					else
					{
						if (kLine != pathOperator)
						{
							//
							// Changing operator types, so write rep count
							// of previous operation and then new operator 
							// into the stream.  Note, if nRepCount is 0, 
							// then we are starting the first operation so 
							// so there is no rep count to write.
							//

							if (nRepCount)
							{
								m_PathOperators.InsertAtEnd( (EPathOperator) nRepCount ) ;
							}

							m_PathOperators.InsertAtEnd( kLine ) ;
							pathOperator = kLine ;
							nRepCount = 0 ;
						}

						m_PathPoints.InsertAtEnd( point );
					}

					ptLast = point ;
				}

				// Add in the last repcount value.
				m_PathOperators.InsertAtEnd( (EPathOperator) nRepCount ) ;
				break;
			}

			default:
				m_PathOperators.InsertAtEnd( pathOperator ) ;

			} // switch

		}

		m_PathOperators.InsertAtEnd( kEnd ) ;

		// recompute bounding data
		CopyPointArray( m_PathPoints, m_TransformedIntPoints );		// m_TransformedIntPoints = m_PathPoints;
		m_TransformedRealPoints = m_PathPoints;
		m_LastTransform.MakeIdentity();
		CalcBoundingRect();
		m_PathSize = RIntSize( m_BoundingRect.m_Right, m_BoundingRect.m_Bottom );
		TpsAssertValid( this );
	}
	catch ( ... )
	{
		Undefine();
		throw;
 	}
}

// ****************************************************************************
//
// Function Name:		RPath::ConvertSplinesToBeziers( )
//
// Description:		Converts all quadratic spline segments in the path to 
//							bezier segments
//
// Returns:				Nothing.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RPath::ConvertSplinesToBeziers( ) 
{
//	TpsAssertAlways( "RPath::ConvertSplinesToBeziers(): integer points are NOT sufficient for this!" );
RRealPointArray					pathPoints = m_PathPoints;
EPathOperatorArrayIterator		iOps = m_PathOperators.Start(); 
RRealPointArrayIterator			iPts = pathPoints.Start(); 
EPathOperator						pathOperator;
YPointCount							pointCount;
YPathOperatorRepCount			repCount;
int									i;
RRealPoint							ptLast( 0, 0 );
RRealPoint							ptSpline[4], ptBezier[4];

	try
	{
		// reinitialize path
		m_PathPoints.Empty();
		m_TransformedIntPoints.Empty();
		m_TransformedRealPoints.Empty();
		m_SegmentLengths.Empty();
		// repopulate path substituting beziers for splines
		while ( ( pathOperator = *iOps ) != kEnd )
		{
			++iOps;
			switch ( pathOperator )
			{
				case kMoveTo :
					m_PathPoints.InsertAtEnd( *iPts );
					ptLast = *iPts;
					++iPts;
					break;
	
				case kClose :
					break;
	
				case kLine :
					pointCount = (YPathOperatorRepCount)*iOps;
					++iOps;
					for ( i = 0; i < pointCount; i++ )
					{
						m_PathPoints.InsertAtEnd( *iPts );
						ptLast = *iPts;
						++iPts;
					}
					break;
	
				case kBezier :
					repCount = (YPathOperatorRepCount)*iOps;
					++iOps;
					for ( i = 0; i < ( repCount * 3 ); i++ )
					{
						m_PathPoints.InsertAtEnd( *iPts );
						ptLast = *iPts;
						++iPts;
					}
					break;
	
				case kQuadraticSpline :
					--iOps;
					*iOps = kBezier;
					++iOps;
					repCount	= (YPathOperatorRepCount)*iOps;
					++iOps;
					for ( i = 0; i < repCount; i++ )
					{
						ptSpline[0] = ptLast;
						ptSpline[1] = *iPts;
						++iPts;
						ptSpline[2] = *iPts;
						++iPts;
						::CreateBezierFromSpline( &ptSpline[ 0 ], &ptBezier[ 0 ] );
						m_PathPoints.InsertAtEnd( ptBezier[1] );
						m_PathPoints.InsertAtEnd( ptBezier[2] );
						m_PathPoints.InsertAtEnd( ptBezier[3] );
						ptLast = ptBezier[3];
					}
					break;
	
				default :
					TpsAssertAlways( "Unrecognized Path operator." );
					break;
			}
		}
		// recompute bounding data
		CopyPointArray( m_PathPoints, m_TransformedIntPoints );		// m_TransformedIntPoints = m_PathPoints;
		m_TransformedRealPoints = m_PathPoints;
		m_LastTransform.MakeIdentity();
		CalcBoundingRect();
		m_PathSize = RIntSize( m_BoundingRect.m_Right, m_BoundingRect.m_Bottom );
		TpsAssertValid( this );
	}
	catch ( ... )
	{
		Undefine();
		throw;
 	}
}

// ****************************************************************************
//
// Function Name:		RPath::ApplyTransform( )
//
// Description:		Apply a transform to the points in the path
//
// Returns:				Nothing.
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RPath::ApplyTransform( const R2dTransform& transform ) 
{
	//
	// If the transformed integer points don't exist, then let's copy the xformd reals in first...
	if( m_TransformedIntPoints.Count() == 0 )
	{
		//
		// NOTE: We might be able to optimize this using a SetSize() member to arrays...
		::Append( m_TransformedIntPoints, m_TransformedRealPoints.Start(), m_TransformedRealPoints.End() );
	}
	else
	{
		TpsAssert( m_TransformedIntPoints.Count() == m_TransformedRealPoints.Count(), "int point array is of different size" );
	}
	//
	//	If the transforms are equal, don't do anything
	if (m_LastTransform != transform)
	{
		//
		// Apply the given transformation to all points...
		// Attempt to optimize it:  if the only thing that changed was the translation
		// then we can simply adjust the already transformed points
		if ( m_LastTransform.AreScaleAndRotateEqual( transform ) )
		{
			//
			// Only the translation is different, so we can compute the delta and
			// adjust the transformed points directly...
			RRealPointArrayIterator	itrOriginal	= m_PathPoints.Start();
			if( itrOriginal != m_PathPoints.End() )
			{
				//
				// First, we compute the offset...
				RIntPointArrayIterator	itrInt		= m_TransformedIntPoints.Start();
				RRealPointArrayIterator	itrReal		= m_TransformedRealPoints.Start();
				RRealPoint					ptNewReal	= (*itrOriginal) * transform;
				RIntPoint					ptNewInt		= ptNewReal;
				RRealSize					realOffset	= ptNewReal - (*itrReal);
				RIntSize						intOffset	= ptNewInt - (*itrInt);
				//
				// Now offset the points...
				while( itrReal != m_TransformedRealPoints.End() )
				{
					(*itrReal)	+= realOffset;
					(*itrInt)	+= intOffset;
					++itrReal;
					++itrInt;
				}
			}
		}
		else
		{
			//	Transform is sufficiently different to allow
			//	the points to be altered with the transform
			RRealPointArrayIterator	itrOriginal	= m_PathPoints.Start();;
			RIntPointArrayIterator	itrInt		= m_TransformedIntPoints.Start(); 
			RRealPointArrayIterator	itrReal		= m_TransformedRealPoints.Start(); 
			while( itrReal != m_TransformedRealPoints.End() )
			{
				(*itrReal)	= (*itrOriginal) * transform;
				(*itrInt)	= (*itrReal);
				++itrOriginal;
				++itrReal;
				++itrInt;
			}
		}
		//
		// Finally, we can copy the transform...
		m_LastTransform	= transform;
		m_SegmentLengths.Empty();
	}
}

// ****************************************************************************
//
// Function Name:		RPath::ApplyScale( )
//
// Description:		Create a transform and apply it to the points in the path
//
// Returns:				Nothing.
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RPath::ApplyScale( YScaleFactor scale ) 
{
R2dTransform				transform;

	transform.PreScale( scale, scale );
	ApplyTransform( transform );
	m_SegmentLengths.Empty();
}

// ****************************************************************************
//
// Function Name:		RPath::PointOnPath( )
//
// Description:		Find the point on the path some faction of its length 
//							along the path.
//
// Returns:				Point on the path.
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RRealPoint RPath::PointOnPath( YScaleFactor fraction ) 
{
	TpsAssert( ( fraction >= 0.0 ), "Distance along a path cannot be negative." );
	TpsAssert( ( fraction <= 1.0 ), "Distance along a path cannot be greater than path length." );
	return PointOnPath( (YIntDimension)((YFloatType)GetLength() * fraction) );
}

// ****************************************************************************
//
// Function Name:		RPath::PointOnPath( )
//
// Description:		Find the point on the path some distance 
//							along the path.
//
// Returns:				Point on the path.
//
// Exceptions:			None
//
// ****************************************************************************
//
RRealPoint RPath::PointOnPath( YIntDimension distance ) 
{
RRealPoint						pt( 0, 0 );
sWORD								segment = 0;		//	changed 6/6/96 mdh - since GetSegmentOfPath takes a sWORD not an int.
RRealPoint						points[4];
YIntDimension					length = GetLength();		// will calc length of each segment if we haven't already
YScaleFactor 					fraction;

	// validations
	TpsAssertValid( this );
	TpsAssert( ( distance >= 0 ), "Distance along a path cannot be negative." );
	TpsAssert( ( distance <= length ), "Distance along a path cannot be greater than path length." );

	// easiest to special case the start point
	if ( distance <= 0 )
		return *m_TransformedRealPoints.Start();

	// find segment in which point resides
	for ( YIntDimensionArrayIterator k = m_SegmentLengths.Start(); k != m_SegmentLengths.End(); ++k )
	{
		length = *k; 
		segment++;
		if ( ( distance - length ) <= 0 )
			break;
		distance -= length;
	}
	fraction = (YRealDimension)distance / (YRealDimension)length;

	// parse path to find that segment 
	switch ( GetSegmentOfPath( segment, points ) )
	{
		case kLine :
			pt = PointOnLine( points[0], points[1], distance );
			break;
		case kBezier :
			pt = PointOnBezier( points, fraction );
			break;
		case kQuadraticSpline :
			pt = PointOnQuadraticSpline( points, fraction );
			break;
		default :
			TpsAssertAlways( "Invalid segment operator." );
			break;
	}

	return pt;
}

// ****************************************************************************
//
// Function Name:		RPath::PointOnPath( )
//
// Description:		Find the point on the path some distance 
//							along the path.
//
// Returns:				Point on the path.
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RRealPoint RPath::PointOnPath( YIntDimension distance, YIntDimension cord ) 
{
	return PointOnPath( distance, cord, NULL );
}

// ****************************************************************************
//
// Function Name:		RPath::PointOnPath( )
//
// Description:		Find the point on the path some faction of its length 
//							along the path.
//
// Returns:				Point on the path.
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RRealPoint RPath::PointOnPath( YIntDimension distance, YIntDimension cord, YIntDimension* segment ) 
{
YIntDimension	total = GetLength();
YIntDimension	dim = ( distance + cord );
YIntDimension	iter = ( cord / 2 );
YIntDimension	length;
RRealPoint		pt0, pt1;

	// validations/initializations
	if ( dim > total )
	{
		if ( segment != NULL )
			*segment = total - distance;
		return GetEndPoint();
	}
	pt0 = PointOnPath( distance );
	pt1 = PointOnPath( dim );
	length = ::Round( pt0.Distance( pt1 ) );

	// binary seach to find correct point
	while ( length < ( cord - 1 ) || length > ( cord + 1 ) )
	{
		if ( length < cord )
			dim = distance + cord + iter;
		else
			dim = distance + cord - iter;
		if ( dim > total )
			dim = total;
		pt1 = PointOnPath( dim );
		length = ::Round( pt0.Distance( pt1 ) );
		if ( ( iter = ( iter / 2 ) ) == 0 )
			break;
	}

	// the shortest distance between two points is a straight line
	if ( dim < ( distance + cord ) )
	{
		dim = distance + cord;
		pt1 = PointOnPath( dim );
	}
	if ( segment != NULL )
		*segment = dim - distance;

	return pt1;
}

// ****************************************************************************
//
// Function Name:		RPath::PointOnLine( )
//
// Description:		Find the point on a line some faction of its length 
//							along the line.
//
// Returns:				Point on a line.
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RIntPoint RPath::PointOnLine( const RIntPoint& ptStart, const RIntPoint& ptEnd, YIntDimension distance )
{
YIntDimension		length = ::Round( ptStart.Distance( ptEnd ) );
RIntSize				delta;

	if ( ptStart == ptEnd )
		return ptStart;
	if ( distance <= 0 )
		return ptStart;
	if ( distance >= length )
		return ptEnd;

	delta = ptEnd - ptStart;
	delta.m_dx = (YIntDimension)((YFloatType)delta.m_dx * ( (YFloatType)distance / (YFloatType)length )); 
	delta.m_dy = (YIntDimension)((YFloatType)delta.m_dy * ( (YFloatType)distance / (YFloatType)length ));

	return RIntPoint( ( ptStart.m_x + delta.m_dx ), ( ptStart.m_y + delta.m_dy ) );
}

// ****************************************************************************
//
// Function Name:		RPath::PointOnLine( )
//
// Description:		Find the point on a line some faction of its length 
//							along the line.
//
// Returns:				Point on a line.
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RRealPoint RPath::PointOnLine( const RRealPoint& ptStart, const RRealPoint& ptEnd, YIntDimension distance )
{
YRealDimension		length = ptStart.Distance( ptEnd );
RRealSize			delta;

	if ( ::AreFloatsEqual( ptStart.m_x, ptEnd.m_x ) ) 
		if ( ::AreFloatsEqual( ptStart.m_y, ptEnd.m_y ) ) 
			return ptStart;
	if ( distance <= 0 )
		return ptStart;
	if ( distance >= length )
		return ptEnd;

	delta = ptEnd - ptStart;
	delta.m_dx = ( delta.m_dx * ( (YFloatType)distance / length )); 
	delta.m_dy = ( delta.m_dy * ( (YFloatType)distance / length ));

	return RRealPoint( ( ptStart.m_x + delta.m_dx ), ( ptStart.m_y + delta.m_dy ) );
}

// ****************************************************************************
//
// Function Name:		RPath::PointOnBezier( )
//
// Description:		Find the point on a bezier segment some faction of its length 
//							along the segment.
//
// Returns:				Point on a bezier.
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RIntPoint RPath::PointOnBezier( const RIntPoint* points, YScaleFactor t ) 
{
	TpsAssert( ( t >= 0.0 ), "Distance along a path cannot be negative." );
	TpsAssert( ( t <= 1.0 ), "Distance along a path cannot be greater than path length." );
	return ::PointOnBezier( points, t );
}

// ****************************************************************************
//
// Function Name:		RPath::PointOnBezier( )
//
// Description:		Find the point on a bezier segment some faction of its length 
//							along the segment.
//
// Returns:				Point on a bezier.
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RRealPoint RPath::PointOnBezier( const RRealPoint* points, YScaleFactor t ) 
{
	TpsAssert( ( t >= 0.0 ), "Distance along a path cannot be negative." );
	TpsAssert( ( t <= 1.0 ), "Distance along a path cannot be greater than path length." );
	return ::PointOnBezier( points, t );
}

// ****************************************************************************
//
// Function Name:		RPath::PointOnQuadraticSpline( )
//
// Description:		Find the point on a quadratic spline segment some faction of its  
//							length along the segment.
//
// Returns:				Point on a Quadratic Spline.
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RIntPoint RPath::PointOnQuadraticSpline( const RIntPoint* points, YScaleFactor t ) 
{
	RRealPoint	bez[ 4 ];
	::CreateBezierFromSpline( points, &bez[ 0 ] );
	return ::PointOnBezier( &bez[ 0 ], t );
}

// ****************************************************************************
//
// Function Name:		RPath::PointOnQuadraticSpline( )
//
// Description:		Find the point on a quadratic spline segment some faction of its  
//							length along the segment.
//
// Returns:				Point on a Quadratic Spline.
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RRealPoint RPath::PointOnQuadraticSpline( const RRealPoint* points, YScaleFactor t ) 
{
	RRealPoint	bez[ 4 ];
	::CreateBezierFromSpline( points, &bez[ 0 ] );
	return ::PointOnBezier( &bez[ 0 ], t );
}

// ****************************************************************************
//
// Function Name:		RPath::PointAtX( )
//
// Description:		
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
//
RRealPoint RPath::PointAtX( YRealCoordinate x )
{
YIntDimension			length = GetLength();		// will calc length of each segment if we haven't already
RRealPoint				pts[4];
RRealPoint				pt;

	switch ( FindXSegment( x, pts ) )
		{
		case kQuadraticSpline:
			//
			// it's a quadratic
			// elevate it to a bezier and fall through...
			// NOTE: ::CreateBezierFromSpline can be called on the spline points as long
			//       as there is room for one more point at the end...
			::CreateBezierFromSpline( &pts[ 0 ], &pts[ 0 ] );
			//
			// intentional fall through...
		case kBezier:
			{
			//
			// it's a bezier
			// find a linear segment that the points falls into...
			RRealPoint	ptSubdivided[7];
			while( !PointsAreCollinear(pts[0],pts[1],pts[2]) || !PointsAreCollinear(pts[0],pts[2],pts[3]) )
				{
				::SubdivideBezier( pts[0], pts[1], pts[2], pts[3], ptSubdivided );
				if( pts[3].m_x < pts[0].m_x )
					{
					//
					// order is right to left...
					if( x < ptSubdivided[3].m_x )
						{
						//
						// keep the right-hand (leftmost) portion
						memmove( pts, ptSubdivided+3, 4*sizeof(RRealPoint) );
						}
					else
						{
						//
						// keep the left-hand (rightmost) portion
						memmove( pts, ptSubdivided, 4*sizeof(RRealPoint) );
						}
					}
				else
					{
					//
					// order is left to right...
					if( x > ptSubdivided[3].m_x )
						{
						//
						// keep the right-hand (leftmost) portion
						memmove( pts, ptSubdivided+3, 4*sizeof(RRealPoint) );
						}
					else
						{
						//
						// keep the left-hand (rightmost) portion
						memmove( pts, ptSubdivided, 4*sizeof(RRealPoint) );
						}
					}
				}
			//
			// Bezier is linear, so copy the last point to the second and fall through
			// the line...
			pts[1]	= pts[3];
			}
			//
			// intentional fall through...
		case kLine:
			//
			// it's a line...
			// solve for the point...
			if( pts[0].m_y==pts[1].m_y )
				{
				//
				// horizontal...
				pt = RRealPoint( x, pts[0].m_y );
				}
			else if( pts[0].m_x==pts[1].m_x )
				{
				//
				// vertical.  pick the midpoint of the segment...
				pt	= RRealPoint( x, (pts[0].m_y+pts[1].m_y)/2 );
				}
			else
				{
				//
				// not horizontal or vertical...
				YRealDimension	deltaY	= pts[1].m_y - pts[0].m_y;
				YRealDimension	deltaX	= pts[1].m_x - pts[0].m_x;
				pt.m_y	= pts[0].m_y + (x-pts[0].m_x)*deltaY/deltaX;
				pt.m_x	= x;
				}
			break;
		}
	return pt;
}

// ****************************************************************************
//
// Function Name:		RPath::PointAtY( )
//
// Description:		
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
//
RRealPoint RPath::PointAtY( YRealCoordinate y )
{
YIntDimension			length = GetLength();		// will calc length of each segment if we haven't already
RRealPoint				pts[4];
RRealPoint				pt;

	switch ( FindYSegment( y, pts ) )
		{
		case kQuadraticSpline:
			//
			// it's a quadratic
			// elevate it to a bezier and fall through...
			// NOTE: ::CreateBezierFromSpline can be called on the spline points as long
			//       as there is room for one more point at the end...
			::CreateBezierFromSpline( &pts[ 0 ], &pts[ 0 ] );
			//
			// intentional fall through...
		case kBezier:
			{
			//
			// it's a bezier
			// find a linear segment that the points falls into...
			RRealPoint	ptSubdivided[7];
			while( !PointsAreCollinear(pts[0],pts[1],pts[2]) || !PointsAreCollinear(pts[0],pts[2],pts[3]) )
				{
				::SubdivideBezier( pts[0], pts[1], pts[2], pts[3], ptSubdivided );
				if( pts[3].m_y < pts[0].m_y )
					{
					//
					// order is right to left...
					if( y < ptSubdivided[3].m_y )
						{
						//
						// keep the right-hand (leftmost) portion
						memmove( pts, ptSubdivided+3, 4*sizeof(RRealPoint) );
						}
					else
						{
						//
						// keep the left-hand (rightmost) portion
						memmove( pts, ptSubdivided, 4*sizeof(RRealPoint) );
						}
					}
				else
					{
					//
					// order is left to right...
					if( y > ptSubdivided[3].m_y )
						{
						//
						// keep the right-hand (leftmost) portion
						memmove( pts, ptSubdivided+3, 4*sizeof(RRealPoint) );
						}
					else
						{
						//
						// keep the left-hand (rightmost) portion
						memmove( pts, ptSubdivided, 4*sizeof(RRealPoint) );
						}
					}
				}
			//
			// Bezier is linear, so copy the last point to the second and fall through
			// the line...
			pts[1]	= pts[3];
			}
			//
			// intentional fall through...
		case kLine:
			//
			// it's a line...
			// solve for the point...
			if( pts[0].m_x==pts[1].m_x )
				{
				//
				// vertical...
				pt = RRealPoint( pts[0].m_x, y );
				}
			else if( pts[0].m_y==pts[1].m_y )
				{
				//
				// horizontal.  pick the midpoint of the segment...
				pt	= RRealPoint( (pts[0].m_x+pts[1].m_x)/2, y );
				}
			else
				{
				//
				// not vertical or horizontal...
				YRealDimension	deltaY	= pts[1].m_y - pts[0].m_y;
				YRealDimension	deltaX	= pts[1].m_x - pts[0].m_x;
				pt.m_x	= pts[0].m_x + (y-pts[0].m_y)*deltaX/deltaY;
				pt.m_y	= y;
				}
			break;
		}
	return pt;
}

// ****************************************************************************
//
// Function Name:		RPath::Render( )
//
// Description:		Renders the path outline into the given drawing surface.
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPath::Render( RDrawingSurface& drawingSurface,
						  const RRealSize& size,
						  const R2dTransform& transform,
						  const RIntRect& rRender,
						  const RColor& monochromeColor,
						  BOOLEAN fImageAsMask )
{
RSolidColor		strokeColor( kBlack );
RColor			fillColor = ( fImageAsMask ? monochromeColor : RColor( strokeColor ) );

	Render( drawingSurface, size, transform, rRender, kStroked, strokeColor, fillColor );
}

// ****************************************************************************
//
// Function Name:		RPath::Render( )
//
// Description:		Renders the path into the given drawing surface with the
//							specified fill method.
//
//							The rotation angle specifies a counter clockwise rotation
//							in radians.
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPath::Render( RDrawingSurface& drawingSurface,
						  const RRealSize& size,
						  const R2dTransform& transform,
						  const RIntRect& /* rRender */,
						  YFillMethod fillMethod,
						  const RSolidColor& strokeColor,
						  const RColor& fillColor,
						  int	nPenWidth)
{
R2dTransform 		resultantTransform = transform;					// make a copy of the transform
RIntSize 			offset( 0, 0 );
 	
	TpsAssertValid( this );
	TpsAssertValid( ( &drawingSurface ) );

	// Initialize the surface attributes
	//	Don't draw a gradiant
	if ( drawingSurface.GetColoringBookMode() )
		{
		drawingSurface.SetPenColor( RSolidColor( kBlack ) );
		drawingSurface.SetFillColor( RColor(RSolidColor(kWhite)) );
		}
	else
		{
		RColor crFillColor( fillColor );
		RRealRect rBoundingRect( size );
		crFillColor.SetBoundingRect( rBoundingRect );
		crFillColor *= transform;

		drawingSurface.SetPenColor( RColor( strokeColor ) );
		drawingSurface.SetFillColor( crFillColor );
		}
	drawingSurface.SetPenWidth(nPenWidth);

	// Scale the graphic
	if ( size.m_dx > 0 && size.m_dy > 0 )
		resultantTransform.PreScale( size.m_dx / m_PathSize.m_dx, size.m_dy / m_PathSize.m_dy );

	// Paths have an origin equal to the bottom left of the graphic.
	// We draw with the origin equal to the top left of the screen. Correct for this here.
	resultantTransform.PreTranslate( 0, m_PathSize.m_dy );
	resultantTransform.PreScale( 1, -1 );

	// Apply the resultant transform to the points
	ApplyTransform( resultantTransform );

	// Draw the path
	RenderPathPoints( drawingSurface, m_PathOperators, m_TransformedIntPoints, m_TransformedRealPoints, fillMethod, FALSE, offset );
}

// ****************************************************************************
//
// Function Name:		RPath::Render( )
//
// Description:		Renders the path into the given drawing surface.
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPath::Render( RDrawingSurface& drawingSurface,
						  const R2dTransform& transform,
						  const RIntRect& /* rRender */,
						  YFillMethod fillMethod )
{
	///	This function would need to be critical sectioned for possible Multi-Threading issues
	TpsAssertValid( this );
	TpsAssertValid( ( &drawingSurface ) );

	//	Assume the drawingSurface is already initialized

	// Apply the resultant transform to the points
	ApplyTransform( transform );

	// Draw the path
	RIntSize		offset (0, 0 );		//	Make sure to give initial values
	RenderPathPoints( drawingSurface, &(*m_PathOperators.Start()), &(*m_TransformedIntPoints.Start()), &(*m_TransformedRealPoints.Start()), fillMethod, FALSE, offset );
}

// ****************************************************************************
//
// Function Name:		RPath::MergeWith( )
//
// Description:		Merge the path into the specified graphic
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RIntRect RPath::MergeWith( const R2dTransform& transform, RSingleGraphic& graphic ) 
{
	TpsAssertValid( this );
	RTempSingleGraphic	tsg;
	RIntRect					rcBounds( GetBoundingRect() );
	tsg.m_OperationsStream.InsertAtEnd( kPath );
	tsg.m_OperationsStream.InsertAtEnd( kEndGraphic );
	tsg.m_FillMethodsStream.InsertAtEnd( kStroked );
	tsg.m_PointCountsStream.InsertAtEnd( m_PathPoints.Count() );
	tsg.m_LastTransform.MakeIdentity();
	tsg.m_OriginalPointsStream			 = m_PathPoints;
//ApplyTransform will copy these	CopyPointArray( m_PathPoints, tsg.m_TransformedIntPointsStream );
	tsg.m_TransformedRealPointsStream = m_PathPoints;
	tsg.m_PathOperatorsStream			 = m_PathOperators;
	tsg.m_GraphicSize						 = RIntSize( rcBounds.m_Right, rcBounds.m_Bottom );	 
	tsg.m_fMonochrome						 = TRUE;
	graphic.AppendGraphic( tsg, transform );
	return rcBounds * transform;
}

// ****************************************************************************
//
// Function Name:		RPath::CalcPath( )
//
// Description:		Calculate the path the path will follow
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPath::CalcPath( const R2dTransform& transform, RIntPoint*& pPoints, YPointCountArray& countArray )
{
	ApplyTransform( transform );
	RIntRect	rClip;
	CalcPathPoints( m_PathOperators, m_TransformedRealPoints, pPoints, countArray, FALSE, rClip );
}

// ****************************************************************************
//
// Function Name:		RPath::CalcSegmentLengths( )
//
// Description:		Calc the length of each segment in the path and cache those lengths
//
// Returns:				Nothing.
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPath::CalcSegmentLengths( )
{
RRealPointArrayIterator		iPtIterator;
EPathOperatorArrayIterator	iOpIterator;
EPathOperator					pathOperator;
YPointCount						pointCount;
YPathOperatorRepCount		repCount;
RRealPoint						ptLast, ptStart, ptEnd, ptC1, ptC2;
int								i;

	TpsAssertValid( this );
	m_SegmentLengths.Empty();
	iPtIterator = m_TransformedRealPoints.Start(); 
	iOpIterator = m_PathOperators.Start(); 
	while( ( pathOperator = *iOpIterator ) != kEnd )
	{
		++iOpIterator;
		switch( pathOperator )
		{
			case kMoveTo :
				ptLast = ptStart = *iPtIterator;
				++iPtIterator;
				break;

			case kClose :
				if ( ptLast != ptStart )
					m_SegmentLengths.InsertAtEnd( ::Round( ptLast.Distance( ptStart ) ) );
				break;

			case kLine :
				pointCount = (YPathOperatorRepCount)*iOpIterator;
				++iOpIterator;
				for ( i = 0; i < pointCount; i++ )
				{
					m_SegmentLengths.InsertAtEnd( ::Round( ptLast.Distance( *iPtIterator ) ) );
					ptLast = *iPtIterator;
					++iPtIterator;
				}
				break;

			case kBezier :
				repCount = (YPathOperatorRepCount)*iOpIterator;
				++iOpIterator;
				for ( i = 0; i < repCount; i++ )
				{
					ptC1 = *iPtIterator;
					++iPtIterator;
					ptC2 = *iPtIterator;
					++iPtIterator;
					ptEnd = *iPtIterator;
					m_SegmentLengths.InsertAtEnd( ::Round( CalcBezierLength( ptLast, ptC1, ptC2, ptEnd ) ) );
					ptLast = *iPtIterator;
					++iPtIterator;
				}
				break;

			case kQuadraticSpline :
				repCount = (YPathOperatorRepCount)*iOpIterator;
				++iOpIterator;
				for ( i = 0; i < repCount; i++ )
				{
					ptC1 = *iPtIterator;
					++iPtIterator;
					ptEnd = *iPtIterator;
					m_SegmentLengths.InsertAtEnd( ::Round( CalcQuadraticSplineLength( ptLast, ptC1, ptEnd ) ) );
					ptLast = *iPtIterator;
					++iPtIterator;
				}
				break;

			default :
				TpsAssertAlways( "Unrecognized Path operator." );
				break;
		}
	}
}

// ****************************************************************************
//
// Function Name:		RPath::CalcBezierLength( )
//
// Description:		Calc the length of a bezier segment
//
// Returns:				Length of curve segment.
//
// Exceptions:			None
//
// ****************************************************************************
//
YIntDimension RPath::CalcBezierLength( const RIntPoint& ptStart, 
													const RIntPoint& ptControl1, 
													const RIntPoint& ptControl2, 
													const RIntPoint& ptEnd ) 
{
	RIntPoint	ptBez[3];
	ptBez[0]	= ptControl1;
	ptBez[1]	= ptControl2;
	ptBez[2]	= ptEnd;

	YPointCount		numExpanded;
	RIntPoint*		pptExpanded	= RDrawingSurface::ExpandPolyBezier( ptStart, ptBez, 3, numExpanded );
	YRealDimension	nLength( 0.0 );
	RIntPoint*	ppt = pptExpanded;
	while( --numExpanded > 0 )
	{
		RIntPoint&	ptFrom	= *ppt++;
		nLength	+= ptFrom.Distance( *ppt );
	}
	//
	// cleanup 
	::ReleaseGlobalBuffer( (uBYTE*)pptExpanded );
	return ::Round( nLength );
}

// ****************************************************************************
//
// Function Name:		RPath::CalcBezierLength( )
//
// Description:		Calc the length of a bezier segment
//
// Returns:				Length of curve segment.
//
// Exceptions:			None
//
// ****************************************************************************
//
YRealDimension RPath::CalcBezierLength( const RRealPoint& ptStart, 
													 const RRealPoint& ptControl1, 
													 const RRealPoint& ptControl2, 
													 const RRealPoint& ptEnd ) 
{
	RRealPoint	ptBez[3];
	ptBez[0]	= ptControl1;
	ptBez[1]	= ptControl2;
	ptBez[2]	= ptEnd;

	YPointCount		numExpanded;
	RRealPoint*		pptExpanded	= RDrawingSurface::ExpandPolyBezier( ptStart, ptBez, 3, numExpanded );
	YRealDimension	nLength( 0.0 );
	RRealPoint*	ppt = pptExpanded;
	while ( --numExpanded > 0 )
	{
		RRealPoint&	ptFrom	= *ppt++;
		nLength	+= ptFrom.Distance( *ppt );
	}
	//
	// cleanup 
	::ReleaseGlobalBuffer( (uBYTE*)pptExpanded );
	return( nLength );
}

// ****************************************************************************
//
// Function Name:		RPath::CalcQuadraticSplineLength( )
//
// Description:		Calc the length of a quadratic spline segment
//
// Returns:				Length of curve segment.
//
// Exceptions:			None
//
// ****************************************************************************
//
YIntDimension RPath::CalcQuadraticSplineLength( const RIntPoint& ptStart, 
																const RIntPoint& ptControl, 
																const RIntPoint& ptEnd ) 
{
	RIntPoint		pts[2];
	pts[0]	= ptControl;
	pts[1]	= ptEnd;
	YPointCount		numExpanded;
	RIntPoint*		pptExpanded = ::ExpandPolyQuadraticSpline( ptStart, pts, 2, numExpanded );
	RIntPoint*		ppt			= pptExpanded + 1;
	YRealDimension	nLength( 0.0 );
	while ( --numExpanded > 0 )
	{
		nLength	+= (ppt-1)->Distance( *ppt );
		++ppt;
	}
	::ReleaseGlobalBuffer( reinterpret_cast<uBYTE*>(pptExpanded) );
	return ::Round( nLength );
}

// ****************************************************************************
//
// Function Name:		RPath::CalcQuadraticSplineLength( )
//
// Description:		Calc the length of a quadratic spline segment
//
// Returns:				Length of curve segment.
//
// Exceptions:			None
//
// ****************************************************************************
//
YRealDimension RPath::CalcQuadraticSplineLength( const RRealPoint& ptStart, 
																 const RRealPoint& ptControl, 
																 const RRealPoint& ptEnd ) 
{
	RRealPoint		pts[2];
	pts[0]	= ptControl;
	pts[1]	= ptEnd;
	YPointCount		numExpanded;
	RRealPoint*		pptExpanded = ::ExpandPolyQuadraticSpline( ptStart, pts, 2, numExpanded );
	RRealPoint*		ppt			= pptExpanded + 1;
	YRealDimension	nLength( 0.0 );
	while ( --numExpanded > 0 )
	{
		nLength	+= (ppt-1)->Distance( *ppt );
		++ppt;
	}
	::ReleaseGlobalBuffer( reinterpret_cast<uBYTE*>(pptExpanded) );
	return( nLength );
}

// ****************************************************************************
//
// Function Name:		RPath::ApproxBezierLength( )
//
// Description:		Calc the approximate length of a bezier segment
//
// Returns:				Close to length of curve segment.
//
// Exceptions:			None
//
// ****************************************************************************
//
YIntDimension RPath::ApproxBezierLength( const RRealPoint& ptStart, 
													  const RRealPoint& ptControl1, 
													  const RRealPoint& ptControl2, 
													  const RRealPoint& ptEnd ) 
{
YRealDimension		length = 0;
RRealPoint			points[ 8 ];

	::SubdivideBezier( ptStart, ptControl1, ptControl2, ptEnd, points );
	for ( int i = 0; i < 6; i++ )
		length += points[i].Distance( points[i + 1] );

	return ::Round( length );
}

// ****************************************************************************
//
// Function Name:		RPath::ApproxQuadraticSplineLength( )
//
// Description:		Calc the approximate length of a quadratic spline segment
//
// Returns:				Close to length of curve segment.
//
// Exceptions:			None
//
// ****************************************************************************
//
YIntDimension RPath::ApproxQuadraticSplineLength( const RRealPoint& ptStart, 
																  const RRealPoint& ptControl, 
																  const RRealPoint& ptEnd ) 
{
YRealDimension		length = 0;
RRealPoint			points[ 8 ];

	::SubdivideQuadraticSpline( ptStart, ptControl, ptEnd, points );
	for ( int i = 0; i < 4; i++ )
		length += points[i].Distance( points[i + 1] );

	return ::Round( length );
}

// ****************************************************************************
//
// Function Name:		RPath::GetLength( )
//
// Description:		Compute the length of the path
//
// Returns:				Length of path.
//
// Exceptions:			None
//
//
// ****************************************************************************
//
YIntDimension RPath::GetLength( ) 
{
YIntDimension		length = 0;

	if ( m_SegmentLengths.Count() == 0 )
		CalcSegmentLengths();

	for ( YIntDimensionArrayIterator k = m_SegmentLengths.Start(); k != m_SegmentLengths.End(); ++k )
		length += *k;

	return length;
}

// ****************************************************************************
//
// Function Name:		RPath::IsEmpty( )
//
// Description:		 
//
// Returns:				Boolean
//
// Exceptions:			None
//
//
// ****************************************************************************
//
BOOLEAN RPath::IsEmpty( const EPathOperator* opz, const RIntPoint* ptz ) 
{
EPathOperator				pathOperator;
YPointCount					pointCount;
YPathOperatorRepCount	repCount;
int							i, j;
RIntPoint					ptStart;
	
	while( ( pathOperator = *opz ) != kEnd )
	{
		++opz;
		switch ( pathOperator )
		{
			case kMoveTo :
				ptStart = *ptz;
				++ptz;
				break;

			case kClose :
				break;

			case kLine :
				pointCount = (YPathOperatorRepCount)*opz;
				++opz;
				for ( i = 0; i < pointCount; i++ )
				{
					if ( ptStart != *ptz )
						return FALSE;
					ptStart = *ptz;
					++ptz;
				}
				break;

			case kBezier :
				repCount = (YPathOperatorRepCount)*opz;
				++opz;
				for ( i = 0; i < repCount; i++ )
				{
					for ( j = 0; j < 3; j++ )
					{
						if ( ptStart != *ptz )
							return FALSE;
						ptStart = *ptz;
						++ptz;
					}
				}
				break;

			case kQuadraticSpline :
				repCount = (YPathOperatorRepCount)*opz;
				++opz;
				for ( i = 0; i < repCount; i++ )
				{
					for ( j = 0; j < 2; j++ )
					{
						if ( ptStart != *ptz )
							return FALSE;
						ptStart = *ptz;
						++ptz;
					}
				}
				break;

			default :
				TpsAssertAlways( "Unrecognized Path operator." );
				break;
		}
	}
	return TRUE;
//RIntRect		rcBounds = CalcBoundingRect( opz, ptz );
//	return rcBounds.IsEmpty();
}

// ****************************************************************************
//
// Function Name:		RPath::IsEmpty( )
//
// Description:		 
//
// Returns:				Boolean
//
// Exceptions:			None
//
//
// ****************************************************************************
//
BOOLEAN RPath::IsEmpty( const EPathOperator* opz, const RRealPoint* ptz ) 
{
EPathOperator				pathOperator;
YPointCount					pointCount;
YPathOperatorRepCount	repCount;
int							i, j;
RRealPoint					ptStart;
	
	while( ( pathOperator = *opz ) != kEnd )
	{
		++opz;
		switch ( pathOperator )
		{
			case kMoveTo :
				ptStart = *ptz;
				++ptz;
				break;

			case kClose :
				break;

			case kLine :
				pointCount = (YPathOperatorRepCount)*opz;
				++opz;
				for ( i = 0; i < pointCount; i++ )
				{
					if ( !::AreFloatsEqual( ptStart.m_x, (*ptz).m_x ) ) 
						return FALSE;
					if ( !::AreFloatsEqual( ptStart.m_y, (*ptz).m_y ) ) 
						return FALSE;
					ptStart = *ptz;
					++ptz;
				}
				break;

			case kBezier :
				repCount = (YPathOperatorRepCount)*opz;
				++opz;
				for ( i = 0; i < repCount; i++ )
				{
					for ( j = 0; j < 3; j++ )
					{
						if ( !::AreFloatsEqual( ptStart.m_x, (*ptz).m_x ) ) 
							return FALSE;
						if ( !::AreFloatsEqual( ptStart.m_y, (*ptz).m_y ) ) 
							return FALSE;
						ptStart = *ptz;
						++ptz;
					}
				}
				break;

			case kQuadraticSpline :
				repCount = (YPathOperatorRepCount)*opz;
				++opz;
				for ( i = 0; i < repCount; i++ )
				{
					for ( j = 0; j < 2; j++ )
					{
						if ( !::AreFloatsEqual( ptStart.m_x, (*ptz).m_x ) ) 
							return FALSE;
						if ( !::AreFloatsEqual( ptStart.m_y, (*ptz).m_y ) ) 
							return FALSE;
						ptStart = *ptz;
						++ptz;
					}
				}
				break;

			default :
				TpsAssertAlways( "Unrecognized Path operator." );
				break;
		}
	}
	return TRUE;
//RIntRect		rcBounds = CalcBoundingRect( opz, ptz );
//	return rcBounds.IsEmpty();
}


// ****************************************************************************
//
// Function Name:		RPath::CalcBoundingRect( )
//
// Description:		Calculates the bounding box of the path from the end points 
//							of line and curve segments and mid-points of curve segments,
//							which should be the extreme points.  This is good enough for 
//							my purposes.  If it is not good enough for anyone else's, 
//                   they can change it.
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RPath::CalcBoundingRect( ) 
{
	if ( m_TransformedRealPoints.Count() == 0 ) 
		m_BoundingRect = RIntRect( 0, 0, 0, 0 );
	else
		m_BoundingRect = CalcBoundingRect( &(*(m_PathOperators.Start())), &(*(m_TransformedRealPoints.Start())) );
}


// ****************************************************************************
//
// Function Name:		RPath::CalcBoundingRect( )
//
// Description:		Calculates the bounding box of the path from the end points 
//							of line and curve segments and mid-points of curve segments,
//							which should be the extreme points.  This is good enough for 
//							my purposes.  If it is not good enough for anyone else's, 
//                   they can change it.
//
// Returns:				RIntRect
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RIntRect RPath::CalcBoundingRect( const EPathOperator* opz,
						  					 const RIntPoint* ptz ) 
{
RIntRect						rcBounds( 0, 0, 0, 0 );
EPathOperator				pathOperator;
YPointCount					pointCount;
YPathOperatorRepCount	repCount;
int							i;
RRealPoint					pt0, pt1, pt2, pt3;
	
	rcBounds = RIntRect( *ptz, *ptz );
	while( ( pathOperator = *opz ) != kEnd )
	{
		++opz;
		switch( pathOperator )
		{
			case kMoveTo :
				rcBounds.AddPointToRect( *ptz );
				pt0 = *ptz;
				++ptz;
				break;

			case kClose :
				break;

			case kLine :
				pointCount = (YPathOperatorRepCount)*opz;
				++opz;
				for ( i = 0; i < pointCount; i++ )
				{
					rcBounds.AddPointToRect( *ptz );
					pt0 = *ptz;
					++ptz;
				}
				break;

			case kBezier :
				repCount = (YPathOperatorRepCount)*opz;
				++opz;
				for ( i = 0; i < repCount; i++ )
				{
					pt1 = *ptz++;
					pt2 = *ptz++;
					pt3 = *ptz++;
					rcBounds.AddPointToRect( RIntPoint( pt3 ) );
					RRealRect	rc;
					::GetBezierBounds( pt0, pt1, pt2, pt3, rc );
					rcBounds.Union( rcBounds, RIntRect( rc ) );
					pt0 = pt3;
				}
				break;

			case kQuadraticSpline :
				repCount = (YPathOperatorRepCount)*opz;
				++opz;
				for ( i = 0; i < repCount; i++ )
				{
					pt1 = *ptz++;
					pt2 = *ptz++;
					RIntRect	rc;
					::CalcSplineBoundingRect( pt0, pt1, pt2, rc );
					rcBounds.Union( rcBounds, rc );
					pt0 = pt2;
				}
				break;

			default :
				TpsAssertAlways( "Unrecognized Path operator." );
				break;
		}
	}
	return rcBounds;
}


// ****************************************************************************
//
// Function Name:		RPath::CalcBoundingRect( )
//
// Description:		Calculates the bounding box of the path from the end points 
//							of line and curve segments and mid-points of curve segments,
//							which should be the extreme points.  This is good enough for 
//							my purposes.  If it is not good enough for anyone else's, 
//                   they can change it.
//
// Returns:				RIntRect
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RIntRect RPath::CalcBoundingRect( const EPathOperator* opz,
						  					 const RRealPoint* ptz ) 
{
RIntRect						rcBounds( 0, 0, 0, 0 );
EPathOperator				pathOperator;
YPointCount					pointCount;
YPathOperatorRepCount	repCount;
int							i;
RRealPoint					pt0, pt1, pt2, pt3;
	
	rcBounds = RIntRect( RIntPoint( *ptz ), RIntPoint( *ptz ) );
	while( ( pathOperator = *opz ) != kEnd )
	{
		++opz;
		switch( pathOperator )
		{
			case kMoveTo :
				rcBounds.AddPointToRect( RIntPoint( *ptz ) );
				pt0 = *ptz;
				++ptz;
				break;

			case kClose :
				break;

			case kLine :
				pointCount = (YPathOperatorRepCount)*opz;
				++opz;
				for ( i = 0; i < pointCount; i++ )
				{
					rcBounds.AddPointToRect( RIntPoint( *ptz ) );
					pt0 = *ptz;
					++ptz;
				}
				break;

			case kBezier :
				repCount = (YPathOperatorRepCount)*opz;
				++opz;
				for ( i = 0; i < repCount; i++ )
				{
					pt1 = *ptz++;
					pt2 = *ptz++;
					pt3 = *ptz++;
					rcBounds.AddPointToRect( RIntPoint( pt3 ) );
					RRealRect	rc;
					::GetBezierBounds( pt0, pt1, pt2, pt3, rc );
					rcBounds.Union( rcBounds, RIntRect( rc ) );
					pt0 = pt3;
				}
				break;

			case kQuadraticSpline :
				repCount = (YPathOperatorRepCount)*opz;
				++opz;
				for ( i = 0; i < repCount; i++ )
				{
					pt1 = *ptz++;
					pt2 = *ptz++;
					RIntRect	rc;
					::CalcSplineBoundingRect( pt0, pt1, pt2, rc );
					rcBounds.Union( rcBounds, rc );
					pt0 = pt2;
				}
				break;

			default :
				TpsAssertAlways( "Unrecognized Path operator." );
				break;
		}
	}
	return rcBounds;
}


// ****************************************************************************
//
// Function Name:		RPath::PrepToWarp( )
//
// Description:		Mostly it subdivides the segments in the path so that there
//							are more points to warp.
//
// Returns:				Nothing.
//
// Exceptions:			Memory
//
//
// ****************************************************************************
//
void RPath::PrepToWarp( BOOLEAN fHorizontal ) 
{
EPathOperatorArray				pathOperators;
RRealPointArray					pathPoints;
RRealPointArrayIterator			ptz = m_PathPoints.Start();
EPathOperatorArrayIterator		opz = m_PathOperators.Start();
EPathOperator						pathOperator;
YPointCount							pointCount;
YPathOperatorRepCount			repCount;
int									i, j, nr, count;
RRealPoint							points[64];							// a little over 20 bezier segments
RRealPoint							ptLast, ptStart;
RRealPoint							pt0, pt1, pt2, pt3;
BOOLEAN								fChanged = FALSE;

	while ( ( pathOperator = *opz ) != kEnd )
	{
		++opz;
		count = 0;
		switch ( pathOperator )
		{
			case kMoveTo :
				pathPoints.InsertAtEnd( *ptz );
				ptLast = *ptz;
				ptStart = ptLast;
				++ptz;
				pathOperators.InsertAtEnd( kMoveTo );
				break;

			case kClose :
				if ( ( nr = SubDivideLineSegment( ptLast, ptStart, fHorizontal, points ) ) > 1 )
				{
					for ( j = 1; j < nr; j++ )
						pathPoints.InsertAtEnd( points[j] );
					count += ( nr - 1 );
					pathOperators.InsertAtEnd( kLine );
					pathOperators.InsertAtEnd( (EPathOperator)( count ) );
					fChanged = TRUE;
				}
				ptLast = ptStart;
				pathOperators.InsertAtEnd( kClose );
				break;

			case kLine :
				pointCount = (YPathOperatorRepCount)*opz;
				for ( i = 0; i < pointCount; i++ )
				{
					if ( ( nr = SubDivideLineSegment( ptLast, *ptz, fHorizontal, points ) ) > 1 )
					{
						for ( j = 1; j < nr; j++ )
							pathPoints.InsertAtEnd( points[j] );
						count += ( nr - 1 );
						fChanged = TRUE;
					}
					pathPoints.InsertAtEnd( *ptz );
					ptLast = *ptz;
					++ptz;
				}
				pathOperators.InsertAtEnd( kLine );
				pathOperators.InsertAtEnd( (EPathOperator)( pointCount + count ) );
				++opz;
				break;

			case kBezier :
				repCount = (YPathOperatorRepCount)*opz;
				for ( i = 0; i < repCount; i++ )
				{
					pt0 = ptLast;
					pt1 = *ptz;
					++ptz;
					pt2 = *ptz;
					++ptz;
					pt3 = *ptz;
					++ptz;
					if ( ( nr = SubDivideBezierSegment( pt0, pt1, pt2, pt3, points ) ) > 1 )
					{
						for ( j = 1; j < ( nr * 3 ); j++ )
							pathPoints.InsertAtEnd( points[j] );
						count += ( nr - 1 );
						fChanged = TRUE;
					}
					else
					{
						pathPoints.InsertAtEnd( pt1 );
						pathPoints.InsertAtEnd( pt2 );
					}
					pathPoints.InsertAtEnd( pt3 );
					ptLast = pt3;
				}
				pathOperators.InsertAtEnd( kBezier );
				pathOperators.InsertAtEnd( (EPathOperator)( repCount + count ) );
				++opz;
				break;

			case kQuadraticSpline :
				repCount = (YPathOperatorRepCount)*opz;
				for ( i = 0; i < repCount; i++ )
				{
					pt0 = ptLast;
					pt1 = *ptz;
					++ptz;
					pt2 = *ptz;
					++ptz;
					if ( ( nr = SubDivideQuadraticSplineSegment( pt0, pt1, pt2, points ) ) > 1 )
					{
						for ( j = 1; j < ( nr * 2 ); j++ )
							pathPoints.InsertAtEnd( points[j] );
						count += ( nr - 1 );
						fChanged = TRUE;
					}
					else
					{
						pathPoints.InsertAtEnd( pt1 );
					}
					pathPoints.InsertAtEnd( pt2 );
					ptLast = pt2;
				}
				pathOperators.InsertAtEnd( kQuadraticSpline );
				pathOperators.InsertAtEnd( (EPathOperator)( repCount + count ) );
				++opz;
				break;

			default :
				TpsAssertAlways( "Unrecognized Path operator." );
				break;
		}
	}
	pathOperators.InsertAtEnd( kEnd );
	try
	{
		if ( fChanged )
		{
			m_PathOperators.Empty();
			m_PathPoints.Empty();
			m_TransformedIntPoints.Empty();
			m_TransformedRealPoints.Empty();
			m_PathOperators = pathOperators;
			m_PathPoints = pathPoints;
			CopyPointArray( pathPoints, m_TransformedIntPoints );		// m_TransformedIntPoints = m_PathPoints;
			m_TransformedRealPoints = m_PathPoints;
			m_LastTransform.MakeIdentity();
			CalcBoundingRect();
			m_PathSize = RIntSize( m_BoundingRect.m_Right, m_BoundingRect.m_Bottom );
			TpsAssertValid( this );
		}
	}
	catch ( ... )
	{
		Undefine();
		throw;
	}
}


// ****************************************************************************
//
// Function Name:		RPath::SubDivideLineSegment( )
//
// Description:		Sub divide a line segment based on length.
//
// Returns:				Number of line segments generated.
//
// Exceptions:			None
//
// ****************************************************************************
//
YIntDimension RPath::SubDivideLineSegment( const RRealPoint& ptStart, 
														 const RRealPoint& ptEnd, 
														 BOOLEAN fHorizontal,
														 RRealPoint* points )
{
YIntDimension	length = ::Round( ptStart.Distance( ptEnd ) );
YIntDimension	count = 0;
RRealPoint		pt0, pt1, pt2, pt3, pt4, pt5, pt6;

	*points = ptStart;
	points++;
	if ( length > 120 )
		if ( ptStart.m_x != ptEnd.m_x || !fHorizontal )
		{
			pt0 = ptStart;
			pt4 = ptEnd;
			::midpoint( pt2, pt0, pt4 );
			if ( length > 240 )
			{
				::midpoint( pt1, pt0, pt2 );
				::midpoint( pt3, pt2, pt4 );
				if ( length > 600 )
				{
					::midpoint( pt5, pt0, pt1 );
					*points = pt5;
					points++;
					count++;
				}
				*points = pt1;
				points++;
				count++;
				if ( length > 360 )
				{
					if ( length > 600 )
					{
						::midpoint( pt5, pt1, pt2 );
						*points = pt5;
						points++;
						count++;
					}
					*points = pt2;
					points++;
					count++;
					if ( length > 600 )
					{
						::midpoint( pt6, pt2, pt3 );
						*points = pt6;
						points++;
						count++;
					}
				}
				*points = pt3;
				points++;
				count++;
				if ( length > 600 )
				{
					::midpoint( pt6, pt3, pt4 );
					*points = pt6;
					points++;
					count++;
				}
			}
			else
			{
				*points = pt2;
				points++;
				count++;
			}
		}
	*points = ptEnd;
	points++;
	count++;

	return count;
}

// ****************************************************************************
//
// Function Name:		RPath::SubDivideBezierSegment( )
//
// Description:		Sub divide a bezier segment based on length and infection.
//
// Returns:				Number of bezier segments generated.
//
// Exceptions:			None
//
// ****************************************************************************
//
YIntDimension RPath::SubDivideBezierSegment( const RRealPoint& ptStart, 
															const RRealPoint& ptControl1, 
															const RRealPoint& ptControl2, 
															const RRealPoint& ptEnd, 
															RRealPoint* points )
{
YIntDimension	length = ApproxBezierLength( ptStart, ptControl1, ptControl2, ptEnd );
RRealPoint		pt0 = ptStart;
RRealPoint		pt1 = ptControl1;
RRealPoint		pt2 = ptControl2;
RRealPoint		pt3 = ptEnd;
RRealPoint		pts[ 64 ];
YIntDimension	nr, count = 0;
int				i;

	*points = ptStart;
	points++;
	if ( length < 240 )
	{
		if ( ::LineSegmentsIntersect(  pt0, pt3, pt1, pt2 ) )
		{
			::SubdivideBezier( pt0, pt1, pt2, pt3, pts );
			for ( i = 1; i < 6; i++ )
			{
				*points = pts[i];
				points++;
			}
			count++;
		}
		else
		{
			*points = ptControl1;
			points++;
			*points = ptControl2;
			points++;
		}
	}
	else
	{
		nr = ( ( length / 240 ) / 4 ) * 4;												// even is necessary
		if ( nr == 0 )
			nr = 2;  
		else if ( nr > 20 )																	// limits are good
			nr = 20;
		count += ( nr - 1 );
		::SubdivideBezier( pt0, pt1, pt2, pt3, pts, nr );
		for ( i = 1; i < ( nr * 3 ); i++ )
		{
			*points = pts[i];
			points++;
		}
	}
	*points = ptEnd;
	points++;
	count++;

	return count;
}

// ****************************************************************************
//
// Function Name:		RPath::SubDivideQuadraticSplineSegment( )
//
// Description:		Sub divide a quadratic spline segment based on length.
//
// Returns:				Number of quadratic spline segments generated.
//
// Exceptions:			None
//
// ****************************************************************************
//
YIntDimension RPath::SubDivideQuadraticSplineSegment( const RRealPoint& ptStart, 
																		const RRealPoint& ptControl, 
																		const RRealPoint& ptEnd, 
																		RRealPoint* points )
{
YIntDimension	length = ApproxQuadraticSplineLength( ptStart, ptControl, ptEnd );
RRealPoint		pt0 = ptStart;
RRealPoint		pt1 = ptControl;
RRealPoint		pt2 = ptEnd;
RRealPoint		pts[ 64 ];
YIntDimension	nr, count = 0;
int				i;

	*points = ptStart;
	points++;
	if ( length < 240 )
	{
		*points = ptControl;
		points++;
	}
	else
	{
		nr = ( ( length / 240 ) / 4 );			
		if ( ( nr & 0x01) != 0 )															// even is necessary
			nr++;
		nr *= 4;  
		if ( nr == 0 )
			nr = 2;  
		else if ( nr > 20 )																	// limits are good
			nr = 20;
		count += ( nr - 1 );
		::SubdivideQuadraticSpline( pt0, pt1, pt2, pts, nr );
		for ( i = 1; i < ( nr * 2 ); i++ )
		{
			*points = pts[i];
			points++;
		}
	}
	*points = ptEnd;
	points++;
	count++;

	return count;
}

// ****************************************************************************
//
// Function Name:		RPath::GetOperatorCount( )
//
// Description:		Get the path operator count
//
// Returns:				Number of operator array entries used
//
// Exceptions:			None
//
//
// ****************************************************************************
//
YPathOperatorArraySize RPath::GetOperatorCount( ) const
{
	TpsAssertValid( this );
	return GetOperatorCount( &(*m_PathOperators.Start()) );
}

// ****************************************************************************
//
// Function Name:		RPath::GetOperatorCount( )
//
// Description:		Count the number of operator array entries used
//
// Returns:				Number of operator array entries used
//
// Exceptions:			None
//
//
// ****************************************************************************
//
YPathOperatorArraySize RPath::GetOperatorCount( EPathOperator* pOperators )
{
YPathOperatorArraySize	opCount = 0;
EPathOperator				pathOperator;
	
	while( ( pathOperator = *pOperators++ ) != kEnd )
	{
		switch ( pathOperator )
		{
			case kClose :
			case kMoveTo :
				opCount++;
				break;
			case kLine :
			case kBezier :
			case kQuadraticSpline :
				pOperators++;
				opCount += 2;
				break;
			default :
				TpsAssertAlways( "Unrecognized Path operator." );
				break;
		}
	}
	opCount++;

	return opCount;
}

// ****************************************************************************
//
// Function Name:		RPath::GetStartPoint( )
//
// Description:		Get the start point of the path
//
// Returns:				RIntPoint
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RRealPoint RPath::GetStartPoint( ) const
{
	TpsAssertValid( this );
	return *m_TransformedRealPoints.Start();
}

// ****************************************************************************
//
// Function Name:		RPath::GetEndPoint( )
//
// Description:		Get the end point of the path
//
// Returns:				RIntPoint
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RRealPoint RPath::GetEndPoint( ) const
{
EPathOperatorArrayIterator		opz = m_PathOperators.Start();
RRealPointArrayIterator			ptz = m_TransformedRealPoints.End();
BOOLEAN								fClosed = FALSE;

	TpsAssertValid( this );
	while ( *opz != kEnd )
	{
		switch ( *opz )
		{
			case kMoveTo :
				fClosed = FALSE;
				break;
			case kClose :
				fClosed = TRUE;
				break;
			case kLine :
			case kBezier :
			case kQuadraticSpline :
				++opz;
				break;
			default :
				TpsAssertAlways( "Unrecognized Path operator." );
				break;
		}
		++opz;
	}

	if ( fClosed )
		return *m_TransformedRealPoints.Start();
	--ptz;
	return *ptz;
}

// ****************************************************************************
//
// Function Name:		RPath::SetPathSize( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPath::SetPathSize( const RIntSize& size )
{
	TpsAssert( ( ( m_PathSize.m_dx - 1 ) <= size.m_dx && ( m_PathSize.m_dy - 1 ) <= size.m_dy ), "It is inadvisable to resize a path to a smaller size." );
	m_PathSize = size;
}

// ****************************************************************************
//
// Function Name:		RPath::GetSegmentOfPath( )
//
// Description:		Get the points defining the segment specified
//
// Returns:				Path operator indicating type of segment found.
//
// Exceptions:			None
//
// ****************************************************************************
//
EPathOperator RPath::GetSegmentOfPath( short segment, RRealPoint* points ) const
{
RRealPointArrayIterator		iPtIterator;
EPathOperatorArrayIterator	iOpIterator;
EPathOperator					pathOperator;
YPointCount						pointCount;
YPathOperatorRepCount		repCount;
RRealPoint						ptLast, ptStart;
int								i;

	// parse path to find segment in which point resides
	iPtIterator = m_TransformedRealPoints.Start(); 
	iOpIterator = m_PathOperators.Start(); 
	while( ( pathOperator = *iOpIterator ) != kEnd )
	{
		++iOpIterator;
		switch( pathOperator )
		{
			case kMoveTo :
				ptLast = ptStart = *iPtIterator;
				++iPtIterator;
				break;

			case kClose :
				if ( ptLast != ptStart )
				{
					segment--;
					if ( segment <= 0 )
					{
						points[0] = ptLast;
						points[1] = ptStart;
						pathOperator = kLine;
						break;
					}
				}
				break;

			case kLine :
				pointCount = (YPathOperatorRepCount)*iOpIterator;
				++iOpIterator;
				for ( i = 0; i < pointCount; i++ )
				{
					segment--;
					if ( segment <= 0 )
					{
						points[0] = ptLast;
						points[1] = *iPtIterator;
						break;
					}
					ptLast = *iPtIterator;
					++iPtIterator;
				}
				break;

			case kBezier :
				repCount = (YPathOperatorRepCount)*iOpIterator;
				++iOpIterator;
				for ( i = 0; i < repCount; i++ )
				{
					segment--;
					if ( segment <= 0 )
					{
						points[0] = ptLast;
						points[1] = *iPtIterator;
						++iPtIterator;
						points[2] = *iPtIterator;
						++iPtIterator;
						points[3] = *iPtIterator;
						break;
					}
					else
						iPtIterator += 2;
					ptLast = *iPtIterator;
					++iPtIterator;
				}
				break;

			case kQuadraticSpline :
				repCount = (YPathOperatorRepCount)*iOpIterator;
				++iOpIterator;
				for (i = 0; i < repCount; i++ )
				{
					segment--;
					if ( segment <= 0 )
					{
						points[0] = ptLast;
						points[1] = *iPtIterator;
						++iPtIterator;
						points[2] = *iPtIterator;
					}
					else
						iPtIterator += 2;
					ptLast = *iPtIterator;
					++iPtIterator;
				}
				break;

			default :
				TpsAssertAlways( "Unrecognized Path operator." );
				break;
		}
		if ( segment <= 0 )
			break;
	}

	return pathOperator;
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RPath::Validate( )
//
// Description:		Validates the object
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPath::Validate( ) const
{
	TpsAssert( ( m_PathOperators.Count() != 0 ), "Path not defined." );
	TpsAssert( ( m_PathPoints.Count() != 0 ), "Path not defined." );
	TpsAssert( ( m_TransformedIntPoints.Count() != 0 ), "Path not defined." );
	TpsAssert( ( m_TransformedRealPoints.Count() != 0 ), "Path not defined." );
	TpsAssert( ( m_PathPoints.Count() == m_TransformedIntPoints.Count() ), "Path ill defined." );
	TpsAssert( ( m_PathPoints.Count() == m_TransformedRealPoints.Count() ), "Path ill defined." );
	TpsAssert( ( *m_PathOperators.Start() == kMoveTo ), "Paths must start with a moveto." );
	TpsAssert( ( m_OutlineCount > 0 ), "No outlines defined." );
	TpsAssert( ( m_OutlineCount <= kMaxPathPolygons ), "Too many outlines defined." );
	TpsAssert( ( m_PathPoints.Count() < 8189 ), "Paths must be less than 32k." );
	RObject::Validate();
	TpsAssertIsObject( RPath, this );
}

#endif	// TPSDEBUG

// ****************************************************************************
//
// Function Name:		RPath::FindXSegment( )
//
// Description:		
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
//
EPathOperator RPath::FindXSegment( YRealCoordinate x, RRealPoint* pptSegment )
{
YRealCoordinate	xMin;
YRealCoordinate	xMax;
EPathOperator		eop;
int					nSegments	= m_SegmentLengths.Count();

	for ( int iSegment = 0 ; iSegment < nSegments ; ++iSegment )
	{
		int	ipt1	= 0;
		eop	= GetSegmentOfPath( ( iSegment + 1 ), pptSegment );
		switch ( eop )
		{
			case kLine:
				ipt1	= 1;
				break;
			case kBezier:
				ipt1	= 3;
				break;
			case kQuadraticSpline:
				ipt1	= 2;
				break;
			default:
				TpsAssertAlways( "Invalid segment operator." );
				eop				= kLine;
				pptSegment[1]	= pptSegment[0];
				break;
		}
		if ( pptSegment[0].m_x < pptSegment[ipt1].m_x )
		{
			xMin	= pptSegment[0].m_x;
			xMax	= pptSegment[ipt1].m_x;
		}
		else
		{
			xMin	= pptSegment[ipt1].m_x;
			xMax	= pptSegment[0].m_x;
		}
		if ( ( x >= xMin ) && ( x <= xMax ) )
			return eop;
	}
	TpsAssertAlways( "Couldn't find the requested segment" );
	return kLine; 
}

// ****************************************************************************
//
// Function Name:		RPath::FindYSegment( )
//
// Description:		
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
//
EPathOperator RPath::FindYSegment( YRealCoordinate y, RRealPoint* pptSegment )
{
YRealCoordinate	yMin;
YRealCoordinate	yMax;
EPathOperator		eop;
int					nSegments	= m_SegmentLengths.Count();

	for( int iSegment = 0 ; iSegment < nSegments ; ++iSegment )
	{
		int	ipt1	= 0;
		eop	= GetSegmentOfPath( ( iSegment + 1 ), pptSegment );
		switch( eop )
		{
			case kLine:
				ipt1	= 1;
				break;
			case kBezier:
				ipt1	= 3;
				break;
			case kQuadraticSpline:
				ipt1	= 2;
				break;
			default:
				TpsAssertAlways( "Invalid segment operator." );
				eop				= kLine;
				pptSegment[1]	= pptSegment[0];
				break;
		}
		if( pptSegment[0].m_y < pptSegment[ipt1].m_y )
		{
			yMin	= pptSegment[0].m_y;
			yMax	= pptSegment[ipt1].m_y;
		}
		else
		{
			yMin	= pptSegment[ipt1].m_y;
			yMax	= pptSegment[0].m_y;
		}
		if ( ( y >= yMin ) && ( y <= yMax ) )
			return eop;
	}
	TpsAssertAlways( "Couldn't find the requested segment" );
	return kLine; 
}

// ****************************************************************************
//
//  Function Name:	CopyPointArray( )
//
//  Description:		
//							
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RPath::CopyPointArray( const RIntPointArray& src, RRealPointArray& dst )
{
	dst.Empty();
	::Append( dst, src.Start(), src.End() );
}

// ****************************************************************************
//
//  Function Name:	CopyPointArray( )
//
//  Description:		
//							
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RPath::CopyPointArray( const RRealPointArray& src, RIntPointArray& dst )
{
	dst.Empty();
	::Append( dst, src.Start(), src.End() );
}

// ****************************************************************************
//
//  Function Name:	RPath::RenderPathPoints( )
//
//  Description:		Draws this Path
//
//  Returns:			Number of operator array entries used
//				//  Exceptions:		None
//
// ****************************************************************************
//
YPathOperatorArraySize RPath::RenderPathPoints( RDrawingSurface& drawingSurface,
						  										EPathOperator* pOperators,
						  										RIntPoint* pPathPoints,
						  										RRealPoint* pRealPathPoints,
						  										YFillMethod fillMethod,
						  										BOOLEAN,
						  										const RIntSize& )
{
	YPathOperatorArraySize	opCount = 0;
	YPointCountArray			pointCounts( kMaxPathPolygons );

	// if the drawing surface supports paths, draw the Path using the path API
	if( drawingSurface.HasPathSupport() )
		opCount = DrawPath( drawingSurface, pOperators, pPathPoints, fillMethod );
	else
	{
		// Calculate the Path points
		RIntPoint*	pptPath;
		BOOLEAN		fClip = TRUE;
		RIntRect		rClip	= drawingSurface.GetClipRect( );
		opCount = CalcPathPoints( pOperators, pRealPathPoints, pptPath, pointCounts, fClip, rClip );

		// Draw the points
		DrawPathPoints( drawingSurface, fillMethod, pptPath, pointCounts );

		::ReleaseGlobalBuffer( reinterpret_cast<uBYTE*>(pptPath) );
	}
#if	defined( TPSDEBUG ) && defined( RAH_TEST_CODE )
#ifdef	IMAGER
	if ( gfShowPathPoints )
#endif	
		MarkPathPoints( drawingSurface, pOperators, pPathPoints );
#endif	
	return opCount;
}

// ****************************************************************************
//
// Function Name:		RPath::DrawPathPoints( )
//
// Description:		Draws a the Path into the given drawing surface, using
//							the correct fill method.
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPath::DrawPathPoints( RDrawingSurface& drawingSurface,
									 YFillMethod fillMethod,
									 RIntPoint* pPolygonPoints,
									 YPointCountArray& pointCountArray )
{
	YPolygonCount	polygonCount = pointCountArray.Count();
	//	If entire polygon was clipped (no point counts) just return
	if ( polygonCount == 0 )
		return;

	switch( fillMethod )
		{
		case kFilled:
			{
			if ( polygonCount == 1 )
				drawingSurface.FillPolygon( pPolygonPoints, *(pointCountArray.Start()) );
			else
				drawingSurface.FillPolyPolygon( pPolygonPoints, pointCountArray, polygonCount );
			break;
			}

		case kStroked:
			{
			if ( polygonCount == 1 )
				drawingSurface.Polyline( pPolygonPoints, *(pointCountArray.Start()) );
			else
				drawingSurface.PolyPolyline( pPolygonPoints, pointCountArray, polygonCount );
			break;
			}

		case kFilledAndStroked:
			{
			// Stroked and filled. For each polygon, if the end point is the same as
			// the start point, we can safely call PolyPolygon. Otherwise we have to
			// Call FillPolyPolygon and PolyPolyLine.
			BOOLEAN		fAllPolysClosed	= TRUE;
			YPointCount	idxPolyStart		= 0;

			for( YPolygonCount i = 0; i < polygonCount; ++i )
				{
				YPointCount	nPolyPoints	= pointCountArray.Start()[i];
				if( pPolygonPoints[idxPolyStart] != pPolygonPoints[idxPolyStart + nPolyPoints - 1] )
					{
					fAllPolysClosed	= FALSE;
					break;
					}
				idxPolyStart += nPolyPoints;
				}

			if( fAllPolysClosed )
				drawingSurface.PolyPolygon( pPolygonPoints, pointCountArray, polygonCount );
			else
				{
				drawingSurface.FillPolyPolygon( pPolygonPoints, pointCountArray, polygonCount );
				drawingSurface.PolyPolyline( pPolygonPoints, pointCountArray, polygonCount );
				}
			break;
			}

		case kStrokedAndFilled:
			{
			drawingSurface.PolyPolyline( pPolygonPoints, pointCountArray, polygonCount );
			drawingSurface.FillPolyPolygon( pPolygonPoints, pointCountArray, polygonCount );
			break;
			}

		case kGradFilled:
			{
			drawingSurface.FillPolyPolygon( pPolygonPoints, pointCountArray, polygonCount );
			break;
			}

		case kGradFilledAndStroked:
			{
			drawingSurface.FillPolyPolygon( pPolygonPoints, pointCountArray, polygonCount );
			drawingSurface.PolyPolyline( pPolygonPoints, pointCountArray, polygonCount );
			break;
			}

		case kStrokedAndGradFilled:
			{
			drawingSurface.PolyPolyline( pPolygonPoints, pointCountArray, polygonCount );
			drawingSurface.FillPolyPolygon( pPolygonPoints, pointCountArray, polygonCount );
			break;
			}

		default :
			TpsAssertAlways( "Unrecognized fill method." );
			break;
		}
}

// ****************************************************************************
//
//  Function Name:	RPath::CalcPathPoints( )
//
//  Description:		Calculates the points of a Path
//
//  Returns:			Number of operator array entries used
//
//  Exceptions:		None
//
// ****************************************************************************
//
YPathOperatorArraySize RPath::CalcPathPoints( EPathOperator* pOperators,
															 RRealPoint* pPathPoints,
															 RIntPoint*& pPolygonPoints,
															 YPointCountArray& pointCountArray,
															 BOOLEAN	fClip,
															 const RIntRect& rClip )
{
	TpsAssert( *(EPathOperator*)pOperators == kMoveTo, "Path objects must begin with a MoveTo." );

	BOOLEAN		fRetried		= FALSE;
	YPointCount	nMaxPoints	= YPointCount( ::GetGlobalBufferSize()/sizeof(RIntPoint) );
	pPolygonPoints	= reinterpret_cast<RIntPoint*>( ::GetGlobalBuffer() );

	DoItAgain:

	// Setup
	YPointCount					pointCount	= 0;
	YPointCount					totalPoints	= 0;
	BOOLEAN						fInObject = FALSE;
	BOOLEAN						fWasMove = FALSE;
	YPathOperatorArraySize	opCount = 1;
	EPathOperator				pathOperator;
	EPathOperator*				parOps	= pOperators;
	RRealPoint*					parPts	= pPathPoints;
	RIntPoint*					pptPoly	= pPolygonPoints;

	pointCountArray.Empty();		//	just incase we move to a static buffer

	while( (pathOperator=*parOps++) != kEnd )
	{
		switch( pathOperator )
		{
			case kMoveTo :
				// if we are currently in an object, end it
				if( fInObject )
				{
					pointCountArray.InsertAtEnd( pointCount );
					totalPoints	+= pointCount;
					pointCount	= 0;
					TpsAssert( pointCountArray.Count() <= kMaxPathPolygons, "Too many polygons!!" );
				}

				// if the last operation was also a move, factor it out
				else if( fWasMove )
				{
					pPolygonPoints--;
					pointCount--;
				}

				// Copy the point
				if( totalPoints < nMaxPoints )
					*pptPoly++ = *parPts;

				//
				// on to the next path point
				++parPts;

				// Increment the point count
				++pointCount;

				fWasMove = TRUE;
				fInObject = FALSE;
				break;

			case kClose :
				// If we are in an object, close and end it
				if( fInObject )
				{
					TpsAssert( (totalPoints+pointCount)>0, "kClose with no points" );

					if( totalPoints < nMaxPoints )
						{
						*pptPoly = *(pptPoly-pointCount);
						++pptPoly;
						}
					//
					// bump the point count...
					++pointCount;

					//	If we are clipping points, check bounds to see if it intersects the clipping region
					BOOLEAN	fAddPolygon	= TRUE;
					if ( fClip )
						{
						int			i;
						RIntPoint*	pTmpPoints	= (pptPoly-pointCount);
						RIntRect		rPoly(*pTmpPoints, *pTmpPoints);
						for ( i = 1, ++pTmpPoints; i < pointCount; ++i, ++pTmpPoints )
							rPoly.AddPointToRect( *pTmpPoints );	

						if ( !rPoly.IsIntersecting(rClip) )
							fAddPolygon = FALSE;
						}

					if ( fAddPolygon )
						{
						pointCountArray.InsertAtEnd( pointCount );
						totalPoints	+= pointCount;
						TpsAssert( pointCountArray.Count() <= kMaxPathPolygons, "Too many polygons!!" );
						}
					else
						{	//	Remove the points from the poly polygon being created
						pptPoly -= pointCount;
						}
					pointCount	= 0;
				}

				fWasMove = FALSE;
				fInObject = FALSE;
				break;

			case kLine :
			{
				YPointCount lineCount = (YPathOperatorRepCount)*parOps++;

				for( YPointCount i = 0; i < lineCount; i++ )
				{
					// Add the end point
					if( (totalPoints+pointCount) < nMaxPoints )
						*pptPoly++ = *parPts;

					// Increment the point count
					++parPts;
					++pointCount;
				}
				fWasMove		= FALSE;
				fInObject	= TRUE;
				++opCount;
				break;
			}

			case kBezier :
			{
				TpsAssert( (totalPoints+pointCount)>0, "kBezier with no previous point" );
				YPathOperatorRepCount	repCount = (YPathOperatorRepCount)*parOps++;
				YPointCount	numExpanded;
				RRealPoint*	pptExpanded = RDrawingSurface::ExpandPolyBezier( *(parPts-1), parPts, repCount*3, numExpanded );
				RRealPoint*	ppt = pptExpanded+1;
				pointCount	+= (numExpanded-1);
				while( --numExpanded > 0 && (totalPoints+pointCount)<nMaxPoints )
					{
					*pptPoly++ = *ppt;
					++ppt;
					}
				::ReleaseGlobalBuffer( (uBYTE*)pptExpanded );
				parPts		+= (3*repCount);
				fWasMove		= FALSE;
				fInObject	= TRUE;
				++opCount;
				break;
			}

			case kQuadraticSpline :
			{
				TpsAssert( (totalPoints+pointCount)>0, "kQuadraticSpline with no previous point" );
				YPathOperatorRepCount repCount = (YPathOperatorRepCount)*parOps++;
				for( YPathOperatorRepCount i = 0; i < repCount; i++ )
				{
					FillQuadraticSpline( *(parPts-1), *parPts, *(parPts+1), pptPoly, pointCount, nMaxPoints-totalPoints );
					parPts += 2;
				}
				fWasMove		= FALSE;
				fInObject	= TRUE;
				++opCount;
				break;
			}

			default :
				TpsAssert( NULL, "Unrecognized Path operator." );
		}
		++opCount;
	}
	//
	// if we overflowed, start over with the real count...
	if( (totalPoints+pointCount) > nMaxPoints )
		{
		//
		// need to try again...
		::ReleaseGlobalBuffer( reinterpret_cast<uBYTE*>(pPolygonPoints) );
		if( fRetried )
			{
			//
			// oh no...
			throw kMemory;
			}
		else
			{
			nMaxPoints		= (totalPoints+pointCount);
			pPolygonPoints	= reinterpret_cast<RIntPoint*>( ::GetGlobalBuffer(uLONG(nMaxPoints)*sizeof(RIntPoint)) );
			fRetried			= TRUE;
			goto DoItAgain;
			}
		}

	// If we were in an object, add it to the count
	if( fInObject )
		pointCountArray.InsertAtEnd( pointCount );

	return opCount;
}

// ****************************************************************************
//
//  Function Name:	RPath::DrawPath( )
//
//  Description:		Sets up to draw paths.
//
//  Returns:			Number of operator array entries used
//
//  Exceptions:		None
//
// ****************************************************************************
//
YPathOperatorArraySize RPath::DrawPath( RDrawingSurface& drawingSurface,
							 						 EPathOperator* pOperators,
							 						 RIntPoint* pPathPoints,
							 						 YFillMethod fillMethod )
{
YPathOperatorArraySize		opCount = 0;

	switch( fillMethod )
		{
		case kFilled:
			{
			drawingSurface.BeginPath();
			opCount = DoPath( drawingSurface, pOperators, pPathPoints );
			drawingSurface.EndPath();
			drawingSurface.FillPath();
			break;
			}

		case kStroked:
			{
			BOOLEAN	fClosed = IsClosed( pOperators );
			if ( fClosed )
				drawingSurface.BeginPath();
			else
				drawingSurface.BeginOpenPath();
			opCount = DoPath( drawingSurface, pOperators, pPathPoints );
			if ( fClosed )
				drawingSurface.EndPath();
			else
				drawingSurface.EndOpenPath();
			drawingSurface.FramePath();
			break;
			}

		case kFilledAndStroked:
			{
			BOOLEAN	fClosed = IsClosed( pOperators );
			if ( fClosed )
				drawingSurface.BeginPath();
			else
				drawingSurface.BeginOpenPath();
			opCount = DoPath( drawingSurface, pOperators, pPathPoints );
			if ( fClosed )
				drawingSurface.EndPath();
			else
				drawingSurface.EndOpenPath();
			drawingSurface.FillAndFramePath();
			break;
			}

		case kStrokedAndFilled:
			{
			BOOLEAN	fClosed = IsClosed( pOperators );
			if ( fClosed )
				drawingSurface.BeginPath();
			else
				drawingSurface.BeginOpenPath();
			opCount = DoPath( drawingSurface, pOperators, pPathPoints );
			if ( fClosed )
				drawingSurface.EndPath();
			else
				drawingSurface.EndOpenPath();
			drawingSurface.FrameAndFillPath();
			break;
			}

		case kGradFilled:
			{
			RDrawingSurfaceState	PreserveClip( &drawingSurface );
			drawingSurface.BeginPath();
			opCount = DoPath( drawingSurface, pOperators, pPathPoints );
			drawingSurface.EndPath();
			drawingSurface.IntersectClipPath();
			RIntRect	rcDrawArea	= drawingSurface.GetClipRect();
			drawingSurface.FillRectangle( rcDrawArea );
			break;
			}

		case kGradFilledAndStroked:
			{
			BOOLEAN			fClosed = IsClosed( pOperators );
			if( fClosed )	drawingSurface.BeginPath();
			else				drawingSurface.BeginOpenPath();
			opCount = DoPath( drawingSurface, pOperators, pPathPoints );
			if( fClosed )	drawingSurface.EndPath();
			else				drawingSurface.EndOpenPath();
			RDrawingSurfaceState	PreservePath( &drawingSurface );
			drawingSurface.IntersectClipPath();
			RIntRect	rcDrawArea	= drawingSurface.GetClipRect();
			drawingSurface.FillRectangle( rcDrawArea );
			PreservePath.Restore();
			drawingSurface.FramePath();
			break;
			}

		case kStrokedAndGradFilled:
			{
			BOOLEAN			fClosed = IsClosed( pOperators );
			if( fClosed )	drawingSurface.BeginPath();
			else				drawingSurface.BeginOpenPath();
			opCount = DoPath( drawingSurface, pOperators, pPathPoints );
			if( fClosed )	drawingSurface.EndPath();
			else				drawingSurface.EndOpenPath();
			RDrawingSurfaceState	PreservePath( &drawingSurface );
			drawingSurface.FramePath();
			PreservePath.Restore();
			drawingSurface.IntersectClipPath();
			RIntRect	rcDrawArea	= drawingSurface.GetClipRect();
			drawingSurface.FillRectangle( rcDrawArea );
			break;
			}

		default :
			TpsAssertAlways( "Unrecognized fill method." );
			break;
		}

	return opCount;
}

// ****************************************************************************
//
//  Function Name:	RPath::DoPath( )
//
//  Description:		Draws a Path using the path APIs
//
//  Returns:			Number of operator array entries used
//
//  Exceptions:		None
//
// ****************************************************************************
//
YPathOperatorArraySize RPath::DoPath( RDrawingSurface& drawingSurface,
						  						  EPathOperator* pOperators,
						 						  RIntPoint* pPathPoints )
{
	TpsAssert( *(EPathOperator*)pOperators == kMoveTo, "Path objects must begin with a MoveTo." );

	RIntPoint startPoint;
	YPathOperatorArraySize	opCount = 1;
	EPathOperator PathOperator;

	while( ( PathOperator = *( (EPathOperator*&)pOperators )++ ) != kEnd )
	{
		switch( PathOperator )
		{
			case kMoveTo :
				drawingSurface.MoveTo( *pPathPoints );

				// Save the start point for a close
				startPoint = *pPathPoints++;
				break;

			case kClose :
				drawingSurface.LineTo( startPoint );
				break;

			case kLine :
			{
				YPointCount pointCount = (YPathOperatorRepCount)*( (EPathOperator*&)pOperators )++;
				if ( pointCount == 1 )
					drawingSurface.LineTo( *pPathPoints );
				else
					drawingSurface.PolylineTo( pPathPoints, pointCount );
				pPathPoints += pointCount;
				opCount++;
				break;
			}

			case kBezier :
			{
				YPathOperatorRepCount repCount = (YPathOperatorRepCount)*( (EPathOperator*&)pOperators )++;
				drawingSurface.PolyBezierTo( pPathPoints, repCount * 3 );
				pPathPoints += repCount * 3;
				opCount++;
				break;
			}

			case kQuadraticSpline :
			{
				YPathOperatorRepCount repCount = (YPathOperatorRepCount)*( (EPathOperator*&)pOperators )++;
				drawingSurface.PolyQuadraticSplineTo( pPathPoints, repCount * 2 );
				pPathPoints += repCount * 2;
				opCount++;
				break;
			}

			default :
				TpsAssert( NULL, "Unrecognized Path operator." );
		}
		opCount++;
	}

	return opCount;
}

// ****************************************************************************
//
//  Function Name:	RPath::MarkPathPoints( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPath::MarkPathPoints( RDrawingSurface& drawingSurface,
						  			 EPathOperator* pOperators,
						 			 RIntPoint* pPathPoints )
{
RIntRect  					rcPoint;
RIntSize  					szPoint;
YPathOperatorArraySize	opCount = 1;
EPathOperator 				PathOperator;
int 							i;

	TpsAssert( *(EPathOperator*)pOperators == kMoveTo, "Path objects must begin with a MoveTo." );
	while( ( PathOperator = *( (EPathOperator*&)pOperators )++ ) != kEnd )
	{
		switch( PathOperator )
		{
			case kMoveTo :
  				szPoint = RIntSize( 3, 3 );
				rcPoint.m_TopLeft = *pPathPoints;
				rcPoint.m_BottomRight = *pPathPoints;
				rcPoint.Inflate( szPoint );
				drawingSurface.FillRectangle( rcPoint );
				pPathPoints++;
  				szPoint = RIntSize( 2, 2 );
				break;

			case kClose :
				break;

			case kLine :
			{
				YPointCount pointCount = (YPathOperatorRepCount)*( (EPathOperator*&)pOperators )++;
				for ( i = 0; i < pointCount; i++ )
				{
					rcPoint.m_TopLeft = *pPathPoints;
					rcPoint.m_BottomRight = *pPathPoints;
					rcPoint.Inflate( szPoint );
					drawingSurface.FillRectangle( rcPoint );
					pPathPoints++;
				}
				opCount++;
				break;
			}

			case kBezier :
			{
				YPathOperatorRepCount repCount = (YPathOperatorRepCount)*( (EPathOperator*&)pOperators )++;
				for ( i = 0; i < repCount; i++ )
				{
					rcPoint.m_TopLeft = *pPathPoints;
					rcPoint.m_BottomRight = *pPathPoints;
					rcPoint.Inflate( szPoint );
					drawingSurface.FrameRectangle( rcPoint );
					pPathPoints++;
					rcPoint.m_TopLeft = *pPathPoints;
					rcPoint.m_BottomRight = *pPathPoints;
					rcPoint.Inflate( szPoint );
					drawingSurface.FrameRectangle( rcPoint );
					pPathPoints++;
					rcPoint.m_TopLeft = *pPathPoints;
					rcPoint.m_BottomRight = *pPathPoints;
					rcPoint.Inflate( szPoint );
					drawingSurface.FillRectangle( rcPoint );
					pPathPoints++;
				}
				opCount++;
				break;
			}

			case kQuadraticSpline :
			{
				YPathOperatorRepCount repCount = (YPathOperatorRepCount)*( (EPathOperator*&)pOperators )++;
				for ( i = 0; i < repCount; i++ )
				{
					rcPoint.m_TopLeft = *pPathPoints;
					rcPoint.m_BottomRight = *pPathPoints;
					rcPoint.Inflate( szPoint );
					drawingSurface.FrameRectangle( rcPoint );
					pPathPoints++;
					rcPoint.m_TopLeft = *pPathPoints;
					rcPoint.m_BottomRight = *pPathPoints;
					rcPoint.Inflate( szPoint );
					drawingSurface.FillRectangle( rcPoint );
					pPathPoints++;
				}
				opCount++;
				break;
			}

			default :
				TpsAssert( NULL, "Unrecognized Path operator." );
		}
		opCount++;
	}
}

void RPath::Read( RArchive& storage )
{
	RRealPoint ptPathPoint;
	uWORD uwPathOperator;

	Undefine();

	uLONG ulIndex;
	uLONG ulCount;

	storage >> ulCount;

	for (ulIndex = 0; ulIndex < ulCount; ulIndex++)
	{
		storage >> uwPathOperator;
		m_PathOperators.InsertAtEnd( (EPathOperator) uwPathOperator );
	}

	storage >> ulCount;

	for (ulIndex = 0; ulIndex < ulCount; ulIndex++)
	{
		storage >> ptPathPoint;
		m_PathPoints.InsertAtEnd( ptPathPoint );
	}

//	archive >> m_OutlineCount;
	storage >> m_PathSize;
	storage >> m_BoundingRect;
	storage >> m_LastTransform;

	m_OutlineCount = 1;
	CopyPointArray( m_PathPoints, m_TransformedIntPoints );		// m_TransformedIntPoints = m_PathPoints;
	m_TransformedRealPoints = m_PathPoints;
	m_LastTransform.MakeIdentity();
//	m_PathSize = RIntSize( m_BoundingRect.m_Right, m_BoundingRect.m_Bottom );
	TpsAssertValid( this );
}

void RPath::Write( RArchive& storage ) const
{
	uLONG ulIndex;
	uLONG ulCount;

	ulCount = m_PathOperators.Count();
	storage << ulCount;

	for (ulIndex = 0; ulIndex < ulCount; ulIndex++)
	{
		uWORD uwValue = (uWORD) m_PathOperators[ulIndex];
		storage << uwValue;
	}

	ulCount = m_PathPoints.Count();
	storage << ulCount;

	for (ulIndex = 0; ulIndex < ulCount; ulIndex++)
		storage << m_PathPoints[ulIndex];

//	archive << m_PathOperators;
//	archive << m_PathPoints;
//	archive << m_OutlineCount;
	storage << m_PathSize;
	storage << m_BoundingRect;
	storage << m_LastTransform;
}

// ****************************************************************************
//
// Function Name:		CalcSplineBoundingRect( )
//
// Description:		Calculates the bounding box of the qspline
//
// Returns:				nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void CalcSplineBoundingRect( const RIntPoint& q0, const RIntPoint& q1, const RIntPoint& q2, RIntRect& rc )
{
	RRealPoint	bzpts[ 4 ];
	RIntPoint	qpts[ 3 ];
	RRealRect	rrc;
	qpts[0]	= q0;
	qpts[1]	= q1;
	qpts[2]	= q2;
	::CreateBezierFromSpline( &qpts[ 0 ], bzpts );
	::GetBezierBounds( bzpts[0], bzpts[1], bzpts[2], bzpts[3], rrc );
	rc = rrc;
}

// ****************************************************************************
//
// Function Name:		operator<<
//
// Description:		Insertion operator for rectangles
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
RArchive& operator<<( RArchive& archive, const RPath& path )
	{
	path.Write( archive );

	return archive;
	}

// ****************************************************************************
//
// Function Name:		operator>>
//
// Description:		Extraction operator for rectangles
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
RArchive& operator>>( RArchive& archive, RPath& path )
	{
	path.Read( archive );

	return archive;
	}

/////////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
//
//  Function Name:	RClippingPath::RClippingPath( )
//
//  Description:		Default constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RClippingPath::RClippingPath( BOOLEAN fPathFromImport ) :
	m_fPathFromImport( fPathFromImport )
{
}

// ****************************************************************************
//
// Function Name:		RClippingPath::RClippingPath( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RClippingPath::RClippingPath( uLONG grows, BOOLEAN fPathFromImport ) : 
	RPath( grows ),
	m_fPathFromImport( fPathFromImport )
{
}

// ****************************************************************************
//
//  Function Name:	RPath::RPath( )
//
//  Description:		Copy constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RClippingPath::RClippingPath( const RClippingPath &rhs ) :
	RPath( (const RPath&) rhs )
{
	m_fPathFromImport = rhs.m_fPathFromImport;
}

// ****************************************************************************
//
//  Function Name:	RClippingPath::RClippingPath( )
//
//  Description:		Copy constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RClippingPath::RClippingPath( const RClippingPath &rhs, const R2dTransform& transform ) :
	RPath( (const RPath&) rhs, transform )
{
	m_fPathFromImport = rhs.m_fPathFromImport;
}

// ****************************************************************************
//
//  Function Name:	RClippingPath::RClippingPath( )
//
//  Description:		Copy constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RClippingPath::RClippingPath( const RPath &rhs, const R2dTransform& transform, BOOLEAN fPathFromImport ) :
	RPath( (const RPath&) rhs, transform )
{
	m_fPathFromImport = fPathFromImport;
}

// ****************************************************************************
//
//  Function Name:	RClippingPath::operator =( )
//
//  Description:		Assignment operator.
//
//  Returns:			reference to this object
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RClippingPath& RClippingPath::operator=( const RClippingPath& rhs )
{
	if ( this != &rhs )
	{
		RPath::operator=( (const RPath&) rhs );
		m_fPathFromImport = rhs.m_fPathFromImport;
	}

	return *this;
}

// ****************************************************************************
//
//  Function Name:	RClippingPath::operator =( )
//
//  Description:		Assignment operator.
//
//  Returns:			reference to this object
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
/*RClippingPath& RClippingPath::operator=( const RPath& rhs )
{
	if ( this != &rhs )
	{
		RPath::operator=( (const RPath&) rhs );
		m_fPathFromImport = FALSE;
	}

	return *this;
}
*/

// ****************************************************************************
//
//  Function Name:	RClippingPath::Read( )
//
//  Description:		Read this clipping path from the given storage object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RClippingPath::Read( RArchive& archive )
{
	RPath::Read( archive );

// GCB 5/17/98 - fix bug that prevented opening old project files with objects
// having a clipping path but without the new m_fPathFromImport field
#if 0
	archive >> m_fPathFromImport;
#else
	// default m_fPathFromImport to TRUE - note that if any QSLs were created
	// prior to the addition of this field the user won't be able to edit the
	// clipping path regardless of whether it was a user-defined clipping path
	m_fPathFromImport = TRUE;

	RChunkStorage* pChunkStorage = dynamic_cast<RChunkStorage*>(&archive);
	if (pChunkStorage)
	{
		YStreamLength yChunkSize = pChunkStorage->GetCurrentChunkSize();
		YStreamLength yChunkPos = pChunkStorage->GetPosition();
		// don't complain if the m_fPathFromImport field is not there since the
		// project could have been saved prior to the addition of this field
		if (yChunkPos <= yChunkSize - 1)
			archive >> m_fPathFromImport;
	}
#endif
}

// ****************************************************************************
//
//  Function Name:	RClippingPath::Read( )
//
//  Description:		Write this clipping path to the given storage object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RClippingPath::Write( RArchive& archive ) const
{
	RPath::Write( archive );
	archive << m_fPathFromImport;
}

// ****************************************************************************
//
// Function Name:		operator<<
//
// Description:		Insertion operator for rectangles
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
RArchive& operator<<( RArchive& archive, const RClippingPath& path )
	{
	path.Write( archive );

	return archive;
	}

// ****************************************************************************
//
// Function Name:		operator>>
//
// Description:		Extraction operator for rectangles
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
RArchive& operator>>( RArchive& archive, RClippingPath& path )
	{
	path.Read( archive );

	return archive;
	}
