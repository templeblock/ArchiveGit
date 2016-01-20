/*
// $Header: /PM8/App/NAMEREC.H 1     3/03/99 6:08p Gbeddow $
//
// Name list record definitions.
// A name list record contains the name of the address book (database) from
// which names were chosen plus a list of (long) ids of the names chosen from
// that address book. Other bookkeeping info may also be maintained (like the
// sort key used, etc).
//
// $Log: /PM8/App/NAMEREC.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:40   Fred
// Initial revision.
// 
//    Rev 1.2   16 Jul 1996 17:29:02   Fred
// More address/sender book stuff
// 
//    Rev 1.1   16 Jul 1996 08:47:06   Fred
// Changes for new address book
// 
//    Rev 1.0   14 Mar 1996 13:44:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:52   FRED
// Initial revision.
// 
//    Rev 1.0   19 Apr 1995 12:36:46   JAY
// Initial revision.
*/

#ifndef __NAMEREC_H__
#define __NAMEREC_H__

/*
// The name list data record.
*/

class NameListRecord : public DatabaseRecord
{
public:

/*
// The destructor.
*/

	~NameListRecord();

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Get version.
*/
	WORD GetVersion(void)
		{ return m_Record.m_wVersion; }

/*
// Add a name.
*/

	void AddName(long lRecordID);

/*
// Delete a name by index.
*/

	void DeleteName(int nName);

/*
// Delete a name by id.
*/

	void DeleteName(long lRecordID);

/*
// Delete all names.
*/

	void DeleteAllNames(void);

/*
// Get the name count.
// (First entry in the array is the size of the array.)
*/

	int Names(void) const
		{ return m_dwaNames.GetSize() - 1; }

/*
// Get a name.
*/

	long Name(int nName) const
		{ return m_dwaNames.GetAt(nName+1); }

/*
// Find a name by id (and return the index).
// Returns -1 if not found.
*/

	int FindName(long lRecordID);

/*
// Address book name access.
*/

	const CString& AddressBookName(void) const
		{ return m_csAddressBookName; }
	void AddressBookName(LPCSTR pName)
		{ m_csAddressBookName = pName; }

/*
// Address book id access.
*/

	long AddressBookID(void) const
		{ return m_Record.m_lAddressBookID; }
	void AddressBookID(long lID)
		{ m_Record.m_lAddressBookID =	lID; }

/*
// Sort field access.
*/

	const CString& SortField(void) const
		{ return m_csSortField; }
	void SortField(LPCSTR pSortField)
		{ m_csSortField = pSortField; }

/*
// Block copy names.
*/

	void GetAllNames(DWORD* pNames);
	void PutAllNames(const DWORD* pNames, int nNames);

/*
// Changed flag methods.
*/

	void ClearChanged(void);
	BOOL GetChanged(void) const
		{ return GetAddressBookDataChanged() || GetAddressBookNameChanged() || GetSortFieldChanged() || GetNamesChanged(); }
	void SetAddressBookDataChanged(BOOL fChanged = TRUE)
		{ m_fAddressBookDataChanged = fChanged; }
	BOOL GetAddressBookDataChanged(void) const
		{ return m_fAddressBookDataChanged; }
	void SetAddressBookNameChanged(BOOL fChanged = TRUE)
		{ m_fAddressBookNameChanged = fChanged; }
	BOOL GetAddressBookNameChanged(void) const
		{ return m_fAddressBookNameChanged; }
	void SetSortFieldChanged(BOOL fChanged = TRUE)
		{ m_fSortFieldChanged = fChanged; }
	BOOL GetSortFieldChanged(void) const
		{ return m_fSortFieldChanged; }
	void SetNamesChanged(BOOL fChanged = TRUE)
		{ m_fNamesChanged = fChanged; }
	BOOL GetNamesChanged(void) const
		{ return m_fNamesChanged; }

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

protected:

	struct
	{
		WORD m_wCount;			// For transportation purposes only.
									// Will mirror the array size, but only set and
									// used at read and write time.
		long m_lAddressBookID;
		WORD m_wVersion;		// 0 = 2.0,2.1, 3 = 3.0
	} m_Record;

/*
// Array of record ids (for names).
*/

	CDWordArray m_dwaNames;

/*
// The name of the address book.
*/

	CString m_csAddressBookName;

/*
// The name of the sort field.
*/

	CString m_csSortField;

/*
// Changed flags.
*/

	BOOL m_fAddressBookDataChanged;
	BOOL m_fNamesChanged;
	BOOL m_fAddressBookNameChanged;
	BOOL m_fSortFieldChanged;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	NameListRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Assign method.
*/

//	virtual ERRORCODE assign(DatabaseRecordRef record);
};

#endif					// #ifndef __NAMEREC_H__
