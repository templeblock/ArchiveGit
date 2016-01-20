// ****************************************************************************
//
//  File Name:			FontLists.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RFontList and RFontSizeInfo class
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
//  $Logfile:: /PM8/Framework/Source/FontLists.cpp                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifdef	_WINDOWS
#include	"GDIObjects.h"
#include "DcDrawingSurface.h"
#endif

#include "ChunkyStorage.h"

#ifndef	IMAGER
#include "ApplicationGlobals.h"
#include "ResourceManager.h"
#else

extern  RFontList            		gFontList;
extern  BOOLEAN				  		gfTrueTypeAvailable;
extern  BOOLEAN				  		gfATMAvailable;
#define GetAppFontList()			gFontList
#define IsTrueTypeAvailable()		gfTrueTypeAvailable
#define IsATMAvailable()  	  		gfATMAvailable

#endif	//	IMAGER

const YChunkTag	kFontListTag			= 'FONT';
const YCounter		kInvalidFontUseCount	= (YCounter)-1;
const YCounter		kDummyFontUseCount	= (YCounter)(0x7FFFFFFF);


RFontSubstitutionCollection RFontList::m_gSubstitutionCollection;

// ****************************************************************************
//
//				RFontList
//
// ****************************************************************************
//

// ****************************************************************************
//
//  Function Name:	RFontList::RFontList( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RFontList::RFontList( ) : RArray<YFontDescriptor>( kFontListGrowSize )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RFontList::RFontList( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RFontList::RFontList( const RFontList& rhs ) : RArray<YFontDescriptor>( kFontListGrowSize )
	{
	*this = rhs;
	}

// ****************************************************************************
//
//  Function Name:	RFontList::~RFontList( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RFontList::~RFontList( )
	{
	YFontId					fontId;
	YIterator				fontListIterator	= Start();

	for ( fontId = 0; fontListIterator != End( ); ++fontId, ++fontListIterator )
		{
		YFontDescriptor&	fontDesc	= *fontListIterator;		
		//	If no uses in this descriptor, just continue - Stonehand does not always
		//		remeber all the used fonts. so the below assert is hit in valid circumstances..
		//TpsAssert( fontDesc.uses == 0, "There are fonts with uses still in the font list" );
		TpsAssert( fontDesc.uses >= 0, "Font in font list with negative uses..." );
		if ( fontDesc.uses != kDummyFontUseCount )
			{
			while ( fontDesc.uses > 0 )
				RemoveFont( fontId );
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RFontList::AddFont( )
//
//  Description:		
//
//  Returns:			The ID of the font added
//
//  Exceptions:		Memory Exception
//
// ****************************************************************************
//
YFontId RFontList::AddFont( const YFont& font )
	{
	//	First try to find the font in this list...
	RFont*					pRFont;
	YFontId					fontId;
	YFontId					unusedFontId		= Count();
	YIterator				fontListIterator	= Start();
	YIterator				fontListEnd			= End();

	for ( fontId = 0; fontListIterator != fontListEnd; ++fontListIterator, ++fontId)
		{
		YFontDescriptor&	fontDesc	= *fontListIterator;		
		//	If no uses in this descriptor, just continue
		if (fontDesc.uses != 0)
			{
			//	special flag to say not to use this value for now.
			//		we are reading the file
			if ( fontDesc.uses == kInvalidFontUseCount )
				continue;
			RFont*	pFont = fontDesc.pRFont;
			TpsAssert( pFont != NULL, "Font descriptor with uses has a NULL RFont pointer" );
			//	If color and info are the same, just increment uses
			if ( (font.info == GetFontInfo(fontId)) && (fontDesc.color == font.color) )
				{
				TpsAssert( (fontDesc.pRFont)->GetUses() > 0, "Font with no uses is in the list" );
				++(fontDesc).uses;
				return fontId;
				}
			}
		else
			unusedFontId = fontId;
		}

#ifndef	IMAGER
	//	Font not in the document list. Check if the font is in
	//	the global app list.
	RFontList&	appFontList			= GetAppFontList();
	YIterator	appListIterator	= appFontList.Start();
	YIterator	appListEnd			= appFontList.End();

	for ( ; appListIterator != appListEnd; ++appListIterator )
		{
		TpsAssert( (*appListIterator).pRFont->GetUses() > 0, "Font in AppList has no uses." );
		if ( (*appListIterator).pRFont->GetInfo() == font.info )
			break;
		}

	//
	//	If appListIterator is at end, add new item to head of list
	//	and set iterator to point to it.
	if (appListIterator == appListEnd)
		{
		YFontDescriptor	appFontDescriptor;
		try
			{
			appFontDescriptor.pRFont	= NULL;
			appFontDescriptor.pRFont	= RFont::CreateFont( font.info, FALSE );
			appFontDescriptor.color		= RSolidColor( kBlack );
			appFontDescriptor.uses		= 0;
			appFontList.InsertAtStart( appFontDescriptor );
			appListIterator = appFontList.Start();
			}
		catch( ... )
			{
			delete appFontDescriptor.pRFont;
			throw;
			}
		}

	pRFont = (*appListIterator).pRFont;
#else
	pRFont = RFont::CreateFont( font.info, FALSE );
#endif	//	IMAGER
	//
	//	add one more use to the RFont
	pRFont->AddUse();

	//	If the last unusedFontId was Count() (i.e. all font slots
	//		are being used), we need to insert a new one.
	if ( unusedFontId == (unsigned)Count() )
		{
		YFontDescriptor	fontDesc;
		InsertAtEnd( fontDesc );
		fontListIterator = End();
		--fontListIterator;
		}
	else
		fontListIterator	= Start() + unusedFontId;

	//
	//	Update the information for this font descriptor
	(*fontListIterator).pRFont = pRFont;
	(*fontListIterator).color	= font.color;
	(*fontListIterator).uses	= 1;

	return unusedFontId;
	}

// ****************************************************************************
//
//  Function Name:	RFontList::FindFont( )
//
//  Description:		Find the YFontId of the given YFont
//
//  Returns:			The ID of the font - or kInvalidFontID
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YFontId RFontList::FindFont( const YFont& font ) const
	{
	YFontId		fontId;
	YIterator	fontListIterator	= Start();
	YIterator	fontListEnd			= End();

	for ( fontId = 0; fontListIterator != fontListEnd; ++fontListIterator, ++fontId)
		{
		YFontDescriptor&	fontDesc	= *fontListIterator;		
		//	If no uses in this descriptor, just continue
		if (fontDesc.uses != 0)
			{
			RFont*	pFont = fontDesc.pRFont;
			TpsAssert( pFont != NULL, "Font descriptor with uses has a NULL RFont pointer" );
			//	If color and info are the same, just increment uses
			if ( (font.info == GetFontInfo(fontId)) && (fontDesc.color == font.color) )
				return fontId;
			}
		}

	return kInvalidFontId;
	}

// ****************************************************************************
//
//  Function Name:	RFontList::FindRFont( )
//
//  Description:		Find the YFontId of a font with the given RFont
//
//  Returns:			The ID of the font - or kInvalidFontID
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YFontId RFontList::FindRFont( RFont* pfont ) const
	{
	YFontId		fontId;
	YIterator	fontListIterator	= Start();
	YIterator	fontListEnd			= End();

	for ( fontId = 0; fontListIterator != fontListEnd; ++fontListIterator, ++fontId)
		{
		YFontDescriptor&	fontDesc	= *fontListIterator;		
		//	If no uses in this descriptor, just continue
		if (fontDesc.uses != 0)
			{
			TpsAssert( fontDesc.pRFont != NULL, "Font descriptor with uses has a NULL RFont pointer" );
			if ( fontDesc.pRFont == pfont )
				return fontId;
			}
		}

	return kInvalidFontId;
	}


// ****************************************************************************
//
//  Function Name:	RFontList::FindFontFamily( )
//
//  Description:		Find the YFontId of a font with the given family name
//
//  Returns:			The ID of the font - or kInvalidFontID
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YFontId RFontList::FindFontFamily( sBYTE* psbFamilyName ) const
	{
	YFontId		fontId;
	YIterator	fontListIterator	= Start();
	YIterator	fontListEnd			= End();

	//	If there is a valid family name... Find the font id...
	if ( psbFamilyName )
		{
		for ( fontId = 0; fontListIterator != fontListEnd; ++fontListIterator, ++fontId)
			{
			YFontDescriptor&	fontDesc	= *fontListIterator;		
			//	If no uses in this descriptor, just continue
			if (fontDesc.uses != 0)
				{
				RFont*				pFont = fontDesc.pRFont;
				const YFontInfo&	info	= GetFontInfo(fontId);
				TpsAssert( pFont != NULL, "Font descriptor with uses has a NULL RFont pointer" );
				if ( strcmp(psbFamilyName, info.sbName) == 0 )
					return fontId;
				}
			}
		}

	return kInvalidFontId;
	}


// ****************************************************************************
//
//  Function Name:	RFontList::RemoveFont( )
//
//  Description:		Remove the font from the font list if the usage count is 0
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void RFontList::RemoveFont( YFontId id )
	{
	TpsAssert( id != kInvalidFontId, "GetFontInfo called with InvalidFontId" );
	TpsAssert( (signed)id <= Count(), "Requested ID is larger than number of fonts" );

	YFontDescriptor&	fontDesc	= Start()[id];

	if ( fontDesc.uses == kDummyFontUseCount )
		return;

	TpsAssert( fontDesc.uses > 0, "Requested id has 0 or less uses" );
	TpsAssert( fontDesc.pRFont != NULL, "Requested id has NULL RFont" );
	//	If this was the last reference to this font, so find the pRFont in the
	//	global AppFontList and decrement it usage counter.
	if (--fontDesc.uses == 0)
		{
		RemoveAppUse( fontDesc.pRFont );
		//	Cleanup Processing
		fontDesc.pRFont = NULL;
		}
	}

// ****************************************************************************
//
//  Function Name:	RFontList::RemoveAppUse( )
//
//  Description:		Remove use from the AppFontList
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void RFontList::RemoveAppUse( RFont*	pRFont )
	{
	TpsAssert( pRFont != NULL, "The given font descriptor identifies a NULL app font" );
	TpsAssert( pRFont->GetUses() > 0, "There are 0 uses on the given AppFont" );

#ifndef	IMAGER
	if (pRFont->RemoveUse())
		{
		RFontList&	appFontList			= GetAppFontList();
		YIterator	appListIterator	= appFontList.Start();
		YIterator	appListEnd			= appFontList.End();

		for ( ; appListIterator != appListEnd; ++appListIterator )
			{
			if ( (*appListIterator).pRFont == pRFont)
				{
				//	Delete the font and remove the iterator....
				delete pRFont;
				(*appListIterator).pRFont	= NULL;
				appFontList.RemoveAt( appListIterator );
				break;
				}
			}
		}
#endif	//	IMAGER
	}

// ****************************************************************************
//
//  Function Name:	RFontList::GetFont( )
//
//  Description:		Return a YFont structure reference for the given id
//
//  Returns:			The requested YFont structure
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void RFontList::GetFont( YFontId id, YFont& font) const
	{
	font.info	= GetRFont(id)->GetInfo();
	font.color	= GetFontColor( id );
	}


// ****************************************************************************
//
//  Function Name:	RFontList::GetFontInfo( )
//
//  Description:		Return a YFontInfo structure reference for the given id
//
//  Returns:			The requested YFont structure
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
const YFontInfo& RFontList::GetFontInfo( YFontId id ) const
	{
	TpsAssert( id != kInvalidFontId, "GetFontInfo called with InvalidFontId" );
	return GetRFont(id)->GetInfo();
	}

// ****************************************************************************
//
//  Function Name:	RFontList::GetFontInfo( )
//
//  Description:		Return a YFontInfo structure reference for the given id
//
//  Returns:			The requested YFont structure
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YFontInfo& RFontList::GetFontInfo( YFontId id )
	{
	TpsAssert( id != kInvalidFontId, "GetFontInfo called with InvalidFontId" );
	return GetRFont(id)->GetInfo();
	}

// ****************************************************************************
//
//  Function Name:	RFontList::GetFontColor( )
//
//  Description:		Return the color of the font with the given id
//
//  Returns:			The color of the requested font
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RSolidColor RFontList::GetFontColor( YFontId id ) const
	{
	TpsAssert( id != kInvalidFontId, "GetFontInfo called with InvalidFontId" );
	return GetFontDescriptor(id).color;
	}

// ****************************************************************************
//
//  Function Name:	RFontList::GetFontDescriptor( )
//
//  Description:		Return a YFontDescriptor reference for the given id
//
//  Returns:			The requested YFontDescriptor structure
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
YFontDescriptor& RFontList::GetFontDescriptor( YFontId id )
	{
	TpsAssert( id != kInvalidFontId, "GetFontInfo called with InvalidFontId" );
	TpsAssert( (signed)id <= Count(), "Requested ID is larger than number of fonts" );
	TpsAssert( (Start()[id]).uses > 0, "Requested id has 0 or less uses" );
	TpsAssert( (Start()[id]).pRFont != NULL, "Requested id has NULL RFont" );
	return Start()[id];
	}

// ****************************************************************************
//
//  Function Name:	RFontList::GetFontDescriptor( )
//
//  Description:		Return a YFontDescriptor reference for the given id
//
//  Returns:			The requested YFontDescriptor structure
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
const YFontDescriptor& RFontList::GetFontDescriptor( YFontId id ) const
	{
	TpsAssert( id != kInvalidFontId, "GetFontInfo called with InvalidFontId" );
	TpsAssert( (signed)id <= Count(), "Requested ID is larger than number of fonts" );
	TpsAssert( (Start()[id]).uses > 0, "Requested id has 0 or less uses" );
	TpsAssert( (Start()[id]).pRFont != NULL, "Requested id has NULL RFont" );
	return Start()[id];
	}

// ****************************************************************************
//
//  Function Name:	RFontList::GetRFont( )
//
//  Description:		Return the RFont object identified by the Given ID
//
//  Returns:			The requested RFont object
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RFont* RFontList::GetRFont( YFontId id ) const
	{
	TpsAssert( id != kInvalidFontId, "GetFontInfo called with InvalidFontId" );
	return GetFontDescriptor(id).pRFont;
	}

// ****************************************************************************
//
//  Function Name:	ReadFontList::ClearFontList( )
//
//  Description:		Remove all items from the font list
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void RFontList::ClearFontList(  )
	{
	YIterator	iterator	= Start();

	for ( ; iterator != End( ); ++iterator )
		{
		RemoveAppUse( (*iterator).pRFont );
		}

	Empty( );		//	Free all items in the list
	}

// ****************************************************************************
//
//  Function Name:	ReadFontList::WriteFontList( )
//
//  Description:		Writes this documents Font List
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RFontList::WriteFontList( RChunkStorage& storage ) const
	{
	YFileFont	fileFont;
	uLONG			ulFontCount	= Count( );

	//	Add a new chunk of data
	storage.AddChunk( kFontListTag );

	Write( storage );
	//	Reset to the parent chunk of data...
	storage.SelectParentChunk( );
	}

// ****************************************************************************
//
//  Function Name:	ReadFontList::Write( )
//
//  Description:		Writes this documents Font List
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RFontList::Write( RArchive& archive ) const
	{
	YFileFont	fileFont;
	uLONG			ulFontCount	= Count( );

	archive << ulFontCount;

	YIterator	iterator	= Start( );
	for ( ulFontCount = 0; iterator != End( ); ++ulFontCount, ++iterator )
		{
		RFont*	pFont		= (*iterator).pRFont;
		if ( pFont )
			fileFont.info	= pFont->GetInfo();
		else
			{
			fileFont.info.attributes	= 0;
			fileFont.info.height			= 0;
			fileFont.info.width			= 0;
			fileFont.info.angle			= 0;
			fileFont.info.ulPadding		= 0;
			fileFont.info.sbName[0]		= 0;	//	NULL Character
			}
		fileFont.ulUses	= (*iterator).uses;
		fileFont.color		= (*iterator).color;
		archive << fileFont;
		}

	TpsAssert( ulFontCount == (uLONG)Count( ), "Number of fonts written not equal to Count( ) " );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::ReadFontList( )
//
//  Description:		Reads the Font List from the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RFontList::ReadFontList( RChunkStorage& storage )
	{
	// Iterate through the components
	RChunkStorage::RChunkSearcher searcher = storage.Start( kFontListTag, FALSE );

	TpsAssert( !searcher.End(), "No Font list found in this chunky section" );
	if ( !searcher.End() )
		{
		Read( storage );
		// Select the parent
		storage.SelectParentChunk( );
		}
	}

// ****************************************************************************
//
//  Function Name:	RDocument::Read( )
//
//  Description:		Reads the Font List from the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RFontList::Read( RArchive& archive )
	{
	YFont		font;
	YFontId		fontId;
	YFileFont	fileFont;
	YIterator	iterator;
	uLONG		ulFontCount;

#ifdef	_WINDOWS
	HDC	hdc	= RFont::GetSystemDC( );
#endif	//	_WINDOWS

	//	Clear all of the fonts currently being used
	ClearFontList( );
	//	Get the number of fonts in the chunk
	archive >> ulFontCount;
	//	Loop through and read them in...
	for ( fontId = 0; fontId < ulFontCount; ++fontId )
		{
		//	Read fileFont
		archive >> fileFont;
		//	Create and add the font to the list
		font.info	= fileFont.info;

		font.color	= fileFont.color;
		if (fileFont.ulUses > 0)
			{
			// see if we need to convert font size from printshop 5 and 6 versions
			// build #300 is the first build after PrintShop 6.
			// If the value is 0 it's not a printshop document (could be leapfrog)
			RVersionInfo info = (RApplication::GetApplicationGlobals())->GetCurrentDocumentVersion();
			if( info.m_InternalVersion > 0 && info.m_InternalVersion < 300 )
				ConvertFontSize( &font );

			YFontId	addedFontId;
			addedFontId		= AddFont( font );
			if ( addedFontId != fontId )
				{
				YFontDescriptor	fontDesc;
				fontDesc.pRFont	= GetRFont( addedFontId );
				fontDesc.color		= fileFont.color;
				fontDesc.uses		= kDummyFontUseCount;	//	Not Really Used
				InsertAtEnd( fontDesc );
				}
			else
				{
				iterator						= Start( );
				iterator[fontId].uses	= fileFont.ulUses;

				//	Validate the font is actually the font requested
#ifdef _WINDOWS
				if ( (strlen(font.info.sbName)!=0) && !RFont::IsFontOnSystem(font.info.sbName) )
					{
					RFontSubstitution	fontSub( font.info.sbName, "" );
					GetFontSubstitutionList().InsertAtEnd( fontSub );
					}
#endif	//	_WINDOWS
#ifdef MAC
#endif	//	MAC
				}

			}
		else
			{
			YFontDescriptor	fontDesc;
			fontDesc.pRFont	= NULL;
			fontDesc.color		= fileFont.color;
			fontDesc.uses		= kInvalidFontUseCount;
			InsertAtEnd( fontDesc );
			}
		}

	//	Clear all of the uses flags for items that don't have pFonts...
	iterator	= Start( );
	while ( iterator != End( ) )
		{
		if ( (*iterator).pRFont == NULL )
			(*iterator).uses	= 0;	//	It really is not in use
		++iterator;
		}

#ifdef	_WINDOWS
	RFont::ReleaseSystemDC( );
#endif	//	_WINDOWS
	}

// ****************************************************************************
//
//  Function Name:	RDocument::ReadFontList( )
//
//  Description:		Reads the Font List from the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
RFontList& RFontList::operator=( const RFontList& rhs )
{
	//	Remove all entries from current list
	ClearFontList( );

	//	Let the array copy all of its members
	RArray<YFontDescriptor>::operator=( rhs );

	//
	//	For each element in the array, add an instance into the AppFontList if
	//	the entry is used
	YCounter		fontsInList	= Count( );
	YIterator	iterator		= Start( );
	for ( ; iterator != End( ); ++iterator )
	{
		YFontDescriptor	fontDescriptor	= *iterator;
		if ( fontDescriptor.pRFont != NULL )
		{
			//	Add a use to the global font list usage
			fontDescriptor.pRFont->AddUse( );
			TpsAssert( fontDescriptor.uses > 0, "fontDescriptor.pFont is not NULL, but the uses is == 0" );
		}
		else
		{
			TpsAssert( fontDescriptor.uses == 0, "fontDescriptor.pFont is NULL, but there is no uses" );
		}
	}

	return *this;
}

// ****************************************************************************
//
//  Function Name:	RDocument::ClearFontSubstitutionList( )
//
//  Description:		Clear the font substituion list
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void	RFontList::ClearFontSubstitutionList( )
{
	m_gSubstitutionCollection.Empty();
}


// ****************************************************************************
//
//  Function Name:	RFontList::ConvertFontSize( )
//
//  Description:	Adjusts the height member of the font info to reflect 		
//					pointsize instead of font height. This function was put 
//					in place when the changes was made in GDIObject.cpp to create
//					fonts at a true pointsize, this function is used to convert
//					old documents.
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RFontList::ConvertFontSize( YFont *pfont )
{

	TEXTMETRIC		tm;
	BOOLEAN			fScreenDC	= FALSE;
	HDC				hdc			= GetDefaultPrinterDC( );

	if (!hdc)
	{
		fScreenDC	= TRUE;
		hdc			= ::GetDC( HWND_DESKTOP );
	}

	RGDIFont testfont( pfont->info );
	// select the font into the DC and get it's metrics
	RGDIFont oFont( (HFONT)::SelectObject( hdc, testfont.GetFont() ) );
	GetTextMetrics( hdc, &tm );
	
	// adjust the font height to reflect the pointsize ( internal lead + pointsize = height )
	pfont->info.height -= tm.tmInternalLeading;

	// Clean up
	SelectObject( hdc, oFont.GetFont() );
	if( hdc && fScreenDC )
		::ReleaseDC( HWND_DESKTOP, hdc );
}



// ****************************************************************************
//
//  Function Name:	RDocument::ClearFontSubstitutionList( )
//
//  Description:		Return the font substitution list
//
//  Returns:			m_gSubstitutionCollection
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFontSubstitutionCollection& RFontList::GetFontSubstitutionList( )
{
	return m_gSubstitutionCollection;
}



const	YIntDimension	kFontSizeMinSize		= 6;
const	YIntDimension	kFontSizeMaxSize		= 72;
const YIntDimension	kFontSizeDefaultSize	= 12;
// ****************************************************************************
//
//  Function Name:	RFontSizeInfo::RFontSizeInfo( ... )
//
//  Description:		ctor 
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFontSizeInfo::RFontSizeInfo( )
		: m_MinSize( kFontSizeMinSize ),
		  m_MaxSize( kFontSizeMaxSize ), 
		  m_DefaultSize( kFontSizeDefaultSize )
	{
		m_FontSizes.InsertAtEnd( kFontSizeMinSize );
		m_FontSizes.InsertAtEnd( kFontSizeDefaultSize );
		m_FontSizes.InsertAtEnd( kFontSizeMaxSize );
	}

// ****************************************************************************
//
//  Function Name:	RFontSizeInfo::RFontSizeInfo( ... )
//
//  Description:		ctor 
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFontSizeInfo::RFontSizeInfo( const RFontSizeInfo& rhs )
	{
	*this = rhs;
	}

// ****************************************************************************
//
//  Function Name:	RFontSizeInfo::RFontSizeInfo( ... )
//
//  Description:		ctor 
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFontSizeInfo::RFontSizeInfo( YIntDimension yMin,		YIntDimension ySize1,
										YIntDimension ySize2,	YIntDimension ySize3,
										YIntDimension ySize4,	YIntDimension ySize5,
										YIntDimension ySize6,	YIntDimension ySize7,
										YIntDimension ySize8,	YIntDimension ySize9,
										YIntDimension ySize10,	YIntDimension ySize11,
										YIntDimension ySize12,	YIntDimension ySize13,
										YIntDimension ySize14,
										YIntDimension yMax,		YIntDimension yDefault )
		: m_MinSize( yMin ), m_MaxSize( yMax ), m_DefaultSize( yDefault )
	{
		m_FontSizes.InsertAtEnd( ySize1 );
		m_FontSizes.InsertAtEnd( ySize2 );
		m_FontSizes.InsertAtEnd( ySize3 );
		m_FontSizes.InsertAtEnd( ySize4 );
		m_FontSizes.InsertAtEnd( ySize5 );
		m_FontSizes.InsertAtEnd( ySize6 );
		m_FontSizes.InsertAtEnd( ySize7 );
		m_FontSizes.InsertAtEnd( ySize8 );
		m_FontSizes.InsertAtEnd( ySize9 );
		m_FontSizes.InsertAtEnd( ySize10 );
		m_FontSizes.InsertAtEnd( ySize11 );
		m_FontSizes.InsertAtEnd( ySize12 );
		m_FontSizes.InsertAtEnd( ySize13 );
		m_FontSizes.InsertAtEnd( ySize14 );
	}

// ****************************************************************************
//
//  Function Name:	RFontSizeInfo::operator=( ... )
//
//  Description:		assignment operator
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFontSizeInfo& RFontSizeInfo::operator=( const RFontSizeInfo& rhs )
	{
	if ( this != &rhs )
		{
		m_MinSize		= rhs.m_MinSize;
		m_MaxSize		= rhs.m_MaxSize;
		m_DefaultSize	= rhs.m_DefaultSize;
		m_FontSizes		= rhs.m_FontSizes;
		}
	return *this;
	}


// ****************************************************************************
//
//  Function Name:	RFontSizeInfo::GetMinFontSize( )
//
//  Description:		return min font size 
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YIntDimension RFontSizeInfo::GetMinFontSize( ) const
	{
	return m_MinSize;
	}


// ****************************************************************************
//
//  Function Name:	RFontSizeInfo::GetMaxFontSize( )
//
//  Description:		return max font size 
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YIntDimension RFontSizeInfo::GetMaxFontSize( ) const
	{
	return m_MaxSize;
	}


// ****************************************************************************
//
//  Function Name:	RFontSizeInfo::GetDefaultFontSize( )
//
//  Description:		return default font size 
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YIntDimension RFontSizeInfo::GetDefaultFontSize( ) const
	{
	return m_DefaultSize;
	}


// ****************************************************************************
//
//  Function Name:	RFontSizeInfo::GetFontSizes( )
//
//  Description:		return font size array
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCFontSizeArray& RFontSizeInfo::GetFontSizes( ) const
	{
	return m_FontSizes;
	}

// ****************************************************************************
//
// Function Name:		RFontSizeInfo::Read( RArchive& archive )
//
// Description:		Read our project font size info
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RFontSizeInfo::Read( RArchive& archive )
	{
	//	read in the recommended font sizes
	archive >> m_MinSize;
	archive >> m_MaxSize;
	archive >> m_DefaultSize;

	uLONG	ulCount;
	archive >> ulCount;	//	count of sizes written;

	m_FontSizes.Empty( );
	for(uLONG i = 0 ; i < ulCount; ++i )
		{
		uLONG	size;
		archive >> size;
		m_FontSizes.InsertAtEnd( static_cast<YIntDimension>( size ) );
		}
	}

// ****************************************************************************
//
// Function Name:		RFontSizeInfo::Write( RArchive& archive )
//
// Description:		Write our project font size info
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RFontSizeInfo::Write( RArchive& archive ) const
	{
	archive << m_MinSize;
	archive << m_MaxSize;
	archive << m_DefaultSize;
	archive << static_cast<uLONG>( m_FontSizes.Count( ) );

	YFontSizeIterator	iterator		= m_FontSizes.Start( );
	YFontSizeIterator	iteratorEnd	= m_FontSizes.End( );	
	//	Write out the recommended font sizes
	for( ; iterator != iteratorEnd; ++iterator )
		archive << ( *iterator );
	}
			

