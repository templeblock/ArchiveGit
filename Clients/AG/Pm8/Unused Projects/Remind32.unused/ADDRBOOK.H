/*
// $Workfile: ADDRBOOK.H $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/ADDRBOOK.H $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:42   Fred
// Initial revision.
// 
//    Rev 1.2   26 Feb 1997 13:52:44   Fred
// Fixed some problems with sender books
// 
//    Rev 1.1   25 Feb 1997 09:54:24   Fred
// Support for sender events
// 
//    Rev 1.0   20 Feb 1997 13:56:10   Fred
// Initial revision.
*/

class CAddressBook;
class CAddressBookListBox;
class CNoAddressBooksDialog;

#ifndef __ADDRBOOK_H__
#define __ADDRBOOK_H__

#include "flatdb.h"

#define ADDRESS_BOOK_EXTENSION	".ABK"

//////////////////////////////////////////////////////////////////////////
// CAddressBook

class CAddressBook
{
public:
	CAddressBook();
	virtual ~CAddressBook();
	
public:
	ERRORCODE OpenAddressBook(LPCSTR pszUserName, BOOL fIsSenderDatabase);
	void CloseAddressBook(void);
	
public:
	BOOL GetNameFromId(DWORD dwId, CString& csName);
	BOOL GetNameFromCursor(CFlatFileDatabaseCursor* pCursor, CString& csName);
	const CString& GetUserName(void) const
		{ return m_csUserName; }
	const CString& GetFullName(void) const
		{ return m_csFullName; }
	BOOL GetIsSenderDatabase(void) const
		{ return m_fIsSenderDatabase; }
	CFlatFileDatabaseCursor* GetIdCursor(void) const
		{ return m_pIdCursor; }
	CFlatFileDatabaseCursor* GetNameCursor(void) const
		{ return m_pNameCursor; }
	BOOL GetIsStandardFormat(void) const
		{ return m_fIsStandardFormat; }
	BOOL GetHasNameFields(void) const
		{ return m_fHasNameFields; }
	DWORD GetIdField(void) const
		{ return m_dwIdField; }
	DWORD GetFirstNameField(void) const
		{ return m_dwFirstNameField; }
	DWORD GetLastNameField(void) const
		{ return m_dwLastNameField; }
	DWORD GetFullNameField(void) const
		{ return m_dwFullNameField; }
	
public:
	// Address book support functions.
	static void GetAddressBookFullName(const CString& csUserName, CString& csFullName, BOOL fIsSenderDatabase);
	static void GetAddressBookUserName(const CString& csFullName, CString& csUserName);
	static BOOL GetAddressBookUserNames(CStringArray& csaUserNames);
	static BOOL GetSenderBookUserNames(CStringArray& csaUserNames);
	static BOOL AddressBooksExist(void);
	static BOOL SenderBooksExist(void);
	static ERRORCODE OpenAddressBookDatabase(LPCSTR pszUserName, StorageFile*& pDatabaseFile, CFlatFileDatabase*& pDatabase, BOOL fIsSenderDatabase);
	static void CloseAddressBookDatabase(StorageFile*& pDatabaseFile, CFlatFileDatabase*& pDatabase);
	
protected:
	CString m_csUserName;
	CString m_csFullName;
	BOOL m_fIsSenderDatabase;
	StorageFile* m_pDatabaseFile;
	CFlatFileDatabase* m_pDatabase;
	CFlatFileDatabaseCursor* m_pIdCursor;
	CFlatFileDatabaseCursor* m_pNameCursor;
	BOOL m_fIsStandardFormat;	
	BOOL m_fHasNameFields;
	DWORD m_dwIdField;
	DWORD m_dwFirstNameField;
	DWORD m_dwLastNameField;
	DWORD m_dwFullNameField;
};

/////////////////////////////////////////////////////////////////////////////
// CAddressBookListBox

class CAddressBookListBox : public CListBox
{
	INHERIT(CAddressBookListBox, CListBox)

// Construction
public:
	CAddressBookListBox();
	virtual ~CAddressBookListBox();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddressBookListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void Refresh(LPCSTR pszSelectName, BOOL fIsSenderDatabase);
	BOOL GetUserName(CString& csUserName, BOOL& fIsSenderDatabase, int nIndex = -1);
	BOOL GetFullName(CString& csFullName, BOOL& fIsSenderDatabase, int nIndex = -1);
};

/////////////////////////////////////////////////////////////////////////////
// CNoAddressBooksDialog dialog

class CNoAddressBooksDialog : public CDialog
{
// Construction
public:
	CNoAddressBooksDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNoAddressBooksDialog)
	enum { IDD = IDD_NO_ADDRESS_BOOKS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CNoAddressBooksDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
