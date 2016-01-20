/*
// Table manager routines.
//
// This file contains methods for RecordTable and TableManager.
//
// Oct. 15, 1992 - Jay N. : Initial creation
*/

#include "stdafx.h"
#include "ctblman.h"

/************************/
/* CRecordTable methods */
/************************/

CRecordTable::CRecordTable(void)
{
/* Initialize our status. */

	used_record_count = 0;
	locked_record_count = 0;

/* All records are unused. */

	memset(file_entry, 0, sizeof(file_entry));
	memset(memory_entry, 0, sizeof(memory_entry));
}

/*
// Initialize a record table.
*/

void CRecordTable::Init(TABLE_NUMBER id, StorageManagerPtr manager)
{
	StorageElement::Init(ELEMENT_TYPE_TABLE, id, manager);
}

/*
// Free the memory for all records in the table.
// This is used by the destructor and the read method.
*/

void CRecordTable::FreeAllRecords(BOOL force)
{
	RECORD_INDEX i;
	CRECORD_MEMORY_ENTRY far *mentry;
	CRECORD_FILE_ENTRY far *fentry;

/* Run through and delete all records which are in memory. */

	for (i = 0, fentry = file_entry, mentry = memory_entry;
							i < RECORDS_PER_TABLE;
							i++, fentry++, mentry++)
	{
		CStandardDataRecord* pRecord;

		if (fentry->type == DB_NO_TYPE)
		{
		/* Just always NULL it out. */
			mentry->data = NULL;
		}
		else if ((pRecord = mentry->data) != NULL
				&& (force || mentry->lock_count == 0))
		{
			pRecord->CDataRecord::Release();
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
// The destructor for a CRecordTable.
*/

CRecordTable::~CRecordTable()
{
	FreeAllRecords(TRUE);
}

/*
// Allocate a record entry in a table.
// It would be very bad to pass DB_NO_TYPE here, because entries are
// marked as unused by having type DB_NO_TYPE. Therefore, this generates
// the appropriate error.
*/

ERRORCODE CRecordTable::AllocateEntry(RECORD_INDEX far *record_number, DATATYPE type)
{
	ERRORCODE error;
	CRECORD_FILE_ENTRY far *entry;
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

			CRECORD_MEMORY_ENTRY far *mentry = memory_entry+i;

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

ERRORCODE CRecordTable::DeallocateEntry(RECORD_INDEX record_number, DATATYPE type)
{
	ERRORCODE error;
	CRECORD_FILE_ENTRY far &fentry = file_entry[record_number];
	CRECORD_MEMORY_ENTRY far &mentry = memory_entry[record_number];

/* See if this is an allocated entry. */

	if (fentry.type == DB_NO_TYPE)
	{
		return ERRORCODE_NotAllocated;
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
	if (mentry.data != NULL)
	{
		mentry.data->CDataRecord::Release();
		mentry.data = NULL;
	}
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

ERRORCODE CRecordTable::GetEntry(RECORD_INDEX record_number, CRECORD_FILE_ENTRY far * far *fentry, CRECORD_MEMORY_ENTRY far * far *mentry)
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

ERRORCODE CRecordTable::FlushRecord(CRECORD_FILE_ENTRY far *fentry, CRECORD_MEMORY_ENTRY far *mentry, BOOL fFreeAll /*=FALSE*/)
{
	ERRORCODE error = ERRORCODE_None;
	CStandardDataRecord* pRecord;

	if (fentry->type != DB_NO_TYPE
			&& (pRecord = mentry->data) != NULL)
	{
		pRecord->OnFlush();
		if (pRecord->IsElementModified())
		{
			ST_DEV_POSITION old_position = pRecord->Position();

		/* Write the record. */

			if ((error = pRecord->Write()) != ERRORCODE_None)
			{
				return error;
			}

		/* See if the table has been modified by this action. */

			if (old_position != pRecord->Position())
			{
				ElementModified();
			}
		}

		/* See if we want to free this (and if we can). */

		if (fFreeAll)
		{
			if (mentry->lock_count == 0)
			{
//				TRACE1("fFree'ing record %ld\r\n", pRecord->Id());
				pRecord->CDataRecord::Release();
				mentry->data = NULL;
			}
			else
			{
				TRACE2("FLUSH: Unable to fFree record %ld (%d)\r\n", pRecord->Id(), mentry->lock_count);
			}
		}
	}

	return error;
}

/*
// Flush this table to disk.
*/

ERRORCODE CRecordTable::Flush(ST_DEV_POSITION far *where, BOOL fFreeAll /*=FALSE*/)
{
	ERRORCODE error = ERRORCODE_None;

	// See if we need to be allocated.
	if (m_position == POSITION_UNALLOCATED)
	{
		hdr.size = SizeofData(manager->m_pDevice);
		if ((error = Allocate(hdr.type, hdr.id, hdr.size)) != ERRORCODE_None)
		{
		/* Error allocating. */
			return error;
		}
	}

	// Flush all the records.
	CRECORD_FILE_ENTRY far *fentry;
	CRECORD_MEMORY_ENTRY far *mentry;
	RECORD_INDEX i;

/* Run through and flush all records which are in memory. */

	for (i = 0, fentry = file_entry, mentry = memory_entry;
							i < RECORDS_PER_TABLE;
							i++, fentry++, mentry++)
	{
		if ((error = FlushRecord(fentry, mentry, fFreeAll)) != ERRORCODE_None)
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

ERRORCODE CRecordTable::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

//	od("Read record table %d...\r\n", id());

/* Make sure all objects are detached. */

	FreeAllRecords(FALSE);

	if ((error = device->read(file_entry, sizeof(file_entry))) == ERRORCODE_None)
	{
		RECORD_INDEX i;
		CRECORD_FILE_ENTRY far *fentry;

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

ERRORCODE CRecordTable::WriteData(StorageDevicePtr device)
{
/* Write our file entry portion. */
	return device->write(file_entry, sizeof(file_entry));
}

/************************/
/* CTableLocator methods */
/************************/

/*
// The constructor for the table locator.
*/

CTableLocator::CTableLocator()
{
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

void CTableLocator::Init(LOCATOR_NUMBER id, StorageManagerPtr manager)
{
	StorageElement::Init(ELEMENT_TYPE_LOCATOR, id, manager);
}

/*
// Free the memory for all tables in the locator.
// This is used by the destructor and the read method.
*/

void CTableLocator::FreeAllTables(void)
{
	TABLE_INDEX i;
	CTABLE_MEMORY_ENTRY far *mentry;

/* Run through and delete all tables which are in memory. */

	for (i = 0, mentry = memory_entry;
							i < tables_in_use;
							i++, mentry++)
	{
		CRecordTablePtr table;

		if ((table = mentry->table) != NULL)
		{
			delete table;
			mentry->table = NULL;
		}
	}
}

/*
// The destructor for a CTableLocator.
*/

CTableLocator::~CTableLocator()
{
	FreeAllTables();
}

/*
// Get the table index from a table number.
*/

TABLE_INDEX CTableLocator::GetTableIndex(TABLE_NUMBER table_number)
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

ERRORCODE CTableLocator::NewTable(CRecordTablePtr far *table)
{
/* See if we have any more room in this locator. */

	if (tables_in_use == TABLES_PER_LOCATOR)
	{
	/* No more room. */
		return ERRORCODE_Full;
	}

	CTABLE_MEMORY_ENTRY far *mentry = memory_entry+tables_in_use;
	CTABLE_FILE_ENTRY far *fentry = file_entry+tables_in_use;

/* We will use the next slot in the locator. */

	if ((mentry->table = new CRecordTable()) == NULL)
	{
		return ERRORCODE_Memory;
	}
	
	mentry->table->Init(BuildTableNumber(tables_in_use), manager);

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
ERRORCODE CTableLocator::DeleteTable(CRecordTablePtr table)
{
	TABLE_INDEX which_table;

/* Get the table index. */

	if ((which_table = GetTableIndex(table->id())) == -1)
	{
		return ERRORCODE_BadParameter;
	}

/* See if this is the last table in the locator. */

	if (which_table == tables_in_use-1)
	{
	/* Let's free this up. */

		CTABLE_MEMORY_ENTRY far *mentry = memory_entry+which_table;
		CTABLE_FILE_ENTRY far *fentry = file_entry+which_table;

	/* Free memory. */

		if (mentry->table != NULL)
		{
			delete mentry->table;
			mentry->table = NULL;
		}

	/* Free storage. */

		if (fentry->position != POSITION_UNALLOCATED)
		{
			manager->FreeStorage(fentry->position);
			fentry->position = POSITION_UNALLOCATED;
		}

	/* One table in use. */

		tables_in_use--;

	/* We're dirty. */

		ElementModified();
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

ERRORCODE CTableLocator::FreeTable(CRecordTablePtr table)
{
	TABLE_INDEX which_table;
	CTABLE_MEMORY_ENTRY far *mentry;
	CTABLE_FILE_ENTRY far *fentry;
	ERRORCODE error;

/* Get the table index. */

	if ((which_table = GetTableIndex(table->Id())) == -1)
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

ERRORCODE CTableLocator::GetTable(TABLE_INDEX which_table, CRecordTablePtr far *table_p)
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
	CTABLE_MEMORY_ENTRY far &mentry = memory_entry[which_table];
	CTABLE_FILE_ENTRY far &fentry = file_entry[which_table];
	CRecordTablePtr table;

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

		if ((table = new CRecordTable()) == NULL)
		{
			return ERRORCODE_Memory;
		}
		
		table->Init(BuildTableNumber(which_table), manager);

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

ERRORCODE CTableLocator::Flush(ST_DEV_POSITION far *where, BOOL fFreeAll /*=FALSE*/)
{
	ERRORCODE error = ERRORCODE_None;

	// See if we need to be allocated.
	if (m_position == POSITION_UNALLOCATED)
	{
		hdr.size = SizeofData(manager->m_pDevice);
		if ((error = Allocate(hdr.type, hdr.id, hdr.size)) != ERRORCODE_None)
		{
		/* Error allocating. */
			return error;
		}
	}

	CTABLE_FILE_ENTRY far *fentry;
	CTABLE_MEMORY_ENTRY far *mentry;
	TABLE_INDEX i;

/* Run through and flush all tables which are in memory. */

	for (i = 0, fentry = file_entry, mentry = memory_entry;
							i < TABLES_PER_LOCATOR;
							i++, fentry++, mentry++)
	{
		CRecordTablePtr table;

		if ((table = mentry->table) != NULL)
		{
			ST_DEV_POSITION old_position = table->Position();

		/* Flush the table. */

			if ((error = table->Flush(&fentry->position, fFreeAll)) != ERRORCODE_None)
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
// DeallocateTableStorage()
//
// Run through and deallocate the storage for all tables in this locator.
// The storage will automatically be reallocated at write/flush time.
//
// Do NOT call this on a shared database (proper locking is not performed).
*/

ERRORCODE CTableLocator::DeallocateTableStorage(void)
{
	CTABLE_FILE_ENTRY far *fentry;
	CTABLE_MEMORY_ENTRY far *mentry;
	TABLE_INDEX i;
	ERRORCODE error = ERRORCODE_None;

/* Run through and flush all tables which are in memory. */

	for (i = 0, fentry = file_entry, mentry = memory_entry;
							i < TABLES_PER_LOCATOR;
							i++, fentry++, mentry++)
	{
		CRecordTablePtr table;

		if ((error = GetTable(i, &table)) != ERRORCODE_None)
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

ERRORCODE CTableLocator::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

/* Make sure all tables are freed before reading new. */

	FreeAllTables();

/* Read the next locator position. */

	if ((error = device->read(&next_locator_position, sizeof(next_locator_position))) == ERRORCODE_None)
	{
	/* Read the table file entries. */

		if ((error = device->read(file_entry, sizeof(file_entry))) == ERRORCODE_None)
		{
			CTABLE_FILE_ENTRY far *fentry;

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

ERRORCODE CTableLocator::WriteData(StorageDevicePtr device)
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
/* CTableManager methods */
/************************/

/*
// The destructor for a table manager.
*/

CTableManager::~CTableManager()
{
}

/*
// Initialize the table manager.
// This is called in cases where a state is not available on the storage
// device (like in the case of a new file).
*/

ERRORCODE CTableManager::Initialize(ST_DEV_POSITION state_offset)
{
/* Make sure the locator list is empty. */

	m_LocatorList.empty();

/* Set the first locator position to unknown. */

	m_State.first_locator_position = POSITION_UNALLOCATED;

	m_StatePosition = state_offset;

	if (m_pStorageManager->IsSharing())
	{
	/* We need to init the semaphore. */

		m_SemPosition = state_offset + m_pDevice->size_record(sizeof(m_State));
		m_Sem.Init(m_pDevice, m_SemPosition);

	/* We need to write the initial state which will be shared. */

		ERRORCODE error;

		if ((error = FlushState()) != ERRORCODE_None)
		{
			return error;
		}
	}

	m_fInitialized = TRUE;

	return ERRORCODE_None;
}

/*
// Initialize the table manager.
// This is called in cases where a state is not available on the storage
// device (like in the case of a new file).
*/

ERRORCODE CTableManager::Reinitialize(void)
{
	ERRORCODE error;

/* Step 1: Lock the state. */

	if ((error = ModifyLockState()) == ERRORCODE_None)
	{
	/* Step 2: Reinitialize the storage manager. */
		if ((error = m_pStorageManager->Reinitialize()) == ERRORCODE_None)
		{
		/* Force the locator list to reinitialize. */
			m_LocatorList.empty();

			m_State.first_locator_position = POSITION_UNALLOCATED;
		}
		ModifyUnlockState();
	}
	return error;
}

/*
// Refresh the state.
*/

ERRORCODE CTableManager::RefreshState(void)
{
	ERRORCODE error;
	SHORT size = m_pDevice->size_record(sizeof(m_State));
	BOOL fIsSharing = m_pStorageManager->IsSharing();

/* Lock the area if we're sharing. */

	if (fIsSharing)
	{
		if ((error = m_pDevice->lock(m_StatePosition, size, FALSE)) != ERRORCODE_None)
		{
			return error;
		}
	}

/* Make sure the locator list is empty. */

	ST_DEV_POSITION old_first_locator = m_State.first_locator_position;

/* Do the read. */

	if ((error = m_pDevice->seek(m_StatePosition, ST_DEV_SEEK_SET)) == ERRORCODE_None)
	{
		if ((error = m_pDevice->read_record(&m_State, sizeof(m_State))) == ERRORCODE_None)
		{
			if (m_State.first_locator_position != old_first_locator)
			{
			/* Something drastic has happened. Throw everything away. */
				m_LocatorList.empty();
			}
		}
	}

/* Unlock the area if we're sharing. */

	if (fIsSharing)
	{
		m_pDevice->lock(m_StatePosition, size, TRUE);
	}

	return error;
}

/*
// Flush the state.
*/

ERRORCODE CTableManager::FlushState(void)
{
	ERRORCODE error;
	BOOL fIsSharing = m_pStorageManager->IsSharing();

	SHORT size = m_pDevice->size_record(sizeof(m_State));

/* Lock the area if we're sharing. */

	if (fIsSharing)
	{
		if ((error = m_pDevice->lock(m_StatePosition, size, FALSE)) != ERRORCODE_None)
		{
			return error;
		}
	}

/* Do the write. */

	if ((error = m_pDevice->seek(m_StatePosition, ST_DEV_SEEK_SET)) == ERRORCODE_None)
	{
		error = m_pDevice->write_record(&m_State, sizeof(m_State));
	}

/* Unlock the area if we're sharing. */

	if (fIsSharing)
	{
		m_pDevice->lock(m_StatePosition, size, TRUE);
	}

	return error;
}

/*
// Read the table manager state from the current position in the
// storage device.
*/

ERRORCODE CTableManager::ReadState(BOOL validate, BOOL reset_semaphores)
{
	ERRORCODE error;

/* Remember the state position. */

	m_pDevice->tell(&m_StatePosition);

/* Read our state. */

	if ((error = RefreshState()) == ERRORCODE_None)
	{
		if (m_pStorageManager->IsSharing())
		{
			m_pDevice->tell(&m_SemPosition);

			if (validate && reset_semaphores)
			{
			/* Reset any read locks on the semaphore. */
				m_Sem.Init(m_pDevice, m_SemPosition);
			}

			m_pDevice->seek(m_SemPosition + m_Sem.Size(), ST_DEV_SEEK_SET);
		}

		m_fInitialized = TRUE;
	}
	return error;
}

/*
// Write the table manager state to the current position in the
// storage device. The table manager must be initialized.
*/

ERRORCODE CTableManager::WriteState()
{
/* Make sure we're initialized. */

	if (!m_fInitialized)
	{
		return ERRORCODE_NotInitialized;
	}

/* Write our state. */

	ERRORCODE error;

	if ((error = FlushState()) == ERRORCODE_None)
	{
		if (m_pStorageManager->IsSharing())
		{
		/* Skip over the semaphore. */
			error = m_pDevice->seek(m_Sem.Size(), ST_DEV_SEEK_CUR);
		}
	}
	return error;
}

ST_DEV_IO_SIZE CTableManager::SizeState(VOID)
{
	ST_DEV_IO_SIZE size = m_pDevice->size_record(sizeof(m_State));

	if (m_pStorageManager->IsSharing())
	{
		size += m_Sem.Size();
	}
	return size;
}

/*
// Read lock the state.
*/

ERRORCODE CTableManager::ReadLockState(void)
{
	ERRORCODE error = ERRORCODE_None;

	if (m_pStorageManager->IsSharing())
	{
	/* Read lock the semaphore. */

		if ((error = m_Sem.ReadLock(m_pDevice, m_SemPosition)) != ERRORCODE_None)
		{
			return error;
		}

	/* If the state changed, refresh it now. */

		if (m_Sem.Changed())
		{
			if ((error = RefreshState()) != ERRORCODE_None)
			{
				m_Sem.ReadUnlock(m_pDevice, m_SemPosition);
			}
		}
	}
	return error;
}

/*
// Read unlock the state.
*/

ERRORCODE CTableManager::ReadUnlockState(void)
{
	if (m_pStorageManager->IsSharing())
	{
		return m_Sem.ReadUnlock(m_pDevice, m_SemPosition);
	}
	return ERRORCODE_None;
}

/*
// Mofidy lock the state.
*/

ERRORCODE CTableManager::ModifyLockState(void)
{
	ERRORCODE error = ERRORCODE_None;

	if (m_pStorageManager->IsSharing())
	{
	/* Modify lock the semaphore. */

		if ((error = m_Sem.ModifyLock(m_pDevice, m_SemPosition)) != ERRORCODE_None)
		{
			return error;
		}

	/* If the state changed, refresh it now. */

		if (m_Sem.Changed())
		{
			if ((error = RefreshState()) != ERRORCODE_None)
			{
				m_Sem.ModifyUnlock(m_pDevice, m_SemPosition, FALSE);
			}
		}
	}
	return error;
}

/*
// Modify unlock the state.
*/

ERRORCODE CTableManager::ModifyUnlockState(void)
{
	if (m_pStorageManager->IsSharing())
	{
		if (m_Sem.GetModifyLocks() == 1)
		{
			FlushState();
		}
		m_Sem.ModifyUnlock(m_pDevice, m_SemPosition, TRUE);
	}
	return ERRORCODE_None;
}

/*
// Flush all table manager data to the disk.
// In this implementation, this involves writing all tables to the storage
// device (allocating them as necessary).
*/

ERRORCODE CTableManager::Flush(BOOL fFreeAll /*=FALSE*/)
{
	CTableLocatorPtr locator;
	CTableLocatorPtr last_locator;
	ST_DEV_POSITION far *where;
	ERRORCODE error = ERRORCODE_None;

	if (!m_fInitialized)
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

	for (where = &m_State.first_locator_position, locator = (CTableLocatorPtr)m_LocatorList.first_element();
					locator != NULL;
					where = &locator->next_locator_position,
						last_locator = locator,
						locator = (CTableLocatorPtr)locator->next_element())
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
			/* Make sure it writes out. */
				last_locator->ElementModified();
			}
		}
	}

/*
// Flush all the table locators.
// This should cause the locators to be written if modified.
// No position changes should occur because locators are always the same size.
*/

	for (where = &m_State.first_locator_position, locator = (CTableLocatorPtr)m_LocatorList.first_element();
					locator != NULL;
					where = &locator->next_locator_position, locator = (CTableLocatorPtr)locator->next_element())
	{
		if ((error = locator->Flush(where, fFreeAll)) != ERRORCODE_None)
		{
			return error;
		}
	}

	return error;
}

/*
// Split a record number.
*/

VOID CTableManager::SplitRecordNumber(DATAID number, TABLE_NUMBER *table, RECORD_INDEX *entry)
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

DATAID CTableManager::BuildRecordNumber(TABLE_NUMBER table, RECORD_INDEX entry)
{
	return (table * RECORDS_PER_TABLE) + entry + 1;
}

/*
// Split a table number.
*/

VOID CTableManager::SplitTableNumber(TABLE_NUMBER table, LOCATOR_NUMBER *locator, TABLE_INDEX *entry)
{
/* Break it down. */

	*locator = (LOCATOR_NUMBER)(table / TABLES_PER_LOCATOR);
	*entry = (TABLE_INDEX)(table % TABLES_PER_LOCATOR);
}

/*
// Build a table number.
*/

TABLE_NUMBER CTableManager::BuildTableNumber(LOCATOR_NUMBER locator, TABLE_INDEX entry)
{
	return (locator * TABLES_PER_LOCATOR) + entry;
}

/*
// Attach a locator to the table manager.
// Locators are always added at the end of the chain.
*/

VOID CTableManager::AttachLocator(CTableLocatorPtr locator)
{
	m_LocatorList.append(locator);
}

/*
// Allocate a new locator.
// The locator is hooked into the table manager, and it's left read-locked.
*/

ERRORCODE CTableManager::NewLocator(LOCATOR_NUMBER which, CTableLocatorPtr far *locator_p)
{
	CTableLocatorPtr locator;
	BOOL fIsSharing = m_pStorageManager->IsSharing();
	CTableLocatorPtr previous_locator;
	ERRORCODE error;

/*
// If this is a shared file, we need to lock the locator which will be
// getting a pointer to this locator.
*/

	if (fIsSharing)
	{
	/* Do the lock. */

		SHORT count = m_LocatorList.count();

		if (count == 0)
		{
		/*
		// There is no last locator.
		// We need to lock the front of the locator list.
		*/
			previous_locator = NULL;
			if ((error = ModifyLockState()) != ERRORCODE_None)
			{
				return error;
			}
		}
		else
		{
			if ((error = GetLocator(count-1, &previous_locator)) != ERRORCODE_None)
			{
			/* Try to modify lock the locator. */

				error = previous_locator->ModifyLock();

			/* Always read unlock the locator. */

				previous_locator->ReadUnlock();
			}

			if (error != NULL)
			{
				return error;
			}
		}
	}

/* Allocate the locator. */

	if ((locator = new CTableLocator()) == NULL)
	{
		error = ERRORCODE_Memory;
	}
	else
	{
		locator->Init(which, m_pStorageManager);

		error = ERRORCODE_None;

		if (fIsSharing)
		{
		/* We need to allocate and write the locator NOW! */

			if ((error = locator->Write()) == ERRORCODE_None)
			{
			/* Success. Hook it in. */
				if (previous_locator == NULL)
				{
				/* Update the header area. */
					m_State.first_locator_position = locator->Position();
				}
				else
				{
				/* Update the previouos locator. */
					previous_locator->SetNextLocator(locator->Position());
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

	if (fIsSharing)
	{
		if (previous_locator == NULL)
		{
		/* Update the header area. */
			ModifyUnlockState();
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

	AttachLocator(locator);

/* Pass back the pointer we found. */

	*locator_p = locator;

	return ERRORCODE_None;
}

/*
// Get a locator, reading it into memory if necessary.
// The locator is left read-locked.
*/

ERRORCODE CTableManager::GetLocator(LOCATOR_NUMBER which, CTableLocatorPtr far *locator_p)
{
	CTableLocatorPtr locator;
	LOCATOR_NUMBER i;
	ST_DEV_POSITION far *where;
	ERRORCODE error = ERRORCODE_None;

/* Lock the locator chain so we can move through it. */

	if ((error = ReadLockState()) != ERRORCODE_None)
	{
		return error;
	}

/* Find the locator. Read it from the file if necessary. */

	for (i = 0, where = &m_State.first_locator_position, locator = (CTableLocatorPtr)m_LocatorList.first_element();;
					where = &locator->next_locator_position, locator = (CTableLocatorPtr)locator->next_element(), i++)
	{
		if (locator == NULL)
		{
		/* The locator is not in memory. See if it's in the file. */

			if (*where == POSITION_UNALLOCATED)
			{
			/* Not in the file either. Bad record number. */
				ReadUnlockState();
				return ERRORCODE_DoesNotExist;
			}

		/* We need to create a new locator. */

			if ((locator = new CTableLocator()) == NULL)
			{
				ReadUnlockState();
				return ERRORCODE_Memory;
			}

			locator->Init(i, m_pStorageManager);

		/* Read the locator. */

			if ((error = locator->Read(*where)) != ERRORCODE_None)
			{
				delete locator;
				ReadUnlockState();
				return error;
			}

		/* Attach the locator to the table manager. */

			AttachLocator(locator);
		}

	/* Try to read lock the header as necessary. */

		if ((error = locator->ReadLock()) != ERRORCODE_None)
		{
			ReadUnlockState();
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

	ReadUnlockState();
	return error;
}

/*
// Allocate an entry in the CTableManager's record table.
*/

ERRORCODE CTableManager::AllocateEntry(DATAID far *number, DATATYPE type)
{
	LOCATOR_NUMBER locator_index;
	ERRORCODE error;

	for (locator_index = 0; locator_index < LOCATORS_PER_DATABASE; locator_index++)
	{
		CTableLocatorPtr locator;

	/* Get this locator. */

		if ((error = GetLocator(locator_index, &locator)) != ERRORCODE_None)
		{
		/* See if this is an bad error. */
			if (error != ERRORCODE_DoesNotExist)
			{
			/* Bad error. Go home. */
				return error;
			}

		/* We need to add a locator. */

			if ((error = NewLocator(locator_index, &locator)) != ERRORCODE_None)
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
			CRecordTablePtr table;

			if ((error = locator->GetTable(table_index, &table)) != ERRORCODE_None)
			{
			/* See if this is a bad error. */

				if (error != ERRORCODE_DoesNotExist)
				{
				/* Bad error. Go home. */
					locator->ModifyUnlock();
					return error;
				}

			/* We need to add a table. */

				if ((error = locator->NewTable(&table)) != ERRORCODE_None)
				{
					locator->ModifyUnlock();
					return error;
				}
			}

		/* We have the table. Allocate an entry. */

			RECORD_INDEX record_index;

			error = table->AllocateEntry(&record_index, type);

		/* We're done reading. Unlock the table. */

			table->ReadUnlock();

			if (error == ERRORCODE_None)
			{
				*number = BuildRecordNumber(table->Id(), record_index);
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
// Deallocate an entry in the CTableManager's record table.
*/

ERRORCODE CTableManager::DeallocateEntry(DATAID number, DATATYPE type, BOOL record_is_locked)
{
	TABLE_NUMBER table_number;
	TABLE_INDEX which_table;
	LOCATOR_NUMBER which_locator;
	RECORD_INDEX which_entry;
	ERRORCODE error;
	CTableLocatorPtr locator;
	CRecordTablePtr table;

/* See if this is a valid record number. */

	if (number == 0)
	{
		return ERRORCODE_ZeroRecord;
	}

/* Break the number down into table and entry fields. */

	SplitRecordNumber(number, &table_number, &which_entry);

/* Break the table number down into locator and table index fields. */

	SplitTableNumber(table_number, &which_locator, &which_table);

/* Get the locator we need. */

	if ((error = GetLocator(which_locator, &locator)) == ERRORCODE_None)
	{
	/* Now that we have the locator, get the table. */

		if ((error = locator->GetTable(which_table, &table)) == ERRORCODE_None)
		{
		/* Free the entry in the table. */

			error = table->DeallocateEntry(which_entry, type);

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

ERRORCODE CTableManager::ModifiedEntry(DATAID number)
{
	TABLE_NUMBER table_number;
	TABLE_INDEX which_table;
	LOCATOR_NUMBER which_locator;
	RECORD_INDEX which_entry;
	ERRORCODE error;
	CTableLocatorPtr locator;
	CRecordTablePtr table;
	CRECORD_FILE_ENTRY far *fentry;
	CRECORD_MEMORY_ENTRY far *mentry;

/* See if this is a valid record number. */

	if (number == 0)
	{
		return ERRORCODE_ZeroRecord;
	}

/* Break the number down into table and entry fields. */

	SplitRecordNumber(number, &table_number, &which_entry);

/* Break the table number down into locator and table index fields. */

	SplitTableNumber(table_number, &which_locator, &which_table);

/* Get the locator we need. */

	if ((error = GetLocator(which_locator, &locator)) == ERRORCODE_None)
	{
	/* Now that we have the locator, get the table. */

		if ((error = locator->GetTable(which_table, &table)) == ERRORCODE_None)
		{
		/* Get the entry in this table. */

			if ((error = table->GetEntry(which_entry, &fentry, &mentry)) == ERRORCODE_None)
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
// Free a table entry's record if possible.
*/

ERRORCODE CTableManager::FlushRecord(DATAID number)
{
	TABLE_NUMBER table_number;
	TABLE_INDEX which_table;
	LOCATOR_NUMBER which_locator;
	RECORD_INDEX which_entry;
	ERRORCODE error;
	CTableLocatorPtr locator;
	CRecordTablePtr table;
	CRECORD_FILE_ENTRY far *fentry;
	CRECORD_MEMORY_ENTRY far *mentry;

/* See if this is a valid record number. */

	if (number == 0)
	{
		return ERRORCODE_ZeroRecord;
	}

/* Break the number down into table and entry fields. */

	SplitRecordNumber(number, &table_number, &which_entry);

/* Break the table number down into locator and table index fields. */

	SplitTableNumber(table_number, &which_locator, &which_table);

/* Get the locator we need. */

	if ((error = GetLocator(which_locator, &locator)) == ERRORCODE_None)
	{
	/* Now that we have the locator, get the table. */

		if ((error = locator->GetTable(which_table, &table)) == ERRORCODE_None)
		{
		/* Get the entry in this table. */

			if ((error = table->GetEntry(which_entry, &fentry, &mentry)) == ERRORCODE_None)
			{
			/* Flush the record. */

				table->FlushRecord(fentry, mentry, TRUE);
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

ERRORCODE CTableManager::LockEntry(DATAID number, CRECORD_FILE_ENTRY far * far *fentry, CRECORD_MEMORY_ENTRY far * far *mentry, BOOL read_only)
{
	TABLE_NUMBER table_number;
	TABLE_INDEX which_table;
	LOCATOR_NUMBER which_locator;
	RECORD_INDEX which_entry;
	ERRORCODE error;
	CTableLocatorPtr locator;
	CRecordTablePtr table;

/* See if this is a valid record number. */

	if (number == 0)
	{
		return ERRORCODE_ZeroRecord;
	}

/* Break the number down into table and entry fields. */

	SplitRecordNumber(number, &table_number, &which_entry);

/* Break the table number down into locator and table index fields. */

	SplitTableNumber(table_number, &which_locator, &which_table);

/* Get the locator we need. */

	if ((error = GetLocator(which_locator, &locator)) == ERRORCODE_None)
	{
	/* Now that we have the locator, get the table. */

		if ((error = locator->GetTable(which_table, &table)) == ERRORCODE_None)
		{
		/* Get the entry in this table. */

			if ((error = table->GetEntry(which_entry, fentry, mentry)) == ERRORCODE_None)
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

//					TRACE3("Lock count to %d for %ld (%d)\r\n",
//							(*mentry)->lock_count,
//							number, (*fentry)->type);

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
//						if ((error = (*mentry)->data->ModifyLock()) != ERRORCODE_None)
//						{
//						/* Failed to lock. */
//							table->ModifyUnlock();
//							(*mentry)->lock_count--;
//						}
//					}

				/* A little debugging test. */

					if ((*mentry)->lock_count > 15)
					{
						TRACE1("Excessive locks on record number %ld.\n", number);
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

ERRORCODE CTableManager::UnlockEntry(DATAID number, BOOL read_only, BOOL unlock_data)
{
	TABLE_NUMBER table_number;
	TABLE_INDEX which_table;
	LOCATOR_NUMBER which_locator;
	RECORD_INDEX which_entry;
	ERRORCODE error;
	CTableLocatorPtr locator;
	CRecordTablePtr table;

/* See if this is a valid record number. */

	if (number == 0)
	{
		return ERRORCODE_ZeroRecord;
	}

/* Break the number down into table and entry fields. */

	SplitRecordNumber(number, &table_number, &which_entry);

/* Break the table number down into locator and table index fields. */

	SplitTableNumber(table_number, &which_locator, &which_table);

/* Get the locator we need. */

	if ((error = GetLocator(which_locator, &locator)) == ERRORCODE_None)
	{
	/* Now that we have the locator, get the table. */

		if ((error = locator->GetTable(which_table, &table)) == ERRORCODE_None)
		{
			CRECORD_FILE_ENTRY far *fentry;
			CRECORD_MEMORY_ENTRY far *mentry;

		/* Get the entry in this table. */

			if ((error = table->GetEntry(which_entry, &fentry, &mentry)) == ERRORCODE_None)
			{
				CStandardDataRecord* pRecord = mentry->data;

			/* And unlock the entry. */

				if (mentry->lock_count == 0)
				{
					error = ERRORCODE_NotLocked;
				/* For debugging purposes. */
					DATATYPE type = (pRecord == NULL) ? -1 : pRecord->Type();
					TRACE2("Lock underflow on record %ld (type:%d).\n", number, type);
				}
				else
				{
					if (unlock_data && pRecord != NULL)
					{
						if (read_only)
						{
							pRecord->ReadUnlock();
						}
						else
						{
							ST_DEV_POSITION old_position = pRecord->Position();

							pRecord->ModifyUnlock();

							if (old_position != pRecord->Position())
							{
								fentry->position = pRecord->Position();
								table->ElementModified();
							}
						}
					}

					mentry->lock_count--;

//					TRACE3("Unlock count to %d for %ld (%d)\r\n",
//							mentry->lock_count,
//							number, fentry->type);

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

ERRORCODE CTableManager::ForceNewDevice(StorageDevicePtr pNewDevice)
{
	if (m_fInitialized)
	{
	/* Sorry. Too late. */
		return ERRORCODE_Busy;
	}

/* Set the new device if not set. */

	if (pNewDevice == NULL)
	{
		pNewDevice = m_pStorageManager->m_pDevice;
	}

/* Get around the 'const' modifier without having to remove it. */

	*(StorageDevicePtr far *)&m_pDevice = pNewDevice;
	return ERRORCODE_None;
}

/*
// Deallocate all locators and tables.
// This merely frees the *storage* for them. They will automatically be
// allocated storage at write/flush time.
//
// This should NOT be called on a shared database (locking is not performed).
*/

ERRORCODE CTableManager::DeallocateTableStorage(void)
{
	if (!m_fInitialized)
	{
		return ERRORCODE_NotInitialized;
	}

	ERRORCODE error = ERRORCODE_None;
	int i;
	ST_DEV_POSITION far *where;
	CTableLocatorPtr locator;

/* Pull in all locators. */

	for (i = 0; ; i++)
	{
		if ((error = GetLocator(i, &locator)) != ERRORCODE_None)
		{
			if (error == ERRORCODE_DoesNotExist)
			{
			/* This is OK and in fact is how we stop. */
				error = ERRORCODE_None;
			}
			break;
		}

	/* Deallocate the storage. */

		error = locator->DeallocateTableStorage();
		locator->ReadUnlock();

		if (error != ERRORCODE_None)
		{
			break;
		}
	}

/* Now go through each locator and free its storage. */

	for (i = 0, where = &m_State.first_locator_position, locator = (CTableLocatorPtr)m_LocatorList.first_element();;
					where = &locator->next_locator_position, locator = (CTableLocatorPtr)locator->next_element(), i++)
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
