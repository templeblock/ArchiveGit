#include <dos.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include "wgtgfx.h"
#include "textmap.h"

unsigned char far thepic[65535];        /* Holds the 256x256 texture image */
color pal[256];                         /* 256 RGB values for the palette */

 /* This example draws four triangles on the screen, set up as shown:
       -------     The + shows the middle point that all four triangles
       |\   /|     share.  We will keep the outside vertices stationary
       | \ / |     and move the middle point around a circular path.
       |  +  |
       | / \ |
       |/   \|
       -------
 */

tpoint mypoly[4][3] = { /* Set up 3 vertices in 4 triangles */
	{96, 36,   0,   0,
	159, 99, 127, 127,
	224, 36, 255,   0},

       {224, 36, 255,   0,
	159, 99, 127, 127,
	224,164, 255, 255},

       {224,164, 255, 255,
	159, 99, 127, 127,
	 96,164,   0, 255},

	{96,164,   0, 255,
	159, 99, 127, 127,
	 96, 36,   0,   0}};

int isin[360];         /* A sin table multiplied by 256 */
int icos[360];         /* A cos table multiplied by 256 */

int rotatectr = 0;     /* Degrees of rotation for middle point */


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

void main (void)
{
int i;
int x, y;

 setvga256 ();                  /* Initialize 320x200x256 mode */
 load_pic ();                   /* Load our texture */
 textureimage = thepic;         /* Make pointer to the textre image */

 /* Build the sin and cos tables */
 for (i = 0; i < 360; i++)
 {
  isin[i] = sin(3.1415 * ((double)i / 180.0)) * 256;
  icos[i] = cos(3.1415 * ((double)i / 180.0)) * 256;
 }



 do
  {
   texturedpoly (mypoly[0], 3); /* Draw each triangle */
   texturedpoly (mypoly[1], 3);
   texturedpoly (mypoly[2], 3);
   texturedpoly (mypoly[3], 3);

   rotatectr += 7;
   rotatectr = rotatectr % 360;

   /* Calculate the center coordinate */
   x = 160 + icos[rotatectr] * 20 / 256;
   y = 100 + isin[rotatectr] * 20 / 256;

   for (i = 0; i < 4; i++)
    {
     mypoly[i][1].x = x;        /* Set coordinate #1 in each triangle */
     mypoly[i][1].y = y;
    }
 } while (!kbhit());
 getch ();

 setvideomode (0x03);  /* 80 column text mode */
}


