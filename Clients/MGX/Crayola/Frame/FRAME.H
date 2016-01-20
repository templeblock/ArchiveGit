/*=======================================================================*\

	FRAME.H - Frame Library include header file (public).

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifndef __frame_h__
#define __frame_h__

#include "types.h"

/*==========================================================*\
	Error codes:
\*==========================================================*/

#define ERR_MAXIMAGEHEIGHT    0
#define ERR_MAXIMAGEWIDTH     1
#define ERR_IMAGEOVERHEAD     2
#define ERR_MALLOC            3
#define ERR_LINEPOOLALLOC     4
#define ERR_DISKCACHE         5
#define ERR_SCROPEN           6
#define ERR_SCRWRITE          7
#define ERR_SCRREAD           8

/*==========================================================*\
	Colormap type definition
\*==========================================================*/

typedef struct _colormap
{
 	int  NumEntries;
	RGBS LastRGB;
	int	 LastIndex;
	RGBS RGBData[1];
} COLORMAP, far *LPCOLORMAP;

/*==========================================================*\
	Frame Data Type Enumerarion definition
\*==========================================================*/

typedef enum
{
	FDT_LINEART      = 0,
	FDT_GRAYSCALE    = 1,
	FDT_PALETTECOLOR = 2,
	FDT_RGBCOLOR     = 3,
	FDT_CMYKCOLOR    = 4
}
FRMDATATYPE, far *LPFRMDATATYPE;

/*==========================================================*\
	Frame Cache release mode constant definitions
\*==========================================================*/

#define RELEASE_NEWEST 0
#define RELEASE_OLDEST 1

/*==========================================================*\
	Frame structure definition
\*==========================================================*/

typedef void FAR *LPFRAME;
typedef void FAR *LPMASK;
typedef void FAR *LPSHAPE;

/*========================================*\
	C++ Garbage...
\*========================================*/
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*=========================================================================*\
	frameold.c
\*=========================================================================*/

void frame_settings( LPTR lpRamDisk, int iMainMemMin, int iMainMemFactor, BOOL EMSdisable );
LPFRAME frame_open( WORD Depth, WORD x_size, WORD y_size, WORD resolution );
void frame_mode( int ReleaseMode );
void frame_close( LPFRAME lpFrame );
LPFRAME frame_set( LPFRAME lpNewFrame );
int frame_inmemory( LPFRAME lpFrame );
BOOL frame_write( int sx, int sy, int dx, LPTR pixels, int count );
BOOL frame_read( int sx, int sy, int dx, LPTR pixels, int count );
LPTR frame_ptr( int Depth, int x, int y, BOOL bModify );
void frame_sample( LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );
long frame_RGB2pixel(LPRGB lpFullRGB);
void frame_pixel2HSL( long color, LPHSL lpHSL );
void frame_pixel2RGB( long color, LPRGB lpRGB );
void frame_setpixels( LPTR ptr, int count, long value );
int frame_depth( BOOL bRealDepth );
void frame_copyline(LPFRAME lpInFrame, LPFRAME lpOutFrame, int y);

/*=========================================================================*\
	frmdtcon.c
\*=========================================================================*/

// Lineart to XXXX conversion
extern	void LineArtToGrayScale(LPTR lpSrc,LPTR lpDst,int iCount);
extern	void LineArtToPaletteColor(LPTR lpSrc,LPTR lpDst,int iCount, LPCOLORMAP lpColorMap);
extern	void LineArtToRGBColor(LPTR lpSrc,LPTR lpDst,int iCount);
extern	void LineArtToCMYKColor(LPTR lpSrc,LPTR lpDst,int iCount);

// Grayscale to XXXX conversion
extern	void GrayScaleToLineArt(LPTR lpSrc,LPTR lpDst,int iCount);
extern	void GrayScaleToPaletteColor(LPTR lpSrc,LPTR lpDst,int iCount,LPCOLORMAP lpDstMap);
extern	void GrayScaleToRGBColor(LPTR lpSrc,LPTR lpDst,int iCount);
extern	void GrayScaleToCMYKColor(LPTR lpSrc,LPTR lpDst,int iCount);

// Palette Color to XXXX conversion
extern	void PaletteColorToLineArt(LPTR lpSrc,LPTR lpDst,int iCount,LPCOLORMAP lpSrcMap);
extern	void PaletteColorToGrayScale(LPTR lpSrc,LPTR lpDst,int iCount,LPCOLORMAP lpSrcMap);
extern	void PaletteColorToRGBColor(LPTR lpSrc,LPTR lpDst,int iCount,LPCOLORMAP lpSrcMap);
extern	void PaletteColorToCMYKColor(LPTR lpSrc,LPTR lpDst,int iCount,LPCOLORMAP lpSrcMap);

// RGB Color to XXXX conversion
extern	void RGBColorToLineArt(LPTR lpSrc,LPTR lpDst,int iCount);
extern	void RGBColorToGrayScale(LPTR lpSrc,LPTR lpDst,int iCount);
extern	void RGBColorToPaletteColor(LPTR lpSrc,LPTR lpDst,int iRow,int iCount,LPCOLORMAP lpDstMap);
extern	void RGBColorToCMYKColor(LPTR lpSrc,LPTR lpDst,int iCount);

// CMYK Color to XXXX conversion
extern	void CMYKColorToLineArt(LPTR lpSrc,LPTR lpDst,int iCount);
extern	void CMYKColorToGrayScale(LPTR lpSrc,LPTR lpDst,int iCount);
extern	void CMYKColorToPaletteColor(LPTR lpSrc,LPTR lpDst,int iRow,int iCount,LPCOLORMAP lpDstMap);
extern	void CMYKColorToRGBColor(LPTR lpSrc,LPTR lpDst,int iCount);

// Standard converter from one type to another
extern  void FAR FrameTypeConvert(LPTR lpSrc, FRMDATATYPE fdtSrc, LPCOLORMAP lpSrcMap,int iRow,LPTR lpDst,FRMDATATYPE fdtDst,LPCOLORMAP lpDstMap,int iCount);

// Frame Conversion from one type to another
extern  LPFRAME FAR FrameConvert(LPFRAME lpSrcFrame, FRMDATATYPE fdtDst, LPROC lpStatusCallback);

/*=========================================================================*\
	frmcpo.c
\*=========================================================================*/

LPTR CreateOptimizedPalette( LPFRAME lpFrame, LPRGB lpRGBmap, LPINT lpMaxColors, LPROC lpStatusCallback, BOOL bOptimize, BOOL bDither, BOOL bMinColors);
long CreateHistogram( LPFRAME lpFrame, LPWORD lpHisto, LPRGB lpRGBmap, LPINT lpMaxColors, LPROC lpStatusCallback);
int  OptimizeBegin( LPFRAME lpFrame, LPRGB RGBmap, int nColors, LPROC lpStatusCallback, BOOL bReduction, BOOL bOptimize, BOOL bScatter, BOOL bDither, int dx);
void OptimizeData(int x, int y, int iCount, LPTR lpIn, LPTR lpOut, int depth);
void OptimizeEnd(void);
int  CountColors(LPFRAME lpFrame, LPRGB lpRGBmap, LPRGB lpRGBData, int iLength, LPROC lpStatusCallback);

/*=========================================================================*\
	frmfiles.c
\*=========================================================================*/

void FixFileName( LPTR lpPathName );
LPTR stripdir( LPTR lpPathName );
LPTR filename( LPTR lpPathName );
LPTR stripfile( LPTR lpPath );
LPTR stripext( LPTR lpPath );
LPTR FixPath( LPTR lpPath );
LPTR extension( LPTR lpPath );
LPTR GetCurrentDir( LPTR lpBuffer, int maxlen );
BOOL SetCurrentDir( LPTR lpCurrentDir );
DWORD DiskSpace( LPTR lpPath );
BOOL FileCopy( LPTR lpInFile, LPTR lpOutFile );
BOOL FileExists( LPTR szFileName );
BOOL FileAccess( LPTR szFileName, WORD wStyle );
long FileDate( LPTR lpFileName );
long FileSize( LPTR lpFileName );
int WildcardDelete( LPTR lpPath, LPTR lpName );
BOOL GetTempFilename( LPTR lpTempName, LPTR lpExtension );
LPTR GetVolumnID( LPTR lpPath, LPTR lpVolumn );
int EnumFiles(LPTR lpSrcName, LPTR lpDstName, LPROC lpEnumProc, long lData);
LPTR MakeFullPath(LPTR lpPath);

/*=========================================================================*\
	frmmain.c
\*=========================================================================*/

LPFRAME FrameGetCurrent(void);
LPFRAME FrameSetCurrent(LPFRAME lpFrame);

void FAR FrameSettings( LPTR lpRamDisk, int iMainMemMin, int iMainMemFactor);
int  FAR FrameGetError( void );
void FAR FrameSetError( int iErrorCode );

LPFRAME FAR FrameOpen( FRMDATATYPE DataType, int XSize, int YSize, int Resolution );

void FAR FrameClose( LPFRAME lpFrame );
BOOL FAR FrameRead( LPFRAME lpFrame, int sx, int sy, int dx, LPTR pixels, int count );
BOOL FAR FrameWrite( LPFRAME lpFrame, int sx, int sy, int dx, LPTR pixels, int count );
int  FAR FrameMode( LPFRAME lpFrame, int ReleaseMode );
int  FAR FrameInMemory( LPFRAME lpFrame );
LPTR FAR FramePointer( LPFRAME lpFrame, int x, int y, BOOL bModify );
LPTR FAR FramePointerRaw( LPFRAME lpFrame, int x, int y, BOOL bModify );
void FAR FrameSample( LPFRAME lpFrame, LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );
void FAR FrameSampleMW(LPFRAME lpFrame, LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate, LPTR mask);
LPTR FAR FrameName(LPFRAME lpFrame);
LPMASK FAR FrameGetMask( LPFRAME lpFrame );
void FAR FrameSetMask( LPFRAME lpFrame, LPMASK lpMask );
FRMDATATYPE FAR FrameType( LPFRAME lpFrame );
int  FAR FrameDepth( LPFRAME lpFrame );
int  FAR FrameXSize( LPFRAME lpFrame );
int  FAR FrameYSize( LPFRAME lpFrame );
int  FAR FrameByteWidth( LPFRAME lpFrame );
int  FAR FrameResolution( LPFRAME lpFrame );

BOOL       FAR FrameSetColorMap( LPFRAME lpFrame, LPCOLORMAP lpColorMap);
LPCOLORMAP FAR FrameGetColorMap( LPFRAME lpFrame );
LPCOLORMAP FAR FrameCreateColorMap( void );
LPCOLORMAP FAR FrameCreateFixedColorMap( void );
void FAR FrameDestroyColorMap( LPCOLORMAP lpColorMap );
BYTE FrameGetNearestIndex(LPCOLORMAP lpColorMap, LPRGB lpRGB);

VOID FAR FrameDuplicateLine( LPFRAME lpInFrame, LPFRAME lpOutFrame, int iny, int outy );
VOID FAR FrameCopyLine( LPFRAME lpInFrame, LPFRAME lpOutFrame, int y );
LPTR BumpPtr( LPTR lp, DWORD lOffset );
int  FramePreload( LPFRAME lpFrame, int fp, BOOL invert, LPROC lpStatusCallback);
LPTR LineInMemory( LPFRAME lpFrame, int y);

/*=========================================================================*\
	frmpixel.c
\*=========================================================================*/

void FAR FrameRGB2Pixel( LPFRAME lpFrame, LPRGB  lpFullRGB, LPLONG lpPixel );
void FAR FramePixel2RGB( LPFRAME lpFrame, LPLONG lpPixel,   LPRGB  lpFullRGB );

void FAR FrameGetRGB( LPFRAME lpFrame, LPTR  lpSrc,    LPRGB lpDstRGB, int iPixelCount);
void FAR FramePutRGB( LPFRAME lpFrame, LPRGB lpSrcRGB, LPTR  lpDst,    int iPixelCount);
void FAR FrameSetRGB( LPFRAME lpFrame, LPRGB lpRGBPixel, LPTR lpDst, int iPixelCount);

void FAR FrameGetPixel( LPFRAME lpFrame, LPTR   lpSrc, LPLONG lpDst, int iPixelCount);
void FAR FramePutPixel( LPFRAME lpFrame, LPLONG lpSrc, LPTR   lpDst, int iPixelCount);
void FAR FrameSetPixel( LPFRAME lpFrame, long lPixel, LPTR lpDst, int iPixelCount);

/*=========================================================================*\
	frmdib.c
\*=========================================================================*/

LPTR    FAR FrameToDIB( LPFRAME lpFrame, LPRECT lpRect );
LPFRAME DIBToFrame( LPTR lpDIBMem );

/*=========================================================================*\
	frmhsl.c
\*=========================================================================*/

void CMYKtoHSL( BYTE C, BYTE M, BYTE Y, BYTE K, LPHSL lpHSL );
void HSLtoCMYK( BYTE, BYTE, BYTE, LPCMYK );
BYTE RGBtoL( LPRGB lpRGB );
void RGBtoHS( BYTE R, BYTE G, BYTE B, LPHSL lpHSL );
void RGBtoHL( BYTE R, BYTE G, BYTE B, LPHSL lpHSL );
void RGBtoSL( BYTE R, BYTE G, BYTE B, LPHSL lpHSL );
void RGBtoH( BYTE R, BYTE G, BYTE B, LPHSL lpHSL );
void RGBtoS( BYTE R, BYTE G, BYTE B, LPHSL lpHSL );

BYTE CMYKtoL( LPCMYK lpCMYK );
void CMYKtoHS( LPCMYK lpCMYK, LPHSL lpHSL);
void CMYKtoHL( LPCMYK lpCMYK, LPHSL lpHSL);
void CMYKtoSL( LPCMYK lpCMYK, LPHSL lpHSL);
void CMYKtoH(  LPCMYK lpCMYK, LPHSL lpHSL);
void CMYKtoS(  LPCMYK lpCMYK, LPHSL lpHSL);

/*=========================================================================*\
	frmhsla.asm
\*=========================================================================*/

void RGBtoHSL( BYTE, BYTE, BYTE, LPHSL );
void HSLtoRGB( BYTE, BYTE, BYTE, LPRGB );

/*=========================================================================*\
	frmmap.c
\*=========================================================================*/

void map( LPTR lut, LPTR buf, int count );
void MapRGB8(  LPTR lpRLut, LPTR lpGLut, LPTR lpBLut, LPTR   lpSrc, LPTR   lpDst, int iCount);
void MapRGB24( LPTR lpRLut, LPTR lpGLut, LPTR lpBLut, LPRGB  lpSrc, LPRGB  lpDst, int iCount);
void MapRGB32( LPTR lpRLut, LPTR lpGLut, LPTR lpBLut, LPCMYK lpSrc, LPCMYK lpDst, int iCount);

/*=========================================================================*\
	frmmask.c
\*=========================================================================*/

LPMASK MaskCreate( int iMaxWidth, int iMaxHeight, BOOL fOn, BOOL fNoUndo);
BOOL MaskAddPoints( LPMASK lpMask, LPPOINT pnts, LPINT nPoints, int nShapes, int iModes, LPRECT lpRect);
BOOL MaskAddShapes( LPMASK lpMask, LPSHAPE lpShape);
LPSHAPE MaskCreateShapes( LPPOINT pnts, LPINT nPoints, int nShapes, int iModes, LPTFORM lpTForm );
void mask_offset( LPMASK lpMask, int dx, int dy );
BOOL mask_close( LPMASK lpMask );
void mask_link( LPFRAME lpFrame, LPMASK lpMask );
void mask_unlink( LPFRAME lpFrame, LPMASK lpMask );
BOOL mask_active( LPFRAME lpFrame );
BOOL mask_rect( LPFRAME lpFrame, LPRECT lpRect );
BOOL MaskRect( LPMASK lpMask, LPRECT lpRect );
LPTR MaskPointer( LPMASK lpMask, int x, int y );
int IsMasked( LPMASK lpMask, int x, int y );
void SetMaskOutside( BOOL Bool );
BOOL IsMaskOutside( void );
void FreeUpShapes( LPSHAPE lpShape );
BOOL MaskLoad( LPMASK lpMask, int x, int y, int dx, LPTR lpM );
BOOL MaskLoadS( LPMASK lpMask, int x, int y, int dx, LPTR lpM, int onMask );
BOOL MaskLoader( LPMASK lpMask, int x, int y, int iMaxWidth, LPTR lpM, BOOL bInvert, BOOL bOrMask );
int ShapeConvertPoints(LPSHAPE lpShape, LPPOINT lpOutputPoint);
int BezierToPoints( LPTFORM lpTForm, LPPOINT lpPoints, int nPoints, LPPOINT lpOutputPoints );

/*=========================================================================*\
	frmmem.c
\*=========================================================================*/

LPTR Alloc( long lCount );
LPTR AllocX( long lCount, WORD wFlags );
LPTR AllocExtend( LPTR lpMemory, long lCount );
void FreeUp( LPTR lpMemory );
DWORD AvailableMemory( void );
BOOL AllocLines( LPPTR lpList, int nLines, int iWidth, int iDepth );
LPTR GetBuffers16Kx4( LPPTR lppBuffers, BOOL bDosMemory );

/*=========================================================================*\
	frmmini.c
\*=========================================================================*/

void MiniRGBtoRGB( WORD miniRGB, LPRGB lpRGB );
BYTE MiniRGBtoL( WORD miniRGB );
void MiniRGBtoHSL( WORD miniRGB, LPHSL lpHSL );
WORD HSLtoMiniRGB( LPHSL lpHSL );

/*=========================================================================*\
	frmsmple.c
\*=========================================================================*/

void Sample_1(  LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );
void Sample_8(  LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );
void Sample_24( LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );
void Sample_32( LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );

void Sample_1MW(  LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate, LPTR mask );
void Sample_8MW(  LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate, LPTR mask );
void Sample_24MW( LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate, LPTR mask );
void Sample_32MW( LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate, LPTR mask );

/*=========================================================================*\
	frmtform.c
\*=========================================================================*/

void TInit( LPTFORM lpTForm );
void TMultiply( LPTFORM lpTFormS1, LPTFORM lpTFormS2, LPTFORM lpTFormD );
void TMove( LPTFORM lpTForm, int tx, int ty );
void TFMove( LPTFORM lpTForm, LFIXED fx, LFIXED fy );
void TScale( LPTFORM lpTForm, LFIXED sx, LFIXED sy );
void TShearX( LPTFORM lpTForm, int height, int dx );
void TShearY( LPTFORM lpTForm, int width, int dy );
void TRotate( LPTFORM lpTForm, int anglex, int angley );
void TInvert( LPTFORM lpTForm );
void FTransformer( LPTFORM lpTForm, LPPOINT lpPoint, LPLONG lpx, LPLONG lpy );
void Transformer( LPTFORM lpTForm, LPPOINT lpPoint, LPINT lpx, LPINT lpy );
void TransformRect( LPTFORM lpTForm, LPRECT lpRect, LPRECT lpNewRect );

/*=========================================================================*\
	frmutil.c
\*=========================================================================*/

int bound( int, int, int );
void AstralUnionRect( LPRECT lpDestRect, LPRECT lpSrc1Rect, LPRECT lpSrc2Rect );
BOOL AstralIntersectRect( LPRECT lpDest, LPRECT lpSrc1, LPRECT lpSrc2 );
void AstralSubtractRect( LPRECT lpDest, LPRECT lpSrc );
void BoundRect( LPRECT lpRect, int left, int top, int right, int bottom );
void lmemcpy( LPTR lpOut, LPTR lpIn, int iCount );
void mirror8( LPTR pixels, int count );
void mirror24( LPTR pixels, int count );
void mirror32( LPTR pixels, int count );
LFIXED fget( long numer, long denom );
LFIXED special_fget( LFIXED numer, LFIXED denom );
long fmul( long a, long b );
long tmul( long a, long b );
LPTR FixedAscii( LFIXED Fixed, LPTR lpAscii, int Precision );
LFIXED AsciiFixed( LPTR lpAscii );
LPTR RGBAscii( LPRGB lpRGB, LPTR pAscii );
void AsciiRGB( LPTR pAscii, LPRGB lpRGB );
LPTR Int2Ascii( LPTR lpAscii, int i );
int Ascii2Int( LPTR lpAscii );
LFIXED ScaleToFit( int far *DestWidth, int far *DestHeight, int SrcWidth, int SrcHeight );
LPTR itoh( WORD i );
int htoi( LPTR s );
LPTR Lowercase( LPTR lp );
LPTR Uppercase( LPTR lp );
void negate( LPTR buf, long lCount );
void ramp( LPTR lut, int inx1, int inx2, int outx1, int outx2 );
void copy( LPTR lpSrc, LPTR lpDst, int iCount );
void clr( LPTR lpDst, int iCount );
void lclr( LPTR ptr, long lCount );
void setifset( LPTR lpDst, int iCount, int iValue );
void set( LPTR lpDst, int iCount, int iValue );
void set16( LPWORD ptr, int count, WORD value );
void set24( LPRGB lpRGB, int iCount, long lValue );
void hist( LPTR pixels, int count, long histo[256] );
void swapw( LPWORD word );
void swapl( LPDWORD dword );
void Unpack4( LPTR lpIn, int iCount, LPTR lpOut );
void ScaleRect( LPRECT lpRect, LFIXED Scale );
void SinCos( int iAngle, LPLONG lplSin, LPLONG lplCos );
WORD lsqrt( long lValue );
BOOL StringsEqual( LPTR lpString1, LPTR lpString2 );
BOOL StringsEqualN( LPTR lpString1, LPTR lpString2, int length );
LPTR SkipSpaces( LPTR lpString );
LPTR RemoveWhiteSpace( LPTR lpString );
LPTR StripTrailer( LPTR lpString, char cChar );
LPTR lStrChr( LPTR lpString, char cChar );
LPTR lstrfind( LPTR lpString, LPTR lpSearchString );
void la2con( LPTR lpIn, int iCount, LPTR lpOut, int normal );
void con2la( LPTR lpIn, int iCount, LPTR lpOut );

/*========================================*\
	C++ Garbage...
\*========================================*/
#ifdef __cplusplus
}
#endif // __cplusplus

// Imported Data
extern BYTE ToMaxi[32];
extern BYTE ShiftUp2Lut[256];

/*=========================================================================*\
	Frame access macros
\*=========================================================================*/

#define RGBtoMiniRGB(R) \
	(((WORD) ( DN((R)->blue) << 5 | DN((R)->green) ) << 5) | DN((R)->red) )
#define ToMiniRGB(r,g,b) \
	( (((WORD)(b) << 5 | (WORD)(g)) << 5) | (WORD)(r) )
#define MiniR(v) (BYTE)( 31  & ( (WORD)(v)       ) )
#define MiniG(v) (BYTE)( 31  & ( (WORD)(v) >> 5  ) )
#define MiniB(v) (BYTE)( 31  & ( (WORD)(v) >> 10 ) )
#define MaxiR(v) (BYTE)( ToMaxi[ MiniR(v) ] )
#define MaxiG(v) (BYTE)( ToMaxi[ MiniG(v) ] )
#define MaxiB(v) (BYTE)( ToMaxi[ MiniB(v) ] )

#define frame_getpixel(lpLine, depth, colorPtr )                        \
{                                                                       \
if (depth == 4)         { CopyCMYK(lpLine, colorPtr); }                 \
else if (depth == 3)    { *(colorPtr) = 0; CopyRGB(lpLine, colorPtr); } \
else if (depth == 1)    { *((LPTR)(colorPtr)) = *((LPTR)(lpLine)); }    \
     else               { *((LPWORD)(colorPtr)) = *((LPWORD)(lpLine)); }\
}

#define frame_getRGB( lpPixel, depth, rgb )                             \
{                                                                       \
if (depth == 4)         { depth4toRGB( lpPixel, rgb ); }                \
else if (depth == 3)    { depth3toRGB( lpPixel, rgb ); }                \
else if (depth == 1)    { depth1toRGB( lpPixel, rgb ); }                \
     else               { depth2toRGB( lpPixel, rgb ); }                \
}

#define frame_getHSL( lpPixel, depth, hsl )                             \
{                                                                       \
if (depth == 4)         { depth4toHSL( lpPixel, hsl ); }                \
else if (depth == 3)    { depth3toHSL( lpPixel, hsl ); }                \
else if (depth == 1)    { depth1toHSL( lpPixel, hsl ); }                \
     else               { depth2toHSL( lpPixel, hsl ); }                \
}

#define frame_putpixel( lpLine, depth, color )                          \
{                                                                       \
if (depth == 4)         { CopyCMYK(&color, lpLine); }                   \
else if (depth == 3)    { CopyRGB(&color, lpLine); }                    \
else if (depth == 1)    { *((LPTR)(lpLine)) = *((LPTR)&(color)); }      \
     else               { *((LPWORD)(lpLine)) = *((LPWORD)&(color)); }  \
}

#define frame_putRGB( lpPixel, depth, rgb )                             \
{                                                                       \
if (depth == 4)         { RGBtodepth4( lpPixel, rgb ); }                \
else if (depth == 3)    { RGBtodepth3( lpPixel, rgb ); }                \
else if (depth == 1)    { RGBtodepth1( lpPixel, rgb ); }                \
     else               { RGBtodepth2( lpPixel, rgb ); }                \
}

#define frame_putHSL( lpPixel, depth, hsl )                             \
{                                                                       \
if (depth == 4)         { HSLtodepth4( lpPixel, hsl ); }                \
else if (depth == 3)    { HSLtodepth3( lpPixel, hsl ); }                \
else if (depth == 1)    { HSLtodepth1( lpPixel, hsl ); }                \
     else               { HSLtodepth2( lpPixel, hsl ); }                \
}

#define frame_copypixel(lpSrc, depth, lpDst )                           \
{                                                                       \
if (depth == 4)         { CopyCMYK(lpSrc, lpDst); }                     \
else if (depth == 3)    { CopyRGB(lpSrc, lpDst); }                      \
else if (depth == 1)    { *((LPTR)(lpDst)) = *((LPTR)(lpSrc)); }        \
     else               { *((LPWORD)(lpDst)) = *((LPWORD)(lpSrc)); }    \
}

#define RGBtodepth1( lpPixel, rgb )                                     \
{                                                                       \
*((LPTR)(lpPixel)) = TOLUM( (rgb)->red, (rgb)->green, (rgb)->blue );    \
}

#define HSLtodepth1( lpPixel, hsl )                                     \
{                                                                       \
*((LPTR)(lpPixel)) = (hsl)->lum;                                        \
}

#define RGBtodepth3( lpPixel, rgb )                                     \
{                                                                       \
*((LPTR)(lpPixel))   = (rgb)->red;                                      \
*((LPTR)(lpPixel)+1) = (rgb)->green;                                    \
*((LPTR)(lpPixel)+2) = (rgb)->blue;                                     \
}

#define RGBtodepth4( lpPixel, rgb )                                     \
{                                                                       \
*((LPTR)(lpPixel))   = (rgb)->red  ^0xFF;                               \
*((LPTR)(lpPixel)+1) = (rgb)->green^0xFF;                               \
*((LPTR)(lpPixel)+2) = (rgb)->blue ^0xFF;                               \
*((LPTR)(lpPixel)+3) = 0;                                               \
}

#define HSLtodepth3( lpPixel, hsl )                                     \
{                                                                       \
HSLtoRGB( (hsl)->hue, (hsl)->sat, (hsl)->lum, (LPRGB)lpPixel );         \
}

#define HSLtodepth4( lpPixel, hsl )                                     \
{                                                                       \
HSLtoCMYK( (hsl)->hue, (hsl)->sat, (hsl)->lum, (LPCMYK)lpPixel );       \
}

#define RGBtodepth2( lpPixel, rgb )                                     \
{                                                                       \
*((LPWORD)(lpPixel)) = RGBtoMiniRGB( rgb );                             \
}

#define HSLtodepth2( lpPixel, hsl )                                     \
{                                                                       \
RGBS rgb;                                                               \
HSLtoRGB( (hsl)->hue, (hsl)->sat, (hsl)->lum, &rgb );                   \
*((LPWORD)(lpPixel)) = RGBtoMiniRGB( &rgb );                            \
}

#define depth1toRGB( lpPixel, rgb )                                     \
{                                                                       \
BYTE b;                                                                 \
b = *((LPTR)(lpPixel));                                                 \
(rgb)->red   = b;                                                       \
(rgb)->green = b;                                                       \
(rgb)->blue  = b;                                                       \
}

#define depth1toHSL( lpPixel, hsl )                                     \
{                                                                       \
(hsl)->hue = 0;                                                         \
(hsl)->sat = 0;                                                         \
(hsl)->lum = *((LPTR)(lpPixel));                                        \
}

#define depth3toRGB( lpPixel, rgb )                                     \
{                                                                       \
(rgb)->red   = *((LPTR)(lpPixel));                                      \
(rgb)->green = *((LPTR)(lpPixel)+1);                                    \
(rgb)->blue  = *((LPTR)(lpPixel)+2);                                    \
}

#define depth4toRGB( lpPixel, rgb )                                     \
{                                                                       \
(rgb)->red    = *((LPTR)(lpPixel))^0xFF;                                \
(rgb)->green  = *((LPTR)(lpPixel)+1)^0xFF;                              \
(rgb)->blue   = *((LPTR)(lpPixel)+2)^0xFF;                              \
if (*((LPTR)(lpPixel)+3)) \
{ \
(rgb)->red   -= ((rgb)->red   * (long)(*((LPTR)(lpPixel)+3))) >> 8; \
(rgb)->green -= ((rgb)->green * (long)(*((LPTR)(lpPixel)+3))) >> 8; \
(rgb)->blue  -= ((rgb)->blue  * (long)(*((LPTR)(lpPixel)+3))) >> 8; \
} \
}

#define depth3toHSL( lpPixel, hsl )                                     \
{                                                                       \
RGBtoHSL( *((LPTR)(lpPixel)),                                           \
	  *((LPTR)(lpPixel)+1),                                              \
	  *((LPTR)(lpPixel)+2), hsl );                                       \
}

#define depth4toHSL( lpPixel, hsl ) \
{ \
CMYKtoHSL( \
*((LPTR)(lpPixel)),\
*((LPTR)(lpPixel+1)),\
*((LPTR)(lpPixel+2)),\
*((LPTR)(lpPixel+3)),\
hsl);\
}

#define depth2toRGB( lpPixel, rgb )                                     \
{                                                                       \
WORD w;                                                                 \
w = *((LPWORD)(lpPixel));                                               \
(rgb)->red   = MaxiR(w);                                                \
(rgb)->green = MaxiG(w);                                                \
(rgb)->blue  = MaxiB(w);                                                \
}

#define depth2toHSL( lpPixel, hsl )                                     \
{                                                                       \
WORD w;                                                                 \
w = *((LPWORD)(lpPixel));                                               \
RGBtoHSL( MaxiR(w), MaxiG(w), MaxiB(w), hsl );                          \
}

#endif /* __frame_h__ */

