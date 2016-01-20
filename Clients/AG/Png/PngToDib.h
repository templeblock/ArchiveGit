#pragma once

#include <atlbase.h>
// ATL/WTL classes in the shared folder
#include <atlapp.h>
#include <atltypes.h>
#define _WTL_NO_WTYPES
#include <atlmisc.h>	// CSize, CPoint, CRect, CString
#include "png.h"

typedef png_color PALETTE;

struct IMAGE
{
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD dwResolution;
	WORD wBitDepth;
	DWORD dwPaletteSize;
	DWORD dwRowBytes;
	DWORD dwImgBytes;
	PALETTE* pPalette;
	BYTE** ppRow;
	BYTE* pBits;
};

/* RGBQUAD */
#define RGBQ_BLUE			0		/* BYTE   rgbBlue;     */
#define RGBQ_GREEN			1		/* BYTE   rgbGreen;    */
#define RGBQ_RED			2		/* BYTE   rgbRed;      */
#define RGBQ_RESERVED		3		/* BYTE   rgbReserved; */
#define RGBQUAD_SIZE		4		/* sizeof(RGBQUAD)     */

#define PNG_BYTES_TO_CHECK 4

class CPng
{
public:
	static COLORREF GetPngTransparentColor(LPCSTR strFileName, bool bDefaultToUpperLeft);
	static bool IsValidPNGFile(LPCSTR strFileName);
	static HGLOBAL ReadPNG(LPCSTR strFileName, bool bDefaultToUpperLeft);
protected:
	static HGLOBAL PNGToDIB(png_structp png_ptr, png_infop info_ptr, bool bDefaultToUpperLeft);
	static void FreeImageStruct(const IMAGE& Image);
	static bool AllocImageStruct(IMAGE& Image);
	static void ConvertTo4BitsPerPix(png_structp png_ptr, png_row_infop row_info, png_bytep data);
};
