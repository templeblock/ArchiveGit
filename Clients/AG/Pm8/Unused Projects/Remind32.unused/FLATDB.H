/*
// $Workfile: FLATDB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/FLATDB.H $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:50   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:20   Fred
// Initial revision.
// 
//    Rev 1.11   17 Jul 1996 12:56:22   Jay
// 
//    Rev 1.10   06 Jul 1996 12:20:30   Fred
// Address book improvements
// 
//    Rev 1.9   01 Jul 1996 07:32:02   Fred
// Receiver list
// 
//    Rev 1.8   18 Jun 1996 15:20:50   Fred
// Field value list management and editing
// 
//    Rev 1.7   18 Jun 1996 08:28:38   Fred
// More adress book changes
// 
//    Rev 1.6   10 Jun 1996 08:15:04   Fred
// Added field and index user data; modified flags.
// 
//    Rev 1.5   06 May 1996 15:08:26   Fred
// New Names
// 
//    Rev 1.4   01 May 1996 17:09:38   Fred
// Database may be not sharable
// 
//    Rev 1.3   01 May 1996 14:34:52   Fred
// More flat file database classes
// 
//    Rev 1.2   25 Apr 1996 11:21:30   Fred
// CFlatFileDatabase works
// 
//    Rev 1.1   23 Apr 1996 16:21:44   Fred
// Compiles
// 
//    Rev 1.0   23 Apr 1996 11:09:08   FRED
// Initial revision.
*/

class CFlatFileFieldDescription;
class CFlatFileIndexDescription;
class CFlatFileRecordNumberRecord;
class CFlatFileCounterRecord;
class CFlatFileDataRecord;
class CFlatFileHeaderRecord;
class CFlatFileIndexArrayElement;
class CFlatFileIndexRecord;
class CFlatFileDatabase;
class CFlatFileDatabaseRecordBuffer;
class CFlatFileDatabaseField;
class CFlatFileDatabaseFieldString;
class CFlatFileDatabaseFieldBlob;
class CFlatFileDatabaseFieldCounter32;
class CFlatFileDatabaseFieldSigned32;
class CFlatFileDatabaseFieldUnsigned32;
class CFlatFileDatabaseCursor;

#ifndef __FLATDB_H__
#define __FLATDB_H__

#include "database.h"
#include "file.h"
#include "arrayrec.h"
#include "sortarry.h"

#pragma warning(disable:4200)	// zero-size array in struct/union

//////////////////////////////////////////////////////////////////////////
// Types available at top level to flat file database users.

#ifdef WIN32
typedef void* FLATFILE_DATABASE_MEMORY_PTR;
typedef const void* FLATFILE_DATABASE_MEMORY_CONST_PTR;
#else
typedef void __huge * FLATFILE_DATABASE_MEMORY_PTR;
typedef const void __huge * FLATFILE_DATABASE_MEMORY_CONST_PTR;
#endif

// Find flags
enum FLATFILE_FIND_FLAGS
{
	FFF_GreaterThan	=	0x0001			// perform a GT instead of a GE search
};

// Record types for a CFlatFileDatabase
enum FLATFILE_RECORD_TYPE
{
	FRT_Header = 1,
	FRT_RecordNumber,
	FRT_Counter,
	FRT_Data,
	FRT_Index,
	FRT_IndexBlock		// must be index+1
};

// Field types for CFlatFileDatabase
enum FLATFILE_FIELD_TYPE
{
	FFT_String			=	1,		// Null terminated string
	FFT_Blob,						// Binary blob data (2^32-1 max byte length)
	FFT_Counter32,					// Unsigned 32 bit integer which is auto-incremented at record creation
	FFT_Signed32,					// Signed 32 bit integer
	FFT_Unsigned32					// Unsigned 32 bit integer
};

// Field description
class CFlatFileFieldDescription
{
public:
	LPCSTR m_pszName;			// Field Name
	DWORD m_dwType;			// Field Type
	DWORD m_dwUserData;		// Field User Data
	DWORD m_dwSize;			// Field Size

public:
	LPCSTR GetName(void) const
		{ return m_pszName; }
	DWORD GetType(void) const
		{ return m_dwType; }
	DWORD GetUserData(void) const
		{ return m_dwUserData; }
	DWORD GetSize(void) const
		{ return m_dwSize; }
};

// Index description
class CFlatFileIndexDescription
{
public:
	LPCSTR m_pszName;			// Index Name
	DWORD m_dwFlags;			// Index Flags
	DWORD m_dwUserData;		// Index User Data
	LPCSTR m_pszField;		// Field Names (separated by '|')

public:
	LPCSTR GetName(void) const
		{ return m_pszName; }
	DWORD GetFlags(void) const
		{ return m_dwFlags; }
	DWORD GetUserData(void) const
		{ return m_dwUserData; }
	LPCSTR GetField(void) const
		{ return m_pszField; }
};

// Index flags
enum FLATFILE_INDEX_FLAGS
{
	RIF_CaseSensitive	=	0x0001			// compare string fields as case sensitive
};

//////////////////////////////////////////////////////////////////////////
// Record number record.

class CFlatFileRecordNumberRecord : public DatabaseRecord
{
protected:
	// Fixed size data
	struct
	{
		DB_RECORD_NUMBER m_dwRecordNumber;	// record number of user data record (0 if none)
	} m_Record;

public:
	// Get the record number.
	DB_RECORD_NUMBER GetRecordNumber(void) const
		{ return m_Record.m_dwRecordNumber; }

	// Set the record number.
	void SetRecordNumber(DWORD dwRecordNumber)
		{ m_Record.m_dwRecordNumber = dwRecordNumber; modified(); }

	// Clear the record number.
	void ClearRecordNumber(void)
		{ SetRecordNumber(0); }

protected:
	// The constructor for this record.
	//	This is private. Use the create method.
	CFlatFileRecordNumberRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);
	virtual ~CFlatFileRecordNumberRecord();

	// Assign method.
	virtual ERRORCODE assign(DatabaseRecordRef record);

public:
	// The creator for this type of record.
	// Note that this is static so that we can register it with the database.
	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

	// Destroy this record.
	// The record is being removed from the database.
	// Any and all sub-records must be removed from the database as well.
	// This should not FREE anything (in the memory sense); that's the job of the
	// destructor.
	virtual void destroy(void);

	/***************************************************/
	/* Methods used exclusively by the StorageManager. */
	/***************************************************/

	// ReadData()
	// Read the record.
	ERRORCODE ReadData(StorageDevicePtr pDevice);

	// WriteData()
	// Write the record.
	ERRORCODE WriteData(StorageDevicePtr pDevice);

	// SizeofData()
	// Return the size of the record.
	ST_MAN_SIZE SizeofData(StorageDevicePtr pDevice);

	// RelocateData()
	// This is a function called by the storage manager when our storage is
	// ever relocated. This allows us to move any data we want saved.
	ERRORCODE RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position);
};

//////////////////////////////////////////////////////////////////////////
// Counter record

class CFlatFileCounterRecord : public DatabaseRecord
{
protected:
	// Fixed size data
	struct
	{
		DWORD m_dwCount;	// current count (0 to 2^32-1)
	} m_Record;

public:
	// Return current count and increment.
	DWORD Increment(void)
		{ modified(); return (m_Record.m_dwCount)++; }

	// Set the current count.
	void SetCount(DWORD dwCount)
		{ modified(); m_Record.m_dwCount = dwCount; }

protected:
	// The constructor for this record.
	//	This is private. Use the create method.
	CFlatFileCounterRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);
	virtual ~CFlatFileCounterRecord();

	// Assign method.
	virtual ERRORCODE assign(DatabaseRecordRef record);

public:
	// The creator for this type of record.
	// Note that this is static so that we can register it with the database.
	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

	// Destroy this record.
	// The record is being removed from the database.
	// Any and all sub-records must be removed from the database as well.
	// This should not FREE anything (in the memory sense); that's the job of the
	// destructor.
	virtual void destroy(void);

	/***************************************************/
	/* Methods used exclusively by the StorageManager. */
	/***************************************************/

	// ReadData()
	// Read the record.
	ERRORCODE ReadData(StorageDevicePtr pDevice);

	// WriteData()
	// Write the record.
	ERRORCODE WriteData(StorageDevicePtr pDevice);

	// SizeofData()
	// Return the size of the record.
	ST_MAN_SIZE SizeofData(StorageDevicePtr pDevice);

	// RelocateData()
	// This is a function called by the storage manager when our storage is
	// ever relocated. This allows us to move any data we want saved.
	ERRORCODE RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position);
};

//////////////////////////////////////////////////////////////////////////
// Data record

class CFlatFileDataRecord : public DatabaseRecord
{
public:
	// Creation data
	class CCreationData
	{
	public:
		// Record types.
		enum
		{
			Normal	=	1,
			Blob		=	2
		} CRecordType;
	public:
		// The user can pass a storage device and a type.
		CCreationData(StorageDevice* pStorageDevice, DWORD dwType = (DWORD)Normal);
	protected:
		StorageDevice* m_pStorageDevice;
		DWORD m_dwType;
	public:
		StorageDevice* GetStorageDevice(void) const
			{ return m_pStorageDevice; }
		DWORD GetType(void) const
			{ return m_dwType; }
	};

protected:
	// Fixed size data
	struct
	{
		DWORD m_dwType;			// type of record
		DWORD m_dwSize;			// size of data that follows (< 2^32-1)
	} m_Record;

	// Variable size data follows

	ST_MAN_SIZE m_Position;	// position of our data in the device

	// GetDataFile completion routine.
	static void FlatFileDataRecordReadOnlyCompletion(StorageFilePtr file, LPVOID data);

public:
	// Return data type.
	DWORD GetType(void) const
		{ return m_Record.m_dwType; }

	// Return data size.
	DWORD GetSize(void) const
		{ return m_Record.m_dwSize; }

	// Return the data position.
	ST_MAN_SIZE GetPosition(void) const
		{ return m_Position; }

protected:
	// The constructor for this record.
	//	This is private. Use the create method.
	CFlatFileDataRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);
	virtual ~CFlatFileDataRecord();

	// Assign method.
	virtual ERRORCODE assign(DatabaseRecordRef record);

	// Write the variable length portion of the data. This method is
	// called when the the record is created. The data to write is
	// specified by the creation data passed to create().
	ERRORCODE WriteCreationData(CCreationData* pCreationData);

public:
	// The creator for this type of record.
	// Note that this is static so that we can register it with the database.
	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

	// Destroy this record.
	// The record is being removed from the database.
	// Any and all sub-records must be removed from the database as well.
	// This should not FREE anything (in the memory sense); that's the job of the
	// destructor.
	virtual void destroy(void);

	// Read the variable size data for this record into memory. If the
	// memory pointer passed in is NULL, a block will be allocated for
	// the data. The caller must free the allocated block with the static
	// function CFlatFileDatabase::FreeMemory(). The second parameter,
	// pdwSize points to a DWORD which, if not NULL, will be filled in
	// with the data size (pdwSize of ONLY and output parameter.)
	ERRORCODE GetData(LPBYTE* ppData, DWORD* pdwSize = NULL) const;

	// Initialize a StorageFile object to access the variable size data
	// for this record. Only read-only access is supported.
	ERRORCODE GetDataAsFile(StorageFile* pFile) const;

	/***************************************************/
	/* Methods used exclusively by the StorageManager. */
	/***************************************************/

	// ReadData()
	// Read the record.
	ERRORCODE ReadData(StorageDevicePtr pDevice);

	// WriteData()
	// Write the record.
	ERRORCODE WriteData(StorageDevicePtr pDevice);

	// SizeofData()
	// Return the size of the record.
	ST_MAN_SIZE SizeofData(StorageDevicePtr pDevice);

	// RelocateData()
	// This is a function called by the storage manager when our storage is
	// ever relocated. This allows us to move any data we want saved.
	ERRORCODE RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position);
};

//////////////////////////////////////////////////////////////////////////
// Header record

class CFlatFileHeaderRecord : public DatabaseRecord
{
public:
	// Creation data
	class CCreationData
	{
	public:
		// The user can pass a block of data, a list of fields, and a list of indicies.
		CCreationData(LPVOID pUserData, DWORD dwUserDataLength, CFlatFileFieldDescription* pField, CFlatFileIndexDescription* pIndex);
	protected:
		LPVOID m_pUserData;
		DWORD m_dwUserDataLength;
		CFlatFileFieldDescription* m_pField;
		CFlatFileIndexDescription* m_pIndex;
	public:
		LPVOID GetUserData(void) const
			{ return m_pUserData; }
		DWORD GetUserDataLength(void) const
			{ return m_dwUserDataLength; }
		CFlatFileFieldDescription* GetFirstField(void) const
			{ return m_pField; }
		CFlatFileIndexDescription* GetFirstIndex(void) const
			{ return m_pIndex; }
		DWORD GetFieldIndex(LPCSTR pszFieldName, DWORD dwLength = (DWORD)-1) const;
	};

protected:
	// Fixed size data
	struct
	{
		DWORD m_dwVersion;
		DWORD m_dwSize;			// size of variable data that follows m_Record
		DWORD m_dwRecordSize;	// size of field information (record buffer size)
		DWORD m_dwFieldCount;	// number of fields
		DWORD m_dwFieldOffset;	// offset of field information in variable sized data
		DWORD m_dwIndexCount;	// number of indicies
		DWORD m_dwIndexOffset;	// offset of index information in variable sized data
	} m_Record;

	// Variable size data follows

	ST_MAN_SIZE m_Position;	// position of our data in the device

	LPBYTE m_pData;			// pointer to variable length data which has been read

public:
	// Return data size.
	DWORD GetSize(void) const
		{ return m_Record.m_dwSize; }

	// Return the record data size.
	DWORD GetRecordSize(void) const
		{ return m_Record.m_dwRecordSize; }

	// Return the number of fields.
	DWORD GetFieldCount(void) const
		{ return m_Record.m_dwFieldCount; }

	// Return offset of field information.
	DWORD* GetFieldOffsetTable(void) const
		{ ASSERT(m_pData != NULL);
		  return (DWORD*)(m_pData+m_Record.m_dwFieldOffset); }

	// Return the number of indicies.
	DWORD GetIndexCount(void) const
		{ return m_Record.m_dwIndexCount; }

	// Return offset of index information.
	DWORD* GetIndexOffsetTable(void) const
		{ ASSERT(m_pData != NULL);
		  return (DWORD*)(m_pData+m_Record.m_dwIndexOffset); }

	// Return the data position.
	ST_MAN_SIZE GetPosition(void) const
		{ return m_Position; }

	// Get a pointer to the data.
	LPBYTE GetData(void) const
		{ return m_pData; }

public:
	// Field data in header record.
	class CField
	{
	public:
		// Field data.
		DWORD m_dwType;		// type, see FLATFILE_FIELD_TYPE
		DWORD m_dwUserData;	// user data
		DWORD m_dwSize;		// total number of bytes for field in record block
		DWORD m_dwOffset;		// offset to field data from start of record block
		char m_szName[0];		// null-terminated field name

		// Return the type of the field.
		DWORD GetType(void) const
			{ return m_dwType; }

		// Return the user data of the field.
		DWORD GetUserData(void) const
			{ return m_dwUserData; }

		// Return the byte size of the field in the record.
		DWORD GetSize(void) const
			{ return m_dwSize; }

		// Return the offset of the field data in the record.
		DWORD GetOffset(void) const
			{ return m_dwOffset; }

		// Get the name of the field.
		LPCSTR GetName(void) const
			{ return &(m_szName[0]); }
	};

	// Get the offset of a given field.
	DWORD GetFieldOffset(DWORD dwField) const
		{ ASSERT(dwField < m_Record.m_dwFieldCount);
		  return (GetFieldOffsetTable())[dwField]; }

	// Get a pointer to a given field.
	CField* GetField(DWORD dwField) const
		{ return (CField*)(m_pData+GetFieldOffset(dwField)); }

	// Index data in header record.
	class CIndex
	{
	public:
		// Index description data.
		DB_RECORD_NUMBER m_dwRecordNumber;	// record number of index array
		DWORD m_dwFlags;							// flags, see FLATFILE_INDEX_FLAGS
		DWORD m_dwUserData;						// user data
		DWORD m_dwFields;							// number of fields referenced by index
		DWORD m_dwField[0];						// array of field numbers

		// null-terminated index name follows field number array

		// Get the index flags.
		DWORD GetFlags(void) const
			{ return m_dwFlags; }

		// Check if the index is case sensitive.
		BOOL IsCaseSensitive(void) const
			{ return (m_dwFlags & (DWORD)RIF_CaseSensitive) != 0; }

		// Return the index user data.
		DWORD GetUserData(void) const
			{ return m_dwUserData; }

		// Get the number of fields referenced by the index.
		DWORD GetFieldCount(void) const
			{ return m_dwFields; }

		// Get the number of a given field.
		DWORD GetFieldNumber(DWORD dwField) const
			{ ASSERT(dwField < m_dwFields); return m_dwField[dwField]; }

		// Get the name of the index.
		LPCSTR GetName(void) const
			{ return ((LPCSTR)(&(m_dwField[0])))+m_dwFields*sizeof(m_dwField[0]); }
	};

	// Get the offset of a given index.
	DWORD GetIndexOffset(DWORD dwIndex) const
		{ ASSERT(dwIndex < m_Record.m_dwIndexCount);
		  return (GetIndexOffsetTable())[dwIndex]; }

	// Get a pointer to a given index.
	CIndex* GetIndex(DWORD dwIndex) const
		{ return (CIndex*)(m_pData+GetIndexOffset(dwIndex)); }

protected:
	// The constructor for this record.
	//	This is private. Use the create method.
	CFlatFileHeaderRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);
	virtual ~CFlatFileHeaderRecord();

	// Assign method.
	virtual ERRORCODE assign(DatabaseRecordRef record);

	// Write the variable length portion of the data. This method is
	// called when the the record is created. The data to write is
	// specified by the creation data passed to create().
	ERRORCODE WriteCreationData(CCreationData* pCreationData);

	// Free any dynamically allocated data.
	void FreeData(void);

	// Helper methods for computing the size of the field and index information
	// in the header block for the definitions passed in the creation data.
	ERRORCODE SizeFieldInformation(CCreationData* pCreationData, DWORD* pdwSize, DWORD* pdwCount);
	ERRORCODE SizeIndexInformation(CCreationData* pCreationData, DWORD* pdwSize, DWORD* pdwCount);

	// Helper methods for building the field and index information
	// in the header block for the definitions passed in the creation data.
	ERRORCODE BuildFieldInformation(CCreationData* pCreationData, LPBYTE pData, DWORD dwCount);
	ERRORCODE BuildIndexInformation(CCreationData* pCreationData, LPBYTE pData, DWORD dwCount);

	// Helper method for parsing a field names string.
	ERRORCODE NextFieldName(LPCSTR* ppszFieldNames, LPCSTR* ppszFieldName, DWORD* pdwLength);

	// Helper methods for sizing field and index description data for current header record.
	ERRORCODE SizeFieldDescriptions(DWORD* pdwSize);
	ERRORCODE SizeIndexDescriptions(DWORD* pdwSize);

public:
	// The creator for this type of record.
	// Note that this is static so that we can register it with the database.
	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

	// Destroy this record.
	// The record is being removed from the database.
	// Any and all sub-records must be removed from the database as well.
	// This should not FREE anything (in the memory sense); that's the job of the
	// destructor.
	virtual void destroy(void);

	/***************************************************/
	/* Methods used exclusively by the StorageManager. */
	/***************************************************/

	// ReadData()
	// Read the record.
	ERRORCODE ReadData(StorageDevicePtr pDevice);

	// WriteData()
	// Write the record.
	ERRORCODE WriteData(StorageDevicePtr pDevice);

	// SizeofData()
	// Return the size of the record.
	ST_MAN_SIZE SizeofData(StorageDevicePtr pDevice);

	// RelocateData()
	// This is a function called by the storage manager when our storage is
	// ever relocated. This allows us to move any data we want saved.
	ERRORCODE RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position);

	// Build the field and index description lists.
	ERRORCODE BuildFieldDescriptions(CFlatFileFieldDescription* pDescription, DWORD* pdwSize = NULL);
	ERRORCODE BuildIndexDescriptions(CFlatFileIndexDescription* pDescription, DWORD* pdwSize = NULL);

	// Useful type for an array of pointers to CFlatFileIndexRecord's.
	typedef CFlatFileIndexRecord** CFlatFileIndexRecordPointerArray;

	// Get all of the index records.
	ERRORCODE GetIndexRecords(CFlatFileIndexRecordPointerArray* pIndexArray, DWORD* pdwCount, BOOL fReadOnly);

	// Release all of the index records.
	ERRORCODE ReleaseIndexRecords(CFlatFileIndexRecordPointerArray IndexArray, DWORD dwCount, BOOL fReadOnly);

	// Insert a record into all the indicies.
	ERRORCODE InsertRecord(DB_RECORD_NUMBER dwNumber, LPBYTE pData);

	// Delete a record from all the indicies.
	ERRORCODE DeleteRecord(DB_RECORD_NUMBER dwNumber);
};

//////////////////////////////////////////////////////////////////////////
// Index record and supporting classes

// CFlatFileIndexArrayElement - an element in the index array

class CFlatFileIndexArrayElement
{
public:
	DB_RECORD_NUMBER	m_dwRecordNumber;	// record number for database record
};

// CFlatFileIndexRecord - an array of CIndexArrayElement structures which
// define a particular record ordering.

class CFlatFileIndexRecord : public CArrayRecord
{
protected:
	// The constructor for this record.
	// This is private. Use the create method.
	CFlatFileIndexRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);
	virtual ~CFlatFileIndexRecord();

public:
	// The creator for this type of record.
	// Note that this is static so that we can register it with the database.
	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

	// Insert a record into an index.
	ERRORCODE InsertRecord(CFlatFileHeaderRecord* pHeader, CFlatFileHeaderRecord::CIndex* pIndex, DB_RECORD_NUMBER dwNumber, LPBYTE pData);

	// Find the array index of a record given its data.
	ERRORCODE FindRecord(CFlatFileHeaderRecord* pHeader, CFlatFileHeaderRecord::CIndex* pIndex, LPBYTE pData, DWORD* pdwPosition, BOOL* pfExact = NULL, DWORD dwFlags = 0);

	// Compare the indexed fields of two records.
	ERRORCODE CompareFields(CFlatFileHeaderRecord* pHeader, CFlatFileHeaderRecord::CIndex* pIndex, LPBYTE pNewData, LPBYTE pCurrentData, int* pnResult);

	// Compare a field of two records.
	ERRORCODE CompareField(CFlatFileHeaderRecord* pHeader, CFlatFileHeaderRecord::CIndex* pIndex, CFlatFileHeaderRecord::CField* pField, LPBYTE pNewFieldData, LPBYTE pCurrentFieldData, int* pnResult);

	// Delete a record from an index.
	ERRORCODE DeleteRecord(DB_RECORD_NUMBER dwNumber);

	// Convert a record number to a sequence number.
	ERRORCODE GetSequenceNumber(DB_RECORD_NUMBER dwNumber, DWORD* pdwSequenceNumber);
};

//////////////////////////////////////////////////////////////////////////
// Flat file database

class CFlatFileDatabase : public Database
{
private:
	ERRORCODE m_last_error;
	ERRORCODE CreateUserDataRecordNumber(void);
	ERRORCODE CreateCounter(void);
	ERRORCODE CreateHeader(CFlatFileHeaderRecord::CCreationData* pCreationData);
	ERRORCODE CreateUserData(CFlatFileHeaderRecord::CCreationData* pCreationData);

#ifdef DATABASE_STATISTICS
	long m_lGetDatabaseRecord;
	long m_lGetDatabaseRecordRO;
#endif

public:
	// Initialize the database. The start of the database in the storage
	// device and the creation data for the header record are passed as
	// parameters.
	virtual ERRORCODE initialize(ST_DEV_POSITION position, CFlatFileHeaderRecord::CCreationData* pCreationData);

	// reinitialize()
	virtual ERRORCODE reinitialize(CFlatFileHeaderRecord::CCreationData* pCreationData);

	// Create a record number record.
	CFlatFileRecordNumberRecord* CreateRecordNumberRecord(void* creation_data = NULL)
		{ return (CFlatFileRecordNumberRecord*)new_record(FRT_RecordNumber, creation_data); }

	// Create a counter record.
	CFlatFileCounterRecord* CreateCounterRecord(void* creation_data = NULL)
		{ return (CFlatFileCounterRecord*)new_record(FRT_Counter, creation_data); }

	// Create a header record.
	CFlatFileHeaderRecord* CreateHeaderRecord(void* creation_data = NULL)
		{ return (CFlatFileHeaderRecord*)new_record(FRT_Header, creation_data); }

	// Create a data record.
	CFlatFileDataRecord* CreateDataRecord(void* creation_data = NULL)
		{ return (CFlatFileDataRecord*)new_record(FRT_Data, creation_data); }

	// Create an index record.
	CFlatFileIndexRecord* CreateIndexRecord(void* creation_data = NULL)
		{ return (CFlatFileIndexRecord*)new_record(FRT_Index, creation_data); }

public:
	// Create a flat file database.
	CFlatFileDatabase(StorageDevice* pDevice, BOOL fSharable = TRUE);

	// Destory a flat file database.
	virtual ~CFlatFileDatabase();

	// Insert a record into the database.
	ERRORCODE InsertRecord(LPBYTE pData, DB_RECORD_NUMBER* pdwNumber);

	// Insert a blob into the database. Return the record number.
	ERRORCODE InsertBlob(StorageDevice* pDevice, DB_RECORD_NUMBER* pdwNumber);

	// Delete a record from the database.
	ERRORCODE DeleteRecord(DWORD dwNumber);

	// Get the number of records in a database.
	ERRORCODE GetRecordCount(DWORD dwIndex, DWORD* pdwCount);

	// Get the size of a record in the database (total size to hold all fields.)
	ERRORCODE GetRecordSize(DWORD* pdwSize);

	// Get a record's number and/or data.
	ERRORCODE GetRecord(DWORD dwIndex, DWORD dwSequence, DWORD* pdwNumber, LPBYTE pData);

	// Delete a record.
	ERRORCODE DeleteRecord(DWORD dwIndex, DWORD dwSequence);

	// Delete all data records.
	ERRORCODE DeleteAllRecords(void);

	// Find a record.
	ERRORCODE FindRecord(DWORD dwIndex, LPBYTE pData, DWORD* pdwPosition, BOOL* pfExact = NULL, DWORD dwFlags = 0);

	// Convert a record number to a sequence number.
	ERRORCODE GetSequenceNumber(DWORD dwIndex, DB_RECORD_NUMBER dwNumber, DWORD* pdwSequenceNumber);

	// Get information about the database field descriptions.
	ERRORCODE GetFieldDescriptions(CFlatFileFieldDescription** ppDescriptions = NULL);
	ERRORCODE GetFieldDescriptionCount(DWORD* pdwCount);
	ERRORCODE GetFieldDescription(CFlatFileFieldDescription** ppDescription, DWORD dwNumber);
	ERRORCODE GetFieldNumberFromName(LPCSTR pszName, DWORD* pdwNumber);

	// Get information about the database index descriptions.
	ERRORCODE GetIndexDescriptions(CFlatFileIndexDescription** ppDescriptions = NULL);
	ERRORCODE GetIndexDescriptionCount(DWORD* pdwCount);
	ERRORCODE GetIndexDescription(CFlatFileIndexDescription** ppDescription, DWORD dwNumber);
	ERRORCODE GetIndexNumberFromName(LPCSTR pszName, DWORD* pdwNumber);
	ERRORCODE GetIndexFieldCount(DWORD* pdwCount, DWORD dwNumber);
	ERRORCODE GetIndexFieldNumber(DWORD* pdwFieldNumber, DWORD dwNumber, DWORD dwField);

	// Get main field information.
	ERRORCODE GetMainFieldOfIndex(DWORD* pdwFieldNumber, DWORD dwIndexNumber);
	ERRORCODE GetIndexOfMainField(DWORD* pdwIndexNumber, DWORD dwFieldNumber);

	// Get records from the database.
	ERRORCODE GetDatabaseRecord(DatabaseRecord** ppRecord, DB_RECORD_NUMBER dwNumber, DB_RECORD_TYPE nType, BOOL fReadOnly = FALSE);
	ERRORCODE GetHeaderRecord(CFlatFileHeaderRecord** ppHeader, BOOL fReadOnly = TRUE);
	ERRORCODE GetCounterRecord(CFlatFileCounterRecord** ppCounter);
	ERRORCODE GetUserDataRecordNumberRecord(CFlatFileRecordNumberRecord** ppUserDataRecordNumber, BOOL fReadOnly = TRUE);

	// Get and increment the current count.
	ERRORCODE GetCounter(DWORD* pdwCount);

	// Set the current count to a particular value.
	ERRORCODE SetCounter(DWORD dwCount);

	// Get the current user data.
	ERRORCODE GetUserData(LPVOID* ppData, DWORD* pdwSize);
	ERRORCODE GetUserDataAsFile(StorageFile* pFile);

	// Set the current user data.
	ERRORCODE SetUserData(LPVOID pData, DWORD dwSize);
	ERRORCODE SetUserData(StorageDevice* pDevice);

	// Delete the current user data.
	ERRORCODE DeleteUserData(void)
		{ return SetUserData(NULL); }

	// Get blob data.
	ERRORCODE GetBlobData(LPVOID* ppData, DB_RECORD_NUMBER dwNumber);
	ERRORCODE GetBlobDataAsFile(StorageFile* pFile, DB_RECORD_NUMBER dwNumber);

	// Set blob data.
	ERRORCODE SetBlobData(LPVOID pData, DWORD dwSize, DB_RECORD_NUMBER* pdwNumber);
	ERRORCODE SetBlobData(StorageDevice* pDevice, DB_RECORD_NUMBER* pdwNumber);

public:
	// Allocate a huge block of memory.
	static ERRORCODE AllocateMemory(FLATFILE_DATABASE_MEMORY_PTR* pMemory, DWORD dwSize);

	// Free a block of memory allocated by AllocateMemory().
	static ERRORCODE FreeMemory(FLATFILE_DATABASE_MEMORY_PTR pMemory);

	// Fill a huge block with a given byte value.
	static void FillBytes(FLATFILE_DATABASE_MEMORY_PTR pMemory, DWORD dwSize, BYTE bByte = 0);

	// Copy a huge block.
	static void CopyBytes(FLATFILE_DATABASE_MEMORY_PTR pDestination, FLATFILE_DATABASE_MEMORY_CONST_PTR pSource, DWORD dwSize, BOOL fTerminate = FALSE);

	// Self test.
	static ERRORCODE SelfTest(void);

protected:
	// Update the indicies to include the new records.
	ERRORCODE UpdateIndex(CFlatFileHeaderRecord* pHeader, DB_RECORD_NUMBER dwNumber, LPBYTE pData);

	// Pointers to cached field and index descriptions.
	CFlatFileFieldDescription* m_pFieldDescriptions;
	CFlatFileIndexDescription* m_pIndexDescriptions;

	// Cached field and index counts.
	DWORD m_dwFieldDescriptionCount;
	DWORD m_dwIndexDescriptionCount;

	// Cached record size (total bytes to hold all fields.)
	DWORD m_dwRecordSize;
};

// Hard-coded database record numbers.
// These records MUST have these record numbers.
// (This is easily maintained by creating them in the proper order.)

#define FLATDB_USERDATA_RECORD		1		// 32-bit record number of user defined data
#define FLATDB_COUNTER_RECORD			2		// 32-bit unsigned utility counter
#define FLATDB_HEADER_RECORD			3		// header record with database structure and other information

//////////////////////////////////////////////////////////////////////////
// Flat file database cursor and supporting classes
// These classes throw CErrorcodeExceptions.

// A buffer for a flat file database record.
class CFlatFileDatabaseRecordBuffer
{
public:
	CFlatFileDatabaseRecordBuffer(CFlatFileDatabase* pDatabase);
	virtual ~CFlatFileDatabaseRecordBuffer();

public:
	// Get pointer to associated database.
	CFlatFileDatabase* GetDatabase(void) const
		{ return m_pDatabase; }

	// Get buffer size.
	DWORD GetSize(void) const
		{ return m_dwSize; }

	// Get a pointer to the buffer data.
	LPBYTE GetData(void) const
		{ return m_pData; }

	// Allocate the data (m_dwSize must be valid).
	void AllocateData(void);

	// Free any allocated data.
	void FreeData(void);

	// Return a pointer to the buffer data for a given buffer offset.
	LPBYTE DataAddress(DWORD dwOffset);

protected:
	// Pointer to database.
	CFlatFileDatabase* m_pDatabase;

	// Size of buffer in bytes.
	DWORD m_dwSize;

	// Pointer to allocated data.
	LPBYTE m_pData;

public:
	// Copy record data from another source.
	void SetData(LPBYTE pData);

	// Assignment operator.
	CFlatFileDatabaseRecordBuffer& operator=(const CFlatFileDatabaseRecordBuffer& Buffer)
		{ SetData(Buffer.GetData()); return *this; }
	CFlatFileDatabaseRecordBuffer& operator=(LPVOID pData)
		{ SetData((LPBYTE)pData); return *this; }
};

// Database field.
class CFlatFileDatabaseField
{
public:
	CFlatFileDatabaseField();
	virtual ~CFlatFileDatabaseField();

public:
	// Attach the field to a particular database, field description, and data address.
	void Attach(CFlatFileDatabase* pDatabase, CFlatFileFieldDescription* pDescription, LPBYTE pData);

	// Detach the field from any current field description and/or data address.
	void Detach(void);

	// Get pointer to associated database.
	CFlatFileDatabase* GetDatabase(void) const
		{ return m_pDatabase; }

	// Get pointer to associated field description.
	CFlatFileFieldDescription* GetDescription(void) const
		{ return m_pDescription; }

	// Get pointer to start of data.
	LPBYTE GetData(void) const
		{ return m_pData; }

	// Get a pointer to the name of the field.
	LPCSTR GetName(void);

	// Get the type of the field.
	DWORD GetType(void);

	// Get the size of the field.
	DWORD GetSize(void);

	// Check if field has been modified.
	BOOL IsModified(void)
		{ return m_fModified; }

	// Mark the field as modified.
	void SetModified(BOOL fModified = TRUE)
		{ m_fModified = fModified; }

protected:
	// Pointer to associated database.
	CFlatFileDatabase* m_pDatabase;

	// Pointer to associated description.
	CFlatFileFieldDescription* m_pDescription;

	// Pointer to associated data buffer.
	LPBYTE m_pData;

	// Helper methods for accessing data in various ways.
	LPSTR StringPtr(void)
		{ return (LPSTR)m_pData; }
	long* Signed32Ptr(void)
		{ return (long*)m_pData; }
	DWORD* Unsigned32Ptr(void)
		{ return (DWORD*)m_pData; }

	// Modified flag.
	BOOL m_fModified;

public:
	// These methods are generally overridden by descendant classes.

	// Initialize the field back to an empty state.
	virtual void Initialize(void);

	// Set counter from database.
	virtual void SetCounter(BOOL fForceNewCounter = FALSE);

	// Get/Set as string.
	virtual LPCSTR GetAsString(void);
	virtual void SetAsString(LPCSTR pszString);

	// Get/Set as 32-bit signed number.
	virtual long GetAsSigned32(void);
	virtual void SetAsSigned32(long lNumber);

	// Get/Set as 32-bit unsigned number.
	virtual DWORD GetAsUnsigned32(void);
	virtual void SetAsUnsigned32(DWORD dwNumber);

	// Assignment operator.
	CFlatFileDatabaseField& operator=(CFlatFileDatabaseField& Field)
		{ SetAsString((LPCSTR)Field); return *this; }
	CFlatFileDatabaseField& operator=(LPCSTR pszString)
		{ SetAsString(pszString); return *this; }
	CFlatFileDatabaseField& operator=(long lNumber)
		{ SetAsSigned32(lNumber); return *this; }
	CFlatFileDatabaseField& operator=(DWORD dwNumber)
		{ SetAsUnsigned32(dwNumber); return *this; }
	CFlatFileDatabaseField& operator=(int nNumber)
		{ SetAsSigned32((long)nNumber); return *this; }
	CFlatFileDatabaseField& operator=(unsigned int uNumber)
		{ SetAsUnsigned32((DWORD)uNumber); return *this; }

	// Conversion operator.
	operator LPCSTR ()
		{ return GetAsString(); }
	operator long ()
		{ return GetAsSigned32(); }
	operator DWORD ()
		{ return GetAsUnsigned32(); }
	operator int ()
		{ return (int)GetAsSigned32(); }
	operator unsigned int ()
		{ return (unsigned int)GetAsUnsigned32(); }

	// Create a sorted array element (see SORTSTR.H) containing the current field value.
	virtual CSortedPtrArrayElement* GetSortedArrayElement(void);
};

// Derived Field classes.
class CFlatFileDatabaseFieldString : public CFlatFileDatabaseField
{
public:
	CFlatFileDatabaseFieldString();
	virtual ~CFlatFileDatabaseFieldString();

public:
	// Get/Set as string.
	virtual LPCSTR GetAsString(void);
	virtual void SetAsString(LPCSTR pszString);

	// Get/Set as 32-bit signed number.
	virtual long GetAsSigned32(void);
	virtual void SetAsSigned32(long lNumber);

	// Get/Set as 32-bit unsigned number.
	virtual DWORD GetAsUnsigned32(void);
	virtual void SetAsUnsigned32(DWORD dwNumber);

	// Create a sorted array element (see SORTSTR.H) containing the current field value.
	virtual CSortedPtrArrayElement* GetSortedArrayElement(void);
};

class CFlatFileDatabaseFieldBlob : public CFlatFileDatabaseField
{
public:
	CFlatFileDatabaseFieldBlob();
	virtual ~CFlatFileDatabaseFieldBlob();

public:
	// Get/Set as 32-bit unsigned number.
	virtual DWORD GetAsUnsigned32(void);
	virtual void SetAsUnsigned32(DWORD dwNumber);
};

class CFlatFileDatabaseFieldCounter32 : public CFlatFileDatabaseField
{
public:
	CFlatFileDatabaseFieldCounter32();
	virtual ~CFlatFileDatabaseFieldCounter32();

protected:
	CString m_csValue;	// buffer for string value

public:
	// Get/Set as string.
	virtual LPCSTR GetAsString(void);
	virtual void SetAsString(LPCSTR pszString);

	// Get/Set as 32-bit signed number.
	virtual long GetAsSigned32(void);
	virtual void SetAsSigned32(long lNumber);

	// Get/Set as 32-bit unsigned number.
	virtual DWORD GetAsUnsigned32(void);
	virtual void SetAsUnsigned32(DWORD dwNumber);

	// Create a sorted array element (see SORTSTR.H) containing the current field value.
	virtual CSortedPtrArrayElement* GetSortedArrayElement(void);
};

class CFlatFileDatabaseFieldSigned32 : public CFlatFileDatabaseField
{
public:
	CFlatFileDatabaseFieldSigned32();
	virtual ~CFlatFileDatabaseFieldSigned32();

protected:
	CString m_csValue;	// buffer for string value

public:
	// Get/Set as string.
	virtual LPCSTR GetAsString(void);
	virtual void SetAsString(LPCSTR pszString);

	// Get/Set as 32-bit signed number.
	virtual long GetAsSigned32(void);
	virtual void SetAsSigned32(long lNumber);

	// Get/Set as 32-bit unsigned number.
	virtual DWORD GetAsUnsigned32(void);
	virtual void SetAsUnsigned32(DWORD dwNumber);

	// Create a sorted array element (see SORTSTR.H) containing the current field value.
	virtual CSortedPtrArrayElement* GetSortedArrayElement(void);
};

class CFlatFileDatabaseFieldUnsigned32 : public CFlatFileDatabaseField
{
public:
	CFlatFileDatabaseFieldUnsigned32();
	virtual ~CFlatFileDatabaseFieldUnsigned32();

protected:
	CString m_csValue;	// buffer for string value

public:
	// Get/Set as string.
	virtual LPCSTR GetAsString(void);
	virtual void SetAsString(LPCSTR pszString);

	// Get/Set as 32-bit signed number.
	virtual long GetAsSigned32(void);
	virtual void SetAsSigned32(long lNumber);

	// Get/Set as 32-bit unsigned number.
	virtual DWORD GetAsUnsigned32(void);
	virtual void SetAsUnsigned32(DWORD dwNumber);

	// Create a sorted array element (see SORTSTR.H) containing the current field value.
	virtual CSortedPtrArrayElement* GetSortedArrayElement(void);
};

// Cursor for high level database access.
class CFlatFileDatabaseCursor
{
public:
	CFlatFileDatabaseCursor();
	virtual ~CFlatFileDatabaseCursor();

public:
	// Attach the cursor to a database.
	void Attach(CFlatFileDatabase* pDatabase);

	// Detach the cursor from a database.
	void Detach(void);

	// Get the database to which the cursor is attached.
	CFlatFileDatabase* GetDatabase(void) const
		{ return m_pDatabase; }

	// Get the record buffer to which the cursor is attached.
	CFlatFileDatabaseRecordBuffer* GetBuffer(void) const
		{ return m_pBuffer; }

	// Get field information.
	CFlatFileDatabaseField** GetFields(void);
	DWORD GetFieldCount(void);
	CFlatFileDatabaseField* GetField(DWORD dwNumber);
	CFlatFileDatabaseField* GetField(LPCSTR pszName);
	DWORD GetFieldNumberFromName(LPCSTR pszName);

	// Get index information.
	CFlatFileIndexDescription* GetIndexDescriptions(void);
	DWORD GetIndexDescriptionCount(void);
	CFlatFileIndexDescription* GetIndexDescription(DWORD dwNumber);
	CFlatFileIndexDescription* GetIndexDescription(LPCSTR pszName);
	DWORD GetIndexNumberFromName(LPCSTR pszName);
	DWORD GetIndexFieldCount(DWORD dwNumber);
	DWORD GetIndexFieldNumber(DWORD dwNumber, DWORD dwField);

	// Get main field information.
	DWORD GetMainFieldOfIndex(DWORD dwIndexNumber);
	DWORD GetIndexOfMainField(DWORD dwFieldNumber);

	// Initialize a sorted array with the unique value of the field.
	virtual void GetSortedArrayOfValues(CSortedPtrArray* pArray);

	// Set the current index.
	void SetIndex(DWORD dwNumber);
	void SetIndex(LPCSTR pszName);

	// Get the number of the current index.
	DWORD GetIndex(void);

	// Get the description of the current index.
	CFlatFileIndexDescription* GetIndexDescription(void);

	// Get the number of records in the current index.
	DWORD GetRecordCount(void);

	// Cursor movement.
	void First(void);							// first record
	void Last(void);							// last record
	void Next(void);							// next record
	void Previous(void);						// previous record
	void SetPosition(DWORD dwPosition);	// specific position
	DWORD GetPosition(void);				// get current position
	BOOL AtBOF(void);							// true if at beginning of index
	BOOL AtEOF(void);							// true if at end of index

	// Update the field data from the current record.
	void Refresh(void);

	// Delete the current record.
	void Delete(BOOL fDeleteBlobs = FALSE);

	// Delete all the records.
	void DeleteAll(void);

	// Insert the current record.
	void Insert(void);

	// Update the current record.
	void Update(void);

	// Create an empty record.
	void New(void);

	// Find a record.
	void Find(BOOL* pfExact = NULL, DWORD dwFlags = 0);

	// Set the counter fields from the database.
	void CFlatFileDatabaseCursor::SetCounters(BOOL fForceNewCounters = FALSE);

	// Get blob data.
	LPVOID GetBlobData(DB_RECORD_NUMBER dwNumber);
	void GetBlobDataAsFile(StorageFile* pFile, DWORD dwNumber);

	// Insert a blob record into the database.
	DB_RECORD_NUMBER SetBlobData(LPVOID pData, DWORD dwSize);
	DWORD SetBlobData(StorageDevice* pDevice);

	// Delete a record from the database.
	void DeleteRecord(DWORD dwRecordNumber);

	// Get the current user data.
	void GetUserData(LPVOID* ppData, DWORD* pdwSize);
	void GetUserDataAsFile(StorageFile* pFile);

	// Set the current user data.
	void SetUserData(LPVOID pData, DWORD dwSize);
	void SetUserData(StorageDevice* pDevice);

	// Delete the current user data.
	void DeleteUserData(void);

	// Check if data has been modified.
	BOOL IsModified(void);

	// Mark the current record as modified.
	void SetModified(BOOL fModified = TRUE);

protected:
	// Allocate and initialize the record buffers used by this cursor.
	void AllocateBuffer(void);

	// Free any allocated record buffers.
	void FreeBuffer(void);

	// Allocate and initialize the array of field objects for the attached database.
	void AllocateFields(void);

	// Free any allocated field objects.
	void FreeFields(void);

	// Initialize the field objects.
	void InitializeFields(void);

protected:
	// Pointer to CFlatFileDatabase to which the cursor is attached.
	CFlatFileDatabase* m_pDatabase;

	// Pointer to record buffer.
	CFlatFileDatabaseRecordBuffer* m_pBuffer;

	// Pointer to allocated array of field objects.
	CFlatFileDatabaseField** m_pFieldArray;

	// Number of allocated fields.
	DWORD m_dwFieldCount;

	// Current position (sequence number).
	DWORD m_dwPosition;

	// Current index number.
	DWORD m_dwIndex;

public:
	// Make a copy of another cursor.
	void SetCursor(CFlatFileDatabaseCursor& Cursor);

	// Copy record buffer data from another source.
	void SetData(LPBYTE pData);
	void SetData(CFlatFileDatabaseRecordBuffer* pBuffer);

	// Assignment operator.
	CFlatFileDatabaseCursor& operator=(CFlatFileDatabaseCursor& Cursor)
		{ SetCursor(Cursor); return *this; }
	CFlatFileDatabaseCursor& operator=(const CFlatFileDatabaseRecordBuffer& Buffer)
		{ SetData(Buffer.GetData()); return *this; }
	CFlatFileDatabaseCursor& operator=(LPVOID pData)
		{ SetData((LPBYTE)pData); return *this; }
};

//////////////////////////////////////////////////////////////////////////
// CFlatFileValueListManager
//
// This is a helper class which manages the lists of unique field values.

class CFlatFileValueListManager : public CMapPtrToPtr
{
public:
	CFlatFileValueListManager();
	~CFlatFileValueListManager();

public:
	// Reinitialize the list manager for a given database.
	void SetDatabase(CFlatFileDatabase* pDatabase);

	// Get the list associated with a given database field.
	CSortedPtrArray* GetListForField(CFlatFileFieldDescription* pField);

	// Get the list associated with a given database index.
	CSortedPtrArray* GetListForIndex(CFlatFileIndexDescription* pIndex);

	// Declare any lists associated with the given field to be modified.
	void FieldModified(CFlatFileFieldDescription* pField = NULL);

protected:
	// Delete all the list manager entries.
	void DeleteAll(void);

	// Delete a specific list manager entry.
	void DeleteList(CFlatFileFieldDescription* pField);

	// Build the list for a particular database field.
	CSortedPtrArray* BuildList(CFlatFileFieldDescription* pField);

protected:
	// Current database.
	CFlatFileDatabase* m_pDatabase;

	// Current database cursor.
	CFlatFileDatabaseCursor m_Cursor;
};

inline void CheckFlatFileResult(ERRORCODE e)
	{ if (e != ERRORCODE_None) { ASSERT(FALSE); ThrowErrorcodeException(e); } }

inline void CheckFlatFileCondition(BOOL f, ERRORCODE e = ERRORCODE_IntError)
	{ if (!f) CheckFlatFileResult(e); }

inline void CheckFlatFileAllocation(LPVOID p)
	{ if (p == NULL) CheckFlatFileResult(ERRORCODE_Memory); }

#pragma warning(default:4200)	// zero-size array in struct/union

#endif
