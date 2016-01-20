/*
// $Header: /PM8/App/PATHREC.CPP 1     3/03/99 6:08p Gbeddow $
//
// $Workfile: PATHREC.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/PATHREC.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:46:52   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:56   FRED
// Initial revision.
// 
//    Rev 1.3   23 Jun 1995 08:35:34   JAY
// Moved m_DesignBounds into the "record" member so that it will be initialized
// and read and written.
// 
//    Rev 1.2   19 May 1995 09:24:36   FRED
// Added bounds to path record
// 
//    Rev 1.1   13 May 1995 11:16:52   JAY
//  
// 
//    Rev 1.0   12 May 1995 10:28:30   JAY
// Initial revision.
*/

#include "stdafx.h"

#include "pathrec.h"

/////////////////////////////////////////////////////////////////////////////
// OutlinePathRecord

/*
// The creator.
*/

ERRORCODE OutlinePathRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */
	OutlinePathRecord* pRecord;
	if ((pRecord = new OutlinePathRecord(number, type, owner, where)) == NULL)
	{
		return ERRORCODE_Memory;
	}
	*record = (DatabaseRecordPtr)pRecord;

	OutlinePathCreateStruct* cs = (OutlinePathCreateStruct*)creation_data;
	if (cs != NULL)
	{
		if (cs->m_pszName != NULL)
		{
			pRecord->Name(cs->m_pszName);
		}
		if (cs->m_pPath != NULL)
		{
			pRecord->OutlinePath().Copy(cs->m_pPath);
		}
		if (cs->m_pDesignBounds != NULL)
		{
			pRecord->DesignBounds() = *(cs->m_pDesignBounds);
		}
	}
	return ERRORCODE_None;
}

/*
// The constructor.
*/

OutlinePathRecord::OutlinePathRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	memset(&record, 0, sizeof(record));
}

/*
// Destructor.
*/

OutlinePathRecord::~OutlinePathRecord()
{
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE OutlinePathRecord::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read_record(&record, sizeof(record))) == ERRORCODE_None
			&& (error = device->read_cstring(m_csName)) == ERRORCODE_None
			&& (error = m_Path.ReadData(device)) == ERRORCODE_None)
	{
		if (record.m_DesignBounds.Width() == 0
			 || record.m_DesignBounds.Height() == 0)
		{
			record.m_DesignBounds.Left = MakeFixed(0);
			record.m_DesignBounds.Top = MakeFixed(0);
			record.m_DesignBounds.Right = MakeFixed(1);
			record.m_DesignBounds.Bottom = MakeFixed(1);
//			m_Path.BoundingBox(&record.m_DesignBounds);		// Normalized to 0,0 to 1,1
		}
	}
	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE OutlinePathRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->write_record(&record, sizeof(record))) == ERRORCODE_None
			&& (error = device->write_cstring(m_csName)) == ERRORCODE_None
			&& (error = m_Path.WriteData(device)) == ERRORCODE_None)
	{
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE OutlinePathRecord::SizeofData(StorageDevicePtr device)
{
	return device->size_record(sizeof(record))
 				+ device->size_cstring(m_csName)
				+ m_Path.SizeofData(device);
}

/*
// Assign method.
*/

ERRORCODE OutlinePathRecord::assign(DatabaseRecordRef srecord)
{
/* Assign the base record first. */
	return DatabaseRecord::assign(srecord);
}
