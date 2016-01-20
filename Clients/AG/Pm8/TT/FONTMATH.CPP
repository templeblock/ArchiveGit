/*
$Header: /PM8/TT/FONTMATH.CPP 1     3/03/99 6:35p Gbeddow $
$Log: /PM8/TT/FONTMATH.CPP $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:15:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:06   Fred
// Initial revision.
// 
//    Rev 1.1   18 Apr 1997 12:49:38   Fred
// Changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:18   Jay
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:14:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:41:42   FRED
// Initial revision.
// 
//    Rev 1.0   29 Sep 1995 13:36:58   FRED
// Initial revision.
 * 
 *    Rev 1.13   14 Jan 1993 16:58:40   JAY
 * Assembly versions/enhancements and defines for FracSqrt and FracDiv.
 * 
 *    Rev 1.12   06 Oct 1992 14:38:52   MACDUFF
 * Fixed rounding in ShortFracMul
 * 
 *    Rev 1.11   15 Sep 1992 15:58:30   ADAM
 * Some MLS assembler code was referencing the compilers far procedures.
 * These are no longer available. They are now: NEAR_ followed by the name.
 * 
 *    Rev 1.10   01 Sep 1992 10:58:48   FRED
 * Fixed bug in assembly version of CompDiv().
 * 
 * 
 *    Rev 1.9   01 Sep 1992 09:23:48   FRED
 * Added PVCS variables.
 * 
*/

/*
    File:       FontMath.c

    Contains:   xxx put contents here xxx

    Written by: xxx put writers here xxx

    Copyright:  c 1990 by Apple Computer, Inc., all rights reserved.

    Change History (most recent first):

         <3>     11/9/90    MR      Fix CompDiv when numer and denom have zero hi longs. [rb]
         <2>     11/5/90    MR      Remove Types.h from include list, rename FixMulDiv to LongMulDiv
                                    [rb]
         <1>    10/20/90    MR      Math routines for font scaler. [rj]

    To Do:
*/

#include "fscdefs.h"
#include "fontmath.h"

#pragma warning(disable:4035)

#define FASTMATH

#ifdef WIN32
#define	INLINEMATH
#endif
      
#ifdef INLINEMATH
void FS_PC_PASCAL Add64(int32 a[2], int32 b[2], int32 r[2])
{
	__asm
	{
		mov	ebx,dword ptr a
		mov	edx,[ebx+0]
		mov	eax,[ebx+4]
		mov	ebx,dword ptr b
		add	eax,[ebx+4]
		adc	edx,[ebx+0]
		mov	ebx,dword ptr r
		mov	[ebx+0],edx
		mov	[ebx+4],eax
	}
}

void FS_PC_PASCAL Shr64(int32 a[2], uint32 c, int32 r[2])
{
	__asm
	{
		mov	ebx,dword ptr a
		mov	edx,[ebx+0]
		mov	eax,[ebx+4]
		mov	cx,word ptr c
		cmp	cx,32
		jb		shift_in_range
		sub	cx,32
		mov	eax,edx
		xor	edx,edx
	shift_in_range:
		shrd	eax,edx,cl
		shr	edx,cl
		mov	ebx,dword ptr r
		mov	[ebx+0],edx
		mov	[ebx+4],eax
	}
}

void FS_PC_PASCAL Mul64(int32 a, int32 b, int32 r[2])
{
	__asm
	{
		mov	eax,dword ptr a
		imul	dword ptr b
		mov	ebx,dword ptr r
		mov	[ebx+0],edx
		mov	[ebx+4],eax
	}
}

void FS_PC_PASCAL MulAdd64(int32 a, int32 b, int32 c, int32 r[2])
{
	__asm
	{
		mov	eax,dword ptr a
		imul	dword ptr b
		mov	ebx,dword ptr r
		mov	[ebx+0],edx
		mov	[ebx+4],eax
		mov	eax,dword ptr c
		cdq
		add	[ebx+4],eax
		adc	[ebx+0],edx
	}
}

int32 FS_PC_PASCAL Div64(int32 a[2], int32 b)
{
	__asm
	{
		mov	ebx,dword ptr a
		mov	edx,[ebx+0]
		mov	eax,[ebx+4]
		idiv	dword ptr b
	}
}

int32 FS_PC_PASCAL Mod64(int32 a[2], int32 b)
{
	__asm
	{
		mov	ebx,dword ptr a
		mov	edx,[ebx+0]
		mov	eax,[ebx+4]
		idiv	dword ptr b
		mov	eax,edx
	}
}

int32 FS_PC_PASCAL MulDiv64(int32 a, int32 b, int32 c)
{
	_asm
	{
		xor	ecx,ecx

		mov	eax,dword ptr c
		cdq
		xor	eax,edx
		sub	eax,edx
		push	eax
		xor	ecx,edx

		mov	eax,dword ptr b
		cdq
		xor	eax,edx
		sub	eax,edx
		mov	ebx,eax
		xor	ecx,edx

		mov	eax,dword ptr a
		cdq
		xor	eax,edx
		sub	eax,edx
		xor	ecx,edx

		mul	ebx

		pop	ebx

		cmp	ebx,edx
		ja		no_overflow

		mov	eax,0x7fffffff

		jmp	adjust_sign

no_overflow:

		div	ebx

adjust_sign:

		xor	eax,ecx
		sub	eax,ecx
	}
}

void FS_PC_PASCAL UMul64(uint32 a, uint32 b, uint32 r[2])
{
	__asm
	{
		mov	eax,dword ptr a
		mul	dword ptr b
		mov	ebx,dword ptr r
		mov	[ebx+0],edx
		mov	[ebx+4],eax
	}
}

int32 FS_PC_PASCAL UDiv64(uint32 a[2], uint32 b)
{
	__asm
	{
		mov	ebx,dword ptr a
		mov	edx,[ebx+0]
		mov	eax,[ebx+4]
		div	dword ptr b
	}
}

int32 FS_PC_PASCAL UMulDiv64(uint32 a, uint32 b, uint32 c)
{
	__asm
	{
		mov	eax,dword ptr a
		mul	dword ptr b
		div	dword ptr c
	}
}

int16 FS_PC_PASCAL MulDiv16(int16 a, int16 b, int16 c)
{
	__asm
	{
		mov	ax,word ptr a
		imul	word ptr b
		idiv	word ptr c
	}
}

Fixed FS_PC_PASCAL Mul16Dot16By16Dot16(Fixed a, Fixed b)
{
#ifdef CHECK_FOR_OVERFLOW
	__asm
	{
		mov	eax,dword ptr a
		imul	dword ptr b
		shrd	eax,edx,16

		sar	edx,15
		cmp	edx,0
		je		no_overflow
		cmp	edx,-1
		je		no_overflow

		mov	eax,edx
		cdq
		mov	eax,edx
		xor	eax,0x7fffffff

	no_overflow:
	}
#else
	__asm
	{
		mov	eax,dword ptr a
		imul	dword ptr b
		shrd	eax,edx,16
	}
#endif
}

Fixed FS_PC_PASCAL Div16Dot16By16Dot16(Fixed a, Fixed b)
{
	__asm
	{
		xor	ecx,ecx

		mov	eax,dword ptr b
		cdq
		xor	eax,edx
		sub	eax,edx
		mov	ebx,eax
		xor	ecx,edx

		mov	eax,dword ptr a
		cdq
		xor	eax,edx
		sub	eax,edx
		xor	ecx,edx

		xor	edx,edx
		shld	edx,eax,16
		shl	eax,16

		cmp	ebx,edx
		ja		no_overflow

		mov	eax,0x7fffffff

		jmp	adjust_sign

no_overflow:

		div	ebx

adjust_sign:

		xor	eax,ecx
		sub	eax,ecx
	}
}

F26Dot6 FS_PC_PASCAL Mul26Dot6By26Dot6(F26Dot6 a, F26Dot6 b)
{
	__asm
	{
		mov	eax,dword ptr a
		imul	dword ptr b
		add	eax,1 shl 5
		adc	edx,0
		shrd	eax,edx,6
	}
}

F26Dot6 FS_PC_PASCAL Div26Dot6By26Dot6(F26Dot6 a, F26Dot6 b)
{
#ifdef CHECK_FOR_OVERFLOW
	__asm
	{
		mov	eax,dword ptr a
		cdq

		mov	ecx,dword ptr b
		jecxz	zero_divisor

		shld	edx,eax,6
		shl	eax,6
		idiv	ecx
		jmp	done

	zero_divisor:

		mov	eax,edx
		xor	eax,07fffffffh

	done:
	}
#else
	__asm
	{
		mov	eax,dword ptr a
		cdq
		shld	edx,eax,6
		shl	eax,6
		idiv	dword ptr b
	}
#endif
}

F26Dot6 FS_PC_PASCAL Mul26Dot6By2Dot14(F26Dot6 a, ShortFract b)
{
	__asm
	{
		mov	ax,word ptr b
		cwde
		imul	dword ptr a
		add	eax,1 shl 13
		adc	edx,0
		shrd	eax,edx,14
	}
}

Fract FS_PC_PASCAL Mul2Dot30By2Dot30(Fract a, Fract b)
{
#ifdef CHECK_FOR_OVERFLOW
	__asm
	{
		mov	eax,dword ptr a
		imul	dword ptr b
		shrd	eax,edx,30

		sar	edx,29
		cmp	edx,0
		je		no_overflow
		cmp	edx,-1
		je		no_overflow

		mov	eax,edx
		cdq
		mov	eax,edx
		xor	eax,0x7fffffff

	no_overflow:
	}
#else
	__asm
	{
		mov	eax,dword ptr a
		imul	dword ptr b
		shrd	eax,edx,30
	}
#endif
}

Fract FS_PC_PASCAL Div2Dot30By2Dot30(Fract a, Fract b)
{
	__asm
	{
		mov	eax,dword ptr a
		cdq
		shld	edx,eax,30
		shl	eax,30
		idiv	dword ptr b
	}
}

ShortFract FS_PC_PASCAL Mul2Dot14By2Dot14(ShortFract a, ShortFract b)
{
	__asm
	{
		mov	ax,word ptr a
		imul	word ptr b
		shl	ax,1
		rcl	dx,1
		shl	ax,1
		rcl	dx,1
		shl	ax,1			// round
		adc	dx,0
		mov	ax,dx
	}
}

ShortFract FS_PC_PASCAL Div2Dot14By2Dot14(ShortFract a, ShortFract b)
{
	__asm
	{
		mov	dx,word ptr a
		xor	ax,ax
		sar	dx,1
		rcr	ax,1
		sar	dx,1
		rcr	ax,1
		idiv	word ptr b
	}
}
#endif

#pragma warning(default:4035)

static void CompMul(long src1, long src2, long dst[2])
{
#ifdef FASTMATH
	Mul64(src1, src2, dst);
#else
    int negative = (src1 ^ src2) < 0;
    register unsigned long dsthi, dstlo;

    if (src1 < 0)
        src1 = -src1;
    if (src2 < 0)
        src2 = -src2;
    {   unsigned short src1hi, src1lo;
        register unsigned short src2hi, src2lo;
        register unsigned long temp;
        src1hi = HIWORD(src1);
        src1lo = LOWORD(src1);
        src2hi = HIWORD(src2);
        src2lo = LOWORD(src2);
        temp = (unsigned long)src1hi * src2lo + (unsigned long)src1lo * src2hi;
        dsthi = (unsigned long)src1hi * src2hi + (temp >> 16);
        dstlo = (unsigned long)src1lo * src2lo;
        temp <<= 16;
        dsthi += (dstlo += temp) < temp;
        dst[0] = dsthi;
        dst[1] = dstlo;
    }
    if (negative)
        if (dstlo = -dstlo)
            dsthi = ~dsthi;
        else
            dsthi = -dsthi;
    dst[0] = dsthi;
    dst[1] = dstlo;
#endif
}

static long CompDiv(long src1, long src2[2])
{
#ifdef FASTMATH
	return Div64(src2, src1);
#else
    register unsigned long src2hi = src2[0], src2lo = src2[1];
    int negative = (long)(src2hi ^ src1) < 0;

    if ((long)src2hi < 0)
        if ((src2lo = -src2lo) != 0)
            src2hi = ~src2hi;
        else
            src2hi = -src2hi;
    if (src1 < 0)
        src1 = -src1;
    {   register unsigned long src1hi, src1lo;
        unsigned long result = 0, place = 0x40000000L;

        if ((src1hi = src1) & 1)
            src1lo = 0x80000000L;
        else
            src1lo = 0;

        src1hi >>= 1;
        src2hi += (src2lo += src1hi) < src1hi;      /* round the result */

        if (src2hi > src1hi || src2hi == src1hi && src2lo >= src1lo)
            if (negative)
                return NEGINFINITY;
            else
                return POSINFINITY;
        while (place && src2hi)
        {   src1lo >>= 1;
            if (src1hi & 1)
                src1lo += 0x80000000L;
            src1hi >>= 1;
            if (src1hi < src2hi)
            {   src2hi -= src1hi;
                src2hi -= src1lo > src2lo;
                src2lo -= src1lo;
                result += place;
            }
            else if (src1hi == src2hi && src1lo <= src2lo)
            {   src2hi = 0;
                src2lo -= src1lo;
                result += place;
            }
            place >>= 1;
        }
        if (src2lo >= src1)
            result += src2lo/src1;
        if (negative)
            return -result;
        else
            return result;
    }
#endif
}

long LongMulDiv(long a, long b, long c)
{
#ifdef FASTMATH
	return MulDiv64(a, b, c);
#else
    long temp[2];

    CompMul(a, b, temp);
    return CompDiv(c, temp);
#endif
}

F26Dot6 ShortFracMul(F26Dot6 aDot6, ShortFract b)
{
#ifdef FASTMATH
	return Mul26Dot6By2Dot14(aDot6, b);
#else
    long a = (long)aDot6;
    int negative = (a ^ (long)b) < 0;
    register unsigned long dsthi, dstlo;

    if (a < 0)
        a = -a;
    if (b < 0)
        b = -b;
    {   unsigned short ahi, alo;
        register unsigned long temp;
        ahi = HIWORD(a);
        alo = LOWORD(a);
        temp = (unsigned long)ahi * (unsigned short)(b);
        dsthi = (temp >> 16);
        dstlo = (unsigned long)alo * (unsigned short)(b);
        temp <<= 16;
        dsthi += (dstlo += temp) < temp;
    }
    if (negative)
        if ((dstlo = -dstlo) != 0)
            dsthi = ~dsthi;
        else
            dsthi = -dsthi;

    a = (long)( (dsthi<<18) | (dstlo>>14) ) + (long) !!(dstlo & (1L<<13));
    return (F26Dot6)(a);
#endif
}

ShortFract ShortFracDot(ShortFract a, ShortFract b)
{
#ifdef FASTMATH
	return Mul2Dot14By2Dot14(a, b);
#else
    return ((((long)a * (b)) + (1 << 13)) >> 14);
#endif
}

int32 ShortMulDiv(int32 a, int16 b, int16 c)
{
#ifdef FASTMATH
	return MulDiv64(a, b, c);
#else
    return LongMulDiv(a, b, c);
#endif
}

F26Dot6 Mul26Dot6(F26Dot6 a, F26Dot6 b)
{
#ifdef FASTMATH
	return Mul26Dot6By26Dot6(a, b);
#else
	#define FASTMUL26LIMIT      46340
	#define USHORTMUL(a, b) ((uint32)((uint32)(uint16)(a)*(uint16)(b)))
    int negative = false;
    uint16 al, bl, ah, bh;
    uint32 lowlong, midlong, hilong;

    if ((a <= FASTMUL26LIMIT) && (b <= FASTMUL26LIMIT) && (a >= -FASTMUL26LIMIT) && (b >= -FASTMUL26LIMIT))
	 {
		  return SHORTMUL(a, b) + (1 << 5) >> 6;
	 }

    if (a < 0) { a = -a; negative = true; }
    if (b < 0) { b = -b; negative ^= true; }

    al = LOWORD(a); ah = HIWORD(a);
    bl = LOWORD(b); bh = HIWORD(b);

    midlong = USHORTMUL(al, bh) + USHORTMUL(ah, bl);
    hilong = USHORTMUL(ah, bh) + HIWORD(midlong);
    midlong <<= 16;
    midlong += 1 << 5;
    lowlong = USHORTMUL(al, bl) + midlong;
    hilong += lowlong < midlong;

	 midlong = (lowlong >> 6) | (hilong << (32 - 6));
    return negative ? -midlong : midlong;
#endif
}

F26Dot6 Div26Dot6(F26Dot6 num, F26Dot6 den)
{
#ifdef FASTMATH
	return Div26Dot6By26Dot6(num, den);
#else
	#define FASTDIV26LIMIT	(1L << (31 - 6))
    int negative = false;
    register uint32 hinum, lownum, hiden, lowden, result, place;

    if (den == 0) return (num < 0 ) ? NEGINFINITY : POSINFINITY;

    if ( (num <= FASTDIV26LIMIT) && (num >= -FASTDIV26LIMIT) )          /* fast case */
	 {
		  return ((uint32)num << 6) / (uint32)den;
	 }

    if (num < 0) { num = -num; negative = true; }
    if (den < 0) { den = -den; negative ^= true; }

	 hinum = ((uint32)num >> (32 - 6));
	 lownum = ((uint32)num << 6);
    hiden = den;
    lowden = 0;
    result = 0;
    place = HIBITSET;

    if (hinum >= hiden) return negative ? NEGINFINITY : POSINFINITY;

    while (place)
    {
        lowden >>= 1;
        if (hiden & 1) lowden += HIBITSET;
        hiden >>= 1;
        if (hiden < hinum)
        {
            hinum -= hiden;
            hinum -= lowden > lownum;
            lownum -= lowden;
            result += place;
        }
        else if (hiden == hinum && lowden <= lownum)
        {
            hinum = 0;
            lownum -= lowden;
            result += place;
        }
        place >>= 1;
    }

    return negative ? -result : result;
#endif
}

ShortFract ShortFracDiv(ShortFract num, ShortFract denum)
{
#ifdef FASTMATH
	return Div2Dot14By2Dot14(num, denum);
#else
	return ((long)(num) << 14) / denum;
#endif
}

ShortFract ShortFracMulDiv(ShortFract numA,ShortFract numB,ShortFract denum)
{
#ifdef FASTMATH
    return (ShortFract)MulDiv16(numA, numB, denum);
#else
    return (ShortFract) LongMulDiv ((long) numA,(long) numB, (long)denum);
#endif
}

#ifndef FASTMATH
#define BIT(i)          (1L<<(i))
#define OVFMASK(i)	( ~0L ^ ( ((unsigned long)BIT(i)) - 1 ) )
#define CHKOVF(a,i,v)   (\
        ( ((a)[0] & OVFMASK(i))==0)          ? ( (v)>=0 ?(v) :POSINFINITY) : \
        ( ((a)[0] & OVFMASK(i))==OVFMASK(i)) ? ( (v)<=0 ?(v) :NEGINFINITY) : \
        ( ((a)[0] & BIT(31))                 ? POSINFINITY   :NEGINFINITY)   \
	)
#define GET32(a,i) \
((((a)[0]<<(32-(i))) | ((unsigned long)((a)[1])>>(i))) + !!((a)[1] & BIT((i)-1)))
#endif

Fixed FixMul(Fixed fxA, Fixed fxB)
{
#ifdef FASTMATH
	return Mul16Dot16By16Dot16(fxA, fxB);
#else
    long alCompProd[2];
    Fixed fxProd;

    if  (fxA == 0 || fxB == 0)
        return 0;

    CompMul (fxA, fxB, alCompProd);
    fxProd = GET32 (alCompProd,16);
    return CHKOVF(alCompProd,16,fxProd);
#endif
}

Fixed FixDiv(Fixed fxA, Fixed fxB)
{
#ifdef FASTMATH
	return Div16Dot16By16Dot16(fxA, fxB);
#else
    long alCompProd[2];

    alCompProd[0] = fxA >> 16;
    alCompProd[1] = fxA << 16;
    return CompDiv (fxB, alCompProd);
#endif
}

Fixed FixRatio(short sA, short sB)
{
#ifdef FASTMATH
	return Div16Dot16By16Dot16((long)sA, (long)sB);
#else
    long alCompProd[2];

    alCompProd[0] = ((long)(sA)) >> 16;
    alCompProd[1] = ((long)(sA)) << 16;
    return CompDiv ((long)(sB), alCompProd);
#endif
}

Fract FracMul(Fract frA, Fract frB)
{
#ifdef FASTMATH
	return Mul2Dot30By2Dot30(frA, frB);
#else
    long alCompProd[2];
    Fract frProd;

    if  (frA == 0 || frB == 0)
        return 0;

    CompMul (frA,frB,alCompProd);
    frProd = GET32 (alCompProd,30);
    return CHKOVF(alCompProd,30,frProd);
#endif
}

Fract FracDiv(Fract frA, Fract frB)
{
#ifdef FASTMATH
	return Div2Dot30By2Dot30(frA, frB);
#else
	long alCompProd[2];
	alCompProd[0] = frA >> 2;
	alCompProd[1] = frA << 30;
	return CompDiv (frB, alCompProd);
#endif
}

// Fract FracSqrt (Fract xf)
// Input:  xf           2.30 fixed point value
// Return: sqrt(xf)     2.30 fixed point value

Fract FracSqrt(Fract xf)
{
    Fract b = 0L;
    unsigned long c, d, x = xf;
    
    if (xf < 0) return (NEGINFINITY);

    // The algorithm extracts one bit at a time, starting from the
    // left, and accumulates the square root in b. The algorithm 
    // takes advantage of the fact that non-negative input values
    // range from zero to just under two, and corresponding output
    // ranges from zero to just under sqrt(2). Input is assigned
    // to temporary value x (unsigned) so we can use the sign bit
    // for more precision.
    
    if (x >= 0x40000000L)
    {
        x -= 0x40000000L; 
        b  = 0x40000000L; 
    }

    // This is the main loop. If we had more precision, we could 
    // do everything here, but the lines above perform the first
    // iteration (to align the 2.30 radix properly in b, and to 
    // preserve full precision in x without overflow), and afterward 
    // we do two more iterations.
    
    for (c = 0x10000000L; c; c >>= 1)
    {
        d = b + c;
        if (x >= d)
        {
            x -= d; 
            b += (c<<1); 
        }
        x <<= 1;
    }

    // Iteration to get last significant bit.
    //
    // This code has been reduced beyond recognition, but basically,
    // at this point c == 1L>>1 (phantom bit on right). We would
    // like to shift x and d left 1 bit when we enter this iteration,
    // instead of at the end. That way we could get phantom bit in
    // d back into the word. Unfortunately, that may cause overflow
    // in x. The solution is to break d into b+c, subtract b from x,
    // then shift x left, then subtract c<<1 (1L).

    if (x > (unsigned long)b) // if (x == b) then (x < d).  We want to test (x >= d)
    {
        x -= b;
        x <<= 1;
        x -= 1L;
        b += 1L;	// b += (c<<1)
    }
    else
    {
        x <<= 1;
    }

    // Final iteration is simple, since we don't have to maintain x.
    // We just need to calculate the bit to the right of the least
    // significant bit in b, and use the result to round our final answer.

    return (b + (x > (unsigned long)b));
}
