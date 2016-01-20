/*************************************************************************
	dibfx.h

	Header file for various DIB-to-DIB effects

	02/08/94        Compiled by Jonbl
*/

#ifndef _INC_DIBFX
#define _INC_DIBFX

#ifndef _INC_WINDOWS
#include <windows.h>
#include <windowsx.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
	Many of the DIBFX functions use fixed point calculations internally.

	These functions are defined in fixed32.asm
*/

typedef long Fixed;

Fixed WINAPI FixedMultiply( Fixed Multiplicand,
	Fixed Multiplier );

Fixed WINAPI FixedDivide( Fixed Dividend, Fixed Divisor );

#define IntToFixed(i) (Fixed)( ((long)(i)) << 16 )
#define FixedToInt(f) (int)( ((long)f) >> 16 )



/*
 *      DibClear
 *      Fills a DIB's memory with a given value, effectively clearing
 *      an 8-bit, 4-bit or 1-bit DIB.
 *
 *      Does not do what you would expect on 16, 24, or 32-bit DIBs,
 *      but it will work.
 *
 *      Source is in clear.c and clear32.asm
 */

typedef LPBITMAPINFOHEADER PDIBFX;

BOOL WINAPI DibClear(PDIBFX pdib, LPVOID lpDst, BYTE value);


//
// 32 bit fast memory copy (faster than hmemcpy)
//
void WINAPI CopyBytes(LPBYTE pDst, LPBYTE pSrc, DWORD cb);

//
// DibBitBlt
//
// operation:
//	0-255			Blt with transparency
//	256	DIB_SRCCOPY	DIBCOPY	Src copy
//
BOOL WINAPI DibBlt(PDIBFX pdibDst, LPBYTE pbitsDst, int xDst, int yDst,
		PDIBFX pdibSrc, LPBYTE pbitsSrc, int xSrc, int ySrc, 
		int cx, int cy);

BOOL WINAPI DibTransparentBlt(PDIBFX pdibDst, LPBYTE pbitsDst, int xDst, int yDst,
		PDIBFX pdibSrc, LPBYTE pbitsSrc, int xSrc, int ySrc,
		int cx, int cy, BYTE transparent);

/*
 *      DibStretch
 *      Stretches one DIB or a portion thereof into another DIB.
 *
 *      DibStretch will work on any two DIBs as long as they are both of
 *      the same depth and format.
 *
 *      Source is in stretch.c and stret32.asm
 */

BOOL WINAPI DibStretch(PDIBFX pdibDst, LPVOID lpDst,
	int DstX, int DstY, int DstXE, int DstYE,
	PDIBFX pdibSrc, LPVOID lpSrc, 
	int SrcX, int SrcY, int SrcXE, int SrcYE);

/*
 *      DibTransparentStretch
 *      Stretches one 8-bit DIB or a portion thereof into another 8-bit
 *      DIB, ignoring one color.
 *
 *      Also, both DIBs are assumed to have the same color table.
 *
 *      It would be better if TransparentColor were a COLORREF.
 *
 *      Source is in tstretch.c and tstret32.asm
 */

BOOL WINAPI DibTransparentStretch(PDIBFX pdibDst, LPVOID lpDst,
	int DstX, int DstY, int DstXE, int DstYE,
	PDIBFX pdibSrc, LPVOID lpSrc,
	int SrcX, int SrcY, int SrcXE, int SrcYE,
	BYTE TransparentColor);

/*
 *      DibWarp
 *      Warps an arbitrary polygonal region of one 8-bit DIB into
 *      an arbitrary region of another 8-bit DIB.
 *
 *      Also, both DIBs are assumed to have the same color table.
 *
 *      Source is in warp.c and warp32.asm
 */

BOOL WINAPI DibWarp(PDIBFX pdibDst, LPVOID lpDst,
	POINT FAR *DstPoints,
	PDIBFX pdibSrc, LPVOID lpSrc,
	POINT FAR *SrcPoints,
	int nPoints);


/*
 *      DibTransparentWarp
 *      Warps an arbitrary polygonal region of one 8-bit DIB into
 *      an arbitrary region of another 8-bit DIB with one color
 *      transparent.
 *
 *      Also, both DIBs are assumed to have the same color table.
 *
 *      Source is in warp.c and warp32.asm
 */

BOOL WINAPI DibTransparentWarp(PDIBFX pdibDst, LPVOID lpDst,
	POINT FAR *DstPoints,
	PDIBFX pdibSrc, LPVOID lpSrc,
	POINT FAR *SrcPoints,
	int nPoints, BYTE TransparentColor);

#ifdef __cplusplus
}
#endif

#endif // _INC_DIBFX

