/*=======================================================================*\

	IMGUTIL.H - Frame Library include header file.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifndef __imgutil_h__
#define __imgutil_h__
#include "types.h"
#include "mgxframe.h"
#include <sys/types.h>
#include <sys/stat.h>		  
#include <dos.h>

extern BYTE ShiftUp2Lut[256];

#define STRING_DELIM	_T('\"')

#define STATICTABLE static const
#define TABLE const

#define BIT1		1L
#define BIT2		2L
#define BIT3		4L
#define BIT6		32L
#define BIT18		131072L
#define BITMASK		( BIT1| BIT2| BIT3| BIT6 )
#define TOGRAY(n)	((int)( ((255L * (n)) +  50) / 100 ))

#define MW_ESCAPES      0x0001          /* MatchWildcard options            */
#define MW_CASE         0x0002

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

#ifndef WIN32
typedef float FLOAT;
typedef FLOAT FAR* LPFLOAT;
#else
typedef float FLOAT;
typedef FLOAT FAR* LPFLOAT;
#endif

typedef struct _floatpt
{
	FLOAT x;
	FLOAT y;	
} FLOATPT, FAR *LPFLOATPT;

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

// Values for reverse flag
#define NO_REVERSE 			0
#define REVERSE_POINTSXY	1  	// for CMYK
#define REVERSE_LUTXY		2	// for Printer Calibration

// Valuse for iCurves
// NOTE: 0 for no interpolation
#define SMOOTH_EXSOFT 	60
#define SMOOTH_SOFT		80
#define SMOOTH_NORMAL	100
#define SMOOTH_HARD		120
#define SMOOTH_EXHARD	140

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

#define MAX_VOLID_LEN	13
typedef char VOLID[MAX_VOLID_LEN];

#ifdef WIN32
typedef LPFILETIME LPFILETIMES;
typedef FILETIME FILETIMES;
#else
typedef struct _filetimes
{
unsigned	time;
unsigned	date;
} FILETIMES, FAR *LPFILETIMES;
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

/************************************************************************/
/* Macro Subroutines                                                    */
/************************************************************************/

// KeyState macros
#define CONTROL ( GetAsyncKeyState( VK_CONTROL )<0 )
#define ALT ( GetAsyncKeyState( VK_MENU )<0 )
#define SHIFT ( GetAsyncKeyState( VK_SHIFT )<0 )
#define ESCAPE ( GetAsyncKeyState( VK_ESCAPE )<0 )
#define CANCEL ( GetAsyncKeyState( VK_ESCAPE )<0 )
#define BACKSPACE ( GetAsyncKeyState( VK_BACK )<0 )
#define SPACE ( GetAsyncKeyState( VK_SPACE )<0 )
#define CAPSLOCK ( GetKeyState( VK_CAPITAL )&1 )
#define NUMLOCK ( GetKeyState( VK_NUMLOCK )&1 )
#define LBUTTON ( GetAsyncKeyState( GetSystemMetrics( SM_SWAPBUTTON ) ? VK_RBUTTON : VK_LBUTTON ) < 0 )
//#define LBUTTON ( GetAsyncKeyState( VK_LBUTTON)<0 )
#define MBUTTON ( GetAsyncKeyState( VK_MBUTTON)<0 )
#define RBUTTON ( GetAsyncKeyState( GetSystemMetrics( SM_SWAPBUTTON ) ? VK_LBUTTON : VK_RBUTTON ) < 0 )
//#define RBUTTON ( GetAsyncKeyState( VK_RBUTTON)<0 )
#define OPTION1 (SHIFT && !CONTROL)
#define OPTION2 (CONTROL && !SHIFT)
#define OPTION3 (SHIFT && CONTROL)
#define CONSTRAINXY 	OPTION2
#define CONSTRAINASPECT CONTROL
#define FROMCENTER 		SHIFT

#define RectWidth( lpRect )  ( (lpRect)->right - (lpRect)->left + 1 )
#define RectHeight( lpRect ) ( (lpRect)->bottom - (lpRect)->top + 1 )


#define LOWERCASE(c) \
	(( (c) >= _T('A') && (c) <= _T('Z') ) ? ((c)-(_T('A')-_T('a'))): (c))
#define UPPERCASE(c) \
	(( (c) >= _T('a') && (c) <= _T('z') ) ? ((c)+(_T('A')-_T('a'))): (c))

#define AVG(a,b) (((a)+(b))>>1)
#define sign( num ) \
	( (num) == 0 ? 0 : ( (num) < 0 ? -1 : 1 ) )
#define ABS( num ) \
	( (num) < 0  ? -(num) : (num) )
#define Min(a,b) ((a)<(b) ?  (a) : (b))
#define Max(a,b) ((a)>(b) ?  (a) : (b))
#define Bound(x,a,b) max(min(b,x),a)
#define mbound Bound
#define bound Bound
#define WITHIN(val,lower,upper) \
	( (val) < (lower) ? 0 : ( (val) > (upper) ? 0 : 1 ) )
	
#define STRICTLY_WITHIN(val,lower,upper) \
	( (val) <= (lower) ? 0 : ( (val) >= (upper) ? 0 : 1 ) )
	
// check for identity transform
#define ISNULLTFORM(lpTForm) \
	(!lpTForm->bx && !lpTForm->cx  && !lpTForm->ay  && !lpTForm->cy && lpTForm->ax == FUNITY && lpTForm->by == FUNITY)

#define MAKEWORD(a,b) ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define PointIsNull(point) (!(point)->x && !(point)->y)

#define PlaceWindow( hWnd, x, y ) \
	SetWindowPos( hWnd, NULL, (x), (y), 0, 0, \
		SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE )
#define SizeWindow( hWnd, dx, dy ) \
	SetWindowPos( hWnd, NULL, 0, 0, (dx), (dy), \
		SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE )
#define PlaceAndSizeWindow( hWnd, x, y, dx, dy ) \
	SetWindowPos( hWnd, NULL, (x), (y), (dx), (dy), \
		SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE )

typedef int ( FAR *ENUMPROC )( LPTSTR, LPTSTR, long, int );

/*=======================================================================*\
	Source file: clrinfo.cpp
\*=======================================================================*/
void        SetColorInfo( LPCOLORINFO lpSrcColor, LPCOLORINFO lpDstColor, COLORSPACE ColorSpace);
LPCOLORINFO GetColorFromTypeInfo(FRMTYPEINFO TypeInfo, LPCOLORINFO lpColor);
COLOR 		GetFrameColor( LPCOLORINFO lpColor, FRMDATATYPE DataType);
COLOR 		GetFrameColor( LPCOLORINFO lpColor, FRMTYPEINFO TypeInfo);
BOOL        ColorsEqual( LPCOLORINFO lpSrcColor, LPCOLORINFO lpDstColor );
void		SetColorInfoString( LPTSTR lpString, LPCOLORINFO lpColorInfo, COLORSPACE ColorSpace );
COLORSPACE 	GetColorInfoString( LPTSTR lpString, LPCOLORINFO lpColorInfo );

/*=======================================================================*\
	Source file: gamma.cpp
\*=======================================================================*/

void BuildGammaLut(LFIXED Gamma, LPTR GammaLut);
void CreateGammaMap(LPMAP lpMap, LFIXED fGamma );

/*=======================================================================*\
	Source file: makemap.cpp
\*=======================================================================*/
BOOL SetupHMSMaps( HINSTANCE hInst, int baseid );
void FreeupHMSMaps();
void MakeMap( LPMAP lpMap );
void MakeFloatMap( LPMAP lpMap, LPFLOAT lpFloatLut );
void MapLookup(LPFLOAT lpLut, LPFLOATPT lpTable);
void MapCombine(LPFLOAT lpLut1, LPFLOAT lpLut2);
void MapReflectXY(LPFLOAT lpPoints);
void MapCreateLut(LPFLOAT lpPoints, LPTR lpLut);
void MapCreateFloatMap(LPTR lpLut, LPFLOAT lpPoints);
void ReverseMap(LPMAP, BOOL fMakeMap);
void InvertMap( LPMAP lpMap );
void ResetMap( LPMAP lpMap, int maxpoints, int type );
void ResetMapEx( LPMAP lpMap, int maxpoints, int type, BOOL bReverseHMS );
double Splint(LPPOINT lpC, double *lpSp, int num, int x, double Val);
void Spline(LPPOINT lpC, int num, double d1, double d2, double *lpOut, double Val);

/*=========================================================================*\
	map.cpp
\*=========================================================================*/

void map( LPTR lut, LPTR buf, int count );
void MapRGB8(  LPTR lpRLut, LPTR lpGLut, LPTR lpBLut, LPTR   lpSrc, unsigned uCount);
void MapRGB24( LPTR lpRLut, LPTR lpGLut, LPTR lpBLut, LPRGB  lpSrc, unsigned uCount);
void MapRGB32( LPTR lpRLut, LPTR lpGLut, LPTR lpBLut, LPCMYK lpSrc, unsigned uCount);

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
#ifdef WIN32
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
	fileio.cpp
\*=========================================================================*/
#ifndef WIN32
#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2
#endif

FILE_HANDLE FileOpen(LPTSTR lpFileName, DWORD dwMode);
BOOL FileClose(FILE_HANDLE handle);
DWORD FileLastError(void);
DWORD FileRead(FILE_HANDLE handle, LPVOID lpBuf, DWORD dwBytes);
DWORD FileWrite(FILE_HANDLE handle, LPVOID lpBuf, DWORD dwBytes);
BOOL FileTruncate(FILE_HANDLE handle);
DWORD FileSeek(FILE_HANDLE handle, long lAmount, int dir);
LPTSTR GetRootDirectory(LPTSTR lpPath, LPTSTR lpRoot);
void FixFileName( LPTSTR lpPathName );
LPTSTR stripdir( LPTSTR lpPathName );
LPTSTR filename( LPTSTR lpPathName );
LPTSTR stripfile( LPTSTR lpPath );
LPTSTR stripext( LPTSTR lpPath );
LPTSTR FixPath( LPTSTR lpPath );
LPTSTR extension( LPTSTR lpPath );
LPTSTR GetCurrentDir( LPTSTR lpBuffer, int maxlen );
BOOL SetCurrentDir( LPTSTR lpCurrentDir );
BOOL DiskSpaceEx( LPTSTR lpPath, LPDWORD lpdwTotal, LPDWORD lpdwFree );
DWORD DiskSpace( LPTSTR lpPath );
UINT GetExtendedDriveType(int i);
BOOL FileCopy( LPTSTR lpInFile, LPTSTR lpOutFile );
#ifndef WIN32
BOOL CopyFile( LPTSTR lpInFile, LPTSTR lpOutFile, BOOL fFailExists );
#endif
BOOL SafeFileExists( LPTSTR szFileName );
int SafeStat(LPTSTR lpFile, struct _stat FAR *buffer);
BOOL FileExists( LPTSTR szFileName );
BOOL FileAccess( LPTSTR szFileName, WORD wStyle );
BOOL FilePermission( LPTSTR lpFileName, int mode);
BOOL FileDate( LPTSTR lpFileName, LPFILETIMES lpTime );
LPTSTR FileTimeToString( LPFILETIMES lpFileTime, LPTSTR lpString );
long FileSize( LPTSTR lpFileName );
BOOL FileDelete( LPTSTR lpFileName );
void WildcardDelete( LPTSTR lpPath, LPTSTR lpName );
BOOL GetTempFilename( LPTSTR lpTempName, LPTSTR lpExtension );
LPTSTR GetVolumnID( LPTSTR lpPath, LPTSTR lpVolumn );
BOOL GetVolumeSerialNumber( LPTSTR lpPath, LPDWORD lpSerialNumber );
int EnumFiles(LPTSTR lpSrcName, LPTSTR lpDstName, ENUMPROC lpEnumProc, long lData);
LPTSTR MakeFullPath(LPTSTR lpPath);
BOOL Dos_FindFirst(LPTSTR match, DWORD dwAttrib, LPFINDDATA lpFind);
BOOL Dos_FindNext(LPFINDDATA lpFind);
BOOL Dos_FindClose(LPFINDDATA lpFind);
BOOL Dos_GetFileAttr(LPTSTR lpFile, LPDWORD lpAttrib);
int RenameFile(LPTSTR lpSrc, LPTSTR lpDst);
LPTSTR MkTemp(LPTSTR lpTemplate);
int MkDir(LPTSTR lpDir);
int RmDir(LPTSTR lpDir);
int ChDir(LPTSTR lpDir);
char *GetCwd(char *pDir, int len);
int GetDrive();
BOOL SetDrive(int drive);
BOOL SafeFileStats(LPTSTR lpFile, LPUINT32 lpSize, LPUINT32 lpCTime, LPUINT32 lpMTime, LPUINT32 lpATime);
#ifndef WIN32
DWORD GetLastError(void);
#endif
BOOL IsAbsolutePath(LPCTSTR lpPath);

/*=========================================================================*\
	memory.cpp
\*=========================================================================*/

LPTR Alloc( long lCount );
LPTR AllocX( long lCount, WORD wFlags );
LPTR AllocExtend( LPTR lpMemory, long lCount );
void FreeUp( LPVOID lpMemory );
DWORD GlobalPtrSize( LPVOID lpMemory );
DWORD AvailableMemory( void );
BOOL AllocLines( LPPTR lpList, int nLines, int iWidth, int iDepth );
LPTR GetBuffers16Kx4( LPPTR lppBuffers, BOOL bDosMemory );
BOOL MemCompare(LPTR lpBuf1, LPTR lpBuf2, DWORD dwBytes);

/*=========================================================================*\
	tform.cpp
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
void Transformer( LPTFORM lpTForm, LPPOINT lpPoint, LPLONG lpx, LPLONG lpy );
void Transformer( LPTFORM lpTForm, LPPOINT lpPoint, LPINT lpx, LPINT lpy );
void TransformRect( LPTFORM lpTForm, LPRECT lpRect, LPRECT lpNewRect );

/*=========================================================================*\
	util.cpp
\*=========================================================================*/
#define CopyToHuge(lpSrc, hpDst, dwCount) copy((lpSrc), (hpDst), (int)(dwCount))
#define CopyFromHuge(hpSrc, lpDst, dwCount) copy((hpSrc), (lpDst), (int)(dwCount))
#define hcopy(hpSrc, hpDst, dwCount) copy((hpSrc), (hpDst), (int)(dwCount))
#define hclr(hpDst, dwCount) clr((hpDst), (int)(dwCount))
#define hset(hpDst, dwCount, iValue) set((hpDst), (int)(dwCount), (iValue))
#define hset24(hpDst, dwCount, lValue) set24((hpDst), (int)(dwCount), (lValue))
#define hset32(hpDst, dwCount, lValue) set32((hpDst), (int)(dwCount), (lValue))
#define hsetifset( hpDst, dwCount, iValue ) setifset((hpDst), (int)(dwCount), (iValue))
#define Mirror8Huge( pixels, count) mirror8((pixels), (count))
#define Mirror24Huge( pixels, count) mirror24((pixels), (count))
#define Mirror32Huge( pixels, count) mirror32((pixels), (count))
#define lmemcpy( lpDst, lpSrc, iCount ) copy((lpSrc), (lpDst), (iCount))
#define lclr( ptr, lCount ) clr((ptr), (lCount))

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
BOOL AstralPtInRect( LPRECT lpRect, POINT pt);
	  
void ResConvertUL(int iSrcRes, int iDstRes, LPLONG lpX, LPLONG lpY);
void ResConvertUL(int iSrcRes, int iDstRes, LPINT lpX, LPINT lpY);
void ResConvertLR(int iSrcRes, int iDstRes, LPLONG lpX, LPLONG lpY);
void ResConvertLR(int iSrcRes, int iDstRes, LPINT lpX, LPINT lpY);
void ResConvertCenter(int iSrcRes, int iDstRes, LPLONG lpX, LPLONG lpY);
void ResConvertCenter(int iSrcRes, int iDstRes, LPINT lpX, LPINT lpY);
void ResConvertWH(int iSrcRes, int iDstRes, LPINT lpW, LPINT lpH);
void ResConvertDist(int iSrcRes, int iDstRes, LPINT lpD);
void ResConvertRect(int iSrcRes, int iDstRes, LPRECT lpRect);

void mirror8( LPTR pixels, int count );
void mirror24( LPTR pixels, int count );
void mirror32( LPTR pixels, int count );
LFIXED ScaleToFit( LPINT DestWidth, LPINT DestHeight, int SrcWidth, int SrcHeight, 
	BOOL bUseSmallerFactor = TRUE);
void ramp( LPTR lut, int inx1, int inx2, int outx1, int outx2 );
void LineartSetBits   (LPTR lpData, int iBegin, int iCount);
void LineartClearBits (LPTR lpData, int iBegin, int iCount);
void LineartNegateBits(LPTR lpData, int iBegin, int iCount);
void copy( LPTR lpSrc, LPTR lpDst, int iCount );

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
void clr( LPTR lpDst, int iCount );
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
void SinCos( LFIXED iAngle, LPLFIXED lplSin, LPLFIXED lplCos );
WORD lsqrt( long lValue );
void la2con( LPTR lpIn, int iCount, LPTR lpOut, int normal );
void con2la( LPTR lpIn, int iCount, LPTR lpOut );
void ScaleData8( LPTR lpDst, int iCount, BYTE factor );
void ScaleDataBuf8( LPTR lpDst, int iCount, LPTR lpMsk );
void ScaleDataBuf8Huge( HPTR lpDst, DWORD dwCount, HPTR lpMsk );
int asciize( LPTR in, int count, LPTSTR out );

/*=========================================================================*\
	strings.cpp
\*=========================================================================*/

LPTSTR 	MyCharNext( LPTSTR pszStr ); 
LPTSTR 	MyCharPrev( LPTSTR pszStart, LPTSTR pszStr );
LPTSTR 	MyCharNextN( LPTSTR lpString, UINT uPos );
UINT 	MBStrlen( LPTSTR lpString );
BYTE 	MBToUpper( LPTSTR lpChar );
BYTE 	MBToLower( LPTSTR lpChar );
WORD 	MBToUpperW( LPTSTR lpChar );
WORD 	MBToLowerW( LPTSTR lpChar );
LPTSTR 	MBStrchr( LPTSTR lpString, WORD wChar );
LPTSTR 	MBStrrchr( LPTSTR lpString, WORD wChar );
UINT 	MBStrcpyN( LPTSTR lpBuffer, LPTSTR lpString, UINT uCount );
BOOL  	StringsEqual( LPTSTR lpString1, LPTSTR lpString2 );
BOOL  	StringsEqualN( LPTSTR lpString1, LPTSTR lpString2, int length );
LPTSTR 	Lowercase( LPTSTR lp );
LPTSTR 	Uppercase( LPTSTR lp );
LPTSTR 	SkipSpaces( LPTSTR lpString );
LPTSTR 	RemoveWhiteSpace( LPTSTR lpString );
LPTSTR 	StripTrailer( LPTSTR lpString, char cChar );
LPTSTR 	lstrfind( LPTSTR lpString, LPTSTR lpSearchString );
LPTSTR 	lStrChr( LPTSTR lpString, char cChar );
LPTSTR 	lStrCpyN(LPTSTR lpDst, LPTSTR lpSrc, int length);
LPTSTR 	RGBAscii( LPRGB lpRGB, LPTSTR pAscii );
void  	AsciiRGB( LPTSTR pAscii, LPRGB lpRGB );
LPTSTR 	Int2ToAscii( LPTSTR lpAscii, int a, int b);
LPTSTR 	Int4ToAscii( LPTSTR lpAscii, int a, int b, int c, int d );
void  	AsciiToInt2( LPTSTR lpAscii, LPINT a, LPINT b);
void  	AsciiToInt4( LPTSTR lpAscii, LPINT a, LPINT b, LPINT c, LPINT d );
LPTSTR 	Int2Ascii( LPTSTR lpAscii, int i );
int   	Ascii2Int( LPTSTR lpAscii );
LONG  	Ascii2Long( LPTSTR lpAscii );
ULONG  	Ascii2ULong( LPTSTR lpAscii );
LPTSTR 	DoubleAscii( double value, LPTSTR lpAscii);
double 	AsciiDouble( LPTSTR  lpAscii, LPINT lpbTranslated );
DWORD 	HexToBytes( LPTSTR lpHex, LPTR lpBytes );
void 	BytesToHex( LPTR lpBytes, LPTSTR lpHex, DWORD dwBytes );
LPTSTR 	itoh( int i, LPTSTR lpHex );
int   	htoi( LPTSTR s );
BOOL  	MatchWildcard( LPTSTR lpText, LPTSTR lpPattern, WORD wFlags );
LPTSTR	SkipParm(LPTSTR lpCommand, int nParam);
int		GetIntParm(LPTSTR lpCommand, int nParam, LPINT lpbTranslated = NULL);
long 	GetLongParm(LPTSTR lpCommand, int nParam, LPINT lpbTranslated = NULL);
long 	GetULongParm(LPTSTR lpCommand, int nParam, LPINT lpbTranslated = NULL);
LFIXED	GetFixedParm(LPTSTR lpCommand, int nParam, LPINT lpbTranslated = NULL);
LPTSTR	GetStringParm(LPTSTR lpCommand, int nParam, LPTSTR lpParam, LPINT lpbTranslated = NULL);

#endif // __imgutil_h__



