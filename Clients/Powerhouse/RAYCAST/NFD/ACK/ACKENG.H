/* Header file for ACK3D engine routines */
/* Author: Lary Myers */
/* modified: John Lundy */

#define FP_SHIFT			16
#define FP_MULT				65536
#define FP_HALF				32768

#define DEF_VIEW_WIDTH		320
#define DEF_VIEW_HEIGHT		200
#define DEF_VIEW_HALF_HEIGHT		100
#define DEF_VIEW_X				0	    // Size of the 3D viewport
#define DEF_VIEW_X1				319
#define DEF_VIEW_XS				DEF_VIEW_WIDTH
#define DEF_VIEW_Y				0
#define DEF_VIEW_Y1				199
#define DEF_VIEW_YS				DEF_VIEW_HEIGHT

#define MAX_DISTANCE	2048

#define TYPE_WALL			0
#define TYPE_OBJECT		1
#define TYPE_PALETTE	2

#define BITMAP_WIDTH	64
#define BITMAP_HEIGHT	64
#define BITMAP_SIZE		(BITMAP_WIDTH * BITMAP_HEIGHT)

#define GRID_SIZE			64
#define GRID_WIDTH		64
#define GRID_HEIGHT		64
#define GRID_MAX			(GRID_WIDTH * GRID_HEIGHT)
#define GRID_XMAX			(GRID_WIDTH * GRID_SIZE)
#define GRID_YMAX			(GRID_HEIGHT * GRID_SIZE)

#define GRID_XMAXLONG	(GRID_XMAX * FP_MULT)
#define GRID_YMAXLONG	(GRID_YMAX * FP_MULT)

#define MAX_HEIGHT		960
#define MIN_HEIGHT		16

#define MAX_UPDOWN	  30	    /* Max up or down spots for each level */

#define MAP_STARTCODE	0xFC00	/* Force player to this square  */
#define MAP_UPCODE	  0xFD00	/* Go up a level	    */
#define MAP_DOWNCODE	0xFE00	/* Go down a level	    */
#define MAP_GOALCODE	0xFF00	/* Finish line!					*/

#define DEF_BYTES_PER_ROW		DEF_VIEW_WIDTH
#define DEF_DWORDS_PER_ROW	(DEF_BYTES_PER_ROW / 4)
#define SCREEN_SIZE			64000
