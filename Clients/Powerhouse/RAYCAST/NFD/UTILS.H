// UTILS.H - useful functions for WinG Sample Apps
//

#ifndef WING_UTILS_H  
#define WING_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif 

typedef     LPBITMAPINFOHEADER PDIB;
typedef     HANDLE             HDIB;

/***************************************************************************
   External function declarations
 **************************************************************************/

void        ClearSystemPalette(CWnd *cwnd);
PDIB        DibOpenFile(LPSTR szFile);
PDIB        DibCreate(int bits, int dx, int dy);

/****************************************************************************
   Internal function declarations
 ***************************************************************************/

PDIB        DibReadBitmapInfo(HFILE fh);

/***************************************************************************
   Utility function declarations
 **************************************************************************/

void	TransparentBits(char unsigned huge *pDestination,
						char unsigned huge *pSource,
						DWORD dwWidth,
						DWORD dwHeight,
						long dwScanD,
						long dwScanS,
						char unsigned bTranClr );

void	CopyBits(	char unsigned huge *pDestination,
					char unsigned huge *pSource,
					DWORD dwWidth,
					DWORD dwHeight,
					long dwScanD,
					long dwScanS );

void	TransparentDIBBits(	BITMAPINFO far *pBufferHeader,
							char huge *pBufferBits,
							int nXOriginDest,
							int nYOriginDest,
							BITMAPINFO far *pBitmapInfo,
							char huge *pBitmapBits,
							int nXOriginSrc,
							int nYOriginSrc,
							unsigned char TransparentColor );

void	CopyDIBBits(BITMAPINFO far *pBufferHeader,
					char huge *pBufferBits,
					int nXOriginDest,
					int nYOriginDest,
					BITMAPINFO far *pBitmapInfo,
					char huge *pBitmapBits,
					int nXOriginSrc,
					int nYOriginSrc);

void	CopySomeDIBBits(BITMAPINFO far *pBufferHeader,
					char huge *pBufferBits,
					int nXOriginDest,
					int nYOriginDest,
					BITMAPINFO far *pBitmapInfo,
					char huge *pBitmapBits,
					int nXOriginSrc,
					int nYOriginSrc,
					int nWidthSrc,
					int nHeightSrc);

/****************************************************************************
   DIB macros.
 ***************************************************************************/

#define HandleFromDib(lpbi)     (HANDLE)GlobalHandle(SELECTOROF(lpbi))
#define DibFromHandle(h)        (PDIB)GlobalLock(h)
#define DibFree(pdib)           GlobalFreePtr(pdib)
#define WIDTHBYTES(i)           ((unsigned)((i+31)&(~31))/8)  /* ULONG aligned ! */
#define DibWidth(lpbi)          (UINT)(((LPBITMAPINFOHEADER)(lpbi))->biWidth)
#define DibHeight(lpbi)         (UINT)(((LPBITMAPINFOHEADER)(lpbi))->biHeight)
#define DibBitCount(lpbi)       (UINT)(((LPBITMAPINFOHEADER)(lpbi))->biBitCount)
#define DibCompression(lpbi)    (DWORD)(((LPBITMAPINFOHEADER)(lpbi))->biCompression)
#define DibWidthBytesN(lpbi, n) (UINT)WIDTHBYTES((UINT)(lpbi)->biWidth * (UINT)(n))
#define DibWidthBytes(lpbi)     DibWidthBytesN(lpbi, (lpbi)->biBitCount)
#define DibSizeImage(lpbi)      ((lpbi)->biSizeImage == 0 \
                                    ? ((DWORD)(UINT)DibWidthBytes(lpbi) * (DWORD)(UINT)(lpbi)->biHeight) \
                                    : (lpbi)->biSizeImage)
#define DibSize(lpbi)           ((lpbi)->biSize + (lpbi)->biSizeImage + (int)(lpbi)->biClrUsed * sizeof(RGBQUAD))
#define DibPaletteSize(lpbi)    (DibNumColors(lpbi) * sizeof(RGBQUAD))
#define DibFlipY(lpbi, y)       ((int)(lpbi)->biHeight-1-(y))
#define DibPtr(lpbi)            (LPVOID)(DibColors(lpbi) + (UINT)(lpbi)->biClrUsed)
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
                                    (lpbi)->biClrUsed = DibNumColors(lpbi);
#define DibInfo(pDIB)           ((BITMAPINFO FAR *)(pDIB))

/***************************************************************************/

#ifdef __cplusplus
}
#endif 

#endif
