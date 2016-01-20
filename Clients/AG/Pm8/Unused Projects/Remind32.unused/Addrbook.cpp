/*
// $Workfile: Addrbook.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/Addrbook.cpp $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:04a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:20   Fred
// Initial revision.
// 
//    Rev 1.3   28 Feb 1997 10:46:48   Fred
// Fixed FindStringExact() loop
// 
//    Rev 1.2   26 Feb 1997 13:52:46   Fred
// Fixed some problems with sender books
// 
//    Rev 1.1   25 Feb 1997 09:53:36   Fred
// Support for sender events
// 
//    Rev 1.0   20 Feb 1997 13:55:40   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "remind.h"
#include "sortarry.h"
#include "util.h"
#include "addrbook.h"
#include "pmwcfg.h"

#include <io.h>

//////////////////////////////////////////////////////////////////////////
// CAddressBook

CAddressBook::CAddressBook()
{
	m_fIsSenderDatabase = FALSE;
	m_pDatabaseFile = NULL;
	m_pDatabase = NULL;
	m_pIdCursor = NULL;
	m_pNameCursor = NULL;
	m_fIsStandardFormat = FALSE;
	m_fHasNameFields = FALSE;
	m_dwIdField = (DWORD)-1;
	m_dwFirstNameField = (DWORD)-1;
	m_dwLastNameField = (DWORD)-1;
	m_dwFullNameField = (DWORD)-1;
}

CAddressBook::~CAddressBook()
{
	CloseAddressBook();
}
	
ERRORCODE CAddressBook::OpenAddressBook(LPCSTR pszUserName, BOOL fIsSenderDatabase)
{
	ERRORCODE Error = ERRORCODE_None;
	
	// If we already have this address book open, then we're done.
	if ((m_csUserName.CompareNoCase(pszUserName) != 0) || (m_fIsSenderDatabase != fIsSenderDatabase))
	{
		// We are switching, close the current address book.
		CloseAddressBook();
		
		TRY
		{
			// Save the address book names.
			m_csUserName = pszUserName;
			GetAddressBookFullName(m_csUserName, m_csFullName, fIsSenderDatabase);
			m_fIsSenderDatabase = fIsSenderDatabase;
			
			// Open up the address book database.
			Error = CAddressBook::OpenAddressBookDatabase(m_csUserName, m_pDatabaseFile, m_pDatabase, m_fIsSenderDatabase);
			if (Error == ERRORCODE_None)
			{
				// Create a cursor on the database which allows us to search by ID.
				m_pIdCursor = new CFlatFileDatabaseCursor;
				m_pIdCursor->Attach(m_pDatabase);
				m_pIdCursor->SetIndex("__ID__");
		
				// Figure the fields that will be used for generating the names.
				m_dwIdField = (DWORD)-1;
				m_dwFirstNameField = (DWORD)-1;
				m_dwLastNameField = (DWORD)-1;
				m_dwFullNameField = (DWORD)-1;
					
				m_fIsStandardFormat = FALSE;
				m_fHasNameFields = FALSE;
			
				// Get the ID field... it MUST exist.
				m_dwIdField = m_pIdCursor->GetFieldNumberFromName("__ID__");
				if (m_dwIdField != (DWORD)-1)
				{
					// Get the database's User Data. The names of the important naming fields are stored there.
					LPVOID pData = NULL;
					DWORD dwSize;
					m_pIdCursor->GetUserData(&pData, &dwSize);
					if (pData != NULL)
					{
						// Parse the user data into separate strings.
						CMlsStringArray csaData((LPCSTR)pData);
			
						// Free the data that was allocated.
						CFlatFileDatabase::FreeMemory(pData);
			
						// Field #0 is type of database. Check if it is "normal".
						if (csaData.GetSize() > 0)
						{
							CString csStandardDescription;
							csStandardDescription.LoadString(m_fIsSenderDatabase ? IDS_STANDARD_SENDER_DATABASE_DESCRIPTION : IDS_STANDARD_DATABASE_DESCRIPTION);
							if (csaData.ElementAt(0) == csStandardDescription)
							{
								m_fIsStandardFormat = TRUE;
							}
						}
			
						// Field #1 is the name of the first name field.
						if (csaData.GetSize() > 1)
						{
							m_dwFirstNameField = m_pIdCursor->GetFieldNumberFromName(csaData.ElementAt(1));
						}
			
						// Field #2 is the name of the last name field.
						if (csaData.GetSize() > 2)
						{
							m_dwLastNameField = m_pIdCursor->GetFieldNumberFromName(csaData.ElementAt(2));
						}
			
						// Field #3 is the name of the full name field.
						if (csaData.GetSize() > 3)
						{
							m_dwFullNameField = m_pIdCursor->GetFieldNumberFromName(csaData.ElementAt(3));
						}
			
						// We need the ID field and both a first name and a last name fields or a full name field.
						ASSERT(m_dwIdField != (DWORD)-1);
						if (m_dwIdField != (DWORD)-1)
						{
							if (((m_dwFirstNameField != (DWORD)-1) && (m_dwLastNameField != (DWORD)-1))
							 || (m_dwFullNameField != (DWORD)-1))
							{
								// Get the main field to use for name retrieval.
								DWORD dwField = m_dwLastNameField;
								if (dwField == (DWORD)-1)
								{
									dwField = m_dwFirstNameField;
									if (dwField == (DWORD)-1)
									{
										dwField = m_dwFullNameField;
									}
								}
									
								ASSERT(dwField != (DWORD)-1);
								if (dwField != (DWORD)-1)
								{
									// We know we have valid name field information.
									m_fHasNameFields = TRUE;
									
									// Check if there is an index associated with the main name field.
									// If so, then we can create a cursor on the index to retrieve
									// names in sorted order. If there is no index, then the names will
									// be retrieved in "ID" order.
									DWORD dwIndex;
									Error = m_pDatabase->GetIndexOfMainField(&dwIndex, dwField);
									if (Error == ERRORCODE_None)
									{
									   ASSERT(dwIndex != (DWORD)-1);
									   if (dwIndex != (DWORD)-1)
									   {
									   	// Create a new cursor for retrieving entries in name order.
									   	m_pNameCursor = new CFlatFileDatabaseCursor;
											m_pNameCursor->Attach(m_pDatabase);
											m_pNameCursor->SetIndex(dwIndex);
										}
									}
									
									// If there was no index, that's OK.
									if (Error == ERRORCODE_DoesNotExist)
									{
										Error = ERRORCODE_None;
									}
								}
							}
						}
					}
				}
					
				if (m_pIdCursor == NULL)
				{
					if (Error == ERRORCODE_None)
					{
						Error = ERRORCODE_Access;
					}
				}
			}
		}
		CATCH_ALL(e)
		{
			if (Error == ERRORCODE_None)
			{
				Error = ERRORCODE_IntError;
			}
		}
		END_CATCH_ALL
		
		// If there was an error, clear cached address book name to indicate that
		// the address book was not successfully opened.
		if (Error != ERRORCODE_None)
		{
			m_csUserName.Empty();
		}
	}
	
	return Error;
}

void CAddressBook::CloseAddressBook(void)
{
	CloseAddressBookDatabase(m_pDatabaseFile, m_pDatabase);
	
	m_csUserName.Empty();
	m_csFullName.Empty();
	m_fIsSenderDatabase = FALSE;
	m_pDatabaseFile = NULL;
	m_pDatabase = NULL;
	delete m_pIdCursor;
	m_pIdCursor = NULL;
	delete m_pNameCursor;
	m_pNameCursor = NULL;
	m_fIsStandardFormat = FALSE;
	m_fHasNameFields = FALSE;
	m_dwIdField = (DWORD)-1;
	m_dwFirstNameField = (DWORD)-1;
	m_dwLastNameField = (DWORD)-1;
	m_dwFullNameField = (DWORD)-1;
}

BOOL CAddressBook::GetNameFromId(DWORD dwId, CString& csName)
{
	TRY
	{
		csName.Empty();
		
		// Check for special "All" id.
		if (dwId == (DWORD)-1)
		{
			csName.LoadString(IDS_ALL_ENTRIES_NAME);
		}
		else
		{
			CFlatFileDatabaseCursor* pCursor = GetIdCursor();
			if (pCursor != NULL)
			{
				// Get pointers to the fields we will be examining.
				if (GetIdField() != (DWORD)-1)
				{
					CFlatFileDatabaseField* pIdField = pCursor->GetField(GetIdField());
					if (pIdField != NULL)
					{
						// Search to the specified record.							
						pIdField->SetAsUnsigned32(dwId);
							
						BOOL fExact = FALSE;
						pCursor->Find(&fExact);
						if (fExact)
						{
							// Get the name from the current record.
							GetNameFromCursor(pCursor, csName);
						}
					}
				}
			}
		}
	}
	END_TRY

	return !csName.IsEmpty();
}

BOOL CAddressBook::GetNameFromCursor(CFlatFileDatabaseCursor* pCursor, CString& csName)
{
	TRY
	{
		csName.Empty();

		if (pCursor != NULL)
		{		
			// Get pointers to the fields we will be examining.
			CFlatFileDatabaseField* pFirstNameField = NULL;
			CFlatFileDatabaseField* pLastNameField = NULL;
			CFlatFileDatabaseField* pFullNameField = NULL;
								
			if (GetFirstNameField() != (DWORD)-1)
			{
				pFirstNameField = pCursor->GetField(GetFirstNameField());
			}
			if (GetLastNameField() != (DWORD)-1)
			{
				pLastNameField = pCursor->GetField(GetLastNameField());
			}
			if (GetFullNameField() != (DWORD)-1)
			{
				pFullNameField = pCursor->GetField(GetFullNameField());
			}

			// Get the name.
			if (pFirstNameField != NULL)
			{
				csName = pFirstNameField->GetAsString();
				if (pLastNameField != NULL)
				{
					csName += ' ';
					csName += pLastNameField->GetAsString();
				}
			}
			else if (pFullNameField != NULL)
			{
				csName = pFullNameField->GetAsString();
			}
		
			Util::Trim(csName);
		}
	}
	END_TRY
	
	return !csName.IsEmpty();
}

// Get the full name of an address book given its user name.
// CMemoryException
void CAddressBook::GetAddressBookFullName(const CString& csUserName, CString& csFullName, BOOL fIsSenderDatabase)
{
	CString csTrimmedUserName;
	TRY
	{
		csTrimmedUserName = csUserName;
		Util::Trim(csTrimmedUserName);
		csFullName = fIsSenderDatabase ? SenderBookDirectory : AddressBookDirectory;
		csFullName += csTrimmedUserName;
		csFullName += ADDRESS_BOOK_EXTENSION;
	}
	CATCH_ALL(e)
	{
		// Some sort of error. Return an empty string.
		TRY
		{
			csFullName.Empty();
		}
		END_TRY

		THROW_LAST();
	}
	END_CATCH_ALL;
}

// Get the user name of an address book given its full name.
// CMemoryException
void CAddressBook::GetAddressBookUserName(const CString& csFullName, CString& csUserName)
{
	TRY
	{
		Util::SplitPath(csFullName, NULL, NULL, &csUserName, NULL);
		Util::Trim(csUserName);
	}
	CATCH_ALL(e)
	{
		// Some sort of error. Return an empty string.
		TRY
		{
			csUserName.Empty();
		}
		END_TRY

		THROW_LAST();
	}
	END_CATCH_ALL;
}

// Get an array of the user names of the available address books.
// No exceptions.
BOOL CAddressBook::GetAddressBookUserNames(CStringArray& csaUserNames)
{
	CFileIterator FileIterator;
	CString csSearch;
	CString csFullName;
	CString csUserName;
	TRY
	{
		csaUserNames.RemoveAll();

		// Build the search path.
		csSearch = AddressBookDirectory;
		csSearch += "*";
		csSearch += ADDRESS_BOOK_EXTENSION;

		// Search for the first file.
		BOOL fContinue = FileIterator.FindFirst(csSearch, _A_NORMAL);
		while (fContinue)
		{
			csFullName = FileIterator.Name();
			Util::SplitPath(csFullName, NULL, NULL, &csUserName, NULL);
			if (!csUserName.IsEmpty())
			{
				csaUserNames.Add(csUserName);
			}
			fContinue = FileIterator.FindNext();
		}
	}
	END_TRY

	return csaUserNames.GetSize() > 0;
}

// Get an array of the user names of the available sender books.
// No exceptions.
BOOL CAddressBook::GetSenderBookUserNames(CStringArray& csaUserNames)
{
	CFileIterator FileIterator;
	CString csSearch;
	CString csFullName;
	CString csUserName;
	TRY
	{
		csaUserNames.RemoveAll();

		// Build the search path.
		csSearch = SenderBookDirectory;
		csSearch += "*";
		csSearch += ADDRESS_BOOK_EXTENSION;

		// Search for the first file.
		BOOL fContinue = FileIterator.FindFirst(csSearch, _A_NORMAL);
		while (fContinue)
		{
			csFullName = FileIterator.Name();
			Util::SplitPath(csFullName, NULL, NULL, &csUserName, NULL);
			if (!csUserName.IsEmpty())
			{
				csaUserNames.Add(csUserName);
			}
			fContinue = FileIterator.FindNext();
		}
	}
	END_TRY

	return csaUserNames.GetSize() > 0;
}

// Check if any address books exist.
// No exceptions.
BOOL CAddressBook::AddressBooksExist(void)
{
	BOOL fResult = FALSE;

	CFileIterator FileIterator;
	CString csSearch;
	TRY
	{
		// Build the search path.
		csSearch = AddressBookDirectory;
		csSearch += "*";
		csSearch += ADDRESS_BOOK_EXTENSION;

		// Search for the first file.
		fResult = FileIterator.FindFirst(csSearch, _A_NORMAL);
	}
	END_TRY

	return fResult;
}

// Check if any address books exist.
// No exceptions.
BOOL CAddressBook::SenderBooksExist(void)
{
	BOOL fResult = FALSE;

	CFileIterator FileIterator;
	CString csSearch;
	TRY
	{
		// Build the search path.
		csSearch = SenderBookDirectory;
		csSearch += "*";
		csSearch += ADDRESS_BOOK_EXTENSION;

		// Search for the first file.
		fResult = FileIterator.FindFirst(csSearch, _A_NORMAL);
	}
	END_TRY

	return fResult;
}

// Open an address book database.
// No exceptions.
ERRORCODE CAddressBook::OpenAddressBookDatabase(LPCSTR pszUserName, StorageFile*& pDatabaseFile, CFlatFileDatabase*& pDatabase, BOOL fIsSenderDatabase)
{
	ERRORCODE Error = ERRORCODE_None;

	// Clean up any existing data.
	CloseAddressBookDatabase(pDatabaseFile, pDatabase);

	CString csUserName;
	CString csFullName;
	CString csPath;
	TRY
	{
		// Make sure the directory exists.
		csUserName = pszUserName;
		GetAddressBookFullName(csUserName, csFullName, fIsSenderDatabase);
		Util::SplitPath(csFullName, &csPath, NULL);
		Util::MakeDirectory(csPath);
			
		// Create the file object.
		pDatabaseFile = new StorageFile(csFullName);
	
		// Create the database object.
		pDatabase = new CFlatFileDatabase(pDatabaseFile);
	
		// Try to open the database exclusively and perform validation.
		pDatabaseFile->set_open_flags(CFile::modeReadWrite | CFile::shareExclusive);
		Error = pDatabase->read_state(TRUE, TRUE);
		pDatabaseFile->flush();
	
		if (Error == ERRORCODE_None)
		{
			// Try to open the database in sharing mode.
			pDatabaseFile->set_open_flags(CFile::modeReadWrite | CFile::shareDenyNone);
			if ((Error = pDatabase->read_state()) == ERRORCODE_None)
			{
				Error = pDatabase->Attach();
			}
		}
	}
	CATCH_ALL(e)
	{
		if (Error == ERRORCODE_None)
		{
			Error = ERRORCODE_IntError;
		}
	}
	END_CATCH_ALL

	if (Error != ERRORCODE_None)
	{
		CloseAddressBookDatabase(pDatabaseFile, pDatabase);
	}

	return Error;
}

// Close an address book database.
// No exceptions.
void CAddressBook::CloseAddressBookDatabase(StorageFile*& pDatabaseFile, CFlatFileDatabase*& pDatabase)
{
	// Close and delete the database object.
	TRY
	{
		if (pDatabase != NULL)
		{
			pDatabase->flush();
			pDatabase->write_state(TRUE);
			delete pDatabase;
		}
	}
	END_TRY

	// Close and delete the file object.
	TRY
	{
		delete pDatabaseFile;
	}
	END_TRY

	// Always NULL the pointers.
	pDatabase = NULL;
	pDatabaseFile = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookListBox

CAddressBookListBox::CAddressBookListBox()
{
}

CAddressBookListBox::~CAddressBookListBox()
{
}

BEGIN_MESSAGE_MAP(CAddressBookListBox, CListBox)
	//{{AFX_MSG_MAP(CAddressBookListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAddressBookListBox::Refresh(LPCSTR pszSelectName /*=NULL*/, BOOL fIsSenderDatabase /*=FALSE*/)
{
	SetRedraw(FALSE);
	ResetContent();

	CStringArray csaUserNames;
	int nName;
	TRY
	{
		// Add address book names.
		CAddressBook::GetAddressBookUserNames(csaUserNames);
		for (nName = 0; nName  < csaUserNames.GetSize(); nName++)
		{
			int nIndex = AddString(csaUserNames.GetAt(nName));
			if (nIndex != LB_ERR)
			{
				// Clear "sender database" flag.
				SetItemData(nIndex, 0);
			}
		}
		
		// Add sender book names.
		CAddressBook::GetSenderBookUserNames(csaUserNames);
		for (nName = 0; nName  < csaUserNames.GetSize(); nName++)
		{
			int nIndex = AddString(csaUserNames.GetAt(nName));
			if (nIndex != LB_ERR)
			{
				// Set "sender database" flag.
				SetItemData(nIndex, 1);
			}
		}
	}
	END_TRY
	
	// Try to select the name which the caller specified.
	if ((pszSelectName != NULL) && (*pszSelectName != '\0'))
	{
		int nStartAfter = -1;
		for(;;)
		{
			int nResult = FindStringExact(nStartAfter, pszSelectName);
			if ((nResult == LB_ERR) || (nResult <= nStartAfter))
			{
				// Not found.
				break;
			}
			
			if ((BOOL)GetItemData(nResult) == fIsSenderDatabase)
			{
				// Found it.
				SetCurSel(nResult);
				break;
			}
			
			// Keep searching.
			nStartAfter = nResult;
		}
	}

	SetRedraw(TRUE);
	Invalidate();
}

BOOL CAddressBookListBox::GetUserName(CString& csUserName, BOOL& fIsSenderDatabase, int nIndex /*=-1*/)
{
	BOOL fResult = FALSE;

	csUserName.Empty();

	TRY
	{
		if (nIndex == -1)
		{
			nIndex = GetCurSel();
		}

		if (nIndex != LB_ERR)
		{
			// Get the user name from the list box.
			GetText(nIndex, csUserName);
			Util::Trim(csUserName);
			
			// Get the "sender database" flag.
			fIsSenderDatabase = (BOOL)GetItemData(nIndex);
			
			fResult = TRUE;
		}
	}
	CATCH_ALL(e)
	{
		csUserName.Empty();
		fResult = FALSE;
	}
	END_CATCH_ALL

	return fResult;
}

BOOL CAddressBookListBox::GetFullName(CString& csFullName, BOOL& fIsSenderDatabase, int nIndex /*=-1*/)
{
	BOOL fResult = FALSE;

	csFullName.Empty();

	CString csUserName;
	TRY
	{
		if (nIndex == -1)
		{
			nIndex = GetCurSel();
		}

		if (nIndex != LB_ERR)
		{
			// Get the user name from the list box.
			GetText(nIndex, csUserName);
			Util::Trim(csUserName);

			// Get the "sender database" flag.
			fIsSenderDatabase = (BOOL)GetItemData(nIndex);
			
			// Get the full name from the user name.
			CAddressBook::GetAddressBookFullName(csUserName, csFullName, fIsSenderDatabase);

			fResult = TRUE;
		}
	}
	CATCH_ALL(e)
	{
		csFullName.Empty();
		fResult = FALSE;
	}
	END_CATCH_ALL

	return fResult;
}


/////////////////////////////////////////////////////////////////////////////
// CNoAddressBooksDialog dialog

CNoAddressBooksDialog::CNoAddressBooksDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNoAddressBooksDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNoAddressBooksDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CNoAddressBooksDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNoAddressBooksDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNoAddressBooksDialog, CDialog)
	//{{AFX_MSG_MAP(CNoAddressBooksDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNoAddressBooksDialog message handlers

BOOL CNoAddressBooksDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetConfiguration()->ReplaceDialogText(this);
	
	CenterWindow();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
