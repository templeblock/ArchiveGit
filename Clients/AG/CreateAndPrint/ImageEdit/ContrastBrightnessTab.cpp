#include "stdafx.h"
#include "ContrastBrightnessTab.h"
#include "MainDlg.h"

/////////////////////////////////////////////////////////////////////////////
CContrastBrightnessTab::CContrastBrightnessTab()
{
}

/////////////////////////////////////////////////////////////////////////////
CContrastBrightnessTab::~CContrastBrightnessTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CContrastBrightnessTab::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	DoDataExchange(DDX_LOAD);
	SetMsgHandled(false);

	m_ContrastSlider.SetRange(0, 100);
	m_ContrastSlider.SetTicFreq(10);
	m_ContrastSlider.SetPageSize(100/10);

	m_BrightnessSlider.SetRange(0, 100);
	m_BrightnessSlider.SetTicFreq(10);
	m_BrightnessSlider.SetPageSize(100/10);

	DWORD dwFlags = CDG_HASITEMS | CDG_HORIZONTAL;
	m_ContrastSlider.SetStyle(m_ContrastSlider, dwFlags | CDG_CONTRAST, RGB(255,255,255), RGB(0,0,0));
	m_BrightnessSlider.SetStyle(m_BrightnessSlider, dwFlags, RGB(0,0,0), RGB(255,255,255));

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnEnterTab(bool bFirstTime)
{
	m_iContrast = 50;
	m_ContrastSlider.SetPos(m_iContrast);
	m_staticContrast.SetWindowText(Str(_T("%d"), m_iContrast));

	m_iBrightness = 50;
	m_BrightnessSlider.SetPos(m_iBrightness);
	m_staticBrightness.SetWindowText(Str(_T("%d"), m_iBrightness));

	bool bOK = DoCommand(_T("ContrastBrightnessTab"), Str(_T("%d"), EDIT_START));
	EnableButtons(/*Apply*/false, /*Reset*/false);
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("ContrastBrightnessTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnApply(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("ContrastBrightnessTab"), Str(_T("%d"), EDIT_APPLY));
	OnEnterTab();
	GoBack(); // added
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnReset(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("ContrastBrightnessTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnAutoCorrect(UINT uCode, int nID, HWND hwndCtrl)
{
	m_iContrast = 50;
	m_iBrightness = 50;

	m_ContrastSlider.SetPos(m_iContrast);
	m_BrightnessSlider.SetPos(m_iBrightness);

	bool bOK = DoCommand(_T("AutoCorrect"), _T(""));
	EnableButtons(/*Apply*/true, /*Reset*/true);
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnHScroll(int nSBCode, short nPos, HWND hWnd)
{
	CTrackBarCtrl pSliderCtrl(hWnd);
	switch (nSBCode)
	{
		case TB_THUMBTRACK:
		case TB_ENDTRACK:
		{
			int iValue = pSliderCtrl.GetPos();

			if (pSliderCtrl.m_hWnd == m_ContrastSlider.m_hWnd)
			{
				if (m_iContrast == iValue)
					return;

				m_iContrast = iValue;
				CString strValue = Str(_T("%d"), iValue);
				m_staticContrast.SetWindowText(strValue);
			}
			else
			if (pSliderCtrl.m_hWnd == m_BrightnessSlider.m_hWnd)
			{
				if (m_iBrightness == iValue)
					return;

				m_iBrightness = iValue;
				CString strValue = Str(_T("%d"), iValue);
				m_staticBrightness.SetWindowText(strValue);
			}

			EnableButtons(/*Apply*/true, /*Reset*/true);

			iValue = MAKEWORD(m_iContrast, m_iBrightness);
			bool bOK = DoCommand(_T("ContrastBrightnessTab"), Str(_T("%d"), iValue));
			break;
		}
	}
}
