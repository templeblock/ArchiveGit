/*
// $Header: /PM8/App/CDCACHE.H 1     3/03/99 6:04p Gbeddow $
//
// Definitions for the CD cache.
//
// $Log: /PM8/App/CDCACHE.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     9/21/98 5:25p Jayn
// Broke PathManager out into a COM-style interface.
// 
//    Rev 1.0   14 Aug 1997 15:18:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:58   Fred
// Initial revision.
// 
//    Rev 1.3   07 Aug 1997 13:45:32   Jay
// file_is_cacheable can now skip the size check (by default)
// 
//    Rev 1.2   30 Jul 1997 12:49:02   Jay
// Clears the latest typeface before reinit'ing.
// 
//    Rev 1.1   17 Jul 1996 12:56:58   Jay
// Speed ups. New 'exclusive' algorithm
// 
//    Rev 1.0   14 Mar 1996 13:27:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:02   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:38:54   JAY
// Reverted. New series.
// 
//    Rev 1.2   20 Jun 1994 13:33:10   JAY
// Added access routines necessary to make the preferences dialog work.
// 
//    Rev 1.1   14 Jun 1994 17:02:26   JAY
// Added cd cache size argument to open().
// 
//    Rev 1.0   09 Jun 1994 11:16:12   JAY
// Initial revision.
*/

#ifndef __CDCACHE_H__
#define __CDCACHE_H__

#include "filedb.h"					/* File database definitions. */
#include "gcache.h"

/*
// What types of files do we store in the cache?
*/

enum
{
	CACHE_FILE_TYPE_Font = 0,
	CACHE_FILE_TYPE_Graphic
};

/*
// The CD cache file database.
*/

class CCacheFileDatabase : public FileDatabase
{
public:
	CCacheFileDatabase(StorageDevicePtr device)
		: FileDatabase(device) {}

/*
// reinitialize() [Our override for reinitialize()]
*/

	virtual ERRORCODE reinitialize(void);
};

/*
// The CD cache.
//
// We derive this from GraphicCache so that we can pass it directly to the
// database.
*/

FARCLASS CDCache : public GraphicCache
{
private:

	StorageFilePtr m_device;
	CCacheFileDatabase* m_database;
	DWORD m_cache_size;
	IPathManager* m_pPathManager;

public:

/*
// Constructor.
*/

	CDCache::CDCache();

/*
// Destructor.
*/

	CDCache::~CDCache();

/*
// Initialize the cache.
// This will either create the cache file or initialize from an existing one.
*/

	ERRORCODE open(LPCSTR cache_name, DWORD cache_size, IPathManager* pPathManager);

/*
// Flush the cache. This means to update it (like 'save'), flushing all
// changes still in memory..
*/

	ERRORCODE flush(BOOL fFinal = FALSE);

/*
// Close the cache.
*/

	void close(void);

/*
// Is the cache open?
*/

	BOOL is_open(void)
		{ return m_device != NULL && m_database != NULL; }

/*
// Store a font file in the cache.
*/

	ERRORCODE cache_font(LPCSTR name);

/*
// Prep a font file from the cache.
*/

	ERRORCODE prep_font_file(LPCSTR name, StorageFilePtr file);

/*
// Is a file cacheable?
*/

	BOOL file_is_cacheable(LPCSTR name, BOOL fCheckSize = FALSE);

/*
// Return the file name of this cache.
*/

	LPCSTR cache_name(void) const
		{ return m_device->get_name(); }

/*
// Return the size of this cache.
*/

	DWORD cache_size(void) const
		{ return m_cache_size; }

/*
// Set the cache size.
*/

	ERRORCODE set_cache_size(DWORD new_size);

/*
// Return the physical size of the cache in bytes.
*/

	DWORD get_physical_size(void);

/************************/
/* GraphicCache methods */
/************************/

/*
// Store a graphic file in the cache.
*/

	virtual ERRORCODE cache_graphic(LPCSTR name);

/*
// Prep a graphic file from the cache.
*/

	virtual ERRORCODE prep_graphic_file(LPCSTR name, StorageFilePtr file);
};

extern CDCache near cd_cache;

#endif						/* #ifndef __CDCACHE_H__ */
