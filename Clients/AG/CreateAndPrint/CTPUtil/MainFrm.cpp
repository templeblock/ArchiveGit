#include "stdafx.h"
#include "CTPUtil.h"
#include "MainFrm.h"
#include "AGDoc.h"
#include "AGLayer.h"
#include "AGDC.h"
#include "AGDib.h"
#include "ProgDlg.h"
#include <shlobj.h>
#include "FYSPrintDoc.h"
#include "XMLDialog.h"
#include "Logger.h"

#include <fstream>
#include <direct.h>
#include ".\mainfrm.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

#define REGKEY_APPROOT				HKEY_CURRENT_USER
#define REGKEY_APP					"Software\\American Greetings\\Create And Print Utility"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_COMMAND(ID_BUILDTHUMBS, OnBuildThumbs)
	ON_COMMAND(ID_BUILDDATASHEET, OnBuildDataSheet)
	ON_COMMAND(ID_BUILDXML, OnBuildXml)
	ON_COMMAND(ID_FILE_CLEAR, OnClear)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	Create(NULL, "Create & Print Utility", WS_OVERLAPPEDWINDOW, rectDefault, NULL, MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pLog = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
	if (m_pLog)
		delete m_pLog;

	m_pLog = NULL;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::CleanUp(CString& strCtpRoot)
{
	CString strPath;

	// Images Dir
	strPath = strCtpRoot + "\\Images";
	CleanDirectory(strPath);
	RemoveDirectory(strPath);

	// XML dir
	strPath = strCtpRoot + "\\Xml";
	CleanDirectory(strPath);
	RemoveDirectory(strPath);

	// Font
	strPath = strCtpRoot + "\\Fonts";
	CleanDirectory(strPath);
	RemoveDirectory(strPath);

	CleanDirectory(strCtpRoot);
	RemoveDirectory(strCtpRoot);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
bool CMainFrame::ValidateXml(CString& strXmlFile, CString& strSchemaFile, CString& strProjectName)
{
	::CoInitialize(NULL);

	HRESULT hr;
	MSXML2::IXMLDOMDocument2Ptr pXmlDoc;
	MSXML2::IXMLDOMSchemaCollectionPtr pSchColl;

	hr = pSchColl.CreateInstance(__uuidof(MSXML2::XMLSchemaCache40));
	hr = pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40));

	// load the cache with the schema
	try
	{
		hr = pSchColl->add(_bstr_t("http://www.4yoursoul.com/Schema/2003/ClientIntegration/DataInputForPrint"), 
						_variant_t((LPCSTR)strSchemaFile));

		if (hr == S_OK)
		{
			CComBSTR bstrVal;
			VARIANT varValue;
			VARIANT_BOOL vbIsSuccessful;

			pXmlDoc->put_async(VARIANT_FALSE);

			varValue.vt = VT_DISPATCH;
			varValue.pdispVal = pSchColl;
			hr = pXmlDoc->putref_schemas(varValue);

			vbIsSuccessful = pXmlDoc->load(_variant_t(strXmlFile));
			if (vbIsSuccessful != VARIANT_TRUE)
			{
				MSXML2::IXMLDOMParseErrorPtr pError = pXmlDoc->GetparseError();
				CString strMessage = (LPCSTR)pError->Getreason();

				m_pLog->Log(String("Error in Project: %s.ctp", strProjectName));
				m_pLog->Log(String("Failed to validate xml : %s", strXmlFile));
				m_pLog->Log(String("Error Description: %s", strMessage), true);

				return false;
			}
		}
	}
	catch (_com_error &e)
	{
		CString strMessage = e.ErrorMessage();
		CString strDesc = (LPCSTR)e.Description();

		m_pLog->Log(String("Error in Project: %s.ctp", strProjectName));
		m_pLog->Log(String("Failed to validate xml : %s", strXmlFile));
		m_pLog->Log(String("Error Description: %s", strDesc), true);

		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::CaptureFileNames()
{
	CString strSrcPath = "V:\\websitesimg\\american\\html\\product\\createprint\\";
	CString strDstPath = "C:\\Production Test\\New Crash\\";
	CString strSrch("Error in Project: ");

	DWORD dwSize = FileSize("C:\\Production Test\\Copy of ErrorFile_Dec03_2004.txt");
	FILE* input = fopen("C:\\Production Test\\Copy of ErrorFile_Dec03_2004.txt", "rb");
	if (input)
	{
		HGLOBAL hMemory = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(BYTE)*dwSize);
		BYTE* pMemory = (BYTE*)::GlobalLock(hMemory);
		fread(pMemory, sizeof(BYTE), dwSize, input);

		char *pStart = (char*)pMemory;
		
		while (pStart)
		{
			pStart = strstr(pStart, strSrch);
			if (pStart)
			{
				pStart += strSrch.GetLength();
				CString strFileName;
				for (; pStart && *pStart != '\r'; pStart++)
					strFileName += *pStart;
				if (!strFileName.IsEmpty())
				{
					strFileName += ".ctp";
					CString strSrcFileName = strSrcPath + strFileName;
					CString strDstFileName = strDstPath + strFileName;
					CopyFile(strSrcFileName, strDstFileName, true);
				}
			}
		}
	}
	fclose(input);
}

///////////////////////////////////////////////////////////////////////////////
//void CMainFrame::CaptureFileNames()
//{
//	m_pLog = new CLogger("C:\\Production Test");
//	m_pLog->SetWriteToFile();
//
//	CString strPath("C:\\Production Test");
//
//	CFileFind FileFind;
//	BOOL bFound = FileFind.FindFile(strPath);
//	while (bFound)
//	{
//		bFound = FileFind.FindNextFile();
//		if (FileFind.IsDots())
//			continue;
//
//		if (FileFind.IsDirectory())
//		{
//			CString strSrcPath = "V:\\websitesimg\\american\\html\\product\\createprint\\";
//			CString strDstPath = strPath + "\\New Crash\\";
//
//			CString strSrcFileName = FileFind.GetFilePath();
//			strSrcFileName += ".ctp";
//			m_pLog->Log(strSrcFileName);
//			strSrcFileName = strSrcPath + (LPCSTR)StrFileName(strSrcFileName);
//			CString strDstFileName = strDstPath + (LPCSTR)StrFileName(strSrcFileName);
//			MoveFile(strSrcFileName, strDstFileName);
//		}
//	}
//
//	if (m_pLog)
//		delete m_pLog;
//
//	m_pLog = NULL;
//}
//
/////////////////////////////////////////////////////////////////////////////
int CMainFrame::BuildXmls(CString& strSrcPath, CString& strDstPath, CString& strSchPath, bool bJustCount, CProgressDlg& ProgDlg, BOOL bUnpackFiles, int nTestFileCount)
{
	::CoInitialize(NULL);

	int nFileCount = 0;
	int nFailedCount = 0;
	CFileFind FileFind;
	BOOL bFound = FileFind.FindFile(strSrcPath);
	while (bFound)
	{
		bFound = FileFind.FindNextFile();
		if (FileFind.IsDots())
			continue;

		if (FileFind.IsDirectory())
			continue;

		CString strFilePath = FileFind.GetFilePath();
		CString strExtension = strFilePath.Right(3);
		strExtension.MakeLower();
		if (strExtension != "ctp")
			continue;

		// Build a thumbnail
		CString csMsg = "Processing " + strFilePath;
		ProgDlg.SetStatus(csMsg);
		if (ProgDlg.CheckCancelButton())
			break;

		nFileCount++;

		if (nFileCount == nTestFileCount)
			break;

		if (bJustCount)
			continue;

		CAGDoc* pAGDoc = new CAGDoc();
		ifstream In(strFilePath, ios::in | /*ios::nocreate |*/ ios::binary);

		CString strProjectName = strFilePath.Mid(strFilePath.ReverseFind('\\')+1);
		bool bAdjusted = false;
		if (!pAGDoc->Read(In, bAdjusted, true/*bAllowAdjustment*/))
		{
			m_pLog->Log(String("Error in Project: %s", strProjectName));
			m_pLog->Log(String("Details: Failed to read project"), true);
			nFailedCount++;
		}
		else
		{
			if (ProgDlg.CheckCancelButton())
			{
				if (pAGDoc) delete pAGDoc;
				break;
			}

			try
			{
				strProjectName = strProjectName.Left(strProjectName.GetLength()-4);
				CString strTargetPath = strDstPath + CString("\\") + strProjectName;

				CString strOrderId("BatchTest");
				CString strCorrId("BatchTest");
				CString strFYSInfo;
				CFYSPrintDoc *pFYSDoc = new CFYSPrintDoc(pAGDoc);
				pFYSDoc->SetXmlPath(strTargetPath);
				bool bRetVal = pFYSDoc->CreateXmlDoc(strOrderId, strCorrId, strFYSInfo, bUnpackFiles?true:false);
				if (!bRetVal)
				{
					m_pLog->Log(String("Error in Project: %s", strProjectName));
					m_pLog->Log(String("Details: %s", pFYSDoc->GetErrorMsg()), true);
					nFailedCount++;
				}

				if (bRetVal && bUnpackFiles)
				{
					CString strXmlFile;
					CString strSchemaFile;

					strXmlFile = strTargetPath + "\\XML\\XP_CreateAndPrint.xml";
					strSchemaFile = strSchPath + "\\ClientInterfaceSchema_CreateAndPrint.xsd";
					bRetVal = ValidateXml(strXmlFile, strSchemaFile, strProjectName);
					if (bRetVal)
					{
						strXmlFile = strTargetPath + "\\XML\\XP_StreamHdr.xml";
						strSchemaFile = strSchPath + "\\FYS_DataStreamHeader.xsd";
						bRetVal = ValidateXml(strXmlFile, strSchemaFile, strProjectName);
					}
					if (!bRetVal) nFailedCount++;
				}

				CleanUp(strTargetPath);
				delete pFYSDoc;
			}
			catch (...)
			{
				m_pLog->Log(String("Error in Project: %s", strProjectName));
				m_pLog->Log(String("Details: Project crashed"), true);
				nFailedCount++;
			}
		}

		ProgDlg.StepIt();
		delete pAGDoc;
	}

	FileFind.Close();

	if (!bJustCount)
	{
		m_pLog->Log(String("Batch Test done:"));
		m_pLog->Log(String("No. of projects tested: %ld", nFileCount), true);

		if (nFailedCount)
			m_pLog->Log(String("No. of projects failed: %ld", nFailedCount), true);
	}

	return nFileCount;
}

/////////////////////////////////////////////////////////////////////////////
int CMainFrame::BuildThumbs(CString& strPath, bool bJustCount, CProgressDlg& ProgDlg)
{
	int nFileCount = 0;
	CFileFind FileFind;
	BOOL bFound = FileFind.FindFile(strPath);
	while (bFound)
	{
		bFound = FileFind.FindNextFile();
		if (FileFind.IsDots())
			continue;

		if (FileFind.IsDirectory())
		{
			CString strFilePath = FileFind.GetFilePath();
			nFileCount += BuildThumbs(strFilePath + _T("\\*.*"), bJustCount, ProgDlg);
			continue;
		}

		CString strFilePath = FileFind.GetFilePath();
		CString strExtension = strFilePath.Right(3);
		strExtension.MakeLower();
		if (strExtension != "ctp")
			continue;

		// Build a thumbnail
		CString csMsg = "Processing " + strFilePath;
		ProgDlg.SetStatus(csMsg);
		if (ProgDlg.CheckCancelButton())
			break;

		nFileCount++;
		if (bJustCount)
			continue;

		CAGDoc* pAGDoc = new CAGDoc();
		ifstream In(strFilePath, ios::in | /*ios::nocreate |*/ ios::binary);

		bool bAdjusted = false;
		if (!pAGDoc->Read(In, bAdjusted, true/*bAllowAdjustment*/))
		{
			CString strMsg;
			strMsg.Format("Error reading project file '%s'", strFilePath);
			::MessageBox(::GetActiveWindow(), strMsg, NULL, MB_OK);
		}
		else
		{
			if (ProgDlg.CheckCancelButton())
			{
				if (pAGDoc) delete pAGDoc;
				break;
			}
		
			CAGPage* pPage = pAGDoc->GetPage(0);
			if (!pPage)
			{
				CString strMsg;
				strMsg.Format("Error reading project file '%s'", strFilePath);
				::MessageBox(::GetActiveWindow(), strMsg, NULL, MB_OK);
			}
			else
			{
				int nLast = strFilePath.ReverseFind('\\');
				CString csThumbDir  = strFilePath.Left(nLast) + "\\Thumbs";
				CString csDetailDir = strFilePath.Left(nLast) + "\\Details";

				mkdir(csThumbDir);
				mkdir(csDetailDir);

				CString szName = strFilePath.Mid(nLast);
				CString csThumbT = csThumbDir + szName + "T.bmp";
				CString csThumbD = csDetailDir + szName + "D.bmp";

				SIZE MaxSizeT = {153, 153};
				pPage->CreateThumb(MaxSizeT, csThumbT);
				SIZE MaxSizeD = {400, 400};
				pPage->CreateThumb(MaxSizeD, csThumbD);
			}

			ProgDlg.StepIt();
		}

		delete pAGDoc;
	}
	
	FileFind.Close();
	return nFileCount;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBuildXml()
{

	//CaptureFileNames();
	//return;

	CXMLDialog dlg;

	if (dlg.DoModal() != IDOK)
		return;

	CString strSrcPath;
	CString strDstPath;
	CString strSchPath;
	dlg.GetSrcPath(strSrcPath);
	dlg.GetDstPath(strDstPath);
	dlg.GetSchemaPath(strSchPath);
	int nTestFileCount = dlg.GetTestFileCount();
	BOOL bUnpackFiles = dlg.GetUnpackFiles();

	strSrcPath += _T("\\*.*");

	CProgressDlg ProgDlg(IDS_PROGRESS_THUMBS);
	ProgDlg.Create();

	m_pLog = new CLogger(strDstPath);
	m_pLog->SetWriteToFile();

	int nFileCount;
	nFileCount = BuildXmls(strSrcPath, strDstPath, strSchPath, true/*bJustCount*/, ProgDlg, bUnpackFiles, nTestFileCount);
	ProgDlg.SetRange(0, nFileCount);
	nFileCount = BuildXmls(strSrcPath, strDstPath, strSchPath, false/*bJustCount*/, ProgDlg, bUnpackFiles, nTestFileCount);

	if (m_pLog)
		delete m_pLog;

	m_pLog = NULL;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBuildThumbs()
{
	BROWSEINFO binfo;
	binfo.hwndOwner = NULL;
	binfo.pidlRoot = NULL;
	binfo.pszDisplayName = NULL;
	binfo.lpszTitle = (LPCTSTR)"Select the Root Directory of Projects";
	binfo.ulFlags = BIF_RETURNONLYFSDIRS;
	binfo.lpfn = NULL;
	binfo.lParam = 0;
	binfo.iImage = 0;

	LPITEMIDLIST pItemID = SHBrowseForFolder(&binfo);	
	if (!pItemID)
		return;

	TCHAR szPath[MAX_PATH];
	*szPath = _T('\0');
	SHGetPathFromIDList(pItemID, szPath);
	CString strPath = szPath;
	strPath += _T("\\*.*");

	CProgressDlg ProgDlg(IDS_PROGRESS_THUMBS);
	ProgDlg.Create();
	int nFileCount = BuildThumbs(strPath, true/*bJustCount*/, ProgDlg);
	ProgDlg.SetRange(0, nFileCount);
	nFileCount = BuildThumbs(strPath, false/*bJustCount*/, ProgDlg);
}

/////////////////////////////////////////////////////////////////////////////
int CMainFrame::BuildDataSheet(CString& strPath, bool bJustCount, CProgressDlg& ProgDlg, FILE* output)
{
	int nFileCount = 0;
	CFileFind FileFind;
	BOOL bFound = FileFind.FindFile(strPath);
	while (bFound)
	{
		bFound = FileFind.FindNextFile();
		if (FileFind.IsDots())
			continue;

		if (FileFind.IsDirectory())
		{
			CString strFilePath = FileFind.GetFilePath();
			nFileCount += BuildDataSheet(strFilePath + _T("\\*.*"), bJustCount, ProgDlg, output);
			continue;
		}

		CString strFilePath = FileFind.GetFilePath();
		CString strExtension = strFilePath.Right(3);
		strExtension.MakeLower();
		if (strExtension != "ctp")
			continue;

		// Make a list entry
		CString csMsg = "Processing " + strFilePath;
		ProgDlg.SetStatus(csMsg);
		if (ProgDlg.CheckCancelButton())
			break;

		nFileCount++;
		if (bJustCount)
			continue;

		CAGDoc* pAGDoc = new CAGDoc();
		ifstream In(strFilePath, ios::in | /*ios::nocreate |*/ ios::binary);

		bool bAdjusted = false;
		if (!pAGDoc->Read(In, bAdjusted, false/*bAllowAdjustment*/))
		{
			CString strMsg;
			strMsg.Format("Error reading project file '%s'", strFilePath);
			::MessageBox(::GetActiveWindow(), strMsg, NULL, MB_OK);
		}
		else
		{
			if (ProgDlg.CheckCancelButton())
			{
				if (pAGDoc) delete pAGDoc;
				break;
			}

			CString strType = CAGDocTypes::Name(pAGDoc->GetDocType());
			CString strOrientation = (pAGDoc->IsPortrait() ? "Portrait" : "Landscape");

			strFilePath.MakeUpper();
			char strFileName[MAX_PATH];
			_splitpath(strFilePath, NULL, NULL, strFileName, NULL);
			int nPages = pAGDoc->GetNumPages();
			fprintf(output, "%s, %s, %s, %d", strFileName, strType, strOrientation, nPages);

			for (int i = 0; i < nPages; i++)
			{
				CAGPage* pPage = pAGDoc->GetPage(i);
				if (pPage)
				{
					int nLayers = pPage->GetNumLayers();
					int nSymbols0 = (nLayers <= 0 ? -1 : pPage->GetLayer(0)->GetNumSymbols());
					int nSymbols1 = (nLayers <= 1 ? -1 : pPage->GetLayer(1)->GetNumSymbols());
					SIZE PageSize = {0,0};
					pPage->GetPageSize(PageSize);
					double dx = (double)PageSize.cx / APP_RESOLUTION;
					double dy = (double)PageSize.cy / APP_RESOLUTION;
					char str[10];
					fprintf(output, ", %0.5G x %0.5G, %s%s", dx, dy, (nSymbols0 ? "BAD " : ""), (nSymbols1 ? itoa(nSymbols1,str,10) : " "));
				}
			}

			fprintf(output, "\n");

			ProgDlg.StepIt();
		}

		delete pAGDoc;
	}
	
	FileFind.Close();
	return nFileCount;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBuildDataSheet()
{
	BROWSEINFO binfo;
	binfo.hwndOwner = NULL;
	binfo.pidlRoot = NULL;
	binfo.pszDisplayName = NULL;
	binfo.lpszTitle = (LPCTSTR)"Select the Root Directory of Projects";
	binfo.ulFlags = BIF_RETURNONLYFSDIRS;
	binfo.lpfn = NULL;
	binfo.lParam = 0;
	binfo.iImage = 0;

	LPITEMIDLIST pItemID = SHBrowseForFolder(&binfo);	
	if (!pItemID)
		return;

	TCHAR szPath[MAX_PATH];
	*szPath = _T('\0');
	SHGetPathFromIDList(pItemID, szPath);
	CString strPath = szPath;
	strPath += _T("\\*.*");

	FILE* output = fopen("DataSheet.csv", "w");
	if (!output)
	{
		::MessageBox(::GetActiveWindow(), "Can't open output file", NULL, MB_OK);
		return;
	}

	fprintf(output, "Product, Type, Orientation, Pages, Size\n");
	CProgressDlg ProgDlg(IDS_PROGRESS_DATASHEET);
	ProgDlg.Create();
	int nFileCount = BuildDataSheet(strPath, true/*bJustCount*/, ProgDlg, output);
	ProgDlg.SetRange(0, nFileCount);
	nFileCount = BuildDataSheet(strPath, false/*bJustCount*/, ProgDlg, output);
	fclose(output);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CFrameWnd::OnPaint() for painting messages
	if (m_pLog)
		m_pLog->Draw(&dc, m_cx, m_cy);
}

/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRegKey regkey;
	regkey.Create(REGKEY_APPROOT, REGKEY_APP);

	CClientDC dc(this);

	TEXTMETRIC Metrics;
	dc.GetTextMetrics(&Metrics);
	m_cx = Metrics.tmAveCharWidth;
	m_cy = Metrics.tmHeight;

	return 0;
}

void CMainFrame::OnClear()
{
	if (m_pLog)
		m_pLog->ClearLog();

	Invalidate();
}