// ****************************************************************************
//
//  File Name:			GDIObjects.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the various GDI Objects
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
//  $Logfile:: /PM8/Framework/Source/GdiObjects.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "GdiObjects.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RGDIBrush::RGDIBrush( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGDIBrush::RGDIBrush( COLORREF color )
	: m_hBrush( ::CreateSolidBrush(color) ),
	  m_fCleanUp( TRUE )
	{
	if (m_hBrush == NULL)
		throw kResource;
	}

// ****************************************************************************
//
//  Function Name:	RGDIBrush::RGDIBrush( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGDIBrush::RGDIBrush( HBRUSH hbrush )
	: m_hBrush( hbrush ),
	  m_fCleanUp( FALSE )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RGDIBrush::~RGDIBrush( )
//
//  Description:		Destructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGDIBrush::~RGDIBrush( )
	{
	if (m_fCleanUp && (m_hBrush != NULL))
		::DeleteObject( m_hBrush );
	}

// ****************************************************************************
//
//  Function Name:	RGDIBrush::GetBrush( )
//
//  Description:		The Windows HBRUSH.
//
//  Returns:			m_hBrush
//
//  Exceptions:		None
//
// ****************************************************************************
//
HBRUSH RGDIBrush::GetBrush( )
	{
	return m_hBrush;
	}

// ****************************************************************************
//
//  Function Name:	RGDIBrush::operator==( )
//
//  Description:		Equality operator.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGDIBrush::operator==( const RGDIBrush& rhs )
	{
	return (BOOLEAN)(rhs.m_hBrush == m_hBrush);
	}

// ****************************************************************************
//
//  Function Name:	RGDIBrush::operator!=( )
//
//  Description:		Inequality operator.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGDIBrush::operator!=( const RGDIBrush& rhs )
	{
	return (BOOLEAN)(rhs.m_hBrush != m_hBrush);
	}


// ****************************************************************************
//
//  Function Name:	RGDIPenInfo::RGDIPenInfo( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGDIPenInfo::RGDIPenInfo( int nStyle, int nWidth, COLORREF color )
	: m_nStyle( nStyle ),
	  m_nWidth( nWidth ),
	  m_Color( color )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RGDIPenInfo::~RGDIPenInfo( )
//
//  Description:		Destructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGDIPenInfo::~RGDIPenInfo( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RGDIPenInfo::operator==()
//
//  Description:		Equality operator.
//
//  Returns:			TRUE if the values are the same
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGDIPenInfo::operator==( const RGDIPenInfo& rhs )
	{
	return static_cast<BOOLEAN>( (rhs.m_nStyle == m_nStyle) &&
										  (rhs.m_nWidth == m_nWidth) &&
										  (rhs.m_Color == m_Color) );
	}

// ****************************************************************************
//
//  Function Name:	RGDIPenInfo::operator!=( )
//
//  Description:		Inequality operator
//
//  Returns:			TRUE if values are not the same
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGDIPenInfo::operator!=( const RGDIPenInfo& rhs )
	{
	return static_cast<BOOLEAN>( !(*this==rhs) );
	}

// ****************************************************************************
//
//  Function Name:	RGDIPen::RGDIPen( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGDIPen::RGDIPen( const RGDIPenInfo& pen )
	: m_hPen( ::CreatePen( pen.m_nStyle, pen.m_nWidth, pen.m_Color ) ),
	  m_fCleanUp( TRUE )
	{
	if (m_hPen == NULL)
		throw kResource;
	}

// ****************************************************************************
//
//  Function Name:	RGDIPen::RGDIPen( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGDIPen::RGDIPen( HPEN hpen )
	: m_hPen( hpen ),
	  m_fCleanUp( FALSE )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RGDIPen::~RGDIPen( )
//
//  Description:		Destructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGDIPen::~RGDIPen( )
	{
	if (m_fCleanUp && (m_hPen != NULL))
		::DeleteObject( m_hPen );
	}

// ****************************************************************************
//
//  Function Name:	RGDIPen::GetPen( )
//
//  Description:		The Windows HPEN.
//
//  Returns:			m_hPen
//
//  Exceptions:		None
//
// ****************************************************************************
//
HPEN RGDIPen::GetPen( )
	{
	return m_hPen;
	}

// ****************************************************************************
//
//  Function Name:	RGDIPen::operator==( )
//
//  Description:		Equality operator.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGDIPen::operator==( const RGDIPen& rhs )
	{
	return (BOOLEAN)(rhs.m_hPen == m_hPen);
	}

// ****************************************************************************
//
//  Function Name:	RGDIPen::operator!=( )
//
                          //  Description:		Inequality operator.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGDIPen::operator!=( const RGDIPen& rhs )
	{
	return (BOOLEAN)!(rhs.m_hPen == m_hPen);
	}

// ****************************************************************************
//
//  Function Name:	RGDIFont::RGDIFont( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGDIFont::RGDIFont( const YFontInfo& fontInfo )
	: m_hFont( NULL ),
	  m_fCleanUp( TRUE )
	{
	LOGFONT	lf;
	memset( &lf, 0, sizeof(lf) );
	// with printshop 7 (10) changed this to be a negative number so we'll get true pointsize.
	// wrote a conversion function in fontlist.cpp to convert the fontheight to pointsize in old docs -ces
	lf.lfHeight				= -(signed)fontInfo.height;
	lf.lfWidth				= fontInfo.width;
	lf.lfEscapement		= fontInfo.angle * 10;
	lf.lfOrientation		= lf.lfEscapement;
	lf.lfWeight				= (fontInfo.attributes & RFont::kBold)? FW_BOLD : FW_NORMAL;
	lf.lfItalic				= static_cast<BYTE>( (fontInfo.attributes & RFont::kItalic)? TRUE : FALSE );
	lf.lfUnderline			= static_cast<BYTE>( (fontInfo.attributes & RFont::kUnderline)? TRUE : FALSE );
	lf.lfCharSet			= DEFAULT_CHARSET;//ANSI_CHARSET;
	lf.lfOutPrecision		= OUT_TT_ONLY_PRECIS;//OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	lf.lfQuality			= DEFAULT_QUALITY;
	lf.lfPitchAndFamily	= VARIABLE_PITCH | FF_SWISS;
	TpsAssert( strlen( fontInfo.sbName ) < LF_FACESIZE, "Face name size is too large" ); 
	strncpy( lf.lfFaceName, fontInfo.sbName, LF_FACESIZE );
	lf.lfFaceName[LF_FACESIZE-1] = '\0';  // force null termination

	//	Create the font
	m_hFont	= CreateFontIndirect( &lf );
	if (!m_hFont)
		throw kResource;

	}

// ****************************************************************************
//
//  Function Name:	RGDIFont::RGDIFont( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGDIFont::RGDIFont( HFONT hfont )
	: m_hFont( hfont ),
	  m_fCleanUp( FALSE )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RGDIFont::~RGDIFont( )
//
//  Description:		Destructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGDIFont::~RGDIFont( )
	{
	if (m_fCleanUp && (m_hFont != NULL))
		::DeleteObject( m_hFont );
	}

// ****************************************************************************
//
//  Function Name:	RGDIFont::GetFont( )
//
//  Description:		The Windows HFONT.
//
//  Returns:			m_hFont
//
//  Exceptions:		None
//
// ****************************************************************************
//
HFONT RGDIFont::GetFont( )
	{
	return m_hFont;
	}

// ****************************************************************************
//
//  Function Name:	RGDIFont::operator==( )
//
//  Description:		Equality operator.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGDIFont::operator==( const RGDIFont& rhs )
	{
	return (BOOLEAN)(rhs.m_hFont == m_hFont);
	}

// ****************************************************************************
//
//  Function Name:	RGDIFont::operator!=( )
//
//  Description:		Inequality operator.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGDIFont::operator!=( const RGDIFont& rhs )
	{
	return (BOOLEAN)!(rhs.m_hFont == m_hFont);
	}
