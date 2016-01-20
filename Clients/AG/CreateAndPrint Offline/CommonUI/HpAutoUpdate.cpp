#include "stdafx.h"
#include "Ctp.h"
#include "HpAutoUpdate.h"
#include "RegKeys.h"
#include "Internet.h"
#include "VersionInfo.h"

#define REGKEY_HPAPP				"Software\\American Greetings\\CpApp"

/////////////////////////////////////////////////////////////////////////////
CHpAutoUpdate::CHpAutoUpdate(CCtp* pCtp): CAutoUpdate(pCtp)
{
	m_strAppPath = "C:\\Program Files\\Hp Design Studio\\";
	m_strAppName = "HpApp.exe";
	m_strKeyName = "Version_HpApp.exe";
}

/////////////////////////////////////////////////////////////////////////////
bool CHpAutoUpdate::Start(bool bSilent, CString& strManifestURL)
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
#ifdef _DEBUG
	CString szManifestURL = "http://localhost/dataupload/hpmanifest.xml";
#else
	CString strImgHost = m_pCtp->GetContextImgHost();
	CString szManifestURL = strImgHost + String(IDS_HPMANIFEST_URL);
#endif
	int index = max(szManifestURL.ReverseFind('/'), szManifestURL.ReverseFind('\\'));
	m_strManifestPath = szManifestURL.Left(index + 1);

	m_strManifestFile = RandomFileName();

	if (!m_pCtp->GetDownload().Init(1/*iFileType*/, CAutoUpdate::MyGetManifestCallback, (LPARAM)m_pCtp, DL_KILLINPROGRESS))
		return false;

	m_pCtp->GetDownload().AddFile(szManifestURL, m_strManifestFile, NULL);
	m_pCtp->GetDownload().Start(true/*m_bGoOnline*/);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CHpAutoUpdate::IsSilentUpdateAllowed()
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_HPAPP) != ERROR_SUCCESS)
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
bool CHpAutoUpdate::RegisterSilentUpdate()
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_HPAPP) != ERROR_SUCCESS)
		return false;
	
	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString strDate;
	strDate.Format("%d/%d/%d", Now.GetMonth(), Now.GetDay(), Now.GetYear());
	if (regkey.SetStringValue(REGVAL_AUTOUPDATE, strDate) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CHpAutoUpdate::FileUpToDate(const CString& strFilePath, const CString& strManifestVersion, bool bTestRegistryVersion)
{
	CString szFilePath = m_strAppPath + m_strAppName;
	DWORD dwFileVersion = 0;

	if (!bTestRegistryVersion)
	{
		if (!FileExists(szFilePath))
			return false;

		dwFileVersion = GetFileVersion(szFilePath);
	}
	else
	{
		CRegKey regkey;
		if (regkey.Open(REGKEY_APPROOT, REGKEY_HPAPP) != ERROR_SUCCESS)
			return false;
		
		DWORD dwSize = MAX_PATH;
		TCHAR szBuffer[MAX_PATH];
		if (regkey.QueryStringValue(m_strKeyName, szBuffer, &dwSize) == ERROR_SUCCESS)
		{
			CString strFileVersion = szBuffer;
			dwFileVersion = Version(strFileVersion);
		}
	}

	return (Version(strManifestVersion) <= dwFileVersion);
}

/////////////////////////////////////////////////////////////////////////////
void CHpAutoUpdate::GetDestLocation(CString strFilePath, CString& strDestPath)
{
	if (!m_pCtp)
		return;

	CString strFolder = m_strAppPath;
	int index = max(strFilePath.ReverseFind('/'), strFilePath.ReverseFind('\\'));
	CString strFileName = strFilePath.Mid(index + 1);
	strDestPath = strFolder + strFileName;
}

/////////////////////////////////////////////////////////////////////////////
bool CHpAutoUpdate::SaveRegistryVersion(const CString& strFilePath, const CString& strVersion)
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_HPAPP) != ERROR_SUCCESS)
		return false;

	regkey.SetStringValue(m_strKeyName, strVersion);
	return true;
}
