/* (WT)  RENDER.C */
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

/* Some minor changes made for the MS-DOS port
 * by Petteri Kangaslampi <pekanga@freeport.uwasa.fi>
*/

/* Some minor changes made for the Windows port.
** by Chris Thomas (GCT).  You'll see a GCT or WIN32 next to my changes. One
** bug fix, but rest are to increase Windows compatibility (resizing) or
** for diagnostic/investigative reasons.  I might have missed some GCT tagging.
** UpdateWT calls are commented out.  These calls repaint the bitmap, so
** you can see rendering in progress - might be too fast on other machines,
** but has a good pace on my 486/33 ISA.
*/

#define PROFILE 0

/* Note! PROFILE MUST be 0 with MS-DOS systems (PK) */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#if PROFILE
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#include <stdio.h>                      /* stdio.h is needed even if */
#include "wt.h"                         /* PROFILE is 0. Fixed. (PK) */
#include "error.h"
#include "fixed.h"
#include "wtmem.h"
#include "table.h"
#include "view.h"
#include "texture.h"
#include "framebuf.h"
#include "graphics.h"
#include "world.h"
#include "render.h"


#define VIEW_WIDTH  (fb->fb_width)			//GCT
#define VIEW_HEIGHT (fb->fb_height)

/* These macros convert 16.16 fixed point numbers to and from a 2.30 format.
**   The extra fractional precision is needed when doing doing 1 / distance
**   calculations for perspective.
*/
#define TO_FIX_2_30(f)   ((f) << 14)
#define FROM_FIX_2_30(f) ((f) >> 14)
#define TO_FIX_8_24(f)   ((f) << 8)
#define FROM_FIX_8_24(f) ((f) >> 8)
#define MAX_WALL_EVENTS 30

#define MAX_ERROR  (VIEW_WIDTH)


typedef struct {
     Wall *wall;
     fixed z, dz;
     Boolean is_back_view;
} Wall_start;

typedef Wall *Wall_end;

typedef struct {
     int n_events;
     Wall_start events[MAX_WALL_EVENTS];
} Wall_start_list;

typedef struct {
     int n_events;
     Wall_end events[MAX_WALL_EVENTS];
} Wall_end_list;

typedef struct {
     Boolean is_back_view;
     Wall *wall;
     Boolean visible;
     fixed pstart1, pend1, pstart2, pend2;
     fixed dpstart1, dpend1, dpstart2, dpend2;
     fixed z;
     fixed dz;
} Active_wall;

typedef struct {
     fixed pstart1, pend1, pstart2, pend2;
     Boolean is_back_view;
     fixed top, bottom;
     Region *front, *back;
     Wall *wall;
     fixed z;
} Wall_intersection;

typedef struct {
     fixed screen_dy, screen_dx;
     fixed view_sin, view_cos;
     fixed sin_dx, cos_dx;
     fixed *sin_tab, *cos_tab, *row_view;
} View_constants;

extern void draw_wall_slice(Pixel *start, Pixel *end, unsigned char *tex_base,
			    fixed tex_y, fixed tex_dy, int fb_width, 
			    int tex_height, int npix);

static void transform_vertices(World *w, View *view);
static void clip_walls(World *w, View *v);
static void add_wall_events(View *v, Wall *wall,
			    fixed x1, fixed px1, fixed x2, fixed px2);
static void render_walls(World *w, View *v);
static int add_events(Active_wall *active, int n_active, int column);
static Boolean wall_obscured(Vertex *common, Vertex *v1, Vertex *v2);
static int remove_events(Active_wall *active, int n_active, int column);
static Wall_intersection *calc_wall_heights(Active_wall *active, int n_active,
					    int column, fixed height,
					    Wall_intersection *cur_slice);
static void draw_walls(Wall_intersection *int_list, View *v);
static void draw_floors(Wall_intersection *int_list, View *v);
static void draw_floor_slices(Region *r, fixed start, fixed end, View *v,
			      int column);
static void draw_ceiling_slices(Region *r, fixed start, fixed end,
				View *v, int column);
static fixed wall_ray_intersection(fixed Vx, fixed Vy, Wall *wall);
static void init_buffers(void);
static void calc_view_constants(View *v, int screen_width, int screen_height);


static Framebuffer *fb = NULL;
static Wall_start_list *start_events = NULL;
static Wall_end_list *end_events = NULL;
static Wall_intersection *intersections = NULL;
static int *fb_rows = NULL;
static View_constants view_constants;

#if defined(__GNUC__) && defined(ARCH_i86)
#include "slice-gas86.c"
#else
#ifdef __WATCOMC__
#include "slice-wc.c"                   /* Watcom C specific scaling
                                           routines */
#else
#ifdef WIN32
#include "slicewin.c"
#else
#include "slice.c"
#endif
#endif
#endif



void init_renderer(int fb_width, int fb_height)
{
     int i;


     if (fb != NULL)
	  wtfree(fb);
     if (start_events != NULL)
	  wtfree(start_events);
     if (end_events != NULL)
	  wtfree(end_events);
     if (fb_rows != NULL)
	  wtfree(fb_rows);

     fb = new_framebuffer(fb_width, fb_height);
     start_events = wtmalloc(sizeof(Wall_start_list) * (fb_width + 1));
     end_events = wtmalloc(sizeof(Wall_end_list) * (fb_width + 1));
     fb_rows = wtmalloc(sizeof(int) * fb_height);
     for (i = 0; i < fb_height; i++)
	  fb_rows[i] = i * fb_width;
}


#if PROFILE
static double current_time(void)
{
     struct timeval tv;
     struct timezone tz;

     tz.tz_minuteswest = 0;
     tz.tz_dsttime = DST_NONE;
     gettimeofday(&tv, &tz);

     return (double) tv.tv_sec + (double) tv.tv_usec / 1000000.0;
}
#endif

#ifdef ARCH_SUN
static void *memmove(void *dest, void *src, size_t size)
{
     int *lsrc = (int *) src;
     int *ldest = (int *) dest;

     size /= 4;
     /* Here, we assume that size is a multiple of four.  That's the only
     **   way in which memmove will be used.  This assumption is nasty,
     **   but life is rough . . . I'm bitter about having to implement
     **   memmove in the first place.
     */
     if (lsrc < ldest) {
	  ldest += size - 1;
	  lsrc += size - 1;
	  while (size-- > 0)
	       *ldest-- = *lsrc--;
     } else {
	  while (size-- > 0)
	       *ldest++ = *lsrc++;
     }

     return dest;
}
#endif


void render(World *w, View *v)
{
#if PROFILE
     static int fps_count = 0;
     static double total_time = 0.0;
     double start_time = current_time();
#endif
     init_buffers();
     calc_view_constants(v, VIEW_WIDTH, VIEW_HEIGHT);
     transform_vertices(w, v);
     clip_walls(w, v);
     clear_framebuffer(fb);
//UpdateWT(1); //GCT diagnostic
     render_walls(w, v);
     update_screen(fb);
//UpdateWT(0); //GCT diagnostic

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

///////////////////////////////////////////////////////////////////////////
// GCT.  Tried a smattering of inline ASM to liven up the day (night 
// really >-).
// Note: __asm blocks are Symantec and Microsoft compatible.

#define WIN32_some_assembler

#ifdef WIN32_some_assembler

//this made a difference, dropped time from 7800 to 7700. that was timed with
//the C fixed routines, not the asm fixed.  Asm fixed and this
//xform routine get 7300 (I once saw 66500.  Windows timings are 
//notoriously inaccurate.  These timings were using Symantec.

/*static*/ xform_vertex_list(Vertex *pVertex, int nVertices, int nVertexSize,
		fixed viewx, fixed viewy, fixed eyedistance, 
		fixed view_sin, fixed view_cos)
{
__asm {
		mov	esi,pVertex

xform_loop:

;	  fixed x = vertex->x - view->x;
		mov	eax,[esi]			;vertex->x
	   sub	eax,viewx			;vertex->x - view->x
		mov	edi,eax 			   ;edi = x = vertex->x - view->x

		mov	edx,view_cos
		imul	edx
		shrd  eax,edx,16
		mov	ebx,eax				;ebx = fixmul(x, view_cos)

;	  fixed y = vertex->y - view->y;
		mov	eax,[esi+4]			;vertex->y
	   sub	eax,viewy			;vertex->y - view->y
		mov	ecx,eax				;ecx = y = vertex->y - view->y

		mov	edx,view_sin
		imul	edx
		shrd  eax,edx,16			;eax = fixmul(y, view_sin)

; vertex->tx = fixmul(x, view_cos) - fixmul(y, view_sin);
		sub 	ebx,eax
		mov	[esi+4+4],ebx 	   ;store in vertex->tx

		mov	eax,edi
		mov	edx,view_sin
		imul	edx
		shrd  eax,edx,16			;eax = fixmul(x, view_sin)
		mov	ebx,eax

		mov	eax,ecx
		mov	edx,view_cos
		imul	edx
		shrd  eax,edx,16			;eax = fixmul(y, view_cos)

; vertex->ty = fixmul(x, view_sin) + fixmul(y, view_cos);
		add	ebx,eax
		mov	[esi+4+4+4],ebx	;store in vertex->ty

; if (vertex->tx > view->eye_distance)
		mov	eax,[esi+4+4]		;vertex->tx
		cmp	eax,eyedistance
		jle	short xform_no_project

;	       /* project point onto view plane */
;	       vertex->proj = fixdiv(vertex->ty, vertex->tx);

		mov	ecx,eax				;vertex->tx, from above.
		mov	eax,ebx				;vertex->ty, from above.
		mov   edx,eax
		sar   edx,16
		shl   eax,16
		idiv	ecx					;vertex->ty / vertex->tx
		mov	[esi+4+4+4+4],eax	;vertex->proj = fixdiv(vertex->ty / vertex->tx)

xform_no_project:

		add	esi,nVertexSize	;advance to next vertex.
		dec	nVertices			;more left?
		jnz	xform_loop	
	}
}

#endif

static void transform_vertices(World *w, View *view)
{
     Vertex *vertex;
     fixed view_sin, view_cos;
     int i;

     vertex = TABLE_ELEMENTS(w->vertices, Vertex);
     view_sin = view_constants.view_sin;
     view_cos = view_constants.view_cos;

#ifdef WIN32_some_assembler
		xform_vertex_list(vertex, TABLE_SIZE(w->vertices), sizeof(Vertex),
				view->x, view->y, view->eye_distance, view_sin, view_cos);
#else
      for (i = 0; i < TABLE_SIZE(w->vertices); i++, vertex++) {
		  	fixed x = vertex->x - view->x;
		  	fixed y = vertex->y - view->y;

			vertex->tx = fixmul(x, view_cos) - fixmul(y, view_sin);
			vertex->ty = fixmul(x, view_sin) + fixmul(y, view_cos);
			if (vertex->tx > view->eye_distance)
				/* project point onto view plane */
				vertex->proj = fixdiv(vertex->ty, vertex->tx);
		}
#endif
}


static void clip_walls(World *w, View *v)
{
     Wall *wall = (Wall *) w->walls->table;
     int i;

     for (i = 0; i < TABLE_SIZE(w->walls); i++, wall++) {
	  fixed x1, y1, px1, x2, y2, px2;
	  unsigned int outcode1, outcode2;

	  x1 = wall->vertex1->tx;
	  x2 = wall->vertex2->tx;
	  /* See if the wall lies completely behid the view plane. */
	  if (x1 < v->eye_distance && x2 < v->eye_distance)
	       continue;

	  y1 = wall->vertex1->ty;
	  px1 = wall->vertex1->proj;
	  y2 = wall->vertex2->ty;
	  px2 = wall->vertex2->proj;

	  /*** Clipping ***/

   	  /* First, clip to the view plane (line, really, since we're
	  **   working in only two dimensions.)
	  */
	  if (x1 <= v->eye_distance) {
	       /* be careful for division overflow */
	       if (x2 - x1 < FIXED_EPSILON)
		    continue;
	       y1 = y1 + fixmul(v->eye_distance - x1,
				fixdiv(y2 - y1, x2 - x1));
	       px1 = y1;
	       x1 = v->eye_distance;
	  }
	  if (x2 <= v->eye_distance) {
	       if (x1 - x2 < FIXED_EPSILON)
		    continue;
	       y2 = y2 + fixmul(v->eye_distance - x2,
				fixdiv(y1 - y2, x1 - x2));
	       px2 = y2;
	       x2 = v->eye_distance;
	  }

	  /* Now, clip to the sides of the view polygon. */
	  outcode1 = FIXED_SIGN(v->view_plane_size + px1);
	  outcode1 |= FIXED_SIGN(v->view_plane_size - px1) << 1;
	  outcode2 = FIXED_SIGN(v->view_plane_size + px2);
	  outcode2 |= FIXED_SIGN(v->view_plane_size - px2) << 1;

	  /* trivial reject */
	  if ((outcode1 & outcode2) != 0)
	       continue;
	  /* check for trivial accept */
	  if ((outcode1 | outcode2) != 0) {
	       /* Damn . . . we need to clip. */
	       fixed slope, denom, y_diff;

	       denom = (x2 - x1);
	       if (FIXED_ABS(denom) < FIXED_EPSILON) {
		    if (denom < 0)
			 slope = FIXED_MIN + v->view_plane_size;
		    else
			 slope = FIXED_MAX - v->view_plane_size;
	       } else
		    slope = fixdiv(y2 - y1, denom);

	       if (outcode1 == 1) {
		    px1 = -v->view_plane_size;
		    y_diff = y1 - fixmul(x1, -v->view_plane_size);
		    slope += v->view_plane_size;
		    if (FIXED_ABS(slope) > FIXED_EPSILON)
			 x1 -= fixdiv(y_diff, slope);
		    else
			 x1 = FIXED_MAX;
	       } else if (outcode1 == 2) {
		    px1 = v->view_plane_size;
		    y_diff = y1 - fixmul(x1, v->view_plane_size);
		    slope -= v->view_plane_size;
		    if (FIXED_ABS(slope) > FIXED_EPSILON)
			 x1 -= fixdiv(y_diff, slope);
		    else
			 x1 = FIXED_MAX;
	       }

	       if (outcode2 == 1) {
		    px2 = -v->view_plane_size;
		    y_diff = y2 - fixmul(x2, -v->view_plane_size);
		    slope += v->view_plane_size;
		    if (FIXED_ABS(slope) > FIXED_EPSILON)
			 x2 -= fixdiv(y_diff, slope);
		    else
			 x2 = FIXED_MAX;
	       } else if (outcode2 == 2) {
		    px2 = v->view_plane_size;
		    y_diff = y2 - fixmul(x2, v->view_plane_size);
		    slope -= v->view_plane_size;
		    if (FIXED_ABS(slope) > FIXED_EPSILON)
			 x2 -= fixdiv(y_diff, slope);
		    else
			 x2 = FIXED_MAX;
	       }
	  }

	  add_wall_events(v, wall, x1, px1, x2, px2);
     }
}


/* Add a wall to the event list--one event is added for the start of the
**   wall, and another is added to mark the end of the wall.
*/
static void add_wall_events(View *v, Wall *wall,
			    fixed x1, fixed px1, fixed x2, fixed px2)
{
     int fb1, fb2;
     fixed z1, z2;
     Wall_start *event;


     /* convert to frame buffer coordinates */
     px1 = fixdiv(px1, view_constants.screen_dx + 1);
     px2 = fixdiv(px2, view_constants.screen_dx + 1);
     fb1 = FIXED_TO_INT(px1) + (VIEW_WIDTH >> 1);
     fb2 = FIXED_TO_INT(px2) + (VIEW_WIDTH >> 1);

     /* There's no need to deal with walls that start and end in the same
     **   screen column.  In a properly contructed world, we're guaranteed
     **   that throwing them away won't leave any gaps.
     */
     if (fb1 == fb2)
	  return;

     /* Here we use a 2.30 fixed point format.  The result of this calculation
     **   is always between 1 and zero, as the distance can never be less
     **   than the view plane distance.  The extra fractional bits are
     **   critical for the inverses.  Note that using 2.30 restricts the
     **   size of the view plane to something less than 2.
     */
     z1 = fixdiv(TO_FIX_2_30(v->eye_distance), x1);
     z2 = fixdiv(TO_FIX_2_30(v->eye_distance), x2);

     if (fb1 < fb2) {

	  event = &start_events[fb1].events[start_events[fb1].n_events];

	  event->wall = wall;
	  event->z = z1;
	  event->dz = fixdiv(z2 - z1, INT_TO_FIXED(fb2 - fb1));
	  event->is_back_view = False;
	  start_events[fb1].n_events++;

	  end_events[fb2].events[end_events[fb2].n_events] = wall;
	  end_events[fb2].n_events++;

     } else {

	  event = &start_events[fb2].events[start_events[fb2].n_events];

	  event->wall = wall;
	  event->z = z2;
	  event->dz = fixdiv(z1 - z2, INT_TO_FIXED(fb1 - fb2));
	  event->is_back_view = True;
	  start_events[fb2].n_events++;

	  end_events[fb1].events[end_events[fb1].n_events] = wall;
	  end_events[fb1].n_events++;

     }
}


static void render_walls(World *w, View *v)
{
     static int last_wall_count = 0;
     static Active_wall *active;
     int column;
     int active_count = 0;
     fixed Vx, Vy, dVy;
     Wall_intersection *cur_slice;


     /* Make sure that the active list is large enough to hold all the
     **   walls in a world.
     */
     if (last_wall_count != TABLE_SIZE(w->walls)) {
	  last_wall_count = TABLE_SIZE(w->walls);
	  if (active == NULL)
	       active = wtmalloc(sizeof(Active_wall) * last_wall_count);
	  else
	       active = wtrealloc(active, sizeof(int) * last_wall_count);

	  /* I know this is really cheesy, but . . . */
	  if (intersections == NULL)
	       intersections = wtmalloc(sizeof(Wall_intersection) * 3000);
	  else
	       intersections = wtrealloc(intersections,
					 sizeof(Wall_intersection) * 4000);
     }

     /* Set up for fast calculation of view rays. */
     Vx = v->eye_distance;
     Vy = -v->view_plane_size;
     dVy = fixdiv(fixmul(v->view_plane_size, INT_TO_FIXED(2)),
                  INT_TO_FIXED(VIEW_WIDTH));

     /* Build a sorted wall intersection list for each screen column. */
     cur_slice = intersections;

     for (column = 0; column < VIEW_WIDTH; column++) {
	  Active_wall *current, *last;
	  /* Keep track of distances of walls in the active list.  Notice that
	  **   we're not actually tracking the distances of walls, but
	  **   1 / distance instead.  That's because we can linearly
	  **   interpolate 1 / distance.  Also, most calculations that
	  **   use distance are really using 1 / distance (i.e. distance
	  **   appears in the denominator.
	  */

	  active_count = add_events(active, active_count, column);
	  cur_slice = calc_wall_heights(active, active_count, column,
					v->height, cur_slice);
	  active_count = remove_events(active, active_count, column);

	  last = active + active_count - 1;
	  for (current = active; current <= last; current++) {
	       current->z += current->dz;
	       if (current->visible) {
		    current->pstart1 += current->dpstart1;
		    current->pend1 += current->dpend1;
		    current->pstart2 += current->dpstart2;
		    current->pend2 += current->dpend2;
	       }
	  }

	  Vy += dVy;
     }

     /* Once we have the per-column sorted intersection list, most of the
     **   tricky stuff is done.  All that's left is to actually blast bytes
     **   into the framebuffer using the routines in slice.c.
     */
     draw_walls(intersections, v);
     draw_floors(intersections, v);
}


/* Add new walls to the active list.  The active list is kept
**   depth sorted.  We have to be careful here.  Correct depth
**   ordering of the walls is vital for rendering.  At corners
**   we have two or more walls at the same distance; however,
**   there is still a correct and incorrect ordering.  If one
**   wall is obscured by another, the visible wall must be placed
**   in front in the list.
*/
static int add_events(Active_wall *active, int n_active, int column)
{
     int i, j;
     Wall_start *event;
     Wall *wall;
     fixed z;

     for (i = 0; i < start_events[column].n_events; i++) {
	  event = start_events[column].events + i;

	  wall = event->wall;
	  z = event->z;

	  for (j = 0; j < n_active; j++) {
	       Wall *wall2 = active[j].wall;
	       Vertex *common, *v1, *v2;

	       if (z < active[j].z - MAX_ERROR)
		    continue;
	       else if (z > active[j].z + MAX_ERROR)
		    break;

	       /* See if the walls share a vertex. */
	       if (wall->vertex1 == wall2->vertex1) {
		    common = wall->vertex1;
		    v1 = wall->vertex2;
		    v2 = wall2->vertex2;
	       } else if (wall->vertex1 == wall2->vertex2) {
		    common = wall->vertex1;
		    v1 = wall->vertex2;
		    v2 = wall2->vertex1;
	       } else if (wall->vertex2 == wall2->vertex1) {
		    common = wall->vertex2;
		    v1 = wall->vertex1;
		    v2 = wall2->vertex2;
	       } else if (wall->vertex2 == wall2->vertex2) {
		    common = wall->vertex2;
		    v1 = wall->vertex1;
		    v2 = wall2->vertex1;
	       } else {
		    /* We have two walls which are really close
		    **   together, but share no vertices.  Because
		    **   of roundoff error, we don't know for certain
		    **   which one is really in front.  Ideally, this
		    **   situation will be avoided by creating worldfiles
		    **   which don't place non-adjoining walls extremely
		    **   close together.
		    */
		    if (z > active[j].z)
			 break;
		    else
			 continue;
	       }

	       if (!wall_obscured(common, v1, v2) &&
		   wall_obscured(common, v2, v1))
		    break;
	  }

	  /* Insert the wall into the active list. */
	  memmove(active + j + 1, active + j,
		  sizeof(Active_wall) * (n_active - j));
	  active[j].wall = wall;
	  active[j].z = z;
	  active[j].dz = event->dz;
	  active[j].visible = False;
	  active[j].is_back_view = event->is_back_view;
	  n_active++;
     }

     return n_active;
}


/* Determine whether wall 1 is obscured by wall 2 from the view point.
**   This will be the case if a halfplane defined by wall 1 contains both the
**   view point and wall 2.  Wall 1 is defined by the points common and v1;
**   wall2 is defined by command and v2.  Note that this function uses the
**   transformed coordinates of the vertices, so the view point and view
**   direction need not be passed explicitly.
*/
static Boolean wall_obscured(Vertex *common, Vertex *v1, Vertex *v2)
{
     fixed x1, y1, x2, y2;
     unsigned int sign1, sign2;


     x1 = common->tx - v1->tx;
     y1 = common->ty - v1->ty;
     x2 = common->tx - v2->tx;
     y2 = common->ty - v2->ty;

     /* There's some tricky stuff done here to try to find the signs of
     **   cross products without actually doing any multiplication.  I'm
     **   really not sure if avoiding a few multiplies is worth the extra
     **   overhead of sign checking, but the profiler shows this function as
     **   taking a surprisingly small percentage of execution time.
     */
     if (FIXED_PRODUCT_SIGN(x1, y2) ^ FIXED_PRODUCT_SIGN(x2, y1))
	  sign1 = FIXED_PRODUCT_SIGN(x1, y2);
     else
	  sign1 = FIXED_SIGN(fixmul(x1, y2) - fixmul(x2, y1));
     if (FIXED_PRODUCT_SIGN(x1, common->ty) ^
	 FIXED_PRODUCT_SIGN(common->tx, y1))
	  sign2 = FIXED_PRODUCT_SIGN(x1, common->ty);
     else
	  sign2 = FIXED_SIGN(fixmul(x1, common->ty) - fixmul(common->tx, y1));

     if (sign1 ^ sign2)
	  return False;
     else
	  return True;
}


/* Remove walls from the active list.  Return the number of remaining
** active walls.
*/
static int remove_events(Active_wall *active, int n_active, int column)
{
     int i, j;
     Wall *wall;


     /* This is really inefficient, so I hope these event lists are small. */
     for (i = 0; i < end_events[column].n_events; i++) {
	  wall = end_events[column].events[i];

	  for (j = 0; j < n_active && active[j].wall != wall; j++);
	  n_active--;
	  memmove(active + j, active + j + 1,
		  sizeof(Active_wall) * (n_active - j));
     }

     return n_active;
}


static Wall_intersection *calc_wall_heights(Active_wall *active, int n_active,
					    int column, fixed height,
					    Wall_intersection *cur_slice)
{
     fixed top = FIXED_ONE_HALF, bottom = -FIXED_ONE_HALF;


     while (n_active-- > 0 && bottom < top) {
	  Wall *wall = active->wall;
	  fixed z = active->z;
	  fixed dz = active->dz;

	  if (!active->visible) {
	       active->pstart1 =
		    fixmul2_30(TO_FIX_8_24(wall->front->floor - height), z);
	       active->pend1 =
		    fixmul2_30(TO_FIX_8_24(wall->back->floor - height), z);
	       active->pstart2 =
		    fixmul2_30(TO_FIX_8_24(wall->back->ceiling - height), z);
	       active->pend2 =
		    fixmul2_30(TO_FIX_8_24(wall->front->ceiling - height), z);
	       active->dpstart1 =
		    fixmul2_30(TO_FIX_8_24(wall->front->floor - height), dz);
	       active->dpend1 =
		    fixmul2_30(TO_FIX_8_24(wall->back->floor - height), dz);
	       active->dpstart2 =
		    fixmul2_30(TO_FIX_8_24(wall->back->ceiling - height), dz);
	       active->dpend2 =
		    fixmul2_30(TO_FIX_8_24(wall->front->ceiling - height), dz);
	       active->visible = True;
	  }
	  cur_slice->pstart1 = FROM_FIX_8_24(active->pstart1);
	  cur_slice->pend1 = FROM_FIX_8_24(active->pend1);
	  cur_slice->pstart2 = FROM_FIX_8_24(active->pstart2);
	  cur_slice->pend2 = FROM_FIX_8_24(active->pend2);

	  if (active->is_back_view) {
	       cur_slice->front = wall->back;
	       cur_slice->back = wall->front;
	       cur_slice->pstart1 = cur_slice->pend1;
	       cur_slice->pend2 = cur_slice->pstart2;
	  } else {
	       cur_slice->front = wall->front;
	       cur_slice->back = wall->back;
	  }

	  cur_slice->top = top;
	  cur_slice->bottom = bottom;
	  cur_slice->wall = wall;
	  cur_slice->z = FROM_FIX_2_30(z);
	  cur_slice->is_back_view = active->is_back_view;

	  if (cur_slice->is_back_view) {
	       if (bottom < cur_slice->pstart1)
		    bottom = cur_slice->pstart1;
	       if (top > cur_slice->pend2)
		    top = cur_slice->pend2;
	  } else {
	       if (bottom < cur_slice->pend1)
		    bottom = cur_slice->pend1;
	       if (top > cur_slice->pstart2)
		    top = cur_slice->pstart2;
	  }

	  active++;
	  cur_slice++;
     }

     /* place the 'end of column' sentinel */
     cur_slice->wall = NULL;
     cur_slice->top = top;
     cur_slice->bottom = bottom;

     cur_slice++;

     return cur_slice;
}


static void draw_walls(Wall_intersection *int_list, View *v)
{
     int i = 0;
     int fb_column = VIEW_WIDTH - 1;
     unsigned int tex_column, tex_mask;
     unsigned char *tex_base;
     fixed tex_y, tex_dy;
     Pixel *fb_byte, *last_byte;
     fixed pstart1, pend1, pstart2, pend2;
     fixed start1, start2;
     fixed Vx, Vy, dVy;
     fixed height = v->height;


     Vx = v->eye_distance;
     Vy = -v->view_plane_size;
     dVy = fixdiv(FIXED_DOUBLE(v->view_plane_size), INT_TO_FIXED(VIEW_WIDTH));

     while (i < VIEW_WIDTH) {
	  Wall *wall = int_list->wall;
	  Texture *texture;
	  fixed z, t;
	  Boolean do_floor, do_ceiling;

	  if (wall == NULL) {
	       int_list++;
	       i++;
	       fb_column--;
	       Vy += dVy;
	       continue;
	  }

	  if (int_list->is_back_view) {
	       start1 = int_list->back->floor;
	       start2 = int_list->front->ceiling;
	       pstart1 = int_list->pend1;
	       pend1 = int_list->pstart1;
	       pstart2 = int_list->pend2;
	       pend2 = int_list->pstart2;
	  } else {
	       start1 = int_list->front->floor;
	       start2 = int_list->back->ceiling;
	       pstart1 = int_list->pstart1;
	       pend1 = int_list->pend1;
	       pstart2 = int_list->pstart2;
	       pend2 = int_list->pend2;
	  }
	  if (pend1 > pend2)
	       pend1 = pend2;
	  if (pstart2 < pstart1)
	       pstart2 = pstart1;


	  texture = wall->surface_texture;
	  z = int_list->z;
	  do_floor = (pstart1 < int_list->top) &&
	             (pend1 > int_list->bottom) &&
		     (pend1 - int_list->pstart1 > FIXED_EPSILON);
	  do_ceiling = (pstart2 < int_list->top) &&
	               (pend2 > int_list->bottom) &&
		       (pend2 - int_list->pstart2 > FIXED_EPSILON);

	  /* Don't do anything more with this wall if there's nothing
	  **   to draw.
	  */
	  if (!do_floor && !do_ceiling) {
	       int_list++;
	       continue;
	  }

	  if (wall->sky) {
	       fixed angle;

	       /* Compute the angle of this column.  We'll use the angle
	       **   exclusively to determine which texture column to
	       **   display for this slice of sky--that's the way sky
	       **   works.
	       */
	       angle = v->angle +
		    fixdiv(FIXED_SCALE(v->arc, i - (VIEW_WIDTH >> 1)),
			   INT_TO_FIXED(VIEW_WIDTH));
	       angle -= FIXED_SCALE(FIXED_2PI,
				    FIXED_TO_INT(fixdiv(angle, FIXED_2PI)));
	       angle = fixdiv(angle, FIXED_2PI);
	       if (angle < FIXED_ZERO)
		    angle = FIXED_ONE - angle;
	       tex_column = FIXED_TO_INT(fixmul(angle, wall->xscale)) &
		    (texture->width - 1);
	       tex_dy = fixdiv(wall->yscale, INT_TO_FIXED(VIEW_HEIGHT));
	  } else {
	       t = wall_ray_intersection(Vx, Vy, wall);
	       /* From t, calculate the integer coordinates in the texture
		**   bitmap.  For efficiency, we assume that the width of the
		**   texture is a power of two.
		*/
	       tex_column =
		    FIXED_TO_INT(wall->xphase + fixmul(t, wall->xscale)) &
			 (texture->width - 1);
	       /* Test to avoid overflow here . . .  if the wall is so far
		**   away that z (which is 1 / distance) is less than
		**   FIXED_EPSILON, then it will be so small when rendered
		**   that we can use a bogus value for tex_dy.
		*/
	       if (z < FIXED_EPSILON)
		    tex_dy = 0;
	       else
		    tex_dy = fixdiv(wall->yscale, FIXED_SCALE(z, VIEW_HEIGHT));
	  }
	  tex_mask = texture->height - 1;
	  tex_base = texture->texels + (tex_column << texture->log2width);

	  /* Now we can actually draw the walls, starting with
	  **   the floor segment.
	  */
	  if (do_floor) {
	       int fb_start, fb_end;

	       if (pstart1 < int_list->bottom) {
		    pstart1 = int_list->bottom;
		    start1 = fixdiv(pstart1, z) + height;
	       }
	       if (pend1 > int_list->top)
		    pend1 = int_list->top;

	       fb_start = FIXED_TO_INT(FIXED_SCALE(pstart1, VIEW_HEIGHT));
	       fb_end = FIXED_TO_INT(FIXED_SCALE(pend1, VIEW_HEIGHT));

	       fb_start = (VIEW_HEIGHT >> 1) - fb_start;
	       /* The following line of code is a hack . . . */
	       if (fb_start >= VIEW_HEIGHT)
			    fb_start = VIEW_HEIGHT - 1;
	       fb_end = (VIEW_HEIGHT >> 1) - fb_end;
				//well one hack deserves another! GCT 7/12/94.
	       if (fb_end >= VIEW_HEIGHT)
			    fb_end = VIEW_HEIGHT - 1;
	       fb_byte = fb->pixels + fb_column + fb_rows[fb_start];
	       last_byte = fb->pixels + fb_column + fb_rows[fb_end];
	       if (wall->sky)
		    tex_y = fixmul(view_constants.row_view[fb_start],
				   wall->yscale);
	       else
		    tex_y = fixmul(start1, wall->yscale) + wall->yphase;

	       draw_wall_slice(fb_byte, last_byte, tex_base,
			       tex_y, tex_dy, VIEW_WIDTH, texture->height,
			       fb_start - fb_end);
//UpdateWT(1);	//GCT diagnostic.
	  }

	  /* Draw the ceiling segment. */
	  if (do_ceiling) {
	       int fb_start, fb_end;

	       if (pstart2 < int_list->bottom) {
		    pstart2 = int_list->bottom;
		    start2 = fixdiv(pstart2, z) + height;
	       }
	       if (pend2 > int_list->top)
		    pend2 = int_list->top;

	       fb_start = FIXED_TO_INT(FIXED_SCALE(pstart2, VIEW_HEIGHT));
	       fb_end = FIXED_TO_INT(FIXED_SCALE(pend2, VIEW_HEIGHT));

	       fb_start = (VIEW_HEIGHT >> 1) - fb_start;
	       /* The following line of code is a hack . . . */
	       if (fb_start >= VIEW_HEIGHT)
			    fb_start = VIEW_HEIGHT - 1;
	       fb_end = (VIEW_HEIGHT >> 1) - fb_end;
				//well one hack deserves another! GCT 7/12/94.
	       if (fb_end >= VIEW_HEIGHT)
			    fb_end = VIEW_HEIGHT - 1;
	       fb_byte = fb->pixels + fb_column + fb_rows[fb_start];
	       last_byte = fb->pixels + fb_column + fb_rows[fb_end];
	       if (wall->sky)
		    tex_y = view_constants.row_view[fb_start];
	       else
		    tex_y = fixmul(start2, wall->yscale) + wall->yphase;

	       draw_wall_slice(fb_byte, last_byte, tex_base,
			       tex_y, tex_dy, VIEW_WIDTH, texture->height,
			       fb_start - fb_end);
//UpdateWT(1);	//GCT diagnostic.
	  }

	  int_list++;
     }
}


static void draw_floors(Wall_intersection *int_list, View *v)
{
     int i;
     Wall_intersection *last, *current;
     Wall_intersection *save_current, *save_last;
     fixed start, end;
     fixed max_floor, min_ceiling;
     fixed last_max_floor = FIXED_ZERO, last_min_ceiling = FIXED_ZERO;


     last = current = int_list;

     for (i = 1; i < VIEW_WIDTH; i++) {

		  for (; current->wall != NULL; current++);
		  save_current = ++current;
		  save_last = last;

		  /* get the highest floor and lowest ceiling */
		  while (current->wall != NULL)
	       current++;
		  max_floor = current->bottom;
		  min_ceiling = current->top;

		  current = save_current;

		  /* draw floors */
		  while (last->wall != NULL && current->wall != NULL) {

	       if (current->front == last->front)
			    start = MAX(last->bottom, current->pstart1);
	       else
			    start = MAX(last->bottom, current->bottom);

	       end = MIN(last->pstart1, current->pend1);

	       if (start < end)
			    draw_floor_slices(last->front, start, end, v, i - 1);

	       if (last->pend1 < current->pend1)
			    last++;
	       else
			    current++;
		  }
/*
		  while (last->wall != NULL) {
		       end = last->bottom;
		       start = MAX(min_ceiling, last->pstart1);
		       if (start < end)
				    draw_floor_slices(last->front, start, end, v, i - 1);
		       last++;
		  }
*/
		  /* draw ceiling slices generated by ceiling walls */
		  last = save_last;
		  current = save_current;
		  while (last->wall != NULL && current->wall != NULL) {

	       if (current->front == last->front)
			    end = MIN(last->top, current->pend2);
	       else
			    end = MIN(last->top, current->top);

	       start = MAX(last->pend2, current->pstart2);

	       if (start < end)
			    draw_ceiling_slices(last->front, start, end, v, i - 1);

	       if (last->pstart2 > current->pstart2)
			    last++;
	       else
			    current++;
		  }

		  /* draw ceiling slices generated by floor walls */
		  while (last->wall != NULL) {
		       start = MAX(last_max_floor, last->pend2);
		       end = MIN(max_floor, last->top);
		       if (start < end)
				    draw_ceiling_slices(last->front, start, end, v, i - 1);
		       last++;
		  }
		  last = save_current;

		  last_max_floor = max_floor;
		  last_min_ceiling = min_ceiling;
     }

     save_last = last;

     /* finish up the floors */
     while (last->wall != NULL) {

		  start = MAX(last->bottom, -FIXED_ONE_HALF);
		  end = MIN(last->pstart1, FIXED_ZERO);

		  if (start < end)
	       draw_floor_slices(last->front, start, end, v, i - 1);

		  last++;
     }

     /* finish up the ceilings */
     last = save_last;
     while (last->wall != NULL) {

		  start = MAX(last->pend2, FIXED_ZERO);
		  end = MIN(last->top, FIXED_ONE_HALF);

		  if (start < end)
	       draw_ceiling_slices(last->front, start, end, v, i - 1);

		  last++;
     }

}


static void draw_floor_slices(Region *r, fixed start, fixed end,
			      View *v, int column)
{
     int screen_column = VIEW_WIDTH - column - 1;
     int start_row, end_row;
     Texture *texture = r->floor_tex;
     fixed height;
     fixed sin_x, cos_x;


     start_row = FIXED_TO_INT(start * VIEW_HEIGHT);
     end_row = FIXED_TO_INT(end * VIEW_HEIGHT);
     /* Bail out early and save time if there's nothing to draw. */
     if (start_row == end_row)
		  return;

     start_row = (VIEW_HEIGHT >> 1) - start_row;
     end_row = (VIEW_HEIGHT >> 1) - end_row;
     if (start_row >= VIEW_HEIGHT)
		  start_row = VIEW_HEIGHT - 1;
     if (end_row < 0)
		  end_row = 0;

     height = r->floor - v->height;
     sin_x = view_constants.sin_tab[column];
     cos_x = view_constants.cos_tab[column];

     while (start_row >= end_row) {
		  fixed x, dx, y, dy;
		  fixed y1;
		  Pixel *fb_byte = fb->pixels + fb_rows[start_row] + screen_column;

		  if (FIXED_ABS(view_constants.row_view[start_row]) < FIXED_EPSILON)
	       y1 = FIXED_ONE;
		  else
	       y1 = fixdiv(height, view_constants.row_view[start_row]) << 4;

		  x = fixmul(view_constants.view_sin - cos_x, y1) - (v->y << 4);
		  y = fixmul(-view_constants.view_cos - sin_x, y1) - (v->x << 4);
		  dx = fixmul(view_constants.cos_dx, y1);
		  dy = fixmul(view_constants.sin_dx, y1);

		  draw_floor_slice(fb_byte, texture->texels, x, y, dx, dy,
			   texture->width);

		  start_row--;
//UpdateWT(1);	//GCT diagnostic.
     }
}


static void draw_ceiling_slices(Region *r, fixed start, fixed end,
				View *v, int column)
{
     int screen_column = VIEW_WIDTH - column - 1;
     int start_row, end_row;
     Texture *texture = r->ceiling_tex;
     fixed height;
     fixed sin_x, cos_x;


     start_row = FIXED_TO_INT(start * VIEW_HEIGHT);
     end_row = FIXED_TO_INT(end * VIEW_HEIGHT);
     /* Bail out early and save time if there's nothing to draw. */
     if (start_row == end_row)
		  return;

     start_row = (VIEW_HEIGHT >> 1) - start_row;
     end_row = (VIEW_HEIGHT >> 1) - end_row;
     if (start_row >= VIEW_HEIGHT)
		  start_row = VIEW_HEIGHT - 1;
     if (end_row < 0)
		  end_row = 0;

     height = r->ceiling - v->height;
     sin_x = view_constants.sin_tab[column];
     cos_x = view_constants.cos_tab[column];


     while (start_row >= end_row) {
		  fixed x, dx, y, dy;
		  fixed y1;
		  Pixel *fb_byte = fb->pixels + fb_rows[start_row] + screen_column;

		  if (FIXED_ABS(view_constants.row_view[start_row]) < FIXED_EPSILON)
	       y1 = FIXED_ONE;
		  else
	       y1 = fixdiv(height, view_constants.row_view[start_row]) << 4;

		  x = fixmul(view_constants.view_sin - cos_x, y1) - (v->y << 4);
		  y = fixmul(-view_constants.view_cos - sin_x, y1) - (v->x << 4);
		  dx = fixmul(view_constants.cos_dx, y1);
		  dy = fixmul(view_constants.sin_dx, y1);

		  draw_floor_slice(fb_byte, texture->texels, x, y, dx, dy,
			   texture->width);

		  start_row--;
//UpdateWT(1);	//GCT diagnostic.
     }
}


/* Calculate the value of the parameter t at the intersection
**   of the view ray and the wall.  t is 0 at the origin of
**   the wall, and 1 at the other endpoint.
*/
static fixed wall_ray_intersection(fixed Vx, fixed Vy, Wall *wall)
{
     fixed denominator, Nx, Ny, Wx, Wy;

     Nx = -Vy;
     Ny = Vx;
     Wx = wall->vertex2->tx - wall->vertex1->tx;
     Wy = wall->vertex2->ty - wall->vertex1->ty;

     denominator = fixmul(Nx, Wx) + fixmul(Ny, Wy); /* N dot W */
     if (denominator < FIXED_EPSILON)
	  return FIXED_ONE - fixdiv(fixmul(Nx, wall->vertex1->tx) +
				    fixmul(Ny, wall->vertex1->ty),
				    -denominator);
     else if (denominator > FIXED_EPSILON)
	  return fixdiv(fixmul(Nx, wall->vertex1->tx) +
			fixmul(Ny, wall->vertex1->ty),
			-denominator);
     else
	  return FIXED_ZERO;
}


static void init_buffers(void)
{
     int i;

     for (i = 0; i < VIEW_WIDTH + 1; i++) {
	  start_events[i].n_events = 0;
	  end_events[i].n_events = 0;
     }
}


/* Calculate values that are dependent only on the screen dimensions and
**   the view.
*/
static void calc_view_constants(View *v, int screen_width, int screen_height)
{
     static int last_height = 0, last_width = 0;
     int i;
     fixed x, y;


     /* Make sure that enough memory has been allocated for the tables. */
     if (screen_height != last_height) {
	  if (last_height == 0)
	       view_constants.row_view =
		    wtmalloc(screen_height * sizeof(fixed));
	  else
	       view_constants.row_view =
		    wtrealloc(view_constants.row_view,
			     screen_height * sizeof(fixed));
	  last_height = screen_height;
     }
     if (screen_width != last_width) {
	  if (last_width == 0) {
	       view_constants.sin_tab = wtmalloc(screen_width * sizeof(fixed));
	       view_constants.cos_tab = wtmalloc(screen_width * sizeof(fixed));
	  } else {
	       view_constants.sin_tab =
		    wtrealloc(view_constants.sin_tab,
			      screen_width * sizeof(fixed));
	       view_constants.cos_tab =
		    wtrealloc(view_constants.cos_tab,
			      screen_width * sizeof(fixed));
	  }
	  last_width = screen_width;
     }

     view_constants.view_sin =
	  FLOAT_TO_FIXED(sin(- FIXED_TO_FLOAT(v->angle)));
     view_constants.view_cos =
	  FLOAT_TO_FIXED(cos(- FIXED_TO_FLOAT(v->angle)));
     view_constants.screen_dx = fixdiv(FIXED_DOUBLE(v->view_plane_size),
				       INT_TO_FIXED(screen_width));
     view_constants.screen_dy = fixdiv(FIXED_ONE,
				       INT_TO_FIXED(screen_height));
     view_constants.sin_dx = fixmul(view_constants.view_sin,
				    view_constants.screen_dx);
     view_constants.cos_dx = fixmul(view_constants.view_cos,
				    view_constants.screen_dx);
     y = FIXED_SCALE(view_constants.sin_dx, -(screen_width >> 1));
     x = FIXED_SCALE(view_constants.cos_dx, -(screen_width >> 1));
     for (i = 0; i < screen_width; i++) {
	  view_constants.sin_tab[i] = y;
	  view_constants.cos_tab[i] = x;
	  y += view_constants.sin_dx;
	  x += view_constants.cos_dx;
     }

     y = FIXED_SCALE(view_constants.screen_dy, screen_height >> 1);
     for (i = 0; i < screen_height; i++) {
	  view_constants.row_view[i] = y;
	  y -= view_constants.screen_dy;
     }
}


