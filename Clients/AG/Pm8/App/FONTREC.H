/*
// $Header: /PM8/App/FONTREC.H 1     3/03/99 6:05p Gbeddow $
//
// Font record definitions.
//
// $Log: /PM8/App/FONTREC.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     9/14/98 12:08p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:20:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:56   Fred
// Initial revision.
// 
//    Rev 1.1   03 Apr 1997 09:32:18   Fred
// More text cleanup
// 
//    Rev 1.0   14 Mar 1996 13:46:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:50   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:34   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:37:18   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   01 Jun 1994 16:35:16   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.1   05 Jan 1994 08:08:18   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:48   JAY
//  
*/

#ifndef __FONTREC_H__
#define __FONTREC_H__

/*
// The font data record.
*/

class FontDataRecord : public DatabaseRecord
{
private:
	struct
	{
	/*
	// The reference count for this font.
	*/

		ULONG	reference_count;

		ULONG tmp_reference_count;

	/*
	// Which face in the list this maps to.
	// A value of -1 means no mapping yet.
	*/

		SHORT face_index;

	} record;

/*
// The name of this font.
*/

	CString m_csName;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	FontDataRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

public:

/*
// The destructor.
*/

	~FontDataRecord();

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, void* creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Increment the font.
// This increments the reference count.
*/

	ULONG inc(BOOL temp = FALSE);

/*
// Decrement the font.
// This decrements the reference count.
*/

	ULONG dec(BOOL temp = FALSE);

/*
// Return the reference count.
*/

	ULONG get_reference_count(void)
		{ return record.reference_count + record.tmp_reference_count; }

/*
// Reset the temporary reference count.
*/

	void reset_tmp_count(void)
		{ record.tmp_reference_count = 0; modified(); }

/*
// Get the face name for this record.
*/

	LPCSTR get_face_name(void)
		{ return m_csName; }

/*
// Set the face name for this record.
*/

	void set_face_name(LPCSTR new_name);

/*
// Get the face index for this font record.
*/

	SHORT get_face_index(void)
		{ return record.face_index; }

/*
// Set the face index for this font record.
*/

	void set_face_index(SHORT fi)
		{ record.face_index = fi; }

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	ERRORCODE	ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// Write the record.
*/

	ERRORCODE	WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// Return the size of the record.
*/

	ST_MAN_SIZE SizeofData(StorageDevicePtr device);
};

#endif					// #ifndef __FONTREC_H__
