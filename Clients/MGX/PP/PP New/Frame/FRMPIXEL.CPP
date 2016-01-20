/*=======================================================================*\

	FRMPIXEL.C - Frame Pixel Get/Set/Put routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include "mgxframe.h"
#include "frameint.h"

#define THRES 127

/*====================================================================*\
	Pixel <--> RGB Conversions
\*====================================================================*/

static long RGB2Pixel_1(LPFRAME lpFrame, LPRGB lpFullRGB)
{
	return(((TOLUM(lpFullRGB->red, lpFullRGB->green, lpFullRGB->blue)) > THRES) ? 0L : 1L);
}

/*====================================================================*/

static long RGB2Pixel_8(LPFRAME lpFrame, LPRGB lpFullRGB)
{
	return((long)(TOLUM(lpFullRGB->red, lpFullRGB->green, lpFullRGB->blue)));
}

/*====================================================================*/

static long RGB2Pixel_8P(LPFRAME lpFrame, LPRGB lpFullRGB)
{
	return((long)(TOLUM(lpFullRGB->red, lpFullRGB->green, lpFullRGB->blue)));
}

/*====================================================================*/

static long RGB2Pixel_24(LPFRAME lpFrame, LPRGB lpFullRGB)
{
	long lColor = 0;

	CopyRGB(lpFullRGB, &lColor);

	return(lColor);
}

/*====================================================================*/

static long RGB2Pixel_32(LPFRAME lpFrame, LPRGB lpFullRGB)
{
	CMYKS cmyk;
	long color;

	RGBtoCMYK( lpFullRGB->red, lpFullRGB->green, lpFullRGB->blue, &cmyk);

	CopyCMYK( &cmyk, &color );

	return(color);
}

/*====================================================================*/

static void Pixel2RGB_1( LPFRAME lpFrame, LPLONG lpColor, LPRGB lpRGB )
{
	lpRGB->red   =
	lpRGB->green =
	lpRGB->blue  = (*lpColor) ? 0x00 : 0xFF;
}

/*====================================================================*/

static void Pixel2RGB_8( LPFRAME lpFrame, LPLONG lpColor, LPRGB lpRGB )
{
	lpRGB->red   = ( BYTE )*lpColor;
	lpRGB->green = ( BYTE )*lpColor;
	lpRGB->blue  = ( BYTE )*lpColor;
}

/*====================================================================*/

static void Pixel2RGB_8P( LPFRAME lpFrame, LPLONG lpColor, LPRGB lpRGB )
{
	*lpRGB = lpFrame->ColorMap->RGBData[*lpColor];
}

/*====================================================================*/

static void Pixel2RGB_24( LPFRAME lpFrame, LPLONG lpColor, LPRGB lpRGB )
{
	CopyRGB(lpColor, lpRGB);
}

/*====================================================================*/

static void Pixel2RGB_32( LPFRAME lpFrame, LPLONG lpColor, LPRGB lpRGB )
{
	BYTE C, M, Y, K;

	// Get the color components
	C = ( BYTE )((*lpColor&0xFF000000L)>>24)^0xFF;
	M = ( BYTE )((*lpColor&0x00FF0000L)>>16)^0xFF;
	Y = ( BYTE )((*lpColor&0x0000FF00L)>>8)^0xFF;
	K = ( BYTE )((*lpColor&0x000000FFL));

	CMYKtoRGB( C, M, Y, K, lpRGB );
}


/*====================================================================*/

static void SetRGB_1(
	LPFRAME lpFrame, 
	LPRGB   lpRGBPixel, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	if (RGBtoL(lpRGBPixel) > THRES)
	{
		set(lpDst, (iPixelCount+7)/8, 0xFF);
	}
	else
	{
		set(lpDst, (iPixelCount+7)/8, 0x00);
	}
}

/*====================================================================*/

static void SetRGB_8(
	LPFRAME lpFrame, 
	LPRGB   lpRGBPixel, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	set(lpDst, iPixelCount, 
		TOLUM(lpRGBPixel->red, lpRGBPixel->green, lpRGBPixel->blue));
}

/*====================================================================*/

static void SetRGB_8P(
	LPFRAME lpFrame, 
	LPRGB   lpRGBPixel, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	BYTE Pixel;
	
	Pixel = FrameGetNearestIndex(lpFrame->ColorMap, lpRGBPixel);
	while(iPixelCount-- > 0)
	{
		*lpDst++ = Pixel;
	}
}

/*====================================================================*/

static void SetRGB_24(
	LPFRAME lpFrame, 
	LPRGB   lpRGBPixel, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	int iRed, iGreen, iBlue;

	iRed   = lpRGBPixel->red;
	iGreen = lpRGBPixel->green;
	iBlue  = lpRGBPixel->blue;

	while(iPixelCount-- > 0)
	{
		*lpDst++ = iRed;
		*lpDst++ = iGreen;
		*lpDst++ = iBlue;
	}
}

/*====================================================================*/

static void SetRGB_32(
	LPFRAME lpFrame, 
	LPRGB   lpRGBPixel, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	CMYKS cmyk;

	RGBtoCMYK( lpRGBPixel->red, lpRGBPixel->green, lpRGBPixel->blue, &cmyk);

	while(iPixelCount-- > 0)
	{
		*lpDst++ = cmyk.c;
		*lpDst++ = cmyk.m;
		*lpDst++ = cmyk.y;
		*lpDst++ = cmyk.k;
	}
}

/*====================================================================*\
	Get/Set/Put Pixel Routines...
\*====================================================================*/

static void GetPixel_1(
	LPFRAME lpFrame, 
	LPTR    lpSrc, 
	LPLONG  lpDstPixel, 
	int     iPixelCount)
{
	int iBitMask = 0x80;

	while(iPixelCount-- > 0)
	{
		if ((*lpSrc)&iBitMask)
		{
			*lpDstPixel++ = 1L;
		}
		else
		{
			*lpDstPixel++ = 0L;
		}

		if (iBitMask == 1)
		{
			iBitMask = 0x80;
			lpSrc++;
		}
		else
		{
			iBitMask>>=1;
		}
	}
}

/*====================================================================*/

static void GetPixel_8(
	LPFRAME lpFrame, 
	LPTR    lpSrc, 
	LPLONG  lpDstPixel, 
	int     iPixelCount)
{
	while(iPixelCount-- > 0)
	{
		*lpDstPixel++ = *lpSrc++;
	}
}

/*====================================================================*/

static void GetPixel_8P(
	LPFRAME lpFrame, 
	LPTR    lpSrc, 
	LPLONG  lpDstPixel, 
	int     iPixelCount)
{
	while(iPixelCount-- > 0)
	{
		*lpDstPixel++ = *lpSrc++;
	}
}

/*====================================================================*/

static void GetPixel_24(
	LPFRAME lpFrame, 
	LPTR    lpSrc, 
	LPLONG  lpDstPixel, 
	int     iPixelCount)
{
	while(iPixelCount-- > 0)
	{
		*lpDstPixel = 0;
		CopyRGB(lpSrc, lpDstPixel);

		lpDstPixel++;
		lpSrc+=3;
	}
}

/*====================================================================*/

static void GetPixel_32(
	LPFRAME lpFrame, 
	LPTR    lpSrc, 
	LPLONG  lpDstPixel, 
	int     iPixelCount)
{
	LONG lPixelValue;

	while(iPixelCount-- > 0)
	{
		lPixelValue  = *lpSrc++;
		lPixelValue <<= 8;

		lPixelValue |= *lpSrc++;
		lPixelValue <<= 8;

		lPixelValue |= *lpSrc++;
		lPixelValue <<= 8;

		lPixelValue |= *lpSrc++;

		*lpDstPixel++ = lPixelValue;
	}
}

/*====================================================================*/

static void PutPixel_1(
	LPFRAME lpFrame, 
	LPLONG  lpSrcPixel, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	int iPixelValue = 0;
	int iBitMask = 0x80;

	while(iPixelCount-- > 0)
	{
		if (*lpSrcPixel++)
		{
			iPixelValue |= iBitMask;
		}

		if (iBitMask == 1)
		{
			*lpDst++ = iPixelValue;
			iPixelValue = 0;
			iBitMask = 0x80;
		}
		else
		{
			iBitMask>>=1;
		}
	}

	if (iBitMask &0x7F)
		*lpDst++ = iPixelValue;
}

/*====================================================================*/

static void PutPixel_8(
	LPFRAME lpFrame, 
	LPLONG  lpSrcPixel, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	while( iPixelCount-- > 0)
	{
		*lpDst++ = ( BYTE )*lpSrcPixel++;
	}
}

/*====================================================================*/

static void PutPixel_8P(
	LPFRAME lpFrame, 
	LPLONG  lpSrcPixel, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	while( iPixelCount-- > 0)
	{
		*lpDst++ = ( BYTE )*lpSrcPixel++;
	}
}

/*====================================================================*/

static void PutPixel_24(
	LPFRAME lpFrame, 
	LPLONG  lpSrcPixel, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	while(iPixelCount--)
	{
		CopyRGB(lpSrcPixel, lpDst);

		lpSrcPixel++;
		lpDst+=3;
	}
}

/*====================================================================*/

static void PutPixel_32(
	LPFRAME lpFrame, 
	LPLONG  lpSrcPixel, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	long lPixelValue;

	lpDst += 3;

	while(iPixelCount--)
	{
		lPixelValue = (*lpSrcPixel++);

		*lpDst-- = (lPixelValue&0xFF);
		lPixelValue >>= 8;

		*lpDst-- = (lPixelValue&0xFF);
		lPixelValue >>= 8;

		*lpDst-- = (lPixelValue&0xFF);
		lPixelValue >>= 8;

		*lpDst-- = (lPixelValue&0xFF);

		lpDst+= 7;
	}
}

/*====================================================================*/

static void SetPixel_1(
	long    lPixel,
	LPTR    lpDst, 
	int     iPixelCount)
{
	set(lpDst, (iPixelCount+7)/8, (lPixel) ? 0xFF : 0x00);
}

/*====================================================================*/

static void SetPixel_8(
	long    lPixel,
	LPTR    lpDst, 
	int     iPixelCount)
{
	set(lpDst, iPixelCount, (int)(lPixel));
}

/*====================================================================*/

static void SetPixel_8P(
	long    lPixel,
	LPTR    lpDst, 
	int     iPixelCount)
{
	set(lpDst, iPixelCount, (int)(lPixel));
}

/*====================================================================*/

static void SetPixel_24(
	long    lPixel,
	LPTR    lpDst, 
	int     iPixelCount)
{
	RGBS rgb;
	int iRed, iGrn, iBlu;

	CopyRGB(&lPixel, &rgb);

	iRed = rgb.red;
	iGrn = rgb.green;
	iBlu = rgb.blue;

	while(iPixelCount-- > 0)
	{
		*lpDst++ = iRed;
		*lpDst++ = iGrn;
		*lpDst++ = iBlu;
	}
}

/*====================================================================*/

static void SetPixel_32(
	long    lPixel,
	LPTR    lpDst, 
	int     iPixelCount)
{
	int iCyn, iMag, iYel, iBlk;

	iCyn = (lPixel&0xFF);
	lPixel >>= 8;

	iMag = (lPixel&0xFF);
	lPixel >>= 8;

	iYel = (lPixel&0xFF);
	lPixel >>= 8;

	iBlk = (lPixel&0xFF);

	while(iPixelCount-- > 0)
	{
		*lpDst++ = iCyn;
		*lpDst++ = iMag;
		*lpDst++ = iYel;
		*lpDst++ = iBlk;
	}
}

/*====================================================================*\
	Jump Tables for Frame conversion functions...
\*====================================================================*/

// Table For FrameSample
STATICTABLE SAMPLETBLPROC lpSampleTable[] = 
{
	(SAMPLETBLPROC) Sample_8,   // FDT_LINEART
	(SAMPLETBLPROC) Sample_8,   // FDT_GRAYSCALE
	(SAMPLETBLPROC) Sample_8,   // FDT_PALETTECOLOR
	(SAMPLETBLPROC) Sample_24,  // FDT_RGBCOLOR
	(SAMPLETBLPROC) Sample_32   // FDT_CMYKCOLOR
};

// Table For FrameSampleMW
STATICTABLE SAMPLEMWTBLPROC lpSampleMWTable[] = 
{
	(SAMPLEMWTBLPROC) Sample_8MW,   // FDT_LINEART
	(SAMPLEMWTBLPROC) Sample_8MW,   // FDT_GRAYSCALE
	(SAMPLEMWTBLPROC) Sample_8MW,   // FDT_PALETTECOLOR
	(SAMPLEMWTBLPROC) Sample_24MW,  // FDT_RGBCOLOR
	(SAMPLEMWTBLPROC) Sample_32MW   // FDT_CMYKCOLOR
};

// Table For FrameRGB2Pixel
STATICTABLE RGB2PIXELTBLPROC lpRGB2PixelTable[] = 
{
	(RGB2PIXELTBLPROC) RGB2Pixel_8,    // FDT_LINEART
	(RGB2PIXELTBLPROC) RGB2Pixel_8,    // FDT_GRAYSCALE
	(RGB2PIXELTBLPROC) RGB2Pixel_8P,   // FDT_PALETTECOLOR
	(RGB2PIXELTBLPROC) RGB2Pixel_24,   // FDT_RGBCOLOR
	(RGB2PIXELTBLPROC) RGB2Pixel_32    // FDT_CMYKCOLOR
};

// Table For FramePixel2RGB
STATICTABLE PIXEL2RGBTBLPROC lpPixel2RGBTable[] = 
{
	(PIXEL2RGBTBLPROC) Pixel2RGB_8,  // FDT_LINEART
	(PIXEL2RGBTBLPROC) Pixel2RGB_8,  // FDT_GRAYSCALE
	(PIXEL2RGBTBLPROC) Pixel2RGB_8P, // FDT_PALETTECOLOR
	(PIXEL2RGBTBLPROC) Pixel2RGB_24, // FDT_RGBCOLOR
	(PIXEL2RGBTBLPROC) Pixel2RGB_32  // FDT_CMYKCOLOR
};

#ifdef UNUSED

// Table For FrameGetRGB
STATICTABLE GETRGBTBLPROC lpGetRGBTable[] = 
{
	(GETRGBTBLPROC) GetRGB_8,  // FDT_LINEART
	(GETRGBTBLPROC) GetRGB_8,  // FDT_GRAYSCALE
	(GETRGBTBLPROC) GetRGB_8P, // FDT_PALETTECOLOR
	(GETRGBTBLPROC) GetRGB_24, // FDT_RGBCOLOR
	(GETRGBTBLPROC) GetRGB_32  // FDT_CMYKCOLOR
};

// Table For FramePutRGB
STATICTABLE LVPROC lpPutRGBTable[] = 
{
	(LVPROC) PutRGB_8,  // FDT_LINEART
	(LVPROC) PutRGB_8,  // FDT_GRAYSCALE
	(LVPROC) PutRGB_8P, // FDT_PALETTECOLOR
	(LVPROC) PutRGB_24, // FDT_RGBCOLOR
	(LVPROC) PutRGB_32  // FDT_CMYKCOLOR
};

#endif // UNUSED

// Table For FrameSetRGB
STATICTABLE SETRGBTBLPROC lpSetRGBTable[] = 
{
	(SETRGBTBLPROC) SetRGB_8,  // FDT_LINEART
	(SETRGBTBLPROC) SetRGB_8,  // FDT_GRAYSCALE
	(SETRGBTBLPROC) SetRGB_8P, // FDT_PALETTECOLOR
	(SETRGBTBLPROC) SetRGB_24, // FDT_RGBCOLOR
	(SETRGBTBLPROC) SetRGB_32  // FDT_CMYKCOLOR
};

// Table For FrameGetPixel
STATICTABLE GETPIXELTBLPROC lpGetPixelTable[] = 
{
	(GETPIXELTBLPROC) GetPixel_8,  // FDT_LINEART
	(GETPIXELTBLPROC) GetPixel_8,  // FDT_GRAYSCALE
	(GETPIXELTBLPROC) GetPixel_8P, // FDT_PALETTECOLOR
	(GETPIXELTBLPROC) GetPixel_24, // FDT_RGBCOLOR
	(GETPIXELTBLPROC) GetPixel_32  // FDT_CMYKCOLOR
};

// Table For FramePutPixel
STATICTABLE PUTPIXELTBLPROC lpPutPixelTable[] = 
{
	(PUTPIXELTBLPROC) PutPixel_8,  // FDT_LINEART
	(PUTPIXELTBLPROC) PutPixel_8,  // FDT_GRAYSCALE
	(PUTPIXELTBLPROC) PutPixel_8P, // FDT_PALETTECOLOR
	(PUTPIXELTBLPROC) PutPixel_24, // FDT_RGBCOLOR
	(PUTPIXELTBLPROC) PutPixel_32  // FDT_CMYKCOLOR
};

// Table For FrameSetPixel
STATICTABLE SETPIXELTBLPROC lpSetPixelTable[] = 
{
	(SETPIXELTBLPROC) SetPixel_8,  // FDT_LINEART
	(SETPIXELTBLPROC) SetPixel_8,  // FDT_GRAYSCALE
	(SETPIXELTBLPROC) SetPixel_8P, // FDT_PALETTECOLOR
	(SETPIXELTBLPROC) SetPixel_24, // FDT_RGBCOLOR
	(SETPIXELTBLPROC) SetPixel_32  // FDT_CMYKCOLOR
};

/*====================================================================*/

void FrameSample( LPFRAME lpFrame, LPTR input, int inskip, LPTR output,
	                  int outskip, int count, LFIXED rate)
{
	SAMPLETBLPROC lpDoSample;

	if (!lpFrame) return;

	lpDoSample = lpSampleTable[lpFrame->DataType];

	(*lpDoSample)(input, inskip, output, outskip, count, rate );
}

/*====================================================================*/

void FrameSampleMW( LPFRAME lpFrame, LPTR input, int inskip, LPTR output,
	                     int outskip, int count, LFIXED rate, LPTR	mask )
{
	SAMPLEMWTBLPROC lpDoSampleMW;

	if (!lpFrame) return;

	lpDoSampleMW = lpSampleMWTable[lpFrame->DataType];

	(*lpDoSampleMW)(input, inskip, output, outskip, count, rate, mask);
}

/*====================================================================*/

void FrameRGB2Pixel(LPFRAME lpFrame, LPRGB lpFullRGB, LPLONG lpPixel)
{
	RGB2PIXELTBLPROC lpDoRGB2Pixel;

	if (!lpFrame) return;

	lpDoRGB2Pixel = lpRGB2PixelTable[lpFrame->DataType];

	*lpPixel = (*lpDoRGB2Pixel)(lpFrame, lpFullRGB);
}

/*====================================================================*/

void FramePixel2RGB( LPFRAME lpFrame, LPLONG lpPixel, LPRGB lpRGB )
{
	PIXEL2RGBTBLPROC lpDoPixel2RGB;

	if (!lpFrame) return;

	lpDoPixel2RGB = lpPixel2RGBTable[lpFrame->DataType];

	(*lpDoPixel2RGB)( lpFrame, lpPixel, lpRGB );
}

/*====================================================================*/

void FrameGetRGB( 
	LPFRAME lpFrame, 
	LPTR    lpSrc,
	LPRGB   lpDstRGB,
	int     iPixelCount)
{
	CFrameTypeConvert TypeConvert;
	FRMTYPEINFO SrcTypeInfo, DstTypeInfo;

	if (!lpFrame) return;

	FrameGetTypeInfo(lpFrame, &SrcTypeInfo);
	FrameSetTypeInfo(&DstTypeInfo, FDT_RGBCOLOR);

	if (!TypeConvert.Init(SrcTypeInfo, DstTypeInfo, iPixelCount, DT_NONE))
		return;

	TypeConvert.ConvertData(lpSrc, (LPTR)lpDstRGB, 0, iPixelCount);
}

/*====================================================================*/

void FramePutRGB(
	LPFRAME lpFrame,
	LPRGB   lpSrcRGB,
	LPTR    lpDst,
	int     iPixelCount)
{
	CFrameTypeConvert TypeConvert;
	FRMTYPEINFO SrcTypeInfo, DstTypeInfo;

	if (!lpFrame) return;

	FrameSetTypeInfo(&SrcTypeInfo, FDT_RGBCOLOR);
	FrameGetTypeInfo(lpFrame, &DstTypeInfo);

	if (!TypeConvert.Init(SrcTypeInfo, DstTypeInfo, iPixelCount, DT_NONE))
		return;

	TypeConvert.ConvertData((LPTR)lpSrcRGB, lpDst, 0, iPixelCount);
}

/*====================================================================*/

void FrameSetRGB( LPFRAME lpFrame, LPRGB lpRGBPixel, LPTR lpDst,int iPixelCount)
{
	SETRGBTBLPROC lpDoSetRGB;

	if (!lpFrame) return;

	lpDoSetRGB = lpSetRGBTable[lpFrame->DataType];

	(*lpDoSetRGB)( lpFrame, lpRGBPixel, lpDst, iPixelCount );
}

/*====================================================================*/

void FrameGetPixel( LPFRAME lpFrame, LPTR lpSrc, LPLONG lpDst, int iPixelCount)
{
	GETPIXELTBLPROC lpDoGetPixel;

	if (!lpFrame) return;

	lpDoGetPixel = lpGetPixelTable[lpFrame->DataType];

	(*lpDoGetPixel)( lpFrame, lpSrc, lpDst, iPixelCount );
}

/*====================================================================*/

void FramePutPixel( LPFRAME lpFrame, LPLONG lpSrc, LPTR lpDst,	int iPixelCount)
{
	PUTPIXELTBLPROC lpDoPutPixel;

	if (!lpFrame) return;

	lpDoPutPixel = lpPutPixelTable[lpFrame->DataType];

	(*lpDoPutPixel)( lpFrame, lpSrc, lpDst, iPixelCount );
}

/*====================================================================*/

void FrameSetPixel( LPFRAME lpFrame, long lPixel, LPTR lpDst, int iPixelCount)
{
	SETPIXELTBLPROC lpDoSetPixel;

	if (!lpFrame) return;

	lpDoSetPixel = lpSetPixelTable[lpFrame->DataType];

	(*lpDoSetPixel)( lPixel, lpDst, iPixelCount );
}

/*====================================================================*/

void FrameSetPixelEx(
	FRMDATATYPE DataType,
	long    lPixel,
	LPTR    lpDst,
	int     iPixelCount)
{
	SETPIXELTBLPROC lpDoSetPixel;

	lpDoSetPixel = lpSetPixelTable[DataType];

	(*lpDoSetPixel)( lPixel, lpDst, iPixelCount );
}

/*====================================================================*/

#ifdef UNUSED

/*====================================================================*\
	Get/Set/Put RGB Routines...
\*====================================================================*/

static void GetRGB_1(
	LPFRAME lpFrame, 
	LPTR    lpSrc, 
	LPRGB   lpDstRGB, 
	int     iPixelCount)
{
	FrameTypeConvert( 
		lpSrc, 
		FDT_LINEART, 
		NULL, 
		0, 
		(LPTR)lpDstRGB, 
		FDT_RGBCOLOR, 
		NULL, 
		iPixelCount);
}

/*====================================================================*/

static void GetRGB_8(
	LPFRAME lpFrame, 
	LPTR    lpSrc, 
	LPRGB   lpDstRGB, 
	int     iPixelCount)
{
	FrameTypeConvert( 
		lpSrc, 
		FDT_GRAYSCALE, 
		NULL, 
		0, 
		(LPTR)lpDstRGB, 
		FDT_RGBCOLOR, 
		NULL, 
		iPixelCount);
}

/*====================================================================*/

static void GetRGB_8P(
	LPFRAME lpFrame, 
	LPTR    lpSrc, 
	LPRGB   lpDstRGB, 
	int     iPixelCount)
{
	LPFRMCOLORMAP lpColorMap;

	lpColorMap = FrameGetColorMap( lpFrame );

	FrameTypeConvert( 
		lpSrc, 
		FDT_PALETTECOLOR, 
		lpColorMap, 
		0, 
		(LPTR)lpDstRGB, 
		FDT_RGBCOLOR, 
		NULL, 
		iPixelCount);
}

/*====================================================================*/

static void GetRGB_24(
	LPFRAME lpFrame, 
	LPTR    lpSrc, 
	LPRGB   lpDstRGB, 
	int     iPixelCount)
{
	FrameTypeConvert( 
		lpSrc, 
		FDT_RGBCOLOR,
		NULL,
		0, 
		(LPTR)lpDstRGB, 
		FDT_RGBCOLOR, 
		NULL, 
		iPixelCount);
}

/*====================================================================*/

static void GetRGB_32(
	LPFRAME lpFrame, 
	LPTR    lpSrc, 
	LPRGB   lpDstRGB, 
	int     iPixelCount)
{
	FrameTypeConvert( 
		lpSrc, 
		FDT_CMYKCOLOR,
		NULL,
		0, 
		(LPTR)lpDstRGB, 
		FDT_RGBCOLOR, 
		NULL, 
		iPixelCount);
}

/*====================================================================*/

static void PutRGB_1(
	LPFRAME lpFrame, 
	LPRGB   lpSrcRGB, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	FrameTypeConvert( 
		(LPTR)lpSrcRGB, 
		FDT_RGBCOLOR,
		NULL,
		0, 
		lpDst, 
		FDT_LINEART, 
		NULL, 
		iPixelCount);
}

/*====================================================================*/

static void PutRGB_8(
	LPFRAME lpFrame, 
	LPRGB   lpSrcRGB, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	FrameTypeConvert( 
		(LPTR)lpSrcRGB, 
		FDT_RGBCOLOR,
		NULL,
		0, 
		lpDst, 
		FDT_GRAYSCALE, 
		NULL, 
		iPixelCount);
}

/*====================================================================*/

static void PutRGB_8P(
	LPFRAME lpFrame, 
	LPRGB   lpSrcRGB, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	LPFRMCOLORMAP lpColorMap;

	lpColorMap = FrameGetColorMap( lpFrame );

	FrameTypeConvert( 
		(LPTR)lpSrcRGB, 
		FDT_RGBCOLOR,
		NULL,
		0, 
		lpDst, 
		FDT_PALETTECOLOR, 
		lpColorMap,
		iPixelCount);
}

/*====================================================================*/

static void PutRGB_24(
	LPFRAME lpFrame, 
	LPRGB   lpSrcRGB, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	FrameTypeConvert( 
		(LPTR)lpSrcRGB, 
		FDT_RGBCOLOR,
		NULL,
		0, 
		lpDst, 
		FDT_RGBCOLOR, 
		NULL, 
		iPixelCount);
}

/*====================================================================*/

static void PutRGB_32(
	LPFRAME lpFrame, 
	LPRGB   lpSrcRGB, 
	LPTR    lpDst, 
	int     iPixelCount)
{
	FrameTypeConvert( 
		(LPTR)lpSrcRGB, 
		FDT_RGBCOLOR,
		NULL,
		0, 
		lpDst, 
		FDT_RGBCOLOR, 
		NULL, 
		iPixelCount);
}

#endif // UNUSED
