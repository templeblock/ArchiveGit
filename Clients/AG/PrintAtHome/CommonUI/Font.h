#pragma once

#include <vector>
typedef std::vector<LOGFONT> LOGFONTLIST;

struct LOGFONTEX
{
	LOGFONT lf;
	char strRegistryName[LF_FULLFACESIZE];
};

typedef std::vector<LOGFONTEX> LOGFONTEXLIST;

typedef std::vector<int> FONTDOWNLOADLIST;

class CFontList
{
public:
	CFontList();
	const char* GetFontFileName(int index);
	void UpdateInstalledFontList();
	bool IsOurFont(const LOGFONT& lfFont);
	static bool IsOurFont(const CString& strFontName);
	void GetFontList(FONTDOWNLOADLIST& DownloadList, bool bMissingOnly);
	void GetProjectFontList(const LOGFONTLIST &lfProjectFontList, FONTDOWNLOADLIST &DownloadList, bool bMissingOnly);
	void InstallFont(BYTE* pBytes, DWORD dwLen, int index);
	LOGFONTEXLIST& GetInstalledFonts()
		{ return m_InstalledFonts; }

protected:
	LOGFONTEXLIST m_InstalledFonts;
};
