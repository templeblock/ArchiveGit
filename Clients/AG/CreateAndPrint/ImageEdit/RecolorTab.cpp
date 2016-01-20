#include "stdafx.h"
#include "RecolorTab.h"
#include "MainDlg.h"

/////////////////////////////////////////////////////////////////////////////
CRecolorTab::CRecolorTab()
{
	m_Primary1Color = RGB(0,0,0);
	m_Primary2Color = RGB(0,0,0);
	m_SecondaryColor = RGB(255,255,255);
	m_iTolerance = 0;
}

/////////////////////////////////////////////////////////////////////////////
CRecolorTab::~CRecolorTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRecolorTab::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	DoDataExchange(DDX_LOAD);
	SetMsgHandled(false);

	CheckRadioButton(IDC_IME_RECOLOR_SOLID, IDC_IME_RECOLOR_SWEEP_CENTER, IDC_IME_RECOLOR_SOLID);
	m_staticGradient.SetStyle(m_staticGradient, GetFlags(), m_Primary2Color, m_SecondaryColor);

	m_ToleranceSlider.SetRange(0, 100);
	m_ToleranceSlider.SetTicFreq(10);
	m_ToleranceSlider.SetPageSize(100/10);

	m_iTolerance = 0;
	m_ToleranceSlider.SetPos(m_iTolerance);
	m_staticTolerance.SetWindowText(Str(_T("%d"), m_iTolerance));

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
int CRecolorTab::GetFlags()
{
	if (IsDlgButtonChecked(IDC_IME_RECOLOR_SOLID))
		return CDG_SOLID;
	if (IsDlgButtonChecked(IDC_IME_RECOLOR_SWEEP_DOWN))
		return CDG_VERTICAL;
	if (IsDlgButtonChecked(IDC_IME_RECOLOR_SWEEP_RIGHT))
		return CDG_HORIZONTAL;
	if (IsDlgButtonChecked(IDC_IME_RECOLOR_SWEEP_CENTER))
		return CDG_FROMCENTER;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnEnterTab(bool bFirstTime)
{
	if (bFirstTime)
	{
		m_btnPrimary1Patch.SetColor(m_Primary1Color);
		m_btnPrimary2Patch.SetColor(m_Primary2Color);
		m_btnSecondaryPatch.SetColor(m_SecondaryColor);
		m_staticGradient.SetStyle(m_staticGradient, GetFlags(), m_Primary2Color, m_SecondaryColor);
	}

	DoCommand(_T("SetPrimaryColor"), Str(_T("%d"), m_Primary1Color));
	DoCommand(_T("SetSecondaryColor"), Str(_T("%d"), m_SecondaryColor));

	if (IsDlgButtonChecked(IDC_IME_RECOLOR_SOLID))
		DoCommand(_T("RecolorTabSubCommand"), _T("SolidFill"));
	else
	if (IsDlgButtonChecked(IDC_IME_RECOLOR_SWEEP_DOWN))
		DoCommand(_T("RecolorTabSubCommand"), _T("SweepDown"));
	else
	if (IsDlgButtonChecked(IDC_IME_RECOLOR_SWEEP_RIGHT))
		DoCommand(_T("RecolorTabSubCommand"), _T("SweepRight"));
	else
	if (IsDlgButtonChecked(IDC_IME_RECOLOR_SWEEP_CENTER))
		DoCommand(_T("RecolorTabSubCommand"), _T("SweepCenter"));

	bool bOK = DoCommand(_T("RecolorTab"), Str(_T("%d"), EDIT_START));
	EnableButtons(/*Apply*/false, /*Reset*/false);
	SetTimer(TAB_TIMER, 1000);
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("RecolorTab"), Str(_T("%d"), EDIT_END));
	KillTimer(TAB_TIMER);
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnApply(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("RecolorTab"), Str(_T("%d"), EDIT_APPLY));
	OnEnterTab();
	GoBack(); // added
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnReset(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("RecolorTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSolid(UINT uCode, int nID, HWND hwndCtrl)
{
	CheckRadioButton(IDC_IME_RECOLOR_SOLID, IDC_IME_RECOLOR_SWEEP_CENTER, IDC_IME_RECOLOR_SOLID);
	m_staticGradient.SetStyle(m_staticGradient, GetFlags(), m_Primary2Color, m_SecondaryColor);
	DoCommand(_T("SetPrimaryColor"), Str(_T("%d"), m_Primary1Color));
	DoCommand(_T("RecolorTabSubCommand"), _T("SolidFill"));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSweepDown(UINT uCode, int nID, HWND hwndCtrl)
{
	CheckRadioButton(IDC_IME_RECOLOR_SOLID, IDC_IME_RECOLOR_SWEEP_CENTER, IDC_IME_RECOLOR_SWEEP_DOWN);
	m_staticGradient.SetStyle(m_staticGradient, GetFlags(), m_Primary2Color, m_SecondaryColor);
	DoCommand(_T("SetPrimaryColor"), Str(_T("%d"), m_Primary2Color));
	DoCommand(_T("SetSecondaryColor"), Str(_T("%d"), m_SecondaryColor));
	DoCommand(_T("RecolorTabSubCommand"), _T("SweepDown"));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSweepRight(UINT uCode, int nID, HWND hwndCtrl)
{
	CheckRadioButton(IDC_IME_RECOLOR_SOLID, IDC_IME_RECOLOR_SWEEP_CENTER, IDC_IME_RECOLOR_SWEEP_RIGHT);
	m_staticGradient.SetStyle(m_staticGradient, GetFlags(), m_Primary2Color, m_SecondaryColor);
	DoCommand(_T("SetPrimaryColor"), Str(_T("%d"), m_Primary2Color));
	DoCommand(_T("SetSecondaryColor"), Str(_T("%d"), m_SecondaryColor));
	DoCommand(_T("RecolorTabSubCommand"), _T("SweepRight"));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSweepFromCenter(UINT uCode, int nID, HWND hwndCtrl)
{
	CheckRadioButton(IDC_IME_RECOLOR_SOLID, IDC_IME_RECOLOR_SWEEP_CENTER, IDC_IME_RECOLOR_SWEEP_CENTER);
	m_staticGradient.SetStyle(m_staticGradient, GetFlags(), m_Primary2Color, m_SecondaryColor);
	DoCommand(_T("SetPrimaryColor"), Str(_T("%d"), m_Primary2Color));
	DoCommand(_T("SetSecondaryColor"), Str(_T("%d"), m_SecondaryColor));
	DoCommand(_T("RecolorTabSubCommand"), _T("SweepCenter"));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSwapColors(UINT uCode, int nID, HWND hwndCtrl)
{
	COLORREF TempColor = m_Primary2Color;
	m_Primary2Color = m_SecondaryColor;
	m_SecondaryColor = TempColor;

	m_btnPrimary2Patch.SetColor(m_Primary2Color);
	m_btnSecondaryPatch.SetColor(m_SecondaryColor);
	m_staticGradient.SetStyle(m_staticGradient, GetFlags(), m_Primary2Color, m_SecondaryColor);

	DoCommand(_T("SetPrimaryColor"), Str(_T("%d"), m_Primary2Color));
	DoCommand(_T("SetSecondaryColor"), Str(_T("%d"), m_SecondaryColor));

	if (IsDlgButtonChecked(IDC_IME_RECOLOR_SOLID))
		OnRecolorSweepDown(uCode, nID, hwndCtrl);
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorPrimary1Patch(UINT uCode, int nID, HWND hwndCtrl)
{
	OnRecolorPrimary1PatchDoubleClick(uCode, nID, hwndCtrl);
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorPrimary2Patch(UINT uCode, int nID, HWND hwndCtrl)
{
	OnRecolorPrimary2PatchDoubleClick(uCode, nID, hwndCtrl);
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSecondaryPatch(UINT uCode, int nID, HWND hwndCtrl)
{
	OnRecolorSecondaryPatchDoubleClick(uCode, nID, hwndCtrl);
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorPrimary1PatchDoubleClick(UINT uCode, int nID, HWND hwndCtrl)
{
	if (!m_btnPrimary1Patch.PickColor())
		return;

	COLORREF Color = m_btnPrimary1Patch.GetColor();
	if (m_Primary1Color == Color)
		return;

	m_Primary1Color = Color;
	DoCommand(_T("SetPrimaryColor"), Str(_T("%d"), m_Primary1Color));

	if (!IsDlgButtonChecked(IDC_IME_RECOLOR_SOLID))
		OnRecolorSolid(uCode, nID, hwndCtrl);
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorPrimary2PatchDoubleClick(UINT uCode, int nID, HWND hwndCtrl)
{
	if (!m_btnPrimary2Patch.PickColor())
		return;

	COLORREF Color = m_btnPrimary2Patch.GetColor();
	if (m_Primary2Color == Color)
		return;

	m_Primary2Color = Color;
	m_staticGradient.SetStyle(m_staticGradient, GetFlags(), m_Primary2Color, m_SecondaryColor);
	DoCommand(_T("SetPrimaryColor"), Str(_T("%d"), m_Primary2Color));

	if (IsDlgButtonChecked(IDC_IME_RECOLOR_SOLID))
		OnRecolorSweepDown(uCode, nID, hwndCtrl);
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSecondaryPatchDoubleClick(UINT uCode, int nID, HWND hwndCtrl)
{
	if (!m_btnSecondaryPatch.PickColor())
		return;

	COLORREF Color = m_btnSecondaryPatch.GetColor();
	if (m_SecondaryColor == Color)
		return;

	m_SecondaryColor = Color;
	m_staticGradient.SetStyle(m_staticGradient, GetFlags(), m_Primary2Color, m_SecondaryColor);
	DoCommand(_T("SetSecondaryColor"), Str(_T("%d"), m_SecondaryColor));

	if (IsDlgButtonChecked(IDC_IME_RECOLOR_SOLID))
		OnRecolorSweepDown(uCode, nID, hwndCtrl);
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnHScroll(int nSBCode, short nPos, HWND hWnd)
{
	CTrackBarCtrl pSliderCtrl(hWnd);
	switch (nSBCode)
	{
		case TB_THUMBTRACK:
		case TB_ENDTRACK:
		{
			int iValue = pSliderCtrl.GetPos();

			if (pSliderCtrl.m_hWnd == m_ToleranceSlider.m_hWnd)
			{
				if (m_iTolerance == iValue)
					return;

				m_iTolerance = iValue;

				CString strValue = Str(_T("%d"), iValue);
				m_staticTolerance.SetWindowText(strValue);
				DoCommand(_T("Tolerance"), strValue);
			}

			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CRecolorTab::OnTimer(UINT nIDEvent, TIMERPROC TimerProc)
{
	if (nIDEvent == TAB_TIMER && GetCommand(_T("GetCanUndo")))
		EnableButtons(/*fEnableApply*/true, /*fEnableReset*/true);
}
