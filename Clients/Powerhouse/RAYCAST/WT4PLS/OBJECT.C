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


#include "wt.h"
#include "error.h"
#include "wtmem.h"
#include "fixed.h"
#include "view.h"
#include "object.h"


Object *new_object(double mass,
		   double xsize, double ysize, double height,
		   double drag)
{
     Object *o;

     o = wtmalloc(sizeof(Object));
     o->mass = mass;
     o->xsize = xsize;
     o->ysize = ysize;
     o->height = height;
     o->drag = drag;

     return o;
}


void object_update(Object *o)
{
     if (o->drag != 1.0) {
	  o->dx *= o->drag;
	  o->dy *= o->drag;
/*	  o->dz *= o->drag; */
	  o->angular_v *= o->drag;
     }

     o->x += o->dx;
     o->y += o->dy;
     o->z += o->dz;
     o->angle += o->angular_v;;
}


void object_apply_force(Object *o, double fx, double fy, double fz)
{
     o->dx += fx / o->mass;
     o->dy += fy / o->mass;
     o->dz += fz / o->mass;
}


/* This is a really simple model of torque--since there's no information in
**   the object about mass distribution, there is no moment of inertia.  For
**   lack of something better, we just use the mass.
*/
void object_apply_torque(Object *o, double torque)
{
     o->angular_v += torque / o->mass;
}


/* Derive a view from an object.  Assume that some fields of the view struct
**   have already been filled in:  view_plane_size, view_arc, and others that
**   cannot be determined from the object.
*/
void object_view(Object *o, View *v)
{
     v->x = FLOAT_TO_FIXED(o->x);
     v->y = FLOAT_TO_FIXED(o->y);
     v->height = FLOAT_TO_FIXED(o->z + o->height);
     v->angle = FLOAT_TO_FIXED(o->angle);
}
