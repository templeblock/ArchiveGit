#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "wt.h"
#include "fixed.h"
#include "wtmem.h"
#include "table.h"
#include "view.h"
#include "texture.h"
#include "framebuf.h"
#include "graphics.h"
#include "world.h"
#include "render.h" 

extern fixed view_height;

#define	DONT_INTERSECT    0
#define	DO_INTERSECT      1
#define COLLINEAR         2

#define FIXED_ONE_QUARTER (1 << 14)
#define MAN_SIZE          (FIXED_ONE+FIXED_ONE_HALF)

static Wall   *s_wall = NULL;
static double  side   = 0.0,
                  x   = 0.0,
                  y   = 0.0;


#define VECTOR_LEN  1.2

/*
-- ----------------------------------------------------------------------
-  Function    : LineDistance
-
-  Parameters  :
-
-  Result      :
-
-  Description :
-    Returns the distance between point c, and the line a-b.
-- ----------------------------------------------------------------------
*/
double
LineDistance( double cx, double cy, double ax, double ay, double bx, double by )
{
  double delta_x = bx-ax;
  double delta_y = by-ay;
  double line_len, r, s;

  /* the lenght of the line in **2 */
  line_len = delta_x*delta_x+delta_y*delta_y;

  r = ((ay-cy)*(-delta_y)-(ax-cx)*delta_x)/line_len;


  /* If s becomes negative we are facing the line clockwise */
  s = ((ay-cy)*(delta_x)-(ax-cx)*delta_y)/sqrt( line_len );


/*
  printf( "R = %f S = %f\n", r, s );
*/

  if ( r < 0.0 || r > 1 )
    return 512.0;

  return s;
}


void
get_vector_xy( double *vx, double *vy, Wall *wall )
{
  fixed  fdx, fdy;
  double dx, dy, line_len;

  fdx = wall->vertex2->x - wall->vertex1->x;
  fdy = wall->vertex2->y - wall->vertex1->y;

  dx = (double)FIXED_TO_FLOAT(fdx);
  dy = (double)FIXED_TO_FLOAT(fdy);

  line_len = sqrt( dx*dx + dy*dy );

  *vx = dx/line_len;
  *vy = dy/line_len;

/*
  printf( "line_len %f, dx %f, dy %f\n", 
	 line_len, dx, dy );
*/
}

/*
  Check how it should behave when you sliding agains a wall. 
*/

void
slide_wall( double *x, double *y, double x2, double y2,
		  double dist_old, double dist_new )
{
  double procent, line_len;
  double vector_x, vector_y;
  double dx = x2-*x;
  double dy = y2-*y;
  double suck1, suck2;

 
  line_len = sqrt( dx*dx + dy*dy );

  /* (dist_old-dist_new) becomes one side in a triangle. */
  /* procent can not be over 1.0. */
  /* How long is the distance towards the line compared to the line_length. */
  procent = (1.0 - fabs( dist_old - dist_new )/line_len);

  get_vector_xy( &vector_x, &vector_y, s_wall );

/*
  printf( "procent %f, line_len %f, vector_x %f, vector_y %f\n", 
	 procent, line_len, vector_x, vector_y );
*/

  /* in what direktion are the vection compared to the walk ? */
  suck1 = (vector_x+dx)*(vector_x+dx) + (vector_y+dy)*(vector_y+dy);
  suck2 = (vector_x-dx)*(vector_x-dx) + (vector_y-dy)*(vector_y-dy);

  if ( suck1 > suck2 )
    {
      *x  += procent*line_len*vector_x;
      *y  += procent*line_len*vector_y;
    }
  else
    {
      *x  -= procent*line_len*vector_x;
      *y  -= procent*line_len*vector_y;
    }

}


int
checkwalls( World *w, View *v  )
{
  extern fixed view_height;
  Wall   *wall = (Wall *) w->walls->table, *mem_wall;
  double  x1, y1, tmp, min = 512.0;
  int i;

  x1 = FIXED_TO_FLOAT(v->x);
  y1 = FIXED_TO_FLOAT(v->y);

  if ( x1 == x && y1 == y )
    return DONT_INTERSECT;

  for (i = 0; i < TABLE_SIZE(w->walls); i++, wall++ ) {

    /* Check Distance */
    tmp = LineDistance( x1, y1, 
	    FIXED_TO_FLOAT(wall->vertex1->x ), 
	    FIXED_TO_FLOAT(wall->vertex1->y ), 
	    FIXED_TO_FLOAT(wall->vertex2->x ), 
	    FIXED_TO_FLOAT(wall->vertex2->y ) );

    if ( fabs(tmp) < fabs( min ) )
      {
	min      = tmp;
	mem_wall = wall; 
      }

  }

  /* is there any point in checking further. */
  if ( fabs( min ) < VECTOR_LEN  )  
    {
      /* start examine the wall */
      fixed ceiling, floor;

      /* Check the outher side of the object */
      if ( min > 0.0 )
	{
	  floor      = mem_wall->front->floor;
	  ceiling    = mem_wall->front->ceiling;
	}
      else
	{
	  floor      = mem_wall->back->floor;
	  ceiling    = mem_wall->back->ceiling;
	}

      /* if the stair is less than FIXED_ONE_QUARTER and the
	 head has goes free and there are enought room. */
      if ( FIXED_ONE_QUARTER >= (floor-v->height) && ( ceiling > v->height ) &&
	  ( MAN_SIZE <= (ceiling-floor) ) ) {
	if ( s_wall != mem_wall || ( min > 0.0 && side <= 0.0 ) ||
	    (min <= 0.0 && side > 0.0 ) )
	  {
	    if ( min < 0.0 )
	      view_height = mem_wall->front->floor+MAN_SIZE;
	    else
	      view_height = mem_wall->back->floor+MAN_SIZE;

	    s_wall       = mem_wall;
	  }
        }
      else
	/* am i running towards the wall? */
	if ( fabs( min ) < fabs( side ) )
	  {

	     s_wall = mem_wall;
	     slide_wall( &x, &y, x1, y1, side, min );

	     v->x   = FLOAT_TO_FIXED( x );
	     v->y   = FLOAT_TO_FIXED( y );

	     return 0;
	  }  
    }

  side = min;
  x    = x1;
  y    = y1;

  return 0;
}

