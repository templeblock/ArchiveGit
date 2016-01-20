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

/*
 * Modified:
 * Handle virtual screen graphics with vgagl. This allows for
 * transparent page flipping in 256 color modes, including SVGA resolutions.
 * Also supports planar (mode X-like) 256 color modes, and even 16-color
 * modes.
 *
 * Triple-buffering is automatically used by vgagl if enough video memory
 * is available.
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>
#include "wt.h"
#include "error.h"
#include "wtmem.h"
#include "framebuf.h"
#include "graphics.h"


static void load_palette(void);

static int graphics_initialized = 0;
static Pixel *fb_mem;

GraphicsContext physicalscreen;


void init_graphics(void)
{
    int vgamode;


    vga_init();		/* This gives up root priviledges. */
    vgamode = G320x200x256;
    vga_setmode(vgamode);
    gl_setcontextvga(vgamode);
    gl_getcontext(&physicalscreen);
    load_palette();
    /* Create the virtual screen framebuffer. */
    gl_setcontextvgavirtual(vgamode);
    fb_mem = VBUF;	/* VBUF points to the virtual screen framebuffer. */

    /* The following function returns 3 if triple-buffering is used,
     * 2 for double-buffering, 0 otherwise. */
    gl_enablepageflipping(&physicalscreen);

    graphics_initialized = 1;
}


void load_palette(void)
{
     FILE *fp;
     int r, g, b;
     int i;

     fp = fopen(DEFAULT_PALETTE_FILE, "rb");
     if (fp == NULL)
	  fatal_error("unable to open palette file");
     for (i = 0; i < PALETTE_ENTRIES; i++) {
	  r = getc(fp);
	  g = getc(fp);
	  b = getc(fp);
	  if (b == EOF)
	       fatal_error("error reading palette file");
	  vga_setpalette(i, r, g, b);
     }

     fclose(fp);
}


void end_graphics(void)
{
     if (graphics_initialized) {
	  keyboard_close();	/* This is required! */
	  vga_setmode(TEXT);
	  graphics_initialized = 0;
     }
}


void update_screen(Framebuffer *fb)
{
    /* Page flipping is done transparently. */
    gl_copyscreen(&physicalscreen);
}


Pixel *get_framebuffer_memory(int width, int height)
{
     return fb_mem;
}
