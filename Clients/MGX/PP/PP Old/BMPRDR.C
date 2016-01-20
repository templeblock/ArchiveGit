// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

#define BMHDRSZ	sizeof(BITMAPFILEHEADER)	/* size of bitmap file header */


/************************************************************************
 * P-FUNCTION bmpReadHeader
 *
 */

static int bmpReadHeader (ifd, hdr)
FILEBUF			*ifd;
BITMAPFILEHEADER	*hdr;
{
    int		err;

    intelReadWord (ifd, &hdr->bfType);
    intelReadDWord (ifd, &hdr->bfSize);
    intelReadWord (ifd, &hdr->bfReserved1);
    intelReadWord (ifd, &hdr->bfReserved2);
    intelReadDWord (ifd, &hdr->bfOffBits);
    if (ifd->err)
	return (-1);

    if (hdr->bfType != 0x4D42) {	/* BM */
	Print ("Invalid Type");
	err = -1;
    }

    return (err);
}


/************************************************************************/
LPFRAME bmprdr(ifile, outdepth, lpBPP)
/************************************************************************/
LPTR ifile;
int outdepth;
LPINT lpBPP;
{
    LPTR		lpFileLine, lpCacheLine, lpRGBLine;
    LPFRAME		lpFrame, lpOldFrame;
    LPTR   		lp, lpgray;
    LPRGB		lpRGB;
    int			ifh;		/* file handle (unbuffered) */
    FILEBUF		ifd;		/* file descriptor (buffered) */
    BITMAPFILEHEADER	hdr;
    BITMAPINFOHEADER	infohdr;
    RGBS		RGBmap [256];
    RGBQUAD		rgbq;
    BOOL		graymap;
    int			i, k, p;
    SPAN		span;
    int			FileBPL;	/* bytes per line (bmp file) */
    int			xres;		/* pixels per inch */
    int			bpp;		/* bits per pixel */
    int			npix;		/* image width (pixels) */
    int			nlin;		/* image height (pixels) */
    int			indepth;	/* bytes per pixel (file 1 or 3) */

    lpFileLine = NULL;
    lpCacheLine = NULL;
    lpOldFrame = NULL;

    if ((ifh = _lopen(ifile,OF_READ)) < 0) {
	Message( IDS_EOPEN, ifile );
	goto BadBMP;
    }

    FileFDOpenRdr (&ifd, ifh, LineBuffer[0], 16*1024);

/***********************************/
/* determine image characteristics */
/***********************************/

/* read bmp file header */
    if (bmpReadHeader (&ifd, &hdr) == -1)
	goto BadRead;

/* read size needed for following structure (determines which structure) */
    intelReadDWord (&ifd, &infohdr.biSize);
    if (ifd.err)
	goto BadRead;

/* check for bitmapcoreheader */
    if (infohdr.biSize == sizeof (BITMAPCOREHEADER)) {
	Print ("Unsupported BMP format.");
	goto BadRead;
    }

/* assume bitmapinfoheader */
    else {
	intelReadDWord (&ifd, &infohdr.biWidth);
	intelReadDWord (&ifd, &infohdr.biHeight);
	intelReadWord (&ifd, &infohdr.biPlanes);
	intelReadWord (&ifd, &infohdr.biBitCount);
	intelReadDWord (&ifd, &infohdr.biCompression);
	intelReadDWord (&ifd, &infohdr.biSizeImage);
	intelReadDWord (&ifd, &infohdr.biXPelsPerMeter);
	intelReadDWord (&ifd, &infohdr.biYPelsPerMeter);
	intelReadDWord (&ifd, &infohdr.biClrUsed);
	intelReadDWord (&ifd, &infohdr.biClrImportant);

    /* validate that image is not compressed */
	if (infohdr.biCompression != BI_RGB) {
	    Print ("Compressed bit maps not supported.");
	    goto BadBMP;
	}

    /* get width of image in pixels */
	npix = infohdr.biWidth;
	nlin = infohdr.biHeight;
	if (infohdr.biXPelsPerMeter)
		xres = ( 254L * infohdr.biXPelsPerMeter ) / 10000L;
	else	xres = 75;
	if ( xres <= 1 )
		xres = 75;

    /* set indepth, outdepth and colormap size */
	switch (infohdr.biBitCount) {
	case 1:
	    if ((infohdr.biClrUsed == 0) || (infohdr.biClrUsed > 2))
		infohdr.biClrUsed = 2;
	    bpp = 1;
	    indepth = 1;
	    FileBPL = (infohdr.biWidth + 7) / 8;
	    break;

	case 4:
	    if ((infohdr.biClrUsed == 0) || (infohdr.biClrUsed > 16))
		infohdr.biClrUsed = 16;
	    bpp = 4;
	    indepth = 3;
	    FileBPL = (infohdr.biWidth + 1) / 2;
	    break;

	case 8:
	    if ((infohdr.biClrUsed == 0) || (infohdr.biClrUsed > 256))
		infohdr.biClrUsed = 256;
	    bpp = 8;
	    indepth = 3;
	    FileBPL = infohdr.biWidth;
	    break;

	case 24:
	    infohdr.biClrUsed = 0;
	    bpp = 24;
	    indepth = 3;
	    FileBPL = 3*infohdr.biWidth;
	    break;

	default:
	    goto BadRead;
	    break;
	}
	if (ifd.err != 0)
	    goto BadRead;

    /* adjust bytes per line to DWORD (long) boundry */
	FileBPL = (FileBPL + 3) & ~3;

    /* read color map, if any */
	if (infohdr.biClrUsed) {

	/* seek to loc of colormap */
	    if ( FileSeek (&ifd, infohdr.biSize + BMHDRSZ, 0)
			< infohdr.biSize + BMHDRSZ)
		return (NULL);

	    graymap = TRUE;
	    for (i = 0; i < infohdr.biClrUsed; i++) {
		FileRead (&ifd, (LPTR) &rgbq, 4);
		RGBmap[i].red = rgbq.rgbRed;
		RGBmap[i].green = rgbq.rgbGreen;
		RGBmap[i].blue = rgbq.rgbBlue;
		if (graymap) {
		    graymap = (rgbq.rgbRed == rgbq.rgbGreen)
				&& (rgbq.rgbRed == rgbq.rgbBlue);
		}
	    }

	/* if color map entries are 'gray', do a depth of one */
	    if ((indepth != 1) && graymap)
		indepth = 1;

	    if (ifd.err != 0)
		goto BadRead;
	}
    }

    if (outdepth < 0)
        outdepth = indepth;

    if (!Control.ColorEnabled && !graymap) {
	Print("Cannot load a color image");
	goto BadBMP;
    }

/* allocate space for one line of the image */
    if ( !AllocLines (&lpCacheLine, 1, npix, outdepth)) {
	Message (IDS_EMEMALLOC );
	goto BadWrite;
    }

/* Create the image frame store */
    lpFrame = frame_open (outdepth, npix, nlin, xres);
    if ( !lpFrame ) {
	Message (IDS_ESCROPEN, (LPTR)Control.RamDisk);
	goto BadBMP;
    }
    lpOldFrame = frame_set (lpFrame);

    span.sx = 0;
    span.sy = 0;
    span.dx = npix;
    span.dy = 1;

/* convert the image */
    switch (bpp) {
    case 1:

    /* allocate space for one line of the image (bmp file) */
	if ( !AllocLines (&lpFileLine, 1, FileBPL, 1)) {
	    Message (IDS_EMEMALLOC);
	    goto BadWrite;
	}

	for (i = 0; i < nlin; i++) {
	    span.sy = nlin - i - 1;
	    AstralClockCursor (i, nlin);
	    if ( FileRead (&ifd, lpFileLine, FileBPL) != FileBPL)
		goto BadRead;

	    if (RGBmap[0].red == 0)
		negate (lpFileLine, (long)FileBPL);

	    ConvertData (lpFileLine, 0, npix, lpCacheLine, outdepth);

	    if ( !frame_write (&span, lpCacheLine, npix))
		goto BadWrite;
	}
	break;

    case 4:

    /* allocate space for one line of the image (bmp file) */
	if ( !AllocLines (&lpFileLine, 1, 2*FileBPL, 4)) {
	    Message (IDS_EMEMALLOC);
	    goto BadWrite;
	}

	lpRGBLine = lpFileLine + 2*FileBPL;
	for (i = 0; i < nlin; i++) {
	    span.sy = nlin - i - 1;
	    AstralClockCursor( i, nlin );
	    if ( FileRead (&ifd, lpFileLine, FileBPL) != FileBPL)
		goto BadRead;

	    lp = lpFileLine;
	    lpRGB = (LPRGB)lpRGBLine;
	    for (k = 0; k < FileBPL; k++, lp++) {
		p = (*lp >> 4) & 0x0F;
		lpRGB->red   = RGBmap[ p ].red;
		lpRGB->green = RGBmap[ p ].green;
		lpRGB->blue  = RGBmap[ p ].blue;
		lpRGB++;

		p = *lp & 0x0F;
		lpRGB->red   = RGBmap[ p ].red;
		lpRGB->green = RGBmap[ p ].green;
		lpRGB->blue  = RGBmap[ p ].blue;
		lpRGB++;
	    }

	    if (3 != outdepth) {
		ConvertData ((LPTR)lpRGBLine, 3, npix, lpCacheLine, outdepth);
		if ( !frame_write (&span, lpCacheLine, npix) )
		    goto BadWrite;
	    }
	    else {
		if ( !frame_write (&span, (LPTR)lpRGBLine, npix) )
		    goto BadWrite;
	    }
	}
	break;

    case 8:

    /* allocate space for one line of the image (bmp file) */
	if ( !AllocLines (&lpFileLine, 1, FileBPL, 4)) {
	    Message (IDS_EMEMALLOC);
	    goto BadWrite;
	}

	lpRGBLine = lpFileLine + FileBPL;
	for (i = 0; i < nlin; i++) {
	    span.sy = nlin - i - 1;
	    AstralClockCursor( i, nlin );
	    if ( FileRead (&ifd, lpFileLine, FileBPL) != FileBPL)
		goto BadRead;

	    lp = lpFileLine;
	    lpRGB = (LPRGB)lpRGBLine;
	    for ( k = 0; k < FileBPL; k++, lp++, lpRGB++ ) {
		p = *lp & 0x00FF;
		lpRGB->red   = RGBmap[ p ].red;
		lpRGB->green = RGBmap[ p ].green;
		lpRGB->blue  = RGBmap[ p ].blue;
	    }

	    if (3 != outdepth) {
		ConvertData ((LPTR)lpRGBLine, 3, npix, lpCacheLine, outdepth);
		if ( !frame_write (&span, lpCacheLine, npix) )
		    goto BadWrite;
	    }
	    else {
		if ( !frame_write (&span, (LPTR)lpRGBLine, npix) )
		    goto BadWrite;
	    }
	}
	break;

    case 24:

    /* allocate space for one line of the image (bmp file) */
	if ( !AllocLines (&lpFileLine, 1, FileBPL, 1)) {
	    Message (IDS_EMEMALLOC);
	    goto BadWrite;
	}

	for (i = 0; i < nlin; i++) {
	    span.sy = nlin - i - 1;
	    AstralClockCursor( i, nlin );
	    if ( FileRead (&ifd, lpFileLine, FileBPL) != FileBPL)
		goto BadRead;

	    for (k = 0, lp = lpFileLine; k < npix; k++, lp += 3) {
		p = *lp;
		*lp = *(lp+2);
		*(lp+2) = p;
	    }

	    if (indepth != outdepth) {
		ConvertData (lpFileLine, indepth, npix, lpCacheLine, outdepth);
		if ( !frame_write (&span, lpCacheLine, npix) )
		    goto BadWrite;
	    }
	    else {
		if ( !frame_write (&span, lpFileLine, npix) )
		    goto BadWrite;
	    }
	}
	break;
    }

    if ( lpFileLine )
	FreeUp (lpFileLine);
    if ( lpCacheLine )
	FreeUp (lpCacheLine);
    if ( ifh >= 0 )
	_lclose (ifh);
    if (lpOldFrame)
        frame_set(lpOldFrame);
    *lpBPP = bpp;
    return (lpFrame);

BadRead:
    Message( IDS_EREAD, ifile );
    goto BadBMP;

BadWrite:
    Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );

BadBMP:
    if ( lpFileLine )
	FreeUp( lpFileLine );
    if ( lpCacheLine )
	FreeUp( lpCacheLine );
    if ( ifh >= 0 )
	_lclose(ifh);
    if (lpOldFrame)
        frame_set(lpOldFrame);
    return( NULL );
}
