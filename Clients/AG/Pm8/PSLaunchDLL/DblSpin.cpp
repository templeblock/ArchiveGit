/* @doc
   @module DblSpin.cpp - the CBDoubleSpinner class |
   
   <c CBDoubleSpinner>, a spin control for double numbers

   <cp>Copyright 1995 Brøderbund Software, Inc., all rights reserved
*/

#include "stdafx.h"
#include "DblSpin.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

inline double LimitPrecision (double value, int nPrecision)
{
	double fScalar = pow (10, nPrecision) ;
	return double(int (value * fScalar /*+ 0.5*/) / fScalar) ;
}

/////////
// @mfunc <c CBDoubleSpinner> constructor, intializes the control to
//        a range of 0.0 to 1.0, with an interval of 0.1, and a
//        precision of 1 digit after the decimal point
//
CBDoubleSpinner::CBDoubleSpinner()
  : m_fFloor(0.0),
    m_fCeiling(1.0),
    m_fInterval(0.1),
    m_nPrecision(1)
{

}

/////////
// @mfunc <c CBDoubleSpinner> function to reinitialize the control.  After the
//        floor, ceiling and interval are set, the position is set to the value
//        of the floor.  Use <mf CBDoubleSpinner::SetPosition> to set it to a 
//        specific value.
//
void CBDoubleSpinner::SetRange(
    double fFloor,      // @parm the lowest value the control can have
    double fCeiling,    // @parm the highest value the control can have
    double fInterval)   // @parm the interval used for incrementing or decrementing
{
	// Set numbers to match precision
	fFloor   = LimitPrecision(fFloor, m_nPrecision) ;
	fCeiling = LimitPrecision(fCeiling, m_nPrecision) ;

    if (fFloor < fCeiling)  // floor must be less than ceiling
    {
        m_fFloor = fFloor;
        m_fCeiling = fCeiling;
    }
    else    // parameter error - no biggie, just swap 'em
    {
        m_fFloor = fCeiling;
        m_fCeiling = fFloor;
    }

    double fSpread = fCeiling - fFloor;

    if ((fSpread / fInterval) < (fSpread / 2))  // interval too small
    {
        fInterval = fSpread / 2;    // correct it
    }

    m_fInterval = fInterval;    // set the interval

    // set integral range of base class control

    SetRange(0, int((fSpread / m_fInterval) /*+ .49*/));

    SetPos(0);
    m_fCurrent = m_fFloor;
}

/////////
// @mfunc <c CBDoubleSpinner> function to set the current position of the control.
//        The actual poition set is rounded to the granularity specified by 
//        the fInterval parameter in <mf CBDoubleSpinner::SetRange>, and clamped
//        to stay within the range of the current floor and ceiling.
//
// @rdesc TRUE if position was set, FALSE if arg was out of range
//
BOOL CBDoubleSpinner::SetPosition(
    double fPos)                    // @parm requested position
{
    double fOldPos = GetPosition();

    int nUpper, nLower;
    GetRange(nLower, nUpper);

    // if (fPos <= m_fFloor)
    if (fPos < (m_fFloor - m_fInterval))
    {
        return FALSE;

        // SetPos(0);
        // m_fCurrent = m_fFloor;
    }
    // else if (fPos >= m_fCeiling)
    else if (fPos > (m_fCeiling + m_fInterval))
    {
        return FALSE;

        // SetPos(nUpper);
        // m_fCurrent = m_fCeiling;
    }
    else    // fPos is in the valid range
    {
        int nPos = int(((fPos - m_fFloor) / m_fInterval) /*+ 0.5*/);
        nPos = max(nLower, nPos);
        nPos = min(nUpper, nPos);

        SetPos(nPos);
        m_fCurrent = fPos;
    }

    // update the buddy edit box
    UpdateBuddy();

    return TRUE;
}

/////////
// @mfunc <c CBDoubleSpinner> function to retrieve the current
//        position of the control.
//
// @rdesc the control's position
//
double CBDoubleSpinner::GetPosition()
{
    Fixup();

    return m_fCurrent;
}

BEGIN_MESSAGE_MAP(CBDoubleSpinner, CSpinButtonCtrl)
	//{{AFX_MSG_MAP(CBDoubleSpinner)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnDeltapos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////
// @mfunc Standard <c CBDoubleSpinner> message map function handles
//        creation and initialization of the control's window.
//
// @rdesc 0 on success, -1 on failure
//
int CBDoubleSpinner::OnCreate(
    LPCREATESTRUCT lpCreateStruct)      // @parm standard argument
{
	if (CSpinButtonCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

    SetRange(m_fFloor, m_fCeiling, m_fInterval);
	
	return 0;
}

/////////
// @mfunc <c CBDoubleSpinner> message map function to set the internal
//        floating point value in sync with changes in the CSpinButtonCtrl's
//        internal integer value.  It returns a 0 in <p pResult>, which 
//        allows the caller to change the integer value.
//
void CBDoubleSpinner::OnDeltapos(
    NMHDR* pNMHDR,      // @parm pointer to the NM_UPDOWN structure
    LRESULT* pResult)   // @parm contains zero on return
{
	NM_UPDOWN* pUpDown = (NM_UPDOWN*)pNMHDR;

    // iterative addition avoids roundoff errors
    int i = pUpDown->iDelta;

    if (i < 0)  // delta is negative
    {
        while (i++)  
        {
            m_fCurrent -= m_fInterval;
        }
    }
    else if (i > 0)   // delta is positive
    {
        while (i--)  
        {
            m_fCurrent += m_fInterval;
        }
    }

    // correct any range errors
    Fixup();

    // update the buddy edit box
    UpdateBuddy();

    // ok to change int value in control
	*pResult = 0;
}

/////////
// @mfunc This private <c CBDoubleSpinner> function is called to correct
//        any range or floating point roundoff errors that might creep into
//        m_fCurrent.  It also rounds the variable to the nearest .001
//        increment, which is desirable in most cases.
//
void CBDoubleSpinner::Fixup(void)
{
    if (m_nPrecision > 0)   // fix roundoff creep problems
    {
        // calculate 10^m_nPrecision
        double fPrecision = pow(10.0, double(m_nPrecision));

        long nCurrent;
    
        if (m_fCurrent < 0.)    // negative
        {
            nCurrent = long((m_fCurrent * fPrecision) - .5);
        }
        else    // positive
        {
            nCurrent = long((m_fCurrent * fPrecision) + .5);
        }

        m_fCurrent = double(nCurrent) / fPrecision;
    }

    // weird roundoff things can happen around the zero point
    if (m_fCurrent < 0.0001 && m_fCurrent > -0.0001)
    {
        m_fCurrent = 0.0;
    }

    // correct any range errors
    m_fCurrent = max(m_fCurrent, m_fFloor);
    m_fCurrent = min(m_fCurrent, m_fCeiling);
}


/////////
// @mfunc Protected <c CBDoubleSpinner> function to set the text
//        in the "buddy" edit box window.
//
void CBDoubleSpinner::UpdateBuddy()
{
    CWnd *pWnd = GetBuddy();

    if (pWnd == NULL || pWnd->GetSafeHwnd() == NULL)
    {
        return;
    }

    if (pWnd->IsKindOf(RUNTIME_CLASS(CBDoubleEdit)))
    {
        ((CBDoubleEdit *)pWnd)->SetPrecision(m_nPrecision);
        ((CBDoubleEdit *)pWnd)->SetValue(m_fCurrent);
    }
    else    // buddy doesn't maintain a double number
    {
        // make a CString from m_fCurrent
        CString str, strFormat ;
		strFormat.Format ("%%.%df", m_nPrecision) ;
		str.Format (strFormat, m_fCurrent) ;

        // Took out the below code and replaced it with the
		// lines above, as the old code had some problems with
		// values below 0.1.  08/19/96 LJW
/*		int nDecimal, nSign;

        // convert float to characters
        char *cp = ::_fcvt(m_fCurrent, m_nPrecision, &nDecimal, &nSign);

        str.Empty();

        if (nSign)
        {
            str += _T("-");
        }

        if (nDecimal > 0)   // if there are digits to the left of the decimal
        {
            // copy the leading digits from the _fcvt() buffer
            while (nDecimal-- > 0)
            {
                str += TCHAR(*cp++);
            }
        }
        else    // we want a leading zero if size is less than 1
        {
            str += _T("0");
        }

        if (m_nPrecision > 0)
        {
            // append decimal point
            str += _T(".");

            // and digits
            for (int n = m_nPrecision; n > 0; --n)
            {
                str += TCHAR(*cp++);
            }
        }
*/
        // set window text to the string
        pWnd->SetWindowText(str);
    }

	if (pWnd == GetFocus() && pWnd->IsKindOf(RUNTIME_CLASS(CEdit)))
		((CEdit *) pWnd)->SetSel(0, -1) ;
}

/////////////////////////////////////////////////////////////////////////////
// CBDoubleEdit

// implement runtime type checking
IMPLEMENT_DYNCREATE(CBDoubleEdit, CEdit)


BEGIN_MESSAGE_MAP(CBDoubleEdit, CEdit)
	//{{AFX_MSG_MAP(CBDoubleEdit)
	ON_WM_KILLFOCUS()
    ON_MESSAGE(WM_SETTEXT, OnSetText)
    ON_MESSAGE(WM_GETTEXT, OnGetText)
    ON_MESSAGE(WM_GETTEXTLENGTH, OnGetTextLength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////
// @mfunc <c CBDoubleEdit> function converts the internal value
//        to a string and does a SetWindowText on it.
//
void CBDoubleEdit::UpdateString() 
{
    // make a CString from m_fValue
    CString str, strFormat ;
	strFormat.Format ("%%.%df", m_nPrecision) ;
	str.Format (strFormat, m_fValue) ;

    // TRACE("%d, %lf : %s\n", m_nPrecision, m_fValue, str );

    // Took out the below code and replaced it with the
	// lines above, as the old code had some problems with
	// values below 0.1.  08/19/96 LJW
/*    int nDecimal, nSign;

    // convert float to characters
    char *cp = ::_fcvt(m_fValue, m_nPrecision, &nDecimal, &nSign);

    str.Empty();

    if (nSign)
    {
        str += _T("-");
    }

    if (nDecimal > 0)   // if there are digits to the left of the decimal
    {
        // copy the leading digits from the _fcvt() buffer
        while (nDecimal-- > 0)
        {
            str += TCHAR(*cp++);
        }
    }
    else    // we want a leading zero if size is less than 1
    {
        str += _T("0");
    }

    if (m_nPrecision > 0)
    {
        // append decimal point
        str += _T(".");

        // and digits
        for (int n = m_nPrecision; n > 0; --n)
        {
            str += TCHAR(*cp++);
        }
    }
*/
    // set window text to the string
    SetWindowText(str);
}

/////////
// @mfunc <c CBDoubleEdit> function called when the edit control loses
//        focus.  If the user has entered a valid floating point number,
//        this function redisplays it at the desired precision.  It 
//        also updates the value in the associated <c CBDoubleSpinner>
//        control.
// <nl>   In the case where the user has entered a non-numeric string,
//        this function will replace the user's string with the valid
//        numeric string generated by <mf CBDoubleEdit::UpdateString>.
//
void CBDoubleEdit::OnKillFocus(
    CWnd* pNewWnd)              // @parm the window receiving focus
{
    // call base class
	CEdit::OnKillFocus(pNewWnd);

    // Get the current string contents
    CString str;
    GetWindowText(str);

	// skip sign character and/or decimal point if present
	int nPos = (str[0] == _T('-'));

	if (nPos < str.GetLength())
		nPos += (str[nPos] == _T('.'));

    // convert string to a double
    // double d = ::atof(str);

	// don't set value unless string starts with a digit
    if ((nPos < str.GetLength() && isdigit(str[nPos])) /*|| str.IsEmpty()*/)
    {
        // convert string to a double
        double d = ::atof(str);

        if (m_pSpinner)
        {
            // set position in spinner
            if (m_pSpinner->SetPosition(d))
            {
                // update value from spinner, 'cause it's smarter!
                SetValue(m_pSpinner->GetPosition());
            }
        }
        else    // no spinner, just set the value
        {
            SetValue(d);
        }
    }
    
	/*
	else
    {
		// new string isn't a number, so
		// put value back in the string
		UpdateString();
    }*/
}

LRESULT CBDoubleEdit::OnSetText (WPARAM wParam, LPARAM lParam)
{
	double value = atof ((LPCTSTR) lParam) ;

    CString str, strFormat ;
	strFormat.Format ("%%.%df", m_nPrecision) ;
	str.Format (strFormat, value) ;

	return DefWindowProc (WM_SETTEXT, wParam, (LPARAM)(LPCTSTR) str) ;
}

LRESULT CBDoubleEdit::OnGetText (WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = DefWindowProc (WM_GETTEXT, wParam, lParam) ;

	if (lResult)
		return lResult ;

	strncpy ((char *) lParam, "0", (int) wParam) ;
	return min (wParam, 2) ;
}

LRESULT CBDoubleEdit::OnGetTextLength (WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = DefWindowProc (WM_GETTEXTLENGTH, wParam, lParam) ;

	//
	// If the text is empty, the WM_GETTEXT message handler, will
	// return a "0".  So we need to fake people into thinking an
	// empty string is really "0".
	// 
	if (!lResult)
		lResult = 1 ;	

	return lResult ;
}