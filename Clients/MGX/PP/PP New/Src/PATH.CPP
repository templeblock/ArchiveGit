// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include "pp.h"

/*
This function supports the window class PATH_CLASS.  Windows of this type are
generally used in dialog boxes to display the current directory.  When the user
clicks the mouse button within the client area of such a control, the parent
window is notified and may obtain the name of the desired directory by calling
GetWindowText. */

/***********************************************************************/
/***********************************************************************/
// PATH CONTROL
/***********************************************************************/
/**********************************************************************/

LOCAL void Path_OnPaint(HWND hWindow);
LOCAL BOOL Path_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct);
LOCAL void Path_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Path_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Path_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Path_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Path_OnMButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Path_OnMButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Path_OnMButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Path_OnRButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Path_OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Path_OnRButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
int Path_OnGetText(HWND hWindow, int cchTextMax, LPTSTR lpszText);
LOCAL short Path_GetText( HWND hPath,short Xpos,LPSTR lpText,int MaxChars);
LOCAL void Path_Highlight( HWND hPath,short nHighlight);
LOCAL void Path_HandleMouse( HWND hPath,UINT Message,UINT Flags, int x, int y);

/************************************************************************/
LONG WINPROC EXPORT Path_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/************************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);

	HANDLE_MSG(hWindow, WM_CREATE, Path_OnCreate);
	HANDLE_MSG(hWindow, WM_PAINT, Path_OnPaint);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Path_OnMouseMove);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Path_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, Path_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Path_OnLButtonDblClk);
	HANDLE_MSG(hWindow, WM_MBUTTONDOWN, Path_OnMButtonDown);
	HANDLE_MSG(hWindow, WM_MBUTTONUP, Path_OnMButtonUp);
	HANDLE_MSG(hWindow, WM_MBUTTONDBLCLK, Path_OnMButtonDblClk);
	HANDLE_MSG(hWindow, WM_RBUTTONDOWN, Path_OnRButtonDown);
	HANDLE_MSG(hWindow, WM_RBUTTONUP, Path_OnRButtonUp);
	HANDLE_MSG(hWindow, WM_RBUTTONDBLCLK, Path_OnRButtonDblClk);
	HANDLE_MSG(hWindow, WM_GETTEXT, Path_OnGetText);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL BOOL Path_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
SetWindowWord( hWnd,PATH_CURPOS,0);
SetWindowWord( hWnd,PATH_DBLCLKSEEN,0);
SetWindowWord( hWnd,PATH_NHIGHLIGHTED,0);
return(TRUE);
}

/***********************************************************************/
LOCAL void Path_OnPaint(HWND hWnd)
/***********************************************************************/
{
PAINTSTRUCT ps;
HDC hDC;
char Path[MAX_PATH];
DWORD OldTextColor;
long Style;
short X, nChars;
RECT ClientRect;
SIZE size;

if (  !(hDC = BeginPaint(hWnd,&ps)) )
	return;
GetClientRect( hWnd,&ClientRect);
EraseControlBackground( hDC, hWnd, &ClientRect, CTLCOLOR_STATIC );

X = 0;
Style = GetWindowLong( hWnd,GWL_STYLE);
nChars = Path_GetText( hWnd,0,Path,MAX_PATH);
if ( Style & ( SS_CENTER | SS_RIGHT))
	{
	GetTextExtentPoint( hDC,Path,nChars, &size);
	X =( Style & SS_CENTER) ? ( ClientRect.right / 2) - 
		(size.cx / 2) : ClientRect.right - size.cx;
	}
SetBkMode(hDC,TRANSPARENT);
OldTextColor = SetTextColor( hDC, GetSysColor(COLOR_WINDOWTEXT) );
TextOut( hDC,X,0,Path,nChars);
SetTextColor( hDC,OldTextColor);
EndPaint( hWnd,&ps);
Path_Highlight( hWnd,GetWindowWord( hWnd,PATH_NHIGHLIGHTED));
}

/***********************************************************************/
LOCAL void Path_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Path_HandleMouse(hWindow,WM_MOUSEMOVE,keyFlags,x,y);
}

/***********************************************************************/
LOCAL void Path_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Path_HandleMouse(hWindow,WM_LBUTTONDOWN,keyFlags,x,y);
}

/***********************************************************************/
LOCAL void Path_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Path_HandleMouse(hWindow,WM_LBUTTONUP,keyFlags,x,y);
}

/***********************************************************************/
LOCAL void Path_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Path_HandleMouse(hWindow,WM_LBUTTONDBLCLK,keyFlags,x,y);
}

/***********************************************************************/
LOCAL void Path_OnMButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Path_HandleMouse(hWindow,WM_MBUTTONDOWN,keyFlags,x,y);
}

/***********************************************************************/
LOCAL void Path_OnMButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Path_HandleMouse(hWindow,WM_MBUTTONUP,keyFlags,x,y);
}

/***********************************************************************/
LOCAL void Path_OnMButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Path_HandleMouse(hWindow,WM_MBUTTONDBLCLK,keyFlags,x,y);
}

/***********************************************************************/
LOCAL void Path_OnRButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Path_HandleMouse(hWindow,WM_RBUTTONDOWN,keyFlags,x,y);
}

/***********************************************************************/
LOCAL void Path_OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Path_HandleMouse(hWindow,WM_RBUTTONUP,keyFlags,x,y);
}

/***********************************************************************/
LOCAL void Path_OnRButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Path_HandleMouse(hWindow,WM_RBUTTONDBLCLK,keyFlags,x,y);
}

/************************************************************************/
int Path_OnGetText(HWND hWindow, int cchTextMax, LPTSTR lpszText)
/************************************************************************/
{
return(Path_GetText( hWindow, GetWindowWord( hWindow,PATH_CURPOS),
       lpszText, cchTextMax));
}

/* ***************************** Local Functions **************************** */

/************************************************************************/
static short Path_GetText( HWND hPath, short Xpos, LPSTR lpText, int MaxChars )
/************************************************************************/
/*  This function retrieves the text associated with the given path control
    window.  If the request is received as a response to a mouse click, the
    directory name indicated by the user is returned as a result.  Otherwise,
    the entire contents of the control text are returned.  The length of the
    copied string is returned as a result. */
{
    short nChars =( short)DefWindowProc( hPath,WM_GETTEXT,MaxChars,
           ( long)lpText);

    if ( Xpos != 0)
    {
	 	SIZE size;
		DWORD Style;
        short Index = 0,
            X = 0;
        HDC hDC = GetDC( hPath);
        GetTextExtentPoint( hDC,lpText,nChars, &size),
        Style = GetWindowLong( hPath,GWL_STYLE);
					 
        if ( Style &( SS_CENTER | SS_RIGHT))
        {
            RECT ClientRect;

            GetClientRect( hPath,&ClientRect);
            X =( Style & SS_CENTER) ?( ClientRect.right / 2) -
             ( LOWORD(size.cx) / 2) : ClientRect.right - LOWORD(size.cx);
        }
        if ( Xpos >= X + LOWORD(size.cx))
            Index = nChars - 1;
        else if ( Xpos > X)
		 	{
			GetTextExtentPoint(hDC, lpText, Index+1, &size);
            while ( Index < nChars && size.cx < Xpos - X)
			   	{
               Index++;
				GetTextExtentPoint(hDC, lpText, Index+1, &size);
				}
			}
        while ( lpText[Index] != '\\' && Index < nChars)
            Index++;
//            Index = CHRNEXT( lpText + Index) - lpText;
        if ( Index < nChars)
            lpText[nChars = Index + 1] = '\0';
        ReleaseDC( hPath,hDC);
    }
    return( nChars);
}


/************************************************************************/
static void Path_Highlight( HWND hPath, short nHighlight )
/************************************************************************/
/*  Highlight the given number of characters in the path */
{
	 SIZE size;
    HDC hDC = nHighlight > 0 ? GetDC( hPath) : NULL;

    if ( hDC)
    {
        char Path [MAX_PATH];
        RECT HighlightArea;
        long Style = GetWindowLong( hPath,GWL_STYLE);
        short X = 0,
        nChars = Path_GetText( hPath,0,Path,MAX_PATH);

        if ( Style &( SS_CENTER | SS_RIGHT))
        {
            RECT ClientRect;

            GetClientRect( hPath,&ClientRect);
            GetTextExtentPoint( hDC,Path,nChars, &size);
            X =( Style & SS_CENTER) ?( ClientRect.right / 2) -
             ( size.cx / 2) : ClientRect.right - size.cx;
        }
        if ( nChars < nHighlight)
            nHighlight = nChars;
        Path [nHighlight] = '\0';
        GetTextExtentPoint( hDC,Path,nHighlight, &size);
        SetRect(&HighlightArea,X,0,X + size.cx, size.cy);
        InvertRect( hDC,&HighlightArea);
        ReleaseDC( hPath,hDC);
    }
}


/************************************************************************/
static void Path_HandleMouse( HWND hPath, UINT Message, UINT Flags,
                                 int x, int y )
/************************************************************************/
/*  This function processes mouse messages for the given path control window.
    If the mouse button is clicked within the client area of the window, the
    parent is sent a WM_COMMAND message informing it of the user's action.  The
    parent may then obtain the contents of the path indicated by the user by
    calling GetWindowText. */
{
    BOOL bDblClk = GetWindowWord( hPath,PATH_DBLCLKSEEN);

    if ( !bDblClk)
    {
        RECT ClientRect;
        short nHighlight = GetWindowWord( hPath,PATH_NHIGHLIGHTED);

        GetClientRect( hPath,&ClientRect);
        if ( !WITHIN( x,ClientRect.left,ClientRect.right) ||
            !WITHIN( y,ClientRect.top,ClientRect.bottom))
        {
            Path_Highlight( hPath,nHighlight);
            nHighlight = 0;
            ReleaseCapture( );
        }
        else
        {
            char Path[MAX_PATH];
            short nPicked;

            if ( Message == WM_LBUTTONUP ||
               ( bDblClk = Message == WM_LBUTTONDBLCLK))
            {
                Path_Highlight( hPath,nHighlight);
                nHighlight = 0;
                SetWindowWord( hPath,PATH_CURPOS,(WORD)x);
                SEND_WM_COMMAND( GetParent( hPath), GetDlgCtrlID( hPath),
				 			hPath, bDblClk ? BN_DOUBLECLICKED : BN_CLICKED);
                SetWindowWord( hPath,PATH_CURPOS,0);
                if ( bDblClk)
                    SetCapture( hPath);
                else
                    ReleaseCapture( );
            }
            else if ( (Flags & MK_LBUTTON) &&( nPicked = Path_GetText( hPath,
                (short)max( x,1),Path,MAX_PATH)) != nHighlight)
            {
                SetCapture( hPath);
                Path_Highlight( hPath,nHighlight);
                Path_Highlight( hPath,nHighlight = nPicked);
            }
        }
        SetWindowWord( hPath,PATH_NHIGHLIGHTED,(WORD)nHighlight);
    }
    else if ( !(bDblClk = Message != WM_LBUTTONUP))
        ReleaseCapture( );
    SetWindowWord( hPath,PATH_DBLCLKSEEN,(WORD)bDblClk);
}




