// ****************************************************************************
//
//  File Name:			Rect.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RRect class
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
//  $Logfile:: /PM8/Framework/Include/Rect.h                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_RECT_H_
#define		_RECT_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RArchive;

template <class T> class RRect;
typedef	RRect<YIntCoordinate>	RIntRect;
typedef	RRect<YRealCoordinate>	RRealRect;

// ****************************************************************************
//
//  Class Name:		RRect
//
//  Description:		Parameterized rectangle class
//
// ****************************************************************************
//
template <class T> class RRect
	{
	// Construction & destruction
	public :
												RRect( );
												RRect( T left, T top, T right, T bottom );
												RRect( RPoint<T> topLeft, RPoint<T> bottomRight );
												RRect( RPoint<T> topLeft, RSize<T> widthHeight );
												RRect( RSize<T> widthHeight );

												RRect( const RIntRect& rhs );
												RRect( const RRealRect& rhs );
#ifdef _WINDOWS
												RRect( const CRect& rhs );
#endif

	// Operators
	public :
		RRect<T>&							operator=( const RIntRect& rhs );
		RRect<T>&							operator=( const RRealRect& rhs );

		BOOLEAN								operator==( const RRect<T>& rhs );
		BOOLEAN								operator!=( const RRect<T>& rhs );

		RRect<T>&							operator*=( const R2dTransform& rhs );
		RRect<T>								operator*( const R2dTransform& rhs ) const;

#ifdef _WINDOWS
												operator CRect( ) const;
#endif

	// Operations
	public :
		T										Width( ) const;
		T										Height( ) const;
		RSize<T>								WidthHeight( ) const;

		BOOLEAN								PointInRect( const RPoint<T>& point ) const;
		BOOLEAN								RectInRect( const RRect<T>& rect ) const ;

		BOOLEAN								IsEmpty( ) const;
		BOOLEAN								IsIntersecting( const RRect<T>& rect ) const;
		BOOLEAN								IsIntersecting( const RPoint<T>& point1, const RPoint<T>& point2 ) const;

		RPoint<T>							GetCenterPoint( ) const;

		RRect<T>&							Move( const RPoint<T>& point );
		RRect<T>&							Offset( const RSize<T>& size );
		RRect<T>&							Inset( const RSize<T>& size );
		RRect<T>&							Inset( const RSize<T>& topLeftSize, const RSize<T>& bottomRightSize);
		RRect<T>&							Inflate( const RSize<T>& size );
		RRect<T>&							Inflate( const RSize<T>& topLeftSize, const RSize<T>& bottomRightSize );
		RRect<T>&							Union( const RRect<T>& rect1, const RRect<T>& rect2 );
		RRect<T>&							Intersect( const RRect<T>& rect1, const RRect<T>& rect2 );
		RRect<T>&							Scale( const RRealSize& scaleFactor );

	//	if anyone needs these, feel free to implement them.
	//	RRect<T>								Union( const RRect<T>& rect );
	//	RRect<T>								Intersect( const RRect<T>& rect );
		RPoint<T>&							PinPointInRect( RPoint<T>& point );
	//	void									PinRectInRect( RRect<T>& rect );
	//	void									CenterPointInRect( RPoint<T>& point );
		RRect<T>&							AddPointToRect( const RPoint<T>& point );
		RRect<T>&							AddRectToRect( const RRect<T>& rect );
		RRect<T>&							CenterRectInRect( RRect<T>& rect, BOOLEAN fHoriz, BOOLEAN fVert );
		RRect<T>&							CenterRectInRect( RRect<T>& rect );
		YScaleFactor						ShrinkToFit( RRect<T> rDisplayRect );

	// Member data
	public :
		RPoint<T>							m_TopLeft;
		RPoint<T>							m_BottomRight;
		T&										m_Left;
		T&										m_Top;
		T&										m_Right;
		T&										m_Bottom;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RRect::RRect( )
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
inline RRect<T>::RRect( )
	: m_Left( m_TopLeft.m_x ),
	  m_Top( m_TopLeft.m_y ),
	  m_Right( m_BottomRight.m_x ),
	  m_Bottom( m_BottomRight.m_y )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RRect::RRect( )
//
//  Description:		Constructs a rectangle given four coordinates
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>::RRect( T left, T top, T right, T bottom )
	: m_TopLeft( left, top ),
	  m_BottomRight( right, bottom ),
	  m_Left( m_TopLeft.m_x ),
	  m_Top( m_TopLeft.m_y ),
	  m_Right( m_BottomRight.m_x ),
	  m_Bottom( m_BottomRight.m_y )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RRect::RRect( )
//
//  Description:		Constructs a rectangle given two points
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>::RRect( RPoint<T> topLeft, RPoint<T> bottomRight )
	: m_TopLeft( topLeft ),
	  m_BottomRight( bottomRight ),
	  m_Left( m_TopLeft.m_x ),
	  m_Top( m_TopLeft.m_y ),
	  m_Right( m_BottomRight.m_x ),
	  m_Bottom( m_BottomRight.m_y )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RRect::RRect( )
//
//  Description:		Constructs a rectangle given a point and a size
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>::RRect( RPoint<T> topLeft, RSize<T> widthHeight )
	: m_TopLeft( topLeft ),
	  m_BottomRight( topLeft+widthHeight ),
	  m_Left( m_TopLeft.m_x ),
	  m_Top( m_TopLeft.m_y ),
	  m_Right( m_BottomRight.m_x ),
	  m_Bottom( m_BottomRight.m_y )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RRect::RRect( )
//
//  Description:		Constructs a rectangle given a size and assuming origin of zero
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>::RRect( RSize<T> widthHeight )
	: m_TopLeft( 0, 0 ),
	  m_BottomRight( widthHeight.m_dx, widthHeight.m_dy ),
	  m_Left( m_TopLeft.m_x ),
	  m_Top( m_TopLeft.m_y ),
	  m_Right( m_BottomRight.m_x ),
	  m_Bottom( m_BottomRight.m_y )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RRect::RRect( )
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
inline RRect<T>::RRect( const RRect<YIntCoordinate>& rhs )
	: m_TopLeft( rhs.m_TopLeft ),
	  m_BottomRight( rhs.m_BottomRight ),
	  m_Left( m_TopLeft.m_x ),
	  m_Top( m_TopLeft.m_y ),
	  m_Right( m_BottomRight.m_x ),
	  m_Bottom( m_BottomRight.m_y )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RRect::RRect( )
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
inline RRect<T>::RRect( const RRect<YRealCoordinate>& rhs )
	: m_TopLeft( rhs.m_TopLeft ),
	  m_BottomRight( rhs.m_BottomRight ),
	  m_Left( m_TopLeft.m_x ),
	  m_Top( m_TopLeft.m_y ),
	  m_Right( m_BottomRight.m_x ),
	  m_Bottom( m_BottomRight.m_y )
	{
	;
	}

#ifdef _WINDOWS

// ****************************************************************************
//
//  Function Name:	RRect::RRect( )
//
//  Description:		ctor which takes a CRect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> RRect<T>::RRect( const CRect& rhs )
	: m_TopLeft( (T)rhs.left, (T)rhs.top ),
	  m_BottomRight( (T)rhs.right, (T)rhs.bottom ),
	  m_Left( m_TopLeft.m_x ),
	  m_Top( m_TopLeft.m_y ),
	  m_Right( m_BottomRight.m_x ),
	  m_Bottom( m_BottomRight.m_y )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RRect::operator CRect( )
//
//  Description:		converts an RRect to a CRect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> RRect<T>::operator CRect( ) const
	{
	return CRect( (int)m_Left, (int)m_Top, (int)m_Right, (int)m_Bottom );
	}

#endif

// ****************************************************************************
//
//  Function Name:	RRect::operator=( )
//
//  Description:		Copy operator
//
//  Returns:			Reference to this point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::operator=( const RRect<YIntCoordinate>& rhs )
	{
	m_TopLeft = rhs.m_TopLeft;
	m_BottomRight = rhs.m_BottomRight;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RRect::operator=( )
//
//  Description:		Copy operator
//
//  Returns:			Reference to this point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::operator=( const RRect<YRealCoordinate>& rhs )
	{
	m_TopLeft = rhs.m_TopLeft;
	m_BottomRight = rhs.m_BottomRight;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RRect::operator==( )
//
//  Description:		Equality operator
//
//  Returns:			TRUE if the rectangles are equal
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline BOOLEAN RRect<T>::operator==( const RRect& rhs )
	{
	return (BOOLEAN)((m_TopLeft == rhs.m_TopLeft) && (m_BottomRight == rhs.m_BottomRight));
	}

// ****************************************************************************
//
//  Function Name:	RRect::operator!=( )
//
//  Description:		Inequality operator
//
//  Returns:			TRUE if the rectangles are NOT equal
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline BOOLEAN RRect<T>::operator!=( const RRect& rhs )
	{
	return (BOOLEAN)((m_TopLeft != rhs.m_TopLeft) || (m_BottomRight != rhs.m_BottomRight));
	}

// ****************************************************************************
//
//  Function Name:	RRect::Width( )
//
//  Description:		Calculates the width of the rectangle
//
//  Returns:			The width
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline T RRect<T>::Width( ) const
	{
	return m_Right - m_Left;
	}

// ****************************************************************************
//
//  Function Name:	RRect::Height( )
//
//  Description:		Calculates the height of the rectangle
//
//  Returns:			The height
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline T RRect<T>::Height( ) const
	{
	return m_Bottom - m_Top;
	}

// ****************************************************************************
//
//  Function Name:	RRect::WidthHeight( )
//
//  Description:		Calculates the Width & Height of the rectangle as a RSize
//
//  Returns:			The Width and Height as a RSize<T>
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RSize<T> RRect<T>::WidthHeight( ) const
	{
	return RSize<T>(Width(), Height());
	}

// ****************************************************************************
//
//  Function Name:	RRect::GetCenterPoint( )
//
//  Description:		Calculates the center point of this rectangle
//
//  Returns:			The center point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T> RRect<T>::GetCenterPoint( ) const
	{
	return RPoint<T>( m_Left + ( Width( ) / 2 ), m_Top + ( Height( ) / 2 ) );
	}

// ****************************************************************************
//
//  Function Name:	RRect::PointInRect( )
//
//  Description:		Determines if the given point is within this rectangle
//
//  Returns:			TRUE:		The point is within the rectangle.
//							FALSE:	The point is not within the rectangle.
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline BOOLEAN RRect<T>::PointInRect( const RPoint<T>& point ) const
	{
	return (BOOLEAN)( point.m_x >= m_Left && point.m_x < m_Right && point.m_y >= m_Top && point.m_y < m_Bottom );
	}

// ****************************************************************************
//
//  Function Name:	RRect::RectInRect( )
//
//  Description:		Determines if the given rect is within this rectangle
//
//  Returns:			TRUE:		The rect is within the rectangle.
//							FALSE:	The rect is not within the rectangle.
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline BOOLEAN RRect<T>::RectInRect( const RRect<T>& point ) const
	{
	return (BOOLEAN)( PointInRect(point.m_TopLeft) && PointInRect(point.m_BottomRight) );
	}

// ****************************************************************************
//
//  Function Name:	RRect::IsEmpty( )
//
//  Description:		Determines if this rect is empty
//
//  Returns:			TRUE:		The rect is Empty.
//							FALSE:	The rect is not Empty.
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline BOOLEAN RRect<T>::IsEmpty( ) const
	{
	return (BOOLEAN)( (m_Left >= m_Right) || (m_Top >= m_Bottom) );
	}

// ****************************************************************************
//
//  Function Name:	RRect::IsIntersecting( )
//
//  Description:		Determines if the given rectangle intersects 'this' one
//
//  Returns:			TRUE:		The rects intersect.
//							FALSE:	The rects do NOT intersect.
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline BOOLEAN RRect<T>::IsIntersecting( const RRect<T>& rect ) const
	{
	return (BOOLEAN)( (m_Right>rect.m_Left) && (m_Left<rect.m_Right) &&
					  (m_Bottom>rect.m_Top) && (m_Top<rect.m_Bottom) );
	}

// ****************************************************************************
//
//  Function Name:	RRect::IsIntersecting( )
//
//  Description:		Determines if the line formed by the specified points
//							intersects this rectangle
//
//  Returns:			TRUE:		The rectangle and line intersect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline BOOLEAN RRect<T>::IsIntersecting( const RPoint<T>& point1, const RPoint<T>& point2 ) const
	{
	if( ::LineSegmentsIntersect( point1, point2, m_TopLeft, RPoint<T>( m_Right, m_Top ) ) )
		return TRUE;

	if( ::LineSegmentsIntersect( point1, point2, RPoint<T>( m_Right, m_Top ), m_BottomRight ) )
		return TRUE;

	if( ::LineSegmentsIntersect( point1, point2, m_BottomRight, RPoint<T>( m_Left, m_Bottom ) ) )
		return TRUE;

	if( ::LineSegmentsIntersect( point1, point2, RPoint<T>( m_Left, m_Bottom ), m_TopLeft ) )
		return TRUE;

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RRect::Move( )
//
//  Description:		Absolute move to the given point
//
//  Returns:			Reference to this rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::Move( const RPoint<T>& point )
	{
	m_Right = point.m_x + Width( );
	m_Bottom = point.m_y + Height( );
	m_TopLeft = point;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RRect::Offset( )
//
//  Description:		Relative offest
//
//  Returns:			Reference to this rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::Offset( const RSize<T>& size )
	{
	m_TopLeft += size;
	m_BottomRight += size;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RRect::Inset( )
//
//  Description:		Inset all edges of the rectangle by the given size
//
//  Returns:			Reference to this rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::Inset( const RSize<T>& size )
	{
	m_TopLeft += size;
	m_BottomRight -= size;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RRect::Inset( )
//
//  Description:		Inset all edges of the rectangle by the given sizes
//
//  Returns:			Reference to this rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::Inset( const RSize<T>& topLeftSize, const RSize<T>& bottomRightSize )
	{
	m_TopLeft += topLeftSize;
	m_BottomRight -= bottomRightSize;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RRect::Inflate( )
//
//  Description:		Inflate all edges of the rectangle by the given size
//
//  Returns:			Reference to this rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::Inflate( const RSize<T>& size )
	{
	m_TopLeft -= size;
	m_BottomRight += size;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RRect::Inflate( )
//
//  Description:		Inflate all edges of the rectangle by the given sizes
//
//  Returns:			Reference to this rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::Inflate( const RSize<T>& topLeftSize, const RSize<T>& bottomRightSize )
	{
	m_TopLeft -= topLeftSize;
	m_BottomRight += bottomRightSize;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RRect::Union( )
//
//  Description:		Compute the union of both rectangles
//
//  Returns:			Reference to this rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::Union( const RRect<T>& rect1, const RRect<T>& rect2 )
	{
	m_Left	= ( rect1.m_Left < rect2.m_Left )		?	rect1.m_Left : rect2.m_Left;
	m_Top		= ( rect1.m_Top < rect2.m_Top )			?	rect1.m_Top : rect2.m_Top;
	m_Right	= ( rect1.m_Right > rect2.m_Right )		?	rect1.m_Right : rect2.m_Right;
	m_Bottom	= ( rect1.m_Bottom > rect2.m_Bottom )	?	rect1.m_Bottom : rect2.m_Bottom;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RRect::Intersect( )
//
//  Description:		Compute the Intersection of both rectangles
//
//  Returns:			Reference to this rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::Intersect( const RRect<T>& rect1, const RRect<T>& rect2 )
	{
	// if the rectangles do not intersect, make this the NULL rect
	if( !rect1.IsIntersecting( rect2 ) )
		m_Left = m_Top = m_Right = m_Bottom = 0;

	else
		{
		m_Left	= ( rect1.m_Left > rect2.m_Left )		?	rect1.m_Left : rect2.m_Left;
		m_Top		= ( rect1.m_Top > rect2.m_Top )			?	rect1.m_Top : rect2.m_Top;
		m_Right	= ( rect1.m_Right < rect2.m_Right )		?	rect1.m_Right : rect2.m_Right;
		m_Bottom	= ( rect1.m_Bottom < rect2.m_Bottom )	?	rect1.m_Bottom : rect2.m_Bottom;
		}
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RRect::PinPointInRect( )
//
//  Description:		Force the point to be within the rectangle.
//
//  Returns:			Reference to the point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RPoint<T>& RRect<T>::PinPointInRect( RPoint<T>& pt )
	{
	if (pt.m_x < m_Left)				pt.m_x = m_Left;
	else if (pt.m_x > m_Right)		pt.m_x = m_Right;
	if (pt.m_y < m_Top)				pt.m_y = m_Top;
	else if (pt.m_y > m_Bottom)	pt.m_y = m_Bottom;
	return pt;
	}

// ****************************************************************************
//
//  Function Name:	RRect::AddPointToRect( )
//
//  Description:		Adjust the rectangle to contain the point.
//
//  Returns:			Reference to this rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::AddPointToRect( const RPoint<T>& pt )
	{
	if (pt.m_x < m_Left)				m_Left = pt.m_x;
	else if (pt.m_x > m_Right)		m_Right = pt.m_x;
	if (pt.m_y < m_Top)				m_Top = pt.m_y;
	else if (pt.m_y > m_Bottom)	m_Bottom = pt.m_y;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RRect::AddRectToRect( )
//
//  Description:		Adjust the rectangle to contain the rect.
//
//  Returns:			Reference to this rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::AddRectToRect( const RRect<T>& rect )
	{
	AddPointToRect( rect.m_TopLeft );
	AddPointToRect( rect.m_BottomRight );
	AddPointToRect( RPoint<T>( rect.m_Right, rect.m_Top ) );
	AddPointToRect( RPoint<T>( rect.m_Left, rect.m_Bottom ) );

	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RRect::CenterRectInRect( )
//
//  Description:		Center the rectangle in the given rectangle based on the
//							passed in flags for Horizontal centering (fHoriz) or 
//							Vertical centering (fVert).
//
//  Returns:			Reference to this rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::CenterRectInRect( RRect<T>& rect, BOOLEAN fHoriz, BOOLEAN fVert )
	{
	RSize<T>	offsetSize;
	RPoint<T> ourCenter( GetCenterPoint( ) );
	RPoint<T> rectCenter( rect.GetCenterPoint( ) );
	offsetSize.m_dx = (fHoriz? (rectCenter.m_x - ourCenter.m_x) : 0);
	offsetSize.m_dy = (fVert? (rectCenter.m_y - ourCenter.m_y) : 0);
	return Offset( offsetSize );
	}

// ****************************************************************************
//
//  Function Name:	RRect::CenterRectInRect( )
//
//  Description:		Center the rectangle in the given rectangle.
//
//  Returns:			Reference to this rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::CenterRectInRect( RRect<T>& rect )
	{
	return CenterRectInRect( rect, TRUE, TRUE );
	}

// ****************************************************************************
//
// Function Name:		RRect::operator=*( )
//
// Description:		Multiplies this rect by the given transform
//
// Returns:				Reference to this rect
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
RRect<T>& RRect<T>::operator*=( const R2dTransform& rhs )
	{
	m_TopLeft *= rhs;
	m_BottomRight *= rhs;
	return *this;
	}

// ****************************************************************************
//
// Function Name:		RRect::operator*( )
//
// Description:		Returns a rect that is the product of this rect and the
//							given transform
//
// Returns:				The transformed rect
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
RRect<T> RRect<T>::operator*( const R2dTransform& rhs ) const
	{
	return RRect( m_TopLeft*rhs, m_BottomRight*rhs );
	}

// ****************************************************************************
//
// Function Name:		RRect::Scale
//
// Description:		Multiplies this rect by a scaling factor
//
// Returns:				Reference to this rect
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
inline RRect<T>& RRect<T>::Scale( const RRealSize& scaleFactor )
	{
	m_TopLeft.Scale( scaleFactor );
	m_BottomRight.Scale( scaleFactor );
	return *this;
	}

// ****************************************************************************
//
// Function Name:		RRect::ShrinkToFit
//
// Description:		Shrinks this rectangle so that it fits within the specified
//							rectangle, maintaining aspect ratio
//
// Returns:				The amount that the rectangle was scaled
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T> YScaleFactor RRect<T>::ShrinkToFit( RRect<T> rDisplayRect )
{
	YScaleFactor flScaleFactor;

	T nDisplayHeight = rDisplayRect.Height();
	T nDisplayWidth = rDisplayRect.Width();

	T nSourceHeight = Height();
	T nSourceWidth  = Width();

 	//
	//Get the differences in size between the source rectangle and display rectangle...
	YFloatType flHeightRatio = (YFloatType)nDisplayHeight/ nSourceHeight;
	YFloatType flWidthRatio  = (YFloatType)nDisplayWidth / nSourceWidth;
	T	nDestHeight	= nDisplayHeight;
	T	nDestWidth	= nDisplayWidth;
	T	nDestX		= 0;
	T	nDestY		= 0;

	//
	//Check to see if the source is larger(in either dimesion) than 
	//its display...
	flScaleFactor = 1.0f;
	if ( flHeightRatio < 1.0f || flWidthRatio < 1.0f )
	{
		//
		//Have to shrink source(and maintain aspect correctness) to fit into display region..

		//
		//Determine largest dimension...
		if ( flHeightRatio <= flWidthRatio )
		{
			//
			// Adjust destination width to maintain aspect correctness...
			flScaleFactor = flHeightRatio;
			nDestWidth	= (T)( nSourceWidth * flHeightRatio );
		}
		else
		{
			//
			//Adjust destination height to maintain aspect correctness...
			flScaleFactor = flWidthRatio;
			nDestHeight	= (T)( nSourceHeight * flWidthRatio );
		}
		//
		//Apply changes..
		m_Left		= nDestX;
		m_Top			= nDestY;
		m_Right		= nDestWidth + nDestX;
		m_Bottom		= nDestHeight + nDestY;
	}

	return flScaleFactor;
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
template <class T> inline RArchive& operator<<( RArchive& archive, const RRect< T >& rect )
	{
	archive << rect.m_Left << rect.m_Top << rect.m_Right << rect.m_Bottom;
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
template <class T> inline RArchive& operator>>( RArchive& archive, RRect< T >& rect )
	{
	archive >> rect.m_Left >> rect.m_Top >> rect.m_Right >> rect.m_Bottom;
	return archive;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _RECT_H_
