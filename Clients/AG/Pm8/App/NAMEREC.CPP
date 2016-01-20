/*
// $Header: /PM8/App/NAMEREC.CPP 1     3/03/99 6:08p Gbeddow $
//
// Name list record routines.
//
// $Log: /PM8/App/NAMEREC.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 3     5/21/98 9:18a Cboggio
// 
// 2     5/20/98 4:57p Cboggio
// If the sort field read from the template is not a valid permanent field
// name then just use default
// 
//    Rev 1.0   14 Aug 1997 15:22:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:18   Fred
// Initial revision.
// 
//    Rev 1.2   16 Jul 1996 17:29:02   Fred
// More address/sender book stuff
// 
//    Rev 1.1   16 Jul 1996 08:47:06   Fred
// Changes for new address book
// 
//    Rev 1.0   14 Mar 1996 13:44:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:50   FRED
// Initial revision.
// 
//    Rev 1.0   19 Apr 1995 12:36:44   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "namerec.h"

#ifdef LOCALIZE
#include "clocale.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// NameListRecord

/*
// The creator for a macro list data record.
*/

ERRORCODE NameListRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	NameListRecord* pRecord;

/* Create the new macro list record. */

	if ((pRecord = new NameListRecord(number, type, owner, where)) == NULL)
	{
		return ERRORCODE_Memory;
	}

	if (creation_data != NULL)
	{
	}

	*record = pRecord;

	return ERRORCODE_None;
}

/*
// Constructor.
*/

NameListRecord::NameListRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	m_Record.m_wCount = 0;
	m_Record.m_lAddressBookID = 0;
	m_Record.m_wVersion = 3;

	m_dwaNames.Add(0);					// 0 record ids in array so far.
}

NameListRecord::~NameListRecord()
{
}

/*
// Add a name.
*/

void NameListRecord::AddName(long lRecordID)
{
	if (FindName(lRecordID) == -1)
	{
		modified();
		m_dwaNames.Add(lRecordID);
		m_dwaNames.SetAt(0, m_dwaNames.GetSize()-1);
	}
}

/*
// Delete a name by index.
*/

void NameListRecord::DeleteName(int nName)
{
	if (nName != -1)
	{
		modified();
		m_dwaNames.RemoveAt(nName+1);
		m_dwaNames.SetAt(0, m_dwaNames.GetSize()-1);
	}
}

/*
// Delete a name by id.
*/

void NameListRecord::DeleteName(long lRecordID)
{
	DeleteName(FindName(lRecordID));
}

/*
// Delete all names.
*/

void NameListRecord::DeleteAllNames(void)
{
	modified();
	TRY
	{
		m_dwaNames.RemoveAll();
		m_dwaNames.Add(0);
	}
	END_TRY
}

/*
// Find a name by record id and return its index.
*/

int NameListRecord::FindName(long lRecordID)
{
	int nNames = m_dwaNames.GetSize();

	for (int nName = 1; nName < nNames; nName++)
	{
		if (m_dwaNames.GetAt(nName) == (DWORD)lRecordID)
		{
			return nName - 1;
		}
	}
	return -1;
}

void NameListRecord::GetAllNames(DWORD* pNames)
{
	int nNames = Names();

	if (nNames != 0)
	{
		memcpy(pNames, &m_dwaNames.ElementAt(1), nNames*sizeof(DWORD));
	}
}

void NameListRecord::PutAllNames(const DWORD* pNames, int nNames)
{
	modified();
	m_dwaNames.SetSize(nNames+1);
	m_dwaNames.SetAt(0, (DWORD)nNames);

	if (nNames != 0)
	{
		memcpy(&m_dwaNames.ElementAt(1), pNames, nNames*sizeof(DWORD));
	}
}

void NameListRecord::ClearChanged(void)
{
	SetAddressBookDataChanged(FALSE);
	SetAddressBookNameChanged(FALSE);
	SetSortFieldChanged(FALSE);
	SetNamesChanged(FALSE);
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE NameListRecord::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

/* Read the master record. */

	if ((error = device->read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		int nNames = m_Record.m_wCount;

	/* Read all name record ids. */

		m_dwaNames.SetSize(nNames+1);

		if ((error = device->read((LPVOID)&(m_dwaNames.ElementAt(0)), (nNames+1)*sizeof(DWORD))) == ERRORCODE_None)
		{
		/* Read the address book string. */
			if ((error = device->read_cstring(m_csAddressBookName)) == ERRORCODE_None)
			{
				if ((error = device->read_cstring(m_csSortField)) == ERRORCODE_None)
				{
				/* Success! */
#ifdef LOCALIZE
					// CB Sep 97 - added for localization
					if (!IsPermanentField(m_csSortField))
						// default to "relationship"
						m_csSortField.LoadString(IDS_LOC_RELATIONSHIP);
#endif
				}
			}
		}
	}
	ASSERT(m_dwaNames.GetSize() == (int)m_dwaNames.GetAt(0)+1);
	ASSERT(m_dwaNames.GetSize() == (int)m_Record.m_wCount+1);

	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE NameListRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;
	int nNames = Names();

/* Establish size. */

	m_Record.m_wCount = nNames;
	ASSERT((int)m_dwaNames.GetAt(0) == nNames);

/* Version 3.0 */

	m_Record.m_wVersion = 3;

/* Write the master record. */

	if ((error = device->write_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
	/* Write the names. */

		if ((error = device->write((LPVOID)&(m_dwaNames.ElementAt(0)), (nNames+1)*sizeof(DWORD))) == ERRORCODE_None)
		{
		/* Read the address book string. */
			if ((error = device->write_cstring(m_csAddressBookName)) == ERRORCODE_None)
			{
				if ((error = device->write_cstring(m_csSortField)) == ERRORCODE_None)
				{
				/* Success! */
				}
			}
		}
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE NameListRecord::SizeofData(StorageDevicePtr device)
{
	ST_MAN_SIZE Size = device->size_record(sizeof(m_Record));

	Size += (Names()+1) * sizeof(DWORD);
	Size += device->size_cstring(m_csAddressBookName);
	Size += device->size_cstring(m_csSortField);

	return Size;
}

