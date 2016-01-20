/***************************************************************************/
/* 																								*/
/*  HEADER:  AccuStd.h - AccuSoft public data types header.  These data 	*/
/* 							 types are used by all AccuSoft products. 			*/
/* 																								*/
/* 																								*/
/*  DATE CREATED:  01/19/96																*/
/* 																								*/
/* 	$Date:: 3/03/99 6:00p  $															*/
/* 	$Revision:: 1      $															*/
/* 																								*/
/* Copyright (c) 1996-97, AccuSoft Corporation.  All rights reserved.		*/
/* 																								*/
/***************************************************************************/

#ifndef __ACCUSTD_H__
#define __ACCUSTD_H__


/***************************************************************************/
/* Simple helper macro constants 														*/
/***************************************************************************/

#ifndef FALSE
#define FALSE			0
#endif

#ifndef TRUE
#define TRUE			1
#endif

#ifndef NULL
#define NULL			0
#endif

#ifndef EOL
#define EOL 			'\000'
#endif

#ifndef PI
#define PI				3.141592654
#endif

#ifndef TWOPI
#define TWOPI			(PI * 2.0)
#endif

#ifndef HALFPI
#define HALFPI 		(PI / 2.0)
#endif

#ifndef VOID
#define VOID			void
#endif

#ifndef AE_SUCCESS
#define AE_SUCCESS	0
#endif

#ifndef AE_FAILURE
#define AE_FAILURE	-1
#endif

#ifndef SB_HORZ
#define SB_HORZ	0
#endif

#ifndef SB_VERT
#define SB_VERT	1
#endif


/***************************************************************************/
/* biCompression field of AT_DIB 														*/
/***************************************************************************/

#define	IG_BI_RGB						0L 	/* Standard DIB								*/
#define	IG_BI_BITFIELDS				3L 	/* 16/32 bit DIB with bit masks			*/
#define	IG_BI_RLE						500L	/* AccuSoft DIB extension: RunEnds		*/
#define	IG_BI_CMYK						501L	/* AccuSoft DIB extension: CMYK			*/
#define	IG_BI_ABIC						502L	/* AccuSoft DIB extension: ABIC			*/
#define	IG_BI_GRAYSCALE				503L	/* AccuSoft DIB extension: Grayscale	*/
#define	IG_BI_PSEUDOCOLOR 			504L	/* AccuSoft DIB extension: Pseudocolor */


/***************************************************************************/
/* Special considerations for Watcom 32 bit Windows 3.x							*/
/***************************************************************************/

#ifdef __WINDOWS_386__

#define UINT				unsigned int
#define LPINT				int *
#define BOOL				int

/* #ifdef __WINDOWS_386__ */
#endif


/***************************************************************************/
/* Simple type definitions 																*/
/***************************************************************************/

#ifndef _INC_WINDOWS

typedef unsigned short	WORD;
typedef unsigned long	DWORD;

#ifndef _OS2

#ifndef XMD_H
typedef int 				BOOL;
typedef unsigned char	BYTE;
#endif
typedef unsigned int 	UINT;
typedef signed long		LONG;

/* #ifndef _OS2 */
#endif
/* #ifndef _INC_WINDOWS */
#endif

#ifndef _OS2

#ifndef _INC_VER
typedef short				SHORT;
#endif

typedef int 				INT;

#ifndef _WINNT_
typedef signed char		CHAR;
/* #ifndef _WINNT_ */
#endif

#else

#undef CHAR
#define CHAR signed char

/* #ifndef _OS2 */
#endif

typedef double 			DOUBLE;

/***************************************************************************/
/* Simple pointer definitions 															*/
/***************************************************************************/

#ifndef _INC_WINDOWS

typedef VOID  FAR 		*LPVOID;
typedef BYTE  FAR 		*LPBYTE;
typedef WORD  FAR 		*LPWORD;
typedef int   FAR 		*LPINT;
typedef long  FAR 		*LPLONG;
typedef DWORD FAR 		*LPDWORD;
typedef char  FAR 		*LPSTR;

/* #ifndef _INC_WINDOWS */
#endif

typedef VOID  FAR32		*LPVOID32;
typedef BOOL  FAR 		*LPBOOL;
typedef UINT  FAR 		*LPUINT;
typedef CHAR  FAR 		*LPCHAR;
typedef SHORT FAR 		*LPSHORT;
typedef DOUBLE FAR		*LPDOUBLE;


/***************************************************************************/
/* Common type definitions 																*/
/***************************************************************************/

typedef INT 				AT_ERRCODE; 	/* error status							*/
typedef INT 				AT_ERRCOUNT;	/* number of errors on stack			*/
typedef UINT				AT_MODE; 		/* used for mode options				*/
typedef DWORD				AT_LMODE;		/* used for mode options				*/
typedef BYTE				AT_PIXEL;		/* definition of an 8 bit pixel		*/
typedef LONG				AT_PIXPOS;		/* used for x or y in a coordinate	*/
typedef LONG				AT_DIMENSION;	/* the width or height of an image	*/
typedef float				AT_REAL; 		/* when maximum precision is needed */


/***************************************************************************/
/* Common pointer definitions 															*/
/***************************************************************************/

typedef AT_ERRCODE FAR		*LPAT_ERRCODE;
typedef AT_MODE FAR			*LPAT_MODE;
typedef AT_LMODE FAR 		*LPAT_LMODE;
typedef AT_PIXEL FAR 		*LPAT_PIXEL;
typedef AT_PIXPOS FAR		*LPAT_PIXPOS;
typedef AT_DIMENSION FAR	*LPAT_DIMENSION;
typedef AT_REAL FAR			*LPAT_REAL;


/***************************************************************************/
/* System types																				*/
/***************************************************************************/

#ifndef _INC_WINDOWS

#ifndef _OS2

#ifdef macintosh /* PJC */
typedef void * HDC;
typedef void * HWND;
typedef void * HBITMAP;
typedef void * HFONT;
typedef void * HPALETTE;
#else

#ifndef __WINDOWS_H
#define HDC 				UINT
#define HWND				UINT
#define HBITMAP			UINT
#define HFONT				UINT
#define HPALETTE			UINT
#endif

#endif

#else
#define HPALETTE			HPAL
#endif

/* #ifndef _INC_WINDOWS */
#endif


/***************************************************************************/
/* Higher level type definitions 														*/
/***************************************************************************/

typedef struct tagAT_POINT
		{
		AT_PIXPOS		x;
		AT_PIXPOS		y;
		}
	AT_POINT;
typedef AT_POINT FAR 	*LPAT_POINT;


typedef struct tagAT_RECT
		{
		AT_PIXPOS		left;
		AT_PIXPOS		top;
		AT_PIXPOS		right;
		AT_PIXPOS		bottom;
		}
	AT_RECT;
typedef AT_RECT FAR		*LPAT_RECT;


typedef struct tagAT_RGBQUAD
		{
		AT_PIXEL 		rgbBlue;
		AT_PIXEL 		rgbGreen;
		AT_PIXEL 		rgbRed;
		BYTE				rgbReserved;
		}
	AT_RGBQUAD;
typedef AT_RGBQUAD FAR	*LPAT_RGBQUAD;


typedef struct tagAT_DIB
		{
		DWORD 		biSize;
		LONG			biWidth;
		LONG			biHeight;
		WORD			biPlanes;
		WORD			biBitCount;
		DWORD 		biCompression;
		DWORD 		biSizeImage;
		LONG			biXPelsPerMeter;
		LONG			biYPelsPerMeter;
		DWORD 		biClrUsed;
		DWORD 		biClrImportant;
		}
	AT_DIB;
typedef AT_DIB FAR		*LPAT_DIB;

typedef struct
{
	LONG					xResNumerator;
	LONG					xResDenominator;
	LONG					yResNumerator;
	LONG					yResDenominator;
	AT_LMODE 			nUnits;
}
AT_RESOLUTION, FAR *LPAT_RESOLUTION;


typedef struct tagAT_RGB
		{
		AT_PIXEL 		b;
		AT_PIXEL 		g;
		AT_PIXEL 		r;
		}
	AT_RGB;
typedef AT_RGB FAR		*LPAT_RGB;

typedef struct tagAT_IHS
		{
		AT_PIXEL 		i;
		AT_PIXEL 		h;
		AT_PIXEL 		s;
		}
	AT_IHS;
typedef AT_IHS FAR		*LPAT_IHS;


typedef struct tagAT_SIZE
		{
		AT_DIMENSION	width;
		AT_DIMENSION	height;
		}
	AT_SIZE;
typedef AT_SIZE FAR		 *LPAT_SIZE;


typedef struct tagAT_YUV
		{
		AT_PIXEL 		y;
		AT_PIXEL 		u;
		AT_PIXEL 		v;
		}
	AT_YUV;
typedef AT_YUV FAR		*LPAT_YUV;

typedef struct tagAT_YCrCb
		{
		AT_PIXEL 		y;
		AT_PIXEL 		cr;
		AT_PIXEL 		cb;
		}
	AT_YCrCb;
typedef AT_YCrCb FAR 	  *LPAT_YCrCb;

typedef struct tagAT_YIQ
		{
		AT_PIXEL 		y;
		AT_PIXEL 		i;
		AT_PIXEL 		q;
		}
	AT_YIQ;
typedef AT_YIQ FAR		*LPAT_YIQ;

typedef struct tagAT_CMY
		{
		AT_PIXEL 		c;
		AT_PIXEL 		m;
		AT_PIXEL 		y;
		}
	AT_CMY;
typedef AT_CMY FAR		*LPAT_CMY;

typedef struct tagAT_CMYK
		{
		AT_PIXEL 		c;
		AT_PIXEL 		m;
		AT_PIXEL 		y;
		AT_PIXEL 		k;
		}
	AT_CMYK;
typedef AT_CMYK FAR		 *LPAT_CMYK;

typedef struct tagAT_HLS
		{
		AT_PIXEL 		h; /* 0 to 255 -> (0.0 to 360.0)*/
		AT_PIXEL 		l; /* 0 to 255 -> (0.0 to	 1.0)*/
		AT_PIXEL 		s; /* 0 to 255 -> (0.0 to	 1.0)*/
		}
	AT_HLS;
typedef AT_HLS FAR		*LPAT_HLS;

typedef struct tagAT_Lab
		{
		AT_PIXEL 		l;
		AT_PIXEL 		a;
		AT_PIXEL 		b;
		}
	AT_Lab;
typedef AT_Lab FAR		*LPAT_Lab;


/***************************************************************************/
/* Logical Font																				*/
/***************************************************************************/

#ifndef _INC_WINDOWS
#define LF_FACESIZE			 32
/* #ifndef _INC_WINDOWS */
#endif

typedef struct tagAT_LOGFONT
{
	 LONG 				lfHeight;
	 LONG 				lfWidth;
	 LONG 				lfEscapement;
	 LONG 				lfOrientation;
	 LONG 				lfWeight;
	 BYTE 				lfItalic;
	 BYTE 				lfUnderline;
	 BYTE 				lfStrikeOut;
	 BYTE 				lfCharSet;
	 BYTE 				lfOutPrecision;
	 BYTE 				lfClipPrecision;
	 BYTE 				lfQuality;
	 BYTE 				lfPitchAndFamily;
	 CHAR 				lfFaceName[LF_FACESIZE];
} AT_LOGFONT, FAR *LPAT_LOGFONT;


typedef struct tagAT_STITCH
{

	LONG		uRefTile;  /* Referenced tile          */
	LONG		uTileRows; /* Number of tiles in a row */
	LONG		uTileCols; /* Number of tiles in a col */

} AT_STITCH, FAR *LPAT_STITCH;


/*#ifndef __ACCUSTD_H__*/
#endif
