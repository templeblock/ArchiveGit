#include "stdafx.h"
#include "Ctp.h"
#include "AGPage.h"
#include "AGLayer.h"
#include "AGSym.h"
#include "AGDC.h"
#include "AGMatrix.h"
#include "AGText.h"
#include "NewDocWiz.h"
#include "HpAutoUpdate.h"
#include "EMDlg.h"
#include "PngToDib.h"
#include "Image.h"
#include "ConvertDlg.h"
#include "PropertiesDlg.h"
#include "Dib.h"
#include "version.h"
#include "HTMLDialog.h"
#include "Shape.h"
#include "RegKeys.h"
#include "atlenc.h"
#include "Select.h"
#include <windef.h> // for min and max
#include <fstream>
#include <Commdlg.h>
#include "Gif.h"
#ifdef FYS
	#include "FYSPrintDoc.h"
#endif FYS

extern int TIMER_BlinkCaret;

const int WORKSPACELEFT_OFFSET = 190;
const int WORKSPACETOP_OFFSET = 21;

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocNew(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CNewDocWizIntro Intro;
	CNewDocWizStep1 Step1; 
	CNewDocWizStep2 Step2;
	CNewDocWizEnd End;

	PROPSHEETPAGE* pPropPages = new PROPSHEETPAGE[4];
	pPropPages[0] = Intro.m_psp;
	pPropPages[1] = Step1.m_psp;
	pPropPages[2] = Step2.m_psp;
	pPropPages[3] = End.m_psp;

	PROPSHEETHEADER psh;
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_WIZARD | PSH_PROPSHEETPAGE;
	psh.hwndParent = GetParent();
	psh.hInstance = NULL;
	psh.hIcon = NULL;
	psh.pszCaption = NULL;
	psh.nPages = 4;
	psh.nStartPage = 0;
	psh.ppsp = pPropPages;
	psh.pfnCallback = NULL;

	::PropertySheet(&psh);
	delete [] pPropPages;

	CNewDocData* pNewDocData = Intro.GetFinishedData();
	if (!pNewDocData)
		return S_OK;

	if (m_pAGDoc)
		Close(false/*bExit*/);

	if (m_pAGDoc)
		return S_OK;

	// Redraw the window underneath, before we get to work
	::UpdateWindow(m_pCtp->GetBrowser());

	CWaitCursor Wait;

	if (!(m_pAGDoc = new CAGDoc()))
		return E_FAIL;

	int nPages = pNewDocData->m_iNumPages;
	int nLayers = 2;
	int nWidth = INCHES(pNewDocData->m_fWidth);
	int nHeight = INCHES(pNewDocData->m_fHeight);

	m_pAGDoc->SetPortrait(nWidth <= nHeight);
	m_pAGDoc->SetDocType(pNewDocData->m_DocType);
	m_pAGDoc->SetFileName(CString("Untitled"));
	m_pCtp->PutSrcURL(CComBSTR("Untitled"));

	for (int i = 0; i < nPages; i++)
	{
		CAGPage* pPage = new CAGPage(nWidth, nHeight);
		if (!pPage)
			continue;

		pPage->SetPageName(pNewDocData->m_strPageNames[i]);
		m_pAGDoc->AddPage(pPage);

		for (int j = 0; j < nLayers; j++)
			pPage->AddLayer(new CAGLayer());
	}

	m_pAGDoc->SetCurrentPageNum(0);
	m_pAGDoc->SetModified(false);

	m_pCtp->CreateBackPage();
	m_pCtp->ActivateBrowser();
	m_pCtp->ActivateNewDoc();

    CleanupLinkedDoc();
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocEnvelopeWiz(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	UINT iTabID = IDD_EMADDRESS_TAB;
	if (-1 == (int)hWnd)
		iTabID = IDD_EMDESIGN_TAB;

	return LaunchEMDlg(m_pAGDoc->GetDocType(), false/*bAtPrintTime*/, iTabID);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocMatchCard(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

    if (!m_pAGDoc)
		return E_FAIL;

     if (!SwitchToCard())
        return E_FAIL;

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::LaunchEMDlg(AGDOCTYPE DocType, bool bAtPrintTime, UINT iTabID)
{
	CWaitCursor Wait;

    if (!m_pAGDoc)
		return E_FAIL;

    if (!SwitchToEnvelope(bAtPrintTime, iTabID))
        return E_FAIL;
        
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnFileRecent(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	// Get file name from the MRU list
	TCHAR szFilename[MAX_PATH];
	if(m_mruList.GetFromList(id, szFilename))
	{
		// open file
		OpenDoc2(szFilename);

		if (m_pAGDoc)
            m_mruList.MoveToTop(id);
        else
			m_mruList.RemoveFromList(id);
		
		m_mruList.WriteToRegistry(REGKEY_APP);
	}
	else
	{
		GetError("OnFileRecent");
	}

    return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocOpen(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CString strFileName = GetDocFileName();
	if (strFileName.IsEmpty())
		return E_FAIL;

    CleanupLinkedDoc();

	return OpenDoc(strFileName);
}

/////////////////////////////////////////////////////////////////////////////
CString CDocWindow::GetDocFileName()
{
	CString strPath;

	// Get the path from the registry
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		char szFilePath[MAX_PATH];
		szFilePath[0] = 0;
		DWORD nChars = MAX_PATH;
		bool bOK = (regkey.QueryStringValue(REGVAL_MRU_CTP_PATH, szFilePath, &nChars) == ERROR_SUCCESS);
		if (bOK)
		{
			szFilePath[nChars] = 0;
			strPath = szFilePath;
		}
	}

	// If there is no path in the registry, use the default
	if (strPath.IsEmpty())
	{
		bool bOK = GetSpecialFolderLocation(strPath, CSIDL_COMMON_DESKTOPDIRECTORY);
		if (!bOK)
			bOK = GetSpecialFolderLocation(strPath, CSIDL_DESKTOPDIRECTORY);
	}

	char szFileName[MAX_PATH];
	szFileName[0] = 0;

	OPENFILENAMEX ofx;
	::ZeroMemory(&ofx, sizeof(ofx));
	ofx.of.lStructSize = (IsEnhancedWinVersion() ? sizeof(ofx) : sizeof(ofx)-12);
	ofx.of.hwndOwner = (m_pCtp ? m_pCtp->GetBrowser() : NULL);
	ofx.of.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK;
	ofx.of.lpstrInitialDir = strPath;
	ofx.of.lpstrFile = szFileName;
	ofx.of.lpfnHook = CDocWindow::OpenFileHookProc;
	ofx.of.nMaxFile = MAX_PATH;
	ofx.of.lpstrFilter = "Project Files (*.ctp)\0*.*ctp\0\0";
	CString strTitle = "Open a saved " + g_szAppName + " project";
	ofx.of.lpstrTitle = strTitle;
	ofx.of.lpstrDefExt = "ctp";

	if (!::GetOpenFileName(&ofx.of))
		return "";

	// Save the path in the registry
	if (regkey.m_hKey)
		bool bOK = (regkey.SetStringValue(REGVAL_MRU_CTP_PATH, StrPath(szFileName)) == ERROR_SUCCESS);

	return CString(szFileName);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OpenDoc(LPCTSTR szFileName)
{
	if (m_pAGDoc)
		Close(false/*bExit*/);

	if (m_pAGDoc)
		return S_OK;

	// Redraw the window underneath, before we get to work
	::UpdateWindow(m_pCtp->GetBrowser());

	CWaitCursor Wait;

#ifndef NETSCAPE
	m_pCtp->PutSrc(CComBSTR(szFileName));
#else
	if (!(m_pAGDoc = new CAGDoc()))
        return E_FAIL;

	ifstream input(szFileName, ios::in | /*ios::nocreate |*/ ios::binary);

	bool bAdjusted = false;
	if (!m_pAGDoc->Read(input, bAdjusted))
	{
		delete m_pAGDoc;
		m_pAGDoc = NULL;
		CMessageBox::Message(String("Error reading from file '%s'.", szFileName));
		return E_FAIL;
	}		

	m_pAGDoc->SetFileName(CString(szFileName));
	m_pCtp->PutSrcURL(CComBSTR(szFileName));

	m_pCtp->CreateBackPage();
	m_pCtp->ActivateNewDoc();
#endif NETSCAPE
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OpenDoc2(LPCTSTR szFileName)
{
	if (m_pAGDoc)
		 Close(false/*bExit*/, true /*bSilent*/);

	if (m_pAGDoc)
		return S_OK;

	// Redraw the window underneath, before we get to work
	::UpdateWindow(m_pCtp->GetBrowser());

	CWaitCursor Wait;

	if (!(m_pAGDoc = new CAGDoc()))
        return E_FAIL;

	ifstream input(szFileName, ios::in | /*ios::nocreate |*/ ios::binary);

	bool bAdjusted = false;
	if (!m_pAGDoc->Read(input, bAdjusted))
	{
		delete m_pAGDoc;
		m_pAGDoc = NULL;
		CMessageBox::Message(String("Error reading from file '%s'.", szFileName));
		return E_FAIL;
	}		

	m_pAGDoc->SetFileName(CString(szFileName));
	m_pCtp->PutSrcURL(CComBSTR(szFileName));

	m_pCtp->CreateBackPage();
	m_pCtp->ActivateNewDoc();
	
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocClose(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	LRESULT lResult = Close(false/*bExit*/);
	if (lResult == S_FALSE) // The user canceled
		return S_OK;
		
	if (m_pCtp && !SpecialsEnabled())
	{
		HWND hWnd = m_pCtp->GetBrowser();
		if (hWnd)
			::PostMessage(hWnd, WM_CLOSE, 0, 0);
	}

	return lResult;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::Close(bool bExit, bool bSilent)
{
	if (!m_pAGDoc)
		return E_FAIL;

	if (m_pEMDlg)
	{
		if (bSilent)
        {
            m_pEMDlg->ShowWindow(SW_HIDE);
        }
        else
        {
            if (::IsWindow(m_pEMDlg->m_hWnd))
			    m_pEMDlg->DestroyWindow();
		    delete m_pEMDlg;
		    m_pEMDlg = NULL;
        }
	}

#ifdef FYS
	if (m_FYSRemoteDlg.m_hWnd && ::IsWindow(m_FYSRemoteDlg.m_hWnd))
		m_FYSRemoteDlg.SendMessage(WM_CLOSE);
#endif FYS

	// Unselect the current symbol, because all symbols are going away
	// and to ensure that the document modifed flag is up to date
	SymbolUnselect(true/*bClearPointer*/);

    m_Undo.Cleanup();

//j	if (m_Undo.CanUndo()) // If there have been any edits made, prompt to save the document
    // If there have been any edits made, prompt to save the document
	if (m_pAGDoc->IsModified() && !bSilent)
	{  
		CString strFullDocPath;
		CString strPath;
		CString strFileName;
		m_pAGDoc->GetFileName(strFullDocPath, strPath, strFileName);
        
        AGDOCTYPE DocType = m_pAGDoc->GetDocType();
        CString szMsg  = String("The contents of '%s' has changed.\n\nDo you want to save the changes?\n", strFileName);
        if (DOC_ENVELOPE == DocType)
            szMsg  = String("The contents of your envelope project '%s' has changed.\n\nDo you want to save the changes?\n", strFileName);
           
		int iResponse = CMessageBox::Message(szMsg, (bExit ? MB_YESNO | MB_SYSTEMMODAL : MB_YESNOCANCEL));
		if (iResponse == IDCANCEL)
			return S_FALSE;

		if (iResponse == IDYES)
		{
			if (SaveAs(true/*bSilent*/, bExit) != S_OK)
				return E_FAIL;
		}
	}

	CWaitCursor Wait;

	if (!m_LinkedDoc.bSwitchInProgress)
        CleanupLinkedDoc();

	if (bExit)
	{
        delete m_pAGDoc;
		m_pAGDoc = NULL;
	}
	else
	{
		Clipboard.NotifyDocumentClose();

		delete m_pAGDoc;
		m_pAGDoc = NULL;

		m_pCtp->ActivateNewDoc(); // Even though the doc pointer is NULL

		Invalidate();
	}
    
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocSave(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return SaveAs(true/*bSilent*/, false/*bExit*/);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocSaveAs(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return SaveAs(false/*bSilent*/, false/*bExit*/);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::SaveAs(bool bSilent, bool bExit)
{
	if (!m_pAGDoc)
		return E_FAIL;

    bool bIsTempFile = m_pAGDoc->IsTempFile(m_szTempDir);
	CString str = m_pCtp->GetContextMemberStatus();
	bool bPaidMember = !(str!="MCP" && str!="MCS");

	// Card Price
	str = m_pCtp->GetContextContentStatus();
	bool bFreeContent = (str=="0.0");

	if (!bPaidMember && !bFreeContent)
	{
		CMessageBox::Message("Join now to save.");
		return S_OK;
	}
   
	CString strFullDocPath;
	CString strPath;
	CString strFileName;
	m_pAGDoc->GetFileName(strFullDocPath, strPath, strFileName);

	// If there is no path specified...
	if (strPath.IsEmpty() || bIsTempFile)
	{
		bSilent = false;

		// Get the path from the registry
		CRegKey regkey;
		if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
		{
			char szFilePath[MAX_PATH];
			szFilePath[0] = 0;
			DWORD nChars = MAX_PATH;
			bool bOK = (regkey.QueryStringValue(REGVAL_MRU_CTP_PATH, szFilePath, &nChars) == ERROR_SUCCESS);
			if (bOK)
			{
				szFilePath[nChars] = 0;
				strPath = szFilePath;
			}
		}

		// If there is no path in the registry, use the default
		if (strPath.IsEmpty())
		{
			bool bOK = GetSpecialFolderLocation(strPath, CSIDL_COMMON_DESKTOPDIRECTORY);
			if (!bOK)
				bOK = GetSpecialFolderLocation(strPath, CSIDL_DESKTOPDIRECTORY);
		}
	}

	char szFileName[MAX_PATH];
	szFileName[0] = 0;
	lstrcpy(szFileName, strFileName);

	if (!bSilent)
	{
		OPENFILENAMEX ofx;
		::ZeroMemory(&ofx, sizeof(ofx));
		ofx.of.lStructSize = (IsEnhancedWinVersion() ? sizeof(ofx) : sizeof(ofx)-12);
		ofx.of.hwndOwner = m_pCtp->GetBrowser();
		ofx.of.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK;
		ofx.of.lpstrInitialDir = strPath;
		ofx.of.lpstrFile = szFileName;
		ofx.of.lpfnHook = CDocWindow::OpenFileHookProc;
		ofx.of.nMaxFile = MAX_PATH;
		ofx.of.lpstrFilter = "Project Files (*.ctp)\0*.*ctp\0\0";
		CString strTitle = "Save your " + g_szAppName + " project";
		ofx.of.lpstrTitle = strTitle;
		ofx.of.lpstrDefExt = "ctp";

		if (!::GetSaveFileName(&ofx.of))
			return E_FAIL;

		// Save the path in the registry
		CRegKey regkey;
		if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
			bool bOK = (regkey.SetStringValue(REGVAL_MRU_CTP_PATH, StrPath(szFileName)) == ERROR_SUCCESS);

		// Redraw the window underneath, before we get to work
		::UpdateWindow(m_pCtp->GetBrowser());
	}
	// TFT if required to make the changes for the new Save issue
	// then uncomment the following
	//else
	//	lstrcpy(szFileName, strFullDocPath);


    // Add to MRU
    CString szFile(szFileName);
    if (m_szTempDir.IsEmpty() || szFile.Find(m_szTempDir) < 0)
    {
        m_mruList.AddToList(szFileName);
	    m_mruList.WriteToRegistry(REGKEY_APP);
    }

	CWaitCursor Wait;

	if (SpecialsEnabled() && m_pAGDoc->NeedsCardBack())
	{ // Hide the card back graphics so they will not be saved
		// Unselect the current symbol
		SymbolUnselect(true/*bClearPointer*/);
		m_pCtp->ShowHideCardBack(false/*bShow*/);
	}

	ofstream output(szFileName, ios::out | ios::binary);
	int bOpen = output.is_open();

	if (!bOpen || !m_pAGDoc->Write(output))
	{
		CMessageBox::Message(String("Error saving to file '%s'.", szFileName), MB_OK | (bExit ? MB_SYSTEMMODAL : 0));
		return E_FAIL;
	}		
	
	if (SpecialsEnabled() && m_pAGDoc->NeedsCardBack())
	{ // Show the card back graphics
		m_pCtp->ShowHideCardBack(true/*bShow*/);
	}

	m_pAGDoc->SetModified(false);
	m_pAGDoc->SetFileName(CString(szFileName));
	m_pCtp->PutSrcURL(CComBSTR(szFileName));

#ifndef NETSCAPE
	m_pCtp->Fire_FileSaved(0/*lValue*/);
#endif NETSCAPE

	// Get out if special features are NOT enabled
	if (!SpecialsEnabled())
		return S_OK;

	// Create thumbnails
	int iResponse = CMessageBox::Message(
		String("Project saved to '%s'.\n\nWould you like to create thumbnails of the saved project?", szFileName),
		MB_YESNO | (bExit ? MB_SYSTEMMODAL : 0));
	if (iResponse != IDYES)
		return S_OK;

	CAGPage* pPage = m_pAGDoc->GetPage(0);
	if (!pPage)
		return E_FAIL;

	strPath = szFileName;
	strPath = strPath.Left(strPath.ReverseFind('.'));
	if (strPath.Right(1) == 'f')
		strPath = strPath.Left(strPath.GetLength()-1);

	SIZE MaxSizeT = {153, 153};
	pPage->CreateThumb(MaxSizeT, strPath + "T.bmp");
	SIZE MaxSizeD = {400, 400};
	pPage->CreateThumb(MaxSizeD, strPath + "D.bmp");

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocSaveAsPDF(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	return SaveAsPDF();
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::SaveAsPDF()
{
	if (!m_pAGDoc)
		return E_FAIL;

	CString strFullDocPath;
	CString strPath;
	CString strFileName;
	m_pAGDoc->GetFileName(strFullDocPath, strPath, strFileName);

	if (strPath.IsEmpty())
	{
		bool bOK = GetSpecialFolderLocation(strPath, CSIDL_COMMON_DESKTOPDIRECTORY);
		if (!bOK)
			 bOK = GetSpecialFolderLocation(strPath, CSIDL_DESKTOPDIRECTORY);
	}

	char szFileName[MAX_PATH];
	szFileName[0] = 0;
	lstrcpy(szFileName, strFileName);

	OPENFILENAMEX ofx;
	::ZeroMemory(&ofx, sizeof(ofx));
	ofx.of.lStructSize = (IsEnhancedWinVersion() ? sizeof(ofx) : sizeof(ofx)-12);
	ofx.of.hwndOwner = m_pCtp->GetBrowser();
	ofx.of.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK;
	ofx.of.lpstrInitialDir = strPath;
	ofx.of.lpstrFile = szFileName;
	ofx.of.lpfnHook = CDocWindow::OpenFileHookProc;
	ofx.of.nMaxFile = MAX_PATH;
	ofx.of.lpstrFilter = "Adobe Acrobat Files (*.pdf)\0*.pdf\0\0";
	CString strTitle = "Save your " + g_szAppName + " project as a PDF";
	ofx.of.lpstrTitle = strTitle;
	ofx.of.lpstrDefExt = "pdf";

	if (!::GetSaveFileName(&ofx.of))
		return E_FAIL;

	// Redraw the window underneath, before we get to work
	::UpdateWindow(m_pCtp->GetBrowser());

	CWaitCursor Wait;

	return m_Print.MakePdf(strFullDocPath, szFileName);
}

static bool bExamineSpec;
static CAGSpec ExamineSpec;

/////////////////////////////////////////////////////////////////////////////
static bool CALLBACK CallbackExamineSpecs(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate)
{
	if (pSpec && bExamineSpec)
	{
		ExamineSpec = *pSpec;
		bExamineSpec = false;
	}
	
	return false; // Do not change the spec
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocSaveAsXML(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
#ifdef FYS
	CString OrderId("1234");
	CString CorrId("5678");
	CString Info("FYSInfo");
	CFYSPrintDoc *pFYSDoc = new CFYSPrintDoc(m_pAGDoc);
	if (!pFYSDoc->CreateXmlDoc(OrderId, CorrId, Info))
	{
		CString strError = pFYSDoc->GetErrorMsg();
		//CMessageBox::Message(strError);
		delete pFYSDoc;
		return S_OK;
	}
	delete pFYSDoc;
	return S_OK;
#else
	if (!m_pAGDoc)
		return E_FAIL;

	CAGDoc* pAGDoc = m_pAGDoc;
	
	CString strFullDocPath;
	CString strPath;
	CString strFileName;
	pAGDoc->GetFileName(strFullDocPath, strPath, strFileName);

	if (strPath.IsEmpty())
	{
		bool bOK = GetSpecialFolderLocation(strPath, CSIDL_COMMON_DESKTOPDIRECTORY);
		if (!bOK)
			 bOK = GetSpecialFolderLocation(strPath, CSIDL_DESKTOPDIRECTORY);
	}

	char szFileName[MAX_PATH];
	szFileName[0] = 0;
	lstrcpy(szFileName, strFileName);

	OPENFILENAMEX ofx;
	::ZeroMemory(&ofx, sizeof(ofx));
	ofx.of.lStructSize = (IsEnhancedWinVersion() ? sizeof(ofx) : sizeof(ofx)-12);
	ofx.of.hwndOwner = m_pCtp->GetBrowser();
	ofx.of.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK;
	ofx.of.lpstrInitialDir = strPath;
	ofx.of.lpstrFile = szFileName;
	ofx.of.lpfnHook = CDocWindow::OpenFileHookProc;
	ofx.of.nMaxFile = MAX_PATH;
	ofx.of.lpstrFilter = "XML Files (*.xml)\0*.*xml\0\0";
	CString strTitle = "Save your " + g_szAppName + " project as XML";
	ofx.of.lpstrTitle = strTitle;
	ofx.of.lpstrDefExt = "xml";

	if (!::GetSaveFileName(&ofx.of))
		return E_FAIL;

	// Redraw the window underneath, before we get to work
	::UpdateWindow(m_pCtp->GetBrowser());

	CWaitCursor Wait;

	FILE* output = fopen(szFileName, "wb");
	if (!output)
	{
		CMessageBox::Message("Can't open output file");
		return E_FAIL;
	}

	CString strType = CAGDocTypes::Name(pAGDoc->GetDocType());
	CString strOrientation = (pAGDoc->IsPortrait() ? "Portrait" : "Landscape");
	int nPages = pAGDoc->GetNumPages();
	fprintf(output, "<?xml version='1.0' encoding='UTF-8'?>\n");
	fprintf(output, "	<Documents Count='1'>\n");
	fprintf(output, "		<Document FileName='%s' Type='%s' Orientation='%s' PageCount='%d'>\n", strFullDocPath, strType, strOrientation, nPages);

	for (int nPage = 0; nPage < nPages; nPage++)
	{
		CAGPage* pPage = pAGDoc->GetPage(nPage);
		if (!pPage)
			continue;

		SIZE PageSize = {0,0};
		pPage->GetPageSize(PageSize);
		double dx = DINCHES(PageSize.cx);
		double dy = DINCHES(PageSize.cy);
		CString strPageName;
		pPage->GetPageName(strPageName);
		fprintf(output, "\t\t\t<Page PageNumber='%d' OrientationInHand='%s' PageWidth='%0.5G' PageHeight='%0.5G' Name='%s'>\n", nPage+1, strOrientation, dx, dy, strPageName);

		int nLayers = pPage->GetNumLayers();
		for (int nLayer = 0; nLayer < nLayers; nLayer++)
		{
			CAGLayer* pLayer = pPage->GetLayer(nLayer);
			if (!pLayer)
				continue;

			int nSymbols = pLayer->GetNumSymbols();
			for (int nSymbol = nSymbols - 1; nSymbol >= 0; nSymbol--)
			{
				CAGSym* pSym = pLayer->GetSymbol(nSymbol);
				if (!pSym || pSym->IsHidden())
					continue;

				CAGMatrix Matrix = pSym->GetMatrix();
//j				RECT DestRect = pSym->GetDestRect();
				RECT DestRect = pSym->GetDestRectTransformed();
				double fXPos = DINCHES(DestRect.left);
				double fYPos = DINCHES(DestRect.top);
				double fWidth = DINCHES(WIDTH(DestRect));
				double fHeight = DINCHES(HEIGHT(DestRect));
				double fcx = Matrix.m_cx / APP_RESOLUTION;
				double fcy = Matrix.m_cy / APP_RESOLUTION;
				double fAngle = Matrix.GetAngle();
				bool bRotated = Matrix.IsRotated();

				if (pSym->IsImage())
				{
					CAGSymImage* pSymImage = static_cast<CAGSymImage*>(pSym);
					if (!pSymImage)
						continue;

					CString strEncoding = "base64";
					COLORREF c = pSymImage->GetTransparentColor();
					CString strTransparentColor;
					strTransparentColor.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
					strTransparentColor.MakeUpper();

					CString strSourceType = pSymImage->GetSourceType();
					CString strFileName = "Unknown";
					BYTE* pMemory = NULL;
					DWORD dwSrcSize = 0;

					// Try to read in the archived file data
					HGLOBAL hMemory = pSymImage->RestoreSourceData();
					if (hMemory)
					{
						pMemory = (BYTE*)::GlobalLock(hMemory);
						dwSrcSize = ::GlobalSize(hMemory);
					}
					
					// If no source data was restored...
					if (!pMemory || dwSrcSize <= 0)
					{
						BITMAPINFOHEADER* pDIB = pSymImage->GetDib();
						pMemory = (BYTE*)pDIB;
						dwSrcSize = DibSize(pDIB);
					}

					int iDestSize = Base64EncodeGetRequiredLength(dwSrcSize);
					BYTE* pDestData = new BYTE[iDestSize];
					BOOL bOK = Base64Encode(pMemory, dwSrcSize, (LPSTR)pDestData, &iDestSize);
					pDestData[iDestSize] = '\0';

					fprintf(output, "\t\t\t\t<ExternalImageContent FileName='%s' FileSize='%d' Content-Encoding='%s' Content-Type='%s' TransparentColor='%s' XPos='%0.5G' YPos='%0.5G' Width='%0.5G' Height='%0.5G' Angle='%0.5G' IsAdvanced='true'>\n", strFileName, dwSrcSize, strEncoding, strSourceType, strTransparentColor, fXPos, fYPos, fWidth, fHeight, fAngle);
//j					fprintf(output, "\t\t\t\t<InlineImageContent FileName='%s' FileSize='%d' Content-Encoding='%s' Content-Type='%s' XPos='%0.5G' YPos='%0.5G' Width='%0.5G' Height='%0.5G' Angle='%0.5G' IsAdvanced='true'>\n", strFileName, dwSrcSize, strEncoding, strSourceType, fXPos, fYPos, fWidth, fHeight, fAngle);
//j					fprintf(output, "\t\t\t\t\t<Transformation X_mx='%0.5G' X_my='%0.5G' X_d='%0.5G' Y_mx='%0.5G' Y_my='%0.5G' Y_d='%0.5G' />\n",
//j						Matrix.m_ax, Matrix.m_bx, fcx, Matrix.m_ay, Matrix.m_by, fcy);

//j					fprintf(output, "\t\t\t\t\t<ImageData>\n<![CDATA[");
//j					fprintf(output, (LPCSTR)pDestData);
//j					fprintf(output, "]]>\n");
//j					fprintf(output, "\t\t\t\t\t</ImageData>\n");

//j					fprintf(output, "\t\t\t\t</InlineImageContent>\n");
					fprintf(output, "\t\t\t\t</ExternalImageContent>\n");

					int iSrcSize = Base64DecodeGetRequiredLength(iDestSize);
					BYTE* pSrcData = new BYTE[iSrcSize];
					bOK = Base64Decode((LPCSTR)pDestData, iDestSize, (BYTE*)pSrcData, &iSrcSize);
					pSrcData[iSrcSize] = '\0';
					delete [] pSrcData;

					delete [] pDestData;

					if (hMemory)
					{
						::GlobalUnlock(hMemory);
						::GlobalFree(hMemory);
					}
				}
				else
				if (pSym->IsGraphic())
				{
					CAGSymGraphic* pSymGraphic = static_cast<CAGSymGraphic*>(pSym);
					if (!pSymGraphic)
						continue;

					CString strShapeName;
					if (pSym->IsRectangle())
						strShapeName = "Rectangle";
					else
					if (pSym->IsEllipse())
						strShapeName = "Circle/Ellipse";
					else
					if (pSym->IsLine())
						strShapeName = "Line";
					else
					if (pSym->IsShape())
					{
						strShapeName = ((CAGSymDrawing*)pSymGraphic)->GetShapeName();
					}

					fprintf(output, "\t\t\t\t<ShapeContent ShapeName='%s' XPos='%0.5G' YPos='%0.5G' Width='%0.5G' Height='%0.5G' Angle='%0.5G' IsAdvanced='true'>\n", strShapeName, fXPos, fYPos, fWidth, fHeight, fAngle);

					LPCSTR szFillTypes[] = {
						"Hollow", "Solid", "Gradient"/*right*/, "Gradient"/*own*/, "Radial"/*center*/, "Radial"/*corner*/ };
					FillType ft = pSymGraphic->GetFillType();
					CString strFillType = szFillTypes[ft];
					if (ft == FT_SweepRight)
						strFillType + " GradientAngle='0'";
					if (ft == FT_SweepDown)
						strFillType + " GradientAngle='90'";

					COLORREF c = pSymGraphic->GetFillColor();
					CString strFillColor;
					strFillColor.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
					strFillColor.MakeUpper();

					c = pSymGraphic->GetFillColor2();
					CString strFillColor2;
					strFillColor2.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
					strFillColor2.MakeUpper();

					fprintf(output, "\t\t\t\t\t<Fill Type='%s' StartColorARGB='%s' EndColorARGB='%s' />\n", strFillType, strFillColor, strFillColor2);

					DWORD dwOutlineThickness = pSymGraphic->GetLineWidth();
					LPCSTR strOutlineType = (dwOutlineThickness == LT_None ? "None" : "Solid");
					LPCSTR strOutlineThicknessUnit = (dwOutlineThickness == LT_Hairline ? "Pixel" : "Point");
					if (dwOutlineThickness == LT_Hairline)
						dwOutlineThickness = 1;

					c = pSymGraphic->GetLineColor();
					CString strOutlineColor;
					strOutlineColor.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
					strOutlineColor.MakeUpper();
					fprintf(output, "\t\t\t\t\t<Outline Type='%s' ColorARGB='%s' Thickness='%0.5G' ThicknessUnit='%s' />\n", strOutlineType, strOutlineColor, POINTSIZE(dwOutlineThickness), strOutlineThicknessUnit);

					fprintf(output, "\t\t\t\t\t<Transformation X_mx='%0.5G' X_my='%0.5G' X_d='%0.5G' Y_mx='%0.5G' Y_my='%0.5G' Y_d='%0.5G' />\n",
						Matrix.m_ax, Matrix.m_bx, fcx, Matrix.m_ay, Matrix.m_by, fcy);

					if (pSym->IsShape())
					{
						CAGSymDrawing* pShape = (CAGSymDrawing*)pSym;
						POINT* pPoints = NULL;
						int nPoints = pShape->GetPoints(&pPoints);
						int nCommands = 0;
						if (nPoints > 0)
						{
							while (SYMBOL_COMMANDPOINT(pPoints[nCommands].x))
								nCommands++;
						}
						nPoints -= nCommands;
						POINT* pCommands = pPoints;
						pPoints += nCommands;

						int nCurrentPoint = 0;
						fprintf(output, "\t\t\t\t\t<Figure>\n");
						for (int i = 0; i < nCommands; i++)
						{
							if (pCommands->x == SYMBOL_MOVE)
							{
								if (i)
								{
									fprintf(output, "\t\t\t\t\t</Figure>\n");
									fprintf(output, "\t\t\t\t\t<Figure>\n");
								}
								double fx = DINCHES(pPoints[nCurrentPoint].x);
								double fy = DINCHES(pPoints[nCurrentPoint].y);
								fprintf(output, "\t\t\t\t\t\t<MoveTo XPos='%0.5G' YPos='%0.5G' />\n", fx, fy);
							}
							else
							if (pCommands->x == SYMBOL_POLYGON)
							{
								fprintf(output, "\t\t\t\t\t\t<Polygon>\n");
								for (int p = 0; p < pCommands->y; p++)
								{
									double fx = DINCHES(pPoints[nCurrentPoint+p].x);
									double fy = DINCHES(pPoints[nCurrentPoint+p].y);
									fprintf(output, "\t\t\t\t\t\t\t<Point XPos='%0.5G' YPos='%0.5G' />\n", fx, fy);
								}
								fprintf(output, "\t\t\t\t\t\t</Polygon>\n");
							}
							else 
							if (pCommands->x == SYMBOL_BEZIER)
							{
								fprintf(output, "\t\t\t\t\t\t<PolyBezierTo>\n");
								for (int p = 0; p < pCommands->y; p++)
								{
									double fx = DINCHES(pPoints[nCurrentPoint+p].x);
									double fy = DINCHES(pPoints[nCurrentPoint+p].y);
									fprintf(output, "\t\t\t\t\t\t\t<Point XPos='%0.5G' YPos='%0.5G' />\n", fx, fy);
								}
								fprintf(output, "\t\t\t\t\t\t</PolyBezierTo>\n");
							}
							else
							if (pCommands->x == SYMBOL_LINE)
							{
								double fx = DINCHES(pPoints[nCurrentPoint].x);
								double fy = DINCHES(pPoints[nCurrentPoint].y);
								fprintf(output, "\t\t\t\t\t\t<LineTo XPos='%0.5G' YPos='%0.5G' />\n", fx, fy);
							}

							nCurrentPoint += pCommands->y;
							pCommands++;
						}

						fprintf(output, "\t\t\t\t\t</Figure>\n");
					}

					fprintf(output, "\t\t\t\t</ShapeContent>\n");
				}
				else
				if (pSym->IsText())
				{
					CAGSymText* pSymText = static_cast<CAGSymText*>(pSym);
					if (!pSymText)
						continue;

					bExamineSpec = true;
					pSymText->SpecChanger(CallbackExamineSpecs, NULL);

					CString strFontName = ExamineSpec.m_Font.lfFaceName;
					int iFontPointSize = ExamineSpec.m_Font.lfHeight;

					LPCSTR szHorzJustTypes[] = {
						"Center", "Left", "Right", "Justified", "Illegal", "Illegal", "Illegal", "Illegal" };
					eTSJust hj = ExamineSpec.m_HorzJust;
					CString strHorzJust = szHorzJustTypes[(hj&7)];

					LPCSTR szVertJustTypes[] = {
						"Illegal", "Middle", "Top", "Bottom", "Justified", "Justify" };
					eVertJust vj = pSymText->GetVertJust();
					CString strVertJust = szVertJustTypes[vj];

					COLORREF c = ExamineSpec.m_FillColor;
					CString strColor;
					strColor.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
					strColor.MakeUpper();

					CString strBold = (ExamineSpec.m_Font.lfWeight != FW_NORMAL ? "true" : "false");
					CString strItalic = (ExamineSpec.m_Font.lfItalic ? "true" : "false");
					CString strUnderline = (ExamineSpec.m_Font.lfUnderline ? "true" : "false");
					fprintf(output, "\t\t\t\t<TextContent FontName='%s' FontPointSize='%0.5G' HorizontalJustification='%s' VerticalJustification='%s' Color='%s' Bold='%s' Italic='%s' Underline='%s' XPos='%0.5G' YPos='%0.5G' Width='%0.5G' Height='%0.5G' Angle='%0.5G' IsAdvanced='true'>\n",
						strFontName, POINTSIZE(iFontPointSize), strHorzJust, strVertJust, strColor, strBold, strItalic, strUnderline, fXPos, fYPos, fWidth, fHeight, fAngle);

					CString strText = pSymText->ExtractText();
					fprintf(output, "\t\t\t\t\t<TextData>\n<![CDATA[%s]]>\n\t\t\t\t\t</TextData>\n", strText);

					LPCSTR szFillTypes[] = {
						"Hollow", "Solid", "Gradient"/*right*/, "Gradient"/*own*/, "Radial"/*center*/, "Radial"/*corner*/ };
					FillType ft = ExamineSpec.m_FillType;
					CString strFillType = szFillTypes[ft];

					c = ExamineSpec.m_FillColor;
					CString strFillColor;
					strFillColor.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
					strFillColor.MakeUpper();

					c = ExamineSpec.m_FillColor2;
					CString strFillColor2;
					strFillColor2.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
					strFillColor2.MakeUpper();

					fprintf(output, "\t\t\t\t\t<Fill Type='%s' StartColorARGB='%s' EndColorARGB='%s' />\n", strFillType, strFillColor, strFillColor2);

					DWORD dwOutlineThickness = ExamineSpec.m_LineWidth;
					LPCSTR strOutlineType = (dwOutlineThickness == LT_None ? "None" : "Solid");
					LPCSTR strOutlineThicknessUnit = (dwOutlineThickness == LT_Hairline ? "Pixel" : "Point");
					if (dwOutlineThickness == LT_Hairline)
						dwOutlineThickness = 1;

					c = ExamineSpec.m_FillColor;
					CString strOutlineColor;
					strOutlineColor.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
					strOutlineColor.MakeUpper();
					fprintf(output, "\t\t\t\t\t<Outline Type='%s' ColorARGB='%s' Thickness='%0.5G' ThicknessUnit='%s' />\n", strOutlineType, strOutlineColor, POINTSIZE(dwOutlineThickness), strOutlineThicknessUnit);

					fprintf(output, "\t\t\t\t\t<Transformation X_mx='%0.5G' X_my='%0.5G' X_d='%0.5G' Y_mx='%0.5G' Y_my='%0.5G' Y_d='%0.5G' />\n",
						Matrix.m_ax, Matrix.m_bx, fcx, Matrix.m_ay, Matrix.m_by, fcy);
					fprintf(output, "\t\t\t\t</TextContent>\n");
				}
				else
				if (pSym->IsCalendar())
				{
				}
			}
		}

		fprintf(output, "\t\t\t</Page>\n");
	}

	fprintf(output, "		</Document>\n");
	fprintf(output, "	</Documents>\n");
	fclose(output);

	return S_OK;
#endif
}

#ifdef FYS
/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::DoFYSDataTransfer(CString OrderId, CString CorrelationId, CString FYSInfo, CString& strError)
{
	CFYSPrintDoc *pFYSDoc = new CFYSPrintDoc(m_pAGDoc);
	if (!pFYSDoc->CreateXmlDoc(OrderId, CorrelationId, FYSInfo))
	{
		strError = pFYSDoc->GetErrorMsg();
#ifdef _DEBUG
		CMessageBox::Message(strError);
#endif _DEBUG
		delete pFYSDoc;
		return false;
	}

	bool bDevelopment = m_pCtp->IsDevelopmentServer();
	CString strURL;
	if (bDevelopment)
		strURL = "http://sitetest.4yoursoul.com/31/ClientInterface/CreateAndPrintDataUpload.asp";
	else
		strURL = "http://4yoursoul.americangreetings.com/31/ClientInterface/CreateAndPrintDataUpload.asp";

#ifdef _DEBUG
	strURL = "http://localhost/simpleupload/Receiver.aspx";
#endif _DEBUG

	if (!pFYSDoc->SendXmlDoc(strURL))
	{
		strError = pFYSDoc->GetErrorMsg();
		delete pFYSDoc;
		return false;
	}

	delete pFYSDoc;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::CloseFYSRemoteDlg()
{
	if (m_FYSRemoteDlg.m_hWnd && ::IsWindow(m_FYSRemoteDlg.m_hWnd))
		m_FYSRemoteDlg.PostMessage(WM_CLOSE);
}
#endif FYS

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocPrint(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	CWaitCursor Wait;

#ifdef FYS
	if (m_FYSRemoteDlg.m_hWnd && ::IsWindow(m_FYSRemoteDlg.m_hWnd))
	{
		m_FYSRemoteDlg.DoModeless(NULL);
		return S_OK;
	}
#endif FYS

	// Member Status
	CString strMemStatus = m_pCtp->GetContextMemberStatus();
	bool bPaidMember = !(strMemStatus!="MCP" && strMemStatus!="MCS");

	// Card Price
	CString strPrice = m_pCtp->GetContextContentStatus();
	int i = atoi(strPrice);
	bool bFreeContent = (i!=1);

	// Allow Remote
	// Note: remote print can be explicitly disabled from server 
	// via bRemotePrintEnabled thus overriding other conditions below.
	bool bBlueMountain = m_pCtp->IsBlueMountain();
	bool bDisney = (m_pCtp->GetContextLicense() == 283);
	BOOL bRemotePrintEnabled = m_pCtp->GetContextRemotePrintStatus(); 
	bool bAllowRemotePrint = bRemotePrintEnabled && (!bBlueMountain && !bDisney && !m_bMetricMode && !m_pCtp->IsAIM() && m_pAGDoc->NeedsCardBack() && !m_pCtp->IsApplication() && !m_pAGDoc->HasLockedImages());

	// Need to display this only for americangreetings.com 
	CString strRetval = "LOCAL";
#ifdef FYS
	CString strURL;
	if (m_pCtp->IsAmericanGreetings())
	{
		if (bAllowRemotePrint || (!bAllowRemotePrint && !bFreeContent && !bPaidMember))
		{
			CString strRemPrint = (bAllowRemotePrint?"1":"0");

			strURL = "PrintOptionsAg.htm";
			CHtmlDialog dlg(strURL, true, ::GetActiveWindow());

			CString strName = "memstat";
			dlg.SetValue(strName, strMemStatus);

			strName = "price";
			dlg.SetValue(strName, strPrice);

			strName = "remprint";
			dlg.SetValue(strName, strRemPrint);

			dlg.SetWindowSize(600, 300);
			dlg.SetResizable(false);
			dlg.SetScrollbars(false);
			if (!dlg.DoModal())
				return S_OK;

			strRetval = dlg.GetReturnString();
			if (strRetval == "")
				return S_OK;
		}
	}

	if (strRetval == "REMOTE")
	{
		if (m_pCtp->IsDevelopmentServer())
			strURL = "http://sitetest.4yoursoul.com/31/ClientInterface/MemberCnP.asp";
		else
			strURL = "http://4yoursoul.americangreetings.com/31/ClientInterface/MemberCnP.asp";

		// build the product number
		strURL += "?prodnum=";
		strURL += m_pCtp->GetContextProductNumber();

		CComQIPtr<IDispatch> pDisp = m_pCtp;
		HWND hWndParent = ::GetActiveWindow();

		m_FYSRemoteDlg.SetWindowSize(720, 600);
		m_FYSRemoteDlg.SetDispatch(pDisp);
		m_FYSRemoteDlg.SetURL(strURL, false);
		m_FYSRemoteDlg.DoModeless(hWndParent);

		return S_OK;
	}

	if (strRetval == "JOIN" || strRetval == "SIGNIN")
	{
		CComBSTR bstr;
		m_pCtp->GetContext(&bstr);

		CString strContext(bstr);
		strContext.Replace('&', '|');
		strContext.Replace('=', ':');
		strContext += "|callproc:signin";

		//CHtmlDialog dlg("FYS_Home.htm", true, ::GetActiveWindow());
		CString strCpOrderProcessPage = m_pCtp->GetContextCpHost();
		if (strCpOrderProcessPage[strCpOrderProcessPage.GetLength()-1] != '/')
			strCpOrderProcessPage += "/";
		strCpOrderProcessPage += "cporderprocess.pd";

		CHtmlDialog dlg(strCpOrderProcessPage, false, ::GetActiveWindow());

		CString strName = "Process";
		dlg.SetValue(strName, strRetval);

		strName = "ServerType";
		CString strServerType = m_pCtp->GetServerType();
		dlg.SetValue(strName, strServerType);

		strName = "ContextString";
		dlg.SetValue(strName, strContext);

		dlg.SetWindowSize(720, 600);
		dlg.SetResizable(false);
		dlg.SetScrollbars(false);
		dlg.DoModal();

		strRetval.Empty();
		strContext.Empty();
		dlg.GetControlValue("Context", strContext);
		dlg.GetControlValue("Status", strRetval);

		if (strRetval == "LOCAL")
			m_pCtp->PutContext(CComBSTR(strContext));
	}
#endif FYS

	HRESULT hr = S_OK;
	if (strRetval == "LOCAL")
		hr = (m_Print.Start() ? S_OK : E_FAIL);

	if (m_pCtp->AutoPrintEnabled())
		m_pCtp->AutoCloseBrowser();

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocPrintPreview(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return (m_Print.StartPreview() ? S_OK : E_FAIL);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocConvert(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	CPagePanel* pPagePanel = m_pCtp->GetPagePanel();
	if (!pPagePanel)
		return E_FAIL;

	CConvertDlg dlg(m_pCtp, m_pAGDoc);
	if (dlg.DoModal() != IDOK)
		return S_OK;

	CWaitCursor Wait;
	
	// Update the list of pages and reactivate the current page
	pPagePanel->ActivateNewDoc();
	pPagePanel->PageChange();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocProperties(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	CPagePanel* pPagePanel = m_pCtp->GetPagePanel();
	if (!pPagePanel)
		return E_FAIL;

	CPropertiesDlg dlg(m_pCtp, m_pAGDoc);
	if (dlg.DoModal() != IDOK)
		return S_OK;

	CWaitCursor Wait;
	
	// Update the list of pages and reactivate the current page
	pPagePanel->ActivateNewDoc();
	pPagePanel->PageChange();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocAddImage(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return AddImage(0);
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CDocWindow::GetTransparentColor(LPCTSTR pFileName)
{
	CString strExtension = StrExtension(pFileName);
	COLORREF TransparentColor = CLR_NONE;
	if (strExtension == ".gif")
		TransparentColor = GetGifTransparentColor(pFileName);
	else
	if (strExtension == ".png")
		TransparentColor = CPng::GetPngTransparentColor(pFileName, true/*bDefaultToUpperLeft*/);

	return TransparentColor;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::AddImage(int iOffset)
{
	if (!m_pAGDoc)
		return E_FAIL;

	CString strPath;

	// Get the path from the registry
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		char szFilePath[MAX_PATH];
		szFilePath[0] = 0;
		DWORD nChars = MAX_PATH;
		bool bOK = (regkey.QueryStringValue(REGVAL_MRU_PICTURE_PATH, szFilePath, &nChars) == ERROR_SUCCESS);
		if (bOK)
		{
			szFilePath[nChars] = 0;
			strPath = szFilePath;
		}
	}

	// If there is no path in the registry, use the default
	if (strPath.IsEmpty())
	{
		bool bOK = GetSpecialFolderLocation(strPath, CSIDL_MYPICTURES);
		if (!bOK)
			bOK = GetSpecialFolderLocation(strPath, CSIDL_COMMON_PICTURES);
	}

	char szFileName[MAX_PATH];
	szFileName[0] = 0;

	#define FILTER_WITH_GDIPLUS    "All Supported Files (*.bmp, *.gif, *.jpg, *.tif, *.png, *.wmf, *.ico, *.emf)\0*.bmp;*.gif;*.jpg;*.tif*;*.png;*.wmf;*.ico;*.emf\0Bitmap Files (*.bmp)\0*.bmp\0GIF Files (*.gif)\0*.gif\0JPG Files (*.jpg)\0*.jpg\0TIF Files (*.tif)\0*.tif*\0PNG Files (*.png)\0*.png\0Windows Metafiles Files (*.wmf)\0*.wmf\0Windows Enhanced Metafiles Files (*.emf)\0*.emf\0Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0\0"
	#define FILTER_WITHOUT_GDIPLUS "All Supported Files (*.bmp, *.gif, *.jpg, *.png, *.wmf, *.ico, *.emf)\0*.bmp;*.gif;*.jpg;*.png;*.wmf;*.ico;*.emf\0Bitmap Files (*.bmp)\0*.bmp\0GIF Files (*.gif)\0*.gif\0JPG Files (*.jpg)\0*.jpg\0PNG Files (*.png)\0*.png\0Windows Metafiles Files (*.wmf)\0*.wmf\0Windows Enhanced Metafiles Files (*.emf)\0*.emf\0Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0\0"

	OPENFILENAMEX ofx;
	::ZeroMemory(&ofx, sizeof(ofx));
	ofx.of.lStructSize = (IsEnhancedWinVersion() ? sizeof(ofx) : sizeof(ofx)-12);
	ofx.of.hwndOwner = m_pCtp->GetBrowser();
	ofx.of.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK;
	ofx.of.lpstrInitialDir = strPath;
	ofx.of.lpstrFile = szFileName;
	ofx.of.lpfnHook = CDocWindow::OpenFileHookProc;
	ofx.of.nMaxFile = MAX_PATH;
	ofx.of.lpstrFilter = (CImage::IsGdiPlusInstalled() ? FILTER_WITH_GDIPLUS : FILTER_WITHOUT_GDIPLUS);
	ofx.of.lpstrTitle = "Add-A-Photo® or an Art File";
	ofx.of.lpstrDefExt = "bmp";

	if (!::GetOpenFileName(&ofx.of))
		return E_FAIL;

	// Save the path in the registry
	if (regkey.m_hKey)
		bool bOK = (regkey.SetStringValue(REGVAL_MRU_PICTURE_PATH, StrPath(szFileName)) == ERROR_SUCCESS);

	// Redraw the window underneath, before we get to work
	::UpdateWindow(ofx.of.hwndOwner);

	return AddImage(szFileName, iOffset);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::AddImage(LPCSTR strFileName, int iOffset, int x, int y, bool bImageDropped)
{
	if (!m_pAGDoc)
		return E_FAIL;

	LRESULT lResult = E_FAIL;

	CString strExtension = StrExtension(strFileName);
	if (strExtension == ".png")
	{
		HGLOBAL hMemory = CPng::ReadPNG(strFileName, true/*bDefaultToUpperLeft*/);
		if (!hMemory)
			return E_FAIL;

		lResult = AddImage(hMemory, true/*bIsDIB*/, iOffset, CLR_NONE, x, y, bImageDropped);
		::GlobalFree(hMemory);
	}
	else
	if (strExtension == ".jpg" || strExtension == ".wmf" || strExtension == ".emf" || strExtension == ".bmp")
	{
		CImage Image(strFileName);
		HGLOBAL hMemory = Image.GetNativeImageHandle();
		if (!hMemory)
			return E_FAIL;

		lResult = AddImage(hMemory, (strExtension == ".bmp")/*bIsDIB*/, iOffset, CLR_NONE, x, y, bImageDropped);
	}
	else
	{ // gif, ico, tif (someday png could fall thru to here - GDI+ is now used to handle tif and can handle png as well)
		CImage Image(strFileName);

		HGLOBAL hMemory = Image.GetDibHandle();
		if (!hMemory)
			return E_FAIL;

		COLORREF TransparentColor = GetTransparentColor(strFileName);
		lResult = AddImage(hMemory, true/*bIsDIB*/, iOffset, TransparentColor, x, y, bImageDropped);
	}

	return lResult;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::AddImage(HANDLE hMemory, bool bIsDIB, int iOffset, COLORREF TransparentColor, int x, int y, bool bImageDropped)
{
	CWaitCursor Wait;

	if (!hMemory)
		return E_FAIL;

	if (!m_pAGDoc)
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CAGLayer* pActiveLayer = pPage->GetActiveLayer();
	if (!pActiveLayer)
		return E_FAIL;

	// Get the page size minus the non-printable margins
	SIZE PageSize = {0,0};
	pPage->GetPageSize(PageSize);
	RECT PageRect = {0, 0, PageSize.cx, PageSize.cy};
	RECT Margins = {0,0,0,0};
	m_Print.GetMargins(m_pAGDoc, Margins);
	PageRect.left   += max(Margins.left, Margins.right);
	PageRect.top    += max(Margins.top, Margins.bottom);
	PageRect.right  -= max(Margins.left, Margins.right);
	PageRect.bottom -= max(Margins.top, Margins.bottom);

	CAGSym* pSym = NULL;
	if (bIsDIB)
	{
		// do we have a bitmap file header preceding the DIB?
		char* pMemory = (char*)::GlobalLock(hMemory);
		BITMAPFILEHEADER* pBMPFile = (BITMAPFILEHEADER*)pMemory;
		#define BFT_BITMAP	0x4d42	// 'BM'
		if (pBMPFile->bfType == BFT_BITMAP)
			pMemory += sizeof(BITMAPFILEHEADER);

		BITMAPINFOHEADER* pDIB = (BITMAPINFOHEADER*)pMemory;
		if (pDIB->biCompression == BI_RGB)
		{
			bool bHasTransparentColor = ((pDIB->biClrImportant & 0xFF000000) == 0xFF000000);
			if (bHasTransparentColor)
				TransparentColor = pDIB->biClrImportant & 0x00FFFFFF;
			pDIB->biClrImportant = 0;

			CAGSymImage* pSymImage = new CAGSymImage(pDIB, PageRect);
			if (pSymImage && TransparentColor != CLR_NONE)
				pSymImage->SetTransparentColor(TransparentColor);
			pSym = pSymImage;
		}

		::GlobalUnlock(hMemory);
	}
	else
	{
		CAGSymImage* pSymImage = new CAGSymImage(hMemory, PageRect);
		if (pSymImage && TransparentColor != CLR_NONE)
			pSymImage->SetTransparentColor(TransparentColor);
		pSym = pSymImage;
	}

	if (!pSym)
		return E_FAIL;

	POINT pt = {x, y};
	::ScreenToClient(m_hWnd, &pt);
	m_pClientDC->GetViewToDeviceMatrix().Inverse().Transform(pt);
	CAGSym* pAddAPhotoSym = pActiveLayer->FindAddAPhotoSymByPoint(pt, GetHiddenFlag());

	bool bAddAPhoto = (m_Select.SymIsAddAPhoto() || (bImageDropped && pAddAPhotoSym));

	RECT DestRect = pSym->GetDestRect();
	int iAddAPhotoOffset = iOffset;
	if (bAddAPhoto)
	{
		if (!pAddAPhotoSym)	pAddAPhotoSym = m_Select.GetSym();
		RECT AddAPhotoRect = pAddAPhotoSym->GetDestRectTransformed();
		CAGMatrix Matrix;
		Matrix.ScaleToFit(AddAPhotoRect, DestRect, false/*bMin*/);
		Matrix.Transform(DestRect);

		// New Image replaces existing AddAPhotoImage
		// Previous AddAPhotoImage is deleted.
		iAddAPhotoOffset = pActiveLayer->FindSymbol(pAddAPhotoSym);
		DeleteAddAPhotoImage(pActiveLayer, pAddAPhotoSym);

		pSym->SetID(ID_ADDAPHOTO);
	}
	else
	{
		CascadeRect(DestRect);
		m_Grid.Snap(DestRect);
	}

	pSym->SetDestRect(DestRect);

	// Change the symbol
	m_Undo.CheckPoint(IDC_DOC_PASTE, pSym);
	m_pAGDoc->SetModified(true);
	pActiveLayer->AddSymbol(pSym);

	pActiveLayer->OrderSymbol(pSym, iAddAPhotoOffset);

	// Select the newly modified symbol
	SymbolSelect(pSym);

	// If image is being added from Art Gallery then set id and product info.
	CArtGalleryDialog * pArtGallery = m_pCtp->GetArtGalleryDialog();
	if (pArtGallery && pArtGallery->IsProcessing() && pSym)
	{
		CString szProductInfo = m_pCtp->GetArtGalleryDialog()->GetClipArtDownloadInfo();
		pSym->SetID(ID_CLIPART);
		pSym->SetProductInfo(szProductInfo);

		// If this is a border image, maximize it. If necessary, rotate image to match orientation of page.
		if (pArtGallery->IsBorders())
		{
			CAGSymImage* pSymImage = (CAGSymImage*)pSym;
			BITMAPINFOHEADER* pDIB = pSymImage->GetDib();
			if ((pPage->IsPortrait() != pSymImage->IsPortrait()) && pDIB)
			{
				CDib DibSrc = pDIB;
				BITMAPINFOHEADER* pDIBNew = DibSrc.Orient(1/*iRotateDirection*/, false/*bFlipX*/, false/*bFlipY*/);
				if (pDIBNew)
					pSymImage->SetDib(pDIBNew);
			}

			MaximizeSymbolSize(true/*bWidth*/, true/*bHeight*/, true/*bRaw*/);
		}
	}

	CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(SELECTED_PANEL);
	if (pCtlPanel)
		pCtlPanel->UpdateControls();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::DeleteAddAPhotoImage(CAGLayer* pActiveLayer, CAGSym* pAddAPhotoSym)
{
	CAGSym* pSym = pActiveLayer->FindNextSymbol(pAddAPhotoSym, -1, GetHiddenFlag(), false);
	if (!pSym) return;

	if (pSym->IsImage() && !pSym->IsHidden() && !pSym->IsDeleted() && pSym->GetID()==999)
	{
		Clipboard.DeleteObject(pSym);
		if (m_Select.GetSym() == pSym)
			SymbolUnselect(true/*bClearPointer*/);
		m_Undo.CheckPoint(IDC_DOC_DELETE, pSym);
		pSym->SetDeleted(true); // The actual delete happens when the undo list is cleared
		pSym->SetID(ID_NONE);
		SymbolSelect(pAddAPhotoSym);
	}
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocAddAPhoto(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return AddTextbox(ST_ADDAPHOTO);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocAddTextbox(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return AddTextbox(ST_TEXT);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::AddTextbox(SYMTYPE SymType)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CAGLayer* pActiveLayer = pPage->GetActiveLayer();
	if (!pActiveLayer)
		return E_FAIL;

	CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(TEXT_PANEL);
	if (!pCtlPanel)
		return E_FAIL;

	bool bAddAPhoto = (SymType == ST_ADDAPHOTO);
	bool bCalendar = (SymType == ST_CALENDAR);

	CAGSym* pSym = NULL;
	CAGText* pText = NULL;

	if (bCalendar)
	{
		pSym = new CAGSymCalendar();
		pText = (CAGText*)(CAGSymCalendar*)pSym;
	}
	else
	{
		pSym = new CAGSymText(SymType);
		pText = (CAGText*)(CAGSymText*)pSym;
	}
	
	if (!pSym)
		return E_FAIL;

	bool bDocAvery = (m_pAGDoc->GetDocType() == DOC_AVERYPOSTER);

	// Get the page size minus the non-printable margins
	SIZE PageSize = {0,0};
	pPage->GetPageSize(PageSize);
	RECT Margins = {0,0,0,0};
	m_Print.GetMargins(m_pAGDoc, Margins);
	PageSize.cx -= (2 * max(Margins.left, Margins.right));
	PageSize.cy -= (2 * max(Margins.top, Margins.bottom));

	RECT DestRect = {0,0,0,0};
	if (bDocAvery)
	{
		DestRect.right = (bAddAPhoto ? min(PageSize.cx, INCHES(3.5)) : min(PageSize.cx, INCHES(6.0)));
		DestRect.bottom = (bAddAPhoto ? min(PageSize.cy, INCHES(3.0)) : min(PageSize.cy, INCHES(1.5)));
	}
	else
	{
		DestRect.right = (bAddAPhoto ? min(PageSize.cx, INCHES(1.5)) : min(PageSize.cx, INCHES(6.0)));
		DestRect.bottom = min(PageSize.cy, INCHES(1.5));
	}
	DestRect.right -= INCHES(.5);
	CascadeRect(DestRect);
	m_Grid.SnapSize(DestRect);
	pText->Create(DestRect);

	// Setup the default font
	LOGFONT Font;
	::ZeroMemory(&Font, sizeof(Font));
	Font.lfWeight = FW_NORMAL;
	Font.lfCharSet = ANSI_CHARSET; //j DEFAULT_CHARSET
	lstrcpy(Font.lfFaceName, "CAC Futura Casual");

	CToolsPanelText1 *pTopTxtToolsPanel = (CToolsPanelText1*)pCtlPanel->GetTopToolPanel();
	if (!pTopTxtToolsPanel)
		return E_FAIL;

	CToolsPanelText2 *pBotTxtToolsPanel = (CToolsPanelText2*)pCtlPanel->GetBotToolPanel();
	if (!pBotTxtToolsPanel)
		return E_FAIL;

	// Get the font
	if (!bAddAPhoto)
	{
		int nItem = pTopTxtToolsPanel->SendDlgItemMessage(IDC_FONT, CB_GETCURSEL, 0, 0);
		if (nItem >= 0)
		{
			int nFont = pTopTxtToolsPanel->SendDlgItemMessage(IDC_FONT, CB_GETITEMDATA, nItem, 0);
			if (nFont >= 0)
			{
				LOGFONTEXLIST& FontList = m_pCtp->GetFontList().GetInstalledFonts();
				LOGFONT NewFont = FontList[nFont].lf;
				lstrcpy(Font.lfFaceName, NewFont.lfFaceName);
				Font.lfWeight = NewFont.lfWeight;
				Font.lfItalic = NewFont.lfItalic;
				Font.lfCharSet = NewFont.lfCharSet;
				Font.lfOutPrecision = NewFont.lfOutPrecision;
				Font.lfClipPrecision = NewFont.lfClipPrecision;
				Font.lfQuality = NewFont.lfQuality;
				Font.lfPitchAndFamily = NewFont.lfPitchAndFamily;
			}
		}
	}

	// Get the fill and line attributes
	COLORREF FillColor = (!bAddAPhoto ? pBotTxtToolsPanel->GetCurrentFillColor() : RGB(0,0,0));
	COLORREF FillColor2 = (!bAddAPhoto ? pBotTxtToolsPanel->GetCurrentFillColor2() : RGB(0,0,0));
	COLORREF LineColor = (!bAddAPhoto ? pBotTxtToolsPanel->GetCurrentLineColor() : RGB(0,0,0));
	FillType Fill = (!bAddAPhoto ? pBotTxtToolsPanel->GetCurrentFillType() : FT_Solid);
	int LineWidth = (!bAddAPhoto ? pBotTxtToolsPanel->GetCurrentLineWidth() : LT_None);

	// Get the size
	int TextSize = (!bAddAPhoto ? pTopTxtToolsPanel->GetCurrentTextSize() : POINTUNITS(16));
	if (bDocAvery)
	{
		if (TextSize < POINTUNITS(36))
			TextSize = POINTUNITS(36);
	}
	else
	{
		if (TextSize < POINTUNITS(6))
			TextSize = POINTUNITS(6);
	}

	Font.lfHeight = -TextSize;

	// Get the emphasis
	if (!bAddAPhoto)
	{
		if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_BOLD))
			Font.lfWeight = FW_BOLD;
		if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_ITALIC))
			Font.lfItalic = true;
		if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_UNDERLINE))
			Font.lfUnderline = true;
	}

	// Get the vertical justification
	TSVertJustifications eVertJust = eVertCentered;
	if (!bAddAPhoto)
	{
		if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_TOP))
			eVertJust = eVertTop;
		else
		if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_MIDDLE))
			eVertJust = eVertCentered;
		else
		if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_BOTTOM))
			eVertJust = eVertBottom;
	}

	// Get the horizontal justification
	eTSJust eHorzJust = eRagCentered;
	if (!bAddAPhoto)
	{
		if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_LEFT))
			eHorzJust = eFlushLeft;
		else
		if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_CENTER))
			eHorzJust = eRagCentered;
		else
		if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_RIGHT))
			eHorzJust = eFlushRight;
	}

	char* pRawText = (bAddAPhoto ? "Double-Click \nto \nAdd-A-Photo®" : (!bCalendar ? "Your text goes here." : ""));

	// Initialize the type specs
	CAGSpec* pSpecs[] = { new CAGSpec(
		Font,			// LOGFONT& Font
		eHorzJust,		// eTSJust HorzJust
		0,				// short sLineLeadPct
		LineWidth,		// int LineWidth
		LineColor,		// COLORREF LineColor
		Fill,			// FillType Fill
		FillColor,		// COLORREF FillColor
		FillColor2)		// COLORREF FillColor2
	};

	const int pOffsets[] = {0};
	pText->SetText(pRawText, 1, pSpecs, pOffsets);
	pText->SetVertJust(eVertJust);

	// Change the symbol
	m_Undo.CheckPoint(IDC_DOC_ADDTEXTBOX, pSym);
	m_pAGDoc->SetModified(true);
	pActiveLayer->AddSymbol(pSym);

	// Select the newly modified symbol
	SymbolSelect(pSym);

	pCtlPanel->UpdateControls();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocAddCalendar(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CAGLayer* pActiveLayer = pPage->GetActiveLayer();
	if (!pActiveLayer)
		return E_FAIL;

	CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(CALENDAR_PANEL);
	if (!pCtlPanel)
		return E_FAIL;

	bool bDocAvery = (m_pAGDoc->GetDocType() == DOC_AVERYPOSTER);

	// Create the object at the default size
	SIZE Size;
	if (bDocAvery)
	{
		Size.cx = INCHES(10.0);
		Size.cy = INCHES(10.0);
	}
	else
	{
		Size.cx = INCHES(5.0);
		Size.cy = INCHES(5.0);
	}

	CAGSymCalendar* pSym = new CAGSymCalendar();

	// Get the page size minus the non-printable margins
	SIZE PageSize = {0,0};
	pPage->GetPageSize(PageSize);
	RECT Margins = {0,0,0,0};
	m_Print.GetMargins(m_pAGDoc, Margins);
	PageSize.cx -= (2 * max(Margins.left, Margins.right));
	PageSize.cy -= (2 * max(Margins.top, Margins.bottom));

	RECT DestRect = {0,0,0,0};
	DestRect.right = min(PageSize.cx, Size.cx);
	DestRect.bottom = min(PageSize.cy, Size.cy);

	CascadeRect(DestRect);
	m_Grid.SnapSize(DestRect);

	pSym->SetDestRect(DestRect);

	// bpp seems to be not needed 10/12/2005
	// Initialize the symbol
	//pSym->SetLineColor(pCtlPanel->GetCurrentLineColor());
	//pSym->SetFillColor(pCtlPanel->GetCurrentFillColor());
	//pSym->SetFillColor2(pCtlPanel->GetCurrentFillColor2());
	//pSym->SetFillColor3(pCtlPanel->GetCurrentFillColor3());

	//int LineWidth = pCtlPanel->GetCurrentLineWidth();
	//pSym->SetLineWidth(LineWidth == LT_None ? LT_Hairline : LineWidth);
	if (bDocAvery)
	{
		int mSize = POINTUNITS(48);
		int wSize = POINTUNITS(36);
		int dSize = POINTUNITS(36);
		pSym->SetSpecTextSizes(mSize, wSize, dSize);
		pSym->SetLineWidth(POINTUNITS(3));
	}

	// Change the symbol
	m_Undo.CheckPoint(IDC_DOC_ADDCALENDAR, pSym);
	m_pAGDoc->SetModified(true);
	pActiveLayer->AddSymbol(pSym);

	// Select the newly modified symbol
	SymbolSelect(pSym);

	pCtlPanel->UpdateControls();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocAddShape(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	CHtmlDialog dlg("ShapeSelect.htm", true/*bRes*/, ::GetActiveWindow());
	dlg.SetWindowSize((10*41) + 53, (6*41) + 128); // 10 x 6 (icons are 41x41)
	dlg.SetResizable(true);
	dlg.SetScrollbars(false);
	if (!dlg.DoModal())
		return S_OK;

	CString szSymbol;
	CString szFillValue;
	dlg.GetControlValue("FillValue", szFillValue);
	dlg.GetControlValue("Symbol", szSymbol);

	//If the user cancels, the string will be empty
	if (szSymbol.IsEmpty())
		return S_OK;

	bool fFilled = ("true" == szFillValue);
	return AddShape(szSymbol, fFilled);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::AddShape(LPCSTR strShapeName, bool bFilled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CAGLayer* pActiveLayer = pPage->GetActiveLayer();
	if (!pActiveLayer)
		return E_FAIL;

	CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(GRAPHICS_PANEL);
	if (!pCtlPanel)
		return E_FAIL;

	bool bDocAvery = (m_pAGDoc->GetDocType() == DOC_AVERYPOSTER);
	SIZE Size;

	// Create the object at the default size
	if (bDocAvery)
	{
		Size.cx = INCHES(4.0);
		Size.cy = INCHES(4.0);
	}
	else
	{
		Size.cx = INCHES(2.0);
		Size.cy = INCHES(2.0);
	}

	CAGSymGraphic* pSym = NULL;
	if (!_stricmp(strShapeName, "Line"))
	{
		CAGSymLine* pSymLine = new CAGSymLine();
		pSym = pSymLine;
		Size.cx = INCHES(3);
		Size.cy = 2;
		bFilled = false;
	}
	else
	if (!_stricmp(strShapeName, "Square"))
	{
		CAGSymRectangle* pSymRectangle = new CAGSymRectangle();
		pSym = pSymRectangle;
		Size.cx = INCHES(1.5);
		Size.cy = INCHES(1.5);
	}
	else
	if (!_stricmp(strShapeName, "Circle"))
	{
		CAGSymEllipse* pSymEllipse = new CAGSymEllipse();
		pSym = pSymEllipse;
		Size.cx = INCHES(1.5);
		Size.cy = INCHES(1.5);
	}

	// Get the page size minus the non-printable margins
	SIZE PageSize = {0,0};
	pPage->GetPageSize(PageSize);
	RECT Margins = {0,0,0,0};
	m_Print.GetMargins(m_pAGDoc, Margins);
	PageSize.cx -= (2 * max(Margins.left, Margins.right));
	PageSize.cy -= (2 * max(Margins.top, Margins.bottom));

	RECT DestRect = {0,0,0,0};
	DestRect.right = min(PageSize.cx, Size.cx);
	DestRect.bottom = min(PageSize.cy, Size.cy);

	CascadeRect(DestRect);
	m_Grid.SnapSize(DestRect);

	if (pSym) 
		pSym->SetDestRect(DestRect);
	else
	{ // Generate the new symbol
		pSym = (CAGSymGraphic*)CShape::GenerateSymbol(strShapeName, DestRect);
		if (!pSym)
			return E_FAIL;
	}

	CToolsPanelGraphics2* pBotGraphicsToolsPanel = (CToolsPanelGraphics2*)pCtlPanel->GetBotToolPanel();
	if (!pBotGraphicsToolsPanel)
		return E_FAIL;

	// Initialize the symbol
	pSym->SetLineColor(pBotGraphicsToolsPanel->GetCurrentLineColor());
	pSym->SetFillColor(pBotGraphicsToolsPanel->GetCurrentFillColor());
	pSym->SetFillColor2(pBotGraphicsToolsPanel->GetCurrentFillColor2());

	if (bFilled)
	{
		pSym->SetLineWidth(pBotGraphicsToolsPanel->GetCurrentLineWidth());
		pSym->SetFillType(FT_Solid);
	}
	else
	{
		int LineWidth = pBotGraphicsToolsPanel->GetCurrentLineWidth();
		pSym->SetLineWidth(LineWidth == LT_None ? LT_Hairline : LineWidth);
		pSym->SetFillType(FT_None);
	}

	// Change the symbol
	m_Undo.CheckPoint(IDC_DOC_ADDSHAPE, pSym);
	m_pAGDoc->SetModified(true);
	pActiveLayer->AddSymbol(pSym);

	// Select the newly modified symbol
	SymbolSelect(pSym);

	pCtlPanel->UpdateControls();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocTextRotate(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
    CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (!m_pClientDC)
		return E_FAIL;

	if (m_Select.NoSymSelected())
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	if (m_Select.SymIsLocked())
	{
		CMessageBox::ObjectIsLocked();
		return E_FAIL;
	}

	m_Select.StartTextRotate();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocTransform(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (!m_pClientDC)
		return E_FAIL;

	if (m_Select.NoSymSelected())
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	if (m_Select.SymIsLocked())
	{
		CMessageBox::ObjectIsLocked();
		return E_FAIL;
	}

	m_Select.StartTransform();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocCrop(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (!m_pClientDC)
		return E_FAIL;

	if (m_Select.NoSymSelected())
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	if (m_Select.SymIsLocked())
	{
		CMessageBox::ObjectIsLocked();
		return E_FAIL;
	}

	m_Select.StartCrop();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoAdd(CAGSym* pSym)
{
	CWaitCursor Wait;
	
	if (!pSym)
		return E_FAIL;

	// Invalidate the symbol's current location, since this symbol is going away...
	SymbolInvalidate(pSym);

	// Change the symbol
	m_pAGDoc->SetModified(true);
	pSym->SetDeleted(true);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoDelete(CAGSym* pSym)
{
	CWaitCursor Wait;
	
	if (!pSym)
		return E_FAIL;;

	// Change the symbol
	m_pAGDoc->SetModified(true);
	pSym->SetDeleted(false);

	// The modified symbol will be selected on the way out
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoPermDelete(CAGSym* pSym)
{
	CWaitCursor Wait;
	
	if (!pSym)
		return E_FAIL;

	// Only deleted symbols will be permanently deleted
	if (!pSym->IsDeleted())
		return E_FAIL;

	if (!m_pAGDoc)
		return E_FAIL;

	CAGPage* pPage = NULL;
	CAGLayer* pLayer = NULL;
	if (m_pAGDoc->FindSymbolAnywhere(pSym, &pPage, &pLayer) < 0)
		return E_FAIL;

	if (!pLayer)
		return E_FAIL;

	// Change the symbol
	m_pAGDoc->SetModified(true);
	m_Undo.DereferenceItem(pSym);
	pLayer->DeleteSymbol(pSym);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoMirrorFlip(CAGSym* pSym, bool bFlipX, bool bFlipy)
{
	return MirrorFlip(pSym, UNDO_COMMAND, bFlipX, bFlipy);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoRotate(CAGSym* pSym, bool bCW)
{
	return Rotate(pSym, UNDO_COMMAND, bCW);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoLayer(CAGSym* pSym, int nOffset)
{
	return Layer(pSym, UNDO_COMMAND, nOffset);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoTransform(CAGSym* pSym, CAGMatrix Matrix)
{
	CWaitCursor Wait;
	
	if (!pSym)
		return E_FAIL;

	if (!m_pAGDoc)
		return E_FAIL;

	// Invalidate the symbol's current location, since this symbol is changing size location, or layering...
	SymbolInvalidate(pSym);

	// Change the symbol
	m_pAGDoc->SetModified(true);
	pSym->SetMatrix(Matrix);

	// The modified symbol will be selected on the way out
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoDestRect(CAGSym* pSym, RECT& DestRect)
{
	CWaitCursor Wait;
	
	if (!pSym)
		return E_FAIL;

	if (!m_pAGDoc)
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	// Invalidate the symbol's current location, since this symbol is changing size location, or layering...
	SymbolInvalidate(pSym);

	// Change the symbol
	m_pAGDoc->SetModified(true);
	pSym->SetDestRect(DestRect);

	// The modified symbol will be selected on the way out
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoCropRect(CAGSym* pSym, RECT& CropRect)
{
	CWaitCursor Wait;
	
	if (!pSym)
		return E_FAIL;

	if (!m_pAGDoc)
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	// Invalidate the symbol's current location, since this symbol is changing size location, or layering...
	SymbolInvalidate(pSym);

	// Change the symbol
	m_pAGDoc->SetModified(true);
	CAGSymImage* pSymImage = (CAGSymImage*)pSym;
	pSymImage->SetCropRect(CropRect);

	// The modified symbol will be selected on the way out
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoColor(CAGSym* pSym, COLORREF Color, bool bLineColor, bool bFillColor2)
{
	CWaitCursor Wait;
	
	if (!pSym)
		return E_FAIL;

	if (!pSym->IsGraphic())
		return E_FAIL;

	if (!m_pAGDoc)
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;

	// Change the symbol
	m_pAGDoc->SetModified(true);
	if (bLineColor)
		pGraphicSym->SetLineColor(Color);
	else
	if (bFillColor2)
		pGraphicSym->SetFillColor2(Color);
	else
		pGraphicSym->SetFillColor(Color);

	// The modified symbol will be selected on the way out
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoLineWidth(CAGSym* pSym, int nLineWidth, int iUnused)
{
	CWaitCursor Wait;
	
	if (!pSym)
		return E_FAIL;

	if (!pSym->IsGraphic())
		return E_FAIL;

	if (!m_pAGDoc)
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	// Invalidate the symbol's current location, since this symbol is changing size location, or layering...
	SymbolInvalidate(pSym);

	CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;

	// Change the symbol
	m_pAGDoc->SetModified(true);
	pGraphicSym->SetLineWidth(nLineWidth);

	// The modified symbol will be selected on the way out
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoFillType(CAGSym* pSym, int nFillType)
{
	CWaitCursor Wait;
	
	if (!pSym)
		return E_FAIL;

	if (!pSym->IsGraphic())
		return E_FAIL;

	if (!m_pAGDoc)
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;

	// Change the symbol
	m_pAGDoc->SetModified(true);
	pGraphicSym->SetFillType((FillType)nFillType);

	// The modified symbol will be selected on the way out
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoTextEdit(CUndoItem* pItem)
{
	CWaitCursor Wait;
	
	if (!m_pAGDoc)
		return E_FAIL;

	CAGSymText* pSym = (CAGSymText*)pItem->GetUndoItem();
	if (!pSym)
		return E_FAIL;

	CAGSymText* pUndoCopy = (CAGSymText*)pItem->GetUndoCopy();
	if (!pUndoCopy)
		return E_FAIL;

	// Invalidate the symbol's current location, since this symbol is changing size location, or layering...
	SymbolInvalidate(pSym);

	// Change the symbol
	m_pAGDoc->SetModified(true);
	pSym->Swap(pUndoCopy);

	// The modified symbol will be selected on the way out
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoPageTurn(int nPage)
{
	CWaitCursor Wait;
	
	CPagePanel* pPagePanel = m_pCtp->GetPagePanel();
	if (!pPagePanel)
		return E_FAIL;

	if (nPage < 0)
		return E_FAIL;

	pPagePanel->m_PageNavigator.SetPage(nPage);
	pPagePanel->PageChange();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoLocked(CAGSym* pSym)
{
	CWaitCursor Wait;
	
	// Change the symbol
	m_pAGDoc->SetModified(true);
	bool bLocked = ((CAGSymImage*)pSym)->IsLocked();
	((CAGSymImage*)pSym)->SetLocked(!bLocked);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoHidden(CAGSym* pSym)
{
	CWaitCursor Wait;
	
	// Change the symbol
	m_pAGDoc->SetModified(true);
	bool bHidden = pSym->IsHidden();
	pSym->SetHidden(!bHidden);

	if (!bHidden)
		// Invalidate the symbol's current location, since this symbol is changing size location, or layering...
		SymbolInvalidate(pSym);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::UndoTransparent(CAGSym* pSym)
{
	CWaitCursor Wait;
	
	// Change the symbol
	m_pAGDoc->SetModified(true);
	bool bTransparent = (((CAGSymImage*)pSym)->GetTransparentColor() != CLR_NONE);
	((CAGSymImage*)pSym)->SetTransparent(!bTransparent);

	// The modified symbol will be selected on the way out
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocUndo(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	m_Undo.OnEditUndo();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocRedo(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	m_Undo.OnEditRedo();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::AddBitmap(HBITMAP hBitmap, HPALETTE hPal)
{
	if (!hBitmap)
		return E_FAIL;

	HGLOBAL hMemory = BitmapToDib(NULL/*hDC*/, hBitmap, hPal);
	if (!hMemory)
		return E_FAIL;

	LRESULT lResult = AddImage(hMemory, true/*bIsDIB*/, 0/*iOffset*/);
	::GlobalFree(hMemory);
	return lResult;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::PasteText(HANDLE hMemory)
{
	if (!hMemory)
		return E_FAIL;

	if (!m_pAGDoc)
		return E_FAIL;

	CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(TEXT_PANEL);
	if (!pCtlPanel)
		return E_FAIL;

	CToolsPanelText1* pTopTxtToolsPanel = (CToolsPanelText1*)pCtlPanel->GetTopToolPanel();
	if (!pTopTxtToolsPanel)
		return E_FAIL;

	// Setup the default font
	LOGFONT Font;
	::ZeroMemory(&Font, sizeof(Font));
	Font.lfWeight = FW_NORMAL;
	Font.lfCharSet = ANSI_CHARSET; //j DEFAULT_CHARSET
	lstrcpy(Font.lfFaceName, "CAC Futura Casual");

	// Get the font
	int nItem = pTopTxtToolsPanel->SendDlgItemMessage(IDC_FONT, CB_GETCURSEL, 0, 0);
	if (nItem >= 0)
	{
		int nFont = pTopTxtToolsPanel->SendDlgItemMessage(IDC_FONT, CB_GETITEMDATA, nItem, 0);
		if (nFont >= 0)
		{
			LOGFONTEXLIST& FontList = m_pCtp->GetFontList().GetInstalledFonts();
			LOGFONT NewFont = FontList[nFont].lf;
			lstrcpy(Font.lfFaceName, NewFont.lfFaceName);
			Font.lfWeight = NewFont.lfWeight;
			Font.lfItalic = NewFont.lfItalic;
			Font.lfCharSet = NewFont.lfCharSet;
			Font.lfOutPrecision = NewFont.lfOutPrecision;
			Font.lfClipPrecision = NewFont.lfClipPrecision;
			Font.lfQuality = NewFont.lfQuality;
			Font.lfPitchAndFamily = NewFont.lfPitchAndFamily;
		}
	}

	// Get the fill and line attributes
	CToolsPanelText2* pBotTxtToolsPanel = (CToolsPanelText2*)pCtlPanel->GetBotToolPanel();
	if (!pBotTxtToolsPanel)
		return E_FAIL;

	COLORREF FillColor = pBotTxtToolsPanel->GetCurrentFillColor();
	COLORREF FillColor2 = pBotTxtToolsPanel->GetCurrentFillColor2();
	COLORREF LineColor = pBotTxtToolsPanel->GetCurrentLineColor();
	FillType Fill = pBotTxtToolsPanel->GetCurrentFillType();
	int LineWidth = pBotTxtToolsPanel->GetCurrentLineWidth();

	// Get the size
	int TextSize = pTopTxtToolsPanel->GetCurrentTextSize();
	if (TextSize < POINTUNITS(6))
		TextSize = POINTUNITS(6);

	Font.lfHeight = -TextSize;

	// Get the emphasis
	if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_BOLD))
		Font.lfWeight = FW_BOLD;
	if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_ITALIC))
		Font.lfItalic = true;
	if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_UNDERLINE))
		Font.lfUnderline = true;

	// Get the vertical justification
	TSVertJustifications eVertJust = eVertCentered;
	if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_TOP))
		eVertJust = eVertTop;
	else
	if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_MIDDLE))
		eVertJust = eVertCentered;
	else
	if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_BOTTOM))
		eVertJust = eVertBottom;

	// Get the horizontal justification
	eTSJust eHorzJust = eRagCentered;
	if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_LEFT))
		eHorzJust = eFlushLeft;
	else
	if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_CENTER))
		eHorzJust = eRagCentered;
	else
	if (pTopTxtToolsPanel->IsDlgButtonChecked(IDC_RIGHT))
		eHorzJust = eFlushRight;

	// Get the raw text
	char* pRawText = (char*)::GlobalLock(hMemory);

	// Strip out returns (leave line feeds '\n' in)
	int iOut = 0;
	for (int i = 0; i < lstrlen(pRawText); i++)
	{
		if (pRawText[i] == '\r')
			continue;
		pRawText[iOut++] = pRawText[i];
	}

	pRawText[iOut++] = '\0';

	// Initialize the type specs
	CAGSpec* pSpecs[] = { new CAGSpec(
		Font,			// LOGFONT& Font
		eHorzJust,		// eTSJust HorzJust
		0,				// short sLineLeadPct
		LineWidth,		// int LineWidth
		LineColor,		// COLORREF LineColor
		Fill,			// FillType Fill
		FillColor,		// COLORREF FillColor
		FillColor2)		// COLORREF FillColor2
	};

	if (m_Select.SymIsText() || (m_Select.SymIsAddAPhoto() && SpecialsEnabled()))
	{
		m_Select.GetTextSym()->PasteText(pRawText, pSpecs[0]);
		m_pAGDoc->SetModified(true);
	}
	else
	{
		CAGPage* pPage = m_pAGDoc->GetCurrentPage();
		if (!pPage)
			return E_FAIL;

		CAGLayer* pActiveLayer = pPage->GetActiveLayer();
		if (!pActiveLayer)
			return E_FAIL;

		CAGSymText* pSym = new CAGSymText(ST_TEXT);
		if (!pSym)
			return E_FAIL;

		// Get the page size minus the non-printable margins
		SIZE PageSize = {0,0};
		pPage->GetPageSize(PageSize);
		RECT Margins = {0,0,0,0};
		m_Print.GetMargins(m_pAGDoc, Margins);
		PageSize.cx -= (2 * max(Margins.left, Margins.right));
		PageSize.cy -= (2 * max(Margins.top, Margins.bottom));

		RECT DestRect = {0,0,0,0};
		DestRect.right = min(PageSize.cx, INCHES(6.0));
		DestRect.bottom = min(PageSize.cy, INCHES(1.5));
		CascadeRect(DestRect);
		m_Grid.Snap(DestRect);
		pSym->Create(DestRect);
		pSym->SetVertJust(eVertJust);
		const int pOffsets[] = {0};
		pSym->SetText(pRawText, 1, pSpecs, pOffsets);

		// Change the symbol
		m_Undo.CheckPoint(IDC_DOC_PASTE, pSym);
		m_pAGDoc->SetModified(true);
		pActiveLayer->AddSymbol(pSym);

		// Select the newly modified symbol
		SymbolSelect(pSym);

		pCtlPanel->UpdateControls();
	}

	::GlobalUnlock(hMemory);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocCut(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;
	
	return DeleteSymbol(true/*bCut*/, id);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocDelete(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;
	
	return DeleteSymbol(false/*bCut*/, id);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::DeleteSymbol(bool bCut, UINT nCommand)
{
	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	// If the envelope wizard is active we need to add the new image to the picture list
	if (m_pEMDlg)
	{
		int SymID = m_Select.GetSym()->GetID();
		if (SymID == ID_ENVELOPE_SEND || SymID == ID_ENVELOPE_RETURN)
		{
			CMessageBox::Message(String("Sorry.  You cannot delete this object."));
			return E_FAIL;
		}

		//if (m_pSym->GetType() == ST_IMAGE)
		//	m_pEMDlg->DeleteGraphicSymbol((CAGSymImage*)m_pSym);
	}

	// Let the clipboard object know what we are doing with this symbol
	bool bDeleteSymbol = true;
	if (bCut)
		bDeleteSymbol = Clipboard.CutObject(m_Select.GetSym());
	else
		Clipboard.DeleteObject(m_Select.GetSym());

	if (!bDeleteSymbol)
		return S_OK;

	CAGSym* pSym = m_Select.GetSym();

	// Unselect the current symbol, because it is going away
	SymbolUnselect(true/*bClearPointer*/);

	// Change the symbol
	m_Undo.CheckPoint(nCommand, pSym);
	m_pAGDoc->SetModified(true);
	pSym->SetDeleted(true); // The actual delete happens when the undo list is cleared

	// Find the symbol beneath the one being deleted
	CAGLayer* pLayer = NULL;
	if (m_pAGDoc->FindSymbolAnywhere(pSym, NULL, &pLayer) < 0)
		return S_OK;
	if (!pLayer)
		return S_OK;

	// Select the symbol beneath the one being deleted
	CAGSym* pSymNext = pLayer->FindNextSymbol(pSym, +1/*iDirection*/, GetHiddenFlag(), !SpecialsEnabled()/*bIgnoreLocked*/);
	if (pSymNext)
		SymbolSelect(pSymNext);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocCopy(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	Clipboard.CopyObject(m_Select.GetSym());

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocPaste(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	// Handle the pasting of our own objects
	CAGSym* pSym = Clipboard.PasteObject();
	if (pSym)
	{	// We got an object to paste
		CAGPage* pPage = m_pAGDoc->GetCurrentPage();
		if (!pPage)
			return E_FAIL;

		CAGLayer* pActiveLayer = pPage->GetActiveLayer();
		if (!pActiveLayer)
			return E_FAIL;

		// Change the symbol
		m_Undo.CheckPoint(IDC_DOC_PASTE, pSym);
		m_pAGDoc->SetModified(true);

		CAGMatrix Matrix = pSym->GetMatrix();
		bool bRotated = Matrix.IsRotated();
		RECT DestRect = (bRotated ? pSym->GetDestRect() : pSym->GetDestRectTransformed());
		CascadeRect(DestRect);
		m_Grid.Snap(DestRect);
		pSym->SetDestRect(DestRect);
		if (!bRotated)
			pSym->SetMatrix(CAGMatrix());

		pActiveLayer->AddSymbol(pSym);

		// Select the newly modified symbol
		SymbolSelect(pSym);

		CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(SELECTED_PANEL);
		if (pCtlPanel)
			pCtlPanel->UpdateControls();

		return S_OK;
	}

	// Handle the generic clipboard object types
	if (!::OpenClipboard(m_hWnd))
		return E_FAIL;

	HANDLE hMemory = NULL;
	LRESULT lSuccess = E_FAIL;
	if (::IsClipboardFormatAvailable(CF_TEXT))
	{
		if (hMemory = ::GetClipboardData(CF_TEXT))
			lSuccess = PasteText(hMemory);
	}
	else
	if (::IsClipboardFormatAvailable(CF_DIB))
	{
		if (hMemory = ::GetClipboardData(CF_DIB))
			lSuccess = AddImage(hMemory, true/*bIsDIB*/, 0/*iOffset*/);
	}
	else
	if (::IsClipboardFormatAvailable(CF_BITMAP))
	{
		if (hMemory = ::GetClipboardData(CF_BITMAP))
		{
			HPALETTE hPal = (HPALETTE)::GetClipboardData(CF_PALETTE);
			lSuccess = AddBitmap((HBITMAP)hMemory, hPal);
		}
	}

	::CloseClipboard();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::MirrorFlip(CAGSym* pSym, UINT nCommand, bool bFlipX, bool bFlipY)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (!pSym || pSym->IsHidden(GetHiddenFlag()))
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	if (pSym->IsImage() && ((CAGSymImage*)pSym)->IsLocked())
	{
		CMessageBox::ObjectIsLocked();
		return E_FAIL;
	}

	if (pSym->IsText() || pSym->IsAddAPhoto())
	{
		CMessageBox::Message(String("Sorry.  You cannot flip the contents of a text object."));
		return E_FAIL;
	}

	if (pSym->IsCalendar())
	{
		CMessageBox::Message(String("Sorry.  You cannot flip the contents of a calendar object."));
		return E_FAIL;
	}

	// Unselect the current symbol, since we are changing the dest rect or the transform matrix
	SymbolUnselect(true/*bClearPointer*/);

	// Invalidate the symbol's current location, since this symbol is changing size location, or layering...
	SymbolInvalidate(pSym);

	// Change the symbol
	BITMAPINFOHEADER* pDIBNew = NULL;
	if (pSym->IsImage())
	{
		CAGSymImage* pSymImage = (CAGSymImage*)pSym;
		BITMAPINFOHEADER* pDIB = pSymImage->GetDib();
		if (!pDIB)
			return E_FAIL;

		CDib DibSrc = pDIB;
		pDIBNew = DibSrc.Orient(0/*iRotateDirection*/, bFlipX, bFlipY);
		if (!pDIBNew)
			return E_FAIL;
		if (!pSymImage->CanChangeOrientation())
			pSymImage->OrientChangeState(0/*iRotateDirection*/, bFlipX, bFlipY);
	}

	if (nCommand != UNDO_COMMAND)
		m_Undo.CheckPoint(nCommand, pSym);
	m_pAGDoc->SetModified(true);

	if (pDIBNew)
	{
		CAGSymImage* pSymImage = (CAGSymImage*)pSym;
		pSymImage->SetDib(pDIBNew);
	}
	else
	{
		// Adjust the symbol's matrix
		RECT DestRect = pSym->GetDestRect();
		CAGMatrix Matrix = pSym->GetMatrix();
		Matrix.Transform(DestRect);
		Matrix.Scale((bFlipX ? -1 : 1), (bFlipY ? -1 : 1),
			(DestRect.left + DestRect.right) / 2,
			(DestRect.top + DestRect.bottom) / 2);

		pSym->SetMatrix(Matrix);
	}

	// Select the newly modified symbol
	SymbolSelect(pSym);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocFlip(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return MirrorFlip(m_Select.GetSym(), id, false/*bFlipX*/, true/*bFlipy*/);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocMirror(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return MirrorFlip(m_Select.GetSym(), id, true/*bFlipX*/, false/*bFlipy*/);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::Rotate(CAGSym* pSym, UINT nCommand, bool bCW)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (!pSym || pSym->IsHidden(GetHiddenFlag()))
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	if (pSym->IsImage() && ((CAGSymImage*)pSym)->IsLocked())
	{
		CMessageBox::ObjectIsLocked();
		return E_FAIL;
	}

	// Unselect the current symbol, since we are changing the dest rect or the transform matrix
	SymbolUnselect(true/*bClearPointer*/);

	// Invalidate the symbol's current location, since this symbol is changing size location, or layering...
	SymbolInvalidate(pSym);

	// Change the symbol
	if (nCommand != UNDO_COMMAND)
		m_Undo.CheckPoint(nCommand, pSym);
	m_pAGDoc->SetModified(true);

	// Adjust the symbol's matrix
	RECT DestRect = pSym->GetDestRect();
	CAGMatrix Matrix = pSym->GetMatrix();
	Matrix.Transform(DestRect);
	double fAngle = (bCW ? 90 : -90);
	Matrix.Rotate(fAngle, fAngle,
		(DestRect.left + DestRect.right) / 2,
		(DestRect.top + DestRect.bottom) / 2);

	pSym->SetMatrix(Matrix);

	// Select the newly modified symbol
	SymbolSelect(pSym);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocRotateCW(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return Rotate(m_Select.GetSym(), id, true/*bCW*/);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocRotateCCW(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return Rotate(m_Select.GetSym(), id, false/*bCW*/);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocLocked(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	if (!m_Select.SymIsImage())
		return E_FAIL;

	CAGSymImage* pSym = m_Select.GetImageSym();

	// Change the symbol
	m_Undo.CheckPoint(IDC_DOC_LOCKED, pSym);
	m_pAGDoc->SetModified(true);
	pSym->SetLocked(!pSym->IsLocked());

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocTransparent(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	if (!m_Select.SymIsImage())
		return E_FAIL;

	CAGSymImage* pSym = m_Select.GetImageSym();

	// Change the symbol
	m_Undo.CheckPoint(IDC_DOC_TRANSPARENT, pSym);
	m_pAGDoc->SetModified(true);
	bool bTransparent = (pSym->GetTransparentColor() != CLR_NONE);
	pSym->SetTransparent(!bTransparent);

	// Select the newly modified symbol
	SymbolSelect(pSym);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::Zoom(UINT nCommand, int iOffset)
{
	if (!m_pAGDoc)
		return E_FAIL;

	if (!iOffset) // Zoom full
		m_iZoom = 0;
	else
		m_iZoom += iOffset;

	if (m_iZoom >= 0)
		m_fZoomScale = 1.0 + m_iZoom;
	else
	if (m_iZoom < 0)
	{
		m_iZoom = -m_iZoom;
		double iZoomList[] = {1.0, 0.75, 0.67, 0.5, 0.33, 0.25};
		int nMaxZoom = (sizeof(iZoomList) / sizeof(double)) - 1;
		if (m_iZoom > nMaxZoom)
			m_iZoom = nMaxZoom; 

		m_fZoomScale = iZoomList[m_iZoom];
		m_iZoom = -m_iZoom;
	}

	SetupPageView(true/*bSetScrollbars*/, true/*bOffset*/);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::ZoomEx(UINT nZoomLevel, double fZoomScale)
{
	m_fZoomScale = fZoomScale;
	m_iZoom = nZoomLevel;

	SetupPageView(true/*bSetScrollbars*/, true/*bOffset*/);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomIn(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return Zoom(id, +1);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomOut(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return Zoom(id, -1);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomLevel5Out(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return ZoomEx(-5, 0.25);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomLevel4Out(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return ZoomEx(-4, 0.33);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomLevel3Out(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return ZoomEx(-3, 0.5);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomLevel2Out(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return ZoomEx(-2, 0.67);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomLevel1Out(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return ZoomEx(-1, 0.75);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomLevel0(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return ZoomEx(0, 1.0);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomLevelPageWidth(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return ZoomEx(0, 0.0); // Page width zoom passes a scale of 0.0
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomLevelSelection(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return ZoomEx(0, -1.0); // Selection width zoom passes a scale of -1.0
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomLevel2(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return ZoomEx(2-1, 2.0);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomLevel3(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return ZoomEx(3-1, 3.0);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomLevel4(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return ZoomEx(4-1, 4.0);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomLevel5(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return ZoomEx(5-1, 5.0);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnZoomFull(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	return Zoom(id, 0);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocHelp(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (m_pHelpWindow)
	{
		VARIANT_BOOL vbClosed = false;
		HRESULT hr = m_pHelpWindow->get_closed(&vbClosed);
		if (vbClosed)
			m_pHelpWindow = NULL;
		else
		{
			m_pHelpWindow->focus();
			return S_OK;
		}
	}
	
	CString strCpHost = m_pCtp->GetContextCpHost();
	bool bHttp = (strCpHost.Find("http://") >= 0);
	CString strSrcURL = strCpHost + (bHttp ? "help/cphelp.htm" : "cphelp.htm");

	CHtmlDialog dlg(strSrcURL, false/*bRes*/, ::GetActiveWindow());
	dlg.SetWindowSize(500, 600);
	dlg.SetResizable(true);
	dlg.SetScrollbars(false);
	m_pHelpWindow = dlg.DoModeless();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnTextLineSpacing(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	//Check for text sym. If none, happily return
    CAGText* pText = GetText();
	if (pText == NULL)
		return S_OK;

	//Found one, lets operate
	switch (id)
	{
		case IDC_LOOSEN_LINE:
        {
			if (pText)
				pText->SetLineSpacing(30);
			break;
		}
		case IDC_TIGHTEN_LINES:
		{
			if (pText)
				pText->SetLineSpacing(-30);
			break;
		}
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnTextAlign(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
    CAGText* pText = GetText();
    CAGSymCalendar* pCalendar = GetCalendar();

	switch (id)
	{
		case IDC_HORZTEXT_LEFT:
            {
			    if (pText)
				    pText->SetHorzJust(eFlushLeft);
                else if (pCalendar)
			    {
				    pCalendar->SetSpecHorzJust(eFlushLeft);
				    SymbolInvalidate(pCalendar);
			    }
			    break;
            }
		case IDC_HORZTEXT_CENTER:
            {
			    if (pText)
				    pText->SetHorzJust(eRagCentered);
                else if (pCalendar)
			    {
				    pCalendar->SetSpecHorzJust(eRagCentered);
				    SymbolInvalidate(pCalendar);
			    }
			    break;
            }
		case IDC_HORZTEXT_RIGHT:
            {
			    if (pText)
				    pText->SetHorzJust(eFlushRight);
                else if (pCalendar)
			    {
				    pCalendar->SetSpecHorzJust(eFlushRight);
				    SymbolInvalidate(pCalendar);
			    }
			    break;
            }
        case IDC_VERTTEXT_TOP:
            {
			    if (pText)
				    pText->SetVertJust(eVertTop);
                else if (pCalendar)
			    {
				    pCalendar->SetSpecVertJust(eVertTop);
				    SymbolInvalidate(pCalendar);
			    }
			    break;
            }
		case IDC_VERTTEXT_MIDDLE:
            {
			    if (pText)
				    pText->SetVertJust(eVertCentered);
                else if (pCalendar)
			    {
				    pCalendar->SetSpecVertJust(eVertCentered);
				    SymbolInvalidate(pCalendar);
			    }
			    break;
            }
		case IDC_VERTTEXT_BOTTOM:
            {
			    if (pText)
				    pText->SetVertJust(eVertBottom);
                else if (pCalendar)
			    {
				    pCalendar->SetSpecVertJust(eVertBottom);
				    SymbolInvalidate(pCalendar);
			    }
			    break;
            }
        default:
            break;
	}

	SetFocus();

    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnTextCharSpacing(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	//Check for text sym. If none, happily return
    CAGText* pText = GetText();
	if (pText == NULL)
		return S_OK;

	//Found one, lets operate
	switch (id)
	{
		case IDC_LOOSEN_CHARS:
        {
			if (pText)
				pText->SetCharSpacing(20);
			break;
		}
		case IDC_TIGHTEN_CHARS:
		{
			if (pText)
				pText->SetCharSpacing(-20);
			break;
		}
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnTBFile(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	m_popupFile.EnableMenuItem(IDC_DOC_OPEN,          MF_BYCOMMAND | MF_ENABLED);
	m_popupFile.EnableMenuItem(IDC_DOC_CLOSE,         MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	m_popupFile.EnableMenuItem(IDC_DOC_SAVE,          MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	m_popupFile.EnableMenuItem(IDC_DOC_SAVEAS,        MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	m_popupFile.EnableMenuItem(IDC_DOC_PRINT,         MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));


    CRect rParent;
	::GetWindowRect(GetParent(), rParent);
	int posx = 15 - WORKSPACELEFT_OFFSET;
	int posy = WORKSPACETOP_OFFSET;
	CPoint ptButton(posx, 0);
	MapWindowPoints(HWND_DESKTOP, &ptButton, 1);
    int x = ptButton.x;
	int y = rParent.top - posy;
	
	m_bIgnoreSetCursor = true;
	m_popupFile.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, x, y, m_hWnd);
    ::UpdateWindow(m_pCtp->GetBrowser());
	m_bIgnoreSetCursor = false;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnTBEdit(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CMenu Menu;
	bool bOK = !!Menu.LoadMenu(IDR_POPUPMENU_EDIT);
	if (!bOK)
		return E_FAIL;

	CMenu Popup(Menu.GetSubMenu(0));
	if (!Popup.IsMenu())
		return E_FAIL;
	
    bool bSymSelected = m_Select.SymSelected();
    Popup.EnableMenuItem(IDC_DOC_COPY,          MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
    Popup.EnableMenuItem(IDC_DOC_CUT,           MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_PASTE,         MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_DELETE,        MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_UNDO,          MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_REDO,          MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
    Popup.EnableMenuItem(8, MF_BYPOSITION | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));


	 CRect rParent;
	::GetWindowRect(GetParent(), rParent);
	int posx = 65 - WORKSPACELEFT_OFFSET;
	int posy = WORKSPACETOP_OFFSET;
	CPoint ptButton(posx, 0);
	MapWindowPoints(HWND_DESKTOP, &ptButton, 1);
	int x = ptButton.x;
	int y = rParent.top - posy;

	m_bIgnoreSetCursor = true;
	Popup.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, x, y, m_hWnd);
    ::UpdateWindow(m_pCtp->GetBrowser());
	m_bIgnoreSetCursor = false;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnTBView(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CMenu Menu;
	bool bOK = !!Menu.LoadMenu(IDR_POPUPMENU_VIEW);
	if (!bOK)
		return E_FAIL;

	CMenu Popup(Menu.GetSubMenu(0));
	if (!Popup.IsMenu())
		return E_FAIL;
	
    Popup.EnableMenuItem(ID_VIEW_ZOOMFULL,        MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(ID_VIEW_PAGEWIDTH,       MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(ID_VIEW_SELECTION,       MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
    Popup.EnableMenuItem(3, MF_BYPOSITION | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));

	CRect rParent;
	::GetWindowRect(GetParent(), rParent);
	int posx = 113 - WORKSPACELEFT_OFFSET;
	int posy = WORKSPACETOP_OFFSET;
	CPoint ptButton(posx, 0);
	MapWindowPoints(HWND_DESKTOP, &ptButton, 1);
	int x = ptButton.x;
	int y = rParent.top - posy;

	m_bIgnoreSetCursor = true;
	Popup.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, x, y, m_hWnd);
    ::UpdateWindow(m_pCtp->GetBrowser());
	m_bIgnoreSetCursor = false;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnTBFormat(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CMenu Menu;
	bool bOK = !!Menu.LoadMenu(IDR_POPUPMENU_FORMAT);
	if (!bOK)
		return E_FAIL;

	CMenu Popup(Menu.GetSubMenu(0));
	if (!Popup.IsMenu())
		return E_FAIL;
	

    bool bSymSelected = m_Select.SymSelected();
    bool bSymIsText = m_Select.SymIsText();
    Popup.EnableMenuItem(0, MF_BYPOSITION | (m_pAGDoc && bSymSelected && bSymIsText ? MF_ENABLED : MF_GRAYED));
    Popup.EnableMenuItem(1, MF_BYPOSITION | (m_pAGDoc && bSymSelected && bSymIsText? MF_ENABLED : MF_GRAYED));
    Popup.EnableMenuItem(2, MF_BYPOSITION | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
    Popup.EnableMenuItem(3, MF_BYPOSITION | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
    Popup.EnableMenuItem(4, MF_BYPOSITION | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));

    Popup.EnableMenuItem(IDC_DOC_MAXIMIZE,      MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_MAXWIDTH,      MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_MAXHEIGHT,     MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
   
	CRect rParent;
	::GetWindowRect(GetParent(), rParent);
	int posx = 156 - WORKSPACELEFT_OFFSET;
	int posy = WORKSPACETOP_OFFSET;
	CPoint ptButton(posx, 0);
	MapWindowPoints(HWND_DESKTOP, &ptButton, 1);
	int x = ptButton.x;
	int y = rParent.top - posy;

	m_bIgnoreSetCursor = true;
	Popup.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, x, y, m_hWnd);
    ::UpdateWindow(m_pCtp->GetBrowser());
	m_bIgnoreSetCursor = false;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnTBTools(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CMenu Menu;
	bool bOK = !!Menu.LoadMenu(IDR_POPUPMENU_TOOLS);
	if (!bOK)
		return E_FAIL;

	CMenu Popup(Menu.GetSubMenu(0));
	if (!Popup.IsMenu())
		return E_FAIL;

	bool bClipArtSelected = m_Select.SymIsClipArt();
	Popup.EnableMenuItem(4/*Image Editor*/,     MF_BYPOSITION | (m_pAGDoc && m_Select.SymIsImage() ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(ID_IME_COLORART,		MF_BYCOMMAND | (m_pAGDoc && bClipArtSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(ID_IME_REDEYE,			MF_BYCOMMAND | (m_pAGDoc && !bClipArtSelected ? MF_ENABLED : MF_GRAYED));

    Popup.EnableMenuItem(0, MF_BYPOSITION | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
    Popup.EnableMenuItem(IDC_DOC_SPELLCHECK,  MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_GRID,        MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
    Popup.CheckMenuItem(IDC_DOC_GRID,         MF_BYCOMMAND | (m_Grid.IsOn() ? MF_CHECKED : MF_UNCHECKED));
	Popup.EnableMenuItem(IDC_DOC_ARTGALLERY,  MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));

    bool bAllowEMDlg = (m_pAGDoc && m_pAGDoc->AllowEMDlg());
	Popup.EnableMenuItem(IDC_DOC_ENVELOPE_WIZ, MF_BYCOMMAND | (bAllowEMDlg || m_pEMDlg ? MF_ENABLED : MF_GRAYED));

	bool bSpecialsEnabled = SpecialsEnabled();
	if (!m_pCtp->IsAmericanGreetings()) // temp - JHC
	{
		Popup.RemoveMenu(4, MF_BYPOSITION); // image editor
		Popup.RemoveMenu(4, MF_BYPOSITION); // art gallery

		CMenu AddSubMenu(Popup.GetSubMenu(0));
		if (AddSubMenu.IsMenu())
		{
			Popup.RemoveMenu(IDC_DOC_ARTGALLERY, MF_BYCOMMAND);
		}
	}
	else if (!bSpecialsEnabled)
	{
		CMenu AddSubMenu(Popup.GetSubMenu(0));
		if (AddSubMenu.IsMenu())
		{
			Popup.RemoveMenu(IDC_DOC_ADDSHAPE, MF_BYCOMMAND);
		}
	}

	CRect rParent;
	::GetWindowRect(GetParent(), rParent);
	int posx = 210 - WORKSPACELEFT_OFFSET;
	int posy = WORKSPACETOP_OFFSET;
	CPoint ptButton(posx, 0);
	MapWindowPoints(HWND_DESKTOP, &ptButton, 1);
	int x = ptButton.x;
	int y = rParent.top - posy;

	m_bIgnoreSetCursor = true;
	Popup.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, x, y, m_hWnd);
    ::UpdateWindow(m_pCtp->GetBrowser());
	m_bIgnoreSetCursor = false;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnTBHelp(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CMenu Menu;
	bool bOK = !!Menu.LoadMenu(IDR_POPUPMENU_HELP);
	if (!bOK)
		return E_FAIL;

	CMenu Popup(Menu.GetSubMenu(0));
	if (!Popup.IsMenu())
		return E_FAIL;

	Popup.EnableMenuItem(IDC_DOC_OPEN,          MF_BYCOMMAND | MF_ENABLED);
	Popup.EnableMenuItem(IDC_DOC_CLOSE,         MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_SAVE,          MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_SAVEAS,        MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_PRINT,         MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));

	CRect rParent;
	::GetWindowRect(GetParent(), rParent);
	int posx = 262 - WORKSPACELEFT_OFFSET;
	int posy = WORKSPACETOP_OFFSET;
	CPoint ptButton(posx, 0);
	MapWindowPoints(HWND_DESKTOP, &ptButton, 1);
	int x = ptButton.x;
	int y = rParent.top - posy;

	m_bIgnoreSetCursor = true;
	Popup.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, x, y, m_hWnd);
    ::UpdateWindow(m_pCtp->GetBrowser());
	m_bIgnoreSetCursor = false;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocExtras(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CMenu Menu;
	bool bOK = !!Menu.LoadMenu(IDR_POPUPMENU_EXTRAS);
	if (!bOK)
		return E_FAIL;

	CMenu Popup(Menu.GetSubMenu(0));
	if (!Popup.IsMenu())
		return E_FAIL;

	// Make sure the menu items are set correctly
	bool bAllowEMDlg = (m_pAGDoc && m_pAGDoc->AllowEMDlg());
	Popup.EnableMenuItem(IDC_DOC_DOWNLOADFONTS, MF_BYCOMMAND | MF_ENABLED);
	Popup.EnableMenuItem(IDC_DOC_DOWNLOADSPELLCHECK, MF_BYCOMMAND | MF_ENABLED);
	Popup.EnableMenuItem(IDC_DOC_ENVELOPE_WIZ, MF_BYCOMMAND | (bAllowEMDlg || m_pEMDlg ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_ABOUT, MF_BYCOMMAND | MF_ENABLED);

	CRect rParent;
	::GetWindowRect(GetParent(), rParent);
	int posx = (m_pCtp->IsHpUI() ? 406 : 572);
	int posy = (m_pCtp->IsHpUI() ? 2 : 12);
	CPoint ptExtrasButton(posx, 0);
	MapWindowPoints(HWND_DESKTOP, &ptExtrasButton, 1);
	int x = ptExtrasButton.x;
	int y = rParent.top - posy;
	m_bIgnoreSetCursor = true;
	Popup.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, x, y, m_hWnd);
	Menu.DestroyMenu();
	m_bIgnoreSetCursor = false;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::Layer(CAGSym* pSym, UINT nCommand, int iOffset)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (!pSym || pSym->IsHidden(GetHiddenFlag()))
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	CAGPage* pPage = NULL;
	CAGLayer* pLayer = NULL;
	int iPosition = m_pAGDoc->FindSymbolAnywhere(pSym, &pPage, &pLayer);
	if (iPosition < 0 || !pPage || !pLayer)
		return E_FAIL;

	int nSymbols = pLayer->GetNumSymbols();
	int iNewPosition = iPosition + iOffset;
	if (iNewPosition < 0)
		iNewPosition = 0;
	if (iNewPosition > nSymbols - 1)
		iNewPosition = nSymbols - 1;
	if (iNewPosition == iPosition)
		return E_FAIL;
	iOffset = iNewPosition - iPosition;

	// Invalidate the symbol's current location, since this symbol is changing size location, or layering...
	SymbolInvalidate(pSym);

	// Change the symbol
	if (nCommand != UNDO_COMMAND)
		m_Undo.CheckPoint(nCommand, pSym, iOffset);
	m_pAGDoc->SetModified(true);
	pLayer->OrderSymbol(pSym, iOffset);

	// Select the newly modified symbol
	SymbolSelect(pSym);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocLayerBackOne(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return Layer(m_Select.GetSym(), id, +1);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocLayerForwardOne(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return Layer(m_Select.GetSym(), id, -1);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocLayerToFront(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return Layer(m_Select.GetSym(), id, -MAX_AGSYMBOLS);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocLayerToBack(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return Layer(m_Select.GetSym(), id, MAX_AGSYMBOLS);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::Position(int xPosition, int yPosition, UINT nCommand)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	if (m_Select.SymIsLocked())
	{
		CMessageBox::ObjectIsLocked();
		return E_FAIL;
	}

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CAGLayer* pActiveLayer = pPage->GetActiveLayer();
	if (!pActiveLayer)
		return E_FAIL;

	CAGSym* pSym = m_Select.GetSym();
	if (!pSym)
		return E_FAIL;

	// Unselect the current symbol, since we are changing the dest rect or the transform matrix
	SymbolUnselect(true/*bClearPointer*/, false/*fSetPanel*/);

	CAGMatrix Matrix = pSym->GetMatrix();
	RECT DestRect = pSym->GetDestRect();
	Matrix.Transform(DestRect);

	// Get the page size minus the non-printable margins
	SIZE PageSize = {0,0};
	pPage->GetPageSize(PageSize);
	RECT PageRect = {0, 0, PageSize.cx, PageSize.cy};
	if (xPosition != POS_CENTER && yPosition != POS_MIDDLE)
	{
		RECT Margins = {0,0,0,0};
		m_Print.GetMargins(m_pAGDoc, Margins);
		PageRect.left   += Margins.left;
		PageRect.top    += Margins.top;
		PageRect.right  -= Margins.right;
		PageRect.bottom -= Margins.bottom;
	}

	int dx = 0;
	switch (xPosition)
	{
		case POS_LEFT:
			dx = PageRect.left - DestRect.left;
			break;
		case POS_CENTER:
			dx = ((PageRect.right + PageRect.left) - (DestRect.right + DestRect.left)) / 2;
			break;
		case POS_RIGHT:
			dx = PageRect.right - DestRect.right;
			break;
	}

	int dy = 0;
	switch (yPosition)
	{
		case POS_TOP:
			dy = PageRect.top - DestRect.top;
			break;
		case POS_MIDDLE:
			dy = ((PageRect.bottom + PageRect.top) - (DestRect.bottom + DestRect.top)) / 2;
			break;
		case POS_BOTTOM:
			dy = PageRect.bottom - DestRect.bottom;
			break;
	}

	Matrix.Translate(dx, dy);

	// Invalidate the symbol's current location, since this symbol is changing size location, or layering...
	SymbolInvalidate(pSym);

	// Change the symbol
	if (Matrix != pSym->GetMatrix())
		m_Undo.CheckPoint(nCommand, pSym, pSym->GetMatrix());
	m_pAGDoc->SetModified(true);

	pSym->SetMatrix(Matrix);

	// Select the newly modified symbol
	SymbolSelect(pSym);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocPosition(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	switch (id)
	{
		case IDC_DOC_POSCENTER:
			return Position(POS_CENTER, POS_MIDDLE, id);
		case IDC_DOC_POSCENTERH:
			return Position(POS_CENTER, 0, id);
		case IDC_DOC_POSCENTERV:
			return Position(0, POS_MIDDLE, id);
		case IDC_DOC_POSLEFT:
			return Position(POS_LEFT, 0, id);
		case IDC_DOC_POSRIGHT:
			return Position(POS_RIGHT, 0, id);
		case IDC_DOC_POSTOP:
			return Position(0, POS_TOP, id);
		case IDC_DOC_POSTOPLEFT:
			return Position(POS_LEFT, POS_TOP, id);
		case IDC_DOC_POSTOPRIGHT:
			return Position(POS_RIGHT, POS_TOP, id);
		case IDC_DOC_POSBOTTOM:
			return Position(0, POS_BOTTOM, id);
		case IDC_DOC_POSBOTTOMLEFT:
			return Position(POS_LEFT, POS_BOTTOM, id);
		case IDC_DOC_POSBOTTOMRIGHT:
			return Position(POS_RIGHT, POS_BOTTOM, id);
	}
	
	return E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocMaxWidth(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return MaximizeSymbolSize(true/*bWidth*/, false/*bHeight*/);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocMaxHeight(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return MaximizeSymbolSize(false/*bWidth*/, true/*bHeight*/);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocMaximize(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return MaximizeSymbolSize(true/*bWidth*/, true/*bHeight*/);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::MaximizeSymbolSize(bool bWidth, bool bHeight, bool bRaw)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	if (m_Select.SymIsLocked())
	{
		CMessageBox::ObjectIsLocked();
		return E_FAIL;
	}

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CAGSym* pSym = m_Select.GetSym();
	if (!pSym)
		return E_FAIL;

	// Unselect the current symbol, since we are changing the dest rect or the transform matrix
	SymbolUnselect(true/*bClearPointer*/, false/*fSetPanel*/);

	// Change the symbol
	// Get the page size minus the non-printable margins
	SIZE PageSize = {0,0};
	pPage->GetPageSize(PageSize);
	RECT PageRect = {0, 0, PageSize.cx, PageSize.cy};
	RECT Margins = {0,0,0,0};
	m_Print.GetMargins(m_pAGDoc, Margins);
	PageRect.left   += Margins.left;
	PageRect.top    += Margins.top;
	PageRect.right  -= Margins.right;
	PageRect.bottom -= Margins.bottom;

	RECT DestRect = pSym->GetDestRect();
	CAGMatrix Matrix = pSym->GetMatrix();
	Matrix.Transform(DestRect);

	double fScaleX = (double)WIDTH(PageRect) / (double)max(WIDTH(DestRect),1);
	double fScaleY = (double)HEIGHT(PageRect) / (double)max(HEIGHT(DestRect),1);
	if (pSym->IsImage() && bWidth && bHeight && !bRaw)
	{
		if (fScaleX > fScaleY)
			fScaleX = fScaleY;
		else
			fScaleY = fScaleX;
	}
	if (!bWidth)
		fScaleX = 1.0;
	if (!bHeight)
		fScaleY = 1.0;
	Matrix.Scale(fScaleX, fScaleY,
		(DestRect.left + DestRect.right) / 2,
		(DestRect.top + DestRect.bottom) / 2);

	int dx = ((PageRect.right + PageRect.left) - (DestRect.right + DestRect.left)) / 2;
	int dy = ((PageRect.bottom + PageRect.top) - (DestRect.bottom + DestRect.top)) / 2;
	if (!bWidth)
		dx = 0;
	if (!bHeight)
		dy = 0;
	Matrix.Translate(dx, dy);

	// Change the symbol, either by applying the matrix, or by changing the rectangle
	bool bSetUndoCheckPoint = !bRaw;
	bool bNonRotatedText = ((pSym->IsText() || pSym->IsAddAPhoto()) && !Matrix.IsRotated());
	bool bDoSetRect = (pSym->IsLine() || bNonRotatedText);
	if (bDoSetRect)
	{
		RECT DestRect = pSym->GetDestRect();
		Matrix.Transform(DestRect);
		// Don't let the rect be empty in either dimension
		::InflateRect(&DestRect, !WIDTH(DestRect), !HEIGHT(DestRect));

		if (!::EqualRect(&DestRect, &pSym->GetDestRect()))
		{
			if (bSetUndoCheckPoint)
				m_Undo.CheckPoint(IDC_DOC_DESTRECTCHANGE, pSym, pSym->GetDestRect());
			m_pAGDoc->SetModified(true);
		}

		pSym->SetDestRect(DestRect);
		Matrix.Unity();
		pSym->SetMatrix(Matrix);
	}
	else
	{
		if (Matrix != pSym->GetMatrix())
		{
			if (bSetUndoCheckPoint)
				m_Undo.CheckPoint(IDC_DOC_TRANSFORM, pSym, pSym->GetMatrix());
			m_pAGDoc->SetModified(true);
		}

		pSym->SetMatrix(Matrix);
	}

	// Select the newly modified symbol
	SymbolSelect(pSym);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::OnDocChangeFillColor(COLORREF Color)
{
	if (!m_pAGDoc)
		return;

	if (m_Select.NoSymSelected())
		return;

	if (m_Select.SymIsGraphic())
	{
		// Change the symbol
		CAGSymGraphic* pGraphicSym = m_Select.GetGraphicSym();

		if (Color != pGraphicSym->GetFillColor())
		{
			m_Undo.CheckPoint(IDC_DOC_FILLCOLORCHANGE, m_Select.GetSym(), pGraphicSym->GetFillColor());
			m_pAGDoc->SetModified(true);
		}
		
		pGraphicSym->SetFillColor(Color);

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}
	else
	if (m_Select.SymIsText() || (m_Select.SymIsAddAPhoto() && SpecialsEnabled()))
	{
		CAGText* pText = (CAGText*)m_Select.GetTextSym();
		pText->SetFillColor(Color);
	}
	else
	if (m_Select.SymIsCalendar())
	{
		// Change the symbol
		CAGSymCalendar* pCalendarSym = m_Select.GetCalendarSym();
		if (pCalendarSym->GetActiveSpec())
			pCalendarSym->SetSpecFillColor(Color);
		else
		{
			if (Color != pCalendarSym->GetFillColor())
			{
				m_Undo.CheckPoint(IDC_DOC_FILLCOLORCHANGE, m_Select.GetSym(), pCalendarSym->GetFillColor());
				m_pAGDoc->SetModified(true);
			}
			
			pCalendarSym->SetFillColor(Color);
		}

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::OnDocChangeFillColor2(COLORREF Color)
{
	if (!m_pAGDoc)
		return;

	if (m_Select.NoSymSelected())
		return;

    if (m_Select.SymIsGraphic())
	{
		// Change the symbol
		CAGSymGraphic* pGraphicSym = m_Select.GetGraphicSym();
		if (Color != pGraphicSym->GetFillColor2())
		{
			m_Undo.CheckPoint(IDC_DOC_FILLCOLORCHANGE2, m_Select.GetSym(), pGraphicSym->GetFillColor2());
			m_pAGDoc->SetModified(true);
		}
		pGraphicSym->SetFillColor2(Color);

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}
	else
	if (m_Select.SymIsText() || (m_Select.SymIsAddAPhoto() && SpecialsEnabled()))
	{
		CAGText* pText = (CAGText*)m_Select.GetTextSym();
		pText->SetFillColor2(Color);
	}
	else
	if (m_Select.SymIsCalendar())
	{
		// Change the symbol
		CAGSymCalendar* pCalendarSym = m_Select.GetCalendarSym();
		if (pCalendarSym->GetActiveSpec())
			pCalendarSym->SetSpecFillColor2(Color);
		else
		{
			if (Color != pCalendarSym->GetFillColor2())
			{
				m_Undo.CheckPoint(IDC_DOC_FILLCOLORCHANGE2, m_Select.GetSym(), pCalendarSym->GetFillColor2());
				m_pAGDoc->SetModified(true);
			}
			
			pCalendarSym->SetFillColor2(Color);
		}

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::OnDocChangeFillColor3(COLORREF Color)
{
	if (!m_pAGDoc)
		return;

	if (m_Select.NoSymSelected())
		return;

	if (m_Select.SymIsGraphic())
	{
	}
	else
	if (m_Select.SymIsText() || (m_Select.SymIsAddAPhoto() && SpecialsEnabled()))
	{
	}
	else
	if (m_Select.SymIsCalendar())
	{
		// Change the symbol
		CAGSymCalendar* pCalendarSym = m_Select.GetCalendarSym();
		if (pCalendarSym->GetActiveSpec())
			int i = 0;//j pCalendarSym->SetSpecFillColor3(Color);
		else
		{
			if (Color != pCalendarSym->GetFillColor3())
			{
				m_Undo.CheckPoint(IDC_DOC_FILLCOLORCHANGE3, m_Select.GetSym(), pCalendarSym->GetFillColor3());
				m_pAGDoc->SetModified(true);
			}
			pCalendarSym->SetFillColor3(Color);
		}

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::OnDocChangeLineColor(COLORREF Color)
{
	if (!m_pAGDoc)
		return;

	if (m_Select.NoSymSelected())
		return;

	if (m_Select.SymIsGraphic())
	{
		// Change the symbol
		CAGSymGraphic* pGraphicSym = m_Select.GetGraphicSym();
		if (Color != pGraphicSym->GetLineColor())
		{
			m_Undo.CheckPoint(IDC_DOC_LINECOLORCHANGE, m_Select.GetSym(), pGraphicSym->GetLineColor());
			m_pAGDoc->SetModified(true);
		}
		pGraphicSym->SetLineColor(Color);

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}
	else
	if (m_Select.SymIsText() || (m_Select.SymIsAddAPhoto() && SpecialsEnabled()))
	{
		CAGText* pText = (CAGText*)m_Select.GetTextSym();
		pText->SetLineColor(Color);
	}
	else
	if (m_Select.SymIsCalendar())
	{
		// Change the symbol
		CAGSymCalendar* pCalendarSym = m_Select.GetCalendarSym();
		if (pCalendarSym->GetActiveSpec())
			pCalendarSym->SetSpecLineColor(Color);
		else
		{
			if (Color != pCalendarSym->GetLineColor())
			{
				m_Undo.CheckPoint(IDC_DOC_LINECOLORCHANGE, m_Select.GetSym(), pCalendarSym->GetLineColor());
				m_pAGDoc->SetModified(true);
			}
			pCalendarSym->SetLineColor(Color);
		}

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocChangeFillType(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(SELECTED_PANEL);
	if (!pCtlPanel)
		return E_FAIL;

	//Get the fill type
	FillType Fill = ((CToolsCtlPanelCommon*)pCtlPanel->GetBotToolPanel())->GetCurrentFillType();

	if (m_Select.SymIsGraphic())
	{
		// Change the symbol
		CAGSymGraphic* pGraphicSym = m_Select.GetGraphicSym();
		if (Fill != pGraphicSym->GetFillType())
		{
			m_Undo.CheckPoint(IDC_DOC_FILLTYPECHANGE, m_Select.GetSym(), static_cast<int>(pGraphicSym->GetFillType()));
			m_pAGDoc->SetModified(true);
		}
		pGraphicSym->SetFillType(Fill);

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}
	else
	if (m_Select.SymIsText() || (m_Select.SymIsAddAPhoto() && SpecialsEnabled()))
	{
		CAGText* pText = (CAGText*)m_Select.GetTextSym();
		pText->SetFillType(Fill);
	}
	else
	if (m_Select.SymIsCalendar())
	{
		// Change the symbol
		CAGSymCalendar* pCalendarSym = m_Select.GetCalendarSym();
		if (pCalendarSym->GetActiveSpec())
		{
			pCalendarSym->SetSpecFillType(Fill);

			// Invalidate the newly modified symbol
			SymbolInvalidate(m_Select.GetSym());
		}
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocChangeCalendarMonth(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(SELECTED_PANEL);
	if (!pCtlPanel)
		return E_FAIL;

	if (m_Select.SymIsCalendar())
	{
		CToolsPanelCal1* pTopCalToolsPanel = (CToolsPanelCal1*)pCtlPanel->GetTopToolPanel();
		if (!pTopCalToolsPanel)
			return E_FAIL;

		// Change the symbol
		CAGSymCalendar* pCalendarSym = m_Select.GetCalendarSym();
		int iCalendarMonth = pTopCalToolsPanel->GetCurrentCalendarMonth();
		if (iCalendarMonth != pCalendarSym->GetMonth())
		{
			m_Undo.CheckPoint(IDC_DOC_CALENDARMONTHCHANGE, m_Select.GetSym(), pCalendarSym->GetMonth());
			m_pAGDoc->SetModified(true);
		}
		pCalendarSym->SetMonth(iCalendarMonth);

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocChangeCalendarStyle(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(SELECTED_PANEL);
	if (!pCtlPanel)
		return E_FAIL;

	if (m_Select.SymIsCalendar())
	{
		// Change the symbol
		CAGSymCalendar* pCalSym = m_Select.GetCalendarSym();
		CToolsPanelCal1* pTopCalToolsPanel = (CToolsPanelCal1*)pCtlPanel->GetTopToolPanel();
		if (!pTopCalToolsPanel)
			return E_FAIL;

		if (pTopCalToolsPanel)
		{
			UINT iYrStyle = pTopCalToolsPanel->GetYearStyle();
			UINT iWkStyle = pTopCalToolsPanel->GetWeekStyle();
			UINT iTtlSize = pTopCalToolsPanel->GetTitleSize();
			UINT iWkSize = pTopCalToolsPanel->GetDayOfWkSize();
			bool iHideBoxes = pTopCalToolsPanel->GetHideBoxes();
			bool iHideAllBoxes = pTopCalToolsPanel->GetHideAllBoxes();
			bool iWkStart = pTopCalToolsPanel->GetWeekStart();

			if (iYrStyle != pCalSym->GetYearStyle())
			{
				m_Undo.CheckPoint(IDC_DOC_CALENDARSTYLECHANGE, m_Select.GetSym(), pCalSym->GetYearStyle());
				m_pAGDoc->SetModified(true);
			}
			if (iWkStyle != pCalSym->GetWeekStyle())
			{
				m_Undo.CheckPoint(IDC_DOC_CALENDARSTYLECHANGE, m_Select.GetSym(), pCalSym->GetWeekStyle());
				m_pAGDoc->SetModified(true);
			}
			if (iHideBoxes != pCalSym->GetHideBoxes())
			{
				m_Undo.CheckPoint(IDC_DOC_CALENDARSTYLECHANGE, m_Select.GetSym(), pCalSym->GetHideBoxes());
				m_pAGDoc->SetModified(true);
			}
			if (iHideAllBoxes != pCalSym->GetHideAllBoxes())
			{
				m_Undo.CheckPoint(IDC_DOC_CALENDARSTYLECHANGE, m_Select.GetSym(), pCalSym->GetHideAllBoxes());
				m_pAGDoc->SetModified(true);
			}
			if (iTtlSize != pCalSym->GetTitleSize())
			{
				m_Undo.CheckPoint(IDC_DOC_CALENDARSTYLECHANGE, m_Select.GetSym(), pCalSym->GetTitleSize());
				m_pAGDoc->SetModified(true);
			}
			if (iWkSize != pCalSym->GetDayOfWkSize())
			{
				m_Undo.CheckPoint(IDC_DOC_CALENDARSTYLECHANGE, m_Select.GetSym(), pCalSym->GetDayOfWkSize());
				m_pAGDoc->SetModified(true);
			}
			if (iWkStart != pCalSym->GetWeekStart())
			{
				m_Undo.CheckPoint(IDC_DOC_CALENDARSTYLECHANGE, m_Select.GetSym(), pCalSym->GetWeekStart());
				m_pAGDoc->SetModified(true);
			}

			pCalSym->SetYearStyle(iYrStyle);
			pCalSym->SetWeekStyle(iWkStyle);

			pCalSym->SetTitleSize(iTtlSize);
			pCalSym->SetDayOfWkSize(iWkSize);

			pCalSym->SetHideBoxes(iHideBoxes);
			pCalSym->SetHideAllBoxes(iHideAllBoxes);

			pCalSym->SetWeekStart(iWkStart);
		}

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocChangeCalendarYear(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(SELECTED_PANEL);
	if (!pCtlPanel)
		return E_FAIL;

	if (m_Select.SymIsCalendar())
	{
		CToolsPanelCal1* pTopCalToolsPanel = (CToolsPanelCal1*)pCtlPanel->GetTopToolPanel();
		if (!pTopCalToolsPanel)
			return E_FAIL;

		// Change the symbol
		CAGSymCalendar* pCalendarSym = m_Select.GetCalendarSym();
		int iCalendarYear = pTopCalToolsPanel->GetCurrentCalendarYear();
		if (iCalendarYear != pCalendarSym->GetYear())
		{
			m_Undo.CheckPoint(IDC_DOC_CALENDARYEARCHANGE, m_Select.GetSym(), pCalendarSym->GetYear());
			m_pAGDoc->SetModified(true);
		}
		pCalendarSym->SetYear(iCalendarYear);

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocChangeTextSize(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(SELECTED_PANEL);
	if (!pCtlPanel)
		return E_FAIL;

	if (m_Select.SymIsText() || (m_Select.SymIsAddAPhoto() && SpecialsEnabled()))
	{
		int TextSize = ((CToolsPanelText1*)pCtlPanel->GetTopToolPanel())->GetCurrentTextSize();
		if (TextSize < POINTUNITS(1))
			TextSize = POINTUNITS(1);

		CAGText* pText = (CAGText*)m_Select.GetTextSym();
		pText->SetTextSize(TextSize);
		m_pAGDoc->SetModified(true);
	}
	else
	if (m_Select.SymIsCalendar())
	{
		CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(TEXT_PANEL);
		if (!pCtlPanel)
			return E_FAIL;

		CToolsPanelText1* pTopTxtToolsPanel = (CToolsPanelText1*)pCtlPanel->GetTopToolPanel();

		int TextSize = pTopTxtToolsPanel->GetCurrentTextSize();
		if (TextSize < POINTUNITS(1))
			TextSize = POINTUNITS(1);

		CAGSymCalendar* pCalendarSym = m_Select.GetCalendarSym();
		pCalendarSym->SetSpecTextSize(TextSize);
		m_pAGDoc->SetModified(true);

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocChangeLineWidth(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(SELECTED_PANEL);
	if (!pCtlPanel)
		return E_FAIL;

	CToolsCtlPanelCommon* pBotCtlPanel = (CToolsCtlPanelCommon*)pCtlPanel->GetBotToolPanel();
	// Get the line width
	int LineWidth = pBotCtlPanel->GetCurrentLineWidth();
	
	if (m_Select.SymIsGraphic())
	{
		// Change the symbol
		CAGSymGraphic* pGraphicSym = m_Select.GetGraphicSym();
		if (LineWidth != pGraphicSym->GetLineWidth())
		{
			m_Undo.CheckPoint(IDC_DOC_LINEWIDTHCHANGE, m_Select.GetSym(), pGraphicSym->GetLineWidth());
			m_pAGDoc->SetModified(true);
		}
		pGraphicSym->SetLineWidth(LineWidth);

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}
	else
	if (m_Select.SymIsText() || (m_Select.SymIsAddAPhoto() && SpecialsEnabled()))
	{
		CAGText* pText = (CAGText*)m_Select.GetTextSym();
		pText->SetLineWidth(LineWidth);
	}
	else
	if (m_Select.SymIsCalendar())
	{
		// Change the symbol
		CAGSymCalendar* pCalendarSym = m_Select.GetCalendarSym();
		if (pCalendarSym->GetActiveSpec())
			pCalendarSym->SetSpecLineWidth(LineWidth);
		else
		{
			if (LineWidth != pCalendarSym->GetLineWidth())
			{
				m_Undo.CheckPoint(IDC_DOC_LINEWIDTHCHANGE, m_Select.GetSym(), pCalendarSym->GetLineWidth());
				m_pAGDoc->SetModified(true);
			}
			pCalendarSym->SetLineWidth(LineWidth);
			COLORREF LineColor = ((LineWidth == LT_None)?CLR_NONE:pBotCtlPanel->GetCurrentLineColor());
			pCalendarSym->SetLineColor(LineColor);
		}

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocChangeShapeName(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(SELECTED_PANEL);
	if (!pCtlPanel)
		return E_FAIL;

	if (m_Select.SymIsShape())
	{
		CAGSymDrawing* pGraphicSym = (CAGSymDrawing*)m_Select.GetSym();
		int iShapeIndex = CShape::GetShapeIndex(pGraphicSym->GetShapeName());
		m_Undo.CheckPoint(IDC_DOC_SHAPENAMECHANGE, m_Select.GetSym(), iShapeIndex);
		m_pAGDoc->SetModified(true);

		iShapeIndex = ((CToolsPanelGraphics1*)pCtlPanel->GetTopToolPanel())->GetCurrentShape();
		pGraphicSym->SetShapeName(CShape::GetShapeName(iShapeIndex));

		// Invalidate the newly modified symbol
		SymbolInvalidate(m_Select.GetSym());
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocMasterLayer(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	m_bEditMasterLayer = !m_bEditMasterLayer;
	m_pAGDoc->SetMasterLayer(m_bEditMasterLayer);

	// Unselect the current symbol, since we are changing the dest rect or the transform matrix
	SymbolUnselect(true/*bClearPointer*/);

	PageInvalidate();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocMetric(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	m_bMetricMode = !m_bMetricMode;
	CAGDocSheetSize::SetMetric(m_bMetricMode);

	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		bool bOK = (regkey.SetDWORDValue(REGVAL_METRIC, (DWORD)m_bMetricMode) == ERROR_SUCCESS);
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocGrid(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	m_Grid.On(!m_Grid.IsOn());

	PageInvalidate();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocDownloadSpellCheck(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	LPCSTR strMsg;
	CSpell Spell(CAGDocSheetSize::GetMetric());
	if (!Spell.Init(true/*bCheckFiles*/))
		strMsg = "The spell check library was not found on your system.\n\nWould you like to download it now?";
	else
		strMsg = "Would you like to download and install the spell check library?";

	int iResponse = CMessageBox::Message(strMsg, MB_YESNO);
	if (iResponse != IDYES)
		return E_FAIL;

	CWaitCursor Wait;
	CString strDstPath;
	Spell.GetSpellCheckerPath(strDstPath);
	m_pCtp->BeginSpellingDownload(strDstPath);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocDownloadFonts(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	int iResponse = CMessageBox::Message("Would you like to download and install the full set of fonts?", MB_YESNO);
	if (iResponse != IDYES)
		return E_FAIL;

	FONTDOWNLOADLIST List;
	m_pCtp->m_FontList.GetFontList(List, true/*bMissingOnly*/);
	if (List.empty())
	{
		iResponse = CMessageBox::Message("You already have the full set of fonts.\n\nWould you like to download them anyway?", MB_YESNO);
		if (iResponse != IDYES)
			return E_FAIL;

		m_pCtp->m_FontList.GetFontList(List, false/*bMissingOnly*/);
	}

	CWaitCursor Wait;

	m_pCtp->FontListDownload(List);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnPageAdd(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	bool bAllowNewPages = m_pAGDoc->AllowNewPages() | (SpecialsEnabled() ? m_pAGDoc->NeedsCardBack() : 0);
	if (!bAllowNewPages)
		return E_FAIL;

	CString strDocType = CAGDocTypes::Name(m_pAGDoc->GetDocType());
	strDocType.MakeLower();

	int iResponse = CMessageBox::Message(
		String("Are you about to add a new page to the end of this %s project?\n\nClick 'Yes' if you want to make a copy of the current page.\n\nClick 'No' if you want a blank page to be added.\n\nClick 'Cancel' if you do not want to add a page.", strDocType),
		MB_YESNOCANCEL);
	if (iResponse == IDCANCEL)
		return E_FAIL;

	CWaitCursor Wait;

	// Get the current page
	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	// Create the new page
	bool bCopySymbols = (iResponse == IDYES);
	bool bFullPage = m_pAGDoc->NeedsCardBack();
	int iNewPageNumber = m_pAGDoc->GetNumPages() + 1;
	CString strNewPageName;
	if (bFullPage)
		strNewPageName.Format("Extra %d", iNewPageNumber - 4);
	CAGPage* pNewPage = pPage->Duplicate(bCopySymbols, bFullPage, iNewPageNumber, strNewPageName);
	if (!pNewPage)
		return E_FAIL;

	m_pAGDoc->SetModified(true);
	m_pAGDoc->AddPage(pNewPage);

	CPagePanel* pPagePanel = m_pCtp->GetPagePanel();
	if (!pPagePanel)
		return E_FAIL;

	// Update the list of pages, and switch to the new page
	CString	strPage;
	pNewPage->GetPageName(strPage); 
	char strPageName[MAX_PATH];
	lstrcpy(strPageName, strPage);
	int nPage = pPagePanel->m_PageNavigator.AddPage(strPageName);
	if (nPage < 0)
		return E_FAIL;

	pPagePanel->PageChange();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnPageDelete(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	bool bAllowNewPages = m_pAGDoc->AllowNewPages() | (SpecialsEnabled() ? m_pAGDoc->NeedsCardBack() : 0);
	if (!bAllowNewPages)
		return E_FAIL;

	CString strDocType = CAGDocTypes::Name(m_pAGDoc->GetDocType());
	strDocType.MakeLower();

	if (m_pAGDoc->GetNumPages() <= 1)
	{
		CMessageBox::Message(String("You cannot delete the only page in this %s project.", strDocType));
		return E_FAIL;
	}

	int iResponse = CMessageBox::Message(
		String("Are you sure you want to delete the current page from this %s project?", strDocType),
		MB_YESNO);
	if (iResponse != IDYES)
		return E_FAIL;

	CWaitCursor Wait;

	// Unselect the current symbol, since the page is going away
	SymbolUnselect(true/*bClearPointer*/);

	// Delete the current page
	m_pAGDoc->SetModified(true);
	if (!m_pAGDoc->DeleteCurrentPage())
		return E_FAIL;

	CPagePanel* pPagePanel = m_pCtp->GetPagePanel();
	if (!pPagePanel)
		return E_FAIL;

	// Update the list of pages, and switch to the new page
	int nPage = pPagePanel->m_PageNavigator.DeleteCurrentPage();
	if (nPage < 0)
		return E_FAIL;

	pPagePanel->PageChange();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnPagePrev(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CPagePanel* pPagePanel = m_pCtp->GetPagePanel();
	if (!pPagePanel)
		return E_FAIL;

	return pPagePanel->OnPagePrev(code, id, hWnd, bHandled);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnPageNext(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CPagePanel* pPagePanel = m_pCtp->GetPagePanel();
	if (!pPagePanel)
		return E_FAIL;

	return pPagePanel->OnPageNext(code, id, hWnd, bHandled);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnSelectAll(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (m_Select.NoSymSelected() || !(m_Select.SymIsText() || (m_Select.SymIsAddAPhoto() && SpecialsEnabled())))
	{
		CMessageBox::Message("You must activate a text object before using this function to select all of its inside text.");
		return E_FAIL;
	}

	CAGSymText* pText = m_Select.GetTextSym();
	pText->SelectAll();

	CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(TEXT_PANEL);
	if (pCtlPanel)
		pCtlPanel->UpdateControls();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnSelectPrev(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return SelectNextPrev(false/*bNext*/);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnSelectNext(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return SelectNextPrev(true/*bNext*/);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::SelectNextPrev(bool bNext)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CAGLayer* pActiveLayer = pPage->GetActiveLayer();
	if (!pActiveLayer)
		return E_FAIL;

	int iDirection = (bNext ? +1 : -1);
	CAGSym* pSymNext = pActiveLayer->FindNextSymbol(m_Select.GetSym(), iDirection, GetHiddenFlag(), !SpecialsEnabled()/*bIgnoreLocked*/);
	if (!pSymNext)
		return E_FAIL;

	// Select the next symbol
	SymbolSelect(pSymNext);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnGiveGift(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
#ifdef NOTUSED //j
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (!m_pAGDoc->NeedsCardBack())
	{
		CMessageBox::Message("Sorry, you can only add a gift certificate to a card.");
		return E_FAIL;
	}

	CString strItemInfo = m_pCtp->GetContextItemInfo();
	if (strItemInfo.IsEmpty() || (strItemInfo.Find("|||") >= 0))
	{
		CMessageBox::Message("Sorry, you cannot add a gift certificate to a saved card.");
		return E_FAIL;
	}

	int nPages = m_pAGDoc->GetNumPages();
	if (nPages < 4)
		return E_FAIL;

	int nLastPage = nPages - 1;

	CAGPage* pPage = m_pAGDoc->GetPage(nLastPage);
	if (!pPage)
		return E_FAIL;

	// If we have added a gift certificate already, get out
	if (pPage->GetMasterLayer()->FindFirstSymbolByID(IDR_GIVEGIFT, PROCESS_HIDDEN/*dwFlags*/))
	{
		CMessageBox::Message(String("You have already added a gift certificate to the back of this card."));
		return E_FAIL;
	}

#ifdef TEST //j
	CString strGiftID = "0000000000000000";
	CString strGiftAmount = "00";
	CString strGiftExpires = "XX-XX-XX";
	CString strGiftPickupUrl = "http://www.giveanything.com/custcar.htm";
#else
	CString strMemberNumber = m_pCtp->GetContextMemberNumber();
	if (strMemberNumber.IsEmpty())
		return E_FAIL;

	CString strCpHost = m_pCtp->GetContextCpHost();
	CString strSrcURL = strCpHost + "cpgift.pd?";
	strSrcURL += "memnum=" + strMemberNumber;
	strSrcURL += "&";
	strSrcURL += "iteminfo=" + strItemInfo;

	CHtmlDialog dlg(strSrcURL, false/*bRes*/, m_hWnd);
	dlg.SetWindowSize(565, 535);
	dlg.SetResizable(true);
	if (!dlg.DoModal())
		return E_FAIL;

	CString strReturnValue = dlg.GetReturnString();
	if (strReturnValue.IsEmpty())
		return E_FAIL;

	CString strDBItem = m_pCtp->GetOptionStringValue(strReturnValue, CString("item"));
	m_pCtp->SetDBItem(strDBItem);
	CString strGiftID = m_pCtp->GetOptionStringValue(strReturnValue, CString("pon"));
	if (strGiftID.IsEmpty())
		strGiftID = "0000000000000000";
	CString strGiftAmount = m_pCtp->GetOptionStringValue(strReturnValue, CString("tp"));
	if (strGiftAmount.IsEmpty())
		strGiftAmount = "00";
	CString strGiftExpires = m_pCtp->GetOptionStringValue(strReturnValue, CString("expdate"));
	if (strGiftExpires.IsEmpty())
		strGiftExpires = "XX-XX-XX";
	CString strGiftPickupUrl = m_pCtp->GetOptionStringValue(strReturnValue, CString("pickupurl"));
	if (strGiftPickupUrl.IsEmpty())
		strGiftPickupUrl = "http://www.giveanything.com/custcar.htm";
#endif TEST //j

	// Update the list of pages, and switch to the last page
	CPagePanel* pPagePanel = m_pCtp->GetPagePanel();
	if (pPagePanel)
	{
		pPagePanel->m_PageNavigator.SetPage(nLastPage);
		pPagePanel->PageChange();
	}

//j	// Be sure the last page became active
//j	if (m_pAGDoc->GetCurrentPageNum() != nLastPage)
//j		return E_FAIL;

	// Remember which layer is active
	int iLayer = (pPage->GetActiveLayer() == pPage->GetLayer(1));

	// Activate the layer where we will place the certificate (the master layer)
	CAGLayer* pMasterLayer = pPage->SetActiveLayer(0);
	if (!pMasterLayer)
		return E_FAIL;

	SIZE PageSize = {0,0};
	pPage->GetPageSize(PageSize);

	SIZE CertSize = {INCHES(3.333), INCHES(2.667)};
	RECT CertRect;
	CertRect.left = (PageSize.cx - CertSize.cx) / 2;
	CertRect.top = INCHES(0.375);
	CertRect.right = CertRect.left + CertSize.cx;
	CertRect.bottom = CertRect.top + CertSize.cy;

	// Get the image from the resource
	CAGSymImage* pCertImage = m_pCtp->GetImageFromResource(IDR_GIVEGIFT, &CertRect);
	if (pCertImage)
	{
		CertRect = pCertImage->GetDestRect();
		pMasterLayer->AddSymbol(pCertImage);
	}

	SIZE TextSize = {INCHES(1.0), INCHES(1.0)};
	RECT TextRect;
	TextRect.left = CertRect.right - TextSize.cx;
	TextRect.top = CertRect.top;
	TextRect.right = TextRect.left + TextSize.cx;
	TextRect.bottom = TextRect.top + TextSize.cy;

	LOGFONT Font;
	::ZeroMemory(&Font, sizeof(Font));
	Font.lfWeight = FW_NORMAL;
	Font.lfCharSet = ANSI_CHARSET; //j DEFAULT_CHARSET
	lstrcpy(Font.lfFaceName, "CAC Futura Casual");

	CAGSymText* pText = new CAGSymText(ST_TEXT);
	if (pText)
	{
		pText->Create(CertRect);
		pText->SetVertJust(eVertBottom);

		char pFormat[] = "You've been given a gift!\n\nGift Certificate #:\n%s\nExpires: %s\n\nTo spend your gift certificate, go to\n%s";
		CString strRawText;
		strRawText.Format(pFormat, strGiftID, strGiftExpires, strGiftPickupUrl);
		
		Font.lfHeight = -POINTUNITS(12);

		// Initialize the type specs
		CAGSpec* pSpecs[] = { new CAGSpec(
			Font,			// LOGFONT& Font
			eRagCentered,	// eTSJust HorzJust
			0,				// short sLineLeadPct
			LT_None,		// int LineWidth
			RGB(0,0,0),		// COLORREF LineColor
			FT_Solid,		// FillType Fill
			RGB(0,0,0),		// COLORREF FillColor
			RGB(0,0,0))		// COLORREF FillColor2
		};

		const int pOffsets[] = {0};
		pText->SetText(strRawText, 1, pSpecs, pOffsets);
		pMasterLayer->AddSymbol(pText);
	}

	pText = new CAGSymText(ST_TEXT);
	if (pText)
	{
		pText->SetID(IDR_GIVEGIFT);
		pText->Create(TextRect);
		pText->SetVertJust(eVertCentered);

		char pFormat[] = "$%s";
		CString strRawText;
		strRawText.Format(pFormat, strGiftAmount);
		
		Font.lfHeight = -POINTUNITS(26);

		// Initialize the type specs
		CAGSpec* pSpecs[] = { new CAGSpec(
			Font,			// LOGFONT& Font
			eRagCentered,	// eTSJust HorzJust
			0,				// short sLineLeadPct
			LT_None,		// int LineWidth
			RGB(0,0,0),		// COLORREF LineColor
			FT_Solid,		// FillType Fill
			RGB(0,0,0),		// COLORREF FillColor
			RGB(0,0,0))		// COLORREF FillColor2
		};

		const int pOffsets[] = {0};
		pText->SetText(strRawText, 1, pSpecs, pOffsets);
		pMasterLayer->AddSymbol(pText);
	}

	// Restore the active layer
	pPage->SetActiveLayer(iLayer);

	// Get the layer where we will find the card-back graphics (the user layer)
	CAGLayer* pUserLayer = pPage->GetUserLayer();
	if (pUserLayer)
	{
		CAGSym* pSym = NULL;

		// Delete the "Created just for you..." message to make room
		if (pSym = pUserLayer->FindFirstSymbolByID(IDR_CARDBACK_MESSAGE, 0/*dwFlags*/))
		{
			// Unselect the current symbol, if need be
			if (pSym == m_Select.GetSym())
				SymbolUnselect(true/*bClearPointer*/);

			// Invalidate the symbol's current location, since this symbol is going away...
			SymbolInvalidate(pSym);

			m_Undo.DereferenceItem(pSym);
			pUserLayer->DeleteSymbol(pSym);
		}

		bool bNotEnoughRoom = (m_pAGDoc->GetDocType() == DOC_QUARTERCARD && !pPage->IsPortrait());
		if (bNotEnoughRoom)
		{
			// Delete the CP logo down to make room
			if (pSym = pUserLayer->FindFirstSymbolByID(IDR_AGLOGO, 0/*dwFlags*/))
			{
				// Unselect the current symbol, if need be
				if (pSym == m_Select.GetSym())
					SymbolUnselect(true/*bClearPointer*/);

				// Invalidate the symbol's current location, since this symbol is going away...
				SymbolInvalidate(pSym);

				m_Undo.DereferenceItem(pSym);
				pUserLayer->DeleteSymbol(pSym);
			}
		}
	}

	m_pAGDoc->SetModified(true);

#endif NOTUSED //j

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnConvertToJPG(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	if (!m_Select.SymIsImage())
		return E_FAIL;

	if (m_Select.GetImageSym()->GetSourceType() == "JPG")
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CAGLayer* pActiveLayer = pPage->GetActiveLayer();
	if (!pActiveLayer)
		return E_FAIL;

	int nQuality = (id - IDC_CONVERT_TO_JPG) * 5;
	CAGSymImage* pSym = new CAGSymImage(m_Select.GetImageSym(), nQuality);
	if (!pSym)
		return E_FAIL;

	// Let the clipboard object know that we are deleting this symbol
	Clipboard.DeleteObject(m_Select.GetSym());

	// Change the symbol
	m_Undo.CheckPoint(IDC_CONVERT_TO_JPG, m_Select.GetSym(), (long)pSym);
	m_pAGDoc->SetModified(true);
	int iOffset = pActiveLayer->FindSymbol(m_Select.GetSym());
	m_Select.GetSym()->SetDeleted(true); // The actual delete happens when the undo list is cleared
	pActiveLayer->AddSymbol(pSym);
	pActiveLayer->OrderSymbol(pSym, iOffset);

	// Select the newly modified symbol
	SymbolSelect(pSym);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnArtGallery(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	m_pCtp->LaunchClipArtPopup(m_hWnd, CCtp::MyArtGalleryCallback, (LPARAM)m_pCtp);
		
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnImageEditor(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.SymIsImage())
		m_Select.LaunchImageEditor();

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnIMEColorArt(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	m_Select.LaunchImageEditor(IDD_IME_RECOLOR_TAB);
		
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnIMECrop(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	m_Select.LaunchImageEditor(IDD_IME_CROP_TAB);
		
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnIMEFlip(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	m_Select.LaunchImageEditor(IDD_IME_FLIP_ROTATE_TAB);
		
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnIMEContrast(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	m_Select.LaunchImageEditor(IDD_IME_CONTRAST_BRIGHTNESS_TAB);
		
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnIMEColorTint(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	m_Select.LaunchImageEditor(IDD_IME_COLORS_TAB);
		
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnIMERedEye(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	m_Select.LaunchImageEditor(IDD_IME_REDEYE_TAB);
		
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnIMEBorder(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	m_Select.LaunchImageEditor(IDD_IME_BORDERS_TAB);
		
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnIMESpecialEffects(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	m_Select.LaunchImageEditor(IDD_IME_EFFECTS_TAB);
		
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocHidden(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	if (!m_pAGDoc)
		return E_FAIL;

	if (m_Select.NoSymSelected())
	{
		CMessageBox::NoObjectSelected();
		return E_FAIL;
	}

	CAGSym* pSym = m_Select.GetSym();
	if (!pSym)
		return E_FAIL;

	// Change the symbol
	m_Undo.CheckPoint(IDC_DOC_HIDDEN, pSym);
	m_pAGDoc->SetModified(true);
	pSym->SetHidden(!pSym->IsHidden());

	// Unselect the current symbol
	// after we hide it so it will go away
	SymbolUnselect(true/*bClearPointer*/);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocShowHidden(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	m_bShowHiddenSymbols = !m_bShowHiddenSymbols;

	PageInvalidate();
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnCheckForUpdates(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CString strImgHost = m_pCtp->GetContextImgHost();
	CString strManifestURL = strImgHost + String(IDS_MANIFEST_URL);
	m_pCtp->m_pAutoUpdate->Start(false/*bSilent*/, strManifestURL);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnAbout(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

	CString strCpHost = m_pCtp->GetContextCpHost();
	int i = strCpHost.Find("://");
	strCpHost = strCpHost.Mid(i+3);

	CString strVersion = String(IDS_MSGBOXTITLE) + " plugin version " + VER_PRODUCT_VERSION_STR;

	CString strMessage;
	if (!m_pCtp->IsHpUI())
	{
		strMessage.Format("Welcome %s!\n%s\n%s\n%s", 
			m_pCtp->GetContextUser(),
			strVersion,
			VER_COPYRIGHT, 
			strCpHost);
	}
	else
	{
		strMessage.Format("Welcome to HP Design Studio 2.0\n%s\n%s\n%s", 
			"by AmericanGreetings.com.",
			strVersion,
			VER_COPYRIGHT);
	}

	CMessageBox::Message(strMessage);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::SetCascadePoint(const POINT& pt)
{
	m_ptCascade = pt;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::CascadeRect(RECT& rect)
{
	if (!m_pAGDoc)
		return;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return;

	CAGLayer* pActiveLayer = pPage->GetActiveLayer();
	if (!pActiveLayer)
		return;

	// Get the page size minus the non-printable margins
	SIZE PageSize = {0,0};
	pPage->GetPageSize(PageSize);
	RECT Margins = {0,0,0,0};
	m_Print.GetMargins(m_pAGDoc, Margins);
//j	PageSize.cx -= (2 * max(Margins.left, Margins.right));
//j	PageSize.cy -= (2 * max(Margins.top, Margins.bottom));

	// If there are no symbols on the page (or the cascade point is null), start over in the center
	int nSymbols = pActiveLayer->GetNumSymbolsVisible();
	if (!nSymbols || (!m_ptCascade.x && !m_ptCascade.y))
	{
		m_ptCascade.x = PageSize.cx / 2;
		m_ptCascade.y = PageSize.cy / 2;
	}

	// Adjust the rect to be centered on the cascade point
	int midx = (rect.right + rect.left) / 2;
	int midy = (rect.bottom + rect.top) / 2;
	::OffsetRect(&rect, m_ptCascade.x - midx, m_ptCascade.y - midy);

	// Check to see if the rect hangs off the page - if so, put it top left
	if (rect.left < Margins.left || rect.right  >= (PageSize.cx - Margins.right) ||
		rect.top  < Margins.top  || rect.bottom >= (PageSize.cy - Margins.bottom))
	{
		::OffsetRect(&rect, Margins.left - rect.left, Margins.top - rect.top);
		int midx = (rect.right + rect.left) / 2;
		int midy = (rect.bottom + rect.top) / 2;
		m_ptCascade.x = midx;
		m_ptCascade.y = midy;
	}

	// Now calculate a new cascade point based on the grid size
	bool bGrid = m_Grid.IsOn();
	SIZE GridSize = m_Grid.GetSize();
	int dx = (bGrid ? GridSize.cx : INCHES(.125));
	int dy = (bGrid ? GridSize.cy : INCHES(.125));

	m_ptCascade.x += dx;
	m_ptCascade.y += dy;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnCheckSpelling(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	return DoSpellCheck();
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::DoSpellCheck()
{
	CWaitCursor Wait;

	if (!m_pAGDoc)
		return E_FAIL;

	CSpell Spell(CAGDocSheetSize::GetMetric());
	if (!Spell.Init(true/*bCheckFiles*/))
	{
		// Use a post or the download will fail when this is called from a button click (for somne unknown reason)
		PostMessage(WM_COMMAND, IDC_DOC_DOWNLOADSPELLCHECK);
		return S_OK;
	}

	int nCurrentPageNum = m_pAGDoc->GetCurrentPageNum();
	CAGSym* pSym = m_Select.GetSym();

	// Unselect the current symbol
	SymbolUnselect(true/*bClearPointer*/);

	DWORD dwWordsChecked = 0;
	DWORD dwWordsChanged = 0;
	DWORD dwErrorsDetected = 0;
	bool bAborted = false;

	int nPages = m_pAGDoc->GetNumPages();
	for (int i = 0; i < nPages && !bAborted; i++)
	{
		CAGPage* pPage = m_pAGDoc->GetPage(i);
		if (!pPage)
			continue;

		m_pAGDoc->SetCurrentPageNum(i);

		int nLayers = pPage->GetNumLayers();
		for (int nLayer = 0; nLayer < nLayers && !bAborted; nLayer++)
		{
			CAGLayer* pLayer = pPage->GetLayer(nLayer);
			if (!pLayer)
				continue;

			int nSymbols = pLayer->GetNumSymbols();
			for (int nSymbol = 0; nSymbol < nSymbols && !bAborted; nSymbol++)
			{
				m_Select.SetSym(pLayer->GetSymbol(nSymbol));
				if (m_Select.NoSymSelected() || !m_Select.SymIsText())
					continue;

				CAGSymText* pText = m_Select.GetTextSym();
				if (!pText)
					continue;

				// Ignore special cardback text symbols
				if (pText->GetID() == IDR_CARDBACK_COPYRIGHT)
					continue;

				DWORD dwChecked = 0;
				DWORD dwChanged = 0;
				DWORD dwErrors = 0;
				bAborted = pText->SpellCheck(Spell, &dwChecked, &dwChanged, &dwErrors, false/*bCheckSelection*/);
				if (!bAborted && dwChanged && i == nCurrentPageNum)
					SymbolInvalidate(pText);
				dwWordsChecked += dwChecked;
				dwWordsChanged += dwChanged;
				dwErrorsDetected += dwErrors; 
			}
		}
	}

	m_pAGDoc->SetCurrentPageNum(nCurrentPageNum);

	// Select the previously selected symbol
	SymbolSelect(pSym);

	LPCSTR strMgsEx = (bAborted ? "has been cancelled" : "is complete");
	CMessageBox::Message(String("The spell check %s.\n\n%d word(s) checked\n%d word(s) changed", strMgsEx, dwWordsChecked, dwWordsChanged));
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
UINT CALLBACK CDocWindow::OpenFileHookProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	// The entire purpose of this hook proc is to center the dialog over its owner
	switch (uiMsg)
	{
		case WM_INITDIALOG:
		{
			OPENFILENAME* lpOpenFileStruct = (OPENFILENAME*)lParam;
			return false; // not handled
		}

		// This is necessary because, when the dialog comes up, Windows moves it to its last location.
		// WM_NOTIFY appears to be the first place we can reverse this unwanted move
		case WM_NOTIFY:
		{
			LPOFNOTIFY pNotify = (LPOFNOTIFY)lParam; 
			if (!pNotify)
				break;
			if (!pNotify->lpOFN)
				break;
			if (pNotify->hdr.code != CDN_INITDONE &&
				pNotify->hdr.code != CDN_SELCHANGE &&
				pNotify->hdr.code != CDN_FOLDERCHANGE)
				break;

			HWND hParent = pNotify->lpOFN->hwndOwner;
			CWindow Dialog(::GetParent(hDlg));
			if (pNotify->hdr.code == CDN_INITDONE)
			{ // too soon
			}
			else
			if (pNotify->hdr.code == CDN_FOLDERCHANGE)
			{ // too soon
			}
			else
			if (pNotify->hdr.code == CDN_SELCHANGE)
			{
				if (!pNotify->lpOFN->lCustData)
				{
					Dialog.CenterWindow(hParent);
					pNotify->lpOFN->lCustData = true;
				}
			}
			return false; // not handled
		}

		default:
			break;
	}

	return false; // not handled
}

#ifdef _DEBUG_TEST_PDF

/////////////////////////////////////////////////////////////////////////////
#include "pdflib.h"
#include <math.h> // for pow function
#pragma warning (disable : 4305) // double to float
#pragma warning (disable : 4244) // double to float

/////////////////////////////////////////////////////////////////////////////
// build a 4-digit ASCII hex representation of an int, even on EBCDIC platforms
static void bin2ascii_hex(char* buf, int n)
{
    static const char digits[] = "\060\061\062\063\064\065\066\067\070\071\101\102\103\104\105\106";
    for (int i = 3; 0 <= i; --i)
    {
		buf[i] = digits[n & 0x0F];
		n >>= 4;
    }

    buf[4] = 0;
}

/////////////////////////////////////////////////////////////////////////////
static void TestPdfOutput()
{
	#define PDFFILE		"C:\\Documents and Settings\\jmccurdy\\Desktop\\PdfTest\\pdftest.pdf"
	#define PNGFILE		"C:\\Documents and Settings\\jmccurdy\\Desktop\\PdfTest\\pdflib.png"
	#define GIFFILE		"C:\\Documents and Settings\\jmccurdy\\Desktop\\PdfTest\\machine.gif"
	#define JPGFILE		"C:\\Documents and Settings\\jmccurdy\\Desktop\\PdfTest\\nesrin.jpg"
	#define FILENAME	"C:\\Documents and Settings\\jmccurdy\\Desktop\\PdfTest\\pdftest.txt"

	PDF_boot();
	PDF* p = PDF_new();

	if (PDF_open_file(p, PDFFILE) == -1)
	{
//j		printf("Couldn't open PDF file '%s'!\n", PDFFILE);
		return S_OK;
	}

	PDF_set_info(p,	"Keywords",	"create print americangreetings");
	PDF_set_info(p,	"Subject", g_szAppName + " Project Output as PDF");
	PDF_set_info(p,	"Title", g_szAppName);
	PDF_set_info(p,	"Creator", "AmericanGreetings.com");
	PDF_set_info(p,	"Author", "Output by " + g_szAppName);

//j	png_image(p);
	{
		int	image = PDF_open_image_file(p, "png", PNGFILE, "", 0);
		if (image != -1)
		{
			PDF_begin_page(p, a4_width, a4_height);
			PDF_add_bookmark(p, "PNG image", 0, 1);
			PDF_place_image(p, image, 0.0, 0.0, 1.0);
			PDF_close_image(p, image);
			PDF_end_page(p);
		}
	}

//j	gif_image(p);
	{
		int	image = PDF_open_image_file(p, "gif", GIFFILE, "", 0);
		if (image != -1)
		{
			PDF_begin_page(p, a4_width, a4_height);
			PDF_add_bookmark(p, "GIF image", 0, 1);
			PDF_place_image(p, image, 0.0, 0.0, 1.0);
			PDF_close_image(p, image);
			PDF_end_page(p);
		}
	}

//j	jpg_image(p);
	{
		int	image = PDF_open_image_file(p, "jpeg", JPGFILE, "", 0);
		if (image != -1)
		{
			PDF_begin_page(p, a4_width, a4_height);
			PDF_add_bookmark(p, "JPG image", 0, 1);
			// first image - fit image to page width
			float scale = a4_width / PDF_get_value(p, "imagewidth", (float)image);
			PDF_place_image(p, image, 0.0, a4_height - PDF_get_value(p, "imageheight", (float)image) * scale, scale);
			// second image
			scale = 0.5;
			PDF_save(p);
			PDF_rotate(p, 90.0);
			PDF_place_image(p, image, 0.0, (a4_width * -1), scale);
			PDF_restore(p);
			// third image
			scale = 0.25;
			PDF_save(p);
			PDF_rotate(p, 45.0);
			PDF_place_image(p, image, 200.0, 0.0, scale);
			PDF_restore(p);
			PDF_close_image(p, image);
			PDF_end_page(p);
		}
	}

//j	bookmarks(p);
	{
	#define UNICODEINFO		"\376\377\000\061\000\056\000\101\0\0"
	#define LEFT			50
	#define SIZEFONT		24
	#define LEAD			((int)(SIZEFONT * 1.5))

		PDF_begin_page(p, a4_width, a4_height);
		int font = PDF_findfont(p, "Helvetica", "host", 0);
		PDF_setfont(p, font, SIZEFONT);
		float y = 700;
		PDF_show_xy(p, "The bookmarks for this page contain all", LEFT, y-=LEAD);
		PDF_show_xy(p, "Unicode characters.", LEFT, y-=LEAD);
		PDF_show_xy(p, "Depending on the available fonts,", LEFT, y-=LEAD);
		PDF_show_xy(p, "only a certain subset will be visible on", LEFT, y-=LEAD);
		PDF_show_xy(p, "your system.", LEFT, y-=LEAD);
	    
		// private Unicode info entry
		PDF_set_info(p, "Revision", UNICODEINFO);

		// Generate Unicode bookmarks
		int level1 = PDF_add_bookmark(p, "Unicode bookmarks", 0, 0);
		int level2 = 0;
		int level3 = 0;

		for (int c = 0; c < 65535; c += 16)
		{
			if (c % 4096 == 0)
			{
				CString strBuffer;
				strBuffer.Format("U+%04X", c);
				level2 = PDF_add_bookmark(p, (char*)strBuffer, level1, 0);
			}
			if (c % 256 == 0)
			{
				CString strBuffer;
				strBuffer.Format("U+%04X", c);
				level3 = PDF_add_bookmark(p, (char*)strBuffer, level2, 0);
			}

			unsigned char tmp[64];
			tmp[0] = '\125';	// ASCII 'U'
			tmp[1] = '\053';	// ASCII '+'
			bin2ascii_hex((char*)tmp+2, c);
			tmp[6] = '\072';	// ASCII ':'
			tmp[7] = '\040';	// ASCII ' '
			tmp[8] = 0;

			// write the Unicode byte order mark
			unsigned char buf[64];
			strcpy((char*)buf, "\376\377");

			for (int pos = 0; tmp[pos/2];)
			{
				buf[2 + pos] = 0;
				pos++;
				buf[2 + pos] = tmp[pos/2];	// construct Unicode string
				pos++;
			}

			pos += 2; // account for the BOM
			for (int i = 0; i < 16; i++)
			{	// continue filling buf with chars
				buf[pos++] = (unsigned char)((((c+i)) & 0xFF00) >> 8);
				buf[pos++] = (unsigned char)(((c+i)) & 0x00FF);
			}

			// signal end of string with two null bytes
			buf[pos++] = '\0';
			buf[pos] = '\0';

			PDF_add_bookmark(p, (char*)buf, level3, 1);
		}

		PDF_end_page(p);

	#undef SIZEFONT
	#undef LEFT
	#undef UNICODEINFO
	#undef LEAD
	}

//j	character_table(p);
	{
	#define LEFT		50
	#define TOP			700
	#define SIZEFONT	16
	#define FONTNAME	"Times-Roman"

		PDF_begin_page(p, a4_width, a4_height);
		PDF_add_bookmark(p, "Host Encoding", 0, 1);
		int font = PDF_findfont(p, FONTNAME, "host", 0);
		PDF_setfont(p, font, 2*SIZEFONT);
		PDF_show_xy(p, "Host Encoding", LEFT, TOP+2*SIZEFONT);
		PDF_show(p, " (");
		PDF_show(p, PDF_get_parameter(p, "fontencoding", 0.0));
		PDF_show(p, ")");
		PDF_setfont(p, font, SIZEFONT);

		char text[10];
		text[1] = 0;
		float y = TOP;
		for (int i = 2; i < 16; i++)
		{
			y -=  2*SIZEFONT;
			float x = LEFT;
			for (int j = 0; j < 16; j++)
			{
				text[0] = (char)(i*16 + j);
				PDF_show_xy(p, text, x, y);
				x += 2*SIZEFONT;
			}
		}

		PDF_end_page(p);

	#undef LEFT
	#undef TOP
	#undef SIZEFONT
	#undef FONTNAME
	}

//j	grid(p);
	{
	#define STEP		10
	#define SIZEFONT	10.0
	#define THICK		1.0
	#define THIN		0.01
	#define FONTNAME	"Helvetica-Bold"
	#define DELTA		9.0
	#define RADIUS		12.0

		PDF_begin_page(p, a4_width, a4_height);
		PDF_add_bookmark(p, "Grid", 0, 1);
		PDF_setlinewidth(p, THIN);
		PDF_setdash(p, 1.0, 2.0);

		// draw vertical lines
		for (int i = 0; i < a4_width; i += STEP)
		{
			PDF_save(p);
			if (!(i % 100))
				PDF_setlinewidth(p, THICK);
			if (!(i % 50))
				PDF_setdash(p, 0.0, 0.0);
			PDF_moveto(p, (float)i, 0.0);
			PDF_lineto(p, (float)i, a4_height);
			PDF_stroke(p);
			PDF_restore(p);
		}

		// draw horizontal lines
		for (i = 0; i < a4_height; i += STEP)
		{
			PDF_save(p);
			if (!(i % 50))
				PDF_setdash(p, 0.0, 0.0);
			if (!(i % 100))
				PDF_setlinewidth(p, THICK);
			PDF_moveto(p, 0.0, (float)i);
			PDF_lineto(p, a4_width, (float)i);
			PDF_stroke(p);
			PDF_restore(p);
		}

		int font = PDF_findfont(p, FONTNAME, "host", 1);
		PDF_setfont(p, font, SIZEFONT);

		// print captions
		for (i = 100; i < a4_width; i+= 100)
		{
			PDF_save(p);
			PDF_setcolor(p, "fill", "gray", 1.0, 0, 0, 0);
			PDF_circle(p, (float)i, 20.0, RADIUS);
			PDF_fill(p);
			PDF_restore(p);
			CString strBuffer;
			strBuffer.Format("%d", i);
			PDF_show_xy(p, strBuffer, (float)i - DELTA, 20.0 - SIZEFONT/3);
		}

		for (i = 100; i < a4_height; i+= 100)
		{
			PDF_save(p);
			PDF_setcolor(p, "fill", "gray", 1.0, 0, 0, 0);
			PDF_circle(p, 40.0, (float)i, RADIUS);
			PDF_fill(p);
			PDF_restore(p);
			CString strBuffer;
			strBuffer.Format("%d", i);
			PDF_show_xy(p, strBuffer, 40.0 - DELTA, (float)i - SIZEFONT/3);
		}

		PDF_end_page(p);

	#undef STEP
	#undef SIZEFONT
	#undef DELTA
	#undef RADIUS
	#undef FONTNAME
	#undef THICK
	#undef THIN
	}


//j	shaded_circle(p);
	{
		// generate perceptual linear color blend
		PDF_begin_page(p, a4_width, a4_height);
		PDF_add_bookmark(p, "Shaded circle", 0, 1);

		float gray = 0.1;
		float r = (float)pow(1.0/gray, 1.0/255.0);
		for (int i = 255; i >= 0; i--)
		{
			PDF_setcolor(p, "fill", "rgb", gray, gray, 1.0, 0.0);
			PDF_circle(p, 300.0, 400.0, (float)i);
			PDF_fill(p);
			gray *= r;
		}

		PDF_end_page(p);
	}

//j	annotations(p);
	{
	#define MIMETYPE	"text/plain"
	#define NOTETEXT	"Hi! I'm a text annotation!"
	#define GREEKTEXT	"\376\377\003\233\003\237\003\223\003\237\003\243\0\0"
	#define RUSSIANTEXT	"\376\377\004\037\004\036\004\024\004\040\004\043\004\023\004\020\0\0"

		PDF_begin_page(p, a4_width, a4_height);
		PDF_add_bookmark(p, "Annotations", 0, 1);
		PDF_attach_file(p, 100, 200, 150, 250, FILENAME, "C source file", "Thomas Merz", MIMETYPE, "graph");
		PDF_set_border_style(p, "dashed", 3.0);
		PDF_set_border_color(p, 0.0, 0.0, 1.0);
		PDF_set_border_dash(p, 5.0, 1.0);
		PDF_add_note(p, 200, 400, 300, 450, NOTETEXT, "Thomas Merz", "comment", 0);
		PDF_set_border_style(p, "dashed", 4.0);
		PDF_set_border_color(p, 1.0, 0.0, 0.0);
		PDF_set_border_dash(p, 1.0, 4.0);
		PDF_add_note(p, 200, 600, 550, 750, GREEKTEXT, "Greek annotation", "insert",1);
		PDF_add_note(p, 100, 500, 400, 650, RUSSIANTEXT, "Russian annotation", "paragraph", 1);
		PDF_add_launchlink(p, 300, 300, 400, 350, "../readme.txt");
		PDF_set_border_color(p, 0.0, 1.0, 0.0);
		PDF_set_border_dash(p, 3.0, 2.0);
		PDF_add_pdflink(p, 400, 200, 500, 250, "../doc/PDFlib-manual.pdf", 2, "fitpage");
		PDF_set_border_color(p, 1.0, 1.0, 0.0);
		PDF_set_border_dash(p, 3.0, 2.0);
		PDF_add_locallink(p, 500, 100, 550, 150, 8, "fitwidth");
		PDF_set_border_style(p, "solid", 7.0);
		PDF_set_border_color(p, 0.0, 1.0, 1.0);
		PDF_set_border_dash(p, 0.0, 0.0);
		PDF_add_weblink(p, 100, 150, 300, 250, "http://www.pdflib.com");
		PDF_end_page(p);

	#undef NOTETEXT
	#undef MIMETYPE
	#undef RUSSIANTEXT
	#undef GREEKTEXT
	}

//j	centered_text(p);
	{
	#define SIZEFONT	24
	#define CENTER		290
	#define TOP			750

	// We don't want to use metrics files for the sample, and therefore
	// stick to host encoding. For this reason the sample text (which
	// uses German special characters) must be platform-specific.
	// The three flavors only differ in the special characters.
		char* title = "Der Zauberlehrling";
		char* subtitle ="Johann Wolfgang von Goethe";
		char* poem[] = {
				"Hat der alte Hexenmeister",
				"Sich doch einmal wegbegeben!",
				"Und nun sollen seine Geister",
				"Auch nach meinem Willen leben.",
				"Seine Wort\222 und Werke",
				"Merkt ich und den Brauch,",
				"Und mit Geistesst\344rke",
				"Tu ich Wunder auch.",
				"Walle! walle",
				"Manche Strecke,",
				"Da\337, zum Zwecke,",
				"Wasser flie\337e",
				"Und mit reichem, vollem Schwalle",
				"Zu dem Bade sich ergie\337e.",
				NULL };

		PDF_begin_page(p, a4_width, a4_height);
		PDF_add_bookmark(p, "Centered text", 0, 1);
		int titlefont = PDF_findfont(p, "Helvetica-Bold", "host", 1);
		PDF_setfont(p, titlefont, (1.5 * SIZEFONT));
		float y = TOP;
		PDF_show_boxed(p, title, CENTER, y, 0, 0, "center", "");
		y -= (1.5 * SIZEFONT);
		PDF_setfont(p, titlefont, SIZEFONT/2);
		PDF_show_boxed(p, subtitle, CENTER, y, 0, 0, "center", "");
		y -= (3.0 * SIZEFONT);
		int textfont = PDF_findfont(p, "Times-Roman", "host", 1);
		PDF_setfont(p, textfont, SIZEFONT);

		for (char** cp = poem; *cp; cp++)
		{
			PDF_show_boxed(p, *cp, CENTER, y, 0, 0, "center", "");
			y -= (1.5 * SIZEFONT);
		}

		PDF_end_page(p);

	#undef SIZEFONT
	#undef RIGHT
	#undef TOP
	}

//j	memory_image(p);
	{
	#define LEFT	50.0

		int width = 256;
		int height = 1;
		int bpc = 8;
		int components = 3;

		float sx = 3.0;		// desired horizontal scaling factor
		float sy = 128.0;	// desired height of one color band

		char* buf = (char*)calloc((size_t)(width * height * components), 1);
		if (!buf)
		{
//j			fprintf(stderr, "Not enough memory for memory image!\n");
			return S_OK;
		}

		// now fill the buffer with fake image data (simple color ramp)
		int i;
		char* bp;
		for (bp = buf, i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				*bp++ = (char)(j % 256); // red blend
				*bp++ = 0;
				*bp++ = 0;
			}
		}

		// In positioning the images below, we will have to compensate for the scaling.
		int image = PDF_open_image(p, "raw", "memory", buf, width*height*components, width, height, components, bpc, "");
		if (image == -1)
		{
//j			fprintf(stderr, "Not enough memory for memory image!\n");
			free(buf);
			return S_OK;
		}

		PDF_begin_page(p, 900, 600);
		PDF_add_bookmark(p, "Memory image", 0, 1);

		// Since the image interface doesn't support non-proportional
		// scaling, we will use PDF_scale() instead to stretch the image.
		PDF_scale(p, sx, sy); // stretch image

		float y = LEFT;
		PDF_place_image(p, image, LEFT/sx, y/sy, 1.0);
		PDF_close_image(p, image);

		for (bp = buf, i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				*bp++ = 0;
				*bp++ = (char)(j % 256); // green blend
				*bp++ = 0;
			}
		}

		y += height * sy; // position the image

		image = PDF_open_image(p, "raw", "memory", buf, width*height*components, width, height, components, bpc, "");
		if (image == -1)
		{
//j			fprintf(stderr, "Not enough memory for memory image!\n");
			free(buf);
			PDF_end_page(p);
			return S_OK;
		}

		PDF_place_image(p, image, LEFT/sx, y/sy, 1.0);
		PDF_close_image(p, image);

		for (bp = buf, i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				*bp++ = 0;
				*bp++ = 0;
				*bp++ = (char)(j % 256); // blue blend
			}
		}

		y += height * sy; // position the image

		image = PDF_open_image(p, "raw", "memory", buf, width*height*components, width, height, components, bpc, "");
		if (image == -1)
		{
//j			fprintf(stderr, "Not enough memory for memory image!\n");
			free(buf);
			PDF_end_page(p);
			return S_OK;
		}

		PDF_place_image(p, image, LEFT/sx, y/sy, 1.0);
		PDF_close_image(p, image);

		for (bp = buf, i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				*bp++ = (char)(j % 256); // gray blend
			}
		}

		y += height * sy; // position the image

		components	= 1; // now a single component image
		image = PDF_open_image(p, "raw", "memory", buf, width*height*components, width, height, components, bpc, "");
		if (image == -1)
		{
//j			fprintf(stderr, "Not enough memory for memory image!\n");
			free(buf);
			PDF_end_page(p);
			return S_OK;
		}

		PDF_place_image(p, image, LEFT/sx, y/sy, 1.0);
		PDF_close_image(p, image);

		free(buf);
		PDF_end_page(p);

	#undef LEFT
	}

//j	radial_structure(p);
	{
		PDF_begin_page(p, a4_width, a4_height);
		PDF_add_bookmark(p, "Radial structure", 0, 1);

		PDF_translate(p, 300.0, 400.0);
		PDF_setlinewidth(p, 0.1);

		// better solution: don't accumulate rounding errors
		for (int alpha = 0; alpha < 360; alpha++)
		{
			PDF_save(p);
			PDF_rotate(p, (float)alpha);
			PDF_moveto(p, 0.0, 0.0);
			PDF_lineto(p, 250.0, 0.0);
			PDF_stroke(p);
			PDF_restore(p);
		}

		PDF_end_page(p);
	}

//j	random_data_graph(p);
	{
	#define STEP	10.0
	#define MARGIN	50.0
	#define RIGHT	500.0
	#define TOP		800.0

		PDF_begin_page(p, a4_width, a4_height);
		PDF_add_bookmark(p, "Random graph", 0, 1);
		PDF_setlinewidth(p, 2);
		PDF_setcolor(p, "stroke", "gray", 0.0, 0.0, 0.0, 0.0); // black
		PDF_moveto(p, RIGHT, MARGIN);
		PDF_lineto(p, MARGIN, MARGIN);
		PDF_lineto(p, MARGIN, TOP);
		PDF_stroke(p);
		PDF_setlinewidth(p, 1);

		// construct some random graph data
		PDF_setcolor(p, "stroke", "rgb", 1.0, 0.0, 0.0, 0.0); //red
		PDF_moveto(p, MARGIN, MARGIN);
		for (float x=MARGIN; x<RIGHT; x+=STEP)
			PDF_lineto(p, x, x + (TOP-MARGIN)/2.0*rand()/(RAND_MAX+1.0));
		PDF_stroke(p);

		PDF_setcolor(p, "stroke", "rgb", 0.0, 1.0, 0.0, 0.0); //green
		PDF_moveto(p, MARGIN, MARGIN);
		for (float x=MARGIN; x<RIGHT; x+=STEP)
			PDF_lineto(p, x, MARGIN + TOP*(x-MARGIN)*(x-MARGIN)/((RIGHT-MARGIN)*(RIGHT-MARGIN)));
		PDF_stroke(p);

		PDF_setcolor(p, "stroke", "rgb", 0.0, 0.0, 1.0, 0.0); //blue
		PDF_moveto(p, MARGIN, MARGIN);
		for (float x=MARGIN; x<RIGHT; x+=STEP)
			PDF_lineto(p, x, MARGIN + x + MARGIN*rand()/(RAND_MAX+1.0));
		PDF_stroke(p);
		PDF_end_page(p);

	#undef STEP
	#undef MARGIN
	#undef RIGHT
	#undef TOP
	}

	PDF_close(p);
	PDF_delete(p);
	PDF_shutdown();

#endif _DEBUG_TEST_PDF
