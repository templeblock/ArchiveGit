//////////////////////////////////////////////////////////////////////////////
// $DBBld: /PMW/DBBLD.CPP 2     5/31/98 5:19p Hforman $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/dbbld.cpp $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 7     2/25/99 3:43p Lwu
// added selected glow state to buttons
// 
// 6     2/17/99 4:27p Lwu
// Added a few columns to handle btnType and other control types
// 
// 5     2/16/99 5:28p Mwilson
// added tile flag
// 
// 4     2/10/99 11:33a Mwilson
// added ctrl id
// 
// 3     2/05/99 11:29a Mwilson
// added classes for new collection handling for CreataCard
// 
// 2     1/28/99 4:47p Mwilson
// added special colleciton builders
// 
// 1     1/27/99 3:41p Mwilson
// 
// 2     5/31/98 5:19p Hforman
// 
//    Rev 1.0   14 Aug 1997 15:21:16   Fred
// Initial revision.
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbbld.h"
#include "resource.h"
#include "progbar.h"    // For progress bar

static const char * s_szPerfectBuild = "Perfect Build! Congratulations!\n";


//
//		class CPCompDBCollBuilder
////////////////////////////////////////////////////////
CPCompDBCollBuilder::CPCompDBCollBuilder(const CString &csSpreadSheetFilePath,
													const CString &csCollectionPath,
													const CString &csCollectionName):
	CPBaseCollBuilder(csSpreadSheetFilePath, csCollectionPath), 
	m_componentDB(&m_cindexFile,&m_storageFile)
													
{
	ASSERT(!csCollectionName.IsEmpty());
	m_csCollectionName = csCollectionName;
	m_pcsaHeaderFields = new CPCompDBHeaderArray[NUM_SPREADSHEET_HEADER_LINES];
}

CPCompDBCollBuilder::~CPCompDBCollBuilder()
{

}

int CPCompDBCollBuilder::DoBuild()
{
   MSG                  msg;
   CPMWProgressBar      progressDialog;
	int nNonEmptyLineCount;
	int nRetVal;
	
	TRY
	{
      nRetVal = OpenAll();
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

		m_trFileSpreadSheet.set_read_buffer(4096);

		nNonEmptyLineCount = GetSpreadSheetLineCount();
		if(nNonEmptyLineCount == 0)
		{
			AfxMessageBox("Error opening spreadsheet");
			return ERRORCODE_Open;
		}
		if(ReadHeaderLines() != ERRORCODE_None)
		{
			CloseAll();
			AfxMessageBox("Error occurred while reading spreadsheet header\nCheck log file");
			AfxGetMainWnd()->EnableWindow(TRUE);
			return ERRORCODE_Fail;
		}
      // Create Progress Bar control
      if(!progressDialog.Create(IDD_PROGRESS_BAR, AfxGetMainWnd()))
      {
         AfxMessageBox("Failed to Create Progress Bar");
         return ERRORCODE_Fail;
      }
      AfxGetMainWnd()->EnableWindow(FALSE);
      progressDialog.EnableWindow(TRUE);     // Re-enable progress dialog
      progressDialog.SetWindowText("Building Collection...");

      CProgressCtrl *pProgressBar = (CProgressCtrl *)progressDialog.GetDlgItem(IDC_PROGRESS_CONTROL);
      pProgressBar->SetRange(1, nNonEmptyLineCount);
      pProgressBar->SetStep(1);
		// Process lines until end of file or serious error
		do
		{
			TRACE("Processing Line %05i: \n",m_nCurLine);

			nRetVal = ProcessLine();
			m_nCurLine++;
			// Empty spreadsheet lines are not reflected in progress bar
			if(!m_bLineIsEmpty)
				pProgressBar->StepIt();

			if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
				// Pump the message through.
				AfxGetApp()->PumpMessage();
		}
		while (nRetVal == ERRORCODE_None && !progressDialog.CancelHit());
	}
	CATCH_ALL(exception)
	{
		CString  strError;
		strError.Format("Exception occurred in builder at Line %05i: Build Aborted",
			m_nCurLine-1);
		AfxMessageBox(strError);
	}
	END_CATCH_ALL;
   
	if(m_BuildStats.GetErrorCount() == 0 &&
      m_BuildStats.GetWarningCount() == 0 &&
      m_BuildStats.GetNoneCount() > 0)
         m_stdioLogFile.WriteString(s_szPerfectBuild);
   // Closes/cleanup
   CloseAll();
 
	AfxGetMainWnd()->EnableWindow(TRUE);
   return nRetVal;

}

int CPCompDBCollBuilder::OpenAll()
{
	int nRetVal;
	CString csIndexFilePath;
	CString csDataFileName;

	Util::ConstructPath(csIndexFilePath, m_strCollectionPath, (m_csCollectionName + ".IDX"));
	Util::ConstructPath(csDataFileName, m_strCollectionPath, (m_csCollectionName + ".DB"));
	TRY
	{
		nRetVal = CPBaseCollBuilder::OpenAll();
      if(nRetVal != ERRORCODE_None)
         AfxThrowUserException();

		if(Util::FileExists(csIndexFilePath))
			CFile::Remove(csIndexFilePath);

		nRetVal = m_cindexFile.Open( csIndexFilePath, &m_cindexList, CCIndexFile::modeCreate);
		if(nRetVal != CCIndexFile::success)
		{
		  AfxThrowUserException();
		}
		// Create User Interface Component Database
		m_storageFile.set_name(csDataFileName);
		m_storageFile.initialize();
	}
	CATCH_ALL(e)
	{
		ReportError("Failed to open collection file", CCollectionBuilderStats::typeError);
		return nRetVal;
	}
	END_CATCH_ALL;

	return nRetVal;
}

int CPCompDBCollBuilder::CloseAll()
{
	CPBaseCollBuilder::CloseAll();
	m_cindexFile.Close();

   return ERRORCODE_None;
}

int CPCompDBCollBuilder::ProcessLine()
{
   int      nRetVal;
   CString  strError;
   m_bLineIsEmpty = FALSE;
	CString csFileName;
	CFile	cfileInput;
	DWORD dwFileLength;
	CPUIImageComponentDirEntry		componentDirEntry;
	CPUIImageComponentDataEntry		componentDataEntry;
	CPUIImageInfo	componentHeader;

	if(CPBaseCollBuilder::ProcessLine() != ERRORCODE_None )
		return ERRORCODE_Done;


	if(ValidateFields() != ERRORCODE_None)
      return ERRORCODE_None;   // Lie!, log file will contain errors

	// Add 1st Image to database
	cfileInput.Open(m_strFullDataFileName,CFile::modeRead);
	dwFileLength = cfileInput.GetLength();

	// Tell Component Data Object to allocate memory equal to image size
	componentDataEntry.AllocData(dwFileLength);
	// Read image file directly into Component Data Object
	cfileInput.ReadHuge(componentDataEntry.GetData(),dwFileLength);
	// Prepare to add Image to Image Component files
	// 1st set key to Image Filename
	componentDirEntry.SetKey(m_strFileName);
	// Fill in Image Header with Image Filelength and 
	// Screen Resolution info. that Image was designed for
	componentHeader.SetSize(dwFileLength);
	componentHeader.SetType(m_nType);
	componentHeader.SetResolution(m_csizeResolution);
	// Send header info. to Component Data Object
	componentDataEntry.SetHeader(componentHeader);
	// Add Image Component To Database
	nRetVal = m_componentDB.Add(&componentDirEntry, &componentDataEntry);
	if(nRetVal != ERRORCODE_None)
		AfxMessageBox("Error Adding Content");
	// Done with Source Image File
	cfileInput.Close();

   return nRetVal;
}


int CPCompDBCollBuilder::ValidateFields()
{
   int      nStartColumn, nEndColumn;
   CString  strError;
   CString  strCategoryField, strField;

   m_nType = CPUIImageInfo::typePNG;
   m_bValidationError=FALSE;
   for(CPCompDBHeaderArray::FieldTypes curField=CPCompDBHeaderArray::TYPE;
      curField < CPCompDBHeaderArray::ListEnd;
      curField++)
   {
      nStartColumn = m_pcsaHeaderFields[0].GetStartColumn(curField);
		if (nStartColumn == -1)
			continue;				// Optional field not found.
      nEndColumn   = m_pcsaHeaderFields[0].GetEndColumn(curField);

      ASSERT(nStartColumn < m_csaFields.GetSize());
      ASSERT(nEndColumn < m_csaFields.GetSize());
      if(nStartColumn >= m_csaFields.GetSize() || nEndColumn >= m_csaFields.GetSize())
      {
         AfxMessageBox("Internal Error");
         return ERRORCODE_IntError;
      }

      // Skip check for empty Tone Thumbnail, and Custom Flags fields
      // Tone, Custom Flags, and Product are checked when they are 
      // converted to values
      // All others should not be empty.
      if(m_csaFields[nStartColumn].IsEmpty() &&
         curField == CPCompDBHeaderArray::FILENAME)
      {
         strError.Empty();
         strError.Format("Line %05i: Empty Field %s\n",
            m_nCurLine, m_pcsaHeaderFields[0].GetName(curField));
         ReportError(strError);
         m_bValidationError = TRUE;
      }
      // Special case checks
      switch(curField)
      {
         case CPCompDBHeaderArray::TYPE:
            m_nType = GetImageTypeFromFlag(nStartColumn, nEndColumn);
            break;
         case CPCompDBHeaderArray::RESOLUTION:
				GetResolution(nStartColumn, nEndColumn);
            break;
         case CPCompDBHeaderArray::DIRECTORY:
            m_strFullDataFileName += m_csaFields[nStartColumn];
            m_strFullDataFileName += "\\";
            break;
         case CPCompDBHeaderArray::FILENAME:
				m_strFileName = m_csaFields[nStartColumn];
            m_strFullDataFileName += m_csaFields[nStartColumn];
            break;
      }
   }
	if(!Util::FileExists(m_strFullDataFileName))
   {
      m_bValidationError = TRUE;
      strError.Empty();
      strError.Format("Line %05i: File %s missing\n", m_nCurLine, m_strFullDataFileName);
      ReportError(strError);
   }
   if(m_bValidationError)
         return ERRORCODE_Fail;    // Warnings/Errors in LOG File

   return ERRORCODE_None;      
}

void CPCompDBCollBuilder::GetResolution(int nStartColumn, int nEndColumn)
{
	CString strError;
	CString csTemp = m_csaFields[nStartColumn];
	if(csTemp.IsEmpty())
	{
      strError.Empty();
      strError.Format("Line %05i: Empty Field %s\n",
         m_nCurLine, m_pcsaHeaderFields[0].GetName(nStartColumn));
      ReportError(strError);
      m_bValidationError = TRUE;
	}
	m_csizeResolution.cx = atoi(csTemp);

	csTemp = m_csaFields[nEndColumn];
	if(csTemp.IsEmpty())
	{
      strError.Empty();
      strError.Format("Line %05i: Empty Field %s\n",
         m_nCurLine, m_pcsaHeaderFields[0].GetName(nStartColumn));
      ReportError(strError);
      m_bValidationError = TRUE;
	}
	m_csizeResolution.cy = atoi(csTemp);
}

CPUIImageInfo::ImageType CPCompDBCollBuilder::GetImageTypeFromFlag(int nStartColumn, int nEndColumn)
{
   BOOL     bSet;
	CPUIImageInfo::ImageType nImageType = CPUIImageInfo::typePNG;

   for(int nCurField = nStartColumn;nCurField <= nEndColumn;nCurField++)
   {
      bSet = atoi(m_csaFields.GetAt(nCurField));
      if(bSet)
      {
			break;
      }
		nImageType++;
   }
	return nImageType;
}



//
//		class CPCompPageCollBuilder
////////////////////////////////////////////////////////
CPCompPageCollBuilder::CPCompPageCollBuilder(const CString &csSpreadSheetFilePath,
													const CString &csCollectionPath,
													const CString &csCollectionName):
	CPBaseCollBuilder(csSpreadSheetFilePath, csCollectionPath), 
	m_componentDB(&m_cindexFile,&m_storageFile),
	m_bNewPage(FALSE),
	m_bFirstPage(TRUE),
	m_nID(100)
													
{
	ASSERT(!csCollectionName.IsEmpty());
	m_csCollectionName = csCollectionName;
	m_pcsaHeaderFields = new CPCompPageHeaderArray[NUM_SPREADSHEET_HEADER_LINES];
}

CPCompPageCollBuilder::~CPCompPageCollBuilder()
{
}

int CPCompPageCollBuilder::DoBuild()
{
   MSG                  msg;
   CPMWProgressBar      progressDialog;
	int nNonEmptyLineCount;
	int nRetVal;
	
	TRY
	{
      nRetVal = OpenAll();
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

		m_trFileSpreadSheet.set_read_buffer(4096);

		nNonEmptyLineCount = GetSpreadSheetLineCount();
		if(nNonEmptyLineCount == 0)
		{
			AfxMessageBox("Error opening spreadsheet");
			return ERRORCODE_Open;
		}
		if(ReadHeaderLines() != ERRORCODE_None)
		{
			CloseAll();
			AfxMessageBox("Error occurred while reading spreadsheet header\nCheck log file");
			AfxGetMainWnd()->EnableWindow(TRUE);
			return ERRORCODE_Fail;
		}
      // Create Progress Bar control
      if(!progressDialog.Create(IDD_PROGRESS_BAR, AfxGetMainWnd()))
      {
         AfxMessageBox("Failed to Create Progress Bar");
         return ERRORCODE_Fail;
      }
      AfxGetMainWnd()->EnableWindow(FALSE);
      progressDialog.EnableWindow(TRUE);     // Re-enable progress dialog
      progressDialog.SetWindowText("Building Collection...");

      CProgressCtrl *pProgressBar = (CProgressCtrl *)progressDialog.GetDlgItem(IDC_PROGRESS_CONTROL);
      pProgressBar->SetRange(1, nNonEmptyLineCount);
      pProgressBar->SetStep(1);
		// Process lines until end of file or serious error
		do
		{
			TRACE("Processing Line %05i: \n",m_nCurLine);

			nRetVal = ProcessLine();
			m_nCurLine++;
			// Empty spreadsheet lines are not reflected in progress bar
			if(!m_bLineIsEmpty)
				pProgressBar->StepIt();

			if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
				// Pump the message through.
				AfxGetApp()->PumpMessage();
		}
		while (nRetVal == ERRORCODE_None && !progressDialog.CancelHit());
		//add the last page;
		AddItem();
	}
	CATCH_ALL(exception)
	{
		CString  strError;
		strError.Format("Exception occurred in builder at Line %05i: Build Aborted",
			m_nCurLine-1);
		AfxMessageBox(strError);
	}
	END_CATCH_ALL;
   
	if(m_BuildStats.GetErrorCount() == 0 &&
      m_BuildStats.GetWarningCount() == 0 &&
      m_BuildStats.GetNoneCount() > 0)
         m_stdioLogFile.WriteString(s_szPerfectBuild);
   // Closes/cleanup
   CloseAll();
 
	AfxGetMainWnd()->EnableWindow(TRUE);
   return nRetVal;

}

int CPCompPageCollBuilder::OpenAll()
{
	int nRetVal;
	CString csIndexFilePath;
	CString csDataFileName;
	CString csIDFileName;

	Util::ConstructPath(csIndexFilePath, m_strCollectionPath, (m_csCollectionName + ".IDX"));
	Util::ConstructPath(csDataFileName, m_strCollectionPath, (m_csCollectionName + ".DB"));
	Util::ConstructPath(csIDFileName, m_strCollectionPath, "CtrlIDs.h");
	TRY
	{
		nRetVal = CPBaseCollBuilder::OpenAll();
      if(nRetVal != ERRORCODE_None)
         AfxThrowUserException();

		if(Util::FileExists(csIndexFilePath))
			CFile::Remove(csIndexFilePath);

		nRetVal = m_cindexFile.Open( csIndexFilePath, &m_cindexList, CCIndexFile::modeCreate);
		if(nRetVal != CCIndexFile::success)
		{
		  AfxThrowUserException();
		}
		// Create User Interface Component Database
		m_storageFile.set_name(csDataFileName);
		m_storageFile.initialize();

		if(!m_fileIDs.Open(csIDFileName, CFile::modeCreate | CFile::modeWrite))
			AfxThrowUserException();

		m_fileIDs.WriteString("#ifndef _CTRL_IDS\n#define _CTRL_IDS\n////Control IDs for custom dialogs\n\n");

	}
	CATCH_ALL(e)
	{
		ReportError("Failed to open collection file", CCollectionBuilderStats::typeError);
		return nRetVal;
	}
	END_CATCH_ALL;

	return nRetVal;
}

int CPCompPageCollBuilder::CloseAll()
{
	CPBaseCollBuilder::CloseAll();
	m_cindexFile.Close();
	m_fileIDs.WriteString("\n\n#endif");
	m_fileIDs.Close();

   return ERRORCODE_None;
}

int CPCompPageCollBuilder::ProcessLine()
{
   CString  strError;
   m_bLineIsEmpty = FALSE;

	if(CPBaseCollBuilder::ProcessLine() != ERRORCODE_None )
		return ERRORCODE_Done;


	if(ValidateFields() != ERRORCODE_None)
      return ERRORCODE_None;   // Lie!, log file will contain errors


   return ERRORCODE_None;
}


int CPCompPageCollBuilder::ValidateFields()
{
   int				nStartColumn, nEndColumn;
   CString			strError;
   CString			strCategoryField, strField;
	CString			csPageName;
	CString			csControlName;
	BOOL				bNewPage = FALSE;
	CPControlInfo*	pTempInfo = new CPControlInfo;

	pTempInfo->nSize = sizeof(CPControlInfo);

   m_bValidationError = FALSE;


   for(CPCompPageHeaderArray::FieldTypes curField=CPCompPageHeaderArray::PAGENAME;
      curField < CPCompPageHeaderArray::ListEnd;
      curField++)
   {
      nStartColumn = m_pcsaHeaderFields[0].GetStartColumn(curField);
		if (nStartColumn == -1)
			continue;				// Optional field not found.
      nEndColumn   = m_pcsaHeaderFields[0].GetEndColumn(curField);

      ASSERT(nStartColumn < m_csaFields.GetSize());
      ASSERT(nEndColumn < m_csaFields.GetSize());
      if(nStartColumn >= m_csaFields.GetSize() || nEndColumn >= m_csaFields.GetSize())
      {
         AfxMessageBox("Internal Error");
         return ERRORCODE_IntError;
      }
		//save all the info in the temp variable tempInfo;
      switch(curField)
      {
         case CPCompPageHeaderArray::PAGENAME:
			{
				if(!m_csaFields[nStartColumn].IsEmpty())
				{
					bNewPage = TRUE;
					csPageName = m_csaFields[nStartColumn];
				}
            break;
			}
         case CPCompPageHeaderArray::CONTROLNAME:
			{
				if(!m_csaFields[nStartColumn].IsEmpty())
				{
					csControlName = m_csaFields[nStartColumn];
				}
            break;
			}
         case CPCompPageHeaderArray::CONTROL:
			{
				pTempInfo->nControlType = GetControlTypeFromFlag(nStartColumn, nEndColumn);
            break;
			}
         case CPCompPageHeaderArray::BTNTYPE:
			{
				if(!m_csaFields[nStartColumn].IsEmpty())
				pTempInfo->nBtnType = atoi(m_csaFields[nStartColumn]);
            break;
			}
         case CPCompPageHeaderArray::POSITION:
			{
				if(!m_csaFields[nStartColumn].IsEmpty())
					pTempInfo->cpPosition.x = atoi(m_csaFields[nStartColumn]);
				if(!m_csaFields[nEndColumn].IsEmpty())
					pTempInfo->cpPosition.y = atoi(m_csaFields[nEndColumn]);
            break;
			}
         case CPCompPageHeaderArray::SIZE:
			{
				if(!m_csaFields[nStartColumn].IsEmpty())
					pTempInfo->czSize.cx = atoi(m_csaFields[nStartColumn]);
				if(!m_csaFields[nEndColumn].IsEmpty())
					pTempInfo->czSize.cy = atoi(m_csaFields[nEndColumn]);
            break;
			}
         case CPCompPageHeaderArray::IMAGEFILE:
			{
				int i = 0;
				pTempInfo->csBmpUp = m_csaFields[nStartColumn + i++];
				pTempInfo->csBmpDown = m_csaFields[nStartColumn + i++];
				pTempInfo->csBmpGlow = m_csaFields[nStartColumn + i++];
				pTempInfo->csBmpSelGlow = m_csaFields[nStartColumn + i++];;
				pTempInfo->csBmpDisabled = m_csaFields[nStartColumn + i++];
				pTempInfo->csBmpDefault = m_csaFields[nEndColumn];
				break;
			}
         case CPCompPageHeaderArray::TILED:
				if(!m_csaFields[nStartColumn].IsEmpty())
					pTempInfo->bTiled = atoi(m_csaFields[nStartColumn]);
            break;
         case CPCompPageHeaderArray::CAPTION:
				pTempInfo->csCaption = m_csaFields[nStartColumn];
            break;
         case CPCompPageHeaderArray::TOOLTIP:
				pTempInfo->csTooltip = m_csaFields[nStartColumn];
            break;

         case CPCompPageHeaderArray::FONTNAME:
				pTempInfo->csFontName = m_csaFields[nStartColumn];
            break;
         case CPCompPageHeaderArray::FONTSIZE:
			{
				if(!m_csaFields[nStartColumn].IsEmpty())
					pTempInfo->nFontSize = atoi(m_csaFields[nStartColumn]);
            break;
			}
      }
   }
	if(bNewPage)
	{
		//if this is not the first page then we want to write out the data
		if(!m_bFirstPage)
		{
			AddItem();
			m_compPage.Reset();
		}
		m_bFirstPage = FALSE;
		//this is the start of a new page so lets set the base information
		ASSERT(!csPageName.IsEmpty());
		m_compPage.SetPageName(csPageName);
		//set id to 0  We don't need id for page
		pTempInfo->nCtrlID = 0;
		m_compPage.AddCtrlInfo(pTempInfo);
	}
	else
	{
		//Set the ctrl ID.  AddCtrlID increments m_nID so we always have
		//unique ID.
		pTempInfo->nCtrlID = m_nID;
		//this is just a control on the page so add it to the list
		m_compPage.AddCtrlInfo(pTempInfo);
		AddCtrlID(csControlName);
	}

   if(m_bValidationError)
         return ERRORCODE_Fail;    // Warnings/Errors in LOG File

   return ERRORCODE_None;      
}

void CPCompPageCollBuilder::AddCtrlID(CString& csCtrlName)
{
	CString csPageName = m_compPage.GetPageName();
	csPageName.MakeUpper();
	csCtrlName.MakeUpper();
	CString csID;
	csID.Format("IDC_%s_%s", csPageName, csCtrlName);
	
	CString csOut;
	csOut.Format("#define %35s\t\t%d\n",csID , m_nID++);
	m_fileIDs.WriteString(csOut);

}

CPCompPageCollBuilder::ControlType CPCompPageCollBuilder::GetControlTypeFromFlag(int nStartColumn, int nEndColumn)
{
   BOOL     bSet;
	CPCompPageCollBuilder::ControlType nControlType = CPCompPageCollBuilder::BKGND;

   for(int nCurField = nStartColumn;nCurField <= nEndColumn;nCurField++, nControlType++)
   {
      bSet = atoi(m_csaFields.GetAt(nCurField));
      if(bSet)
      {
			break;
      }
   }
	return nControlType;
}

int CPCompPageCollBuilder::AddItem()
{
	DWORD dwFileLength = 0;
	int nRetVal = ERRORCODE_None;

	CPUIImageComponentDirEntry		componentDirEntry;
	CPUIPageComponentDataEntry		componentDataEntry;

	CMemFile mf;
	CArchive ar(&mf, CArchive::store);
	
	m_compPage.Serialize(ar);
	ar.Close();
	
	mf.SeekToBegin();
	// Add 1st Image to database
	dwFileLength = mf.GetLength();

	// Tell Component Data Object to allocate memory equal to image size
	componentDataEntry.AllocData(dwFileLength);
	// Read image file directly into Component Data Object
	mf.Read(componentDataEntry.GetData(),dwFileLength);
	// Prepare to add Image to Image Component files
	// 1st set key to Image Filename
	componentDirEntry.SetKey(m_compPage.GetPageName());
	componentDataEntry.SetHeader(dwFileLength);
	// Add Image Component To Database
	nRetVal = m_componentDB.Add(&componentDirEntry, &componentDataEntry);
	if(nRetVal != ERRORCODE_None)
		AfxMessageBox("Error Adding Content");
	// Done with Source Image File
	mf.Close();

	return nRetVal;
}

//
//		class CPCompPage
////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( CPCompPage, CObject, 1 )

CPCompPage::CPCompPage()
{

}

CPCompPage::~CPCompPage()
{
	while(!m_listCtrls.IsEmpty())
		delete m_listCtrls.RemoveHead();

}

void CPCompPage::Reset()
{
	m_csPageName.Empty();

	while(!m_listCtrls.IsEmpty())
		delete m_listCtrls.RemoveHead();
}

void CPCompPage::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << m_csPageName;
	}
	else
	{
		ar >> m_csPageName;
	}
	m_listCtrls.Serialize(ar);
}

//
//		class CPControlInfo
////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( CPControlInfo, CObject, 1 )

CPControlInfo::CPControlInfo():
	nSize(0),
	nControlType(0),
	nBtnType(0),
	bTiled(FALSE),
	nFontSize(0),
	nCtrlID(0)

{
}

void CPControlInfo::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << nSize;
		ar << nControlType;
		ar << nBtnType;
		ar << cpPosition;
		ar << czSize;
		ar << csBmpUp;
		ar << csBmpDown;		
		ar << csBmpGlow;
		ar	<< csBmpSelGlow;
		ar << csBmpDisabled;		
		ar << csBmpDefault;
		ar << csCaption;
		ar << csTooltip;		
		ar << csFontName;
		ar << bTiled;		
		ar << nFontSize;		
		ar << nCtrlID;		
	}
	else
	{
		ar >> nSize;
		ar >> nControlType;
		ar >> nBtnType;
		ar >> cpPosition;
		ar >> czSize;
		ar >> csBmpUp;
		ar >> csBmpDown;		
		ar >> csBmpGlow;
		ar	>> csBmpSelGlow;
		ar >> csBmpDisabled;		
		ar >> csBmpDefault;
		ar >> csCaption;
		ar >> csTooltip;		
		ar >> csFontName;
		ar >> bTiled;		
		ar >> nFontSize;		
		ar >> nCtrlID;		
	}
}
