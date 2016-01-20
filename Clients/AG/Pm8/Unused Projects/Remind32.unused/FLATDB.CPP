/*
// $Workfile: FLATDB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/FLATDB.CPP $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:28   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:55:50   Fred
// Initial revision.
// 
//    Rev 1.26   13 Sep 1996 09:55:58   Fred
// Fixed bug with detach where position and index were not reset
// 
//    Rev 1.25   28 Aug 1996 21:24:40   Fred
// Larger block sizes to reduce array splits
// 
//    Rev 1.24   09 Aug 1996 16:51:40   Fred
// Uses halloc() instead of new __huge which doesn't work
// 
//    Rev 1.23   01 Aug 1996 12:41:04   Fred
// 
//    Rev 1.22   01 Aug 1996 12:28:06   Fred
// FreeMemory()
// 
//    Rev 1.21   18 Jul 1996 15:09:50   Fred
// FP_OFF -> LOWORD
// 
//    Rev 1.20   18 Jul 1996 07:09:12   Fred
// Fixed 16 bit memory leak
// 
//    Rev 1.19   18 Jul 1996 06:52:14   Fred
// WIN16 huge new.
// 
//    Rev 1.18   18 Jul 1996 06:45:04   Fred
// DEBUG_NEW
// 
//    Rev 1.17   17 Jul 1996 12:55:38   Jay
// Speed ups. New 'exclusive' algorithm
// 
//    Rev 1.15   12 Jul 1996 16:29:16   Fred
// Sharable enabled... i t s   v e r y  s l o w !
// 
//    Rev 1.14   06 Jul 1996 12:20:28   Fred
// Address book improvements
// 
//    Rev 1.13   03 Jul 1996 18:56:06   Fred
// GetIndex() can now return (DWORD)-1
// 
//    Rev 1.12   01 Jul 1996 07:32:00   Fred
// Receiver list
// 
//    Rev 1.11   24 Jun 1996 19:10:46   Fred
// Removed some m_dwIndex checks
// 
//    Rev 1.10   18 Jun 1996 15:20:52   Fred
// Field value list management and editing
// 
//    Rev 1.9   18 Jun 1996 08:28:36   Fred
// More adress book changes
// 
//    Rev 1.8   10 Jun 1996 08:15:06   Fred
// Added field and index user data; modified flags.
// 
//    Rev 1.7   05 Jun 1996 08:58:02   Fred
// RelocateData can be called!
// 
//    Rev 1.6   24 May 1996 16:15:56   Fred
// TRACEx
// 
//    Rev 1.5   06 May 1996 15:08:26   Fred
// New Names
// 
//    Rev 1.4   01 May 1996 17:09:36   Fred
// Database may be not sharable
// 
//    Rev 1.3   01 May 1996 14:34:52   Fred
// More flat file database classes
// 
//    Rev 1.2   25 Apr 1996 11:21:30   Fred
// CFlatFileDatabase works
// 
//    Rev 1.1   23 Apr 1996 16:21:46   Fred
// Compiles
// 
//    Rev 1.0   23 Apr 1996 11:09:28   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "error.h"
#include "memdev.h"
#include "file.h"
#include "flatdb.h"
#ifndef WIN32
#include <malloc.h>
#endif

//////////////////////////////////////////////////////////////////////////
// Record number record.

// Constructor.
CFlatFileRecordNumberRecord::CFlatFileRecordNumberRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
	: DatabaseRecord(number, type, owner, where)
{
	// Initialize data.
	memset(&m_Record, 0, sizeof(m_Record));
}

// Destructor.
CFlatFileRecordNumberRecord::~CFlatFileRecordNumberRecord()
{
}

// The creator for this type of record.
// Note that this is static so that we can register it with the database.
ERRORCODE CFlatFileRecordNumberRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	ERRORCODE error = ERRORCODE_None;

	CFlatFileRecordNumberRecord* pNewRecord = NULL;

	// Create the new object.
	TRY
	{
		pNewRecord = new CFlatFileRecordNumberRecord(number, type, owner, where);
	}
	END_TRY

	if (pNewRecord == NULL)
	{
		ASSERT(FALSE);
		error = ERRORCODE_Memory;
	}

	*record = pNewRecord;

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
void CFlatFileRecordNumberRecord::destroy(void)
{
}

// Assign method.
ERRORCODE CFlatFileRecordNumberRecord::assign(DatabaseRecordRef record)
{
	// This should never happen.
	ASSERT(FALSE);
	return ERRORCODE_IntError;
}

// ReadData()
// Read the record.
ERRORCODE CFlatFileRecordNumberRecord::ReadData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	// Read the main record.
	error = pDevice->read_record(&m_Record, sizeof(m_Record));

	ASSERT(error == ERRORCODE_None);
	return error;
}

// WriteData()
// Write the record.
ERRORCODE CFlatFileRecordNumberRecord::WriteData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	// Write the main record.
	error = pDevice->write_record(&m_Record, sizeof(m_Record));

	ASSERT(error == ERRORCODE_None);
	return error;
}

// SizeofData()
// Return the size of the record.
ST_MAN_SIZE CFlatFileRecordNumberRecord::SizeofData(StorageDevicePtr pDevice)
{
	return pDevice->size_record(sizeof(m_Record));
}

// RelocateData()
// This is a function called by the storage manager when our storage is
// ever relocated. This allows us to move any data we want saved.
ERRORCODE CFlatFileRecordNumberRecord::RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position)
{
	return ERRORCODE_None;
}

//////////////////////////////////////////////////////////////////////////
// Counter record

// Constructor.
CFlatFileCounterRecord::CFlatFileCounterRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
	: DatabaseRecord(number, type, owner, where)
{
	// Initialize data.
	memset(&m_Record, 0, sizeof(m_Record));
	m_Record.m_dwCount = 1;		// start at 1
}

// Destructor.
CFlatFileCounterRecord::~CFlatFileCounterRecord()
{
}

// The creator for this type of record.
// Note that this is static so that we can register it with the database.
ERRORCODE CFlatFileCounterRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	ERRORCODE error = ERRORCODE_None;

	CFlatFileCounterRecord* pNewRecord = NULL;

	// Create the new object.
	TRY
	{
		pNewRecord = new CFlatFileCounterRecord(number, type, owner, where);
	}
	END_TRY

	if (pNewRecord == NULL)
	{
		ASSERT(FALSE);
		error = ERRORCODE_Memory;
	}

	*record = pNewRecord;

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
void CFlatFileCounterRecord::destroy(void)
{
}

// Assign method.
ERRORCODE CFlatFileCounterRecord::assign(DatabaseRecordRef record)
{
	// This should never happen.
	ASSERT(FALSE);
	return ERRORCODE_IntError;
}

// ReadData()
// Read the record.
ERRORCODE CFlatFileCounterRecord::ReadData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	// Read the main record.
	error = pDevice->read_record(&m_Record, sizeof(m_Record));

	ASSERT(error == ERRORCODE_None);
	return error;
}

// WriteData()
// Write the record.
ERRORCODE CFlatFileCounterRecord::WriteData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	// Write the main record.
	error = pDevice->write_record(&m_Record, sizeof(m_Record));

	ASSERT(error == ERRORCODE_None);
	return error;
}

// SizeofData()
// Return the size of the record.
ST_MAN_SIZE CFlatFileCounterRecord::SizeofData(StorageDevicePtr pDevice)
{
	return pDevice->size_record(sizeof(m_Record));
}

// RelocateData()
// This is a function called by the storage manager when our storage is
// ever relocated. This allows us to move any data we want saved.
ERRORCODE CFlatFileCounterRecord::RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position)
{
	return ERRORCODE_None;
}

//////////////////////////////////////////////////////////////////////////
// Index record and supporting classes

// CFlatFileIndexRecord - an array of CIndexArrayElement structures which
// define a particular record ordering.

CFlatFileIndexRecord::CFlatFileIndexRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where) :
		CArrayRecord(number, type, owner, where)
{
	ElementSize(sizeof(CFlatFileIndexArrayElement));	// 4 bytes (record number)
	MaxBlockElements(1024);										// 4*1024 = 4096 bytes
	ElementIncrement(256);										// 4*256 = 1024 bytes
}

CFlatFileIndexRecord::~CFlatFileIndexRecord()
{
}

ERRORCODE CFlatFileIndexRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	CFlatFileIndexRecord* pNewRecord = NULL;

	TRY
	{
		// Create the new record.
		pNewRecord = new CFlatFileIndexRecord(number, type, owner, where);
	}
	END_TRY

	*record = pNewRecord;

	return (*record == NULL) ? ERRORCODE_Memory : ERRORCODE_None;
};

// Insert a record into an index.
ERRORCODE CFlatFileIndexRecord::InsertRecord(CFlatFileHeaderRecord* pHeader, CFlatFileHeaderRecord::CIndex* pIndex, DB_RECORD_NUMBER dwNumber, LPBYTE pData)
{
	ERRORCODE error = ERRORCODE_None;

#ifdef _DEBUG
	// If we're debugging, validate that the record number
	// is not already in the index.
	DWORD dwSequenceNumber;
	if (GetSequenceNumber(dwNumber, &dwSequenceNumber) != ERRORCODE_DoesNotExist)
	{
		// Record number already in index???
		ASSERT(FALSE);
		return ERRORCODE_IntError;
	}
#endif

	// Find the position to insert the record based on its data.
	DWORD dwPosition;
	BOOL fExact;
	error = FindRecord(pHeader, pIndex, pData, &dwPosition, &fExact);

	if (error == ERRORCODE_None)
	{
		// We found the place to insert the record. Create the array
		// element to insert.
		CFlatFileIndexArrayElement Element;
		Element.m_dwRecordNumber = dwNumber;
		
		// Insert the new element.
		TRY
		{
			Insert(dwPosition, &Element);
		}
		CATCH(CErrorcodeException, e)
		{
			ASSERT(FALSE);
			error = e->m_error;
		}
		AND_CATCH_ALL(e)
		{
			ASSERT(FALSE);
			error = ERRORCODE_IntError;
		}
		END_CATCH_ALL
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Find the array index of a record given its data.
ERRORCODE CFlatFileIndexRecord::FindRecord(CFlatFileHeaderRecord* pHeader, CFlatFileHeaderRecord::CIndex* pIndex, LPBYTE pData, DWORD* pdwPosition, BOOL* pfExact /*=NULL*/, DWORD dwFlags /*=0*/)
{
	ERRORCODE error = ERRORCODE_None;

	*pdwPosition = 0;

	if (pfExact != NULL)
	{
		*pfExact = FALSE;
	}

	// Allocate a buffer for reading data records.
	LPBYTE pBuffer = NULL;
	TRY
	{
		pBuffer = new BYTE[pHeader->GetRecordSize()];
	}
	END_TRY

	if (pBuffer == NULL)
	{
		ASSERT(FALSE);
		error = ERRORCODE_Memory;
	}
	else
	{
		// Perform a binary search for the data.
		DWORD dwMin = 0;
		DWORD dwMax = ElementCount();

		// Record first known index which is greater than new data (end of array).
		*pdwPosition = dwMax;

		while (dwMin < dwMax)
		{
			// Split the search space.
			DWORD dwTry = (dwMin+dwMax)/2;

			// Get the record number from the array.
			// GetAt() can throw an exception.
			CFlatFileIndexArrayElement* pIndexElement = NULL;
			TRY
			{
				pIndexElement = (CFlatFileIndexArrayElement*)GetAt(dwTry);
			}
			CATCH(CErrorcodeException, e)
			{
				ASSERT(FALSE);
				error = e->m_error;
				break;
			}
			END_CATCH

			// Check for some less likely errors.
			if ((pIndexElement == NULL) || (pIndexElement->m_dwRecordNumber == 0))
			{
				ASSERT(FALSE);
				error = ERRORCODE_IntError;
				break;
			}

			// Read in the data for the record.
			CFlatFileDataRecord* pRecord;
			if ((error = ((CFlatFileDatabase*)database)->GetDatabaseRecord((DatabaseRecord**)&pRecord, pIndexElement->m_dwRecordNumber, FRT_Data, TRUE)) != ERRORCODE_None)
			{
				// Could not read record.
				break;
			}

			// Get the data for the record.
			error = pRecord->GetData(&pBuffer);

			// Release the record. We have its data.
			pRecord->release_read_only();
			pRecord = NULL;

			// Make sure we got the data.
			if (error != ERRORCODE_None)
			{
				// Could not get data.
				break;
			}

			// Compare the data fields.
			int nResult;
			if ((error = CompareFields(pHeader, pIndex, pData, pBuffer, &nResult)) != ERRORCODE_None)
			{
				// Got an error.
				ASSERT(FALSE);
				break;
			}

			if ((dwFlags & FFF_GreaterThan) == 0)
			{
				// We're looking for the position where records at lesser positions
				// are less than the search value, and records at higher positions
				// are greater than or equal to the search value.

				// Check if new data less than or equal to current element.
				if (nResult <= 0)
				{
					// Data being inserted is less than or equal to buffer.

					// Set exact flag if we found an exact match.
					if (nResult == 0)
					{
						if (pfExact != NULL)
						{
							*pfExact = TRUE;
						}
					}

					// We know everything from dwTry forward is greater than or equal.
					dwMax = dwTry;

					// Record index of element we were less than or equal to.
					*pdwPosition = dwMax;
				}

				// Check if new data greater than current element.
				if (nResult > 0)
				{
					// Data being inserted is greater than buffer.

					// We know everything from dwTry back is lesser.
					dwMin = dwTry+1;
				}
			}
			else
			{
				// We're looking for the position where records at lesser positions
				// are less than or equal to the search value, and records at higher
				// positions are greater than the search value.

				// Check if new data less than the current element.
				if (nResult < 0)
				{
					// Data being inserted is less than buffer.

					// We know everything from dwTry forward is greater than.
					dwMax = dwTry;

					// Record index of element we were less than.
					*pdwPosition = dwMax;
				}

				// Check if new data greater than or equal to current element.
				if (nResult >= 0)
				{
					// Data being inserted is greater than or equal to buffer.

					// Set exact flag if we found an exact match.
					if (nResult == 0)
					{
						if (pfExact != NULL)
						{
							*pfExact = TRUE;
						}
					}

					// We know everything from dwTry back is lesser or equal.
					dwMin = dwTry+1;
				}
			}
		}

		delete [] pBuffer;
		pBuffer = NULL;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Compare the indexed fields of two records.
ERRORCODE CFlatFileIndexRecord::CompareFields(CFlatFileHeaderRecord* pHeader, CFlatFileHeaderRecord::CIndex* pIndex, LPBYTE pNewData, LPBYTE pCurrentData, int* pnResult)
{
	ERRORCODE error = ERRORCODE_None;

	*pnResult = 0;

	// The field are listed in order of their significance. We compare
	// them in this order until we can decide on the result of the
	// comparison.

	DWORD dwFieldCount = pIndex->GetFieldCount();

	for (DWORD dwFieldIndex = 0; dwFieldIndex < dwFieldCount; dwFieldIndex++)
	{
		// Compare the current field.
		DWORD dwField = pIndex->GetFieldNumber(dwFieldIndex);
		CFlatFileHeaderRecord::CField* pField = pHeader->GetField(dwField);

		if ((error = CompareField(pHeader, pIndex, pField, pNewData+pField->GetOffset(), pCurrentData+pField->GetOffset(), pnResult)) != ERRORCODE_None)
		{
			// Got an error.
			ASSERT(FALSE);
			break;
		}

		// Check if we have a result.
		if (*pnResult != 0)
		{
			break;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Compare a field of two records.
ERRORCODE CFlatFileIndexRecord::CompareField(CFlatFileHeaderRecord* pHeader, CFlatFileHeaderRecord::CIndex* pIndex, CFlatFileHeaderRecord::CField* pField, LPBYTE pNewFieldData, LPBYTE pCurrentFieldData, int* pnResult)
{
	ERRORCODE error = ERRORCODE_None;

	*pnResult = 0;

	// Branch on the type of field.
	switch (pField->GetType())
	{
		case FFT_String:
		{
			// String.
			if (pIndex->IsCaseSensitive())
			{
				*pnResult = strcmp((LPCSTR)pNewFieldData, (LPCSTR)pCurrentFieldData);
			}
			else
			{
				*pnResult = strcmpi((LPCSTR)pNewFieldData, (LPCSTR)pCurrentFieldData);
			}
			break;
		}

		case FFT_Blob:
		{
			// Blob? A Blob should not normally be part of an index.
			ASSERT(FALSE);
			error = ERRORCODE_IntError;
			break;
		}

		case FFT_Signed32:
		{
			// Signed 32-bit integers.
			long lNew = *((long*)pNewFieldData);
			long lCurrent = *((long*)pCurrentFieldData);
			if (lNew > lCurrent)
			{
				*pnResult = 1;
			}
			else if (lNew < lCurrent)
			{
				*pnResult = -1;
			}
			else
			{
				*pnResult = 0;
			}
			break;
		}

		case FFT_Counter32:
		case FFT_Unsigned32:
		{
			// Unsigned 32-bit integers.
			DWORD dwNew = *((DWORD*)pNewFieldData);
			DWORD dwCurrent = *((DWORD*)pCurrentFieldData);
			if (dwNew > dwCurrent)
			{
				*pnResult = 1;
			}
			else if (dwNew < dwCurrent)
			{
				*pnResult = -1;
			}
			else
			{
				*pnResult = 0;
			}
			break;
		}

		default:
		{
			// Unknown field type.
			ASSERT(FALSE);
			error = ERRORCODE_IntError;
			break;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Delete a record from an index.
ERRORCODE CFlatFileIndexRecord::DeleteRecord(DB_RECORD_NUMBER dwNumber)
{
	ERRORCODE error = ERRORCODE_None;

	// Get the index of the index array element that contains the record number to delete.
	DWORD dwIndex;
	if ((error = GetSequenceNumber(dwNumber, &dwIndex)) == ERRORCODE_None)
	{
		// Delete the index array element.
		TRY
		{
			Delete(dwIndex);
		}
		CATCH(CErrorcodeException, e)
		{
			error = e->m_error;
		}
		AND_CATCH_ALL(e)
		{
			error = ERRORCODE_IntError;
		}
		END_CATCH_ALL
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Convert a record number to a sequence number.
ERRORCODE CFlatFileIndexRecord::GetSequenceNumber(DB_RECORD_NUMBER dwNumber, DWORD* pdwSequenceNumber)
{
	ERRORCODE error = ERRORCODE_DoesNotExist;

	*pdwSequenceNumber = 0;

	// Search through the array for the specified record number.
	DWORD dwIndex = 0;
	BOOL fFound = FALSE;

	while (!fFound && (dwIndex < ElementCount()))
	{
		DWORD dwAvailable = 0;
		CFlatFileIndexArrayElement* pIndexElement = NULL;

		TRY
		{
			pIndexElement = (CFlatFileIndexArrayElement*)GetAt(dwIndex, &dwAvailable);
		}
		CATCH(CErrorcodeException, e)
		{
			ASSERT(FALSE);
			error = e->m_error;
			break;
		}
		END_CATCH

		// Check for some less likely errors.
		if ((pIndexElement == NULL) || (dwAvailable == 0) || (pIndexElement->m_dwRecordNumber == 0))
		{
			ASSERT(FALSE);
			error = ERRORCODE_IntError;
			break;
		}

		// Tool through the elements looking for the one we want.
		while (dwAvailable-- > 0)
		{
			// Check if we found the number we are looking for.
			if (pIndexElement->m_dwRecordNumber == dwNumber)
			{
				// Got it!
				fFound = TRUE;
				error = ERRORCODE_None;
				*pdwSequenceNumber = dwIndex;
				break;
			}

			// Not this one, try the next.
			pIndexElement++;
			dwIndex++;
		}
	}

//	ASSERT(error == ERRORCODE_None);
	return error;
}

//////////////////////////////////////////////////////////////////////////
// Data record
//
// A data record is place to store a simple block of data that is created
// once and then only read. Its structure is a DWORD size followed by the
// data itself. The initial block data is read from a StorageDevice (or descendant)
// passed in the creation data.
//
// This record is very close to the embedded file record except it does not
// have the notion of a file name.

// Constructor.
CFlatFileDataRecord::CFlatFileDataRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
	: DatabaseRecord(number, type, owner, where)
{
	// Initialize data.
	memset(&m_Record, 0, sizeof(m_Record));
	m_Position = POSITION_UNKNOWN;
}

// Destructor.
CFlatFileDataRecord::~CFlatFileDataRecord()
{
}

// The creator for this type of record.
// Note that this is static so that we can register it with the database.
ERRORCODE CFlatFileDataRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	ERRORCODE error = ERRORCODE_None;

	CFlatFileDataRecord* pNewRecord = NULL;

	// Create the new object.
	TRY
	{
		pNewRecord = new CFlatFileDataRecord(number, type, owner, where);
	}
	END_TRY

	if (pNewRecord == NULL)
	{
		ASSERT(FALSE);
		error = ERRORCODE_Memory;
	}

	else
	{
		// A description of the data for this object has been passed in
		// the creation data. Write it to the record at this time. The
		// WriteData() method will be called when the record is released
		// to write out the fixed part of the record (m_Record).

		if (creation_data != NULL)
		{
			if ((error = pNewRecord->WriteCreationData((CCreationData*)creation_data)) != ERRORCODE_None)
			{
				ASSERT(FALSE);
				delete pNewRecord;
				pNewRecord = NULL;
			}
		}
	}

	*record = pNewRecord;

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
void CFlatFileDataRecord::destroy(void)
{
}

// Assign method.
ERRORCODE CFlatFileDataRecord::assign(DatabaseRecordRef record)
{
	// This should never happen.
	ASSERT(FALSE);
	return ERRORCODE_IntError;
}

// ReadData()
// Read the record.
ERRORCODE CFlatFileDataRecord::ReadData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	// Read the main record.
	if ((error = pDevice->read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		// We now know the length, remember where the data is. The actual
		// data portion is not read until the user explicitly requests it.
		error = pDevice->tell(&m_Position);
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// WriteData()
// Write the record.
ERRORCODE CFlatFileDataRecord::WriteData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	// Write the main record.
	if ((error = pDevice->write_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		// Record where the main data starts. This type of record is never
		// changed, it is only written once at the time it is created. Thus,
		// we only need to write out the fixed part of the record (m_Record).
		// The data portion that is variable length is written when the create()
		// method calls WriteCreationData().
		error = pDevice->tell(&m_Position);
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// WriteCreationData()
// Write the variable length portion of the data. This method is
// called when the the record is created. The data to write is
// specified by the creation data passed to create().
ERRORCODE CFlatFileDataRecord::WriteCreationData(CCreationData* pCreationData)
{
	ERRORCODE error = ERRORCODE_None;

	// Initialize the type of record from the creation data.
	m_Record.m_dwType = pCreationData->GetType();

	// Initialize the size of the variable length data associated
	// with this record.
	m_Record.m_dwSize = 0;

	if (pCreationData != NULL)
	{
		// Get the storage device used for the database.
		StorageDevice* pDatabaseDevice = database->get_storage_device();
		if (pDatabaseDevice != NULL)
		{
			// Compute the length of the data we will be writing. We need to
			// allocate this data within the database before write write the data.

			// Length of storage device data.
			DWORD dwStorageDeviceSize = 0;
			StorageDevice* pCreationDataDevice = pCreationData->GetStorageDevice();

			if (pCreationDataDevice != NULL)
			{
				error = pCreationDataDevice->length((ST_DEV_POSITION*)(&dwStorageDeviceSize));
			}

			if (error == ERRORCODE_None)
			{
				// Set the size of the variable data. This is used by SizeofData()
				// which is called by Allocate() to figure out the size to allocate.
				m_Record.m_dwSize = dwStorageDeviceSize;

				// Allocate the space in the database.
				if ((error = Allocate(StorageElement::Type(), StorageElement::Id())) == ERRORCODE_None)
				{
					// Remember where we are.
					*where = m_position;

					// Compute the starting and ending positions of where we
					// are going to write the variable length data.
					m_Position = *where+(SizeofData(pDatabaseDevice)-m_Record.m_dwSize);
					ST_DEV_POSITION Start = m_Position;
					ST_DEV_POSITION End = Start+m_Record.m_dwSize;

					// Restrict ourselves to this range.
					pDatabaseDevice->io_limits(Start, End);

					// Create a device on top of the database one so
					// we can write to it virtually.

					StorageFile DestinationDevice(pDatabaseDevice);
					DestinationDevice.set_subfile(Start, End);

					error = copy_file(pCreationDataDevice, &DestinationDevice);

					if (error != ERRORCODE_None)
					{
						ASSERT(FALSE);
//						TRACE("Got error %d while copying %ld bytes of data [%ld, %ld]\r\n", error, End - Start, Start, End);
					}

					// Restore unrestricted access.

					pDatabaseDevice->io_limits();
				}
			}
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// SizeofData()
// Return the size of the record.
ST_MAN_SIZE CFlatFileDataRecord::SizeofData(StorageDevicePtr pDevice)
{
	return pDevice->size_record(sizeof(m_Record))+GetSize();
}

// RelocateData()
// This is a function called by the storage manager when our storage is
// ever relocated. This allows us to move any data we want saved.
ERRORCODE CFlatFileDataRecord::RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position)
{
	return ERRORCODE_None;
}

// Read the variable size data for this record into memory. If the
// memory pointer passed in is NULL, a block will be allocated for
// the data. The caller must free the allocated block using the
// static function CFlatFileDatabase::FreeMemory(). The second parameter,
// pdwSize, points to a DWORD which, if not NULL, will be filled in
// with the data size (pdwSize of ONLY and output parameter.)
ERRORCODE CFlatFileDataRecord::GetData(LPBYTE* ppData, DWORD* pdwSize /*=NULL*/) const
{
	ERRORCODE error = ERRORCODE_None;

	// Get the storage device associated with the database. We will be
	// reading from this device.
	StorageDevice* pDevice = database->get_storage_device();
	if (pDevice == NULL)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	else
	{
		ASSERT(ppData != NULL);

		// Seek to the data in anticipation of reading it.
		if ((error = pDevice->seek(m_Position, ST_DEV_SEEK_SET)) == ERRORCODE_None)
		{
			// If a memory block has not been specified, allocate one.
			if (*ppData == NULL)
			{
				// Allocate the memory block using CFlatFileDatabase::AllocateMemory().
				LPBYTE pMemory = NULL;
				if ((error = CFlatFileDatabase::AllocateMemory((FLATFILE_DATABASE_MEMORY_PTR*)&pMemory, m_Record.m_dwSize)) == ERRORCODE_None)
				{
					// We have the memory, read the data.
					if ((error = pDevice->huge_read(pMemory, m_Record.m_dwSize, NULL)) == ERRORCODE_None)
					{
						// Save a pointer to the data.
						*ppData = pMemory;
					}
					else
					{
						// Got an error, free the memory.
						ASSERT(FALSE);
						CFlatFileDatabase::FreeMemory(pMemory);
						pMemory = NULL;
					}
				}
			}

			else
			{
				// A buffer was passed in. Read the data into the buffer (it must
				// be large enough to hold the entire data block.)
				error = pDevice->huge_read(*ppData, m_Record.m_dwSize, NULL);
			}
		}
	}

	// Pass back the length if the caller requested.
	if (pdwSize != NULL)
	{
		if (error == ERRORCODE_None)
		{
			*pdwSize = m_Record.m_dwSize;
		}
		else
		{
			*pdwSize = 0;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// The file completion notify for a read-only data record.
void CFlatFileDataRecord::FlatFileDataRecordReadOnlyCompletion(StorageFilePtr file, LPVOID data)
{
	((CFlatFileDataRecord*)data)->release_read_only();
}

// Initialize a StorageFile object to access the variable size data for this record.
// Only read only access is supported.
ERRORCODE CFlatFileDataRecord::GetDataAsFile(StorageFile* pFile) const
{
	ERRORCODE error = ERRORCODE_None;

	// Initialize the file to be based on the database device.
	if ((error = pFile->set_based_device(database->get_storage_device())) == ERRORCODE_None)
	{
		// Total success. 'error' is set correctly.

		// Set up the subfile to correspond to the portion of the
		// database containing the variable sized record data.
		ST_DEV_POSITION Start = GetPosition();
		ST_DEV_POSITION End = Start + GetSize();

		pFile->set_subfile(Start, End);
		pFile->set_completion_notify(FlatFileDataRecordReadOnlyCompletion, (LPVOID)this);

		// Make sure we can actually access the file.
		if ((error = pFile->seek(0, ST_DEV_SEEK_SET)) != ERRORCODE_None)
		{
			// Undo what we just did.
			ASSERT(FALSE);
			pFile->reset();
		}
	}
	else
	{
		// Caller assumes record release is always handled. So we handle it.
		release_read_only();
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// CFlatFileDataRecord::CCreationData
// This class encapsulates the creation data for the data record.

CFlatFileDataRecord::CCreationData::CCreationData(StorageDevice* pStorageDevice, DWORD dwType /*=(DWORD)Normal*/)
{
	ASSERT(pStorageDevice != NULL);
	m_pStorageDevice = pStorageDevice;
	m_dwType = dwType;
}

//////////////////////////////////////////////////////////////////////////
// Header record

// Constructor.
CFlatFileHeaderRecord::CFlatFileHeaderRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
	: DatabaseRecord(number, type, owner, where)
{
	memset(&m_Record, 0, sizeof(m_Record));
	m_Position = POSITION_UNKNOWN;
	m_pData = NULL;
}

// Destructor.
CFlatFileHeaderRecord::~CFlatFileHeaderRecord()
{
	FreeData();
}

// Assign method.
ERRORCODE CFlatFileHeaderRecord::assign(DatabaseRecordRef record)
{
	// This should never happen.
	ASSERT(FALSE);
	return ERRORCODE_IntError;
}

// The creator for this type of record.
// Note that this is static so that we can register it with the database.
ERRORCODE CFlatFileHeaderRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	ERRORCODE error = ERRORCODE_None;

	CFlatFileHeaderRecord* pNewRecord = NULL;

	// Make sure the header is only created using the proper record number.
	ASSERT(number == FLATDB_HEADER_RECORD);
	if (number != FLATDB_HEADER_RECORD)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	else
	{
		// Create the new object.
		TRY
		{
			pNewRecord = new CFlatFileHeaderRecord(number, type, owner, where);
		}
		END_TRY

		if (pNewRecord == NULL)
		{
			ASSERT(FALSE);
			error = ERRORCODE_Memory;
		}

		else
		{
			// Initialize the fixed portion of the record. At the moment, the caller
			// has no way of overriding these values. They are fixed by the object.

			pNewRecord->m_Record.m_dwVersion = 0x00010000L;

			// A description of the data for this object has been passed in
			// the creation data. Write it to the record at this time. The
			// WriteData() method will be called when the record is released
			// to write out the fixed part of the record (m_Record).

			if (creation_data != NULL)
			{
				if ((error = pNewRecord->WriteCreationData((CCreationData*)creation_data)) != ERRORCODE_None)
				{
					ASSERT(FALSE);
					delete pNewRecord;
					pNewRecord = NULL;
				}
			}
		}
	}

	*record = pNewRecord;

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
void CFlatFileHeaderRecord::destroy(void)
{
}

// ReadData()
// Read the record.
ERRORCODE CFlatFileHeaderRecord::ReadData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	// Read the main record.
	if ((error = pDevice->read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		// We now know the length, remember where the data is.
		if ((error = pDevice->tell(&m_Position)) == ERRORCODE_None)
		{
			// Allocate the data to hold the variable length portion of
			// the header record.
			FreeData();
			if ((error = CFlatFileDatabase::AllocateMemory((FLATFILE_DATABASE_MEMORY_PTR*)&m_pData, GetSize())) == ERRORCODE_None)
			{
				// Read in the variable length portion of the header record.
				if ((error = pDevice->huge_read(m_pData, GetSize())) != ERRORCODE_None)
				{
					// Got an error, release the memory we allocated.
					ASSERT(FALSE);
					FreeData();
				}
			}
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// WriteData()
// Write the record.
ERRORCODE CFlatFileHeaderRecord::WriteData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	// Write the main record.
	if ((error = pDevice->write_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		// Record where the main data starts. This type of record is never
		// changed, it is only written once at the time it is created. Thus,
		// we only need to write out the fixed part of the record (m_Record).
		// The data portion that is variable length is written when the create()
		// method calls WriteCreationData().
		error = pDevice->tell(&m_Position);
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// WriteCreationData()
// Write the variable length portion of the data. This method is
// called when the the record is created. The data to write is
// specified by the creation data passed to create().
ERRORCODE CFlatFileHeaderRecord::WriteCreationData(CCreationData* pCreationData)
{
	ERRORCODE error = ERRORCODE_None;

	// Initialize the size of the variable length data associated
	// with this record.
	m_Record.m_dwSize = 0;

	// Free any existing data.
	FreeData();

	// Get the storage device used for the database.
	StorageDevice* pDatabaseDevice = database->get_storage_device();
	if (pDatabaseDevice != NULL)
	{
		if (pCreationData != NULL)
		{
			// We want to take the field and index descriptions the user passed
			// in the creation data and construct the header block which will be
			// written as our variable length data. Once the header block is
			// created, it's never changed.

			// The user has given us:
			//
			//		Fields (0, 1, ...)
			//
			//			pCreationData->FirstField() points to first field description:
			//
			//				LPCSTR pointer to field name
			//				DWORD  field type (FLATFILE_FIELD_TYPE)
			//				DWORD  byte size of field
			//
			//				...more fields...
			//
			//				NULL field name pointer
			//
			//		Indexes (0, 1, ...)
			//
			//			pCreationData->FirstIndex() pointer to first index description:
			//
			//				LPCSTR pointer to index name
			//				DWORD  index flags (FLATFILE_INDEX_FLAGS)
			//				LPCSTR pointer to field names separated by '|'
			//
			//				...more indicies...
			//
			//				NULL index name pointer
			//
			//
			// We want to take was the user has given and create the variable
			// length portion of the header block:
			//
			//		DWORD offset of field 0 from start of variable length block
			//		DWORD offset of field 1 from start of variable length block
			//		...more field offsets...
			//
			//		Field 0
			//
			//			DWORD  field type (FLATFILE_FIELD_TYPE)
			//			DWORD  byte size of field
			//			DWORD  byte offset of field data within record
			//			char[] null-terminated field name
			//
			//		Field 1
			//
			//		...more fields...
			//
			//		DWORD offset of index 0 from start of variable length block
			//		DWORD offset of index 1 from start of variable length block
			//		...more index offsets...
			//
			//		Index 0
			//
			//			DWORD  index block number (0 if does not yet exist)
			//			DWORD  index flags (FLATFILE_INDEX_FLAGS)
			//			DWORD  number of fields
			//			DWORD  number of field 0 (in field offset array)
			//			DWORD  number of field 1
			//			...more field numbers...
			//			char[] null-terminated index name
			//
			//		Index 1
			//
			//		... more indicies...
			//
			// The number of fields and indicies along with the offsets of the
			// field and index information in the variable length data are
			// recorded in m_Record.
			//
			// The size of the variable length data will be recorded in
			// the member variable m_Record.m_dwSize.

			// First, compute the size of the block we will need (this is the same
			// value we need to save in m_Record.m_dwSize.)

			// Size fields.
			DWORD dwFieldInformationSize;
			DWORD dwNumberOfFields;

			if ((error = SizeFieldInformation(pCreationData, &dwFieldInformationSize, &dwNumberOfFields)) == ERRORCODE_None)
			{
				m_Record.m_dwSize += dwFieldInformationSize;

				// Size indicies.
				DWORD dwIndexInformationSize;
				DWORD dwNumberOfIndicies;

				if ((error = SizeIndexInformation(pCreationData, &dwIndexInformationSize, &dwNumberOfIndicies)) == ERRORCODE_None)
				{
					m_Record.m_dwSize += dwIndexInformationSize;

					// We now know the size of the variable length data. Allocate
					// the memory that will hold the variable length portion of
					// the header block data.
					if ((error = CFlatFileDatabase::AllocateMemory((FLATFILE_DATABASE_MEMORY_PTR*)&m_pData, GetSize())) == ERRORCODE_None)
					{
						// Allocate the space in the database for the variable length data.
						if ((error = Allocate(StorageElement::Type(), StorageElement::Id())) == ERRORCODE_None)
						{
							// Remember where we are.
							*where = m_position;

							// Save the position of the variable length data block.
							m_Position = *where+(SizeofData(pDatabaseDevice)-m_Record.m_dwSize);

							// Build the field information.
							if ((error = BuildFieldInformation(pCreationData, m_pData, dwNumberOfFields)) == ERRORCODE_None)
							{
								// Build the index information.
								if ((error = BuildIndexInformation(pCreationData, m_pData+dwFieldInformationSize, dwNumberOfIndicies)) == ERRORCODE_None)
								{
									// The variable length data block is now properly
									// initialized. Write the data block to the database.
									pDatabaseDevice->seek(m_Position, ST_DEV_SEEK_SET);
									error = pDatabaseDevice->huge_write(m_pData, m_Record.m_dwSize);
								}
							}
						}
					}
				}
			}
		}
	}

	if (error != ERRORCODE_None)
	{
		FreeData();
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

ERRORCODE CFlatFileHeaderRecord::SizeFieldInformation(CCreationData* pCreationData, DWORD* pdwSize, DWORD* pdwCount)
{
	ERRORCODE error = ERRORCODE_None;

	*pdwSize = 0;
	*pdwCount = 0;

	// Size fields.
	*pdwSize += sizeof(DWORD);		// Number of fields

	CFlatFileFieldDescription* pFieldDescription = pCreationData->GetFirstField();
	if (pFieldDescription != NULL)
	{
		while (pFieldDescription->m_pszName != NULL)
		{
			// Size current field.
			*pdwSize += sizeof(DWORD);		// Field offset
			*pdwSize += sizeof(CField);	// Field descriptor

			// Field name.
			*pdwSize += strlen(pFieldDescription->m_pszName)+1;

			// Move to next field.
			(*pdwCount)++;
			pFieldDescription++;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

ERRORCODE CFlatFileHeaderRecord::SizeIndexInformation(CCreationData* pCreationData, DWORD* pdwSize, DWORD* pdwCount)
{
	ERRORCODE error = ERRORCODE_None;

	*pdwSize = 0;
	*pdwCount = 0;

	// Size indicies.
	*pdwSize += sizeof(DWORD);		// Number of indicies

	CFlatFileIndexDescription* pIndexDescription = pCreationData->GetFirstIndex();
	if (pIndexDescription != NULL)
	{
		while (pIndexDescription->m_pszName != NULL)
		{
			// Size current index.
			*pdwSize += sizeof(DWORD);		// Index offset
			*pdwSize += sizeof(CIndex);	// Index descriptor

			// Size fields.
			LPCSTR pszFieldNames = pIndexDescription->m_pszField;
			if (pszFieldNames == NULL)
			{
				// Index has no field name string!
				ASSERT(FALSE);
				error = ERRORCODE_BadParameter;
				break;
			}
			else
			{
				// Parse the field names.
				while (pszFieldNames != NULL)
				{
					// Get the next field name from the field name list.
					LPCSTR pszFieldName;
					DWORD dwLength;
					error = NextFieldName(&pszFieldNames, &pszFieldName, &dwLength);
					if (error != ERRORCODE_None)
					{
						ASSERT(FALSE);
						break;
					}

					// Filter out empty field names.
					if ((pszFieldName != NULL) && (dwLength > 0))
					{
						// We have a field, account for number in the index.
						*pdwSize += sizeof(DWORD);	// Field number
					}
				}
			}

			// Index name.
			*pdwSize += strlen(pIndexDescription->m_pszName)+1;

			// Move to next index.
			(*pdwCount)++;
			pIndexDescription++;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

ERRORCODE CFlatFileHeaderRecord::BuildFieldInformation(CCreationData* pCreationData, LPBYTE pData, DWORD dwCount)
{
	ERRORCODE error = ERRORCODE_None;

	// Save field count and offset to field information in m_Record.
	m_Record.m_dwFieldCount = dwCount;
	m_Record.m_dwFieldOffset = pData-m_pData;

	// Build the field data.
	DWORD* pdwFieldOffset = (DWORD*)pData;
	LPBYTE pFieldData = (LPBYTE)(pdwFieldOffset+dwCount);

	DWORD dwRecordOffset = 0;
	CFlatFileFieldDescription* pFieldDescription = pCreationData->GetFirstField();
	if (pFieldDescription != NULL)
	{
		while (pFieldDescription->m_pszName != NULL)
		{
			// Save the offset to the field.
			*(pdwFieldOffset++) = pFieldData-m_pData;

			// Build the current field.
			CField* pField = (CField*)pFieldData;
			pField->m_dwType = pFieldDescription->m_dwType;
			pField->m_dwUserData = pFieldDescription->m_dwUserData;
			pField->m_dwSize = pFieldDescription->m_dwSize;
			pField->m_dwOffset = dwRecordOffset;
			strcpy(pField->m_szName, pFieldDescription->m_pszName);

			// Accumulate field size into total record size.
			m_Record.m_dwRecordSize += pField->m_dwSize;

			// Move to next field.
			pFieldData += sizeof(CField)+strlen(pField->m_szName)+1;
			dwRecordOffset += pField->m_dwSize;
			pFieldDescription++;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

ERRORCODE CFlatFileHeaderRecord::BuildIndexInformation(CCreationData* pCreationData, LPBYTE pData, DWORD dwCount)
{
	ERRORCODE error = ERRORCODE_None;

	// Save index count and offset to index information in m_Record.
	m_Record.m_dwIndexCount = dwCount;
	m_Record.m_dwIndexOffset = pData-m_pData;

	// Build the index data.
	DWORD* pdwIndexOffset = (DWORD*)pData;
	LPBYTE pIndexData = (LPBYTE)(pdwIndexOffset+dwCount);

	CFlatFileIndexDescription* pIndexDescription = pCreationData->GetFirstIndex();
	if (pIndexDescription != NULL)
	{
		while (pIndexDescription->m_pszName != NULL)
		{
			// Save the offset to the index.
			*(pdwIndexOffset++) = pIndexData-m_pData;

			// Allocate a record for the index.
			CFlatFileIndexRecord* pIndexRecord = (CFlatFileIndexRecord*)(((CFlatFileDatabase*)database)->CreateIndexRecord());
			if (pIndexRecord == NULL)
			{
				ASSERT(FALSE);
				error = database->last_creation_error();
				break;
			}

			// Build the current index.
			CIndex* pIndex = (CIndex*)pIndexData;
			pIndex->m_dwRecordNumber = pIndexRecord->Id();
			pIndexRecord->release();

			pIndex->m_dwFlags = pIndexDescription->m_dwFlags;
			pIndex->m_dwUserData = pIndexDescription->m_dwUserData;

			// Build field number list.
			DWORD dwIndexFields = 0;
			DWORD* pdwFieldNumber = &(pIndex->m_dwField[0]);
			LPCSTR pszFieldNames = pIndexDescription->m_pszField;
			if (pszFieldNames == NULL)
			{
				// Index has no field name string!
				ASSERT(FALSE);
				error = ERRORCODE_BadParameter;
				break;
			}
			else
			{
				// Parse the field names.
				while (pszFieldNames != NULL)
				{
					// Get the next field name from the field name list.
					LPCSTR pszFieldName;
					DWORD dwLength;
					error = NextFieldName(&pszFieldNames, &pszFieldName, &dwLength);
					if (error != ERRORCODE_None)
					{
						ASSERT(FALSE);
						break;
					}

					// Filter out empty field names.
					if ((pszFieldName != NULL) && (dwLength > 0))
					{
						// We have a field, build its reference number into the index.
						DWORD dwFieldNumber = pCreationData->GetFieldIndex(pszFieldName, dwLength);
						if (dwFieldNumber == (DWORD)-1)
						{
							// Index references unknown field.
							ASSERT(FALSE);
							error = ERRORCODE_BadParameter;
							break;
						}

						// Save field number.
						*pdwFieldNumber = dwFieldNumber;

						// Move to next field.
						dwIndexFields++;
						pdwFieldNumber++;
					}
				}
			}

			if (error != ERRORCODE_None)
			{
				// Got an error, we're aborting.
				break;
			}

			// Save number of fields.
			pIndex->m_dwFields = dwIndexFields;

			// Save name of index after last field number.
			strcpy((LPSTR)pdwFieldNumber, pIndexDescription->m_pszName);

			// Move to next index.
			pIndexData += sizeof(CIndex)+(pIndex->m_dwFields*sizeof(DWORD))+strlen((LPCSTR)pdwFieldNumber)+1;
			pIndexDescription++;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Extract the next field name from a field name list string. The format of this
// string is the field names separated by '|' characters. Leading and trailing
// spaces are removed.
ERRORCODE CFlatFileHeaderRecord::NextFieldName(LPCSTR* ppszFieldNames, LPCSTR* ppszFieldName, DWORD* pdwLength)
{
	ERRORCODE error = ERRORCODE_None;

	// First, find the bounds of everything up to a field terminator.
	*ppszFieldName = *ppszFieldNames;
	*pdwLength = 0;

	if (*ppszFieldNames != NULL)
	{
		while ((**ppszFieldNames != '\0') && (**ppszFieldNames != '|'))
		{
			(*pdwLength)++;
			(*ppszFieldNames)++;
		}

		// Advance the names pointer if we did not find a null.
		if (**ppszFieldNames == '|')
		{
			(*ppszFieldNames)++;
		}

		// ppszFieldNames is now set correctly for finding the next name.

		// Trim off any trailing whitespace (" \t\n\r").
		while ((*pdwLength > 0) && (strchr(" \t\n\r", (*ppszFieldName)[(*pdwLength)-1]) != NULL))
		{
			(*pdwLength)--;
		}

		// Trim off any leading whitespace (" \t\n\r").
		while ((*pdwLength > 0) && (strchr(" \t\n\r", (*ppszFieldName)[(*pdwLength)-1]) != NULL))
		{
			(*ppszFieldName)++;
			(*pdwLength)--;
		}

		// Don't return a character pointer if length is zero.
		if (*pdwLength == 0)
		{
			*ppszFieldName = NULL;
		}

		// If we're pointing at a NULL, set *ppszFieldNames to NULL so
		// the caller will know to stop.
		if (**ppszFieldNames == '\0')
		{
			*ppszFieldNames = NULL;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Compute the buffer size needed to hold the current field descriptions.
ERRORCODE CFlatFileHeaderRecord::SizeFieldDescriptions(DWORD* pdwSize)
{
	ERRORCODE error = ERRORCODE_None;

	*pdwSize = 0;

	// Size all the known fields.
	for (DWORD dwField = 0; dwField < GetFieldCount(); dwField++)
	{
		// Size the current field.
		CField* pField = GetField(dwField);
		ASSERT(pField != NULL);
		if (pField != NULL)
		{
			*pdwSize += sizeof(CFlatFileFieldDescription);	// size of entry
			*pdwSize += strlen(pField->GetName())+1;				// size of name
		}
	}

	// Size the empty entry which terminates the field description list.
	*pdwSize += sizeof(CFlatFileFieldDescription);

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Compute the buffer size needed to hold the current index descriptions.
ERRORCODE CFlatFileHeaderRecord::SizeIndexDescriptions(DWORD* pdwSize)
{
	ERRORCODE error = ERRORCODE_None;

	*pdwSize = 0;

	// Size all the known indicies.
	for (DWORD dwIndex = 0; dwIndex < GetIndexCount(); dwIndex++)
	{
		// Size the current index.
		CIndex* pIndex = GetIndex(dwIndex);
		ASSERT(pIndex != NULL);
		if (pIndex != NULL)
		{
			*pdwSize += sizeof(CFlatFileIndexDescription);	// size of entry

			// Size the names.
			for (DWORD dwFieldNumberIndex = 0; dwFieldNumberIndex < pIndex->GetFieldCount(); dwFieldNumberIndex++)
			{
				// Size the current name (with separator).
				*pdwSize += strlen(GetField(pIndex->GetFieldNumber(dwFieldNumberIndex))->GetName())+1;
			}

			*pdwSize += strlen(pIndex->GetName())+1;					// size of index name
		}
	}

	// Size the empty entry which terminates the index description list.
	*pdwSize += sizeof(CFlatFileIndexDescription);

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get a list of fields in "CFlatFileFieldDescription" format.
ERRORCODE CFlatFileHeaderRecord::BuildFieldDescriptions(CFlatFileFieldDescription* pDescription, DWORD* pdwSize /*=NULL*/)
{
	ERRORCODE error = ERRORCODE_None;

	ASSERT((pDescription != NULL) || (pdwSize != NULL));

	// Get the required size.
	DWORD dwRequiredSize;
	if ((error = SizeFieldDescriptions(&dwRequiredSize)) == ERRORCODE_None)
	{
		// If no description buffer was passed in, then simply return the required size.
		if (pDescription == NULL)
		{
			*pdwSize = dwRequiredSize;
		}
		else
		{
			// We have a buffer. If the caller has specified a size, make sure it is sufficient.
			if (*pdwSize != NULL)
			{
				 if (*pdwSize < dwRequiredSize)
				 {
					// Not enough memory.
					error = ERRORCODE_Memory;
				 }

				 // Always return the required size.
				 *pdwSize = dwRequiredSize;
			}

			if (error == ERRORCODE_None)
			{
				// Everthing's in place, build the field descriptions.
				LPSTR pszStringData = (LPSTR)(pDescription+GetFieldCount()+1);

				// Build all fields.
				for (DWORD dwField = 0; dwField < GetFieldCount(); dwField++)
				{
					// Build the current field.
					CField* pField = GetField(dwField);
					if (pField == NULL)
					{
						ASSERT(FALSE);
						error = ERRORCODE_IntError;
						break;
					}

					// Build name.
					pDescription->m_pszName = pszStringData;
					strcpy(pszStringData, pField->GetName());
					pszStringData += strlen(pDescription->m_pszName)+1;

					pDescription->m_dwType = pField->GetType();
					pDescription->m_dwUserData = pField->GetUserData();
					pDescription->m_dwSize = pField->GetSize();

					// Move to the next description.
					pDescription++;
				}

				// Build the empty entry which terminates the list.
				if (error == ERRORCODE_None)
				{
					pDescription->m_pszName = NULL;
					pDescription->m_dwType = 0;
					pDescription->m_dwUserData = 0;
					pDescription->m_dwSize = 0;
				}
			}
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get a list of indicies in "CFlatFileIndexDescription" format.
ERRORCODE CFlatFileHeaderRecord::BuildIndexDescriptions(CFlatFileIndexDescription* pDescription, DWORD* pdwSize /*=NULL*/)
{
	ERRORCODE error = ERRORCODE_None;

	ASSERT((pDescription != NULL) || (pdwSize != NULL));

	// Get the required size.
	DWORD dwRequiredSize;
	if ((error = SizeIndexDescriptions(&dwRequiredSize)) == ERRORCODE_None)
	{
		// If no description buffer was passed in, then simply return the required size.
		if (pDescription == NULL)
		{
			*pdwSize = dwRequiredSize;
		}
		else
		{
			// We have a buffer. If the caller has specified a size, make sure it is sufficient.
			if (*pdwSize != NULL)
			{
				 if (*pdwSize < dwRequiredSize)
				 {
					// Not enough memory.
					error = ERRORCODE_Memory;
				 }

				 // Always return the required size.
				 *pdwSize = dwRequiredSize;
			}

			if (error == ERRORCODE_None)
			{
				// Everthing's in place, build the index descriptions.
				LPSTR pszStringData = (LPSTR)(pDescription+GetIndexCount()+1);

				// Build all indicies.
				for (DWORD dwIndex = 0; dwIndex < GetIndexCount(); dwIndex++)
				{
					// Build the current index.
					CIndex* pIndex = GetIndex(dwIndex);
					if (pIndex == NULL)
					{
						ASSERT(FALSE);
						error = ERRORCODE_IntError;
						break;
					}

					// Build name.
					pDescription->m_pszName = pszStringData;
					strcpy(pszStringData, pIndex->GetName());
					pszStringData += strlen(pDescription->m_pszName)+1;

					pDescription->m_dwFlags = pIndex->GetFlags();
					pDescription->m_dwUserData = pIndex->GetUserData();

					// Build the field name string.
					pDescription->m_pszField = pszStringData;
					pszStringData--;				// pretend we're pointing to the terminator
					for (DWORD dwFieldNumberIndex = 0; dwFieldNumberIndex < pIndex->GetFieldCount(); dwFieldNumberIndex++)
					{
						// Append the current name.
						pszStringData++;			// advance past previous terminator
						LPCSTR pszFieldName = GetField(pIndex->GetFieldNumber(dwFieldNumberIndex))->GetName();
						strcpy(pszStringData, pszFieldName);
						pszStringData += strlen(pszFieldName);
						*pszStringData = '|';	// append terminator
					}

					*(pszStringData++) = '\0';	// final terminator

					// Move to the next description.
					pDescription++;
				}

				// Build the empty entry which terminates the list.
				if (error == ERRORCODE_None)
				{
					pDescription->m_pszName = NULL;
					pDescription->m_dwFlags = 0;
					pDescription->m_dwUserData = 0;
					pDescription->m_pszField = NULL;
				}
			}
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get all of the index records.
ERRORCODE CFlatFileHeaderRecord::GetIndexRecords(CFlatFileIndexRecordPointerArray* pIndexArray, DWORD* pdwCount, BOOL fReadOnly)
{
	ERRORCODE error = ERRORCODE_None;

	*pIndexArray = NULL;
	*pdwCount = 0;

	// Get the number of indicies we will be dealing with.
	DWORD dwCount = GetIndexCount();

	if (dwCount > 0)
	{
		// Allocate the memory needed to hold a pointer to each index.
		CFlatFileIndexRecordPointerArray IndexArray = NULL;
		if ((error = CFlatFileDatabase::AllocateMemory((FLATFILE_DATABASE_MEMORY_PTR*)&IndexArray, dwCount*sizeof(CFlatFileIndexRecord*))) == ERRORCODE_None)
		{
			DWORD dwIndex;

			// Initializa all the pointers to NULL.
			for (dwIndex = 0; dwIndex < dwCount; dwIndex++)
			{
				IndexArray[dwIndex] = NULL;
			}

			// We have the memory, attempt to lock the indicies.
			for (dwIndex = 0; dwIndex < dwCount; dwIndex++)
			{
				// Get pointer to index data.
				CIndex* pIndex = GetIndex(dwIndex);
				if ((pIndex == NULL) || (pIndex->m_dwRecordNumber == 0))
				{
					// Bad index data.
					ASSERT(FALSE);
					error = ERRORCODE_IntError;
					break;
				}

				// Attempt to get the index record.
				if ((error = ((CFlatFileDatabase*)database)->GetDatabaseRecord((DatabaseRecord**)&(IndexArray[dwIndex]), pIndex->m_dwRecordNumber, FRT_Index, fReadOnly)) != ERRORCODE_None)
				{
					// Could not get record, fail.
					ASSERT(FALSE);
					break;
				}
			}

			// Check if there was an error. If so, then release the index records
			// we were able to get.
			if (error != ERRORCODE_None)
			{
				for (dwIndex = 0; dwIndex < dwCount; dwIndex++)
				{
					if (IndexArray[dwIndex] != NULL)
					{
						if (fReadOnly)
						{
							IndexArray[dwIndex]->release_read_only();
						}
						else
						{
							IndexArray[dwIndex]->release();
						}
					}
				}
			}
			else
			{
				// We were able to lock all the records. Amazing!!
				*pIndexArray = IndexArray;
				*pdwCount = dwCount;
			}

			// If we got an error, free the memory block.
			if (error != ERRORCODE_None)
			{
				ASSERT(FALSE);
				CFlatFileDatabase::FreeMemory(IndexArray);
				IndexArray = NULL;
			}
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Release all of the index records.
ERRORCODE CFlatFileHeaderRecord::ReleaseIndexRecords(CFlatFileIndexRecordPointerArray IndexArray, DWORD dwCount, BOOL fReadOnly)
{
	ERRORCODE error = ERRORCODE_None;

	// Release each of the index records.
	for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
	{
		if (IndexArray[dwIndex] != NULL)
		{
			if (fReadOnly)
			{
				IndexArray[dwIndex]->release_read_only();
			}
			else
			{
				IndexArray[dwIndex]->release();
			}
		}
	}

	// Free the memory.
	CFlatFileDatabase::FreeMemory(IndexArray);
	IndexArray = NULL;

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Insert a new record into all the indicies.
ERRORCODE CFlatFileHeaderRecord::InsertRecord(DB_RECORD_NUMBER dwNumber, LPBYTE pData)
{
	ERRORCODE error = ERRORCODE_None;

	// Attempt to get all of the index records with modify locks.
	CFlatFileIndexRecordPointerArray IndexRecordArray;
	DWORD dwCount;
	if ((error = GetIndexRecords(&IndexRecordArray, &dwCount, FALSE)) == ERRORCODE_None)
	{
		// Loop through the indicies and update each one.
		DWORD dwIndex;
		for (dwIndex = 0; dwIndex < dwCount; dwIndex++)
		{
			// Update the current index descriptor.
			CIndex* pIndex = GetIndex(dwIndex);
			if (pIndex == NULL)
			{
				ASSERT(FALSE);
				error = ERRORCODE_IntError;
				break;
			}

			// Sanity check.
			if (IndexRecordArray[dwIndex] == NULL)
			{
				ASSERT(FALSE);
				error = ERRORCODE_IntError;
				break;
			}

			// Add the record to the index.
			error = IndexRecordArray[dwIndex]->InsertRecord(this, pIndex, dwNumber, pData);
		}

		// What should we do if we get an error? This is a really bad situation
		// since some of the indicies contain the record and some do not. We try
		// to clean up by deleting the record back out of the indicies which
		// we were able to add it into.
		if (error != ERRORCODE_None)
		{
			while (dwIndex > 0)
			{
				// Remove the record from the previous index.
				dwIndex--;
				if (IndexRecordArray[dwIndex] != NULL)
				{
					// Delete the record from the index.
					IndexRecordArray[dwIndex]->DeleteRecord(dwNumber);
				}
			}
		}

		// Release all the index records.
		ReleaseIndexRecords(IndexRecordArray, dwCount, FALSE);
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Delete an existing record from all the indicies.
ERRORCODE CFlatFileHeaderRecord::DeleteRecord(DB_RECORD_NUMBER dwNumber)
{
	ERRORCODE error = ERRORCODE_None;

	// Attempt to get all of the index records with modify locks.
	CFlatFileIndexRecordPointerArray IndexRecordArray;
	DWORD dwCount;
	if ((error = GetIndexRecords(&IndexRecordArray, &dwCount, FALSE)) == ERRORCODE_None)
	{
		// Loop through the indicies and update each one.
		DWORD dwIndex;
		for (dwIndex = 0; dwIndex < dwCount; dwIndex++)
		{
			// Sanity check.
			if (IndexRecordArray[dwIndex] == NULL)
			{
				ASSERT(FALSE);
				error = ERRORCODE_IntError;
				break;
			}

			// Add the record to the index.
			error = IndexRecordArray[dwIndex]->DeleteRecord(dwNumber);
		}

		// What should we do if we get an error? This is a really bad situation
		// since some of the indicies contain the record and some do not. For now,
		// we do NOTHING!?

		// Release all the index records.
		ReleaseIndexRecords(IndexRecordArray, dwCount, FALSE);
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// FreeData()
// The variable length portion of the data is kept in a block allocated
// from the heap. Free it if it exists.
void CFlatFileHeaderRecord::FreeData(void)
{
	CFlatFileDatabase::FreeMemory(m_pData);
	m_pData = NULL;
}

// SizeofData()
// Return the size of the record.
ST_MAN_SIZE CFlatFileHeaderRecord::SizeofData(StorageDevicePtr pDevice)
{
	return pDevice->size_record(sizeof(m_Record))+GetSize();
}

// RelocateData()
// This is a function called by the storage manager when our storage is
// ever relocated. This allows us to move any data we want saved.
ERRORCODE CFlatFileHeaderRecord::RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position)
{
	return ERRORCODE_None;
}

// CFlatFileDataRecord::CCreationData
// This class encapsulates the creation data for the data record.
CFlatFileHeaderRecord::CCreationData::CCreationData(LPVOID pUserData, DWORD dwUserDataLength, CFlatFileFieldDescription* pField, CFlatFileIndexDescription* pIndex)
{
	ASSERT(pField != NULL);
	m_pUserData = pUserData;
	m_dwUserDataLength = dwUserDataLength;
	m_pField = pField;
	m_pIndex = pIndex;
}

// Retrieve the field index of a given field name. It's convenient to pass the name
// as a pointer to the first character and a length. If the length parameter is not
// supplied, then the name is assume to be null terminated.
DWORD CFlatFileHeaderRecord::CCreationData::GetFieldIndex(LPCSTR pszFieldName, DWORD dwLength /*=(DWORD)-1*/) const
{
	// If the user has not passed a length, assume that the Field Name is null
	// terminated and use its length.
	if (dwLength == (DWORD)-1)
	{
		dwLength = strlen(pszFieldName);
	}

	// Search field descriptions for name.
	CFlatFileFieldDescription* pFieldDescription = GetFirstField();
	if (pFieldDescription != NULL)
	{
		DWORD dwIndex = 0;

		while (pFieldDescription->m_pszName != NULL)
		{
			LPCSTR pszName1 = pFieldDescription->m_pszName;
			LPCSTR pszName2 = pszFieldName;
			DWORD dwCount = dwLength;

			// Comparison loop.
			for (;;)
			{
				// Check if we've compared all the characters.
				if (dwCount == 0)
				{
					// Yes, check if we've reached the end of the name string.
					if (*pszName1 == '\0')
					{
						// Yes, a match!
						return dwIndex;
					}

					// Not a match, try the next name.
					break;
				}

				// Check if we've reached the end of the name string.
				if (*pszName1 == '\0')
				{
					// Yes, not a match.
					break;
				}

				// Compare the characters (not case sensitive.)
				if (toupper(*pszName1) != toupper(*pszName2))
				{
					// Not a match.
					break;
				}

				// The current characters match, try the next ones.
				pszName1++;
				pszName2++;
				dwCount--;
			}

			// Current field does not match, try the next one.
			dwIndex++;
			pFieldDescription++;
		}
	}

	// Not found, usually bad news.
	return (DWORD)-1;
}

//////////////////////////////////////////////////////////////////////////
// Flat file database

// Constructor
CFlatFileDatabase::CFlatFileDatabase(StorageDevice* pDevice, BOOL fSharable /*=TRUE*/)
	: Database(pDevice, fSharable)
{
	// Record creation methods.
	register_creation_method(FRT_Header, CFlatFileHeaderRecord::create);
	register_creation_method(FRT_RecordNumber, CFlatFileRecordNumberRecord::create);
	register_creation_method(FRT_Counter, CFlatFileCounterRecord::create);
	register_creation_method(FRT_Data, CFlatFileDataRecord::create);
	register_creation_method(FRT_Index, CFlatFileIndexRecord::create);
	register_creation_method(FRT_IndexBlock, CArrayElementRecord::create);

	// Initialize local data.
	m_pFieldDescriptions = NULL;
	m_pIndexDescriptions = NULL;
	m_dwFieldDescriptionCount = 0;
	m_dwIndexDescriptionCount = 0;

#ifdef DATABASE_STATISTICS
	m_lGetDatabaseRecord = 0;
	m_lGetDatabaseRecordRO = 0;
#endif
}

// Destructor
CFlatFileDatabase::~CFlatFileDatabase()
{
#ifdef DATABASE_STATISTICS
	TRACE("GetDatabaseRecord: %ld, GetDatabaseRecordRO: %ld\n", m_lGetDatabaseRecord, m_lGetDatabaseRecordRO);
#endif
	// Free any cached field descriptions.
	FreeMemory(m_pFieldDescriptions);
	m_pFieldDescriptions = NULL;

	// Free any cached index descriptions.
	FreeMemory(m_pIndexDescriptions);
	m_pIndexDescriptions = NULL;
}

// Initialize the database. The start of the database in the storage
// device and the creation data for the header record are passed as
// parameters.
ERRORCODE CFlatFileDatabase::initialize(ST_DEV_POSITION position, CFlatFileHeaderRecord::CCreationData* pCreationData)
{
	ERRORCODE error;

	// Do the regular init first.
	if ((error = Database::initialize(position)) == ERRORCODE_None)
	{
		// Create the standard records in the defined order.
		if ((CreateUserDataRecordNumber() != ERRORCODE_None)
		 || (CreateCounter() != ERRORCODE_None)
		 || (CreateHeader(pCreationData) != ERRORCODE_None)
		 || (CreateUserData(pCreationData) != ERRORCODE_None))
		{
			ASSERT(FALSE);
			initialized = FALSE;
			error = ERRORCODE_NotInitialized;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// reinitialize()
ERRORCODE CFlatFileDatabase::reinitialize(CFlatFileHeaderRecord::CCreationData* pCreationData)
{
	ERRORCODE error;

	// Do the regular reinit first.
	if ((error = Database::reinitialize()) == ERRORCODE_None)
	{
		// Create the standard records in the defined order.
		if ((CreateUserDataRecordNumber() != ERRORCODE_None)
		 || (CreateCounter() != ERRORCODE_None)
		 || (CreateHeader(pCreationData) != ERRORCODE_None)
		 || (CreateUserData(pCreationData) != ERRORCODE_None))
		{
			ASSERT(FALSE);
			initialized = FALSE;
			error = ERRORCODE_NotInitialized;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Create the user data record number record.
ERRORCODE CFlatFileDatabase::CreateUserDataRecordNumber(void)
{
	ERRORCODE error = ERRORCODE_None;

	// Create the record.
	CFlatFileRecordNumberRecord* pRecordNumberRecord = CreateRecordNumberRecord();

	// If there was an error, return it.
	if (pRecordNumberRecord == NULL)
	{
		ASSERT(FALSE);
		error = last_creation_error();
	}
	else
	{
		// Make sure we got the correct record number.
		if (pRecordNumberRecord->Id() != FLATDB_USERDATA_RECORD)
		{
			ASSERT(FALSE);
			error = ERRORCODE_IntError;
		}
		else
		{
			// We were successful, release the lock.
			pRecordNumberRecord->release();
			pRecordNumberRecord = NULL;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Create the counter record.
ERRORCODE CFlatFileDatabase::CreateCounter(void)
{
	ERRORCODE error = ERRORCODE_None;

	// Create the record.
	CFlatFileCounterRecord* pCounterRecord = CreateCounterRecord();

	// If there was an error, return it.
	if (pCounterRecord == NULL)
	{
		ASSERT(FALSE);
		error = last_creation_error();
	}
	else
	{
		// Make sure we got the correct record number.
		if (pCounterRecord->Id() != FLATDB_COUNTER_RECORD)
		{
			ASSERT(FALSE);
			error = ERRORCODE_IntError;
		}
		else
		{
			// We were successful, release the lock.
			pCounterRecord->release();
			pCounterRecord = NULL;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Create the header record.
ERRORCODE CFlatFileDatabase::CreateHeader(CFlatFileHeaderRecord::CCreationData* pCreationData)
{
	ERRORCODE error = ERRORCODE_None;

	// We must have some creation data!
	ASSERT(pCreationData != NULL);
	if (pCreationData == NULL)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	else
	{
		// Create the record.
		CFlatFileHeaderRecord* pHeaderRecord = CreateHeaderRecord(pCreationData);

		// If there was an error, return it.
		if (pHeaderRecord == NULL)
		{
			ASSERT(FALSE);
			error = last_creation_error();
		}
		else
		{
			// Make sure we got the correct record number.
			if (pHeaderRecord->Id() != FLATDB_HEADER_RECORD)
			{
				ASSERT(FALSE);
				error = ERRORCODE_IntError;
			}
			else
			{
				// We were successful, release the lock.
				pHeaderRecord->release();
				pHeaderRecord = NULL;
			}
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Create the user data record.
ERRORCODE CFlatFileDatabase::CreateUserData(CFlatFileHeaderRecord::CCreationData* pCreationData)
{
	ERRORCODE error = ERRORCODE_None;

	// We must have some creation data!
	ASSERT(pCreationData != NULL);
	if (pCreationData == NULL)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	else
	{
		// Check if the user has specified some data in the creation data.
		if ((pCreationData->GetUserData() != NULL)
		 && (pCreationData->GetUserDataLength() > 0))
		{
			// Create the data record.
			error = SetUserData(pCreationData->GetUserData(), pCreationData->GetUserDataLength());
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get a record from the database.
ERRORCODE CFlatFileDatabase::GetDatabaseRecord(DatabaseRecord** ppRecord, DB_RECORD_NUMBER dwNumber, DB_RECORD_TYPE nType, BOOL fReadOnly /*=FALSE*/)
{
#ifdef DATABASE_STATISTICS
	if (fReadOnly)
	{
		m_lGetDatabaseRecordRO++;
	}
	else
	{
		m_lGetDatabaseRecord++;
	}
#endif
	ERRORCODE error = ERRORCODE_None;

	// Read the record.
	*ppRecord = get_record(dwNumber, &error, nType, fReadOnly);

	// If we got a NULL pointer, make sure there is an error number.
	if (*ppRecord == NULL)
	{
		if (error == ERRORCODE_None)
		{
			ASSERT(FALSE);
			error = ERRORCODE_IntError;
		}
	}

	// If the pointer is not NULL, make sure there is no error number.
	if (*ppRecord != NULL)
	{
		if (error != ERRORCODE_None)
		{
			// This is a weird situation and should never happen. Since it has
			// happened, clean up as best we can.
			ASSERT(FALSE);
			if (fReadOnly)
			{
				(*ppRecord)->release_read_only();
			}
			else
			{
				(*ppRecord)->release();
			}
			*ppRecord = NULL;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get the header record.
ERRORCODE CFlatFileDatabase::GetHeaderRecord(CFlatFileHeaderRecord** ppHeader, BOOL fReadOnly /*=TRUE*/)
{
	ERRORCODE error = ERRORCODE_None;

	// Read the header record
	error = GetDatabaseRecord((DatabaseRecord**)ppHeader, FLATDB_HEADER_RECORD, FRT_Header, fReadOnly);

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get the counter record.
ERRORCODE CFlatFileDatabase::GetCounterRecord(CFlatFileCounterRecord** ppCounter)
{
	ERRORCODE error = ERRORCODE_None;

	// Read the counter record. Always "modify" lock it.
	error = GetDatabaseRecord((DatabaseRecord**)ppCounter, FLATDB_COUNTER_RECORD, FRT_Counter, FALSE);

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get the user data record number record.
ERRORCODE CFlatFileDatabase::GetUserDataRecordNumberRecord(CFlatFileRecordNumberRecord** ppUserDataRecordNumber, BOOL fReadOnly /*=TRUE*/)
{
	ERRORCODE error = ERRORCODE_None;

	// Read the user data record number record.
	error = GetDatabaseRecord((DatabaseRecord**)ppUserDataRecordNumber, FLATDB_USERDATA_RECORD, FRT_RecordNumber, fReadOnly);

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Insert a record into the database.
ERRORCODE CFlatFileDatabase::InsertRecord(LPBYTE pData, DB_RECORD_NUMBER* pdwNumber)
{
	ERRORCODE error = ERRORCODE_None;

	*pdwNumber = 0;

	// Read the header record
	CFlatFileHeaderRecord* pHeader;
	if ((error = GetHeaderRecord(&pHeader)) == ERRORCODE_None)
	{
		// Validate the data.
		if (IsBadHugeReadPtr(pData, pHeader->GetRecordSize()))
		{
			ASSERT(FALSE);
			error = ERRORCODE_IntError;
		}
		else
		{
			// Create a record in the database for the data.
			MemoryDevice Device;
			Device.AddPiece(pData, pHeader->GetRecordSize());
			CFlatFileDataRecord::CCreationData CreationData(&Device, CFlatFileDataRecord::CCreationData::Normal);
			CFlatFileDataRecord* pDataRecord = CreateDataRecord(&CreationData);

			if (pDataRecord == NULL)
			{
				ASSERT(FALSE);
				error = last_creation_error();
			}
			else
			{
				// The record was successfully created. Remember the record number.
				DB_RECORD_NUMBER dwNumber = pDataRecord->Id();

				// Update the indicies to include the new record.
				error = pHeader->InsertRecord(dwNumber, pData);
				
				// If everything worked, save the record number.
				if (error == ERRORCODE_None)
				{
					*pdwNumber = dwNumber;
				}

				// Release the record.
				pDataRecord->release();
				pDataRecord = NULL;

				// If something went wrong, delete the record.
				if (error != ERRORCODE_None)
				{
					delete_record(dwNumber, FRT_Data);
				}
			}
		}

		// Release the header record.
		pHeader->release_read_only();
		pHeader = NULL;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Delete a record from the database.
ERRORCODE CFlatFileDatabase::DeleteRecord(DWORD dwNumber)
{
	ERRORCODE error = ERRORCODE_None;

	// Attempt to delete the specified record.
	error = delete_record(dwNumber);

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get the number of records in a database.
ERRORCODE CFlatFileDatabase::GetRecordCount(DWORD dwIndex, DWORD* pdwCount)
{
	ERRORCODE error = ERRORCODE_None;

	*pdwCount = (DWORD)0;

	// Read the header record
	CFlatFileHeaderRecord* pHeader;
	if ((error = GetHeaderRecord(&pHeader)) == ERRORCODE_None)
	{
		// Validate the index number.
		if (dwIndex >= pHeader->GetIndexCount())
		{
			ASSERT(FALSE);
			error = ERRORCODE_BadParameter;
		}
		else
		{
			// Get the index record.
			CFlatFileIndexRecord* pIndexRecord;
			if ((error = GetDatabaseRecord((DatabaseRecord**)&pIndexRecord, pHeader->GetIndex(dwIndex)->m_dwRecordNumber, FRT_Index, TRUE)) == ERRORCODE_None)
			{
				// Get the count.
				*pdwCount = pIndexRecord->ElementCount();

				// Release the index record.
				pIndexRecord->release_read_only();
				pIndexRecord = NULL;
			}
		}

		// Release the header record.
		pHeader->release_read_only();
		pHeader = NULL;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get the size of a record in the database (total size to hold all fields.)
ERRORCODE CFlatFileDatabase::GetRecordSize(DWORD* pdwSize)
{
	ERRORCODE error = ERRORCODE_None;

	*pdwSize = (DWORD)0;

	// Read the header record
	CFlatFileHeaderRecord* pHeader;
	if ((error = GetHeaderRecord(&pHeader)) == ERRORCODE_None)
	{
		// Get the record size.
		*pdwSize = pHeader->GetRecordSize();

		// Release the header record.
		pHeader->release_read_only();
		pHeader = NULL;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get a record's number and/or data.
ERRORCODE CFlatFileDatabase::GetRecord(DWORD dwIndex, DWORD dwSequence, DWORD* pdwNumber, LPBYTE pData)
{
	ERRORCODE error = ERRORCODE_None;

	if (pdwNumber != NULL)
	{
		*pdwNumber = (DWORD)0;
	}

	// Read the header record
	CFlatFileHeaderRecord* pHeader;
	if ((error = GetHeaderRecord(&pHeader)) == ERRORCODE_None)
	{
		// Validate the index number.
		if (dwIndex >= pHeader->GetIndexCount())
		{
			ASSERT(FALSE);
			error = ERRORCODE_BadParameter;
		}
		else
		{
			// Get the index record.
			CFlatFileIndexRecord* pIndexRecord;
			if ((error = GetDatabaseRecord((DatabaseRecord**)&pIndexRecord, pHeader->GetIndex(dwIndex)->m_dwRecordNumber, FRT_Index, TRUE)) == ERRORCODE_None)
			{
				// Validate the sequence number.
				if (dwSequence >= pIndexRecord->ElementCount())
				{
//					ASSERT(FALSE);
					error = ERRORCODE_DoesNotExist;
				}
				else
				{
					// Get the record number at the specified sequence number.
					CFlatFileIndexArrayElement* pIndexElement = NULL;
					TRY
					{
						pIndexElement = (CFlatFileIndexArrayElement*)(pIndexRecord->GetAt(dwSequence));
					}
					CATCH(CErrorcodeException, e)
					{
						ASSERT(FALSE);
						error = e->m_error;
					}
					AND_CATCH_ALL(e)
					{
						error = ERRORCODE_IntError;
					}
					END_CATCH_ALL

					if (error == ERRORCODE_None)
					{
						// Save the record number if the caller wants.
						if (pdwNumber != NULL)
						{
							*pdwNumber = pIndexElement->m_dwRecordNumber;
						}

						// Read the data if the user wants.
						if (pData != NULL)
						{
							// Read in the data for the record.
							CFlatFileDataRecord* pRecord;
							if ((error = GetDatabaseRecord((DatabaseRecord**)&pRecord, pIndexElement->m_dwRecordNumber, FRT_Data, TRUE)) == ERRORCODE_None)
							{
								// Get the data for the record.
								error = pRecord->GetData(&pData);

								// Release the record.
								pRecord->release_read_only();
								pRecord = NULL;
							}
						}
					}
				}

				// Release the index record.
				pIndexRecord->release_read_only();
				pIndexRecord = NULL;
			}
		}

		// Release the header record.
		pHeader->release_read_only();
		pHeader = NULL;
	}

//	ASSERT(error == ERRORCODE_None);
	return error;
}

// Delete a record.
ERRORCODE CFlatFileDatabase::DeleteRecord(DWORD dwIndex, DWORD dwSequence)
{
	ERRORCODE error = ERRORCODE_None;

	// Read the header record
	CFlatFileHeaderRecord* pHeader;
	if ((error = GetHeaderRecord(&pHeader)) == ERRORCODE_None)
	{
		// Validate the index number.
		if (dwIndex >= pHeader->GetIndexCount())
		{
			ASSERT(FALSE);
			error = ERRORCODE_BadParameter;
		}
		else
		{
			// Get the index record.
			CFlatFileIndexRecord* pIndexRecord;
			if ((error = GetDatabaseRecord((DatabaseRecord**)&pIndexRecord, pHeader->GetIndex(dwIndex)->m_dwRecordNumber, FRT_Index, TRUE)) == ERRORCODE_None)
			{
				// Validate the sequence number.
				if (dwSequence >= pIndexRecord->ElementCount())
				{
					ASSERT(FALSE);
					error = ERRORCODE_DoesNotExist;
				}
				else
				{
					// Get the record number at the specified sequence number.
					CFlatFileIndexArrayElement* pIndexElement = NULL;
					TRY
					{
						pIndexElement = (CFlatFileIndexArrayElement*)(pIndexRecord->GetAt(dwSequence));
					}
					CATCH(CErrorcodeException, e)
					{
						ASSERT(FALSE);
						error = e->m_error;
					}
					AND_CATCH_ALL(e)
					{
						error = ERRORCODE_IntError;
					}
					END_CATCH_ALL

					// Save the record number.
					DWORD dwNumber = pIndexElement->m_dwRecordNumber;

					// Release the index record since we are done with it and
					// others will need to use it.
					pIndexRecord->release_read_only();
					pIndexRecord = NULL;

					if (error == ERRORCODE_None)
					{
						// Attempt to delete the record from all of the indicies.
						if ((error = pHeader->DeleteRecord(dwNumber)) == ERRORCODE_None)
						{
							// The record was successfully deleted from the indicies.
							// Now, delete the record data. Note that only the main
							// data is deleted. Any related blobs, etc. must be deleted
							// by other code.
							delete_record(dwNumber);
						}
					}
				}

				// Release the index record.
				if (pIndexRecord != NULL)
				{
					pIndexRecord->release_read_only();
					pIndexRecord = NULL;
				}
			}
		}

		// Release the header record.
		pHeader->release_read_only();
		pHeader = NULL;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Delete all data records from the database.
ERRORCODE CFlatFileDatabase::DeleteAllRecords(void)
{
	ERRORCODE error = ERRORCODE_None;

	// To delete all the records, we extract the information we need to reinitialize
	// the database and do the reinitialization.

	// Get the current user data.
	LPVOID pUserData = NULL;
	DWORD dwUserDataSize = 0;
	error = GetUserData(&pUserData, &dwUserDataSize);
	if ((error = GetUserData(&pUserData, &dwUserDataSize)) == ERRORCODE_None)
	{
		// Get the field descriptions.
		CFlatFileFieldDescription* pFieldDescriptions = NULL;
		if ((error = GetFieldDescriptions(&pFieldDescriptions)) == ERRORCODE_None)
		{
			// Get the index descriptions.
			CFlatFileIndexDescription* pIndexDescriptions = NULL;
			if ((error = GetIndexDescriptions(&pIndexDescriptions)) == ERRORCODE_None)
			{
				// Get the current counter value.
				DWORD dwCounter = 0;
				if ((error = GetCounter(&dwCounter)) == ERRORCODE_None)
				{
					// Make a new creation data object.
					CFlatFileHeaderRecord::CCreationData CreationData(pUserData, dwUserDataSize, pFieldDescriptions, pIndexDescriptions);
					if ((error = reinitialize(&CreationData)) != ERRORCODE_None)
					{
						// Set the counter back to its last value. Ignore any errors since
						// we have already reinitialized the database.
						SetCounter(dwCounter);
					}
				}
			}
		}
	}

	// Free the user data if we have some.
	if (pUserData != NULL)
	{
		FreeMemory(pUserData);
		pUserData = NULL;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Find a record.
ERRORCODE CFlatFileDatabase::FindRecord(DWORD dwIndex, LPBYTE pData, DWORD* pdwPosition, BOOL* pfExact /*=NULL*/, DWORD dwFlags /*=0*/)
{
	ERRORCODE error = ERRORCODE_None;

	if (pfExact != NULL)
	{
		*pfExact = FALSE;
	}

	*pdwPosition = 0;

	// Read the header record
	CFlatFileHeaderRecord* pHeader;
	if ((error = GetHeaderRecord(&pHeader)) == ERRORCODE_None)
	{
		// Validate the index number.
		if (dwIndex >= pHeader->GetIndexCount())
		{
			ASSERT(FALSE);
			error = ERRORCODE_BadParameter;
		}
		else
		{
			// Get the index data.
			CFlatFileHeaderRecord::CIndex* pIndex = pHeader->GetIndex(dwIndex);
			if (pIndex == NULL)
			{
				ASSERT(FALSE);
				error = ERRORCODE_IntError;
			}
			else
			{
				// Get the index record.
				CFlatFileIndexRecord* pIndexRecord;
				if ((error = GetDatabaseRecord((DatabaseRecord**)&pIndexRecord, pIndex->m_dwRecordNumber, FRT_Index, TRUE)) == ERRORCODE_None)
				{
					// Attempt to find the specified data in the index.
					error = pIndexRecord->FindRecord(pHeader, pIndex, pData, pdwPosition, pfExact, dwFlags);

					// Release the index record.
					pIndexRecord->release_read_only();
					pIndexRecord = NULL;
				}
			}
		}

		// Release the header record.
		pHeader->release_read_only();
		pHeader = NULL;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Convert a record number to an index sequence number.
ERRORCODE CFlatFileDatabase::GetSequenceNumber(DWORD dwIndex, DB_RECORD_NUMBER dwNumber, DWORD* pdwSequenceNumber)
{
	ERRORCODE error = ERRORCODE_None;

	*pdwSequenceNumber = (DWORD)-1;

	// Read the header record
	CFlatFileHeaderRecord* pHeader;
	if ((error = GetHeaderRecord(&pHeader)) == ERRORCODE_None)
	{
		// Validate the index number.
		if (dwIndex >= pHeader->GetIndexCount())
		{
			ASSERT(FALSE);
			error = ERRORCODE_BadParameter;
		}
		else
		{
			// Get the index record.
			CFlatFileIndexRecord* pIndexRecord;
			if ((error = GetDatabaseRecord((DatabaseRecord**)&pIndexRecord, pHeader->GetIndex(dwIndex)->m_dwRecordNumber, FRT_Index, TRUE)) == ERRORCODE_None)
			{
				// Get the sequence number of the specified record number.
				error = pIndexRecord->GetSequenceNumber(dwNumber, pdwSequenceNumber);

				// Release the index record.
				if (pIndexRecord != NULL)
				{
					pIndexRecord->release_read_only();
					pIndexRecord = NULL;
				}
			}
		}

		// Release the header record.
		pHeader->release_read_only();
		pHeader = NULL;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get the field descriptions for the database.
ERRORCODE CFlatFileDatabase::GetFieldDescriptions(CFlatFileFieldDescription** ppDescriptions /*=NULL*/)
{
	ERRORCODE error = ERRORCODE_None;

	// Check if we already have field descriptions.
	if (m_pFieldDescriptions == NULL)
	{
		// Read the header record and get the field descriptions.
		CFlatFileHeaderRecord* pHeader;
		if ((error = GetHeaderRecord(&pHeader)) == ERRORCODE_None)
		{
			DWORD dwSize;

			// Ask the header for the size of the field descriptions.
			if ((error = pHeader->BuildFieldDescriptions(NULL, &dwSize)) == ERRORCODE_None)
			{
				// Allocate the memory for the field descriptions.
				if ((error = AllocateMemory((FLATFILE_DATABASE_MEMORY_PTR*)&m_pFieldDescriptions, dwSize)) == ERRORCODE_None)
				{
					// Build the field descriptions into the buffer.
					if ((error = pHeader->BuildFieldDescriptions(m_pFieldDescriptions, &dwSize)) == ERRORCODE_None)
					{
						// Count the field descriptions.
						m_dwFieldDescriptionCount = 0;
						CFlatFileFieldDescription* pField = m_pFieldDescriptions;
						while (pField->m_pszName != NULL)
						{
							m_dwFieldDescriptionCount++;
							pField++;
						}
					}

					// If there were any errors, clean up.
					if (error != ERRORCODE_None)
					{
						FreeMemory(m_pFieldDescriptions);
						m_pFieldDescriptions = NULL;
					}
				}
			}

			// Release the header record.
			pHeader->release_read_only();
			pHeader = NULL;
		}
	}

	// Return a pointer to the field description structure.
	if (ppDescriptions != NULL)
	{
		*ppDescriptions = m_pFieldDescriptions;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get the number of field descriptions for the database.
ERRORCODE CFlatFileDatabase::GetFieldDescriptionCount(DWORD* pdwCount)
{
	ERRORCODE error = ERRORCODE_None;

	// Check if we already know the count.
	if (m_pFieldDescriptions == NULL)
	{
		// Force field description information to be cached.
		error = GetFieldDescriptions();
	}

	*pdwCount = m_dwFieldDescriptionCount;

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get a particular database field description.
ERRORCODE CFlatFileDatabase::GetFieldDescription(CFlatFileFieldDescription** ppDescription, DWORD dwNumber)
{
	ERRORCODE error;

	*ppDescription = NULL;

	if ((error = GetFieldDescriptions()) == ERRORCODE_None)
	{
		if (dwNumber >= m_dwFieldDescriptionCount)
		{
			ASSERT(FALSE);
			error = ERRORCODE_DoesNotExist;
		}
		else
		{
			*ppDescription = &(m_pFieldDescriptions[dwNumber]);
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Return the number of a field description given its name.
ERRORCODE CFlatFileDatabase::GetFieldNumberFromName(LPCSTR pszName, DWORD* pdwNumber)
{
	ERRORCODE error;

	// Get the field description data.
	if ((error = GetFieldDescriptions()) == ERRORCODE_None)
	{
		// Search for specified field name.
		*pdwNumber = 0;
		CFlatFileFieldDescription* pDescription = m_pFieldDescriptions;
		while (pDescription->m_pszName != NULL)
		{
			// Is this the one?
			if (stricmp(pszName, pDescription->m_pszName) == 0)
			{
				// Yes.
				break;
			}

			// Not this description, try the next.
			(*pdwNumber)++;
			pDescription++;
		}

		// If we ran through all the descriptions, then the name did not exist.
		if (pDescription->m_pszName == NULL)
		{
			error = ERRORCODE_DoesNotExist;
		}
	}

	// Set the field number to -1 on any error.
	if (error != ERRORCODE_None)
	{
		*pdwNumber = (DWORD)-1;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get the index descriptions for the database.
ERRORCODE CFlatFileDatabase::GetIndexDescriptions(CFlatFileIndexDescription** ppDescriptions /*=NULL*/)
{
	ERRORCODE error = ERRORCODE_None;

	// Check if we already have index descriptions.
	if (m_pIndexDescriptions == NULL)
	{
		// Read the header record and get the index descriptions.
		CFlatFileHeaderRecord* pHeader;
		if ((error = GetHeaderRecord(&pHeader)) == ERRORCODE_None)
		{
			DWORD dwSize;

			// Ask the header for the size of the index descriptions.
			if ((error = pHeader->BuildIndexDescriptions(NULL, &dwSize)) == ERRORCODE_None)
			{
				// Allocate the memory for the index descriptions.
				if ((error = AllocateMemory((FLATFILE_DATABASE_MEMORY_PTR*)&m_pIndexDescriptions, dwSize)) == ERRORCODE_None)
				{
					// Build the index descriptions into the buffer.
					if ((error = pHeader->BuildIndexDescriptions(m_pIndexDescriptions, &dwSize)) == ERRORCODE_None)
					{
						// Count the index descriptions.
						m_dwIndexDescriptionCount = 0;
						CFlatFileIndexDescription* pIndex = m_pIndexDescriptions;
						while (pIndex->m_pszName != NULL)
						{
							m_dwIndexDescriptionCount++;
							pIndex++;
						}
					}

					// If there were any errors, clean up.
					if (error != ERRORCODE_None)
					{
						FreeMemory(m_pIndexDescriptions);
						m_pIndexDescriptions = NULL;
					}
				}
			}

			// Release the header record.
			pHeader->release_read_only();
			pHeader = NULL;
		}
	}

	// Return a pointer to the index description structure.
	if (ppDescriptions != NULL)
	{
		*ppDescriptions = m_pIndexDescriptions;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get the number of index descriptions for the database.
ERRORCODE CFlatFileDatabase::GetIndexDescriptionCount(DWORD* pdwCount)
{
	ERRORCODE error = ERRORCODE_None;

	// Check if we already know the count.
	if (m_pIndexDescriptions == NULL)
	{
		// Force index description information to be cached.
		error = GetIndexDescriptions();
	}

	*pdwCount = m_dwIndexDescriptionCount;

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get a particular database index description.
ERRORCODE CFlatFileDatabase::GetIndexDescription(CFlatFileIndexDescription** ppDescription, DWORD dwNumber)
{
	ERRORCODE error;

	*ppDescription = NULL;

	if ((error = GetIndexDescriptions()) == ERRORCODE_None)
	{
		if (dwNumber >= m_dwIndexDescriptionCount)
		{
			ASSERT(FALSE);
			error = ERRORCODE_DoesNotExist;
		}
		else
		{
			*ppDescription = &(m_pIndexDescriptions[dwNumber]);
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Return the number of an index description given its name.
ERRORCODE CFlatFileDatabase::GetIndexNumberFromName(LPCSTR pszName, DWORD* pdwNumber)
{
	ERRORCODE error;

	// Get the index description data.
	if ((error = GetIndexDescriptions()) == ERRORCODE_None)
	{
		// Search for specified index name.
		*pdwNumber = 0;
		CFlatFileIndexDescription* pDescription = m_pIndexDescriptions;
		while (pDescription->m_pszName != NULL)
		{
			// Is this the one?
			if (stricmp(pszName, pDescription->m_pszName) == 0)
			{
				// Yes.
				break;
			}

			// Not this description, try the next.
			(*pdwNumber)++;
			pDescription++;
		}

		// If we ran through all the descriptions, then the name did not exist.
		if (pDescription->m_pszName == NULL)
		{
			error = ERRORCODE_DoesNotExist;
		}
	}

	// Set the index number to -1 on any error.
	if (error != ERRORCODE_None)
	{
		*pdwNumber = (DWORD)-1;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Get the number of fields associated with the given index.
ERRORCODE CFlatFileDatabase::GetIndexFieldCount(DWORD* pdwCount, DWORD dwNumber)
{
	ERRORCODE error = ERRORCODE_None;

	// Read the header record and get the index information.
	CFlatFileHeaderRecord* pHeader;
	if ((error = GetHeaderRecord(&pHeader)) == ERRORCODE_None)
	{
		// Make sure the index exists.
		if (dwNumber >= pHeader->GetIndexCount())
		{
			error = ERRORCODE_DoesNotExist;
		}
		else
		{
			// Get a pointer to the index data.
			CFlatFileHeaderRecord::CIndex* pIndex = pHeader->GetIndex(dwNumber);
			ASSERT(pIndex != NULL);

			// Return the number of fields.
			*pdwCount = pIndex->GetFieldCount();
		}

		// Release the header record.
		pHeader->release_read_only();
		pHeader = NULL;
	}

	// Set the count -1 on any error.
	if (error != ERRORCODE_None)
	{
		*pdwCount = (DWORD)-1;
	}

	return error;
}

// Get the number of the given field associated with the given index.
ERRORCODE CFlatFileDatabase::GetIndexFieldNumber(DWORD* pdwFieldNumber, DWORD dwNumber, DWORD dwField)
{
	ERRORCODE error = ERRORCODE_None;

	// Read the header record and get the index information.
	CFlatFileHeaderRecord* pHeader;
	if ((error = GetHeaderRecord(&pHeader)) == ERRORCODE_None)
	{
		// Make sure the index exists.
		if (dwNumber >= pHeader->GetIndexCount())
		{
			error = ERRORCODE_DoesNotExist;
		}
		else
		{
			// Get a pointer to the index data.
			CFlatFileHeaderRecord::CIndex* pIndex = pHeader->GetIndex(dwNumber);
			ASSERT(pIndex != NULL);

			// Make sure the field exists.
			if (dwField >= pIndex->GetFieldCount())
			{
				error = ERRORCODE_DoesNotExist;
			}
			else
			{
				// Get the number of the field.
				*pdwFieldNumber = pIndex->GetFieldNumber(dwField);
			}
		}

		// Release the header record.
		pHeader->release_read_only();
		pHeader = NULL;
	}

	// Set the field number to -1 on any error.
	if (error != ERRORCODE_None)
	{
		*pdwFieldNumber = (DWORD)-1;
	}

	return error;
}

// Get the main field number of the given index.
ERRORCODE CFlatFileDatabase::GetMainFieldOfIndex(DWORD* pdwFieldNumber, DWORD dwIndexNumber)
{
	ERRORCODE error = ERRORCODE_None;

	// Read the header record and get the index information.
	CFlatFileHeaderRecord* pHeader;
	if ((error = GetHeaderRecord(&pHeader)) == ERRORCODE_None)
	{
		// Make sure the index exists.
		if (dwIndexNumber >= pHeader->GetIndexCount())
		{
			error = ERRORCODE_DoesNotExist;
		}
		else
		{
			// Get a pointer to the index data.
			CFlatFileHeaderRecord::CIndex* pIndex = pHeader->GetIndex(dwIndexNumber);
			ASSERT(pIndex != NULL);

			// Make sure the index has a main field.
			if ((pIndex == NULL) || (pIndex->GetFieldCount() < 1))
			{
				error = ERRORCODE_DoesNotExist;
			}
			else
			{
				// Get the number of the main field.
				*pdwFieldNumber = pIndex->GetFieldNumber(0);
			}
		}

		// Release the header record.
		pHeader->release_read_only();
		pHeader = NULL;
	}

	// Set the count -1 on any error.
	if (error != ERRORCODE_None)
	{
		*pdwFieldNumber = (DWORD)-1;
	}

	return error;
}


// Get the number of the index which has the given field as its main field.
ERRORCODE CFlatFileDatabase::GetIndexOfMainField(DWORD* pdwIndexNumber, DWORD dwFieldNumber)
{
	ERRORCODE error = ERRORCODE_None;

	*pdwIndexNumber = (DWORD)-1;

	// Read the header record and get the index information.
	CFlatFileHeaderRecord* pHeader;
	if ((error = GetHeaderRecord(&pHeader)) == ERRORCODE_None)
	{
		// Search all the indexes until we find one which has the given field as
		// its main field.
		for (DWORD dwIndex = 0; dwIndex < pHeader->GetIndexCount(); dwIndex++)
		{
			// Get a pointer to the index data.
			CFlatFileHeaderRecord::CIndex* pIndex = pHeader->GetIndex(dwIndex);
			ASSERT(pIndex != NULL);

			// Make sure the index has a main field.
			if ((pIndex != NULL) && (pIndex->GetFieldCount() >= 1))
			{
				if (pIndex->GetFieldNumber(0) == dwFieldNumber)
				{
					// Found an index (there may be more than one.)
					*pdwIndexNumber = dwIndex;
					break;
				}
			}
		}

		// Make sure we found an index.
		if (*pdwIndexNumber == (DWORD)-1)
		{
			error = ERRORCODE_DoesNotExist;
		}

		// Release the header record.
		pHeader->release_read_only();
		pHeader = NULL;
	}

	return error;
}

// Get the current counter and increment it.
ERRORCODE CFlatFileDatabase::GetCounter(DWORD* pdwCount)
{
	ERRORCODE error = ERRORCODE_None;

	*pdwCount = 0;

	// Read the counter record (it's locked by GetCounterRecord.)
	CFlatFileCounterRecord* pCounter;
	if ((error = GetCounterRecord(&pCounter)) == ERRORCODE_None)
	{
		// Save the current value. The current value is incremented.
		*pdwCount = pCounter->Increment();

		// Release the record. This will write the new value back to the database.
		pCounter->release();
		pCounter = NULL;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Set the current count to a particular value.
ERRORCODE CFlatFileDatabase::SetCounter(DWORD dwCount)
{
	ERRORCODE error = ERRORCODE_None;

	// Read the counter record (it's locked by GetCounterRecord.)
	CFlatFileCounterRecord* pCounter;
	if ((error = GetCounterRecord(&pCounter)) == ERRORCODE_None)
	{
		// Set the current value.
		pCounter->SetCount(dwCount);

		// Release the record. This will write the new value back to the database.
		pCounter->release();
		pCounter = NULL;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Read the current user data into a memory block. If the
// memory pointer passed in is NULL, a block will be allocated for
// the data. The caller must free the allocated block using the
// static function CFlatFileDatabase::FreeMemory().
ERRORCODE CFlatFileDatabase::GetUserData(LPVOID* ppData, DWORD* pdwData)
{
	ERRORCODE error = ERRORCODE_None;

	// Read the user data record number record (it's locked by GetUserDataRecordNumberRecord.)
	CFlatFileRecordNumberRecord* pRecordNumber;
	if ((error = GetUserDataRecordNumberRecord(&pRecordNumber, TRUE)) == ERRORCODE_None)
	{
		// Check if the user data exists.
		if (pRecordNumber->GetRecordNumber() != 0)
		{
			// Read in the user data record.
			CFlatFileDataRecord* pRecord;
			if ((error = GetDatabaseRecord((DatabaseRecord**)&pRecord, pRecordNumber->GetRecordNumber(), FRT_Data, TRUE)) == ERRORCODE_None)
			{
				// Get the data for the record.
				error = pRecord->GetData((LPBYTE*)ppData, pdwData);

				// Release the record.
				pRecord->release_read_only();
				pRecord = NULL;
			}
		}

		// Release the record number record.
		pRecordNumber->release_read_only();
		pRecordNumber = NULL;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Initialize a StorageFile for the current user data. Only read-only
// access is supported.
ERRORCODE CFlatFileDatabase::GetUserDataAsFile(StorageFile* pFile)
{
	ERRORCODE error = ERRORCODE_None;

	// Read the user data record number record (it's locked by GetUserDataRecordNumberRecord.)
	CFlatFileRecordNumberRecord* pRecordNumber;
	if ((error = GetUserDataRecordNumberRecord(&pRecordNumber, TRUE)) == ERRORCODE_None)
	{
		// Check if the user data exists.
		if (pRecordNumber->GetRecordNumber() != 0)
		{
			// Read in the user data record.
			CFlatFileDataRecord* pRecord;
			if ((error = GetDatabaseRecord((DatabaseRecord**)&pRecord, pRecordNumber->GetRecordNumber(), FRT_Data, TRUE)) == ERRORCODE_None)
			{
				// Get the data for the record.
				error = pRecord->GetDataAsFile(pFile);

				// We assume the file user will release the record.
				// On an error, GetDataAsFile() will release the record.
			}
		}

		// Release the record number record.
		pRecordNumber->release_read_only();
		pRecordNumber = NULL;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Set the current user data given a data pointer and a size.
ERRORCODE CFlatFileDatabase::SetUserData(LPVOID pData, DWORD dwSize)
{
	ERRORCODE error = ERRORCODE_None;

	// Validate memory block.
	if (IsBadHugeReadPtr(pData, dwSize))
	{
		// Bad memory block.
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	else
	{
		// Check if we're inserting new user data.
		if ((pData != NULL) && (dwSize > 0))
		{
			// Convert memory block to storage device.
			MemoryDevice Device;
			TRY
			{
				Device.AddPiece(pData, dwSize);
			}
			CATCH_ALL(e)
			{
				// Some sort of error.
				ASSERT(FALSE);
				error = ERRORCODE_IntError;
			}
			END_CATCH_ALL

			// Call the method to initialize the user data with a storage device.
			if (error == ERRORCODE_None)
			{
				error = SetUserData(&Device);
			}
		}
		else
		{
			// Delete the current user data.
			error = SetUserData(NULL);
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Set the current user data to the contents of a storage device.
ERRORCODE CFlatFileDatabase::SetUserData(StorageDevice* pDevice)
{
	ERRORCODE error = ERRORCODE_None;

	// Read the user data record number record (it's locked by GetUserDataRecordNumberRecord.)
	CFlatFileRecordNumberRecord* pRecordNumber;
	if ((error = GetUserDataRecordNumberRecord(&pRecordNumber, FALSE)) == ERRORCODE_None)
	{
		// Lock the old record (if there is one) to make sure we can zap it later.
		CFlatFileDataRecord* pOldUserDataRecord = NULL;
		if (pRecordNumber->GetRecordNumber() != 0)
		{
			error = GetDatabaseRecord((DatabaseRecord**)&pOldUserDataRecord, pRecordNumber->GetRecordNumber(), FRT_Data, FALSE);
		}

		// Create the new record if we have some data.
		DWORD dwNewUserDataRecordNumber = 0;
		if (error == ERRORCODE_None)
		{
			if (pDevice != NULL)
			{
				CFlatFileDataRecord::CCreationData CreationData(pDevice, CFlatFileDataRecord::CCreationData::Blob);
				CFlatFileDataRecord* pNewUserDataRecord = CreateDataRecord(&CreationData);

				// If the record could not be created, return the error.
				if (pNewUserDataRecord == NULL)
				{
					ASSERT(FALSE);
					error = last_creation_error();
				}
				else
				{
					// The record was created, save its number and release the record.
					dwNewUserDataRecordNumber = pNewUserDataRecord->Id();
					pNewUserDataRecord->release();
					pNewUserDataRecord = NULL;
				}
			}
		}

		// Change the record number record to point to the new data.
		if (error == ERRORCODE_None)
		{
			pRecordNumber->SetRecordNumber(dwNewUserDataRecordNumber);
		}

		// Release the record number record. If it was changed, then it will be written out.
		pRecordNumber->release();
		pRecordNumber == NULL;

		// Delete the old record if it exists.
		if (error == ERRORCODE_None)
		{
			if (pOldUserDataRecord != NULL)
			{
				// Zap the old user data record. We have it modify locked, but we
				// don't have to release it since we're using zap.
				error = pOldUserDataRecord->zap();
			}
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Read the blob data record into a memory block. If the memory
// pointer passed in is NULL, a block will be allocated for
// the data. The caller must free the allocated block using the
// static function CFlatFileDatabase::FreeMemory().
ERRORCODE CFlatFileDatabase::GetBlobData(LPVOID* ppData, DB_RECORD_NUMBER dwNumber)
{
	ERRORCODE error = ERRORCODE_None;

	// Read in the blob data record.
	CFlatFileDataRecord* pRecord;
	if ((error = GetDatabaseRecord((DatabaseRecord**)&pRecord, dwNumber, FRT_Data, TRUE)) == ERRORCODE_None)
	{
		// Get the data for the record.
		error = pRecord->GetData((LPBYTE*)ppData);

		// Release the record.
		pRecord->release_read_only();
		pRecord = NULL;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Initialize a StorageFile for the given blob data record. Only read-only
// access is supported.
ERRORCODE CFlatFileDatabase::GetBlobDataAsFile(StorageFile* pFile, DB_RECORD_NUMBER dwNumber)
{
	ERRORCODE error = ERRORCODE_None;

	// Read in the blob data record.
	CFlatFileDataRecord* pRecord;
	if ((error = GetDatabaseRecord((DatabaseRecord**)&pRecord, dwNumber, FRT_Data, TRUE)) == ERRORCODE_None)
	{
		// Get the data for the record.
		error = pRecord->GetDataAsFile(pFile);

		// We assume the file user will release the record.
		// On an error, GetDataAsFile() will release the record.
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Create a blob record and set its data with the given data pointer and and size.
ERRORCODE CFlatFileDatabase::SetBlobData(LPVOID pData, DWORD dwSize, DB_RECORD_NUMBER* pdwNumber)
{
	ERRORCODE error = ERRORCODE_None;

	*pdwNumber = 0;

	// Validate memory block.
	if (IsBadHugeReadPtr(pData, dwSize))
	{
		// Bad memory block.
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	else
	{
		// Make sure we have some blob data.
		if ((pData != NULL) && (dwSize > 0))
		{
			// Bad parameters.
			ASSERT(FALSE);
			error = ERRORCODE_BadParameter;
		}
		else
		{
			// Convert memory block to storage device.
			MemoryDevice Device;
			TRY
			{
				Device.AddPiece(pData, dwSize);
			}
			CATCH_ALL(e)
			{
				// Some sort of error.
				ASSERT(FALSE);
				error = ERRORCODE_IntError;
			}
			END_CATCH_ALL

			// Call the method to initialize the blob data with a storage device.
			if (error == ERRORCODE_None)
			{
				error = SetBlobData(&Device, pdwNumber);
			}
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Set the current blob data to the contents of a storage device.
ERRORCODE CFlatFileDatabase::SetBlobData(StorageDevice* pDevice, DB_RECORD_NUMBER* pdwNumber)
{
	ERRORCODE error = ERRORCODE_None;

	*pdwNumber = 0;

	// Make sure we have some data.
	if (pDevice == NULL)
	{
		// Bad parameters.
		ASSERT(FALSE);
		error = ERRORCODE_BadParameter;
	}
	else
	{
		// Create the blob record.
		CFlatFileDataRecord::CCreationData CreationData(pDevice, CFlatFileDataRecord::CCreationData::Blob);
		CFlatFileDataRecord* pNewBlobRecord = CreateDataRecord(&CreationData);

		// If the record could not be created, return the error.
		if (pNewBlobRecord == NULL)
		{
			ASSERT(FALSE);
			error = last_creation_error();
		}
		else
		{
			// The record was created, save its number and release the record.
			*pdwNumber = pNewBlobRecord->Id();
			pNewBlobRecord->release();
			pNewBlobRecord = NULL;
		}
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Allocate a huge block of memory.
ERRORCODE CFlatFileDatabase::AllocateMemory(FLATFILE_DATABASE_MEMORY_PTR* ppMemory, DWORD dwSize)
{
	ERRORCODE error = ERRORCODE_None;

	// Allocate a huge block of memory.
	*ppMemory = NULL;
	TRY
	{
#ifdef WIN32
		*ppMemory = (FLATFILE_DATABASE_MEMORY_PTR)new BYTE[dwSize];
#else
		*ppMemory = (FLATFILE_DATABASE_MEMORY_PTR)halloc(dwSize, sizeof(BYTE));
#endif
	}
	END_TRY
	if (*ppMemory == NULL)
	{
		ASSERT(FALSE);
		error = ERRORCODE_Memory;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

// Free a block of memory allocated by AllocateMemory().
ERRORCODE CFlatFileDatabase::FreeMemory(FLATFILE_DATABASE_MEMORY_PTR pMemory)
{
	// Free a huge block of memory allocated with AllocateMemory().
#ifdef WIN32
		delete [] (BYTE*)pMemory;
#else
		if (pMemory != NULL)
		{
			hfree((void __huge *)pMemory);
		}
#endif
	return ERRORCODE_None;
}

// Fill a block of memory with a given value. The memory block can be > 64K.
void CFlatFileDatabase::FillBytes(FLATFILE_DATABASE_MEMORY_PTR pMemory, DWORD dwSize, BYTE bByte /*=0*/)
{
#ifdef WIN32
	// In Win32, memset handles > 64K blocks.
	memset(pMemory, bByte, dwSize);
#else
	// Have to do it in chunks.
	while (dwSize > 0)
	{
		// Figure our how many bytes we can fill on this pass.
		DWORD dwFill = ((DWORD)~LOWORD(pMemory))+1;
		if (dwFill > dwSize)
		{
			dwFill = dwSize;
		}

		// The above code can generate 65536 if the memory offset is zero and we are
		// filling 64K or more. In this case, we do the fill in two 32K blocks.
		if (dwFill >= 65536)
		{
			dwFill = 32768;
		}

		// Fill this chunk.
		memset(pMemory, bByte, (int)dwFill);

		// Advance to the next block.
		dwSize -= dwFill;
		pMemory = (FLATFILE_DATABASE_MEMORY_PTR)(((BYTE __huge *)pMemory)+dwFill);
	}
#endif
}

// Copy a huge block.
void CFlatFileDatabase::CopyBytes(FLATFILE_DATABASE_MEMORY_PTR pDestination, FLATFILE_DATABASE_MEMORY_CONST_PTR pSource, DWORD dwSize, BOOL fTerminate /*=FALSE*/)
{
#ifdef WIN32
	memcpy(pDestination, pSource, dwSize);
	if (fTerminate)
	{
		((BYTE *)pDestination)[dwSize] = '\0';
	}
#else
	hmemcpy(pDestination, pSource, dwSize);
	if (fTerminate)
	{
		((BYTE __huge *)pDestination)[dwSize] = '\0';
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
// Flat file database cursor and supporting classes

// A buffer for a flat file database record.

// Constructor.
CFlatFileDatabaseRecordBuffer::CFlatFileDatabaseRecordBuffer(CFlatFileDatabase* pDatabase)
{
	// Initialize data.
	ASSERT(pDatabase != NULL);
	m_pDatabase = pDatabase;
	m_dwSize = 0;
	m_pData = NULL;
}

// Destructor.
CFlatFileDatabaseRecordBuffer::~CFlatFileDatabaseRecordBuffer()
{
	TRY
	{
		FreeData();
	}
	END_TRY
}

// Allocate the data (m_dwSize must be valid).
void CFlatFileDatabaseRecordBuffer::AllocateData(void)
{
	// Sanity Check.
	CheckFlatFileCondition(m_pDatabase != NULL);

	// Check if data is already allocated.
	if (m_pData == NULL)
	{
		// Get the size to allocate.
		CheckFlatFileResult(m_pDatabase->GetRecordSize(&m_dwSize));

		// Allocate the data.
		CheckFlatFileResult(CFlatFileDatabase::AllocateMemory((FLATFILE_DATABASE_MEMORY_PTR*)&m_pData, m_dwSize));
	}
}

// Free any allocated data.
void CFlatFileDatabaseRecordBuffer::FreeData(void)
{
	ERRORCODE error = CFlatFileDatabase::FreeMemory(m_pData);
	m_pData = NULL;
	CheckFlatFileResult(error);
}

// Return address of data to use for a given offset.
LPBYTE CFlatFileDatabaseRecordBuffer::DataAddress(DWORD dwOffset)
{
	// Make sure the data is allocated.
	AllocateData();

	// Sanity check.
	CheckFlatFileCondition(
			(m_pData != NULL)
		&& (m_dwSize > 0)
		&& (dwOffset < m_dwSize));

	// Return pointer to first byte of data.
	return m_pData+dwOffset;
}

// Copy record data from another source.
void CFlatFileDatabaseRecordBuffer::SetData(LPBYTE pData)
{
	// Make sure the data is allocated.
	AllocateData();

	// Sanity check.
	CheckFlatFileCondition(
			(m_pData != NULL)
		&& (m_dwSize > 0)
		&& (!IsBadHugeReadPtr(pData, m_dwSize)));

	// Copy data.
	CFlatFileDatabase::CopyBytes(GetData(), pData, m_dwSize);
}

// Copy record buffer data from another source.
void CFlatFileDatabaseCursor::SetData(CFlatFileDatabaseRecordBuffer* pBuffer)
{
	// Sanity check.
	CheckFlatFileCondition(pBuffer != NULL);

	// Copy the data.
	SetData(pBuffer->GetData());
}

// Database field.

// Constructor.
CFlatFileDatabaseField::CFlatFileDatabaseField()
{
	// Initialize data.
	m_pDatabase = NULL;
	m_pDescription = NULL;
	m_pData = NULL;
}

// Destructor.
CFlatFileDatabaseField::~CFlatFileDatabaseField()
{
	TRY
	{
		Detach();
	}
	END_TRY
}

// Attach the field to a particular database, field description, and data address.
void CFlatFileDatabaseField::Attach(CFlatFileDatabase* pDatabase, CFlatFileFieldDescription* pDescription, LPBYTE pData)
{
	// Detach from any previous values.
	Detach();

	// Sanity check.
	CheckFlatFileCondition(
			(pDatabase != NULL)
		&& (pDescription != NULL)
		&& (pData != NULL));

	// Attach to current values.
	m_pDatabase = pDatabase;
	m_pDescription = pDescription;
	m_pData = pData;
}

// Detach the field from the current database, description, and data.
void CFlatFileDatabaseField::Detach(void)
{
	// Simply set pointer to NULL.
	m_pDatabase = NULL;
	m_pDescription = NULL;
	m_pData = NULL;
}

// Return a pointer to the name of the field.
LPCSTR CFlatFileDatabaseField::GetName(void)
{
	CheckFlatFileCondition(m_pDescription != NULL);
	return m_pDescription->GetName();
}

// Get the type of the field.
DWORD CFlatFileDatabaseField::GetType(void)
{
	CheckFlatFileCondition(m_pDescription != NULL);
	return m_pDescription->GetType();
}

// Get the size of the field.
DWORD CFlatFileDatabaseField::GetSize(void)
{
	CheckFlatFileCondition(m_pDescription != NULL);
	return m_pDescription->GetSize();
}

// Initialize the field back to an empty state.
void CFlatFileDatabaseField::Initialize(void)
{
	// By default, we simply set the bytes to zero.
	CheckFlatFileCondition(m_pData != NULL);
	CFlatFileDatabase::FillBytes(m_pData, GetSize(), 0);
	SetModified(FALSE);
}

// Set counter from database.
void CFlatFileDatabaseField::SetCounter(BOOL fForceNewCounter /*=FALSE*/)
{
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_pDescription != NULL)
		&& (m_pData != NULL));

	// Only do something if this is a counter field.
	if (GetType() == FFT_Counter32)
	{
		// If the field already has a value, don't change it unless the
		// "force new counter" flag is set.
		if (fForceNewCounter || (*Unsigned32Ptr() == 0))
		{
			DWORD dwCounter;
			CheckFlatFileResult(m_pDatabase->GetCounter(&dwCounter));
			*Unsigned32Ptr() = dwCounter;
			SetModified();
		}
	}
}

// Get value as string.
LPCSTR CFlatFileDatabaseField::GetAsString(void)
{
	CheckFlatFileResult(ERRORCODE_IntError);
	return NULL;
}

// Set value as string.
void CFlatFileDatabaseField::SetAsString(LPCSTR pszString)
{
	CheckFlatFileResult(ERRORCODE_IntError);
}

// Get value as 32-bit signed number.
long CFlatFileDatabaseField::GetAsSigned32(void)
{
	CheckFlatFileResult(ERRORCODE_IntError);
	return 0;
}

// Set value as 32-bit signed number.
void CFlatFileDatabaseField::SetAsSigned32(long lNumber)
{
	CheckFlatFileResult(ERRORCODE_IntError);
}

// Get value as 32-bit unsigned number.
DWORD CFlatFileDatabaseField::GetAsUnsigned32(void)
{
	CheckFlatFileResult(ERRORCODE_IntError);
	return 0;
}

// Set value as 32-bit unsigned number.
void CFlatFileDatabaseField::SetAsUnsigned32(DWORD dwNumber)
{
	CheckFlatFileResult(ERRORCODE_IntError);
}

// Create a sorted array element (see SORTARRY.H) containing the current field value.
CSortedPtrArrayElement* CFlatFileDatabaseField::GetSortedArrayElement(void)
{
	CheckFlatFileResult(ERRORCODE_IntError);
	return NULL;
}

// String field.

// Constructor.
CFlatFileDatabaseFieldString::CFlatFileDatabaseFieldString()
{
}

// Destructor.
CFlatFileDatabaseFieldString::~CFlatFileDatabaseFieldString()
{
}

// Get value as string.
LPCSTR CFlatFileDatabaseFieldString::GetAsString(void)
{
	CheckFlatFileCondition(m_pData != NULL);
	return StringPtr();
}

// Set value as string.
void CFlatFileDatabaseFieldString::SetAsString(LPCSTR pszString)
{
	CheckFlatFileCondition(m_pData != NULL);

	// Figure out how much to copy. The string will be clipped to
	// the maximum field width.
	DWORD dwSize = strlen(pszString);
	if (dwSize > GetSize()-1)
	{
		dwSize = GetSize()-1;
	}

	// Copy the data.
	CFlatFileDatabase::CopyBytes(StringPtr(), pszString, dwSize, TRUE);
	SetModified();
}

// Get value as 32-bit signed number.
long CFlatFileDatabaseFieldString::GetAsSigned32(void)
{
	CheckFlatFileCondition(m_pData != NULL);
	return strtol(StringPtr(), NULL, 10);
}

// Set value as 32-bit signed number.
void CFlatFileDatabaseFieldString::SetAsSigned32(long lNumber)
{
	CheckFlatFileCondition(m_pData != NULL);
	char szBuffer[128];
	SetAsString(ltoa(lNumber, szBuffer, 10));
}

// Get value as 32-bit unsigned number.
DWORD CFlatFileDatabaseFieldString::GetAsUnsigned32(void)
{
	CheckFlatFileCondition(m_pData != NULL);
	return strtoul(StringPtr(), NULL, 10);
}

// Set value as 32-bit unsigned number.
void CFlatFileDatabaseFieldString::SetAsUnsigned32(DWORD dwNumber)
{
	CheckFlatFileCondition(m_pData != NULL);
	char szBuffer[128];
	SetAsString(ultoa(dwNumber, szBuffer, 10));
}

// Create a sorted array element (see SORTARRY.H) containing the current field value.
CSortedPtrArrayElement* CFlatFileDatabaseFieldString::GetSortedArrayElement(void)
{
	CSortedPtrArrayElement* pElement = NULL;

	TRY
	{
		pElement = new CSortedPtrArrayElementString(GetAsString());
	}
	END_TRY;

	CheckFlatFileAllocation(pElement);
	return pElement;
}

// Blob field.

// Constructor.
CFlatFileDatabaseFieldBlob::CFlatFileDatabaseFieldBlob()
{
}

// Destructor.
CFlatFileDatabaseFieldBlob::~CFlatFileDatabaseFieldBlob()
{
}

// Get value as 32-bit unsigned number.
DWORD CFlatFileDatabaseFieldBlob::GetAsUnsigned32(void)
{
	CheckFlatFileCondition(m_pData != NULL);
	return *Unsigned32Ptr();
}

// Set value as 32-bit unsigned number.
void CFlatFileDatabaseFieldBlob::SetAsUnsigned32(DWORD dwNumber)
{
	CheckFlatFileCondition(m_pData != NULL);
	*Unsigned32Ptr() = dwNumber;
	SetModified();
}

// 32-bit unsigned counter field.

// Constructor.
CFlatFileDatabaseFieldCounter32::CFlatFileDatabaseFieldCounter32()
{
}

// Destructor.
CFlatFileDatabaseFieldCounter32::~CFlatFileDatabaseFieldCounter32()
{
}

// Get value as string.
LPCSTR CFlatFileDatabaseFieldCounter32::GetAsString(void)
{
	CheckFlatFileCondition(m_pData != NULL);

	// Convert to local string.
	char szBuffer[128];
	ultoa(*Unsigned32Ptr(), szBuffer, 10);
	TRY
	{
		m_csValue = szBuffer;
	}
	CATCH_ALL(e)
	{
		CheckFlatFileAllocation(FALSE);
	}
	END_CATCH_ALL

	// Return pointer to string.
	return (LPCSTR)m_csValue;
}

// Set value as string.
void CFlatFileDatabaseFieldCounter32::SetAsString(LPCSTR pszString)
{
	CheckFlatFileCondition(m_pData != NULL);
	*Unsigned32Ptr() = strtol(pszString, NULL, 10);
	SetModified();
}

// Get value as 32-bit signed number.
long CFlatFileDatabaseFieldCounter32::GetAsSigned32(void)
{
	return (long)GetAsUnsigned32();
}

// Set value as 32-bit signed number.
void CFlatFileDatabaseFieldCounter32::SetAsSigned32(long lNumber)
{
	SetAsUnsigned32((DWORD)lNumber);
}

// Get value as 32-bit unsigned number.
DWORD CFlatFileDatabaseFieldCounter32::GetAsUnsigned32(void)
{
	CheckFlatFileCondition(m_pData != NULL);
	return *Unsigned32Ptr();
}

// Set value as 32-bit unsigned number.
void CFlatFileDatabaseFieldCounter32::SetAsUnsigned32(DWORD dwNumber)
{
	CheckFlatFileCondition(m_pData != NULL);
	*Unsigned32Ptr() = dwNumber;
	SetModified();
}

// Create a sorted array element (see SORTARRY.H) containing the current field value.
CSortedPtrArrayElement* CFlatFileDatabaseFieldCounter32::GetSortedArrayElement(void)
{
	CSortedPtrArrayElement* pElement = NULL;

	TRY
	{
		pElement = new CSortedPtrArrayElementUnsigned32(GetAsUnsigned32());
	}
	END_TRY;

	CheckFlatFileAllocation(pElement);
	return pElement;
}

// 32-bit signed integer field.

// Constructor.
CFlatFileDatabaseFieldSigned32::CFlatFileDatabaseFieldSigned32()
{
}

// Destructor.
CFlatFileDatabaseFieldSigned32::~CFlatFileDatabaseFieldSigned32()
{
}

// Get value as string.
LPCSTR CFlatFileDatabaseFieldSigned32::GetAsString(void)
{
	CheckFlatFileCondition(m_pData != NULL);

	// Convert to local string.
	char szBuffer[128];
	ltoa(*Signed32Ptr(), szBuffer, 10);
	TRY
	{
		m_csValue = szBuffer;
	}
	CATCH_ALL(e)
	{
		CheckFlatFileAllocation(FALSE);
	}
	END_CATCH_ALL

	// Return pointer to string.
	return (LPCSTR)m_csValue;
}

// Set value as string.
void CFlatFileDatabaseFieldSigned32::SetAsString(LPCSTR pszString)
{
	CheckFlatFileCondition(m_pData != NULL);
	*Signed32Ptr() = strtol(pszString, NULL, 10);
	SetModified();
}

// Get value as 32-bit signed number.
long CFlatFileDatabaseFieldSigned32::GetAsSigned32(void)
{
	CheckFlatFileCondition(m_pData != NULL);
	return *Signed32Ptr();
}

// Set value as 32-bit signed number.
void CFlatFileDatabaseFieldSigned32::SetAsSigned32(long lNumber)
{
	CheckFlatFileCondition(m_pData != NULL);
	*Signed32Ptr() = lNumber;
	SetModified();
}

// Get value as 32-bit unsigned number.
DWORD CFlatFileDatabaseFieldSigned32::GetAsUnsigned32(void)
{
	return (DWORD)GetAsSigned32();
}

// Set value as 32-bit unsigned number.
void CFlatFileDatabaseFieldSigned32::SetAsUnsigned32(DWORD dwNumber)
{
	SetAsSigned32((long)dwNumber);
}

// Create a sorted array element (see SORTARRY.H) containing the current field value.
CSortedPtrArrayElement* CFlatFileDatabaseFieldSigned32::GetSortedArrayElement(void)
{
	CSortedPtrArrayElement* pElement = NULL;

	TRY
	{
		pElement = new CSortedPtrArrayElementSigned32(GetAsSigned32());
	}
	END_TRY;

	CheckFlatFileAllocation(pElement);
	return pElement;
}

// 32-bit unsigned integer field.

// Constructor.
CFlatFileDatabaseFieldUnsigned32::CFlatFileDatabaseFieldUnsigned32()
{
}

// Destructor.
CFlatFileDatabaseFieldUnsigned32::~CFlatFileDatabaseFieldUnsigned32()
{
}

// Get value as string.
LPCSTR CFlatFileDatabaseFieldUnsigned32::GetAsString(void)
{
	CheckFlatFileCondition(m_pData != NULL);

	// Convert to local string.
	char szBuffer[128];
	ultoa(*Unsigned32Ptr(), szBuffer, 10);
	TRY
	{
		m_csValue = szBuffer;
	}
	CATCH_ALL(e)
	{
		CheckFlatFileAllocation(FALSE);
	}
	END_CATCH_ALL

	// Return pointer to string.
	return (LPCSTR)m_csValue;
}

// Set value as string.
void CFlatFileDatabaseFieldUnsigned32::SetAsString(LPCSTR pszString)
{
	CheckFlatFileCondition(m_pData != NULL);
	*Unsigned32Ptr() = strtol(pszString, NULL, 10);
	SetModified();
}

// Get value as 32-bit signed number.
long CFlatFileDatabaseFieldUnsigned32::GetAsSigned32(void)
{
	return (long)GetAsUnsigned32();
}

// Set value as 32-bit signed number.
void CFlatFileDatabaseFieldUnsigned32::SetAsSigned32(long lNumber)
{
	SetAsUnsigned32((DWORD)lNumber);
}

// Get value as 32-bit unsigned number.
DWORD CFlatFileDatabaseFieldUnsigned32::GetAsUnsigned32(void)
{
	CheckFlatFileCondition(m_pData != NULL);
	return *Unsigned32Ptr();
}

// Set value as 32-bit unsigned number.
void CFlatFileDatabaseFieldUnsigned32::SetAsUnsigned32(DWORD dwNumber)
{
	CheckFlatFileCondition(m_pData != NULL);
	*Unsigned32Ptr() = dwNumber;
	SetModified();
}

// Create a sorted array element (see SORTARRY.H) containing the current field value.
CSortedPtrArrayElement* CFlatFileDatabaseFieldUnsigned32::GetSortedArrayElement(void)
{
	CSortedPtrArrayElement* pElement = NULL;

	TRY
	{
		pElement = new CSortedPtrArrayElementUnsigned32(GetAsUnsigned32());
	}
	END_TRY;

	CheckFlatFileAllocation(pElement);
	return pElement;
}

// Cursor for high level database access.

// Constructor.
CFlatFileDatabaseCursor::CFlatFileDatabaseCursor()
{
	// Initialize data.
	m_pDatabase = NULL;
	m_pBuffer = NULL;
	m_pFieldArray = NULL;
	m_dwFieldCount = 0;
	m_dwPosition = (DWORD)-1;
	m_dwIndex = (DWORD)-1;
}

// Destructor.
CFlatFileDatabaseCursor::~CFlatFileDatabaseCursor()
{
	TRY
	{
		Detach();
	}
	END_TRY
}

// Attach the cursor to a database.
void CFlatFileDatabaseCursor::Attach(CFlatFileDatabase* pDatabase)
{
	// Sanity check.
	CheckFlatFileCondition(pDatabase != NULL);

	// Detach from any current database.
	Detach();

	// Save a pointer to the database.
	m_pDatabase = pDatabase;

	TRY
	{
		// Allocate a record buffer.
		AllocateBuffer();

		// Allocate the field objects
		AllocateFields();
	}
	CATCH_ALL(e)
	{
		// If we got any kind of an error, clean up.
		TRY
		{
			Detach();
		}
		END_TRY

		THROW_LAST();
	}
	END_CATCH_ALL
};

// Detach the cursor from a database.
void CFlatFileDatabaseCursor::Detach(void)
{
	// Disconnect from the database.
	m_pDatabase = NULL;

	// Free any record buffers.
	TRY
	{
		FreeBuffer();
	}
	END_TRY

	// Free any field objects.
	TRY
	{
		FreeFields();
	}
	END_TRY

	// Clear position and index values.
	m_dwPosition = (DWORD)-1;
	m_dwIndex = (DWORD)-1;
};

// Allocate and initialize the record buffer used by this cursor.
void CFlatFileDatabaseCursor::AllocateBuffer(void)
{
	// Check if the record buffer has already been allocated.
	if (m_pBuffer == NULL)
	{
		// Sanity check.
		CheckFlatFileCondition(m_pDatabase != NULL);

		TRY
		{
			// Allocate the buffer.
			m_pBuffer = new CFlatFileDatabaseRecordBuffer(m_pDatabase);
		}
		END_TRY

		// Make sure the allocation was successful.
		CheckFlatFileAllocation(m_pBuffer);
	}
}

// Free the allocated record buffer.
void CFlatFileDatabaseCursor::FreeBuffer(void)
{
	// Delete the record buffer.
	TRY
	{
		delete m_pBuffer;
	}
	END_TRY

	m_pBuffer = NULL;
}

// Allocate and initialize the array of field objects for the attached database.
void CFlatFileDatabaseCursor::AllocateFields(void)
{
	// Check if field objects are already available.
	if (m_pFieldArray == NULL)
	{
		// Sanity check.
		CheckFlatFileCondition(
				(m_pDatabase != NULL)
			&& (m_pBuffer != NULL));

		// Get the number of fields we will be allocating.
		CheckFlatFileResult(m_pDatabase->GetFieldDescriptionCount(&m_dwFieldCount));

		if (m_dwFieldCount > 0)
		{
			// Allocate the memory to hold the field objects.
			CheckFlatFileResult(CFlatFileDatabase::AllocateMemory((FLATFILE_DATABASE_MEMORY_PTR*)&m_pFieldArray, m_dwFieldCount*sizeof(CFlatFileDatabaseField*)));

			DWORD dwField;

			// Initialize each of the field object pointer to NULL.
			for (dwField = 0; dwField < m_dwFieldCount; dwField++)
			{
				m_pFieldArray[dwField] = NULL;
			}

			DWORD dwBufferOffset = 0;

			TRY
			{
				// Allocate a field object for each of the fields in the database.
				for (dwField = 0; dwField < m_dwFieldCount; dwField++)
				{
					// Get the field description to use.
					CFlatFileFieldDescription* pFieldDescription;
					CheckFlatFileResult(m_pDatabase->GetFieldDescription(&pFieldDescription, dwField));

					// Allocate the field object.
					CFlatFileDatabaseField* pField = NULL;
					TRY
					{
						// Allocate the correct field object base on field type.
						switch (pFieldDescription->GetType())
						{
							case FFT_String:		// Null terminated string
							{
								pField = new CFlatFileDatabaseFieldString();
								break;
							}
							case FFT_Blob:			// Binary blob data (2^32-1 max byte length)
							{
								pField = new CFlatFileDatabaseFieldBlob();
								break;
							}
							case FFT_Counter32:	// Unsigned 32 bit integer which is auto-incremented at record creation
							{
								pField = new CFlatFileDatabaseFieldCounter32();
								break;
							}
							case FFT_Signed32:	// Signed 32 bit integer
							{
								pField = new CFlatFileDatabaseFieldSigned32();
								break;
							}
							case FFT_Unsigned32:	// Unsigned 32 bit integer
							{
								pField = new CFlatFileDatabaseFieldUnsigned32();
								break;
							}
							default:
							{
								CheckFlatFileResult(ERRORCODE_IntError);
								break;
							}
						}
					}
					END_TRY

					// Make sure the allocation was successful.
					CheckFlatFileAllocation(pField);

					// Save pointer to object in array.
					m_pFieldArray[dwField] = pField;

					// Get the address of the buffer data to use.
					LPBYTE pBufferData = m_pBuffer->DataAddress(dwBufferOffset);

					// Attach field object to descriptions and memory buffer.
					m_pFieldArray[dwField]->Attach(m_pDatabase, pFieldDescription, pBufferData);

					// Tell the field to initialize itself.
					m_pFieldArray[dwField]->Initialize();

					// Everything worked, bump the buffer offset by the size of the field.
					dwBufferOffset += pFieldDescription->GetSize();
				}
			}
			CATCH_ALL(e)
			{
				// If we got an error, free any allocated field objects.
				for (dwField = 0; dwField < m_dwFieldCount; dwField++)
				{
					TRY
					{
						delete m_pFieldArray[dwField];
					}
					END_TRY
					m_pFieldArray[dwField] = NULL;
				}
				CFlatFileDatabase::FreeMemory(m_pFieldArray);
				m_pFieldArray = NULL;
			}
			END_CATCH_ALL
		}
	}
}

// Free any allocated field objects.
void CFlatFileDatabaseCursor::FreeFields(void)
{
	// Check if field objects exist.
	if (m_pFieldArray != NULL)
	{
		// Loop through and destroy each field object.
		for (DWORD dwField = 0; dwField < m_dwFieldCount; dwField++)
		{
			// Delete the current field.
			TRY
			{
				delete m_pFieldArray[dwField];
			}
			END_TRY
			m_pFieldArray[dwField] = NULL;
		}

		// Delete the field array.
		CFlatFileDatabase::FreeMemory(m_pFieldArray);
		m_pFieldArray = NULL;
	}

	m_dwFieldCount = 0;
}

// Initialize all the fields.
void CFlatFileDatabaseCursor::InitializeFields(void)
{
	// Make sure field objects exist.
	CheckFlatFileCondition(m_pFieldArray != NULL);

	// Loop through and initialize each field object.
	for (DWORD dwField = 0; dwField < m_dwFieldCount; dwField++)
	{
		// Initialize the current field.
		m_pFieldArray[dwField]->Initialize();
	}
}

// Check if data has been modified.
BOOL CFlatFileDatabaseCursor::IsModified(void)
{
	// Check if any of the field objects are modified.
	if (m_pFieldArray != NULL)
	{
		// Loop through and destroy each field object.
		for (DWORD dwField = 0; dwField < m_dwFieldCount; dwField++)
		{
			// Check the current field.
			if (m_pFieldArray[dwField]->IsModified())
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

// Mark the current record as modified.
void CFlatFileDatabaseCursor::SetModified(BOOL fModified /*=TRUE*/)
{
	// Set the modified flags for all the field.
	if (m_pFieldArray != NULL)
	{
		for (DWORD dwField = 0; dwField < m_dwFieldCount; dwField++)
		{
			m_pFieldArray[dwField]->SetModified(fModified);
		}
	}
}

// Set the counter fields.
void CFlatFileDatabaseCursor::SetCounters(BOOL fForceNewCounters /*=FALSE*/)
{
	// Make sure field objects exist.
	CheckFlatFileCondition(m_pFieldArray != NULL);

	// Loop through and initialize each counter field object.
	for (DWORD dwField = 0; dwField < m_dwFieldCount; dwField++)
	{
		// Initialize the current field.
		m_pFieldArray[dwField]->SetCounter(fForceNewCounters);
	}
}

// Get the current user data into an allocated memory block.
// The block must be freed using CFlatFileDatabase::FreeMemory().
void CFlatFileDatabaseCursor::GetUserData(LPVOID* ppData, DWORD* pdwSize)
{
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (ppData != NULL));

	// Get user data from database.
	CheckFlatFileResult(m_pDatabase->GetUserData(ppData, pdwSize));
}

// Initialize a StorageFile to access the current user data. The record
// is read-only locked until the StorageFile is closed.
void CFlatFileDatabaseCursor::GetUserDataAsFile(StorageFile* pFile)
{
	CheckFlatFileCondition(m_pDatabase != NULL);
	CheckFlatFileResult(m_pDatabase->GetUserDataAsFile(pFile));
}

// Set the current user data from a memory block.
void CFlatFileDatabaseCursor::SetUserData(LPVOID pData, DWORD dwSize)
{
	CheckFlatFileCondition(m_pDatabase != NULL);
	CheckFlatFileResult(m_pDatabase->SetUserData(pData, dwSize));
}

// Set the current user data from a Storage Device.
void CFlatFileDatabaseCursor::SetUserData(StorageDevice* pDevice)
{
	CheckFlatFileCondition(m_pDatabase != NULL);
	CheckFlatFileResult(m_pDatabase->SetUserData(pDevice));
}

// Delete the current user data.
void CFlatFileDatabaseCursor::DeleteUserData(void)
{
	CheckFlatFileCondition(
		m_pDatabase != NULL);
	CheckFlatFileResult(m_pDatabase->DeleteUserData());
}

// Get field information.
CFlatFileDatabaseField** CFlatFileDatabaseCursor::GetFields(void)
{
	CheckFlatFileCondition(m_pFieldArray != NULL);
	return m_pFieldArray;
}

DWORD CFlatFileDatabaseCursor::GetFieldCount(void)
{
	CheckFlatFileCondition(m_pFieldArray != NULL);
	return m_dwFieldCount;
}

CFlatFileDatabaseField* CFlatFileDatabaseCursor::GetField(DWORD dwNumber)
{
	CheckFlatFileCondition(
			(m_pFieldArray != NULL)
		&& (dwNumber < m_dwFieldCount));
	return m_pFieldArray[dwNumber];
}

CFlatFileDatabaseField* CFlatFileDatabaseCursor::GetField(LPCSTR pszName)
{
	CFlatFileDatabaseField* pResult = NULL;

	// Sanity check.
	CheckFlatFileCondition(m_pFieldArray != NULL);

	// Get the field number.
	DWORD dwField = GetFieldNumberFromName(pszName);

	// Get pointer to field structure.
	if (dwField != (DWORD)-1)
	{
		pResult = m_pFieldArray[dwField];
	}

	// NULL is returned if the field was not found.
	return pResult;
}

DWORD CFlatFileDatabaseCursor::GetFieldNumberFromName(LPCSTR pszName)
{
	DWORD dwNumber = (DWORD)-1;

	// Sanity check.
	CheckFlatFileCondition(m_pFieldArray != NULL);

	// Loop through fields looking for the one with the given name.
	for (DWORD dwField = 0; dwField < m_dwFieldCount; dwField++)
	{
		// Get a pointer to the field.
		CFlatFileDatabaseField* pField = m_pFieldArray[dwField];
		CheckFlatFileCondition(pField != NULL);

		// Check the name of the current field.
		LPCSTR pszFieldName = pField->GetName();

		if (stricmp(pszFieldName, pszName) == 0)
		{
			// Found it.
			dwNumber = dwField;
			break;
		}
	}

	// If we didn't find the field, (DWORD)-1 is returned.

	return dwNumber;
}

// Get index information.
CFlatFileIndexDescription* CFlatFileDatabaseCursor::GetIndexDescriptions(void)
{
	// Sanity Check.
	CheckFlatFileCondition(m_pDatabase != NULL);

	// Get pointer to index descriptions.
	CFlatFileIndexDescription* pDescriptions;
	CheckFlatFileResult(m_pDatabase->GetIndexDescriptions(&pDescriptions));

	return pDescriptions;
}

DWORD CFlatFileDatabaseCursor::GetIndexDescriptionCount(void)
{
	// Sanity Check.
	CheckFlatFileCondition(m_pDatabase != NULL);

	// Get pointer to index descriptions.
	DWORD dwDescriptionCount;
	CheckFlatFileResult(m_pDatabase->GetIndexDescriptionCount(&dwDescriptionCount));

	return dwDescriptionCount;
}

CFlatFileIndexDescription* CFlatFileDatabaseCursor::GetIndexDescription(DWORD dwNumber)
{
	// Sanity Check.
	CheckFlatFileCondition(m_pDatabase != NULL);

	// Get pointer to index description.
	CFlatFileIndexDescription* pDescription;
	CheckFlatFileResult(m_pDatabase->GetIndexDescription(&pDescription, dwNumber));

	return pDescription;
}

CFlatFileIndexDescription* CFlatFileDatabaseCursor::GetIndexDescription(LPCSTR pszName)
{
	return GetIndexDescription(GetIndexNumberFromName(pszName));
}

DWORD CFlatFileDatabaseCursor::GetIndexNumberFromName(LPCSTR pszName)
{
	// Sanity Check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (pszName != NULL));

	// Get number of named index.
	DWORD dwNumber;
	CheckFlatFileResult(m_pDatabase->GetIndexNumberFromName(pszName, &dwNumber));

	return dwNumber;
}

// Get the number of fields associated with the given index.
DWORD CFlatFileDatabaseCursor::GetIndexFieldCount(DWORD dwNumber)
{
	// Sanity Check.
	CheckFlatFileCondition(m_pDatabase != NULL);

	// Get number of fields associated with the given index.
	DWORD dwFieldCount;
	CheckFlatFileResult(m_pDatabase->GetIndexFieldCount(&dwFieldCount, dwNumber));

	return dwFieldCount;
}

// Get the number of the given field associated with the given index.
DWORD CFlatFileDatabaseCursor::GetIndexFieldNumber(DWORD dwNumber, DWORD dwField)
{
	// Sanity Check.
	CheckFlatFileCondition(m_pDatabase != NULL);

	// Get the number of the given field associated with the given index.
	DWORD dwFieldNumber;
	CheckFlatFileResult(m_pDatabase->GetIndexFieldNumber(&dwFieldNumber, dwNumber, dwField));

	return dwFieldNumber;
}

// Get the main field number of the given index.
DWORD CFlatFileDatabaseCursor::GetMainFieldOfIndex(DWORD dwIndexNumber)
{
	// Sanity Check.
	CheckFlatFileCondition(m_pDatabase != NULL);

	// Get the main field number of the given index.
	DWORD dwFieldNumber;
	CheckFlatFileResult(m_pDatabase->GetMainFieldOfIndex(&dwFieldNumber, dwIndexNumber));

	return dwFieldNumber;
}

// Get the number of the index which has the given field as its main field.
DWORD CFlatFileDatabaseCursor::GetIndexOfMainField(DWORD dwFieldNumber)
{
	// Sanity Check.
	CheckFlatFileCondition(m_pDatabase != NULL);

	// Get the number of the index which has the given field as its main field.
	DWORD dwIndexNumber;
	CheckFlatFileResult(m_pDatabase->GetIndexOfMainField(&dwIndexNumber, dwFieldNumber));

	return dwIndexNumber;
}

// Initialize a sorted array with the unique values of the main field associated with the current index.
void CFlatFileDatabaseCursor::GetSortedArrayOfValues(CSortedPtrArray* pArray)
{
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1)
		&& (pArray != NULL));

	// Remove any current values.
	pArray->DeleteAll();

	// Get the main field of the current index. This is the field for which we will
	// be collecting values.
	CFlatFileDatabaseField* pSearchField = GetField(GetMainFieldOfIndex(m_dwIndex));
	ASSERT(pSearchField != NULL);
	if (pSearchField != NULL)
	{
		// Start the search at the first record.
		First();

		// Loop until we've tried all the records.
		while (!AtEOF())
		{
			// Don't add empty values.
			if (strlen(pSearchField->GetAsString()) > 0)
			{
				// Add the current value.
				pArray->Add(pSearchField->GetSortedArrayElement());
			}

			// Find the next unique value.
			Find(NULL, FFF_GreaterThan);
		}
	}
}

// Set the current index by number.
void CFlatFileDatabaseCursor::SetIndex(DWORD dwNumber)
{
	// Sanity Check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (dwNumber < GetIndexDescriptionCount()));

	// Check if we have a current position. If so then convert it to a record number
	// and translate it back to the new position in the new index.
	if ((m_dwIndex != (DWORD)-1) && (m_dwPosition < GetRecordCount()))
	{
		DWORD dwRecordNumber;

		// Attempt to get the actual record number of the current position.
		ERRORCODE error = m_pDatabase->GetRecord(m_dwIndex, m_dwPosition, &dwRecordNumber, NULL);
		if (error == ERRORCODE_None)
		{
			// Convert the record number into the sequence number for the new index.
			error = m_pDatabase->GetSequenceNumber(dwNumber, dwRecordNumber, &m_dwPosition);
			if (error != ERRORCODE_None)
			{
				// Could not get new sequence number.
				m_dwPosition = (DWORD)-1;
			}
		}

		// Catch this for debugging, otherwise ignore.
		ASSERT(error == ERRORCODE_None);
	}

	// Set the new index.
	m_dwIndex = dwNumber;
}

// Set the current index by name.
void CFlatFileDatabaseCursor::SetIndex(LPCSTR pszName)
{
	// Sanity Check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (pszName != NULL));

	// Convert index name to number.
	DWORD dwNumber;
	CheckFlatFileResult(m_pDatabase->GetIndexNumberFromName(pszName, &dwNumber));

	// Set the current index.
	SetIndex(dwNumber);
}

// Get the current index number.
DWORD CFlatFileDatabaseCursor::GetIndex(void)
{
	// Sanity Check.
	CheckFlatFileCondition(m_pDatabase != NULL);
	return m_dwIndex; 
}

// Get the description of the current index.
CFlatFileIndexDescription* CFlatFileDatabaseCursor::GetIndexDescription(void)
{
	// Sanity Check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1));
	return GetIndexDescription(m_dwIndex); 
}

// Get the number of records in the current index.
DWORD CFlatFileDatabaseCursor::GetRecordCount(void)
{
	// Sanity Check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1));

	// Get the number of records.
	DWORD dwCount;
	CheckFlatFileResult(m_pDatabase->GetRecordCount(m_dwIndex, &dwCount));

	return dwCount;
}

// Move to first record.
void CFlatFileDatabaseCursor::First(void)
{
	// Sanity Check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1));

	// Move to first position.
	SetPosition(0);
}

// Move to last record.
void CFlatFileDatabaseCursor::Last(void)
{
	// Sanity Check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1));

	// Move to last position.
	SetPosition(0xfffffffeL);
}

// Move to next record.
void CFlatFileDatabaseCursor::Next(void)
{
	// Sanity Check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1)
		&& (m_dwPosition != (DWORD)-1));

	// Move to next position.
	SetPosition(GetPosition()+1);
}

// Move to previous record.
void CFlatFileDatabaseCursor::Previous(void)
{
	// Sanity Check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1)
		&& (m_dwPosition != (DWORD)-1));

	// Move to previous position.
	if (GetPosition() > 0)
	{
		SetPosition(GetPosition()-1);
	}
}

// Move to specific postion.
void CFlatFileDatabaseCursor::SetPosition(DWORD dwPosition)
{
	// Sanity check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1)
		&& (dwPosition != (DWORD)-1));

	// Clip position to record range.
	DWORD dwRecordCount = GetRecordCount();
	if (dwPosition > dwRecordCount)
	{
		dwPosition = dwRecordCount;
	}

	// Save the current position and read the appropriate record.
	m_dwPosition = dwPosition;
	Refresh();
}

// Return current position.
DWORD CFlatFileDatabaseCursor::GetPosition(void)
{
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1));
	return m_dwPosition;
}

// Check if at start of index.
BOOL CFlatFileDatabaseCursor::AtBOF(void)
{
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1));
	return (m_dwPosition != (DWORD)-1) && (GetPosition() == 0);
}

// Check if at start of index.
BOOL CFlatFileDatabaseCursor::AtEOF(void)
{
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1));
	return (m_dwPosition != (DWORD)-1) && (GetPosition() >= GetRecordCount());
}

// Update the field data with the current record data.
void CFlatFileDatabaseCursor::Refresh(void)
{
	// Make sure we're ready.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1)
		&& (m_pBuffer != NULL));

	// Attempt to read the record
	ERRORCODE error = m_pDatabase->GetRecord(m_dwIndex, m_dwPosition, NULL, m_pBuffer->GetData());

	// If the record does not exist, set the field to empty.
	if (error == ERRORCODE_DoesNotExist)
	{
		InitializeFields();
		error = ERRORCODE_None;
	}

	// The record is not modified.
	SetModified(FALSE);

	// Check for errors.
	CheckFlatFileResult(error);
}

// Delete the current record.
void CFlatFileDatabaseCursor::Delete(BOOL fDeleteBlobs /*=FALSE*/)
{
	// Sanity check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1)
		&& (m_dwPosition != (DWORD)-1)
		&& (m_pBuffer != NULL));

	// Delete any associated blobs.
	if (fDeleteBlobs)
	{
		// Loop through each field and check its type. If it's a blob, then
		// delete it and set the field data to 0.
		if (m_pFieldArray != NULL)
		{
			for (DWORD dwField = 0; dwField < m_dwFieldCount; dwField++)
			{
				CFlatFileDatabaseField* pField = m_pFieldArray[dwField];
				if (pField != NULL)
				{
					if (pField->GetType() == FFT_Blob)
					{
						DWORD dwBlob = pField->GetAsUnsigned32();
						if (dwBlob != 0)
						{
							TRY
							{
								TRACE("Delete Blob #%lu\n", dwBlob);
								DeleteRecord(dwBlob);
								pField->SetAsUnsigned32(0);
							}
							END_TRY
						}
					}
				}
			}
		}
	}

	// Delete the record.
	CheckFlatFileResult(m_pDatabase->DeleteRecord(m_dwIndex, m_dwPosition));

	// Refresh fields with next record.
	Refresh();
}

// Delete all records.
void CFlatFileDatabaseCursor::DeleteAll(void)
{
	// Sanity check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1)
		&& (m_pBuffer != NULL));

	// Delete the record.
	CheckFlatFileResult(m_pDatabase->DeleteAllRecords());

	// Refresh fields with next record.
	Refresh();
}

// Insert the current record.
void CFlatFileDatabaseCursor::Insert(void)
{
	// Sanity check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1)
		&& (m_pBuffer != NULL));

	// Insert the new record.
	DB_RECORD_NUMBER dwNumber;
	CheckFlatFileResult(m_pDatabase->InsertRecord(m_pBuffer->GetData(), &dwNumber));

	// Convert record number to sequence number.
	DWORD dwPosition;
	CheckFlatFileResult(m_pDatabase->GetSequenceNumber(m_dwIndex, dwNumber, &dwPosition));

	// Move to the new position. The data should already be correct.
	m_dwPosition = dwPosition;

	// The record is not modified.
	SetModified(FALSE);
}

// Update the current record.
void CFlatFileDatabaseCursor::Update(void)
{
	// Sanity check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1)
		&& (m_dwPosition != (DWORD)-1)
		&& (m_pBuffer != NULL));

	// Delete the current record without refreshing the data.
	CheckFlatFileResult(m_pDatabase->DeleteRecord(m_dwIndex, m_dwPosition));

	// Insert the new record.
	Insert();
}

// Initialize the current record to empty.
void CFlatFileDatabaseCursor::New(void)
{
	// Sanity check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1)
		&& (m_pBuffer != NULL));

	// Initialize the fields.
	InitializeFields();

	// Set the current position to -1.
	m_dwPosition = (DWORD)-1;
}

// Find a record.
void CFlatFileDatabaseCursor::Find(BOOL* pfExact /*=NULL*/, DWORD dwFlags /*=0*/)
{
	// Sanity Check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1)
		&& (m_pBuffer != NULL));

	// Move the cursor to position where the data exists or would be inserted.
	DWORD dwPosition;
	CheckFlatFileResult(m_pDatabase->FindRecord(m_dwIndex, m_pBuffer->GetData(), &dwPosition, pfExact, dwFlags));

	// Set the new position.
	SetPosition(dwPosition);
}

// Make a copy of another cursor.
void CFlatFileDatabaseCursor::SetCursor(CFlatFileDatabaseCursor& Cursor)
{
	// Detach from anything we are currently attached to.
	Detach();

	// Attach to other cursor's database.
	Attach(Cursor.GetDatabase());

	// Set the index to the other cursor's index.
	SetIndex(Cursor.GetIndex());

	// Set the position to the other cursor's position.
	m_dwPosition = Cursor.GetPosition();

	// Set the record data to the other cursor's record data.
	SetData(Cursor.GetBuffer());
}

// Copy record buffer data from another source.
void CFlatFileDatabaseCursor::SetData(LPBYTE pData)
{
	// Sanity Check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (m_dwIndex != (DWORD)-1)
		&& (m_pBuffer != NULL));

	// Copy data to buffer.
	*m_pBuffer = pData;
}

// Read the blob data record into a memory block. If the memory
// pointer passed in is NULL, a block will be allocated for
// the data. The caller must free the allocated block using the
// static function CFlatFileDatabase::FreeMemory().
LPVOID CFlatFileDatabaseCursor::GetBlobData(DB_RECORD_NUMBER dwNumber)
{
	// Sanity check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (dwNumber > 0));

	// Get the blob data.
	LPVOID pData;
	CheckFlatFileResult(m_pDatabase->GetBlobData(&pData, dwNumber));

	return pData;
}

// Initialize a StorageFile for the given blob data record. Only read-only
// access is supported.
void CFlatFileDatabaseCursor::GetBlobDataAsFile(StorageFile* pFile, DWORD dwNumber)
{
	// Sanity check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (pFile != NULL)
		&& (dwNumber > 0));

	// Get the blob data.
	CheckFlatFileResult(m_pDatabase->GetBlobDataAsFile(pFile, dwNumber));
}

// Create a blob record and set its data with the given data pointer and and size.
DB_RECORD_NUMBER CFlatFileDatabaseCursor::SetBlobData(LPVOID pData, DWORD dwSize)
{
	// Sanity check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (pData != NULL)
		&& (dwSize > 0));

	// Insert the new record.
	DB_RECORD_NUMBER dwNumber;
	CheckFlatFileResult(m_pDatabase->SetBlobData(pData, dwSize, &dwNumber));

	// Return record number.
	return dwNumber;
}

// Create a blob record and set its data with the given data pointer and and size.
DWORD CFlatFileDatabaseCursor::SetBlobData(StorageDevice* pDevice)
{
	// Sanity check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (pDevice != NULL));

	// Insert the new record.
	DB_RECORD_NUMBER dwNumber;
	CheckFlatFileResult(m_pDatabase->SetBlobData(pDevice, &dwNumber));

	// Return record number.
	return dwNumber;
}

// Delete a record from the database.
void CFlatFileDatabaseCursor::DeleteRecord(DWORD dwRecordNumber)
{
	// Sanity check.
	CheckFlatFileCondition(
			(m_pDatabase != NULL)
		&& (dwRecordNumber != (DWORD)-1));

	// Delete the record.
	CheckFlatFileResult(m_pDatabase->DeleteRecord(m_dwIndex, m_dwPosition));

	// Refresh fields with next record.
	Refresh();
}

//////////////////////////////////////////////////////////////////////////
// CFlatFileValueListManager
//
// This is a helper class which manages the lists of unique field values.

CFlatFileValueListManager::CFlatFileValueListManager()
{
	m_pDatabase = NULL;
}

CFlatFileValueListManager::~CFlatFileValueListManager()
{
	DeleteAll();
}

// Reinitialize the list manager for a given database.
void CFlatFileValueListManager::SetDatabase(CFlatFileDatabase* pDatabase)
{
	// Delete any lists we have for the old database.
	DeleteAll();

	// Detach our cursor from the old database.
	m_Cursor.Detach();

	// Save the pointer to the new database.
	m_pDatabase = pDatabase;

	// Attach our cursor to the new database if it exists.
	if (m_pDatabase != NULL)
	{
		m_Cursor.Attach(m_pDatabase);
	}
}

// Get the list associated with a given database field.
CSortedPtrArray* CFlatFileValueListManager::GetListForField(CFlatFileFieldDescription* pField)
{
	CSortedPtrArray* pList = NULL;

	// See if the list already exits.
	if (!Lookup(pField, (void*&)pList))
	{
		// Attempt to build the list from scratch.
		pList = BuildList(pField);
	}

	return pList;
}

// Get the list associated with a given database index.
CSortedPtrArray* CFlatFileValueListManager::GetListForIndex(CFlatFileIndexDescription* pIndex)
{
	CSortedPtrArray* pList = NULL;

	TRY
	{
		// Convert the description pointer into an index number.
		DWORD dwIndex = m_Cursor.GetIndexNumberFromName(pIndex->GetName());

		// Get the main field associated with the index.
		DWORD dwField = m_Cursor.GetMainFieldOfIndex(dwIndex);

		// Get a pointer to the field description.
		CFlatFileFieldDescription* pField = m_Cursor.GetField(dwField)->GetDescription();

		// Get the list for the given field description.
		pList = GetListForField(pField);
	}
	END_TRY

	return pList;
}

// Declare any lists associated with the given field to be modified.
void CFlatFileValueListManager::FieldModified(CFlatFileFieldDescription* pField /*=NULL*/)
{
	// NULL means all fields have been modified (as after a delete).
	if (pField == NULL)
	{
		DeleteAll();
	}
	else
	{
		// The field has been modified, so the list is no longer valid. Simply delete the list.
		DeleteList(pField);
	}
}

// Delete all the list manager entries.
void CFlatFileValueListManager::DeleteAll(void)
{
	// Delete all the list objects in our map.
	POSITION p = GetStartPosition();
	while (p != NULL)
	{
		// Get the current entry and delete the list object.
		CFlatFileFieldDescription* pField;
		CSortedPtrArray* pList;
		GetNextAssoc(p, (void*&)pField, (void*&)pList);
		delete pList;
	}

	// Remove deleted entries from map.
	RemoveAll();
}

// Delete a specific list manager entry.
void CFlatFileValueListManager::DeleteList(CFlatFileFieldDescription* pField)
{
	// Lookup the specified map entry.
	CSortedPtrArray* pList;
	if (Lookup(pField, (void*&)pList))
	{
		// Found the entry, delete the list and remove the entry.
		delete pList;
		RemoveKey(pField);
	}
}

// Build the list for a particular database field.
CSortedPtrArray* CFlatFileValueListManager::BuildList(CFlatFileFieldDescription* pField)
{
	CSortedPtrArray* pList = NULL;

	// Make sure we have a field and a database.
	ASSERT(pField != NULL);
	if (pField != NULL)
	{
		if (m_pDatabase != NULL)
		{
			// Attempt to build the list.
			TRY
			{
				// Allocate the new list.
				pList = new CSortedPtrArray;
				ASSERT(pList != NULL);
				if (pList != NULL)
				{
					// Get the field number of the field description.
					DWORD dwField = m_Cursor.GetFieldNumberFromName(pField->GetName());
					ASSERT(dwField != (DWORD)-1);
					if (dwField != (DWORD)-1)
					{
						// Get the number of the index which has the field as its main field.
						// This is the index which we will use to search and create the list.
						DWORD dwIndex = m_Cursor.GetIndexOfMainField(dwField);
						ASSERT(dwIndex != (DWORD)-1);
						if (dwIndex != (DWORD)-1)
						{
							// Set the cursor to use the index we found.
							m_Cursor.SetIndex(dwIndex);

							// Finally, we're ready to actually build the list.
							m_Cursor.GetSortedArrayOfValues(pList);

							// Everything worked, add the list to the map.
							SetAt(pField, pList);
						}
					}
				}
			}
			CATCH_ALL(e)
			{
				// Some sort of catastrophic error.
				delete pList;
				pList = NULL;
			}
			END_CATCH_ALL
		}
	}

	return pList;
}

//////////////////////////////////////////////////////////////////////////
// Test ourselves!

ERRORCODE CFlatFileDatabase::SelfTest(void)
{
	ERRORCODE error = ERRORCODE_None;

#if 0
// The following definitions are used to build and parse the user data
// for a PrintMaster address book database field.

// defined field editors
#define	FIELD_EDIT_NONE						(0)		// cannot be edited
#define	FIELD_EDIT_NORMAL						(1)		// type in string value
#define	FIELD_EDIT_LIST						(2)		// select from list or type in new value
#define	FIELD_EDIT_LIST_FIXED				(3)		// select from fixed list
#define	FIELD_EDIT_PICTURE					(4)		// choose picture (bmp)
#define	FIELD_EDIT_MEMO						(5)		// edit variable length text

// defined lists
#define	FIELD_LIST_NONE						(0)		// no specific list
#define	FIELD_LIST_RELATIONSHIP				(1)		// relationship list
#define	FIELD_LIST_STATE						(2)		// state list
#define	FIELD_LIST_COUNTRY					(3)		// Country list

// construct field user data
#define	MAKE_FIELD_USER_DATA(e, w)		MAKELONG(e, w)

// predefined common values
#define	FNOEDIT			MAKE_FIELD_USER_DATA(FIELD_EDIT_NONE, 0)
#define	FNORMAL			MAKE_FIELD_USER_DATA(FIELD_EDIT_NORMAL, 0)
#define	FLIST				MAKE_FIELD_USER_DATA(FIELD_EDIT_LIST, FIELD_LIST_NONE)
#define	FLISTFIXED		MAKE_FIELD_USER_DATA(FIELD_EDIT_LIST_FIXED, FIELD_LIST_NONE)
#define	FRELATIONSHIP	MAKE_FIELD_USER_DATA(FIELD_EDIT_LIST, FIELD_LIST_RELATIONSHIP)
#define	FSTATE			MAKE_FIELD_USER_DATA(FIELD_EDIT_LIST, FIELD_LIST_STATE)
#define	FCOUNTRY			MAKE_FIELD_USER_DATA(FIELD_EDIT_LIST, FIELD_LIST_COUNTRY)
#define	FPICTURE			MAKE_FIELD_USER_DATA(FIELD_EDIT_PICTURE, 0)
#define	FMEMO				MAKE_FIELD_USER_DATA(FIELD_EDIT_MEMO, 0)

// parse field user data
#define	FIELD_USER_DATA_EDITOR(d)	LOWORD(d)
#define	FIELD_USER_DATA_WORD(d)		HIWORD(d)

// The following definitions are used to build and parse the user data
// for a PrintMaster address book database index.

// defined index display types
#define	INDEX_DISPLAY_NONE					(0)		// do not display index
#define	INDEX_DISPLAY_ALPHA					(1)		// display index in alphabetic format
#define	INDEX_DISPLAY_ALPHA_LIST			(2)		// display index in alphabetic list format
#define	INDEX_DISPLAY_NUMERIC				(3)		// display index in numeric format
#define	INDEX_DISPLAY_ZIP						(4)		// display index in zip format

// defined alphabetic lists
#define	INDEX_LIST_NONE						(0)		// no specific list
#define	INDEX_LIST_RELATIONSHIP				(1)		// relationship list
#define	INDEX_LIST_STATE						(2)		// state list
#define	INDEX_LIST_COUNTRY					(3)		// country list

// construct index user data
#define	MAKE_INDEX_USER_DATA(d, w)			MAKELONG(d, w)

// predefined common values
#define	INODISPLAY		MAKE_INDEX_USER_DATA(INDEX_DISPLAY_NONE, 0)
#define	IALPHA(l)		MAKE_INDEX_USER_DATA(INDEX_DISPLAY_ALPHA, l)
#define	IALPHA1			IALPHA(1)
#define	IALPHA2			IALPHA(2)
#define	IALPHA3			IALPHA(3)
#define	INUMERIC			MAKE_INDEX_USER_DATA(INDEX_DISPLAY_NUMERIC, 0)
#define	IZIP(l)			MAKE_INDEX_USER_DATA(INDEX_DISPLAY_ZIP, l)
#define	IZIP1				IZIP(1)
#define	IZIP2				IZIP(2)
#define	IZIP3				IZIP(3)
#define	ILIST				MAKE_INDEX_USER_DATA(INDEX_DISPLAY_ALPHA_LIST, INDEX_LIST_NONE)
#define	IRELATIONSHIP	MAKE_INDEX_USER_DATA(INDEX_DISPLAY_ALPHA_LIST, INDEX_LIST_RELATIONSHIP)
#define	ISTATE			MAKE_INDEX_USER_DATA(INDEX_DISPLAY_ALPHA_LIST, INDEX_LIST_STATE)
#define	ICOUNTRY			MAKE_INDEX_USER_DATA(INDEX_DISPLAY_ALPHA_LIST, INDEX_LIST_COUNTRY)

// parse index user data
#define	INDEX_USER_DATA_DISPLAY(d)	LOWORD(d)
#define	INDEX_USER_DATA_WORD(d)		HIWORD(d)

// The structure of the standard address book database.

static char far szStandardDatabaseDescription[] = "PrintMaster Gold Standard Address Book Database v1.0|First Name|Last Name||";

static CFlatFileFieldDescription far StandardDatabaseFieldDescriptions[] =
{
	{ "__ID__",					FFT_Counter32,		FNOEDIT,			4		},
	{ "First Name",			FFT_String,			FNORMAL,			20+1	},
	{ "Last Name",				FFT_String,			FNORMAL,			20+1	},
	{ "Relationship",			FFT_String,			FRELATIONSHIP,	40+1	},
	{ "Title",					FFT_String,			FNORMAL,			40+1	},
	{ "Company",				FFT_String,			FNORMAL,			40+1	},
	{ "Address1",				FFT_String,			FNORMAL,			40+1	},
	{ "Address2",				FFT_String,			FNORMAL,			40+1	},
	{ "City/Town",				FFT_String,			FNORMAL,			20+1	},
	{ "State/Province",		FFT_String,			FNORMAL,			15+1	},
	{ "Zip/Postal Code",		FFT_String,			FNORMAL,			15+1	},
	{ "Country",				FFT_String,			FCOUNTRY,		50+1	},
	{ "Phone",					FFT_String,			FNORMAL,			25+1	},
	{ "Fax",						FFT_String,			FNORMAL,			25+1	},
	{ "Email",					FFT_String,			FNORMAL,			60+1	},
	{ "Birthday",				FFT_String,			FNORMAL,			20+1	},
	{ "Anniversary",			FFT_String,			FNORMAL,			20+1	},
	{ "Comments",				FFT_Blob,			FMEMO,			4		},
	{ "Picture",				FFT_Blob,			FPICTURE,		4		},
	{ "User Defined Info",	FFT_String,			FNORMAL,			50+1	},
	{ "User Defined Date",	FFT_String,			FNORMAL,			20+1	},
	{ NULL,						0,						0,					0		}
};

static CFlatFileIndexDescription far StandardDatabaseIndexDescriptions[] =
{
	{ "__ID__",					0,		INODISPLAY,		"__ID__"										},
	{ "Last Name",				0,		IALPHA1,			"Last Name|First Name"					},
	{ "First Name",			0,		IALPHA1,			"First Name|Last Name"					},
	{ "Relationship",			0,		IRELATIONSHIP,	"Relationship|Last Name|First Name"	},
	{ "Company",				0,		IALPHA1,			"Company"									},
	{ "City/Town",				0,		IALPHA1,			"City/Town"									},
	{ "State/Province",		0,		IALPHA2,			"State/Province"							},
	{ "Zip/Postal Code",		0,		IZIP2,			"Zip/Postal Code"							},
	{ "Country",				0,		ICOUNTRY,		"Country"									},
	{ "User Defined Info",	0,		IALPHA1,			"User Defined Info"						},
	{ NULL,						0,		0,					NULL											}
};

	struct CRecord
	{
		DWORD dwID;
		char szFirstName[20+1];
		char szLastName[20+1];
		char szRelationship[40+1];
		char szTitle[40+1];
		char szCompany[40+1];
		char szAddress1[40+1];
		char szAddress2[40+1];
		char szCity[20+1];
		char szState[15+1];
		char szZip[15+1];
		char szCountry[50+1];
		char szPhone[25+1];
		char szFax[25+1];
		char szEmail[60+1];
		char szBirthday[20+1];
		char szAnniversary[20+1];
		DWORD dwComments;
		DWORD dwPicture;
		char szUserDefinedInfo[50+1];
		char szUserDefinedDate[20+1];
	};

	static CRecord far Record[] =
	{
		{ 0, "Fred",		"Fischer", "Family", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "1/30/85", 0, 0, "aSome user defined info", "A user defined date" },
		{ 0, "Joe",			"Jones", "Vendor", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "2/30/85", 0, 0, "bSome user defined info", "Stuff" },
		{ 0, "Peter",		"Peterson", "Client", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "3/30/85", 0, 0, "cSome user defined info", "Wild" },
		{ 0, "Shaq",		"Smith", "Friend", "Tester", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "4/30/85", 0, 0, "dSome user defined info", "Crazy" },
		{ 0, "William",	"Smith", "Family", "QA Engineer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "5/30/85", 0, 0, "eSome user defined info", "Circut" },
		{ 0, "John",		"Smith", "Family", "Salesman", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "6/30/85", 0, 0, "fSome user defined info", "Cellar" },
		{ 0, "Jason",		"Brown", "Family", "Writer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "7/30/85", 0, 0, "gSome user defined info", "Battery" },
		{ 0, "Jaqueline",	"Olson", "Friend", "Manager", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "8/30/85", 0, 0, "hSome user defined info", "Tester" },
		{ 0, "Lisa",		"Jones", "Family", "Vice President", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "9/30/85", 0, 0, "iSome user defined info", "EPC" },
		{ 0, "Sue",			"Banks", "Client", "", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "10/30/85", 0, 0, "jSome user defined info", "Stepper" },
		{ 0, "Susan",		"Mohler", "Family", "", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "11/30/85", 0, 0, "kSome user defined info", "Chess" },
		{ 0, "Susie",		"Young", "Vendor", "Queen", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "12/30/85", 0, 0, "lSome user defined info", "Pawn" },
		{ 0, "Jessica",	"Cooper", "Friend", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "13/30/85", 0, 0, "mSome user defined info", "Bishop" },
		{ 0, "Bob",			"Johnson", "Client", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "14/30/85", 0, 0, "nSome user defined info", "Knight" },
		{ 0, "Teddy",		"White", "", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "15/30/85", 0, 0, "oSome user defined info", "Queen" },
		{ 0, "Derek",		"Black", "Vendor", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "16/30/85", 0, 0, "pSome user defined info", "Rook" },
		{ 0, "Franz",		"Green", "Friend", "King", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "17/30/85", 0, 0, "qSome user defined info", "King" },
		{ 0, "Herbert",	"Wilkens", "Client", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "18/30/85", 0, 0, "rSome user defined info", "Elephant" },
		{ 0, "Michael",	"True", "Family", "Worker bee", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "19/30/85", 0, 0, "sSome user defined info", "Donkey" },
		{ 0, "Chris",		"Ungar", "Vendor", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "20/30/85", 0, 0, "tSome user defined info", "Turtle" },
		{ 0, "Jackie",		"Pryor", "Friend", "Drone", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "11/30/85", 0, 0, "uSome user defined info", "Bald Eagle" },
		{ 0, "Turk",		"West", "", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "12/30/85", 0, 0, "vSome user defined info", "Golden Eagle" },
		{ 0, "Manfred",	"Jackson", "Family", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "13/30/85", 0, 0, "wSome user defined info", "Turkey Vulture" },
		{ 0, "Xerxes",		"Shank", "Vendor", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "14/30/85", 0, 0, "xSome user defined info", "Parakeet" },
		{ 0, "Scott",		"Dylan", "Friend", "", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "15/30/85", 0, 0, "ySome user defined info", "Knee" },
		{ 0, "Monty",		"Willbury", "Client", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "16/30/85", 0, 0, "zSome user defined info", "Leg" },
		{ 0, "Wallace",	"Hanson", "Family", "", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "17/30/85", 0, 0, "aSome user defined info", "Arm" },
		{ 0, "Zeus",		"Hanson", "Vendor", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "18/30/85", 0, 0, "bSome user defined info", "Elbow" },
		{ 0, "Thor",		"Kline", "Friend", "Artillary Gunner 1st Class", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "19/30/85", 0, 0, "cSome user defined info", "" },
		{ 0, "Athena",		"Johnson", "Client", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "10/30/85", 0, 0, "dSome user defined info", "" },
		{ 0, "Aphrodite",	"Smith", "Family", "", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "11/30/85", 0, 0, "eSome user defined info", "" },
		{ 0, "Andy",		"Jones", "Vendor", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "12/30/85", 0, 0, "fSome user defined info", "" },
		{ 0, "Caitlin",	"Ungar", "Friend", "", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "13/30/85", 0, 0, "gSome user defined info", "" },
		{ 0, "Kurt",		"Brown", "Family", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "14/30/85", 0, 0, "hSome user defined info", "" },
		{ 0, "Larry",		"Green", "", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "15/30/85", 0, 0, "iSome user defined info", "" }
	};

	#define NUMBER_OF_RECORDS	(sizeof(Record)/sizeof(Record[0]))

#if 0
	static CRecord far OtherRecords[] =
	{
		{ 0, "Fred", "Fischer", "Family", "Programmer", "MicroLogic Software, Inc.", "1351 Ocean Avenue", "", "Emeryville", "CA", "94608-1128", "USA", "510-652-5464", "510-450-6511", "ffischer@aol.com", "1/28/59", "11/30/85", 0, 0, "Some user defined info", "A user defined date" }
	};

	#define NUMBER_OF_OTHER_RECORDS	(sizeof(OtherRecords)/sizeof(OtherRecords[0]))
#endif

	StorageFile Device("Address.pbk");
	Device.initialize();
	CFlatFileDatabase Database(&Device);
	CFlatFileHeaderRecord::CCreationData CreationData(szStandardDatabaseDescription, strlen(szStandardDatabaseDescription)+1, StandardDatabaseFieldDescriptions, StandardDatabaseIndexDescriptions);
	Database.initialize(0, &CreationData);

	for (int i = 0; i < NUMBER_OF_RECORDS; i++)
	{
		DB_RECORD_NUMBER dwRecordNumber;
		ERRORCODE error;
		if ((error = Database.GetCounter(&(Record[i].dwID))) != ERRORCODE_None)
		{
			TRACE2("Can't get counter for record #%d (error %d)\n", i, error);
			break;
		}
		if ((error = Database.InsertRecord((LPBYTE)(&Record[i]), &dwRecordNumber)) != ERRORCODE_None)
		{
			TRACE2("Can't insert record #%d (error %d)\n", i, error);
			break;
		}
	}

	DWORD dwFieldCount;
	if (Database.GetFieldDescriptionCount(&dwFieldCount) == ERRORCODE_None)
	{
		TRACE1("%ld Fields:\n", dwFieldCount);
		CFlatFileFieldDescription* pField = NULL;
		if (Database.GetFieldDescriptions(&pField) == ERRORCODE_None)
		{
			while (pField->m_pszName != NULL)
			{
				TRACE("Field Name |%s| Type |%ld| Data |%ld| Size |%d|\n", pField->m_pszName, pField->m_dwType, pField->m_dwUserData, pField->m_dwSize);
				pField++;
			}
		}

		DWORD dwNumber;
		if (Database.GetFieldNumberFromName("City/Town", &dwNumber) == ERRORCODE_None)
		{
			TRACE1("City is field #%d\n", dwNumber);
		}
	}

	DWORD dwIndexCount;
	if (Database.GetIndexDescriptionCount(&dwIndexCount) == ERRORCODE_None)
	{
		TRACE1("%ld Indicies:\n", dwIndexCount);
		CFlatFileIndexDescription* pIndex = NULL;
		if (Database.GetIndexDescriptions(&pIndex) == ERRORCODE_None)
		{
			while (pIndex->m_pszName != NULL)
			{
				TRACE("Index Name |%s| Flags |%ld| Data |%ld| Fields |%s|\n", pIndex->m_pszName, pIndex->m_dwFlags, pIndex->m_dwUserData, pIndex->m_pszField);
				pIndex++;
			}
		}

		DWORD dwNumber;
		if (Database.GetIndexNumberFromName("Last Name", &dwNumber) == ERRORCODE_None)
		{
			TRACE1("Last Name is index #%d\n", dwNumber);
		}
	}

	DWORD dwIndex = 0;
	DWORD dwRecordCount;
	if (Database.GetRecordCount(dwIndex, &dwRecordCount) == ERRORCODE_None)
	{
		TRACE2("%ld records in index #%ld:\n", dwRecordCount, dwIndex);

		for (DWORD dwSequence = 0; dwSequence < dwRecordCount; dwSequence++)
		{
			CRecord Record;
			DWORD dwRecordNumber;
			ERRORCODE error;
			if ((error = Database.GetRecord(dwIndex, dwSequence, &dwRecordNumber, (LPBYTE)&Record)) == ERRORCODE_None)
			{
				TRACE("  #%ld: (%ld) |%ld| |%s| |%s|\n",
								dwSequence,
								dwRecordNumber,
								Record.dwID,
								Record.szFirstName,
								Record.szLastName);
			}
			else
			{
				TRACE2("  #%ld: error %d\n", dwSequence, error);
			}
		}
	}
#if 0
	DWORD dwDeleteNumber = 17;
	if (Database.DeleteRecord(dwIndex, dwDeleteNumber) == ERRORCODE_None)
	{
		TRACE2("Deleted record %ld using index %ld\n", dwDeleteNumber, dwIndex);
	}
#endif
	if (Database.GetRecordCount(dwIndex, &dwRecordCount) == ERRORCODE_None)
	{
		TRACE2("%ld records in index #%ld:\n", dwRecordCount, dwIndex);

		for (DWORD dwSequence = 0; dwSequence < dwRecordCount; dwSequence++)
		{
			CRecord Record;
			DWORD dwRecordNumber;
			ERRORCODE error;
			if ((error = Database.GetRecord(dwIndex, dwSequence, &dwRecordNumber, (LPBYTE)&Record)) == ERRORCODE_None)
			{
				TRACE("  #%ld: (%ld) |%ld| |%s| |%s|\n",
								dwSequence,
								dwRecordNumber,
								Record.dwID,
								Record.szFirstName,
								Record.szLastName);
			}
			else
			{
				TRACE2("  #%ld: error %d\n", dwSequence, error);
			}
		}
	}

	TRY
	{
		CFlatFileDatabaseCursor Cursor;
		Cursor.Attach(&Database);

		dwFieldCount = Cursor.GetFieldCount();

		TRACE1("%ld Field Objects:\n", dwFieldCount);

		CFlatFileDatabaseField** pFields = Cursor.GetFields();

		for (DWORD dwField = 0; dwField < dwFieldCount; dwField++)
		{
			TRACE("Field Name |%s| Type |%ld| Size |%d|\n", pFields[dwField]->GetName(), pFields[dwField]->GetType(), pFields[dwField]->GetSize());
		}

		TRACE0("---- Sort forward by Last Name ----\n");
		Cursor.SetIndex("Last Name");
		Cursor.First();
		while (!Cursor.AtEOF())
		{
			TRACE("Record #%ld: |%s| |%s| |%s|\n",
				Cursor.GetPosition(),
				Cursor.GetField("__ID__")->GetAsString(),
				Cursor.GetField("First Name")->GetAsString(),
				Cursor.GetField("Last Name")->GetAsString());
			Cursor.Next();
		}

		TRACE0("---- Sort backward by First Name ----\n");
		Cursor.SetIndex("First Name");
		CFlatFileDatabaseField* pAgeField = Cursor.GetField("First Name");
		CFlatFileDatabaseField* pIdField = Cursor.GetField("__ID__");
		Cursor.Last();
		while (!Cursor.AtBOF())
		{
			Cursor.Previous();
			TRACE("Record #%ld: |%s| |%s| |%s|\n",
				Cursor.GetPosition(),
				pIdField->GetAsString(),
				Cursor.GetField("First Name")->GetAsString(),
				Cursor.GetField("Last Name")->GetAsString());
		}

#if 0
		TRACE0("---- Find All Candies ----\n");
		Cursor.SetIndex("First Name");
		CFlatFileDatabaseField* pFirstNameField = Cursor.GetField("First Name");
		Cursor.New();
		pFirstNameField->SetAsString("Candy");
		Cursor.Find();
		while (!Cursor.AtEOF() && (stricmp(pFirstNameField->GetAsString(), "Candy") == 0))
		{
			TRACE("Record #%ld: |%s| |%s| |%s| |%s| |%s|\n",
				Cursor.GetPosition(),
				Cursor.GetField("Balance")->GetAsString(),
				Cursor.GetField("Zip")->GetAsString(),
				Cursor.GetField("First Name")->GetAsString(),
				Cursor.GetField("Last Name")->GetAsString(),
				Cursor.GetField("Age")->GetAsString());
			Cursor.Next();
		}

		TRACE0("---- Delete all Candies ----\n");
		Cursor.New();
		*pFirstNameField = "Candy";
		Cursor.Find();
		while (!Cursor.AtEOF() && (stricmp(pFirstNameField->GetAsString(), "Candy") == 0))
		{
			TRACE("Delete Record #%ld: |%s| |%s| |%s| |%s| |%s|\n",
				Cursor.GetPosition(),
				Cursor.GetField("Balance")->GetAsString(),
				Cursor.GetField("Zip")->GetAsString(),
				Cursor.GetField("First Name")->GetAsString(),
				Cursor.GetField("Last Name")->GetAsString(),
				Cursor.GetField("Age")->GetAsString());
			Cursor.Delete();
		}

		TRACE0("---- After deletion of Candies ----\n");
		Cursor.SetIndex("First Name");
		Cursor.First();
		while (!Cursor.AtEOF())
		{
			TRACE("Record #%ld: |%s| |%s| |%s| |%s| |%s|\n",
				Cursor.GetPosition(),
				(LPCSTR)(*pFirstNameField),
				Cursor.GetField("Last Name")->GetAsString(),
				Cursor.GetField("Balance")->GetAsString(),
				(LPCSTR)(*pIdField),
				(LPCSTR)(*pAgeField));
			Cursor.Next();
		}

		TRACE0("---- Change all negative balances to 0 ----\n");
		CFlatFileDatabaseField* pBalanceField = Cursor.GetField("Balance");
		CFlatFileDatabaseField* pLastNameField = Cursor.GetField("Last Name");
		Cursor.SetIndex("Balance");
		Cursor.First();
		while (!Cursor.AtEOF() && ((long)(*pBalanceField) < 0))
		{
			*pBalanceField = 0;
			Cursor.Update();
			Cursor.First();
		}

		TRACE0("---- After changing negative balances to 0 ----\n");
		Cursor.SetIndex("Balance");
		Cursor.First();
		while (!Cursor.AtEOF())
		{
			TRACE("Record #%ld: |%s| |%s| |%s| |%s| |%s|\n",
				Cursor.GetPosition(),
				(LPCSTR)(*pBalanceField),
				(LPCSTR)(*pFirstNameField),
				(LPCSTR)(*pLastNameField),
				(LPCSTR)(*pIdField),
				(LPCSTR)(*pAgeField));
			Cursor.Next();
		}

		TRACE0("---- Adding other records ----\n");
		Cursor.SetIndex("First Name");
		Cursor.First();

		for (i = 0; i < NUMBER_OF_OTHER_RECORDS; i++)
		{
			Cursor = &(OtherRecords[i]);
			Cursor.SetCounters();
			Cursor.Insert();
		}

		TRACE0("---- After adding other records ----\n");
		Cursor.First();
		while (!Cursor.AtEOF())
		{
			TRACE("Record #%ld: |%s| |%s| |%s| |%s| |%s|\n",
				Cursor.GetPosition(),
				(LPCSTR)(*pBalanceField),
				(LPCSTR)(*pFirstNameField),
				(LPCSTR)(*pLastNameField),
				(LPCSTR)(*pIdField),
				(LPCSTR)(*pAgeField));
			Cursor.Next();
		}
#endif

		LPCSTR pUserData;
		DWORD dwLength;
		pUserData = NULL;
		Cursor.GetUserData((LPVOID*)(&pUserData), &dwLength);
		TRACE("---- User Data = |%s| (%lu bytes) ----\n", pUserData, dwLength);
		CFlatFileDatabase::FreeMemory((FLATFILE_DATABASE_MEMORY_PTR)pUserData);

#if 0
		Cursor.SetUserData("This is new data", 17);
		pUserData = NULL;
		Cursor.GetUserData((LPVOID*)(&pUserData), &dwLength);
		TRACE("---- New User Data = |%s| (%lu bytes) ----\n", pUserData, dwLength);
		CFlatFileDatabase::FreeMemory((FLATFILE_DATABASE_MEMORY_PTR)pUserData);
#endif

		Cursor.Detach();
	}
	CATCH(CErrorcodeException, e)
	{
		TRACE1("Got error %d\n", e->m_error);
	}
	AND_CATCH_ALL(e)
	{
		TRACE0("Got an unknown error.\n");
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	Database.flush();
	Database.write_state(TRUE);
	Device.flush();
#endif

	return error;
}
