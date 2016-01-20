// ****************************************************************************
//
//  File Name:			URLList.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				C. Schendel
//
//  Description:		Definition of the RURLList 
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc. 
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME
#include "URLList.h"
#include "ChunkyStorage.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

const YChunkTag	kURLListChunkTag = 'URL_';

// ****************************************************************************
//
//				RURLList
//
// ****************************************************************************
//

// ****************************************************************************
//
//  Function Name:	RURLList::RURLList( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RURLList::RURLList( ) : RArray<RURL>( kURLListGrowSize )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RURLList::RURLList( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RURLList::RURLList( const RURLList& URLList ) : RArray<RURL>( kURLListGrowSize )
{
	*this = URLList;
}

// ****************************************************************************
//
//  Function Name:	RURLList::~RURLList( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RURLList::~RURLList( )
{

}

// ****************************************************************************
//
//  Function Name:	RURLList::AddURL( )
//
//  Description:		
//
//  Returns:		a pointer to the URL object in the list
//
//  Exceptions:		Memory Exception
//
// ****************************************************************************
//
const RURL * RURLList::AddURL( RURL &URL ) 
{
	YIterator	URLListIterator	= Start();
	int		i;
	uLONG	ulValidID = 1;

	// first see if the URL exists in the list
	for ( i = 0; URLListIterator != End(); ++URLListIterator, ++i)
	{
		RURL *pURL = &( *URLListIterator);		
		if( *pURL == URL )
			return pURL;
		if( ulValidID == pURL->GetID() )
			ulValidID++;
	}
	// the URL was not found, insert it
	URL.SetID( ulValidID );
	InsertAtEnd( URL );
	URLListIterator = Find( URL );
	TpsAssert( URLListIterator != End(), "URL find failed after insert" );
	return &( *URLListIterator);
}

// ****************************************************************************
//
//  Function Name:	RURLList::FindURL( )
//
//  Description: Return a RURL pointer for the given id
//
//  Returns:		The requested RURL object 
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
const RURL* RURLList::FindURL( YURLId id ) const
{
	YIterator	URLListIterator	= Start();
	int i;

	// first see if the URL exists in the list
	for ( i = 0; URLListIterator != End(); ++URLListIterator, ++i)
	{
		RURL *pURL = &( *URLListIterator);		
		if( pURL->GetID() == id )
			return pURL;
	}
	// the URL was not found
	return NULL;
}

// ****************************************************************************
//
//  Function Name:	RURLList::WriteURLList( )
//
//  Description:		Writes the URL List to the storage passed
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RURLList::WriteURLList( RChunkStorage& storage ) const
{
	//	Add a new chunk of data
	storage.AddChunk( kURLListChunkTag );

	Write( storage );
	//	Reset to the parent chunk of data...
	storage.SelectParentChunk( );
}

// ****************************************************************************
//
//  Function Name:	RURLList::Write( )
//
//  Description:	Writes the URL List to the archive
//
//  Returns:		Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RURLList::Write( RArchive& archive ) const
{
	YFileFont	fileFont;
	uLONG	ulURLCount	= Count( );

	archive << ulURLCount;

	YIterator	iterator	= Start( );
	for ( ulURLCount = 0; iterator != End( ); ++ulURLCount, ++iterator )
	{
		RURL *pURL = &( *iterator);
		pURL->Write( archive );
	}
}

// ****************************************************************************
//
//  Function Name:	RURLList::ReadURLList( )
//
//  Description:		Reads the URL List from the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RURLList::ReadURLList( RChunkStorage& storage )
{
	// Iterate for the URL list
	RChunkStorage::RChunkSearcher searcher = storage.Start( kURLListChunkTag, FALSE );

	// if found read it
	if ( !searcher.End() )
	{
		Read( storage );
		//	Reset to the parent chunk of data...
		storage.SelectParentChunk( );
	}
}

// ****************************************************************************
//
//  Function Name:	RURLList::Read( )
//
//  Description:	Reads the URL List from the archive
//
//  Returns:		Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RURLList::Read( RArchive& archive )
{
	uLONG	ulURLCount;
	uLONG	ul;
	// empty the list first
	Empty();
	//	Get the number of URLS in the chunk
	archive >> ulURLCount;
	//	Loop through and read them in...
	for ( ul = 0; ul < ulURLCount; ++ul )
	{
		RURL URL;
		URL.Read( archive );
		InsertAtEnd( URL );
	}
}

// ****************************************************************************
//
//  Function Name:	 RURLList::operator=()
//
//  Description:	assignment operator
//
//  Returns:		Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
RURLList& RURLList::operator=( const RURLList& URLList )
{
	//	Remove all entries from current list
	Empty( );

	//	Let the array copy all of its members
	RArray<RURL>::operator=( URLList );

	return *this;
}

