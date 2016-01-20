/* Graphics support routines for this tutorial */

/* A single palette register definition */
typedef struct 
 {
  unsigned char r,g,b;
 } color;



extern unsigned char far *abuf;   		/* A000 buffer */
extern int tx, ty, bx, by;   	/* Clipping variables, top and bottom */
extern unsigned char currentcolor;

extern void setvideomode (int mode);
extern void setclipping (int x1, int y1, int x2, int y2);
extern void drawcolor (unsigned char col);
extern void setvga256 (void);
extern void horizontal_line (int x1, int x2, int y);
extern void drawpixel (int x, int y, unsigned char col);
extern void setpalette (unsigned char start, unsigned char finish, color *pal);

