/*=======================================================================*\

	SUPERLIB.H - SuperBlt Library include header file.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifndef __super_h_
#define __super_h_

#include "frame.h"

/********************************************************************* 
 **** MAP: The map structure
 *********************************************************************/

#define MAXPOINTS 12
#define LUTSIZE 256
#define CALPOINTS 11
#define MAPPOINTS 11
#define MINPOINTS 3

typedef struct _map
{
int	Reverse;	/* Does the map need a reversal during MakeMap? */
int	Levels;		/* Number of posterization levels 0-256 */
int	Negate;		/* Is the image negated? */
int	Threshold;	/* The value at which image is thresheld 0-100 */
int	Points;		/* The number of graymap points from 3-11 */
int	Contrast;	/* The Contrast setting from -100 to +100 */
int	Brightness;	/* The Brightness setting from -100 to +100 */
BOOL	bStretch;	/* Should the points be stretched to 0 and 100%? */
BOOL	bModified;	/* Has the map been modified? */
POINT	Pnt[MAXPOINTS]; /* Curve break points in pixels from 0-255 */
BYTE	Lut[LUTSIZE];	/* the lut created from the values in this table */
} MAP;
typedef MAP _far *LPMAP;

/*=======================================================================*\
	BMI: (BITMAPINFO) The Superblt/DIB structure
\*=======================================================================*/

typedef struct _bmi
{
	BITMAPINFOHEADER bmiHeader;
	union {
		RGBQUAD Colors[256];
		WORD    Index [256];
	} bmi;
} BMI, far *LPBMI;

/*=======================================================================*\
	BLTSETTINGS: The super blt settings
\*=======================================================================*/

typedef struct _bltsettings
{
	LFIXED	RGamma;
	LFIXED	GGamma;
	LFIXED	BGamma;
} BLTSETTINGS, far *LPBLTSETTINGS;

/*=======================================================================*\
	BLT: The super blt structure
\*=======================================================================*/

typedef struct _blt
{
	BMI	BitMapInfo;	/* The DIB info structure */
	WORD	hGrayPal;	/* Handle to a Windows palette */
	WORD	hColorPal;	/* Handle to a Windows palette */
	LVPROC	fpConvert1;	/* Display convertor for depth 1 images */
	LVPROC	fpConvert2;	/* Display convertor for depth 2 images */
	LVPROC	fpConvert3;	/* Display convertor for depth 3 images */
	LVPROC	fpConvert4;	/* Display convertor for depth 4 images */
	LVPROC	fpConvert1C;	/* Convertor for depth 1 images with color palette */
	LVPROC	fpConvert2G;	/* Convertor for depth 2 images with gray palette */
	LVPROC	fpConvert3G;	/* Convertor for depth 3 images with gray palette */
	LVPROC	fpConvert4G;	/* Convertor for depth 3 images with gray palette */
	LVPROC fpConvert3CPO;  /* Converter for depth 3 images with CPO palette */
	LVPROC fpConvert4CPO;  /* Converter for depth 4 images with CPO palette */
	BOOL	bNeedErrorBuf;  /* Does the converter need an error buffer */
	HPALETTE hPal;		/* Current Display Palette */
	BOOL	bDIB;		/* Does device support DIB's? */
	LPTR	lpBltBuf;	/* 64K data buffer for blting */
	BOOL	bOptimized;	/* Do CPO display of color images */
	LPTR	lpDummy1;	/* Handle to CPO color lookup table */
	LPRGB	lpDummy2;	/* The RGBmap */
	LFIXED	RGamma;		/* The Red Gamma Value */
	LFIXED	GGamma;		/* The Green Gamma Value */
	LFIXED	BGamma;		/* The Blue Gamma Value */
	BYTE	RGammaLut[256];	/* The Red GammaLut */
	BYTE	GGammaLut[256];	/* The Green GammaLut */
	BYTE	BGammaLut[256];	/* The Blue GammaLut */
	int	nGrayEntries;
	PALETTEENTRY GrayPalette[257]; /* palette for grayscale images */
	int	nColorEntries;
	PALETTEENTRY ColorPalette[257]; /* palette for color images */
	BOOL	bPaletteGamma;  /* Do gamma correction with palette */
} BLT, far *LPBLT;

/*=======================================================================*\
	BLTSESSION: The super blt session structure
\*=======================================================================*/

typedef struct _bltsession
{
	BOOL		bAllocated;	// Was the memory for this structure allocated
	BOOL		bFailure;	// A failure occured due to memory allocation
	LPBLT		lpBlt;		// The Blt device description for this session
	BMI		BitMapInfo;	/* The DIB info structure */
	HDC		hDC;		// Output device context
	HDC		hMemDC;		// Temporary memory device context
	HDC		hDestMemDC;	// device context for outputting to a bitmap
	HBITMAP		hBitmap;	// Temporary bitmap
	HBITMAP		hBitmapOut;	// the bitmap for outputting to a bitmap
	int		xBlt;		// x position of blt
	int		yBlt;		// y position of blt
	int		dxBlt;		// width in pixels of blt
	int		dyBlt;		// height in pixels of blt
	int		dxConverted;	// bytes per line of blt
	int		xDiva;		// x dithering offset
	int		yDiva;		// y dithering offset
	int		iLinesPerBlt;	// lines to build up before blting
	WORD		wUsage;		// colors usage
	BOOL		bBufAllocated;	// the Blt buffer was allocated?
	LPTR		lpBltBufStart;	// the Blt buffer
	LPTR		lpBltBuf;	// data buffer LinesPerBlt lines (working ptr)
	LPTR		lpError;	// error buffer for error diffusion converters
	LPTR		lpGammaBuf;	// buffer to store gamma corrected data
	LVPROC		fpConvert;	// the data converter proc
	LVPROC		fpMap;		// the mapping proc, for gamma correction
	BOOL		bOptimized;	// using CPO optimized palette
	HPALETTE    hOldPal; // The original Palette...
} BLTSESSION, far *LPBLTSESSION;

/*========================================*\
	C++ Garbage...
\*========================================*/
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*=======================================================================*\
	Source file: superblt.c
\*=======================================================================*/

LPBLT InitSuperBlt(
	HDC  hDC,
	int  nTotalBits,
	LPTR lpBltBuf,
	BOOL bPaletteGamma,
	BOOL bProcessData );

void EndSuperBlt( LPBLT lpBlt );

LPBLTSESSION StartSuperBlt(
	LPBLTSESSION lpSession,
	HDC          hDC,
	HBITMAP      hDestBitmap,
	LPBLT        lpBltDevice,
	LPRECT       lpRect,
	int          Depth,
	int          MaxLines,
	int          xDivaStart,
	int          yDivaStart,
	BOOL         fDoGamma,
	LPBLTPROCESS lpProcessProc );

void SuperBlt( LPBLTSESSION lpSession, LPTR lp );

void SuperFill(
	HDC    hDC,
	LPBLT  lpBltDevice,
	LPRECT lpRect,
	int    xDivaStart,
	int    yDivaStart,
	COLOR  Color,
	LPBLTPROCESS lpProcessProc);

BOOL SetSuperBltGamma(LPBLT lpBlt, LFIXED RGamma, LFIXED GGamma, LFIXED BGamma);
BOOL GammaAdjustPalette(LPBLT lpBlt, HPALETTE hPal, int nEntries, LPPALETTEENTRY lpEntries);

/*=======================================================================*\
	Source file: convert.c
\*=======================================================================*/

void Convert8to4(     LPTR lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert8to4S(    LPTR lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert8to4HT(   LPTR lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert8to8(     LPTR lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert8to8C(    LPTR lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );

void Convert16to4(    LPWORD lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert16to4S(   LPWORD lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert16to4MS(  LPWORD lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert16to4MHT( LPWORD lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert16to8(    LPWORD lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert16to8G(   LPWORD lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert16to24(   LPWORD lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );

void Convert24to4(    LPRGB lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert24to4MS(  LPRGB lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert24to4MHT( LPRGB lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert24to4S(   LPRGB lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert24to8G(   LPRGB lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert24to8CPO( LPRGB lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );

/*=======================================================================*\
	Source file: conv32.c
\*=======================================================================*/

void InitConvert32to8(void);
void CloseConvert32to8(void);
void Convert32to4(    LPCMYK lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert32to4MS(  LPCMYK lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert32to4MHT( LPCMYK lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert32to4S(   LPCMYK lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert32to8(    LPCMYK lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert32to8G(   LPCMYK lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert32to8CPO( LPCMYK lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert32to24(   LPCMYK lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );

/*=======================================================================*\
	Source file: converta.asm
\*=======================================================================*/

void Convert24to24( LPRGB lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert24to8( LPRGB lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert24to4( LPRGB lpPixel, int iCount, register LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );
void Convert8to24( LPTR lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLT lpBlt );

/*=======================================================================*\
	Source file: makemap.c
\*=======================================================================*/

void MakeMap( LPMAP lpMap );
void InvertMap( LPMAP lpMap );
void ResetMap( LPMAP lpMap, int maxpoints, int type );

/*=======================================================================*\
	Source file: gamma.c
\*=======================================================================*/

void BuildGammaLut(LFIXED Gamma, LPTR GammaLut);
void CreateGammaMap(LPMAP lpMap, LFIXED fGamma );

/*========================================*\
	C++ Garbage...
\*========================================*/
#ifdef __cplusplus
}
#endif // __cplusplus

#endif
