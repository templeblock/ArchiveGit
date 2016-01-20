#include "stdafx.h"
#include "VersionInfo.h"
#pragma comment(lib, "version.lib")

/////////////////////////////////////////////////////////////////////////////
DWORD Version(WORD wMajor, WORD wMinor, WORD wBuild1, WORD wBuild2)
{
	// Pack the 4 parts of a version number into a DWORD for easy numeric comparison
	// This packing limits wMajor <= 255, wMinor <= 255, and does not take wBuild2 into account
	wMajor  = min(wMajor, 255);
	wMinor  = min(wMinor, 255);
	return (DWORD)((LOBYTE(wMajor) << 24) | (LOBYTE(wMinor) << 16) | LOWORD(wBuild1));
}

/////////////////////////////////////////////////////////////////////////////
DWORD Version(LPCTSTR strVersion)
{
	// Convert a version string into the 4 component parts of a version number, then...
	// Pack the 4 parts of the version number into a DWORD for easy numeric comparison
	WORD wMajor = 0;
	WORD wMinor = 0;
	WORD wBuild1 = 0;
	WORD wBuild2 = 0;
	int n = sscanf_s(strVersion, "%hu.%hu.%hu.%hu", &wMajor, &wMinor, &wBuild1, &wBuild2);
	return Version(wMajor, wMinor, wBuild1, wBuild2);
}

/////////////////////////////////////////////////////////////////////////////
DWORD GetFileVersion(LPCTSTR szModuleFileName, DWORD dwLen, LPTSTR szVersionInfo)
{
	if (dwLen == 0)
		return 0;
	
	*szVersionInfo = 0;
	DWORD dwHandle = NULL;
	DWORD dwSize = ::GetFileVersionInfoSize(szModuleFileName, &dwHandle); 
	if (!dwSize)
		return 0;

	LPBYTE lpVersionData = new BYTE[dwSize]; 
	if (!::GetFileVersionInfo(szModuleFileName, dwHandle, dwSize, (void**)lpVersionData))
	{
		delete [] lpVersionData;
		return 0;
	}

	UINT nQuerySize = 0;
	VS_FIXEDFILEINFO* pVersionInfo = NULL;
	bool bOK = !!::VerQueryValue((void **)lpVersionData, _T("\\"), (void**)&pVersionInfo, &nQuerySize);
	WORD wMajor  = HIWORD(pVersionInfo->dwFileVersionMS);
	WORD wMinor  = LOWORD(pVersionInfo->dwFileVersionMS);
	WORD wBuild1 = HIWORD(pVersionInfo->dwFileVersionLS);
	//WORD wBuild2 = LOWORD(pVersionInfo->dwFileVersionLS);
	delete [] lpVersionData;

	if (!bOK)
		return 0;

	int n = sprintf_s(szVersionInfo, dwLen, "%02hu.%02hu.%04hu", wMajor, wMinor, wBuild1);
	if (n < 0)
		return 0;

	return 1;
}

/////////////////////////////////////////////////////////////////////////////
DWORD GetFileVersion(LPCTSTR szModuleFileName)
{
	DWORD dwHandle = NULL;
	DWORD dwSize = ::GetFileVersionInfoSize(szModuleFileName, &dwHandle); 
	if (!dwSize)
		return 0;

	LPBYTE lpVersionData = new BYTE[dwSize]; 
	if (!::GetFileVersionInfo(szModuleFileName, dwHandle, dwSize, (void**)lpVersionData))
	{
		delete [] lpVersionData;
		return 0;
	}

	UINT nQuerySize = 0;
	VS_FIXEDFILEINFO* pVersionInfo = NULL;
	bool bOK = !!::VerQueryValue((void **)lpVersionData, _T("\\"), (void**)&pVersionInfo, &nQuerySize);
	WORD wMajor  = HIWORD(pVersionInfo->dwFileVersionMS);
	WORD wMinor  = LOWORD(pVersionInfo->dwFileVersionMS);
	WORD wBuild1 = HIWORD(pVersionInfo->dwFileVersionLS);
	WORD wBuild2 = LOWORD(pVersionInfo->dwFileVersionLS);
	delete [] lpVersionData;

	if (!bOK)
		return 0;

	// Pack the 4 parts of the version number into a DWORD for easy numeric comparison
	return Version(wMajor, wMinor, wBuild1, wBuild2);
}
