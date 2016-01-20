/*
    mmisc.h
*/



LPSTR  MiscLtoaWithCommas(long lVal, LPSTR psz, UINT nSize);
LPSTR  MiscUltoaWithCommas(DWORD dw, LPSTR psz, UINT nSize);
int    MiscPaletteColors(const char *pszFilename, RGBQUAD PICFAR* pColors, int nColors);
BOOL   MiscPaletteBrowse(HWND hwndDlg, int nEditID, int nColors);
BOOL   MiscGetPassword(HWND hwnd, LPSTR pszPassword);
void   MiscShortFilename(LPCSTR pszFilename, LPSTR pszShortname, UINT nShortLen);
LPCSTR MiscVisualOrient(ORIENTATION Orient);
BOOL   MiscIsPaletteDisplay(int *pNumColors);
BOOL   MiscIsImageType(DWORD biCompression, OPENOP *pOpen);
BOOL   MiscQuantization(LPCSTR pszFilename, BYTE PICHUGE* Quantization);
BOOL   MiscQuantizationBrowse(HWND hwndDlg, int nID);
VOID PICHUGE* MiscGlobalAllocPtr(DWORD dwSize, int nIDSError);
void   MiscGlobalFreePtr(LPVOID pv);    /* see comments, used as LPVOID PICFAR* ppv */
BOOL MiscAllocateComment(
    LONG dwLen,
    CHAR PICHUGE* PICFAR* ppComment,
    LONG PICFAR* pdwSize);
BOOL MiscIsGrayPalette(DWORD dwColors, const RGBQUAD PICFAR* pColorTable);
void MiscCopyToQueue(
    BYTE PICHUGE* PICFAR* ppSrc,
    DWORD PICFAR* pdwSrc,
    QUEUE PICFAR* pQ,
    DWORD dwStripLen);
void MiscCopyFromQueue(
    BYTE PICHUGE* PICFAR* ppDest,
    DWORD PICFAR* pdwDest,
    QUEUE PICFAR* pQ,
    DWORD dwCopyLen);
DWORD MiscQLen(QUEUE PICFAR* pQ);
BOOL MiscYield(void);
void MiscDisplayTimingDialog(HWND hwnd, LPCSTR pszDlg, DWORD *pdwTimes, LPCTRLINITDATA pTimingDef);
BOOL MiscImageFileOpenDialog(HWND hwnd, BOOL (*pfn)(LPCSTR, DWORD), DWORD dwParam);
CHAR PICHUGE *MiscAllocatePIC2Comment(CHAR PICHUGE *PIC2List, BYTE  Type);
CHAR PICHUGE *MiscAllocatePIC2RawData(CHAR PICHUGE *PIC2List, CHAR *RawDesc);
DWORD MiscTickCount(void);
