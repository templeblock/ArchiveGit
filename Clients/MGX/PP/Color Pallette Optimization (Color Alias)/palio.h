#ifndef _PALIO_H_
#define _PALIO_H_
BOOL SavePalette(LPSTR lpFileName, LPRGBQUAD lpRGBMap, int nEntries);
int LoadPalette(LPSTR lpFileName, LPRGBQUAD lpMap);
#endif // _PALIO_H_