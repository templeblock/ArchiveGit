/*=======================================================================*\

	FRMMAP.C - Pixel Mapping routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include <windows.h>
#include "framelib.h"

BYTE ShiftUp2Lut[256] = {
  0,  4,  9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77,
 81, 85, 89, 94, 98,102,106,110,114,118,122,126,130,134,138,142,146,150,154,158,
162,166,170,174,179,183,187,191,195,199,203,207,211,215,219,223,227,231,235,239,
243,247,251,255,
  0,  4,  9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77,
 81, 85, 89, 94, 98,102,106,110,114,118,122,126,130,134,138,142,146,150,154,158,
162,166,170,174,179,183,187,191,195,199,203,207,211,215,219,223,227,231,235,239,
243,247,251,255,
  0,  4,  9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77,
 81, 85, 89, 94, 98,102,106,110,114,118,122,126,130,134,138,142,146,150,154,158,
162,166,170,174,179,183,187,191,195,199,203,207,211,215,219,223,227,231,235,239,
243,247,251,255,
  0,  4,  9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77,
 81, 85, 89, 94, 98,102,106,110,114,118,122,126,130,134,138,142,146,150,154,158,
162,166,170,174,179,183,187,191,195,199,203,207,211,215,219,223,227,231,235,239,
243,247,251,255,
};

/*=======================================================================*/

void map( LPTR lut, LPTR buf, int count )
{
	while ( --count >= 0 )
	{
		*buf = lut[*buf];
		buf++;
	}
}

/*=======================================================================*/

void MapRGB8( 
	LPTR lpRLut,
	LPTR lpGLut,
	LPTR lpBLut,
	LPTR lpSrc,
	LPTR lpDst,
	int  iCount)
{
#ifdef C_CODE
while (--iCount >= 0)
	{
	*lpDst++ = lpGLut[*lpSrc++];
	}
#else
__asm	{
	push	ds
	mov	si, WORD PTR lpSrc	; offset
	les	di, lpDst
	mov	cx, iCount
	mov	bx, WORD PTR lpGLut
	mov	dx, WORD PTR lpGLut+2
	cld
Map8_1:
	mov	ds, WORD PTR lpSrc+2	; segment
	lodsb				; get pixel
	mov	ds,dx			; get lpGLut segment
	xlatb				; mov al,ds:[bx+al]
	stosb
	loop	Map8_1
	pop		ds
	}
#endif
}

/*=======================================================================*/

void MapRGB24(
	LPTR  lpRLut,
	LPTR  lpGLut,
	LPTR  lpBLut,
	LPRGB lpSrc,
	LPRGB lpDst,
	int   iCount)
{
#ifdef C_CODE
while (--iCount >= 0)
	{
	lpDst->red = lpRLut[lpSrc->red];
	lpDst->green = lpGLut[lpSrc->green];
	lpDst->blue = lpBLut[lpSrc->blue];
	++lpSrc;
	++lpDst;
	}
#else
__asm	{
	;;
	;; Notice:  I am assuming the LUT's are all in the same data segment
	;; 	    for speed...Watch out!!!
	;;
	push	ds
	les	di, lpDst
	mov	si, WORD PTR lpSrc	; offset
	mov	cx, iCount
	cld
Map24_1:
	mov	ds, WORD PTR lpSrc+2	; segment
	lodsb				; AL = get red
	mov	ah, BYTE PTR ds:[si]	; AH = get green
	inc	si
	mov	dl, BYTE PTR ds:[si]	; DL = get blue
	inc	si
	lds	bx, lpRLut
	xlatb				; mov al,ds:[bx+al]
	stosb
	mov	al, ah			; get green
	mov	bx, WORD PTR lpGLut	; offset
	xlatb				; mov al,ds:[bx+al]
	stosb
	mov	al, dl			; get blue
	mov	bx, WORD PTR lpBLut	; offset
	xlatb				; mov al,ds:[bx+al]
	stosb
	loop	Map24_1
	pop		ds
	}
#endif
}

/*=======================================================================*/

void MapCMYK32(
	LPTR   lpCLut,
	LPTR   lpMLut,
	LPTR   lpYLut,
	LPTR   lpKLut,
	LPCMYK lpSrc,
	LPCMYK lpDst,
	int    iCount)
{
	while (--iCount >= 0)
	{
		lpDst->c = lpCLut[(lpSrc->c)];
		lpDst->m = lpMLut[(lpSrc->m)];
		lpDst->y = lpYLut[(lpSrc->y)];
		lpDst->k = lpKLut[(lpSrc->k)];
		++lpSrc;
		++lpDst;
	}
}

/*=======================================================================*/

void MapRGB32(
	LPTR   lpRLut,
	LPTR   lpGLut,
	LPTR   lpBLut,
	LPCMYK lpSrc,
	LPCMYK lpDst,
	int    iCount)
{
	while (--iCount >= 0)
	{
		lpDst->c = lpRLut[(lpSrc->c)^0xFF]^0xFF;
		lpDst->m = lpGLut[(lpSrc->m)^0xFF]^0xFF;
		lpDst->y = lpBLut[(lpSrc->y)^0xFF]^0xFF;
		lpDst->k = lpGLut[(lpSrc->k)^0xFF]^0xFF;
		++lpSrc;
		++lpDst;
	}
}

/*=======================================================================*/

