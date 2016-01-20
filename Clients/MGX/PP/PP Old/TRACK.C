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

#define H_ONE	1
#define H_TWO  2
#define H_THREE 3
#define H_FOUR 4
#define H_FIVE 5
#define H_SIX 6
#define H_SEVEN 7
#define H_EIGHT 8
#define H_MOVE 16

#define NUM_HANDLES 8
#define H_SIZE 6
#define H_OFFSET (H_SIZE/2)

static int handle_grabbed, grabbed_x, grabbed_y, track_moved;
static LPROC lpTrackDrawRoutine;
static int TrackAspectW, TrackAspectH;
static BOOL TrackConstrainAspect, TrackConstrainXY, TrackRotateEnabled;
static BOOL TrackTransformMode;
static RECT TrackRect;
static RECT hRects[NUM_HANDLES];
static POINT hPoints[NUM_HANDLES];
static TFORM TrackTForm, FTrackTForm;
static int TrackAngleX, TrackAngleY;
static FIXED TrackScaleX, TrackScaleY;

/************************************************************************/
VOID TrackInit(tlpDrawRoutine, lpTForm, lpRect)
/************************************************************************/
LPROC tlpDrawRoutine;
LPTFORM lpTForm;
LPRECT lpRect;
{
POINT point;

lpTrackDrawRoutine = tlpDrawRoutine;
TrackTransformMode = lpTForm != NULL;
TrackRect = *lpRect;

TrackAngleX = TrackAngleY = 0;
TrackScaleX = TrackScaleY = UNITY;

if (TrackTransformMode)
	{
	FTrackTForm = *lpTForm;
	TrackTForm = FTrackTForm;
	File2DispTForm(&TrackTForm);
	point.x = TrackRect.left;
	point.y = TrackRect.top;
	Transformer(&TrackTForm, &point, &hPoints[H_ONE-1].x, &hPoints[H_ONE-1].y);
	point.x = TrackRect.right;
	point.y = TrackRect.bottom;
	Transformer(&TrackTForm, &point, &hPoints[H_THREE-1].x, &hPoints[H_THREE-1].y);
	}
else	{
	hPoints[H_ONE-1].x = TrackRect.left;
	hPoints[H_ONE-1].y = TrackRect.top;
	hPoints[H_THREE-1].x = TrackRect.right;
	hPoints[H_THREE-1].y = TrackRect.bottom;
	}

hPoints[H_TWO-1].x = hPoints[H_THREE-1].x;
hPoints[H_TWO-1].y = hPoints[H_ONE-1].y;
hPoints[H_FOUR-1].x = hPoints[H_ONE-1].x;
hPoints[H_FOUR-1].y = hPoints[H_THREE-1].y;

TrackSetHandles();
TrackConstrainAspect = FALSE;
}


/************************************************************************/
VOID TrackGrab(x, y)
/************************************************************************/
int x, y;
{
RECT rect;
int minx, maxx, miny, maxy;
int num_handles, i;
POINT Point;

TrackDraw(NULL, OFF);
track_moved = FALSE;
TrackConstrainAspect = FALSE;
if (TrackConstrainAspect)
	{
	TrackAspectW = 1;
	TrackAspectH = 1;
	}
TrackRotateEnabled = SHIFT;
grabbed_x = x;
grabbed_y = y;
//if (Control.Function == IDC_SQUARE && Mask.ConstrainRectAspect)
//	{
//	TrackAspectW = FMUL(lpImage->xres, Mask.ConstrainRectWidth);
//	TrackAspectH = FMUL(lpImage->xres, Mask.ConstrainRectHeight);
//	TrackConstrainAspect = TRUE;
//	}
//else
//if (Control.Function == IDC_CIRCLE && Mask.ConstrainCircleAspect)
//	{
//	TrackAspectW = FMUL(lpImage->xres, Mask.ConstrainCircleWidth);
//	TrackAspectH = FMUL(lpImage->xres, Mask.ConstrainCircleHeight);
//	TrackConstrainAspect = TRUE;
//	}
TrackDraw(NULL, ON);
TrackHandleMinMax(&minx, &maxx, &miny, &maxy);
rect.top = miny; rect.bottom = maxy;
rect.left = minx; rect.right = maxx;
rect.top += 5; rect.left += 5; rect.bottom -= 5; rect.right -= 5;
if (TrackConstrainAspect && !TrackTransformMode)
	num_handles = 4;
else	num_handles = NUM_HANDLES;
handle_grabbed = 0;
Point.x = x;
Point.y = y;
for (i = 0; i < num_handles; ++i)
	{
	if (PtInRect(&hRects[i], Point))
		{
		handle_grabbed = i + 1;
		break;
		}
	}
if (!handle_grabbed)
	if (PtInRect(&rect, Point))
		handle_grabbed = H_MOVE;
if (handle_grabbed >= H_FIVE && handle_grabbed <= H_EIGHT)
	TrackConstrainXY = TRUE;
else	
	TrackConstrainXY = CONTROL;
}

/************************************************************************/
BOOL TrackMove(x, y)
/************************************************************************/
int x, y;
{
int minx, maxx, miny, maxy, x_anchor, y_anchor, gx, gy, dx, dy;
RECT CropRect;
int width, height, new_width, new_height;

if (!handle_grabbed)
	return(FALSE);

track_moved = TRUE;
TrackDraw(NULL, OFF);
x = bound(x, lpImage->lpDisplay->DispRect.left, lpImage->lpDisplay->DispRect.right);
y = bound(y, lpImage->lpDisplay->DispRect.top, lpImage->lpDisplay->DispRect.bottom);
switch (handle_grabbed)
	{
	case H_ONE:
		if (TrackTransformMode)
			TrackTransform(x, y);
		else	{
			hPoints[H_ONE-1].x = x;
			hPoints[H_ONE-1].y = y;
			hPoints[H_TWO-1].y = y;
			hPoints[H_FOUR-1].x = x;
			x_anchor = hPoints[H_THREE-1].x;
			y_anchor = hPoints[H_THREE-1].y;
			}
		break;
	case H_TWO:
		if (TrackTransformMode)
			TrackTransform(x, y);
		else	{
			hPoints[H_TWO-1].x = x;
			hPoints[H_TWO-1].y = y;
			hPoints[H_ONE-1].y = y;
			hPoints[H_THREE-1].x = x;
			x_anchor = hPoints[H_FOUR-1].x;
			y_anchor = hPoints[H_FOUR-1].y;
			}
		break;
	case H_THREE:
		if (TrackTransformMode)
			TrackTransform(x, y);
		else	{
			hPoints[H_THREE-1].x = x;
			hPoints[H_THREE-1].y = y;
			hPoints[H_TWO-1].x = x;
			hPoints[H_FOUR-1].y = y;
			x_anchor = hPoints[H_ONE-1].x;
			y_anchor = hPoints[H_ONE-1].y;
			}
		break;
	case H_FOUR:
		if (TrackTransformMode)
			TrackTransform(x, y);
		else	{
			hPoints[H_FOUR-1].x = x;
			hPoints[H_FOUR-1].y = y;
			hPoints[H_ONE-1].x = x;
			hPoints[H_THREE-1].y = y;
			x_anchor = hPoints[H_TWO-1].x;
			y_anchor = hPoints[H_TWO-1].y;
			}
		break;
	case H_FIVE:
		if (TrackTransformMode)
			TrackTransform(x, y);
		else	{
			hPoints[H_ONE-1].y = y;
			hPoints[H_TWO-1].y = y;
			x_anchor = hPoints[H_THREE-1].x;
			y_anchor = hPoints[H_THREE-1].y;
			}
		break;
	case H_SIX:
		if (TrackTransformMode)
			TrackTransform(x, y);
		else	{
			hPoints[H_TWO-1].x = x;
			hPoints[H_THREE-1].x = x;
			x_anchor = hPoints[H_FOUR-1].x;
			y_anchor = hPoints[H_FOUR-1].y;
			}
		break;
	case H_SEVEN:
		if (TrackTransformMode)
			TrackTransform(x, y);
		else	{
			hPoints[H_THREE-1].y = y;
			hPoints[H_FOUR-1].y = y;
			x_anchor = hPoints[H_ONE-1].x;
			y_anchor = hPoints[H_ONE-1].y;
			}
		break;
	case H_EIGHT:
		if (TrackTransformMode)
			TrackTransform(x, y);
		else	{
			hPoints[H_ONE-1].x = x;
			hPoints[H_FOUR-1].x = x;
			x_anchor = hPoints[H_TWO-1].x;
			y_anchor = hPoints[H_TWO-1].y;
			}
		break;
	case H_MOVE:
		if (TrackTransformMode)
			TrackTransform(x, y);
		else	{
			TrackHandleMinMax(&minx, &maxx, &miny, &maxy);
			gx = grabbed_x;
			gy = grabbed_y;
			dx = x - gx;
			dy = y - gy;
			if (minx + dx < lpImage->lpDisplay->DispRect.left)
				dx += lpImage->lpDisplay->DispRect.left - (minx + dx);
			else if (maxx + dx > lpImage->lpDisplay->DispRect.right)
				dx -= (maxx + dx) - lpImage->lpDisplay->DispRect.right;
			if (miny + dy < lpImage->lpDisplay->DispRect.top)
				dy += lpImage->lpDisplay->DispRect.top - (miny + dy);
			else if (maxy + dy > lpImage->lpDisplay->DispRect.bottom)
				dy -= (maxy + dy) - lpImage->lpDisplay->DispRect.bottom;
			hPoints[H_ONE-1].x += dx; hPoints[H_TWO-1].x += dx;
			hPoints[H_THREE-1].x += dx; hPoints[H_FOUR-1].x += dx;
			hPoints[H_ONE-1].y += dy; hPoints[H_TWO-1].y += dy;
			hPoints[H_THREE-1].y += dy; hPoints[H_FOUR-1].y += dy;
			grabbed_x += dx;
			grabbed_y += dy;
			TrackSetHandles();
			}
		TrackDraw(NULL, ON);
		return(TRUE);

	default:
		break;
	}

if (!TrackTransformMode && handle_grabbed != H_MOVE)
	{
	TrackSetHandles();
	TrackHandleMinMax(&minx, &maxx, &miny, &maxy);
	if (x == minx)
		{
		if (y == miny)
			handle_grabbed = H_ONE;
		else if (y == maxy)
			handle_grabbed = H_FOUR;
		else	handle_grabbed = H_EIGHT;
		}
	else
	if (x == maxx)
		{
		if (y == miny)
			handle_grabbed = H_TWO;
		else if (y == maxy)
			handle_grabbed = H_THREE;
		else	handle_grabbed = H_SIX;
		}
	else	{
		if (y == miny)
			handle_grabbed = H_FIVE;
		else if (y == maxy)
			handle_grabbed = H_SEVEN;
		}

	if (TrackConstrainAspect)
		{
		width = maxx - minx + 1;
		height = maxy - miny + 1;

		new_width = ((height * TrackAspectW) + (TrackAspectH/2))/TrackAspectH;
		new_height = ((width * TrackAspectH) + (TrackAspectW/2))/TrackAspectW;

		if (new_width < width && FALSE)
			{
			if (x_anchor == minx)
				maxx = minx + new_width - 1;
			else	minx = maxx - new_width + 1;
			}
		else	{
			if (y_anchor == miny)
				maxy = miny + new_height - 1;
			else	miny = maxy - new_height + 1;
			}
		}
	if ((!((maxx - minx + 1) & 1)) && Control.Function == IDC_CIRCLE)
		{
		if (x_anchor == minx)
			--maxx;
		else	++minx;
		}
	if ((!((maxy - miny + 1) & 1)) && Control.Function == IDC_CIRCLE)
		{
		if (y_anchor == miny)
			--maxy;
		else	++miny;
		}
	hPoints[H_ONE-1].x = minx;
	hPoints[H_ONE-1].y = miny;
	hPoints[H_TWO-1].x = maxx;
	hPoints[H_TWO-1].y = miny;
	hPoints[H_THREE-1].x = maxx;
	hPoints[H_THREE-1].y = maxy;
	hPoints[H_FOUR-1].x = minx;
	hPoints[H_FOUR-1].y = maxy;
	TrackSetHandles();
	CropRect.left = minx; CropRect.right = maxx;
	CropRect.top = miny; CropRect.bottom = maxy;
	}

TrackDraw(NULL, ON);
return(TRUE);
}

/************************************************************************/
VOID TrackUp(x, y)
/************************************************************************/
int x, y;
{
TrackDraw(NULL, OFF);
//TrackConstrainAspect = 
//  (Control.Function == IDC_SQUARE && Mask.ConstrainRectAspect) ||
//  (Control.Function == IDC_CIRCLE && Mask.ConstrainCircleAspect);
TrackDraw(NULL, ON);
}

/************************************************************************/
VOID TrackDone(rect, undraw)
/************************************************************************/
RECT *rect;
BOOL undraw;
{
int minx, maxx, miny, maxy;

if ( undraw )
	TrackDraw(NULL, OFF);

TrackHandleMinMax(&minx, &maxx, &miny, &maxy);

rect->left = minx; rect->right = maxx;
rect->top = miny; rect->bottom = maxy;
}

/************************************************************************/
VOID TrackDraw(lpRect, on)
/************************************************************************/
LPRECT lpRect;
BOOL on;
{
(*lpTrackDrawRoutine)(0, lpRect, on, NUM_HANDLES );
}

/************************************************************************/
BOOL TrackMoved()
/************************************************************************/
{
return(track_moved);
}

/************************************************************************/
VOID TrackGetTForm(lpTForm)
/************************************************************************/
LPTFORM lpTForm;
{
*lpTForm = FTrackTForm;
}

/************************************************************************/
VOID TrackTransform(x, y)
/************************************************************************/
int x, y;
{
POINT point;
int dx, dy;
TFORM temp;

temp = TrackTForm;
switch (handle_grabbed)
	{
	case H_ONE:
		if (TrackRotateEnabled)
			{
			if (TrackConstrainXY)
				{
				TrackShear(x, y, hPoints[H_ONE-1].x, hPoints[H_ONE-1].y, TrackRect.left, TrackRect.top);
				}
			else	{
				TrackRotate(x, y, hPoints[H_ONE-1].x, hPoints[H_ONE-1].y);
				}
			}
		else	{
			TrackScale(x, y, hPoints[H_ONE-1].x, hPoints[H_ONE-1].y);
			}
		if (!TrackRotateEnabled)
			{
			point.x = TrackRect.right;
			point.y = TrackRect.bottom;
			Transformer(&TrackTForm, &point, &x, &y);
			dx = hPoints[H_THREE-1].x - x;
			dy = hPoints[H_THREE-1].y - y;
			TMove(&TrackTForm, dx, dy);
			}
		break;

	case H_TWO:
		if (TrackRotateEnabled)
			{
			if (TrackConstrainXY)
				{
				TrackShear(x, y, hPoints[H_TWO-1].x, hPoints[H_TWO-1].y, TrackRect.right, TrackRect.top);
				}
			else	{
				TrackRotate(x, y, hPoints[H_TWO-1].x, hPoints[H_TWO-1].y);
				}
			}
		else	{
			TrackScale(x, y, hPoints[H_TWO-1].x, hPoints[H_TWO-1].y);
			}
		if (!TrackRotateEnabled)
			{
			point.x = TrackRect.left;
			point.y = TrackRect.bottom;
			Transformer(&TrackTForm, &point, &x, &y);
			dx = hPoints[H_FOUR-1].x - x;
			dy = hPoints[H_FOUR-1].y - y;
			TMove(&TrackTForm, dx, dy);
			}
		break;

	case H_THREE:
		if (TrackRotateEnabled)
			{
			if (TrackConstrainXY)
				{
				TrackShear(x, y, hPoints[H_THREE-1].x, hPoints[H_THREE-1].y, TrackRect.right, TrackRect.bottom);
				}
			else	{
				TrackRotate(x, y, hPoints[H_THREE-1].x, hPoints[H_THREE-1].y);
				}
			}
		else	{
			TrackScale(x, y, hPoints[H_THREE-1].x, hPoints[H_THREE-1].y);
			}
		if (!TrackRotateEnabled)
			{
			point.x = TrackRect.left;
			point.y = TrackRect.top;
			Transformer(&TrackTForm, &point, &x, &y);
			dx = hPoints[H_ONE-1].x - x;
			dy = hPoints[H_ONE-1].y - y;
			TMove(&TrackTForm, dx, dy);
			}
		break;

	case H_FOUR:
		if (TrackRotateEnabled)
			{
			if (TrackConstrainXY)
				{
				TrackShear(x, y, hPoints[H_FOUR-1].x, hPoints[H_FOUR-1].y, TrackRect.left, TrackRect.bottom);
				}
			else	{
				TrackRotate(x, y, hPoints[H_FOUR-1].x, hPoints[H_FOUR-1].y);
				}
			}
		else	{
			TrackScale(x, y, hPoints[H_FOUR-1].x, hPoints[H_FOUR-1].y);
			}
		if (!TrackRotateEnabled)
			{
			point.x = TrackRect.right;
			point.y = TrackRect.top;
			Transformer(&TrackTForm, &point, &x, &y);
			dx = hPoints[H_TWO-1].x - x;
			dy = hPoints[H_TWO-1].y - y;
			TMove(&TrackTForm, dx, dy);
			}
		break;

	case H_FIVE:
		if (!TrackRotateEnabled)
			{
			x = hPoints[H_FIVE-1].x;
			TrackScale(x, y, hPoints[H_FIVE-1].x, hPoints[H_FIVE-1].y);
			point.x = (TrackRect.right+TrackRect.left)/2;
			point.y = TrackRect.bottom;
			Transformer(&TrackTForm, &point, &x, &y);
			dx = hPoints[H_SEVEN-1].x - x;
			dy = hPoints[H_SEVEN-1].y - y;
			TMove(&TrackTForm, dx, dy);
			}
		break;

	case H_SIX:
		if (!TrackRotateEnabled)
			{
			y = hPoints[H_SIX-1].y;
			TrackScale(x, y, hPoints[H_SIX-1].x, hPoints[H_SIX-1].y);
			point.x = TrackRect.left;
			point.y = (TrackRect.top+TrackRect.bottom)/2;
			Transformer(&TrackTForm, &point, &x, &y);
			dx = hPoints[H_EIGHT-1].x - x;
			dy = hPoints[H_EIGHT-1].y - y;
			TMove(&TrackTForm, dx, dy);
			}
		break;

	case H_SEVEN:
		if (!TrackRotateEnabled)
			{
			x = hPoints[H_SEVEN-1].x;
			TrackScale(x, y, hPoints[H_SEVEN-1].x, hPoints[H_SEVEN-1].y);
			point.x = (TrackRect.left+TrackRect.right)/2;
			point.y = TrackRect.top;
			Transformer(&TrackTForm, &point, &x, &y);
			dx = hPoints[H_FIVE-1].x - x;
			dy = hPoints[H_FIVE-1].y - y;
			TMove(&TrackTForm, dx, dy);
			}
		break;

	case H_EIGHT:
		if (!TrackRotateEnabled)
			{
			y = hPoints[H_EIGHT-1].y;
			TrackScale(x, y, hPoints[H_EIGHT-1].x, hPoints[H_EIGHT-1].y);
			point.x = TrackRect.right;
			point.y = (TrackRect.top+TrackRect.bottom)/2;
			Transformer(&TrackTForm, &point, &x, &y);
			dx = hPoints[H_SIX-1].x - x;
			dy = hPoints[H_SIX-1].y - y;
			TMove(&TrackTForm, dx, dy);
			}
		break;

	case H_MOVE:
		dx = x - grabbed_x;
		dy = y - grabbed_y;
		TMove(&TrackTForm, dx, dy);
		grabbed_x = x;
		grabbed_y = y;
		break;

	default:
		break;
	}
FTrackTForm = TrackTForm;
Disp2FileTForm(&FTrackTForm);
}

/************************************************************************/
VOID TrackRotate(x, y, hx, hy)
/************************************************************************/
int x, y;
int hx, hy;
{
RECT TFRect;
int dx, dy;
int old_angle, new_angle, angle;
POINT center;
int minx, maxx, miny, maxy;

TrackHandleMinMax(&minx, &maxx, &miny, &maxy);
center.x = (minx + maxx + 1)/2;
center.y = (miny + maxy + 1)/2;

TransformRect( &TrackTForm, &TrackRect, &TFRect );
dx = ( TFRect.left + TFRect.right ) / 2;
dy = ( TFRect.top + TFRect.bottom ) / 2;

TMove( &TrackTForm, -dx, -dy );
old_angle = TrackGetAngle(center, hx, hy);
new_angle = TrackGetAngle(center, x, y);
angle = old_angle - new_angle;
TRotate( &TrackTForm, angle, angle );
TMove(&TrackTForm, dx, dy);
TrackAngleX += angle;
TrackAngleY += angle;
if ( TrackAngleX != TrackAngleY )
	{
//	ReadOut1( "Angle X: ", TrackAngleX, FALSE, FALSE );
//	ReadOut2( "Angle Y: ", TrackAngleY, FALSE, FALSE );
	}
else	{
//	ReadOut1( "Angle: ", TrackAngleX, FALSE, FALSE );
//	ReadOut2( NULL, 0, FALSE, FALSE );
	}
}


/************************************************************************/
VOID TrackScale(x, y, hx, hy)
/************************************************************************/
int x, y;
int hx, hy;
{
int minx, maxx, miny, maxy, dx, dy, midx, midy;
int old_width, old_height, new_width, new_height, xdiff, ydiff;
FIXED scalex, scaley;

TrackHandleMinMax(&minx, &maxx, &miny, &maxy);
midx = (minx+maxx)/2;
midy = (miny+maxy)/2;
old_width = maxx - minx + 1;
old_height = maxy - miny + 1;

dx = x - hx;	/* positive is a move to the right */
dy = y - hy;	/* positive is a move to the bottom */

if (hx < midx)
	new_width = old_width - dx;
else 	new_width = old_width + dx;

if (hy < midy)
	new_height = old_height - dy;
else	new_height = old_height + dy;

if (new_width < 2)
	new_width = 2;
if (new_height < 2)
	new_height = 2; 
scalex = FGET(new_width, old_width);
scaley = FGET(new_height, old_height);
if (!TrackConstrainXY)
	{
	if (hx == minx || hx == maxx)
		scaley = scalex;
	else
	if (hy == miny || hy == maxy)
		scalex = scaley;
	else	{
		xdiff = min(abs(hx-minx), abs(hx-maxx));
		ydiff = min(abs(hy-miny), abs(hy-maxy));
		if (xdiff < ydiff)
			scaley = scalex;
		else	scalex = scaley;
		}
	}
TScale(&TrackTForm, scalex, scaley);
TrackScaleX = FMUL( TrackScaleX, scalex );
TrackScaleY = FMUL( TrackScaleY, scaley );
if ( TrackScaleX != TrackScaleY )
	{
//	ReadOut1F("Scale X: ", 100 * TrackScaleX, FALSE, FALSE );
//	ReadOut2F("Scale Y: ", 100 * TrackScaleY, FALSE, FALSE );
	}
else	{
//	ReadOut1F( "Scale: ", 100 * TrackScaleX, FALSE, FALSE  );
//	ReadOut2F( NULL, 0, FALSE, FALSE );
	}
}


/************************************************************************/
VOID TrackShear(x, y, hx, hy, ox, oy)
/************************************************************************/
int x, y;
int hx, hy;
int ox, oy;
{
int minx, maxx, miny, maxy, dx, dy, midx, midy, mx, my, nx, ny;
int old_width, old_height, new_width, new_height, length;
RECT TFRect;
FIXED scalex, scaley;
POINT point;

TransformRect( &TrackTForm, &TrackRect, &TFRect );
mx = ( TFRect.left + TFRect.right ) / 2;
my = ( TFRect.top + TFRect.bottom ) / 2;

TrackHandleMinMax(&minx, &maxx, &miny, &maxy);
midx = (minx + maxx)/2;
midy = (miny + maxy)/2;

dx = x - hx;	/* positive is a move to the right */
dy = y - hy;	/* positive is a move to the bottom */

if (ConstrainY())
	{
	if (hx == minx || hx == maxx)			/* left side */
		{
		if (hy > midy)	/* some bottom left corner */
			{
			length = hy - miny;
			}
		else	{			/* some top left corner */
			length = maxy - hy;
			dx = -dx;
			}
		}
	else if (hy == miny)	/* top side */
		{
		length = maxy - miny;
		dx = -dx;
		}
	else if (hy == maxy)   /* bottom side */
		{
		length = hy - miny;
		}
	else	{			/* the point is not an extreme */
		if (hy > midy)	/* some bottom right corner */
			{
			length = hy - miny;
			}
		else	{			/* some top right corner */
			length = maxy - hy;
			dx = -dx;
			}
		}
	TShearX(&TrackTForm, length, dx);
	}
else	{
	if (hy == miny || hy == maxy)			/* top side */
		{
		if (hx > midx)	/* some top right corner */
			{
			length = hx - minx;
			}
		else	{			/* some top left corner */
			length = maxx - hx;
			dy = -dy;
			}
		}
	else if (hx == minx)	/* left side */
		{
		length = maxx - hx;
		dy = -dy;
		}
	else if (hx == maxx)	/* right side */
		{
		length = hx - minx;
		}
	else	{
		if (hx > midx)	/* some bottom right corner */
			{
			length = hx - minx;
			}
		else	{			/* some bottom left corner */
			length = maxx - hx;
			dy = -dy;
			}
		}
	TShearY(&TrackTForm, length, dy);
	}

if (!ConstrainX())
	hx = x;
if (!ConstrainY())
	hy = y;
point.x = ox;
point.y = oy;
Transformer(&TrackTForm, &point, &nx, &ny);
mx = hx - nx;
my = hy - ny;
TMove(&TrackTForm, mx, my);
}


/************************************************************************/
VOID TrackDrawRect(lpRect, on, num_handles)
/************************************************************************/
LPRECT lpRect;
BOOL on;
int num_handles;
{
if (lpRect)
	Drect(0, lpRect, ON );
else	{
	Dline(0,  hPoints[H_ONE-1].x,   hPoints[H_ONE-1].y,
		hPoints[H_TWO-1].x,   hPoints[H_TWO-1].y,   ON );
	Dline(0,  hPoints[H_TWO-1].x,   hPoints[H_TWO-1].y,
		hPoints[H_THREE-1].x, hPoints[H_THREE-1].y, ON );
	Dline(0,	hPoints[H_THREE-1].x, hPoints[H_THREE-1].y,
		hPoints[H_FOUR-1].x,  hPoints[H_FOUR-1].y,  ON );
	Dline(0,	hPoints[H_FOUR-1].x,  hPoints[H_FOUR-1].y,
		hPoints[H_ONE-1].x,   hPoints[H_ONE-1].y,   ON );
	}
if (num_handles)
	TrackDrawHandles(0, lpRect, on, num_handles);
}


/************************************************************************/
VOID TrackDrawEllipse(lpRect, on, num_handles)
/************************************************************************/
LPRECT lpRect;
BOOL on;
int num_handles;
{
int xsize, ysize, x, y;

if (lpRect)
	{
	xsize = (lpRect->right - lpRect->left + 1) / 2;
	ysize = (lpRect->bottom - lpRect->top + 1) / 2;
	x = lpRect->left + xsize;
	y = lpRect->top + ysize;
	}
else	{
	xsize = (hPoints[H_TWO-1].x - hPoints[H_ONE-1].x + 1) / 2;
	ysize = (hPoints[H_FOUR-1].y - hPoints[H_ONE-1].y + 1) / 2;
	x = hPoints[H_ONE-1].x + xsize;
	y = hPoints[H_ONE-1].y + ysize;
	}

//ellipse_overlay( x, y, xsize, ysize, on );
if (num_handles)
	TrackDrawHandles(0, lpRect, on, num_handles);
}


/************************************************************************/
VOID TrackDrawClip(lpRect, on, num_handles)
/************************************************************************/
LPRECT lpRect;
BOOL on;
int num_handles;
{
//disp_mask( (LPPOINT)LineBuffer[0], &TrackTForm, on, NO );
if (num_handles)
	TrackDrawHandles(0, lpRect, on, num_handles);
}


/************************************************************************/
VOID TrackDrawHandles(hDC, lpRect, on, num_handles)
/************************************************************************/
HDC hDC;
LPRECT lpRect;
BOOL on;
int num_handles;
{
int i;

if (!hDC)
	hDC = Window.hDC;

if (TrackTransformMode)
	TrackResetHandles();

if (num_handles < 0)
	num_handles = NUM_HANDLES;
if (TrackConstrainAspect && !TrackTransformMode)
	num_handles = 4;
for (i = 0; i < num_handles; ++i)
	{
	hRects[i].left = hPoints[i].x - H_OFFSET;
	hRects[i].top = hPoints[i].y - H_OFFSET;
	hRects[i].right = hRects[i].left + H_SIZE;
	hRects[i].bottom = hRects[i].top + H_SIZE;
	TrackDrawHandle(hDC, &hRects[i], on);
	}
}


/************************************************************************/
VOID TrackDrawHandle(hDC, lpRect, on)
/************************************************************************/
HDC hDC;
LPRECT lpRect;
BOOL on;
{
int x1, x2, y;

if (!hDC)
	hDC = Window.hDC;

if (lpRect->right < lpImage->lpDisplay->DispRect.left || lpRect->left > lpImage->lpDisplay->DispRect.right)
	return;
if (lpRect->bottom < lpImage->lpDisplay->DispRect.top || lpRect->top > lpImage->lpDisplay->DispRect.bottom)
	return;
for (y = lpRect->top; y <= lpRect->bottom; ++y)
	{
	if (y >= lpImage->lpDisplay->DispRect.top & y <= lpImage->lpDisplay->DispRect.bottom)
		{
		x1 = bound(lpRect->left, lpImage->lpDisplay->DispRect.left, lpImage->lpDisplay->DispRect.right);
		x2 = bound(lpRect->right+1, lpImage->lpDisplay->DispRect.left, lpImage->lpDisplay->DispRect.right);
		Dline(hDC, x1, y, x2, y, ON);
		}
	}
}


/************************************************************************/
VOID TrackHandleMinMax(minx, maxx, miny, maxy)
/************************************************************************/
LPINT minx, maxx, miny, maxy;
{
int i, num_handles;

if (TrackConstrainAspect)
	num_handles = 4;
else	num_handles = NUM_HANDLES;

*minx = 32767;
*maxx = -32767;
*miny = 32767;
*maxy = -32767;
for (i = 0; i < num_handles; ++i)
	{
	*minx = min(*minx, hPoints[i].x);
	*maxx = max(*maxx, hPoints[i].x);
	*miny = min(*miny, hPoints[i].y);
	*maxy = max(*maxy, hPoints[i].y);
	}
}


/************************************************************************/
VOID TrackCalcShearDxDy(dx, dy, thePoint, Point1, Point2)
/************************************************************************/
LPINT dx, dy;
POINT thePoint, Point1, Point2;
{
int dx1, dy1, dx2, dy2, maxx, maxy;

dx1 = thePoint.x - Point1.x;
dy1 = thePoint.y - Point1.y;
dx2 = thePoint.x - Point2.x;
dy2 = thePoint.y - Point2.y;
maxx = max(dx1, dx2);
maxy = max(dy1, dy2);
if (dx > dy)
	{
	if (abs(dy1) < abs(dy2))	/* closer to Point1 in y, so assume */
		{			/* thePoint is right or left of Point1*/
		if (dx1 < 0)		/* thePoint is left of Point 1 */
			*dx = -*dx;	/* for left side dx must be negative */
		if (dy2 < 0)		/* thePoint is above Point 2 */ 
			*dx = -*dx;	/* for above dx must be negative */
		}
	else				/* closer to Point2 in y */
		{			/* so lets assume thePoint is right or left of Point2 */
		if (dx2 < 0)		/* thePoint if left of Point 2 */
			*dx = -*dx;	/* for left side dx must be negative */
		if (dy1 < 0)		/* thePoint is above Point 1 */
			*dx = -*dx;	/* for above dx must be negative */
		}
	}
else	{
	if (abs(dx1) < abs(dx2))	/* closer to Point1 in x, so assume */
		{			/* thePoint is above or below Point1 */
		if (dy1 < 0)		/* thePoint is above Point 1 */
			*dx = -*dx;	/* for above dx must be negative */
		if (dx2 < 0)		/* thePoint is left of Point 2 */
			*dx = -*dx;	/* for left side dx must be negative */
		}
	else	{
		if (dy2 < 0)		/* thePoint is above Point 2 */
			*dx = -*dx;	/* for above dx must be negative */
		if (dx1 < 0)		/* thePoint is left of Point 1 */
			*dx = -*dx;	/* for left side dx must be negative */
		}
	}
}


/************************************************************************/
int TrackGetAngle(center, x, y)
/************************************************************************/
POINT center;
int x, y;
{
int dx, dy;
double radians, angle;
double atan2(double, double);
#define PI 3.14159265

dx = x - center.x;
dy = center.y - y;

radians = atan2((double)dy, (double)dx);
angle = (radians * 180.0) / PI;
if (angle < 0)
	angle -= 0.5;
else	angle += 0.5;
return((int)angle);
}

/************************************************************************/
VOID TrackResetHandles()
/************************************************************************/
{
hPoints[H_ONE-1].x = TrackRect.left;
hPoints[H_ONE-1].y = TrackRect.top;
Transformer(&TrackTForm, &hPoints[H_ONE-1], &hPoints[H_ONE-1].x, &hPoints[H_ONE-1].y);

hPoints[H_TWO-1].x = TrackRect.right;
hPoints[H_TWO-1].y = TrackRect.top;
Transformer(&TrackTForm, &hPoints[H_TWO-1], &hPoints[H_TWO-1].x, &hPoints[H_TWO-1].y);

hPoints[H_THREE-1].x = TrackRect.right;
hPoints[H_THREE-1].y = TrackRect.bottom;
Transformer(&TrackTForm, &hPoints[H_THREE-1], &hPoints[H_THREE-1].x, &hPoints[H_THREE-1].y);

hPoints[H_FOUR-1].x = TrackRect.left;
hPoints[H_FOUR-1].y = TrackRect.bottom;
Transformer(&TrackTForm, &hPoints[H_FOUR-1], &hPoints[H_FOUR-1].x, &hPoints[H_FOUR-1].y);

hPoints[H_FIVE-1].x = (TrackRect.left+TrackRect.right)/2;
hPoints[H_FIVE-1].y = TrackRect.top;
Transformer(&TrackTForm, &hPoints[H_FIVE-1], &hPoints[H_FIVE-1].x, &hPoints[H_FIVE-1].y);

hPoints[H_SIX-1].x = TrackRect.right;
hPoints[H_SIX-1].y = (TrackRect.top+TrackRect.bottom)/2;
Transformer(&TrackTForm, &hPoints[H_SIX-1], &hPoints[H_SIX-1].x, &hPoints[H_SIX-1].y);

hPoints[H_SEVEN-1].x = (TrackRect.left+TrackRect.right)/2;
hPoints[H_SEVEN-1].y = TrackRect.bottom;
Transformer(&TrackTForm, &hPoints[H_SEVEN-1], &hPoints[H_SEVEN-1].x, &hPoints[H_SEVEN-1].y);

hPoints[H_EIGHT-1].x = TrackRect.left;
hPoints[H_EIGHT-1].y = (TrackRect.top+TrackRect.bottom)/2;
Transformer(&TrackTForm, &hPoints[H_EIGHT-1], &hPoints[H_EIGHT-1].x, &hPoints[H_EIGHT-1].y);
}

/************************************************************************/
VOID TrackSetHandles()
/************************************************************************/
{
hPoints[H_FIVE-1].x = (hPoints[H_ONE-1].x + hPoints[H_TWO-1].x)/2;
hPoints[H_FIVE-1].y = hPoints[H_ONE-1].y;
hPoints[H_SIX-1].x = hPoints[H_TWO-1].x;
hPoints[H_SIX-1].y = (hPoints[H_TWO-1].y + hPoints[H_THREE-1].y)/2;
hPoints[H_SEVEN-1].x = hPoints[H_FIVE-1].x;
hPoints[H_SEVEN-1].y = hPoints[H_THREE-1].y;
hPoints[H_EIGHT-1].x = hPoints[H_FOUR-1].x;
hPoints[H_EIGHT-1].y = hPoints[H_SIX-1].y;
}

/************************************************************************/
VOID TrackDispReset()
/************************************************************************/
{
if (TrackTransformMode)
	{
	TrackTForm = FTrackTForm;
	File2DispTForm(&TrackTForm);
	}
}