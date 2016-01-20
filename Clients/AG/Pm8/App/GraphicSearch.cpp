/*	$Header: /PM8/App/GraphicSearch.cpp 1     3/03/99 6:05p Gbeddow $
//
//	Definition of the CPTimePieceDlg class.
//
//	Definition of abstract base class for searching on
// a graphics collection file. Concrete subclasses are
//	suitable as parameterized type of an RCollectionSearcher.
//	
// Author:			Greg Beddow
//
// Portability:	Cross platform
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
// Copyright (C) 1998 Broderbund Software, Inc. All Rights Reserved.
//
// $Log: /PM8/App/GraphicSearch.cpp $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 1     2/01/99 5:40p Rgrenfel
// Graphic search support classes.
*/

#include "StdAfx.h"	// standard Renaissance includes

ASSERTNAME

#include "GraphicSearch.h"				// header for this class


#define CACHE_INDEX 1


/****************************************************************************
	RGraphicSearchCollection::RGraphicSearchCollection( RChunkStorage *aChunkStorage )

	Construct an RGraphicSearchCollection from aChunkStorage
*****************************************************************************/

RGraphicSearchCollection::RGraphicSearchCollection( RChunkStorage *aChunkStorage )
	:	RSearchCollection( aChunkStorage ),
		theKeywordOffset( 0 ),
		theINDX( 0 ),
		theIndexOffset( 0 )
{
}

/****************************************************************************
	RGraphicSearchCollection::~RGraphicSearchCollection()

	Destructor for an RGraphicSearchCollection
*****************************************************************************/

RGraphicSearchCollection::~RGraphicSearchCollection()
{
	delete[] theINDX;
}

/****************************************************************************
	uLONG RGraphicSearchCollection::GetNumItems()

	Gets the number of items to be searched in the collection.
*****************************************************************************/

uLONG RGraphicSearchCollection::GetNumItems()
{
#if CACHE_INDEX
	CacheIndexChunk();
	TpsAssert( theINDX, "NULL index info." );
	uLONG numItems = *((uLONG*)theINDX);
	MacCode( numItems = ::tintSwapLong( numItems ); );
	return numItems;
#else
	SeekToFirstIndexEntry();
	return theTotalEntries;
#endif
}

/****************************************************************************
	void RGraphicSearchCollection::InitSearch()

	Call RSearchCollection::InitSearch(), set
	RSearchCollection::theTotalEntries and prepare for reading first
	index entry.

	Throws exception if INAL chunk can't be found in
	theChunkStorage or on a read error.
*****************************************************************************/

void RGraphicSearchCollection::InitSearch()
{
#if CACHE_INDEX
	CacheIndexChunk();
#endif
	RSearchCollection::InitSearch();
}

/****************************************************************************
	void RGraphicSearchCollection::SeekToFirstIndexEntry()

	Set RSearchCollection::theTotalEntries.
	Prepare for reading first index entry.
*****************************************************************************/

void RGraphicSearchCollection::SeekToFirstIndexEntry()
{
#if CACHE_INDEX
	TpsAssert( theINDX, "NULL index info." );

	RSearchCollection::SeekToFirstIndexEntry();

	theTotalEntries = *((uLONG*)theINDX);
	MacCode( theTotalEntries = ::tintSwapLong( theTotalEntries ); );

	theEntrySize = *((YStreamOffset*)(theINDX + sizeof(theTotalEntries)));
	MacCode( theEntrySize = ::tintSwapLong( theEntrySize ); );

	if (theEntrySize < 0)
		throw kBadSeek;

	// skip over record count + record size
	theIndexOffset = sizeof(theTotalEntries) + sizeof(theEntrySize);

#else

	RSearchCollection::SeekToFirstIndexEntry();

	RChunkStorage::RChunkSearcher yIter =
		theChunkStorage->Start( 'INAL', FALSE );
	if (yIter.End())
		throw kBadSeek;
	else
	{
		theChunkStorage->SeekAbsolute( 0 );
		*theChunkStorage >> theTotalEntries;
		*theChunkStorage >> theEntrySize;
		if (theEntrySize < 0)
			throw kBadSeek;
	}
#endif // CACHE_INDEX
}

/****************************************************************************
	BOOLEAN RGraphicSearchCollection::ReadNextIndexEntry()

	Read the next index entry.
	Return TRUE for success, else FALSE.
	Throws exception if a read error occurs.
*****************************************************************************/

BOOLEAN RGraphicSearchCollection::ReadNextIndexEntry()
{
#if CACHE_INDEX
	if (!RSearchCollection::ReadNextIndexEntry())
		return FALSE;

	const YStreamOffset bytesToRead =
		sizeof(theNameOffset) +
		sizeof(theDataOffset) +
		sizeof(theDataOffset) +
		sizeof(thePrimaryMinorCategory) +
		sizeof(theGraphicType) +
		sizeof(theID) +
		sizeof(theSecondaryMinorCategory) +
		sizeof(theContentType);

	BYTE* pIndex = theINDX + theIndexOffset;

	theNameOffset = *((uLONG*)pIndex);
	pIndex += sizeof(theNameOffset);

	theKeywordOffset = *((uLONG*)pIndex);
	pIndex += sizeof(theKeywordOffset);

	theDataOffset = *((uLONG*)pIndex);
	pIndex += sizeof(theDataOffset);

	thePrimaryMinorCategory = *((uWORD*)pIndex);
	pIndex += sizeof(thePrimaryMinorCategory);

	theGraphicType = *((sWORD*)pIndex);
	pIndex += sizeof(theGraphicType);

	theID = *((uLONG*)pIndex);
	pIndex += sizeof(theID);

	theSecondaryMinorCategory = *((uWORD*)pIndex);
	pIndex += sizeof(theSecondaryMinorCategory);

	theContentType = *((uWORD*)pIndex);
	pIndex += sizeof(theContentType);

	theIndexOffset += bytesToRead;

	MacCode( theNameOffset = ::tintSwapLong( theNameOffset ); );
	MacCode( theDataOffset = ::tintSwapLong( theDataOffset ); );
	MacCode( thePrimaryMinorCategory = ::tintSwapWord( thePrimaryMinorCategory ); );
	MacCode( theGraphicType = ::tintSwapWord( theGraphicType ); );
	MacCode( theID = ::tintSwapLong( theID ); );
	MacCode( theSecondaryMinorCategory = ::tintSwapLong( theSecondaryMinorCategory ); );
	MacCode( theContentType = ::tintSwapLong( theContentType ); );

	// skip over future additions to collection file format
	YStreamOffset bytesToSkip = theEntrySize - bytesToRead;
	if (bytesToSkip < 0)
		throw kBadSeek;
	else
		theIndexOffset += bytesToSkip;

	return TRUE;

#else

	if (!RSearchCollection::ReadNextIndexEntry())
		return FALSE;

	const YStreamOffset bytesToRead =
		sizeof(theNameOffset) +
		sizeof(theDataOffset) +
		sizeof(theDataOffset) +
		sizeof(thePrimaryMinorCategory) +
		sizeof(theGraphicType) +
		sizeof(theID) +
		sizeof(theSecondaryMinorCategory) +
		sizeof(theContentType);

	*theChunkStorage >> theNameOffset;
	*theChunkStorage >> theDataOffset;	// keyword offset - ignore
	*theChunkStorage >> theDataOffset;	// graphic offset - keep
	*theChunkStorage >> thePrimaryMinorCategory;
	*theChunkStorage >> theGraphicType;
	*theChunkStorage >> theID;
	*theChunkStorage >> theSecondaryMinorCategory;
	*theChunkStorage >> theContentType;

	// skip over future additions to collection file format
	YStreamOffset bytesToSkip = theEntrySize - bytesToRead;
	if (bytesToSkip < 0)
		throw kBadSeek;
	else if (bytesToSkip > 0)
		theChunkStorage->SeekRelative( bytesToSkip );

	return TRUE;
#endif // CACHE_INDEX
}

/****************************************************************************
	void RGraphicSearchCollection::CacheIndexChunk()

	If the INAL chunk has not already been cached,
	seek to beginning of INAL chunk and cache into theINDX.
	Throws exception if theIndexChunkType chunk can't be found or on a read error.
*****************************************************************************/

void RGraphicSearchCollection::CacheIndexChunk()
{
	if (!theINDX)
	{
		theChunkStorage->SelectRootChunk();
		RChunkStorage::RChunkSearcher yIter = theChunkStorage->Start( 'INDX', FALSE );
		if (yIter.End())
			throw kBadSeek;
		else
		{
			yIter = theChunkStorage->Start( 'INAL', FALSE );
			if (yIter.End())
				throw kBadSeek;
			else
			{
				YStreamLength yBufferSize = theChunkStorage->GetCurrentChunkSize();
				theINDX = new BYTE[yBufferSize];
				theChunkStorage->SeekAbsolute( 0 );
				theChunkStorage->Read( yBufferSize, theINDX );
			}
		}
	}
}

