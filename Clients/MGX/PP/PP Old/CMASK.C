// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®PL1¯®FD1¯®BT0¯®TP0¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

static RECT rPreview;

/************************************************************************/
BOOL InColorMask(lpRGB, lpPixel)
/************************************************************************/
LPRGB lpRGB;
LPTR lpPixel;
{
BYTE mask;

if (lpPixel)
	frame_getRGB( lpPixel, lpRGB );

if ( !ColorMask.Mask || ColorMask.Off )
	return(FALSE);

mask = ColorMask.RMask[lpRGB->red];
if (!mask)
	return(ColorMask.Include);

mask &= ColorMask.GMask[lpRGB->green];
if (!mask)
	return(ColorMask.Include);

mask &= ColorMask.BMask[lpRGB->blue];
if (!mask)
	return(ColorMask.Include);
return(!ColorMask.Include);
}


/***********************************************************************/
void SetupShield()
/***********************************************************************/
{
int i, range;
BOOL on;
RGBS rgb, rgb1, rgb2;
HWND hWnd;

for (i = 0; i < 8; ++i)
	{
	rgb = ColorMask.MaskColor[i];
	range = ColorMask.MaskRange[i];
	range = TOGRAY(range);
	rgb1.red = bound((int)rgb.red - range, 0, 255);
	rgb1.green = bound((int)rgb.green - range, 0, 255);
	rgb1.blue = bound((int)rgb.blue - range, 0, 255);
	rgb2.red = bound((int)rgb.red + range, 0, 255);
	rgb2.green = bound((int)rgb.green + range, 0, 255);
	rgb2.blue = bound((int)rgb.blue + range, 0, 255);
	on = ColorMask.Mask & (1<<i);
	SetupColorMask((BYTE)(1<<i), &rgb1, &rgb2, on); 
	}

if ( hWnd = AstralDlgGet(IDD_SHIELDFLOAT) )
	ControlEnable( hWnd, IDC_PREVIEW, ColorMask.Mask );
}


/************************************************************************/
void SetupColorMask(Mask, lpRGB1, lpRGB2, On)
/************************************************************************/
BYTE Mask;
LPRGB lpRGB1, lpRGB2;
BOOL On;
{
int min, max, i;

min = min(lpRGB1->red, lpRGB2->red);
max = max(lpRGB1->red, lpRGB2->red);
for (i = 0; i < 256; ++i)
	{
	if (i >= min && i <= max && On)
		ColorMask.RMask[i] |= Mask;
	else	ColorMask.RMask[i] &= ~Mask;
	}

min = min(lpRGB1->green, lpRGB2->green);
max = max(lpRGB1->green, lpRGB2->green);
for (i = 0; i < 256; ++i)
	{
	if (i >= min && i <= max && On)
		ColorMask.GMask[i] |= Mask;
	else	ColorMask.GMask[i] &= ~Mask;
	}

min = min(lpRGB1->blue, lpRGB2->blue);
max = max(lpRGB1->blue, lpRGB2->blue);
for (i = 0; i < 256; ++i)
	{
	if (i >= min && i <= max && On)
		ColorMask.BMask[i] |= Mask;
	else	ColorMask.BMask[i] &= ~Mask;
	}
}


/************************************************************************/
void UpdateColorMask( lpRect )
/************************************************************************/
LPRECT lpRect;
{
RGBS rgb, rgb1, rgb2;
int i, range;
long lCount;
HWND hWnd;
COLOR Color;

i = ColorMask.MaskShield;
lCount = Measure( lpRect, &rgb, NO );
ColorStatus( &rgb, lCount );
ColorMask.MaskColor[i] = rgb;
range = TOGRAY(ColorMask.MaskRange[i]);
rgb1.red = bound((int)rgb.red - range, 0, 255);
rgb1.green = bound((int)rgb.green - range, 0, 255);
rgb1.blue = bound((int)rgb.blue - range, 0, 255);
rgb2.red = bound((int)rgb.red + range, 0, 255);
rgb2.green = bound((int)rgb.green + range, 0, 255);
rgb2.blue = bound((int)rgb.blue + range, 0, 255);
SetupColorMask(1<<i, &rgb1, &rgb2, TRUE);
CopyRGB( &rgb, &Color );
if ( hWnd = AstralDlgGet(IDD_SHIELDFLOAT) )
	{
	SetWindowLong( GetDlgItem( hWnd, IDC_MASKCOLOR1+i ), 0, Color );
	InvalidateRect( GetDlgItem( hWnd, IDC_MASKCOLOR1+i ), NULL, FALSE );
	}
}

/************************************************************************/
int ShowShieldProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
HWND hDlg;

switch (msg)
    {
    case WM_LBUTTONDBLCLK:
    case WM_DESTROY:	// The cancel operation message
	ShowShieldReset();
	break;
    default:
	break;
    }
}

/************************************************************************/
void ShowShieldPreview()
/************************************************************************/
{
RECT rMask;
HWND hWnd;

if (!lpImage)
	return;

mask_rect(&rMask);
File2DispRect(&rMask, &rMask);
if (Window.ModalProc != ShowShieldProc)
	DeactivateTool();
else // already previewed
	AstralUnionRect(&rMask, &rMask, &rPreview);
mask_rect(&rPreview);
File2DispRect(&rPreview, &rPreview);
DisplayBrush(0, 0, 0, OFF);
SetClassWord( lpImage->hWnd, GCW_HCURSOR, LoadCursor(NULL, IDC_ARROW) );
Window.ModalProc = ShowShieldProc;
lpImage->lpDisplay->FloatingImageProc = ShieldFloatProc;
AstralCursor( IDC_WAIT);
InvalidateRect(lpImage->hWnd, &rMask, FALSE);
UpdateWindow(lpImage->hWnd);
if ( hWnd = AstralDlgGet(IDD_SHIELDFLOAT) )
	ControlEnable( hWnd, IDC_RESET, YES );
AstralCursor( NULL );
}

/************************************************************************/
void ShowShieldReset()
/************************************************************************/
{
HWND hWnd;

Window.ModalProc = NULL;
if (!lpImage)
	return;
SetClassWord(lpImage->hWnd, GCW_HCURSOR, Window.hCursor);
lpImage->lpDisplay->FloatingImageProc = NULL;
AstralCursor( IDC_WAIT);
InvalidateRect(lpImage->hWnd, &rPreview, FALSE);
UpdateWindow(lpImage->hWnd);
if ( hWnd = AstralDlgGet(IDD_SHIELDFLOAT) )
	ControlEnable( hWnd, IDC_RESET, NO );
AstralCursor( NULL );
}

/************************************************************************/
int ShieldFloatProc(yline, xleft, xright, lpDst, lpSrc, lpMask)
/************************************************************************/
int yline, xleft, xright;
LPTR lpDst, lpSrc, lpMask;
{
int dx;
long color;

if (!ColorMask.Mask || ColorMask.Off)
	return(NO);
if (!lpMask)
	return(NO);
dx = xright-xleft+1;
mload(xleft, yline, dx, 1, lpMask, lpDst, frame_set(NULL));
setifset(lpMask, dx, 255);
color = frame_tocache(&Palette.ActiveRGB);
frame_setpixels(lpSrc, dx, color);
ProcessData(lpDst, lpSrc, lpMask, dx, DEPTH);
return(YES);
}

