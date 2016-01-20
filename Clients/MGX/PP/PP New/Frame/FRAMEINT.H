/*=======================================================================*\

	FRAMEINT.H - Frame Library internal include header file.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifndef __frameint_h__
#define __frameint_h__

#define STATICTABLE static const
#define TABLE const

#define YES			TRUE
#define NO			FALSE
#define REG			register
#define LOCAL		static

#define copy(lpSrc, lpDst, iCount) CopyMemory((lpDst), (lpSrc), (iCount))
#define Bound(x,a,b) max(min(b,x),a)
#define mbound Bound
#define bound Bound
#define RectWidth( lpRect )  ( (lpRect)->right - (lpRect)->left + 1 )
#define RectHeight( lpRect ) ( (lpRect)->bottom - (lpRect)->top + 1 )
#define Min(a,b) ((a)<(b) ?  (a) : (b))
#define Max(a,b) ((a)>(b) ?  (a) : (b))
#ifdef WIN32
#define SPAN64K(lp,x) (FALSE)
#else
#define SPAN64K(lp,x) (((long)(((long)lp) & 0xFFFFL) + ((long)x)) >= 65536L)
#endif

typedef unsigned char HUGE *HPTR;

typedef int ( FAR *SAMPLETBLPROC )( LPTR, int, LPTR, int, int, LFIXED );
typedef int ( FAR *SAMPLEMWTBLPROC )( LPTR, int, LPTR, int, int, LFIXED, LPTR );
typedef int ( FAR *RGB2PIXELTBLPROC )( struct _frame far *, struct _rgb far * );
typedef int ( FAR *PIXEL2RGBTBLPROC )( struct _frame far *, LPLONG, struct _rgb far * );
typedef int ( FAR *SETRGBTBLPROC )( struct _frame far *, struct _rgb far *, LPTR, int );
typedef int ( FAR *GETPIXELTBLPROC )( struct _frame far *, LPTR, LPLONG, int );
typedef int ( FAR *PUTPIXELTBLPROC )( struct _frame far *, LPLONG, LPTR, int );
typedef int ( FAR *SETPIXELTBLPROC )( long, LPTR, int );


/*=========================================================================*\
	frmmini.c
\*=========================================================================*/


/*=========================================================================*\
	frmutils.c
\*=========================================================================*/

LPTR Alloc( long lCount );
LPTR AllocX( long lCount, WORD wFlags );
void FreeUp( LPVOID lpMemory );
DWORD AvailableMemory( void );
void clr( LPTR lpDst, int iCount );
void set( LPTR lpDst, int iCount, int iValue );
void swapBGR( LPTR lpSrc, LPTR lpDst, int iCount );
void negate( LPTR buf, long lCount );
LPSTR Int2Ascii( LPSTR lpAscii, int i );
int   Ascii2Int( LPSTR lpAscii );

#endif // __frameint_h__
