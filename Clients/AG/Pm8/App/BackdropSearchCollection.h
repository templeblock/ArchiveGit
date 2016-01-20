// ****************************************************************************
//
//  File Name:			BackdropSearchCollection.h
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of class that performs a category search on
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

#ifndef _BackdropSearchCollection_H_
#define _BackdropSearchCollection_H_


#include "SearchCollection.h"					// RSearchCollection abstract base class
#include "SearchResult.h"						// cbBACKGROUNDTYPES enum


class RBackdropSearchCriteria : public RSearchCriteria
{
public:
	uWORD				theMajorCategory;
	uWORD				theMinorCategory;
	uWORD				theBackdropAspectType;	// a value from cbBACKGROUNDTYPES enum
	BOOLEAN			theOmitTiledBackdropsFlag;
	uWORD				theContentType;

						RBackdropSearchCriteria()
						:	theMajorCategory( 0 ),
							theMinorCategory( 0 ),
							theBackdropAspectType( 0 ),
							theOmitTiledBackdropsFlag( FALSE ),
							theContentType( 0 )
						{}

						RBackdropSearchCriteria(
							uWORD		aCategory,
							uWORD		aBackdropAspectType,
							BOOLEAN	anOmitTiledBackdropsFlag = FALSE,
							uWORD		aContentType = 0 )
						// construct an RBackdropSearchCriteria:
						//
						// aCategory may be either 0="All" or a major or minor
						// category id obtained by searching using an
						// RCollectionSearcher<RSearchBackdropCollectionCategories>
						// if a major category, specifies backdrops with a matching
						// major category; if a minor category, specifies backdrops
						// with a matching minor category
						//
						// aBackdropAspectType specifies the backdrop aspect type
						//
						// anOmitTiledBackdropsFlag, if TRUE, indicates that tiled
						// backdrops should be omitted from the search
						//
						// aContentType may be either 0="All" or a content type id
						// obtained by searching using an
						// RCollectionSearcher<RSearchCollectionGraphicContents>
						//
						// useful for creating a search criteria object to pass to
						// RCollectionSearcher::SetSearchCriteria() for a category and
						// BackdropAspectType search
							:	theMajorCategory( (uWORD)(aCategory - (aCategory % 100)) ),
								theMinorCategory( aCategory ),
								theBackdropAspectType( aBackdropAspectType ),
								theOmitTiledBackdropsFlag( anOmitTiledBackdropsFlag ),
								theContentType( aContentType )
						{
						}

	const RBackdropSearchCriteria& operator=( const RBackdropSearchCriteria& rhs )
						{
							theMajorCategory = rhs.theMajorCategory;
							theMinorCategory = rhs.theMinorCategory;
							theBackdropAspectType = rhs.theBackdropAspectType;
							theOmitTiledBackdropsFlag = rhs.theOmitTiledBackdropsFlag;
							theContentType = rhs.theContentType;
							return *this;
						}
};


class RBackdropSearchCollection : public RSearchCollection
{
public:
						RBackdropSearchCollection( RChunkStorage *aChunkStorage );
						// construct an RBackdropSearchCollection from aChunkStorage

	virtual			~RBackdropSearchCollection();

	virtual void	SetSearchCriteria( const RSearchCriteria* aSearchCriteria );
						// set theBackdropSearchCriteria used by
						// MatchSearchCriteria() and GetNumItems()

	virtual uLONG	GetNumItems();
						// gets the number of items to be searched in the collection
						// throws exception if INAL chunk can't be found
	
	virtual void	InitSearch();
						// seek to beginning of ITBK chunk, read RSearchCollection::theTotalEntries
						// and leave aChunkStorage positioned to the first index entry
						// throws exception if ITBK chunk can't be found in
						// theChunkStorage or on a read error

	virtual void	SeekToFirstIndexEntry();
						// seek to beginning of ITBK chunk and read theTotalEntries
						// leave theChunkStorage positioned for reading first index entry

	virtual BOOLEAN ReadNextIndexEntry();
						// read the next ITBK index entry from theChunkStorage
						// (assumes that theChunkStorage is already positioned to the next entry)
						// return TRUE for success, else FALSE
						// throws exception if a read error occurs

	virtual BOOLEAN MatchSearchCriteria();
						// compare the current index entry to theBackdropSearchCriteria
						// set by SetSearchCriteria()
						// return TRUE if equal, else FALSE

protected:
	uWORD				theCurrentPrimaryMinorCategory;
	uWORD				theCurrentBackdropAspectType;
	uWORD				theCurrentSecondaryMinorCategory;
	uWORD				theCurrentContentType;

	RBackdropSearchCriteria	theBackdropSearchCriteria;
};


#endif	// _BackdropSearchCollection_H_
