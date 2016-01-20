//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

typedef enum
	{
	BM_NORMAL,
	BM_SETSRC,
	BM_SETDST
	} BRUSH_MODE;

// Static routines
static void WinBrush( HWND hWnd, HDC hDC, LPRECT lpRect, int flag, int fTickMarks, int shape, LFIXED DispRate );

// Retouch statics
static int cfdx = 60, cfdy = 60;
static BRUSH_MODE BrushMode;
static POINT ptCloneSrc;
static HWND hCloneSrcWnd;
static LPMGXBRUSH lpBrush;

/************************************************************************/
int TextureProc( HWND hWindow, LPARAM lParam, UINT msg )
/************************************************************************/
{
int x, y, fx, fy;
LPOBJECT lpObject;
FNAME szFileName;
int bpp, depth;
static FNAME szTextureName;
static LPFRAME lpTextureFrame;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if (!Retouch.hBrush)
		break;
	Tool.bActive = YES;
	break;

	case WM_ACTIVATE:
	if (!lParam) // a deactivate
		{
		FrameClose( lpTextureFrame );
		lpTextureFrame = NULL;
		if (Retouch.hBrush)
			DestroyMgxBrush(Retouch.hBrush);
		Retouch.hBrush = NULL;
		}
	else
		{	// an activate ( to re-create brush)
			if ( Tool.hRibbon )
				SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 1, 0L );
		}
	break;
		
    case WM_LBUTTONDOWN:
	Tool.bActive = NO;
	if (ImgCountSelObjects(lpImage, NULL) == 1)
		lpObject = ImgGetSelObject(lpImage, NULL);
	else
		{
		fx = x;
		fy = y;
		Display2File( &fx, &fy );
		lpObject = ImgFindSelObject(lpImage, x, y, NO);
		}
	if (!lpObject)
		break;
	
	if (GetMgxBrushSource(Retouch.hBrush) != SD_FILE)
		{
		// Load the texture
		if ( !LookupExtFile(Texture.TextureName, szFileName, IDN_TEXTURE))
			break;
		PixmapGetInfo(&lpObject->Pixmap, PMT_EDIT, NULL, NULL, &depth, NULL);
		if ( lpTextureFrame )
			{
			if ( !StringsEqual( szFileName, szTextureName ) ||
				FrameDepth(lpTextureFrame) != depth )
				{
				FrameClose( lpTextureFrame );
				lpTextureFrame = NULL;
				}
			}
		lstrcpy( szTextureName, szFileName );
		if ( !lpTextureFrame )
			{
			if ( !(lpTextureFrame = AstralFrameLoad( szTextureName,
				depth, &bpp, NULL )) )
				break;
			}
		SetMgxBrushSource(Retouch.hBrush, lpTextureFrame, 0, 0);
		}

	if (!ImgEditInit(lpImage, ET_OBJECT, UT_DATA, lpObject))
		{
		if (lpTextureFrame)
			{
			FrameClose(lpTextureFrame);
			lpTextureFrame = NULL;
			}
		break;
		}

	if (!MgxBrushBegin(hWindow, &lpObject->Pixmap,
					lpObject->rObject.left, lpObject->rObject.top,
					Retouch.hBrush))
		{
		if (lpTextureFrame)
			{
			FrameClose(lpTextureFrame);
			lpTextureFrame = NULL;
			}
	    break;
		}
	ImgEditedObject(lpImage, lpObject, IDS_UNDOCHANGE, NULL);
	Tool.bActive = MgxBrushActive();

	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x; fy = y;
	Display2File(&fx, &fy);
	MgxBrushStroke(fx, fy, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	MgxBrushEnd(NO);
	Tool.bActive = NO;
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	MgxBrushEnd(NO);
	Tool.bActive = NO;
	break;
    }
return(TRUE);
}

/************************************************************************/
void DisplayBrush(HWND hWnd, int x, int y, BOOL on )
/************************************************************************/
{
DisplayBrushEx(hWnd, x, y, on, NULL);
}

/************************************************************************/
BOOL DisplayBrushEx(HWND hWnd, int x, int y, BOOL on, LPRECT lpOffRect )
/************************************************************************/
{
HDC hDC;
RECT onFRect;
//LPIMAGE lpSrcImage;
LPDISPLAY lpDisplay;
//int npix, nlin;
static RECT onRect, on2Rect, rSect;
static int bx, by;
static BOOL bBrushOn, bLocked;
static HWND hBrushWnd, hBrush2Wnd;
static int ShapeOn;
static int SizeOn;
int HalfSize;

// make sure we are in a valid state
if ( on == bBrushOn || !lpImage || !Retouch.hBrush)
	return(FALSE);

if (on)
    {
	// special case to redisplay brush at same location it was turned off
    if (x == 32767)
		{
		// get location that the brush was turned off at
		x = bx;
		y = by;
		}
	// save window we turned brush on in
    hBrushWnd = hWnd;
    }
else
    {
	if (lpOffRect)
		{
		if ((hWnd != hBrushWnd ||
			!AstralIntersectRect(&rSect, &onRect, lpOffRect)) &&
			(hWnd != hBrush2Wnd ||
			!AstralIntersectRect(&rSect, &on2Rect, lpOffRect)))
			return(FALSE);
		}
    hWnd = hBrushWnd;
    hBrushWnd = 0;
    }
// do we have a window to use?
if (!hWnd)
    return(FALSE);

// save last on state
bBrushOn = on;
if ( on )
	{
	// save current mouse location for turning off brush
	bx = x;
	by = y;

	// convert display to file coordinates and get brush info
	Display2File(&x, &y);
	ShapeOn = GetMgxBrushShape(Retouch.hBrush);
	SizeOn = GetMgxBrushSize(Retouch.hBrush);
	HalfSize = SizeOn/2;

	// setup rectangle in file coordinates for brush
	if( ShapeOn == IDC_BRUSHCUSTOM      ||
	    ShapeOn == IDC_BRUSHSQUARE      ||
	    ShapeOn == IDC_BRUSHSQUAREHAIR  ||
	    ShapeOn == IDC_BRUSHCIRCLE      ||
	    ShapeOn == IDC_BRUSHGLITTERCIRCLE   ||
	    ShapeOn == IDC_BRUSHSWIRLCIRCLE   ||
	    ShapeOn == IDC_BRUSHHEXCIRCLE   ||
	    ShapeOn == IDC_BRUSHHOLLOWCIRCLE ||
	    ShapeOn == IDC_BRUSHSLASH       ||
	    ShapeOn == IDC_BRUSHSLASHHAIR   ||
	    ShapeOn == IDC_BRUSHBACKSLASH   ||
	    ShapeOn == IDC_BRUSHBACKSLASHHAIR )
		{
		onFRect.left = x - HalfSize;
		onFRect.right = onFRect.left + SizeOn - 1;
		onFRect.top = y - HalfSize;
		onFRect.bottom = onFRect.top + SizeOn - 1;
		}
	else if (ShapeOn == IDC_BRUSHHORIZONTAL || ShapeOn == IDC_BRUSHHORZHAIR)
		{
		onFRect.left = x - HalfSize;
		onFRect.right = onFRect.left + SizeOn - 1;
		onFRect.top = onFRect.bottom = y;
		}
	else if (ShapeOn == IDC_BRUSHVERTICAL || ShapeOn == IDC_BRUSHVERTHAIR)
		{
		onFRect.left = onFRect.right = x;
		onFRect.top = y - HalfSize;
		onFRect.bottom = onFRect.top + SizeOn - 1;
		}

	// convert on rectangle to display coordinates
	File2DispRect(&onFRect, &onRect);
	}

hDC = GetDC(hWnd);
lpDisplay = (LPDISPLAY)GetWindowLong(hWnd, GWL_DISPLAYPTR);
if (lpDisplay != NULL)
	WinBrush( hWnd, hDC, &onRect, bBrushOn, BrushMode == BM_SETSRC, ShapeOn,
	  lpDisplay->DispRate );
ReleaseDC(hWnd, hDC);
return(TRUE);
}

/************************************************************************/
static void WinBrush( HWND hWnd, HDC hDC, LPRECT lpRect, int flag,
						int fTickMarks, int shape, LFIXED DispRate )
/************************************************************************/
{
int size, size2, size3, y1, y2, x1, x2, dx, dy;
HBRUSH hOldBrush;
short OldROP;

if ( !hDC )
	return;

if (shape == IDC_BRUSHCUSTOM)
	shape = IDC_BRUSHSQUARE;

OldROP = SetROP2(hDC, R2_NOT);
x1 = lpRect->left; y1 = lpRect->top;
x2 = lpRect->right; y2 = lpRect->bottom;

if ( shape == IDC_BRUSHCIRCLE      || shape == IDC_BRUSHGLITTERCIRCLE || 
     shape == IDC_BRUSHSWIRLCIRCLE || shape == IDC_BRUSHHEXCIRCLE || 
     shape == IDC_BRUSHHOLLOWCIRCLE )
	{
	hOldBrush = (HBRUSH)SelectObject( hDC, GetStockObject(NULL_BRUSH) );
	Ellipse( hDC, x1, y1, x2, y2 );
	SelectObject( hDC, hOldBrush );
	}
else
if( shape == IDC_BRUSHSQUARE        ||
    shape == IDC_BRUSHSQUAREHAIR    ||
    shape == IDC_BRUSHHORIZONTAL    ||
    shape == IDC_BRUSHHORZHAIR      ||
    shape == IDC_BRUSHVERTICAL      ||
    shape == IDC_BRUSHVERTHAIR )
	{
	MoveToEx( hDC, x1, y1, NULL );
	LineTo( hDC, x2, y1 );
	LineTo( hDC, x2, y2 );
	LineTo( hDC, x1, y2 );
	LineTo( hDC, x1, y1 );
	}
else
if( shape == IDC_BRUSHSLASH || shape == IDC_BRUSHSLASHHAIR )
	{
	size2 = FMUL(2, DispRate);
	size2 = (size2+2)/2;
	size3 = FMUL(3, DispRate);
	size3 = (size3+2)/2;
	MoveToEx( hDC, x2-size2, y1, NULL );
	LineTo( hDC, x2-size2+size3, y1+size3 );
	LineTo( hDC, x1+size2, y2+size2 );
	LineTo( hDC, x1+size2-size3, y2+size2-size3 );
	LineTo( hDC, x2-size2, y1 );
	}
else
if( shape == IDC_BRUSHBACKSLASH || shape == IDC_BRUSHBACKSLASHHAIR )
	{
	size2 = FMUL(2, DispRate);
	size2 = (size2+2)/2;
	size3 = FMUL(3, DispRate);
	size3 = (size3+2)/2;
	MoveToEx( hDC, x1+size2, y1, NULL );
	LineTo( hDC, x1+size2-size3, y1+size3 );
	LineTo( hDC, x2-size2, y2+size2 );
	LineTo( hDC, x2-size2+size3, y2+size2-size3 );
	LineTo( hDC, x1+size2, y1 );
	}

if ( !fTickMarks )
	{
	SetROP2(hDC, OldROP);
	return;
	}

dx = 1;
dy = 1;
if ( shape == IDC_BRUSHCIRCLE      || shape == IDC_BRUSHGLITTERCIRCLE || 
     shape == IDC_BRUSHSWIRLCIRCLE || shape == IDC_BRUSHHEXCIRCLE || 
     shape == IDC_BRUSHHOLLOWCIRCLE )
	{
	size = x2 - x1 + 1;
	dx = size - ((size * 100) / 141);
	size = y2 - y1 + 1;
	dy = size - ((size * 100) / 141);
	if (dx & 1)
		++dx;
	if (dy & 1)
		++dy;
	dx /= 2;
	dy /= 2;
	}
x1 += dy; y1 += dy;
x2 -= dy; y2 -= dy;
MoveToEx( hDC, x1, y1, NULL );
LineTo( hDC, x2, y2 );
MoveToEx( hDC, x2, y1, NULL );
LineTo( hDC, x1, y2 );

SetROP2(hDC, OldROP);
}

/***********************************************************************/
void SaveRetouchBrush( ITEMID idItem )
/***********************************************************************/
{
int brush;

if ( idItem < 0 )
	return;

brush = GetBrushIndex( idItem );
lpBrushSettings [brush].BrushShape = Retouch.BrushShape;
lpBrushSettings [brush].BrushSize = Retouch.BrushSize;
lpBrushSettings [brush].Opacity = Retouch.Opacity;
lpBrushSettings [brush].Pressure = Retouch.Pressure;
lpBrushSettings [brush].Feather = Retouch.Feather;
lpBrushSettings [brush].MergeMode = Retouch.MergeMode;
lstrcpy( lpBrushSettings [brush].StyleName, Retouch.StyleName );
lstrcpy( lpBrushSettings [brush].CustomBrushLoaded, Retouch.CustomBrushLoaded );
}


/***********************************************************************/
void SelectRetouchBrush( ITEMID idItem )
/***********************************************************************/
{
int brush;
static int RetouchBrushCurrent = -1;

if ( idItem < 0 )
	return;

SaveRetouchBrush( RetouchBrushCurrent );

brush = GetBrushIndex( idItem );
Retouch.BrushShape = lpBrushSettings [brush].BrushShape;
Retouch.BrushSize = lpBrushSettings [brush].BrushSize;
Retouch.Opacity = lpBrushSettings [brush].Opacity;
Retouch.Pressure = lpBrushSettings [brush].Pressure;
Retouch.Feather = lpBrushSettings [brush].Feather;
Retouch.MergeMode = lpBrushSettings [brush].MergeMode;
lstrcpy( Retouch.StyleName, lpBrushSettings [brush].StyleName );
lstrcpy( Retouch.CustomBrushLoaded, lpBrushSettings [brush].CustomBrushLoaded );

RetouchBrushCurrent = idItem;
}


/***********************************************************************/
int GetBrushIndex( ITEMID idItem )
/***********************************************************************/
{
int brush;

switch ( idItem )
	{
    case IDC_PAINT:
		brush = 0;
    case IDC_CRAYON:
		brush = 1;
    case IDC_MARKER:
		brush = 2;
    case IDC_LINES:
		brush = 3;
    case IDC_SHAPES:
		brush = 4;
    case IDC_ERASER:
		brush = 5;
	default:
		brush = 0;
	}

if (brush < 0 || brush > TOTAL_BRUSHES)
	brush = 0;
return( brush );
}
