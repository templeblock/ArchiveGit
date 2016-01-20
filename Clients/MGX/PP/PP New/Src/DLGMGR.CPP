//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "svrview.h"
#include "id.h"

extern HWND PASCAL _AfxGetSafeOwner(CWnd* pParent);

/****************************** Constants **********************************/
#define	MAXDLG 30

/****************************** Types **************************************/
static int idLastModal;
static struct	
{
	HWND  hdlg;
	int	  id;
#ifndef WIN32
	HTASK  hownerTask;
#else
   HANDLE hownerTask;
#endif
}	
MyModelessDlgList[ MAXDLG ];

static struct	
{
	int	  id;
    HWND hOwnerView;
}	
MyModalDlgList[ MAXDLG ];

/************************************************************************/
/************************************************************************/
/************************************************************************/

/************************************************************************/
/* dlgmgr.c    Astral dialog and window management routines		*/
/************************************************************************/

static BOOL bUserLowResourceAlerted = FALSE;

/***********************************************************************/
BOOL IsModalDialogInUse(int idDlgIn, BOOL bAlertUser,HWND hOwnerView)
/***********************************************************************/
{
	int i;

	for ( i=0; i<MAXDLG; i++ )
	{
		if (MyModalDlgList[i].id == idDlgIn)
		{
			if (bAlertUser)
				Message(IDS_DIALOGINUSE);
			return (TRUE);
		}
	}

	for ( i=0; i<MAXDLG; i++ )
	{
		if ( !MyModalDlgList[i].id )
        {
			MyModalDlgList[i].id = idDlgIn;
			MyModalDlgList[i].hOwnerView = hOwnerView;
        }
	}

	return( FALSE );
}

/***********************************************************************/
BOOL OwnActiveModalDlg(HWND hOwnerView)    
/***********************************************************************/
{
    int i;
    for (i=0; i<MAXDLG;i++)
    {
        if (MyModalDlgList[i].hOwnerView == hOwnerView)
        {
            return (TRUE);
        }
    }
    return (FALSE);
}

/***********************************************************************/
int AstralControlRect( HWND hWindow, int idControl, LPRECT lpRect )
/***********************************************************************/
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
int AstralControlPaint( HWND hWindow, int idControl )
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
int AstralControlRepaint( HWND hWindow, int idControl )
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
int ControlEnable( HWND hWindow, int idControl, BOOL bGray )
/***********************************************************************/
{
HWND hControl;
STRING szClassName;
int ret;
DWORD dwStyle;
BOOL bEnabled;

hControl = GetDlgItem( hWindow, idControl );
if (!hControl)
	return(FALSE);
dwStyle = GetWindowLong( hControl, GWL_STYLE );
bEnabled  = !( dwStyle & WS_DISABLED );
if (bEnabled == bGray)
	return(TRUE);

ret = EnableWindow( hControl, bGray );
// The following is necessary because the 'combobox' and 'listbox'
// controls do not do an InvalidateRect when the control is disabled
// and do not draw the control disabled themselves.  Currently, we
// draw a force them to draw disabled by changing the text color.
GetClassName(hControl, szClassName, MAX_STR_LEN);
if (lstrfind(szClassName, "combobox") ||
    lstrfind(szClassName, "listbox"))
	InvalidateRect(hControl, NULL, TRUE);
//InvalidateRect(hControl, NULL, TRUE);
return(ret);
}

/***********************************************************************/
/* The dialog box list management function are defined below		*/
/***********************************************************************/
/***********************************************************************/
HWND AstralDlg( int iModeless, HINSTANCE hInstance, HWND hWndParent,
                  int idDlgIn, DLGPROC lpDlgControl )
/***********************************************************************/
{
return(AstralDlgParam(iModeless, hInstance, hWndParent, idDlgIn,
					lpDlgControl, 0));
}


/***********************************************************************/
HWND AstralDlgParam( int iModeless, HINSTANCE hInstance, HWND hWndParent,
                        int idDlgIn, DLGPROC lpDlgControl, LPARAM lParamInit )
/***********************************************************************/
{
int	iResult;
DLGPROC lpProc;
HWND	hDlg, hInsertAfter;

/***********************************************************************/
/*  Used to bring up all of our dialog boxes whether modal or modeless */
/***********************************************************************/

/* If the dialog box is in our modeless window list, */
/* Expose it if hidden or covered */

if (iModeless != DLG_MODELESS_MULTIPLE)
	{
	if ( hDlg = AstralDlgGet( idDlgIn ) )
		{
		AstralDlgShow( idDlgIn );
		/* Return the window handle */
		return( hDlg );
		}
	}

/* Otherwise we need to create a new one... */
lpProc = (DLGPROC)MakeProcInstance( (FARPROC)lpDlgControl, hInstance );

/* If the caller wants modeless, */
/* create it and enter its handle into the list */
if ( iModeless )
	{
	hDlg = CreateDialogParam( hInstance, MAKEINTRESOURCE(idDlgIn), hWndParent,
		lpProc, lParamInit );
	if (!hDlg)
	{
		if (!bUserLowResourceAlerted)
		{
			Message(IDS_ELOWRESOURCES);
			bUserLowResourceAlerted = TRUE;
		}
		return(hDlg);
	}

	if (iModeless != DLG_MODELESS_MULTIPLE)
		AstralDlgSet( idDlgIn, hDlg );

	if (iModeless == DLG_MODELESS_ONTOP || iModeless == DLG_MODELESS_MULTIPLE)
		hInsertAfter = HWND_TOP;
	else if (iModeless == DLG_MODELESS_TOPMOST)
		hInsertAfter = HWND_TOPMOST;
	else // if (iModeless == DLG_MODELESS_BOTTOM)
		hInsertAfter = HWND_BOTTOM;

	SetWindowPos( hDlg,
		hInsertAfter,
		0, 0, /* Same location */
		0, 0, /* Same size */
		SWP_NOMOVE | SWP_NOSIZE /* Flags */ );
	/* Return the new window handle */
	return( hDlg );
	}

/* If the caller wants modal, */
/* let windows take care of the whole matter right now */
else	{
    CServerView *pView = PictPubApp.GetActiveView();
	if (IsModalDialogInUse(idDlgIn,TRUE,pView ? pView->GetSafeHwnd() : NULL))
		return (NULL);

	idLastModal = idDlgIn;
	EnableOverlappedWindow( hWndParent, NO );
	iResult = DialogBoxParam( hInstance, MAKEINTRESOURCE(idDlgIn), hWndParent,
					lpProc, lParamInit );
	EnableOverlappedWindow( hWndParent, YES );

	if (iResult < 0)
	{
		if (!bUserLowResourceAlerted)
		{
			Message(IDS_ELOWRESOURCES);
			bUserLowResourceAlerted = TRUE;
		}
		iResult = FALSE;
	}

	// if IsModalDialogInUse returns FALSE then the in-use state has 
	// already been set to true -- so remove dialog from in-use state now
	for ( int i=0; i<MAXDLG; i++ )
    {
		if (MyModalDlgList[i].id == idDlgIn)
        {
			MyModalDlgList[i].id = NULL;
			MyModalDlgList[i].hOwnerView = NULL;
        }
    }    
	idLastModal = 0;
	SetFocus( hWndParent );
	FreeProcInstance((FARPROC)lpProc);
	/* Return the user's response to the modal box */
	return( (HWND)iResult );
	}
} /* end AstralDlg */


/***********************************************************************/
int AstralDlgEnd( HWND hDlg, int iResult )
/***********************************************************************/
{
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
		AstralDlgDestroy(hDlg);
		}
	else	{
		ShowWindow( hDlg, SW_HIDE );
		}
	}
	/* If the window is modal, just do an end */
else
	{
	EndDialog(hDlg, iResult);
	}

return( iResult );
} /* end AstralDlgEnd */

/***********************************************************************/
void AstralDlgDestroy(HWND hDlg)
/***********************************************************************/
{
	DLGPROC	lpProc;

	lpProc = (DLGPROC) GetWindowLong( hDlg, DWL_DLGPROC );
	DestroyWindow( hDlg );
	if (lpProc)
		FreeProcInstance( (FARPROC)lpProc );
}

/***********************************************************************/
void AstralDlgEndAll()
/***********************************************************************/
{
int i;
HWND hDlg;

for ( i=0; i<MAXDLG; i++ )
	{
	if ( hDlg = MyModelessDlgList[i].hdlg )
		AstralDlgEnd( hDlg, TRUE );
	}
}


/***********************************************************************/
BOOL AstralDlgMsg( LPMSG lpMsg )
/***********************************************************************/
{
HWND	hDlg;
int	i;

/***********************************************************************/
/*  Used to pass messages to any child windows or dialog boxes	       */
/***********************************************************************/
for ( i=0; i<MAXDLG; i++ )
	{
	if ( hDlg = MyModelessDlgList[i].hdlg )
		if ( IsDialogMessage( hDlg, lpMsg ) )
			return( TRUE );
	}

return( FALSE );
}


/***********************************************************************/
BOOL AstralDlgClear( HWND hDlg )
/***********************************************************************/
{
int	i;

/***********************************************************************/
/*  Used when a dialog box is to be removed from the list	       */
/***********************************************************************/
if ( !hDlg )
	return( FALSE );

for ( i=0; i<MAXDLG; i++ )
	{
	if ( MyModelessDlgList[i].hdlg == hDlg )
		{
		MyModelessDlgList[i].id = 0;
		MyModelessDlgList[i].hdlg = NULL;
		MyModelessDlgList[i].hownerTask = NULL;
		return( TRUE );
		}
	}

return( FALSE );
}


/***********************************************************************/
HWND AstralDlgGet( int idDlg ) 
/***********************************************************************/
{
int	i;

/***********************************************************************/
/*  Used to check to see if a dialog box is already up		       */
/***********************************************************************/

for ( i=0; i<MAXDLG; i++ )
	{
	if ( MyModelessDlgList[i].id == idDlg )
			return( MyModelessDlgList[i].hdlg );
	}

return( NULL );
}

/***********************************************************************/
int AstralDlgGetID( HWND hDlg )
/***********************************************************************/
{
int	i;

if ( !hDlg )
	return( idLastModal );

/***********************************************************************/
/*  Used to check to see if a dialog box is already up		       */
/***********************************************************************/
for ( i=0; i<MAXDLG; i++ )
	{
	if ( MyModelessDlgList[i].hdlg == hDlg )
		return( MyModelessDlgList[i].id );
	}

return( 0 );
}


/***********************************************************************/
BOOL AstralDlgCheck( HWND hDlg )
/***********************************************************************/
{
int	i;

/***********************************************************************/
/*  Used to see whether a dialog box is in the list		       */
/***********************************************************************/
if ( !hDlg )
	return( FALSE );

for ( i=0; i<MAXDLG; i++ )
	{
	if ( MyModelessDlgList[i].hdlg == hDlg )
		return( TRUE );
	}

return( FALSE );
}


/***********************************************************************/
BOOL AstralDlgSet( int idDlg, HWND hDlg, BOOL bModalDlg /* FALSE */ )
/***********************************************************************/
{
int	i;

/***********************************************************************/
/*  Used to put the dialog box ID into the list			       */
/***********************************************************************/
for ( i=0; i<MAXDLG; i++ )
	{
	if ( !MyModelessDlgList[i].id )
		{
		MyModelessDlgList[i].id = idDlg;
		MyModelessDlgList[i].hdlg = hDlg;
#ifndef WIN32
		MyModelessDlgList[i].hownerTask = ::GetCurrentTask();
#else
		MyModelessDlgList[i].hownerTask = ::GetCurrentProcess();
#endif
		return( TRUE );
		}
	}

return( FALSE );
}


/***********************************************************************/
VOID AstralDlgShow( int idDlg )
/***********************************************************************/
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
VOID AstralDlgHide( int idDlg )
/***********************************************************************/
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


