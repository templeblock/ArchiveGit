#include "stdafx.h"
#include "CpDesktop.h"
#include "CpDialog.h"
#include "RegKeys.h"
#include "Internet.h"
#include "Resource.h"
#include "Utility.h"
#include "VersionInfo.h"
#include "MessageBox.h"

extern CString g_szAppName;
extern CCpDesktopModule _AtlModule;

/////////////////////////////////////////////////////////////////////////////
CAutoUpdate::CAutoUpdate(CCpDialog* pCpDlg)
{
	m_pCpDlg = pCpDlg;
	m_bSilent = false;
}

/////////////////////////////////////////////////////////////////////////////
CAutoUpdate::~CAutoUpdate()
{
}

/////////////////////////////////////////////////////////////////////////////
bool CAutoUpdate::Start(bool bSilent, CString& strManifestURL)
{
	// Check the online status to see if we need to defer this operation
	CInternet Internet;
	if (!Internet.IsOnline(false/*bForceLANOnline*/))
	{
		if (!bSilent)
			CMessageBox::Message(String(IDS_AUTOUPDATE_ONLINE_PROMPT));

		return false;
	}

	if (bSilent)
	{
		if (!IsSilentUpdateAllowed())
			return false;

		RegisterSilentUpdate();
	}

	m_bSilent = bSilent;
	
	// Go get the manifest file
	int index = max(strManifestURL.ReverseFind('/'), strManifestURL.ReverseFind('\\'));
	m_strManifestPath = strManifestURL.Left(index + 1);
	m_strManifestFile = RandomFileName();

	if (!m_pCpDlg->m_Download.Init(1/*iFileType*/, CAutoUpdate::MyGetManifestCallback, (LPARAM)m_pCpDlg, DL_KILLINPROGRESS))
		return false;

	m_pCpDlg->m_Download.AddFile(strManifestURL, m_strManifestFile, NULL);
	m_pCpDlg->m_Download.Start(true/*m_bGoOnline*/);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAutoUpdate::IsSilentUpdateAllowed()
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return true;
	
	DWORD dwSize = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_AUTOUPDATE, szBuffer, &dwSize) != ERROR_SUCCESS)
		return true;

	COleDateTime Date;
	bool bOK = Date.ParseDateTime(CString(szBuffer), 0/*dwFlags*/);
	if (!bOK)
		return true;

	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString strDate;
	strDate.Format("%d/%d/%d", Now.GetMonth(), Now.GetDay(), Now.GetYear());
	bOK = Now.ParseDateTime(CString(strDate), 0/*dwFlags*/);
	if (!bOK)
		return true;

	return (Date != Now);
}

/////////////////////////////////////////////////////////////////////////////
bool CAutoUpdate::RegisterSilentUpdate()
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;
	
	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString strDate;
	strDate.Format("%d/%d/%d", Now.GetMonth(), Now.GetDay(), Now.GetYear());
	if (regkey.SetStringValue(REGVAL_AUTOUPDATE, strDate) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CAutoUpdate::MyGetManifestCallback(LPARAM lParam, void* pDownloadVoid)
{
	CCpDialog* pCpDlg = (CCpDialog*)lParam;
	DOWNLOAD* pDownload = (DOWNLOAD*)pDownloadVoid;
	if (pDownload)
		return true;

	pCpDlg->m_AutoUpdate.HandleManifest();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CAutoUpdate::HandleManifest()
{
	if (!::FileExists(m_strManifestFile))
		return;

	if (FileSize(m_strManifestFile) > 0)
	{
		if (LoadManifest(m_strManifestFile))
			ProcessManifest();
		else
		if (!m_bSilent)
			CMessageBox::Message(String(IDS_MANIFEST_ERROR));
	}

	::DeleteFile(m_strManifestFile);
}

/////////////////////////////////////////////////////////////////////////////
bool CAutoUpdate::LoadManifest(CString& strManifestFile)
{
	m_CommandList.clear();

	try
	{
		CComPtr<IXMLDOMDocument> pXMLDoc;
		pXMLDoc.CoCreateInstance(CLSID_DOMDocument);
		if (pXMLDoc == NULL)
			return false;

		VARIANT_BOOL bOK;
		pXMLDoc->load(CComVariant(strManifestFile), &bOK); 
		if (!bOK)
			return false;
		
		// Get commands node
		CComPtr<IXMLDOMNodeList> pNodeList;
		pXMLDoc->getElementsByTagName(CComBSTR("commands"), &pNodeList);
		if (pNodeList == NULL)
			return false;

		CComBSTR bstrNodeName;

		// Process all of the "commands" nodes - probably only one
		while (1)
		{
			CComPtr<IXMLDOMNode> pNode;
			pNodeList->nextNode(&pNode);
			if (pNode == NULL)
				break;

			pNode->get_nodeName(&bstrNodeName);
			if (bstrNodeName != "commands")
				continue;

			CComPtr<IXMLDOMNodeList> pChildNodeList;
			pNode->get_childNodes(&pChildNodeList);
			if (pChildNodeList == NULL)
				continue;

			// Process the child nodes - look for the "command" nodes
			while (1)
			{
				CComPtr<IXMLDOMNode> pNode;
				pChildNodeList->nextNode(&pNode);
				if (pNode == NULL)
					break;

				pNode->get_nodeName(&bstrNodeName);
				if (bstrNodeName != "command")
					continue;
					
				// Process all of "command" node's attributes
				CComPtr<IXMLDOMNamedNodeMap> pAttributes;
				pNode->get_attributes(&pAttributes);
				if (pAttributes == NULL)
					continue;

				CString strType;
				CString strName;
				CString strVersion;
				DWORD dwFlags = 0;
				while (1)
				{
					CComPtr<IXMLDOMNode> pAttribute;
					pAttributes->nextNode(&pAttribute);
					if (pAttribute == NULL)
						break;
						
					CComBSTR bstrNodeName;
					pAttribute->get_nodeName(&bstrNodeName);
					CComVariant vNodeValue;
					pAttribute->get_nodeValue(&vNodeValue);
					vNodeValue.ChangeType(VT_BSTR);
					CString strValueString(vNodeValue.bstrVal);

					if (bstrNodeName == "type")
						strType = strValueString;
					else
					if (bstrNodeName == "name")
						strName = strValueString;
					else
					if (bstrNodeName == "version")
						strVersion = strValueString;
					else
					if (bstrNodeName == "flags")
						dwFlags = _tcstoul(strValueString, 0, 16);
				}

				// Store them off to be processed after we return
				DOWNLOAD_COMMAND_STRUCT DCS; 
				DCS.strType = strType;
				DCS.strName = strName;
				DCS.strVersion = strVersion;
				DCS.dwFlags = dwFlags;
				m_CommandList.push_back(DCS);
			}
		}
	}
	catch(...)
	{
		return false;
	}	

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAutoUpdate::ProcessManifest()
{
	if (!m_pCpDlg)
		return false;

	bool bStartDownload = false;
	bool bDownloadRejected = false;
	for (UINT nIndex = 0; nIndex < m_CommandList.size(); nIndex++)
	{
		DOWNLOAD_COMMAND_STRUCT* pCmd = &m_CommandList[nIndex];
		if (pCmd->strType != "file")
			continue;

		if (bDownloadRejected)
			continue; // Skip all files after the user rejects the download

		CString strFilePath = pCmd->strName;
		if (strFilePath.Find("http://") < 0)
			strFilePath = m_strManifestPath + strFilePath;

		// Check the file version in the registry and, if needed, download the file
		CString strDestPath;
		GetDestLocation(strFilePath, strDestPath);
		bool bTestRegistryVersion = (pCmd->dwFlags & UPDATE_TESTREGISTRYVERSION);
		if (FileUpToDate(strDestPath, pCmd->strVersion, bTestRegistryVersion))
			continue;

		if (!bStartDownload)
		{
			bDownloadRejected = (CMessageBox::Message(String(IDS_NEWVERSION_PROMPT), MB_YESNO) == IDNO);
			if (bDownloadRejected)
				continue;

			bStartDownload = m_pCpDlg->m_Download.Init(1/*iFileType*/, CAutoUpdate::MyUpdateCallback, (LPARAM)m_pCpDlg, DL_KILLINPROGRESS);
		}

		if (bStartDownload)
			m_pCpDlg->m_Download.AddFile(strFilePath, strDestPath, (int)(LPVOID)pCmd);
	}

	if (bStartDownload)
		m_pCpDlg->m_Download.Start(true/*bGoOnline*/, g_szAppName + " Update");
	else
	if (!bDownloadRejected)
	{
		if (!m_bSilent)
			CMessageBox::Message(String(IDS_CURRENTVERSION_PROMPT));
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CAutoUpdate::MyUpdateCallback(LPARAM lParam, void* pDownloadVoid)
{
	CCpDialog* pCpDlg = (CCpDialog*)lParam;
	DOWNLOAD* pDownload = (DOWNLOAD*)pDownloadVoid;
	if (pDownload)
	{ // A file has successfully been downloaded
		DOWNLOAD_COMMAND_STRUCT* pCmd = (DOWNLOAD_COMMAND_STRUCT*)pDownload->iData;
		if (pCmd->dwFlags & UPDATE_SAVEREGISTRYVERSION)
			SaveRegistryVersion(pDownload->strDstPath, pCmd->strVersion);

		if (pCmd->dwFlags & UPDATE_EXECUTE)
		{
			bool bCloseApp = (pCpDlg->m_hWnd && ::IsWindow(pCpDlg->m_hWnd));
			if (bCloseApp)
				CMessageBox::Message("Your application will be closed now to install the updates.\n\n");

			::ShellExecute(NULL, "open", pDownload->strDstPath, NULL, NULL, SW_SHOWNORMAL);

			if (bCloseApp)
				::PostMessage(pCpDlg->m_hWnd, WM_INSTALLBEGIN, 0, 0);
		}

	//	if (pCmd->dwFlags & UPDATE_RESOURCES)
	//	{
	//		pCtp->GetExtensions().Reload();
	//		CMessageBox::Message(String(IDS_VERSIONUPDATED_PROMPT));
	//	}
	//	
	//	if (pCmd->dwFlags & UPDATE_REGISTER)
	//	{ //j Someday, add some code to handle this case
	//	}
	//	
		return true;
	}
		
	// The final callback
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CAutoUpdate::GetDestLocation(CString strFilePath, CString& strDestPath)
{
	if (!m_pCpDlg)
		return;

	char szModule[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szModule, sizeof(szModule));
	CString strFolder = CString(szModule);

	strFolder = strFolder.Left(strFolder.ReverseFind(_T('\\')) + 1);
	int index = max(strFilePath.ReverseFind('/'), strFilePath.ReverseFind('\\'));
	CString strFileName = strFilePath.Mid(index + 1);
	strDestPath = strFolder + strFileName;
}

/////////////////////////////////////////////////////////////////////////////
bool CAutoUpdate::FileUpToDate(const CString& strFilePath, const CString& strManifestVersion, bool bTestRegistryVersion)
{
	DWORD dwFileVersion = 0;

	if (!bTestRegistryVersion)
	{
		if (!FileExists(strFilePath))
			return false;

		dwFileVersion = GetFileVersion(strFilePath);
	}
	else
	{
		CRegKey regkey;
		if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
			return false;

		int index = max(strFilePath.ReverseFind('/'), strFilePath.ReverseFind('\\'));

		DWORD dwSize = MAX_PATH;
		TCHAR szBuffer[MAX_PATH];
		if (regkey.QueryStringValue(REGVAL_HPAPP_VERSION, szBuffer, &dwSize) == ERROR_SUCCESS)
		{
			CString strFileVersion = szBuffer;
			dwFileVersion = Version(strFileVersion);
		}
	}

	return (Version(strManifestVersion) <= dwFileVersion);
}

/////////////////////////////////////////////////////////////////////////////
bool CAutoUpdate::SaveRegistryVersion(const CString& strFilePath, const CString& strVersion)
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	int index = max(strFilePath.ReverseFind('/'), strFilePath.ReverseFind('\\'));
	regkey.SetStringValue(REGVAL_HPAPP_VERSION, strVersion);
	return true;
}
