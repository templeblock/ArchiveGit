/*
// $Workfile: CheckArtExists.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $ 
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/
 
/*
// Revision History:
//
// $Log: /PM8/App/CheckArtExists.cpp $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 5     8/05/98 12:00p Mwilson
// added ability to check for art in a spreadsheet
// 
// 4     5/18/98 2:47p Dennis
// fixed in release version
// 
// 3     5/15/98 9:50a Mwilson
// added check for txt files
// 
// 2     5/14/98 11:50a Mwilson
// moved include of pmwdoc.h out of header and into cpp file
// 
// 1     5/14/98 10:32a Mwilson
// Utility for checking to make sure the art is all there for a directory
// of pmw files
// 
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwDoc.h"
#include "CheckArtExists.h"
#include "pmdirdlg.h"
#include "pmgobj.h"
#include "pmgdb.h"
#include "carddoc.h"
#include "ArtExistsDlg.h"
#include "grpobj.h"
#include "ArtCheck.h"
#include "sortarry.h"
#include "util.h"


// Note: The object is used for utility purposes only.
// There is nothing that will be localized in this object.

CCheckArtExists::CCheckArtExists():
	m_pListDlg(NULL),
	m_bSrcIsDir(TRUE),
	m_bCheckArt(FALSE)
{
}

CCheckArtExists::~CCheckArtExists()
{
	if(m_pListDlg)
		delete m_pListDlg;
}

void CCheckArtExists::Run(void)
{

	CArtCheckUtil dlg(m_bSrcIsDir);
	if(dlg.DoModal() == IDOK)
	{
		m_bCheckArt = dlg.m_bCheckArt;

		GetCurrentDirectory(MAX_PATH, m_szCurDir );
		if(!SetupProjectIterator(dlg.m_csArtSource, dlg.m_csProjSource))
			return;

		if(dlg.m_bGenerateFileList)
		{
			//open the error log
			CString csFileList = m_csArtDir + "FileNameList.txt";
			if(m_cfOutFileNames.Open(csFileList , CFile::modeCreate | CFile::modeWrite ) == FALSE)
			{
				if(AfxMessageBox("Failed to output file for file names list.  Continue?", MB_YESNO) == IDNO)
					return;
			}
		}
		else if(!m_bCheckArt)
		{
			AfxMessageBox("No option was selected.");
		}

		//open the error log
		CString csErrorLog = m_csProjDir + "\\ErrorLog.txt";
      if(m_errorFile.Open(csErrorLog , CFile::modeCreate | CFile::modeWrite ) == FALSE)
      {
         AfxMessageBox("Failed to open error log file");
         return;
      }

		//create the output dlg
		if(m_pListDlg == NULL)
			m_pListDlg = new CArtExistsDlg;
		
		if(m_pListDlg->GetSafeHwnd() == NULL)
			m_pListDlg->Create(IDD_ART_EXISTS);

		//ensure the dialog is visible
		m_pListDlg->ShowWindow(SW_SHOW);
		CListBox* pListBox = (CListBox*)m_pListDlg->GetDlgItem(IDC_LIST_ERRORS);
		pListBox->ResetContent();
		m_errorList.RemoveAll();

		CString csFilePath;
		while((csFilePath = GetNextProjName()).IsEmpty() == FALSE)
		{
			CString csListString;
			csListString.Format("Processing File %s",csFilePath);

			if(pListBox->GetSafeHwnd() != NULL)
			{
				pListBox->AddString(csFilePath);
				pListBox->UpdateWindow();
			}
			//if the hwnd is null they closed the output dlg.  This aborts the process
			else
				break;

			CPmwDoc* pDoc = GET_PMWAPP()->OpenHiddenDocument(csFilePath);				

			if(pDoc)
			{
				PMGDatabase* pDatabase = pDoc->get_database();
				BOOL bContinue = TRUE;
				
				int nLoopCount = 0;
				BOOL bCardDoc = FALSE;
				//set up the loop count if it is a card loop panels else loop pages
				if(pDoc->IsKindOf(RUNTIME_CLASS(CCardDoc)))
				{
					nLoopCount = 2;
					bCardDoc = TRUE;
				}
				else
					nLoopCount = pDoc->NumberOfPages();

				//loop throught the panels/pages
				for(int i = 0; i < nLoopCount; i++)
				{
					if(bCardDoc)
						((CCardDoc*)pDoc)->set_panel(i);
					else
						pDoc->GotoPage(i);

					//get and check the object list
					ObjectList* pObjects = pDoc->object_list();						
					CheckObjectList(pListBox, pObjects);
				}
				pDoc->OnCloseDocument();
			}
		}
		
		//reset the list box and output any errors
		if(pListBox->GetSafeHwnd() != NULL)
		{
			pListBox->ResetContent();
			if(m_errorList.GetSize() > 0)
			{
				CString strNumErrors;
				strNumErrors.Format("There are %d missing graphics", m_errorList.GetSize());
				pListBox->AddString(strNumErrors);
				for(int i = 0; i< m_errorList.GetSize(); i++)
				{
					pListBox->AddString(m_errorList[i]);
				}
			}
			else 
				pListBox->AddString("No Graphics are missing");
		}
		//set the directory back
		SetCurrentDirectory(m_szCurDir);
		m_errorFile.Close();
	}

}

void CCheckArtExists::CheckObjectList(CListBox* pListBox, ObjectList *pList)
{
	PMGPageObject *   pObject;

	//loop the object list
	for (pObject = (PMGPageObject*)pList->first_object();
						pObject != NULL;
						pObject = (PMGPageObject*)pObject->next_object())
	{
		//check for grapic or group type
		if(pObject->type() == OBJECT_TYPE_Graphic)
		{
			//get the graphic name
			char name[256];
			pObject->get_name(name, 256, 256);
			if(m_cfOutFileNames.m_hFile != CFile::hFileNull)
			{
				CString csOutput;
				csOutput.Format("%s\n", name);
				m_cfOutFileNames.WriteString(csOutput);
			}

			CString csFilePath = m_csArtDir + name;
			//check to see if the file exists
			if(m_bCheckArt)
			{
				if(!CheckArtFile(csFilePath))
				{
					m_errorList.Add(csFilePath);
					csFilePath += 13;
					csFilePath += 10;
					m_errorFile.Write(csFilePath, csFilePath.GetLength());
				}
			}
		}
		//if it is a group type we need to split up the group and check each individual object
		else if (pObject->type() == OBJECT_TYPE_Group)
		{
			GroupObject *pGroupObject = (GroupObject *) pObject;
			CheckObjectList(pListBox, ((GroupObject*)pObject)->object_list());
			break;
		}
	}

}


BOOL CCheckArtExistsDir::SetupProjectIterator(CString csArtSrc, CString csProjSrc)
{
	m_csArtDir = csArtSrc + '\\';
	m_csProjDir = csProjSrc;
	//set the current directory
	SetCurrentDirectory(m_csProjDir);
	m_fileFinder.FindFile("*.*");
	return TRUE;
}

CString CCheckArtExistsDir::GetNextProjName()
{
	CString csProjName;
	csProjName.Empty();
		
	while(m_fileFinder.FindNextFile())
	{
		if(!m_fileFinder.IsDirectory())
		{
			csProjName = m_fileFinder.GetFilePath();
			break;
		}
	}

	return csProjName;
}

BOOL CCheckArtExistsDir::CheckArtFile(CString csArtFilePath)
{
	CFileFind ff;
	//check to see if the file exists
	if(!ff.FindFile(csArtFilePath))
		return FALSE;

	return TRUE;
}


CString CCheckArtExistsSS::GetNextProjName()
{
	CString csProjName;
	CString csLine;
   CMlsStringArray csaItems;
	

   if(Util::ReadString(&m_cfProjFile, csLine))
	{
      // Parse the current line into fields.
      csaItems.SetNames(csLine, '\t');
      if ((csaItems.GetSize() > m_nProjDirIndex) && (csaItems.GetSize() > m_nProjFileNameIndex))
		{
         // Get the directory and file names.
         CString csDirectory = csaItems.GetAt(m_nProjDirIndex);
         CString csFilename = csaItems.GetAt(m_nProjFileNameIndex);
         if (!csDirectory.IsEmpty() && !csFilename.IsEmpty())
         {
            // Build the name of the project.
	         csProjName = m_csProjDir + csDirectory + "\\" + csFilename;
				if (!Util::FileExists(csProjName))
				{
					AfxMessageBox("Project File Missing");
					csProjName.Empty();
				}
				
			}
		}
	}

	return csProjName;
}

BOOL CCheckArtExistsSS::SetupProjectIterator(CString csArtSrc, CString csProjSrc)
{
	if(!CreateArtFileList(csArtSrc))
		return FALSE;

   CMlsStringArray csaItems;

	m_csProjSS = csProjSrc;
	m_csArtDir = csArtSrc.Left(csArtSrc.ReverseFind('\\') + 1);
	m_csProjDir = csProjSrc.Left(csProjSrc.ReverseFind('\\'));
	//set the current directory
	SetCurrentDirectory(m_csProjDir);

   if (!m_cfProjFile.Open(csProjSrc, CFile::modeRead|CFile::shareDenyNone|CFile::typeText))
	{
		AfxMessageBox("Failed to open project spreadsheet");
		return FALSE;
	}

   // Read the header and figure out the column to use for directory and file name.
	CString csLine;
	Util::ReadString(&m_cfProjFile, csLine);
   csLine.MakeUpper();
   // Parse the items.
   csaItems.SetNames(csLine, '\t');
   // Try to find the "DIRECTORY" field.
   m_nProjDirIndex = csaItems.FindName("DIRECTORY");
   if (m_nProjDirIndex == -1)
	{
		AfxMessageBox("Failed to find directory field in project spreadsheet");
		return FALSE;
	}

   // Try to find the "FILE NAME" field.
   m_nProjFileNameIndex = csaItems.FindName("FILE NAME");
   if (m_nProjFileNameIndex == -1)
	{
		AfxMessageBox("Failed to find file name field in project spreadsheet");
		return FALSE;
	}
   // Skip the next header line.
   Util::ReadString(&m_cfProjFile, csLine);

	return TRUE;
}

BOOL CCheckArtExistsSS::CreateArtFileList(CString csArtSSPath)
{
   CMlsStringArray csaItems;
	CStdioFile cfArtFileSS;
	int nArtFileNameIndex;

   if (!cfArtFileSS.Open(csArtSSPath, CFile::modeRead|CFile::shareDenyNone|CFile::typeText))
	{
		AfxMessageBox("Failed to open art spreadsheet");
		return FALSE;
	}

   // Read the header and figure out the column to use for file name.
	CString csLine;
	Util::ReadString(&cfArtFileSS, csLine);
   csLine.MakeUpper();
   // Parse the items.
   csaItems.SetNames(csLine, '\t');

   // Try to find the "FILE NAME" field.
   nArtFileNameIndex = csaItems.FindName("FILE NAME");
   if (nArtFileNameIndex == -1)
	{
		AfxMessageBox("Failed to find file name field in art spreadsheet");
		return FALSE;
	}
   // Skip the next header line.
   Util::ReadString(&cfArtFileSS, csLine);

	//loop through remaining lines of art spreadsheet and extract the art filenames
	CString csArtFileName;
	//value for mapping
	DWORD mapValue = 0;
	while(Util::ReadString(&cfArtFileSS, csLine))
	{
      // Parse the current line into fields.
      csaItems.SetNames(csLine, '\t');
      if (csaItems.GetSize() > nArtFileNameIndex)
		{
			//extract filename
			csArtFileName = csaItems.GetAt(nArtFileNameIndex);
			csArtFileName.MakeUpper();
			//add string to map
			m_StringMap.SetAt(csArtFileName, (void*)mapValue);
			mapValue++;
		}
	}
	return TRUE;
}

BOOL CCheckArtExistsSS::CheckArtFile(CString csArtFilePath)
{
	CString csArtFileName = csArtFilePath.Right(csArtFilePath.GetLength() - csArtFilePath.ReverseFind('\\') - 1);
	csArtFileName.MakeUpper();
	void* p;
	if(!m_StringMap.Lookup(csArtFileName, p))
		return FALSE;
	
	return TRUE;
}
