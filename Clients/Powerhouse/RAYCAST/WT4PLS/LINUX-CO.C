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

#include <vgakeyboard.h>
#include "wt.h"
#include "error.h"
#include "input.h"


static void add_special(Intent *intent, int special);

static Boolean rotating_cw = False;
static Boolean rotating_ccw = False;
static Boolean moving_forward = False;
static Boolean moving_backward = False;
static Boolean running = False;
static Boolean strafing = False;


void init_input_devices(void)
{
     if (keyboard_init())
	  fatal_error("Unable to open console");
}


void end_input_devices(void)
{
     keyboard_close();
}


Intent *read_input_devices(void)
{
     static Intent intent;


     intent.force_x = intent.force_y = intent.force_z = 0.0;
     intent.force_rotate = 0.0;
     intent.n_special = 0;

     keyboard_update();
     if (keyboard_keypressed(SCANCODE_CURSORBLOCKLEFT))
	  rotating_ccw = True;
     if (keyboard_keypressed(SCANCODE_CURSORBLOCKRIGHT))
	  rotating_cw = True;
     if (keyboard_keypressed(SCANCODE_CURSORBLOCKUP))
	  moving_forward = True;
     if (keyboard_keypressed(SCANCODE_CURSORBLOCKDOWN))
	  moving_backward= True;
     if (keyboard_keypressed(42) || keyboard_keypressed(54))
	  running= True;
     if (keyboard_keypressed(57))
	  add_special(&intent, INTENT_JUMP);
     if (keyboard_keypressed(56) || keyboard_keypressed(100))
	  strafing= True;

     if (!keyboard_keypressed(SCANCODE_CURSORBLOCKLEFT))
	  rotating_ccw = False;
     if (!keyboard_keypressed( SCANCODE_CURSORBLOCKRIGHT))
	  rotating_cw = False;
     if (!keyboard_keypressed(SCANCODE_CURSORBLOCKUP))
	  moving_forward = False;
     if (!keyboard_keypressed(SCANCODE_CURSORBLOCKDOWN))
	  moving_backward= False;
     if (!keyboard_keypressed(42) && !keyboard_keypressed(54))
	  running= False;
     if (!keyboard_keypressed(56) && !keyboard_keypressed(100))
	  strafing= False;
     if (keyboard_keypressed(SCANCODE_Q))
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
