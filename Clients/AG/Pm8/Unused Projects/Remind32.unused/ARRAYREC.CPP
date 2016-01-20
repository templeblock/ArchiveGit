/*
// $Header: /PM8/REMIND32/ARRAYREC.CPP 1     3/03/99 6:26p Gbeddow $
//
// $Log: /PM8/REMIND32/ARRAYREC.CPP $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   14 Aug 1997 15:18:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:08   Fred
// Initial revision.
// 
//    Rev 1.17   06 Jan 1997 15:45:26   Jay
// Optimized BlockOfIndex.
// 
//    Rev 1.16   05 Nov 1996 14:06:42   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.15   01 Sep 1996 16:28:40   Fred
// New non-recursive FreeNextBlock()
// 
//    Rev 1.14   25 Jul 1996 13:14:40   Jay
// Fixed a bug.
// 
//    Rev 1.13   17 Jul 1996 12:53:20   Jay
// Speed ups. New 'exclusive' algorithm
// 
//    Rev 1.12   28 Jun 1996 16:18:08   Jay
//  
// 
//    Rev 1.11   26 Jun 1996 14:21:34   Jay
//  
// 
//    Rev 1.10   24 Jun 1996 18:06:04   Jay
//  
// 
//    Rev 1.9   19 Jun 1996 13:45:46   Jay
//  
// 
//    Rev 1.8   14 Jun 1996 17:05:34   Jay
//  
// 
//    Rev 1.7   10 Jun 1996 08:27:42   Jay
// Now calls modified() in places it needs to.
// 
//    Rev 1.6   04 Jun 1996 08:43:44   Jay
//  
// 
//    Rev 1.5   24 May 1996 16:15:52   Fred
// TRACEx
// 
//    Rev 1.4   25 Apr 1996 10:37:42   Jay
// New final release notify code
// 
//    Rev 1.3   24 Apr 1996 08:17:14   Fred
// Moved Array record type
// 
//    Rev 1.2   23 Apr 1996 08:07:44   Jay
// More new stuff
// 
//    Rev 1.1   11 Apr 1996 15:35:32   Jay
//  
*/

#include "stdafx.h"
#include "arrayrec.h"

#ifdef WIN32
#define MEMCPY memcpy
#define MEMMOVE memmove
#else
//#include "util.h"
//#define MEMCPY hmemcpy
//#define MEMMOVE(pDst, pSrc, lCount) Util::HMemMove((char __huge*)pSrc, (char __huge*)pDst, lCount)
#define MEMCPY memcpy
#define MEMMOVE memmove
#endif

/////////////////////////////////////////////////////////////////////////////
// CArrayElementRecord

/*
// The creator for an array element record.
*/

ERRORCODE CArrayElementRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	CArrayElementRecord* pElement = NULL;

/* Create the new array record. */

	TRY
		pElement = new CArrayElementRecord(number, type, owner, where);
	END_TRY

	if (pElement == NULL)
	{
		return ERRORCODE_Memory;
	}
	*record = (DatabaseRecordPtr)pElement;

	if (creation_data != NULL)
	{
	// Handle the creation data.
	}
	return ERRORCODE_None;
}

/*
// The constructor for an array element record.
*/

CArrayElementRecord::CArrayElementRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	memset(&m_Record, 0, sizeof(m_Record));
	m_pNextBlock = NULL;
	m_lDataPosition = POSITION_UNKNOWN;
	m_pData = NULL;
	m_dwElementsAllocated = 0;			// Nothing allocated elements
}

/*
// The destructor for an array element record.
*/

CArrayElementRecord::~CArrayElementRecord()
{
	FreeNextBlock();
	FreeData();
}

/*
// Set the next block.
*/

void CArrayElementRecord::SetNextBlock(DB_RECORD_NUMBER NextBlock)
{
	if (m_Record.m_NextBlock != NextBlock)
	{
		// Get rid of any next pointer we have.
		FreeNextBlock();
		m_Record.m_NextBlock = NextBlock;
		modified();
	}
}

/*
// Free any cached data for the next block.
*/

void CArrayElementRecord::FreeNextBlock(void)
{
	if (m_pNextBlock != NULL)
	{
		CPtrArray m_Blocks;
		CArrayElementRecord* pBlock = NULL;

		// Collect the block numbers of the next blocks in an array.
		// The next block pointer of each collected block is set to NULL.
		TRY
		{
			pBlock = OwnNextBlock();
			while ((m_Blocks.GetSize() < 1000) && (pBlock != NULL))
			{
				// If this throws a memory exception, we'll end up calling
				// release on pBlock and then releasing whatever was collected
				// in the array. The result is the same as if the array limit
				// was reduced.
				m_Blocks.Add(pBlock);

				// Move to the next block.
				pBlock = pBlock->OwnNextBlock();
			}
		}
		END_TRY

		// If we reached the maximum array size without reaching the last block, then
		// recursively free the next chunk (through FinalReleaseNotify()).
		if (pBlock != NULL)
		{
			pBlock->release();
			pBlock = NULL;
		}

		// Free the blocks we collected in reverse order. Their next block
		// pointers have all been set to NULL so FinalReleaseNotify() will call
		// FreeNextBlock() and nothing will happen.
		for (int nBlock = m_Blocks.GetSize()-1; nBlock >= 0; nBlock--)
		{
			((CArrayElementRecord*)(m_Blocks.GetAt(nBlock)))->release();
		}
	}
}

void CArrayElementRecord::FreeData(void)
{
	if (m_pData != NULL)
	{
		delete [] m_pData;
		m_pData = NULL;
	}
	m_dwElementsAllocated = 0;
}

/*
// Get a pointer to the next block.
*/

CArrayElementRecord* CArrayElementRecord::AccessNextBlock(void)
{
	if (m_pNextBlock == NULL)
	{
		if (m_Record.m_NextBlock != 0)
		{
			ERRORCODE error;

			if ((m_pNextBlock = (CArrayElementRecord*)database->get_record(m_Record.m_NextBlock, &error, type())) == NULL)
			{
				ThrowErrorcodeException(error);
			}
		}
	}

	return m_pNextBlock;
}

/*
// Return the pointer to the next block to it's new owner.
// Set out pointer to NULL.
*/

CArrayElementRecord* CArrayElementRecord::OwnNextBlock(void)
{
	CArrayElementRecord* pNextBlock = m_pNextBlock;
	m_pNextBlock = NULL;
	return pNextBlock;
}

/*
// Return the address of a particular element within the data block.
*/

LPBYTE CArrayElementRecord::ElementAddress(DWORD dwIndex)
{
	ASSERT(m_pData != NULL);
	if (m_pData == NULL)
	{
		ThrowErrorcodeException(ERRORCODE_NotAllocated);
	}
	return ((LPBYTE)m_pData) + SizeOfElements(dwIndex);
}

/*
// Set the array data block size (in elements).
*/

void CArrayElementRecord::SetAllocatedCount(DWORD dwCount)
{
	PageInData();
	DoSetAllocatedCount(dwCount);
}

void CArrayElementRecord::DoSetAllocatedCount(DWORD dwCount)
{
	if (dwCount != m_dwElementsAllocated)
	{
		DWORD dwNewSize = SizeOfElements(dwCount);
		DWORD dwOldSize = DataSize();

#ifndef WIN32
		if (dwNewSize > 0x0000fffeL)
		{
			ThrowErrorcodeException(ERRORCODE_Full);
		}
#endif
		LPBYTE pNewBlock = NULL;
		if (dwNewSize != 0)
		{
			pNewBlock = new BYTE[(unsigned int)dwNewSize];
			ASSERT(pNewBlock != NULL);
		}

		// Copy any old data over.
		if (m_pData != NULL)
		{
			if (pNewBlock != NULL)
			{
				DWORD dwCopySize = (dwNewSize < dwOldSize) ? dwNewSize : dwOldSize;
				memcpy(pNewBlock, m_pData, (size_t)dwCopySize);
			}
			delete [] m_pData;
		}
		// This is our new data block.
		m_pData = pNewBlock;
		// Remember how many elements we allocated.
		m_dwElementsAllocated = dwCount;
		// We are now modified.
		modified();
	}
}

/*
// Insert one or more elements into the array.
*/

void CArrayElementRecord::Insert(DWORD dwIndex, LPVOID pElement, DWORD dwInsertCount /*=1*/)
{
	PageInData();

	if (dwIndex > ElementCount())
	{
		ASSERT(FALSE);
		ThrowErrorcodeException(ERRORCODE_BadParameter);
	}

	// Make sure we have enough room in the array.

	if (ElementCount() + dwInsertCount > m_dwElementsAllocated)
	{
		ThrowErrorcodeException(ERRORCODE_Full);
	}

	LPBYTE pDest = ElementAddress(dwIndex);

	if (dwIndex != ElementCount())
	{
	// Move existing data to make a hole for our insertion.
		DWORD dwEnd = dwIndex+dwInsertCount;
		LPBYTE pEnd = ElementAddress(dwEnd);
		MEMMOVE(pEnd, pDest, (size_t)SizeOfElements(ElementCount() - dwIndex));
	}
// Insert the data.
	MEMCPY(pDest, pElement, (size_t)SizeOfElements(dwInsertCount));

	m_Record.m_dwElementCount += dwInsertCount;
	modified();
}

/*
// Delete one or more elements from the array.
*/

void CArrayElementRecord::Delete(DWORD dwIndex, DWORD dwElements /*=1*/)
{
	PageInData();

	if (dwIndex >= ElementCount())
	{
		ThrowErrorcodeException(ERRORCODE_BadParameter);
	}

	DWORD dwEnd = dwIndex + dwElements;
	if (dwEnd >= m_Record.m_dwElementCount)
	{
		// Not trying to delete too much, are we?
		ASSERT(dwEnd == m_Record.m_dwElementCount);
	/* Truncate. */
		m_Record.m_dwElementCount = dwIndex;
	}
	else
	{
	/* Compact data. */
		LPBYTE pSource = ElementAddress(dwEnd);
		LPBYTE pDest = ElementAddress(dwIndex);
	/* This is a non-overlapping copy. */
		MEMCPY(pDest, pSource, (size_t)SizeOfElements(m_Record.m_dwElementCount-dwEnd));

		m_Record.m_dwElementCount -= dwElements;
	}
	// We are now modified.
	modified();
}

/*
// Get a particular element in the array.
*/

LPVOID CArrayElementRecord::GetAt(DWORD dwIndex)
{
	PageInData();

	if (dwIndex >= ElementCount())
	{
		ASSERT(FALSE);
		ThrowErrorcodeException(ERRORCODE_BadParameter);
	}
	return (LPVOID)ElementAddress(dwIndex);
}

/*
// Read the data for this record.
*/

ERRORCODE CArrayElementRecord::ReadData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

/*
// Read the main record.
*/

	if ((error = pDevice->read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
	/* Remember where our data is. */

		error = pDevice->tell(&m_lDataPosition);
	}

/*
// At this point, we don't read any actual array data.
// We don't know where the first read will be.
*/

	return error;
}

/*
// Write the data for this record.
*/

ERRORCODE CArrayElementRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;
/*
// Write the structure header.
*/

	if ((error = device->write_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		// Write the data!
		DWORD dwDataSize = DataSize();

		// Update the data position (probably not necessary).
		device->tell(&m_lDataPosition);

		if (m_pData != NULL && dwDataSize != 0)
		{
			error = device->huge_write(m_pData, dwDataSize);
		}
	}
	return error;
}

/*
// Return the size of the record.
*/

ST_MAN_SIZE CArrayElementRecord::SizeofData(StorageDevicePtr pDevice)
{
//	return pDevice->size_record(sizeof(m_Record)) + DataSize();
	return pDevice->size_record(sizeof(m_Record))
			+ SizeOfElements(database->IsSharing()
											? m_dwElementsAllocated
											: m_Record.m_dwElementCount);
}

/*
// Relocate our array data.
*/

ERRORCODE CArrayElementRecord::RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position)
{
	return ERRORCODE_None;
}

DWORD CArrayElementRecord::DataSize(void)
{
	return SizeOfElements(m_Record.m_dwElementCount);
}

void CArrayElementRecord::PageInData(void)
{
	if (m_pData == NULL && m_Record.m_dwElementCount != 0)
	{
		ASSERT(m_lDataPosition != POSITION_UNKNOWN);
		if (m_lDataPosition == POSITION_UNKNOWN)
		{
		/*
		// We need data, yet don't know where to get it from.
		// Something is corrupt.
		*/
			ThrowErrorcodeException(ERRORCODE_Corrupt);
		}
		DoSetAllocatedCount(m_Record.m_dwElementCount);

		ASSERT(m_pData != NULL);

		ASSERT(database != NULL);
		StorageDevice* pDevice = database->get_storage_device();

		ASSERT(pDevice != NULL);

		ERRORCODE error;
		if ((error = pDevice->seek(m_lDataPosition, ST_DEV_SEEK_SET)) != ERRORCODE_None
			 || (error = pDevice->huge_read(m_pData, DataSize())) != ERRORCODE_None)
		{
			ThrowErrorcodeException(error);
		}
	}
}

/*
// Final release notification.
// We need this to manage our subrecords.
*/

void CArrayElementRecord::FinalReleaseNotify(void)
{
	if (database->IsSharing())
	{
		FreeNextBlock();
	}
}

/*
// The database is going away. All records are being freed.
// Release any sub-records we have locked.
*/

void CArrayElementRecord::ReleaseSubrecords(void)
{
	FreeNextBlock();
}

/*
// Assign method.
*/

ERRORCODE CArrayElementRecord::assign(DatabaseRecordRef record)
{
	ERRORCODE error = ERRORCODE_None;
/* Assign the base record first. */
	TRY
	{
		// Do the base assign first.
		if ((error = DatabaseRecord::assign(record)) == ERRORCODE_None)
		{
			CArrayElementRecord& SourceRecord = (CArrayElementRecord&)record;
			m_Record.m_dwElementSize = SourceRecord.ElementSize();
			m_Record.m_dwElementCount = SourceRecord.ElementCount();
			// We leave the "next block" number alone. It is set externally and
			// is not our responsibility (short of not changing it).

			// Free any old data.
			FreeData();

			// If there is data to copy...
			if (m_Record.m_dwElementCount != 0)
			{
				// Page in the source data so we can copy it.
				SourceRecord.PageInData();

				// Allocate the space we need for the copy.
				DoSetAllocatedCount(m_Record.m_dwElementCount);

				// Copy the data.
				if (SourceRecord.m_pData == NULL || m_pData == NULL)
				{
					// Some pointer is not valid.
					ASSERT(FALSE);
				}
				else
				{
					memcpy(m_pData, SourceRecord.m_pData, (size_t)(ElementSize()*ElementCount()));
				}
			}
		}
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH(CMemoryException, e)
	{
		error = ERRORCODE_Memory;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL
	return error;
}

/////////////////////////////////////////////////////////////////////////////
// CArrayRecord

/*
// Insert one or more elements into the array.
*/

void CArrayRecord::Insert(DWORD dwIndex, LPVOID pElement, DWORD dwInsertSize /*=1*/)
{
	if (dwIndex > ElementCount())
	{
		ASSERT(FALSE);
		ThrowErrorcodeException(ERRORCODE_BadParameter);
	}

/*
// Get the block of the passed index.
*/

	CArrayElementRecord* pBlock = NULL;
	TRY
	{
		DWORD dwIndexInBlock;
		BlockOfIndex(dwIndex, pBlock, dwIndexInBlock);

	/*
	// Loop until all elements have been inserted.
	*/

		while (dwInsertSize != 0)
		{
		/* Compute how much will fit in this block. */

			DWORD dwRoomInBlock = (pBlock == NULL)
												? 0
												: m_Record.m_dwMaxBlockElements - pBlock->ElementCount();
			if (dwRoomInBlock < 0)
			{
			/* Should never be negative. Make sure it's not. */
				ASSERT(FALSE);
				dwRoomInBlock = 0;
			}

		/* Compute how much of what we need to insert will fit in this block. */

			DWORD dwThisInsert = dwInsertSize;
			if (dwThisInsert > dwRoomInBlock)
			{
				dwThisInsert = dwRoomInBlock;
			}

		/*
		// Figure out what to do with this block: insert or split.
		*/

			if (dwThisInsert == 0)
			{
			/*
			// Need to create a new block.
			*/
				SplitBlockForInsert(pBlock, dwIndexInBlock);
			}
			else
			{
			/* Room in block. Do the insert here. */
				InsertIntoBlock(pBlock, dwIndexInBlock, pElement, dwThisInsert);

			/* Update the total array size. */
				m_Record.m_dwElementCount += dwThisInsert;

			/*
			// Update the loop variables.
			*/
				dwInsertSize -= dwThisInsert;
				dwIndexInBlock += dwThisInsert;
				pElement = ((LPBYTE)pElement) + SizeOfElements(dwThisInsert);
			}
		}
		modified();
	}
	CATCH_ALL(e)
	{
		THROW_LAST();
	}
	END_CATCH_ALL
}

/*
// Delete one or more elements from the array.
*/

void CArrayRecord::Delete(DWORD dwIndex, DWORD dwDeleteCount /*=1*/)
{
	// Validate the starting offset.
	if (dwIndex >= ElementCount())
	{
		ThrowErrorcodeException(ERRORCODE_BadParameter);
	}

	// Validate the count.
	if (dwIndex + dwDeleteCount > ElementCount())
	{
		dwDeleteCount = ElementCount() - dwIndex;
	}

	CArrayElementRecord* pBlock;
	CArrayElementRecord* pParentBlock;
	DWORD dwIndexInBlock;

	// Compute the block and index for the start of the deletion.
	BlockOfIndex(dwIndex, pBlock, dwIndexInBlock, &pParentBlock);
	if (pBlock == NULL)
	{
		ASSERT(FALSE);
		ThrowErrorcodeException(ERRORCODE_BadParameter);
	}

	// Loop while we have elements to delete.
	while (dwDeleteCount != 0)
	{
		// Compute how much to delete.
		DWORD dwThisCount = pBlock->ElementCount()-dwIndexInBlock;
		if (dwThisCount > dwDeleteCount)
		{
			dwThisCount = dwDeleteCount;
		}

		// Do the delete.
		pBlock->Delete(dwIndexInBlock, dwThisCount);
		// Update the total array size.
		m_Record.m_dwElementCount -= dwThisCount;
		// Update the loop variables.
		dwDeleteCount -= dwThisCount;

		// If the block now has no elements in it, delete it.
		if (pBlock->ElementCount() == 0)
		{
			// We want to delete the block.
			DB_RECORD_NUMBER lBlock = pBlock->Id();
			DB_RECORD_NUMBER lNextBlock = pBlock->GetNextBlock();
			pBlock->SetNextBlock(0);

			if (pParentBlock == NULL)
			{
				// First block in chain.
				SetFirstBlock(lNextBlock);			// Releases pBlock.
				pBlock = AccessFirstBlock();
			}
			else
			{
				// Block with parent.
				pParentBlock->SetNextBlock(lNextBlock);	// Releases pBlock.
				pBlock = pParentBlock->AccessNextBlock();
			}
			// And delete the record itself.
			database->delete_record(lBlock, type()+1);

			ResetRecentBlock();
		}
		else
		{
			// Continue deleting from the start of the next block.
			pParentBlock = pBlock;
			pBlock = pBlock->AccessNextBlock();
		}
		dwIndexInBlock = 0;
	}
	modified();
}

/*
// Access an element in the array.
*/

LPVOID CArrayRecord::GetAt(DWORD dwIndex)
{
	CArrayElementRecord* pBlock;
	DWORD dwIndexInBlock;

	BlockOfIndex(dwIndex, pBlock, dwIndexInBlock);

	if (pBlock == NULL)
	{
		ASSERT(FALSE);
		ThrowErrorcodeException(ERRORCODE_BadParameter);
	}
	return pBlock->GetAt(dwIndexInBlock);
}

/*
// Access an element in the array, and report back how many are
// contiguously accessible through the returned pointer.
*/

LPVOID CArrayRecord::GetAt(DWORD dwIndex, DWORD* pElementsAvailable)
{
	CArrayElementRecord* pBlock;
	DWORD dwIndexInBlock;

	BlockOfIndex(dwIndex, pBlock, dwIndexInBlock);

	if (pBlock == NULL)
	{
		ASSERT(FALSE);
		ThrowErrorcodeException(ERRORCODE_BadParameter);
	}
	*pElementsAvailable = pBlock->ElementCount() - dwIndexInBlock;
	return pBlock->GetAt(dwIndexInBlock);
}

/*
// Access an element in the array with the intent of modifying it.
*/

LPVOID CArrayRecord::ModifyAt(DWORD dwIndex)
{
	CArrayElementRecord* pBlock;
	DWORD dwIndexInBlock;

	BlockOfIndex(dwIndex, pBlock, dwIndexInBlock);

	if (pBlock == NULL)
	{
		ASSERT(FALSE);
		ThrowErrorcodeException(ERRORCODE_BadParameter);
	}
	pBlock->modified();
	return pBlock->GetAt(dwIndexInBlock);
}

/*
// Set elements in the array.
*/

ERRORCODE CArrayRecord::SetAt(DWORD dwIndex, LPVOID pElement, DWORD dwSize /*=1*/)
{
// Not implemented.
	ASSERT(FALSE);
	return ERRORCODE_IntError;
}

/*
// Insert elements into a block.
// It is up to the caller to ensure that the result will not exceed the
// maximum block size.
*/

void CArrayRecord::InsertIntoBlock(CArrayElementRecord* pBlock, DWORD dwIndex, LPVOID pElement, DWORD dwInsertCount /*=1*/)
{
	DWORD dwElementCount = pBlock->ElementCount();

	ASSERT(dwElementCount + dwInsertCount <= m_Record.m_dwMaxBlockElements);

/*
// Make sure there is enough room in the block before inserting.
*/

	if (dwInsertCount + dwElementCount > pBlock->GetAllocatedCount())
	{
		DWORD dwNewCount = dwElementCount + dwInsertCount + ElementIncrement();
		if (dwNewCount > m_Record.m_dwMaxBlockElements)
		{
			dwNewCount = m_Record.m_dwMaxBlockElements;
		}
		pBlock->SetAllocatedCount(dwNewCount);
	}

/*
// Do the insertion.
*/

	pBlock->Insert(dwIndex, pElement, dwInsertCount);
}

/*
// Split a block at insertion time.
// This will update the passed variables to allow the insert to take place.
*/

void CArrayRecord::SplitBlockForInsert(CArrayElementRecord*& pBlock, DWORD& dwIndexInBlock)
{
/*
// We need to create a new block.
*/

	// Allocate the new block.
	CArrayElementRecord* pNewBlock = (CArrayElementRecord*)database->new_record(type()+1, NULL);
	if (pNewBlock == NULL)
	{
		ThrowErrorcodeException(database->last_creation_error());
	}
	pNewBlock->ElementSize(ElementSize());

/*
// Hook the new block into the chain.
// Move the data necessary for the split.
*/

	if (pBlock == NULL)
	{
	/*
	// Hook us into the chain at the front.
	*/
		pNewBlock->SetNextBlock(0);
		SetFirstBlock(pNewBlock->Id());
		pNewBlock->release();	// Release the creation count.

		// Access the new block through the chain
		pNewBlock = AccessFirstBlock();

	/*
	// No data to move. Insertion point is start of new block.
	*/
		pBlock = pNewBlock;
		dwIndexInBlock = 0;
	}
	else
	{
	/*
	// Hook us into the chain after the last block.
	*/

		pNewBlock->SetNextBlock(pBlock->GetNextBlock());
		pBlock->SetNextBlock(pNewBlock->Id());
		pNewBlock->release();	// Release the creation count.

		// Access the new block through the current block.
		pNewBlock = pBlock->AccessNextBlock();

	/*
	// We will split the old block in half.
	*/

		DWORD dwCountA = pBlock->ElementCount()/2;
		DWORD dwCountB = pBlock->ElementCount() - dwCountA;

		// Move it to the new block.
		InsertIntoBlock(pNewBlock, 0, pBlock->GetAt(dwCountA), dwCountB);
		// Remove it from the old block.
		pBlock->Delete(dwCountA, dwCountB);

		if (dwIndexInBlock > dwCountA)
		{
			// Insertion point is in the new block.
			pBlock = pNewBlock;
			dwIndexInBlock -= dwCountA;
		}
	}
}

/*
// Find the block containing the index (or the nearest closest).
*/

void CArrayRecord::BlockOfIndex(DWORD dwIndex, CArrayElementRecord*& pBlock, DWORD& dwIndexInBlock, CArrayElementRecord** ppParentBlock /*=NULL*/)
{
 	CArrayElementRecord* pSearchBlock;
	CArrayElementRecord* pParentBlock;
	DWORD dwCurrentIndex;

	if (m_pRecentBlock == NULL
			|| dwIndex < m_dwRecentBlockStart
			|| database->IsSharing())

	{
		// The recent block is not usable. Start at the front.
		pSearchBlock = AccessFirstBlock();
		pParentBlock = NULL;
		dwCurrentIndex = 0;
	}
	else
	{
		pSearchBlock = m_pRecentBlock;
		pParentBlock = m_pRecentBlockParent;
		dwCurrentIndex = m_dwRecentBlockStart;
	}

/*
// March along the blocks until we either find the one we want or run out.
*/

	while (pSearchBlock != NULL)
	{
		CArrayElementRecord* pNextBlock;
		pNextBlock = pSearchBlock->AccessNextBlock();

	/*
	// Two cases for breakage:
	// (1) The index is within this block.
	// (2) We are about to run off the end.
	// Break out and set us to this block.
	*/

		DWORD dwElementCount = pSearchBlock->ElementCount();
		if ((dwCurrentIndex + dwElementCount) > dwIndex
				|| pNextBlock == NULL)
		{
			break;
		}

		dwCurrentIndex += dwElementCount;
		pParentBlock = pSearchBlock;
		pSearchBlock = pNextBlock;
	}

/*
// Set the return values.
*/

	pBlock = pSearchBlock;
	dwIndexInBlock = dwIndex - dwCurrentIndex;
	if (ppParentBlock != NULL)
	{
		*ppParentBlock = pParentBlock;
	}

	m_pRecentBlock = pSearchBlock;
	m_pRecentBlockParent = pParentBlock;
	m_dwRecentBlockStart = dwCurrentIndex;
}

/*
// Set the first block.
*/

void CArrayRecord::SetFirstBlock(DB_RECORD_NUMBER FirstBlock)
{
	if (m_Record.m_FirstBlock != FirstBlock)
	{
		// Get rid of any first pointer we have.
		FreeFirstBlock();
		m_Record.m_FirstBlock = FirstBlock;
		modified();
	}
}

/*
// Free any cached data for the first block.
*/

void CArrayRecord::FreeFirstBlock(void)
{
	if (m_pFirstBlock != NULL)
	{
		m_pFirstBlock->release();
		m_pFirstBlock = NULL;
	}
	ResetRecentBlock();
}

/*
// Get a pointer to the first block.
*/

CArrayElementRecord* CArrayRecord::AccessFirstBlock(void)
{
	if (m_pFirstBlock == NULL)
	{
		if (m_Record.m_FirstBlock != 0)
		{
			ERRORCODE error;

			if ((m_pFirstBlock = (CArrayElementRecord*)database->get_record(m_Record.m_FirstBlock, &error, type()+1)) == NULL)
			{
				ThrowErrorcodeException(error);
			}
		}
	}

	return m_pFirstBlock;
}

/*
// The creator for an array record.
*/

ERRORCODE CArrayRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	CArrayRecord* pArray = NULL;

/* Create the new array record. */

	TRY
		pArray = new CArrayRecord(number, type, owner, where);
	END_TRY

	if (pArray == NULL)
	{
		return ERRORCODE_Memory;
	}
	*record = (DatabaseRecordPtr)pArray;

	if (creation_data != NULL)
	{
	// Handle the creation data.
	}
	return ERRORCODE_None;
}

/*
// The constructor for an array record.
*/

CArrayRecord::CArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	memset(&m_Record, 0, sizeof(m_Record));
	m_pFirstBlock = NULL;
	m_Record.m_dwElementIncrement = 8;

	ResetRecentBlock();
}

/*
// The destructor for an array record.
*/

CArrayRecord::~CArrayRecord()
{
	FreeFirstBlock();
}

void CArrayRecord::ResetRecentBlock(void)
{
	m_pRecentBlock = NULL;
	m_pRecentBlockParent = NULL;
	m_dwRecentBlockStart = 0;
}

/*
// Read the data for this record.
*/

ERRORCODE CArrayRecord::ReadData(StorageDevicePtr pDevice)
{
/*
// Read the main record.
*/

	ERRORCODE error = pDevice->read_record(&m_Record, sizeof(m_Record));

/*
// Read anything else.
*/

	return error;
}

/*
// Write the data for this record.
*/

ERRORCODE CArrayRecord::WriteData(StorageDevicePtr pDevice)
{
/*
// Read the main record.
*/

	ERRORCODE error = pDevice->write_record(&m_Record, sizeof(m_Record));

	return error;
}

/*
// Return the size of the record.
*/

ST_MAN_SIZE CArrayRecord::SizeofData(StorageDevicePtr pDevice)
{
	return pDevice->size_record(sizeof(m_Record));
}

/*
// Relocate our array data.
*/

ERRORCODE CArrayRecord::RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position)
{
	return ERRORCODE_None;
}

/*
// Assign method.
*/

ERRORCODE CArrayRecord::assign(DatabaseRecordRef srecord)
{
/* Assign the base record first. */
	ERRORCODE error = ERRORCODE_None;
/* Assign the base record first. */
	TRY
	{
		// Do the base assign first.
		if ((error = DatabaseRecord::assign(srecord)) == ERRORCODE_None)
		{
			CArrayRecord& SourceRecord = (CArrayRecord&)srecord;
			Database* pSourceDatabase = SourceRecord.database;
			Database* pDestDatabase = database;

			// Make sure we have no data yet.
			destroy();

			// Copy the record information over.
			m_Record.m_dwElementCount = SourceRecord.m_Record.m_dwElementCount;
			ElementSize(SourceRecord.ElementSize());
			ElementIncrement(SourceRecord.ElementIncrement());
			// We leave the "first block" number alone. It is set externally and
			// is not our responsibility (short of not changing it).
			MaxBlockElements(SourceRecord.MaxBlockElements());

			// Duplicate all the blocks.
			DB_RECORD_NUMBER lBlock = SourceRecord.GetFirstBlock();
			if (lBlock != 0)
			{
				// Copy the first block.
				SetFirstBlock(pSourceDatabase->duplicate_record(lBlock, pDestDatabase));
				// Access the first blocks so we can loop.
				CArrayElementRecord* pSourceBlock = SourceRecord.AccessFirstBlock();
				CArrayElementRecord* pDestBlock = AccessFirstBlock();
				while ((lBlock = pSourceBlock->GetNextBlock()) != 0)
				{
					// Duplicate the next block in the chain.
					pDestBlock->SetNextBlock(pSourceDatabase->duplicate_record(lBlock, pDestDatabase));
					// Move on to the next blocks.
					pSourceBlock = pSourceBlock->AccessNextBlock();
					pDestBlock = pDestBlock->AccessNextBlock();
				}
			}
		}
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH(CMemoryException, e)
	{
		error = ERRORCODE_Memory;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	ASSERT(error == ERRORCODE_None);
	return error;
}

/*
// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

void CArrayRecord::destroy(void)
{
	TRY
	{
		CArrayElementRecord* pBlock;
		while ((pBlock = AccessFirstBlock()) != NULL)
		{
			DB_RECORD_NUMBER lBlock = pBlock->Id();
		/* Get the next block record number. */
			DB_RECORD_NUMBER lNext = pBlock->GetNextBlock();
			pBlock->SetNextBlock(0);
		/* Set the next one. */
			SetFirstBlock(lNext);
		/* Zap this one. */
			database->delete_record(lBlock);
		}
	}
	END_TRY
}

/*
// Final release notification.
// We need this to manage our subrecords.
*/

void CArrayRecord::FinalReleaseNotify(void)
{
	if (database->IsSharing())
	{
		FreeFirstBlock();
	}
}

/*
// The database is going away. All records are being freed.
// Release any sub-records we have locked.
*/

void CArrayRecord::ReleaseSubrecords(void)
{
	FreeFirstBlock();
}

void CArrayRecord::FreeAllBlockData(void)
{
	CArrayElementRecord* pBlock;
	for (pBlock = AccessFirstBlock(); pBlock != NULL; pBlock = pBlock->AccessNextBlock())
	{
		// Maybe there should be some check whether the block is dirty?
		pBlock->FreeData();
	}
}

void CArrayRecord::BlockStatistics(void)
{
	DWORD dwBlockCount = 0;
	DWORD dwTotalCount = 0;
	DWORD dwEmptyBlockCount = 0;
	DWORD dwSmallestBlock = 0;
	DWORD dwLargestBlock = 0;

	CArrayElementRecord* pBlock;
	for (pBlock = AccessFirstBlock(); pBlock != NULL; pBlock = pBlock->AccessNextBlock())
	{
		DWORD dwElementCount = pBlock->ElementCount();
		dwTotalCount += dwElementCount;
		if (dwBlockCount == 0)
		{
			dwSmallestBlock = dwLargestBlock = dwElementCount;
		}
		else
		{
			if (dwElementCount < dwSmallestBlock)
			{
				dwSmallestBlock = dwElementCount;
			}
			if (dwElementCount > dwLargestBlock)
			{
				dwLargestBlock = dwElementCount;
			}
		}

		if (dwElementCount == 0)
		{
			dwEmptyBlockCount++;
		}
		dwBlockCount++;
	}

	if (dwBlockCount == 0)
	{
		TRACE0("DUMP: <No blocks>\n");
	}
	else
	{
		TRACE("DUMP: Elts=%lu, Blks=%lu, Empty=%lu, Min=%lu, Max=%lu, Avg=%lu\n",
					dwTotalCount,
					dwBlockCount,
					dwEmptyBlockCount,
					dwSmallestBlock,
					dwLargestBlock,
					dwTotalCount/dwBlockCount);
	}
}

void CArrayRecord::SelfTest(void)
{
	BOOL fError = FALSE;
	ERRORCODE error;		// General usage.

	TRY
	{
		ElementSize(sizeof(int));
		MaxBlockElements(100);
		ElementIncrement(8);

		int i;

		TRACE0("Add elements...\n");
		for (i = 0; i < 100000; i++)
		{
			Add((LPVOID)&i, 1);
		}
		BlockStatistics();

		TRACE0("Flush #1\n");
		if ((error = database->flush()) != ERRORCODE_None)
		{
			TRACE1("Error %d flushing (#1)\n", error);
			ThrowErrorcodeException(error);
		}
		FreeAllBlockData();

		ASSERT(ElementCount() == 100000);
		TRACE0("Read elements...\n");
		for (i = 0; i < 100000; i++)
		{
			int* pi = (int*)GetAt(i);
			int ReadI = (pi == NULL) ? -1 : *pi;
			if (ReadI != i)
			{
				TRACE2("index %d has value %d\n", i, ReadI);
				fError = TRUE;
				break;		/* And don't keep going. */
			}
		}
		if (!fError)
		{
			TRACE0("Delete elements...\n");
			for (i = 0; i < 2807; i++)
			{
				Delete(0, 1);
			}		
			BlockStatistics();

			ASSERT(ElementCount() == 100000-2807);
			TRACE0("Read elements after delete...\n");
			for (i = 2807; i < 100000; i++)
			{
				int* pi = (int*)GetAt(i-2807);
				int ReadI = (pi == NULL) ? -1 : *pi;
				if (ReadI != i)
				{
					TRACE2("index %d has value %d\n", i, ReadI);
					fError = TRUE;
					break;		/* And don't keep going. */
				}
			}
		}
		if (!fError)
		{
			TRACE0("Delete all elements...\n");
			Delete(0, ElementCount());
			BlockStatistics();

			if (ElementCount() != 0)
			{
				TRACE1("Delete all did not work (%d left).\n", ElementCount());
				fError = TRUE;
			}
		}

		if (!fError)
		{
			TRACE0("Add elements #2...\n");
			int Data[7] = { 0, 1, 2, 3, 4, 5, 6 };

			for (i = 0; i < 100000; i++)
			{
				DWORD dwIndex = ((ElementCount()/7+1)/2)*7;
				Insert(dwIndex, (LPVOID)Data, 7);
			}
			BlockStatistics();

			TRACE0("Flush #2\n");
			if ((error = database->flush()) != ERRORCODE_None)
			{
				TRACE1("Error %d flushing (#2)\n", error);
				ThrowErrorcodeException(error);
			}

			FreeAllBlockData();

			ASSERT(ElementCount() == 100000*7);
			TRACE0("Read elements after insert #2...\n");
			for (i = 0; i < 100000*7; i++)
			{
				int* pi = (int*)GetAt(i);
				int ReadI = (pi == NULL) ? -1 : *pi;
				if (ReadI != i%7)
				{
					TRACE3("index %d has value %d (not %d)\n", i, ReadI, i%7);
					fError = TRUE;
					break;		/* And don't keep going. */
				}
			}

			if (!fError)
			{
				TRACE0("Flush #3\n");
				if ((error = database->flush()) != ERRORCODE_None)
				{
					TRACE1("Error %d flushing (#3)\n", error);
					ThrowErrorcodeException(error);
				}

				FreeAllBlockData();

				TRACE0("Delete elements #2...\n");
#ifdef WIN32
				UINT wTestCount = 100000;
#else
				UINT wTestCount = 314;
#endif
				for (i = wTestCount; --i != 0;)
				{
					Delete(i*7, 1);
				}
				BlockStatistics();

				ASSERT(ElementCount() == wTestCount*6+1);
			}
		}
	}
	CATCH_ALL(e)
	{
		fError = TRUE;
		TRACE0("Got an exception.\n");
	}
	END_CATCH_ALL
}

/////////////////////////////////////////////////////////////////////////////
// CRecordArrayRecord

CRecordArrayRecord::CRecordArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
	: CArrayRecord(number, type, owner, where)
{
	ElementSize(sizeof(DB_RECORD_NUMBER));	// 4 bytes
	MaxBlockElements(2048);						// 4*2048 = 8192 bytes
	ElementIncrement(32);						// 4*32 = 128
}

/*
// The creator for a text state array record.
*/

ERRORCODE CRecordArrayRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */

	*record = NULL;
	TRY
	{
		*record = new CRecordArrayRecord(number, type, owner, where);
	}
	END_TRY

	return (*record == NULL) ? ERRORCODE_Memory : ERRORCODE_None;
}

/*
// Remove a record from a record array.
*/

void CRecordArrayRecord::RemoveRecord(DB_RECORD_NUMBER lRecord)
{
	DWORD dwIndex = FindRecord(lRecord);
	if (dwIndex != (DWORD)-1)
	{
		Delete(dwIndex);
	}
}

/*
// Find a record in the record array.
*/

DWORD CRecordArrayRecord::FindRecord(DB_RECORD_NUMBER lRecord)
{
	DWORD dwCount = ElementCount();
	for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
	{
		if (GetRecord(dwIndex) == lRecord)
		{
			return dwIndex;
		}
	}
	return (DWORD)-1;
}
