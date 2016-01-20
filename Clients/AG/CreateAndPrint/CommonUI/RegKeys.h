#pragma once

#define REGKEY_APPROOT			HKEY_CURRENT_USER

#define REGKEY_APP				"Software\\American Greetings\\Create And Print"

#define REGVAL_INSTALLPATH		"InstallPath"
#define REGVAL_SPECIALSENABLED	"bEnabled"
#define REGVAL_METRIC			"Metric"
#define REGVAL_SHORTCUT_VERSION	"ShortcutVersion"
#define REGVAL_MRU_PICTURE_PATH	"MRUPicturePath"
#define REGVAL_MRU_CTP_PATH		"MRUCtpPath"

#define REGKEY_DUPLEX			"Software\\American Greetings\\Create And Print\\Duplex Printing"
#define REGKEY_PAPERFEED		"Software\\American Greetings\\Create And Print\\Envelope Code"

#define REGKEY_FONT 			"Software\\Microsoft\\Windows\\CurrentVersion\\Fonts"
#define REGKEY_NTFONT			"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"

#define REGVAL_SHELL_OPEN		"Ctp.File\\shell\\open\\command"
#define REGVAL_SHELL_PRINT		"Ctp.File\\shell\\print\\command"

// AutoUpdate setting
#define REGVAL_AUTOUPDATE		"AutoUpdate"

// Addressbook
#define REGVAL_ADDRESSBOOK_URL	"AddressbookURL"
#define REGVAL_SENDADDRESS		"SendAddress"
#define REGVAL_RETURNADDRESS	"ReturnAddress"

// Clip Art URL
#define REGVAL_CLIPART_URL	"ClipArtURL"

/////////////////////////////////////////////////////////////////////////////
bool ReadRegistryDWORD(LPCTSTR lpszRegValName, DWORD& dwValue);
bool WriteRegistryDWORD(LPCTSTR lpszRegValName, DWORD dwValue);
bool ReadRegistryString(LPCTSTR lpszRegValName, CString& szValue);
bool WriteRegistryString(LPCTSTR lpszRegValName, LPCTSTR szValue);
