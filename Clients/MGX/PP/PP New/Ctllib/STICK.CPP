/***********************************************************************/
/***********************************************************************/
// JOYSTICK CONTROL
/***********************************************************************/
/***********************************************************************/

#include <math.h>
#include "ctlutil.h"
#include "ctltools.h"
#include "macros.h"
#include "stick.h"

#define JOYSIZE		10L

#define MAX_STICKS	4

// WINDOW WORDS/LONGS
// POINTER TO MAP POINTS MEMORY
#define GWL_STICKSTRUCT	(GWW_START+0)			
// Number of extra memory for window words	 
#define STICK_EXTRA	  		(GWL_STICKSTRUCT+sizeof(long))


static int Stick_bConstrain;
// struct allocated for every map control
typedef struct _stick
{		
	int x[MAX_STICKS];
	int y[MAX_STICKS];
	RGBS colors[MAX_STICKS];
	int iActiveStick;
	int iNumSticks;
	
	RECT StickRect;
}STICK, FAR * LPSTICK; 
// helper macro
#define GET_LPSTICK(hStick) \
	(hStick ? (LPSTICK)GetWindowLong(hStick,GWL_STICKSTRUCT) : NULL)


LOCAL void stick_setvalue( HWND hWindow, int x, int y, int iStick = -1);
LOCAL void stick_init_stick(LPSTICK lpstick, HWND hWindow);
LOCAL void undraw_stick( HDC hDC,LPSTICK lpstick, POINT Point, BOOL bActive);
LOCAL void draw_stick( HDC hDC,LPSTICK lpstick, POINT Point, LPRGB lpColor, BOOL bActive);

LOCAL BOOL Stick_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct);
LOCAL void Stick_OnDestroy(HWND hWindow);	
LOCAL void Stick_OnPaint(HWND hWindow);
LOCAL void Stick_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Stick_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Stick_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Stick_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Stick_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Stick_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Stick_OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Stick_OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL BOOL Stick_Draw( HDC hDC,LPSTICK lpstick , POINT Point );


/***********************************************************************/
void Stick_GetValue( HWND hWindow, LPINT lpX, LPINT lpY, int iStick)
/***********************************************************************/
{
	LPSTICK lpstick;

	*lpX = 0;
	*lpY = 0;
	
	if (!(lpstick = GET_LPSTICK(hWindow)))
		return;
		
	if (iStick < 0)
		iStick =  lpstick->iActiveStick;

	*lpX = lpstick->x[iStick];
	*lpY = lpstick->y[iStick];
}

/***********************************************************************/
void Stick_SetValue( HWND hWindow, int x, int y, int iStick)
/***********************************************************************/
{
	stick_setvalue(hWindow, x, y, iStick);

	// redraw
    InvalidateRect (hWindow,NULL,FALSE);
    UpdateWindow(hWindow); 
}

/***********************************************************************/
LOCAL void stick_setvalue( HWND hWindow, int x, int y, int iStick)
/***********************************************************************/
{
	LPSTICK lpstick;
	
	if (!(lpstick = GET_LPSTICK(hWindow)))
		return;

	if (iStick < 0)
		iStick =  lpstick->iActiveStick;
	lpstick->x[iStick] = x;
	lpstick->y[iStick] = y;
}


//************************************************************************
//		Stick_SetNumSticks
// DESCRIPTION:
//		Sets the number of displayed sticks to iNumSticks.
//		Redraws the control.
//************************************************************************
void Stick_SetNumSticks( HWND hWindow, int iNumSticks)
//************************************************************************
{
	LPSTICK lpstick;
	
	if (!(lpstick = GET_LPSTICK(hWindow)))
		return;
	
	lpstick->iNumSticks = Bound(iNumSticks, 1, MAX_STICKS);
	lpstick->iActiveStick = Bound(lpstick->iActiveStick, 0, lpstick->iNumSticks-1);
						
	// redraw
    InvalidateRect (hWindow,NULL,FALSE);
    UpdateWindow(hWindow); 
}



//************************************************************************
//		Stick_ActivateStick
// DESCRIPTION:
//		Sets active stick to iStick.  
//		Active stick is drawn on top and can be grabbed.
//		Redraws the control.
//************************************************************************
void Stick_ActivateStick( HWND hWindow, int iStick)
//************************************************************************
{
	LPSTICK lpstick;
	
	if (!(lpstick = GET_LPSTICK(hWindow)))
		return;

	lpstick->iActiveStick = Bound(iStick, 0, lpstick->iNumSticks-1);

	// redraw
    InvalidateRect (hWindow,NULL,FALSE);
    UpdateWindow(hWindow); 
}


//************************************************************************
//		Stick_SetColor
// DESCRIPTION:
//		Sets the color of the given stick to lpColor
//		Redraws the control.
//************************************************************************
void Stick_SetColor( HWND hWindow, int iStick, LPRGB lpColor)
//************************************************************************
{
	LPSTICK lpstick;
	
	if (!(lpstick = GET_LPSTICK(hWindow)))
		return;

	lpstick->colors[iStick] = *lpColor;

	// redraw
    InvalidateRect (hWindow,NULL,FALSE);
    UpdateWindow(hWindow); 
}

/***********************************************************************/
LONG WINPROC EXPORT Stick_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_ENABLE, Control_OnEnable);
	HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);

	HANDLE_MSG(hWindow, WM_CREATE, Stick_OnCreate);
	HANDLE_MSG(hWindow, WM_DESTROY, Stick_OnDestroy);
	HANDLE_MSG(hWindow, WM_PAINT, Stick_OnPaint);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Stick_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, Stick_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Stick_OnLButtonDblClk);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Stick_OnMouseMove);
	HANDLE_MSG(hWindow, WM_SETFOCUS, Stick_OnSetFocus);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, Stick_OnKillFocus);
	HANDLE_MSG(hWindow, WM_KEYDOWN, Stick_OnKeyDown);
	HANDLE_MSG(hWindow, WM_KEYUP, Stick_OnKeyUp);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}


/***********************************************************************/
LOCAL BOOL Stick_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	LPSTICK lpstick;

	lpstick = (LPSTICK)Alloc(sizeof(STICK));
	if (lpstick)
		stick_init_stick(lpstick, hWindow);
	SetWindowLong( hWindow, GWL_STICKSTRUCT, (long)lpstick);
	return(TRUE);
}

/***********************************************************************/
LOCAL void Stick_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	LPSTICK lpstick;

	if (lpstick = GET_LPSTICK(hWindow))
	{
		FreeUp((LPTR)lpstick);
		SetWindowLong( hWindow, GWL_STICKSTRUCT, (long)NULL);
	}
	Control_OnDestroy(hWindow);
}

/***********************************************************************/
LOCAL void Stick_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
POINT       rPoint, Point;    
LFIXED      xrate, yrate;
int         iWidth, iHeight, i, x, y;
HRGN        hRgn;
HDC         hDC; 
HPEN        hPen;
HGDIOBJ 	hOldObject;
//HGDIOBJ     hOldPen;
RECT        ClientRect;
LPSTICK 	lpstick;
	
if (!(lpstick = GET_LPSTICK(hWindow)))
	return;

hDC = BeginPaint( hWindow, &ps );
GetClientRect( hWindow, &ClientRect );

FillRect( hDC, &ClientRect, ( HBRUSH )GetStockObject(WHITE_BRUSH) );  


FrameRect(hDC, &ClientRect, (HBRUSH)GetStockObject(BLACK_BRUSH) ); 

InflateRect( &ClientRect, -1, -1 ); 

// Draw Grid Stuff
iWidth = RectWidth(&ClientRect);
iHeight = RectHeight(&ClientRect);

xrate = FGET( iWidth, 4 );
yrate = FGET( iHeight, 4 );

hPen = CreatePen( PS_SOLID, 1, RGB(128,128,128) );
hOldObject = SelectObject( hDC, hPen );

for( i = 1, x = FMUL( i, xrate ); x < iWidth;
	x = FMUL( i, xrate ), i++ )
	{
	MoveToEx( hDC, x, ClientRect.top, &rPoint );
	LineTo( hDC, x, ClientRect.bottom );
	}

for( i = 1, y = FMUL( i, yrate ); y < iHeight;
	y = FMUL( i, yrate ), i++ )
	{
	MoveToEx( hDC, ClientRect.left, y, &rPoint );
	LineTo( hDC, ClientRect.right, y );
	}
SelectObject( hDC, hOldObject );
DeleteObject( hPen );
 
SelectClipRgn( hDC, hRgn = CreateRectRgnIndirect( &lpstick->StickRect ) );
DeleteObject( hRgn );

// The window long holds the values from -100 and 100
Stick_GetValue(hWindow, ( LPINT )&Point.x, ( LPINT )&Point.y);
Point.x = bound( Point.x, -100, 100 );
Point.y = bound( Point.y, -100, 100 );
//if (StyleOn( hWindow, ST_HONLY) || (Stick_bConstrain == 2))
//	Point.y = 0;
//if (Stick_bConstrain == 3)
//	Point.x = 0;
Stick_Draw( hDC, lpstick, Point );
GrayWindow( hDC, hWindow, WS_NOTENABLED );
EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Stick_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
	int x, y;
	
	Stick_GetValue(hWindow, &x, &y);
	if ( vk == VK_HOME )
		Stick_OnMouseMove(hWindow, x, y-1, 0);
	else
	if ( vk == VK_END )
		Stick_OnMouseMove(hWindow, x, y+1, 0);
	else
	if ( vk == VK_LEFT )
		Stick_OnMouseMove(hWindow, x-1, y, 0);
	else
	if ( vk == VK_RIGHT )
		Stick_OnMouseMove(hWindow, x+1, y, 0);
	else
	if ( vk == VK_SPACE )
		Stick_OnLButtonDown(hWindow, FALSE, x, y, 0);
}

/***********************************************************************/
LOCAL void Stick_OnKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
	int x, y;
	if ( vk == VK_SPACE )
	{
		Stick_GetValue(hWindow, &x, &y);
		Control_OnLButtonUp(hWindow, x, y, 0);
	}
}

/***********************************************************************/
LOCAL void Stick_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
BOOL bChanged;
POINT Point;
int xMax, yMax;
HRGN hRgn;
LPSTICK lpstick;
	
if (!(lpstick = GET_LPSTICK(hWindow)))
	return;

if ( Control_bTrack )
	return;
SetCapture( hWindow );
Control_bTrack = TRUE;
Stick_bConstrain = 0;
if ( GetFocus() != hWindow )
	SetFocus( hWindow );
Control_shDC = GetDC( hWindow );
SelectClipRgn( Control_shDC, hRgn = CreateRectRgnIndirect( &lpstick->StickRect ) );
DeleteObject( hRgn );
xMax = RectWidth( &lpstick->StickRect ) - 1;
yMax = RectHeight( &lpstick->StickRect ) - 1;
Point.x = bound( x-lpstick->StickRect.left, 0, xMax );
Point.y = bound( y-lpstick->StickRect.top, 0, yMax );
Point.x = -100 + FMUL( 200, FGET( Point.x, xMax ) );
Point.y = -100 + FMUL( 200, FGET( Point.y, yMax ));
Point.y = -Point.y;
if (keyFlags & MK_CONTROL)
   Stick_bConstrain = 3;
if (keyFlags & MK_SHIFT)
   Stick_bConstrain = 2;
//if (StyleOn( hWindow, ST_HONLY) || (Stick_bConstrain == 2))
if((GetWindowLong(hWindow, GWL_USERDATA) == (LONG)ST_HONLY) || (Stick_bConstrain == 2))
	Point.y = lpstick->y[lpstick->iActiveStick];
if (Stick_bConstrain == 3)
	Point.x = lpstick->x[lpstick->iActiveStick];
bChanged = Stick_Draw( Control_shDC, lpstick, Point );
if ( !bChanged )
	return;
stick_setvalue(hWindow, Point.x, Point.y);
FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
					hWindow, SCN_STICK_CHANGING, SendMessage);
}

/***********************************************************************/
LOCAL void Stick_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
if ( !Control_bTrack )
	return;
Control_OnLButtonUp(hWindow, x, y, keyFlags);
FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
					hWindow, SCN_STICK_CHANGED, SendMessage);
}


/***********************************************************************/
LOCAL void Stick_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
POINT Point;
HRGN hRgn;
HDC hDC;
LPSTICK lpstick;
	
if (!(lpstick = GET_LPSTICK(hWindow)))
	return;

if ( Control_bTrack )
	return;
Point.x = Point.y = 0;
hDC = GetDC( hWindow );
SelectClipRgn( hDC, hRgn = CreateRectRgnIndirect( &lpstick->StickRect ) );
DeleteObject( hRgn );
Stick_Draw( hDC, lpstick, Point );
ReleaseDC( hWindow, hDC );
stick_setvalue(hWindow, Point.x, Point.y);
FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
					hWindow, SCN_STICK_CHANGED, SendMessage);
}

/***********************************************************************/
LOCAL void Stick_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
BOOL bChanged;
POINT Point;
int xMax, yMax;
LPSTICK lpstick;
	
if (!(lpstick = GET_LPSTICK(hWindow)))
	return;

if ( !Control_bTrack )
	return;
xMax = RectWidth( &lpstick->StickRect ) - 1;
yMax = RectHeight( &lpstick->StickRect ) - 1;
Point.x = bound( x-lpstick->StickRect.left, 0, xMax );
Point.y = bound( y-lpstick->StickRect.top, 0, yMax );
Point.x = -100 + FMUL( 200, FGET( Point.x, xMax ) );
Point.y = -100 + FMUL( 200, FGET( Point.y, yMax ) );
Point.y = -Point.y;
if (!Stick_bConstrain && (keyFlags & MK_CONTROL))
   Stick_bConstrain = 3;
if (!Stick_bConstrain && (keyFlags & MK_SHIFT))
   Stick_bConstrain = 2;
//if (StyleOn( hWindow, ST_HONLY) || (Stick_bConstrain == 2))
if((GetWindowLong(hWindow, GWL_USERDATA) == (LONG)ST_HONLY) || (Stick_bConstrain == 2))
	Point.y = lpstick->y[lpstick->iActiveStick];
if (Stick_bConstrain == 3)
	Point.x = lpstick->x[lpstick->iActiveStick];
bChanged = Stick_Draw( Control_shDC, lpstick, Point );
if ( !bChanged )
	return;
stick_setvalue(hWindow, Point.x, Point.y);
FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
					hWindow, SCN_STICK_CHANGING, SendMessage);
}

/***********************************************************************/
LOCAL void Stick_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
if ( !StyleOn( hWindow, WS_TABSTOP ) )
	return;
// skip the focus rect for now
//	// Draw the focus rectangle
//	hDC = GetDC( hWindow );
//	GetClientRect( hWindow, &ClientRect );
//	bHasFocus = YES;
//	FrameRect( hDC, &ClientRect,
//		GetStockObject( bHasFocus ? BLACK_BRUSH : WHITE_BRUSH ) );
//	ReleaseDC( hWindow, hDC );
}

/***********************************************************************/
LOCAL void Stick_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
if ( !StyleOn( hWindow, WS_TABSTOP ) )
	return;
// skip the focus rect for now
//	// Draw the focus rectangle
//	hDC = GetDC( hWindow );
//	GetClientRect( hWindow, &ClientRect );
//	bHasFocus = NO;
//	FrameRect( hDC, &ClientRect,
//		GetStockObject( bHasFocus ? BLACK_BRUSH : WHITE_BRUSH ) );
//	ReleaseDC( hWindow, hDC );
}

// Note: Point is the position to draw the ACTIVE point.
/***********************************************************************/
BOOL Stick_Draw( HDC hDC,LPSTICK lpstick, POINT Point )
/***********************************************************************/
{
LFIXED		xrate, yrate;
int			iWidth, iHeight, i, xgrid, ygrid;
HGDIOBJ		hOldObject;
HGDIOBJ		hPen;
POINT		p, rPoint;

#define R 5

// undraw old sticks
for (i=0; i<lpstick->iNumSticks;i++)
{
	p.x = lpstick->x[i];
	p.y = lpstick->y[i];
	undraw_stick(hDC, lpstick, p, i == lpstick->iActiveStick);
}
	
// Grid Redraw Stuff
iWidth = RectWidth(&lpstick->StickRect);
iHeight = RectHeight(&lpstick->StickRect);

xrate = FGET( iWidth, 4 );
yrate = FGET( iHeight, 4 );

hPen = CreatePen( PS_SOLID, 1, RGB(128,128,128) );
hOldObject = SelectObject( hDC, hPen );

for( i = 1, xgrid = FMUL( i, xrate ); xgrid < iWidth;
	xgrid = FMUL( i, xrate ), i++ )
	{
	MoveToEx( hDC, xgrid, lpstick->StickRect.top, &rPoint );
	LineTo( hDC, xgrid, lpstick->StickRect.bottom );
	}

for( i = 1, ygrid = FMUL( i, yrate ); ygrid < iHeight;
	ygrid = FMUL( i, yrate ), i++ )
	{
	MoveToEx( hDC, lpstick->StickRect.left, ygrid, &rPoint );
	LineTo( hDC, lpstick->StickRect.right, ygrid );
	}
SelectObject( hDC, hOldObject );
DeleteObject( hPen ); 
	

// Draw the inactive ones
for (i=0; i<lpstick->iNumSticks; i++)
{
	if (i == lpstick->iActiveStick)
		continue;
	p.x = lpstick->x[i];
	p.y = lpstick->y[i];
	draw_stick(hDC, lpstick, p, &lpstick->colors[i], NO);
}

// draw active stick
draw_stick(hDC, lpstick, Point, &lpstick->colors[lpstick->iActiveStick], YES);

return( YES );
}

LOCAL void undraw_stick( HDC hDC,LPSTICK lpstick , POINT Point, BOOL bActive)
{
	HGDIOBJ		hOldBrush;
	HGDIOBJ		hOldPen;
   POINT       p;
	int x,y;

	Point.y = -Point.y;
	x = lpstick->StickRect.left + FMUL( 100+Point.x, FGET( RectWidth(&lpstick->StickRect)-1, 200 ) );
	y = lpstick->StickRect.top  + FMUL( 100+Point.y, FGET( RectHeight(&lpstick->StickRect)-1, 200 ) );
	
	hOldPen = SelectObject( hDC, GetStockObject(WHITE_PEN) );
	hOldBrush = SelectObject( hDC, GetStockObject(WHITE_BRUSH) );
	MoveToEx(hDC, x, lpstick->StickRect.top, &p);
	LineTo(hDC, x, lpstick->StickRect.bottom);
	MoveToEx(hDC, lpstick->StickRect.left, y, &p);
	LineTo(hDC, lpstick->StickRect.right, y);
	Ellipse(hDC, x-R, y-R, x+R, y+R);
	SelectObject( hDC, hOldPen );
	SelectObject( hDC, hOldBrush );
}

LOCAL void draw_stick( HDC hDC,LPSTICK lpstick, POINT Point,  LPRGB lpColor, BOOL bActive)
{
	HGDIOBJ		hOldBrush;
	HGDIOBJ		hOldPen;
	HBRUSH 		hBrush = NULL;
   POINT       p;
	int x,y;

	Point.y = -Point.y;
	x = lpstick->StickRect.left + FMUL( 100+Point.x, FGET( RectWidth(&lpstick->StickRect)-1, 200 ) );
	y = lpstick->StickRect.top  + FMUL( 100+Point.y, FGET( RectHeight(&lpstick->StickRect)-1, 200 ) );
	
	hOldPen = SelectObject( hDC, GetStockObject(BLACK_PEN) );
 	if (lpColor->red == 127 && lpColor->green == 127 && lpColor->blue == 127)
		hOldBrush = SelectObject( hDC, DrawTools.GetBtnShadowBrush() );
	else
	{
		// use a color brush
		hBrush = CreateSolidBrush(RGB(lpColor->red,lpColor->green,lpColor->blue));
		hOldBrush = SelectObject( hDC, hBrush );
	}
	if (bActive)
	{
		MoveToEx( hDC, x, lpstick->StickRect.top, &p);
		LineTo( hDC, x, lpstick->StickRect.bottom );
		MoveToEx( hDC, lpstick->StickRect.left, y, &p);
		LineTo( hDC, lpstick->StickRect.right, y );
	}
	Ellipse( hDC, x-R, y-R, x+R, y+R );
	SelectObject( hDC, hOldPen );
	SelectObject( hDC, hOldBrush );
	if (hBrush)
		DeleteObject(hBrush);
	
}


BOOL Stick_RegisterClass (HINSTANCE hInstance, UINT ClassScope)
{
	WNDCLASS WndClass;

	/* Setup and define the window class labeled frame control */
	/* Setup and define the window class for a joystick */
	WndClass.hIcon		= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPTSTR)_T("joystick");
	WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
	WndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	WndClass.hInstance	= hInstance;
	WndClass.style		= CS_DBLCLKS | ClassScope;
	WndClass.lpfnWndProc	= Stick_Control;
	WndClass.cbClsExtra	= 0;
	WndClass.cbWndExtra	= STICK_EXTRA;

	return(RegisterClass( &WndClass ));
}

BOOL Stick_UnRegisterClass (HINSTANCE hInstance)
{
    return (UnregisterClass ((LPTSTR)_T("joystick"), hInstance));
}

LOCAL void stick_init_stick(LPSTICK lpstick,HWND hWindow)
{
	int i;
	RECT ClientRect;
	
	for (i=0; i<MAX_STICKS; i++)
	{
		lpstick->x[i] = 0;
		lpstick->y[i] = 0;
		lpstick->colors[i].red = 
		lpstick->colors[i].green = 
		lpstick->colors[i].blue = 127;
	}
	
	lpstick->iActiveStick = 0;
	lpstick->iNumSticks = 1;

	// get the drawing area
	GetClientRect( hWindow, &ClientRect );
	InflateRect( &ClientRect, -1, -1 ); 
	lpstick->StickRect = ClientRect;
}
