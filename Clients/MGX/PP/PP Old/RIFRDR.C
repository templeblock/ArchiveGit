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
#include "riff.h"


/************************************************************************
 * FUNCTION
 *	uncompress a compressed scan line
 *	return TRUE if scan is OK, FALSE if it would not parse
 */

static BOOL unrlescan (LPTR rlebuff, LPTR outbuff, short bytes, short recCount);

static BOOL unrlescan (rlebuff, outbuff, bytes, recCount)
LPTR	rlebuff;
LPTR	outbuff;
short	bytes;
short	recCount;
{
    BYTE	subtag;
    BYTE	count;

/* just copy first byte */
    *outbuff++ = *rlebuff++;
    bytes--;

/* decode remaining bytes */
    while ((recCount > 2) && (bytes > 0)) {
	subtag = *rlebuff++;
	count = *rlebuff++;
	recCount -= 2;
	bytes -= count;
	switch (subtag) {
	case st_data:
	    count++;
	    recCount -= count;
	    bytes--;
	    while (count--)
		*outbuff++ = *rlebuff++;
	    break;

	case st_run:
	    while (count--)
		*outbuff++ = *rlebuff;
	    rlebuff++;
	    recCount--;
	    break;

	default:
	    return (FALSE);
	}
    }
    return (TRUE);
}


/************************************************************************
 * FUNCTION
 *	uncompress a compressed scan line
 *	return TRUE if scan is OK, FALSE if it would not parse
 */

static BOOL unhuffscan (LPTR huffbuff, LPTR outbuff,
			short bytes, short recCount);

static BOOL unhuffscan (huffbuff, outbuff, bytes, recCount)
LPTR	huffbuff;
LPTR	outbuff;
short	bytes;
short	recCount;
{
    signed char		ac;
    unsigned char	byte;
    char	bitsin;
    LPTR	inptr;
    LPTR	outptr;
    LPTR	instop;
    LPTR	outstop;

/* this macro gets the next bit from input */
/* it is left in the sign bit of "ac" */
#define getbit ac += ac; if (--bitsin == 0) \
	{ \
	    ac = *inptr++; \
	    bitsin = 8; \
	}

    outptr = outbuff;
    outstop = outbuff + bytes;
    instop = huffbuff + recCount + 1;
    *outptr++ = *huffbuff;
    inptr = huffbuff+1;
    ac = *inptr++;
    bitsin = 8;
    while ((inptr < instop) && (outptr < outstop)) {

    /*
	these nested "if" statements parse the huffman-code
	tree for RIFF compressed file encoding.
	at the leaves of the tree are statements like so:
	
		*outptr++ = nnn;
	
	these statements write a sample delta byte to the output buffer
	the parse tree nodes are commented with the bitstrings
	that they map onto.  an ellipsis in the comment indicates
	an internal tree node.
    */
	if (ac >= 0) {

	/* 0 */
	    *outptr++ = 0;
	    getbit
	}
	else {

	/* 1... */
	    getbit
	    if (ac >= 0) {

	    /* 10... */
		getbit
		if (ac >= 0) {

		/* 100 */
		    *outptr++ = 1;
		    getbit
		}
		else {

		/* 101 */
		    *outptr++ = 255;
		    getbit
		}
	    }
	    else {

	    /* 11... */
		getbit
		if (ac >= 0) {

		/* 110... */
		    getbit
		    if (ac >= 0) {

		    /* 1100 */
			*outptr++ = 2;
			getbit
		    }
		    else {

		    /* 1101 */
			*outptr++ = 254;
			getbit
		    }
		}
		else {

		/* 111... */
		    getbit
		    if (ac >= 0) {

		    /* 1110... */
			getbit
			if (ac >= 0) {

			/* 11100 */
			    *outptr++ = 3;
			    getbit
			}
			else {

			/* 11101 */
			    *outptr++ = 253;
			    getbit
			}
		    }
		    else {

		    /* 1111... */
			getbit
			if (ac >= 0) {

			/* 11110... */
			    getbit
			    if (ac >= 0) {

			    /* 111100 */
				*outptr++ = 4;
				getbit
			    }
			    else {

			    /* 111101 */
				*outptr++ = 252;
				getbit
			    }
			}
			else {

			/* 11111... */
			    getbit
			    if (ac >= 0) {

			    /* 111110... */
				getbit
				if (ac >= 0) {

				/* 1111100 */
				    *outptr++ = 5;
				    getbit
				}
				else {

				/* 1111101 */
				    *outptr++ = 251;
				    getbit
				}
			    }
			    else {

			    /* 111111... */
				getbit
				if (ac >= 0) {

				/* 1111110... */
				    getbit
				    if (ac >= 0) {

				    /* 11111100 */
					*outptr++ = 6;
					getbit
				    }
				    else {

				    /* 11111101 */
					*outptr++ = 250;
					getbit
				    }
				}
				else {

				/* 1111111... */
				    getbit
				    if (ac >= 0) {

				    /* 11111110... */
					getbit
					if (ac >= 0) {

					/* 111111100 */
					    *outptr++ = 7;
					    getbit
					}
					else {

					/* 111111101 */
					    *outptr++ = 249;
					    getbit
					}
				    }
				    else {

				    /* 11111111... */
				    /* decode an eight-bit delta which follows */
				    /* the escape code */
					byte = 0;
					getbit
					if (ac < 0)
					    byte++;
					byte += byte;
					getbit
					if (ac < 0)
					    byte++;
					byte += byte;
					getbit
					if (ac < 0)
					    byte++;
					byte += byte;
					getbit
					if (ac < 0)
					    byte++;
					byte += byte;
					getbit
					if (ac < 0)
					    byte++;
					byte += byte;
					getbit
					if (ac < 0)
					    byte++;
					byte += byte;
					getbit
					if (ac < 0)
					    byte++;
					byte += byte;
					getbit
					if (ac < 0)
					    byte++;

				    /* check for bad data */
					if (byte < 8 || byte > 248)
					    return (FALSE);
					*outptr++ = byte;
					getbit
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }
    return (TRUE);
}


/************************************************************************
 * P-FUNCTION rifReadScanLine
 */

static int rifReadScanLine (FILEBUF *ifd, short flags, short flat,
		short bytes, LPTR lpFileBuf, LPTR lpCacheBuf);

static int rifReadScanLine (ifd, flags, flat, bytes, lpFileBuf, lpCacheBuf)
FILEBUF		*ifd;
short		flags;
short		flat;
short		bytes;
LPTR		lpFileBuf;
LPTR		lpCacheBuf;
{
    RIFSCANHDR	slhdr;
    int		i;

/* check for uncompressed data */
    if (flags & hf_uncompressed) {
	FileRead (ifd, lpCacheBuf, bytes);
	return (ifd->err);
    }

/* get the scan line header */
    FileRead (ifd, &slhdr.tag, 1);
    FileRead (ifd, &slhdr.byte_zero, 1);
    motReadWord (ifd, (WORD FAR *)&slhdr.record_count);

    if ((slhdr.record_count > flat) || (slhdr.record_count < 6)) {
	Print ("Record count is bad");
	return (-1);
    }

/* get rest of this scan line data */
    FileRead (ifd, lpFileBuf+1, (long) (slhdr.record_count - 4));
    *lpFileBuf = slhdr.byte_zero;

    switch (slhdr.tag) {
    case 0:
	if (!unhuffscan (lpFileBuf, lpCacheBuf, bytes, slhdr.record_count-4)) {
	    Print ("Error from huffman decoder");
	    return (-1);
	}
	break;

    case 1:
	for (i = 0; i < bytes; i++)
	    *lpCacheBuf++ = *lpFileBuf++;
	break;

    case 2:
	if (!unrlescan (lpFileBuf, lpCacheBuf, bytes, slhdr.record_count-4)) {
	    Print ("Error from rle decoder");
	    return (-1);
	}
	break;

    default:
	Print ("Unknown scan line tag (%d)", slhdr.tag);
	return (-1);
	break;
    }

    return (ifd->err);
}


/************************************************************************
 * P-FUNCTION rifReadHeader
 */

static int rifReadHeader (ifd, hdr)
FILEBUF		*ifd;
RIFHDR		*hdr;
{
    motReadDWord (ifd, &hdr->revnum);
    motReadWord (ifd, (WORD FAR *) &hdr->nrows);
    motReadWord (ifd, (WORD FAR *) &hdr->ncols);
    motReadWord (ifd, (WORD FAR *) &hdr->flags);
    motReadWord (ifd, (WORD FAR *) &hdr->storage_type);
    motReadWord (ifd, (WORD FAR *) &hdr->nsamples);
    motReadDWord (ifd, &hdr->chunks[0]);
    motReadDWord (ifd, &hdr->chunks[1]);
    motReadDWord (ifd, &hdr->chunks[2]);
    motReadDWord (ifd, &hdr->chunks[3]);
    motReadWord (ifd, (WORD FAR *) &hdr->resolution);
    FileRead (ifd, (LPTR) hdr->reserved, 32);

dbg ("rif: revnum: %08lx", hdr->revnum);
dbg ("rif: nrows: %d", hdr->nrows);
dbg ("rif: ncols: %d", hdr->ncols);
dbg ("rif: flags: %x", hdr->flags);
dbg ("rif: nsamples: %d", hdr->nsamples);
dbg ("rif: storage_type: %d", hdr->storage_type);
dbg ("rif: chunks[0]: %08lx", hdr->chunks[0]);
dbg ("rif: chunks[1]: %08lx", hdr->chunks[1]);
dbg ("rif: chunks[2]: %08lx", hdr->chunks[2]);
dbg ("rif: chunks[3]: %08lx", hdr->chunks[3]);
dbg ("rif: resolution: %d", hdr->resolution);

    if (ifd->err)
	return (ifd->err);

    if ((hdr->nsamples < 2) || (256 < hdr->nsamples)) {
	Print ("Invalid number of sample values (%d)", hdr->nsamples);
	return (-1);
    }

    switch (hdr->storage_type < 0) {
    case st_gray:
    case 7:
	break;

    default:
	Print ("Invalid storage_type (%d)", hdr->storage_type);
	return (-1);
    }

    return (0);
}


/************************************************************************/
LPFRAME rifrdr(ifile, outdepth, lpBPP)
/************************************************************************/
LPTR ifile;
int outdepth;
LPINT lpBPP;
{
    LPTR   		lpName;
    LPTR		lpFileLine, lpCacheLine, lpRGBLine, lpRawLine;
    LPFRAME		lpFrame, lpOldFrame;
    LPTR   		lp, lpOut, lpPrev;
    LPRGB		lpRGB;
    int			ifh;		/* file handle (unbuffered) */
    FILEBUF		ifd;		/* file descriptor (buffered) */
    RIFHDR		hdr;
    RGBS		RGBmap [256], rgb;
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

    int			nchunks;
    int			flat;
    BYTE		transfer_table[256];

    lpFileLine = NULL;
    lpCacheLine = NULL;
    lpOldFrame = NULL;

    if (!OKToOpen (ifile, &lpName))
	return (NULL);

    if ((ifh = _lopen (ifile,OF_READ)) < 0) {
	Message (IDS_EOPEN, ifile );
	goto BadRIFF;
    }

    FileFDOpenRdr (&ifd, ifh, LineBuffer[0], 16*1024);

/* read rif file header */
    if (rifReadHeader (&ifd, &hdr) == -1)
	goto BadRead;

/* get width of image in pixels */
    npix = hdr.ncols;
    nlin = hdr.nrows;
    xres = 
    yres = (hdr.resolution) ? hdr.resolution : 72;

/* set number of chunks, based on storage type */
    switch (hdr.storage_type) {
    case st_gray:
    case st_vlt:
	nchunks = 1;
	indepth = outdepth = 1;
	FileBPL = npix;
	break;

    case st_rgb:
    case st_hsv:
    case st_cmy:
	nchunks = 3;
	indepth = outdepth = 3;
	FileBPL = npix;
	break;

    case st_cmyk:
	nchunks = 4;
	indepth = 4;
	outdepth = 3;
	FileBPL = npix;
	break;

    case 7:
	nchunks = 1;
	indepth = 4;
	outdepth = 3;
	FileBPL = 4*npix;
	break;
    }

/* set the transfer function */
    if (hdr.flags & hf_transfer)
	FileRead (&ifd, transfer_table, hdr.nsamples);
    else {

    /* assume normalized transfer */
	for (i = 0; i < hdr.nsamples; i++)
	    transfer_table[i] = i;
    }

/* convert transfer table for number of samples desired */
    if (256 != hdr.nsamples) {
	k = hdr.nsamples/2;
	for (i = 0; i < hdr.nsamples; i++) {
	    j = transfer_table[i];
	    j = (256L * (long) j + k) / hdr.nsamples;
	    transfer_table[i] = j;
	}
    }

    graymap = FALSE;

/* set outdepth */
    if (outdepth < 0)
        outdepth = indepth;

/* determine the size of a flat scan line */
/* (largest record coming in from data) */
    flat = (4 + FileBPL) & ~1;

/* allocate space for one line of the image (file) */
    if ( !AllocLines (&lpFileLine, 1, flat+16, 1)) {
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
	goto BadRIFF;
    }
    lpOldFrame = frame_set (lpFrame);

    span.sx = 0;
    span.sy = 0;
    span.dx = npix;
    span.dy = 1;

/* convert the image */
    switch (hdr.storage_type) {
    case st_gray:
	if (FileSeek (&ifd, hdr.chunks[0], 0) == -1)
	    goto BadRead;

    /* read image deltas into frame */
	for (i = 0; i < nlin; i++) {
	    span.sy = i;
	    AstralClockCursor (i, nlin);

	    if (rifReadScanLine (&ifd, hdr.flags, flat,
				npix, lpFileLine, lpCacheLine))
		goto BadRead;

	    if (!frame_write (&span, lpCacheLine, npix))
		goto BadWrite;
	}

	if (!(hdr.flags & hf_uncompressed)) {

	/* now undo the delta calculations in memory */
	/* undo vertical deltas */
	    for (i = 1; i < nlin; i++) {

	    /* get pointers to this line and previous line */
		if ( !(lp = frame_ptr (0, 0, i, YES)) )
		    goto BadRead;
		if ( !(lpPrev = frame_ptr (0, 0, i-1, NO)) )
		    goto BadRead;

		for (j = 0; j < npix; j++)
		    *lp++ += *lpPrev++;
	    }

	/* undo horizontal deltas */
	    for (i = 0; i < nlin; i++) {

	    /* get pointers to this line */
		if ( !(lp = frame_ptr (0, 0, i, YES)) )
		    goto BadRead;

		lpOut = lp;
		for (j = 0; j < npix-1; j++, lpOut++)
		    *(lpOut+1) += *lpOut;

	    /* apply transfer function */
		for (j = 0; j < npix; j++, lp++)
		    *lp = 255 - transfer_table [*lp & 0xFF];
	    }
	}

	break;

    case 7:
	if (FileSeek (&ifd, hdr.chunks[0], 0) == -1)
	    goto BadRead;

    /* allocate space for one line of the image (raw) */
	if ( !AllocLines (&lpRawLine, 1, 4*npix, 1)) {
	    Message (IDS_EMEMALLOC);
	    goto BadWrite;
	}

    /* read image deltas into frame */
	for (i = 0; i < nlin; i++) {
	    span.sy = i;
	    AstralClockCursor (i, nlin);

	    if (rifReadScanLine (&ifd, hdr.flags, flat,
				4*npix, lpFileLine, lpRawLine)) {
		FreeUp (lpRawLine);
		goto BadRead;
	    }

	/* convert from 32 bit (xrgb) color to 24 bit (rgb) color */
	    for (lpRGB = (LPRGB) lpRawLine, lp = lpRawLine, j = 0;
			j < npix;
			j++) {
		lp++;
		rgb.red = *lp++;
		rgb.green = *lp++;
		rgb.blue = *lp++;
		*lpRGB++ = rgb;
	    }
	    ConvertData (lpRawLine, 3, npix, lpCacheLine, outdepth);

	    if (!frame_write (&span, lpCacheLine, npix)) {
		FreeUp (lpRawLine);
		goto BadWrite;
	    }
	}
	break;

    default:
	goto BadRead;
    }

    if ( lpFileLine )
	FreeUp (lpFileLine);
    if ( lpCacheLine )
	FreeUp (lpCacheLine);
    if ( ifh >= 0 )
	_lclose (ifh);
    if (lpOldFrame)
        frame_set(lpOldFrame);
    *lpBPP = 8;
    return (lpFrame);

BadRead:
    Message( IDS_EREAD, ifile );
    goto BadRIFF;

BadWrite:
    Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );

BadRIFF:
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