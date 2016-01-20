// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include "imgutil.h"

// Static prototypes
static COLOR GetColor( LPCOLORINFO lpColor, COLORSPACE ColorSpace );
static COLORSPACE GetFrameTypeColorSpace( FRMDATATYPE DataType );

/************************************************************************/
void SetColorInfo( LPCOLORINFO lpSrcColor, LPCOLORINFO lpDstColor,
                     COLORSPACE ColorSpace )
/************************************************************************/
{
	if (ColorSpace < CS_GRAY || ColorSpace > CS_CMYK)
		ColorSpace = CS_GRAY;
	lpDstColor->ColorSpace = ColorSpace;

	switch(ColorSpace)
	{
		// Original Color Space is Gray
		case CS_GRAY :
		{
			unsigned char Gray = lpSrcColor->gray;

			// Set up the Gray Color Entry
			lpDstColor->gray = Gray;

			// Set up the RGB Color Entry
			lpDstColor->rgb.red   = Gray;
			lpDstColor->rgb.green = Gray;
			lpDstColor->rgb.blue  = Gray;

			// Set up the CMYK Color Entry
			RGBtoCMYK(Gray, Gray, Gray, &lpDstColor->cmyk);

			// Set up the HSL Color Entry
			RGBtoHSL(Gray, Gray, Gray, &lpDstColor->hsl);
		}
		break;

		case CS_HSL  :
		{
			HSLS hsl;

			hsl.hue = lpSrcColor->hsl.hue;
			hsl.sat = lpSrcColor->hsl.sat;
			hsl.lum = lpSrcColor->hsl.lum;

			// Set up the HSL Color Entry
			lpDstColor->hsl.hue = hsl.hue;
			lpDstColor->hsl.sat = hsl.sat;
			lpDstColor->hsl.lum = hsl.lum;

			// Set up the RGB Color Entry
			HSLtoRGB(
				hsl.hue,
				hsl.sat,
				hsl.lum,
				&lpDstColor->rgb);

			// Set up the GRAY Color Entry
			lpDstColor->gray = TOLUM(
				lpDstColor->rgb.red,
				lpDstColor->rgb.green,
				lpDstColor->rgb.blue);

			// Set up the CMYK Color Entry
			RGBtoCMYK(
				lpDstColor->rgb.red,
				lpDstColor->rgb.green,
				lpDstColor->rgb.blue,
				&lpDstColor->cmyk);
		}
		break;

		case CS_RGB  :
		{
			RGBS rgb;

			rgb.red   = lpSrcColor->rgb.red;
			rgb.green = lpSrcColor->rgb.green;
			rgb.blue  = lpSrcColor->rgb.blue;

			// Set up the RGB Color Entry
			lpDstColor->rgb.red   = rgb.red;
			lpDstColor->rgb.green = rgb.green;
			lpDstColor->rgb.blue  = rgb.blue;

			// Set up the GRAY Color Entry
			lpDstColor->gray = TOLUM(
				lpDstColor->rgb.red,
				lpDstColor->rgb.green,
				lpDstColor->rgb.blue);

			// Set up the HSL Color Entry
			RGBtoHSL(
				rgb.red,
				rgb.green,
				rgb.blue,
				&lpDstColor->hsl);

			// Set up the CMYK Color Entry
			RGBtoCMYK(rgb.red, rgb.green, rgb.blue, &lpDstColor->cmyk);
		}
		break;

		case CS_CMYK :
		{
			CMYKS cmyk;

			cmyk.c = lpSrcColor->cmyk.c;
			cmyk.m = lpSrcColor->cmyk.m;
			cmyk.y = lpSrcColor->cmyk.y;
			cmyk.k = lpSrcColor->cmyk.k;

			// Set up the CMYK Color Entry
			lpDstColor->cmyk.c = cmyk.c;
			lpDstColor->cmyk.m = cmyk.m;
			lpDstColor->cmyk.y = cmyk.y;
			lpDstColor->cmyk.k = cmyk.k;

			// Set up the RGB Color Entry
			CMYKtoRGB(
				cmyk.c,
				cmyk.m,
				cmyk.y,
				cmyk.k,
				&lpDstColor->rgb);

			// Set up the GRAY Color Entry
			lpDstColor->gray = TOLUM(
				lpDstColor->rgb.red,
				lpDstColor->rgb.green,
				lpDstColor->rgb.blue);

			// Set up the HSL Color Entry
			RGBtoHSL(
				lpDstColor->rgb.red,
				lpDstColor->rgb.green,
				lpDstColor->rgb.blue,
				&lpDstColor->hsl);
		}
		break;
	}
}

/************************************************************************/
void SetColorInfoString( LPTSTR lpString, LPCOLORINFO lpColorInfo,
	COLORSPACE ColorSpace )
/************************************************************************/
{
int a, b, c, d;

AsciiToInt4( lpString, &a, &b, &c, &d );
switch( ColorSpace )
	{
	case CS_HSL:
		lpColorInfo->hsl.hue = a;
		lpColorInfo->hsl.sat = b;
		lpColorInfo->hsl.lum = c;
		break;

	case CS_RGB:
		lpColorInfo->rgb.red   = a;
		lpColorInfo->rgb.green = b;
		lpColorInfo->rgb.blue  = c;
		break;

	case CS_CMYK:
		lpColorInfo->cmyk.c = a;
		lpColorInfo->cmyk.m = b;
		lpColorInfo->cmyk.y = c;
		lpColorInfo->cmyk.k = d;
		break;

	default:
	case CS_GRAY:
		lpColorInfo->gray = a;
		break;

	}
}

/************************************************************************/
COLORSPACE GetColorInfoString( LPTSTR lpString, LPCOLORINFO lpColorInfo )
/************************************************************************/
{
switch( lpColorInfo->ColorSpace )
	{
	case CS_HSL:
		Int4ToAscii( lpString, lpColorInfo->hsl.hue, lpColorInfo->hsl.sat,
			lpColorInfo->hsl.lum, 0 );
		break;

	case CS_RGB:
		Int4ToAscii( lpString, lpColorInfo->rgb.red, lpColorInfo->rgb.green,
			lpColorInfo->rgb.blue, 0 );
		break;

	case CS_CMYK:
		Int4ToAscii( lpString, lpColorInfo->cmyk.c, lpColorInfo->cmyk.m,
			lpColorInfo->cmyk.y, lpColorInfo->cmyk.k );
		break;
	default:
		lpColorInfo->ColorSpace = CS_GRAY;
	case CS_GRAY:
		Int4ToAscii( lpString, lpColorInfo->gray, 0, 0, 0 );
		break;
	}

return( lpColorInfo->ColorSpace );
}

/************************************************************************/
LPCOLORINFO GetColorFromTypeInfo(FRMTYPEINFO TypeInfo, LPCOLORINFO lpColor)
/************************************************************************/
{
if (lpColor)
	{
	if (TypeInfo.DataType == FDT_PALETTECOLOR)
		lpColor->index = FrameGetNearestIndex(TypeInfo.ColorMap, &lpColor->rgb);
	}
return(lpColor);
}

/************************************************************************/
static COLOR GetColor( LPCOLORINFO lpColor, COLORSPACE ColorSpace )
/************************************************************************/
{
COLOR Color;

switch(ColorSpace)
{
	case CS_GRAY :
		Color = lpColor->gray;
	break;

	case CS_HSL  :
		Color = 0;
		CopyRGB( &lpColor->hsl, &Color );
	break;

	case CS_RGB  :
		Color = 0;
		CopyRGB( &lpColor->rgb, &Color );
	break;

	case CS_CMYK :
		CopyCMYK( &lpColor->cmyk, &Color );
	break;
}

return(Color);
}

/************************************************************************/
COLOR GetFrameColor( LPCOLORINFO lpColor, FRMDATATYPE DataType)
/************************************************************************/
{
COLORSPACE ColorSpace;

ColorSpace = GetFrameTypeColorSpace(DataType);
return(GetColor(lpColor, ColorSpace));
}

/************************************************************************/
COLOR GetFrameColor( LPCOLORINFO lpColor, FRMTYPEINFO TypeInfo)
/************************************************************************/
{
COLORSPACE ColorSpace;

GetColorFromTypeInfo(TypeInfo, lpColor);
if (TypeInfo.DataType == FDT_PALETTECOLOR)
	return(lpColor->index);
else
	{
	ColorSpace = GetFrameTypeColorSpace(TypeInfo.DataType);
	return(GetColor(lpColor, ColorSpace));
	}
}

/************************************************************************/
static COLORSPACE GetFrameTypeColorSpace( FRMDATATYPE DataType )
/************************************************************************/
{
	switch( DataType )
	{
		case FDT_LINEART :
		case FDT_GRAYSCALE :
			return(CS_GRAY);
		break;

		case FDT_RGBCOLOR :
		case FDT_PALETTECOLOR :
			return(CS_RGB);
		break;

		case FDT_CMYKCOLOR :
			return(CS_CMYK);
		break;

		default:
			return(CS_RGB);
		break;
	}
}

/************************************************************************/
BOOL ColorsEqual( LPCOLORINFO lpSrcColor, LPCOLORINFO lpDstColor)
/************************************************************************/
{
	BOOL result = TRUE;

	// Check Colorspace
	if (lpSrcColor->ColorSpace != lpDstColor->ColorSpace) result = FALSE;

	// Check Gray
	if (lpSrcColor->gray != lpDstColor->gray) result = FALSE;

	// Check RGB
	if (lpSrcColor->rgb.red   != lpDstColor->rgb.red  ) result = FALSE;
	if (lpSrcColor->rgb.green != lpDstColor->rgb.green) result = FALSE;
	if (lpSrcColor->rgb.blue  != lpDstColor->rgb.blue ) result = FALSE;

	// Check HSL
	if (lpSrcColor->hsl.hue != lpDstColor->hsl.hue) result = FALSE;
	if (lpSrcColor->hsl.sat != lpDstColor->hsl.sat) result = FALSE;
	if (lpSrcColor->hsl.lum != lpDstColor->hsl.lum) result = FALSE;

	// Check CMYK
	if (lpSrcColor->cmyk.c != lpDstColor->cmyk.c) result = FALSE;
	if (lpSrcColor->cmyk.m != lpDstColor->cmyk.m) result = FALSE;
	if (lpSrcColor->cmyk.y != lpDstColor->cmyk.y) result = FALSE;
	if (lpSrcColor->cmyk.k != lpDstColor->cmyk.k) result = FALSE;

	return(result);
}
