// (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// mapctl.c for map control
#include <math.h>
#include "ctlutil.h"
#include "tagctl.h"
#include "macros.h"

// POINTER TO MEMORY
#define GWL_TAGSTRUCT	(GWW_START+0)			
// Number of extra memory for window words
#define TAG_EXTRA	(GWW_START+4)

// struct allocated for every tagcontrol
typedef struct _tagctl
{
	int Icons[MAX_TAG_MARKS];	// the icon id's
	int Marks[MAX_TAG_MARKS];	// The moveable marks
	int nMarks;				// number of movable marks
	int iMaxRange;			// the control goes from 0 to iMaxRange
	LFIXED xrate;		// rate to convert from display to MaxRange.
	RECT rArea;			// Client rect of ACTIVE area of control.
	int nMoving;			// Which mark is grabbed
	int xMoving;			// Original position grabbed (screen coords)
	int orgMoving;			// Original position of moving mark (tag coords)
}TAGCTL, FAR * LPTAGCTL;

#define TAG_SIZE 11

// helper macros
#define GET_LPTAGCTL(hDlg) \
	(hDlg ? (LPTAGCTL)GetWindowLong(hDlg,GWL_TAGSTRUCT) : NULL)
	
// Parent notification
#define MARK_CHANGING(hWindow, mark) \
	(FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, \
		TN_CHANGING_FIRST+mark, SendMessage ))

// Parent notification
#define MARK_CHANGED(hWindow, mark) \
	(FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, \
		TN_CHANGED_FIRST+mark, SendMessage ))

#define DRAW_WIDTH(lptag) \
	(RectWidth(&lptag->rArea)-4)
#define DRAW_HEIGHT(lptag) \
	(RectHeight(&lptag->rArea))
#define DRAW_LEFT(lptag) \
	(lptag->rArea.left + 1)
#define DRAW_RIGHT(lptag) \
	(lptag->rArea.right-2)
#define DRAW_BOTTOM(lptag) \
	(lptag->rArea.bottom)
#define DRAW_TOP(lptag) \
	(lptag->rArea.top)

LOCAL void draw_tag(HWND hControl, LPTAGCTL lptag, HDC hDC, int iMark, LPRECT lprUpdate, BOOL bOn);
LOCAL HDC get_dc(HWND hWindow, LPTAGCTL lptag);
LOCAL VOID done_dc(HWND hWindow, HDC hDC, BOOL bOk2Free);
LOCAL VOID init_tagctl(LPTAGCTL lptag, HWND hTag);
LOCAL BOOL tag_SelectClipRect(HDC hDC, LPRECT lpNewRect, LPRECT lpOldRect );
LOCAL int client_to_tag(LPTAGCTL lptag, int x, BOOL bMove);

//************************************************************************
//************************************************************************
//	TAG Access Functions
//************************************************************************
//************************************************************************

//************************************************************************
//		Tag_Init
//	PARAMETERS:
//			int nMarks- How many marks should there be?
//			int iMaxValue - tags run from 0 to Max value.
//			LPINT lpIconId- Array of size nMarks giving the icon ID's.
//	DESCRIPTION: 
//			Sets the above parameters and redraws the control.
//			Any item whichis -1 is not changed (except lpIconId). 
//	RETURNS: 
//			FALSE if allocation fails. (caller should put up memory error)
//************************************************************************
BOOL Tag_Init(HWND hTag, int nMarks, int iMaxValue, LPINT lpIconId)
//************************************************************************
{
	LPTAGCTL lptag;
	int i;
	
	if (!(lptag = GET_LPTAGCTL(hTag)))
	{
		lptag = (LPTAGCTL)Alloc(sizeof(TAGCTL));
		if (!lptag)
			return(FALSE);
		init_tagctl(lptag, hTag);
		SetWindowLong( hTag, GWL_TAGSTRUCT, (long)lptag);
	}
	
	if (iMaxValue != -1)
	{
		lptag->iMaxRange = iMaxValue;
		lptag->xrate = FGET(lptag->iMaxRange, DRAW_WIDTH(lptag));
	}
	if (nMarks != -1)
	{
		lptag->nMarks = Bound(nMarks,0, MAX_TAG_MARKS);
		for(i=0;i<lptag->nMarks;i++)
			lptag->Marks[i] = (i*lptag->iMaxRange)/lptag->nMarks;
		
	}
	if (lpIconId)
	{
		for(i=0;i<lptag->nMarks;i++)
			lptag->Icons[i] = lpIconId[i];
	}
	
	// draw it!
    InvalidateRect (hTag,NULL,TRUE /*Erase background*/);
//   UpdateWindow (hTag); 
	return(TRUE);
}

//************************************************************************
//		Tag_SetMark
//	PARAMETERS:
//			int iMark- Which mark to set.
//			int iValue- What to set it to.
//			BOOL bConstrain- Ensure that it is between its neighbors?
//	DESCRIPTION: 
//			Sets the given marker.
//			update the display.
//	RETURNS: 
//			The new position of the marker. 
//			TAG_BAD_MARKER for error (iMark out of bounds etc)
//************************************************************************
int Tag_SetMark(HWND hTag, int iMark, int iValue, BOOL bConstrain)
//************************************************************************
{
	LPTAGCTL lptag;
	HDC hDC;
	int iMin, iMax;
	
	if (!(lptag = GET_LPTAGCTL(hTag)))
		return(TAG_BAD_MARKER);
	
	if (!WITHIN (iMark+1, 0, lptag->nMarks))
		return(TAG_BAD_MARKER);
	
	if (bConstrain)
	{
		// ensure that it is within its neighbors
		iMax = (iMark+1<lptag->nMarks) ?  lptag->Marks[iMark+1]:lptag->iMaxRange;
		iMax = (iMark > 0) ? lptag->Marks[iMark+1]:0;
		iValue = Bound(iValue, iMin, iMax);
	}
	
	if (lptag->Marks[iMark] == iValue)
		return(iValue);
	// undraw old mark
	hDC = get_dc(hTag, lptag);
	draw_tag(hTag, lptag, hDC, iMark, NULL, OFF);
	// set mark
	lptag->Marks[iMark] = iValue;
	// draw new mark
	draw_tag(hTag, lptag, hDC, iMark, NULL, ON);
	done_dc(hTag, hDC, YES /*Free DC?*/);
	return(iValue);
}

//************************************************************************
//			Tag_GetMark
//	PARAMETERS:
//			int iMark- The mark to get.
//	RETURNS: 
//			The value of the given hitso marker.
//			TAG_BAD_MARKER for error (iMark out of bounds etc)
//************************************************************************
int  Tag_GetMark(HWND hTag, int iMark)
//************************************************************************
{
	LPTAGCTL lptag;
	
	if (!(lptag = GET_LPTAGCTL(hTag)))
		return(TAG_BAD_MARKER);
	if (!WITHIN(iMark+1, 0, lptag->nMarks))
		return(TAG_BAD_MARKER);
	return(lptag->Marks[iMark]);
}

BOOL Tag_RegisterClass (HINSTANCE hInstance, UINT ClassScope)
{
	WNDCLASS WndClass;

	/* Setup and define the window class labeled frame control */
	/* Setup and define the window class for a joystick */
	WndClass.hIcon		= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPTSTR)_T("tag");
	WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
	WndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	WndClass.hInstance	= hInstance;
	WndClass.style		= CS_DBLCLKS | ClassScope;
	WndClass.lpfnWndProc	= Tag_Control;
	WndClass.cbClsExtra	= 0;
	WndClass.cbWndExtra	= TAG_EXTRA;

	return(RegisterClass( &WndClass ));
}

BOOL Tag_UnRegisterClass (HINSTANCE hInstance)
{
    return (UnregisterClass ((LPTSTR)_T("tag"), hInstance));
}


/***********************************************************************/
/***********************************************************************/
// TAG CONTROL
/***********************************************************************/
/**********************************************************************/

LOCAL BOOL Tag_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct);
LOCAL void Tag_OnDestroy(HWND hWindow);
LOCAL void Tag_OnPaint(HWND hWindow);
LOCAL void Tag_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Tag_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Tag_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Tag_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Tag_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Tag_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Tag_HandleFocus(HWND hWindow, BOOL fSetFocus);
LOCAL void Tag_Draw( HDC hDC, LPRECT lpRect, DWORD dwMaxValue, LPDWORD lpTag, LPRGB lpRGB, DWORD dwNewMarks, DWORD dwOldMarks );


/***********************************************************************/
LONG WINPROC EXPORT Tag_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_ENABLE, Control_OnEnable);
		HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);
		HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);
	
		HANDLE_MSG(hWindow, WM_CREATE, Tag_OnCreate);
	   	HANDLE_MSG(hWindow, WM_DESTROY, Tag_OnDestroy);
		HANDLE_MSG(hWindow, WM_PAINT, Tag_OnPaint);
		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Tag_OnLButtonDown);
		HANDLE_MSG(hWindow, WM_LBUTTONUP, Tag_OnLButtonUp);
		HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Tag_OnLButtonDblClk);
		HANDLE_MSG(hWindow, WM_MOUSEMOVE, Tag_OnMouseMove);
	
		default:
			return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL BOOL Tag_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	LPTAGCTL lptag;

	lptag = (LPTAGCTL)Alloc(sizeof(TAGCTL));
	if (lptag)
		init_tagctl(lptag, hWindow);
	SetWindowLong( hWindow, GWL_TAGSTRUCT, (long)lptag);
	return(TRUE);
}

/***********************************************************************/
LOCAL void Tag_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	LPTAGCTL lptag;

	if (lptag = GET_LPTAGCTL(hWindow))
	{
		FreeUp((LPTR)lptag);
		SetWindowLong( hWindow, GWL_TAGSTRUCT, (long)NULL);
	}
	Control_OnDestroy(hWindow);
}


/***********************************************************************/
LOCAL void Tag_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC hDC;
	int mark;
	BOOL bErase;
	LPTAGCTL lptag;
	
	bErase = ps.fErase;
	hDC = BeginPaint( hWindow, &ps );
	if (!(lptag = GET_LPTAGCTL(hWindow)))
		goto Done;
	// erase background
	if (bErase)
		Ctl_EraseBackground( hWindow, hDC, &ps.rcPaint, CTLCOLOR_DLG );
	// draw tags
	for (mark=0; mark<lptag->nMarks; mark++)
		draw_tag(hWindow, lptag, hDC, mark, &ps.rcPaint, ON);
Done:
	EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Tag_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	int i, pos;
	int dist, bestdist;
	LPTAGCTL lptag;
	
	if ( Control_bTrack )
		return;
	if (!(lptag = GET_LPTAGCTL(hWindow)))
		return;

	// find cloeset mark
	lptag->nMoving = TAG_BAD_MARKER;
	pos = client_to_tag(lptag, x, YES /*absolute coords*/);
	bestdist = client_to_tag(lptag, TAG_SIZE, NO /*delta*/);
	for (i=0; i<lptag->nMarks; i++)
	{
		dist = abs(lptag->Marks[i]-pos);
		if(dist < bestdist)
			lptag->nMoving = i; 
	}
	lptag->xMoving = x;
	if (lptag->nMoving != TAG_BAD_MARKER)
		lptag->orgMoving = lptag->Marks[lptag->nMoving];

	// Set stuff needed during tracking
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	SetCapture( hWindow ); Control_bTrack = TRUE;
}

/***********************************************************************/
LOCAL void Tag_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
}

/***********************************************************************/
LOCAL void Tag_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	LPTAGCTL lptag;
	int dx, newpos;
	HDC hDC;
	
	if (!Control_bTrack )
		return;
	if (!(lptag = GET_LPTAGCTL(hWindow)))
		return;
	// are we moving?
	if (!WITHIN(lptag->nMoving,0,lptag->nMarks))
		return;
	
	// how much have we moved?
	dx = x - lptag->xMoving;

	// translate to tag coords
	dx = client_to_tag(lptag, dx, NO /*delta*/);
	newpos = lptag->orgMoving+dx;
	newpos = bound(newpos, 0, lptag->iMaxRange);
	
	// any change?
	if (newpos == lptag->Marks[lptag->nMoving])
		return;
		
	// undraw mark
	hDC = get_dc(hWindow, lptag);
	draw_tag(hWindow, lptag, hDC, lptag->nMoving, NULL, OFF);
	// set newpos value
	lptag->Marks[lptag->nMoving] = newpos;
	// draw the mark
	draw_tag(hWindow, lptag, hDC, lptag->nMoving, NULL, ON);
	done_dc(hWindow, hDC, YES /*Free DC?*/);

	// notify parent
	MARK_CHANGING(hWindow, lptag->nMoving);
}

/***********************************************************************/
LOCAL void Tag_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	LPTAGCTL lptag;

	if (!(lptag = GET_LPTAGCTL(hWindow)))
		return;
		
	// notify parent and clean up
	if (Control_bTrack )
	{
		if (lptag->nMoving == TAG_BAD_MARKER)
			MARK_CHANGED(hWindow, lptag->nMoving);
		lptag->nMoving = TAG_BAD_MARKER;
	}
		
	// to release capture
	Control_OnLButtonUp(hWindow, x, y, keyFlags);
}



//************************************************************************
//		draw_tag
// DESCRIPTION:
//			Draws the given mark.
//************************************************************************
LOCAL void draw_tag(HWND hControl, LPTAGCTL lptag, HDC hDC, int iMark, LPRECT lprUpdate, BOOL bOn)
//************************************************************************
{
	int x, i;
	HICON hResource;
	RECT rIcon, dummy;
	
	if (!WITHIN(iMark,0,lptag->nMarks-1))
		return;
		
	x = lptag->Marks[iMark];
	x = DRAW_LEFT(lptag)+((LONG)x*DRAW_WIDTH(lptag))/lptag->iMaxRange;
	x -= TAG_SIZE/2;	
	rIcon.left = x;
	rIcon.top = DRAW_TOP(lptag);
	rIcon.right = rIcon.left+TAG_SIZE+1;
	rIcon.bottom = rIcon.top+TAG_SIZE+1;
	
	// do we need to draw at all?
	if (lprUpdate)
		if (!IntersectRect(&dummy, &rIcon, lprUpdate))
			return;
			
	if (bOn)
	{
		hResource =LoadIcon( Ctl_GetIconInstance(), 
							MAKEINTRESOURCE(lptag->Icons[iMark]));
		if (!hResource)
			return;
			
		DrawIcon( hDC, rIcon.left, rIcon.top, hResource);
		DestroyIcon(hResource);
		return;
	}
	else // off
	{
		// undraw self.
		Ctl_EraseBackground( hControl, hDC, &rIcon, CTLCOLOR_DLG );
		// redraw other if neded
		for (i=0; i<lptag->nMarks;i++)
		{
			if (i!=iMark)
				draw_tag(hControl,lptag,hDC,i, &rIcon, ON);
		}
	}
}


//************************************************************************
//		init_tagctl
// DESCRIPTION:
//		sets lptag to default values
//************************************************************************
LOCAL VOID init_tagctl(LPTAGCTL lptag, HWND hTag)
//************************************************************************
{
	lptag->iMaxRange = 255;
	lptag->nMarks = 0;				// number of movable marks
	lptag->nMoving = TAG_BAD_MARKER;			// Which mark is grabbed
	
	GetClientRect(hTag, &lptag->rArea);
	lptag->xrate = FGET(lptag->iMaxRange,DRAW_WIDTH(lptag));
}

//************************************************************************
//			get_dc
// PARAMETERS:
//					HWND hWindow- the window to get the HDC for.
//					LPTAGCTL lptag- uses for cliping area.
// DESCRIPTION:
//			Creates or uses an exising HDC for the given window cliped to 
//			the tag drawing area.  
// RETURNS:
//			The HDC for drawing or NULL for failure.
//************************************************************************
LOCAL HDC get_dc(HWND hWindow, LPTAGCTL lptag)
//***********************************************************************
{
	HDC hDC;
	
	hDC = GetDC(hWindow);
	tag_SelectClipRect(hDC, &lptag->rArea, NULL);
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
LOCAL BOOL tag_SelectClipRect(HDC hDC, LPRECT lpNewRect, LPRECT lpOldRect )
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
//			client_to_tag
// PARAMETERS:
//			int x- The value to convert.
//			BOOL bMove- Should we adjust it to the client rect?
// DESCRIPTION:
//			Translates x from client coords to tag coords.
//			Use bMove=NO if its a delta value.
// RETURNS:
//			Translated value.
//************************************************************************
LOCAL int client_to_tag(LPTAGCTL lptag, int x, BOOL bMove)
//************************************************************************
{
	if (bMove)
		x -= DRAW_LEFT(lptag);
	x = (long)x*lptag->iMaxRange/(DRAW_WIDTH(lptag));
	return (x);
}
