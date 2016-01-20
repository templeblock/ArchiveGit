// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

extern WORD wDrawShapeStyle;	/* lsh */
extern int LastShape;
extern int FullBrush, HalfBrush;
extern HWND hClientAstral;

// Retouch statics
static long pval;
static int cdx, cdy, cfdx, cfdy, gx, gy, slastx, slasty, sx, sy, startx, starty;
static BOOL any_Brushed, first, fStart;
static long TintVal, TintPixel, LightenVal, DarkenVal;
static WORD TR, TG, TB;
static int TP, OP;
static RECT StrokeRect;
static POINT StartPoint;
static RECT ClientRect, SelectRect;
static FNAME szTextureName;
static LPFRAME lpTextureFrame;
static LPMASK lpDrawShape;

/************************************************************************/
int DrawShapeProc(y, left, right, lpDst, lpSrc )
/************************************************************************/
int y, left, right;
LPTR lpDst, lpSrc;
{
LPTR m;
long color, pixel;
int iCount, depth;

mask_compute(lpDrawShape, y, FALSE);
m = mask_point(lpDrawShape, left);
color = frame_tocache(&Palette.AlternateRGB);
iCount = right - left + 1;
depth = DEPTH;
while (--iCount >= 0)
	{
	if (*m++)
		{
		frame_putpixel(lpSrc, color);
		}
	else
		{
		frame_getpixel(lpDst, &pixel);
		frame_putpixel(lpSrc, pixel);
		}
	lpSrc += depth;
	lpDst += depth;
	}
}

/************************************************************************/
int MaskDisp2File( lpPoint, count, fCircle )
/************************************************************************/
LPPOINT lpPoint;
int count;
BOOL fCircle;
{
LPPOINT	lpFirstPoint, lpOutPoint;
int i, x, y, xlast, ylast, xlastout, ylastout, iOutCount;
int nPoints[1];

lpOutPoint = lpFirstPoint = lpPoint;
iOutCount = 0;
xlast = ylast = xlastout = ylastout = 32767;
for (i=0; i<count; i++)
	{
	x = lpPoint->x;
	y = lpPoint->y;
	if (x == BEZIER_MARKER)
		{
		lpOutPoint->x = x;
		lpOutPoint->y = y;
		lpOutPoint++;
		iOutCount++;
		xlast = ylast = xlastout = ylastout = 32767;
		}
	else
	if ((abs(x-xlast) + abs(y-ylast) != 1) )
		{ /* Don't bother with 1 pixel vectors */
		xlast = x;
		ylast = y;
		Display2File( &x, &y ); // Ted uses a special Display2File here
		if ( abs(x-xlastout) + abs(y-ylastout) != 1 )
			{ /* Don't bother with 1 pixel hires vectors */
			xlastout = x;
			ylastout = y;
			lpOutPoint->x = x;
			lpOutPoint->y = y;
			lpOutPoint++;
			iOutCount++;
			}
		}
	lpPoint++;
	}

return( iOutCount );
}

/************************************************************************/
void DrawShape (lpMask, fPolyline)
LPMASK		lpMask;
BOOL		fPolyline;
{
int	nPoints;
int	x, y;
int	px, py;
int	dx, dy;
int	i, total;
LPTR	lpLine;
RGBS	rgb;
LPRGB	lpRGB;
BOOL	onMask;
BOOL	firstTime;
LPPOINT lpPoints;
RECT	rRgn;
LPFRAME lpFrame;

if (!(lpFrame = frame_set(NULL)))
	return;
any_Brushed = FALSE;
onMask = (mask_active() && !Mask.IsOutside);
rRgn = lpMask->rMask;

/* Draw interior */
if ( (wDrawShapeStyle != IDC_CIRDRAWEDGE) && !fPolyline )
	{
	lpDrawShape = lpMask;
	LineEngine(&rRgn, NULL, NULL, lpFrame, DrawShapeProc, 0, Retouch.Opacity);
	}


/* Draw exterior */
if ( (wDrawShapeStyle != IDC_CIRFILLINTERIOR) || fPolyline)
	{
	Retouch.Fadeout = 0;
	lpPoints = (LPPOINT)LineBuffer[0];
	nPoints = ShapeConvertPoints(lpMask->lpHeadShape, lpPoints);

	if (nPoints)
		{
		SetupMask();
		if (EngineInit(FullBrush, FullBrush, NULL,
				lpImage->EditFrame, NULL,
				Retouch.Fadeout, 0, 0, 0, FALSE))
			{
			lpPoints = (LPPOINT)LineBuffer[0];
			for (i = 0; i < nPoints; i++)
				{
				AstralClockCursor( i, nPoints );
				EngineStroke(0, 0, lpPoints[i].x, lpPoints[i].y, 0, 0);
				}
			if (!fPolyline && nPoints)
				EngineStroke(0, 0, lpPoints[0].x, lpPoints[0].y, 0, 0);
			EngineDone();
			}
		}
	}

AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &rRgn);
rRgn.top -= HalfBrush;
rRgn.left -= HalfBrush;
rRgn.bottom += (FullBrush-HalfBrush);
rRgn.right += (FullBrush-HalfBrush);
UpdateImage(&rRgn, YES);
}



/************************************************************************/
int PaintProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, fx, fy;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	lpImage->dirty = IDS_UNDOPAINT;
	Window.ToolActive = YES;
	SetupMask();
	if ( mask_active() )
		{
		Mask.IsOutside = !IsDisplayMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	break;

    case WM_LBUTTONDOWN:
	if (!EngineInit(FullBrush, FullBrush, NULL, lpImage->EditFrame, NULL,
	 Retouch.Fadeout, 0, x, y, TRUE))
  	    {
	    Window.ToolActive = NO;
	    break;
	    }
	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x; fy = y;
	Display2File(&fx, &fy);
	EngineStroke(0, 0, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	EngineDone();
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	break;
    }
}

/************************************************************************/
int SprayProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, fx, fy;
static int FuzzyPixels;
static int Opacity;
static LONG LastPosition;
static BOOL fActive;
static WORD wTimer;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	lpImage->dirty = IDS_UNDOSPRAY;
	Window.ToolActive = YES;
	SetupMask();
	if ( mask_active() )
		{
		Mask.IsOutside = !IsDisplayMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	break;

    case WM_LBUTTONDOWN:
	LastPosition = lParam;
	if (!EngineInit(FullBrush, FullBrush, NULL, lpImage->EditFrame, NULL,
	 Retouch.Fadeout, 0, x, y, TRUE))
  	    {
	    Window.ToolActive = NO;
	    break;
	    }

	SetTimer( lpImage->hWnd, wTimer = 1, 250, NULL );
	fActive = TRUE;
	FuzzyPixels = Retouch.FuzzyPixels;
	Opacity = Retouch.Opacity;
	Retouch.FuzzyPixels = Retouch.BrushSize/2;
	Retouch.Opacity = Retouch.Opacity/(8/(Retouch.Pressure-IDC_PRESSLIGHT+1));
	SetupMask();
	if ( mask_active() )
		{
		Mask.IsOutside = !IsDisplayMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	if (!fActive)
		break;
	LastPosition = lParam;
	fx = x; fy = y;
	Display2File(&fx, &fy);
	EngineStroke(0, 0, fx, fy, x, y);
	LastPosition = lParam;
	break;

    case WM_LBUTTONUP:
	if (!fActive)
		break;
	fActive = FALSE;
	Window.ToolActive = NO;
	EngineDone();
	if ( wTimer )
		{
		KillTimer( lpImage->hWnd, wTimer );
		wTimer = NULL;
		}
        Retouch.FuzzyPixels = FuzzyPixels;
	Retouch.Opacity = Opacity;
	SetupMask();
	if ( mask_active() )
		{
		Mask.IsOutside = !IsDisplayMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_TIMER:
	if (!fActive)
		break;
	EngineDone();
	x = LOWORD(LastPosition);
	y = HIWORD(LastPosition);
	if (EngineInit(FullBrush, FullBrush, NULL, lpImage->EditFrame, NULL,
	Retouch.Fadeout, 0, x, y, TRUE))
		{
		fx = x; fy = y;
		Display2File(&fx, &fy);
		EngineStroke(0, 0, fx, fy, x, y);
		}
	else
		{
		Window.ToolActive = NO;
		}
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	break;
    }
}

/************************************************************************/
int CloneProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, sx, sy, fx, fy;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	lpImage->dirty = IDS_UNDOCLONE;
	Window.ToolActive = YES;
	SetupMask();
	if ( mask_active() )
		{
		Mask.IsOutside = !IsDisplayMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	break;

    case WM_LBUTTONDOWN:
	if (!EngineInit(FullBrush, FullBrush, lpImage->EditFrame,
	 lpImage->EditFrame, NULL, Retouch.Fadeout, 0, x, y, TRUE))
  	    {
	    Window.ToolActive = NO;
	    break;
	    }
	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	CloneBrush(x, y, YES);
	fx = x;
	fy = y;
	Display2File( &fx, &fy );
	sx = cfdx + fx;
	sy  = cfdy + fy;
	EngineStroke(sx, sy, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
        EngineDone();
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	break;
    }
}

/************************************************************************/
int TextureProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, fx, fy, TileMode, bpp, type;
LPSHAPE lpShape;
FNAME szFileName;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	if ( !LookupExtFile(Texture.Name, szFileName, IDN_TEXTURE))
		break;
	if ( lpTextureFrame )
		{
		if ( !StringsEqual( szFileName, szTextureName ) || lpTextureFrame->Depth != DEPTH)
			{
			frame_close( lpTextureFrame );
			lpTextureFrame = NULL;
			}
		}
	lstrcpy( szTextureName, szFileName );
	if ( !lpTextureFrame )
		if ( !(lpTextureFrame = AstralFrameLoad(szTextureName, DEPTH, &bpp,
		     &type)) )
			break;
	lpImage->dirty = IDS_UNDOTEXTURE;
	Window.ToolActive = YES;
	SetupMask();
	if ( mask_active() )
		{
		Mask.IsOutside = !IsDisplayMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	break;

    case WM_LBUTTONDOWN:
	TileMode = 1;
	if (Texture.fHorzFlip)
	    TileMode |= 2;
	if (Texture.fVertFlip)
	    TileMode |= 4;
    	if (!EngineInit(FullBrush, FullBrush, lpTextureFrame, lpImage->EditFrame, NULL,
	 Retouch.Fadeout, TileMode, x, y, TRUE))
  	    {
	    Window.ToolActive = NO;
	    break;
	    }
	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x;
	fy = y;
	Display2File( &fx, &fy );
	EngineStroke(fx, fy, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
        EngineDone();
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	if (lpTextureFrame)
		{
		frame_close(lpTextureFrame);
		lpTextureFrame = NULL;
		}
	break;
    }
}


/************************************************************************/
int SharpSmoothProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, fx, fy;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if ( !CacheInit( YES, YES ) ) // Prepare the undo frame
		break;
        if (!lpImage->UndoFrame)
		{
		MessageBeep(0);
		Print("Can't use sharpen or smooth tools without undo buffer");
		break;
		}
	else if (Control.UseApply && lpImage->dirty)
		{
		Message(IDS_APPLYORUNDO);
		break;
		}
	if (Control.Function == IDC_SHARP)
		lpImage->dirty = IDS_UNDOSHARP;
        else	lpImage->dirty = IDS_UNDOSMOOTH;
	Window.ToolActive = YES;
	SetupMask();
	if ( mask_active() )
		{
		Mask.IsOutside = !IsDisplayMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	break;

    case WM_LBUTTONDOWN:
	if (!EngineInit(FullBrush, FullBrush, NULL, lpImage->EditFrame,
	 sharpsmooth_proc, Retouch.Fadeout, 0, x, y, TRUE))
  	    {
	    Window.ToolActive = NO;
	    break;
	    }
	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x; fy = y;
	Display2File(&fx, &fy);
	EngineStroke(0, 0, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	EngineDone();
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	break;
    }
}


/************************************************************************/
static void sharpsmooth_proc(x, y, dx, dy, lpDest, lpSource, lpMask, start_x, start_y, end_x, end_y)
/************************************************************************/
int x, y;
int dx, dy;
LPTR lpDest, lpSource, lpMask;
int start_x, start_y, end_x, end_y;
{
int xs, ys, xe, ye, ry, rx;
BOOL clip;
LPTR lpSLine1, lpSLine2, lpSLine3;
int bcount;
long sval;

xs = x;
ys = y;
xe = x + dx - 1;
ye = y + dy - 1;
bcount = dx * dy * DEPTH;

if ( !INFILE1( xs, ys ) || !INFILE1( xe, ye ) )
	clip = YES; else clip = NO;
frame_set(lpImage->UndoFrame);
lpSLine1 = CachePtr(0, 0, ys - 1, NO);
lpSLine2 = CachePtr(0, 0, ys, NO);
for (ry=ys; ry<=ye; ry++)
    {
    lpSLine3 = CachePtr(0, 0, ry+1, NO);
    if (!lpSLine1 || !lpSLine2 || !lpSLine3)
	{
	lpSLine1 = lpSLine2;
	lpSLine2 = lpSLine3;
	lpDest += bcount;
	lpSource += bcount;
	continue;
	}
    for (rx=xs; rx<=xe; rx++, lpDest += DEPTH, lpSource += DEPTH)
	{
	if (clip)				/* need to check clip */
	    if (!INFILE1(rx, ry))		/* dest on image? */
		{
		copy(lpDest, lpSource, DEPTH);
		continue;
		}
        sval = frame_sharpsmooth(Control.Function, rx, 
                   lpSLine1, lpSLine2, lpSLine3, Retouch.Pressure);
	frame_putpixel(lpSource, sval);
	}
    lpSLine1 = lpSLine2;
    lpSLine2 = lpSLine3;
    }
frame_set(lpImage->EditFrame);
}


/************************************************************************/
int LightenProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, fx, fy;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	lpImage->dirty = IDS_UNDOLIGHTEN;
	Window.ToolActive = YES;
	SetupMask();
	if ( mask_active() )
		{
		Mask.IsOutside = !IsDisplayMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	break;

    case WM_LBUTTONDOWN:
	if (!EngineInit(FullBrush, FullBrush, NULL, lpImage->EditFrame,
	 lightendarken_proc, Retouch.Fadeout, 0, x, y, TRUE))
  	    {
	    Window.ToolActive = NO;
	    break;
	    }
	TintVal = 255;
	DarkenVal = 0;
	if (DEPTH == 1)
		LightenVal = 2*(1+Retouch.Pressure-IDC_PRESSLIGHT);
	else
	if (DEPTH == 2)
		LightenVal = 5*(1+Retouch.Pressure-IDC_PRESSLIGHT);
	else	LightenVal = 2*(1+Retouch.Pressure-IDC_PRESSLIGHT);
	/* fall through to WM_MOUSEMOVE */
	
    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x; fy = y;
	Display2File(&fx, &fy);
	EngineStroke(0, 0, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	break;
    }
}

/************************************************************************/
int DarkenProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, fx, fy;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	lpImage->dirty = IDS_UNDODARKEN;
	Window.ToolActive = YES;
	SetupMask();
	if ( mask_active() )
		{
		Mask.IsOutside = !IsDisplayMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	break;

    case WM_LBUTTONDOWN:
	if (!EngineInit(FullBrush, FullBrush, NULL, lpImage->EditFrame,
	 lightendarken_proc, Retouch.Fadeout, 0, x, y, TRUE))
  	    {
	    Window.ToolActive = NO;
	    break;
	    }
	TintVal = 0;
	LightenVal = 0;
	if (DEPTH == 1)
		DarkenVal = 2*(1+Retouch.Pressure-IDC_PRESSLIGHT);
	else
	if (DEPTH == 2)
		DarkenVal = 5*(1+Retouch.Pressure-IDC_PRESSLIGHT);
	else	DarkenVal = 2*(1+Retouch.Pressure-IDC_PRESSLIGHT);
	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x; fy = y;
	Display2File(&fx, &fy);
	EngineStroke(0, 0, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	break;
    }
}

/************************************************************************/
static void lightendarken_proc(x, y, dx, dy, lpDest, lpSource )
/************************************************************************/
int x, y;
int dx, dy;
LPTR lpDest, lpSource;
{
int bcount, lum;
HSLS hsl;
RGBS rgb;

bcount = dx * dy;
while (--bcount >= 0)
	{
	frame_getHSL( lpDest, &hsl );
	lum = hsl.lum;
	if (lum > TintVal)
		{
		if ((lum -= (int)DarkenVal) < (int)TintVal)
			lum = TintVal;
		}
	else if (lum < TintVal)
		{
		if ((lum += (int)LightenVal) > (int)TintVal)
			lum = TintVal;
		}
	hsl.lum = lum;
	frame_putHSL( lpSource, &hsl );
	lpDest += DEPTH;
	lpSource += DEPTH;
	}
}

/************************************************************************/
int FloodProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	lpImage->dirty = IDS_UNDOFLOOD;
	Window.ToolActive = YES;
	SetupMask();
	if ( mask_active() )
		{
		Mask.IsOutside = !IsDisplayMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	break;

    case WM_LBUTTONDOWN:
	Display2File(&x, &y);
	frame_flood(Window.hDC, x, y);
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	Display2File(&x, &y);
	frame_flood(Window.hDC, x, y);
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	break;
    }
}


/************************************************************************/
int SmearProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y;
int fx, fy;
int deltax, deltay;
static int Opacity;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	lpImage->dirty = IDS_UNDOSMEAR;
	Window.ToolActive = YES;
	break;

    case WM_LBUTTONDOWN:
	if (!EngineInit(FullBrush, FullBrush, NULL,
	 lpImage->EditFrame, smear_proc, Retouch.Fadeout, 0, x, y, TRUE))
  	    {
	    Window.ToolActive = NO;
	    break;
	    }
	slastx = x; slasty = y;
	first = TRUE;
        //Opacity = Retouch.Opacity;
	//Retouch.Opacity /= 2;
	SetupMask();
	if ( mask_active() )
		{
		Mask.IsOutside = !IsDisplayMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x; fy = y;
	Display2File(&fx, &fy);
	fStart = TRUE;
	EngineStroke(0, 0, fx, fy, x, y);
#ifdef UNUSED
	deltax = x - slastx; deltay = y - slasty;
	if (!deltax && !deltay)
    	    break;
	slastx = x; slasty = y;
	fx = x; fy = y;
	Display2File(&fx, &fy);

	if ( deltay > 0 ) /* reverse the loop to paint backwards */
    		{ 
    		deltay = FMUL(deltay, lpImage->lpDisplay->DispRate);
    		if (deltay <= 0)
			deltay = 1;
    		}
	else if (deltay < 0)
    		{
    		deltay = FMUL(deltay, lpImage->lpDisplay->DispRate);
    		if (deltay >= 0)
			deltay = -1;
    		}
	if ( deltax > 0 ) /* reverse the loop to paint backwards */
    		{ 
    		deltax = FMUL(deltax, lpImage->lpDisplay->DispRate);
    		if (deltax <= 0)
			deltax = 1;
    		}
	else if (deltax < 0)
    		{
    		deltax = FMUL(deltax, lpImage->lpDisplay->DispRate);
    		if (deltax >= 0)
			deltax = -1;
    		}
	EngineStroke(fx-deltax, fy-deltay, fx, fy, x, y);
#endif
	break;


    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	EngineDone();
//	Retouch.Opacity = Opacity;
//	SetupMask();
	Mask.IsOutside = !IsDisplayMasked(x, y);
	UpdateStatusBar( NO, NO, YES, NO );
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	break;
    }
}

static RECT rSource, rLastSource;
static int ix, iy;
static FIXED dx, dy, ax, ay;
static LPTR lpLastSource;

/************************************************************************/
void smear_proc(rx, ry, rdx, rdy, lpDest, lpSource, lpMask, x1, y1, x2, y2)
/************************************************************************/
int rx, ry;
int rdx, rdy;
LPTR lpDest, lpSource, lpMask;
int x1, y1, x2, y2;
{
int x, y, sx, sy, ex, ey, lx, ly, depth;
WORD bs, bd, m;
static WORD bsrc;
int width;
RECT rDest, rSect, rData, rDummy;
LPFRAME lpFrame;
LPTR lpData, lpSrc, lpDst, lpMsk;
RGBS srgb, drgb;
int tx, ty, smearx, smeary, deltax, deltay;
long size;

if (first && lpLastSource)
	{
	FreeUp(lpLastSource);
	lpLastSource = NULL;
	}
lpFrame = frame_set(NULL);
depth = lpFrame->Depth;

// if no movement has taken place, just copy Destination
// to Source
if (x1 == x2 && y1 == y2)
	{
	copy(lpDest, lpSource, rdx*rdy*depth);
	return;
	}

// Setup rectangle to be worked on for this stroke
rDest.top = ry; rDest.bottom = rDest.top + rdy - 1;
rDest.left = rx; rDest.right = rDest.left + rdx - 1;

// Setup rectangle for smearing, if not the first rectangle
// of a stroke, expand rectangle to smear from previous rectangle
rData = rDest;
if (fStart && FALSE)
	{
	rData.right = rData.left + rdx - 1;
	rData.bottom = rData.top + rdy - 1;
	}
else
	{
	if (x1 <= x2)
		--rData.left;
	if (y1 <= y2)
		--rData.top;
	rData.right = rData.left + rdx;
	rData.bottom = rData.top + rdy;
	}

// Allocate memory for smear buffer
if (!(lpData = Alloc((long)(RectWidth(&rData)*depth*RectHeight(&rData)))))
	return;
	
// Load data to be smeared
bload(rData.left, rData.top, RectWidth(&rData), RectHeight(&rData), lpData, lpFrame, &rDummy, 0, NULL);

// If not the very first time(first button down), merge data from previous
// rectangles smear, so we can smear from rectangle to rectangle
if (!first)
	{
	if (AstralIntersectRect(&rSect, &rData, &rSource))
		MergeData(lpData, &rData, RectWidth(&rData), lpSource, &rSource, RectWidth(&rSource), &rSect);
	}

// merge data from smear source before last
if (lpLastSource)
	{
	if (AstralIntersectRect(&rSect, &rData, &rLastSource))
		MergeData(lpData, &rData, RectWidth(&rData), 
			  lpLastSource, &rLastSource, RectWidth(&rLastSource),
			  &rSect);
	FreeUp(lpLastSource);
	lpLastSource = NULL;
	}

// Merge data from destination buffer into our smear buffer
if (AstralIntersectRect(&rSect, &rData, &rDest))
	MergeData(lpData, &rData, RectWidth(&rData), lpDest, &rDest, RectWidth(&rDest), &rSect);

if (!first) // means it is at least the second time in
	{
	size = RectWidth(&rSource)*RectHeight(&rSource)*depth;
	if (lpLastSource = Alloc((long)size))
		{
		copy(lpSource, lpLastSource, size);
		rLastSource = rSource;
		}
	}

width = RectWidth(&rData) * depth;

// if start of stroke, set up all the dda shit, also set up
// smear percentage value to be used
if (fStart)
	{
	if ( x1 > x2 )
		{ x = x1 - x2; ix = -1; }
	else	{ x = x2 - x1; ix = 1; }
	if ( y1 > y2 )
		{ y = y1 - y2; iy = -1; }
	else	{ y = y2 - y1; iy = 1; }
	if ( x < y )
		{ dy = UNITY; dx = FGET( x, y ); }
	else	{ dx = UNITY; dy = FGET( y, x ); }

	/* Initialize the ax accumulator with half the dx increment */
	/* plus one half for rounding off */	
	ax = HALF; //( dx + UNITY )/2;
	//ax &= 0xFFFFL;
	ay = HALF; //( dy + UNITY )/2;
	//ay &= 0xFFFFL;

	bsrc = 64 + ((Retouch.Pressure-IDC_PRESSLIGHT)*64);
	}

// set up bounds for processing this rectangle based on
// direction of dda
if ( x1 > x2 )
	{ sx = rData.right; ex = rData.left; }
else	{ sx = rData.left; ex = rData.right; }

if ( y1 > y2 )
	{ sy = rData.bottom; ey = rData.top; }
else	{ sy = rData.top; ey = rData.bottom; }

// set up start x,y and last x,y
x = sx;
y = sy;
lx = x;
ly = y;

// do dda until we hit endpoint
while (1)
	{
	// hit endpoint - get the hell out
	if (x == ex || y == ey)
		break;

	// increment our x and y based on dda
	if ( ( ax += dx ) > UNITY )
		{ ax -= UNITY; x += ix; }
	if ( ( ay += dy ) > UNITY )
		{ ay -= UNITY; y += iy; }

	// get change in x and y from last x and y
	deltax = x - lx;
	deltay = y - ly;
	
	if (dx > dy) // are we moving horizontally?
		{ // moving horizontally

		// smear a column of data to smearx from lx, if smearx is in
		// destination rectangle
		smearx = lx + deltax;
		if (smearx >= rDest.left && smearx <= rDest.right)
			{
			// get start address of where we are smearing from
			lpSrc = lpData + ((lx-rData.left)*depth); 

			// get start address of where we are smearing to
			smeary = rData.top + deltay;
			lpDst = lpData + ((smeary-rData.top)*width)+((smearx-rData.left)*depth);

			// get start of mask of where we are smearing to
			lpMsk = lpMask + ((smeary-ry)*rdx)+(smearx-rx);

			// smear of a column of data
			for (ty = rData.top; ty <= rData.bottom; ++ty)
				{
				// smear pixel at smearx,smeary if smeary
				// is in our destination rectangle
				smeary = ty + deltay;
				if (smeary >= rDest.top && smeary <= rDest.bottom)
					{
					// get mask value for this pixel
					m = *lpMsk;
					if (m) // process this pixel
						{
						// scale the smear percentage
						// by the mask value
						bs = (m * bsrc)>>8;
						// inc for divide by 256
						if (bs > 127)
							++bs;
						bd = 256-bs;
						// avergae source and dest
						frame_getRGB(lpSrc, &srgb);
						frame_getRGB(lpDst, &drgb);
						drgb.red = (((WORD)srgb.red * bs) + ((WORD)drgb.red * bd))>>8;
						drgb.green = (((WORD)srgb.green * bs) + ((WORD)drgb.green * bd))>>8;
						drgb.blue = (((WORD)srgb.blue * bs) + ((WORD)drgb.blue * bd))>>8;
						frame_putRGB(lpDst, &drgb);
						}
					}
				// advance pointers by line width
				lpSrc += width;
				lpDst += width;
				lpMsk += rdx;
				}
			}
		}
	else
		{ // moving vertically
		// smear a row of data to smeary from ly, if smeary is in
		// destination rectangle
		smeary = ly + deltay;
		if (smeary >= rDest.top && smeary <= rDest.bottom)
			{
			// get start address of where we are smearing from
			lpSrc = lpData + ((ly-rData.top)*width);

			// get start address of where we are smearing to
			smearx = rData.left + deltax;
			lpDst = lpData + ((smeary-rData.top)*width)+((smearx-rData.left)*depth);
			// get start of mask of where we are smearing to
			lpMsk = lpMask + ((smeary-ry)*rdx) + (smearx-rx);

			// smear of a row of data
			for (tx = rData.left; tx <= rData.right; ++tx)
				{
				// smear pixel at smearx,smeary if smearx
				// is in our destination rectangle
				smearx = tx + deltax;
				if (smearx >= rDest.left && smearx <= rDest.right)
					{
					// get mask value for this pixel
					m = *lpMsk; 
					if (m) // process this pixel
						{
						// scale the smear percentage
						// by the mask value
						bs = (m * bsrc)>>8;
						// inc for divide by 256
						if (bs > 127)
							++bs;
						bd = 256-bs;
						// avergae source and dest
						frame_getRGB(lpSrc, &srgb);
						frame_getRGB(lpDst, &drgb);
						drgb.red = (((WORD)srgb.red * bs) + ((WORD)drgb.red * bd))>>8;
						drgb.green = (((WORD)srgb.green * bs) + ((WORD)drgb.green * bd))>>8;
						drgb.blue = (((WORD)srgb.blue * bs) + ((WORD)drgb.blue * bd))>>8;
						frame_putRGB(lpDst, &drgb);
						}
					}
				// advance pointers by pixel size
				lpSrc += depth;
				lpDst += depth;
				++lpMsk;
				}
			}
		}
	// set last x and y
	lx = x;
	ly = y;
	}
// merge smeared data into the source buffer
if (AstralIntersectRect(&rSect, &rDest, &rData))
	{
	MergeData(lpSource, &rDest, RectWidth(&rDest), lpData, &rData, RectWidth(&rData), &rSect);
	}
FreeUp(lpData);
// save rectangle of last source buffer
rSource = rDest;
first = FALSE;
fStart = FALSE;
}

// Masking statics
static int vx, vy;
static int begin_x, begin_y, begin_fx, begin_fy;
static int veccnt;
static LPPOINT vecptr;
static BOOL maskWaitMouseUp;

/************************************************************************/
int FreeProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
static BOOL bAutoMask;
int x, y, i;
LPMASK	lpMask;
LPPOINT	points;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if (Control.Function == IDC_DRAWFREE || Control.Function == IDC_PENCIL)
		{
		if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
			break;
		lpImage->dirty = ( Control.Function == IDC_DRAWFREE ?
			IDS_UNDODRAWFREE : IDS_UNDOPENCIL );
		SetupMask();
		}
	Display2File(&x, &y);
	Window.ToolActive = YES;

	maskWaitMouseUp = FALSE;
	vecptr = (LPPOINT)LineBuffer[0];
	veccnt = 0;
	vx = begin_fx = x;
	vy = begin_fy = y;
	File2Display(&vx, &vy);
	/* Draw a guide line from the last point to the cursor */
	begin_x = gx = vx; 
	begin_y = gy = vy;
	Dline( 0, vx, vy, gx, gy, ON );
	lpImage->lpDisplay->FloatingGraphicsProc = MaskRedrawProc;
	break;

    case WM_LBUTTONDOWN:
	ClientRect = lpImage->lpDisplay->DispRect;
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	Display2File(&x, &y);
	bAutoMask = ( Mask.AutoMask ^ SHIFT );
	if ( bAutoMask && veccnt)
		automask(x, y, &vecptr, &veccnt, &vx, &vy, &gx, &gy);
	else	vector_draw( x, y, YES );
	break;

    case WM_LBUTTONUP:
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	Display2File(&x, &y);
	if ( maskWaitMouseUp )
		{
		maskWaitMouseUp = FALSE;
		vector_draw( x, y, NO );
		}
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	Display2File(&x, &y);
	vector_draw( x, y, Window.fButtonDown );
	break;

    case WM_DESTROY:	// The cancel operation message
	if (lParam == 1) /* cancel function */
	    	{
		Window.ToolActive = NO;
		lpImage->lpDisplay->FloatingGraphicsProc = NULL;
    		/* Undraw the guide line */
		Dline( 0, vx, vy, gx, gy, OFF );
		/* Draw the final segment */
		Dline( 0, vx, vy, begin_x, begin_y, ON );
		/* Undraw the shape */
		MaskUndraw( (LPPOINT)LineBuffer[0], &veccnt, 1, YES );
		break;
		}
    	/* fall through to ending function */
    case WM_LBUTTONDBLCLK:
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	Display2File(&x, &y);
	Window.ToolActive = NO;
	lpImage->lpDisplay->FloatingGraphicsProc = NULL;

	if (/*--*/veccnt <= 1)	// Delete the last vector because double click
		break;		// added an invalid point

    	/* Undraw the guide line */
	Dline( 0, vx, vy, gx, gy, OFF );
	if ( bAutoMask )
		automask(begin_fx, begin_fy, &vecptr, &veccnt,
			&vx, &vy, &gx, &gy);
	else	{
		/* Draw the final segment */
		Dline( 0, vx, vy, begin_x, begin_y, ON );
		}

	MaskUndraw( (LPPOINT)LineBuffer[0], &veccnt, 1, YES );
	if (Control.Function == IDC_DRAWFREE)
		{
		lpMask = mask_create( NULL, (LPPOINT)LineBuffer[0],
						&veccnt, 1, NO );
		if (lpMask)
			{
			DrawShape( lpMask, NO );
			mask_close( lpMask );
			}
		}
	else if (Control.Function == IDC_PENCIL)
		{
		lpMask = mask_create( NULL, (LPPOINT)LineBuffer[0],
						&veccnt, 1, NO );
		if (lpMask)
			{
			DrawShape( lpMask, YES );
			mask_close( lpMask );
			}
		}
	else	{
		SetNewMask( (LPPOINT)LineBuffer[0], &veccnt, 1, NO );
		}
	UpdateStatusBar( YES, NO, YES, NO );
	Marquee(YES);
	break;

    case WM_COMMAND:
	if (lParam == IDM_BACKUP)
	    MaskFreeBackup();
	break;
    }
}

/************************************************************************/
int MaskRedrawProc(hDC, lpRect, on )
/************************************************************************/
HDC hDC;
LPRECT lpRect;
BOOL on;
{
int iCount, x1, y1, x2, y2;
LPPOINT lpPoint;

lpPoint = (LPPOINT)LineBuffer[0];
iCount = veccnt;
while (--iCount >= 0)
	{
	x1 = lpPoint->x;
	y1 = lpPoint->y;
	++lpPoint;
	if (iCount)
		{
		x2 = lpPoint->x;
		y2 = lpPoint->y;
		}
	else
		{
		lpPoint = (LPPOINT)LineBuffer[0];
		x2 = lpPoint->x;
		y2 = lpPoint->y;
		}
	File2Display(&x1, &y1);
	File2Display(&x2, &y2);
	Dline( hDC, x1, y1, x2, y2, ON );  /* draw the vector */
	}
Dline( hDC, vx, vy, gx, gy, ON );  /* draw the guide line */
}

/************************************************************************/
void vector_draw( x, y, bCapture )
/************************************************************************/
int x, y;
BOOL bCapture;
{
int message, newx, newy;

if ( maskWaitMouseUp )
	return;

newx = x; newy = y;
File2Display(&newx, &newy);
if ( !bCapture )
	{
	/* if the cursor didn't move... */
	if ( newx == gx && newy == gy )
		return;
	/* Undraw the guide line ... */
	Dline( 0, vx, vy, gx, gy, OFF );
	/* .. and draw a new one */
	gx = newx; gy = newy;
	Dline( 0, vx, vy, gx, gy, ON );
	return;
	}

/* Undraw the guide line ... */
Dline( 0, vx, vy, gx, gy, OFF );
Dline( 0, vx, vy, newx, newy, ON );  /* draw the new vector */
/* Capture the new vector */
vecptr->x = x;
vecptr->y = y;
vecptr++;
veccnt++;
vx = gx = newx; vy = gy = newy;
Dline( 0, vx, vy, gx, gy, ON );  /* draw the guide line */

#ifdef UNUSED
message = 0;
if ( (x-1) <= lpImage->disp_x_start &&
	 lpImage->file_x_start > 0 )
	message = IDM_SCROLLLEFT;
else
if ( (x+1) >= lpImage->disp_x_end &&
	 lpImage->file_x_end < (lpImage->npix-1) )
	message = IDM_SCROLLRIGHT;
else
if ( (y-1) <= lpImage->disp_y_start &&
	 lpImage->file_y_start > 0 )
	message = IDM_SCROLLUP;
else
if ( (y+1) >= lpImage->disp_y_end &&
	 lpImage->file_y_end < (lpImage->nlin-1) )
	message = IDM_SCROLLDOWN;
if (message)
	{
	HandleMaskScroll(message);
	maskWaitMouseUp = TRUE;
	}
#endif
}

/************************************************************************/
void MaskUndraw( lpPoint, npoints, nShapes, fFileUnits)
/************************************************************************/
LPPOINT lpPoint;
LPINT npoints;
int nShapes;
BOOL fFileUnits;
{
int count, i;
LPPOINT lpNextPoint, lpFirstPoint;
int x1, y1, x2, y2;
lpFirstPoint = lpPoint;
for (i = 0; i < nShapes; ++i)
	{
	count = npoints[i];
	while (--count >= 0)
		{
		if (count)
			lpNextPoint = lpPoint + 1;
		else	lpNextPoint = lpFirstPoint;
		x1 = lpPoint->x; y1 = lpPoint->y;
		x2 = lpNextPoint->x; y2 = lpNextPoint->y;
		if (fFileUnits)
		    {
		    File2Display(&x1, &y1);
		    File2Display(&x2, &y2);
		    }
		Dline(0, x1, y1, x2, y2, OFF);
		++lpPoint;
		}
	}
}

/************************************************************************/
void MaskFreeBackup()
/************************************************************************/
{
int x2, y2;

if (lpImage && Window.ToolActive)
    {
    /* Undraw the guide line ... */
    Dline( 0, vx, vy, gx, gy, OFF );
    if (veccnt <= 1)
	Window.ToolActive = NO;
    else
        {
	vecptr--; veccnt--;
	/* Undraw the vector */
	
	vx = (vecptr - 1)->x; vy = (vecptr - 1)->y;
	File2Display(&vx, &vy);
	x2 = vecptr->x; y2 = vecptr->y;
	File2Display(&x2, &y2);
	Dline( 0, vx, vy, x2, y2, OFF);
	/* Draw the guide line */
	Dline( 0, vx, vy, gx, gy, ON);
	}
    }
}


/************************************************************************/
int CircleProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, dx, dy;
POINT Points[20];
LPMASK	lpMask;
int	nPoints;
POINT	deltas, mids;
long	AspectX, AspectY;
BOOL	fConstrain;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if (Control.Function == IDC_DRAWCIRCLE)
		{
		if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
			break;
		lpImage->dirty = IDS_UNDODRAWCIRCLE;
		SetupMask();
		}
	Window.ToolActive = YES;
	break;

    case WM_LBUTTONDOWN:
	ClientRect = lpImage->lpDisplay->DispRect;
	StartPoint = MAKEPOINT(lParam);
	SetRectEmpty( &SelectRect );
	StartSelection( hWindow, NULL, StartPoint, &SelectRect,
		SL_ELLIPSE | SL_SPECIAL );
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if (Control.Function == IDC_DRAWCIRCLE)
		fConstrain = NO;
	else	fConstrain = Mask.ConstrainCircleAspect;
	if ( fConstrain ^ SHIFT )
		{ // Constrain it
		AspectX = FMUL (100, Mask.ConstrainCircleWidth);
		AspectY = FMUL (100, Mask.ConstrainCircleHeight);
		dx = abs( StartPoint.x - x );
		dy = abs( StartPoint.y - y );
		if ( AspectY * dx < AspectX * dy )
			{
			y = StartPoint.y + 
			  (AspectY * ((y > StartPoint.y) ? dx : -dx)) / AspectX;
			}
		else	{
			x = StartPoint.x + 
			  (AspectX * ((x > StartPoint.x) ? dy : -dy)) / AspectY;
			}
		}
	lParam = MAKELONG( x, y );
	EndSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect,SL_ELLIPSE);
	InvertSelection( hWindow, NULL, &SelectRect, SL_ELLIPSE );
	deltas.x = ( 2 * (SelectRect.right - SelectRect.left)) / 9;
	deltas.y = ( 2 * (SelectRect.bottom - SelectRect.top)) / 9;
	mids.x = (SelectRect.left + SelectRect.right ) / 2;
	mids.y = (SelectRect.top + SelectRect.bottom ) / 2;

/* Upper/Right */
	Points[0].x = BEZIER_MARKER;
	Points[0].y = 4;
	Points[1].x = mids.x;
	Points[1].y = SelectRect.top;

	Points[2].x = SelectRect.right - deltas.x;
	Points[2].y = SelectRect.top;

	Points[3].x = SelectRect.right;
	Points[3].y = SelectRect.top + deltas.y;

	Points[4].x = SelectRect.right;
	Points[4].y = mids.y;

/* Lower/Right */
	Points[5].x = BEZIER_MARKER;
	Points[5].y = 4;
	Points[6].x = SelectRect.right;
	Points[6].y = mids.y;

	Points[7].x = SelectRect.right;
	Points[7].y = SelectRect.bottom - deltas.y;

	Points[8].x = SelectRect.right - deltas.x;
	Points[8].y = SelectRect.bottom;

	Points[9].x = mids.x;
	Points[9].y = SelectRect.bottom;

/* Lower/Left */
	Points[10].x = BEZIER_MARKER;
	Points[10].y = 4;
	Points[11].x = mids.x;
	Points[11].y = SelectRect.bottom;

	Points[12].x = SelectRect.left + deltas.x;
	Points[12].y = SelectRect.bottom;

	Points[13].x = SelectRect.left;
	Points[13].y = SelectRect.bottom - deltas.y;

	Points[14].x = SelectRect.left;
	Points[14].y = mids.y;

/* Upper/Left */
	Points[15].x = BEZIER_MARKER;
	Points[15].y = 4;
	Points[16].x = SelectRect.left;
	Points[16].y = mids.y;

	Points[17].x = SelectRect.left;
	Points[17].y = SelectRect.top + deltas.y;

	Points[18].x = SelectRect.left + deltas.x;
	Points[18].y = SelectRect.top;

	Points[19].x = mids.x;
	Points[19].y = SelectRect.top;

	if (Control.Function == IDC_DRAWCIRCLE)
		{
		nPoints = MaskDisp2File( Points, 20, NO );
		lpMask = mask_create( NULL, Points, &nPoints, 1, NO );
		if (lpMask)
			{
			DrawShape( lpMask, NO );
			mask_close( lpMask );
			}
		}
	else	{
		SetDrawnMask( Points, 20, NO /* Circle */ );
		}
	UpdateStatusBar( YES, NO, YES, NO );
	Marquee(YES);
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if (Control.Function == IDC_DRAWCIRCLE)
		fConstrain = NO;
	else	fConstrain = Mask.ConstrainCircleAspect;
	if ( fConstrain ^ SHIFT )
		{ // Constrain it
		AspectX = FMUL (100, Mask.ConstrainCircleWidth);
		AspectY = FMUL (100, Mask.ConstrainCircleHeight);
		dx = abs( StartPoint.x - x );
		dy = abs( StartPoint.y - y );
		if ( AspectY * dx < AspectX * dy )
			{
			y = StartPoint.y + 
			  (AspectY * ((y > StartPoint.y) ? dx : -dx)) / AspectX;
			}
		else	{
			x = StartPoint.x + 
			  (AspectX * ((x > StartPoint.x) ? dy : -dy)) / AspectY;
			}
		}
	lParam = MAKELONG( x, y );
	UpdateSelection( hWindow,NULL,MAKEPOINT(lParam),&SelectRect,SL_ELLIPSE);
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	if ( !Window.ToolActive )
		break;
	Window.ToolActive = NO;
	InvertSelection( hWindow, NULL, &SelectRect, SL_ELLIPSE );
	break;
    }
}


/************************************************************************/
int SquareProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, dx, dy;
POINT Points[4];
LPMASK	lpMask;
int	nPoints;
long	AspectX, AspectY;
BOOL	fConstrain;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if (Control.Function == IDC_DRAWSQUARE)
		{
		if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
			break;
		lpImage->dirty = IDS_UNDODRAWSQUARE;
		SetupMask();
		}
	Window.ToolActive = YES;
	break;

    case WM_LBUTTONDOWN:
	ClientRect = lpImage->lpDisplay->DispRect;
	StartPoint = MAKEPOINT(lParam);
	SetRectEmpty( &SelectRect );
	StartSelection( hWindow, NULL, StartPoint, &SelectRect,
		SL_BOX | SL_SPECIAL );
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if (Control.Function == IDC_DRAWSQUARE)
		fConstrain = NO;
	else	fConstrain = Mask.ConstrainRectAspect;
	if ( fConstrain ^ SHIFT )
		{ // Constrain it
		AspectX = FMUL (100, Mask.ConstrainRectWidth);
		AspectY = FMUL (100, Mask.ConstrainRectHeight);
		dx = abs( StartPoint.x - x );
		dy = abs( StartPoint.y - y );
		if ( AspectY * dx < AspectX * dy )
			{
			y = StartPoint.y + 
			  (AspectY * ((y > StartPoint.y) ? dx : -dx)) / AspectX;
			}
		else	{
			x = StartPoint.x + 
			  (AspectX * ((x > StartPoint.x) ? dy : -dy)) / AspectY;
			}
		}
	lParam = MAKELONG( x, y );
	EndSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect, SL_BOX );
	InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
	Points[0].x = SelectRect.left;
	Points[0].y = SelectRect.top;
	Points[1].x = SelectRect.right;
	Points[1].y = SelectRect.top;
	Points[2].x = SelectRect.right;
	Points[2].y = SelectRect.bottom;
	Points[3].x = SelectRect.left;
	Points[3].y = SelectRect.bottom;
	if (Control.Function == IDC_DRAWSQUARE)
		{
		nPoints = MaskDisp2File( Points, 4, NO );
		lpMask = mask_create( NULL, Points, &nPoints, 1, NO );
		if (lpMask)
			{
			DrawShape( lpMask, NO );
			mask_close( lpMask );
			}
		}
	else	{
		SetDrawnMask( Points, 4, NO /* Circle */ );
		}
	UpdateStatusBar( YES, NO, YES, NO );
	Marquee(YES);
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if (Control.Function == IDC_DRAWSQUARE)
		fConstrain = NO;
	else	fConstrain = Mask.ConstrainRectAspect;
	if ( fConstrain ^ SHIFT )
		{ // Constrain it
		AspectX = FMUL (100, Mask.ConstrainRectWidth);
		AspectY = FMUL (100, Mask.ConstrainRectHeight);
		dx = abs( StartPoint.x - x );
		dy = abs( StartPoint.y - y );
		if ( AspectY * dx < AspectX * dy )
			{
			y = StartPoint.y + 
			  (AspectY * ((y > StartPoint.y) ? dx : -dx)) / AspectX;
			}
		else	{
			x = StartPoint.x + 
			  (AspectX * ((x > StartPoint.x) ? dy : -dy)) / AspectY;
			}
		}
	lParam = MAKELONG( x, y );
	UpdateSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect, SL_BOX);
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	if ( !Window.ToolActive )
		break;
	Window.ToolActive = NO;
	InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
	break;
    }
}


/************************************************************************/
int CustomProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, dx, dy;
BOOL fGotaRect;
RECT WindowRect;
DWORD dXY;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	Window.ToolActive = YES;
	break;

    case WM_LBUTTONDOWN:
	ClientRect = lpImage->lpDisplay->DispRect;
	StartPoint = MAKEPOINT(lParam);
	SetRectEmpty( &SelectRect );
	StartSelection( hWindow, NULL, StartPoint, &SelectRect,
		SL_BOX | SL_SPECIAL );
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if ( SHIFT )
		{ // Constrain it to a square
		dx = abs( StartPoint.x - x );
		dy = abs( StartPoint.y - y );
		if ( dx < dy )
			y = StartPoint.y + ( y > StartPoint.y ? dx : -dx );
		else	x = StartPoint.x + ( x > StartPoint.x ? dy : -dy );
		}
	lParam = MAKELONG( x, y );
	EndSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect, SL_BOX );
	InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
	Display2File( &SelectRect.left, &SelectRect.top );
	Display2File( &SelectRect.right, &SelectRect.bottom );
	if ( abs( SelectRect.right - SelectRect.left ) <= SMALL_MOVEMENT &&
	     abs( SelectRect.bottom - SelectRect.top ) <= SMALL_MOVEMENT )
		fGotaRect = NO;
	else	fGotaRect = YES;

	if ( fGotaRect )
		ViewRect(&SelectRect);
	else	
		Zoom( SelectRect.left, SelectRect.top,
		      ( Control.ZoomOut ^ SHIFT ) ? -100 : +100, YES,
		      ( Control.ZoomWindow ^ CONTROL ) );
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if ( SHIFT )
		{ // Constrain it to a square
		dx = abs( StartPoint.x - x );
		dy = abs( StartPoint.y - y );
		if ( dx < dy )
			y = StartPoint.y + ( y > StartPoint.y ? dx : -dx );
		else	x = StartPoint.x + ( x > StartPoint.x ? dy : -dy );
		}
	lParam = MAKELONG( x, y );
	UpdateSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect, SL_BOX);
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	if ( !Window.ToolActive )
		break;
	Window.ToolActive = NO;
	InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
	break;
    }
}


/************************************************************************/
int MagicProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y;

x = LOWORD( lParam );
y = HIWORD( lParam );
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	Window.ToolActive = YES;

	Display2File(&x, &y);
	if ( INFILE(x, y) )
	    DoTracer( x, y );
	UpdateStatusBar( YES, NO, YES, NO );
	break;

    case WM_LBUTTONDOWN:
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	break;
    }
}


#ifdef NOTUSED
/************************************************************************/
int PencilProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y;

x = LOWORD( lParam );
y = HIWORD( lParam );
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	lpImage->dirty = IDS_UNDOPENCIL;
	Window.ToolActive = YES;
	SetupMask();
	break;

    case WM_LBUTTONDOWN:
	first = TRUE;
        onMask = (mask_active() && !Mask.IsOutside);
	//startx = x; starty = y; Display2File(&startx, &starty);
	//onMask = IsMasked(startx, starty);

    case WM_MOUSEMOVE:	// sent when ToolActive is on
        pencil_draw(x, y);
	break;

    case WM_LBUTTONUP:
	EngineDone ();
	Window.ToolActive = NO;
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	EngineDone ();
	Window.ToolActive = NO;
	break;
    }
}

/************************************************************************/
static void DoPencil(int x, int y)
/************************************************************************/
    {
    LPTR lpLine;
    RGBS rgb;
    LPRGB lpRGB;
    
    if (!INFILE(x, y))
        return;
    lpLine = CachePtr(0, x, y, YES);
    if (!lpLine)
        return;
    if (IsProtected(x, y, onMask, &rgb, lpLine))
         return;
    lpRGB = &Palette.ActiveRGB;
    frame_putRGB( lpLine, lpRGB );
    any_Brushed = TRUE;
    }
    
/************************************************************************/
static void pencil_draw(x, y)
/************************************************************************/
int x, y;
{
int fx, fy;

any_Brushed = FALSE;
fx = x; fy = y;
Display2File(&fx, &fy);
          
#ifdef OLD
if (first)
    {
    DoPencil(fx, fy);
    first = FALSE;
    sx = fx;
    sy = fy;
    }
else
    ddaline(sx, sy, fx, fy, DoPencil);
#else
if (first)
    {
    first = !EngineInit(FullBrush, FullBrush, NULL, lpImage->EditFrame, NULL,
	Retouch.Fadeout, 0, x, y, TRUE);
    sx = fx;
    sy = fy;
    }
else
    {
    ddaline(sx, sy, fx, fy, DrawShapeDot);
    any_Brushed = TRUE;
    }
#endif
SetRect(&StrokeRect, min(sx,fx), min(sy, fy), max(sx,fx), max(sy, fy));
sx = fx; sy = fy;
        
AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &StrokeRect);
if ( any_Brushed )
	UpdateImage( &StrokeRect, YES );
}
#endif


/************************************************************************/
int TransformerProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, dx, dy;
LPFRAME lpFrame;
LPMASK lpMask;
RECT maskRect;
TFORM EditTForm;
LPSHAPE lpShape, lpNextShape;
BOOL fGotaRect;
static LPSHAPE lpEditShape, lpPrevShape;
static BOOL down;

x = LOWORD( lParam );
y = HIWORD( lParam );
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	Window.ToolActive = YES;
	lpEditShape = NULL;
	break;

    case WM_LBUTTONDOWN:
	down = TRUE;
	if ( lpEditShape )
		{
		ShapeTransformMouseDown(x, y);
		break;
		}
	ClientRect = lpImage->lpDisplay->DispRect;
	StartPoint = MAKEPOINT(lParam);
	SetRectEmpty( &SelectRect );
	StartSelection( hWindow, NULL, StartPoint, &SelectRect,
		SL_BOX | SL_SPECIAL );
	break;

    case WM_LBUTTONUP:
	down = FALSE;
	if ( lpEditShape )
	    {
	    ShapeTransformMouseUp(x, y);
	    break;
	    }
	Window.ToolActive = NO;
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if ( SHIFT )
		{ // Constrain it to a square
		dx = abs( StartPoint.x - x );
		dy = abs( StartPoint.y - y );
		if ( dx < dy )
			y = StartPoint.y + ( y > StartPoint.y ? dx : -dx );
		else	x = StartPoint.x + ( x > StartPoint.x ? dy : -dy );
		}
	lParam = MAKELONG( x, y );
	EndSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect, SL_BOX );
	InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
	Display2File( &SelectRect.left, &SelectRect.top );
	Display2File( &SelectRect.right, &SelectRect.bottom );
	if ( abs( SelectRect.right - SelectRect.left ) <= SMALL_MOVEMENT &&
	     abs( SelectRect.bottom - SelectRect.top ) <= SMALL_MOVEMENT )
		fGotaRect = NO;
	else	fGotaRect = YES;

	if ( !(lpFrame = frame_set(NULL)) )
		break;
	if ( !(lpMask = lpFrame->WriteMask) )
		break;
	if ( fGotaRect )
		{
		if ( !(lpEditShape = ShapeMultiSelect(lpMask, &SelectRect)) )
			break;
		lpPrevShape = NULL;
		maskRect.top = 32767;
		maskRect.bottom = -32767;
		maskRect.left = 32767;
		maskRect.right = -32767;
		lpNextShape = lpEditShape;
		while (lpNextShape)
			{
			AstralUnionRect(&maskRect, &lpNextShape->rShape,
				&maskRect);
			lpNextShape = lpNextShape->lpNext;
			}
		}
	else 	{
		if ( !(lpEditShape = ShapeSelect(lpMask, SelectRect.left,
		     SelectRect.top)) )
			break;
		lpPrevShape = ShapeUnlink(lpMask, lpEditShape);
		maskRect = lpEditShape->rShape;
		}

	Window.ToolActive = YES;
	File2DispRect(&maskRect, &maskRect);
	PaintImage( Window.hDC, &maskRect, NULL );
	BuildMarquee();
        Marquee(YES);
	ShapeTransformInit(lpEditShape, NULL);
	ShapeTransformDraw(0, NULL, ON, -1);
	lpImage->lpDisplay->FloatingGraphicsProc = ShapeFloatProc;
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	if ( lpEditShape )
		{
		if (down)
			ShapeTransformMouseMove(x, y);
		break;
		}
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if ( SHIFT )
		{ // Constrain it to a square
		dx = abs( StartPoint.x - x );
		dy = abs( StartPoint.y - y );
		if ( dx < dy )
			y = StartPoint.y + ( y > StartPoint.y ? dx : -dx );
		else	x = StartPoint.x + ( x > StartPoint.x ? dy : -dy );
		}
	lParam = MAKELONG( x, y );
	UpdateSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect, SL_BOX);
	break;

    case WM_DESTROY:	// The cancel operation message
    case WM_LBUTTONDBLCLK:
	if ( !Window.ToolActive )
		break;
	Window.ToolActive = NO;
	if ( !lpEditShape )
		{
		InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
		Window.ToolActive = NO;
		break;
		}
	lpImage->lpDisplay->FloatingGraphicsProc = NULL;
	ShapeTransformDone();
	if ( !(lpFrame = frame_set(NULL)) )
		break;
	ShapeLink(lpFrame->WriteMask, lpEditShape, lpPrevShape);
	if ( !(lpMask = BuildEdges(lpFrame->WriteMask, NULL)) )
		{
		MessageBeep(0);
		break;
		}
	mask_link( lpMask, WRITE_MASK );
	BuildMarquee();
        Marquee(YES);
	break;

    case WM_COMMAND:
	if (lParam == IDM_DELETEITEM)
	    {
	    if (!lpEditShape)
		break;
	    Window.ToolActive = NO;
	    lpImage->lpDisplay->FloatingGraphicsProc = NULL;
	    ShapeTransformDone();
	    if ( !(lpFrame = frame_set(NULL)) )
		break;
	    FreeUpShapes(lpEditShape);
	    
	    lpMask = lpFrame->WriteMask;
	    if (!lpMask->lpHeadShape)
		{
		mask_unlink(lpMask, WRITE_MASK);
		mask_close(lpMask);
		}
    	    else if ( !(lpMask = BuildEdges(lpMask, NULL)) )
		{
		MessageBeep(0);
		break;
		}
	    else
		{
	        mask_link( lpMask, WRITE_MASK );
		}
	    BuildMarquee();
	    Marquee(YES);
	    }
        break;
    }
}

/************************************************************************/
int ShapeFloatProc(hDC, lpRect, on)
/************************************************************************/
HDC hDC;
LPRECT lpRect;
BOOL on;
{
ShapeTransformDraw(hDC, NULL, on, -1);
}

/************************************************************************/
int PointerProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, dx, dy;
LPFRAME lpFrame;
LPMASK lpMask;
RECT maskRect;
TFORM EditTForm;
LPSHAPE lpShape, lpNextShape;
BOOL fGotaRect;
static LPSHAPE lpEditShape, lpPrevShape;
static BOOL down;

x = LOWORD( lParam );
y = HIWORD( lParam );
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	Window.ToolActive = YES;
	lpEditShape = NULL;
	break;

    case WM_LBUTTONDOWN:
	down = TRUE;
	if ( lpEditShape )
		{
		ShapeEditMouseDown(x, y);
		break;
		}
	ClientRect = lpImage->lpDisplay->DispRect;
	StartPoint = MAKEPOINT(lParam);
	SetRectEmpty( &SelectRect );
	StartSelection( hWindow, NULL, StartPoint, &SelectRect,
		SL_BOX | SL_SPECIAL );
	break;

    case WM_LBUTTONUP:
	down = FALSE;
	if ( lpEditShape )
	    {
	    ShapeEditMouseUp(x, y);
	    break;
	    }
	Window.ToolActive = NO;
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if ( SHIFT )
		{ // Constrain it to a square
		dx = abs( StartPoint.x - x );
		dy = abs( StartPoint.y - y );
		if ( dx < dy )
			y = StartPoint.y + ( y > StartPoint.y ? dx : -dx );
		else	x = StartPoint.x + ( x > StartPoint.x ? dy : -dy );
		}
	lParam = MAKELONG( x, y );
	EndSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect, SL_BOX );
	InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
	Display2File( &SelectRect.left, &SelectRect.top );
	Display2File( &SelectRect.right, &SelectRect.bottom );
	if ( abs( SelectRect.right - SelectRect.left ) <= SMALL_MOVEMENT &&
	     abs( SelectRect.bottom - SelectRect.top ) <= SMALL_MOVEMENT )
		fGotaRect = NO;
	else	fGotaRect = YES;

	if ( !(lpFrame = frame_set(NULL)) )
		break;
	if ( !(lpMask = lpFrame->WriteMask) )
		break;
	if ( !(lpEditShape = ShapeSelect(lpMask, SelectRect.left,
	     SelectRect.top)) )
		break;
	lpPrevShape = ShapeUnlink(lpMask, lpEditShape);
	maskRect = lpEditShape->rShape;

	Window.ToolActive = YES;
	File2DispRect(&maskRect, &maskRect);
	PaintImage( Window.hDC, &maskRect, NULL );
	BuildMarquee();
        Marquee(YES);
	ShapeEditInit(lpEditShape);
	ShapeEditDraw(0, ON);
	lpImage->lpDisplay->FloatingGraphicsProc = ShapeEditProc;
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	if ( lpEditShape )
		{
		if (down)
			ShapeEditMouseMove(x, y);
		break;
		}
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if ( SHIFT )
		{ // Constrain it to a square
		dx = abs( StartPoint.x - x );
		dy = abs( StartPoint.y - y );
		if ( dx < dy )
			y = StartPoint.y + ( y > StartPoint.y ? dx : -dx );
		else	x = StartPoint.x + ( x > StartPoint.x ? dy : -dy );
		}
	lParam = MAKELONG( x, y );
	UpdateSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect, SL_BOX);
	break;

    case WM_DESTROY:	// The cancel operation message
    case WM_LBUTTONDBLCLK:
	if ( !Window.ToolActive )
		break;
	Window.ToolActive = NO;
	if ( !lpEditShape )
		{
		InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
		Window.ToolActive = NO;
		break;
		}
	lpImage->lpDisplay->FloatingGraphicsProc = NULL;
	ShapeEditDone();
	if ( !(lpFrame = frame_set(NULL)) )
		break;
	ShapeLink(lpFrame->WriteMask, lpEditShape, lpPrevShape);
	if ( !(lpMask = BuildEdges(lpFrame->WriteMask, NULL)) )
		{
		MessageBeep(0);
		break;
		}
	mask_link( lpMask, WRITE_MASK );
	BuildMarquee();
        Marquee(YES);
	break;

    case WM_COMMAND:
	if (lParam == IDM_DELETEITEM)
	    {
	    if (!lpEditShape)
		break;
	    Window.ToolActive = NO;
	    lpImage->lpDisplay->FloatingGraphicsProc = NULL;
	    ShapeEditDone();
	    if ( !(lpFrame = frame_set(NULL)) )
		break;
	    FreeUpShapes(lpEditShape);
	    
	    lpMask = lpFrame->WriteMask;
	    if (!lpMask->lpHeadShape)
		{
		mask_unlink(lpMask, WRITE_MASK);
		mask_close(lpMask);
		}
    	    else if ( !(lpMask = BuildEdges(lpMask, NULL)) )
		{
		MessageBeep(0);
		break;
		}
	    else
	        mask_link( lpMask, WRITE_MASK );
	    BuildMarquee();
	    }
        break;
    }
}

/************************************************************************/
int ShapeEditProc(hDC, lpRect, on)
/************************************************************************/
HDC hDC;
LPRECT lpRect;
BOOL on;
{
ShapeEditDraw(hDC, on);
}

/************************************************************************/
int GrabberProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, dx, dy, fx, fy, fxleft, fxright, fytop, fybottom;
RECT rRepair;
static POINT Anchor;
static BOOL down;

x = LOWORD( lParam );
y = HIWORD( lParam );
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	Window.ToolActive = YES;
	break;

    case WM_LBUTTONDOWN:
        down = TRUE;
	Anchor.x = x;
	Anchor.y = y;
        Display2File(&Anchor.x, &Anchor.y);
	break;

    case WM_LBUTTONUP:
	down = FALSE;
	Window.ToolActive = NO;
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
  	if (down)
	    {
	    Display2File(&x, &y);
	    fx = x - Anchor.x;
	    fy = y - Anchor.y;
            fxleft = lpImage->lpDisplay->FileRect.left;
	    fxright = lpImage->lpDisplay->FileRect.right-(lpImage->npix-1); 	    	    fytop = lpImage->lpDisplay->FileRect.top;
	    fybottom = lpImage->lpDisplay->FileRect.bottom-(lpImage->nlin-1);
            if (fx > 0 && fx > fxleft)
		fx = fxleft;
	    if (fx < 0 && fx < fxright)
		fx = fxright;
	    if (fy > 0 && fy > fytop)
		fy = fytop;
	    if (fy < 0 && fy < fybottom)
		fy = fybottom;
	    dx = FMUL( fx, lpImage->lpDisplay->DispRate );
	    dy = FMUL( fy, lpImage->lpDisplay->DispRate );
	    if (dy || dx)
		{
		lpImage->lpDisplay->FileRect.left -= fx;
		lpImage->lpDisplay->FileRect.right -= fx;
		lpImage->lpDisplay->xDiva -= dx;
		lpImage->lpDisplay->FileRect.top -= fy;
		lpImage->lpDisplay->FileRect.bottom -= fy;
		lpImage->lpDisplay->yDiva -= dy;
		ScrollWindow( hWindow, dx, dy, NULL, NULL );
		ValidateRect(hWindow, NULL);
		
		if (dx > 0)
		    {
		    rRepair.left = lpImage->lpDisplay->DispRect.left;
		    rRepair.right = rRepair.left + dx;
		    }
		else
		    {
		    rRepair.left = lpImage->lpDisplay->DispRect.right+dx;
		    rRepair.right = lpImage->lpDisplay->DispRect.right;
		    }
		if (dy > 0)
		    {
		    rRepair.top = lpImage->lpDisplay->DispRect.top + dy;
		    rRepair.bottom = lpImage->lpDisplay->DispRect.bottom;
		    }
		else
		    {
		    rRepair.top = lpImage->lpDisplay->DispRect.top;
		    rRepair.bottom = lpImage->lpDisplay->DispRect.bottom+dy;
		    }
		++rRepair.right; ++rRepair.bottom;
		InvalidateRect(hWindow, &rRepair, FALSE);
		AstralUpdateWindow( hWindow );
		if (dy > 0)
		    {
		    rRepair.top = lpImage->lpDisplay->DispRect.top;
		    rRepair.bottom = rRepair.top + dy;
		    }
		else
		    {
		    rRepair.top = lpImage->lpDisplay->DispRect.bottom+dy;
		    rRepair.bottom = lpImage->lpDisplay->DispRect.bottom;
		    }
		rRepair.left = lpImage->lpDisplay->DispRect.left;
		rRepair.right = lpImage->lpDisplay->DispRect.right;

		++rRepair.right; ++rRepair.bottom;
		InvalidateRect(hWindow, &rRepair, FALSE);
		AstralUpdateWindow( hWindow );
		Anchor.x = x-fx;
		Anchor.y = y-fy;
		}
	    }
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	break;
    }
}


/************************************************************************/
int EraserProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, fx, fy;
BOOL edited;

x = LOWORD( lParam );
y = HIWORD( lParam );
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
        if (lpImage->NoUndo)
            {
            MessageBeep(0);
	    Message(IDS_NEEDUNDO);
            break;
            }
        else if (lpImage->fNewFrame)
	    {
            MessageBeep(0);
	    Message(IDS_MUSTUSEUNDO);
	    break;
	    }
	else
            {
            edited = FALSE;
            if (lpImage->UndoFrame && lpImage->EditFlags)
                {
                for (fy = 0; fy < lpImage->EditFrame->Ysize; ++fy)
                    {
                    if (lpImage->EditFlags[fy] & LINE_EDITED)
                        {
                        edited = TRUE;
                        break;
                        }
                    }
                }
            if (!edited)
                {
                MessageBeep(0);
                Print("Can't use eraser tool until editing takes place");
                break;
                }
	    }
	Window.ToolActive = YES;
	SetupMask();
	if ( mask_active() )
		{
		Mask.IsOutside = !IsDisplayMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	break;

    case WM_LBUTTONDOWN:
	if (!EngineInit(FullBrush, FullBrush, lpImage->UndoFrame,
	 lpImage->EditFrame, NULL, Retouch.Fadeout, 0, x, y, TRUE))
  	    {
	    Window.ToolActive = NO;
	    break;
	    }
	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x; fy = y;
	Display2File(&fx, &fy);
	EngineStroke(fx, fy, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	EngineDone();
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	break;
    }
}

/************************************************************************/
int ProbeProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y;
RGBS rgb;
long lCount;
static BOOL fDoArea;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	Window.ToolActive = YES;
	break;

    case WM_LBUTTONDOWN:
	ClientRect = lpImage->lpDisplay->DispRect;
	fDoArea = (ColorMask.ProbeMethod == IDC_PROBERECT);
	if ( fDoArea ^ SHIFT )
		{
		StartPoint = MAKEPOINT(lParam);
		SetRectEmpty( &SelectRect );
		StartSelection( hWindow, NULL, StartPoint, &SelectRect,
			SL_BOX | SL_SPECIAL );
		}
	else	{
		SelectRect.left = SelectRect.right = x;
		SelectRect.top = SelectRect.bottom = y;
		if ( Measure( &SelectRect, &rgb, NO ) )
			{
			ColorStatus( &rgb, 0L );
			SetActiveColor( &rgb );
			}
		}
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if ( fDoArea )
		{
		lParam = MAKELONG( x, y );
		EndSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect,
			SL_BOX );
		InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
		lCount = Measure( &SelectRect, &rgb, NO );
		ColorStatus( &rgb, lCount );
		SetActiveColor( &rgb );
		}
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if ( fDoArea )
		{
		lParam = MAKELONG( x, y );
		UpdateSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect,
			SL_BOX);
		}
	else	{
		SelectRect.left = SelectRect.right = x;
		SelectRect.top = SelectRect.bottom = y;
		if ( Measure( &SelectRect, &rgb, NO ) )
			{
			ColorStatus( &rgb, 0L );
			SetActiveColor( &rgb );
			}
		}
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
        if ( !Window.ToolActive )
		break;
	Window.ToolActive = NO;
	if ( !fDoArea )
		break;
	InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
	break;
    }
}


/************************************************************************/
void SetActiveColor( lpRGB )
/************************************************************************/
LPRGB lpRGB;
{
RGBS rgb;
HWND hWnd;
int id;
COLOR Color;

rgb = *lpRGB;
Palette.ActiveRGB = rgb;
if ( hWnd = AstralDlgGet(IDD_TOOLS) )
	SendMessage( hWnd, WM_COMMAND, IDC_ACTIVECOLOR, 2L );
id = GetDlgCtrlID( hWnd = GetFocus() );
if ( id >= IDC_PALETTE001 && id <= IDC_PALETTE128 )
	{
	Palette.RGB[ id - IDC_PALETTE001 ] = rgb;
	CopyRGB( &rgb, &Color );
	SetWindowLong( hWnd, 0, Color );
	InvalidateRect( hWnd, NULL, FALSE );
	}
}


/************************************************************************/
int ShieldProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y;
HWND hWnd;
static BOOL fDoArea;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	Window.ToolActive = YES;
	break;

    case WM_LBUTTONDOWN:
	ClientRect = lpImage->lpDisplay->DispRect;
	fDoArea = (ColorMask.ProbeMethod == IDC_PROBERECT) ^ SHIFT;
	if ( fDoArea )
		{
		StartPoint = MAKEPOINT(lParam);
		SetRectEmpty( &SelectRect );
		StartSelection( hWindow, NULL, StartPoint, &SelectRect,
			SL_BOX | SL_SPECIAL );
		}
	else	{
		SelectRect.left = SelectRect.right = x;
		SelectRect.top = SelectRect.bottom = y;
		UpdateColorMask( &SelectRect );
		if ( hWnd = AstralDlgGet(IDD_SHIELDFLOAT) )
		    AstralControlRepaint( hWnd,
			IDC_MASKCOLOR1+ColorMask.MaskShield );
		}
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if ( fDoArea )
		{
		lParam = MAKELONG( x, y );
		EndSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect,
			SL_BOX );
		InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
		UpdateColorMask( &SelectRect );
		if ( hWnd = AstralDlgGet(IDD_SHIELDFLOAT) )
		    AstralControlRepaint( hWnd,
			IDC_MASKCOLOR1+ColorMask.MaskShield );
		}
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if ( fDoArea )
		{
		lParam = MAKELONG( x, y );
		UpdateSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect,
			SL_BOX);
		}
	else	{
		SelectRect.left = SelectRect.right = x;
		SelectRect.top = SelectRect.bottom = y;
		UpdateColorMask( &SelectRect );
		if ( hWnd = AstralDlgGet(IDD_SHIELDFLOAT) )
		    AstralControlRepaint( hWnd,
			IDC_MASKCOLOR1+ColorMask.MaskShield );
		}
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
        if ( !Window.ToolActive )
		break;
	Window.ToolActive = NO;
	if ( !fDoArea )
		break;
	InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
	break;
    }
}


/************************************************************************/
int TextProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, dx, dy;
BOOL fGotRect;

x = LOWORD( lParam );
y = HIWORD( lParam );
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	Window.ToolActive = YES;
	break;

    case WM_LBUTTONDOWN:
	ClientRect = lpImage->lpDisplay->DispRect;
	StartPoint = MAKEPOINT(lParam);
	SetRectEmpty( &SelectRect );
	StartSelection( hWindow, NULL, StartPoint, &SelectRect,
		SL_BOX | SL_SPECIAL );
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if ( SHIFT )
		{ // Constrain it to a square
		dx = abs( StartPoint.x - x );
		dy = abs( StartPoint.y - y );
		if ( dx < dy )
			y = StartPoint.y + ( y > StartPoint.y ? dx : -dx );
		else	x = StartPoint.x + ( x > StartPoint.x ? dy : -dy );
		}
	lParam = MAKELONG( x, y );
	EndSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect, SL_BOX );
	InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
	DoText(&SelectRect);
	UpdateStatusBar( YES, NO, YES, NO );
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	// Bound the point to the window
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );
	if ( SHIFT )
		{ // Constrain it to a square
		dx = abs( StartPoint.x - x );
		dy = abs( StartPoint.y - y );
		if ( dx < dy )
			y = StartPoint.y + ( y > StartPoint.y ? dx : -dx );
		else	x = StartPoint.x + ( x > StartPoint.x ? dy : -dy );
		}
	lParam = MAKELONG( x, y );
	UpdateSelection( hWindow, NULL, MAKEPOINT(lParam), &SelectRect, SL_BOX);
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
        if ( !Window.ToolActive )
		break;
	InvertSelection( hWindow, NULL, &SelectRect, SL_BOX );
	Window.ToolActive = NO;
	break;
    }
}


/************************************************************************/
int VignetteProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y;
static int xStart, yStart, xEnd, yEnd;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	lpImage->dirty = IDS_UNDOVIGNETTE;
	Window.ToolActive = YES;
	break;

    case WM_LBUTTONDOWN:
	xStart = x;
	yStart = y;
	xEnd = x;
	yEnd = y;
	Dline( 0, xStart, yStart, xEnd, yEnd, ON );
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	Dline( 0, xStart, yStart, xEnd, yEnd, OFF );
	xEnd = x;
	yEnd = y;
	Vignette( xStart, yStart, xEnd, yEnd );
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	Dline( 0, xStart, yStart, xEnd, yEnd, OFF );
	xEnd = x;
	yEnd = y;
	Dline( 0, xStart, yStart, xEnd, yEnd, ON );
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
        if (Window.ToolActive)
	    Dline( 0, xStart, yStart, xEnd, yEnd, OFF );
	Window.ToolActive = NO;
	break;
    }
}


/************************************************************************/
int TextureFillProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y, bpp, type, TileMode;
FNAME szFileName;

x = LOWORD( lParam );
y = HIWORD( lParam );
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	if (!LookupExtFile(Texture.Name, szFileName, IDN_TEXTURE))
		break;
	if ( lpTextureFrame )
		{
		if ( !StringsEqual( szFileName, szTextureName ) ||
			lpTextureFrame->Depth != DEPTH)
			{
			frame_close( lpTextureFrame );
			lpTextureFrame = NULL;
			}
		}
	lstrcpy( szTextureName, szFileName );
	if ( !lpTextureFrame )
		if ( !(lpTextureFrame = AstralFrameLoad(szTextureName, DEPTH, &bpp,
		     &type)) )
			break;
	lpImage->dirty = IDS_UNDOTEXTUREFILL;
	Window.ToolActive = YES;
	Display2File(&x, &y);
	if ( !INFILE(x, y) )
		break;
	if ( mask_active() )
		{
		Mask.IsOutside = !IsMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	TileMode = 1;
	if (Texture.fHorzFlip)
	    TileMode |= 2;
	if (Texture.fVertFlip)
	    TileMode |= 4;
	TextureFill( lpTextureFrame, TileMode );
	break;

    case WM_LBUTTONDOWN:
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	if (lpTextureFrame)
	    {
	    frame_close(lpTextureFrame);
	    lpTextureFrame = NULL;
	    }
	break;
    }
}


/************************************************************************/
int TintFillProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y;

x = LOWORD( lParam );
y = HIWORD( lParam );
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	lpImage->dirty = IDS_UNDOTINTFILL;
	Window.ToolActive = YES;
	Display2File(&x, &y);
	if ( !INFILE(x, y) )
		break;
	if ( mask_active() )
		{
		Mask.IsOutside = !IsMasked(x, y);
		UpdateStatusBar( NO, NO, YES, NO );
		}
	TintFill( &Palette.ActiveRGB );
	break;

    case WM_LBUTTONDOWN:
	break;

    case WM_LBUTTONUP:
	Window.ToolActive = NO;
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Window.ToolActive = NO;
	break;
    }
}


/************************************************************************/
void WinBrush( hDC, x, y, flag, newflag, fTickMarks )
/************************************************************************/
HDC hDC;
int x, y, flag, newflag, fTickMarks;
{
int size, size1, shape, y1, y2, x1, x2;
RECT rBrush;
HBRUSH hOldBrush;
short OldROP;

if ( !hDC )
	return;

if (newflag)
	{
	size = Retouch.BrushSize;
	shape = Retouch.BrushShape;
	}
else	{
	size = FullBrush;
	shape = LastShape;
	}

if (shape == IDC_BRUSHCUSTOM)
	shape = IDC_BRUSHSQUARE;
size = FMUL(size, lpImage->lpDisplay->DispRate);
size = (size+2)/2;
size1 = FMUL(1, lpImage->lpDisplay->DispRate);
size1 = (size1+2)/2;

OldROP = SetROP2(hDC, R2_NOT);
if ( shape == IDC_BRUSHCIRCLE )
	{
	x1 = x - size - 1;	x2 = x + size;
	y1 = y - size - 1;	y2 = y + size;
	hOldBrush = SelectObject( hDC, GetStockObject(NULL_BRUSH) );
	Ellipse( hDC, x1, y1, x2, y2 );
	SelectObject( hDC, hOldBrush );
	}
else
if ( shape == IDC_BRUSHSQUARE )
	{
	x1 = x - size - 1;	x2 = x + size;
	y1 = y - size - 1;	y2 = y + size;
	MoveTo( hDC, x1, y1 );
	LineTo( hDC, x2, y1 );
	LineTo( hDC, x2, y2 );
	LineTo( hDC, x1, y2 );
	LineTo( hDC, x1, y1 );
	}
else
if ( shape == IDC_BRUSHHORIZONTAL )
	{
	x1 = x - size - 1;	x2 = x + size;
	y1 = y - size1 - 1;	y2 = y + size1;
	MoveTo( hDC, x1, y1 );
	LineTo( hDC, x2, y1 );
	LineTo( hDC, x2, y2 );
	LineTo( hDC, x1, y2 );
	LineTo( hDC, x1, y1 );
	}
else
if ( shape == IDC_BRUSHVERTICAL )
	{
	x1 = x - size1 - 1;	x2 = x + size1;
	y1 = y - size - 1;	y2 = y + size;
	MoveTo( hDC, x1, y1 );
	LineTo( hDC, x2, y1 );
	LineTo( hDC, x2, y2 );
	LineTo( hDC, x1, y2 );
	LineTo( hDC, x1, y1 );
	}
else
if ( shape == IDC_BRUSHSLASH )
	{
	x1 = x - size - 1;	x2 = x + size;
	y1 = y - size - 1;	y2 = y + size;
	MoveTo( hDC, x2-size1, y1 );
	LineTo( hDC, x2, y1+size1 );
	LineTo( hDC, x1+size1, y2 );
	LineTo( hDC, x1, y2-size1 );
	LineTo( hDC, x2-size1, y1 );
	}
else
if ( shape == IDC_BRUSHBACKSLASH )
	{
	x1 = x - size - 1;	x2 = x + size;
	y1 = y - size - 1;	y2 = y + size;
	MoveTo( hDC, x1+size1, y1 );
	LineTo( hDC, x1, y1+size1 );
	LineTo( hDC, x2-size1, y2 );
	LineTo( hDC, x2, y2-size1 );
	LineTo( hDC, x1+size1, y1 );
	}

if ( !fTickMarks )
	{
	SetROP2(hDC, OldROP);
	return;
	}

if (shape == IDC_BRUSHCIRCLE)
	size = (size * 100) / 141;
x--;
y--;
MoveTo( hDC, x - size, y - size );
LineTo( hDC, x + size, y + size );
MoveTo( hDC, x + size, y - size );
LineTo( hDC, x - size, y + size );

SetROP2(hDC, OldROP);
}


/************************************************************************/
void CreateFuzzTable(fuzz, table_size, FuzzyPixels, Opacity)
/************************************************************************/
LPTR fuzz;
int table_size;
int FuzzyPixels;
int Opacity;
{
FIXED deltax;
FIXED deltax_table[3];
FIXED deltay_table[3];
FIXED xvalue, yvalue, height;
FIXED xstart, ystart;
FIXED x;
long rise_table[3], run_table[3], rise, run;
BYTE y;
int i, seg;

/* The edge of the fuzzy brush is created by generating a curve 
   representing the edge of the brush in deltax_table and
   deltay_table.  The three segments in the curve have the
   following slopes: 1/2, 2/1, 1/2.  This provides a smooth
   transition from the background image and to the full
   opacity of the brush. What may be confusing is that the curve
   starts and ends one pixel to the left of and one pixel to
   the right of the fuzzy edge.  It is assumed that one pixel
   to the left is the background image and one pixel to the right
   is the full opacity of the brush.  This means that there
   are (FuzzyPixels+2) x-coordinates in the curve and the 
   coordinates '-1' and 'FuzzyPixels' extend beyond the fuzzy
   edge of the brush. Linear interpolation is used to calculate
   the y-coordinates y-coordinate
   to the opacity range. Due to the slopes used the height of 
   the curve is 4/5 of the width (1+2+1/2+1+2) */

deltax = FGET(FuzzyPixels+1, 5);

if (Control.Function == IDC_SPRAY)
    {
    yvalue = 0;
    xvalue = FGET(-1, 1);
    rise_table[0] = 1;
    run_table[0] = 2;
    deltax_table[0] = xvalue + (run_table[0] * deltax);
    deltay_table[0] = yvalue + (rise_table[0] * deltax);
    rise_table[1] = 2;
    run_table[1] = 2;
    deltax_table[1] = deltax_table[0] + (run_table[1] * deltax);
    deltay_table[1] = deltay_table[0] + (rise_table[1] * deltax);
    rise_table[2] = 1;
    run_table[2] = 1;
    deltax_table[2] = deltax_table[1] + (run_table[2] * deltax);
    deltay_table[2] = deltay_table[1] + (rise_table[2] * deltax);
    }
else
    {
    yvalue = 0;
    xvalue = FGET(-1, 1);
    rise_table[0] = 1;
    run_table[0] = 2;
    deltax_table[0] = xvalue + (run_table[0] * deltax);
    deltay_table[0] = yvalue + (rise_table[0] * deltax);
    rise_table[1] = 2;
    run_table[1] = 1;
    deltax_table[1] = deltax_table[0] + (run_table[1] * deltax);
    deltay_table[1] = deltay_table[0] + (rise_table[1] * deltax);
    rise_table[2] = 1;
    run_table[2] = 2;
    deltax_table[2] = deltax_table[1] + (run_table[2] * deltax);
    deltay_table[2] = deltay_table[1] + (rise_table[2] * deltax);
    }
for (i = 0; i < FuzzyPixels; ++i)
	{
	x = FGET(i, 1) + 0;
	if (x <= deltax_table[0])
		{
		ystart = 0;
		xstart = FGET(-1, 1);
		seg = 0;
		}
	else
	if (x <= deltax_table[1])
		{
		ystart = deltay_table[0];
		xstart = deltax_table[0];
		seg = 1;
		}
	else	{
		ystart = deltay_table[1];
		xstart = deltax_table[1];
		seg = 2;
		}
	yvalue = ystart+(((long)(x-xstart)*rise_table[seg])/run_table[seg]);
	height = FGET(4 * (FuzzyPixels+1), 5);
	y = (BYTE)(((yvalue * (long)Opacity)+(long)(height/2)) / height);
	fuzz[i] = y;
	}

for (; i < table_size; ++i)
	fuzz[i] = Opacity;
}


/************************************************************************/
void SetupFuzzyBrush()
/************************************************************************/
{
int x, y, i;
long xd, yd, xc, yc, ydsquared, ydiff;
unsigned int ed;
PTR pBrush;
long x_tbl[MAX_BRUSH_SIZE+1];
static BYTE fuzz[MAX_BRUSH_SIZE];
static int fuzzOpacity = -1, fuzzFuzzyPixels = -1;

if (Retouch.FuzzyPixels <= 0 && Retouch.Opacity == 255)
	return;

pBrush = Retouch.BrushMask;
if (Retouch.FuzzyPixels == 0)
	{
	for (y=0; y<FullBrush; y++)
		{
		for (x=0; x<FullBrush; x++, ++pBrush)
			{
			if (*pBrush)
				*pBrush = Retouch.Opacity;
			}
		}
	return;
	}

if (Retouch.Opacity != fuzzOpacity || Retouch.FuzzyPixels != fuzzFuzzyPixels)
	CreateFuzzTable(fuzz, MAX_BRUSH_SIZE, Retouch.FuzzyPixels, Retouch.Opacity);
fuzzOpacity = Retouch.Opacity;
fuzzFuzzyPixels = Retouch.FuzzyPixels;
xc = (long)(FullBrush / 2);
yc = (long)(FullBrush / 2);
if (Retouch.BrushShape == IDC_BRUSHCIRCLE)
	{
	for (x=0; x<FullBrush; x++)
		{
		xd = abs(xc - x);
		x_tbl[x] = xd*xd;
		}
	for (y=0; y<FullBrush; y++)
		{
		yd = (long)abs(yc - y);
		ydsquared = yd*yd;
		for (x=0; x<FullBrush; x++, ++pBrush)
			{
			ed = -100;
			if (*pBrush)
				{
				ed = abs(xc - lsqrt(x_tbl[x]+ydsquared));
				*pBrush = fuzz[ed];
				}
			}
		}
	}
else	{
	for (x=0; x<FullBrush; x++)
		{
		xd = abs(xc - x);
		x_tbl[x] = abs(xc - xd);
		}

	for (y=0; y<FullBrush; y++)
		{
		yd = (long)abs(yc - y);
		ydiff = abs(yc - yd);
		for (x=0; x<FullBrush; x++, ++pBrush)
			{
			ed = -100;
			if (*pBrush)
				{
				if (x_tbl[x] < ydiff)
					ed = (unsigned int)abs(x_tbl[x]);
				else	ed = (unsigned int)ydiff;
				*pBrush = fuzz[ed];
				}
			}
		}
	}
}

/************************************************************************/
void CloneBrush(x, y, on)
/************************************************************************/
int x, y;
BOOL on;
{
static int bx, by;

if (on)
	{
	if ( !cdx && !cdy )
		{
		cdx = -(2 * FullBrush);
		cfdx = FMUL( cdx, lpImage->lpDisplay->FileRate );
		cfdy = FMUL( cdy, lpImage->lpDisplay->FileRate );
		}
	if ( SHIFT )
		{
		cdx += (bx - x);
		cdy += (by - y);
		cfdx = FMUL( cdx, lpImage->lpDisplay->FileRate );
		cfdy = FMUL( cdy, lpImage->lpDisplay->FileRate );
		}
	}
else
	cdx = cdy = 0;
bx = x;
by = y;
}

/************************************************************************/
void DisplayBrush(hWnd, x, y, on )
/************************************************************************/
HWND hWnd;
int x, y;
BOOL on;
{
HDC hDC;
static int bx, by, on_cdx, on_cdy;
static BOOL bBrushOn, bBrush2On, OnWhenOff;
static HWND hBrushWnd;

if ( on == bBrushOn )
	return;
if (on)
    {
    if (x == 32767)
	{
	if (!OnWhenOff)
	    return;
	x = bx;
	y = by;
	}
    hBrushWnd = hWnd;
    }
else
    {
    OnWhenOff = bBrushOn;
    hWnd = hBrushWnd;
    hBrushWnd = 0;
    }
if (!hWnd)
    return;
bBrushOn = on;
if ( on )
	{
	bBrush2On = ( Control.Function == IDC_CLONE );
	CloneBrush(x, y, bBrush2On);

	bx = x;
	by = y;
	on_cdx = FMUL( cfdx, lpImage->lpDisplay->DispRate );
	on_cdy = FMUL( cfdy, lpImage->lpDisplay->DispRate );
	}
hDC = GetDC(hWnd);
WinBrush( hDC, bx, by, bBrushOn, YES, 0 );
if ( bBrush2On )
	WinBrush( hDC, bx+on_cdx, by+on_cdy, bBrushOn, YES, 1 );
ReleaseDC(hWnd, hDC);
}
