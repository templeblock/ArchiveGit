#define TYPES

/*®FD1¯®PL1¯®TP0¯®BT0¯*/
/* Miscellaneous definitions */
#define	MAX_FNAME_LEN	80
#define	MAX_STR_LEN	80
#define	MAX_IMAGE_LINE	4000
#define MAX_HUES	252
#define BIT1		1L
#define BIT2		2L
#define BIT3		4L
#define BIT6		32L
#define BIT18		131072L
#define BITMASK		( BIT1| BIT2| BIT3| BIT6 )
#define MAX_DISP_WIDTH	1024
#define MAXPALETTE	235
#define SMALL_MOVEMENT	4
#define DEPTH		lpImage->Depth
#define TOPERCENT(n)	 ( ((100L * (n)) + 127) / 255 )
#define TOGRAY(n)	 ( ((255L * (n)) +  50) / 100 )
#define TOANGLE(n)	 ( ((359L * (n)) + 125) / 251 )
#define FROMANGLE(n)	 ( ((251L * (n)) + 179) / 359 )
#define RULER_SIZE	20
#define MIN_WINDOW_SIZE 20

/************************************************************************/
/* TIFF defines             			                 				*/
/************************************************************************/
#define TIF_BYTE 1
#define TIF_ASCII 2
#define TIF_SHORT 3
#define TIF_LONG 4
#define TIF_RATIONAL 5
#define TIF_MM (0x4D4D)
#define TIF_II (0x4949)
#define MYORDER (TIF_II)
#define TIFF_VERSION (0x2A)

/************************************************************************/
/* Upper-cased type definitions						*/
/************************************************************************/
#define FAR	far
#define NEAR	near
#define LONG	long
#define VOID	void
#define CHAR	char
#define INT	int
#define SHORT	short

/************************************************************************/
/* Type definitions - copied from windows.h for consistency		*/
/************************************************************************/
#ifndef PASCAL
typedef unsigned char	BYTE;
typedef unsigned short  WORD;
typedef unsigned long  DWORD;
typedef int BOOL;
typedef char *PSTR;
typedef char near *NPSTR;
typedef char far *LPSTR;
typedef int  far *LPINT;
typedef struct tagRECT {
    int left;
    int top;
    int right;
    int bottom;
} RECT;
#else
typedef LPPOINT far *LPPPOINT;
#endif

/************************************************************************/
/* Type definitions							*/
/************************************************************************/
#undef LPTR
typedef unsigned char near *PTR;
typedef int near *PINT;
typedef unsigned char far *LPTR;
typedef LPTR far *LPPTR;
typedef LPINT far *LPPINT;
typedef LPPINT far *LPPPINT;
typedef unsigned char huge *HPTR;
typedef WORD far *LPWORD;
typedef LPWORD far *LPPWORD;
typedef LPPWORD far *LPPPWORD;
typedef long far *LPLONG;
typedef unsigned long far *LPDWORD;
typedef char FNAME[MAX_FNAME_LEN];
typedef char STRING[MAX_STR_LEN];
typedef unsigned int UINT;
typedef unsigned int UINT16;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef long FIXED;
typedef unsigned long PFIXED;
typedef unsigned long COLOR;
typedef int (far *LPROC)();
typedef long (far *LPFL)();
typedef HWND far *LPHWND;

/************************************************************************/
/* Macintosh compatibility types					*/
/************************************************************************/
typedef RECT RCT;
typedef POINT APOINT;
typedef BOOL BOOLEAN;

/************************************************************************/
/* Constant definitions							*/
/************************************************************************/
#define LOCAL		static
#define REG		register
#define YES		1
#define NO		0
#define ON		1
#define OFF		0
#define NULLP		((void *)0)
#define UNITY		65536L
#define HALF		32768L
#define WRITE_MASK	1
#define READ_MASK	2

/************************************************************************/
/* Error definitions							*/
/************************************************************************/
//#ifdef ERROR
//#undef ERROR
//#endif
//#define SUCCESS		0
//#define ERROR		(-1)
//#define FAILURE		(-1)
//#define OPEN_ERROR	(-100)
//#define READ_ERROR	(-200)
//#define WRITE_ERROR	(-300)
//#define ALLOC_ERROR	(-400)
//#define BADP_ERROR	(-500)
//#define RANGE_ERROR	(-600)

/************************************************************************/
/* Span vector definition						*/
/************************************************************************/
typedef struct _span
	{
	int	sx;
	int	sy;
	int	dx;
	int	dy;
} SPAN;
typedef SPAN far *LPSPAN;

/************************************************************************/
/* Address list entry definition					*/
/************************************************************************/
typedef struct _addr
	{
	LPTR	Memory;
	ULONG	Disk;
} ADDR;
typedef ADDR far *LPADDR;
#define MODIFIED_ON  0x80000000
#define MODIFIED_OFF 0x7FFFFFFF

#define LINE_EDITED	1
#define LINE_DIRTY	2

/************************************************************************/
/* Link list entry definition						*/
/************************************************************************/
typedef struct _link
	{
	int	Next;
	int	Prev;
} LINK;
typedef LINK far *LPLINK;

/************************************************************************/
/* Tag structure definition						*/
/************************************************************************/
typedef struct _tag
	{
	int	tagno;
	int	type;
	long	length;
	long	value;
} TAG;
typedef TAG far *LPTAG;

/************************************************************************/
/* Transformation matrix definition					*/
/************************************************************************/
typedef struct _tform
	{				/*  ax  ay  0  */
	FIXED ax, ay;			/*  bx  by  0  */
	FIXED bx, by;			/*  cx  cy  1  */
	FIXED cx, cy;
} TFORM;
typedef TFORM far *LPTFORM;

/************************************************************************/
/* Shape structure definition						*/
/************************************************************************/
typedef struct _shape
	{
	BOOL fCircle;
	int nPoints;
	LPPOINT lpPoints;
	RECT rShape;
	TFORM tform;
	struct _shape far *lpNext;
} SHAPE;
typedef SHAPE far *LPSHAPE;

/************************************************************************/
/* Mask structure definition						*/
/************************************************************************/
typedef struct _mask
	{
	LPSHAPE lpHeadShape;
	int nEdges;
	LPPOINT lpEdges;
	int nEdgeMapEntries;
	LPPPOINT lpEdgeMap;
	int nMarqueePoints;
	LPPOINT lpMarquee;
	RECT rMask;
	int mbufsize;
	LPTR mbuffer;
} MASK;
typedef MASK far *LPMASK;
typedef LPMASK far *LPPMASK;

#define MAX_BEZIER 25
#define BEZIER_MARKER 32766
#define PENUP_MARKER 32767

/************************************************************************/
/* Convolution kernal definition					*/
/************************************************************************/
typedef struct _kernal
	{
	int	absflag;
	int	rotation;
	int	edgetype;
	int	lut1[256];
	int	lut2[256];
	int	lut3[256];
	int	lut4[256];
	int	lut5[256];
	int	lut6[256];
	int	lut7[256];
	int	lut8[256];
	int	lut9[256];
	char	outlut[256];
} KERNAL;
typedef KERNAL far *LPKERNAL;

/************************************************************************/
/* Hue, Saturation, and Luminace color definition			*/
/************************************************************************/
typedef struct _hsl
	{
	BYTE hue;
	BYTE sat;
	BYTE lum;
} HSLS;
typedef HSLS far *LPHSL;

/************************************************************************/
/* Red, Green, and Blue color definition				*/
/************************************************************************/
typedef struct _rgb
	{
	BYTE red;
	BYTE green;
	BYTE blue;
} RGBS;
typedef RGBS far *LPRGB;

/************************************************************************/
/* The DOS register structures for intdosx() and IntDos21()		*/
/************************************************************************/
union REGS
	{
	struct { WORD ax, bx, cx, dx, si, di, cflag; } x;
	struct { BYTE al, ah, bl, bh, cl, ch, dl, dh; } h;
	};

struct SREGS { WORD es, cs, ss, ds; }; /* segment registers */

typedef union REGS REGS;
typedef struct SREGS SREGS;
typedef REGS far *LPREGS;
typedef SREGS far *LPSREGS;

/************************************************************************/
/* Our own register structures for do_int()				*/
/************************************************************************/
typedef union
	{
	struct { WORD ax, bx, cx, dx, si, di, es, ds, flags; } x;
	struct { BYTE al, ah, bl, bh, cl, ch, dl, dh; } h;
	} MREGS;
typedef MREGS far *LPMREGS;

/************************************************************************/
/* Buffered IO structure */
/************************************************************************/
typedef struct _filebuf
	{
	LPTR	buf;	/* pointer to buffer */
	int	bsiz;	/* size of buffer */
	int	valid;	/* number of valid bytes in buffer (read) */
	int	offset;	/* next byte to read/write in buffer */
	int	fh;	/* file handle */
	int	mode;	/* 1 = write, 0 = read */
	long	pos;	/* file position for first byte in buffer */
	int	err;	/* error status (-1 = error) */
} FILEBUF;
typedef FILEBUF *PFILEBUF;
typedef FILEBUF far *LPFILEBUF;
