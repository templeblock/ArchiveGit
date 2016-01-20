#include "stdafx.h"
#include "Font.h"
#include "resource.h"
#include "zutil.h"
#include "MessageBox.h"
#include "RegKeys.h"
#include "TrueType.h"

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
    "BONNN___.TTZ",	"BonnieN Plain",				"BonnieN",						FW_NORMAL,	0,		false,
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
	"CARMF___.TTZ",	"Carmody Font",					"Carmody",						FW_NORMAL,	0,		false,
	"CARTM___.TTZ",	"Cartoon Medium",				"Cartoon",						FW_NORMAL,	0,		false,
	"TT0952M_.TTZ",	"Cataneo BT",					"Cataneo BT",					FW_NORMAL,	0,		false,
	"CHRIM___.TTZ",	"ChristineMed",					"ChristineMed",					FW_NORMAL,	0,		false,
	"COLLE27.TTZ",	"ColleenBTwo",					"ColleenBTwo",					FW_NORMAL,	0,		false,
	"COLLP___.TTZ",	"Collier Plain",				"Collier",						FW_NORMAL,	0,		false,
	"TT0604M_.TTZ",	"Dom Casual BT", 				"DomCasual BT", 				FW_NORMAL,	0,		false,
	"EMILRT__.TTZ",	"EmilyRose Two",				"EmilyRose",					FW_NORMAL,	0,		false,
	"ERIKF___.TTZ",	"Erikhead Font",				"Erikhead",						FW_NORMAL,	0,		false,
	"TT1046M_.TTZ",	"Freehand 575 BT",				"Freehand575 BT",				FW_NORMAL,	0,		false,
	"FUNTT___.TTZ",	"Funtime Two",					"Funtime",						FW_NORMAL,	0,		false,
	"GIRAP___.TTZ",	"Giraffe Plain",				"Giraffe",						FW_NORMAL,	0,		false,
	"TT0108M_.TTZ",	"Goudy Old Style BT",			"GoudyOlSt BT",			 		FW_NORMAL,	0,		false,
	"GRAPRG__.TTZ",	"Grapes",						"Grapes",						FW_NORMAL,	0,		false,
	"GUMBP___.TTZ",	"Gumbo Plain",					"Gumbo",						FW_NORMAL,	0,		false,
	"HARVT___.TTZ",	"Harvest Two",					"Harvest",						FW_NORMAL,	0,		false,
	"HETZT___.TTZ",	"HetzelTwo",					"HetzelTwo",					FW_NORMAL,	0,		false,
	"HOMET___.TTZ",	"HomespunTwo",					"HomespunTwo",					FW_NORMAL,	0,		false,
	"TT1115M_.TTZ",	"Informal 011 BT",				"Informal011 BT",				FW_NORMAL,	0,		false,
	"Isaac.TTZ",	"Isaac",						"Isaac",						FW_NORMAL,	0,		false,
	"JIFFYTWO.TTZ",	"Jiffy Two",					"Jiffy",						FW_NORMAL,	0,		false,
	"JOHNNIED.TTZ",	"JohnnieD",						"JohnnieD",						FW_NORMAL,	0,		false,
	"JOHNST__.TTZ",	"JohnScriptTwo",				"JohnScriptTwo",				FW_NORMAL,	0,		false,
	"JOKEWF__.TTZ",	"JokersWild Font",				"JokersWild",					FW_NORMAL,	0,		false,
	"KERBT___.TTZ",	"Kerber Two",					"Kerber",						FW_NORMAL,	0,		false,
	"LADYSTP_.TTZ",	"LadyScriptTwo Plain",			"LadyScriptTwo",				FW_NORMAL,	0,		false,
	"LINETP__.TTZ",	"LinetThree Plain",				"LinetThree",					FW_NORMAL,	0,		false,
	"LOREI___.TTZ",	"Lored Italic",					"Lored",						FW_NORMAL,	1,		false,
	"Lored52.TTZ",	"LoredTwoFont Two Font",		"LoredTwoFont",					FW_NORMAL,	0,		false,
	"LOUITP__.TTZ",	"LouiseThree Plain",			"LouiseThree",					FW_NORMAL,	0,		false,
	"TT0845M_.TTZ",	"Lydian Cursive BT",			"Lydian Csv BT",				FW_NORMAL,	0,		false,
	"MARST___.TTZ",	"Mars Three",					"Mars",							FW_NORMAL,	0,		false,
	"MARYTF__.TTZ",	"Mary TwoFont",					"Mary",							FW_NORMAL,	0,		false,
	"TT1014M_.TTZ",	"Matt Antique BT",				"MattAntique BT",				FW_NORMAL,	0,		false,
	"MICHT___.TTZ",	"Michael Two",					"Michael",						FW_NORMAL,	0,		false,
	"NAPITF__.TTZ",	"NapierTwo Font",				"NapierTwo Font",				FW_NORMAL,	0,		false,
	"NOLAFP__.TTZ",	"NolanFour Plain",				"NolanFour",					FW_NORMAL,	0,		false,
	"NORIM___.TTZ",	"Noris Medium",					"Noris",						FW_NORMAL,	0,		false,
	"NUNNSTP_.TTZ",	"NunneyScriptTwo Plain",		"NunneyScriptTwo",				FW_NORMAL,	0,		false,
	"PARCTP__.TTZ",	"ParchmentThree Plain",			"ParchmentThree",				FW_NORMAL,	0,		false,
	"SAGESTP_.TTZ",	"SageScriptTwo Plain",			"SageScriptTwo",				FW_NORMAL,	0,		false,
	"SCOTO___.TTZ",	"Scotty O Plain",				"Scotty O",						FW_NORMAL,	0,		false,
	"SNAPF___.TTZ",	"Snappy Font",					"Snappy",						FW_NORMAL,	0,		false,
	"TT0196M_.TTZ",	"Snell BT", 					"Snell BT", 					FW_NORMAL,	0,		false,
	"TT1244M_.TTZ",	"Sprocket BT",					"Sprocket BT",					FW_NORMAL,	0,		false,
	"SpunkJesPla.TTZ", "SpunkyJesPlain",			"SpunkyJes",					FW_NORMAL,	0,		false,
	"SUEZQ___.TTZ",	"SueZQ Plain",					"SueZQ",						FW_NORMAL,	0,		false,
	"TT0005M_.TTZ",	"Swiss 721 Bold BT",			"Swis721 BT",		 			FW_BOLD,	0,		false,
	"TT0759M_.TTZ",	"Swiss 721 Medium BT",			"Swis721 Md BT",				FW_NORMAL,	0,		false,
	"TIGGF___.TTZ",	"Tigger Font",					"Tigger",						FW_NORMAL,	0,		false,
	"TYPOFONT.TTZ",	"Typo Font",					"Typo",							FW_NORMAL,	0,		false,
	"WILDBT__.TTZ",	"WildBill Two",					"WildBill",						FW_NORMAL,	0,		false,
	"WILDBBT_.TTZ",	"WildBillBold Two",				"WildBillBold",					FW_BOLD,	0,		false,
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
	CString strTempPath = CTrueType::GetTempPathFromFileName(szFileName);
	
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
		if (hFile == INVALID_HANDLE_VALUE)
		{
			if (dwUncompressedLen != FileSize(strTempPath))
			{
				hFile = ::CreateFile(strTempPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				strPath = strTempPath;
			}
		}
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
