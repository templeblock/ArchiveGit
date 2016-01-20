// ****************************************************************************
// @Doc
// @Module Color.inl |
//
//  File Name:			Color.inl
//
//  Project:			Renaissance Framework
//
//  Author:				Michael G. Dunlap
//
//  Description:		implementations of the inline RColor class members
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
//  $Logfile:: /Development/CalendarComp/Source/MonthlyCalendarDocument.cpp   $
//   $Author:: Mgd                                                            $
//     $Date:: 12/12/96 7:00a                                                 $
// $Revision:: 1                                                              $
//
// ****************************************************************************


#ifdef TPSDEBUG
#define	INLINE
#else
#define	INLINE inline
#endif // defined(TPSDEBUG)


//
// @MFunc Construct an RColor
//
// @Syntax RColor()
// @Syntax RColor( const RColor& rgb )
// @Syntax RColor( const RGradientFill& gradient )
//
// @Parm const RColor& | rgb | solid color for fill
// @Parm const RGradientFill& | gradient | gradient for fill
//
// @Comm the default ctor creates a transparent fill
//
INLINE RColor::RColor() :
	m_eFillMethod( kTransparent ),
	m_eFillEffect( kOpaque ),
	m_rgbFill( kBlack ),
	m_pGradient( NULL ),
	m_pImageColor( NULL )
{
}

INLINE RColor::RColor( EColors eColor ) :
	m_eFillMethod( kSolid ),
	m_eFillEffect( kOpaque ),
	m_rgbFill( eColor ),
	m_pGradient( NULL ),
	m_pImageColor( NULL )
{
}

INLINE RColor::RColor( YPlatformColor yColor ) :
	m_eFillMethod( kSolid ),
	m_eFillEffect( kOpaque ),
	m_rgbFill( yColor ),
	m_pGradient( NULL ),
	m_pImageColor( NULL )
{
}

INLINE RColor::RColor( const RSolidColor& rgb ) :
	m_eFillMethod( kSolid ),
	m_eFillEffect( kOpaque ),
	m_rgbFill( rgb ),
	m_pGradient( NULL ),
	m_pImageColor( NULL )
{
}

//
// @MFunc create a new RColor and transform it
// @RDesc the new RColor
//
INLINE RColor RColor::operator*(
			const R2dTransform&	transform )	// @Parm the transformation to apply
			const
{
	RColor	fill( *this );
	fill *= transform;
	return fill;
}

//
// @MFunc get the current fill effect (kOpaque or kTranslucent)
// @RDesc the fill effect
//
INLINE RColor::EFillEffect RColor::GetFillEffect( ) const
	{
	return m_eFillEffect;
	}

//
// @MFunc set the current fill effect (kOpaque or kTranslucent)
// @RDesc reference to this color
//
INLINE RColor& RColor::SetFillEffect( RColor::EFillEffect eEffect )
	{
	m_eFillEffect = eEffect;
	return *this;
	}

//
// @MFunc get the current fill method
// @RDesc the fill method
//
INLINE RColor::EFillMethod RColor::GetFillMethod( ) const
	{
	TpsAssert( m_eFillMethod >= kTransparent && m_eFillMethod <= kBitmap, "Invalid Fill Method." );
	return m_eFillMethod;
	}

//
// @MFunc get the gradient fill (asserts kGradient)
// @RDesc the gradient
//
INLINE const RGradientFill* RColor::GetGradientFill( ) const
	{
	TpsAssert( GetFillMethod( ) == kGradient, "Not gradient." );
	TpsAssert( m_pGradient != NULL, "No gradient." );
	return m_pGradient;
	}

//
// @MFunc get the bitmap fill image (asserts kBitmap)
// @RDesc the bitmap
//
INLINE const RImageColor* RColor::GetImageColor( ) const
	{
	TpsAssert( GetFillMethod( ) == kBitmap, "Not bitmap." );
	TpsAssert( m_pImageColor != NULL, "No bitmap." );
	return m_pImageColor;
	}

//
// @MFunc test this color for inequality with the given color
// @RDesc TRUE if the colors are different else FALSE
//
INLINE BOOLEAN RColor::operator!=(
			const RColor& rhs )	// @Parm the color to compare with
			const
	{
	return static_cast<BOOLEAN>( !operator==( rhs ) );
	}

//
// Serialization...
//
INLINE RArchive& operator<<( RArchive& ar, const RColor& fill )
{
	fill.Write( ar );
	return ar;
}

INLINE RArchive& operator>>( RArchive& ar, RColor& fill )
{
	fill.Read( ar );
	return ar;
}
