/* (WT)      SLICEWIN.C */
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

// 7/13/94 Chris Thomas (GCT), Windows Port.
// Basically identical to original slice.c.  Define USE_ASM_ROUTINES
// to use the ASM code.  I found Symantec 6.1 to have higher frame rates
// using this ASM code.  But I found Microsoft MSVC32s to have higher
// frame rates using the C code.   Even more importantly, MSVC frame rates
// were about 15% higher than Symantecs, so I chose to comment out the ASM
// routines.
// Note: __asm blocks are Symantec and Microsoft compatible.

//#define USE_ASM_ROUTINES

#ifndef USE_ASM_ROUTINES
inline void draw_wall_slice(Pixel *start, Pixel *end, unsigned char *tex_base,
			    fixed tex_y, fixed tex_dy, int fb_width, 
			    int tex_height, int npix)
{
     unsigned mask = tex_height - 1;

     while (start >= end) {
		  *start = tex_base[FIXED_TO_INT(tex_y) & mask];
		  tex_y += tex_dy;
		  start -= fb_width;
     }
}
#endif


inline void draw_floor_slice(Pixel *start, unsigned char *tex,
			     fixed x, fixed y, fixed dx, fixed dy,
			     int tex_width)
{
	fixed tx,ty;

   if (*start != 255)
	  return;

   if (tex_width == 64) {
	  while (*start == 255) {
//	       x &= 0x3fffff;
//	       y &= 0x3fffff;
//	       *start++ = tex[((y >> 16) << 6) + (x >> 16)];
			tx = x >> 16;
			ty = y >> 16;
			tx &= 0x3f;
			ty &= 0x3f;
			tx <<= 6;
	       *start++ = tex[ty + tx];
	       x += dx;
	       y += dy;
	  }
     } else {
	  while (*start == 255) {
//	       x &= 0x7fffff;
//	       y &= 0x7fffff;
//	       *start++ = tex[((y >> 16) << 7) + (x >> 16)];
			tx = x >> 16;
			ty = y >> 16;
			tx &= 0x7f;
			ty &= 0x7f;
			tx <<= 7;
	       *start++ = tex[ty + tx];
	       x += dx;
	       y += dy;
	  }
     }
}



#ifdef USE_ASM_ROUTINES

inline void draw_wall_slice(Pixel *start, Pixel *end, unsigned char *tex_base,
			    fixed tex_y, fixed tex_dy, int fb_width, 
			    int tex_height, int npix)
{
//	int x = (start - end)/fb_width;
//x++;
//x--;

__asm {
		mov	ebx,start
		mov	esi,end
		
		cmp	ebx,esi
		jb		dws_exit

		mov	edi,tex_base
		mov	edx,tex_height
		dec	edx					;mask = tex_height - 1

		mov	eax,tex_y

dws_loop:
		mov	ecx,eax
		shr	ecx,16
		and	ecx,edx				; & mask
		mov	cl,[edi][ecx]		; cl = [tex_base][(tex_y>>16)&mask]
		mov	[ebx],cl				; *start = cl
		add	eax,tex_dy
		sub	ebx,fb_width
		cmp	ebx,esi
		jae	dws_loop

dws_exit:
}
}
#endif
