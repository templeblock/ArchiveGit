/*
// $Header: /PM8/App/DBMAP.CPP 1     3/03/99 6:04p Gbeddow $
//
// Implementation for database mappings.
//
// $Log: /PM8/App/DBMAP.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 5     10/21/98 4:58p Jayn
// Fixed path manager problems.
// 
// 4     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 3     9/14/98 12:06p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 2     11/21/97 11:01a Jayn
// Moved StorageFileOnCFile from dbmap.* to file.*
// 
//    Rev 1.0   14 Aug 1997 15:20:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:02   Fred
// Initial revision.
// 
//    Rev 1.2   03 Apr 1997 09:30:52   Fred
// More text cleanup
// 
//    Rev 1.1   18 Jul 1996 17:59:22   Fred
// Moved class from file.cpp
// 
//    Rev 1.0   14 Mar 1996 13:36:22   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:58   FRED
// Initial revision.
// 
//    Rev 2.14   20 Dec 1995 09:56:40   JAY
// 32-bit fixes
// 
//    Rev 2.13   06 Oct 1995 11:59:46   JAY
// New print code, etc.
// 
//    Rev 2.12   30 Aug 1995 14:14:40   JAY
// 
//    Rev 2.11   31 Jul 1995 10:27:18   JAY
// New projects.
// 
//    Rev 2.10   20 Jul 1995 12:51:24   JAY
// Label project. New paper info stuff. etc
// 
//    Rev 2.9   12 Jul 1995 17:45:04   JAY
// New project sub-types (esp. for cards).
// 
//    Rev 2.8   05 Jul 1995 11:16:56   JAY
// Rearrange includes to reduce symbol count.
// 
//    Rev 2.7   30 Jun 1995 16:35:20   JAY
// New PaperInfo and EditDesktop
// 
//    Rev 2.6   09 Jun 1995 10:49:40   JAY
// Fixed bug with file name.
// 
//    Rev 2.5   07 Jun 1995 15:39:54   JAY
//  
// 
//    Rev 2.4   01 May 1995 15:27:56   JAY
// Restructured the macro stuff
// 
//    Rev 2.3   25 Apr 1995 09:00:30   JAY
// DatabaseMapping now can create mandatory records.
// 
//    Rev 2.2   17 Feb 1995 16:48:06   JAY
// Text editing undo and redo.
// 
//    Rev 2.1   08 Feb 1995 13:36:08   JAY
// Reverted. New series.
// 
//    Rev 1.4   30 Nov 1994 17:02:42   JAY
//  
// 
//    Rev 1.3   04 Aug 1994 08:31:00   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   19 Jan 1994 17:09:26   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 17:15:20   JAY
// Cut and paste!
// 
//    Rev 1.0   05 Jan 1994 08:11:28   JAY
// Restructuring, cut/paste, images, etc.
*/

#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"
#include "docrec.h"
#include "prevrec.h"
#include "macrorec.h"

#include "signdoc.h"
#include "carddoc.h"
#include "caldoc.h"
#include "banndoc.h"
#include "util.h"
#include "utils.h"

#include "dbmap.h"
#include "paper.h"

#include "pmgfont.h"
#include "macrostd.h"
#include "file.h"

#ifdef WIN32
#include <afxpriv.h>
#endif

extern char far preamble[];

DatabaseMapping::DatabaseMapping()
{
	m_pStorage = NULL;
	device = NULL;
	font_server = NULL;
	database = NULL;
	m_pMacroServer = NULL;
}

/*
// Cleanup a database mapping.
*/

DatabaseMapping::~DatabaseMapping()
{
	FreeAll();
}

void DatabaseMapping::FreeAll(void)
{
/* Since this uses the database, if must come first. */
	if (font_server != NULL)
	{
		delete font_server;
		font_server = NULL;
	}
	if (m_pMacroServer != NULL)
	{
		delete m_pMacroServer;
		m_pMacroServer = NULL;
	}
	if (database != NULL)
	{
		delete database;
		database = NULL;
	}
	if (device != NULL)
	{
		delete device;
		device = NULL;
	}

/*
// This must be released after the device in case the device is a stream
// within the storage.
*/

	if (m_pStorage != NULL)
	{
		m_Stream.Close();
		m_pStorage->Release();
	}
}

/*
// This is the standard creator for a database mapping.
*/

ERRORCODE DatabaseMapping::map_to_file(LPCSTR name, BOOL read_only)
{
	if (name != NULL)
	{
		m_csName = name;
	}

	if (device == NULL)
	{
	/* Create the file. */

		if (read_only)
		{
			device = new ReadOnlyFile(name);
		}
		else
		{
			device= new StorageFile(name);
		}

		if (device == NULL)
		{
			return ERRORCODE_Memory;
		}
	}

/* Create the font server. */

	if (font_server == NULL)
	{
		if ((font_server = new PMGFontServer) == NULL)
		{
			return ERRORCODE_Memory;
		}
	}

/* Create the database. */

	if (database == NULL)
	{
		if ((database = new PMGDatabase(device, font_server)) == NULL)
		{
			return ERRORCODE_Memory;
		}
	}

	((class PMGFontServer*)font_server)->set_database(database);

/* Create the macro server. */

	if (m_pMacroServer == NULL)
	{
		if ((m_pMacroServer = new CStdMacroServer) == NULL)
		{
			return ERRORCODE_Memory;
		}
	}

	database->SetMacroServer(m_pMacroServer);

	return ERRORCODE_None;
}

/*
// This is the creator for a database mapping when there is an existing
// file that may be a storage-based document.
*/

ERRORCODE DatabaseMapping::map_to_document(LPCSTR pName, BOOL fReadOnly)
{
	ASSERT(device == NULL);

	CString csFullName = GetGlobalPathManager()->LocatePath(pName);

#ifdef WIN32
	USES_CONVERSION;
	HRESULT hr = StgIsStorageFile(T2COLE(csFullName));
#else
	HRESULT hr = StgIsStorageFile(csFullName);
#endif

	if (GetScode(hr) == S_OK)
	{
	/*
	// Open the storage file.
	*/
		DWORD dwMode = fReadOnly
								? (STGM_READ|STGM_DIRECT|STGM_SHARE_DENY_WRITE)
								: (STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE);

#ifdef WIN32
		HRESULT hr = ::StgOpenStorage(T2COLE(csFullName),
#else
		HRESULT hr = ::StgOpenStorage(csFullName,
#endif
												NULL,
												dwMode,
												0, 0, &m_pStorage);
		if (hr != NOERROR)
		{
			return ERRORCODE_Open;			// Approximate error.
		}
	/*
	// We have the storage open.
	// Now open the document stream.
	*/
	/*
	// Open the stream on our storage.
	// Note that we always use STGM_SHARE_EXCLUSIVE. This is required by the
	// current OLE implementation (since streams can not be shared).
	*/

		dwMode = fReadOnly
						? (STGM_READ|STGM_DIRECT|STGM_SHARE_EXCLUSIVE)
						: (STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE);

		if (!m_Stream.OpenStream(m_pStorage, CPmwDoc::DocumentStreamName, dwMode))
		{
			m_pStorage->Release();
			m_pStorage = NULL;
			return ERRORCODE_Open;			// Approximate error, again.
		}

	/* Create our device to manage this stream. */

		device = new StorageFileOnCFile(&m_Stream);

	/*
	// Now call map_to_file().
	// Since 'device' is now set, it won't be set again; but the rest of the
	// members will be filled out as needed.
	*/
	}

	return map_to_file(pName, fReadOnly);
}

ERRORCODE DatabaseMapping::initialize(char *preamble,
							SHORT signature,
							SHORT file_type,
							SHORT file_flags)
{
	ST_DEV_POSITION database_start;
	ERRORCODE error;

	if ((error = device->initialize()) != ERRORCODE_None
		|| (error = device->seek(0L, ST_DEV_SEEK_SET)) != ERRORCODE_None
		|| (error = device->write(preamble, strlen(preamble)+1)) != ERRORCODE_None
		|| (error = device->write(&signature, sizeof(signature))) != ERRORCODE_None
		|| (error = device->write(&file_type, sizeof(file_type))) != ERRORCODE_None
		|| (error = device->write(&file_flags, sizeof(file_flags))) != ERRORCODE_None

/* Remember the start of the database portion. */

		|| (error = device->tell(&database_start)) != ERRORCODE_None

/* Initialize the database. */

		|| (error = database->initialize(database_start)) != ERRORCODE_None)
	{
		return error;
	}
	return error;
}

/*
// Parse the document preamble to determine type and start of document.
*/

PUBLIC ERRORCODE
parse_document_preamble(StorageDevice* device, SHORT *signature,
									SHORT *file_type, SHORT *file_flags)
{
/*
// Find the end of the preamble.
// This is at most 256 characters.
*/
	BYTE preamble_memory[256];

	LPBYTE p;
	ERRORCODE error = ERRORCODE_IllegalType;
	ST_DEV_IO_SIZE size_read;

/* Read the preamble from the start of the file. */

	device->seek(0, ST_DEV_SEEK_SET);
	if (device->read(preamble_memory, 256, &size_read) != ERRORCODE_None
			|| size_read == 0
			|| (p = (LPBYTE)memchr(preamble_memory, 0x1a, size_read)) == NULL)
	{
	/* Bad file. */
	}
	else
	{
		p++;			/* Skip the 0x1a */
		p++;			/* Skip the 0 */

	/* Get the signature for the file to determine validity and type. */

		*signature = *(LPSHORT)p;
		p += sizeof(SHORT);

		switch (*signature)
		{
			case SIGNATURE:
			{
				SHORT desired_type = FILE_TYPE;

				*file_type = *(LPSHORT)p;
#if 0
				if (CPmwDoc::m_fFixTemplates)
				{
				/* We assume everything is a template. */
					if (*file_type != TEMPLATE_TYPE)
					{
					/* Change this from a normal document to a template. */
						desired_type = TEMPLATE_TYPE;
						device->seek(p - preamble_memory, ST_DEV_SEEK_SET);
						device->write(&desired_type, sizeof(desired_type));
					}
				}
				else
#endif
				{
				/* Not special "fix template" mode. */
					if (*file_type == TEMPLATE_TYPE && !CPmwDoc::allow_edit_templates)
					{
					/* Change this from a template to a normal document. */
						device->seek(p - preamble_memory, ST_DEV_SEEK_SET);
						device->write(&desired_type, sizeof(desired_type));
					}
				}

				p += sizeof(SHORT);

				*file_flags = *(LPSHORT)p;
				p += sizeof(SHORT);

				if (*file_type == FILE_TYPE || *file_type == TEMPLATE_TYPE)
				{
				/* We only support one type of file as of now. */
					error = ERRORCODE_None;
				}
//				od("Standard document.\r\n");

				break;
			}
			case OLD_SIGNATURE:
			{
//				od("Old PMG document.\r\n");

			/* This is an "old" file type. */
				*file_type = *(LPSHORT)p;
				p += sizeof(SHORT);
				*file_flags = 0;

				error = ERRORCODE_None;
				break;
			}
			default:
			{
				break;
			}
		}
	}

	if (error == ERRORCODE_None)
	{
	/* Seek to the start of data. */
		error = device->seek(p - preamble_memory, ST_DEV_SEEK_SET);
	}

	return error;
}

/*
// Open an existing database.
*/

ERRORCODE DatabaseMapping::open(SHORT *signature, SHORT *file_type, SHORT *file_flags, BOOL fConvertIfOld /*=FALSE*/)
{
	ERRORCODE error;

/* Parse the preamble of this document. */

	if ((error = parse_document_preamble(device, signature,
		 				file_type, file_flags)) != ERRORCODE_None)
	{
		return error;
	}

/* Verify we have a good file signature. */

	if (*signature != SIGNATURE)
	{
	/*
	// If we are requested to convert this, do it now.
	*/
		if (!fConvertIfOld || *signature != OLD_SIGNATURE)
		{
		/* Nothing to do. */
			return ERRORCODE_IllegalType;
		}

	////////// Old document conversion time. //////////
	/*
	// We need to determine the file type.
	// Normally this is handled automatically by the document template
	// mechanism. Here we need to do it ourselves; we use the extension.
	// Warning: Brute-force code ahead.
	*/

		CString csExtension;

		Util::SplitPath(m_csName, NULL, NULL, NULL, &csExtension);

		ProjectInfo Info;

		csExtension.MakeUpper();

		if (csExtension == ".CAR")
		{
		/* Old cards are assumed to be side fold. */
			Info = CCardDoc::m_SideFoldInfo;
		}
		else if (csExtension == ".CAL")
		{
			Info = CCalendarDoc::StandardInfo;
		}
		else if (csExtension == ".BAN")
		{
			Info = CBannerDoc::StandardInfo;
		}
		else
		{
		/* Generic type of document (or poster). */
			Info = CPosterDoc::StandardInfo;
		}

	/*
	// Do the conversion.
	*/
		/*
		// It is impossible for an old document to be coming from a storage file.
		*/
		ASSERT(m_pStorage == NULL);

		/*
		// Save the old device and make a new one.
		*/

		StorageDevice* pOldDevice = device;

		device = new TmpFile((char *)CPmwDoc::tempfile_template);
		if ((error = database->force_new_device(device)) == ERRORCODE_None)
		{
			if ((error = initialize(preamble,
											SIGNATURE,
											FILE_TYPE,
											0)) == ERRORCODE_None)
			{
			/* Get the app for pertinent app-stored info. */
				CPmwApp *pApp = (CPmwApp *)AfxGetApp();

				DOC_CREATE_STRUCT dcs;

				dcs.project_type = Info.project_type;
				dcs.m_nProjectSubtype = Info.project_sub_type;
				dcs.orientation = PORTRAIT;		/* Since we don't know yet... */

				CPaperInfo* pPaperInfo = pApp->GetPaperInfo(PORTRAIT);

				dcs.paper_width = 0;
				dcs.paper_height = 0;
				dcs.m_pPaperInfo = pPaperInfo;
				dcs.dimensions = pPaperInfo->GetProjectDims();

				if ((error = CreateMandatoryRecords(&dcs)) == ERRORCODE_None)
				{
				/* Create a page for our document. */

					DocumentPtr pDocument;
					if ((pDocument = (DocumentPtr)database->get_record(PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document)) == NULL)
					{
						return error;
					}

					DB_RECORD_NUMBER Page;

					if ((error = pDocument->new_page(&Page)) != ERRORCODE_None)
					{
						pDocument->release();
						return error;
					}

					pDocument->load_page(Page);

					PBOX Margins = pPaperInfo->GetPageMargins();
					RECT rMargins;
					rMargins.left = (int)Margins.x0;
					rMargins.top = (int)Margins.y0;
					rMargins.right = (int)Margins.x1;
					rMargins.bottom = (int)Margins.y1;

					error = database->convert_old_document(*signature, pOldDevice, &Info, &rMargins);

					pDocument->release();
				}
			}
		}
		delete pOldDevice;
		return error;
	}

/* Read the database state. */

	return database->read_state();
}

/*
// Flush the database in a mapping.
*/

ERRORCODE DatabaseMapping::flush_database()
{
	ERRORCODE error;

/*
// Flush all database information that needs to be written.
*/

	if ((error = database->flush()) != ERRORCODE_None)
	{
		printf("Got error %d flushing the database.\n", error);
		return error;
	}

/*
// Write the database state information.
*/

	error = database->write_state();
	return error;
}

ERRORCODE DatabaseMapping::CreateMandatoryRecords(DOC_CREATE_STRUCT* pDCS /*=NULL*/)
{
/* Create the project preview. */

	ProjectPreviewPtr pPreview = database->create_project_preview();
	if (pPreview == NULL)
	{
		return database->last_creation_error();
	}
	pPreview->release();

/* Create a document. */

	DocumentPtr pDocument = database->create_document(pDCS);
	if (pDocument == NULL)
	{
		return database->last_creation_error();
	}

/* Create the macro list. */

	MacroListRecord* pMacroList = (MacroListRecord*)database->new_record(RECORD_TYPE_MacroList, NULL);
	if (pMacroList == NULL)
	{
		pDocument->release();
		return database->last_creation_error();
	}
	pDocument->set_macro_list_record(pMacroList->Id());

	if (m_pMacroServer == NULL)
	{
		pMacroList->release();
	}
	else
	{
		m_pMacroServer->SetMacroList(pMacroList);
	}

	pDocument->release();

	return ERRORCODE_None;
}

