/*
// $Header: /PM8/App/calrec.cpp 1     3/03/99 6:03p Gbeddow $
//
// calendar info record routines.
//
// $Log: /PM8/App/calrec.cpp $
// 
// 1     3/03/99 6:03p Gbeddow
 * 
 * 2     10/31/97 3:55p Jstoner
 * multi page calendar fixes
*/

#include "stdafx.h"
#include "calrec.h"
#include "calinfo.h"

/*****************************************************************************/
/*                          Cal info record                                  */
/*****************************************************************************/

/*
// The creator for a font data record.
*/

ERRORCODE CalendarInfoRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, void* creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	CalendarInfoRecord *  pCir;

	

	/* Create the new calinfo record. */

	if ((pCir = new CalendarInfoRecord(number, type, owner, where)) == NULL)
	{
		return ERRORCODE_Memory;
	}

	// add creation data 
	*record = pCir;

	return ERRORCODE_None;
}

/*
// The constructor for a project preview record.
*/

CalendarInfoRecord::CalendarInfoRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	record.picture_style = CAL_PICTURE_NONE;
	record.number_of_calendars = 1;
}

/*
// The destructor for this record.
*/

CalendarInfoRecord::~CalendarInfoRecord()
{
	
}


/*
// ReadData()
//
// Read the record.
*/

ERRORCODE CalendarInfoRecord::ReadData(StorageDevicePtr device)
{
	return device->read_record(&record, sizeof(record));
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE CalendarInfoRecord::WriteData(StorageDevicePtr device)
{
	return device->write_record(&record, sizeof(record));
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE CalendarInfoRecord::SizeofData(StorageDevicePtr device)
{
	return device->size_record(sizeof(record));
}

/*
// Assign method.
*/

ERRORCODE CalendarInfoRecord::assign(DatabaseRecordRef srecord)
{
/* Assign the base record first. */
	return DatabaseRecord::assign(srecord);
}
