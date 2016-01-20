// ****************************************************************************
//
//  File Name:			SolidColor.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of RSolidColor
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
//  $Logfile:: /PM8/Framework/Include/SolidColor.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_SOLIDCOLOR_H_
#define		_SOLIDCOLOR_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RArchive;

#ifdef	_WINDOWS
	typedef COLORREF YPlatformColor;
	typedef uBYTE	YColorComponent;
	const YColorComponent kMaxColorComponent = 0xFF;
#else
	typedef RGBColor YPlatformColor;
	typedef uWORD	YColorComponent;
	const YColorComponent kMaxColorComponent = 0xFFFF;
#endif
	const YColorComponent kMinColorComponent = 0x00;

enum EColors { kBlack, kWhite, kYellow, kMagenta, kRed, kCyan, kGreen, kBlue, kGray, kLightGray, kDarkGray };

// ****************************************************************************
//
//  Class Name:	RSolidColor
//
//  Description:	Color class
//						
//
// ****************************************************************************
//
class FrameworkLinkage RSolidColor
	{
	// Construction
	public :
												RSolidColor( );
												RSolidColor( const RSolidColor& rhs );
												RSolidColor( YColorComponent red, YColorComponent green, YColorComponent blue );
												RSolidColor( EColors eColor );
												RSolidColor( YPlatformColor color );
												RSolidColor( const RSolidColor& left, const RSolidColor& right, YPercentage percent );

	// Operations
	public :
		void									DarkenBy( YScaleFactor scale );
		void									LightenBy( YScaleFactor scale );

#ifdef	MAC
				//		These are Mac only because a YColorComponent is a uBYTE under Windows
												RSolidColor( uBYTE ubRed, uBYTE ubGreen, uBYTE ubBlue );
#endif

	// Accessors
	public :
		YColorComponent					GetRed( ) const;
		YColorComponent					GetGreen( ) const;
		YColorComponent					GetBlue( ) const;
		YDrawMode							GetHighContrastDrawMode( ) const;
		BOOLEAN								IsDark( ) const;

	// Operators
	public :
		RSolidColor&						operator=( const RSolidColor& rhs );
		BOOLEAN								operator==( const RSolidColor& rhs ) const;
		BOOLEAN								operator!=( const RSolidColor& rhs ) const;

	// Conversions
	public :
												operator YPlatformColor( ) const;

	// Member data
	private :
		YPlatformColor						m_Color;
	};

FrameworkLinkage RArchive& operator<<( RArchive& archive, const RSolidColor& color );
FrameworkLinkage RArchive& operator>>( RArchive& archive, RSolidColor& color );

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RSolidColor::RSolidColor( )
//
//  Description:		Default constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RSolidColor::RSolidColor( )
	{
	NULL;
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
inline RSolidColor::RSolidColor( YColorComponent red, YColorComponent green, YColorComponent blue )
	{
#ifdef	_WINDOWS							 
	m_Color = RGB( red, green, blue );
#endif	// _WINDOWS
#ifdef	MAC
	m_Color.red		= red; 
	m_Color.green	= green;
	m_Color.blue	= blue;
#endif	// MAC
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
inline RSolidColor::RSolidColor( YPlatformColor color )
	{
	m_Color = color;
	}

// ****************************************************************************
//
//  Function Name:	RSolidColor::RSolidColor( )
//
//  Description:		Copy constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RSolidColor::RSolidColor( const RSolidColor& rhs )
	{
	m_Color = rhs.m_Color;
	}

// ****************************************************************************
//
//  Function Name:	RSolidColor::operator( )
//
//  Description:		Conversion operator
//
//  Returns:			A Windows COLORREF or Mac rgb color
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RSolidColor::operator YPlatformColor( ) const
	{
	return m_Color;
	}

// ****************************************************************************
//
//  Function Name:	RSolidColor::GetRed( )
//
//  Description:		Accessor
//
//  Returns:			The red component of this color
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YColorComponent RSolidColor::GetRed( ) const
	{
	return MacWinDos( m_Color.red, GetRValue(m_Color), XXX );
	}

// ****************************************************************************
//
//  Function Name:	RSolidColor::GetGreen( )
//
//  Description:		Accessor
//
//  Returns:			The green component of this color
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YColorComponent RSolidColor::GetGreen( ) const
	{
	return MacWinDos( m_Color.green, GetGValue(m_Color), XXX );
	}

// ****************************************************************************
//
//  Function Name:	RSolidColor::GetBlue( )
//
//  Description:		Accessor
//
//  Returns:			The blue component of this color
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YColorComponent RSolidColor::GetBlue( ) const
	{
	return MacWinDos( m_Color.blue, GetBValue(m_Color), XXX );
	}

// ****************************************************************************
//
//  Function Name:	RSolidColor::operator=( )
//
//  Description:		Assignment operator
//
//  Returns:			Reference to this color
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RSolidColor& RSolidColor::operator=( const RSolidColor& rhs )
	{
	m_Color = rhs.m_Color;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RSolidColor::operator==( )
//
//  Description:		Equality operator
//
//  Returns:			TRUE if the two colors are exactly equal
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RSolidColor::operator==( const RSolidColor& rhs ) const
	{
#ifdef	_WINDOWS							 
	return static_cast<BOOLEAN>( m_Color == rhs.m_Color );
#endif	// _WINDOWS
#ifdef	MAC							 
	return ( m_Color.red == rhs.m_Color.red && 
				m_Color.green == rhs.m_Color.green && 
				m_Color.blue == rhs.m_Color.blue ); 
#endif	// MAC
	}

// ****************************************************************************
//
//  Function Name:	RSolidColor::operator!=( )
//
//  Description:		Inequality operator
//
//  Returns:			TRUE if the two colors are not exactly equal
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RSolidColor::operator!=( const RSolidColor& rhs ) const
	{
#ifdef	_WINDOWS							 
	return static_cast<BOOLEAN>( m_Color != rhs.m_Color );
#endif	// _WINDOWS
#ifdef	MAC							 
	return ( m_Color.red != rhs.m_Color.red || 
				m_Color.green != rhs.m_Color.green || 
				m_Color.blue != rhs.m_Color.blue ); 
#endif	// MAC
	}

// ****************************************************************************
//
//  Function Name:	Mix( )
//
//  Description:		To combine two colors acording to a given percentage
//
//  Returns:			The mixed color
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RSolidColor Mix( const RSolidColor& left, const RSolidColor& right, YPercentage percent )
	{
	return RSolidColor( left, right, percent );
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _SOLIDCOLOR_H_
