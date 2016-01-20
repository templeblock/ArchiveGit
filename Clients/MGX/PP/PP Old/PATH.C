// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "data.h"
#include "routines.h"
/*
This function supports the window class PATH_CLASS.  Windows of this type are
generally used in dialog boxes to display the current directory.  When the user
clicks the mouse button within the client area of such a control, the parent
window is notified and may obtain the name of the desired directory by calling
GetWindowText. */


#define PATH_CURPOS         0
#define PATH_DBLCLKSEEN    ( PATH_CURPOS        + sizeof( short))
#define PATH_NHIGHLIGHTED  ( PATH_DBLCLKSEEN    + sizeof( BOOL))
#define PATH_EXTRA         ( PATH_NHIGHLIGHTED  + sizeof( short))
#define LOCAL static
#define MAX_PATH 128

/************************************************************************/
DWORD FAR PASCAL PathControl(hPath,Message,wParm,lParm)
/************************************************************************/
/*  This function handles input to path controls.  All messages except paint,
    mouse, and window text messages are sent to DefWindowProc. */
HWND hPath;
long lParm;
unsigned Message;
WORD wParm;
{
    BOOL bHandled = TRUE;

    if ( Message == WM_PAINT)
        path_paint( hPath);
    else if ( Message == WM_CREATE)
    {
        SetWindowWord( hPath,PATH_CURPOS,0);
        SetWindowWord( hPath,PATH_DBLCLKSEEN,0);
        SetWindowWord( hPath,PATH_NHIGHLIGHTED,0);
    }
    else if ( Message == WM_SETTEXT)
    {
        InvalidateRect( hPath,NULL,TRUE);
        bHandled = FALSE;
    }
    else if ( WITHIN( Message,WM_MOUSEFIRST,WM_MOUSELAST))
        path_mouse( hPath,Message,wParm,(LPPOINT)&lParm);
    else
        bHandled = FALSE;
    return( bHandled ? 0L :( Message == WM_GETTEXT ?
       ( DWORD)get_path_text( hPath,GetWindowWord( hPath,PATH_CURPOS),
       ( LPSTR)lParm,wParm) : DefWindowProc( hPath,Message,wParm,lParm)));
}


/* ***************************** Local Functions **************************** */

/************************************************************************/
LOCAL short NEAR PASCAL get_path_text( hPath,Xpos,lpText,MaxChars)
/************************************************************************/
/*  This function retrieves the text associated with the given path control
    window.  If the request is received as a response to a mouse click, the
    directory name indicated by the user is returned as a result.  Otherwise,
    the entire contents of the control text are returned.  The length of the
    copied string is returned as a result. */
HWND hPath;
short Xpos,
    MaxChars;
LPSTR lpText;
{
    short nChars =( short)DefWindowProc( hPath,WM_GETTEXT,MaxChars,
           ( long)lpText);

    if ( Xpos != 0)
    {
        short Index = 0,
            X = 0;
        HDC hDC = GetDC( hPath);
        DWORD Extent = GetTextExtent( hDC,lpText,nChars),
              Style = GetWindowLong( hPath,GWL_STYLE);

        if ( Style &( SS_CENTER | SS_RIGHT))
        {
            RECT ClientRect;

            GetClientRect( hPath,&ClientRect);
            X =( Style & SS_CENTER) ?( ClientRect.right / 2) -
             ( LOWORD(Extent) / 2) : ClientRect.right - LOWORD(Extent);
        }
        if ( Xpos >= X + LOWORD(Extent))
            Index = nChars - 1;
        else if ( Xpos > X)
            while ( Index < nChars &&( WORD)(GetTextExtent( hDC,
              lpText,Index + 1)) < Xpos - X)
                Index++;
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
LOCAL void NEAR PASCAL highlight_path( hPath,nHighlight)
/************************************************************************/
/*  Highlight the given number of characters in the path */
HWND hPath;
short nHighlight;
{
    HDC hDC = nHighlight > 0 ? GetDC( hPath) : NULL;

    if ( hDC)
    {
        char Path [MAX_PATH];
        RECT HighlightArea;
        long Style = GetWindowLong( hPath,GWL_STYLE);
        DWORD Extent;
        short X = 0,
            nChars = get_path_text( hPath,0,Path,MAX_PATH);

        if ( Style &( SS_CENTER | SS_RIGHT))
        {
            RECT ClientRect;

            GetClientRect( hPath,&ClientRect);
            Extent = GetTextExtent( hDC,Path,nChars);
            X =( Style & SS_CENTER) ?( ClientRect.right / 2) -
             ( LOWORD(Extent) / 2) : ClientRect.right - LOWORD(Extent);
        }
        if ( nChars < nHighlight)
            nHighlight = nChars;
        Path [nHighlight] = '\0';
        Extent = GetTextExtent( hDC,Path,nHighlight);
        SetRect(&HighlightArea,X,0,X + LOWORD(Extent),HIWORD(Extent));
        InvertRect( hDC,&HighlightArea);
        ReleaseDC( hPath,hDC);
    }
}


/************************************************************************/
LOCAL void NEAR PASCAL path_mouse( hPath,Message,Flags,lpPos)
/************************************************************************/
/*  This function processes mouse messages for the given path control window.
    If the mouse button is clicked within the client area of the window, the
    parent is sent a WM_COMMAND message informing it of the user's action.  The
    parent may then obtain the contents of the path indicated by the user by
    calling GetWindowText. */
HWND hPath;
unsigned Message;
WORD Flags;
LPPOINT lpPos;
{
    BOOL bDblClk = GetWindowWord( hPath,PATH_DBLCLKSEEN);

    if ( !bDblClk)
    {
        RECT ClientRect;
        short nHighlight = GetWindowWord( hPath,PATH_NHIGHLIGHTED);

        GetClientRect( hPath,&ClientRect);
        if ( !WITHIN( lpPos->x,ClientRect.left,ClientRect.right) ||
            !WITHIN( lpPos->y,ClientRect.top,ClientRect.bottom))
        {
            highlight_path( hPath,nHighlight);
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
                highlight_path( hPath,nHighlight);
                nHighlight = 0;
                SetWindowWord( hPath,PATH_CURPOS,lpPos->x);
                SendMessage( GetParent( hPath),WM_COMMAND,
                    GetWindowWord( hPath,GWW_ID),
                    MAKELONG( hPath,bDblClk ? BN_DOUBLECLICKED : BN_CLICKED));
                SetWindowWord( hPath,PATH_CURPOS,0);
                if ( bDblClk)
                    SetCapture( hPath);
                else
                    ReleaseCapture( );
            }
            else if ( (Flags & MK_LBUTTON) &&( nPicked = get_path_text( hPath,
                max( lpPos->x,1),Path,MAX_PATH)) != nHighlight)
            {
                SetCapture( hPath);
                highlight_path( hPath,nHighlight);
                highlight_path( hPath,nHighlight = nPicked);
            }
        }
        SetWindowWord( hPath,PATH_NHIGHLIGHTED,nHighlight);
    }
    else if ( !(bDblClk = Message != WM_LBUTTONUP))
        ReleaseCapture( );
    SetWindowWord( hPath,PATH_DBLCLKSEEN,bDblClk);
}


/************************************************************************/
LOCAL void NEAR PASCAL path_paint( hWnd)
/************************************************************************/
/*  This function paints a path control.  It displays the text associated with
    the window against the proper background color. */
HWND hWnd;
{
PAINTSTRUCT ps;
HDC hDC;
char Path[MAX_PATH];
DWORD OldTextColor, Extent;
long Style;
short X, nChars;
RECT ClientRect;

if (  !(hDC = BeginPaint(hWnd,&ps)) )
	return;
GetClientRect( hWnd,&ClientRect);
EraseControlBackground( hDC, hWnd, &ClientRect, CTLCOLOR_STATIC );

X = 0;
Style = GetWindowLong( hWnd,GWL_STYLE);
nChars = get_path_text( hWnd,0,Path,MAX_PATH);
if ( Style & ( SS_CENTER | SS_RIGHT))
	{
	Extent = GetTextExtent( hDC,Path,nChars);
	X =( Style & SS_CENTER) ? ( ClientRect.right / 2) - 
		(LOWORD(Extent) / 2) : ClientRect.right - LOWORD(Extent);
	}
SetBkMode(hDC,TRANSPARENT);
OldTextColor = SetTextColor( hDC, GetSysColor(COLOR_WINDOWTEXT) );
TextOut( hDC,X,0,Path,nChars);
SetTextColor( hDC,OldTextColor);
EndPaint( hWnd,&ps);
highlight_path( hWnd,GetWindowWord( hWnd,PATH_NHIGHLIGHTED));
}
