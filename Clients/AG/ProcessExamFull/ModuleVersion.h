#pragma once
#include "shlwapi.h"

// tell linker to link with version.lib for VerQueryValue, etc.
#pragma comment(linker, "/defaultlib:version.lib")

/////////////////////////////////////////////////////////////////////////////
// CModuleVersion ver;
// if (ver.GetFileVersionInfo("_T("mymodule)))
// {
//		// info is in ver, you can call GetValue to get variable info like
//		CString s = ver.GetValue(_T("CompanyName"));
// }
//
// You can also call the static function DllGetVersion to get DLLVERSIONINFO
//

class CModuleVersion
{
public:
	CModuleVersion();
	virtual ~CModuleVersion();

	bool GetFileVersionInfo(LPCTSTR strFileName);
	bool GetModuleVersionInfo(LPCTSTR strModuleName);
	CString	GetValue(LPCTSTR lpKeyName);
	static bool DllGetVersion(LPCTSTR strModuleName, DLLVERSIONINFO& dvi);

protected:
	BYTE* m_pVersionInfo;	// all version info
	VS_FIXEDFILEINFO m_VersionInfo;
	struct TRANSLATION
	{
		WORD langID;		// language ID
		WORD charset;		// character set (code page)
	} m_Translation;
};
