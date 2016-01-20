// WINGRAPH.C for Windows Win32
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

/* Modified from DOS-VGA.C by Chris Thomas, 7/13/94 
**
** WTWIN does not use graphics like the other WT ports.  WT does not
** drive - it is the slave, Windows is the master.  So WT is called to
** render into a Windows provided bitmap, and then Windows displays this
** bitmap.  Hence the init_graphics(), term_graphics(), and update_screen()
** routines are stubs, and load_palette simply fills in a local RGB array.
** I added some helper routines, called from the Windows part of WTWIN.
** These start with the wt_ prefix.
**
** void wt_set_fb_mem(void *pMem)
**    Saves the bitmap pointer, pMem, locally so that when WT calls 
**		get_framebuffer_memory(), it will return this pointer.   The Windows
**		code calls this with the WinG bitmap address prior to initializing WT.
**
** void wt_get_palette_entry(int i, int *r, int *g, int *b)
**		Gets the specified entry from the local palette.  Called by the 
**	 	Windows code (after it called wt_load_palette).
** 
** wt_load_palette() is just the same old load_palette(), renamed, and with
** the vga palette writes commented out.  And also it uses the WT path,
** instead of a hardcoded path.  
** 
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

#ifdef WIN32
extern char szWTpath[];
#endif

void load_palette(void);

static int nColors;

static int graphics_initialized = 0;
static Pixel *fb_mem;

struct {
   int r,g,b;
} wt_palette[256];

void wt_set_fb_mem(void *pMem)
{
   fb_mem = (Pixel *)pMem;
}

void wt_get_palette_entry(int i, int *r, int *g, int *b)
{
	if (i > nColors-1) {
		*r = *g = *b = 0;
	} else {
	   *r = wt_palette[i].r << 2;		//damn VGA 6-bit DAC...
	   *g = wt_palette[i].g << 2;		//Windows don't want that!
	   *b = wt_palette[i].b << 2;
	}
}

void init_graphics(void)
{
//    wt_load_palette();

//don't want this!  we want the WinG buffer.
//    fb_mem = (Pixel*) wtmalloc(SCREEN_WIDTH * SCREEN_HEIGHT);

    graphics_initialized = 1;
}


int wt_load_palette(void)
{
     FILE *fp;
     int r, g, b;
     int i;

     char szPath[256];
     sprintf(szPath, "%s%s", szWTpath, DEFAULT_PALETTE_FILENAME);
     fp = fopen(szPath, "rb");
     if (fp == NULL)
	  fatal_error("unable to open palette file");
     for (i = 0; i < PALETTE_ENTRIES; i++) {
   	  r = getc(fp);
   	  g = getc(fp);
   	  b = getc(fp);
   	  if (b == EOF)
  	       fatal_error("error reading palette file");
        wt_palette[i].r = r;
        wt_palette[i].g = g;
        wt_palette[i].b = b;
     }
     fclose(fp);
	nColors = PALETTE_ENTRIES;
   return PALETTE_ENTRIES;
}


void end_graphics(void)
{
   graphics_initialized = 0;
}


void update_screen(Framebuffer *fb)
{
//DOS code.  reject. This functionality is now done by the Windows code.
//    memcpy((void*) 0x000A0000, fb->pixels, fb->fb_width * fb->fb_height);
}


Pixel *get_framebuffer_memory(int width, int height)
{
//unchanged from DOS code.
     return fb_mem;
}
