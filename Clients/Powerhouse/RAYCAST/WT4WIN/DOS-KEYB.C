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

/* Modified from linux-console.c by Petteri Kangaslampi
 * <pekanga@freeport.uwasa.fi>
*/

#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <conio.h>
#include "wt.h"
#include "error.h"
#include "input.h"
#include "dos-keyb.h"


static void add_special(Intent *intent, int special);

static Boolean rotating_cw = False;
static Boolean rotating_ccw = False;
static Boolean moving_forward = False;
static Boolean moving_backward = False;
static Boolean running = False;
static Boolean strafing = False;


unsigned char   kbPressed[128];         /* 1 for each keyboard scancode if
                                           the key is being held down, 0 if
                                           not */
int             kbInitialized = 0;

(__interrupt __far *kbOldInterrupt)();

void __interrupt __far kbInterrupt(void)
{
    unsigned char c;

    c = inp(0x60);
    kbPressed[c & 0x7F] = (c >> 7) ^ 1;

    c = inp(0x61);
    outp(0x61, c | 0x80);
    outp(0x61, c & 0x7F);

    outp(0x20, 0x20);
}


void init_input_devices(void)
{
    atexit(&end_input_devices);         /* make sure that keyboard interrupt
                                           vector will be restored */
    kbOldInterrupt = _dos_getvect(9);
    _dos_setvect(9, kbInterrupt);
    kbInitialized = 1;
    memset(&kbPressed, 0, 128);
}


void end_input_devices(void)
{
    if ( kbInitialized )
        _dos_setvect(9, kbOldInterrupt);
}


Intent *read_input_devices(void)
{
     static Intent intent;

     intent.force_x = intent.force_y = intent.force_z = 0.0;
     intent.force_rotate = 0.0;
     intent.n_special = 0;

	  rotating_ccw = False;
	  rotating_cw = False;
	  moving_forward = False;
	  moving_backward= False;
	  running= False;
	  strafing= False;

    if ( kbPressed[kbLeftArrow] )
        rotating_ccw = True;
    else
        rotating_ccw = False;

    if ( kbPressed[kbRightArrow] )
        rotating_cw = True;
    else
        rotating_cw = False;

    if ( kbPressed[kbUpArrow] )
        moving_forward = True;
    else
        moving_forward = False;

    if ( kbPressed[kbDownArrow] )
        moving_backward = True;
    else
        moving_backward = False;

    if ( kbPressed[kbLeftShift] || kbPressed[kbRightShift] )
        running = True;
    else
        running = False;

    if ( kbPressed[kbSpace] )
        add_special(&intent, INTENT_JUMP);

    if ( kbPressed[kbAlt] )
        strafing = True;
    else
        strafing = False;

    if ( kbPressed[kbEsc] || kbPressed[kbQ] )
        add_special(&intent, INTENT_END_GAME);

     if (rotating_cw) {
	  if (strafing)
	       intent.force_y -= 0.5;
	  else
	       intent.force_rotate -= 0.5;
     }
     if (rotating_ccw) {
	  if (strafing) {
	       intent.force_y += 0.5;
	  } else
	       intent.force_rotate += 0.5;
     }
     if (moving_forward)
	  intent.force_x += 0.5;
     if (moving_backward)
	  intent.force_x -= 0.5;
     if (running) {
	  intent.force_x *= 2.0;
	  intent.force_y *= 2.0;
	  intent.force_z *= 2.0;
	  intent.force_rotate *= 2.0;
     }

     return &intent;
}


void add_special(Intent *intent, int special)
{
     if (intent->n_special < MAX_SPECIAL_INTENTIONS) {
	  intent->special[intent->n_special] = special;
	  intent->n_special++;
     }
}
