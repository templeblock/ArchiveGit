//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef NO_MFC
#include <limits.h>
#include <stdlib.h>
#include "ctlutil.h"
#include "spin.h"
#include "macros.h"
#include "dlgitem.h"

IMPLEMENT_DYNAMIC(CSpinFixedCtrl, CSpinButtonCtrl)

CSpinFixedCtrl *CSpinFixedCtrl::Init(LFIXED fValue, LFIXED fMin, LFIXED fMax, BOOL bConvert)
{
	// all the sillyness below is so that this class can be
	// used just like you would use CSpinButtonCtrl.  Ordinarily
	// in a WM_INITDIALOG you initialize a spin button.  You do
	// this by calling GetDlgItem() and casting the return value
	// to CSpinFixedCtrl().  When this is done a temporary object is
	// created.  This function will create a permanent object and subclass
	// the window with it.  This object automatically goes away
	// when the control receives a WM_NCDESTROY (see CWnd).  This
	// would not be necessary if I did not need to carry data around
	// with the control.
	CSpinFixedCtrl *pSpin = (CSpinFixedCtrl *)CWnd::FromHandlePermanent(GetSafeHwnd());
	if (!pSpin)
	{
		TRY
		{
			pSpin = new CSpinFixedCtrl();
			pSpin->SubclassWindow(GetSafeHwnd());
		}
		CATCH_ALL(e)
		{
 		}
		END_CATCH_ALL
	}
	if (pSpin)
		pSpin->DoInit(fValue, fMin, fMax, bConvert);
	return(pSpin);
}

void CSpinFixedCtrl::DoInit(LFIXED fValue, LFIXED fMin, LFIXED fMax, BOOL bConvert)
{
	m_bConvert = bConvert;
	m_fValue = fValue;
	m_fMin = fMin;
	m_fMax = fMax;
	CSpinButtonCtrl::SetRange(0, 100);
	CSpinButtonCtrl::SetPos(50);
	SetPos( m_fValue );
}

void CSpinFixedCtrl::SetRange(LFIXED fMin, LFIXED fMax)
{
	CSpinFixedCtrl *pSpin = (CSpinFixedCtrl *)CWnd::FromHandlePermanent(GetSafeHwnd());
	if (!pSpin)
		return;

	m_fMin = fMin;
	m_fMax = fMax;
	SetPos( m_fValue );
}
 
LFIXED CSpinFixedCtrl::SetPos(LFIXED fValue)
{
	HWND 	hEditControl;
		 
	CSpinFixedCtrl *pSpin = (CSpinFixedCtrl *)CWnd::FromHandlePermanent(GetSafeHwnd());
	if (!pSpin)
		return(0);

	/* Get the handle to the control */
	if ( !(hEditControl = ::GetWindow(GetSafeHwnd(), GW_HWNDPREV) ) )
		return(fValue);

	if ( GetWindowLong(hEditControl, GWL_STYLE) & WS_DISABLED )
		return(fValue);

	m_fValue = fValue;
	if (m_fValue < m_fMin)
	{
		m_fValue = m_fMin;
	}
	else
	if (m_fValue > m_fMax)
	{
		m_fValue = m_fMax;
	}
	// Set the edit control (translate into units if bConvert)

	AstralSetDlgItemFixed( ::GetParent(GetSafeHwnd()), ::GetDlgCtrlID(hEditControl), m_fValue, !m_bConvert);

	return(m_fValue);
}

LFIXED CSpinFixedCtrl::GetPos()
{
	CSpinFixedCtrl *pSpin = (CSpinFixedCtrl *)CWnd::FromHandlePermanent(GetSafeHwnd());
	if (!pSpin)
		return(0);
	return(m_fValue);
}

LFIXED CSpinFixedCtrl::GetEditValue(LPINT lpTranslated)
{
	WORD wCur, wNext;
	LFIXED fValue;
	STRING szString;
	HWND hEditControl;
	HWND hWindow = ::GetParent(GetSafeHwnd());

	CSpinFixedCtrl *pSpin = (CSpinFixedCtrl *)CWnd::FromHandlePermanent(GetSafeHwnd());
	if (!pSpin)
		return(0);

	/* Get the handle to the control */
	if ( !(hEditControl = ::GetWindow(GetSafeHwnd(), GW_HWNDPREV) ) )
		return(0 );

	m_bGetting = FALSE;

	fValue = 0L;
	if (!m_bGetting)
	{
		// Get the fixed value (translate into units if bConvert)
		fValue = AstralGetDlgItemFixed( hWindow, ::GetDlgCtrlID(hEditControl), lpTranslated, !m_bConvert );
	}

 	if (m_bGetting)
		return(m_fValue);

	// if zero from the control, check to see if it's empty (which is OK)
	if ( !fValue || !(*lpTranslated))
	{ 
		::GetDlgItemText(hWindow, ::GetDlgCtrlID(hEditControl), szString, MAX_STR_LEN);
		if( !MBStrlen( szString ))
			return( m_fMin ); // it's empty

		if( IsDBCSLeadByte(( BYTE )*szString ))
 			wCur = *(( LPWORD )szString );
		else
 			wCur = *szString;

		if( IsDBCSLeadByte(( BYTE )*MyCharNextN( szString, 1 )))
 			wNext = *(( LPWORD )MyCharNextN( szString, 1 ));
		else
 			wNext = *MyCharNextN( szString, 1 );
			
		if ( wCur == '-' && wNext == '\0' )
			return( m_fMin ); // it's a minus sign

		fValue = AsciiFixed( szString );
	}

	if (fValue < m_fMin)
		m_fValue = m_fMin;
	else 
	if (fValue > m_fMax)
		m_fValue = m_fMax;
	else 
	if (!(*lpTranslated))
		m_fValue = fValue; // != 0 so was partially translated
	else
	{
		m_fValue = fValue;
		return( m_fValue );
	}
	m_bGetting = TRUE;

	// Set the edit control (translate into units if bConvert)
	AstralSetDlgItemFixed( hWindow, ::GetDlgCtrlID(hEditControl), m_fValue, !m_bConvert );

	m_bGetting = FALSE;

	return( m_fValue );
}

void CSpinFixedCtrl::OnNotifyDeltaPos( LPNM_UPDOWN lpUpDown )
{
	CSpinFixedCtrl *pSpin = (CSpinFixedCtrl *)CWnd::FromHandlePermanent(GetSafeHwnd());
	if (!pSpin)
		return;
	BumpValue(lpUpDown->iDelta);
}

void CSpinFixedCtrl::BumpValue(int iDelta)
{
	if (iDelta) 
		m_fValue = BumpFixedSpinnerValue(m_fValue, iDelta > 0, 0);
	SetPos(m_fValue);
	CSpinButtonCtrl::SetPos(50);
}

LFIXED CSpinFixedCtrl::BumpFixedSpinnerValue(LFIXED Fixed, BOOL bUp, long step)
{
	double d, r;
	int	tenths;
	UNIT_TYPE Units;
	int Points, UnitRes;


	if (!step)
	{
		GetUnitInfo(&Units, &UnitRes, &Points);
		switch ( Units )
		{
			case UT_MM:
				d = DOUBLE(Fixed) * (double) 25.4;
				// Increment/Decrement mm count
				if ( bUp )
					d += 1.0;
				else
					d -= 1.0;
				Fixed = DBL2FIX(d / 25.4);
			break;

			case UT_PICAS:
				d = (DOUBLE(Fixed) * (double)Points);
				// Increment/Decrement point count
				if ( bUp )
					d += 10.0; // Points is X10
 				else
					d -= 10.0; // Points is X10
				Fixed = DBL2FIX(d / (double)Points);
			break;

			case UT_CM:
				d = (DOUBLE(Fixed) * (double) 2.54);
				// Increment/Decrement mm count
				if ( bUp )
					d += 1.0;
				else
					d -= 1.0;
				Fixed = DBL2FIX(d / 2.54);
			break;

			case UT_PIXELS:
				d = (DOUBLE(Fixed) * (double)UnitRes);
				// Increment/Decrement pixelCount
				if ( bUp )
					d += 1.0;
				else
					d -= 1.0;
				Fixed = DBL2FIX(d / (double)UnitRes);
			break;

			case UT_INCHES:
			default:
				if (bUp)
					r = 0.05;
				else
					r = 0.95;
				d = DOUBLE(Fixed);
				if (d < 0)
					tenths = (int)((d / 0.1) - r);
				else
					tenths = (int)((d / 0.1) + r);
				if (bUp)
					d = (tenths * 0.1) + 0.1;
				else
					d = (tenths * 0.1) - 0.1;
				Fixed = DBL2FIX(d);
			break;
		}
	}
	else
	{
		if (bUp)
			r = 0.05;
		else
			r = 0.95;
		d = DOUBLE(Fixed);
		if (d < 0)
			tenths = (int)((d / 0.1) - r);
		else
			tenths = (int)((d / 0.1) + r);
		if (bUp)
			d = (tenths * 0.1) + 0.1;
		else
			d = (tenths * 0.1) - 0.1;
		Fixed = DBL2FIX(d);
	}

	return(Fixed);
}

/***********************************************************************/

IMPLEMENT_DYNAMIC(CSpinIntCtrl, CSpinButtonCtrl)

void CSpinIntCtrl::Init(int iValue, int iMin, int iMax)
{
	// all the sillyness below is so that this class can be
	// used just like you would use CSpinButtonCtrl.  Ordinarily
	// in a WM_INITDIALOG you initialize a spin button.  You do
	// this by calling GetDlgItem() and casting the return value
	// to CSpinFixedCtrl().  When this is done a temporary object is
	// created.  This function will create a permanent object and subclass
	// the window with it.  This object automatically goes away
	// when the control receives a WM_NCDESTROY (see CWnd).  This
	// would not be necessary if I did not need to carry data around
	// with the control.
	SetRange(iMin, iMax);
	SetPos(iValue);
}

int CSpinIntCtrl::GetPos()
{
	int iValue = CSpinButtonCtrl::GetPos();
	iValue = (int)(INT16)LOWORD(iValue);
	return(iValue);
}

int CSpinIntCtrl::SetPos(int iValue)
{
	CSpinButtonCtrl::SetPos(iValue);
	return(GetPos());
}

int CSpinIntCtrl::GetEditValue(LPINT lpTranslated)
{
	int iMin, iMax, iValue, iNewValue;
	HWND hEditControl;
	int idControl;
	HWND hWindow = ::GetParent(GetSafeHwnd());
	GetRange(iMin, iMax);
	BOOL bSigned = iMin < 0;

	/* Get the handle to the control */
	if ( !(hEditControl = ::GetWindow(GetSafeHwnd(), GW_HWNDPREV) ) )
		return(0);
	idControl = ::GetDlgCtrlID(hEditControl);

	iValue = ::GetDlgItemInt( hWindow, idControl, lpTranslated, bSigned);

	iNewValue = GetPos();
	if (iNewValue != iValue) // was value bounded by spinner?
		::SetDlgItemInt( hWindow, idControl, iNewValue, bSigned );
	return(iNewValue);
}

void InitDlgItemSpin( HWND hWindow, int idControl, int iValue, BOOL bSigned,
                        int iMin, int iMax )
{
	HWND hEditControl;
	HWND hSpinControl;
	STRING szClass;

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		return;
	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		return;
	GetClassName(hSpinControl, szClass, sizeof(szClass)-1);
	if (StringsEqual(szClass, UPDOWN_CLASS))
	{
		CSpinIntCtrl *pSpin = (CSpinIntCtrl*)CWnd::FromHandle(hSpinControl);
		if (pSpin)
			pSpin->Init(iValue, iMin, iMax);
	}
}

/***********************************************************************/

void SetDlgItemSpinLimits( HWND hWindow, int idControl, int iMin, int iMax )
{
	HWND hEditControl;
	HWND hSpinControl;
	STRING szClass;

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		return;

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		return;

	GetClassName(hSpinControl, szClass, sizeof(szClass)-1);
	if (StringsEqual(szClass, UPDOWN_CLASS))
	{
		CSpinIntCtrl *pSpin = (CSpinIntCtrl*)CWnd::FromHandle(hSpinControl);
		if (pSpin)
			pSpin->SetRange(iMin, iMax);
	}		 
}

/***********************************************************************/

void SetDlgItemSpinStep( HWND hWindow, int idControl, long lStep )
{
	HWND hEditControl;
	HWND hSpinControl;
	STRING szClass;

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		return;

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		return;

	GetClassName(hSpinControl, szClass, sizeof(szClass)-1);
	if (StringsEqual(szClass, UPDOWN_CLASS))
	{
		CSpinIntCtrl *pSpin = (CSpinIntCtrl*)CWnd::FromHandle(hSpinControl);
		if (pSpin)
		{
			UDACCEL udaccel;

			pSpin->GetAccel(1, &udaccel);
			udaccel.nInc = (int)lStep;
			pSpin->SetAccel(1, &udaccel);
		}
	}		 
}

/***********************************************************************/

int SetDlgItemSpin( HWND hWindow, int idControl, int iValue, BOOL bSigned )
{
	HWND hEditControl;
	HWND hSpinControl;
	STRING szClass;

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		return(0);

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		return(0);

	if ( GetWindowLong(hEditControl, GWL_STYLE) & WS_DISABLED )
		return(0);

	GetClassName(hSpinControl, szClass, sizeof(szClass)-1);
	if (StringsEqual(szClass, UPDOWN_CLASS))
	{
		CSpinIntCtrl *pSpin = (CSpinIntCtrl*)CWnd::FromHandle(hSpinControl);
		if (pSpin)
			return(pSpin->SetPos(iValue));
	}
	return(0);
}

/***********************************************************************/

int GetDlgItemSpin( HWND hWindow, int idControl, LPINT lpTranslated,
                        BOOL bSigned )
{
	HWND hEditControl;
	HWND hSpinControl;
	STRING szClass;

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		return( 0 );

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		return( 0 );

	GetClassName(hSpinControl, szClass, sizeof(szClass)-1);
	if (StringsEqual(szClass, UPDOWN_CLASS))
	{
		CSpinIntCtrl *pSpin = (CSpinIntCtrl*)CWnd::FromHandle(hSpinControl);
		if (pSpin)
			return(pSpin->GetEditValue(lpTranslated));
	}
	return(0);
}

/***********************************************************************/

void InitDlgItemSpinFixed( HWND hWindow, int idControl, LFIXED fValue,
                              BOOL bConvert, LFIXED fMin, LFIXED fMax )
{
	HWND hEditControl;
	HWND hSpinControl;
	STRING szClass;

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		return;

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		return;

	GetClassName(hSpinControl, szClass, sizeof(szClass)-1);
	if (StringsEqual(szClass, UPDOWN_CLASS))
	{
		CSpinFixedCtrl *pSpin = (CSpinFixedCtrl *)CWnd::FromHandle(hSpinControl);
		if (pSpin)
			pSpin = pSpin->Init(fValue, fMin, fMax, bConvert);
	}
}

/***********************************************************************/

void SetDlgItemSpinFixedLimits( HWND hWindow, int idControl, LFIXED fMin,
                                 LFIXED fMax )
{
	HWND hEditControl;
	HWND hSpinControl;
	STRING szClass;

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		return;

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		return;

	GetClassName(hSpinControl, szClass, sizeof(szClass)-1);
	if (StringsEqual(szClass, UPDOWN_CLASS))
	{
		CSpinFixedCtrl *pSpin = (CSpinFixedCtrl *)CWnd::FromHandlePermanent(hSpinControl);
		if (pSpin)
			pSpin->SetRange(fMin, fMax);
		return;
	}
}

/***********************************************************************/

LFIXED SetDlgItemSpinFixed( HWND hWindow, int idControl, LFIXED fValue,
                              BOOL bConvert )
{
	HWND 	hEditControl; 
	HWND 	hSpinControl;
	STRING	szClass;

	hSpinControl = NULL;

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		return(fValue);

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		return(fValue);

	GetClassName(hSpinControl, szClass, sizeof(szClass)-1);
	if (StringsEqual(szClass, UPDOWN_CLASS))
	{
		CSpinFixedCtrl *pSpin = (CSpinFixedCtrl *)CWnd::FromHandlePermanent(hSpinControl);
		if (pSpin)
			return(pSpin->SetPos(fValue));
	}
	return(0);
}

/***********************************************************************/

LFIXED GetDlgItemSpinFixed( HWND hWindow, int idControl, LPINT lpTranslated,
                              BOOL bConvert )
{
	HWND hEditControl;
	HWND hSpinControl;
	STRING szClass;

	/* Get the handle to the control */
	if ( !(hEditControl = GetDlgItem( hWindow, idControl ) ) )
		return( 0 );

	if ( !(hSpinControl = GetWindow(hEditControl, GW_HWNDNEXT) ) )
		return( 0 );

	GetClassName(hSpinControl, szClass, sizeof(szClass)-1);
	if (StringsEqual(szClass, UPDOWN_CLASS))
	{
		CSpinFixedCtrl *pSpin = (CSpinFixedCtrl *)CWnd::FromHandlePermanent(hSpinControl);
		if (pSpin)
			return(pSpin->GetEditValue(lpTranslated));
	}  
	return(AstralGetDlgItemFixed( hWindow, idControl, lpTranslated, !bConvert ));
}

#endif



		   
