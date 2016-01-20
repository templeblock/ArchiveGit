/*
$Header: /PM8/TT/FSCDEFS.H 1     3/03/99 6:35p Gbeddow $
$Log: /PM8/TT/FSCDEFS.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:14:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:16   Fred
// Initial revision.
// 
//    Rev 1.2   01 Jul 1997 09:08:26   Fred
// More changes for PMG 4.0
// 
//    Rev 1.1   18 Apr 1997 12:49:58   Fred
// Changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:24   Jay
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:15:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:41:46   FRED
// Initial revision.
// 
//    Rev 1.1   04 Oct 1995 10:59:24   FRED
// 32 bit TrueType engine
// 
//    Rev 1.0   29 Sep 1995 13:37:06   FRED
// Initial revision.
 * 
 *    Rev 1.6   29 Aug 1992 22:51:14   MACDUFF
 * Use #ifdef MLS_PRECISION, thereby forcing FNT.C to use fixed-precision.
 * 
 *    Rev 1.5   19 Aug 1992 15:46:54   MACDUFF
 * Declared variable-precision globals.
 * 
 *    Rev 1.4   18 Aug 1992 18:50:10   MACDUFF
 * Support for global _Precision
 * 
 *    Rev 1.3   04 Aug 1992 12:54:30   ADAM
 * No change.
 * 
 *    Rev 1.2   13 Jul 1992 18:39:16   ADAM
 * 
 *    Rev 1.1   10 Jul 1992 11:02:20   ADAM
 * PVCS keywords
*/

/*
    File:       fscdefs.h

    Contains:   xxx put contents here (or delete the whole line) xxx

    Written by: xxx put name of writer here (or delete the whole line) xxx

    Copyright:  c 1988-1990 by Apple Computer, Inc., all rights reserved.

    Change History (most recent first):

         <3>    11/27/90    MR      Add #define for PASCAL. [ph]
         <2>     11/5/90    MR      Move USHORTMUL from fontmath.h, add Debug definition [rb]
         <7>     7/18/90    MR      Add byte swapping macros for INTEL, moved rounding macros from
                                    fnt.h to here
         <6>     7/14/90    MR      changed defines to typedefs for int[8,16,32] and others
         <5>     7/13/90    MR      Declared ReleaseSFNTFunc and GetSFNTFunc
         <4>      5/3/90    RB      cant remember any changes
         <3>     3/20/90    CL      type changes for Microsoft
         <2>     2/27/90    CL      getting bbs headers
       <3.0>     8/28/89    sjk     Cleanup and one transformation bugfix
       <2.2>     8/14/89    sjk     1 point contours now OK
       <2.1>      8/8/89    sjk     Improved encryption handling
       <2.0>      8/2/89    sjk     Just fixed EASE comment
       <1.5>      8/1/89    sjk     Added composites and encryption. Plus some enhancements.
       <1.4>     6/13/89    SJK     Comment
       <1.3>      6/2/89    CEL     16.16 scaling of metrics, minimum recommended ppem, point size 0
                                    bug, correct transformed integralized ppem behavior, pretty much
                                    so
       <1.2>     5/26/89    CEL     EASE messed up on "c" comments
      <,1.1>     5/26/89    CEL     Integrated the new Font Scaler 1.0 into Spline Fonts
       <1.0>     5/25/89    CEL     Integrated 1.0 Font scaler into Bass code for the first time.

    To Do:
*/

#ifndef __FSCDEFS_H__
#define __FSCDEFS_H__

#include "fsconfig.h"

#define true 1
#define false 0

#define ONEFIX      ( 1L << 16 )
#define ONEFRAC     ( 1L << 30 )
#define ONEHALFFIX  0x8000L
#define ONEVECSHIFT 16
#define HALFVECDIV  (1L << (ONEVECSHIFT-1))

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;

typedef short FUnit;
typedef unsigned short uFUnit;

typedef long Fixed;
typedef long Fract;

#ifndef F26Dot6
#define F26Dot6 int32
#endif

#ifndef boolean
#define boolean int
#endif

#ifndef CONST
#define CONST const
#endif

#ifndef TMP_CONV
#define TMP_CONV
#endif 

#ifndef FS_MAC_PASCAL
#define FS_MAC_PASCAL
#endif

#ifndef FS_PC_PASCAL
#define FS_PC_PASCAL
#endif 

#ifndef FS_MAC_TRAP
#define FS_MAC_TRAP(a)
#endif 

typedef struct {
    Fixed       transform[3][3];
} transMatrix;

typedef struct {
    Fixed       x, y;
} vectorType;

/* Private Data Types */
typedef struct {
    int32 xMin;
    int32 yMin;
    int32 xMax;
    int32 yMax;
} BBOX;

#ifndef SHORTMUL
#define SHORTMUL(a,b)   (int32)((int32)(a) * (b))
#endif

#ifndef SHORTDIV
#define SHORTDIV(a,b)   (int32)((int32)(a) / (b))
#endif

#ifdef FSCFG_BIG_ENDIAN	/* target byte order matches Motorola 68000 */
 #define SWAPL(a)        (a)
 #define SWAPW(a)        (a)
 #define SWAPWINC(a)     (*(a)++)
#else
 /* Portable code to extract a short or a long from a 2- or 4-byte buffer */
 /* which was encoded using Motorola 68000 (TrueType "native") byte order. */
 #define FS_2BYTE(p)  ( ((unsigned short)((p)[0]) << 8) |  (p)[1])
 #define FS_4BYTE(p)  ( FS_2BYTE((p)+2) | ( (FS_2BYTE(p)+0L) << 16) )
#endif

#ifndef SWAPW
#define SWAPW(a)        ((short) FS_2BYTE( (unsigned char *)(&a) ))
#endif

#ifndef SWAPL
#define SWAPL(a)        ((long) FS_4BYTE( (unsigned char *)(&a) ))
#endif

#ifndef SWAPWINC
#define SWAPWINC(a)     SWAPW(*(a)); a++	/* Do NOT parenthesize! */
#endif

#ifndef LoopCount
#define LoopCount int16      /* short gives us a Motorola DBF */
#endif

#ifndef ArrayIndex
#define ArrayIndex int       /* avoids EXT.L on Motorola */
#endif

typedef void (*voidFunc)();
typedef void* voidPtr;

#ifdef WIN32
typedef void (__stdcall *ReleaseSFNTFunc)(voidPtr);
typedef void* (__stdcall *GetSFNTFunc)(long, long, long);
#else
typedef void (pascal *ReleaseSFNTFunc)(voidPtr);
typedef void* (pascal *GetSFNTFunc)(long, long, long);
#endif


#ifndef MEMSET
#define MEMSET(dst, value, size)    memset(dst, value, (size_t)(size))
#define FS_NEED_STRING_DOT_H
#endif

#ifndef MEMCPY
#define MEMCPY(dst, src, size)      memcpy(dst, src, (size_t)(size))
#ifndef FS_NEED_STRING_DOT_H
#define FS_NEED_STRING_DOT_H
#endif
#endif

#ifdef FS_NEED_STRING_DOT_H
#undef FS_NEED_STRING_DOT_H
#include <string.h>
#endif

#ifndef FS_UNUSED_PARAMETER
#define FS_UNUSED_PARAMETER(a)		(a)	/* Silence some warnings */
#endif

#define _PRECISION			6
#define _INTBITS				(16 - _PRECISION)
#define _INTBITSM1			(_INTBITS - 1)
#define _ERRSHIFT				(_PRECISION - 2)
#define _ONE					(1 << _PRECISION)
#define _HALF					(1 << (_PRECISION - 1))
#define _ERRDIV				(1 << _ERRSHIFT)
#define _ONEM1					(_ONE - 1)
#define _HALFM1				(_HALF - 1)
#define _INTPART				(~_ONEM1)

#endif
