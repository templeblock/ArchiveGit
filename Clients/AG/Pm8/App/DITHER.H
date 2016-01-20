/*
// $Header: /PM8/App/DITHER.H 1     3/03/99 6:04p Gbeddow $
//
// Dither matrix definitions.
//
// $Log: /PM8/App/DITHER.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     3/19/98 1:06p Hforman
// modified Dither8To8() to deal with transparency
// 
//    Rev 1.0   14 Aug 1997 15:20:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:34   Fred
// Initial revision.
// 
//    Rev 1.5   25 Jun 1997 15:16:46   Jay
// Added Dither8to8
// 
//    Rev 1.4   28 May 1997 09:17:14   Jay
// Catalog support. Better transparent previews.
// 
//    Rev 1.3   05 Dec 1996 12:30:30   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.2   08 Nov 1996 08:57:48   Jay
// Removed 16-bit compress.obj
// 
//    Rev 1.1   07 Aug 1996 17:49:34   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:42:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:06   FRED
// Initial revision.
// 
//    Rev 2.1   03 May 1995 10:39:40   FRED
//  
// 
//    Rev 2.0   07 Feb 1995 15:48:28   JAY
// Initial revision.
// 
//    Rev 1.3   30 Dec 1994 13:59:42   JAY
// New dither method with expanded color set (based on WinG palette).
// 
//    Rev 1.2   04 Aug 1994 07:33:40   JAY
// Large model and 32-bit changes
// 
//    Rev 1.1   25 Jan 1994 15:18:20   JAY
// More
// 
//    Rev 1.0   16 Dec 1993 16:26:04   JAY
//  
*/

#ifndef __DITHER_H__
#define __DITHER_H__

#include "redisp.h"

typedef BYTE DITHER_MATRIX[8][8];

extern VOID build_dither_matrix(RedisplayContextPtr rc);

extern DITHER_MATRIX near dither_matrix;
extern DITHER_MATRIX near palette_dither_matrix;

extern BYTE * near tiff_dither_matrix;

extern BYTE near dither_16_matrix[], near hires_dither_16_matrix[];
extern BYTE near gray_translate_table[256];

extern VOID convertGrayScale(LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
 							int bpp, int row,
							int x_offset, int width, BOOL smooth);

extern VOID xflip(LPBYTE, int width, int height, int row_bytes);
extern VOID yflip(LPBYTE, int width, int height, int row_bytes);

extern BYTE const near aDividedBy51Rounded[256];
extern BYTE const near aDividedBy51[256];
extern BYTE const near aModulo51[256];
extern BYTE const near aTimes6[6];
extern BYTE const near aTimes36[6];
extern BYTE const near aWinGHalftoneTranslation[216];

extern void Create8x8Dither(BYTE bRed, BYTE bGreen, BYTE bBlue, LPBYTE pData);

extern void Dither24To8(BYTE* pSource, int nSrcWidth, int nSrcHeight, BYTE* pDest, int nDstWidth, int nDstHeight, BOOL fGrayHack = FALSE);
extern void Dither8To8(BYTE* pSource, int nSrcWidth, int nSrcHeight, BYTE* pDest, int nDstWidth, int nDstHeight, RGBQUAD* pColors, BOOL fHasTransparency);
extern void ErrDiff24To8(BYTE* pSource, int nSrcWidth, int nSrcHeight, BYTE* pDest, int nDstWidth, int nDstHeight);

#endif					// #ifndef __DITHER_H__
