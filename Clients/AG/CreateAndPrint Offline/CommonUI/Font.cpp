#include "stdafx.h"
#include "Font.h"
#include "resource.h"
#include "zutil.h"
#include "MessageBox.h"
#include "RegKeys.h"
#include "TrueType.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

struct FONTRECORD {
	const TCHAR* pFileName;
	const TCHAR* pRegistryName;
	const TCHAR* lfFaceName;
	long lfWeight;
	BYTE lfItalic;
	bool bIsInstalled;
};

static FONTRECORD OurFontList[] =
{
//	pFileName		pRegistryName					lfFaceName						lfWeight/lfItalic	bIsInstalled
	"CACCAMEL.TTZ",	"CAC Camelot",					"CAC Camelot",					FW_NORMAL,	0,		false,
	"CACCHAMP.TTZ",	"CAC Champagne",				"CAC Champagne",				FW_NORMAL,	0,		false,
	"CACFC___.TTZ",	"CAC Futura Casual",			"CAC Futura Casual",			FW_NORMAL,	0,		false,
	"CACFCB__.TTZ",	"CAC Futura Casual Bold",		"CAC Futura Casual Bold",		FW_NORMAL,	0,		false,
	"CACFCBI_.TTZ",	"CAC Futura Casual Bold Italic","CAC Futura Casual Bold Italic",FW_NORMAL,	0,		false,
	"CACFCMI_.TTZ",	"CAC Futura Casual Med. Italic","CAC Futura Casual Med. Italic",FW_NORMAL,	0,		false,
	"CACKL___.TTZ",	"CAC Krazy Legs",				"CAC Krazy Legs",				FW_NORMAL,	0,		false,
	"CACKLB__.TTZ",	"CAC Krazy Legs Bold",			"CAC Krazy Legs Bold",			FW_NORMAL,	0,		false,
	"CACLC___.TTZ",	"CAC Lasko Condensed",			"CAC Lasko Condensed",			FW_NORMAL,	0,		false,
	"CACLEW__.TTZ",	"CAC Lasko Even Weight",		"CAC Lasko Even Weight",		FW_LIGHT,	0,		false,
	"CACLESLI.TTZ",	"CAC Leslie",					"CAC Leslie",					FW_NORMAL,	0,		false,
	"CACLA___.TTZ",	"CAC Logo Alternate",			"CAC Logo Alternate",			FW_NORMAL,	0,		false,
	"CACMOOSE.TTZ",	"CAC Moose",					"CAC Moose",					FW_NORMAL,	0,		false,
	"CACNH___.TTZ",	"CAC Norm Heavy",				"CAC Norm Heavy",				FW_NORMAL,	0,		false,
	"CACOS___.TTZ",	"CAC One Seventy",				"CAC One Seventy",				FW_NORMAL,	0,		false,
	"CACPINAF.TTZ",	"CAC Pinafore", 				"CAC Pinafore", 				FW_NORMAL,	0,		false,
	"CACSB___.TTZ",	"CAC Saxon Bold",				"CAC Saxon Bold",				FW_NORMAL,	0,		false,
	"CACSHISH.TTZ",	"CAC Shishoni Brush",			"CAC Shishoni Brush",			FW_NORMAL,	0,		false,
	"CACVALIA.TTZ",	"CAC Valiant",					"CAC Valiant",					FW_NORMAL,	0,		false,
	"COLLE27.TTZ",	"ColleenBTwo",					"ColleenBTwo",					FW_NORMAL,	0,		false,
	"TT0952M_.TTZ",	"Cataneo BT",					"Cataneo BT",					FW_NORMAL,	0,		false,
	"TT0604M_.TTZ",	"Dom Casual BT", 				"DomCasual BT", 				FW_NORMAL,	0,		false,
	"TT1046M_.TTZ",	"Freehand 575 BT",				"Freehand575 BT",				FW_NORMAL,	0,		false,
	"TT0108M_.TTZ",	"Goudy Old Style BT",			"GoudyOlSt BT",			 		FW_NORMAL,	0,		false,
	"TT1115M_.TTZ",	"Informal 011 BT",				"Informal011 BT",				FW_NORMAL,	0,		false,
	"TT0845M_.TTZ",	"Lydian Cursive BT",			"Lydian Csv BT",				FW_NORMAL,	0,		false,
	"TT1014M_.TTZ",	"Matt Antique BT",				"MattAntique BT",				FW_NORMAL,	0,		false,
//j	"TT1015M_.TTZ",	"Matt Antique Italic BT",		"MattAntique BT",				FW_NORMAL,	255,	false, // redundant italic version
	"TT0196M_.TTZ",	"Snell BT", 					"Snell BT", 					FW_NORMAL,	0,		false,
	"TT1244M_.TTZ",	"Sprocket BT",					"Sprocket BT",					FW_NORMAL,	0,		false,
	"TT0005M_.TTZ",	"Swiss 721 Bold BT",			"Swis721 BT",		 			FW_BOLD,	0,		false,
	"TT0759M_.TTZ",	"Swiss 721 Medium BT",			"Swis721 Md BT",				FW_NORMAL,	0,		false,
};

/////////////////////////////////////////////////////////////////////////////
static int CALLBACK EnumFontFamilyProc(const LOGFONT* pLogFont, const TEXTMETRIC* pTextMetric, DWORD FontType, LPARAM lParam)
{
	if (!(FontType & TRUETYPE_FONTTYPE))
		return true; // continue enumerating

	ENUMLOGFONTEX* pLogFontEx = (ENUMLOGFONTEX*)pLogFont;
	NEWTEXTMETRICEX* pTextMetricEx = (NEWTEXTMETRICEX*)pTextMetric;

	LOGFONTEX fi;
	fi.lf = *pLogFont;
	lstrcpy(fi.strRegistryName, (const char*)pLogFontEx->elfFullName);

	if (fi.lf.lfFaceName[0] == '@')
		lstrcpy(fi.lf.lfFaceName, (const char*)&fi.lf.lfFaceName[1]);
	if (fi.strRegistryName[0] == '@')
		lstrcpy(fi.strRegistryName, (const char*)&fi.strRegistryName[1]);

	// Add it to the caller's list
	CFontList* pFontList = (CFontList*)lParam;
	pFontList->GetInstalledFonts().push_back(fi);

	// Loop through all of our fonts
	for (int i = 0; i < sizeof(OurFontList)/sizeof(FONTRECORD); i++)
	{
		// See if this is one of ours
		if (lstrcmp(OurFontList[i].lfFaceName, fi.lf.lfFaceName))
			continue; // Not a match

		// It's our font!
		// Flag it as being installed
		OurFontList[i].bIsInstalled = true;
		break;
	}

	return true; // continue enumerating
}

/////////////////////////////////////////////////////////////////////////////
CFontList::CFontList()
{
	UpdateInstalledFontList();
}

/////////////////////////////////////////////////////////////////////////////
const char* CFontList::GetFontFileName(int index)
{
	if (index < 0 || index >= sizeof(OurFontList)/sizeof(FONTRECORD))
		return NULL;

	return OurFontList[index].pFileName;
}

/////////////////////////////////////////////////////////////////////////////
void CFontList::UpdateInstalledFontList()
{
	HDC hDC = ::CreateIC("DISPLAY", NULL, NULL, NULL);
	if (!hDC)
		return;

	GetInstalledFonts().clear();

	// Loop through all of our fonts, and mark them as uninstalled
	for (int i = 0; i < sizeof(OurFontList)/sizeof(FONTRECORD); i++)
		OurFontList[i].bIsInstalled = false;

	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(lf));
	lf.lfCharSet = ANSI_CHARSET; //j DEFAULT_CHARSET;
	DWORD dw = 0;
	::EnumFontFamiliesEx(hDC, &lf, EnumFontFamilyProc, (LPARAM)this, dw);
	::DeleteDC(hDC);
}	

/////////////////////////////////////////////////////////////////////////////
bool CFontList::IsOurFont(const LOGFONT& lfFont)
{
	// Loop through all of our fonts
	for (int i = 0; i < sizeof(OurFontList)/sizeof(FONTRECORD); i++)
	{
		// See if this is one of ours
		if (lstrcmp(OurFontList[i].lfFaceName, lfFont.lfFaceName))
			continue; // Not a match

		// It's our font!
		// Consider it a match if the facenames match (ignore styles)
		bool bStyleMatch = 
			(lfFont.lfWeight == OurFontList[i].lfWeight &&
			!lfFont.lfItalic == !OurFontList[i].lfItalic);

		return true; 
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CFontList::IsOurFont(const CString& strFontName)
{
	// Loop through all of our fonts
	for (int i = 0; i < sizeof(OurFontList)/sizeof(FONTRECORD); i++)
	{
		// See if this is one of ours
		if (lstrcmp(OurFontList[i].lfFaceName, (LPCSTR)strFontName))
			continue; // Not a match

		// It's our font!
		return true; 
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
void CFontList::GetFontList(FONTDOWNLOADLIST& DownloadList, bool bMissingOnly)
{
	DownloadList.clear();

	// Loop through all of our fonts
	for (int i = 0; i < sizeof(OurFontList)/sizeof(FONTRECORD); i++)
	{
		// If we want them all, or if it's not installed, add it to the list
		if (!bMissingOnly || !OurFontList[i].bIsInstalled)
			DownloadList.push_back(i);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CFontList::GetProjectFontList(const LOGFONTLIST& lfProjectFontList, FONTDOWNLOADLIST& DownloadList, bool bMissingOnly)
{
	DownloadList.clear();

	// Loop through all of the passed fonts
	int nCount = lfProjectFontList.size();
	for (int j = 0; j < nCount; j++)
	{
		// Loop through all of our fonts
		for (int i = 0; i < sizeof(OurFontList)/sizeof(FONTRECORD); i++)
		{
			// See if this is one of ours
			if (lstrcmp(OurFontList[i].lfFaceName, lfProjectFontList[j].lfFaceName))
				continue; // Not a match

			// It's our font!
			// If we want them all, or if it's not installed, add it to the list
			if (!bMissingOnly || !OurFontList[i].bIsInstalled)
				DownloadList.push_back(i);

			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CFontList::InstallFont(BYTE* pBytes, DWORD dwLen, int index)
{
	if (!pBytes || !dwLen || index < 0 || index >= sizeof(OurFontList)/sizeof(FONTRECORD))
		return;

	FONTRECORD& InstallFontRec = OurFontList[index];
	char szFileName[MAX_PATH];
	lstrcpy(szFileName, InstallFontRec.pFileName);
	szFileName[lstrlen(szFileName) - 1] = 'F'; // Change the .TTZ to .TTF
	CString strPath = CTrueType::GetPathFromFileName(szFileName);

	// Uncompress the font data
	DWORD dwUncompressedLen = *((DWORD*)pBytes);
	BYTE* pUncompressedBytes = new BYTE[dwUncompressedLen];
	if (!pUncompressedBytes)
		return;

	int iRet = uncompress(pUncompressedBytes, &dwUncompressedLen, pBytes + 4, dwLen - 4);
	if (iRet != Z_OK)
	{
		delete [] pUncompressedBytes;
		return;
	}

	// See if the font file already exists
	bool bCopy = (dwUncompressedLen != FileSize(strPath));
	if (bCopy)
	{
		int bRemoved = ::RemoveFontResource(strPath);

		// Write the font data to disk
		HANDLE hFile = ::CreateFile(strPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwWritten = 0;
			::WriteFile(hFile, pUncompressedBytes, dwUncompressedLen, &dwWritten, NULL);
			::CloseHandle(hFile);
		}
	}

	delete [] pUncompressedBytes;

	// Make the font installation permanent by adding the font's display name and filename to the registry 
	CTrueType::AddToRegistry(InstallFontRec.pRegistryName, szFileName);
	
	// Make the font available immediately
	int nFontsAdded = ::AddFontResource(strPath);

	// Warn the user if the font doesn't appear to be installed
	if (nFontsAdded <= 0)
		CMessageBox::Message(String("Error installing font '%s'.", InstallFontRec.pRegistryName));
}
