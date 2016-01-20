/*=======================================================================*\

	MGXFRAME.H - MGX Frame Library include header file.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifndef __mgxframe_h__
#define __mgxframe_h__
#ifndef __AFXWIN_H__
#include <windows.h>
#endif
#include <limits.h>

#ifdef WIN32
#define NO_CACHE
#endif

extern BYTE ToMaxi[32];

/*==========================================================*\
	Generic DataTypes:
\*==========================================================*/
#ifndef LPTR_DEFINED
#define LPTR_DEFINED
#undef LPTR
typedef unsigned char FAR *LPTR;
typedef LPTR FAR *LPPTR;
#endif

#ifdef HUGE
#undef HUGE
#endif
#define HUGE
#define FRAME_CLASS_EXPORT __declspec(dllexport)
#define BIT1		1L
#define BIT2		2L
#define BIT3		4L
#define BIT6		32L
#define BIT18		131072L
#define BITMASK		( BIT1| BIT2| BIT3| BIT6 )

//**********************************************************************
// Hue, Saturation, and Luminace color definition
//**********************************************************************
typedef struct _hsl
	{
	BYTE hue;
	BYTE sat;
	BYTE lum;
} HSLS;
typedef HSLS FAR *LPHSL;

//**********************************************************************
// Red, Green, and Blue color definition
//**********************************************************************
typedef struct _rgb
	{
	BYTE red;
	BYTE green;
	BYTE blue;
} RGBS;
typedef RGBS FAR *LPRGB, HUGE *HPRGB;

//**********************************************************************
// Cyan, Magenta, Yellow, and Black color definition
//**********************************************************************
typedef struct _cmyk
	{
	BYTE c;
	BYTE m;
	BYTE y;
	BYTE k;
} CMYKS;
typedef CMYKS FAR *LPCMYK, HUGE *HPCMYK;

//********************************************************************
// Active/Alternate Color structure
//********************************************************************

typedef enum _colorspace
{
	CS_GRAY  = 1,
	CS_HSL   = 2,
	CS_RGB   = 3,
	CS_CMYK  = 4
} COLORSPACE;

typedef struct _colorinfo
{
	COLORSPACE ColorSpace;
	int   gray;
	RGBS  rgb;
	HSLS  hsl;
	CMYKS cmyk;
	BYTE  index; // convenient place to store this
} COLORINFO, FAR *LPCOLORINFO;

/*==========================================================*\
	Maximum image size definitions:
\*==========================================================*/

#define MAX_IMAGE_WIDTH		INT_MAX
#define MAX_IMAGE_HEIGHT	INT_MAX

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

// C++ Strict Prototypes for Function Ptrs.
typedef BOOL ( FAR *STATUSCALLBACKPROC )( int, int, BOOL );
typedef int ( FAR *FRMCNVTCBPROC )( int, int, int );
typedef int ( FAR *LPFRAMESTATUSPROC )( int, int, int, DWORD dwStatusData );
typedef int ( FAR *L3PROC )( int, int, int );
typedef int ( FAR *L4PROC )( LPSTR, LPSTR, long, int );
typedef void ( FAR *RGB2CMYKPROC )( struct _rgb far *, struct _cmyk far *, int );
typedef void ( FAR *CMYK2RGBPROC )( struct _cmyk far *, struct _rgb far *, int );

/*==========================================================*\
	Colormap type definition
\*==========================================================*/

typedef struct _frmcolormap
{
 	int  NumEntries;
	RGBS LastRGB;
	int	 LastIndex;
	RGBS RGBData[1];
} FRMCOLORMAP, FAR *LPFRMCOLORMAP;

// the following defines are used because COLORMAP and
// LPCOLORMAP are defined in commctrl.h and this enables
// us to not change a lot of code to change the typedef
#ifdef OLD_COLORMAP
#define COLORMAP FRMCOLORMAP
#define LPCOLORMAP LPFRMCOLORMAP
#endif

/*=========================================================================*\
   ColorManagement stuff
\*=========================================================================*/

typedef struct _ptinfo
	{
	LONG	toRCS;
	LONG	frRCS;
} PTINFO;
typedef PTINFO FAR *LPPTINFO;

/*==========================================================*\
	Frame Data Type Enumerarion definition
\*==========================================================*/

typedef enum
{
	FDT_NONE		 = -1,
	FDT_LINEART      = 0,
	FDT_GRAYSCALE    = 1,
	FDT_PALETTECOLOR = 2,
	FDT_RGBCOLOR     = 3,
	FDT_CMYKCOLOR    = 4
}
FRMDATATYPE, FAR *LPFRMDATATYPE;

/*==========================================================*\
	Frame Type Info definition
\*==========================================================*/

typedef struct 
{
	FRMDATATYPE 	DataType;
	LPFRMCOLORMAP	ColorMap;
	PTINFO			ptInfo;
}
FRMTYPEINFO, FAR *LPFRMTYPEINFO;

/*=========================================================================*\
   ColorManagement stuff
\*=========================================================================*/
typedef struct _cmsinfo
	{
	FRMTYPEINFO	src;
	FRMTYPEINFO	dst;
} CMSINFO;
typedef CMSINFO FAR *LPCMSINFO;

/*==========================================================*\
	Frame Cache release mode constant definitions
\*==========================================================*/

#define RELEASE_NEWEST 0
#define RELEASE_OLDEST 1

/*==========================================================*\
	Line Pool buffer structure definition
\*==========================================================*/

typedef struct _linepool
{
	struct _linepool FAR *Next;
	long                 Size;
} LINEPOOL, FAR *LPLINEPOOL;

#define MODIFIED_ON  0x80000000
#define MODIFIED_OFF 0x7FFFFFFF

/*==========================================================*\
	Link list entry definition
\*==========================================================*/

typedef struct _link
{
	int Next;
	int Prev;
} LINK, FAR *LPLINK;

/*==========================================================*\
	Frame structure definition
\*==========================================================*/

typedef struct _frame
{
	FRMDATATYPE DataType;
	BYTE    Depth;
	BYTE    RealDepth;
	BYTE    ReleaseMode;
	int     Xsize;
	int     Ysize;
	int     Width;
	int     Resolution;
	int     Background;
	LPTR    BackScratch;
	LPTR    LinePtr;  // Unpacked lineart buffer
	int     CacheLine; // Linenumber in the unpacked buffer
	LPFRMCOLORMAP ColorMap;
	char   CacheFileName[MAX_PATH];

// CMS - CRC of PT to transform from current colorspace to RCS
	PTINFO	ptInfo;

#ifdef NO_CACHE
	LPTR	ImageBase;
	LPTR	EmptyList;
#else
	int     CacheSeq;
	FILE_HANDLE CacheFile;
	LPLINK  HeadLink;
	LPLINK  LinkList;
	LPPTR	MemList;
	LPDWORD DiskList;
	LPPTR   FreePool;
	LPPTR   FreePoolEnd;
	LPPTR   FreeNext;
	LPLINEPOOL LinePool;
#endif
	int		xCache;
	int		dxCache;
} FRAME, FAR *LPFRAME;

/*=========================================================================*\
	fixed.c
\*=========================================================================*/
#ifndef WIN32
#define USING_FIXED16
#endif

typedef long  LFIXED16;
typedef LFIXED16 FAR *LPLFIXED16;
typedef LPLFIXED16 FAR *LPPLFIXED16;
#define FMUL16(num, fixed) fmul16( (long)num, (LFIXED16)fixed)
#define FGET16(n, d) fget16((long)n, (long)d)
#define TMUL16(num, fixed) tmul16( (long)num, (LFIXED16)fixed)
#define FP 16
#define WM 0xFFFF0000L
#define FINT(f) ((int)HIWORD(f))
#define FRAC(f) ((unsigned)LOWORD(f))
#define TOFIXED(num) ( (num)>0 ? ((long)(num))<<16 : -(((long)-(num))<<16) )
#define FONE    0x00010000L
#define UNITY		65536L
#define HALF		32768L
#define fix2int(num) ((int)((num) >> FP))
#define ROUND(num) ((int)(((num) + HALF) >> FP))
#define fix2fix(num) ((LFIXED16)((unsigned long)(num) >> FP))
#define DBLFRACTION(n) ((n) - (int)(n))
#define DBLWHOLE(n) ((int)((n) < 0 ? (n)-1 : (n)))
#define DBLFRACTION32(n) ((long)(DBLFRACTION(n) * (double)0x7FFFFFFF))
#define DBLMAKEWHOLE(n) ((double)(n))
#define DBLMAKEFRACTION(n) ((double)(n) / (double)0x7FFFFFFF)


#ifdef USING_FIXED16
typedef  LFIXED16 LFIXED;
#define DOUBLE(n) ((double)(n) / 65536.)
#define DBL2FIX(num) ((LFIXED16)(num * 65536.))
#define WHOLE(n) FINT(n)
#define WHOLEFIX(n) ((n) & WM)
#define FRACTION(n) FRAC(n)
#define FRACTION32(n) ((long)FRACTION(n))
#define FGET(n1, n2) FGET16(n1, n2)
#define FROUND(n) ROUND(n)
#define POSFROUND(n) ROUND(n)
#define FMUL(n1, n2) FMUL16(n1, n2)
#define FIXMUL(n1, n2) (DBL2FIX(DOUBLE(n1) * DOUBLE(n2)))
#define FIXDIV(n1, n2) (DBL2FIX(DOUBLE(n1) / DOUBLE(n2)))
#define FIXDIVWHOLE(n1, n2) ((n1) / (n2))
#define TMUL(n1, n2) TMUL16(n1, n2)
#define GETFIXED16(n) (n)
#define FUNITY UNITY
#define FHALF   HALF
#define FDIV2(n) ((n)/2)
#define MAKEFRACTION(n) (n)
#define TINYRATE 1
#define MAKEFIXED(n) TOFIXED(n)
#else
typedef double LFIXED;
#define DBL2FIX(num) (num)
#define WHOLE(n) DBLWHOLE(n)
#define WHOLEFIX(n) WHOLE(n)
#define FRACTION(n) DBLFRACTION(n)
#define FRACTION32(n) DBLFRACTION32(n)
#define FGET(n1, n2) ((double)(n1) / (double)(n2))
#define FROUND(n) ((int)((n) < 0 ? (n) - 0.5 : (n) + 0.5))
#define POSFROUND(n) ((int)((n) + 0.5))
#define FMUL(n1, n2) FROUND((double)(n1) * (double)(n2))
#define FIXMUL(n1, n2) ((double)(n1) * (double)(n2))
#define FIXDIV(n1, n2) ((double)(n1) / (double)(n2))
#define FIXDIVWHOLE(n1, n2) (WHOLE((n1) / (n2)))
#define TMUL(n1, n2) (WHOLE((double)(n1) * (double)(n2)))
#define GETFIXED16(n) ((long)((n) * 65536.0))
#define DOUBLE(n) (n)
#define FUNITY 1.0
#define FHALF 0.5
#define FDIV2(n) ((n) / 2)
#define MAKEFRACTION(n) DBLMAKEFRACTION(n)
#define TINYRATE (1.0 / (double)0x7FFFFFFF)
#define MAKEFIXED(n) DBLMAKEWHOLE(n)
#endif

typedef LFIXED FAR *LPLFIXED;
typedef LPLFIXED FAR *LPPLFIXED;

//**********************************************************************
// Fixed-Point Point definition
//**********************************************************************
typedef struct _fpoint
	{
	LFIXED	fx;
	LFIXED	fy;
} FPOINT;
typedef FPOINT FAR *LPFPOINT;

//**********************************************************************
// Fixed-Point Rectangle definition
//**********************************************************************
typedef struct _frect
	{
	LFIXED	left;
	LFIXED	top;
	LFIXED	right;
	LFIXED	bottom;
} FRECT;
typedef FRECT FAR *LPFRECT;

LPSTR    FixedAscii( LFIXED Fixed, LPSTR lpAscii, int Precision );
LFIXED   AsciiFixed( LPSTR lpAscii );
LFIXED16  fget16( long numer, long denom );
LFIXED16  special_fget16( LFIXED16 numer, LFIXED16 denom );
long    fmul16( long a, long b );
long    tmul16( long a, long b );

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
typedef void *( FAR * LPKCMSCNCTPROC )( LPFRMTYPEINFO, LPFRMTYPEINFO,
	LPVOID pXform = NULL, LONG size = NULL );
typedef void ( FAR * LPKCMSEVALPROC )( LPFRMTYPEINFO, LPFRMTYPEINFO,
	LPVOID, LPTR, LPTR, int, int nLines );
typedef void ( FAR * LPKCMSFREEPROC )( LPVOID );
typedef BOOL ( FAR * LPKCMSUIDMPROC )( LPFRMTYPEINFO, LPFRMTYPEINFO );
typedef DWORD ( FAR * LPFRAMEREADPROC ) ( DWORD dwData, LPTR lpDst, DWORD dwBytes );

/*==========================================================*\
	Dither Type enumeration definition - For CFrameTypeConvert
\*==========================================================*/

typedef enum
	{
	DT_DEFAULT,
	DT_NONE,
	DT_SCATTER,
	DT_PATTERN
	} DITHER_TYPE;

// Frame Convert type  class
class FRAME_CLASS_EXPORT CFrameTypeConvert
{
typedef void ( FAR CFrameTypeConvert::* LPCONVERTPROC )( LPTR lpSrc, LPTR lpDst, int y, int iCount );

public:
	// Constructors
 	CFrameTypeConvert();

	// Destructor
	~CFrameTypeConvert();						

	// initialize the frame type converter
	BOOL Init(FRMTYPEINFO SrcTypeInfo, FRMTYPEINFO DstTypeInfo, int iMaxConvertWidth,
				DITHER_TYPE DitherType = DT_DEFAULT, LPVOID pXform = NULL,
				LONG size = NULL);

	void ConvertData(LPTR lpSrc, LPTR lpDst, int i, int iCount, int nLines = 1);

	// reset the frame type converter
	void Reset();


private:
	void CopyData(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
public:
	void LineArtToGrayScale(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void LineArtToPaletteColor(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
public:
	void LineArtToRGBColor(
		LPTR lpSrc,
		LPRGB lpDst,
		int y,
		int iCount);
public:
	void LineArtToCMYKColor(
		LPTR lpSrc,
		LPCMYK lpDst,
		int y,
		int iCount);
private:
	void GrayScaleToScatter(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
public:
	void GrayScaleToLineArt(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void GrayScaleToPaletteColor(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void GrayScaleToPaletteColorScatter(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void GrayScaleToPaletteColorPattern(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
public:
	void GrayScaleToRGBColor(
		LPTR lpSrc,
		LPRGB lpDst,
		int y,
		int iCount);
public:
	void GrayScaleToCMYKColor(
		LPTR lpSrc,
		LPCMYK lpDst,
		int y,
		int iCount);
private:
	void PaletteColorToScatter(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void PaletteColorToLineArt(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void PaletteColorToGrayScale(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void PaletteColorToPaletteColor(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void PaletteColorToPaletteColorScatter(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void PaletteColorToPaletteColorPattern(
		LPTR lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void PaletteColorToRGBColor(
		LPTR lpSrc,
		LPRGB lpDst,
		int y,
		int iCount);
private:
	void PaletteColorToCMYKColor(
		LPTR lpSrc,
		LPCMYK lpDst,
		int y,
		int iCount);
private:
	void RGBColorToScatter(
		LPRGB lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
public:
	void RGBColorToLineArt(
		LPRGB lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
public:
	void RGBColorToGrayScale(
		LPRGB lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void RGBColorToPaletteColor(
		LPRGB lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void RGBColorToPaletteColorScatter(
		LPRGB lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void RGBColorToPaletteColorPattern(
		LPRGB lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
public:
	void RGBColorToCMYKColor(
		LPRGB lpSrc,
		LPCMYK lpDst,
		int y,
		int iCount);
private:
	void CMYKColorToScatter(
		LPCMYK lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
public:
	void CMYKColorToLineArt(
		LPCMYK lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
public:
	void CMYKColorToGrayScale(
		LPCMYK lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void CMYKColorToPaletteColor(
		LPCMYK lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void CMYKColorToPaletteColorScatter(
		LPCMYK lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
private:
	void CMYKColorToPaletteColorPattern(
		LPCMYK lpSrc,
		LPTR lpDst,
		int y,
		int iCount);
public:
	void CMYKColorToRGBColor(
		LPCMYK lpSrc,
		LPRGB lpDst,
		int y,
		int iCount);

private:
	FRMTYPEINFO		SrcType;
	FRMTYPEINFO 	DstType;
	int				iWidth;
	LPTR			lpPaletteLUT;
	short FAR *		lpErrorLine;
	BYTE			Black;
	BYTE			White;
	int				Depth;
	LPCONVERTPROC	lpConvertProc;
	LPTR			lpErrorLUT;
	LPKCMSCNCTPROC	lpKcmsCnctProc;
	LPKCMSEVALPROC	lpKcmsEvalProc;
	LPKCMSFREEPROC	lpKcmsFreeProc;
	LPVOID			KcmXform;
	LPKCMSUIDMPROC	lpKcmsUidmProc;
};

// Frame Conversion from one type to another
extern  LPFRAME FrameConvert(LPFRAME lpSrcFrame, FRMDATATYPE fdtDst,
                FRMCNVTCBPROC lpStatusCallback, DITHER_TYPE DitherType = DT_DEFAULT);
extern  LPFRAME FrameConvert(LPFRAME lpSrcFrame, FRMTYPEINFO DstTypeInfo,
                FRMCNVTCBPROC lpStatusCallback, DITHER_TYPE DitherType = DT_DEFAULT);

/*=========================================================================*\
	frmmain.c
\*=========================================================================*/

LPFRAME FrameGetCurrent(void);
LPFRAME FrameSetCurrent(LPFRAME lpFrame);

void FrameSettings( LPSTR lpRamDisk, int iMainMemMin, int iMainMemFactor);
int  FrameGetError( void );
void FrameSetError( int iErrorCode );

LPFRAME FrameOpen( FRMDATATYPE DataType, int XSize, int YSize, int Resolution );
LPFRAME FrameOpen( FRMTYPEINFO TypeInfo, int XSize, int YSize, int Resolution );

LPFRMTYPEINFO FrameSetTypeInfo( LPFRMTYPEINFO lpTypeInfo, FRMDATATYPE DataType, LPFRMCOLORMAP ColorMap = NULL);
LPFRMTYPEINFO FrameSetTypeInfo( LPFRMTYPEINFO lpTypeInfo, FRMDATATYPE DataType, LPFRMCOLORMAP ColorMap,
	LPPTINFO lpptInfo);

void        FrameClose( LPFRAME lpFrame );
BOOL        FrameRead( LPFRAME lpFrame, int sx, int sy, int dx, LPTR pixels, int count );
BOOL        FrameWrite( LPFRAME lpFrame, int sx, int sy, int dx, LPTR pixels, int count );
int         FrameMode( LPFRAME lpFrame, int ReleaseMode );
int         FrameInMemory( LPFRAME lpFrame );
LPTR        FramePointer( LPFRAME lpFrame, int x, int y, BOOL bModify, int dx = 0 );
LPTR        FramePointerRaw( LPFRAME lpFrame, int x, int y, BOOL bModify, int dx = 0 );
void        FrameSample( LPFRAME lpFrame, LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );
void        FrameSampleMW(LPFRAME lpFrame, LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate, LPTR mask);
LPSTR       FrameName(LPFRAME lpFrame);
FRMDATATYPE FrameType( LPFRAME lpFrame );
FRMTYPEINFO FrameTypeInfo( LPFRAME lpFrame );
BOOL FrameTypeInfoEqual( FRMTYPEINFO TypeInfo1, FRMTYPEINFO TypeInfo2);
LPFRMTYPEINFO FrameGetTypeInfo( LPFRAME lpFrame, LPFRMTYPEINFO lpTypeInfo );
int         FrameDepth( LPFRAME lpFrame );
int         FrameXSize( LPFRAME lpFrame );
int         FrameYSize( LPFRAME lpFrame );
int         FrameByteWidth( LPFRAME lpFrame );
int         FrameResolution( LPFRAME lpFrame );
int         FrameSetResolution( LPFRAME lpFrame, int Resolution );
void        FrameSetBackground(LPFRAME lpFrame, BYTE background);
BYTE        FrameGetBackground(LPFRAME);
BOOL        FrameSetColorMap( LPFRAME lpFrame, LPFRMCOLORMAP LPFRMCOLORMAP);
LPFRMCOLORMAP  FrameGetColorMap( LPFRAME lpFrame );
LPFRMCOLORMAP  FrameCreateColorMap( void );
LPFRMCOLORMAP  FrameCreateFixedColorMap( void );
LPFRMCOLORMAP  FrameCopyColorMap( LPFRMCOLORMAP LPFRMCOLORMAP );
void		 FrameCopyColorMap( LPFRMCOLORMAP lpSrcMap, LPFRMCOLORMAP lpDstMap );
void        FrameDestroyColorMap( LPFRMCOLORMAP LPFRMCOLORMAP );
BYTE        FrameGetNearestIndex(LPFRMCOLORMAP LPFRMCOLORMAP, LPRGB lpRGB);
BOOL		 FrameColorMapsEqual(LPFRMCOLORMAP LPFRMCOLORMAP1, LPFRMCOLORMAP LPFRMCOLORMAP2);

VOID        FrameDuplicateLine( LPFRAME lpInFrame, LPFRAME lpOutFrame, int iny, int outy );
VOID        FrameCopyLine( LPFRAME lpInFrame, LPFRAME lpOutFrame, int y );
VOID        FrameExchLine( LPFRAME lpInFrame, LPFRAME lpOutFrame, int y );
LPTR        BumpPtr( LPTR lp, DWORD lOffset );
int         FramePreload( LPFRAME lpFrame, BOOL invert, DWORD dwReadData, LPFRAMEREADPROC lpReadProc, DWORD dwStatusData, LPFRAMESTATUSPROC lpStatusCallback);
LPFRAME     FrameCopy( LPFRAME lpSrcFrame, LPRECT lpRect );
LPTR        LineInMemory( LPFRAME lpFrame, int y);

/*=========================================================================*\
	frmpixel.c
\*=========================================================================*/

void FrameRGB2Pixel( LPFRAME lpFrame, LPRGB  lpFullRGB, LPLONG lpPixel );
void FramePixel2RGB( LPFRAME lpFrame, LPLONG lpPixel,   LPRGB  lpFullRGB );

void FrameGetRGB( LPFRAME lpFrame, LPTR  lpSrc,    LPRGB lpDstRGB, int iPixelCount);
void FramePutRGB( LPFRAME lpFrame, LPRGB lpSrcRGB, LPTR  lpDst,    int iPixelCount);
void FrameSetRGB( LPFRAME lpFrame, LPRGB lpRGBPixel, LPTR lpDst, int iPixelCount);

void FrameGetPixel( LPFRAME lpFrame, LPTR   lpSrc, LPLONG lpDst, int iPixelCount);
void FramePutPixel( LPFRAME lpFrame, LPLONG lpSrc, LPTR   lpDst, int iPixelCount);
void FrameSetPixel( LPFRAME lpFrame, long lPixel, LPTR lpDst, int iPixelCount);
void FrameSetPixelEx( FRMDATATYPE DataType, long lPixel, LPTR lpDst, int iPixelCount);

/*=========================================================================*\
	frmdib.c
\*=========================================================================*/

LPTR    FrameToDIB( LPFRAME lpFrame, LPRECT lpRect );
LPFRAME DIBToFrame( LPTR lpDIBMem, BOOL bForceRGB );

/*=========================================================================*\
	frmhsl.c
\*=========================================================================*/

void SetRGBToCMYKProc( RGB2CMYKPROC lpRGBtoCMYK, CMYK2RGBPROC lpCMYKtoRGB);
void RGBtoCMYK( BYTE R, BYTE G, BYTE B, LPCMYK lpCMYK );
void RGBtoCMYKScanline( LPRGB lpRGB, LPCMYK lpCMYK, int iCount );

void CMYKtoRGB( BYTE C, BYTE M, BYTE Y, BYTE K, LPRGB lpRGB );
void CMYKtoRGBScanline( LPCMYK lpCMYK, LPRGB lpRGB, int iCount );
void CMYKtoRGBBuf(LPCMYK lpCMYK, LPRGB lpRGB, int dx, FRMTYPEINFO TypeInfo, int dy);

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
	frmmini.c
\*=========================================================================*/

void MiniRGBtoRGB( WORD miniRGB, LPRGB lpRGB );
BYTE MiniRGBtoL( WORD miniRGB );
void MiniRGBtoHSL( WORD miniRGB, LPHSL lpHSL );
WORD HSLtoMiniRGB( LPHSL lpHSL );

/*=========================================================================*\
	frmsmple.c
\*=========================================================================*/
#ifndef C_CODE
extern "C" {
#endif
void Sample_1(  LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );
void Sample_8(  LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );
void Sample_24( LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );
void Sample_32( LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );

void Sample_1MW(  LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate, LPTR mask );
void Sample_8MW(  LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate, LPTR mask );
void Sample_24MW( LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate, LPTR mask );
void Sample_32MW( LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate, LPTR mask );
#ifndef C_CODE
}
#endif

/*=========================================================================*\
	palette.c
\*=========================================================================*/
LPTR CreatePaletteLut15(LPRGB lpRGBmap, int nCubes, LPTR lpLut15, STATUSCALLBACKPROC lpStatusCallback);
void CreateErrorLut15(LPRGB lpRGBmap, LPTR lpTable, LPTR lpErrorTable, BOOL bDither);

/*=========================================================================*\
	Frame access macros
\*=========================================================================*/

#define DN(v) ((WORD)(v)>>3)
#define RGBtoMiniRGB(R) \
	(((WORD) ( DN((R)->blue) << 5 | DN((R)->green) ) << 5) | DN((R)->red) )
#define GraytoMiniRGB(G) \
	(((WORD) ( DN((G)) << 5 | DN((G)) ) << 5) | DN((G)) )
#define RGB3toMiniRGB(r, g, b) \
	(((WORD) ( DN((b)) << 5 | DN((g)) ) << 5) | DN((r)) )
#define ToMiniRGB(r,g,b) \
	( (((WORD)(b) << 5 | (WORD)(g)) << 5) | (WORD)(r) )
#define MiniR(v) (BYTE)( 31  & ( (WORD)(v)       ) )
#define MiniG(v) (BYTE)( 31  & ( (WORD)(v) >> 5  ) )
#define MiniB(v) (BYTE)( 31  & ( (WORD)(v) >> 10 ) )
#define MaxiR(v) (BYTE)( ToMaxi[ MiniR(v) ] )
#define MaxiG(v) (BYTE)( ToMaxi[ MiniG(v) ] )
#define MaxiB(v) (BYTE)( ToMaxi[ MiniB(v) ] )
#define AVG(a,b) (((a)+(b))>>1)
#define TOLUM(r,g,b) ( r<g ? ( r>b ? AVG(b,g) : (g>b ? AVG(g,r) : AVG(b,r)) ) \
			   : ( g>b ? AVG(b,r) : (r>b ? AVG(g,r) : AVG(b,g)) ) )
#define RGB2long(rgb) RGB( (rgb).red, (rgb).green, (rgb).blue )
#define CopyRGB(s, d)                                                   \
{                                                                       \
*((LPTR)(d)) = *((LPTR)(s));                                            \
*((LPTR)(d)+1) = *((LPTR)(s)+1);                                        \
*((LPTR)(d)+2) = *((LPTR)(s)+2);                                        \
}

#define CopyRGBHuge(s, d)                                               \
{                                                                       \
*((HPTR)(d)) = *((HPTR)(s));                                            \
*((HPTR)(d)+1) = *((HPTR)(s)+1);                                        \
*((HPTR)(d)+2) = *((HPTR)(s)+2);                                        \
}

#define CopyCMYK(s, d)                                                  \
{                                                                       \
*((LPDWORD)(d)) = *((LPDWORD)(s));                                      \
}

#define CopyCMYKHuge(s, d)                                              \
{                                                                       \
*((HPDWORD)(d)) = *((HPDWORD)(s));                                      \
}


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

#endif /* __mgxframe_h__ */


