/* ----------------------------------------------------------------------- */
/* raycast.h      (C) Copyright 
 *
 * Description:
 * 	Header file for the ray-casting engine
 * 	
 * This module belongs to the project:
 * 	CRONICLES
 * 
 * This module may be compiled by: 	'C compiler'
 *
 * Libraries: 
 * 	
 *
 * Programmer: Jaime del Palacio CIS:73072,3134
 *
 */


/* ----------------------------------------------------------------------- */
/* Remarks */

/* ----------------------------------------------------------------------- */
/* Conditional defines */


/* ----------------------------------------------------------------------- */
/* Compiler include files */


/* ----------------------------------------------------------------------- */
/* Third part include files */


/* ----------------------------------------------------------------------- */
/* Libraries include files */


/* ----------------------------------------------------------------------- */
/* Project include files */

/* ----------------------------------------------------------------------- */
/* Global constants & definitions declaration */

typedef int err;

#define MAP_WIDTH   64     // Width of the map in map units
#define MAP_HEIGHT  64     // Height of the map in map units
#define CELL_WIDTH  64     // Width of one cell in pixels
#define CELL_HEIGHT 64     // Height of one cell in pixels
#define SHIFT_CELL_WIDTH   6   // bits to shift to get CELL_WIDTH
#define SHIFT_CELL_HEIGHT  6   // bits to shift to get CELL_HEIGHT
#define AND_CELL_WIDTH  (int)0xffc0  // 63
#define AND_CELL_HEIGHT (int)0xffc0  // 63 (CEL_HEIGHT -1)
#define VIEW_DIST   256    // Viewers distance from screen
#define BLOCK_Z     64     // Height of the block in 3d proj.

#define VERTICAL_SCALE      13312  // used to scale the "slivers" to get proper
                                   // perspective and aspect ratio
#define FOV         60     // Focus of vision

#define VIEW_X_START 2
#define VIEW_Y_START 2
#define VIEW_X_END   317
#define VIEW_Y_END   169
#define VIEW_Y_HALF  84
#define VIEW_X_SIZE  316
#define VIEW_Y_SIZE  168


/* ----------------------------------------------------------------------- */
/* External global variables */

extern byte Map[MAP_WIDTH][MAP_HEIGHT];      // World 2d Map
extern unsigned int * WorldAngles;   // World arcs -> 360 deg look up tbl
extern unsigned char far *VSCREEN;     // offscreen buffer;



/* ----------------------------------------------------------------------- */
/* Global functions & clases declaration */

extern err LoadMap(char *file);
extern void RayCast(int x, int y, long angle);
extern void MakeTables(void);
extern void Mapper (unsigned char far *SCREEN,unsigned char *Tx, long dstx,long dsty, long txcol, long run);
/*
extern void Mapper (unsigned char far *SCREEN,unsigned char *Tx, long dstx,
            long txcol, float dist);
  */
// temp functions

extern void vline(unsigned char far *SCREEN,int x1,int y1, int y2,byte Color);


/* ----------------------------------------------------------------------- */
/* End of raycast.h */
