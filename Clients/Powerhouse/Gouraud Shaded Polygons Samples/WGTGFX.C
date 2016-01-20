#include <dos.h>

/* Contains various graphics support routines for the WGT tutorials. */

unsigned char far *abuf;   		/* A000 buffer */
int tx, ty, bx, by;   	/* Clipping variables, top and bottom */
unsigned char currentcolor;

void setvideomode (int mode)
{
  struct REGPACK reg;
  
  reg.r_ax = mode;
  intr (0x10, &reg);
}


void setclipping (int x1, int y1, int x2, int y2)
{
 tx = x1;
 ty = y1;
 bx = x2;
 by = y2;
}


void drawcolor (unsigned char col)
{
 currentcolor = col;
}

void setvga256 (void)
{
 setvideomode (0x13);
 abuf = MK_FP (0xA000, 0x0000);     /* Make pointer to visual screen */

 setclipping (0, 0, 319, 199);
}


void horizontal_line (int x1, int x2, int y)
{
int temp;
int length;  /* Length of the line */
unsigned char far *ptr;

 if (x1 > x2)  /* Swap x coords */
  {
   temp = x1;
   x1 = x2;
   x2 = temp;
  }

  if (x1 < tx)
      x1 = tx;   /* Clip the left edge */
  if (x2 > bx)
      x2 = bx;  /* Clip the left edge */

  length = x2 - x1 + 1;
  if (length > 0)
     memset (&abuf[y*320 + x1], currentcolor, length);
}


void drawpixel (int x, int y, unsigned char col)
{
 abuf [y * 320 + x] = col;
}



