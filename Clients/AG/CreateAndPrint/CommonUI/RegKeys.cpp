#include "stdafx.h"
#include "RegKeys.h"

/////////////////////////////////////////////////////////////////////////////
bool WriteRegistryDWORD(LPCTSTR lpszRegValName, DWORD dwValue)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetDWORDValue(lpszRegValName, dwValue) != ERROR_SUCCESS)
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool ReadRegistryString(LPCTSTR lpszRegValName, CString& szValue) 
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	if (regkey.QueryStringValue(lpszRegValName, szBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	szValue = szBuffer;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool WriteRegistryString(LPCTSTR lpszRegValName, LPCTSTR szValue)
{
	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(lpszRegValName, szValue) != ERROR_SUCCESS)
		return false;

	return true;
}