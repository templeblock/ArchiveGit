#include <windows.h>
#include "proto.h"

/***********************************************************************/
BOOL DialogControlRect( HWND hWindow, ITEMID idControl, LPRECT lpRect )
/***********************************************************************/
{
	HWND hControl;

	/* Get the handle to the control */
	if ( !(hControl = GetDlgItem( hWindow, idControl ) ) )
		return( FALSE );

	// Map the coordinates to the parent
	GetClientRect( hControl, lpRect );
	MapWindowPoints( hControl, hWindow, (LPPOINT)lpRect, 2 );
	return( TRUE );
}


/***********************************************************************/
BOOL DialogControlPaint( HWND hWindow, ITEMID idControl )
/***********************************************************************/
{
	HWND hControl;

	/* Get the handle to the control */
	if ( !(hControl = GetDlgItem( hWindow, idControl ) ) )
		return( FALSE );
	InvalidateRect( hControl, NULL, TRUE );
	return( TRUE );
}


/***********************************************************************/
BOOL DialogControlRepaint( HWND hWindow, ITEMID idControl )
/***********************************************************************/
{
	HWND hControl;

	/* Get the handle to the control */
	if ( !(hControl = GetDlgItem( hWindow, idControl ) ) )
		return( FALSE );
	InvalidateRect( hControl, NULL, FALSE );
	return( TRUE );
}


/***********************************************************************/
BOOL ControlEnable( HWND hWindow, ITEMID idControl, BOOL bGray )
/***********************************************************************/
{
	HWND hControl;
	BOOL ret;
	DWORD dwStyle;
	BOOL bEnabled;

	hControl = GetDlgItem( hWindow, idControl );
	if (!hControl)
		return(FALSE);
	dwStyle = GetWindowStyle( hControl );
	bEnabled  = !( dwStyle & WS_DISABLED );
	if (bEnabled == bGray)
		return(TRUE);

	ret = EnableWindow( hControl, bGray );
	// The following is necessary because the 'combobox' and 'listbox'
	// controls do not do an InvalidateRect when the control is disabled
	// and do not draw the control disabled themselves.	 Currently, we
	// draw a force them to draw disabled by changing the text color.
	//GetClassName(hControl, szClassName, MAX_STR_LEN);
	//if (lstrfind(szClassName, "combobox") ||
	//	lstrfind(szClassName, "listbox"))
	//		InvalidateRect(hControl, NULL, TRUE);
	return(ret);
}

/***********************************************************************/
/* The dialog box list management function are defined below		*/
/***********************************************************************/
#define MAXDLG 30

static int idLastModal;
static struct
{
	HWND	hdlg;
	ITEMID	id;
	DLGPROC lpProc;
}	MyDlgList[ MAXDLG ];


/***********************************************************************/
HWND DialogStart( int iModeless, HINSTANCE hInstance, HWND hWndParent, ITEMID idDlgIn, DLGPROC lpDlgControl )
/***********************************************************************/
{
	int iResult;
	DLGPROC lpProc;
	HWND hDlg;
	BOOL bOnBottom;

	if ( bOnBottom = (iModeless < 0) )
		iModeless = -iModeless;
	iModeless &= 1;

	/***********************************************************************/
	/*	Used to bring up all of our dialog boxes whether modal or modeless */
	/***********************************************************************/

	/* If the dialog box is in our modeless window list, */
	/* Expose it if hidden or covered */
	if ( hDlg = DialogGet( idDlgIn ) )
	{
		DialogShow( idDlgIn );
		/* Return the window handle */
		return( hDlg );
	}

	/* Otherwise we need to create a new one... */
	lpProc = (DLGPROC)MakeProcInstance( (FARPROC)lpDlgControl, hInstance );

	/* If the caller wants modeless, */
	/* create it and enter its handle into the list */
	if ( iModeless )
	{
		hDlg = CreateDialogParam( hInstance, MAKEINTRESOURCE(idDlgIn), hWndParent,
		  lpProc, (LPARAM)idDlgIn);

		if (!hDlg)
		{ // This should never happen, so warn the developer
			Print( "Error loading window %d", idDlgIn );
			return( NULL );
		}

		DialogSet( idDlgIn, hDlg, lpProc );
		SetWindowPos( hDlg,
			( bOnBottom ? HWND_BOTTOM : HWND_TOP ),
			0, 0, /* Same location */
			0, 0, /* Same size */
			SWP_NOMOVE | SWP_NOSIZE /* Flags */ );
		/* Return the new window handle */
		return( hDlg );
	}

	/* If the caller wants modal, */
	/* let windows take care of the whole matter right now */
	else
	{
	//	EnableOverlappedWindow( hWndParent, NO );
		EnableWindow(hWndParent, FALSE);
		idLastModal = idDlgIn;

		iResult = DialogBoxParam( hInstance, MAKEINTRESOURCE(idDlgIn), hWndParent,
		  lpProc, (LPARAM)idDlgIn );

		idLastModal = NULL;
		EnableWindow(hWndParent, TRUE);
//		EnableOverlappedWindow( hWndParent, YES );
		SetFocus( hWndParent );
		FreeProcInstance((FARPROC)lpProc);
		/* Return the user's response to the modal box */
		return( (HWND)iResult );
	}
}


/***********************************************************************/
void DialogRemove(HWND hDlg)
/***********************************************************************/
{
	DLGPROC lpProc;

	if (hDlg)
	{
		if ( DialogCheck( hDlg ) )
		{
			lpProc = DialogGetProc( hDlg );
			DialogClear( hDlg );
			if (lpProc)
				FreeProcInstance( (FARPROC)lpProc );
		}
	}
}

/***********************************************************************/
int DialogEnd( HWND hDlg, int iResult )
/***********************************************************************/
{
	DLGPROC lpProc;
	HWND hParent;

	/***********************************************************************/
	/*	Used to close all dialog boxes whether modal or modeless		   */
	/***********************************************************************/

	if ( !hDlg )
		return( iResult );

	hParent = GetParent(hDlg);
	/* If the window is in our modeless list... */
	if ( DialogCheck( hDlg ) )
	{
		/* Then see if the caller wants to */
		/* destroy it (TRUE) or hide it (FALSE) */
		if ( iResult == TRUE )
		{
			/* Remove it from the list */
			lpProc = DialogGetProc( hDlg );
			DialogClear( hDlg );
			DestroyWindow( hDlg );
			if (lpProc)
				FreeProcInstance( (FARPROC)lpProc );
		}
		else
		{
			ShowWindow( hDlg, SW_HIDE );
		}
	}
	/* If the window is modal, just do an end */
	else
	{
		EndDialog(hDlg, iResult);
	}

	SetFocus( hParent );
	return( iResult );
}


/***********************************************************************/
void DialogEndAll( void )
/***********************************************************************/
{
	int i;
	HWND hDlg;

	for ( i=0; i<MAXDLG; i++ )
	{
		if ( hDlg = MyDlgList[i].hdlg )
			DialogEnd( hDlg, TRUE );
	}
}


/***********************************************************************/
BOOL DialogMsg( LPMSG lpMsg )
/***********************************************************************/
{
	HWND hDlg;
	int i;

	/***********************************************************************/
	/*	Used to pass messages to any child windows or dialog boxes		   */
	/***********************************************************************/
	for ( i=0; i<MAXDLG; i++ )
	{
	//	if ( MyDlgList[i].id == IDD_BACKGROUND )
	//		continue;
		if ( hDlg = MyDlgList[i].hdlg )
			if ( IsDialogMessage( hDlg, lpMsg ) )
				return( TRUE );
	}

	return( FALSE );
}


/***********************************************************************/
BOOL DialogClear( HWND hDlg )
/***********************************************************************/
{
	int i;

	/***********************************************************************/
	/*	Used when a dialog box is to be removed from the list		   */
	/***********************************************************************/
	if ( !hDlg )
		return( FALSE );

	for ( i=0; i<MAXDLG; i++ )
	{
		if ( MyDlgList[i].hdlg == hDlg )
		{
			MyDlgList[i].id = NULL;
			MyDlgList[i].hdlg = NULL;
			MyDlgList[i].lpProc = NULL;
			return( TRUE );
		}
	}

	return( FALSE );
}


/***********************************************************************/
HWND DialogGet( ITEMID idDlg )
/***********************************************************************/
{
	int i;

	/***********************************************************************/
	/*	Used to check to see if a dialog box is already up			   */
	/***********************************************************************/
	for ( i=0; i<MAXDLG; i++ )
	{
		if ( MyDlgList[i].id == idDlg )
			return( MyDlgList[i].hdlg );
	}

	return( NULL );
}

/***********************************************************************/
ITEMID DialogGetID( HWND hDlg )
/***********************************************************************/
{
	int i;

	if ( !hDlg )
		return( idLastModal );

	/***********************************************************************/
	/*	Used to check to see if a dialog box is already up			   */
	/***********************************************************************/
	for ( i=0; i<MAXDLG; i++ )
	{
		if ( MyDlgList[i].hdlg == hDlg )
			return( MyDlgList[i].id );
	}

	return( NULL );
}


/***********************************************************************/
DLGPROC DialogGetProc( HWND hDlg )
/***********************************************************************/
{
	int i;

	if ( !hDlg )
		return( NULL );

	for ( i=0; i<MAXDLG; i++ )
	{
		if ( MyDlgList[i].hdlg == hDlg )
			return( MyDlgList[i].lpProc );
	}

	return( NULL );
}


/***********************************************************************/
BOOL DialogCheck( HWND hDlg )
/***********************************************************************/
{
	int i;

	/***********************************************************************/
	/*	Used to see whether a dialog box is in the list			   */
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
BOOL DialogSet( ITEMID idDlg, HWND hDlg, DLGPROC lpProc )
/***********************************************************************/
{
	int i;

	/***********************************************************************/
	/*	Used to put the dialog box ID into the list				   */
	/***********************************************************************/
	for ( i=0; i<MAXDLG; i++ )
	{
		if ( !MyDlgList[i].id )
		{
			MyDlgList[i].id = idDlg;
			MyDlgList[i].hdlg = hDlg;
			MyDlgList[i].lpProc = lpProc;
			return( TRUE );
		}
	}

	return( FALSE );
}


/***********************************************************************/
void DialogShow( ITEMID idDlg )
/***********************************************************************/
{
	HWND hDlg;

	/***********************************************************************/
	/*	Used to show the dialog box ID					   */
	/***********************************************************************/

	/* If the dialog box is in our modeless window list, */
	/* Expose it if hidden or covered */
	if ( hDlg = DialogGet( idDlg ) )
	{
		SetWindowPos( hDlg, 0, /* Place on top */
			0, 0, /* Same location */
			0, 0, /* Same size */
			SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE /* Flags */ );
	}
}


/***********************************************************************/
void DialogHide( ITEMID idDlg )
/***********************************************************************/
{
	HWND hDlg;

	/***********************************************************************/
	/*	Used to hide the dialog box ID					   */
	/***********************************************************************/

	/* If the dialog box is in our modeless window list, hide it from view */
	if ( hDlg = DialogGet( idDlg ) )
	{
		ShowWindow( hDlg, SW_HIDE );
		SetFocus( GetParent( hDlg ) );
	}
}
