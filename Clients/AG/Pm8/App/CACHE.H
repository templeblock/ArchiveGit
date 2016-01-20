/*
// $Workfile: CACHE.H $
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
// $Log: /PM8/App/CACHE.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:48   Fred
// Initial revision.
// 
//    Rev 1.2   12 May 1997 11:07:26   Fred
// Fixed pointer bug, added validation to debug build
// 
//    Rev 1.1   11 Apr 1997 13:57:20   Fred
// Font Caching (bitmaps, rotated, warped)
// 
//    Rev 1.0   09 Apr 1997 14:51:48   Fred
// Start of new font caching (fonts and metrics)
*/

#ifndef __CACHE_H__
#define __CACHE_H__

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
// time sensitive code, you might be better off writeing your own
// caching mechanism.

class CItemCache
{

public:

	// Class definition for item in cache. This is an abstract defintion,
	// the real item class must be derived from this.
	class Item
	{

	public:

		// Class definition for item key.
		class Key
		{

		public:

			// Constructor
			Key();

			// Destructor
			virtual ~Key();

		public:

			// Return the hashed value of the key.
			DWORD GetHash(void)
			{
				if (!m_fHashValid)
				{
					ComputeHash();
					m_fHashValid = TRUE;
				}

				return m_dwHash;
			}

			// Compute the hashed value of the key. This function is
			// called once by Insert() before inserting the item.
			virtual void ComputeHash(void) = 0;

		protected:

			// TRUE if hash value was successfully computed.
			BOOL m_fHashValid;

			// Hashed value of item.
			DWORD m_dwHash;

		public:

			// Test if another key is equal to this key.
			virtual BOOL Compare(Key* pKey) = 0;
		};

	public:

		// Constructor
		Item();

		// Destructor
		virtual ~Item();

	public:

		// Pointer to previous item in hash list.
		Item* m_pHashPrevious;

		// Pointer to next item in hash list.
		Item* m_pHashNext;

		// Index of hash table list containing item.
		int m_nHashIndex;

		// Get pointer to key portion of this object.
		virtual Key* GetKey(void) = 0;

	public:

		// This is a utility function which quickly hashes an array of bytes.
		// This is useful for some derived classes.
		static DWORD SumBytes(void* pBytes, int nCount);
	};

public:

	// Constructor
	CItemCache(int nHashTableSize);

	// Destructor
	virtual ~CItemCache();

public:

	// Lookup the item with the given key.
	virtual Item* Lookup(Item::Key* pKey);

	// Insert a new item into the cache.
	virtual BOOL Insert(Item* pItem);

	// Delete an item from the cache
	virtual void DeleteItem(Item* pItem);

	// Delete all items from the cache.
	virtual void DeleteAll(void);

public:

	// Pointer to the last item found. This item is checked first with the hope
	// that a client will tend to make consecutive accesses to the same object.
	// This may or may not be true depending on the object.
	Item* m_pLastItemFound;

	// Hash table size.
	int m_nHashTableSize;

	// Pointer to hash table.
	Item** m_pHashTable;

#ifdef _DEBUG
public:
	BOOL Validate();
#endif
};

#endif
