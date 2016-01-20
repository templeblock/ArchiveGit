#include "stdafx.h"
#include "Ctp.h"
#include "AutoUpdate.h"
#include "Download.h"
#include "Extensions.h"
#include "RegKeys.h"
#include "Internet.h"
#include "VersionInfo.h"

/////////////////////////////////////////////////////////////////////////////
CAutoUpdate::CAutoUpdate(CCtp* pCtp)
{
	m_pCtp = pCtp;
	m_bSilent = false;
}

/////////////////////////////////////////////////////////////////////////////
CAutoUpdate::~CAutoUpdate()
{
}

/////////////////////////////////////////////////////////////////////////////
bool CAutoUpdate::Start(bool bSilent, CString& strManifestURL)
{
	if (!m_pCtp)
		return false;

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

	if (!m_pCtp->GetDownload().Init(1/*iFileType*/, CAutoUpdate::MyGetManifestCallback, (LPARAM)m_pCtp, DL_KILLINPROGRESS))
		return false;

	m_pCtp->GetDownload().AddFile(strManifestURL, m_strManifestFile, NULL);
	m_pCtp->GetDownload().Start(true/*m_bGoOnline*/);

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
	CCtp* pCtp = (CCtp*)lParam;
	DOWNLOAD* pDownload = (DOWNLOAD*)pDownloadVoid;
	if (pDownload) // Wait for the final callback when there are no more files to download
		return true;
		
	// The final callback
	pCtp->GetAutoUpdate().HandleManifest();
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
	if (!m_pCtp)
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

			bStartDownload = m_pCtp->GetDownload().Init(1/*iFileType*/, CAutoUpdate::MyUpdateCallback, (LPARAM)m_pCtp, DL_KILLINPROGRESS);
		}

		if (bStartDownload)
			m_pCtp->GetDownload().AddFile(strFilePath, strDestPath, (int)(LPVOID)pCmd);
	}

	if (bStartDownload)
		m_pCtp->GetDownload().Start(true/*bGoOnline*/, g_szAppName + " Update");
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
	CCtp* pCtp = (CCtp*)lParam;
	DOWNLOAD* pDownload = (DOWNLOAD*)pDownloadVoid;
	if (pDownload)
	{ // A file has successfully been downloaded
		DOWNLOAD_COMMAND_STRUCT* pCmd = (DOWNLOAD_COMMAND_STRUCT*)pDownload->iData;
		if (pCmd->dwFlags & UPDATE_EXECUTE)
			::ShellExecute(NULL, "open", pDownload->strDstPath, NULL, NULL, SW_SHOWNORMAL);

		if (pCmd->dwFlags & UPDATE_RESOURCES)
		{
			pCtp->GetExtensions().Reload();
			CMessageBox::Message(String(IDS_VERSIONUPDATED_PROMPT));
		}
		
		if (pCmd->dwFlags & UPDATE_REGISTER)
		{ //j Someday, add some code to handle this case
		}
		
		if (pCmd->dwFlags & UPDATE_SAVEREGISTRYVERSION)
			pCtp->GetAutoUpdate().SaveRegistryVersion(pDownload->strDstPath, pCmd->strVersion);

		return true;
	}
		
	// The final callback
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CAutoUpdate::GetDestLocation(CString strFilePath, CString& strDestPath)
{
	if (!m_pCtp)
		return;

	CString strFolder = m_pCtp->GetExtensions().GetDllLocation();
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
