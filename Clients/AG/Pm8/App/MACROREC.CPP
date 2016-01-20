/*
// $Header: /PM8/App/MACROREC.CPP 1     3/03/99 6:07p Gbeddow $
//
// Macro list record routines.
//
// $Log: /PM8/App/MACROREC.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 3     5/18/98 5:32p Cboggio
// Changes for localized versions
// 1) Allow for run-time field name translation. For cases where old
// templates were edited and saved with old translations which were wrong
// or have been changed for the current version.
// 2) When reading permanent fill-in-field names from a template only add
// field names that are in our fixed list of permanent field names. This
// is a way of removing US fill-in-field names from localized templates.
// 
// 2     3/18/98 11:14a Jayn
// Page numbers
// 
//    Rev 1.0   14 Aug 1997 15:22:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:06   Fred
// Initial revision.
// 
//    Rev 1.2   16 Jul 1996 17:29:02   Fred
// More address/sender book stuff
// 
//    Rev 1.1   16 Jul 1996 08:47:04   Fred
// Changes for new address book
// 
//    Rev 1.0   14 Mar 1996 13:44:06   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:46   FRED
// Initial revision.
// 
//    Rev 1.8   28 Aug 1995 08:17:12   JAY
// Fixed the "fix" I added last time.
// 
//    Rev 1.7   25 Aug 1995 18:37:30   JAY
// AssignData for macros were copying the id (bad, I think).
// 
//    Rev 1.6   25 Aug 1995 15:01:24   JAY
// New user type.
// 
//    Rev 1.5   23 Aug 1995 10:21:02   FRED
// Or's in all macros
// 
//    Rev 1.4   01 May 1995 15:24:48   JAY
// Restructured the macro stuff
// 
//    Rev 1.3   01 May 1995 08:44:32   JAY
// New AssignData() method.
// 
//    Rev 1.2   25 Apr 1995 08:51:06   JAY
// Fixed some problems with reading macros.
// 
//    Rev 1.1   19 Apr 1995 12:31:20   JAY
//  
// 
//    Rev 1.0   29 Mar 1995 09:00:56   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "macrorec.h"
#include "text.h"

#ifdef LOCALIZE
#include "clocale.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CMacro

CMacro::CMacro()
{
	memset(&m_Record, 0, sizeof(m_Record));
	m_Record.m_wType = MACRO_TYPE_User;
	m_Record.m_wValueType = VALUE_TYPE_String;
}

CMacro::~CMacro()
{
}

/*
// Assign data to us from another macro.
// This will not change our id.
*/

void CMacro::AssignData(const CMacro* pMacro)
{
	WORD wId = Id();
	m_Record = pMacro->m_Record;
	Id(wId);

	Name(pMacro->Name());
	Value(pMacro->Value());
	FieldClass(pMacro->FieldClass());
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE CMacro::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None
		  && (error = device->read_cstring(m_csName)) == ERRORCODE_None
		  && (error = device->read_cstring(m_csValue)) == ERRORCODE_None)
	{
		switch (m_Record.m_wValueType)
		{
			case VALUE_TYPE_OldField:
			{
				m_csFieldClass = "Address";			// A default.
				m_Record.m_wValueType = VALUE_TYPE_Field;
				break;
			}
			case VALUE_TYPE_Field:
			{
				error = device->read_cstring(m_csFieldClass);
				break;
			}
			default:			// Includes VALUE_TYPE_String.
			{
				m_csFieldClass = "";
				break;
			}
		}
#ifdef LOCALIZE
		// cb October 97 translate any permanent fields where required
		if (m_Record.m_wFlags & MACRO_FLAG_PermanentName)
		{
			m_csName = TranslateFields((LPCSTR) m_csName);
		}
#endif

	}
	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE CMacro::WriteData(StorageDevicePtr device)
{
	ASSERT(m_Record.m_wValueType != VALUE_TYPE_OldField);

	BOOL fWriteClass = (m_Record.m_wValueType == VALUE_TYPE_Field);

	ERRORCODE error;

	if ((error = device->write_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None
		  && (error = device->write_cstring(m_csName)) == ERRORCODE_None
		  && (error = device->write_cstring(m_csValue)) == ERRORCODE_None
		  && (!fWriteClass || (error = device->write_cstring(m_csFieldClass)) == ERRORCODE_None))
	{
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE CMacro::SizeofData(StorageDevicePtr device)
{
	ASSERT(m_Record.m_wValueType != VALUE_TYPE_OldField);
	BOOL fSizeClass = (m_Record.m_wValueType == VALUE_TYPE_Field);

	return device->size_record(sizeof(m_Record))
				+ device->size_cstring(m_csName)
				+ device->size_cstring(m_csValue)
				+ (fSizeClass ? device->size_cstring(m_csFieldClass) : 0);
}

/////////////////////////////////////////////////////////////////////////////
// CMacroList

/*
// Destructor.
*/

CMacroList::~CMacroList()
{
	DeleteAll();
}

/*
// Delete all macros in this list.
*/

void CMacroList::DeleteAll(void)
{
	while (Macros() != 0)
	{
		DeleteMacro(0);
	}
}

/*
// Assignment operator.
*/

const CMacroList& CMacroList::operator=(const CMacroList& List)
{
/* Clear the list. */

	DeleteAll();

/* Copy over the macros. */

	int nMacros = List.Macros();

	for (int nMacro = 0; nMacro < nMacros; nMacro++)
	{
		TRY
		{
			Add(new CMacro(*List.Macro(nMacro)));
		}
		CATCH_ALL(e)
		{
			nMacro = nMacros;
		}
		END_CATCH_ALL
	}

	return *this;
}

/*
// Add a macro.
*/

CMacro* CMacroList::AddMacro(LPCSTR pName, LPCSTR pValue)
{
	return AddMacroAt(GetSize(), pName, pValue);
}

/*
// Add a macro.
*/

CMacro* CMacroList::AddMacroAt(int nIndex, LPCSTR pName, LPCSTR pValue)
{
	CMacro* pMacro = FindMacro(pName);
	
	if (pMacro == NULL)
	{
	// Safe 'new'...
		TRY
			pMacro = new CMacro;
		END_TRY
	
		WORD wID;
	
		if ((wID = UniqueID()) == 0)
		{
		/* Should never happen, but who knows? */
			return NULL;
		}
	
		if (pMacro != NULL)
		{
			pMacro->Id(wID);
			pMacro->Name(pName);
			pMacro->Value(pValue);
	
			InsertAt(nIndex, pMacro);
		}
	}
	return pMacro;			// And return it in case anybody cares.
}

/*
// Delete a macro by name.
*/

void CMacroList::DeleteMacro(LPCSTR pName)
{
	int nMacros = Macros();

	for (int nMacro = 0; nMacro < nMacros; nMacro++)
	{
		CMacro* pMacro = Macro(nMacro);
		if (pMacro->Name() == pName)
		{
			DeleteMacro(nMacro);
			return;
		}
	}
}

/*
// Delete a macro by index.
*/

void CMacroList::DeleteMacro(int nMacro)
{
	ASSERT(nMacro < Macros());

	if (nMacro < Macros())
	{
		delete Macro(nMacro);
		RemoveAt(nMacro);
	}
}

/*
// Find a macro by id.
*/

CMacro* CMacroList::FindMacro(WORD wID)
{
	int nMacros = Macros();

	for (int nMacro = 0; nMacro < nMacros; nMacro++)
	{
		CMacro* pMacro = Macro(nMacro);
		if (pMacro->Id() == wID)
		{
			return pMacro;
		}
	}
	return NULL;
}

/*
// Find a macro by name.
*/

CMacro* CMacroList::FindMacro(LPCSTR pName)
{
	int nMacros = Macros();

	for (int nMacro = 0; nMacro < nMacros; nMacro++)
	{
		CMacro* pMacro = Macro(nMacro);
		if (pMacro->Name() == pName)
		{
			return pMacro;
		}
	}
	return NULL;
}

/*
// Generate a unique ID for a macro.
*/

WORD CMacroList::UniqueID(void)
{
	// Brute force method. Start at 1 and look...

	// Look for an open ID. If we hit the max, fail...
	WORD wID = 1;

	while (wID < MACRO_Max)
	{
		if (FindMacro(wID) == NULL)
		{
			break;
		}
		wID++;
	}

	// Return what we found.
	return (wID == MACRO_Max) ? 0 : wID;
}

/*
// Get the title for the n'th macro.
*/

BOOL CMacroList::GetMacroTitle(int nMacro, CString& csTitle)
{
/* In case of error. */
	csTitle.Empty();

	if (nMacro < Macros())
	{
		CMacro* pMacro = Macro(nMacro);
		if (pMacro != NULL)
		{
			csTitle = pMacro->Name();
			return TRUE;
		}
	}
	ASSERT(FALSE);
	return FALSE;
}

/*
// Get the value for the n'th macro.
*/

BOOL CMacroList::GetMacroValue(int nMacro, CString& csValue)
{
/* In case of error. */
	csValue.Empty();

	if (nMacro < Macros())
	{
		CMacro* pMacro = Macro(nMacro);
		if (pMacro != NULL)
		{
			csValue = pMacro->Value();
			return TRUE;
		}
	}
	ASSERT(FALSE);
	return FALSE;
}

/*
// Set the value for the n'th macro.
*/

BOOL CMacroList::SetMacroValue(int nMacro, const CString& csNewValue)
{
	if (nMacro < Macros())
	{
		CMacro* pMacro = Macro(nMacro);
		if (pMacro != NULL)
		{
			pMacro->Value(csNewValue);
			return TRUE;
		}
	}
	ASSERT(FALSE);
	return FALSE;
}

/*
// Import a macro from another macro list.
// Returns 0 on failure.
*/

WORD CMacroList::ImportMacro(WORD wMacroId, CMacroList* pSrcList)
{
/* Find the macro in the source list. */
	CMacro* pSrcMacro = pSrcList->FindMacro(wMacroId);
	ASSERT(pSrcMacro != NULL);		// It must be there.
	if (pSrcMacro != NULL)
	{
	/* First, see if this macro exists in our list. */
		CMacro* pOurMacro = FindMacro(pSrcMacro->Name());
		if (pOurMacro == NULL)
		{
		/* We need to create it. */
			pOurMacro = AddMacro(pSrcMacro->Name(), "");
			if (pOurMacro == NULL)
			{
				return 0;
			}
			else
			{
			/* Copy over the data. Don't change the new id. */
				pOurMacro->AssignData(pSrcMacro);
				return pOurMacro->Id();
			}
		}
		else
		{
		/* It exists. Just reference it. */
			return pOurMacro->Id();
		}
	}
/* Macro is missing from the source. */
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// MacroListRecord

/*
// The creator for a macro list data record.
*/

ERRORCODE MacroListRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	MacroListRecord* pRecord;

/* Create the new macro list record. */

	if ((pRecord = new MacroListRecord(number, type, owner, where)) == NULL)
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

MacroListRecord::MacroListRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	m_Record.m_wCount = 0;
	m_Record.m_fShowValues = FALSE;
	m_Record.m_wVersion = 3;
}

MacroListRecord::~MacroListRecord()
{
}

/*
// Add a macro.
*/

CMacro* MacroListRecord::AddMacro(LPCSTR pName, LPCSTR pValue)
{
	modified();
	return m_Macros.AddMacro(pName, pValue);
}

/*
// Delete a macro.
*/

void MacroListRecord::DeleteMacro(int nMacro)
{
	modified();
	m_Macros.DeleteMacro(nMacro);
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE MacroListRecord::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

/* Read the master record. */

	if ((error = device->read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		int nMacros = m_Record.m_wCount;

	/* Read all macros. */

		for (int nMacro = 0; nMacro < nMacros; nMacro++)
		{
			CMacro* pMacro = NULL;

			TRY
				pMacro = new CMacro;
			END_TRY

			if (pMacro == NULL)
			{
				return ERRORCODE_Memory;
			}

		/* Read the macro. */

			if ((error = pMacro->ReadData(device)) != ERRORCODE_None)
			{
				delete pMacro;
				return error;		// And get outta' here.
			}

#ifdef LOCALIZE			
// CB - september 97 for localization
// TODO find a permanent solution
// for now don't add any of the Permanent field names from the template if they don't exist
// in the list for this language. This is a temporary solution to the problem
// of templates with hard-coded permanent field names
			if ( (pMacro != NULL) &&
				 ( !(pMacro->Flags() & MACRO_FLAG_PermanentName) || (IsPermanentField(pMacro->Name()) ) )
				 )
				m_Macros.Add(pMacro);
		}
	}
#else	

			m_Macros.Add(pMacro);
		}
	}
	ASSERT(m_Macros.GetSize() == (int)m_Record.m_wCount);
#endif

	// to the new style.
	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE MacroListRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;

/* Establish size. */

	m_Record.m_wCount = Macros();

/* We are version 3. */

	m_Record.m_wVersion = 3;

/* Write the master record. */

	if ((error = device->write_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
	/* Write the macros. */

		for (int nMacro = 0; nMacro < (int)m_Record.m_wCount; nMacro++)
		{
			CMacro* pMacro = Macro(nMacro);
			ASSERT(pMacro != NULL);
			if (pMacro == NULL)
			{
				return ERRORCODE_IntError;
			}
		/* Write the macro. */
			if ((error = pMacro->WriteData(device)) != ERRORCODE_None)
			{
				return error;
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

ST_MAN_SIZE MacroListRecord::SizeofData(StorageDevicePtr device)
{
	ST_MAN_SIZE Size = device->size_record(sizeof(m_Record));

	int nMacros = Macros();

	for (int nMacro = 0; nMacro < nMacros; nMacro++)
	{
		CMacro* pMacro = Macro(nMacro);
		ASSERT(pMacro != NULL);
		if (pMacro != NULL)			// Avoid any GPs at run-time.
		{
			Size += pMacro->SizeofData(device);
		}
	}
	return Size;
}

