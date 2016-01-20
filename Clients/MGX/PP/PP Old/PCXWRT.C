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
 * FUNCTION pcxEncPut
 *	Put encoded image data into PCX file.
 */

static void	pcxEncPut (ofd, cnt, byte)
FILEBUF		*ofd;
unsigned char	cnt;
unsigned char	byte;
{
    unsigned char	data;

    if (ofd->err)
	return;

    if (cnt) {
	if ((cnt == 1) && (0xC0 != (0xC0 & byte))) {
	    (void) FileWrite (ofd, (LPTR) &byte, 1);
	}
	else {
	    cnt |= 0xC0;
	    (void) FileWrite (ofd, (LPTR) &cnt, 1);
	    (void) FileWrite (ofd, (LPTR) &byte, 1);
	}
    }
}


/************************************************************************
 * FUNCTION pcxEncLine
 *	Put line of image data into PCX file.
 */

static void	pcxEncLine (ofd, buf, len)
FILEBUF		*ofd;
LPTR		buf;
int		len;
{
    unsigned char	this, last;
    int			srcindex;
    unsigned char	runCount;

    runCount = 1;
    last = *buf;
    for (srcindex = 1; srcindex < len; srcindex++) {
	this = *(++buf);
	if (this == last) {
	    runCount++;
	    if (runCount == 63) {
		pcxEncPut (ofd, runCount, last);
		runCount = 0;
	    }
	}
	else {
	    if (runCount)
		pcxEncPut (ofd, runCount, last);
	    last = this;
	    runCount = 1;
	}
    }
    if (runCount)
	pcxEncPut (ofd, runCount, last);
}


/************************************************************************/
int pcxwrt (ifile, lpFrame, lpRect, flag, fCompressed)
/************************************************************************/
LPTR ifile;
LPFRAME lpFrame;
LPRECT lpRect;
int flag;
BOOL fCompressed;
{
    PCXHDR		hdr;
    PCXMAP		rgbentry;
    LPTR		lpColorLut, lpOut;
    WORD		iCount;
    RGBS		RGBmap[256], rgb;
    LPRGB		lpRGB;
    int			ofh;		/* file handle (unbuffered) */
    FILEBUF		ofd;		/* file descriptor (buffered) */
    int			i, k, p, npix, nlin;
    LPTR		lp, lpBuffer, lpFileLine;
    FNAME		temp;
    WORD		wMini;
    LPFRAME		lpOldFrame;
    RECT		rSave;

    if (!lpFrame)
        return(-1);
    /* open the output file */
    if ((ofh = _lcreat (ifile,0)) < 0) {
	Message (IDS_EOPEN, ifile);
	return (-1);
    }

    lpOldFrame = frame_set(lpFrame);
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

    lpBuffer = NULL;
    FileFDOpenWrt (&ofd, ofh, LineBuffer[0], 16*1024);

/* initialize file header */
    lp = (LPTR) & hdr;
    for (i = 0; i < sizeof (hdr); i++)
	*lp++ = 0;

    hdr.Manufacturer = 10;
    hdr.Version = 5;
    hdr.Encoding = 1;
    hdr.Xmax = npix-1;
    hdr.Ymax = nlin-1;
    hdr.HDpi = lpFrame->Resolution;
    hdr.VDpi = lpFrame->Resolution;
    hdr.NPlanes = 1;
    hdr.BytesPerLine = 0;
    hdr.PaletteInfo = 1;
    hdr.HscreenSize = 640;
    hdr.VscreenSize = 480;

/* determine type of image to create */
    switch (flag) {
    case IDC_SAVECT:
    /* gray */
	hdr.BitsPerPixel = 8;
	hdr.BytesPerLine = npix;
	hdr.PaletteInfo = 2;

    /* build the gra color map */
	for (i = 0; i < 256; i++) {
	    RGBmap[i].red =
	    RGBmap[i].green =
	    RGBmap[i].blue = i;
	}
	break;
 
    case IDC_SAVE24BITCOLOR:
    /* mini color */
	hdr.BitsPerPixel = 8;
	hdr.NPlanes = 3;
	hdr.BytesPerLine = npix;
	hdr.PaletteInfo = 1;
	break;

    case IDC_SAVE8BITCOLOR:
    /* mini color */
	hdr.BitsPerPixel = 8;
	hdr.BytesPerLine = npix;
	hdr.PaletteInfo = 1;

    /* build the mini color map lut */
	if ( !(lpColorLut = CreateOptimizedPalette(RGBmap)) )
	    goto BadWrite;
        break;

    case IDC_SAVELA:
    case IDC_SAVESP:
    /* line art or scatter */
	hdr.BitsPerPixel = 1;
	hdr.BytesPerLine = ((npix + 7) / 8);
	hdr.PaletteInfo = 2;
	hdr.Colormap[0] = hdr.Colormap[1] = hdr.Colormap[2] = 0;
	hdr.Colormap[3] = hdr.Colormap[4] = hdr.Colormap[5] = 255;
	break;

    default:
	Print ("Unknown flag value");
	goto BadPCX;
    }

/* round bytes per line to a 16 bit boundry */
    hdr.BytesPerLine = (hdr.BytesPerLine + 1) & ~1;

    if (!AllocLines (&lpBuffer, 1, max (hdr.BytesPerLine, npix), 3)) {
	Message (IDS_EMEMALLOC);
	goto BadPCX;
    }

/* write file header */
    FileWrite (&ofd, (LPTR) &hdr.Manufacturer, 1);
    FileWrite (&ofd, (LPTR) &hdr.Version, 1);
    FileWrite (&ofd, (LPTR) &hdr.Encoding, 1);
    FileWrite (&ofd, (LPTR) &hdr.BitsPerPixel, 1);
    intelWriteWord (&ofd, hdr.Xmin);
    intelWriteWord (&ofd, hdr.Ymin);
    intelWriteWord (&ofd, hdr.Xmax);
    intelWriteWord (&ofd, hdr.Ymax);
    intelWriteWord (&ofd, hdr.HDpi);
    intelWriteWord (&ofd, hdr.VDpi);
    FileWrite (&ofd, (LPTR) hdr.Colormap, 48);
    FileWrite (&ofd, (LPTR) &hdr.Reserved, 1);
    FileWrite (&ofd, (LPTR) &hdr.NPlanes, 1);
    intelWriteWord (&ofd, hdr.BytesPerLine);
    intelWriteWord (&ofd, hdr.PaletteInfo);
    intelWriteWord (&ofd, hdr.HscreenSize);
    intelWriteWord (&ofd, hdr.VscreenSize);
    FileWrite (&ofd, (LPTR) hdr.Filler, 54);
    if (ofd.err != 0)
	goto BadWrite;

/* write the image */
    if (flag == IDC_SAVE24BITCOLOR) {
	if (!AllocLines (&lpFileLine, 1, hdr.BytesPerLine, 1)) {
	    Message (IDS_EMEMALLOC);
	    goto BadPCX;
	}

	for (i = rSave.top; i <= rSave.bottom; i++) {
	    AstralClockCursor (i, nlin );
	    if ( !(lp = frame_ptr (0, rSave.left, i, NO)) ) {
		FreeUp (lpFileLine);
		goto BadRead;
	    }
	    ConvertData (lp, lpFrame->Depth, npix, lpBuffer, 3);

	/* save red plane */
	    lpRGB = (LPRGB)lpBuffer;
	    lpOut = lpFileLine;
	    for (k = 0; k < npix; k++) {
		*lpOut++ = (lpRGB++)->red;
	    }
	    pcxEncLine (&ofd, lpFileLine, hdr.BytesPerLine);

	/* save green plane */
	    lpRGB = (LPRGB)lpBuffer;
	    lpOut = lpFileLine;
	    for (k = 0; k < npix; k++)
		*lpOut++ = (lpRGB++)->green;
	    pcxEncLine (&ofd, lpFileLine, hdr.BytesPerLine);

	/* save blue plane */
	    lpRGB = (LPRGB)lpBuffer;
	    lpOut = lpFileLine;
	    for (k = 0; k < npix; k++)
		*lpOut++ = (lpRGB++)->blue;
	    pcxEncLine (&ofd, lpFileLine, hdr.BytesPerLine);
	    if (ofd.err) {
		FreeUp (lpFileLine);
		goto BadWrite;
	    }
	}
	FreeUp (lpFileLine);
    }
    else {
	for (i = rSave.top; i <= rSave.bottom; i++) {
	    AstralClockCursor (i, nlin );
	    if ( !(lp = frame_ptr (0, rSave.left, i, NO)) )
		goto BadRead;

	    switch (flag) {
	    case IDC_SAVECT:
		ConvertData (lp, lpFrame->Depth, npix, lpBuffer, 1);
		break;

	    case IDC_SAVE8BITCOLOR:
		lpOut = lpBuffer;
		iCount = npix;
		while ( --iCount >= 0 ) {
		    frame_getRGB( lp, &rgb );
		    wMini = RGBtoMiniRGB( &rgb );
		    *lpOut++ = *( lpColorLut + wMini );
		    lp += DEPTH;
		}
		break;

	    case IDC_SAVELA:
		ConvertData (lp, lpFrame->Depth, npix,
				    lpBuffer+hdr.BytesPerLine, 1);
		con2la (lpBuffer+hdr.BytesPerLine, npix, lpBuffer);
		break;

	    case IDC_SAVESP:
		ConvertData (lp, lpFrame->Depth, npix,
				    lpBuffer+hdr.BytesPerLine, 1);
		diffuse (0, i, 0, NULL, lpBuffer+hdr.BytesPerLine,
				    npix, lpBuffer);
		break;
	    }
	    pcxEncLine (&ofd, lpBuffer, hdr.BytesPerLine);
	    if (ofd.err)
		goto BadWrite;
	}

	if (hdr.BitsPerPixel == 8) {
	/* write extended color map */
	    rgbentry.Red = 12;
	    FileWrite (&ofd, (LPTR) &rgbentry.Red, 1);
	    for (i = 0; i < 256; i++) {
		rgbentry.Red = RGBmap[i].red;
		rgbentry.Green = RGBmap[i].green;
		rgbentry.Blue = RGBmap[i].blue;
		FileWrite (&ofd, (LPTR) &rgbentry, 3);
	    }
	}
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
    goto BadPCX;

BadRead:
    Message (IDS_EREAD, (LPTR)Control.RamDisk);

BadPCX:
    _lclose (ofh);
    FreeUp (lpBuffer);
    lstrcpy (temp, ifile);
    unlink (temp);
    if (lpOldFrame)
	frame_set(lpOldFrame);
    return (-1);
}
