#include <windows.h>
#include <custcntl.h>
#include "rainbow.h"

/* global static variables */
LPFNSTRTOID lpfnVerId;
LPFNIDTOSTR lpfnIdStr;

/* string for property lists */
#define	IDFNLO			"lpfnIdFnLo"
#define	IDFNHI			"lpfnIdFnHi"

/* rainbow style dialog definitions */
#define	IDTEXT 			0x0100
#define	IDVALUE			0x0101

/* rainbow message definitions */
#define	RM_SETSEL		(WM_USER+1)
#define RM_GETSEL		(WM_USER+2)
#define	RM_SETCOLORS		(WM_USER+3)
#define	RM_GETCOLORS		(WM_USER+4)


#define	RAINBOWCLASS		"Rainbow"

/* general rainbow definitions */
#define	ID			GetWindowWord( hWnd, GWW_ID )
#define	PARENT			GetWindowWord( hWnd, GWW_HWNDPARENT )
#define	INSTANCE		GetWindowWord( hWnd, GWW_HINSTANCE )

/* rainbow specific definitions */
#define	RAINBOW_EXTRA		12

#define	RANGE			GetWindowWord( hWnd, 0 )
#define	TABLE			GetWindowWord( hWnd, 2 )
#define	WIDTH			GetWindowWord( hWnd, 4 )
#define	HEIGHT			GetWindowWord( hWnd, 6 )
#define	CHOICE			GetWindowWord( hWnd, 8 )
#define	CAPTURE			GetWindowWord( hWnd, 10 )

#define	SET_RANGE(x)		SetWindowWord( hWnd, 0, x )
#define	SET_TABLE(x)		SetWindowWord( hWnd, 2, x )
#define	SET_WIDTH(x)		SetWindowWord( hWnd, 4, x )
#define	SET_HEIGHT(x)		SetWindowWord( hWnd, 6, x )
#define	SET_CHOICE(x)		SetWindowWord( hWnd, 8, x )
#define	SET_CAPTURE(x)		SetWindowWord( hWnd, 10, x )

/* caret related definitions */
#define	CARET_XPOS		((CHOICE*WIDTH)+3)
#define	CARET_YPOS		(3)
#define	CARET_WIDTH		(WIDTH-6)
#define	CARET_HEIGHT		(HEIGHT-6)

/* selector related definitions */
#define	SELECTOR_XPOS		((CHOICE*WIDTH)+1)
#define	SELECTOR_YPOS		(1)
#define	SELECTOR_WIDTH		(WIDTH-2)
#define	SELECTOR_HEIGHT		(HEIGHT-2)

/* internal rainbow function prototypes */
BOOL FAR PASCAL RainbowDlgFn( HWND, WORD, WORD, LONG );
LONG FAR PASCAL RainbowWndFn( HWND, WORD, WORD, LONG );
void static DrawSelector( HWND, HDC );

/*
 * RainbowWndFn( hWnd, wMsg, wParam, lParam ) : LONG
 *
 *	hWnd	handle to rainbow window
 *	wMsg	message number
 *	wParam	single word parameter
 *	lParam	double word parameter
 *
 * This function is responsible for processing all the messages
 * which relate to the rainbow control window.  Note how the
 * code is written to avoid potential problems when re-entrancy
 * ocurrs - this involves the use of extra bytes associated with
 * the window data structure.
 *
 * The LONG value returned by this function is either a value
 * returned by the internal handling of the message or by the
 * default window procedure.
 *
 */

LONG FAR PASCAL RainbowWndFn( hWnd, wMsg, wParam, lParam )
HWND hWnd;
WORD wMsg;
WORD wParam;
LONG lParam;
{
/* local variables */
LONG		lResult;
HANDLE		hrgbList;	/* handle to rgb list */
LONG FAR *	lprgbEntry;	/* pointer to rgb list */
PAINTSTRUCT	Ps;		/* paint structure */
WORD		wEntry;		/* current color entry */
HANDLE		hBrush;		/* handle to new brush */
HANDLE		hOldBrush;	/* handle to old brush */
HDC		hDC;		/* display context handle */
WORD		wNewChoice;	/* new mouse selection */
RECT		rectClient;

/* initialization */
lResult = TRUE;

/* process message */
switch( wMsg )
    {
    case WM_CREATE : /* create palette window */
	/* allocate space for rgb color list */
	if ( !(hrgbList = GlobalAlloc( GMEM_MOVEABLE, sizeof(LONG)*16L )) )
		{
		DestroyWindow( hWnd );
		break;
		}
	/*
	 * Define initial rgb color & value list - note that
	 * eight default colors are selected with the values
	 * matching each of the colors.
	 */

	lprgbEntry = (LONG FAR *)GlobalLock( hrgbList );
	lprgbEntry[0] = RGB( 0x00, 0x00, 0x00 );
	lprgbEntry[1] = RGB( 0x00, 0x00, 0xFF );
	lprgbEntry[2] = RGB( 0x00, 0xFF, 0x00 );
	lprgbEntry[3] = RGB( 0xFF, 0x00, 0x00 );
	lprgbEntry[4] = RGB( 0x00, 0xFF, 0xFF );
	lprgbEntry[5] = RGB( 0xFF, 0xFF, 0x00 );
	lprgbEntry[6] = RGB( 0xFF, 0x00, 0xFF );
	lprgbEntry[7] = RGB( 0xFF, 0xFF, 0xFF );
	lprgbEntry[8] = RGB( 0x00, 0x00, 0x00 );
	lprgbEntry[9] = RGB( 0x00, 0x00, 0xFF );
	lprgbEntry[10] = RGB( 0x00, 0xFF, 0x00 );
	lprgbEntry[11] = RGB( 0xFF, 0x00, 0x00 );
	lprgbEntry[12] = RGB( 0x00, 0xFF, 0xFF );
	lprgbEntry[13] = RGB( 0xFF, 0xFF, 0x00 );
	lprgbEntry[14] = RGB( 0xFF, 0x00, 0xFF );
	lprgbEntry[15] = RGB( 0xFF, 0xFF, 0xFF );
	GlobalUnlock( hrgbList );

	/* define instance variables */
	SET_RANGE( 8 );
	SET_TABLE( hrgbList );
	SET_WIDTH( ((LPCREATESTRUCT)lParam)->cx / 8 );
	SET_HEIGHT( ((LPCREATESTRUCT)lParam)->cy );
	SET_CHOICE( 0 );
	SET_CAPTURE( FALSE );
	break;

    case WM_GETDLGCODE : /* capture all key strokes */
	lParam = DLGC_WANTARROWS;
	break;

    case WM_SIZE : /* window being resized */
	/* redefine width & height instance variables */
	SET_WIDTH( LOWORD(lParam) / 8 );
	SET_HEIGHT( HIWORD(lParam) );
	break;

    case WM_PAINT : /* paint control window */
	BeginPaint( hWnd, (LPPAINTSTRUCT)&Ps );
	lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
	for ( wEntry=0; wEntry<RANGE; wEntry++ )
`		{ /* iteratively paint each color patch */
		/* create solid brush for patch & select */
		hBrush = CreateSolidBrush( lprgbEntry[wEntry] );
		hOldBrush = SelectObject( Ps.hdc, hBrush );
		/* draw rectangle with brush fill */
		Rectangle( Ps.hdc, wEntry*WIDTH, 0,
			(wEntry*WIDTH)+WIDTH, HEIGHT );
		/* unselect brush and delete */
		SelectObject( Ps.hdc, hOldBrush );
		DeleteObject( hBrush );
		}
	GlobalUnlock( TABLE );
	EndPaint( hWnd, (LPPAINTSTRUCT)&Ps );
	break;

    case WM_KEYDOWN : /* key being pressed */
	hDC = GetDC( hWnd );
	/* unmark current selection */
	DrawSelector( hWnd, hDC );

	/* process virtual key codes */
	switch( wParam )
	    {
	    case VK_HOME : /* home key */
		SET_CHOICE( 0 );
		break;
	    case VK_UP : /* up cursor key */
	    case VK_LEFT : /* left cursor key */
		SET_CHOICE( (CHOICE > 0) ? CHOICE-1 : RANGE-1 );
		break;
	    case VK_DOWN : /* down cursor key */
	    case VK_RIGHT : /* right cursor key */
	    case VK_SPACE : /* space bar - move right */
		SET_CHOICE( (CHOICE < RANGE-1) ? CHOICE+1 : 0 );
		break;
	    case VK_END : /* end key */
		SET_CHOICE( RANGE-1 );
		break;
	    default : /* some other key */
		lResult = FALSE;
		break;
	    }

	/* mark new selection & release display context */
	DrawSelector( hWnd, hDC );
	ReleaseDC( hWnd, hDC );

	/* move caret to new position */
	SetCaretPos( CARET_XPOS, CARET_YPOS );

	/* notify parent of new selection */
	lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
		SendMessage(PARENT,WM_COMMAND,ID,lprgbEntry[RANGE+CHOICE]);
	GlobalUnlock( TABLE );
	break;

    case WM_SETFOCUS : /* get focus - display caret */
	/* create caret & display */
	CreateCaret( hWnd, NULL, CARET_WIDTH, CARET_HEIGHT );
	SetCaretPos( CARET_XPOS, CARET_YPOS );
	ShowCaret( hWnd );
	break;

    case WM_LBUTTONDOWN : /* left button depressed - fall through */
	hDC = GetDC ( hWnd );
	/* unmark old selection & mark new one */
	DrawSelector( hWnd, hDC );
	SET_CHOICE( LOWORD(lParam)/WIDTH );
	DrawSelector( hWnd, hDC );

	/* release display context & move caret */
	ReleaseDC( hWnd, hDC );

	/* capture focus & move caret */
	SetFocus(hWnd);
	SetCaretPos( CARET_XPOS, CARET_YPOS );

	/* notify parent of new selection */
	lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
		SendMessage(PARENT,WM_COMMAND,ID,lprgbEntry[RANGE+CHOICE]);
	GlobalUnlock( TABLE );

	/* activate capture */
	SetCapture( hWnd );
	SET_CAPTURE( TRUE );
	break;

    case WM_MOUSEMOVE : /* mouse being moved */
	/* track mouse only if capture on */
	if ( !CAPTURE )
		break;
	/* calculate new selection */
	wNewChoice = ( LOWORD(lParam) <= 0 ) ? 0 :
		( LOWORD(lParam)/WIDTH >= RANGE ) ? RANGE - 1 :
			LOWORD(lParam) / WIDTH;
	/* update display if different */
	if ( wNewChoice == CHOICE )
		break;
	hDC = GetDC ( hWnd );
	/* unmark old selection & mark new one */
	DrawSelector( hWnd, hDC );
	SET_CHOICE( wNewChoice );
	DrawSelector( hWnd, hDC );

	/* release display context & move caret */
	ReleaseDC( hWnd, hDC );
	SetCaretPos( CARET_XPOS, CARET_YPOS );

	/* notify parent of new selection */
	lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
	SendMessage(PARENT,WM_COMMAND,ID,lprgbEntry[RANGE+CHOICE]);
		GlobalUnlock( TABLE );
	break;

    case WM_LBUTTONUP : /* left button released */
	if ( !CAPTURE )
		break;
	SET_CAPTURE( FALSE );
	ReleaseCapture();
	break;

    case WM_KILLFOCUS : /* kill focus - hide caret */
	DestroyCaret();
	break;

    case RM_SETSEL : /* select a color entry */
	/* update selection & redraw rainbow */
	SET_CHOICE( (wParam >= RANGE) ? 0 : wParam );
	InvalidateRect( hWnd, NULL, TRUE );

	/* notify parent of change in color */
	lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
	SendMessage( PARENT, WM_COMMAND, ID, lprgbEntry[RANGE+CHOICE] );
	GlobalUnlock( TABLE );

	/* return new choice */
	lResult = CHOICE;
	break;

    case RM_GETSEL : /* retrieve selected color */
	/* return selected entry */
	lResult = CHOICE;
	/* define selected color */
	lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
	*(LONG FAR *)lParam = lprgbEntry[RANGE+CHOICE];
	GlobalUnlock( TABLE );
	break;

    case RM_SETCOLORS : /* define rainbow color table */
	/* release previous table from memory */
	GlobalFree( TABLE );

	hrgbList = GlobalAlloc(	GMEM_MOVEABLE,	sizeof(LONG)*wParam*2L);
	if ( !hrgbList )
		{
		lResult = -1L;
		break;
		}
	/* define initial rgb colors & values */
	lprgbEntry = (LONG FAR *)GlobalLock( hrgbList );
	for ( wEntry=0; wEntry < wParam; wEntry++ )
		{
		lprgbEntry[wEntry] = ((LONG FAR*)lParam)[wEntry];
		lprgbEntry[wParam+wEntry] =
			((LONG FAR*)lParam)[wParam+wEntry];
		}
	GlobalUnlock( hrgbList );

	/* retrieve current window dimensions */
	GetClientRect( hWnd, &rectClient );

	/* re-define instance variables */
	SET_RANGE( wParam );
	SET_TABLE( hrgbList );
	SET_WIDTH( (rectClient.right-rectClient.left)/wParam );
	SET_HEIGHT( rectClient.bottom-rectClient.top );
	SET_CHOICE( 0 );

	/* update window & notify parent of new selection */
	InvalidateRect( hWnd, NULL, TRUE );
	SendMessage( PARENT, WM_COMMAND, ID, lprgbEntry[RANGE+CHOICE] );

	/* normal return */
	lResult = wParam;
	break;

    case RM_GETCOLORS : /* retrieve rainbow color table */
	/* retrieve number of colors */
	lResult = RANGE;

	/* retrieve rgb color list */
	lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
	for ( wEntry=0; (wEntry < RANGE)&&(wEntry < wParam); wEntry++ )
		{
		((LONG FAR *)lParam)[wEntry] = lprgbEntry[wEntry];
		((LONG FAR *)lParam)[RANGE+wEntry] = lprgbEntry[RANGE+wEntry];
		}
	GlobalUnlock( TABLE );
	break;

    case WM_DESTROY : /* window being destroyed */
	GlobalFree( TABLE );
	break;

    default : /* default window message processing */
	lResult = DefWindowProc( hWnd, wMsg, wParam, lParam );
	break;
    }

/* return final result */
return( lResult );
}


/*
 * RainbowDlgFn( hDlg, wMessage, wParam, lParam ) : BOOL;
 *
 *    hDlg           dialog box handle
 *    wMessage       current window message
 *    wParam         word parameter
 *    lParam         long parameter
 *
 * This function is responsible for processing all the messages that
 * relate to the style dialog box.  This function transfers data 
 * between itself and the RainbowStyle using a global data handle.
 *
 * If the user presses the OK button, this data handle is used to pass
 * back a new style data block.  It is left to the calling routine to
 * delete this new block.
 *
 */

BOOL FAR PASCAL RainbowDlgFn(
HWND hDlg,
WORD wMessage,
WORD wParam,
LONG lParam )
{
BOOL bResult;

	/* initialization */
	bResult = TRUE;

	/* process message */
	switch( wMessage )
		{
	case WM_INITDIALOG :
		{
			HANDLE		hCtlStyle;
			LPCTLSTYLE	lpCtlStyle;
			char			szId[  20 ];	/* temp. string to hold id */

			/* disable Ok button & save dialog data handle */
			EnableWindow( GetDlgItem(hDlg,IDOK), FALSE );

			/* retrieve & display style parameters */
			if ( hCtlStyle ) {

				/* add handle to property list */
				SetProp( hDlg, MAKEINTRESOURCE(1), hCtlStyle );

				/* update dialog box fields */
				lpCtlStyle = (LPCTLSTYLE)GlobalLock( hCtlStyle );
				SetDlgItemText( hDlg, IDTEXT, lpCtlStyle->szTitle );

				/* Kanhom Ng 2/7/89 
				 * Set the id value string correctly.
				 * Save the pointer to the verify id function from dialog editor
				 */
				if ( lpfnIdStr )
				{
					(*lpfnIdStr)(lpCtlStyle->wId, (LPSTR)szId, sizeof( szId ) );
					SetDlgItemText( hDlg, IDVALUE, szId );
				}
				else
				{
					EnableWindow( GetDlgItem( hDlg, IDVALUE ), FALSE );
				}
				lstrcpy( lpCtlStyle->szClass, RAINBOWCLASS );
				SetProp( hDlg, IDFNHI, HIWORD( (DWORD)lpfnVerId ) );
				SetProp( hDlg, IDFNLO, LOWORD( (DWORD)lpfnVerId ) );

				GlobalUnlock( hCtlStyle );

			} else
				EndDialog( hDlg, FALSE );

		}
		break;
	case WM_COMMAND :

		/* process sub-message */
		switch( wParam )
			{
		case IDCANCEL : /* cancel dialog box */
			RemoveProp( hDlg, MAKEINTRESOURCE(1) );
			RemoveProp( hDlg, IDFNLO );
			RemoveProp( hDlg, IDFNHI );
			EndDialog( hDlg, FALSE );
			break;
		case IDOK : /* save dialog changes */
			{
				HANDLE		hCtlStyle;
				LPCTLSTYLE	lpCtlStyle;
				LPFNSTRTOID	  lpfnId;		  /* pointer to the verify id function from dialog editor */
				char			szId[ 20 ];		/* temp. string to hold the id */

				hCtlStyle = GetProp( hDlg, MAKEINTRESOURCE(1) );

				/* update structure contents */
				lpCtlStyle = (LPCTLSTYLE)GlobalLock( hCtlStyle );

				szId[ 0 ] = NULL;
				GetDlgItemText( hDlg, IDVALUE, szId, sizeof(szId) );
				lpfnId = (LPFNSTRTOID)MAKELONG( GetProp( hDlg, IDFNLO ), GetProp( hDlg, IDFNHI ) );
				if ( lpfnId )
				{
					DWORD		dwResult; /* result ofthe verifyId function */

					dwResult = (*lpfnId)( (LPSTR)szId );
					if ( !(BOOL)dwResult )
					{
						/* Wrong id */
						GlobalUnlock( hCtlStyle );
						break;
					}

					lpCtlStyle->wId = HIWORD( dwResult );
				}

				GetDlgItemText( hDlg, IDTEXT, lpCtlStyle->szTitle, sizeof(lpCtlStyle->szTitle) );
				GlobalUnlock( hCtlStyle );

				RemoveProp( hDlg, MAKEINTRESOURCE(1) );
				RemoveProp( hDlg, IDFNLO );
				RemoveProp( hDlg, IDFNHI );

				/* end dialog box */
				EndDialog( hDlg, TRUE );

			}
			break;
		case IDTEXT : /* control text */
		case IDVALUE : /* control id */

			/* enable or disable Ok button */
			if ( HIWORD(lParam) == EN_CHANGE )
				EnableWindow(
					GetDlgItem(hDlg,IDOK),
					(SendMessage(GetDlgItem(hDlg,IDTEXT),WM_GETTEXTLENGTH,0,0L) &&
				 	SendMessage(GetDlgItem(hDlg,IDVALUE),WM_GETTEXTLENGTH,0,0L)) ? TRUE : FALSE
				);

			break;
		default : /* something else */
			bResult = FALSE;
			break;
		}

		break;
	default :
		bResult = FALSE;
		break;
	}

/* return final result */
return( bResult );
}


/*
 * DrawSelector( hWnd, hDC ) : void;
 *
 *		hWnd			window handle
 *		hDC			handle to display context
 *
 *	This function is responsible for drawing the selector
 * which surrounds the active color patch.  This drawing
 * process involves the use of the R2_NOT operator to
 * simplify the drawing & re-drawing process.
 *
 */
 
void static DrawSelector( hWnd, hDC )
HWND hWnd;
HDC hDC;
{
HANDLE hOldPen;		/* old pen */
WORD wOldROP2;		/* old raster op code */
WORD wOldBkMode;	/* old background mode */

/* setup display context */
wOldROP2 = SetROP2( hDC, R2_NOT );
wOldBkMode = SetBkMode( hDC, TRANSPARENT );

hOldPen = SelectObject( hDC, CreatePen(0,1,RGB(0,0,0)) );

/* draw selector rectangle */
Rectangle( hDC, SELECTOR_XPOS, SELECTOR_YPOS, SELECTOR_XPOS+SELECTOR_WIDTH,
	SELECTOR_YPOS+SELECTOR_HEIGHT );

DeleteObject( SelectObject(hDC,hOldPen) );

/* restore display context */
SetBkMode( hDC, wOldBkMode );
SetROP2( hDC, wOldROP2 );
}
