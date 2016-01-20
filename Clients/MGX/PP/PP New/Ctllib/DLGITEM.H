//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// DLGITEM.H - Public functions for working with dialog items

#ifndef _DLGITEM_H_
#define _DLGITEM_H_

#include "mgxframe.h"

typedef enum
	{
	UT_INCHES,
	UT_MM,
	UT_PICAS,
	UT_CM,
	UT_PIXELS
	} UNIT_TYPE, FAR *LPUNIT_TYPE;

void SetUnitInfo( UNIT_TYPE UnitType, int res, int pts );
void GetUnitInfo( LPUNIT_TYPE UnitType, LPINT res, LPINT pts );
VOID AstralSetDlgItemFixed( HWND hDlg, int idDlg, LFIXED Fixed,
                              BOOL bNoConvert );
LFIXED AstralGetDlgItemFixed( HWND hDlg, int idDlg, BOOL FAR *lpTranslated,
                              BOOL bNoConvert );
LPTSTR Unit2String( LFIXED Fixed, LPTSTR lpFixed );
void SetDlgItemIntExt( HWND hDlg, ITEMID id, int iValue, LPTSTR lpString );
void SetDlgItemTextNow(HWND hDlg, int idCtl, LPTSTR lpText);
void SetDlgItemTextMultiple(HWND hDlg, int idFirstCtl, int idLastCtl, LPTSTR lpText);
void SetDlgItemIntNow(HWND hDlg, int idCtl, UINT uValue, BOOL fSigned);

#endif // _DLGITEM_H_

