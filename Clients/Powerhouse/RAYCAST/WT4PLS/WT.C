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
#include <stdio.h>
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

int checkwalls( World *, View * );

fixed view_height = FIXED_ONE+FIXED_ONE_HALF;

fixed start_x = FIXED_ZERO;
fixed start_y = FIXED_ZERO;

int main(int argc, char *argv[])
{
     World *w;
     FILE *fp;
     Boolean quit = False;
     Intent *intent;
     fixed v = FIXED_ZERO;
     double vx = 0.0, vy = 0.0, va = 0.0;
     Graphics_info *ginfo;

     
     if (argc != 2) {
	  fprintf(stderr, "Usage:  wt <world file>\n");
	  exit(EXIT_FAILURE);
     }

     ginfo = init_graphics();
     set_texture_trans(ginfo->palette.rgb_cube_size,
		       ginfo->palette.color_lookup);
     init_renderer(ginfo->width, ginfo->height);
     init_input_devices();

     if ((fp = fopen(argv[1], "r")) == NULL) {
	  perror(argv[1]);
	  exit(EXIT_FAILURE);
     }
     
     w = read_world_file(fp);
     fclose(fp);

     /* setup view */
/*
     view = new_view(fixdiv(FIXED_2PI, INT_TO_FIXED(4)));
*/
     view = new_view(FLOAT_TO_FIXED(3.14159265 / 2.0));

     view->x      = start_x;
     view->y      = start_y;
     view->height = FIXED_ONE+FIXED_ONE_HALF;
     view->angle  = FIXED_ZERO;

     while (!quit) {
	  double sin_facing, cos_facing;
	  Framebuffer *fb;

	  fb = render(w, view);
	  update_screen(fb);

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

	  if (view->height > view_height)
	       v -= FIXED_ONE / 16;

	  view->height += v;

	  if (view->height < view_height) {
	       v = FIXED_ZERO;
	       view->height = view_height;
	  }

	  while (intent->n_special--) {
	       if (intent->special[intent->n_special] == INTENT_END_GAME)
		    quit = True;
	       else
		    v = FIXED_ONE / 2;
	  }

	  checkwalls( w, view );

     }

     end_input_devices();
     end_graphics();

     return EXIT_SUCCESS;
}
