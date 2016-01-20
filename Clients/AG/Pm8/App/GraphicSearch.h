// ****************************************************************************
//
//  File Name:			GraphicSearch.h
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of abstract base class for searching on
//							a graphics collection file. Concrete subclasses are
//							suitable as parameterized type of an RCollectionSearcher.
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef _GraphicSearch_H_
#define _GraphicSearch_H_


#include "SearchCollection.h"					// RSearchCollection abstract base class
#include "SearchResult.h"


class RGraphicSearchCollection : public RSearchCollection
{
public:
						RGraphicSearchCollection( RChunkStorage *aChunkStorage );
						// construct an RGraphicSearchCollection from aChunkStorage

	virtual			~RGraphicSearchCollection();

	virtual void	SetSearchCriteria( const RSearchCriteria* aSearchCriteria )=0;
						// Initializes the RSearchCriteria object. This object is
						// used by MatchSearchCriteria() to perform
						// the appropriate filtering operations.

	virtual uLONG	GetNumItems();
						// Gets the number of items to be searched in the collection.
						// Throws exception if INAL chunk can't be found or on a read error.
	
	virtual void	InitSearch();
						// Call RSearchCollection::InitSearch(), set
						// RSearchCollection::theTotalEntries and prepare for reading first
						// index entry.
						// Throws exception if INAL chunk can't be found in
						// theChunkStorage or on a read error.

	virtual void	SeekToFirstIndexEntry();
						// Set RSearchCollection::theTotalEntries.
						// Prepare for reading first index entry.

	virtual BOOLEAN ReadNextIndexEntry();
						// Read the next index entry.
						// Return TRUE for success, else FALSE.

protected:
	YStreamLength	theKeywordOffset;

	BYTE*				theINDX;
	uLONG				theIndexOffset;

	uWORD				thePrimaryMinorCategory;
	sWORD				theGraphicType;
	uWORD				theSecondaryMinorCategory;
	uWORD				theContentType;

	void				CacheIndexChunk();
};


#endif	// _GraphicSearch_H_
