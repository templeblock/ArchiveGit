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

#include <stdlib.h>
#include <string.h>
#include "wt.h"
#include "error.h"
#include "wtmem.h"
#include "framebuf.h"
#include "graphics.h"

inline void draw_wall_slice(Pixel *start, Pixel *end, unsigned char *tex_base,
			    fixed tex_y, fixed tex_dy, int fb_width, 
			    int tex_height, int npix)
{
     int count = start - end;
     
     if (npix < 0)
	  return;
     if (tex_height == 64) {
	  tex_y <<= 10;
	  tex_dy <<= 10;
__asm__ volatile("movl %0, %%ebx\n\t"
		 "movl %1, %%ecx\n\t"
		 "movl %2, %%edx\n\t"
		 "movl %3, %%esi\n\t"
		 "movl %4, %%edi\n"
		 "\t1:\n\t"
		 "movl %%edi, %%eax\n\t"
		 "shrl $26, %%eax\n\t"
		 "addl %%edx, %%edi\n\t"
		 "movb (%%ecx,%%eax), %%al\n\t"
		 "movb %%al, (%%ebx,%%esi)\n\t"
		 "subl $320, %%ebx\n\t"
		 "jns  1b\n"
		 :: "m" (count), "m" (tex_base), "m" (tex_dy),
		 "m" (end), "m" (tex_y):
		 "eax", "ebx", "ecx", "edx", "esi", "edi" );
} else {
     tex_y <<= 9;
     tex_dy <<= 9;

__asm__ volatile("movl %0, %%ebx\n\t"
		 "movl %1, %%ecx\n\t"
		 "movl %2, %%edx\n\t"
		 "movl %3, %%esi\n\t"
		 "movl %4, %%edi\n\t"
		 "movl %5, %%eax\n\t"
		 "andl $3, %%eax\n\t"
		 "jz   4f\n\t"
		 "cmpl $1, %%eax\n\t"
		 "je   3f\n\t"
		 "cmpl $2, %%eax\n\t"
		 "je   2f\n\t"
		 "\t1:\n\t"
		 "movl %%edi, %%eax\n\t"
		 "shrl $25, %%eax\n\t"
		 "addl %%edx, %%edi\n\t"
		 "movb (%%ecx,%%eax), %%al\n\t"
		 "movb %%al, (%%ebx,%%esi)\n\t"
		 "subl $320, %%ebx\n"
		 "\t2:\n\t"
		 "movl %%edi, %%eax\n\t"
		 "shrl $25, %%eax\n\t"
		 "addl %%edx, %%edi\n\t"
		 "movb (%%ecx,%%eax), %%al\n\t"
		 "movb %%al, (%%ebx,%%esi)\n\t"
		 "subl $320, %%ebx\n\t"
		 "\t3:\n\t"
		 "movl %%edi, %%eax\n\t"
		 "shrl $25, %%eax\n\t"
		 "addl %%edx, %%edi\n\t"
		 "movb (%%ecx,%%eax), %%al\n\t"
		 "movb %%al, (%%ebx,%%esi)\n\t"
		 "subl $320, %%ebx\n"
		 "\t4:\n\t"
		 "movl %%edi, %%eax\n\t"
		 "shrl $25, %%eax\n\t"
		 "addl %%edx, %%edi\n\t"
		 "movb (%%ecx,%%eax), %%al\n\t"
		 "movb %%al, (%%ebx,%%esi)\n\t"
		 "subl $320, %%ebx\n\t"
		 "jns   1b\n"
		 :: "m" (count), "m" (tex_base), "m" (tex_dy),
		 "m" (end), "m" (tex_y), "m" (npix):
		 "eax", "ebx", "ecx", "edx", "esi", "edi" );
     }
}


inline void draw_floor_slice(Pixel *start, unsigned char *tex,
			     fixed x, fixed y, fixed dx, fixed dy,
			     int tex_width)
{
     if (*start != 255)
	  return;

     if (tex_width == 128) {
	  
	  y <<= 9;
	  dy <<= 9;
	  x <<= 2;
	  dx <<= 2;

__asm__ volatile("movl %0, %%ebx\n\t"
		 "movl %1, %%edi\n\t"
		 "movl %2, %%eax\n\t"
		 "movl %3, %%edx\n\t"
		 "movl %4, %%esi\n\t"
		 "\t1:\n\t"
		 "movl %%edx, %%ecx\n\t"
		 "andl $33554431, %%eax\n\t"
		 "andl $4261412864, %%ecx\n\t"
		 "addl %%eax, %%ecx\n\t"
		 "shrl $18, %%ecx\n\t"
		 "addl %5, %%edx\n\t"
		 "movb (%%ecx,%%edi), %%cl\n\t"
		 "movb %%cl, (%%ebx)\n\t"
		 "incl %%ebx\n\t"
		 "addl %%esi, %%eax\n\t"
		 "cmpb $255, (%%ebx)\n\t"
		 "je   1b\n"
		 :: "m" (start), "m" (tex), "m" (x), "m" (y),
		 "m" (dx), "m" (dy):
		 "eax", "ebx", "ecx", "edx", "esi", "edi" );

     } else {
	  
	  y <<= 10;
	  dy <<= 10;
	  x <<= 4;
	  dx <<= 4;

__asm__ volatile("movl %0, %%ebx\n\t"
		 "movl %1, %%edi\n\t"
		 "movl %2, %%eax\n\t"
		 "movl %3, %%edx\n\t"
		 "movl %4, %%esi\n\t"
		 "\t1:\n\t"
		 "movl %%edx, %%ecx\n\t"
		 "andl $67108863, %%eax\n\t"
		 "andl $4227858432, %%ecx\n\t"
		 "addl %%eax, %%ecx\n\t"
		 "shrl $20, %%ecx\n\t"
		 "addl %5, %%edx\n\t"
		 "movb (%%ecx,%%edi), %%cl\n\t"
		 "movb %%cl, (%%ebx)\n\t"
		 "incl %%ebx\n\t"
		 "addl %%esi, %%eax\n\t"
		 "cmpb $255, (%%ebx)\n\t"
		 "je   1b\n"
		 :: "m" (start), "m" (tex), "m" (x), "m" (y),
		 "m" (dx), "m" (dy):
		 "eax", "ebx", "ecx", "edx", "esi", "edi" );
     }
}
