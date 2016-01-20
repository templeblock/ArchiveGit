/*
    mopen.h
*/



void OpenOnFileOpen(void);
void OpenOnFileMruOpen(int nMruNum);
void OpenImageWindow(LPCSTR pszFilename, BYTE PICHUGE* pImage, DWORD dwSize, DWORD dwFlags);
BOOL OpenImage(HWND hwnd, LPCSTR pszFilename, BYTE PICHUGE* pImage, DWORD dwSize, DWORD dwFlags);
void OpenCleanup(CHILDINSTANCEDATA PICFAR* pInstance);
BOOL OpenFileExtractDib(
    LPCSTR                pszFilename,
    BYTE PICHUGE* PICFAR* ppDib,
    DWORD PICFAR*         pdwDibSize,
    PIC_PARM PICFAR*      pPicParmDib);
BOOL OpenMemoryExtractDib(
    LPCSTR                pszFilename,
    BYTE PICHUGE*         pImage,
    DWORD                 dwSize,
    BYTE PICHUGE* PICFAR* ppDib,
    DWORD PICFAR*         pdwDibSize,
    PIC_PARM PICFAR*      pPicParmDib);
BOOL OpenFileQuery(
    LPCSTR        pszFilename,
    BYTE PICHUGE* *ppDib,
    DWORD         *pdwDibSize,
    OPENOP       **ppOpenOp,
    PIC_PARM      *pDibPicParm);
