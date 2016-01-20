// ****************************************************************************
//
//  File Name:			VectorRect.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RVectorRect class
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
//  $Logfile:: /PM8/Framework/Include/VectorRect.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_VECTORRECT_H_
#define		_VECTORRECT_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

#define __AssertFile__ __FILE__

class RArchive;

template <class T> class RVectorRect;
typedef	RVectorRect<YIntCoordinate>	RIntVectorRect;
typedef	RVectorRect<YRealCoordinate>	RRealVectorRect;

// ****************************************************************************
//
//  Class Name:		RVectorRect
//
//  Description:		Rotatable rectangle class. The rectangle is stored as 
//							four vertices, rather than a TopLeft and BottomRight.
//
// ****************************************************************************
//
template <class T> class RVectorRect
	{
	// Construction & destruction
	public :
												RVectorRect( );
												RVectorRect( T left, T top, T right, T bottom );
												RVectorRect( const RRect<T>& rect );
												RVectorRect( const RSize<T> rsWH );
												RVectorRect( const RIntVectorRect& rhs );
												RVectorRect( const RRealVectorRect& rhs );

	// Operators
	public :
		RVectorRect<T>&					operator=( const RIntVectorRect& rhs );
		RVectorRect<T>&					operator=( const RRealVectorRect& rhs );
		void									operator*=( const R2dTransform& rhs );
		RVectorRect<T>						operator*( const R2dTransform& rhs ) const;

	// Operations :
	public :
		void									Set( T left, T top, T right, T bottom );
		void									Set( const RRect<T>& rect );
		void									Set( const RSize<T>& rsWH );
		void									Set( const RSize<T>& rsWH, const R2dTransform& transform );

		void									RotateAboutCenter( YAngle angle );
		void									RotateAboutPoint( const RRealPoint& point, YAngle angle );

		void									Reset( );
		void									Offset( const RSize<T>& size );
		void									Inflate( const RSize<T>& size );
		void									Inset( const RSize<T>& topLeft, const RSize<T>& bottomRight );
		void									Scale( const RRealSize& scaleFactor );
		void									ScaleAboutPoint( const RRealPoint& point, const RRealSize& scaleFactor );
		void									UnrotateAndScaleAboutPoint( const RRealPoint& point, const RRealSize& scaleFactor );
		void									UnrotateAndScaleAboutPoint( const RRealPoint& point, const RRealSize& scaleFactor, const RRealSize& minimumSize, const RRealSize& maximumSize, BOOLEAN fMaintainAspectRatio );
		void									ResizeAboutPoint( const RRealPoint& point, const RRealSize& scaleFactor );
		void									ResizeAboutPoint( const RRealPoint& point, const RRealSize& scaleFactor, const RRealSize& minimumSize, const RRealSize& maximumSize, BOOLEAN fMaintainAspectRatio );
		void									UnrotateAndResizeAboutPoint( const RRealPoint& point, const RRealSize& scaleFactor );
		void									UnrotateAndResizeAboutPoint( const RRealPoint& point, const RRealSize& scaleFactor, const RRealSize& minimumSize, const RRealSize& maximumSize, BOOLEAN fMaintainAspectRatio );

		RPoint<T>							GetCenterPoint( ) const;
		BOOLEAN								PointInRect( const RPoint<T>& point ) const;
		BOOLEAN								IsIntersecting( const RRect<T>& rect ) const;
		BOOLEAN								IsInRect( const RRect<T>& rect ) const;
		BOOLEAN								IsEmpty( ) const;
		const R2dTransform&				GetTransform( ) const;
		const RSize<T>&					GetUntransformedSize( ) const;
		void									GetPolygonPoints( RPoint<T>* polygonPoints ) const;
		YAngle								GetRotationAngle( ) const;

		T										Width( ) const;
		T										Height( ) const;
		RSize<T>								WidthHeight( ) const;

	// Implementation
	private :
		RRealSize							ConstrainScaleFactor( const RRealSize& scaleFactor, const RRealSize& minimumSize, const RRealSize& maximumSize, BOOLEAN fMaintainAspectRatio ) const;

	// Member data
	public :
		RRect<T>								m_TransformedBoundingRect;

		RPoint<T>							m_TopLeft;
		RPoint<T>							m_TopRight;
		RPoint<T>							m_BottomLeft;
		RPoint<T>							m_BottomRight;

	private :
		RSize<T>								m_UntransformedSize;
		R2dTransform						m_Transform;

		void									Recalculate( );

	//	Friend functions
	private :
		friend RArchive& operator<<( RArchive& storage, const RVectorRect< T >& rect );
		friend RArchive& operator>>( RArchive& storage, RVectorRect< T >& rect );
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RVectorRect::RVectorRect( )
//
//  Description:		Default constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> RVectorRect<T>::RVectorRect( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::RVectorRect( )
//
//  Description:		Constructs a vector rect given four coordinates
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> RVectorRect<T>::RVectorRect( T left, T top, T right, T bottom )
	: m_UntransformedSize( right - left, bottom - top )
	{
	m_Transform.PreTranslate( left, top );

	Recalculate( );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::RVectorRect( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> RVectorRect<T>::RVectorRect( const RRect<T>& rect )
	: m_UntransformedSize( rect.WidthHeight( ) )
	{
	m_Transform.PreTranslate( rect.m_Left, rect.m_Top );

	Recalculate( );
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
template <class T> RVectorRect<T>::RVectorRect( const RSize<T> size )
	: m_UntransformedSize( size )
	{
	Recalculate( );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::RVectorRect( )
//
//  Description:		Copy constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> RVectorRect<T>::RVectorRect( const RVectorRect<YIntDimension>& rhs )
	: m_UntransformedSize( rhs.GetUntransformedSize( ) ),
	  m_Transform( rhs.GetTransform( ) )
	{
	Recalculate( );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::RVectorRect( )
//
//  Description:		Copy constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> RVectorRect<T>::RVectorRect( const RVectorRect<YRealDimension>& rhs )
	: m_UntransformedSize( rhs.GetUntransformedSize( ) ),
	  m_Transform( rhs.GetTransform( ) )
	{
	Recalculate( );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::operator=( )
//
//  Description:		Copy operator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> RVectorRect<T>& RVectorRect<T>::operator=( const RVectorRect<YIntDimension>& rhs )
	{
	m_UntransformedSize = rhs.GetUntransformedSize( );
	m_Transform = rhs.GetTransform( );
	Recalculate( );
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::operator=( )
//
//  Description:		Copy operator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> RVectorRect<T>& RVectorRect<T>::operator=( const RVectorRect<YRealDimension>& rhs )
	{
	m_UntransformedSize = rhs.GetUntransformedSize( );
	m_Transform = rhs.GetTransform( );
	Recalculate( );
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::Set( )
//
//  Description:		Sets a vector rect given four coordinates
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RVectorRect<T>::Set( T left, T top, T right, T bottom )
	{
	m_UntransformedSize.m_dx = right - left;
	m_UntransformedSize.m_dy = bottom - top;

	m_Transform.MakeIdentity( );
	m_Transform.PreTranslate( left, top  );

	Recalculate( );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::Set( )
//
//  Description:		Sets a vector rect given a rectangle
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RVectorRect<T>::Set( const RRect<T>& rect )
	{
	m_UntransformedSize.m_dx = rect.m_Right - rect.m_Left;
	m_UntransformedSize.m_dy = rect.m_Bottom - rect.m_Top;

	m_Transform.MakeIdentity( );
	m_Transform.PreTranslate( rect.m_Left, rect.m_Top  );

	Recalculate( );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::Set( )
//
//  Description:		Sets a vector rect given a rectangle
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RVectorRect<T>::Set( const RSize<T>& rsWH )
	{
	m_UntransformedSize = rsWH;

	m_Transform.MakeIdentity( );

	Recalculate( );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::Set( )
//
//  Description:		Sets a vector rect given a rectangle
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RVectorRect<T>::Set( const RSize<T>& rsWH, const R2dTransform& transform )
	{
	m_UntransformedSize = rsWH;

	m_Transform = transform;

	Recalculate( );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::Recalculate( )
//
//  Description:		Recalculates the points and bounding rectangle of this 
//							vectorrect, using the rotate transform.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RVectorRect<T>::Recalculate( )
	{
	// Copy the original points. We always transform the original points to avoid losing precision
	m_TopLeft.m_x = m_BottomLeft.m_x = 0;
	m_TopRight.m_x = m_BottomRight.m_x = m_UntransformedSize.m_dx;
	m_TopLeft.m_y = m_TopRight.m_y = 0;
	m_BottomLeft.m_y = m_BottomRight.m_y = m_UntransformedSize.m_dy;

	// Apply the transform to the four points
	m_TopLeft *= m_Transform;
	m_TopRight *= m_Transform;
	m_BottomLeft *= m_Transform;
	m_BottomRight *= m_Transform;

	// Calculate a new transformed bounding rect
	m_TransformedBoundingRect.m_Left = Min( Min( m_TopLeft.m_x, m_TopRight.m_x ), Min( m_BottomLeft.m_x, m_BottomRight.m_x ) );
	m_TransformedBoundingRect.m_Right = Max( Max( m_TopLeft.m_x, m_TopRight.m_x ), Max( m_BottomLeft.m_x, m_BottomRight.m_x ) );
	m_TransformedBoundingRect.m_Top = Min( Min( m_TopLeft.m_y, m_TopRight.m_y ), Min( m_BottomLeft.m_y, m_BottomRight.m_y ) );
	m_TransformedBoundingRect.m_Bottom = Max( Max( m_TopLeft.m_y, m_TopRight.m_y ), Max( m_BottomLeft.m_y, m_BottomRight.m_y ) );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::RotateAboutCenter( )
//
//  Description:		Rotates this vector rect about its center.
//
//							The angle is a counterclockwise rotation in radians
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RVectorRect<T>::RotateAboutCenter( YAngle angle )
	{
	// Rotate
	RotateAboutPoint( GetCenterPoint( ), angle );

	// Recalc
	Recalculate( );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::RotateAboutPoint( )
//
//  Description:		Rotates this vector rect about a point.
//
//							The angle is a counterclockwise rotation in radians
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RVectorRect<T>::RotateAboutPoint( const RRealPoint& point, YAngle angle )
	{
	// Rotate
	m_Transform.PostRotateAboutPoint( point.m_x, point.m_y, angle );

	// Recalc
	Recalculate( );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::Reset( )
//
//  Description:		Resets the vector rect back to a zero degree rotation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RVectorRect<T>::Reset( )
	{
	// Reinitialize the transform
	m_Transform.MakeIdentity( );

	Recalculate( );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::IsEmpty( )
//
//  Description:		Determines if this vector rect is empty
//
//  Returns:			TRUE:		The vector rect is Empty.
//							FALSE:	The vector rect is not Empty.
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RVectorRect<YRealCoordinate>::IsEmpty( ) const
	{
	return ( ::AreFloatsEqual( Width( ), 0.0 ) || ::AreFloatsEqual( Height( ), 0.0 ) );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::IsEmpty( )
//
//  Description:		Determines if this vector rect is empty
//
//  Returns:			TRUE:		The vector rect is Empty.
//							FALSE:	The vector rect is not Empty.
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline BOOLEAN RVectorRect<T>::IsEmpty( ) const
	{
	return ( Width( ) == 0 || Height( ) == 0 );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::Offset( )
//
//  Description:		Offsets the unrotated rectangle by the given amount
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RVectorRect<T>::Offset( const RSize<T>& size )
	{
	// Translate
	m_Transform.PostTranslate( size.m_dx, size.m_dy );

	// Recalc
	Recalculate( );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::GetCenterPoint( )
//
//  Description:		Accessor
//
//  Returns:			The center of this vector rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> RPoint<T> RVectorRect<T>::GetCenterPoint( ) const
	{
	RPoint<T> point( m_UntransformedSize.m_dx / 2, m_UntransformedSize.m_dy / 2 );
	point *= m_Transform;
	return point;
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::GetRotationAngle( )
//
//  Description:		Accessor
//
//  Returns:			The angle which this vector rect is rotated about its
//							center
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> YAngle RVectorRect<T>::GetRotationAngle( ) const
	{
	YAngle			rotation;
	YRealDimension	xScale;
	YRealDimension	yScale;
	m_Transform.Decompose( rotation, xScale, yScale );
	return rotation;
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::PointInRect( )
//
//  Description:		Determines if the given point is within this vector rect
//
//  Returns:			TRUE:		The point is within the vector rect.
//							FALSE:	The point is not within the vector rect.
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline BOOLEAN RVectorRect<T>::PointInRect( const RPoint<T>& point ) const
	{
	// To determine if the point falls within the rotated rect, multiply the 
	// point by the inverse of the transform, and then check it against the 
	// original size
	R2dTransform inverse = m_Transform;
	inverse.Invert( );
	RPoint<T> tempPoint = point * inverse;

	RRect<T> tempRect( m_UntransformedSize );
	return tempRect.PointInRect( tempPoint );
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::IsIntersecting( )
//
//  Description:		Determines if the specified rectangle intersects this
//							vector rect
//
//  Returns:			TRUE if it intersects
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
BOOLEAN RVectorRect<T>::IsIntersecting( const RRect<T>& rect ) const
	{
	//
	//	First perform a gross bounding check with the Transformed Bounding Rect
	if ( !m_TransformedBoundingRect.IsIntersecting( rect ) )
		return FALSE;
	//
	//	It falls in the gross bounds, so perform the more complex check
	if( rect.IsIntersecting( m_TopLeft, m_TopRight )			||
		 rect.IsIntersecting( m_TopRight, m_BottomRight )		||
		 rect.IsIntersecting( m_BottomRight, m_BottomLeft )	||
		 rect.IsIntersecting( m_BottomLeft, m_TopLeft ) )
		 return TRUE;
	else
		return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::IsInRect( )
//
//  Description:		Determines the this vector rect is entirely withing the
//							specified rectangle
//
//  Returns:			TRUE if this vector rect is within the rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
BOOLEAN RVectorRect<T>::IsInRect( const RRect<T>& rect ) const
	{
	if( rect.PointInRect( m_TopLeft ) && rect.PointInRect( m_TopRight ) && rect.PointInRect( m_BottomRight ) && rect.PointInRect( m_BottomLeft ) )
		return TRUE;
	else
		return FALSE;
	}

// ****************************************************************************
//
// Function Name:		RRect::RVectorRect=*( )
//
// Description:		Multiplies this vector rect by the given transform
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
void RVectorRect<T>::operator*=( const R2dTransform& rhs )
	{
	m_Transform *= rhs;
	Recalculate( );
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::operator*( )
//
// Description:		Returns a vector rect that is the product of this vector 
//							rect and the given transform
//
// Returns:				The transformed vector rect
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
RVectorRect<T> RVectorRect<T>::operator*( const R2dTransform& rhs ) const
	{
	RVectorRect<T> temp = *this;
	temp *= rhs;
	return temp;
	}

// ****************************************************************************
//
//  Function Name:	RVectorRect::Inflate( )
//
//  Description:		Inflates this vector rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RVectorRect<T>::Inflate( const RSize<T>& size )
	{
	RRealSize	dim( WidthHeight() );
	TpsAssert( (dim.m_dx!=0.0) && (dim.m_dy!=0.0), "Dimension of VectorRect is 0 inflate undefined" );
	RRealSize	scale( (dim.m_dx+(size.m_dx*2))/dim.m_dx, (dim.m_dy+(size.m_dy*2))/dim.m_dy );
	UnrotateAndScaleAboutPoint( GetCenterPoint(), scale );
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::Scale
//
// Description:		Multiplies this vector rect by a scaling factor
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
inline void RVectorRect<T>::Scale( const RRealSize& scaleFactor )
	{
	m_UntransformedSize.Scale( scaleFactor );

	// Cheeze alert
	m_Transform.m_c1 *= scaleFactor.m_dx;
	m_Transform.m_c2 *= scaleFactor.m_dy;
	
	Recalculate( );
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::ScaleAboutPoint
//
// Description:		Scales this vector rect by applying a scale to the tranform
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
void RVectorRect<T>::ScaleAboutPoint( const RRealPoint& point, const RRealSize& scaleFactor )
	{
	m_Transform.PostScaleAboutPoint( point.m_x, point.m_y, scaleFactor.m_dx, scaleFactor.m_dy );
	Recalculate( );
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::UnrotateAndScaleAboutPoint
//
// Description:		Scales this vector rect by applying a scale to the tranform
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
void RVectorRect<T>::UnrotateAndScaleAboutPoint( const RRealPoint& point, const RRealSize& scaleFactor )
	{
	TpsAssert( (Width()!=0) && (Height()!=0), "Dimension of VectorRect is 0 inflate undefined" );
	// Calculate the rotation angle
	YAngle rotation = -::atan2( m_TopRight.m_y - m_TopLeft.m_y, m_TopRight.m_x - m_TopLeft.m_x );

	// Unrotate and scale
	m_Transform.PostTranslate( -point.m_x, -point.m_y );
	m_Transform.PostRotateAboutOrigin( -rotation );
	m_Transform.PostScale( scaleFactor.m_dx, scaleFactor.m_dy );
	m_Transform.PostRotateAboutOrigin( rotation );
	m_Transform.PostTranslate( point.m_x, point.m_y );

	// Recalc
	Recalculate( );
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::UnrotateAndScaleAboutPoint
//
// Description:		Scales this vector rect by applying a scale to the tranform
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
void RVectorRect<T>::UnrotateAndScaleAboutPoint( const RRealPoint& point, const RRealSize& scaleFactor, const RRealSize& minimumSize, const RRealSize& maximumSize, BOOLEAN fMaintainAspectRatio )
	{
	// Constrain the scale factor
	RRealSize constrainedScaleFactor = ConstrainScaleFactor( scaleFactor, minimumSize, maximumSize, fMaintainAspectRatio );

	// Do the scale
	UnrotateAndScaleAboutPoint( point, constrainedScaleFactor );
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::ResizeAboutPoint
//
// Description:		Scales this vector rect by directly modifiying the
//							rectangle size.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
void RVectorRect<T>::ResizeAboutPoint( const RRealPoint& point, const RRealSize& scaleFactor )
	{
	// Scale the rectangle size
	m_UntransformedSize.Scale( scaleFactor );

	// The scale was about the top left point. Fix up the rectangle position to correct for this.
	// First unrotate the top left point
	R2dTransform	transform;
	YAngle			rotation;
	YRealDimension	xScale;
	YRealDimension	yScale;
	m_Transform.Decompose( rotation, xScale, yScale );
	transform.PreRotateAboutPoint( point.m_x, point.m_y, -rotation );
	RRealPoint tempPoint = RRealPoint( 0, 0 ) * m_Transform;
	RRealPoint rotatedTopLeftPoint = tempPoint * transform;

	// Now calculate an offset that will correctly position the rectangle
	RRealSize offset;
	offset.m_dx = ( rotatedTopLeftPoint.m_x - point.m_x ) * ( scaleFactor.m_dx - 1.0 );
	offset.m_dy = ( rotatedTopLeftPoint.m_y - point.m_y ) * ( scaleFactor.m_dy - 1.0 );

	// Now rotate the offset vector back into the correct reference frame.
	transform.MakeIdentity( );
	transform.PreRotateAboutOrigin( rotation );
	offset *= transform;

	// Do the offset
	m_Transform.PostTranslate( offset.m_dx, offset.m_dy );

	// Recalc
	Recalculate( );
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::ResizeAboutPoint
//
// Description:		Scales this vector rect by directly modifiying the
//							rectangle size.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
void RVectorRect<T>::ResizeAboutPoint( const RRealPoint& point,
												   const RRealSize& scaleFactor,
													const RRealSize& minimumSize,
													const RRealSize& maximumSize,
													BOOLEAN fMaintainAspectRatio )
	{
	// Constrain the scale factor
	RRealSize constrainedScaleFactor = ConstrainScaleFactor( scaleFactor, minimumSize, maximumSize, fMaintainAspectRatio );

	// Do the scale
	ResizeAboutPoint( point, constrainedScaleFactor );
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::UnrotateAndResizeAboutPoint
//
// Description:		Scales this vector rect by applying a resize to the tranform
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
void RVectorRect<T>::UnrotateAndResizeAboutPoint( const RRealPoint& point, const RRealSize& scaleFactor )
	{
	TpsAssert( (Width()!=0) && (Height()!=0), "Dimension of VectorRect is 0 inflate undefined" );

	// Scale the rectangle size
	//m_UntransformedSize.Scale( scaleFactor );

	// Calculate the rotation angle
	YAngle rotation = -::atan2( m_TopRight.m_y - m_TopLeft.m_y, m_TopRight.m_x - m_TopLeft.m_x );
	// Unrotate and scale
	m_Transform.PostTranslate( -point.m_x, -point.m_y );
	m_Transform.PostRotateAboutOrigin( -rotation );

	//m_Transform.PostScale( scaleFactor.m_dx, scaleFactor.m_dy );
	Scale( scaleFactor );

	m_Transform.PostRotateAboutOrigin( rotation );
	m_Transform.PostTranslate( point.m_x, point.m_y );
	

	// Recalc
	Recalculate( );
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::UnrotateAndResizeAboutPoint
//
// Description:		Scales this vector rect by applying a resize to the tranform
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
void RVectorRect<T>::UnrotateAndResizeAboutPoint( const RRealPoint& point, const RRealSize& scaleFactor, const RRealSize& minimumSize, const RRealSize& maximumSize, BOOLEAN fMaintainAspectRatio )
	{
	// Constrain the scale factor
	RRealSize constrainedScaleFactor = ConstrainScaleFactor( scaleFactor, minimumSize, maximumSize, fMaintainAspectRatio );

	// Do the resize
	UnrotateAndResizeAboutPoint( point, constrainedScaleFactor );
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::GetTransform
//
// Description:		Accessor
//
// Returns:				Reference to the transform
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
RRealSize RVectorRect<T>::ConstrainScaleFactor( const RRealSize& scaleFactor, const RRealSize& minimumSize, const RRealSize& maximumSize, BOOLEAN fMaintainAspectRatio ) const
	{
	// Get the current size
	RRealSize currentSize = WidthHeight( );

	// Calculate the minimum and maximum scale factors
	RRealSize minimumScaleFactor( minimumSize.m_dx / currentSize.m_dx, minimumSize.m_dy / currentSize.m_dy );
	RRealSize maximumScaleFactor( maximumSize.m_dx / currentSize.m_dx, maximumSize.m_dy / currentSize.m_dy );

	TpsAssert( maximumScaleFactor.m_dx >= minimumScaleFactor.m_dx, "Min size > max size" );
	TpsAssert( maximumScaleFactor.m_dy >= minimumScaleFactor.m_dy, "Min size > max size" );

	// Constrain the scale factor so that it falls within the allowable range
	RRealSize constrainedScaleFactor( scaleFactor );

	if( constrainedScaleFactor.m_dx < minimumScaleFactor.m_dx )
		constrainedScaleFactor.m_dx = minimumScaleFactor.m_dx;

	if( constrainedScaleFactor.m_dx > maximumScaleFactor.m_dx )
		constrainedScaleFactor.m_dx = maximumScaleFactor.m_dx;

	if( constrainedScaleFactor.m_dy < minimumScaleFactor.m_dy )
		constrainedScaleFactor.m_dy = minimumScaleFactor.m_dy;

	if( constrainedScaleFactor.m_dy > maximumScaleFactor.m_dy )
		constrainedScaleFactor.m_dy = maximumScaleFactor.m_dy;

	// If we are maintaining aspect ratio, normalize the scale factor
	if( fMaintainAspectRatio )
		{
		if( constrainedScaleFactor.m_dx >= minimumScaleFactor.m_dy && constrainedScaleFactor.m_dx <= maximumScaleFactor.m_dy )
			constrainedScaleFactor.m_dy = constrainedScaleFactor.m_dx;
		else if( constrainedScaleFactor.m_dy >= minimumScaleFactor.m_dx && constrainedScaleFactor.m_dy <= maximumScaleFactor.m_dx )
			constrainedScaleFactor.m_dx = constrainedScaleFactor.m_dy;
		}

	return constrainedScaleFactor;
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::GetTransform
//
// Description:		Accessor
//
// Returns:				Reference to the transform
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
inline const R2dTransform& RVectorRect<T>::GetTransform( ) const
	{
	return m_Transform;
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::GetUntransformedSize
//
// Description:		Accessor
//
// Returns:				Reference to the original size
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
inline const RSize<T>& RVectorRect<T>::GetUntransformedSize( ) const
	{
	return m_UntransformedSize;
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::Width
//
// Description:		Accessor
//
// Returns:				Width of the transformed rect
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T> inline T RVectorRect<T>::Width( ) const
	{
	return (T)m_TopRight.Distance( m_TopLeft );
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::Height
//
// Description:		Accessor
//
// Returns:				Height of the transformed rect
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T> inline T RVectorRect<T>::Height( ) const
	{
	return (T)m_BottomLeft.Distance( m_TopLeft );
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::WidthHeight
//
// Description:		Accessor
//
// Returns:				Width and height of the transformed rect
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T> inline RSize<T> RVectorRect<T>::WidthHeight( ) const
	{
	return RSize<T>( Width( ), Height( ) );
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::Inset
//
// Description:		Insets this vector rect by the specified sizes
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T> void RVectorRect<T>::Inset( const RSize<T>& topLeft, const RSize<T>& bottomRight )
	{
	const RSize<T>& size = GetUntransformedSize( );
	RRealRect rect( topLeft.m_dx, topLeft.m_dy, size.m_dx - bottomRight.m_dx, size.m_dy - bottomRight.m_dy );
	RVectorRect<T> temp( rect );
	temp *= GetTransform( );
	*this = temp;
	}

// ****************************************************************************
//
// Function Name:		RVectorRect::GetPolygonPoints
//
// Description:		Fills in the specified array with the points of the 
//							vector rect polygon
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
void RVectorRect<T>::GetPolygonPoints( RPoint<T>* polygonPoints ) const
	{
	polygonPoints[ 0 ] = m_TopLeft;
	polygonPoints[ 1 ] = m_TopRight;
	polygonPoints[ 2 ] = m_BottomRight;
	polygonPoints[ 3 ] = m_BottomLeft;
	}

#ifdef _WINDOWS

// ****************************************************************************
//
// Function Name:		operator<<
//
// Description:		Insertion operator for vector rects
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RArchive& operator<<( RArchive& archive, const RVectorRect< YFloatType >& rect )
	{
	archive << rect.m_UntransformedSize;
	archive << rect.m_Transform;
	return archive;
	}

// ****************************************************************************
//
// Function Name:		operator>>
//
// Description:		Extraction operator for vector rects
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RArchive& operator>>( RArchive& archive, RVectorRect< YFloatType >& rect )
	{
	archive >> rect.m_UntransformedSize;
	archive >> rect.m_Transform;
	rect.Recalculate( );
	return archive;
	}

#endif	//	_WINDOWS

#ifdef	_WINDOWS

// ****************************************************************************
//
// Function Name:		::RotateRect( )
//
// Description:		Function for external applications (ie Park Row) so that
//							they can rotate rects. Renaissance should not use
//
//							The angle is a counterclockwise rotation in degrees
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T> void RotateRect( T& left, T& top, T& right, T& bottom, YAngle angle )
	{
	// Create a vector rect
	RVectorRect<T> vectorRect( left, top, right, bottom );

	// Rotate it
	vectorRect.Rotate( angle );

	// Copy the rotated bounding rect
	left = vectorRect.m_TransformedBoundingRect.m_Left;
	top = vectorRect.m_TransformedBoundingRect.m_Top;
	right = vectorRect.m_TransformedBoundingRect.m_Right;
	bottom = vectorRect.m_TransformedBoundingRect.m_Bottom;
	}

void RotateRect( CRect& rect, YAngle angle );

#endif	//	_WINDOWS


#undef __AssertFile__

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _VECTORRECT_H_
