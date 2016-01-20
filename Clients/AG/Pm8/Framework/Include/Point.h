// ****************************************************************************
//
//  File Name:			Point.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RPoint class
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
//  $Logfile:: /PM8/Framework/Include/Point.h                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_POINT_H_
#define		_POINT_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RArchive;

template <class T> class RPoint;
typedef	RPoint<YIntCoordinate>	RIntPoint;
typedef	RPoint<YRealCoordinate>	RRealPoint;

// ****************************************************************************
//
//  Class Name:		RPoint
//
//  Description:		Two dimensional parameterized point class.
//
// ****************************************************************************
//
template <class T>
class RPoint
	{
	// Construction & destruction
	public :
												RPoint( );
												RPoint( T x, T y );
												RPoint( const RRealPoint& rhs );
												RPoint( const RIntPoint& rhs );

	// Operators
	public :
		RPoint<T>&							operator=( const RRealPoint& rhs );
		RPoint<T>&							operator=( const RIntPoint& rhs );

		BOOLEAN 								operator!=(const RPoint<T>& rpt) const;
		BOOLEAN 								operator==(const RPoint<T>& rpt) const;

		RPoint<T>							operator-( const RSize<T>& rhs ) const;
		RSize<T>								operator-( const RPoint<T>& rhs ) const;
		RPoint<T>							operator+( const RSize<T>& rhs ) const;
		RPoint<T>							operator+( const RPoint<T>& rhs ) const;
		RPoint<T>&							operator+=( const RSize<T>& rhs );
		RPoint<T>&							operator+=( const RPoint<T>& rhs );
		RPoint<T>&							operator-=( const RSize<T>& rhs );
		RPoint<T>&							operator-=( const RPoint<T>& rhs );

		RPoint<T>&							operator*=( const R2dTransform& rhs );
		RPoint<T>							operator*( const R2dTransform& rhs ) const;

	// Operations
	public :
		RPoint<T>&							Scale( const RRealSize& scaleFactor );
		YRealDimension						Distance( ) const;
		YRealDimension						Distance( const RPoint<T>& rhs ) const;
		YRealDimension						DistanceToLine( const RPoint<T>& l0, const RPoint<T>& l1 ) const;

	// Member data
	public :
		T										m_x;
		T										m_y;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RPoint::RPoint( )
//
//  Description:		Default constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T>::RPoint( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RPoint::RPoint( )
//
//  Description:		Constructs a point given two coordinates
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T>::RPoint( T x, T y ) : m_x( x ), m_y( y )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RPoint::RPoint( )
//
//  Description:		Copy constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T>::RPoint( const RPoint<YIntCoordinate>& rhs ) : m_x( rhs.m_x ), m_y( rhs.m_y )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RPoint::RPoint( )
//
//  Description:		Copy constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RPoint<YRealCoordinate>::RPoint( const RPoint<YRealCoordinate>& rhs ) : m_x( rhs.m_x ), m_y( rhs.m_y )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RPoint::RPoint( )
//
//  Description:		Copy constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RPoint<YIntCoordinate>::RPoint( const RPoint<YRealCoordinate>& rhs )
	{
	m_x = ::Round( rhs.m_x );
	m_y = ::Round( rhs.m_y );
	}

// ****************************************************************************
//
//  Function Name:	RPoint::operator=( )
//
//  Description:		Copy operator
//
//  Returns:			Reference to this point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline RPoint<T>& RPoint<T>::operator=( const RPoint<YIntCoordinate>& rhs )
	{
	m_x = rhs.m_x;
	m_y = rhs.m_y;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RPoint::operator=( )
//
//  Description:		Copy operator
//
//  Returns:			Reference to this point
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RPoint<YRealCoordinate>& RPoint<YRealCoordinate>::operator=( const RPoint<YRealCoordinate>& rhs )
	{
	m_x = rhs.m_x;
	m_y = rhs.m_y;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RPoint::operator=( )
//
//  Description:		Copy operator
//
//  Returns:			Reference to this point
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RPoint<YIntCoordinate>& RPoint<YIntCoordinate>::operator=( const RPoint<YRealCoordinate>& rhs )
	{
	m_x = ::Round( rhs.m_x );
	m_y = ::Round( rhs.m_y );
	return *this;
	}

/// ****************************************************************************
//
//  Function Name:	RPoint::operator!=( )
//
//  Description:		returns TRUE if the points are NOT the same
//
//  Returns:			The new point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline BOOLEAN RPoint<T>::operator!=(const RPoint<T>& rpt) const
	{
	return (BOOLEAN)!operator==(rpt);
	}

// ****************************************************************************
//
//  Function Name:	RPoint::operator==( )
//
//  Description:		returns TRUE if the points are the same
//
//  Returns:			The new point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline BOOLEAN RPoint<T>::operator==(const RPoint<T>& rpt) const
	{
	return (BOOLEAN)((m_x == rpt.m_x) && (m_y == rpt.m_y));
	}

inline BOOLEAN RPoint<YRealCoordinate>::operator==( const RPoint<YRealCoordinate>& rhs ) const
	{
	return static_cast<BOOLEAN>( ::AreFloatsEqual( m_x, rhs.m_x ) && ::AreFloatsEqual( m_y, rhs.m_y ) );
	}

// ****************************************************************************
//
//  Function Name:	RPoint::operator-( )
//
//  Description:		Subtracts the given size from this point
//
//  Returns:			The new point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T> RPoint<T>::operator-( const RSize<T>& rhs ) const
	{
	return RPoint<T>( m_x - rhs.m_dx, m_y - rhs.m_dy );
	}

// ****************************************************************************
//
//  Function Name:	RPoint::operator-( )
//
//  Description:		Subtracts the given point from this point
//
//  Returns:			A size object representing the difference between the 
//							two points.
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RSize<T> RPoint<T>::operator-( const RPoint<T>& rhs ) const
	{
	return RSize<T>( m_x - rhs.m_x, m_y - rhs.m_y );
	}

// ****************************************************************************
//
//  Function Name:	RPoint::operator+( )
//
//  Description:		Adds the given size from this point
//
//  Returns:			The new point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T> RPoint<T>::operator+( const RSize<T>& rhs ) const
	{
	return RPoint<T>( m_x + rhs.m_dx, m_y + rhs.m_dy );
	}

// ****************************************************************************
//
//  Function Name:	RPoint::operator+( )
//
//  Description:		Adds the given Point from this point
//
//  Returns:			The new point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T> RPoint<T>::operator+( const RPoint<T>& rhs ) const
	{
	return RPoint<T>( m_x + rhs.m_x, m_y + rhs.m_y );
	}

/// ****************************************************************************
//
//  Function Name:	RPoint::operator+=( )
//
//  Description:		Adds the given size to this point
//
//  Returns:			Reference to this point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T>& RPoint<T>::operator+=( const RSize<T>& rhs )
	{
	m_x += rhs.m_dx;
	m_y += rhs.m_dy;
	return *this;
	}

/// ****************************************************************************
//
//  Function Name:	RPoint::operator+=( )
//
//  Description:		Adds the given point to this point
//
//  Returns:			Reference to this point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T>& RPoint<T>::operator+=( const RPoint<T>& rhs )
	{
	m_x += rhs.m_x;
	m_y += rhs.m_y;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RPoint::operator-=( )
//
//  Description:		Adds the given size to this point
//
//  Returns:			Reference to this point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T>& RPoint<T>::operator-=( const RSize<T>& rhs )
	{
	m_x -= rhs.m_dx;
	m_y -= rhs.m_dy;
	return *this;
	}

// ****************************************************************************
//
// Function Name:		RPoint::operator=*( )
//
// Description:		Multiplies this point by the given transform
//
// Returns:				Reference to this point
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
RPoint<T>& RPoint<T>::operator*=( const R2dTransform& rhs )
	{
	T tempX = (T)( ( m_x * rhs.m_a1 ) + ( m_y * rhs.m_b1 ) + rhs.m_c1 );
	T tempY = (T)( ( m_x * rhs.m_a2 ) + ( m_y * rhs.m_b2 ) + rhs.m_c2 );

	m_x = tempX;
	m_y = tempY;
	return *this;
	}

// ****************************************************************************
//
// Function Name:		RPoint::operator*=( )
//
// Description:		Multiplies this point by the given transform
//
//							Specialized version for ints that rounds
//
// Returns:				Reference to this point
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RPoint<YIntCoordinate>& RPoint<YIntCoordinate>::operator*=( const R2dTransform& rhs )
	{
	YIntCoordinate tempX = (YIntCoordinate)::Round( ( ( m_x * rhs.m_a1 ) + ( m_y * rhs.m_b1 ) + rhs.m_c1 ) );
	YIntCoordinate tempY = (YIntCoordinate)::Round( ( ( m_x * rhs.m_a2 ) + ( m_y * rhs.m_b2 ) + rhs.m_c2 ) );

	m_x = tempX;
	m_y = tempY;
	return *this;
	}

// ****************************************************************************
//
// Function Name:		RPoint::operator*( )
//
// Description:		Returns a point that is the product of this point and the
//							given transform
//
// Returns:				The transformed point
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T> RPoint<T>::operator*( const R2dTransform& rhs ) const
	{
	RPoint<T> temp;
	temp.m_x = (T)( ( m_x * rhs.m_a1 ) + ( m_y * rhs.m_b1 ) + rhs.m_c1 );
	temp.m_y = (T)( ( m_x * rhs.m_a2 ) + ( m_y * rhs.m_b2 ) + rhs.m_c2 );
	return temp;
	}

// ****************************************************************************
//
// Function Name:		RPoint::operator*( )
//
// Description:		Returns a point that is the product of this point and the
//							given transform
//
//							Specialized version for ints that rounds
//
// Returns:				The transformed point
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RPoint<YIntCoordinate> RPoint<YIntCoordinate>::operator*( const R2dTransform& rhs ) const
	{
	RPoint<YIntCoordinate> temp;
	temp.m_x = (YIntCoordinate)::Round( ( ( m_x * rhs.m_a1 ) + ( m_y * rhs.m_b1 ) + rhs.m_c1 ) );
	temp.m_y = (YIntCoordinate)::Round( ( ( m_x * rhs.m_a2 ) + ( m_y * rhs.m_b2 ) + rhs.m_c2 ) );
	return temp;
	}

// ****************************************************************************
//
// Function Name:		RPoint::operator-=( )
//
// Description:		Subtracts a point from this point
//
// Returns:				Reference to this point
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T>& RPoint<T>::operator-=( const RPoint<T>& rhs )
	{
	m_x -= rhs.m_x;
	m_y -= rhs.m_y;
	return *this;
	}

// ****************************************************************************
//
// Function Name:		RPoint::Scale
//
// Description:		Multiplies this point by a scaling factor
//
// Returns:				Reference to this point
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T>& RPoint<T>::Scale( const RRealSize& scaleFactor )
	{
	m_x *= scaleFactor.m_dx;
	m_y *= scaleFactor.m_dy;
	return *this;
	}

// ****************************************************************************
//
// Function Name:		RPoint::Scale
//
// Description:		Multiplies this point by a scaling factor
//
//							Specialized version for ints that rounds
//
// Returns:				Reference to this point
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RPoint<YIntCoordinate>& RPoint<YIntCoordinate>::Scale( const RRealSize& scaleFactor )
	{
	m_x = ::Round( m_x * scaleFactor.m_dx );
	m_y = ::Round( m_y * scaleFactor.m_dy );
	return *this;
	}

// ****************************************************************************
//
// Function Name:		RPoint::Distance
//
// Description:		Computes the distance between this point and the given
//							point
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
YRealDimension RPoint<T>::Distance( const RPoint<T>& rhs ) const
	{
	YRealDimension dx = rhs.m_x - m_x;
	YRealDimension dy = rhs.m_y - m_y;

	return ::sqrt( ( dx * dx ) + ( dy * dy ) );
	}

// ****************************************************************************
//
// Function Name:		RPoint::Distance
//
// Description:		Computes the distance between this point and the origin
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
YRealDimension RPoint<T>::Distance( ) const
	{
	YRealDimension dx = -m_x;
	YRealDimension dy = -m_y;

	return ::sqrt( ( dx * dx ) + ( dy * dy ) );
	}

// ****************************************************************************
//
// Function Name:		RPoint::DistanceToLine
//
// Description:		Computes the distance between this point and the given
//							line (perpendicular distance, line extended)
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
YRealDimension RPoint<T>::DistanceToLine( const RPoint<T>& l0, const RPoint<T>& l1 ) const
	{
	YRealDimension	nDistance( 0.0 );
	RSize<T>			delta0( l0 - *this );
	RSize<T>			delta1( l1 - *this );

	YRealDimension	dd( YRealDimension(delta1.m_dx*delta0.m_dy - delta0.m_dx*delta1.m_dy) );
	YRealDimension	dd2( dd*dd );
	YRealDimension	ddx( YRealDimension(delta0.m_dx-delta1.m_dx) );
	YRealDimension	ddy( YRealDimension(delta0.m_dy-delta1.m_dy) );
	YRealDimension	dtmp( ddx*ddx + ddy*ddy );
	if( !AreFloatsEqual(dtmp,0.0) )
		nDistance	= YRealDimension( ::sqrt(dd2/dtmp) );
	return nDistance;
	}

// ****************************************************************************
//
// Function Name:		operator<<
//
// Description:		Insertion operator for points
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T> inline RArchive& operator<<( RArchive& archive, const RPoint< T >& point )
	{
	archive << point.m_x << point.m_y;
	return archive;
	}

// ****************************************************************************
//
// Function Name:		operator>>
//
// Description:		Extraction operator for points
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T> inline RArchive& operator>>( RArchive& archive, RPoint< T >& point )
	{
	archive >> point.m_x >> point.m_y;
	return archive;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// POINT_H_
