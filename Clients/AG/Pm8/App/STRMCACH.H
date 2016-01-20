//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/STRMCACH.H 1     3/03/99 6:12p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/STRMCACH.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 2     9/23/98 10:28a Jayn
// Converted CollectionManager to COM-style object.
// 
// 1     9/22/98 4:03p Jayn
//////////////////////////////////////////////////////////////////////////////

#ifndef __STRMCACH_H__
#define __STRMCACH_H__

#include "icollmgr.h"

class CFileThumbnails;
class CFileItems;

//
// A little helper class to cache streams (an optimizing thing)
//

class CStreamCache
{
public:
	CStreamCache(ICollectionManager* pCollectionManager);
	virtual ~CStreamCache();

	// Close all cached streams.
	void CloseCachedStreams(void);

	// Get the thumbnail stream for a collection.
	CFileThumbnails* GetThumbnailStream(WORD wID);

	// Get the item info stream for a collection.
	CFileItems* GetItemStream(WORD wID);

protected:
	ICollectionManager* m_pCollectionManager;

	// Our map for remembering open thumbnail streams.
	CMapWordToPtr m_mapThumbnailStreams;

	// Our map for remembering open item streams.
	CMapWordToPtr m_mapItemStreams;
};

#endif
