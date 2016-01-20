#pragma once
#define REGKEY_APPROOT				HKEY_CURRENT_USER

#define REGKEY_ADDIN_LOCATION		_T("CLSID\\{BF897EBA-7FBE-41B5-85A3-FBC9212DB1C5}\\LocalServer32")
#define REGKEY_CLSID_OE				_T("CLSID\\{DA514619-4DF0-4F28-A591-0639E104A397}")
#define REGKEY_CLSID_OL				_T("CLSID\\{C704648D-6030-47E9-ADBA-1E13B6A784AE}")
#define REGKEY_CLSID_IE				_T("CLSID\\{F8B5C2ED-0329-413A-AEE6-4134E93C3E38}")
#define REGKEY_CLSID_AOL			_T("CLSID\\{B90DFDAB-F41C-492B-9EDB-59B9277C863C}")

#define REGKEY_APP					_T("Software\\BlueMountain\\Creata Mail")
#define REGVAL_INSTALLPATH			_T("InstallPath")

// Search history location
#define REGKEY_SEARCH_HISTORY		_T("Software\\BlueMountain\\Creata Mail\\History")

// Browse dialog location
#define REGVAL_LOCATION				_T("Location")

// SmartCache setting
#define REGVAL_SMARTCACHE			_T("SmartCache")

// Always on Top Status
#define REGVAL_TOPMOST				_T("TopMost")

// Default Stationery				
#define REGVAL_DEFAULTSTA			_T("Default Sta")

// AutoUpdate setting
#define REGVAL_AUTOUPDATE			_T("AutoUpdate")
#define REGVAL_AUTOUPDATE_STATUS	_T("AutoUpdate Status")

// Default IE Category 
#define REGVAL_DEFAULT_IECAT		_T("Default IE Cat")


// Last Used Category 
#define REGVAL_LAST_IE_CAT				_T("Last IE Cat")
#define REGVAL_LAST_OL_CAT				_T("Last OL Cat")
#define REGVAL_LAST_OE_CAT				_T("Last OE Cat")
#define REGVAL_LAST_AOL_CAT				_T("Last AOL Cat")

// Authentication values
#define REGVAL_USERID				"User"
#define REGVAL_PASSWORD				"Data"
#define REGVAL_MEMBERSTATUS			"MS"
#define REGVAL_HOST					"Host"
#define REGVAL_UPGRADEURL			"Upgrade"
#define REGVAL_USAGEURL				"Usage"
#define REGVAL_BROWSEPAGE			"Browse"
#define REGVAL_HELPPAGE				"Help"
#define REGVAL_TIMESTAMP			"TS"
#define REGVAL_PREVIEW_LOCATION		"Preview Location"
#define REGVAL_VERSION				"Version_CreataMailSetup.exe"
#define REGVAL_MEMBERNUM			"Member #"
#define REGVAL_SHOWICON				"Show"
#define REGVAL_USAGETIMESTAMP		"UTS"
#define REGVAL_LOGGING				"Log"
#define REGVAL_LOGGING_LEVEL		"Log Level"
#define REGVAL_LOGGING_SERVER		"Log Server"
#define REGVAL_INSTALL_STATUS		"Install Status"
#define REGVAL_AUTH_STATUS			"Result Status"
#define REGVAL_AOL_STATUS			"AOL"
#define REGVAL_LINELIST_DATE		"LineListDate"
#define REGVAL_LINELIST_UPDATE		"LineListUpdate"
#define REGVAL_PROMOSTATUS			"PromoStatus"
#define REGVAL_CLIENTSTATUS			"ClientStatus"
#define REGVAL_SOURCE				"source"

// For checking to see if Word is the email editor
#define REGKEY_OUTLOOK08_USING_WORD	"Software\\Microsoft\\Office\\8.0\\Outlook\\Options\\Mail"
#define REGKEY_OUTLOOK09_USING_WORD	"Software\\Microsoft\\Office\\9.0\\Outlook\\Options\\Mail"
#define REGKEY_OUTLOOK10_USING_WORD	"Software\\Microsoft\\Office\\10.0\\Outlook\\Options\\Mail"
#define REGKEY_OUTLOOK11_USING_WORD	"Software\\Microsoft\\Office\\11.0\\Outlook\\Options\\Mail"
#define REGVAL_OUTLOOK_USING_WORD	"EditorPreference" // DWORD bit 0 is on if using Word

// For checking to see if the Outlook security zone is restricted
#define REGKEY_OUTLOOK08_RESTRICTED	"Software\\Microsoft\\Office\\8.0\\Outlook\\Options\\General"
#define REGKEY_OUTLOOK09_RESTRICTED	"Software\\Microsoft\\Office\\9.0\\Outlook\\Options\\General"
#define REGKEY_OUTLOOK10_RESTRICTED	"Software\\Microsoft\\Office\\10.0\\Outlook\\Options\\General"
#define REGKEY_OUTLOOK11_RESTRICTED	"Software\\Microsoft\\Office\\11.0\\Outlook\\Options\\General"
#define REGVAL_OUTLOOK_RESTRICTED	"Security Zone" // DWORD == 3 if Internet Zone, 4 if Restricted Zone

// For checking to see if the Outlook Express security zone is restricted
#define REGKEY_IDENTITIES			"Identities"
#define REGVAL_DEFAULT_USER_ID		"Default User ID"
#define REGVAL_LAST_USER_ID			"Last User ID"

#define REGKEY_OUTLOOKEX_RESTRICTED	  "Software\\Microsoft\\Outlook Express\\5.0"
#define REGVAL_OUTLOOKEX_RESTRICTED	  "Email Security Zone" // DWORD == 3 if Internet Zone, 4 if Restricted Zone
#define REGVAL_OUTLOOKEX_RESTRICTED2  "Security Zone" // DWORD == 3 if Internet Zone, 4 if Restricted Zone
#define REGKEY_OUTLOOKEX_SENDPICTURES "Software\\Microsoft\\Outlook Express\\5.0\\Mail"
#define REGVAL_OUTLOOKEX_SENDPICTURES "Send Pictures With Document"

// For finding the user's default email account 
#define REGKEY_INTERNET_ACCOUNTMGR	"Software\\Microsoft\\Internet Account Manager"
#define REGKEY_INTERNET_ACCOUNTS	"Software\\Microsoft\\Internet Account Manager\\Accounts\\"
#define REGKEY_OUTLOOK_ACCOUNTMGR	"Software\\Microsoft\\Office\\Outlook\\OMI Account Manager"
#define REGKEY_OUTLOOK_ACCOUNTS		"Software\\Microsoft\\Office\\Outlook\\OMI Account Manager\\Accounts\\"
#define REGVAL_DEFAULTMAILACCOUNT	"Default Mail Account"
#define REGVAL_EMAILADDRESS			"SMTP Email Address"

// For modifying IE's caching 
#define REGKEY_IE					"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"
#define REGVAL_SYNCMODE				"SyncMode"
#define REGVAL_SYNCMODE5			"SyncMode5"

// For modifying IE's ActiveX security
#define REGKEY_IE_ZONEDEFAULTS		"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\ProtocolDefaults"
#define REGVAL_HTTP					"http"

#define REGKEY_IE_ZONES				"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones"
#define REGVAL_RUNACTIVEX			"1200"

#define REGKEY_IE_MAIN				"Software\\Microsoft\\Internet Explorer\\Main"
#define REGVAL_IE_ENABLE_EXTENSIONS	"Enable Browser Extensions"
