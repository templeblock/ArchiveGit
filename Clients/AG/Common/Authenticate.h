#pragma once

#include <atltime.h>

#define MAXPASSWORDSIZE 32 

#define SOURCE_UNKNOWN			0
#define SOURCE_TOOLBAR			700
#define SOURCE_TRAYICON			700
#define SOURCE_PRODUCTSELECT	701
#define SOURCE_PREVIEW			701
#define SOURCE_BROWSEWINDOW		700
//JM205 - Upgrade Unlock

static const DWORD USER_STATUS_AFU				= 0x0000;
static const DWORD USER_STATUS_PAY				= 0xACCE55;
static const DWORD USER_STATUS_BMA				= 0x0002;
static const DWORD USER_STATUS_AG				= 0x0004;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Authenticate Status values. These are used in conjunction with result
// status passed in OnAuthenticate(). The HIWORD of the result status is reserved 
// for failure statuses. The LOWORD is reserved for additional information.
/////////////////////////////////////////////////////////////////////////////////////////
// Authenticate possible LOWORD values.
static const DWORD AUTH_USAGE_SEND_ON			= 0x0001; // Usage send,		bit 1: Send Off=0, Send On=1;
static const DWORD AUTH_USAGE_TRACKING_ON		= 0x0002; // Usage tracking,	bit 2: Tracking Off=0, Tracking On=1;


// Authenticate possible result status values (HIWORD Result Status)
static const DWORD AUTH_FAILURE_NONE			= 0x0000;
static const DWORD AUTH_FAILURE_GENERAL			= 0x1000;
static const DWORD AUTH_FAILURE_PASSWORD		= 0x1001;
static const DWORD AUTH_FAILURE_USERID			= 0x1002;
static const DWORD AUTH_FAILURE_HOST			= 0x1004;
static const DWORD AUTH_FAILURE_USERSTATUS   	= 0x1008;
static const DWORD AUTH_FAILURE_USAGEURL		= 0x1010;
static const DWORD AUTH_FAILURE_MEMNUM			= 0x1020;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const DWORD MODE_REGPROMPT				= 0x0000;
static const DWORD MODE_FIRSTRUN				= 0x0001;
static const DWORD MODE_SILENT					= 0x0002;
static const DWORD MODE_SIGNIN					= 0x0003;
static const DWORD MODE_UPGRADE					= 0x0004;

static const DWORD LOGGING_OFF					= 0x0000;
static const DWORD LOGGING_WINDOW				= 0x0001;
static const DWORD LOGGING_FILE					= 0x0002;

static const DWORD LOGGING_LEVEL_ERROR			= 0x0000;
static const DWORD LOGGING_LEVEL_WARNING		= 0x0001;
static const DWORD LOGGING_LEVEL_INFO			= 0x0002;

static const DWORD  CLIENT_TYPE_EXPRESS		= 0x0001; // bit 0 set
static const DWORD  CLIENT_TYPE_OUTLOOK		= 0x0002; // bit 1 set
static const DWORD  CLIENT_TYPE_IE			= 0x0004; // bit 2 set
static const DWORD  CLIENT_TYPE_AOL			= 0x0008; // bit 3 set

static const DWORD  CLIENT_TYPE_ALL			= 0x000F; // bits 0,1,2,3 set
static const DWORD  CLIENT_TYPE_UPDATE		= 0x0008; // bit 4 set

static const DWORD PROMO_NONE				= 0x0000;
static const DWORD PROMO_FOOTER				= 0x0001;

static LPCTSTR TAG_MODE		= _T("intMode"); // 1=First Run; 2= Silent mode
static LPCTSTR TAG_USERID	= _T("strEmail");
static LPCTSTR TAG_VERSION	= _T("strVerNbr");
static LPCTSTR TAG_CUSTNUM	= _T("cn");
static LPCTSTR TAG_DEBUG	= _T("blnDebug");
static LPCTSTR TAG_MEMTYPE	= _T("memtype");	// 0=free; 1=pay
static LPCTSTR TAG_SOURCE	= _T("source");		// bma=BlueMountain.com; bma742=CM Download.com; bma743=CM Google


static LPCTSTR DEFAULT_HOME			= _T("/mail/index.pd");
static LPCTSTR DEFAULT_BROWSE		= _T("/mail/plugin/browse.pd");
static LPCTSTR DEFAULT_USAGE		= _T("/mail/plugin/usage.pd");
static LPCTSTR DEFAULT_UNINSTALL	= _T("/mail/plugin/uninstall.pd");
static LPCTSTR DEFAULT_HELP			= _T("/customer/cm.pd");
static LPCTSTR DEFAULT_FEEDBACK		= _T("/customer/emailus.pd");
static LPCTSTR AUTHENTICATE_PAGE	= _T("/mail/plugin/auth.pd");
static LPCTSTR DEFAULT_UPGRADE		= _T("/members/mjoin.pd");
static LPCTSTR DEFAULT_MYACCOUNT	= _T("https://www.bluemountain.com/members/myaccount.pd");
static LPCTSTR DEFAULT_MYACCOUNT_DEV	= _T("https://dev.bluemountain.com/members/myaccount.pd");
static LPCTSTR DEFAULT_MYACCOUNT_STAGE	= _T("https://stage.bluemountain.com/members/myaccount.pd");
static LPCTSTR DEFAULT_MYACCOUNT_WORK	= _T("https://work.bluemountain.com/members/myaccount.pd");

#ifdef _DEBUG
	static LPCTSTR DEFAULT_HOST = _T("http://www.bluemountain.com"); 
	static LPCWSTR DEFAULT_HOSTW  = L"http://www.bluemountain.com";
#else
	static LPCTSTR DEFAULT_HOST = _T("http://www.bluemountain.com");
	static LPCWSTR DEFAULT_HOSTW  = L"http://www.bluemountain.com";
#endif _DEBUG

class CAuthenticate
{
public:

	CAuthenticate();
	~CAuthenticate();

	static BOOL ProcessRegistration(DWORD dwMode);
	bool IsRegistered(DWORD& dwFailure);
	bool IsExpired();
	bool IsUserPaidMember();
	bool IsUsageSendRequestOn();
	bool IsUsageTrackingRequestOn();
	bool IsPromoFooterOn();
	bool IsOutlookEnabled();
	bool IsExpressEnabled();
	bool IsOutlookDefaultStaEnabled();
	bool IsExpressDefaultStaEnabled();
	bool IsIEDefaultStaEnabled();
	bool IsAOLDefaultStaEnabled();
	bool IsIEEnabled();
	bool IsAOLEnabled();
	bool IsSmartCacheEnabled();
	bool IsTopMost();
	bool IsAOLClientPresent();
	bool IsClientRegistered(DWORD dwClientType);
	bool ReadUserID(CString& strUserID);
	bool WriteUserID(LPCTSTR strUserID);
	bool ReadMemberStatus(DWORD& dwMemberStatus);
	bool WriteMemberStatus(DWORD dwMemberStatus);
	bool InitializeCrypto();
	bool ReadPassword(CString& strPassword);
	bool WritePassword(const CString& strPassword);
	bool ReadHost(CString& strHost);
	bool WriteHost(LPCTSTR strHost);
	bool ReadUpgradePage(CString& strUpgradeUrl, int iSource, int iPath=0, int iProdnum=0);
	bool WriteUpgradePage(LPCTSTR strUpgradeUrl);
	bool ReadUsagePage(CString& strUsageUrl);
	bool WriteUsagePage(LPCTSTR strUsageUrl);
	bool ReadBrowsePage(CString& strBrowse);
	bool WriteBrowsePage(LPCTSTR strBrowse);
	bool ReadHelpPage(CString& strHelp);
	bool WriteHelpPage(LPCTSTR strHelp);
	bool ReadTimeStamp(CTime& tTime);
	bool WriteTimeStamp();
	bool ReadVersion(CString& strVersion);
	bool WriteVersion(LPCTSTR strVersion);
	bool ReadLineListDate(CString& szLineListDate);
	bool WriteLineListDate(LPCTSTR szLineListDate);
	bool ReadLineListUpdate(CString& szLineListUpdate);
	bool WriteLineListUpdate(LPCTSTR szLineListUpdate);
	bool ReadMemberNum(DWORD& strMemNum);
	bool WriteMemberNum(DWORD strMemNum);
	bool ReadAuthStatus(DWORD& dwAuthStatus) ;
	bool WriteAuthStatus(DWORD dwAuthStatus);
	bool ReadShowIcon(DWORD& dwShow);
	bool WriteShowIcon(DWORD dwShow);
	bool WriteDWORDStatus(LPCTSTR lpszRegVal, DWORD dwStatus);
	bool ReadDWORDStatus(LPCTSTR lpszRegVal, DWORD& dwStatus);
	bool ReadMyAccountPage(CString& strBrowse);
	bool ReadLoggingOption(DWORD& dwLogging);
	bool ReadLoggingLevelOption(DWORD& dwLevel);
	bool ReadServerLoggingOption(DWORD& dwLogging);
	bool GetMemberStatus(CString& szMemStat);
	bool GetMemberStatusQueryString(CString& szQueryPair);
	bool GetPaginationQueryString(CString& szQueryPair);
	bool ReadInstallPath(CString& strPath);
	bool ReadSource(CString& szId);
	LPCTSTR GetBrowsePage();
	LPCTSTR GetUsagePage();
	LPCTSTR GetHelpPage();
	LPCTSTR GetBaseUrl();
	DWORD GetUserStatus();

private:
	CString m_strURL;
	CString m_szUserId;
	CString m_szUsageUrl;
	CString m_szHost;
	CString m_szBrowse;
	DWORD m_dwMemNum;
	DWORD m_dwUserStatus;
	DWORD m_dwErrorStatus;
};
