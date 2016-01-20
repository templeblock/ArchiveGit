//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "ctxhelp.h"
#include "afxpriv.h"

/***********************************************************************/
/***********************************************************************/
// COMMON DIALOG IMPLEMENTATION
/***********************************************************************/
/***********************************************************************/

static ITEMID Dialog_ID;

LRESULT Dialog_OnSizeParent(HWND hwnd, AFX_SIZEPARENTPARAMS* lpLayout);

#define HANDLE_WM_SIZEPARENT(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (AFX_SIZEPARENTPARAMS *)(lParam)))

/***********************************************************************/
BOOL Dialog_HandleDefaults(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam,
							ITEMID idDialog)
/***********************************************************************/
{
Dialog_ID = idDialog;
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_SETCURSOR, Dialog_OnSetCursor);
	HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, Dialog_OnMeasureItem);
	HANDLE_DLGMSG(hDlg, WM_DRAWITEM, Dialog_OnDrawItem);
	HANDLE_DLGMSG(hDlg, WM_MENUSELECT, Dialog_OnMenuSelect);
	HANDLE_DLGMSG(hDlg, WM_CLOSE, Dialog_OnClose);
//	HANDLE_CONTROL_COLORS(hDlg, Dialog_OnCtlColor);
	HANDLE_DLGMSG(hDlg, WM_DESTROY, Dialog_OnDestroy);
	HANDLE_DLGMSG(hDlg, WM_CREATE, Dialog_OnCreate);
	HANDLE_DLGRET(hDlg, WM_SIZEPARENT, Dialog_OnSizeParent);
	HANDLE_DLGMSG(hDlg, WM_NOTIFY, Dialog_OnNotify);
	MONITOR_DLGMSG(hDlg, WM_HSCROLL, Dialog_OnScroll);
	MONITOR_DLGMSG(hDlg, WM_VSCROLL, Dialog_OnScroll);
	default:
	return(FALSE);
	}
}

LRESULT Dialog_OnSizeParent(HWND hwnd, AFX_SIZEPARENTPARAMS* lpLayout)
{
	// align the control bar
	CRect 	rect, r;
	CSize 	size;
	BOOL	Ribbon = hwnd == AstralDlgGet(Tool.idRibbon);
	BOOL	Status = hwnd == AstralDlgGet(IDD_STATUS);

	rect.CopyRect(&lpLayout->rect);
	CSize sizeAvail = rect.Size();  // maximum size available

	// get maximum requested size, size of ribbon
   	GetClientRect(GetParent(hwnd), &r);
	size.cx = r.Width();
	if (Ribbon)
		size.cy = 42;
	else if (Status)
		size.cy = 22;

	size.cx = min(size.cx, sizeAvail.cx);
	size.cy = min(size.cy, sizeAvail.cy);

	lpLayout->sizeTotal.cy += size.cy;
	lpLayout->sizeTotal.cx = max(lpLayout->sizeTotal.cx, size.cx);
	if (Ribbon)
		lpLayout->rect.top += size.cy;
	else if (Status)
	{
		rect.top = rect.bottom - size.cy;
		lpLayout->rect.bottom -= size.cy;
	}
	rect.right = rect.left + size.cx;
	rect.bottom = rect.top + size.cy;
	// only resize the window if doing layout and not just rect query
	if (lpLayout->hDWP != NULL)
		AfxRepositionWindow(lpLayout, hwnd, &rect);
return 0;
}

/************************************************************************/
BOOL Dialog_OnSetCursor(HWND hDlg, HWND hWndCursor, UINT codeHitTest,
							UINT msg)
/************************************************************************/
{
CPoint pt;
HWND hChild;

// check for disabled control
GetCursorPos( &pt );
if( hChild = MyChildWindowFromPoint( hWndCursor, pt ))
{
    hWndCursor = hChild;
}

return( SetupCursor( hWndCursor, codeHitTest, Dialog_ID ) );
}

/************************************************************************/
void Dialog_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem)
/************************************************************************/
{
MeasureItem( hDlg, lpMeasureItem, NO );
}

/************************************************************************/
void Dialog_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem)
/************************************************************************/
{
DrawItem( hDlg, lpDrawItem, 0 );
}

/************************************************************************/
void Dialog_OnMenuSelect(HWND hDlg, HMENU hmenu, int item, HMENU hmenuPopup, UINT flags)
/************************************************************************/
{
HelpContext.OnHints( hDlg, 0 );
HintLine(item);
}

/************************************************************************/
void Dialog_OnClose(HWND hDlg)
/************************************************************************/
{
AstralDlgEnd( hDlg, FALSE );
}

/***********************************************************************/
BOOL Dialog_OnQueryNewPalette(HWND hDlg)
/***********************************************************************/
{
return(HandleQueryNewPalette(hDlg, NO, NO));
}

/***********************************************************************/
void Dialog_OnPaletteChanged(HWND hDlg, HWND hwndPaletteChange)
/***********************************************************************/
{
if (IsOurWindow(hwndPaletteChange))
	FORWARD_WM_PALETTECHANGED(hDlg, hwndPaletteChange, SendMessageToChildren);
}

/***********************************************************************/
HBRUSH Dialog_OnCtlColor(HWND hDlg, HDC hDC, HWND hControl, int wType)
/***********************************************************************/
{
HBRUSH 	hBrush;
DWORD 	dwStyle;
LOGBRUSH brush;
int	nCount;

dwStyle = GetWindowLong(hControl, GWL_STYLE);
if (wType == CTLCOLOR_LISTBOX && (dwStyle & WS_DISABLED))
	SetTextColor( hDC, GetSysColor(COLOR_BTNFACE) );
else	SetTextColor( hDC,GetSysColor (COLOR_WINDOWTEXT) );

switch( wType )
    {
    case CTLCOLOR_EDIT:
    case CTLCOLOR_MSGBOX: // Used in 3.0 edit boxes (not 3.1)
    case CTLCOLOR_LISTBOX:
	hBrush = (HBRUSH)(COLOR_WINDOW + 1);
	break;
    case CTLCOLOR_CUSTOM:
	hBrush = SetClassBackground(hControl, NULL);
	break;
    case CTLCOLOR_BTN:
    case CTLCOLOR_STATIC:
    case CTLCOLOR_DLG:
    case CTLCOLOR_SCROLLBAR:
    default:
 	hBrush = NULL;
	break;
    }

if ( !hBrush ) // Try using the dialog's background
	hBrush = SetClassBackground(hDlg, NULL);
if ( !hBrush ) // if no class background brush, use white
	hBrush = (HBRUSH)(COLOR_BTNFACE + 1);

if ((int)hBrush-1 <= 20/*COLOR_ENDCOLORS*/)
	{
	hBrush = (HBRUSH)((int)hBrush-1);
	SetBkColor (hDC, GetSysColor((int)hBrush));
	switch ((int)hBrush)
		{
		case COLOR_BTNFACE:
			hBrush = DrawTools.GetBtnFaceBrush();
			break;
		case COLOR_WINDOWFRAME:
			hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
			break;
		case COLOR_BTNSHADOW:
			hBrush = (HBRUSH)DrawTools.GetBtnShadowBrush();
			break;
		case COLOR_SCROLLBAR:
		case COLOR_BACKGROUND:
		case COLOR_ACTIVECAPTION:
		case COLOR_INACTIVECAPTION:
		case COLOR_MENU:
		case COLOR_WINDOW:
		case COLOR_MENUTEXT:
		case COLOR_WINDOWTEXT:
		case COLOR_CAPTIONTEXT:
		case COLOR_ACTIVEBORDER:
		case COLOR_INACTIVEBORDER:
		case COLOR_APPWORKSPACE:
		case COLOR_HIGHLIGHT:
		case COLOR_HIGHLIGHTTEXT:
		case COLOR_GRAYTEXT:
		case COLOR_BTNTEXT:
		default:
			hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
			break;
		}
	}
else
	{
	nCount = sizeof(LOGBRUSH);
	if (GetObject(hBrush, nCount, (LPTR)&brush) == nCount)
		SetBkColor(hDC, brush.lbColor);
	else
		SetBkColor (hDC, GetSysColor (COLOR_BTNFACE));
	}
return(hBrush);
}


//************************************************************************
BOOL Dialog_OnCreate(HWND hDlg, CREATESTRUCT FAR* lpCreateStruct)
/***************************************************************************/
{
	SetWindowLong(hDlg, DWL_USER, 0L);
	return(TRUE);
}

/***************************************************************************/
void Dialog_OnDestroy(HWND hDlg)
/***************************************************************************/
{
	LPTR lptr;
	
	lptr = (LPTR)GetWindowLong(hDlg, DWL_USER);
	if (lptr)
		FreeUp(lptr);
}			  

/***************************************************************************/
void Dialog_OnNotify(HWND hDlg, int idFrom, NMHDR FAR *pnmhdr)
/***************************************************************************/
{
	if (pnmhdr->code == UDN_DELTAPOS)
	{
		CSpinFixedCtrl *pSpin = (CSpinFixedCtrl *)CWnd::FromHandlePermanent(pnmhdr->hwndFrom);
		if (pSpin && pSpin->IsKindOf(RUNTIME_CLASS(CSpinFixedCtrl)))
			pSpin->OnNotifyDeltaPos((LPNM_UPDOWN)pnmhdr);
	}
}	

/***************************************************************************/
void Dialog_OnScroll(HWND hDlg, HWND hControl, UINT uCode, int iPos)
/***************************************************************************/
{
	STRING szClass;
	GetClassName(hControl, szClass, sizeof(szClass)-1);
	if (StringsEqual(szClass, TRACKBAR_CLASS))
		SliderOnScroll(hControl, uCode, iPos);
}
    
