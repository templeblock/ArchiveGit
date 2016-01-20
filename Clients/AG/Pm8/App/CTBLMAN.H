/*
// $Header: /PM8/App/CTBLMAN.H 1     3/03/99 6:04p Gbeddow $
//
// Table manager definitions.
// The table manager manages the "record table" in the database.
// From the database's perspective, there is one large table (holding all
// record entries). The actual implementation is different (for obvious
// reasons).
//
// The definitions for CRecordTable, CTableLocator,
// and CTableManager are in this file.
//
// $Log: /PM8/App/CTBLMAN.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:26:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:40   FRED
// Initial revision.
// 
//    Rev 2.3   06 Jul 1995 09:37:36   JAY
// FlushRecord
// 
//    Rev 2.2   05 Jul 1995 11:14:38   JAY
// Memory freeing arg to Flush...
// 
//    Rev 2.1   08 Feb 1995 13:39:20   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:57:00   FRED
//  
// 
//    Rev 1.1   01 Nov 1994 09:59:44   JAY
// New data manager routines
// 
//    Rev 1.0   25 Oct 1994 16:24:48   JAY
// Initial revision.
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

class CRecordTable;
class CTableLocator;
class CTableManager;

#ifndef __CTBLMAN_H__
#define __CTBLMAN_H__

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

#include "crecord.h"			/* We need storage manager definitions. */
#include "list.h"					/* We need list definitions. */
#include "rwsem.h"

#ifndef RECORDS_PER_TABLE
//
// Common definitions with TableManager (tableman.h)
//

/*
// How many records are in each record table.
// (In Avagio 2.0, this was 256.)
*/

#define	RECORDS_PER_TABLE		64

#define	TABLES_PER_LOCATOR	16			/* 16 tables hold 1024 records */

#define	LOCATORS_PER_DATABASE	32767

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
// When a table's position is unknown, its position is -1.
// When a table has not yet been allocated in the file, its position is 0.
*/

#define	TABLE_POSITION_UNKNOWN		(-1L)
#define	TABLE_POSITION_UNALLOCATED	(0L)

#endif					// #ifndef RECORDS_PER_TABLE

/*
// A record table entry.
*/

struct CRECORD_FILE_ENTRY
{
	DATATYPE		type;					/* The type of this record */
	ST_DEV_POSITION	position;			/* Offset in storage file */
};

/*
// A record in-memory entry.
*/

struct CRECORD_MEMORY_ENTRY
{
	CStandardDataRecord*	data;						/* In-memory pointer */
	SHORT						lock_count;
};

/*
// A record (sub)table.
// This object is a storage element, and is managed by the storage manager.
*/

class CRecordTable : public StorageElement
{
protected:
	SHORT		used_record_count;					/* How many records used? */
	SHORT		locked_record_count;					/* How many records locked? */

/*
// The file entry portion.
// Only this part is actually written to the storage device.
*/

	CRECORD_FILE_ENTRY		file_entry[RECORDS_PER_TABLE];

/*
// The memory entry portion.
// This part manages the "in-memory" half of the records.
*/

	CRECORD_MEMORY_ENTRY	memory_entry[RECORDS_PER_TABLE];

/*
// FreeAllRecords()
//
// Free the memory for all records in the table.
// This is used by the destructor and the read method.
*/

	void FreeAllRecords(BOOL force = TRUE);

public:

/*
// The constructor for a CRecordTable.
*/

	CRecordTable(void);
	virtual void Init(TABLE_NUMBER id, StorageManagerPtr manager);

/*
// The destructor for a CRecordTable.
*/

	~CRecordTable();

/*
// AllocateEntry()
//
// Allocate an entry in the table. The index is returned.
*/

	ERRORCODE AllocateEntry(RECORD_INDEX far *record_number, DATATYPE type = DB_ANY_TYPE);

/*
// DeallocateEntry()
//
// Deallocate an entry in the table.
*/

	ERRORCODE DeallocateEntry(RECORD_INDEX record_number, DATATYPE type = DB_ANY_TYPE);

/*
// GetEntry()
//
// Return references to a record entry.
*/

	ERRORCODE GetEntry(RECORD_INDEX record_number, CRECORD_FILE_ENTRY far * far *fentry, CRECORD_MEMORY_ENTRY far * far *mentry);

/*
// Flush()
//
// Write anything which needs to be written to the StorageDevice.
// This would include all modified DatabaseRecords in memory.
*/

	ERRORCODE Flush(ST_DEV_POSITION far *where, BOOL fFreeAll = FALSE);

/*
// FlushRecord()
//
// Flush a record if the record has been modified.
*/

	ERRORCODE FlushRecord(CRECORD_FILE_ENTRY far *fentry, CRECORD_MEMORY_ENTRY far *mentry, BOOL fFreeAll = FALSE);

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

typedef CRecordTable far *CRecordTablePtr;

/*
// The storage management component of a CTableLocator.
*/

typedef struct
{
	ST_DEV_POSITION	position;
} CTABLE_FILE_ENTRY;

/*
// The memory management component of a CTableLocator.
*/

typedef struct
{
	CRecordTablePtr		table;
} CTABLE_MEMORY_ENTRY;

/*
// The table locator (directory) class.
// This object is a storage element, and is managed by the storage manager.
*/

typedef class CTableLocator far *CTableLocatorPtr;

class CTableLocator : public StorageElement, public ListElement
{
	friend class CTableManager;
protected:
/*
// How many entries have been allocated?
*/

	SHORT						tables_in_use;

/*
// The file entry portion.
// Only this part is actually written to the storage device.
*/

	CTABLE_FILE_ENTRY		file_entry[TABLES_PER_LOCATOR];

/*
// The memory entry portion.
// The part manages the "in-memory" half of the locators.
*/

	CTABLE_MEMORY_ENTRY		memory_entry[TABLES_PER_LOCATOR];

/*
// The address of the next locator in the storage device.
*/

	ST_DEV_POSITION	next_locator_position;

/*
// BuildTableNumber()
//
// Build a table number for this locator. The passed number must be a valid
// table index (0 to TABLES_PER_LOCATOR-1) or a bogus table number will
// be generated.
*/

	TABLE_NUMBER BuildTableNumber(TABLE_INDEX table_index) { return Id()*TABLES_PER_LOCATOR + table_index; }

/*
// GetTableIndex()
//
// Convert a table number into a table index within this locator.
// If the table doesn't belong to this locator, -1 is returned to indicate
// an error.
*/

	TABLE_INDEX GetTableIndex(TABLE_NUMBER table_number);

/*
// FreeAllTables()
//
// Free the memory for all tables in the locator.
// This is used by the destructor and the read method.
*/

	void FreeAllTables(void);

public:

/*
// The constructor for a table locator.
*/

	CTableLocator(void);

	virtual void Init(LOCATOR_NUMBER id, StorageManagerPtr manager);

/*
// The destructor for a TableLocator.
*/

	~CTableLocator();

/*
// NewTable()
//
// Create a new table in the locator.
// A pointer to the table is returned.
*/

	ERRORCODE NewTable(CRecordTablePtr far *table);

/*
// DeleteTable()
//
// Delete a table in the locator. The table is not allocated anymore.
// Due to the nature of how tables are allocated, it's considered an error
// to attempt to free a table that is not the last table allocated in the 
// locator.
*/

	ERRORCODE DeleteTable(CRecordTablePtr table);

/*
// FreeTable()
//
// Free the memory associated with a table preserving the table contents.
// The table is written if it has been modified.
*/

	ERRORCODE FreeTable(CRecordTablePtr table);

/*
// GetTable()
//
// Get an existing table.
// If the table does not exist, an error is returned.
// 'which' should be in the correct range.
*/

	ERRORCODE GetTable(TABLE_INDEX which, CRecordTablePtr far *table);

/*
// Flush()
//
// Write anything which needs to be written to the StorageDevice.
// This will include all modified tables.
*/

	ERRORCODE Flush(ST_DEV_POSITION far *where, BOOL fFreeAll = FALSE);

/*
// SetNextLocator()
//
// Set the position of the next table locator.
*/

	void SetNextLocator(ST_DEV_POSITION next)
		{ next_locator_position = next; ElementModified(); }

/*
// DeallocateTableStorage()
//
// Run through and deallocate the storage for all tables in this locator.
// The storage will automatically be reallocated at write/flush time.
//
// Do NOT call this on a shared database (proper locking is not performed).
*/

	ERRORCODE DeallocateTableStorage(void);

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

class CTableManager
{
protected:
	StorageDevicePtr const m_pDevice;
	StorageManagerPtr const m_pStorageManager;
	BOOL m_fInitialized;
	List m_LocatorList;

	struct CTBL_MAN_STATE
	{
		ST_DEV_POSITION	first_locator_position;
	} m_State;
	RW_Semaphore m_Sem;

/* Positions of state and state semaphore things. */

	ST_DEV_POSITION m_StatePosition;
	ST_DEV_POSITION m_SemPosition;

/* Refresh the state. */

	ERRORCODE RefreshState(void);

/* Flush the state. */

	ERRORCODE FlushState(void);

/* Read lock the state. */

	ERRORCODE ReadLockState(void);

/* Read unlock the state. */

	ERRORCODE ReadUnlockState(void);

/* Modify lock the state. */

	ERRORCODE ModifyLockState(void);

/* Modify unlock the state. */

	ERRORCODE ModifyUnlockState(void);

/* Split a record number into table and entry numbers. */

	VOID SplitRecordNumber(DATAID number, TABLE_NUMBER *table, RECORD_INDEX *entry);

/* Bulid a record number from table and entry numbers. */

	DATAID BuildRecordNumber(TABLE_NUMBER table, RECORD_INDEX entry);

/* Split a table number into locator and entry numbers. */

	VOID SplitTableNumber(TABLE_NUMBER table, LOCATOR_NUMBER *locator, TABLE_INDEX *entry);

/* Bulid a record number from table and entry numbers. */

	TABLE_NUMBER BuildTableNumber(LOCATOR_NUMBER locator, TABLE_INDEX entry);

/*
// Allocate a new locator.
// The locator is left read-locked.
*/

	ERRORCODE NewLocator(LOCATOR_NUMBER which, CTableLocatorPtr far *locator);

/*
// Get a locator.
// The locator is left read-locked.
*/

	ERRORCODE GetLocator(LOCATOR_NUMBER which, CTableLocatorPtr far *locator);

/* Attach a locator to the table manager. */

	VOID AttachLocator(CTableLocatorPtr locator);

public:

/*
// The constructor for a table manager.
*/

	CTableManager(StorageManagerPtr st_man)
			: m_LocatorList(0, TRUE), m_pDevice(st_man->m_pDevice), m_pStorageManager(st_man), m_fInitialized(FALSE) { }

/*
// The destructor for a table manager.
*/

	~CTableManager();

/*
// Initialize()
//
// Initialize the table manager.
// This is done in lieu of calling read_information().
// The table manager will be initialized (and usable) after this call.
*/

	ERRORCODE Initialize(ST_DEV_POSITION state_offset);

/*
// Reinitialize()
//
// Reinitialize the table manager.
// This frees all records and reinitializes the storage manager.
*/

	ERRORCODE Reinitialize(void);

/*
// SizeState()
//
// Return the size of the table manager state information as it resides
// on the storage device.
// This call works even if the table manager is not initialized.
*/

	ST_DEV_IO_SIZE SizeState(VOID);

/*
// ReadState()
//
// Read table manager state information from the storage device.
// This is done in lieu of calling initialize(), and returns the
// table manager to the state it was in when the information was saved.
// The table manager will be initialized (and usable) after this call.
*/

	ERRORCODE ReadState(BOOL validate = FALSE, BOOL reset_semaphores = TRUE);

/*
// WriteState()
//
// Write table manager state information to the storage device.
// This is done so that the table manager can be returned to this state
// at a later time.
*/

	ERRORCODE WriteState();

/*
// Flush()
//
// Write anything which needs to be written to the StorageDevice.
*/

	ERRORCODE Flush(BOOL fFreeAll = FALSE);

/*
// AllocateEntry()
//
// Allocate an entry in the record table.
*/

	ERRORCODE AllocateEntry(DATAID far *number, DATATYPE type = DB_ANY_TYPE);

/*
// DeallocateEntry()
//
// Deallocate an entry in the record table.
*/

	ERRORCODE DeallocateEntry(DATAID number, DATATYPE type = DB_ANY_TYPE, BOOL record_is_locked = FALSE);

/*
// LockEntry()
//
// Lock a table entry.
*/

	ERRORCODE LockEntry(DATAID number, CRECORD_FILE_ENTRY far * far *fentry, CRECORD_MEMORY_ENTRY far * far *mentry, BOOL read_only = FALSE);

/*
// ModifiedEntry()
//
// Mark a table entry as being modified.
// It's considered bad form to mark an entry as modified which is not locked.
*/

	ERRORCODE ModifiedEntry(DATAID number);

/*
// FlushRecord()
//
// Flush a record to the storage file and release its memory if possible.
*/

	ERRORCODE FlushRecord(DATAID number);

/*
// UnlockEntry()
//
// Unlock a table entry.
*/

	ERRORCODE UnlockEntry(DATAID number, BOOL read_only = FALSE, BOOL unlock_data = TRUE);

/*
// Reread the storage manager's device assuming it changed.
// This will fail if the table manager has been initialized.
*/

	ERRORCODE ForceNewDevice(StorageDevicePtr new_device = NULL);

/*
// Deallocate all locators and tables.
// This merely frees the *storage* for them. They will automatically be
// allocated storage at write/flush time.
//
// This should NOT be called on a shared database (locking is not performed).
*/

	ERRORCODE DeallocateTableStorage(void);

};

typedef CTableManager far *CTableManagerPtr;

#endif						/* #ifndef __CTBLMAN_H__ */

