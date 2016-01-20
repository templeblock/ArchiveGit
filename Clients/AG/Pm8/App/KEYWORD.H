/*
// $Workfile: KEYWORD.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/KEYWORD.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:24   Fred
// Initial revision.
// 
//    Rev 1.1   30 Aug 1996 10:45:20   Jay
// Keyword/item stuff
// 
//    Rev 1.0   15 Aug 1996 16:00:22   Jay
// Initial revision.
*/

class CSortedKeywordArray;
class CKeywordRecord;
class CKeywordArrayRecord;

#ifndef __KEYWORD_H__
#define __KEYWORD_H__

#include "cmanalb.h"
#include "recarray.h"
#include "sortarry.h"

class CMlsStringArray;
class CFileItemData;

/////////////////////////////////////////////////////////////////////////////
// CSortedKeywordArray.

class CSortedKeywordArray : public CSortedDWordArray
{
public:
	CSortedKeywordArray(BOOL fNoDuplicates = TRUE);

	virtual int CustomCompare(DWORD dwValue, DWORD dwTry);

	// Search for a value by name.
	int Find(LPCSTR pszKeyword);

	ERRORCODE ReadData(StorageDevice* pDevice);
	ERRORCODE WriteData(StorageDevice* pDevice);
	ST_MAN_SIZE SizeofData(StorageDevice* pDevice);

	CDataManager* m_pManager;
protected:
	LPCSTR m_pszSearchKeyword;
};

/////////////////////////////////////////////////////////////////////////////
// The keyword record.

class CKeywordRecord : public CStandardDataRecord
{
public:

	// The creator for this type of record.
	// Note that this is static so that we can register it with the database.
	static CStandardDataRecord* Create();
	virtual BOOL Init(DATAID id, DATATYPE type, CStandardDataManager* pManager, LPVOID pData, ST_DEV_POSITION* in_where);

	// The destructor.
	virtual ~CKeywordRecord();

	// Return the keyword name.
	const CString& Name(void) const
		{ return m_csName; }

/////////////////////////////////////////////////////////////////////////////
// Item management.

	// Return the number of items having this keyword.
	int Items(void)
		{ return m_Items.count(); }

	// Return a item.
	DATAID Item(int nItem)
		{ return m_Items.get(nItem); }

	// Add a item.
	void AddItem(DATAID lItem);

	// Remove a item.
	void RemoveItem(DATAID lItem);

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	virtual ERRORCODE ReadData(StorageDevicePtr pDevice);

/*
// WriteData()
//
// Write the record.
*/

	virtual ERRORCODE WriteData(StorageDevicePtr pDevice);

/*
// SizeofData()
//
// Return the size of the record.
*/

	virtual ST_MAN_SIZE SizeofData(StorageDevicePtr pDevice);

protected:

	// Data!
	CString m_csName;
	RecordArray m_Items;

	// Record for any other data.
	struct
	{
		DWORD m_dwType;
	} m_KRecord;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CKeywordRecord();

/*
// Assign method.
*/

	virtual void Assign(const CDataRecord& Record);
};

/////////////////////////////////////////////////////////////////////////////
// The keyword array record.

class CKeywordArrayRecord : public CStandardDataRecord
{
public:

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static CStandardDataRecord* Create();
	virtual BOOL Init(DATAID id, DATATYPE type, CStandardDataManager* pManager, LPVOID pData, ST_DEV_POSITION* in_where);

/*
// The destructor.
*/

	virtual ~CKeywordArrayRecord();

#if 0
	// Add an item to keywords. (NOT GENERALLY USEFUL ANYMORE.)
	// This is a generic entry which does not do keyword back attachment.
	// See alternate AddItemToKeywords() with CFileItemData* below.
	void AddItemToKeywords(DATAID lItem, LPCSTR pszKeyword);
#endif

	// Add an item to keywords.
	// This method will back attach the file item to the keywords.
	void AddItemToKeywords(CFileItemData* pItem, LPCSTR pszKeyword);

	// Add a keyword.
	// The keyword is returned and must be Released.
	CKeywordRecord* AddKeyword(LPCSTR pszKeyword);

	// Get a keyword.
	// The keyword must be Released.
	CKeywordRecord* GetKeyword(LPCSTR pszKeyword);

	// Delete a keyword.
	void DeleteKeyword(LPCSTR pszKeyword);

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	virtual ERRORCODE ReadData(StorageDevicePtr pDevice);

/*
// WriteData()
//
// Write the record.
*/

	virtual ERRORCODE WriteData(StorageDevicePtr pDevice);

/*
// SizeofData()
//
// Return the size of the record.
*/

	virtual ST_MAN_SIZE SizeofData(StorageDevicePtr pDevice);

protected:

	// Data!
	CSortedKeywordArray m_Keywords;

	// Record for any other data.
	struct
	{
		DWORD m_dwFlags;
	} m_KRecord;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CKeywordArrayRecord();

/*
// Assign method.
*/

	virtual void Assign(const CDataRecord& Record);
};

// General utilities.
void FixupKeyword(CString& csWord);
void GetKeywords(LPCSTR pszKeywords, CMlsStringArray& Keywords);

#endif
