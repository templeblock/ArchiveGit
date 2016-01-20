#include <dos.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "wgtgfx.h"
#include "textmap.h"

unsigned char far thepic[65535];        /* Holds the 256x256 texture image */
color pal[256];                         /* 256 RGB values for the palette */

tpoint mypoly[6] = { /* Set up vertices */
	0, 0, 0, 0,
	319, 0, 255, 0,
	319, 199, 255, 255,
	0, 199, 0, 255};

tpoint rotatepoly[6];  /* Make another polygon structure to store the
			  rotated values */

int isin[360];         /* A sin table multiplied by 256 */
int icos[360];         /* A cos table multiplied by 256 */

int distancectr = 0;   /* Used for zooming in and out */
int distancedir = 10;  /* Direction and speed for zooming */
int rotatectr = 0;     /* Rotation for the polygon */


void load_pic (void)
/* Loads a raw picture format created with the picconv utility. */
{
FILE *fp;

 fp = fopen ("tmap.raw", "rb");
 fread (pal, 1, 768 , fp);              /* Load the palette */
 fread (thepic, 1, 65535, fp);          /* Load the picture */
 fclose (fp);

 setpalette (0, 255, pal);              /* Set to the correct palette */
}


void rotate_polygon_source (int rot, int distancectr)
/* Rotates the 4 corners of the texture source points, given the rotational
   amount and distance factor. */
{
int vertex;
int x, y;
int x2, y2;

 /* Set the unrotated source coordinates based on the zoom factor */
 /* A zoom factor of 0 would produce the coordinates
    (0,0),(255,0),(255,255),(0,255) */
 rotatepoly[0].sx = -distancectr;
 rotatepoly[0].sy = -distancectr;
 rotatepoly[1].sx = 255 + distancectr;
 rotatepoly[1].sy = -distancectr;
 rotatepoly[2].sx = 255 + distancectr;
 rotatepoly[2].sy = 255 + distancectr;
 rotatepoly[3].sx = -distancectr;
 rotatepoly[3].sy = 255 + distancectr;

 for (vertex = 0; vertex < 4; vertex++)
  {
   x = rotatepoly[vertex].sx - 160;/* Subtract the centers */
   y = rotatepoly[vertex].sy - 100;/* so coordinates are based around (0,0) */

   x2 = ((long)x * icos[rot] - (long)y * isin[rot]) >> 8;
   y2 = ((long)x * isin[rot] + (long)y * icos[rot]) >> 8;
   /* Apply the 2D rotation */

   rotatepoly[vertex].sx = x2 + 160;
   rotatepoly[vertex].sy = y2 + 100;
   /* Add the centers back on */
  }
}


void rotate_polygon_dest (int rot)
/* Rotates the 4 vertices of the polygon, given the rotational
   amount and distance factor. */
{
int vertex;
int x, y;
int x2, y2;

 for (vertex = 0; vertex < 4; vertex++)
  {
   x = mypoly[vertex].x - 160;  /* Subtract the centers */
   y = mypoly[vertex].y - 100;  /* so coordinates are based around (0,0) */

   x2 = ((long)x * icos[rot] - (long)y * isin[rot]) >> 8;
   y2 = ((long)x * isin[rot] + (long)y * icos[rot]) >> 8;
   /* Apply the 2D rotation */

   rotatepoly[vertex].x = x2 + 160;
   rotatepoly[vertex].y = y2 + 100;
   /* Add the centers back on */
  }
}



void main (void)
{
int i;
int ctr, ctr2;

 setvga256 ();                  /* Initialize 320x200x256 mode */
 load_pic ();                   /* Load our texture */
 textureimage = thepic;         /* Make pointer to the textre image */

 randomize ();

 /* Build the sin and cos tables */
 for (i = 0; i < 360; i++)
 {
  isin[i] = sin(3.1415 * ((double)i / 180.0)) * 256;
  icos[i] = cos(3.1415 * ((double)i / 180.0)) * 256;
 }


  /* Duplicate the screen coordinates of the polygon since these will remain
    the same.  We are only rotating the texture coordinates. */
 for (i = 0; i < 4; i++)
  {
   rotatepoly[i].x = mypoly[i].x;
   rotatepoly[i].y = mypoly[i].y;
  }


 /* First texture mapping loop.  Fills the screen with a polygon which
    has rotating and zooming texture coordinates. */
 do
  {
   rotate_polygon_source (rotatectr, distancectr);
   texturedpoly (rotatepoly, 4);

   rotatectr += 3;                      /* Increase the rotation amount */
   rotatectr = rotatectr % 360;         /* Wrap around at 360 */

   distancectr += distancedir;          /* Modify the zoom factor */
   if (distancectr > 256 * 8)           /* Change direction */
     distancedir = -10;
   if (distancectr < -256 * 4)          /* Change direction */
     distancedir = 10;

 } while (!kbhit());
 getch ();


/* The second texture mapping loop picks 4 random texture coordinates, and
   and moves the current coordinates towards them.  Once the coordinate has
   been reached, a new random coordinate is picked. */
 for (i = 0; i < 4; i++)
  {
   mypoly[i].sx = (rand () % 256) * 20 - 2560;
   mypoly[i].sy = (rand () % 256) * 20 - 2560;
  }
 /* rotatepoly holds the current texture coordinates.
    mypoly holds the the coordinate that we want to move towards. */

 rotatepoly[0].sx = 0;
 rotatepoly[0].sy = 0;
 rotatepoly[1].sx = 240;
 rotatepoly[1].sy = 0;
 rotatepoly[2].sx = 240;
 rotatepoly[2].sy = 240;
 rotatepoly[3].sx = 0;
 rotatepoly[3].sy = 240;
 /* Set up the original texture coordinates */
 /* The polygon is already set up for a full screen rectangle. */

 do
  {
   for (i = 0; i < 4; i++)
    /* Move each vertex */
    {
     /* Move coordinates towards destination */
     if (rotatepoly[i].sx < mypoly[i].sx)
	 rotatepoly[i].sx += 20;
     if (rotatepoly[i].sx > mypoly[i].sx)
	 rotatepoly[i].sx -= 20;
     if (rotatepoly[i].sy < mypoly[i].sy)
	 rotatepoly[i].sy += 20;
     if (rotatepoly[i].sy > mypoly[i].sy)
	 rotatepoly[i].sy -= 20;

     /* The coordinate has reached the destination, so pick a new one */
     if ((rotatepoly[i].sx == mypoly[i].sx) &&
	 (rotatepoly[i].sy == mypoly[i].sy))
     {
       mypoly[i].sx = (rand () % 256) * 20 - 2560;
       mypoly[i].sy = (rand () % 256) * 20 - 2560;
     }

    }
   texturedpoly (rotatepoly, 4);
 } while (!kbhit());
 getch ();



 mypoly[0].x = 96;
 mypoly[0].y = 36;
 mypoly[1].x = 224;
 mypoly[1].y = 36;
 mypoly[2].x = 224;
 mypoly[2].y = 164;
 mypoly[3].x = 96;
 mypoly[3].y = 164;
 /* Make a 128x128 square polygon */

 rotatepoly[0].sx = 0;
 rotatepoly[0].sy = 0;
 rotatepoly[1].sx = 255;
 rotatepoly[1].sy = 0;
 rotatepoly[2].sx = 255;
 rotatepoly[2].sy = 255;
 rotatepoly[3].sx = 0;
 rotatepoly[3].sy = 255;
 /* Set up the texture coordinates */

 do
  {
   rotate_polygon_dest (rotatectr);

   rotatectr += 5;
   rotatectr = rotatectr % 360;

   texturedpoly (rotatepoly, 4);
 } while (!kbhit());
 getch ();



/* The third loop makes a 6 sides polygon, and squishes the middle
   points inwards. */

 /* Set up 6 vertices, 3 along each side of the screen */
 mypoly[0].x = 0;       /* Top left */
 mypoly[0].y = 0;
 mypoly[1].x = 319;     /* Top right */
 mypoly[1].y = 0;
 mypoly[2].x = 319;     /* Middle right */
 mypoly[2].y = 99;
 mypoly[3].x = 319;     /* Bottom right */
 mypoly[3].y = 199;
 mypoly[4].x = 0;       /* Bottom left */
 mypoly[4].y = 199;
 mypoly[5].x = 0;       /* Middle left */
 mypoly[5].y = 99;

 /* Set up the source texture coordinates, using the same places but on
   the 256x256 bitmap */
 mypoly[0].sx = 0;       /* Top left */
 mypoly[0].sy = 0;
 mypoly[1].sx = 255;     /* Top right */
 mypoly[1].sy = 0;
 mypoly[2].sx = 255;     /* Middle right */
 mypoly[2].sy = 127;
 mypoly[3].sx = 255;     /* Bottom right */
 mypoly[3].sy = 255;
 mypoly[4].sx = 0;       /* Bottom left */
 mypoly[4].sy = 255;
 mypoly[5].sx = 0;       /* Middle left */
 mypoly[5].sy = 127;


 distancectr = 0;
 distancedir = 4;
 do {
   distancectr += distancedir;
   if (distancectr > 155 )
     distancedir = -4;
   if (distancectr < 4)
     distancedir = 4;

   /* Move the middle points inwards */
   mypoly[5].x = distancectr;
   mypoly[2].x = 319 - distancectr;

   texturedpoly (mypoly, 6);
 } while (!kbhit());
 getch ();


 setvideomode (0x03);  /* 80 column text mode */
}


