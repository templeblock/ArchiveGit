//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/COLLSRCH.H 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/COLLSRCH.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 4     1/28/99 2:07p Gbeddow
// support for NOT displaying "web art" in the Art Gallery and NOT
// displaying "art" in the Web Art Gallery (yet still displaying both in
// the
// stand-alone ClickArt Gallery browser); collection building support for
// matching the order of graphic types used in current collection building
// spreadsheets
// 
// 3     1/05/99 5:31p Gbeddow
// art gallery (graphic browser) support for Print Shop-style graphic type
// filtering
// 
// 2     10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 1     9/23/98 10:44a Jayn
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __COLLSRCH_H__
#define __COLLSRCH_H__

//#include "pmwcoll.h"

//
// A result from a search (there may be many of them).
//

class CPMWCollectionResult
{
public:
	CPMWCollectionResult()
		{ m_bitsCollectionID = 0; m_bitsItemNumber = 0; }

	// Get operations
	void SetCollectionID(USHORT usID)
		{ m_bitsCollectionID = usID; }
	void SetItemNumber(DWORD dwItemNumber)
		{ m_bitsItemNumber = dwItemNumber; }

   // Set operations
	USHORT         GetCollectionID() const
		{ return (USHORT) m_bitsCollectionID; }
	DWORD          GetItemNumber() const
		{ return m_bitsItemNumber; }

protected:
	DWORD m_bitsItemNumber   : 22;   // Lower 22 is Item Number
	DWORD m_bitsCollectionID : 10;   // Upper 10 bits is ID
};

typedef CArray<CPMWCollectionResult, CPMWCollectionResult&> CPMWCollectionResultArray;

//
// Ranking info returned by a search.
//

class CPMWRankingInfo
{
public:
	CPMWRankingInfo()
		{ Empty(); }

	// Get Operations
	USHORT GetStartIndex()
		{ return m_shStartIndex; }
	USHORT GetCount()
		{  return m_shCount; }

	// Set Operations
	void SetStartIndex(USHORT shIndex)
		{ m_shStartIndex = shIndex; }
	void SetCount(USHORT shCount)
		{  m_shCount = shCount; }
	void IncrementCount()
		{  m_shCount++; }
	void Empty(void)
		{  m_shCount = m_shStartIndex = 0; }
protected:
	USHORT    m_shCount;          // # of items with this ranking
	USHORT    m_shStartIndex;     // starting position in result array
};

typedef CArray<CPMWRankingInfo, CPMWRankingInfo&> CPMWRankingInfoArray;

/*
// The context for a search.
*/

class CPMWCollectionSearchContext
{
public:
	CPMWCollectionSearchContext();
	~CPMWCollectionSearchContext();

	long GetResultCount(void)
		{ return m_craSearchResults.GetSize(); }

	CPMWCollectionResult GetSearchResult(long lIndex) const
		{ return m_craSearchResults.GetAt(lIndex); }
	const CPMWCollectionResult* GetSearchResults(void) const
	   { return m_craSearchResults.GetData(); }

	// Array access.
	CPMWCollectionResultArray& GetSearchResultArray(void)
		{ return m_craSearchResults; }
	CPMWRankingInfoArray& GetRankingInfoArray(void)
		{ return m_riaRankingInfo; }

   // Find an item in the result set, returns -1 if failed
	long Find(USHORT nID, DWORD dwItemNumber);

public:
	int				 m_nType;		// What type of collection do we want?
	int				 m_nSubType;	// What subtype of collection do we want?

	// Search variables. These are passed to the collection Search() method.
	CPtrArray		m_ptrAryCollections;		// Which collections? (empty means all)
	CStringArray	m_strAryCategories;		// if more than 1, these are super-categories
	CString			m_strKeywords;
	BOOL				m_fKeywordsMatchAll;		// return "and" of keywords?
	int				m_nTone;
	int				m_nGraphicType;

protected:
	CPMWCollectionResultArray m_craSearchResults;
	CPMWRankingInfoArray m_riaRankingInfo;
};

#endif
