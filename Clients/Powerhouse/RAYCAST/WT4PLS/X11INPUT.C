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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "wt.h"
#include "error.h"
#include "input.h"
#include "x11graphics.h"


static void add_special(Intent *intent, int special);

static Boolean rotating_cw = False;
static Boolean rotating_ccw = False;
static Boolean moving_forward = False;
static Boolean moving_backward = False;
static Boolean running = False;
static Boolean strafing = False;


void init_input_devices(void)
{
     XSelectInput(display, wtwin,
		  KeyPressMask | KeyReleaseMask | ExposureMask |
		  EnterWindowMask | LeaveWindowMask);
}


void end_input_devices(void)
{
}


Intent *read_input_devices(void)
{
     static Intent intent;
     int count;
     char buffer[20];
     KeySym key;
     XComposeStatus compose;
     XEvent event;
     int n_events;


     n_events = XEventsQueued(display, QueuedAlready);
     intent.force_x = intent.force_y = intent.force_z = 0.0;
     intent.force_rotate = 0.0;
     intent.n_special = 0;

     while (n_events--) {
	  XNextEvent(display, &event);

	  if (event.type == KeyPress) {
	       count = XLookupString(&event.xkey, buffer, 20, &key, &compose);

	       switch (key) {
		  case XK_Left:
		    rotating_ccw = True;
		    break;

		  case XK_Right:
		    rotating_cw = True;
		    break;

		  case XK_Up:
		    moving_forward = True;
		    break;

		  case XK_Down:
		    moving_backward = True;
		    break;

		  case XK_Shift_L:
		  case XK_Shift_R:
		    running = True;
		    break;

		  case XK_Meta_R:
		  case XK_Meta_L:
		  case XK_Alt_R:
		  case XK_Alt_L:
		  case XK_slash:
		    strafing = True;
		    break;

		  case XK_space:
		    add_special(&intent, INTENT_JUMP);
		    break;

		  case XK_q:
		    add_special(&intent, INTENT_END_GAME);
		    break;

		  case XK_1:
		    add_special(&intent, INTENT_ACTION1);
		    break;

		  case XK_2:
		    add_special(&intent, INTENT_ACTION2);
		    break;

		  case XK_3:
		    add_special(&intent, INTENT_ACTION3);
		    break;

		  case XK_4:
		    add_special(&intent, INTENT_ACTION4);
		    break;

		  case XK_5:
		    add_special(&intent, INTENT_ACTION5);
		    break;

		  default:
		    break;
	       }

	  } else if (event.type == KeyRelease) {

	       count = XLookupString(&event.xkey, buffer, 20, &key, &compose);

	       switch (key) {
		  case XK_Left:
		    rotating_ccw = False;
		    break;

		  case XK_Right:
		    rotating_cw = False;
		    break;

		  case XK_Up:
		    moving_forward = False;
		    break;

		  case XK_Down:
		    moving_backward = False;
		    break;

		  case XK_Shift_L:
		  case XK_Shift_R:
		    running = False;
		    break;

		  case XK_Meta_R:
		  case XK_Meta_L:
		  case XK_Alt_R:
		  case XK_Alt_L:
		  case XK_slash:
		    strafing = False;
		    break;

		  default:
		    break;
	       }

	  }
     }
     
     if (rotating_cw) {
	  if (strafing)
	       intent.force_y -= MOVE_FORCE;
	  else
	       intent.force_rotate -= TURN_FORCE;
     }
     if (rotating_ccw) {
	  if (strafing) {
	       intent.force_y += MOVE_FORCE;
	  } else
	       intent.force_rotate += TURN_FORCE;
     }
     if (moving_forward)
	  intent.force_x += MOVE_FORCE;
     if (moving_backward)
	  intent.force_x -= MOVE_FORCE;
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
