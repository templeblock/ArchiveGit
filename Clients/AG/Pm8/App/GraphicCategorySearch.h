// ****************************************************************************
//
//  File Name:			GraphicCategorySearch.h
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of class that performs a category search on
//							a graphics collection file, suitable for use as the
//							parameterized type of an RCollectionSearcher
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1997 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef _GraphicCategorySearch_H_
#define _GraphicCategorySearch_H_


#include "GraphicSearch.h"				// RGraphicSearchCollection abstract base class
#include "SearchResult.h"


class RGraphicCategorySearchCriteria : public RSearchCriteria
{
public:
	uWORD				theMajorCategory;
	uWORD				theMinorCategory;
	sWORD				theGraphicType;	// set to values from cbSHAPETYPES in SearchCollection.h
												// setting to stImage will match either
												// stImage or stImageRef

						RGraphicCategorySearchCriteria()
						:	theMajorCategory( 0 ),
							theMinorCategory( 0 ),
							theGraphicType( stAnyType )
						{}

						RGraphicCategorySearchCriteria( uWORD aCategory )
						// Construct an RGraphicCategorySearchCriteria from aCategory,
						// which may be a major or minor category or 0="All".
						// Useful for creating a search criteria object to pass to
						// RCollectionSearcher::SetSearchCriteria().
						// If aCategory is a major category,
						// RCollectionSearcher::SearchCollections() will
						// look for backdrops with a matching major category.
						// If aCategory is a minor category,
						// RCollectionSearcher::SearchCollections() will
						// look for backdrops with a matching minor category.
						:	theMajorCategory( (uWORD)(aCategory - (aCategory % 100)) ),
							theMinorCategory( aCategory ),
							theGraphicType( stAnyType )
						{
						}

	const RGraphicCategorySearchCriteria& operator=( const RGraphicCategorySearchCriteria& rhs )
						{
							theMajorCategory = rhs.theMajorCategory;
							theMinorCategory = rhs.theMinorCategory;
							theGraphicType = rhs.theGraphicType;
							return *this;
						}
};


class RGraphicCategorySearchCollection : public RGraphicSearchCollection
{
public:
						RGraphicCategorySearchCollection( RChunkStorage *aChunkStorage );
						// construct an RGraphicCategorySearchCollection from aChunkStorage

	virtual void	SetSearchCriteria( const RSearchCriteria* aSearchCriteria );
						// Initializes theGraphicCategorySearchCriteria used by
						// MatchSearchCriteria() to perform the appropriate filtering
						// operations.

	virtual BOOLEAN MatchSearchCriteria();
						// compare the current index entry to theGraphicCategorySearchCriteria
						// set by SetSearchCriteria()
						// return TRUE if equal, else FALSE

protected:
	RGraphicCategorySearchCriteria	theGraphicCategorySearchCriteria;
};


#endif	// _GraphicCategorySearch_H_
