//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "framelib.h"
#include "object.h"

#define GWL_IMAGEPTR	12
#define GWL_DISPLAYPTR	8

typedef void (_far *LPDISPLAYHOOK)(HDC hDC, LPRECT lpRect);
typedef void (_far *LPIMAGEHOOK)(int x, int y, int dx, int depth, LPTR lpSrc, LPTR lpDst, LPTR lpMsk, LPMASK lpMask);

/********************************************************************* 
 **** DISPLAY: The display structure
 *********************************************************************/
typedef struct display_struct {
RECT	DispRect;		/* The display rectangle */
RECT	FileRect;		/* The file rectangle */
LFIXED	DispRate;		/* The ratio of display pixels to file pixels */
LFIXED	FileRate;		/* The ratio of file pixels to display pixels */
int		ViewPercentage;	/* The viewing percentage */
int		xDiva;			/* x-start position of dither */
int		yDiva;			/* y-start position of dither */
RECT	LastWindowRect;	/* The last display rectangle */
RECT	LastFileRect;	/* The last file rectangle */
LFIXED	LastFileRate;	/* The last ratio of file pixels to display pixels */
int		LastViewPercentage; /* The last viewing percentage */
LPIMAGEHOOK	lpImageHook;	/* Image hook proc */
LPDISPLAYHOOK	lpDisplayHook; /* display hook floating graphics */
RECT	UpdateRect;
RECT	WindowRect;
BOOL	ResizeType;
BOOL	HasRulers;
BOOL	bOptimized;		/* Use CPO display */
RECT 	ZoomWndRect;	// pos and size of zoom wnd for restoration.
} DISPLAY;
typedef DISPLAY _far *LPDISPLAY;

#define ONIMAGE( x, y ) \
	( (x) >= lpImage->lpDisplay->DispRect.left && (x) <= lpImage->lpDisplay->DispRect.right && \
	  (y) >= lpImage->lpDisplay->DispRect.top && (y) <= lpImage->lpDisplay->DispRect.bottom )
#define ONIMAGE1( x, y ) \
	( (x) > lpImage->lpDisplay->DispRect.left && (x) < lpImage->lpDisplay->DispRect.right && \
	  (y) > lpImage->lpDisplay->DispRect.top && (y) < lpImage->lpDisplay->DispRect.bottom )

#include "framelib.h"

typedef enum		// current view mode of image
	{
	VM_NORMAL,		// normal viewing of image
	VM_SLIME		// view with alpha channel slimed
	} VIEW_MODE;

/********************************************************************* 
 **** IMAGE: The image structure
 *********************************************************************/

typedef struct image_struct {
LPOLESERVERDOCVTBL pvtbl;	// OLE Standard
LHSERVERDOC lhDoc;		// OLE Required by OleRegisterServerDoc
BOOL	fOwnedByClient; // Does a client app own this image's data?
BOOL	fRelease;		// Document released flag
LPTR	pObj;			// (actually LPIMAGEOBJECT) OLE Last object we allocated
HWND	hWnd;			// Handle to the image window containing this image
int		npix, nlin;		/* Number of high res pixels in image */
int		Dummy1;
int		Dummy2;
ITEMID	DataType;		/* Lineart (IDC_SAVELA), Contone (IDC_SAVECT), etc. */
ITEMID	FileType;		/* TIFF (IDC_SAVETIFF), EPS (IDC_SAVEEPS), etc. */
int		DocumentType;	/* IMG_DOCUMENT, IMG_BRUSH, IMG_CLIPBOARD */
int		UndoDataType;	/* The last DataType for an Undo */
BOOL 	Compressed;		/* Was the file read in as compressed */
BOOL	fChanged;		/* current file modified? */
BOOL	fUntitled;		/* current file untitled? */
BOOL    UseObjectMarquee;
int		MarqueeStatus;	/* The draw status of the marquee */
LPDISPLAY lpDisplay;	/* pointer to current view display state */
LPDISPLAY lpNewDisplay; /* New display pointer */
OBJECTLIST	ObjList;	// The list of objects for this image
OBJECTLIST	AltObjList;	// The alternate list of objects
VIEW_MODE	ViewMode;	// Viewing mode for this image
LPFRAME lpFastbitsPreview;	/* editing frame */
RECT	rFastbitsGrid;	/* Undo area in file coordinates */
FNAME	CurFile;		/* Name of current picture in the window */
FNAME	ImageName;		/* Name of the image */
HPALETTE hPal;			/* Color palette for this image */
int 	nPaletteEntries; /* entries in Palette */
PALETTEENTRY Palette[257]; /* current realized palette */
BOOL	fNoSize;		/* This image cannot be resized */
} IMAGE;
typedef IMAGE _far *LPIMAGE;

#define IMG_DOCUMENT		0x0001
#define IMG_BRUSH			0x0002
#define IMG_CLIPBOARD		0x0003
#define IMG_SPLITR			0x0004
#define IMG_SPLITG			0x0005
#define IMG_SPLITB			0x0006
#define IMG_SPLITC			0x0007
#define IMG_SPLITM			0x0008
#define IMG_SPLITY			0x0009
#define IMG_SPLITK			0x000A
#define IMG_SPLITH			0x000B
#define IMG_SPLITS			0x000C
#define IMG_SPLITL			0x000D
#define IMG_DOCPIECE		0x000E

#define IMG_NOSPLIT			0x0000
#define IMG_SPLITRGB		0x0100
#define IMG_SPLITCMY		0x0200
#define IMG_SPLITCMYK		0x0300
#define IMG_SPLITHSL		0x0400

#define MARQUEE_OFF			1
#define MARQUEE_CYCLE		64
#define MARQUEE_NOT_READY	128

#define INFILE( x, y ) \
	( (x) >= 0 && (x) < lpImage->npix && (y) >= 0 && (y) < lpImage->nlin )
#define INFILE1( x, y ) \
	( (x) > 0 && (x) < lpImage->npix-1 && (y) > 0 && (y) < lpImage->nlin-1)

#endif // _IMAGE_H_
