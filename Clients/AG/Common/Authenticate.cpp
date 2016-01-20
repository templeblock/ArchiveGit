#include "stdafx.h"
#include "Authenticate.h"
#include "RegKeys.h"
#include "Utility.h"
#include "HelperFunctions.h"
#include <atlcrypt.h>

//#import "progid:JMSrvr.JMBrkr"
//using namespace JMSrvrLib;

static const LONG HoursToExpire = 24;		// Authentication expires in 24 hour intervals
static TCHAR strPasswordKey[] = _T("Mz6@a0i*");
static DWORD dwLengthPasswordKey = sizeof(TCHAR) * strlen(strPasswordKey);

/////////////////////////////////////////////////////////////////////////////
CAuthenticate::CAuthenticate()
{
	InitializeCrypto();
	m_dwUserStatus = 0;
}

/////////////////////////////////////////////////////////////////////////////
CAuthenticate::~CAuthenticate()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CAuthenticate::ProcessRegistration(DWORD dwMode)
{
	CFuncLog log(g_pLog, "CAuthenticate::ProcessRegistration()");

	VARIANT_BOOL bSuccess = VARIANT_FALSE;
	CComPtr<IJMBrkr> spBrkr;
	HRESULT hr = spBrkr.CoCreateInstance(CComBSTR("JMSrvr.JMBrkr"), NULL, CLSCTX_LOCAL_SERVER);
	if (FAILED(hr) || spBrkr == NULL)
	{
		log.LogString(LOG_ERROR, log.FormatString(_T("CAuthenticate::ProcessRegistration()::CoCreateInstance(), hr = [%X]"), hr));
		return FALSE;
	}

	hr = spBrkr->Register(dwMode, &bSuccess);
	if (FAILED(hr))
	{
		log.LogString(LOG_ERROR, log.FormatString(_T("CAuthenticate::ProcessRegistration()::Register, hr = [%X]"), hr));
		return FALSE;
	}

	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsRegistered(DWORD& dwFailure)
{
	dwFailure = AUTH_FAILURE_NONE;
	bool bSuccess = true;

	if (!ReadUserID(m_szUserId) || m_szUserId.IsEmpty())
	{
		dwFailure |= AUTH_FAILURE_USERID;	
		bSuccess = false;
	}

	if (!ReadMemberNum(m_dwMemNum) || m_dwMemNum==0 )
	{
		dwFailure |= AUTH_FAILURE_MEMNUM;	
		bSuccess = false;
	}
	if (!ReadHost(m_szHost) || m_szHost.IsEmpty())
	{
		dwFailure |= AUTH_FAILURE_HOST;	
		bSuccess = false;
	}
	
	if (!ReadMemberStatus(m_dwUserStatus) || (m_dwUserStatus != USER_STATUS_AFU && m_dwUserStatus != USER_STATUS_PAY))
	{
		dwFailure |= AUTH_FAILURE_USERSTATUS;
		bSuccess = false;
	}

	if (!ReadUsagePage(m_szUsageUrl) || m_szUsageUrl.IsEmpty())
	{
		dwFailure |= AUTH_FAILURE_USAGEURL;	
		bSuccess = false;
	}


	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsExpired()
{
	CTime tTimeStamp;
	if (!ReadTimeStamp(tTimeStamp))
		return true;

	CTime tCurrentTime = CTime::GetCurrentTime();
	CTimeSpan tElapsedTime  = tCurrentTime -  tTimeStamp;
	LONGLONG llTotalHours = tElapsedTime.GetTotalHours();
	if (llTotalHours >= HoursToExpire)
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadUserID(CString& strUserID) 
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	char strUserIDBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_USERID, strUserIDBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	strUserID = strUserIDBuffer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteUserID(LPCTSTR strUserID)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(REGVAL_USERID, strUserID) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadMemberStatus(DWORD& dwMemberStatus) 
{
	dwMemberStatus = USER_STATUS_AFU;

	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwMemberStatusBuffer = 0;
	if (regkey.QueryDWORDValue(REGVAL_MEMBERSTATUS,dwMemberStatusBuffer) != ERROR_SUCCESS)
		return false;

	dwMemberStatus = dwMemberStatusBuffer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteMemberStatus(DWORD dwMemberStatus)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (dwMemberStatus > USER_STATUS_AFU)
		dwMemberStatus = USER_STATUS_PAY;

	if (regkey.SetDWORDValue(REGVAL_MEMBERSTATUS, dwMemberStatus) != ERROR_SUCCESS)
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadAuthStatus(DWORD& dwAuthStatus) 
{
	dwAuthStatus = 0;

	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwBuffer = 0;
	if (regkey.QueryDWORDValue(REGVAL_AUTH_STATUS, dwBuffer) != ERROR_SUCCESS)
		return false;

	dwAuthStatus = dwBuffer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteAuthStatus(DWORD dwAuthStatus)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetDWORDValue(REGVAL_AUTH_STATUS, dwAuthStatus) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadMemberNum(DWORD& dwMemberNum) 
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwBuffer = 0;
	if (regkey.QueryDWORDValue(REGVAL_MEMBERNUM,dwBuffer) != ERROR_SUCCESS)
		return false;

	dwMemberNum = dwBuffer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteMemberNum(DWORD dwMemberNum)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetDWORDValue(REGVAL_MEMBERNUM, dwMemberNum) != ERROR_SUCCESS)
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadLoggingOption(DWORD& dwLogging) 
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	dwLogging = 0;
	if (regkey.QueryDWORDValue(REGVAL_LOGGING, dwLogging) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadServerLoggingOption(DWORD& dwLogging) 
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	dwLogging = 0;
	if (regkey.QueryDWORDValue(REGVAL_LOGGING_SERVER, dwLogging) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadLoggingLevelOption(DWORD& dwLevel) 
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	dwLevel = 0;
	if (regkey.QueryDWORDValue(REGVAL_LOGGING_LEVEL, dwLevel) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadHost(CString& strHost) 
{
	strHost = DEFAULT_HOST;

	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	char strBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_HOST, strBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	CString szTemp(strBuffer);
	if (szTemp.IsEmpty())
		return false;

	strHost = strBuffer;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteHost(LPCTSTR strHost)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(REGVAL_HOST, strHost) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadUpgradePage(CString& strUpgradeUrl, int iSource, int iPath, int iProdnum) 
{
	strUpgradeUrl = DEFAULT_UPGRADE;

	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	char strBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_UPGRADEURL, strBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	CString szTemp(strBuffer);
	if (szTemp.IsEmpty())
		return false;

	strUpgradeUrl = strBuffer;

	// Modify Upgrade URL based on iSource
	// "cmplugin" = Content based Ad Tag
	// "cmmisc"	  = Non-Content based Ad Tag
	if (iSource != SOURCE_PRODUCTSELECT)
		Replace(strUpgradeUrl, "cmplugin", "cmmisc"); 


	// Determine if we need to add asset path number.
	if (iPath)
	{
		strUpgradeUrl += _T("&");
		strUpgradeUrl += String("path=%d", iPath);
	}

	// Determine if we need to add product number.
	if (iProdnum)
	{
		strUpgradeUrl += _T("&");
		strUpgradeUrl += String("prodnum=%d", iProdnum);
	}
	
	// Determine if we need to add source code.
	if (iSource)
	{
		strUpgradeUrl +=  _T("&");
		strUpgradeUrl += String("source=bma%d", iSource);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteUpgradePage(LPCTSTR strUpgradeUrl)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(REGVAL_UPGRADEURL, strUpgradeUrl) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadUsagePage(CString& strUsageUrl) 
{
	strUsageUrl = DEFAULT_USAGE;
	
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	char strBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_USAGEURL, strBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	CString szTemp(strBuffer);
	if (szTemp.IsEmpty())
		return false;

	strUsageUrl = strBuffer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteUsagePage(LPCTSTR strUsageUrl)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(REGVAL_USAGEURL, strUsageUrl) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadBrowsePage(CString& strBrowse) 
{
	strBrowse = DEFAULT_BROWSE;

	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	char strBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_BROWSEPAGE, strBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	CString szTemp(strBuffer);
	if (szTemp.IsEmpty())
		return false;

	strBrowse = strBuffer;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteBrowsePage(LPCTSTR strBrowse)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(REGVAL_BROWSEPAGE, strBrowse) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadHelpPage(CString& strHelp) 
{
	strHelp = DEFAULT_HELP;

	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	char strBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_HELPPAGE, strBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	CString szTemp(strBuffer);
	if (szTemp.IsEmpty())
		return false;

	strHelp = strBuffer;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteHelpPage(LPCTSTR strHelp)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(REGVAL_HELPPAGE, strHelp) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadMyAccountPage(CString& strMyAccount) 
{
	CString szHost;
	ReadHost(szHost);
	szHost.MakeLower();

	if (szHost.Find(_T("//dev")) >= 0)
		strMyAccount = DEFAULT_MYACCOUNT_DEV;
	else if (szHost.Find(_T("//work")) >= 0)
		strMyAccount = DEFAULT_MYACCOUNT_WORK;
	else if (szHost.Find(_T("//stage")) >= 0)
		strMyAccount = DEFAULT_MYACCOUNT_STAGE;
	else
		strMyAccount = DEFAULT_MYACCOUNT;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadVersion(CString& strVersion) 
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	char strVersionBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_VERSION, strVersionBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	strVersion = strVersionBuffer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteVersion(LPCTSTR strVersion)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(REGVAL_VERSION, strVersion) != ERROR_SUCCESS)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadLineListDate(CString& szLineListDate) 
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_LINELIST_DATE, szBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	szLineListDate = szBuffer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteLineListDate(LPCTSTR szLineListDate)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(REGVAL_LINELIST_DATE, szLineListDate) != ERROR_SUCCESS)
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadLineListUpdate(CString& szLineListUpdate) 
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_LINELIST_UPDATE, szBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	szLineListUpdate = szBuffer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteLineListUpdate(LPCTSTR szLineListUpdate)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(REGVAL_LINELIST_UPDATE, szLineListUpdate) != ERROR_SUCCESS)
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadTimeStamp(CTime& tTime) 
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	ULONGLONG TimeStamp = 0;
	if (regkey.QueryQWORDValue(REGVAL_TIMESTAMP, TimeStamp) != ERROR_SUCCESS)
		return false;

	CTime t(TimeStamp);

	tTime = t;
	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadSource(CString& szId) 
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_SOURCE, szBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	szId = szBuffer;
	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteTimeStamp()
{
	// Get current time and store in registry.
	CTime tTime = CTime::GetCurrentTime();
	ULONGLONG  ullTimeStamp = tTime.GetTime();
		

	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetQWORDValue(REGVAL_TIMESTAMP, ullTimeStamp) != ERROR_SUCCESS)
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadDWORDStatus(LPCTSTR lpszRegVal, DWORD& dwStatus) 
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwBuffer = 0;
	if (regkey.QueryDWORDValue(lpszRegVal, dwBuffer) != ERROR_SUCCESS)
		return false;

	dwStatus = dwBuffer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteDWORDStatus(LPCTSTR lpszRegVal, DWORD dwStatus)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetDWORDValue(lpszRegVal, dwStatus) != ERROR_SUCCESS)
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadShowIcon(DWORD& dwShow) 
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwBuffer = 0;
	if (regkey.QueryDWORDValue(REGVAL_SHOWICON,dwBuffer) != ERROR_SUCCESS)
		return false;

	dwShow = dwBuffer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WriteShowIcon(DWORD dwShow)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetDWORDValue(REGVAL_SHOWICON, dwShow) != ERROR_SUCCESS)
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::InitializeCrypto()
{
	// Ensure that the default cryptographic client is set up
	// Attempt to acquire a handle to the default key container
	CCryptProv hProv;
	if (FAILED(hProv.Initialize(PROV_RSA_FULL, NULL, MS_DEF_PROV, 0)))
	{
		// Some sort of error occured, create default key container
		if (FAILED(hProv.Initialize(PROV_RSA_FULL, NULL, MS_DEF_PROV, CRYPT_NEWKEYSET)))
			return false; // Error creating key container!
	}

	// Attempt to get handle to signature key
	CCryptUserSigKey hSigKey;
	if (FAILED(hSigKey.Initialize(hProv)))
	{
		if (GetLastError() != NTE_NO_KEY)
			return false;

		// Create signature key pair
		if (FAILED(hSigKey.Create(hProv)))
			return false;
	}

	// Attempt to get handle to exchange key
	CCryptUserExKey hExKey;
	if (FAILED(hExKey.Initialize(hProv)))
	{
		if (GetLastError() != NTE_NO_KEY)
			return false;

		// Create key exchange key pair
		if (FAILED(hExKey.Create(hProv)))
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::WritePassword(const CString& strPassword)
{
	// Get handle to user default provider
	CCryptProv hProv;
	if (FAILED(hProv.Initialize(PROV_RSA_FULL, NULL, NULL, 0)))
		return false;

	// Create hash object
	CCryptKeyedHash hHash;
	CCryptKey hNewKey;
	if (FAILED(hHash.Initialize(hProv, CALG_MD5, hNewKey, 0)))
		return false;

	// Hash password key string
	if (FAILED(hHash.AddData((BYTE*)strPasswordKey, dwLengthPasswordKey, 0)))
		return false;

	// Create block cipher session key based on hash of the password key
	CCryptDerivedKey hKey;
	if (FAILED(hKey.Initialize(hProv, hHash, CALG_RC4, CRYPT_EXPORTABLE)))
		return false;

	// Allocate a buffer
	DWORD dwLength = sizeof(TCHAR) * strPassword.GetLength();
	BYTE* pbBuffer = (BYTE*)malloc(dwLength);
	if (!pbBuffer)
		return false;
	
	memcpy(pbBuffer, strPassword, dwLength);

	// Encrypt the password
	bool bResult = false;
	CCryptHash hNewHash;
	if (SUCCEEDED(hKey.Encrypt(true, pbBuffer, &dwLength, dwLength, hNewHash)))
	{
		// Write data to registry
		CRegKey regkey;
		if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
		{
			if (regkey.SetBinaryValue(REGVAL_PASSWORD, pbBuffer, dwLength) == ERROR_SUCCESS)
				bResult = true;
		}
	}
	
	free(pbBuffer);
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadPassword(CString& strPassword) 
{
	// Get handle to user default provider
	CCryptProv hProv;
	if (FAILED(hProv.Initialize(PROV_RSA_FULL, NULL, NULL, 0)))
		return false;

	// Create hash object
	CCryptKeyedHash hHash;
	CCryptKey hNewKey;
	if (FAILED(hHash.Initialize(hProv, CALG_MD5, hNewKey, 0)))
		return false;

	// Hash password key string
	if (FAILED(hHash.AddData((BYTE*)strPasswordKey, dwLengthPasswordKey, 0)))
		return false;

	// Create block cipher session key based on hash of the password key
	CCryptDerivedKey hKey;
	if (FAILED(hKey.Initialize(hProv, hHash, CALG_RC4, CRYPT_EXPORTABLE)))
		return false;

	// The password will be less than MAXPASSWORDSIZE characters
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	char strPasswordBuffer[MAX_PATH];
	DWORD dwLength = MAXPASSWORDSIZE * sizeof(TCHAR);
	if (regkey.QueryBinaryValue(REGVAL_PASSWORD, strPasswordBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	// Decrypt the password
	CCryptHash hNewHash;
	if (FAILED(hKey.Decrypt(true, (BYTE*)strPasswordBuffer, &dwLength, hNewHash)))
		return false;

	if (dwLength >= 0 && dwLength >= MAXPASSWORDSIZE)
		return false;

	strPasswordBuffer[dwLength] = '\0';
	strPassword = strPasswordBuffer;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
DWORD CAuthenticate::GetUserStatus()
{
	DWORD dwStatus=0;
	ReadMemberStatus(dwStatus);
	return dwStatus;
}

/////////////////////////////////////////////////////////////////////////////
LPCTSTR CAuthenticate::GetBaseUrl()
{
	ReadHost(m_szHost);
	return m_szHost;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::GetMemberStatus(CString& szMemStat)
{
	szMemStat = _T("0");
	DWORD dwMemType=0;
	if (!ReadMemberStatus(dwMemType))
		return false;
	
	if (dwMemType == USER_STATUS_PAY)
		szMemStat = _T("1");

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::GetMemberStatusQueryString(CString& szQueryPair)
{
	bool bSuccess;
	CString szVal;
	bSuccess = GetMemberStatus(szVal);
	szQueryPair = TAG_MEMTYPE;
	szQueryPair += "=";
	szQueryPair += szVal;

	return  bSuccess; 
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::GetPaginationQueryString(CString& szQueryPair)
{
	szQueryPair = "adisplay=1";
	return  true; 
}

/////////////////////////////////////////////////////////////////////////////
LPCTSTR CAuthenticate::GetBrowsePage()
{
	ReadBrowsePage(m_szBrowse);
	return m_szBrowse;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsUserPaidMember()
{
	DWORD dwUserStatus = GetUserStatus();
	if (dwUserStatus & USER_STATUS_PAY)
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsUsageSendRequestOn()
{
	DWORD dwAuthStatus=0;
	if (!ReadAuthStatus(dwAuthStatus))
		return true; // by default usage send capability will be on.

	if (dwAuthStatus & AUTH_USAGE_SEND_ON)
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsUsageTrackingRequestOn()
{
	DWORD dwAuthStatus=0;
	if (!ReadAuthStatus(dwAuthStatus))
		return true; // by default usage tracking capability will be on.

	if (dwAuthStatus & AUTH_USAGE_TRACKING_ON)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsPromoFooterOn()
{
	DWORD dwPromoStatus=0;
	if (!ReadDWORDStatus(REGVAL_PROMOSTATUS, dwPromoStatus))
		return true; // by default promo footer will be on.

	if (dwPromoStatus & PROMO_FOOTER)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsOutlookEnabled()
{
	DWORD dwStatus=0;
	if (!ReadDWORDStatus(REGVAL_CLIENTSTATUS, dwStatus))
		return true; // by default Outlook will be enabled.

	if (dwStatus & CLIENT_TYPE_OUTLOOK)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsExpressEnabled()
{
	DWORD dwStatus=0;
	if(!ReadDWORDStatus(REGVAL_CLIENTSTATUS, dwStatus))
		return true; // by default Outlook Express will be enabled.

	if (dwStatus & CLIENT_TYPE_EXPRESS)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsIEEnabled()
{
	DWORD dwStatus=0;
	if (!ReadDWORDStatus(REGVAL_CLIENTSTATUS, dwStatus))
		return true; // by default Outlook Express will be enabled.

	if (dwStatus & CLIENT_TYPE_IE)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsAOLEnabled()
{
	return false;// Temp, until aol supported - JHC
	DWORD dwStatus=0;
	if (!ReadDWORDStatus(REGVAL_CLIENTSTATUS, dwStatus))
		return true; // by default Outlook Express will be enabled.

	if (dwStatus & CLIENT_TYPE_AOL)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsSmartCacheEnabled()
{
	DWORD dwStatus=1;
	if (!ReadDWORDStatus(REGVAL_SMARTCACHE, dwStatus))
	{
		WriteDWORDStatus(REGVAL_SMARTCACHE, TRUE);
		return true;
	}

	if (dwStatus > 0)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsTopMost()
{
	DWORD dwStatus=0;
	if (!ReadDWORDStatus(REGVAL_TOPMOST, dwStatus))
		return false; // by default not top most

	if (dwStatus > 0)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsOutlookDefaultStaEnabled()
{
	DWORD dwStatus=0;
	if (!ReadDWORDStatus(REGVAL_DEFAULTSTA, dwStatus))
		return true; // by default Outlook stattionery will be enabled.

	if (dwStatus & CLIENT_TYPE_OUTLOOK)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsExpressDefaultStaEnabled()
{
	DWORD dwStatus=0;
	if (!ReadDWORDStatus(REGVAL_DEFAULTSTA, dwStatus))
		return true; // by default Express stationery will be enabled.

	if (dwStatus & CLIENT_TYPE_EXPRESS)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsIEDefaultStaEnabled()
{
	DWORD dwStatus=0;
	if (!ReadDWORDStatus(REGVAL_DEFAULTSTA, dwStatus))
		return true; // by default Express stationery will be enabled.

	if (dwStatus & CLIENT_TYPE_IE)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsAOLDefaultStaEnabled()
{
	return false; // temp, until AOL supported - JHC
	DWORD dwStatus=0;
	if (!ReadDWORDStatus(REGVAL_DEFAULTSTA, dwStatus))
		return true; // by default AOL stationery will be enabled.

	if (dwStatus & CLIENT_TYPE_AOL)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsClientRegistered(DWORD dwClientType)
{
	CString szClsidKey;

	if (dwClientType == CLIENT_TYPE_EXPRESS)
		szClsidKey = REGKEY_CLSID_OE;
	else if (dwClientType == CLIENT_TYPE_OUTLOOK)
		szClsidKey = REGKEY_CLSID_OL;
	else if (dwClientType == CLIENT_TYPE_IE)
		szClsidKey = REGKEY_CLSID_IE;
	else if (dwClientType == CLIENT_TYPE_AOL)
		szClsidKey = REGKEY_CLSID_AOL;
	else
		return false;

	CRegKey regkey;
	if (regkey.Open(HKEY_CLASSES_ROOT, szClsidKey) != ERROR_SUCCESS)
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::IsAOLClientPresent()
{
	DWORD dwStatus=0;
	if (!ReadDWORDStatus(REGVAL_AOL_STATUS, dwStatus))
		return false; 

	if (dwStatus & CLIENT_TYPE_AOL)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CAuthenticate::ReadInstallPath(CString& strPath) 
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	char strBuffer[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_INSTALLPATH, strBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	strPath = strBuffer;
	return true;
}
