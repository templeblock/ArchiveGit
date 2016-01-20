#include "wgtgfx.h"

unsigned char far *textureimage;

/* This structure contains information about each scan line of the
   polygon. Each scan line can only have two intersections with
   the polygon, to keep things simple. */
struct {
   int startx;          /* The first X coord */
   int endx;            /* The last X coord */
   int x1;              /* First pixel coord out of texture */
   int y1;
   int x2;              /* Last pixel coord out of texture */
   int y2;
   } texturepoint[200];  /* One for each scan line (320x200x256) */

typedef struct
    {
     int x,y;
     int sx, sy;	/* Source coordinates from the texture image */
    } tpoint;


void tmapline (int x1, int tmapx1, int tmapy1, int x2, int tmapx2,
	       int tmapy2, int y)
/* Draws one scanline of the textured polygon.
   Where:
     x1 is the first x coordinate
     tmapx1 is the x coordinate on the texture relating to (x1,y)
     tmapy1 is the y coordinate on the texture relating to (x1,y)
     x2 is the second x coordinate
     tmapx2 is the x coordinate on the texture relating to (x2,y)
     tmapy2 is the y coordinate on the texture relating to (x2,y)
     y is the scanline to draw the line on
*/
{
long length;
long deltax, deltay;	    /* Difference between the x and y coordinates */

unsigned char far * dest;   /* Ptr to the screen */
unsigned char far * src;    /* Ptr to texture image */

int xincr; /* X offset into texture, amount to increase every pixel */
int yincr; /* Y offset into texture, amount to increase every pixel */

int xpos, ypos;   /* Stores fractional part after clipping */

int t;   /* Used for swapping */


 if (x1 > x2)		/* Swap all the coordinates so x1 < x2 */
  {
   t = x1; x1 = x2; x2 = t;
   t = tmapx1; tmapx1 = tmapx2; tmapx2 = t;
   t = tmapy1; tmapy1 = tmapy2; tmapy2 = t;
  }

 length = x2 - x1 + 1;		/* Calculate the length of this line */
 if (length > 0)
 {
  deltax = tmapx2 - tmapx1 + 1;	/* Find the delta values */
  deltay = tmapy2 - tmapy1 + 1;

  dest = abuf + y * 320 + x1;  /* Make a pointer to the correct place in
				  video memory */
  src  = textureimage + tmapy1 * 256 + tmapx1;
	/* Make a pointer to the correct place on the texture map
	   Assumes the image is 256 in width. */

    xincr = ((long)(deltax) << 8) / (long)length;
    yincr = ((long)(deltay) << 8) / (long)length;

    /* Calculate the step value for the x and y coordinates.
       For every pixel on the destination, the x coordinate on the texture
       will move xincr pixels. */

 xpos = tmapx1<<8;
 ypos = tmapy1<<8;

 asm {
       .386
       push ds
       cld
       mov cx, word ptr length		/* Set length */
       shr cx, 1
       les di, dest			/* Set destination ptr */
       lds si, src			/* Set source ptr */
       mov dx, word ptr ypos		/* Put the y in the low word */
       shl edx, 16                      /* Move the y to the high word */
       mov dx, word ptr xpos		/* Put the x in the low word */

       mov si, word ptr yincr		/* Set up the increments the  */
       shl esi, 16			/* same way */
       mov si, word ptr xincr
      /* Now to advance one pixel, we can add edx and esi together to
	 advance the x and y at the same time, with the fractional
	 portion automatically carrying at 256. */

      cmp cx, 0
      je onepixel
      }
  tlineloop:
  ;
  asm {
       mov ebx, edx
       shr ebx, 16      /* BH now contains the y coordinate */
       mov bl, dh	/* Store the x value in BL, */
			/* BX is now an offset into the texture image,
			   between 0 and 65535. */
       mov al, ds:[bx]	/* Get the color from the texture image */
       add edx, esi	/* Advance one pixel */

       mov ebx, edx	/* Repeat the above, and get another pixel */
       shr ebx, 16
       mov bl, dh
       mov ah, ds:[bx]
       add edx, esi

       stosw		/* Store a word to the destination */
       dec cx		/* Decrease length */
       jnz tlineloop	/* Repeat for all pixels */
      }
  onepixel:
   asm {
       mov cx, word ptr length
       and cx, 1
       jz tlinedone

       mov ebx, edx
       shr ebx, 16      /* BH now contains the y coordinate */
       mov bl, dh	/* Store the x value in BL, */
			/* BX is now an offset into the texture image,
			   between 0 and 65535. */
       mov al, ds:[bx]	/* Get the color from the texture image */
       mov es:[di], al

    }
   tlinedone:
   asm {
       pop ds
      }
  }
}



void tpolyline (int x1, int y1, int tx1, int ty1, int x2, int y2,
		int tx2, int ty2)
/* Calculates the coordinates of a line given two
   vertices, (x1,y1) with texture coordinates (tx1, ty1),  and
   (x2, y2) with texture coordinates (tx2,ty2).

   We will use fixed point math to speed things up.
   The x coordinate is multiplied by 256 and for each row,
   a constant m is added to x.  This is a simplified version
   of a line algorithm because we only have to store 1 x coordinate
   for every y coordinate.

   The texture coordinates increase by a step value based on the
   number of pixels between the texture coordinates and the distance between
   the screen y coordinates.

*/
{
 int tmp,y;
 long x,m;

 long xcoord, xstep;   /* X texture coordinate, and step value */
 long ycoord, ystep;   /* Y texture coordinate, and step value */

 if (y2 != y1)         /* This isn't a horizontal line */
 {
   if (y2 < y1)        /* Make sure y2 is greater than y1 */
   {
    tmp = y1;          /* Swap the y coordinate */
    y1 = y2;
    y2 = tmp;

    tmp = x1;          /* Swap the corresponding x coordinates */
    x1 = x2;
    x2 = tmp;

    tmp = tx1;        /* Swap the corresponding x values */
    tx1 = tx2;
    tx2 = tmp;

    tmp = ty1;        /* Swap the corresponding y values */
    ty1 = ty2;
    ty2 = tmp;
   }

 x = (long)x1<<8;  /* Multiply by 256 */

 m = ((long)(x2 - x1)<<8) / ((long)(y2 - y1));
 /* m is the fractional amount to add to the x coordinate every row.
    m is equal to (delta x) / (delta y).  In other words, the x coordinate
    has to change by (x2 - x1) columns in (y2 - y1) rows. */

 xcoord = (long)tx1 << 8;   /* Initial x coord in 8.8 fixed point format */
 xstep = ((long)(tx2 - tx1) << 8) / ((long)(y2 - y1));
 /* Calculate the x step value */

 ycoord = (long)ty1 << 8;   /* Initial y coord in 8.8 fixed point format */
 ystep = ((long)(ty2 - ty1) << 8) / ((long)(y2 - y1));
 /* Calculate the y step value */

 x += m; /* We ALWAYS skip the first point in every line. This is done */
 y1++; /* because we do not want to store the point where two lines
	  meet, twice.  This would result in a single point being drawn. */


 for (y = y1; y <= y2; y++) /* Go through each row */
  {
   if ((y >= 0) & (y < 200)) /* If the coordinate is on the screen */
    if (texturepoint[y].startx == -16000) /* Store the first coordinate */
      {
       texturepoint[y].startx = x >> 8;  /* Store the first coordinate */
       texturepoint[y].x1 = xcoord >> 8;
       texturepoint[y].y1 = ycoord >> 8;
      }
    else
      {
       texturepoint[y].endx = x >> 8;  /* Store the last coordinate */
       texturepoint[y].x2 = xcoord >> 8;
       texturepoint[y].y2 = ycoord >> 8;
      }
   x += m;		     /* Add our constant to x */
   xcoord += xstep;	     /* Add our x step value to the texture */
   ycoord += ystep;          /* Add our y step value to the texture */
   }
 }
}


void texturedpoly (tpoint *vertexlist, int numvertex)
/* Draws a shaded polygon given an array of vertices. */
{
int i;
tpoint *curpt,*nextpt;
  /* Two pointers to a vertex. These are used to connect to vertices
     together in when calling the gpolyline routine. */

 curpt = vertexlist;      /* Set to the first vertex in the array */
 nextpt = vertexlist + 1; /* and to the second vertex */

 for (i = 0; i < 200; i++)
  {
   texturepoint[i].startx = -16000;     /* Set up our impossible values */
   texturepoint[i].endx = -16000;
  }

 for (i = 1; i < numvertex; i++)
  {
   tpolyline (curpt->x,  curpt->y,  curpt->sx, curpt->sy,
	      nextpt->x, nextpt->y, nextpt->sx, nextpt->sy);
   /* Calculate the edge of this line. */

   curpt += 1;  /* Go to the next line */
   nextpt += 1;
  }

  nextpt = vertexlist;  /* Now close the polygon by doing a line between
			   the first and last vertex. */
  tpolyline (curpt->x,  curpt->y,  curpt->sx, curpt->sy,
	     nextpt->x, nextpt->y, nextpt->sx, nextpt->sy);

  for (i = 0; i < 200; i++)   /* Now draw the horizontal line list */
    if (texturepoint[i].startx != -16000)  /* Indicates there is a line on this row */
    {
     if (texturepoint[i].endx == -16000)
	 texturepoint[i].endx = texturepoint[i].startx;
	 /* In case there was only one point found on this row */
       tmapline (texturepoint[i].startx,
		 texturepoint[i].x1, texturepoint[i].y1,
		 texturepoint[i].endx,
		 texturepoint[i].x2, texturepoint[i].y2,
		 i);
       /* Draw a shaded line between the two x coordinates, on the row i. */
    }
}
