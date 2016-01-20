/*=======================================================================*\

	UTIL.C - Basic Utlilty Routines.

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

#include <ctype.h>
#include <direct.h>
#include <limits.h>
#include <math.h>
#include "imgutil.h"

/***********************************************************************/
LONG Bits(unsigned long val)
/***********************************************************************/
{
	long bits;
	
	bits = 0;
	
	while (val)
	{
		bits++;
		val >>= 1L;
	}
	return (bits);
}

/***********************************************************************/
LPRECT AstralToWindowsRect( LPRECT lpDest )
/***********************************************************************/
{
if (AstralIsRectEmpty(lpDest))
	SetRectEmpty(lpDest);
else
	{
	lpDest->right++;
	lpDest->bottom++;
	}
return(lpDest);
}

/***********************************************************************/
LPRECT WindowsToAstralRect( LPRECT lpDest )
/***********************************************************************/
{
if (IsRectEmpty(lpDest))
	AstralSetRectEmpty(lpDest);
else
	{
	lpDest->right--;
	lpDest->bottom--;
	}
return(lpDest);
}

/***********************************************************************/
void AstralSetRectEmpty( LPRECT lpDestRect )
/***********************************************************************/
{
SetRect(lpDestRect, INT_MAX, INT_MAX, INT_MIN, INT_MIN);
}

/***********************************************************************/
BOOL AstralIsRectEmpty( LPRECT lpRect )
/***********************************************************************/
{
return(lpRect->left > lpRect->right || lpRect->top > lpRect->bottom);
}


/***********************************************************************/
BOOL AstralPtInRect( LPRECT lpRect, POINT pt)
/***********************************************************************/
{
	if (!WITHIN(pt.x, lpRect->left, lpRect->right))
		return(FALSE);
	if (!WITHIN(pt.y, lpRect->top, lpRect->bottom))
		return(FALSE);
	return(TRUE);
}

/***********************************************************************/
void AstralUnionRect( LPRECT lpDestRect, LPRECT lpSrc1Rect, LPRECT lpSrc2Rect )
/***********************************************************************/
{
lpDestRect->top = min(lpSrc1Rect->top, lpSrc2Rect->top);
lpDestRect->left = min(lpSrc1Rect->left, lpSrc2Rect->left);
lpDestRect->bottom = max(lpSrc1Rect->bottom, lpSrc2Rect->bottom);
lpDestRect->right = max(lpSrc1Rect->right, lpSrc2Rect->right);
}

/************************************************************************/
BOOL AstralIntersectRect( LPRECT lpDest, LPRECT lpSrc1, LPRECT lpSrc2 )
/************************************************************************/
{
if (lpSrc1->left > lpSrc2->right ||
    lpSrc1->right < lpSrc2->left ||
    lpSrc1->top > lpSrc2->bottom ||
    lpSrc1->bottom < lpSrc2->top)
	{
	return(FALSE);
	}
lpDest->left = max(lpSrc1->left, lpSrc2->left);
lpDest->right = min(lpSrc1->right, lpSrc2->right);
lpDest->top = max(lpSrc1->top, lpSrc2->top);
lpDest->bottom = min(lpSrc1->bottom, lpSrc2->bottom);
return(TRUE);
}

/************************************************************************/
BOOL AstralInsideRect( LPRECT lpDst, LPRECT lpSrc )
/************************************************************************/
{
return(lpSrc->left >= lpDst->left &&
	lpSrc->right <= lpDst->right &&
	lpSrc->top >= lpDst->top &&
	lpSrc->bottom <= lpDst->bottom);
}

/************************************************************************/
void AstralSubtractRect( LPRECT lpDest, LPRECT lpSrc )
/************************************************************************/
{
RECT rSect;

if (lpDest->right < lpDest->left)
	return;

if (AstralIntersectRect(&rSect, lpDest, lpSrc))
	{
// first see if we can reduce height
	if (rSect.left == lpDest->left && rSect.right == lpDest->right)
		{
		if (rSect.top == lpDest->top && rSect.bottom == lpDest->bottom)
			{
			lpDest->top = lpDest->left = 0;
			lpDest->bottom = lpDest->right = -1;
			}
		else if (rSect.top == lpDest->top)
			{
			lpDest->top = rSect.bottom+1;
			}
		else if (rSect.bottom == lpDest->bottom)
			{
			lpDest->bottom = rSect.top-1;
			}
		}
	else if (rSect.top == lpDest->top && rSect.bottom == lpDest->bottom)
		{
		if (rSect.left == lpDest->left && rSect.right == lpDest->right)
			{
			lpDest->top = lpDest->left = 0;
			lpDest->bottom = lpDest->right = -1;
			}
		else if (rSect.left == lpDest->left)
			{
			lpDest->left = rSect.right+1;
			}
		else if (rSect.right == lpDest->right)
			{
			lpDest->right = rSect.left-1;
			}
		}
	}
}

/***********************************************************************/
void BoundRect( LPRECT lpRect, int left, int top, int right, int bottom )
/***********************************************************************/
{
// Check the left
if ( lpRect->left < left )      lpRect->left = left;    else
if ( lpRect->left > right )     lpRect->left = right;

// Check the right
if ( lpRect->right < left )     lpRect->right = left;   else
if ( lpRect->right > right )    lpRect->right = right;

// Check the top
if ( lpRect->top < top )        lpRect->top = top;      else
if ( lpRect->top > bottom )     lpRect->top = bottom;

// Check the bottom
if ( lpRect->bottom < top )     lpRect->bottom = top;   else
if ( lpRect->bottom > bottom )  lpRect->bottom = bottom;
}


/***********************************************************************/
// coverts upper left rect coords, uses rounding 
/***********************************************************************/
void ResConvertUL(int iSrcRes, int iDstRes, LPLONG lpX, LPLONG lpY)
/***********************************************************************/
{
ResConvertUL( iSrcRes, iDstRes, ( LPINT )lpX, ( LPINT )lpY );
}

/***********************************************************************/
// coverts upper left rect coords, uses rounding 
/***********************************************************************/
void ResConvertUL(int iSrcRes, int iDstRes, LPINT lpX, LPINT lpY)
/***********************************************************************/
{
	if (iSrcRes == iDstRes || !iSrcRes)
		return;
	
	*lpX = (int)(((double)iDstRes**lpX)/iSrcRes + .4999999);
	*lpY = (int)(((double)iDstRes**lpY)/iSrcRes + .4999999);
}

/***********************************************************************/
// coverts upper lower right rect coords, uses rounding 
/***********************************************************************/
void ResConvertLR(int iSrcRes, int iDstRes, LPLONG lpX, LPLONG lpY)
/***********************************************************************/
{
ResConvertLR( iSrcRes, iDstRes, ( LPINT )lpX, ( LPINT )lpY );
}

/***********************************************************************/
// coverts upper lower right rect coords, uses rounding 
/***********************************************************************/
void ResConvertLR(int iSrcRes, int iDstRes, LPINT lpX, LPINT lpY)
/***********************************************************************/
{
	if (iSrcRes == iDstRes || !iSrcRes)
		return;
	
	*lpX = (int)(((double)iDstRes*(*lpX+1))/iSrcRes - .5000001 /*1.0 +.4999999*/);
	*lpY = (int)(((double)iDstRes*(*lpY+1))/iSrcRes - .5000001 /*1.0 +.4999999*/);
}

/***********************************************************************/
// converts to/from center points
/***********************************************************************/
void ResConvertCenter(int iSrcRes, int iDstRes, LPLONG lpX, LPLONG lpY)
/***********************************************************************/
{
ResConvertCenter( iSrcRes, iDstRes, ( LPINT )lpX, ( LPINT )lpY );
}

/***********************************************************************/
// converts to/from center points
/***********************************************************************/
void ResConvertCenter(int iSrcRes, int iDstRes, LPINT lpX, LPINT lpY)
/***********************************************************************/
{
	if (iSrcRes == iDstRes || !iSrcRes)
		return;
	
	*lpX = (int)(((double)iDstRes*(*lpX+.5))/iSrcRes);
	*lpY = (int)(((double)iDstRes*(*lpY+.5))/iSrcRes);
}

/***********************************************************************/
// converts widths and heights, (no rounding)
// NOTE Using width and height is NOT reccomended (for accuracy reasons.)
/***********************************************************************/
void ResConvertWH(int iSrcRes, int iDstRes, LPINT lpW, LPINT lpH)
/***********************************************************************/
{
	if (iSrcRes == iDstRes || !iSrcRes)
		return;
	
	*lpW = ((long)iDstRes**lpW)/iSrcRes;
	*lpH = ((long)iDstRes**lpH)/iSrcRes;
}

/***********************************************************************/
// converts a 'distance', (no rounding)
/***********************************************************************/
void ResConvertDist(int iSrcRes, int iDstRes, LPINT lpD)
/***********************************************************************/
{
	if (iSrcRes == iDstRes || !iSrcRes)
		return;
	
	*lpD = ((long)iDstRes**lpD)/iSrcRes;
}

/***********************************************************************/
void ResConvertRect(int iSrcRes, int iDstRes, LPRECT lpRect)
/***********************************************************************/
{
	if (iSrcRes == iDstRes || !iSrcRes)
		return;
	
	lpRect->top 	= (int)(((double)iDstRes*lpRect->top )/iSrcRes + .4999999);
	lpRect->left 	= (int)(((double)iDstRes*lpRect->left)/iSrcRes + .4999999);
	lpRect->right 	= (int)(((double)iDstRes*(lpRect->right+1))/iSrcRes - .5000001 /*1.0  +.4999999*/);
	lpRect->bottom 	= (int)(((double)iDstRes*(lpRect->bottom+1))/iSrcRes - .5000001 /*1.0 +.4999999*/);

}

/************************************************************************/
void mirror8( LPTR pixels, int count )
/************************************************************************/
{
LPTR    endpixels;
BYTE    byte;

endpixels = pixels + (count - 1);

count /= 2;
while( --count >= 0 )
	{
	byte = *endpixels;
	*endpixels-- = *pixels;
	*pixels++ = byte;
	}
}

/************************************************************************/
void mirror24( LPTR pixels, int count )
/************************************************************************/
{
LPTR    endpixels;
BYTE r, g, b;

endpixels = pixels + ((count - 1)*3L);

count /= 2;
while( --count >= 0 )
	{
	r = *endpixels;
	g = *(endpixels+1);
	b = *(endpixels+2);
	*(endpixels) = *(pixels);
	*(endpixels+1) = *(pixels+1);
	*(endpixels+2) = *(pixels+2);
	endpixels -= 3;
	*pixels++ = r;
	*pixels++ = g;
	*pixels++ = b;
	}
}

/************************************************************************/
void mirror32( LPTR pixels, int count )
/************************************************************************/
{
LPTR endpixels;
BYTE r, g, b, k;

endpixels = pixels + ((count - 1)*4L);

count /= 2;
while( --count >= 0 )
	{
	r = *endpixels;
	g = *(endpixels+1);
	b = *(endpixels+2);
	k = *(endpixels+3);
	*(endpixels) = *(pixels);
	*(endpixels+1) = *(pixels+1);
	*(endpixels+2) = *(pixels+2);
	*(endpixels+3) = *(pixels+3);
	endpixels -= 4;
	*pixels++ = r;
	*pixels++ = g;
	*pixels++ = b;
	*pixels++ = k;
	}
}

/***********************************************************************/
LFIXED ScaleToFit( LPINT DestWidth, LPINT DestHeight,
                     int SrcWidth, int SrcHeight, BOOL bUseSmallerFactor)
/***********************************************************************/
{
LFIXED  scale1, scale2, rate;

/* Scale the source window to fit the destination window... */
scale1 = FGET( *DestWidth, SrcWidth );
scale2 = FGET( *DestHeight, SrcHeight );

if (bUseSmallerFactor)
	rate = ( scale1 < scale2 ) ? scale1 : scale2;
else
	rate = ( scale1 < scale2 ) ? scale2 : scale1;

/* Adjust the desination size, and return the scale factor */
*DestHeight = FMUL( SrcHeight, rate );
*DestWidth  = FMUL( SrcWidth, rate );
return( rate );
}

/************************************************************************/
void ramp( LPTR lut, int inx1, int outx1, int inx2, int outx2 )
/************************************************************************/
{
int     i;
LFIXED  accum, rate;

rate = FGET( (outx2-outx1), (inx2-inx1) );
lut += inx1;
accum = outx1;
accum += FGET(1, 2);
for ( i=inx1; i<=inx2; i++ )
	{
	*lut++ = WHOLE(accum);
	accum += rate;
	}
}

/************************************************************************/
void copy( LPTR lpSrc, LPTR lpDst, int iCount )
/************************************************************************/
{
#ifdef _X86_
__asm   {
	mov esi,DWORD PTR lpSrc	;lpSrc
	mov	edi,DWORD PTR lpDst	;lpDst
	mov	ecx,DWORD PTR iCount	;iCount

	cld
	
	mov	edx,ecx			;get iCount
	and	edx,3			;get two low bits
	shr	ecx,2			;convert for double word copy
	je	copy_bytes		; no double words to copy
	rep	movsd			; move double-words

copy_bytes:
	or	edx,edx			; any bytes to move
	je	copy_done		; no bytes to copy
	mov	ecx,edx
	rep	movsb			; move bytes

copy_done:

	}
#else
while ( --iCount >= 0 )
	{
	*lpDst++ = *lpSrc++;
	}
#endif
}


/************************************************************************/
void swapBGR( LPTR lpSrc, LPTR lpDst, int iCount )
/************************************************************************/
{
#ifdef C_CODE
	while(iCount-- > 0)
	{
   		char  temp[3];

      	temp[0] = lpSrc[2];
      	temp[1] = lpSrc[1];
      	temp[2] = lpSrc[0];
		*lpDst++ = temp[0];
		*lpDst++ = temp[1];
		*lpDst++ = temp[2];

		lpSrc+= 3;
	}
#else // !C_CODE
__asm
	{
		push    ds
		lds     si, lpSrc    ; setup Source pointer
		les     di, lpDst    ; setup Destination pointer
		mov     cx, iCount	 ; get count
		cmp     cx, 0        ; check for zero count
		jz      Done         ; zero count
Loop1:
		mov     al, ds:[si]   ; Get Blue
		mov     ah, ds:[si+1] ; Get Green
		mov     bl, ds:[si+2] ; Get Red

		mov     es:[di]  ,bl  ; Put Red
		mov     es:[di+1],ah  ; Put Green
		mov     es:[di+2],al  ; Put Blue

		add     di,3
		add     si,3
		dec     cx            ; Note that on a 486 this is faster than
		jnz     Loop1         ; the "loop" instruction
Done:
		pop     ds
	}
#endif // !C_CODE
}

/************************************************************************/
void or( LPTR lpSrc, LPTR lpDst, int iCount )
/************************************************************************/
{
#ifdef C_CODE
while ( --iCount >= 0 )
	{
	*lpDst++ |= (*lpSrc++);
	}
#else
__asm   {
	push	ds
	les     di, lpDst		; setup destination
	lds		si, lpSrc		; setup source
	mov     cx, iCount		; get count
	cld
	sub		dx,dx			; set dx = 0
	shr		cx,1			; get words to process
	adc		dx,dx			; save carry state - one more byte to process
	or		cx,cx			; any words to move?
	je		ML_OR_BYTE		; no words to move
ML_OR:
	lodsw					; get source word
	or		es:[di],ax		; or source into dest
	add		di,2			; bump dest pointer
	loop	ML_OR			; keep going
ML_OR_BYTE:
	or	dx,dx				; any bytes to process?
	je	Done				; no bytes to process
	lodsb					; get source byte
	or		es:[di],al		; or source into dest
Done:
	pop		ds
	}
#endif
}


/************************************************************************/
void nor( LPTR lpSrc, LPTR lpDst, int iCount )
/************************************************************************/
{
#ifdef C_CODE
while ( --iCount >= 0 )
	{
	*lpDst++ |= ~(*lpSrc++); // beware the squiggle in XyWrite
	}
#else
__asm   {
	push	ds
	les     di, lpDst		; setup destination
	lds		si, lpSrc		; setup source
	mov     cx, iCount		; get count
	cld
	sub		dx,dx			; set dx = 0
	shr		cx,1			; get words to process
	adc		dx,dx			; save carry state - one more byte to process
	or		cx,cx			; any words to move?
	je		ML_OR_BYTE		; no words to move
ML_OR:
	lodsw					; get source word
	not		ax				; invert word
	or		es:[di],ax		; or source into dest
	add		di,2			; bump dest pointer
	loop	ML_OR			; keep going
ML_OR_BYTE:
	or	dx,dx				; any bytes to process?
	je	Done				; no bytes to process
	lodsb					; get source byte
	not		al				; invert byte
	or		es:[di],al		; or source into dest
Done:
	pop		ds
	}
#endif
}

/************************************************************************/
void and( LPTR lpSrc, LPTR lpDst, int iCount )
/************************************************************************/
{
#ifdef C_CODE
while ( --iCount >= 0 )
	{
	*lpDst++ &= (*lpSrc++);
	}
#else
__asm   {
	push	ds
	les     di, lpDst		; setup destination
	lds		si, lpSrc		; setup source
	mov     cx, iCount		; get count
	cld
	sub		dx,dx			; set dx = 0
	shr		cx,1			; get words to process
	adc		dx,dx			; save carry state - one more byte to process
	or		cx,cx			; any words to move?
	je		ML_OR_BYTE		; no words to move
ML_OR:
	lodsw					; get source word
	and		es:[di],ax		; or source into dest
	add		di,2			; bump dest pointer
	loop	ML_OR			; keep going
ML_OR_BYTE:
	or	dx,dx				; any bytes to process?
	je	Done				; no bytes to process
	lodsb					; get source byte
	and		es:[di],al		; or source into dest
Done:
	pop		ds
	}
#endif
}


/************************************************************************/
void nand( LPTR lpSrc, LPTR lpDst, int iCount )
/************************************************************************/
{
#ifdef C_CODE
while ( --iCount >= 0 )
	{
	*lpDst++ &= ~(*lpSrc++); // beware the squiggle in XyWrite
	}
#else
__asm   {
	push	ds
	les     di, lpDst		; setup destination
	lds		si, lpSrc		; setup source
	mov     cx, iCount		; get count
	cld
	sub		dx,dx			; set dx = 0
	shr		cx,1			; get words to process
	adc		dx,dx			; save carry state - one more byte to process	or		cx,cx			; any words to move?
	je		ML_OR_BYTE		; no words to move
ML_OR:
	lodsw					; get source word
	not		ax				; invert word
	and		es:[di],ax		; or source into dest
	add		di,2			; bump dest pointer
	loop	ML_OR			; keep going
ML_OR_BYTE:
	or	dx,dx				; any bytes to process?
	je	Done				; no bytes to process
	lodsb					; get source byte
	not		al				; invert byte
	and		es:[di],al		; or source into dest
Done:
	pop		ds
	}
#endif
}

/************************************************************************/
void xor( LPTR lpSrc, LPTR lpDst, int iCount )
/************************************************************************/
{
#ifdef C_CODE
while ( --iCount >= 0 )
	{
	*lpDst++ ^= (*lpSrc++);
	}
#else
__asm   {
	push	ds
	les     di, lpDst		; setup destination
	lds		si, lpSrc		; setup source
	mov     cx, iCount		; get count
	cld
	sub		dx,dx			; set dx = 0
	shr		cx,1			; get words to process
	adc		dx,dx			; save carry state - one more byte to process
	or		cx,cx			; any words to move?
	je		ML_OR_BYTE		; no words to move
ML_OR:
	lodsw					; get source word
	xor		es:[di],ax		; or source into dest
	add		di,2			; bump dest pointer
	loop	ML_OR			; keep going
ML_OR_BYTE:
	or	dx,dx				; any bytes to process?
	je	Done				; no bytes to process
	lodsb					; get source byte
	xor		es:[di],al		; or source into dest
Done:
	pop		ds
	}
#endif
}

/************************************************************************/
void nxor( LPTR lpSrc, LPTR lpDst, int iCount )
/************************************************************************/
{
#ifdef C_CODE
while ( --iCount >= 0 )
	{
	*lpDst++ ^= ~(*lpSrc++); // beware the squiggle in XyWrite
	}
#else
__asm   {
	push	ds				; si,di saved by compiler
	les     di, lpDst		; setup destination
	lds		si, lpSrc		; setup source
	mov     cx, iCount		; get count
	cld
	sub		dx,dx			; set dx = 0
	shr		cx,1			; get words to process
	adc		dx,dx			; save carry state - one more byte to process
	or		cx,cx			; any words to move?
	je		ML_OR_BYTE		; no words to move
ML_OR:
	lodsw					; get source word
	not		ax				; invert word
	xor		es:[di],ax		; or source into dest
	add		di,2			; bump dest pointer
	loop	ML_OR			; keep going
ML_OR_BYTE:
	or	dx,dx				; any bytes to process?
	je	Done				; no bytes to process
	lodsb					; get source byte
	not		al				; invert byte
	xor		es:[di],al		; or source into dest
Done:
	pop		ds
	}
#endif
}

/************************************************************************/
void mul( LPTR lpSrc, LPTR lpDst, int iCount )
/************************************************************************/
{
#ifdef C_CODE
{
WORD s;

while ( --iCount >= 0 )
	{
	s = *lpSrc++;
	if (s > 127)
		++s;
	*lpDst++ = ((s * (WORD)*lpDst) + 128) >> 8;
	}
}
#else
__asm   {
	push	ds
	les     di, lpDst		; setup destination
	lds		si, lpSrc		; setup source
	mov     cx, iCount		; get count
	cld
	or		cx,cx			; any words to move?
	je		Done		; no words to move
ML_MUL:
	lodsb
	sub ah,ah
	cmp	ax,127
	jbe	ML_NOINC
	inc	ax
ML_NOINC:
	mov bl,es:[di]
	sub bh,bh
	mul bx
	add ax,128
	mov al,ah
	stosb
	loop	ML_MUL			; keep going
Done:
	pop		ds
	}
#endif
}

/************************************************************************/
void nmul( LPTR lpSrc, LPTR lpDst, int iCount )
/************************************************************************/
{
#ifdef C_CODE
{
WORD s;

while ( --iCount >= 0 )
	{
	s = (~(*lpSrc++));
	if (s > 127)
		++s;
	*lpDst++ = ((s * (WORD)*lpDst) + 128) >> 8;
	}
}
#else
__asm   {
	push	ds
	les     di, lpDst		; setup destination
	lds		si, lpSrc		; setup source
	mov     cx, iCount		; get count
	cld
	or		cx,cx			; any words to move?
	je		Done		; no words to move
ML_MUL:
	lodsb
	not al
	sub ah,ah
	cmp	ax,127
	jbe	ML_NOINC
	inc	ax
ML_NOINC:
	mov bl,es:[di]
	sub bh,bh
	mul bx
	add ax,128
	mov al,ah
	stosb
	loop	ML_MUL			; keep going
Done:
	pop		ds
	}
#endif
}

/************************************************************************/
void not( LPTR lpSrc, LPTR lpDst, int iCount )
/************************************************************************/
{
#ifdef C_CODE
while ( --iCount >= 0 )
	{
	*lpDst++ = ~(*lpSrc++); // beware the squiggle in XyWrite
	}
#else
__asm   {
	push	ds
	les     di, lpDst		; setup destination
	lds		si, lpSrc		; setup source
	mov     cx, iCount		; get count
	cld
	sub		dx,dx			; set dx = 0
	shr		cx,1			; get words to process
	adc		dx,dx			; save carry state - one more byte to process
	or		cx,cx			; any words to move?
	je		ML_INV_BYTE		; no words to move
ML_INV:
	lodsw					; get source word
	not 	ax				; invert word
	stosw					; put word
	loop	ML_INV			; keep going
ML_INV_BYTE:
	or		dx,dx			; any bytes to process?
	je		Done			; no bytes to process
	lodsb					; get source byte
	not		al				; invert byte
	stosb					; put byte
Done:
	pop		ds
	}
#endif
}

/************************************************************************/
void CombineData( LPTR lpSrc, LPTR lpDst, int iCount,
				BOOL bInvert, COMBINE_ROP ROP )
/************************************************************************/
{
switch (ROP)
	{
	case CR_OR:
		if (bInvert)
			nor( lpSrc, lpDst, iCount );
		else
			or( lpSrc, lpDst, iCount );
		break;

	case CR_AND:
		if (bInvert)
			nand( lpSrc, lpDst, iCount );
		else
			and( lpSrc, lpDst, iCount );
		break;

	case CR_XOR:
		if (bInvert)
			nxor( lpSrc, lpDst, iCount );
		else
			xor( lpSrc, lpDst, iCount );
		break;

	case CR_MULTIPLY:
		if (bInvert)
			nmul( lpSrc, lpDst, iCount );
		else
			mul( lpSrc, lpDst, iCount );
		break;

	default:
		if (bInvert)
			not( lpSrc, lpDst, iCount );
		else
			copy( lpSrc, lpDst, iCount );
		break;
	}
}

/************************************************************************/

void negate( LPTR lpDst, long lCount )
{
#ifndef C_CODE
	WORD wCount;
#endif

	if (lCount >= 65536)
	{
		HPTR hpBuf;
		hpBuf = (HPTR)lpDst;
		while ( --lCount >= 0 )
			*hpBuf++ ^= 0xFF;
		return;
	}

#ifdef C_CODE
while ( --lCount >= 0 )
	{
	*lpDst++ ^= 255;
	}
#else
	wCount = (WORD)lCount;
__asm   {
	les     di, lpDst
	mov     cx, wCount
	cmp		cx, 0
	jz		NEGATE_DONE
	cld
	sub     dx, dx		; clear dx to hold carry
	shr     cx, 1
	jz		NEGATE_ONE
	adc		dx,dx		; save carry state - one more byte to process
NEGATE_LOOP:
	mov		ax,es:[di]
	not		ax
	stosw
	loop	NEGATE_LOOP
	or		dx,dx
	je		NEGATE_DONE
NEGATE_ONE:
	mov		al,es:[di]
	not		al
	stosb
NEGATE_DONE:
		}
#endif
}

/************************************************************************/
void clr( LPTR lpDst, int iCount )
/************************************************************************/
{
#ifdef C_CODE
while ( --iCount >= 0 )
	*lpDst++ = 0;
#else
__asm   {
	les     di, lpDst
	mov     cx, iCount
	cld
	xor     ax, ax
	shr     cx, 1
	rep     stosw
	jnc     Done
	stosb
Done:
	}
#endif
}

/************************************************************************/
void setifset( LPTR lpDst, int iCount, int iValue )
/************************************************************************/
{
#ifdef C_CODE
while ( --iCount >= 0 )
	{
	if ( *lpDst )
		*lpDst++ = iValue;
	else     lpDst++;
	}
#else
__asm   {
	les     di, lpDst
	mov     cx, iCount
	mov     ax, iValue
	cld
SetIfSetLoop:
	mov     dl, BYTE PTR es:[di]
	or      dl, dl
	je      SetIfSetNone
	stosb
	loop    SetIfSetLoop
	jmp     Done
SetIfSetNone:
	inc     di
	loop    SetIfSetLoop
Done:
	}
#endif
}


/************************************************************************/
void set( LPTR lpDst, int iCount, int iValue )
/************************************************************************/
{
#ifdef C_CODE
while ( --iCount >= 0 )
	*lpDst++ = iValue;
#else
__asm   {
	les     di, lpDst
	mov     cx, iCount
	mov     dx, iValue
	xor     dh, dh
	mov     ax, dx
	shl     dx, 8
	or      ax, dx
	cld
	shr     cx, 1
	rep     stosw
	jnc     Done
	stosb
Done:
	}
#endif
}

/************************************************************************/
void set16( LPWORD ptr, int count, WORD value )
/************************************************************************/
{
while ( --count >= 0 )
	*ptr++ = value;
}

/************************************************************************/
void set24( LPRGB lpRGB, int iCount, long lValue )
/************************************************************************/
{
#ifdef _X86_
// the code below was written in assember because version
// 9.00 of the MS compiler could no optimize it properly
// it tried to optimize by doing a rep movsd which does
// not work for a structure of 3 bytes
__asm   {
	mov		edi, DWORD PTR lpRGB
	mov     ecx, DWORD PTR iCount
	mov     ax, WORD PTR lValue     ; R and G
	mov     dx, WORD PTR lValue+2   ; B
	cld
Set24_1:
	stosw
	mov	BYTE PTR [edi], dl
	inc     edi
	loop    Set24_1
	}
#else
RGBS rgb;

CopyRGB( &lValue, &rgb );
while ( --iCount >= 0 )
	{
	*lpRGB++ = rgb;
	}
#endif
}

/************************************************************************/

void set32( LPCMYK lpCMYK, int iCount, long lValue )
{
	CMYKS cmyk;

	CopyCMYK( &lValue, &cmyk );
	while ( --iCount >= 0 )
	{
		*lpCMYK++ = cmyk;
	}
}

/************************************************************************/
void hist( LPTR pixels, int count, long histo[256] )
/************************************************************************/
{
while ( --count >= 0 )
	{
 	histo[*pixels++]++;
	}
}

/************************************************************************/
void swapw( LPWORD word )
/************************************************************************/
{
LPTR pb;
BYTE b1, b2;

pb = (LPTR)(word);
b1 = *pb;
pb++;
b2 = *pb;
*pb = b1;
pb--;
*pb = b2;
}

/************************************************************************/
void swapl( LPDWORD dword )
/************************************************************************/
{
LPWORD pw;
WORD w1, w2;

pw = (LPWORD)(dword);
w1 = *pw;
pw++;
w2 = *pw;
*pw = w1;
swapw( pw );
pw--;
*pw = w2;
swapw( pw );
}

/************************************************************************/
void Unpack4( LPTR lpIn, int iCount, LPTR lpOut )
/************************************************************************/
{
BYTE    b;

while ( --iCount >= 0 )
	{
	b = *lpIn++;
	*lpOut++ = b & 0xF0;
	*lpOut++ = (b << 4);
	}
}

/************************************************************************/
void ScaleRect( LPRECT lpRect, LFIXED Scale )
/************************************************************************/
{
lpRect->left   = FMUL( lpRect->left, Scale );
lpRect->top    = FMUL( lpRect->top, Scale );
lpRect->right  = FMUL( lpRect->right+1, Scale ) - 1;
lpRect->bottom = FMUL( lpRect->bottom+1, Scale ) - 1;
}

#ifndef USING_FIXED16

#ifdef UNUSED
static long sintab[180] = 
 

   { 0, 
   74957514, 
   149892196, 
   224781220, 
   299601773, 
   374331064, 
   448946331, 
   523424844, 
   597743916, 
   671880911, 
   745813243, 
   819518394, 
   892973912, 
   966157421, 
   1039046629, 
   1111619334, 
   1183853428, 
   1255726909, 
   1327217884, 
   1398304575, 
   1468965329, 
   1539178622, 
   1608923067, 
   1678177417, 
   1746920579, 
   1815131612, 
   1882789738, 
   1949874348, 
   2016365008, 
   2082241463, 
   2147483647, 
   2212071687, 
   2275985908, 
   2339206842, 
   2401715231, 
   2463492035, 
   2524518435, 
   2584775842, 
   2644245901, 
   2702910497, 
   2760751761, 
   2817752073, 
   2873894070, 
   2929160651, 
   2983534982, 
   3037000499, 
   3089540916, 
   3141140229, 
   3191782720, 
   3241452964, 
   3290135829, 
   3337816487, 
   3384480414, 
   3430113396, 
   3474701531, 
   3518231239, 
   3560689260, 
   3602062660, 
   3642338837, 
   3681505522, 
   3719550785, 
   3756463037, 
   3792231034, 
   3826843880, 
   3860291033, 
   3892562304, 
   3923647863, 
   3953538240, 
   3982224331, 
   4009697399, 
   4035949073, 
   4060971358, 
   4084756633, 
   4107297650, 
   4128587545, 
   4148619833, 
   4167388410, 
   4184887561, 
   4201111954, 
   4216056648, 
   4229717091, 
   4242089120, 
   4253168968, 
   4262953260, 
   4271439014, 
   4278623647, 
   4284504970, 
   4289081191, 
   4292350916, 
   4294313150, 
     4294967295,  // 90
  4294313150, 
  4292350916, 
  4289081191, 
  4284504970, 
  4278623647, 
  4271439014, 
  4262953260, 
  4253168968, 
  4242089120, 
  4229717091, 
   4216056648, 
   4201111954, 
   4184887561, 
   4167388410, 
   4148619833, 
   4128587546, 
   4107297650, 
   4084756633, 
   4060971359, 
   4035949073, 
   4009697399, 
   3982224331, 
   3953538240, 
   3923647863, 
   3892562304, 
   3860291033, 
   3826843881, 
   3792231034, 
   3756463037, 
   3719550786, 
   3681505522, 
   3642338837, 
   3602062660, 
   3560689260, 
   3518231240, 
   3474701532, 
   3430113396, 
   3384480415, 
   3337816488, 
   3290135829, 
   3241452964, 
   3191782721, 
   3141140229, 
   3089540916, 
   3037000499, 
   2983534982, 
   2929160651, 
   2873894070, 
   2817752073, 
   2760751761, 
   2702910497, 
   2644245901, 
   2584775842, 
   2524518435, 
   2463492035, 
   2401715232, 
   2339206843, 
   2275985909, 
   2212071687, 
   2147483647, 
   2082241463, 
   2016365008, 
   1949874348, 
   1882789738, 
   1815131612, 
   1746920579, 
   1678177418, 
   1608923067, 
   1539178623, 
   1468965330, 
   1398304576, 
   1327217884, 
   1255726910, 
   1183853428, 
   1111619334, 
   1039046629, 
   966157421,
   892973912,
   819518395, 
   745813244, 
   671880911, 
   597743917, 
   523424844, 
   448946331, 
   374331065, 
   299601773, 
   224781220, 
   149892197, 
   74957515 };
#endif

/************************************************************************/
void SinCos( int iAngle, LPLFIXED lplSin, LPLFIXED lplCos )
/************************************************************************/
{
while ( iAngle < 0 ) iAngle += 360;
while ( iAngle > 360 ) iAngle -= 360;

#define PI 3.14159265

*lplSin = sin((double)iAngle * PI / 180.);
*lplCos = cos((double)iAngle * PI / 180.);

#ifdef UNUSED
LFIXED   sin;
LFIXED   cos;
DWORD    val;

if ( iAngle < 180)
	{
	if ( ( val = sintab[ iAngle ] ) == 4294967295 )
      sin = 1;
   else
      sin = (double)val / 4294967295.;
	}
else
   {
	if ( ( val = sintab[ iAngle ] ) == 4294967295 )
      sin = -1;
	else
      sin = -((double)val / 4294967295.);
	}
*lplSin = sin;

iAngle += 90;
if ( iAngle > 360 ) iAngle -= 360;
if ( iAngle < 180)
	{
	if ( ( val = sintab[ iAngle ] ) == 4294967295 )
 	   cos = 1;
   else
      cos = (double)val / 4294967295.;
	}
else
   {
	if ( ( val = sintab[ iAngle ] ) == 4294967295 )
      cos = -1;
	else
      cos = -((double)val / 4294967295.);
	}
*lplCos = cos;
#endif
}

#endif

#ifdef WIN32
/************************************************************************/
void SinCos( LFIXED iAngle, LPLFIXED lplSin, LPLFIXED lplCos )
/************************************************************************/
{
// while ( iAngle < 0 ) iAngle += TOFIXED(360);
// while ( iAngle > (TOFIXED(360)) ) iAngle -= TOFIXED(360);

#define PI 3.14159265

*lplSin = DBL2FIX(sin(DOUBLE(iAngle) * PI / 180.));
*lplCos = DBL2FIX(cos(DOUBLE(iAngle) * PI / 180.));

}
#endif

#ifdef NOTENTHS

static WORD sintab[180] = {
    0, 1144, 2287, 3430, 4572, 5712, 6850, 7987, 9121,10252,
11380,12505,13626,14742,15855,16962,18064,19161,20252,21336,
22415,23486,24550,25607,26656,27697,28729,29753,30767,31772,
32768,33754,34729,35693,36647,37590,38521,39441,40348,41243,
42126,42995,43852,44695,45525,46341,47143,47930,48703,49461,
50203,50931,51643,52339,53020,53684,54332,54963,55578,56175,
56756,57319,57865,58393,58903,59396,59870,60326,60764,61183,
61584,61966,62328,62672,62997,63303,63589,63856,64104,64332,
64540,64729,64898,65048,65177,65287,65376,65446,65496,65526,
65535,65526,65496,65446,65376,65287,65177,65048,64898,64729,
64540,64332,64104,63856,63589,63303,62997,62672,62328,61966,
61584,61183,60764,60326,59870,59396,58903,58393,57865,57319,
56756,56175,55578,54963,54332,53684,53020,52339,51643,50931,
50203,49461,48703,47930,47143,46341,45525,44695,43852,42995,
42126,41243,40348,39441,38521,37590,36647,35693,34729,33754,
32768,31772,30767,29753,28729,27697,26656,25607,24550,23486,
22415,21336,20252,19161,18064,16962,15855,14742,13626,12505,
11380,10252, 9121, 7987, 6850, 5712, 4572, 3430, 2287, 1144 };

/************************************************************************/
void SinCos( int iAngle, LPLONG lplSin, LPLONG lplCos )
/************************************************************************/
{
long l;

while ( iAngle < 0 ) iAngle += 360;
while ( iAngle > 360 ) iAngle -= 360;
if ( iAngle < 180)
	{
	l =  (long)sintab[ iAngle ];
	if ( l ==  65535L ) l++;
	}
else    {
	l = -(long)sintab[ iAngle-180 ];
	if ( l == -65535L ) l--;
	}
*lplSin = l;

iAngle += 90;
if ( iAngle > 360 ) iAngle -= 360;
if ( iAngle < 180 )
	{
	l =  (long)sintab[ iAngle ];
	if ( l ==  65535L ) l++;
	}
else    {
	l = -(long)sintab[ iAngle-180 ];
	if ( l == -65535L ) l--;
	}
*lplCos = l;
}

#endif

static WORD Roots[257] = {
    0,    1,    4,    9,   16,   25,   36,   49,   64,   81,  100,  121,
  144,  169,  196,  225,  256,  289,  324,  361,  400,  441,  484,  529,
  576,  625,  676,  729,  784,  841,  900,  961, 1024, 1089, 1156, 1225,
 1296, 1369, 1444, 1521, 1600, 1681, 1764, 1849, 1936, 2025, 2116, 2209,
 2304, 2401, 2500, 2601, 2704, 2809, 2916, 3025, 3136, 3249, 3364, 3481,
 3600, 3721, 3844, 3969, 4096, 4225, 4356, 4489, 4624, 4761, 4900, 5041,
 5184, 5329, 5476, 5625, 5776, 5929, 6084, 6241, 6400, 6561, 6724, 6889,
 7056, 7225, 7396, 7569, 7744, 7921, 8100, 8281, 8464, 8649, 8836, 9025,
 9216, 9409, 9604, 9801,10000,10201,10404,10609,10816,11025,11236,11449,
11664,11881,12100,12321,12544,12769,12996,13225,13456,13689,13924,14161,
14400,14641,14884,15129,15376,15625,15876,16129,16384,16641,16900,17161,
17424,17689,17956,18225,18496,18769,19044,19321,19600,19881,20164,20449,
20736,21025,21316,21609,21904,22201,22500,22801,23104,23409,23716,24025,
24336,24649,24964,25281,25600,25921,26244,26569,26896,27225,27556,27889,
28224,28561,28900,29241,29584,29929,30276,30625,30976,31329,31684,32041,
32400,32761,33124,33489,33856,34225,34596,34969,35344,35721,36100,36481,
36864,37249,37636,38025,38416,38809,39204,39601,40000,40401,40804,41209,
41616,42025,42436,42849,43264,43681,44100,44521,44944,45369,45796,46225,
46656,47089,47524,47961,48400,48841,49284,49729,50176,50625,51076,51529,
51984,52441,52900,53361,53824,54289,54756,55225,55696,56169,56644,57121,
57600,58081,58564,59049,59536,60025,60516,61009,61504,62001,62500,63001,
63504,64009,64516,65025,65535 };

/************************************************************************/
WORD lsqrt( long lValue )
/************************************************************************/
{
UINT i;
#ifdef C_CODE
UINT r, hi, lo, Shifts;
long l;

Shifts = 0;
while ( lValue > 65535 )
	{
	lValue >>= 2;
	++Shifts;
	}

lo = 0;
hi = 256;
while (1)
	{
	i = (lo+hi) >> 1;
	if ( i == lo ) /* hi and lo must be consecutive */
		break;
	r = Roots[i];
	l = lValue + i;
	if ( l <= r )
		hi = i;
	else
	if ( l >= r )
		lo = i;
	else    break;
	}
if ( Shifts )
	i <<= Shifts;
#else
__asm   {
	mov     ax, WORD PTR lValue
	mov     dx, WORD PTR lValue+2
	sub     cx, cx                  ; Shifts = 0;
lsqrt_1:
	or      dx, dx                  ; any HIWORD(lValue)?
	jne     lsqrt_6                 ; nope, we shifted enough
	cmp     ax, 65279               ; max value is 65535-256
	jbe     lsqrt_2                 ; its cool
lsqrt_6:
	shr     dx, 1                   ; HIWORD(lValue)>>1
	rcr     ax, 1                   ; LOWORD(lValue)>>1
	shr     dx, 1                   ; HIWORD(lValue)>>1
	rcr     ax, 1                   ; LOWORD(lValue)>>1
	inc     cx                      ; Shifts += 1
	jmp     lsqrt_1                 ; keep going
lsqrt_2:
	mov     dx, ax                  ; save lValue
	push    cx
	mov     si, OFFSET Roots        ; offset to roots table - ds:[si]
	mov     bx, 0                   ; BX = lo = 0;
	mov     cx, 256                 ; CX = hi = 256;
	mov     ax, bx                  ; DX = lo
	add     ax, cx                  ; DX = lo + hi
lsqrt_3:
	shr     ax, 1                   ; DX = i = (lo + hi)/2
	cmp     ax, bx                  ; if (i == lo)
	je      lsqrt_5                 ; break;
	mov     di, ax
	shl     di, 1
	add     di, si
	add     dx, ax
	cmp     dx, WORD PTR ds:[di]    ; lValue <= r
	ja      lsqrt_4                 ; lValue > r
	sub     dx, ax
	mov     cx, ax                  ; hi = i
	add     ax, bx                  ; lo + hi
	jmp     lsqrt_3
lsqrt_4:
	sub     dx, ax
	mov     bx, ax                  ; lo = i
	add     ax, cx                  ; lo + hi
	jmp     lsqrt_3
lsqrt_5:
	pop     cx
	or      cx, cx
	je      lsqrt_10
	shl     ax, cl
lsqrt_10:
	mov		WORD PTR i,ax
	}
#endif
return( i );
}

/***********************************************************************/
void LineartSetBits   (LPTR lpData, int iBegin, int iCount)
/***********************************************************************/
{
	int ByteStart, ByteEnd, ByteCount;
	BYTE BitStartMask, BitEndMask;
	LPTR lptr;
	
	if (!iCount)
		return;
	ByteStart = iBegin>>3;
	// calculate the unused start bits
	BitStartMask = (BYTE)0xff >> (iBegin - (ByteStart<<3));
	ByteEnd = (iBegin+iCount-1)>>3;
	// clculate the unused end bits
	BitEndMask = (BYTE)0xff << ((ByteEnd<<3) + 7 - (iBegin+iCount-1));
	ByteCount = ByteEnd - ByteStart - 1;
	
	// special case 1 byte
	if (ByteStart == ByteEnd)
	{
		lpData[ByteStart] |= (BitStartMask & BitEndMask);
		return;
	}
	// do actual setting
	lptr = &lpData[ByteStart];
	*lptr++ |= BitStartMask;
	while(ByteCount-- > 0)
	{
		*lptr++ = 0xff;
	}
	*lptr |= BitEndMask;
}

/***********************************************************************/
void LineartClearBits (LPTR lpData, int iBegin, int iCount)
/***********************************************************************/
{
	int ByteStart, ByteEnd, ByteCount;
	BYTE BitStartMask, BitEndMask;
	LPTR lptr;
	
	if (!iCount)
		return;
	ByteStart = iBegin>>3;
	// calculate the unused start bits
	BitStartMask = (BYTE)0xff >> (iBegin - (ByteStart<<3));
	ByteEnd = (iBegin+iCount-1)>>3;
	// clculate the unused end bits
	BitEndMask = (BYTE)0xff << ((ByteEnd<<3) + 7 - (iBegin+iCount-1));
	ByteCount = ByteEnd - ByteStart - 1;
	
	// special case 1 byte
	if (ByteStart == ByteEnd)
	{
		lpData[ByteStart] &= ~(BitStartMask & BitEndMask);
		return;
	}
	// do actual setting
	lptr = &lpData[ByteStart];
	*lptr++ &= ~BitStartMask;
	while(ByteCount-- > 0)
	{
		*lptr++ = 0x00;
	}
	*lptr &= ~BitEndMask;
}

/***********************************************************************/
void LineartNegateBits(LPTR lpData, int iBegin, int iCount)
/***********************************************************************/
{
	int ByteStart, ByteEnd, ByteCount;
	BYTE BitStartMask, BitEndMask;
	LPTR lptr;
	
	if (!iCount)
		return;
	ByteStart = iBegin>>3;
	// calculate the unused start bits
	BitStartMask = (BYTE)0xff >> (iBegin - (ByteStart<<3));
	ByteEnd = (iBegin+iCount-1)>>3;
	// clculate the unused end bits
	BitEndMask = (BYTE)0xff << ((ByteEnd<<3) + 7 - (iBegin+iCount-1));
	ByteCount = ByteEnd - ByteStart - 1;
	
	// special case 1 byte
	if (ByteStart == ByteEnd)
	{
		lpData[ByteStart] ^= (BitStartMask & BitEndMask);
		return;
	}
	// do actual setting
	lptr = &lpData[ByteStart];
	*lptr++ ^= BitStartMask;
	while(ByteCount-- > 0)
	{
		*lptr++ ^= 0xff;
	}
	*lptr ^= BitEndMask;
}

/************************************************************************/
void ScaleData8( LPTR lpDst, int iCount, BYTE factor )
/************************************************************************/
{
#ifdef C_CODE
WORD wFactor;

wFactor = factor;
++wFactor;
while ( --iCount >= 0 )
	*lpDst++ = ((WORD)*lpDst * wFactor) >> 8;
#else
__asm   {
	push	ds
	pushf
	push	di
	cld
	les     di, lpDst
	mov     cx, iCount
	mov     bl, factor
	sub		bh,bh
	inc		bx
Scale8Loop:
	mov     al, BYTE PTR es:[di]
	sub		ah,ah
	mul		bx
	mov		BYTE PTR es:[di],ah
	inc		di
	loop    Scale8Loop
	pop		di
	popf
	pop		ds
	}
#endif
}

/************************************************************************/
void ScaleDataBuf8( LPTR lpDst, int iCount, LPTR lpMsk )
/************************************************************************/
{
#ifdef C_CODE
WORD wFactor;

while ( --iCount >= 0 )
	{
	wFactor = (WORD)(*lpMsk++) + 1;
	*lpDst++ = ((WORD)*lpDst * wFactor) >> 8;
	}
#else
__asm   {
	push	ds
	pushf
	push	di
	push	si
	cld
	les     di, lpDst
	lds		si, lpMsk
	mov     cx, iCount
	sub		bh,bh
SDB8_LOOP:
	lodsb				// get factor in al
	sub		ah,ah		// cast to WORD
	inc		ax
	mov     bl, BYTE PTR es:[di]
	mul		bx
	mov		BYTE PTR es:[di],ah
	inc		di
	loop    SDB8_LOOP
	pop		si
	pop		di
	popf
	pop		ds
	}
#endif
}

/************************************************************************/
void ScaleDataBuf8Huge( HPTR lpDst, DWORD dwCount, HPTR lpMsk )
/************************************************************************/
{
// It is assumed that lpSrc and lpDst are allocated pointers
// That they both point to the beginning of an allocated block
// i.e. - zero offset
	int iCount;

	// get current position in segment
	while (dwCount)
	{
		// get how many bytes left in segment
		if (dwCount >= 16384L)
			iCount = 16384;
		else
			iCount = (int)dwCount;
		ScaleDataBuf8(lpDst, iCount, lpMsk);
		lpDst += iCount;
		lpMsk += iCount;
 		dwCount -= iCount;
	}
}

static TCHAR aschex[16] = 
{ '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

/************************************************************************/
int asciize( LPTR in, int count, LPTSTR out )
/************************************************************************/
{
	register int lCount;
	register int rCount;
	unsigned char temp;

	rCount = (2*count);
	lCount = 0;

	while( --count >= 0 )
	{
		temp = *in++;
		*out = aschex[((temp >> 4) & 15)]; 	out = MyCharNext( out );
		*out = aschex[temp & 15];		   	out = MyCharNext( out );
		if (++lCount >= 40)
		{
			*out = '\r';					out = MyCharNext( out );
			*out = '\n';					out = MyCharNext( out );
			rCount += 2;
			lCount = 0;
		}
	}
	if (lCount)
	{
		*out = '\r';					   out = MyCharNext( out );
		*out = '\n';					   out = MyCharNext( out );
		rCount += 2;
	}
	*out = '\0';

	return( rCount );
}

#ifdef UNUSED

/***********************************************************************/
void la2con( LPTR lpIn, int iCount, LPTR lpOut, int normal )
/***********************************************************************/
{
#define THRESH  127
#ifdef C_CODE
BYTE    byte, bit, black, white;

if ( normal )
	{ white = 0;  black = 255; }
else    { white = 255; black = 0;  }

bit = 1;
while ( --iCount >= 0 )
	{
	if ( --bit == 0 )
		{
		byte = *lpIn++;
		bit = 8;
		}
	if ( byte > THRESH )
		*lpOut++ = black;
	else    *lpOut++ = white;
	byte <<= 1;
	}
#else
__asm   {
	push	ds
	mov     cx, iCount
	or      cx, cx
	je      la2con_end
	lds     si, lpIn
	les     di, lpOut
	cld
	mov     bl, 1                   ; bl = bit
	cmp     normal, 0
	je      la2con_1
	mov     dl, 0                   ;dl = white
	mov     dh, 255                 ;dh = black
	jmp     la2con_loop
la2con_1:
	mov     dl, 255                 ;dl = white
	mov     dh, 0                   ;dh = black

la2con_loop:
	dec     bl                      ;bit
	jne     la2con_2
	lodsb                           ;ax=*lpIn++
	mov     bh, al                  ;bh=byte
	mov     bl, 8                   ;bl=bit=8
la2con_2:
	cmp     bh, 127                 ;byte
	jbe     la2con_3
	mov     al, dh                  ;black
	jmp     la2con_4
la2con_3:
	mov     al, dl                  ;white
la2con_4:
	stosb
	shl     bh, 1                   ;byte
	loop    la2con_loop
la2con_end:
	pop		ds
	}
#endif
}


/***********************************************************************/
void con2la( LPTR lpIn, int iCount, LPTR lpOut )
/***********************************************************************/
{
#define THRESH  127
#ifdef C_CODE
BYTE    byte, bit;

byte = 0;
bit = 128;
while ( --iCount >= 0 )
	{
	if (*lpIn++ <= THRESH)
		byte |= bit;
	if ( (bit >>= 1) == 0 )
		{
		*lpOut++ = byte;
		byte = 0;
		bit = 128;
		}
	}

if ( bit != 128 )
	*lpOut++ = byte;
#else
__asm   {
	push	ds
	mov     cx, iCount
	or      cx, cx
	je      con2la_end
	lds     si, lpIn
	les     di, lpOut
	cld
	mov     bl, 128                 ; bit
	mov     bh, 0                   ; byte

con2la_loop:
	lodsb                           ; al = *lpIn++
	cmp     al, 127
	ja      con2la_1
	or      bh, bl                  ; byte |= bit
con2la_1:
	shr     bl, 1                   ;bit
 	jne     con2la_2
	mov     al, bh
	stosb
	mov     bl, 128                 ;bit
	mov     bh, 0                   ;byte
con2la_2:
	loop    con2la_loop

	cmp     bl, 128                 ;bit
	je      con2la_end
	mov     al, bh
	stosb
con2la_end:
	pop		ds
	}
#endif
}


#endif // UNUSED



