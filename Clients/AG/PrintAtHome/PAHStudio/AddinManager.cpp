#include "stdafx.h"
#include "AddinManager.h"

LPCTSTR REGVAL_TITLE		= _T("Title");
LPCTSTR REGVAL_PROGID		= _T("Prog ID");
LPCTSTR REGVAL_ICONPATH		= _T("Icon Path");
LPCTSTR REGVAL_TIP			= _T("Tip");
LPCTSTR REGVAL_VERSION		= _T("Version");

//=============================================================================
// Contructor/Destructor
//============================================================================= 
CAddinManager::CAddinManager(HWND hwndClient, HKEY hKeyRoot, LPCTSTR szKeyAddin)
{
	m_hwndClient	= hwndClient;
	m_hKeyRoot		= hKeyRoot;
	m_szKeyAddins	= szKeyAddin;
}

CAddinManager::~CAddinManager(void)
{
}
//=============================================================================
// GetAddins
//============================================================================= 
DWORD CAddinManager::GetAddins(void)
{
	// Open Addins location
	CRegKey regKey;
	if (m_szKeyAddins.IsEmpty() || (regKey.Open(m_hKeyRoot, m_szKeyAddins) != ERROR_SUCCESS))
		return ADDIN_ERROR_OPEN;


	// Create Addin list by iterating thru all registered addins' CLSIDs. 
	TCHAR szCLSID[38];
	DWORD dwSize = 39; // includes null
	DWORD dwIndex = 0;
	LONG lRet;
	ADDIN_INFO_TYPE AddinInfo;
	while ((lRet = regKey.EnumKey(dwIndex, szCLSID, &dwSize)) != ERROR_NO_MORE_ITEMS)
	{
		if (ERROR_SUCCESS != lRet)
			return ADDIN_ERROR_OPEN;

		// CLSID
		CRegKey regSubKey;
		if ((ERROR_SUCCESS != regSubKey.Open(regKey.m_hKey, szCLSID)) || szCLSID[0] != _T('{'))
			return ADDIN_ERROR_OPEN;

		// Info
		if (!ReadAddinInfo(regSubKey, AddinInfo))
			return ADDIN_ERROR_READ;

		// Add addin to list
		m_AddinList[szCLSID] = AddinInfo;

		// Next addin
		dwIndex++;
	}

	return ADDIN_ERROR_NONE;
}
//=============================================================================
// ReadAddinInfo
//============================================================================= 
bool CAddinManager::ReadAddinInfo(CRegKey& regKey, ADDIN_INFO_TYPE& AddinInfo)
{
	DWORD dwLength = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	
	// Title
	if (ERROR_SUCCESS != regKey.QueryStringValue(REGVAL_TITLE, szBuffer, &dwLength))
		return false;
	AddinInfo.szTitle = szBuffer;

	// Prog Id
	if (ERROR_SUCCESS != regKey.QueryStringValue(REGVAL_PROGID, szBuffer, &dwLength))
		return false;
	AddinInfo.szProgId = szBuffer;

	// Icon Path
	if (ERROR_SUCCESS != regKey.QueryStringValue(REGVAL_ICONPATH, szBuffer, &dwLength))
		return false;
	AddinInfo.szIconPath = szBuffer;

	// Tip
	if (ERROR_SUCCESS != regKey.QueryStringValue(REGVAL_TIP, szBuffer, &dwLength))
		return false;
	AddinInfo.szTip = szBuffer;

	// Version
	if (ERROR_SUCCESS != regKey.QueryStringValue(REGVAL_VERSION, szBuffer, &dwLength))
		return false;
	AddinInfo.szVersion = szBuffer;

	return true;
}
//=============================================================================
// RegisterAddin
//============================================================================= 
//
// Example of addin registry entries follows: 
//
//
//	HKEY_CURRENT_USER
//		Software
//			American Greetings
//				PiB Studio
//					Addins
//						{4D5C8C2A-D075-11D0-B416-00C04FB90376}
//							Title		= Image Editor 
//							Prog ID		= ImageLib.Editor
//							Icon Path	= C:\Program Files\PiBStudio\Image Editor\Icon.ico
//							Tip			= Modify Images
//							Version		= 1.2.3
//						{6790C8CA-D135-11D0-B312-00C044070456}
//							Title		= Monogram Wizard 
//							Prog ID		= TextLib.Monogram;6
//							Icon Path	= C:\Program Files\PiBStudio\TextLib\Icon.ico
//							Tip			= Create Monograms
//							Version		= 2.4.6
//
//=============================================================================
DWORD CAddinManager::RegisterAddin(LPCTSTR szAddinCLSID, ADDIN_INFO_TYPE &AddinInfo)
{
	CRegKey regkey;
	CString szCLSID = szAddinCLSID;
	
	// Create CLSID key for new Addin entry
	CString szNewEntry;
	szNewEntry = m_szKeyAddins;
	szNewEntry += _T("\\");
	szNewEntry += szCLSID;
	if (szCLSID.IsEmpty() || (regkey.Create(m_hKeyRoot, szNewEntry) != ERROR_SUCCESS))
		return ADDIN_ERROR_CLSID;

	// Set Title reg value
	if (AddinInfo.szTitle.IsEmpty() || (regkey.SetStringValue(REGVAL_TITLE, AddinInfo.szTitle) != ERROR_SUCCESS))
		return ADDIN_ERROR_TITLE;

	// Set Prog ID reg value 
	if (AddinInfo.szTitle.IsEmpty() || (regkey.SetStringValue(REGVAL_PROGID, AddinInfo.szProgId) != ERROR_SUCCESS))
		return ADDIN_ERROR_PROGID;

	// Set Path reg value
	if (AddinInfo.szTitle.IsEmpty() || (regkey.SetStringValue(REGVAL_ICONPATH, AddinInfo.szIconPath) != ERROR_SUCCESS))
		return ADDIN_ERROR_ICONPATH;

	// Set Tip reg value
	if (AddinInfo.szTip.IsEmpty() || (regkey.SetStringValue(REGVAL_TIP, AddinInfo.szTip) != ERROR_SUCCESS))
		return ADDIN_ERROR_TIP;

	// Set Version reg value
	if (AddinInfo.szVersion.IsEmpty() || (regkey.SetStringValue(REGVAL_VERSION, AddinInfo.szVersion) != ERROR_SUCCESS))
		return ADDIN_ERROR_VERSION;

	return ADDIN_ERROR_NONE;
}
//=============================================================================
// UnregisterAddin
//============================================================================= 
DWORD CAddinManager::UnregisterAddin(LPCTSTR szAddinCLSID)
{
	CRegKey regkey;
	CString szCLSID = szAddinCLSID;
	CString szEntry;
	szEntry = m_szKeyAddins;
	szEntry += _T("\\");
	szEntry += szCLSID;
	if (szCLSID.IsEmpty() || (regkey.Open(m_hKeyRoot, szEntry) != ERROR_SUCCESS))
		return ADDIN_ERROR_CLSID;

	if (regkey.DeleteSubKey(szCLSID) != ERROR_SUCCESS)
		return ADDIN_ERROR_UNREG;

	return ADDIN_ERROR_NONE;
}