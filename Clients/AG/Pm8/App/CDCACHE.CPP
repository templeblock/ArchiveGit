/*
// $Header: /PM8/App/CDCACHE.CPP 1     3/03/99 6:03p Gbeddow $
//
// CD cache routines.
//
// $Log: /PM8/App/CDCACHE.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 4     9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 3     9/21/98 5:25p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     2/05/98 11:02a Jayn
// Fixed a crash when pointer was NULL.
// 
//    Rev 1.0   14 Aug 1997 15:18:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:30   Fred
// Initial revision.
// 
//    Rev 1.11   07 Aug 1997 13:45:32   Jay
// file_is_cacheable can now skip the size check (by default)
// 
//    Rev 1.10   07 Aug 1997 09:13:10   Jay
// Traps exceptions around the Find.
// 
//    Rev 1.9   30 Jul 1997 12:49:02   Jay
// Clears the latest typeface before reinit'ing.
// 
//    Rev 1.8   20 Jul 1997 16:23:26   Jay
// Bumped max size/4 to max size/3
// 
//    Rev 1.7   13 Jun 1997 14:07:54   Jay
// Handles CollectionItem case in file_is_cacheable().
// 
//    Rev 1.6   25 Apr 1997 13:17:58   Jay
// Now also caches collection item files.
// 
//    Rev 1.5   08 Jan 1997 16:04:14   dennisp
// Changes so that workspace file is not shareable.
// 
//    Rev 1.4   16 Sep 1996 17:19:06   Jay
// Allows missing files to be considered "cacheable".
// 
//    Rev 1.3   18 Jul 1996 17:35:02   Jay
// file_is_cacheable() checks for proper database pointer.
// 
//    Rev 1.2   17 Jul 1996 17:41:50   Jay
// Will not cache a file larger than 1/4 the cache maximum size.
// 
//    Rev 1.1   17 Jul 1996 12:56:58   Jay
// Speed ups. New 'exclusive' algorithm
// 
//    Rev 1.0   14 Mar 1996 13:25:50   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:08   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:34:28   JAY
// Reverted. New series.
// 
//    Rev 1.6   04 Oct 1994 14:19:20   JAY
// Will now cache pictures from any removable media (cd or floppy).
// 
//    Rev 1.5   10 Aug 1994 15:05:16   JAY
// ...
// 
//    Rev 1.4   04 Aug 1994 08:30:48   JAY
// Large model and 32-bit changes
// 
//    Rev 1.3   22 Jun 1994 14:13:36   JAY
// Now updates the maximum size before each attempt to cache something.
// 
//    Rev 1.2   20 Jun 1994 13:32:50   JAY
// Added access routines necessary to make the preferences dialog work.
// 
//    Rev 1.1   14 Jun 1994 17:03:02   JAY
// Added cd cache size to open().
// Shows busy cursor when caching files.
// Uses dynamic name when caching (this should be unique and common).
// 
//    Rev 1.0   09 Jun 1994 11:16:12   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"

#ifdef CD_VERSION

#include "cdcache.h"
#include "fdirrec.h"
#include "compfn.h"
#include "pmgfont.h"
#include "tfengine.h"
#include "pmwcoll.h"

/////////////////////////////////////////////////////////////////////////////
// CCacheFileDatabase

ERRORCODE CCacheFileDatabase::reinitialize(void)
{
   if (pFontEngine != NULL)
   {
      pFontEngine->ClearLastFace();
   }

	return FileDatabase::reinitialize();
}

/////////////////////////////////////////////////////////////////////////////
// CDCache

/*
// The constructor for the CD cache.
*/

CDCache::CDCache(void)
{
	m_device = NULL;
	m_database = NULL;
	m_pPathManager = NULL;
}

/*
// The destructor for the CD cache.
*/

CDCache::~CDCache(void)
{
	close();
}

/*
// Open the cache.
// This will either create the cache file or open an existing one.
*/

ERRORCODE CDCache::open(LPCSTR cache_name, DWORD cache_size, IPathManager* pPathManager)
{		
	ERRORCODE error = ERRORCODE_Memory;

	m_cache_size = cache_size;

/* Create the device. */

	if ((m_device = new StorageFile(cache_name)) == NULL)
	{
		return error;
	}

/* Create the file database. */

	if ((m_database = new CCacheFileDatabase(m_device)) == NULL)
	{
		close();
		return error;
	}

	m_database->set_maximum_size(cache_size);

	// Remember the path manager.
	m_pPathManager = pPathManager;
	m_pPathManager->AddRef();

/*
// Now, we need to open the database.
// (1) Try to open the database with exclusive access rights. If we are
// the only one accessing it at this time, we can run through and reset
// all the semaphores and provide other integrity checks. If the database
// file is missing or invalid, proceed to step three.
//
// (2) If we are unable to access the database file (access violation)
// because someone else has it open, try to open it in a sharing mode.
// If this fails, the database is not available.
//
// (3) Try to initialize the database as new. If this fails, the database
// will be unavailable.
*/

#ifdef USE_WORKSPACE_SHARING
  /* Try a few times. */
	for (int i = 0; i < 5; i++)
	{
		od("** Iteration %d **\r\n", i);

	/* Try to open the database exclusively and perform validation. */

		m_device->set_open_flags(CFile::modeReadWrite | CFile::shareExclusive);
		error = m_database->read_state(TRUE);
		m_device->flush();

		od("Got error %d in exclusive open...\r\n", error);

	/* If the error is not one of access, try to initialize the database. */

		if (error != ERRORCODE_Access && error != ERRORCODE_None)
		{
		/* Try to initialize it. */

			if ((error = m_device->initialize()) == ERRORCODE_None)
			{
				error = m_database->initialize(0);
				m_device->flush();

				if (error != ERRORCODE_None)
				{
				/* Got the file, but database error. Just leave with this error. */
					m_device->zap();
					break;
				}
			}
			od("Got error %d in create\r\n", error);
		}

		m_device->set_open_flags(CFile::modeReadWrite | CFile::shareDenyNone);

	/*
	// Try to open the database in sharing mode.
	// This is the state we want to be in.
	*/

		if ((error = m_database->read_state()) == ERRORCODE_None)
		{
			error = m_database->Attach();
		}
		od("Got error %d in shared open\r\n", error);

	/* If we got it, leave now. */

		if (error == ERRORCODE_None)
		{
			break;
		}

	/* For next time around. */

		m_device->flush();
	}

#else // NON SHARED WORKSPACE FILE
	/* Try to open the database exclusively and perform validation. */

   m_device->set_open_flags(CFile::modeReadWrite | CFile::shareExclusive);
	error = m_database->read_state();

   od("Got error %d in exclusive open...\r\n", error);

  	/* If the error is not one of access, try to initialize the database. */
	if (error != ERRORCODE_Access && error != ERRORCODE_None)
	{
	  	/* Try to initialize it. */

		if ((error = m_device->initialize()) == ERRORCODE_None)
		{
			error = m_database->initialize(0);

			if (error != ERRORCODE_None)
			{
  			/* Got the file, but database error. Just leave with this error. */
				m_device->flush();
				m_device->zap();
			}
		}
		od("Got error %d in create\r\n", error);
	}
#endif
	if (error != ERRORCODE_None)
	{
		close();
	}
	return error;
}

/*
// Flush the cache. This refers to updating its state.
*/

ERRORCODE CDCache::flush(BOOL fFinal /*=FALSE*/)
{
	ERRORCODE error;

	if (m_database == NULL)
	{
		return ERRORCODE_NotInitialized;
	}

/*
// Flush all database information that needs to be written.
*/

	if ((error = m_database->flush()) != ERRORCODE_None)
	{
		printf("Got error %d flushing the CD cache.\n", error);
		return error;
	}

/*
// Write the database state information.
*/

	return m_database->write_state(fFinal);
}

/*
// Close the cache.
*/

void CDCache::close(void)
{
	// Get rid of the database.
	if (m_database != NULL)
	{
		flush(TRUE);

		delete m_database;
		m_database = NULL;
	}

	// Get rid of the device.
	if (m_device != NULL)
	{
		delete m_device;
		m_device = NULL;
	}

	// Get rid of the path manager.
	if (m_pPathManager != NULL)
	{
		m_pPathManager->Release();
		m_pPathManager = NULL;
	}
}

/*
// Store a font file in the cache.
*/

ERRORCODE CDCache::cache_font(LPCSTR name)
{
	ERRORCODE error;

/* If we don't have a database, we don't have anything. */

	if (m_database == NULL)
	{
		return ERRORCODE_NotInitialized;
	}

	TRY
	{
		m_database->set_maximum_size(m_cache_size = GET_PMWAPP()->get_workspace_size());
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL

	CompositeFileName cfn(name);

/* Store the file in the database (if not already there). */

	AfxGetApp()->BeginWaitCursor();
	error = m_database->add_file(cfn.get_dynamic_name(TRUE), CACHE_FILE_TYPE_Font);
	AfxGetApp()->EndWaitCursor();

	return error;
}

/*
// Prep a font file from the cache.
*/

ERRORCODE CDCache::prep_font_file(LPCSTR name, StorageFilePtr file)
{
/* If we don't have a database, we don't have anything. */

	if (m_database == NULL)
	{
		return ERRORCODE_NotInitialized;
	}

	CompositeFileName cfn(name);

/* Let the database prep the file. */

	return m_database->prep_file(cfn.get_dynamic_name(TRUE), CACHE_FILE_TYPE_Font, file);
}

/*
// Is a file a candidate for cacheing?
*/

BOOL CDCache::file_is_cacheable(LPCSTR name, BOOL fCheckSize /*=FALSE*/)
{
	BOOL fCacheable = FALSE;

	// If we don't have a database, we don't have anything.
	if (m_database != NULL)
	{
		long lMaxSize = m_database->get_maximum_size()/3;

		switch (m_pPathManager->GetPathBindingType(name))
		{
			case PBT_HomeDataPath:
			case PBT_RemovableDrive:
			case PBT_CDDrive:
			{
				if (fCheckSize)
				{
					// The file is of the correct type.
					// Make sure it is not too big.
					// If the file gets an error, say we can cache it (this allows
					// us to still read missing files from the cache).
					CString csName;
					TRY
					{
						// Build a real name.
						csName = m_pPathManager->ExpandPath(name);

						CFileStatus Status;
						if (!CFile::GetStatus(csName, Status)
								|| Status.m_size < lMaxSize)
						{
							fCacheable = TRUE;
						}
					}
					END_TRY
				}
				else
				{
					// Assume size is OK.
					fCacheable = TRUE;
				}
				break;
			}
			case PBT_CollectionItem:
			{
				if (fCheckSize)
				{
					// Extract the collection name.
					CString csName;
					CString csCollection;
					VERIFY(m_pPathManager->BindPath(name, csName, &csCollection) == PBT_CollectionItem);
					CPMWCollection* pCollection = GetGlobalCollectionManager()->FindCollection(csCollection);

					// Lookup the file to get its size.
					if (pCollection != NULL)
					{
						CFileContent* pContent;
						if (pCollection->NewContentStream(&pContent) == ERRORCODE_None)
						{
							CContentDirEntry Dir;
							CContentDataEntry Data;

							Dir.SetKey(csName);
							TRY
							{
								if (pContent->Find(&Dir, &Data) == ERRORCODE_None)
								{
									long lSize = (long)((CCompressInfo*)Data.GetHeader())->GetUnCompressedSize();
									fCacheable = (lSize < lMaxSize);
								}
							}
							END_TRY
							pCollection->ReleaseContentStream(pContent);
						}
					}
				}
				else
				{
					// Assume size is OK.
					fCacheable = TRUE;
				}
				break;
			}

	//		case PBT_Unknown
	//		case PBT_None:
			default:
			{
				break;
			}
		}
	}
	return fCacheable;
}

/*
// Store a graphic file in the cache.
*/

ERRORCODE CDCache::cache_graphic(LPCSTR name)
{
	ERRORCODE error;

/* If we don't have a database, we don't have anything. */

	if (m_database == NULL)
	{
		return ERRORCODE_NotInitialized;
	}

	CompositeFileName cfn(name);

	PCSTR n = cfn.get_dynamic_name(TRUE);

	if (!file_is_cacheable(n, TRUE))
	{
		return ERRORCODE_IllegalType;
	}

/* Store the file in the database (if not already there). */

	od("Cacheing graphic '%s'\r\n", (LPCSTR)n);

	AfxGetApp()->BeginWaitCursor();
	error = m_database->add_file(n, CACHE_FILE_TYPE_Graphic);
	AfxGetApp()->EndWaitCursor();

	return error;
}

/*
// Prep a graphic file from the cache.
*/

ERRORCODE CDCache::prep_graphic_file(LPCSTR name, StorageFilePtr file)
{
/* If we don't have a database, we don't have anything. */

	if (m_database == NULL)
	{
		return ERRORCODE_NotInitialized;
	}

	TRY
	{
		m_database->set_maximum_size(m_cache_size = GET_PMWAPP()->get_workspace_size());
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL

	CompositeFileName cfn(name);

	PCSTR n = cfn.get_dynamic_name(TRUE);

/* See if the file is something cacheable. */

	if (!file_is_cacheable(n))
	{
		return ERRORCODE_IllegalType;
	}

/* Let the database prep the file. */

	od("Trying to use graphic '%s' from cache... ", (LPCSTR)n);

	ERRORCODE error = m_database->prep_file(n, CACHE_FILE_TYPE_Graphic, file);

	od("(Got error %d)\r\n", error);

	return error;
}

/*
// Set the size of the CD cache.
*/

ERRORCODE CDCache::set_cache_size(DWORD new_size)
{
/* Set the new size. */

	m_cache_size = new_size;

/* Clear any open font files in the database. */

	PMGFontServer::reset_fonts();

	ERRORCODE error;

	if ((error = m_database->set_maximum_size(new_size, TRUE)) == ERRORCODE_NotInitialized)
	{
	/* We can't remain open any longer. */
		close();
	}
	return error;
}

/*
// Return the physical size of the cache in bytes.
*/

DWORD CDCache::get_physical_size(void)
{
	ST_DEV_POSITION length;

	if (m_device == NULL
			|| m_device->length(&length) != ERRORCODE_None)
	{
		return 0;
	}
	return (DWORD)length;
}

/*
// The global CD cache.
*/

CDCache near cd_cache;

#endif
