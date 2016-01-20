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


Framebuffer *new_framebuffer(int width, int height)
{
     Framebuffer *fb;

     fb = wtmalloc(sizeof(Framebuffer));
     fb->fb_width = width;
     fb->fb_height = height;
     fb->pixels = get_framebuffer_memory(width, height);
     
     return fb;
}

#ifdef BLOCKY

void clear_framebuffer(Framebuffer *fb)
{
	static int done=0;
	static void *blank;
     int i;
     Pixel *p;

	if (!done)
	{
	     blank = (void *)calloc(fb->fb_height * fb->fb_width, 1);
	     memset(blank, (Pixel) 255, fb->fb_height * fb->fb_width);

	     p = blank + fb->fb_width - 1;
	     for (i = 0; i < fb->fb_height; i++, p += fb->fb_width)
		  *p = 0;
		done++;
	}
	bcopy(blank, fb->pixels, fb->fb_height*fb->fb_width);
}

#else

void clear_framebuffer(Framebuffer *fb)
{
     int i;
     Pixel *p;

     memset(fb->pixels, (Pixel) 255, fb->fb_height * fb->fb_width);

     p = fb->pixels + fb->fb_width - 1;
     for (i = 0; i < fb->fb_height; i++, p += fb->fb_width)
	  *p = 0;
}

#endif
