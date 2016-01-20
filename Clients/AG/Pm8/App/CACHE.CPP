/*
// $Workfile: CACHE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CACHE.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:18   Fred
// Initial revision.
// 
//    Rev 1.3   12 May 1997 11:07:26   Fred
// Fixed pointer bug, added validation to debug build
// 
//    Rev 1.2   17 Apr 1997 15:35:18   Fred
// Rotated fill patterns
// 
//    Rev 1.1   11 Apr 1997 13:56:50   Fred
// Font Caching (bitmaps, rotated, warped)
// 
//    Rev 1.0   09 Apr 1997 14:50:42   Fred
// Start of new font caching (fonts and metrics)
*/

#include "stdafx.h"
#include "cache.h"


///////////////////////////////////////////////////////////////////
// CItemCache
// CItemCache::Item
// CItemCache::Item::Key
//
//		These classes comprise a simple abstract caching object
//		based on a hash table. Override these classes to create
//		a cache for a particular type of client object.
//
//		The cache object represents the entire cache of items.
//
//		The item object represents a single item in the cache. Each
//		item object has a virtual function that returns a pointer
//		to the key for that item.
//
//		The key object represents a unique identifier for an item
//		within the cache. No two items in the cache should have
//		the same key.
//
// Note: These object implement a general caching scheme. As a
// generic object, it may be less than optimal for caching certain
// types of information. After some analysis, it seems likely that
// the overhead will probably be overwhelmed by whatever process
// is using the data retrieved from the cache. This may or may not
// be true four *your* application. So beware, if you have very
// time sensitive code, you might be better off writing your own
// caching mechanism.


///////////////////////////////////////////////////////////////////
// CItemCache::Item::Key

// Constructor
CItemCache::Item::Key::Key()
{
	m_fHashValid = FALSE;
}

// Destructor
CItemCache::Item::Key::~Key()
{
}


///////////////////////////////////////////////////////////////////
// CItemCache::Item

// Constructor
CItemCache::Item::Item()
{
	m_pHashPrevious = NULL;
	m_pHashNext = NULL;
	m_nHashIndex = -1;
}

// Destructor
CItemCache::Item::~Item()
{
	ASSERT(m_pHashPrevious == NULL);
	ASSERT(m_pHashNext == NULL);
	ASSERT(m_nHashIndex == -1);
}

// This is a utility function which quickly hashes an array of bytes.
// This is useful for some derived classes.
DWORD CItemCache::Item::SumBytes(void* pBytes, int nCount)
{
	DWORD dwSum = 0;

	unsigned char* pByte = (unsigned char*)pBytes;
	while (nCount-- > 0)
	{
		dwSum += (DWORD)(*(pByte++))+(nCount<<4);
	}

	return dwSum;
}

///////////////////////////////////////////////////////////////////
// CItemCache

// Constructor
CItemCache::CItemCache(int nHashTableSize)
{
	ASSERT(nHashTableSize > 0);

	m_pLastItemFound = NULL;
	m_nHashTableSize = nHashTableSize;
	m_pHashTable = NULL;

	// Allocate the hash table.
	TRY
	{
		m_pHashTable = new Item* [m_nHashTableSize];
		memset(m_pHashTable, 0, m_nHashTableSize*sizeof(Item*));
	}
	END_TRY
	ASSERT(m_pHashTable != NULL);

}

// Destructor
CItemCache::~CItemCache()
{
	ASSERT(Validate());

	// Delete all the items.
	DeleteAll();

	// Delete the hash table itself.
	delete [] m_pHashTable;
	m_pHashTable = NULL;
}

// Lookup the item with the given key.
CItemCache::Item* CItemCache::Lookup(Item::Key* pKey)
{
	ASSERT(Validate());

	ASSERT(pKey != NULL);
	if (pKey != NULL)
	{
		// Check the last item we found. If it's a match, just return it.
		if (m_pLastItemFound != NULL)
		{
			if (m_pLastItemFound->GetKey()->Compare(pKey))
			{
				// Silly user, they want the same thing again.
				return m_pLastItemFound;
			}

			// We wasted some time!
			m_pLastItemFound = NULL;
		}

		// Compute the hash index.
		int nHashIndex = (int)(pKey->GetHash() % m_nHashTableSize);

		// Search the hash chain for this hash bucket.
		Item* pItem = m_pHashTable[nHashIndex];
		while (pItem != NULL)
		{
			if (pItem->GetKey()->Compare(pKey))
			{
				// Found it, save a pointer to the last item found so we can check for it
				// quickly the next time we do a lookup.

				// PMGTODO: Verify that this mechanism is helpful. What kinds of caches
				// repeatedly lookup the same item? Perhaps the code using the item should
				// cache the pointer.

				m_pLastItemFound = pItem;

				// Return a pointer ot the item we found.
				return pItem;
			}

			pItem = pItem->m_pHashNext;
		}
	}

	// Not found.
	return NULL;
}

// Insert a new item into the cache.
BOOL CItemCache::Insert(Item* pItem)
{
	ASSERT(Validate());

	ASSERT(pItem != NULL);
	if (pItem != NULL)
	{
		// BIG ASSUMPTION: The item does not already exist.
		// If this is not true, the cache will be screwed up.

		// Compute the hash index.
		int nHashIndex = (int)(pItem->GetKey()->GetHash() % m_nHashTableSize);

#ifdef _DEBUG
		Item* pHashItem = m_pHashTable[nHashIndex];
		while (pHashItem != NULL)
		{
			if (pHashItem->GetKey()->Compare(pItem->GetKey()))
			{
				// Yikes! Item already exists!
				ASSERT(FALSE);
				return FALSE;
			}
			pHashItem = pHashItem->m_pHashNext;
		}
#endif

		// Insert the item at the start of the chain for this hash bucket.
		pItem->m_pHashPrevious = NULL;
		pItem->m_pHashNext = m_pHashTable[nHashIndex];
		if (m_pHashTable[nHashIndex] != NULL)
		{
			m_pHashTable[nHashIndex]->m_pHashPrevious = pItem;
		}
		m_pHashTable[nHashIndex] = pItem;
		pItem->m_nHashIndex = nHashIndex;

		ASSERT(Validate());

		return TRUE;
	}

	return FALSE;
}

// Delete an item from the cache
void CItemCache::DeleteItem(Item* pItem)
{
	ASSERT(Validate());

	ASSERT(pItem != NULL);
	if (pItem != NULL)
	{
		ASSERT(pItem->m_nHashIndex != -1);

		// If we're deleting the item to which we have a cachec pointer,
		// then invalidate that pointer.
		if (pItem == m_pLastItemFound)
		{
			m_pLastItemFound = NULL;
		}

		// Unlink from previous item in hash list.
		if (pItem->m_pHashPrevious != NULL)
		{
			pItem->m_pHashPrevious->m_pHashNext = pItem->m_pHashNext;
		}
		else
		{
			m_pHashTable[pItem->m_nHashIndex] = pItem->m_pHashNext;
		}

		// Unlink from next item in hash list.
		if (pItem->m_pHashNext != NULL)
		{
			pItem->m_pHashNext->m_pHashPrevious = pItem->m_pHashPrevious;
		}

		pItem->m_nHashIndex = -1;
		pItem->m_pHashPrevious = NULL;
		pItem->m_pHashNext = NULL;

		// Delete the item.
		delete pItem;
	}

	ASSERT(Validate());
}

// Delete all items from the cache.
void CItemCache::DeleteAll()
{
	ASSERT(Validate());

	if (m_pHashTable != NULL)
	{
		// Delete all of the items in the cache.
		for (int nHashIndex = 0; nHashIndex < m_nHashTableSize; nHashIndex++)
		{
			while (m_pHashTable[nHashIndex] != NULL)
			{
				DeleteItem(m_pHashTable[nHashIndex]);
			}
		}
	}

	ASSERT(Validate());
}

#ifdef _DEBUG
BOOL CItemCache::Validate()
{
	if (m_pHashTable != NULL)
	{
		CMapPtrToPtr Map;

		// Validate all of the items in the cache.
		for (int nHashIndex = 0; nHashIndex < m_nHashTableSize; nHashIndex++)
		{
			Item* pPreviousItem = NULL;
			Item* pItem = m_pHashTable[nHashIndex];
			while (pItem != NULL)
			{
				if ((pItem->m_pHashPrevious == (Item*)0xddddddddL)
				 || (pItem->m_pHashNext == (Item*)0xddddddddL))
				{
					// Item has been deleted, but is still in hash table!
					ASSERT(FALSE);
					return FALSE;
				}

				if (pItem->m_nHashIndex != nHashIndex)
				{
					// Item hash index is wrong!
					ASSERT(FALSE);
					return FALSE;
				}

				void* p;
				if (Map.Lookup(pItem, p))
				{
					// Item is in hash table more than once!
					ASSERT(FALSE);
					return FALSE;
				}

				Map.SetAt(pItem, (void*)(long)nHashIndex);

				if (pItem->m_pHashPrevious != pPreviousItem)
				{
					// Previous pointer is wrong!
					ASSERT(FALSE);
					return FALSE;
				}

				pPreviousItem = pItem;
				pItem = pItem->m_pHashNext;
			}
		}

		// Validate cached pointer to last item found.
		if (m_pLastItemFound != NULL)
		{
			void* p;
			if (!Map.Lookup(m_pLastItemFound, p))
			{
				ASSERT(FALSE);
				return FALSE;
			}
		}
	}

	return TRUE;
}
#endif
