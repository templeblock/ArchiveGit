/* (WT)      SLICE.C */
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
