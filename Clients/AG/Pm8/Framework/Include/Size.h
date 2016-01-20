// ****************************************************************************
//
//  File Name:			Size.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RSize class
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
//  $Logfile:: /PM8/Framework/Include/Size.h                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_SIZE_H_
#define		_SIZE_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RArchive;

template <class T> class RSize;
typedef	RSize<YIntDimension>		RIntSize;
typedef	RSize<YRealDimension>	RRealSize;

// ****************************************************************************
//
//  Class Name:		RSize
//
//  Description:		Two dimensional parameterized size class.
//
// ****************************************************************************
//
template <class T>
class RSize
	{
	// Construction & destruction
	public :
												RSize( );
												RSize( T x, T y );
												RSize( const RRealSize& rhs );
												RSize( const RIntSize& rhs );

	// Operators
	public :
		RSize<T>&							operator=( const RRealSize& rhs );
		RSize<T>&							operator=( const RIntSize& rhs );

		RSize<T>&							operator*=( const R2dTransform& rhs );
		RSize<T>								operator*( const R2dTransform& rhs ) const;

		BOOLEAN								operator==( const RSize<T>& rhs ) const;
		BOOLEAN								operator!=( const RSize<T>& rhs ) const;

	// Operations
	public :
		RSize<T>&							Scale( const RSize<YRealDimension>& scaleFactor );
		void									FitInside( RSize<T>& rhs );
		void									FitOutside( RSize<T>& rhs );

	// Member data
	public :
		T										m_dx;
		T										m_dy;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RSize::RSize( )
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
inline RSize<T>::RSize( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSize::RSize( )
//
//  Description:		Constructs a size given two dimensions
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T>
inline RSize<T>::RSize( T x, T y ) : m_dx( x ), m_dy( y )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSize::RSize( )
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
inline RSize<T>::RSize( const RSize<YIntDimension>& rhs ) : m_dx( rhs.m_dx ), m_dy( rhs.m_dy )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSize::RSize( )
//
//  Description:		Copy constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RSize<YRealDimension>::RSize( const RSize<YRealDimension>& rhs ) : m_dx( rhs.m_dx ), m_dy( rhs.m_dy )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSize::RSize( )
//
//  Description:		Copy constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RSize<YIntDimension>::RSize( const RSize<YRealDimension>& rhs )
	{
	m_dx = ::Round( rhs.m_dx );
	m_dy = ::Round( rhs.m_dy );
	}

// ****************************************************************************
//
//  Function Name:	RSize::operator=( )
//
//  Description:		Copy operator
//
//  Returns:			Reference to this point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline RSize<T>& RSize<T>::operator=( const RSize<YIntDimension>& rhs )
	{
	m_dx = rhs.m_dx;
	m_dy = rhs.m_dy;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RSize::operator=( )
//
//  Description:		Copy operator
//
//  Returns:			Reference to this point
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RSize<YRealDimension>& RSize<YRealDimension>::operator=( const RSize<YRealDimension>& rhs )
	{
	m_dx = rhs.m_dx;
	m_dy = rhs.m_dy;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RSize::operator=( )
//													
//  Description:		Copy operator
//
//  Returns:			Reference to this point
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RSize<YIntDimension>& RSize<YIntDimension>::operator=( const RSize<YRealDimension>& rhs )
	{
	m_dx = ::Round( rhs.m_dx );
	m_dy = ::Round( rhs.m_dy );
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RSize::operator==( )
//
//  Description:		Equality operator
//
//  Returns:			TRUE if the two sizes are equal
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline BOOLEAN RSize<T>::operator==( const RSize<T>& rhs ) const
	{
	return static_cast<BOOLEAN>( m_dx == rhs.m_dx && m_dy == rhs.m_dy );
	}

// ****************************************************************************
//
//  Function Name:	RSize::operator==( )
//
//  Description:		Equality operator
//
//  Returns:			TRUE if the two sizes are equal
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RSize<YRealDimension>::operator==( const RSize<YRealDimension>& rhs ) const
	{
	return static_cast<BOOLEAN>( ::AreFloatsEqual( m_dx, rhs.m_dx ) && ::AreFloatsEqual( m_dy, rhs.m_dy ) );
	}

// ****************************************************************************
//
//  Function Name:	RSize::operator==( )
//
//  Description:		Inequality operator
//
//  Returns:			TRUE if the two sizes are not equal
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline BOOLEAN RSize<T>::operator!=( const RSize<T>& rhs ) const
	{
	return static_cast<BOOLEAN>( !operator==( rhs ) );
	}

// ****************************************************************************
//
// Function Name:		RSize::operator=*( )
//
// Description:		Multiplies this size by the given transform
//
// Returns:				Reference to this size
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
RSize<T>& RSize<T>::operator*=( const R2dTransform& rhs )
	{
	//	sizes do not account for translation...
	T tempX = (T)( ( m_dx * rhs.m_a1 ) + ( m_dy * rhs.m_b1 ) );
	T tempY = (T)( ( m_dx * rhs.m_a2 ) + ( m_dy * rhs.m_b2 ) );

	m_dx = tempX;
	m_dy = tempY;
	return *this;
	}

// ****************************************************************************
//
// Function Name:		RSize::operator*=( )
//
// Description:		Multiplies this size by the given transform
//
//							Specialized for ints so I can round.
//
// Returns:				Reference to this size
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RSize<YIntCoordinate>& RSize<YIntCoordinate>::operator*=( const R2dTransform& rhs )
	{
	//	sizes do not account for translation...
	YIntCoordinate tempX = (YIntCoordinate)::Round( ( ( m_dx * rhs.m_a1 ) + ( m_dy * rhs.m_b1 ) ) );
	YIntCoordinate tempY = (YIntCoordinate)::Round( ( ( m_dx * rhs.m_a2 ) + ( m_dy * rhs.m_b2 ) ) );

	m_dx = tempX;
	m_dy = tempY;
	return *this;
	}

// ****************************************************************************
//
// Function Name:		RSize::operator*( )
//
// Description:		Returns a size that is the product of this size and the
//							given transform
//
// Returns:				The transformed size
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
inline RSize<T> RSize<T>::operator*( const R2dTransform& rhs ) const
	{
	RSize<T> temp = *this;
	temp	*= rhs;
	return temp;
	}

// ****************************************************************************
//
// Function Name:		RSize::Scale
//
// Description:		Multiplies this size by a scaling factor
//
// Returns:				Reference to this size
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T>
inline RSize<T>& RSize<T>::Scale( const RSize<YRealDimension>& scaleFactor )
	{
	m_dx *= scaleFactor.m_dx;
	m_dy *= scaleFactor.m_dy;
	return *this;
	}

// ****************************************************************************
//
// Function Name:		RSize::FitInside
//
// Description:		Fit this size aspect correct inside the given size
//
// Returns:				
//
// Exceptions:			
//
// ****************************************************************************
//
template <class T>
inline void RSize<T>::FitInside(RSize<T>& rhs)
	{
	YFloatType nAs = static_cast<YRealDimension>(m_dx) / static_cast<YRealDimension>(m_dy);
	YFloatType nAd = static_cast<YRealDimension>(rhs.m_dx) / static_cast<YRealDimension>(rhs.m_dy);
	YFloatType nScale = (nAs < nAd) ? static_cast<YFloatType>(rhs.m_dy) / static_cast<YFloatType>(m_dy) : static_cast<YFloatType>(rhs.m_dx) / static_cast<YFloatType>(m_dx);
	Scale(RRealSize(nScale, nScale));
	}

// ****************************************************************************
//
// Function Name:		RSize::Scale
//
// Description:		Fit this size aspect correct outside the given size
//
// Returns:				
//
// Exceptions:			
//
// ****************************************************************************
//
template <class T>
inline void RSize<T>::FitOutside(RSize<T>& rhs)
	{
	YFloatType nAs = static_cast<YRealDimension>(m_dx) / static_cast<YRealDimension>(m_dy);
	YFloatType nAd = static_cast<YRealDimension>(rhs.m_dx) / static_cast<YRealDimension>(rhs.m_dy);
	YFloatType nScale = (nAs > nAd) ? static_cast<YFloatType>(rhs.m_dy) / static_cast<YFloatType>(m_dy) : static_cast<YFloatType>(rhs.m_dx) / static_cast<YFloatType>(m_dx);
	Scale(RRealSize(nScale, nScale));
	}

// ****************************************************************************
//
// Function Name:		RSize::Scale
//
// Description:		Multiplies this size by a scaling factor
//
//							Specialized version for ints that rounds
//
// Returns:				Reference to this size
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RSize<YIntCoordinate>& RSize<YIntCoordinate>::Scale( const RSize<YRealDimension>& scaleFactor )
	{
	m_dx = ::Round( m_dx * scaleFactor.m_dx );
	m_dy = ::Round( m_dy * scaleFactor.m_dy );
	return *this;
	}

// ****************************************************************************
//
// Function Name:		operator<<
//
// Description:		Insertion operator for sizes
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T> inline RArchive& operator<<( RArchive& archive, const RSize< T >& size )
	{
	archive << size.m_dx << size.m_dy;
	return archive;
	}

// ****************************************************************************
//
// Function Name:		operator>>
//
// Description:		Extraction operator for sizes
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
template <class T> inline RArchive& operator>>( RArchive& archive, RSize< T >& size )
	{
	archive >> size.m_dx >> size.m_dy;
	return archive;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _SIZE_H_
