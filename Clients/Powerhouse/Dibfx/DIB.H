/*----------------------------------------------------------------------------*\
|   Routines for dealing with Device independent bitmaps                       |
|									       |
|   History:                                                                   |
|       06/23/89 toddla     Created                                            |
|                                                                              |
\*----------------------------------------------------------------------------*/

#ifndef _INC_DIB
#define _INC_DIB

#ifdef __cplusplus
extern "C" {
#endif

typedef LPBITMAPINFOHEADER PDIB;
typedef HANDLE HDIB;
typedef BYTE huge *HPBYTE;

PDIB WINAPI DibOpenFile(LPSTR szFile);
BOOL WINAPI DibWriteFile(PDIB pdib, LPSTR szFile);
PDIB WINAPI DibReadBitmapInfo(HFILE fh);
HPALETTE WINAPI DibCreatePalette(PDIB pdib);
BOOL WINAPI DibSetUsage(PDIB pdib, HPALETTE hpal,UINT wUsage);
BOOL WINAPI DibDraw(HDC hdc, int x, int y, int dx, int dy, PDIB pdib, int x0, int y0, int dx0, int dy0, LONG rop, UINT wUsage);
PDIB WINAPI DibCreate(int bits, int dx, int dy);
PDIB WINAPI DibCopy(PDIB pdib);
void WINAPI DibMapToPalette(PDIB pdib, HPALETTE hpal);  
PDIB WINAPI DibConvert(PDIB pdib, int BitCount, DWORD biCompression);
PDIB WINAPI DibHalftoneDIB(PDIB pdib);

PDIB WINAPI DibFromBitmap(HBITMAP hbm, DWORD biStyle, UINT biBits, HPALETTE hpal, UINT wUsage);
HBITMAP WINAPI BitmapFromDib(PDIB pdib, HPALETTE hpal, UINT wUsage);

void WINAPI MakeIdentityPalette(HPALETTE hpal); 
HPALETTE WINAPI CopyPalette(HPALETTE hpal);

BOOL WINAPI DibIsRectEmpty(PDIB pdib, LPRECT pRect, BYTE bTransparent);
BOOL WINAPI DibIsRectFull(PDIB pdib, LPRECT pRect, BYTE bTransparent);
DWORD WINAPI DibCountRectPixels(PDIB pdib, LPRECT pRect, BYTE bTransparent);
int WINAPI DibFindColor(PDIB pdib, COLORREF color);

HPBYTE WINAPI GetDIBPixelAddress(PDIB pdib, UINT x, UINT y);
BYTE WINAPI GetDIBPixelValue(PDIB pdib, UINT x, UINT y);
COLORREF WINAPI GetDIBPixelColor(PDIB pdib, UINT x, UINT y);

void WINAPI DeltaFrame386(PDIB pdib, LPBYTE pbitsPrev, LPBYTE pbitsCur, LPBYTE pbitsRLE);
void WINAPI DecodeRle(LPBITMAPINFOHEADER lpbi, HPBYTE pb, HPBYTE prle);

/****************************************************************************
 DIB macros.
 ***************************************************************************/

#ifdef  WIN32
    #define HandleFromDib(lpbi) GlobalHandle(lpbi)
#else
    #define HandleFromDib(lpbi) (HANDLE)GlobalHandle(SELECTOROF(lpbi))
#endif

#define DibFromHandle(h)        (PDIB)GlobalLock(h)

#define DibFree(pdib)           GlobalFreePtr(pdib)

#define WIDTHBYTES(cbits)	((((unsigned)cbits+31)&(~31))/8)  /* DWORD aligned ! */

#define DibWidth(pdib)          ((UINT)(pdib)->biWidth)
#define DibHeight(pdib)		(((pdib)->biHeight < 0)			\
					? (UINT) (- (pdib)->biHeight)	\
					: (UINT) (pdib)->biHeight	\
				)

#define DibBitCount(pdib)       (UINT)((pdib)->biBitCount)
#define DibCompression(pdib)    (DWORD)((pdib)->biCompression)

#define DibWidthBytesN(pdib, n) WIDTHBYTES(DibWidth(pdib) * (UINT)(n))
#define DibWidthBytes(pdib)	DibWidthBytesN(pdib, DibBitCount(pdib))

#define DibPitch(pdib)		DibWidthBytesN(pdib, (pdib)->biBitCount)

#define DibSizeImage(pdib)      ((pdib)->biSizeImage == 0				\
				    ? (DWORD)DibPitch(pdib) * (DWORD)DibHeight(pdib)	\
				    : (pdib)->biSizeImage				\
				)

#define DibSizeColors(pdib)	(DibNumColors(pdib) * sizeof(RGBQUAD))
#define DibPaletteSize(lpbi)    (DibNumColors(lpbi) * sizeof(RGBQUAD))
#define DibSizeHeader(pdib)	((pdib)->biSize)
#define DibSize(pdib)		((pdib)->biSize + DibSizeImage(pdib) + DibSizeColors(pdib))

#define DibFlipY(lpbi, y)       ((int)(lpbi)->biHeight-1-(y))

//HACK for NT BI_BITFIELDS DIBs
#ifdef WIN32
    #define DibPtr(lpbi)            ((lpbi)->biCompression == BI_BITFIELDS \
                                       ? (LPVOID)(DibColors(lpbi) + 3) \
                                       : (LPVOID)(DibColors(lpbi) + (UINT)(lpbi)->biClrUsed))
#else
    #define DibPtr(lpbi)            (LPVOID)(DibColors(lpbi) + (UINT)(lpbi)->biClrUsed)
#endif

#define DibColors(lpbi)         ((RGBQUAD FAR *)((LPBYTE)(lpbi) + (int)(lpbi)->biSize))

#define DibNumColors(lpbi)      ((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
                                    ? (int)(1 << (int)(lpbi)->biBitCount)          \
                                    : (int)(lpbi)->biClrUsed)

#define DibXYN(lpbi,pb,x,y,n)   (LPVOID)(                                     \
                                (BYTE _huge *)(pb) +                          \
                                (UINT)((UINT)(x) * (UINT)(n) / 8u) +          \
                                ((DWORD)DibWidthBytesN(lpbi,n) * (DWORD)(UINT)(y)))

#define DibXY(lpbi,x,y)         DibXYN(lpbi,DibPtr(lpbi),x,y,(lpbi)->biBitCount)

#define FixBitmapInfo(lpbi)     if ((lpbi)->biSizeImage == 0)                 \
                                    (lpbi)->biSizeImage = DibSizeImage(lpbi); \
                                if ((lpbi)->biClrUsed == 0)                   \
                                    (lpbi)->biClrUsed = DibNumColors(lpbi);   \
                                if ((lpbi)->biCompression == BI_BITFIELDS && (lpbi)->biClrUsed == 0) \
                                    ; // (lpbi)->biClrUsed = 3;                    

#define DibInfo(pDIB)			((BITMAPINFO FAR *)(pDIB))

/****************************************************************************
 ***************************************************************************/

#ifndef BI_BITFIELDS
	#define BI_BITFIELDS 3
#endif  

#ifndef HALFTONE
	#define HALFTONE COLORONCOLOR
#endif

#ifdef __cplusplus
}
#endif

#endif // _INC_DIB
