#pragma once

static const DWORD  ADDIN_ERROR_NONE		= 0x00000000; // NO bit set
static const DWORD  ADDIN_ERROR_CLSID		= 0x00000001; // bit 0 set
static const DWORD  ADDIN_ERROR_TITLE		= 0x00000002; // bit 1 set
static const DWORD  ADDIN_ERROR_ICONPATH    = 0x00000004; // bit 2 set
static const DWORD  ADDIN_ERROR_TIP			= 0x00000008; // bit 3 set
static const DWORD  ADDIN_ERROR_VERSION		= 0x00000010; // bit 4 set
static const DWORD  ADDIN_ERROR_PROGID		= 0x00000020; // bit 5 set


static const DWORD  ADDIN_ERROR_REG			= 0x00010000; // bit 16 set
static const DWORD  ADDIN_ERROR_UNREG	    = 0x00020000; // bit 17 set
static const DWORD  ADDIN_ERROR_READ	    = 0x00040000; // bit 18 set
static const DWORD  ADDIN_ERROR_OPEN	    = 0x00080000; // bit 19 set

struct ADDIN_INFO_TYPE
{
	CString szTitle;
	CString szProgId;
	CString szIconPath;
	CString szTip;
	CString szVersion;
	HWND	hWnd;
	CComPtr<IUnknown> spIUnknown;
};
typedef std::map <LPCTSTR, ADDIN_INFO_TYPE> ADDIN_MAP_TYPE;
class CAddinManager
{
public:
	CAddinManager(HWND hwndClient, HKEY hKeyRoot, LPCTSTR szKeyName);
public:
	virtual ~CAddinManager(void);
public:
	DWORD GetAddins(void);
	DWORD RegisterAddin(LPCTSTR szAddinCLSID, ADDIN_INFO_TYPE &AddinInfo);
	DWORD UnregisterAddin(LPCTSTR szAddinCLSID);
	bool ReadAddinInfo(CRegKey& regKey, ADDIN_INFO_TYPE& AddinInfo);
private:
	HWND m_hwndClient;
	HKEY m_hKeyRoot;
	CString m_szKeyAddins;
	ADDIN_MAP_TYPE m_AddinList;

};
