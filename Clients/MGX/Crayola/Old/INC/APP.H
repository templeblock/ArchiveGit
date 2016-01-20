//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _APP_H_
#define _APP_H_

// App specific
#define MIN_WINDOW_SIZE 20
#define MAX_HUES	252
#define SMALL_MOVEMENT	4
#define TEXTCOLOR(n) ((n > 127) ? RGB(0,0,0): RGB(255,255,255))
#define GETGRAY(n) RGB(n,n,n)
#define TOANGLE(n)	((int)( ((359L * (n)) + 125) / 251 ))
#define FROMANGLE(n)	((int)( ((251L * (n)) + 179) / 359 ))
#define TOPERCENT(n)	((int)( ((100L * (n)) + 127) / 255 ))
#define TOGRAY(n)	((int)( ((255L * (n)) +  50) / 100 ))
#define PIX2DISP(n)	(Control.DisplayPixelValue ? (n) : TOPERCENT(n))
#define GRAY2DISP(n) (Control.DisplayPixelValue ? (n) : 100-TOPERCENT(n))
#define DISP2PIX(n)	(Control.DisplayPixelValue ? (n) : TOGRAY(n))
#define MAXPIXDISP (Control.DisplayPixelValue ? 255 : 100)

//Adventure specific
#define BRUSH_SIZE_SMALL	0
#define BRUSH_SIZE_MEDIUM	1
#define BRUSH_SIZE_LARGE	2

// Generic custom control definitions
#define WS_NOTENABLED	(WS_NOTACTIVE|WS_DISABLED)
#define WS_NOTACTIVE	1L

//***************************************************
// Icon & image controls share some code.. be careful about overlaping flags
// Icon control styles & LabeledFrame Also
#define WS_NOSHADOWS	0x0002L
#define WS_BITMAP 		0x0004L
#define WS_TOOL  		0x0008L
#define WS_TEXT	  		0x0010L
#define WS_IMAGE  		0x0020L
#define WS_ONTIMER		0x0040L
#define WS_OBJECT		0x0080L
#define WS_BORDERONLY	0x0100L
#define WS_TWOFACED		0x0200L
// Image control styles
#define IS_STRETCHABLE	0x0002L
#define IS_MOVEABLE		0x0004L
#define IS_HASCHANGED	0x0010L
#define IS_RATIO_CONSTRAIN  0x0020L
#define IS_LIMITSIZE	0x0040L
//***************************************************



// Stick Control syles
#define ST_HONLY	0x0002L


// Ruler control styles
#define RS_HORZ		1L
#define RS_VERT		2L

#define CTLCOLOR_CUSTOM 100

/* All controls should have the first 3 words reserved */
/* either private data or a data pointer should follow */
#define GWW_STATE		0
#define GWW_FONT		2
#define GWW_STATUS		4
#define GWL_CTLDATAPTR		6

/* GWW_STATUS possible values */
#define ST_TRACK		0x0001
#define ST_INRECT		0x0002

/* Icon control window word defintions */
#define GWW_ICONID		6
#define GWW_ICONINST	8

/* Icon control window word defintions */
#define GWL_IMAGE			8 // Used in icon control also
#define GWL_IMAGE_TOOLPROC	12 // An image control tool proc

/* Color2 control window word defintions */
#define GWL_ACTIVE		6
#define GWL_ALTERNATE		10

// Spin control windows words
#define GWW_SPINTYPE	0
#define GWW_SPINMIN	2
#define GWW_SPINMAX	4
#define GWL_FSPINMIN	6
#define GWL_FSPINMAX	10
#define GWL_FSPINDISPLAYVALUE 14
#define GWL_FSPINVALUE 18
#define GWL_SPINSTEP   22

/* Gamma control */
#define GWW_ANIMATE		6
#define GWW_ANIMATECOLOR	8

// Visual control
#define GWL_LPVISUAL	8 


/* Palette control */
#define GWL_PALETTE		6
#define GWW_SELECT		10
#define GWL_SIZE		12
#define GWW_HSCROLL_MIN		16
#define GWW_HSCROLL_MAX		18
#define GWW_HSCROLL_POS		20
#define GWW_VSCROLL_MIN		22
#define GWW_VSCROLL_MAX		24
#define GWW_VSCROLL_POS		26

// Zoom Window extras
#define GWW_ZOOMBOXLEFT 	20		// inverted box in zoom window
#define GWW_ZOOMBOXTOP 		22		
#define GWW_ZOOMBOXRIGHT	24		
#define GWW_ZOOMBOXBOTTOM	26		
#define GWW_ZOOMOWNER		28		
#define WNDEXTRA_ZOOMBOX	30

/* Private Messages */
#define WM_SETCONTROL		(WM_USER)
#define WM_SETPALETTE		(WM_USER+1)
#define WM_PALETTESIZE		(WM_USER+2)
#define WM_SELECTPALETTE	(WM_USER+3)
#define WM_DOCCHANGED		(WM_USER+4)
#define WM_DOCACTIVATED		(WM_USER+5)
#define WM_CONTROLENABLE	(WM_USER+6)
#define WM_GETCOLOR		(WM_USER+7)
#define WM_DRAGQUERY		(WM_USER+8)
#define WM_DRAGACCEPT		(WM_USER+9)
#define WM_IMAGESPACE		(WM_USER+10)
#define WM_SHIELDCHANGE	(WM_USER+11)

typedef void (_far *LPPROCESSPROC)(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount);
typedef void (_far *LPTRANSFORMENDPROC)(LPIMAGE lpImage, BOOL fCancel,
	BOOL fMoveOnly);
typedef int (FAR *LPTOOLPROC)( HWND, LPARAM, UINT );
#ifndef STATIC16 // only in new framelib
typedef BOOL ( FAR *STATUSCALLBACKPROC )( int, int, BOOL );
#endif
typedef void (_far *LPPOINTFUNC)(int, int);

typedef BOOL (_far *LPGETMASKPIXEL)(int, int, LPLONG);
typedef BOOL (_far *LPPUTMASKPIXEL)(int, int, long);
typedef BOOL (_far *LPINMASKREGION)(long);
typedef BOOL (_far *LPADDSHAPE)(LPPOINT, int);


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
typedef KERNAL _far *LPKERNAL;

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
typedef FILEBUF _far *LPFILEBUF;

/********************************************************************* 
 **** STYLE:
 *********************************************************************/

typedef struct _style
{
FNAME	HalftoneName;
FNAME	SeparateName;
FNAME	CalibrateName;
} STYLE;
typedef STYLE _far *LPSTYLE;

/********************************************************************* 
 **** PALETTE:
 *********************************************************************/

typedef struct _palett
{
FNAME	szName;
LPRGB	lpColors;
short	iColors;
LPTR	lpLabels;
WORD	LabelsLength;
DWORD	dwGroup;
STRING	szFormat;
struct _palett far *lpNext;
LPCOLORINFO lpColorInfo;
} PALETTE;
typedef PALETTE far *LPPALETTE;


#ifdef WIN32

 #define GET_WINDOW_ID(hWindow) \
	GetWindowLong(hWindow, GWL_ID)
 #define GET_WINDOW_INSTANCE(hWindow) \
	GetWindowLong(hWindow, GWL_HINSTANCE)
 #define GET_WINDOW_PARENT(hWindow) \
	GetWindowLong(hWindow, GWL_HWNDPARENT)

 #define SET_CLASS_STYLE(hWnd,style) \
 	SetClassLong(hWnd, GCL_STYLE, (long)style)
 #define SET_CLASS_CURSOR(hWnd,hCursor) \
 	SetClassLong(hWnd, GCL_HCURSOR, (long)hCursor)
 #define SET_CLASS_HBRBACKGROUND(hWnd,hBrush) \
 	SetClassLong(hWnd, GCL_HBRBACKGROUND, (long)hBrush)
 #define SET_CLASS_CBCLSEXTRA(hWnd,extra) \
 	SetClassLong(hWnd, GCL_CBCLSEXTRA, (long)extra)

 #define GET_CLASS_STYLE(hWnd) \
 	GetClassLong(hWnd, GCL_STYLE)
 #define GET_CLASS_CURSOR(hWnd) \
 	GetClassLong(hWnd, GCL_HCURSOR)
 #define GET_CLASS_HBRBACKGROUND(hWnd) \
 	GetClassLong(hWnd, GCL_HBRBACKGROUND)
 #define GET_CLASS_CBCLSEXTRA(hWnd) \
 	GetClassLong(hWnd, GCL_CBCLSEXTRA)

 POINT MAKEPOINT(long l);	// POINT is 32bit values, POINTS is 16bit values	

#else

 #define GET_WINDOW_ID(hWindow) \
	GetWindowWord(hWindow, GWW_ID)
 #define GET_WINDOW_INSTANCE(hWindow) \
	GetWindowWord(hWindow, GWW_HINSTANCE)
 #define GET_WINDOW_PARENT(hWindow) \
	GetWindowWord(hWindow, GWW_HWNDPARENT)
	
 #define SET_CLASS_STYLE(hWnd,style) \
 	SetClassWord(hWnd, GCW_STYLE, (WORD)style)
 #define SET_CLASS_CURSOR(hWnd,hCursor) \
 	SetClassWord(hWnd, GCW_HCURSOR, (WORD)hCursor)
 #define SET_CLASS_HBRBACKGROUND(hWnd,hBrush) \
 	SetClassWord(hWnd, GCW_HBRBACKGROUND, (WORD)hBrush)
 #define SET_CLASS_CBCLSEXTRA(hWnd,extra) \
 	SetClassWord(hWnd, GCW_CBCLSEXTRA, (WORD)extra)

 #define GET_CLASS_STYLE(hWnd) \
 	GetClassWord(hWnd, GCW_STYLE)
 #define GET_CLASS_CURSOR(hWnd) \
 	GetClassWord(hWnd, GCW_HCURSOR)
 #define GET_CLASS_HBRBACKGROUND(hWnd) \
 	GetClassWord(hWnd, GCW_HBRBACKGROUND)
 #define GET_CLASS_CBCLSEXTRA(hWnd) \
 	GetClassWord(hWnd, GCW_CBCLSEXTRA)
	
#endif


#endif // _APP_H_


