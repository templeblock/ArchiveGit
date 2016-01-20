// (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// lutctl.c for map control
#include <math.h>
#include "ctlutil.h"
#include "lutctl.h"
#include "macros.h"
#include "ctltools.h"

// WINDOW WORDS/LONGS
// POINTER TO LUT POINTS MEMORY
#define GWL_LUTSTRUCT	(GWW_START+0)			
// Number of extra memory for window words
#define LUT_EXTRA	(GWW_START+4)

// range of input & output values
#define MAXVAL 	255
#define MINVAL	0

// numer of elements in the final map - 1.
#define MAXLUTS 4

#define MAXLUT 255

// struct allocated for every lut control
typedef struct _lutctl
{						    
	DWORD dwStyle;		// save for convenience
	int gridX;			// how often to draw vert lines (0 for none)
	int gridY;			// how often to draw horiz lines (0 for none)
	RGBS gridRGB;		// color of grid.
	int	 nLuts;		// number of luts
	LPTR lpLut;
	RGBS Colors[MAXLUTS];
}LUTCTL, FAR * LPLUTCTL;

// helper macro
#define GET_LPLUTCTL(hWindow) \
	(hWindow ? (LPLUTCTL)GetWindowLong(hWindow,GWL_LUTSTRUCT) : NULL)

#define BOUND(x,a,b) max(min(b,x),a)
#define WITHIN(val,lower,upper) \
	( (val) < (lower) ? 0 : ( (val) > (upper) ? 0 : 1 ) )

LOCAL VOID lut_draw_grid(LPLUTCTL lpdata, HDC hDC, LPRECT lpArea);
LOCAL void lut_draw(LPLUTCTL lpdata, HDC hDC, LPRECT lpArea, BOOL bOn);
LOCAL VOID lut_init_lutctl(LPLUTCTL lpdata, HWND hWindow);

//************************************************************************
//************************************************************************
//	LUT Access Functions
//************************************************************************
//************************************************************************


//************************************************************************
//	Lut_Init
//
//	PARAMETERS:
//				int gridX- vertical grid spacing (0 for none, -1 for NoChange).
//				int gridY- horiz grid spacing (0 for none, -1 for NC).
//	DESCRIPTION: 
//			Sets the above parameters and redraws the control.
//	RETURNS: 
//		   	FALSE if allocation fails. (caller should put up memory error)
//************************************************************************
BOOL Lut_Init(HWND hWindow, int gridX, int gridY)
//************************************************************************
{
	LPLUTCTL lpdata;
	
	if (!(lpdata = GET_LPLUTCTL(hWindow)))
	{
		lpdata = (LPLUTCTL)Alloc(sizeof(LUTCTL));
		if (!lpdata)
			return(FALSE);
		lut_init_lutctl(lpdata, hWindow);
		SetWindowLong( hWindow, GWL_LUTSTRUCT, (long)lpdata);
	}
	if (gridX >= 0)
		lpdata->gridX = gridX;			// how often to draw vert lines (0 for none)
	if (gridY >= 0)
		lpdata->gridY = gridY;			// how often to draw horiz lines (0 for none)

	// draw it!
    InvalidateRect (hWindow,NULL,FALSE);
	return(TRUE);
}


//************************************************************************
//	Lut_SetLut
//
//	PARAMETERS:
//				LPTR lpLut - lookup table to draw.
//				LPRGB lpRGB - pointer to color to draw lookup table with.
//	DESCRIPTION: 
//			Sets the above parameters and redraws the control.
//	RETURNS: 
//		   	FALSE if too many adds. 
//************************************************************************
BOOL Lut_SetLut(HWND hWindow, int nLuts, LPTR lpLut)
//************************************************************************
{
	LPLUTCTL lpdata;
	
	if (!(lpdata = GET_LPLUTCTL(hWindow)))
		return(FALSE);

	if (lpdata->lpLut)
	{
		FreeUp(lpdata->lpLut);
		lpdata->lpLut = NULL;
		lpdata->nLuts = 0;
	}
	if (nLuts && lpLut)
	{
		lpdata->nLuts = nLuts;
		lpdata->lpLut = Alloc(256L*lpdata->nLuts);
		if (!lpdata->lpLut)
		{
			lpdata->nLuts = 0;
			return(FALSE);
		}
		copy(lpLut, lpdata->lpLut, 256*lpdata->nLuts);
	}

    InvalidateRect (hWindow, NULL, FALSE);
	return(TRUE);
}

//************************************************************************
//	Lut_SetColor
//
//	PARAMETERS:
//				int nLutIndex - index for color
//				LPRGB lpRGB - pointer to color to draw lookup table with.
//	DESCRIPTION: 
//			Sets the above parameters and redraws the control.
//	RETURNS: 
//		   	FALSE if too many adds. 
//************************************************************************
BOOL Lut_SetColor(HWND hWindow, int nLutIndex, LPRGB lpRGB)
//************************************************************************
{
	LPLUTCTL lpdata;
	
	if (!(lpdata = GET_LPLUTCTL(hWindow)))
		return(FALSE);
	if (nLutIndex >= lpdata->nLuts)
		return(FALSE);

	lpdata->Colors[nLutIndex] = *lpRGB;
    InvalidateRect (hWindow, NULL, FALSE);
	return(TRUE);
}

BOOL Lut_RegisterClass (HINSTANCE hInstance, UINT ClassScope)
{
	WNDCLASS WndClass;

	/* Setup and define the window class labeled frame control */
	/* Setup and define the window class for a joystick */
	WndClass.hIcon		= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPTSTR)MGXCTL_LUT;
	WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
	WndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	WndClass.hInstance	= hInstance;
	WndClass.style		= CS_DBLCLKS | ClassScope;
	WndClass.lpfnWndProc	= Lut_Control;
	WndClass.cbClsExtra	= 0;
	WndClass.cbWndExtra	= LUT_EXTRA;

	return(RegisterClass( &WndClass ));
}

BOOL Lut_UnRegisterClass (HINSTANCE hInstance)
{
    return (UnregisterClass ((LPTSTR)_T("lutctl"), hInstance));
}

//***********************************************************************
//***********************************************************************
// LUT CONTROL
//***********************************************************************
//***********************************************************************

LOCAL BOOL Lut_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct);
LOCAL void Lut_OnPaint(HWND hWindow);
LOCAL void Lut_OnDestroy(HWND hWindow);
LOCAL BOOL Lut_Draw( HDC hDC, LPRECT lpRect, LPPOINT lpLastPoints, LPPOINT lpPoints, int nPoints, int idPointToChange, POINT Point, BOOL bDrawHandles);

/***********************************************************************/
LONG WINPROC EXPORT Lut_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_ENABLE, Control_OnEnable);
		HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);

		HANDLE_MSG(hWindow, WM_CREATE, Lut_OnCreate);
		HANDLE_MSG(hWindow, WM_DESTROY, Lut_OnDestroy);
		HANDLE_MSG(hWindow, WM_PAINT, Lut_OnPaint);

		default:
		return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL BOOL Lut_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	LPLUTCTL lpdata;

	lpdata = (LPLUTCTL)Alloc(sizeof(LUTCTL));
	if (lpdata)
		lut_init_lutctl(lpdata, hWindow);
	SetWindowLong( hWindow, GWL_LUTSTRUCT, (long)lpdata);
	return(TRUE);
}

/***********************************************************************/
LOCAL void Lut_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	LPLUTCTL lpdata;

	if (lpdata = GET_LPLUTCTL(hWindow))
	{
		if (lpdata->lpLut)
			FreeUp(lpdata->lpLut);
		FreeUp((LPTR)lpdata);
		SetWindowLong( hWindow, GWL_LUTSTRUCT, (long)NULL);
	}
	Control_OnDestroy(hWindow);
}

/***********************************************************************/
LOCAL void Lut_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC         hDC;
	RECT        ClientRect;
	LPLUTCTL 	lpdata;

	hDC = BeginPaint( hWindow, &ps );
	if (!(lpdata = GET_LPLUTCTL(hWindow)))
	   	goto Done;

	GetClientRect(hWindow, &ClientRect);

	// Draw the interior and exterior box frames
	FillRect( hDC, &ClientRect, ( HBRUSH )GetStockObject(WHITE_BRUSH) );
	FrameRect( hDC, &ClientRect, ( HBRUSH )GetStockObject(BLACK_BRUSH) ); 
	InflateRect(&ClientRect, -1, -1);

	// Draw the grid
	lut_draw_grid(lpdata, hDC, &ClientRect);

	// Draw the lines
	lut_draw( lpdata, hDC, &ClientRect, ON);
	
//	GrayWindow( hDC, hWindow, WS_NOTENABLED );
Done:
	EndPaint( hWindow, &ps );
}

//************************************************************************
//				lut_draw_grid
// PARAMETERS:
//					HDC hDC- Can NOT be NULL.
//					LPRECT lpArea- The area to update.
// DESCRIPTION:
//				draws the grid at the given spacing (map coords).
//************************************************************************
LOCAL VOID lut_draw_grid(LPLUTCTL lpdata, HDC hDC, LPRECT lpArea)
//***********************************************************************
{
	LFIXED rate;
	HPEN hOldPen, hPen;
	int i, val, offset, maxval;
	int gridX = lpdata->gridX;
	int gridY = lpdata->gridY;
	RECT rArea;
	POINT	point;

	// subtract one cause we are using MoveTo, LineTo
	rArea = *lpArea;
	--rArea.right;	// we can draw on left and right pixels
	--rArea.bottom; // we can draw on top and bottom pixels
	hPen = DrawTools.GetBtnFacePen();
	hOldPen = (HPEN)SelectObject( hDC, hPen );

	// do vertical lines
	if (gridX)
	{
		maxval = RectWidth(&rArea)-1; // max x value
		rate = FGET(maxval, MAXVAL);
		offset = rArea.left;
		for (i=gridX; i<256; i+=gridX)
		{
			val = offset + FMUL(i, rate);
			MoveToEx( hDC, val, rArea.top, &point);
			// add one cause LineTo doesn't draw on endpoint
			LineTo( hDC, val, rArea.bottom+1); 
		}
	}
	// do horizontal lines
	if (gridY)
	{
		maxval = RectHeight(&rArea)-1; // max y value
		rate = FGET(maxval, MAXVAL);
		offset = rArea.bottom;
		for (i=gridY; i<256; i+=gridY)
		{
			val = offset - FMUL(i, rate);
			MoveToEx( hDC, rArea.left, val, &point);
			// add one cause LineTo doesn't draw on endpoint
			LineTo( hDC, rArea.right+1, val);
		}
	}
	SelectObject( hDC, hOldPen );
}
	
	

//************************************************************************
//			lut_draw
// PARAMETERS:
//					LPRECT lpArea- The area to update.
//					BOOL bOn- draw ON or draw OFF?
//
// DESCRIPTION:
//			Draws the given range from lpData.  If lpData==NULL then it 
//			will use lpdata->Map (setup in lut_compute).	 Draws handles 
//			unless (dwFlags & MS_NOHANDLES). 
// RETURNS:
//
//************************************************************************
LOCAL void lut_draw(LPLUTCTL lpdata, HDC hDC, LPRECT lpArea, BOOL bOn)
//***********************************************************************
{
	LFIXED rateX, rateY, lVal;
	BOOL bInvX = FALSE, bInvY = FALSE;
	POINT offset;
	POINT DataOut[256];
	int OldROP;
	int i, lut, nLuts, maxx, maxy, num;
	LPTR lpLut;
        RGBS rgb;
	HPEN hOldPen, hPen;
	RECT rArea;
	POINT	point;

	if ((lpdata->nLuts <= 0) || !lpdata->lpLut)
		return;

	// subtract one cause we are using PolyLine
	rArea = *lpArea;
	--rArea.bottom;
	--rArea.right;

//	bInvX = (lpdata->dwStyle & MS_INVERTX);
//	bInvY = (lpdata->dwStyle & MS_INVERTY);
	offset.x = (bInvX ? rArea.right : rArea.left);
	offset.y = (bInvY ? rArea.top: rArea.bottom);

	for (lut = 0; lut < lpdata->nLuts; ++lut)
	{
		// determine source data
		lpLut = lpdata->lpLut;
		lpLut += lut;
		nLuts = lpdata->nLuts;

		maxx = RectWidth(&rArea)-1;
		maxy = RectHeight(&rArea)-1;
	   	rateX = FGET(maxx, MAXVAL);
   		rateY = FGET(maxy, MAXVAL);

		// convert to screen coords
		if (rateX < FUNITY)
		{
			// go through pixels
	   		rateX = FGET(MAXVAL, maxx);
			num = maxx+1;
			for (i=0; i<num; i++)
			{
				DataOut[i].x = i;
				DataOut[i].y = FMUL(lpLut[FMUL(i,rateX)*nLuts], rateY);
			}
		}
		else
		{
			// go through map
			lVal = 0;
			num = 256;
	 		for (i=0; i<num;i++)
			{
			  	DataOut[i].x = FROUND(lVal);
				DataOut[i].y = FMUL(lpLut[i*nLuts], rateY);
				lVal += rateX;
			}
		}
	
		// convert to screen coords
		for (i=0; i<num;i++)
		{
			DataOut[i].x = (bInvX ? (offset.x-DataOut[i].x) : (offset.x+DataOut[i].x));
			DataOut[i].y = (bInvY ? (offset.y+DataOut[i].y) : (offset.y-DataOut[i].y));
		}
	
		// draw line
		rgb = lpdata->Colors[lut];
		hPen = CreatePen(PS_SOLID, 1, RGB(rgb.red, rgb.green, rgb.blue));
		hOldPen = (HPEN)SelectObject(hDC, hPen);
		Polyline(hDC, DataOut, num);
		// make sure we draw on endpoint
		MoveToEx(hDC, DataOut[num-1].x, DataOut[num-1].y, &point);
		LineTo(hDC, DataOut[num-1].x+1, DataOut[num-1].y-1);
		SelectObject(hDC, hOldPen);
		DeleteObject(hPen);
	}
}

//************************************************************************
//		lut_init_lutctl
// DESCRIPTION:
//		sets lpdata to default values
//************************************************************************
LOCAL VOID lut_init_lutctl(LPLUTCTL lpdata, HWND hWindow)
//************************************************************************
{
	if (!lpdata)
		return;
	lpdata->dwStyle = GetWindowLong(hWindow, GWL_STYLE);
	lpdata->nLuts = 0;		// The maximum number of points to allow
	lpdata->gridX = 25;			// how often to draw vert lines (0 for none)
	lpdata->gridY = 25;			// how often to draw horiz lines (0 for none)
	lpdata->gridRGB.red = 0;  	// color of grid
	lpdata->gridRGB.green = 0;	
	lpdata->gridRGB.blue = 0;	
	lpdata->lpLut = NULL;
}





