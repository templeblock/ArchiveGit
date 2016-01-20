// (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// mapctl.c for map control
#include <math.h>
#include "ctlutil.h"
#include "histoctl.h"
#include "macros.h"

// POINTER TO MEMORY
#define GWL_HISTOSTRUCT	(GWW_START+0)			
// Number of extra memory for window words
#define HISTO_EXTRA	(GWW_START+4)

// how close do we have to be to a mark to hit it (in display units)
#define HIT_DIST	4

#define DRAW_WIDTH(lphisto) \
	(RectWidth(&lphisto->rArea)-1)
#define DRAW_HEIGHT(lphisto) \
	(RectHeight(&lphisto->rArea)-1)
#define DRAW_LEFT(lphisto) \
	(lphisto->rArea.left )
#define DRAW_RIGHT(lphisto) \
	(lphisto->rArea.right)
#define DRAW_BOTTOM(lphisto) \
	(lphisto->rArea.bottom)
#define DRAW_TOP(lphisto) \
	(lphisto->rArea.top)

// struct allocated for every histocontrol
typedef struct _histoctl
{
	LPDWORD lpHisto;		// Actual histo data (256 elements)
	LONG lMaxValue;			// The largest value in array (height of histo)
	int Marks[MAX_MARKS];	// The moveable marks
	int nMarks;				// number of movable marks
	DWORD dwStyle;			// Copy of window style for convienence
	RGBS Color;				// color to draw histo;
	RECT rArea;			// Client rect of ACTIVE area of control.
	LFIXED xrate;		// rate to convert from display to 0-255
	int nMoving;			// Which mark is grabbed
	int xMoving;			// Original position grabbed (screen coords)
	int orgMoving;			// Original position of moving mark (histo coords)
	int xBrowse;		// last place browsed
}HISTOCTL, FAR * LPHISTOCTL;

// helper macros
#define GET_LPHISTOCTL(hDlg) \
	(hDlg ? (LPHISTOCTL)GetWindowLong(hDlg,GWL_HISTOSTRUCT) : NULL)
	
// defines to handle fliped x axis
#define FLIPX(x, flip)	(flip ? -(x):(x))
#define GETX(x)	((lphisto->dwStyle & HTS_FLIPX) ? -(x):(x))

// Parent notification
#define MARK_CHANGING(hWindow, mark) \
	(FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, \
		HTN_CHANGING_FIRST+mark, SendMessage ))

// Parent notification
#define MARK_CHANGED(hWindow, mark) \
	(FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, \
		HTN_CHANGED_FIRST+mark, SendMessage ))

LOCAL void draw_histo(LPHISTOCTL lphisto, HDC hDC, LPRECT lpRect);
LOCAL void draw_mark(LPHISTOCTL lphisto, HDC hDC, int iMark, BOOL bOn);
LOCAL HDC get_dc(HWND hWindow, LPHISTOCTL lphisto);
LOCAL VOID done_dc(HWND hWindow, HDC hDC, BOOL bOk2Free);
LOCAL VOID init_histoctl(LPHISTOCTL lphisto, HWND hHisto);
LOCAL BOOL histo_SelectClipRect(HDC hDC, LPRECT lpNewRect, LPRECT lpOldRect );
LOCAL int client_to_histo(LPHISTOCTL lphisto, int x, BOOL bMove);

//************************************************************************
//************************************************************************
//	HISTO Access Functions
//************************************************************************
//************************************************************************

//************************************************************************
//		Histo_Init
//	PARAMETERS:
//			LPDWORD lpHisto- Array of histogram data (256 elents) or NULL 
//			LONG lMaxValue- The largest value in array (height of histo)
//			int nMarks- How many marks should there be?
//			WORD wStyles- the style  bits.
//	DESCRIPTION: 
//			Sets the above parameters and redraws the control.
//			Any item whichis -1 is not changed (except lpHisto). 
//	RETURNS: 
//			FALSE if allocation fails. (caller should put up memory error)
//************************************************************************
BOOL Histo_Init(HWND hHisto, LPDWORD lpHisto, LONG lMaxValue, int nMarks, WORD wStyles)
//************************************************************************
{
	LPHISTOCTL lphisto;
	int i;
	DWORD dwStyles;
	
	
	if (!(lphisto = GET_LPHISTOCTL(hHisto)))
	{
		lphisto = (LPHISTOCTL)Alloc(sizeof(HISTOCTL));
		if (!lphisto)
			return(FALSE);
		init_histoctl(lphisto, hHisto);
		SetWindowLong( hHisto, GWL_HISTOSTRUCT, (long)lphisto);
	}
	
	lphisto->lpHisto = lpHisto;
	if (lMaxValue != -1)
		lphisto->lMaxValue = lMaxValue;
	if (nMarks != -1)
	{
		lphisto->nMarks = Bound(nMarks,0, MAX_MARKS);
		for(i=0;i<lphisto->nMarks;i++)
			lphisto->Marks[i] = (i*255)/lphisto->nMarks;
		
	}
	if (wStyles != -1)
	{
		dwStyles = GetWindowLong(hHisto, GWL_STYLE);
		dwStyles &= 0xffff0000;
		dwStyles |= wStyles;
		SetWindowLong(hHisto, GWL_STYLE, dwStyles);
		lphisto->dwStyle = dwStyles;
	}
	
	// draw it!
    InvalidateRect (hHisto,NULL,FALSE);
//   UpdateWindow (hHisto); 
	return(TRUE);
}

//************************************************************************
//		Histo_SetColor
//	DESCRIPTION: 
//			Sets the color of the histogram.
//			Redraws control.
//************************************************************************
void Histo_SetColor(HWND hHisto, RGBS Color)
{
	LPHISTOCTL lphisto;
	
	if (!(lphisto = GET_LPHISTOCTL(hHisto)))
		return;
	
	lphisto->Color = Color;
	
	// draw it!
    InvalidateRect (hHisto,NULL,FALSE);
    UpdateWindow (hHisto); 
}

//************************************************************************
//		Histo_SetMark
//	PARAMETERS:
//			int iMark- Which mark to set.
//			int iValue- What to set it to.
//			BOOL bConstrain- Ensure that it is between its neighbors?
//	DESCRIPTION: 
//			Sets the given marker.
//			update the display.
//	RETURNS: 
//			The new position of the marker. 
//			HISTO_BAD_MARKER for error (iMark out of bounds etc)
//************************************************************************
int Histo_SetMark(HWND hHisto, int iMark, int iValue, BOOL bConstrain)
//************************************************************************
{
	LPHISTOCTL lphisto;
	HDC hDC;
	int iMin, iMax;
	
	if (!(lphisto = GET_LPHISTOCTL(hHisto)))
		return(HISTO_BAD_MARKER);
	
	if (!WITHIN (iMark+1, 0, lphisto->nMarks))
		return(HISTO_BAD_MARKER);
	
	if (bConstrain)
	{
		// ensure that it is within its neighbors
		iMax = (iMark+1<lphisto->nMarks) ?  lphisto->Marks[iMark+1]:256;
		iMax = (iMark > 0) ? lphisto->Marks[iMark+1]:0;
		iValue = Bound(iValue, iMin, iMax);
	}
	
	if (lphisto->Marks[iMark] == iValue)
		return(iValue);
	// undraw old mark
	hDC = get_dc(hHisto, lphisto);
	draw_mark(lphisto, hDC, iMark, OFF);
	// set mark
	lphisto->Marks[iMark] = iValue;
	// draw new mark
	draw_mark(lphisto, hDC, iMark, ON);
	done_dc(hHisto, hDC, YES /*Free DC?*/);
	return(iValue);
}

//************************************************************************
//			Histo_GetMark
//	PARAMETERS:
//			int iMark- The mark to get.
//	RETURNS: 
//			The value of the given hitso marker.
//			HISTO_BAD_MARKER for error (iMark out of bounds etc)
//************************************************************************
int  Histo_GetMark(HWND hHisto, int iMark)
//************************************************************************
{
	LPHISTOCTL lphisto;
	
	if (!(lphisto = GET_LPHISTOCTL(hHisto)))
		return(HISTO_BAD_MARKER);
	if (!WITHIN(iMark+1, 0, lphisto->nMarks))
		return(HISTO_BAD_MARKER);
	return(lphisto->Marks[iMark]);
}


//************************************************************************
//		Histo_GetBrowse
// DESCRIPTION:
//
// RETURNS:
//			the x coord last browsed (in histo coords).
//			HISTO_BAD_MARKER for an error.
//************************************************************************
int  Histo_GetBrowse(HWND hHisto)
//************************************************************************
{
	LPHISTOCTL lphisto;
	
	if (!(lphisto = GET_LPHISTOCTL(hHisto)))
		return(HISTO_BAD_MARKER);
	return(lphisto->xBrowse);
}

BOOL Histo_RegisterClass (HINSTANCE hInstance, UINT ClassScope)
{
	WNDCLASS WndClass;

	/* Setup and define the window class labeled frame control */
	/* Setup and define the window class for a joystick */
	WndClass.hIcon		= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPTSTR)_T("histo");
	WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
	WndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	WndClass.hInstance	= hInstance;
	WndClass.style		= CS_DBLCLKS | ClassScope;
	WndClass.lpfnWndProc	= Histo_Control;
	WndClass.cbClsExtra	= 0;
	WndClass.cbWndExtra	= HISTO_EXTRA;

	return(RegisterClass( &WndClass ));
}

BOOL Histo_UnRegisterClass (HINSTANCE hInstance)
{
    return (UnregisterClass ((LPTSTR)"histo", hInstance));
}


/***********************************************************************/
/***********************************************************************/
// HISTO CONTROL
/***********************************************************************/
/**********************************************************************/

LOCAL BOOL Histo_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct);
LOCAL void Histo_OnDestroy(HWND hWindow);
LOCAL void Histo_OnPaint(HWND hWindow);
LOCAL void Histo_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Histo_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Histo_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Histo_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Histo_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Histo_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Histo_HandleFocus(HWND hWindow, BOOL fSetFocus);
LOCAL void Histo_Draw( HDC hDC, LPRECT lpRect, DWORD dwMaxValue, LPDWORD lpHisto, LPRGB lpRGB, DWORD dwNewMarks, DWORD dwOldMarks );


/***********************************************************************/
LONG WINPROC EXPORT Histo_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_ENABLE, Control_OnEnable);
		HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);
		HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);
	
		HANDLE_MSG(hWindow, WM_CREATE, Histo_OnCreate);
	   	HANDLE_MSG(hWindow, WM_DESTROY, Histo_OnDestroy);
		HANDLE_MSG(hWindow, WM_PAINT, Histo_OnPaint);
		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Histo_OnLButtonDown);
		HANDLE_MSG(hWindow, WM_LBUTTONUP, Histo_OnLButtonUp);
		HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Histo_OnLButtonDblClk);
		HANDLE_MSG(hWindow, WM_MOUSEMOVE, Histo_OnMouseMove);
	
		default:
			return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL BOOL Histo_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	LPHISTOCTL lphisto;

	lphisto = (LPHISTOCTL)Alloc(sizeof(HISTOCTL));
	if (lphisto)
		init_histoctl(lphisto, hWindow);
	SetWindowLong( hWindow, GWL_HISTOSTRUCT, (long)lphisto);
	return(TRUE);
}

/***********************************************************************/
LOCAL void Histo_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	LPHISTOCTL lphisto;

	if (lphisto = GET_LPHISTOCTL(hWindow))
	{
		FreeUp((LPTR)lphisto);
		SetWindowLong( hWindow, GWL_HISTOSTRUCT, (long)NULL);
	}
	Control_OnDestroy(hWindow);
}


/***********************************************************************/
LOCAL void Histo_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC hDC;
	RECT ClientRect;
	int mark;
	LPHISTOCTL lphisto;
	
	hDC = BeginPaint( hWindow, &ps );
	if (!(lphisto = GET_LPHISTOCTL(hWindow)))
		goto Done;
		
	GetClientRect( hWindow, &ClientRect );
	FillRect( hDC, &ClientRect, ( HBRUSH )GetStockObject(WHITE_BRUSH));
	FrameRect( hDC, &ClientRect, ( HBRUSH )GetStockObject(BLACK_BRUSH));
	
	draw_histo(lphisto, hDC, &ps.rcPaint);
	for (mark=0; mark<lphisto->nMarks; mark++)
		draw_mark(lphisto, hDC, mark, ON);
Done:
	EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Histo_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	int i, pos;
	int dist, bestdist;
	LPHISTOCTL lphisto;
	
	if ( Control_bTrack )
		return;
	if (!(lphisto = GET_LPHISTOCTL(hWindow)))
		return;

	// find cloeset mark
	lphisto->nMoving = HISTO_BAD_MARKER;
	pos = client_to_histo(lphisto, x, YES /*absolute coords*/);
	bestdist = client_to_histo(lphisto, HIT_DIST, NO /*delta*/);
	for (i=0; i<lphisto->nMarks; i++)
	{
		dist = abs(lphisto->Marks[i]-pos);
		if(dist < bestdist)
			lphisto->nMoving = i; 
	}
	lphisto->xMoving = x;
	if (lphisto->nMoving != HISTO_BAD_MARKER)
		lphisto->orgMoving = lphisto->Marks[lphisto->nMoving];

	// Set stuff needed during tracking
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	SetCapture( hWindow ); Control_bTrack = TRUE;
}

/***********************************************************************/
LOCAL void Histo_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
}

/***********************************************************************/
LOCAL void Histo_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	LPHISTOCTL lphisto;
	int dx, newpos, i;
	BOOL bMovedOthers;
	HDC hDC;
	
	if (!(lphisto = GET_LPHISTOCTL(hWindow)))
		return;
	
	if (!Control_bTrack || (!WITHIN(lphisto->nMoving,0,lphisto->nMarks)))
	{
		// BROWSING
		lphisto->xBrowse = client_to_histo(lphisto, x, NO /*delta*/);
		FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, HTN_BROWSE, SendMessage );
		return;
	}
	
	// how much have we moved?
	dx = x - lphisto->xMoving;

	// translate to histo coords
	dx = client_to_histo(lphisto, dx, NO /*delta*/);
	newpos = lphisto->orgMoving+dx;
	newpos = bound(newpos, 0, 255);
	
	// any change?
	if (newpos == lphisto->Marks[lphisto->nMoving])
		return;
		
	// undraw mark
	hDC = get_dc(hWindow, lphisto);
	draw_mark(lphisto, hDC, lphisto->nMoving, OFF);
	// set newpos value
	lphisto->Marks[lphisto->nMoving] = newpos;
	// draw the mark
	draw_mark(lphisto, hDC, lphisto->nMoving, ON);
	done_dc(hWindow, hDC, YES /*Free DC?*/);

	// notify parent
	MARK_CHANGING(hWindow, lphisto->nMoving);
}

/***********************************************************************/
LOCAL void Histo_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	LPHISTOCTL lphisto;

	if (!(lphisto = GET_LPHISTOCTL(hWindow)))
		return;
		
	// notify parent and clean up
	if (Control_bTrack )
	{
		if (lphisto->nMoving == HISTO_BAD_MARKER)
			MARK_CHANGED(hWindow, lphisto->nMoving);
		lphisto->nMoving = HISTO_BAD_MARKER;
	}
		
	// to release capture
	Control_OnLButtonUp(hWindow, x, y, keyFlags);
}



//************************************************************************
//			draw_histo
// DESCRIPTION:
//		Draws the actual histogram (background and bars)
//************************************************************************
LOCAL void draw_histo(LPHISTOCTL lphisto, HDC hDC, LPRECT lpRect)
//************************************************************************
{
	LFIXED   xrate, yscl, xoffset;
	int      index, x, left, right, bottom;
	WORD     wHeight;
	HGDIOBJ  hOldPen, hPen;
	int      iHeight;
	LPDWORD  lpHisto;
	BOOL	   bFlip;
   POINT    p;

	hPen = CreatePen( PS_SOLID, 1, RGB(lphisto->Color.red, lphisto->Color.green, lphisto->Color.blue));
	hOldPen = SelectObject(hDC, hPen);

	bFlip = (lphisto->dwStyle & HTS_FLIPX);
	
	xrate = lphisto->xrate;
	lpHisto = lphisto->lpHisto;
	yscl = FGET(DRAW_HEIGHT(lphisto), lphisto->lMaxValue-1);
	xoffset = FDIV2(xrate);
	left = DRAW_LEFT(lphisto);
	right = left + DRAW_WIDTH(lphisto);
	bottom = DRAW_BOTTOM(lphisto);
	
	if (lpHisto )
	{
		for (x = left; x <= right; ++x)
		{
			index = WHOLE(xoffset);
			xoffset += xrate;
			index = bound(index, 0, 255);
			if (bFlip)
				index = 255-index;
			// draw the histo height line at this point
			iHeight = FMUL(( long )lpHisto[index], yscl);
		   	wHeight = ( WORD )iHeight; 
			MoveToEx(hDC, x, bottom, &p);
			LineTo(hDC, x, bottom-wHeight);
		}
	}
	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
}

//************************************************************************
//		draw_mark
// DESCRIPTION:
//			Draws the given mark.
//************************************************************************
LOCAL void draw_mark(LPHISTOCTL lphisto, HDC hDC, int iMark, BOOL bOn)
//************************************************************************
{
	short    OldROP;
	int x;
   POINT    p;
	
	if (!WITHIN(iMark,0,lphisto->nMarks-1))
		return;
	x = lphisto->Marks[iMark];
	x = DRAW_LEFT(lphisto)+((LONG)x*DRAW_WIDTH(lphisto))/255;
	OldROP = SetROP2(hDC, R2_NOT);
	MoveToEx(hDC, x, DRAW_BOTTOM(lphisto), &p);
	LineTo(hDC, x, DRAW_TOP(lphisto));
	SetROP2(hDC, OldROP);
}


//************************************************************************
//		init_histoctl
// DESCRIPTION:
//		sets lphisto to default values
//************************************************************************
LOCAL VOID init_histoctl(LPHISTOCTL lphisto, HWND hHisto)
//************************************************************************
{
	if (!lphisto)
		return;
	lphisto->lpHisto = NULL;			// Actual histo data (256 elements)
	lphisto->lMaxValue = 255; 		// The largest value in array (height of histo)
	lphisto->nMarks = 0;				// number of movable marks
	lphisto->dwStyle = GetWindowLong(hHisto, GWL_STYLE); // Copy of window style for convienence
	lphisto->Color.red = 0;  		// color to draw histo;
	lphisto->Color.green = 0;
	lphisto->Color.blue = 0;
	lphisto->nMoving = HISTO_BAD_MARKER;			// Which mark is grabbed
	lphisto->xBrowse = HISTO_BAD_MARKER;
	
	GetClientRect(hHisto, &lphisto->rArea);
	// one pixel for bevel & one for astral rect
	lphisto->rArea.left += 1;
	lphisto->rArea.top += 1;
	lphisto->rArea.right -= 2;
	lphisto->rArea.bottom -= 2;
	lphisto->xrate = FGET(255, DRAW_WIDTH(lphisto));
}

//************************************************************************
//			get_dc
// PARAMETERS:
//					HWND hWindow- the window to get the HDC for.
//					LPHISTOCTL lphisto- uses for cliping area.
// DESCRIPTION:
//			Creates or uses an exising HDC for the given window cliped to 
//			the histo drawing area.  
// RETURNS:
//			The HDC for drawing or NULL for failure.
//************************************************************************
LOCAL HDC get_dc(HWND hWindow, LPHISTOCTL lphisto)
//***********************************************************************
{
	HDC hDC;
	
	hDC = GetDC(hWindow);
	histo_SelectClipRect(hDC, &lphisto->rArea, NULL);
	return(hDC);
}
	

//************************************************************************
//			done_dc
// PARAMETERS:
//					HWND hWindow- The window for the DC.
//					HDC hDC- Must be from map_get_dc.
//					BOOL bOk2Free- Can the DC be freed?
//
// DESCRIPTION:
//			Call when done with DC from get_dc.
//************************************************************************
LOCAL VOID done_dc(HWND hWindow, HDC hDC, BOOL bOk2Free)
//***********************************************************************
{
	if(hDC && bOk2Free)
		ReleaseDC(hWindow, hDC);
}	

/***********************************************************************/
LOCAL BOOL histo_SelectClipRect(HDC hDC, LPRECT lpNewRect, LPRECT lpOldRect )
/***********************************************************************/
{
	HRGN	hRgn;
	int	iRet;

	// this routines takes care of the fact that windows rects are
	// different than ours.  i.e. - ours include the right/bottom pixels
	if (lpOldRect)
  	{
		GetClipBox(hDC, lpOldRect);
		--lpOldRect->right; --lpOldRect->bottom;
  	}
	++lpNewRect->right; ++lpNewRect->bottom;
	hRgn = CreateRectRgnIndirect(lpNewRect);
	--lpNewRect->right; --lpNewRect->bottom;
	if (!hRgn)
		return(FALSE);
	iRet = SelectClipRgn(hDC, hRgn);
	DeleteObject(hRgn);
	return(iRet != ERROR);
}

//************************************************************************
//			client_to_histo
// PARAMETERS:
//			int x- The value to convert.
//			BOOL bMove- Should we adjust it to the client rect?
// DESCRIPTION:
//			Translates x from client coords to histo coords.
//			Use bMove=NO if its a delta value.
// RETURNS:
//			Translated value.
//************************************************************************
LOCAL int client_to_histo(LPHISTOCTL lphisto, int x, BOOL bMove)
//************************************************************************
{
	if (bMove)
		x -= DRAW_LEFT(lphisto);
	x = x*255L/(DRAW_WIDTH(lphisto));
	return (x);
}
