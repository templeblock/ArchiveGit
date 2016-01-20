// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include "pp.h"
#include "ppafx.h"
#include "id.h"

/***********************************************************************/
HFONT CreateLightFont( HFONT hFont )
/***********************************************************************/
{
LOGFONT logFont;
HFONT hNewFont;

if ( !hFont || !GetObject( hFont, sizeof(LOGFONT), (LPTR)&logFont ) )
	return( NULL );

logFont.lfWeight = FW_NORMAL;

if ( !(hNewFont = CreateFontIndirect( &logFont )) )
	return( NULL );
return( hNewFont );
}

#ifdef WIN32
                                                      
/***********************************************************************/
int WINPROC EXPORT EnumFunc( LPLOGFONT lpLogFont, LPTEXTMETRIC lpTextMetric,
                        DWORD FontType, LPARAM lParam )
// PURPOSE: Initializes window data and registers window class
/***********************************************************************/
#else
/***********************************************************************/
int WINPROC EXPORT EnumFunc( LPLOGFONT lpLogFont, LPTEXTMETRIC lpTextMetric,
                        short FontType, LPARAM lParam )
// PURPOSE: Initializes window data and registers window class
/***********************************************************************/
#endif
{
int i;
LPFONTLIST lpFontList;

lpFontList = (LPFONTLIST)lParam;
if (!lpFontList)
	return(FALSE);
if (lpFontList->fCountFonts)
	return( ++lpFontList->FontCount );
else
	{
	for (i = 0; i < lpFontList->FontCount; ++i)
		{
		if (StringsEqual(lpLogFont->lfFaceName,
				lpFontList->FontDesc[i].fdFaceName))
			return(lpFontList->FontCount);
		}
	lstrcpy( lpFontList->FontDesc[lpFontList->FontCount].fdFaceName,
		lpLogFont->lfFaceName );
	lpFontList->FontDesc[lpFontList->FontCount].fdCharSet = lpLogFont->lfCharSet;
	lpFontList->FontDesc[lpFontList->FontCount].fdPitchAndFamily =
		lpLogFont->lfPitchAndFamily;
	lpFontList->FontDesc[lpFontList->FontCount].FontType = FontType;
	return( ++lpFontList->FontCount );
	}
}

LPSTR BASED_CODE Empty_Str1       		  = "";
LPSTR BASED_CODE szFaceName_Str           = "szFaceName";
LPSTR BASED_CODE CurrentFontSize_Str      = "CurrentFont.Size";
LPSTR BASED_CODE CurrentFontItalic_Str    = "CurrentFont.Italic";
LPSTR BASED_CODE CurrentFontUnderline_Str = "CurrentFont.Underline";
LPSTR BASED_CODE CurrentFontStrikeout_Str = "CurrentFont.Strikeout";
LPSTR BASED_CODE CurrentFontWeight_Str    = "CurrentFont.Weight";
LPSTR BASED_CODE AntiAlias_Str            = "AntiAlias";
LPSTR BASED_CODE Justification_Str        = "Justification";
LPSTR BASED_CODE TextOpacity_Str          = "TextOpacity";
LPSTR BASED_CODE TextMergeMode_Str        = "TextMergeMode";

CTextForum		TextForum;

void CTextForum::Load()
{
	GetDefStr(FontName, Empty_Str1 );
	GetDefaultString(szFaceName_Str,Empty_Str1,FontName,sizeof(FontName));
	CurrentFont.Size=GetDefaultInt(CurrentFontSize_Str,12);
	CurrentFont.Italic=GetDefaultInt(CurrentFontItalic_Str,NO);
	CurrentFont.Underline=GetDefaultInt(CurrentFontUnderline_Str,NO);
	CurrentFont.Strikeout=GetDefaultInt(CurrentFontStrikeout_Str,NO);
	CurrentFont.Weight=GetDefaultInt(CurrentFontWeight_Str,NO);
	AntiAlias=GetDefaultInt(AntiAlias_Str,1);
	Justification=GetDefaultInt(Justification_Str,0);
	GetDefMergeMode(MergeMode, MM_NORMAL );
	Opacity=GetDefaultInt(TextOpacity_Str,255);
	FillFontList();
}

void CTextForum::Save()
{
	PutDefaultString( szFaceName_Str, FontName );
	PutDefaultInt( CurrentFontSize_Str, CurrentFont.Size );
	PutDefaultInt( CurrentFontItalic_Str, CurrentFont.Italic );
	PutDefaultInt( CurrentFontUnderline_Str, CurrentFont.Underline );
	PutDefaultInt( CurrentFontStrikeout_Str, CurrentFont.Strikeout );
	PutDefaultInt( CurrentFontWeight_Str, CurrentFont.Weight );
	PutDefaultInt( AntiAlias_Str, AntiAlias );
	PutDefaultInt( TextMergeMode_Str, MergeMode );
	PutDefaultInt( TextOpacity_Str, Opacity );
}

void CTextForum::SetAttr(LPFONTID lpFontID, int just, int AntiAlias)
{
	copy((LPTR)lpFontID, (LPTR)&CurrentFont, sizeof(FONTID));
	lstrcpy(FontName, CurrentFont.FontDesc.fdFaceName);
    AntiAlias       = AntiAlias;
    Justification   = just;
	lstrcpy(FontName, CurrentFont.FontDesc.fdFaceName);
	for (int i = 0; i < FontList->FontCount; ++i)
	{
		if (StringsEqual(FontName, FontList->FontDesc[i].fdFaceName))
		{
			FontIndex = i;
			break;
		}
	}
	MakeRemark (NULL);
}

void CTextForum::SetFontIndex(int index, CAttendee* pSkip)
{
	FontIndex = index;
	copy((LPTR)&FontList->FontDesc[index], (LPTR)&CurrentFont.FontDesc, sizeof(FONTDESC));
	lstrcpy(FontName, CurrentFont.FontDesc.fdFaceName);
	MakeRemark (pSkip);
}

int CTextForum::GetFontDesc(LPSTR lpFaceName, LPFONTDESC lpFontDesc)
{
	for (int i = 0; i < FontList->FontCount; ++i)
	{
		if (StringsEqual(lpFaceName, FontList->FontDesc[i].fdFaceName))
		{
			copy((LPTR)&FontList->FontDesc[i], (LPTR)lpFontDesc, sizeof(FONTDESC));
			return(i);
		}
	}
	return(-1);
}

void CTextForum::FillCombo(HWND hWnd)
{
	ShowWindow(hWnd, SW_HIDE);		//Hide Combo Box while filling with new items.
   	SendMessage(hWnd, CB_RESETCONTENT, 0, 0L );
   	if (!FontList)
		FillFontList();
	if (FontList)
	{
		for (int i=0; i < FontList->FontCount; i++)
			ComboBox_AddString(hWnd, FontList->FontDesc[i].fdFaceName );
	}
   	ShowWindow(hWnd, SW_SHOW);
}

/***********************************************************************/
void CTextForum::FillFontList()
/***********************************************************************/
{
	HDC 			hDC = GetDC(NULL);
	OLDFONTENUMPROC lpEnumFunc;
	long 			lSize;
	FONTLIST		List;

	if (FontList)
		return;
	lpEnumFunc = (OLDFONTENUMPROC)MakeProcInstance((FARPROC)EnumFunc, PictPubApp.Get_hInstAstral());
	List.FontCount = 0;
	List.fCountFonts = YES;
	EnumFonts(hDC, NULL, lpEnumFunc, (LPARAM)&List);
	lSize = sizeof(FONTLIST) + ((List.FontCount-1) * sizeof(FONTDESC));
	FontList = (LPFONTLIST)Alloc(lSize);
	if (!FontList)
	{
		Message(IDS_EMEMALLOC);
		return;
	}
	FontList->FontCount = 0;
	FontList->fCountFonts = NO;
	EnumFonts(hDC, NULL, lpEnumFunc, (LPARAM)FontList);
	FreeProcInstance((FARPROC)lpEnumFunc);
	ReleaseDC(NULL, hDC);
	SortFonts();
	if ((FontIndex = GetFontDesc(FontName, &CurrentFont.FontDesc)) == -1)
	{
		FontIndex = 0;
		copy((LPTR)&FontList->FontDesc[0], (LPTR)&CurrentFont.FontDesc, sizeof(FONTDESC));
		lstrcpy(FontName, CurrentFont.FontDesc.fdFaceName);
	}
	MakeRemark (NULL);
}


void CTextForum::SortFonts()
{
	int i, j, n;
	FONTDESC tempDesc;

	n = FontList->FontCount;
	for (i = 1; i < n; ++i)
	{
		tempDesc = FontList->FontDesc[i];
		for (j = i-1; j >= 0 && lstrcmp(FontList->FontDesc[j].fdFaceName, tempDesc.fdFaceName) > 0; --j)
			FontList->FontDesc[j+1] = FontList->FontDesc[j];
		FontList->FontDesc[j+1] = tempDesc;
	}
}
