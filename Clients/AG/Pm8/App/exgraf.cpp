// $Workfile: exgraf.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
//
//	Extracts graphics from all projects in the projects folder
// to files in a folder created in the destination folder; 
// creates a list and error log in the destination folder.
//
// Copyright © 1998 Mindscape, Inc.
// All rights reserved.
//
// Revision History:
//
// $Log: /PM8/App/exgraf.cpp $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 4     9/22/98 3:32p Johno
// Embedded graphics are skipped by default
// 
// 3     9/14/98 12:08p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 2     9/03/98 4:54p Johno
// Created
//
#include "stdafx.h"

#include	"pmw.h"		// For GET_PMWAPP()
#include "exgraf.h"
#include <io.h>		// For _A_NORMAL
#include	"pmwdoc.h"	// For CPmwDoc
#include "progbar.h"	// For CPMWProgressBar
#include "compfn.h"	// For CompositeFileName

void 
CExtractGraphics::Breathe (HWND BreatheDlg)
{
   MSG Message;

   while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
   {
      if ((BreatheDlg == NULL) || !::IsDialogMessage (BreatheDlg, &Message))
      {
         ::TranslateMessage(&Message);
         ::DispatchMessage(&Message);
      }
   }
}

#include "compfn.h"	// For CompositeFileName
//#include "fontrec.h" // for FontDataRecord
//#include "stylerec.h" // for TextStyleRecord
//#include "pagerec.h" // for PageRecord
//#include "layrec.h" // for ObjectLayoutRecord
//#include "pathrec.h" // for OutlinePathRecord
//#include "warpfld.h" // for WarpFieldRecord
//#include "framerec.h" // for FrameRecord
//#include "grpobj.h" // for GroupObject
//#include "calobj.h" // for CalendarObject
//#include "lineobj.h" // for LineObject
//#include "patobj.h" // for PatternObject
#include "grafobj.h" // for GraphicObject
//#include "backobj.h" // for BackgroundObject
//#include "rectobj.h" // for RectangleObject
//#include "brdrobj.h" // for BorderObject
#include "compobj.h" // for ComponentObject
//#include "dateobj.h" // for CalendarDateObject
//#include "tblobj.h" // for CTableObject
//#include "oleobj.h" // for OleObject
#include "ComponentView.h" // for RComponentView
#include "ImageInterface.h" // for RImageInterface
#include "BitmapImage.h" // for RBitmapImage
#include "pmgobj.h" // for CObjectProperties
#include "carddoc.h" // for CCardDoc
#include "comprec.h" // for ComponentRecord
#include "ComponentDocument.h" // for ComponentDocument
//#include "PathInterface.h" // for IPathInterface
//#include "Typeface.h" // for FaceEntry
//#include "PmgFont.h" // for PMGFontServer
//#include "Path.h" // for RPath
//#include "Paper.h" // for CPaperInfo
void
CExtractGraphics::Extract(CString &Projects)
{
	CPmwDoc*				pDocument;
	PMGDatabase*		pDatabase;
	CString				ProjName, ProjPathIn, ArtPath, ProjNameFull, Temp, ErrorStr;
	// Build the source path
	ProjPathIn = Projects;
	Util::RemoveBackslashFromPath(ProjPathIn);
	ProjPathIn += "\\";
	// Build the art destination path, create folder or die
	ArtPath = ProjPathIn;
//j	ArtPath += "..\\art\\";
	if (Util::MakeDirectory(ArtPath) != TRUE)
	{
      ErrorStr.Format("Failed to create\r\n%s", ArtPath);
		AfxMessageBox(ErrorStr);
      return;
	}
	// Create the error log, or die
	Temp = ProjPathIn;
	Util::RemoveBackslashFromPath(Temp);
	Temp += "\\errorlog.txt";
	CMyFile	ErrorFile;
	if(ErrorFile.Open(Temp, CFile::modeCreate | CFile::modeWrite ) == FALSE)
   {
      ErrorStr.Format("Failed to open error log file\r\n%s", Temp);
		AfxMessageBox(ErrorStr);
      return;
   }
	// Create the file list, or die
	Temp = ProjPathIn;
	Util::RemoveBackslashFromPath(Temp);
	Temp += "\\filelist.txt";
	CMyFile	ListFile;
	if(ListFile.Open(Temp, CFile::modeCreate | CFile::modeWrite ) == FALSE)
   {
      ErrorStr.Format("Failed to open error log file\r\n%s", Temp);
		AfxMessageBox(ErrorStr);

		ErrorFile.WriteLines(ErrorStr);
      ErrorFile.Close();
		return;
   }

	int	ErrorCount = 0;
	// Initialize ourselves
	Init(ProjPathIn);
	// Create a progress dialog
	CPMWProgressBar ProgressDialog;
	ProgressDialog.Create(IDD_PROGRESS_BAR, AfxGetMainWnd());
	CProgressCtrl *pProgressBar = (CProgressCtrl *)ProgressDialog.GetDlgItem(IDC_PROGRESS_CONTROL);
	if(pProgressBar)
	{
		pProgressBar->SetRange(1, 10);
		pProgressBar->SetStep(1);
	}
	int	StepCount = 0;
	// Loop through all projects in source
	while ((GetFile(ProjName, ErrorFile) == TRUE) && (ProgressDialog.CancelHit() == FALSE))
	{
		ProjNameFull = ProjPathIn;
		ProjNameFull += ProjName;
		// Look busy
		ProgressDialog.SetWindowText(ProjNameFull);
		// Try to open the project
		if ((pDocument = GET_PMWAPP()->OpenHiddenDocument(ProjNameFull)) != NULL)
		{
			pDatabase = pDocument->get_database();
			ASSERT(pDatabase != NULL);
			if (pDatabase)	// Need it's database
			{
				DocumentRecord* pDocumentRecord = pDocument->DocumentRecord();
				ASSERT(pDocumentRecord != NULL);
				if (pDocumentRecord != NULL)	// Need it's document record
				{
					DWORD dwGraphics = pDocumentRecord->NumberOfGraphics();
					// Loop through all graphics here
					for (DWORD dwGraphic = 0; dwGraphic < dwGraphics; dwGraphic++)
					{
						DB_RECORD_NUMBER g_record = pDocumentRecord->GetGraphic(dwGraphic);
						GraphicPtr pGraphic;
						if ((pGraphic = (GraphicPtr)pDatabase->get_record(g_record, NULL, RECORD_TYPE_Graphic)) != NULL)
						{
							CompositeFileName cfn(pGraphic->m_csFileName);	// Handles goofy characters maybe used in name
							ReadOnlyFile File;
							ERRORCODE e = pGraphic->prep_storage_file(&File);	// Get a "file" of this graphic
							if (e == ERRORCODE_None)
							{
								// Build a destination file path
								CString GrafName = cfn.get_dynamic_name(TRUE);
								GrafName.Replace('\\', '-');
								GrafName.Replace(':', '-');
								GrafName.Replace('[', '-');
								GrafName.Replace(']', '-');
								CString	GrafDest = ArtPath + ProjName + GrafName;
								// Look busy
								ProgressDialog.SetWindowText(GrafDest);
								StorageFile	FileOut(GrafDest);
								// Copy file
								e = copy_file(&File, &FileOut);
								if (e == ERRORCODE_None)
									ListFile.WriteLines(cfn.get_dynamic_name(TRUE), " -> ", GrafDest);
								else
									ErrorStr.Format("Couldn't copy (%s) %s in %s", (LPCSTR)pGraphic->m_csName, cfn.get_dynamic_name(TRUE), ProjName);
							}
							else
								ErrorStr.Format("Couldn't get file for (%s) %s in %s", (LPCSTR)pGraphic->m_csName, cfn.get_dynamic_name(TRUE), ProjName);

							pGraphic->release();
						}
						else
							ErrorStr.Format("Couldn't get graphic record in %s", ProjNameFull);
						// Look busy
						if(pProgressBar)
						{
							pProgressBar->StepIt();
							StepCount++;
							if(StepCount > 9)
							{
								StepCount = 0;
								pProgressBar->SetStep(1);
							}
						}
						// Deal with error
						if (!ErrorStr.IsEmpty())
						{
							ErrorCount ++;

							ErrorFile.WriteLines(ErrorStr);
							ErrorStr.Empty();
						}
						// Look busy
						Breathe(ProgressDialog.GetSafeHwnd());	
					}
				}
				else
					ErrorStr.Format("Couldn't get document record for %s", ProjNameFull); 
			}
			else
				ErrorStr.Format("Couldn't open database for %s", ProjNameFull); 

			pDocument->OnCloseDocument();
			// Deal with error
			if (!ErrorStr.IsEmpty())
			{
				ErrorCount ++;

				ErrorFile.WriteLines(ErrorStr);
				ErrorStr.Empty();
			}
		}
		else
		{
			ErrorCount ++;
			ErrorFile.WriteLines("Couldn't open ", ProjNameFull);
		}
		// Look busy
		Breathe(ProgressDialog.GetSafeHwnd());	
	}

	if (ProgressDialog.CancelHit() == TRUE)
	{
		CString	Temp = "Aborted by user";
		ErrorFile.WriteLines(Temp);	
		ListFile.WriteLines(Temp);	
	}

	ProgressDialog.DestroyWindow();

	ListFile.Close();
	ErrorFile.Close();

	if (ErrorCount != 0)
	{
		ErrorStr.Format("There were %d errors", ErrorCount);
		AfxMessageBox(ErrorStr);
	}
}
// Get the next project file name
BOOL
CExtractGraphics::GetFile(CString &Name, CMyFile &ErrorFile)
{
	BOOL	ret = FALSE;
	
	Name.Empty();
	if (pFileIterator != NULL)
	{
		do
		{
			if (m_Started == FALSE)
			{
				m_Started = TRUE;
				ret = pFileIterator->FindFirst(m_Search, _A_NORMAL);
			}
			else
				ret = pFileIterator->FindNext();
			// Test if this is a PrintMaster Project file
			if (ret == TRUE)
			{
				if (GET_PMWAPP()->GetMatchingTemplate(pFileIterator->Name(), TRUE) != NULL)
				{
					Name = pFileIterator->Name();
					return ret;
				}
				else
				{
					ErrorFile.WriteLines("Not a project file: ", pFileIterator->Name());
				}
			}

		} while (ret == TRUE);
	}

	return ret;
}

void
CExtractGraphics::Init(CString &Projects)
{
	Kill();	// Kill any previous File Iterator
	m_Search.Format("%s%s", Projects, "*.*");

	TRY
	{
		pFileIterator = new CFileIterator;
	}
	END_TRY
}


