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
#include "gif.h"

typedef struct {
    int		first;
    int		step;
} GIFROW;

static GIFROW gifRowMap [5] = {
    {0, 1},	/* not interlaced */
    {0, 8},	/* first frame of interlaced date */
    {4, 8},	/* second frame of interlaced date */
    {2, 4},	/* third frame of interlaced date */
    {1, 2},	/* fourth frame of interlaced date */
};

/************************************************************************
 * P-FUNCTION gifReadHeader
 */

int		gifReadHeader (ifd, hdr)
FILEBUF		*ifd;
GIFHDR		*hdr;
{
    FileRead (ifd, (LPTR) hdr->Signiture, 6);
    intelReadWord (ifd, &hdr->ScnWidth);
    intelReadWord (ifd, &hdr->ScnHeight);
    FileRead (ifd, (LPTR) &hdr->RawImInfo, 1);
    FileRead (ifd, (LPTR) &hdr->Background, 1);
    FileRead (ifd, (LPTR) &hdr->Zero, 1);
    if (ifd->err)
	return (-1);

/* validate header values */
    if ((hdr->Signiture[0] != 'G') ||
		(hdr->Signiture[1] != 'I') ||
		(hdr->Signiture[2] != 'F'))
	return (-1);

    if (hdr->Zero != 0)
	return (-1);

/* pick apart the RawImInfo byte */
    hdr->GlobalMap = (hdr->RawImInfo & 0x80) ? TRUE : FALSE;
    hdr->ColorRes = ((hdr->RawImInfo >> 4) & 0x07) + 1;
    hdr->bpp = (hdr->RawImInfo & 0x07) + 1;

    return (0);
}


/************************************************************************
 * P-FUNCTION gifReadImDesc
 */

int		gifReadImDesc (ifd, ImDesc)
FILEBUF		*ifd;
GIFDESC		*ImDesc;
{
    intelReadWord (ifd, &ImDesc->ImLeft);
    intelReadWord (ifd, &ImDesc->ImTop);
    intelReadWord (ifd, &ImDesc->ImWidth);
    intelReadWord (ifd, &ImDesc->ImHeight);
    FileRead (ifd, (LPTR) &ImDesc->RawImInfo, 1);

/* pick apart the RawImInfo byte */
    ImDesc->LocalMap = (ImDesc->RawImInfo & 0x80) ? TRUE : FALSE;
    ImDesc->Interlaced = (ImDesc->RawImInfo & 0x40) ? TRUE : FALSE;
    ImDesc->bpp = (ImDesc->RawImInfo & 0x07) + 1;

    return (ifd->err);
}


/************************************************************************
 * P-FUNCTION gifInitColorMap
 */

void		gifInitColorMap (map)
GIFMAP		*map;
{
    int		i;

    map->Length = 256;

/* Initialize the map */
    for (i = 0; i < 256; i++) {
	map->Map[i].red =
	map->Map[i].green =
	map->Map[i].blue = i;
    }
}


/************************************************************************
 * P-FUNCTION gifReadColorMap
 *	(if ifd == NULL, initialize the color map)
 */

int		gifReadColorMap (ifd, bpp, map)
FILEBUF		*ifd;
int		bpp;
GIFMAP		*map;
{
    int		i;

    map->Length = 1 << bpp;

/* load the color map */
    for (i = 0; i < map->Length; i++) {
	FileRead (ifd, &map->Map[i].red, 1);
	FileRead (ifd, &map->Map[i].green, 1);
	FileRead (ifd, &map->Map[i].blue, 1);
    }

/* Initialize the rest */
    for ( ; i < 256; i++) {
	map->Map[i].red =
	map->Map[i].green =
	map->Map[i].blue = i;
    }

    return (ifd->err);
}


/************************************************************************/
LPFRAME gifrdr(ifile, outdepth, lpBPP)
/************************************************************************/
LPTR ifile;
int outdepth;
LPINT lpBPP;
{
    GIFHDR		hdr;
    GIFDESC		ImDesc;
    GIFMAP		GlobalMap;
    GIFMAP		LocalMap;
    BYTE		cTemp;
    LPTR		lpFileLine, lpCacheLine, lpRGBLine;
    LPFRAME		lpFrame, lpOldFrame;
    LPTR   		lp, lpOut;
    LPRGB		lpRGB;
    int			ifh;		/* file handle (unbuffered) */
    FILEBUF		ifd;		/* file descriptor (buffered) */
    BOOL		graymap;
    int			i, k, p;
    SPAN		span;
    int			xres;		/* pixels per inch */
    int			yres;		/* pixels per inch */
    int			npix;		/* image width (pixels) */
    int			nlin;		/* image height (pixels) */
    int			indepth;
    BYTE		codeSize;
    int			iCodeSize;
    int			iRowMapIndex;

    lpFileLine = NULL;
    lpCacheLine = NULL;
    lpOldFrame = NULL;
    if ((ifh = _lopen (ifile,OF_READ)) < 0) {
	Message (IDS_EOPEN, ifile );
	goto BadGIF;
    }

    FileFDOpenRdr (&ifd, ifh, LineBuffer[0], 16*1024);

/* initialize the Global and local color maps */
    gifInitColorMap (&GlobalMap);
    gifInitColorMap (&LocalMap);

/* read gif file header */
    if (gifReadHeader (&ifd, &hdr))
	goto BadRead;

/* get global color map, if any */
    if (hdr.GlobalMap) {
	if (gifReadColorMap (&ifd, hdr.bpp, &GlobalMap))
	    goto BadRead;
    }

/* look for start of image */
    while (1) {
	FileRead (&ifd, (LPTR) &cTemp, 1);
	if (ifd.err)
	    goto BadRead;

    /* test for image separator character */
	if (cTemp == GIFImSep)
	    break;

    /* test for terminator character (no image blocks in file?) */
	if (cTemp == GIFImSep)
	    goto BadRead;

    /* test for extension block character */
	if (cTemp == GIFExtBlk) {

	/* Skip over the extension block */

	/* read function code */
	    FileRead (&ifd, (LPTR) &cTemp, 1);

	    do {
	    /* read byte count */
		FileRead (&ifd, (LPTR) &cTemp, 1);

	    /* skip data bytes */
		if (cTemp)
		    FileSeek (&ifd, (long) cTemp, 1);
	    } while (cTemp);
	}
    }

/* now at the start of the first image */
    if (gifReadImDesc (&ifd, &ImDesc))
	goto BadRead;

/* read local color map, if any */
    if (ImDesc.LocalMap) {
	if (gifReadColorMap (&ifd, ImDesc.bpp, &LocalMap))
	    goto BadRead;
    }
    else {
	LocalMap = GlobalMap;
	ImDesc.bpp = hdr.bpp;
    }

/* check for gray map */
    graymap = TRUE;
    for (i = 0; (i < LocalMap.Length) && graymap; i++)
	graymap = (LocalMap.Map[i].red == LocalMap.Map[i].green)
		    && (LocalMap.Map[i].red == LocalMap.Map[i].blue);

    if (!Control.ColorEnabled && !graymap) {
	Print("Cannot load a color image");
	goto BadGIF;
    }

/* get width of image in pixels */
    npix = ImDesc.ImWidth;
    nlin = ImDesc.ImHeight;
    xres =
    yres = 75;

/* set outdepth */
    indepth = 3;
    if (outdepth < 0)
        outdepth = (graymap) ? 1 : 3;

/* allocate space for one line of the image (file) */
    if ( !AllocLines (&lpFileLine, 1, npix, 4)) {
	Message (IDS_EMEMALLOC);
	goto BadWrite;
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
	goto BadGIF;
    }
    lpOldFrame = frame_set (lpFrame);

    span.sx = 0;
    span.sy = 0;
    span.dx = npix;
    span.dy = 1;

/* convert the image */
    if (FileRead (&ifd, &codeSize, 1) == -1)
	goto BadRead;

    iCodeSize = codeSize;

    if (FileSeek (&ifd, 0L, 1) == -1)
	goto BadRead;

    if ( DecompressLZW_GIF (ifd.fh, NULL, 0, iCodeSize) < 0)
	goto BadRead;

    if (ImDesc.Interlaced)
	iRowMapIndex = 1;
    else
	iRowMapIndex = 0;

    span.sy = gifRowMap [iRowMapIndex].first;
    lpRGBLine = lpFileLine + npix;
    for (i = 0; i < nlin; i++) {
	AstralClockCursor (i, nlin);
	if (DecompressLZW_GIF (ifd.fh, lpFileLine, npix, iCodeSize) < 0) {
	    DecompressLZW_GIF (ifd.fh, NULL, 0, iCodeSize);
	    goto BadRead;
	}
	lp = lpFileLine;
	lpRGB = (LPRGB)lpRGBLine;
	for (k = 0; k < npix; k++, lp++, lpRGB++) {
	    p = *lp & 0xFF;
	    lpRGB->red = LocalMap.Map[p].red;
	    lpRGB->green = LocalMap.Map[p].green;
	    lpRGB->blue = LocalMap.Map[p].blue;
	}
	if (indepth != outdepth) {
	    ConvertData (lpRGBLine, indepth, npix, lpCacheLine, outdepth);
	    if ( !frame_write (&span, lpCacheLine, npix) ) {
		DecompressLZW_GIF (ifd.fh, NULL, 0, iCodeSize);
		goto BadWrite;
	    }
	}
	else {
	    if ( !frame_write (&span, lpRGBLine, npix) ) {
		DecompressLZW_GIF (ifd.fh, NULL, 0, iCodeSize);
		goto BadWrite;
	    }
	}
	span.sy += gifRowMap [iRowMapIndex].step;
	if (span.sy >= ImDesc.ImHeight) {
	    iRowMapIndex++;
	    span.sy = gifRowMap [iRowMapIndex].first;
	}
    }
    DecompressLZW_GIF (ifd.fh, NULL, 0, iCodeSize);

    if ( lpFileLine )
	FreeUp (lpFileLine);
    if ( lpCacheLine )
	FreeUp (lpCacheLine);
    if ( ifh >= 0 )
	_lclose (ifh);
    if (lpOldFrame)
  	frame_set(lpOldFrame);
    *lpBPP = ImDesc.bpp;
    return (lpFrame);

BadRead:
    Message( IDS_EREAD, ifile );
    goto BadGIF;

BadWrite:
    Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );

BadGIF:
    if ( lpFileLine )
	FreeUp( lpFileLine );
    if ( lpCacheLine )
	FreeUp( lpCacheLine );
    if ( ifh >= 0 )
	_lclose(ifh);
    if (lpOldFrame)
        frame_set(lpOldFrame);
    return(NULL);
}
