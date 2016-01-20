#include "stdafx.h"
#include ".\resource.h"       // main symbols
#include "Utility.h"
#include "SecurityMgr.h"
#include "RegKeys.h"
#include "HelperFunctions.h"
#include "Authenticate.h"

static UINT UWM_HTML_GETOBJECT	= ::RegisterWindowMessage(_T("WM_HTML_GETOBJECT"));

static const DWORD ZONE_INTERNET			= 3;
static const DWORD ZONE_RESTRICTED			= 4;

/////////////////////////////////////////////////////////////////////////////
CSecurityMgr::CSecurityMgr(void)
{
	m_hMSAAInst = ::LoadLibrary( _T("OLEACC.DLL"));
	m_bMustRestartToSetZone = false;
	m_bMustRestartToSetRunActiveX = false;
	m_bMustRestartToSetHtmlSettings = false;
	m_bMustRestartToSetEmailEditor = false;
}

/////////////////////////////////////////////////////////////////////////////
CSecurityMgr::~CSecurityMgr(void)
{
	if (m_hMSAAInst)
		::FreeLibrary(m_hMSAAInst);
}

/////////////////////////////////////////////////////////////////////////////
IInternetSecurityManager* CSecurityMgr::GetInternetSecurityManager(HWND hCompose)
{
	CFuncLog log(g_pLog, "CSecurityMgr::GetInternetSecurityManager()");
	
	// Is this a valid compose window?
	if (!::IsWindow(hCompose))
	{
		log.LogString(LOG_ERROR, " FAILURE: IsWindow(hCompose))");	
		return NULL;
	}

	if (m_hMSAAInst == NULL)
	{
		log.LogString(LOG_ERROR, " FAILURE: m_hMSAAInst = NULL)");
		return NULL;
	}

	// Get Internet Explorer_Server
	HWND hwndIEServer = NULL;
	GetIEServerWindow(hCompose, hwndIEServer);
	if (!::IsWindow(hwndIEServer))
		return NULL;

	// Get LResult from Html Object - needed for call to ObjectFromLresult();
	LRESULT lResult = NULL;
	if (::SendMessageTimeout(hwndIEServer, 
							UWM_HTML_GETOBJECT, 
							0L, 
							0L, 
							SMTO_ABORTIFHUNG, 
							5000, 
							(DWORD*)&lResult) == 0)
		return NULL;


	// Get pointer to ObjectFromLresult() function - used to get html interface.
	LPFNOBJECTFROMLRESULT pfObjectFromLresult = (LPFNOBJECTFROMLRESULT)::GetProcAddress(m_hMSAAInst, _T("ObjectFromLresult") );
	if (!pfObjectFromLresult)
	{
		log.LogString(LOG_ERROR, " FAILURE: pfObjectFromLresult = NULL)");
		return NULL;
	}

	// Get a pointer to the Internet Security Manager interface.
	CComPtr<IInternetSecurityManager> spMgr;
	HRESULT hr = (*pfObjectFromLresult)( lResult, IID_IInternetSecurityManager, 0, (void**)&spMgr );
	if (FAILED(hr) || spMgr == NULL)
	{
		CString szMsg;
		szMsg.Format("FAILED(%X) || spMgr == %X", hr, spMgr); 
		GetError(szMsg);
		log.LogString(LOG_ERROR, szMsg.GetBuffer());
		return NULL;
	}

	return spMgr;
}

#define bMustRestart true //j Set to true if you want to test making on the fly changes - not possible in all cases

/////////////////////////////////////////////////////////////////////////////
bool CSecurityMgr::FixupSecurityZone(bool bSilent, DWORD dwClient)
{
	LPCTSTR lpszClient;
	if (dwClient == CLIENT_TYPE_EXPRESS)
		lpszClient = " Express";
	else if (dwClient == CLIENT_TYPE_OUTLOOK)
		lpszClient = " Outlook";
	else
		lpszClient = " Internet Explorer";

	CString strRestartMessage;
	strRestartMessage.Format("In order to activate the change that enables Flash content, please:\n\no    Close all open mail messages\no    Close %s\no    Restart %s", lpszClient, lpszClient);
	if (m_bMustRestartToSetZone)
	{
		if (!bSilent)
			::MessageBox(NULL, strRestartMessage, g_szAppName, MB_OK);
		return true;
	}

	CRegKey regkey;
	bool bFound = false;
	CString pKeyName;
	LPCTSTR pValueName = NULL;
	if (dwClient == CLIENT_TYPE_OUTLOOK)
	{
		pKeyName = REGKEY_OUTLOOK11_RESTRICTED;
		pValueName = REGVAL_OUTLOOK_RESTRICTED;
		bFound = RegKeyValueExists(pKeyName, pValueName);
		if (!bFound)
		{
			pKeyName = REGKEY_OUTLOOK10_RESTRICTED;
			bFound = RegKeyValueExists(pKeyName, pValueName);
			if (!bFound)
			{
				pKeyName = REGKEY_OUTLOOK09_RESTRICTED;
				bFound = RegKeyValueExists(pKeyName, pValueName);
			}
		}
	}
	else if (dwClient == CLIENT_TYPE_EXPRESS)
	{
		if (regkey.Open(HKEY_CURRENT_USER, REGKEY_IDENTITIES) != ERROR_SUCCESS)
			return false;

		DWORD dwLength = MAX_PATH;
		char szUserID[MAX_PATH];
		if (regkey.QueryStringValue(REGVAL_LAST_USER_ID, szUserID, &dwLength) != ERROR_SUCCESS)
			if (regkey.QueryStringValue(REGVAL_DEFAULT_USER_ID, szUserID, &dwLength) != ERROR_SUCCESS)
				return false;

		DWORD dwMajor = 6;
		GetIEMajorVersion(dwMajor);

		pKeyName = String("%s\\%s\\%s", REGKEY_IDENTITIES, szUserID, REGKEY_OUTLOOKEX_RESTRICTED);
		pValueName = (dwMajor >= 6 ? REGVAL_OUTLOOKEX_RESTRICTED : REGVAL_OUTLOOKEX_RESTRICTED2);
		bFound = RegKeyValueExists(pKeyName, pValueName);
		
		// If reg key value "Email Security Zone" is not found then create 
		// and set value to 4 (restricted) - this forces the user prompt below. 
		// 
		// NOTE: "Email Security Zone" reg value does not exist after Outlook Express 
		// is first installed, this logic handles that scenario.
		if (!bFound)
		{
			if (regkey.Open(HKEY_CURRENT_USER, pKeyName) == ERROR_SUCCESS)
			{
				if (regkey.SetDWORDValue(pValueName, 4) == ERROR_SUCCESS)
					bFound = true;
			}
		}
	}

	if (!bFound)
		return false;

	if (regkey.Open(HKEY_CURRENT_USER, pKeyName) != ERROR_SUCCESS)
		return false;

	DWORD dwZoneValue = 0;
	if (regkey.QueryDWORDValue(pValueName, dwZoneValue) != ERROR_SUCCESS)
		return false;
		
	if (dwZoneValue == ZONE_INTERNET) // The Internet Zone == 3, Restricted Zone == 4
		return false;

	if (!bSilent)
	{
		CString szMsg;
		szMsg.LoadString(IDS_FLASH_PROMPT);
		bool bOK = (::MessageBox(NULL, szMsg, g_szAppName, MB_YESNO) == IDYES);
		if (!bOK)
			return true;
	}

	dwZoneValue = ZONE_INTERNET;
	if (regkey.SetDWORDValue(pValueName, dwZoneValue) != ERROR_SUCCESS)
		return true;
	
	if (bMustRestart)
	{
		m_bMustRestartToSetZone = true;
		if (!bSilent)
			::MessageBox(NULL, strRestartMessage, g_szAppName, MB_OK);
	}
	
	return bMustRestart;
}

/////////////////////////////////////////////////////////////////////////////
bool CSecurityMgr::FixupRunActiveX(bool bSilent, DWORD dwClient)
{
	LPCTSTR lpszClient;
	if (dwClient == CLIENT_TYPE_EXPRESS)
		lpszClient = " Express";
	else if (dwClient == CLIENT_TYPE_OUTLOOK)
		lpszClient = " Outlook";
	else
		lpszClient = " Internet Explorer";

	CString strRestartMessage;
	strRestartMessage.Format("In order to activate the change that enables Flash content, please:\n\no    Close all open mail messages\no    Close %s\no    Restart %s", lpszClient, lpszClient);
	if (m_bMustRestartToSetRunActiveX)
	{
		if (!bSilent)
			::MessageBox(NULL, strRestartMessage, g_szAppName, MB_OK);
		return true;
	}

	CRegKey regkey;
	bool bFound = false;
	CString pKeyName;
	LPCTSTR pValueName = NULL;
	DWORD dwZoneValue = 0;
	if (dwClient == CLIENT_TYPE_OUTLOOK)
	{
		pKeyName = REGKEY_OUTLOOK11_RESTRICTED;
		pValueName = REGVAL_OUTLOOK_RESTRICTED;
		bFound = RegKeyValueExists(pKeyName, pValueName);
		if (!bFound)
		{
			pKeyName = REGKEY_OUTLOOK10_RESTRICTED;
			bFound = RegKeyValueExists(pKeyName, pValueName);
			if (!bFound)
			{
				pKeyName = REGKEY_OUTLOOK09_RESTRICTED;
				bFound = RegKeyValueExists(pKeyName, pValueName);
			}
		}
	}
	else if (dwClient == CLIENT_TYPE_EXPRESS)
	{
		if (regkey.Open(HKEY_CURRENT_USER, REGKEY_IDENTITIES) != ERROR_SUCCESS)
			return false;

		DWORD dwLength = MAX_PATH;
		char szUserID[MAX_PATH];
		if (regkey.QueryStringValue(REGVAL_LAST_USER_ID, szUserID, &dwLength) != ERROR_SUCCESS)
			if (regkey.QueryStringValue(REGVAL_DEFAULT_USER_ID, szUserID, &dwLength) != ERROR_SUCCESS)
				return false;

		DWORD dwMajor = 6;
		GetIEMajorVersion(dwMajor);

		pKeyName = String("%s\\%s\\%s", REGKEY_IDENTITIES, szUserID, REGKEY_OUTLOOKEX_RESTRICTED);
		pValueName = (dwMajor >= 6 ? REGVAL_OUTLOOKEX_RESTRICTED : REGVAL_OUTLOOKEX_RESTRICTED2);
		bFound = RegKeyValueExists(pKeyName, pValueName);
	}
	else // Must be IE, it uses Internet Zone.
	{
		dwZoneValue = ZONE_INTERNET;
		bFound = true;
	}

	if (!bFound)
		return false;

	// For Outlook or Express get the current security zone.
	if (dwClient == CLIENT_TYPE_EXPRESS || dwClient == CLIENT_TYPE_OUTLOOK)
	{
		if (regkey.Open(HKEY_CURRENT_USER, pKeyName) != ERROR_SUCCESS)
	//j	if (regkey.Open(HKEY_CURRENT_USER, REGKEY_IE_ZONEDEFAULTS) != ERROR_SUCCESS)
			return false;

		if (regkey.QueryDWORDValue(pValueName, dwZoneValue) != ERROR_SUCCESS)
	//j	if (regkey.QueryDWORDValue(REGVAL_HTTP, dwZoneValue) != ERROR_SUCCESS)
			return false;
	}


	// Check if Zone's Run ActiveX option is enabled.
	CString strKeyName = String("%s\\%d", REGKEY_IE_ZONES, dwZoneValue);
	if (regkey.Open(HKEY_CURRENT_USER, strKeyName) != ERROR_SUCCESS)
		return false;

	DWORD dwRunActiveXValue = 0;
	if (regkey.QueryDWORDValue(REGVAL_RUNACTIVEX, dwRunActiveXValue) != ERROR_SUCCESS)
		return false;

	if (!dwRunActiveXValue)
		return false;

	if (!bSilent)
	{
		CString szMsg;
		szMsg.LoadString(IDS_ACTIVEX_PROMPT);
		bool bOK = (::MessageBox(NULL, szMsg, g_szAppName, MB_YESNO) == IDYES);
		if (!bOK)
			return true;
	}

	dwRunActiveXValue = 0; // Enable
	if (regkey.SetDWORDValue(REGVAL_RUNACTIVEX, dwRunActiveXValue) != ERROR_SUCCESS)
		return true;
	
	if (bMustRestart)
	{
		m_bMustRestartToSetRunActiveX = true;
		if (!bSilent)
			::MessageBox(NULL, strRestartMessage, g_szAppName, MB_OK);
	}
	
	return bMustRestart;
}

/////////////////////////////////////////////////////////////////////////////
bool CSecurityMgr::FixupHtmlSettings(bool bSilent, DWORD dwClient)
{
	LPCTSTR lpszClient;
	if (dwClient == CLIENT_TYPE_EXPRESS)
		lpszClient = " Express";
	else if (dwClient == CLIENT_TYPE_OUTLOOK)
		lpszClient = " Outlook";
	else
		lpszClient = " Internet Explorer";

	CString strRestartMessage;
	strRestartMessage.Format("In order to activate the change that enables content to be sent, please:\n\no    Close all open mail messages\no    Close %s\no    Restart %s", lpszClient, lpszClient);
	if (m_bMustRestartToSetHtmlSettings)
	{
		if (!bSilent)
			::MessageBox(NULL, strRestartMessage, g_szAppName, MB_OK);
		return true;
	}

	CRegKey regkey;
	bool bFound = false;
	CString pKeyName;
	
	if (dwClient == CLIENT_TYPE_EXPRESS)
	{
		if (regkey.Open(HKEY_CURRENT_USER, REGKEY_IDENTITIES) != ERROR_SUCCESS)
			return false;

		DWORD dwLength = MAX_PATH;
		char szUserID[MAX_PATH];
		if (regkey.QueryStringValue(REGVAL_LAST_USER_ID, szUserID, &dwLength) != ERROR_SUCCESS)
			if (regkey.QueryStringValue(REGVAL_DEFAULT_USER_ID, szUserID, &dwLength) != ERROR_SUCCESS)
				return false;

	
		pKeyName = String("%s\\%s\\%s", REGKEY_IDENTITIES, szUserID, REGKEY_OUTLOOKEX_SENDPICTURES);
		bFound = RegKeyValueExists(pKeyName, REGVAL_OUTLOOKEX_SENDPICTURES);
	}
	
	if (!bFound)
		return false;

	if (regkey.Open(HKEY_CURRENT_USER, pKeyName) != ERROR_SUCCESS)
		return false;

	DWORD dwSendPicsValue = 0;
	if (regkey.QueryDWORDValue(REGVAL_OUTLOOKEX_SENDPICTURES, dwSendPicsValue) != ERROR_SUCCESS)
		return false;
		
	if (dwSendPicsValue == 0) // Enable == 1, Disable == 0
		return false;


	if (!bSilent)
	{
		CString szMsg;
		szMsg.LoadString(IDS_SENDPICTURES_PROMPT);
		bool bOK = (::MessageBox(NULL, szMsg, g_szAppName, MB_YESNO) == IDYES);
		if (!bOK)
			return false;
	}

	// Disable Send Pictures option
	if (regkey.SetDWORDValue(REGVAL_OUTLOOKEX_SENDPICTURES, 0) != ERROR_SUCCESS)
		return false;

	if (bMustRestart)
	{
		m_bMustRestartToSetHtmlSettings = true;
		if (!bSilent)
			::MessageBox(NULL, strRestartMessage, g_szAppName, MB_OK);
	}

	return bMustRestart;

}
/////////////////////////////////////////////////////////////////////////////
bool CSecurityMgr::FixupEmailEditor(bool bSilent, bool bAllowChange)
{
	CFuncLog log(g_pLog, "CSecurityMgr::WordIsEmailEditor()");
	LPCTSTR pRegistrySetMessage = "HTML is now your default email format.\n\nPlease restart Outlook in order to activate this change.";
	if (m_bMustRestartToSetEmailEditor)
	{
		if (!bSilent)
			::MessageBox(NULL, pRegistrySetMessage, g_szAppName, MB_OK);
		return true;
	}

	LPCTSTR pKeyName = REGKEY_OUTLOOK11_USING_WORD;
	LPCTSTR pValueName = REGVAL_OUTLOOK_USING_WORD;
	bool bFound = RegKeyValueExists(pKeyName, pValueName);
	if (!bFound)
	{
		pKeyName = REGKEY_OUTLOOK10_USING_WORD;
		bFound = RegKeyValueExists(pKeyName, pValueName);
		if (!bFound)
		{
			pKeyName = REGKEY_OUTLOOK09_USING_WORD;
			bFound = RegKeyValueExists(pKeyName, pValueName);
		}
	}

	if (!bFound)
		return false;

	CRegKey regkey;
	if (regkey.Open(HKEY_CURRENT_USER, pKeyName) != ERROR_SUCCESS)
		return false;

	DWORD dwValue = 0;
	if (regkey.QueryDWORDValue(pValueName, dwValue) != ERROR_SUCCESS)
		return false;
		
	// dwValue format:
	// 0x00010001 = PlainText & WordMail
	// 0x00020001 = HTML & WordMail
	// 0x00030001 = RTF & WordMail
	// 0x00010000 = PlainText
	// 0x00020000 = HTML
	// 0x00030002 = RTF

	bool bWordMail  = !!(dwValue & 0x00000001);
	bool bRTF       = !!(dwValue & 0x00000002);
	bool bPlainText = !!(dwValue & 0x00010000);
//j	if (!bWordMail && !bRTF && !bPlainText)
	if (!bWordMail && !bRTF) // Allow plain text for now since we can convert it
		return false;

	if (!bAllowChange)
		return true;
		
	// Ask the user if we can make the change from Word to HTML mail
	if (!bSilent)
	{
		CString szMsg;
		szMsg.LoadString(IDS_WORD_PROMPT);
		bool bOK = (::MessageBox(NULL, szMsg, g_szAppName, MB_YESNO) == IDYES);
		if (!bOK)
			return true;
	}

	dwValue &= ~0x00000001;
	dwValue &= ~0x00000002;
	dwValue &= ~0x00010000;
	if (regkey.SetDWORDValue(pValueName, dwValue) != ERROR_SUCCESS)
		return true;
	
	m_bMustRestartToSetEmailEditor = true;

	if (!bSilent)
		::MessageBox(NULL, pRegistrySetMessage, g_szAppName, MB_OK);
	return true;

#ifdef NOTUSED
	Outlook::_InspectorPtr pInspector = GetActiveInspector();
	Outlook::_MailItemPtr spMailItem = GetMailItem(pInspector);
	if (pInspector == NULL || spMailItem == NULL)
		return true;

	Outlook::FormDescriptionPtr pFormDescription = spMailItem->GetFormDescription();
	if (pFormDescription == NULL)
		return true;

	VARIANT_BOOL bUseWordMail = pFormDescription->GetUseWordMail();
	bUseWordMail = false;
	pFormDescription->PutUseWordMail(bUseWordMail);

	IHTMLDocument2Ptr ptr = GetHTMLDocument2(pInspector, true/*bConvert*/);
	Outlook::OlEditorType EditorType = pInspector->GetEditorType();

	VARIANT_BOOL bIsWordMail = pInspector->IsWordMail();
	return !!bIsWordMail;
#endif NOTUSED
}

/////////////////////////////////////////////////////////////////////////////
bool CSecurityMgr::IsUrlActionAllowed(HWND hIEWindow, DWORD dwUrlAction, LPCWSTR pwszUrl)
{
	CFuncLog log(g_pLog, "CSecurityMgr::IsUrlActionAllowed()");
	HRESULT hr;
	CComPtr<IInternetSecurityManager> spSecurityMgr = NULL;

	if (hIEWindow)
	{
		spSecurityMgr = GetInternetSecurityManager(hIEWindow);
		if (spSecurityMgr == NULL)
			log.LogString(LOG_ERROR, "GetInternetSecurityManager() FAILED, spSecurityMgr == NULL");
	}
	
	if (!spSecurityMgr)
	{
		hr = spSecurityMgr.CoCreateInstance(CLSID_InternetSecurityManager);		
		if (FAILED(hr))
		{
			CString szMsg;
			szMsg.Format(_T("CoCreateInstance(CLSID_InternetSecurityManager) failed [%X]"), hr); 
			GetError(szMsg);
			log.LogString(LOG_ERROR, szMsg.GetBuffer());
			return false;
		}
	}

	if (spSecurityMgr == NULL)
		return false;

	DWORD dwPolicy = URLPOLICY_DISALLOW;
	hr = spSecurityMgr->ProcessUrlAction(pwszUrl				/* pwszUrl*/, 
										dwUrlAction				/* dwAction*/, 
										(BYTE*)&dwPolicy		/* pPolicy*/, 
										sizeof(dwPolicy)		/* cbPolicy*/,
										NULL					/*pContext*/ ,
										0 						/*cbContext*/,
										PUAF_NOUI				/*dwFlags*/,
										NULL					/*dwReserved*/);


	
	if (dwPolicy == URLPOLICY_DISALLOW)
	{
		CString szMsg;
		szMsg.Format(_T("ProcessUrlAction() hr = [%X], Policy = [%X]"), hr, dwPolicy); 
		GetError(szMsg);
		log.LogString(LOG_ERROR, szMsg.GetBuffer());
		return false; // Security check failed!
	}

	return (dwPolicy != URLPOLICY_DISALLOW);
}
/////////////////////////////////////////////////////////////////////////////
bool CSecurityMgr::EnableBrowserExtensions(bool bSilent)
{
	CRegKey regkey;

	if (regkey.Open(HKEY_CURRENT_USER, REGKEY_IE_MAIN) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_IE_ENABLE_EXTENSIONS, szBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	CString szValue(szBuffer);
	if (szValue.CompareNoCase(_T("yes")) == 0)
		return true; 


	if (!bSilent)
	{
		CString szMsg;
		szMsg.LoadString(IDS_BROWSEREXT_WARN);
		bool bOK = (::MessageBox(NULL, szMsg, g_szAppName, MB_YESNO) == IDYES);
		if (!bOK)
			return true;
	}

	if (regkey.SetStringValue(REGVAL_IE_ENABLE_EXTENSIONS, _T("yes")) != ERROR_SUCCESS)
		return false;

	return true;
}