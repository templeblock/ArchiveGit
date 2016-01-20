#pragma once

DWORD Version(WORD wMajor, WORD wMinor = NULL, WORD wBuild1 = NULL, WORD wBuild2 = NULL);
DWORD Version(LPCTSTR strVersion);
DWORD GetFileVersion(LPCTSTR szModuleFileName);
DWORD GetFileVersion(LPCTSTR szModuleFileName, LPTSTR szVersionInfo);
