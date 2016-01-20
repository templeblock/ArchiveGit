// ****************************************************************************
//
//  File Name:			CharacterInfo.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RCharacterWidths and RKerningInfo class
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
//  $Logfile:: /PM8/Framework/Source/CharacterInfo.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "CharacterInfo.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifndef	IMAGER
	#include "ApplicationGlobals.h"
#else
	#include "GlobalBuffer.h"
#endif

#ifdef	_WINDOWS
	#include "DcDrawingSurface.h"
#endif	//	_WINDOWS

#include "Path.h"

//	macros
#define RoundFixed( f ) 			( ( f < 0 ) ? -FixRound( -f ) : FixRound( f ) )

const	YKernSize kDefaultKernPairValue	= 0;

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::RCharacterWidths( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCharacterWidths::RCharacterWidths( const YFontInfo& info )
	: m_nFirstCharacter( 0 ),
	  m_nCharacters( 0 ),
	  m_FontInfo( info ),
	  m_pWidthTable( NULL ),
	  m_AverageWidth( 0 ),
	  m_MaximumWidth( 0 ),
	  m_DefaultHeight( info.height )
	{
	//	Width must be zero so we can compute the aspect correct width for device
	m_FontInfo.width	= 0;	

#ifdef	_WINDOWS

	TEXTMETRIC		tm;
	RRealSize		scaleFactor( 1.0, 1.0 );
	BOOLEAN			fScreenDC		= FALSE;
	HDC				hdc				= GetDefaultPrinterDC( );

	if (!hdc)
		{
		fScreenDC	= TRUE;
		hdc			= ::GetDC( HWND_DESKTOP );
		}

	try
		{
		//	Convert the point size from 1440 to printer point size -
		//		we only care about the Y scaling for fonts...
		if ( hdc )
			{
			RDcDrawingSurface		ds;
			ds.Initialize( hdc, hdc );
			DeviceUnitsToLogicalUnits( scaleFactor, ds );
			ds.DetachDCs( );
			}

		//	Convert the height to device units by dividing by the scale factor.
		R2dTransform	transform;
		transform.PreScale( 1.0 / scaleFactor.m_dy, 1.0 / scaleFactor.m_dy );
		YFontInfo	fontInfo		= m_FontInfo * transform;
		RGDIFont		font( fontInfo );
		RGDIFont		oldFont( (HFONT)::SelectObject( hdc, font.GetFont() ) );

		if ( hdc && GetTextMetrics( hdc, &tm ) )
			{
			m_nFirstCharacter	= tm.tmFirstChar;
			m_nCharacters		= tm.tmLastChar - tm.tmFirstChar + 1;
			m_AverageWidth		= tm.tmAveCharWidth;
			m_MaximumWidth		= tm.tmMaxCharWidth;
			m_DefaultHeight	= tm.tmAscent + tm.tmDescent + tm.tmExternalLeading;
			
			m_Dimensions.m_Overhang			= tm.tmOverhang;
			m_Dimensions.m_Ascent			= tm.tmAscent;
			m_Dimensions.m_Descent			= tm.tmDescent;
			m_Dimensions.m_Leading			= tm.tmExternalLeading;
			}
		else
			{
			//	If we can't get the text metrics, try to be reasonable
			m_nFirstCharacter	= 0;
			m_nCharacters		= 256;		//	Default for Roman font
			m_AverageWidth		= fontInfo.height >> 1;										//	1/2 height for lack of better
			m_MaximumWidth		= (fontInfo.height >> 1) + (fontInfo.height >> 2);	//	3/4 height for lack of better
			m_DefaultHeight	= fontInfo.height;

			m_Dimensions.m_Overhang			= 0;
			m_Dimensions.m_Ascent			= (fontInfo.height >> 1) + (fontInfo.height >> 2) - 1;		//	3/4 height for lack of better
			m_Dimensions.m_Descent			= (fontInfo.height >> 2) - 1;											//	1/4 height for lack of better
			m_Dimensions.m_Leading			= 2;						//	2 pixel for absolutely no reason what-so-ever
			}

		//	
		//	Convert the dimensions to Logical units
		m_AverageWidth				= YFontSize( (YRealDimension)m_AverageWidth * scaleFactor.m_dx );
		m_MaximumWidth				= YFontSize( (YRealDimension)m_MaximumWidth * scaleFactor.m_dx );
		m_DefaultHeight			= YFontSize( (YRealDimension)m_DefaultHeight * scaleFactor.m_dy );

		m_Dimensions.m_Overhang	= YFontSize( (YRealDimension)m_Dimensions.m_Overhang * scaleFactor.m_dy );
		m_Dimensions.m_Ascent	= YFontSize( (YRealDimension)m_Dimensions.m_Ascent * scaleFactor.m_dy );
		m_Dimensions.m_Descent	= YFontSize( (YRealDimension)m_Dimensions.m_Descent * scaleFactor.m_dy );
		m_Dimensions.m_Leading	= YFontSize( (YRealDimension)m_Dimensions.m_Leading * scaleFactor.m_dy );

		//	What will happen if the HDC is deleted but the new font is still in it??
		::SelectObject( hdc, oldFont.GetFont( ) );
		}
	catch( YException	/* exception */)
		{
		;
		}

	// Clean up
	if( hdc && fScreenDC )
		::ReleaseDC( HWND_DESKTOP, hdc );

#else		//	!_WINDOWS

		//		Preserve the current settings
	short			oldFont		= qd.thePort->txFont;
	Style			oldStyle		= qd.thePort->txFace;
	int			oldSize		= qd.thePort->txSize;

	try
		{
		FMetricRec	metrics;
		WidthTable*	pWidthTable;

		//	Setup the font
		::TextFont( RFont::GetMacFontFamilyId( (const LPSZ)m_FontInfo.sbName ) );
		//	Setup the style
		::TextFace( RFont::GetMacStyleBits( m_FontInfo.attributes ) );
		// setup the size
		::TextSize( m_FontInfo.height );
		//	Get the metrics
		::FontMetrics( &metrics );

		m_nFirstCharacter 	= 0;
		m_nCharacters			= 256;
		m_AverageWidth			= RoundFixed( metrics.widMax );	//	REVIEW MDH - What should the average be?
		m_MaximumWidth			= RoundFixed( metrics.widMax );
		m_DefaultHeight		= RoundFixed( metrics.ascent + metrics.descent + metrics.leading );

		m_Dimensions.m_Overhang				= 0;
		m_Dimensions.m_Ascent				= RoundFixed( metrics.ascent );
		m_Dimensions.m_Descent				= RoundFixed( metrics.descent );
		m_Dimensions.m_Leading				= RoundFixed( metrics.leading );
		}
	catch( YException	/* exception */ )
		{
		}

	::TextFont( oldFont );
	::TextFace( oldStyle );
	::TextSize( oldSize );

#endif	//	_WINDOWS
	}

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::~RCharacterWidths( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCharacterWidths::~RCharacterWidths( )
	{
	if (IsInMemory())
		Deallocate( );
	}

#ifdef	_WINDOWS

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::Allocate( )
//
//  Description:		Memory Allocator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCharacterWidths::Allocate( )
	{
	RRealSize		scaleFactor( 1.0, 1.0 );
	BOOLEAN			fScreenDC		= FALSE;
	YFontSize*		pFontWidths		= NULL;
	HDC				hdc				= ::GetDefaultPrinterDC( );

	if (!hdc)
		{
		fScreenDC	= TRUE;
		hdc			= ::GetDC( HWND_DESKTOP );
		}

	try
		{
		//	Convert the point size from 1440 to printer point size -
		//		we only care about the Y scaling for fonts...
		if ( hdc )
			{
			RDcDrawingSurface		ds;
			ds.Initialize( hdc, hdc );
			DeviceUnitsToLogicalUnits( scaleFactor, ds );
			ds.DetachDCs( );
			}

		//	Convert the height to device units by dividing by the scale factor.
		R2dTransform	transform;
		transform.PreScale( 1.0 / scaleFactor.m_dy, 1.0 / scaleFactor.m_dy );
		YFontInfo	fontInfo		= m_FontInfo * transform;
		RGDIFont		font( fontInfo );
		RGDIFont		oldFont( (HFONT)::SelectObject( hdc, font.GetFont() ) );

		pFontWidths		= new YFontSize[ m_nCharacters ];
		if (hdc && ::GetCharWidth( hdc, m_nFirstCharacter, m_nFirstCharacter+m_nCharacters-1, (int*)pFontWidths ))
			{
			m_pWidthTable	= pFontWidths;
			for ( int i = 0; i < m_nCharacters; ++i )
				m_pWidthTable[i]	=	YFontSize( ::Round( (YRealDimension)m_pWidthTable[i] * scaleFactor.m_dy ) );
			pFontWidths		= NULL;
			}
		else
			{
				DWORD	dwError	= ::GetLastError( );
				TpsAssertL( hdc, "GetCharWidthFloat did not work", &dwError, 1 );
			}

		//	What will happen if the HDC is deleted but the new font is still in it??
		::SelectObject( hdc, oldFont.GetFont( ) );

		RDataObject::Allocate( );
		}
	catch( YException	/* exception */)
		{
		;
		}

	// Clean up
	if( hdc && fScreenDC )
		::ReleaseDC( NULL, hdc );

	delete[] pFontWidths;
	}

#elif		MAC

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::Allocate( )
//
//  Description:		Memory Allocator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCharacterWidths::Allocate( )
	{
		//		Preserve the current settings
	short			oldFont		= qd.thePort->txFont;
	Style			oldStyle		= qd.thePort->txFace;
	int			oldSize		= qd.thePort->txSize;

	try
		{
		FMetricRec	metrics;
		WidthTable*	pWidthTable;

		//	Setup the font
		::TextFont( RFont::GetMacFontFamilyId( (const LPSZ)m_FontInfo.sbName ) );
		//	Setup the style
		::TextFace( RFont::GetMacStyleBits( m_FontInfo.attributes ) );
		// setup the size
		::TextSize( m_FontInfo.height );
		//	Get the metrics
		::FontMetrics( &metrics );

		m_pWidthTable			= new YFontSize[ m_nCharacters ];
		pWidthTable				= (WidthTable*)*(metrics.wTabHandle);
		YRealDimension	scale = (YRealDimension)pWidthTable->fSize / (YRealDimension)pWidthTable->aSize;
		for (int i = m_nFirstCharacter; i < m_nCharacters; ++i )
			{
			YRealDimension	width = ( (long)pWidthTable->tabData[i] ) / 65536.0;
			m_pWidthTable[i] = ::Round( width * scale );
			}

		RDataObject::Allocate( );
		}
	catch( YException	/* exception */ )
		{
		}

	::TextFont( oldFont );
	::TextFace( oldStyle );
	::TextSize( oldSize );
	}

#endif	//	_WINDOWS or MAC

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::Deallocate( )
//
//  Description:		Memory Deallocator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCharacterWidths::Deallocate( )
	{
	delete[] m_pWidthTable;
	m_pWidthTable = NULL;
	RDataObject::Deallocate( );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::Validate( )
//
//  Description:		Class object validation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCharacterWidths::Validate( ) const
	{
	RDataObject::Validate( );
	}

#endif	//	TPSDEBUG


// ****************************************************************************
//
//  Function Name:	RKerningInfo::RKerningInfo( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RKerningInfo::RKerningInfo( const YFontInfo& info )
	: m_nEntries( 0 ),
	  m_FontInfo( info ),
	  m_fTableValid( FALSE ),
	  m_KernTable( ),
	  m_TableIndex( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RKerningInfo::~RKerningInfo( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RKerningInfo::~RKerningInfo( )
	{
	if (IsInMemory())
		Deallocate( );
	}

#ifdef	_WINDOWS

// ****************************************************************************
//
//  Function Name:	RKerningInfo::Allocate( )
//
//  Description:		Memory Allocator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RKerningInfo::Allocate( )
	{
	int				nPairs			= 0;
	BOOLEAN			fScreenDC		= FALSE;
	YKernPair*		pKernPairs		= NULL;
	RRealSize		scaleFactor( 1.0, 1.0 );
	HDC				hdc				= ::GetDefaultPrinterDC( );

	if (!hdc)
		{
		fScreenDC	= TRUE;
		hdc			= ::GetDC( HWND_DESKTOP );
		}

	try
		{
		//	Convert the point size from 1440 to printer point size -
		//		we only care about the Y scaling for fonts...
		if ( hdc )
			{
			RDcDrawingSurface		ds;
			ds.Initialize( hdc, hdc );
			DeviceUnitsToLogicalUnits( scaleFactor, ds );
			ds.DetachDCs( );
			}

		//	Convert the height to device units by dividing by the scale factor.
		R2dTransform	transform;
		transform.PreScale( 1.0 / scaleFactor.m_dy, 1.0 / scaleFactor.m_dy );
		YFontInfo	fontInfo		= m_FontInfo * transform;
		RGDIFont			font( fontInfo );
		RGDIFont			oldFont( (HFONT)::SelectObject( hdc, font.GetFont() ) );

		m_nEntries		= GetKerningPairs( hdc, nPairs, NULL );

		pKernPairs		= new YKernPair[ m_nEntries ];

		if (GetKerningPairs( hdc, m_nEntries, pKernPairs ) )
			{
			for ( int i = 0; i < m_nEntries; ++i )
				pKernPairs[i].iKernAmount = ::Round( YRealDimension(pKernPairs[i].iKernAmount) * scaleFactor.m_dx );
			RKernArray	tmpArray( pKernPairs, (uLONG)m_nEntries, (uLONG)m_nEntries );
			m_KernTable = tmpArray;

			Sort( );

			pKernPairs = NULL;
			}

		//	What will happen if the HDC is deleted but the new font is still in it??
		::SelectObject( hdc, oldFont.GetFont( ) );
		RDataObject::Allocate( );
		}
	catch( YException	exception )
		{
		if (exception == kMemory)
			{
			;
			}
		else if (exception == kResource)
			{
			;
			}
		}

	// Clean up
	if( hdc && fScreenDC )
		::ReleaseDC( NULL, hdc );

	delete pKernPairs;
	}

#elif		MAC

// ****************************************************************************
//
//  Function Name:	RKerningInfo::Allocate( )
//
//  Description:		Memory Allocator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RKerningInfo::Allocate( )
	{
	///xxx	Not implemented yet......
	;
	}

#endif	//	_WINDOWS or MAC

// ****************************************************************************
//
//  Function Name:	RKerningInfo::Deallocate( )
//
//  Description:		Memory Deallocator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RKerningInfo::Deallocate( )
	{
	m_KernTable.Empty( );
	m_TableIndex.Empty( );

	RDataObject::Deallocate( );
	}


// ****************************************************************************
//		
//  Function Name:	KernCompareProc( )
//
//  Description:		Compare two Kern entries for equality
//
//  Returns:			standard ansi compare values
//
//  Exceptions:		None
//		
// ****************************************************************************
//
	inline int KernCompareProc( const YKernPair& kern1, const YKernPair& kern2 )
		{
		int	nCmp;
#ifdef	_WINDOWS
		nCmp = kern1.wFirst - kern2.wFirst;
		if (!nCmp)	nCmp = kern1.wSecond - kern2.wSecond;
#elif		defined( MAC )
		nCmp = kern1.kernFirst - kern2.kernFirst;
		if (!nCmp)	nCmp = kern1.kernSecond - kern2.kernSecond;
#endif	//	_WINDOWS
		return nCmp;
		}

// ****************************************************************************
//
//  Function Name:	RKerningInfo::GetKerningPair( )
//
//  Description:		Return the size of the given kern values
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
YKernSize RKerningInfo::GetKerningPair( const RCharacter& char1, const RCharacter& char2 ) const
	{
	int						nCompValue;
	YKernPair				kernPair;
	RKernArrayIterator	iterator		= m_KernTable.Start();
	RKernArrayIterator	iteratorEnd	= m_KernTable.End();

#ifdef	_WINDOWS
	kernPair.wFirst	= (WORD)(int)(char1);
	kernPair.wSecond	= (WORD)(int)(char2);
#else		//	_WINDOWS
	kernPair.kernFirst	= (uBYTE)(int)(char1);
	kernPair.kernSecond	= (uBYTE)(int)(char2);
#endif	//	_WINDOWS

	for (iterator = m_KernTable.Start(), iteratorEnd = m_KernTable.End();
				iterator != iteratorEnd; ++iterator )
		{
		nCompValue = KernCompareProc( kernPair, *iterator );
		if (nCompValue == 0)
			return GetKernValue( *iterator );
		else if (nCompValue < 0)
			break;
		}

	return 0;
	}

// ****************************************************************************
//
//  Function Name:	RKerningInfo::Sort( )
//
//  Description:		Sort the kerning pair information for quick retrieval
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RKerningInfo::Sort( )
	{
	::Sort( m_KernTable.Start(), m_KernTable.End(), KernCompareProc );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RKerningInfo::Validate( )
//
//  Description:		Class object validation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RKerningInfo::Validate( ) const
	{
	RDataObject::Validate( );
	}

#endif	//	TPSDEBUG


// ****************************************************************************
//
//  Function Name:	RCharacterPaths::RCharacterPaths( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCharacterPaths::RCharacterPaths( const YFontInfo& info )
	: m_nFirstCharacter( 0 ),
	  m_nCharacters( 0 ),
	  m_FontInfo( info ),
	  m_ppPaths( NULL )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RCharacterPaths::~RCharacterPaths( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCharacterPaths::~RCharacterPaths( )
	{
	if (IsInMemory())
		Deallocate( );
	}

#ifdef	_WINDOWS

// ****************************************************************************
//
//  Function Name:	RCharacterPaths::Allocate( )
//
//  Description:		Memory Allocator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCharacterPaths::Allocate( )
	{
	TEXTMETRIC		tm;
	HDC				hdc			= NULL;	///xxx	Eventually, this MUST be a printer DC
	YFontSize*		pFontWidths = NULL;

	try
		{
		int	i;
		hdc			= GetDC( NULL );

		RGDIFont			font( m_FontInfo );
		RGDIFont			oldFont( (HFONT)::SelectObject( hdc, font.GetFont() ) );

		if ( GetTextMetrics( hdc, &tm ) )
			{
			m_nFirstCharacter	= tm.tmFirstChar;
			m_nCharacters		= tm.tmLastChar - tm.tmFirstChar + 1;
			}
		else
			{
			//	If we can't get the text metrics, try to be reasonable
			m_nFirstCharacter	= 0;
			m_nCharacters		= 256;		//	Default for Roman font
			}

		m_ppPaths		= new RPath*[ m_nCharacters ];
		for (i = 0; i < m_nCharacters; ++i)
			m_ppPaths[i] = NULL;

		//	What will happen if the HDC is deleted but the new font is still in it??
		::SelectObject( hdc, oldFont.GetFont( ) );

		RDataObject::Allocate( );
		}
	catch( YException	exception )
		{
		if (exception == kMemory)
			{
			;
			}
		else if (exception == kResource)
			{
			;
			}
		}

	ReleaseDC( NULL, hdc );
	}

#elif		MAC

// ****************************************************************************
//
//  Function Name:	RCharacterPaths::Allocate( )
//
//  Description:		Memory Allocator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCharacterPaths::Allocate( )
	{
	try
		{
		int	i;
		m_nFirstCharacter 	= 0;
		m_nCharacters			= 256;

		m_ppPaths	= new RPath*[m_nCharacters];
		for (i = 0; i < m_nCharacters; ++i)
			m_ppPaths[i] = NULL;

		RDataObject::Allocate( );
		}
	catch( YException	/* exception */ )
		{
		}
	}

#endif	//	_WINDOWS or MAC

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::Deallocate( )
//
//  Description:		Memory Deallocator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCharacterPaths::Deallocate( )
	{
	int		i;
	if (m_ppPaths)
		{
		for ( i = 0; i < m_nCharacters; ++i )
			delete m_ppPaths[i];
		delete [] m_ppPaths;
		}
	RDataObject::Deallocate();
	}

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::GetCharacterPath( )
//
//  Description:		Return the path at the given character
//
//  Returns:			m_ppPaths[ (int)character - m_nFirstCharacter ];
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPath*	RCharacterPaths::GetCharacterPath( const RCharacter& character ) const
	{
	int		index = (int)character - m_nFirstCharacter;
	TpsAssert( index >= 0, "index is not in range for this character set" );
	TpsAssert( index < m_nCharacters, "index is not in range for this character set" );
	TpsAssert( m_ppPaths != NULL, "m_ppPaths is NULL in GetCharacterPath. Did you call Validate?" );

	return m_ppPaths[index];
	}

// ****************************************************************************
//
//  Function Name:	RCharacterWidths::GetCharacterPath( )
//
//  Description:		Return the path at the given character
//
//  Returns:			m_ppPaths[ (int)character - m_nFirstCharacter ];
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RCharacterPaths::SetCharacterPath( const RCharacter& character, RPath* pPath )
	{
	int		index = (int)character - m_nFirstCharacter;
	TpsAssert( index >= 0, "index is not in range for this character set" );
	TpsAssert( index < m_nCharacters, "index is not in range for this character set" );
	TpsAssert( m_ppPaths != NULL, "m_ppPaths is NULL in GetCharacterPath. Did you call Validate?" );
	TpsAssert( pPath != NULL, "Path passed into SetCharacterPath was NULL" );
	m_ppPaths[index] = pPath;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RCharacterPaths::Validate( )
//
//  Description:		Class object validation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCharacterPaths::Validate( ) const
	{
	RDataObject::Validate( );
	}

#endif	//	TPSDEBUG


