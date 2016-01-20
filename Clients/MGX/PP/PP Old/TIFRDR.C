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

/* TIFF defines */
#define TIF_BYTE 1
#define TIF_ASCII 2
#define TIF_SHORT 3
#define TIF_LONG 4
#define TIF_RATIONAL 5
#define TIF_MM (0x4D4D)
#define TIF_II (0x4949)
#define MYORDER (TIF_II)

typedef struct {
    long StripsPerImage;
    long RowsPerStrip;
    long FAR *StripByteCounts;
    long FAR *StripOffsets;
    LPTR StripPtr;
    long ImagePos;
} STRIPS_DESC;

STRIPS_DESC Strips;
static short order;

/************************************************************************/
VOID ReadHorzDiff8 (LPTR lpImage, int iCount)
/************************************************************************/
{
    int col;

    for (col = 1; col < iCount; col++)
	lpImage[col] += lpImage[col-1];
}

/************************************************************************/
VOID ReadHorzDiff24 (LPTR lpImage, int iCount)
/************************************************************************/
{
    int col;

    for (col = 3; col < iCount; col++)
	lpImage[col] += lpImage[col-3];
}

/************************************************************************/
void image_read_startstrip ()
/************************************************************************/
{
    int		k;
    long	strip_start;

    strip_start = 0;
    for (k = 0;
	    (k < Strips.StripsPerImage) && (Strips.ImagePos > strip_start);
	    k++) {
	strip_start += Strips.StripByteCounts[k];
    }

    Strips.ImagePos = strip_start;
}

/************************************************************************/
int image_read (LPTR ptr, unsigned size_of_ptr, int count, int ifh)
/************************************************************************/
{
    long req_bytes, strip_start, strip_end, strip_pos;
    long bytes_read, bytes_in_strip, bytes_to_read;
    int k;
    
    if (Strips.StripsPerImage == 1)
        return (_lread (ifh, ptr, size_of_ptr * count));
    else {
        req_bytes = (long)size_of_ptr * (long)count;
        strip_start = 0;
        for (k = 0; k < Strips.StripsPerImage; k++) {
            strip_end = strip_start + Strips.StripByteCounts[k] - 1;
            if ((Strips.ImagePos >= strip_start)
			&& (Strips.ImagePos <= strip_end)) {
                strip_pos = Strips.StripOffsets[k]
			+ (Strips.ImagePos - strip_start);
                _llseek (ifh, strip_pos, 0);
                bytes_in_strip = strip_end - Strips.ImagePos + 1;
                if (bytes_in_strip > req_bytes)
                    bytes_to_read = req_bytes;
                else
                    bytes_to_read = bytes_in_strip;
                bytes_read = _lread (ifh, ptr, bytes_to_read);
                Strips.ImagePos += bytes_read;
		return (bytes_read / size_of_ptr);
	    }
            strip_start = strip_end + 1;
	}
    }
    return (-1);
}

/************************************************************************/
LPFRAME tifrdr (ifile, out_depth, lpBPP)
/************************************************************************/
LPTR ifile;
int out_depth;
LPINT lpBPP;
{
SPAN	span;
long	ifd_ptr, xresden, xresnum, yresden, yresnum, colormap, temp;
short	sTemp;
long	lTemp;
WORD	npix, nlin;
int	holder, FileBPL, bpp, photomet, ifh, i, in_depth, k;
int	compressed, maxsampval, samples, planes, predictor;
TAG	tag;
LPFRAME	lpFrame, lpOldFrame;
LPTR	lp, lpOut, lpFileLine, lpCacheLine, lpRGBLine;
LPRGB	lpRGB;
LPWORD	lpRed, lpGreen, lpBlue;
LPWORD	lpWord;
RGBS	RGBmap[256];
long	lRedPlane, lGreenPlane, lBluePlane;
BOOL	bMustShiftDown;
BOOL	preloadOK;
long	offset;

/* default values */
preloadOK = TRUE;
lpFileLine = NULL;
lpCacheLine = NULL;
lpOldFrame = NULL;
Strips.StripsPerImage = 1;
Strips.StripByteCounts = NULL;
Strips.StripOffsets = NULL;
Strips.ImagePos = 0;

/* assumed to be full res data, tag 0xff value = 1 */
bpp = 1; /* Lineart is the default */
photomet = 0; /* min value is white */
xresnum = 0;
xresden = 1;
yresnum = 0;
yresden = 1;
colormap = 0;
maxsampval = 255;
samples = 1;
planes = 1;
compressed = NO;
predictor = 1;

/* end default values */
if ((ifh = _lopen (ifile, OF_READ)) < 0)
	{
	Message (IDS_EOPEN, ifile);
	goto BadTiff;
	}
if (_lread (ifh, (LPTR)&order, 2) != 2) /* read byte order */
	goto BadRead;
if (order != TIF_II && order != TIF_MM)
	{ Message (IDS_EIMGBAD, ifile); goto BadTiff; }
if (flexiread (ifh, (LPTR)&holder, 2) != 2) /* read tiff version number */
	goto BadRead;
if (holder != 0x2a) /* only version 42 allowed for now */
	{ Message (IDS_EIMGVERSION, ifile); }
if (flexiread (ifh, (LPTR)&ifd_ptr, 4) != 4) /* read ifd pointer */
	goto BadRead;

/****************************************************************/

next_ifd:
if (_llseek (ifh, ifd_ptr, 0) == -1) /* seek to location of ifd */
	goto BadRead;
if (flexiread (ifh, (LPTR)&holder, 2) != 2) /* read # of tag field entries */
	goto BadRead;
for (i=0; i<holder; i++)
	{
	if (flexiread (ifh, (LPTR)&tag, 12) != 12) /* read the tag entry */
		goto BadRead;
	switch (tag.tagno)
	    {
	    case 0xff: /* sub-file type */
		if (tag.value != 1) /* want only full res data */
			{
			for (i=0; i<holder-1; i++)
			if (flexiread (ifh, (LPTR)&tag, 12) != 12)
				goto BadRead;
			if (flexiread (ifh, (LPTR)&ifd_ptr, 4) != 4)
				goto BadRead;
			if (ifd_ptr == 0)
				goto BadTiff;
				goto next_ifd;
			}
		break;
	    case 0x100: /* image width */
		npix = tag.value;
		break;
	    case 0x101: /* image height */
		nlin = tag.value;
		break;
	    case 0x102: /* bits per pixel */
		if (tag.value > 8)
			bpp = 24;
		else	bpp = tag.value; /* get the bits per pixel */
		break;
	    case 0x103: /* compression */
		if (tag.value == 5)
			{
			compressed = YES;
			preloadOK = FALSE;
			}
		else
		if (tag.value == 1)
			compressed = NO;
		else /* other compressed images, we don't handle */
			{ Message (IDS_EIMGCOMP, ifile); goto BadTiff; }
		break;
	    case 0x106: /* photometeric interpretation */
		if ((tag.value != 0) && (tag.value != 1) &&
		    (tag.value != 2) && (tag.value != 3))
			{ Message (IDS_EIMGPHOTO, ifile); }
		photomet = tag.value;
		break;
	    case 0x10a: /* fill order */
		if (tag.value != 1) /* not default, try to continue */
			{
			Message (IDS_EIMGFILL, ifile);
			preloadOK = FALSE;
			}
		break;
	    case 0x111: /* strip offsets */
		if (tag.length == 1)
		        {
			Strips.StripOffsets = (long FAR *)
				Alloc (tag.length * sizeof (LONG));
			if (!Strips.StripOffsets)
			    {
			    Message (IDS_EMEMALLOC);
			    goto BadRead;
			    }
		        Strips.StripOffsets[0] = tag.value;
			}
		else	{
			Strips.StripOffsets = (long FAR *)
				Alloc (tag.length * sizeof (LONG));
			if (!Strips.StripOffsets)
			    {
			    Message (IDS_EMEMALLOC);
			    goto BadRead;
			    }
			/* save the current file position */
			temp = _llseek (ifh, 0L, 1);
			/* find the first offset */
			/* seek to line loc */
			if (_llseek (ifh, tag.value, 0) == -1)
				goto BadRead;
			/* read the first line pointer */
			for (k = 0; k < tag.length; ++k)
			    {
			    if (tag.type == TIF_LONG)
				{
				if (flexiread (ifh, (LPTR)&lTemp, 4) == 0)
				    goto BadRead;
				}
			    else {
				if (flexiread (ifh, (LPTR)&sTemp, 2) == 0)
				    goto BadRead;
				lTemp = sTemp;
				}
			    Strips.StripOffsets[k] = lTemp;
			    }
			/* seek to where we were */
			if (_llseek (ifh, temp, 0) == -1)
				goto BadRead;
			}
		break;
	    case 0x112: /* orientation */
		if (tag.value != 1) /* not default, try to continue */
			{
			Message (IDS_EIMGORIENT, ifile);
			preloadOK = FALSE;
			}
		break;
	    case 0x115: /* samples per pixel */
		samples = tag.value;
		break;
	    case 0x116:
	        Strips.RowsPerStrip = tag.value;
	        Strips.StripsPerImage =
			(nlin + Strips.RowsPerStrip - 1)/Strips.RowsPerStrip;
	        
	        break;
	    case 0x117:
		if (tag.length == 1)
			{
			/* save the pos of byte counts */
			Strips.StripByteCounts = (long FAR *)
				Alloc (tag.length * sizeof (LONG));
			if (!Strips.StripByteCounts)
			    {
			    Message (IDS_EMEMALLOC);
			    goto BadRead;
			    }
			Strips.StripByteCounts[0] = tag.value;
			}
		else	{
			Strips.StripByteCounts = (long FAR *)
				Alloc (tag.length * sizeof (LONG));
			if (!Strips.StripByteCounts)
			    {
			    Message (IDS_EMEMALLOC);
			    goto BadRead;
			    }
			/* save the current file position */
			temp = _llseek (ifh, 0L, 1);
			/* find the first offset */
			/* seek to line loc */
			if (_llseek (ifh, tag.value, 0) == -1)
				goto BadRead;
			/* read the first line pointer */
			for (k = 0; k < tag.length; ++k)
			    {
			    if (tag.type == TIF_LONG)
				{
				if (flexiread (ifh, (LPTR)&lTemp, 4) == 0)
				    goto BadRead;
				}
			    else {
				if (flexiread (ifh, (LPTR)&sTemp, 2) == 0)
				    goto BadRead;
				lTemp = sTemp;
				}
			    Strips.StripByteCounts[k] = lTemp;
			    }
			/* seek to where we were */
			if (_llseek (ifh, temp, 0) == -1)
				goto BadRead;
			}
	        break;
	    case 0x118: /* min sample value */
		break;
	    case 0x119: /* max sample value */
		maxsampval = tag.value;
		break;
	    case 0x11a: /* xresolution */
		xresnum = tag.value;
		break;
	    case 0x11b: /* yresolution */
		yresnum = tag.value;
		break;
	    case 0x11c: /* planar configuration */
		planes = tag.value;
		break;
	    case 0x13d: /* predictor */
	        predictor = tag.value;
	        break;
	    case 0x12d: /* color response curves */
	    case 0x140: /* color map */
		if (tag.length == 3*256)
			colormap = tag.value;
		break;
	    default:	/* return */
		break;
	    }
	}
if (Strips.StripOffsets == NULL)
	{
	Print ("Strip offsets missing");
	goto BadTiff;
	}

/* if byte counts tag missing, fake it */
if (Strips.StripByteCounts == NULL)
	{
	Strips.StripByteCounts = (long FAR *)
			Alloc (Strips.StripsPerImage * sizeof (LONG));
	if (!Strips.StripByteCounts)
	    {
	    Message (IDS_EMEMALLOC);
	    goto BadRead;
	    }
	for (k = 0; k < Strips.StripsPerImage-1; k++)
	    {
	    Strips.StripByteCounts[k] =
		    Strips.StripOffsets[k+1] - Strips.StripOffsets[k];
	    if (Strips.StripByteCounts[k] < 1)
		{
		Print ("Computed Strip Byte Count is Negative");
		goto BadTiff;
		}
	    }
	Strips.StripByteCounts[k] = 1L << 30;
	}

/* if preload is still ok, check for multiple strips in order */
if (preloadOK)
	{
	if (Strips.StripsPerImage > 1)
		{
		offset = Strips.StripOffsets[0];
		for (k = 1; (k < Strips.StripsPerImage) && preloadOK; k++)
			{
			offset += Strips.StripByteCounts[k-1];
			if (offset != Strips.StripOffsets [k])
				preloadOK = FALSE;
			}
		}
	}

if (xresnum)
	{
	if (_llseek (ifh, xresnum, 0) == -1) /* seek to loc of xres */
		goto BadRead;
	if (flexiread (ifh, (LPTR)&xresnum, 4) != 4) /* read xres numerator */
		goto BadRead;
	if (flexiread (ifh, (LPTR)&xresden, 4) != 4) /* read xres denominator */
		goto BadRead;
	xresnum /= xresden;
	}
else	xresnum = 75;

if (yresnum)
	{
	if (_llseek (ifh, yresnum, 0) == -1) /* seek to loc of yres */
		goto BadRead;
	if (flexiread (ifh, (LPTR)&yresnum, 4) != 4) /* read yres numerator */
		goto BadRead;
	if (flexiread (ifh, (LPTR)&yresden, 4) != 4) /* read yres denominator */
		goto BadRead;
	yresnum /= yresden;
	}
else	yresnum = 75;

if (colormap)
	{
	bpp = 24;
	if (_llseek (ifh, colormap, 0) == -1) /* seek to loc of colormap */
		goto BadRead;
	if (_lread (ifh, LineBuffer[0], 2*3*256) != 2*3*256)
		goto BadRead;
	if (MYORDER != order)
		{
		lpWord = (LPWORD)LineBuffer[0];
		for (i=0; i<3*256; i++, lpWord++)
			swapw (lpWord);
		}

	lpWord = (LPWORD)LineBuffer[0];
	bMustShiftDown = NO;
	for (i=0; i<3*256; i++)
		{ // See if the pallete needs to be shifted down
		if (*lpWord++ > 255)
			{
			bMustShiftDown = YES;
			break;
			}
		}

	lpRGB = RGBmap;
	lpRed = (LPWORD)LineBuffer[0];
	lpGreen = lpRed + 256;
	lpBlue = lpGreen + 256;
	for (i=0; i<256; i++)
		{
		if (bMustShiftDown)
			{
			lpRGB->red   = *lpRed++ >> 8;
			lpRGB->green = *lpGreen++ >> 8;
			lpRGB->blue  = *lpBlue++ >> 8;
			}
		else	{
			lpRGB->red   = *lpRed++;
			lpRGB->green = *lpGreen++;
			lpRGB->blue  = *lpBlue++;
			}
		lpRGB++;
		}
	}

#ifdef PPVIDEO
if (xresden != 2 || yresden != 2)
	{
	Message (IDS_EVIDEOREAD);
	goto BadTiff;
	}
#endif

/* seek to beginning of line */
if (_llseek (ifh, Strips.StripOffsets[0], 0) == -1)
	goto BadRead;

if (samples == 3)
	bpp = 24;

if (bpp == 24)
	{
	if (!Control.ColorEnabled)
		{
		Print ("Cannot load a color image with the grayscale version of Picture Publisher.  Contact Astral Development at 603-432-6800 for information on how to upgrade to Picture Publisher Plus.");
		goto BadRead;
		}
	in_depth = 3;
	}
else	in_depth = 1;

if (out_depth < 0)
    out_depth = in_depth;
if (in_depth != out_depth)
    {
    if (!AllocLines (&lpCacheLine, 1, npix, out_depth))
	{
	Message (IDS_EMEMALLOC);
	goto BadWrite;
	}
    }

/* Create the image frame store */
if (! (lpFrame = frame_open (out_depth, npix, nlin, (int)xresnum)))
	{ Message (IDS_ESCROPEN, (LPTR)Control.RamDisk); goto BadTiff; }
lpOldFrame = frame_set (lpFrame);
span.sx = 0;
span.sy = 0;
span.dx = npix;
span.dy = 1;

if (compressed)
	if (DecompressLZW (ifh, NULL, 0) < 0) /* Initialize */
		Message (IDS_ECOMPRESSINIT);

if (bpp == 1)
	{
	FileBPL = (npix+7)/8;
	if (!AllocLines (&lpFileLine, 1, FileBPL, 1))
		{
		Message (IDS_EMEMALLOC);
		goto BadWrite;
		}
	if (lpCacheLine == NULL)
		{
		if (!AllocLines (&lpCacheLine, 1, npix, out_depth))
			{
			Message (IDS_EMEMALLOC);
			goto BadWrite;
			}
		}
	for (i=0; i<nlin; i++)
		{
		AstralClockCursor (i, nlin);
		if (compressed)
			{
			if (DecompressLZW (ifh, lpFileLine, FileBPL) < 0)
				{
				DecompressLZW (ifh, NULL, 0); /* Terminate */
				goto BadRead;
				}
			}
		else	{
			if (image_read(lpFileLine, FileBPL, 1, ifh) == 0)
				goto BadRead;
			}
		if (photomet)
		    negate (lpFileLine, (long)FileBPL);
		ConvertData (lpFileLine, 0, npix, lpCacheLine, out_depth);

		if (!frame_write (&span, lpCacheLine, npix))
			goto BadWrite;
		span.sy++;
		}
	}
else
if (bpp == 4)
	{
	bpp = 8;
	FileBPL = (npix+1)/2;
	if (!AllocLines (&lpFileLine, 1, 2*FileBPL, 1))
		{
		Message (IDS_EMEMALLOC);
		goto BadWrite;
		}
	if (lpCacheLine == NULL)
		{
		if (!AllocLines (&lpCacheLine, 1, npix, out_depth))
			{
			Message (IDS_EMEMALLOC);
			goto BadWrite;
			}
		}
	for (i = 0; i < nlin; i++)
		{
		AstralClockCursor (i, nlin);
		if (compressed)
			{
			if (DecompressLZW (ifh, lpFileLine, FileBPL) < 0)
				{
				DecompressLZW (ifh, NULL, 0); /* Terminate */
				goto BadRead;
				}
			}
		else	{
			if (image_read(lpFileLine, FileBPL, 1, ifh) == 0)
				goto BadRead;
			}
		if (!photomet)
			negate (lpFileLine, (long)FileBPL);

		lp = lpFileLine + FileBPL - 1;
		lpOut = lpFileLine + 2*FileBPL - 1;
		for (k = 0; k < FileBPL; k++)
			{
			*lpOut-- = (*lp << 4) & 0xF0;
			*lpOut-- = (*lp--) & 0xF0;
			}
		ConvertData (lpFileLine, 1, npix, lpCacheLine, out_depth);

		if (!frame_write (&span, lpCacheLine, npix))
			goto BadWrite;

		span.sy++;
		}
	}
else
if (bpp == 6 || (bpp == 8 && maxsampval <= 63))
	{
	bpp = 8;
	FileBPL = npix;
	if (!AllocLines (&lpFileLine, 1, FileBPL, 1))
		{
		Message (IDS_EMEMALLOC);
		goto BadWrite;
		}
	for (i=0; i<nlin; i++)
		{
		AstralClockCursor (i, nlin);
		if (compressed)
			{
			if (DecompressLZW (ifh, lpFileLine, FileBPL) < 0)
				{
				DecompressLZW (ifh, NULL, 0); /* Terminate */
				goto BadRead;
				}
			}
		else	{
			if (image_read(lpFileLine, FileBPL, 1, ifh) == 0)
				goto BadRead;
			}
		map (ShiftUp2Lut, lpFileLine, FileBPL);
		if (!photomet)
			negate (lpFileLine, (long)FileBPL);

	        if (in_depth != out_depth)
 		    {
	            ConvertData (lpFileLine, 1, npix, 
				 (LPTR)lpCacheLine, out_depth);
		    if (!frame_write (&span, lpCacheLine, npix))
			goto BadWrite;
		    }
		else
		    {
		    if (!frame_write (&span, lpFileLine, npix))
			goto BadWrite;
		    }

		span.sy++;
		}
	}
else
if (bpp == 8)
	{
	FileBPL = npix;
	if (!AllocLines (&lpFileLine, 1, FileBPL, 1))
		{
		Message (IDS_EMEMALLOC);
		goto BadWrite;
		}
	/* if all the right circumstances... */
	if (!compressed && (out_depth == 1))
		{
		span.sy = frame_preload (ifh, !photomet);
		if (span.sy == -1)
			goto BadRead;
		else
		if (span.sy == -2)
			goto BadWrite;
		}
	for (i=span.sy; i<nlin; i++)
		{
		AstralClockCursor (i, nlin);
		if (compressed)
			{
			if (DecompressLZW (ifh, lpFileLine, FileBPL) < 0)
				{
				DecompressLZW (ifh, NULL, 0); /* Terminate */
				goto BadRead;
				}
			if (predictor == 2)
			    ReadHorzDiff8 (lpFileLine, FileBPL);
			}
		else	{
			if (image_read(lpFileLine, FileBPL, 1, ifh) == 0)
				goto BadRead;
			}
		if (!photomet)
			negate (lpFileLine, (long)FileBPL);

	        if (in_depth != out_depth)
 		    {
	            ConvertData (lpFileLine, 1, npix, 
				 (LPTR)lpCacheLine, out_depth);
		    if (!frame_write (&span, lpCacheLine, npix))
			goto BadWrite;
		    }
		else
		    {
		    if (!frame_write (&span, lpFileLine, npix))
			goto BadWrite;
		    }

		span.sy++;
		}
	}
else
if (bpp == 24 && colormap) // Palette color
	{
	FileBPL = npix;
	if (!AllocLines (&lpFileLine, 1, FileBPL, 4))
		{
		Message (IDS_EMEMALLOC);
		goto BadWrite;
		}

	lpRGBLine = lpFileLine + FileBPL;
	for (i=span.sy; i<nlin; i++)
		{
		AstralClockCursor (i, nlin);
		if (compressed)
			{
			if (DecompressLZW (ifh, lpFileLine, FileBPL) < 0)
				{
				DecompressLZW (ifh, NULL, 0); /* Terminate */
				goto BadRead;
				}
			if (predictor == 2)
			    ReadHorzDiff8 (lpFileLine, FileBPL);
			}
		else	{
			if (image_read(lpFileLine, FileBPL, 1, ifh) == 0)
				goto BadRead;
			}

		lp = lpFileLine;
		lpRGB = (LPRGB)lpRGBLine;
		for (k=0; k<FileBPL; k++)
			{
			lpRGB->red   = RGBmap[ *lp ].red;
			lpRGB->green = RGBmap[ *lp ].green;
			lpRGB->blue  = RGBmap[ *lp ].blue;
			lp++;
			lpRGB++;
			}
	        if (in_depth != out_depth)
 		    {
	            ConvertData ((LPTR)lpRGBLine, in_depth, npix, 
				 (LPTR)lpCacheLine, out_depth);
		    if (!frame_write (&span, lpCacheLine, npix))
			goto BadWrite;
		    }
		else
		    {
		    if (!frame_write (&span, lpRGBLine, npix))
			goto BadWrite;
		    }
		span.sy++;
		}
	}
else
if (bpp == 24 && planes == 1)
	{
	FileBPL = npix * 3;
	if (!AllocLines (&lpFileLine, 1, FileBPL, 1))
		{
		Message (IDS_EMEMALLOC);
		goto BadWrite;
		}

	/* if all the right circumstances... */
	if (!compressed && (out_depth == 3))
		{
		span.sy = frame_preload (ifh, !photomet);
		if (span.sy == -1)
			goto BadRead;
		else
		if (span.sy == -2)
			goto BadWrite;
		}
	for (i=span.sy; i<nlin; i++)
		{
		AstralClockCursor (i, nlin);
		if (compressed)
			{
			if (DecompressLZW (ifh, lpFileLine, FileBPL) < 0)
				{
				DecompressLZW (ifh, NULL, 0); /* Terminate */
				goto BadRead;
				}
			if (predictor == 2)
			    ReadHorzDiff24 (lpFileLine, FileBPL);
			}
		else	{
			if (image_read(lpFileLine, FileBPL, 1, ifh) == 0)
				goto BadRead;
			}
		if (in_depth != out_depth)
		    {
	            ConvertData ((LPTR)lpFileLine, in_depth, npix, 
				 (LPTR)lpCacheLine, out_depth);
		    if (!frame_write (&span, lpCacheLine, npix))
			goto BadWrite;
		    }
		else
		    {
		    if (!frame_write (&span, lpFileLine, npix))
			goto BadWrite;
		    }
		span.sy++;
		}
	}
else
if (bpp == 24 && planes != 1)
	{
	if (compressed)
		{
		Print ("Cannot load a long color image (multiple planes) if it is compressed with LZW");
		goto BadRead;
		}
	FileBPL = npix;
	if (!AllocLines (&lpFileLine, 1, FileBPL, 4))
		{
		Message (IDS_EMEMALLOC);
		goto BadWrite;
		}

	lpRGBLine = lpFileLine + FileBPL;
	lRedPlane = Strips.StripOffsets[0];
	lGreenPlane = lRedPlane  + ((long)nlin*FileBPL);
	lBluePlane = lGreenPlane + ((long)nlin*FileBPL);

	for (i=0; i<nlin; i++)
		{
		AstralClockCursor (i, nlin);

		if (_llseek (ifh, lRedPlane, 0) == -1)
			goto BadRead;
		if (image_read(lpFileLine, FileBPL, 1, ifh) == 0)
			goto BadRead;
		SetEachThirdValue (lpFileLine, lpRGBLine, FileBPL);
		lRedPlane += FileBPL;

		if (_llseek (ifh, lGreenPlane, 0) == -1)
			goto BadRead;
		if (image_read(lpFileLine, FileBPL, 1, ifh) == 0)
			goto BadRead;
		SetEachThirdValue (lpFileLine, lpRGBLine+1, FileBPL);
		lGreenPlane += FileBPL;

		if (_llseek (ifh, lBluePlane, 0) == -1)
			goto BadRead;
		if (image_read(lpFileLine, FileBPL, 1, ifh) == 0)
			goto BadRead;
		SetEachThirdValue (lpFileLine, lpRGBLine+2, FileBPL);
		lBluePlane += FileBPL;

		if (in_depth != out_depth)
		    {
	            ConvertData ((LPTR)lpRGBLine, in_depth, npix, 
				 (LPTR)lpCacheLine, out_depth);
		    if (!frame_write (&span, lpCacheLine, npix))
			goto BadWrite;
		    }
		else
		    {
		    if (!frame_write (&span, lpRGBLine, npix))
			goto BadWrite;
		    }
		span.sy++;
		}
	}

if (compressed)
	if (DecompressLZW (ifh, NULL, 0) < 0) /* Terminate */
		Message (IDS_ECOMPRESSCLOSE);

if (lpFileLine)
	FreeUp (lpFileLine);
if (lpCacheLine)
	FreeUp (lpCacheLine);
if (ifh >= 0)
	_lclose (ifh);
if (lpOldFrame)
    frame_set (lpOldFrame);
*lpBPP = bpp;
return (lpFrame);

BadRead:
Message (IDS_EREAD, ifile);
goto BadTiff;

BadWrite:
Message (IDS_ESCRWRITE, (LPTR)Control.RamDisk);

BadTiff:
if (lpFileLine)
	FreeUp (lpFileLine);
if (lpCacheLine)
	FreeUp (lpCacheLine);
if (ifh >= 0)
	_lclose (ifh);
if (lpOldFrame)
    frame_set (lpOldFrame);
return (NULL);
}


/************************************************************************/
LOCAL VOID diff_fixtagval (tagp)
/************************************************************************/
LPTAG tagp;
{
long length;

/* this is necessary because tag values fields are left justified */
/* and need to be swapped if a different machine is reading the file */
/* - this is executed both on the way in and on the way out */
if (tagp->type == TIF_LONG)  length = 4;
else
if (tagp->type == TIF_SHORT) length = 2;
else
if (tagp->type == TIF_BYTE)  length = 1;
else
if (tagp->type == TIF_ASCII) length = 1;
length *= tagp->length;

if (length == 1) /* will this work for read and write? */
	tagp->value = * ((LPTR) (& (tagp->value)));
else
if (length == 2)
	{
	swapw ((LPWORD)&tagp->value);
	tagp->value = * ((LPWORD) (& (tagp->value)));
	}
else
//if (length > 2)
	swapl ((LPDWORD)&tagp->value);
}

/************************************************************************/
LOCAL VOID rightjust_tagval (tagp)
/************************************************************************/
LPTAG tagp;
{
long length;

/* this is necessary because tag values fields are left justified */
/* in the file, but are converted to longs when read in */
if (tagp->type == TIF_LONG)  length = 4;
else
if (tagp->type == TIF_SHORT) length = 2;
else
if (tagp->type == TIF_BYTE)  length = 1;
else
if (tagp->type == TIF_ASCII) length = 1;
length *= tagp->length;

if (length == 2)
	 (ULONG)tagp->value = * ((LPWORD) (& (tagp->value)));
else
if (length == 1)
	 (ULONG)tagp->value = * ((LPTR) (& (tagp->value)));
}

/************************************************************************/
static int flexiread (file, ptr, size)
/************************************************************************/
INT	file;
LPTR	ptr;
REG INT	size;
{
REG INT	rc;
LPTAG tagp;

if ((rc = _lread (file, ptr, size)) != size)
	return (rc);

if (MYORDER == order)
	{
	if (size == sizeof (TAG))
		rightjust_tagval ((LPTAG)ptr);
	}
else	{
	if (size == sizeof (SHORT))
		swapw ((LPWORD)ptr);
	if (size == sizeof (LONG))
		swapl ((LPDWORD)ptr);
	if (size == sizeof (TAG))
		{
		tagp = (LPTAG)ptr;
		swapw (& (tagp->tagno));
		swapw (& (tagp->type));
		swapl (& (tagp->length));
		diff_fixtagval (tagp);
		}
	}
return (rc);
}
