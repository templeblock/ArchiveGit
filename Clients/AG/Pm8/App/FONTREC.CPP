/*
// $Header: /PM8/App/FONTREC.CPP 1     3/03/99 6:05p Gbeddow $
//
// Font data record routines.
//
// $Log: /PM8/App/FONTREC.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     9/14/98 12:08p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:20:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:32   Fred
// Initial revision.
// 
//    Rev 1.1   03 Apr 1997 09:30:54   Fred
// More text cleanup
// 
//    Rev 1.0   14 Mar 1996 13:47:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:32   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:47:46   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:33:16   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   01 Jun 1994 16:35:18   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.1   05 Jan 1994 08:05:42   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:24:56   JAY
// 
*/

#include "stdafx.h"
#include "fontrec.h"

/*****************************************************************************/
/*                          FontData record                                  */
/*****************************************************************************/

/*
// The creator for a font data record.
*/

ERRORCODE FontDataRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, void* creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	FontDataPtr font_data;

/* Create the new array record. */

	if ((font_data = new FontDataRecord(number, type, owner, where)) == NULL)
	{
		return ERRORCODE_Memory;
	}

	if (creation_data != NULL)
	{
		font_data->set_face_name((LPCSTR)creation_data);
	}

	*record = font_data;

	return ERRORCODE_None;
}

/*
// The constructor for a project preview record.
*/

FontDataRecord::FontDataRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	record.reference_count = 0;
	record.tmp_reference_count = 0;
	record.face_index = -1;
}

/*
// The destructor for this record.
*/

FontDataRecord::~FontDataRecord()
{
}

/*
// Set the face name for this record.
*/

void FontDataRecord::set_face_name(LPCSTR new_name)
{
	m_csName = new_name;

	modified();
}

/*
// Increment a reference count.
*/

ULONG FontDataRecord::inc(BOOL temp)
{
	modified();
	if (temp)
	{
		record.tmp_reference_count++;
	}
	else
	{
		record.reference_count++;
	}
	return record.tmp_reference_count + record.reference_count;
}

/*
// Decrement a reference count.
*/

ULONG FontDataRecord::dec(BOOL temp)
{
	modified();
	if (temp)
	{
		if (record.tmp_reference_count != 0)
		{
			record.tmp_reference_count--;
		}
	}
	else
	{
		if (record.reference_count != 0)
		{
			record.reference_count--;
		}
	}
	return record.tmp_reference_count + record.reference_count;
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE FontDataRecord::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read_record(&record, sizeof(record))) == ERRORCODE_None)
	{
		error = device->read_cstring(m_csName);
	}
	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE FontDataRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->write_record(&record, sizeof(record))) == ERRORCODE_None)
	{
		error = device->write_cstring(m_csName);
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE FontDataRecord::SizeofData(StorageDevicePtr device)
{
	return device->size_record(sizeof(record)) + device->size_cstring(m_csName);
}

/*
// Assign method.
*/

ERRORCODE FontDataRecord::assign(DatabaseRecordRef srecord)
{
/* Assign the base record first. */
	return DatabaseRecord::assign(srecord);
}
