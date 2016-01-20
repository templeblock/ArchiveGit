#ifndef __FONT_H_
#define __FONT_H_

#include "AGDoc.h"

typedef struct
{
	LOGFONT lf;
	char szFullName[LF_FULLFACESIZE];
} FONTINFO;

typedef std::vector<FONTINFO> FONTARRAY;

typedef struct
{
	char szFontFile[_MAX_FNAME];
	char szFullName[LF_FULLFACESIZE];
	BYTE* pDownloadData;
	DWORD dwDownloadSize;
} FONTDOWNLOAD;

typedef std::vector<FONTDOWNLOAD> FONTDOWNLOADARRAY;


class CFontList
{
public:
	CFontList();

	void CheckFonts(LOGFONTARRAY &lfArray, FONTDOWNLOADARRAY &DownloadArray);
	HDC GetEnumFontDC()
		{ return (m_EnumFontDC); }
	FONTARRAY& GetFontArray()
		{ return (m_FontArray); }
	void InitFontArray();
	void InstallFont(BYTE* pBytes, DWORD dwLen, const char* pszTypeFace, const char* pszTTFName);

protected:
	void CheckDefaultFont();

protected:
	FONTARRAY m_FontArray;
	HDC m_EnumFontDC;
};

#endif //__FONT_H_
