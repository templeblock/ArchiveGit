/*
// $Workfile: MACROSTD.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/MACROSTD.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 6     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 5     5/18/98 4:03p Cboggio
// Removed dynamic sender field name construction for localized versions
// 
// 4     3/18/98 11:14a Jayn
// Page numbers
// 
// 3     1/28/98 1:04p Johno
// 
// 2     1/27/98 11:54a Johno
// For a version that will compile
// 
//    Rev 1.0   14 Aug 1997 15:22:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:08   Fred
// Initial revision.
// 
//    Rev 1.7   04 Aug 1997 10:19:42   Fred
// Fixed crash from bad address book data.
// 
//    Rev 1.6   13 Mar 1997 13:17:08   Jay
// Changes for 16-bit and 5.0 builds
// 
//    Rev 1.5   31 Jul 1996 11:56:34   Fred
// 16-bit compatibility
// 
//    Rev 1.4   17 Jul 1996 16:38:42   Fred
// Uses addrdata; compresses pictures
// 
//    Rev 1.3   16 Jul 1996 17:30:34   Fred
// More address/sender book stuff
// 
//    Rev 1.2   16 Jul 1996 08:50:54   Fred
// Hooked up address/sender book
// 
//    Rev 1.1   05 Jun 1996 08:21:48   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:32:48   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:20   FRED
// Initial revision.
// 
//    Rev 1.16   27 Sep 1995 14:41:22   FRED
// BDE Thunking
// 
//    Rev 1.15   27 Aug 1995 12:35:22   FRED
// New fill-in-fields stuff
// 
//    Rev 1.14   25 Aug 1995 15:00:50   JAY
// Insert fields dialog
// 
//    Rev 1.13   11 Aug 1995 10:58:58   FRED
// Phil Frank page; macro size limits; move project album text
// 
//    Rev 1.12   09 Aug 1995 17:32:26   FRED
// Does not report record not found errors in some cases.
// 
//    Rev 1.11   09 Aug 1995 13:59:00   FRED
//  
// 
//    Rev 1.10   08 Aug 1995 12:10:32   FRED
// Open databases read-only
// 
//    Rev 1.9   07 Aug 1995 07:48:14   FRED
// Standard fill-in fields.
// 
//    Rev 1.8   06 Aug 1995 09:45:42   FRED
// More fill-in fields stuff.
// 
//    Rev 1.7   04 Aug 1995 10:47:04   FRED
// Sender fields.
// 
//    Rev 1.6   03 Aug 1995 08:00:30   FRED
// Fill in stuff.
// 
//    Rev 1.5   02 Aug 1995 14:56:28   JAY
//  
// 
//    Rev 1.4   02 Aug 1995 12:58:12   FRED
// Some more fill-in fields stuff.
// 
//    Rev 1.3   02 Aug 1995 08:07:14   FRED
// Fill-in fields stuff.
// 
//    Rev 1.2   12 Jul 1995 17:45:18   JAY
// New project sub-types (esp. for cards).
// 
//    Rev 1.1   13 Jun 1995 17:30:56   JAY
// Caches field names to handle the common case.
// 
//    Rev 1.0   01 May 1995 15:25:10   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"

#include "ffldfile.h"

#include "macrorec.h"
#include "namerec.h"

#include "macrostd.h"
#include "addrdata.h"
#include "addrbook.h"

#include "util.h"

#ifdef LOCALIZE
#include "clocale.h"
#endif

/*
// Caching to help speed things up.
*/

CStringArray CStdMacroServer::m_csaLastReceiverFieldNames;
CString CStdMacroServer::m_csLastReceiverDatabaseName;
CString CStdMacroServer::m_csReceiverFieldClass("Address");

CStringArray CStdMacroServer::m_csaLastSenderFieldNames;
CString CStdMacroServer::m_csLastSenderDatabaseName;
CString CStdMacroServer::m_csSenderFieldClass("Sender");

CStringArray CStdMacroServer::m_csaGlobalFieldNames;
CString CStdMacroServer::m_csGlobalFieldClass("Global");

/*
// Constructor.
*/

CStdMacroServer::CStdMacroServer()
{
	m_pMacroList = NULL;
	m_pNameList = NULL;
	m_fReceiverIsBound = FALSE;
	m_fSenderIsBound = FALSE;
	m_pMacroText = NULL;
	m_nMacroTextLength = 0;
	m_pDoc = NULL;
}

/*
// Destructor.
*/

CStdMacroServer::~CStdMacroServer()
{
	if (m_pMacroList != NULL)
	{
		m_pMacroList->release();
	}
	if (m_pNameList != NULL)
	{
		m_pNameList->release();
	}
	
	delete [] m_pMacroText;
}

/*
// Set the macro list in this server.
*/

void CStdMacroServer::SetMacroList(MacroListRecord* pMacroList)
{
	if (m_pMacroList != NULL)
	{
		m_pMacroList->release();
	}
	m_pMacroList = pMacroList;
}

BOOL CStdMacroServer::ShowMacroValues(void) const
{
	return m_pMacroList == NULL ? FALSE : m_pMacroList->ShowValues();
}

void CStdMacroServer::ShowMacroValues(BOOL fShowValues)
{
	if (m_pMacroList != NULL)
	{
		m_pMacroList->ShowValues(fShowValues);
	}
}

/*
// Set the name list in this server.
*/

void CStdMacroServer::SetNameList(NameListRecord* pNameList)
{
	if (m_pNameList != NULL)
	{
		m_pNameList->release();
	}
	m_pNameList = pNameList;
}

/*
// Create a macro name from a database field name.
*/

void CStdMacroServer::DatabaseFieldNameToMacroName(LPCSTR pszField, char* pszMacro, int nBufferSize)
{
	strncpy(pszMacro, pszField, nBufferSize-1);
	pszMacro[nBufferSize-1] = '\0';
}

/*
// Convert a macro name back to a database field name.
*/

void CStdMacroServer::MacroNameToDatabaseFieldName(LPCSTR pszMacro, char* pszField, int nBufferSize)
{
	strncpy(pszField, pszMacro, nBufferSize-1);
	pszField[nBufferSize-1] = '\0';
}

/*
// Add macros for all the fields (receiver and sender) to the macro list.
*/

void CStdMacroServer::AddFieldMacrosToList(void)
{
	int nField;
	int nFields;

	// Add the receiver fields. For now, the class
	// is canned as "Address".
	
	nFields = ReceiverFields();

	for (nField = 0; nField < nFields; nField++)
	{
		const CString& csField = ReceiverFieldName(nField);
		char cbMacroName[128];

		DatabaseFieldNameToMacroName(csField, cbMacroName, sizeof(cbMacroName));
		
		BOOL fMacroExists = (m_pMacroList->FindMacro(cbMacroName) != NULL);

		CMacro* pMacro = m_pMacroList->AddMacro(cbMacroName, "");
		if (pMacro != NULL)
		{
			pMacro->MacroType(CMacro::MACRO_TYPE_Receiver);
			if (!fMacroExists)
			{
				pMacro->MacroValueType(CMacro::VALUE_TYPE_Field);
			}
			pMacro->FieldClass(m_csReceiverFieldClass);
			pMacro->Flags(MACRO_FLAG_PermanentName);
		}
	}
	
	// Add the sender fields. For now, the class
	// is canned as "Sender".
	
	nFields = SenderFields();

	for (nField = 0; nField < nFields; nField++)
	{
		const CString& csField = SenderFieldName(nField);
		char cbMacroName[128];

		DatabaseFieldNameToMacroName(csField, cbMacroName, sizeof(cbMacroName));

		BOOL fMacroExists = (m_pMacroList->FindMacro(cbMacroName) != NULL);
		
		CMacro* pMacro = m_pMacroList->AddMacro(cbMacroName, "");
		if (pMacro != NULL)
		{
			pMacro->MacroType(CMacro::MACRO_TYPE_Sender);
			if (!fMacroExists)
			{
				pMacro->MacroValueType(CMacro::VALUE_TYPE_Field);
			}
			pMacro->FieldClass(m_csSenderFieldClass);
			pMacro->Flags(MACRO_FLAG_PermanentName);
		}
	}
}

/*
// Add macros for all the global fields (PMW.FIF) to the macro list.
*/

void CStdMacroServer::AddGlobalMacrosToList(void)
{
	int nField;
	int nFields;

	nFields = m_csaGlobalFieldNames.GetSize();

	for (nField = 0; nField < nFields; nField++)
	{
		CMacro* pMacro = m_pMacroList->AddMacro(GlobalFieldName(nField), "");
		if (pMacro != NULL)
		{
			pMacro->MacroType(CMacro::MACRO_TYPE_Global);
			pMacro->MacroValueType(CMacro::VALUE_TYPE_String);
			pMacro->FieldClass(m_csGlobalFieldClass);
			pMacro->Flags(MACRO_FLAG_PermanentName);
		}
	}
	
}

// This is a collection of stuff to do after the address book has been run.
// It's assumed that CAddressBook::Run() has just returned and the name list
// hass been updated.
void CStdMacroServer::AfterAddressBookRun(CWnd* pParent, CPmwDoc* pDoc)
{
	NameListRecord* pNameList = GetNameList();
	if (pNameList != NULL)
	{
		// The names of any orphaned fields are kept here.
		CStringArray csaOrphanedMacros;

		if (pNameList->GetAddressBookNameChanged())
		{
			// Update the field macros to reflect the new address book.
			// First, any receiver field macros which are not in use are
			// removed. Then the receiver field macros for the new address
			// book are added into the macro list following any retained
			// fields from the previous address book.
			//
			// NOTE: This algorithm assumes that the macros are arranged in the
			// list as groups: Receiver, Sender, and User. Within each group, the
			// macros are arranged in their database field order.

			CWordArray cwaMacrosInUse;

			TRY
			{
				pDoc->GetMacrosInUse(cwaMacrosInUse);

				// Delete all the receiver macros which are not in use.
				int nMacroCount = m_pMacroList->MacroList().GetSize();
				for (int nMacro = 0; nMacro < nMacroCount; )
				{
					BOOL fDeleteMacro = FALSE;

					CMacro* pMacro = (CMacro*)(m_pMacroList->MacroList().GetAt(nMacro));
					ASSERT(pMacro != NULL);
					if (pMacro != NULL)
					{
						// Check for a receiver macro.
						if (pMacro->MacroType() == CMacro::MACRO_TYPE_Receiver)
						{
							// Check if the macro is not in use, delete it.
							fDeleteMacro = TRUE;
							WORD wMacro = pMacro->Id() + MACRO_CHARACTER;
							int nInUseCount = cwaMacrosInUse.GetSize();
							for (int nInUse = 0; nInUse < nInUseCount; nInUse++)
							{
								if (cwaMacrosInUse[nInUse] == wMacro)
								{
									fDeleteMacro = FALSE;
									break;
								}
							}
						}
					}

					// Delete the macro... or not.
					if (fDeleteMacro)
					{
						m_pMacroList->DeleteMacro(nMacro);
						nMacroCount = m_pMacroList->MacroList().GetSize();
					}
					else
					{
						nMacro++;
					}
				}
			}
			END_TRY

			TRY
			{
				// Read the new receiver field names.
				ReadReceiverFieldNames(pNameList->AddressBookName());
				int nFieldCount = ReceiverFields();
				int nField;

				// Delete any receiver macros which have the same names as the new receiver
				// fields since these will be reinserted.
				for (nField = 0; nField < nFieldCount; nField++)
				{
					const CString& csField = ReceiverFieldName(nField);
					char cbMacroName[128];
					DatabaseFieldNameToMacroName(csField, cbMacroName, sizeof(cbMacroName));
					CMacro* pMacro = m_pMacroList->FindMacro(cbMacroName);
					if (pMacro != NULL)
					{
						if (pMacro->MacroType() == CMacro::MACRO_TYPE_Receiver)
						{
							m_pMacroList->MacroList().DeleteMacro(cbMacroName);
						}
					}
				}
			}
			END_TRY

			TRY
			{
				// Remember the names of the remaining receiver macros since we want to
				// tell the user about them. These are "orphaned" receiver macros which
				// do not relate to the new address book.
				int nMacroCount = m_pMacroList->MacroList().GetSize();
				for (int nMacro = 0; nMacro < nMacroCount; nMacro++)
				{
					CMacro* pMacro = m_pMacroList->Macro(nMacro);
					if (pMacro != NULL)
					{
						if (pMacro->MacroType() == CMacro::MACRO_TYPE_Receiver)
						{
							csaOrphanedMacros.Add(pMacro->Name());
						}
					}
				}
			}
			END_TRY

			TRY
			{
				// Insert the new receiver macros in database order at the start of the macro list.
				// The orphaned receivers will appear after this list.
				int nFieldCount = ReceiverFields();
				for (int nField = 0; nField < nFieldCount; nField++)
				{
					const CString& csField = ReceiverFieldName(nField);
					char cbMacroName[128];

					DatabaseFieldNameToMacroName(csField, cbMacroName, sizeof(cbMacroName));
					
					BOOL fMacroExists = (m_pMacroList->FindMacro(cbMacroName) != NULL);

					CMacro* pMacro = m_pMacroList->MacroList().AddMacroAt(nField, cbMacroName, "");
					if (pMacro != NULL)
					{
						pMacro->MacroType(CMacro::MACRO_TYPE_Receiver);
						if (!fMacroExists)
						{
							pMacro->MacroValueType(CMacro::VALUE_TYPE_Field);
						}
						pMacro->FieldClass(m_csReceiverFieldClass);
						pMacro->Flags(MACRO_FLAG_PermanentName);
					}
				}
			}
			END_TRY

			ModifiedMacros();
		}

		// Reflow.
		if ((pNameList->GetAddressBookDataChanged())
		 || (pNameList->GetAddressBookNameChanged())
		 || (pNameList->GetNamesChanged()))
		{
 			pDoc->SetModifiedFlag();
			pDoc->ViewSampleRecord();
			pDoc->ReflowMacros();
		}

		// Report any orphaned receiver macros.
		if (csaOrphanedMacros.GetSize() > 0)
		{
			COrphanedFieldsDialog Dialog(csaOrphanedMacros, pNameList->AddressBookName(), pParent);
			Dialog.DoModal();
		}
	}
}

CMacroList& CStdMacroServer::MacroList(void)
{
	return m_pMacroList->MacroList();
}

/*
// Return the number of macros which are available.
*/

int CStdMacroServer::Macros(void)
{
	ASSERT(m_pMacroList != NULL);

	return m_pMacroList->Macros();
}

/*
// Get the macro character for the n'th macro.
*/

CHARACTER CStdMacroServer::GetMacroCharacter(int nMacro)
{
	if (nMacro < m_pMacroList->Macros())
	{
		CMacro* pMacro = m_pMacroList->Macro(nMacro);
		if (pMacro != NULL)
		{
			return (CHARACTER)pMacro->Id() + MACRO_CHARACTER;
		}
	}
	return MACRO_CHARACTER;			// Illegal macro value.
}

/*
// Say that we modified the macros (via the MacroList).
*/

void CStdMacroServer::ModifiedMacros(void)
{
	m_pMacroList->modified();
}

// Given a macro character, return a pointer to an array of CHARACTER
// containing the corresponding macro's Value. If the macro value is a
// field, then the field value is returned. If the macro is not a field,
// then its Value() method is used to retrieved the value.
 
CHARACTER* CStdMacroServer::GetMacroText(CHARACTER c, DB_RECORD_NUMBER lPage /*=0*/)
{
	if (c == MACRO_CHARACTER_Page)
	{
		// Handle a page macro. We need a document.
		if (m_pDoc != NULL)
		{
			CString csPage = m_pDoc->GetPageString(lPage);
			return CharactersFromString(csPage);
		}
	}
	else if (c >= MACRO_CHARACTER)
	{
		c -= MACRO_CHARACTER;		// IDs start at 1.

		CMacro* pMacro;
		if ((pMacro = m_pMacroList->FindMacro((WORD)c)) != NULL)
		{
		   return GetMacroText(pMacro);
		}
	}
	return NULL;
}

LPCSTR CStdMacroServer::GetMacroFieldText(CHARACTER c)
{
	if (c >= MACRO_CHARACTER)
	{
		c -= MACRO_CHARACTER;		// IDs start at 1.

		CMacro* pMacro;
		if ((pMacro = m_pMacroList->FindMacro((WORD)c)) != NULL)
		{
		   return pMacro->Name();
		}
	}
	return NULL;
}

// Given a macro, return a pointer to an array of CHARACTER
// containing the macro's Value. If the macro value is a field,
// then the field value is returned. If the macro is not a field,
// then its Value() method is used to retrieved the value.

CHARACTER* CStdMacroServer::GetMacroText(CMacro* pMacro)
{
	CHARACTER* pResult = NULL;
	
	// Get the value into a CString.
	CString csValue = GetMacroTextString(pMacro);

	// ASSUMPTION: This function is only called to get the text to use
	// on the page, not the fill-in fields dialog. So, we do the Name?
	// thing if the text is empty unless ShowValues is true in which
	// case we are printing.

	if (csValue.IsEmpty())
	{
		if (!m_pMacroList->ShowValues())
		{
			csValue = pMacro->Name();
			csValue += '?';
		}
	}

	return CharactersFromString(csValue);
}

CHARACTER* CStdMacroServer::CharactersFromString(const CString& csValue)
{
	CHARACTER* pResult = NULL;

	// Copy the CString to CHARACTER buffer.
	TRY
	{
		// Make sure the buffer exists and is big enough.
		if ((m_pMacroText == NULL) || (m_nMacroTextLength < csValue.GetLength()))
		{
			delete [] m_pMacroText;
			m_pMacroText = NULL;
			m_pMacroText = new CHARACTER[csValue.GetLength()+1];
			ASSERT(m_pMacroText != NULL);
			m_nMacroTextLength = csValue.GetLength();
		}
		
		if (m_pMacroText != NULL)
		{
			LPCSTR pText = csValue;
			CHARACTER* cPtr = m_pMacroText;

			do
			{
				*cPtr++ = (CHARACTER)(BYTE)*pText;
			} while (*pText++ != '\0');
		}

		pResult = m_pMacroText;
	}
	END_TRY

	return pResult;
}

// Given a macro, return a CString containing it's Value.
// If the macro value is a field, then the field value is
// returned. If the macro is not a field, then its Value()
// method is used to retrieved the value.

CString CStdMacroServer::GetMacroTextString(CMacro* pMacro)
{
	CString csValue;

	csValue = pMacro->Value();
			
	if (pMacro->MacroValueType() == CMacro::VALUE_TYPE_Field)
	{
		// The macro represents a field, get its value by looking
		// up its composite name in the name/value mapping. The macro's
		// type indicates whether it is a Receiver or Sender field.
		
		CString csField;
		MacroNameToDatabaseFieldName(pMacro->Name(), csField.GetBuffer(128), 128);
		csField.ReleaseBuffer();
		CString csName;
		CompositeName(csField, pMacro->FieldClass(), csName);
		
		switch (pMacro->MacroType())
		{
			case CMacro::MACRO_TYPE_Receiver:
			{
				csValue = ReceiverFieldValue(csName);
				break;
			}
			
			case CMacro::MACRO_TYPE_Sender:
			{
				csValue = SenderFieldValue(csName);
				break;
			}
			
			default:
			{
				csValue.Empty();
			}
		}
	}
	
	return csValue;
}

// Clear all the field values, receiver and sender.

void CStdMacroServer::ClearFieldValues(void)
{
	ClearReceiverFieldValues();
	ClearSenderFieldValues();
}

/* A composite name is Class+":"+Name */

void CStdMacroServer::CompositeName(LPCSTR pFieldName, LPCSTR pClassName, CString& csName)
{
	csName = pClassName;
	csName += ":";
	csName += pFieldName;
}

/* Number of receiver fields. */

int CStdMacroServer::ReceiverFields(void)
{
	return m_csaReceiverFieldNames.GetSize();
}

/* Name of receiver field by index. */

const CString& CStdMacroServer::ReceiverFieldName(int nField)
{
	return m_csaReceiverFieldNames.ElementAt(nField);
}

/* Class of receiver field by index. */

const CString& CStdMacroServer::ReceiverFieldClass(int nField)
{
	return m_csReceiverFieldClass;
}

/* By field index. */

CString CStdMacroServer::ReceiverFieldValue(int nField)
{
	return ReceiverFieldValue(ReceiverFieldName(nField));
}

void CStdMacroServer::ReceiverFieldValue(int nField, const CString& csValue)
{
	ReceiverFieldValue(ReceiverFieldName(nField), csValue);
}

/* By field name. */

CString CStdMacroServer::ReceiverFieldValue(const CString& csFieldName)
{
	CString csValue;
	m_mapReceiverFieldValues.Lookup(csFieldName, csValue);
	return csValue;
}

void CStdMacroServer::ReceiverFieldValue(const CString& csFieldName, const CString& csValue)
{
	m_mapReceiverFieldValues.SetAt(csFieldName, csValue);
}

/*
// Remove all receiver field values mappings.
*/

void CStdMacroServer::ClearReceiverFieldValues(void)
{
	m_mapReceiverFieldValues.RemoveAll();
	m_fReceiverIsBound = FALSE;
}

/* Number of sender fields. */

int CStdMacroServer::SenderFields(void)
{
	return m_csaSenderFieldNames.GetSize();
}

/* Name of sender field by index. */

const CString& CStdMacroServer::SenderFieldName(int nField)
{
	return m_csaSenderFieldNames.ElementAt(nField);
}

/* Class of sender field by index. */

const CString& CStdMacroServer::SenderFieldClass(int nField)
{
	return m_csSenderFieldClass;
}

/* By field index. */

CString CStdMacroServer::SenderFieldValue(int nField)
{
	return SenderFieldValue(SenderFieldName(nField));
}

void CStdMacroServer::SenderFieldValue(int nField, const CString& csValue)
{
	SenderFieldValue(SenderFieldName(nField), csValue);
}

/* By field name. */

CString CStdMacroServer::SenderFieldValue(const CString& csFieldName)
{
	CString csValue;
	m_mapSenderFieldValues.Lookup(csFieldName, csValue);
	return csValue;
}

void CStdMacroServer::SenderFieldValue(const CString& csFieldName, const CString& csValue)
{
	m_mapSenderFieldValues.SetAt(csFieldName, csValue);
}

/*
// Remove all sender field values mappings.
*/

void CStdMacroServer::ClearSenderFieldValues(void)
{
	m_mapSenderFieldValues.RemoveAll();
	m_fSenderIsBound = FALSE;
}

// Read and cache the receiver field names from the specified receiver database.

BOOL CStdMacroServer::ReadReceiverFieldNames(LPCSTR pszDatabaseName)
{
	DWORD dwFields;
	DWORD dwField;
	
	// Get rid of all current information.
	
	m_csaReceiverFieldNames.RemoveAll();
	ClearReceiverFieldValues();

	if (pszDatabaseName == NULL)
	{
		return FALSE;
	}

	// Make sure the receiver field names are cached.

	if (pszDatabaseName != m_csLastReceiverDatabaseName)
	{
		// We have switched receiver databases, rebuild the receiver cache.
		
		m_csLastReceiverDatabaseName = pszDatabaseName;
		m_csaLastReceiverFieldNames.RemoveAll();

		TRACE("Open database %s for receiver field names.\r\n", pszDatabaseName);

		StorageFile* pDatabaseFile = NULL;
		CFlatFileDatabase* pDatabase = NULL;
		CFlatFileDatabaseCursor* pCursor = NULL;

		// Open up the address book.
		ERRORCODE Error = CAddressBook::OpenAddressBookDatabase(pszDatabaseName, pDatabaseFile, pDatabase, TRUE);
		if (Error == ERRORCODE_None)
		{
			TRY
			{
				// Create a cursor on the database.
				pCursor = new CFlatFileDatabaseCursor;
				pCursor->Attach(pDatabase);
			
				// Loop through all the fields and record them.
         
				dwFields = pCursor->GetFieldCount();
				for (dwField = 0; dwField < dwFields; dwField++)
				{
					CFlatFileDatabaseField* pField = pCursor->GetField(dwField);
					if (pField != NULL)
					{
						if ((pField->GetType() != FFT_Blob)
						 && (FIELD_USER_DATA_EDITOR(pField->GetDescription()->GetUserData()) != FIELD_EDIT_NONE))
						{
							TRACE("Got field: %s\r\n", pField->GetName());
							m_csaLastReceiverFieldNames.Add(pField->GetName());
						}
					}
				}
			}
			CATCH_ALL(e)
			{
				// Got some sort of error, reset cached receiver field information.
				
				m_csLastReceiverDatabaseName.Empty();
				m_csaLastReceiverFieldNames.RemoveAll();
			}
			END_CATCH_ALL
		}
			
		// All done with receiver database.
		delete pCursor;
		pCursor = NULL;
		CAddressBook::CloseAddressBookDatabase(pDatabaseFile, pDatabase);
	}
	
	// Copy the receiver field names from the cache to the array.

	dwFields = m_csaLastReceiverFieldNames.GetSize();
	for (dwField = 0; dwField < dwFields; dwField++)
	{
		m_csaReceiverFieldNames.Add(m_csaLastReceiverFieldNames.ElementAt((int)dwField));
	}

	return TRUE;
}

/*
// Bind the document to the identified receiver record using the passed (open) cursor.
*/

BOOL CStdMacroServer::BindToRecord(CFlatFileDatabaseCursor* pCursor, DWORD dwKey)
{
	BOOL fRet = FALSE;

	TRY
	{
		// Move to the record with the given ID.
		pCursor->SetIndex("__ID__");
		pCursor->New();
		pCursor->GetField("__ID__")->SetAsUnsigned32(dwKey);
		BOOL fFound = FALSE;
		pCursor->Find(&fFound);
		if (fFound)
		{
			fRet = TRUE;
	
			// We now have the record! Fill in values from the fields.
         
			DWORD dwFields = pCursor->GetFieldCount();
			for (DWORD dwField = 0; dwField < dwFields; dwField++)
			{
				CFlatFileDatabaseField* pField = pCursor->GetField(dwField);
				if (pField != NULL)
				{
					if ((pField->GetType() != FFT_Blob)
					 && (FIELD_USER_DATA_EDITOR(pField->GetDescription()->GetUserData()) != FIELD_EDIT_NONE))
					{
						// Get the value.
						CString csField;
						CString csValue;
						TRY
						{
							// Build a composite name.
							CompositeName(pField->GetName(), m_csReceiverFieldClass, csField);
							csValue = pField->GetAsString();
				
							// Add the value.
							ReceiverFieldValue(csField, csValue);
						}
						CATCH_ALL(e)
						{
							fRet = FALSE;
							break;
						}
						END_CATCH_ALL
					}
				}
			}
		}
	}
	END_TRY

	if (!fRet)
	{
		// Some error occurred.
		ClearReceiverFieldValues();
	}
	
	m_fReceiverIsBound = fRet;
	
	return fRet;
}

// Read and cache the sender field names from the specified sender database.

BOOL CStdMacroServer::ReadSenderFieldNames(LPCSTR pszDatabaseName)
{
	DWORD dwFields;
	DWORD dwField;
	
	// Get rid of all current information.
	
	m_csaSenderFieldNames.RemoveAll();
	ClearSenderFieldValues();

	if (pszDatabaseName == NULL)
	{
		return FALSE;
	}

	// Make sure the sender field names are cached.

	if (pszDatabaseName != m_csLastSenderDatabaseName)
	{
		// We have switched sender databases, rebuild the receiver cache.
		
		m_csLastSenderDatabaseName = pszDatabaseName;
		m_csaLastSenderFieldNames.RemoveAll();

		TRACE("Open database %s for sender field names.\r\n", pszDatabaseName);

		StorageFile* pDatabaseFile = NULL;
		CFlatFileDatabase* pDatabase = NULL;
		CFlatFileDatabaseCursor* pCursor = NULL;

		// Open up the sender address book.
		ERRORCODE Error = CAddressBook::OpenAddressBookSenderDatabase(pszDatabaseName, pDatabaseFile, pDatabase, TRUE);
		if (Error == ERRORCODE_None)
		{
			CString csName;
			TRY
			{
				// Create a cursor on the database.
				pCursor = new CFlatFileDatabaseCursor;
				pCursor->Attach(pDatabase);
			
				// Loop through all the fields and record them.
         
				dwFields = pCursor->GetFieldCount();
				for (dwField = 0; dwField < dwFields; dwField++)
				{
					CFlatFileDatabaseField* pField = pCursor->GetField(dwField);
					if (pField != NULL)
					{
						if ((pField->GetType() != FFT_Blob)
						 && (FIELD_USER_DATA_EDITOR(pField->GetDescription()->GetUserData()) != FIELD_EDIT_NONE))
						{
							TRACE("Got field: %s\r\n", pField->GetName());
#ifdef LOCALIZE
// CB May 18 98, get rid of hard-coded string for localization
							csName = GetSenderFieldNameFromFieldName(pField->GetName());
#else
							csName = "Sender ";
							csName += pField->GetName();
#endif
							m_csaLastSenderFieldNames.Add(csName);
						}
					}
				}
			}
			CATCH_ALL(e)
			{
				// Got some sort of error, reset cached receiver field information.
				
				m_csLastSenderDatabaseName.Empty();
				m_csaLastSenderFieldNames.RemoveAll();
			}
			END_CATCH_ALL
		}
			
		// All done with sender database.
		delete pCursor;
		pCursor = NULL;
		CAddressBook::CloseAddressBookDatabase(pDatabaseFile, pDatabase);
	}
	
	// Copy the sender field names from the cache to the array.

	dwFields = m_csaLastSenderFieldNames.GetSize();
	for (dwField = 0; dwField < dwFields; dwField++)
	{
		m_csaSenderFieldNames.Add(m_csaLastSenderFieldNames.ElementAt((int)dwField));
	}

	return TRUE;
}

// Bind the sender values to the sender information record for the current user.

BOOL CStdMacroServer::BindSender(void)
{
	BOOL fResult = FALSE;

	// Remove any current values.
	ClearSenderFieldValues();
	
	// Get the sender ID.
	DWORD dwSenderId = GET_PMWAPP()->GetSenderId();
	if (dwSenderId != (DWORD)-1)
	{
		StorageFile* pDatabaseFile = NULL;
		CFlatFileDatabase* pDatabase = NULL;
		CFlatFileDatabaseCursor* pCursor = NULL;

		// Open up the sender address book.
		ERRORCODE Error = CAddressBook::OpenAddressBookSenderDatabase(GET_PMWAPP()->SenderBook(), pDatabaseFile, pDatabase, TRUE);
		if (Error == ERRORCODE_None)
		{
			TRY
			{
				// Create a cursor on the database.
				pCursor = new CFlatFileDatabaseCursor;
				pCursor->Attach(pDatabase);
			
				// Move to the record with the given Sender ID.
				pCursor->SetIndex("__ID__");
				pCursor->New();
				pCursor->GetField("__ID__")->SetAsUnsigned32(dwSenderId);
				BOOL fFound = FALSE;
				pCursor->Find(&fFound);
				if (fFound)
				{
					fResult = TRUE;
	
					// We have the record, fill in values from the fields.
					DWORD dwFields = pCursor->GetFieldCount();
					for (DWORD dwField = 0; dwField < dwFields; dwField++)
					{
						CFlatFileDatabaseField* pField = pCursor->GetField(dwField);
						if (pField != NULL)
						{
							if ((pField->GetType() != FFT_Blob)
							 && (FIELD_USER_DATA_EDITOR(pField->GetDescription()->GetUserData()) != FIELD_EDIT_NONE))
							{
								// Get the value.
								CString csName;
								CString csField;
								CString csValue;
								TRY
								{
									// Build a composite name.
#ifdef LOCALIZE
									csName = GetSenderFieldNameFromFieldName(pField->GetName());
#else

									csName = "Sender ";
									csName += pField->GetName();
#endif
									CompositeName(csName, m_csSenderFieldClass, csField);
									csValue = pField->GetAsString();
						
									// Add the value.
									SenderFieldValue(csField, csValue);
								}
								CATCH_ALL(e)
								{
									fResult = FALSE;
									break;
								}
								END_CATCH_ALL
							}
						}
					}
				}
			}
			END_TRY
		}
			
		// All done with sender database.
		delete pCursor;
		pCursor = NULL;
		CAddressBook::CloseAddressBookDatabase(pDatabaseFile, pDatabase);

		if (!fResult)
		{
			// Got some sort of error, reset cached sender field information.
			ClearSenderFieldValues();
		}
	}

	m_fSenderIsBound = fResult;
	
	return fResult;
}

/* Name of global field by index. */

const CString& CStdMacroServer::GlobalFieldName(int nField)
{
	return m_csaGlobalFieldNames.ElementAt(nField);
}

// Read and cache the sender field names from the specified sender database.

BOOL CStdMacroServer::ReadGlobalFieldNames(LPCSTR pszFileName)
{
	BOOL fResult = FALSE;
	
	// Clear any previous global field names.
	m_csaGlobalFieldNames.RemoveAll();
	
	CFillinFieldFile FieldFile;
	
	TRY
	{
		fResult = FieldFile.Init(pszFileName, &m_csaGlobalFieldNames);
	}
	END_TRY

	if (!fResult)
	{
		m_csaGlobalFieldNames.RemoveAll();
	}
	
	return fResult;
}	
