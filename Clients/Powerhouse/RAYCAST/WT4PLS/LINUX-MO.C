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

/* Linux mouse code by Pekka Pietik{inen <pp@lyseo.otol.fi>, tweaked
**   by Chris Laurel
*/

#include <math.h>
#include <vgamouse.h>
#include "wt.h"
#include "error.h"
#include "input.h"


#define MOUSE_XRANGE 320
#define MOUSE_YRANGE 200


static void add_special(Intent *intent, int special);

/*
static Boolean rotating_cw = False;
static Boolean rotating_ccw = False;
static Boolean moving_forward = False;
static Boolean moving_backward = False;
*/
static Boolean running = False;
static Boolean strafing = False;


void init_input_devices(void)
{
     mouse_init("/dev/mouse", MOUSE_MICROSOFT, MOUSE_DEFAULTSAMPLERATE);
     mouse_setxrange(0, MOUSE_XRANGE - 1);
     mouse_setyrange(0, MOUSE_YRANGE - 1);
     mouse_setwrap(MOUSE_NOWRAP);
     mouse_setposition(MOUSE_XRANGE / 2, MOUSE_YRANGE / 2);
}


void end_input_devices(void)
{
     mouse_close();
}


Intent *read_input_devices(void)
{
     static Intent intent;
     int mouse_x, mouse_y, mouse_button;


     intent.n_special = 0;
     intent.force_y = 0.0;
     intent.force_x = 0.0;
     intent.force_rotate = 0.0;

     mouse_update();
     mouse_x = mouse_getx();
     mouse_y = mouse_gety();
     mouse_button = mouse_getbutton();
     if (mouse_button & MOUSE_LEFTBUTTON)
	  add_special(&intent, INTENT_JUMP);
     if (mouse_button & MOUSE_RIGHTBUTTON)
	  strafing=True;
     else
	  strafing=False;  

     if (strafing)
	  intent.force_y += (double) (2 * (MOUSE_XRANGE / 2 - mouse_x)) /
	       (double) MOUSE_XRANGE;
     else
	  intent.force_rotate += (double) (2 * (MOUSE_XRANGE / 2 - mouse_x)) /
	       (double) MOUSE_XRANGE;
     intent.force_x += (double) (2 * (MOUSE_YRANGE / 2 - mouse_y)) /
	  (double) MOUSE_YRANGE;
     
     intent.force_x *= fabs(intent.force_x);
     intent.force_y *= fabs(intent.force_y);
     intent.force_rotate *= intent.force_rotate * intent.force_rotate;;

     if (fabs(intent.force_x) < 0.1)
	  intent.force_x = 0.0;
     if (fabs(intent.force_y) < 0.1)
	  intent.force_y = 0.0;
     if (fabs(intent.force_rotate) < 0.1)
	  intent.force_rotate = 0.0;

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
