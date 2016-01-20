/*
// $Header: /PM8/App/GCACHE.H 1     3/03/99 6:05p Gbeddow $
//
// Definitions for the graphic cache interface.
//
// $Log: /PM8/App/GCACHE.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:46:50   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:54   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:36   JAY
// Initial revision.
// 
//    Rev 1.0   09 Jun 1994 11:12:22   JAY
// Initial revision.
*/

#ifndef __GCACHE_H__
#define __GCACHE_H__

/*
// The graphic file cache.
// This merely defines an interface (like OLE) that can be registered with
// the database. In other words, this is an abstract class.
*/

FARCLASS GraphicCache
{
public:
/*
// A virtual destructor.
// This allows a derived class to have its destructor called correctly.
*/

	virtual ~GraphicCache() {}

/*
// A graphic file is being used for the first time.
// Allow the cache to do something good with it (like store it).
*/

	virtual ERRORCODE cache_graphic(LPCSTR name) = 0;

/*
// A graphic file needs to be opened.
// Allow the cache to do something good with it (like virtualize it).
*/

	virtual ERRORCODE prep_graphic_file(LPCSTR name, StorageFilePtr file) = 0;

};

typedef GraphicCache far *GraphicCachePtr;

#endif						/* #ifndef __GCACHE_H__ */
