#ifndef _INC_DIBAPI
#define _INC_DIBAPI

/* Handle to a DIB */
DECLARE_HANDLE(HDIB);

/* DIB constants */
#define PALVERSION   0x300

/* DIB Macros*/

#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth * biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed
// to hold those bits.

#define WIDTHBYTES(pbi)    ((((pbi)->bmiHeader.biWidth * (pbi)->bmiHeader.biBitCount) + 31) / 32 * 4)

/* Function prototypes */
HGLOBAL GetClipboardDib(HWND hWnd);
BOOL    PaintDIB (HDC, LPRECT, HDIB, LPRECT, CPalette* pPal);
BOOL    CreateDIBPalette(HDIB hDIB, CPalette* cPal);
LPSTR   FindDIBBits (LPSTR lpbi);
DWORD   DIBWidth (LPSTR lpDIB);
DWORD   DIBHeight (LPSTR lpDIB);
WORD    PaletteSize (LPSTR lpbi);
WORD    DIBNumColors (LPSTR lpbi);

#endif //!_INC_DIBAPI
