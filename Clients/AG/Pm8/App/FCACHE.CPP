/*
// $Workfile: FCACHE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
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
// $Log: /PM8/App/FCACHE.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:22   Fred
// Initial revision.
// 
//    Rev 1.6   12 May 1997 11:06:16   Fred
//  
// 
//    Rev 1.5   22 Apr 1997 13:16:56   Fred
// Start of project and art browsers
// 
//    Rev 1.4   11 Apr 1997 13:56:50   Fred
// Font Caching (bitmaps, rotated, warped)
// 
//    Rev 1.3   09 Apr 1997 14:50:44   Fred
// Start of new font caching (fonts and metrics)
//
//    Rev 1.2   03 Apr 1997 09:30:54   Fred
// More text cleanup
// 
//    Rev 1.1   06 Jan 1997 15:45:44   Jay
// Some optimizations
// 
//    Rev 1.0   14 Mar 1996 13:47:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:08   FRED
// Initial revision.
// 
//    Rev 2.4   19 Jan 1996 09:21:36   JAY
// More fixes for based() code.
// 
//    Rev 2.3   20 Dec 1995 09:54:30   JAY
// Removed overloaded new and delete. Bug!
// 
//    Rev 2.2   31 Aug 1995 09:08:10   JAY
// Handles underflow of cache size (just in case).
// 
//    Rev 2.1   31 Aug 1995 08:55:36   JAY
// Fixed a problem with current_cache_size not being updated correctly.
// Also added a fail-safe to ensure cache size is 0 if cache becomes empty.
// 
//    Rev 2.0   07 Feb 1995 15:47:42   JAY
// Initial revision.
// 
//    Rev 1.5   30 Nov 1994 16:37:56   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.4   04 Aug 1994 07:34:00   JAY
// Large model and 32-bit changes
// 
//    Rev 1.3   22 Feb 1994 16:01:48   MACDUFF
// Eliminated warnings caused by previous change.
// 
//    Rev 1.2   22 Feb 1994 15:57:10   MACDUFF
// Sped up font cache as follows:
// 1) Put all items of a cache in the same segment.
// 2) Used based pointers for the 'next' and 'previous' members of
// the DoubleLinkedListItem.
// 3) Use assembly code to eliminate unneeded segment register loads.
// 4) Do not hash on character codes.  This enables us to eliminate
// virtual methods in the DoubleLinkedListItem.  It turns out that
// under Windows the overhead of the VMT is greater than the savings
// gained from hashing.
// 
//    Rev 1.1   30 Dec 1993 16:10:30   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:25:32   JAY
//  
// 
//    Rev 1.8   24 Mar 1993 13:51:26   MACDUFF
// Removed MacduffOptions and associated debug code
// 
//    Rev 1.7   04 Mar 1993 11:38:04   MACDUFF
// Made hashfunc() inline.
// 
//    Rev 1.6   03 Mar 1993 17:25:40   MACDUFF
// Added tFontCacheItem::ForceFindChar
// MemAlloc will not delete the last font item in the cache.
// 
//    Rev 1.5   26 Feb 1993 14:30:02   MACDUFF
// Full implementation of font caching
// 
//    Rev 1.4   25 Feb 1993 11:25:00   MACDUFF
// Added debugging code, #ifdef'd out.
// Temporarily disabled hashing.
// 
//    Rev 1.3   24 Feb 1993 14:13:34   MACDUFF
// Fixed some errors.
// 
//    Rev 1.2   24 Feb 1993 10:16:00   MACDUFF
// fcache.h included inside of "C" block
// 
//    Rev 1.1   23 Feb 1993 16:10:32   MACDUFF
// Fixed typo
// 
//    Rev 1.0   23 Feb 1993 14:46:34   MACDUFF
// Initial revision.
*/

#include "stdafx.h"
#include "text.h"
#include "fcache.h"


///////////////////////////////////////////////////////////////////
// CCharacterBitmapCache::CharacterBitmapCacheItem

// Constructor
CCharacterBitmapCache::CharacterBitmapCacheItem::CharacterBitmapCacheItem(CHARACTER Character, DWORD dwComponent, CharacterBitmap& Bitmap) :
	m_Key(Character, dwComponent), m_Bitmap(Bitmap)
{
	m_fSizeValid = FALSE;

	m_pCharacterBitmapCache = NULL;

	m_pAccessPrevious = NULL;
	m_pAccessNext = NULL;
}

// Destructor
CCharacterBitmapCache::CharacterBitmapCacheItem::~CharacterBitmapCacheItem()
{
	// Make sure we've been unlinked.
	ASSERT(m_pAccessPrevious == NULL);
	ASSERT(m_pAccessNext == NULL);

	// Delete out map memory.
	delete [] (char*)(m_Bitmap.m_Map.map_memory);
}


///////////////////////////////////////////////////////////////////
// CCharacterBitmapCache

// Constructor
CCharacterBitmapCache::CCharacterBitmapCache(CFontCache* pFontCache) :
	CItemCache(127)
{
	ASSERT(pFontCache != NULL);
	m_pFontCache = pFontCache;
}

// Destructor
CCharacterBitmapCache::~CCharacterBitmapCache()
{
	// We have to override the destructor since the CItemCache
	// destructor will not call our DeleteItem().

	// Delete all the items.
	DeleteAll();
}

// Lookup the item with the given key.
CItemCache::Item* CCharacterBitmapCache::Lookup(Item::Key* pKey)
{
	// Lookup the key in the normal cache.
	CharacterBitmapCacheItem* pItem = (CharacterBitmapCacheItem*)(CItemCache::Lookup(pKey));
	if (pItem != NULL)
	{
		// Tell the font cache we accessed a character bitmap.
		m_pFontCache->AccessBitmap(pItem);

		// Return a pointer to the item which was found.
		return pItem;
	}

	// Not found.
	return NULL;
}

// Insert a new item into the cache.
BOOL CCharacterBitmapCache::Insert(Item* pItem)
{
	CharacterBitmapCacheItem* pBitmapItem = (CharacterBitmapCacheItem*)pItem;

	ASSERT(pBitmapItem != NULL);
	if (pBitmapItem != NULL)
	{
		ASSERT(pBitmapItem->m_pCharacterBitmapCache == NULL);

		// Ask the font cache for some room.
		if (m_pFontCache->SizeBitmap(pBitmapItem))
		{
			// Attempt to insert the item.
			if (CItemCache::Insert(pBitmapItem))
			{
				// Tell the item which cache it belongs to.
				pBitmapItem->m_pCharacterBitmapCache = this;

				// Tell the font cache we inserted a new bitmap.
				m_pFontCache->InsertBitmap(pBitmapItem);
				return TRUE;
			}
		}
	}

	return FALSE;
}

// Delete an item from the cache
void CCharacterBitmapCache::DeleteItem(Item* pItem)
{
	CharacterBitmapCacheItem* pBitmapItem = (CharacterBitmapCacheItem*)pItem;

	ASSERT(pBitmapItem != NULL);
	if (pBitmapItem != NULL)
	{
		// Tell the font cache we are going to delete the character bitmap.
		m_pFontCache->DeleteBitmap(pBitmapItem);

		// Break link between item and cache.
		pBitmapItem->m_pCharacterBitmapCache = NULL;

		// Delete the item.
		CItemCache::DeleteItem(pBitmapItem);
	}
}


///////////////////////////////////////////////////////////////////
// CFontCache

// Constructor
CFontCache::CFontCache(DWORD dwMaximumCacheSize, DWORD dwMaximumCacheItemSize) :
	CItemCache(301)
{
	m_dwSize = 0;
	m_dwMaximumCacheSize = dwMaximumCacheSize;
	m_dwMaximumCacheItemSize = dwMaximumCacheItemSize;
	m_pAccessMostRecent = NULL;
	m_pAccessLeastRecent = NULL;
}

// This method is called to make sure there is enough room to insert
// the specified bitmap into the cache. If there is not enough room,
// the least recently used bitmaps are deleted.
BOOL CFontCache::SizeBitmap(CCharacterBitmapCache::CharacterBitmapCacheItem* pBitmapItem)
{
	// If our maximum cache size is zero, then we don't care.
	if (m_dwMaximumCacheSize == 0)
	{
		// Sure, go ahead, we don't care!
		return TRUE;
	}

	if (pBitmapItem != NULL)
	{
		// Get the size of the bitmap item.
		DWORD dwItemSize = pBitmapItem->GetSize();

		// Make sure the bitmap item size is smaller than the maximum item size.
		if (dwItemSize > m_dwMaximumCacheItemSize)
		{
			// Item is too big.
			return FALSE;
		}

		// Get the amount of available free space.
		DWORD dwFreeSize = GetFreeSize();

		// Check if we already have enough free space.
		if (dwItemSize < dwFreeSize)
		{
			// The cache already has the space.
			return TRUE;
		}

		// Check if we have any hope of getting enough free space. If the size
		// needed is greater than the maximum cache size, then we just return
		// FALSE. Basically, the item can't be cached and we don't want to throw
		// out the existing cache contents just to find that out when the cache
		// is empty.
		ASSERT(dwItemSize <= m_dwMaximumCacheSize);
		if (dwItemSize > m_dwMaximumCacheSize)
		{
			return FALSE;
		}

		// Delete the least recently used bitmaps until we have enough memory.
		// Note that we should be guaranteed to get enough memory since our
		// request is less than the maximum cache size and all allocated
		// bitmaps are account for in the size.
		while (dwItemSize > dwFreeSize)
		{
#ifdef _DEBUG
			if (m_pAccessLeastRecent == NULL)
			{
				// Nothing more to free. Sizes must be out of whack.
				ASSERT(FALSE);
				return FALSE;
			}
#endif

			// Free the least recently used bitmap.
			m_pAccessLeastRecent->m_pCharacterBitmapCache->DeleteItem(m_pAccessLeastRecent);
//TRACE("Throw LRU bitmap out of font cache.\n");

			// Get the new free size.
			dwFreeSize = GetFreeSize();
		}
	}

	// We found enough free space, return TRUE.
	return TRUE;
}

// This method is called when a bitmap item is accessed. It moves the bitmap
// item to the front (most recently used) of the access list.
void CFontCache::AccessBitmap(CCharacterBitmapCache::CharacterBitmapCacheItem* pBitmapItem)
{
	// Unlink the bitmap from access list.
	Unlink(pBitmapItem);

	// Move the bitmap item to the front of the access list.
	InsertAtFront(pBitmapItem);
}

// This method is called when a bitmap item is inserted. It adds the bitmap size
// to the current cache size and moves the bitmap item to the front (most
// recently used) of the access list. This method ASSUMES that room has been
// made for the bitmap item.
void CFontCache::InsertBitmap(CCharacterBitmapCache::CharacterBitmapCacheItem* pBitmapItem)
{
	ASSERT(pBitmapItem != NULL);

	// Insert the bitmap at the start of the access list.
	InsertAtFront(pBitmapItem);

	// Add the bitmap item's size to our current size.
	m_dwSize += pBitmapItem->GetSize();
}

// This method is called when a bitmap item is deleted. It subtracts the bitmap
// size to the current cache size and removes the bitmap item from the access list.
void CFontCache::DeleteBitmap(CCharacterBitmapCache::CharacterBitmapCacheItem* pBitmapItem)
{
	ASSERT(pBitmapItem != NULL);

	// Subtract the bitmap item's size to our current size.
	m_dwSize -= pBitmapItem->GetSize();

	// Unlink the bitmap from the access list.
	Unlink(pBitmapItem);
}

// Unlink a bitmap item from the access list.
void CFontCache::Unlink(CCharacterBitmapCache::CharacterBitmapCacheItem* pBitmapItem)
{
	if (pBitmapItem != NULL)
	{
		// Unlink from previous item in access list.
		if (pBitmapItem->m_pAccessPrevious != NULL)
		{
			pBitmapItem->m_pAccessPrevious->m_pAccessNext = pBitmapItem->m_pAccessNext;
		}
		else
		{
			m_pAccessMostRecent = pBitmapItem->m_pAccessNext;
		}

		// Unlink from next item in access list.
		if (pBitmapItem->m_pAccessNext != NULL)
		{
			pBitmapItem->m_pAccessNext->m_pAccessPrevious = pBitmapItem->m_pAccessPrevious;
		}
		else
		{
			m_pAccessLeastRecent = pBitmapItem->m_pAccessPrevious;
		}

		pBitmapItem->m_pAccessPrevious = NULL;
		pBitmapItem->m_pAccessNext = NULL;
	}
}

// Insert a bitmap item into the front (most recently used) of the access list.
void CFontCache::InsertAtFront(CCharacterBitmapCache::CharacterBitmapCacheItem* pBitmapItem)
{
	if (pBitmapItem != NULL)
	{
		// Insert item at start of access list (it's the most recently used.)
		pBitmapItem->m_pAccessPrevious = NULL;
		pBitmapItem->m_pAccessNext = m_pAccessMostRecent;
		if (m_pAccessMostRecent == NULL)
		{
			// Access list is empty.
			ASSERT(m_pAccessLeastRecent == NULL);
			m_pAccessMostRecent = pBitmapItem;
			m_pAccessLeastRecent = pBitmapItem;
		}
		else
		{
			// Access list is not empty.
			ASSERT(m_pAccessMostRecent->m_pAccessPrevious == NULL);
			m_pAccessMostRecent->m_pAccessPrevious = pBitmapItem;
			m_pAccessMostRecent = pBitmapItem;
		}
	}
}
