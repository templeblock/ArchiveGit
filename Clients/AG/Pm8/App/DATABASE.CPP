/*
// $Header: /PM8/App/DATABASE.CPP 1     3/03/99 6:04p Gbeddow $
//
// Database routines.
//
// $Log: /PM8/App/DATABASE.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 3     2/10/99 8:54p Jfleischhauer
// improved errTable for reporting get/release count mismatches, and
// clarified my comments about it
// 
// 2     2/09/99 4:55p Jfleischhauer
// DATABASE_STATISTICS of database creates, gets, releases and deletes now
// tallied on basis of record type to make debugging a bit less crazy
// 
//    Rev 1.0   14 Aug 1997 15:19:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:00   Fred
// Initial revision.
// 
//    Rev 1.7   18 Jul 1996 11:10:38   Jay
// More accurate statistics
// 
//    Rev 1.6   17 Jul 1996 14:10:04   Jay
// Database bookkeeping
// 
//    Rev 1.5   17 Jul 1996 12:53:22   Jay
// Speed ups. New 'exclusive' algorithm
// 
//    Rev 1.4   27 Jun 1996 13:27:32   Jay
//  
// 
//    Rev 1.3   26 Jun 1996 14:22:24   Jay
//  
// 
//    Rev 1.2   04 Jun 1996 17:37:50   Jay
//  
// 
//    Rev 1.1   25 Apr 1996 10:38:00   Jay
// Can now query the IsSharing status
// 
//    Rev 1.0   14 Mar 1996 13:47:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:32   FRED
// Initial revision.
// 
//    Rev 2.1   01 Aug 1995 12:58:26   JAY
// Even more extensive retrying on release_record.
// 
//    Rev 2.0   07 Feb 1995 15:47:38   JAY
// Initial revision.
// 
//    Rev 1.10   30 Nov 1994 16:33:54   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.9   04 Aug 1994 07:32:54   JAY
// Large model and 32-bit changes
// 
//    Rev 1.8   13 Jul 1994 11:59:56   JAY
// New access and freeing methods to enable document database utility apps.
// 
//    Rev 1.7   07 Jul 1994 11:23:48   JAY
// Added set_sharing().
// 
//    Rev 1.6   20 Jun 1994 13:21:12   JAY
// Added reinitialize().
// Added extensive retrying in release_record().
// Added an release_record in failed delete_record().
// 
//    Rev 1.5   13 Jun 1994 16:04:58   JAY
// Fixed a reversed comparison.
// 
//    Rev 1.4   09 Jun 1994 11:06:52   JAY
// Support for "shareable" databases. File locking, etc.
// 
//    Rev 1.3   01 Jun 1994 16:39:22   JAY
// Moved another new_record() method here.
// Added 'type' parameter to object construct/create methods.
// Put "m_last_error" and last_creation_error() here from PMGDatabase.
// 
//    Rev 1.2   23 May 1994 08:35:18   JAY
// Now calls (NEW) destroy() method of record in delete_record().
// 
//    Rev 1.1   16 Mar 1994 16:01:38   MACDUFF
// Divorced line spacing from font height
//
//   Rev 1.0   16 Dec 1993 16:24:36   JAY
// Initial revision.
*/

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
// The constructor for the database.
*/

Database::Database(StorageDevicePtr device, BOOL is_sharing)
			: storage_manager(device, is_sharing), table_manager(&storage_manager), initialized(FALSE)
{
/* Wipe the creator methods clean. */

	memset(creation_method, 0, sizeof(creation_method));

#ifdef DATABASE_STATISTICS
	unsigned int sz = REC_TYPE_CNT * sizeof(long);
	memset(&m_lCreateRecord, 0, sz);
	memset(&m_lGetRecord, 0, sz);
	memset(&m_lReleaseRecord, 0, sz);
	memset(&m_lGetRORecord, 0, sz);
	memset(&m_lReleaseRORecord, 0, sz);
	memset(&m_lDeleteRecord, 0, sz);
#endif
}

/*
// The destructor for the database.
*/

Database::~Database()
{
	table_manager.ReleaseSubrecords();

	initialized = FALSE;
#ifdef DATABASE_STATISTICS
	int creates = 0;
	int gets = 0;
	int releases = 0;
	int ROgets = 0;
	int ROreleases = 0;
	int deletes = 0;

	// a table to store count mismatches
	int errTable[REC_TYPE_CNT][7];
	memset(&errTable[0], 0, 7 * REC_TYPE_CNT * sizeof(long) );

	// tally counts by record type
	int errCnt = 0;
	for( int i = 0; i < REC_TYPE_CNT; i++ )
	{
		creates += m_lCreateRecord[i];
		gets += m_lGetRecord[i];
		releases += m_lReleaseRecord[i];
		ROgets += m_lGetRORecord[i];
		ROreleases += m_lReleaseRORecord[i];
		deletes += m_lDeleteRecord[i];

		// if gets and releases don't match, add a row to the errTable
		if( (m_lCreateRecord[i] + m_lGetRecord[i] != m_lReleaseRecord[i] + m_lDeleteRecord[i]) ||
				m_lGetRORecord[i] != m_lReleaseRORecord[i] )
		{
			//	There is no record type 0, so I accumulate counts for unidentified
			//	types in the zeroth array index; they will appear in the errTable
			//	as record type -1
			errTable[errCnt][0] = i == 0 ? -1 : i;
			errTable[errCnt][1] = m_lCreateRecord[i];
			errTable[errCnt][2] = m_lGetRecord[i];
			errTable[errCnt][3] = m_lReleaseRecord[i];
			errTable[errCnt][4] = m_lDeleteRecord[i];
			errTable[errCnt][5] = m_lGetRORecord[i];
			errTable[errCnt][6] = m_lReleaseRORecord[i];
			errCnt++;
		}
	}

	//	if you get an assert on the line below, there was a mismatch of
	//	the create/get count vs. the release/delete count;
	ASSERT(errCnt == 0);
//	ASSERT(creates + gets == releases + deletes);
//	ASSERT(ROgets == ROreleases);
	//
	//	the errTable will contain errCnt "entries", one for each record type
	//	that had a mismatch, in this order:
	//		[0] = record type
	//		[1] = creates
	//		[2] = gets
	//		[3] = releases
	//		[4] = deletes
	//		[5] = read-only gets
	//		[6] = read-only releases
	//
	//	Note: Counts for records whose type could not be identifiec appear in
	//			the errTable as record type -1.
	//
	//	Note:	The table is based on the current record types (total 34, last
	//			being RECORD_TYPE_Component.  If more record types are added, the
	//			REC_TYPE_CNT definition in DATABASE.H must be increased, and checks
	//			for RECORD_TYPE_Component in this file must be changed.


//	TRACE("create: %ld; get: %ld/%ld; release: %ld/%ld; delete: %ld\n",
//			creates, gets, ROgets, releases, ROreleases, deletes );
#endif
}

/*
// Initialize the database.
// The starting position in the storage device is passed.
*/

ERRORCODE Database::initialize(ST_DEV_POSITION position)
{
	ST_DEV_POSITION storeman_start, tableman_start, store_start;
	ERRORCODE error;

/* Remember our start for later. */

	database_start = position;

/* Compute the start of the free store area. */

	storeman_start = position;
	tableman_start = storeman_start + storage_manager.SizeState();
	store_start = tableman_start + table_manager.size_state();

/* Initialize everybody. */

	if ((error = storage_manager.Initialize(storeman_start, store_start)) == ERRORCODE_None)
	{
		error = table_manager.initialize(tableman_start);
	}

/* We're initialized. */

	initialized = TRUE;

	return error;
}

/*
// Reinitialize the database.
*/

ERRORCODE Database::reinitialize(void)
{
	return table_manager.reinitialize();
}

/*
// Read the state of the database.
*/

ERRORCODE Database::read_state(BOOL validate, BOOL reset_semaphores)
{
	ERRORCODE error;

/* Remember our start */

	if ((error = storage_manager.m_pDevice->tell(&database_start)) == ERRORCODE_None)
	{
	/* Read the state of the managers. */

		if ((error = storage_manager.ReadState(validate, reset_semaphores)) == ERRORCODE_None)
		{
			if ((error = table_manager.read_state(validate, reset_semaphores)) == ERRORCODE_None)
			{
			/* Integrity check the storage area if sharing. */
				if (!validate
						|| (error = storage_manager.IntegrityCheck(reset_semaphores)) == ERRORCODE_None)
				{
				/* We're initialized. */
					initialized = TRUE;
				}
			}
		}
	}

	return error;
}

ERRORCODE Database::Attach(void)
{
	return storage_manager.Attach();
}

ERRORCODE Database::Detach(void)
{
	return storage_manager.Detach();
}

/*
// Write the state of the database.
*/

ERRORCODE Database::write_state(BOOL fDetach /*=FALSE*/)
{
	ERRORCODE error;

/* Make sure we're initialized. */

	if (!initialized)
	{
		return ERRORCODE_NotInitialized;
	}

/* Seek to our start. */

	if ((error = storage_manager.m_pDevice->seek(database_start, ST_DEV_SEEK_SET)) == ERRORCODE_None)
	{
	/* Write the state of the managers. */

		if ((error = storage_manager.WriteState()) == ERRORCODE_None)
		{
			if ((error = table_manager.write_state()) == ERRORCODE_None)
			{
				if (fDetach)
				{
					error = storage_manager.Detach();
				}
			}
		}
	}
	return error;
}

/*
// Flush the database.
*/

ERRORCODE Database::flush()
{
	ERRORCODE error;

/* Flush the managers. */

	if ((error = table_manager.flush()) == ERRORCODE_None)
	{
		error = storage_manager.Flush();
	}
	return error;
}

/*
// Register a creation method with the database.
*/

ERRORCODE Database::register_creation_method(DB_RECORD_TYPE type, RECORD_CREATOR method)
{
/* Validate the type. */

	if (type < 0 || type >= TYPES_PER_DATABASE)
	{
		return ERRORCODE_IllegalType;
	}

/* Do the register. */

	creation_method[type] = method;

	return ERRORCODE_None;
}

/*
// A private method to create a record.
*/

ERRORCODE Database::create_record(DB_RECORD_TYPE type, DB_RECORD_NUMBER number, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Validate the type. */

	if (type < 0 || type >= TYPES_PER_DATABASE)
	{
		return ERRORCODE_IllegalType;
	}

/* See if a creation method has been registered. */

	RECORD_CREATOR creator;

	if ((creator = creation_method[type]) == NULL)
	{
		return ERRORCODE_NotRegistered;
	}

/* Invoke the creator. */

	return creator(number, type, this, creation_data, where, record);
}

/*
// Create a new record. The record remains locked until a release_object()
// is done.
*/

ERRORCODE Database::new_record(DB_RECORD_TYPE type, VOIDPTR creation_data, DatabaseRecordPtr far *record)
{
	ERRORCODE error;
	DB_RECORD_NUMBER number;

	// Always want to be exclusive.
	storage_manager.SetExclusive(TRUE);

/* The record should start out as NULL as an additional check. */

	*record = NULL;

/* Allocate a new entry. */

	if ((error = table_manager.allocate_entry(&number, type)) == ERRORCODE_None)
	{
		RECORD_FILE_ENTRY far *fentry;
		RECORD_MEMORY_ENTRY far *mentry;

	/* Lock the entry. */

		if ((error = table_manager.lock_entry(number, &fentry, &mentry)) == ERRORCODE_None)
		{
			if ((error = create_record(type, number, creation_data, &fentry->position, record)) == ERRORCODE_None)
			{
			/* Remember this for later. */

//				TRACE("new_record allocated entry %ld (%d)\n", (*record)->Id(), (*record)->type());
				mentry->data = *record;

			/* Start us off right. */

				if (storage_manager.IsSharing())
				{
					error = mentry->data->write();
				}

				if (error == ERRORCODE_None)
				{
					mentry->data->ModifyLock();
					mentry->data->ElementModified();
				}
				else
				{
				/* Unable to write. */
					delete mentry->data;
					mentry->data = *record = NULL;
				}
			}

			if (error != ERRORCODE_None)
			{
			/* Error clean up. */
				table_manager.unlock_entry(number);
				table_manager.deallocate_entry(number);
			}
		}
	}
	storage_manager.SetExclusive(FALSE);
#ifdef DATABASE_STATISTICS
	if (error == ERRORCODE_None)
	{
		if( type < RECORD_TYPE_ProjectPreview || type > RECORD_TYPE_Component )
		{
			m_lCreateRecord[0]++;
		}
		else
		{
			m_lCreateRecord[type]++;
		}
	}
#endif
	return error;
}

/*
// Another way to create a record.
*/

DatabaseRecordPtr Database::new_record(DB_RECORD_TYPE type, VOIDPTR creation_data)
{
	DatabaseRecordPtr record;

	if ((m_last_error = new_record(type, creation_data, &record)) != ERRORCODE_None)
	{
		record = NULL;
	}

	return record;
}

/*
// delete_record()
//
// Remove a record from the database. The record number becomes invalid.
*/

ERRORCODE Database::delete_record(DB_RECORD_NUMBER number, DB_RECORD_TYPE type)
{
/* We need to get the record so we can tell it it's being destroyed. */

	DatabaseRecordPtr record;
	ERRORCODE error;

//	od("Delete get record %ld...\r\n", number);

	if ((record = get_record(number, &error, type)) == NULL)
	{
		return error;
	}

	if ((error = record->zap()) != ERRORCODE_None)
	{
	/* Zap failed! Record still exists. We need to unlock it for next time. */
		record->release();
	}
	return error;
}

/*
// Delete a database record given the record itself.
*/

ERRORCODE Database::delete_record(DatabaseRecordPtr record)
{
#ifdef DATABASE_STATISTICS
	if( record->type() < RECORD_TYPE_ProjectPreview || record->type() > RECORD_TYPE_Component )
	{
		m_lDeleteRecord[0]++;
	}
	else
	{
		m_lDeleteRecord[record->type()]++;
	}
#endif

/* Deallocate the space for it in the table manager's list. */

	ERRORCODE error;
//	od("Modify lock %ld...\r\n", record->id());
	if ((error = record->ModifyLock()) == ERRORCODE_None)
	{
	/* Free the entry. */

//		TRACE("Deallocate entry %ld (%d)...\n", record->Id(), record->type());

		if ((error = table_manager.deallocate_entry(record->Id(), record->type(), TRUE)) == ERRORCODE_None)
		{
		/*
		// Successfully deallocated. Delete the record.
		// "We are destroying you..."
		*/

//			od("Modify unlock record %ld...\r\n", record->id());

			record->DropAllLocks();

//			od("Destroy record %ld...\r\n", record->id());
			record->destroy();
			delete record;
		}
		else
		{
			record->ModifyUnlock();
		}
	}
	return error;
}

/*
// Get the data associated with a record.
// The record's lock count is increased by one.
*/

DatabaseRecordPtr Database::get_record(DB_RECORD_NUMBER number, LPERRORCODE e, DB_RECORD_TYPE type, BOOL read_only)
{
	RECORD_FILE_ENTRY far *fentry;
	RECORD_MEMORY_ENTRY far *mentry;
	ERRORCODE error;
	DatabaseRecordPtr record = NULL;

	// Always want to be exclusive.
	storage_manager.SetExclusive(TRUE);

//	TRACE("get_record: %ld, %d, %d\n", number, type, read_only);

/* Lock the entry. */

	if ((error = table_manager.lock_entry(number, &fentry, &mentry, read_only)) == ERRORCODE_None)
	{
	/* Do some type checking. */
		if (type != DB_ANY_TYPE && type != fentry->type)
		{
			table_manager.unlock_entry(number, read_only, FALSE);
			error = ERRORCODE_TypeMismatch;
		}
		else
		{
		/*
		// Make sure we have the record in memory.
		*/

			if (mentry->data == NULL)
			{
				DatabaseRecordPtr new_record;

			/* Create the new record and read it into memory. */

				if ((error = create_record(fentry->type, number, NULL, &fentry->position, &new_record)) == ERRORCODE_None)
				{
				/* Read the record from its position. */

					if ((error = new_record->read()) == ERRORCODE_None)
					{
					/* Success! Use this record now. */

						mentry->data = new_record;
					}
					else
					{
					/* We failed to read the record. Unlock and delete it. */

						delete new_record;
					}
				}
			}

		/* Handle the lock if successful. */

			if ((record = mentry->data) != NULL)
			{
				if (read_only)
				{
					error = record->ReadLock();
				}
				else
				{
					error = record->ModifyLock();
				}
			}

			if (error != ERRORCODE_None)
			{
				record = NULL;
				table_manager.unlock_entry(number, read_only, FALSE);
			}
		}
	}

	if (e != NULL)
	{
		*e = error;
	}
	storage_manager.SetExclusive(FALSE);

#ifdef DATABASE_STATISTICS
	if (record != NULL)
	{
		if (read_only)
		{
			if( type == DB_ANY_TYPE )
			{
				m_lGetRORecord[fentry->type]++;
			}
			else if( type < RECORD_TYPE_ProjectPreview || type > RECORD_TYPE_Component )
			{
				m_lGetRORecord[0]++;
			}
			else
			{
				m_lGetRORecord[type]++;
			}
		}
		else
		{
			if( type == DB_ANY_TYPE )
			{
				m_lGetRecord[fentry->type]++;
			}
			else if( type < RECORD_TYPE_ProjectPreview || type > RECORD_TYPE_Component )
			{
				m_lGetRecord[0]++;
			}
			else
			{
				m_lGetRecord[type]++;
			}
		}
	}
#endif

	return record;
}

/*
// Release a record.
*/

ERRORCODE Database::release_record(DatabaseRecordPtr record, BOOL read_only)
{
	if (!initialized)
	{
		return ERRORCODE_NotInitialized;
	}

#ifdef DATABASE_STATISTICS
	if (read_only)
	{
		if( record->type() < RECORD_TYPE_ProjectPreview || record->type() > RECORD_TYPE_Component )
		{
			m_lReleaseRORecord[0]++;
		}
		else
		{
			m_lReleaseRORecord[record->type()]++;
		}
	}
	else
	{
		if( record->type() < RECORD_TYPE_ProjectPreview || record->type() > RECORD_TYPE_Component )
		{
			m_lReleaseRecord[0]++;
		}
		else
		{
			m_lReleaseRecord[record->type()]++;
		}
	}
#endif

/*
// It's very important that we release the record.
// If we don't, then the whole locking scheme gets out of whack.
// So we do extensive retrys on this in order to be as sure as we can that
// this will be successful.
*/

	// Always want to be exclusive.
	storage_manager.SetExclusive(TRUE);

	ERRORCODE error;

	for (int retry_count = 50; retry_count-- != 0; )
	{
		if ((error = table_manager.unlock_entry(record->Id(), read_only)) != ERRORCODE_Busy)
		{
			break;
		}
	}
	storage_manager.SetExclusive(FALSE);
	return error;
}

/*
// Modified a record.
*/

ERRORCODE Database::modified_record(DatabaseRecordPtr record)
{
	return table_manager.modified_entry(record->Id());
}

/*
// Duplicate a record by record.
*/

DatabaseRecordPtr Database::duplicate_record(DatabaseRecordPtr record, DatabasePtr new_database, LPERRORCODE lperror)
{
	return record->duplicate(new_database, lperror);
}

/*
// Duplicate a record by number.
*/

DB_RECORD_NUMBER Database::duplicate_record(DB_RECORD_NUMBER record_number, DatabasePtr new_database, LPERRORCODE lperror)
{
	DB_RECORD_NUMBER new_record_number = 0;

/*
// Get the record for duplication purposes.
*/

	if (record_number != 0)
	{
		DatabaseRecordPtr record;
		if ((record = get_record(record_number, lperror)) != NULL)
		{
			DatabaseRecordPtr new_record;

		/* Invoke the record to duplicate itself. */

			if ((new_record = duplicate_record(record, new_database, lperror)) != NULL)
			{
			/* Success! Release the record after getting its number. */
				new_record_number = new_record->Id();
				new_record->release();
			}
			record->release();
		}
	}

	return new_record_number;
}

/*
// Force a new device on this database.
// An error will be reported if the database has been initialized.
*/

ERRORCODE Database::force_new_device(StorageDevicePtr new_device)
{
	if (initialized)
	{
		return ERRORCODE_Busy;
	}

	ERRORCODE error;

	if ((error = storage_manager.ForceNewDevice(new_device)) == ERRORCODE_None)
	{
		error = table_manager.force_new_device();
	}
	return error;
}

ERRORCODE Database::set_sharing(BOOL is_sharing)
{
	if (initialized)
	{
		return ERRORCODE_Busy;
	}

	return storage_manager.SetSharing(is_sharing);
}

/*
// Are we in sharing mode?
*/

BOOL Database::IsSharing(void) const
{
	return storage_manager.IsSharing();
}

/*
// Free all table storage.
// This merely frees the *storage* for them. They will automatically be
// allocated storage at write/flush time.
//
// This should NOT be called on a shared database (locking is not performed).
*/

ERRORCODE Database::deallocate_table_storage(void)
{
	if (!initialized)
	{
		return ERRORCODE_NotInitialized;
	}
	return table_manager.deallocate_table_storage();
}
