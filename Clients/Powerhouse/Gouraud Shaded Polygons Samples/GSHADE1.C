#include "wgtgfx.h"
#include <time.h>


void shadedline (int x1, int firstcolor, int x2, int lastcolor, int y)
{
int length;
int numcolors;
int colorvalue;
int step;
int x;

 length = x2 - x1 + 1;
 if (length > 0)
 {
  numcolors = lastcolor - firstcolor + 1;

  colorvalue = firstcolor << 8;
  step = ((long)numcolors << 8) / (long)length;
  for (x = x1; x <= x2; x++)
    {
     drawpixel (x, y, colorvalue >> 8);
     colorvalue += step;
    }
 }
}


void main (void)
{
int i;
unsigned char col;

 setvga256 ();

 do {
 col = rand () % 256;  /* Pick a random color for the right endpoint */
		       /* This will let us see how fast the screen is
			  being updated. You may want to take the turbo
			  button off for a while to see what speed it
			  runs at. */
 for (i = 0; i < 200; i++)
    shadedline (0, 0, 319 - i, col, i);

 } while (!kbhit ());

 getch ();
 setvideomode (3);

}