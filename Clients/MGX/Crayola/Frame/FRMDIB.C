/*=======================================================================*\

	FRMCONV.C - Frame <--> DIB Conversion routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include <windows.h>
#include <stdio.h>
#include "framelib.h"
#include "macros.h"

/*=======================================================================*/

static LPTR FrameToLineArtDIB( LPFRAME lpFrame, LPRECT lpRect );
static LPTR FrameToPaletteDIB( LPFRAME lpFrame, LPRECT lpRect );
static LPTR FrameToColorDIB(   LPFRAME lpFrame, LPRECT lpRect );

static LPFRAME DIBToLineArtFrame( LPTR lpDIBMem, BOOL bForceRGB );
static LPFRAME DIBToPaletteFrame( LPTR lpDIBMem, BOOL bForceRGB );
static LPFRAME DIBToColorFrame(   LPTR lpDIBMem, BOOL bForceRGB );

/*=======================================================================*/

LPTR FrameToDIB( LPFRAME lpFrame, LPRECT lpRect )
{
	RECT Rect;

	// Make sure that this is a valid pointer
	if (!lpFrame)
	{
		return(NULL);
	}

	// Validate the image rectangle
	if (lpRect == NULL)
	{
		lpRect = &Rect;

		Rect.top    = 0;
		Rect.left   = 0;
		Rect.bottom = FrameYSize( lpFrame ) - 1;
		Rect.right  = FrameXSize( lpFrame ) - 1;
	}

	// Call the appropriate conversion function
	switch (FrameType( lpFrame ))
	{
		case FDT_LINEART      : 
			return(FrameToLineArtDIB( lpFrame, lpRect ));
		break;

		case FDT_GRAYSCALE    : 
		case FDT_PALETTECOLOR : 
			return(FrameToPaletteDIB( lpFrame, lpRect ));
		break;

		case FDT_RGBCOLOR     : 
		case FDT_CMYKCOLOR    : 
			return(FrameToColorDIB(   lpFrame, lpRect ));
		break;
	}

	return(NULL);
}

/*=======================================================================*/

static LPTR FrameToLineArtDIB( LPFRAME lpFrame, LPRECT lpRect )
{
	int y, bpl, fbpl, i;
	LPTR lpDIBMem;
	HPTR lpDst;
	HPTR lpDstLine;
	LPTR lpSrc;
	DWORD lCount, lImageSize, lInfoSize;
	int iCount, dx, dy;
	LPBITMAPINFO lpInfo;

	if ( !lpFrame )
		return( NULL );

	dx = (lpRect->right  - lpRect->left) + 1;
	dy = (lpRect->bottom - lpRect->top)  + 1;

	bpl = (dx+7)/8;

	lInfoSize = sizeof(BITMAPINFOHEADER) + 2*sizeof(RGBQUAD);
	bpl       = 4 * ((bpl + 3)/4); // DIB packing

	lImageSize = (long)bpl * dy;
	lCount     = lInfoSize + lImageSize;

	// Allocate the memory to hold the DIB
	if ( !(lpDIBMem = Alloc( lCount )) )
	{
		FrameSetError( ERR_MALLOC );
		return( NULL );
	}

	lpInfo = (LPBITMAPINFO)lpDIBMem;
	lpInfo->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
	lpInfo->bmiHeader.biWidth         = dx;
	lpInfo->bmiHeader.biHeight        = dy;
	lpInfo->bmiHeader.biPlanes        = 1;
	lpInfo->bmiHeader.biCompression   = BI_RGB;
	lpInfo->bmiHeader.biBitCount      = 1;
	lpInfo->bmiHeader.biSizeImage     = lImageSize;
	lpInfo->bmiHeader.biXPelsPerMeter = (10000L * (long) FrameResolution(lpFrame)) / 254L;
	lpInfo->bmiHeader.biYPelsPerMeter = (10000L * (long) FrameResolution(lpFrame)) / 254L;
	lpInfo->bmiHeader.biClrUsed       = 2;
	lpInfo->bmiHeader.biClrImportant  = 2;

	for (i = 0; i < 2; i++)
	{
		lpInfo->bmiColors[i].rgbBlue     = (i&1) ? 0xFF : 0x00;
		lpInfo->bmiColors[i].rgbGreen    = (i&1) ? 0xFF : 0x00;
		lpInfo->bmiColors[i].rgbRed      = (i&1) ? 0xFF : 0x00;
		lpInfo->bmiColors[i].rgbReserved = 0;
	}

	lpDstLine = (HPTR)lpDIBMem;
	lpDstLine += lInfoSize;

	fbpl = FrameByteWidth( lpFrame );

	// This goes backwards...
	for (y = lpRect->bottom; y >= lpRect->top; y--)
	{
		lpDst = lpDstLine;
		lpSrc = FramePointerRaw(lpFrame, lpRect->left, y, NO);

		if (lpSrc)
		{
			if (bpl > fbpl)
			{
				iCount = fbpl;

				while (iCount-- > 0)
					*lpDst++ = *lpSrc++;

				iCount = bpl-fbpl;

				while (iCount-- > 0)
					*lpDst++ = 0;
			}
			else
			{
				iCount = bpl;

				while (iCount-- > 0)
					*lpDst++ = *lpSrc++;
			}
		}

		lpDstLine += bpl;
	}

	return( lpDIBMem );
}

/*=======================================================================*/

static LPTR FrameToPaletteDIB( LPFRAME lpFrame, LPRECT lpRect )
{
	int y, bpl, i;
	LPTR lpSrc, lpDIBMem;
	HPTR lpDstLine;
	HPTR lpDst;
	DWORD lCount, lImageSize, lInfoSize;
	int iCount, dx, dy;
	LPBITMAPINFO lpInfo;

	if ( !lpFrame )
		return( NULL );

	dx = (lpRect->right  - lpRect->left) + 1;
	dy = (lpRect->bottom - lpRect->top)  + 1;

	bpl = dx;

	lInfoSize = sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD);
	bpl = 4 * ((bpl + 3)/4); // DIB packing

	lImageSize = (long)bpl * dy;
	lCount     = lInfoSize + lImageSize;

	// Allocate the memory to hold the DIB
	if ( !(lpDIBMem = Alloc( lCount )) )
	{
		FrameSetError( ERR_MALLOC );
		return( NULL );
	}

	lpInfo = (LPBITMAPINFO)lpDIBMem;
	lpInfo->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
	lpInfo->bmiHeader.biWidth         = dx;
	lpInfo->bmiHeader.biHeight        = dy;
	lpInfo->bmiHeader.biPlanes        = 1;
	lpInfo->bmiHeader.biCompression   = BI_RGB;
	lpInfo->bmiHeader.biBitCount      = 8;
	lpInfo->bmiHeader.biSizeImage     = lImageSize;
	lpInfo->bmiHeader.biXPelsPerMeter = (10000L * (long) FrameResolution(lpFrame)) / 254L;
	lpInfo->bmiHeader.biYPelsPerMeter = (10000L * (long) FrameResolution(lpFrame)) / 254L;
	lpInfo->bmiHeader.biClrUsed       = 256;
	lpInfo->bmiHeader.biClrImportant  = 256;

	if (FrameType(lpFrame) == FDT_GRAYSCALE)
	{
		for (i = 0; i < 256; ++i)
		{
			lpInfo->bmiColors[i].rgbBlue     = i;
			lpInfo->bmiColors[i].rgbGreen    = i;
			lpInfo->bmiColors[i].rgbRed      = i;
			lpInfo->bmiColors[i].rgbReserved = 0;
		}
	}
	else
	{
		LPCOLORMAP lpColorMap;

		lpColorMap = FrameGetColorMap( lpFrame );

		if (!lpColorMap)
		{
			for (i = 0; i < 256; ++i)
			{
				lpInfo->bmiColors[i].rgbBlue     = i;
				lpInfo->bmiColors[i].rgbGreen    = i;
				lpInfo->bmiColors[i].rgbRed      = i;
				lpInfo->bmiColors[i].rgbReserved = 0;
			}
		}
		else
		{
			for (i = 0; i < 256; ++i)
			{
				lpInfo->bmiColors[i].rgbBlue     = lpColorMap->RGBData[i].blue;
				lpInfo->bmiColors[i].rgbGreen    = lpColorMap->RGBData[i].green;
				lpInfo->bmiColors[i].rgbRed      = lpColorMap->RGBData[i].red;
				lpInfo->bmiColors[i].rgbReserved = 0;
			}
		}
	}

	lpDstLine = (HPTR)lpDIBMem;
	lpDstLine += lInfoSize;

	// This goes backwards...
	for (y = lpRect->bottom; y >= lpRect->top; y--)
	{
		lpDst = lpDstLine;
		lpSrc = FramePointer(lpFrame, lpRect->left, y, NO);

		if (lpSrc)
		{
			iCount = dx;

			while (iCount-- > 0)
				*lpDst++ = *lpSrc++;
		}
		lpDstLine += bpl;
	}

	return( lpDIBMem );
}

/*=======================================================================*/

static LPTR FrameToColorDIB( LPFRAME lpFrame, LPRECT lpRect )
{
	int y, bpl;
	LPTR lpSrc, lpDIBMem, lpConvertLine;
	HPTR hpDstLine;
	DWORD lCount, lImageSize, lInfoSize;
	int dx, dy;
	LPBITMAPINFO lpInfo;
	FRMTYPEINFO SrcTypeInfo, DstTypeInfo;
	CFrameTypeConvert TypeConvert;

	if ( !lpFrame )
		return( NULL );

	FrameGetTypeInfo(lpFrame, &SrcTypeInfo);
	FrameSetTypeInfo(&DstTypeInfo, FDT_RGBCOLOR);

	dx = (lpRect->right  - lpRect->left) + 1;
	dy = (lpRect->bottom - lpRect->top)  + 1;

	lInfoSize = sizeof(BITMAPINFOHEADER);
	bpl = dx * 3;  // Always 24-bit color out

	bpl        = 4 * ((bpl + 3) / 4); // DIB packing
	lImageSize = (long)bpl * dy;
	lCount     = lInfoSize + lImageSize;

	// Allocate the memory to hold the DIB
	if ( !(lpDIBMem = Alloc( lCount )) )
	{
		FrameSetError( ERR_MALLOC );
		return( NULL );
	}

	if ( !(lpConvertLine = Alloc( bpl )) )
	{
		FreeUp(lpDIBMem);
		FrameSetError( ERR_MALLOC );
		return( NULL );
	}

	lpInfo = (LPBITMAPINFO)lpDIBMem;
	lpInfo->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
	lpInfo->bmiHeader.biWidth         = dx;
	lpInfo->bmiHeader.biHeight        = dy;
	lpInfo->bmiHeader.biPlanes        = 1;
	lpInfo->bmiHeader.biCompression   = BI_RGB;
	lpInfo->bmiHeader.biBitCount      = 24;
	lpInfo->bmiHeader.biSizeImage     = lImageSize;
	lpInfo->bmiHeader.biXPelsPerMeter = (10000L * (long) FrameResolution(lpFrame)) / 254L;
	lpInfo->bmiHeader.biYPelsPerMeter = (10000L * (long) FrameResolution(lpFrame)) / 254L;
	lpInfo->bmiHeader.biClrUsed       = 0;
	lpInfo->bmiHeader.biClrImportant  = 0;

	hpDstLine = (HPTR)lpDIBMem;
	hpDstLine += lInfoSize;

	if (!TypeConvert.Init(SrcTypeInfo, DstTypeInfo, dx))
	{
		FreeUp(lpDIBMem);
		FreeUp(lpConvertLine);
		FrameSetError( ERR_MALLOC );
		return( NULL );
	}

	// This goes backwards...
	for (y = lpRect->bottom; y >= lpRect->top; y--)
	{
		lpSrc = FramePointer(lpFrame, lpRect->left, y, NO);
		if (lpSrc)
		{
			TypeConvert.ConvertData(lpSrc, lpConvertLine, y, dx);
			swapBGR( lpConvertLine, lpConvertLine, dx);
			CopyToHuge(lpConvertLine, hpDstLine, bpl);
		}
		hpDstLine += bpl;
	}

	FreeUp(lpConvertLine);

	return( lpDIBMem );
}

/*=======================================================================*/

LPFRAME DIBToFrame( LPTR lpDIBMem, BOOL bForceRGB )
{
	LPBITMAPINFO lpInfo;

	// Make sure that the DB is valid
	if (!lpDIBMem) return( NULL );

	lpInfo = (LPBITMAPINFO)lpDIBMem;

	// Make sure that the DIB is uncompressed
	if (lpInfo->bmiHeader.biCompression != BI_RGB)
	{
		return(NULL);
	}

	// Call the appropriate conversion function
	switch( lpInfo->bmiHeader.biBitCount )
	{
		case 1 : 
			return(DIBToLineArtFrame( lpDIBMem, bForceRGB));
		break;

		case 4 :
		case 8 :
			return(DIBToPaletteFrame( lpDIBMem, bForceRGB));
		break;

		case 16 :
		case 24 :
			return(DIBToColorFrame( lpDIBMem, bForceRGB));
		break;
	}

	return(NULL);
}

/*=======================================================================*/

LPFRAME DIBToLineArtFrame( LPTR lpDIBMem, BOOL bForceRGB )
{
	LPBITMAPINFO lpInfo;
	LPFRAME lpFrame;
	HPTR lpSrc;
	HPTR lpSrcLine;
	LPTR lpDst;
	int iResolution;
	int iFrameXSize;
	int iFrameYSize;
	int iPixelFlavor;
	int iSrcWidth;
	int iDstWidth;
	int iCount;
	int y;

	lpInfo = (LPBITMAPINFO)lpDIBMem;

	// Get the important bitmap information
	iFrameXSize = lpInfo->bmiHeader.biWidth;
	iFrameYSize = lpInfo->bmiHeader.biHeight;
	iResolution = ((lpInfo->bmiHeader.biXPelsPerMeter) * 254L) / 10000L;

	// Make sure that we have something to do
	if ((iFrameXSize <= 0) || (iFrameYSize <= 0))
	{
		return(NULL);
	}

	// Validate the image resolution
	if (iResolution <= 0)
	{
		iResolution = 75;
	}

	// Find out whether or not we need to invert
	iPixelFlavor = (lpInfo->bmiColors[0].rgbBlue == 0);

	// Create the new image frame
	lpFrame = FrameOpen(FDT_LINEART, iFrameXSize, iFrameYSize, iResolution);

	if (!lpFrame) return(NULL);

	// Find the DIB data pointer and width
	lpSrcLine = lpDIBMem + lpInfo->bmiHeader.biSize + 
		lpInfo->bmiHeader.biClrUsed * (sizeof(RGBQUAD));

	iSrcWidth = ((lpInfo->bmiHeader.biWidth+31)/32) * 4;

	iDstWidth = FrameByteWidth( lpFrame );

	// Copy the DIB data into the Frame
	for(y=iFrameYSize-1;y >= 0;y--)
	{
		lpSrc = lpSrcLine;
		lpDst = FramePointerRaw( lpFrame, 0, y, TRUE );

		iCount = iDstWidth;

		if (iPixelFlavor)
		{
			while(iCount-- > 0)
			{
				*lpDst++ = *lpSrc++;
			}
		}
		else
		{
			while(iCount-- > 0)
			{
				*lpDst++ = (*lpSrc++)^0xFF;
			}
		}
		lpSrcLine += iSrcWidth;
	}

	return(lpFrame);
}

/*=======================================================================*/

LPFRAME DIBToPaletteFrame( LPTR lpDIBMem, BOOL bForceRGB )
{
	FRMDATATYPE FrameDataType;
	LPBITMAPINFO lpInfo;
	LPCOLORMAP lpColorMap;
	LPRGBQUAD lpRGBQColorMap;
	LPFRAME lpFrame;
	HPTR lpSrcLine;
	HPTR lpSrc;
	LPTR lpDst;
	BOOL bGrayMap;
	int iColorMapEntries;
	int iResolution;
	int iFrameXSize;
	int iFrameYSize;
	int iSrcWidth;
	int iDstWidth;
	int iCount;
	int y;
	int i;

	lpInfo = (LPBITMAPINFO)lpDIBMem;

	// Get the important bitmap information
	iFrameXSize = lpInfo->bmiHeader.biWidth;
	iFrameYSize = lpInfo->bmiHeader.biHeight;
	iResolution = ((lpInfo->bmiHeader.biXPelsPerMeter) * 254L) / 10000L;

	// Make sure that we have something to do
	if ((iFrameXSize <= 0) || (iFrameYSize <= 0))
	{
		return(NULL);
	}

	// Validate the image resolution
	if (iResolution <= 0)
	{
		iResolution = 75;
	}

	lpRGBQColorMap = &lpInfo->bmiColors[0];
	bGrayMap = TRUE;

	if (lpInfo->bmiHeader.biBitCount == 4)
		iColorMapEntries = 16;
	else
		iColorMapEntries = 256;

	// Find out whether or not this is a grayscale image
	for(i=0;i<iColorMapEntries;i++)
	{
		if (bGrayMap)
		{
			if ((lpRGBQColorMap[i].rgbBlue != lpRGBQColorMap[i].rgbRed) ||
				(lpRGBQColorMap[i].rgbBlue != lpRGBQColorMap[i].rgbRed))
			{
				bGrayMap = FALSE;
				break;
			}
		}
	}

	if (bGrayMap)
	{
		FrameDataType = FDT_GRAYSCALE;
		bForceRGB     = FALSE;
	}
	else
	{
		if (bForceRGB)
		{
			FrameDataType = FDT_RGBCOLOR;
		}
		else
		{
			FrameDataType = FDT_PALETTECOLOR;
		}
	}

	// Create the new image frame
	lpFrame = FrameOpen(FrameDataType, iFrameXSize, iFrameYSize, iResolution);

	if (!lpFrame) return(NULL);

	// Add the colormap to the frame if necessary
	if (!bGrayMap)
	{
		// Allocate space for the colormap

		lpColorMap = (LPCOLORMAP)Alloc( sizeof(COLORMAP)+ sizeof(RGBS) * 256);

		if (!lpColorMap)
		{
			FrameClose(lpFrame);
			return(NULL);
		}

		lpColorMap->NumEntries = 256;

		for(i=0;i<iColorMapEntries;i++)
		{
			lpColorMap->RGBData[i].red   = lpRGBQColorMap[i].rgbRed;
			lpColorMap->RGBData[i].green = lpRGBQColorMap[i].rgbGreen;
			lpColorMap->RGBData[i].blue  = lpRGBQColorMap[i].rgbBlue;
		}

		for(i=iColorMapEntries;i<256;i++)
		{
			lpColorMap->RGBData[i].red   = 0;
			lpColorMap->RGBData[i].green = 0;
			lpColorMap->RGBData[i].blue  = 0;
		}

		if (FrameDataType == FDT_RGBCOLOR)
			if (!FrameSetColorMap( lpFrame, lpColorMap ))
			{
				FrameClose( lpFrame );
				return(NULL);
			}
	}

	// Find the DIB data pointer and width
	lpSrcLine = lpDIBMem + lpInfo->bmiHeader.biSize + 
		iColorMapEntries * (sizeof(RGBQUAD));
//OLD VERSION USED  lpInfo->bmiHeader.biClrUsed instead of iColorMapEntries
// some 4 bit DIB's don't set biCLrUsed that way

	if (lpInfo->bmiHeader.biBitCount == 4)
		iSrcWidth = ((lpInfo->bmiHeader.biWidth+7)/8)*4;
	else
		iSrcWidth = ((lpInfo->bmiHeader.biWidth+3)/4)*4;

	iDstWidth = FrameByteWidth( lpFrame );

	if (bForceRGB)
	{
		// Copy the DIB data into the RGB Frame
		if (lpInfo->bmiHeader.biBitCount == 4)
		{
			for(y=iFrameYSize-1;y >= 0;y--)
			{
				lpSrc = lpSrcLine;
				lpDst = FramePointer( lpFrame, 0, y, TRUE );

				iCount = (iFrameXSize+1)/2;

				while(iCount-- > 0)
				{
					*lpDst++ = lpColorMap->RGBData[(*lpSrc & 0xF0)>>4].red;
					*lpDst++ = lpColorMap->RGBData[(*lpSrc & 0xF0)>>4].green;
					*lpDst++ = lpColorMap->RGBData[(*lpSrc & 0xF0)>>4].blue;

					*lpDst++ = lpColorMap->RGBData[(*lpSrc & 0x0F)].red;
					*lpDst++ = lpColorMap->RGBData[(*lpSrc & 0x0F)].green;
					*lpDst++ = lpColorMap->RGBData[(*lpSrc & 0x0F)].blue;
					lpSrc++;
				}

				lpSrcLine += iSrcWidth;
			}
		}
		else
		{
			for(y=iFrameYSize-1;y >= 0;y--)
			{
				lpSrc = lpSrcLine;
				lpDst = FramePointer( lpFrame, 0, y, TRUE );

				iCount = iFrameXSize;

				while(iCount-- > 0)
				{
					*lpDst++ = lpColorMap->RGBData[*lpSrc].red;
					*lpDst++ = lpColorMap->RGBData[*lpSrc].green;
					*lpDst++ = lpColorMap->RGBData[*lpSrc].blue;
					lpSrc++;
				}

				lpSrcLine += iSrcWidth;
			}
		}
	}
	else
	{
		// Copy the DIB data into the Frame
		if (lpInfo->bmiHeader.biBitCount == 4)
		{
			for(y=iFrameYSize-1;y >= 0;y--)
			{
				lpSrc = lpSrcLine;
				lpDst = FramePointer( lpFrame, 0, y, TRUE );

				iCount = (iFrameXSize+1)/2;

				while(iCount-- > 0)
				{
					*lpDst++ = (*lpSrc & 0xF0)>>4;
					*lpDst++ = (*lpSrc & 0x0F);
					lpSrc++;
				}

				lpSrcLine += iSrcWidth;
			}
		}
		else
		{
			for(y=iFrameYSize-1;y >= 0;y--)
			{
				lpSrc = lpSrcLine;
				lpDst = FramePointer( lpFrame, 0, y, TRUE );

				iCount = iFrameXSize;

				while(iCount-- > 0)
				{
					*lpDst++ = *lpSrc++;
				}

				lpSrcLine += iSrcWidth;
			}
		}
	}

	return(lpFrame);
}

/*=======================================================================*/

LPFRAME DIBToColorFrame( LPTR lpDIBMem, BOOL bForceRGB )
{
	LPBITMAPINFO lpInfo;
	LPFRAME lpFrame;
	HPTR lpSrcLine;
	HPTR lpSrc;
	LPTR lpDst;
	int iResolution;
	int iFrameXSize;
	int iFrameYSize;
	int iSrcWidth;
	int iDstWidth;
	int iCount;
	int y;
	WORD word;

	lpInfo = (LPBITMAPINFO)lpDIBMem;

	// Make sure we handle it
	if (lpInfo->bmiHeader.biBitCount != 16 &&
			lpInfo->bmiHeader.biBitCount != 24)
	{
		return(NULL);
	}

	// Get the important bitmap information
	iFrameXSize = lpInfo->bmiHeader.biWidth;
	iFrameYSize = lpInfo->bmiHeader.biHeight;
	iResolution = ((lpInfo->bmiHeader.biXPelsPerMeter) * 254L) / 10000L;

	// Make sure that we have something to do
	if ((iFrameXSize <= 0) || (iFrameYSize <= 0))
	{
		return(NULL);
	}

	// Validate the image resolution
	if (iResolution <= 0)
	{
		iResolution = 75;
	}

	// Create the new image frame
	lpFrame = FrameOpen(FDT_RGBCOLOR, iFrameXSize, iFrameYSize, iResolution);

	if (!lpFrame) return(NULL);

	// Find the DIB data pointer and width
	lpSrcLine = lpDIBMem + lpInfo->bmiHeader.biSize + 
		lpInfo->bmiHeader.biClrUsed * (sizeof(RGBQUAD));
	iSrcWidth = (((lpInfo->bmiHeader.biWidth * 3)+3)/4)*4;

	iDstWidth = FrameByteWidth( lpFrame );

	// Copy the DIB data into the Frame
	switch( lpInfo->bmiHeader.biBitCount )
	{
		case 16 :
			iSrcWidth = (((lpInfo->bmiHeader.biWidth * 2)+2)/4)*4;
			for(y=iFrameYSize-1;y >= 0;y--)
			{
				lpSrc = lpSrcLine;
				lpDst = FramePointer( lpFrame, 0, y, TRUE );
			
				iCount = iFrameXSize;
			
				while(iCount-- > 0)
				{
					word = *(LPWORD)lpSrc;
					*lpDst++ = MaxiB(word);
					*lpDst++ = MaxiG(word);
					*lpDst++ = MaxiR(word);
					lpSrc += 2;
				}
			
				lpSrcLine += iSrcWidth;
			}
		break;
		case 24 :
			iSrcWidth = (((lpInfo->bmiHeader.biWidth * 3)+3)/4)*4;
			for(y=iFrameYSize-1;y >= 0;y--)
			{
				lpSrc = lpSrcLine;
				lpDst = FramePointer( lpFrame, 0, y, TRUE );
			
				iCount = iFrameXSize;
			
				while(iCount-- > 0)
				{
					*lpDst++ = lpSrc[2];
					*lpDst++ = lpSrc[1];
					*lpDst++ = lpSrc[0];
					lpSrc += 3;
				}
			
				lpSrcLine += iSrcWidth;
			}
		break;
		default:
			FrameClose(lpFrame);
			return(NULL);
		break;
	}

	return(lpFrame);
}

/*=======================================================================*/

