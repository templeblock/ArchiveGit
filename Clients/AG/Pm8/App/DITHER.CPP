/*
// $Header: /PM8/App/DITHER.CPP 1     3/03/99 6:04p Gbeddow $
//
// Dither matrix routines.
//
// $Log: /PM8/App/DITHER.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     3/19/98 1:06p Hforman
// modified Dither8To8() to deal with transparency
// 
//    Rev 1.0   14 Aug 1997 15:20:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:08   Fred
// Initial revision.
// 
//    Rev 1.7   25 Jun 1997 15:16:48   Jay
// Added Dither8to8
// 
//    Rev 1.6   29 May 1997 14:06:00   Jay
// Extended the "gray hack".
// 
//    Rev 1.5   28 May 1997 09:17:14   Jay
// Catalog support. Better transparent previews.
// 
//    Rev 1.4   05 Dec 1996 12:30:06   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.3   08 Nov 1996 08:57:40   Jay
// Removed 16-bit compress.obj
// 
//    Rev 1.2   15 Oct 1996 08:54:30   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.1   07 Aug 1996 17:49:32   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:45:30   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:50   FRED
// Initial revision.
// 
//    Rev 2.2   19 Sep 1995 15:42:50   JAY
// Some changes for 23-bt version.
// 
//    Rev 2.1   05 Jun 1995 11:31:14   JAY
// Got rid of DGROUP objects.
// 
//    Rev 2.0   07 Feb 1995 15:47:40   JAY
// Initial revision.
// 
//    Rev 1.7   30 Dec 1994 13:59:56   JAY
// New dither method with expanded color set (based on WinG palette).
// 
//    Rev 1.6   30 Nov 1994 16:37:16   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.5   04 Aug 1994 07:33:54   JAY
// Large model and 32-bit changes
// 
//    Rev 1.4   25 Feb 1994 12:57:14   JAY
// Improved the "fine" dither matrix to get rid of some lineniness.
// 
//    Rev 1.3   17 Feb 1994 08:42:12   MACDUFF
// Support for Scaling access methods in redisplay context.
// 
//    Rev 1.2   28 Jan 1994 17:11:14   JAY
//  
// 
//    Rev 1.1   25 Jan 1994 15:18:46   JAY
// More
//
//   Rev 1.0   16 Dec 1993 16:25:34   JAY
// 
*/

#include "stdafx.h"
#include "redisp.h"
#include "utils.h"
#include "gif.h"		// for TRANSPARENT_INDEX
#include "dither.h"

#include <string.h>

/* from compress.asm */

BYTE * near tiff_dither_matrix = dither_16_matrix;
BYTE near gray_translate_table[256];

// Dither matrices for gray-scale translation.

// 16x16
BYTE near dither_16_matrix[] =
{
#ifdef Random
// Random!!
	245,113,156,204, 10,220, 37,176,232,108,127, 29, 78,104,190,105,
	154, 91,101, 31, 48, 80, 35, 69,173, 76, 85, 17,175,244, 92, 57,
	158,189,214, 79,124,188, 27, 62,254,248, 87,110,152,252,106,129,
	102, 66,238, 11,207,118,236, 32, 65,157,193,130, 53, 90,237,  7,
//	151, 97, 55,251, 34,206,  0,138, 52,153, 59,137,192, 44,150, 64,
	151, 97, 55,251, 34,206,  1,138, 52,153, 59,137,192, 44,150, 64,
	 33, 14,215,171,216, 54, 39,  6,  2,187, 61, 25,  1, 82,205, 94,
	167, 84,177,182,235, 36, 68, 71,255,174, 24, 49, 42,225, 70,133,
	141, 67, 18, 13, 46, 74,  5,226, 77,233,211,196,250,241,239,224,
	 98, 19,246,249, 45,212, 20,223,  8,149, 56,202,183,243, 28,123,
	100, 26, 12, 89,227,169,203, 58, 83,103, 99,160,111,180, 73,170,
	201,200,163,234, 63,197,140,146,117, 43,122,147,219, 41,120,208,
	119, 23,194, 15,155,168,162,210, 86,159,195,191,148, 95,144,186,
	247,221,172,253,181,121,199, 60, 51,178,145,  4, 75,184,179,222,
	107,128,139,165, 30,  9,115,114,218, 16,132,209, 50,242, 93, 22,
	213,126, 72, 81,217,134,185,136,229,142,112,135, 21,131, 38,166,
	  3,228, 88,161,125,198,109,231, 96,143,116,230, 40, 47,164,240
#else
//	  0, 128,  32, 160,   8, 136,  40, 168,   2, 130,  34, 162,  10, 138,  42, 170,
	  1, 128,  32, 160,   8, 136,  40, 168,   2, 130,  34, 162,  10, 138,  42, 170,
	192,  64, 224,  96, 200,  72, 232, 104, 194,  66, 226,  98, 202,  74, 234, 106,
	 48, 176,  16, 144,  56, 184,  24, 152,  50, 178,  18, 146,  58, 186,  26, 154,
	240, 112, 208,  80, 248, 120, 216,  88, 242, 114, 210,  82, 250, 122, 218,  90,
	 12, 140,  44, 172,   4, 132,  36, 164,  14, 142,  46, 174,   6, 134,  38, 166,
	204,  76, 236, 108, 196,  68, 228, 100, 206,  78, 238, 110, 198,  70, 230, 102,
	 60, 188,  28, 156,  52, 180,  20, 148,  62, 190,  30, 158,  54, 182,  22, 150,
	252, 124, 220,  92, 244, 116, 212,  84, 254, 126, 222,  94, 246, 118, 214,  86,
	  3, 131,  35, 163,  11, 139,  43, 171,   1, 129,  33, 161,   9, 137,  41, 169,
	195,  67, 227,  99, 203,  75, 235, 107, 193,  65, 225,  97, 201,  73, 233, 105,
	 51, 179,  19, 147,  59, 187,  27, 155,  49, 177,  17, 145,  57, 185,  25, 153,
	243, 115, 211,  83, 251, 123, 219,  91, 241, 113, 209,  81, 249, 121, 217,  89,
	 15, 143,  47, 175,   7, 135,  39, 167,  13, 141,  45, 173,   5, 133,  37, 165,
	207,  79, 239, 111, 199,  71, 231, 103, 205,  77, 237, 109, 197,  69, 229, 101,
	 63, 191,  31, 159,  55, 183,  23, 151,  61, 189,  29, 157,  53, 181,  21, 149,
	255, 127, 223,  95, 247, 119, 215,  87, 253, 125, 221,  93, 245, 117, 213,  85
#endif
};

BYTE near hires_dither_16_matrix[] =
{
#ifdef Random
// Random!!!

	245,113,156,204, 10,220, 37,176,232,108,127, 29, 78,104,190,105,
	154, 91,101, 31, 48, 80, 35, 69,173, 76, 85, 17,175,244, 92, 57,
	158,189,214, 79,124,188, 27, 62,254,248, 87,110,152,252,106,129,
	102, 66,238, 11,207,118,236, 32, 65,157,193,130, 53, 90,237,  7,
//	151, 97, 55,251, 34,206,  0,138, 52,153, 59,137,192, 44,150, 64,
	151, 97, 55,251, 34,206,  1,138, 52,153, 59,137,192, 44,150, 64,
	 33, 14,215,171,216, 54, 39,  6,  2,187, 61, 25,  1, 82,205, 94,
	167, 84,177,182,235, 36, 68, 71,255,174, 24, 49, 42,225, 70,133,
	141, 67, 18, 13, 46, 74,  5,226, 77,233,211,196,250,241,239,224,
	 98, 19,246,249, 45,212, 20,223,  8,149, 56,202,183,243, 28,123,
	100, 26, 12, 89,227,169,203, 58, 83,103, 99,160,111,180, 73,170,
	201,200,163,234, 63,197,140,146,117, 43,122,147,219, 41,120,208,
	119, 23,194, 15,155,168,162,210, 86,159,195,191,148, 95,144,186,
	247,221,172,253,181,121,199, 60, 51,178,145,  4, 75,184,179,222,
	107,128,139,165, 30,  9,115,114,218, 16,132,209, 50,242, 93, 22,
	213,126, 72, 81,217,134,185,136,229,142,112,135, 21,131, 38,166,
	  3,228, 88,161,125,198,109,231, 96,143,116,230, 40, 47,164,240
#else
#if 1
//	  0, 48,176,224,232,184, 56,  8, 10, 58,186,234,226,178, 50,  2,
	  1, 48,176,224,232,184, 56,  8, 10, 58,186,234,226,178, 50,  2,
	 32, 96,144,192,200,152,104, 40, 42,106,154,202,194,146, 98, 34,
	208,128,112, 80, 88,120,136,216,218,138,122, 90, 82,114,130,210,
	240,160, 64, 16, 24, 72,168,248,250,170, 74, 26, 18, 66,162,242,
	236,188, 60, 12,  4, 52,180,228,230,182, 54,  6, 14, 62,190,238,
	204,156,108, 44, 36,100,148,196,198,150,102, 38, 46,110,158,206,
	 92,124,140,220,212,132,116, 84, 86,118,134,214,222,142,126, 94,
	 28, 76,172,252,244,164, 68, 20, 22, 70,166,246,254,174, 78, 30,
	 11, 59,187,235,227,179, 51,  3,  1, 49,177,225,233,185, 57,  9,
	 43,107,155,203,195,147, 99, 35, 33, 97,145,193,201,153,105, 41,
	219,139,123, 91, 83,115,131,211,209,129,113, 81, 89,121,137,217,
	251,171, 75, 27, 19, 67,163,243,241,161, 65, 17, 25, 73,169,249,
	231,183, 55,  7, 15, 63,191,239,237,189, 61, 13,  5, 53,181,229,
	199,151,103, 39, 47,111,159,207,205,157,109, 45, 37,101,149,197,
	 87,119,135,215,223,143,127, 95, 93,125,141,221,213,133,117, 85,
	 23, 71,167,247,255,175, 79, 31, 29, 77,173,253,245,165, 69, 21
#else
	  0, 80,176,224,232,184, 88,  8, 10, 90,186,234,226,178, 82,  2,
	 32, 96,128,192,200,136,104, 40, 42,106,138,202,194,130, 98, 34,
	208,144,112, 48, 56,120,152,216,218,154,122, 58, 50,114,146,210,
	240,160, 64, 16, 24, 72,168,248,250,170, 74, 26, 18, 66,162,242,
	236,188, 92, 12,  4, 84,180,228,230,182, 86,  6, 14, 94,190,238,
	204,140,108, 44, 36,100,132,196,198,134,102, 38, 46,110,142,206,
	 60,124,156,220,212,148,116, 52, 54,118,150,214,222,158,126, 62,
	 28, 76,172,252,244,164, 68, 20, 22, 70,166,246,254,174, 78, 30,
	 11, 91,187,235,227,179, 83,  3,  1, 81,177,225,233,185, 89,  9,
	 43,107,139,203,195,131, 99, 35, 33, 97,129,193,201,137,105, 41,
	219,155,123, 59, 51,115,147,211,209,145,113, 49, 57,121,153,217,
	251,171, 75, 27, 19, 67,163,243,241,161, 65, 17, 25, 73,169,249,
	231,183, 87,  7, 15, 95,191,239,237,189, 93, 13,  5, 85,181,229,
	199,135,103, 39, 47,111,143,207,205,141,109, 45, 37,101,133,197,
	 55,119,151,215,223,159,127, 63, 61,125,157,221,213,149,117, 53,
	 23, 71,167,247,255,175, 79, 31, 29, 77,173,253,245,165, 69, 21
#endif
#endif
};

/*
// The global dither matrix.
*/

PUBLIC DITHER_MATRIX near dither_matrix;

/*
// The global dither matrix for palette drawing.
*/

PUBLIC DITHER_MATRIX near palette_dither_matrix;

/*
// In the following matrices, the entry "01" is actually the entry for
// 0. We bias it to a value of 1 in order to get pure black without having
// to tweak the code.
*/

static DITHER_MATRIX BASED_CODE dither_matrix_pool[] =
{
// Fine
	{
#if 0
		20, 53, 28, 60, 22, 55, 30, 62,
		41,  1, 33,  9, 43,  3, 35, 11,
		25, 51, 17, 57, 27, 49, 19, 59,
		45, 13, 37,  5, 47, 15, 39,  7,
		21, 52, 29, 61, 23, 54, 31, 63,
		42,  2, 34, 10, 40, 01, 32,  8,
		26, 50, 18, 58, 24, 48, 16, 56,
		46, 14, 38,  6, 44, 12, 36,  4
#else
		01, 32,  8, 40,  2, 34, 10, 42,
		48, 16, 56, 24, 50, 18, 58, 26,
		12, 44,  4, 36, 14, 46,  6, 38,
		60, 28, 52, 20, 62, 30, 54, 22,
		 3, 35, 11, 43,  1, 33,  9, 41,
		51, 19, 59, 27, 49, 17, 57, 25,
		15, 47,  7, 39, 13, 45,  5, 37,
		63, 31, 55, 23, 61, 29, 53, 21
#endif
	},
// Medium
	{
		01,  8, 32, 40,  2, 10, 34, 42,
		16, 24, 48, 56, 18, 26, 50, 58,
		38, 46,  4, 12, 36, 44,  6, 14,
		54, 62, 22, 28, 52, 60, 20, 30,
		 3, 11, 33, 43,  1,  9, 35, 41,
		19, 27, 51, 59, 17, 25, 49, 57,
		37, 45,  7, 15, 39, 47,  5, 13,
		53, 61, 23, 31, 55, 63, 21, 29
	},
// Coarse
	{
		 2, 14, 46, 58, 56, 44, 12, 01, 
		10, 26, 38, 50, 48, 36, 24,  8,
		54, 34, 30, 22, 20, 28, 32, 52,
		62, 42, 18,  6,  4, 16, 40, 60,
		57, 45, 13,  1,  3, 15, 47, 59,
		49, 37, 25,  9, 11, 27, 39, 51,
		21, 29, 33, 53, 55, 35, 31, 23,
		 5, 17, 41, 61, 63, 43, 19,  7
	}
};

/*
// In the following matrices, the entry "01" is actually the entry for
// 0. We bias it to a value of 1 in order to get pure black without having
// to tweak the code.
*/

static DITHER_MATRIX BASED_CODE palette_dither_matrix_pool[] =
{
// Fine
	{
		01, 25,  6, 31,  1, 27,  7, 33,
		38, 12, 44, 19, 39, 14, 46, 20,
		 9, 35,  3, 28, 11, 36,  4, 30,
		47, 22, 41, 15, 49, 23, 43, 17,
		 2, 27,  8, 34,  1, 26,  7, 32,
		40, 15, 47, 21, 39, 13, 45, 19,
		11, 37,  5, 31, 10, 35,  3, 29,
		50, 24, 43, 18, 48, 23, 42, 16
	}
#if 0
	,
// Medium - Not scaled yet!!!
	{
		01,  8, 32, 40,  2, 10, 34, 42,
		16, 24, 48, 56, 18, 26, 50, 58,
		38, 46,  4, 12, 36, 44,  6, 14,
		54, 62, 22, 28, 52, 60, 20, 30,
		 3, 11, 33, 43,  1,  9, 35, 41,
		19, 27, 51, 59, 17, 25, 49, 57,
		37, 45,  7, 15, 39, 47,  5, 13,
		53, 61, 23, 31, 55, 63, 21, 29
	},
// Coarse - Not scaled yet!!!
	{
		 2, 14, 46, 58, 56, 44, 12, 01, 
		10, 26, 38, 50, 48, 36, 24,  8,
		54, 34, 30, 22, 20, 28, 32, 52,
		62, 42, 18,  6,  4, 16, 40, 60,
		57, 45, 13,  1,  3, 15, 47, 59,
		49, 37, 25,  9, 11, 27, 39, 51,
		21, 29, 33, 53, 55, 35, 31, 23,
		 5, 17, 41, 61, 63, 43, 19,  7
	}
#endif
};

BYTE const near aDividedBy51Rounded[256] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
};

BYTE const near aDividedBy51[256] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 
};

BYTE const near aModulo51[256] =
{
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
  38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6,
  7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
  44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
  31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
  49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
  36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3,
  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 
};


/*----------------------------------------------------------------------------

Multiplication LUTs.  These compute 0-5 times 6 and 36.

*/

BYTE const near aTimes6[6] =
{
  0, 6, 12, 18, 24, 30
};

BYTE const near aTimes36[6] =
{
  0, 36, 72, 108, 144, 180
};

/***************************************************************************
  aWinGHalftoneTranslation

  Translates a 2.6 bit-per-pixel halftoned representation into the
  slightly rearranged WinG Halftone Palette.
*/

BYTE const near aWinGHalftoneTranslation[216] =
{
  0,
  29,
  30,
  31,
  32,
  249,
  33,
  34,
  35,
  36,
  37,
  38,
  39,
  40,
  41,
  42,
  43,
  44,
  45,
  46,
  47,
  48,
  49,
  50,
  51,
  52,
  53,
  54,
  55,
  56,
  250,
  250,
  57,
  58,
  59,
  251,
  60,
  61,
  62,
  63,
  64,
  65,
  66,
  67,
  68,
  69,
  70,
  71,
  72,
  73,
  74,
  75,
  76,
  77,
  78,
  79,
  80,
  81,
  82,
  83,
  84,
  85,
  86,
  87,
  88,
  89,
  250,
  90,
  91,
  92,
  93,
  94,
  95,
  96,
  97,
  98,
  99,
  100,
  101,
  102,
  103,
  104,
  105,
  106,
  107,
  108,
  109,
  110,
  111,
  227,
  112,
  113,
  114,
  115,
  116,
  117,
  118,
  119,
  151,
  120,
  121,
  122,
  123,
  124,
  228,
  125,
  126,
  229,
  133,
  162,
  135,
  131,
  132,
  137,
  166,
  134,
  140,
  130,
  136,
  143,
  138,
  139,
  174,
  141,
  142,
  177,
  129,
  144,
  145,
  146,
  147,
  148,
  149,
  150,
  157,
  152,
  153,
  154,
  155,
  156,
  192,
  158,
  159,
  160,
  161,
  196,
  163,
  164,
  165,
  127,
  199,
  167,
  168,
  169,
  170,
  171,
  172,
  173,
  207,
  175,
  176,
  210,
  178,
  179,
  180,
  181,
  182,
  183,
  184,
  185,
  186,
  187,
  188,
  189,
  190,
  191,
  224,
  193,
  194,
  195,
  252,
  252,
  197,
  198,
  128,
  253,
  252,
  200,
  201,
  202,
  203,
  204,
  205,
  206,
  230,
  208,
  209,
  231,
  211,
  212,
  213,
  214,
  215,
  216,
  217,
  218,
  219,
  220,
  221,
  222,
  254,
  223,
  232,
  225,
  226,
  255,
};

/*
// Build the global dither matrix based on a redisplay context.
*/

PUBLIC VOID
build_dither_matrix(RedisplayContextPtr rc)
{
	SHORT index;
	SHORT mix = midpoint(rc->x_resolution, rc->y_resolution);

	if (mix < 200
 				|| rc->IsScreen()
 				|| rc->GetScaleNumerator() != rc->GetScaleDenominator())
	{
		index = 0;
		tiff_dither_matrix = dither_16_matrix;
	}
	else if (mix < 250)
	{
		index = 1;
		tiff_dither_matrix = dither_16_matrix;
	}
	else
	{
		index = 2;
		tiff_dither_matrix = hires_dither_16_matrix;
	}

//	dither_matrix = dither_matrix_pool[index];
	memcpy(&dither_matrix, &dither_matrix_pool[index], sizeof(dither_matrix));
	memcpy(&palette_dither_matrix, &palette_dither_matrix_pool[0 /*index*/], sizeof(palette_dither_matrix));
}

void convertGrayScale(LPBYTE pInSrc, LPBYTE pInDst,
							int nSrcFactor, int nDstFactor,
 							int nBPP, int nRow,
							int nXOffset, int nWidth, BOOL fSmooth)
{
/*
// We only do 8 bit-per-pixel conversions. That's all we need now.
*/
	ASSERT(nBPP == 8);
	if (nBPP != 8)
	{
		return;
	}

	BYTE bMask = 0x0080;
	BYTE bByte = 0;

	int nCompare = nSrcFactor;
	int nDelta = nDstFactor;
	int nFraction = 0;
	int nGridX = 0;
	int nSourceOffset = 0;

/*
// Handle an offset into the source data.
*/

	LPBYTE pDst = pInDst;
	if (nXOffset != 0)
	{
	/* We need to start part way into the row. */
		nGridX = nXOffset & 0x0f;
		nSourceOffset = nXOffset*nCompare;
		nFraction = -(nSourceOffset % nDelta);
		nSourceOffset /= nDelta;
		if (nFraction == 0)
		{
			nSourceOffset--;
			nFraction = -nDelta;
		}
		nFraction += nCompare;
	}

/* Set a pointer to the starting row. */

	LPBYTE pRow = tiff_dither_matrix + ((nRow & 15) << 4);

/*
// Now branch on smoothing vs. non-smoothing.
*/
	int nSmoothSize;

	if (fSmooth && (nSmoothSize = nDelta/nCompare) >= 2)
	{
	/* Smoothing conversion. */
		LPBYTE pSrc = pInSrc + nSourceOffset;
		int nSmoothValue = (int)*pSrc;
		int nSmoothCompare = nSmoothSize;		// Common to all.

		while (nWidth > 0)
		{
			int nSmoothFraction = 0;
			int nSmoothInc = 1;
			int nSmoothFinal = (int)*pSrc++;
			int nSmoothDelta = nSmoothFinal - nSmoothValue;
			if (nSmoothDelta < 0)
			{
				nSmoothDelta = -nSmoothDelta;
				nSmoothInc = -nSmoothInc;
			}

		/*
		// Smooth this next source pixel.
		//
		// We don't have to worry about compressing (i.e. skipping source)
		// because we only smooth in the event that we are at least doubling
		// the size of the source (width-wise).
		*/

			nFraction += nDelta;
			while (nFraction >= nCompare)
			{
				nFraction -= nCompare;

			/*
			// Update the smoothed value if we are still smoothing.
			*/

				if (nSmoothDelta != 0)
				{
					nSmoothFraction += nSmoothDelta;
					while (nSmoothFraction >= nSmoothCompare)
					{
						nSmoothValue += nSmoothInc;
						nSmoothFraction -= nSmoothCompare;
					}
				/* If we reached the final value, then stop smoothing. */
					if (nSmoothValue == nSmoothFinal)
					{
						nSmoothDelta = 0;
					}
				}

			/* Set the smoothed value. */

				if (nSmoothValue >= (int)pRow[nGridX])
				{
					bByte |= bMask;
				}
				if ((bMask >>= 1) == 0)
				{
					*pDst++ = bByte;	// Store the finished byte

					bMask = 0x0080;	// Reset the mask.
					bByte = 0;			// Reset for next byte.
				}
			/* Advance the grid position. */
				nGridX = (nGridX+1) & 0x0f;
			/* One less destination bit. */
				if (--nWidth == 0)
				{
				/* All done. Break out of the loop now. */
					break;
				}
			}
		/* Set smooth value to be what we wanted the final to be. */
			nSmoothValue = nSmoothFinal;
		}
	}
	else
	{
	/* Non-smoothing conversion. */
		LPBYTE pSrc = pInSrc + nSourceOffset;
	/* Loop while we have destination bits to generate. */
		while (nWidth > 0)
		{
			nFraction += nDelta;
			if (nFraction >= nCompare)
			{
			/* This source byte is a keeper! Dither it into the destination. */

				BYTE bSrcByte = gray_translate_table[*pSrc];

				while (nFraction >= nCompare)
				{
					nFraction -= nCompare;

					if (bSrcByte >= pRow[nGridX])
					{
						bByte |= bMask;
					}
					if ((bMask >>= 1) == 0)
					{
						*pDst++ = bByte;	// Store the finished byte

						bMask = 0x0080;	// Reset the mask.
						bByte = 0;			// Reset for next byte.
					}
				/* Advance the grid position. */
					nGridX = (nGridX+1) & 0x0f;
				/* One less destination bit. */
					if (--nWidth == 0)
					{
					/* All done. Break out of the loop now. */
						break;
					}
				}
			}
			pSrc++;
		}
	}
	if (bMask != 0x0080)
	{
	/* Store the last (partial) byte. */
		*pDst = bByte;
	}
}

void xflip(LPBYTE pData, int nWidth, int nHeight, int nRowBytes)
{
	if (nWidth > 1 && nHeight != 0)
	{
		int nByteWidth = (nWidth-1) >> 3;
		BYTE bStartLMask = 0x80;
      BYTE bStartRMask = 0x80 >> ((nWidth-1) & 7);
      do
      {
			BYTE bLMask = bStartLMask;
			BYTE bRMask = bStartRMask;

			LPBYTE pLByte = pData;
			LPBYTE pRByte = pData + nByteWidth;

			for (int nBit = nWidth/2; nBit > 0; nBit--)
			{
				int nSwitch = 0;
				if (*pLByte & bLMask)
				{
					nSwitch ^= 1;
				}
				if (*pRByte & bRMask)
				{
					nSwitch ^= 1;
				}

				if (nSwitch)
				{
					*pLByte ^= bLMask;
					*pRByte ^= bRMask;
				}

				if ((bLMask >>= 1) == 0)
				{
					bLMask = 0x80;
					pLByte++;
				}
				if ((bRMask <<= 1) == 0)
				{
					bRMask = 0x01;
					pRByte--;
				}
			}
			pData += nRowBytes;
		} while (--nHeight > 0);
	}
}

void yflip(LPBYTE pData, int nWidth, int nHeight, int nRowBytes)
{
	ASSERT((nRowBytes & 1) == 0);
	/* We ignore width and just flip whole row bytes for each line. */

	if (nHeight > 1 && nRowBytes != 0)
	{
		LPBYTE pSrc = pData;
		LPBYTE pDst = pData + (nRowBytes*(nHeight-1));

		do
		{
			for (int n = 0; n < nRowBytes; n++)
			{
				BYTE b = *pSrc;
				*pSrc++ = *pDst;
				*pDst++ = b;
			}
			pDst -= 2*nRowBytes;
		} while ((nHeight -= 2) > 1);
	}
}

void Create8x8Dither(BYTE bRed, BYTE bGreen, BYTE bBlue, LPBYTE pData)
{
#if 0
	LPBYTE data = pData;
	BYTE base_red, base_green, base_blue;
	_asm
	{
		mov	al, red					; al <- red
		mov	ah, al
		rol	al, 2
		and	ax, 0x3f03
		mov	base_red, al
;;		mov	red, ah
		mov	dl, ah

		mov	al, blue								; al <- blue
		mov	ah, green							; ah <- green
		mov	bx, ax								; bl <- blue, bh <- green
		rol	ax, 2									; ah <- blue >> 6
		and	ax, 0x0303							;  al <- green >> 6
		mov	base_blue, ah
		mov	base_green, al
		and	bx, 0x3f3f
;;		mov	blue, bl
;;		mov	green, bh

	; Build the dither values matrix.

		inc	dl					; red + 1
		inc	bh					; green + 1
		inc	bl					; blue + 1

#ifdef _WIN32
		mov	esi, offset dither_matrix
		mov	edi, data

		mov	ecx, 8*8
#else
		mov	si, offset dither_matrix
		les	di, data

		mov	cx, 8*8
#endif

		d256build:

#ifdef _WIN32
			mov	dh, [esi]
			inc	esi
#else
			mov	dh, [si]
			inc	si
#endif

		;
		; Byte starts out zero!
		;
			xor	al, al

;; Note: red >= dither byte   <==>   dither_byte < red+1

		;
		; Handle red
		;

			cmp	dh, dl
			adc	al, base_red				; carry if "<"!

		; rval * 5 (so far)

			mov	ah, al
			shl	al, 2
			add	al, ah

		;
		; Handle green
		;

			cmp	dh, bh
			adc	al, base_green

		; ((rval * 5) + gval) * 5 (so far)

			mov	ah, al
			shl	al, 2
			add	al, ah

		;
		; Handle blue
		;

			cmp	dh, bl
			adc	al, base_blue

			stosb
			loop	d256build
	}
#else
	BYTE bBaseRed = aDividedBy51[bRed];
	BYTE bBaseGreen = aDividedBy51[bGreen];
	BYTE bBaseBlue = aDividedBy51[bBlue];

	BYTE bBaseIndex = aTimes36[bBaseBlue]
							+ aTimes6[bBaseGreen]
							+ bBaseRed;

	BYTE bDitherRed = aModulo51[bRed];
	BYTE bDitherGreen = aModulo51[bGreen];
	BYTE bDitherBlue = aModulo51[bBlue];

	// Start at the top; do all 64 values.

	BYTE near * pDitherByte = palette_dither_matrix[0];

	for (int i = 0; i < 8*8; i++)
	{
		BYTE bDitherValue = *pDitherByte++;
		BYTE bIndex = bBaseIndex;

		if (bDitherRed >= bDitherValue)
		{
			bIndex++;				// Increment red component.
		}
		if (bDitherGreen >= bDitherValue)
		{
			bIndex += 6;			// Increment green component.
		}
		if (bDitherBlue >= bDitherValue)
		{
			bIndex += 36;			// Increment blue component.
		}
		*pData++ = aWinGHalftoneTranslation[bIndex];
	}
#endif
}

/*
// Dither a 24-bit bitmap into an 8-bit bitmap using the PrintMaster palette.
*/

void Dither24To8(BYTE* pSource, int nSrcWidth, int nSrcHeight, BYTE* pDest, int nDstWidth, int nDstHeight, BOOL fGrayHack /*=FALSE*/)
{
	// Establish the palette.
	memcpy(&palette_dither_matrix, &palette_dither_matrix_pool[0 /*index*/], sizeof(palette_dither_matrix));

	// 24-bit source.
	int nSrcRowBytes = ((nSrcWidth*3) + 3) & ~3;
	// 8-bit destination.
	int nDstRowBytes = ((nDstWidth) + 3) & ~3;

	int nYCompare = nSrcHeight;
	int nYDelta = nDstHeight;
	int nYFraction = 0;

	int nSrcLine = 0;
	int nDstLine = 0;
#ifdef WIN32
	BYTE* pSrcRow = pSource;
	BYTE* pDstRow = pDest;
#else
	BYTE __huge* pSrcRow = pSource;
	BYTE __huge* pDstRow = pDest;
#endif

	while (nSrcLine != nSrcHeight)
	{
		ASSERT(nDstLine < nDstHeight);
		nYFraction += nYDelta;
		while (nYFraction >= nYCompare)
		{
			nYFraction -= nYCompare;

#ifdef WIN32
			BYTE* pSrcByte = pSrcRow;
			BYTE* pDstByte = pDstRow;
#else
			BYTE __huge* pSrcByte = pSrcRow;
			BYTE __huge* pDstByte = pDstRow;
#endif
			// We want to copy this row.
			int nXCompare = nSrcWidth;
			int nXDelta = nDstWidth;
			int nXFraction = 0;

			int nSrcX = 0;
			int nDstX = 0;

			BYTE *pDitherRow = palette_dither_matrix[nDstLine & 7];

			while (nSrcX < nSrcWidth)
			{
				nXFraction += nXDelta;
				if (nXFraction >= nXCompare)
				{
					// We want this pixel.
					// Unpack it.
					BYTE bRed = pSrcByte[2];
					BYTE bGreen = pSrcByte[1];
					BYTE bBlue = pSrcByte[0];

					if (fGrayHack
						 && bRed >= 188
						 && bRed <= 196
						 && bGreen >= 188
						 && bGreen <= 196
						 && bBlue >= 188
						 && bBlue <= 196)
					{
						// Copy it while we want to.
						while (nXFraction >= nXCompare)
						{
							nXFraction -= nXCompare;
							*pDstByte++ = 7;
							nDstX++;
						}
					}
					else
					{
						// Div is "plateau".
						// Mod is dither at that level.
						BYTE bRedDither = aModulo51[bRed];
						BYTE bGreenDither = aModulo51[bGreen];
						BYTE bBlueDither = aModulo51[bBlue];

						BYTE bBaseIndex = aTimes36[aDividedBy51[bBlue]]
									+ aTimes6[aDividedBy51[bGreen]]
									+ aDividedBy51[bRed];

						// Copy it while we want to.
						while (nXFraction >= nXCompare)
						{
							nXFraction -= nXCompare;

							BYTE bIndex = bBaseIndex;

						/* Get the dither entry. */

							BYTE bDitherEntry = pDitherRow[nDstX & 7];

						/* Dither all three simultaneously. */

							if (bRedDither >= bDitherEntry)
							{
								bIndex += 1;
							}
							if (bGreenDither >= bDitherEntry)
							{
								bIndex += 6;
							}
							if (bBlueDither >= bDitherEntry)
							{
								bIndex += 36;
							}

							*pDstByte++ = aWinGHalftoneTranslation[bIndex];
							nDstX++;
						}
					}
				}
				pSrcByte += 3;
				nSrcX++;
			}

			nDstLine++;
			pDstRow += nDstRowBytes;
		}
		nSrcLine++;
		pSrcRow += nSrcRowBytes;
	}
}

/*
// Dither a 8-bit bitmap into an 8-bit bitmap using the PrintMaster palette.
*/

void Dither8To8(BYTE* pSource, int nSrcWidth, int nSrcHeight,
					 BYTE* pDest, int nDstWidth, int nDstHeight,
					 RGBQUAD* pColors, BOOL fHasTransparency)
{
	// Establish the palette.
	memcpy(&palette_dither_matrix, &palette_dither_matrix_pool[0 /*index*/], sizeof(palette_dither_matrix));

	// 8-bit source.
	int nSrcRowBytes = (nSrcWidth + 3) & ~3;
	// 8-bit destination.
	int nDstRowBytes = (nDstWidth + 3) & ~3;

	int nYCompare = nSrcHeight;
	int nYDelta = nDstHeight;
	int nYFraction = 0;

	int nSrcLine = 0;
	int nDstLine = 0;
	BYTE* pSrcRow = pSource;
	BYTE* pDstRow = pDest;

	while (nSrcLine != nSrcHeight)
	{
		ASSERT(nDstLine < nDstHeight);
		nYFraction += nYDelta;
		while (nYFraction >= nYCompare)
		{
			nYFraction -= nYCompare;

			BYTE* pSrcByte = pSrcRow;
			BYTE* pDstByte = pDstRow;

			// We want to copy this row.
			int nXCompare = nSrcWidth;
			int nXDelta = nDstWidth;
			int nXFraction = 0;

			int nSrcX = 0;
			int nDstX = 0;

			BYTE *pDitherRow = palette_dither_matrix[nDstLine & 7];

			while (nSrcX < nSrcWidth)
			{
				nXFraction += nXDelta;
				if (nXFraction >= nXCompare)
				{
					// check if this is a transparent pixel
					if (fHasTransparency && *pSrcByte == 255)
					{
						// Copy it while we want to.
						while (nXFraction >= nXCompare)
						{
							nXFraction -= nXCompare;
							*pDstByte++ = TRANSPARENT_INDEX;
							nDstX++;
						}
					}
					else
					{
						// We want this pixel.
						// Unpack it.
						RGBQUAD c = pColors[*pSrcByte];
						BYTE bRed = c.rgbRed;
						BYTE bGreen = c.rgbGreen;
						BYTE bBlue = c.rgbBlue;

						// Div is "plateau".
						// Mod is dither at that level.
						BYTE bRedDither = aModulo51[bRed];
						BYTE bGreenDither = aModulo51[bGreen];
						BYTE bBlueDither = aModulo51[bBlue];

						BYTE bBaseIndex = aTimes36[aDividedBy51[bBlue]]
									+ aTimes6[aDividedBy51[bGreen]]
									+ aDividedBy51[bRed];

						// Copy it while we want to.
						while (nXFraction >= nXCompare)
						{
							nXFraction -= nXCompare;

							BYTE bIndex = bBaseIndex;

						/* Get the dither entry. */

							BYTE bDitherEntry = pDitherRow[nDstX & 7];

						/* Dither all three simultaneously. */

							if (bRedDither >= bDitherEntry)
							{
								bIndex += 1;
							}
							if (bGreenDither >= bDitherEntry)
							{
								bIndex += 6;
							}
							if (bBlueDither >= bDitherEntry)
							{
								bIndex += 36;
							}

							*pDstByte++ = aWinGHalftoneTranslation[bIndex];
							nDstX++;
						}
					}
				}
				pSrcByte++;
				nSrcX++;
			}

			nDstLine++;
			pDstRow += nDstRowBytes;
		}
		nSrcLine++;
		pSrcRow += nSrcRowBytes;
	}
}

static void ErrDiff24To8Row(BYTE* pSource, BYTE* pDest, int nSrcWidth, int nDstWidth, int nLine, SHORT* pErrors)
{
/* Initialize the DDA variables. */

	int nXDelta = nDstWidth;
	int nXCompare = nSrcWidth;
	int nXFraction = 0;

	SHORT* e = pErrors+3;

#ifdef WIN32
	LPBYTE s = pSource;
	LPBYTE d = pDest;
#else
	BYTE __huge* s = pSource;
	BYTE __huge* d = pDest;
#endif

/* Initialize the pointers for the error buffers. */

	if (nLine & 1)
	{
		e += (nDstWidth-1)*3;
		d += nDstWidth-1;

		int nOffset = scale_number((nDstWidth-1)+1, nSrcWidth, nDstWidth);
		if ((nXFraction = nXDelta - mulrem((nDstWidth-1)+1, nSrcWidth, nDstWidth)) == nDstWidth)
		{
			nXFraction = 0;
			nOffset--;
		}

#ifdef WIN32
		s = pSource+nOffset*3;
#else
		s = ((BYTE __huge*)pSource)+nOffset*3;
#endif

	/* Reset and recalc the destination variables. */

		SHORT nRedError = e[0];
		SHORT nGreenError = e[1];
		SHORT nBlueError = e[2];

		e[0] = e[1] = e[2] = 0;

		for (int nCount = nDstWidth; nCount > 0; )
		{
			/* This byte is a keeper. */
			/* Get the current byte and translate it to its correct value. */

			// Compute the translated, error corrected value.

			SHORT nRed = s[2] + nRedError;
			SHORT nGreen = s[1] + nGreenError;
			SHORT nBlue = s[0] + nBlueError;

			SHORT nNewRed = nRed;
			if (nNewRed <= 0) nNewRed = 0;
			else if (nNewRed > 255)	nNewRed = 255;
			else nNewRed -= aModulo51[nNewRed];

			SHORT nNewGreen = nGreen;
			if (nNewGreen <= 0) nNewGreen = 0;
			else if (nNewGreen > 255) nNewGreen = 255;
			else nNewGreen -= aModulo51[nNewGreen];

			SHORT nNewBlue = nBlue;
			if (nNewBlue <= 0) nNewBlue = 0;
			else if (nNewBlue > 255) nNewBlue = 255;
			else nNewBlue -= aModulo51[nNewBlue];

			// Compute the new error.

			nRedError = nRed - nNewRed;
			nGreenError = nGreen - nNewGreen;
			nBlueError = nBlue - nNewBlue;

			// Store the destination value.

			BYTE bIndex = aTimes36[aDividedBy51Rounded[nNewBlue]]
								+ aTimes6[aDividedBy51Rounded[nNewGreen]]
								+ aDividedBy51Rounded[nNewRed];
			*d-- = aWinGHalftoneTranslation[bIndex];

			// Distribute the error in the correct directions.

			int nAlpha, nBeta, nGamma, nDelta;
			nDelta = nRedError;
			nDelta -= (nAlpha = (7*nRedError) >> 4);
			nDelta -= (nBeta  = (3*nRedError) >> 4);
			nDelta -= (nGamma = (5*nRedError) >> 4);

		/*
		// e[1] += alpha
		// ne[-1] += beta;
		// ne[0] += gamma;
		// ne[1] += delta;
		*/

			nRedError = e[0-1*3] + nAlpha;				/* for next time */
			e[0+1*3] += nBeta;
			e[0+0*3] += nGamma;
			e[0-1*3] = nDelta;

			nDelta = nGreenError;
			nDelta -= (nAlpha = (7*nGreenError) >> 4);
			nDelta -= (nBeta  = (3*nGreenError) >> 4);
			nDelta -= (nGamma = (5*nGreenError) >> 4);

		/*
		// e[1] += alpha
		// ne[-1] += beta;
		// ne[0] += gamma;
		// ne[1] += delta;
		*/

			nGreenError = e[1-1*3] + nAlpha;				/* for next time */
			e[1+1*3] += nBeta;
			e[1+0*3] += nGamma;
			e[1-1*3] = nDelta;

			nDelta = nBlueError;
			nDelta -= (nAlpha = (7*nBlueError) >> 4);
			nDelta -= (nBeta  = (3*nBlueError) >> 4);
			nDelta -= (nGamma = (5*nBlueError) >> 4);

		/*
		// e[1] += alpha
		// ne[-1] += beta;
		// ne[0] += gamma;
		// ne[1] += delta;
		*/

			nBlueError = e[2-1*3] + nAlpha;				/* for next time */
			e[2+1*3] += nBeta;
			e[2+0*3] += nGamma;
			e[2-1*3] = nDelta;

			e -= 3;

			nXFraction += nXCompare;
			while (nXFraction >= nXDelta)
			{
				nXFraction -= nXDelta;
				s -= 3;
			}

			if (--nCount == 0)
			{
				break;
			}
		}
	}
	else
	{
		SHORT nRedError = e[0];
		SHORT nGreenError = e[1];
		SHORT nBlueError = e[2];

		e[0] = e[1] = e[2] = 0;

		for (int nCount = nDstWidth; nCount > 0; )
		{
			if ((nXFraction += nXDelta) >= nXCompare)
			{
			/* This byte is a keeper. */
			/* Get the current byte and translate it to its correct value. */
			/* Compute the translated, error corrected value. */

				while (nXFraction >= nXCompare)
				{
					nXFraction -= nXCompare;

					SHORT nRed = s[2] + nRedError;
					SHORT nGreen = s[1] + nGreenError;
					SHORT nBlue = s[0] + nBlueError;

					SHORT nNewRed = nRed;
					if (nNewRed <= 0) nNewRed = 0;
					else if (nNewRed > 255)	nNewRed = 255;
					else nNewRed -= aModulo51[nNewRed];

					SHORT nNewGreen = nGreen;
					if (nNewGreen <= 0) nNewGreen = 0;
					else if (nNewGreen > 255) nNewGreen = 255;
					else nNewGreen -= aModulo51[nNewGreen];

					SHORT nNewBlue = nBlue;
					if (nNewBlue <= 0) nNewBlue = 0;
					else if (nNewBlue > 255) nNewBlue = 255;
					else nNewBlue -= aModulo51[nNewBlue];

					// Compute the new error.

					nRedError = nRed - nNewRed;
					nGreenError = nGreen - nNewGreen;
					nBlueError = nBlue - nNewBlue;

					// Store the destination value.

					BYTE bIndex = aTimes36[aDividedBy51Rounded[nNewBlue]]
									+ aTimes6[aDividedBy51Rounded[nNewGreen]]
									+ aDividedBy51Rounded[nNewRed];
					*d++ = aWinGHalftoneTranslation[bIndex];

					// Distribute the error in the correct directions.

					int nAlpha, nBeta, nGamma, nDelta;

					nDelta = nRedError;
					nDelta -= (nAlpha = (7*nRedError) >> 4);
					nDelta -= (nBeta  = (3*nRedError) >> 4);
					nDelta -= (nGamma = (5*nRedError) >> 4);

				/*
				// e[1] += alpha
				// ne[-1] += beta;
				// ne[0] += gamma;
				// ne[1] += delta;
				*/

					nRedError = e[0+1*3] + nAlpha;		/* for next time */
					e[0-1*3] += nBeta;
					e[0+0*3] += nGamma;
					e[0+1*3] = nDelta;

					nDelta = nGreenError;
					nDelta -= (nAlpha = (7*nGreenError) >> 4);
					nDelta -= (nBeta  = (3*nGreenError) >> 4);
					nDelta -= (nGamma = (5*nGreenError) >> 4);

				/*
				// e[1] += alpha
				// ne[-1] += beta;
				// ne[0] += gamma;
				// ne[1] += delta;
				*/

					nGreenError = e[1+1*3] + nAlpha;				/* for next time */
					e[1-1*3] += nBeta;
					e[1+0*3] += nGamma;
					e[1+1*3] = nDelta;

					nDelta = nBlueError;
					nDelta -= (nAlpha = (7*nBlueError) >> 4);
					nDelta -= (nBeta  = (3*nBlueError) >> 4);
					nDelta -= (nGamma = (5*nBlueError) >> 4);

				/*
				// e[1] += alpha
				// ne[-1] += beta;
				// ne[0] += gamma;
				// ne[1] += delta;
				*/

					nBlueError = e[2+1*3] + nAlpha;				/* for next time */
					e[2-1*3] += nBeta;
					e[2+0*3] += nGamma;
					e[2+1*3] = nDelta;

					e += 3;

					if (--nCount == 0)
					{
						break;
					}
				}
			}
			s += 3;
		}
	}
}

/*
// Error diffuse a 24-bit bitmap into an 8-bit bitmap using the PrintMaster palette.
*/

void ErrDiff24To8(BYTE* pSource, int nSrcWidth, int nSrcHeight, BYTE* pDest, int nDstWidth, int nDstHeight)
{
	// Establish the palette.
	memcpy(&palette_dither_matrix, &palette_dither_matrix_pool[0 /*index*/], sizeof(palette_dither_matrix));

	// 24-bit source.
	int nSrcRowBytes = ((nSrcWidth*3) + 3) & ~3;
	// 8-bit destination.
	int nDstRowBytes = ((nDstWidth) + 3) & ~3;

	int nYCompare = nSrcHeight;
	int nYDelta = nDstHeight;
	int nYFraction = 0;

	int nSrcLine = 0;
	int nDstLine = 0;
#ifdef WIN32
	BYTE* pSrcRow = pSource;
	BYTE* pDstRow = pDest;
#else
	BYTE __huge* pSrcRow = pSource;
	BYTE __huge* pDstRow = pDest;
#endif
	int nErrors = (nDstWidth+4)*3;
	SHORT* pErrors = new SHORT[nErrors];
	memset(pErrors, 0, sizeof(SHORT)*nErrors);

	while (nSrcLine != nSrcHeight)
	{
		ASSERT(nDstLine < nDstHeight);
		nYFraction += nYDelta;
		while (nYFraction >= nYCompare)
		{
			nYFraction -= nYCompare;

			ErrDiff24To8Row(pSrcRow, pDstRow, nSrcWidth, nDstWidth, nDstLine, pErrors);

			nDstLine++;
			pDstRow += nDstRowBytes;
		}
		nSrcLine++;
		pSrcRow += nSrcRowBytes;
	}

	delete [] pErrors;
}
