//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern HINSTANCE hInstAstral;

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


/***********************************************************************/
int WINPROC EXPORT EnumFunc(
// PURPOSE: Initializes window data and registers window class
/***********************************************************************/
LPLOGFONT 		lpLogFont,
LPTEXTMETRIC 	lpTextMetric,
short 			FontType,
LPTR 			lpData)
{
int i;

switch (LOWORD(lpData))
	{
	case 0:
		return( ++Text.FontCount );

	case 1:
		if (!Text.lpFontList)
			break;
		for (i = 0; i < Text.FontCount; ++i)
			{
			if (StringsEqual(lpLogFont->lfFaceName,
					Text.lpFontList[i].fdFaceName))
				return(Text.FontCount);
			}
		lstrcpy( Text.lpFontList[Text.FontCount].fdFaceName,
			lpLogFont->lfFaceName );
		Text.lpFontList[Text.FontCount].fdCharSet = lpLogFont->lfCharSet;
		Text.lpFontList[Text.FontCount].fdPitchAndFamily =
			lpLogFont->lfPitchAndFamily;
		Text.lpFontList[Text.FontCount].FontType = FontType;
		return( ++Text.FontCount );
	}

return( 0 );
}


/***********************************************************************/
void GetFonts(HWND hWnd)
/***********************************************************************/
{
HDC hDC;
OLDFONTENUMPROC lpEnumFunc;

if (Text.lpFontList)
	{
	FreeUp((LPTR)Text.lpFontList);
	Text.lpFontList = NULL;
	}
hDC = GetDC(hWnd);
lpEnumFunc = (OLDFONTENUMPROC)MakeProcInstance((FARPROC)EnumFunc, hInstAstral);
Text.FontCount = 0;
EnumFonts(hDC, NULL, lpEnumFunc, (LPARAM)0);
Text.lpFontList = (LPFONTDESC)Alloc((long)sizeof(FONTDESC)*Text.FontCount);
Text.FontCount = 0;
EnumFonts(hDC, NULL, lpEnumFunc, (LPARAM)1);
FreeProcInstance((FARPROC)lpEnumFunc);
ReleaseDC(hWnd, hDC);
SortFonts();
}


/***********************************************************************/
void SortFonts()
/***********************************************************************/
{
int i, j, n;
FONTDESC tempDesc;

n = Text.FontCount;
for (i = 1; i < n; ++i)
	{
	tempDesc = Text.lpFontList[i];
	for (j = i-1; j >= 0 && lstrcmp(Text.lpFontList[j].fdFaceName, tempDesc.fdFaceName) > 0; --j)
		Text.lpFontList[j+1] = Text.lpFontList[j];
	Text.lpFontList[j+1] = tempDesc;
	}
}
