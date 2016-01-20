/*
// $Header: /PM8/App/STYLEREC.CPP 1     3/03/99 6:12p Gbeddow $
//
// Text style record routines.
//
// $Log: /PM8/App/STYLEREC.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:38   Fred
// Initial revision.
// 
//    Rev 1.3   02 Jul 1996 08:55:08   Jay
//  
// 
//    Rev 1.2   24 May 1996 09:27:00   Jay
// New text code.
// 
//    Rev 1.1   25 Apr 1996 12:16:26   Jay
//  
// 
//    Rev 1.0   23 Apr 1996 08:08:16   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "stylerec.h"

/////////////////////////////////////////////////////////////////////////////
// CTextStyleRecord

/*
// The creator for a text style record.
*/

ERRORCODE CTextStyleRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	CTextStyleRecord* pStyleRecord = NULL;

/* Create the new array record. */

	TRY
		pStyleRecord = new CTextStyleRecord(number, type, owner, where);
	END_TRY

	if (pStyleRecord != NULL)
	{
		if (creation_data != NULL)
		{
			CCreationData* pData = (CCreationData*)creation_data;
			pStyleRecord->m_csName = pData->m_pszName;
			pStyleRecord->m_Record.m_wType = pData->m_wType;
			if (pData->m_pStyle != NULL)
			{
				pStyleRecord->m_Style = *(pData->m_pStyle);
			}
			pStyleRecord->m_Style.m_Paragraph.m_lStyle = number;	// Self-reference.
		}
	}

	*record = pStyleRecord;

	return pStyleRecord == NULL ? ERRORCODE_Memory : ERRORCODE_None;
}

/*
// The constructor for a text style record.
*/

CTextStyleRecord::CTextStyleRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where),
				  m_Style((PMGDatabase*)owner, FALSE)
{
	m_Record.m_wType = 0;
	m_Record.m_dwReferenceCount = 0;
}

/*
// The destructor for this record.
*/

CTextStyleRecord::~CTextStyleRecord()
{
}

/*
// Increment a reference count.
*/

DWORD CTextStyleRecord::Increment(void)
{
	modified();
	return ++m_Record.m_dwReferenceCount;
}

/*
// Decrement a reference count.
*/

DWORD CTextStyleRecord::Decrement(void)
{
	modified();
	ASSERT(m_Record.m_dwReferenceCount != 0);
	if (m_Record.m_dwReferenceCount != 0)
	{
		m_Record.m_dwReferenceCount--;
	}

	return m_Record.m_dwReferenceCount;
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE CTextStyleRecord::ReadData(StorageDevice* pDevice)
{
	ERRORCODE error = ERRORCODE_None;

	TRY
	{
		if ((error = pDevice->read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None
			 && (error = pDevice->read_cstring(m_csName)) == ERRORCODE_None)
		{
			m_Style.ReadData(pDevice);
			m_Style.Database((PMGDatabase*)GetDatabase());
		}
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
	}
	END_CATCH_ALL
	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE CTextStyleRecord::WriteData(StorageDevice* pDevice)
{
	ERRORCODE error = ERRORCODE_None;

	TRY
	{
		if ((error = pDevice->write_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None
			 && (error = pDevice->write_cstring(m_csName)) == ERRORCODE_None)
		{
			m_Style.WriteData(pDevice);
		}
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
	}
	END_CATCH_ALL
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE CTextStyleRecord::SizeofData(StorageDevice* pDevice)
{
	return pDevice->size_record(sizeof(m_Record))
			+ pDevice->size_cstring(m_csName)
			+ m_Style.SizeofData(pDevice);
}

/*
// Assign method.
*/

ERRORCODE CTextStyleRecord::assign(DatabaseRecordRef srecord)
{
/* Assign the base record first. */
	ASSERT(FALSE);				// Incomplete.
	return DatabaseRecord::assign(srecord);
}
