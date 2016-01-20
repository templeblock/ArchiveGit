// ****************************************************************************
//
//  File Name:			Utilities.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Useful utility functions
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
//  $Logfile:: /PM8/Framework/Include/Utilities.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_UTILITIES_H_
#define		_UTILITIES_H_

// ****************************************************************************
//
//  Function Name:	::CreateRectPoly( const RIntRect& rect, RIntPoint* points )
//
//  Description:		Create a 4 point polygon from the given RIntVectorRect
//
//  Returns:			void
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template <class S, class T>
inline void CreateRectPoly( const RVectorRect<S>& vectorRect, RPoint<T>* points )
{
	points[ 0 ] = vectorRect.m_TopLeft;
	points[ 1 ] = vectorRect.m_TopRight;
	points[ 2 ] = vectorRect.m_BottomRight;
	points[ 3 ] = vectorRect.m_BottomLeft;
}

// ****************************************************************************
//
//  Function Name:	::CreateRectPoly( const RIntRect& rect, RIntPoint* points )
//
//  Description:		Create a 4 point polygon from the given RIntVectorRect
//
//  Returns:			void
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template <class S, class T>
inline void CreateRectPoly( const RRect<S>& rect, RPoint<T>* points )
{
	points[ 0 ] = rect.m_TopLeft;
	points[ 1 ] = RPoint<S>( rect.m_Right, rect.m_Top );
	points[ 2 ] = rect.m_BottomRight;
	points[ 3 ] = RPoint<S>( rect.m_Left, rect.m_Bottom );
}

// ****************************************************************************
//
//  Function Name:	::TransformPoints( A* points, int nPoints, const T& transform )
//
//  Description:		Transform the given polygon points
//
//  Returns:			void
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template <class T>
inline void TransformPoints( RPoint<T>* points, int nPoints, const R2dTransform& transform )
{
	// Apply the transform to the points
	while( nPoints-- )
		{
		(*points) *= transform;
		++points;
		}
}

// ****************************************************************************
//
//  Function Name:	::TransformPoints( A* points, int nPoints, const T& transform )
//
//  Description:		Transform the given points into an output array
//
//  Returns:			void
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template <class S, class T>
inline void TransformPoints( const RPoint<S>* sourcePoints, RPoint<T>* destPoints, int nPoints, const R2dTransform& transform )
{
	// Apply the transform to the points
	while( nPoints-- )
		{
		(*destPoints) = (*sourcePoints) * transform;
		++sourcePoints;
		++destPoints;
		}
}

// ****************************************************************************
//
//  Function Name:	::CreateRectPoly( const E& rect, const F& transform, G* points )
//
//  Description:		Create a 4 point polygon from the given RIntRect and transforming with the given transformation
//
//  Returns:			void
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template <class T>
inline void CreateRectPoly( const RRealVectorRect& rect, const R2dTransform& transform, RPoint<T>* points )
{
	CreateRectPoly( rect, points );
	TransformPoints( points, 4, transform );
}

// ****************************************************************************
//
//  Function Name:	::CreateRectPoly( const E& rect, const F& transform, G* points )
//
//  Description:		Create a 4 point polygon from the given RIntRect and transforming with the given transformation
//
//  Returns:			void
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template <class T>
inline void CreateRectPoly( const RRealRect& rect, const R2dTransform& transform, RPoint<T>* points )
{
	CreateRectPoly( rect, points );
	TransformPoints( points, 4, transform );
}

// ****************************************************************************
//
//  Function Name:	Min
//
//  Description:		Computes the minimum of the specified values
//
//  Returns:			See above
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template< class T > inline const T& Min( const T& m1, const T& m2 )
	{
	return m1 < m2 ? m1 : m2;
	}

// ****************************************************************************
//
//  Function Name:	Max
//
//  Description:		Computes the maximum of the specified values
//
//  Returns:			See above
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template< class T > inline const T& Max( const T& m1, const T& m2 )
	{
	return m1 > m2 ? m1 : m2;
	}

// ****************************************************************************
//
//  Function Name:	RenaissanceBeep
//
//  Description:		Beeps
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
inline void RenaissanceBeep( )
	{
#ifdef	_WINDOWS
	::MessageBeep( MB_OK );
#else		//	_WINDOWS
	::SysBeep( 0 );
#endif	//	_WINDOWS
	}

// ****************************************************************************
//
//  Function Name:	RenaissanceGetTickCount
//
//  Description:		Returns the number milliseconds since the system was
//							started.
//
//  Returns:			See above
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
inline YTickCount RenaissanceGetTickCount( )
	{
#ifdef	_WINDOWS
	return ::GetTickCount( );
#else
	return ( ::TickCount() * 1000 ) / 60;
#endif
	}

// ****************************************************************************
//
//  Function Name:	DegreesToRadians
//
//  Description:		Converts degrees to radians
//
//  Returns:			See above
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template <class T> inline YAngle DegreesToRadians( T angle )
	{
	return ( (YAngle)angle) * ( kPI / 180.0 );
	}

// ****************************************************************************
//
//  Function Name:	RadiansToDegrees
//
//  Description:		Converts radians to degrees
//
//  Returns:			See above
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
inline YRealDegrees RadiansToDegrees( YAngle nRadians )
	{
	return (YRealDegrees)( nRadians * ( 360.0 / k2PI ) );
	}

// ****************************************************************************
//
//  Function Name:	NumElements
//
//  Description:		Macro which resovles to the number of elements in the
//							specified array.
//
//  Returns:			The number of elements in the array
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
#define NumElements( array ) ( sizeof( array ) / sizeof( array[ 0 ] ) )

// ****************************************************************************
//
//  Function Name:	Abs
//
//  Description:		Template abs function
//
//  Returns:			The absolute value of the specified value
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template< class T > inline T Abs( const T& t )
	{
	return ( t < 0 ) ? -t : t;
	}


// ****************************************************************************
//
//  Function Name:	::midpoint( )
//
//  Description:		Computes the midpoint of a line.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline void midpoint( RPoint<T>& out, const RPoint<T>& in1, const RPoint<T>& in2 )
	{
	out.m_x = ( in1.m_x + in2.m_x ) / 2;
	out.m_y = ( in1.m_y + in2.m_y ) / 2;
	}

// ****************************************************************************
//
//  Function Name:	::mid( )
//
//  Description:		Computes the midpoint of a line.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline void mid( T* out, T* in1, T* in2 )
	{
	out[ 0 ] = ( in1[ 0 ] + in2[ 0 ] ) / 2;
	out[ 1 ] = ( in1[ 1 ] + in2[ 1 ] ) / 2;
	}

// ****************************************************************************
//
//  Function Name:	::ComputeAngle( )
//
//  Description:		Computes the angle from pt0 to pt1 in Renaissance coordinate space.
//
//  Returns:			the angle
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T>
inline YAngle ComputeAngle( const RPoint<T>& pt0, const RPoint<T>& pt1 )
{
	return ::atan2( double(pt0.m_y-pt1.m_y), double(pt1.m_x-pt0.m_x) );
}

// ****************************************************************************
//
//  Function Name:	::Swap( RRealPoint& point )
//
//  Description:		Swap the given point's dimensions
//
//  Returns:			void
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template< class T > inline void Swap( RPoint<T>& point )
	{
	T yTemp = point.m_x;
	point.m_x = point.m_y;
	point.m_y = yTemp;
	}

// ****************************************************************************
//
//  Function Name:	::Swap( RRealSize& size )
//
//  Description:		Swap the given size's dimensions
//
//  Returns:			void
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template< class T > inline void Swap( RSize<T>& size )
	{
	T yTemp = size.m_dx;
	size.m_dx = size.m_dy;
	size.m_dy = yTemp;
	}

// ****************************************************************************
//
//  Function Name:	::Swap( RRealRect& rect )
//
//  Description:		Swap the top left and bottom right points
//
//  Returns:			void
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template< class T > inline void Swap( RRect<T>& rect )
	{
	Swap( rect.m_TopLeft );		
	Swap( rect.m_BottomRight );		
	}

#ifdef MAC

// ****************************************************************************
//
//  Function Name:	_hypot
//
//  Description:		Calculates the length of the hypotenuse of a right
//							triangle, given the length of the two sides x and y.
//
//  Returns:			The hypotenuse
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
inline double _hypot( double dy, double dx )
	{
	return ::sqrt( ( dx * dx ) + ( dy * dy ) );
	}

#endif  // MAC

#ifdef	MAC
//inline void* operator new(size_t, void* p) {return p;}
#endif

FrameworkLinkage HNATIVE PointerToHandle( const uBYTE* pData, YDataLength dataLength );
FrameworkLinkage void HandleToPointer( HNATIVE hNative, uBYTE*& pData, YDataLength& dataLength );
FrameworkLinkage HNATIVE HandleToHandle( HNATIVE hNative );
FrameworkLinkage void FreeHandle( HNATIVE hNative );
FrameworkLinkage int SolveQuadratic( YFloatType A, YFloatType B, YFloatType C, YFloatType* pRoots );
FrameworkLinkage int GetScreenColorDepth( );

FrameworkLinkage BOOLEAN IsFileAtPath( RMBCString& rFile, RMBCString& rPath );
FrameworkLinkage int GetCDROMDrives( char *szBuff, int nMaxCDs );


#ifdef	_WINDOWS

void OutputDebugStringWithTime( char* pString );

#endif	// _WINDOWS

#endif	//	_UTILITIES_H_
