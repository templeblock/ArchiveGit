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


/************************************************************************/
BOOL tgaComp8Bit (FILEBUF *ofd, LPTR buf, int npix);

BOOL tgaComp8Bit (ofd, buf, npix)
FILEBUF		*ofd;
LPTR		buf;
int		npix;
{
    unsigned char	last;
    int			srcindex;
    LPTR		runStart;
    unsigned char	runCount;

    runCount = 1;
    runStart = buf;
    last = *buf++;
    srcindex = 1;
    while (srcindex < npix) {
	if (last != *buf) {
	    while ((srcindex < npix) && (runCount < 128) && (last != *buf)) {
		last = *buf;
		srcindex++;
		runCount++;
		buf++;
	    }
	    runCount--;
	    FileWrite (ofd, &runCount, 1);
	    FileWrite (ofd, runStart, runCount+1);
	}
	else {
	    while ((srcindex < npix) && (runCount < 128) && (last == *buf)) {
		srcindex++;
		runCount++;
		buf++;
	    }
	    runCount = (runCount-1) | 0x80;
	    FileWrite (ofd, &runCount, 1);
	    FileWrite (ofd, runStart, 1);
	}
	if (srcindex < npix) {
	    runCount = 1;
	    runStart = buf;
	    last = *buf++;
	    srcindex++;
	}
    }
    return (ofd->err ? TRUE : FALSE);
}


/************************************************************************/
extern BOOL tgaComp24Bit (FILEBUF *ofd, LPTR buf, int npix);

BOOL tgaComp24Bit (ofd, buf, npix)
FILEBUF		*ofd;
LPTR		buf;
int		npix;
{
    LPRGB		lpRGB;
    long		this, last;
    int			srcindex;
    LPRGB		runStart;
    unsigned char	runCount;

    lpRGB = (LPRGB) buf;
    runCount = 1;
    runStart = lpRGB;
    last = RGB2long ((*lpRGB));
    lpRGB++;
    srcindex = 1;
    while (srcindex < npix) {
	this = RGB2long ((*lpRGB));
	if (last != this) {
	    while ((srcindex < npix) && (runCount < 128) && (last != this)) {
		last = this;
		srcindex++;
		runCount++;
		lpRGB++;
		this = RGB2long ((*lpRGB));
	    }
	    runCount--;
	    FileWrite (ofd, &runCount, 1);
	    FileWrite (ofd, (LPTR) runStart, 3*(runCount+1));
	}
	else {
	    while ((srcindex < npix) && (runCount < 128) && (last == this)) {
		srcindex++;
		runCount++;
		lpRGB++;
		this = RGB2long ((*lpRGB));
	    }
	    runCount = (runCount-1) | 0x80;
	    FileWrite (ofd, &runCount, 1);
	    FileWrite (ofd, (LPTR) runStart, 3);
	}
	if (srcindex < npix) {
	    runCount = 1;
	    runStart = lpRGB;
	    last = this;
	    lpRGB++;
	    srcindex++;
	}
    }
    return (ofd->err ? TRUE : FALSE);
}


/************************************************************************/
int tgawrt (ifile, lpFrame, lpRect, flag, fCompressed)
/************************************************************************/
LPTR ifile;
LPFRAME lpFrame;
LPRECT lpRect;
int flag;
BOOL fCompressed;
{
    TGAHDR	PicHdr;
    LPTR	lpColorLut, lpOut;
    WORD	iCount;
    RGBS	RGBmap[256], rgb;
    int		bpl;
    int		ofh;		/* file handle (unbuffered) */
    FILEBUF	ofd;		/* file descriptor (buffered) */
    int		i, k, p;
    LPTR	lp, lpBuffer;
    FNAME	temp;
    WORD	wMini;
    LPFRAME 	lpOldFrame;
    RECT	rSave;
    int		npix, nlin;

    if (!lpFrame)
	return(-1);

/* image compression does not currently work */
    fCompressed = NO;

/* open the output file */
    if ((ofh = _lcreat (ifile,0)) < 0) {
	Message (IDS_EOPEN, ifile);
	return (-1);
    }

    lpOldFrame = NULL;
    if (lpRect)
	rSave = *lpRect;
    else
	{
	rSave.top = rSave.left = 0;
	rSave.bottom = lpFrame->Ysize-1;
	rSave.right = lpFrame->Xsize-1;
	}
    npix = RectWidth(&rSave);
    nlin = RectHeight(&rSave);

    FileFDOpenWrt (&ofd, ofh, LineBuffer[0], 16*1024);

/* initialize file header */
    PicHdr.textSize = 0;	/* no id field */
    PicHdr.XOffset = 0;
    PicHdr.YOffset = 0;
    PicHdr.x = npix;
    PicHdr.y = nlin;
    PicHdr.imType = 0;

/*************************************/
/* determine type of image to create */
/*************************************/

/* save line art and scatter as gray */
    if ( flag == IDC_SAVELA || flag == IDC_SAVESP )
	flag = IDC_SAVECT;

    switch (flag) {
    case IDC_SAVECT:
    /* gray */
	PicHdr.mapType = 1;
	PicHdr.mapOrig = 0;
	PicHdr.mapLength = 256;
	PicHdr.CMapBits = 24;
	PicHdr.dataType = (fCompressed) ? 9 : 1;
	PicHdr.dataBits = 8;
	bpl = npix;
	break;

    case IDC_SAVE8BITCOLOR:
    /* mini color */
	PicHdr.mapType = 1;
	PicHdr.mapOrig = 0;
	PicHdr.mapLength = 256;
	PicHdr.CMapBits = 24;
	PicHdr.dataType = (fCompressed) ? 9 : 1;
	PicHdr.dataBits = 8;
	bpl = npix;
	break;

    case IDC_SAVE24BITCOLOR:
    /* full color */
	PicHdr.mapType = 0;	/* no color map data */
	PicHdr.mapOrig = 0;
	PicHdr.mapLength = 0;
	PicHdr.CMapBits = 0;
	PicHdr.dataType = 2;
	PicHdr.dataType = (fCompressed) ? 10 : 2;
	PicHdr.dataBits = 24;
	bpl = npix * 3;
	break;

    default:
	Print ("Unknown flag value");
	goto BadRead;
    }

    if ( !AllocLines (&lpBuffer, 1, max (bpl, npix), 3) ) {
	Message (IDS_EMEMALLOC);
	_lclose (ofh);
	return (-1);
    }

/* write file header */
    FileWrite (&ofd, &PicHdr.textSize, 1);
    FileWrite (&ofd, &PicHdr.mapType, 1);
    FileWrite (&ofd, &PicHdr.dataType, 1);
    intelWriteWord (&ofd, PicHdr.mapOrig);
    intelWriteWord (&ofd, PicHdr.mapLength);
    FileWrite (&ofd, &PicHdr.CMapBits, 1);
    intelWriteWord (&ofd, PicHdr.XOffset);
    intelWriteWord (&ofd, PicHdr.YOffset);
    intelWriteWord (&ofd, PicHdr.x);
    intelWriteWord (&ofd, PicHdr.y);
    FileWrite (&ofd, &PicHdr.dataBits, 1);
    FileWrite (&ofd, &PicHdr.imType, 1);
    if (ofd.err != 0)
	goto BadWrite;

    lpOldFrame = frame_set(lpFrame);
    switch (flag) {
    case IDC_SAVECT:
    /* gray */
    /* write the color map */
	for (i = 0; i < 256; i++) {
	    RGBmap[0].red =
	    RGBmap[0].green =
	    RGBmap[0].blue = i;
	    FileWrite (&ofd, (LPTR) &RGBmap[0], 3);
	}

    /* write the image */
	for(i = rSave.bottom; i >= rSave.top; i--) {
	    AstralClockCursor (rSave.bottom-i, nlin);
	    if ( !(lp = frame_ptr (0, rSave.left, i, NO)) )
		goto BadRead;

	    ConvertData (lp, lpFrame->Depth, npix, lpBuffer, 1);
	    if (FileWrite (&ofd, lpBuffer, bpl) != bpl)
		goto BadWrite;
	}
	break;

    case IDC_SAVE8BITCOLOR:
    /* mini color */
	if ( !(lpColorLut = CreateOptimizedPalette(RGBmap)) )
	    goto BadWrite;

    /* write the color map */
	for (i = 0; i < 256; i++) {
	    FileWrite (&ofd, (LPTR) &RGBmap[i].blue, 1);
	    FileWrite (&ofd, (LPTR) &RGBmap[i].green, 1);
	    FileWrite (&ofd, (LPTR) &RGBmap[i].red, 1);
	}

    /* write the image */
	for (i = rSave.bottom; i >= rSave.top; i--) {
	    AstralClockCursor (rSave.bottom-i, nlin);
	    if ( !(lp = frame_ptr (0, rSave.left, i, NO)) )
		goto BadRead;

	    lpOut = lpBuffer;
	    iCount = npix;
	    while ( --iCount >= 0 ) {
		frame_getRGB( lp, &rgb );
		wMini = RGBtoMiniRGB( &rgb );
		*lpOut++ = *( lpColorLut + wMini );
		lp += DEPTH;
	    }

	    if ( fCompressed ) {
		if (tgaComp8Bit (&ofd, lpBuffer, npix))
		    goto BadWrite;
	    }
	    else {
		if (FileWrite (&ofd, lpBuffer, bpl) != bpl)
		    goto BadWrite;
	    }
	}
	break;

    case IDC_SAVE24BITCOLOR:
    /* full color */
    /* no color map */

    /* write the image */
	for(i = rSave.bottom; i >= rSave.top; i--) {
	    AstralClockCursor (rSave.bottom-i, nlin);
	    if ( !(lp = frame_ptr (0, rSave.left, i, NO)) )
		goto BadRead;

	    ConvertData (lp, lpFrame->Depth, npix, lpBuffer, 3);

	/* swap from rgb to bgr */
	    for (k = 0, lp = lpBuffer; k < npix; k++, lp += 3) {
		p = *lp;
		*lp = *(lp+2);
		*(lp+2) = p;
	    }

	    if ( fCompressed ) {
		if (tgaComp24Bit (&ofd, lpBuffer, npix))
		    goto BadWrite;
	    }
	    else {
		if (FileWrite (&ofd, lpBuffer, bpl) != bpl)
		    goto BadWrite;
	    }
	}
	break;
    }

/* clean up */
    if (FileFlush (&ofd))
	goto BadWrite;

    _lclose (ofh);
    FreeUp (lpBuffer);
    if (lpOldFrame)
	frame_set(lpOldFrame);
    return (0);

BadWrite:
    Message (IDS_EWRITE, ifile);
    goto BadTGA;

BadRead:
    Message (IDS_EREAD, (LPTR)Control.RamDisk);

BadTGA:
    _lclose (ofh);
    FreeUp (lpBuffer);
    lstrcpy (temp, ifile);
    unlink (temp);
    if (lpOldFrame)
	frame_set(lpOldFrame);
    return (-1);
}
