/*
// $Workfile: DBTABS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/DBTABS.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:30   Fred
// Initial revision.
// 
//    Rev 1.5   31 Jul 1996 11:56:22   Fred
// 16-bit compatibility
// 
//    Rev 1.4   06 Jul 1996 09:25:20   Fred
// Picture editor and other improvements
// 
//    Rev 1.3   01 Jul 1996 07:30:34   Fred
// Receiver list
// 
//    Rev 1.2   18 Jun 1996 15:20:40   Fred
// Field value list management and editing
// 
//    Rev 1.1   18 Jun 1996 08:24:56   Fred
// More adress book changes
// 
//    Rev 1.0   10 Jun 1996 08:22:30   Fred
// Initial revision.
*/

class CDatabaseTabs;

#ifndef __DBTABS_H__
#define __DBTABS_H__

#include "flatdb.h"

#ifdef WIN32
#include <afxcmn.h>
#endif

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "addrbook.h"

//////////////////////////////////////////////////////////////////////////
// CTabDescriptionBase
//
// The base class for an object used to describe a tab. This information
// is kept separate from the tab control object itself.

class CTabDescriptionBase
{
public:
	CTabDescriptionBase(LPCSTR pszName, DWORD dwNumber);
	virtual ~CTabDescriptionBase();

public:
	LPCSTR GetName(void)
		{ return m_csName; }
	DWORD GetNumber(void)
		{ return m_dwNumber; }

protected:
	CString m_csName;		// the "name" that actually appears on the tab
	DWORD m_dwNumber;		// the sequence number of the first record associated with the tab
};

//////////////////////////////////////////////////////////////////////////
// CTabDescriptionString
//
// The object used to describe a tab associated with a string field.

class CTabDescriptionString : public CTabDescriptionBase
{
public:
	CTabDescriptionString(LPCSTR pszName, DWORD dwNumber, LPCSTR pszValue);
	virtual ~CTabDescriptionString();

public:
	LPCSTR GetValue(void)
		{ return m_csValue; }

protected:
	CString m_csValue;	// the full string value associated with the tab
};

//////////////////////////////////////////////////////////////////////////
// CTabDescriptionUnsigned32
//
// The object used to describe a tab associated with an unsigned 32-bit number.

class CTabDescriptionUnsigned32 : public CTabDescriptionBase
{
public:
	CTabDescriptionUnsigned32(LPCSTR pszName, DWORD dwNumber, DWORD dwValue, BOOL fEmpty);
	virtual ~CTabDescriptionUnsigned32();

public:
	DWORD GetValue(void)
		{ return m_dwValue; }
	BOOL GetEmpty(void)
		{ return m_fEmpty; }

protected:
	DWORD m_dwValue;		// the 32-bit unsigned value associated with the tab
	BOOL m_fEmpty;
};

//////////////////////////////////////////////////////////////////////////
// CTabDescriptionSigned32
//
// The object used to describe a tab associated with a signed 32-bit number.

class CTabDescriptionSigned32 : public CTabDescriptionBase
{
public:
	CTabDescriptionSigned32(LPCSTR pszName, DWORD dwNumber, long lValue, BOOL fEmpty);
	virtual ~CTabDescriptionSigned32();

public:
	long GetValue(void)
		{ return m_lValue; }
	BOOL GetEmpty(void)
		{ return m_fEmpty; }

protected:
	long m_lValue;			// the 32-bit signed value associated with the tab
	BOOL m_fEmpty;
};

//////////////////////////////////////////////////////////////////////////
// CTabDescriptionArray
//
// An array of CTabDescriptionBase (or derived) objects.

class CTabDescriptionArray : public CPtrArray
{
public:
	CTabDescriptionArray();
	virtual ~CTabDescriptionArray();

public:
	void RemoveAll();
	CTabDescriptionBase* GetAt(int nIndex) const
		{ return (CTabDescriptionBase*)(CPtrArray::GetAt(nIndex)); }
	CTabDescriptionBase*& ElementAt(int nIndex)
		{ return (CTabDescriptionBase*&)(CPtrArray::ElementAt(nIndex)); }
	void RemoveAt(int nIndex, int nCount = 1, BOOL fDelete = TRUE);
	CTabDescriptionBase* operator[](int nIndex) const
		{ return (CTabDescriptionBase*)(CPtrArray::GetAt(nIndex)); }
	CTabDescriptionBase*& operator[](int nIndex)
		{ return (CTabDescriptionBase*&)(CPtrArray::ElementAt(nIndex)); }
};

//////////////////////////////////////////////////////////////////////////
// CDatabaseTabsGeneratorBase
//
// The base class from which all tab generators are derived.

class CDatabaseTabsGeneratorBase
{
public:
	CDatabaseTabsGeneratorBase(CFlatFileDatabaseCursor* pCursor, CAddressBookTabControl* pTabs);
	virtual ~CDatabaseTabsGeneratorBase();

protected:
	void CreateSearchCursor(void);
	void FreeSearchCursor(void);

public:
	// rebuild all tab descriptions from database
	virtual void UpdateTabs(void);

	// select the tab which represents the record at the current cursor position
	virtual void UpdateCurrentTab(void);

	// move the cursor to the first record represented by a given tab
	virtual void UpdateCurrentRecord(int nTabIndex);

	// return the index of the tab that represents the record at the current cursor position
	virtual int CurrentRecordTabIndex(void);

	// Return a pointer to the current tab descriptions.
	virtual CTabDescriptionArray* GetTabDescriptions(void)
		{ return &m_Descriptions; }

protected:
	// virtual functions called by UpdateTabs()
	virtual BOOL UpdateTabsStart(void);
	virtual void SetKeyToEmpty(void) = 0;
	virtual BOOL KeyGreaterThanMaximum(void) = 0;
	virtual void MakeSearchKey(void) = 0;
	virtual void FindNearest(void) = 0;
	virtual BOOL GetValue(void) = 0;
	virtual BOOL SearchGreaterThanValue(void) = 0;
	virtual BOOL SearchEqualToValue(void) = 0;
	virtual LPCSTR KeyName(void) = 0;
	virtual LPCSTR LessName(void) = 0;
	virtual LPCSTR GreaterName(void) = 0;
	virtual void Add(LPCSTR pszName);
	virtual CTabDescriptionBase* Description(LPCSTR pszName) = 0;
	virtual void IncrementKey(void) = 0;
	virtual void SetKeyToValue(void) = 0;
	virtual void SetKeyToMinimum(void) = 0;
	virtual BOOL ValueLessThanMinimum(void) = 0;
	virtual BOOL ValueGreaterThanMaximum(void) = 0;
	virtual void UpdateTabsFinish(void);

	// member variables used by UpdateTabs()
	DWORD m_dwSequenceNumber;
	CString m_csKeyName;
	CString m_csLessName;
	CString m_csGreaterName;

protected:
	// virtual functions called by UpdateCurrentRecord()
	virtual void SetSearchField(CFlatFileDatabaseField* pField, int nTabIndex) = 0;

protected:
	// virtual functions called by CurrentRecordTabIndex()
	virtual void GetFieldValue(CFlatFileDatabaseField* pField) = 0;
	virtual BOOL CompareFieldValue(int nDescription) = 0;

protected:
	// main database cursor
	CFlatFileDatabaseCursor* m_pDataCursor;

	// dynamically allocated second cursor for searching
	CFlatFileDatabaseCursor* m_pSearchCursor;

	// pointer to main search field
	CFlatFileDatabaseField* m_pSearchField;

	// tab control
	CAddressBookTabControl* m_pTabs;

	// tab descriptions
	CTabDescriptionArray m_Descriptions;

	// set the text in the tab control tabs to the name strings in the descriptions
	virtual void SetTabNames(void);

	// check if the tab names have already been set (used for redraw optimization)
	virtual BOOL TabNamesEqual(void);
};

//////////////////////////////////////////////////////////////////////////
// CDatabaseTabsGeneratorAlpha
//
// The tab generator class for the Alpha1, Alpha2, ... format types.

class CDatabaseTabsGeneratorAlpha : public CDatabaseTabsGeneratorBase
{
public:
	CDatabaseTabsGeneratorAlpha(CFlatFileDatabaseCursor* pCursor, CAddressBookTabControl* pTabs, UINT nLength);
	virtual ~CDatabaseTabsGeneratorAlpha();

protected:
	// virtual functions called by UpdateTabs()
	virtual BOOL UpdateTabsStart(void);
	virtual void SetKeyToEmpty(void);
	virtual BOOL KeyGreaterThanMaximum(void);
	virtual void MakeSearchKey(void);
	virtual void FindNearest(void);
	virtual BOOL GetValue(void);
	virtual BOOL SearchGreaterThanValue(void);
	virtual BOOL SearchEqualToValue(void);
	virtual LPCSTR KeyName(void);
	virtual LPCSTR LessName(void);
	virtual LPCSTR GreaterName(void);
	virtual CTabDescriptionBase* Description(LPCSTR pszName);
	virtual void IncrementKey(void);
	virtual void SetKeyToValue(void);
	virtual void SetKeyToMinimum(void);
	virtual BOOL ValueLessThanMinimum(void);
	virtual BOOL ValueGreaterThanMaximum(void);
	virtual void UpdateTabsFinish(void);

	// helper functions used by the above functions
	void SetKeyToSearchMinimum(void);

	// member variables used by UpdateTabs()
	CString m_csKey;
	CString m_csSearch;
	CString m_csValue;

protected:
	// virtual functions called by UpdateCurrentRecord()
	virtual void SetSearchField(CFlatFileDatabaseField* pField, int nTabIndex);

protected:
	// virtual functions called by CurrentRecordTabIndex()
	virtual void GetFieldValue(CFlatFileDatabaseField* pField);
	virtual BOOL CompareFieldValue(int nDescription);

	// member variables used by CurrentRecordTabIndex()
	CString m_csFieldValue;

protected:
	UINT m_nLength;	// "length" of alpha format
};

//////////////////////////////////////////////////////////////////////////
// CDatabaseTabsGeneratorAlphaList
//
// The tab generator class for the AlphaList format types.

class CDatabaseTabsGeneratorAlphaList : public CDatabaseTabsGeneratorBase
{
public:
	CDatabaseTabsGeneratorAlphaList(CFlatFileDatabaseCursor* pCursor, CAddressBookTabControl* pTabs, CSortedPtrArray* pList);
	virtual ~CDatabaseTabsGeneratorAlphaList();

protected:
	// virtual functions called by UpdateTabs()
	virtual BOOL UpdateTabsStart(void);
	virtual void SetKeyToEmpty(void);
	virtual BOOL KeyGreaterThanMaximum(void);
	virtual void MakeSearchKey(void);
	virtual void FindNearest(void);
	virtual BOOL GetValue(void);
	virtual BOOL SearchGreaterThanValue(void);
	virtual BOOL SearchEqualToValue(void);
	virtual LPCSTR KeyName(void);
	virtual LPCSTR LessName(void);
	virtual LPCSTR GreaterName(void);
	virtual CTabDescriptionBase* Description(LPCSTR pszName);
	virtual void IncrementKey(void);
	virtual void SetKeyToValue(void);
	virtual void SetKeyToMinimum(void);
	virtual BOOL ValueLessThanMinimum(void);
	virtual BOOL ValueGreaterThanMaximum(void);
	virtual void UpdateTabsFinish(void);

	// helper functions used by the above functions
	int GetStringIndex(LPCSTR pszString);

	// member variables used by UpdateTabs()
	int m_nKey;
	CString m_csSearch;
	CString m_csValue;

protected:
	// virtual functions called by UpdateCurrentRecord()
	virtual void SetSearchField(CFlatFileDatabaseField* pField, int nTabIndex);

protected:
	// virtual functions called by CurrentRecordTabIndex()
	virtual void GetFieldValue(CFlatFileDatabaseField* pField);
	virtual BOOL CompareFieldValue(int nDescription);

	// member variables used by CurrentRecordTabIndex()
	CString m_csFieldValue;

protected:
	CSortedPtrArray* m_pList;	// strings in list
};

//////////////////////////////////////////////////////////////////////////
// CDatabaseTabsGeneratorIndexList
//
// The tab generator class for the IndexList format types.

class CDatabaseTabsGeneratorIndexList : public CDatabaseTabsGeneratorAlphaList
{
public:
	CDatabaseTabsGeneratorIndexList(CFlatFileDatabaseCursor* pCursor, CAddressBookTabControl* pTabs, CFlatFileValueListManager* pListManager, CFlatFileIndexDescription* pIndex);
	virtual ~CDatabaseTabsGeneratorIndexList();

protected:
	// virtual functions called by UpdateTabs()
	virtual BOOL UpdateTabsStart(void);

protected:
	CFlatFileValueListManager* m_pListManager;	// list manager
	CFlatFileIndexDescription* m_pIndex;			// index description
};

//////////////////////////////////////////////////////////////////////////
// CDatabaseTabsGeneratorZip
//
// The tab generator class for the Zip1, Zip2, ... format types.

class CDatabaseTabsGeneratorZip : public CDatabaseTabsGeneratorBase
{
public:
	CDatabaseTabsGeneratorZip(CFlatFileDatabaseCursor* pCursor, CAddressBookTabControl* pTabs, UINT nLength);
	virtual ~CDatabaseTabsGeneratorZip();

protected:
	// virtual functions called by UpdateTabs()
	virtual BOOL UpdateTabsStart(void);
	virtual void SetKeyToEmpty(void);
	virtual BOOL KeyGreaterThanMaximum(void);
	virtual void MakeSearchKey(void);
	virtual void FindNearest(void);
	virtual BOOL GetValue(void);
	virtual BOOL SearchGreaterThanValue(void);
	virtual BOOL SearchEqualToValue(void);
	virtual LPCSTR KeyName(void);
	virtual LPCSTR LessName(void);
	virtual LPCSTR GreaterName(void);
	virtual CTabDescriptionBase* Description(LPCSTR pszName);
	virtual void IncrementKey(void);
	virtual void SetKeyToValue(void);
	virtual void SetKeyToMinimum(void);
	virtual BOOL ValueLessThanMinimum(void);
	virtual BOOL ValueGreaterThanMaximum(void);
	virtual void UpdateTabsFinish(void);

	// member variables used by UpdateTabs()
	CString m_csKey;
	CString m_csSearch;
	CString m_csValue;

protected:
	// virtual functions called by UpdateCurrentRecord()
	virtual void SetSearchField(CFlatFileDatabaseField* pField, int nTabIndex);

protected:
	// virtual functions called by CurrentRecordTabIndex()
	virtual void GetFieldValue(CFlatFileDatabaseField* pField);
	virtual BOOL CompareFieldValue(int nDescription);

	// member variables used by CurrentRecordTabIndex()
	CString m_csFieldValue;

protected:
	UINT m_nLength;	// "length" of zip format
};

//////////////////////////////////////////////////////////////////////////
// CDatabaseTabs
//
// A class that wraps a given generator class to hide the face that
// changing field types changes the underlying generator object.

class CDatabaseTabs
{
public:
	CDatabaseTabs(CFlatFileDatabaseCursor* pCursor, CAddressBookTabControl* pTabs);
	virtual ~CDatabaseTabs();

public:
	// set the format of the tabs
	void SetAlphaFormat(UINT nLength);
	void SetAlphaListFormat(CSortedPtrArray* pList);
	void SetIndexListFormat(CFlatFileValueListManager* pListManager, CFlatFileIndexDescription* pIndex);
	void SetNumFormat(UINT nStart, UINT nEnd, UINT nIncrement);
	void SetZipFormat(UINT nLength);

	// rebuild all tab descriptions from database
	virtual void UpdateTabs(void);

	// select the tab which represents the record at the current cursor position
	virtual void UpdateCurrentTab(void);

	// move the cursor to the first record represented by a given tab
	virtual void UpdateCurrentRecord(int nTabIndex);

	// return the index of the tab that represents the record at the current cursor position
	virtual int CurrentRecordTabIndex(void);

	// Return a pointer to the current tab descriptions.
	virtual CTabDescriptionArray* GetTabDescriptions(void);

protected:
	// main database cursor
	CFlatFileDatabaseCursor* m_pDataCursor;

	// tab control
	CAddressBookTabControl* m_pTabs;

	// allocated generator
	CDatabaseTabsGeneratorBase* m_pGenerator;

	// free any currently allocated generator
	void FreeGenerator(void);
};

#endif
