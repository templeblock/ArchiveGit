// StaticSpinCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "StaticSpinCtrl.h"
#include "util.h"

#ifdef LOCALIZE
#include "clocale.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BITBTN_COLOR_OUTER_SHADOW   RGB(0, 0, 0)

extern CPalette* pOurPal;  // PrintMaster Palette

// CStaticSpinButton

CStaticSpinButton::CStaticSpinButton(UINT nBmpID):
	m_nBmpID(nBmpID),
	m_nTimer(1)
{
	m_clFace = GetSysColor(COLOR_BTNFACE);
	m_clText = GetSysColor(COLOR_BTNTEXT);
	m_clHilight = GetSysColor(COLOR_BTNHILIGHT);
	m_clShadow = GetSysColor(COLOR_BTNSHADOW);
}

CStaticSpinButton::~CStaticSpinButton()
{
}


BEGIN_MESSAGE_MAP(CStaticSpinButton, CButton)
	//{{AFX_MSG_MAP(CStaticSpinButton)
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(BM_SETSTATE, OnSetState)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticSpinButton message handlers


LRESULT CStaticSpinButton::OnSetState(WPARAM wParam, LPARAM lParam) 
{
	//do default handling
	Default();

	//Being set to selected state
	if (wParam != 0)
		SetTimer(m_nTimer, 500, NULL);
	else
	{
		KillTimer(m_nTimer);
	}
	return 0L;

}

void CStaticSpinButton::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == m_nTimer)
	{
		//kill the initial timer and reset it to shorter delay
		KillTimer(m_nTimer);
		SetTimer(m_nTimer, 100, NULL);
		if(GetState() & 0x0004)
		{
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());
		}

	}
	else
		CButton::OnTimer(nIDEvent);
}

void CStaticSpinButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	//we want to treat double clicks the same as l button down.  This
	//way if the user is clicking really fast the spinner keeps up.
	SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
}

void CStaticSpinButton::CreateBkgndBmp(CDC* pDC, CBitmap& bmBitmap, BOOL bSelected)
{
	CRect rcButton;
	GetClientRect(rcButton);
	CDC dcButton;
	dcButton.CreateCompatibleDC(pDC);
	CBitmap* pOldBmp = dcButton.SelectObject(&bmBitmap);

	// Initialize the button bitmap to the button face color.
	Util::FillRectangleWithColor(dcButton, rcButton, m_clFace);
	if (bSelected)
	{
		// Button is selected, draw a black border with a shadow border inside.
		CRect rcDraw(rcButton);
		Util::DrawBorderWithColor(dcButton, rcDraw, m_clText, 1);
		rcDraw.InflateRect(-1, -1);
		Util::DrawBorderWithColor(dcButton, rcDraw, m_clShadow, 1);
	}
	else
	{
		CRect rcDraw(rcButton);

		// Draw the raised 3D border:
		//
		//    W-----------WB
		//    |           D|
		//    |           ||
		//    |           ||
		//    WD----------D|
		//    B------------B


		CRect rcSide;

		// W horizontal
		rcSide.SetRect(rcDraw.left, rcDraw.top, rcDraw.right-1, rcDraw.top+1);
		Util::FillRectangleWithColor(dcButton, rcSide, m_clHilight);

		// W vertical
		rcSide.SetRect(rcDraw.left, rcDraw.top+1, rcDraw.left+1, rcDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, rcSide, m_clHilight);

		// B horizontal
		rcSide.SetRect(rcDraw.left, rcDraw.bottom-1, rcDraw.right, rcDraw.bottom);
		Util::FillRectangleWithColor(dcButton, rcSide, BITBTN_COLOR_OUTER_SHADOW);

		// B vertical
		rcSide.SetRect(rcDraw.right-1, rcDraw.top, rcDraw.right, rcDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, rcSide, BITBTN_COLOR_OUTER_SHADOW);

		// D horizontal
		rcSide.SetRect(rcDraw.left+1, rcDraw.bottom-2, rcDraw.right-1, rcDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, rcSide, m_clShadow);

		// D vertical
		rcSide.SetRect(rcDraw.right-2, rcDraw.top+1, rcDraw.right-1, rcDraw.bottom-2);
		Util::FillRectangleWithColor(dcButton, rcSide, m_clShadow);
	}

	if(pOldBmp)
		dcButton.SelectObject(pOldBmp);
}

void CStaticSpinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	BOOL bSelected = lpDrawItemStruct->itemState & ODS_SELECTED;
	CBitmap* pOldBmp = NULL;
	// Create a bitmap which will be used to draw the button image.
	// When the bitmap is complete, it will be drawn onto the button.
	CRect rcButton;
	GetClientRect(&rcButton);
	CBitmap bmButton;
	bmButton.CreateCompatibleBitmap(pDC, rcButton.Width(), rcButton.Height());
	CreateBkgndBmp(pDC, bmButton, bSelected);

	CDC dcButton;
	dcButton.CreateCompatibleDC(pDC);
	pOldBmp = dcButton.SelectObject(&bmButton);

	// Compute the area available for content.
	CRect rcContent(rcButton);
	rcContent.InflateRect(-MinimumMargin, -MinimumMargin);

	CBitmap bmBitmap;
	if(Util::LoadResourceBitmap(bmBitmap, MAKEINTRESOURCE(m_nBmpID), pOurPal))
	{
		BITMAP Bitmap;
		CRect rcBitmap;
		rcBitmap.SetRectEmpty();
		
		if (bmBitmap.GetBitmap(&Bitmap) != 0)
		{
			// Compute the bitmap dimensions
			rcBitmap.SetRect(0, 0, Bitmap.bmWidth, Bitmap.bmWidth);
			rcBitmap.OffsetRect(rcContent.left + (rcContent.Width() - rcBitmap.Width()) / 2, 
				rcContent.top + (rcContent.Height() - rcBitmap.Height()) / 2);
		
			// If the button is selected, offset the bitmap
			if (bSelected)
				rcBitmap.OffsetRect(1, 1);

			Util::TransparentBltBitmap(dcButton, rcBitmap, bmBitmap);
		}
	}
	// Copy the bitmap onto the button.
	pDC->BitBlt(rcButton.left, rcButton.top, rcButton.Width(), rcButton.Height(), &dcButton, 0, 0, SRCCOPY);

	if(pOldBmp)
		dcButton.SelectObject(pOldBmp);
	
}


/////////////////////////////////////////////////////////////////////////////
// CStaticSpinText

CStaticSpinText::CStaticSpinText()
{
}

CStaticSpinText::~CStaticSpinText()
{
}


BEGIN_MESSAGE_MAP(CStaticSpinText, CStatic)
	//{{AFX_MSG_MAP(CStaticSpinText)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticSpinText message handlers
void CStaticSpinText::OnPaint() 
{
	CStatic::OnPaint();
	CDC* pDC = GetDC();

	CRect rcClient; 
	GetClientRect(&rcClient);

	int nOldMode = pDC->SetBkMode(TRANSPARENT);

	pDC->DrawText(m_strWindowText, rcClient, DT_CENTER | DT_VCENTER);

	pDC->SetBkMode(nOldMode);
	ReleaseDC(pDC);
}

void CStaticSpinText::SetText(CString strText)
{
	m_strWindowText = strText;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CWnd* pParent = GetParent()->GetParent()->GetParent();
	if(pParent)
	{
		pParent->ScreenToClient(&rcWindow);
		pParent->InvalidateRect(rcWindow);
	}

	InvalidateRect(NULL);
}




/////////////////////////////////////////////////////////////////////////////
// CStaticSpinCtrl

CStaticSpinCtrl::CStaticSpinCtrl():
	m_nMin(0),
	m_nMax(100),
	m_nPos(0),
	m_strStaticText(""),
	m_cButtonIncrease(IDB_RIGHT_ARROW),
	m_cButtonDecrease(IDB_LEFT_ARROW)
{
}

CStaticSpinCtrl::~CStaticSpinCtrl()
{
}


BEGIN_MESSAGE_MAP(CStaticSpinCtrl, CStatic)
	//{{AFX_MSG_MAP(CStaticSpinCtrl)
	ON_BN_CLICKED(IDC_SPIN_INCREASE, OnIncrease)
	ON_BN_CLICKED(IDC_SPIN_DECREASE, OnDecrease)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticSpinCtrl message handlers
void CStaticSpinCtrl::Init()
{
	CRect rect;
	GetClientRect(&rect);
	int nID = GetDlgCtrlID();
	CRect rcButtonDown(rect);
	rcButtonDown.right = rcButtonDown.left + rcButtonDown.Height();
	m_cButtonDecrease.Create("", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , rcButtonDown, this, IDC_SPIN_DECREASE);
	
	CRect rcButtonUp(rect);
	rcButtonUp.left = rcButtonUp.right - rcButtonUp.Height();
	m_cButtonIncrease.Create("", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rcButtonUp, this, IDC_SPIN_INCREASE);

	CRect rcStatic(rcButtonDown.right, rect.top, rcButtonUp.left, rect.bottom);
	m_cStaticText.Create("", WS_CHILD | WS_VISIBLE, rcStatic, this, IDC_STATIC);
	SetPos(0);
}

void CStaticSpinCtrl::OnIncrease() 
{
	if(!GetParent()->SendMessage(UM_SPIN_CHANGE, GetValue() + 1, GetDlgCtrlID()))
	{
		Increase();
	}
}

void CStaticSpinCtrl::OnDecrease() 
{
	if(!GetParent()->SendMessage(UM_SPIN_CHANGE, GetValue() - 1, GetDlgCtrlID()))
		Decrease();
}

void CStaticSpinCtrl::Increase() 
{
	m_nPos++;
	if(m_nMin + m_nPos > m_nMax)
		m_nPos = 0;

	SetPos(m_nPos);
}

void CStaticSpinCtrl::Decrease() 
{
	m_nPos--;
	if(m_nMin + m_nPos < m_nMin)
		m_nPos = m_nMax - m_nMin;

	SetPos(m_nPos);
}

void CStaticSpinCtrl::SetRange(int nMin, int nMax)
{
	ASSERT(m_nMax > m_nMin);
	if(m_nMin > nMax)
		return;

	m_nMin = nMin; 
	m_nMax = nMax;
	if(m_nMin + m_nPos > nMax)
	{
		m_nPos = m_nMax - m_nMin;
		SetPos(m_nPos);
	}
	else if (m_nMin + m_nPos < nMin)
	{
		m_nPos = 0;
		SetPos(m_nPos);
	}
}

void CStaticSpinCtrl::SetPos(int nPos)
{
	m_nPos = nPos;
	int nValue = m_nMin + nPos;
	char text[50];
	m_strStaticText = itoa(nValue, text, 10);
	m_cStaticText.SetText(m_strStaticText);
}

HBRUSH CStaticSpinCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = NULL; 
	
	if(nCtlColor == CTLCOLOR_STATIC )
	{
		pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		pDC->SetBkMode(TRANSPARENT);
		hbr =  (HBRUSH)GetStockObject(HOLLOW_BRUSH);	
	}
	else if (nCtlColor == CTLCOLOR_BTN)
	{
		hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);	
	}
	else
		hbr = CStatic::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}


CMonthSpinCtrl::CMonthSpinCtrl()
{
	m_nMin = 1;
	m_nMax = 12;
}

void CMonthSpinCtrl::SetPos(int nPos)
{
	m_nPos = nPos;
	int nValue = m_nMin + nPos - 1;
	ASSERT(nValue >= 0 && nValue < 13);

#ifdef LOCALIZE
	m_strStaticText = CurrentLocale.AbbrevMonthName((MONTH)nValue);
	m_strStaticText.MakeUpper();
#else
	m_strStaticText.LoadString(IDS_MONTH1 + nValue);
#endif
	m_cStaticText.SetText(m_strStaticText);
}



/////////////////////////////////////////////////////////////////////////////
// CSpinDate

CSpinDate::CSpinDate()
{
}

CSpinDate::~CSpinDate()
{
}


BEGIN_MESSAGE_MAP(CSpinDate, CStatic)
	//{{AFX_MSG_MAP(CSpinDate)
	ON_MESSAGE(UM_SPIN_CHANGE, OnSpinChange)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpinDate message handlers

void CSpinDate::Init(BOOL bIncludeDays)
{
	m_bIncludeDays = bIncludeDays;
	
	m_Time.Set();
	if(bIncludeDays)
	{
		CTimeStampSpan tmSpan(7,0,0,0);
		m_Time += tmSpan;
	}
	FILETIME ft = m_Time.GetTime();
	FileTimeToSystemTime(&ft, &m_sysTime);

	CRect rcClient;
	GetClientRect(&rcClient);
	int nDenominator = 2;

	if(m_bIncludeDays)
		nDenominator = 3;

	int nWidth = rcClient.Width() / nDenominator;

	CRect rcMonth;
	CRect rcDay;
	CRect rcYear;

	if(m_bIncludeDays)
	{
		rcMonth.SetRect(rcClient.left, rcClient.top, rcClient.left + nWidth, rcClient.bottom);
		rcDay.SetRect(rcMonth.right, rcClient.top, rcMonth.right + nWidth, rcClient.bottom);
		rcYear.SetRect(rcDay.right, rcClient.top, rcClient.right, rcClient.bottom);
		rcDay.InflateRect(-rcClient.Height() / 2, 0);
	}
	else
	{
		nWidth -= rcClient.Height() / 2;
		rcMonth.SetRect(rcClient.left, rcClient.top, rcClient.left + nWidth , rcClient.bottom);
		rcYear.SetRect(rcClient.right - nWidth, rcClient.top, rcClient.right, rcClient.bottom);
	}


	m_cMonth.Create("", WS_CHILD | WS_VISIBLE, rcMonth, this, IDC_SETTINGMONTH);
	m_cMonth.Init();
	m_cYear.Create("", WS_CHILD | WS_VISIBLE, rcYear, this, IDC_SETTINGYEAR);
	m_cYear.Init();

	if(m_bIncludeDays)
	{
		m_cDay.Create("", WS_CHILD | WS_VISIBLE, rcDay, this, IDC_SETTINGDAY);
		m_cDay.Init();
		m_cDay.SetRange(1, DaysInMonth(m_sysTime.wMonth));
		m_cDay.SetPos(m_sysTime.wDay - 1);
	}

	m_cMonth.SetRange(1, 12);
	m_cMonth.SetPos(m_sysTime.wMonth - 1);

	m_cYear.SetRange(m_sysTime.wYear, m_sysTime.wYear + 2000);
	m_cYear.SetPos(0);	
}

int CSpinDate::DaysInMonth(int nMonth)
{
	int nDays = 31;
	if(nMonth > 7)
	{
		if(nMonth % 2)
			nDays = 30;

	}
	else 
	{
		if(nMonth == 2)
			nDays = 28;
		else if(!(nMonth % 2))
			nDays = 30;
	}
	return nDays;
}

LRESULT CSpinDate::OnSpinChange(WORD wParam, LONG lParam) 
{
	//this is a switch statement in case handlers are wanted for changing dya and year
	switch(lParam)
	{
		case IDC_SETTINGMONTH:
		{
			OnChangeMonth(wParam);
			break;
		}
		case IDC_SETTINGYEAR:
		{
			OnChangeYear(wParam);
			break;
		}
		case IDC_SETTINGDAY:
		{
			OnChangeDay(wParam);
			break;
		}
		default:
			break;
	}
	return 1L;
}

void CSpinDate::OnChangeYear( int nNewYear ) 
{
	int nMonth = m_cMonth.GetValue();
	int nDay = m_cDay.GetValue();
	int nCurYear = m_cYear.GetValue();

	if(!IsBeforeSysTime(nDay, nMonth, nNewYear))
		m_cYear.SetPos(m_cYear.GetPos() + nNewYear - nCurYear);
}

void CSpinDate::OnChangeDay( int nDay ) 
{
	int nMonth = m_cMonth.GetValue();
	int nMaxDays = DaysInMonth(nMonth);
	int nYear = m_cYear.GetValue();


	if(nDay > nMaxDays)
	{
		int nTempMonth = nMonth + 1;
		if(nMonth == 12)
		{
			if(nYear == 3000)
				return;

			m_cYear.SetPos(m_cYear.GetPos() + 1);
			nTempMonth = 1;
		}
		m_cMonth.SetPos(nTempMonth - 1);
		m_cDay.SetRange(1, DaysInMonth(nTempMonth));
		m_cDay.SetPos(0);

		return;
	}
	if(nDay < 1)
	{
		int nTempMonth = nMonth - 1;
		int nTempYear = nYear;
		if(nMonth == 1)
		{
			nTempMonth = 12;
			nTempYear--;
		}
		if(!IsBeforeSysTime(DaysInMonth(nTempMonth), nTempMonth, nTempYear))
		{
			m_cMonth.SetPos(nTempMonth - 1);

			if(nYear != nTempYear)
				m_cYear.SetPos(m_cYear.GetPos() - 1);

			m_cDay.SetRange(1, DaysInMonth(nTempMonth));
			m_cDay.SetPos(DaysInMonth(nTempMonth) - 1);
			return;
		}
	}	
	
	if(!IsBeforeSysTime(nDay, nMonth, nYear))
	{
		m_cDay.SetPos(nDay - 1);
	}
}

void CSpinDate::OnChangeMonth( int nMonth ) 
{
	int nDay = m_cDay.GetValue();
	int nYear = m_cYear.GetValue();

	if(nMonth > 12)
	{
		m_cYear.SetPos(m_cYear.GetPos() + 1);
		
		if(m_bIncludeDays)
			m_cDay.SetRange(1, 31);//Jan

		m_cMonth.SetPos(0);
		return;
	}
	if(nMonth < 1)
	{
		if(!IsBeforeSysTime(nDay, 12, nYear - 1))
		{
			m_cYear.SetPos(m_cYear.GetPos() - 1);

			if(m_bIncludeDays)
				m_cDay.SetRange(1,31);//Dec

			m_cMonth.SetPos(11);
			return;	
		}
	}
	
	if(!IsBeforeSysTime(nDay, nMonth, nYear))
	{
		if(m_bIncludeDays)
			m_cDay.SetRange(1, DaysInMonth(nMonth));
	
		m_cMonth.SetPos(nMonth - 1);
	}	

}

BOOL CSpinDate::IsBeforeSysTime(int nDay, int nMonth, int nYear )
{
	if(nYear > m_sysTime.wYear)
		return FALSE;

	else if(nYear == m_sysTime.wYear && nMonth > m_sysTime.wMonth)
		return FALSE;

	else if(nYear == m_sysTime.wYear && nMonth == m_sysTime.wMonth && ( !m_bIncludeDays  || nDay >= m_sysTime.wDay ) )
		return FALSE;

	return TRUE;
}	

CTimeStamp CSpinDate::GetSetTime()
{
	SYSTEMTIME time = m_sysTime;

	if(m_bIncludeDays)
		time.wDay = m_cDay.GetValue();
	
	time.wMonth = m_cMonth.GetValue();
	time.wYear = m_cYear.GetValue();

	CTimeStamp result;
	result.Set(time);
	
	return result;
}

HBRUSH CSpinDate::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = NULL;
	
	if(nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_BTN)
	{
		pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		pDC->SetBkMode(TRANSPARENT);
		hbr =  (HBRUSH)GetStockObject(HOLLOW_BRUSH);	
	}
	else
		hbr = CStatic::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

