#include "stdafx.h"
#include "Utility.h"
#include <shlobj.h> // for SHGetSpecialFolderLocation() and SHGetPathFromIDList()
#include <intshcut.h> // for IUniformResourceLocator
#include <io.h>
#include <sys/stat.h>
#include "MyAtlUrl.h"

#import "msxml4.dll" named_guids
#pragma comment(lib, "winmm.lib") // for ::timeGetTime

//////////////////////////////////////////////////////////////////////
CString String(LPCSTR pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	CString str;
	str.FormatV(pszFormat, argList);
	va_end(argList);
	return str;
}

//////////////////////////////////////////////////////////////////////
CString String(UINT nFormatID, ...)
{
	CString strFormat;
	strFormat.LoadString(nFormatID);

	va_list argList;
	va_start(argList, nFormatID);
	CString str;
	str.FormatV(strFormat, argList);
	va_end(argList);
	return str;
}

//////////////////////////////////////////////////////////////////////
CString StrPath(LPCSTR szFilePath)
{
	CString strFilePath = szFilePath;
	int i = max(strFilePath.ReverseFind('/'), strFilePath.ReverseFind('\\'));
	return strFilePath.Left(i);
}

//////////////////////////////////////////////////////////////////////
CString StrFileName(LPCTSTR szFilePath)
{
	CString strFilePath = szFilePath;
	int i = max(strFilePath.ReverseFind('/'), strFilePath.ReverseFind('\\'));
	return strFilePath.Mid(i+1);
}

//////////////////////////////////////////////////////////////////////
CString StrExtension(LPCSTR szFilePath)
{
	CString strFilePath = szFilePath;
	int i = strFilePath.ReverseFind('.');
	if (i < 0)
		return "";
	CString strExtension = strFilePath.Mid(i);
	strExtension.MakeLower();
	return strExtension;
}

//////////////////////////////////////////////////////////////////////
CString StripExtension(LPCTSTR szFilePath)
{
	CString strFilePath = szFilePath;
	int i = strFilePath.ReverseFind('.');
	return (i < 0 ? strFilePath : strFilePath.Left(i));
}

//////////////////////////////////////////////////////////////////////
CString HRString(HRESULT hr)
{
	char szMessage[512];
	if (::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), szMessage, sizeof(szMessage), NULL))
	{
		CString strMessage = szMessage;
		strMessage.TrimRight();
		return strMessage;
	}

	CString strMessage;
	switch (hr)
	{ // URL Moniker Error Codes - returned in IBSC::OnStopBinding
		case INET_E_AUTHENTICATION_REQUIRED:	strMessage = "Authentication is needed to access the object."; break;
		case INET_E_CANNOT_CONNECT:				strMessage = "The attempt to connect to the Internet has failed."; break;
		case INET_E_CANNOT_INSTANTIATE_OBJECT:	strMessage = "CoCreateInstance failed."; break;
		case INET_E_CANNOT_LOAD_DATA:			strMessage = "The object could not be loaded."; break;
		case INET_E_CANNOT_LOCK_REQUEST:		strMessage = "The requested resource could not be locked."; break;
		case INET_E_CANNOT_REPLACE_SFP_FILE:	strMessage = "The exact version requested by a component download cannot be found."; break;
		case INET_E_CODE_DOWNLOAD_DECLINED:		strMessage = "The component download was declined by the user."; break;
		case INET_E_CONNECTION_TIMEOUT:			strMessage = "The Internet connection has timed out."; break;
		case INET_E_DATA_NOT_AVAILABLE:			strMessage = "An Internet connection was established, but the data cannot be retrieved."; break;
		case INET_E_DOWNLOAD_FAILURE:			strMessage = "The download has failed (the connection was interrupted)."; break;
		case INET_E_INVALID_REQUEST:			strMessage = "The request was invalid."; break;
		case INET_E_INVALID_URL:				strMessage = "The URL could not be parsed."; break;
		case INET_E_NO_SESSION:					strMessage = "No Internet session was established."; break;
		case INET_E_NO_VALID_MEDIA:				strMessage = "The object is not in one of the acceptable MIME types."; break;
		case INET_E_OBJECT_NOT_FOUND:			strMessage = "The object was not found."; break;
		case INET_E_QUERYOPTION_UNKNOWN:		strMessage = "The requested option is unknown."; break;
		case INET_E_REDIRECT_TO_DIR:			strMessage = "The request is being redirected to a directory."; break;
		case INET_E_REDIRECTING:				strMessage = "The request is being redirected."; break;
		case INET_E_RESOURCE_NOT_FOUND:			strMessage = "The server or proxy was not found."; break;
		case INET_E_RESULT_DISPATCHED:			strMessage = "The binding has already been completed and the result has been dispatched, so your abort call has been canceled."; break;
		case INET_E_SECURITY_PROBLEM:			strMessage = "A security problem was encountered."; break;
		case INET_E_UNKNOWN_PROTOCOL:			strMessage = "The protocol is not known and no pluggable protocols have been entered that match."; break;
		case INET_E_USE_DEFAULT_PROTOCOLHANDLER:strMessage = "Use the default protocol handler."; break;
		case INET_E_USE_DEFAULT_SETTING:		strMessage = "Use the default settings."; break;
	}

	return strMessage;
}

//////////////////////////////////////////////////////////////////////
bool AllocateHeap(DWORD aptr, LPCSTR lpszString)
{
	(*(VOID**)aptr) = ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(lpszString));
	if (!(*(VOID**)aptr))
		return false;

	memcpy((*(VOID**)aptr), lpszString, strlen(lpszString));

	return true;
}

//////////////////////////////////////////////////////////////////////
bool AppendStringToHeap(DWORD aptr, LPCSTR lpszString)
{
	HANDLE hHeap = ::GetProcessHeap();
	VOID* xptr = *((VOID**)aptr);
	int hLen = ::HeapSize(hHeap, 0, xptr);

	(*(VOID**)aptr) = ::HeapReAlloc(hHeap, HEAP_ZERO_MEMORY, xptr, hLen+strlen(lpszString));
	if (!(*(VOID**)aptr))
		return false;

	xptr = *((VOID**)aptr);
	memcpy((char*)xptr+hLen, lpszString, strlen(lpszString));
	return true;
}

// The following code does not like the 'new' debug macro, so save it undefine it, and restore it
#pragma push_macro("new")
#undef new
	#include <vector>
#pragma pop_macro("new")
using namespace std;

//////////////////////////////////////////////////////////////////////
int Replace(CString& strDest, const CString& strFind, const CString& strReplace)
{
	CString strDestLower = strDest;
	strDestLower.MakeLower();
	CString strFindLower = strFind;
	strFindLower.MakeLower();
	int iLength = strFindLower.GetLength();

	vector<int> List;
	int iCount = 0;
	int nIndex = 0;
	while ((nIndex = strDestLower.Find(strFindLower, nIndex)) >= 0)
	{
		List.push_back(nIndex);
		nIndex += iLength;
		iCount++;
	}

	int i = iCount;
	while (--i >= 0)
	{
		int nIndex = List.at(i);
		strDest = strDest.Left(nIndex) + strReplace + strDest.Mid(nIndex + iLength);
	}

	return iCount;
}

//////////////////////////////////////////////////////////////////////
CString dtoa(double fValue, int nDecimalDigits)
{
	int iDecimalPoint = 0;
	int iSign = 0;
	const int BUFFERSIZE = 64;
	char szDigits[BUFFERSIZE] = {0}; 

	// Updated to reflect security changes to CRT (VS 2005). Need to allocate destination - JHC
	if (_fcvt_s(szDigits, BUFFERSIZE, fValue, nDecimalDigits, &iDecimalPoint, &iSign))
		return "";

	int iTrailingZeros = -1;
	int i = 0;
	int j = 0;
	char szFloat[BUFFERSIZE] = {0};
	while (*szDigits && i < BUFFERSIZE && szDigits[j] != '\0')
	{
		if (i == iDecimalPoint)
		{
			if (!i)
				szFloat[i++] = '0';
			szFloat[i++] = '.';
			iTrailingZeros = 1;
		}

		if (iTrailingZeros != -1)
		{
			if (szDigits[j] == '0')
				iTrailingZeros++;
			else
				iTrailingZeros = 0;
		}


		szFloat[i++] = szDigits[j];
		j++;
	}

	if (iTrailingZeros != -1)
		i -= iTrailingZeros;

	szFloat[i] = '\0';

	return (CString)szFloat;
}

//////////////////////////////////////////////////////////////////////
CString Get4YourSoulPath()
{
	char szFileName[MAX_PATH];
	int nChars = GetTempPath(MAX_PATH, szFileName);
	szFileName[nChars] = 0;
	strcat_s(szFileName, _countof(szFileName), "4YourSoul");
	return szFileName;
}

//////////////////////////////////////////////////////////////////////
void CleanDirectory(CString strDirectory)
{
	if (strDirectory.GetLength()<=0)
		return;

	DWORD dwAttrs = ::GetFileAttributes(strDirectory);
	if (!(dwAttrs & FILE_ATTRIBUTE_DIRECTORY))
		return;

	bool bFound = false;
	CString strTemp = strDirectory + "\\*.*";
	WIN32_FIND_DATA FileData;
	HANDLE hSearch = FindFirstFile(strTemp, &FileData);
	if (hSearch == INVALID_HANDLE_VALUE)
		return;

	do{
		//if (FileData.cFileName check for . and ..
		strTemp = strDirectory + CString("\\") + FileData.cFileName;
		DeleteFile(strTemp);
		bFound = !!FindNextFile(hSearch, &FileData);
	} while (bFound);

	FindClose(hSearch);
}

//////////////////////////////////////////////////////////////////////
CString RandomFileName(LPCTSTR szPrefixString, LPCTSTR szExtension)
{
	// Compute a random temorary file name
	char szTempPath[MAX_PATH];
	::GetTempPath(sizeof(szTempPath), szTempPath);
	char szTempFileName[MAX_PATH];
	::GetTempFileName(szTempPath, (szPrefixString == NULL ? "tmp" : szPrefixString), 0, szTempFileName);

	CString strTempFileName = szTempFileName;
	if (szExtension)
	{
		strTempFileName = strTempFileName.Left(strTempFileName.ReverseFind('.')) + szExtension;
	
		// Rename the file with the desired extension
		if (!::MoveFile(szTempFileName, strTempFileName))
			strTempFileName = szTempFileName;
	}
	
	return strTempFileName;
}

/////////////////////////////////////////////////////////////////////////////
CString TempFileName(LPCTSTR szPath, LPCTSTR szPrefixString, LPCTSTR szExtension)
{
	char szTempFileName[MAX_PATH];

	*szTempFileName = 0;
	if (!szPath) 
		return szTempFileName;

	::GetTempFileName(szPath, szPrefixString, 0, szTempFileName);

	CString strTempFileName = szTempFileName;
	if (szExtension)
	{
		strTempFileName = strTempFileName.Left(strTempFileName.ReverseFind('.')+1) + szExtension;
	
		// Rename the file with the desired extension
		if (!::MoveFile(szTempFileName, strTempFileName))
			strTempFileName = szTempFileName;
	}
	
	return strTempFileName.Mid(strTempFileName.ReverseFind('\\')+1);
}

/////////////////////////////////////////////////////////////////////////////
bool ModifyStyles(HWND hWnd, DWORD dwRemove, DWORD dwAdd, bool bExtended, UINT nShowFlags)
{
	int nIndex = (bExtended ? GWL_EXSTYLE : GWL_STYLE);
	DWORD dwStyle = ::GetWindowLong(hWnd, nIndex);
	DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
	if (dwStyle == dwNewStyle)
		return false;

	::SetWindowLong(hWnd, nIndex, dwNewStyle);
	if (nShowFlags)
		::SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nShowFlags);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool IsEnhancedWinVersion()
{
	OSVERSIONINFO v;
	memset(&v, 0, sizeof(v));
	v.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!::GetVersionEx(&v))
		return false;

	if (v.dwPlatformId == VER_PLATFORM_WIN32_NT && v.dwMajorVersion >= 5)
		return true;

	if (v.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && (v.dwMajorVersion >= 4 && v.dwMinorVersion >= 90))
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool GetSpecialFolderLocation(CString& strPath, int csidl)
{
	LPITEMIDLIST pidl = NULL;
	if (FAILED(SHGetSpecialFolderLocation(NULL/*hWnd*/, csidl, &pidl)))
		return false;

	char szPath[MAX_PATH];
	bool bOK = !!SHGetPathFromIDList(pidl, szPath);
	if (bOK)
		strPath = szPath;

	LPMALLOC pMalloc = NULL;
	if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		pMalloc->Free(pidl);
		pMalloc->Release();
	}

	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
HGLOBAL ReadFileIntoMemory(LPCSTR strFileName)
{
	// Open the file
	HANDLE hFile = ::CreateFile(strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;

	// Get the file size
	DWORD dwSize = ::GetFileSize(hFile, NULL);
	if (dwSize <= 0)
	{
		::CloseHandle(hFile);
		return NULL;
	}

	// Allocate memory based on file size
	HGLOBAL hMemory = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
	if (!hMemory)
	{
		::CloseHandle(hFile);
		return NULL;
	}

	// Lock down the memory and get a pointer
	BYTE* pMemory = (BYTE*)::GlobalLock(hMemory);
	if (!pMemory)
	{
		::GlobalFree(hMemory);
		::CloseHandle(hFile);
		return NULL;
	}

	// Read the file and store it in memory
	DWORD dwBytesRead = 0;
	if (!::ReadFile(hFile, pMemory, dwSize, &dwBytesRead, NULL))
	{
		::GlobalUnlock(hMemory);
		::GlobalFree(hMemory);
		::CloseHandle(hFile);
		return NULL;
	}

	::GlobalUnlock(hMemory);
	::CloseHandle(hFile);

	// The caller must call ::GlobalFree() on the returned handle
	return hMemory;
}

/////////////////////////////////////////////////////////////////////////////
bool WriteDataToFile(LPCTSTR strFileName, LPCTSTR pMemory, DWORD dwSize)
{
	HANDLE hFile = ::CreateFile(strFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	DWORD dwBytesWritten = 0;
	bool bOK = !!::WriteFile(hFile, (LPCVOID)(LPCSTR)pMemory, dwSize, &dwBytesWritten, NULL);
	::CloseHandle(hFile);
	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
bool WriteResourceToFile(HINSTANCE hInstance, LPCTSTR szResource, LPCTSTR strResourceType, LPCTSTR strFileName)
{
	// Get the image from a resource, whatever the type
	HRSRC hResource = ::FindResource(hInstance, szResource, strResourceType);
	if (!hResource)
		return false;

	// Get the resource size
	DWORD dwSize = ::SizeofResource(hInstance, hResource);
	if (dwSize <= 0)
		return false;

	//// If the file has the correct number of bytes, we don't need to write anything
	//if (FileSize(strFileName) == dwSize)
	//	return true;

	HGLOBAL hResMemory = ::LoadResource(hInstance, hResource);
	if (!hResMemory)
		return false;

	// Lock down the resource memory and get a pointer
	char* pResMemory = (char*)::LockResource(hResMemory);
	if (!pResMemory)
	{
		::FreeResource(hResMemory);
		return false;
	}

	// Write the resource data to the named file
	bool bOK = WriteDataToFile(strFileName, pResMemory, dwSize);

	UnlockResource(hResMemory);
	::FreeResource(hResMemory);

	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
bool WriteResourceToFile(HINSTANCE hInstance, int idResource, LPCTSTR strResourceType, LPCTSTR strFileName)
{
	// Get the image from a resource, whatever the type
	HRSRC hResource = ::FindResource(hInstance, MAKEINTRESOURCE(idResource), strResourceType);
	if (!hResource)
		return false;

	// Get the resource size
	DWORD dwSize = ::SizeofResource(hInstance, hResource);
	if (dwSize <= 0)
		return false;

	// If the file has the correct number of bytes, we don't need to write anything
	if (FileSize(strFileName) == dwSize)
		return true;

	HGLOBAL hResMemory = ::LoadResource(hInstance, hResource);
	if (!hResMemory)
		return false;

	// Lock down the resource memory and get a pointer
	char* pResMemory = (char*)::LockResource(hResMemory);
	if (!pResMemory)
	{
		::FreeResource(hResMemory);
		return false;
	}

	// Write the resource data to the named file
	bool bOK = WriteDataToFile(strFileName, pResMemory, dwSize);

	UnlockResource(hResMemory);
	::FreeResource(hResMemory);

	return bOK;
}

static LPCTSTR g_pClassName			=	NULL;
static LPCTSTR g_pWindowName		=	NULL;
static LPCTSTR g_pParentClassFilter	=	NULL;

/////////////////////////////////////////////////////////////////////////////
static BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lParam)
{
	HWND hWndFound = ::FindWindowEx(hWnd, NULL, g_pClassName, g_pWindowName);
	if (hWndFound)
	{
		if (g_pParentClassFilter)
		{
			HWND hWndParent = ::GetParent(hWndFound); // immediate parent
			if (hWndParent)
			{
				TCHAR szParentClass[40];
				::GetClassName(hWndParent, szParentClass, sizeof(szParentClass) - 1);
				if (_tcsstr(szParentClass, g_pParentClassFilter))
				{
					*(HWND*)lParam = hWndFound;
					return false; // don't continue
				}
			}
		}
		else
		{
			*(HWND*)lParam = hWndFound;
			return false; // don't continue
		}
	}

	::EnumChildWindows(hWnd, EnumWindowProc, lParam);
	bool bContinue = (*(HWND*)lParam == NULL);
	return bContinue; // continue? yes or no
}

/////////////////////////////////////////////////////////////////////////////
HWND GetChildWindow(HWND hWndParent, LPCTSTR pClassName, LPCTSTR pWindowName, LPCTSTR pParentClassFilter)
{
	g_pClassName = pClassName;
	g_pWindowName = pWindowName;
	g_pParentClassFilter = pParentClassFilter;
	HWND hWndChild = NULL;
	EnumWindowProc(hWndParent, (LPARAM)&hWndChild);
	
	// Reset globals now that we are done.
	g_pClassName = NULL;
	g_pWindowName = NULL;
	g_pParentClassFilter = NULL;

	return hWndChild;
}

/////////////////////////////////////////////////////////////////////////////
void GetRichTextWindow(HWND hwndParent, HWND& hwndRichEdit)
{
	if (!hwndParent)
		return;

	// Find RichEdit20W (Outlook 2002/2003) child window, if not found then search for RichEdit20A (Outlook 2000).
	HWND hWndChild = GetChildWindow(hwndParent, _T("RichEdit20W"), NULL, _T("AfxWnd"));
	if (!hWndChild)
		hWndChild = GetChildWindow(hwndParent, _T("RichEdit20A"), NULL, _T("AfxWnd"));

	hwndRichEdit = hWndChild;
}

/////////////////////////////////////////////////////////////////////////////
void GetIEServerWindow(HWND hwndParent, HWND& hwndIEServer, bool bCheckParent)
{
	if (!hwndParent)
		return;

	// Check if Parent is Internet Explorer_Server
	TCHAR szClass[100];
	::GetClassName(hwndParent, szClass, sizeof(szClass) - 1);
	if (bCheckParent && !_tcscmp(szClass, _T("Internet Explorer_Server")))
	{
		hwndIEServer = hwndParent;
		return;
	}

	// Find Internet Explorer_Server child window.
	HWND hWndChild = GetChildWindow(hwndParent, _T("Internet Explorer_Server"));
	if (hWndChild)
		hwndIEServer = hWndChild;
}
/////////////////////////////////////////////////////////////////////////////
bool GetAppPath(CString& strFileName)
{
	CString strKeyPath = "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\";
	strKeyPath += strFileName;
	CRegKey regkey;
	if (regkey.Open(HKEY_LOCAL_MACHINE, strKeyPath) != ERROR_SUCCESS)
		return false;

	char strValue[MAX_PATH];
	strValue[0] = '\0';
	DWORD dwLength = MAX_PATH;
	if (regkey.QueryStringValue(NULL/*Default*/, strValue, &dwLength) != ERROR_SUCCESS)
		return false;

	strFileName = strValue;
	if (strFileName.Find('%') >= 0)
	{
		char strDstPath[MAX_PATH];
		strDstPath[0] = '\0';
		if (::ExpandEnvironmentStrings(strFileName, strDstPath, MAX_PATH) > 0)
			strFileName = strDstPath;
	}
 
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool RegKeyValueExists(LPCTSTR pKeyName, LPCTSTR pValueName)
{
	CRegKey regkey;
	if (regkey.Open(HKEY_CURRENT_USER, pKeyName) != ERROR_SUCCESS)
		return false;

	DWORD dwValue = 0;
	if (regkey.QueryDWORDValue(pValueName, dwValue) != ERROR_SUCCESS)
		return false;
		
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void WriteShortcut(LPCSTR strShortcutName, LPCSTR strTarget, LPCSTR strArguments, LPCSTR strIconFile, int iIconIndex, bool bCreate, int csidl)
{
	CString strShortcutPath;
	GetSpecialFolderLocation(strShortcutPath, csidl);
	strShortcutPath = strShortcutPath + "\\" + strShortcutName + ".lnk";
	if (!bCreate && !FileExists(strShortcutPath))
		return;

	CComPtr<IShellLink> psl;
	HRESULT hr = psl.CoCreateInstance(CLSID_ShellLink);
	if (FAILED(hr))
		return;
	
	CComQIPtr<IPersistFile> ppf;
	ppf = psl;
	if (ppf == NULL)
		return;

	CString strWorkFolder = "";

	hr = psl->SetPath(strTarget);
	hr = psl->SetArguments(strArguments);
	hr = psl->SetWorkingDirectory(strWorkFolder);
	hr = psl->SetIconLocation(strIconFile, iIconIndex);
	hr = psl->SetShowCmd(SW_SHOWNORMAL);
	hr = ppf->Save(CComBSTR(strShortcutPath), true);
}

/////////////////////////////////////////////////////////////////////////////
void WriteInternetShortcut(LPCSTR strShortcutName, LPCSTR strTarget, LPCSTR strIconFile, int iIconIndex, bool bCreate, int csidl)
{
	CString strShortcutPath;
	GetSpecialFolderLocation(strShortcutPath, csidl);
	strShortcutPath = strShortcutPath + "\\" + strShortcutName + ".url";
	if (!bCreate && !FileExists(strShortcutPath))
		return;

	CComPtr<IUniformResourceLocator> pURL;
//j	HRESULT hr = pURL.CoCreateInstance(CLSID_InternetShortcut);
	HRESULT hr = ::CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER, IID_IUniformResourceLocator, (void**)&pURL);
	if (FAILED(hr))
		return;

	CComQIPtr<IShellLink> psl;
	psl = pURL;
	if (psl == NULL)
		return;

	CComQIPtr<IPersistFile> ppf;
	ppf = pURL;
	if (ppf == NULL)
		return;

	hr = pURL->SetURL(strTarget, IURL_SETURL_FL_GUESS_PROTOCOL);
	hr = psl->SetIconLocation(strIconFile, iIconIndex);
	hr = psl->SetShowCmd(SW_SHOWNORMAL);
	hr = ppf->Save(CComBSTR(strShortcutPath), true);
}

/////////////////////////////////////////////////////////////////////////////
void WriteInternetShortcut2(LPCSTR strShortcutName, LPCSTR strTarget, LPCSTR strIconFile, int iIconIndex, bool bCreate, int csidl)
{
	CString strTargetPath = "iexplore.exe";
	if (!GetAppPath(strTargetPath))
		return;

	CString strArguments = CString("-nohome ") + strTarget;
	WriteShortcut(strShortcutName, strTargetPath, strArguments, strIconFile, iIconIndex, bCreate, csidl);
}

/////////////////////////////////////////////////////////////////////////////
void CleanFileName(CString& strFileName)
{
	CString strFileNameIn = strFileName;

	// Trim white space
	strFileNameIn.Trim();

	// Remove trailing periods.
	int i;
	int iNewLen = strFileNameIn.GetLength();
	while ((i=strFileNameIn.ReverseFind('.')) > -1 && i == iNewLen-1)
		iNewLen = strFileNameIn.Delete(i,1);

	LPCTSTR pFileName = strFileNameIn;
	DWORD dwLength = strFileNameIn.GetLength() + 1;
	char* pFileNameOut = new char[dwLength];
	
	// Run thru each character and replace invalid characters
	// with space. FILENAME_MAX ensures name does not exceed legal
	// size.
	int len = 0;
	char cAlpha;
	bool bDone = false;
	while (!bDone && len < FILENAME_MAX)
	{
		cAlpha = pFileName[len];

		// The upper characters can be passed with a single check and
		// since only the backslash and bar are above the ampersand
		// it saves memory to do the check this way with little performance
		// cost.
		if (pFileName[len] >= '@')
		{
			if (pFileName[len] == '\\' || pFileName[len] == '|')
				cAlpha = ' ';
		}

		else if (pFileName[len] == '\0') // if end of string w're done
		{
			cAlpha = '\0';
			bDone = true;
		}
		else if (pFileName[len] < ' ')
		{
			cAlpha = ' ';
		}
		else
		{
			static bool isCharValid[32] =
			{
			//  ' '   !     "      #     $     %     &     '     (     )     *      +     ,      -     .      / 
				true, true, false, true, true, true, true, true, true, true, false, true, true,  true, true,  false,
			//  0     1     2      3     4     5     6     7     8     9     :      ;     <      =     >      ?
				true, true, true,  true, true, true, true, true, true, true, false, true, false, true, false, false
			//  0     1     2      3     4     5     6     7     8     9     :      ;     <      =     >      ?
			};

			// This is faster, at the expense of memory, than checking each
			// invalid character individually. However, either method is much
			// faster than using strchr().
			if (pFileName[len] >= 32)
			{
				if (!isCharValid[pFileName[len] - 32])
					cAlpha = ' ';
			}
		}

		// Update out going filename
		pFileNameOut[len] = cAlpha;
		len++;
	}

	strFileName = pFileNameOut;
	strFileName.Trim();
	delete [] pFileNameOut;
	return;
}

/////////////////////////////////////////////////////////////////////////////
void GetError(LPCTSTR lpszFunction) 
{ 
#if defined _DEBUG

	DWORD dwError = ::GetLastError(); 
	CString strMsg;
    

	LPVOID lpMsgBuf;
	if (!FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL ))
	{
	// Handle the error.
	return;
	}

	strMsg.Format(_T("%s failed: GetLastError returned <%d>\n %s"), lpszFunction, dwError, (LPCTSTR)lpMsgBuf); 

	// Display the string.
	MessageBox( NULL, strMsg, "Error", MB_OK | MB_ICONINFORMATION );

	// Free the buffer.
	LocalFree( lpMsgBuf );

#endif
}

/////////////////////////////////////////////////////////////////////////////
bool MenuItemExists(HMENU hMenu, UINT idMenu)
{
	if (!::IsMenu(hMenu))
		return false;

	return (::EnableMenuItem(hMenu, idMenu, MF_BYCOMMAND | MF_ENABLED) != -1);
}

//////////////////////////////////////////////////////////////////////
double ScaleToFit(LPINT DestWidth, LPINT DestHeight, int SrcWidth, int SrcHeight, bool bUseSmallerFactor)
{
	// Scale the source to fit the destination...
	double scale1 = (double)*DestWidth / SrcWidth;
	double scale2 = (double)*DestHeight / SrcHeight;
	double rate = (bUseSmallerFactor ? min(scale1, scale2) : max(scale1, scale2));

	// Adjust the desination size, and return the scale factor
	*DestHeight = (int)((double)SrcHeight * rate);
	*DestWidth	= (int)((double)SrcWidth * rate);
	return rate;
}

//////////////////////////////////////////////////////////////////////
bool CStringFromCLSID(CLSID clsid, CString& szClsid)
{
	USES_CONVERSION;
	HRESULT	 hr;	
    CString  szCLSID;
    LPOLESTR pwsz=NULL;

    // Get the CLSID in string form.
    hr = StringFromCLSID(clsid, &pwsz);
	if (FAILED(hr))
		return false;

	if (pwsz == NULL)
		return false;


	szClsid = OLE2T(pwsz);

    // Free the string.
    CoTaskMemFree(pwsz);
	pwsz = NULL;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
HBITMAP ReplaceBitmapColor(HBITMAP hBitmap, COLORREF clrFind, COLORREF clrReplace, COLORREF clrReplaceOthers)
{ // Pass in CLR_DEFAULT for clrFind in order to replace the color of the top-left pixel
	if (!hBitmap)
		return NULL;

	HDC hSrcMemDC = ::CreateCompatibleDC(NULL);
	if (!hSrcMemDC)
		return NULL;

	HDC hDstMemDC = ::CreateCompatibleDC(NULL);
	if (!hDstMemDC)
		return NULL;

	HGDIOBJ hOldSrcBitmap = ::SelectObject(hSrcMemDC, hBitmap);
		
	BITMAP bm;
	::GetObject(hBitmap, sizeof(bm), &bm);
			
	// Create a BITMAPINFO for the CreateDIBSection
	BITMAPINFO Dib; 
	::ZeroMemory(&Dib, sizeof(BITMAPINFO));
	Dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Dib.bmiHeader.biWidth = bm.bmWidth;
	Dib.bmiHeader.biHeight = bm.bmHeight;
	Dib.bmiHeader.biPlanes = 1;
	Dib.bmiHeader.biBitCount = 24;

	BYTE* pPixels = NULL;
	HBITMAP hNewBitmap = ::CreateDIBSection(hDstMemDC, &Dib, DIB_RGB_COLORS, (void **)&pPixels, NULL, 0);
	if (!pPixels)
		hNewBitmap = NULL;
	if (hNewBitmap)
	{
		HGDIOBJ hOldDstBitmap = ::SelectObject(hDstMemDC, hNewBitmap);
		::BitBlt(hDstMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hSrcMemDC, 0, 0, SRCCOPY);

		BYTE r_Replace = GetRValue(clrReplace);
		BYTE g_Replace = GetGValue(clrReplace);
		BYTE b_Replace = GetBValue(clrReplace);

		bool bReplaceOthers = (clrReplaceOthers != CLR_NONE);
		BYTE r_ReplaceOthers = GetRValue(clrReplaceOthers);
		BYTE g_ReplaceOthers = GetGValue(clrReplaceOthers);
		BYTE b_ReplaceOthers = GetBValue(clrReplaceOthers);

		int iCount = bm.bmWidth * bm.bmHeight;
		while (--iCount >= 0)
		{
			BYTE* p = pPixels;
			BYTE b = *p++;
			BYTE g = *p++;
			BYTE r = *p++;
			
			// See if we should use the top left pixel as the source color
			if (clrFind == CLR_DEFAULT)
				clrFind = RGB(r,g,b);

			if (clrFind == RGB(r,g,b))
			{
				*pPixels++ = b_Replace;
				*pPixels++ = g_Replace;
				*pPixels++ = r_Replace;
			}
			else 
			if (bReplaceOthers)
			{
				*pPixels++ = b_ReplaceOthers;
				*pPixels++ = g_ReplaceOthers;
				*pPixels++ = r_ReplaceOthers;
			}
			else
				pPixels += 3;
		}

		::SelectObject(hDstMemDC, hOldDstBitmap);
	}

	::DeleteDC(hDstMemDC);

	::SelectObject(hSrcMemDC, hOldSrcBitmap);
	::DeleteDC(hSrcMemDC);

	return hNewBitmap;
}
///////////////////////////////////////////////////////////////////////////////
bool CreateDir(LPCTSTR szDir)
{
	CString szTemp(szDir);
	if (szTemp.GetLength() > MAX_PATH)
		return false;

	TCHAR* szPath = szTemp.GetBuffer();
	TCHAR szDirName[MAX_PATH];
	TCHAR* p = szPath;
	TCHAR* q = szDirName;	

	while(*p)
	{
		if ((_T('\\') == *p) || (_T('/') == *p))
		{
			if (_T(':') != *(p-1))
			{
				CreateDirectory(szDirName, NULL);
			}
		}
		*q++ = *p++;
		*q = _T('\0');
	}
	
	CreateDirectory(szDirName, NULL);

	return true; 
}
///////////////////////////////////////////////////////////////////////////////
bool IsDots(LPCTSTR str) 
{
    if(_tcscmp(str,".") && _tcscmp(str,"..")) 
		return false;
    
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool DeleteDirectory(LPCTSTR szPath) 
{
    HANDLE hFind;  // file handle
    WIN32_FIND_DATA FindFileData;

    TCHAR DirPath[MAX_PATH];
    TCHAR FileName[MAX_PATH];

    _tcscpy_s(DirPath,szPath);
    _tcscat_s(DirPath, _countof(DirPath), _T("\\*"));    // searching all files
    _tcscpy_s(FileName, szPath);
    _tcscat_s(FileName, _countof(FileName), _T("\\"));

    hFind = FindFirstFile(DirPath,&FindFileData); // find the first file
    if(hFind == INVALID_HANDLE_VALUE) return FALSE;
    _tcscpy_s(DirPath, FileName);
        
    bool bSearch = true;
    while(bSearch) { // until we finds an entry
        if(FindNextFile(hFind,&FindFileData)) {
			if(IsDots(FindFileData.cFileName)) continue;
            _tcscat_s(FileName, _countof(FileName), FindFileData.cFileName);
            if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

                // we have found a directory, recurse
                if(!DeleteDirectory(FileName)) { 
                    FindClose(hFind); 
                    return false; // directory couldn't be deleted
                }
                RemoveDirectory(FileName); // remove the empty directory
                _tcscpy_s(FileName,DirPath);
            }
            else {
                if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
                    _chmod(FileName, _S_IWRITE); // change read-only file mode
                if(!DeleteFile(FileName)) {  // delete the file
                    FindClose(hFind); 
                    return false; 
                }                 
                _tcscpy_s(FileName,DirPath);
            }
        }
        else {
            if(GetLastError() == ERROR_NO_MORE_FILES) // no more files there
            bSearch = false;
            else {
                // some error occured, close the handle and return FALSE
                FindClose(hFind); 
                return false;
            }

        }

    }

    FindClose(hFind);  // closing file handle
 
	// remove the empty directory
	if (!RemoveDirectory(szPath))
		return false;

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool SaveXML(LPCTSTR szDestPath, LPCTSTR szXML)
{
	if (CString(szXML).IsEmpty() || CString(szDestPath).IsEmpty())
		return false;

	HRESULT hr;
	CComPtr<IXMLDOMDocument> spXMLDoc;
	hr = spXMLDoc.CoCreateInstance(MSXML2::CLSID_DOMDocument);
	if (FAILED(hr))
		return false;

	VARIANT_BOOL bSuccess = false;
	hr = spXMLDoc->loadXML(CComBSTR(szXML), &bSuccess);
	if (!bSuccess)
		return false;

	// Create Directory
	CString szDirPath = StrPath(szDestPath);
	CreateDir(szDirPath);

	CComVariant varDestPath(szDestPath);
	hr = spXMLDoc->save(varDestPath);
	if (FAILED(hr))
		return false;
	
	return true;
}
///////////////////////////////////////////////////////////////////////////////
CString GetTimeStamp(LPCTSTR szFormatString)
{
	CTime t = CTime::GetCurrentTime();
	return t.Format(szFormatString);
}
///////////////////////////////////////////////////////////////////////////////
bool Wait(DWORD dwExpire)
{
	bool bExpired = false; 
	DWORD dwMaxTime = ::timeGetTime() + dwExpire;
	while (!bExpired)
	{
		MSG msg;
		BOOL bRet = ::GetMessage(&msg, NULL, 0, 0);
		if (!bRet)
			return false; // WM_QUIT, exit message loop

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (::timeGetTime() > dwMaxTime)
			bExpired = true;

		Sleep(200);
	}

	return true;//(!(BOOL)bBusy);
}
/////////////////////////////////////////////////////////////////////////////
CString GetNamePairValue(LPCTSTR szOptionString, LPCTSTR szNameTarget, TCHAR szDelimiter)
{
	CString szOptions = szOptionString;
	if (szOptions.IsEmpty())
		return CString("");

	if (szOptions.Find(_T('=')) < 0)
		return CString("");

	szOptions += szDelimiter;

	int iStart = 0;
	while (iStart >= 0 )
	{
		int iEnd = szOptions.Find(szDelimiter, iStart);
		if (iEnd <= iStart)
			break;
		int iPair = szOptions.Find(_T('='), iStart);
		if (iPair > iStart && iPair < iEnd)
		{
			CString szName = szOptions.Mid(iStart, iPair-iStart);
			szName.TrimLeft();
			szName.TrimRight();
			szName.MakeLower();
			if (szName.CompareNoCase(szNameTarget) == 0)
			{
				iPair++;
				CString szValue = szOptions.Mid(iPair, iEnd-iPair);
				szValue.TrimLeft();
				szValue.TrimRight();
				UnescapeUrl(szValue, szValue);
				return szValue;
			}
		}

		iStart = iEnd + 1;
	}

	return CString("");
}
/////////////////////////////////////////////////////////////////////////////////
// DrawTransparentBitmap():
// The first step is to obtain the contents of the area where the bitmap will 
// be drawn and to store this background image in a memory display context (DC). 
// Mask out the area of the background that corresponds to the nontransparent 
// portion of the image bitmap and mask out all transparent pixels from the 
// image bitmap. Use the XOR raster operation to merge the image bitmap into 
// the background bitmap. Finally, use the BitBlt function to move the merged 
// image to the destination DC. 
void DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, int xStart, int yStart, COLORREF cTransparentColor)
{
	BITMAP     bm;
	COLORREF   cColor;
	HBITMAP    bmAndBack, bmAndObject, bmAndMem, bmSave;
	HGDIOBJ    hBackOld, hObjectOld, hMemOld, hSaveOld;
	HDC        hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT      ptSize;

	hdcTemp = CreateCompatibleDC(hdc);
	SelectObject(hdcTemp, hBitmap);   // Select the bitmap

	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
	ptSize.x = bm.bmWidth;            // Get width of bitmap
	ptSize.y = bm.bmHeight;           // Get height of bitmap
	DPtoLP(hdcTemp, &ptSize, 1);      // Convert from device
									  // to logical points

	// Create some DCs to hold temporary data.
	hdcBack   = CreateCompatibleDC(hdc);
	hdcObject = CreateCompatibleDC(hdc);
	hdcMem    = CreateCompatibleDC(hdc);
	hdcSave   = CreateCompatibleDC(hdc);

	// Create a bitmap for each DC. DCs are required for a number of
	// GDI functions.

	// Monochrome DC
	bmAndBack   = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

	// Monochrome DC
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

	bmAndMem    = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	bmSave      = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

	// Each DC must select a bitmap object to store pixel data.
	hBackOld   = ::SelectObject(hdcBack, bmAndBack);
	hObjectOld = ::SelectObject(hdcObject, bmAndObject);
	hMemOld    = ::SelectObject(hdcMem, bmAndMem);
	hSaveOld   = ::SelectObject(hdcSave, bmSave);

	// Set proper mapping mode.
	SetMapMode(hdcTemp, GetMapMode(hdc));

	// Save the bitmap sent here, because it will be overwritten.
	BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	// Set the background color of the source DC to the color.
	// contained in the parts of the bitmap that should be transparent
	cColor = SetBkColor(hdcTemp, cTransparentColor);

	// Create the object mask for the bitmap by performing a BitBlt
	// from the source bitmap to a monochrome bitmap.
	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0,
		  SRCCOPY);

	// Set the background color of the source DC back to the original
	// color.
	SetBkColor(hdcTemp, cColor);

	// Create the inverse of the object mask.
	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0,
		  NOTSRCCOPY);

	// Copy the background of the main DC to the destination.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, xStart, yStart,
		  SRCCOPY);

	// Mask out the places where the bitmap will be placed.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);

	// Mask out the transparent colored pixels on the bitmap.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);

	// XOR the bitmap with the background on the destination DC.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);

	// Copy the destination to the screen.
	BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, 0, 0,
		  SRCCOPY);

	// Place the original bitmap back into the bitmap sent here.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

	// Delete the memory bitmaps.
	::SelectObject(hdcBack, hBackOld);
	::SelectObject(hdcObject, hObjectOld);
	::SelectObject(hdcMem, hMemOld);
	::SelectObject(hdcSave, hSaveOld);

	::DeleteObject(hBackOld);
	::DeleteObject(hObjectOld);
	::DeleteObject(hMemOld);
	::DeleteObject(hSaveOld);

	// Delete the memory DCs.
	::DeleteDC(hdcMem);
	::DeleteDC(hdcBack);
	::DeleteDC(hdcObject);
	::DeleteDC(hdcSave);
	::DeleteDC(hdcTemp);
}
