/*
// $Workfile: PROJGRPH.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PROJGRPH.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 9     10/02/98 7:10p Jayn
// More content manager changes
// 
// 8     9/23/98 10:28a Jayn
// Converted CollectionManager to COM-style object.
// 
// 7     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 6     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 5     8/28/98 3:41p Jayn
// Calendar fixes
// 
// 4     5/19/98 11:35a Dennis
// Added progress bar and methods to control it.
// 
// 3     4/20/98 9:09a Jayn
// Better messages.
// 
// 2     2/27/98 2:45p Jayn
// Added Extract Project
// 
//    Rev 1.0   14 Aug 1997 15:25:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:10   Fred
// Initial revision.
// 
//    Rev 1.2   24 Jul 1997 08:29:40   Fred
//  
// 
//    Rev 1.1   15 Jul 1997 15:57:12   Fred
//  
// 
//    Rev 1.0   14 Jul 1997 11:19:30   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "projgrph.h"
#include "sortarry.h"
#include "pmwdoc.h"
#include "compfn.h"
#include "util.h"

// Helper functions.

static BOOL FinishItem(CMlsStringArray& csaItems, LPCSTR pszItem)
{
   BOOL fResult = FALSE;

   if (*pszItem != '\0')
   {
      CString csItem;
      TRY
      {
         csItem = '"';
         csItem += pszItem;
         csItem += '"';
         csaItems.Add(csItem);
         fResult = TRUE;
      }
      END_TRY
   }

   return fResult;
}

static BOOL StartLine(CMlsStringArray& csaItems, LPCSTR pszDirectory, LPCSTR pszFilename, LPCSTR pszGraphic)
{
   BOOL fResult = FALSE;

   CString csItem;
   TRY
   {
      csaItems.RemoveAll();
      if (FinishItem(csaItems, pszDirectory)
       && FinishItem(csaItems, pszFilename)
       && FinishItem(csaItems, pszGraphic))
      {
         fResult = TRUE;
      }
   }
   END_TRY

   return fResult;
}

static void FinishLine(CMlsStringArray& csaItems, CStdioFile& cfOutput)
{
   // Write out the text line.
   CString csLine;

   TRY
   {
      csaItems.GetNames(csLine, '\t');
      if (!csLine.IsEmpty())
      {
         csLine += '\n';
         cfOutput.WriteString(csLine);
      }
   }
   END_TRY
}


// Note: The object is used for utility purposes only.
// There is nothing that will be localized in this object.

CFindProjectsWithGraphics::CFindProjectsWithGraphics()
{
}

CFindProjectsWithGraphics::~CFindProjectsWithGraphics()
{
}

void CFindProjectsWithGraphics::Run(void)
{
   // Get the name of the file to load.
   CFileDialog OpenDialog(
                  TRUE,
                  "*.txt",
                  NULL,
                  OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
                  "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||",
                  AfxGetMainWnd());

   CString csInput;
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
   CFindProjectsWithGraphicsProgressDialog* m_pProgressDialog = NULL;

   TRY
   {
	// Ask for the project spreadsheet file.
      if (OpenDialog.DoModal() == IDOK)
      {
         // Get the input file name.
         csInput = OpenDialog.GetPathName();
			if (!csInput.IsEmpty())
			{
				// Open up the text file.
				if (!cfInput.Open(csInput, CFile::modeRead|CFile::shareDenyNone|CFile::typeBinary))
				{
					csMessage.Format("Can't open input file %s", (LPCSTR)csInput);
					cfError.WriteString(csMessage);
					cfError.WriteString("\n");
					AfxMessageBox(csMessage);
					AfxThrowUserException();
				}
			}
		}

		  CFileDialog saveDlg(FALSE, "*.txt", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
						  "Comma Delimited (*.txt)|*.txt|All Files (*.*)|*.*||", AfxGetMainWnd());

		 if(saveDlg.DoModal() == IDOK)
	  		 csOutput = saveDlg.GetPathName();
	  	 else
	  		 AfxThrowUserException();

     // Create the error file.
     csError = csOutput.Left(csOutput.ReverseFind('\\') + 1) + "PROJERR.TXT";
      if (!cfError.Open(csError, CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
      {
         csMessage.Format("Can't create error file %s", (LPCSTR)csError);
         AfxMessageBox(csMessage);
         AfxThrowUserException();
      }

		Util::SplitPath(csInput, &csInputDirectory, NULL);

      // Create a progress dialog.
      m_pProgressDialog = new CFindProjectsWithGraphicsProgressDialog(IDD_FIND_PROJECTS_WITH_GRAPHICS_PROGRESS_DIALOG, AfxGetMainWnd());

		// Report what files we're using.
      csMessage.Format("Error File: %s", (LPCSTR)csError);
      m_pProgressDialog->AddMessage(csMessage);
      csMessage.Format("Input File: %s", (LPCSTR)csInput);
      m_pProgressDialog->AddMessage(csMessage);

      csMessage.Format("Reading header information");
      m_pProgressDialog->AddMessage(csMessage);

      // Read the header and figure out the column to use for directory and file name.
      Util::ReadString(&cfInput, csLine);
      csLine.MakeUpper();

      // Parse the items.
      csaItems.SetNames(csLine, '\t');

      csMessage.Format("Locating DIRECTORY and FILE NAME columns");
      m_pProgressDialog->AddMessage(csMessage);

      // Try to find the "DIRECTORY" field.
      int nDirectoryIndex = csaItems.FindName("DIRECTORY");
      if (nDirectoryIndex == -1)
      {
         csMessage.Format("No DIRECTORY Field in %s", (LPCSTR)csInput);
         m_pProgressDialog->AddMessage(csMessage);
			cfError.WriteString(csMessage);
			cfError.WriteString("\n");
         AfxThrowUserException();
      }

      // Try to find the "FILE NAME" field.
      int nFilenameIndex = csaItems.FindName("FILE NAME");
      if (nFilenameIndex == -1)
      {
         csMessage.Format("No FILE NAME Field in %s", (LPCSTR)csInput);
         m_pProgressDialog->AddMessage(csMessage);
			cfError.WriteString(csMessage);
			cfError.WriteString("\n");
         AfxThrowUserException();
      }

      // Skip the next header line.
      Util::ReadString(&cfInput, csLine);

      // Create the output file.
      csMessage.Format("Output File: %s", (LPCSTR)csOutput);
      m_pProgressDialog->AddMessage(csMessage);

      if (!cfOutput.Open(csOutput, CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
      {
         csMessage.Format("Can't create output file %s", (LPCSTR)csOutput);
         m_pProgressDialog->AddMessage(csMessage);
			cfError.WriteString(csMessage);
			cfError.WriteString("\n");
         AfxThrowUserException();
      }

      // Process each line from the input file.
      while (Util::ReadString(&cfInput, csLine))
      {
         if (m_pProgressDialog->CheckForAbort())
         {
            AfxThrowUserException();
         }

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

               csMessage.Format("Examining graphics in project %s\\%s", (LPCSTR)csDirectory, (LPCSTR)csFilename);
               m_pProgressDialog->AddMessage(csMessage);

					if (!Util::FileExists(csProject))
					{
                  csMessage.Format("Project file %s\\%s does not exist", (LPCSTR)csDirectory, (LPCSTR)csFilename);
                  m_pProgressDialog->AddMessage(csMessage);
						cfError.WriteString(csMessage);
						cfError.WriteString("\n");
						continue;
					}

               // Attempt to open the PrintMaster document.
					pDocument = GET_PMWAPP()->OpenHiddenDocument(csProject);
               if (pDocument == NULL)
               {
                  csMessage.Format("Can't open the project file %s\\%s", (LPCSTR)csDirectory, (LPCSTR)csFilename);
                  m_pProgressDialog->AddMessage(csMessage);
						cfError.WriteString(csMessage);
						cfError.WriteString("\n");
						continue;
               }

               pDatabase = pDocument->get_database();
               ASSERT(pDatabase != NULL);

					DocumentRecord* pDocumentRecord = pDocument->DocumentRecord();
					ASSERT(pDocumentRecord != NULL);

					// Run through all the graphics.
					DWORD dwGraphics = pDocumentRecord->NumberOfGraphics();
					for (DWORD dwGraphic = 0; dwGraphic < dwGraphics; dwGraphic++)
					{
						// Check if the user wants to stop.
                  if (m_pProgressDialog->CheckForAbort())
                  {
                     AfxThrowUserException();
                  }

						GraphicPtr pGraphic;
						DB_RECORD_NUMBER g_record = pDocumentRecord->GetGraphic(dwGraphic);

						if ((pGraphic = (GraphicPtr)pDatabase->get_record(g_record, NULL, RECORD_TYPE_Graphic)) != NULL)
						{
							CompositeFileName cfn(pGraphic->m_csFileName);
                     csItem.Format("%s {%d}", (LPCSTR)pGraphic->m_csName, pGraphic->record.reference_count);
							pGraphic->release();
							csGraphic = cfn.get_file_name();
							if (!csGraphic.IsEmpty())
							{
								Util::SplitPath(csGraphic, NULL, &csGraphicBaseName);
								// Project contains one of the specified graphics.
								if (StartLine(csaText, csDirectory, csFilename, csItem))
								{ // Write out the Text line.
									FinishItem(csaText, csGraphic);
									FinishLine(csaText, cfOutput);
									csMessage.Format("Project uses %s", csGraphic);
									m_pProgressDialog->AddMessage(csMessage);
								}
							}
						}
					}

               // Close the document.
               pDocument->OnCloseDocument();
               pDocument = NULL;
            }
         }
      }

      csMessage.Format("Normal termination");
      m_pProgressDialog->AddMessage(csMessage);
   }
   CATCH_ALL(e)
   {
      csMessage.Format("Abnormal termination");
		if (m_pProgressDialog != NULL)
		{
			m_pProgressDialog->AddMessage(csMessage);
		}
   }
   END_CATCH_ALL

   if (m_pProgressDialog != NULL)
   {
      m_pProgressDialog->WaitForDone();
      m_pProgressDialog->Finish();
      delete m_pProgressDialog;
      m_pProgressDialog = NULL;
   }

   if (pDocument != NULL)
   {
      pDocument->OnCloseDocument();
      pDocument = NULL;
   }
}


/////////////////////////////////////////////////////////////////////////////
// CFindProjectsWithGraphicsProgressDialog dialog

CFindProjectsWithGraphicsProgressDialog::CFindProjectsWithGraphicsProgressDialog(int IDD, CWnd* pParent /*=NULL*/)
   : CDialog(IDD, pParent)
{
   //{{AFX_DATA_INIT(CInstallProgressDialog)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   m_fIsAborted = FALSE;
   m_fIsDone = FALSE;
   m_pMessageList = NULL;
   m_pParent = pParent;
   
   m_pParent->EnableWindow(FALSE);
   
   Create(IDD, pParent);
}

CFindProjectsWithGraphicsProgressDialog::~CFindProjectsWithGraphicsProgressDialog()
{
   Finish();
}

void CFindProjectsWithGraphicsProgressDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CFindProjectsWithGraphicsProgressDialog)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFindProjectsWithGraphicsProgressDialog, CDialog)
   //{{AFX_MSG_MAP(CFindProjectsWithGraphicsProgressDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFindProjectsWithGraphicsProgressDialog::Breathe(void)
{
   MSG Message;

   while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
   {
      if ((m_hWnd == 0) || !IsDialogMessage(&Message))
      {
         ::TranslateMessage(&Message);
         ::DispatchMessage(&Message);
      }
   }
}

void CFindProjectsWithGraphicsProgressDialog::WaitForDone(void)
{
   CWnd* pOK = GetDlgItem(IDOK);
   if (pOK != NULL)
   {
      pOK->EnableWindow(TRUE);
      CWnd* pCancel = GetDlgItem(IDCANCEL);
      if (pCancel != NULL)
      {
         pCancel->EnableWindow(FALSE);
      }

      while (!m_fIsDone)
      {
         Breathe();
      }
   }
}

void CFindProjectsWithGraphicsProgressDialog::Finish(void)
{
   if (m_hWnd != NULL)
   {
      m_pParent->EnableWindow(TRUE);
      DestroyWindow();
   }
}

void CFindProjectsWithGraphicsProgressDialog::AddMessage(LPCSTR pszAction)
{
   if (m_pMessageList != NULL)
   {
      int nIndex = m_pMessageList->AddString(pszAction);
		if (nIndex != LB_ERR)
		{
			m_pMessageList->SetCurSel(nIndex);
		}

      Breathe();
   }
}

/////////////////////////////////////////////////////////////////////////////
// CFindProjectsWithGraphicsProgressDialog message handlers

void CFindProjectsWithGraphicsProgressDialog::OnCancel()
{
   // Make sure the user really wants to cancel the installation.
   if (AfxMessageBox("Do you really want to stop extracting project text?", MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
   {
      m_fIsAborted = TRUE;
   }
}

void CFindProjectsWithGraphicsProgressDialog::OnOK()
{
   m_fIsDone = TRUE;
}

BOOL CFindProjectsWithGraphicsProgressDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   CenterWindow();

   m_pMessageList = (CListBox*)GetDlgItem(IDC_PROGRESS_MESSAGE_LIST);

   m_pProgressBar = (CProgressCtrl *) GetDlgItem(IDC_FIND_PROJECTS_WITH_GRAPHICS_PROGRESS);
   ASSERT(m_pProgressBar);
   if(m_pProgressBar)
      {
         SetProgressStep(1);
      }
   
   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFindProjectsWithGraphicsProgressDialog::SetProgressRange(int nStart, int nEnd)
   {
      if(m_pProgressBar == NULL)
         return;
      m_pProgressBar->SetRange(nStart, nEnd);
   }

void CFindProjectsWithGraphicsProgressDialog::SetProgressStep(int nStep)
   {
      if(m_pProgressBar == NULL)
         return;
      m_pProgressBar->SetStep(nStep);
   }

void CFindProjectsWithGraphicsProgressDialog::StepProgress()
   {
      if(m_pProgressBar == NULL)
         return;
      m_pProgressBar->StepIt();
   }

/////////////////////////////////////////////////////////////////////////////
// Extract content class.

CExtractContent::CExtractContent()
{
}

CExtractContent::~CExtractContent()
{
}

void CExtractContent::Run(void)
{
   // Get the name of the text file to load.
   CFileDialog OpenDialog(
                  TRUE,
                  "*.txt",
                  NULL,
                  OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
                  "TXT Files (*.txt)|*.txt|All Files (*.*)|*.*||",
                  AfxGetMainWnd());

   CString csInput;
	CString csInputDrive;
	CString csInputDirectory;
	CString csInputBaseName;
	CString csInputExtension;
   CStdioFile cfInput;
   CString csLine;
   CString csItem;
	CString csName;
   CString csError;
   CStdioFile cfError;
   CString csDirectory;
   CString csFilename;
   CString csMessage;
   CString csSource;
	CString csLeft, csRight;
	CMlsStringArray csaItems;
   CFindProjectsWithGraphicsProgressDialog* m_pProgressDialog = NULL;

   TRY
   {
      // Create the error file.
      csError = GetGlobalPathManager()->ExpandPath("EXTRERR.TXT");
      if (!cfError.Open(csError, CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
      {
         csMessage.Format("Can't create error file %s", (LPCSTR)csError);
         AfxMessageBox(csMessage);
         AfxThrowUserException();
      }

		// Ask for the input file name.
      if (OpenDialog.DoModal() != IDOK)
		{
         AfxThrowUserException();
		}

      // Get the input file name.
      csInput = OpenDialog.GetPathName();
		ASSERT(!csInput.IsEmpty());

		// Split up the name of the input file so we can build the spreadsheet name.
		Util::SplitPath(csInput, &csInputDrive, &csInputDirectory, &csInputBaseName, &csInputExtension);

		// Open up the input file.
		if (!cfInput.Open(csInput, CFile::modeRead|CFile::shareDenyNone|CFile::typeBinary))
		{
			csMessage.Format("Can't open input file '%s'", (LPCSTR)csInput);
			cfError.WriteString(csMessage);
			cfError.WriteString("\n");
			AfxMessageBox(csMessage);
			AfxThrowUserException();
		}

		// ExtractOverwrite
		BOOL fOverwrite = false;
		// AddSubdirectory
		int nSubdirs = 5;

      // Create a progress dialog.
      m_pProgressDialog = new CFindProjectsWithGraphicsProgressDialog(IDD_FIND_PROJECTS_WITH_GRAPHICS_PROGRESS_DIALOG, AfxGetMainWnd());
		m_pProgressDialog->SetWindowText("Extract Content");

		// Report what files we're using.
      csMessage.Format("Error File: %s", (LPCSTR)csError);
      m_pProgressDialog->AddMessage(csMessage);
      csMessage.Format("Input File: %s", (LPCSTR)csInput);
      m_pProgressDialog->AddMessage(csMessage);

//j      csMessage.Format("Verifying source collection");
//j      m_pProgressDialog->AddMessage(csMessage);
//j
//j		if (GetGlobalCollectionManager()->FindCollection(csInputBaseName) == NULL)
//j		{
//j         csMessage.Format("Collection %s not available.", (LPCSTR)csInputBaseName);
//j         m_pProgressDialog->AddMessage(csMessage);
//j			cfError.WriteString(csMessage);
//j			cfError.WriteString("\n");
//j         AfxThrowUserException();
//j		}

      csMessage.Format("Reading header information");
      m_pProgressDialog->AddMessage(csMessage);

      // Read the header and figure out the column to use for directory and file name.
      Util::ReadString(&cfInput, csLine);
      csLine.MakeUpper();

      // Parse the items.
      csaItems.SetNames(csLine, '\t');

      csMessage.Format("Locating DIRECTORY and FILE NAME columns");
      m_pProgressDialog->AddMessage(csMessage);

      // Try to find the "DIRECTORY" field.
      int nDirectoryIndex = csaItems.FindName("DIRECTORY");
      if (nDirectoryIndex == -1)
      {
         csMessage.Format("No DIRECTORY Field in %s", (LPCSTR)csInput);
         m_pProgressDialog->AddMessage(csMessage);
			cfError.WriteString(csMessage);
			cfError.WriteString("\n");
         AfxThrowUserException();
      }

      // Try to find the "FILE NAME" field.
      int nFilenameIndex = csaItems.FindName("FILE NAME");
      if (nFilenameIndex == -1)
      {
         csMessage.Format("No FILE NAME Field in %s", (LPCSTR)csInput);
         m_pProgressDialog->AddMessage(csMessage);
			cfError.WriteString(csMessage);
			cfError.WriteString("\n");
         AfxThrowUserException();
      }

      // Skip the next header line.
      Util::ReadString(&cfInput, csLine);

      // Process each line from the input file.
      while (Util::ReadString(&cfInput, csLine))
      {
         if (m_pProgressDialog->CheckForAbort())
         {
            AfxThrowUserException();
         }

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
					CString csDir;
					csItem = csInputDrive;
					csItem += csInputDirectory;
					Util::RemoveBackslashFromPath(csItem);		// Cause csDirectory starts with one.
					csItem += csDirectory;
					csDir = csItem;			// Remember this for later.
               csItem += '\\';

					if (nSubdirs > 0)
					{
						// Split the path off the base name.
						CString csFoo;
						Util::SplitPath(csFilename, NULL, &csFoo);
						csItem += csFoo;
					}
					else
					{
						csItem += csFilename;
					}

					// 
					// Split out the directory this for later.
					Util::SplitPath(csItem, &csDir, NULL);

					BOOL fExists = Util::FileExists(csItem);
					if (fExists && !fOverwrite)
					{
						// We do not want to overwrite the file.
                  csMessage.Format("Skipping existing file %s\\%s", (LPCSTR)csDirectory, (LPCSTR)csFilename);
                  m_pProgressDialog->AddMessage(csMessage);
						continue;
					}

					if (fExists)
					{
						csMessage.Format("Overwriting file %s\\%s", (LPCSTR)csDirectory, (LPCSTR)csFilename);
					}
					else
					{
						csMessage.Format("Creating file %s\\%s", (LPCSTR)csDirectory, (LPCSTR)csFilename);
					}
               m_pProgressDialog->AddMessage(csMessage);

					// Build the name of the file to extract from.

					csName = csFilename;
					int nDirs = nSubdirs;

					// A hack: if the subdirectory ends in a backslash, and we are doing
					// subdirs, then the item was saved with one less directory than expected.
					// So, we decrease the count by one.

					if (!csDirectory.IsEmpty() && csDirectory[csDirectory.GetLength()-1] == '\\')
					{
						nDirs--;
					}
					while (nDirs-- > 0 && !csDirectory.IsEmpty())
					{
						Util::RemoveBackslashFromPath(csDirectory);
						Util::SplitPath(csDirectory, &csLeft, &csRight);
						// Append the name.
						csRight += '\\';
						csRight += csName;
						// Set the name to this.
						csName = csRight;
						// Set the directory to what is left.
						csDirectory = csLeft;
					}

					// Make sure the directory exists.
					Util::MakeDirectory(csDir);

					csSource.Format("[[G:%s]]\\%s", (LPCSTR)csInputBaseName, (LPCSTR)csName);
//					TRACE("Copying '%s' to '%s'\n", (LPCSTR)csSource, (LPCSTR)csItem);
					ReadOnlyFile SourceFile(csSource);
					StorageFile DestFile(csItem);
					TRY
					{
						ERRORCODE error;
						if ((error = copy_file(&SourceFile, &DestFile)) != ERRORCODE_None)
						{
							csMessage.Format("Error %d extracting %s\\%s", error, (LPCSTR)csDirectory, (LPCSTR)csFilename);
							m_pProgressDialog->AddMessage(csMessage);
							cfError.WriteString(csMessage);
							cfError.WriteString("\n");
						}
					}
					END_TRY
            }
         }
      }

      csMessage.Format("Normal termination");
      m_pProgressDialog->AddMessage(csMessage);
   }
   CATCH_ALL(e)
   {
      csMessage.Format("Abnormal termination");
		if (m_pProgressDialog != NULL)
		{
			m_pProgressDialog->AddMessage(csMessage);
		}
   }
   END_CATCH_ALL

   if (m_pProgressDialog != NULL)
   {
      m_pProgressDialog->WaitForDone();
      m_pProgressDialog->Finish();
      delete m_pProgressDialog;
      m_pProgressDialog = NULL;
   }
}

