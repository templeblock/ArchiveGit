/*
// Embedded file record.
//
// $Workfile: EFILEDAT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/EFILEDAT.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:27:06   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:00   FRED
// Initial revision.
// 
//    Rev 2.2   15 Sep 1995 14:04:52   JAY
// More 32-bit changes.
// 
//    Rev 2.1   08 Feb 1995 13:39:28   JAY
// Reverted. New series.
// 
//    Rev 1.1   04 Nov 1994 09:55:12   JAY
// Added ExtractFile().
// 
//    Rev 1.0   03 Nov 1994 15:53:52   JAY
// Initial revision.
// 
//    Rev 1.0   02 Nov 1994 12:54:50   JAY
// Initial revision.
*/

class CEmbeddedFileData;

#ifndef __EFILEDAT_H__
#define __EFILEDAT_H__

class StorageFile;

#include "cmanstd.h"

typedef enum
{
	EMBEDDED_TYPE_UNKNOWN,
	EMBEDDED_TYPE_FILE
} EMBEDDED_FILE_TYPE;

/*
// The embedded file source record.
*/

class CEmbeddedFileData : public CStandardDataRecord
{
protected:
/*
// Data!
*/
	ST_DEV_POSITION	m_DataStart;
	CString m_csFileName;

	struct
	{
		SHORT m_Type;				// EMBEDDED_FILE_TYPE
		ST_MAN_SIZE m_DataSize;
	} m_Record;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CEmbeddedFileData();

/*
// Assign method.
*/

	virtual void Assign(const CDataRecord& Record);

/*
// Embed a file in this record.
*/

	ERRORCODE EmbedFile(LPCSTR filename);

/*
// Prep file completion routines.
*/

	static void EfileNormalCompletion(StorageFile* file, LPVOID pData);
	static void EfileReadonlyCompletion(StorageFile* file, LPVOID pData);

public:

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static CStandardDataRecord* Create();
	virtual BOOL Init(DATAID id, DATATYPE type, CStandardDataManager* pManager, LPVOID pData, ST_DEV_POSITION* in_where);

/*
// The destructor.
*/

	virtual ~CEmbeddedFileData();

/*
// Return the start of data.
*/

	ST_DEV_POSITION DataStart(void) const;

/*
// Return the length of data.
*/

	ST_MAN_SIZE DataSize(void) const;

/*
// Return the file name.
*/

	const CString& FileName(void) const;

/*
// Prep a file based on us.
// This leaves the record with the appropriate lock (param must be correct!)
*/

	ERRORCODE PrepFile(StorageFile* pFile, BOOL read_only = FALSE) const;

/*
// Extract the file from this record.
// The passed filename is the name of the destination file.
// (The filename in this record is ignored.)
*/

	ERRORCODE ExtractFile(LPCSTR filename);

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	virtual ERRORCODE ReadData(StorageDevicePtr pDevice);

/*
// WriteData()
//
// Write the record.
*/

	virtual ERRORCODE WriteData(StorageDevicePtr pDevice);

/*
// SizeofData()
//
// Return the size of the record.
*/

	virtual ST_MAN_SIZE SizeofData(StorageDevicePtr pDevice);

/*
// RelocateData()
//
// This is a function called by the storage manager when our storage is
// ever relocated. This allows us to move any data we want saved.
// We need this so that we can move the file image.
*/

	virtual ERRORCODE RelocateData(StorageDevicePtr pDevice, ST_DEV_POSITION OldPosition, ST_DEV_POSITION NewPosition);
};

/*
// In-line functions.
*/

inline ST_DEV_POSITION CEmbeddedFileData::DataStart(void) const
	{ return m_DataStart; }
inline ST_MAN_SIZE CEmbeddedFileData::DataSize(void) const
	{ return m_Record.m_DataSize; }
inline const CString& CEmbeddedFileData::FileName(void) const
	{ return m_csFileName; }

#endif					// #ifndef __EFILEDAT_H__
