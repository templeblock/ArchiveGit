/*
$Header: /PM8/TT/FONTMATH.H 1     3/03/99 6:35p Gbeddow $
$Log: /PM8/TT/FONTMATH.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:14:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:16   Fred
// Initial revision.
// 
//    Rev 1.2   01 Jul 1997 09:08:24   Fred
// More changes for PMG 4.0
// 
//    Rev 1.1   18 Apr 1997 12:49:56   Fred
// Changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:20   Jay
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:14:56   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:41:42   FRED
// Initial revision.
// 
//    Rev 1.0   29 Sep 1995 13:37:06   FRED
// Initial revision.
 * 
 *    Rev 1.6   01 Sep 1992 09:24:30   FRED
 * Added PVCS variables.
 * 
*/
/*
        File:           fontmath.h

        Contains:       xxx put contents here xxx

        Written by:     xxx put writers here xxx

        Copyright:      c 1990 by Apple Computer, Inc., all rights reserved.

        Change History (most recent first):

                 <4>    11/27/90        MR              make pascal declaration a macro, conditionalize traps -vs-
                                                                        externs for Fix/Frac math routines. [ph]
                 <3>     11/5/90        MR              Move [U]SHORTMUL into fscdefs.h Rename FixMulDiv to LongMulDiv.
                                                                        [rb]
                 <2>    10/20/90        MR              Add some new math routines (stolen from skia). [rj]
                 <1>     4/11/90        dba             first checked in

        To Do:
*/

#ifndef __FONTMATH_H__
#define __FONTMATH_H__

#define HIBITSET                0x80000000L
#define POSINFINITY             0x7FFFFFFFL
#define NEGINFINITY             0x80000000L
#define HIWORDMASK              0xffff0000L
#define LOWORDMASK              0x0000ffffL
#define FIXONEHALF              0x00008000L
#define ONESHORTFRAC            ((short)(1 << 14))

#define FIXROUND( x )           (int16)((((Fixed) x) + FIXONEHALF) >> 16)
#define ROUNDFIXED( x )         (((x) + FIXONEHALF) & HIWORDMASK)

#define DOT6TOFIX(n)				  ((Fixed) (n) << 10)
#define HIWORD(n)               ((uint16)((uint32)(n) >> 16))
#define LOWORD(n)               ((uint16)(n))
#define LOWSIXBITS              63

#define MAKELONG(a, b)          ((uint32)(((uint16)(a)) | ((uint32)((uint16)(b))) << 16))
#define MAKEWORD(a, b)          ((uint16)(((uint8)(a)) | ((uint16)((uint8)(b))) << 8))

#define HIBYTE(w)               ((uint8)(((uint16)(w) >> 8) & 0xFF))
#define LOBYTE(w)               ((uint8)(w))

typedef short ShortFract;                       // 2.14

#if 0
#ifndef __TOOLUTILS__
FS_MAC_PASCAL Fixed FS_PC_PASCAL FixMul(Fixed,Fixed)   FS_MAC_TRAP(0xA868);
#endif

#ifndef __FIXMATH__
FS_MAC_PASCAL Fixed FS_PC_PASCAL FixDiv(Fixed,Fixed)  FS_MAC_TRAP(0xA84D);
FS_MAC_PASCAL Fract FS_PC_PASCAL FracMul(Fract,Fract) FS_MAC_TRAP(0xA84A);
FS_MAC_PASCAL Fract FS_PC_PASCAL FracDiv(Fract,Fract) FS_MAC_TRAP(0xA84B);
FS_MAC_PASCAL Fract FS_PC_PASCAL FracSqrt(Fract)      FS_MAC_TRAP(0xA849);
#endif
#endif

#if 0
ShortFract      TMP_CONV ShortFracDot (ShortFract x, ShortFract y);
F26Dot6         TMP_CONV ShortFracMul (F26Dot6 x, ShortFract y);
ShortFract      TMP_CONV ShortFracDiv (ShortFract x, ShortFract y);
F26Dot6         TMP_CONV Mul26Dot6 (F26Dot6 a, F26Dot6 b);
F26Dot6         TMP_CONV Div26Dot6 (F26Dot6 num, F26Dot6 den);

#define MulDiv26Dot6(a,b,c) LongMulDiv(a,b,c)
long LongMulDiv(long a, long b, long c);			// (a*b)/c
long ShortMulDiv(long a, short b, short c);		// (a*b)/c
ShortFract ShortFracMulDiv(ShortFract,ShortFract,ShortFract);

/* mh 8/18/92 */
extern Fixed FixMul(Fixed, Fixed);
extern Fixed FixRatio(short, short);
#endif

#define MulDiv26Dot6(a,b,c) LongMulDiv(a,b,c)
long LongMulDiv(long a, long b, long c);
F26Dot6 ShortFracMul(F26Dot6 aDot6, ShortFract b);
ShortFract ShortFracDot(ShortFract a, ShortFract b);
int32 ShortMulDiv(int32 a, int16 b, int16 c);
F26Dot6 Mul26Dot6(F26Dot6 a, F26Dot6 b);
F26Dot6 Div26Dot6(F26Dot6 num, F26Dot6 den);
ShortFract ShortFracDiv(ShortFract num, ShortFract denum);
ShortFract ShortFracMulDiv(ShortFract numA,ShortFract numB,ShortFract denum);
Fixed FixMul(Fixed fxA, Fixed fxB);
Fixed FixDiv(Fixed fxA, Fixed fxB);
Fixed FixRatio(short sA, short sB);
Fract FracMul(Fract frA, Fract frB);
Fract FracDiv(Fract frA, Fract frB);
Fract FracSqrt(Fract xf);
       
extern "C" void FS_PC_PASCAL Add64(int32 a[2], int32 b[2], int32 r[2]);
extern "C" void FS_PC_PASCAL Shr64(int32 a[2], uint32 c, int32 r[2]);
extern "C" void FS_PC_PASCAL Mul64(int32 a, int32 b, int32 r[2]);
extern "C" void FS_PC_PASCAL MulAdd64(int32 a, int32 b, int32 c, int32 r[2]);
extern "C" int32 FS_PC_PASCAL Div64(int32 a[2], int32 b);
extern "C" int32 FS_PC_PASCAL Mod64(int32 a[2], int32 b);
extern "C" int32 FS_PC_PASCAL MulDiv64(int32 a, int32 b, int32 c);
extern "C" void FS_PC_PASCAL UMul64(uint32 a, uint32 b, uint32 r[2]);
extern "C" int32 FS_PC_PASCAL UDiv64(uint32 a[2], uint32 b);
extern "C" int32 FS_PC_PASCAL UMulDiv64(uint32 a, uint32 b, uint32 c);
extern "C" int16 FS_PC_PASCAL MulDiv16(int16 a, int16 b, int16 c);
extern "C" Fixed FS_PC_PASCAL Mul16Dot16By16Dot16(Fixed a, Fixed b);
extern "C" Fixed FS_PC_PASCAL Div16Dot16By16Dot16(Fixed a, Fixed b);
extern "C" F26Dot6 FS_PC_PASCAL Mul26Dot6By26Dot6(F26Dot6 a, F26Dot6 b);
extern "C" F26Dot6 FS_PC_PASCAL Div26Dot6By26Dot6(F26Dot6 a, F26Dot6 b);
extern "C" F26Dot6 FS_PC_PASCAL Mul26Dot6By2Dot14(F26Dot6 a, ShortFract b);
extern "C" Fract FS_PC_PASCAL Mul2Dot30By2Dot30(Fract a, Fract b);
extern "C" Fract FS_PC_PASCAL Div2Dot30By2Dot30(Fract a, Fract b);
extern "C" ShortFract FS_PC_PASCAL Mul2Dot14By2Dot14(ShortFract a, ShortFract b);
extern "C" ShortFract FS_PC_PASCAL Div2Dot14By2Dot14(ShortFract a, ShortFract b);

#endif
