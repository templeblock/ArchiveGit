/*.T(32)  BMP.H  Include file for BMP support     */

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ   Copyright (c) 1993-1995, Pegasus Imaging Corp.  All rights reserved.บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#if !defined(_BMP)
#define _BMP

#ifdef __MWERKS__
	#if PRAGMA_ALIGN_SUPPORTED
	#pragma options align=packed
	#endif
#elif defined(__BORLANDC__)
    #pragma option -a1
#else /* assume microsoft or watcom */
    #if defined(__FLAT__) || defined(__WATCOMC__)
        #pragma pack(push)
    #endif
    /* microsoft 16-bit doesn't support pack(push) */
	#pragma pack(1)
#endif

#include    "stdtypes.h"

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ   The following typedef declarations are exactly as in WINDOWS.       บ
//  ณ   They will be removed if WINDOWS.H is included.                      บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
#if !defined(WINDOWS)

typedef struct {
		BYTE    peRed;
		BYTE    peGreen;
		BYTE    peBlue;
		BYTE    peFlags;
	} PALETTEENTRY;

typedef struct {
		WORD         palVersion;        // 0x310
		WORD         palNumEntries;     // total number of enttries
		PALETTEENTRY palPalEntry[1];
	} LOGPALETTE;

typedef struct {
		BYTE    rgbtBlue;
		BYTE    rgbtGreen;
		BYTE    rgbtRed;
	} RGBTRIPLE;

typedef struct {
		BYTE	rgbReserved;
		BYTE	rgbBlue;
		BYTE	rgbGreen;
		BYTE	rgbRed;
	} MACRGBQUAD;

typedef struct {
		BYTE    rgbBlue;
		BYTE    rgbGreen;
		BYTE    rgbRed;
		BYTE    rgbReserved;
	} RGBQUAD;

typedef struct {
		WORD    bfType;             // 0x4D42 = 'BM'
		DWORD   bfSize;
		WORD    bfReserved1;
		WORD    bfReserved2;
		DWORD   bfOffBits;
	} BITMAPFILEHEADER;

/* The following is only used to read old-style BMP files */
typedef struct {
		DWORD   bcSize;             // 12 = sizeof(BITMAPCOREHEADER)
		WORD    bcWidth;            // in pixels
		WORD    bcHeight;           // in pixels
		WORD    bcPlanes;           // 1
		WORD    bcBitCount;         // 1/4/8/24,  bcBitCount < 24 => RGBTRIPLEs
	} BITMAPCOREHEADER;

/* constants for the BITMAPINFOHEAER biCompression field */
#define BI_RGB      0L              // uncompressed
#define BI_RLE8     1L              //8-bit rle
#define BI_RLE4     2L              //4-bit rle

typedef struct {
		DWORD   biSize;             // 68 = sizeof(BITMAPINFOHEADER)
		LONG    biWidth;            // in pixels
		LONG    biHeight;           // in pixels, negative if top-to-bottom
		WORD    biPlanes;           // 1
		WORD    biBitCount;         // 4, 8, 16, 24 or 32
		DWORD   biCompression;      // BI_JPEG = 'JPEG' = 0x4745504A
		DWORD   biSizeImage;        // compressed image size [SOI..EOI]
		LONG    biXPelsPerMeter;    // 0 if none
		LONG    biYPelsPerMeter;    // 0 if none
		DWORD   biClrUsed;          // 2 <= ncolors <= 256  (typically 236)
		DWORD   biClrImportant;     // 0
	} BITMAPINFOHEADER;

typedef struct {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD	         bmiColors[1];
    } BITMAPINFO;

#endif

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ   The following typedefs declare Windoes extensions for BMP files.    บ
//  ณ   They are not currently defined in Windows 3.10.                     บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

typedef struct {        /* used to extend BITMAPINFOHEADER fields  */
		BITMAPINFOHEADER    bmi;
		DWORD   biExtDataOffset;    //  44 = offset of JPEGINFOHEADER
		/* Other stuff will go here */
		/* Format-specific information follows EXBMINFOHEADER */
	} EXBMINFOHEADER;

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ   This structure is used to convert to 16-bit DIBs.  Only r5:g5:b5    บ
//  ณ   is suported.                                                        บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#if 0
typedef struct {
		WORD    blue  : 5;       // 00000000 000bbbbb
		WORD    green : 5;       // 000000gg ggg00000
		WORD    red   : 5;       // 0rrrrr00 00000000
	} RGB16;

#endif

#ifdef __MWERKS__
    /*#### find out how to save/restore the alignment */
#elif defined(__BORLANDC__)
    /*#### find out how to save/restore the alignment
            "#pragma option -a" doesn't do it in BC 4.5 */
#else /* assume microsoft or watcom */
    #if defined(__FLAT__) || defined(__WATCOMC__)
        #pragma pack(pop)
    #else
        /* in MS 16-bit, best we can do is to restore to the command-line state */
        #pragma pack()
    #endif
#endif

#endif  /* #if !defined(_BMP) */

