// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// Static prototypes
static COLOR GetColor( LPCOLORINFO lpColor, COLORSPACE ColorSpace );
static void SetColorInfoString( LPSTR lpString, LPCOLORINFO lpColorInfo,
	COLORSPACE ColorSpace );
static COLORSPACE GetColorInfoString( LPSTR lpString, LPCOLORINFO lpColorInfo );
static COLORSPACE GetFrameTypeColorSpace( FRMDATATYPE DataType );

// Static data
static COLORINFO ActiveColor;
static COLORINFO AlternateColor;
static BOOL bRandom;

/************************************************************************/
void SwapActiveAlternate( void )
/************************************************************************/
{
COLORINFO TempColor;

TempColor = ActiveColor;
ActiveColor = AlternateColor;
AlternateColor = TempColor;
}

/************************************************************************/
static void SetColorInfoString( LPSTR lpString, LPCOLORINFO lpColorInfo,
	COLORSPACE ColorSpace )
/************************************************************************/
{
int a, b, c, d;

AsciiToInt4( lpString, &a, &b, &c, &d );
switch( ColorSpace )
	{
	case CS_GRAY:
		lpColorInfo->gray = a;
		break;

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
	}
}

/************************************************************************/
void SetColorInfo(
/************************************************************************/
	LPCOLORINFO lpSrcColor,
	LPCOLORINFO lpDstColor,
	COLORSPACE  ColorSpace)
{
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

#ifdef UNUSED
	// Force to gray if it really is gray
	if (ColorSpace != CS_GRAY && ColorSpace != CS_CMYK)
		if (lpDstColor->rgb.red == lpDstColor->rgb.green && 
			lpDstColor->rgb.red == lpDstColor->rgb.blue)
			lpDstColor->ColorSpace = CS_GRAY;
#endif // UNUSED

}

/************************************************************************/
void SetActiveColorString( LPSTR lpString, COLORSPACE ColorSpace )
/************************************************************************/
{
COLORINFO ColorInfo;

SetColorInfoString( lpString, &ColorInfo, ColorSpace );
SetColorInfo( &ColorInfo, &ActiveColor, ColorSpace );
if ( bRandom = !(ActiveColor.rgb.red + ActiveColor.rgb.green + ActiveColor.rgb.blue) )
	RandomizeActiveColor();
}

/************************************************************************/
void SetAlternateColorString( LPSTR lpString, COLORSPACE ColorSpace )
/************************************************************************/
{
COLORINFO ColorInfo;

SetColorInfoString( lpString, &ColorInfo, ColorSpace );
SetColorInfo( &ColorInfo, &AlternateColor, ColorSpace );
}

/************************************************************************/
void SetActiveColor(
/************************************************************************/
LPCOLORINFO lpColor,
COLORSPACE  ColorSpace,
BOOL        bToolBar)
{
COLORINFO Color;
HWND hWnd;

SetColorInfo( lpColor, &Color, ColorSpace );
bRandom = !(Color.rgb.red + Color.rgb.green + Color.rgb.blue);
if ( ColorsEqual( &Color, &ActiveColor ) )
	return;
SwapActiveAlternate();
if ( bRandom )
	RandomizeActiveColor();
else
	ActiveColor = Color;
if ( !bToolBar )
	if ( hWnd = AstralDlgGet(Tool.idRibbon) )
		SendMessage( hWnd, WM_COMMAND, IDC_ACTIVECOLOR, 2L );
}

/************************************************************************/
void SetAlternateColor(
/************************************************************************/
LPCOLORINFO lpColor,
COLORSPACE  ColorSpace,
BOOL        bToolBar)
{
HWND hWnd;

SetColorInfo( lpColor, &AlternateColor, ColorSpace );
if ( !bToolBar )
	if ( hWnd = AstralDlgGet(Tool.idRibbon) )
		SendMessage( hWnd, WM_COMMAND, IDC_ACTIVECOLOR, 4L );
}

/************************************************************************/
void SetActiveRGB(LPRGB lpRGB, BOOL bToolBar)
/************************************************************************/
{
COLORINFO Color;

Color.rgb = *lpRGB;
SetActiveColor( &Color, CS_RGB, bToolBar );
}

/************************************************************************/
void SetAlternateRGB(LPRGB lpRGB, BOOL bToolBar)
/************************************************************************/
{
COLORINFO Color;

Color.rgb = *lpRGB;
SetAlternateColor( &Color, CS_RGB, bToolBar );
}

/************************************************************************/
void RandomizeActiveColor()
/************************************************************************/
{
RGBS rgb;
int _cdecl rand(void);

if ( !bRandom )
	return;

rgb.red   = 51 * (rand() % 6);
rgb.green = 51 * (rand() % 6);
rgb.blue  = 51 * (rand() % 6);
if ( !(rgb.red + rgb.green + rgb.blue) ) // pure black
	{ rgb.red = 51; rgb.green = 51; rgb.blue = 51; }
if ( (rgb.red + rgb.green + rgb.blue) == 3*255 ) // pure white
	{ rgb.red = 204; rgb.green = 204; rgb.blue = 204; }
SetActiveRGB( &rgb, NO/*bToolBar*/ );
bRandom = YES;
}

/************************************************************************/
COLORSPACE GetActiveColorString( LPSTR lpString )
/************************************************************************/
{
return( GetColorInfoString( lpString, &ActiveColor ) );
}

/************************************************************************/
COLORSPACE GetAlternateColorString( LPSTR lpString )
/************************************************************************/
{
return( GetColorInfoString( lpString, &AlternateColor ) );
}

/************************************************************************/
COLORSPACE GetActiveColor( LPCOLORINFO lpColor )
/************************************************************************/
{
*lpColor = ActiveColor;
return( lpColor->ColorSpace );
}

/************************************************************************/
COLORSPACE GetAlternateColor( LPCOLORINFO lpColor )
/************************************************************************/
{
*lpColor = AlternateColor;
return( lpColor->ColorSpace );
}

/************************************************************************/
LPCOLORINFO GetActiveColorInfo( void )
/************************************************************************/
{
return((LPCOLORINFO)&ActiveColor);
}

/************************************************************************/
LPCOLORINFO GetAlternateColorInfo( void )
/************************************************************************/
{
return((LPCOLORINFO)&AlternateColor);
}

/************************************************************************/
COLORSPACE GetActiveColorFromType(
/************************************************************************/
FRMDATATYPE Type,
LPCOLORINFO lpColor)
{
HWND hDlg;
BOOL bSelected, Bool;

if (Type == FDT_LINEART)
	{
	if ( hDlg = AstralDlgGet(Tool.idRibbon) )
		{
		bSelected = GetDlgItemInt( hDlg, IDC_ACTIVECOLOR, &Bool, NO );
		if (bSelected)
			lpColor->gray = 0xFF;
		else
			lpColor->gray = 0x00;
		}
	else
		lpColor->gray = 0x00;
	SetColorInfo(lpColor, lpColor, CS_GRAY);
	}
else
	*lpColor = ActiveColor;

return( lpColor->ColorSpace );
}

/************************************************************************/
COLORSPACE GetAlternateColorFromType(
/************************************************************************/
FRMDATATYPE Type,
LPCOLORINFO lpColor)
{
HWND hDlg;
BOOL bSelected, Bool;

if (Type == FDT_LINEART)
	{
	if ( hDlg = AstralDlgGet(Tool.idRibbon) )
		{
		bSelected = GetDlgItemInt( hDlg, IDC_ACTIVECOLOR, &Bool, NO );
		if (bSelected)
			lpColor->gray = 0x00;
		else
			lpColor->gray = 0xFF;
		}
	else
		lpColor->gray = 0x00;
	SetColorInfo(lpColor, lpColor, CS_GRAY);
	}
else
	*lpColor = AlternateColor;

return(lpColor->ColorSpace);
}

/************************************************************************/
LPRGB GetActiveRGB(LPRGB lpRGB)
/************************************************************************/
{
if (lpRGB)
	{
	*lpRGB = ActiveColor.rgb;
	return( lpRGB );
	}
return(&ActiveColor.rgb);
}

/************************************************************************/
LPRGB GetAlternateRGB(LPRGB lpRGB)
/************************************************************************/
{
if (lpRGB)
	{
	*lpRGB = AlternateColor.rgb;
	return( lpRGB );
	}
return(&AlternateColor.rgb);
}

/************************************************************************/
static COLORSPACE GetColorInfoString( LPSTR lpString, LPCOLORINFO lpColorInfo )
/************************************************************************/
{
switch( lpColorInfo->ColorSpace )
	{
	case CS_GRAY:
		Int4ToAscii( lpString, lpColorInfo->gray, 0, 0, 0 );
		break;

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
	}

return( lpColorInfo->ColorSpace );
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

/************************************************************************/
void RepaintColors( void )
/************************************************************************/
{
HWND hWnd;

if ( hWnd = AstralDlgGet(Tool.idRibbon) )
	AstralControlRepaint( hWnd, IDC_ACTIVECOLOR );
}
