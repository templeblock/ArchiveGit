/* WT.C */
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

/* Frames per second counter added for MS-DOS version. All code specific
 * this counter is inside #ifdef MSDOS statements.
 * Petteri Kangaslampi <pekanga@freeport.uwasa.fi>
*/

/* 7/13/94 Chris Thomas (GCT).  Mucho chango for Windows message loop
** compatibility.   Basically, I first rewrote main(), breaking out groups
** of code.  Toward the end of this file you'll see this new main(), which
** calls these other functions:  wt_init, loops with { wt_render, wt_input },
** then terminates with wt_term.  This main() has been tested with Watcom
** and is known to work fine.  Windows calls the four main functions (wt_init,
** wt_render, wt_input, wt_term), as well as some new helper functions.
** [The code in #ifdef ORIGINAL is the original main(), so you can easily
** see how it has changed.]
**
** Broken-out existing routines:
**
** wt_init     - sets up renderer with the specified world, using a bitmap
**                of the specified size.  Sets up the view.
** wt_input    - alters the view according to input.  does acceleration,etc.
** wt_render   - renders a single frame to the bitmap.
** wt_term     - terminates the renderer, freeing resources.
**
** New functions, unique to WIN32, are:
**
** wt_reinit   - takes a new x,y size, and reinits the renderer to that size.
**               keeps the current world and view.
** wt_new_world - reloads the World with the specified world file.  keeps
**               same view and renderer setup.
** wt_set_path  - sets a local string to the specified path (path of EXE).
** wt_reset_view - resets view to starting parameters.
** wt_get_player_info - returns info about player x,y,angle,height.
**
** That's about it.  Let's talk about the porting philosophy used here.  I
** had two pieces of code: the WT watcom port, and a WinG example.  I knew
** that I might be upgrading the WT code soon, so I wanted to keep the
** two code sets disjoint.  The Windows portion used C++, and WT used C.  I
** could easily call from C++ to C, using extern "C" blocks, but I wasn't
** too sure about calling from C to C++ (including referencing variables).
** That's why there is wt_set_path() - this sets a local string, which the
** rest of WT references, without the rest of WT having to know about the
** Windows C++ code.  Same with wt_reset_view() - I wanted the Windows C++
** code to be able to reset the view without having to #include any WT
** specific information in the C++ code.
** 
** That's the reason for the bootstrapping process of setting the WinG
** bitmap pointer, in wingraph.c.  The C++ code gets the WinG pointer, then
** calls wt_set_fb_mem(), which stores this pointer in a variable in
** wingraph.c.  Then WT is initialized, and it ends up calling 
** get_framebuffer_memory(), which then returns this same pointer.  You'll
** see a fair amount of this cooperative processing (another example is the
** keyboard handling in wtwin.cpp, which sets the keyboard array, which
** wt_input then examines later).
**
** In error.c, you'll see UpdateWT().  This is another example of this
** philosophy.  WT calls this routine which then sends itself a windows
** message (which was registered previously via the error.c wt_setlink()
** routine).  This message is received in the C++ message loop, and is 
** then processed, resulting in the WinG bitmap blit.  This way UpdateWT(),
** in error.c, doesn't have to know about any of the WinG stuff, or the
** bitmap sizes, or any of that stuff.  All error.c needs is some basic
** windows definitions, which it had anyway due to the implementation of
** DebugOut() and DebugMsg().
** 
** UpdateWT() is used to blit partial renderings to the screen.  Cool to
**    check out how the renderer works.  See render.c for these calls.
** DebugOut(...) is a printf() style routine that sents the resulting string
**    to the Windows Output device (dbwin.exe, softice/win, windows 386
**    debugger).  Have to use this for debugging WM_PAINT messages, as 
**    DebugMsg() has the potential to start an infinite loop.  Use a newline
**    at the end of your string!
** DebugMsg(...) is a printf() style routine that displays the resulting
**    string in a Windows message box.  Newline at the end of the string
**    is not required.  NOTE: don't use this in a PAINT message handler!!!
**    Infinite loops may result!
**
** That's it.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "wt.h"
#include "error.h"
#include "fixed.h"
#include "view.h"
#include "texture.h"
#include "table.h"
#include "world.h"
#include "worldfile.h"
#include "framebuf.h"
#include "render.h"
#include "graphics.h"
#include "input.h"

View *view;

#ifdef MSDOS
#include <time.h>
#endif

#ifdef WIN32
char szWTpath[256];
#endif


World *w;
Intent *intent;
fixed v = FIXED_ZERO;
double vx = 0.0, vy = 0.0, va = 0.0;
long frames;


int wt_init(char *pszWorldFile, int width, int height)
{
     FILE *fp;

     if ((fp = fopen(pszWorldFile, "r")) == NULL) {
   	  perror(pszWorldFile);
   	  exit(EXIT_FAILURE);
     }
     w = read_world_file(fp);
     fclose(fp);

     init_graphics();
     init_renderer(width, height);
     init_input_devices();

     /* setup view */
     view = new_view(fixdiv(FIXED_2PI, INT_TO_FIXED(4)));

     view->x = FIXED_ZERO;
     view->y = FIXED_ZERO;
     view->height = FIXED_ONE;
     view->angle = FIXED_ZERO;

     frames = 0;
     return EXIT_SUCCESS;
}


void wt_reinit(int width,int height)
{
	init_renderer(width, height);
	//so far, I think that's all I have to do.
}


int wt_render(void)
{
   render(w, view);
   frames++;
   return frames;
}

int wt_input(void)
{
	int quit = False;
	
	  double sin_facing, cos_facing;

	  intent = read_input_devices();

	  /* This block code is a hack to do acceleration and deceleration. */
	  if (fabs(vx) > fabs(intent->force_x)) {
	       if (vx < 0.0)
		    vx = MIN(vx + 0.1, intent->force_x);
	       else
		    vx = MAX(vx - 0.1, intent->force_x);
	  } else if (fabs(vx) < fabs(intent->force_x)) {
	       vx += intent->force_x / 5.0;
	       if (fabs(vx) > fabs(intent->force_x))
		    vx = intent->force_x;
	  }
	  if (fabs(vy) > fabs(intent->force_y)) {
	       if (vy < 0.0)
		    vy = MIN(vy + 0.1, intent->force_y);
	       else
		    vy = MAX(vy - 0.1, intent->force_y);
	  } else if (fabs(vy) < fabs(intent->force_y)) {
	       vy += intent->force_y / 5.0;
	       if (fabs(vy) > fabs(intent->force_y))
		    vy = intent->force_y;
	  }
	  if (fabs(vy) > fabs(intent->force_y)) {
	       if (vy < 0.0)
		    vy = MIN(vy + 0.1, intent->force_y);
	       else
		    vy = MAX(vy - 0.1, intent->force_y);
	  } else if (fabs(vy) < fabs(intent->force_y)) {
	       vy += intent->force_y / 5.0;
	       if (fabs(vy) > fabs(intent->force_y))
		    vy = intent->force_y;
	  }
	  /* Angular deceleration here is weird and unrealistic, but it feels
	  **   right to me.
	  */
	  if (fabs(va) > fabs(intent->force_rotate))
	       va *= 0.6;
	  else if (fabs(va) < fabs(intent->force_rotate)) {
	       va += intent->force_rotate / 8.0;
	       if (fabs(va) > fabs(intent->force_rotate))
		    va = intent->force_rotate;
	  }
	  view->angle += FLOAT_TO_FIXED(0.3 * va);
	  sin_facing = sin(FIXED_TO_FLOAT(view->angle));
	  cos_facing = cos(FIXED_TO_FLOAT(view->angle));

	  view->x += FLOAT_TO_FIXED(0.8 * vx * cos_facing);
	  view->y += FLOAT_TO_FIXED(0.8 * vx * sin_facing);
	  view->x += FLOAT_TO_FIXED(0.8 * vy * -sin_facing);
	  view->y += FLOAT_TO_FIXED(0.8 * vy * cos_facing);
	  if (view->height > FIXED_ONE)
	       v -= FIXED_ONE / 16;
	  view->height += v;
	  if (view->height < FIXED_ONE) {
	       v = FIXED_ZERO;
	       view->height = FIXED_ONE;
	  }
	  while (intent->n_special--) {
	       if (intent->special[intent->n_special] == INTENT_END_GAME)
		    quit = True;
	       else
		    v = FIXED_ONE / 2;
	  }

   return quit;
}

int wt_term(void)
{
   end_input_devices();
   end_graphics();
   return EXIT_SUCCESS;
}

#ifdef WIN32
int wt_new_world(char *pszWorldFile)
{
   FILE *fp;

   if (w)
      free_world(w);

   if ((fp = fopen(pszWorldFile, "r")) == NULL) {
      perror(pszWorldFile);
      exit(EXIT_FAILURE);
   }
   w = read_world_file(fp);
   fclose(fp);
   return EXIT_SUCCESS;
}

void wt_set_path(char *szModulePath)
{
   strcpy(szWTpath, szModulePath);
}

void wt_reset_input(void)
{
	init_input_devices();		//I know this routine, in wininput.c, does
										//nothing but reset the keyboard array, so
										//let's just use it!
}

void wt_reset_view(void)
{
	if (view) {
		view->x = FIXED_ZERO;
		view->y = FIXED_ZERO;
		view->height = FIXED_ONE;
		view->angle = FIXED_ZERO;
	}
}

void wt_get_player_info(float *pfX,float *pfY,float *pfAngle,float *pfHeight)
{
	//this is a sucky way to do this.  should just fill in a View struct.
	if (view) {
		*pfX      = (float)FIXED_TO_FLOAT(view->x);
		*pfY      = (float)FIXED_TO_FLOAT(view->y);
		*pfAngle  = (float)FIXED_TO_FLOAT(view->angle);
		*pfHeight = (float)FIXED_TO_FLOAT(view->height);
	}
}
#endif


#ifndef WIN32
int main(int argc, char *argv[])
{
     int quit = False;
     
     if (argc != 2) {
   	  fprintf(stderr, "Usage:  wt <world file>\n");
   	  exit(EXIT_FAILURE);
     }

     if (wt_init(argv[1],320,200) == EXIT_FAILURE) {
   	  perror(argv[1]);
   	  exit(EXIT_FAILURE);   
     }

     while (!quit) {
       wt_render();
       quit = wt_input();
     }

     wt_term();
     return EXIT_SUCCESS;
}
#endif

#ifdef ORIGINAL
int main(int argc, char *argv[])
{
     World *w;
     FILE *fp;
     Boolean quit = False;
     Intent *intent;
     fixed v = FIXED_ZERO;
     double vx = 0.0, vy = 0.0, va = 0.0;
     #ifdef MSDOS
     long       frames;
     time_t     starttime, endtime;
     #endif


     if (argc != 2) {
	  fprintf(stderr, "Usage:  wt <world file>\n");
	  exit(EXIT_FAILURE);
     }

     if ((fp = fopen(argv[1], "r")) == NULL) {
	  perror(argv[1]);
	  exit(EXIT_FAILURE);
     }
     w = read_world_file(fp);
     fclose(fp);

     init_graphics();
     init_renderer(SCREEN_WIDTH, SCREEN_HEIGHT);
     init_input_devices();

     /* setup view */
     view = new_view(fixdiv(FIXED_2PI, INT_TO_FIXED(4)));

     view->x = FIXED_ZERO;
     view->y = FIXED_ZERO;
     view->height = FIXED_ONE;
     view->angle = FIXED_ZERO;

     #ifdef MSDOS
     starttime = time(NULL);
     frames = 0;
     #endif

     while (!quit) {
	  double sin_facing, cos_facing;

	  render(w, view);
      #ifdef MSDOS
      frames++;
      #endif
	  intent = read_input_devices();

	  /* This block code is a hack to do acceleration and deceleration. */
	  if (fabs(vx) > fabs(intent->force_x)) {
	       if (vx < 0.0)
		    vx = MIN(vx + 0.1, intent->force_x);
	       else
		    vx = MAX(vx - 0.1, intent->force_x);
	  } else if (fabs(vx) < fabs(intent->force_x)) {
	       vx += intent->force_x / 5.0;
	       if (fabs(vx) > fabs(intent->force_x))
		    vx = intent->force_x;
	  }
	  if (fabs(vy) > fabs(intent->force_y)) {
	       if (vy < 0.0)
		    vy = MIN(vy + 0.1, intent->force_y);
	       else
		    vy = MAX(vy - 0.1, intent->force_y);
	  } else if (fabs(vy) < fabs(intent->force_y)) {
	       vy += intent->force_y / 5.0;
	       if (fabs(vy) > fabs(intent->force_y))
		    vy = intent->force_y;
	  }
	  if (fabs(vy) > fabs(intent->force_y)) {
	       if (vy < 0.0)
		    vy = MIN(vy + 0.1, intent->force_y);
	       else
		    vy = MAX(vy - 0.1, intent->force_y);
	  } else if (fabs(vy) < fabs(intent->force_y)) {
	       vy += intent->force_y / 5.0;
	       if (fabs(vy) > fabs(intent->force_y))
		    vy = intent->force_y;
	  }
	  /* Angular deceleration here is weird and unrealistic, but it feels
	  **   right to me.
	  */
	  if (fabs(va) > fabs(intent->force_rotate))
	       va *= 0.6;
	  else if (fabs(va) < fabs(intent->force_rotate)) {
	       va += intent->force_rotate / 8.0;
	       if (fabs(va) > fabs(intent->force_rotate))
		    va = intent->force_rotate;
	  }
	  view->angle += FLOAT_TO_FIXED(0.3 * va);
	  sin_facing = sin(FIXED_TO_FLOAT(view->angle));
	  cos_facing = cos(FIXED_TO_FLOAT(view->angle));

	  view->x += FLOAT_TO_FIXED(0.8 * vx * cos_facing);
	  view->y += FLOAT_TO_FIXED(0.8 * vx * sin_facing);
	  view->x += FLOAT_TO_FIXED(0.8 * vy * -sin_facing);
	  view->y += FLOAT_TO_FIXED(0.8 * vy * cos_facing);
	  if (view->height > FIXED_ONE)
	       v -= FIXED_ONE / 16;
	  view->height += v;
	  if (view->height < FIXED_ONE) {
	       v = FIXED_ZERO;
	       view->height = FIXED_ONE;
	  }
	  while (intent->n_special--) {
	       if (intent->special[intent->n_special] == INTENT_END_GAME)
		    quit = True;
	       else
		    v = FIXED_ONE / 2;
	  }
     }

     #ifdef MSDOS
     endtime = time(NULL);
     #endif

     end_input_devices();
     end_graphics();

     #ifdef MSDOS
     printf("%li frames in %lu seconds - %.2f frames per second",
        (long) frames, (long) endtime - starttime,
        (float) frames / (float) (endtime-starttime));
     #endif

     return EXIT_SUCCESS;
}
#endif


