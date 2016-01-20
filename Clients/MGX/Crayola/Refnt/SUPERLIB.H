/*=======================================================================*\

	SUPERLIB.H - SuperBlt Library include header file.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifndef __superlib
#define __superlib

#include "framelib.h"

typedef enum
	{
	PT_GRAYPALETTE,
	PT_COLORPALETTE,
	PT_CUSTOMPALETTE
	} PALETTE_TYPE, FAR *LPPALETTE_TYPE;

/*=======================================================================*\
	MAP: The map structure
\*=======================================================================*/

#define QTONEPOINTS   3
// CALPOINTS for calibration maps
#define CALPOINTS    11
// MAPPOINTS for Seps and most mapping (execept hue)
#define MAPPOINTS    11
// HUEMAPPOINTS for  hue mapping
#define HUEMAPPOINTS    13
//  MAXPOINTS Largest map 
#define MAXPOINTS    13
#define LUTSIZE     256
#define MAX8BITCOLORS 236

// bands for contrast brightness
#define 	NUM_BANDS		4
#define	BAND_ALL		0		
#define	BAND_HI			1
#define	BAND_MID		2
#define BAND_SHADOWS	3


typedef struct _map
{
	int   Reverse;        /* Does the map need a reversal during MakeMap? */
	int   Levels;         /* Number of posterization levels 0-256 */
	int   Negate;         /* Is the image negated? */
	int   Threshold;      /* The value at which image is thresheld 0-100 */
	int   Points;         /* The number of graymap points from 3-11 */
	int   Contrast;    /* The Contrast settings from -100 to +100  for all,h,m,s*/
	int   Brightness[NUM_BANDS];  /* The Brightness settings from -100 to +100 all,h,m,s*/
	int   iCurves;		// 0 for no curve interpolation
	double gamma;		// 1.0 for no gamma
	BOOL  bStretch;       /* Should the points be stretched to 0 and 100%? */
	BOOL  bModified;      /* Has the map been modified? */
	POINT Pnt[MAXPOINTS]; /* Curve break points in pixels from 0-255 */
	BYTE  Lut[LUTSIZE];   /* the lut created from the values in this table */
	BOOL  bReverseHMS;		// is the map left to right reversed for HMS? (AARGH)
} MAP, FAR *LPMAP;


// for backwards compatibilty
typedef struct _oldmap
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
} OLDMAP;
typedef OLDMAP FAR *LPOLDMAP;

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
} BMI, FAR *LPBMI;

/*=======================================================================*\
	BLTSETTINGS: The super blt settings
\*=======================================================================*/
typedef struct _bltsettings
{
	LFIXED	RGamma;
	LFIXED	GGamma;
	LFIXED	BGamma;
}
BLTSETTINGS, FAR *LPBLTSETTINGS;

/*=======================================================================*\
	LPBLTPROCESS: The super blt process proc type definition
\*=======================================================================*/

typedef void (FAR *LPBLTPROCESS)(LPTR lpSrc, LPTR lpDst, int iCount, int iDepth);

/*=======================================================================*\
	BLT: The super blt structure
\*=======================================================================*/

typedef struct _blt
{
	BMI               BitMapInfo;     	/* The DIB info structure */

	HPALETTE          hPal;         	/* Current Display Palette */
	LPTR			  lpPaletteLUT;		/* lookup table for hPal */
	PALETTE_TYPE	  PaletteType;		/* Type of palette in use */
	int				  nEntries;			/* entries in hPal */
	PALETTEENTRY      Palette[257]; 	/* palette for hPal */

	HPALETTE          hGrayPal;       	/* Handle to grayscale image palette */
	LPTR			  lpGrayPaletteLUT; /* lookup table for gray palette */
	PALETTE_TYPE	  GrayPaletteType;	/* palette type for gray palette */
	int               nGrayEntries;		/* entries in hGrayPal */
	PALETTEENTRY      GrayPalette[257]; /* palette for grayscale images */

	HPALETTE          hColorPal;      	/* Handle to color image palette */
	LPTR			  lpColorPaletteLUT;/* lookup table for color palette */
	PALETTE_TYPE	  ColorPaletteType;	/* palette type for color palette */
	int               nColorEntries;	/* entries in hColorPal */
	PALETTEENTRY      ColorPalette[257];/* palette for color images */

	/* Display convertor for FDT_LINEART images */
   void ( FAR *fpConvert0 )( LPTR, int, LPINT, LPTR, int, int,
       struct _bltsession far * );
	/* Display convertor for FDT_GRAYSCALE images */
   void ( FAR *fpConvert1 )( LPTR, int, LPINT, LPTR, int, int,
       struct _bltsession far * );
	/* Display convertor for FDT_PALETTECOLOR images */
   void ( FAR *fpConvert1P )( LPTR, int, LPINT, LPTR, int, int,
       struct _bltsession far * ); 
	/* Display convertor for FDT_RGBCOLOR images */
   void ( FAR *fpConvert3 )( struct _rgb far *, int, LPINT, LPTR, int,
       int, struct _bltsession far * );
	/* Display convertor for FDT_CMYKCOLOR images */
   void ( FAR *fpConvert4 )( struct _cmyk far *, int, LPINT, LPTR, int,
       int, struct _bltsession far * );
   /* Convertor for FDT_GRAYSCALE images with a color palette */
   void ( FAR *fpConvert1C )( LPTR, int, LPINT, LPTR, int, int,
       struct _bltsession far * );
	/* Display convertor for FDT_PALETTECOLOR images with color palette */
   void ( FAR *fpConvert1PC )( LPTR, int, LPINT, LPTR, int, int,
       struct _bltsession far * ); 
	/* Display convertor for FDT_PALETTECOLOR images with gray palette */
   void ( FAR *fpConvert1PG )( LPTR, int, LPINT, LPTR, int, int,
       struct _bltsession far * ); 
   /* Convertor for FDT_GRAYSCALE images with custom palette */
   void ( FAR *fpConvert1CP )( LPTR, int, LPINT, LPTR, int, int,
       struct _bltsession far * );
	/* Convertor for FDT_RGBCOLOR images with gray palette */
   void ( FAR *fpConvert3G )( struct _rgb far *, int, LPINT, LPTR, int,
       int, struct _bltsession far * );
	/* Convertor for FDT_RGBCOLOR images with custom palette */
   void ( FAR *fpConvert3CP )( struct _rgb far *, int, LPINT, LPTR, int,
       int, struct _bltsession far * );
	/* Convertor for FDT_CMYKCOLOR images with gray palette */
   void ( FAR *fpConvert4G )( struct _cmyk far *, int, LPINT, LPTR, int,
       int, struct _bltsession far * );
	/* Convertor for FDT_CMYKCOLOR images with custom palette */
   void ( FAR *fpConvert4CP )( struct _cmyk far *, int, LPINT, LPTR, int,
       int, struct _bltsession far * );
	LPROC             fpConvert3CPO;  /* Converter for depth 3 images with CPO palette */
	LPROC             fpConvert4CPO;  /* Converter for depth 4 images with CPO palette */
	BOOL              bNeedErrorBuf;  /* Does the converter need an error buffer */
	BOOL              bDIB;           /* Does device support DIB's? */
	LPTR              lpBltBuf;       /* 64K data buffer for blting */
	BOOL              bOptimized;     /* Do CPO display of color images */
	LPVOID            lpOptimizeData; /* Data for CPO display of color images */
	LPTR              lpDummy1;       /* Handle to CPO color lookup table */
	LPRGB             lpDummy2;       /* The RGBmap */
	LFIXED            RGamma;         /* The Red Gamma Value */
	LFIXED            GGamma;         /* The Green Gamma Value */
	LFIXED            BGamma;         /* The Blue Gamma Value */
	BYTE              RGammaLut[256]; /* The Red GammaLut */
	BYTE              GGammaLut[256]; /* The Green GammaLut */
	BYTE              BGammaLut[256]; /* The Blue GammaLut */
	int               nCalls;
	BOOL              bPaletteGamma;  /* Do gamma correction with palette */
	BOOL              bProcessData;   /* Do we want data processing ? */
	LPCOLORMAP		  lpColorMap;	  /* spare colormap for doing gamma */
									  /* with palette colored images */
#ifdef WIN32
	CRITICAL_SECTION  csSuperBlt;
#endif
} BLT, FAR *LPBLT;

/*=======================================================================*\
	BLTSESSION: The super blt session structure
\*=======================================================================*/

typedef struct _bltsession
{
	BOOL     bAllocated;     // Was the memory for this structure allocated
	BOOL     bFailure;       // A failure occured due to memory allocation
	LPBLT    lpBlt;          // The Blt device description for this session
	BMI      BitMapInfo;     // The DIB info structure
	HDC      hDC;            // Output device context
	HDC      hMemDC;         // Temporary memory device context
	HDC      hDestMemDC;     // device context for outputting to a bitmap
	HBITMAP  hBitmap;        // Temporary bitmap
	HBITMAP  hBitmapOut;     // the bitmap for outputting to a bitmap
	FRMTYPEINFO TypeInfo;         // The input data type.
	int      xBlt;           // x position of blt
	int      yBlt;           // y position of blt
	int      dxBlt;          // width in pixels of blt
	int      dyBlt;          // height in pixels of blt
	int      dxConverted;    // bytes per line of blt
	int      xDiva;          // x dithering offset
	int      yDiva;          // y dithering offset
	int      iLinesPerBlt;   // lines to build up before blting
	WORD     wUsage;         // colors usage
	BOOL     bBufAllocated;  // the Blt buffer was allocated?
	LPTR     lpBltBufStart;  // the Blt buffer
	LPTR     lpBltBuf;       // data buffer LinesPerBlt lines (working ptr)
	LPTR     lpError;        // error buffer for error diffusion converters
	void ( FAR *fpConvert )( LPTR, int, LPINT, LPTR, int, int,
       struct _bltsession FAR * );      // the data converter proc
	void ( FAR *fpMap )( LPTR, LPTR, LPTR, LPTR, LPTR, int ); // the mapping proc, for gamma correction
	LPTR     lpGammaBuf;     // buffer for our gamma correctiion
	BOOL     bOptimized;     // using CPO optimized palette
	HPALETTE hOldPal;        // The original Palette...
	LPBLTPROCESS lpProcessProc;  // Process function pointer
	LPTR     lpProcessBuf;   // buffer to store processed data
	LPCOLORMAP   lpColorMap;		/* palette for 8-bit palette colored data */
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
	FRMTYPEINFO  TypeInfo,
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

BOOL SetNewPalette(LPBLT lpBlt, PALETTE_TYPE PaletteType, HPALETTE hPal, LPTR lpPaletteLUT, LPPALETTEENTRY lpEntries, int nEntries);
BOOL SetSuperBltGamma(LPBLT lpBlt, LFIXED RGamma, LFIXED GGamma, LFIXED BGamma);
BOOL GammaAdjustPalette(LPBLT lpBlt, HPALETTE hPal, int nEntries, LPPALETTEENTRY lpEntries);
HPALETTE CreateLogicalPalette(LPRGB lpRGBmap, int nEntries);

/*=======================================================================*\
	Source file: conv4.c - some routines may be in converta.asm
\*=======================================================================*/
void Convert32to4(    LPCMYK lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert32to4MS(  LPCMYK lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert32to4MHT( LPCMYK lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert32to4S(   LPCMYK lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert24to4(    LPRGB  lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert24to4MS(  LPRGB  lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert24to4MHT( LPRGB  lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert24to4S(   LPRGB  lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8to4(     LPTR   lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8to4S(    LPTR   lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8to4HT(   LPTR   lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8Pto4(   LPTR   lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8Pto4S(   LPTR   lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8Pto4MS(   LPTR   lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8Pto4MHT(   LPTR   lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );

/*=======================================================================*\
	Source file: conv8.c - some routines may be in converta.asm
\*=======================================================================*/
void InitConvert32to8(void);
void CloseConvert32to8(void);
void Convert32to8(    LPCMYK lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert32to8G(   LPCMYK lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert32to8CP(   LPCMYK lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert24to8(  LPRGB lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert24to8G(   LPRGB  lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert24to8CP(   LPRGB  lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8to8(     LPTR   lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8to8C(    LPTR   lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8to8CP(    LPTR   lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8Pto8(     LPTR   lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8Pto8C(     LPTR   lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8Pto8G(     LPTR   lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );

/*=======================================================================*\
	Source file: conv24.c - some routines may be in converta.asm
\*=======================================================================*/
void Convert32to24(   LPCMYK lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert24to24( LPRGB lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8to24(  LPTR  lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );
void Convert8Pto24(  LPTR  lpPixel, int iCount, LPINT lpError, LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession );

/*=======================================================================*\
	Source file: makemap.c
\*=======================================================================*/
BOOL SetupHMSMaps( HINSTANCE hInst, int baseid );
void FreeupHMSMaps();
void MakeMap( LPMAP lpMap );
void InvertMap( LPMAP lpMap );
void ResetMap( LPMAP lpMap, int maxpoints, int type );
void ResetMapEx( LPMAP lpMap, int maxpoints, int type, BOOL bReverseHMS );

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
