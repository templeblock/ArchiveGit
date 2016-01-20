// ****************************************************************************
//
//  File Name:			SolidColor.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of RSolidColor
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
//  $Logfile:: /PM8/Framework/Source/SolidColor.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifdef	MAC
// ****************************************************************************
//
//  Function Name:	RSolidColor::RSolidColor( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSolidColor::RSolidColor( uBYTE red, uBYTE green, uBYTE blue )
	{
	m_Color.red = ((red << 8) | red); 
	m_Color.green = ((green << 8) | green);
	m_Color.blue = ((blue << 8) | blue);
	}
#endif

// ****************************************************************************
//
//  Function Name:	RSolidColor::RSolidColor( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSolidColor::RSolidColor( const RSolidColor& left, const RSolidColor& right, YPercentage percent )
	{
	int	nRedValue	= right.GetRed() - left.GetRed();
	int	nGreenValue	= right.GetGreen() - left.GetGreen();
	int	nBlueValue	= right.GetBlue() - left.GetBlue();
	TpsAssert( percent >= 0.0 && percent <= 1.0, "Percent is not in range 0 - 1" );
	*this = RSolidColor( static_cast<YColorComponent>( left.GetRed() + (nRedValue * percent) ),
								static_cast<YColorComponent>( left.GetGreen() + (nGreenValue * percent) ),
								static_cast<YColorComponent>( left.GetBlue() + (nBlueValue * percent) ) );
	}

// ****************************************************************************
//
//  Function Name:	RSolidColor::RSolidColor( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSolidColor::RSolidColor( EColors eColor )
	{
	switch ( eColor )
		{
#ifdef	_WINDOWS							 
		case kBlack :
			m_Color	= RGB( 0x00, 0x00, 0x00 );
			break;
		case kWhite :
			m_Color	= RGB( 0xFF, 0xFF, 0xFF );
			break;
		case kYellow :
			m_Color	= RGB( 0xFF, 0xFF, 0 );
			break;
		case kMagenta :
			m_Color	= RGB( 0xFF, 0, 0xFF );
			break;
		case kRed :
			m_Color	= RGB( 0xFF, 0, 0 );
			break;
		case kCyan :
			m_Color	= RGB( 0, 0xFF, 0xFF );
			break;
		case kGreen :
			m_Color	= RGB( 0, 0xFF, 0 );
			break;
		case kBlue :
			m_Color	= RGB( 0, 0, 0xFF );
			break;
		case kGray :
			m_Color	= RGB( 0x80, 0x80, 0x80 );
			break;
		case kLightGray :
			m_Color	= RGB( 0xC0, 0xC0, 0xC0 );
			break;
		case kDarkGray :
			m_Color	= RGB( 0x40, 0x40, 0x40 );
			break;
#endif	// _WINDOWS
#ifdef	MAC							 
		case kBlack :
			m_Color.red = 0x0000; 
			m_Color.green = 0x0000;
			m_Color.blue = 0x0000;
			break;
		case kWhite :
			m_Color.red = 0xFFFF; 
			m_Color.green = 0xFFFF;
			m_Color.blue = 0xFFFF;
			break;
		case kYellow :
			m_Color.red = 0xFC00; 
			m_Color.green = 0xF37D;
			m_Color.blue = 0x052F;
			break;
		case kMagenta :
			m_Color.red = 0xF2D7; 
			m_Color.green = 0x0856;
			m_Color.blue = 0x84EC;
			break;
		case kRed :
			m_Color.red = 0xDD6B; 
			m_Color.green = 0x08C2;
			m_Color.blue = 0x06A2;
			break;
		case kCyan :
			m_Color.red = 0x0241; 
			m_Color.green = 0xAB54;
			m_Color.blue = 0xEAFF;
			break;
		case kGreen :
			m_Color.red = 0x0000; 
			m_Color.green = 0x8000;
			m_Color.blue = 0x11B0;
			break;
		case kBlue :
			m_Color.red = 0x0000; 
			m_Color.green = 0x0000;
			m_Color.blue = 0xD400;
			break;
		case kGray :
			m_Color.red = 0x8080; 
			m_Color.green = 0x8080;
			m_Color.blue = 0x8080;
			break;
		case kLightGray :
			m_Color.red = 0xC0C0; 
			m_Color.green = 0xC0C0;
			m_Color.blue = 0xC0C0;
			break;
		case kDarkGray :
			m_Color.red = 0x4040; 
			m_Color.green = 0x4040;
			m_Color.blue = 0x4040;
			break;
#endif	// MAC
		default :
			TpsAssertAlways( "Unrecognized color type." );
			break;
		}
	}

// ****************************************************************************
//
//  Function Name:	RSolidColor::LightenBy( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSolidColor::LightenBy( YScaleFactor scale )
	{
	YColorComponent	red	= GetRed();
	YColorComponent	green	= GetGreen();
	YColorComponent	blue	= GetBlue();
	uLONG				redder	= static_cast<uLONG>( ::Round( static_cast<YRealDimension>( kMaxColorComponent - red )  * scale ) );
	uLONG				greener	= static_cast<uLONG>( ::Round( static_cast<YRealDimension>( kMaxColorComponent - green )  * scale ) );
	uLONG				bluer		= static_cast<uLONG>( ::Round( static_cast<YRealDimension>( kMaxColorComponent - blue )  * scale ) );

	TpsAssert( ( scale >= 0.0 ), "Negative color component makes no sense." );
	TpsAssert( ( scale <= 1.0 ), "Bad color component scale factor." );

	redder = red + redder;
	if ( redder > kMaxColorComponent )
		red = kMaxColorComponent;
	else
		red = static_cast<YColorComponent>( redder );
	greener = green + greener;
	if ( greener > kMaxColorComponent )
		green = kMaxColorComponent;
	else
		green = static_cast<YColorComponent>( greener );
	bluer = blue + bluer;
	if ( bluer > kMaxColorComponent )
		blue = kMaxColorComponent;
	else
		blue = static_cast<YColorComponent>( bluer );

#ifdef	_WINDOWS							 
	m_Color = RGB( red, green, blue );
#endif	// _WINDOWS
#ifdef	MAC							 
	m_Color.red = red;
	m_Color.green = green;
	m_Color.blue = blue;
#endif	// MAC
	}

// ****************************************************************************
//
//  Function Name:	RSolidColor::DarkenBy( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSolidColor::DarkenBy( YScaleFactor scale )
	{
	YColorComponent	red	= GetRed();
	YColorComponent	green	= GetGreen();
	YColorComponent	blue	= GetBlue();
	uLONG				redder	= static_cast<uLONG>( ::Round( static_cast<YRealDimension>( red )  * scale ) );
	uLONG				greener	= static_cast<uLONG>( ::Round( static_cast<YRealDimension>( green )  * scale ) );
	uLONG				bluer		= static_cast<uLONG>( ::Round( static_cast<YRealDimension>( blue )  * scale ) );

	TpsAssert( ( scale >= 0.0 ), "Negative color component makes no sense." );
	TpsAssert( ( scale <= 1.0 ), "Bad color component scale factor." );

	red	= ( redder > red ) ? kMinColorComponent : static_cast<YColorComponent>( red - redder );
	green	= ( greener > green ) ? kMinColorComponent : static_cast<YColorComponent>( green - greener );
	blue	= ( bluer > blue ) ? kMinColorComponent : static_cast<YColorComponent>( blue - bluer );

#ifdef	_WINDOWS							 
	m_Color = RGB( red, green, blue );
#endif	// _WINDOWS
#ifdef	MAC							 
	m_Color.red = red;
	m_Color.green = green;
	m_Color.blue = blue;
#endif	// MAC
	}

// ****************************************************************************
//
//  Function Name:	RSolidColor::GetHighContrastDrawMode( )
//
//  Description:		Calculates the draw mode that should be used to draw this
//							color on top of the same color to ensure high contrast.
//
//  Returns:			The calculated drawmode
//
//  Exceptions:		None
//
// ****************************************************************************
//
YDrawMode RSolidColor::GetHighContrastDrawMode( ) const
	{
	// If the color is dark, use XNOR to go to white. Otherwise use XOR to go to black
	if( IsDark( ) )
		return kXNOR;
	else
		return kXOR;
	}

// ****************************************************************************
//
//  Function Name:	RSolidColor::IsDark( )
//
//  Description:		Determines if this color is "dark" or not. It works by
//							converting the RGB to a luminance (Y) value. The conversion
//							was taken from the Color FAQ, which is available from:
//							ftp://ftp.inforamp.net/pub/users/poynton/doc/colour/
//
//  Returns:			TRUE if this color is dark
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RSolidColor::IsDark( ) const
	{
	// The color weights
	const int kRedWeight = 3;
	const int kGreenWeight = 10;
	const int kBlueWeight = 1;

	// A color is dark if the weighted sum of the components is less than this value:
	const int kDark = ( kMaxColorComponent * ( kRedWeight + kGreenWeight + kBlueWeight ) ) / 2;

	return static_cast<BOOLEAN>( ( GetRed( ) * kRedWeight ) + ( GetGreen( ) * kGreenWeight ) + ( GetBlue( ) * kBlueWeight ) < kDark );
	}

// ****************************************************************************
//
// Function Name:		operator<<
//
// Description:		Insertion operator for solid colors
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
RArchive& operator<<( RArchive& archive, const RSolidColor& color )
	{
	uWORD		uwRedComponent		= color.GetRed( );
	uWORD		uwGreenComponent	= color.GetGreen( );
	uWORD		uwBlueComponent	= color.GetBlue( );

#ifdef	_WINDOWS
	uwRedComponent		|=	static_cast<uWORD>(static_cast<uBYTE>(uwRedComponent) << 8);
	uwGreenComponent	|=	static_cast<uWORD>(static_cast<uBYTE>(uwGreenComponent) << 8);
	uwBlueComponent	|=	static_cast<uWORD>(static_cast<uBYTE>(uwBlueComponent) << 8);
#endif	//	_WINDOWS

	archive << uwRedComponent << uwGreenComponent << uwBlueComponent;
	return archive;
	}

// ****************************************************************************
//
// Function Name:		operator>>
//
// Description:		Extraction operator for solid colors
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
RArchive& operator>>( RArchive& archive, RSolidColor& color )
	{
	uWORD		uwRedComponent;
	uWORD		uwGreenComponent;
	uWORD		uwBlueComponent;

	archive >> uwRedComponent >> uwGreenComponent >> uwBlueComponent;

#ifdef	_WINDOWS
	uwRedComponent		=	static_cast<uWORD>(uwRedComponent >> 8);
	uwGreenComponent	=	static_cast<uWORD>(uwGreenComponent >> 8);
	uwBlueComponent	=	static_cast<uWORD>(uwBlueComponent >> 8);
#endif	//	_WINDOWS

	color = RSolidColor( static_cast<YColorComponent>(uwRedComponent), 
								static_cast<YColorComponent>(uwGreenComponent), 
								static_cast<YColorComponent>(uwBlueComponent) );
	return archive;
	}
