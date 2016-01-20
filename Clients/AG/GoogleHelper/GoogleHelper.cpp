// GoogleHelper.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "tchar.h"
#include "shellapi.h"
#include "Msi.h"
#include "Msiquery.h"
#include "GTAPI.h"
////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL WINAPI GoogleValidate(MSIHANDLE hInstall)
{
	TCHAR szSupportDir[MAX_PATH];
    DWORD dwSize = sizeof(szSupportDir);
    DWORD dwResults = MsiGetProperty(hInstall, _T("SUPPORTDIR"), szSupportDir, &dwSize);
	if (dwResults != ERROR_SUCCESS)
		return true ;

	CString szGTAPIPath = szSupportDir;

	szGTAPIPath += _T("GTAPI.dll");

	//::MessageBox(NULL, szGTAPIPath, "GoogleValidate", MB_OK);

	DWORD dwReason=0;
	if (!ToolbarCompatibilityCheck(&dwReason))
		return false;

	return true;

}
/////////////////////////////////////////////////////////////////////////////
void WINAPI InstallGoogleToolbar(MSIHANDLE hInstall)
{
	// Get SupportDir path
	TCHAR szSupportDir[MAX_PATH];
    DWORD dwSize = sizeof(szSupportDir);
    MsiGetProperty(hInstall, _T("SUPPORTDIR"), szSupportDir, &dwSize);
	CString szGoogleSetup = szSupportDir;

	// Ensure path ends with '\'
	int iLen = szGoogleSetup.GetLength();
	if (szGoogleSetup.Find(_T("\\")) < iLen-1)
		szGoogleSetup += _T("\\");

	// Get appropo Toolbar setup based on OS version.
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&osvi);

	if (osvi.dwMajorVersion < 5)
		szGoogleSetup += _T("GoogleToolbarInstaller9X_en.exe"); // 98/ME setup	
	else
		szGoogleSetup += _T("GoogleToolbarInstaller_en.exe");	// 2000/XP setup


	//::MessageBox(NULL, szGoogleSetup, "InstallGoogleToolbar", MB_OK);


	// Add Google command line arguments - run silent (/q) and disable page rank (/d).
	szGoogleSetup += _T(" /q /d");

	// Launch setup
	STARTUPINFO startup_info;
	ZeroMemory(&startup_info, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);

	PROCESS_INFORMATION process_info;
	ZeroMemory(&process_info, sizeof(process_info));

	if (CreateProcess(NULL, szGoogleSetup.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &startup_info, &process_info))
	{
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hThread);
		CloseHandle(process_info.hProcess);
	}

	return;
}
/////////////////////////////////////////////////////////////////////////////
void WINAPI ShowGoogleTerms(MSIHANDLE hInstall)
{
	::ShellExecute(NULL, "open", _T("http://toolbar.google.com/partners/terms"), NULL, NULL, SW_SHOWNORMAL);
}
/////////////////////////////////////////////////////////////////////////////
void WINAPI ShowGooglePrivacyPolicy(MSIHANDLE hInstall)
{
	::ShellExecute(NULL, "open", _T("http://www.google.com/support/toolbar/?quick=privacy"), NULL, NULL, SW_SHOWNORMAL);
}