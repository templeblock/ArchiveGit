/* (WT)      WORLD.H */
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

#define NO_WALL -1

typedef struct {
     fixed x, y;
     fixed tx, ty, proj;      /* transformed coordinates */
} Vertex;


typedef struct {
     fixed floor, ceiling;
     Texture *floor_tex, *ceiling_tex;
} Region;


typedef struct {
     Vertex *vertex1, *vertex2;
     Texture *surface_texture;
     Region *front, *back;
     fixed xphase, yphase;
     fixed xscale, yscale;
     Boolean sky;
     Boolean opaque;
} Wall;


typedef struct {
     Table *vertices;
     Table *walls;
     Table *regions;
     Table *textures;
} World;


#define WORLD_VERTEX(w, v)   ((TABLE_ELEMENTS((w)->vertices, Vertex))[v])
#define WORLD_TEXTURE(w, t)  ((TABLE_ELEMENTS((w)->textures, Texture *))[t])
#define WORLD_REGION(w, r)   ((TABLE_ELEMENTS((w)->regions, Region))[r])
#define WORLD_WALL(w, x)     ((TABLE_ELEMENTS((w)->walls, Wall))[x])

extern World *new_world(void);
extern void add_texture(World *w, Texture *tex);
extern void add_wall(World *w, Wall *wall);
extern void add_vertex(World *w, Vertex *v);
extern void add_region(World *w, Region *r);
extern void update_wall_scale(World *w, int wall_num,
			      fixed xscale, fixed yscale);
extern void update_wall_phase(World *w, int wall_num,
			      fixed xphase, fixed yphase);
extern void update_wall_height(World *w, int wall_num,
			       fixed floor_front, fixed floor_back,
			       fixed ceiling_front, fixed ceiling_back);
#ifdef WIN32
//GCT minor addition to support the reloading of Worlds and their components.
extern int free_world(World *w);
#endif
