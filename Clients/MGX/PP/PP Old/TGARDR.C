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
#include "targa.h"

#define MAXBUFSIZE 32767		/* must be at least 2048 */


/************************************************************************
 * FUNCTION tgaReadHeader
 *	Read header for targa file.
 */

static void tgaReadHeader (ifd, hdr)
FILEBUF		*ifd;
TGAHDR		*hdr;
{
    FileRead (ifd, &hdr->textSize, 1);
    FileRead (ifd, &hdr->mapType, 1);
    FileRead (ifd, &hdr->dataType, 1);
    intelReadWord (ifd, &hdr->mapOrig);
    intelReadWord (ifd, &hdr->mapLength);
    FileRead (ifd, &hdr->CMapBits, 1);
    intelReadWord (ifd, &hdr->XOffset);
    intelReadWord (ifd, &hdr->YOffset);
    intelReadWord (ifd, &hdr->x);
    intelReadWord (ifd, &hdr->y);
    FileRead (ifd, &hdr->dataBits, 1);
    FileRead (ifd, &hdr->imType, 1);
}


/*
** CheckPic - Check that a file is an uncompressed VDA, ICB, or TARGA image
**
** Usage:
**	CheckPic (head)
**		struct hdStruct *head;
**
** Return value:
**	 0 - File is an uncompressed VDA, ICB, or TARGA image
**	-1 - File is not an acceptable image
*/
/************************************************************************/
static int CheckPic (head)
/************************************************************************/
TGAHDR	 *head;
{
    unsigned char	imType;

/* A proper image file should indicate the presence or absence of a color map */

    if ((head->mapType != 0) && (head->mapType != 1))
	return (-1);
/*
** Acceptable formats for ICB/TARGA (RGB) Images
**
**	 1 -- Color Mapped
**	 2 -- RGB
**	 3 -- Black and White
**	 9 -- Compresssed Color Mapped
**	10 -- Compresssed RGB
**	11 -- Compresssed B&W Mapped
*/

    if ((head->dataType != 1) && (head->dataType != 2) &&
	    (head->dataType != 3) && (head->dataType != 9) &&
	    (head->dataType !=10) && (head->dataType != 11)) {
	Print ("Unsupported Data Type: %d", head->dataType);
	return (-1);
    }

/*
** If it is a color mapped image--- verify that color map is
** present and that data is at 256 levels, also varify CMapBits
*/
    if ((head->dataType == 1) || (head->dataType == 9)) {
	if (head->mapType != 1)
	    return (-1);
	if (head->dataBits != 8)
	    return (-1);
	if ((head->CMapBits != 16) && (head->CMapBits != 24) &&
		(head->CMapBits !=32))
	    return (-1);
    }

/*
** Varify that the number of bits per pixel is 8, 16, 24 or 32
*/
    if ((head->dataBits != 8) && (head->dataBits != 16) &&
	    (head->dataBits !=24) && (head->dataBits != 32))
	return (-1);

/*
** Varify that the image type (descriptor byte) is one we support
** (non-interleaved and either screen origin)
*/
    imType = head->imType & 0xE0;
    if ((imType != 0) && (imType != 0x20)) {
	Print ("Unsupported Interleave: %02x", imType);
	return (-1);
    }

    return (0);
}


/************************************************************************/
void tgaGetPixel (FILEBUF *ifd, int InputBytes,
		RGBS RGBmap [256], LPRGB lpRGB);

void		tgaGetPixel (ifd, InputBytes, RGBmap, lpRGB)
FILEBUF		*ifd;
int		InputBytes;
RGBS 		RGBmap [256];
LPRGB		lpRGB;
{
    unsigned char	ucTemp;
    WORD		wTemp;

    switch (InputBytes) {
    case 1:
	FileRead (ifd, (LPTR) &ucTemp, 1);
	lpRGB->red = RGBmap[ucTemp].red;
	lpRGB->green = RGBmap[ucTemp].green;
	lpRGB->blue = RGBmap[ucTemp].blue;
	break;

    case 2:
	intelReadWord (ifd, &wTemp);
	lpRGB->red = (wTemp >> 7) & 0xF8;
	lpRGB->green = (wTemp >> 2) & 0xF8;
	lpRGB->blue = (wTemp << 3) & 0xF8;
	break;

    case 3:
	FileRead (ifd, (LPTR) &lpRGB->blue, 1);
	FileRead (ifd, (LPTR) &lpRGB->green, 1);
	FileRead (ifd, (LPTR) &lpRGB->red, 1);
	break;

    case 4:
	FileRead (ifd, (LPTR) &lpRGB->blue, 1);
	FileRead (ifd, (LPTR) &lpRGB->green, 1);
	FileRead (ifd, (LPTR) &lpRGB->red, 1);
	FileRead (ifd, (LPTR) &wTemp, 1);
	break;
    }
}


/************************************************************************/
void	tgaGetCPixel (TGASTATE *tgaState, LPRGB lpRGB);

void	tgaGetCPixel (tgaState, lpRGB)
TGASTATE 	*tgaState;
LPRGB		lpRGB;
{
    char	byte;

/* if count is zero, we must start a new packet */
    if (tgaState->cnt == 0) {

    /* get packet header */
	FileRead (tgaState->ifd, &byte, 1);

    /* set count for packet */
	tgaState->cnt = 0x7F & byte;

    /* set type of packet */
	tgaState->rawPacket = (0x80 & byte) == 0;

    /* check for run-length packet */
	if (0x80 & byte) {
	    tgaState->rawPacket = FALSE;
	    tgaGetPixel (tgaState->ifd, tgaState->inputBytes,
			    tgaState->RGBmap, &tgaState->pixel);
	    *lpRGB = tgaState->pixel;
	}
	else {
	    tgaState->rawPacket = TRUE;
	    tgaGetPixel (tgaState->ifd, tgaState->inputBytes,
			    tgaState->RGBmap, lpRGB);
	}
    }

/* if raw packet, get next pixel from the file */
    else if (tgaState->rawPacket) {
	tgaState->cnt--;
	tgaGetPixel (tgaState->ifd, tgaState->inputBytes,
			tgaState->RGBmap, lpRGB);
    }

/* if run-length packet, return another copy of the saved pixel */
    else {
	tgaState->cnt--;
	*lpRGB = tgaState->pixel;
    }
}


/************************************************************************/
LPFRAME tgardr (ifile, outdepth, lpBPP)
/************************************************************************/
LPTR ifile;
int outdepth;
LPINT lpBPP;
{
    LPTR	lpName, lp;
    LPTR	lpFileLine, lpCacheLine;
    LPRGB	lpRGB;
    LPFRAME	lpFrame, lpOldFrame;
    int		ifh;		/* file handle (unbuffered) */
    FILEBUF	ifd;		/* file descriptor (buffered) */
    RGBS	RGBmap [256];
    int		row;
    int		i, j;
    int		indepth;
    int		npix, nlin;
    SPAN	span;
    WORD	wTemp;
    int		mapBytes;
    int		mapLength;
    int		colorMapped;	/* color mapped image flag */
    int		Compressed;	/* flag to indicate if Compressed image */
    int		InputBytes;	/* number o bytes per input pixel */
    TGAHDR	PicHdr;
    BOOL	graymap;
    TGASTATE	tgaState;

    lpFileLine = NULL;
    lpCacheLine = NULL;
    lpOldFrame = NULL;

    if ((ifh = _lopen(ifile,OF_READ)) < 0) {
	Message( IDS_EOPEN, ifile );
	goto BadTGA;
    }

    FileFDOpenRdr (&ifd, ifh, LineBuffer[0], 16*1024);

/* Open the file header and see if it is a valid TARGA image */
    tgaReadHeader (&ifd, &PicHdr);
    if (ifd.err)
	goto BadRead;

/* Check for an acceptable image type */
    if (CheckPic (&PicHdr))
	goto BadRead;

/* skip past image description */
    if (PicHdr.textSize) {
	if (FileSeek (&ifd, (long) PicHdr.textSize, 1) == -1)
	    goto BadRead;
    }

    InputBytes = (PicHdr.dataBits + 7) / 8;	/* byte/pixel on the disk */

/* Set up the color map */
/* remember whether or not the image is color mapped. */
    if ((PicHdr.dataType == 1) || (PicHdr.dataType == 9))
	colorMapped = 1;
    else colorMapped = 0;

/* Read color map if provided */
    graymap = FALSE;
    if ((PicHdr.mapType == 1) && (PicHdr.mapLength > 0)) {
	mapBytes = (PicHdr.CMapBits + 7) / 8;

    /* We either want to read the color map data, or skip over it. */
	if (colorMapped == 1) {

	/* initialize color map entries */
	    for (i = 0; i < 256; i++) {
		RGBmap [i].red =
		RGBmap [i].green =
		RGBmap [i].blue = 0;
	    }
	    j = PicHdr.mapOrig;
	    graymap = TRUE;
	    for (i = PicHdr.mapLength; i; i--, j++) {
		switch (mapBytes) {
		case 2:
		    intelReadWord (&ifd, &wTemp);
		    RGBmap[j].red = (wTemp >> 7) & 0xF8;
		    RGBmap[j].green = (wTemp >> 2) & 0xF8;
		    RGBmap[j].blue = (wTemp << 3) & 0xF8;
		    break;

		case 3:
		    FileRead (&ifd, (LPTR) &RGBmap[j].blue, 1);
		    FileRead (&ifd, (LPTR) &RGBmap[j].green, 1);
		    FileRead (&ifd, (LPTR) &RGBmap[j].red, 1);
		    break;

		case 4:
		    FileRead (&ifd, (LPTR) &RGBmap[j].blue, 1);
		    FileRead (&ifd, (LPTR) &RGBmap[j].green, 1);
		    FileRead (&ifd, (LPTR) &RGBmap[j].red, 1);
		    FileRead (&ifd, (LPTR) &wTemp, 1);
		    break;

		default:
		    Print ("Unsupported color map size (%d)", mapBytes);
		    goto BadTGA;
		    break;
		}
		if (graymap) {
		    graymap = (RGBmap[j].red == RGBmap[j].green)
				&& (RGBmap[j].red == RGBmap[j].blue);
		}
	    }
	}
	else {
	    mapLength = mapBytes * PicHdr.mapLength;
	    if (mapLength) {
		if (FileSeek (&ifd, (long) mapLength, 1) == -1)
		    goto BadRead;
	    }
	}
    }
    else colorMapped = 0;

    if (!Control.ColorEnabled && !graymap) {
	Print("Cannot load a color image");
	goto BadTGA;
    }

    npix = PicHdr.x;
    nlin = PicHdr.y;
    indepth = 3;
    if (outdepth < 0)
        outdepth = (graymap) ? 1 : 3;

/* allocate space for one line of the image */
    if ( !AllocLines (&lpFileLine, 1, npix, indepth)) {
	Message (IDS_EMEMALLOC );
	goto BadWrite;
    }

    if ( !AllocLines (&lpCacheLine, 1, npix, outdepth)) {
	Message (IDS_EMEMALLOC );
	goto BadWrite;
    }

/* Create the image frame store */
    lpFrame = frame_open (outdepth, npix, nlin, 75);
    if ( !lpFrame ) {
	Message (IDS_ESCROPEN, (LPTR)Control.RamDisk);
	goto BadTGA;
    }
    lpOldFrame = frame_set (lpFrame);

    span.sx = 0;
    span.sy = 0;
    span.dx = npix;
    span.dy = 1;

/* See if compressed file or not */
    Compressed = (PicHdr.dataType > 8) ? 1 : 0;

    if (Compressed) {
	tgaState.RGBmap = RGBmap;
	tgaState.ifd = &ifd;
	tgaState.inputBytes = InputBytes;
	tgaState.cnt = 0;
    }

    lp = (indepth == outdepth) ? lpFileLine : lpCacheLine;

    for (row = 0; row < PicHdr.y; row++) {
	span.sy = (PicHdr.imType & 0x20) ? row : nlin - row - 1;
	AstralClockCursor( row, nlin );
	if (Compressed) {
	    for (lpRGB = (LPRGB)lpFileLine, i = 0; i < npix; i++, lpRGB++)
		tgaGetCPixel (&tgaState, lpRGB);
	}
	else {
	    for (lpRGB = (LPRGB)lpFileLine, i = 0; i < npix; i++, lpRGB++)
		tgaGetPixel (&ifd, InputBytes, RGBmap, lpRGB);
	}

	if (indepth != outdepth)
	    ConvertData ((LPTR)lpFileLine, indepth, npix,
			lpCacheLine, outdepth);
	if (!frame_write (&span, lp, npix))
	    goto BadWrite;
    }

    if ( lpFileLine )
	FreeUp (lpFileLine);
    if ( lpCacheLine )
	FreeUp (lpCacheLine);
    if ( ifh >= 0 )
	_lclose (ifh);
    if (lpOldFrame)
 	frame_set(lpOldFrame);
    *lpBPP = indepth * 8;
    return (lpFrame);

BadRead:
    Message( IDS_EREAD, ifile );
    goto BadTGA;

BadWrite:
    Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );

BadTGA:
    if (lpFileLine)
	FreeUp (lpFileLine);
    if (lpCacheLine)
	FreeUp (lpCacheLine);
    if (ifh >= 0)
	_lclose(ifh);
    if (lpOldFrame)
 	frame_set(lpOldFrame);
    return (NULL);
}
