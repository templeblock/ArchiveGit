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

#include "stdafx.h"
#include "bitmapio.h"
#include "cpo.h"
#include "getline.h"

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

	// we have a color image
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

void Convert8BitTo4Bit(
	LPTR lpInBuf, 
	LPTR lpOutBuf, 
	int iCount)
{
	register BYTE p, op;
	BOOL IsOdd = NO;

	while (--iCount >= 0)
   	{
		p = *lpInBuf++;
    	if ( IsOdd )
		{
			op += p;
			*lpOutBuf++ = op;
			IsOdd = NO;
	   	}
    	else
	   	{
			op = p << 4;
			IsOdd = YES;
	   	}
    }

	if ( IsOdd )
    	*lpOutBuf = op;
}

/*=========================================================================*/

BOOL CWriteBitmap::BMPWrite()
{
	BITMAPFILEHEADER bmhdr;
	BITMAPINFOHEADER infohdr;
	int colormapentries;
	LPRGB lpRGBmap;
	RGBQUAD rgbq;
	int     y, i, bpl, npix, nlin;
    int     indepth, outdepth;
	LPTR    lp, lpBuffer, lpImgScanline;
	RECT    rSave;
	BOOL    bEscapable;
	LPFRAME lpFrame;
	LPOBJECT lpObject;
	FRMTYPEINFO inType, outType;
	CFrameTypeConvert TypeConvert;
	BOOL fRet = FALSE;
	BOOL fIn16Color;
	CFile 	theFile;
	CFile*	pTheFile = NULL;

	if (m_lpObjList)
	{
		lpObject = m_lpObjList->lpHead;
		if (lpObject)
			lpFrame = ObjGetEditFrame(lpObject);
	}
	else
	{
		lpFrame = m_lpFrame;
		lpObject = NULL;
	}

	ASSERT(lpFrame);
	
	ProgressBegin(1);
	if ((pTheFile = OpenFile()) == NULL)
	{
		ProgressEnd();
		return(FALSE);
	}

	TRY
	{
		lpBuffer      	= NULL;
		lpImgScanline 	= NULL;

		if (m_fSrcArea)
			rSave = m_rSrcArea;
		else
		{
			rSave.top    = rSave.left = 0;
			rSave.bottom = FrameYSize(lpFrame)-1;
			rSave.right  = FrameXSize(lpFrame)-1;
		}

		inType = FrameTypeInfo(lpFrame);
		// cause FramePointer never returns line art
		if (inType.DataType == FDT_LINEART)
			inType.DataType = FDT_GRAYSCALE;
		// make sure colormap is inited for outType so we don't free bogus ptr
		FrameSetTypeInfo(&outType, FDT_NONE);
		fIn16Color =(inType.DataType == FDT_PALETTECOLOR) &&
				 	(inType.ColorMap->NumEntries <= 16);

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
		if ( m_DataType == FDT_GRAYSCALE )
		{
			/* gray */
			infohdr.biBitCount = 8;
			colormapentries = 256;
			bpl = npix;
			FrameSetTypeInfo(&outType, FDT_GRAYSCALE);
		}
		else if ( m_DataType == FDT_RGBCOLOR  || m_DataType == FDT_CMYKCOLOR )
		{
			/* full color */
			infohdr.biBitCount = 24;
			colormapentries = 0;
			bpl = npix * 3;
			FrameSetTypeInfo(&outType, FDT_RGBCOLOR);
		}
		else if ( m_DataType == FDT_PALETTECOLOR )
		{
			/* 16-color system palette */
			if (m_BitsPerPixel == 4 || fIn16Color)
			{
				infohdr.biBitCount = 4;
				colormapentries = 16;
				bpl = (npix+1)/2;
				if (fIn16Color)
					outType = inType;
				else
					FrameSetTypeInfo(&outType, FDT_RGBCOLOR);
			}
			else
			{
				/* mini color */
				infohdr.biBitCount = 8;
				colormapentries = 256;
				bpl = npix;
				if (inType.DataType == FDT_PALETTECOLOR)
					outType = inType;
				else
					FrameSetTypeInfo(&outType, FDT_PALETTECOLOR);
			}
		}
		else if ( m_DataType == FDT_LINEART )
		{
			/* line art or scatter */
			infohdr.biBitCount = 1;
			colormapentries = 2;
			bpl = ((npix + 7) / 8);
			FrameSetTypeInfo(&outType, FDT_LINEART);
		}
		else
		{
			//Print ("Unknown flag value");
			goto Exit;
		}
		bpl = (bpl + 3) & ~3;

    	indepth = FrameDepth( lpFrame );
    	if( indepth == 0 ) 
            indepth = 1;

        switch( outType.DataType )
		{
		    case FDT_LINEART:
		    case FDT_GRAYSCALE:
		    case FDT_PALETTECOLOR:
			    outdepth = 1;
			    break;
		    case FDT_RGBCOLOR:
			    outdepth = 3;
			    break;
		    case FDT_CMYKCOLOR:
			    outdepth = 4;
			    break;
	    }

		AllocLines( &lpBuffer,      1, npix, max( indepth, outdepth ));
		AllocLines( &lpImgScanline, 1, npix, max( indepth, outdepth ));

		if ( !lpBuffer || !lpImgScanline )
		{
			SetError(BEC_errMemory);
			//Message (IDS_EMEMALLOC);
			goto Exit;
		}

		/* determine size of image, in bytes */
		infohdr.biSizeImage = bpl * infohdr.biHeight;

		/* determine where image data starts */
		bmhdr.bfOffBits = sizeof(BITMAPFILEHEADER) + 
			infohdr.biSize + 4 * colormapentries;

		/* determine size of file */
//		bmhdr.bfSize = (bmhdr.bfOffBits + infohdr.biSizeImage) / 2;
		bmhdr.bfSize = infohdr.biSizeImage + sizeof(BITMAPFILEHEADER);

		/* write bit map file header */
		pTheFile->Write((LPTR)&bmhdr.bfType,      2);
		pTheFile->Write((LPTR)&bmhdr.bfSize,      4);
		pTheFile->Write((LPTR)&bmhdr.bfReserved1, 2);
		pTheFile->Write((LPTR)&bmhdr.bfReserved2, 2);
		pTheFile->Write((LPTR)&bmhdr.bfOffBits,   4);

		/* write bit map info header */
		pTheFile->Write((LPTR)&infohdr.biSize,          4);
		pTheFile->Write((LPTR)&infohdr.biWidth,         4);
		pTheFile->Write((LPTR)&infohdr.biHeight,        4);
		pTheFile->Write((LPTR)&infohdr.biPlanes,        2);
		pTheFile->Write((LPTR)&infohdr.biBitCount,      2);
		pTheFile->Write((LPTR)&infohdr.biCompression,   4);
		pTheFile->Write((LPTR)&infohdr.biSizeImage,     4);
		pTheFile->Write((LPTR)&infohdr.biXPelsPerMeter, 4);
		pTheFile->Write((LPTR)&infohdr.biYPelsPerMeter, 4);
		pTheFile->Write((LPTR)&infohdr.biClrUsed,       4);
		pTheFile->Write((LPTR)&infohdr.biClrImportant,  4);

		switch (infohdr.biBitCount)
		{
			case 1:
				rgbq.rgbReserved = 0;

				/* write the color map */
				rgbq.rgbRed   =
				rgbq.rgbGreen =
				rgbq.rgbBlue  = 0;
				pTheFile->Write((LPTR) &rgbq, 4);

				rgbq.rgbRed   =
				rgbq.rgbGreen =
				rgbq.rgbBlue  = 255;
				pTheFile->Write((LPTR) &rgbq, 4);
			break;

			case 4:
				rgbq.rgbReserved = 0;

				/* write the color map */
				if (fIn16Color)
				{
					for (i = 0; i < 16; i++)
					{
						rgbq.rgbRed   = inType.ColorMap->RGBData[i].red;
						rgbq.rgbGreen = inType.ColorMap->RGBData[i].green;
						rgbq.rgbBlue  = inType.ColorMap->RGBData[i].blue;
						pTheFile->Write((LPTR) &rgbq, 4);
					}
				}
				else
				{
					for (i = 0; i < 16; i++)
					{
						rgbq.rgbRed   = (i&RED_LO) ? 0xFF : 0x00;
						rgbq.rgbGreen = (i&GRN_LO) ? 0xFF : 0x00;
						rgbq.rgbBlue  = (i&BLU_LO) ? 0xFF : 0x00;
						pTheFile->Write((LPTR) &rgbq, 4);
					}
				}
			break;

			case 8:
				if ( m_DataType == FDT_GRAYSCALE )
				{
					rgbq.rgbReserved = 0;

					/* write the color map */
					for (i = 0; i < 256; i++)
					{
						rgbq.rgbRed   = i;
						rgbq.rgbGreen = i;
						rgbq.rgbBlue  = i;
						pTheFile->Write((LPTR) &rgbq, 4);
					}
				}
				else
				{
					// see if we need to create a palette
					if (!outType.ColorMap)
						{
						outType.ColorMap = FrameCreateColorMap();
						if (!outType.ColorMap)
							{
							SetError(BEC_errMemory);
							//Message(IDS_EMEMALLOC);
							goto Exit;
							}
						if (m_BitsPerPixel == 4)
							outType.ColorMap->NumEntries = 16;
						else
							outType.ColorMap->NumEntries = 256;
						if (!CreateOptimizedPalette(lpObject, lpFrame,
											outType.ColorMap->RGBData,
											&outType.ColorMap->NumEntries,
											TRUE,
											NULL))
							{
							SetError(BEC_errMemory);
							//Message(IDS_EMEMALLOC);
							goto Exit;
							}
						}
					lpRGBmap = outType.ColorMap->RGBData;
					rgbq.rgbReserved = 0;

					/* write the color map */
					for (i = 0; i < 256; i++)
					{
						rgbq.rgbRed   = lpRGBmap[i].red;
						rgbq.rgbGreen = lpRGBmap[i].green;
						rgbq.rgbBlue  = lpRGBmap[i].blue;
						pTheFile->Write((LPTR) &rgbq, 4);
					}
				}
			break;

			case 24:
				/* no color map */
			break;
		}

		// initialize stuff to do the type conversion
		if (!TypeConvert.Init(inType, outType, npix, m_DitherType))
		{
			SetError(BEC_errMemory);
			//Message(IDS_EMEMALLOC);
			goto Exit;
		}
		for (y = rSave.bottom; y >= rSave.top; y--)
		{
			if (Progress(rSave.bottom-y, nlin, bEscapable ))
				goto Cancelled;

			if ( lpObject )
			{
				if (!ObjGetLine( lpObject, rSave.left, y,
					(rSave.right - rSave.left) + 1, lpImgScanline))
					goto BadRead;
				lp = lpImgScanline;
			}
			else
			{
				if ( !(lp = FramePointer( lpFrame, rSave.left, y, NO )) )
					goto BadRead;
			}
			// convert the data to the new type
			TypeConvert.ConvertData(lp, lpBuffer, y, npix);

			if (infohdr.biBitCount == 4)
			{
				if (fIn16Color)
					Convert8BitTo4Bit(lpBuffer, lpImgScanline, npix);
				else
					Dither16(lpBuffer, lpImgScanline, npix, y);
				lp = lpImgScanline;
			}
			else
			if (outType.DataType == FDT_RGBCOLOR)
			{
				/* swap from rgb to bgr */
				swapBGR(lpBuffer, lpImgScanline, npix);
				lp = lpImgScanline;
			}
			else
				lp = lpBuffer;

			pTheFile->Write(lp, bpl );
		}
		fRet = TRUE;
		goto Exit;
	}
	CATCH_ALL(e)
	{
		goto BadWrite;
	}
	END_CATCH_ALL

BadWrite:
	SetError(BEC_errWriteFile);
	//Message (IDS_EWRITE, m_lpFileName);
	goto Exit;

BadRead:
	SetError(BEC_errFrameRead);
	//Message (IDS_EREAD, (LPTR)Control.RamDisk);

Cancelled:
Exit:
	/* clean up */
	CloseFile(pTheFile, fRet);
	if (lpBuffer)
		FreeUp (lpBuffer);
	if (lpImgScanline)
		FreeUp (lpImgScanline);
	if (outType.ColorMap && (outType.ColorMap != inType.ColorMap))
		FrameDestroyColorMap(outType.ColorMap);
	ProgressEnd();

	return (fRet);
}

/*=========================================================================*/




