#include <dos.h>
#include <stdlib.h>
#include <conio.h>
#include "wgtgfx.h"

int startx[200];
int endx[200];
int startcol[200];
int endcol[200];

typedef struct
    {
     int x,y;
     unsigned char col;
    } gpoint;


void shadedline (int x1, int firstcolor, int x2, int lastcolor, int y)
{
int length;
int numcolors;
int colorvalue;
int step;
int x;
unsigned char far * dest;   /* Ptr to the screen */

 length = x2 - x1 + 1;
 if (length > 0)
 {
  numcolors = lastcolor - firstcolor + 1;

  colorvalue = firstcolor << 8;
  step = ((long)numcolors << 8) / (long)length;

  dest = abuf + y * 320 + x1;
  /* Make a pointer to the first pixel */


  /* Begin assembly optimization */
  if (length > 0)
   {
    asm {
	mov cx, word ptr length		/* Set length */
	les di, dest			/* Set destination ptr */
	mov ax, word ptr colorvalue	/* Set color */
       }
    shadedlineloop:
    ;
    asm {
	mov es:di, ah			/* Move color to screen */
	add ax, word ptr step  		/* Add to color */
	inc di				/* Move to next pixel */
	dec cx				/* Decrease length */
	jnz shadedlineloop		/* Repeat for all pixels */
	}
   }
 }
}


void gpolyline (int x1, int y1, int col1, int x2, int y2, int col2)
/* Calculates the coordinates of a line given two
   vertices (x1,y1) with color col1,  and (x2,y2) with color col2.

   We will use fixed point math to speed things up.
   The x coordinate is multiplied by 256 and for each row,
   a constant m is added to x.  This is a simplified version
   of a line algorithm because we only have to store 1 x coordinate
   for every y coordinate.

   The color value is increase by a step value based on the
   number of colors between the vertices and the distance between the
   y coordinates.

*/
{
 int tmp,y;
 long x,m;
 long col, colstep;   /* First color, and the step value */

 if (y2 != y1)      /* This isn't a horizontal line */
 {
   if (y2 < y1)    /* Make sure y2 is greater than y1 */
   {
    tmp = y1;      /* Swap the y coordinate */
    y1 = y2;
    y2 = tmp;

    tmp = x1;      /* Swap the corresponding x coordinates */
    x1 = x2;
    x2 = tmp;

    tmp = col1;    /* Swap the corresponding color values */
    col1 = col2;
    col2 = tmp;
   }

 x = (long)x1<<8;  /* Multiply be 256 */

 m = ((long)(x2 - x1)<<8) / ((long)(y2 - y1));
 /* m is the fractional amount to add to the x coordinate every row.
    m is equal to (delta x) / (delta y).  In other words, the x coordinate
    has to change by (x2 - x1) columns in (y2 - y1) rows. */

 col = (long)col1 << 8;	 /* Initial color in 8.8 fixed point format */
 colstep = ((long)(col2 - col1) << 8) / ((long)(y2 - y1));
 /* Calculate the color step value similar to the method in the
    shadedline routine, only we're dividing by the delta y value. */


 x += m; /* We ALWAYS skip the first point in every line. This is done */
 y1++; /* because we do not want to store the point where two lines
	  meet, twice.  This would result in a single point being drawn. */

 for (y = y1; y <= y2; y++) /* Go through each row */
  {
   if ((y >= 0) & (y < 200)) /* If the coordinate is on the screen */
    if (startx[y] == -16000) /* Store the first coordinate */
      {
       startx[y] = x>>8;
       startcol[y] = col >> 8;	/* store the color */
      }
    else
      {
       endx[y] = x>>8;        /* Store the last coordinate */
       endcol[y] = col >> 8;
      }
   x += m;		     /* Add our constant to x */
   col += colstep;
   }
 }
}


void shadedpoly (gpoint *vertexlist, int numvertex)
/* Draws a shaded polygon given an array of vertices. */
{
int i;
gpoint *curpt,*nextpt;
  /* Two pointers to a vertex. These are used to connect to vertices
     together in when calling the gpolyline routine. */

 curpt = vertexlist;      /* Set to the first vertex in the array */
 nextpt = vertexlist + 1; /* and to the second vertex */

 for (i = 0; i < 200; i++)
  {
   startx[i] = -16000;     /* Set up our impossible values */
   endx[i] = -16000;
  }

 for (i = 1; i < numvertex; i++)
  {
   gpolyline (curpt->x,  curpt->y,  curpt->col,
	      nextpt->x, nextpt->y, nextpt->col);
   /* Calculate the edge of this line. */

   curpt += 1;  /* Go to the next line */
   nextpt += 1;
  }

  nextpt = vertexlist;  /* Now close the polygon by doing a line between
			   the first and last vertex. */
  gpolyline (curpt->x,  curpt->y,  curpt->col,
	     nextpt->x, nextpt->y, nextpt->col);

  for (i = 0; i < 200; i++)   /* Now draw the horizontal line list */
    if (startx[i] != -16000)  /* Indicates there is a line on this row */
    {
     if (endx[i] == -16000)
	 endx[i] = startx[i]; /* In case there was only one
				 point found on this row */
       shadedline (startx[i], startcol[i], endx[i], endcol[i], i);
       /* Draw a shaded line between the two x coordinates, on the row i. */
    }
}



gpoint mypoints[10];

void main (void)
{
int i;

 setvga256 ();

 do
 {
   for (i = 0; i < 3; i++)
    {
     mypoints[i].x = rand () % 320;
     mypoints[i].y = rand () % 200;
     mypoints[i].col = rand () % 256;
    }

   shadedpoly (&mypoints, 3);
 } while (!kbhit());

 getch ();
 setvideomode (0x03);  /* 80 column text mode */
}

