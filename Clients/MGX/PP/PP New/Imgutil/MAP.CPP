/*=======================================================================*\

	MAP.C - Pixel Mapping routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifndef WIN32
#include <windows.h>
#endif

#include "imgutil.h"

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
	unsigned uCount)
{
#ifdef C_CODE
while (uCount--)
	{
	*lpSrc++ = lpGLut[*lpSrc];
	}
#else
__asm	{
	push	ds
	push	si
	push	di
	les	di, lpSrc
	lds bx, lpGLut
	mov	cx, uCount
	cld
Map8_1:
	mov		al,es:[di]
	xlatb				; mov al,ds:[bx+al]
	stosb
	loop	Map8_1
	pop		di
	pop		si
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
	unsigned uCount)
{
#ifdef C_CODE
while (uCount--)
	{
	lpSrc->red = lpRLut[lpSrc->red];
	lpSrc->green = lpGLut[lpSrc->green];
	lpSrc->blue = lpBLut[lpSrc->blue];
	++lpSrc;
	}
#else
__asm	{
	;;
	;; Notice:  I am assuming the LUT's are all in the same data segment
	;; 	    for speed...Watch out!!!
	;;
	push	ds
	push	si
	push	di
	les		di, lpSrc
	mov		ds, WORD PTR lpRLut+2
	mov		cx, uCount
	cld
Map24_1:
	;; Process Red channel
	mov		al, es:[di]
	mov		bx, WORD PTR lpRLut
	xlatb
	stosb	
	;; Process Green channel
	mov		al, es:[di]
	mov		bx, WORD PTR lpGLut
	xlatb
	stosb	
	;; Process Blue channel
	mov		al, es:[di]
	mov		bx, WORD PTR lpBLut
	xlatb
	stosb	
	loop	Map24_1
	pop		di
	pop		si
	pop		ds
	}
#endif
}

/*=======================================================================*/


