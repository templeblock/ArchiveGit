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

#include "stdafx.h"
#include "mgxbm.h"
#include "imgutil.h"

/*=========================================================================*/

#define BMHDRSZ	sizeof(BITMAPFILEHEADER)	/* size of bitmap file header */

/*=========================================================================*/

void Expand4to24bits(LPTR lpSrc, LPRGB lpDst, int iCount, LPRGB lpRGBmap)
{
	int i;
	BYTE p;

	if (!lpRGBmap)
		return;

	// expand 4-bit data
	for (i = 0; i < iCount; i++, lpSrc++)
	{
		p = (*lpSrc >> 4) & 0x0F;
		lpDst->red   = lpRGBmap[ p ].red;
		lpDst->green = lpRGBmap[ p ].green;
		lpDst->blue  = lpRGBmap[ p ].blue;
		lpDst++;

		p = *lpSrc & 0x0F;
		lpDst->red   = lpRGBmap[ p ].red;
		lpDst->green = lpRGBmap[ p ].green;
		lpDst->blue  = lpRGBmap[ p ].blue;
		lpDst++;
	}
}

/*=========================================================================*/

void Expand4to8bits(LPTR lpSrc, LPTR lpDst, int iCount)
{
	// expand 4-bit data
	while (--iCount >= 0)
	{
		*lpDst++ = (*lpSrc >> 4) & 0x0F;
		*lpDst++ = *lpSrc & 0x0F;
		++lpSrc;
	}
}

/*=========================================================================*/

static int bmpReadHeader( CFile *pFile, BITMAPFILEHEADER *hdr )
{
	int err;

	TRY
	{
		pFile->Read(&hdr->bfType, 2);
		pFile->Read(&hdr->bfSize, 4);
		pFile->Read(&hdr->bfReserved1, 2);
		pFile->Read(&hdr->bfReserved2, 2);
		pFile->Read(&hdr->bfOffBits, 4);

		if (hdr->bfType != 0x4D42)
		{	/* BM */
			//Print ("Invalid Type");
			err = -1;
		}
	}
	CATCH_ALL(e)
	{
		err = -1;
	}
	END_CATCH_ALL

	return (err);
}

/*=========================================================================*/

BOOL CReadBitmap::BMPRead()
{
	LPTR    lpFileLine, lpCacheLine;
	LPFRAME lpFrame, lpOldFrame;
	LPTR    lp, lpOut;
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER infohdr;
	LPRGB    lpRGBmap;
	RGBQUAD rgbq;
	RGBTRIPLE rgbt;
	BOOL graymap, bOS2, graylinear, fNegate;
	int  i;
	int  sy;
	int  FileBPL;	/* bytes per line (bmp file) */
	int  xres;		/* pixels per inch */
	int  bpp;		/* bits per pixel */
	int  npix;		/* image width (pixels) */
	int  nlin;		/* image height (pixels) */
	int  indepth;	/* bytes per pixel (file 1 or 3) */
	FRMTYPEINFO inType, outType;
	LPCOLORMAP lpColorMap;
	CFrameTypeConvert TypeConvert;
	CFile*	pTheFile;
	BOOL	fRet = FALSE;

	ProgressBegin(1);
	if ((pTheFile = OpenFile()) == NULL)
	{
		ProgressEnd();
		return(FALSE);
	}

	TRY
	{
		lpFileLine = NULL;
		lpCacheLine = NULL;
		lpFrame = NULL;
		lpOldFrame = NULL;
		lpColorMap = NULL;
		lpRGBmap = NULL;

		/***********************************/
		/* determine image characteristics */
		/***********************************/
		/* read bmp file header */
		if (bmpReadHeader (pTheFile, &hdr) == -1)
			goto BadRead;

		/* read size needed for following structure (determines which structure) */
		if (pTheFile->Read(&infohdr.biSize, 4) != 4)
			goto BadRead;

		/* check for bitmapcoreheader */
		bOS2 = (infohdr.biSize == sizeof (BITMAPCOREHEADER));

		if ( bOS2 )
		{
			/* bitmapinfocoreheader */
			pTheFile->Read(&i, 2);
			infohdr.biWidth = i;

			pTheFile->Read(&i, 2);
			infohdr.biHeight = i;

			pTheFile->Read( &infohdr.biPlanes,   2);
			pTheFile->Read( &infohdr.biBitCount, 2);
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
			pTheFile->Read( &infohdr.biWidth, 4);
			pTheFile->Read( &infohdr.biHeight, 4);
			pTheFile->Read( &infohdr.biPlanes, 2);
			pTheFile->Read( &infohdr.biBitCount, 2);
			pTheFile->Read( &infohdr.biCompression, 4);
			pTheFile->Read( &infohdr.biSizeImage, 4);
			pTheFile->Read( &infohdr.biXPelsPerMeter, 4);
			pTheFile->Read( &infohdr.biYPelsPerMeter, 4);
			pTheFile->Read( &infohdr.biClrUsed, 4);
			pTheFile->Read( &infohdr.biClrImportant, 4);

			/* validate that image is not compressed */
			if (infohdr.biCompression != BI_RGB)
			{
				//Print ("Compressed bitmaps not supported.");
				goto Exit;
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
				FrameSetTypeInfo(&inType, FDT_LINEART);
			break;

			case 4:
				if ((infohdr.biClrUsed == 0) || (infohdr.biClrUsed > 16))
					infohdr.biClrUsed = 16;
				bpp = 4;
				indepth = 3;
				FileBPL = (infohdr.biWidth + 1) / 2;
				FrameSetTypeInfo(&inType, FDT_GRAYSCALE);
			break;

			case 8:
				if ((infohdr.biClrUsed == 0) || (infohdr.biClrUsed > 256))
					infohdr.biClrUsed = 256;
				bpp = 8;
				indepth = 1;
				FileBPL = infohdr.biWidth;
				FrameSetTypeInfo(&inType, FDT_GRAYSCALE);
			break;

			case 24:
				infohdr.biClrUsed = 0;
				bpp = 24;
				indepth = 3;
				FileBPL = 3*infohdr.biWidth;
				FrameSetTypeInfo(&inType, FDT_RGBCOLOR);
			break;

			default:
				goto BadRead;
			break;
		}

		m_BitsPerPixel = bpp;

		/* adjust bytes per line to DWORD (long) boundry */
		FileBPL = (FileBPL + 3) & ~3;

		/* read color map, if any */
		if (infohdr.biClrUsed)
		{
			/* seek to loc of colormap */
			pTheFile->Seek (infohdr.biSize + BMHDRSZ, CFile::begin);
			graymap    = TRUE;
			graylinear = TRUE;

			lpColorMap = FrameCreateColorMap();
			if (!lpColorMap)
			{
				SetError(BEC_errMemory);
				//Message(IDS_EMEMALLOC);
				goto Exit;
			}
			lpRGBmap = lpColorMap->RGBData;
			lpColorMap->NumEntries = infohdr.biClrUsed;
			for (i = 0; i < infohdr.biClrUsed; i++)
			{
				if ( bOS2 )
				{
					pTheFile->Read( (LPTR) &rgbt, 3);
					lpRGBmap[i].red   = rgbt.rgbtRed;
					lpRGBmap[i].green = rgbt.rgbtGreen;
					lpRGBmap[i].blue  = rgbt.rgbtBlue;
				}
				else
				{
					pTheFile->Read( (LPTR) &rgbq, 4);
					lpRGBmap[i].red   = rgbq.rgbRed;
					lpRGBmap[i].green = rgbq.rgbGreen;
					lpRGBmap[i].blue  = rgbq.rgbBlue;
				}
				if (graymap)
				{
					graymap = (lpRGBmap[i].red == lpRGBmap[i].green) &&
				  	(lpRGBmap[i].red == lpRGBmap[i].blue);

					if (graylinear)
					{
						if (lpRGBmap[i].red != i)
							graylinear = FALSE;
					}

					if (!graymap)
						graylinear = FALSE;
				}
			}

			if (bpp > 1)
				FrameSetTypeInfo(&inType, FDT_PALETTECOLOR, lpColorMap);
		}

		if (!SetupTypes(&inType, &outType, graymap))
			goto Exit;
		
		/* Seek to location of bitmap data */
		pTheFile->Seek(hdr.bfOffBits, CFile::begin);

		/* Create the image frame store */
		lpFrame = FrameOpen(outType, npix, nlin, xres);
		if ( !lpFrame )
		{
			SetError(BEC_errFrameOpen);
			//FrameError(IDS_EIMAGEOPEN);
			goto Exit;
		}

		/* allocate space for one line of the image */
		if ( !AllocLines (&lpCacheLine, 1, npix, 3))
		{
			SetError(BEC_errMemory);
			//Message (IDS_EMEMALLOC );
			goto Exit;
		}

		/* allocate space for one line of the image (bmp file) */
		if ( !AllocLines (&lpFileLine, 1, FileBPL, 1))
		{
			SetError(BEC_errMemory);
			//Message (IDS_EMEMALLOC);
			goto Exit;
		}

		if (!TypeConvert.Init(inType, outType, npix, m_DitherType))
		{
			SetError(BEC_errMemory);
			//Message(IDS_EMEMALLOC);
			goto Exit;	
		}

		fNegate = (bpp == 1) && (lpRGBmap != NULL) && (lpRGBmap[0].red != 0);
		for (i = 0; i < nlin; i++)
		{
			sy = nlin - i - 1;
			if (Progress (i, nlin, YES))
				goto Cancelled;

			if ( !(lpOut = FramePointerRaw(lpFrame, 0, sy, YES)) )
				goto BadWrite;

			if ( pTheFile->Read( lpFileLine, FileBPL) != FileBPL)
				goto BadRead;
	
			if (fNegate)
				negate (lpFileLine, (long)FileBPL);

			if (bpp == 4)
			{
				Expand4to8bits(lpFileLine, lpCacheLine, FileBPL);
				lp = lpCacheLine;
			}
			else
			if (bpp == 24)
			{
				swapBGR(lpFileLine, lpCacheLine, npix);
				lp = lpCacheLine;
			}
			else
				lp = lpFileLine;

			TypeConvert.ConvertData(lp, lpOut, i, npix);
		}

		m_iWidth = npix;
		m_iHeight = nlin;
		m_iRes = xres;
		m_ObjList.lpHead = m_ObjList.lpTail = NULL;
		m_lpFrame = lpFrame;
		fRet = TRUE;
		goto Exit;
	}
	CATCH_ALL(e)
	{
		goto BadRead;
	}
	END_CATCH_ALL

BadRead:
	SetError(BEC_errFileRead);
	//Message( IDS_EREAD, m_lpFileName );
	goto Exit;

BadWrite:
	SetError(BEC_errFrameRead);
	//FrameError(IDS_EFRAMEREAD);	

Cancelled:
Exit:

	if ( lpFileLine )
		FreeUp( lpFileLine );
	if ( lpCacheLine )
		FreeUp( lpCacheLine );
	if ( lpColorMap )
		FrameDestroyColorMap(lpColorMap);
	CloseFile(pTheFile);

	if (!fRet && lpFrame)
		FrameClose(lpFrame);

	ProgressEnd();

	return( fRet );
}

/*=========================================================================*/



