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

/************************************************************************/
void SetScrollBar( hDlg, idControl, Value, MinValue, MaxValue )
/************************************************************************/
HWND hDlg;
int idControl, MinValue, MaxValue;
{
hDlg = GetDlgItem( hDlg, idControl );
SetScrollRange( hDlg, SB_CTL, MinValue, MaxValue, NO/*fDraw*/ );
SetScrollPos( hDlg, SB_CTL, Value, NO /*fDraw*/ );
}


/************************************************************************/
BOOL HandleMiniScroll( hDlg, wParam, lParam )
/************************************************************************/
HWND hDlg;
WORD wParam;
LONG lParam;
{
int id, iValue;
BOOL Bool;

if ( !(id = GetDlgCtrlID( HIWORD(lParam) )) )
	return( NO );
iValue = GetDlgItemInt( hDlg, id, &Bool, YES );
if ( wParam == SB_LINEUP || wParam == SB_PAGEUP )
	iValue++;
else
if ( wParam == SB_LINEDOWN || SB_PAGEDOWN )
	iValue--;
else	return( YES );
SetDlgItemSpin( hDlg, id, iValue, YES );
SendMessage( hDlg, WM_COMMAND, id, (long)EN_CHANGE<<16 );
return( YES );
}


/************************************************************************/
int HandleScrollBar( hDlg, idControl, wParam, lParam, MinValue, MaxValue  )
/************************************************************************/
HWND	 hDlg;
int	 idControl;
WORD	 wParam;
LONG	 lParam;
int MinValue, MaxValue;
{
int Style, Value;
#define SCROLLPIXELS 1

hDlg = GetDlgItem( hDlg, idControl );
Style = SB_CTL;

Value = GetScrollPos( hDlg, Style );
switch ( wParam )
    {
    case SB_TOP:
    case SB_BOTTOM:
	MessageBeep(0);
	break;

    case SB_PAGEUP:
	Value -= (10*SCROLLPIXELS);
	if ( Value < MinValue || Value > MaxValue )
		break;
	SetScrollPos( hDlg, Style, Value, YES );
	break;

    case SB_PAGEDOWN:
	Value += (10*SCROLLPIXELS);
	if ( Value < MinValue || Value > MaxValue )
		break;
	SetScrollPos( hDlg, Style, Value, YES );
	break;

    case SB_LINEUP:
	Value -= SCROLLPIXELS;
	if ( Value < MinValue || Value > MaxValue )
		break;
	SetScrollPos( hDlg, Style, Value, YES );
	break;

    case SB_LINEDOWN:
	Value += SCROLLPIXELS;
	if ( Value < MinValue || Value > MaxValue )
		break;
	SetScrollPos( hDlg, Style, Value, YES );
	break;

    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
	Value = LOWORD(lParam);
	if ( Value < MinValue || Value > MaxValue )
		break;
	SetScrollPos( hDlg, Style, Value, YES );
	break;
    }

return( Value );
}


#ifdef UNUSED
    case WM_KEYDOWN:
	/* Translate keyboard messages to scroll commands */
	switch (wParam)
	    {
	    case VK_UP:    PostMessage (hDlg, WM_VSCROLL, SB_LINEUP,   0L);
		break;
	    case VK_DOWN:  PostMessage (hDlg, WM_VSCROLL, SB_LINEDOWN, 0L);
		break;
	    case VK_PRIOR: PostMessage (hDlg, WM_VSCROLL, SB_PAGEUP,   0L);
		break;
	    case VK_NEXT:  PostMessage (hDlg, WM_VSCROLL, SB_PAGEDOWN, 0L);
		break;
	    case VK_HOME:  PostMessage (hDlg, WM_HSCROLL, SB_PAGEUP,   0L);
		break;
	    case VK_END:   PostMessage (hDlg, WM_HSCROLL, SB_PAGEDOWN, 0L);
		break;
	    case VK_LEFT:  PostMessage (hDlg, WM_HSCROLL, SB_LINEUP,   0L);
		break;
	    case VK_RIGHT: PostMessage (hDlg, WM_HSCROLL, SB_LINEDOWN, 0L);
		break;
	    }
	break;

    case WM_KEYUP:
	switch (wParam)
	    {
	    case VK_UP:
	    case VK_DOWN:
	    case VK_PRIOR:
	    case VK_NEXT:
		  PostMessage (hDlg, WM_VSCROLL, SB_ENDSCROLL, 0L);
		  break;

	    case VK_HOME:
	    case VK_END:
	    case VK_LEFT:
	    case VK_RIGHT:
		  PostMessage (hDlg, WM_HSCROLL, SB_ENDSCROLL, 0L);
		  break;
	    }
	break;
#endif
