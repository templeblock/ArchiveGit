/*
// $Header: /PM8/REMIND32/ARRAYREC.H 1     3/03/99 6:26p Gbeddow $
//
// Definitions for PMG database record objects.
//
// $Log: /PM8/REMIND32/ARRAYREC.H $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   14 Aug 1997 15:18:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:40   Fred
// Initial revision.
// 
//    Rev 1.11   06 Jan 1997 15:45:28   Jay
// Optimized BlockOfIndex.
// 
//    Rev 1.10   01 Sep 1996 16:28:40   Fred
// New non-recursive FreeNextBlock()
// 
//    Rev 1.9   25 Jul 1996 19:38:08   Jay
//  
// 
//    Rev 1.8   28 Jun 1996 16:18:08   Jay
//  
// 
//    Rev 1.7   26 Jun 1996 14:21:34   Jay
//  
// 
//    Rev 1.6   19 Jun 1996 13:45:44   Jay
//  
// 
//    Rev 1.5   14 Jun 1996 17:05:36   Jay
//  
// 
//    Rev 1.4   04 Jun 1996 08:43:46   Jay
//  
// 
//    Rev 1.3   25 Apr 1996 10:37:42   Jay
// New final release notify code
// 
//    Rev 1.2   23 Apr 1996 08:07:46   Jay
// More new stuff
// 
//    Rev 1.1   11 Apr 1996 15:35:34   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:43:10   Jay
// Initial revision.
*/

#ifndef __ARRAYREC_H__
#define __ARRAYREC_H__

/*
// An array element record.
// This implements a virtual array element.
*/

class CArrayElementRecord : public DatabaseRecord
{
protected:
	struct
	{
		DWORD m_dwElementSize;
		DWORD m_dwElementCount;
		DB_RECORD_NUMBER m_NextBlock;
	} m_Record;

	CArrayElementRecord* m_pNextBlock;
	ST_DEV_POSITION m_lDataPosition;
	DWORD m_dwElementsAllocated;
	LPBYTE m_pData;

/*
// The constructor for this record.
// This is protected. Use the create method.
*/

	CArrayElementRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);
	virtual ~CArrayElementRecord();

/*
// Return the address of a particular element within the data block.
*/

	LPBYTE ElementAddress(DWORD dwIndex);

/*
// The size of the data in this block (elements*size).
*/

	DWORD DataSize(void);

/*
// Make sure data has been read in for this array record.
// This must be called before m_pData is accessed for the first time
// (after reading).
*/

	void PageInData(void);

/*
// Actually set the allocated count.
// This does not call PageInData (because PageInData calls this).
*/

	void DoSetAllocatedCount(DWORD dwCount);

public:
/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Data statistics.
*/

	DWORD ElementCount(void) const
		{ return m_Record.m_dwElementCount; }
	DWORD ElementSize(void) const
		{ return m_Record.m_dwElementSize; }
	void ElementSize(DWORD dwElementSize)
		{ m_Record.m_dwElementSize = dwElementSize; }

	DWORD SizeOfElements(DWORD dwCount) const
		{ return m_Record.m_dwElementSize*dwCount; }
/*
// Next block management.
*/

	void SetNextBlock(DB_RECORD_NUMBER NextBlock);
	DB_RECORD_NUMBER GetNextBlock(void)
		{ return m_Record.m_NextBlock; }
	void FreeNextBlock(void);			// Free the cached next record pointer.

/*
// Get a pointer to the next block.
*/

	CArrayElementRecord* AccessNextBlock(void);

/*
// Become the owner of the pointer to the next block.
*/

	CArrayElementRecord* OwnNextBlock(void);

/*
// Data management.
*/

	void FreeData(void);			// Free the block data.

/*
// Insert elements into the block.
*/

	void Insert(DWORD dwIndex, LPVOID pElement, DWORD dwInsertSize = 1);

/*
// Delete elements from the block.
*/

	void Delete(DWORD dwIndex, DWORD dwSize = 1);

/*
// Access an element in the block.
*/

	LPVOID GetAt(DWORD dwIndex);

/*
// Set/get the allocated (in-memory) size of the array.
// SetAllocatedCount() can throw an exception.
*/

	void SetAllocatedCount(DWORD dwCount);
	DWORD GetAllocatedCount(void) const
		{ return m_dwElementsAllocated; }

/*
// Final release notification.
// We need this to manage our subrecords.
*/

	virtual void FinalReleaseNotify(void);

/*
// The database is going away. All records are being freed.
// Release any sub-records we have locked.
*/

	virtual void ReleaseSubrecords(void);

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	ERRORCODE ReadData(StorageDevicePtr pDevice);

/*
// WriteData()
//
// Write the record.
*/

	ERRORCODE WriteData(StorageDevicePtr pDevice);

/*
// SizeofData()
//
// Return the size of the record.
*/

	ST_MAN_SIZE SizeofData(StorageDevicePtr pDevice);

/*
// RelocateData()
//
// This is a function called by the storage manager when our storage is
// ever relocated. This allows us to move any data we want saved.
*/

	ERRORCODE RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position);
};

/*
// An array record.
// This implements a virtual array.
*/

class CArrayRecord : public DatabaseRecord
{
protected:
/*
// Data.
*/

	struct
	{
		DWORD m_dwElementSize;
		DWORD m_dwElementCount;			// Total elements in the array.
		DWORD m_dwElementIncrement;
		DB_RECORD_NUMBER m_FirstBlock;
		DWORD m_dwMaxBlockElements;
	} m_Record;

	CArrayElementRecord* m_pFirstBlock;

	// Access optimization.
	CArrayElementRecord* m_pRecentBlock;
	CArrayElementRecord* m_pRecentBlockParent;
	DWORD m_dwRecentBlockStart;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);
	virtual ~CArrayRecord();

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

/*
// First block management.
*/

	void SetFirstBlock(DB_RECORD_NUMBER NextBlock);
	DB_RECORD_NUMBER GetFirstBlock(void)
		{ return m_Record.m_FirstBlock; }
	void FreeFirstBlock(void);			// Free the cached next record pointer.

/*
// Max block elements management.
*/

	void MaxBlockElements(DWORD dwMaxBlockElements)
		{ m_Record.m_dwMaxBlockElements = dwMaxBlockElements; }
	DWORD MaxBlockElements(void) const
		{ return m_Record.m_dwMaxBlockElements; }

/*
// Get a pointer to the first block.
*/

	CArrayElementRecord* AccessFirstBlock(void);

/*
// Return the block and block index of the passed element index.
*/

	void BlockOfIndex(DWORD dwIndex, CArrayElementRecord*& pBlock, DWORD& dwIndexInBlock, CArrayElementRecord** ppParentBlock = NULL);

/*
// Insert data into a block, making sure there's enough room.
*/

	void InsertIntoBlock(CArrayElementRecord* pBlock, DWORD dwIndex, LPVOID pElement, DWORD dwInsertCount = 1);

/*
// Split a block during insertion.
*/

	void SplitBlockForInsert(CArrayElementRecord*& pBlock, DWORD& dwIndexInBlock);

	// Reset the recent block variables.
	void ResetRecentBlock(void);

public:

/*
// Element statistics.
*/

	DWORD ElementCount(void) const
		{ return m_Record.m_dwElementCount; }
	DWORD ElementSize(void) const
		{ return m_Record.m_dwElementSize; }
	void ElementSize(DWORD dwElementSize)
		{ m_Record.m_dwElementSize = dwElementSize; }

	DWORD SizeOfElements(DWORD dwCount) const
		{ return m_Record.m_dwElementSize*dwCount; }

	DWORD ElementIncrement(void) const
		{ return m_Record.m_dwElementIncrement; }
	void ElementIncrement(DWORD dwElementIncrement)
		{ m_Record.m_dwElementIncrement = dwElementIncrement; }
/*
// Insert one or more elements into the array.
*/

	void Insert(DWORD dwIndex, LPVOID pElement, DWORD dwSize = 1);
	void Add(LPVOID pElement, DWORD dwSize = 1)
		{ Insert(ElementCount(), pElement, dwSize); }

/*
// Delete one or more elements from the array.
*/

	void Delete(DWORD dwIndex, DWORD dwSize = 1);

/*
// Access an element in the array.
*/

	LPVOID GetAt(DWORD dwIndex);
	LPVOID GetAt(DWORD dwIndex, DWORD* pElementsAvailable);

/*
// Access an element in the array with the intent of modifying it.
*/

	LPVOID ModifyAt(DWORD dwIndex);

/*
// Set elements in the array.
*/

	ERRORCODE SetAt(DWORD dwIndex, LPVOID pElement, DWORD dwSize = 1);

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual void destroy(void);

/*
// Final release notification.
// We need this to manage our subrecords.
*/

	virtual void FinalReleaseNotify(void);

/*
// The database is going away. All records are being freed.
// Release any sub-records we have locked.
*/

	virtual void ReleaseSubrecords(void);

/*
// A little routine to test this object.
*/

	void FreeAllBlockData(void);			// Use cautiously.
	void BlockStatistics(void);
	void SelfTest(void);

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	ERRORCODE ReadData(StorageDevicePtr pDevice);

/*
// WriteData()
//
// Write the record.
*/

	ERRORCODE WriteData(StorageDevicePtr pDevice);

/*
// SizeofData()
//
// Return the size of the record.
*/

	ST_MAN_SIZE SizeofData(StorageDevicePtr pDevice);

/*
// RelocateData()
//
// This is a function called by the storage manager when our storage is
// ever relocated. This allows us to move any data we want saved.
*/

	ERRORCODE RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position);
};

/*
// A generally useful array of record numbers.
*/

class CRecordArrayRecord : public CArrayRecord
{
public:
/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Record-specific access.
*/

	void AddRecord(DB_RECORD_NUMBER lRecord)
		{ Add((LPVOID)&lRecord); }
	void InsertRecord(DB_RECORD_NUMBER lRecord, DWORD dwIndex)
		{ Insert(dwIndex, (LPVOID)&lRecord); }
	DB_RECORD_NUMBER GetRecord(DWORD dwIndex)
		{ return *(DB_RECORD_NUMBER*)GetAt(dwIndex); }
	void RemoveRecord(DB_RECORD_NUMBER lRecord);
	DWORD FindRecord(DB_RECORD_NUMBER lRecord);

protected:
/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CRecordArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);
};

#endif								/* #ifndef __ARRAYREC_H__ */
