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

/* Modified from linuxvga.c by Petteri Kangaslampi
 * <pekanga@freeport.uwasa.fi>
 *
 * Tested with Watcom C32 9.5 in flat model.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include "wt.h"
#include "error.h"
#include "wtmem.h"
#include "framebuf.h"
#include "graphics.h"


static void load_palette(void);

static int graphics_initialized = 0;
static Pixel *fb_mem;



void init_graphics(void)
{
    union REGS  regs;

    regs.w.ax = 0x0013;
    int386(0x10, &regs, &regs);

    load_palette();
    fb_mem = (Pixel*) wtmalloc(SCREEN_WIDTH * SCREEN_HEIGHT);

    graphics_initialized = 1;
}


void vga_setpalette(int col, int r, int g, int b)
{
    outp(0x03C8, col);
    outp(0x03C9, r);
    outp(0x03C9, g);
    outp(0x03C9, b);
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

     graphics_initialized = 1;

     fclose(fp);
}


void end_graphics(void)
{
    union REGS  regs;

    if (graphics_initialized) {
        regs.w.ax = 0x0003;
        int386(0x10, &regs, &regs);
        graphics_initialized = 0;
    }
}


void update_screen(Framebuffer *fb)
{
    memcpy((void*) 0x000A0000, fb->pixels, fb->fb_width * fb->fb_height);
}


Pixel *get_framebuffer_memory(int width, int height)
{
     return fb_mem;
}
