#include "stdafx.h"
#include "CpDesktop.h"
#include "CpDialog.h"
#include "AboutDlg.h"
#include "XmlFileSystem.h"
#include "Regkeys.h"
#include "Utility.h"
#include "WorkspaceDialog.h"
#include "Internet.h"
#include "HtmlDialog.h"
#include "MessageBox.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

extern CString g_szAppName;


BEGIN_DHTML_EVENT_MAP(CCpDialog)
	DHTML_EVENT_ONCLICK(_T("Launch"), OnLaunch)
	DHTML_EVENT_ONCLICK(_T("Help"), OnHelp)
	DHTML_EVENT_ONCLICK(_T("AutoUpdate"), OnAutoUpdate)
	DHTML_EVENT_ONCLICK(_T("RegStartup"), OnMyStartup)
	DHTML_EVENT_ONCLICK(_T("LaunchApp"), OnLaunchApp)
END_DHTML_EVENT_MAP()

BEGIN_MESSAGE_MAP(CCpDialog, CDHtmlDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_MESSAGE(WM_INSTALLBEGIN, OnInstallBegin)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CCpDialog::CCpDialog(CWnd* pParent /*=NULL*/) : 
	CDHtmlDialog(CCpDialog::IDD, NULL/*CCpDialog::IDH*/, pParent),
	m_AutoUpdate(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(GetIconId());
}

/////////////////////////////////////////////////////////////////////////////
CCpDialog::~CCpDialog()
{
}

/////////////////////////////////////////////////////////////////////////////
void CCpDialog::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CCpDialog::OnInstallBegin(WPARAM wParam, LPARAM lParam)
{
	ATLTRACE("On Install Begin\n");

	EndDialog(IDOK);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
bool CCpDialog::ShowCheckUpdateDialog()
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return true;
	
	DWORD dwSize = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_SHOWAUTOUPDATEDLG, szBuffer, &dwSize) != ERROR_SUCCESS)
		return true;
	
	return !(atoi(szBuffer)==1);
}

/////////////////////////////////////////////////////////////////////////////
bool CCpDialog::SetCheckUpdateDialog(CString val)
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(REGVAL_SHOWAUTOUPDATEDLG, val) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CCpDialog::DoAutoUpdate()
{
	// check if they are connected to the internet
	CInternet Internet;

	if (Internet.GetIsOnline())
	{
		CString strManifestURL("http://di.imgag.com/imgag/cp/install/hpmanifest.xml");
		//CString strManifestURL("http://localhost/dataupload/hpmanifest.xml");
		m_AutoUpdate.Start(true, strManifestURL);
		return;
	}

	if (!ShowCheckUpdateDialog())
		return;

	CHtmlDialog dlg("CheckUpdate_0.htm", true, ::GetActiveWindow());
	dlg.SetWindowSize(560, 340);
	dlg.SetResizable(false);
	dlg.SetScrollbars(false);
	dlg.DoModal();

	CString szBtnClick;
	CString szNoShowDlg;
	dlg.GetControlValue("BtnClick", szBtnClick);
	dlg.GetControlValue("NoShowDlg", szNoShowDlg);

	SetCheckUpdateDialog(szNoShowDlg);

	while (szBtnClick == "YES")
	{
		if (Internet.GetIsOnline())
		{
			CString strManifestURL("http://di.imgag.com/imgag/cp/install/hpmanifest.xml");
			//CString strManifestURL("http://localhost/dataupload/manifest.xml");
			m_AutoUpdate.Start(false, strManifestURL);
			return;
		}

		CHtmlDialog dlg("CheckUpdate.htm", true, ::GetActiveWindow());
		dlg.SetWindowSize(500, 154);
		dlg.SetResizable(false);
		dlg.SetScrollbars(false);
		dlg.DoModal();

		szBtnClick = dlg.GetReturnString();
	}

	if (szBtnClick != "YES")
	{
		// probably display message that they can always 
		// check for updates from within the application
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCpDialog::OnInitDialog()
{
	CMessageBox::SetDefaults(m_hWnd, g_szAppName, "MessageBox_hp.htm");
	DoAutoUpdate();

	CString strAppRootFolder = GetHomeFolder() + String(GetFolderId());

	CString strContentRootFolder;
	CRegKey regkey;
	if (regkey.Open(HKEY_CURRENT_USER, REGKEY_APP) == ERROR_SUCCESS)
	{
		ULONG nLen = 255;
		char* pszValue = new char[nLen+1];

		*pszValue = 0;
		nLen = 256;
		if (regkey.QueryStringValue(REGVAL_CONTENT_FOLDER, pszValue, &nLen) == ERROR_SUCCESS)
		{
			pszValue[nLen] = 0;
			strContentRootFolder = CString(pszValue);
		}

		delete [] pszValue;
	}

	if (strContentRootFolder.IsEmpty())
		strContentRootFolder = GetHomeFolder() + String(IDS_CONTENT_FOLDER);

	DWORD dwCount = CheckForUiUpdates(strAppRootFolder);
	CheckForNewContent(strContentRootFolder, strAppRootFolder, (dwCount > 0)/*bForceCreate*/);

	// Update the variables.xsl and linelist.xml files
	SaveAppVariablesFile(strContentRootFolder, strAppRootFolder);
	SaveContentVariablesFile(strContentRootFolder, strAppRootFolder);

	CString strCaption;
	strCaption.LoadString(GetTitleId());
	SetWindowText(strCaption);

	// Initialize the url prior to calling CDHtmlDialog::OnInitDialog()
	if (m_strCurrentUrl.IsEmpty())
		m_strCurrentUrl = "about:blank";

	CDHtmlDialog::OnInitDialog();

	// This magically makes the scroll bars appear and dis-allows text selection
	DWORD dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME | DOCHOSTUIFLAG_DIALOG; // DOCHOSTUIFLAG_NO3DOUTERBORDER;
	SetHostFlags(dwFlags);

	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(false);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icons for this dialog
	SetIcon(m_hIcon, true);		// Set big icon
	SetIcon(m_hIcon, false);	// Set small icon

	// Set the window size and position
	CRect rect;
	rect.SetRect(0, 0, 800, 600);
	if (0) // Restore the saved window size and position
	{
		DWORD dwSize = sizeof(rect);
		regkey.QueryBinaryValue(REGVAL_LOCATION, &rect, &dwSize);

		//j Someday, clip the rect to the display
		SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
	}
	else
	{
		int cx, cy; 
		HDC dc = ::GetDC(NULL); 
		cx = GetDeviceCaps(dc,HORZRES);
		cy = GetDeviceCaps(dc,VERTRES);
		::ReleaseDC(0,dc); 

		// Put window on top and expand it to fill screen
		SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE); 

		// to remove the border and stuff
		//{
		//	int cx, cy; 
		//	HDC dc = ::GetDC(NULL); 
		//	cx = GetDeviceCaps(dc,HORZRES) + 
		//		GetSystemMetrics(SM_CXBORDER); 
		//	cy = GetDeviceCaps(dc,VERTRES) +
		//		GetSystemMetrics(SM_CYBORDER); 
		//	::ReleaseDC(0,dc); 

		//	// Remove caption and border
		//	SetWindowLong(m_hWnd, GWL_STYLE, 
		//		GetWindowLong(m_hWnd, GWL_STYLE) & 
		//	(~(WS_CAPTION | WS_BORDER))); 

		//	// Put window on top and expand it to fill screen
		//	::SetWindowPos(m_hWnd, HWND_TOPMOST, 
		//		-(GetSystemMetrics(SM_CXBORDER)+1), 
		//		-(GetSystemMetrics(SM_CYBORDER)+1), 
		//		cx+1,cy+1, SWP_NOZORDER); 

		//	//...		
		//}
		//SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
		//CenterWindow();
	}

	CString strHomePage = strAppRootFolder + String(IDS_HOME_PAGE);
	if (FileExists(strHomePage))
	{
		CString strVal = GetShowWelcomeScreen();
		strHomePage += "?ShStartUp=" + strVal;
		Navigate(strHomePage);
	}

	ShowWindow(SW_NORMAL);

	DragAcceptFiles(false);

	return true;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCpDialog::PreTranslateMessage(MSG* pMsg) 
{
	if (!pMsg)
		return false;

	if (!m_hWnd)
		return false;

	if (!m_pBrowserApp)
		return false;

	// check if the browser control wants to handle the message
	CComQIPtr<IOleInPlaceActiveObject> spActiveObject = m_pBrowserApp;
	if (!spActiveObject)
		return false;

	return (spActiveObject->TranslateAccelerator(pMsg) == S_OK);
}

/////////////////////////////////////////////////////////////////////////////
bool CCpDialog::CheckForNewContent(CString& strContentRootFolder, CString& strAppRootFolder, bool bForceCreate)
{
	if (strContentRootFolder.IsEmpty())
		return false;

	CString strLinelistPage = strAppRootFolder + String(IDS_LINELIST_PAGE);
	bool bLinelistExists = FileExists(strLinelistPage);

	// Check the registry for the last saved file count, if the linelist file exists
	DWORD dwOldFileCount = 0;
	CRegKey regkey;
	if (regkey.Open(HKEY_CURRENT_USER, REGKEY_APP) == ERROR_SUCCESS)
	{
		if (bLinelistExists && !bForceCreate)
			regkey.QueryDWORDValue(REGVAL_FILECOUNT, dwOldFileCount);
	}

	CXmlFileSystem xml;
	xml.SetFileSpec("*.*");
	CString strLinelistFolder = strContentRootFolder + String(IDS_LINELIST_FOLDER);
	DWORD dwFileCount = xml.FileSystemCount(strLinelistFolder);
	if (dwFileCount == dwOldFileCount)
		return false;

	if (!xml.CreateXml(strLinelistFolder))
		return false;

	if (bLinelistExists)
	{
		DWORD dwFileAttributes = ::GetFileAttributes(strLinelistPage);
		if (dwFileAttributes && FILE_ATTRIBUTE_READONLY)
			::SetFileAttributes(strLinelistPage, dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);
	}

	if (!xml.Save(strLinelistPage))
		return false;

	// Write the file count out to the registry so we can detect changes in the future
	if (regkey.m_hKey != NULL)
		regkey.SetDWORDValue(REGVAL_FILECOUNT, dwFileCount);

	#ifdef _DEBUG
	if (dwOldFileCount)
		MessageBox("New content has been installed on your machine.  It has been imported automatically.", String(GetTitleId()), MB_OK);
	#endif _DEBUG

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CCpDialog::GetFileTimes(LPCSTR strFilePath, FILETIME& TimeCreation, FILETIME& TimeAccessed, FILETIME& TimeModfied)
{
	HANDLE hFile = ::CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	bool bOK = !!::GetFileTime(hFile, &TimeCreation, &TimeAccessed, &TimeModfied);
	CloseHandle(hFile);
	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
bool CCpDialog::SetFileTimes(LPCSTR strFilePath, FILETIME& TimeCreation, FILETIME& TimeAccessed, FILETIME& TimeModfied)
{
	HANDLE hFile = ::CreateFile(strFilePath, FILE_WRITE_ATTRIBUTES | GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	bool bOK = !!::SetFileTime(hFile, &TimeCreation, &TimeAccessed, &TimeModfied);
	CloseHandle(hFile);
	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCpDialog::UpdateFolder(LPCSTR strSrcFolder, LPCSTR strDstFolder, bool bIncludeSubFolders)
{
	CString strSrcPath = strSrcFolder + CString("*.*");

	DWORD dwFileCount = 0;
	CFileFind FileFind;
	BOOL bFound = FileFind.FindFile(strSrcPath);
	while (bFound)
	{
		bFound = FileFind.FindNextFile();
		if (FileFind.IsDots())
			continue;

		CString strSrcFilePath = FileFind.GetFilePath();
		CString strSrcFileName = FileFind.GetFileName();

		if (FileFind.IsDirectory())
		{
			if (!bIncludeSubFolders)
				continue;

			CString strNewDstFolder = CString(strDstFolder) + strSrcFileName + "\\";
			bool bCreated = !!::CreateDirectory(strNewDstFolder, NULL);
			if (bCreated)
				dwFileCount++;

			CString strNewSrcFolder = strSrcFilePath + "\\";
			dwFileCount += UpdateFolder(strNewSrcFolder, strNewDstFolder);
			continue;
		}

		FILETIME SrcCreationTime;
		FILETIME SrcAccessedTime;
		FILETIME SrcModfiedTime;
		FileFind.GetCreationTime(&SrcCreationTime);
		FileFind.GetLastAccessTime(&SrcAccessedTime);
		FileFind.GetLastWriteTime(&SrcModfiedTime);

		CString strDstFilePath = CString(strDstFolder) + strSrcFileName;
		if (FileExists(strDstFilePath))
		{
			FILETIME DstCreationTime;
			FILETIME DstAccessedTime;
			FILETIME DstModfiedTime;
			bool bOK = GetFileTimes(strDstFilePath, DstCreationTime, DstAccessedTime, DstModfiedTime);
			if (CFileTime(SrcModfiedTime) <= CFileTime(DstModfiedTime))
				continue;

			DWORD dwFileAttributes = ::GetFileAttributes(strDstFilePath);
			if (dwFileAttributes && FILE_ATTRIBUTE_READONLY)
				::SetFileAttributes(strDstFilePath, dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);
		}

		bool bCopied = !!::CopyFile(strSrcFilePath, strDstFilePath, false/*bFailIfExists*/);
		if (bCopied)
		{
			dwFileCount++;
			SetFileTimes(strDstFilePath, SrcCreationTime, SrcAccessedTime, SrcModfiedTime);
		}
	}
	
	FileFind.Close();
	return dwFileCount;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCpDialog::CheckForUiUpdates(CString& strAppRootFolder)
{
	if (strAppRootFolder.IsEmpty())
		return 0;

	CString strUpdatesFolder = strAppRootFolder + String(IDS_UIUPDATES_FOLDER);

	// Be sure the update folder and sub-folders exist
	bool bCreated = false;
	bCreated = !!::CreateDirectory(strUpdatesFolder, NULL);
	bCreated = !!::CreateDirectory(strUpdatesFolder + "cp\\", NULL);
	bCreated = !!::CreateDirectory(strUpdatesFolder + "images\\", NULL);

	// Update the app folder from the updates folder
	DWORD dwCount = UpdateFolder(strUpdatesFolder, strAppRootFolder);

	// Update the updates folders from the app folder
	dwCount += UpdateFolder(strAppRootFolder, strUpdatesFolder, false/*bIncludeSubFolders*/);
	dwCount += UpdateFolder(strAppRootFolder + "cp\\", strUpdatesFolder + "cp\\");
	dwCount += UpdateFolder(strAppRootFolder + "images\\", strUpdatesFolder + "images\\");

	return dwCount;
}

/////////////////////////////////////////////////////////////////////////////
bool CCpDialog::SaveAppVariablesFile(CString& strContentRootFolder, CString& strAppRootFolder)
{
	if (strContentRootFolder.IsEmpty())
		return false;

	CString strVariablesPage = strAppRootFolder + String(IDS_VARIABLES_PAGE);
	if (FileExists(strVariablesPage))
	{
		DWORD dwFileAttributes = ::GetFileAttributes(strVariablesPage);
		if (dwFileAttributes && FILE_ATTRIBUTE_READONLY)
			::SetFileAttributes(strVariablesPage, dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);
	}

	FILE* output = fopen(strVariablesPage, "wb");
	if (!output)
		return false;

	CString strContentRootPath = strContentRootFolder;
	strContentRootPath.Replace("\\", "/");

	CString strThumbslistPath = strContentRootFolder + String(IDS_THUMBSLIST_FOLDER);
	strThumbslistPath.Replace("\\", "/");

	fprintf(output, "<?xml version='1.0' encoding='iso-8859-1' ?>\n");
	fprintf(output, "<xsl:stylesheet version='1.0'  xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>\n");
	fprintf(output, "\t<xsl:variable name='content-path' select=\"'%s'\" />\n", strContentRootPath);
	fprintf(output, "\t<xsl:variable name='thumbslist-path' select=\"'%s'\" />\n", strThumbslistPath);
	fprintf(output, "</xsl:stylesheet>\n");

	fclose(output);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CCpDialog::SaveContentVariablesFile(CString& strContentRootFolder, CString& strAppRootFolder)
{
	if (strContentRootFolder.IsEmpty())
		return false;

	CString strVariablesPage = strContentRootFolder + String(IDS_VARIABLES_PAGE);
	if (FileExists(strVariablesPage))
	{
		DWORD dwFileAttributes = ::GetFileAttributes(strVariablesPage);
		if (dwFileAttributes && FILE_ATTRIBUTE_READONLY)
			::SetFileAttributes(strVariablesPage, dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);
	}

	FILE* output = fopen(strVariablesPage, "wb");
	if (!output)
		return false;

	CString strApplicationPath = strAppRootFolder;
	strApplicationPath.Replace("\\", "/");

	CString strContentRootPath = strContentRootFolder;
	strContentRootPath.Replace("\\", "/");

	CString strProductPath = strContentRootFolder + String(IDS_PRODUCT_FOLDER);
	strProductPath.Replace("\\", "/");

	fprintf(output, "<?xml version='1.0' encoding='iso-8859-1' ?>\n");
	fprintf(output, "<xsl:stylesheet version='1.0'  xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>\n");
	fprintf(output, "\t<xsl:variable name='content-path' select=\"'%s'\" />\n", strContentRootPath);
	fprintf(output, "\t<xsl:variable name='product-path' select=\"'%s'\" />\n", strProductPath);
	fprintf(output, "\t<xsl:variable name='application-path' select=\"'%s'\" />\n", strApplicationPath);
	fprintf(output, "</xsl:stylesheet>\n");

	fclose(output);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCpDialog::ShowContextMenu(DWORD dwID, POINT* ppt, IUnknown* pcmdtReserved, IDispatch* pdispReserved)
{
// Pass back S_FALSE for the standard menu, S_OK for no menu
#ifdef _DEBUG //j
	return S_FALSE; 
#else
	return S_OK;
#endif _DEBUG
}

/////////////////////////////////////////////////////////////////////////////
void CCpDialog::OnOK()
{
	// This catches the Enter key and avoids default dialog processing
}

/////////////////////////////////////////////////////////////////////////////
void CCpDialog::OnCancel()
{
	// This catches the Escape key and avoids default dialog processing
}

/////////////////////////////////////////////////////////////////////////////
void CCpDialog::OnClose()
{
	// Store the window location in the registty
	CRegKey regkey;
	if (regkey.Create(HKEY_CURRENT_USER, REGKEY_APP) == ERROR_SUCCESS)
	{
		WINDOWPLACEMENT placement;
		placement.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(&placement);
		regkey.SetBinaryValue(REGVAL_LOCATION, &placement.rcNormalPosition, sizeof(RECT));
	}

	int i = m_WorkspaceArray.GetSize();
	while (--i >= 0)
	{
		CWorkspaceDialog* pWorkspace = m_WorkspaceArray[i];
		pWorkspace->Close();
		if (!pWorkspace->m_hWnd)
			delete pWorkspace;
		m_WorkspaceArray.RemoveAt(i);
	}
	
	CDHtmlDialog::OnClose();
	EndDialog(IDCANCEL);
}

/////////////////////////////////////////////////////////////////////////////
void CCpDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout(this);
		dlgAbout.DoModal();
		return;
	}

	CDHtmlDialog::OnSysCommand(nID, lParam);
}

/////////////////////////////////////////////////////////////////////////////
CString CCpDialog::GetHomeFolder()
{
	char szAppPath[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szAppPath, sizeof(szAppPath));
	CString strAppPath = szAppPath;

	int i = strAppPath.ReverseFind('\\');
	CString strHomeFolder = strAppPath.Left(i + 1);

	strAppPath.MakeLower();
	int iDebug = strAppPath.Find("\\debug\\");
	if (iDebug > 0)
		strHomeFolder = strHomeFolder.Left(iDebug + 1);
	int iRelease = strAppPath.Find("\\release\\");
	if (iRelease > 0)
		strHomeFolder = strHomeFolder.Left(iRelease + 1);

	return strHomeFolder;
}

/////////////////////////////////////////////////////////////////////////////
void CCpDialog::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		return;
	}

	CDHtmlDialog::OnPaint();
}

/////////////////////////////////////////////////////////////////////////////
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCpDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCpDialog::OnAutoUpdate(IHTMLElement *pElement)
{
	DoAutoUpdate();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
CString CCpDialog::GetShowWelcomeScreen()
{
	CString strVal = "1";
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return strVal;
	
	DWORD dwSize = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_SHOWWELCOMESCRN, szBuffer, &dwSize) != ERROR_SUCCESS)
		return strVal;
	
	strVal = szBuffer;
	return (strVal == "0" ? "0" : "1");
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCpDialog::OnLaunchApp(IHTMLElement* pElement)
{
	if (!pElement)
		return E_FAIL;

	CString strCmdToRun;
	CComBSTR bstrVal;
	pElement->get_className(&bstrVal);
	CString strVal(bstrVal);
	strVal = strVal.MakeLower();
	if (strVal == "ize")
		strCmdToRun = "\"C:\\Program Files\\HP\\Image Zone Express\\Hp_IZE.exe\"";

	::ShellExecute(NULL, "open", strCmdToRun, NULL, NULL, SW_SHOWNORMAL);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCpDialog::OnMyStartup(IHTMLElement* pElement)
{
	// Need to write to the registry
	if (!pElement)
		return E_FAIL;

	CComBSTR bstrVal;
	pElement->get_className(&bstrVal);
	CString strVal(bstrVal);
	strVal = strVal.MakeLower();
	strVal = (strVal == "false" ? "0" : "1");

	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return S_OK;
		
	regkey.SetStringValue(REGVAL_SHOWWELCOMESCRN, strVal);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCpDialog::OnLaunch(IHTMLElement* pElement)
{
	return WindowOpen(pElement, -1, -1, 780, 601, "Workspace");
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCpDialog::OnHelp(IHTMLElement* pElement)
{
	return WindowOpen(pElement, -1, -1, 500, 600, "Help");
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCpDialog::WindowOpen(IHTMLElement* pElement, int l, int t, int w, int h, LPCSTR pstrName)
{
	if (!pElement)
		return E_FAIL;

	CComBSTR bstrURL;
	pElement->get_className(&bstrURL);
	CString strURL(bstrURL);
	int iProtocol = strURL.Find(':');
	if (iProtocol < 0 || iProtocol > 5)
	{
		CString strAppRootFolder = GetHomeFolder() + String(GetFolderId());
		strURL = strAppRootFolder + strURL;
	}

	// Get the desired window size and location
	if (l < 0) l = (::GetSystemMetrics(SM_CXSCREEN) - w) / 2;
	if (l < 0) l = 0;
	if (t < 0) t = (::GetSystemMetrics(SM_CYSCREEN) - h) / 2;
	if (t < 0) t = 0;

	RECT Rect = {l, t, l+w, t+h};
	CWorkspaceDialog* pWorkspace = CWorkspaceDialog::CreateModeless(GetDesktopWindow()/*pParent*/, strURL, &Rect, pstrName);
	if (pWorkspace)
		m_WorkspaceArray.Add(pWorkspace);
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
CString CCpDialog::String(UINT nFormatID, ...)
{
	CString strFormat;
	strFormat.LoadString(nFormatID);

	va_list argList;
	va_start(argList, nFormatID);
	CString str;
	str.FormatV(strFormat, argList);
	va_end(argList);
	return str;
}
