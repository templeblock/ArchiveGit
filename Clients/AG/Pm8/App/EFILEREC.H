/*
// $Header: /PM8/App/EFILEREC.H 1     3/03/99 6:05p Gbeddow $
//
// Embedded File record.
//
// $Log: /PM8/App/EFILEREC.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     9/14/98 12:08p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:20:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:43:08   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:28   FRED
// Initial revision.
// 
//    Rev 2.1   14 Sep 1995 09:32:16   JAY
// 32-bit changes
// 
//    Rev 2.0   07 Feb 1995 15:48:30   JAY
// Initial revision.
// 
//    Rev 1.4   09 Jan 1995 16:27:18   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.3   30 Nov 1994 16:33:46   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   09 Jun 1994 11:07:08   JAY
// Added prep_file().
// 
//    Rev 1.1   01 Jun 1994 16:35:26   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.0   23 May 1994 08:37:52   JAY
// Initial revision.
//  
*/

#ifndef __EFILEREC_H__
#define __EFILEREC_H__

typedef enum
{
	EMBEDDED_TYPE_UNKNOWN,
	EMBEDDED_TYPE_FILE
} EMBEDDED_FILE_TYPE;

/*
// The embedded file source record.
*/

FARCLASS EmbeddedFileRecord : public DatabaseRecord
{
private:
/*
// Data!
*/
	ST_DEV_POSITION	m_data_start;
	CString m_csFileName;

	struct
	{
		SHORT m_type;
		ST_MAN_SIZE m_data_size;
	} m_record;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	EmbeddedFileRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

/*
// Prep file completion routines.
*/

	static VOID efile_normal_completion(StorageFilePtr file, LPVOID data);
	static VOID efile_readonly_completion(StorageFilePtr file, LPVOID data);

public:

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// The destructor.
*/

	virtual ~EmbeddedFileRecord();

/*
// Return the start of data.
*/

	ST_DEV_POSITION get_data_start(void) const;

/*
// Return the length of data.
*/

	ST_MAN_SIZE get_data_size(void) const;

/*
// Return the file name.
*/

	LPCSTR get_file_name(void) const;

/*
// Prep a file based on us.
// This leaves the record with the appropriate lock (param must be correct!)
*/

	ERRORCODE prep_file(StorageFilePtr file, BOOL read_only = FALSE) const;

/*
// Embed a file in this record using a standard file as source.
*/

	ERRORCODE embed_file(LPCSTR filename);

/*
// Embed a file in this record using a device as source.
*/

	ERRORCODE embed_file(StorageDevice& source_file);

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	virtual ERRORCODE ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// Write the record.
*/

	virtual ERRORCODE WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// Return the size of the record.
*/

	virtual ST_MAN_SIZE SizeofData(StorageDevicePtr device);

/*
// RelocateData()
//
// This is a function called by the storage manager when our storage is
// ever relocated. This allows us to move any data we want saved.
// We need this so that we can move the file image.
*/

	virtual ERRORCODE RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position);
};

/*
// In-line functions.
*/

inline ST_DEV_POSITION EmbeddedFileRecord::get_data_start(void) const
	{ return m_data_start; }
inline ST_MAN_SIZE EmbeddedFileRecord::get_data_size(void) const
	{ return m_record.m_data_size; }
inline LPCSTR EmbeddedFileRecord::get_file_name(void) const
	{ return m_csFileName; }

#endif					// #ifndef __EFILEREC_H__
