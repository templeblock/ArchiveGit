#pragma once

#include "png.h"

typedef png_color PALETTE;

struct IMAGE
{
	LONG    width;
	LONG    height;
	UINT    pixdepth;
	UINT    palnum;
	BOOL    topdown;
	DWORD   rowbytes;
	DWORD   imgbytes;
	PALETTE *palette;
	BYTE    **rowptr;
	BYTE    *bmpbits;
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
	static bool IsValidPNGFile(LPCSTR strFileName);
	static HGLOBAL ReadPNG(LPCSTR strFileName);
protected:
	static HGLOBAL PNGToDIB(png_structp png_ptr, png_infop info_ptr);
	static void FreeImageStruct(IMAGE* img);
	static bool AllocImageStruct(IMAGE* img);
	static void ConvertTo4BitsPerPix(png_structp png_ptr, png_row_infop row_info, png_bytep data);
};
