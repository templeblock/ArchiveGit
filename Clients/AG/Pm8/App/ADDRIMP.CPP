/*
// $Workfile: ADDRIMP.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:01p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/ADDRIMP.CPP $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 3     9/21/98 5:25p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     1/19/98 2:09p Fredf
// Removed 16-bit code.
// 
//    Rev 1.0   14 Aug 1997 15:18:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:04   Fred
// Initial revision.
// 
//    Rev 1.20   24 Jan 1997 12:26:10   Fred
// Some bug fixes associated with field and index name access
// 
//    Rev 1.19   15 Nov 1996 17:27:26   johno
// Accomodate address book strings from resource DLL
// 
//    Rev 1.18   05 Nov 1996 16:15:18   johno
// Moved strings to resource DLL
// 
//    Rev 1.17   05 Nov 1996 14:06:40   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.16   07 Sep 1996 17:33:32   Fred
// New import code
// 
//    Rev 1.15   01 Sep 1996 17:28:14   Fred
// Changed some dialogs from CPmwDialog to CDialog becuase of missing sounds
// 
//    Rev 1.14   28 Aug 1996 21:26:22   Fred
// CAddressBookImportError::Throw() is no longer inline
// 
//    Rev 1.13   28 Aug 1996 15:22:44   Fred
// Enforces maximum number of names
// 
//    Rev 1.12   15 Aug 1996 15:55:44   Fred
// 256 color wizard bitmaps
// 
//    Rev 1.11   09 Aug 1996 16:50:14   Fred
// More 16-bit fixes
// 
//    Rev 1.10   05 Aug 1996 18:55:30   Fred
//  
// 
//    Rev 1.9   01 Aug 1996 09:22:30   Fred
// More 16-bit changes
// 
//    Rev 1.8   31 Jul 1996 11:57:12   Fred
// 16-bit compatibility
// 
//    Rev 1.7   23 Jul 1996 08:06:24   Fred
// Trims address book names
// 
//    Rev 1.6   17 Jul 1996 16:39:24   Fred
// Uses addrdata; compresses pictures
// 
//    Rev 1.5   17 Jul 1996 12:56:56   Jay
// Speed ups. New 'exclusive' algorithm
// 
//    Rev 1.4   16 Jul 1996 08:51:22   Fred
// Hooked up address/sender book
// 
//    Rev 1.3   11 Jul 1996 08:37:32   Fred
// More address book features
// 
//    Rev 1.2   09 Jul 1996 16:21:22   Fred
// Import names feature
// 
//    Rev 1.1   08 Jul 1996 07:14:10   Fred
// Import Names Wizard
// 
//    Rev 1.0   05 Jul 1996 09:47:00   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "util.h"
#include "file.h"
#include "flatdb.h"
#include "addrdata.h"
#include "addrbook.h"
#include "addrimp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CAddressBookImportError, CException);

extern CPalette *pOurPal;		// THE PrintMaster Palette (from PMWVIEW.CPP)

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportError
// Exception class for reporting errors.

void CAddressBookImportError::Throw(DWORD dwError)
{
	THROW(new CAddressBookImportError(dwError));
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportReader
// Class for reading a file of names to be imported.

CAddressBookImportReader::CAddressBookImportReader()
{
	m_pFile = NULL;
	m_dwRecordCount = 0;
	m_pStatistics = NULL;
	m_dwValueOffset = (DWORD)-1;
}

CAddressBookImportReader::~CAddressBookImportReader()
{
	if (m_pFile != NULL)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	delete [] m_pStatistics;
}

void CAddressBookImportReader::Init(LPCSTR pszFileName)
{
	TRY
	{
		m_csFileName = pszFileName;

		if ((m_pFile = fopen(pszFileName, "rb")) == NULL)
		{
			CAddressBookImportError::Throw(ERROR_CantAccessFile);
		}

		// Read the first element and see what the separator is. This should
		// tell us which reader to use.
		int nSeparatorFound = 0;
		ReadElement('\t', nSeparatorFound);
		if (nSeparatorFound != '\t')
		{
			// Not a tab, use the comma separator.
			m_nSeparator = ',';
		}
		else
		{
			// Use the tab seprator.
			m_nSeparator = '\t';
		}

		// Analyze the file contents.
		CollectStatistics();
	}
	CATCH_ALL(e)
	{
		// Something has gone wrong... terribly wrong...
		if (m_pFile != NULL)
		{
			fclose(m_pFile);
			m_pFile = NULL;
		}

		// All class methods return only CAddressBookImportError exceptions.
		if (e->IsKindOf(RUNTIME_CLASS(CAddressBookImportError)))
		{
			THROW_LAST();
		}
		else
		{
			CAddressBookImportError::Throw(ERROR_CantAccessFile);
		}
	}
	END_CATCH_ALL
}

void CAddressBookImportReader::SetAddressBookName(LPCSTR pszAddressBookName)
{
	TRY
	{
		m_csAddressBookName = pszAddressBookName;
		Util::SplitPath(m_csAddressBookName, NULL, NULL, &m_csAddressBookBaseName, NULL);
	}
	END_TRY
}

BOOL CAddressBookImportReader::ImportNames(CWnd* pParent)
{
	BOOL fResult = FALSE;

	// Put up the progress dialog.
	CImportingNamesProgressDialog ProgressDialog(this, pParent);

	// Create the new address book.
	CString csDrive;
	CString csDirectory;
	CString csFile;
	CString csExtension;
	CString csDatabaseName;
	StorageFile* pDatabaseFile = NULL;
	CFlatFileDatabase* pDatabase = NULL;
	CFlatFileDatabaseCursor* pDatabaseCursor = NULL;
	CString csDatabaseDescription;
	CFlatFileFieldDescription* pFieldDescriptions = NULL;
	CFlatFileIndexDescription* pIndexDescriptions = NULL;

	TRY
	{
		// Get temporary name.
		Util::SplitPath(m_csAddressBookName, &csDrive, &csDirectory, &csFile, &csExtension);
		Util::ConstructPath(csDatabaseName, csDrive, csDirectory, csFile, ".$$$");
		if (Util::FileExists(csDatabaseName))
		{
			CFile::Remove(csDatabaseName);
		}

		// Decide which files to use for naming.
		ChooseNameFields();

		// Build the database description.
		// JRO csDatabaseDescription = "PrintMaster Gold Imported Address Book Database v1.0";
      csDatabaseDescription = GET_PMWAPP()->GetResourceStringPointer (IDS_PMG_ADDR_BOOK);
		csDatabaseDescription += '|';
		csDatabaseDescription += m_csFirstNameField;
		csDatabaseDescription += '|';
		csDatabaseDescription += m_csLastNameField;
		csDatabaseDescription += '|';
		csDatabaseDescription += m_csFullNameField;
		csDatabaseDescription += '|';

		// Build the field descriptions.
		pFieldDescriptions = new CFlatFileFieldDescription[1+GetFieldCount()+1];

		// Initialize the ID field.
      pFieldDescriptions[0].m_idName = 0;
		pFieldDescriptions[0].m_pszName = "__ID__";		// Field Name
		pFieldDescriptions[0].m_dwType = FFT_Counter32;	// Field Type
		pFieldDescriptions[0].m_dwUserData = FNOEDIT;	// Field User Data
		pFieldDescriptions[0].m_dwSize = 4;					// Field Size

		// Initialize the imported fields.
		for (DWORD dwField = 1; dwField <= GetFieldCount(); dwField++)
		{
	      pFieldDescriptions[dwField].m_idName = 0;
			pFieldDescriptions[dwField].m_pszName = GetFieldName(dwField-1);
			pFieldDescriptions[dwField].m_dwType = FFT_String;
			pFieldDescriptions[dwField].m_dwUserData = FNORMAL;
			pFieldDescriptions[dwField].m_dwSize = GetImportFieldLength(dwField-1)+1;
		}

		// Initialize the end marker.
      pFieldDescriptions[dwField].m_idName = 0;
		pFieldDescriptions[dwField].m_pszName = NULL;
		pFieldDescriptions[dwField].m_dwType = 0;
		pFieldDescriptions[dwField].m_dwUserData = 0;
		pFieldDescriptions[dwField].m_dwSize = 0;

		// Build the Index descriptions.
		pIndexDescriptions = new CFlatFileIndexDescription[1+GetFieldCount()+1];

		// Initialize the ID index.
      pIndexDescriptions[0].m_idName = 0;
		pIndexDescriptions[0].m_pszName = "__ID__";			// Index Name
		pIndexDescriptions[0].m_dwFlags = 0;					// Index Flags
		pIndexDescriptions[0].m_dwUserData = INODISPLAY;	// Index User Data
      pIndexDescriptions[0].m_idField = 0;
		pIndexDescriptions[0].m_pszField = "__ID__";			// Field Names (separated by '|')

		// Initialize the imported indicies.
		for (DWORD dwIndex = 1; dwIndex <= GetFieldCount(); dwIndex++)
		{
		   pIndexDescriptions[dwIndex].m_idName = 0;
			pIndexDescriptions[dwIndex].m_pszName = GetFieldName(dwIndex-1);
			pIndexDescriptions[dwIndex].m_dwFlags = 0;
			pIndexDescriptions[dwIndex].m_dwUserData = GetImportFieldIndexType(dwIndex-1);
		   pIndexDescriptions[dwIndex].m_idField = 0;
			pIndexDescriptions[dwIndex].m_pszField = GetFieldName(dwIndex-1);
		}

		// Initialize the end marker.
	   pIndexDescriptions[dwIndex].m_idName = 0;
		pIndexDescriptions[dwIndex].m_pszName = NULL;
		pIndexDescriptions[dwIndex].m_dwFlags = 0;
		pIndexDescriptions[dwIndex].m_dwUserData = 0;
	   pIndexDescriptions[dwIndex].m_idField = 0;
		pIndexDescriptions[dwIndex].m_pszField = NULL;

		// Open the database.
		if (CAddressBook::OpenAddressBookDatabase(csDatabaseName, pDatabaseFile, pDatabase, TRUE, csDatabaseDescription, pFieldDescriptions, pIndexDescriptions) != ERRORCODE_None)
		{
			CAddressBookImportError::Throw(ERROR_CantAccessAddressBook);
		}

		// Create a cursor on the database.
		pDatabaseCursor = new CFlatFileDatabaseCursor;
		pDatabaseCursor->Attach(pDatabase);
		pDatabaseCursor->SetIndex((DWORD)0);

		// Make sure field counts match up.
		DWORD dwFieldCount = pDatabaseCursor->GetFieldCount();
		ASSERT(dwFieldCount == GetFieldCount()+1);
		if (dwFieldCount == GetFieldCount()+1)
		{
			// Seek to the place to read the values.
			if (fseek(m_pFile, m_dwValueOffset, SEEK_SET) != 0)
			{
				CAddressBookImportError::Throw(ERROR_CantAccessFile);
			}

			// Import each record.
			for (DWORD dwRecord = 0; dwRecord < GetRecordCount(); dwRecord++)
			{
				// Check for user abort.
				if (ProgressDialog.CheckForAbort())
				{
					CAddressBookImportError::Throw(ERROR_UserAbort);
				}

				// Read the values for the field.
				ReadFieldValues();

				// Create the new record.
				pDatabaseCursor->New();
				pDatabaseCursor->SetCounters(TRUE);
				for (DWORD dwField = 1; dwField < dwFieldCount; dwField++)
				{
					pDatabaseCursor->GetField(dwField)->SetAsString(m_FieldValues.GetAt((int)(dwField-1)));
				}

				// Insert the record into the database.
				pDatabaseCursor->Insert();

				// Update the progress dialog.
				ProgressDialog.SetPosition(dwRecord+1);
			}
		}

		// Delete the cursor object.
		delete pDatabaseCursor;
		pDatabaseCursor = NULL;

		// Close and delete the database object.
		if (pDatabase != NULL)
		{
			pDatabase->flush();
			pDatabase->write_state(TRUE);
			delete pDatabase;
			pDatabase = NULL;
		}

		// Close and delete the file object.
		delete pDatabaseFile;
		pDatabaseFile = NULL;

		// Delete the old address book if it exists.
		TRY
		{
			CFile::Remove(GetAddressBookName());
		}
		END_TRY

		// Rename the new address book to the real name.
		CFile::Rename(csDatabaseName, GetAddressBookName());

		// Everything worked!
		fResult = TRUE;
	}
	CATCH_ALL(e)
	{
		delete [] pIndexDescriptions;
		delete [] pFieldDescriptions;
		csDatabaseDescription.Empty();
		delete pDatabaseCursor;
		delete pDatabase;
		delete pDatabaseFile;

		TRY
		{
			CFile::Remove(csDatabaseName);
		}
		END_TRY

		csDatabaseName.Empty();
		csExtension.Empty();
		csExtension.Empty();
		csDirectory.Empty();
		csDrive.Empty();

		if (e->IsKindOf(RUNTIME_CLASS(CAddressBookImportError)))
		{
			THROW_LAST();
		}
		else
		{
			CAddressBookImportError::Throw(ERROR_CantAccessAddressBook);
		}
	}
	END_CATCH_ALL

	delete [] pIndexDescriptions;
	delete [] pFieldDescriptions;
	delete pDatabaseCursor;
	delete pDatabase;
	delete pDatabaseFile;

	return fResult;
}

CAddressBookImportReader::CFieldStatistics* CAddressBookImportReader::GetFieldStatistics(DWORD dwFieldNumber)
{
	CFieldStatistics* pStatistics = NULL;

	ASSERT(dwFieldNumber < GetFieldCount());
	if (dwFieldNumber < GetFieldCount())
	{
		ASSERT(m_pStatistics != NULL);
		if (m_pStatistics != NULL)
		{
			pStatistics = &(m_pStatistics[dwFieldNumber]);
		}
	}

	return pStatistics;
}

LPCSTR CAddressBookImportReader::GetFieldName(DWORD dwFieldNumber)
{
	LPCSTR pszName = NULL;

	ASSERT(dwFieldNumber < GetFieldCount());
	if (dwFieldNumber < GetFieldCount())
	{
		pszName = m_FieldNames.ElementAt((int)dwFieldNumber);
	}

	return pszName;
}

DWORD CAddressBookImportReader::GetImportFieldLength(DWORD dwFieldNumber)
{
	DWORD dwLength = 0;

	// Figure out a good choice for the imported field length.
	ASSERT(dwFieldNumber < GetFieldCount());
	if (dwFieldNumber < GetFieldCount())
	{
		CFieldStatistics* pStatistics = GetFieldStatistics(dwFieldNumber);
		ASSERT(pStatistics != NULL);
		if (pStatistics != NULL)
		{
			dwLength = pStatistics->GetMaximumLength();
			dwLength = dwLength*2;
			if (dwLength < 15)
			{
				dwLength = 15;
			}
		}
	}

	return dwLength;
}

DWORD CAddressBookImportReader::GetImportFieldIndexType(DWORD dwFieldNumber)
{
	return IALPHA1;
}

void CAddressBookImportReader::ReadFieldNames(void)
{
	TRY
	{
		ReadLine(m_FieldNames);
	}
	CATCH_ALL(e)
	{
		if (e->IsKindOf(RUNTIME_CLASS(CAddressBookImportError)))
		{
			THROW_LAST();
		}
		else
		{
			CAddressBookImportError::Throw(ERROR_UnknownFormat);
		}
	}
	END_CATCH_ALL
}

BOOL CAddressBookImportReader::ReadFieldValues(void)
{
	BOOL fResult = FALSE;

	TRY
	{
		if (ReadLine(m_FieldValues))
		{
			// Fill in any missing fields with blank values.
			for (DWORD dwValue = GetValueCount(); dwValue < GetFieldCount(); dwValue++)
			{
				m_FieldValues.Add("");
			}

			// Clip any extra values.
			if (GetValueCount() > GetFieldCount())
			{
				m_FieldValues.SetSize((int)GetFieldCount());
			}

			ASSERT(GetValueCount() == GetFieldCount());

			fResult = TRUE;
		}
	}
	CATCH_ALL(e)
	{
		if (e->IsKindOf(RUNTIME_CLASS(CAddressBookImportError)))
		{
			THROW_LAST();
		}
		else
		{
			CAddressBookImportError::Throw(ERROR_UnknownFormat);
		}
	}
	END_CATCH_ALL

	return fResult;
}

void CAddressBookImportReader::ReadElement(int nSeparator, int& nSeparatorFound)
{
	BOOL fInQuotes = FALSE;
	BOOL fFinishElement = FALSE;
	int nTrimLength = 0;
	int nElementLength = 0;
	m_szElement[0] = '\0';
	nSeparatorFound = '\0';

	int c = EOF;

	for (;;)
	{
		// Get the next character, filter out carriage returns.
		do
		{
			c = getc(m_pFile);
		} while (c == '\r');

		// Check for a quote.
		if (c == '"')
		{
			// Got a quote, if this is the first character, then start quoting.
			if (!fInQuotes)
			{
				// We found a first quote. If it's the first character, then
				// assume the element value is enclosed in quotes.
				if (nElementLength == 0)
				{
					// The element is enclosed in quotes.
					fInQuotes = TRUE;

					// Continue reading with the next character.
					continue;
				}
			}
			else
			{
				// We are in quotes and we have another quote. Check for
				// two quotes, otherwise it must be an ending quote.

				// Get the next character after the quote, filter out carriage returns.
				do
				{
					c = getc(m_pFile);
				} while (c == '\r');

				if (c != '"')
				{
					// We have a situation where the element is enclosed
					// in quotes, we've found the ending quote, and we've
					// read the character after it. Process the character
					// normally.
					fInQuotes = FALSE;
					fFinishElement = TRUE;
				}

				// We have two consecutive quote characters in a quoted element.
				// Turn the two quotes into one by processing only the second one.
			}
		}

		// Check for end of file.
		if (c == EOF)
		{
			// If we reached the end of file before reaching the ending quote,
			// then we have a bad format.
			if (fInQuotes)
			{
				CAddressBookImportError::Throw(ERROR_UnknownFormat);
			}

			// The EOF terminates the current element.
			break;
		}

		// We have a character to process. If the character is not
		// quoted, then check for separators.
		if (!fInQuotes)
		{
			// Check for new lines.
			if (c == '\n')
			{
				// Found the end of line (not quoted), this terminates the element.
				break;
			}

			// Check for specific separators.
			if (c == nSeparator)
			{
				// Found a separator.
				break;
			}
		}

		// Not a separator. Change strange characters which aren't supported
		// by PrintMaster's fill-in fields into spaces.
		if (c < ' ')
		{
			c = ' ';
		}

		// Check if the character should be considered white space. Quoted elements
		// always contain all characters including white space. White space is trimmed
		// from the start and end of unquoted elements.
		BOOL fIsWhite = !fInQuotes && (c == ' ');

		if (fFinishElement && !fIsWhite)
		{
			break;
		}

		// Add the character to the element.
		if (!fIsWhite || (nElementLength > 0))
		{
			if (nElementLength > MAXIMUM_ELEMENT_LENGTH)
			{
				// Element is a ridiculous length, so we probably have a bad format.
				CAddressBookImportError::Throw(ERROR_UnknownFormat);
			}

			// Save the character and advance the length;
			m_szElement[nElementLength++] = (char)c;

			// If the character is not white, then adjust the trim length
			// to ensure the character is not trimmed.
			if (!fIsWhite)
			{
				nTrimLength = nElementLength;
			}
		}
	}

	// Trim the element if necessary.
	m_szElement[nTrimLength] = '\0';

	// Remember the element separator.
	nSeparatorFound = c;
}

BOOL CAddressBookImportReader::ReadLine(CStringArray& StringArray)
{
	// Clear out all the strings.
	StringArray.RemoveAll();

	// Read elements until we reach the end or encounter an error.
	for (;;)
	{
		// Read the next element.
		int nSeparatorFound;
		ReadElement(m_nSeparator, nSeparatorFound);

		// Save the element.
		StringArray.Add(m_szElement);

		// If we read an EOF separator, then return FALSE.
		if (nSeparatorFound == EOF)
		{
			return FALSE;
		}

		// If we found a new line separator, then return TRUE;
		if (nSeparatorFound == '\n')
		{
			return TRUE;
		}

		// If we didn't find the separator we were looking for,
		// then throw a fit.
		if (nSeparatorFound != m_nSeparator)
		{
			ASSERT(FALSE);
			CAddressBookImportError::Throw(ERROR_UnknownFormat);
		}

		// Read the next element.
	}
}

void CAddressBookImportReader::CollectStatistics(void)
{
	// This may take a while...
	HCURSOR hWaitCursor = AfxGetApp()->LoadStandardCursor(IDC_WAIT);
	HCURSOR hOldCursor = NULL;
	if (hWaitCursor!= NULL)
	{
		hOldCursor = ::SetCursor(hWaitCursor);
	}

	TRY
	{
		// Delete any current statistics.
		delete [] m_pStatistics;
		m_pStatistics = NULL;

		// First, read the field names.
		if (fseek(m_pFile, 0, SEEK_SET) != 0)
		{
			CAddressBookImportError::Throw(ERROR_CantAccessFile);
		}

		ReadFieldNames();

		// Make sure we have some fields.
		if (GetFieldCount() == 0)
		{
			CAddressBookImportError::Throw(ERROR_UnknownFormat);
		}

		// Make sure each field names is not empty.
		for (DWORD dwField = 0; dwField < GetFieldCount(); dwField++)
		{
			if (m_FieldNames.ElementAt((int)dwField).IsEmpty())
			{
				CAddressBookImportError::Throw(ERROR_BlankFieldNames);
			}
		}

		// Allocate the array of objects to record the field statistics.
		m_pStatistics = new CFieldStatistics[GetFieldCount()];

		// Save the file offset of the start of the value lines.
		if ((m_dwValueOffset = ftell(m_pFile)) == (DWORD)-1L)
		{
			CAddressBookImportError::Throw(ERROR_CantAccessFile);
		}

		// Run through the value lines and collect statistics.
		m_dwRecordCount = 0;
		while (ReadFieldValues())
		{
			// Make sure there are not too many names.
			if (m_dwRecordCount >= CAddressBook::MAXIMUM_NAMES)
			{
				BOOL fReadMaximumNames = TRUE;

				CString csFormat;
				CString csText;
				TRY
				{
					csFormat.LoadString(IDS_IMPORT_MAXIMUM_NAMES);
					csText.Format(csFormat, CAddressBook::MAXIMUM_NAMES);
					if (AfxMessageBox(csText, MB_YESNO) != IDYES)
					{
						fReadMaximumNames = FALSE;
					}
				}
				END_TRY

				if (!fReadMaximumNames)
				{
					CAddressBookImportError::Throw(ERROR_TooManyImportNames);
				}

				break;
			}

			// Analyze the record.
			m_dwRecordCount++;

			for (DWORD dwValue = 0; dwValue < GetValueCount(); dwValue++)
			{
				DWORD dwLength = m_FieldValues.ElementAt((int)dwValue).GetLength();
				m_pStatistics[dwValue].AdjustMaximumLength(dwLength);
				m_pStatistics[dwValue].AdjustMinimumLength(dwLength);
				m_pStatistics[dwValue].AdjustEmptyCount(dwLength);
			}
		}
	}
	CATCH_ALL(e)
	{
		if (hOldCursor != NULL)
		{
			::SetCursor(hOldCursor);
		}

		// All base class methods return only CAddressBookImportError exceptions.
		if (e->IsKindOf(RUNTIME_CLASS(CAddressBookImportError)))
		{
			THROW_LAST();
		}
		else
		{
			CAddressBookImportError::Throw(ERROR_UnknownFormat);
		}
	}
	END_CATCH_ALL

	if (hOldCursor != NULL)
	{
		::SetCursor(hOldCursor);
	}
}

void CAddressBookImportReader::ChooseNameFields(void)
{
	TRY
	{
		// Clear any existing values.
		m_csFirstNameField.Empty();
		m_csLastNameField.Empty();
		m_csFullNameField.Empty();

		// Try to find a first name field.
		DWORD dwFirstName = FindNameField(
										"FIRST NAME",
										"FIRST_NAME",
										"FNAME",
										"NAME FIRST",
										"NAME_FIRST",
										"NAMEF",
										"FIRST",
										(DWORD)0);

		// Try to find a last name field.
		DWORD dwLastName = FindNameField(
										"LAST NAME",
										"LAST_NAME",
										"LNAME",
										"NAME LAST",
										"NAME_LAST",
										"NAMEL",
										"LAST",
										(DWORD)0);

		// Try to find a full name field.
		DWORD dwFullName = FindNameField(
										"FULL NAME",
										"FULL_NAME",
										"CONTACT",
										"NAME",
										"FULL",
										(DWORD)0);

		// See how we did. If we don't have a first/last pair and/or a full name,
		// then we need to choose some fields based on other criteria.
		if (((dwFirstName == (DWORD)-1) || (dwLastName == (DWORD)-1)) && (dwFullName == (DWORD)-1))
		{
			DWORD dwField1 = (DWORD)-1;
			DWORD dwScore1 = 0;
			DWORD dwField2 = (DWORD)-1;
			DWORD dwScore2 = 0;

			// We score each field based on it's length and the percentage of non-empty values.
			for (DWORD dwField = 0; dwField < GetFieldCount(); dwField++)
			{
				// Get statistics for field.
				CFieldStatistics* pStatistics = GetFieldStatistics(dwField);

				// Score 0-1000 for percentage of non-empty records.
				DWORD dwScore = 1000;
				if (GetRecordCount() > 0)
				{
					dwScore -= (pStatistics->GetEmptyCount()*1000)/GetRecordCount();
				}

				// Adjust score for length. Penalize fields which are "non-name" lengths.
				if (pStatistics->GetMaximumLength() < 3)
				{
					dwScore /= 2;
				}

				// Record high scores.
				if (dwScore > dwScore1)
				{
					dwField2 = dwField1;
					dwScore2 = dwScore1;
					dwField1 = dwField;
					dwScore1 = dwScore;
				}
				else if (dwScore > dwScore2)
				{
					dwField2 = dwField;
					dwScore2 = dwScore;
				}
			}

			// Fill in full name.
			if (dwFullName == (DWORD)-1)
			{
				if (dwField1 != (DWORD)-1)
				{
					dwFullName = dwField1;
					dwField1 = (DWORD)-1;
				}
				else if (dwField2 != (DWORD)-1)
				{
					dwFullName = dwField2;
					dwField2 = (DWORD)-1;
				}
			}

			// Fill in first name.
			if (dwFirstName == (DWORD)-1)
			{
				if (dwField1 != (DWORD)-1)
				{
					dwFirstName = dwField1;
					dwField1 = (DWORD)-1;
				}
				else if (dwField2 != (DWORD)-1)
				{
					dwFirstName = dwField2;
					dwField2 = (DWORD)-1;
				}
			}

			// Fill in last name.
			if (dwLastName == (DWORD)-1)
			{
				if (dwField1 != (DWORD)-1)
				{
					dwLastName = dwField1;
					dwField1 = (DWORD)-1;
				}
				else if (dwField2 != (DWORD)-1)
				{
					dwLastName = dwField2;
					dwField2 = (DWORD)-1;
				}
			}
		}

		// If we only have a first or last name, record it as the full name.
		if (((dwFirstName == (DWORD)-1) || (dwLastName == (DWORD)-1)) && (dwFullName == (DWORD)-1))
		{
			if (dwFirstName != (DWORD)-1)
			{
				dwFullName = dwFirstName;
				dwFirstName = (DWORD)-1;
			}
			else if (dwLastName != (DWORD)-1)
			{
				dwFullName = dwLastName;
				dwLastName = (DWORD)-1;
			}
		}

		// Make sure first and last names are both set or cleared.
		if ((dwFirstName == (DWORD)-1) || (dwLastName == (DWORD)-1))
		{
			dwFirstName = (DWORD)-1;
			dwLastName = (DWORD)-1;
		}

		// If we still don't have the name fields, give up.
		if (((dwFirstName == (DWORD)-1) || (dwLastName == (DWORD)-1)) && (dwFullName == (DWORD)-1))
		{
			CAddressBookImportError::Throw(ERROR_UnknownFormat);
		}

		// Record the names of the chosen fields.
		if (dwFirstName != (DWORD)-1)
		{
			TRY
			{
				m_csFirstNameField = GetFieldName(dwFirstName);
			}
			END_TRY
		}
		if (dwLastName != (DWORD)-1)
		{
			TRY
			{
				m_csLastNameField = GetFieldName(dwLastName);
			}
			END_TRY
		}
		if (dwFullName != (DWORD)-1)
		{
			TRY
			{
				m_csFullNameField = GetFieldName(dwFullName);
			}
			END_TRY
		}
	}
	CATCH_ALL(e)
	{
		// All base class methods return only CAddressBookImportError exceptions.
		if (e->IsKindOf(RUNTIME_CLASS(CAddressBookImportError)))
		{
			THROW_LAST();
		}
		else
		{
			CAddressBookImportError::Throw(ERROR_UnknownFormat);
		}
	}
	END_CATCH_ALL
}

DWORD CAddressBookImportReader::FindNameField(LPCSTR pszName, ...)
{
	DWORD dwResult = (DWORD)-1;
	DWORD dwExact = (DWORD)-1;
	DWORD dwPartial = (DWORD)-1;

	CString csName;
	CString csFieldName;
	TRY
	{
		va_list List;
		va_start(List, pszName);

		while ((DWORD)pszName != 0)
		{
			csName = pszName;
			csName.MakeUpper();

			// Check if the current name matches any of the fields.

			for (DWORD dwField = 0; dwField < GetFieldCount(); dwField++)
			{
				// Don't match any field with more than 10% empty values.
				CFieldStatistics* pStatistics = GetFieldStatistics(dwField);
				if ((GetRecordCount() == 0) || (((pStatistics->GetEmptyCount()*1000)/GetRecordCount()) < 100))
				{
					csFieldName = GetFieldName(dwField);
					csFieldName.MakeUpper();

					// Check for exact match.
					if (csName == csFieldName)
					{
						dwExact = dwField;
						break;
					}

					// Check for partial match.
					if ((dwPartial == -1) && (csFieldName.Find(csName) != -1))
					{
						dwPartial = dwField;
					}
				}
			}

			// If we've found an exact match, we're done.
			if (dwExact != (DWORD)-1)
			{
				break;
			}

			// Get the next name to try.
			pszName = va_arg(List, LPCSTR);
		}

		va_end(List);

		// Return an exact or partial result.
		if (dwExact != (DWORD)-1)
		{
			dwResult = dwExact;
		}
		else if (dwPartial != (DWORD)-1)
		{
			dwResult = dwPartial;
		}
	}
	END_TRY

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportReader::CFieldStatistics
// Class for collecting and managing field statistics.

CAddressBookImportReader::CFieldStatistics::CFieldStatistics()
{
	m_dwMaximumLength = 0;
	m_dwMinimumLength = 0xffffffffL;
	m_dwEmptyCount = 0;
}

CAddressBookImportReader::CFieldStatistics::~CFieldStatistics()
{
}

void CAddressBookImportReader::CFieldStatistics::AdjustMaximumLength(DWORD dwLength)
{
	if (dwLength > m_dwMaximumLength)
	{
		m_dwMaximumLength = dwLength;
	}
}

void CAddressBookImportReader::CFieldStatistics::AdjustMinimumLength(DWORD dwLength)
{
	if (dwLength <m_dwMinimumLength)
	{
		m_dwMinimumLength = dwLength;
	}
}

void CAddressBookImportReader::CFieldStatistics::AdjustEmptyCount(DWORD dwLength)
{
	if (dwLength == 0)
	{
		m_dwEmptyCount++;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportWizard

CAddressBookImportWizard::CAddressBookImportWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPmwWizard(nIDCaption, pParentWnd, iSelectPage)
{
	CommonConstruct();
}

CAddressBookImportWizard::CAddressBookImportWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPmwWizard(pszCaption, pParentWnd, iSelectPage)
{
	CommonConstruct();
}

void CAddressBookImportWizard::CommonConstruct(void)
{
	m_pReader = NULL;
	Util::LoadResourceBitmap(m_Bitmap, "IMPORT", pOurPal);
}

CAddressBookImportWizard::~CAddressBookImportWizard()
{
	TRY
	{
		delete m_pReader;
	}
	END_TRY
}

void CAddressBookImportWizard::InitDialog() 
{
	INHERITED::InitDialog();
	
	CString csTitle;
	TRY
	{
		csTitle.LoadString(IDS_ADDRESS_BOOK_IMPORT_NAMES_WIZARD);
		SetWindowText(csTitle);
	}
	END_TRY
}

BEGIN_MESSAGE_MAP(CAddressBookImportWizard, CPmwWizard)
	//{{AFX_MSG_MAP(CAddressBookImportWizard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAddressBookImportWizard::Run(CWnd* pParent, CString& csAddressBookName)
{
	BOOL fResult = FALSE;

	csAddressBookName.Empty();

	CAddressBookImportWizard Wizard("", pParent, 0);
	CAddressBookImportIntroduction Introduction;
	CAddressBookImportChooseFile ChooseFile;
	CAddressBookImportChooseBook ChooseBook;
	CAddressBookImportImporting Importing;

	Wizard.m_psh.dwFlags &= ~PSH_HASHELP;
	Introduction.m_psp.dwFlags &= ~PSP_HASHELP;
	ChooseFile.m_psp.dwFlags &= ~PSP_HASHELP;
	ChooseBook.m_psp.dwFlags &= ~PSP_HASHELP;
	Importing.m_psp.dwFlags &= ~PSP_HASHELP;
	
	Wizard.AddPage(&Introduction);
	Wizard.AddPage(&ChooseFile);
	Wizard.AddPage(&ChooseBook);
	Wizard.AddPage(&Importing);
	Wizard.SetWizardMode();

	if (Wizard.DoModal() == ID_WIZFINISH)
	{
		if (Wizard.GetReader() != NULL)
		{
			TRY
			{
				csAddressBookName = Wizard.GetReader()->GetAddressBookName();
			}
			END_TRY
		}

		fResult = TRUE;
	}

	return fResult;
}

void CAddressBookImportWizard::CreateReader(LPCSTR pszFileName)
{
	// Delete any current reader.
	TRY
	{
		delete m_pReader;
		m_pReader = NULL;
	}
	END_TRY

	ASSERT(pszFileName != NULL);
	if (pszFileName != NULL)
	{
		// Create and initialize the new reader.
		m_pReader = new CAddressBookImportReader;
		m_pReader->Init(pszFileName);
	}
}

void CAddressBookImportWizard::ReportReaderError(CException* pException)
{
	if (pException->IsKindOf(RUNTIME_CLASS(CAddressBookImportError)))
	{
		switch (((CAddressBookImportError*)pException)->GetError())
		{
			case CAddressBookImportReader::ERROR_UserAbort:
			{
				AfxMessageBox(IDS_IMPORT_USER_ABORT);
				break;
			}
			case CAddressBookImportReader::ERROR_FileNameEmpty:
			{
				AfxMessageBox(IDS_IMPORT_FILE_NAME_EMPTY);
				break;
			}
			case CAddressBookImportReader::ERROR_CantAccessFile:
			{
				AfxMessageBox(IDS_IMPORT_CANT_ACCESS_FILE);
				break;
			}
			case CAddressBookImportReader::ERROR_UnknownFormat:
			{
				AfxMessageBox(IDS_IMPORT_UNKNOWN_FORMAT);
				break;
			}
			case CAddressBookImportReader::ERROR_BlankFieldNames:
			{
				CString csMessage1;
				CString csMessage2;
				TRY
				{
					csMessage1.LoadString(IDS_IMPORT_BLANK_FIELD_NAMES_1);
					csMessage2.LoadString(IDS_IMPORT_BLANK_FIELD_NAMES_2);
					csMessage1 += csMessage2;
					AfxMessageBox(csMessage1);
				}
				END_TRY
				break;
			}
			case CAddressBookImportReader::ERROR_FieldNumberMismatch:
			{
				AfxMessageBox(IDS_IMPORT_FIELD_NUMBER_MISMATCH);
				break;
			}
			case CAddressBookImportReader::ERROR_AddressBookNameEmpty:
			{
				AfxMessageBox(IDS_IMPORT_ADDRESS_BOOK_NAME_EMPTY);
				break;
			}
			case CAddressBookImportReader::ERROR_CantAccessAddressBook:
			{
				AfxMessageBox(IDS_IMPORT_CANT_ACCESS_ADDRESS_BOOK);
				break;
			}
			case CAddressBookImportReader::ERROR_TooManyImportNames:
			{
				CString csFormat;
				CString csText;
				TRY
				{
					csFormat.LoadString(IDS_ADDRESS_BOOK_TOO_MANY_NAMES);
					csText.Format(csFormat, CAddressBook::MAXIMUM_NAMES);
					AfxMessageBox(csText);
				}
				END_TRY
				break;
			}
			default:
			{
				AfxMessageBox(IDS_IMPORT_UNKNOWN_FORMAT);
				break;
			}
		}
	}
	else
	{
		AfxMessageBox(IDS_IMPORT_CANT_PROCESS_FILE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportWizard message handlers

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportWizardPage

CAddressBookImportWizardPage::CAddressBookImportWizardPage(UINT nIDTemplate, UINT nIDCaption /*=0*/) :
	CPmwWizardPage(nIDTemplate, nIDCaption)
{
}

CAddressBookImportWizardPage::CAddressBookImportWizardPage(LPCTSTR lpszTemplateName, UINT nIDCaption /*=0*/) :
	CPmwWizardPage(lpszTemplateName, nIDCaption)
{
}

CAddressBookImportWizardPage::~CAddressBookImportWizardPage()
{
}

BEGIN_MESSAGE_MAP(CAddressBookImportWizardPage, CPmwWizardPage)
	//{{AFX_MSG_MAP(CAddressBookImportWizardPage)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportWizardPage message handlers

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportIntroduction property page

CAddressBookImportIntroduction::CAddressBookImportIntroduction() :
	CAddressBookImportWizardPage(CAddressBookImportIntroduction::IDD)
{
	//{{AFX_DATA_INIT(CAddressBookImportIntroduction)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAddressBookImportIntroduction::~CAddressBookImportIntroduction()
{
}

void CAddressBookImportIntroduction::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddressBookImportIntroduction)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddressBookImportIntroduction, CAddressBookImportWizardPage)
	//{{AFX_MSG_MAP(CAddressBookImportIntroduction)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportIntroduction message handlers

BOOL CAddressBookImportIntroduction::OnSetActive() 
{
	BOOL fResult = INHERITED::OnSetActive();
	if (fResult)
	{
		SetWizardButtons(PSWIZB_NEXT);
	}
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportChooseFile property page

CAddressBookImportChooseFile::CAddressBookImportChooseFile() : CAddressBookImportWizardPage(CAddressBookImportChooseFile::IDD)
{
	//{{AFX_DATA_INIT(CAddressBookImportChooseFile)
	//}}AFX_DATA_INIT
}

CAddressBookImportChooseFile::~CAddressBookImportChooseFile()
{
}

void CAddressBookImportChooseFile::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddressBookImportChooseFile)
	DDX_Control(pDX, IDC_FILE_NAME, m_FileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddressBookImportChooseFile, CAddressBookImportWizardPage)
	//{{AFX_MSG_MAP(CAddressBookImportChooseFile)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportChooseFile message handlers

BOOL CAddressBookImportChooseFile::OnSetActive() 
{
	BOOL fResult = INHERITED::OnSetActive();
	if (fResult)
	{
		SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	}
	return fResult;
}

void CAddressBookImportChooseFile::OnBrowse() 
{
	// MLSTODO: Move strings to resource file.
	CString csDirectory;
	TRY
	{
		m_FileName.GetWindowText(csDirectory);
		Util::Trim(csDirectory);
		CFileDialog Dialog(TRUE, "*.txt", "", OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||", GetParent());
		Dialog.m_ofn.lpstrInitialDir = (LPCSTR)csDirectory;
		if (Dialog.DoModal() == IDOK)
		{
			m_FileName.SetWindowText(Dialog.GetPathName());
		}
	}
	END_TRY
}

LRESULT CAddressBookImportChooseFile::OnWizardNext() 
{
	LRESULT lResult = -1;

	CString csFileName;
	TRY
	{
		// Get the file name.
		m_FileName.GetWindowText(csFileName);
		Util::Trim(csFileName);

		// Check for an empty file name.
		if (csFileName.IsEmpty())
		{
			CAddressBookImportError::Throw(CAddressBookImportReader::ERROR_FileNameEmpty);
		}

		// Create the reader.
		GetWizard()->CreateReader(csFileName);

		// Move to the next page.
		lResult = 0;
	}
	CATCH_ALL(e)
	{
		ReportReaderError(e);
		m_FileName.SetFocus();
		m_FileName.SetSel(0, -1);
	}
	END_CATCH_ALL

	return lResult;
}

BOOL CAddressBookImportChooseFile::OnInitDialog() 
{
	INHERITED::OnInitDialog();

	m_FileName.LimitText(_MAX_PATH);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportChooseBook property page

CAddressBookImportChooseBook::CAddressBookImportChooseBook() : CAddressBookImportWizardPage(CAddressBookImportChooseBook::IDD)
{
	//{{AFX_DATA_INIT(CAddressBookImportChooseBook)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAddressBookImportChooseBook::~CAddressBookImportChooseBook()
{
}

void CAddressBookImportChooseBook::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddressBookImportChooseBook)
	DDX_Control(pDX, IDC_ADDRESS_BOOK_NAME, m_AddressBookName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddressBookImportChooseBook, CAddressBookImportWizardPage)
	//{{AFX_MSG_MAP(CAddressBookImportChooseBook)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportChooseBook message handlers

BOOL CAddressBookImportChooseBook::OnSetActive() 
{
	BOOL fResult = INHERITED::OnSetActive();
	if (fResult)
	{
		SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	
		// Set the default name.
		CString csFileName;
		CString csBaseName;
		TRY
		{
			CAddressBookImportReader* pReader = GetReader();
			ASSERT(pReader != NULL);
			if (pReader != NULL)
			{
				csFileName = pReader->GetFileName();
				Util::SplitPath(csFileName, NULL, NULL, &csBaseName, NULL);
				m_AddressBookName.SetWindowText(csBaseName);
			}
		}
		END_TRY
	}
	
	return fResult;
}

LRESULT CAddressBookImportChooseBook::OnWizardNext() 
{
	LRESULT lResult = -1;

	CString csUserName;
	CString csFullName;
	TRY
	{
		// Get the address book user name.
		m_AddressBookName.GetWindowText(csUserName);
		Util::Trim(csUserName);

		// Check for a valid address book user name.
		if (CAddressBook::ValidateAddressBookUserName(csUserName, TRUE))
		{
			// Get the full name from the user name.
			CAddressBook::GetAddressBookFullName(csUserName, csFullName);

			// Set the address book name.
			GetReader()->SetAddressBookName(csFullName);

			// Check if the address book already exists.
			if ((!Util::FileExists(csFullName))
			 || (AfxMessageBox(IDS_IMPORT_ADDRESS_BOOK_EXISTS, MB_YESNO) == IDYES))
			{
				// Move to the next page.
				lResult = 0;
			}
		}
	}
	CATCH_ALL(e)
	{
		ReportReaderError(e);
	}
	END_CATCH_ALL

	if (lResult != 0)
	{
		// The name was not acceptable, put the focus back onto the name edit.
		m_AddressBookName.SetFocus();
		m_AddressBookName.SetSel(0, -1);
	}

	return lResult;
}

BOOL CAddressBookImportChooseBook::OnInitDialog() 
{
	INHERITED::OnInitDialog();
	
	m_AddressBookName.LimitText(_MAX_PATH);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportImporting property page

CAddressBookImportImporting::CAddressBookImportImporting() : CAddressBookImportWizardPage(CAddressBookImportImporting::IDD)
{
	//{{AFX_DATA_INIT(CAddressBookImportImporting)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAddressBookImportImporting::~CAddressBookImportImporting()
{
}

void CAddressBookImportImporting::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddressBookImportImporting)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddressBookImportImporting, CAddressBookImportWizardPage)
	//{{AFX_MSG_MAP(CAddressBookImportImporting)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportImporting message handlers

BOOL CAddressBookImportImporting::OnSetActive() 
{
	BOOL fResult;

	if (fResult = INHERITED::OnSetActive())
	{
		SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);

		CAddressBookImportReader* pReader = GetReader();

		ASSERT(pReader != NULL);
		if (pReader != NULL)
		{
			DWORD dwCount = pReader->GetRecordCount();

			// Set the title text.
			CString csText;
			TRY
			{
				csText.Format(m_csImportingTitleFormat, dwCount);
			}
			END_TRY
			SetDlgItemText(IDC_IMPORTING_TITLE, csText);

			// Set the "from" name.
			TRY
			{
				csText = pReader->GetFileName();
				SetDlgItemText(IDC_FROM, csText);
			}
			END_TRY

			// Set the "to" name.
			CString csBaseName;
			TRY
			{
				csText = pReader->GetAddressBookName();
				Util::SplitPath(csText, NULL, NULL, &csBaseName, NULL);
				SetDlgItemText(IDC_TO, csBaseName);
			}
			END_TRY
		}
	}

	return fResult;
}

BOOL CAddressBookImportImporting::OnInitDialog() 
{
	INHERITED::OnInitDialog();
	
	TRY
	{
		GetDlgItem(IDC_IMPORTING_TITLE)->GetWindowText(m_csImportingTitleFormat);
	}
	END_TRY

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAddressBookImportImporting::OnWizardFinish() 
{
	BOOL fResult = FALSE;

	TRY
	{
		fResult = GetReader()->ImportNames(GetWizard());
	}
	CATCH_ALL(e)
	{
		ReportReaderError(e);
		fResult = FALSE;
	}
	END_CATCH_ALL

	if (fResult)
	{
		CImportingNamesCongratulationsDialog Dialog(GetReader(), this);
		Dialog.DoModal();
	}

	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CImportingNamesProgressDialog dialog

CImportingNamesProgressDialog::CImportingNamesProgressDialog(CAddressBookImportReader* pReader, CWnd* pParent /*=NULL*/)
	: CDialog(CImportingNamesProgressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportingNamesProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	ASSERT(pReader != NULL);
	m_pReader = pReader;

	m_fAbort = FALSE;

	pParent->EnableWindow(FALSE);
	Create(CImportingNamesProgressDialog::IDD, pParent);
}

CImportingNamesProgressDialog::~CImportingNamesProgressDialog()
{
	if (m_hWnd != NULL)
	{
		DestroyWindow();
	}
}

void CImportingNamesProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportingNamesProgressDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CImportingNamesProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CImportingNamesProgressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CImportingNamesProgressDialog::SetPosition(DWORD dwPosition)
{
	m_ProgressBar.SetPosition((double)dwPosition);
}

BOOL CImportingNamesProgressDialog::CheckForAbort(void)
{
	if (!m_fAbort)
	{
		MSG Msg;
		while (::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!AfxGetApp()->PumpMessage())
			{
				::PostQuitMessage(0);
				m_fAbort = TRUE;
				break;
			}
		}
	}

	return m_fAbort;
}

/////////////////////////////////////////////////////////////////////////////
// CImportingNamesProgressDialog message handlers

BOOL CImportingNamesProgressDialog::OnInitDialog() 
{
	INHERITED::OnInitDialog();
	
	m_ProgressBar.Initialize(IDC_PROGRESS, this);

	// Set the title text.
	ASSERT(m_pReader != NULL);
	if (m_pReader != NULL)
	{
		DWORD dwCount = m_pReader->GetRecordCount();

		// Set progress bar.
		m_ProgressBar.SetRange(0.0, (double)dwCount);
		m_ProgressBar.SetPosition(0.0);

		// Set the title text.
		CString csFormat;
		CString csText;
		TRY
		{
			GetDlgItem(IDC_TITLE)->GetWindowText(csFormat);
			csText.Format(csFormat, dwCount);
		}
		END_TRY
		SetDlgItemText(IDC_TITLE, csText);
	}

	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImportingNamesProgressDialog::OnCancel() 
{
	if (!m_fAbort)
	{
		if (AfxMessageBox(IDS_CONFIRM_CANCEL_IMPORT_NAMES, MB_YESNO) == IDYES)
		{
			m_fAbort = TRUE;
		}
	}
}

BOOL CImportingNamesProgressDialog::DestroyWindow() 
{
	GetParent()->EnableWindow(TRUE);
	return INHERITED::DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CImportingNamesCongratulationsDialog dialog

CImportingNamesCongratulationsDialog::CImportingNamesCongratulationsDialog(CAddressBookImportReader* pReader, CWnd* pParent /*=NULL*/)
	: CDialog(CImportingNamesCongratulationsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportingNamesCongratulationsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pReader = pReader;
}


void CImportingNamesCongratulationsDialog::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportingNamesCongratulationsDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportingNamesCongratulationsDialog, CDialog)
	//{{AFX_MSG_MAP(CImportingNamesCongratulationsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportingNamesCongratulationsDialog message handlers

BOOL CImportingNamesCongratulationsDialog::OnInitDialog() 
{
	INHERITED::OnInitDialog();

	CString csFormat;
	CString csText;

	if (m_pReader != NULL)
	{
		// Set the title text.
		TRY
		{
			GetDlgItem(IDC_TITLE)->GetWindowText(csFormat);
			csText.Format(csFormat, m_pReader->GetRecordCount(), m_pReader->GetAddressBookBaseName());
		}
		END_TRY
		SetDlgItemText(IDC_TITLE, csText);
	
		// Set the instruction text.
		TRY
		{
			GetDlgItem(IDC_INSTRUCTIONS)->GetWindowText(csFormat);
			csText.Format(csFormat, m_pReader->GetAddressBookBaseName());
		}
		END_TRY
		SetDlgItemText(IDC_INSTRUCTIONS, csText);
	}
	
	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAddressBookImportWizardPage::OnEraseBkgnd(CDC* pDC) 
{
	CWnd* pBitmap = GetDlgItem(IDC_BITMAP);
	if (pBitmap != NULL)
	{
		CRect crBitmap;
		pBitmap->GetWindowRect(crBitmap);
		ScreenToClient(crBitmap);
		pDC->ExcludeClipRect(crBitmap);
	}
	return INHERITED::OnEraseBkgnd(pDC);
}
