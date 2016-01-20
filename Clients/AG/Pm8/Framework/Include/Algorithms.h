// ****************************************************************************
//
//  File Name:			Algorithms.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of Various Algorithm templates
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
//  $Logfile:: /PM8/Framework/Include/Algorithms.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_ALGORITHMS_H_
#define		_ALGORITHMS_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

#define __AssertFile__ __FILE__

typedef	YCounter 	YCollectionIndex;
template<class obj>	class RArray;

// ****************************************************************************
//
//  Function Name:	Append( )
//
//  Description:		Append all of the item from one list onto the other
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory (if items can not be added)
//
// ****************************************************************************
//
template <class TCollection, class TIter> void Append( TCollection& collection, const TIter& start, const TIter& end )
	{
	TIter	iterator	= start;
	while ( iterator != end )
		{
		collection.InsertAtEnd( *iterator );
		++iterator;
		}
	}

// ****************************************************************************
//
//  Function Name:	::DotProduct( )
//
//  Description:		Computes the dot product between two vectors (sizes)
//
//  Returns:			the dot product (x1*x2 + y1*y2)
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T>
inline T DotProduct( const RSize<T>& vec1, const RSize<T>& vec2 )
{
	return vec1.m_dx*vec2.m_dx + vec1.m_dy*vec2.m_dy;
}

// ****************************************************************************
//
//  Function Name:	::PerpDotProduct( )
//
//  Description:		Computes the perp dot product between two vectors (sizes)
//
//  Returns:			the perp dot product (-y1*x2 + x1*y2)
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T>
inline T PerpDotProduct( const RSize<T>& vec1, const RSize<T>& vec2 )
{
	return DotProduct( RSize<T>(-vec1.m_dy,vec1.m_dx), vec2 );
}

// ****************************************************************************
//
//  Function Name:	::PerpDotProduct( )
//
//  Description:		Computes the perp dot product between two vectors
//
//  Returns:			the perp dot product (-y1*x2 + x1*y2)
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T>
inline T PerpDotProduct( const RPoint<T>& ptVertex, const RPoint<T>& ptLeg1, const RPoint<T>& ptLeg2 )
{
	return DotProduct( RSize<T>(ptVertex.m_y-ptLeg1.m_y,ptLeg1.m_x-ptVertex.m_x),
							 RSize<T>(ptLeg2.m_x-ptVertex.m_x,ptLeg2.m_y-ptVertex.m_y) );
}

// ****************************************************************************
//
//  Function Name:	FindMatch( )
//
//  Description:		Find the occurance of the given data and that matches the given
//							data.
//
//  Returns:			Iterator pointing to the first item in the range that matches
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TIter, class TData, class MatchProc >
TIter FindMatch( TIter start, const TIter& end, const TData& data, MatchProc match )
	{
	//	Until we are at the end of the list
	while (start != end)
		{
		if (match( *start, data ))
			break;
		++start;
		}
		return start;
	}

// ****************************************************************************
//
//  Function Name:	FindMatch( )
//
//  Description:		Find the occurance of the given data and that matches the given
//							data. This version assumes that the values can be tested
//							for equality.
//
//  Returns:			Iterator pointing to the first item in the range that matches
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TIter, class TData >
TIter FindMatch( TIter start, const TIter& end, const TData& data )
	{
	//	Until we are at the end of the list
	while (start != end)
		{
		if (*start ==data)
			break;
		++start;
		}
		return start;
	}

// ****************************************************************************
//
//  Function Name:	Count( )
//
//  Description:		Count the number of items in the given range.
//
//  Returns:			YCounter as the number of items;
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TIter> inline YCounter Count( TIter start, const TIter& end )
	{
		YCounter count = 0;
		while (start++ != end)
			++count;
		return count;
	}

// ****************************************************************************
//
//  Function Name:	CountIf( )
//
//  Description:		Count the number of items in the given range,
//							but only if the given function returns true on
//							the specific item.
//
//  Returns:			YCounter as the number of items that pass the IfProc
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TIter, class IfProc> YCounter CountIf( TIter start, const TIter& end, IfProc ifproc )
	{
		YCounter count = 0;
		while (start != end)
			{
			if (ifproc(*start))
				++count;
			++start;
			}
		return count;
	}

// ****************************************************************************
//
//  Function Name:	ForEach( )
//
//  Description:		Call the given function for each item in the given
//							range.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TIter, class DoProc> void ForEach( TIter start, const TIter& end, DoProc doproc )
	{
		while (start != end)
			doproc(*start++);
	}

// ****************************************************************************
//
//  Function Name:	ForEachIf( )
//
//  Description:		Call the given function for each item in the given
//							range, but only if the IfProc returns true.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TIter, class DoProc, class IfProc> void ForEachIf( TIter start, const TIter& end, DoProc doproc, IfProc ifproc )
	{
		while (start != end)
			{
			if (ifproc(*start))
				doproc(*start);
			++start;
			}
	}

// ****************************************************************************
//
//  Function Name:	UninitializedCopy( )
//
//  Description:		Copies the objects from start to dst using the Placement
//							new operator.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TIter> void UninitializedCopy( TIter start, const TIter& end, TIter dst )
	{
	while (start != end)
		{
		new ((void*)&(*dst)) TIter::YContainerType( *start );
		dst++;
		start++;
		}
	}

// ****************************************************************************
//
//  Function Name:	CopyItemsForward( )
//
//  Description:		Copy the given range to the destination in a source to end
//							direction
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TIter> void CopyItemsForwards( TIter start, const TIter& end, TIter dst )
	{
	while (start != end)
		*dst++ = *start++;
	}

// ****************************************************************************
//
//  Function Name:	CopyItemsForward( )
//
//  Description:		Copy the given range to the destination in a end to source
//							direction
//							
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TIter> void CopyItemsBackwards( TIter end, TIter dst, const TIter& start )
	{
	while (start != end)
		*--dst = *--end;
	}

// ****************************************************************************
//
//  Function Name:	Sort( )
//
//  Description:		Sort the given range of data given a comparison function.
//							Will perform an insertion sort.
//
//							The CompareProc must return an integer value that can be
//							compared in the following way.
//							<  0 - The data must be after (no movement occurs)
//							==	0 - The data values are the same (no movement occurs
//							>  0 - The data must be before the tested value,
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TIter, class CompareProc> void Sort( const TIter& start, const TIter& end, CompareProc compare )
	{
	//	Get the simple case out of the way
	if (start == end)
		return;

	//	Initialize the 'current iterator'
	TIter		previousIter	= start;
	TIter		currentIter		= previousIter + 1;
	//	Loop until the current iterator is the end
	while (currentIter != end)
		{
		//	Compare the two locations
		if (compare( *previousIter, *currentIter ) > 0)
			{
			// the currentItem is less than the previous item, we must
			//	loop backwards to find the new insertion point and "Shuffle"
			//	the iterators
			while (previousIter != start)
				{
				--previousIter;
				if (compare( *previousIter, *currentIter ) <= 0)
					{
					//	Current item is >= than previous, so insert it must be
					//	inserted after the 'previous' one, increment and break
					++previousIter;
					break;
					}
				}
				TIter::YContainerType	data = *currentIter;
				CopyItemsBackwards( currentIter, currentIter+1, previousIter );
				*previousIter = data;
				//	If current was insert before start, reset start
			}
		//	Else setup for next iteration pointer
		previousIter = currentIter;
		++currentIter;
		}

	}

// ****************************************************************************
//
//  Function Name:	GetIndex( )
//
//  Description:		Gets the index of the specified object within the
//							specified collection
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TCollection, class TObject>
YCollectionIndex GetIndex( const TCollection& collection, const TObject& object )
	{
	YCollectionIndex index = 0;

	TCollection::YIterator iterator = collection.Start( );
	for( ; iterator != collection.End( ); ++iterator, ++index )
		if( *iterator == object )
			return index;

	TpsAssertAlways( "Object not found" );
	return 0;
	}

// ****************************************************************************
//
//  Function Name:	GetByIndex( )
//
//  Description:		Gets the object within the	specified collection with the
//							specified index number
//
//  Returns:			See above
//
//  Exceptions:		kUnknown
//
// ****************************************************************************
//
template <class TCollection, class TObject>
void GetByIndex( const TCollection& collection, YCollectionIndex index, TObject& object )
	{
	TpsAssert( index < collection.Count( ), "Bad index." );

	TCollection::YIterator iterator = collection.Start( );
	for( ; iterator != collection.End( ); ++iterator, --index )
		{
		if( index == 0 )
			{
			object = *iterator;
			return;
			}
		}

	TpsAssertAlways( "Why are we here?" );
	}
							
// ****************************************************************************
//
//  Function Name:	DeleteAllItems( )
//
//  Description:		Calls the destructor on ALL items assuming that
//							they derive from RObject.  First, delete all of the objects
//							next call Empty.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TCollection> void DeleteAllItems( TCollection& collection )
	{
	//	First, call the destructor of everything
	TCollection::YIterator	iterator		= collection.Start( );
	TCollection::YIterator	iteratorEnd	= collection.End( );
	for( ; iterator != iteratorEnd; ++iterator )
		{
		delete *iterator;
		}

	collection.Empty( );
	}

// ****************************************************************************
//
//  Function Name:	Reverse( )
//
//  Description:		Reverse the order of all items between the two iterators.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory (if items can not be added)
//
// ****************************************************************************
//
template <class TIter> void Reverse( const TIter& start, const TIter& end )
	{
	YCounter						i;
	TIter::YContainerType	tmpData;

	TIter			iterLow		= start;
	TIter			iterHigh		= end;
	YCounter		halfCount	= Count( iterLow, iterHigh ) / 2;

	for (i = 0; i < halfCount; ++i)
		{
		--iterHigh;		//	Decrement since high is one past object to operate on
		tmpData		= *iterHigh;	//	copy high
		*iterHigh	= *iterLow;		//	replace high with low
		*iterLow		= tmpData;		//	replace low with old high
		++iterLow;		//	increment low to next value
		}
	}

// ****************************************************************************
//
//  Function Name:	SameSign( )
//
//  Description:		Determines if two numbers have the same sign
//
//  Returns:			TRUE if the two numbers have the same sign
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> BOOLEAN SameSign( T a, T b )
	{
	if( a > 0 && b > 0 )
		return TRUE;

	if( a < 0 && b < 0 )
		return TRUE;

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	TestRangeFor0to1( )
//
//  Description:		Determine of the result of a devision of numerator / denomenator
//							will be in the range [ 0, 1 ]
//
//  Returns:			TRUE if the range is between [ 0, 1 ]
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> BOOLEAN TestRangeFor0to1( T numerator, T denominator )
	{
	if ( denominator > (T)0 )
		{
		//	If signes are different or numer>denom so must be > 1
		if ( ( numerator < 0 ) || ( numerator > denominator ) )
			return FALSE;
		}
	else if ( denominator < (T)0 )
		{
		//	If signes are different or numer<denom so must be > 1
		if ( ( numerator > 0 ) || ( numerator < denominator ) )
			return FALSE;
		}

	//	denominator of 0 implies collinatity for below and that means point is in range...
	//	so return true
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	LinesIntersect( )
//
//  Description:		Determines if two line segments intersect, and optionally
//                   the point of intersection...
//
//							This is an implementation of the algorithm described in
//							Graphics Gems II (p7-9). If necessary there is a faster,
//							but harder to understand, algorithm described in Graphics
//							Gems III (p199-202).
//
//							By default, The algorithm from GGIII is used, to use the
//							algorithm from GGII comment USE_ALGORITHM_3
//
//  Returns:			TRUE if the line segments intersect
//
//  Exceptions:		None
//
// ****************************************************************************
//
#define	USE_ALGORITHM_3	TRUE
template <class T> BOOLEAN LineSegmentsIntersect(  const RPoint<T>& point1,
																	const RPoint<T>& point2,
																	const RPoint<T>& point3,
																	const RPoint<T>& point4 )
	{
#ifdef	USE_ALGORITHM_3
	//
	//	usually a futher optimization is performed by compairing the bounding rectangles of point1,point2 and point3,point4
	//	however, since we cannot gaurentee that point1,point2 or point3,point4 form a valid rectangle as such, the number
	//	of additional computations was determined to not be that benificial.  This may be reevaluated in the future if necessary

	//	First form some intermedary values for easier computation and better readability (with the algorithm)
	RPoint<T>	A(	point2.m_x - point1.m_x, point2.m_y - point1.m_y );
	RPoint<T>	B( point3.m_x - point4.m_x, point3.m_y - point4.m_y );
	RPoint<T>	C( point1.m_x - point3.m_x, point1.m_y - point3.m_y );
	T				denominator = ( ( A.m_y * B.m_x ) - ( A.m_x * B.m_y ) );
	//
	//	If denominator is 0, points are collinear, Assume they don't meet
	if ( denominator == (T)0 )
		return FALSE;
	//
	T	numeratorA	= ( ( B.m_y * C.m_x ) - ( B.m_x * C.m_y ) );
	//
	//	If solution for Alpha is not in range 0,1. The lines do not intersect
	if ( !TestRangeFor0to1( numeratorA, denominator ) )
		return FALSE;
	//
	T	numeratorB	= ( ( A.m_x * C.m_y ) - ( A.m_y * C.m_x ) );
	//
	//	If solution for Beta is not in range 0,1. The lines do not intersect
	if ( !TestRangeFor0to1( numeratorB, denominator ) )
		return FALSE;

#else

	// Plug point3 into the equation of the line formed by point1 and point2
	T r3 = ( ( point3.m_x - point1.m_x ) * ( point2.m_y - point1.m_y ) ) - ( ( point3.m_y - point1.m_y ) * ( point2.m_x - point1.m_x ) );
	if( r3 == 0 )
		return TRUE;

	// Now plug point4 into the equation of the line formed by point1 and point2
	T r4 = ( ( point4.m_x - point1.m_x ) * ( point2.m_y - point1.m_y ) ) - ( ( point4.m_y - point1.m_y ) * ( point2.m_x - point1.m_x ) );
	if( r4 == 0 )
		return TRUE;

	// If the two results have the same sign, then point3 and point4 lie on the same side
	// of the line formed by point1 and point2. They dont intersect.
	if( SameSign( r3, r4 ) )
		return FALSE;

	// Plug point1 into the equation of the line formed by point3 and point4
	T r1 = ( ( point1.m_x - point3.m_x ) * ( point4.m_y - point3.m_y ) ) - ( ( point1.m_y - point3.m_y ) * ( point4.m_x - point3.m_x ) );
	if( r1 == 0 )
		return TRUE;

	// Now plug point2 into the equation of the line formed by point3 and point4
	T r2 = ( ( point2.m_x - point3.m_x ) * ( point4.m_y - point3.m_y ) ) - ( ( point2.m_y - point3.m_y ) * ( point4.m_x - point3.m_x ) );
	if( r2 == 0 )
		return TRUE;

	// If the two results have the same sign, then point1 and point2 lie on the same side
	// of the line formed by point3 and point4. They dont intersect.
	if( SameSign( r1, r2 ) )
		return FALSE;
#endif

	// Otherwise the line segments must intersect
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	GetLineSegmentIntersection( )
//
//  Description:		Determines if two line segments intersect, and computes
//                   the point of intersection...
//
//							This is an implementation of the algorithm described in
//							Graphics Gems III (p199-202).
//
//  Returns:			TRUE if the line segments intersect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> BOOLEAN GetLineSegmentIntersection(	const RPoint<T>& point1,
																			const RPoint<T>& point2,
																			const RPoint<T>& point3,
																			const RPoint<T>& point4,
																			RPoint<T>&		  ptIntersection,
																			BOOLEAN			  fOnlyWithinSegments=TRUE )
	{
	//
	//	usually a futher optimization is performed by compairing the bounding rectangles of point1,point2 and point3,point4
	//	however, since we cannot gaurentee that point1,point2 or point3,point4 form a valid rectangle as such, the number
	//	of additional computations was determined to not be that benificial.  This may be reevaluated in the future if necessary

	//	First form some intermedary values for easier computation and better readability (with the algorithm)
	RPoint<T>	A(	point2.m_x - point1.m_x, point2.m_y - point1.m_y );
	RPoint<T>	B( point3.m_x - point4.m_x, point3.m_y - point4.m_y );
	RPoint<T>	C( point1.m_x - point3.m_x, point1.m_y - point3.m_y );
	T				denominator = ( ( A.m_y * B.m_x ) - ( A.m_x * B.m_y ) );
	//
	//	If denominator is 0, points are collinear, Assume they don't meet
	if ( denominator == (T)0 )
		return FALSE;
	//
	T	numeratorA	= ( ( B.m_y * C.m_x ) - ( B.m_x * C.m_y ) );
	//
	//	If solution for Alpha is not in range 0,1. The lines do not intersect
	if ( fOnlyWithinSegments && !TestRangeFor0to1( numeratorA, denominator ) )
		return FALSE;
	//
	T	numeratorB	= ( ( A.m_x * C.m_y ) - ( A.m_y * C.m_x ) );
	//
	//	If solution for Beta is not in range 0,1. The lines do not intersect
	if ( fOnlyWithinSegments && !TestRangeFor0to1( numeratorB, denominator ) )
		return FALSE;

	//
	// They intersect, so compute the actual intersection point
	ptIntersection	= RPoint<T>( point1.m_x + numeratorA*A.m_x/denominator, point1.m_y + numeratorA*A.m_y/denominator );
	return TRUE;
	}


// ****************************************************************************
//
//  Function Name:	::PointsAreCollinear( )
//
//  Description:		Determins if three points are collinear (ie. they fall
//							on the same line. )
//
//  Returns:			TRUE if the points are collinear, FALSE otherwise
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> BOOLEAN inline PointsAreCollinear( T* a, T* b, T* c )
	{
	const T kDelta = (T)2;

	// Form vector ab and vector bc
	T abX = b[ 0 ] - a[ 0 ];
	T abY = b[ 1 ] - a[ 1 ];

	T bcX = c[ 0 ] - b[ 0 ];
	T bcY = c[ 1 ] - b[ 1 ];

	// Now form the dot product of ab perp and bc

	T perpDotProduct = ( abX * bcY ) - ( abY * bcX );

	// If the perp dot product is 0, the points are collinear
	return static_cast<BOOLEAN>( ( perpDotProduct >= -kDelta && perpDotProduct <= kDelta ) );
	}

// ****************************************************************************
//
//  Function Name:	::PointsAreCollinear( )
//
//  Description:		Determins if three points are collinear (ie. they fall
//							on the same line. )
//
//  Returns:			TRUE if the points are collinear, FALSE otherwise
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T> BOOLEAN inline PointsAreCollinear( const RPoint<T>& a, const RPoint<T>& b, const RPoint<T>& c )
	{
	const T kDelta = (T)2;

	// Form vector ab and vector bc
	T	perpDotProduct( ::PerpDotProduct((b-a),(c-b)) );

	// If the perp dot product is 0, the points are collinear
	return static_cast<BOOLEAN>( ( perpDotProduct >= -kDelta && perpDotProduct <= kDelta ) );
	}



const int kFractionalBits = 10;
const int kFractionalHalf = 1 << 7;

// ****************************************************************************
//
//  Function Name:	::FixedPointsAreCollinear( )
//
//  Description:		Fixed point version of ::PointsAreCollinear
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline BOOLEAN FixedPointsAreCollinear( T* a, T* b, T* c )
	{
	const T kDelta = (T)2 << ( kFractionalBits * 2 );

	// Form vector ab and vector bc
	T abX = b[ 0 ] - a[ 0 ];
	T abY = b[ 1 ] - a[ 1 ];

	T bcX = c[ 0 ] - b[ 0 ];
	T bcY = c[ 1 ] - b[ 1 ];

	// Now form the dot product of ab perp and bc
	T perpDotProduct = ( abX * bcY ) - ( abY * bcX );

	// If the perp dot product is 0, the points are collinear
	return static_cast<BOOLEAN>( ( perpDotProduct >= -kDelta && perpDotProduct <= kDelta ) );
	}

// ****************************************************************************
//
//  Function Name:	::SideOfLine( )
//
//  Description:		Determins to which side of a vector a point falls.
//
//  Returns:			< 0 if point is port (left) of vector
//							> 0 if point is starboard (right) of vector
//							  0 if point is on line
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline T SideOfLine( const RPoint<T>& a, const RPoint<T>& b, const RPoint<T>& c )
	{
	RPoint<T>	ab, ac;

	// Form vector ab and vector ac
	ab.m_x = b.m_x - a.m_x;
	ab.m_y = b.m_y - a.m_y;

	ac.m_x = c.m_x - a.m_x;
	ac.m_y = c.m_y - a.m_y;

	// Now form the dot product of ab perp and ac
	return( ( ab.m_x * ac.m_y ) - ( ab.m_y * ac.m_x ) );
	}

// ****************************************************************************
//
//  Function Name:	::PerpendicularLineAtMidPoint( )
//
//  Description:		Computes a line perpendicular to line provided at the mid
//							point of said line.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline void PerpendicularLineAtMidPoint( const RPoint<T>& pt0, const RPoint<T>& pt1, RPoint<T>& pt2, RPoint<T>& pt3 )
	{
	::midpoint( pt2, pt0, pt1 );
	pt3.m_x = pt2.m_x + ( pt1.m_y - pt0.m_y );
	pt3.m_y = pt2.m_y - ( pt1.m_x - pt0.m_x );
	}



// ****************************************************************************
//
//  Function Name:	::TurnsCCW( )
//
//  Description:		determines if the turn from the start leg to the end leg is counterclockwise
//
//  Returns:			TRUE if it's ccw else FALSE (cw or parallel)
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T>
inline BOOLEAN TurnsCCW( const RPoint<T>& ptVertex, const RPoint<T>& ptStartLeg, const RPoint<T>& ptEndLeg )
{
	return BOOLEAN( PerpDotProduct(ptVertex,ptStartLeg,ptEndLeg) > 0 );
}

// ****************************************************************************
//
//  Function Name:	::TurnsCW( )
//
//  Description:		determines if the turn from the start leg to the end leg is clockwise
//
//  Returns:			TRUE if it's cw else FALSE (ccw or parallel)
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T>
inline BOOLEAN TurnsCW( const RPoint<T>& ptVertex, const RPoint<T>& ptStartLeg, const RPoint<T>& ptEndLeg )
{
	return BOOLEAN( PerpDotProduct(ptVertex,ptStartLeg,ptEndLeg) < 0 );
}

// ****************************************************************************
//
//  Function Name:	::AreParellel( )
//
//  Description:		determines if the start leg and the end leg are parallel (or antiparallel)
//
//  Returns:			TRUE if they're parallel (or antiparallel) else FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T>
inline BOOLEAN AreParallel( const RPoint<T>& ptVertex, const RPoint<T>& ptStartLeg, const RPoint<T>& ptEndLeg )
{
	return BOOLEAN( PerpDotProduct(ptVertex,ptStartLeg,ptEndLeg) == 0 );
}
//
// Implementation for real points, since we need to use AreFloatsEqual() to do the test
//
inline BOOLEAN AreParallel( const RRealPoint& ptVertex, const RRealPoint& ptStartLeg, const RRealPoint& ptEndLeg )
{
	return AreFloatsEqual( PerpDotProduct(ptVertex,ptStartLeg,ptEndLeg), 0.0 );
}

// ****************************************************************************
//
//  Function Name:	::Interpolate( )
//
//  Description:		Compute the point on the given line at the given parameter
//
//  Returns:			the point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T>
RPoint<T> Interpolate( const RPoint<T>& pt0, const RPoint<T>& pt1, YFloatType t )
{
	return RPoint<T>( RRealPoint(YRealDimension(pt0.m_x+((pt1.m_x-pt0.m_x)*t)), YRealDimension(pt0.m_y+((pt1.m_y-pt0.m_y)*t))) );
}

// ****************************************************************************
//
//  Function Name:	::BilinearInterpolateValue( )
//
//  Description:		Compute bilinear interpolation value
//
//  Returns:			the bilinear interpolation value
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline T BilinearInterpolateValue( const T& min, const T& midPoint, const T& max, YFloatType t )
{
	if (t < 0.5)
		return T( YRealDimension( min+((midPoint-min)*(t*2.0)) ) );
	else
		return T( YRealDimension( midPoint+((max-midPoint)*((t-0.5)*2.0)) ) );
}

//
// sort the array of floats in ascending order
//
inline void SortFloats( YFloatType* pItem, int nItems )
{
	for( int i=0 ; i<(nItems-1) ; ++i )
		{
		for( int j=i+1 ; j<nItems ; ++j )
			{
			if( pItem[j] < pItem[i] )
				{
				YFloatType	tmp	= pItem[i];
				pItem[i]				= pItem[j];
				pItem[j]				= tmp;
				}
			}
		}
}

//
// sort the array of floats in ascending order, and remove
// any duplicates
// returns the number of unique entries...
//
inline int UniqueFloats( YFloatType* pItem, int nItems )
{
	if( nItems<2 )
		return nItems;

	::SortFloats( pItem, nItems );
	int			nUnique	= 1;
	YFloatType*	pNext		= pItem+1;
	while( --nItems )
		{
		if( AreFloatsEqual(*pItem,*pNext) )
			++pNext;
		else
			{
			++nUnique;
			if( (++pItem) != pNext )
				*pItem = *pNext;
			}
		}
	return nUnique;
}

// ****************************************************************************
//
//  Function Name:	ComputeInteriorAngle( )
//
//  Description:		Determines the interior angle of an arc between angStart and angEnd
//                   passing through angThrough...
//
//  Returns:			the interior angle
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YAngle ComputeInteriorAngle( YAngle angStart, YAngle angThrough, YAngle angEnd )
{
	if( angStart < angEnd )
		{
		//
		// start angle is less than end angle.
		// if through is greater than start and less than end, interior angle increases
		// from start to end else interior angle decreases from start to end wrapping around 0.
		if( angThrough>angStart && angThrough<angEnd )
			{
			//
			// angle increases from start to end.
			return angEnd - angStart;
			}
		else
			{
			//
			// angle decreases from end to start wrapping around 0.
			return angEnd - angStart - k2PI;
			}
		}
	else
		{
		//
		// start angle is greater than end angle.
		// if through is less than start angle and greater than end angle, interior angle decreases
		// from start to end else interior angle increases from start to end wrapping around 0.
		if( angThrough<angStart && angThrough>angEnd )
			{
			//
			// angle decreases from start to end.
			return angEnd - angStart;
			}
		else
			{
			//
			// angle increases from start to end wrapping around 0.
			return angEnd - angStart + k2PI;
			}
		}
}

// ****************************************************************************
//
//  Function Name:	ComputeArcMetrics( )
//
//  Description:		Computes the center of the circle defined by the three points and
//                   the interior angle (signed!) from a0 to a2 passing through a1...
//
//  Returns:			TRUE if the three points form an arc else FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class PointIn, class PointOut>
BOOLEAN ComputeArcMetrics( const PointIn& a0, const PointIn& a1, const PointIn& a2, PointOut& ptCenter, YAngle& angInterior )
{
	PointOut	ptPerpA0A1a;
	PointOut	ptPerpA0A1b;
	PointOut	ptPerpA1A2a;
	PointOut	ptPerpA1A2b;
	PerpendicularLineAtMidPoint( PointOut(a0), PointOut(a1), ptPerpA0A1a, ptPerpA0A1b );
	PerpendicularLineAtMidPoint( PointOut(a1), PointOut(a2), ptPerpA1A2a, ptPerpA1A2b );
	if( GetLineSegmentIntersection(ptPerpA0A1a,ptPerpA0A1b,ptPerpA1A2a,ptPerpA1A2b,ptCenter,BOOLEAN(FALSE)) )
		{
		//
		// Got the center.
		// Compute the interior angle...
		angInterior = ComputeInteriorAngle( ::atan2(double(a0.m_y-ptCenter.m_y),double(a0.m_x-ptCenter.m_x)),
														::atan2(double(a1.m_y-ptCenter.m_y),double(a1.m_x-ptCenter.m_x)),
														::atan2(double(a2.m_y-ptCenter.m_y),double(a2.m_x-ptCenter.m_x)) );
		return TRUE;
		}
	//
	// points don't form an arc...
	return FALSE;
}

#undef __AssertFile__


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_ALGORITHMS_H_

