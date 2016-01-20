#include "stdafx.h"
#include "LayoutTab.h"
#include "MainDlg.h"

/////////////////////////////////////////////////////////////////////////////
CFlipRotateTab::CFlipRotateTab()
{
	m_iDegrees = 0;
}

/////////////////////////////////////////////////////////////////////////////
CFlipRotateTab::~CFlipRotateTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CFlipRotateTab::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	DoDataExchange(DDX_LOAD);
	SetMsgHandled(false);

	m_RotateSlider.SetRange(-180, 180);
	m_RotateSlider.SetTicFreq(30);
	m_RotateSlider.SetPageSize(360/30);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CFlipRotateTab::OnEnterTab(bool bFirstTime)
{
	m_iDegrees = GetCommand(_T("GetAngle"));
	m_RotateSlider.SetPos(m_iDegrees);
	m_staticRotateAmount.SetWindowText(Str(_T("%d"), m_iDegrees));

	bool bOK = DoCommand(_T("FlipRotateTab"), Str(_T("%d"), EDIT_START));
	EnableButtons(/*Apply*/false, /*Reset*/false);
}

/////////////////////////////////////////////////////////////////////////////
void CFlipRotateTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("FlipRotateTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CFlipRotateTab::OnApply(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("FlipRotateTab"), Str(_T("%d"), EDIT_APPLY));
	OnEnterTab();
	GoBack(); // added
}

/////////////////////////////////////////////////////////////////////////////
void CFlipRotateTab::OnReset(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("FlipRotateTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CFlipRotateTab::OnFlipHorizontal(UINT uCode, int nID, HWND hwndCtrl)
{
	DoCommand(_T("FlipRotateTabSubCommand"), _T("FlipHorizontal"));
	EnableButtons(/*Apply*/true, /*Reset*/true);
}

/////////////////////////////////////////////////////////////////////////////
void CFlipRotateTab::OnFlipVertical(UINT uCode, int nID, HWND hwndCtrl)
{
	DoCommand(_T("FlipRotateTabSubCommand"), _T("FlipVertical"));
	EnableButtons(/*Apply*/true, /*Reset*/true);
}

/////////////////////////////////////////////////////////////////////////////
void CFlipRotateTab::OnRotateLeft(UINT uCode, int nID, HWND hwndCtrl)
{
	m_iDegrees -= 90;
	while (m_iDegrees < -180)
		m_iDegrees += 360;
	m_RotateSlider.SetPos(m_iDegrees);
	m_staticRotateAmount.SetWindowText(Str(_T("%d"), m_iDegrees));

	DoCommand(_T("FlipRotateTab"), Str(_T("%d"), PositiveDegrees()));
	EnableButtons(/*Apply*/true, /*Reset*/true);
}

/////////////////////////////////////////////////////////////////////////////
void CFlipRotateTab::OnRotateRight(UINT uCode, int nID, HWND hwndCtrl)
{
	m_iDegrees += 90;
	while (m_iDegrees > 180)
		m_iDegrees -= 360;
	m_RotateSlider.SetPos(m_iDegrees);
	m_staticRotateAmount.SetWindowText(Str(_T("%d"), m_iDegrees));

	DoCommand(_T("FlipRotateTab"), Str(_T("%d"), PositiveDegrees()));
	EnableButtons(/*Apply*/true, /*Reset*/true);
}

/////////////////////////////////////////////////////////////////////////////
void CFlipRotateTab::OnHScroll(int nSBCode, short nPos, HWND hWnd)
{
	CTrackBarCtrl pSliderCtrl(hWnd);
	switch (nSBCode)
	{
		case TB_ENDTRACK:
		{
			EnableButtons(/*Apply*/true, /*Reset*/true);
			// Fall through...
		}
		case TB_THUMBTRACK:
		{
			int iValue = pSliderCtrl.GetPos();

			if (pSliderCtrl.m_hWnd == m_RotateSlider.m_hWnd)
			{
				if (m_iDegrees == iValue)
					return;

				m_iDegrees = iValue;
				m_staticRotateAmount.SetWindowText(Str(_T("%d"), m_iDegrees));
				DoCommand(_T("FlipRotateTab"), Str(_T("%d"), PositiveDegrees()));
			}

			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
int CFlipRotateTab::PositiveDegrees()
{
	int iPositiveDegrees = m_iDegrees;
	while (iPositiveDegrees < 0)
		iPositiveDegrees += 360;
	while (iPositiveDegrees > 360)
		iPositiveDegrees -= 360;
	return iPositiveDegrees;
}
