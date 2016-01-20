#include "stdafx.h"
#include "HelperFunctions.h"
#include "Regkeys.h"
#include "Authenticate.h"
#include "mmsystem.h"
#include "Utility.h"
#include "ModuleVersion.h"
#include "MyAtlUrl.h"
#include "HTTPClient.h"
#include "Internet.h"


#pragma comment(lib, "winmm.lib")

typedef UINT (WINAPI *LPFNENABLECLIENTS)(DWORD);
typedef UINT (WINAPI *LPFNDISABLELECLIENTS)(DWORD);


CLog* g_pLog = NULL; 
/////////////////////////////////////////////////////////////////////////////
bool StartLogging(LPCTSTR szLogFile)
{	
	if (g_pLog)
		return false;

	CAuthenticate  Authenticate;
	DWORD dwLoggingOpt=0;
	DWORD dwLogLevel=0;
	Authenticate.ReadLoggingOption(dwLoggingOpt);
	Authenticate.ReadLoggingLevelOption(dwLogLevel);
	if (dwLoggingOpt == LOGGING_OFF)
		return false;

	if (dwLoggingOpt == LOGGING_FILE)
	{
		CString szPath;
		GetInstallPath(szPath);
		szPath += szLogFile;
		g_pLog = new CLog( new CFileLog(std::string(szPath)), dwLogLevel, true ); 
	}
	else if (dwLoggingOpt == LOGGING_WINDOW)
	{
		g_pLog = new CLog(new CWinLog(), dwLogLevel, true ); 
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void EndLogging()
{
	delete g_pLog;
	g_pLog = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
BOOL CreateDir(LPCSTR lpszPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	CFuncLog log(g_pLog, "HelperFunctions::CreateDir()");
	if (::CreateDirectory(lpszPathName, lpSecurityAttributes))
		return TRUE;

	DWORD dwError = GetLastError();
	
	if (dwError != ERROR_ALREADY_EXISTS)
	{
		CString strMsg; 	
		strMsg.Format(_T("::CreateDirectory attempt to create [%s] "), lpszPathName); 
		GetError(strMsg);
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
BOOL GetInstallPath(CString &szPath)
{
	CFuncLog log(g_pLog, "HelperFunctions::GetInstallPath()");

	// Get the full path of the registered addin, includes file name.
	CRegKey regkey;
	if (regkey.Open(HKEY_CLASSES_ROOT, REGKEY_ADDIN_LOCATION) != ERROR_SUCCESS)
		return FALSE;

	DWORD dwLength = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	if (regkey.QueryStringValue(NULL, szBuffer, &dwLength) != ERROR_SUCCESS)
		return FALSE;

	// Get just the path portion, remove file name.
	szPath = szBuffer;
	int index = szPath.ReverseFind('\\');
	if (index >= 0)
		szPath = szPath.Left(index+1);

	// Ensure any double quotes are removed - Sometimes reg entries include double quotes as part of the string.
	int nCount = szPath.Replace("\"","");

#ifdef _DEBUG
	szPath = _T("C:\\My Projects\\AgApplications\\AgOutlookAddIn\\Debug\\");
#endif _DEBUG

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
bool IsDocInEditMode(IHTMLDocument2* pDoc)
{
	CFuncLog log(g_pLog, "HelperFunctions::IsDocInEditMode()");
	CComPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;

	CComBSTR bstr;
	HRESULT hr = spDoc->get_designMode(&bstr);
	if (FAILED(hr))
		return false;

	bstr.ToLower();
	return (bstr != "off");
}

/////////////////////////////////////////////////////////////////////////////
bool SetAttribute(IDispatch* pDisp, CComBSTR bsAttribute, VARIANT varAttributeValue, LONG lFlag)
{
	CFuncLog log(g_pLog, "HelperFunctions::SetAttribute()");

	CComQIPtr<IHTMLElement> spElem(pDisp);
	if (spElem == NULL)
		return false; 

	HRESULT hr = spElem->setAttribute(bsAttribute, varAttributeValue, lFlag);
	if (FAILED(hr))
	{
		CString szName(bsAttribute);
		log.LogString(LOG_ERROR, log.FormatString(" FAILURE: hr == [%X], Attribute Name == %s",hr, szName.GetBuffer()));
		return false;
	}
		
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool GetAttribute(IDispatch* pDisp, CComBSTR bsAttribute, LONG lFlag, VARIANT* varAttributeValue)
{
	CFuncLog log(g_pLog, "HelperFunctions::GetAttribute()");

	CComQIPtr<IHTMLElement> spElem(pDisp);
	if (spElem == NULL)
		return false; 

	HRESULT hr = spElem->getAttribute(bsAttribute, lFlag, varAttributeValue);
	if (FAILED(hr) || (varAttributeValue->vt == VT_NULL))
	{
		CString szName(bsAttribute);
		log.LogString(LOG_ERROR, log.FormatString(" FAILURE: hr == [%X], Attribute Name == %s",hr, szName.GetBuffer()));
		return false;
	}
		
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool IsElementContentEditable(IDispatch* pDisp)
{
	CFuncLog log(g_pLog, "HelperFunctions::IsElementContentEditable()");

	CComQIPtr<IHTMLElement> spElem(pDisp);
	if (!spElem)
		return false; 
	
	CComVariant varValue;
	if (!GetAttribute(spElem, CComBSTR(HTML_ATTRIBUTE_CONTENTEDITABLE), 0, &varValue))
		return false;

	CString szValue(varValue.bstrVal);
	szValue.MakeLower();
	return (szValue.Find("false") < 0);
}

//////////////////////////////////////////////////////////////////////////////
bool IsDocLoaded(IHTMLDocument2* pDoc, bool bInteractive)
{
	CFuncLog log(g_pLog, "HelperFunctions::IsDocLoaded()");
	CComPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;

	CComBSTR bstr;
	HRESULT hr = spDoc->get_readyState(&bstr);
	if (FAILED(hr))
		return false;

	bstr.ToLower();
	if (bInteractive)
		return (bstr == "complete" || bstr == "interactive");
	else
		return (bstr == "complete");
}

//////////////////////////////////////////////////////////////////////////////
bool IsDocLoadedWait(IHTMLDocument2* pDoc, bool bInteractive, DWORD dwLimit)
{
	CFuncLog log(g_pLog, "HelperFunctions::IsDocLoadedWait()");
	CComPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;
	
	// Wait for the document to become ready, or time out
	DWORD dwMaxTime = ::timeGetTime() + dwLimit/*10 seconds*/;
	while (!IsDocLoaded(spDoc, bInteractive))
	{
		MSG msg;
		BOOL bRet = ::GetMessage(&msg, NULL, 0, 0);
		if (!bRet)
			return false; // WM_QUIT, exit message loop

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (::timeGetTime() > dwMaxTime)
			return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
void GetProductInfoFromContent(LPCTSTR szProductRecord, CString& szProdID, DownloadParms& Parms)
{
	CFuncLog log(g_pLog, "HelperFunctions::GetProductInfoFromContent()");
	CString szProdInfo(szProductRecord);
	int curPos= 0;
	CString szCategory	= szProdInfo.Tokenize(",",curPos);
	szProdID			= szProdInfo.Tokenize(",",curPos);
	CString szPathPrice	= szProdInfo.Tokenize(",",curPos);
	CString szAssetType = szProdInfo.Tokenize(",",curPos);

	Parms.iCategory		= strtoul(szCategory.GetBuffer(), NULL, 10);
	Parms.iProductId	= strtoul(szProdID.GetBuffer(), NULL, 10);
	Parms.iPathPrice	= strtoul(szPathPrice.GetBuffer(), NULL, 10);
	Parms.iGroup		= strtoul(szAssetType.GetBuffer(), NULL, 10);
}

///////////////////////////////////////////////////////////////////////////////
//bool Wait(DWORD dwExpire)
//{
//	// Wait for the document to become ready, or time out
//	bool bExpired = false; 
//	DWORD dwMaxTime = ::timeGetTime() + dwExpire;
//	while (!bExpired)
//	{
//		MSG msg;
//		BOOL bRet = ::GetMessage(&msg, NULL, 0, 0);
//		if (!bRet)
//			return false; // WM_QUIT, exit message loop
//
//		::TranslateMessage(&msg);
//		::DispatchMessage(&msg);
//
//		if (::timeGetTime() > dwMaxTime)
//			bExpired = true;
//
//		Sleep(200);
//	}
//
//	return true;//(!(BOOL)bBusy);
//}

///////////////////////////////////////////////////////////////////////////////
bool GetVersionInfo(LPCTSTR szAppPath, VERSIONINFOSTRUCT &Version)
{
	CString szPath = szAppPath;
	if (!GetAppPath(szPath))
		return false;

	CModuleVersion ver;
	if (!ver.GetFileVersionInfo(szPath))
		return false;

	CString szVer = ver.GetValue(_T("ProductVersion"));

	// Populate VERSIONINFOSTRUCT.
	Version.szVer = szVer;
	int curPos= 0;
	CString szTemp;
	szTemp		 	= szVer.Tokenize(". ,",curPos);
	Version.iMajor  = strtoul(szTemp.GetBuffer(), NULL, 10);
	szTemp		 	= szVer.Tokenize(". ,",curPos);
	Version.iMinor  = strtoul(szTemp.GetBuffer(), NULL, 10);
	szTemp		 	= szVer.Tokenize(". ,",curPos);
	Version.iBuild  = strtoul(szTemp.GetBuffer(), NULL, 10);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool GetIEMajorVersion(DWORD &dwVersion)
{
	CString szAppPath = "iexplore.exe";

	VERSIONINFOSTRUCT Ver;
	if (!GetVersionInfo(szAppPath, Ver))
		return false;
	
	dwVersion = Ver.iMajor;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool GetSetupVersion(CString &szVer)
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_VERSION, szBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	szVer = szBuffer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool GetInstallStatus(DWORD &dwStatus)
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwBuffer = 0;
	if (regkey.QueryDWORDValue(REGVAL_INSTALL_STATUS, dwBuffer) != ERROR_SUCCESS)
		return false;

	dwStatus = dwBuffer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool SetInstallStatus(DWORD dwStatus)
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetDWORDValue(REGVAL_INSTALL_STATUS, dwStatus) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool GetHeadElement(IHTMLDocument2* pDoc, CComPtr<IHTMLElement> &spElem)
{
	CFuncLog log(g_pLog, "HelperFunctions::GetHeadElement()");
	CComQIPtr<IHTMLDocument3> spDoc3(pDoc);
	if (!spDoc3)
		return false;	

	// Find Head element.
	CComPtr<IHTMLElementCollection> spAll;
	HRESULT hr = spDoc3->getElementsByTagName(CComBSTR("head"), &spAll);
	if (!spAll)
		return false;

	long nCnt = 0;
	hr = spAll->get_length(&nCnt);
	if (FAILED(hr))
		return false;

	CComQIPtr<IHTMLElement> spHeadElem;
	for (int i=0; i<nCnt; i++)
	{
		CComPtr<IDispatch> spTagDisp;
		hr = spAll->item(CComVariant(i), CComVariant(i), &spTagDisp);
		if (FAILED(hr) || !spTagDisp)
			continue;
		
		spHeadElem = spTagDisp;
		if (spHeadElem)
		{
			spElem = spHeadElem;
			return true;
		}
	}

	// If Head element not found then we're done.
	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool GetElementByTagAndAttribute(IHTMLDocument2* pDoc, LPCTSTR szTag, LPCTSTR szAttribute, LPCTSTR szAttrValue,  CComPtr<IHTMLElement> &spElem)
{
	CFuncLog log(g_pLog, "HelperFunctions::GetElementByTagAndAttribute()");

	CComQIPtr<IHTMLDocument3> spDoc3(pDoc);
	if (!spDoc3)
		return false;	

	if (CString(szTag).IsEmpty() || CString(szAttribute).IsEmpty())
		return false;
	
	// Find the collection of szTag element.
	CComPtr<IHTMLElementCollection> spAll;
	HRESULT hr = spDoc3->getElementsByTagName(CComBSTR(szTag), &spAll);
	if (!spAll)
		return false;

	// Get enumerator to collection.
	CComPtr<IUnknown> pUnk;
	hr = spAll->get__newEnum(&pUnk);
	if (!pUnk)
		return false;

	CComQIPtr<IEnumVARIANT> spItr(pUnk);
	if (!spItr)
		return false;

	// Iterate thru each element until we find a szTag with
	// szAttribute and szAttrValue.
	CComQIPtr<IHTMLElement> spTagElem;
	VARIANT varValue; 
	VARIANT varData;
	varValue.vt = VT_NULL;
	ULONG uCount=0;
	bool bFound = false;
	while (!bFound)
	{
		hr = spItr->Next(1, &varData, &uCount);
		if (FAILED(hr) || uCount<=0)
			return false;

		spTagElem = varData.pdispVal;
		if (!spTagElem)
			return false;

		if (!GetAttribute(spTagElem, CComBSTR(szAttribute), 0, &varValue))
			return false;

		CString szValue(varValue.bstrVal);
		if (szValue.CompareNoCase(szAttrValue) == 0)
			bFound = true;
	}

	// If target element found then we're done.
	spElem = spTagElem;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool GetBody(LPCTSTR szDoc, CString &szBody)
{
	CString szDocument = szDoc;

	szDocument.MakeLower();
	int iLen = szDocument.GetLength();
	int iStart = szDocument.Find("<body");
	int iEnd = szDocument.Find("</body>");
	iLen = szDocument.GetLength();
	int iNewLen = (iEnd - iStart) + 7;
	
	if (iStart<0 || iEnd<0 || iNewLen <=0)
		return false;

	szBody = szDocument.Mid(iStart, iNewLen);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
int FindMenuItem(CMenu* pMenu, UINT MenuId)
{
	ATLASSERT(pMenu);
	ATLASSERT(::IsMenu(pMenu->m_hMenu));

	int count = pMenu->GetMenuItemCount();
	for (int i = 0; i < count; i++)
	{
		if (pMenu->GetMenuItemID(i) == MenuId)
			return i;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
int FindMenuItem(CMenu* pMenu, CString& str)
{
	ATLASSERT(pMenu);
	ATLASSERT(::IsMenu(pMenu->m_hMenu));

	int count = pMenu->GetMenuItemCount();
	for (int i = 0; i < count; i++)
	{
		CString menuString;
		int nBytes = pMenu->GetMenuString(i, menuString, MF_BYPOSITION);
		if(str.CompareNoCase(menuString) == 0)
			return i;
		}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
CString GetOptionStringValue(LPCTSTR szOptionString, LPCTSTR szNameTarget)
{
	CString szOptions = szOptionString;
	if (szOptions.IsEmpty())
		return CString("");

	if (szOptions.Find(_T('=')) < 0)
		return CString("");

	szOptions += _T('|');

	int iStart = 0;
	while (iStart >= 0 )
	{
		int iEnd = szOptions.Find(_T('|'), iStart);
		if (iEnd <= iStart)
			break;
		int iPair = szOptions.Find(_T('='), iStart);
		if (iPair > iStart && iPair < iEnd)
		{
			CString szName = szOptions.Mid(iStart, iPair-iStart);
			szName.TrimLeft();
			szName.TrimRight();
			szName.MakeLower();
			if (szName.CompareNoCase(szNameTarget) == 0)
			{
				iPair++;
				CString szValue = szOptions.Mid(iPair, iEnd-iPair);
				szValue.TrimLeft();
				szValue.TrimRight();
				UnescapeUrl(szValue, szValue);
				return szValue;
			}
		}

		iStart = iEnd + 1;
	}

	return CString("");
}

/////////////////////////////////////////////////////////////////////////////
BOOL EnableClient(DWORD dwClientType)
{
	CString szPath;
	CAuthenticate  Authenticate;
	if (!Authenticate.ReadInstallPath(szPath))
		return S_OK;

#ifdef _DEBUG
	szPath = _T("C:\\My Projects\\AgApplications\\AgOutlookAddIn\\Debug\\");
#endif _DEBUG

	szPath += _T("AgOutlookAddIn.dll");

	HINSTANCE hInst = ::LoadLibrary(szPath);
	if (!hInst)
	{
		GetError(_T("HELPER::EnableClient(DWORD dwClientType), LoadLibrary"));
		return false;
	}

	LPFNENABLECLIENTS lpfnEnableClients;
	lpfnEnableClients = (LPFNENABLECLIENTS) ::GetProcAddress(hInst, _T("EnableClients"));
	if (!lpfnEnableClients)
	{
		GetError(_T("CJMBrkr::EnableClient(DWORD dwClientType), lpfnEnableClients"));
		::FreeLibrary(hInst);
		return false;
	}

	BOOL bSuccess = (*lpfnEnableClients)(dwClientType);

	::FreeLibrary(hInst);

	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DisableClient(DWORD dwClientType)
{
	CString szPath;
	CAuthenticate  Authenticate;
	if (!Authenticate.ReadInstallPath(szPath))
		return S_OK;

#ifdef _DEBUG
	szPath = _T("C:\\My Projects\\AgApplications\\AgOutlookAddIn\\Debug\\");
#endif _DEBUG

	szPath += _T("AgOutlookAddIn.dll");

	HINSTANCE hInst = ::LoadLibrary(szPath);
	if (!hInst)
	{
		GetError(_T("CJMBrkr::DisableClient(DWORD dwClientType), LoadLibrary"));
		return false;
	}

	LPFNDISABLELECLIENTS lpfnDisableClients;
	lpfnDisableClients = (LPFNDISABLELECLIENTS) ::GetProcAddress(hInst, _T("DisableClients"));
	if (!lpfnDisableClients)
	{
		GetError(_T("HELPER::DisableClient(DWORD dwClientType), lpfnDisableClients"));
		::FreeLibrary(hInst);
		return false;
	}

	BOOL bSuccess = (*lpfnDisableClients)(dwClientType);

	::FreeLibrary(hInst);

	return bSuccess;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool EvaluateConnection(IWebBrowser2* pWB, DWORD dwTimeOut)
{
	CWaitCursor WaitCursor;

	HRESULT hr = S_OK;
	CComPtr<IWebBrowser2> spWB(pWB);
	if (!spWB)
		return false;

	
	BOOL bTimeOut = false;
	CHTTPClient Http(spWB);
	bTimeOut = !Http.Wait(dwTimeOut);

	CString szUrl;
	CComPtr<IDispatch> spDocDisp;
	hr = spWB->get_Document(&spDocDisp);
	CComQIPtr<IHTMLDocument2> spDoc2(spDocDisp);
	if (spDoc2)
	{
		CComBSTR bsUrl;
		spDoc2->get_URL(&bsUrl);
		szUrl = bsUrl;
		szUrl.MakeLower();
	}

	// If offline, network down or web site otherwise unreachable then load appropriate
	// web page.
	if (bTimeOut || !spDoc2 || szUrl.Find(_T("res://")) >= 0)
		ShowPage(spWB, PAGE_UNAVAILABLE);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ShowPage(IWebBrowser2* pWB, int nPage)
{
	HRESULT hr = S_OK;
	CComPtr<IWebBrowser2> spWB(pWB);
	if (!spWB)
		return false;

	CString szPath;
	GetPagePath(nPage, szPath);
		
	CComVariant varFlags = navNoHistory;
	CComVariant varTargetFrameName = "_self";
	hr = pWB->Navigate(CComBSTR(szPath), &varFlags, &varTargetFrameName, NULL/*&varPostData*/, NULL/*&varHeaders*/);
	if (FAILED(hr))
		return false;
	
	Wait(1000);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool GetPagePath(int nPage, CString &szPath)
{
	if (!GetInstallPath(szPath))
		return false;
		
#ifdef _DEBUG
	szPath = _T("C:\\My Projects\\AgApplications\\AgOutlookAddIn\\Setup\\");
#endif _DEBUG

	if (nPage == PAGE_UNAVAILABLE)
		szPath += _T("Offline\\Unavailable.htm");
	else if (nPage == PAGE_WAIT)
		szPath += _T("Offline\\Wait.htm");

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool RemoveDefaultStationery(DWORD dwClientType)
{
	CString szTemp;
	if (!GetInstallPath(szTemp))
		return false;

	szTemp += DEFAULT_STATIONERY_FOLDER;
	szTemp+= '\\';

	CString szClientFolder; 
	if (dwClientType & CLIENT_TYPE_EXPRESS)
	{
		szClientFolder = szTemp;
		szClientFolder += CString(_T("Express"));
		RemoveFilesFromFolder(szClientFolder, _T("htm?")); 
	}
	if (dwClientType & CLIENT_TYPE_OUTLOOK)
	{
		szClientFolder = szTemp;
		szClientFolder += CString(_T("Outlook"));
		RemoveFilesFromFolder(szClientFolder, _T("htm?")); 
	}
	if (dwClientType & CLIENT_TYPE_IE)
	{
		szClientFolder = szTemp;
		szClientFolder += CString(_T("IE"));
		RemoveFilesFromFolder(szClientFolder, _T("htm?")); 
	}

	if (dwClientType & CLIENT_TYPE_AOL)
	{
		szClientFolder = szTemp;
		szClientFolder += CString(_T("AOL"));
		RemoveFilesFromFolder(szClientFolder, _T("htm?")); 
	}
	
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool RemoveFilesFromFolder(LPCTSTR szFolder, LPCTSTR szType)
{
	// Delete all html files in default stationery folder.
	CString szFindPath = szFolder;
	if (szFindPath.Find(_T("\\")) < 0)
		return false;

	WIN32_FIND_DATA FileData;
	HANDLE hFind;
	
	// Ensure path ends with '\'
	int iLen = szFindPath.GetLength();
	if (szFindPath.Find(_T("\\")) < iLen-1)
		szFindPath += _T("\\");

	CString szPath = szFindPath;

	// Concatenate file type filter to path.
	CString szFileFilter;
	szFileFilter = _T("*.");
	szFileFilter += szType;
	szFindPath += szFileFilter; // _T("*.htm?");


	// Look for first occurence
	hFind = FindFirstFile(szFindPath, &FileData); 
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		FindClose(hFind);
		return false;
	}
	
	
	// Iterate thru files and delete.
	CString strFilePath;
	bool bDone = false;
	while (!bDone)
	{
		strFilePath = szPath;
		strFilePath += FileData.cFileName;
		if (!DeleteFile(strFilePath))
			GetError("RemoveFilesFromFolder()");

		if (!FindNextFile(hFind, &FileData)) 
		{
			if (GetLastError() != ERROR_NO_MORE_FILES) 
				GetError("FindNextFile(hFind, &FileData)");

			bDone = true; 
		}
	}
	

	FindClose(hFind);
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsFilePresent(LPCTSTR szFilePath)
{
	CString szFindPath = szFilePath;
	if (szFindPath.Find(_T("\\")) < 0)
		return false;
	
	WIN32_FIND_DATA FileData;
	HANDLE hFind;

	hFind = FindFirstFile(szFindPath, &FileData); 
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		FindClose(hFind);
		return false;
	}
	
	FindClose(hFind);
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool GetFrameInfo(IHTMLElement *pElem, CString &szName, CString &szID, CString &szSrc, CString &szParentID, CString szBody, CComPtr<IHTMLWindow2> &spContainingWindow, CComPtr<IHTMLWindow2> &spParentWindow)
{
	CComPtr<IHTMLElement> spElem(pElem);
	if (!spElem)
		return false;

	CComPtr<IDispatch> spDocDisp;
	HRESULT hr = spElem->get_document(&spDocDisp);
	CComQIPtr<IHTMLDocument2> spDoc2(spDocDisp);
	if (!spDoc2)
		return false;


	CComPtr<IHTMLElement> spBody;
	hr = spDoc2->get_body(&spBody);
	if (!spBody)
		return false;

	
	CComBSTR bsBody;
	hr = spBody->get_outerHTML(&bsBody);
	szBody = bsBody;
	

	CComPtr<IHTMLWindow2> spWindow;
	spDoc2->get_parentWindow(&spWindow);
	spContainingWindow = spWindow;
	if (!spWindow)
		return false;


	CComQIPtr<IHTMLWindow4> spWindow4(spWindow);
	CComPtr<IHTMLFrameBase> spFrame;
	spWindow4->get_frameElement(&spFrame);
	if (!spFrame)
		return false;

	CComBSTR bsSrc;
	spFrame->get_src(&bsSrc);
	szSrc = bsSrc;

	CComBSTR bsName;
	spFrame->get_name(&bsName);
	szName = bsName;

	CComQIPtr<IHTMLElement> spFrameElem(spFrame);
	if (!spFrameElem)
		return false;


	CComVariant varValue;
	if (GetAttribute(spFrameElem, CComBSTR("id"), 0, &varValue))
		szID = varValue.bstrVal;
		
	
	CComPtr<IHTMLWindow2> spParentWnd;
	spWindow->get_parent(&spParentWnd);
	spParentWindow = spParentWnd;
	if (!spParentWnd)
		return false;

	CComQIPtr<IHTMLWindow4> spParentWindow4(spParentWnd);
	if (!spParentWnd)
		return false;

	CComPtr<IHTMLFrameBase> spParentFrame;
	CComQIPtr<IHTMLWindow4> spParentWnd4(spParentWnd);
	spParentWnd4->get_frameElement(&spParentFrame);
	if (!spParentFrame)
		return false;
							

	CComQIPtr<IHTMLElement> spParentFrameElem(spParentFrame);
	if (!spParentFrameElem)
		return false;
							
										
	if (GetAttribute(spParentFrameElem, CComBSTR("id"), 0, &varValue))
		szParentID = varValue.bstrVal;
							

	return true;

}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsAttributeValuePresent(IDispatch* pDisp, LPCTSTR lpszAttribute, LPCTSTR lpszExpectedValue)
{
	CComQIPtr<IHTMLElement> spElem(pDisp);
	if (!spElem)
		return false; 

	CComVariant varValue;
	CString szActualValue;
	if (!GetAttribute(spElem, CComBSTR(lpszAttribute), 0, &varValue))
		return false;
	
	szActualValue = varValue.bstrVal;
	if (szActualValue.CompareNoCase(lpszExpectedValue) != 0)
		return false;

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
WNDPROC SetWindowProc(HWND hwnd, WNDPROC pfProc)
{
	if (!::IsWindow(hwnd))
		return NULL;

	LONG dwAddr  = reinterpret_cast<LONG>(pfProc);
	WNDPROC pfOrigProc = (WNDPROC)::SetWindowLong(hwnd, GWL_WNDPROC, dwAddr);
	return pfOrigProc;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL RestoreWindowProc(HWND hwnd, WNDPROC pfProc)
{
	if (!pfProc || !::IsWindow(hwnd))
		return false;

	LONG dwAddr = reinterpret_cast<LONG>(pfProc);
	WNDPROC pTempProc = (WNDPROC)::SetWindowLong(hwnd, GWL_WNDPROC, dwAddr);
	return (pTempProc != NULL);
}