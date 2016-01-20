#include "stdafx.h"
#include "HelperFunctions.h"
#include "Utility.h"
#include "ModuleVersion.h"
#include "HTTPClient.h"
#include "Internet.h"


#pragma comment(lib, "winmm.lib")

////////////////////////////////////////////////////////////////////////////////////////////////////
bool RemoveFilesFromFolder(LPCTSTR szFolder, LPCTSTR szType)
{
	// Delete all html files in default stationery folder.
	CString szFindPath = szFolder;
	if (szFindPath.Find(_T("\\")) < 0)
		return false;

	WIN32_FIND_DATA FileData;
	HANDLE hFind;
	
	// Ensure path ends with '\'
	int iLen = szFindPath.GetLength();
	if (szFindPath.Find(_T("\\")) < iLen-1)
		szFindPath += _T("\\");

	CString szPath = szFindPath;

	// Concatenate file type filter to path.
	CString szFileFilter;
	szFileFilter = _T("*.");
	szFileFilter += szType;
	szFindPath += szFileFilter; // _T("*.htm?");


	// Look for first occurence
	hFind = FindFirstFile(szFindPath, &FileData); 
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		FindClose(hFind);
		return false;
	}
	
	
	// Iterate thru files and delete.
	CString strFilePath;
	bool bDone = false;
	while (!bDone)
	{
		strFilePath = szPath;
		strFilePath += FileData.cFileName;
		if (!DeleteFile(strFilePath))
			GetError(_T("RemoveFilesFromFolder()"));

		if (!FindNextFile(hFind, &FileData)) 
		{
			if (GetLastError() != ERROR_NO_MORE_FILES) 
				GetError(_T("FindNextFile(hFind, &FileData)"));

			bDone = true; 
		}
	}
	

	FindClose(hFind);
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsFilePresent(LPCTSTR szFilePath)
{
	CString szFindPath = szFilePath;
	if (szFindPath.Find(_T("\\")) < 0)
		return false;
	
	WIN32_FIND_DATA FileData;
	HANDLE hFind;

	hFind = FindFirstFile(szFindPath, &FileData); 
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		FindClose(hFind);
		return false;
	}
	
	FindClose(hFind);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
WNDPROC SetWindowProc(HWND hwnd, LONG pfProc)
{
	if (!::IsWindow(hwnd))
		return NULL;

	LONG dwAddr  = pfProc;
	WNDPROC pfOrigProc = (WNDPROC)::SetWindowLong(hwnd, GWL_WNDPROC, dwAddr);
	return pfOrigProc;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL RestoreWindowProc(HWND hwnd, LONG pfProc)
{
	if (!pfProc || !::IsWindow(hwnd))
		return false;

	LONG dwAddr = pfProc;
	WNDPROC pTempProc = (WNDPROC)::SetWindowLong(hwnd, GWL_WNDPROC, dwAddr);
	return (pTempProc != NULL);
}
