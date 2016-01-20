// ****************************************************************************
//
//  File Name:			CustomGraphicSearch.h
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of classes that perform a search on a custom
//							graphic collection file, suitable for use as the
//							parameterized type of an RCollectionSearcher
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1997 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef _CustomGraphicSearch_H_
#define _CustomGraphicSearch_H_


#include "SearchCollection.h"					// RSearchCollection abstract base class


// RCustomGraphicSearchCollection
// base class for RNumberGraphicSearchCollection, etc. below
class RCustomGraphicSearchCollection : public RSearchCollection
{
public:
						RCustomGraphicSearchCollection(
							RChunkStorage *aChunkStorage, YChunkTag anIndexChunkType );

	virtual			~RCustomGraphicSearchCollection();

	virtual void	SetSearchCriteria( const RSearchCriteria* )
						// search criteria not relevant for custom graphics because all
						// number/initCap/logo/sealInner/sealOuter/signature/timepiece
						// graphics are always displayed
						{ TpsAssertAlways( "RCustomGraphicSearchCollection::SetSearchCriteria() should not be used." ); }

	virtual uLONG	GetNumItems();
						// gets the number of items to be searched in the collection
						// throws exception if theIndexChunkType chunk can't be found
	
	virtual void	InitSearch();
						// Call RSearchCollection::InitSearch(), set
						// RSearchCollection::theTotalEntries and prepare for reading first
						// index entry

	virtual void	SeekToFirstIndexEntry();
						// set RSearchCollection::theTotalEntries
						// and prepare for reading first index entry

	virtual BOOLEAN ReadNextIndexEntry();
						// read the next index entry
						// return TRUE for success, else FALSE
						// throws exception if a read error occurs

	virtual BOOLEAN MatchSearchCriteria()
						// always return TRUE
						// (search criteria not relevant for custom graphics because all
						// number/initCap/logo/sealInner/sealOuter/signature/timepiece
						// graphics are always displayed)
						{ return TRUE; }

private:
	BYTE*				theINDX;
	YChunkTag		theIndexChunkType;
	uLONG				theIndexOffset;

	YStreamLength	theKeywordOffset;
	uWORD				theMinorCategory;
	sWORD				theGraphicType;

	void				CacheIndexChunk();
};


// RNumberGraphicSearchCollection
// Performs a search on a custom graphic collection file for all Number graphics
// Suitable for use as the parameterized type of an RCollectionSearcher
class RNumberGraphicSearchCollection : public RCustomGraphicSearchCollection
{
public:
						RNumberGraphicSearchCollection( RChunkStorage *aChunkStorage )
						:	RCustomGraphicSearchCollection( aChunkStorage, 'INNU' )
						{}
};

// RInitCapGraphicSearchCollection
// Performs a search on a custom graphic collection file for all InitCap graphics
// Suitable for use as the parameterized type of an RCollectionSearcher
class RInitCapGraphicSearchCollection : public RCustomGraphicSearchCollection
{
public:
						RInitCapGraphicSearchCollection( RChunkStorage *aChunkStorage )
						:	RCustomGraphicSearchCollection( aChunkStorage, 'ININ' )
						{}
};

// RLogoGraphicSearchCollection
// Performs a search on a custom graphic collection file for all Logo graphics
// Suitable for use as the parameterized type of an RCollectionSearcher
class RLogoGraphicSearchCollection : public RCustomGraphicSearchCollection
{
public:
						RLogoGraphicSearchCollection( RChunkStorage *aChunkStorage )
						:	RCustomGraphicSearchCollection( aChunkStorage, 'INLO' )
						{}
};

// RSealInnerGraphicSearchCollection
// Performs a search on a custom graphic collection file for all SealInner graphics
// (also known as "spot" graphics)
// Suitable for use as the parameterized type of an RCollectionSearcher
class RSealInnerGraphicSearchCollection : public RCustomGraphicSearchCollection
{
public:
						RSealInnerGraphicSearchCollection( RChunkStorage *aChunkStorage )
						:	RCustomGraphicSearchCollection( aChunkStorage, 'INSP' )
						{}
};

// RSealOuterGraphicSearchCollection
// Performs a search on a custom graphic collection file for all SealOuter graphics
// Suitable for use as the parameterized type of an RCollectionSearcher
class RSealOuterGraphicSearchCollection : public RCustomGraphicSearchCollection
{
public:
						RSealOuterGraphicSearchCollection( RChunkStorage *aChunkStorage )
						:	RCustomGraphicSearchCollection( aChunkStorage, 'INSE' )
						{}
};

// RSignatureGraphicSearchCollection
// Performs a search on a custom graphic collection file for all Signature graphics
// (also known as "autograph" graphics)
// Suitable for use as the parameterized type of an RCollectionSearcher
class RSignatureGraphicSearchCollection : public RCustomGraphicSearchCollection
{
public:
						RSignatureGraphicSearchCollection( RChunkStorage *aChunkStorage )
						:	RCustomGraphicSearchCollection( aChunkStorage, 'INAU' )
						{}
};

// RTimepieceGraphicSearchCollection
// Performs a search on a custom graphic collection file for all Timepiece graphics
// Suitable for use as the parameterized type of an RCollectionSearcher
class RTimepieceGraphicSearchCollection : public RCustomGraphicSearchCollection
{
public:
						RTimepieceGraphicSearchCollection( RChunkStorage *aChunkStorage )
						:	RCustomGraphicSearchCollection( aChunkStorage, 'INTI' )
						{}
};

// RCalendarPartGraphicSearchCollection
// Performs a search on a custom graphic collection file for all Calendar Part graphics
// Suitable for use as the parameterized type of an RCollectionSearcher
class RCalendarPartGraphicSearchCollection : public RCustomGraphicSearchCollection
{
public:
						RCalendarPartGraphicSearchCollection( RChunkStorage *aChunkStorage )
						:	RCustomGraphicSearchCollection( aChunkStorage, 'INCA' )
						{}
};


#endif	// _CustomGraphicSearch_H_
