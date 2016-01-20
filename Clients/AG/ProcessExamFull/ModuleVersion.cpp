#include "StdAfx.h"
#include "ModuleVersion.h"

/////////////////////////////////////////////////////////////////////////////
CModuleVersion::CModuleVersion()
{
	m_pVersionInfo = NULL;
	memset(&m_VersionInfo, 0, sizeof(m_VersionInfo));
}

/////////////////////////////////////////////////////////////////////////////
CModuleVersion::~CModuleVersion()
{
	if (m_pVersionInfo)
		delete [] m_pVersionInfo;
}

/////////////////////////////////////////////////////////////////////////////
bool CModuleVersion::GetFileVersionInfo(LPCTSTR strFilename)
{
	DWORD dwDummyHandle;
	DWORD dwLength = ::GetFileVersionInfoSize(strFilename, &dwDummyHandle);
	if (dwLength <= 0)
		return false;

	if (m_pVersionInfo)
		delete [] m_pVersionInfo;

	m_pVersionInfo = new BYTE[dwLength]; // allocate version info
	if (!::GetFileVersionInfo(strFilename, 0, dwLength, m_pVersionInfo))
		return false;

	LPVOID lpvi = NULL;
	UINT iLength = 0;
	if (!::VerQueryValue(m_pVersionInfo, "\\", &lpvi, &iLength) || iLength < sizeof(VS_FIXEDFILEINFO))
		return false;

	m_VersionInfo = *(VS_FIXEDFILEINFO*)lpvi;

	// Get translation info
	if (!::VerQueryValue(m_pVersionInfo, "\\VarFileInfo\\Translation", &lpvi, &iLength) || iLength < sizeof(TRANSLATION))
	{
		m_Translation.langID = 1033; // default
		m_Translation.charset = 1252; // default = ANSI code page
	}
	else
		m_Translation = *(TRANSLATION*)lpvi;

	return m_VersionInfo.dwSignature == VS_FFI_SIGNATURE;
}

/////////////////////////////////////////////////////////////////////////////
bool CModuleVersion::GetModuleVersionInfo(LPCTSTR strModuleName)
{
	TCHAR strFilename[_MAX_PATH];
	HMODULE hModule = ::GetModuleHandle(strModuleName);
	if (!hModule && strModuleName)
		return false;

	DWORD len = ::GetModuleFileName(hModule, strFilename, sizeof(strFilename)/sizeof(strFilename[0]));
	if (len <= 0)
		return false;

	return GetFileVersionInfo(strFilename);
}

/////////////////////////////////////////////////////////////////////////////
// Key name is something like "CompanyName".
CString CModuleVersion::GetValue(LPCTSTR lpKeyName)
{
	if (!m_pVersionInfo)
		return "";

	// To get a string value must pass query in the form "\StringFileInfo\<langID><codepage>\keyname"
	// where <langID><codepage> is the languageID concatenated with the code page (in hex)
	CString strQuery;
	strQuery.Format("\\StringFileInfo\\%04x%04x\\%s", m_Translation.langID, m_Translation.charset, lpKeyName);

	LPCTSTR pValue = NULL;
	UINT iLength = 0;
	CString strValue;
	if (::VerQueryValue(m_pVersionInfo, (LPTSTR)(LPCTSTR)strQuery, (LPVOID*)&pValue, &iLength))
		strValue = pValue;

	return strValue;
}

typedef HRESULT (CALLBACK* DLLGETVERSIONPROC)(DLLVERSIONINFO*);

/////////////////////////////////////////////////////////////////////////////
// Get DLL Version by calling DLL's DllGetVersion proc
bool CModuleVersion::DllGetVersion(LPCTSTR pModuleName, DLLVERSIONINFO& dvi)
{
	memset(&dvi, 0, sizeof(dvi));
	dvi.cbSize = sizeof(dvi);

	HINSTANCE hInstance = ::LoadLibrary(pModuleName);
	if (!hInstance)
		return false;

	DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hInstance, "DllGetVersion");
	bool bOK = (pDllGetVersion ? SUCCEEDED((*pDllGetVersion)(&dvi)) : false);
	::FreeLibrary(hInstance);
	return bOK;
}
