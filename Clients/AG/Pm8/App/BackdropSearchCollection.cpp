// ****************************************************************************
//
//  File Name:			BackdropSearchCollection.cpp
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Definition of class that performs a category search on
//							a backdrop collection file, suitable for use as the
//							parameterized type of an RCollectionSearcher
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1997 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#include "stdafx.h"	// standard Renaissance includes
ASSERTNAME

#include "BackdropSearchCollection.h"				// header for this class


// GCB 4/10/98 - debug - set to 1 to dump backdrop collection to backdrop.dat
#define DUMP_BACKDROP 0
#if DUMP_BACKDROP
static FILE* outFile = NULL;
#endif


/****************************************************************************
	RBackdropSearchCollection::RBackdropSearchCollection( RChunkStorage *aChunkStorage )

	Construct an RBackdropSearchCollection from aChunkStorage
*****************************************************************************/

RBackdropSearchCollection::RBackdropSearchCollection( RChunkStorage *aChunkStorage )
	:	RSearchCollection( aChunkStorage ),
		theCurrentPrimaryMinorCategory( 0 ),
		theCurrentBackdropAspectType( 0 ),
		theCurrentSecondaryMinorCategory( 0 ),
		theCurrentContentType( 0 )
{
#if DUMP_BACKDROP
	outFile = fopen( "backdrop.dat", "wt" );
#endif
}

/****************************************************************************
	RBackdropSearchCollection::~RBackdropSearchCollection()

	Destructor for an RBackdropSearchCollection
*****************************************************************************/

RBackdropSearchCollection::~RBackdropSearchCollection()
{
#if DUMP_BACKDROP
	fclose( outFile );
#endif
}

/****************************************************************************
	void RBackdropSearchCollection::SetSearchCriteria(
		const RSearchCriteria* aSearchCriteria)

	Set theBackdropSearchCriteria used by MatchSearchCriteria() and GetNumItems()
*****************************************************************************/

void RBackdropSearchCollection::SetSearchCriteria(
	const RSearchCriteria* aSearchCriteria )
{
	RSearchCriteria* aNonConstSearchCriteria =
		const_cast<RSearchCriteria*>(aSearchCriteria);
	RBackdropSearchCriteria* aBackdropSearchCriteria =
		dynamic_cast<RBackdropSearchCriteria*>(aNonConstSearchCriteria);

	if (aBackdropSearchCriteria == NULL)
	{
		TpsAssertAlways( "A valid RBackdropSearchCriteria object is required." );
	}
	else
	{
		theBackdropSearchCriteria = *aBackdropSearchCriteria;
	}
}

/****************************************************************************
	uLONG RBackdropSearchCollection::GetNumItems()

	Gets the number of items to be searched in the collection.
	Throws exception if ITBK chunk can't be found or on a read error.
*****************************************************************************/

uLONG RBackdropSearchCollection::GetNumItems()
{
	SeekToFirstIndexEntry();
	return theTotalEntries;
}

/****************************************************************************
	void RBackdropSearchCollection::InitSearch()

	Seek to beginning of ITBK chunk, read RSearchCollection::theTotalEntries
	and leave aChunkStorage positioned to the first index entry.
	Throws exception if ITBK chunk can't be found in
	theChunkStorage or on a read error.
*****************************************************************************/

void RBackdropSearchCollection::InitSearch()
{
	RSearchCollection::InitSearch();

	SeekToFirstIndexEntry();
}

/****************************************************************************
	void RBackdropSearchCollection::SeekToFirstIndexEntry()

	Seek to beginning of ITBK chunk, read RSearchCollection::theTotalEntries.
	Leave theChunkStorage positioned for reading first index entry.
	Throws exception if ITBK chunk can't be found or on a read error.
*****************************************************************************/

void RBackdropSearchCollection::SeekToFirstIndexEntry()
{
	RSearchCollection::SeekToFirstIndexEntry();

	RChunkStorage::RChunkSearcher yIter =
		theChunkStorage->Start( 'ITBK', FALSE );
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
}

/****************************************************************************
	BOOLEAN RBackdropSearchCollection::ReadNextIndexEntry()

	Read the next ITBK index entry from theChunkStorage
	(assumes that theChunkStorage is already positioned to the next entry).
	Return TRUE for success, else FALSE.
	Throws exception if a read error occurs.
*****************************************************************************/

BOOLEAN RBackdropSearchCollection::ReadNextIndexEntry()
{
	if (!RSearchCollection::ReadNextIndexEntry())
		return FALSE;

	const YStreamOffset bytesToRead =
		sizeof(theNameOffset) +
		sizeof(theDataOffset) +
		sizeof(theCurrentBackdropAspectType) +
		sizeof(theCurrentPrimaryMinorCategory) +
		sizeof(theID) +
		sizeof(theCurrentSecondaryMinorCategory) +
		sizeof(theCurrentContentType);

	*theChunkStorage >> theNameOffset;
	*theChunkStorage >> theDataOffset;
	*theChunkStorage >> theCurrentBackdropAspectType;
	*theChunkStorage >> theCurrentPrimaryMinorCategory;
	*theChunkStorage >> theID;
	*theChunkStorage >> theCurrentSecondaryMinorCategory;
	*theChunkStorage >> theCurrentContentType;

	// skip over future additions to collection file format
	YStreamOffset bytesToSkip = theEntrySize - bytesToRead;
	if (bytesToSkip < 0)
		throw kBadSeek;
	else if (bytesToSkip > 0)
		theChunkStorage->SeekRelative( bytesToSkip );

	return TRUE;
}

/****************************************************************************
	BOOLEAN RBackdropSearchCollection::MatchSearchCriteria()

	Compare the current index entry to theBackdropSearchCriteria set by
	SetSearchCriteria().
	Return TRUE if equal, else FALSE.
*****************************************************************************/

BOOLEAN RBackdropSearchCollection::MatchSearchCriteria()
{
#if DUMP_BACKDROP
	fprintf( outFile, "AspectType=%#2d ID=%#5d Primary=%#5d Secondary=%#5d SearchContentType=%#5d CurrentContentType=%#5d name=%s\n",
		theCurrentBackdropAspectType, theID, theCurrentPrimaryMinorCategory,
		theCurrentSecondaryMinorCategory, theBackdropSearchCriteria.theContentType, theCurrentContentType, NAMS() + theNameOffset );
#endif

	// if the search criteria content type is not "All" and we don't
	// have a matching content type
	if (theBackdropSearchCriteria.theContentType != 0 &&
		theBackdropSearchCriteria.theContentType != theCurrentContentType)
		return FALSE;

	// if we have a matching BackdropAspectType or it's a tiled backdrop
	// that should be considered...
	if (theBackdropSearchCriteria.theBackdropAspectType == theCurrentBackdropAspectType ||
		(!theBackdropSearchCriteria.theOmitTiledBackdropsFlag && theCurrentBackdropAspectType == btTiled))
	{
		// if the search criteria category is "All"...
		if (theBackdropSearchCriteria.theMajorCategory == 0)
		{
			// if the search criteria content type is "All" or matching
			return TRUE;
		}
		else
		{
			// if this is a minor category search...
			if (theBackdropSearchCriteria.theMajorCategory !=
				theBackdropSearchCriteria.theMinorCategory)
			{
				// if we have a matching minor category...
				if (theBackdropSearchCriteria.theMinorCategory == theCurrentPrimaryMinorCategory)
					return TRUE;
			}
			else	// it's a major category search...
			{
				// if we have a matching major category...
				int currentPrimaryMajorCategory =
					theCurrentPrimaryMinorCategory - (theCurrentPrimaryMinorCategory % 100);
				if (theBackdropSearchCriteria.theMajorCategory == currentPrimaryMajorCategory)
					return TRUE;
			}
		}
	}
	return FALSE;
}

