/*
// $Header: /PM8/App/MACRO.CPP 1     3/03/99 6:07p Gbeddow $
//
// macro.cpp : implementation of text macros
//
// $Log: /PM8/App/MACRO.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 10    10/21/98 4:58p Jayn
// Fixed path manager problems.
// 
// 9     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 8     7/10/98 11:22a Rlovejoy
// Pass NULL as address book parent.
// 
// 7     7/10/98 11:08a Rlovejoy
// Launch address book with the active window as the parent.
// 
// 6     6/04/98 7:52p Jayn
// Table fill down and right plus a lot of "fit to text" stuff
// 
// 5     5/31/98 5:11p Fredf
// More Address Book and merge field changes.
// 
// 4     2/24/98 5:48p Johno
// Not my task anymore so NOTHING IS MY FAULT!
// Use #define LOOK_AND_FEEL in addrbook.h to turn on / off most changes
// 
// 3     2/20/98 12:15p Johno
// Compile update
// 
// 2     2/10/98 11:05a Johno
// Use ADDRESS_BOOK_COLOR in CPmwDoc::EditNameList
// 
//    Rev 1.0   14 Aug 1997 15:22:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:06   Fred
// Initial revision.
// 
//    Rev 1.7   17 Jul 1996 08:17:02   Jay
// Now reflows macros in ViewRecord().
// 
//    Rev 1.6   16 Jul 1996 17:33:30   Fred
// More address/sender book stuff
// 
//    Rev 1.5   16 Jul 1996 08:51:20   Fred
// Hooked up address/sender book
// 
//    Rev 1.4   28 Jun 1996 16:18:58   Jay
//  
// 
//    Rev 1.3   26 Jun 1996 14:23:40   Jay
//  
// 
//    Rev 1.2   05 Jun 1996 08:21:48   Jay
//  
// 
//    Rev 1.1   07 May 1996 12:44:06   Jay
// From PMW2
// 
//    Rev 1.27   07 May 1996 08:21:44   JAY
// Resets fields and reflows macros if address book is not available.
// 
//    Rev 1.26   18 Jan 1996 17:09:48   JAY
// New project configuration class
// 
//    Rev 1.25   29 Sep 1995 16:01:04   JAY
// More drag/drop/paste/OLE stuff
// 
//    Rev 1.24   09 Sep 1995 08:44:40   JAY
// MacrosInUse and ReflowMacros now use the master object list, not the
// current one. This fixes a problem with cards (which change the current one).
// 
//    Rev 1.23   08 Sep 1995 11:05:48   JAY
// Banner resizing fixes.
// 
//    Rev 1.22   27 Aug 1995 12:34:14   FRED
// New fill-in-fields stuff
// 
//    Rev 1.21   25 Aug 1995 15:00:48   JAY
// Insert fields dialog
// 
//    Rev 1.20   24 Aug 1995 14:07:44   JAY
// Removed unused code.
// 
//    Rev 1.19   23 Aug 1995 11:18:48   JAY
// Fixed undo'ing of a fill-in field insertion.
// 
//    Rev 1.18   22 Aug 1995 12:59:06   FRED
// Merges in all known fields
// 
//    Rev 1.17   16 Aug 1995 16:23:36   JAY
//  
// 
//    Rev 1.16   14 Aug 1995 09:36:32   JAY
// Text undo now handles fill-in fields correctly.
// 
//    Rev 1.15   10 Aug 1995 17:13:50   JAY
// Now recurses into calendar objects (like groups) during MacrosInUse.
// 
//    Rev 1.14   09 Aug 1995 13:12:52   JAY
// Misc changes.
// 
//    Rev 1.13   08 Aug 1995 12:10:36   FRED
// Open databases read-only
// 
//    Rev 1.12   07 Aug 1995 07:48:14   FRED
// Standard fill-in fields.
// 
//    Rev 1.11   06 Aug 1995 09:45:42   FRED
// More fill-in fields stuff.
// 
//    Rev 1.10   04 Aug 1995 10:47:08   FRED
// Sender fields.
// 
//    Rev 1.9   03 Aug 1995 08:00:30   FRED
// Fill in stuff.
// 
//    Rev 1.8   02 Aug 1995 08:07:22   FRED
// Fill-in fields stuff.
// 
//    Rev 1.7   31 Jul 1995 07:28:02   FRED
// Support for editing sender.
// 
//    Rev 1.6   28 Jul 1995 06:55:24   FRED
// Fixed bug where wrong address book page was being selected.
// 
// 
//    Rev 1.5   26 Jul 1995 10:52:26   FRED
// New Address Book interface
// 
//    Rev 1.4   11 Jul 1995 09:42:04   JAY
// Initial fill-ins dialog for doc.
// 
//    Rev 1.3   02 May 1995 17:09:20   JAY
// Fixed some smart fields bugs.
// 
//    Rev 1.2   01 May 1995 15:27:58   JAY
// Restructured the macro stuff
// 
//    Rev 1.1   19 Apr 1995 12:33:04   JAY
//  
// 
//    Rev 1.0   29 Mar 1995 09:20:32   JAY
// Initial revision.
// 
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"

#include "macrorec.h"
#include "namerec.h"
#include "textrec.h"
#include "macrodlg.h"
#include "frameobj.h"
#include "grpobj.h"
#include "tblobj.h"

#include "addrbook.h"
#include "dlgmgr.h"
#include "addrbook.h"
#include "macrostd.h"
#include "pmwcfg.h"

/////////////////////////////////////////////////////////////////////////////
// CPmwView support for macros.

void CPmwView::InsertCharacter(CHARACTER c)
{
	CPmwDoc* pDoc = GetDocument();

	FLAGS flags = m_Txp.Flags();
	PBOX bound;

	if (flags & CTxp::FLAG_stretch_frame)
	{
		bound = m_Txp.Object()->get_bound();
	}

#if 0
	if (m_Txp.SomethingSelected())
	{
		m_Txp.DeleteText(1);
	}
#endif

/* Insert the actual character. */

	UpdateUndoTyping(&m_Txp, c, 1);
	m_Txp.InsertText(&c, 1, TRUE);

/* See if the txp moved. */

	TxpCursorOn();

/* We have been modified. */

	pDoc->SetModifiedFlag();

	CFrameObject* pObject = m_Txp.Object();
	if (flags & CTxp::FLAG_stretch_frame)
	{
		PBOX new_bound = pObject->get_bound();

		if (new_bound.x1 != bound.x1)
		{
		/* We need to resize the project. */
			pDoc->resize_project(pObject);
		}
	}
	else if (pObject->get_panel() == OBJECT_IN_CELL_PANEL)
	{
		pDoc->resize_project(pObject);
	}
	reframe_on_txp();
}

/////////////////////////////////////////////////////////////////////////////
// CPmwDoc support for macros.

/*
// Macro support.
*/

void CPmwDoc::ReflowMacros(CWordArray* pMacroList /*=NULL*/)
{
	// We will run through all the stories in this document looking for
	// fill-in fields to reflow.

	PMGDatabase* pDatabase = get_database();
	DWORD nStories = document->NumberOfStories();
	for (DWORD nStory = 0; nStory < nStories; nStory++)
	{
		DB_RECORD_NUMBER lStory = document->GetStory(nStory);
		CTextRecord* pText = (CTextRecord*)pDatabase->get_record(lStory, NULL, RECORD_TYPE_Text);
		if (pText != NULL)
		{
			pText->ReflowMacros(pMacroList);
			pText->release();
		}
	}
	resize_project();
}

void CPmwDoc::GetMacrosInUse(CWordArray& cwaMacros)
{
	// We will run through all the stories in this document looking for
	// fill-in fields.

	PMGDatabase* pDatabase = get_database();
	DWORD nStories = document->NumberOfStories();
	for (DWORD nStory = 0; nStory < nStories; nStory++)
	{
		DB_RECORD_NUMBER lStory = document->GetStory(nStory);
		CTextRecord* pText = (CTextRecord*)pDatabase->get_record(lStory, NULL, RECORD_TYPE_Text);
		if (pText != NULL)
		{
			pText->GetMacrosInUse(cwaMacros);
			pText->release();
		}
	}
}

/*
// Edit the name list.
*/
  
BOOL CPmwDoc::EditNameList(BOOL fSelectReceivers /*=FALSE*/)
{
	BOOL fResult = CAddressBook::Run(NULL, mapping.m_pMacroServer->GetNameList(), fSelectReceivers, FALSE, ADDRESS_BOOK_COLOR);

	if (fResult)
	{
		mapping.m_pMacroServer->AfterAddressBookRun(NULL, this);
	}

	return fResult;
}

/*
// Edit the sender.
*/

BOOL CPmwDoc::EditSender(void)
{
	BOOL fResult = CSelectSenderDialog::Run(NULL);

	if (fResult)
	{
		SetModifiedFlag();
		ViewSampleRecord();
	}

	return fResult;
}

/*
// View record.
*/

BOOL CPmwDoc::ViewRecord(DWORD dwKey)
{
	BOOL fResult = FALSE;

	ERRORCODE Error = ERRORCODE_None;

	StorageFile* pDatabaseFile = NULL;
	CFlatFileDatabase* pDatabase = NULL;
	CFlatFileDatabaseCursor* pCursor = NULL;

	// Construct the full name of the address book we're using.

	CString csName = GetPathManager()->LocatePath(mapping.m_pMacroServer->GetNameList()->AddressBookName());
	if (!csName.IsEmpty())
	{
		TRACE("Open database %s for data!\r\n", (LPCSTR)csName);

		// Open up the address book, do not create it.
		Error = CAddressBook::OpenAddressBookDatabase(csName, pDatabaseFile, pDatabase, FALSE);
		if (Error == ERRORCODE_None)
		{
			TRY
			{
				// Create a cursor on the database.
				pCursor = new CFlatFileDatabaseCursor;
				pCursor->Attach(pDatabase);

				// Bind to the database record.
				mapping.m_pMacroServer->BindToRecord(pCursor, dwKey);
				ReflowMacros();

				fResult = TRUE;
			}
			CATCH_ALL(e)
			{
				if (e->IsKindOf(RUNTIME_CLASS(CErrorcodeException)))
				{
					Error = ((CErrorcodeException*)e)->m_error;
				}
				else
				{
					Error = ERRORCODE_IntError;
				}
			}
			END_CATCH_ALL
		}
	}

	delete pCursor;
	pCursor = NULL;
	CAddressBook::CloseAddressBookDatabase(pDatabaseFile, pDatabase);

	if (Error != ERRORCODE_None)
	{
		fResult = FALSE;
	}

	return fResult;
}

/*
// View the sample record in the document.
*/

void CPmwDoc::ViewSampleRecord(void)
{
	if (!GetConfiguration()->SupportsAddressBook(FALSE))
	{
	/* Reset all field values to nothing (field names). */
		mapping.m_pMacroServer->ClearFieldValues();
		ReflowMacros();
	}
	else
	{
		if (m_wDocumentFlags & FLAG_ClipDocument)
		{
			return;
		}

		CString csName = GetPathManager()->LocatePath(mapping.m_pMacroServer->GetNameList()->AddressBookName());

		if (!csName.IsEmpty())
		{
			// get sender values for address book.	
			mapping.m_pMacroServer->BindSender();
		}
		
	/*
	// If there is at least one name, grab the first name.
	*/

		NameListRecord* pNameList = mapping.m_pMacroServer->GetNameList();

		if ((pNameList->Names() == 0)					// No names.
		 || (!ViewRecord(pNameList->Name(0))))		// Could not bind.
		{
		/* Reset all field values to nothing (field names). */
			mapping.m_pMacroServer->ClearReceiverFieldValues();
			ReflowMacros();
		}
	}
}

/*
// Does the document contain a database macro?
*/

BOOL CPmwDoc::HasDatabaseMacro(void)
{
	CMacroList& AMacroList = mapping.m_pMacroServer->GetMacroList()->MacroList();
	CWordArray cwaMacros;

	GetMacrosInUse(cwaMacros);

#if 1
	return cwaMacros.GetSize() != 0;
#else
/* See if we have any "merge" (i.e. address book field) macros. */

	BOOL fHasField = FALSE;

	int nMacros = cwaMacros.GetSize();

	for (int nMacro = 0; nMacro < nMacros; nMacro++)
	{
		CMacro* pMacro = AMacroList.FindMacro(cwaMacros.GetAt(nMacro) - MACRO_CHARACTER);
		ASSERT(pMacro != NULL);
		if (pMacro != NULL && pMacro->ValueType() == CMacro::VALUE_TYPE_Field)
		{
			return TRUE;
		}
	}
	return FALSE;
#endif
}

BOOL CPmwDoc::CreateMacroList()
{
/*
// Create the macro list record.
*/

	MacroListRecord* pMacroRecord;

	if ((pMacroRecord = (MacroListRecord*)get_database()->new_record(RECORD_TYPE_MacroList, NULL)) != NULL)
	{
		pMacroRecord->ShowValues(FALSE);

		ASSERT(document != NULL);
		if (document != NULL)
		{
			document->set_macro_list_record(pMacroRecord->Id());
		}

	/*
	// Hook us into the macro server.
	*/

		mapping.m_pMacroServer->SetMacroList(pMacroRecord);

	/*
	// Set up macro names corresponding to address book fields.
	*/

		mapping.m_pMacroServer->AddFieldMacrosToList();

	/*
	// Set up the "global" string macro names.
	*/

		mapping.m_pMacroServer->AddGlobalMacrosToList();

		return TRUE;
	}
	return FALSE;
}

BOOL CPmwDoc::GetMacroList()
{
	DB_RECORD_NUMBER record;

	if ((record = document->get_macro_list_record()) != 0)
	{
		MacroListRecord* pMacroRecord;
		if ((pMacroRecord = (MacroListRecord*)get_database()->get_record(record, NULL, RECORD_TYPE_MacroList)) != NULL)
		{
			pMacroRecord->ShowValues(FALSE);

			mapping.m_pMacroServer->SetMacroList(pMacroRecord);
			mapping.m_pMacroServer->AddFieldMacrosToList();
			mapping.m_pMacroServer->AddGlobalMacrosToList();
			return TRUE;
		}
	}

	return CreateMacroList();
}
