//==========================================================================//
//==========================================================================//
#include "stdafx.h"
#include "font.h"
#include "resource.h"
#include "zutil.h"


#ifdef _AFX
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] =__FILE__;
#define new DEBUG_NEW
#endif
#endif

#define DEFAULT_FONT		"CAC Futura Casual"
#define DEFAULT_FONTFILE	"CACFC___.TTF"
#define FONT_SUBKEY 		"Software\\Microsoft\\Windows\\CurrentVersion\\Fonts"
#define NTFONT_SUBKEY		"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"

typedef struct
{
	const char	*pszFont;
	const char	*pszFullName;
	const char	*pszFontFile;
} FONTNAMES;


const static FONTNAMES FontNames[] =
{
	"CAC Camelot",					"CAC Camelot",					"CACCAMEL.TTZ",
	"CAC Champagne",				"CAC Champagne",				"CACCHAMP.TTZ",
	"CAC Futura Casual",			"CAC Futura Casual",			"CACFC___.TTZ",
	"CAC Futura Casual Bold",		"CAC Futura Casual Bold",		"CACFCB__.TTZ",
	"CAC Futura Casual Bold Italic","CAC Futura Casual Bold Italic","CACFCBI_.TTZ",
	"CAC Futura Casual Med. Italic","CAC Futura Casual Med. Italic","CACFCMI_.TTZ",
	"CAC Krazy Legs",				"CAC Krazy Legs",				"CACKL___.TTZ",
	"CAC Krazy Legs Bold",			"CAC Krazy Legs Bold",			"CACKLB__.TTZ",
	"CAC Lasko Condensed",			"CAC Lasko Condensed",			"CACLC___.TTZ",
	"CAC Lasko Even Weight",		"CAC Lasko Even Weight",		"CACLEW__.TTZ",
	"CAC Leslie",					"CAC Leslie",					"CACLESLI.TTZ",
	"CAC Logo Alternate",			"CAC Logo Alternate",			"CACLA___.TTZ",
	"CAC Moose",					"CAC Moose",					"CACMOOSE.TTZ",
	"CAC Norm Heavy",				"CAC Norm Heavy",				"CACNH___.TTZ",
	"CAC One Seventy",				"CAC One Seventy",				"CACOS___.TTZ",
	"CAC Pinafore", 				"CAC Pinafore", 				"CACPINAF.TTZ",
	"CAC Saxon Bold",				"CAC Saxon Bold",				"CACSB___.TTZ",
	"CAC Shishoni Brush",			"CAC Shishoni Brush",			"CACSHISH.TTZ",
	"CAC Valiant",					"CAC Valiant",					"CACVALIA.TTZ",
	"Cataneo BT",					"Cataneo BT",					"TT0952M_.TTZ",
	"DomCasual BT", 				"Dom Casual BT",				"TT0604M_.TTZ",
	"Freehand575 BT",				"Freehand 575 BT",				"TT1046M_.TTZ",
	"GoudyOlSt BT(Italic)", 		"Goudy Old Style Italic BT",	"TT0108M_.TTZ",
	"Informal011 BT",				"Informal 011 BT",				"TT1115M_.TTZ",
	"Lydian Csv BT",				"Lydian Cursive BT",			"TT0845M_.TTZ",
	"MattAntique BT",				"Matt Antique BT",				"TT1014M_.TTZ",
	"MattAntique BT(Italic)",		"Matt Antique Italic BT",		"TT1015M_.TTZ",
	"Snell BT", 					"Snell BT", 					"TT0196M_.TTZ",
	"Sprocket BT",					"Sprocket BT",					"TT1244M_.TTZ",
	"Swis721 BT(Bold)", 			"Swiss 721 Bold BT",			"TT0005M_.TTZ",
	"Swis721 Md BT",				"Swiss 721 Medium BT",			"TT0759M_.TTZ",
	NULL,							NULL,							NULL
};

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
static int CALLBACK EnumFontFamilyProc2(ENUMLOGFONT* pelf, NEWTEXTMETRIC* pntm, int FontType, LPARAM lParam)
{
	if (FontType & TRUETYPE_FONTTYPE)
	{
		CFontList* pFontList = (CFontList*)lParam;
		FONTINFO fi;
		fi.lf = pelf->elfLogFont;
		lstrcpy(fi.szFullName, (const char*)pelf->elfFullName);
		pFontList->GetFontArray().push_back(fi);
	}

	return (1);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
static int CALLBACK EnumFontFamilyProc(ENUMLOGFONT* pelf, NEWTEXTMETRIC* pntm, int FontType, LPARAM lParam)
{
	if (FontType & TRUETYPE_FONTTYPE)
	{
		CFontList* pFontList = (CFontList*)lParam;
		EnumFontFamilies(pFontList->GetEnumFontDC(), pelf->elfLogFont.lfFaceName, (FONTENUMPROC)EnumFontFamilyProc2, lParam);
	}

	return (1);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CFontList::CFontList()
{
	m_EnumFontDC = NULL;
	InitFontArray();
	CheckDefaultFont();
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CFontList::InitFontArray()
{
	m_FontArray.clear();

	m_EnumFontDC = ::CreateIC("DISPLAY", NULL, NULL, NULL);
	EnumFontFamilies(m_EnumFontDC, NULL, (FONTENUMPROC)EnumFontFamilyProc, (LPARAM)this);
	::DeleteDC(m_EnumFontDC);
	m_EnumFontDC = NULL;
}	

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CFontList::CheckDefaultFont()
{
	int nFonts = m_FontArray.size();
	for (int i = 0; i < nFonts; i++)
	{
		if (!lstrcmp(m_FontArray[i].lf.lfFaceName, DEFAULT_FONT))
			break;
	}

	if (i >= nFonts)
	{
		HGLOBAL hResFont = NULL;
		HRSRC	hResource;

		if ((hResource = ::FindResource(_Module.GetResourceInstance(),
		  MAKEINTRESOURCE(IDR_CACFC), "TTZ")) != NULL)
		{
			hResFont = ::LoadResource(_Module.GetResourceInstance(), hResource);
		}

		if (hResFont)
		{
			BYTE* pCompressedBytes = (BYTE*)::LockResource(hResFont);
			DWORD dwCompressedLen = ::SizeofResource(_Module.GetResourceInstance(), hResource);
			InstallFont(pCompressedBytes, dwCompressedLen, DEFAULT_FONT, DEFAULT_FONTFILE);
		}
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CFontList::CheckFonts(LOGFONTARRAY& lfArray, FONTDOWNLOADARRAY& DownloadArray)
{
	int nCheckFonts = lfArray.size();
	for (int i = 0; i < nCheckFonts; i++)
	{
		int nFonts = m_FontArray.size();
		for (int j = 0; j < nFonts; j++)
		{
			if (!lstrcmp(lfArray[i].lfFaceName, m_FontArray[j].lf.lfFaceName) &&
			  lfArray[i].lfWeight == m_FontArray[j].lf.lfWeight &&
			  lfArray[i].lfItalic == m_FontArray[j].lf.lfItalic)
			{
				break;
			}
		}

		if (j >= nFonts)
		{
			char szFont[LF_FULLFACESIZE];
			lstrcpy(szFont, lfArray[i].lfFaceName);
			if (lfArray[i].lfWeight == 700)
				lstrcat(szFont, "(Bold)");
			if (lfArray[i].lfItalic)
				lstrcat(szFont, "(Italic)");

			for (int k = 0; FontNames[k].pszFont; k++)
			{
				if (!lstrcmp(FontNames[k].pszFont, szFont))
				{
					FONTDOWNLOAD fd;
					lstrcpy(fd.szFontFile, FontNames[k].pszFontFile);
					lstrcpy(fd.szFullName, FontNames[k].pszFullName);
					fd.pDownloadData = NULL;
					fd.dwDownloadSize = 0;
					DownloadArray.push_back(fd);
					break;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CFontList::InstallFont(BYTE* pBytes, DWORD dwLen, const char* pszTypeFace, const char* pszTTFName)
{
	bool bInstalled = false;

	DWORD dwUncompressedLen = *((DWORD*)pBytes);
	BYTE* pUncompressedBytes = (BYTE*)malloc(dwUncompressedLen);
	uncompress(pUncompressedBytes, &dwUncompressedLen, pBytes + 4, dwLen - 4);

	char szTTFName[15];
	char szTTFDir[_MAX_PATH];
	char szTTFPath[_MAX_PATH];
	::GetWindowsDirectory(szTTFDir, sizeof(szTTFDir));
	if (szTTFDir[lstrlen(szTTFDir) - 1] != '\\')
		lstrcat(szTTFDir, "\\");

	lstrcpy(szTTFName, pszTTFName);
	szTTFName[lstrlen(szTTFName) - 1] = 'F';

	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&osvi);
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS || (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion >= 4))
		lstrcat(szTTFDir, "Fonts\\");
	else
		lstrcat(szTTFDir, "System\\");

	HANDLE	hf;
	int nCount = 0;
	do
	{
		if (nCount > 0)
			szTTFName[7] = (char)('0' + nCount);
		lstrcpy(szTTFPath, szTTFDir);
		lstrcat(szTTFPath, szTTFName);
		hf = ::CreateFile(szTTFPath, GENERIC_WRITE, 0, NULL,
		  CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	} while (hf == INVALID_HANDLE_VALUE && ++nCount < 10);

	if (hf != INVALID_HANDLE_VALUE)
	{
		DWORD dwWritten;
		::WriteFile(hf, pUncompressedBytes, dwUncompressedLen, &dwWritten, NULL);
		::CloseHandle(hf);

		if (::AddFontResource(szTTFPath))
		{
			LPCTSTR lpSubKey;
			if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
				lpSubKey = FONT_SUBKEY;
			else
				lpSubKey = NTFONT_SUBKEY;

			HKEY hKey;
			if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
			{
				char szFaceName[100];
				lstrcpy(szFaceName, pszTypeFace);
				lstrcat(szFaceName, "(TrueType)");

				::RegSetValueEx(hKey, szFaceName, 0, REG_SZ, (BYTE*)szTTFName, lstrlen(szTTFName) + 1);
				::RegCloseKey(hKey);

				bInstalled = true;
			}

			::SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
		}
	}

	free(pUncompressedBytes);

	if (!bInstalled)
	{
		char szMsg[128];
		wsprintf(szMsg, "Unable to install font '%s' on your system.", pszTypeFace);
		::MessageBox(NULL, szMsg, "Font Install", MB_OK);
	}
}
