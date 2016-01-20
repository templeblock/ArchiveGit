/* (WT)  FIXED.H */
/*
**  wt -- a 3d game engine
**
**  Copyright (C) 1994 by Chris Laurel
**  email:  claurel@mr.net
**  snail mail:  Chris Laurel, 5700 W Lake St #208,  St. Louis Park, MN  55416
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* Watcom C inline assembler versions of some functions added
 * by Petteri Kangaslampi <pekanga@freeport.uwasa.fi>
*/

/* symantec C inline assembler versions of the aforementioned Watcom stuff.
 * by Chris Thomas, 7/13/94.  Compatible with Microsoft compiler as well.
 * Definitely faster, but not a whole lot faster, than the vanilla routines.
*/

#ifndef FIXED_H_
#define FIXED_H_

#include "limits.h"

/* fixed point conversions */
#define INT_TO_FIXED(i) ((i) << 16)
#define FIXED_TO_INT(f) ((f) >> 16)
#define FIXED_TO_FLOAT(f) (((double) (f)) / 65536.0)
#define FLOAT_TO_FIXED(f) ((fixed) (f * 65536.0))

/* functions */
#define FIXED_ABS(f) ((f) < 0 ? -(f) : (f))
#define FIXED_TRUNC(f) ((f) & 0xffff0000)
#define FIXED_SIGN(f) ((unsigned int) (f) >> 31)
#define FIXED_PRODUCT_SIGN(f, g) ((unsigned int) ((f) ^ (g)) >> 31)
#define FIXED_HALF(f) ((f) / 2)
#define FIXED_DOUBLE(f) ((f) << 1)

/* perform integer scaling of a fixed point number */
#define FIXED_SCALE(f, i) ((f) * (i))


/* fixed point constants */
#define FIXED_ZERO     (INT_TO_FIXED(0))
#define FIXED_ONE      (INT_TO_FIXED(1))
#define FIXED_ONE_HALF (FIXED_HALF(FIXED_ONE))
#define FIXED_PI       (FLOAT_TO_FIXED(3.14159265))
#define FIXED_2PI      (FLOAT_TO_FIXED(6.28318531))
#define FIXED_HALF_PI  (FLOAT_TO_FIXED(1.57079633))
#define FIXED_MIN      LONG_MIN
#define FIXED_MAX      LONG_MAX

/* we need this for kludges to avoid fixed point division overflow */
#define FIXED_EPSILON  ((fixed) 0x100)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef long fixed;

extern fixed fix_sqrt(fixed);


#if defined(__GNUC__) && defined(ARCH_i86)
static inline fixed fixmul(fixed r1, fixed r2)
{
     fixed result;

     __asm__ ("imull %2\n\t"
	      "shrd  $16, %%edx, %%eax\n\t"
	      :"=a" (result):"a" (r1), "d" (r2):"eax", "edx");

     return result;
}


static inline fixed fixmul2_30(fixed r1, fixed r2)
{
     fixed result;

     __asm__ ("imull %2\n\t"
	      "shrd  $30, %%edx, %%eax\n\t"
	      :"=a" (result):"a" (r1), "d" (r2):"eax", "edx");

     return result;
}


static inline fixed fixdiv(fixed dividend, fixed divisor)
{
     fixed result;

     if (divisor == FIXED_ZERO)
	  fatal_error("divide by zero");

     /* no checking for overflow is done yet */
     __asm__("movl %%edx, %%eax\n\t"
	     "sar  $16, %%edx\n\t"
	     "shl  $16, %%eax\n\t"
	     "idivl %%ecx, %%eax\n\t"
	     :"=a" (result):"d" (dividend), "c" (divisor):"eax", "ecx", "edx");

     return result;
}

#else

#ifdef __WATCOMC__

fixed fixmul(fixed r1, fixed r2);
#pragma aux fixmul = \
    "   imul    edx"            \
    "   shrd    eax,edx,16"     \
    parm [eax] [edx] \
    modify exact [eax edx] \
    value [eax];

fixed fixmul2_30(fixed r1, fixed r2);
#pragma aux fixmul2_30 = \
    "   imul    edx"            \
    "   shrd    eax,edx,30 "    \
    parm [eax] [edx] \
    modify exact [eax edx] \
    value [eax];

fixed fixdiv(fixed dividend, fixed divisor);
#pragma aux fixdiv = \
    "   mov     edx,eax"        \
    "   sar     edx,16"         \
    "   shl     eax,16"         \
    "   idiv    ecx"            \
    parm [eax] [ecx] \
    modify exact [eax edx] \
    value [eax];

#else

#ifdef WIN32

//That's Intel Win32, not Risc!
//Symantec and Microsoft compatible.  Imagine that!  Routines are in fixed.c

fixed fixmul(fixed r1, fixed r2);
fixed fixmul2_30(fixed r1, fixed r2);
fixed fixdiv(fixed dividend, fixed divisor);

#else

#define fixmul(a, b) ((fixed) (((double) (a) * (double) (b)) / 65536.0))
#define fixmul2_30(a, b) ((fixed) (((double) (a) * (double) (b)) / \
				   1073741824.0))
#define fixdiv(a, b) ((fixed) (((double) (a) / (double) (b)) * 65536.0))

#endif
#endif
#endif

#endif /* _FIXED_H_ */
