//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "data.h"
#include "routines.h"
#include "id.h"

extern HWND hWndAstral;

// Dot to Dot
// image:	only shades of red will be hidden
//			make 1st dot a red of 255,0,0,
//			make 2nd dot a red of 254,0,0,
//			etc.
//			make all lines a red of 1,0,0
//			code the first pixel with (iDotCount,0,0)
// display:	hide all shades of red except the target red
//			start target at 255
//			display any reds with the target value to solid red
// tool:	if clicked on the target red,
//				fill with line shade of red (1,0,0), then fill again with black
//				increment target
//				display next target red dot

static int iDotCount, iTarget;
/************************************************************************/
int DotToDotToolProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
int x, y;
RECT rect;
LPOBJECT lpObject;
static POINT ptStart;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
	case WM_ACTIVATE:
	if ( lParam ) // an activate
		{
		iDotCount = 0;
		iTarget = 255;
		ImgSetPixelProc( DotToDotPixelProc );
		}
	else
		{ // a deactivate
		ImgSetPixelProc( NULL );
		if ( lpImage )
			InvalidateRect( lpImage->hWnd, NULL, FALSE );
		}
	break;

    case WM_CREATE:	// The first mouse down message
	Tool.bActive = YES;
	break;

    case WM_LBUTTONDOWN:
	if (!ONIMAGE(x, y))
		break;
	Display2File(&x, &y);
	// Select the object under the cursor
	RemoveObjectMarquee(lpImage);
	SetRect(&rect, x, y, x, y);
	ImgSelObject(lpImage, NULL, &rect, NO/*fExtendedSel*/, NO/*fExcludeBase*/);
	if ( !(lpObject = ImgGetSelObject(lpImage,NULL)) )
		break;
	if ( !DotToDotEdit( hWindow, lpObject, x, y ) )
		break;
	iTarget--;
	if ( --iDotCount <= 0 )
		{ // we're done!!!
		SoundStartResource( (LPSTR)MAKEINTRESOURCE(IDS_SELECTOR),
			NO/*bLoop*/, NULL/*hInstance*/ );
		Tool.bActive = NO;
//		LayoutSetMode( hWndAstral, NULL );
		}
	break;

    case WM_LBUTTONUP:
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Tool.bActive = NO;
//	LayoutSetMode( hWndAstral, NULL );
	break;
    }
return(TRUE);
}


/************************************************************************/
static BOOL DotToDotEdit( HWND hWindow, LPOBJECT lpObject, int x, int y )
/************************************************************************/
{
COLORINFO Color;
LPTR lp;
LPFRAME lpFrame;
RGBS rgb;
RECT rect;

if ( !(lpFrame = ObjGetEditFrame(lpObject)) )
	return( NO );
if ( !iDotCount )
	{ // just starting, so set the dot count from the first pixel in the image
	if ( !(lp = FramePointer(lpFrame, 0, 0, NO)) )
		return( NO );
	frame_getRGB( lp, FrameDepth(lpFrame), &rgb );
	iDotCount = rgb.red;
	}
if ( !(lp = FramePointer(lpFrame, x, y, NO)) )
	return( NO );
frame_getRGB( lp, FrameDepth(lpFrame), &rgb );
if ( rgb.red != iTarget || rgb.green || rgb.blue )
	return( NO );
// hit the target red dot!
// Paint the dot with hidden line color first rgb=(1,0,0)
rgb.red = ( iDotCount != 1 ? 1 : 2 );
rgb.green = 0;
rgb.blue = 0;
Color.rgb = rgb;
SetColorInfo( &Color, &Color, CS_RGB );
MgxFlood( lpImage, x, y, 0/*FloodRange*/, 255/*Opacity*/, &Color,
	MM_NORMAL/*FloodMergeMode*/,
	IDC_MODEL_HL-IDC_FIRST_MODEL/*idFillColorModel*/ );
// Now, paint with black to get the dot and the line colored
Color.gray = 0; // black
SetColorInfo( &Color, &Color, CS_GRAY );
MgxFlood( lpImage, x, y, 0/*FloodRange*/, 255/*Opacity*/, &Color,
	MM_NORMAL/*FloodMergeMode*/,
	IDC_MODEL_HL-IDC_FIRST_MODEL/*idFillColorModel*/ );
// Redraw a larger version of the edit rectangle so the next red dot appears
File2DispRect( &lpObject->Pixmap.UndoRect, &rect );
InflateRect( &rect, 20, 20 );
InvalidateRect( hWindow, &rect, FALSE );
return( YES );
}


/************************************************************************/
void DotToDotPixelProc( LPTR lpSrc, LPTR lpDst, int dx, int iDepth )
/************************************************************************/
{
BYTE r, g, b;

if ( !iDepth ) iDepth = 1;
copy( lpSrc, lpDst, dx * iDepth );
if ( iDepth != 3 )
	return;
while ( --dx >= 0 )
	{
	r = *lpSrc++;
	g = *lpSrc++;
	b = *lpSrc++;
	if ( r && !g && !b && (r != iTarget) )
		{ // If it's a shade of red and not the target, hide the pixel
		*lpDst++ = 255;
		*lpDst++ = 255;
		*lpDst++ = 255;
		}
	else
		lpDst += 3;
	}
}


// Paint ball
// image:	if red is even, hide the pixel,
//			so code the color areas with even red values
//			code the first pixel with (iDotCount,0,0)
// display:	if red is even, hide the pixel
// tool:	if clicked on a pure red (255,0,0),
//				move to the right to read the area color,
//				fill red dot with that color,
//				clear the low red bit to make it odd (visible)
//				fill again to make the color visible
//			redisplay bounds
/************************************************************************/
int PaintBallToolProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
int x, y;
RECT rect;
LPOBJECT lpObject;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
	case WM_ACTIVATE:
	if ( lParam ) // an activate
		{
		iDotCount = 0;
		ImgSetPixelProc( PaintBallPixelProc );
		}
	else
		{ // a deactivate
		ImgSetPixelProc( NULL );
		if ( lpImage )
			InvalidateRect( lpImage->hWnd, NULL, FALSE );
		}
	break;

    case WM_CREATE:	// The first mouse down message
	Tool.bActive = YES;
	break;

    case WM_LBUTTONDOWN:
	if (!ONIMAGE(x, y))
		break;
	Display2File(&x, &y);
	// Select the object under the cursor
	RemoveObjectMarquee(lpImage);
	SetRect(&rect, x, y, x, y);
	ImgSelObject(lpImage, NULL, &rect, NO/*fExtendedSel*/, NO/*fExcludeBase*/);
	if ( !(lpObject = ImgGetSelObject(lpImage,NULL)) )
		break;
	if ( !PaintBallEdit( hWindow, lpObject, x, y ) )
		break;
	if ( --iDotCount <= 0 )
		{ // we're done!!!
		SoundStartResource( (LPSTR)MAKEINTRESOURCE(IDS_SELECTOR),
			NO/*bLoop*/, NULL/*hInstance*/ );
		Tool.bActive = NO;
//		LayoutSetMode( hWndAstral, NULL );
		}
	break;

    case WM_LBUTTONUP:
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Tool.bActive = NO;
//	LayoutSetMode( hWndAstral, NULL );
	break;
    }
return(TRUE);
}


/************************************************************************/
static BOOL PaintBallEdit( HWND hWindow, LPOBJECT lpObject, int x, int y )
/************************************************************************/
{
COLORINFO Color;
LPTR lp;
LPFRAME lpFrame;
RGBS rgb;
RECT rect;
int i;

if ( !(lpFrame = ObjGetEditFrame(lpObject)) )
	return( NO );
if ( !iDotCount )
	{ // just starting, so set the dot count from the first pixel in the image
	if ( !(lp = FramePointer(lpFrame, 0, 0, NO)) )
		return( NO );
	frame_getRGB( lp, FrameDepth(lpFrame), &rgb );
	iDotCount = rgb.red;
	}
if ( !(lp = FramePointer(lpFrame, x, y, NO)) )
	return( NO );
frame_getRGB( lp, FrameDepth(lpFrame), &rgb );
if ( rgb.red != 255 || rgb.green && rgb.blue )
	return( NO );
// we hit on a red dot!
// Look to the right to find the area color
for ( i=1; i<20; i++ )
	{
	if ( !(lp = FramePointer(lpFrame, x+i, y, NO)) )
		return( NO );
	frame_getRGB( lp, FrameDepth(lpFrame), &rgb );
	if ( rgb.red != 255 || rgb.green && rgb.blue )
		break; // if not pure red, we got it
	}
// Paint the dot with area color first
Color.rgb = rgb;
SetColorInfo( &Color, &Color, CS_RGB );
MgxFlood( lpImage, x, y, 0/*FloodRange*/, 255/*Opacity*/, &Color,
	MM_NORMAL/*FloodMergeMode*/,
	IDC_MODEL_HL-IDC_FIRST_MODEL/*idFillColorModel*/ );
// Now, paint with black to get the dot and the line colored
// Since the color is hidden if (r,g,b) are all odd, make them even
if ( !(Color.rgb.red & 1) )
	Color.rgb.red++;
SetColorInfo( &Color, &Color, CS_RGB );
MgxFlood( lpImage, x, y, 0/*FloodRange*/, 255/*Opacity*/, &Color,
	MM_NORMAL/*FloodMergeMode*/,
	IDC_MODEL_HL-IDC_FIRST_MODEL/*idFillColorModel*/ );
// Redraw the edit rectangle so the colors appear
File2DispRect( &lpObject->Pixmap.UndoRect, &rect );
InvalidateRect( hWindow, &rect, FALSE );
return( YES );
}


/************************************************************************/
void PaintBallPixelProc( LPTR lpSrc, LPTR lpDst, int dx, int iDepth )
/************************************************************************/
{
BYTE r, g, b;

if ( !iDepth ) iDepth = 1;
copy( lpSrc, lpDst, dx * iDepth );
if ( iDepth != 3 )
	return;
while ( --dx >= 0 )
	{
	r = *lpSrc++;
	g = *lpSrc++;
	b = *lpSrc++;
	if ( !(r & 1) )
		{ // If red is even, hide the pixel
		*lpDst++ = 255;
		*lpDst++ = 255;
		*lpDst++ = 255;
		}
	else
		lpDst += 3;
	}
}
