/*
// $Header: /PM8/App/DATABASE.H 1     3/03/99 6:04p Gbeddow $
//
// Definitions for database objects.
//
// $Log: /PM8/App/DATABASE.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     2/09/99 4:55p Jfleischhauer
// DATABASE_STATISTICS of database creates, gets, releases and deletes now
// tallied on basis of record type to make debugging a bit less crazy
// 
//    Rev 1.0   14 Aug 1997 15:19:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:26   Fred
// Initial revision.
// 
//    Rev 1.4   17 Jul 1996 14:10:04   Jay
// Database bookkeeping
// 
//    Rev 1.3   17 Jul 1996 12:53:22   Jay
// Speed ups. New 'exclusive' algorithm
// 
//    Rev 1.2   25 Apr 1996 10:38:00   Jay
// Can now query the IsSharing status
// 
//    Rev 1.1   12 Apr 1996 12:49:56   Jay
// Bumped types per database to 64.
// 
//    Rev 1.0   14 Mar 1996 13:43:14   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:34   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:26   JAY
// Initial revision.
// 
//    Rev 1.7   30 Nov 1994 16:33:40   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.6   13 Jul 1994 11:59:54   JAY
// New access and freeing methods to enable document database utility apps.
// 
//    Rev 1.5   07 Jul 1994 11:23:48   JAY
// Added set_sharing().
// 
//    Rev 1.4   20 Jun 1994 13:20:34   JAY
// Made some private variables protected.
// Added reinitialize().
// 
//    Rev 1.3   09 Jun 1994 11:06:50   JAY
// Support for "shareable" databases. File locking, etc.
// 
//    Rev 1.2   01 Jun 1994 16:38:52   JAY
// Moved another new_record() method here.
// Added 'type' parameter to object construct/create methods.
// Put "m_last_error" and last_creation_error() here from PMGDatabase.
*/

#ifndef __DATABASE_H__
#define __DATABASE_H__

/*
// Database definitions and classes.
//
// Oct. 13, 1992 - Jay N. : Initial creation.
//
*/

#include "tableman.h"

/*
// A DatabaseRecord creation method.
*/

typedef ERRORCODE (*RECORD_CREATOR)(DB_RECORD_NUMBER, DB_RECORD_TYPE, DatabasePtr, VOIDPTR, ST_DEV_POSITION far *, DatabaseRecordPtr far *);

#define	TYPES_PER_DATABASE	64

#ifdef _DEBUG
#define DATABASE_STATISTICS
#define REC_TYPE_CNT	35			// RECORD_TYPE_Component + 1 (for unknown types)
#endif

/*
// The database.
*/

class Database
{
private:
/*
// The record creators for this database.
// Theoretically, each database could have its own different set of creation
// methods. This may be useful in the future.
//
// You set a creation method by calling register_creation_method().
*/

	RECORD_CREATOR		creation_method[TYPES_PER_DATABASE];

/*
// The StorageManager for this database.
// Every database must have a StorageManager.
*/

	StorageManager storage_manager;

/*
// The TableManager for this database.
*/

	TableManager table_manager;

/*
// Create a record by calling the appropriate creation method.
// The creation data field will be NULL when the record is
// being read from the database. Otherwise, it's the value passed to
// new_record() by the caller.
*/

	ERRORCODE create_record(DB_RECORD_TYPE type, DB_RECORD_NUMBER number, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);
	friend DatabaseRecord::DatabaseRecord(DB_RECORD_NUMBER record_number, DB_RECORD_TYPE record_type, DatabasePtr owner, ST_DEV_POSITION far *in_where);

protected:

/*
// Are we initialized?
*/

	BOOL initialized;

/*
// The start of this database.
*/

	ST_DEV_POSITION	database_start;

/*
// The last error encountered.
*/

	ERRORCODE m_last_error;

#ifdef DATABASE_STATISTICS
	long m_lCreateRecord[REC_TYPE_CNT];
	long m_lGetRecord[REC_TYPE_CNT];
	long m_lReleaseRecord[REC_TYPE_CNT];
	long m_lGetRORecord[REC_TYPE_CNT];
	long m_lReleaseRORecord[REC_TYPE_CNT];
	long m_lDeleteRecord[REC_TYPE_CNT];
#endif

public:

/*
// The constructor for a database.
*/

	Database(StorageDevicePtr device, BOOL is_sharing = FALSE);

/*
// The destructor for a database.
*/

	~Database();

/*
// initialize()
// 
// Initialize the database. The start of the database in the storage
// device is passed as a parameter.
// 
*/

	virtual ERRORCODE initialize(ST_DEV_POSITION position);

/*
// reinitialize()
//
// This will reset the storage and table managers so that the file is empty.
*/

	virtual ERRORCODE reinitialize(void);

/*
// read_state()
//
// Read the state of the Database from the device.
// This includes the states for the database's sub-managers.
*/

	ERRORCODE read_state(BOOL validate = FALSE, BOOL reset_semaphores = TRUE);

/*
// write_state()
//
// Write the state of the Database to the device.
// This includes the states for the database's sub-managers.
*/

	ERRORCODE write_state(BOOL fDetach = FALSE);

/*
// flush()
//
// Write anything which needs to be written to the StorageDevice.
// This include all tables and records.
*/

	ERRORCODE flush();

/*
// get_storage_manager()
//
// Return the address of the storage manager used in this database.
*/

	StorageManagerPtr get_storage_manager(void)
		{ return &storage_manager; }

/*
// get_storage_device()
//
// Return the address of the storage device used in this database.
*/

	StorageDevicePtr get_storage_device(void)
		{ return storage_manager.m_pDevice; }

/*
// Attach()
//
// Attach ourselves to this database (as another user).
*/

	ERRORCODE Attach(void);

/*
// Detach()
//
// Detach ourselves from this database (one less user).
*/

	ERRORCODE Detach(void);

/*********************/
/* Record Management */
/*********************/

/*
// register_creation_method()
//
// This will register a routine to create a record of a certain type with
// the database so that the database will know how to call when a record
// needs creation.
*/

	ERRORCODE register_creation_method(DB_RECORD_TYPE type, RECORD_CREATOR method);

/*
// new_record()
//
// Create a new record in the database.
// This will allocate a new record number, then call the appropriate
// creation method to actually create the record.
// "creation_data" is specific to the type of record being created.
*/

	ERRORCODE new_record(DB_RECORD_TYPE type, VOIDPTR creation_data, DatabaseRecordPtr far *record);
	DatabaseRecordPtr new_record(DB_RECORD_TYPE type, VOIDPTR creation_data);

/*
// last_creation_error()
//
// Return the last creation error.
*/

	ERRORCODE last_creation_error() { return m_last_error; }

/*
// delete_record()
//
// Remove a record from the database. The record number becomes invalid.
//
// The type of record expected is also passed so we can do some
// basic integrity checking. If DB_ANY_TYPE is passed as the type (or the
// default argument is used), no type checking is done. ** THIS IS NOT
// RECOMMENDED. ***
*/

	ERRORCODE delete_record(DB_RECORD_NUMBER number, DB_RECORD_TYPE type = DB_ANY_TYPE);
	ERRORCODE delete_record(DatabaseRecordPtr record);

/*
// get_record()
//
// Get the desired record. The lock count for the record is incremented.
// (i.e. The record is locked.)
// If the record is loaded from the StorageDevice, the record is considered
// "clean" until dirtied by modified_record().
//
// The type of record expected is also passed so we can do some
// basic integrity checking. If DB_ANY_TYPE is passed as the type (or the
// default argument is used), no type checking is done. ** THIS IS NOT
// RECOMMENDED. ***
//
// This calling sequence differs from the rest (ERRORCODE is not the return
// value), but this simplifies calling it greatly.
*/

	DatabaseRecordPtr get_record(DB_RECORD_NUMBER number, LPERRORCODE error, DB_RECORD_TYPE type = DB_ANY_TYPE, BOOL read_only = FALSE);

/*
// modified_record()
//
// Mark the specified record as having been modified.
// (Records created by new_record() are initially modified by default.)
*/

	ERRORCODE modified_record(DatabaseRecordPtr record);

/*
// release_record()
//
// Release the specified record. The lock count for the record is decremented.
// An error is returned it the record is not locked.
*/

	ERRORCODE release_record(DatabaseRecordPtr record, BOOL read_only = FALSE);

/*
// attach_record_data()
//
// Attach data to a record.
// Any data currently attached to the record is freed.
*/

//	ERRORCODE attach_record_data(DB_RECORD_NUMBER number, DatabaseRecordPtr record);

/*
// detach_record_data()
//
// Detach the data from a record.
//
// (FUTURE DEVELOPMENT)
*/

/*
// Duplicate a record.
// If you pass a record number, you get a record number.
// If you pass an actual record, you get an actual record.
*/

	DatabaseRecordPtr duplicate_record(DatabaseRecordPtr record, DatabasePtr dup_database = NULL, LPERRORCODE lperror = NULL);
	DB_RECORD_NUMBER duplicate_record(DB_RECORD_NUMBER record_number, DatabasePtr dup_database = NULL, LPERRORCODE lperror = NULL);

/*
// Force a new device on this database.
// An error will be reported if the database has been initialized.
*/

	ERRORCODE force_new_device(StorageDevicePtr new_device);

/*
// Set the sharing mode for the database.
// An error will be reported if the database has been initialized.
*/

	ERRORCODE set_sharing(BOOL is_sharing);

/*
// Are we in sharing mode?
*/

	BOOL IsSharing(void) const;

/*
// Free all table storage.
// This merely frees the *storage* for them. They will automatically be
// allocated storage at write/flush time.
//
// This should NOT be called on a shared database (locking is not performed).
*/

	ERRORCODE deallocate_table_storage(void);

};

//typedef Database far *DatabasePtr;			/* In record.h */

#endif						/* #ifndef __DATABASE_H__ */
