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


/************************************************************************/
int bmpwrt (ifile, lpFrame, lpRect, flag, fCompressed)
/************************************************************************/
LPTR ifile;
LPFRAME lpFrame;
LPRECT lpRect;
int flag;
BOOL fCompressed;
{
    BITMAPFILEHEADER	bmhdr;
    BITMAPINFOHEADER	infohdr;
    int		colormapentries;
    LPTR	lpColorLut, lpOut;
    WORD	iCount;
    RGBS	RGBmap[256], rgb;
    RGBQUAD	rgbq;
    int		bpl;
    int		ofh;		/* file handle (unbuffered) */
    FILEBUF	ofd;		/* file descriptor (buffered) */
    int		i, k, p;
    LPTR	lp, lpBuffer;
    FNAME	temp;
    WORD	wMini;
    int		npix, nlin;
    LPFRAME     lpOldFrame;
    RECT	rSave;

    if (!lpFrame)
 	return(-1);
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

/* initialize bit map file header */
    bmhdr.bfType = 0x4D42;
    bmhdr.bfSize = 0;
    bmhdr.bfReserved1 =
    bmhdr.bfReserved2 = 0;
    bmhdr.bfOffBits = 0;

/* initialize bit map info header */
    infohdr.biSize = 40; /* should be the same as sizeof(BITMAPINFOHEADER) */
    infohdr.biWidth = npix;
    infohdr.biHeight = nlin;
    infohdr.biPlanes = 1;
    infohdr.biBitCount = 0;
    infohdr.biCompression = BI_RGB;
    infohdr.biSizeImage = 0;
    infohdr.biXPelsPerMeter = (10000L * (long) lpFrame->Resolution) / 254L;
    infohdr.biYPelsPerMeter = (10000L * (long) lpFrame->Resolution) / 254L;
    infohdr.biClrUsed = 0;
    infohdr.biClrImportant = 0;

/* determine type of image to create */
    if ( flag == IDC_SAVECT ) {
    /* gray */
	infohdr.biBitCount = 8;
	colormapentries = 256;
	bpl = npix;
    }
    else if ( flag == IDC_SAVE24BITCOLOR ) {
    /* full color */
	infohdr.biBitCount = 24;
	colormapentries = 0;
	bpl = npix * 3;
    }
    else if ( flag == IDC_SAVE8BITCOLOR ) {
    /* mini color */
	infohdr.biBitCount = 8;
	colormapentries = 256;
	bpl = npix;
    }
    else if ( flag == IDC_SAVELA || flag == IDC_SAVESP ) {
    /* line art or scatter */
	infohdr.biBitCount = 1;
	colormapentries = 2;
	bpl = ((npix + 7) / 8);
    }
    else {
	Print ("Unknown flag value");
	return (-1);
    }
    bpl = (bpl + 3) & ~3;

    if ( !AllocLines (&lpBuffer, 1, max (bpl, npix), 3) ) {
	Message (IDS_EMEMALLOC);
	_lclose (ofh);
	return (-1);
    }

/* determine size of image, in bytes */
    infohdr.biSizeImage = bpl * infohdr.biHeight;

/* determine where image data starts */
    bmhdr.bfOffBits = 14 + infohdr.biSize + 4 * colormapentries;

/* determine size of file */
    bmhdr.bfSize = (bmhdr.bfOffBits + infohdr.biSizeImage) / 2;

/* write bit map file header */
    intelWriteWord (&ofd, bmhdr.bfType);
    intelWriteDWord (&ofd, bmhdr.bfSize);
    intelWriteWord (&ofd, bmhdr.bfReserved1);
    intelWriteWord (&ofd, bmhdr.bfReserved2);
    intelWriteDWord (&ofd, bmhdr.bfOffBits);
    if (ofd.err != 0)
	goto BadWrite;

/* write bit map info header */
    intelWriteDWord (&ofd, infohdr.biSize);
    intelWriteDWord (&ofd, infohdr.biWidth);
    intelWriteDWord (&ofd, infohdr.biHeight);
    intelWriteWord (&ofd, infohdr.biPlanes);
    intelWriteWord (&ofd, infohdr.biBitCount);
    intelWriteDWord (&ofd, infohdr.biCompression);
    intelWriteDWord (&ofd, infohdr.biSizeImage);
    intelWriteDWord (&ofd, infohdr.biXPelsPerMeter);
    intelWriteDWord (&ofd, infohdr.biYPelsPerMeter);
    intelWriteDWord (&ofd, infohdr.biClrUsed);
    intelWriteDWord (&ofd, infohdr.biClrImportant);
    if (ofd.err != 0)
	goto BadWrite;

    lpOldFrame = frame_set(lpFrame);
    switch (infohdr.biBitCount) {
    case 1:
    /* write the color map */
	rgbq.rgbRed =
	rgbq.rgbGreen =
	rgbq.rgbBlue =
	rgbq.rgbReserved = 0;
	FileWrite (&ofd, (LPTR) &rgbq, 4);
	rgbq.rgbRed =
	rgbq.rgbGreen =
	rgbq.rgbBlue = 255;
	FileWrite (&ofd, (LPTR) &rgbq, 4);
    
    /* write the image */
	for (i = rSave.bottom; i >= rSave.top; i--) {
	    AstralClockCursor (rSave.bottom-i, nlin );
	    if ( !(lp = frame_ptr (0, rSave.left, i, NO)) )
		goto BadRead;

	    ConvertData (lp, lpFrame->Depth, npix, lpBuffer+bpl, 1);
	    if ( flag == IDC_SAVESP)
		diffuse (0, i, 0, NULL, lpBuffer+bpl,
				npix, lpBuffer);
	    else {
		con2la (lpBuffer+bpl, npix, lpBuffer);
	    }
	    if ( FileWrite (&ofd, lpBuffer, bpl) != bpl )
		goto BadWrite;
	}
	break;

    case 8:
	if ( flag == IDC_SAVECT ) {
	/* write the color map */
	    rgbq.rgbRed =
	    rgbq.rgbGreen =
	    rgbq.rgbBlue =
	    rgbq.rgbReserved = 0;
	    for (i = 0; i < 256; i++) {
		rgbq.rgbRed = i;
		rgbq.rgbGreen = i;
		rgbq.rgbBlue = i;
		FileWrite (&ofd, (LPTR) &rgbq, 4);
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
	}
	else {
	    if ( !(lpColorLut = CreateOptimizedPalette(RGBmap)) )
		goto BadWrite;

	/* write the color map */
	    rgbq.rgbRed =
	    rgbq.rgbGreen =
	    rgbq.rgbBlue =
	    rgbq.rgbReserved = 0;
	    for (i = 0; i < 256; i++) {
		rgbq.rgbRed = RGBmap[i].red;
		rgbq.rgbGreen = RGBmap[i].green;
		rgbq.rgbBlue = RGBmap[i].blue;
		FileWrite (&ofd, (LPTR) &rgbq, 4);
	    }

	/* write the image */
	    for(i = rSave.bottom; i >= rSave.top; i--) {
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

		if (FileWrite (&ofd, lpBuffer, bpl) != bpl)
		    goto BadWrite;
	    }
	}
	break;

    case 24:
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

	    if (FileWrite (&ofd, lpBuffer, bpl) != bpl)
		goto BadWrite;
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
    goto BadBMP;

BadRead:
    Message (IDS_EREAD, (LPTR)Control.RamDisk);

BadBMP:
    _lclose (ofh);
    FreeUp (lpBuffer);
    lstrcpy (temp, ifile);
    unlink (temp);
    if (lpOldFrame)
	frame_set(lpOldFrame);
    return (-1);
}
