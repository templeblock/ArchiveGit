// ****************************************************************************
//
//  File Name:			SearchResult.h
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of SearchResult class
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1997 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef	_SEARCHRESULT_H_
#define	_SEARCHRESULT_H_

#include "CollectionBuilderTypes.h"
#include "GraphicInterface.h"

class IComponentInfo ;
class RComponentDocument;
class RComponentView;
class RDocument;

/*
		GRAF chunk header information
		=============================
		- graphic ID (4  bytes)
		- graphic type (square, row, column, rect, image, or imageRef) (2 bytes)
		- minor category ID (2 bytes, unsigned )
		- graphic name offset into NAMS chunk(4 bytes)
		- keyword offset into GKWC chunk(4 bytes)
		- monochrome flag (1 byte)
		- background type (1 byte)
		- fittable subtype (1 byte)
		- thumbnail type (1 byte)
		- graphic data size (4 bytes)
		- thumbnail size (4 bytes)
*/

#pragma pack(push, 1)
struct GRAF_Header
{
	DWORD				graphicID;
	short				graphicType;
	unsigned short		minorID;
	DWORD				NAMS_offset;
	DWORD				GKWC_offset;
	uBYTE				bMonochrome;
	uBYTE				bkType;
	uBYTE				fittableSubtype;
	uBYTE				thumbnailType;
	DWORD				grDataSize;
	DWORD				thumbnailSize;
};
#pragma pack(pop)


// ****************************************************************************
//
//  Class Name:	SearchResult
//
//  Description:	Entry in the RSearchResultArray produced by
//						RCollectionSearcher::SearchCollections()
//
// ****************************************************************************
//
class SearchResult
{
public :
	RChunkStorage*		collectionStorage;	// collection file that contains this entry
	YStreamLength		dataOffset;				// absolute offset to "data" in collectionStorage
														// (or category id when searching for category names/ids)
	BYTE*					namePtr;					// pointer to "name" in memory
	uLONG					id;						// graphic id, quote & verse id,
														// category id or readymade kit member project type

// Interface
public :
	RComponentDocument*						GetComponent( IComponentInfo* pInfo, BOOLEAN fPreview, int nSubType = 0, const RRealSize& backdropSize = RRealSize( 0, 0 ) ) const;

	static int									SearchResult::CompareString( 
														LCID Locale, 
														DWORD dwCmpFlags, 
														LPCTSTR lpString1, 
														int cchCount1, 
														LPCTSTR lpString2, 
														int cchCount2 ); 

// Implementation
private :
	class RShapeToComponentMapEntry
		{
		// Construction
		public :
													RShapeToComponentMapEntry( cbSHAPETYPES shape, int nSubType, const YComponentType& component );

		// Members
		public :
			cbSHAPETYPES						m_ShapeType;
			int									m_nSubType;
			YComponentType						m_ComponentType;
		};

	static RShapeToComponentMapEntry		m_ShapeToComponentMap[ ];

	const YComponentType&					ComponentTypeFromShapeType( cbSHAPETYPES shape, int nSubType ) const;
	void											SetTileAttributes( RComponentView* pComponent ) const ;
	RGraphicInterface::EGraphicType		GetGraphicType( cbSHAPETYPES shape, uBYTE backdropType ) const;
};


typedef	RArray<SearchResult>						RSearchResultArray;
typedef	RArray<SearchResult>::YIterator		RSearchResultIterator;


inline RSearchResultIterator FindSearchResultArrayID(
	RSearchResultArray* aSearchResultArray, uLONG anID, uLONG& foundPosition )
{
	RSearchResultIterator resultIter( aSearchResultArray->Start() );
	RSearchResultIterator resultIterEnd( aSearchResultArray->End() );
	for (foundPosition = 0; resultIter != resultIterEnd && (*resultIter).id != anID;
		resultIter++, foundPosition++)
		;
	return resultIter;
}


#endif	// _SEARCHRESULT_H_
