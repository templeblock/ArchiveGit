/*
// $Header: /PM8/App/DOCREC.CPP 1     3/03/99 6:05p Gbeddow $
//
// Document record routines.
//
// $Log: /PM8/App/DOCREC.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 15    9/25/98 11:07a Mwilson
// added support for instruction page
// 
// 14    9/14/98 12:08p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 13    7/01/98 4:16p Johno
// Added DocumentRecord::NumberOfHyperlinks()
// 
// 12    6/29/98 3:26p Johno
// Fixed ASSERT in AppendFullURLString()
// 
// 11    4/16/98 4:24p Johno
// Split AppendFullURL() into AppendFullURL() and AppendFullURLString()
// 
// 10    4/08/98 3:28p Johno
// Added completed handling of TYPE_FilePath in AppendFullURL
// 
// 9     4/03/98 5:54p Johno
// Fixed bad ASSERT in AppendFullURL
// 
// 8     4/03/98 4:23p Johno
// Added AppendFullURL()
// 
// 7     3/25/98 6:29p Johno
// HyperLink and Hyperlink now all Hyperlink for sanity
// 
// 6     3/10/98 5:24p Jayn
// Master page changes
// 
// 5     3/09/98 9:27a Johno
// Compile update
// 
// 4     3/06/98 5:44p Johno
// Hyperlink stuff
// 
// 3     11/12/97 6:19p Hforman
// took out guide arrays -- now in page's object list
// 
// 2     11/05/97 10:41a Hforman
// add arrays for vertical/horizontal guides
// 
//    Rev 1.0   14 Aug 1997 15:20:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:12   Fred
// Initial revision.
// 
//    Rev 1.14   06 Jan 1997 10:13:34   Jay
// Optimizations
// 
//    Rev 1.13   05 Nov 1996 14:06:46   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.12   28 Jul 1996 11:59:18   Jay
// Now deletes more records it should.
// 
//    Rev 1.11   26 Jul 1996 11:38:50   Jay
// Page stuff
// 
//    Rev 1.10   25 Jul 1996 19:38:08   Jay
//  
// 
//    Rev 1.9   25 Jul 1996 11:16:24   Jay
// Page stuff
// 
//    Rev 1.8   17 Jul 1996 14:10:06   Jay
// Database bookkeeping
// 
//    Rev 1.7   08 Jul 1996 17:51:26   Jay
// Linked frames, etc.
// 
//    Rev 1.6   02 Jul 1996 08:55:00   Jay
//  
// 
//    Rev 1.5   28 Jun 1996 16:18:10   Jay
//  
// 
//    Rev 1.4   24 Jun 1996 18:06:08   Jay
//  
// 
//    Rev 1.3   19 Jun 1996 17:32:08   Jay
//  
// 
//    Rev 1.2   04 Jun 1996 17:37:50   Jay
//  
// 
//    Rev 1.1   23 Apr 1996 08:07:52   Jay
// More new stuff
// 
//    Rev 1.0   14 Mar 1996 13:48:06   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:14   FRED
// Initial revision.
// 
//    Rev 2.8   11 Aug 1995 11:19:36   JAY
// 
//    Rev 2.7   11 Aug 1995 09:27:52   JAY
// Tries to handle bad flags field in ReadData.
// 
//    Rev 2.6   20 Jul 1995 12:48:26   JAY
// Now owns, load and saves a CPaperInfo.
// 
//    Rev 2.5   12 Jul 1995 17:43:36   JAY
// New project sub-types (esp. for cards).
// 
//    Rev 2.4   05 Jul 1995 11:17:40   JAY
// Rearranged includes to reduce symbol count.
// 
//    Rev 2.3   13 May 1995 11:14:24   JAY
//  
// 
//    Rev 2.2   11 May 1995 09:38:00   JAY
// Changed warp field CreateStruct to an external class.
// 
//    Rev 2.1   27 Apr 1995 13:21:32   FRED
//  
// 
//    Rev 2.0   07 Feb 1995 15:47:40   JAY
// Initial revision.
// 
//    Rev 1.8   30 Nov 1994 16:38:22   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.7   04 Aug 1994 07:34:06   JAY
// Large model and 32-bit changes
// 
//    Rev 1.6   13 Jul 1994 11:58:40   JAY
// New flag to re-initialize graphics.
// 
//    Rev 1.5   01 Jun 1994 16:35:16   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.4   23 May 1994 08:16:42   JAY
// Made find_graphic() take an LPCSTR instead of an LPSTR.
// 
//    Rev 1.3   19 Jan 1994 17:06:36   JAY
//  
// 
//    Rev 1.2   13 Jan 1994 17:00:12   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:05:40   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:24:56   JAY
// 
*/

#include "stdafx.h"

#include <string.h>

#include "pmgdb.h"

#include "objlist.h"
#include "docrec.h"
#include "fontrec.h"
#include "layrec.h"
#include "warpfld.h"
#include "pathrec.h"
#include "pagerec.h"
#include "paper.h"
#include "recarray.h"
#include "stylerec.h"
#include "hyperrec.h"   // For GetFullURL()
#include "htmlconv.h"   // For CHTMLDocConverter::GetHTMLFileName()
#include "util.h"       // For DocumentRecord::AppendFullURL
/////////////////////////////////////////////////////////////////////////////
// Document record

/*
// The creator for a document record.
*/

ERRORCODE DocumentRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	DocumentPtr pDocument = NULL;

/* Create the new array record. */

	TRY
		pDocument = new DocumentRecord(number, type, owner, where);
	END_TRY

	if (pDocument == NULL)
	{
		return ERRORCODE_Memory;
	}

/* Say what we allocated. */

	*record = (DatabaseRecordPtr)pDocument;

/* Plug in the data from the create struct if one was passed. */

	if (creation_data != NULL)
	{
		DOC_CREATE_STRUCT_PTR dcs = (DOC_CREATE_STRUCT_PTR)creation_data;

		pDocument->record.project_type = dcs->project_type;
		pDocument->record.m_nProjectSubtype = dcs->m_nProjectSubtype;
		pDocument->record.orientation = dcs->orientation;
		pDocument->record.paper_width = dcs->paper_width;
		pDocument->record.paper_height = dcs->paper_height;
		pDocument->record.dimensions = dcs->dimensions;
		*(pDocument->m_pPaperInfo) = *(dcs->m_pPaperInfo);
	}

	return ERRORCODE_None;
}

/*
// The constructor for a document record.
*/

DocumentRecord::DocumentRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where),
				  m_pCurrentPage(NULL),
				  m_pCurrentMasterPage(NULL),
				  m_pCurrentInstructionPage(NULL)
{
	memset(&record, 0, sizeof(record));
	m_pPaperInfo = new CPaperInfo;

   m_pPageArray = NULL;
	m_pFontArray = NULL;
	m_pLayoutArray = NULL;
	m_pGraphicArray = NULL;
	m_pWarpFieldArray = NULL;
	m_pOutlinePathArray = NULL;
	m_pTextStyleArray = NULL;
	m_pStoryArray = NULL;
   m_pHyperlinkArray = NULL;
   m_pMasterPageArray = NULL;
   m_pInstructionPageArray = NULL;
}

/*
// The destructor for a document record.
*/

DocumentRecord::~DocumentRecord()
{
	ReleaseSubrecords();
	delete m_pPaperInfo;
}

void DocumentRecord::ReleaseSubrecords(void)
{
	// Release all the subrecords.
	ReleaseSubrecord((DatabaseRecord*&)m_pPageArray);
	ReleaseSubrecord((DatabaseRecord*&)m_pFontArray);
	ReleaseSubrecord((DatabaseRecord*&)m_pLayoutArray);
	ReleaseSubrecord((DatabaseRecord*&)m_pGraphicArray);
	ReleaseSubrecord((DatabaseRecord*&)m_pWarpFieldArray);
	ReleaseSubrecord((DatabaseRecord*&)m_pOutlinePathArray);
	ReleaseSubrecord((DatabaseRecord*&)m_pTextStyleArray);
	ReleaseSubrecord((DatabaseRecord*&)m_pStoryArray);
   ReleaseSubrecord((DatabaseRecord*&)m_pHyperlinkArray);
   ReleaseSubrecord((DatabaseRecord*&)m_pMasterPageArray);
   ReleaseSubrecord((DatabaseRecord*&)m_pInstructionPageArray);

	unload_page();
	UnloadMasterPage();
	UnloadInstructionPage();
}

void DocumentRecord::destroy(void)
{
	// Release all records before the delete.
	ReleaseSubrecords();

	// Delete all allocated records.
	DeleteSubrecord(record.m_lPageArray, RECORD_TYPE_RecordArray);
	DeleteSubrecord(record.m_lFontArray, RECORD_TYPE_RecordArray);
	DeleteSubrecord(record.m_lLayoutArray, RECORD_TYPE_RecordArray);
	DeleteSubrecord(record.m_lGraphicArray, RECORD_TYPE_RecordArray);
	DeleteSubrecord(record.m_lWarpFieldArray, RECORD_TYPE_RecordArray);
	DeleteSubrecord(record.m_lOutlinePathArray, RECORD_TYPE_RecordArray);
	DeleteSubrecord(record.m_lTextStyleArray, RECORD_TYPE_RecordArray);
	DeleteSubrecord(record.m_lStoryArray, RECORD_TYPE_RecordArray);
	DeleteSubrecord(record.m_lHyperlinkArray, RECORD_TYPE_RecordArray);
	DeleteSubrecord(record.m_lMasterPageArray, RECORD_TYPE_RecordArray);
	DeleteSubrecord(record.m_lInstructionPageArray, RECORD_TYPE_RecordArray);

	DeleteSubrecord(record.info_record, DB_ANY_TYPE);
	DeleteSubrecord(record.macro_list_record, RECORD_TYPE_MacroList);
	DeleteSubrecord(record.name_list_record, RECORD_TYPE_NameList);
}

void DocumentRecord::LockPageArray(void)
{
	if (m_pPageArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pPageArray, record.m_lPageArray, RECORD_TYPE_RecordArray, TRUE);
	}
}

void DocumentRecord::LockFontArray(void)
{
	if (m_pFontArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pFontArray, record.m_lFontArray, RECORD_TYPE_RecordArray, TRUE);
	}
}

void DocumentRecord::LockLayoutArray(void)
{
	if (m_pLayoutArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pLayoutArray, record.m_lLayoutArray, RECORD_TYPE_RecordArray, TRUE);
	}
}

void DocumentRecord::LockGraphicArray(void)
{
	if (m_pGraphicArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pGraphicArray, record.m_lGraphicArray, RECORD_TYPE_RecordArray, TRUE);
	}
}

void DocumentRecord::LockWarpFieldArray(void)
{
	if (m_pWarpFieldArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pWarpFieldArray, record.m_lWarpFieldArray, RECORD_TYPE_RecordArray, TRUE);
	}
}

void DocumentRecord::LockOutlinePathArray(void)
{
	if (m_pOutlinePathArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pOutlinePathArray, record.m_lOutlinePathArray, RECORD_TYPE_RecordArray, TRUE);
	}
}

void DocumentRecord::LockTextStyleArray(void)
{
	if (m_pTextStyleArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pTextStyleArray, record.m_lTextStyleArray, RECORD_TYPE_RecordArray, TRUE);
	}
}

void DocumentRecord::LockStoryArray(void)
{
	if (m_pStoryArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pStoryArray, record.m_lStoryArray, RECORD_TYPE_RecordArray, TRUE);
	}
}

void DocumentRecord::LockHyperlinkArray(void)
{
	if (m_pHyperlinkArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pHyperlinkArray, record.m_lHyperlinkArray, RECORD_TYPE_RecordArray, TRUE);
	}
}

void DocumentRecord::LockMasterPageArray(void)
{
	if (m_pMasterPageArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pMasterPageArray, record.m_lMasterPageArray, RECORD_TYPE_RecordArray, TRUE);
	}
}

void DocumentRecord::LockInstructionPageArray(void)
{
	if (m_pInstructionPageArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pInstructionPageArray, record.m_lInstructionPageArray, RECORD_TYPE_RecordArray, TRUE);
	}
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE DocumentRecord::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read_record(&record, sizeof(record))) == ERRORCODE_None)
	{
		TRY
		{
			BOOL fReadPaperInfo = TRUE;

			// Convert the old arrays if we need to.
			if (!(record.m_wFlags & FLAG_HasNewStyleArrays))
			{
				ConvertArray(device, record.m_lPageArray);
				ConvertArray(device, record.m_lFontArray);
				ConvertArray(device, record.m_lLayoutArray);
				ConvertArray(device, record.m_lGraphicArray);

				if (record.m_wFlags & FLAG_HasWarpFields)
				{
					ConvertArray(device, record.m_lWarpFieldArray);
				}
				if (record.m_wFlags & FLAG_HasOutlinePaths)
				{
					ConvertArray(device, record.m_lOutlinePathArray);
				}
				if (!(record.m_wFlags & FLAG_HasPaperInfo))
				{
					fReadPaperInfo = FALSE;
				}
			}

			// Now read the paper info if we have one to read.
			if (fReadPaperInfo)
			{
				if ((error = m_pPaperInfo->ReadData(device)) != ERRORCODE_None)
				{
					// Construct an object to get the "unread" state.
					CPaperInfo BlankInfo;
					*m_pPaperInfo = BlankInfo;
					error = ERRORCODE_None;
				}
			}
		}
		CATCH(CErrorcodeException, e)
		{
			error = e->m_error;
		}
		AND_CATCH_ALL(e)
		{
			ASSERT(FALSE);
		}
		END_CATCH_ALL
	}

	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE DocumentRecord::WriteData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	// Make sure...
	record.m_wFlags |= FLAG_HasNewStyleArrays;
	// No longer needed.
	record.m_wFlags &= ~(FLAG_HasWarpFields | FLAG_HasOutlinePaths | FLAG_HasPaperInfo);

	if ((error = pDevice->write_record(&record, sizeof(record))) == ERRORCODE_None
			&& (error = m_pPaperInfo->WriteData(pDevice)) == ERRORCODE_None)
	{
	}

	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE DocumentRecord::SizeofData(StorageDevicePtr pDevice)
{
	return pDevice->size_record(sizeof(record))
				+ m_pPaperInfo->SizeofData(pDevice);
}

/*
// Convert a record array at read time.
// This routine can throw exceptions.
*/

void DocumentRecord::ConvertArray(StorageDevice* pDevice, DB_RECORD_NUMBER& lRecord)
{
	RecordArray Array;
	ERRORCODE error;

	// We shouldn't have a record yet!
	if (lRecord != 0)
	{
		ASSERT(FALSE);
		ThrowErrorcodeException(ERRORCODE_IntError);
	}

	// Read the old array.
	if ((error = Array.read(pDevice)) != ERRORCODE_None)
	{
		ThrowErrorcodeException(error);
	}

	ST_DEV_POSITION Here;
	pDevice->tell(&Here);

	// Create the new array. We do this via the generic locking routine.
	CRecordArrayRecord* pRecord = NULL;

	LockSubrecord((DatabaseRecord*&)pRecord, lRecord, RECORD_TYPE_RecordArray, TRUE);
	ASSERT(pRecord != NULL);
	ASSERT(lRecord != 0);

	TRY
	{
		pRecord->Add(Array.get_element(0), Array.count());
	}
	CATCH_ALL(e)
	{
		// Try to clean up before we go on.
		// Ignore any exceptions generated while cleaning up.
		TRY
			ReleaseSubrecord((DatabaseRecord*&)pRecord);
			ASSERT(pRecord == NULL);
			DeleteSubrecord(lRecord, RECORD_TYPE_RecordArray);
			ASSERT(lRecord == 0);
		END_TRY
		// And report our exception to our superiors.
		THROW_LAST();
	}
	END_CATCH_ALL

	// Success. Release the record.
	ReleaseSubrecord((DatabaseRecord*&)pRecord);
	ASSERT(pRecord == NULL);

	pDevice->seek(Here, ST_DEV_SEEK_SET);
}

/////////////////////////////////////////////////////////////////////////////
// Page array routines.

/*
// Create a new page and insert it into the document list.
*/

ERRORCODE DocumentRecord::new_page(DB_RECORD_NUMBER far *rnp, DWORD dwIndex /*=(DWORD)-1*/)
{
	PagePtr page;
	DB_RECORD_NUMBER page_record;
	ERRORCODE error;

/* Make it a bad number. */

	*rnp = 0;

/* Create the page. */

	if ((page = ((PMGDatabasePtr)database)->create_page()) == NULL)
	{
		return ((PMGDatabasePtr)database)->last_creation_error();
	}

/* Get the record number. */

	page_record = page->Id();

/* Unlock the record. */

	page->release();

/* Hook it into the page array. */

	error = AddPage(page_record, dwIndex);

	if (error == ERRORCODE_None)
	{
		*rnp = page_record;
	}
	else
	{
	/* Couldn't add. Delete our attempt. */
		database->delete_record(page_record);
	}
	return error;
}

/*
// Delete a page from this document.
*/

void DocumentRecord::delete_page(DB_RECORD_NUMBER page_record)
{
	RemovePage(page_record);
	database->delete_record(page_record);
}

ERRORCODE DocumentRecord::AddPage(DB_RECORD_NUMBER lPage, DWORD dwIndex /*=(DWORD)-1*/)
{
	ERRORCODE error;

	TRY
	{
		LockPageArray();
		if (dwIndex == (DWORD)-1)
		{
			m_pPageArray->AddRecord(lPage);
		}
		else
		{
			m_pPageArray->InsertRecord(lPage, dwIndex);
		}
		error = ERRORCODE_None;
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL
	return error;
}

void DocumentRecord::RemovePage(DB_RECORD_NUMBER lPage)
{
	TRY
	{
		LockPageArray();
		m_pPageArray->RemoveRecord(lPage);
	}
	END_TRY
}

/*
// Load a page making it current.
*/

ObjectListPtr DocumentRecord::load_page(DB_RECORD_NUMBER page_record)
{
/* Just return the current page if passed -1. */

	if (page_record == -1)
	{
		if (m_pCurrentPage == NULL)
		{
		/* Not a good time to call. */
			return NULL;
		}
	/* Return the list. */
		m_pCurrentPage->modified();
		return m_pCurrentPage->objects();
	}

/* See what page we need to load. */

	if (page_record == 0)
	{
	/* Use the current one. */
		page_record = record.last_loaded_page;
	}

/* Unload any current page. */

	if (page_record != ((PMGDatabase*)database)->CurrentPage())
	{
		unload_page();

		// Inform the database about this load. The database keeps the last-loaded
		// page record number as a convenience for whomever wants it.
		// We need to set this before the page is loaded because objects being
		// converted during the page read may need to know what page they are on.

		((PMGDatabase*)database)->CurrentPage(page_record);

	/* Get the record. */

		if ((m_pCurrentPage = (PagePtr)database->get_record(page_record,
							NULL,
							RECORD_TYPE_Page)) == NULL)
		{
			((PMGDatabase*)database)->CurrentPage(0);
			return NULL;
		}

	/* This is now current. */

		record.last_loaded_page = page_record;
	}
	ASSERT(m_pCurrentPage != NULL);

/* We assume we will modify this (since we can't be sure). */

	m_pCurrentPage->modified();

/* Pass back a pointer to the object list. */

	return m_pCurrentPage->objects();
}

/*
// Unload the current page.
*/

void DocumentRecord::unload_page(VOID)
{
	if (m_pCurrentPage != NULL)
	{
		m_pCurrentPage->release(TRUE);
		m_pCurrentPage = NULL;
	}
}

DWORD DocumentRecord::NumberOfPages(void)
{
	TRY
	{
		LockPageArray();
		return m_pPageArray->ElementCount();
	}
	END_TRY
	return 0;
}

DB_RECORD_NUMBER DocumentRecord::GetPage(DWORD dwPage)
{
	TRY
	{
		LockPageArray();
		return m_pPageArray->GetRecord(dwPage);
	}
	END_TRY
	return 0;
}

DWORD DocumentRecord::IndexOfPage(DB_RECORD_NUMBER lPage)
{
	TRY
	{
		LockPageArray();
		return m_pPageArray->FindRecord(lPage);
	}
	END_TRY
	return (DWORD)-1;
}

/////////////////////////////////////////////////////////////////////////////
// Master page routines.

/*
// Create a new master page and insert it into the document list.
*/

ERRORCODE DocumentRecord::NewMasterPage(DB_RECORD_NUMBER far *plPage /*=NULL*/, DWORD dwIndex /*=(DWORD)-1*/)
{
	PagePtr pPage;
	DB_RECORD_NUMBER lPage;
	ERRORCODE error;

	// Make it a bad number.
	if (plPage != NULL)
	{
		*plPage = 0;
	}

	// Create the page.
	if ((pPage = ((PMGDatabasePtr)database)->create_page()) == NULL)
	{
		return ((PMGDatabasePtr)database)->last_creation_error();
	}

	// Get the record number.
	lPage = pPage->Id();

	// We do not see master items on a master page.
	pPage->objects()->GetPageProperties()->SetHideMasterItems(-1);

	// Unlock the record.
	pPage->release();

	// Hook it into the page array.
	error = AddMasterPage(lPage, dwIndex);

	if (error == ERRORCODE_None)
	{
		// No error. Set the outgoing page record number if desired.
		if (plPage != NULL)
		{
			*plPage = lPage;
		}
	}
	else
	{
		// Could not add. Delete our attempt.
		database->delete_record(lPage);
	}
	return error;
}

ERRORCODE DocumentRecord::AddMasterPage(DB_RECORD_NUMBER lPage, DWORD dwIndex /*=(DWORD)-1*/)
{
	ERRORCODE error;

	TRY
	{
		LockMasterPageArray();
		if (dwIndex == (DWORD)-1)
		{
			m_pMasterPageArray->AddRecord(lPage);
		}
		else
		{
			m_pMasterPageArray->InsertRecord(lPage, dwIndex);
		}
		error = ERRORCODE_None;
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL
	return error;
}

void DocumentRecord::RemoveMasterPage(DB_RECORD_NUMBER lPage)
{
	TRY
	{
		LockMasterPageArray();
		m_pMasterPageArray->RemoveRecord(lPage);
	}
	END_TRY
}

DWORD DocumentRecord::NumberOfMasterPages(void)
{
	if (GetSided() != NotSided)
	{
		TRY
		{
			LockMasterPageArray();
			return m_pMasterPageArray->ElementCount();
		}
		END_TRY
	}
	return 0;
}

DB_RECORD_NUMBER DocumentRecord::GetMasterPage(DWORD dwPage)
{
	if (GetSided() != NotSided)
	{
		TRY
		{
			LockMasterPageArray();
			return m_pMasterPageArray->GetRecord(dwPage);
		}
		END_TRY
	}
	return 0;
}

DWORD DocumentRecord::IndexOfMasterPage(DB_RECORD_NUMBER lPage)
{
	TRY
	{
		LockMasterPageArray();
		return m_pMasterPageArray->FindRecord(lPage);
	}
	END_TRY
	return (DWORD)-1;
}

//
// Load a master page by record number.
//

ObjectList* DocumentRecord::LoadMasterPageRecord(DB_RECORD_NUMBER lPage)
{
	// Just return the current page if passed -1.
	if (lPage == -1)
	{
		if (m_pCurrentMasterPage == NULL)
		{
			// Not a good time to call.
			return NULL;
		}
		// Return the list.
		return m_pCurrentMasterPage->objects();
	}

	// Unload any current page.
	if (m_pCurrentMasterPage == NULL
			|| m_pCurrentMasterPage->Id() != lPage)
	{
		UnloadMasterPage();

		// Get the record.
		if ((m_pCurrentMasterPage = (PagePtr)database->get_record(lPage,
							NULL,
							RECORD_TYPE_Page)) == NULL)
		{
			return NULL;
		}
	}
	ASSERT(m_pCurrentMasterPage != NULL);

	// Pass back a pointer to the object list.
	return m_pCurrentMasterPage->objects();
}

//
// Load a master page by index.
//

ObjectList* DocumentRecord::LoadMasterPage(DWORD dwPage)
{
	if (dwPage < NumberOfMasterPages())
	{
		return LoadMasterPageRecord(GetMasterPage(dwPage));
	}
	return NULL;
}

//
// Unload the current master page.
//

void DocumentRecord::UnloadMasterPage(void)
{
	if (m_pCurrentMasterPage != NULL)
	{
		m_pCurrentMasterPage->release();
		m_pCurrentMasterPage = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Instruction page routines.

/*
// Create a new Instruction page and insert it into the document list.
*/

ERRORCODE DocumentRecord::NewInstructionPage(DB_RECORD_NUMBER far *plPage /*=NULL*/, DWORD dwIndex /*=(DWORD)-1*/)
{
	PagePtr pPage;
	DB_RECORD_NUMBER lPage;
	ERRORCODE error;

	// Make it a bad number.
	if (plPage != NULL)
	{
		*plPage = 0;
	}

	// Create the page.
	if ((pPage = ((PMGDatabasePtr)database)->create_page()) == NULL)
	{
		return ((PMGDatabasePtr)database)->last_creation_error();
	}

	// Get the record number.
	lPage = pPage->Id();

	// We do not see master items on a master page.
	pPage->objects()->GetPageProperties()->SetHideMasterItems(-1);

	// Unlock the record.
	pPage->release();

	// Hook it into the page array.
	error = AddInstructionPage(lPage, dwIndex);

	if (error == ERRORCODE_None)
	{
		// No error. Set the outgoing page record number if desired.
		if (plPage != NULL)
		{
			*plPage = lPage;
		}
	}
	else
	{
		// Could not add. Delete our attempt.
		database->delete_record(lPage);
	}
	return error;
}

ERRORCODE DocumentRecord::AddInstructionPage(DB_RECORD_NUMBER lPage, DWORD dwIndex /*=(DWORD)-1*/)
{
	ERRORCODE error;

	TRY
	{
		LockInstructionPageArray();
		if (dwIndex == (DWORD)-1)
		{
			m_pInstructionPageArray->AddRecord(lPage);
		}
		else
		{
			m_pInstructionPageArray->InsertRecord(lPage, dwIndex);
		}
		error = ERRORCODE_None;
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL
	return error;
}

void DocumentRecord::RemoveInstructionPage(DB_RECORD_NUMBER lPage)
{
	TRY
	{
		LockInstructionPageArray();
		m_pInstructionPageArray->RemoveRecord(lPage);
	}
	END_TRY
}

DWORD DocumentRecord::NumberOfInstructionPages(void)
{
	TRY
	{
		LockInstructionPageArray();
		return m_pInstructionPageArray->ElementCount();
	}
	END_TRY
	return 0;
}

DB_RECORD_NUMBER DocumentRecord::GetInstructionPage(DWORD dwPage)
{
	TRY
	{
		LockInstructionPageArray();
		return m_pInstructionPageArray->GetRecord(dwPage);
	}
	END_TRY
	return 0;
}

DWORD DocumentRecord::IndexOfInstructionPage(DB_RECORD_NUMBER lPage)
{
	TRY
	{
		LockInstructionPageArray();
		return m_pInstructionPageArray->FindRecord(lPage);
	}
	END_TRY
	return (DWORD)-1;
}

//
// Load a Instruction page by record number.
//

ObjectList* DocumentRecord::LoadInstructionPageRecord(DB_RECORD_NUMBER lPage)
{
	// Just return the current page if passed -1.
	if (lPage == -1)
	{
		if (m_pCurrentInstructionPage == NULL)
		{
			// Not a good time to call.
			return NULL;
		}
		// Return the list.
		return m_pCurrentInstructionPage->objects();
	}

	// Unload any current page.
	if (m_pCurrentInstructionPage == NULL
			|| m_pCurrentInstructionPage->Id() != lPage)
	{
		UnloadInstructionPage();

		// Get the record.
		if ((m_pCurrentInstructionPage = (PagePtr)database->get_record(lPage,
							NULL,
							RECORD_TYPE_Page)) == NULL)
		{
			return NULL;
		}
	}
	ASSERT(m_pCurrentInstructionPage != NULL);

	// Pass back a pointer to the object list.
	return m_pCurrentInstructionPage->objects();
}

//
// Load a Instruction page by index.
//

ObjectList* DocumentRecord::LoadInstructionPage(DWORD dwPage)
{
	if (dwPage < NumberOfInstructionPages())
	{
		return LoadInstructionPageRecord(GetInstructionPage(dwPage));
	}
	return NULL;
}

//
// Unload the current Instruction page.
//

void DocumentRecord::UnloadInstructionPage(void)
{
	if (m_pCurrentInstructionPage != NULL)
	{
		m_pCurrentInstructionPage->release();
		m_pCurrentInstructionPage = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// Graphic array routines.

/*
// Add a graphic to the graphic array.
*/

VOID DocumentRecord::add_graphic(DB_RECORD_NUMBER g_record)
{
	TRY
	{
		LockGraphicArray();
		m_pGraphicArray->AddRecord(g_record);
	}
	END_TRY
}

/*
// Remove a graphic from the graphic array.
*/

VOID DocumentRecord::remove_graphic(DB_RECORD_NUMBER g_record)
{
	TRY
	{
		LockGraphicArray();
		m_pGraphicArray->RemoveRecord(g_record);
	}
	END_TRY
}

/*
// Find a graphic record by name.
*/

DB_RECORD_NUMBER DocumentRecord::find_graphic(LPCSTR name)
{              
	if (name == NULL)
	{
		return 0;
	}
 
	TRY
	{
		LockGraphicArray();
		for (DWORD i = m_pGraphicArray->ElementCount(); i-- > 0; )
		{
			GraphicPtr graphic;
			DB_RECORD_NUMBER g_record = m_pGraphicArray->GetRecord(i);

			if ((graphic = (GraphicPtr)database->get_record(g_record, NULL, RECORD_TYPE_Graphic)) != NULL)
			{
				SHORT ret = strcmp(graphic->m_csFileName, name);

				graphic->release();

				if (ret == 0)
				{
					return g_record;
				}
			}
		}
	}
	END_TRY
	return 0;
}

ERRORCODE DocumentRecord::initialize_graphic_records(BOOL fInitAll)
{
	TRY
	{
		LockGraphicArray();

	/* Run through all the graphics in the array. */

		for (DWORD i = m_pGraphicArray->ElementCount(); i-- > 0; )
		{
			GraphicPtr graphic;
			DB_RECORD_NUMBER g_record = m_pGraphicArray->GetRecord(i);

			if ((graphic = (GraphicPtr)database->get_record(g_record, NULL, RECORD_TYPE_Graphic)) != NULL)
			{
				if (graphic->record.storage == GRAPHIC_STORAGE_MISSING)
				{
				/* Try again on load. */
					graphic->record.storage = GRAPHIC_STORAGE_FILE;
					graphic->modified();
				}

				if (fInitAll)
				{
				/* Do the init. */
					graphic->reinit();
				}

				graphic->release();
			}
		}
	}
	CATCH(CErrorcodeException, e)
	{
		return e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
	}
	END_CATCH_ALL
	return ERRORCODE_None;
}

DWORD DocumentRecord::NumberOfGraphics(void)
{
	TRY
	{
		LockGraphicArray();
		return m_pGraphicArray->ElementCount();
	}
	END_TRY
	return 0;
}

DB_RECORD_NUMBER DocumentRecord::GetGraphic(DWORD dwGraphic)
{
	TRY
	{
		LockGraphicArray();
		return m_pGraphicArray->GetRecord(dwGraphic);
	}
	END_TRY
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Font array routines.

/*
// Add a font data record number to the font data array.
*/

void DocumentRecord::add_font_data(DB_RECORD_NUMBER f_record)
{
	TRY
	{
		LockFontArray();
		m_pFontArray->AddRecord(f_record);
	}
	END_TRY
}

/*
// Remove a font data record number from the font data array .
*/

void DocumentRecord::remove_font_data(DB_RECORD_NUMBER f_record)
{
	TRY
	{
		LockFontArray();
		m_pFontArray->RemoveRecord(f_record);
	}
	END_TRY
}

/*
// Find a font data record by name.
*/

DB_RECORD_NUMBER DocumentRecord::find_font_data(LPSTR name)
{              
	if (name == NULL)
	{
		return 0;
	}

	TRY
	{
		LockFontArray();

		for (DWORD i = m_pFontArray->ElementCount(); i-- > 0; )
		{
			FontDataPtr fontdata;
			DB_RECORD_NUMBER f_record = m_pFontArray->GetRecord(i);

			if ((fontdata = (FontDataPtr)database->get_record(f_record, NULL, RECORD_TYPE_FontData)) != NULL)
			{
				SHORT ret = strcmp(fontdata->get_face_name(), name);

				fontdata->release();

				if (ret == 0)
				{
					return f_record;
				}
			}
		}
	}
	END_TRY
	return 0;
}

/*
// Initialize all the font records for this document.
*/

ERRORCODE DocumentRecord::initialize_font_records(BOOL delete_temps)
{
/* Run through all the fonts in the array. */

	TRY
	{
		LockFontArray();
		for (DWORD i = m_pFontArray->ElementCount(); i-- > 0; )
		{
			FontDataPtr fontdata;
			DB_RECORD_NUMBER f_record = m_pFontArray->GetRecord(i);

			if ((fontdata = (FontDataPtr)database->get_record(f_record, NULL, RECORD_TYPE_FontData)) != NULL)
			{
			/* We don't have a face index yet. */

				fontdata->set_face_index(-1);

			/* If we need to delete temporary styles, do it now. */

				if (delete_temps)
				{
				/* Zero the temporary count. */
					fontdata->reset_tmp_count();

				/* See if we have any references left. */

					ULONG ref_count = fontdata->get_reference_count();

					fontdata->release(TRUE);

					if (ref_count == 0)
					{
						remove_font_data(f_record);
						database->delete_record(f_record, RECORD_TYPE_FontData);
					}
				}
				else
				{
				/* Just release it. */
					fontdata->release(TRUE);
				}
			}
		}
	}
	CATCH(CErrorcodeException, e)
	{
		return e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
	}
	END_CATCH_ALL
	return ERRORCODE_None;
}

DWORD DocumentRecord::NumberOfFonts(void)
{
	TRY
	{
		LockFontArray();
		return m_pFontArray->ElementCount();
	}
	END_TRY
	return 0;
}

DB_RECORD_NUMBER DocumentRecord::GetFont(DWORD dwFont)
{
	TRY
	{
		LockFontArray();
		return m_pFontArray->GetRecord(dwFont);
	}
	END_TRY
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Layout array routines.

/*
// Add a layout to the layout array.
*/

void DocumentRecord::add_layout(DB_RECORD_NUMBER l_record)
{
	TRY
	{
		LockLayoutArray();
		m_pLayoutArray->AddRecord(l_record);
	}
	END_TRY
}

/*
// Remove a layout from the layout array.
*/

void DocumentRecord::remove_layout(DB_RECORD_NUMBER l_record)
{
	TRY
	{
		LockLayoutArray();
		m_pLayoutArray->RemoveRecord(l_record);
	}
	END_TRY
}

/*
// Find a layout record by name.
*/

DB_RECORD_NUMBER DocumentRecord::find_layout(LPSTR name, SHORT type)
{              
	if (name == NULL)
	{
		return 0;
	}

	TRY
	{
		LockLayoutArray();
		for (DWORD i = m_pLayoutArray->ElementCount(); i-- > 0; )
		{
			ObjectLayoutPtr layout;
			DB_RECORD_NUMBER l_record = m_pLayoutArray->GetRecord(i);

			if ((layout = (ObjectLayoutPtr)database->get_record(l_record, NULL, RECORD_TYPE_ObjectLayout)) != NULL)
			{
				BOOL ret = layout->get_type() == type && strcmp(layout->get_name(), name) == 0;

				layout->release();

				if (ret)
				{
					return l_record;
				}
			}
		}
	}
	END_TRY
	return 0;
}

DWORD DocumentRecord::NumberOfLayouts(void)
{
	TRY
	{
		LockLayoutArray();
		return m_pLayoutArray->ElementCount();
	}
	END_TRY
	return 0;
}

DB_RECORD_NUMBER DocumentRecord::GetLayout(DWORD dwLayout)
{
	TRY
	{
		LockLayoutArray();
		return m_pLayoutArray->GetRecord(dwLayout);
	}
	END_TRY
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Warp field array routines.

/*
// Add a warp field to the warp field array.
*/

void DocumentRecord::add_warp_field(DB_RECORD_NUMBER l_record)
{
	TRY
	{
		LockWarpFieldArray();
		m_pWarpFieldArray->AddRecord(l_record);
	}
	END_TRY
}

/*
// Remove a warp field from the warp field array.
*/

void DocumentRecord::remove_warp_field(DB_RECORD_NUMBER l_record)
{
	TRY
	{
		LockWarpFieldArray();
		m_pWarpFieldArray->RemoveRecord(l_record);
	}
	END_TRY
}

/*
// Find a warp field record by name.
*/

DB_RECORD_NUMBER DocumentRecord::find_warp_field(LPCSTR name)
{              
	if (name == NULL)
	{
		return 0;
	}

	TRY
	{
		LockWarpFieldArray();
		for (DWORD i = m_pWarpFieldArray->ElementCount(); i-- > 0; )
		{
			WarpFieldRecord* pRecord;
			DB_RECORD_NUMBER l_record = m_pWarpFieldArray->GetRecord(i);

			if ((pRecord = (WarpFieldRecord*)database->get_record(l_record, NULL, RECORD_TYPE_WarpField)) != NULL)
			{
				BOOL fFound = (pRecord->Name() == name);

				pRecord->release();

				if (fFound)
				{
					return l_record;
				}
			}
		}
	}
	END_TRY
	return 0;
}

DWORD DocumentRecord::NumberOfWarpFields(void)
{
	TRY
	{
		LockWarpFieldArray();
		return m_pWarpFieldArray->ElementCount();
	}
	END_TRY
	return 0;
}

DB_RECORD_NUMBER DocumentRecord::GetWarpField(DWORD dwWarpField)
{
	TRY
	{
		LockWarpFieldArray();
		return m_pWarpFieldArray->GetRecord(dwWarpField);
	}
	END_TRY
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Outline path array routines.

/*
// Add an outline path to the array.
*/

void DocumentRecord::add_outline_path(DB_RECORD_NUMBER l_record)
{
	TRY
	{
		LockOutlinePathArray();
		m_pOutlinePathArray->AddRecord(l_record);
	}
	END_TRY
}

/*
// Remove an outline path from the array.
*/

void DocumentRecord::remove_outline_path(DB_RECORD_NUMBER l_record)
{
	TRY
	{
		LockOutlinePathArray();
		m_pOutlinePathArray->RemoveRecord(l_record);
	}
	END_TRY
}

/*
// Find an outline path record by name.
*/

DB_RECORD_NUMBER DocumentRecord::find_outline_path(LPCSTR name)
{              
	if (name == NULL)
	{
		return 0;
	}

	TRY
	{
		LockOutlinePathArray();
		for (DWORD i = m_pOutlinePathArray->ElementCount(); i-- > 0; )
		{
			OutlinePathRecord* pRecord;
			DB_RECORD_NUMBER l_record = m_pOutlinePathArray->GetRecord(i);

			if ((pRecord = (OutlinePathRecord*)database->get_record(l_record, NULL, RECORD_TYPE_OutlinePath)) != NULL)
			{
				BOOL fFound = (pRecord->Name() == name);

				pRecord->release();

				if (fFound)
				{
					return l_record;
				}
			}
		}
	}
	END_TRY
	return 0;
}

DWORD DocumentRecord::NumberOfOutlinePaths(void)
{
	TRY
	{
		LockOutlinePathArray();
		return m_pOutlinePathArray->ElementCount();
	}
	END_TRY
	return 0;
}

DB_RECORD_NUMBER DocumentRecord::GetOutlinePath(DWORD dwOutlinePath)
{
	TRY
	{
		LockOutlinePathArray();
		return m_pOutlinePathArray->GetRecord(dwOutlinePath);
	}
	END_TRY
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Text style array routines.

/*
// Add a text style to the array.
*/

void DocumentRecord::AddTextStyle(DB_RECORD_NUMBER lRecord)
{
	TRY
	{
		LockTextStyleArray();
		m_pTextStyleArray->AddRecord(lRecord);
	}
	END_TRY
}

/*
// Remove a text style from the array.
*/

void DocumentRecord::RemoveTextStyle(DB_RECORD_NUMBER lRecord)
{
	TRY
	{
		LockTextStyleArray();
		m_pTextStyleArray->RemoveRecord(lRecord);
	}
	END_TRY
}

/*
// Find a text style record by name.
*/

DB_RECORD_NUMBER DocumentRecord::FindTextStyle(LPCSTR name, int nType)
{              
	if (name == NULL)
	{
		return 0;
	}

	TRY
	{
		LockTextStyleArray();

		for (DWORD i = m_pTextStyleArray->ElementCount(); i-- > 0; )
		{
			CTextStyleRecord* pRecord;
			DB_RECORD_NUMBER lRecord = m_pTextStyleArray->GetRecord(i);

			if ((pRecord = (CTextStyleRecord*)database->get_record(lRecord, NULL, RECORD_TYPE_TextStyle)) != NULL)
			{
				BOOL fFound = (stricmp(pRecord->Name(), name) == 0) && ((int)pRecord->Type() == nType);

				pRecord->release();

				if (fFound)
				{
					return lRecord;
				}
			}
		}
	}
	END_TRY
	return 0;
}

DWORD DocumentRecord::NumberOfTextStyles(void)
{
	TRY
	{
		LockTextStyleArray();
		return m_pTextStyleArray->ElementCount();
	}
	END_TRY
	return 0;
}

DB_RECORD_NUMBER DocumentRecord::GetTextStyle(DWORD dwTextStyle)
{
	TRY
	{
		LockTextStyleArray();
		return m_pTextStyleArray->GetRecord(dwTextStyle);
	}
	END_TRY
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Story array routines.

/*
// Add a story to the array.
*/

void DocumentRecord::AddStory(DB_RECORD_NUMBER lRecord)
{
	TRY
	{
		LockStoryArray();
		m_pStoryArray->AddRecord(lRecord);
	}
	END_TRY
}

/*
// Remove a story from the array.
*/

void DocumentRecord::RemoveStory(DB_RECORD_NUMBER lRecord)
{
	TRY
	{
		LockStoryArray();
		m_pStoryArray->RemoveRecord(lRecord);
	}
	END_TRY
}

/*
// Find a story in the array.
*/

DWORD DocumentRecord::FindStory(DB_RECORD_NUMBER lRecord)
{
	DWORD dwIndex = (DWORD)-1;
	TRY
	{
		LockStoryArray();
		dwIndex = m_pStoryArray->FindRecord(lRecord);
	}
	END_TRY
	return dwIndex;
}

DWORD DocumentRecord::NumberOfStories(void)
{
	TRY
	{
		LockStoryArray();
		return m_pStoryArray->ElementCount();
	}
	END_TRY
	return 0;
}

DB_RECORD_NUMBER DocumentRecord::GetStory(DWORD dwStory)
{
	TRY
	{
		LockStoryArray();
		return m_pStoryArray->GetRecord(dwStory);
	}
	END_TRY
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Hyperlink array routines.
// Add a Hyperlink to the Hyperlink array.
void DocumentRecord::AddHyperlink(DB_RECORD_NUMBER h_record)
{
	TRY
	{
		LockHyperlinkArray();
		m_pHyperlinkArray->AddRecord(h_record);
	}
	END_TRY
}
// Remove a Hyperlink from the Hyperlink array.
void DocumentRecord::RemoveHyperlink(DB_RECORD_NUMBER h_record)
{
	TRY
	{
		LockHyperlinkArray();
		m_pHyperlinkArray->RemoveRecord(h_record);
	}
	END_TRY
}

DB_RECORD_NUMBER DocumentRecord::GetHyperlink(DWORD dwHyperlink)
{
	TRY
	{
		LockHyperlinkArray();
		return m_pHyperlinkArray->GetRecord(dwHyperlink);
	}
	END_TRY
	return 0;
}
// Gets just the URL string
void 
DocumentRecord::AppendFullURLString(struct HyperlinkData &hld, CString &str)
{
   CString  Temp;
   DWORD PageIdx;

   switch (hld.StringType)
   {
      case TYPE_URL:             // "Complete" URL, i.e. "http://foo.bar"
      str += hld.FilePathOrURL;
      break;   
      
      case TYPE_PageURL:         // "Incomplete" URL, ie "foo.bar"
      PageIdx = IndexOfPage(hld.PageRecordNumber);
      ASSERT((long)PageIdx >= 0);      
      CHTMLDocConverter::GetHTMLFileName(PageIdx, Temp);
      str += Temp;
      break;   
      
      case TYPE_EmailAddress:    // As "mailto:foobar@foo.bar"
      str += "mailto:";
      str += hld.FilePathOrURL;
      break;   
      
      case TYPE_FilePath:        // As c:\foo\bar.foo
      Util::SplitPath(hld.FilePathOrURL, NULL, &Temp);
      str += Temp;
      break;   
   }
}
// Gets "HREF=" + the URL string
void DocumentRecord::AppendFullURL(struct HyperlinkData &hld, CString &str)
{
   str += " HREF=\"";
   AppendFullURLString(hld, str);
   str += "\"";
}

DWORD DocumentRecord::NumberOfHyperlinks(void)
{
	TRY
	{
		LockHyperlinkArray();
		return m_pHyperlinkArray->ElementCount();
	}
	END_TRY
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// Miscellaneous routines.

/*
// Assign method.
*/

ERRORCODE DocumentRecord::assign(DatabaseRecordRef srecord)
{
/* Assign the base record first. */
	return DatabaseRecord::assign(srecord);
}

CPaperInfo* DocumentRecord::GetPaperInfo(void)
{
	ASSERT(m_pPaperInfo != NULL);
	return m_pPaperInfo;
}
