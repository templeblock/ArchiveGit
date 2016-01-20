/* (WT)      FIXED.C */
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

#include <math.h>
#include <stdio.h>
#include "error.h"
#include "fixed.h"


fixed fix_sqrt(fixed f)
{
     double d;

     d = FIXED_TO_FLOAT(f);
     return FLOAT_TO_FIXED(sqrt(d));
}


#ifdef WIN32

//That's Intel Win32, not Risc!
//Symantec and Microsoft compatible.  Imagine that!  Routines are in fixed.c

//Do I have to push/pop registers (ie, ecx, edx)?  I guess I don't as I 
//haven't had problems yet...

//compilers won't take inline keyword, for some reason.

/*inline*/ fixed fixmul(fixed r1, fixed r2)
{
	__asm {
		mov 	eax,r1
		mov	edx,r2
		imul	edx
		shrd  eax,edx,16
	}
}

/*inline*/ fixed fixmul2_30(fixed r1, fixed r2)
{
	__asm {
		mov 	eax,r1
		mov	edx,r2
		imul	edx
		shrd  eax,edx,30
	}
}

/*inline*/ fixed fixdiv(fixed dividend, fixed divisor)
{
	__asm {
		mov 	eax,dividend
		mov	ecx,divisor
		mov   edx,eax
		sar   edx,16
		shl   eax,16
		idiv	ecx
	}
}
#endif

