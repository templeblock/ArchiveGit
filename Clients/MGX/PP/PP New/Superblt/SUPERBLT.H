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
#include "types.h"

typedef enum
	{
	PT_GRAYPALETTE,
	PT_COLORPALETTE,
	PT_CUSTOMPALETTE
	} PALETTE_TYPE, FAR *LPPALETTE_TYPE;

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
	LPBLTPROCESS: The super blt process proc type definition
\*=======================================================================*/

typedef void (FAR *LPBLTPROCESS)(LPTR lpSrc, LPTR lpDst, int iCount, int iDepth, int iRows);

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
	LPTR              lpCopyBuf1;     /* 64K data buffer for blting */
	LPTR			  lpCopyBuf2;	  /* 64K data buffer for CMS and/or	gamma correction */
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
	BOOL			  bUseRgbLUIndex;
	void ( FAR *lpCmsGammaProc )( BOOL bIniting = FALSE );
	LFIXED            CmsRGamma;         /* The Red Gamma Value */
	LFIXED            CmsGGamma;         /* The Green Gamma Value */
	LFIXED            CmsBGamma;         /* The Blue Gamma Value */
	BYTE              CmsRGammaLut[256]; /* The Red GammaLut */
	BYTE              CmsGGammaLut[256]; /* The Green GammaLut */
	BYTE              CmsBGammaLut[256]; /* The Blue GammaLut */
	BOOL				   bGammaDisabled;	/* disable gamma flag */
   HDC				   WinGDC;
   HBITMAP			   WinGBM;
   HBITMAP			   WinGOldBM;
   int				   WinGWidth;
   LPTR     	      WinGBuf;
} BLT, FAR *LPBLT;

/*=======================================================================*\
	BLTSESSION: The super blt session structure
\*=======================================================================*/

typedef void ( FAR *LPKCMSEVALPROC )( LPFRMTYPEINFO, LPFRMTYPEINFO, LPVOID, LPTR, LPTR, int, int );

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
	int		 dxBytes;		 // bytes per line of frame
	int      dxConverted;    // bytes per line of blt
	int      xDiva;          // x dithering offset
	int      yDiva;          // y dithering offset
	int      iLinesPerBlt;   // lines to build up before blting
	WORD     wUsage;         // colors usage
	BOOL     bBufAllocated;  // the Blt buffer was allocated?
	LPTR     lpBltBufStart;  // the Blt buffer
	LPTR     lpCopyBuf1;     // data buffer LinesPerBlt lines (working ptr)
	LPTR     lpCopyBuf2;     // data buffer LinesPerBlt lines (working ptr)

	// the data converter proc
	void ( FAR *fpConvert )( LPTR, int, LPINT, LPTR, int, int,
       struct _bltsession FAR * );      

	// the mapping proc, for gamma correction
	void ( FAR *fpMap )( LPTR, LPTR, LPTR, LPTR, unsigned ); 

	HPALETTE 		hOldPal;        // The original Palette...
	LPVOID	 		cmsXform;  	// Pointer to Precision Transform.
	LPKCMSEVALPROC	lpKcmsEvalProc;
	LPCOLORMAP   	lpColorMap;		/* palette for 8-bit palette colored data */
	LPTR	 		lpError;
	BOOL	 		fUseStretchDIB;     // Use StretchDIBits ;otherwise use SetDIBitstoDevice
	BOOL			bDoCmsGamma;		// Use CMS Gamma LUTs.
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
	BOOL bProcessData,
	BOOL bUseRgbLUIndex = FALSE,
	BOOL bPcReserved = TRUE);

void EndSuperBlt( LPBLT lpBlt );

HPALETTE SuperBltSetupPalette(LPBLT lpBlt, HDC hDC, HPALETTE hPal, BOOL fRealize);

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
	LPVOID		 cmsXform,
	LPKCMSEVALPROC lpKcmsEvalProc,
	BOOL         fUseStretchDIB = FALSE,
	BOOL		 bDoCmsGamma = FALSE,
	BOOL         bIsMetaDC = FALSE);

void SuperBlt( LPBLTSESSION lpSession, LPTR lp, BOOL bIsMetaDC = FALSE );

void SuperFill(
	HDC    hDC,
	LPBLT  lpBltDevice,
	LPRECT lpRect,
	int    xDivaStart,
	int    yDivaStart,
	COLOR  Color,
	LPBLTPROCESS lpProcessProc);

BOOL SetNewPalette(LPBLT lpBlt, PALETTE_TYPE PaletteType, HPALETTE hPal, LPTR lpPaletteLUT, LPPALETTEENTRY lpEntries, int nEntries, BOOL fApplyGamma);
BOOL SetSuperBltGamma( LPBLT lpBlt, LFIXED RGamma, LFIXED GGamma, LFIXED BGamma, BOOL bDisableGamma );
BOOL SetSuperBltGammaLuts(LPBLT lpBlt, LPTR lpRLut, LPTR lpGLut, LPTR lpBLut);
BOOL GammaAdjustPalette(LPBLT lpBlt, HPALETTE hPal, int nEntries, LPPALETTEENTRY lpEntries);
HPALETTE CreateLogicalPalette(LPRGB lpRGBmap, int nEntries, BOOL bPcReserved = TRUE);

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

/*========================================*\
	C++ Garbage...
\*========================================*/

#ifdef __cplusplus
}
#endif // __cplusplus

#endif





