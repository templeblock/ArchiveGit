#pragma once
typedef enum _AGDownloadType
{
	DT_Invalid = 0,
	DT_DownloadAndSetBackground,
	DT_DownloadAndEmbedContents,
	DT_EmbedReference,
} AGDownloadType;

struct DownloadParms
{
	AGDownloadType Type;
	CString strURL;
	CString strSrc;
	CString strDst;
	CString strFile;
	CString strName;
	CString strCategory;
	CString strFlashPath;
	CString strExternalLink;
	int iSource;
	int iProductId;
	int iWidth;
	int iHeight;
	int iCategory;
	int iPathPrice;
	int iGroup;
	bool bFreeContent;
	bool bPayingMember;
};
struct VERSIONINFOSTRUCT
{
	int iMajor;
	int iMinor;
	int iBuild;
	LPCTSTR szVer;
};


const int TYPE_REFERAFRIEND	= 0x0000;
const int TYPE_STATIONERY	= 0x0001;
const int TYPE_ECARDS		= 0x0002;
const int TYPE_INVITES		= 0x0003;
const int TYPE_ANNOUNCE		= 0x0004;
const int TYPE_ADDAPHOTO		= 0x0005;
const int TYPE_CLIPART		= 0x0006;
const int TYPE_SMILEYS		= 0x0007;
const int TYPE_SOUNDS		= 0x0008;
const int TYPE_FAVORITES		= 0x0009;

const int NUM_OF_BTNS		= 11; // Total toolbar buttons

struct ITEMINFOTYPE
{
	int	BitmapID;
	LPCTSTR szText;
	LPCTSTR szQuery;
};
const UINT ITEM_COUNT		= 8;
const UINT LAST_CAT_USED	= 7;
const UINT DEFAULT_CAT		= 1; // Smileys
const ITEMINFOTYPE ITEM_INFO[ITEM_COUNT] =			{{IDB_BTN1, " Stationery",	"?type=sta"},	// index = 0
													{IDB_BTN2, " Smileys",		"?type=smi"},	// index = 1
													{IDB_BTN3, " Clip Art",		"?type=clp"},	// index = 2
													{IDB_BTN5, " Add A Photo",	"?type=pho"},	// index = 3
													{IDB_BTN6, " Invitations",	"?type=inv"},	// index = 4
													{IDB_BTN7, " Announcements","?type=ann"},	// index = 5
													{IDB_BTN8, " Sounds",		"?type=snd"},	// index = 6			
													{-1,	   " Last Used",	""}};			// index = 7

const UINT MAX_CATS = 11;
//										    na,  sta, ecrd, inv, ann, pho, clp, smi, snd, fav, opt
const int WebToPluginCatConvert[MAX_CATS] = {-1,  0,   -1,   4,   5,   3,   2,   1,   6,   -1,  -1};

#define UWM_HELP_MSG _T("UWM_HELP_MSG-46D6D2C8-D238-4e79-ABAC-8A2B9E381C7A")
const UINT UWM_HELP	= ::RegisterWindowMessage(UWM_HELP_MSG);

#define UWM_UPDATESIZE_MSG _T("UWM_UPDATESIZE_MSG-6EF001DE-8129-46cc-82F3-FA697323C916")
const UINT UWM_UPDATESIZE	= ::RegisterWindowMessage(UWM_UPDATESIZE_MSG);

#define UWM_MAILSEND_MSG _T("UWM_MAILSEND_MSG-37793B4C-4D0A-497c-9DEC-5B52D33EC2B9")
const UINT UWM_MAILSEND	= ::RegisterWindowMessage(UWM_MAILSEND_MSG);

#define UWM_CMACTIVATE_MSG _T("UWM_CMACTIVATE_MSG-94B97052-249C-4174-8312-7BC4A869A24B")
const UINT UWM_CMACTIVATE	= ::RegisterWindowMessage(UWM_CMACTIVATE_MSG);

#define UWM_LAUNCH_MSG _T("UWM_LAUNCH_MSG-065C1759-72DB-4dc2-ADBF-CB3C2103D2D8")
const UINT UWM_LAUNCH	= ::RegisterWindowMessage(UWM_LAUNCH_MSG);

#define UWM_SHUTDOWN_MSG _T("UWM_SHUTDOWN_MSG-33E531B1_14D3_11d5_A025_006067718D04")
const UINT UWM_SHUTDOWN	= ::RegisterWindowMessage(UWM_SHUTDOWN_MSG);

#define UWM_SHUTDOWNALL_MSG _T("UWM_SHUTDOWNALL_MSG-DE8B2FF0-BD06-4e5b-8325-52DEA2E6DE7E")
const UINT UWM_SHUTDOWNALL	= ::RegisterWindowMessage(UWM_SHUTDOWNALL_MSG);

#define UWM_REMOVEADDIN_MSG _T("UWM_REMOVEADDIN_MSG-80A5FBF0-E5D6-413a-870A-1260B1E85BB1")
const UINT UWM_REMOVEADDIN	= ::RegisterWindowMessage(UWM_REMOVEADDIN_MSG);

#define UWM_UPDATEADDINS_MSG _T("UWM_UPDATEEADDINS_MSG-857D99BB-7300-49f2-8726-DF71A4FD5CB2")
const UINT UWM_UPDATEADDINS	= ::RegisterWindowMessage(UWM_UPDATEADDINS_MSG);

#define UWM_OPTIONS_MSG _T("UWM_OPTIONS_MSG-B4252986-3590-4f1a-A89A-AE18F78216AC")
const UINT UWM_OPTIONS	= ::RegisterWindowMessage(UWM_OPTIONS_MSG);

#define UWM_PREFERENCES_MSG _T("UWM_PREFERENCE_MSG-EADA2787-622E-42c2-BB25-1B86EA771741")
const UINT UWM_PREFERENCES	= ::RegisterWindowMessage(UWM_PREFERENCES_MSG);

#define UWM_UPDATERESOURCE_MSG _T("UWM_UPDATERESOURCE_MSG-EC298CD4-7EC0-4e15-AC68-6F5B4D06508C")
const UINT UWM_UPDATERESOURCE = ::RegisterWindowMessage(UWM_UPDATERESOURCE_MSG);

const LPCTSTR HTML_ATTRIBUTE_CONTENTEDITABLE	= "contentEditable";
const LPCTSTR DEFAULT_STATIONERY_FOLDER			= _T("Default Stationery");
const LPCTSTR DEFAULT_OL_STATIONERY_PATH		= _T("Default Stationery\\Outlook\\Default.html");
const LPCTSTR DEFAULT_OE_STATIONERY_PATH		= _T("Default Stationery\\Express\\Default.html");
const LPCTSTR DEFAULT_IE_STATIONERY_PATH		= _T("Default Stationery\\IE\\Default.html");
const LPCTSTR DEFAULT_AOL_STATIONERY_PATH		= _T("Default Stationery\\AOL\\Default.html");



const int CMD_REGISTERADDIN			= 0x1001;
const int CMD_UNREGISTERADDIN		= 0x1002;
const int CMD_OPTIMIZATIONWIZARD	= 0x1003;
const int CMD_CMSETTINGS			= 0x1004;
const int CMD_CMLAUNCH				= 0x1005;


const DWORD UNINSTALL_STATUS_DONE			= 0x0000;
const DWORD UNINSTALL_STATUS_MAINTENANCE	= 0x0001;

const int	PAGE_UNAVAILABLE		= 0x0002;
const int	PAGE_WAIT				= 0x0003;