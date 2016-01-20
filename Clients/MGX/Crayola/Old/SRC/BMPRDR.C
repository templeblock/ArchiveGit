/*=========================================================================*\

	(c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

	®PL1¯®FD1¯®TP0¯®BT0¯

\*=========================================================================*/

/*=========================================================================*\
	bmprdr.c
\*=========================================================================*/

#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

/*=========================================================================*/

#define BMHDRSZ	sizeof(BITMAPFILEHEADER)	/* size of bitmap file header */

/*=========================================================================*/

static int bmpReadHeader (int ifh, BITMAPFILEHEADER *hdr)
{
	int err;

	_lread(ifh, &hdr->bfType, 2);
	_lread(ifh, &hdr->bfSize, 4);
	_lread(ifh, &hdr->bfReserved1, 2);
	_lread(ifh, &hdr->bfReserved2, 2);
	_lread(ifh, &hdr->bfOffBits, 4);

	if (hdr->bfType != 0x4D42)
	{	/* BM */
		Print ("Invalid Type");
		err = -1;
	}

	return (err);
}

/*=========================================================================*/

LPFRAME bmprdr(LPSTR ifile, int outdepth, LPINT lpDataType)
{
	LPTR    lpFileLine, lpCacheLine, lpRGBLine;
	LPFRAME lpFrame, lpOldFrame;
	LPTR    lp, lpOut;
	LPRGB   lpRGB;
	int     ifh;
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER infohdr;
	RGBS    RGBmap [256];
	RGBQUAD rgbq;
	RGBTRIPLE rgbt;
	BOOL graymap, bOS2, graylinear;
	int  i, k, p;
	int  sy;
	int  FileBPL;	/* bytes per line (bmp file) */
	int  xres;		/* pixels per inch */
	int  bpp;		/* bits per pixel */
	int  npix;		/* image width (pixels) */
	int  nlin;		/* image height (pixels) */
	int  indepth;	/* bytes per pixel (file 1 or 3) */

	lpFileLine = NULL;
	lpCacheLine = NULL;
	lpFrame = NULL;
	lpOldFrame = NULL;

	ProgressBegin(1,0);
	
	if ((ifh = _lopen(ifile,OF_READ)) < 0)
	{
		Message( IDS_EOPEN, ifile );
		goto BadBMP;
	}

	/***********************************/
	/* determine image characteristics */
	/***********************************/

	/* read bmp file header */
	if (bmpReadHeader (ifh, &hdr) == -1)
		goto BadRead;

	/* read size needed for following structure (determines which structure) */
	_lread(ifh, &infohdr.biSize, 4);

	/* check for bitmapcoreheader */
	bOS2 = (infohdr.biSize == sizeof (BITMAPCOREHEADER));

	if ( bOS2 )
	{
		/* bitmapinfocoreheader */
		_lread(ifh, &i, 2);
		infohdr.biWidth = i;

		_lread(ifh, &i, 2);
		infohdr.biHeight = i;

		_lread(ifh, &infohdr.biPlanes,   2);
		_lread(ifh, &infohdr.biBitCount, 2);
		infohdr.biCompression = NO;
		infohdr.biSizeImage = 0;
		infohdr.biXPelsPerMeter = 0;
		infohdr.biYPelsPerMeter = 0;
		infohdr.biClrUsed = 0;
		infohdr.biClrImportant = 0;
	}
   else
	{
		/* bitmapinfoheader */
		_lread(ifh, &infohdr.biWidth, 4);
		_lread(ifh, &infohdr.biHeight, 4);
		_lread(ifh, &infohdr.biPlanes, 2);
		_lread(ifh, &infohdr.biBitCount, 2);
		_lread(ifh, &infohdr.biCompression, 4);
		_lread(ifh, &infohdr.biSizeImage, 4);
		_lread(ifh, &infohdr.biXPelsPerMeter, 4);
		_lread(ifh, &infohdr.biYPelsPerMeter, 4);
		_lread(ifh, &infohdr.biClrUsed, 4);
		_lread(ifh, &infohdr.biClrImportant, 4);

		/* validate that image is not compressed */
		if (infohdr.biCompression != BI_RGB)
		{
			Print ("Compressed bitmaps not supported.");
			goto BadBMP;
		}
	}

	/* get width of image in pixels */
	npix = infohdr.biWidth;
	nlin = infohdr.biHeight;

	if (infohdr.biXPelsPerMeter)
		xres = (( 254L * infohdr.biXPelsPerMeter + 5000)) / 10000L;
	else
		xres = 75;

	if ( xres <= 1 )
		xres = 75;

	/* set indepth, outdepth and colormap size */
	switch (infohdr.biBitCount)
	{
		case 1:
			if ((infohdr.biClrUsed == 0) || (infohdr.biClrUsed > 2))
				infohdr.biClrUsed = 2;
			bpp = 1;
			indepth = 0;
			FileBPL = (infohdr.biWidth + 7) / 8;
			*lpDataType = IDC_SAVELA;
		break;

		case 4:
			if ((infohdr.biClrUsed == 0) || (infohdr.biClrUsed > 16))
				infohdr.biClrUsed = 16;
			bpp = 4;
			indepth = 3;
			FileBPL = (infohdr.biWidth + 1) / 2;
			*lpDataType = IDC_SAVECT;
		break;

		case 8:
			if ((infohdr.biClrUsed == 0) || (infohdr.biClrUsed > 256))
				infohdr.biClrUsed = 256;
			bpp = 8;
			indepth = 3;
			FileBPL = infohdr.biWidth;
			*lpDataType = IDC_SAVECT;
		break;

		case 24:
			infohdr.biClrUsed = 0;
			bpp = 24;
			indepth = 3;
			FileBPL = 3*infohdr.biWidth;
			*lpDataType = IDC_SAVE24BITCOLOR;
		break;

		default:
			goto BadRead;
		break;
	}

	/* adjust bytes per line to DWORD (long) boundry */
	FileBPL = (FileBPL + 3) & ~3;

	/* read color map, if any */
	if (infohdr.biClrUsed)
	{
		/* seek to loc of colormap */
		if ( _llseek (ifh, infohdr.biSize + BMHDRSZ, 0)
			< infohdr.biSize + BMHDRSZ)
		{
			ProgressEnd();
			return (NULL);
		}
		graymap    = TRUE;
		graylinear = TRUE;

		for (i = 0; i < infohdr.biClrUsed; i++)
		{
			if ( bOS2 )
			{
				_lread(ifh, (LPTR) &rgbt, 3);
				RGBmap[i].red   = rgbt.rgbtRed;
				RGBmap[i].green = rgbt.rgbtGreen;
				RGBmap[i].blue  = rgbt.rgbtBlue;
			}
			else
			{
				_lread(ifh, (LPTR) &rgbq, 4);
				RGBmap[i].red   = rgbq.rgbRed;
				RGBmap[i].green = rgbq.rgbGreen;
				RGBmap[i].blue  = rgbq.rgbBlue;
			}
			if (graymap)
			{
				graymap = (RGBmap[i].red == RGBmap[i].green) &&
				  (RGBmap[i].red == RGBmap[i].blue);

				if (graylinear)
				{
					if (RGBmap[i].red != i)
						graylinear = FALSE;
				}

				if (!graymap)
					graylinear = FALSE;
			}
		}

		if (*lpDataType == IDC_SAVECT && !graymap)
		{
			if (bpp == 4)
				*lpDataType = IDC_SAVE4BITCOLOR;
			else
				*lpDataType = IDC_SAVE8BITCOLOR;
		}

		/* if color map entries are 'gray', do a depth of one */
		if ((indepth > 1) && graymap)
			indepth = 1;
	}

	if (outdepth < 0)
	{
		outdepth = indepth;
  		if (outdepth == 0 && Control.LineArtAsGray)
			outdepth = 1;
	}

	/* Seek to location of bitmap data */
	if ( _llseek(ifh, hdr.bfOffBits, 0) < hdr.bfOffBits )
	{
		ProgressEnd();
		return (NULL);
	}

	/* allocate space for one line of the image */
	if ( !AllocLines (&lpCacheLine, 1, npix, outdepth ? outdepth : 1))
	{
		Message (IDS_EMEMALLOC );
		goto BadWrite;
	}

	/* Create the image frame store */
	lpFrame = FrameOpen((FRMDATATYPE)outdepth, npix, nlin, xres);

	if ( !lpFrame )
	{
		FrameError(IDS_EIMAGEOPEN);
		goto BadBMP;
	}

	if (!outdepth)
		outdepth = 1;

	/* convert the image */
	switch (bpp)
	{
		case 1:
			/* allocate space for one line of the image (bmp file) */
			if ( !AllocLines (&lpFileLine, 1, FileBPL, 1))
			{
				Message (IDS_EMEMALLOC);
				goto BadWrite;
			}

			if (indepth == outdepth)
			{
				for (i = 0; i < nlin; i++)
				{
					sy = nlin - i - 1;
					if (AstralClockCursor (i, nlin, YES))
						goto Cancelled;

					if ( !(lpOut = FramePointer(lpFrame, 0, sy, YES)) )
						goto BadWrite;

					if ( _lread(ifh, lpOut, FileBPL) != FileBPL)
						goto BadRead;

					if (RGBmap[0].red != 0)
						negate (lpOut, (long)FileBPL);
				}
			}
			else
			{
				for (i = 0; i < nlin; i++)
				{
					sy = nlin - i - 1;
					if (AstralClockCursor (i, nlin, YES))
						goto Cancelled;

					if ( _lread(ifh, lpFileLine, FileBPL) != FileBPL)
						goto BadRead;

					if (RGBmap[0].red != 0)
						negate (lpFileLine, (long)FileBPL);

					if ( !(lpOut = FramePointer(lpFrame, 0, sy, YES)) )
						goto BadWrite;

					ConvertData (lpFileLine, 0, npix, lpOut, outdepth);
				}
			}
		break;

		case 4:

			/* allocate space for one line of the image (bmp file) */
			if ( !AllocLines (&lpFileLine, 1, 2*FileBPL, 4))
			{
				Message (IDS_EMEMALLOC);
				goto BadWrite;
			}

			lpRGBLine = lpFileLine + 2*FileBPL;

			for (i = 0; i < nlin; i++)
			{
				sy = nlin - i - 1;
				if (AstralClockCursor( i, nlin, YES ))
					goto Cancelled;

				if ( _lread(ifh, lpFileLine, FileBPL) != FileBPL)
					goto BadRead;

				lp = lpFileLine;
				lpRGB = (LPRGB)lpRGBLine;

				for (k = 0; k < FileBPL; k++, lp++)
				{
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

				if (outdepth != 3)
				{
					ConvertData ((LPTR)lpRGBLine, 3, npix, lpCacheLine, outdepth);
					if ( !FrameWrite( lpFrame, 0, sy, npix, lpCacheLine, npix ) )
						goto BadWrite;
				}
				else
				{
					if ( !FrameWrite( lpFrame, 0, sy, npix, (LPTR)lpRGBLine, npix ) )
						goto BadWrite;
				}
			}
		break;

		case 8:
			/* allocate space for one line of the image (bmp file) */
			if ( !AllocLines (&lpFileLine, 1, FileBPL, 4))
			{
				Message (IDS_EMEMALLOC);
				goto BadWrite;
			}

			lpRGBLine = lpFileLine + FileBPL;

			// Special Case linear gray...
			if (graylinear && (outdepth == 1))
			{
				for (i = 0; i < nlin; i++)
				{
					sy = nlin - i - 1;
					if (AstralClockCursor( i, nlin, YES ))
						goto Cancelled;

					lpOut = FramePointer( lpFrame, 0, sy, YES );
					if (!lpOut)
						goto BadWrite;

					if ( _lread(ifh, lpOut, FileBPL) != FileBPL)
						goto BadRead;
				}
			}
			else
			{
				for (i = 0; i < nlin; i++)
				{
					sy = nlin - i - 1;
					if (AstralClockCursor( i, nlin, YES ))
						goto Cancelled;

					if ( _lread(ifh, lpFileLine, FileBPL) != FileBPL)
						goto BadRead;

					lp = lpFileLine;
					lpRGB = (LPRGB)lpRGBLine;
					for ( k = 0; k < FileBPL; k++, lp++, lpRGB++ )
					{
						p = *lp & 0x00FF;
						lpRGB->red   = RGBmap[ p ].red;
						lpRGB->green = RGBmap[ p ].green;
						lpRGB->blue  = RGBmap[ p ].blue;
					}

					if (outdepth != 3)
					{
						ConvertData ((LPTR)lpRGBLine, 3, npix, lpCacheLine, outdepth);
						if ( !FrameWrite( lpFrame, 0, sy, npix, lpCacheLine, npix ) )
							goto BadWrite;
					}
					else
					{
						if ( !FrameWrite( lpFrame, 0, sy, npix, (LPTR)lpRGBLine, npix ) )
							goto BadWrite;
					}
				}
			}
		break;

		case 24:
			/* allocate space for one line of the image (bmp file) */
			if ( !AllocLines (&lpFileLine, 1, FileBPL, 1))
			{
				Message (IDS_EMEMALLOC);
				goto BadWrite;
			}

			if (indepth == outdepth)
			{
				for (i = 0; i < nlin; i++)
				{
					sy = nlin - i - 1;
					if (AstralClockCursor( i, nlin, YES ))
						goto Cancelled;

					if ( _lread(ifh, lpFileLine, FileBPL) != FileBPL)
						goto BadRead;

					lpOut = FramePointer( lpFrame, 0, sy, YES );

					if (!lpOut)
						goto BadWrite;

					swapBGR(lpFileLine, lpOut, npix);
				}
			}
			else
			{
				for (i = 0; i < nlin; i++)
				{
					sy = nlin - i - 1;
					if (AstralClockCursor( i, nlin, YES ))
						goto Cancelled;

					if ( _lread(ifh, lpFileLine, FileBPL) != FileBPL)
						goto BadRead;

					lpOut = FramePointer( lpFrame, 0, sy, YES );

					if (!lpOut)
						goto BadWrite;

					swapBGR(lpFileLine, lpFileLine, npix);
					ConvertData (lpFileLine, indepth, npix, lpOut, outdepth);
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

	ProgressEnd();

	return (lpFrame);

BadRead:
	Message( IDS_EREAD, ifile );
	goto BadBMP;

BadWrite:
	FrameError(IDS_EFRAMEREAD);	

Cancelled:
BadBMP:

	if ( lpFileLine )
		FreeUp( lpFileLine );

	if ( lpCacheLine )
		FreeUp( lpCacheLine );

	if ( ifh >= 0 )
		_lclose(ifh);

	if (lpFrame)
		FrameClose(lpFrame);

	ProgressEnd();

	return( NULL );
}

/*=========================================================================*/
