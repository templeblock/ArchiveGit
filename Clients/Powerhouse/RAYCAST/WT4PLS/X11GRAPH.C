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


#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/ipc.h>
#ifndef NO_XSHM
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif

#include "wt.h"
#include "error.h"
#include "wtmem.h"
#include "framebuf.h"
#include "graphics.h"

#ifdef BLOCKY
#define REAL_SCREEN_WIDTH  (SCREEN_WIDTH*BLOCKY)
#define REAL_SCREEN_HEIGHT (SCREEN_HEIGHT*BLOCKY)
#endif

static void create_window(void);
static void load_palette(void);
static void map_shared_mem(void);

Display *display;
int screen;
Window wtwin;
static GC gc;
#ifndef NO_XSHM
static XShmSegmentInfo xshminfo;
#endif
static XImage *image;
static unsigned char *image_mem;
#ifdef BLOCKY
static unsigned char *real_image_mem;
#endif
static int graphics_initialized = 0;

XColor palette[PALETTE_ENTRIES];

void init_graphics(void)
{
     if (graphics_initialized == 1)
	  return;

     image_mem = wtmalloc(SCREEN_HEIGHT * SCREEN_WIDTH);
     create_window();
     load_palette();
     map_shared_mem();

     graphics_initialized = 1;
}


static void create_window(void)
{
     XGCValues gc_values;

     display = XOpenDisplay(NULL);
     if (display == NULL)
	  fatal_error("Unable to open display.\n");

     screen = DefaultScreen(display);

#ifdef BLOCKY
     wtwin = XCreateSimpleWindow(display, DefaultRootWindow(display),
				 0, 0, REAL_SCREEN_WIDTH, REAL_SCREEN_HEIGHT,
				 2, BlackPixel(display, screen),
				 BlackPixel(display, screen));
#else
     wtwin = XCreateSimpleWindow(display, DefaultRootWindow(display),
				 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
				 2, BlackPixel(display, screen),
				 BlackPixel(display, screen));
#endif

     XStoreName(display, wtwin, "xwt");
     XMapWindow(display, wtwin);
     XSelectInput(display, wtwin,
		  ButtonPressMask | ButtonReleaseMask |
		  ButtonMotionMask | KeyPressMask | ExposureMask);
     gc_values.graphics_exposures = False;
     gc = XCreateGC(display, wtwin, GCGraphicsExposures, &gc_values);
}


static void load_palette(void)
{
     FILE *fp;
     int i, r, g, b;
     Colormap wtcolormap;
     XVisualInfo *visual_info_list; 
     XVisualInfo desired_visual_template;
     int no_visual_matched;
   

     fp = fopen(DEFAULT_PALETTE_FILE, "rb");
     if (fp == NULL)
	  fatal_error("unable to open palette file");

     for (i = 0; i < PALETTE_ENTRIES; i++) {
	  r = getc(fp);
	  g = getc(fp);
	  b = getc(fp);
	  if (b == EOF)
	       fatal_error("error reading palette file");

	  palette[i].pixel = i;
	  palette[i].red = r << 10;
	  palette[i].green = g << 10;
	  palette[i].blue = b << 10;
	  palette[i].flags = DoRed | DoGreen | DoBlue;
     }
     
     fclose(fp);

     desired_visual_template.screen = screen;
     desired_visual_template.class = PseudoColor;
     visual_info_list = XGetVisualInfo(display, 
				       VisualScreenMask | VisualClassMask,
				       &desired_visual_template,
				       &no_visual_matched);
     if (no_visual_matched == 0)
	  fatal_error("Unable to get a PseudoColor Visual.\n");

     wtcolormap = XCreateColormap(display, DefaultRootWindow(display),
				  visual_info_list[0].visual, AllocAll);
     XStoreColors(display, wtcolormap, palette, PALETTE_ENTRIES);
     XSetWindowColormap(display, wtwin, wtcolormap);
}


#ifdef NO_XSHM
void map_shared_mem(void)
{
     int depth;
     XWindowAttributes win_attributes;

     XGetWindowAttributes(display, DefaultRootWindow(display),
                          &win_attributes);
     depth = win_attributes.depth;
     image = XCreateImage(display, DefaultVisual(display, screen), depth,
                             ZPixmap, 0,image_mem,SCREEN_WIDTH,
                             SCREEN_HEIGHT,8,0);
}


void update_screen(Framebuffer *fb)
{
     XPutImage(display, wtwin, gc, image, 0, 0, 0, 0,
                  fb->fb_width, fb->fb_height);
     XSync(display, 0);
}


void end_graphics(void)
{
   graphics_initialized = 0;
}
#else
static void map_shared_mem(void)
{
     int depth;
     XWindowAttributes win_attributes;

     XGetWindowAttributes(display, DefaultRootWindow(display),
			  &win_attributes);
     depth = win_attributes.depth;
#ifdef BLOCKY
     image = XShmCreateImage(display, DefaultVisual(display, screen), depth,
				ZPixmap, real_image_mem, &xshminfo,
				REAL_SCREEN_WIDTH, REAL_SCREEN_HEIGHT);
	printf("Creating %d x %d blocky screen\n", REAL_SCREEN_WIDTH, REAL_SCREEN_HEIGHT);
#else
     image = XShmCreateImage(display, DefaultVisual(display, screen), depth,
				ZPixmap, image_mem, &xshminfo,
				SCREEN_WIDTH, SCREEN_HEIGHT);
#endif
     xshminfo.shmid = shmget(IPC_PRIVATE,
			    image->bytes_per_line * image->height,
			    IPC_CREAT | 0777);
     if (xshminfo.shmid < 0)
	  fatal_error("shmget failed");
     
     image->data = (unsigned char *) shmat(xshminfo.shmid, 0, 0);
     if (image->data == ((char *) -1))
	  fatal_error("shmat failed");
#ifdef BLOCKY
     real_image_mem = image->data;
     image_mem = (char *)calloc(SCREEN_WIDTH*SCREEN_HEIGHT, 1);
#else
     image_mem = image->data;
#endif
     xshminfo.shmaddr = image->data;
     xshminfo.readOnly = False;
     if (!XShmAttach(display, &xshminfo))
	  fatal_error("Unable to map shared memory.\n");
}


void update_screen(Framebuffer *fb)
{
#ifndef BLOCKY
     XShmPutImage(display, wtwin, gc, image, 0, 0, 0, 0,
		  fb->fb_width, fb->fb_height, 0);
#else
     expand(image_mem, real_image_mem);
     XShmPutImage(display, wtwin, gc, image, 0, 0, 0, 0,
		  REAL_SCREEN_WIDTH, REAL_SCREEN_HEIGHT, 0);
#endif
     XSync(display, 0); 
}


void end_graphics(void)
{
   XShmDetach(display, &xshminfo);
   XDestroyImage(image);
   shmdt(xshminfo.shmaddr);
   shmctl(xshminfo.shmid, IPC_RMID, 0);
   graphics_initialized = 0;
}
#endif


Pixel *get_framebuffer_memory(int width, int height)
{
     return (Pixel *) image_mem;
}
