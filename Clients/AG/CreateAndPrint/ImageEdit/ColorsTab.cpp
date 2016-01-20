#include "stdafx.h"
#include "ColorsTab.h"
#include "MainDlg.h"

/////////////////////////////////////////////////////////////////////////////
CColorsTab::CColorsTab()
{
}

/////////////////////////////////////////////////////////////////////////////
CColorsTab::~CColorsTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CColorsTab::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	DoDataExchange(DDX_LOAD);
	SetMsgHandled(false);

	m_AmountSlider.SetRange(0, 100);
	m_AmountSlider.SetTicFreq(10);
	m_AmountSlider.SetPageSize(100/10);

	m_HueSlider.SetRange(0, 100);
	m_HueSlider.SetTicFreq(10);
	m_HueSlider.SetPageSize(100/10);

	m_SaturationSlider.SetRange(0, 100);
	m_SaturationSlider.SetTicFreq(10);
	m_SaturationSlider.SetPageSize(100/10);

	DWORD dwFlags = CDG_HASITEMS | CDG_HORIZONTAL;
	m_AmountSlider.SetStyle(m_AmountSlider, dwFlags, RGB(255,255,255), RGB(0,0,0));
	m_HueSlider.SetStyle(m_HueSlider, dwFlags | CDG_HUE, RGB(0,255,128), RGB(252,255,128));
	m_SaturationSlider.SetStyle(m_SaturationSlider, dwFlags, RGB(128,128,128), RGB(0,0,255));

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnEnterTab(bool bFirstTime)
{
	m_iAmount = 0;
	m_AmountSlider.SetPos(m_iAmount);
	m_staticAmount.SetWindowText(Str(_T("%d"), m_iAmount));

	m_iHue = 50;
	m_HueSlider.SetPos(m_iHue);
	m_staticHue.SetWindowText(Str(_T("%d"), m_iHue));

	m_iSaturation = 50;
	m_SaturationSlider.SetPos(m_iSaturation);
	m_staticSaturation.SetWindowText(Str(_T("%d"), m_iSaturation));

	bool bOK = DoCommand(_T("ColorsTab"), Str(_T("%d"), EDIT_START));
	EnableButtons(/*Apply*/false, /*Reset*/false);
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("ColorsTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnApply(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("ColorsTab"), Str(_T("%d"), EDIT_APPLY));
	OnEnterTab();
	GoBack(); // added
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnReset(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("ColorsTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnHScroll(int nSBCode, short nPos, HWND hWnd)
{
	CTrackBarCtrl pSliderCtrl(hWnd);
	switch (nSBCode)
	{
		case TB_THUMBTRACK:
		case TB_ENDTRACK:
		{
			int iValue = pSliderCtrl.GetPos();

			if (pSliderCtrl.m_hWnd == m_AmountSlider.m_hWnd)
			{
				if (m_iAmount == iValue)
					return;

				m_iAmount = iValue;
				CString strValue = Str(_T("%d"), iValue);
				m_staticAmount.SetWindowText(strValue);
			}
			else
			if (pSliderCtrl.m_hWnd == m_HueSlider.m_hWnd)
			{
				if (m_iHue == iValue)
					return;

				m_iHue = iValue;
				CString strValue = Str(_T("%d"), iValue);
				m_staticHue.SetWindowText(strValue);
			}
			else
			if (pSliderCtrl.m_hWnd == m_SaturationSlider.m_hWnd)
			{
				if (m_iSaturation == iValue)
					return;

				m_iSaturation = iValue;
				CString strValue = Str(_T("%d"), iValue);
				m_staticSaturation.SetWindowText(strValue);
			}

			EnableButtons(/*Apply*/true, /*Reset*/true);

			iValue = MAKELONG(MAKEWORD(m_iHue, m_iSaturation), m_iAmount);
			bool bOK = DoCommand(_T("ColorsTab"), Str(_T("%d"), iValue));
			break;
		}
	}
}
