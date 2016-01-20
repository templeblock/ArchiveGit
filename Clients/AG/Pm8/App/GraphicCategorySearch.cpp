/*	$Header: /PM8/App/GraphicCategorySearch.cpp 1     3/03/99 6:05p Gbeddow $
//
//	Definition of the CPTimePieceDlg class.
//
//	Definition of class that performs a category search on
// a graphics collection file, suitable for use as the
// parameterized type of an RCollectionSearcher
//	
// Author:				Greg Beddow
//
// Portability:		Cross platform
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
// Copyright (C) 1997 Broderbund Software, Inc. All Rights Reserved.
//
// $Log: /PM8/App/GraphicCategorySearch.cpp $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 1     2/01/99 5:41p Rgrenfel
// Graphic search support classes.
*/

#include "StdAfx.h"	// standard Renaissance includes

ASSERTNAME

#include "GraphicCategorySearch.h"				// header for this class


/****************************************************************************
	RGraphicCategorySearchCollection::RGraphicCategorySearchCollection( RChunkStorage *aChunkStorage )

	Construct an RGraphicCategorySearchCollection from aChunkStorage
*****************************************************************************/

RGraphicCategorySearchCollection::RGraphicCategorySearchCollection( RChunkStorage *aChunkStorage )
	:	RGraphicSearchCollection( aChunkStorage )
{
}

/****************************************************************************
	void RGraphicCategorySearchCollection::SetSearchCriteria(
		const RSearchCriteria* aSearchCriteria)

	Set theGraphicCategorySearchCriteria used by MatchSearchCriteria()
*****************************************************************************/

void RGraphicCategorySearchCollection::SetSearchCriteria(
	const RSearchCriteria* aSearchCriteria )
{
	RSearchCriteria* aNonConstSearchCriteria =
		const_cast<RSearchCriteria*>(aSearchCriteria);
	RGraphicCategorySearchCriteria* aGraphicCategorySearchCriteria =
		dynamic_cast<RGraphicCategorySearchCriteria*>(aNonConstSearchCriteria);

	if (aGraphicCategorySearchCriteria == NULL)
	{
		TpsAssertAlways( "A valid RGraphicCategorySearchCriteria object is required." );
	}
	else
	{
		theGraphicCategorySearchCriteria = *aGraphicCategorySearchCriteria;
	}
}

/****************************************************************************
	BOOLEAN RGraphicCategorySearchCollection::MatchSearchCriteria()

	Compare the current index entry to theGraphicCategorySearchCriteria set by
	SetSearchCriteria().
	Return TRUE if equal, else FALSE.
*****************************************************************************/

BOOLEAN RGraphicCategorySearchCollection::MatchSearchCriteria()
{
	// test for match on graphic ("shape") type

	if ( theGraphicCategorySearchCriteria.theGraphicType != stAnyType &&
		theGraphicCategorySearchCriteria.theGraphicType != theGraphicType)
		return FALSE;

	// test for match on category

	// if the search criteria category is "All"...
	if (theGraphicCategorySearchCriteria.theMajorCategory == 0)
		return TRUE;
	else
	{
		// if this is a minor category search...
		if (theGraphicCategorySearchCriteria.theMajorCategory !=
			theGraphicCategorySearchCriteria.theMinorCategory)
		{
			// if we have a matching minor category...
			if (theGraphicCategorySearchCriteria.theMinorCategory == thePrimaryMinorCategory ||
				theGraphicCategorySearchCriteria.theMinorCategory == theSecondaryMinorCategory)
				return TRUE;
		}
		else	// it's a major category search...
		{
			// if we have a matching major category...
			int currentPrimaryMajorCategory =
				thePrimaryMinorCategory - (thePrimaryMinorCategory % 100);
			int currentSecondaryMajorCategory =
				theSecondaryMinorCategory - (theSecondaryMinorCategory % 100);
			if (theGraphicCategorySearchCriteria.theMajorCategory == currentPrimaryMajorCategory ||
				theGraphicCategorySearchCriteria.theMajorCategory == currentSecondaryMajorCategory)
				return TRUE;
		}
	}
	return FALSE;
}

