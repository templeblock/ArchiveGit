/* Header file for ACK3D engine routines */
/* Author: Lary Myers */

#define TRANS_WALLS	1
#define FLOOR_ACTIVE	1
#define USE_XMS		1	    /* Set to 0 if XMS not desired */

#define FP_SHIFT	16
#define FP_MULT		65536
#define FP_HALF		32768

#define VIEW_WIDTH	320
#define MAX_DISTANCE	2048

#define TYPE_WALL	0
#define TYPE_OBJECT	1
#define TYPE_PALETTE	2

#define BITMAP_WIDTH	64
#define BITMAP_HEIGHT	64
#define BITMAP_SIZE	(BITMAP_WIDTH * BITMAP_HEIGHT)

#define GRID_SIZE	64
#define GRID_WIDTH	64
#define GRID_HEIGHT	64
#define GRID_MAX	GRID_WIDTH * GRID_HEIGHT
#define GRID_XMAX	GRID_WIDTH * GRID_SIZE
#define GRID_YMAX	GRID_HEIGHT * GRID_SIZE

#define GRID_XMAXLONG	(GRID_XMAX * FP_MULT)
#define GRID_YMAXLONG	(GRID_YMAX * FP_MULT)

#define MAX_HEIGHT	960
#define MIN_HEIGHT	16


#define MAX_UPDOWN	    30	    /* Max up or down spots for each level */

#define MAP_STARTCODE		0xFC00		/* Force player to this square  */
#define MAP_UPCODE			0xFD00		/* Go up to previous level	    */
#define MAP_DOWNCODE		0xFE00		/* Go down to next level	    */
#define MAP_GOALCODE		0xFF00		/* Finish line!		    */


#define MAX_XARRAY	    10	    /* Max number in XMS array	    */

typedef struct {
    UCHAR   far	    *Bmp;
    unsigned long   xHandle;
	    int	    count;
} XARRAY;


typedef struct {
unsigned    int	    VidOffset;
	    void    (far *bdfp)();
	    int	    filler;
} HTABLE;


#if TRANS_WALLS
typedef struct _WARRAY {
	int	    Number;
	int	    Column;
	int	    Distance;
	int	    LightAdj;
struct	_WARRAY	    *Prev;
struct	_WARRAY	    *Next;
    } WALLARRAY;
#else
typedef struct {
	int	    Number;
	int	    Column;
	int	    Distance;
	int	    LightAdj;
    } WALLARRAY;
#endif

typedef struct {
	    int	    mPos;
	    UINT    mCode;
} SPECIALCODE;


#define BYTES_PER_ROW	320
#define DWORDS_PER_ROW	(BYTES_PER_ROW / 4)
#define SCREEN_SIZE	64000

