/*
// Table manager routines.
//
// This file contains methods for RecordTable and TableManager.
//
// Oct. 15, 1992 - Jay N. : Initial creation
*/

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "tableman.h"

/***********************/
/* RecordTable methods */
/***********************/

/*
// Initialize a record table.
*/

RecordTable::RecordTable(TABLE_NUMBER id, StorageManagerPtr manager)
{
	StorageElement::Init(ELEMENT_TYPE_TABLE, id, manager);
/* Initialize our status. */

	used_record_count = 0;
	locked_record_count = 0;

/* All records are unused. */

	memset(file_entry, 0, sizeof(file_entry));
	memset(memory_entry, 0, sizeof(memory_entry));
}

/*
// Free the memory for all records in the table.
// This is used by the destructor and the read method.
*/

void RecordTable::free_all_records(BOOL force)
{
	RECORD_INDEX i;
	RECORD_MEMORY_ENTRY far *mentry;
	RECORD_FILE_ENTRY far *fentry;

/* Run through and delete all records which are in memory. */

	for (i = 0, fentry = file_entry, mentry = memory_entry;
							i < RECORDS_PER_TABLE;
							i++, fentry++, mentry++)
	{
		DatabaseRecordPtr record;

		if (fentry->type == DB_NO_TYPE)
		{
		/* Just always NULL it out. */
			mentry->data = NULL;
		}
		else if ((record = mentry->data) != NULL
				&& (force || mentry->lock_count == 0))
		{
			delete record;
			mentry->data = NULL;
		}
#if 0
		else
		{
			od("Spared entry %d (lock count %d, data:%lx)\r\n",
				i,
				mentry->lock_count,
				mentry->data);
		}
#endif
	}
}

/*
// Release subrecords for all records in the table.
// This is used by the table manager destructor.
*/

void RecordTable::ReleaseSubrecords(void)
{
	RECORD_INDEX i;
	RECORD_MEMORY_ENTRY far *mentry;
	RECORD_FILE_ENTRY far *fentry;

/* Run through and release subrecords for all records which are in memory. */

	for (i = 0, fentry = file_entry, mentry = memory_entry;
							i < RECORDS_PER_TABLE;
							i++, fentry++, mentry++)
	{
		DatabaseRecordPtr record;

		if (fentry->type != DB_NO_TYPE
				&& (record = mentry->data) != NULL)
		{
			record->ReleaseSubrecords();
		}
	}
}

/*
// The destructor for a RecordTable.
*/

RecordTable::~RecordTable()
{
	free_all_records(TRUE);
}

/*
// Allocate a record entry in a table.
// It would be very bad to pass DB_NO_TYPE here, because entries are
// marked as unused by having type DB_NO_TYPE. Therefore, this generates
// the appropriate error.
*/

ERRORCODE RecordTable::allocate_entry(RECORD_INDEX far *record_number, DB_RECORD_TYPE type)
{
	ERRORCODE error;
	RECORD_FILE_ENTRY far *entry;
	RECORD_INDEX i;

/* Validate the type. */

	if (type == DB_NO_TYPE)
	{
		return ERRORCODE_NoType;
	}

/* See if we have room. */

	if (used_record_count == RECORDS_PER_TABLE)
	{
		return ERRORCODE_Full;
	}

/* We need to lock ourselves prior to making a potential modification. */

	if ((error = ModifyLock()) != ERRORCODE_None)
	{
		return error;
	}

/* Find the next open record. */

	for (i = 0, entry = file_entry; i < RECORDS_PER_TABLE; i++, entry++)
	{
		if (entry->type == DB_NO_TYPE)
		{
		/* Found an available record. */

			*record_number = i;

		/* Initialize the file entry. */

			entry->type = type;
			entry->position = POSITION_UNALLOCATED;

		/* Initialize the memory entry. */

			RECORD_MEMORY_ENTRY far *mentry = memory_entry+i;

			mentry->data = NULL;
			mentry->lock_count = 0;

		/* Update the table. */

			used_record_count++;
			ElementModified();

		/* Return the found number. */

			ModifyUnlock();
			return ERRORCODE_None;
		}
	}

/*
// Table is inconsistent.
// Correct it now.
*/

	used_record_count = RECORDS_PER_TABLE;
	ElementModified();

	ModifyUnlock();
	return ERRORCODE_Full;
}

/*
// Deallocate a record entry in a table.
*/

ERRORCODE RecordTable::deallocate_entry(RECORD_INDEX record_number, DB_RECORD_TYPE type)
{
	ERRORCODE error;
	RECORD_FILE_ENTRY far &fentry = file_entry[record_number];
	RECORD_MEMORY_ENTRY far &mentry = memory_entry[record_number];

/* See if this is an allocated entry. */

	if (fentry.type == DB_NO_TYPE)
	{
		return ERRORCODE_NotAllocated;
	}

	if (type != DB_ANY_TYPE && fentry.type != type)
	{
		ASSERT(FALSE);
		return ERRORCODE_TypeMismatch;
	}
/* See if this entry is locked. A lock of one is allowed. */

	if (mentry.lock_count > 1)
	{
		return ERRORCODE_Locked;
	}

/* Lock with the intention of modifying. */

	if ((error = ModifyLock()) != ERRORCODE_None)
	{
		return error;
	}

/* If this entry has a record attached, free the record. */

#if 0
	if (mentry.data != NULL)
	{
		delete mentry.data;
		mentry.data = NULL;
	}
#else
/* This entry is now unoccupied. */
	mentry.data = NULL;
#endif

/* Time to free us. */

	if (fentry.position != POSITION_UNALLOCATED)
	{
		manager->FreeStorage(fentry.position);
	}

	fentry.type = DB_NO_TYPE;			/* Now freed! */
	ElementModified();
	used_record_count--;					/* One less entry used */

/* And now we're done... */

	ModifyUnlock();

	return ERRORCODE_None;
}

/*
// Return pointers to the desired record entry fields.
// The table is assumed to be at least read-locked.
*/

ERRORCODE RecordTable::get_entry(RECORD_INDEX record_number, RECORD_FILE_ENTRY far * far *fentry, RECORD_MEMORY_ENTRY far * far *mentry)
{
/* Validate the record number. */

	if (record_number < 0 || record_number >= RECORDS_PER_TABLE)
	{
		return ERRORCODE_InvalidRecord;
	}

	*fentry = file_entry + record_number;
	*mentry = memory_entry + record_number;

/* Make sure we're allocated. */

	if ((*fentry)->type == DB_NO_TYPE)
	{
		return ERRORCODE_NotAllocated;
	}

/* Got the entry OK. */

	return ERRORCODE_None;
}

/*
// Flush a record of the record table.
*/

ERRORCODE RecordTable::flush_record(RECORD_FILE_ENTRY far *fentry, RECORD_MEMORY_ENTRY far *mentry)
{
	ERRORCODE error = ERRORCODE_None;
	DatabaseRecordPtr record;

	if (fentry->type != DB_NO_TYPE
			&& (record = mentry->data) != NULL
			&& record->IsElementModified())
	{
		ST_DEV_POSITION old_position = record->Position();

	/* Write the record. */

		if ((error = record->write()) != ERRORCODE_None)
		{
			return error;
		}

	/* See if the table has been modified by this action. */

		if (old_position != record->Position())
		{
			ElementModified();
		}
	}

	return error;
}

/*
// Flush this table to disk.
*/

ERRORCODE RecordTable::flush(ST_DEV_POSITION far *where)
{
	RECORD_FILE_ENTRY far *fentry;
	RECORD_MEMORY_ENTRY far *mentry;
	ERRORCODE error = ERRORCODE_None;
	RECORD_INDEX i;

/* Run through and flush all records which are in memory. */

	for (i = 0, fentry = file_entry, mentry = memory_entry;
							i < RECORDS_PER_TABLE;
							i++, fentry++, mentry++)
	{
		if ((error = flush_record(fentry, mentry)) != ERRORCODE_None)
		{
			return error;
		}
	}

/* Write ourselves. */

	if (IsElementModified())
	{	
		error = Write(where);
	}

	return error;
}

/*
// Read this table from a device.
*/

ERRORCODE RecordTable::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

//	od("Read record table %d...\r\n", id());

/* Make sure all objects are detached. */

	free_all_records(FALSE);

	if ((error = device->read(file_entry, sizeof(file_entry))) == ERRORCODE_None)
	{
		RECORD_INDEX i;
		RECORD_FILE_ENTRY far *fentry;

	/* All records are not in memory. */
	/* We no longer do this since we may be re-reading. */
//		memset(memory_entry, 0, sizeof(memory_entry));
//		locked_record_count = 0;

	/* Count how many records are used. */

		for (i = RECORDS_PER_TABLE, fentry = file_entry, used_record_count = 0;
 							i--;
							fentry++)
		{
			if (fentry->type != DB_NO_TYPE)
			{
				used_record_count++;
			}
		}
	}
	return error;
}

/*
// Write this table to a device.
*/

ERRORCODE RecordTable::WriteData(StorageDevicePtr device)
{
/* Write our file entry portion. */
	return device->write(file_entry, sizeof(file_entry));
}

/************************/
/* TableLocator methods */
/************************/

/*
// The constructor for the table locator.
*/

TableLocator::TableLocator(LOCATOR_NUMBER id, StorageManagerPtr manager)
{
	StorageElement::Init(ELEMENT_TYPE_LOCATOR, id, manager);
/* All records are not allocated. */

	memset(file_entry, 0, sizeof(file_entry));
	memset(memory_entry, 0, sizeof(memory_entry));

/* No tables in use. */

	tables_in_use = 0;

/* No next table address yet. */

	next_locator_position = POSITION_UNALLOCATED;

/* We are indeed dirty. */

	ElementModified();
}

/*
// Free the memory for all tables in the locator.
// This is used by the destructor and the read method.
*/

void TableLocator::free_all_tables(void)
{
	TABLE_INDEX i;
	TABLE_MEMORY_ENTRY far *mentry;

/* Run through and delete all tables which are in memory. */

	for (i = 0, mentry = memory_entry;
							i < tables_in_use;
							i++, mentry++)
	{
		RecordTablePtr table;

		if ((table = mentry->table) != NULL)
		{
			delete table;
			mentry->table = NULL;
		}
	}
}

/*
// Release subrecords for all records of all tables in the locator.
// This is used by the destructor for the table manager.
*/

void TableLocator::ReleaseSubrecords(void)
{
	TABLE_INDEX i;
	TABLE_MEMORY_ENTRY far *mentry;

/* Run through and delete all tables which are in memory. */

	for (i = 0, mentry = memory_entry;
							i < tables_in_use;
							i++, mentry++)
	{
		RecordTablePtr table;

		if ((table = mentry->table) != NULL)
		{
			table->ReleaseSubrecords();
		}
	}
}

/*
// The destructor for a TableLocator.
*/

TableLocator::~TableLocator()
{
	free_all_tables();
}

/*
// Get the table index from a table number.
*/

TABLE_INDEX TableLocator::get_table_index(TABLE_NUMBER table_number)
{
/* See if this table belongs to this locator. */

	if ((table_number / TABLES_PER_LOCATOR) == Id())
	{
		return (TABLE_INDEX)(table_number % TABLES_PER_LOCATOR);
	}

/* Not a good table number. */

	return -1;
}

/*
// Create a new table in the locator.
// It is assumed that the locator is "modify_locked" at this point.
*/

ERRORCODE TableLocator::new_table(RecordTablePtr far *table)
{
/* See if we have any more room in this locator. */

	if (tables_in_use == TABLES_PER_LOCATOR)
	{
	/* No more room. */
		return ERRORCODE_Full;
	}

	TABLE_MEMORY_ENTRY far *mentry = memory_entry+tables_in_use;
	TABLE_FILE_ENTRY far *fentry = file_entry+tables_in_use;

/* We will use the next slot in the locator. */

	if ((mentry->table = new RecordTable(build_table_number(tables_in_use), manager)) == NULL)
	{
		return ERRORCODE_Memory;
	}

/* If we're sharing, we need to write the table now. */

	if (manager->IsSharing())
	{
		ERRORCODE error;

		if ((error = mentry->table->Write(&fentry->position)) != ERRORCODE_None)
		{
		/* Uh, oh! Couldn't write. Fail the operation. */
			delete mentry->table;
			mentry->table = NULL;
			return error;
		}
	}

/* Go out of here with a read lock. */

	mentry->table->ReadLock();

/* Successful. */

	tables_in_use++;

	ElementModified();

/* return the table. */

	*table = mentry->table;

	return ERRORCODE_None;
}

/*
// Delete a table from the locator.
// This must be the last table allocated in the locator.
*/

#if 0
ERRORCODE TableLocator::delete_table(RecordTablePtr table)
{
	TABLE_INDEX which_table;

/* Get the table index. */

	if ((which_table = get_table_index(table->id())) == -1)
	{
		return ERRORCODE_BadParameter;
	}

/* See if this is the last table in the locator. */

	if (which_table == tables_in_use-1)
	{
	/* Let's free this up. */

		TABLE_MEMORY_ENTRY far *mentry = memory_entry+which_table;
		TABLE_FILE_ENTRY far *fentry = file_entry+which_table;

	/* Free memory. */

		if (mentry->table != NULL)
		{
			delete mentry->table;
			mentry->table = NULL;
		}

	/* Free storage. */

		if (fentry->position != POSITION_UNALLOCATED)
		{
			manager->free_storage(fentry->position);
			fentry->position = POSITION_UNALLOCATED;
		}

	/* One table in use. */

		tables_in_use--;

	/* We're dirty. */

		element_modified();
		return ERRORCODE_None;
	}

/* Not the last table in the locator. */

	return ERRORCODE_IntError;
}
#endif

/*
// Free a table within the locator.
// This just returns the memory used by the table to the system.
*/

ERRORCODE TableLocator::free_table(RecordTablePtr table)
{
	TABLE_INDEX which_table;
	TABLE_MEMORY_ENTRY far *mentry;
	TABLE_FILE_ENTRY far *fentry;
	ERRORCODE error;

/* Get the table index. */

	if ((which_table = get_table_index(table->Id())) == -1)
	{
		return ERRORCODE_BadParameter;
	}

/* See if this is a good index. */

	if (which_table >= tables_in_use)
	{
		return ERRORCODE_BadParameter;
	}

/* Let's free this up. */

	mentry = memory_entry+which_table;
	fentry = file_entry+which_table;

/* Free memory. */

	if (table == mentry->table)
	{
		if (table->IsElementModified())
		{
			ST_DEV_POSITION old_position = fentry->position;

		/* Write the table since we are going to free it and it's dirty. */

			if ((error = table->Write(&fentry->position)) != ERRORCODE_None)
			{
				return error;
			}

		/* See if we need writing now. */

			if (old_position != fentry->position)
			{
				ElementModified();
			}
		}

	/* Delete the table. */

		delete table;
		mentry->table = NULL;
	}
	else
	{
	/* Table doesn't match what's in the locator. */
		return ERRORCODE_IntError;
	}

	return ERRORCODE_None;
}

/*
// Get a table from the locator.
// The table must exist and the locator must be at least read-locked.
*/

ERRORCODE TableLocator::get_table(TABLE_INDEX which_table, RecordTablePtr far *table_p)
{
/* Validate the table number. */

	if (which_table < 0 || which_table >= RECORDS_PER_TABLE)
	{
		return ERRORCODE_InvalidRecord;
	}

/* See if this is not allocated yet. */

	if (which_table >= tables_in_use)
	{
		return ERRORCODE_DoesNotExist;
	}

/* See if the table is here. */

	ERRORCODE error = ERRORCODE_None;
	TABLE_MEMORY_ENTRY far &mentry = memory_entry[which_table];
	TABLE_FILE_ENTRY far &fentry = file_entry[which_table];
	RecordTablePtr table;

	if ((table = mentry.table) == NULL)
	{
		ST_DEV_POSITION position;

	/* The table is not in memory. See if the table has been allocated. */

		if ((position = fentry.position) == POSITION_UNALLOCATED)
		{
		/*
 		// Uh, oh. Something's out of sync. There was supposedly a table here
		// according to the locator's tables_in_use field.
		// We could theoretically make the locator consistent at this point.
		// *** Something to consider ***
 		*/

			return ERRORCODE_IntError;
		}

	/* Allocate a table to read from file. */

		if ((table = new RecordTable(build_table_number(which_table), manager)) == NULL)
		{
			return ERRORCODE_Memory;
		}

	/* Read the table from file. */

		if ((error = table->Read(position)) != ERRORCODE_None)
		{
			delete table;
			return error;
		}

	/* Hook the table into the locator. */

		mentry.table = table;
	}

/* Sync with file if this is a sharing database. */

	if ((error = table->ReadLock()) != ERRORCODE_None)
	{
		return error;
	}

/* Pass back our found table. */

	*table_p = table;

	return error;
}

/*
// Flush this table locator to disk.
// It's assumed that the next_locator_position field has already been set.
*/

ERRORCODE TableLocator::flush(ST_DEV_POSITION far *where)
{
	TABLE_FILE_ENTRY far *fentry;
	TABLE_MEMORY_ENTRY far *mentry;
	TABLE_INDEX i;
	ERRORCODE error = ERRORCODE_None;

/* Run through and flush all tables which are in memory. */

	for (i = 0, fentry = file_entry, mentry = memory_entry;
							i < TABLES_PER_LOCATOR;
							i++, fentry++, mentry++)
	{
		RecordTablePtr table;

		if ((table = mentry->table) != NULL)
		{
			ST_DEV_POSITION old_position = table->Position();

		/* Flush the table. */

			if ((error = table->flush(&fentry->position)) != ERRORCODE_None)
			{
				return error;
			}

		/* See if we got modified. */

			if (old_position != table->Position())
			{
				ElementModified();
			}
		}
	}

/* Write ourselves if necessary. */

	if (IsElementModified())
	{
		error = Write(where);
	}

	return error;
}

/*
// deallocate_table_storage()
//
// Run through and deallocate the storage for all tables in this locator.
// The storage will automatically be reallocated at write/flush time.
//
// Do NOT call this on a shared database (proper locking is not performed).
*/

ERRORCODE TableLocator::deallocate_table_storage(void)
{
	TABLE_FILE_ENTRY far *fentry;
	TABLE_MEMORY_ENTRY far *mentry;
	TABLE_INDEX i;
	ERRORCODE error = ERRORCODE_None;

/* Run through and flush all tables which are in memory. */

	for (i = 0, fentry = file_entry, mentry = memory_entry;
							i < TABLES_PER_LOCATOR;
							i++, fentry++, mentry++)
	{
		RecordTablePtr table;

		if ((error = get_table(i, &table)) != ERRORCODE_None)
		{
			break;
		}

		if (fentry->position != POSITION_UNALLOCATED)
		{
			table->Deallocate();
			fentry->position = table->Position();
			ElementModified();
		}
		table->ReadUnlock();
	}
	return error;
}

/*
// Read this locator from a device.
*/

ERRORCODE TableLocator::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

/* Make sure all tables are freed before reading new. */

	free_all_tables();

/* Read the next locator position. */

	if ((error = device->read(&next_locator_position, sizeof(next_locator_position))) == ERRORCODE_None)
	{
	/* Read the table file entries. */

		if ((error = device->read(file_entry, sizeof(file_entry))) == ERRORCODE_None)
		{
			TABLE_FILE_ENTRY far *fentry;

		/* All records are not in memory. */

			memset(memory_entry, 0, sizeof(memory_entry));

		/* Count how many tables are in use. */

			for (tables_in_use = 0, fentry = file_entry;
						tables_in_use != TABLES_PER_LOCATOR && fentry->position != POSITION_UNALLOCATED;
						tables_in_use++, fentry++)
				;
		}
	}
	return error;
}

/*
// Write this locator to a device.
*/

ERRORCODE TableLocator::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;

/* Write the next position first. This may allow easier chaining later. */

	if ((error = device->write(&next_locator_position, sizeof(next_locator_position))) == ERRORCODE_None)
	{
	/* Write the file entries. */
		error = device->write(file_entry, sizeof(file_entry));
	}
	return error;
}

/************************/
/* TableManager methods */
/************************/

/*
// The destructor for a table manager.
*/

TableManager::~TableManager()
{
	ReleaseSubrecords();			// Release subrecords before list destructs.
}

/*
// Release subrecords for all records.
*/

void TableManager::ReleaseSubrecords(void)
{
	for (TableLocatorPtr pLocator = (TableLocatorPtr)locator_list.first_element();
					pLocator != NULL;
					pLocator = (TableLocatorPtr)pLocator->next_element())
	{
		pLocator->ReleaseSubrecords();
	}
}

/*
// Initialize the table manager.
// This is called in cases where a state is not available on the storage
// device (like in the case of a new file).
*/

ERRORCODE TableManager::initialize(ST_DEV_POSITION state_offset)
{
/* Make sure the locator list is empty. */

	locator_list.empty();

/* Set the first locator position to unknown. */

	state.first_locator_position = POSITION_UNALLOCATED;

	m_state_position = state_offset;

	if (storage_manager->IsSharing())
	{
	/* We need to init the semaphore. */

		m_sem_position = state_offset + device->size_record(sizeof(state));
		m_sem.Init(device, m_sem_position);

	/* We need to write the initial state which will be shared. */

		ERRORCODE error;

		if ((error = flush_state()) != ERRORCODE_None)
		{
			return error;
		}
	}

	initialized = TRUE;

	return ERRORCODE_None;
}

/*
// Initialize the table manager.
// This is called in cases where a state is not available on the storage
// device (like in the case of a new file).
*/

ERRORCODE TableManager::reinitialize(void)
{
	ERRORCODE error;

/* Step 1: Lock the state. */

	if ((error = modify_lock_state()) == ERRORCODE_None)
	{
	/* Step 2: Reinitialize the storage manager. (We have one lock.) */
		if ((error = storage_manager->Reinitialize(1)) == ERRORCODE_None)
		{
		/* Force the locator list to reinitialize. */
			locator_list.empty();

			state.first_locator_position = POSITION_UNALLOCATED;
		}
		modify_unlock_state();
	}
	return error;
}

/*
// Refresh the state.
*/

ERRORCODE TableManager::refresh_state(void)
{
	ERRORCODE error;
	SHORT size = device->size_record(sizeof(state));
	BOOL is_sharing = storage_manager->IsSharing();

/* Lock the area if we're sharing. */

	if (is_sharing)
	{
		if ((error = device->lock(m_state_position, size, FALSE)) != ERRORCODE_None)
		{
			return error;
		}
	}

/* Make sure the locator list is empty. */

	ST_DEV_POSITION old_first_locator = state.first_locator_position;

/* Do the read. */

	if ((error = device->seek(m_state_position, ST_DEV_SEEK_SET)) == ERRORCODE_None)
	{
		if ((error = device->read_record(&state, sizeof(state))) == ERRORCODE_None)
		{
			if (state.first_locator_position != old_first_locator)
			{
			/* Something drastic has happened. Throw everything away. */
				locator_list.empty();
			}
		}
	}

/* Unlock the area if we're sharing. */

	if (is_sharing)
	{
		device->lock(m_state_position, size, TRUE);
	}

	return error;
}

/*
// Flush the state.
*/

ERRORCODE TableManager::flush_state(void)
{
	ERRORCODE error;
	BOOL is_sharing = storage_manager->IsSharing();

	SHORT size = device->size_record(sizeof(state));

/* Lock the area if we're sharing. */

	if (is_sharing)
	{
		if ((error = device->lock(m_state_position, size, FALSE)) != ERRORCODE_None)
		{
			return error;
		}
	}

/* Do the write. */

	if ((error = device->seek(m_state_position, ST_DEV_SEEK_SET)) == ERRORCODE_None)
	{
		error = device->write_record(&state, sizeof(state));
	}

/* Unlock the area if we're sharing. */

	if (is_sharing)
	{
		device->lock(m_state_position, size, TRUE);
	}

	return error;
}

/*
// Read the table manager state from the current position in the
// storage device.
*/

ERRORCODE TableManager::read_state(BOOL validate, BOOL reset_semaphores)
{
	ERRORCODE error;

/* Remember the state position. */

	device->tell(&m_state_position);

/* Read our state. */

	if ((error = refresh_state()) == ERRORCODE_None)
	{
		if (storage_manager->IsSharing())
		{
			device->tell(&m_sem_position);

			if (validate && reset_semaphores)
			{
			/* Reset any read locks on the semaphore. */
				m_sem.Init(device, m_sem_position);
			}

			device->seek(m_sem_position + m_sem.Size(), ST_DEV_SEEK_SET);
		}

		initialized = TRUE;
	}
	return error;
}

/*
// Write the table manager state to the current position in the
// storage device. The table manager must be initialized.
*/

ERRORCODE TableManager::write_state()
{
/* Make sure we're initialized. */

	if (!initialized)
	{
		return ERRORCODE_NotInitialized;
	}

/* Write our state. */

	/*
	// 8/2/95 (JN)
	// It would be bad news to always write the state on exit.
	// In the sharing case, we may not have updated the state recently; we
	// may be writing old information back. So we skip over it in the
	// that case, letting the normal modify_unlock_state() code handle writing
	// it when needed.
	// This could cause serious problems if three or more people are sharing
	// this device at once.
	*/

	ERRORCODE error;
	if (storage_manager->IsSharing())
	{
	/* Skip over the semaphore. */
		error = device->seek(size_state(), ST_DEV_SEEK_CUR);
	}
	else
	{
		error = flush_state();
	}
	return error;
}

ST_DEV_IO_SIZE TableManager::size_state(VOID)
{
	ST_DEV_IO_SIZE size = device->size_record(sizeof(state));

	if (storage_manager->IsSharing())
	{
		size += m_sem.Size();
	}
	return size;
}

/*
// Read lock the state.
*/

ERRORCODE TableManager::read_lock_state(void)
{
	ERRORCODE error = ERRORCODE_None;

	if (storage_manager->IsSharing())
	{
	/* Read lock the semaphore. */

		if ((error = m_sem.ReadLock(device, m_sem_position, storage_manager->IsExclusive())) != ERRORCODE_None)
		{
			return error;
		}
		storage_manager->AddedLock();

	/* If the state changed, refresh it now. */

		if (m_sem.Changed())
		{
			if ((error = refresh_state()) != ERRORCODE_None)
			{
				m_sem.ReadUnlock(device, m_sem_position, storage_manager->IsExclusive());
				storage_manager->RemovedLock();
			}
		}
	}
	return error;
}

/*
// Read unlock the state.
*/

ERRORCODE TableManager::read_unlock_state(void)
{
	ERRORCODE error = ERRORCODE_None;
	if (storage_manager->IsSharing())
	{
		error = m_sem.ReadUnlock(device, m_sem_position, storage_manager->IsExclusive());
		storage_manager->RemovedLock();
	}
	return error;
}

/*
// Mofidy lock the state.
*/

ERRORCODE TableManager::modify_lock_state(void)
{
	ERRORCODE error = ERRORCODE_None;

	if (storage_manager->IsSharing())
	{
	/* Modify lock the semaphore. */

		if ((error = m_sem.ModifyLock(device, m_sem_position, storage_manager->IsExclusive())) != ERRORCODE_None)
		{
			return error;
		}
		storage_manager->AddedLock();

	/* If the state changed, refresh it now. */

		if (m_sem.Changed())
		{
			if ((error = refresh_state()) != ERRORCODE_None)
			{
				m_sem.ModifyUnlock(device, m_sem_position, FALSE, storage_manager->IsExclusive());
				storage_manager->RemovedLock();
			}
		}
	}
	return error;
}

/*
// Modify unlock the state.
*/

ERRORCODE TableManager::modify_unlock_state(void)
{
	if (storage_manager->IsSharing())
	{
		if (m_sem.GetModifyLocks() == 1)
		{
			flush_state();
		}
		m_sem.ModifyUnlock(device, m_sem_position, TRUE, storage_manager->IsExclusive());
		storage_manager->RemovedLock();
	}
	return ERRORCODE_None;
}

/*
// Flush all table manager data to the disk.
// In this implementation, this involves writing all tables to the storage
// device (allocating them as necessary).
*/

ERRORCODE TableManager::flush()
{
	TableLocatorPtr locator;
	TableLocatorPtr last_locator;
	ST_DEV_POSITION far *where;
	ERRORCODE error = ERRORCODE_None;

	if (!initialized)
	{
		return ERRORCODE_NotInitialized;
	}

/*
// Make sure all the table locators are allocated by the storage manager.
// We need to do this because the locators are written in the opposite order
// from which they need to be allocated (since each locator holds the position
// of the NEXT table, it can't be written until the next table is allocated).
*/

	last_locator = NULL;

	for (where = &state.first_locator_position, locator = (TableLocatorPtr)locator_list.first_element();
					locator != NULL;
					where = &locator->next_locator_position,
						last_locator = locator,
						locator = (TableLocatorPtr)locator->next_element())
	{
		if (*where == POSITION_UNALLOCATED)
		{
		/* We're unallocated. Preallocate us. */

			if ((error = locator->Allocate(locator->Type(), locator->Id())) != ERRORCODE_None)
			{
				return error;
			}
			*where = locator->Position();
			if (last_locator != NULL)
			{
				last_locator->ElementModified();
			}
		}
	}

/*
// Flush all the table locators.
// This should cause the locators to be written if modified.
// No position changes should occur because locators are always the same size.
*/

	for (where = &state.first_locator_position, locator = (TableLocatorPtr)locator_list.first_element();
					locator != NULL;
					where = &locator->next_locator_position, locator = (TableLocatorPtr)locator->next_element())
	{
		if ((error = locator->flush(where)) != ERRORCODE_None)
		{
			return error;
		}
	}

	return error;
}

/*
// Split a record number.
*/

VOID TableManager::split_record_number(DB_RECORD_NUMBER number, TABLE_NUMBER *table, RECORD_INDEX *entry)
{
/* Go to zero based from one based. */

	number--;

/* Break it down. */

	*table = number / RECORDS_PER_TABLE;
	*entry = (RECORD_INDEX)(number % RECORDS_PER_TABLE);
}

/*
// Build a record number.
*/

DB_RECORD_NUMBER TableManager::build_record_number(TABLE_NUMBER table, RECORD_INDEX entry)
{
	return (table * RECORDS_PER_TABLE) + entry + 1;
}

/*
// Split a table number.
*/

VOID TableManager::split_table_number(TABLE_NUMBER table, LOCATOR_NUMBER *locator, TABLE_INDEX *entry)
{
/* Break it down. */

	*locator = (LOCATOR_NUMBER)(table / TABLES_PER_LOCATOR);
	*entry = (TABLE_INDEX)(table % TABLES_PER_LOCATOR);
}

/*
// Build a table number.
*/

TABLE_NUMBER TableManager::build_table_number(LOCATOR_NUMBER locator, TABLE_INDEX entry)
{
	return (locator * TABLES_PER_LOCATOR) + entry;
}

/*
// Attach a locator to the table manager.
// Locators are always added at the end of the chain.
*/

VOID TableManager::attach_locator(TableLocatorPtr locator)
{
	locator_list.append(locator);
}

/*
// Allocate a new locator.
// The locator is hooked into the table manager, and it's left read-locked.
*/

ERRORCODE TableManager::new_locator(LOCATOR_NUMBER which, TableLocatorPtr far *locator_p)
{
	TableLocatorPtr locator;
	BOOL is_sharing = storage_manager->IsSharing();
	TableLocatorPtr previous_locator;
	ERRORCODE error;

/*
// If this is a shared file, we need to lock the locator which will be
// getting a pointer to this locator.
*/

	if (is_sharing)
	{
	/* Do the lock. */

		SHORT count = locator_list.count();

		if (count == 0)
		{
		/*
		// There is no last locator.
		// We need to lock the front of the locator list.
		*/
			previous_locator = NULL;
			if ((error = modify_lock_state()) != ERRORCODE_None)
			{
				return error;
			}
		}
		else
		{
			if ((error = get_locator(count-1, &previous_locator)) != ERRORCODE_None)
			{
			/* Try to modify lock the locator. */

				error = previous_locator->ModifyLock();

			/* Always read_unlock the locator. */

				previous_locator->ReadUnlock();
			}

			if (error != NULL)
			{
				return error;
			}
		}
	}

/* Allocate the locator. */

	if ((locator = new TableLocator(which, storage_manager)) == NULL)
	{
		error = ERRORCODE_Memory;
	}
	else
	{
		error = ERRORCODE_None;

		if (is_sharing)
		{
		/* We need to allocate and write the locator NOW! */

			if ((error = locator->Write()) == ERRORCODE_None)
			{
			/* Success. Hook it in. */
				if (previous_locator == NULL)
				{
				/* Update the header area. */
					state.first_locator_position = locator->Position();
				}
				else
				{
				/* Update the previouos locator. */
					previous_locator->set_next_locator(locator->Position());
				}
			/* And put a read lock on this for returning. */
				locator->ReadLock();
			}
			else
			{
				delete locator;
			}
		}
	}

/* Unlock what we locked. */

	if (is_sharing)
	{
		if (previous_locator == NULL)
		{
		/* Update the header area. */
			modify_unlock_state();
		}
		else
		{
		/* Update the previous locator. */
			previous_locator->ModifyUnlock();
		}
	}

/* If something failed, return now. */

	if (error != ERRORCODE_None)
	{
		return error;
	}

/* Attach the locator to the table manager. */

	attach_locator(locator);

/* Pass back the pointer we found. */

	*locator_p = locator;

	return ERRORCODE_None;
}

/*
// Get a locator, reading it into memory if necessary.
// The locator is left read-locked.
*/

ERRORCODE TableManager::get_locator(LOCATOR_NUMBER which, TableLocatorPtr far *locator_p)
{
	TableLocatorPtr locator;
	LOCATOR_NUMBER i;
	ST_DEV_POSITION far *where;
	ERRORCODE error = ERRORCODE_None;

/* Lock the locator chain so we can move through it. */

	if ((error = read_lock_state()) != ERRORCODE_None)
	{
		return error;
	}

/* Find the locator. Read it from the file if necessary. */

	for (i = 0, where = &state.first_locator_position, locator = (TableLocatorPtr)locator_list.first_element();;
					where = &locator->next_locator_position, locator = (TableLocatorPtr)locator->next_element(), i++)
	{
		if (locator == NULL)
		{
		/* The locator is not in memory. See if it's in the file. */

			if (*where == POSITION_UNALLOCATED)
			{
			/* Not in the file either. Bad record number. */
				read_unlock_state();
				return ERRORCODE_DoesNotExist;
			}

		/* We need to create a new locator. */

			if ((locator = new TableLocator(i, storage_manager)) == NULL)
			{
				read_unlock_state();
				return ERRORCODE_Memory;
			}

		/* Read the locator. */

			if ((error = locator->Read(*where)) != ERRORCODE_None)
			{
				delete locator;
				read_unlock_state();
				return error;
			}

		/* Attach the locator to the table manager. */

			attach_locator(locator);
		}

	/* Try to read lock the header as necessary. */

		if ((error = locator->ReadLock()) != ERRORCODE_None)
		{
			read_unlock_state();
			return error;
		}

	/* See if this is the one we want. */

		if (locator->Id() == which)
		{
		/* We leave it "read locked" since we're returning it. */
			break;
		}

	/* We don't need to lock anymore. */

		locator->ReadUnlock();
	}

/* Pass back what we found. */

	*locator_p = locator;

	read_unlock_state();
	return error;
}

/*
// Allocate an entry in the TableManager's record table.
*/

ERRORCODE TableManager::allocate_entry(DB_RECORD_NUMBER far *number, DB_RECORD_TYPE type)
{
	LOCATOR_NUMBER locator_index;
	ERRORCODE error;

	for (locator_index = 0; locator_index < LOCATORS_PER_DATABASE; locator_index++)
	{
		TableLocatorPtr locator;

	/* Get this locator. */

		if ((error = get_locator(locator_index, &locator)) != ERRORCODE_None)
		{
		/* See if this is an bad error. */
			if (error != ERRORCODE_DoesNotExist)
			{
			/* Bad error. Go home. */
				return error;
			}

		/* We need to add a locator. */

			if ((error = new_locator(locator_index, &locator)) != ERRORCODE_None)
			{
				return error;
			}
		}

	/* Lock the locator in case we need to modify it. */

		error = locator->ModifyLock();

	/* Always "read unlock" the locator. */

		locator->ReadUnlock();

	/* If we weren't able to modify lock this, go home. */

		if (error != ERRORCODE_None)
		{
			return error;
		}

	/* Now we have the locator. */

		for (TABLE_INDEX table_index = 0; table_index < TABLES_PER_LOCATOR; table_index++)
		{
			RecordTablePtr table;

			if ((error = locator->get_table(table_index, &table)) != ERRORCODE_None)
			{
			/* See if this is a bad error. */

				if (error != ERRORCODE_DoesNotExist)
				{
				/* Bad error. Go home. */
					locator->ModifyUnlock();
					return error;
				}

			/* We need to add a table. */

				if ((error = locator->new_table(&table)) != ERRORCODE_None)
				{
					locator->ModifyUnlock();
					return error;
				}
			}

		/* We have the table. Allocate an entry. */

			RECORD_INDEX record_index;

			error = table->allocate_entry(&record_index, type);

		/* We're done reading. Unlock the table. */

			table->ReadUnlock();

			if (error == ERRORCODE_None)
			{
				*number = build_record_number(table->Id(), record_index);
				locator->ModifyUnlock();
				return ERRORCODE_None;
			}

		/* See if this was a bad error. */

			if (error != ERRORCODE_Full)
			{
				locator->ModifyUnlock();
				return error;
			}

		/* That table was full. Move to the next table. */
		}

		locator->ModifyUnlock();

	/* That locator was full. Move to the next locator. */
	}

/* All full! No more records! This really should never happen! */

	return ERRORCODE_Full;
}

/*
// Deallocate an entry in the TableManager's record table.
*/

ERRORCODE TableManager::deallocate_entry(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, BOOL record_is_locked)
{
	TABLE_NUMBER table_number;
	TABLE_INDEX which_table;
	LOCATOR_NUMBER which_locator;
	RECORD_INDEX which_entry;
	ERRORCODE error;
	TableLocatorPtr locator;
	RecordTablePtr table;

/* See if this is a valid record number. */

	if (number == 0)
	{
		return ERRORCODE_ZeroRecord;
	}

/* Break the number down into table and entry fields. */

	split_record_number(number, &table_number, &which_entry);

/* Break the table number down into locator and table index fields. */

	split_table_number(table_number, &which_locator, &which_table);

/* Get the locator we need. */

	if ((error = get_locator(which_locator, &locator)) == ERRORCODE_None)
	{
	/* Now that we have the locator, get the table. */

		if ((error = locator->get_table(which_table, &table)) == ERRORCODE_None)
		{
		/* Free the entry in the table. */

			error = table->deallocate_entry(which_entry, type);

		/* And release the table. */

			table->ReadUnlock();

		/* If the entry was locked, the table has an extra modify lock on it. */

			if (record_is_locked)
			{
				table->ModifyUnlock();
			}
		}

	/* And release the locator. */

		locator->ReadUnlock();
	}

	return error;
}

/*
// Mark a table entry as modified.
// It's considered bad form to mark an entry as modified which is not locked
// (because it can cause a whole table to be loaded, the entry read and then
// written as is - a big waste of time).
*/

ERRORCODE TableManager::modified_entry(DB_RECORD_NUMBER number)
{
	TABLE_NUMBER table_number;
	TABLE_INDEX which_table;
	LOCATOR_NUMBER which_locator;
	RECORD_INDEX which_entry;
	ERRORCODE error;
	TableLocatorPtr locator;
	RecordTablePtr table;
	RECORD_FILE_ENTRY far *fentry;
	RECORD_MEMORY_ENTRY far *mentry;

/* See if this is a valid record number. */

	if (number == 0)
	{
		return ERRORCODE_ZeroRecord;
	}

/* Break the number down into table and entry fields. */

	split_record_number(number, &table_number, &which_entry);

/* Break the table number down into locator and table index fields. */

	split_table_number(table_number, &which_locator, &which_table);

/* Get the locator we need. */

	if ((error = get_locator(which_locator, &locator)) == ERRORCODE_None)
	{
	/* Now that we have the locator, get the table. */

		if ((error = locator->get_table(which_table, &table)) == ERRORCODE_None)
		{
		/* Get the entry in this table. */

			if ((error = table->get_entry(which_entry, &fentry, &mentry)) == ERRORCODE_None)
			{
			/* Mark the entry. */

				if (mentry->data != NULL)
				{
					mentry->data->ElementModified();
				}
			}

		/* And release the table. */

			table->ReadUnlock();
		}

	/* And release the locator. */

		locator->ReadUnlock();
	}
	return error;
}

/*
// Lock a table entry.
// Locking an entry locks the table. The table lock count reflects the
// lock count for all the entries locked within it.
*/

ERRORCODE TableManager::lock_entry(DB_RECORD_NUMBER number, RECORD_FILE_ENTRY far * far *fentry, RECORD_MEMORY_ENTRY far * far *mentry, BOOL read_only)
{
	TABLE_NUMBER table_number;
	TABLE_INDEX which_table;
	LOCATOR_NUMBER which_locator;
	RECORD_INDEX which_entry;
	ERRORCODE error;
	TableLocatorPtr locator;
	RecordTablePtr table;

/* See if this is a valid record number. */

	if (number == 0)
	{
		return ERRORCODE_ZeroRecord;
	}

/* Break the number down into table and entry fields. */

	split_record_number(number, &table_number, &which_entry);

/* Break the table number down into locator and table index fields. */

	split_table_number(table_number, &which_locator, &which_table);

/* Get the locator we need. */

	if ((error = get_locator(which_locator, &locator)) == ERRORCODE_None)
	{
	/* Now that we have the locator, get the table. */

		if ((error = locator->get_table(which_table, &table)) == ERRORCODE_None)
		{
		/* Get the entry in this table. */

			if ((error = table->get_entry(which_entry, fentry, mentry)) == ERRORCODE_None)
			{
			/* Lock the table. */

				if (!read_only)
				{
				/* Modify lock the table because the record may change and move. */
					error = table->ModifyLock();
				}

				if (error == ERRORCODE_None)
				{
				/* And lock the entry. */

					(*mentry)->lock_count++;

				/* A special case to handle the initial locking of a new record. */

					if (!read_only && (*fentry)->position == 0)
					{
					/*
					// First lock on a record.
					// Assume that 'fentry->position' will be modified.
					*/
						table->ElementModified();
					}

//					if ((*mentry)->data != NULL)
//					{
//						if ((error = (*mentry)->data->modify_lock()) != ERRORCODE_None)
//						{
//						/* Failed to lock. */
//							table->modify_unlock();
//							(*mentry)->lock_count--;
//						}
//					}

				/* A little debugging test. */

					if ((*mentry)->lock_count > 5)
					{
						od("Excessive locks on record number %ld.\r\n", number);
					}
				}
			}

		/* And release the table. */

			table->ReadUnlock();
		}

	/* And release the locator. */

		locator->ReadUnlock();
	}
	return error;
}

/*
// Unlock a table entry.
*/

ERRORCODE TableManager::unlock_entry(DB_RECORD_NUMBER number, BOOL read_only, BOOL unlock_data)
{
	TABLE_NUMBER table_number;
	TABLE_INDEX which_table;
	LOCATOR_NUMBER which_locator;
	RECORD_INDEX which_entry;
	ERRORCODE error;
	TableLocatorPtr locator;
	RecordTablePtr table;

/* See if this is a valid record number. */

	if (number == 0)
	{
		return ERRORCODE_ZeroRecord;
	}

/* Break the number down into table and entry fields. */

	split_record_number(number, &table_number, &which_entry);

/* Break the table number down into locator and table index fields. */

	split_table_number(table_number, &which_locator, &which_table);

/* Get the locator we need. */

	if ((error = get_locator(which_locator, &locator)) == ERRORCODE_None)
	{
	/* Now that we have the locator, get the table. */

		if ((error = locator->get_table(which_table, &table)) == ERRORCODE_None)
		{
			RECORD_FILE_ENTRY far *fentry;
			RECORD_MEMORY_ENTRY far *mentry;

		/* Get the entry in this table. */

			if ((error = table->get_entry(which_entry, &fentry, &mentry)) == ERRORCODE_None)
			{
				DatabaseRecordPtr record = mentry->data;

			/* And unlock the entry. */

				if (mentry->lock_count == 0)
				{
					error = ERRORCODE_NotLocked;
				/* For debugging purposes. */
					DB_RECORD_TYPE type = (record == NULL) ? -1 : record->type();
					od("Lock underflow on record %ld (type:%d).\r\n", number, type);
				}
				else
				{
					if (unlock_data && record != NULL)
					{
						if (read_only)
						{
							record->ReadUnlock();
						}
						else
						{
							ST_DEV_POSITION old_position = record->Position();

							record->ModifyUnlock();

							if (old_position != record->Position())
							{
								fentry->position = record->Position();
								table->ElementModified();
							}
						}
					/*
					// If the lock count is one, then we are being unlocked
					// for the last time.
					// Signal the record that this is occurring so that it
					// can do any cleanup it wants.
					*/
						if (mentry->lock_count == 1)
						{
							record->FinalReleaseNotify();
						}
					}

					mentry->lock_count--;

				/* Unlock the table. */

					if (!read_only)
					{
					/* We need to modify unlock the table now, too. */
						table->ModifyUnlock();
					}
				}
			}

		/* And release the table. */

			table->ReadUnlock();
		}

	/* And release the locator. */

		locator->ReadUnlock();
	}
	return error;
}

/*
// Force a new device to be used.
// This routine will fail if the storage manager has been initialized.
*/

ERRORCODE TableManager::force_new_device(StorageDevicePtr new_device)
{
	if (initialized)
	{
	/* Sorry. Too late. */
		return ERRORCODE_Busy;
	}

/* Set the new device if not set. */

	if (new_device == NULL)
	{
		new_device = storage_manager->m_pDevice;
	}

/* Get around the 'const' modifier without having to remove it. */

	*(StorageDevicePtr far *)&device = new_device;
	return ERRORCODE_None;
}

/*
// Deallocate all locators and tables.
// This merely frees the *storage* for them. They will automatically be
// allocated storage at write/flush time.
//
// This should NOT be called on a shared database (locking is not performed).
*/

ERRORCODE TableManager::deallocate_table_storage(void)
{
	if (!initialized)
	{
		return ERRORCODE_NotInitialized;
	}

	ERRORCODE error = ERRORCODE_None;
	int i;
	ST_DEV_POSITION far *where;
	TableLocatorPtr locator;

/* Pull in all locators. */

	for (i = 0; ; i++)
	{
		if ((error = get_locator(i, &locator)) != ERRORCODE_None)
		{
			if (error == ERRORCODE_DoesNotExist)
			{
			/* This is OK and in fact is how we stop. */
				error = ERRORCODE_None;
			}
			break;
		}

	/* Deallocate the storage. */

		error = locator->deallocate_table_storage();
		locator->ReadUnlock();

		if (error != ERRORCODE_None)
		{
			break;
		}
	}

/* Now go through each locator and free its storage. */

	for (i = 0, where = &state.first_locator_position, locator = (TableLocatorPtr)locator_list.first_element();;
					where = &locator->next_locator_position, locator = (TableLocatorPtr)locator->next_element(), i++)
	{
		if (locator == NULL)
		{
		/* The locator is not in memory. See if it's in the file. */

			if (*where == POSITION_UNALLOCATED)
			{
			/* Not in the file either. We are done! */
				break;
			}
		}

		if (*where != POSITION_UNALLOCATED)
		{
			locator->Deallocate();
			*where = locator->Position();
		}
	}

	return error;
}
