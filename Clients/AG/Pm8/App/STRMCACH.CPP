//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/STRMCACH.CPP 1     3/03/99 6:12p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/STRMCACH.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 2     9/23/98 10:28a Jayn
// Converted CollectionManager to COM-style object.
// 
// 1     9/22/98 4:03p Jayn
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "strmcach.h"
#include "pmwcoll.h"

CStreamCache::CStreamCache(ICollectionManager* pCollectionManager)
{
	m_pCollectionManager = pCollectionManager;
	m_pCollectionManager->AddRef();
}

CStreamCache::~CStreamCache()
{
	CloseCachedStreams();

	m_pCollectionManager->Release();
	m_pCollectionManager = NULL;
}

//
// Close all cached streams.
//

void CStreamCache::CloseCachedStreams(void)
{
	//
	// Iterate through and close the thumbnail streams.
	//

	POSITION Pos = m_mapThumbnailStreams.GetStartPosition();
	while (Pos != NULL)
	{
		WORD wID;
		LPVOID p;
		m_mapThumbnailStreams.GetNextAssoc(Pos, wID, p);
		// Find the collection with this ID.
		CPMWCollection* pCollection;
		if ((pCollection = m_pCollectionManager->FindCollection(wID)) != NULL)
		{
			pCollection->ReleaseThumbnailStream((CFileThumbnails*)p);
		}
		else
		{
			// Could not find the collection for an open stream.
			ASSERT(FALSE);
		}
	}
	m_mapThumbnailStreams.RemoveAll();

	//
	// Iterate through and close the item streams.
	//

	Pos = m_mapItemStreams.GetStartPosition();
	while (Pos != NULL)
	{
		WORD wID;
		LPVOID p;
		m_mapItemStreams.GetNextAssoc(Pos, wID, p);
		// Find the collection with this ID.
		CPMWCollection* pCollection;
		if ((pCollection = m_pCollectionManager->FindCollection(wID)) != NULL)
		{
			pCollection->ReleaseItemInfoStream((CFileItems*)p);
		}
		else
		{
			// Could not find the collection for an open item info stream.
			ASSERT(FALSE);
		}
	}
	m_mapItemStreams.RemoveAll();
}

//
// Get the thumbnail stream for a collection.
//

CFileThumbnails* CStreamCache::GetThumbnailStream(WORD wID)
{
	// Try to find the stream in the cache.
	LPVOID p;
	if (m_mapThumbnailStreams.Lookup(wID, p))
	{
		return (CFileThumbnails*)p;
	}

	// The stream does not exist. Create it now.
	CFileThumbnails* pStream = NULL;
   CPMWCollection* pCollection;
	if ((pCollection = m_pCollectionManager->FindCollection(wID)) != NULL)
	{
		// Create a new thumbnail stream.
		if (pCollection->NewThumbnailStream(&pStream) == ERRORCODE_None)
		{
			// Created. Add it to the map.
			TRY
			{
				m_mapThumbnailStreams.SetAt(wID, pStream);
			}
			CATCH_ALL(e)
			{
				ASSERT(FALSE);
				// SetAt failed. Should we delete the stream or leave
				// it dangling?
				// For now, delete it.
				pCollection->ReleaseThumbnailStream(pStream);
				pStream = NULL;
			}
			END_CATCH_ALL
		}
	}
	return pStream;
}

//
// Get the item info stream for a collection.
//

CFileItems* CStreamCache::GetItemStream(WORD wID)
{
	// Try to find the stream in the cache.
	LPVOID p;
	if (m_mapItemStreams.Lookup(wID, p))
	{
		return (CFileItems*)p;
	}

	// The stream does not exist. Create it now.
	CFileItems* pStream = NULL;
   CPMWCollection* pCollection;
	if ((pCollection = m_pCollectionManager->FindCollection(wID)) != NULL)
	{
		// Create a new item info stream.
		if (pCollection->NewItemInfoStream(&pStream) == ERRORCODE_None)
		{
			// Created. Add it to the map.
			TRY
			{
				m_mapItemStreams.SetAt(wID, pStream);
			}
			CATCH_ALL(e)
			{
				ASSERT(FALSE);
				// SetAt failed. Should we delete the stream or leave
				// it dangling?
				// For now, delete it.
				pCollection->ReleaseItemInfoStream(pStream);
				pStream = NULL;
			}
			END_CATCH_ALL
		}
	}
	return pStream;
}
