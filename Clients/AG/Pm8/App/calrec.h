/*
// $Header: /PM8/App/calrec.h 1     3/03/99 6:03p Gbeddow $
//
// Font record definitions.
//
// $Log: /PM8/App/calrec.h $
// 
// 1     3/03/99 6:03p Gbeddow
 * 
 * 2     10/31/97 3:55p Jstoner
 * multi page calendar fixes
*/

#ifndef __CALREC_H__
#define __CALREC_H__

/*
// The calendar info record.
*/

class CalendarInfoRecord : public DatabaseRecord
{
public:
	struct
	{
		short picture_style;
		short number_of_calendars;

	} record;

private:
/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CalendarInfoRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

public:

/*
// The destructor.
*/

	~CalendarInfoRecord();

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, void* creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);


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

#endif					// #ifndef __CALREC_H_
