/*
// Storage manager routines.
// This file includes the methods for the StorageManager class.
//
// Oct. 14, 1992 - Jay N. : Initial creation
*/

#include "stdafx.h"

#include "storeman.h"

#define DUMP_LISTx

/***************************/
/* Storage Manager methods */
/***************************/

/*
// The constructor for the storage manager class.
*/

StorageManager::StorageManager(StorageDevicePtr pDevice, BOOL is_sharing)
		: m_pDevice(pDevice),
			initialized(FALSE),
			m_is_sharing(is_sharing)
{
	m_size_limit = POSITION_MAX;
	m_nHdrBlockSize = ST_MAN_HDR_SIZE;
	m_fExclusive = FALSE;
	m_fAttached = FALSE;

	if (is_sharing)
	{
		m_nHdrBlockSize += RW_Semaphore::Size();
	}

	m_lTotalLocks = 0;

#ifdef STOREMAN_STATISTICS
	m_lAllocate = 0;
	m_lFree = 0;
	m_lResize = 0;
	m_lElementRead = 0;
	m_lElementWrite = 0;
	memset(m_lElementReadLock, 0, sizeof(m_lElementReadLock));
	memset(m_lElementReadUnlock, 0, sizeof(m_lElementReadUnlock));
	memset(m_lElementModifyLock, 0, sizeof(m_lElementModifyLock));
	memset(m_lElementModifyUnlock, 0, sizeof(m_lElementModifyUnlock));
#ifdef RWSEM_STATISTICS
	RW_Semaphore::m_lReadLock = 0;
	RW_Semaphore::m_lReadLockSkip = 0;
	RW_Semaphore::m_lReadUnLock = 0;
	RW_Semaphore::m_lReadUnLockSkip = 0;
#endif
#endif
}

StorageManager::~StorageManager()
{
	ASSERT(m_lTotalLocks == 0);

#ifdef STOREMAN_STATISTICS
#if 0
	TRACE("Allocate:%ld, Free:%ld, Resize:%ld, Read:%ld, Write:%ld\n",
			m_lAllocate,
			m_lFree,
			m_lResize,
			m_lElementRead,
			m_lElementWrite);
	TRACE("ReadLock:    %ld, %ld, %ld, %ld\n"
			"ReadUnlock:  %ld, %ld, %ld, %ld\n"
			"ModifyLock:  %ld, %ld, %ld, %ld\n"
			"ModifyUnlock:%ld, %ld, %ld, %ld\n",
			m_lElementReadLock[1], m_lElementReadLock[2], m_lElementReadLock[3], m_lElementReadLock[4],
			m_lElementReadUnlock[1],m_lElementReadUnlock[2],m_lElementReadUnlock[3],m_lElementReadUnlock[4],
			m_lElementModifyLock[1],m_lElementModifyLock[2],m_lElementModifyLock[3],m_lElementModifyLock[4],
			m_lElementModifyUnlock[1],m_lElementModifyUnlock[2],m_lElementModifyUnlock[3],m_lElementModifyUnlock[4]);
#ifdef RWSEM_STATISTICS
	TRACE("Sem ReadLock:%ld, ReadLockSkip:%ld, ReadUnlock:%ld, ReadUnlockSkip:%ld\n",
			RW_Semaphore::m_lReadLock,
			RW_Semaphore::m_lReadLockSkip,
			RW_Semaphore::m_lReadUnLock,
			RW_Semaphore::m_lReadUnLockSkip);
#endif
#endif	// #if 0
#endif
}

/*
// Set the sharing flag.
// This should only be done before the state has been read (i.e. before
// this guy gets initialized).
*/

ERRORCODE StorageManager::SetSharing(BOOL is_sharing)
{
	if (initialized)
	{
	/* Sorry. Too late. */
		return ERRORCODE_Busy;
	}

	m_is_sharing = is_sharing;

	m_nHdrBlockSize = ST_MAN_HDR_SIZE;

	if (is_sharing)
	{
		m_nHdrBlockSize += RW_Semaphore::Size();
	}
	return ERRORCODE_None;
}

ERRORCODE StorageManager::SetExclusive(BOOL fExclusive)
{
	ERRORCODE error = ERRORCODE_None;

	// We can only change exclusive state when our locks are 0.
	if (m_fExclusive != fExclusive)
	{
		if (!fExclusive)
		{
			// We want to stop being exclusive.
			if (m_lTotalLocks == 1)
			{
				// We are currently in exclusive mode and we are down to
				// one lock (which is the state lock).
				// We will stop being exclusive.
				// Set m_fExclusive to FALSE first. ModifyLockState() was called
				// before with fExclusive set to FALSE. We need to unlock the
				// state the same way.

				m_fExclusive = FALSE;		// Do this first!
				ModifyUnlockState();
				ASSERT(m_lTotalLocks == 0);
			}
		}
		else
		{
			// We want to be exclusive, but we are not.
			// We can only be exclusive if we do not have any locks going.
			if (m_lTotalLocks == 0)
			{
				// We can only be exclusive if we are the only user.
				if ((error = ModifyLockState()) == ERRORCODE_None)
				{
//					ASSERT(state.m_lUsers > 0);
					if (state.m_lUsers == 1)
					{
						// We can be exclusive!
						m_fExclusive = TRUE;
						// Leave the state locked for the duration.
						ASSERT(m_lTotalLocks == 1);
					}
					else
					{
						ModifyUnlockState();
						// Too many users!
						error = ERRORCODE_Busy;
					}
				}
			}
		}
	}
	return error;
}

void StorageManager::AddedLock(void)
{
	m_lTotalLocks++;
}

void StorageManager::RemovedLock(int nCount /*=1*/)
{
	m_lTotalLocks -= nCount;
	if (m_lTotalLocks < 0)
	{
		ASSERT(FALSE);
		m_lTotalLocks = 0;
	}
}

ERRORCODE StorageManager::Attach(void)
{
	ERRORCODE error = ERRORCODE_None;
	if (!m_fAttached)
	{
		// Increment the user count.
		if ((error = ModifyLockState()) == ERRORCODE_None)
		{
			state.m_lUsers++;
			ModifyUnlockState();

			m_fAttached = TRUE;
		}
	}
	return error;
}

ERRORCODE StorageManager::Detach(void)
{
	ERRORCODE error = ERRORCODE_None;
	if (m_fAttached)
	{
		if ((error = ModifyLockState()) == ERRORCODE_None)
		{
			// Decrement the user count.
			if (--state.m_lUsers < 0)
			{
				ASSERT(FALSE);
				state.m_lUsers = 0;
			}
			ModifyUnlockState();

			m_fAttached = FALSE;
		}
	}
	return error;
}

/*
// Initialize a Storage Manager object.
// The beginning and end of storage are set to the passed start.
*/

ERRORCODE StorageManager::Initialize(ST_DEV_POSITION state_offset, ST_DEV_POSITION storage_base)
{
	state.storage_start = state.storage_end = storage_base;
	state.free_start = POSITION_UNALLOCATED;
 	state.free_biggest = ST_MAN_MAX_SIZE;
	state.m_lUsers = 0;

	m_state_position = state_offset;

	if (IsSharing())
	{
	/* We need to init the semaphore. */

		m_sem_position = state_offset + m_pDevice->size_record(SizeStateRecord());
		m_sem.Init(m_pDevice, m_sem_position);

		state.m_lUsers = 0;

	/* We need to write the initial state which will be shared. */

		ERRORCODE error;

		if ((error = FlushState()) != ERRORCODE_None)
		{
			return error;
		}
	}

	initialized = TRUE;
	return ERRORCODE_None;
}

/*
// Reinitialize()
//
// Reinitialize the storage manager.
// This will free all storage in one fell swoop.
*/

ERRORCODE StorageManager::Reinitialize(long lLocks /*=0*/)
{
	ERRORCODE error;

/* Step 1: Lock the state. */

	if (m_lTotalLocks != lLocks)
	{
		// We, at least, have a lock.
		error = ERRORCODE_Busy;
	}
	else if ((error = ModifyLockState()) == ERRORCODE_None)
	{
		BOOL locked_all = FALSE;
		ST_DEV_POSITION lock_end = state.storage_end;

		if (IsSharing())
		{
		/* Step 2: Try to lock all of storage. This ensures no MODIFY locks. */

			if ((error = m_pDevice->lock(state.storage_start, lock_end, FALSE)) == ERRORCODE_None)
			{
				locked_all = TRUE;

			/* Step 3: Make sure there are no READ locks in effect. */

				ST_DEV_POSITION pos = state.storage_start;
				ST_MAN_HDR_BLOCK hdr;
				RW_Semaphore sem;

			/* Loop through all blocks and see if anyone has a read lock. */

				while (pos < state.storage_end)
				{
				/* Read this header so we can know where the next is. */

					if ((error = ReadHeaderBlock(&hdr, pos, FALSE)) != ERRORCODE_None)
					{
						break;
					}

				/* Read the semaphore state to see if anyone has a lock. */

					if (hdr.type != ELEMENT_TYPE_FREE)
					{
						if ((error = sem.Read(m_pDevice, pos + ST_MAN_HDR_SIZE)) != ERRORCODE_None)
						{
							break;
						}
	
						if (sem.GetSharedReadLocks() != 0)
						{
						/* This guy is locked for reading. We can't do anything. */
							error = ERRORCODE_Busy;
							break;
						}
					}
					pos += m_nHdrBlockSize + hdr.size;
				}
			}
		}

	/* If we are able to truncate, do it now. */

		if (error == ERRORCODE_None)
		{
			m_pDevice->seek(state.storage_start, ST_DEV_SEEK_SET);
			m_pDevice->truncate();

			state.storage_end = state.storage_start;
			state.free_start = POSITION_UNALLOCATED;
			state.free_biggest = ST_MAN_MAX_SIZE;
		}

	/* Unlock everything is we locked everything. */

		if (locked_all)
		{
			m_pDevice->lock(state.storage_start, lock_end, TRUE);
		}
		ModifyUnlockState();
	}

	return error;
}

/*
// Refresh the state.
*/

ERRORCODE StorageManager::RefreshState(void)
{
	ERRORCODE error;
	int nStateSize = SizeStateRecord();
	SHORT size = m_pDevice->size_record(nStateSize);

/* Lock the area if we're sharing. */

	if (IsSharing())
	{
		if ((error = m_pDevice->lock(m_state_position, size, FALSE)) != ERRORCODE_None)
		{
			return error;
		}
	}

/* Do the read. */

	if ((error = m_pDevice->seek(m_state_position, ST_DEV_SEEK_SET)) == ERRORCODE_None)
	{
		ST_DEV_IO_SIZE nReadSize;
		error = m_pDevice->read_record(&state, nStateSize, &nReadSize);

		// We need to read exactly what we expect.
		// The state record can not change size.
		if (error == ERRORCODE_None && nReadSize != (ST_DEV_IO_SIZE)nStateSize)
		{
			error = ERRORCODE_Read;
		}
	}

/* Unlock the area if we're sharing. */

	if (IsSharing())
	{
		m_pDevice->lock(m_state_position, size, TRUE);
	}

	return error;
}

/*
// Flush the state.
*/

ERRORCODE StorageManager::FlushState(void)
{
	ERRORCODE error;
	int nStateSize = SizeStateRecord();
	SHORT size = m_pDevice->size_record(nStateSize);

/* Lock the area if we're sharing. */

	if (IsSharing())
	{
		if ((error = m_pDevice->lock(m_state_position, size, FALSE)) != ERRORCODE_None)
		{
			return error;
		}
	}

/* Do the write. */

	if ((error = m_pDevice->seek(m_state_position, ST_DEV_SEEK_SET)) == ERRORCODE_None)
	{
		error = m_pDevice->write_record(&state, nStateSize);
	}

/* Unlock the area if we're sharing. */

	if (IsSharing())
	{
		m_pDevice->lock(m_state_position, size, TRUE);
	}

	return error;
}

/*
// Read the storage manager state from the current position in the
// storage device.
*/

ERRORCODE StorageManager::ReadState(BOOL validate, BOOL reset_semaphores)
{
	ERRORCODE error;

	m_pDevice->tell(&m_state_position);

	if ((error = RefreshState()) == ERRORCODE_None)
	{
		if (IsSharing())
		{
			m_pDevice->tell(&m_sem_position);

			if (validate && reset_semaphores)
			{
			/* Reset any read locks on the semaphore. */
				m_sem.Init(m_pDevice, m_sem_position);
				// Flush the state with the new user count.
				state.m_lUsers = 0;
				FlushState();
			}

			m_pDevice->seek(m_sem_position + m_sem.Size(), ST_DEV_SEEK_SET);
		}
		initialized = TRUE;
	}
	return error;
}

/*
// Write the storage manager state to the current position in the
// storage device. The storage manager must be initialized.
*/

ERRORCODE StorageManager::WriteState()
{
/* Make sure we're initialized. */

	if (!initialized)
	{
		return ERRORCODE_NotInitialized;
	}

/* Write our state. */

	ERRORCODE error;

	/*
	// 8/2/95 (JN)
	// It would be bad news to always write the state on exit.
	// In the sharing case, we may not have updated the state recently; we
	// may be writing old information back. So we skip over it in the
	// that case, letting the normal ModifyUnlockState() code handle writing
	// it when needed.
	// This could cause serious problems if three or more people are sharing
	// this device at once.
	*/

	if (IsSharing())
	{
	/* Skip over the semaphore. */
		error = m_pDevice->seek(SizeState(), ST_DEV_SEEK_CUR);
	}
	else
	{
		error = FlushState();
	}
	return error;
}

/*
// Return the size of the storage manager state.
*/

ST_DEV_IO_SIZE StorageManager::SizeState()
{
	ST_DEV_IO_SIZE size = m_pDevice->size_record(SizeStateRecord());

	if (IsSharing())
	{
		size += m_sem.Size();
	}
	return size;
}

/*
// Read lock the state.
*/

#if 0
ERRORCODE StorageManager::ReadLockState(void)
{
	ERRORCODE error = ERRORCODE_None;

	if (IsSharing())
	{
	/* Read lock the semaphore. */

		if ((error = m_sem.read_lock(m_pDevice, m_sem_position)) != ERRORCODE_None)
		{
			return error;
		}
		AddedLock();

	/* If the state changed, refresh it now. */

		if (sem.IsChanged())
		{
			if ((error = RefreshState()) != ERRORCODE_None)
			{
				m_sem.read_unlock(m_pDevice, m_sem_position);
				RemovedLock();
			}
		}
	}
	return error;
}
#endif

/*
// Read unlock the state.
*/

#if 0
ERRORCODE StorageManager::ReadUnlockState(void)
{
	ERRORCODE error = ERRORCODE_None;
	if (IsSharing())
	{
		error = m_sem.ReadUnlock(m_pDevice, m_sem_position, TRUE, IsExclusive());
		RemovedLock();
	}
	return error;
}
#endif

/*
// Mofidy lock the state.
// This locks the entire free chain.
*/

ERRORCODE StorageManager::ModifyLockState(void)
{
	ERRORCODE error = ERRORCODE_None;

	if (IsSharing())
	{
	/* Modify lock the semaphore. */

		if ((error = m_sem.ModifyLock(m_pDevice, m_sem_position, IsExclusive())) != ERRORCODE_None)
		{
			return error;
		}
		AddedLock();

	/* If the state changed, refresh it now. */

		if (m_sem.Changed())
		{
			if ((error = RefreshState()) != ERRORCODE_None)
			{
				m_sem.ModifyUnlock(m_pDevice, m_sem_position, FALSE, IsExclusive());
				RemovedLock();
			}
		}
	}
	return error;
}

/*
// Modify unlock the state.
*/

ERRORCODE StorageManager::ModifyUnlockState(void)
{
	if (IsSharing())
	{
		if (m_sem.GetModifyLocks() == 1)
		{
			FlushState();
		}
		m_sem.ModifyUnlock(m_pDevice, m_sem_position, TRUE, IsExclusive());
		RemovedLock();
	}
	return ERRORCODE_None;
}

/*
// Flush all storage manager data to the disk.
*/

ERRORCODE StorageManager::Flush()
{
	if (!initialized)
	{
		return ERRORCODE_NotInitialized;
	}

/* Make sure the device is flushed. */

	return m_pDevice->flush();
}

/*
// Read a storage block header.
// This is a private method.
*/

ERRORCODE StorageManager::ReadHeaderBlock(ST_MAN_HDR_BLOCK far *header, ST_DEV_POSITION where, BOOL do_lock)
{
	ERRORCODE error;

	do_lock = do_lock && IsSharing();

/* A value of -1 for where means don't seek. */

	if (where == -1L)
	{
		m_pDevice->tell(&where);
	}

/* Handle file locking in the sharing case. */

	if (do_lock)
	{
		if ((error = m_pDevice->lock(where, ST_MAN_HDR_SIZE, FALSE)) != ERRORCODE_None)
		{
			return error;
		}
	}

	if ((error = m_pDevice->seek(where, ST_DEV_SEEK_SET)) == ERRORCODE_None)
	{
	/* Only read 8 bits. */

		BYTE header_block[1 + 3 + sizeof(header->id)];
		if ((error = m_pDevice->read(header_block, sizeof(header_block))) == ERRORCODE_None)
		{
			header->type = header_block[0];
			header->size = 0;
#if 1
			((BYTE*)&header->size)[0] = header_block[1];
			((BYTE*)&header->size)[1] = header_block[2];
			((BYTE*)&header->size)[2] = header_block[3];
#else
			memcpy(&header->size, header_block+1, 3);
#endif
			header->id = *(ST_MAN_ID *)(header_block+1+3);
		}
	}

//	od("Read header - type:%d, id:%lx, size:%lx\r\n",
//				header->type, header->id, header->size);

	if (do_lock)
	{
		m_pDevice->lock(where, ST_MAN_HDR_SIZE, TRUE);
	}
	return error;
}

/*
// Write a storage block header.
// This is a private method.
*/

ERRORCODE StorageManager::WriteHeaderBlock(ST_MAN_HDR_BLOCK far *header, ST_DEV_POSITION where)
{
	ERRORCODE error;

/* A value of -1 for where means don't seek. */

	if (where == -1L)
	{
		m_pDevice->tell(&where);
	}

/* Handle file locking in the sharing case. */

	if (IsSharing())
	{
		if ((error = m_pDevice->lock(where, ST_MAN_HDR_SIZE, FALSE)) != ERRORCODE_None)
		{
			return error;
		}
	}

	if ((error = m_pDevice->seek(where, ST_DEV_SEEK_SET)) == ERRORCODE_None)
	{
#if 1
		BYTE header_block[1 + 3 + sizeof(header->id)];
		header_block[0] = (BYTE)header->type;
		memcpy(header_block+1, &header->size, 3);
		*(ST_MAN_ID *)(header_block+1+3) = header->id;

		error = m_pDevice->write(header_block, sizeof(header_block));
#else
		error = m_pDevice->write(header, sizeof(*header));
#endif
	}

	if (IsSharing())
	{
		m_pDevice->lock(where, ST_MAN_HDR_SIZE, TRUE);
	}
	return error;
}

/*
// Allocate some storage space.
*/

ERRORCODE StorageManager::AllocateStorage(ST_DEV_POSITION far *position, ST_MAN_SIZE size, ST_MAN_TYPE type, ST_MAN_ID id)
{
#ifdef STOREMAN_STATISTICS
	m_lAllocate++;
#endif

	ERRORCODE error;
	ST_DEV_POSITION new_position;
	ST_MAN_HDR_BLOCK hdr;

//	dump_free_list("AllocateStorage - begin");

/* Set position in case we get an error no one checks. */

	*position = -1L;

/* Make sure we're initialized. */

	if (!initialized)
	{
		return ERRORCODE_NotInitialized;
	}

/* Make sure the size is valid. */

	if (size > ST_MAN_MAX_SIZE)
	{
		return ERRORCODE_BadParameter;
	}

/* Make sure the type is valid. */

	if (type == ELEMENT_TYPE_FREE)
	{
		return ERRORCODE_NoType;
	}

/*
// Lock the state for modifications.
*/

	if ((error = ModifyLockState()) != ERRORCODE_None)
	{
		return error;
	}

/*
// For now, always allocate at the end of the storage heap.
// If we make more intelligent allocations (based on a free list or such)
// the calculations would probably go here.
*/

	new_position = POSITION_UNALLOCATED;

/*
// See if we have any free blocks to use.
*/

	if (size < state.free_biggest)
	{
	/*
 	// We should indeed bother to search the free list.
	// We recompute the biggest free in case it was lying.
 	*/

		ST_MAN_SIZE biggest = 0L;
		ST_DEV_POSITION last_header = POSITION_UNALLOCATED;

		for (new_position = state.free_start; new_position != POSITION_UNALLOCATED;)
		{
			ST_MAN_HDR_BLOCK alloc_hdr;

		/* Read the header to see how big it is. */

			if ((error = ReadHeaderBlock(&alloc_hdr, new_position)) != ERRORCODE_None)
			{
			/* Not a good block in the free list. Try to ignore it. */
				new_position = POSITION_UNALLOCATED;
				break;
			}

			if (alloc_hdr.size > biggest)
			{
				biggest = alloc_hdr.size;
			}

			if (alloc_hdr.size >= size)
			{
			/*
 			// We found a block big enough.
			// Carve a block from this to satisfy our needs.
 			*/
				if (alloc_hdr.size <= size + m_nHdrBlockSize)
				{
				/*
 				// There is not enough left to make another free block.
				// Just keep the block this big.
 				*/
					size = alloc_hdr.size;

				/* Remove the entire block from the list. */

					if (last_header == 0)
					{
					/* State holds us! Unlink us from the state. */
						state.free_start = alloc_hdr.id;
					}
					else
					{
					/* Unlink us from the last header. */
						ST_MAN_HDR_BLOCK last_hdr;

						ReadHeaderBlock(&last_hdr, last_header);
						last_hdr.id = alloc_hdr.id;
						WriteHeaderBlock(&last_hdr, last_header);
					}
				}
				else
				{
				/*
				// We found a block which is big enough for this request with
				// another block left over.
				*/
				/* Allocate from the tail of this block. */

					alloc_hdr.size -= m_nHdrBlockSize + size;
					WriteHeaderBlock(&alloc_hdr, new_position);
					new_position += m_nHdrBlockSize + alloc_hdr.size;
				}

			/*
 			// We have removed some free memory.
			// We don't know what the state of the biggest block is anymore.
			// Set it to max. This will force the next allocation to search the
			// free list and recompute the biggest block if it fails.
			*/
				state.free_biggest = ST_MAN_MAX_SIZE;
				break;
			}
			last_header = new_position;
			new_position = alloc_hdr.id;
		}

		if (new_position == POSITION_UNALLOCATED)
		{
		/* Didn't find a block though we thought we could. Update biggest. */
			state.free_biggest = biggest;
		}
	}

/*
// Stop-gap: No appropriate free block.
// Stick at end of file.
*/

	if (new_position == POSITION_UNALLOCATED)
	{
		new_position = state.storage_end;

	/* Check the size since the storage area will be enlarging. */

		if (new_position + size > m_size_limit)
		{
		/* The storage area is full. */
			ModifyUnlockState();
			return ERRORCODE_Full;
		}
	}

/* Build a header to write for this storage block. */

	hdr.size = size;
	hdr.type = type;
	hdr.id = id;

/* Write the header. */

	if ((error = WriteHeaderBlock(&hdr, new_position)) == ERRORCODE_None)
	{
	/* Initialize the sharing area if sharing. */

		if (IsSharing())
		{
			RW_Semaphore sem;

			sem.Init(m_pDevice, new_position + ST_MAN_HDR_SIZE);
		}

	/* Set position. */

		*position = new_position + m_nHdrBlockSize;

	/* Update the end of storage. */

		ST_DEV_POSITION new_end;

		if ((new_end = *position + size) > state.storage_end)
		{
//			TRACE("Expand storage arena to %ld\n", new_end);
			state.storage_end = new_end;
		}
	}

//	dump_free_list("allocate_storage - end");

	ModifyUnlockState();

	return error;
}

/*
// Dump the free storage list.
*/

#ifdef DUMP_LIST
VOID StorageManager::dump_free_list(LPSTR s)
{
	ST_DEV_POSITION pos;

	if (s != NULL)
	{
		od("Dumping @ '%s'\r\n", s);
	}

	for (pos = state.free_start; pos != POSITION_UNALLOCATED; )
	{
		ST_MAN_HDR_BLOCK hdr;
		ERRORCODE error;

		if ((error = ReadHeaderBlock(&hdr, pos)) != ERRORCODE_None)
		{
			od("Got error %d reading block @ %lx\r\n", error, pos);
			break;
		}

		od("Block @ %lx - type %d, size %ld, next %lx\r\n",
						pos, hdr.type, hdr.size, hdr.id);
		if (hdr.type != ELEMENT_TYPE_FREE)
		{
			od("Error in free list - illegal type (%d)!\r\n", hdr.type);
			break;
		}
		pos = hdr.id;
	}
	od("Done with dump.\r\n");

	if (s != NULL)
	{
	/*
	// An additional test.
	// Dump all storage elements from front to end of file.
	*/

		for (pos = state.storage_start; pos < state.storage_end; )
		{
			ST_MAN_HDR_BLOCK hdr;
			ERRORCODE error;

			if ((error = ReadHeaderBlock(&hdr, pos)) != ERRORCODE_None)
			{
				od("Got error %d reading block @ %lx\r\n", error, pos);
				break;
			}

			od("@%lx: type %d, size %ld, id %lx\r\n",
						pos, hdr.type, hdr.size, hdr.id);

			pos += hdr.size + m_nHdrBlockSize;
		}
	}
}
#endif

/*
// Free some storage.
*/

ERRORCODE StorageManager::FreeStorage(ST_DEV_POSITION position)
{
#ifdef STOREMAN_STATISTICS
	m_lFree++;
#endif

	ERRORCODE error;
	ST_MAN_HDR_BLOCK hdr, next_hdr;

//	od("free %ld; ", position);
//	dump_free_list("FreeStorage - begin");

/* Make sure we're initialized. */

	if (!initialized)
	{
		return ERRORCODE_NotInitialized;
	}

/*
// Lock the state for modifications.
*/

	if ((error = ModifyLockState()) != ERRORCODE_None)
	{
		return error;
	}

/* Back up to the start of the header. */

	position -= m_nHdrBlockSize;

/* Validate the position. */

	if (position < state.storage_start || position >= state.storage_end)
	{
		ModifyUnlockState();
		return ERRORCODE_BadParameter;
	}

/* Read the header block. */

	if ((error = ReadHeaderBlock(&hdr, position)) == ERRORCODE_None)
	{
	/* If it's already free, that's an error. */

		if (hdr.type == ELEMENT_TYPE_FREE)
		{
		/* Block is not allocated. */
			ModifyUnlockState();
			return ERRORCODE_NotAllocated;
		}

	/*
	// Free the storage.
 	// Free areas are stored in ascending order.
	*/

		if (state.free_start == POSITION_UNALLOCATED
				|| state.free_start > position)
		{
		/* New block goes in the front. */
			hdr.id = state.free_start;		/* Next is start. */
			state.free_start = position;
		}
		else
		{
			ST_DEV_POSITION search_pos = state.free_start;

		/* We need to find the place to insert. */

			for (;;)
			{
			/* Read the next free block in the chain. */

				if ((error = ReadHeaderBlock(&next_hdr, search_pos)) != ERRORCODE_None)
				{
					ModifyUnlockState();
					return error;
				}

			/* See if we insert here or not. */

				if (next_hdr.id == POSITION_UNALLOCATED || next_hdr.id > position)
				{
				/* Insert here! */
//					od("Insert %ld after %ld...\r\n", position, search_pos);
					hdr.id = next_hdr.id;
					next_hdr.id = position;
					break;
				}
				search_pos = next_hdr.id;
			}

		/* See if we can compact this block with the one being inserted. */

			if (search_pos + m_nHdrBlockSize + next_hdr.size == position)
			{
			/* We can compact the one being inserted. */
//				od("Compact %ld and %ld...\r\n", search_pos, position);

				hdr.size += m_nHdrBlockSize + next_hdr.size;
				position = search_pos;
			}
			else
			{
//				od("No compact; write header %ld\r\n", search_pos);
			/* Just write this header back out. */
				WriteHeaderBlock(&next_hdr, (ST_DEV_POSITION)search_pos);
			}
		}

		ST_DEV_POSITION end = position + m_nHdrBlockSize + hdr.size;

	/* See if we can compact the next block. */

		if (hdr.id == end)
		{
			ReadHeaderBlock(&next_hdr, (ST_DEV_POSITION)hdr.id);
			hdr.size += m_nHdrBlockSize + next_hdr.size;		/* Suck it up. */
			hdr.id = next_hdr.id;
		}

	/* Mark us as free. */

//		od("Write %ld\r\n", position);
		hdr.type = ELEMENT_TYPE_FREE;
		WriteHeaderBlock(&hdr, position);

	/* Update the biggest! */

		if (hdr.size > state.free_biggest)
		{
			state.free_biggest = hdr.size;
		}

	/*
	// If this free block is now at the end of the file, we can make the
	// file smaller and throw away the free block.
	*/

		if (position + m_nHdrBlockSize + hdr.size == state.storage_end)
		{
		/*
		// This free block is at the end of the file! We can truncate.
		// Search the free list for this block.
		*/
			BOOL found = FALSE;

			if (state.free_start == position)
			{
				state.free_start = POSITION_UNALLOCATED;
				found = TRUE;
			}
			else
			{
				ST_DEV_POSITION search_pos = state.free_start;

			/* We need to find the place to delete. */

				for (;;)
				{
				/* Read the next free block in the chain. */

					if ((error = ReadHeaderBlock(&next_hdr, search_pos)) != ERRORCODE_None)
					{
						ModifyUnlockState();
						return error;
					}

				/* See if we insert here or not. */

					if (next_hdr.id == POSITION_UNALLOCATED)
					{
					/* Reached the end without finding ourselves! */
						break;
					}

					if (next_hdr.id == position)
					{
						found = TRUE;
						next_hdr.id = POSITION_UNALLOCATED;
						WriteHeaderBlock(&next_hdr, search_pos);
						break;
					}
					search_pos = next_hdr.id;
				}
			}

			if (found)
			{
			/* Shrink the storage area. */

//				TRACE("Shrink the storage arena to %ld\r\n", position);
				m_pDevice->seek(state.storage_end = position, ST_DEV_SEEK_SET);
				m_pDevice->truncate();
			}
		}
	}

	ModifyUnlockState();

//	dump_free_list("FreeStorage - end");

	return error;
}

/*
// Expand a data block into an existing free block.
// It is assumed that the free block has been verified to hold enough room.
*/

ERRORCODE StorageManager::ExpandBlock(ST_MAN_HDR_BLOCK hdr,
					ST_DEV_POSITION pos,
					ST_MAN_HDR_BLOCK free_hdr,
 					ST_DEV_POSITION free_pos,
					ST_MAN_SIZE expand_amount)
{
	ST_DEV_POSITION link_position;
	ERRORCODE error;

//	dump_free_list("ExpandBlock - begin");

/*
// See if we need to make another free block.
*/

	if (free_hdr.size <= expand_amount)
	{
	/*
	// After we remove what we want, the free block won't have
	// enough space left to be considered another free block.
	// Just suck the whole block up into ours.
	*/
		hdr.size += free_hdr.size + m_nHdrBlockSize;
		link_position = free_hdr.id;		/* Link to next block. */
	}
	else
	{
	/*
	// There is enough left over for a new free block.
	// Create one after taking what we need.
	*/
		hdr.size += expand_amount;
		free_hdr.size -= expand_amount;

		link_position = free_pos + expand_amount;		/* Link to this block. */

	/* Write this free block. */

		if ((error = WriteHeaderBlock(&free_hdr, link_position)) != ERRORCODE_None)
		{
			return error;
		}
	}

/* Write the expanded block. */

	if ((error = WriteHeaderBlock(&hdr, pos)) != ERRORCODE_None)
	{
		return error;
	}

/*
// Now we need to go through and update the free list where it points to the
// free block. The location pointing to "free_pos" now needs to point to
// "link_position".
*/

	ST_DEV_POSITION search_pos = state.free_start;

	if (search_pos == free_pos)
	{
	/* Free entry is first in the list. Move it to the new position. */
		state.free_start = link_position;
	}
	else
	{
	/* We need to find the place to insert. */

		for (search_pos = state.free_start; search_pos != POSITION_UNALLOCATED;)
		{
			ST_MAN_HDR_BLOCK search_hdr;

		/* Read the next free block in the chain. */

			if ((error = ReadHeaderBlock(&search_hdr, search_pos)) != ERRORCODE_None)
			{
				return error;
			}

		/* See if we insert here or not. */

			if (search_hdr.id == free_pos)
			{
			/* We found it. Do the link. */

				search_hdr.id = link_position;

				WriteHeaderBlock(&search_hdr, search_pos);

				break;
			}
			search_pos = search_hdr.id;
		}
	}

	/*
	// If search_pos went to POSITION_ALLOCATED, then we reached the end of the
	// free chain without finding the block we just used part of.
	// We didn't find the block! This is a major error!
	// What this means is that we used a free block not in the list or that
	// we used a part of storage that wasn't really free.
	// The new free block is also now not in the list.
	// The storage arena may be corrupted.
	// We leave things as they are and just ignore this. In the best case,
	// the file will just have missing free space. In the worst case, the
	// file is corrupt; but then, it was corrupt already anyway.
	*/

//	dump_free_list("ExpandBlock - end");

	return ERRORCODE_None;
}

/*
// Resize a storage block.
// NOTE: If this routine fails, things are left the way that they were to
// the extent that that's possible (i.e. previously allocated memory remains
// allocated, etc.)
*/

ERRORCODE StorageManager::ResizeStorage(ST_DEV_POSITION far *current_position, ST_MAN_SIZE new_size, StorageElementPtr element)
{
#ifdef STOREMAN_STATISTICS
	m_lResize++;
#endif

	ERRORCODE error;
	ST_MAN_HDR_BLOCK hdr;
	ST_DEV_POSITION old_position = *current_position;
	ST_DEV_POSITION hdr_position = old_position - m_nHdrBlockSize;

/* Make sure we're initialized. */

	if (!initialized)
	{
		return ERRORCODE_NotInitialized;
	}

/*
// Lock the state for modifications.
*/

	if ((error = ModifyLockState()) != ERRORCODE_None)
	{
		return error;
	}

/* Validate the position. */

	if (hdr_position < state.storage_start || hdr_position >= state.storage_end)
	{
		ModifyUnlockState();
		return ERRORCODE_BadParameter;
	}

/* Get the block. */

	if ((error = ReadHeaderBlock(&hdr, hdr_position)) == ERRORCODE_None)
	{
		if (hdr.type == ELEMENT_TYPE_FREE)
		{
		/* Area is free! This is an error. */
			ModifyUnlockState();
			return ERRORCODE_NotAllocated;
		}

	/* See how the new size compares with the current size. */

		if (new_size == hdr.size)
		{
		/* The block stays the same size. */
		}
		else if (new_size > hdr.size)
		{
		/*
		// See if the block just above us is a free block.
		// If so, see if it has enough room.
		*/
			ST_DEV_POSITION next_pos;

			next_pos = hdr_position + m_nHdrBlockSize + hdr.size;

			if (next_pos < state.storage_end)
			{
				ST_MAN_HDR_BLOCK next_hdr;

				if ((error = ReadHeaderBlock(&next_hdr, next_pos)) != ERRORCODE_None)
				{
					ModifyUnlockState();
					return error;
				}

			/* Compute how much we need. */

				ST_MAN_SIZE needed_size = new_size - hdr.size;

				if (next_hdr.type == ELEMENT_TYPE_FREE
						&& (next_hdr.size+m_nHdrBlockSize) >= needed_size)
				{
					if ((error = ExpandBlock(hdr, hdr_position, next_hdr, next_pos, needed_size)) != ERRORCODE_None)
					{
						ModifyUnlockState();
						return error;
					}

				/* "Move" the data into this new area. */

					ModifyUnlockState();
					return element->RelocateData(m_pDevice, old_position, old_position);
				}
			}

		/*
		// We weren't able to expand our block. Allocate a new block.
		// Allocate new storage first.
		// That way, if it fails, we still have the old position.
		*/
			if ((error = AllocateStorage(current_position, new_size, hdr.type, hdr.id)) == ERRORCODE_None)
			{
			/* See if we were passed an element. If so, inform it of the move. */

				if (element != NULL)
				{
				/* Let the element relocate any data it may want saved. */
					if ((error = element->RelocateData(m_pDevice, old_position, *current_position)) != ERRORCODE_None)
					{
						*current_position = old_position;
						ModifyUnlockState();
						return error;
					}
				}

			/* Safe to deallocate old now. */

				FreeStorage(old_position);
			}
		}
		else
		{
		/* Getting smaller! Leave it alone for now. */
		/* See if we were passed an element. If so, inform it of the "move". */

			if (element != NULL)
			{
			/* Let the element relocate any data it may want saved. */

				ModifyUnlockState();
				return element->RelocateData(m_pDevice, old_position, old_position);
			}
		}
	}
	ModifyUnlockState();
	return error;
}

/*
// Force a new device to be used.
// This routine will fail if the storage manager has been initialized.
*/

ERRORCODE StorageManager::ForceNewDevice(StorageDevicePtr new_device)
{
	if (initialized)
	{
	/* Sorry. Too late. */
		return ERRORCODE_Busy;
	}

/* Get around the 'const' modifier without having to remove it. */

	*(StorageDevicePtr far *)&m_pDevice = new_device;
	return ERRORCODE_None;
}

/*
// Rebuild the free list.
// We simply march through storage and add all free blocks into the chain in
// the order we find them.
*/

ERRORCODE StorageManager::RebuildFreeList(void)
{
	ERRORCODE error;

/* Lock the state for modification. */

	if ((error = ModifyLockState()) != ERRORCODE_None)
	{
		return error;
	}

/* Rebuild the free list by scanning general storage. */

	SHORT const hdr_size = HdrBlockSize();
	ST_DEV_POSITION last_free_position = POSITION_UNALLOCATED;
	ST_MAN_HDR_BLOCK last_free_hdr;

/* Loop through all storage elements. */

	for (ST_DEV_POSITION pos = state.storage_start; pos < state.storage_end; )
	{
		ST_MAN_HDR_BLOCK hdr;

	/*
	// Read the next header block.
	*/

		if ((error = ReadHeaderBlock(&hdr, pos)) != ERRORCODE_None)
		{
			break;
		}

	/*
	// If this is a free block, hook it into the chain.
	*/

		if (hdr.type == ST_TYPE_FREE)
		{
		/* We need to add this to the chain. */

			if (last_free_position == POSITION_UNALLOCATED)
			{
			/* First block. Hook into start. */
				state.free_start = pos;
			}
			else
			{
			/* Hook to last free block. */
				last_free_hdr.id = pos;
				if ((error = WriteHeaderBlock(&last_free_hdr, last_free_position)) != ERRORCODE_None)
				{
					break;
				}
			}

		/* Update the "last" variables. */

			last_free_hdr = hdr;
			last_free_position = pos;
		}

	/*
	// Skip to the next header block.
	*/

		pos += hdr_size + hdr.size;
	}

/* Handle the last block (or empty state). */

	if (error == ERRORCODE_None)
	{
		if (last_free_position == POSITION_UNALLOCATED)
		{
		/* First block. Hook into start. */
			state.free_start = POSITION_UNALLOCATED;
		}
		else
		{
		/* Hook to last free block. */
			last_free_hdr.id = POSITION_UNALLOCATED;
			error = WriteHeaderBlock(&last_free_hdr, last_free_position);
		}

	}

	ModifyUnlockState();

	return error;
}

/*
// IntegrityCheck()
//
// Perform an integrity check on the storage area. This will also reset
// the read locks on all storage element semaphores if requested.
// *** NOTE: It is assumed that the file is not being shared (in the
// file access sense) at this point if all read locks are to be reset. ***
*/

ERRORCODE StorageManager::IntegrityCheck(BOOL reset_semaphores)
{
/* Validate the general storage. */

	ERRORCODE error;
	RW_Semaphore sem;				/* A blank semaphore for general use. */
	SHORT const hdr_size = HdrBlockSize();
	ST_DEV_POSITION next_free_block = state.free_start;
	BOOL free_list_valid = TRUE;			/* Optimism! */

/* Loop through all storage elements. */

	for (ST_DEV_POSITION pos = state.storage_start; pos < state.storage_end; )
	{
		ST_MAN_HDR_BLOCK hdr;

	/*
	// Read the next header block.
	*/

		if ((error = ReadHeaderBlock(&hdr, pos)) != ERRORCODE_None)
		{
			return error;
		}

	/*
	// Do a simple validation of the header.
	*/

		if (hdr.type == 0
				|| pos + hdr.size > state.storage_end)
		{
			return ERRORCODE_InvalidRecord;
		}

	/*
	// If this is supposedly the next free block, validate it.
	*/

		if (pos == next_free_block)
		{
		/*
		// Make sure this is a good free header.
		// A good free header:
		// (1) has type ST_TYPE_FREE
		// (2) has a next free link greater than its own position (if not end)
		*/

			if (hdr.type != ST_TYPE_FREE
					|| (hdr.id != POSITION_UNALLOCATED && hdr.id <= pos))
			{
				free_list_valid = FALSE;
			}
			else
			{
				next_free_block = hdr.id;
			}
		}
		else if (hdr.type == ST_TYPE_FREE)
		{
		/*
		// This header says 'free' even though it's not in the free list.
		// Either the free list is corrupted or general storage is corrupted.
		// Let's assume that the free list is just corrupt. If general storage
		// is messed up, we'll probably see it sooner or later in this routine.
		*/
			free_list_valid = FALSE;
		}

	/*
	// Reset the semaphore if desired and we're sharing.
	*/ 

		if (IsSharing() && reset_semaphores)
		{
		/*
		// We call init() here. This just writes the blank semaphore to the
		// file. We don't really care about maintaining modification counts
		// since it's only used for "dirty notification" anyway.
		*/
			if ((error = sem.Init(m_pDevice, pos + ST_MAN_HDR_SIZE)) != ERRORCODE_None)
			{
				return error;
			}
		}

	/*
	// Skip to the next header block.
	*/

		pos += hdr_size + hdr.size;
	}

/*
// Now handle the free list.
// If we didn't find the end of the free chain or we hit an "unfree" block,
// we rebuild.
*/

	if (!free_list_valid || next_free_block != POSITION_UNALLOCATED)
	{
		error = RebuildFreeList();
	}

	return ERRORCODE_None;
}

/***************************/
/* Storage Element methods */
/***************************/

/*
// The constructor for a storage element.
*/

StorageElement::StorageElement(void)
{
	manager = NULL;
	hdr.type = 0;
	hdr.id = 0;
	hdr.size = ST_MAN_NO_SIZE;
	m_position = POSITION_UNALLOCATED;
	m_modified = FALSE;
}

void StorageElement::Init(ST_MAN_TYPE type, ST_MAN_ID id, StorageManagerPtr stman)
{
	manager = stman;
	hdr.type = type;
	hdr.id = id;
}

/*
// Allocate storage for this storage element.
// This is called by write(), so it's not usual for this routine to be called
// elsewhere (though that doesn't mean it doesn't happen!).
// If the item is already allocated and hasn't changed size, it will remain
// in the same place (GUARANTEED).
*/

ERRORCODE StorageElement::Allocate(ST_MAN_TYPE type, ST_MAN_ID id, ST_MAN_SIZE size)
{
	ERRORCODE error;

/* Generate the size if not already generated. */

	if (size == ST_MAN_NO_SIZE)
	{
		size = SizeofData(manager->m_pDevice);
	}

/* If this is unallocated, allocate it now. */

	if (m_position == POSITION_UNALLOCATED)
	{
	/* Allocate storage for us. */

		error = manager->AllocateStorage(&m_position, size, type, id);
	}
	else
	{
	/* Make sure our storage is the right size. */

		error = manager->ResizeStorage(&m_position, size, this);
	}

/*
// Do a little internal update in case the header changed.
// There are cases where more storage is allocated than requested. This
// keeps the element in sync with the actual value in the storage.
*/

	if (error == ERRORCODE_None)
	{
		error = manager->ReadHeaderBlock(&hdr, m_position - manager->HdrBlockSize());
	}

	return error;
}

/*
// Deallocate the space allotted to this storage element.
// If successful, *where will equal POSITION_UNALLOCATED.
*/

ERRORCODE StorageElement::Deallocate(void)
{
	ERRORCODE error = ERRORCODE_None;

/* See if we've even been allocated. */

	if (m_position != POSITION_UNALLOCATED)
	{
	/* We've been allocated; deallocate us. */

		if ((error = manager->FreeStorage(m_position)) == ERRORCODE_None)
		{
		/* All done! We're now unallocated. */
			m_position = POSITION_UNALLOCATED;
		}
	}
	return error;
}

/*
// read() a storage element into memory.
// Basic checking is done to see if this is the right element.
// To disable such checking, create a StorageElement with type ST_TYPE_ANY.
*/

ERRORCODE StorageElement::Read(ST_DEV_POSITION where)
{
	ST_MAN_HDR_BLOCK hdr_read;
	ERRORCODE error;
	SHORT hdr_size = manager->HdrBlockSize();

#ifdef STOREMAN_STATISTICS
	manager->m_lElementRead++;
#endif

/* Handle the incoming. */

	if (where == POSITION_UNKNOWN)
	{
		where = m_position;
	}
	else
	{
		m_position = where;
	}

/* Lock the header for reading (but don't recursively read!). */

	if ((error = ReadLock(TRUE)) != ERRORCODE_None)
	{
		return error;
	}

/* Read the header into the temp hdr. */

	if ((error = manager->ReadHeaderBlock(&hdr_read, where-hdr_size)) == ERRORCODE_None)
	{
#if 0
		if (hdr.type == ELEMENT_TYPE_RECORD)
		{
			TRACE("Hdr @ %08lx : type: %u, size: %u, id: %u\n",
					where-hdr_size, hdr_read.type, hdr_read.size, hdr_read.id);
		}
#endif

	/* Validate the header if not disabled. */

		if (hdr.type == ST_TYPE_ANY
				|| (hdr.type == hdr_read.type && hdr.id == hdr_read.id))
		{
		/* No header checking or headers match. */

			hdr = hdr_read;

		/* Invoke the object to read the specific data. */

			if ((error = manager->m_pDevice->seek(where, ST_DEV_SEEK_SET)) == ERRORCODE_None)
			{
				if (manager->IsSharing())
				{
					manager->m_pDevice->lock(where, hdr.size, FALSE);
				}
				error = ReadData(manager->m_pDevice);
				if (manager->IsSharing())
				{
					manager->m_pDevice->lock(where, hdr.size, TRUE);
				}
			}
		}
		else
		{
		/* Invalid header. They don't match. */
			error = ERRORCODE_TypeMismatch;
		}
	}

/* We're done reading. Unlock the header if we're locking. */

	ReadUnlock();

/* No longer modified. */

	m_modified = FALSE;

	return error;
}

/*
// write() a storage element to memory.
// The element is allocated space in the storage device by the storage
// manager as necessary.
*/

ERRORCODE StorageElement::Write(ST_DEV_POSITION far *where)
{
	ERRORCODE error;
	ST_DEV_POSITION limit_start, limit_end;
	ST_MAN_SIZE size;

#ifdef STOREMAN_STATISTICS
	manager->m_lElementWrite++;
#endif

//	TRACE("write - type: %d, id: %ld\n", type(), id());
//	manager->dump_free_list(NULL);

/* Generate the size for this element. */

	size = SizeofData(manager->m_pDevice);

/* See if we need to (re)allocate. */

	if (m_position == POSITION_UNALLOCATED || size != hdr.size)
	{
		if ((error = Allocate(hdr.type, hdr.id, hdr.size=size)) != ERRORCODE_None)
		{
		/* Error allocating. */
			return error;
		}
	}

/* Send the position back if desired. */

	if (where != NULL)
	{
		*where = m_position;
	}

/* Compute the write limits. */

	limit_start = m_position;
	limit_end = limit_start + hdr.size;

//	TRACE("write - type: %d, id: %ld (%ld - %ld)\n", Type(), Id(), limit_start, limit_end);

/* Set the I/O limits. */

	if ((error = manager->m_pDevice->io_limits(limit_start, limit_end)) == ERRORCODE_None)
	{
	/* The header is already written. Just seek and write the data. */

		if ((error = manager->m_pDevice->seek(limit_start, ST_DEV_SEEK_SET)) == ERRORCODE_None)
		{
		/* Invoke the object to write the specific data. */

			if (manager->IsSharing())
			{
				manager->m_pDevice->lock(limit_start, hdr.size, FALSE);
			}
			error = WriteData(manager->m_pDevice);
			if (manager->IsSharing())
			{
				manager->m_pDevice->lock(limit_start, hdr.size, TRUE);
			}
		}

	/* Always reset the I/O limits back to full, even if error. */

		manager->m_pDevice->io_limits();
	}

//	manager->dump_free_list("write - end");

	m_modified = FALSE;

	return error;
}

/*
// Read lock a storage element.
*/

ERRORCODE StorageElement::ReadLock(BOOL is_reading)
{
#ifdef STOREMAN_STATISTICS
	ASSERT(Type() >= 0 && Type() <= 4);
	(manager->m_lElementReadLock[Type()])++;
#endif

	ERRORCODE error = ERRORCODE_None;
	ST_DEV_POSITION where = m_position;

/* We only need to lock if we are sharing. */

	if (manager->IsSharing() && where != POSITION_UNALLOCATED)
	{
		SHORT sem_offset = ST_MAN_HDR_SIZE - manager->HdrBlockSize();

		if ((error = m_sem.ReadLock(manager->m_pDevice, where + sem_offset, manager->IsExclusive())) == ERRORCODE_None)
		{
			manager->AddedLock();
		/* We got the lock. If the data has changed, re-read it. */

			if (!is_reading && m_sem.Changed())
			{
			/* Read the element. */
				if ((error = Read()) != ERRORCODE_None)
				{
				/* Error! Unlock and report home. */
					m_sem.ReadUnlock(manager->m_pDevice, where + sem_offset, manager->IsExclusive());
					manager->RemovedLock();
					return error;
				}
			}
		}
	}
	return error;
}

/*
// Read unlock a storage element.
*/

ERRORCODE StorageElement::ReadUnlock(void)
{
#ifdef STOREMAN_STATISTICS
	ASSERT(Type() >= 0 && Type() <= 4);
	(manager->m_lElementReadUnlock[Type()])++;
#endif

	ERRORCODE error = ERRORCODE_None;
	ST_DEV_POSITION where = m_position;

	if (manager->IsSharing() && where != POSITION_UNALLOCATED)
	{
		SHORT sem_offset = ST_MAN_HDR_SIZE - manager->HdrBlockSize();

		error = m_sem.ReadUnlock(manager->m_pDevice, where + sem_offset, manager->IsExclusive());
		manager->RemovedLock();
	}
	return error;
}

/*
// Modify lock a storage element.
*/

ERRORCODE StorageElement::ModifyLock(void)
{
	ERRORCODE error;
	ST_DEV_POSITION where = m_position;

#ifdef STOREMAN_STATISTICS
	ASSERT(Type() >= 0 && Type() <= 4);
	(manager->m_lElementModifyLock[Type()])++;
#endif

	if (!manager->IsSharing() || where == POSITION_UNALLOCATED)
	{
		return ERRORCODE_None;
	}

/* See if we need to lock. */

#if 0
//	od("Modify lock (id:%ld, type: %d, where:%ld, rd:%d, mod:%d)\r\n",
			hdr.id, hdr.type, where,
			m_sem.get_read_lock_count(),
			m_sem.get_modify_lock_count());
#endif

	SHORT sem_offset = ST_MAN_HDR_SIZE - manager->HdrBlockSize();

	if ((error = m_sem.ModifyLock(manager->m_pDevice, where + sem_offset, manager->IsExclusive())) != ERRORCODE_None)
	{
		return error;
	}
	manager->AddedLock();

/* If the semaphore has changed, the data is new. Re-read it. */

	if (m_sem.Changed())
	{
		if ((error = Read(where)) != ERRORCODE_None)
		{
		/* Error! Unlock and report home. */
			m_sem.ModifyUnlock(manager->m_pDevice, where + sem_offset, FALSE, manager->IsExclusive());
			manager->RemovedLock();
			return error;
		}
	}

	return error;
}

/*
// Modify unlock a storage element.
*/

ERRORCODE StorageElement::ModifyUnlock(void)
{
	ST_DEV_POSITION where = m_position;

#ifdef STOREMAN_STATISTICS
	ASSERT(Type() >= 0 && Type() <= 4);
	(manager->m_lElementModifyUnlock[Type()])++;
#endif

/* See if we need to lock. */

	if (manager->IsSharing()
			&& where != POSITION_UNALLOCATED)
	{
#if 0
//		od("Modify unlock (id:%ld, type: %d, where:%ld, rd:%d, mod:%d)\r\n",
			hdr.id, hdr.type, where,
			m_sem.get_read_lock_count(),
			m_sem.get_modify_lock_count());
#endif

		SHORT sem_offset = ST_MAN_HDR_SIZE - manager->HdrBlockSize();
		BOOL was_modified = IsElementModified();

		if (m_sem.GetModifyLocks() == 1 && was_modified)
		{
		/* We need to write this element. */
			Write();			
		}

		m_sem.ModifyUnlock(manager->m_pDevice, where + sem_offset, was_modified, manager->IsExclusive());
		manager->RemovedLock();

		if (where != m_position)
		{
		/* It changed position! Initialize the sem at the new location. */
			m_sem.Init(manager->m_pDevice, m_position + sem_offset);
		}
	}
	return ERRORCODE_None;
}

/*
// Drop all locks on this storage element.
*/

ERRORCODE StorageElement::DropAllLocks(void)
{
	ST_DEV_POSITION where = m_position;
	ERRORCODE error = ERRORCODE_None;

	if (manager->IsSharing()
			&& where != POSITION_UNALLOCATED)
	{
		SHORT sem_offset = ST_MAN_HDR_SIZE - manager->HdrBlockSize();

		int nLocks = m_sem.GetReadLocks() + m_sem.GetModifyLocks();
		error = m_sem.DropAllLocks(manager->m_pDevice, where + sem_offset, manager->IsExclusive());
		manager->RemovedLock(nLocks);
	}
	return error;
}

#if 0
/*
// In sharing cases, refresh the storage element from the file.
*/

ERRORCODE StorageElement::Refresh(BOOL do_lock)
{
	ERRORCODE error;
	ST_DEV_POSITION where = m_position;

/* See if there is anything to refresh. */

	if (!manager->IsSharing() || where == POSITION_UNALLOCATED)
	{
		return ERRORCODE_None;
	}

/* We need to read the header to get the mod count. */

	SHORT hdr_size = manager->HdrBlockSize();

	ST_DEV_POSITION hdr_where = where - hdr_size;

/* Lock the header to ensure we're safe to read. */

	WORD old_mod_count = mod_count();
	BOOL needs_read = TRUE;

	if (do_lock)
	{
		if ((error = manager->ReadLockHeader(hdr_where)) != ERRORCODE_None)
		{
			return error;
		}
	}

	if ((manager->ReadHeaderBlock(&hdr, hdr_where)) == ERRORCODE_None)
	{
		if (mod_count() == old_mod_count)
		{
			needs_read = FALSE;
		}
	}

/* If we locked, unlock. */

	if (do_lock)
	{
		manager->ReadUnlockHeader(hdr_where);
	}

/* Re-read the element if it needs it. */

	if (needs_read)
	{
		error = read(where, do_lock);
	}

	return error;
}
#endif

