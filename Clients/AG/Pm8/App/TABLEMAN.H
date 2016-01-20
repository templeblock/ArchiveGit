/*
// $Header: /PM8/App/TABLEMAN.H 1     3/03/99 6:12p Gbeddow $
//
// Table manager definitions.
// The table manager manages the "record table" in the database.
// From the database's perspective, there is one large table (holding all
// record entries). The actual implementation is different (for obvious
// reasons).
//
// The definitions for RecordTable, TableLocator,
// and TableManager are in this file.
//
// $Log: /PM8/App/TABLEMAN.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:08   Fred
// Initial revision.
// 
//    Rev 1.1   04 Jun 1996 17:38:58   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:43:08   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:28   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:49:02   JAY
// Initial revision.
// 
//    Rev 1.4   30 Nov 1994 16:33:38   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.3   13 Jul 1994 11:59:52   JAY
// New access and freeing methods to enable document database utility apps.
// 
//    Rev 1.2   20 Jun 1994 13:22:24   JAY
// Added reinitialize().
// Added param to free_all_records().
// 
//    Rev 1.1   09 Jun 1994 11:06:50   JAY
// Support for "shareable" databases. File locking, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:38   JAY
//  
*/

#ifndef __TABLEMAN_H__
#define __TABLEMAN_H__

/*
// Oct. 14, 1992 - Jay N. : Initial definitions
*/

/*
// A note about being "dirty":
// TableLocators and RecordTables keep track of whether they need writing
// or not.
// DatabaseRecords do NOT keep track of their dirty status. That's handled
// by the RecordTable.
*/

#include "record.h"			/* We need storage manager definitions. */
#include "list.h"					/* We need list definitions. */
#include "rwsem.h"

/*
// How many records are in each record table.
// (In Avagio 2.0, this was 256.)
*/

#define	RECORDS_PER_TABLE		64

/*
// A record index.
// This is the entry of a record within a table.
// This number ranges from 0 to RECORDS_PER_TABLE-1.
*/

typedef SHORT RECORD_INDEX;

/*
// A table number.
// This is the ID of the table in the database.
// Table numbers start from 0.
*/

typedef ST_MAN_ID TABLE_NUMBER;

/*
// A table index.
// This is the entry of the table within the locator.
// This number ranges from 0 to TABLES_PER_LOCATOR-1.
*/

typedef SHORT TABLE_INDEX;

/*
// A locator number.
// This is the ID of the locator in the database.
// Locator numbers start from 0.
*/

typedef SHORT LOCATOR_NUMBER;

/*
// The size of a database element (record, table, etc.)
*/

typedef LONG DB_SIZE;

/*
// A record table entry.
*/

struct RECORD_FILE_ENTRY
{
	DB_RECORD_TYPE		type;					/* The type of this record */
	ST_DEV_POSITION	position;			/* Offset in storage file */
};

/*
// A record in-memory entry.
*/

struct RECORD_MEMORY_ENTRY
{
	DatabaseRecordPtr		data;						/* In-memory pointer */
	SHORT						lock_count;
};

/*
// A record (sub)table.
// This object is a storage element, and is managed by the storage manager.
*/

FARCLASS RecordTable : public StorageElement
{
protected:
	SHORT		used_record_count;					/* How many records used? */
	SHORT		locked_record_count;					/* How many records locked? */

/*
// The file entry portion.
// Only this part is actually written to the storage device.
*/

	RECORD_FILE_ENTRY		file_entry[RECORDS_PER_TABLE];

/*
// The memory entry portion.
// This part manages the "in-memory" half of the records.
*/

	RECORD_MEMORY_ENTRY	memory_entry[RECORDS_PER_TABLE];

/*
// free_all_records()
//
// Free the memory for all records in the table.
// This is used by the destructor and the read method.
*/

	void free_all_records(BOOL force = TRUE);

/*
// flush_record()
//
// Flush a record if the record has been modified.
*/

	ERRORCODE flush_record(RECORD_FILE_ENTRY far *fentry, RECORD_MEMORY_ENTRY far *mentry);

public:

/*
// The constructor for a RecordTable.
*/

	RecordTable(TABLE_NUMBER id, StorageManagerPtr manager);

/*
// The destructor for a RecordTable.
*/

	~RecordTable();

/*
// allocate_entry()
//
// Allocate an entry in the table. The index is returned.
*/

	ERRORCODE allocate_entry(RECORD_INDEX far *record_number, DB_RECORD_TYPE type = DB_ANY_TYPE);

/*
// deallocate_entry()
//
// Deallocate an entry in the table.
*/

	ERRORCODE deallocate_entry(RECORD_INDEX record_number, DB_RECORD_TYPE type = DB_ANY_TYPE);

/*
// get_entry()
//
// Return references to a record entry.
*/

	ERRORCODE get_entry(RECORD_INDEX record_number, RECORD_FILE_ENTRY far * far *fentry, RECORD_MEMORY_ENTRY far * far *mentry);

/*
// flush()
//
// Write anything which needs to be written to the StorageDevice.
// This would include all modified DatabaseRecords in memory.
*/

	ERRORCODE flush(ST_DEV_POSITION far *where);

/*
// ReleaseSubrecords()
//
// Release all subrecords for any records we manage.
*/

	void ReleaseSubrecords(void);

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the table.
*/

	ERRORCODE	ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// Write the table.
*/

	ERRORCODE	WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// Return the size of the table.
*/

	ST_MAN_SIZE SizeofData(StorageDevicePtr device) { return sizeof(file_entry); }

};

typedef RecordTable far *RecordTablePtr;

/*
// When a table's position is unknown, its position is -1.
// When a table has not yet been allocated in the file, its position is 0.
*/

#define	TABLE_POSITION_UNKNOWN		(-1L)
#define	TABLE_POSITION_UNALLOCATED	(0L)

/*
// The storage management component of a TableLocator.
*/

typedef struct
{
	ST_DEV_POSITION	position;
} TABLE_FILE_ENTRY;

/*
// The memory management component of a TableLocator.
*/

typedef struct
{
	RecordTablePtr		table;
} TABLE_MEMORY_ENTRY;

#define	TABLES_PER_LOCATOR	16			/* 16 tables hold 1024 records */

/*
// The table locator (directory) class.
// This object is a storage element, and is managed by the storage manager.
*/

typedef class TableLocator far *TableLocatorPtr;

FARCLASS TableLocator : public StorageElement, public ListElement
{
	friend class TableManager;
protected:
/*
// How many entries have been allocated?
*/

	SHORT						tables_in_use;

/*
// The file entry portion.
// Only this part is actually written to the storage device.
*/

	TABLE_FILE_ENTRY		file_entry[TABLES_PER_LOCATOR];

/*
// The memory entry portion.
// The part manages the "in-memory" half of the locators.
*/

	TABLE_MEMORY_ENTRY		memory_entry[TABLES_PER_LOCATOR];

/*
// The address of the next locator in the storage device.
*/

	ST_DEV_POSITION	next_locator_position;

/*
// build_table_number()
//
// Build a table number for this locator. The passed number must be a valid
// table index (0 to TABLES_PER_LOCATOR-1) or a bogus table number will
// be generated.
*/

	TABLE_NUMBER build_table_number(TABLE_INDEX table_index) { return Id()*TABLES_PER_LOCATOR + table_index; }

/*
// get_table_index()
//
// Convert a table number into a table index within this locator.
// If the table doesn't belong to this locator, -1 is returned to indicate
// an error.
*/

	TABLE_INDEX get_table_index(TABLE_NUMBER table_number);

/*
// free_all_tables()
//
// Free the memory for all tables in the locator.
// This is used by the destructor and the read method.
*/

	void free_all_tables(void);

public:

/*
// The next table locator. This is public to ease table management
// (which is what the table manager does best).
*/

//	TableLocatorPtr	next;

/*
// The constructor for a table locator.
*/

	TableLocator(LOCATOR_NUMBER id, StorageManagerPtr manager);

/*
// The destructor for a TableLocator.
*/

	~TableLocator();

/*
// new_table()
//
// Create a new table in the locator.
// A pointer to the table is returned.
*/

	ERRORCODE new_table(RecordTablePtr far *table);

/*
// delete_table()
//
// Delete a table in the locator. The table is not allocated anymore.
// Due to the nature of how tables are allocated, it's considered an error
// to attempt to free a table that is not the last table allocated in the 
// locator.
*/

	ERRORCODE delete_table(RecordTablePtr table);

/*
// free_table()
//
// Free the memory associated with a table preserving the table contents.
// The table is written if it has been modified.
*/

	ERRORCODE free_table(RecordTablePtr table);

/*
// get_table()
//
// Get an existing table.
// If the table does not exist, an error is returned.
// 'which' should be in the correct range.
*/

	ERRORCODE get_table(TABLE_INDEX which, RecordTablePtr far *table);

/*
// flush()
//
// Write anything which needs to be written to the StorageDevice.
// This will include all modified tables.
*/

	ERRORCODE flush(ST_DEV_POSITION far *where);

/*
// set_next_locator()
//
// Set the position of the next table locator.
*/

	void set_next_locator(ST_DEV_POSITION next)
		{ next_locator_position = next; ElementModified(); }

/*
// deallocate_table_storage()
//
// Run through and deallocate the storage for all tables in this locator.
// The storage will automatically be reallocated at write/flush time.
//
// Do NOT call this on a shared database (proper locking is not performed).
*/

	ERRORCODE deallocate_table_storage(void);

/*
// ReleaseSubrecords()
//
// Release all subrecords for any records we manage.
*/

	void ReleaseSubrecords(void);

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the table locator.
*/

	ERRORCODE	ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// Write the table locator.
*/

	ERRORCODE	WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// Return the size of the table.
*/

	ST_MAN_SIZE SizeofData(StorageDevicePtr device) { return sizeof(file_entry) + sizeof(next_locator_position); }
};

/*
// A piece of the table pointer chain.
*/

/*
// The table manager.
//
// This class has the responsibility of managing the record table
// (or directory) in the database file. It handles allocating and
// deallocating record numbers, as well as keeping track of file and memory
// entries for the resulting record. It does NOT talk to the records
// themselves; it merely manages the storage and retrieval of the table
// entries so the database can manage the records.
//
// This is the object a Database (or whomever else wants us) sees.
// All lower-level details should remain hidden.
*/

FARCLASS TableManager
{
protected:
	StorageDevicePtr const device;
	StorageManagerPtr const storage_manager;
	BOOL initialized;
	List locator_list;

	struct TBL_MAN_STATE
	{
		ST_DEV_POSITION	first_locator_position;
	} state;
	RW_Semaphore m_sem;

/* Positions of state and state semaphore things. */

	ST_DEV_POSITION m_state_position;
	ST_DEV_POSITION m_sem_position;

/* Refresh the state. */

	ERRORCODE refresh_state(void);

/* Flush the state. */

	ERRORCODE flush_state(void);

/* Read lock the state. */

	ERRORCODE read_lock_state(void);

/* Read unlock the state. */

	ERRORCODE read_unlock_state(void);

/* Modify lock the state. */

	ERRORCODE modify_lock_state(void);

/* Modify unlock the state. */

	ERRORCODE modify_unlock_state(void);

/* Split a record number into table and entry numbers. */

	VOID split_record_number(DB_RECORD_NUMBER number, TABLE_NUMBER *table, RECORD_INDEX *entry);

/* Bulid a record number from table and entry numbers. */

	DB_RECORD_NUMBER build_record_number(TABLE_NUMBER table, RECORD_INDEX entry);

/* Split a table number into locator and entry numbers. */

	VOID split_table_number(TABLE_NUMBER table, LOCATOR_NUMBER *locator, TABLE_INDEX *entry);

/* Bulid a record number from table and entry numbers. */

	TABLE_NUMBER build_table_number(LOCATOR_NUMBER locator, TABLE_INDEX entry);

/*
// Allocate a new locator.
// The locator is left read-locked.
*/

	ERRORCODE new_locator(LOCATOR_NUMBER which, TableLocatorPtr far *locator);

/*
// Get a locator.
// The locator is left read-locked.
*/

	ERRORCODE get_locator(LOCATOR_NUMBER which, TableLocatorPtr far *locator);

/* Attach a locator to the table manager. */

	VOID attach_locator(TableLocatorPtr locator);

public:

/*
// The constructor for a table manager.
*/

	TableManager(StorageManagerPtr st_man)
			: locator_list(0, TRUE), device(st_man->m_pDevice), storage_manager(st_man), initialized(FALSE) { }

/*
// The destructor for a table manager.
*/

	~TableManager();

/*
// initialize()
//
// Initialize the table manager.
// This is done in lieu of calling read_information().
// The table manager will be initialized (and usable) after this call.
*/

	ERRORCODE initialize(ST_DEV_POSITION state_offset);

/*
// reinitialize()
//
// Reinitialize the table manager.
// This frees all records and reinitializes the storage manager.
*/

	ERRORCODE reinitialize(void);

/*
// size_state()
//
// Return the size of the table manager state information as it resides
// on the storage device.
// This call works even if the table manager is not initialized.
*/

	ST_DEV_IO_SIZE size_state(VOID);

/*
// read_state()
//
// Read table manager state information from the storage device.
// This is done in lieu of calling initialize(), and returns the
// table manager to the state it was in when the information was saved.
// The table manager will be initialized (and usable) after this call.
*/

	ERRORCODE read_state(BOOL validate = FALSE, BOOL reset_semaphores = TRUE);

/*
// write_state()
//
// Write table manager state information to the storage device.
// This is done so that the table manager can be returned to this state
// at a later time.
*/

	ERRORCODE write_state();

/*
// flush()
//
// Write anything which needs to be written to the StorageDevice.
*/

	ERRORCODE flush();

/*
// allocate_entry()
//
// Allocate an entry in the record table.
*/

	ERRORCODE allocate_entry(DB_RECORD_NUMBER far *number, DB_RECORD_TYPE type = DB_ANY_TYPE);

/*
// deallocate_entry()
//
// Deallocate an entry in the record table.
*/

	ERRORCODE deallocate_entry(DB_RECORD_NUMBER number, DB_RECORD_TYPE type = DB_ANY_TYPE, BOOL record_is_locked = FALSE);

/*
// lock_entry()
//
// Lock a table entry.
*/

	ERRORCODE lock_entry(DB_RECORD_NUMBER number, RECORD_FILE_ENTRY far * far *fentry, RECORD_MEMORY_ENTRY far * far *mentry, BOOL read_only = FALSE);

/*
// modified_entry()
//
// Mark a table entry as being modified.
// It's considered bad form to mark an entry as modified which is not locked.
*/

	ERRORCODE modified_entry(DB_RECORD_NUMBER number);

/*
// unlock_entry()
//
// Unlock a table entry.
*/

	ERRORCODE unlock_entry(DB_RECORD_NUMBER number, BOOL read_only = FALSE, BOOL unlock_data = TRUE);

/*
// Reread the storage manager's device assuming it changed.
// This will fail if the table manager has been initialized.
*/

	ERRORCODE force_new_device(StorageDevicePtr new_device = NULL);

/*
// Deallocate all locators and tables.
// This merely frees the *storage* for them. They will automatically be
// allocated storage at write/flush time.
//
// This should NOT be called on a shared database (locking is not performed).
*/

	ERRORCODE deallocate_table_storage(void);

/*
// ReleaseSubrecords()
//
// Release all subrecords for any records we manage.
*/

	void ReleaseSubrecords(void);

};

typedef TableManager far *TableManagerPtr;

#define	LOCATORS_PER_DATABASE	32767

#endif						/* #ifndef __TABLEMAN_H__ */

