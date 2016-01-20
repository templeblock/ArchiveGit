/*=======================================================================*\

	FRAMELIB.H - Frame Library include header file.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifndef __framelib_h__
#define __framelib_h__
#ifdef USE_CFILE
#include <afxwin.h>
#else
#include <windows.h>
#endif
#include "types.h"
#include "fixed.h"
#include <sys/types.h>
#include <sys/stat.h>		  
#include <dos.h>
#ifndef WIN32
#include "winerror.h" // swiped from WIN32 sdk
#endif
#ifdef WIN32
#define NO_CACHE
#endif

#ifdef WIN32
#define STATIC16
#else
#define STATIC16 static
#endif

#define STATICTABLE static const
#define TABLE const

#define BIT1		1L
#define BIT2		2L
#define BIT3		4L
#define BIT6		32L
#define BIT18		131072L
#define BITMASK		( BIT1| BIT2| BIT3| BIT6 )
#define TOGRAY(n)	((int)( ((255L * (n)) +  50) / 100 ))

#define MAX_IMAGE_WIDTH	INT_MAX
#define MAX_IMAGE_HEIGHT	INT_MAX


#define MW_ESCAPES      0x0001          /* MatchWildcard options            */
#define MW_CASE         0x0002

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
	File I/O definitions
\*==========================================================*/
#define FO_READ	0x00000001
#define FO_WRITE	0x00000002
#define FO_CREATE	0x00000004
#define FO_HIDDEN 0x00000008

#define FILE_DWERR (DWORD)0xffffffff

#ifdef WIN32
#define FILE_HANDLE HANDLE
#define FILE_HANDLE_INVALID INVALID_HANDLE_VALUE
#else
#define FILE_HANDLE int
#define FILE_HANDLE_INVALID -1
#endif

#ifndef WIN32
typedef struct _filetime
{
unsigned	time;
unsigned	date;
} FILETIMES, FAR *LPFILETIMES;
#else
#define FILETIMES FILETIME
typedef FILETIMES FAR *LPFILETIMES;
#endif

/*==========================================================*\
	Dos_FindFirst definitions
\*==========================================================*/
#define ATTRIB_NORMAL		0x00000001
#define ATTRIB_SUBDIR		0x00000002
#define ATTRIB_READONLY	0x00000004
#define ATTRIB_SYSTEM		0x00000008
#define ATTRIB_HIDDEN		0x00000010
#define ATTRIB_ARCHIVE		0x00000020
#define ATTRIB_VOLID		0x00000040
#define ATTRIB_ALL (ATTRIB_NORMAL|ATTRIB_SUBDIR|ATTRIB_READONLY| \
					ATTRIB_SYSTEM|ATTRIB_HIDDEN|ATTRIB_ARCHIVE)

/*==========================================================*\
	Additional drive type definitions - in WIN32 and not WIN16
\*==========================================================*/
#ifndef WIN32
#define DRIVE_CDROM     5
#define DRIVE_RAMDISK   6
#endif



// C++ Strict Prototypes for Function Ptrs.
typedef BOOL ( FAR *STATUSCALLBACKPROC )( int, int, BOOL );

typedef int ( FAR *SAMPLETBLPROC )( LPTR, int, LPTR, int, int, LFIXED );
typedef int ( FAR *SAMPLEMWTBLPROC )( LPTR, int, LPTR, int, int, LFIXED, LPTR );
typedef int ( FAR *RGB2PIXELTBLPROC )( struct _frame far *, struct _rgb far * );
typedef int ( FAR *PIXEL2RGBTBLPROC )( struct _frame far *, LPLONG, struct _rgb far * );
typedef int ( FAR *SETRGBTBLPROC )( struct _frame far *, struct _rgb far *, LPTR, int );
typedef int ( FAR *GETPIXELTBLPROC )( struct _frame far *, LPTR, LPLONG, int );
typedef int ( FAR *PUTPIXELTBLPROC )( struct _frame far *, LPLONG, LPTR, int );
typedef int ( FAR *SETPIXELTBLPROC )( long, LPTR, int );
typedef void ( FAR *RGB2CMYKPROC )( struct _rgb far *, struct _cmyk far *, int );
typedef void ( FAR *CMYK2RGBPROC )( struct _cmyk far *, struct _rgb far *, int );
typedef int ( FAR *FRMCNVTCBPROC )( int, int, int );

typedef int ( FAR *L3PROC )( int, int, int );
typedef int ( FAR *L4PROC )( LPSTR, LPSTR, long, int );

typedef struct _finddata
{
FNAME				szName;
DWORD				dwAttrib;
DWORD				dwHighSize;
DWORD				dwLowSize;
FILETIMES			ftLastWriteTime;
#ifdef WIN32
HANDLE				hFindFile;
DWORD				dwFindAttrib;
#else
struct find_t 		ft;
#endif
} FINDDATA, FAR *LPFINDDATA;

/*==========================================================*\
	Colormap type definition
\*==========================================================*/

typedef struct _colormap
{
 	int  NumEntries;
	RGBS LastRGB;
	int	 LastIndex;
	RGBS RGBData[1];
} COLORMAP, FAR *LPCOLORMAP;

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

typedef struct
{
	FRMDATATYPE DataType;
	LPCOLORMAP	ColorMap;
}
FRMTYPEINFO, FAR *LPFRMTYPEINFO;

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
	LPCOLORMAP ColorMap;
	FNAME   CacheFileName;
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
} FRAME, FAR *LPFRAME;

/*==========================================================*\
	Span vector definition
\*==========================================================*/

typedef struct _span
{
	int sx;
	int sy;
	int dx;
	int dy;
} SPAN, FAR *LPSPAN;

/*==========================================================*\
	Combine Data ROPS definition
\*==========================================================*/

typedef enum
	{
	CR_COPY,
	CR_OR,
	CR_AND,
	CR_XOR,
	CR_MULTIPLY
	} COMBINE_ROP;

typedef enum
	{
	DT_DEFAULT,
	DT_NONE,
	DT_SCATTER,
	DT_PATTERN
	} DITHER_TYPE;


/*========================================*\
	C++ Garbage... !!! Not Garbage any more
\*========================================*/

/*=========================================================================*\
	qmul.asm
\*=========================================================================*/

#ifndef C_CODE
extern "C" {
#endif
void quick_average24(LPTR, LPTR, long, long, long, long, LPTR);
void quick_average32(LPTR, LPTR, long, long, long, long, LPTR);
#ifndef C_CODE
}
#endif
#if 1//#ifdef WIN32
#define quick_lmul(n1, n2) ((n1) * (n2))
#define quick_lmul2(num1, num2, num3, num4) \
	((num1) * (num2)) + ((num3) * (num4))
#define quick_average8(lpBuf1, lpBuf2, area1, area2, area3, area4, lpDst) \
	(*lpDst = (((lpBuf1)[0] * (area1) + (lpBuf1)[1] * (area2) +			\
	(lpBuf2)[0] * (area3) + (lpBuf2)[1] * (area4)) >> 16))
#else
#ifndef C_CODE
extern "C" {
#endif
long  quick_lmul(long, long);
long  quick_lmul2(long, long, long, long);
short quick_lmul4shr16(long, long, long, long, long, long, long, long);
void quick_average8(LPTR, LPTR, long, long, long, long, LPTR);
#ifndef C_CODE
}
#endif
#endif

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
// Frame Convert type class
class CFrameTypeConvert
{
typedef void ( FAR CFrameTypeConvert::* LPCONVERTPROC )( LPTR lpSrc, LPTR lpDst, int y, int iCount );

public:
	// Constructors
 	CFrameTypeConvert();

	// Destructor
	~CFrameTypeConvert();						

	// initialize the frame type converter
	BOOL Init(FRMTYPEINFO SrcTypeInfo, FRMTYPEINFO DstTypeInfo, int iMaxConvertWidth,
				DITHER_TYPE DitherType = DT_DEFAULT);

	void ConvertData(LPTR lpSrc, LPTR lpDst, int i, int iCount);

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
	LPINT16			lpErrorLine;
	BYTE			Black;
	BYTE			White;
	int				Depth;
	LPCONVERTPROC	lpConvertProc;
	LPTR			lpErrorLUT;
};

// Frame Conversion from one type to another
extern  LPFRAME FrameConvert(LPFRAME lpSrcFrame, FRMDATATYPE fdtDst,
                FRMCNVTCBPROC lpStatusCallback);

#ifdef UNUSED
/*=========================================================================*\
	frmcnvrt.c
\*=========================================================================*/

void ConvertData(LPTR lpIn, int iInDepth, int iCount, LPTR lpOut, int iOutDepth);

void con1to8(  LPTR   lpIn,     int iCount,LPTR   lpOut, int normal);
void con1to16( LPTR   lpIn,     int iCount,LPWORD lpOut);
void con1to24( LPTR   lpIn,     int iCount,LPRGB  lpOut);

void con8to1(  LPTR   lpIn,     int iCount,LPTR   lpOut);
void con8to16( LPTR   lpIn,     int iCount,LPWORD lpOut);
void con8to24( LPTR   lpIn,     int iCount,LPRGB  lpOut);

void con16to1( LPWORD lpIn,     int iCount,LPTR   lpOut);
void con16to8( LPWORD lpMiniRGB,int iCount,LPTR   lpOut);
void con16to24(LPWORD lpMiniRGB,int iCount,LPRGB  lpRGB);

void con24to1( LPRGB  lpIn,     int iCount,LPTR   lpOut);
void con24to8( LPRGB  lpIn,     int iCount,LPTR   lpOut);
void con24to16(LPRGB  lpRGB,    int iCount,LPWORD lpMiniRGB);

void diffuse(int x, int y, int iColor, LPTR lpLut,	LPTR lpIn, int iCount, LPTR lpOut);
void dither(int x, int y, LPTR lpIn, int iCount, LPTR lpOut);

#endif // UNUSED

/*=========================================================================*\
	frmfiles.c
\*=========================================================================*/
#ifndef WIN32
#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2
#endif
FILE_HANDLE FileOpen(LPSTR lpFileName, DWORD dwMode);
BOOL FileClose(FILE_HANDLE handle);
DWORD FileLastError(void);
DWORD FileRead(FILE_HANDLE handle, LPVOID lpBuf, DWORD dwBytes);
DWORD FileWrite(FILE_HANDLE handle, LPVOID lpBuf, DWORD dwBytes);
BOOL FileTruncate(FILE_HANDLE handle);
DWORD FileSeek(FILE_HANDLE handle, long lAmount, int dir);
LPSTR GetRootDirectory(LPSTR lpPath, LPSTR lpRoot);
void FixFileName( LPSTR lpPathName );
LPSTR stripdir( LPSTR lpPathName );
LPSTR filename( LPSTR lpPathName );
LPSTR stripfile( LPSTR lpPath );
LPSTR stripext( LPSTR lpPath );
LPSTR FixPath( LPSTR lpPath );
LPSTR extension( LPSTR lpPath );
LPSTR GetCurrentDir( LPSTR lpBuffer, int maxlen );
BOOL SetCurrentDir( LPSTR lpCurrentDir );
BOOL DiskSpaceEx( LPSTR lpPath, LPDWORD lpdwTotal, LPDWORD lpdwFree );
DWORD DiskSpace( LPSTR lpPath );
UINT GetExtendedDriveType(int i);
BOOL FileCopy( LPSTR lpInFile, LPSTR lpOutFile );
#ifndef WIN32
BOOL CopyFile( LPSTR lpInFile, LPSTR lpOutFile, BOOL fFailExists );
#endif
BOOL SafeFileExists( LPSTR szFileName );
int SafeStat(LPSTR lpFile, struct _stat FAR *buffer);
BOOL FileExists( LPSTR szFileName );
BOOL FileAccess( LPSTR szFileName, WORD wStyle );
BOOL FilePermission( LPSTR lpFileName, int mode);
BOOL FileDate( LPSTR lpFileName, LPFILETIMES lpTime );
LPSTR FileTimeToString( LPFILETIMES lpFileTime, LPSTR lpString );
long FileSize( LPSTR lpFileName );
BOOL FileDelete( LPSTR lpFileName );
void WildcardDelete( LPSTR lpPath, LPSTR lpName );
BOOL GetTempFilename( LPSTR lpTempName, LPSTR lpExtension );
LPSTR GetVolumnID( LPSTR lpPath, LPSTR lpVolumn );
BOOL GetVolumeSerialNumber( LPSTR lpPath, LPDWORD lpSerialNumber );
int EnumFiles(LPSTR lpSrcName, LPSTR lpDstName, L4PROC lpEnumProc, long lData);
LPSTR MakeFullPath(LPSTR lpPath);
BOOL Dos_FindFirst(LPSTR match, DWORD dwAttrib, LPFINDDATA lpFind);
BOOL Dos_FindNext(LPFINDDATA lpFind);
BOOL Dos_FindClose(LPFINDDATA lpFind);
BOOL Dos_GetFileAttr(LPSTR lpFile, LPDWORD lpAttrib);
int RenameFile(LPSTR lpSrc, LPSTR lpDst);
LPSTR MkTemp(LPSTR lpTemplate);
int MkDir(LPSTR lpDir);
int RmDir(LPSTR lpDir);
int ChDir(LPSTR lpDir);
char *GetCwd(char *pDir, int len);
int GetDrive();
BOOL SetDrive(int drive);
BOOL SafeFileStats(LPSTR lpFile, LPUINT32 lpSize, LPUINT32 lpCTime, LPUINT32 lpMTime, LPUINT32 lpATime);
#ifndef WIN32
DWORD GetLastError(void);
#endif

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

void        FrameClose( LPFRAME lpFrame );
BOOL        FrameRead( LPFRAME lpFrame, int sx, int sy, int dx, LPTR pixels, int count );
BOOL        FrameWrite( LPFRAME lpFrame, int sx, int sy, int dx, LPTR pixels, int count );
int         FrameMode( LPFRAME lpFrame, int ReleaseMode );
int         FrameInMemory( LPFRAME lpFrame );
LPTR        FramePointer( LPFRAME lpFrame, int x, int y, BOOL bModify );
LPTR        FramePointerRaw( LPFRAME lpFrame, int x, int y, BOOL bModify );
void        FrameSample( LPFRAME lpFrame, LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );
void        FrameSampleMW(LPFRAME lpFrame, LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate, LPTR mask);
LPSTR       FrameName(LPFRAME lpFrame);
FRMDATATYPE FrameType( LPFRAME lpFrame );
FRMTYPEINFO FrameTypeInfo( LPFRAME lpFrame );
BOOL FrameTypeInfoEqual( FRMTYPEINFO TypeInfo1, FRMTYPEINFO TypeInfo2);
LPFRMTYPEINFO FrameGetTypeInfo( LPFRAME lpFrame, LPFRMTYPEINFO lpTypeInfo );
LPFRMTYPEINFO FrameSetTypeInfo( LPFRMTYPEINFO lpTypeInfo, FRMDATATYPE DataType, LPCOLORMAP ColorMap = NULL);
int         FrameDepth( LPFRAME lpFrame );
int         FrameXSize( LPFRAME lpFrame );
int         FrameYSize( LPFRAME lpFrame );
int         FrameByteWidth( LPFRAME lpFrame );
int         FrameResolution( LPFRAME lpFrame );
int         FrameSetResolution( LPFRAME lpFrame, int Resolution );
void        FrameSetBackground(LPFRAME lpFrame, BYTE background);
BYTE        FrameGetBackground(LPFRAME);
BOOL        FrameSetColorMap( LPFRAME lpFrame, LPCOLORMAP lpColorMap);
LPCOLORMAP  FrameGetColorMap( LPFRAME lpFrame );
LPCOLORMAP  FrameCreateColorMap( void );
LPCOLORMAP  FrameCreateFixedColorMap( void );
LPCOLORMAP  FrameCopyColorMap( LPCOLORMAP lpColorMap );
void		 FrameCopyColorMap( LPCOLORMAP lpSrcMap, LPCOLORMAP lpDstMap );
void        FrameDestroyColorMap( LPCOLORMAP lpColorMap );
BYTE        FrameGetNearestIndex(LPCOLORMAP lpColorMap, LPRGB lpRGB);
BOOL		 FrameColorMapsEqual(LPCOLORMAP lpColorMap1, LPCOLORMAP lpColorMap2);

VOID        FrameDuplicateLine( LPFRAME lpInFrame, LPFRAME lpOutFrame, int iny, int outy );
VOID        FrameCopyLine( LPFRAME lpInFrame, LPFRAME lpOutFrame, int y );
VOID        FrameExchLine( LPFRAME lpInFrame, LPFRAME lpOutFrame, int y );
LPTR        BumpPtr( LPTR lp, DWORD lOffset );
#ifdef USE_CFILE
int         FramePreload( LPFRAME lpFrame, CFile *pFile, BOOL invert, L3PROC lpStatusCallback);
#else
int         FramePreload( LPFRAME lpFrame, FILE_HANDLE fh, BOOL invert, L3PROC lpStatusCallback);
#endif
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
#ifndef C_CODE
extern "C" {
#endif
void RGBtoHSL( BYTE, BYTE, BYTE, LPHSL );
void HSLtoRGB( BYTE, BYTE, BYTE, LPRGB );
#ifndef C_CODE
}
#endif

/*=========================================================================*\
	frmmap.c
\*=========================================================================*/

void map( LPTR lut, LPTR buf, int count );
void MapRGB8(  LPTR lpRLut, LPTR lpGLut, LPTR lpBLut, LPTR   lpSrc, LPTR   lpDst, int iCount);
void MapRGB24( LPTR lpRLut, LPTR lpGLut, LPTR lpBLut, LPRGB  lpSrc, LPRGB  lpDst, int iCount);
void MapRGB32( LPTR lpRLut, LPTR lpGLut, LPTR lpBLut, LPCMYK lpSrc, LPCMYK lpDst, int iCount);

/*=========================================================================*\
	frmmem.c
\*=========================================================================*/

LPTR Alloc( long lCount );
LPTR AllocX( long lCount, WORD wFlags );
LPTR AllocExtend( LPTR lpMemory, long lCount );
void FreeUp( LPVOID lpMemory );
DWORD GlobalPtrSize( LPVOID lpMemory );
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
	frmtform.c
\*=========================================================================*/
//**********************************************************************
// Transformation matrix definition
//**********************************************************************
typedef struct _tform
	{
	LFIXED ax;			//  ax  ay  0
	LFIXED ay; 
	LFIXED bx;			//  bx  by  0
	LFIXED by;
	LFIXED cx;			//  cx  cy  1
	LFIXED cy;
} TFORM;
typedef TFORM FAR *LPTFORM;

void TInit( LPTFORM lpTForm );
void TMultiply( LPTFORM lpTFormS1, LPTFORM lpTFormS2, LPTFORM lpTFormD );
void TMove( LPTFORM lpTForm, int tx, int ty );
void TFMove( LPTFORM lpTForm, LFIXED fx, LFIXED fy );
void TScale( LPTFORM lpTForm, LFIXED sx, LFIXED sy );
void TShearX( LPTFORM lpTForm, int height, int dx );
void TShearY( LPTFORM lpTForm, int width, int dy );
void TRotate( LPTFORM lpTForm, LFIXED anglex, LFIXED angley );
void TInvert( LPTFORM lpTForm );
void FTransformer( LPTFORM lpTForm, LPPOINT lpPoint, LPLFIXED lpx, LPLFIXED lpy );
void Transformer( LPTFORM lpTForm, LPPOINT lpPoint, LPINT lpx, LPINT lpy );
void TransformRect( LPTFORM lpTForm, LPRECT lpRect, LPRECT lpNewRect );

/*=========================================================================*\
	frmutil.c
\*=========================================================================*/

int bound( int, int, int );
LONG Bits(unsigned long val);
LPRECT AstralToWindowsRect(LPRECT lpDest);
LPRECT WindowsToAstralRect(LPRECT lpDest);
void AstralSetRectEmpty(LPRECT lpDestRect);
BOOL AstralIsRectEmpty(LPRECT lpRect);
void AstralUnionRect( LPRECT lpDestRect, LPRECT lpSrc1Rect, LPRECT lpSrc2Rect );
BOOL AstralIntersectRect( LPRECT lpDest, LPRECT lpSrc1, LPRECT lpSrc2 );
void AstralSubtractRect( LPRECT lpDest, LPRECT lpSrc );
BOOL AstralInsideRect( LPRECT lpDest, LPRECT lpSrc );
void BoundRect( LPRECT lpRect, int left, int top, int right, int bottom );
void lmemcpy( LPTR lpOut, LPTR lpIn, int iCount );
void Mirror8Huge( HPTR pixels, int count );
void Mirror24Huge( HPTR pixels, int count );
void Mirror32Huge( HPTR pixels, int count );
void mirror8( LPTR pixels, int count );
void mirror24( LPTR pixels, int count );
void mirror32( LPTR pixels, int count );
LFIXED ScaleToFit( LPINT DestWidth, LPINT DestHeight, int SrcWidth, int SrcHeight );
void ramp( LPTR lut, int inx1, int inx2, int outx1, int outx2 );
void CopyToHuge(LPTR lpSrc, HPTR hpDst, DWORD dwCount);
void CopyFromHuge(HPTR hpSrc, LPTR lpDst, DWORD dwCount);
void hcopy(HPTR hpSrc, HPTR hpDst, DWORD dwCount);
void hclr( HPTR hpDst, DWORD dwCount );
void hset( HPTR hpDst, DWORD dwCount, int iValue );
void hset24( HPRGB hpDst, DWORD dwCount, long lValue );
void hset32( HPCMYK hpDst, DWORD dwCount, long lValue );
#ifndef C_CODE
extern "C" {
#endif
void copy( LPTR lpSrc, LPTR lpDst, int iCount );
#ifndef C_CODE
}
#endif

#ifndef WIN32
void MoveMemory (LPTR lpDestination, LPTR lpSource, int Length);
#endif

void swapBGR( LPTR lpSrc, LPTR lpDst, int iCount );
void or( LPTR lpSrc, LPTR lpDst, int iCount );
void nor( LPTR lpSrc, LPTR lpDst, int iCount );
void and( LPTR lpSrc, LPTR lpDst, int iCount );
void nand( LPTR lpSrc, LPTR lpDst, int iCount );
void xor( LPTR lpSrc, LPTR lpDst, int iCount );
void nxor( LPTR lpSrc, LPTR lpDst, int iCount );
void mul( LPTR lpSrc, LPTR lpDst, int iCount );
void nmul( LPTR lpSrc, LPTR lpDst, int iCount );
void not( LPTR lpSrc, LPTR lpDst, int iCount );
void CombineData( LPTR lp, LPTR lpM, int iCount, BOOL bInvert, COMBINE_ROP ROP );
void negate( LPTR buf, long lCount );
#ifndef C_CODE
extern "C" {
#endif
void clr( LPTR lpDst, int iCount );
#ifndef C_CODE
}
#endif
void lclr( LPTR ptr, long lCount );
void setifset( LPTR lpDst, int iCount, int iValue );
void set( LPTR lpDst, int iCount, int iValue );
void set16( LPWORD ptr, int count, WORD value );
void set24( LPRGB lpRGB, int iCount, long lValue );
void set32( LPCMYK lpRGB, int iCount, long lValue );
void hist( LPTR pixels, int count, long histo[256] );
void swapw( LPWORD word );
void swapl( LPDWORD dword );
void Unpack4( LPTR lpIn, int iCount, LPTR lpOut );
void ScaleRect( LPRECT lpRect, LFIXED Scale );
#ifndef USING_FIXED16
void SinCos( int iAngle, LPLFIXED lplSin, LPLFIXED lplCos );
#else
void SinCos( LFIXED iAngle, LPLFIXED lplSin, LPLFIXED lplCos );
#endif
//void SinCos( LFIXED iAngle, LPLFIXED lplSin, LPLFIXED lplCos );
WORD lsqrt( long lValue );
void la2con( LPTR lpIn, int iCount, LPTR lpOut, int normal );
void con2la( LPTR lpIn, int iCount, LPTR lpOut );

/*=========================================================================*\
	palette.c
\*=========================================================================*/
LPTR CreatePaletteLut15(LPRGB lpRGBmap, int nCubes, LPTR lpLut15, STATUSCALLBACKPROC lpStatusCallback);
void CreateErrorLut15(LPRGB lpRGBmap, LPTR lpTable, LPTR lpErrorTable, BOOL bDither);

/*=========================================================================*\
	strings.c
\*=========================================================================*/

BOOL  StringsEqual( LPSTR lpString1, LPSTR lpString2 );
BOOL  StringsEqualN( LPSTR lpString1, LPSTR lpString2, int length );
LPSTR Lowercase( LPSTR lp );
LPSTR Uppercase( LPSTR lp );
LPSTR SkipSpaces( LPSTR lpString );
LPSTR RemoveWhiteSpace( LPSTR lpString );
LPSTR StripTrailer( LPSTR lpString, char cChar );
LPSTR lstrfind( LPSTR lpString, LPSTR lpSearchString );
LPSTR lStrChr( LPSTR lpString, char cChar );
LPSTR lStrCpyN(LPSTR lpDst, LPSTR lpSrc, int length);
LPSTR RGBAscii( LPRGB lpRGB, LPSTR pAscii );
void  AsciiRGB( LPSTR pAscii, LPRGB lpRGB );
LPSTR Int2ToAscii( LPSTR lpAscii, int a, int b);
LPSTR Int4ToAscii( LPSTR lpAscii, int a, int b, int c, int d );
void  AsciiToInt2( LPSTR lpAscii, LPINT a, LPINT b);
void  AsciiToInt4( LPSTR lpAscii, LPINT a, LPINT b, LPINT c, LPINT d );
LPSTR Int2Ascii( LPSTR lpAscii, int i );
int   Ascii2Int( LPSTR lpAscii );
LONG  Ascii2Long( LPSTR lpAscii );
LPSTR DoubleAscii( double value, LPSTR lpAscii);
double AsciiDouble( LPSTR  lpAscii, LPINT lpbTranslated );
DWORD HexToBytes( LPSTR lpHex, LPTR lpBytes );
void BytesToHex( LPTR lpBytes, LPSTR lpHex, DWORD dwBytes );
LPSTR itoh( int i );
int   htoi( LPSTR s );
BOOL  MatchWildcard( LPSTR lpText, LPSTR lpPattern, WORD wFlags );

/*========================================*\
	C++ Garbage...
\*========================================*/

// Imported Data
extern BYTE ToMaxi[32];
extern BYTE ShiftUp2Lut[256];

/*=========================================================================*\
	Frame access macros
\*=========================================================================*/

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

#endif /* __framelib_h__ */






