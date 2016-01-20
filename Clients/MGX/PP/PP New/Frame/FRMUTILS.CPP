/*=======================================================================*\

	FRMUTILS.C - Memory Management routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include "mgxframe.h"
#include "frameint.h"
#include "windowsx.h"


/************************************************************************/
LPTR Alloc( long lCount )
/************************************************************************/
{
return( AllocX( lCount, 0 ) );
}

/************************************************************************/
LPTR AllocX( long lCount, WORD wFlags )
/************************************************************************/
{
LPTR     lpMemory;
HGLOBAL  hMem;

// Round up to the next 16 byte boundary
lCount++; // Protection against a possible C7.0 bug
lCount = ((lCount + 15) / 16) * 16;

// Allocate the memory from the global heap - NOT limited to 64K
hMem = GlobalAlloc(GMEM_MOVEABLE | wFlags, lCount);
if (!hMem)
	return(NULL);

lpMemory = (LPTR)GlobalLock(hMem);
if (!lpMemory)
	GlobalFree(hMem);

return( lpMemory );
}

/************************************************************************/
void FreeUp( LPVOID lpMemory )
/************************************************************************/
{
HGLOBAL hMem;

if ( !lpMemory )
	return;

//GlobalFreePtr(lpMemory);
// use explicit functions instead of all encompassing macro

hMem = GlobalPtrHandle(lpMemory);
if (!hMem)
	return;
GlobalUnlock(hMem);
GlobalFree(hMem);
}

/************************************************************************/
DWORD AvailableMemory()
/************************************************************************/
{
#ifdef WIN32
MEMORYSTATUS ms;

GlobalMemoryStatus(&ms);
return(ms.dwAvailVirtual);
#else
return( GetFreeSpace(0) );
#endif
}

/************************************************************************/
void negate( LPTR lpDst, long lCount )
/************************************************************************/
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

/***********************************************************************/
LPSTR Int2Ascii( LPSTR lpAscii, int i )
/***********************************************************************/
{
wsprintf( lpAscii, "%d", i );
return( lpAscii );
}


/***********************************************************************/
int Ascii2Int( LPSTR lpAscii )
/***********************************************************************/
{
	BYTE c;
	BOOL neg;
	long lValue;

	if ( !lpAscii )
		return( 0 );
	neg = NO;
	lValue = 0;

	while ( c = (*lpAscii++) )
	{
		if ( c == '-' )
			neg = YES;
		else
		if ( c >= '0' && c <= '9' )
		{
			lValue *= 10L;
			lValue += (c-'0');
		}
		else
			if ( c >= ' ' )
				break;
	}

	if (lValue > 32767)
		lValue = 32767;

	if ( neg )
		lValue = -lValue;

	return( (int)lValue );
}

