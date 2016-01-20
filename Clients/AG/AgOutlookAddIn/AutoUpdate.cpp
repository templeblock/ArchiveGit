#include "stdafx.h"
#include "resource.h"
#include "AutoUpdate.h"
#include "Async.h"
#include "RegKeys.h"
#include "Internet.h"
#include "VersionInfo.h"
#include "Authenticate.h"

/////////////////////////////////////////////////////////////////////////////
CAutoUpdate::CAutoUpdate(CAsync* pAddin)
{
	m_pAddin = pAddin;
	m_bSilent = false;
	m_hOwner = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CAutoUpdate::~CAutoUpdate()
{
}

/////////////////////////////////////////////////////////////////////////////
bool CAutoUpdate::Start(bool bSilent)
{
	if (!m_pAddin)
		return false;

	// Check the online status to see if we need to defer this operation
	CInternet Internet;
	if (!Internet.IsOnline(false/*bForceLANOnline*/))
	{
		if (!bSilent)
			::MessageBox(m_hOwner, String(IDS_AUTOUPDATE_ONLINE_PROMPT), g_szAppName, MB_OK);

		return false;
	}

	if (bSilent)
	{
		if (!IsSilentUpdateAllowed())
			return false;

		RegisterSilentUpdate();
	}

	m_bSilent = bSilent;
	
	// Compute the manifest URL
	CString strManifestURL;
	CAuthenticate Authenticate;
	Authenticate.ReadHost(strManifestURL);
	strManifestURL += String(IDS_MANIFEST_URL);

	// Go get the manifest file
	int index = max(strManifestURL.ReverseFind('/'), strManifestURL.ReverseFind('\\'));
	m_strManifestPath = strManifestURL.Left(index + 1);
	m_strManifestFile = RandomFileName();

	if (!m_pAddin->m_Download.Init(1/*iFileType*/, CAutoUpdate::MyGetManifestCallback, (LPARAM)m_pAddin, DL_KILLINPROGRESS))
		return false;

	m_pAddin->m_Download.AddFile(strManifestURL, m_strManifestFile, NULL);
	m_pAddin->m_Download.Start(true/*m_bGoOnline*/);

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
	CAsync* pAddin = (CAsync*)lParam;
	DOWNLOAD* pDownload = (DOWNLOAD*)pDownloadVoid;
	if (pDownload) // Wait for the final callback when there are no more files to download
		return true;
		
	// The final callback
	pAddin->m_AutoUpdate.HandleManifest();
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
			::MessageBox(m_hOwner, String(IDS_MANIFEST_ERROR), g_szAppName, MB_OK);
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
					CString strValueString = V_BSTR(&vNodeValue);

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
	if (!m_pAddin)
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
			bDownloadRejected = (::MessageBox(m_hOwner, String(IDS_NEWVERSION_PROMPT), g_szAppName, MB_YESNO) == IDNO);
			if (bDownloadRejected)
				continue;

			// Set AutoUpdate status in registry to indicate update started
			SetAutoUpdateStatus(AUTOUPDATE_STATUS_STARTED);
			bStartDownload = m_pAddin->m_Download.Init(1/*iFileType*/, CAutoUpdate::MyUpdateCallback, (LPARAM)m_pAddin, DL_KILLINPROGRESS);
		}

		if (bStartDownload)
			m_pAddin->m_Download.AddFile(strFilePath, strDestPath, (int)(LPVOID)pCmd);
	}

	if (bStartDownload)
		m_pAddin->m_Download.Start(true/*bGoOnline*/, g_szAppName + " Update");
	else
	{
		if (!bDownloadRejected)
		{
			if (!m_bSilent)
				::MessageBox(m_hOwner, String(IDS_CURRENTVERSION_PROMPT), g_szAppName, MB_OK|MB_ICONEXCLAMATION);
		}
		m_pAddin->AutoUpdateDone(0);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CAutoUpdate::MyUpdateCallback(LPARAM lParam, void* pDownloadVoid)
{
	CAsync* pAddin = (CAsync*)lParam;
	DOWNLOAD* pDownload = (DOWNLOAD*)pDownloadVoid;
	if (pDownload)
	{ // A file has successfully been downloaded
		DOWNLOAD_COMMAND_STRUCT* pCmd = (DOWNLOAD_COMMAND_STRUCT*)pDownload->iData;
		if (pCmd->dwFlags & UPDATE_EXECUTE)
			::ShellExecute(NULL, "open", pDownload->strDstPath, NULL, NULL, SW_SHOWNORMAL);

		if (pCmd->dwFlags & UPDATE_RESOURCES)
		{
		}
		
		if (pCmd->dwFlags & UPDATE_REGISTER)
		{ //j Someday, add some code to handle this case
		}
		
		if (pCmd->dwFlags & UPDATE_SAVEREGISTRYVERSION)
			SaveRegistryVersion(pDownload->strDstPath, pCmd->strVersion);

		pAddin->AutoUpdateDone(pCmd->dwFlags);
		return true;
	}
		
	// The final callback
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CAutoUpdate::GetDestLocation(CString strFilePath, CString& strDestPath)
{
	if (!m_pAddin)
		return;

	CString strFolder = m_pAddin->m_Extend.GetDllLocation();
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
		CString strFileName = strFilePath.Mid(index + 1);
		CString strKeyName = "Version_" + strFileName;

		DWORD dwSize = MAX_PATH;
		TCHAR szBuffer[MAX_PATH];
		if (regkey.QueryStringValue(strKeyName, szBuffer, &dwSize) == ERROR_SUCCESS)
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
	CString strFileName = strFilePath.Mid(index + 1);
	CString strKeyName = "Version_" + strFileName;
	regkey.SetStringValue(strKeyName, strVersion);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAutoUpdate::SetAutoUpdateStatus(DWORD dwStatus)
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	bool bSuccess = (regkey.SetDWORDValue(REGVAL_AUTOUPDATE_STATUS, dwStatus) == ERROR_SUCCESS);
	regkey.Close();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////
void CAutoUpdate::SetWndOwner(HWND hwnd)
{
	m_hOwner = hwnd;
}

/////////////////////////////////////////////////////////////////////////////
bool CAutoUpdate::GetAutoUpdateStatus(DWORD &dwStatus)
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwBuffer = 0;
	if (regkey.QueryDWORDValue(REGVAL_AUTOUPDATE_STATUS, dwBuffer) != ERROR_SUCCESS)
		return false;

	dwStatus = dwBuffer;
	return true;
}
