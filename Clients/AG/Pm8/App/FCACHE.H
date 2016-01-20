/*
// $Workfile: FCACHE.H $
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
// $Log: /PM8/App/FCACHE.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
//    Rev 1.0   14 Aug 1997 15:20:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:46   Fred
// Initial revision.
// 
//    Rev 1.9   16 Jul 1997 09:44:10   Fred
//  
// 
//    Rev 1.8   15 May 1997 16:52:24   Fred
// Uses full effects structures
// 
//    Rev 1.7   22 Apr 1997 13:16:54   Fred
// Start of project and art browsers
// 
//    Rev 1.6   17 Apr 1997 16:30:50   Fred
// Fixed bug with rotated characters
// 
//    Rev 1.5   17 Apr 1997 15:35:18   Fred
// Rotated fill patterns
// 
//    Rev 1.4   11 Apr 1997 13:57:20   Fred
// Font Caching (bitmaps, rotated, warped)
// 
//    Rev 1.3   09 Apr 1997 14:50:44   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.2   03 Apr 1997 09:32:16   Fred
// More text cleanup
// 
//    Rev 1.1   06 Jan 1997 15:45:46   Jay
// Some optimizations
// 
//    Rev 1.0   14 Mar 1996 13:47:24   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:20   FRED
// Initial revision.
// 
//    Rev 2.2   20 Dec 1995 09:54:30   JAY
// Removed overloaded new and delete. Bug!
// 
//    Rev 2.1   31 Aug 1995 08:56:00   JAY
// Fixed a problem with current_cache_size not being updated correctly.
// Also added a fail-safe to ensure cache size is 0 if cache becomes empty.
// 
//    Rev 2.0   07 Feb 1995 15:48:30   JAY
// Initial revision.
// 
//    Rev 1.2   22 Feb 1994 15:57:08   MACDUFF
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
//    Rev 1.1   30 Dec 1993 16:11:08   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:26:18   JAY
//  
// 
//    Rev 1.3   03 Mar 1993 17:25:22   MACDUFF
// Added a few methods.
// 
//    Rev 1.2   26 Feb 1993 14:30:02   MACDUFF
// Full implementation of font caching
// 
//    Rev 1.1   23 Feb 1993 16:09:16   MACDUFF
// Now C files can compile me, too.
// 
//    Rev 1.0   23 Feb 1993 14:46:24   MACDUFF
// Initial revision.
*/

#ifndef __FCACHE_H__
#define __FCACHE_H__

#include	"dlist.h"
#include "cfixed.h"
#include "cache.h"

class CFontCache;


typedef struct
{
	SHORT width;
	SHORT height;
	void* map_memory;
} MAP, *MAPPTR;


///////////////////////////////////////////////////////////////////
// CCharacterMetricsCache
//
//		This object defines a cache for character metrics.

class CCharacterMetricsCache : public CItemCache
{

public:

	// Class definition for item in cache.
	class CharacterMetricsCacheItem : public CItemCache::Item
	{

	public:

		// Class definition for item key.
		class CharacterMetricsCacheKey : public CItemCache::Item::Key
		{

		public:

			// Constructor
			CharacterMetricsCacheKey(CHARACTER Character)
				{ m_Character = Character; }

			// Compute the hashed value of the key.
			virtual void ComputeHash(void)
				{ m_dwHash = (DWORD)m_Character; }

			// Test if another key is equal to this key.
			virtual BOOL Compare(Key* pKey)
				{ return ((CharacterMetricsCacheKey*)pKey)->m_Character == m_Character; }

			CHARACTER m_Character;
		};

		// Cached metrics for character.
		class CharacterMetrics
		{

		public:

			// Delta X.
			PCOORD m_lDeltaX;
		};

		// Constructor
		CharacterMetricsCacheItem(CHARACTER Character, CharacterMetrics& Metrics)
			: m_Key(Character), m_Metrics(Metrics) {}

		// Get pointer to key portion of this object.
		virtual Key* GetKey(void)
			{ return &m_Key; }

		// Key.
		CharacterMetricsCacheKey m_Key;
		
		// Cached metrics.
		CharacterMetrics m_Metrics;
	};

	// Constructor
	CCharacterMetricsCache(void) : CItemCache(127) {}
};


///////////////////////////////////////////////////////////////////
// CCharacterBitmapCache
//
//		This object defines a cache for character bitmaps. All
//		bitmaps have identical character attributes such as fill
//		resolution, rotation, etc.

class CCharacterBitmapCache : public CItemCache
{

public:

	// Class definition for item in cache.
	class CharacterBitmapCacheItem : public CItemCache::Item
	{

	public:

		// Class definition for item key.
		class CharacterBitmapCacheKey : public CItemCache::Item::Key
		{

		public:

			// Constructor
			CharacterBitmapCacheKey(CHARACTER Character, DWORD dwComponent)
				{ m_Character = Character; m_dwComponent = dwComponent; }

			// Compute the hashed value of the key.
			virtual void ComputeHash(void)
				{ m_dwHash = ((DWORD)m_Character)^(m_dwComponent<<4); }

			// Test if another key is equal to this key.
			virtual BOOL Compare(Key* pKey)
				{ return (((CharacterBitmapCacheKey*)pKey)->m_Character == m_Character)
						&& (((CharacterBitmapCacheKey*)pKey)->m_dwComponent == m_dwComponent); }

			// Character number.
			CHARACTER m_Character;

			// Component number.
			DWORD m_dwComponent;
		};

		// Cached character bitmap description.
		class CharacterBitmap
		{

		public:

			// Character bitmap "MAP" (PMG trivia: the MAP structure was defined around 1987!)
			MAP m_Map;

			// Bounding rectangle of character.
			CRect m_crCharacter;

			// Origin offset of warped cached character bitmap.
			CFixedPoint m_OriginWarpOffset;
		};

		// Constructor
		CharacterBitmapCacheItem(CHARACTER Character, DWORD dwComponents, CharacterBitmap& Bitmap);

		// Destructor
		virtual ~CharacterBitmapCacheItem();

		// Get pointer to key portion of this object.
		virtual Key* GetKey(void)
			{ return &m_Key; }

		// Key.
		CharacterBitmapCacheKey m_Key;
		
		// Cached bitmap.
		CharacterBitmap m_Bitmap;

	public:

		// Return the size of the bitmap.
		DWORD GetSize(void)
		{
			if (!m_fSizeValid)
			{
				ComputeSize();
				m_fSizeValid = TRUE;
			}
			return m_dwSize;
		}

		// Compute the size of the bitmap.
		void ComputeSize(void)
			{ m_dwSize = (DWORD)((4*((m_Bitmap.m_Map.width+31)/32))*m_Bitmap.m_Map.height); }

	protected:

		// TRUE if bitmap size has been calculated.
		BOOL m_fSizeValid;

		// Size of bitmap.
		DWORD m_dwSize;

	public:

		// Pointer to bitmap cache which owns this item.
		CCharacterBitmapCache* m_pCharacterBitmapCache;

		// Pointer to previous item in access list.
		CharacterBitmapCacheItem* m_pAccessPrevious;

		// Pointer to next item in access list.
		CharacterBitmapCacheItem* m_pAccessNext;
	};

public:

	// Constructor
	CCharacterBitmapCache(CFontCache* pFontCache);

	// Destructor
	virtual ~CCharacterBitmapCache();

public:

	// Lookup the item with the given key.
	virtual Item* Lookup(Item::Key* pKey);

	// Insert a new item into the cache.
	virtual BOOL Insert(Item* pItem);

	// Delete an item from the cache
	virtual void DeleteItem(Item* pItem);

public:

	// Pointer to controlling font cache object;
	CFontCache* m_pFontCache;
};


///////////////////////////////////////////////////////////////////
// CCharacterDescription
//
//		The class contains the attributes which uniquely define a
//		character within a particular font.

class CCharacterDescription
{

public:

	// X resolution.
	int m_nXResolution;

	// Y resolution.
	int m_nYResolution;

	// Effects rotation.
	CFixed m_lRotation;

	// Warp document ID. This number uniquely identifies the document
	// containing the warp field definition. Note: when a document is
	// closed, the cache should be notified and the appropriate
	// information removed from the cache.
	DWORD m_dwWarpDocument;

	// Warp field number. This number uniquely identifies the warp field
	// within the document. This will typically be the database record
	// number of the warp field record. Note: when a warp field is
	// removed from the database (and it's record number is no longer
	// used), the cache should be notified and the appropriate
	// information removed from the cache.
	DB_RECORD_NUMBER m_lWarpNumber;

	// The dimensions of the warp field in device coordinates.
	PCOORD m_nWarpWidth;
	PCOORD m_nWarpHeight;

	// The unrotated coordinates of the character in the warp field in device coordinates.
	PPNT m_WarpPoint;
};


///////////////////////////////////////////////////////////////////
// CCharacterCache
//
//		This object defines a cache for information that relies on
//		unique character properties (e.g., fill pattern).

class CCharacterCache : public CItemCache
{

public:

	// Class definition for item in cache.
	class CharacterCacheItem : public CItemCache::Item
	{

	public:

		// Class definition for item key.
		class CharacterCacheKey : public CItemCache::Item::Key
		{

		public:

			// The attributes which uniquely define a character.
			class CCharacterDescription m_Description;

			// Constructor
			CharacterCacheKey(void) {}
			CharacterCacheKey(CCharacterDescription& Description)
				{ m_Description = Description; }

			// Compute the hashed value of the key.
			virtual void ComputeHash(void)
				{ m_dwHash = SumBytes(&m_Description, sizeof(m_Description)); }

			// Test if another key is equal to this key.
			virtual BOOL Compare(Key* pKey)
				{ return memcmp(&(((CCharacterCache::CharacterCacheItem::CharacterCacheKey*)pKey)->m_Description), &m_Description, sizeof(m_Description)) == 0; }
		};

		// Constructor
		CharacterCacheItem(CFontCache* pFontCache, CCharacterDescription& Description) :
			m_Key(Description), m_CharacterBitmapCache(pFontCache) {}

		// Get pointer to key portion of this object.
		virtual Key* GetKey(void)
			{ return &m_Key; }

		// Key.
		CharacterCacheKey m_Key;
		
		// Character bitmap cache.
		CCharacterBitmapCache m_CharacterBitmapCache;
	};

public:

	// Constructor
	CCharacterCache(void) : CItemCache(127) {}
};


///////////////////////////////////////////////////////////////////
// CFontDescription
//
//		The class contains the attributes which uniquely define a font.

class CFontDescription
{

public:

	// Typeface number. This is the index number of the face in the typeface
	// server's typeface list. Note that when a WM_FONTCHANGE is received by
	// PrintMaster, the typeface list potentially changes and thus this index
	// becomes invalid. We could do something fancy that remaps the index or
	// deletes the font (the typefame may have gone away.) Or, we could simply
	// delete everything in the cache and start over. Since WM_FONTCHANGE is
	// very rare as Windows messages go, we choose "DELETE EVERYTHING" for now.
	int m_nTypeface;

	// Point size.
	CFixed m_lPointSize;

	// Horizontal expansion.
	CFixed m_lHorizontalExpansion;

	// Fill description.
	FillFormatV2 m_Fill;

	// Outline description.
	OutlineFormat m_Outline;

	// Shadow description.
	ShadowFormat m_Shadow;
};


///////////////////////////////////////////////////////////////////
// CFontCache
//
//		This object defines a the font cache which includes
//		information that relies only on unique font properties
//		(e.g., typeface, point size). Each font cache item also
//		contains caches for character metrics and character
//		attribute dependent items such as the character bitmaps.

class CFontCache : public CItemCache
{

public:

	// Class definition for item in cache.
	class FontCacheItem : public CItemCache::Item
	{

	public:

		// Class definition for item key.
		class FontCacheKey : public CItemCache::Item::Key
		{

		public:

			// The attributes which uniquely define a font.
			class CFontDescription m_Description;

			// Constructor
			FontCacheKey(void) {}
			FontCacheKey(CFontDescription& Description)
				{ m_Description = Description; }

			// Compute the hashed value of the key.
			virtual void ComputeHash(void)
				{ m_dwHash = SumBytes(&m_Description, sizeof(m_Description)); }

			// Test if another key is equal to this key.
			virtual BOOL Compare(Key* pKey)
				{ return memcmp(&(((CFontCache::FontCacheItem::FontCacheKey*)pKey)->m_Description), &m_Description, sizeof(m_Description)) == 0; }
		};

		// Cached font information.
		class FontMetrics
		{

		public:

			// Space width.
			PCOORD m_lSpaceWidth;

			// Underline position (offset from baseline).
			PCOORD m_lUnderlinePosition;

			// Underline thickness.
			PCOORD m_lUnderlineThickness;

			// Bounding box of all characters superimposed on their reference points.
			PBOX m_Bounds;
		};

		// Constructor
		FontCacheItem(CFontDescription& Description, FontMetrics& Metrics) :
			m_Key(Description), m_Metrics(Metrics) {}

		// Get pointer to key portion of this object.
		virtual Key* GetKey(void)
			{ return &m_Key; }

		// Key.
		FontCacheKey m_Key;
		
		// Cached metrics.
		FontMetrics m_Metrics;

		// Character metrics cache.
		CCharacterMetricsCache m_CharacterMetricsCache;

		// Character cache (bitmaps and any other character style dependent information).
		CCharacterCache m_CharacterCache;
	};

public:

	// Constructor
	CFontCache(DWORD dwMaximumCacheSize, DWORD dwMaximumCacheItemSize);

public:

	// This method is called to make sure there is enough room to insert
	// the specified bitmap into the cache. If there is not enough room,
	// the least recently used bitmaps are deleted.
	BOOL SizeBitmap(CCharacterBitmapCache::CharacterBitmapCacheItem* pBitmapItem);

	// This method is called when a bitmap item is accessed. It moves the bitmap
	// item to the front (most recently used) of the access list.
	void AccessBitmap(CCharacterBitmapCache::CharacterBitmapCacheItem* pBitmapItem);

	// This method is called when a bitmap item is inserted. It adds the bitmap size
	// to the current cache size and moves the bitmap item to the front (most
	// recently used) of the access list. This method ASSUMES that room has been
	// made for the bitmap item.
	void InsertBitmap(CCharacterBitmapCache::CharacterBitmapCacheItem* pBitmapItem);

	// This method is called when a bitmap item is deleted. It subtracts the bitmap
	// size to the current cache size and removes the bitmap item from the access list.
	void DeleteBitmap(CCharacterBitmapCache::CharacterBitmapCacheItem* pBitmapItem);

	// Return the amount of free space in the cache.
	DWORD GetFreeSize(void)
		{ return m_dwMaximumCacheSize-m_dwSize; }

protected:

	// Current size of cache.
	DWORD m_dwSize;

	// Maximum size of cache.
	DWORD m_dwMaximumCacheSize;

	// Maximum size of cache item.
	DWORD m_dwMaximumCacheItemSize;

protected:

	// Pointer to most recently used bitmap cache item.
	CCharacterBitmapCache::CharacterBitmapCacheItem* m_pAccessMostRecent;

	// Pointer to least recently used bitmap cache item.
	CCharacterBitmapCache::CharacterBitmapCacheItem* m_pAccessLeastRecent;

	// Unlink an item from the access list.
	void Unlink(CCharacterBitmapCache::CharacterBitmapCacheItem* pItem);

	// Insert an item into the front (most recently used) of the access list.
	void InsertAtFront(CCharacterBitmapCache::CharacterBitmapCacheItem* pItem);
};

#endif					// #ifndef __FCACHE_H__
