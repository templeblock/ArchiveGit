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


/************************************************************************/
int		gifWriteImDesc (ofd, imDesc, ColorMap, codeSize)
FILEBUF		*ofd;
GIFDESC		*imDesc;
GIFMAP		*ColorMap;
int		codeSize;
{
    int		i;
    BYTE	cSize;

/* write the color map */
    for (i = 0; i < ColorMap->Length; i++) {
	FileWrite (ofd, &ColorMap->Map[i].red, 1);
	FileWrite (ofd, &ColorMap->Map[i].green, 1);
	FileWrite (ofd, &ColorMap->Map[i].blue, 1);
    }

/* write the image header */
    FileWrite (ofd, ",", 1);
    intelWriteWord (ofd, imDesc->ImLeft);
    intelWriteWord (ofd, imDesc->ImTop);
    intelWriteWord (ofd, imDesc->ImWidth);
    intelWriteWord (ofd, imDesc->ImHeight);
    FileWrite (ofd, &imDesc->Zero, 1);

/* write the code size */
    cSize = codeSize;
    FileWrite (ofd, &cSize, 1);

/* flush the buffer because LZW compression uses raw IO */
    if (FileSeek (ofd, 0L, 1) == -1)
	return (-1);
    return (ofd->err);
}


/************************************************************************/
int gifwrt (ifile, lpFrame, lpRect, flag, fCompressed)
/************************************************************************/
LPTR ifile;
LPFRAME lpFrame;
LPRECT lpRect;
int flag;
BOOL fCompressed;
{
    GIFHDR	hdr;
    GIFDESC	imDesc;
    GIFMAP	ColorMap;

    LPTR	lpColorLut, lpOut;
    WORD	iCount;
    RGBS	RGBmap[256], rgb;
    int		ofh;		/* file handle (unbuffered) */
    FILEBUF	ofd;		/* file descriptor (buffered) */
    int		i, j;
    LPTR	lp, lpBuffer;
    FNAME	temp;
    WORD	wMini;
    BYTE	cTerm;
    int		codeSize;
    LPFRAME	lpOldFrame;
    RECT	rSave;
    int		npix, nlin;

/* full color not supported, force mini color */
    if ( flag == IDC_SAVE24BITCOLOR )
        flag = IDC_SAVE8BITCOLOR;

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

/* initialize file header */
    hdr.ScnWidth = npix;
    hdr.ScnHeight = nlin;
    hdr.Background = 0;
    hdr.GlobalMap = TRUE;
    hdr.ColorRes = 8;

/* initialize image descriptor */
    imDesc.ImLeft = 0;
    imDesc.ImTop = 0;
    imDesc.ImWidth = npix;
    imDesc.ImHeight = nlin;
    imDesc.Zero = 0;

/* determine type of image to create */
    if ( flag == IDC_SAVECT ) {
    /* gray */
	hdr.ColorRes = 8;
	hdr.bpp = 8;
	codeSize = 8;
    }
    else if ( flag == IDC_SAVE8BITCOLOR ) {
    /* mini color */
	hdr.ColorRes = 8;
	hdr.bpp = 8;
	codeSize = 8;
    }
    else if ( flag == IDC_SAVELA || flag == IDC_SAVESP ) {
    /* line art or scatter */
	hdr.ColorRes = 1;
	hdr.bpp = 1;
	codeSize = 2;
    }
    else {
	Print ("Unknown flag value");
	return (-1);
    }

    if ( !AllocLines (&lpBuffer, 1, npix, 3) ) {
	Message (IDS_EMEMALLOC);
	_lclose (ofh);
	return (-1);
    }

/* set raw info byte */
    hdr.RawImInfo = 0x80 | ((hdr.ColorRes-1) << 4) | (hdr.bpp-1);

/* write file header */
    FileWrite (&ofd, "GIF87a", 6);
    intelWriteWord (&ofd, hdr.ScnWidth);
    intelWriteWord (&ofd, hdr.ScnHeight);
    FileWrite (&ofd, &hdr.RawImInfo, 1);
    FileWrite (&ofd, &hdr.Background, 1);
    FileWrite (&ofd, "", 1);
    if (ofd.err != 0)
	goto BadWrite;

    lpOldFrame = frame_set(lpFrame);
    switch (flag) {
    case IDC_SAVESP:
    case IDC_SAVELA:

    /* initialize the color map */
	ColorMap.Length = 2;
	ColorMap.Map[0].red =
	ColorMap.Map[0].green =
	ColorMap.Map[0].blue = 0x00;
	ColorMap.Map[1].red =
	ColorMap.Map[1].green =
	ColorMap.Map[1].blue = 0xFF;
    
    /* write image descriptor, color map and code size */
	if (gifWriteImDesc (&ofd, &imDesc, &ColorMap, codeSize))
	    goto BadWrite;

    /* write the image */
	CompressLZW_GIF (ofd.fh, NULL, 0, codeSize);
	lpOut = lpBuffer + npix;
	for (i = rSave.top; i <= rSave.bottom; i++) {
	    AstralClockCursor (i, nlin );
	    if ( !(lp = frame_ptr (0, rSave.left, i, NO)) ) {
		CompressLZW_GIF (ofd.fh, NULL, 0, codeSize);
		goto BadRead;
	    }

	/* convert to grey */
	    ConvertData (lp, lpFrame->Depth, npix, lpOut, 1);
	
	/* now do scatter or line art conversion */
	    if ( flag == IDC_SAVESP)
		diffuse (0, i, 0, NULL, lpOut, npix, lpBuffer);
	    else
		con2la (lpOut, npix, lpBuffer);
	
	/* convert back to depth one, but with pixel values of 0 and 1 */
	    ConvertData (lpBuffer, 0, npix, lpOut, 1);
	    for (lp = lpOut, j = 0; j < npix; j++, lp++)
		*lp = (*lp) ? 0 : 1;

	/* compress the data */
	    if (CompressLZW_GIF (ofd.fh, lpOut, npix, codeSize) == -1) {
		CompressLZW_GIF (ofd.fh, NULL, 0, codeSize);
		goto BadWrite;
	    }
	}
	break;

    case IDC_SAVECT:
    /* initialize the color map */
	ColorMap.Length = 256;
	for (i = 0; i < 256; i++) {
	    ColorMap.Map[i].red =
	    ColorMap.Map[i].green =
	    ColorMap.Map[i].blue = i;
	}
    
    /* write image descriptor, color map and code size */
	if (gifWriteImDesc (&ofd, &imDesc, &ColorMap, codeSize))
	    goto BadWrite;

    /* write the image */
	CompressLZW_GIF (ofd.fh, NULL, 0, codeSize);
	for(i = rSave.top; i <= rSave.bottom; i++) {
	    AstralClockCursor (i, nlin);
	    if ( !(lp = frame_ptr (0, rSave.left, i, NO)) )
		goto BadRead;

	    ConvertData (lp, lpFrame->Depth, npix, lpBuffer, 1);
	    if (CompressLZW_GIF (ofd.fh, lpBuffer, npix, codeSize) == -1) {
		CompressLZW_GIF (ofd.fh, NULL, 0, codeSize);
		goto BadWrite;
	    }
	}
	break;

    case IDC_SAVE8BITCOLOR:
	if ( !(lpColorLut = CreateOptimizedPalette(RGBmap)) )
	    goto BadWrite;

    /* initialize the color map */
	ColorMap.Length = 256;
	for (i = 0; i < 256; i++) {
	    ColorMap.Map[i].red = RGBmap[i].red;
	    ColorMap.Map[i].green = RGBmap[i].green;
	    ColorMap.Map[i].blue = RGBmap[i].blue;
	}
    
    /* write image descriptor, color map and code size */
	if (gifWriteImDesc (&ofd, &imDesc, &ColorMap, codeSize))
	    goto BadWrite;

    /* write the image */
	if (CompressLZW_GIF (ofd.fh, NULL, 0, codeSize) == -1)
	    goto BadWrite;

	for(i = rSave.top; i <= rSave.bottom; i++) {
	    AstralClockCursor (i, nlin);
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
	    if (CompressLZW_GIF (ofd.fh, lpBuffer, npix, codeSize) == -1) {
		CompressLZW_GIF (ofd.fh, NULL, 0, codeSize);
		goto BadWrite;
	    }
	}
	break;
    }
    CompressLZW_GIF (ofd.fh, NULL, 0, codeSize);
    cTerm = 0;
    if (_lwrite (ofd.fh, &cTerm, 1) != 1)
	goto BadWrite;

    cTerm = GIFTerm;
    if (_lwrite (ofd.fh, &cTerm, 1) != 1)
	goto BadWrite;

/* clean up */
    _lclose (ofh);
    FreeUp (lpBuffer);
    if (lpOldFrame)
	frame_set(lpOldFrame);
    return (0);

BadWrite:
    Message (IDS_EWRITE, ifile);
    goto BadGIF;

BadRead:
    Message (IDS_EREAD, (LPTR)Control.RamDisk);

BadGIF:
    _lclose (ofh);
    FreeUp (lpBuffer);
    lstrcpy (temp, ifile);
    unlink (temp);
    if (lpOldFrame)
	frame_set(lpOldFrame);
    return (-1);
}
