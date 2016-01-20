// Source file: pal.cpp
void ClearSystemPalette(void);
HPALETTE CreateIdentityPalette( LPRGBQUAD pColorTable, int nColors );
BOOL MakeIdentityPalette( HPALETTE hPal );
HPALETTE CopySystemPalette( BYTE peFlags = 0 );
HPALETTE CopyPalette( HPALETTE hPal );
HPALETTE CreateCustomPalette( LPRGB lpRGBmap, int iEntries );
HPALETTE CreateCustomPalette( LPRGBQUAD lpRGBmap, int iEntries );
BOOL ComparePalettes(HPALETTE hPal1, HPALETTE hPal2);
HPALETTE LoadPalette(LPCTSTR lpFileName);
BOOL SavePalette(LPCTSTR lpFileName, HPALETTE hPal);

