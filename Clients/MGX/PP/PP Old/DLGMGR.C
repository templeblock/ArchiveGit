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
/* dlgmgr.c    Astral dialog and window management routines		*/
/************************************************************************/

extern HWND hWndAstral;

/***********************************************************************/
int AstralControlRect( hWindow, idControl, lpRect )
/***********************************************************************/
HWND	 	hWindow;
int		idControl;
LPRECT		lpRect;
{
HWND	 	hControl;

/* Get the handle to the control */
if ( !(hControl = GetDlgItem( hWindow, idControl ) ) )
	return( FALSE );
/* Get screen coordinates of the control */
GetWindowRect( hControl, lpRect );
/* Convert the control's coordinates to be relative to the parent */
ScreenToClient( hWindow, (LPPOINT)&lpRect->left );
ScreenToClient( hWindow, (LPPOINT)&lpRect->right );
return( TRUE );
}


/***********************************************************************/
int AstralControlPaint( hWindow, idControl )
/***********************************************************************/
HWND	 	hWindow;
int		idControl;
{
HWND hControl;

/* Get the handle to the control */
if ( !(hControl = GetDlgItem( hWindow, idControl ) ) )
	return( FALSE );
InvalidateRect( hControl, NULL, TRUE );
return( TRUE );
}


/***********************************************************************/
int AstralControlRepaint( hWindow, idControl )
/***********************************************************************/
HWND hWindow;
int idControl;
{
HWND hControl;

/* Get the handle to the control */
if ( !(hControl = GetDlgItem( hWindow, idControl ) ) )
	return( FALSE );
InvalidateRect( hControl, NULL, FALSE );
return( TRUE );
}


/***********************************************************************/
int ControlEnable( hWindow, idControl, bGray )
/***********************************************************************/
HWND	 	hWindow;
int		idControl;
BOOL		bGray;
{
HWND hControl;
STRING szClassName;
int ret;

hControl = GetDlgItem( hWindow, idControl );
ret = EnableWindow( hControl, bGray );
GetClassName(hControl, szClassName, MAX_STR_LEN);
if (StringsEqual(szClassName, "combobox") ||
    StringsEqual(szClassName, "listbox"))
	InvalidateRect(hControl, NULL, TRUE);
return(ret);
}

/***********************************************************************/
void InitDlgItemSpin( hWindow, idControl, iValue, bSigned, iMin, iMax )
/***********************************************************************/
HWND	 	hWindow;
int		idControl;
int		iValue;
BOOL		bSigned;
int		iMin;
int		iMax;
{
HWND hEditControl;
HWND hSpinControl;

/* Get the handle to the control */
if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
	return;
if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
	return;
if ( GetDlgCtrlID(hSpinControl) != idControl )
	return;
SetWindowWord( hSpinControl, GWW_SPINMIN, iMin );
SetWindowWord( hSpinControl, GWW_SPINMAX, iMax );
SetDlgItemSpin( hWindow, idControl, iValue, bSigned );
}

/***********************************************************************/
void SetDlgItemSpin( hWindow, idControl, iValue, bSigned )
/***********************************************************************/
HWND	 	hWindow;
int		idControl;
int		iValue;
BOOL		bSigned;
{
HWND hEditControl;
HWND hSpinControl;
int iMin, iMax;

/* Get the handle to the control */
if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
	return;
if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
	return;
if ( GetDlgCtrlID(hSpinControl) != idControl )
	return;
iMin = GetWindowWord( hSpinControl, GWW_SPINMIN );
iMax = GetWindowWord( hSpinControl, GWW_SPINMAX );
if (iValue < iMin)
	{
	MessageBeep(0);
	iValue = iMin;
	}
if (iValue > iMax)
	{
	MessageBeep(0);
	iValue = iMax;
	}
SetDlgItemInt( hWindow, idControl, iValue, bSigned );
}

/***********************************************************************/
int GetDlgItemSpin( hWindow, idControl, lpTranslated, bSigned )
/***********************************************************************/
HWND	 	hWindow;
int		idControl;
LPINT		lpTranslated;
BOOL		bSigned;
{
HWND hEditControl;
HWND hSpinControl;
int iMin, iMax, iValue;
STRING szString;
BOOL fDigit;
int i;

/* Get the handle to the control */
if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
	return;
if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
	return;
if ( GetDlgCtrlID(hSpinControl) != idControl )
	return;
iValue = GetDlgItemInt(hWindow, idControl, lpTranslated, YES);
iMin = GetWindowWord( hSpinControl, GWW_SPINMIN );
iMax = GetWindowWord( hSpinControl, GWW_SPINMAX );
if (iValue < iMin)
	{
	iValue = iMin;
	GetDlgItemText(hWindow, idControl, szString, MAX_STR_LEN);
	fDigit = FALSE;
	for (i = 0; i < lstrlen(szString); ++i)
		if (isdigit(szString[i]))
			fDigit = TRUE;
	if (fDigit)
		{
		MessageBeep(0);
		SetDlgItemSpin(hWindow, idControl, iValue, bSigned);
		}
	}
if (iValue > iMax)
	{
	iValue = iMax;
	GetDlgItemText(hWindow, idControl, szString, MAX_STR_LEN);
	fDigit = FALSE;
	for (i = 0; i < lstrlen(szString); ++i)
		if (isdigit(szString[i]))
			fDigit = TRUE;
	if (fDigit)
		{
		MessageBeep(0);
		SetDlgItemSpin(hWindow, idControl, iValue, bSigned);
		}
	}
return(iValue);
}

/***********************************************************************/
/* The dialog box list management function are defined below		*/
/***********************************************************************/
#define	MAXDLG 30

static struct	{
	HWND	hdlg;
	int	id;
	}	MyDlgList[ MAXDLG ];


/***********************************************************************/
int AstralDlg( iModeless, hInstance, hWndParent, idDlgIn, lpDlgControl )
/***********************************************************************/
int	iModeless;
HANDLE	hInstance;	/* app module instance handle */
HWND	hWndParent;	/* window handle of parent window */
int	idDlgIn;	/* open dialog box id */
FARPROC	lpDlgControl;
{
int	iResult;
FARPROC lpProc;
HWND	hDlg;
LPTR	lpDlgResource;

/***********************************************************************/
/*  Used to bring up all of our dialog boxes whether modal or modeless */
/***********************************************************************/

/* If the dialog box is in our modeless window list, */
/* Expose it if hidden or covered */
if ( hDlg = AstralDlgGet( idDlgIn ) )
	{
	AstralDlgShow( idDlgIn );
	/* Return the window handle */
	return( hDlg );
	}

/* Otherwise we need to create a new one... */
lpProc = MakeProcInstance( lpDlgControl, hInstance );

/* If the caller wants modeless, */
/* create it and enter its handle into the list */
if ( iModeless )
	{
	lpDlgResource = MAKEINTRESOURCE(idDlgIn);
	hDlg = CreateDialog(hInstance, lpDlgResource, hWndParent, lpProc );
	AstralDlgSet( idDlgIn, hDlg );
	SetWindowPos( hDlg, 0, /* Place on top */
		0, 0, /* Same location */
		0, 0, /* Same size */
		SWP_NOMOVE | SWP_NOSIZE /* Flags */ );
	/* Return the new window handle */
	return( hDlg );
	}

/* If the caller wants modal, */
/* let windows take care of the whole matter right now */
else	{
	iResult = DialogBox(hInstance, MAKEINTRESOURCE(idDlgIn),
		hWndParent, lpProc );
	SetFocus( hWndParent );
	FreeProcInstance(lpProc);
	/* Return the user's response to the modal box */
	return( iResult );
	}
} /* end AstralDlg */


/***********************************************************************/
int AstralDlgEnd( hDlg, iResult )
/***********************************************************************/
HWND	hDlg;
int	iResult;
{
FARPROC	lpProc;

/***********************************************************************/
/*  Used to close all dialog boxes whether modal or modeless	       */
/***********************************************************************/

if ( !hDlg )
	return( iResult );

/* If the window is in our modeless list... */
if ( AstralDlgCheck( hDlg ) )
	{
	/* Then see if the caller wants to */
	/* destroy it (TRUE) or hide it (FALSE) */
	if ( iResult == TRUE )
		{
		/* Remove it from the list */
		AstralDlgClear( hDlg );
		lpProc = (FARPROC) GetWindowLong( hDlg, GWL_WNDPROC );
		DestroyWindow( hDlg );
		FreeProcInstance( lpProc );
		}
	else	{
		ShowWindow( hDlg, SW_HIDE );
		}
	}

/* If the window is modal, just do an end */
else	{
	EndDialog(hDlg, iResult);
	}

SetFocus( GetParent( hDlg ) );
return( iResult );
} /* end AstralDlgEnd */


/***********************************************************************/
BOOL AstralDlgMsg( lpMsg )
/***********************************************************************/
LPMSG	lpMsg;
{
HWND	hDlg;
int	i;

/***********************************************************************/
/*  Used to pass messages to any child windows or dialog boxes	       */
/***********************************************************************/
for ( i=0; i<MAXDLG; i++ )
	{
	if ( hDlg = MyDlgList[i].hdlg )
		if ( IsDialogMessage( hDlg, lpMsg ) )
			return( TRUE );
	}

return( FALSE );
}


/***********************************************************************/
BOOL AstralDlgClear( hDlg )
/***********************************************************************/
HWND	hDlg;
{
int	i;

/***********************************************************************/
/*  Used when a dialog box is to be removed from the list	       */
/***********************************************************************/
if ( !hDlg )
	return( FALSE );

for ( i=0; i<MAXDLG; i++ )
	{
	if ( MyDlgList[i].hdlg == hDlg )
		{
		MyDlgList[i].id = NULL;
		MyDlgList[i].hdlg = NULL;
		return( TRUE );
		}
	}

return( FALSE );
}


/***********************************************************************/
HWND AstralDlgGet( idDlg )
/***********************************************************************/
int	idDlg;
{
int	i;

/***********************************************************************/
/*  Used to check to see if a dialog box is already up		       */
/***********************************************************************/
for ( i=0; i<MAXDLG; i++ )
	{
	if ( MyDlgList[i].id == idDlg )
		return( MyDlgList[i].hdlg );
	}

return( NULL );
}

/***********************************************************************/
int AstralDlgGetID( hDlg )
/***********************************************************************/
HWND	hDlg;
{
int	i;

/***********************************************************************/
/*  Used to check to see if a dialog box is already up		       */
/***********************************************************************/
for ( i=0; i<MAXDLG; i++ )
	{
	if ( MyDlgList[i].hdlg == hDlg )
		return( MyDlgList[i].id );
	}

return( 0 );
}


/***********************************************************************/
BOOL AstralDlgCheck( hDlg )
/***********************************************************************/
HWND	hDlg;
{
int	i;

/***********************************************************************/
/*  Used to see whether a dialog box is in the list		       */
/***********************************************************************/
if ( !hDlg )
	return( FALSE );

for ( i=0; i<MAXDLG; i++ )
	{
	if ( MyDlgList[i].hdlg == hDlg )
		return( TRUE );
	}

return( FALSE );
}


/***********************************************************************/
BOOL AstralDlgSet( idDlg, hDlg )
/***********************************************************************/
int	idDlg;
HWND	hDlg;
{
int	i;

/***********************************************************************/
/*  Used to put the dialog box ID into the list			       */
/***********************************************************************/
for ( i=0; i<MAXDLG; i++ )
	{
	if ( !MyDlgList[i].id )
		{
		MyDlgList[i].id = idDlg;
		MyDlgList[i].hdlg = hDlg;
		return( TRUE );
		}
	}

return( FALSE );
}


/***********************************************************************/
VOID AstralDlgShow( idDlg )
/***********************************************************************/
int	idDlg;
{
HWND	hDlg;

/***********************************************************************/
/*  Used to show the dialog box ID				       */
/***********************************************************************/

/* If the dialog box is in our modeless window list, */
/* Expose it if hidden or covered */
if ( hDlg = AstralDlgGet( idDlg ) )
	{
	SetWindowPos( hDlg, 0, /* Place on top */
		0, 0, /* Same location */
		0, 0, /* Same size */
		SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE /* Flags */ );
	}
}


/***********************************************************************/
VOID AstralDlgHide( idDlg )
/***********************************************************************/
int	idDlg;
{
HWND	hDlg;

/***********************************************************************/
/*  Used to hide the dialog box ID				       */
/***********************************************************************/

/* If the dialog box is in our modeless window list, hide it from view */
if ( hDlg = AstralDlgGet( idDlg ) )
	{
	ShowWindow( hDlg, SW_HIDE );
	SetFocus( GetParent( hDlg ) );
	}
}


/***********************************************************************/
VOID AstralDlgIcon( idDlg )
/***********************************************************************/
int	idDlg;
{
HWND	hDlg;

/***********************************************************************/
/*  Used to minimize the dialog box ID				       */
/***********************************************************************/

/* If the dialog box is in our modeless window list, minimize it */
if ( hDlg = AstralDlgGet( idDlg ) )
	{
	ShowWindow( hDlg, SW_MINIMIZE );
	SetFocus( GetParent( hDlg ) );
	}
}

static RECT ClientRect;
static int Constrain;
static int Mode;
static POINT Anchor;

#define CONSTRAINX 1
#define CONSTRAINY 2
#define DRAG_MODE 1
#define MOVE_MODE 2

/***********************************************************************/
void NewSelection(hWnd, hDC,lpOldRect, lpNewRect, fFlags )
/***********************************************************************/
HWND hWnd;
HDC hDC;
LPRECT lpOldRect;
LPRECT lpNewRect;
int fFlags;
{
RECT rect, temp;
BOOL swap = FALSE;

if ((lpNewRect->top > lpOldRect->bottom) ||  /* completely below */
    (lpNewRect->bottom < lpOldRect->top) ||  /* completely above */
    (lpNewRect->left > lpOldRect->right) ||  /* completely right */
    (lpNewRect->right < lpOldRect->left) ||   /* completely left */
    ((fFlags & SL_TYPE) != SL_BLOCK))
    {
    InvertSelection(hWnd, hDC, lpOldRect, fFlags);
    InvertSelection(hWnd, hDC, lpNewRect, fFlags);
    return;
    }
if (lpNewRect->top < lpOldRect->top)  /* new rect on top of old */
    {
    temp = *lpNewRect;
    *lpNewRect = *lpOldRect;
    *lpOldRect = temp;
    swap = TRUE;
    }
/* the following code assumes that lpNewRect is below lpOldRect */
rect.top = lpOldRect->top;
rect.bottom = lpNewRect->top;
rect.left = lpOldRect->left;
rect.right = lpOldRect->right;
InvertSelection(hWnd, hDC, &rect, fFlags);
if (lpNewRect->left < lpOldRect->left)  /* new rect on left of old */
    {
    rect.top = lpNewRect->top;
    rect.bottom = lpOldRect->bottom;
    rect.left = lpNewRect->right;
    rect.right = lpOldRect->right;
    InvertSelection(hWnd, hDC, &rect, fFlags);
    rect.top = lpNewRect->top;
    rect.bottom = lpOldRect->bottom;
    rect.left = lpNewRect->left;
    rect.right = lpOldRect->left;
    InvertSelection(hWnd, hDC, &rect, fFlags);
    rect.top = lpOldRect->bottom;
    rect.bottom = lpNewRect->bottom;
    rect.left = lpNewRect->left;
    rect.right = lpNewRect->right;
    InvertSelection(hWnd, hDC, &rect, fFlags);
    }
else if (lpNewRect->right > lpOldRect->right)  /* new rect on right of old */
    {
    rect.top = lpNewRect->top;
    rect.bottom = lpOldRect->bottom;
    rect.left = lpOldRect->left;
    rect.right = lpNewRect->left;
    InvertSelection(hWnd, hDC, &rect, fFlags);
    rect.top = lpNewRect->top;
    rect.bottom = lpOldRect->bottom;
    rect.left = lpOldRect->right;
    rect.right = lpNewRect->right;
    InvertSelection(hWnd, hDC, &rect, fFlags);
    rect.top = lpOldRect->bottom;
    rect.bottom = lpNewRect->bottom;
    rect.left = lpNewRect->left;
    rect.right = lpNewRect->right;
    InvertSelection(hWnd, hDC, &rect, fFlags);
    }
else    /* no change in x-direction */
    {
    rect.top = lpOldRect->bottom;
    rect.bottom = lpNewRect->bottom;
    rect.left = lpNewRect->left;
    rect.right = lpNewRect->right;
    InvertSelection(hWnd, hDC, &rect, fFlags);
    }
if (swap)
    {
    temp = *lpNewRect;
    *lpNewRect = *lpOldRect;
    *lpOldRect = temp;
    }
}

/***********************************************************************/
void StartSelection( hWnd, hDC, ptCurrent, lpSelectRect, fFlags )
/* PURPOSE: Begin selection of region					*/
/***********************************************************************/
HWND hWnd;
HDC hDC;
POINT ptCurrent;
LPRECT lpSelectRect;
int fFlags;
{
int tmp, minx, maxx, miny, maxy;

SetCapture(hWnd);
GetClientRect( hWnd, &ClientRect );
ptCurrent.x = bound( ptCurrent.x, ClientRect.left, ClientRect.right );
ptCurrent.y = bound( ptCurrent.y, ClientRect.top, ClientRect.bottom );

Mode = DRAG_MODE;
Anchor.x = Anchor.y = 0;
if ( fFlags & SL_SPECIAL )
	{ /* Setup a selection rectangle at the current location */
	if ( !IsRectEmpty(lpSelectRect) )
		InvertSelection( hWnd, hDC, lpSelectRect, fFlags );
	Constrain = 0;
	lpSelectRect->left = ptCurrent.x;
	lpSelectRect->top = ptCurrent.y;
	lpSelectRect->right = ptCurrent.x;
	lpSelectRect->bottom = ptCurrent.y;
	return;
	}

/* Modify the existing selection rectangle */
/* First clip it to the window's client area */
IntersectRect( lpSelectRect, lpSelectRect, &ClientRect );

/* Figure out which side or corner to drag */
/* The bottom right point is always the one that moves */
Constrain = CONSTRAINX | CONSTRAINY;
if (ptCurrent.x >= lpSelectRect->left - CLOSENESS &&
    ptCurrent.x <= lpSelectRect->right + CLOSENESS &&
    ptCurrent.y >= lpSelectRect->top - CLOSENESS &&
    ptCurrent.y <= lpSelectRect->bottom + CLOSENESS)
    	{
	if ((fFlags & SL_TYPE) == SL_BOXHANDLES)
		ConstrainHandles(ptCurrent, lpSelectRect);
	else	{
		if ( abs( ptCurrent.y - lpSelectRect->bottom ) <= CLOSENESS )
			Constrain ^= CONSTRAINY;
		else
		if ( abs( ptCurrent.y - lpSelectRect->top ) <= CLOSENESS )
			{
			Constrain ^= CONSTRAINY;
			tmp = lpSelectRect->top;
			lpSelectRect->top = lpSelectRect->bottom;
			lpSelectRect->bottom = tmp;
			}
		if ( abs( ptCurrent.x - lpSelectRect->right ) <= CLOSENESS )
			Constrain ^= CONSTRAINX;
		else
		if ( abs( ptCurrent.x - lpSelectRect->left ) <= CLOSENESS )
			{
			Constrain ^= CONSTRAINX;
			tmp = lpSelectRect->left;
			lpSelectRect->left = lpSelectRect->right;
			lpSelectRect->right = tmp;
			}
		}
	if (Constrain == (CONSTRAINX | CONSTRAINY))
    		{
    		minx = min(lpSelectRect->left, lpSelectRect->right)+CLOSENESS;
    		maxx = max(lpSelectRect->left, lpSelectRect->right)-CLOSENESS;
    		miny = min(lpSelectRect->top, lpSelectRect->bottom)+CLOSENESS;
    		maxy = max(lpSelectRect->top, lpSelectRect->bottom)-CLOSENESS;
    		if (ptCurrent.x >= minx && ptCurrent.x <= maxx &&
        	ptCurrent.y >= miny && ptCurrent.y <= maxy)
        		{
        		Mode = MOVE_MODE;
        		lpSelectRect->left = minx-CLOSENESS;
        		lpSelectRect->right = maxx+CLOSENESS;
        		lpSelectRect->top = miny-CLOSENESS;
        		lpSelectRect->bottom = maxy+CLOSENESS;
        		Anchor.x = ptCurrent.x - lpSelectRect->left;
        		Anchor.y = ptCurrent.y - lpSelectRect->top;
        		}
    		}
	}

StatusOfRectangle( lpSelectRect );
}


/***********************************************************************/
void UpdateSelection( hWnd, hDC, ptCurrent, lpSelectRect, fFlags )
/* PURPOSE: Update selection						*/
/***********************************************************************/
HWND hWnd;
HDC hDC;
POINT ptCurrent;
LPRECT lpSelectRect;
int fFlags;
{
HBRUSH hOldBrush;
short OldROP;
BOOL fNeedDC;
int dx, dy, AnchorX, AnchorY;
RECT newSelectRect;

if ( Mode == MOVE_MODE || RBUTTON )
	{
	if ( !Anchor.x && !Anchor.y )
		{
		if ( lpSelectRect->left > lpSelectRect->right )
			{
			dx = lpSelectRect->left;
			lpSelectRect->left = lpSelectRect->right;
			lpSelectRect->right = dx;
			}
		if ( lpSelectRect->top > lpSelectRect->bottom )
			{
			dy = lpSelectRect->top;
			lpSelectRect->top = lpSelectRect->bottom;
			lpSelectRect->bottom = dy;
			}
		Anchor.x = ptCurrent.x - lpSelectRect->left;
		Anchor.y = ptCurrent.y - lpSelectRect->top;
		}
	AnchorX = Anchor.x + lpSelectRect->left;
	AnchorY = Anchor.y + lpSelectRect->top;
	dx = ptCurrent.x - AnchorX;
	dy = ptCurrent.y - AnchorY;
	if (lpSelectRect->left + dx < ClientRect.left)
        	dx = ClientRect.left - lpSelectRect->left;
	if (lpSelectRect->right + dx > ClientRect.right)
        	dx = ClientRect.right - lpSelectRect->right;
    	if (lpSelectRect->top + dy < ClientRect.top)
        	dy = ClientRect.top - lpSelectRect->top;
    	if (lpSelectRect->bottom + dy > ClientRect.bottom)
        	dy = ClientRect.bottom - lpSelectRect->bottom;
    	newSelectRect.left = lpSelectRect->left + dx;
    	newSelectRect.right = lpSelectRect->right + dx;
    	newSelectRect.top = lpSelectRect->top + dy;
    	newSelectRect.bottom = lpSelectRect->bottom + dy;
    	NewSelection(hWnd, hDC, lpSelectRect, &newSelectRect, fFlags);
    	*lpSelectRect = newSelectRect;
	StatusOfRectangle( lpSelectRect );
	return;
    	}
else	Anchor.x = Anchor.y = 0;

if ( Constrain == (CONSTRAINX | CONSTRAINY) ) /* It hasn't been started yet */
	return( StartSelection( hWnd, hDC, ptCurrent, lpSelectRect, fFlags ) );
if ( Constrain & CONSTRAINX )
	ptCurrent.x = lpSelectRect->right;
if ( Constrain & CONSTRAINY )
	ptCurrent.y = lpSelectRect->bottom;

ptCurrent.x = bound( ptCurrent.x, ClientRect.left, ClientRect.right );
ptCurrent.y = bound( ptCurrent.y, ClientRect.top, ClientRect.bottom );

if ( fNeedDC = (!hDC) )
	hDC = GetDC(hWnd);

switch (fFlags & SL_TYPE)
	{
	case SL_BOX:
		OldROP = SetROP2(hDC, R2_NOT);
		MoveTo( hDC, lpSelectRect->left, lpSelectRect->top );
		LineTo( hDC, lpSelectRect->right, lpSelectRect->top );
		LineTo( hDC, lpSelectRect->right, lpSelectRect->bottom );
		LineTo( hDC, lpSelectRect->left, lpSelectRect->bottom );
		LineTo( hDC, lpSelectRect->left, lpSelectRect->top );
		MoveTo( hDC, lpSelectRect->left, lpSelectRect->top );
		LineTo( hDC, ptCurrent.x, lpSelectRect->top );
		LineTo( hDC, ptCurrent.x, ptCurrent.y );
		LineTo( hDC, lpSelectRect->left, ptCurrent.y );
		LineTo( hDC, lpSelectRect->left, lpSelectRect->top );
		SetROP2(hDC, OldROP);
		break;

	case SL_BOXHANDLES:
		OldROP = SetROP2(hDC, R2_NOT);
		MoveTo( hDC, lpSelectRect->left, lpSelectRect->top );
		LineTo( hDC, lpSelectRect->right, lpSelectRect->top );
		LineTo( hDC, lpSelectRect->right, lpSelectRect->bottom );
		LineTo( hDC, lpSelectRect->left, lpSelectRect->bottom );
		LineTo( hDC, lpSelectRect->left, lpSelectRect->top );
		SetROP2(hDC, OldROP);

		DrawHandles(hDC, lpSelectRect);
		lpSelectRect->right = ptCurrent.x;
		lpSelectRect->bottom = ptCurrent.y;

		OldROP = SetROP2(hDC, R2_NOT);
		MoveTo( hDC, lpSelectRect->left, lpSelectRect->top );
		LineTo( hDC, lpSelectRect->right, lpSelectRect->top );
		LineTo( hDC, lpSelectRect->right, lpSelectRect->bottom );
		LineTo( hDC, lpSelectRect->left, lpSelectRect->bottom );
		LineTo( hDC, lpSelectRect->left, lpSelectRect->top );
		SetROP2(hDC, OldROP);

		DrawHandles(hDC, lpSelectRect);
		break;

	case SL_ELLIPSE:
		OldROP = SetROP2(hDC, R2_NOT);
		hOldBrush = SelectObject( hDC, GetStockObject(NULL_BRUSH) );
		Ellipse( hDC, lpSelectRect->left, lpSelectRect->top,
	      lpSelectRect->right, lpSelectRect->bottom );
		Ellipse( hDC, lpSelectRect->left, lpSelectRect->top,
	      	ptCurrent.x, ptCurrent.y );
		SetROP2(hDC, OldROP);
		SelectObject( hDC, hOldBrush );
		break;

	case SL_BLOCK:
		PatBlt(hDC, lpSelectRect->left, lpSelectRect->bottom,
	    		lpSelectRect->right - lpSelectRect->left,
	    		ptCurrent.y - lpSelectRect->bottom, DSTINVERT);
		PatBlt(hDC, lpSelectRect->right, lpSelectRect->top,
	   		 ptCurrent.x - lpSelectRect->right,
	    		ptCurrent.y - lpSelectRect->top, DSTINVERT);
		break;
	}

lpSelectRect->right = ptCurrent.x;
lpSelectRect->bottom = ptCurrent.y;
StatusOfRectangle( lpSelectRect );
if ( fNeedDC )
	ReleaseDC(hWnd, hDC);
}


/***********************************************************************/
void EndSelection( hWnd, hDC, ptCurrent, lpSelectRect, fFlags )
/* PURPOSE: End selection of region, release capture of mouse movement	*/
/***********************************************************************/
HWND hWnd;
HDC hDC;
POINT ptCurrent;
LPRECT lpSelectRect;
int fFlags;
{
int tmp;

UpdateSelection( hWnd, hDC, ptCurrent, lpSelectRect, fFlags );
ReleaseCapture();
if ( lpSelectRect->right < lpSelectRect->left )
	{
	tmp = lpSelectRect->right;
	lpSelectRect->right = lpSelectRect->left;
	lpSelectRect->left = tmp;
	}
if ( lpSelectRect->bottom < lpSelectRect->top )
	{
	tmp = lpSelectRect->bottom;
	lpSelectRect->bottom = lpSelectRect->top;
	lpSelectRect->top = tmp;
	}
}


/***********************************************************************/
int InvertSelection(hWnd, hDC, lpSelectRect, fFlags)
/* PURPOSE: Inverts the current selection					*/
/***********************************************************************/
HWND hWnd;
HDC hDC;
LPRECT lpSelectRect;
int fFlags;
{
HBRUSH hOldBrush;
short OldROP;
BOOL fNeedDC;

if ( fNeedDC = (!hDC) )
	hDC = GetDC(hWnd);

switch (fFlags & SL_TYPE)
    {
    case SL_BOX:
	OldROP = SetROP2(hDC, R2_NOT);
	MoveTo( hDC, lpSelectRect->left, lpSelectRect->top );
	LineTo( hDC, lpSelectRect->right, lpSelectRect->top );
	LineTo( hDC, lpSelectRect->right, lpSelectRect->bottom );
	LineTo( hDC, lpSelectRect->left, lpSelectRect->bottom );
	LineTo( hDC, lpSelectRect->left, lpSelectRect->top );
	SetROP2(hDC, OldROP);
	break;

    case SL_BOXHANDLES:
	OldROP = SetROP2(hDC, R2_NOT);
	MoveTo( hDC, lpSelectRect->left, lpSelectRect->top );
	LineTo( hDC, lpSelectRect->right, lpSelectRect->top );
	LineTo( hDC, lpSelectRect->right, lpSelectRect->bottom );
	LineTo( hDC, lpSelectRect->left, lpSelectRect->bottom );
	LineTo( hDC, lpSelectRect->left, lpSelectRect->top );
	SetROP2(hDC, OldROP);
        DrawHandles(hDC, lpSelectRect);
	break;

    case SL_ELLIPSE:
	OldROP = SetROP2(hDC, R2_NOT);
	hOldBrush = SelectObject( hDC, GetStockObject(NULL_BRUSH) );
	Ellipse( hDC, lpSelectRect->left, lpSelectRect->top,
		      lpSelectRect->right, lpSelectRect->bottom );
	SetROP2(hDC, OldROP);
	SelectObject( hDC, hOldBrush );
	break;

    case SL_BLOCK:
	PatBlt(hDC, lpSelectRect->left, lpSelectRect->top,
		    lpSelectRect->right - lpSelectRect->left,
		    lpSelectRect->bottom - lpSelectRect->top, DSTINVERT);
	break;
    }

if ( fNeedDC )
	ReleaseDC(hWnd, hDC);
}

/***********************************************************************/
VOID ConstrainHandles(ptCurrent, lpSelectRect)
/***********************************************************************/
POINT ptCurrent;
LPRECT lpSelectRect;
{
int tmp;

if ( abs ( ptCurrent.y - lpSelectRect->bottom ) <= CLOSENESS )
	{
	if ( abs (ptCurrent.x - lpSelectRect->right ) <= CLOSENESS)
		Constrain = 0;
	else
	if ( abs (ptCurrent.x - lpSelectRect->left ) <= CLOSENESS)
		{
		Constrain = 0;
		tmp = lpSelectRect->left;
		lpSelectRect->left = lpSelectRect->right;
		lpSelectRect->right = tmp;
		}
	else
	if ( abs (ptCurrent.x - ( ( lpSelectRect->left + lpSelectRect->right+1)/2) ) <= CLOSENESS)
		Constrain ^= CONSTRAINY;
	}
else
if ( abs ( ptCurrent.y - lpSelectRect->top ) <= CLOSENESS )
	{
	if ( abs (ptCurrent.x - lpSelectRect->right ) <= CLOSENESS)
		Constrain = 0;
	else
	if ( abs (ptCurrent.x - lpSelectRect->left ) <= CLOSENESS)
		{
		Constrain = 0;
		tmp = lpSelectRect->left;
		lpSelectRect->left = lpSelectRect->right;
		lpSelectRect->right = tmp;
		}
	else
	if ( abs (ptCurrent.x - ( ( lpSelectRect->left + lpSelectRect->right+1)/2) ) <= CLOSENESS)
		Constrain ^= CONSTRAINY;
	if (Constrain != (CONSTRAINY | CONSTRAINX))
		{
		tmp = lpSelectRect->top;
		lpSelectRect->top = lpSelectRect->bottom;
		lpSelectRect->bottom = tmp;
		}
	}
else
if ( abs ( ptCurrent.x - lpSelectRect->right ) <= CLOSENESS )
	{
	if ( abs (ptCurrent.y - ( ( lpSelectRect->top + lpSelectRect->bottom+1)/2) ) <= CLOSENESS)
		Constrain ^= CONSTRAINX;
	}
else
if ( abs ( ptCurrent.x - lpSelectRect->left ) <= CLOSENESS )
	{
	if ( abs (ptCurrent.y - ( ( lpSelectRect->top + lpSelectRect->bottom+1)/2) ) <= CLOSENESS)
		{
		Constrain ^= CONSTRAINX;
		tmp = lpSelectRect->left;
		lpSelectRect->left = lpSelectRect->right;
		lpSelectRect->right = tmp;
		}
	}
}

/***********************************************************************/
void Help( idTopic, lParam )
/***********************************************************************/
WORD idTopic;
DWORD lParam;
{
FNAME szFileName;

lstrcpy( szFileName, Control.ProgHome );
lstrcat( szFileName, "PPHLP.HLP" );
WinHelp( hWndAstral, szFileName, idTopic, lParam );
}


typedef struct {
	BYTE system[16];
	unsigned time;
	unsigned date;
	BYTE volume[14];
} PID;

/***********************************************************************/
int IsDemo( Program )
/***********************************************************************/
char	*Program;
{
int	fp, i, CopyProtected;
OFSTRUCT ofstruct;
BYTE	drive[8], buf[50];
BYTE far *bios;
struct find_t vol;
PID	*pid;

#define	DEMO "thoseguys"
#define SZDEMO 9
#define	PROTECT "mtlartsa"
#define SZPROTECT 8
#define SYSCHAR (BYTE far *)0xffff0000

if ( ( fp = OpenFile( Program, &ofstruct, OF_READ ) ) < 0 )
	return( 0 );	/* something is wierd */
if ( _llseek( fp, (long)(-sizeof(buf)), 2 ) < 0 )
	*buf = '\0';	/* goto the bottom bytes */
if ( _lread( fp, buf, sizeof(buf) ) != sizeof(buf) )
	*buf = '\0';	/* something is wierd */
_lclose( fp );

for ( i=0; i<=sizeof(buf)-SZDEMO; i++ )
	if (!strnicmp( &buf[i], DEMO, SZDEMO ) ) /* Is it a crippled demo? */
		return( IDS_CRIPPLED );

CopyProtected = NO;
for ( i=0; i<=sizeof(buf)-SZPROTECT; i++ )
	if (!strnicmp( &buf[i], PROTECT, SZPROTECT ) ) /* Is it protected? */
		{
		CopyProtected = YES;
		break;
		}

if ( !CopyProtected )
	/* Program OK - not protected */
	return( 0 );

/* Are there enough bytes left to complete the BIOS and DRIVE checks? */
i += SZPROTECT;
if ( (sizeof(buf) - i) < sizeof(PID) )
	/* Program crippled - no BIOS or DRIVE stamps */
	return( IDS_COPYPROTECTED );

/* Fix i so that we read the info from the tail of the file */
i = sizeof(buf) - sizeof(PID);
pid = (PID*)&buf[i];

/*Print("time=%d, date=%d, volume=%ls", pid->time, pid->date, pid->volume);
for ( i=0; i<16; i++ ) Print("%d-%x", i, (LPTR)pid->system[i] );*/

/* Check #1: Is the BIOS stamp present, and does it match this machine? */
bios = SYSCHAR;  /* get the pointer to the system characterization list */
for ( i=0; i<16; i++ )
	if ( pid->system[i] != *bios++ )
		return( IDS_COPYPROTECTED );

/* Get the volume info to do a comparison */
drive[0] = Program[0]; /* get the drive */
drive[1] = '\0';	/* ASCIIZ terminator */
strcat(drive,":\\*.*"); /* wildcard search of root dir */
/* get the volume name only */
if ( _dos_findfirst(drive, _A_VOLID, &vol) )
	{ /* Can't get volume info - use defaults */
	strcpy( vol.name, "lartsa" );
	vol.wr_time = 0;
	vol.wr_date = 0;
	}

/* Check #2: Is the DRIVE stamp present, and does it match this drive? */
if ( pid->time != vol.wr_time	||
     pid->date != vol.wr_date	||
     strcmp( pid->volume, vol.name ) )
	return( IDS_COPYPROTECTED );

/* Program OK - protected but kosher */
return( 0 );
}

#define HSIZE 2

/***********************************************************************/
static int DrawHandles( hDC, lpSelectRect )
/***********************************************************************/
HDC hDC;
LPRECT lpSelectRect;
{
RECT hRect, sRect;
HPEN hOldPen;

hOldPen = SelectObject(hDC, Window.hBlackPen);

sRect.top = min(lpSelectRect->top, lpSelectRect->bottom);
sRect.bottom = max(lpSelectRect->top, lpSelectRect->bottom);
sRect.left = min(lpSelectRect->left, lpSelectRect->right);
sRect.right = max(lpSelectRect->left, lpSelectRect->right);

hRect.top = sRect.top - HSIZE;
hRect.bottom = sRect.top + HSIZE + 1;
hRect.left = sRect.left - HSIZE;
hRect.right = sRect.left + HSIZE + 1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = sRect.top - HSIZE;
hRect.bottom = sRect.top + HSIZE + 1;
hRect.left = sRect.right - HSIZE;
hRect.right = sRect.right + HSIZE + 1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = sRect.bottom - HSIZE;
hRect.bottom = sRect.bottom + HSIZE + 1;
hRect.left = sRect.right - HSIZE;
hRect.right = sRect.right + HSIZE + 1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = sRect.bottom - HSIZE;
hRect.bottom = sRect.bottom + HSIZE + 1;
hRect.left = sRect.left - HSIZE;
hRect.right = sRect.left + HSIZE + 1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = ((sRect.top+sRect.bottom+1)/2)-HSIZE;
hRect.bottom = ((sRect.top+sRect.bottom+1)/2)+HSIZE+1;
hRect.left = sRect.left - HSIZE;
hRect.right = sRect.left + HSIZE + 1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = sRect.top - HSIZE;
hRect.bottom = sRect.top + HSIZE + 1;
hRect.left = ((sRect.left+sRect.right+1)/2)-HSIZE;
hRect.right = ((sRect.left+sRect.right+1)/2)+HSIZE+1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = ((sRect.top+sRect.bottom+1)/2)-HSIZE;
hRect.bottom = ((sRect.top+sRect.bottom+1)/2)+HSIZE+1;
hRect.left = sRect.right - HSIZE;
hRect.right = sRect.right + HSIZE + 1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = sRect.bottom - HSIZE;
hRect.bottom = sRect.bottom + HSIZE + 1;
hRect.left = ((sRect.left+sRect.right+1)/2)-HSIZE;
hRect.right = ((sRect.left+sRect.right+1)/2)+HSIZE+1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);

SelectObject(hDC, hOldPen);
}

/***********************************************************************/
static int DrawDottedLines( hDC, lpSelectRect, lpWindowRect )
/***********************************************************************/
HDC hDC;
LPRECT lpSelectRect;
LPRECT lpWindowRect;
{
HPEN hOldPen;
short OldROP;
RECT sRect;

hOldPen = SelectObject(hDC, Window.hDottedPen);
OldROP = SetROP2(hDC, R2_NOTXORPEN);

sRect.top = min(lpSelectRect->top, lpSelectRect->bottom);
sRect.bottom = max(lpSelectRect->top, lpSelectRect->bottom);
sRect.left = min(lpSelectRect->left, lpSelectRect->right);
sRect.right = max(lpSelectRect->left, lpSelectRect->right);

MoveTo(hDC, lpWindowRect->left+1, sRect.top);
LineTo(hDC, sRect.left-HSIZE-1, sRect.top);
MoveTo(hDC, sRect.left, lpWindowRect->top+1);
LineTo(hDC, sRect.left, sRect.top-HSIZE-1);
MoveTo(hDC, sRect.right, lpWindowRect->top+1);
LineTo(hDC, sRect.right, sRect.top-HSIZE-1);
MoveTo(hDC, lpWindowRect->left+1, sRect.bottom);
LineTo(hDC, sRect.left-HSIZE-1, sRect.bottom);
SelectObject(hDC, hOldPen);
SetROP2(hDC, OldROP);
}

