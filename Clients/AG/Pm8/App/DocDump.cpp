/*
// $Workfile: DocDump.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/DocDump.cpp $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 11    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 10    9/14/98 12:06p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 9     8/21/98 6:49p Hforman
// checking extension and only setting project type/subtype if .cft
// 
// 8     8/21/98 4:27p Hforman
// Add warnings if Project Type or Subtype are bad
// 
// 7     8/20/98 6:19p Hforman
// looking for PROJECT TYPE & PROJECT SUBTYPE columns and writing info
// into document if found.
// 
// 6     7/22/98 1:07p Jayn
// Fixed output string. Added EXTENSIVE graphic fixing (ifdef'd)
// 
// 5     6/05/98 2:31p Fredf
// Allows graphics to be optionally embedded.
// 
// 4     5/19/98 11:34a Dennis
// Moved some methods from CDocDumpDialog into new class "CDocDump" 
// 
// 3     1/13/98 11:16a Jayn
// Compact, freeing old frames and paragraphs, etc.
// 
// 2     12/23/97 10:27a Jayn
// 
// 1     12/23/97 9:57a Jayn
// Document dump and utility classes.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwtempl.h"
#include "mainfrm.h"

#include "docdump.h"

#include "compfn.h"
#include "util.h"

#include "docrec.h"
#include "prevrec.h"
#include "fontrec.h"
#include "pagerec.h"
#include "efilerec.h"
#include "layrec.h"
#include "arrayrec.h"

#include "projgrph.h"
#include "progbar.h"    // for CPMWProgressBar
#include "collbld.h"    // for StringIsEmpty

//#define EXTENSIVE_GRAPHIC_FIX

/////////////////////////////////////////////////////////////////////////////
// General entry point (stub).

void DumpDocument(void)
{
	// Get the document to use.
	CPmwDoc* pDoc = (CPmwDoc*)(((CMainFrame*)AfxGetMainWnd())->GetActiveFrame()->GetActiveDocument());

	CDocDumpDialog Dialog(pDoc);
	Dialog.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// Dump Storage classes

class DumpStorageIterator
{
	friend class DumpStorageManager;
private:
	ST_DEV_POSITION	m_next_position;
public:
	ST_DEV_POSITION	m_position;
	ST_MAN_HDR_BLOCK	m_hdr;

	DumpStorageIterator()
		{ m_next_position = POSITION_UNKNOWN; }
};

/*
// A Storage Manager derivative which allows information access.
// This class is not designed to ever be instantiated, but should instead
// be used as a cast for a pointer from a StorageManager.
// IT SHOULD DEFINE NO DATA OR VIRTUAL FUNCTIONS SINCE THESE WON'T WORK.
*/

class DumpStorageManager : public StorageManager
{
private:
/* This is designed to never be called. */
	DumpStorageManager() : StorageManager(NULL)
		{}
public:

/*
// Get a storage block.
*/

	BOOL GetStorageBlock(DumpStorageIterator &dsit);

/*
// reset_storage()
//
// This is a very dangerous call! It will completely reset the storage
// arena. The only case it is currently called is when all storage objects
// are held entirely in memory and can thus be reallocated.
// It makes no checks! Use with caution!
*/

	void reset_storage(void);

};

/*
// Get a storage block.
*/

BOOL DumpStorageManager::GetStorageBlock(DumpStorageIterator &dsit)
{
/* Init if necessary. */

	if (dsit.m_next_position == POSITION_UNKNOWN)
	{
		dsit.m_next_position = state.storage_start;
	}

	dsit.m_position = dsit.m_next_position;

/* See if we're done. */

	if (dsit.m_position < state.storage_end)
	{
	/*
	// Read the next header.
	*/

		if (ReadHeaderBlock(&dsit.m_hdr, dsit.m_position) == ERRORCODE_None)
		{
			dsit.m_next_position += HdrBlockSize() + dsit.m_hdr.size;
			return TRUE;
		}
	}

	return FALSE;
}

/*
// reset_storage()
//
// This is a very dangerous call! It will completely reset the storage
// arena. The only case it is currently called is when all storage objects
// are held entirely in memory and can thus be reallocated.
// It makes no checks! Use with caution!
*/

void DumpStorageManager::reset_storage(void)
{
	state.storage_end = state.storage_start;
	state.free_start = POSITION_UNALLOCATED;
 	state.free_biggest = ST_MAN_MAX_SIZE;
}

/////////////////////////////////////////////////////////////////////////////
// CDocDumpDialog dialog

CDocDumpDialog::CDocDumpDialog(CPmwDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CDocDumpDialog::IDD, pParent), m_docDump(pDoc, GetGlobalPathManager())
{
	m_pDoc = pDoc;
	//{{AFX_DATA_INIT(CDocDumpDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDocDumpDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDocDumpDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDocDumpDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CDocDumpDialog)
	ON_BN_CLICKED(IDC_FREE_EMBEDDED, OnFreeEmbedded)
	ON_BN_CLICKED(IDC_COMPACT, OnCompact)
	ON_BN_CLICKED(IDC_BATCH_FIX, OnBatchFix)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocDumpDialog message handlers

BOOL CDocDumpDialog::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();

	if (m_pDoc == NULL)
	{
		// Do not allow these.
		EnableDlgItem(IDC_COMPACT, FALSE);
		EnableDlgItem(IDC_FREE_EMBEDDED, FALSE);
	}
	else
	{
		UpdateRecordList();
		UpdateStorageList();
	}

	return TRUE;
}

void CDocDumpDialog::UpdateRecordList(void)
{
	ASSERT(m_pDoc != NULL);

	static const char * const type_name[] =
	{
		"(Illegal)",
		"Preview",
		"Document",
		"Font Data",
		"Page",
		"Graphic",
		"Old Text Box",
		"Paragraph",
		"Object Layout",
		"Calendar Info",
		"UNUSED",
		"Embedded File",
		"Macro List",
		"Name List",
		"Warp Field",
		"Outline Path",
		"Array",
		"ArrayBlock",
		"RecordArray",
		"RecordArrayBlock",
		"CharacterArray",
		"CharacterArrayBlock",
		"WordArray",
		"WordArrayBlock",
		"StyleArray",
		"StyleArrayBlock",
		"ParagraphArray",
		"ParagraphArrayBlock",
		"LineArray",
		"LineArrayBlock",
		"Text",
		"Frame",
		"TextStyle"
	};

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_RECORDS);
	if (pListBox == NULL)
	{
		// Not much to do here.
		return;
	}

	int rgTabStops[4] = {20, 70, 140, 280};
	pListBox->SetTabStops(4, rgTabStops);

/*
// Grab the document's database.
*/

	PMGDatabasePtr database = m_pDoc->get_database();

/*
// Stuff all records into the listbox.
*/

	pListBox->SetRedraw(FALSE);
	pListBox->ResetContent();

	CString csBuffer;
	CString csRest;

	DB_RECORD_NUMBER record;

	for (record = 1; ; record++)
	{
		DatabaseRecordPtr pRecord;
		ERRORCODE error;

		if ((pRecord = database->get_record(record, &error)) == NULL)
		{
			if (error == ERRORCODE_DoesNotExist)
			{
				break;
			}
			else if (error == ERRORCODE_NotAllocated)
			{
//				od("Record %ld: (Free)\r\n", record);
			}
			else
			{
				csBuffer.Format("Got error %d for record %ld", error, record);
				pListBox->AddString(csBuffer);
				break;
			}
		}
		else
		{
			int type = pRecord->type();
			BOOL fAddedType = FALSE;

			if (type > RECORD_TYPE_TextStyle || type == 0)
			{
				csBuffer.Format("%ld:\tIllegal type %d", pRecord->Id(), type);
			}
			else
			{
				csBuffer.Format("%ld:\t%s", pRecord->Id(), (LPCSTR)type_name[pRecord->type()]);

				switch (type)
				{
					case RECORD_TYPE_ProjectPreview:
					{
						ProjectPreviewPtr preview = (ProjectPreviewPtr)pRecord;

						csRest.Format("\t'%s'", preview->get_description());
						csBuffer += csRest;
						break;
					}
					case RECORD_TYPE_Document:
					{
						DocumentPtr doc = (DocumentPtr)pRecord;
						break;
					}
					case RECORD_TYPE_FontData:
					{
						FontDataPtr fontdata = (FontDataPtr)pRecord;

						csRest.Format("\t'%s'\t\tref:%ld",
										  fontdata->get_face_name(),
										  fontdata->get_reference_count());
						csBuffer += csRest;
						break;
					}
					case RECORD_TYPE_Page:
					{
						PagePtr page = (PagePtr)pRecord;

						csRest.Format("\t%d objects", page->objects()->count());
						csBuffer += csRest;
						break;
					}
					case RECORD_TYPE_Graphic:
					{
						GraphicPtr graphic = (GraphicPtr)pRecord;
						CompositeFileName cfn(graphic->m_csFileName);

						csRest.Format("\t'%s'\t\tref:%d",
										  graphic->m_csName,
										  graphic->record.reference_count);
						csBuffer += csRest;

						int nSelection = pListBox->AddString(csBuffer);
						if (nSelection != -1)
						{
							pListBox->SetItemData(nSelection, MAKELONG(type, record));
						}
						fAddedType = TRUE;

						csBuffer.Format("\t\tFile Name:\t'%s'", cfn.get_file_name());

						if (cfn.has_dynamic_name())
						{
							pListBox->AddString(csBuffer);
							csBuffer.Format("\t\tDynamic Name:\t'%s'", (LPCSTR)cfn.get_dynamic_name());
						}

						if (cfn.has_virtual_name())
						{
							pListBox->AddString(csBuffer);
							csBuffer.Format("\t\tVirtual Name:\t'%s'", (LPCSTR)cfn.get_virtual_name());
						}

						if (graphic->record.source != 0)
						{
							pListBox->AddString(csBuffer);
							csBuffer.Format("\tSource: %ld\n", graphic->record.source);
						}

						if (graphic->record.image != 0)
						{
							pListBox->AddString(csBuffer);
							csBuffer.Format("\tImage: %ld\n", graphic->record.image);
						}
						break;
					}
					case RECORD_TYPE_Frame:
					{
						FramePtr frame = (FramePtr)pRecord;
						break;
					}
					case RECORD_TYPE_Paragraph:
					{
						ParagraphPtr paragraph = (ParagraphPtr)pRecord;
						break;
					}
					case RECORD_TYPE_ObjectLayout:
					{
						ObjectLayoutPtr layout = (ObjectLayoutPtr)pRecord;

						csRest.Format("\t'%s'\t\tref:%d",
											 layout->get_name(),
											 layout->get_reference_count());
						csBuffer += csRest;
						break;
					}
					case RECORD_TYPE_CalendarInfo:
					{
						break;
					}
					case RECORD_TYPE_Array:
					{
						break;
					}
					case RECORD_TYPE_EmbeddedFile:
					{
						EmbeddedFilePtr efile = (EmbeddedFilePtr)pRecord;

						csRest.Format("\t'%s'", efile->get_file_name());
						csBuffer += csRest;
						break;
					}
					default:
					{
						break;
					}
				}
			}
			int nSelection = pListBox->AddString(csBuffer);
			if (!fAddedType && nSelection != -1)
			{
				pListBox->SetItemData(nSelection, MAKELONG(type, record));
			}
			pRecord->release();
		}
	}

	pListBox->SetRedraw(TRUE);
}

void CDocDumpDialog::UpdateStorageList(void)
{
	ASSERT(m_pDoc != NULL);

	static const char * const type_name[] =
	{
		"(Illegal)",
		"(Free)",
		"Record",
		"Table",
		"Table Locator"
	};

/*
// Grab the listbox so we can shove stuff into it.
*/

	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_STORAGE);

	if (pListBox == NULL)
	{
	/* We can't do much here! */
		return;
	}

	int rgTabStops[4] = {45, 100, 130, 200};
	pListBox->SetTabStops(4, rgTabStops);

/*
// Grab the document's database.
*/

	PMGDatabasePtr database = m_pDoc->get_database();
	DumpStorageManager* sm = (DumpStorageManager*)database->get_storage_manager();

/*
// Stuff all records into the listbox.
*/

	pListBox->SetRedraw(FALSE);
	pListBox->ResetContent();

	char buffer[1024];

	DumpStorageIterator dsit;

	while (sm->GetStorageBlock(dsit))
	{
		int type = dsit.m_hdr.type;

		if (type > RECORD_TYPE_EmbeddedFile || type == 0)
		{
			wsprintf(buffer, "%ld:\tIllegal type %d", dsit.m_position, type);
		}
		else
		{
			wsprintf(buffer, "%ld:\t%s\t%ld\t%ld", dsit.m_position,
					(LPCSTR)type_name[type],
					dsit.m_hdr.id, dsit.m_hdr.size);
		}

		int nSelection = pListBox->AddString(buffer);

		if (nSelection != -1)
		{
			pListBox->SetItemData(nSelection, dsit.m_position);
		}
	}

	pListBox->SetRedraw(TRUE);
}

void CDocDumpDialog::OnFreeEmbedded()
{
	m_docDump.DoFreeEmbedded();
   UpdateRecordList();
	UpdateStorageList();
}

void CDocDumpDialog::OnCompact() 
{
	m_docDump.DoCompact();
   UpdateRecordList();
	UpdateStorageList();

	m_docDump.DoFreeOldTextBoxes();
   UpdateRecordList();
	UpdateStorageList();
}

void CDocDumpDialog::OnBatchFix()
   {
   // Get the name of the file to load.
   CFileDialog OpenDialog(
                  TRUE,
                  "*.txt",
                  NULL,
                  OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
                  "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||",
                  AfxGetMainWnd());

		// Ask for the project spreadsheet file.
      if (OpenDialog.DoModal() == IDOK)
      {
         // Get the input file name.
         CString csInput = OpenDialog.GetPathName();
         m_docDump.BatchFix(csInput);
		}
   }

/////////////////////////////////////////////////////////////////////
// CDocDump Methods

CDocDump::CDocDump(CPmwDoc* pDoc, IPathManager* pPathManager)
{
	m_pDoc = pDoc;
	m_pPathManager = pPathManager;
}

void CDocDump::BatchFix(LPCSTR szInputFileName, BOOL bRunSilent, BOOL bFreeEmbedded /*=TRUE*/, BOOL bEmbed /*=FALSE*/)
{
	CString csInputDirectory;
   CStdioFile cfInput;
   CString csLine;
   CString csItem;
   CString csName;
   CString csValue;
   CMlsStringArray csaItems;
   CMlsStringArray csaText;
   CMlsStringArray csaNames;
   CMlsStringArray csaValues;
   CString csOutput;
   CStdioFile cfOutput;
   CString csError;
   CStdioFile cfError;
   CString csDirectory;
   CString csFilename;
   CString csProject;
	CString csGraphic;
	CString csGraphicBaseName;
   CString csMessage;
   CPmwDoc* pDocument = NULL;
   PMGDatabase* pDatabase = NULL;
   DocumentRecord* pDocumentRecord = NULL;
   CTextRecord* pText = NULL;
   CFindProjectsWithGraphicsProgressDialog* pProgressDialog = NULL;
	CPmwDoc* pDocSave = m_pDoc;
   int      nLineCount = 0;

   TRY
   {
      // Create the error file.
      csError = m_pPathManager->ExpandPath("FIXERR.TXT");
      if (!cfError.Open(csError, CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
      {
         csMessage.Format("Can't create error file %s", (LPCSTR)csError);
         AfxMessageBox(csMessage);
         AfxThrowUserException();
      }

      // Open up the text file.
		if (!cfInput.Open(szInputFileName, CFile::modeRead|CFile::shareDenyNone|CFile::typeBinary))
		{
			csMessage.Format("Can't open input file %s", szInputFileName);
			cfError.WriteString(csMessage);
			cfError.WriteString("\n");
			AfxMessageBox(csMessage);
			AfxThrowUserException();
		}

		Util::SplitPath(szInputFileName, &csInputDirectory, NULL);

      // Create a progress dialog.
      pProgressDialog = new CFindProjectsWithGraphicsProgressDialog(IDD_FIND_PROJECTS_WITH_GRAPHICS_PROGRESS_DIALOG, AfxGetMainWnd());

		// Report what files we're using.
      csMessage.Format("Error File: %s", (LPCSTR)csError);
      pProgressDialog->AddMessage(csMessage);
      csMessage.Format("Input File: %s", szInputFileName);
      pProgressDialog->AddMessage(csMessage);

      csMessage.Format("Reading header information");
      pProgressDialog->AddMessage(csMessage);

      // Get Line Count
      while (Util::ReadString(&cfInput, csLine))
         {
            // Don't count it if empty
            if(!CCollectionBuilder::StringIsEmpty(csLine))
               nLineCount++;
         }
      // Don't include header lines
      if(nLineCount > 2)
         nLineCount -= 2;
      pProgressDialog->SetProgressRange(1, nLineCount);

      cfInput.SeekToBegin();

      // Read the header and figure out the column to use for directory and file name.
      Util::ReadString(&cfInput, csLine);
      csLine.MakeUpper();

      // Parse the items.
      csaItems.SetNames(csLine, '\t');

      csMessage.Format("Locating DIRECTORY and FILE NAME columns");
      pProgressDialog->AddMessage(csMessage);

      // Try to find the "DIRECTORY" field.
      int nDirectoryIndex = csaItems.FindName("DIRECTORY");
      if (nDirectoryIndex == -1)
      {
         csMessage.Format("No DIRECTORY Field in %s", szInputFileName);
         pProgressDialog->AddMessage(csMessage);
			cfError.WriteString(csMessage);
			cfError.WriteString("\n");
         AfxThrowUserException();
      }

      // Try to find the "FILE NAME" field.
      int nFilenameIndex = csaItems.FindName("FILE NAME");
      if (nFilenameIndex == -1)
      {
         csMessage.Format("No FILE NAME Field in %s", szInputFileName);
         pProgressDialog->AddMessage(csMessage);
			cfError.WriteString(csMessage);
			cfError.WriteString("\n");
         AfxThrowUserException();
      }

		// Try to find the "PROJECT TYPE" and "PROJECT SUBTYPE" fields
		int nProjectTypeIndex = csaItems.FindName("PROJECT TYPE");
		int nProjectSubTypeIndex = csaItems.FindName("PROJECT SUBTYPE");

      // Skip the next header line.
      Util::ReadString(&cfInput, csLine);

      // Create the output file.
      int      nCharIndex;
      csOutput = szInputFileName;
      nCharIndex = csOutput.ReverseFind('.');
      ASSERT(nCharIndex >= 0);
      if(nCharIndex >= 0)
         {
            // Make Output log file .FIX extension
            csOutput.GetBufferSetLength(nCharIndex+1);
            csOutput += "FIX";
         }
//      csOutput = m_pPathManager->ExpandPath("PROJFIX.TXT");

      csMessage.Format("Output File: %s", (LPCSTR)csOutput);
      pProgressDialog->AddMessage(csMessage);

      if (!cfOutput.Open(csOutput, CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
      {
         csMessage.Format("Can't create output file %s", (LPCSTR)csOutput);
         pProgressDialog->AddMessage(csMessage);
			cfError.WriteString(csMessage);
			cfError.WriteString("\n");
         AfxThrowUserException();
      }

      // Process each line from the input file.
      while (Util::ReadString(&cfInput, csLine))
      {
         Util::YieldToWindows(); // Allow windows message processing

         if (pProgressDialog->CheckForAbort())
         {
            AfxThrowUserException();
         }

         // Skip empty lines
         if(CCollectionBuilder::StringIsEmpty(csLine))
            continue;

         // Parse the current line into fields.
         csaItems.SetNames(csLine, '\t');

         // Make sure the fields exist.
         if ((csaItems.GetSize() > nDirectoryIndex) && (csaItems.GetSize() > nFilenameIndex))
         {
            // Get the directory and file names.
            csDirectory = csaItems.GetAt(nDirectoryIndex);
            csFilename = csaItems.GetAt(nFilenameIndex);

            if (!csDirectory.IsEmpty() && !csFilename.IsEmpty())
            {
               // Build the name of the project.
               csProject = csInputDirectory;
					Util::RemoveBackslashFromPath(csProject);
               csProject += csDirectory;
               csProject += '\\';
               csProject += csFilename;

               csMessage.Format("Fixing project %s\\%s", (LPCSTR)csDirectory, (LPCSTR)csFilename);
               pProgressDialog->AddMessage(csMessage);

					if (!Util::FileExists(csProject))
					{
                  csMessage.Format("Project file %s\\%s does not exist", (LPCSTR)csDirectory, (LPCSTR)csFilename);
                  pProgressDialog->AddMessage(csMessage);
						cfError.WriteString(csMessage);
						cfError.WriteString("\n");
						continue;
					}

					// Attempt to get Project Type and Subtype
					// NOTE: we only do this for Crafts right now!
					int nProjectType = -1;
					int nProjectSubType = -1;

					// Get project type from extension
					CDocTemplate* pTemplate = GET_PMWAPP()->GetMatchingTemplate(csProject);
					if (pTemplate)
					{
						nProjectType = ((CPmwDocTemplate*)pTemplate)->ProjectType();
					}

					if (nProjectType == PROJECT_TYPE_Craft && nProjectTypeIndex >= 0)
					{
						CString csProjectType = csaItems.GetAt(nProjectTypeIndex);
						if (!csProjectType.IsEmpty())
						{
							CMlsStringArray mlsProjects(IDS_PROJECT_TYPES);
							nProjectType = mlsProjects.FindName(csProjectType, CMlsStringArray::IgnoreCase);
							if (nProjectType < 1)
							{
			               csMessage.Format("Warning: Bad Project Type - %s", (LPCSTR)csProjectType);
			               pProgressDialog->AddMessage(csMessage);
								cfOutput.WriteString(csMessage);
								cfOutput.WriteString("\n");
							}
						}

						CString csProjectSubType;
						if (nProjectSubTypeIndex >= 0)
						{
							if (nProjectType == PROJECT_TYPE_Craft)
							{
								csProjectSubType = csaItems.GetAt(nProjectSubTypeIndex);
								if (!csProjectSubType.IsEmpty())
								{
									CMlsStringArray mlsCrafts(IDS_CRAFT_TYPES);
									nProjectSubType = mlsCrafts.FindName(csProjectSubType, CMlsStringArray::IgnoreCase);

									if (nProjectSubType < 1)
									{
										csMessage.Format("Warning: Bad Project SubType - %s", (LPCSTR)csProjectSubType);
										pProgressDialog->AddMessage(csMessage);
										cfOutput.WriteString(csMessage);
										cfOutput.WriteString("\n");
									}
								}
							}
						}
					}

               // Attempt to open the PrintMaster document.
					pDocument = GET_PMWAPP()->OpenHiddenDocument(csProject);
               if (pDocument == NULL)
               {
                  csMessage.Format("Can't open the project file %s\\%s", (LPCSTR)csDirectory, (LPCSTR)csFilename);
                  pProgressDialog->AddMessage(csMessage);
						cfError.WriteString(csMessage);
						cfError.WriteString("\n");
						continue;
               }

					// Write the document name to the output.
					csMessage.Format("%s\\%s\n",  (LPCSTR)csDirectory, (LPCSTR)csFilename);
					cfOutput.WriteString(csMessage);

					m_pDoc = pDocument;
					pDocumentRecord = pDocument->DocumentRecord();

					// Write Project Type and SubType if available.
					// Again, only for Crafts
					if (nProjectType == PROJECT_TYPE_Craft)
					{
						pDocumentRecord->set_project_type((PROJECT_TYPE)nProjectType);
						csMessage.Format("Setting Project Type to %d", nProjectType);
						pProgressDialog->AddMessage(csMessage);
						csMessage += '\n';
						cfOutput.WriteString(csMessage);
						if (nProjectSubType > 0)
						{
							pDocumentRecord->SetProjectSubtype(nProjectSubType);
							csMessage.Format("Setting Project SubType to %d", nProjectSubType);
							pProgressDialog->AddMessage(csMessage);
							csMessage += '\n';
							cfOutput.WriteString(csMessage);
						}
					}

               pDatabase = pDocument->get_database();
               ASSERT(pDatabase != NULL);

					// Free the embedded graphics.
               if(bFreeEmbedded)
   					DoFreeEmbedded(TRUE, &cfOutput);

					// Prep all the graphics to make sure none are missing.
#ifdef EXTENSIVE_GRAPHIC_FIX
					DB_RECORD_NUMBER record;

					for (record = 1; ; record++)
					{
						DatabaseRecordPtr pRecord;
						ERRORCODE error;

						if ((pRecord = pDatabase->get_record(record, &error)) == NULL)
						{
							if (error == ERRORCODE_DoesNotExist)
							{
								break;
							}
							else if (error == ERRORCODE_NotAllocated)
							{
				//				od("Record %ld: (Free)\r\n", record);
							}
							else
							{
								csMessage.Format("Got error %d for record %ld", error, record);
								cfOutput.WriteString(csMessage);
								break;
							}
						}
						else
						{
							if (pRecord->type() == RECORD_TYPE_Graphic)
							{
								GraphicPtr pGraphic = (GraphicPtr)pRecord;
								ERRORCODE error;
								ReadOnlyFile File;
								if ((error = pGraphic->prep_storage_file(&File)) != ERRORCODE_None
									 || File.seek(0, ST_DEV_SEEK_SET) != ERRORCODE_None)
								{
									pGraphic->handle_open_error(NULL);
									if (pGraphic->record.storage == GRAPHIC_STORAGE_MISSING)
									{
										// Graphic is missing.
										CompositeFileName cfn(pGraphic->file_name);
										csGraphic = cfn.get_file_name();
										csMessage.Format("ERROR: '%s' is missing.\n", (LPCSTR)csGraphic);
										cfOutput.WriteString(csMessage);
									}
								}
							}
							pRecord->release();
						}
					}
#else

					DWORD dwGraphics = pDocumentRecord->NumberOfGraphics();
					for (DWORD dwGraphic = 0; dwGraphic < dwGraphics; dwGraphic++)
					{
						GraphicPtr pGraphic;
						DB_RECORD_NUMBER g_record = pDocumentRecord->GetGraphic(dwGraphic);

						if ((pGraphic = (GraphicPtr)pDatabase->get_record(g_record, NULL, RECORD_TYPE_Graphic)) != NULL)
						{
							ERRORCODE error;
							ReadOnlyFile File;
							if ((error = pGraphic->prep_storage_file(&File)) != ERRORCODE_None
								 || File.seek(0, ST_DEV_SEEK_SET) != ERRORCODE_None)
							{
								pGraphic->handle_open_error(NULL);
								if (pGraphic->record.storage == GRAPHIC_STORAGE_MISSING)
								{
									// Graphic is missing.
									CompositeFileName cfn(pGraphic->m_csFileName);
									csGraphic = cfn.get_file_name();
									csMessage.Format("ERROR: '%s' is missing.\n", (LPCSTR)csGraphic);
									cfOutput.WriteString(csMessage);
								}
							}
						}
						pGraphic->release();
					}
#endif

					// Compact the space.
					DoCompact();

					// And free all the old text box.
					DoFreeOldTextBoxes();

					if (bEmbed)
					{
						pDocument->EmbedAllPictures();
					}

					// Save the document.
					if (!pDocument->DoSave(pDocument->GetPathName()))
					{
						// Could not save.
						csMessage.Format("Unable to save document (%s)\n", (LPCSTR)pDocument->GetPathName());
						cfOutput.WriteString(csMessage);
					}

               // Close the document.
               pDocument->OnCloseDocument();
               pDocument = NULL;
            }
         }

         pProgressDialog->StepProgress();
      }

      csMessage.Format("Normal termination");
      pProgressDialog->AddMessage(csMessage);
   }
   CATCH_ALL(e)
   {
      csMessage.Format("Abnormal termination");
		if (pProgressDialog != NULL)
		{
			pProgressDialog->AddMessage(csMessage);
		}
   }
   END_CATCH_ALL

   if (pProgressDialog != NULL)
   {
      if(!bRunSilent)
      {
         pProgressDialog->WaitForDone();
      }
      else
         pProgressDialog->EndDialog(IDOK);

      pProgressDialog->Finish();
      delete pProgressDialog;
      pProgressDialog = NULL;
   }

   if (pDocument != NULL)
   {
      pDocument->OnCloseDocument();
      pDocument = NULL;
   }

	// Restore the opened document.
	m_pDoc = pDocSave;
}

void CDocDump::DoFreeEmbedded(BOOL fBatch /*=FALSE*/, CStdioFile* pOutputFile /*=NULL*/)
{
	ASSERT(m_pDoc != NULL);
	CString csMessage;

	// Run through all the graphics and free any embedded source.
	DocumentRecord* pDocRec = m_pDoc->DocumentRecord();
	PMGDatabasePtr pDatabase = m_pDoc->get_database();

	DWORD dwGraphics = pDocRec->NumberOfGraphics();

	for (DWORD dwGraphic = 0; dwGraphic < dwGraphics; dwGraphic++)
	{
		DB_RECORD_NUMBER record = pDocRec->GetGraphic(dwGraphic);
		GraphicRecord* pGraphic;
		ERRORCODE error;

		if ((pGraphic = (GraphicRecord*)pDatabase->get_record(record, &error)) == NULL)
		{
			// Got an error.
			CString csBuffer;
			csBuffer.Format("Got error %d for graphic %ld", error, record);
			AfxMessageBox(csBuffer);
		}
		else
		{
			// If this is an embedded graphic, it is a candidate for un-embedding.
			if (pGraphic->record.source != 0)
			{
				BOOL fFreeIt = TRUE;
				// Embedded. See if we can un-embed it.
				switch (m_pPathManager->GetPathBindingType(pGraphic->m_csFileName))
				{
					case PBT_None:
					{
						// Make sure it's not a clipboard image.
						// We assume that any image "xxxxxx\CLIP.yyy" is a clipboard
						// image. This covers 99.99% of the cases. You'd have to
						// embed a graphic with base name CLIP for this to fail.
						// (And it would just leave it embedded - not a fatal problem.)

						CString csName;
						Util::SplitPath(pGraphic->m_csFileName, NULL, NULL, &csName, NULL);
						if (csName.CompareNoCase("CLIP") == 0)
						{
							fFreeIt = FALSE;
						}
						break;
					}
					case PBT_CDDrive:
					case PBT_RemovableDrive:
					{
						// Source is not available. Leave embedded.
						if (fBatch)
						{
							fFreeIt = FALSE;
						}
						else
						{
							csMessage.Format("Un-embed graphic?\n%s", (LPCSTR)pGraphic->m_csFileName);
							fFreeIt = (AfxMessageBox(csMessage, MB_YESNO) == IDYES);
						}
						break;
					}
					default:
					{
						break;
					}
				}

				// If we want to free it, do it now.
				if (fFreeIt)
				{
					pDatabase->delete_record(pGraphic->record.source, RECORD_TYPE_EmbeddedFile);
					pGraphic->record.source = 0;
					m_pDoc->SetModifiedFlag();
				}
				else
				{
					if (pOutputFile != NULL)
					{
						CompositeFileName cfn(pGraphic->m_csFileName);
						csMessage.Format("ERROR: Leaving graphic '%s' embedded\n", cfn.get_file_name());
						pOutputFile->WriteString(csMessage);
					}
				}
			}
			pGraphic->release();
		}
	}
}

void CDocDump::DoCompact()
{
	ASSERT(m_pDoc != NULL);

/*
// Grab the document's database.
*/

	PMGDatabasePtr pDatabase = m_pDoc->get_database();

/*
// Stuff all records into the listbox.
*/

	DB_RECORD_NUMBER record;
	BOOL fReset = TRUE;

	for (record = 1; ; record++)
	{
		DatabaseRecordPtr pRecord;
		ERRORCODE error;

		if ((pRecord = pDatabase->get_record(record, &error)) == NULL)
		{
			if (error == ERRORCODE_DoesNotExist)
			{
				break;
			}
			else if (error == ERRORCODE_NotAllocated)
			{
//				od("Record %ld: (Free)\r\n", record);
			}
			else
			{
				CString csMessage;
				csMessage.Format("Got error %d for record %ld", error, record);
				AfxMessageBox(csMessage);
				break;
			}
		}
		else
		{
			int type = pRecord->type();

			if (type == RECORD_TYPE_EmbeddedFile)
			{
			/* We can't free all; at least one embedded file remains. */
				fReset = FALSE;
			}
			else
			{
				switch (type)
				{
					case RECORD_TYPE_ProjectPreview:
					{
						ProjectPreviewPtr preview = (ProjectPreviewPtr)pRecord;

						preview->remove_preview();
						break;
					}
					case RECORD_TYPE_ArrayBlock:
					case RECORD_TYPE_RecordArrayBlock:
					case RECORD_TYPE_CharacterArrayBlock:
					case RECORD_TYPE_WordArrayBlock:
					case RECORD_TYPE_StyleArrayBlock:
					case RECORD_TYPE_ParagraphArrayBlock:
					case RECORD_TYPE_LineArrayBlock:
					{
						CArrayElementRecord* pArrayRecord = (CArrayElementRecord*)pRecord;
						if (pArrayRecord->ElementCount() > 0)
						{
							// Page in the data.
							pArrayRecord->GetAt(0);
						}
						break;
					}
					default:
					{
						break;
					}
				}
				pRecord->Deallocate();
			}
			pRecord->release(TRUE);
		}
	}
	pDatabase->deallocate_table_storage();

	if (fReset)
	{
		((DumpStorageManager*)pDatabase->get_storage_manager())->reset_storage();
	}

	m_pDoc->SetModifiedFlag();
}

void CDocDump::DoFreeOldTextBoxes()
{
	ASSERT(m_pDoc != NULL);

/*
// Grab the document's database.
*/

	PMGDatabasePtr pDatabase = m_pDoc->get_database();

/*
// Stuff all records into the listbox.
*/

	DB_RECORD_NUMBER record;
	BOOL fReset = TRUE;

	for (record = 1; ; record++)
	{
		DatabaseRecordPtr pRecord;
		ERRORCODE error;

		if ((pRecord = pDatabase->get_record(record, &error)) == NULL)
		{
			if (error == ERRORCODE_DoesNotExist)
			{
				break;
			}
			else if (error == ERRORCODE_NotAllocated)
			{
//				od("Record %ld: (Free)\r\n", record);
			}
			else
			{
				CString csMessage;
				csMessage.Format("Got error %d for record %ld", error, record);
				AfxMessageBox(csMessage);
				break;
			}
		}
		else
		{
			int type = pRecord->type();

			if (pRecord->type() == RECORD_TYPE_OldFrame
					|| pRecord->type() == RECORD_TYPE_Paragraph)
			{
				pRecord->zap();
				m_pDoc->SetModifiedFlag();
			}
			else
			{
				pRecord->release();
			}
		}
	}
}


