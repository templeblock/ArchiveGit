//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

//************************************************************************
BOOL InitSlide( HWND hDlg, ITEMID idItem, int iValue, int iMin, int iMax )
//************************************************************************
{
SendDlgItemMessage( hDlg, idItem, SM_SETRANGE, 0, MAKELONG(iMin,iMax) );
return( SendDlgItemMessage( hDlg, idItem, SM_SETVALUE, iValue, 0L ) );
}


//************************************************************************
BOOL SetSlide( HWND hDlg, ITEMID idItem, int iValue )
//************************************************************************
{
return( SendDlgItemMessage( hDlg, idItem, SM_SETVALUE, iValue, 0L ) );
}


//************************************************************************
int HandleSlide( HWND hDlg, WORD wParam, long lParam, LPINT lpBoolCompleted )
//************************************************************************
{
if ( lpBoolCompleted )
	*lpBoolCompleted == ( HIWORD(lParam) == SN_VALUESELECTED );
return( SendDlgItemMessage( hDlg, (ITEMID)wParam, SM_GETVALUE, 0, 0L ) );
}


