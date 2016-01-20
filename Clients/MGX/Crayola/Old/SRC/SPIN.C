//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "limits.h"

/***********************************************************************/

void InitDlgItemSpin(
	HWND 	hWindow,
	int  	idControl,
	int		iValue,
	BOOL 	bSigned,
	int  	iMin,
	int		iMax)
{
	HWND hEditControl;
	HWND hSpinControl;

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		goto Exit;

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		goto Exit;

	if ( GetDlgCtrlID(hSpinControl) != idControl )
		goto Exit;

	SetWindowWord( hSpinControl, GWW_SPINTYPE, 0 );
	SetWindowWord( hSpinControl, GWW_SPINMIN, iMin );
	SetWindowWord( hSpinControl, GWW_SPINMAX, iMax );
	SetWindowLong( hSpinControl, GWL_SPINSTEP, 0 );
Exit:
	SetDlgItemSpin( hWindow, idControl, iValue, bSigned );
}

/***********************************************************************/

void SetDlgItemSpinLimits(
	HWND hWindow,
	int  idControl,
	int  iMin,
	int  iMax)
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
}

/***********************************************************************/

void SetDlgItemSpinStep(
	HWND hWindow,
	int  idControl,
	long lStep)
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

	SetWindowLong( hSpinControl, GWL_SPINSTEP, lStep );
}

/***********************************************************************/

int SetDlgItemSpin(
	HWND hWindow,
	int  idControl,
	int  iValue,
	BOOL bSigned)
{
	HWND hEditControl;
	HWND hSpinControl;
	int  iMin, iMax;

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		goto Exit;

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		goto Exit;

	if ( GetDlgCtrlID(hSpinControl) != idControl )
		goto Exit;

	if ( GetWindowLong(hEditControl, GWL_STYLE) & WS_DISABLED )
		goto Exit;

	iMin = (int)GetWindowWord( hSpinControl, GWW_SPINMIN );
	iMax = (int)GetWindowWord( hSpinControl, GWW_SPINMAX );

	if (iValue < iMin)
	{
		MessageBeep(0);
		iValue = iMin;
	}
	else
	if (iValue > iMax)
	{
		MessageBeep(0);
		iValue = iMax;
	}

Exit:
	iValue = mbound(iValue, INT_MIN, INT_MAX);

	SetDlgItemInt( hWindow, idControl, iValue, bSigned );

	return(iValue);
}

/***********************************************************************/

int GetDlgItemSpin(
	HWND  hWindow,
	int   idControl,
	LPINT lpTranslated,
	BOOL  bSigned)
{
	HWND hEditControl;
	HWND hSpinControl;
	int iMin, iMax, iValue, iNewValue;
	STRING szString;
	long l;

	iValue = GetDlgItemInt( hWindow, idControl, lpTranslated, bSigned );

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		return( iValue );

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		return( iValue );

	if ( GetDlgCtrlID(hSpinControl) != idControl )
		return( iValue );

	iMin = (int)GetWindowWord( hSpinControl, GWW_SPINMIN );
	iMax = (int)GetWindowWord( hSpinControl, GWW_SPINMAX );

	if ( !iValue && !(*lpTranslated))
	{ // if zero from the control, check to see if it's empty (which is OK)
		GetDlgItemText(hWindow, idControl, szString, MAX_STR_LEN);
		if ( !lstrlen( szString ) )
			return( iMin); // it's empty
	  	if ( bSigned && szString[0] == '-' && szString[1] == '\0' )
			return( iMin); // it's a minus sign
		l = atol(szString);
		iNewValue = mbound(l, iMin, iMax);
	}
	else if (iValue < iMin)
		iNewValue = iMin;
	else if (iValue > iMax)
		iNewValue = iMax;
	else if (!(*lpTranslated))
		iNewValue = iValue; //  != 0 so was partialy translated
	else
	{
		return( iValue );
	}

	MessageBeep(0);
	iNewValue = mbound(iNewValue, INT_MIN, INT_MAX);
	SetDlgItemInt( hWindow, idControl, iNewValue, bSigned );

	return( iNewValue );
}

/***********************************************************************/

void InitDlgItemSpinFixed(
	HWND   hWindow,
	int    idControl,
	LFIXED fValue,
	BOOL   bConvert,
	LFIXED fMin,
	LFIXED fMax)
{
	HWND hEditControl;
	HWND hSpinControl;

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		goto Exit;

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		goto Exit;

	if ( GetDlgCtrlID(hSpinControl) != idControl )
		goto Exit;

	SetWindowWord( hSpinControl, GWW_SPINTYPE, 1 );
	SetWindowLong( hSpinControl, GWL_FSPINMIN, fMin );
	SetWindowLong( hSpinControl, GWL_FSPINMAX, fMax );
	SetWindowLong( hSpinControl, GWL_SPINSTEP, 0 );

Exit:
	SetDlgItemSpinFixed( hWindow, idControl, fValue, bConvert );
}

/***********************************************************************/

void SetDlgItemSpinFixedLimits(
	HWND   hWindow,
	int    idControl,
	LFIXED fMin,
	LFIXED fMax)
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

	SetWindowLong( hSpinControl, GWL_FSPINMIN, fMin );
	SetWindowLong( hSpinControl, GWL_FSPINMAX, fMax );
}

/***********************************************************************/

LFIXED SetDlgItemSpinFixed(
	HWND 	hWindow,
	int 	idControl,
	LFIXED 	fValue,
	BOOL 	bConvert)
{
	HWND hEditControl;
	HWND hSpinControl;
	LFIXED fMin, fMax;

	hSpinControl = NULL;

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		goto Exit;

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		goto Exit;

	if ( GetDlgCtrlID(hSpinControl) != idControl )
		goto Exit;

	if ( GetWindowLong(hEditControl, GWL_STYLE) & WS_DISABLED )
		goto Exit;

	fMin = GetWindowLong( hSpinControl, GWL_FSPINMIN );
	fMax = GetWindowLong( hSpinControl, GWL_FSPINMAX );

	if (fValue < fMin)
	{
		MessageBeep(0);
		fValue = fMin;
	}
	else
	if (fValue > fMax)
	{
		MessageBeep(0);
		fValue = fMax;
	}

	SetWindowLong(hSpinControl, GWL_FSPINVALUE, fValue);

	// Set the edit control (translate into units if bConvert)
	AstralSetDlgItemFixed( hWindow, idControl, fValue, !bConvert);

Exit:
	return(fValue);
}

/***********************************************************************/

LFIXED GetDlgItemSpinFixed(
	HWND  hWindow,
	int   idControl,
	LPINT lpTranslated,
	BOOL  bConvert)
{
	HWND hEditControl;
	HWND hSpinControl;
	LFIXED fMin, fMax, fValue, fNewValue;
	STRING szString;
	static BOOL bGetting = FALSE;

	fValue = 0L;

	if (!bGetting)
	{
		// Get the fixed value (translate into units if bConvert)
		fValue = AstralGetDlgItemFixed( hWindow, idControl, lpTranslated, !bConvert );
	}

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		return( fValue );

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		return( fValue );

	if ( GetDlgCtrlID(hSpinControl) != idControl )
		return( fValue );

	if (bGetting)
	{
		fValue = GetWindowLong( hSpinControl, GWL_FSPINVALUE );
		goto Exit;
	}

	fMin = GetWindowLong( hSpinControl, GWL_FSPINMIN );
	fMax = GetWindowLong( hSpinControl, GWL_FSPINMAX );

	if ( !fValue || !(*lpTranslated))
	{ // if zero from the control, check to see if it's empty (which is OK)
		GetDlgItemText(hWindow, idControl, szString, MAX_STR_LEN);
		if ( !lstrlen( szString ) )
			return( fMin ); // it's empty
		if ( szString[0] == '-' && szString[1] == '\0' )
			return( fMin ); // it's a minus sign

		fNewValue = AsciiFixed( szString );

		if (fNewValue < fMin) 
			fNewValue = fMin;

		if (fNewValue > fMax)
			fNewValue = fMax;
	}

	if (fValue < fMin)
		fNewValue = fMin;
	else if (fValue > fMax)
		fNewValue = fMax;
	else if (!(*lpTranslated))
		fNewValue = fValue; // != 0 so was partially translated
	else
	{
		SetWindowLong(hSpinControl, GWL_FSPINVALUE, fValue);
		return( fValue );
	}

	MessageBeep(0);

	SetWindowLong(hSpinControl, GWL_FSPINVALUE, fNewValue);

	bGetting = TRUE;

	// Set the edit control (translate into units if bConvert)
	AstralSetDlgItemFixed( hWindow, idControl, fNewValue, !bConvert );

	bGetting = FALSE;

Exit:
	return( fNewValue );
}

/***********************************************************************/

long WINPROC EXPORT SpinControl(
	HWND     hWindow,
	unsigned message,
	WPARAM   wParam,
	LPARAM   lParam)
{
	int    y;
	RECT   ClientRect;
	PAINTSTRUCT ps;
	HDC    hDC;
	BOOL   bDown;
	DWORD  dwStyle;
	long   lReturn;
	static BOOL bTrack, bInTopRect, bInBottomRect;

	switch ( message )
	{
		case WM_GETDLGCODE:
			return( DLGC_WANTARROWS );

		case WM_ERASEBKGND:
		break; // handle ERASEBKGND and do nothing; PAINT covers everything

		case WM_PAINT:
			hDC = BeginPaint( hWindow, &ps );
			GetClientRect( hWindow, &ClientRect );

			y = ClientRect.bottom;
			ClientRect.bottom =
				1 + ( ClientRect.top + ClientRect.bottom + 1 ) / 2;

			if ( ClientRect.bottom & 1 )
				ClientRect.bottom++; // make sure its even

			bDown = bInTopRect && bTrack;
			DrawSpin( hDC, &ClientRect, 1, bDown );
			ClientRect.top = ClientRect.bottom;
			ClientRect.bottom = y;
			bDown = bInBottomRect && bTrack;
			DrawSpin( hDC, &ClientRect, 0, bDown );
//			dwStyle = GetWindowLong(GetDlgItem( GetParent(hWindow),
//			GET_WINDOW_ID(hWindow) ), GWL_STYLE);
//			if ( WS_NOTENABLED & dwStyle )
//				GrayWindow( hDC, hWindow, -1L );
			EndPaint( hWindow, &ps );
		break;

   	case WM_LBUTTONDOWN:
			dwStyle = GetWindowLong(GetDlgItem( GetParent(hWindow),
			GET_WINDOW_ID(hWindow) ), GWL_STYLE);
			if ( WS_NOTENABLED & dwStyle )
				break;
			if ( bTrack )
				break;
			SetCapture( hWindow ); bTrack = TRUE;
			SetFocus( GetDlgItem( GetParent(hWindow),
				GET_WINDOW_ID(hWindow) ) );
			GetClientRect( hWindow, &ClientRect );
			ClientRect.bottom =
				( ClientRect.top + ClientRect.bottom + 1 ) / 2;
			bInTopRect = PtInRect( &ClientRect, MAKEPOINT(lParam) );
			bInBottomRect = !bInTopRect;
			InvalidateRect( hWindow, NULL, FALSE );
			UpdateWindow( hWindow );
			SetTimer( hWindow, bTrack, 500, NULL );
		break;

		case WM_LBUTTONUP:
			if ( !bTrack )
				break;
			KillTimer( hWindow, bTrack );
			ReleaseCapture(); bTrack = FALSE;
			InvalidateRect( hWindow, NULL, FALSE );
			UpdateWindow( hWindow );
			HandleSpinValue( hWindow, bInTopRect, bInBottomRect );
		break;

		case WM_TIMER:
			if ( !bTrack )
				break;
			KillTimer( hWindow, bTrack );
			SetTimer( hWindow, bTrack, 100, NULL );
			HandleSpinValue( hWindow, bInTopRect, bInBottomRect );
		break;

		case WM_LBUTTONDBLCLK:
		break;

		case WM_MOUSEMOVE:
			if ( !bTrack )
				break;
			GetClientRect( hWindow, &ClientRect );
			y = ClientRect.bottom;
			ClientRect.bottom = ( ClientRect.top + ClientRect.bottom ) / 2;
			if ( bInTopRect == !PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
			{
				bInTopRect = !bInTopRect;
				InvalidateRect( hWindow, NULL, FALSE );
				UpdateWindow( hWindow );
			}
			ClientRect.top = ClientRect.bottom;
			ClientRect.bottom = y;
			if ( bInBottomRect == !PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
			{
				bInBottomRect = !bInBottomRect;
				InvalidateRect( hWindow, NULL, FALSE );
				UpdateWindow( hWindow );
			}
		break;

		case WM_ENABLE:
		case WM_SETTEXT:
			lReturn = DefWindowProc( hWindow, message, wParam, lParam );
			InvalidateRect( hWindow, NULL, FALSE );
			UpdateWindow(hWindow);
			return(lReturn);
		break;

		case WM_DESTROY:
			if ( !bTrack )
				break;
			ReleaseCapture();
			bTrack = NO;
		break;

		default:
			return( DefWindowProc( hWindow, message, wParam, lParam ) );
	}

	return( TRUE );
}

/***********************************************************************/

static LFIXED BumpFixedSpinnerValue(LFIXED Fixed, BOOL bUp, long step)
{
	double d;
	LFIXED frac;
	long   tenth;

	if (!step)
	{
		switch ( Control.Units )
		{
			case IDC_PREF_UNITMM:
				d = ((double)Fixed * (double) 25.4)/65536.0;
				// Increment/Decrement mm count
				if ( bUp )
					d += 1.0;
				else
					d -= 1.0;
				Fixed = (LFIXED)((d * 65536.0) / 25.4);
			break;

			case IDC_PREF_UNITPICAS:
				d = ((double)Fixed * (double)Control.Points)/65536.0;
				// Increment/Decrement point count
				if ( bUp )
					d += 10.0; // Control.Points is X10
				else
					d -= 10.0; // Control.Points is X10
				Fixed = (LFIXED)((d * 65536.0) / (double)Control.Points);
			break;

			case IDC_PREF_UNITCM:
				d = ((double)Fixed * (double) 2.54)/65536.0;
				// Increment/Decrement mm count
				if ( bUp )
					d += 1.0;
				else
					d -= 1.0;
				Fixed = (LFIXED)((d * 65536.0) / 2.54);
			break;

			case IDC_PREF_UNITPIXELS:
				d = (double)(Fixed * (double)Control.UnitRes)/65536.0;
				// Increment/Decrement pixelCount
				if ( bUp )
					d += 1.0;
				else
					d -= 1.0;
				Fixed = (LFIXED)((d * 65536.0)/ (double)Control.UnitRes);
			break;

			case IDC_PREF_UNITINCHES:
			default:
				frac = (long)Fixed & 0x0000FFFFL;

				// Get rid of fraction
				Fixed = (long)Fixed & 0xFFFF0000L;

				// Get current tenth
	 			tenth = ((frac*10L) + (UNITY/2))/UNITY;

				// Increment/Decrement tenth
				if ( bUp )
					tenth++;
				else
					tenth--;

				if (tenth < 0)
				{
					tenth += 10L;
					Fixed -= UNITY;
				}

				Fixed += ((tenth * UNITY)+9L)/10L;
			break;
		}
	}
	else
	{
		frac = (long)Fixed & 0x0000FFFFL;

		// Get rid of fraction
		Fixed = (long)Fixed & 0xFFFF0000L;

		// Get current tenth
	 	tenth = ((frac*10L) + (UNITY/2))/UNITY;

		// Increment/Decrement tenth
		if ( bUp )
			tenth++;
		else
			tenth--;

		if (tenth < 0)
		{
			tenth += 10L;
			Fixed -= UNITY;
		}

		Fixed += ((tenth * UNITY)+9L)/10L;
	}

	return(Fixed);
}

/***********************************************************************/

void HandleSpinValue(
	HWND 	hWindow,
	BOOL 	bInTopRect,
	BOOL	bInBottomRect)
{
	LFIXED fValue;
	int    iValue;
	BOOL   Bool;
	STRING szString;
	long   l;
	long   step;

	step = GetWindowLong( hWindow, GWL_SPINSTEP );

	if ( GetWindowWord( hWindow, GWW_SPINTYPE ) )
	{ // Its a fixed spinner...
		fValue = GetWindowLong(hWindow, GWL_FSPINVALUE);

		if (bInTopRect) 
			fValue = BumpFixedSpinnerValue(fValue, TRUE,  step);
		else if (bInBottomRect)
			fValue = BumpFixedSpinnerValue(fValue, FALSE, step);

		SetDlgItemSpinFixed(GetParent(hWindow),
			GET_WINDOW_ID(hWindow), fValue, step == 0);
	}
	else
	{
		iValue = GetDlgItemInt( GetParent(hWindow), 
			GET_WINDOW_ID(hWindow), &Bool, YES );

		if ( !iValue && !Bool)
		{ // if zero from the control, check to see if it's empty (which is OK)
			GetDlgItemText( GetParent(hWindow),GET_WINDOW_ID(hWindow),
				szString, MAX_STR_LEN);
			l = atol(szString);
			iValue = mbound(l, INT_MIN, INT_MAX);
		}

		if (step == 0)
		{
			if ( bInTopRect )	 iValue++;
			if ( bInBottomRect ) iValue--;
		}
		else
		{
			if ( bInTopRect )	 iValue += step;
			if ( bInBottomRect ) iValue -= step;
		}

		SetDlgItemSpin( GetParent(hWindow), GET_WINDOW_ID(hWindow),
			iValue, YES );
	}
}

/***********************************************************************/

void DrawSpin(
	HDC    	hDC,
	LPRECT 	lpRect,
	BOOL   	bPlus,
	BOOL	bDown)
{
	int  x1, y1, x2, y2, cx, cy;
	HPEN hOldPen;
	RECT Rect;

	// Draw the box interior
	if ( bDown )
		FillRect( hDC, lpRect, Window.hShadowBrush );
	else
		FillRect( hDC, lpRect, Window.hButtonBrush );

	Rect = *lpRect;
	Rect.right--;
	Rect.bottom--;

	x1 = Rect.left;
	x2 = Rect.right;

	y1 = Rect.top;
	y2 = Rect.bottom;

	// Draw a 3 sided dark box
	hOldPen = (HPEN)SelectObject( hDC, GetStockObject(BLACK_PEN) );

	if ( bPlus )
	{
		MoveToEx( hDC, x1, y2, NULL );
		LineTo( hDC, x1, y1 );
		LineTo( hDC, x2, y1 );
		LineTo( hDC, x2, y2 );
		LineTo( hDC, x2, y2+1 );
		x1++; x2--; y1++;
	}
	else
	{
		MoveToEx( hDC, x1, y1, NULL );
		LineTo( hDC, x1, y2 );
		LineTo( hDC, x2, y2 );
		LineTo( hDC, x2, y1 );
		LineTo( hDC, x2, y1-1 );
		x1++; x2--; y2--;
	}

	x1++; x2--; y1++; y2--;

	if ( bDown )
	{
		x1++; x2++; y1++; y2++;
	}

	cx = ( x1 + x2 ) / 2;
	cy = ( y1 + y2 ) / 2;

	SelectObject( hDC, Window.hShadowPen );

	// draw the horizontal rectangle
	MoveToEx( hDC, x1, cy-1, NULL );
	LineTo( hDC, x2, cy-1 );
	LineTo( hDC, x2, cy+1 );
	LineTo( hDC, x1, cy+1 );
	LineTo( hDC, x1, cy-1 );

	if ( bPlus )
	{
		// draw the vertical rectangle
		MoveToEx( hDC, cx-1, y1, NULL );
		LineTo( hDC, cx-1, y2 );
		LineTo( hDC, cx+1, y2 );
		LineTo( hDC, cx+1, y1 );
		LineTo( hDC, cx-1, y1 );
		SelectObject( hDC, GetStockObject(BLACK_PEN) );

		// draw the black hilights
		MoveToEx( hDC, cx+1, y1, NULL );
		LineTo( hDC, cx+1, cy-1 );
		LineTo( hDC, cx+1, cy );
		MoveToEx( hDC, x2, cy, NULL );
		LineTo( hDC, x2, cy+1 );
		LineTo( hDC, cx+1, cy+1 );
		LineTo( hDC, cx+1, y2 );
		LineTo( hDC, cx-1, y2 );
		LineTo( hDC, cx-1, y2+1 ); 
		MoveToEx( hDC, cx-1, cy+1, NULL );
		LineTo( hDC, x1, cy+1 ); LineTo( hDC, x1, cy );

		// draw the inside cross
		SelectObject( hDC, GetStockObject(WHITE_PEN) );
		MoveToEx( hDC, x1+1, cy, NULL );
		LineTo( hDC, x2-1, cy );
		MoveToEx( hDC, cx, y1+1, NULL );
		LineTo( hDC, cx, y2-1 );
	}
	else
	{
		SelectObject( hDC, GetStockObject(BLACK_PEN) );
		// draw the black hilights
		MoveToEx( hDC, x2, cy, NULL );
		LineTo( hDC, x2, cy+1 );
		LineTo( hDC, x1, cy+1 );
		LineTo( hDC, x1, cy );

		// draw the inside dash
		SelectObject( hDC, GetStockObject(WHITE_PEN) );
		MoveToEx( hDC, x1+1, cy, NULL );
		LineTo( hDC, x2-1, cy );
	}

	SelectObject( hDC, hOldPen );
}

/***********************************************************************/

