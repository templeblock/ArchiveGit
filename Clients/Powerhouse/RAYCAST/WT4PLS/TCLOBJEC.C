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

#include <tcl/tcl.h>

#include "wt.h"
#include "error.h"
#include "wtmem.h"
#include "fixed.h"
#include "view.h"
#include "object.h"


/* Object whose viewpoint which we will see from. */
static Object *viewpoint_object = NULL;

/* Object which we will control from the console. */
static Object *controlled_object = NULL;


static int object_command(ClientData clientData, Tcl_Interp *interp,
			  int argc, char *argv[]);


int parse_create(ClientData clientData, Tcl_Interp *interp,
		 int argc, char *argv[])
{
     Object *o;
     Object template = { 1.0,
			 1.0, 1.0, 1.0,
 		         0.0, 0.0, 0.0,
			 0.0, 0.0, 0.0,
			 0.0, 0.0,
			 0.6 };
     int i;
     char *name;


     if (argc < 2) {
	  interp->result = "too few arguments";
	  return TCL_ERROR;
     }

     name = argv[1];

     
     for (i = 2; i < argc; ) {

	  if (strcmp(argv[i], "-position") == 0) {

	       if (argc < i + 3) {
		    interp->result = "too few arguments to -position";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1], &template.x) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 2], &template.y) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 3], &template.z) != TCL_OK) {
		    interp->result = "invalid position";
		    return TCL_ERROR;
	       }
	       i += 4;

	  } else if (strcmp(argv[i], "-velocity") == 0) {

	       if (argc < i + 3) {
		    interp->result = "too few arguments to -velocity";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1],
				 &template.dx) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 2],
				 &template.dy) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 3],
				 &template.dz) != TCL_OK) {
		    interp->result = "invalid velocity";
		    return TCL_ERROR;
	       }
	       i += 4;

	  } else if (strcmp(argv[i], "-mass") == 0) {

	       if (argc < i + 1) {
		    interp->result = "too few arguments to -mass";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1],
				 &template.mass) != TCL_OK) {
		    interp->result = "invalid mass";
		    return TCL_ERROR;
	       }
	       i += 2;

	  } else if (strcmp(argv[i], "-angle") == 0) {

	       if (argc < i + 1) {
		    interp->result = "too few arguments to -angle";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1],
				 &template.angle) != TCL_OK) {
		    interp->result = "invalid angle";
		    return TCL_ERROR;
	       }
	       i += 2;

	  } else if (strcmp(argv[i], "-angular_velocity") == 0) {

	       if (argc < i + 1) {
		    interp->result = "too few arguments to -angular_velocity";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1],
				 &template.angular_v) != TCL_OK) {
		    interp->result = "invalid angular velocity";
		    return TCL_ERROR;
	       }
	       i += 2;

	  } else if (strcmp(argv[i], "-size") == 0) {

	       if (argc < i + 3) {
		    interp->result = "too few arguments to -size";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1],
				 &template.xsize) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 2],
				 &template.ysize) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 3],
				 &template.height) != TCL_OK) {
		    interp->result = "invalid position";
		    return TCL_ERROR;
	       }
	       i += 4;

	  } else if (strcmp(argv[i], "-drag") == 0) {

	       if (argc < i + 1) {
		    interp->result = "too few arguments to -drag";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1],
				 &template.drag) != TCL_OK) {
		    interp->result = "invalid drag";
		    return TCL_ERROR;
	       }
	       i += 2;

	  } else {

	       interp->result = "unknown object parameter";
	       return TCL_ERROR;

	  }
     }
		    
     o = new_object(0.0, 0.0, 0.0, 0.0, 0.0);
     *o = template;

     Tcl_CreateCommand(interp, name, object_command, (ClientData) o, NULL);

     return TCL_OK;
}


static int object_command(ClientData clientData, Tcl_Interp *interp,
			  int argc, char *argv[])
{
     int i;
     Object *o = (Object *) clientData;


     for (i = 1; i < argc; ) {

	  if (strcmp(argv[i], "-position") == 0) {

	       if (argc < i + 3) {
		    interp->result = "too few arguments to -position";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1], &o->x) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 2], &o->y) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 3], &o->z) != TCL_OK) {
		    interp->result = "invalid position";
		    return TCL_ERROR;
	       }
	       i += 4;

	  } else if (strcmp(argv[i], "-velocity") == 0) {

	       if (argc < i + 3) {
		    interp->result = "too few arguments to -velocity";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1], &o->dx) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 2], &o->dy) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 3], &o->dz) != TCL_OK) {
		    interp->result = "invalid velocity";
		    return TCL_ERROR;
	       }
	       i += 4;

	  } else if (strcmp(argv[i], "-angle") == 0) {

	       if (argc < i + 1) {
		    interp->result = "too few arguments to -angle";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1], &o->angle) != TCL_OK) {
		    interp->result = "invalid angle";
		    return TCL_ERROR;
	       }
	       i += 2;

	  } else if (strcmp(argv[i], "-angular_velocity") == 0) {

	       if (argc < i + 1) {
		    interp->result = "too few arguments to -angular_velocity";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1],
				 &o->angular_v) != TCL_OK) {
		    interp->result = "invalid angular velocity";
		    return TCL_ERROR;
	       }
	       i += 2;

	  } else if (strcmp(argv[i], "-force") == 0) {

	       double fx, fy, fz;

	       if (argc < i + 3) {
		    interp->result = "too few arguments to -force";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1], &fx) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 2], &fy) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 3], &fz) != TCL_OK) {
		    interp->result = "invalid force";
		    return TCL_ERROR;
	       }
	       i += 4;

	       object_apply_force(o, fx, fy, fz);

	  } else if (strcmp(argv[i], "-torque") == 0) {

	       double torque;

	       if (argc < i + 1) {
		    interp->result = "too few arguments to -torque";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1], &torque) != TCL_OK) {
		    interp->result = "invalid torque";
		    return TCL_ERROR;
	       }
	       i += 2;

	       object_apply_torque(o, torque);

	  } else if (strcmp(argv[i], "-size") == 0) {

	       if (argc < i + 3) {
		    interp->result = "too few arguments to -size";
		    return TCL_ERROR;
	       }
	       if (Tcl_GetDouble(interp, argv[i + 1], &o->xsize) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 2], &o->ysize) != TCL_OK ||
		   Tcl_GetDouble(interp, argv[i + 3], &o->height) != TCL_OK) {
		    interp->result = "invalid size";
		    return TCL_ERROR;
	       }
	       i += 4;
	       
	  } else if (strcmp(argv[i], "-attachviewpoint") == 0) {

	       viewpoint_object = o;
	       i++;

	  } else if (strcmp(argv[i], "-attachcontrol") == 0) {

	       controlled_object = o;
	       i++;

	  } else {

	       interp->result = "unknown object parameter";
	       return TCL_ERROR;

	  }

     }

     return TCL_OK;
}


Object *get_viewpoint_object(void)
{
     return viewpoint_object;
}


Object *get_controlled_object(void)
{
     return controlled_object;
}

