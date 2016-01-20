#pragma once

// Because this will be linked into an ATL library, force the "consumer" to
// address CString's as CAtlString's so we can use the library in an MFC app
#ifdef _AFX
	#include <atlstr.h>
	#define CStringW ATL::CAtlStringW
	#define CStringA ATL::CAtlStringA
	#define CString ATL::CAtlString
#endif

#define SWAP(a,b)		((a) ^= (b), (b) ^= (a), (a) ^= (b))
#define WIDTH(r)		((r).right - (r).left)
#define HEIGHT(r)		((r).bottom - (r).top)
#define TWIPS_PER_INCH	1440
#define APP_RESOLUTION	TWIPS_PER_INCH

bool AllocateHeap(DWORD aptr, LPCSTR lpszString);
bool AppendStringToHeap(DWORD aptr, LPCSTR lpszString);
void CleanDirectory(CString strDirectory);
bool CreateDir(LPCTSTR szDir);
bool DeleteDirectory(LPCTSTR szPath); 
bool IsDots(LPCTSTR str);
bool SaveXML(LPCTSTR szDestPath, LPCTSTR szXML);
bool Wait(DWORD dwExpire);
CString GetNamePairValue(LPCTSTR szOptionString, LPCTSTR szNameTarget, TCHAR szDelimiter=_T('|'));
CString GetTimeStamp(LPCTSTR szFormatString);
CString Get4YourSoulPath();
CString TempFileName(LPCTSTR szPath, LPCTSTR szPrefixString, LPCTSTR szExtension);
CString String(LPCSTR pszFormat, ...);
CString String(UINT nFormatID, ...);
CString StrPath(LPCTSTR szFilePath);
CString StrFileName(LPCTSTR szFilePath);
CString StrExtension(LPCTSTR szFilePath);
CString StripExtension(LPCTSTR szFilePath);
CString HRString(HRESULT hr);
int Replace(CString& strDest, const CString& strFind, const CString& strReplace);
CString dtoa(double fValue, int nDecimalDigits);
CString RandomFileName(LPCTSTR szPrefixString = NULL, LPCTSTR szExtension = NULL);
bool ModifyStyles(HWND hWnd, DWORD dwRemove, DWORD dwAdd, bool bExtended, UINT nShowFlags = 0);
bool IsEnhancedWinVersion();
bool GetSpecialFolderLocation(CString& strPath, int csidl);
HGLOBAL ReadFileIntoMemory(LPCSTR strFileName);
bool WriteDataToFile(LPCTSTR strFileName, LPCTSTR pMemory, DWORD dwSize);
bool WriteResourceToFile(HINSTANCE hInstance, LPCTSTR szResource, LPCTSTR strResourceType, LPCTSTR strFileName);
bool WriteResourceToFile(HINSTANCE hInstance, int idResource, LPCTSTR strResourceType, LPCTSTR strFileName);
HWND GetChildWindow(HWND hwndParent, LPCTSTR pClassName, LPCTSTR pWindowName = NULL, LPCTSTR pParentClassFilter = NULL);
void GetRichTextWindow(HWND hwndParent, HWND& hwndIEServer);
void GetIEServerWindow(HWND hwndParent, HWND& hwndIEServer, bool bCheckParent=true);
bool GetAppPath(CString& strFileName);
bool RegKeyValueExists(LPCTSTR pKeyName, LPCTSTR pValueName);
void WriteShortcut(LPCSTR strShortcutName, LPCSTR strTarget, LPCSTR strArguments, LPCSTR strIconFile, int iIconIndex, bool bCreate, int csidl);
void WriteInternetShortcut(LPCSTR strShortcutName, LPCSTR strTarget, LPCSTR strIconFile, int iIconIndex, bool bCreate, int csidl);
void WriteInternetShortcut2(LPCSTR strShortcutName, LPCSTR strTarget, LPCSTR strIconFile, int iIconIndex, bool bCreate, int csidl);
void CleanFileName(CString& strFileName);
void GetError(LPCTSTR lpszFunction); 
bool MenuItemExists(HMENU hMenu, UINT idMenu);
double ScaleToFit(LPINT DestWidth, LPINT DestHeight, int SrcWidth, int SrcHeight, bool bUseSmallerFactor);
bool CStringFromCLSID(CLSID clsid, CString &szClsid);
HBITMAP ReplaceBitmapColor(HBITMAP hBitmap, COLORREF clrFind, COLORREF clrReplace, COLORREF clrReplaceOthers = CLR_NONE);
void DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, int xStart, int yStart, COLORREF cTransparentColor);

//////////////////////////////////////////////////////////////////////
inline int dtoi(double x)
{
	return ((int)(x >= 0 ? x + 0.5 : x - 0.5));
}

static const double InchesPerMM = 0.03937;

//////////////////////////////////////////////////////////////////////
inline double MM2Inches(double fMM)
{
	return fMM * InchesPerMM;
}

//////////////////////////////////////////////////////////////////////
inline double Inches2MM(double fInches)
{
	return fInches / InchesPerMM;
}

//////////////////////////////////////////////////////////////////////
inline int INCHES(int numer, int denom)
{
	if (!denom)
		return 0;
	return dtoi(((double)numer * APP_RESOLUTION) / (double)denom);
}

//////////////////////////////////////////////////////////////////////
inline int INCHES(double fInches)
{
	return dtoi(fInches * APP_RESOLUTION);
}

//////////////////////////////////////////////////////////////////////
inline double DINCHES(int iUnits)
{
	return (double)iUnits / APP_RESOLUTION;
}

//////////////////////////////////////////////////////////////////////
inline int POINTUNITS(double fPointSize)
{
	return dtoi(fPointSize * (APP_RESOLUTION / 72));
}

//////////////////////////////////////////////////////////////////////
inline double POINTSIZE(int iPointUnits)
{
	return (double)abs(iPointUnits) / (APP_RESOLUTION / 72);
}

//////////////////////////////////////////////////////////////////////
inline int bound(int n, int nLower, int nUpper)
{
	if (n <= nLower)
		return nLower;
	if (n >= nUpper)
		return nUpper;
	return n;
}

/////////////////////////////////////////////////////////////////////////////
inline bool FileExists(LPCTSTR pFileName)
{
	return (::GetFileAttributes(pFileName) != 0xFFFFFFFF);
}

/////////////////////////////////////////////////////////////////////////////
inline DWORD FileSize(LPCTSTR pFileName)
{
	// Open the file
	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	// Get the file size
	DWORD dwSize = ::GetFileSize(hFile, NULL);
	::CloseHandle(hFile);

	return dwSize;
}

/////////////////////////////////////////////////////////////////////////////
// Integer square root
inline long lsqrt(long n)
{
	if (n < 1)
		return 0;

	// Load the binary constant 01 00 00 ... 00, where the number of zero bits to the right of the
	// single one bit is even, and the one bit is as far left as is consistant with that condition
	long lSquaredBit = 0x40000000;
	long lRemainder = n;
	long lRoot = 0;
	while (lSquaredBit > 0)
	{
		if (lRemainder >= (lSquaredBit | lRoot))
		{
			 lRemainder -= (lSquaredBit | lRoot);
			 lRoot >>= 1;
			 lRoot |= lSquaredBit;
		}
		else
			lRoot >>= 1;

		lSquaredBit >>= 2; 
	}

	return lRoot;
}

#ifdef _AFX
	#undef CStringW
	#undef CStringA
	#undef CString
#endif
