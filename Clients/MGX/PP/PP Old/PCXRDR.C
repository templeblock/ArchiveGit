// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

typedef struct _pcxmap {
    BYTE	Red;
    BYTE	Green;
    BYTE	Blue;
} PCXMAP;

typedef struct _pcxhdr {
    BYTE	Manufacturer;
    BYTE	Version;
    BYTE	Encoding;
    BYTE	BitsPerPixel;
    WORD	Xmin, Ymin, Xmax, Ymax;
    WORD	HDpi;
    WORD	VDpi;
    BYTE	Colormap [48];	/* rgb rgb rgb rgb ... */
    BYTE	Reserved;
    BYTE	NPlanes;
    WORD	BytesPerLine;
    WORD	PaletteInfo;
    WORD	HscreenSize;
    WORD	VscreenSize;
    BYTE	Filler [54];
} PCXHDR;


/************************************************************************
 * P-FUNCTION pcxReadHeader
 */

static int pcxReadHeader (ifd, hdr)
FILEBUF		*ifd;
PCXHDR		*hdr;
{
    int		err;

    FileRead (ifd, (LPTR) &hdr->Manufacturer, 1);
    FileRead (ifd, (LPTR) &hdr->Version, 1);
    FileRead (ifd, (LPTR) &hdr->Encoding, 1);
    FileRead (ifd, (LPTR) &hdr->BitsPerPixel, 1);
    intelReadWord (ifd, &hdr->Xmin);
    intelReadWord (ifd, &hdr->Ymin);
    intelReadWord (ifd, &hdr->Xmax);
    intelReadWord (ifd, &hdr->Ymax);
    intelReadWord (ifd, &hdr->HDpi);
    intelReadWord (ifd, &hdr->VDpi);
    FileRead (ifd, (LPTR) hdr->Colormap, 48);
    FileRead (ifd, (LPTR) &hdr->Reserved, 1);
    FileRead (ifd, (LPTR) &hdr->NPlanes, 1);
    intelReadWord (ifd, &hdr->BytesPerLine);
    intelReadWord (ifd, &hdr->PaletteInfo);
    intelReadWord (ifd, &hdr->HscreenSize);
    intelReadWord (ifd, &hdr->VscreenSize);
    if (ifd->err)
	return (-1);

/* validate header values */
    if (hdr->Manufacturer != 10) {
	Print ("Unknown manufacturer");
	err = -1;
    }

    switch (hdr->Version) {
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
	break;

    default:
	Print ("Unsupported version");
    }

    if (hdr->Encoding != 1) {
	Print ("Unknown encoding");
	err = -1;
    }

    switch (hdr->BitsPerPixel) {
    case 1:
    case 2:
    case 4:
    case 8:
	break;

    default:
	Print ("Unsupported number of bits per pixel");
	err = -1;
	break;
    }

    if (hdr->Reserved != 0) {
	Print ("Reserved byte not zero");
	err = -1;
    }

    if (hdr->BytesPerLine & 1) {
	Print ("Bytes per line is not even");
	err = -1;
    }

    return (err);
}


/************************************************************************
 * P-FUNCTION pcxReadPlane
 */

static int	pcxReadPlane (ifd, bpl, buffer)
FILEBUF		*ifd;
int		bpl;
LPTR		buffer;
{
    int		i;
    int		cnt;
    char	data;

    while (bpl) {

    /* get count and data value */
	cnt = 1;
	FileRead (ifd, (LPTR) &data, 1);
	if (ifd->err)
	    return (-1);

	if (0xC0 == (0xC0 & data)) {
	    cnt = 0x3F & data;
	    FileRead (ifd, (LPTR) &data, 1);
	    if (ifd->err)
		return (-1);
	}

    /* put values into buffer */
	bpl -= cnt;
	while (cnt--)
	    *buffer++ = data;
    }
    return (0);
}


/************************************************************************
 * P-FUNCTION pcxReadRow
 */

static int pcxReadRow (ifd, hdr, buffer)
FILEBUF		*ifd;
PCXHDR far	*hdr;
LPTR		buffer;
{
    int		i, k;
    LPTR	lpIn, lpOut;
    char	byte;
    int		bytesperplane;
    int		bytes;
    int		shift;

    bytes = hdr->BytesPerLine * hdr->NPlanes;

/* read one plane for the row */
    if (pcxReadPlane (ifd, bytes, buffer) < 0)
	return (-1);

    lpIn = buffer + bytes - 1;
    bytesperplane = hdr->BytesPerLine;
    switch (hdr->BitsPerPixel) {
    case 1:
	bytesperplane *= 8;
	lpOut = buffer + 8*bytes - 1;
	for (k = 0; k < bytes; k++) {
	    byte = *lpIn--;
	    *lpOut-- = byte & 0x01;
	    *lpOut-- = (byte >> 1) & 0x01;
	    *lpOut-- = (byte >> 2) & 0x01;
	    *lpOut-- = (byte >> 3) & 0x01;
	    *lpOut-- = (byte >> 4) & 0x01;
	    *lpOut-- = (byte >> 5) & 0x01;
	    *lpOut-- = (byte >> 6) & 0x01;
	    *lpOut-- = (byte >> 7) & 0x01;
	}
	break;

    case 2:
	bytesperplane *= 4;
	lpOut = buffer + 4*bytes - 1;
	for (k = 0; k < bytes; k++) {
	    byte = *lpIn--;
	    *lpOut-- = byte & 0x03;
	    *lpOut-- = (byte >> 2) & 0x03;
	    *lpOut-- = (byte >> 4) & 0x03;
	    *lpOut-- = (byte >> 6) & 0x03;
	}
	break;

    case 4:
	bytesperplane *= 2;
	lpOut = buffer + 2*bytes - 1;
	for (k = 0; k < bytes; k++) {
	    byte = *lpIn--;
	    *lpOut-- = byte & 0x0F;
	    *lpOut-- = (byte >> 4) & 0x0F;
	}
	break;
    }

/* now pack the planes into the first set of bytes */
    bytes = hdr->Xmax - hdr->Xmin + 1;
    for (i = 1; i < hdr->NPlanes; i++) {
	shift = i * hdr->BitsPerPixel;
	lpOut = buffer;
	lpIn = lpOut + i * bytes;
	for (k = 0; k < bytes; k++, lpIn++, lpOut++)
	    *lpOut |= (*lpIn << shift);
    }
    return (0);
}


/************************************************************************/
LPFRAME pcxrdr(ifile, outdepth, lpBPP)
/************************************************************************/
LPTR ifile;
int outdepth;
LPINT lpBPP;
{
    LPTR   		lpName;
    LPTR		lpFileLine, lpCacheLine, lpRGBLine;
    LPFRAME		lpFrame, lpOldFrame;
    LPTR   		lp, lpOut;
    LPRGB		lpRGB;
    int			ifh;		/* file handle (unbuffered) */
    FILEBUF		ifd;		/* file descriptor (buffered) */
    PCXHDR		hdr;
    RGBS		RGBmap [256];
    PCXMAP		rgbentry;
    BOOL		headermap;
    BOOL		graymap;
    BYTE		colormapflag;
    int			i, j, k, p;
    SPAN		span;
    int			FileBPL;	/* bytes per line (file) */
    int			xres;		/* pixels per inch */
    int			yres;		/* pixels per inch */
    int			npix;		/* image width (pixels) */
    int			nlin;		/* image height (pixels) */
    int			indepth;
    int			bpp;

    lpFileLine = NULL;
    lpCacheLine = NULL;
    lpOldFrame = NULL;

    if ((ifh = _lopen (ifile,OF_READ)) < 0) {
	Message (IDS_EOPEN, ifile );
	goto BadPCX;
    }

    FileFDOpenRdr (&ifd, ifh, LineBuffer[0], 16*1024);

/* read pcx file header */
    if (pcxReadHeader (&ifd, &hdr) == -1)
	goto BadRead;

/* get width of image in pixels */
    npix = hdr.Xmax - hdr.Xmin + 1;
    nlin = hdr.Ymax - hdr.Ymin + 1;
    xres = (hdr.HDpi) ? hdr.HDpi : 75;
    yres = (hdr.VDpi) ? hdr.VDpi : 75;

/* determine total number of bits per pixel */
    if ((hdr.NPlanes == 3) && (hdr.BitsPerPixel == 8)) {
	bpp = 24;
	graymap = FALSE;
    }
    else {
	if (hdr.NPlanes * hdr.BitsPerPixel > 8) {
	    Print ("More then 256 color images not supported");
	    goto BadPCX;
	}
	bpp = hdr.NPlanes * hdr.BitsPerPixel;

    /***********************************/
    /* get the color map for the image */
    /***********************************/
	headermap = TRUE;
	graymap = TRUE;

    /* read 256 color color map, if any */
	if ((hdr.BitsPerPixel == 8) && (hdr.Version == 5)) {

	/* seek to loc of colormap */
	    if ( FileSeek (&ifd, -769L, 2) < 0)
		goto BadRead;

	    FileRead (&ifd, (LPTR) &colormapflag, 1);
	    if (ifd.err)
		goto BadRead;

	    if (colormapflag == 12) {

	    /* read the color map */
		graymap = TRUE;
		for (i = 0; i < 256; i++) {
		    FileRead (&ifd, (LPTR) &rgbentry, 3);
		    RGBmap[i].red = rgbentry.Red;
		    RGBmap[i].green = rgbentry.Green;
		    RGBmap[i].blue = rgbentry.Blue;
		    if (graymap) {
			graymap = (rgbentry.Red == rgbentry.Green)
				    && (rgbentry.Red == rgbentry.Blue);
		    }
		}
		headermap = FALSE;

		if (ifd.err != 0)
		    goto BadRead;
	    }
	}

    /* use colormap from header if extended map not being used */
	if (headermap) {
	    k = 0;
	    for (i = 0; i < (1 << hdr.BitsPerPixel * hdr.NPlanes); i++) {
		RGBmap[i].red = hdr.Colormap [k++];
		RGBmap[i].green = hdr.Colormap [k++];
		RGBmap[i].blue = hdr.Colormap [k++];
		if (graymap) {
		    graymap = (rgbentry.Red == rgbentry.Green)
				&& (rgbentry.Red == rgbentry.Blue);
		}
	    }
	}
    }

    if (!Control.ColorEnabled && !graymap) {
	Print("Cannot load a color image");
	goto BadPCX;
    }

/* set outdepth */
    indepth = (graymap) ? 1 : 3;
    if (outdepth < 0)
        outdepth = indepth;

/* set number of bytes per line (1 byte per pixel) */
    FileBPL = hdr.BytesPerLine * hdr.NPlanes;
    switch (hdr.BitsPerPixel) {
    case 1:
	FileBPL *= 8;
	break;
    case 2:
	FileBPL *= 4;
	break;
    case 4:
	FileBPL *= 2;
	break;
    }

/* seek to loc of image data */
    if ( FileSeek (&ifd, 128L, 0) < 128)
	goto BadRead;

/* allocate space for one line of the image */
    if ( !AllocLines (&lpCacheLine, 1, npix, outdepth)) {
	Message (IDS_EMEMALLOC );
	goto BadWrite;
    }

/* Create the image frame store */
    lpFrame = frame_open (outdepth, npix, nlin, xres);
    if ( !lpFrame ) {
	Message (IDS_ESCROPEN, (LPTR)Control.RamDisk);
	goto BadPCX;
    }
    lpOldFrame = frame_set (lpFrame);

    span.sx = 0;
    span.sy = 0;
    span.dx = npix;
    span.dy = 1;

/* allocate space for one line of the image (file) */
    if ( !AllocLines (&lpFileLine, 1, FileBPL, (graymap) ? 1 : 4)) {
	Message (IDS_EMEMALLOC);
	goto BadWrite;
    }

/* convert the image */
    lpRGBLine = lpFileLine + FileBPL;
    for (i = 0; i < nlin; i++) {
	span.sy = i;
	AstralClockCursor( i, nlin );
	if (bpp == 24) {

	/* get red */
	    if (pcxReadPlane (&ifd, hdr.BytesPerLine, lpFileLine) < 0)
		goto BadRead;

	    lp = lpFileLine;
	    lpRGB = (LPRGB)lpRGBLine;
	    for (k = 0; k < npix; k++, lp++, lpRGB++)
		lpRGB->red = *lp;

	/* get green */
	    if (pcxReadPlane (&ifd, hdr.BytesPerLine, lpFileLine) < 0)
		goto BadRead;

	    lp = lpFileLine;
	    lpRGB = (LPRGB)lpRGBLine;
	    for (k = 0; k < npix; k++, lp++, lpRGB++)
		lpRGB->green = *lp;

	/* get blue */
	    if (pcxReadPlane (&ifd, hdr.BytesPerLine, lpFileLine) < 0)
		goto BadRead;

	    lp = lpFileLine;
	    lpRGB = (LPRGB)lpRGBLine;
	    for (k = 0; k < npix; k++, lp++, lpRGB++)
		lpRGB->blue = *lp;

	    ConvertData ((LPTR)lpRGBLine, indepth, npix, lpCacheLine, outdepth);
	    if (!frame_write (&span, lpCacheLine, npix))
		goto BadWrite;
	}
	else {
	    if (pcxReadRow (&ifd, (PCXHDR far *) &hdr, lpFileLine))
		goto BadRead;

	    if (graymap) {
		lp = lpFileLine;
		for (k = 0; k < npix; k++, lp++)
		    *lp = RGBmap[*lp].red;
		if (indepth != outdepth) {
		    ConvertData (lpFileLine, 1, npix, lpCacheLine, outdepth);
		    if ( !frame_write (&span, lpCacheLine, npix) )
			goto BadWrite;
		}
		else {
		    if ( !frame_write (&span, lpFileLine, npix) )
			goto BadWrite;
		}
	    }
	    else {
		lp = lpFileLine;
		lpRGB = (LPRGB)lpRGBLine;
		for ( k = 0; k < npix; k++, lp++, lpRGB++ ) {
		    lpRGB->red   = RGBmap[ *lp ].red;
		    lpRGB->green = RGBmap[ *lp ].green;
		    lpRGB->blue  = RGBmap[ *lp ].blue;
		}

		if (indepth != outdepth) {
		    ConvertData ((LPTR)lpRGBLine, indepth, npix,
				lpCacheLine, outdepth);
		    if ( !frame_write (&span, lpCacheLine, npix) )
			goto BadWrite;
		}
		else {
		    if ( !frame_write (&span, (LPTR)lpRGBLine, npix) )
			goto BadWrite;
		}
	    }
	}
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
    return(lpFrame);

BadRead:
    Message( IDS_EREAD, ifile );
    goto BadPCX;

BadWrite:
    Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );

BadPCX:
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