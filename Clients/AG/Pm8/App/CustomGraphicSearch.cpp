// ****************************************************************************
//
//  File Name:			CustomGraphicSearch.cpp
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Definition of class that performs a search on a custom
//							graphic collection file. Subclasses (defined in header
//							file CustomGraphicSearch.h) are suitable for use
//							as the parameterized type of an RCollectionSearcher.
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1997 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#include "StdAfx.h"

ASSERTNAME

#include "CustomGraphicSearch.h"				// header for this class


/****************************************************************************
	RCustomGraphicSearchCollection::RCustomGraphicSearchCollection(
		RChunkStorage *aChunkStorage, YChunkTag anIndexChunkType )

	Construct an RCustomGraphicSearchCollection from aChunkStorage
	and anIndexChunkType. (Also seeks to beginning of anIndexChunkType chunk and
	caches into memory in theINDX for better performance.)
*****************************************************************************/

RCustomGraphicSearchCollection::RCustomGraphicSearchCollection(
	RChunkStorage *aChunkStorage, YChunkTag anIndexChunkType )
	:	RSearchCollection( aChunkStorage ),
		theINDX( 0 ),
		theIndexChunkType( anIndexChunkType ),
		theIndexOffset( 0 ),
		theKeywordOffset( 0 ),
		theMinorCategory( 0 ),
		theGraphicType( 0 )
{
	TpsAssert( theIndexChunkType != 0, "Index chunk type must be non-zero." );

	CacheIndexChunk();	// cache into theINDX for better performance
}

/****************************************************************************
	RCustomGraphicSearchCollection::~RCustomGraphicSearchCollection()

	Destructor for an RCustomGraphicSearchCollection
*****************************************************************************/

RCustomGraphicSearchCollection::~RCustomGraphicSearchCollection()
{
	delete[] theINDX;
}

/****************************************************************************
	uLONG RCustomGraphicSearchCollection::GetNumItems()

	Gets the number of items to be searched in the collection.
	Throws exception if theIndexChunkType chunk can't be found or on a read error.
*****************************************************************************/

uLONG RCustomGraphicSearchCollection::GetNumItems()
{
	TpsAssert( theINDX, "NULL index info." );

	uLONG numItems = *((uLONG*)theINDX);
	MacCode( numItems = ::tintSwapLong( numItems ); );
	return numItems;
}

/****************************************************************************
	void RCustomGraphicSearchCollection::InitSearch()

	Call RSearchCollection::InitSearch(), set
	RSearchCollection::theTotalEntries and prepare for reading first
	index entry
*****************************************************************************/

void RCustomGraphicSearchCollection::InitSearch()
{
	RSearchCollection::InitSearch();

	SeekToFirstIndexEntry();
}

/****************************************************************************
	void RCustomGraphicSearchCollection::SeekToFirstIndexEntry()

	Set RSearchCollection::theTotalEntries.
	Prepare for reading first index entry.
*****************************************************************************/

void RCustomGraphicSearchCollection::SeekToFirstIndexEntry()
{
	TpsAssert( theINDX, "NULL index info." );

	RSearchCollection::SeekToFirstIndexEntry();

	theTotalEntries = *((uLONG*)theINDX);
	MacCode( theTotalEntries = ::tintSwapLong( theTotalEntries ); );

#if 1 // GCB 5/23/97 - enable when new collection format is ready
	theEntrySize = *((YStreamOffset*)(theINDX + 4));
	MacCode( theEntrySize = ::tintSwapLong( theEntrySize ); );

	if (theEntrySize < 0)
		throw kBadSeek;

	theIndexOffset = 8; // skip over record count + record size
#else
	theIndexOffset = 4; // skip over count
#endif
}

/****************************************************************************
	BOOLEAN RCustomGraphicSearchCollection::ReadNextIndexEntry()

	Read the next index entry.
	Return TRUE for success, else FALSE.
	Throws exception if a read error occurs.
*****************************************************************************/

BOOLEAN RCustomGraphicSearchCollection::ReadNextIndexEntry()
{
	if (!RSearchCollection::ReadNextIndexEntry())
		return FALSE;

	const YStreamOffset bytesToRead =
		4 +	// theNameOffset
		4 +	// theKeywordOffset
		4 +	// theDataOffset
		2 +	// theMinorCategory
		2 +	// theGraphicType
		4;		// theID

	BYTE* pIndex = theINDX + theIndexOffset;

	theNameOffset = *((uLONG*)pIndex);
	pIndex += 4;
	theKeywordOffset = *((uLONG*)pIndex);	// unused
	pIndex += 4;
	theDataOffset = *((uLONG*)pIndex);
	pIndex += 4;
	theMinorCategory = *((uWORD*)pIndex);	// unused
	pIndex += 2;
	theGraphicType = *((sWORD*)pIndex);		// unused
	pIndex += 2;
	theID = *((uLONG*)pIndex);

	theIndexOffset += bytesToRead;

	MacCode( theNameOffset = ::tintSwapLong( theNameOffset ); );
	MacCode( theKeywordOffset = ::tintSwapLong( theKeywordOffset ); );
	MacCode( theDataOffset = ::tintSwapLong( theDataOffset ); );
	MacCode( theMinorCategory = ::tintSwapWord( theMinorCategory ); );
	MacCode( theGraphicType = ::tintSwapWord( theGraphicType ); );
	MacCode( theID = ::tintSwapLong( theID ); );

#if 1 // GCB 5/23/97 - enable when new collection format is ready
	// skip over future additions to collection file format
	YStreamOffset bytesToSkip = theEntrySize - bytesToRead;
	if (bytesToSkip < 0)
		throw kBadSeek;
	else
		theIndexOffset += bytesToSkip;
#endif

	return TRUE;
}

/****************************************************************************
	void RCustomGraphicSearchCollection::CacheIndexChunk()

	Seek to beginning of theIndexChunkType chunk and cache into theINDX.
	Throws exception if theIndexChunkType chunk can't be found or on a read error.
*****************************************************************************/

void RCustomGraphicSearchCollection::CacheIndexChunk()
{
   theChunkStorage->SelectRootChunk();
   RChunkStorage::RChunkSearcher yIter = theChunkStorage->Start( 'INDX', FALSE );
   if (yIter.End())
		throw kBadSeek;
	else
	{
		yIter = theChunkStorage->Start( theIndexChunkType, FALSE );
		if (yIter.End())
			throw kBadSeek;
		else
		{
			YStreamLength yBufferSize = theChunkStorage->GetCurrentChunkSize();
			delete theINDX;
			theINDX = new BYTE[yBufferSize];
			theChunkStorage->SeekAbsolute( 0 );
			theChunkStorage->Read( yBufferSize, theINDX );
	   }
	}
}

