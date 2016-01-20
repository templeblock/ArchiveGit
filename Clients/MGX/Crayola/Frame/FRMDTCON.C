/*=======================================================================*\

	FRMDTCON.C - Frame Data Type Conversion Routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include <windows.h>
#include "framelib.h"
#include "macros.h"

#define LINEART_THRESHOLD 127
#define MAXVALUE   255
#define THRESH		127

#define RGB_TO_GRAY(rgb) \
	(((rgb->red*38) + (rgb->green*76) + (rgb->blue*14))>>7)

/*=========================================================================*\
	Routine to create a new frame from an existing frame.
\*=========================================================================*/

LPFRAME FrameConvert( LPFRAME lpSrcFrame, FRMDATATYPE fdtDst,
	                     FRMCNVTCBPROC lpStatusCallback)
{
	FRMDATATYPE fdtSrc;
	LPCOLORMAP  lpSrcMap;
	LPFRAME     lpNewFrame;
	CFrameTypeConvert TypeConvert;
	FRMTYPEINFO SrcTypeInfo, DstTypeInfo;

	LPTR lpSrc, lpDst;

	int iFrameHeight;
	int iFrameWidth;
	int y;

	if (!lpSrcFrame) return(NULL);

	// Get the source frame information
	iFrameHeight = FrameYSize( lpSrcFrame );
	iFrameWidth  = FrameXSize( lpSrcFrame );
	lpSrcMap     = FrameGetColorMap( lpSrcFrame );
	fdtSrc       = FrameType( lpSrcFrame );

	// Create the new output frame
	lpNewFrame = FrameOpen(
		fdtDst,
		iFrameWidth,
		iFrameHeight,
		FrameResolution( lpSrcFrame ));

	if (!lpNewFrame) return(lpNewFrame);

	// Create a new palette if necessary
	if (fdtDst == FDT_PALETTECOLOR)
	{
		int i;
		LPCOLORMAP lpNewMap = FrameCreateColorMap();

		if (!lpNewMap)
		{
			FrameClose(lpNewFrame);
			return(NULL);
		}

		lpNewMap->NumEntries = 256;

		switch(fdtSrc)
		{
			case FDT_LINEART      :
			case FDT_GRAYSCALE    :
				// Create a grayscale palette

				for(i=0;i<256;i++)
				{
					lpNewMap->RGBData[i].red   = i;
					lpNewMap->RGBData[i].green = i;
					lpNewMap->RGBData[i].blue  = i;
				}
			break;

			case FDT_PALETTECOLOR :
			{
				if (lpSrcMap)
				{
					// Copy the original palette

					for(i=0;i<256;i++)
					{
						lpNewMap->RGBData[i].red   = lpSrcMap->RGBData[i].red;
						lpNewMap->RGBData[i].green = lpSrcMap->RGBData[i].green;
						lpNewMap->RGBData[i].blue  = lpSrcMap->RGBData[i].blue;
					}
					break;
				}
			} // Fall through if no original color map

			case FDT_RGBCOLOR     :
			case FDT_CMYKCOLOR    :
				FrameDestroyColorMap( lpNewMap );
				lpNewMap = FrameCreateFixedColorMap();
			break;
		}

		FrameSetColorMap( lpNewFrame, lpNewMap );

		FreeUp( (LPTR)lpNewMap );
	}

	// Get a pointer to the destination color map
	FrameGetTypeInfo(lpSrcFrame, &SrcTypeInfo);
	FrameGetTypeInfo(lpNewFrame, &DstTypeInfo);

	if (!TypeConvert.Init(SrcTypeInfo, DstTypeInfo, iFrameWidth))
	{
		FrameClose(lpNewFrame);
		return(NULL);
	}

	// Start of scanline loop
	for(y=0;y<iFrameHeight;y++)
	{
		if (lpStatusCallback)
		{
			(*lpStatusCallback)(y, iFrameHeight, NO);
		}

		// Get the data pointers
		lpSrc = FramePointerRaw( lpSrcFrame, 0, y, NO );
		lpDst = FramePointerRaw( lpNewFrame, 0, y, YES );

		// Call conversion routine
		TypeConvert.ConvertData(lpSrc, lpDst, y, iFrameWidth);
	}

	// Return the new frame pointer
	return(lpNewFrame);
}

// Constructor
CFrameTypeConvert::CFrameTypeConvert()
{
	lpPaletteLUT = NULL;
	lpErrorLUT = NULL;
	lpErrorLine = NULL;
}

// Destructor
CFrameTypeConvert::~CFrameTypeConvert()
{
	Reset();
}

// Reset function
void CFrameTypeConvert::Reset()
{
	if (lpPaletteLUT)
	{
		FreeUp(lpPaletteLUT);
		lpPaletteLUT = NULL;
	}
	if (lpErrorLUT)
	{
		FreeUp(lpErrorLUT);
		lpErrorLUT = NULL;
	}
	if (lpErrorLine)
	{
		FreeUp((LPTR)lpErrorLine);
		lpErrorLine = NULL;
	}
}

// Init function
CFrameTypeConvert::Init(FRMTYPEINFO SrcTypeInfo, FRMTYPEINFO DstTypeInfo,
						int iMaxConvertWidth, DITHER_TYPE DitherType)
{
	RGBS rgb;
	BOOL fNeedPaletteLUT, fNeedErrorLUT;
	long lNumInts;

	SrcType = SrcTypeInfo;
	DstType = DstTypeInfo;
	iWidth = iMaxConvertWidth;
	lNumInts = 0;
	fNeedPaletteLUT = fNeedErrorLUT = NO;

	if (SrcType.DataType != DstType.DataType)
	{
		// Force Source type to grayscale if no input colormap given
		if ((SrcType.DataType == FDT_PALETTECOLOR) && (SrcType.ColorMap == NULL))
			SrcType.DataType = FDT_GRAYSCALE;

		// Force Destination type to grayscale if no output colormap given
		if ((DstType.DataType == FDT_PALETTECOLOR) && (DstType.ColorMap == NULL))
			DstType.DataType = FDT_GRAYSCALE;
	}

	lpConvertProc = NULL;
	// Call the appropriate routine for conversion
	switch(SrcType.DataType)
	{
		case FDT_LINEART :
			switch(DstType.DataType)
			{
				case FDT_LINEART :
					lpConvertProc = CopyData;
					Depth = 0;
				break;

				case FDT_GRAYSCALE :
					lpConvertProc = (LPCONVERTPROC)LineArtToGrayScale;
				break;

				case FDT_PALETTECOLOR :
					lpConvertProc = (LPCONVERTPROC)LineArtToPaletteColor;
					rgb.red = rgb.green = rgb.blue = 0;
					Black = FrameGetNearestIndex(DstType.ColorMap, &rgb);
					rgb.red = rgb.green = rgb.blue = 255;
					White = FrameGetNearestIndex(DstType.ColorMap, &rgb);
				break;

				case FDT_RGBCOLOR :
					lpConvertProc = (LPCONVERTPROC)LineArtToRGBColor;
				break;

				case FDT_CMYKCOLOR :
					lpConvertProc = (LPCONVERTPROC)LineArtToCMYKColor;
				break;
			}
		break;

		case FDT_GRAYSCALE :
			switch(DstType.DataType)
			{
				case FDT_LINEART :
					if (DitherType == DT_SCATTER)
					{
						lNumInts = iWidth + 1;
						lpConvertProc = (LPCONVERTPROC)GrayScaleToScatter;
					}
					else
						lpConvertProc = (LPCONVERTPROC)GrayScaleToLineArt;
				break;

				case FDT_GRAYSCALE :
					lpConvertProc = (LPCONVERTPROC)CopyData;
					Depth = 1;
				break;

				case FDT_PALETTECOLOR :
					fNeedPaletteLUT = YES;
					if (DitherType == DT_SCATTER || DitherType == DT_DEFAULT)
					{
						lNumInts = (long)(iWidth+2) * 3L;
						lpConvertProc = (LPCONVERTPROC)GrayScaleToPaletteColorScatter;			
					}
					else
					if (DitherType == DT_PATTERN)
					{
						fNeedErrorLUT = YES;
						lpConvertProc = (LPCONVERTPROC)GrayScaleToPaletteColorPattern;			
					}
					else
						lpConvertProc = (LPCONVERTPROC)GrayScaleToPaletteColor;
				break;

				case FDT_RGBCOLOR :
					lpConvertProc = (LPCONVERTPROC)GrayScaleToRGBColor;
				break;

				case FDT_CMYKCOLOR :
					lpConvertProc = (LPCONVERTPROC)GrayScaleToCMYKColor;
				break;
			}
		break;

		case FDT_PALETTECOLOR :
			switch(DstType.DataType)
			{
				case FDT_LINEART :
					if (DitherType == DT_SCATTER)
					{
						lNumInts = iWidth + 1;
						lpConvertProc = (LPCONVERTPROC)PaletteColorToScatter;
					}
					else
						lpConvertProc = (LPCONVERTPROC)PaletteColorToLineArt;
				break;

				case FDT_GRAYSCALE :
					lpConvertProc = (LPCONVERTPROC)PaletteColorToGrayScale;
				break;

				case FDT_PALETTECOLOR :
					if (FrameColorMapsEqual(SrcType.ColorMap, DstType.ColorMap))
					{
						lpConvertProc = CopyData;
						Depth = 1;
					}
					else
					{
						fNeedPaletteLUT = YES;
						if (DitherType == DT_SCATTER || DitherType == DT_DEFAULT)
						{
							lNumInts = (long)(iWidth+2) * 3L;
							lpConvertProc = (LPCONVERTPROC)PaletteColorToPaletteColorScatter;	
						}
						else
						if (DitherType == DT_PATTERN)
						{
							fNeedErrorLUT = YES;
							lpConvertProc = (LPCONVERTPROC)PaletteColorToPaletteColorPattern;
						}
						else
							lpConvertProc = (LPCONVERTPROC)PaletteColorToPaletteColor;
					}
				break;

				case FDT_RGBCOLOR :
					lpConvertProc = (LPCONVERTPROC)PaletteColorToRGBColor;			
				break;

				case FDT_CMYKCOLOR :
					lpConvertProc = (LPCONVERTPROC)PaletteColorToCMYKColor;			
				break;
			}
		break;

		case FDT_RGBCOLOR :
			switch(DstType.DataType)
			{
				case FDT_LINEART :
					if (DitherType == DT_SCATTER)
					{
						lNumInts = iWidth + 2;
						lpConvertProc = (LPCONVERTPROC)RGBColorToScatter;			
					}
					else
						lpConvertProc = (LPCONVERTPROC)RGBColorToLineArt;			
				break;

				case FDT_GRAYSCALE :
					lpConvertProc = (LPCONVERTPROC)RGBColorToGrayScale;
				break;

				case FDT_PALETTECOLOR :
					fNeedPaletteLUT = YES;
					if (DitherType == DT_SCATTER || DitherType == DT_DEFAULT)
					{
						lNumInts = (long)(iWidth+2) * 3L;
						lpConvertProc = (LPCONVERTPROC)RGBColorToPaletteColorScatter;			
					}
					else
					if (DitherType == DT_PATTERN)
					{
						fNeedErrorLUT = YES;
						lpConvertProc = (LPCONVERTPROC)RGBColorToPaletteColorPattern;			
					}
					else
						lpConvertProc = (LPCONVERTPROC)RGBColorToPaletteColor;
				break;

				case FDT_RGBCOLOR :
 					lpConvertProc = (LPCONVERTPROC)CopyData;
					Depth = 3;
				break;

				case FDT_CMYKCOLOR :
					lpConvertProc = (LPCONVERTPROC)RGBColorToCMYKColor;
				break;
			}
		break;

		case FDT_CMYKCOLOR :
			switch(DstType.DataType)
			{
				case FDT_LINEART :
					if (DitherType == DT_SCATTER)
					{
						lNumInts = iWidth + 1;
						lpConvertProc = (LPCONVERTPROC)CMYKColorToScatter;			
					}
					else
						lpConvertProc = (LPCONVERTPROC)CMYKColorToLineArt;			
				break;

				case FDT_GRAYSCALE :
					lpConvertProc = (LPCONVERTPROC)CMYKColorToGrayScale;
				break;

				case FDT_PALETTECOLOR :
					fNeedPaletteLUT = YES;
					if (DitherType == DT_SCATTER || DitherType == DT_DEFAULT)
					{
						lNumInts = (long)(iWidth+2) * 3L;
						lpConvertProc = (LPCONVERTPROC)CMYKColorToPaletteColorScatter;			
					}
					else
					if (DitherType == DT_PATTERN)
					{
						fNeedErrorLUT = YES;
						lpConvertProc = (LPCONVERTPROC)CMYKColorToPaletteColorPattern;			
					}
					else
						lpConvertProc = (LPCONVERTPROC)CMYKColorToPaletteColor;
				break;

				case FDT_RGBCOLOR :
					lpConvertProc = (LPCONVERTPROC)CMYKColorToRGBColor;
				break;

				case FDT_CMYKCOLOR :
					lpConvertProc = (LPCONVERTPROC)CopyData;
					Depth = 4;
				break;
			}
		break;
	}
	if (lpConvertProc == NULL)
		return(FALSE);

	if (fNeedPaletteLUT)
	{
		lpPaletteLUT = CreatePaletteLut15(DstType.ColorMap->RGBData,
										DstType.ColorMap->NumEntries, NULL, NULL);
		if (!lpPaletteLUT)
		{
			Reset();
			return(FALSE);
		}
		if (fNeedErrorLUT)
		{
			lpErrorLUT = Alloc(32768L);
			if (!lpErrorLUT)
			{
				Reset();
				return(FALSE);
			}
			CreateErrorLut15(DstType.ColorMap->RGBData,
							lpPaletteLUT, lpErrorLUT, TRUE);
		}
	}
	if (lNumInts)
	{
		lpErrorLine = (LPINT16)Alloc(lNumInts * sizeof(INT16));
		if (!lpErrorLine)
		{
			Reset();
			return(FALSE);
		}
		hclr((HPTR)lpErrorLine, lNumInts * sizeof(INT16));
	}
	return(TRUE);
}

/*=========================================================================*/

void CFrameTypeConvert::ConvertData(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	(this->*lpConvertProc)(lpSrc, lpDst, y, iCount);
}

/*=========================================================================*/

void CFrameTypeConvert::CopyData(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	DWORD dwCount;

	if (Depth)
		dwCount = (DWORD)iCount * Depth;
	else
		dwCount = (iCount + 7) / 8;
	hcopy(lpSrc, lpDst, dwCount);
}

/*=========================================================================*/
// from FDT_LINEART convertors
/*=========================================================================*/

void CFrameTypeConvert::LineArtToGrayScale(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
#ifdef C_CODE
	int iValue;
	int iMask = 1<<7;

	iValue = *lpSrc++;

	while(iCount-- > 0)
	{
		*lpDst++ = (iValue & iMask) ? 0xFF : 0x00;

		if (iMask&1)
		{
			iMask = 1<<7;
			iValue = *lpSrc++;
		}
		else
		{
			iMask >>= 1;
		}
	}
#else // !C_CODE
__asm {
	push	ds
	mov     cx, iCount
	or      cx, cx
	je      la2con_end
	lds     si, lpSrc
	les     di, lpDst
	cld
	mov     bl, 1                   ;bl = bit
	mov     dl, 0                   ;dl = black
	mov     dh, 255                 ;dh = white

la2con_loop:
	dec     bl                      ;bit
	jne     la2con_2
	lodsb                           ;ax=*lpIn++
	mov     bh, al                  ;bh=byte
	mov     bl, 8                   ;bl=bit=8
la2con_2:
	cmp     bh, 127                 ;byte
	jbe     la2con_3
	mov     al, dh                  ;white
	jmp     la2con_4
la2con_3:
	mov     al, dl                  ;black
la2con_4:
	stosb
	shl     bh, 1                   ;byte
	loop    la2con_loop
la2con_end:
	pop		ds
	}
#endif // !C_CODE
}

/*=========================================================================*/

void CFrameTypeConvert::LineArtToPaletteColor(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	int iValue;
	int iMask = 1<<7;

	iValue = *lpSrc++;

	while(iCount-- > 0)
	{
		*lpDst++ = (iValue & iMask) ? White : Black;

		if (iMask&1)
		{
			iMask = 1<<7;
			iValue = *lpSrc++;
		}
		else
		{
			iMask >>= 1;
		}
	}
}

/*=========================================================================*/

void CFrameTypeConvert::LineArtToRGBColor(
	LPTR lpSrc,
	LPRGB lpDst,
	int y,
	int iCount)
{
	RGBS white, black;
	int iValue;
	int iMask = 1<<7;

	iValue = *lpSrc++;

	white.red   = 255;
	white.green = 255;
	white.blue  = 255;

	black.red   = 0;
	black.green = 0;
	black.blue  = 0;

	while(iCount-- > 0)
	{
		if (iValue & iMask)
			*lpDst++ = white;
		else
			*lpDst++ = black;

		if (iMask&1)
		{
			iMask = 1<<7;
			iValue = *lpSrc++;
		}
		else
		{
			iMask >>= 1;
		}
	}
}

/*=========================================================================*/

void CFrameTypeConvert::LineArtToCMYKColor(
	LPTR lpSrc,
	LPCMYK lpDst,
	int y,
	int iCount)
{
	CMYKS black, white;
	int iValue;
	int iMask = 1<<7;

	iValue = *lpSrc++;

	RGBtoCMYK(   0,   0,   0, &black );
	RGBtoCMYK( 255, 255, 255, &white );

	while(iCount-- > 0)
	{
		if (iValue & iMask)
		{
			*lpDst++ = white;
		}
		else
		{
			*lpDst++ = black;
		}

		if (iMask&1)
		{
			iMask = 1<<7;
			iValue = *lpSrc++;
		}
		else
		{
			iMask >>= 1;
		}
	}
}

/*=========================================================================*/
// from FDT_GRAYSCALE convertors
/*=========================================================================*/

void CFrameTypeConvert::GrayScaleToScatter(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	BYTE  byte, bit;
	LPINT16 lpError;
	int   pixel;
	static ULONG lSeed = 0xDEADBABA;

	lpError = lpErrorLine;

	byte = 0;
	bit = 128;

	while (--iCount > 0)
	{
		pixel = *lpSrc++;

		if ( (pixel += *lpError) > THRESH )
			{
			byte |= bit;
			pixel -= MAXVALUE;
			}

		if ( lSeed & BIT18 )
		{
			lSeed += lSeed;
			lSeed ^= BITMASK;
			*lpError++ = 0;
			*lpError  += pixel;
		}
		else
		{
			lSeed += lSeed;
			*lpError++ = pixel;
		}

		if ( (bit >>= 1) == 0 )
		{
			*lpDst++ = byte;
			byte = 0;
			bit = 128;
		}
	}

	if ( bit != 128 )
		*lpDst++ = byte;
}

/*=========================================================================*/

void CFrameTypeConvert::GrayScaleToLineArt(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
#ifdef C_CODE
	int iMask = 1<<7;
	int iValue;

	iValue = 0;

	while(iCount-- > 0)
	{
		if (*lpSrc++ > THRESH)
		{
			iValue |= iMask;
		}

		if (iMask&1)
		{
			*lpDst++ = iValue;
			iValue = 0;
			iMask = 1<<7;
		}
		else
		{
			iMask >>= 1;
		}
	}

	if (iMask != 1<<7)
		*lpDst++ = iValue;

#else // !C_CODE
__asm {
	push	ds
	mov     cx, iCount
	or      cx, cx
	je      con2la_end
	lds     si, lpSrc
	les     di, lpDst
	cld
	mov     bl, 128                 ; bit
	mov     bh, 0                   ; byte

con2la_loop:
	lodsb                           ; al = *lpIn++
	cmp     al, 127
	jbe     con2la_1
	or      bh, bl                  ; byte |= bit
con2la_1:
	shr     bl, 1                   ;bit
	jne     con2la_2
	mov     al, bh
	stosb
	mov     bl, 128                 ;bit
	mov     bh, 0                   ;byte
con2la_2:
	loop    con2la_loop

	cmp     bl, 128                 ;bit
	je      con2la_end
	mov     al, bh
	stosb
con2la_end:
	pop		ds
	}
#endif // !C_CODE
}

/*=========================================================================*/

void CFrameTypeConvert::GrayScaleToPaletteColor(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	WORD wRGB;

	while (--iCount >= 0)
	{
		wRGB = GraytoMiniRGB(*lpSrc);
		++lpSrc;
		*lpDst++ = lpPaletteLUT[wRGB];
	}
}

/*=========================================================================*/

void CFrameTypeConvert::GrayScaleToPaletteColorPattern(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	WORD wRGB;
	int x = 0;

	while (--iCount >= 0)
	{
		wRGB = GraytoMiniRGB(*lpSrc);
		++lpSrc;
		if ((x ^ y) & 1)
			*lpDst++ = lpErrorLUT[wRGB];
		else
			*lpDst++ = lpPaletteLUT[wRGB];
		++x;
	}
}

/*=========================================================================*/

void CFrameTypeConvert::GrayScaleToPaletteColorScatter(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	INT16 HUGE *lpError;
	INT16 curError;
	INT16 r, g, b;
	WORD wRGB;
	LPRGB lpRGBmap, lpRGB;
	BYTE Pixel;
	RGBS rgb;

	lpRGBmap = DstType.ColorMap->RGBData;
	lpError = lpErrorLine+3;

	while (--iCount >= 0)
	{
		rgb.red = rgb.green = rgb.blue = *lpSrc++;

		// Get Red Value with error and bound
		r = ((INT16)rgb.red + lpError[0]);
		r = Bound(r, 0, 255);
		// Get Green Value with error and bound
		g = ((INT16)rgb.green + lpError[1]);
		g = Bound(g, 0, 255);
		// Get Blue Value with error and bound
		b = ((INT16)rgb.blue + lpError[2]);
		b = Bound(b, 0, 255);
		// lookup value in palette LUT
		wRGB = RGB3toMiniRGB(r, g, b);
		Pixel = lpPaletteLUT[wRGB];

		// store pixel value and bump pointers
		*lpDst++ = Pixel;

		// get color that this pixel corresponds to
		// and calculate errors
		lpRGB = lpRGBmap + Pixel;
		
		// Propogate Error left, right and down
		curError = r - (INT16)lpRGB->red;
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		// Propogate Error left, right and down
		curError = g - (INT16)lpRGB->green;
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		// Propogate error left, right and down
		curError = b - (INT16)lpRGB->blue;
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;
	}
}

/*=========================================================================*/

void CFrameTypeConvert::GrayScaleToRGBColor(
	LPTR lpSrc,
	LPRGB lpDst,
	int y,
	int iCount)
{
#ifdef C_CODE
	int iValue;

	while(iCount-- > 0)
	{
		iValue = *lpSrc++;
		lpDst->red = iValue;
		lpDst->green = iValue;
		lpDst->blue = iValue;
		++lpDst;
	}
#else // !C_CODE
__asm {
		push	ds
		les	di, lpDst	; Setup Destination
		lds	si, lpSrc	; Setup Source
		mov	cx, iCount	; get count
		cld
gr2rgb:
		lodsb			; Get Source Gray
		stosb			; Put Dest Red
		stosb			; Put Dest Green
		stosb			; Put Dest Blue
		dec	cx
		jnz	gr2rgb
		pop	ds
	}
#endif // !C_CODE
}

/*=========================================================================*/

void CFrameTypeConvert::GrayScaleToCMYKColor(
	LPTR lpSrc,
	LPCMYK lpDst,
	int y,
	int iCount)
{
	int iValue;

	while(iCount-- > 0)
	{
		iValue = *lpSrc++;
		RGBtoCMYK( (BYTE)iValue, (BYTE)iValue, (BYTE)iValue, lpDst );
		lpDst++;
	}
}

/*=========================================================================*/
// from FDT_PALETTECOLOR convertors
/*=========================================================================*/

void CFrameTypeConvert::PaletteColorToScatter(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	BYTE  byte, bit;
	LPINT16 lpError;
	int   pixel;
	static ULONG lSeed = 0xDEADBABA;
	LPRGB lpRGB, lpRGBmap;
	BYTE i;

	lpError = lpErrorLine;

	byte = 0;
	bit = 128;

	lpRGBmap = SrcType.ColorMap->RGBData;
	while (--iCount > 0)
	{
		i = *lpSrc++;
		lpRGB = lpRGBmap+i;
		pixel = RGB_TO_GRAY(lpRGB);

		if ( (pixel += *lpError) > THRESH )
			{
			byte |= bit;
			pixel -= MAXVALUE;
			}

		if ( lSeed & BIT18 )
		{
			lSeed += lSeed;
			lSeed ^= BITMASK;
			*lpError++ = 0;
			*lpError  += pixel;
		}
		else
		{
			lSeed += lSeed;
			*lpError++ = pixel;
		}

		if ( (bit >>= 1) == 0 )
		{
			*lpDst++ = byte;
			byte = 0;
			bit = 128;
		}
	}

	if ( bit != 128 )
		*lpDst++ = byte;
}

/*=========================================================================*/

void CFrameTypeConvert::PaletteColorToLineArt(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	int iValue, iMask;
	BYTE gray;
	LPRGB lpRGB, lpRGBmap;
	BYTE i;

	iValue = 0;
	iMask = 1<<7;

	lpRGBmap = SrcType.ColorMap->RGBData;
	while(iCount-- > 0)
	{
		i = *lpSrc++;
		lpRGB = lpRGBmap+i;
		gray = RGB_TO_GRAY(lpRGB);

		if (gray > THRESH)
		{
			iValue |= iMask;
		}

		if (iMask & 1)
		{
			*lpDst++ = iValue;
			iValue = 0;
			iMask = 1<<7;
		}
		else
		{
			iMask >>= 1;
		}
	}

	if (iMask != 1<<7)
		*lpDst++ = iValue;
}

/*=========================================================================*/

void CFrameTypeConvert::PaletteColorToGrayScale(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	LPRGB lpRGB, lpRGBmap;
	BYTE i;
	
	lpRGBmap = SrcType.ColorMap->RGBData;
	while (--iCount >= 0)
	{
		i = *lpSrc++;
		lpRGB = lpRGBmap+i;
		*lpDst++ = RGB_TO_GRAY(lpRGB);
	}
}

/*=========================================================================*/

void CFrameTypeConvert::PaletteColorToPaletteColor(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	WORD wRGB;
	LPRGB lpRGB, lpRGBmap;
	BYTE i;
	
	lpRGBmap = SrcType.ColorMap->RGBData;
	while (--iCount >= 0)
	{
		i = *lpSrc++;
		lpRGB = lpRGBmap+i;
		wRGB = RGBtoMiniRGB(lpRGB);
		*lpDst++ = lpPaletteLUT[wRGB];
	}
}

/*=========================================================================*/

void CFrameTypeConvert::PaletteColorToPaletteColorPattern(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	WORD wRGB;
	LPRGB lpRGB, lpRGBmap;
	BYTE i;
	int x = 0;
	
	lpRGBmap = SrcType.ColorMap->RGBData;
	while (--iCount >= 0)
	{
		i = *lpSrc++;
		lpRGB = lpRGBmap+i;
		wRGB = RGBtoMiniRGB(lpRGB);
		if ((x ^ y) & 1)
			*lpDst++ = lpErrorLUT[wRGB];
		else
			*lpDst++ = lpPaletteLUT[wRGB];
		++x;
	}
}

/*=========================================================================*/

void CFrameTypeConvert::PaletteColorToPaletteColorScatter(
	LPTR lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	INT16 HUGE *lpError;
	INT16 curError;
	INT16 r, g, b;
	WORD wRGB;
	LPRGB lpDstMap, lpSrcMap, lpRGB;
	BYTE Pixel;

	lpSrcMap = SrcType.ColorMap->RGBData;
	lpDstMap = DstType.ColorMap->RGBData;
	lpError = lpErrorLine+3;

	while (--iCount >= 0)
	{
		Pixel = *lpSrc++;
		lpRGB = lpSrcMap+Pixel;

		// Get Red Value with error and bound
		r = ((INT16)lpRGB->red + lpError[0]);
		r = Bound(r, 0, 255);
		// Get Green Value with error and bound
		g = ((INT16)lpRGB->green + lpError[1]);
		g = Bound(g, 0, 255);
		// Get Blue Value with error and bound
		b = ((INT16)lpRGB->blue + lpError[2]);
		b = Bound(b, 0, 255);
		// lookup value in palette LUT
		wRGB = RGB3toMiniRGB(r, g, b);
		Pixel = lpPaletteLUT[wRGB];

		// store pixel value and bump pointers
		*lpDst++ = Pixel;

		// get color that this pixel corresponds to
		// and calculate errors
		lpRGB = lpDstMap + Pixel;
		
		// Propogate Error left, right and down
		curError = r - (INT16)lpRGB->red;
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		// Propogate Error left, right and down
		curError = g - (INT16)lpRGB->green;
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		// Propogate error left, right and down
		curError = b - (INT16)lpRGB->blue;
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;
	}
}

/*=========================================================================*/

void CFrameTypeConvert::PaletteColorToRGBColor(
	LPTR lpSrc,
	LPRGB lpDst,
	int y,
	int iCount)
{
	LPRGB lpRGBmap;

	lpRGBmap = SrcType.ColorMap->RGBData;
	while(iCount-- > 0)
	{
		*lpDst++ = lpRGBmap[*lpSrc++];
	}
}

/*=========================================================================*/

void CFrameTypeConvert::PaletteColorToCMYKColor(
	LPTR lpSrc,
	LPCMYK lpDst,
	int y,
	int iCount)
{
	LPRGB  lpRGBmap, lpSrcRGB;

	lpRGBmap = SrcType.ColorMap->RGBData;
	while(iCount-- > 0)
	{
		lpSrcRGB = &lpRGBmap[*lpSrc++];

		RGBtoCMYK( lpSrcRGB->red, lpSrcRGB->green, lpSrcRGB->blue, lpDst );

		lpDst++;
	}
}

/*=========================================================================*/
// from FDT_RGBCOLOR convertors
/*=========================================================================*/
void CFrameTypeConvert::RGBColorToScatter(
	LPRGB lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	BYTE  byte, bit;
	LPINT16 lpError;
	int   pixel;
	static ULONG lSeed = 0xDEADBABA;

	byte = 0;
	bit = 128;
	lpError = lpErrorLine;
	while (--iCount > 0)
	{
		pixel = RGB_TO_GRAY(lpSrc);
		++lpSrc;

		if ( (pixel += *lpError) > THRESH )
			{
			byte |= bit;
			pixel -= MAXVALUE;
			}

		if ( lSeed & BIT18 )
		{
			lSeed += lSeed;
			lSeed ^= BITMASK;
			*lpError++ = 0;
			*lpError  += pixel;
		}
		else
		{
			lSeed += lSeed;
			*lpError++ = pixel;
		}

		if ( (bit >>= 1) == 0 )
		{
			*lpDst++ = byte;
			byte = 0;
			bit = 128;
		}
	}

	if ( bit != 128 )
		*lpDst = byte;
}

#ifdef DIDNT_WORK_TOO_WELL
void CFrameTypeConvert::RGBColorToScatter(
	LPRGB lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	BYTE  byte, bit;
	LPINT16 lpError;
	int   pixel;
	static ULONG lSeed = 0xDEADBABA;

	byte = 0;
	bit = 128;
	lpError = lpErrorLine+1;
	while (--iCount > 0)
	{
		pixel = RGB_TO_GRAY(lpSrc);
		++lpSrc;

		if ( (pixel += *lpError) > THRESH )
			{
			byte |= bit;
			pixel -= MAXVALUE;
			}

		// Propogate Error left, right and down
		lpError[0]   = (pixel*26)/64;
		lpError[-1] += (pixel*19)/64;
		lpError[1]  += (pixel*19)/64;
		lpError++;

		if ( (bit >>= 1) == 0 )
		{
			*lpDst++ = byte;
			byte = 0;
			bit = 128;
		}
	}

	if ( bit != 128 )
		*lpDst = byte;
}
#endif

/*=========================================================================*/

void CFrameTypeConvert::RGBColorToLineArt(
	LPRGB lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	int iValue, iMask;

	iValue = 0;
	iMask = 1<<7;

	while(iCount-- > 0)
	{
		if (RGB_TO_GRAY(lpSrc) > LINEART_THRESHOLD)
		{
			iValue |= iMask;
		}

		lpSrc++;

		if (iMask & 1)
		{
			*lpDst++ = iValue;
			iValue = 0;
			iMask = 1<<7;
		}
		else
		{
			iMask >>= 1;
		}
	}

	if (iMask != (1<<7))
	{
		*lpDst++ = iValue;
	}
}

/*=========================================================================*/

void CFrameTypeConvert::RGBColorToGrayScale(
	LPRGB lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	while(iCount-- > 0)
	{
		*lpDst++ = RGB_TO_GRAY(lpSrc);

		lpSrc++;
	}
}

/*=========================================================================*/

void CFrameTypeConvert::RGBColorToPaletteColor(
	LPRGB lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	WORD wRGB;

	while (--iCount >= 0)
	{
		wRGB = RGBtoMiniRGB(lpSrc);
		++lpSrc;
		*lpDst++ = lpPaletteLUT[wRGB];
	}
}

/*=========================================================================*/

void CFrameTypeConvert::RGBColorToPaletteColorPattern(
	LPRGB lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	WORD wRGB;
	int x = 0;

	while (--iCount >= 0)
	{
		wRGB = RGBtoMiniRGB(lpSrc);
		++lpSrc;
		if ((x ^ y) & 1)
			*lpDst++ = lpErrorLUT[wRGB];
		else
			*lpDst++ = lpPaletteLUT[wRGB];
		++x;
	}
}

/*=========================================================================*/

void CFrameTypeConvert::RGBColorToPaletteColorScatter(
	LPRGB lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	INT16 HUGE *lpError;
	INT16 curError;
	INT16 r, g, b;
	WORD wRGB;
	LPRGB lpRGBmap, lpRGB;
	BYTE Pixel;

	lpRGBmap = DstType.ColorMap->RGBData;
	lpError = lpErrorLine+3;

	while( iCount-- > 0 )
	{
		// Get Red Value with error and bound
		r = ((INT16)lpSrc->red + lpError[0]);
		r = Bound(r, 0, 255);
		// Get Green Value with error and bound
		g = ((INT16)lpSrc->green + lpError[1]);
		g = Bound(g, 0, 255);
		// Get Blue Value with error and bound
		b = ((INT16)lpSrc->blue + lpError[2]);
		b = Bound(b, 0, 255);
		// lookup value in palette LUT
		wRGB = RGB3toMiniRGB(r, g, b);
		Pixel = lpPaletteLUT[wRGB];

		// store pixel value and bump pointers
		*lpDst++ = Pixel;
		++lpSrc;

		// get color that this pixel corresponds to
		// and calculate errors
		lpRGB = lpRGBmap + Pixel;
		
		// Propogate Error left, right and down
		curError = r - (INT16)lpRGB->red;
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		// Propogate Error left, right and down
		curError = g - (INT16)lpRGB->green;
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		// Propogate error left, right and down
		curError = b - (INT16)lpRGB->blue;
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;
	}
}

/*=========================================================================*/

void CFrameTypeConvert::RGBColorToCMYKColor(
	LPRGB lpSrc,
	LPCMYK lpDst,
	int y,
	int iCount)
{
	RGBtoCMYKScanline( lpSrc, lpDst, iCount );
}

/*=========================================================================*/

void CFrameTypeConvert::CMYKColorToScatter(
	LPCMYK lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	BYTE  byte, bit;
	LPINT16 lpError;
	int   pixel;
	static ULONG lSeed = 0xDEADBABA;

	lpError = lpErrorLine;

	byte = 0;
	bit = 128;

	while (--iCount > 0)
	{
		pixel = CMYKtoL(lpSrc);
		++lpSrc;

		if ( (pixel += *lpError) > THRESH )
			{
			byte |= bit;
			pixel -= MAXVALUE;
			}

		if ( lSeed & BIT18 )
		{
			lSeed += lSeed;
			lSeed ^= BITMASK;
			*lpError++ = 0;
			*lpError  += pixel;
		}
		else
		{
			lSeed += lSeed;
			*lpError++ = pixel;
		}

		if ( (bit >>= 1) == 0 )
		{
			*lpDst++ = byte;
			byte = 0;
			bit = 128;
		}
	}

	if ( bit != 128 )
		*lpDst++ = byte;
}

/*=========================================================================*/

void CFrameTypeConvert::CMYKColorToLineArt(
	LPCMYK lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	int iValue, iMask;

	iValue = 0;
	iMask = 1<<7;

	while(iCount-- > 0)
	{
		if (CMYKtoL(lpSrc) > LINEART_THRESHOLD)
		{
			iValue |= iMask;
		}

		lpSrc++;

		if (iMask & 1)
		{
			*lpDst++ = iValue;
			iValue = 0;
			iMask = 1<<7;
		}
		else
		{
			iMask >>= 1;
		}
	}

	if (iMask != (1<<7))
	{
		*lpDst++ = iValue;
	}
}

/*=========================================================================*/

void CFrameTypeConvert::CMYKColorToGrayScale(
	LPCMYK lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	while(iCount-- > 0)
	{
		*lpDst++ = CMYKtoL(lpSrc);
		lpSrc++;
	}
}

/*=========================================================================*/

void CFrameTypeConvert::CMYKColorToPaletteColor(
	LPCMYK lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	RGBS rgb;
	WORD wRGB;

	while (--iCount >= 0)
	{
		CMYKtoRGB( lpSrc->c, lpSrc->m, lpSrc->y, lpSrc->k, &rgb);
		lpSrc++;
		wRGB = RGBtoMiniRGB(&rgb);
		*lpDst++ = lpPaletteLUT[wRGB];
	}
}

/*=========================================================================*/

void CFrameTypeConvert::CMYKColorToPaletteColorPattern(
	LPCMYK lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	RGBS rgb;
	WORD wRGB;
	int x = 0;

	while (--iCount >= 0)
	{
		CMYKtoRGB( lpSrc->c, lpSrc->m, lpSrc->y, lpSrc->k, &rgb);
		lpSrc++;
		wRGB = RGBtoMiniRGB(&rgb);
		if ((x ^ y) & 1)
			*lpDst++ = lpErrorLUT[wRGB];
		else
			*lpDst++ = lpPaletteLUT[wRGB];
		++x;
	}
}

/*=========================================================================*/

void CFrameTypeConvert::CMYKColorToPaletteColorScatter(
	LPCMYK lpSrc,
	LPTR lpDst,
	int y,
	int iCount)
{
	INT16 HUGE *lpError;
	INT16 curError;
	INT16 r, g, b;
	WORD wRGB;
	LPRGB lpRGBmap, lpRGB;
	BYTE Pixel;
	RGBS rgb;

	lpRGBmap = DstType.ColorMap->RGBData;
	lpError = lpErrorLine+3;

	while (--iCount >= 0)
	{
		CMYKtoRGB( lpSrc->c, lpSrc->m, lpSrc->y, lpSrc->k, &rgb);
		lpSrc++;

		// Get Red Value with error and bound
		r = ((INT16)rgb.red + lpError[0]);
		r = Bound(r, 0, 255);
		// Get Green Value with error and bound
		g = ((INT16)rgb.green + lpError[1]);
		g = Bound(g, 0, 255);
		// Get Blue Value with error and bound
		b = ((INT16)rgb.blue + lpError[2]);
		b = Bound(b, 0, 255);
		// lookup value in palette LUT
		wRGB = RGB3toMiniRGB(r, g, b);
		Pixel = lpPaletteLUT[wRGB];

		// store pixel value and bump pointers
		*lpDst++ = Pixel;

		// get color that this pixel corresponds to
		// and calculate errors
		lpRGB = lpRGBmap + Pixel;
		
		// Propogate Error left, right and down
		curError = r - (INT16)lpRGB->red;
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		// Propogate Error left, right and down
		curError = g - (INT16)lpRGB->green;
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		// Propogate error left, right and down
		curError = b - (INT16)lpRGB->blue;
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;
	}
}

/*=========================================================================*/

void CFrameTypeConvert::CMYKColorToRGBColor(
	LPCMYK lpSrc,
	LPRGB lpDst,
	int y,
	int iCount)
{
	CMYKtoRGBScanline( lpSrc, lpDst, iCount );
}

/*=========================================================================*/

void LineArtToGrayScale(
	LPTR lpSrc, 
	LPTR lpDst,
	int  iCount)
{
#ifdef C_CODE
	int iValue;
	int iMask = 1<<7;

	iValue = *lpSrc++;

	while(iCount-- > 0)
	{
		*lpDst++ = (iValue & iMask) ? 0xFF : 0x00;

		if (iMask&1)
		{
			iMask = 1<<7;
			iValue = *lpSrc++;
		}
		else
		{
			iMask >>= 1;
		}
	}
#else // !C_CODE
__asm {
	push	ds
	mov     cx, iCount
	or      cx, cx
	je      la2con_end
	lds     si, lpSrc
	les     di, lpDst
	cld
	mov     bl, 1                   ;bl = bit
	mov     dl, 0                   ;dl = black
	mov     dh, 255                 ;dh = white

la2con_loop:
	dec     bl                      ;bit
	jne     la2con_2
	lodsb                           ;ax=*lpIn++
	mov     bh, al                  ;bh=byte
	mov     bl, 8                   ;bl=bit=8
la2con_2:
	cmp     bh, 127                 ;byte
	jbe     la2con_3
	mov     al, dh                  ;white
	jmp     la2con_4
la2con_3:
	mov     al, dl                  ;black
la2con_4:
	stosb
	shl     bh, 1                   ;byte
	loop    la2con_loop
la2con_end:
	pop		ds
	}
#endif // !C_CODE
}

#ifdef UNUSED
/*=========================================================================*/

static LPCOLORMAP lpLastColorMap = NULL;
static int iLastBlack = -1;
static int iLastWhite = -1;

static void FindBlackWhite(
	LPCOLORMAP lpColorMap,
	int FAR *  lpiBlack,
	int FAR *  lpiWhite)
{
	LPRGB lpRGB;
	int i;
	int iBlack, iWhite;
	int iMinIndex, iMinLum;
	int iMaxIndex, iMaxLum;

	if (lpColorMap == lpLastColorMap)
	{
		*lpiBlack = iLastBlack;
		*lpiWhite = iLastWhite;
		return;
	}

	iBlack = -1;
	iWhite = -1;

	iMinIndex = 0;
	iMaxIndex = 0;
	iMinLum = 255;
	iMaxLum = 0;

	lpRGB = lpColorMap->RGBData;

	i = 0;

	while(((iBlack == -1) || (iWhite == -1)) && (i <= 255))
	{
		if (iBlack < 0)
		{
			if ((lpRGB->red   == 0) &&
				(lpRGB->green == 0) &&
				(lpRGB->blue  == 0))
			{
				iBlack = i;
			}
		}

		if (iWhite < 0)
		{
			if ((lpRGB->red   == 255) &&
				(lpRGB->green == 255) &&
				(lpRGB->blue  == 255))
			{
				iWhite = i;
			}
		}

		if (RGB_TO_GRAY(lpRGB) < iMinLum)
		{
			iMinLum = RGB_TO_GRAY(lpRGB);
			iMinIndex = i;
		}

		if (RGB_TO_GRAY(lpRGB) > iMaxLum)
		{
			iMaxLum = RGB_TO_GRAY(lpRGB);
			iMaxIndex = i;
		}

		i++;
		lpRGB++;
	}

	if (iBlack == -1)
	{
		iBlack = iMinIndex;
	}

	if (iWhite == -1) 
	{
		iWhite = iMaxIndex;
	}

	iLastBlack = iBlack;
	iLastWhite = iWhite;
	lpLastColorMap = lpColorMap;

	*lpiBlack = iBlack;
	*lpiWhite = iWhite;
}

/*=========================================================================*/

void LineArtToPaletteColor(
	LPTR       lpSrc, 
	LPTR       lpDst,
	int        iCount,
	LPCOLORMAP lpColorMap)
{
	int iBlack, iWhite;
	int iValue;
	int iMask = 1<<7;

	FindBlackWhite( lpColorMap, (int FAR *)&iBlack, (int FAR *)&iWhite );

	iValue = *lpSrc++;

	while(iCount-- > 0)
	{
		*lpDst++ = (iValue & iMask) ? iWhite : iBlack;

		if (iMask&1)
		{
			iMask = 1<<7;
			iValue = *lpSrc++;
		}
		else
		{
			iMask >>= 1;
		}
	}
}

/*=========================================================================*/

void LineArtToRGBColor(
	LPTR lpSrc, 
	LPTR lpDst,
	int  iCount)
{
	RGBS white, black;
	LPRGB lpRGB = (LPRGB)lpDst;
	int iValue;
	int iMask = 1<<7;

	iValue = *lpSrc++;

	white.red   = 255;
	white.green = 255;
	white.blue  = 255;

	black.red   = 0;
	black.green = 0;
	black.blue  = 0;

	while(iCount-- > 0)
	{
		if (iValue & iMask)
			*lpRGB++ = white;
		else
			*lpRGB++ = black;

		if (iMask&1)
		{
			iMask = 1<<7;
			iValue = *lpSrc++;
		}
		else
		{
			iMask >>= 1;
		}
	}
}

/*=========================================================================*/

void LineArtToCMYKColor(
	LPTR lpSrc, 
	LPTR lpDst,
	int  iCount)
{
	LPCMYK lpCMYK = (LPCMYK)lpDst;
	CMYKS black, white;
	int iValue;
	int iMask = 1<<7;

	iValue = *lpSrc++;

	RGBtoCMYK(   0,   0,   0, &black );
	RGBtoCMYK( 255, 255, 255, &white );

	while(iCount-- > 0)
	{
		if (iValue & iMask)
		{
			*lpCMYK++ = white;
		}
		else
		{
			*lpCMYK++ = black;
		}

		if (iMask&1)
		{
			iMask = 1<<7;
			iValue = *lpSrc++;
		}
		else
		{
			iMask >>= 1;
		}
	}
}

/*=========================================================================*/

void GrayScaleToLineArt(
	LPTR lpSrc, 
	LPTR lpDst,
	int  iCount)
{
#ifdef C_CODE
	int iMask = 1<<7;
	int iValue;

	iValue = 0;

	while(iCount-- > 0)
	{
		if (*lpSrc++ > LINEART_THRESHOLD)
		{
			iValue |= iMask;
		}

		if (iMask&1)
		{
			*lpDst++ = iValue;
			iValue = 0;
			iMask = 1<<7;
		}
		else
		{
			iMask >>= 1;
		}
	}

	if (iMask != 1<<7)
		*lpDst++ = iValue;

#else // !C_CODE
__asm {
	push	ds
	mov     cx, iCount
	or      cx, cx
	je      con2la_end
	lds     si, lpSrc
	les     di, lpDst
	cld
	mov     bl, 128                 ; bit
	mov     bh, 0                   ; byte

con2la_loop:
	lodsb                           ; al = *lpIn++
	cmp     al, 127
	jbe     con2la_1
	or      bh, bl                  ; byte |= bit
con2la_1:
	shr     bl, 1                   ;bit
	jne     con2la_2
	mov     al, bh
	stosb
	mov     bl, 128                 ;bit
	mov     bh, 0                   ;byte
con2la_2:
	loop    con2la_loop

	cmp     bl, 128                 ;bit
	je      con2la_end
	mov     al, bh
	stosb
con2la_end:
	pop		ds
	}
#endif // !C_CODE
}

/*=========================================================================*/

void GrayScaleToPaletteColor(
	LPTR       lpSrc, 
	LPTR       lpDst,
	int        iCount,
	LPCOLORMAP lpDstMap)
{
	copy( lpSrc, lpDst, iCount );
}

void GrayScaleToPaletteColorEx(
	LPTR       lpSrc, 
	LPTR       lpDst,
	int        iCount,
	LPTR 	   lpPaletteLUT)
{
	WORD wRGB;

	while (--iCount >= 0)
	{
		wRGB = GraytoMiniRGB(*lpSrc);
		++lpSrc;
		*lpDst++ = lpPaletteLUT[wRGB];
	}
}

/*=========================================================================*/

void GrayScaleToRGBColor(
	LPTR lpSrc, 
	LPTR lpDst,
	int  iCount)
{
#ifdef C_CODE
	int iValue;

	while(iCount-- > 0)
	{
		iValue = *lpSrc++;
		*lpDst++ = iValue;
		*lpDst++ = iValue;
		*lpDst++ = iValue;
	}
#else // !C_CODE
__asm {
		push	ds
		les	di, lpDst	; Setup Destination
		lds	si, lpSrc	; Setup Source
		mov	cx, iCount	; get count
		cld
gr2rgb:
		lodsb			; Get Source Gray
		stosb			; Put Dest Red
		stosb			; Put Dest Green
		stosb			; Put Dest Blue
		dec	cx
		jnz	gr2rgb
		pop	ds
	}
#endif // !C_CODE
}

/*=========================================================================*/

void GrayScaleToCMYKColor(
	LPTR    lpSrc, 
	LPTR    lpDst,
	int     iCount)
{
	int iValue;
	LPCMYK lpCMYK = (LPCMYK)lpDst;

	while(iCount-- > 0)
	{
		iValue = *lpSrc++;
		RGBtoCMYK( (BYTE)iValue, (BYTE)iValue, (BYTE)iValue, lpCMYK );
		lpCMYK++;
	}
}

/*=========================================================================*/

static unsigned char lpLastGray[256];
static LPCOLORMAP    lpLastGrayMap = NULL;

static LPTR MakeGrayMap( LPCOLORMAP lpColorMap )
{
	int i;
	LPRGB lpRGBData;

	if (lpColorMap != lpLastGrayMap)
	{
		lpRGBData = lpColorMap->RGBData;
		for(i=0;i<lpColorMap->NumEntries;i++)
		{
			lpLastGray[i] = RGB_TO_GRAY(lpRGBData);
			lpRGBData++;
		}
		lpLastGrayMap = lpColorMap;
	}

	return(lpLastGray);
}

/*=========================================================================*/

static LPTR MakeLineArtMap(LPCOLORMAP lpColorMap)
{
	int i;
	LPRGB lpRGBData;

	if (lpColorMap != lpLastGrayMap)
	{
		lpRGBData = lpColorMap->RGBData;
		for(i=0;i<lpColorMap->NumEntries;i++)
		{
			lpLastGray[i] = (RGB_TO_GRAY(lpRGBData) > LINEART_THRESHOLD) 
				? 0xFF : 0x00;
			lpRGBData++;
		}
		lpLastGrayMap = lpColorMap;
	}

	return(lpLastGray);
}

/*=========================================================================*/

void PaletteColorToLineArt(
	LPTR       lpSrc, 
	LPTR       lpDst,
	int        iCount,
	LPCOLORMAP lpSrcMap)
{
	int iValue, iMask;

	LPTR lpMap = MakeLineArtMap( lpSrcMap );

	iValue = 0;
	iMask = 1<<7;

	while(iCount-- > 0)
	{
		if (lpMap[*lpSrc++])
		{
			iValue |= iMask;
		}

		if (iMask & 1)
		{
			*lpDst++ = iValue;
			iValue = 0;
			iMask = 1<<7;
		}
		else
		{
			iMask >>= 1;
		}
	}

	if (iMask != 1<<7)
		*lpDst++ = iValue;
}

/*=========================================================================*/

void PaletteColorToGrayScale(
	LPTR       lpSrc, 
	LPTR       lpDst,
	int        iCount,
	LPCOLORMAP lpSrcMap)
{
	LPTR lpGray = MakeGrayMap(lpSrcMap);

	while(iCount-- > 0)
	{
		*lpDst++ = lpGray[*lpSrc++];
	}
}

/*=========================================================================*/

void PaletteColorToPaletteColor(
	LPTR       lpSrc, 
	LPCOLORMAP lpSrcMap,
	LPTR       lpDst,
	int        iCount,
	LPTR	   lpPaletteLUT)
{
	WORD wRGB;
	LPRGB lpRGB, lpRGBmap;
	BYTE i;
	
	lpRGBmap = lpSrcMap->RGBData;
	while (--iCount >= 0)
	{
		i = *lpSrc++;
		lpRGB = lpRGBmap+i;
		wRGB = RGBtoMiniRGB(lpRGB);
		*lpDst++ = lpPaletteLUT[wRGB];
	}
}

/*=========================================================================*/

void PaletteColorToRGBColor(
	LPTR       lpSrc, 
	LPTR       lpDst,
	int        iCount,
	LPCOLORMAP lpSrcMap)
{
	LPRGB lpRGB, lpSrcRGB;

	lpRGB = lpSrcMap->RGBData;

	while(iCount-- > 0)
	{
		lpSrcRGB = &lpRGB[*lpSrc++];
		*lpDst++ = lpSrcRGB->red;
		*lpDst++ = lpSrcRGB->green;
		*lpDst++ = lpSrcRGB->blue;
	}
}

/*=========================================================================*/

void PaletteColorToCMYKColor(
	LPTR       lpSrc, 
	LPTR       lpDst,
	int        iCount,
	LPCOLORMAP lpSrcMap)
{
	LPCMYK lpCMYK = (LPCMYK)lpDst;
	LPRGB  lpRGB, lpSrcRGB;

	lpRGB = lpSrcMap->RGBData;

	while(iCount-- > 0)
	{
		lpSrcRGB = &lpRGB[*lpSrc++];

		RGBtoCMYK( lpSrcRGB->red, lpSrcRGB->green, lpSrcRGB->blue, lpCMYK );

		lpCMYK++;
	}
}

/*=========================================================================*/

void RGBColorToLineArt(
	LPTR lpSrc, 
	LPTR lpDst,
	int  iCount)
{
	int iValue, iMask;
	LPRGB lpRGB = (LPRGB)lpSrc;

	iValue = 0;
	iMask = 1<<7;

	while(iCount-- > 0)
	{
		if (RGB_TO_GRAY(lpRGB) > LINEART_THRESHOLD)
		{
			iValue |= iMask;
		}

		lpRGB++;

		if (iMask & 1)
		{
			*lpDst++ = iValue;
			iValue = 0;
			iMask = 1<<7;
		}
		else
		{
			iMask >>= 1;
		}
	}

	if (iMask != (1<<7))
	{
		*lpDst++ = iValue;
	}
}

/*=========================================================================*/

void RGBColorToGrayScale(
	LPTR lpSrc, 
	LPTR lpDst,
	int  iCount)
{
	LPRGB lpRGB = (LPRGB)lpSrc;

	while(iCount-- > 0)
	{
		*lpDst++ = RGB_TO_GRAY(lpRGB);

		lpRGB++;
	}
}

/*=========================================================================*/

static LPINT lpErrorLine = NULL;
static long lBufLength  = -1L;

void RGBColorToPaletteColor(
	LPTR       lpSrc, 
	LPTR       lpDst,
	int        iRow,
	int        iCount,
	LPCOLORMAP lpDstMap)
{
	LPINT lpError;
	int curError, value;

	// Free the error buffer
	if (!iCount)
	{
		if (lpErrorLine)
			FreeUp((LPTR)lpErrorLine);

		lpErrorLine = NULL;
		return;
	}

	if (iCount != lBufLength)
	{
		if (lpErrorLine)
			FreeUp((LPTR)lpErrorLine);

		lpErrorLine = NULL;
	}

	// Allocate the error buffer
	if (!lpErrorLine)
	{
		lBufLength  = iCount;
		lpErrorLine = (LPINT)Alloc(sizeof(int)*(lBufLength+16)*3L);

		if (!lpErrorLine)
			return;

		lclr( (LPTR)lpErrorLine, (lBufLength+16) * sizeof(WORD) * 3);
	}

	lpError = lpErrorLine+8;

	while( iCount-- > 0 )
	{
		value = 0;

		// Set Red Value
		curError = (*lpSrc++ + *lpError);
		if (curError >= 255) {
			curError -= 255;

			value += 5*36;
		} else {
			if (curError > 0) {
				value    += ((curError+25)/51)*36;
				curError -= ((curError+25)/51)*51;
			}
		}

		// Propogate Error left, right and down
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		// Set Green Value
		curError = (*lpSrc++ + *lpError);
		if (curError >= 255) {
			curError -= 255;

			value += 5*6;
		} else {
			if (curError > 0) {
				value    += ((curError+25)/51)*6;
				curError -= ((curError+25)/51)*51;
			}
		}
		// Propogate Error left, right and down
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		// Set Blue Value
		curError = (*lpSrc++ + *lpError);
		if (curError >= 255) {
			curError -= 255;

			value += 5;
		} else {
			if (curError > 0) {
				value    += ((curError+25)/51);
				curError -= ((curError+25)/51)*51;
			}
		}
		// Propogate error left, right and down
		lpError[0]   = (curError*26)/64;
		lpError[-3] += (curError*19)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		*lpDst++ = value;
	}
}

/*=========================================================================*/

void RGBColorToPaletteColorEx(
	LPTR       lpSrc, 
	LPTR       lpDst,
	int        iCount,
	LPTR	   lpPaletteLUT)
{
	WORD wRGB;
	LPRGB lpRGB;

	lpRGB = (LPRGB)lpSrc;
	
	while (--iCount >= 0)
	{
		wRGB = RGBtoMiniRGB(lpRGB);
		++lpRGB;
		*lpDst++ = lpPaletteLUT[wRGB];
	}
}

/*=========================================================================*/

void RGBColorToCMYKColor(
	LPTR lpSrc, 
	LPTR lpDst,
	int  iCount)
{
	RGBtoCMYKScanline( (LPRGB)lpSrc, (LPCMYK)lpDst, iCount );
}

/*=========================================================================*/

void CMYKColorToLineArt(
	LPTR lpSrc, 
	LPTR lpDst,
	int  iCount)
{
	int iValue, iMask;
	LPCMYK lpCMYK = (LPCMYK)lpSrc;

	iValue = 0;
	iMask = 1<<7;

	while(iCount-- > 0)
	{
		if (CMYKtoL(lpCMYK) > LINEART_THRESHOLD)
		{
			iValue |= iMask;
		}

		lpCMYK++;

		if (iMask & 1)
		{
			*lpDst++ = iValue;
			iValue = 0;
			iMask = 1<<7;
		}
		else
		{
			iMask >>= 1;
		}
	}

	if (iMask != (1<<7))
	{
		*lpDst++ = iValue;
	}
}

/*=========================================================================*/

void CMYKColorToGrayScale(
	LPTR lpSrc, 
	LPTR lpDst,
	int  iCount)
{
	LPCMYK lpCMYK = (LPCMYK)lpSrc;

	while(iCount-- > 0)
	{
		*lpDst++ = CMYKtoL(lpCMYK);

		lpCMYK++;
	}
}

/*=========================================================================*/

void CMYKColorToPaletteColor(
	LPTR       lpSrc, 
	LPTR       lpDst,
	int        iRow,
	int        iCount,
	LPCOLORMAP lpDstMap)
{
	LPCMYK lpCMYK = (LPCMYK)lpSrc;
	LPINT lpError;
	int curError;
	RGBS rgb;
	int r, g, b;

	// Free the error buffer
	if (!iCount)
	{
		if (lpErrorLine)
			FreeUp((LPTR)lpErrorLine);

		lpErrorLine = NULL;
		return;
	}

	if (iCount != lBufLength)
	{
		if (lpErrorLine)
			FreeUp((LPTR)lpErrorLine);

		lpErrorLine = NULL;
	}

	// Allocate the error buffer
	if (!lpErrorLine)
	{
		lBufLength  = iCount;
		lpErrorLine = (LPINT)Alloc(sizeof(int)*(lBufLength+16)*3L);

		if (!lpErrorLine)
			return;

		lclr( (LPTR)lpErrorLine, (lBufLength+16) * sizeof(WORD) * 3);
	}

	lpError = lpErrorLine+8;

	while( iCount-- > 0 )
	{
		CMYKtoRGB( lpCMYK->c, lpCMYK->m, lpCMYK->y, lpCMYK->k, &rgb);
		lpCMYK++;

		// Set Red Value
		curError = (rgb.red + *lpError);
		if (curError >= 255) {
			curError -= 255;

			r = 0xE0;
		} else {
			if (curError < 0) {
				r = 0;
				*lpError = curError;
			} else {
				r = (curError & 0xE0)>>5;
				curError -= (r * 255) / 7;
				r = r << 5;
			}
		}
		// Propogate Error left, right and down
		lpError[-3] += (curError*19)/64;
		lpError[0]   = (curError*26)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		// Set Green Value
		curError = (rgb.green + *lpError);
		if (curError >= 255) {
			curError -= 255;

			g = 0x1C;
		} else {
			if (curError < 0) {
				g = 0;
			} else {
				g = (curError & 0xE0) >> 5;
				curError -= (g * 255) / 7;
				g <<= 2;
			}
		}
		// Propogate Error left, right and down
		lpError[-3] += (curError*19)/64;
		lpError[0]   = (curError*26)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		// Set Blue Value
		curError = (rgb.blue + *lpError);
		if (curError >= 255) {
			curError -= 255;

			b = 0x03;
		} else {
			if (curError < 0) {
				b = 0;
			} else {
				b = (curError & 0xC0)>>6;
				curError -= (b * 255) / 3;
			}
		}
		// Propogate error left, right and down
		lpError[-3] += (curError*19)/64;
		lpError[0]   = (curError*26)/64;
		lpError[3]  += (curError*19)/64;
		lpError++;

		*lpDst++ = r | g | b;
	}
}

/*=========================================================================*/

void CMYKColorToPaletteColorEx(
	LPTR       lpSrc, 
	LPTR       lpDst,
	int        iCount,
	LPTR		lpPaletteLUT)
{
	LPCMYK lpCMYK = (LPCMYK)lpSrc;
	RGBS rgb;
	WORD wRGB;

	while (--iCount >= 0)
	{
		CMYKtoRGB( lpCMYK->c, lpCMYK->m, lpCMYK->y, lpCMYK->k, &rgb);
		lpCMYK++;
		wRGB = RGBtoMiniRGB(&rgb);
		*lpDst++ = lpPaletteLUT[wRGB];
	}
}

/*=========================================================================*/

void CMYKColorToRGBColor(
	LPTR lpSrc, 
	LPTR lpDst,
	int  iCount)
{
	CMYKtoRGBScanline( (LPCMYK)lpSrc, (LPRGB)lpDst, iCount );
}

/*=========================================================================*\
	Routine to convert frame data types.	
\*=========================================================================*/

void FrameTypeConvert(
	LPTR        lpSrc,
	FRMDATATYPE fdtSrc,
	LPCOLORMAP  lpSrcMap,
	int         iRow,
	LPTR        lpDst,
	FRMDATATYPE fdtDst,
	LPCOLORMAP  lpDstMap,
	int         iCount)
{
	// Make sure that we have valid parameters
	if ((!lpSrc) || (!lpDst))
		return;

	if (fdtSrc != fdtDst)
	{
		// Force Source type to grayscale if no input colormap given
		if ((fdtSrc == FDT_PALETTECOLOR) && (lpSrcMap == NULL))
			fdtSrc = FDT_GRAYSCALE;

		// Force Destination type to grayscale if no output colormap given
		if ((fdtDst == FDT_PALETTECOLOR) && (lpDstMap == NULL))
			fdtDst = FDT_GRAYSCALE;
	}

	// Call the appropriate routine for conversion
	switch(fdtSrc)
	{
		case FDT_LINEART :
			switch(fdtDst)
			{
				case FDT_LINEART :
					copy(lpSrc, lpDst, (iCount+7)/8);
				break;

				case FDT_GRAYSCALE :
					LineArtToGrayScale( lpSrc, lpDst, iCount );
				break;

				case FDT_PALETTECOLOR :
					LineArtToPaletteColor( lpSrc, lpDst, iCount, lpDstMap );
				break;

				case FDT_RGBCOLOR :
					LineArtToRGBColor( lpSrc, lpDst, iCount );
				break;

				case FDT_CMYKCOLOR :
					LineArtToCMYKColor( lpSrc, lpDst, iCount );
				break;
			}
		break;

		case FDT_GRAYSCALE :
			switch(fdtDst)
			{
				case FDT_LINEART :
					GrayScaleToLineArt( lpSrc, lpDst, iCount );
				break;

				case FDT_GRAYSCALE :
					copy( lpSrc, lpDst, iCount );
				break;

				case FDT_PALETTECOLOR :
					GrayScaleToPaletteColor( lpSrc, lpDst, iCount, lpDstMap );
				break;

				case FDT_RGBCOLOR :
					GrayScaleToRGBColor( lpSrc, lpDst, iCount );
				break;

				case FDT_CMYKCOLOR :
					GrayScaleToCMYKColor( lpSrc, lpDst, iCount );
				break;
			}
		break;

		case FDT_PALETTECOLOR :
			switch(fdtDst)
			{
				case FDT_LINEART :
					PaletteColorToLineArt( lpSrc, lpDst, iCount, lpSrcMap );			
				break;

				case FDT_GRAYSCALE :
					PaletteColorToGrayScale( lpSrc, lpDst, iCount, lpSrcMap );			
				break;

				case FDT_PALETTECOLOR :
					copy( lpSrc, lpDst, iCount );
				break;

				case FDT_RGBCOLOR :
					PaletteColorToRGBColor( lpSrc, lpDst, iCount, lpSrcMap );			
				break;

				case FDT_CMYKCOLOR :
					PaletteColorToCMYKColor( lpSrc, lpDst, iCount, lpSrcMap );			
				break;
			}
		break;

		case FDT_RGBCOLOR :
			switch(fdtDst)
			{
				case FDT_LINEART :
					RGBColorToLineArt( lpSrc, lpDst, iCount );
				break;

				case FDT_GRAYSCALE :
					RGBColorToGrayScale( lpSrc, lpDst, iCount );
				break;

				case FDT_PALETTECOLOR :
					RGBColorToPaletteColor( lpSrc, lpDst, iRow, iCount, lpDstMap );
				break;

				case FDT_RGBCOLOR :
					copy( lpSrc, lpDst, iCount*3 );
				break;

				case FDT_CMYKCOLOR :
					RGBColorToCMYKColor( lpSrc, lpDst, iCount );
				break;
			}
		break;

		case FDT_CMYKCOLOR :
			switch(fdtDst)
			{
				case FDT_LINEART :
					CMYKColorToLineArt( lpSrc, lpDst, iCount );
				break;

				case FDT_GRAYSCALE :
					CMYKColorToGrayScale( lpSrc, lpDst, iCount );
				break;

				case FDT_PALETTECOLOR :
					CMYKColorToPaletteColor( lpSrc, lpDst, iRow, iCount, lpDstMap );
				break;

				case FDT_RGBCOLOR :
					CMYKColorToRGBColor( lpSrc, lpDst, iCount );
				break;

				case FDT_CMYKCOLOR :
					copy( lpSrc, lpDst, iCount*4 );
				break;
			}
		break;
	}
}

/*=========================================================================*\
	Routine to convert frame data types.	
\*=========================================================================*/

void FrameTypeConvertEx(
	LPTR        lpSrc,
	FRMTYPEINFO ftiSrc,
	int         iRow,
	LPTR        lpDst,
	FRMTYPEINFO ftiDst,
	int         iCount,
	LPTR		lpPaletteLUT)
{
	FRMDATATYPE fdtSrc, fdtDst;
	LPCOLORMAP lpSrcMap, lpDstMap;

	// Make sure that we have valid parameters
	if ((!lpSrc) || (!lpDst))
		return;

	fdtSrc = ftiSrc.DataType;
	lpSrcMap = ftiSrc.ColorMap;
	fdtDst = ftiDst.DataType;
	lpDstMap = ftiDst.ColorMap;

	if (fdtSrc != fdtDst)
	{
		// Force Source type to grayscale if no input colormap given
		if ((fdtSrc == FDT_PALETTECOLOR) && (lpSrcMap == NULL))
			fdtSrc = FDT_GRAYSCALE;

		// Force Destination type to grayscale if no output colormap given
		if ((fdtDst == FDT_PALETTECOLOR) && (lpDstMap == NULL))
			fdtDst = FDT_GRAYSCALE;
	}

	// Call the appropriate routine for conversion
	switch(fdtSrc)
	{
		case FDT_LINEART :
			switch(fdtDst)
			{
				case FDT_LINEART :
					copy(lpSrc, lpDst, (iCount+7)/8);
				break;

				case FDT_GRAYSCALE :
					LineArtToGrayScale( lpSrc, lpDst, iCount );
				break;

				case FDT_PALETTECOLOR :
					LineArtToPaletteColor( lpSrc, lpDst, iCount, lpDstMap );
				break;

				case FDT_RGBCOLOR :
					LineArtToRGBColor( lpSrc, lpDst, iCount );
				break;

				case FDT_CMYKCOLOR :
					LineArtToCMYKColor( lpSrc, lpDst, iCount );
				break;
			}
		break;

		case FDT_GRAYSCALE :
			switch(fdtDst)
			{
				case FDT_LINEART :
					GrayScaleToLineArt( lpSrc, lpDst, iCount );
				break;

				case FDT_GRAYSCALE :
					copy( lpSrc, lpDst, iCount );
				break;

				case FDT_PALETTECOLOR :
					if (lpPaletteLUT)
						GrayScaleToPaletteColorEx( lpSrc, lpDst, iCount, lpPaletteLUT );
					else
						GrayScaleToPaletteColor( lpSrc, lpDst, iCount, lpDstMap );
				break;

				case FDT_RGBCOLOR :
					GrayScaleToRGBColor( lpSrc, lpDst, iCount );
				break;

				case FDT_CMYKCOLOR :
					GrayScaleToCMYKColor( lpSrc, lpDst, iCount );
				break;
			}
		break;

		case FDT_PALETTECOLOR :
			switch(fdtDst)
			{
				case FDT_LINEART :
					PaletteColorToLineArt( lpSrc, lpDst, iCount, lpSrcMap );			
				break;

				case FDT_GRAYSCALE :
					PaletteColorToGrayScale( lpSrc, lpDst, iCount, lpSrcMap );			
				break;

				case FDT_PALETTECOLOR :
					if (lpPaletteLUT)
						PaletteColorToPaletteColor(lpSrc, lpSrcMap, lpDst, iCount, lpPaletteLUT);
					else
						copy( lpSrc, lpDst, iCount );
				break;

				case FDT_RGBCOLOR :
					PaletteColorToRGBColor( lpSrc, lpDst, iCount, lpSrcMap );			
				break;

				case FDT_CMYKCOLOR :
					PaletteColorToCMYKColor( lpSrc, lpDst, iCount, lpSrcMap );			
				break;
			}
		break;

		case FDT_RGBCOLOR :
			switch(fdtDst)
			{
				case FDT_LINEART :
					RGBColorToLineArt( lpSrc, lpDst, iCount );
				break;

				case FDT_GRAYSCALE :
					RGBColorToGrayScale( lpSrc, lpDst, iCount );
				break;

				case FDT_PALETTECOLOR :
					if (lpPaletteLUT)
						RGBColorToPaletteColorEx( lpSrc, lpDst, iCount, lpPaletteLUT );
					else
						RGBColorToPaletteColor( lpSrc, lpDst, iRow, iCount, lpDstMap );
				break;

				case FDT_RGBCOLOR :
					copy( lpSrc, lpDst, iCount*3 );
				break;

				case FDT_CMYKCOLOR :
					RGBColorToCMYKColor( lpSrc, lpDst, iCount );
				break;
			}
		break;

		case FDT_CMYKCOLOR :
			switch(fdtDst)
			{
				case FDT_LINEART :
					CMYKColorToLineArt( lpSrc, lpDst, iCount );
				break;

				case FDT_GRAYSCALE :
					CMYKColorToGrayScale( lpSrc, lpDst, iCount );
				break;

				case FDT_PALETTECOLOR :
					if (lpPaletteLUT)
						CMYKColorToPaletteColorEx( lpSrc, lpDst, iCount, lpPaletteLUT );
					else
						CMYKColorToPaletteColor( lpSrc, lpDst, iRow, iCount, lpDstMap );
				break;

				case FDT_RGBCOLOR :
					CMYKColorToRGBColor( lpSrc, lpDst, iCount );
				break;

				case FDT_CMYKCOLOR :
					copy( lpSrc, lpDst, iCount*4 );
				break;
			}
		break;
	}
}
#endif

/*=========================================================================*/

