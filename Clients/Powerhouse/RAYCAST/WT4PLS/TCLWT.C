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
#include <tcl/tcl.h>
#include "wt.h"
#include "error.h"
#include "fixed.h"
#include "view.h"
#include "framebuf.h"
#include "texture.h"
#include "table.h"
#include "world.h"
#include "tclworld.h"
#include "render.h"
#include "graphics.h"
#include "input.h"
#include "object.h"
#include "tclobject.h"
#include "tclupdate.h"
#include "tclwt.h"

#define PROFILE 1
#if PROFILE
#include "timer.h"
#endif

static void parser_init(Tcl_Interp *interp, World *w);

static Updater *updater;


int main(int argc, char *argv[])
{
     World *w;
     View *view;
     Intent *intent;
     Graphics_info *ginfo;
     Tcl_Interp *interp;
     Object *me;
     Boolean quit = False;
#if PROFILE
     int fps_count = 0;
     double total_time = 0.0;
     double start_time;
#endif


     interp = Tcl_CreateInterp();
     updater = updater_create(interp);

     if (argc != 2) {
	  fprintf(stderr, "Usage:  %s <world file>\n", argv[0]);
	  exit(EXIT_FAILURE);
     }

     ginfo = init_graphics();
#define TRUECOLOR
#ifdef TRUECOLOR
     set_texture_trans(32, ginfo->palette.color_lookup);
#else
     set_texture_trans(ginfo->palette.rgb_cube_size,
		       ginfo->palette.color_lookup);
#endif
     init_renderer(ginfo->width, ginfo->height);
     init_input_devices();

     /* Initialize the view */
     view = new_view(FLOAT_TO_FIXED(3.14159265 / 2.0));

     w = new_world();

     parser_init(interp, w);
     if (Tcl_EvalFile(interp, WT_INIT_FILENAME) != TCL_OK) {
	  fprintf(stderr, "%s\n", Tcl_GetVar(interp, "errorCode", 0));
	  fprintf(stderr, "%s\n", Tcl_GetVar(interp, "errorInfo", 0));
	  exit(1);
     }

     if (Tcl_EvalFile(interp, argv[1]) != TCL_OK) {
	  fprintf(stderr, "%s\n", Tcl_GetVar(interp, "errorCode", 0));
	  fprintf(stderr, "%s\n", Tcl_GetVar(interp, "errorInfo", 0));
	  exit(1);
     }


     while (!quit) {
	  double sin_facing, cos_facing;
	  double fx, fy, fz, torque;
	  Framebuffer *fb;
	  int i;


	  start_time = current_time();

	  intent = read_input_devices();
	  
	  me = get_controlled_object();
	  if (me == NULL)
	       fatal_error("No controlled object");

	  for (i = 0; i < intent->n_special; i++) {

	       switch (intent->special[i]) {
		  case INTENT_END_GAME:
		    quit = True;
		    (void) Tcl_Eval(interp, "action_quit");
		    break;

		  case INTENT_JUMP:
		    (void) Tcl_Eval(interp, "action_jump");
		    break;

		  case INTENT_ACTION1:
		    (void) Tcl_Eval(interp, "action_1");
		    break;

		  case INTENT_ACTION2:
		    (void) Tcl_Eval(interp, "action_2");
		    break;

		  case INTENT_ACTION3:
		    (void) Tcl_Eval(interp, "action_3");
		    break;

		  case INTENT_ACTION4:
		    (void) (Tcl_Eval(interp, "action_4") != TCL_OK);
		    break;

		  case INTENT_ACTION5:
		    (void) Tcl_Eval(interp, "action_5");
		    break;

		  default:
		    break;
	       }
	  }

	  /* Determine forces on viewer. */
	  sin_facing = sin(me->angle);
	  cos_facing = cos(me->angle);
	  fx = cos_facing * intent->force_x - sin_facing * intent->force_y;
	  fy = sin_facing * intent->force_x + cos_facing * intent->force_y;
 	  fz = -0.05 * me->mass;  /* gravity */
	  torque = intent->force_rotate;

	  /* Apply the forces. */
	  object_apply_force(me, fx, fy, fz);
	  object_apply_torque(me, torque);
	  object_update(me);
	  if (me->z <= 0.0 && me->dz <= 0.0) {
	       me->z = 0.0;
	       me->dz = 0.0;
	  }

	  /* Determine the view. */
	  me = get_viewpoint_object();
	  if (me == NULL)
	       fatal_error("No viewpoint object");
	  object_view(me, view);

	  updater_run(updater);

	  /* Display the world. */
	  fb = render(w, view);
	  update_screen(fb);
#if PROFILE
	  fps_count++;
	  total_time += current_time() - start_time;
	  if (fps_count == 100) {
	       printf("fps = %3.2f\n", (double) fps_count / total_time);
	       fps_count = 0;
	       total_time = 0.0;
	  }
#endif
     }

     end_input_devices();
     end_graphics();

     return EXIT_SUCCESS;
}


static void parser_init(Tcl_Interp *interp, World *w)
{
     /* World building commands. */
     Tcl_CreateCommand(interp, WT_CMD_CREATE_TEXTURE, parse_texture,
		       (ClientData) w, (Tcl_CmdDeleteProc *) NULL);
     Tcl_CreateCommand(interp, WT_CMD_CREATE_WALL, parse_wall,
		       (ClientData) w, (Tcl_CmdDeleteProc *) NULL);
     Tcl_CreateCommand(interp, WT_CMD_CREATE_REGION, parse_region,
		       (ClientData) w, (Tcl_CmdDeleteProc *) NULL);
     Tcl_CreateCommand(interp, WT_CMD_CREATE_VERTEX, parse_vertex,
		       (ClientData) w, (Tcl_CmdDeleteProc *) NULL);

     /* World modifying commands. */
     Tcl_CreateCommand(interp, WT_CMD_SKYWALL, parse_skywall, (ClientData) w,
		       (Tcl_CmdDeleteProc *) NULL);
     Tcl_CreateCommand(interp, WT_CMD_MODIFY_REGION, parse_set_region,
		       (ClientData) w, (Tcl_CmdDeleteProc *) NULL);
     Tcl_CreateCommand(interp, WT_CMD_MODIFY_VERTEX, parse_move_vertex,
		       (ClientData) w, (Tcl_CmdDeleteProc *) NULL);
     Tcl_CreateCommand(interp, WT_CMD_GET_REGION, parse_get_region,
		       (ClientData) w, (Tcl_CmdDeleteProc *) NULL);
     

     /* Updater commands. */
     Tcl_CreateCommand(interp, WT_CMD_UPDATE, parse_addupdate,
		       (ClientData) updater, (Tcl_CmdDeleteProc *) NULL);

     /* Object commands. */
     Tcl_CreateCommand(interp, WT_CMD_CREATE_OBJECT, parse_create,
		       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
}
