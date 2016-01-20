// ****************************************************************************
//
//  File Name:			Font.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RFont and RFontList class
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
//  $Logfile:: /PM8/Framework/Source/Font.cpp                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"ATMFont.h"
#include "RasterFont.h"
#include "TrueTypeFont.h"
#include "CharacterInfo.h"
#include "ChunkyStorage.h"

#ifndef	IMAGER

#include "ApplicationGlobals.h"

#else

extern  RFontList            		gFontList;
extern  BOOLEAN				  		gfTrueTypeAvailable;
extern  BOOLEAN				  		gfATMAvailable;
#define GetAppFontList()			gFontList
#define IsTrueTypeAvailable()		gfTrueTypeAvailable
#define IsATMAvailable()  	  		gfATMAvailable

#endif	//	IMAGER

// ****************************************************************************
//
//  Function Name:	RFont::RFont( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFont::RFont( const YFontInfo& info, BOOLEAN fInitialize )
	: m_Info( info ),
	  m_uses( 0 ),
	  m_pWidthsTable( NULL ),
	  m_pKernTable( NULL ),
	  m_pPaths( NULL )
	{
	try
		{
		//	Don't initialize the information up front
		m_pWidthsTable = new RCharacterWidths( info );
		m_Info.width	= m_pWidthsTable->GetFontAverageWidth( );
		//	Initialize the Kern table AND path table with the new font info
		m_pKernTable	= new RKerningInfo( m_Info );
		m_pPaths			= new RCharacterPaths( m_Info );
		if ( fInitialize )
			{
			m_pWidthsTable->Initialize( );
			m_pKernTable->Initialize( );
			m_pPaths->Initialize( );
			}
		}
	catch( ... )
		{
		delete m_pWidthsTable;
		delete m_pKernTable;
		delete m_pPaths;
		throw;
		}
	}

// ****************************************************************************
//
//  Function Name:	RFont::~RFont( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFont::~RFont( )
	{
	delete m_pWidthsTable;
	delete m_pKernTable;
	delete m_pPaths;
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetInfo( )
//
//  Description:		Return a reference to the given font Info
//
//  Returns:			reference to info
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
const YFontInfo&	RFont::GetInfo( ) const
	{
	return m_Info;
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetInfo( )
//
//  Description:		Return a reference to the given font Info
//
//  Returns:			reference to info
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YFontInfo&	RFont::GetInfo( )
	{
	return m_Info;
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetHeight( )
//
//  Description:		Return the height of the font
//
//  Returns:			The font height
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YFontSize	RFont::GetHeight( ) const
	{
	return m_Info.height;
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetDimensions( )
//
//  Description:		Return the height of the font
//
//  Returns:			The font height
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YFontDimensions&	RFont::GetDimensions( ) const
	{
	return m_pWidthsTable->GetDimensions( );
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetAttributes( )
//
//  Description:		Return the style attributes of the font
//
//  Returns:			The font attributes
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YFontAttributes	RFont::GetAttributes( ) const
	{
	return m_Info.attributes;
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetName( )
//
//  Description:		Return the height of the font
//
//  Returns:			The font height
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void	RFont::GetName( LPSZ szName ) const
	{
	strcpy( szName, m_Info.sbName );
	}

// ****************************************************************************
//
//  Function Name:	RFont::IsAttribute( )
//
//  Description:		TRUE if the font has the desired attribute(s) set
//							FALSE otherwise
//
//  Returns:			TRUE if the attributes match, FALSE otherwise
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
BOOLEAN	RFont::IsAttribute( YFontAttributes flags ) const
	{
	return (BOOLEAN)(m_Info.attributes & flags);
	}


// ****************************************************************************
//
//  Function Name:	RFont::GetCharacterWidth( )
//
//  Description:		Return the width of the given character
//
//  Returns:			The requested character size
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YFontSize RFont::GetCharacterWidth( const RCharacter& character )
	{
	RDataObjectLocker	locker( m_pWidthsTable );
	return m_pWidthsTable->GetCharacterWidth( character );
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetKerningPair( )
//
//  Description:		Return the kern amount of the given characters
//
//  Returns:			The requested character	kern amount
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YKernSize RFont::GetKernAmount( const RCharacter& char1, const RCharacter& char2 ) const
	{
	RDataObjectLocker	locker( m_pKernTable );
	return m_pKernTable->GetKerningPair( char1, char2 );
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetCompensationAmount( )
//
//  Description:		Return the compensation amount of the font
//
//  Returns:			The requested compensation amount
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YKernSize RFont::GetCompensationAmount( )
	{
		return( ( GetCharacterWidth( RCharacter( 0x20 ) ) / 4 ) );	// REVEIW RAH will this do?
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetCharacterBounds( )
//
//  Description:		Return a rectangle that would bound the given character
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void RFont::GetCharacterBounds( const RCharacter& character, RIntRect* pBounds ) const
	{
	RDataObjectLocker	locker( m_pWidthsTable );
	pBounds->m_Left	= 0;
	pBounds->m_Right	= m_pWidthsTable->GetCharacterWidth( character );
	pBounds->m_Top		= m_pWidthsTable->GetAscent( );
	pBounds->m_Bottom	= -(signed)m_pWidthsTable->GetDescent( );
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetMaxCharBounds( )
//
//  Description:		Return a rectangle that would bound the widest character
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void RFont::GetMaxCharBounds( RIntRect* pBounds ) const
	{
	RDataObjectLocker	locker( m_pWidthsTable );
	pBounds->m_Left	= 0;
	pBounds->m_Right	= m_pWidthsTable->GetFontMaximumWidth( );
	pBounds->m_Top		= m_pWidthsTable->GetAscent( );
	pBounds->m_Bottom	= -(signed)m_pWidthsTable->GetDescent( );
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetStringWidth( )
//
//  Description:		Return the width of the given string of characters
//
//  Returns:			The requested string size
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YFontSize RFont::GetStringWidth( const RMBCString& string ) const
	{
	YFontSize				size = 0;

	RDataObjectLocker	locker( m_pWidthsTable );
	for ( RMBCStringIterator cz = string.Start(); cz != string.End(); ++cz )
		size += m_pWidthsTable->GetCharacterWidth( *cz );

	return size;
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetStringWidth( )
//
//  Description:		Return the width of the given string of characters
//
//  Returns:			The requested string size
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YFontSize RFont::GetStringWidth( const RMBCString& string, BOOLEAN fKern ) const
	{
	YFontSize				size = GetStringWidth( string );
	RMBCStringIterator	cz;
	RCharacter				character, next;

	if ( fKern )
		{
		cz = string.Start(); 
		character = *cz;
		++cz;
		while ( cz != string.End() )
			{
			next = *cz;
			size += GetKernAmount( character, next );
			character = next;
			++cz;
			}
		}

	return size;
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetStringBounds( )
//
//  Description:		Return a rectangle that would bound the given string of characters
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void RFont::GetStringBounds( const RMBCString& string, RIntRect* pBounds ) const
	{
	RDataObjectLocker	locker( m_pWidthsTable );
	pBounds->m_Left	= 0;
	pBounds->m_Right	= GetStringWidth( string );
	pBounds->m_Top		= m_pWidthsTable->GetAscent( );
	pBounds->m_Bottom	= -(signed)m_pWidthsTable->GetDescent( );
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetStringBounds( )
//
//  Description:		Return a rectangle that would bound the given string of characters
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void RFont::GetStringBounds( const RMBCString& string, RIntRect* pBounds, BOOLEAN fKern ) const
	{
	RDataObjectLocker	locker( m_pWidthsTable );
	pBounds->m_Left	= 0;
	pBounds->m_Right	= GetStringWidth( string, fKern );
	pBounds->m_Top		= m_pWidthsTable->GetAscent( );
	pBounds->m_Bottom	= -(signed)m_pWidthsTable->GetDescent( );
	}

// ****************************************************************************
//
//  Function Name:	RFont::AddUse( )
//
//  Description:		Increase the use counter
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void	RFont::AddUse( )
	{
	++m_uses;
	}

// ****************************************************************************
//
//  Function Name:	RFont::RemoveUse( )
//
//  Description:		Decrease the use counter
//
//  Returns:			TRUE if are no more uses of this font
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
BOOLEAN	RFont::RemoveUse( )
	{
	return static_cast<BOOLEAN>( (--m_uses == 0) );
	}

#ifdef	MAC
// ****************************************************************************
//
//  Function Name:	RFont::GetMacFontFamilyId( )
//
//  Description:		Return the Mac Font Family Id given a font name
//
//  Returns:			The value of the given font family
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
short RFont::GetMacFontFamilyId( const LPSZ lpszName ) 
	{
	short		sFontNum;
	Str255	ubName;
	
	strncpy( (Ptr)(ubName+1), lpszName, sizeof(ubName) );
	ubName[0]	= (uBYTE)strlen(lpszName);

	::GetFNum( ubName, &sFontNum );
	
	return sFontNum;
	}

// ****************************************************************************
//
//  Function Name:	RFont::GetMacStyleBits( )
//
//  Description:		Return the Mac Font Style bits given the YFontAttributes
//
//  Returns:			The value of the given font family
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
Style RFont::GetMacStyleBits( YFontAttributes attributes ) 
	{
	//	Set the style bits.
	Style	textStyle = 0;
	textStyle 	|= (attributes & RFont::kBold) ? bold : 0;
	textStyle 	|= (attributes & RFont::kItalic) ? italic : 0;
	textStyle 	|= (attributes & RFont::kUnderline) ? underline : 0;
	return textStyle;
	}

#endif	//	MAC

#ifdef	TPSDEBUG
// ****************************************************************************
//
//  Function Name:	RFont::GetUses( )
//
//  Description:		Decrease the use counter
//
//  Returns:			TRUE if are no more uses of this font
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YCounter	RFont::GetUses( )
	{
	return m_uses;
	}

// ****************************************************************************
//
//  Function Name:	RFont::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFont::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RFont, this );
	}

#endif	//	TPSDEBUG


// ****************************************************************************
//
//  Function Name:	operator==( YFontInfo& , YFontInfo&)
//
//  Description:		True if the font info structures match
//
//  Returns:			TRUE if the font info structures match
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
BOOLEAN YFontInfo::operator==( const YFontInfo& rhs ) const
	{
	TpsAssert( ulPadding == 0, "The Font1 padding field is not 0" );
	TpsAssert( rhs.ulPadding == 0, "The Font2 padding field is not 0" );
	
	return static_cast<BOOLEAN>( ( (attributes == rhs.attributes) &&
											 (height == rhs.height)	&&
											 //(width == rhs.width) &&
											 (angle == rhs.angle)	&&
											 (strcmp( sbName, rhs.sbName) == 0) ) );
	}

// ****************************************************************************
//
//  Function Name:	YFontInfo::operator!=( )
//
//  Description:		True if the font info structures DO NOT match
//
//  Returns:			TRUE if the font info structures DO NOT match
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
BOOLEAN YFontInfo::operator!=( const YFontInfo& rhs ) const
	{
	return static_cast<BOOLEAN>( !( *this == rhs ) );
	}

// ****************************************************************************
//
//  Function Name:	YFontInfo::operator*( )
//
//  Description:		Apply the transform to alter the FontInfo
//							This involves scaling the height by the transform.XScale
//							and changing the rotation by the transform XRotation
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YFontInfo YFontInfo::operator*( const R2dTransform& transform ) const
	{
	YFontInfo		newFont	= *this;
	YAngle			rotation;
	YRealDimension	xScale;
	YRealDimension	yScale;
	transform.Decompose( rotation, xScale, yScale );
	if ( xScale < yScale )
		yScale = xScale;
	else
		xScale = yScale;
	newFont.height		= ::Abs( ::Round( (YRealDimension)height * yScale ) );
	newFont.width		= ::Abs( ::Round( (YRealDimension)width * xScale ) );
	newFont.angle		+= (YIntDegrees)Round( RadiansToDegrees( rotation ) );
	return newFont;
	}

// ****************************************************************************
//
// Function Name:		operator<<
//
// Description:		Insertion operator for fonts
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
RArchive& operator<<( RArchive& archive, const YFileFont& font )
	{
	archive << font.info.attributes;
	archive << font.info.height;
	archive << font.info.angle;
	archive << font.info.ulPadding;
#ifdef _WINDOWS
	archive.Write( sizeof(font.info.sbName), reinterpret_cast<const uBYTE*>(font.info.sbName) );
#endif
#ifdef MAC
	archive.Write( sizeof(font.info.sbName), (uBYTE*)(font.info.sbName) );
#endif
	archive << font.ulUses;
	archive << font.color;
	return archive;
	}

// ****************************************************************************
//
// Function Name:		operator>>
//
// Description:		Extraction operator for fonts
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
RArchive& operator>>( RArchive& archive, YFileFont& font )
	{
	archive >> font.info.attributes;
	archive >> font.info.height;
	archive >> font.info.angle;
	archive >> font.info.ulPadding;
#ifdef _WINDOWS
	archive.Read( sizeof(font.info.sbName), reinterpret_cast<uBYTE*>(font.info.sbName) );
#endif
#ifdef MAC
	archive.Read( sizeof(font.info.sbName), (uBYTE*)(font.info.sbName) );
#endif
	archive >> font.ulUses;
	archive >> font.color;
	return archive;
	}

// ****************************************************************************
//			Static Support Functions
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RFontList::CreateFont( )
//
//  Description:		Make a RFont of the correct type
//
//  Returns:			pFont
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
#ifdef	_WINDOWS
	//		These are not the parameters listed in the Help file, but this are
	//		the parameters needed to compile the function .
	int CALLBACK EnumFamCallback( const LOGFONT FAR* /* lplf */, const TEXTMETRIC FAR* /* lpntm */, DWORD FontType, LPARAM lpfFontHasTrueType )
		{
		if (!(*(LPBOOLEAN)lpfFontHasTrueType))
			*(LPBOOLEAN)lpfFontHasTrueType = (BOOLEAN)(FontType & TRUETYPE_FONTTYPE);
		return TRUE;
		}	
#endif
RFont* RFont::CreateFont( const YFontInfo& fontInfo, BOOLEAN fInitialize )
	{
	RFont*	pFont = NULL;

	// Handle True Type fonts
	if ( (pFont == NULL) && IsTrueTypeAvailable( ) )
	{
#ifdef	_WINDOWS
		HDC		hdc = RFont::GetSystemDC( );
		if (hdc)
			{
			BOOLEAN	fFontHasTrueType = FALSE;
			EnumFontFamilies( hdc, fontInfo.sbName, &EnumFamCallback, (LPARAM)&fFontHasTrueType );;
///xxx	For now we must always assume we can at least get a TrueType alternative...
			if (fFontHasTrueType)
				pFont = new RTrueTypeFont( fontInfo, fInitialize );
//			if ( !fFontHasTrueType )
//				{
//				RenaissanceBeep( );
//				///xxx	REVIEW: Need a warning that the requested font was not available
//				//	and another was substitued.
//				}

			RFont::ReleaseSystemDC( );
			}
#endif	//	_WINDOWS

#ifdef	MAC
		CGrafPtr	pCPort = (CGrafPtr)::NewPtr( sizeof( CGrafPort ) );
		GrafPtr	holdPort;
		::GetPort( &holdPort );   
		if ( pCPort != NULL )
		{
			Point		ptNumer, ptDenom;
			::OpenCPort( pCPort );
			::SetPort( (GrafPtr)pCPort );
			Boolean	fOutlines = ::GetOutlinePreferred();
			ptNumer.h = 1;
			ptNumer.v = 1;
			ptDenom.h = 1;
			ptDenom.v = 1;
			::TextFont( RFont::GetMacFontFamilyId( (const LPSZ)fontInfo.sbName ) );
			::TextFace( RFont::GetMacStyleBits( fontInfo.attributes ) );
			::SetOutlinePreferred( true );				
			if ( ::IsOutline( ptNumer, ptDenom ) )
				pFont = new RTrueTypeFont( fontInfo, fInitialize );
			::SetOutlinePreferred( fOutlines );
			::CloseCPort( pCPort );
			::DisposePtr( (Ptr)pCPort );
		}
		::SetPort( holdPort );
#endif	//	MAC
	}
	
	// Handle ATM fonts
	if ( (pFont == NULL) && IsATMAvailable( ) )	
	{
#ifdef	_WINDOWS
		int	nFromOutline;
		//	I must call ATMProperlyLoaded on Windows since our Multiple DLL's might not
		//		think that it was called.
		if ( ATMProperlyLoaded() && 
				ATMFontAvailable ( (char*)fontInfo.sbName,
											(fontInfo.attributes & RFont::kBold)? FW_BOLD : FW_NORMAL,
											static_cast<BYTE>( (fontInfo.attributes & RFont::kItalic)? 1 : 0 ),
											static_cast<BYTE>( (fontInfo.attributes & RFont::kUnderline)? 1 : 0 ),
											0,
											&nFromOutline ) )
			pFont = new RATMFont( fontInfo, fInitialize );
#endif	//	_WINDOWS

#ifdef	MAC
#ifdef	powerc
		if ( fontAvailableATM( RFont::GetMacFontFamilyId( (const LPSZ)fontInfo.sbName ), 
					RFont::GetMacStyleBits( fontInfo.attributes ) ) == 1 )
			pFont = new RATMFont( fontInfo, fInitialize );
#else
	#error 68k not implemented
#endif	// power mac
#endif	//	MAC
	}

	// Last, but not least handle Raster fonts
	if ( pFont == NULL )
#ifdef _WINDOWS	//	Use TrueType and let the mapper fix it.
		pFont = new RTrueTypeFont( fontInfo, fInitialize );
#else
		pFont	= new RRasterFont( fontInfo, fInitialize );
#endif	//	_WINDOWS

	return pFont;
}

// ****************************************************************************
//
//  Function Name:	RFont::IsFontOnSystem( )
//
//  Description:		Make a RFont of the correct type
//
//  Returns:			pFont
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
#ifdef	_WINDOWS
	//		These are not the parameters listed in the Help file, but this are
	//		the parameters needed to compile the function .
	int CALLBACK EnumFamCallbackProc( const LOGFONT FAR* /* lplf */, const TEXTMETRIC FAR* /* lpntm */, DWORD , LPARAM lpfFontFound )
		{
		*(LPBOOLEAN)lpfFontFound = TRUE;
		return TRUE;
		}	
#endif

BOOLEAN	RFont::IsFontOnSystem( const char* pFontName )
{
	BOOLEAN	fFontFound = FALSE;

#ifdef	_WINDOWS
	HDC	hdc	= RFont::GetSystemDC( );
	EnumFontFamilies( hdc, pFontName, &EnumFamCallbackProc, (LPARAM)&fFontFound );
	RFont::ReleaseSystemDC( );
#endif	//	_WINDOWS

	return fFontFound;
}


#ifdef	_WINDOWS
HDC			_SystemDC				= NULL;
int			nSystemDCUseCount		= 0;
const	int	kMaxSystemDCUseCount	= 10;
// ****************************************************************************
//
//  Function Name:	RFont::GetSystemFont( )
//
//  Description:		Make a RFont of the correct type
//
//  Returns:			pFont
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
HDC RFont::GetSystemDC( )
{
	TpsAssert( nSystemDCUseCount >= 0, "SystemDC use count is negative" );
	TpsAssert( nSystemDCUseCount < kMaxSystemDCUseCount, "SystemDC use count is > MaxSystem, if this is OK, increase Max" );
	TpsAssert( (nSystemDCUseCount!=0)||(_SystemDC==NULL), "SystemDC use count is not 0, but DC is NULL" );

	if ( nSystemDCUseCount == 0 )
		_SystemDC	= ::GetDC( HWND_DESKTOP );
	if ( _SystemDC == NULL )
		throw	kResource;
	++nSystemDCUseCount;
	return _SystemDC;
}

// ****************************************************************************
//
//  Function Name:	RFont::ReleaseSystemFont( )
//
//  Description:		Make a RFont of the correct type
//
//  Returns:			pFont
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
void RFont::ReleaseSystemDC( )
{
	TpsAssert( nSystemDCUseCount >= 1, "ReleaseSystemDC Called, but use count < 1" );
	TpsAssert( _SystemDC != NULL, "ReleaseSystemDC Called, with SystemDC NULL" );

	if ( --nSystemDCUseCount == 0 )
	{
		::ReleaseDC( HWND_DESKTOP, _SystemDC );
		_SystemDC = NULL;
	}
}

#endif	//	_WINDOWS