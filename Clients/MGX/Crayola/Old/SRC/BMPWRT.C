/*=========================================================================*\

	(c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

	®PL1¯®FD1¯®TP0¯®BT0¯

\*=========================================================================*/

/*=========================================================================*\
	bmpwrt.c
\*=========================================================================*/

#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

/*=========================================================================*/

#define BMHDRSZ sizeof(BITMAPFILEHEADER) /* size of bitmap file header */

#define RED_HI 0x40
#define GRN_HI 0x20
#define BLU_HI 0x10

#define RED_LO 0x04
#define GRN_LO 0x02
#define BLU_LO 0x01

#define PATTERN_MAX 64

/*=========================================================================*/

unsigned int DitherPat[8][8] = {
	 1, 33,  9, 41,  3, 35, 11, 43,
	49, 17, 57, 25, 51, 19, 59, 27,
	13, 45,  5, 37, 15, 47,  7, 39,
	61, 29, 53, 21, 63, 31, 55, 23,
	 4, 36, 12, 44,  2, 34, 10, 42,
	52, 20, 60, 28, 50, 18, 58, 26,
	16, 48,  8, 40, 14, 46,  6, 38,
	64, 32, 56, 24, 62, 30, 54, 22
};

/*=========================================================================*/

void Dither16(
	LPFRAME lpFrame,
	LPTR lpInBuf, 
	LPTR lpOutBuf, 
	int iPixelCount,
	int iRow)
{
	LPTR sp, dp;
	unsigned int Even, Odd;
	unsigned int OutValue;
	unsigned int Dither1;
	unsigned int Dither2;
	unsigned int Dither3;
	unsigned int Dither4;
	unsigned int Dither5;
	unsigned int Dither6;
	unsigned int Dither7;
	unsigned int Dither8;
	int iCount;
	int iDitCell;

	iRow &= 0x07;

	Dither1 = (DitherPat[iRow][0]*255)/(PATTERN_MAX+1);
	Dither2 = (DitherPat[iRow][1]*255)/(PATTERN_MAX+1);
	Dither3 = (DitherPat[iRow][2]*255)/(PATTERN_MAX+1);
	Dither4 = (DitherPat[iRow][3]*255)/(PATTERN_MAX+1);
	Dither5 = (DitherPat[iRow][4]*255)/(PATTERN_MAX+1);
	Dither6 = (DitherPat[iRow][5]*255)/(PATTERN_MAX+1);
	Dither7 = (DitherPat[iRow][6]*255)/(PATTERN_MAX+1);
	Dither8 = (DitherPat[iRow][7]*255)/(PATTERN_MAX+1);

	sp = lpInBuf;
	dp = lpOutBuf;

	iCount = iPixelCount/2;
	iDitCell = 0;

	// If we have a color image
	if (FrameType(lpFrame) == FDT_RGBCOLOR)
	{
		while(iCount--)
		{
			OutValue = 0;

			switch(iDitCell)
			{
				case 0 : Even = Dither1; Odd = Dither2; iDitCell++; break;
				case 1 : Even = Dither3; Odd = Dither4; iDitCell++; break;
				case 2 : Even = Dither5; Odd = Dither6; iDitCell++; break;
				case 3 : Even = Dither7; Odd = Dither8; iDitCell=0; break;
			}

			if (*sp++ >= Even) OutValue |= RED_HI;
			if (*sp++ >= Even) OutValue |= GRN_HI;
			if (*sp++ >= Even) OutValue |= BLU_HI;

			if (*sp++ >= Odd ) OutValue |= RED_LO;
			if (*sp++ >= Odd ) OutValue |= GRN_LO;
			if (*sp++ >= Odd ) OutValue |= BLU_LO;

			*dp++ = OutValue;
		}

 		// If we had an odd number of input pixels
		if (iPixelCount&1)
		{
			OutValue = 0;

			switch(iDitCell)
			{
				case 0 : Even = Dither1; Odd = Dither2; iDitCell++; break;
				case 1 : Even = Dither3; Odd = Dither4; iDitCell++; break;
				case 2 : Even = Dither5; Odd = Dither6; iDitCell++; break;
				case 3 : Even = Dither7; Odd = Dither8; iDitCell=0; break;
			}

			if (*sp++ >= Even) OutValue |= RED_HI;
			if (*sp++ >= Even) OutValue |= GRN_HI;
			if (*sp++ >= Even) OutValue |= BLU_HI;

			*dp++ = OutValue;
		}
	}
	else
	{
		// If we have a monochrome image
		while(iCount--)
		{
			OutValue = 0;

			switch(iDitCell)
			{
				case 0 : Even = Dither1; Odd = Dither2; iDitCell++; break;
				case 1 : Even = Dither3; Odd = Dither4; iDitCell++; break;
				case 2 : Even = Dither5; Odd = Dither6; iDitCell++; break;
				case 3 : Even = Dither7; Odd = Dither8; iDitCell=0; break;
			}

			if (*sp++ >= Even) OutValue |= RED_HI | GRN_HI | BLU_HI;
			if (*sp++ >= Odd ) OutValue |= RED_LO | GRN_LO | BLU_LO;

			*dp++ = OutValue;
		}

 		// If we had an odd number of input pixels
		if (iPixelCount&1)
		{
			OutValue = 0;

			switch(iDitCell)
			{
				case 0 : Even = Dither1; Odd = Dither2; iDitCell++; break;
				case 1 : Even = Dither3; Odd = Dither4; iDitCell++; break;
				case 2 : Even = Dither5; Odd = Dither6; iDitCell++; break;
				case 3 : Even = Dither7; Odd = Dither8; iDitCell=0; break;
			}

			if (*sp++ >= Even) OutValue |= RED_HI | GRN_HI | BLU_HI;

			*dp++ = OutValue;
		}
	}
}

/*=========================================================================*/

int bmpwrt (LPSTR ifile, LPOBJECT lpObject, LPFRAME lpFrame, LPRECT lpRect, int flag, BOOL fCompressed)
{
	BITMAPFILEHEADER bmhdr;
	BITMAPINFOHEADER infohdr;
	int colormapentries;
	RGBS RGBmap[256];
	RGBQUAD rgbq;
	int     ofh;
	int     y, i, bpl, npix, nlin;
	LPTR    lp, lpBuffer, lpImgScanline;
	FNAME   temp;
	RECT    rSave;
	BOOL    bEscapable;
	int     depth;

	lpBuffer      = NULL;
	lpImgScanline = NULL;

	if (!lpFrame)
 		return(-1);
	
	ProgressBegin(1,0);

	bEscapable = !FileExists(ifile);

	/* open the output file */
	if ((ofh = _lcreat (ifile,0)) < 0)
	{
		Message (IDS_EWRITE, ifile);
		goto ExitBad;
	}

	if (lpRect)
	{
		rSave = *lpRect;
	}
	else
	{
		rSave.top = rSave.left = 0;
		rSave.bottom = FrameYSize(lpFrame)-1;
		rSave.right = FrameXSize(lpFrame)-1;
	}
	depth = FrameDepth(lpFrame);
	if (depth == 0) depth = 1;
	npix = RectWidth(&rSave);
	nlin = RectHeight(&rSave);

	/* initialize bit map file header */
	bmhdr.bfType      = 0x4D42;
	bmhdr.bfSize      = 0;
	bmhdr.bfReserved1 = 0;
	bmhdr.bfReserved2 = 0;
	bmhdr.bfOffBits   = 0;

	/* initialize bit map info header */
	/* should be the same as sizeof(BITMAPINFOHEADER) */
	infohdr.biSize          = 40;
	infohdr.biWidth         = npix;
	infohdr.biHeight        = nlin;
	infohdr.biPlanes        = 1;
	infohdr.biBitCount      = 0;
	infohdr.biCompression   = BI_RGB;
	infohdr.biSizeImage     = 0;
	infohdr.biXPelsPerMeter = (10000L * (long) FrameResolution(lpFrame)) / 254L;
	infohdr.biYPelsPerMeter = (10000L * (long) FrameResolution(lpFrame)) / 254L;
	infohdr.biClrUsed       = 0;
	infohdr.biClrImportant  = 0;

	/* determine type of image to create */
	if ( flag == IDC_SAVECT )
	{
		/* gray */
		infohdr.biBitCount = 8;
		colormapentries = 256;
		bpl = npix;
	}
	else if ( flag == IDC_SAVE24BITCOLOR  || flag == IDC_SAVE32BITCOLOR )
	{
		/* full color */
		infohdr.biBitCount = 24;
		colormapentries = 0;
		bpl = npix * 3;
	}
	else if ( flag == IDC_SAVE8BITCOLOR )
	{
		/* mini color */
		infohdr.biBitCount = 8;
		colormapentries = 256;
		bpl = npix;
	}
	else if ( flag == IDC_SAVELA || flag == IDC_SAVESP )
	{
		/* line art or scatter */
		infohdr.biBitCount = 1;
		colormapentries = 2;
		bpl = ((npix + 7) / 8);
	}
	else if ( flag == IDC_SAVE4BITCOLOR )
	{
		/* 16-color system palette */
		infohdr.biBitCount = 4;
		colormapentries = 16;
		bpl = (npix+1)/2;
	}
	else
	{
		Print ("Unknown flag value");
		goto ExitBad;
	}
	bpl = (bpl + 3) & ~3;

	AllocLines (&lpBuffer,      1, max (bpl, npix), 3);
	AllocLines (&lpImgScanline, 1, max (bpl, npix), 3);

	if ( !lpBuffer || !lpImgScanline )
	{
		Message (IDS_EMEMALLOC);
		_lclose (ofh);
		goto ExitBad;
	}

	/* determine size of image, in bytes */
	infohdr.biSizeImage = bpl * infohdr.biHeight;

	/* determine where image data starts */
	bmhdr.bfOffBits = 14 + infohdr.biSize + 4 * colormapentries;

	/* determine size of file */
	bmhdr.bfSize = (bmhdr.bfOffBits + infohdr.biSizeImage) / 2;

	/* write bit map file header */
	_lwrite(ofh, (LPSTR)&bmhdr.bfType,      2);
	_lwrite(ofh, (LPSTR)&bmhdr.bfSize,      4);
	_lwrite(ofh, (LPSTR)&bmhdr.bfReserved1, 2);
	_lwrite(ofh, (LPSTR)&bmhdr.bfReserved2, 2);
	_lwrite(ofh, (LPSTR)&bmhdr.bfOffBits,   4);

	/* write bit map info header */
	_lwrite(ofh, (LPSTR)&infohdr.biSize,          4);
	_lwrite(ofh, (LPSTR)&infohdr.biWidth,         4);
	_lwrite(ofh, (LPSTR)&infohdr.biHeight,        4);
	_lwrite(ofh, (LPSTR)&infohdr.biPlanes,        2);
	_lwrite(ofh, (LPSTR)&infohdr.biBitCount,      2);
	_lwrite(ofh, (LPSTR)&infohdr.biCompression,   4);
	_lwrite(ofh, (LPSTR)&infohdr.biSizeImage,     4);
	_lwrite(ofh, (LPSTR)&infohdr.biXPelsPerMeter, 4);
	_lwrite(ofh, (LPSTR)&infohdr.biYPelsPerMeter, 4);
	_lwrite(ofh, (LPSTR)&infohdr.biClrUsed,       4);
	_lwrite(ofh, (LPSTR)&infohdr.biClrImportant,  4);

	switch (infohdr.biBitCount)
	{
		case 1:
			rgbq.rgbReserved = 0;

			/* write the color map */
			rgbq.rgbRed   =
			rgbq.rgbGreen =
			rgbq.rgbBlue  = 0;
			_lwrite(ofh, (LPSTR) &rgbq, 4);

			rgbq.rgbRed   =
			rgbq.rgbGreen =
			rgbq.rgbBlue  = 255;
			_lwrite(ofh, (LPSTR) &rgbq, 4);
	
			/* write the image */
			for (y = rSave.bottom; y >= rSave.top; y--)
			{
				if (AstralClockCursor (rSave.bottom-y, nlin, bEscapable ))
					goto Cancelled;

				if ( lpObject )
				{
					if (!ImgGetLine( NULL, lpObject, rSave.left, y,
						(rSave.right - rSave.left) + 1, lpImgScanline))
						goto BadRead;
					lp = lpImgScanline;
				}
				else
				{
					if ( !(lp = FramePointer( lpFrame, rSave.left, y, NO )) )
						goto BadRead;
				}

				ConvertData (lp, depth, npix, lpBuffer+bpl, 1);

				if ( flag == IDC_SAVESP)
					diffuse (0, y, 0, NULL, lpBuffer+bpl, npix, lpBuffer);
				else
				{
					con2la (lpBuffer+bpl, npix, lpBuffer);
				}
				// negate( lpBuffer, bpl);

				if ( _lwrite(ofh, (LPSTR)lpBuffer, bpl) != bpl )
					goto BadWrite;
			}
		break;

		case 4:
			rgbq.rgbReserved = 0;

			/* write the color map */
			for (i = 0; i < 16; i++)
			{
				rgbq.rgbRed   = (i&RED_LO) ? 0xFF : 0x00;
				rgbq.rgbGreen = (i&GRN_LO) ? 0xFF : 0x00;
				rgbq.rgbBlue  = (i&BLU_LO) ? 0xFF : 0x00;
				_lwrite(ofh, (LPSTR) &rgbq, 4);
			}

			/* write the image */
			for ( y = rSave.bottom; y >= rSave.top; y--)
			{
				if (AstralClockCursor (rSave.bottom - y, nlin, bEscapable))
					goto Cancelled;

				if ( lpObject )
				{
					if (!ImgGetLine( NULL, lpObject, rSave.left, y,
						(rSave.right - rSave.left) + 1, lpImgScanline))
						goto BadRead;
					lp = lpImgScanline;
				}
				else
				{
					if ( !(lp = FramePointer( lpFrame, rSave.left, y, NO )) )
						goto BadRead;
				}

				Dither16( lpFrame, lp, lpBuffer, npix, y);

				if (_lwrite(ofh, (LPSTR)lpBuffer, bpl) != bpl)
					goto BadWrite;
			}
		break;

		case 8:
			if ( flag == IDC_SAVECT )
			{
				rgbq.rgbReserved = 0;

				/* write the color map */
				for (i = 0; i < 256; i++)
				{
					rgbq.rgbRed   = i;
					rgbq.rgbGreen = i;
					rgbq.rgbBlue  = i;
					_lwrite(ofh, (LPSTR) &rgbq, 4);
				}

				/* write the image */
				for(y = rSave.bottom; y >= rSave.top; y--)
				{
					if (AstralClockCursor (rSave.bottom-y, nlin, bEscapable))
						goto Cancelled;

					if ( lpObject )
					{
						if (!ImgGetLine( NULL, lpObject, rSave.left, y,
							(rSave.right - rSave.left) + 1, lpImgScanline))
							goto BadRead;
						lp = lpImgScanline;
					}
					else
					{
						if ( !(lp = FramePointer( lpFrame, rSave.left, y, NO )) )
							goto BadRead;
					}
	
					ConvertData (lp, depth, npix, lpBuffer, 1);

					if (_lwrite(ofh, (LPSTR)lpBuffer, bpl) != bpl)
						goto BadWrite;
				}
			}
			else
			{
				if (!OptimizeBegin(lpObject, lpFrame, RGBmap, 256, 
					NULL /*(LPROC)AstralClockCursor*/, // No Progress Report
					NO, Convert.fOptimize, Convert.fScatter, Convert.fDither, npix))
				{
					goto BadWrite;
				}

				rgbq.rgbReserved = 0;

				/* write the color map */
				for (i = 0; i < 256; i++)
				{
					rgbq.rgbRed   = RGBmap[i].red;
					rgbq.rgbGreen = RGBmap[i].green;
					rgbq.rgbBlue  = RGBmap[i].blue;
					_lwrite(ofh, (LPSTR) &rgbq, 4);
				}

				/* write the image */
				for ( y = rSave.bottom; y >= rSave.top; y--)
				{
					if (AstralClockCursor (rSave.bottom-y, nlin, bEscapable))
						goto Cancelled;

					if ( lpObject )
					{
						if (!ImgGetLine( NULL, lpObject, rSave.left, y,
							(rSave.right - rSave.left) + 1, lpImgScanline))
							goto BadRead;
						lp = lpImgScanline;
					}
					else
					{
						if ( !(lp = FramePointer( lpFrame, rSave.left, y, NO )) )
							goto BadRead;
					}

					OptimizeData(0, y, npix, lp, lpBuffer, depth);

					if (_lwrite(ofh, (LPSTR)lpBuffer, bpl) != bpl)
						goto BadWrite;
				}
			}
		break;

		case 24:
			/* no color map */

			if (depth == 3)
			{
				/* write the image */
				for(y = rSave.bottom; y >= rSave.top; y--)
				{
					if (AstralClockCursor (rSave.bottom-y, nlin, bEscapable))
						goto Cancelled;

					if ( lpObject )
					{
						if (!ImgGetLine( NULL, lpObject, rSave.left, y,
							(rSave.right - rSave.left) + 1, lpImgScanline))
							goto BadRead;
						lp = lpImgScanline;
					}
					else
					{
						if ( !(lp = FramePointer( lpFrame, rSave.left, y, NO )) )
							goto BadRead;
					}

					/* swap from rgb to bgr */
					swapBGR(lp, lpBuffer, npix);

					if (_lwrite(ofh, (LPSTR)lpBuffer, bpl) != bpl)
						goto BadWrite;
				}
			}
			else
			{
				/* write the image */
				for(y = rSave.bottom; y >= rSave.top; y--)
				{
					if (AstralClockCursor (rSave.bottom-y, nlin, bEscapable))
						goto Cancelled;

					if ( lpObject )
					{
						if (!ImgGetLine( NULL, lpObject, rSave.left, y,
							(rSave.right - rSave.left) + 1, lpImgScanline))
							goto BadRead;
						lp = lpImgScanline;
					}
					else
					{
						if ( !(lp = FramePointer( lpFrame, rSave.left, y, NO )) )
							goto BadRead;
					}

					ConvertData (lp, depth, npix, lpBuffer, 3);

					/* swap from rgb to bgr */
					swapBGR(lpBuffer, lpBuffer, npix);

					if (_lwrite(ofh, (LPSTR)lpBuffer, bpl) != bpl)
						goto BadWrite;
				}
			}
		break;
	}

	/* clean up */
	OptimizeEnd();

	_lclose (ofh);

	if (lpBuffer)
		FreeUp (lpBuffer);

	if (lpImgScanline)
		FreeUp (lpImgScanline);

	ProgressEnd();

	return (0);

BadWrite:
	Message (IDS_EWRITE, ifile);
	goto BadBMP;

BadRead:
	Message (IDS_EREAD, (LPTR)Control.RamDisk);

Cancelled:
BadBMP:
	OptimizeEnd();

	_lclose (ofh);

	if (lpBuffer)
		FreeUp (lpBuffer);

	if (lpImgScanline)
		FreeUp (lpImgScanline);

	lstrcpy (temp, ifile);

	FileDelete (temp);

ExitBad:

	ProgressEnd();

	return (-1);
}

/*=========================================================================*/
