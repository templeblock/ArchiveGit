/* (WT)      WORLD.C */
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
#include "wtmem.h"
#include "texture.h"
#include "table.h"
#include "world.h"


#define START_VERTICES 50
#define START_WALLS    30
#define START_TEXTURES 10


World *new_world(void)
{
     World *w;

     w = wtmalloc(sizeof(World));
     w->vertices = new_table(sizeof(Vertex));
     w->walls = new_table(sizeof(Wall));
     w->regions = new_table(sizeof(Region));
     w->textures = new_table(sizeof(Texture *));

     return w;
}


void add_texture(World *w, Texture *tex)
{
     add_table_entry(w->textures, &tex);
}


void add_wall(World *w, Wall *wall)
{
     add_table_entry(w->walls, wall);
}


void add_vertex(World *w, Vertex *v)
{
     add_table_entry(w->vertices, v);
}


void add_region(World *w, Region *r)
{
     add_table_entry(w->regions, r);
}


void update_wall_scale(World *w, int wall_num, fixed xscale, fixed yscale)
{
     Wall *wall;
     Texture *texture;
     fixed wall_length;

     if (wall_num < 0 || wall_num > TABLE_SIZE(w->walls))
	  fatal_error("invalid wall number");

     wall = (Wall *) w->walls->table + wall_num;
     texture = wall->surface_texture;

     wall_length =
	  FLOAT_TO_FIXED(sqrt(FIXED_TO_FLOAT(wall->vertex2->x -
					     wall->vertex1->x) *
			      FIXED_TO_FLOAT(wall->vertex2->x -
					     wall->vertex1->x) +
			      FIXED_TO_FLOAT(wall->vertex2->y -
					     wall->vertex1->y) *
			      FIXED_TO_FLOAT(wall->vertex2->y -
					     wall->vertex1->y)));
     wall->yscale = fixmul(yscale, INT_TO_FIXED(texture->height));
     wall->xscale = fixmul(fixmul(xscale, INT_TO_FIXED(texture->width)),
			   wall_length);
}


void update_wall_phase(World *w, int wall_num, fixed xphase, fixed yphase)
{
     Wall *wall;

     if (wall_num < 0 || wall_num > TABLE_SIZE(w->walls))
	  fatal_error("invalid wall number");

     wall = (Wall *) w->walls->table + wall_num;

     wall->xphase = xphase;
     wall->yphase = yphase;
}

#ifdef WIN32
//GCT minor addition to support the reloading of Worlds and their components.
int free_world(World *w)
{
   if (w->vertices)
      free_table(w->vertices);
   if (w->walls)
      free_table(w->walls);
   if (w->regions)
      free_table(w->regions);
   if (w->textures)
      free_table(w->textures);
   wtfree(w);
   return EXIT_SUCCESS;
}
#endif


