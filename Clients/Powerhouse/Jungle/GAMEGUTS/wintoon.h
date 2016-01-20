/*
 * wintoon.h
 *
 * Functions, types and definitions
 *
 * Copyright (c) 1994 Microsoft Corp.  All rights reserved
 */

#ifndef _INC_WINTOON
#define _INC_WINTOON

#ifndef RC_INVOKED
#ifdef _MSC_VER
#pragma warning (disable : 4200 )	/* Don't complain about 0 sized arrays */
#endif
#endif

#ifndef _INC_WINDOWS
#include <windows.h>
#endif

#ifndef STRICT
#define STRICT 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EXPORT
#define EXPORT __export
#endif

#define WINTOON_VERSION 0x0100

typedef struct toon NEAR *HTOON;

/* FOURCC's for the VFW */

#define streamhdlrTOON mmioFOURCC('t','o','o','n')
#define streamtypeRECT mmioFOURCC('r','e','c','t')
#define streamhdlrDRTY mmioFOURCC('d','r','t','y')

#define LIST_NORMAL		0	/* Middle of the road			*/
#define LIST_SLOW		1	/* More, smaller more efficient rects	*/
#define LIST_FAST		2	/* Big rects for fast machines		*/
#define LIST_DROP		3	/* Offset for dropped frame lists	*/
#define LIST_NORMAL_DROP	(LIST_NORMAL + LIST_DROP)
#define LIST_SLOW_DROP		(LIST_SLOW + LIST_DROP)
#define LIST_FAST_DROP		(LIST_FAST + LIST_DROP)
#define C_LIST_MAX		(LIST_DROP + LIST_DROP)

#define PAINT_NONE		0
#define PAINT_MULTIPLE_BLT	1
#define PAINT_INVALIDATE_RECT	2
#define PAINT_CLIP_REGION	3
#define PAINT_ALL		4
#define PAINT_DEFAULT		5

typedef struct
{
	DWORD			cb;
	WORD			version;
	WORD		bTransparent;			/* word aligned byte */
	COLORREF		transparent;		
	WORD			cRectMax;
	short		vRectMax[C_LIST_MAX];		/* -1 if list not present */
	WORD			vDensity[C_LIST_MAX];
	WORD		vFuzziness[C_LIST_MAX];
	DWORD			vOverhead[C_LIST_MAX];
} RECT_STREAM_HEADER, FAR *LPRECT_STREAM_HEADER;

typedef struct
{
	DWORD	cb;
	DWORD	lFrame;
	short	cRect[C_LIST_MAX];			/* -1 if list not present */
	WORD	indexList[C_LIST_MAX];
	RECT	vRect[0];				/* variable size */
} RECT_STREAM_FRAME, FAR *LPRECT_STREAM_FRAME;


WORD EXPORT WINAPI ToonVersion(void);

/* Toon management */

HTOON EXPORT WINAPI ToonCreate(HWND hwnd, UINT cx, UINT cy, UINT cRect);
HTOON EXPORT WINAPI ToonFromWindow(HWND hwnd);
HWND EXPORT WINAPI ToonWindow(HTOON htoon);
void EXPORT WINAPI ToonDestroy (HTOON htoon);
BOOL EXPORT WINAPI ToonResize (HTOON htoon, unsigned cx, unsigned cy);
BOOL EXPORT WINAPI ToonSetColors(HTOON htoon, LPRGBQUAD lpColors);	/* lpColors must point to 256 RGBQUADs */
UINT EXPORT WINAPI ToonGetColors (HTOON htoon, UINT nStartIndex, UINT nColors, LPRGBQUAD lpColors);
UINT EXPORT WINAPI ToonRealizePalette(HTOON htoon);
BOOL EXPORT WINAPI ToonAnimateColors (HTOON htoon, LPRGBQUAD lpColors, UINT cStart, UINT cEntries);
BOOL EXPORT WINAPI ToonSetBackground(HTOON htoon, LPBITMAPINFOHEADER pdib, LPBYTE pbits);
void EXPORT WINAPI ToonSetMessage(HTOON htoon, UINT message);
UINT EXPORT WINAPI ToonMessage(HTOON htoon);
HDC EXPORT WINAPI ToonDC(HTOON htoon);
SIZE EXPORT WINAPI ToonSize(HTOON htoon);
BOOL EXPORT WINAPI ToonIsTopDown(HTOON htoon);

/* Dirty rect management */

BOOL EXPORT WINAPI ToonMarkRect(HTOON htoon, LPRECT lprect);
BOOL EXPORT WINAPI ToonMarkRectList(HTOON htoon, WORD numRects, LPRECT lprectList);
BOOL EXPORT WINAPI ToonClearRects(HTOON htoon);
BOOL EXPORT WINAPI ToonPaintRects(HTOON htoon);
void EXPORT WINAPI ToonPaintDC(HTOON htoon, HDC hdc);

/* Drawing and painting control */
BOOL EXPORT WINAPI ToonRestoreBackground (HTOON htoon);
LPBYTE EXPORT WINAPI ToonGetDIBPointer (HTOON hToon, LPBITMAPINFOHEADER pHeader);
BOOL EXPORT WINAPI ToonSetPaintMethod(HTOON htoon, UINT method);
int EXPORT WINAPI ToonPaintMethod(HTOON htoon);
BOOL EXPORT WINAPI ToonSetPaintList(HTOON htoon, UINT list);
int EXPORT WINAPI ToonPaintList(HTOON htoon);

/* Movie functions */

WORD EXPORT WINAPI ToonOpenMovie (HTOON htoon, LPSTR szAVIFile, BYTE transparent);
BOOL EXPORT WINAPI ToonDrawCurrentFrame (HTOON htoon);
BOOL EXPORT WINAPI ToonGetRectHeader(HTOON htoon, LPRECT_STREAM_HEADER lpHeader);

/*
 * Message Crackers
 * These functions can be used as arguments to the HANDLE_WM_* macros
 * defined in windowsx.h
 */
BOOL EXPORT WINAPI ToonOnCreate(HWND hwnd, CREATESTRUCT FAR *lpCreate);
void EXPORT WINAPI ToonOnDestroy(HWND hwnd);
int EXPORT WINAPI ToonOnPaletteChanged(HWND hwnd, HWND hwndPaletteChanged);
int EXPORT WINAPI ToonOnQueryNewPalette(HWND hwnd);
void EXPORT WINAPI ToonOnPaint(HWND hwnd);
BOOL EXPORT WINAPI ToonOnEraseBkgnd(HWND hwnd, HDC hdc);

/*
 * A replacement for DefWindowProc.	 Really just calls the above ToonOn* functions
 */
LRESULT EXPORT WINAPI ToonDefWindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

/* A message cracker for the toon message */
/* BOOL Cls_OnToonDraw(HWND hwnd, PTOON ptoon, LONG lFrame) */
#define HANDLE_WM_TOONDRAW(hwnd, wparam, lparam, fn) (((fn)(hwnd, (HTOON)wparam, (LONG)lparam)), TRUE)

#ifdef __cplusplus
}
#endif

#endif /* _INC_WINTOON */
