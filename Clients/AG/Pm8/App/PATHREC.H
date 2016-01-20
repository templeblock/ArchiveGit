/*
// $Header: /PM8/App/PATHREC.H 1     3/03/99 6:08p Gbeddow $
//
// $Workfile: PATHREC.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/PATHREC.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:47:48   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:38   FRED
// Initial revision.
// 
//    Rev 1.2   23 Jun 1995 08:35:10   JAY
// Moved m_DesignBounds into the "record" member so that it will be initialized
// and read and written.
// 
//    Rev 1.1   19 May 1995 09:24:34   FRED
// Added bounds to path record
// 
//    Rev 1.0   12 May 1995 10:28:32   JAY
// Initial revision.
*/

#ifndef __PATHREC_H__
#define __PATHREC_H__

#include "outpath.h"
#include "cfixed.h"

/////////////////////////////////////////////////////////////////////////////
// OutlinePathCreateStruct

struct OutlinePathCreateStruct
{
	LPCSTR m_pszName;
	COutlinePath* m_pPath;
	CFixedRect* m_pDesignBounds;
};

/////////////////////////////////////////////////////////////////////////////
// OutlinePathRecord

class OutlinePathRecord : public DatabaseRecord
{
public:
protected:

/* Data */

	CString m_csName;			/* the name of this warp field. */

	struct
	{
		USHORT reference_count;
		CFixedRect m_DesignBounds;
	} record;

	COutlinePath m_Path;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	OutlinePathRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

public:

	virtual ~OutlinePathRecord();

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Set the reference count.
*/

	VOID set_reference_count(USHORT count)
		{ record.reference_count = count; }

/*
// Get the reference count.
*/

	USHORT get_reference_count(VOID)
		{ return record.reference_count; }

/*
// Increment the reference count.
*/

	USHORT inc_reference_count(VOID)
		{ return ++record.reference_count; }

/*
// Decrement the reference count.
*/

	USHORT dec_reference_count(VOID)
		{ return --record.reference_count; }

/*
// Set the name.
*/

	void Name(LPCSTR pszName)
		{ m_csName = pszName; }

/*
// Get the name.
*/

	const CString& Name(void) const
		{ return m_csName; }
		
/*
// Get a pointer to the outline path.
*/

	COutlinePath& OutlinePath(void)
		{ return m_Path; }
		
/*
// Get a pointer to the design bounds.
*/

	CFixedRect& DesignBounds(void)
		{ return record.m_DesignBounds; }

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

#endif
