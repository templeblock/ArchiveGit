LPFRAME aviGetFrame(LPTSTR lpFileName, LONG lFrame);
BOOL aviInstertFrame(LPTSTR lpSrcFile, LPTSTR lpDstFile, int iAVIPosType, long AVIPosition, BOOL bClip, LPFRAME lpFrame, LPOBJECT lpObject);
BOOL aviNewFile(LPTSTR lpFile, LPFRAME lpFrame, LPOBJECT lpObject);
