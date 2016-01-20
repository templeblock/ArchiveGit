#include "stdafx.h"
#include "EffectsTab.h"
#include "MainDlg.h"
#include "Utility.h"

typedef struct
{
	LPCTSTR pName;
    int iParamter;
} CEffectEntry;

const static CEffectEntry g_EffectEntries[] = 
{
	{_T("None"),		0},
	{_T("Sharpen"),		0},
	{_T("Blur"),		0},
	{_T("Edges"),		0},
	{_T("Emboss"),		0},
	{_T("Pixelize"),	0},
	{_T("Posterize"),	0},
	{_T("Prism"),		0},
	{_T("Oil Painting"),0},
	{_T("Antiqued"),	0},
//j	{_T("Texturize"),	0},
};

/////////////////////////////////////////////////////////////////////////////
CEffectsTab::CEffectsTab()
{
	m_iIntensity = -1;
}

/////////////////////////////////////////////////////////////////////////////
CEffectsTab::~CEffectsTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CEffectsTab::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	DoDataExchange(DDX_LOAD);
	SetMsgHandled(false);

	int nSizes = sizeof(g_EffectEntries) / sizeof(CEffectEntry);
	for (int i=0; i<nSizes; i++)
		m_listEffects.AddString(Str(_T("%s"), g_EffectEntries[i].pName));

	m_IntensitySlider.SetRange(0, 100);
	m_IntensitySlider.SetTicFreq(10);
	m_IntensitySlider.SetPageSize(100/10);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnEnterTab(bool bFirstTime)
{
	m_iIntensity = 50;
	m_IntensitySlider.SetPos(m_iIntensity);
	m_staticIntensity.SetWindowText(Str(_T("%d"), m_iIntensity));

	int iIndex = 0;
	m_listEffects.SetCurSel(iIndex);

	// The 0th entry in the list ("None") disables the intensity slider
	bool bEnable = (iIndex != 0);
	m_IntensitySlider.EnableWindow(bEnable);

	bool bOK = DoCommand(_T("EffectsTab"), Str(_T("%d"), EDIT_START));
	bOK = DoCommand(_T("EffectsTabSubCommand"), g_EffectEntries[iIndex].pName);
	EnableButtons(/*fEnableApply*/bEnable, /*fEnableReset*/bEnable);
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("EffectsTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnApply(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("EffectsTab"), Str(_T("%d"), EDIT_APPLY));
	OnEnterTab();
	GoBack(); // added
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnReset(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("EffectsTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnEffectsListChanged(UINT uCode, int nID, HWND hwndCtrl)
{
//j	bool bOK = DoCommand(_T("EffectsTab"), Str(_T("%d"), EDIT_RESET));

	int i = m_listEffects.GetCurSel();
	if (i < 0) i = 0;

	// The 0th entry in the list ("None") disables the intensity slider
	bool bEnable = (i != 0);
	m_IntensitySlider.EnableWindow(bEnable);

	bool bOK = DoCommand(_T("EffectsTabSubCommand"), g_EffectEntries[i].pName);
	bOK = DoCommand(_T("EffectsTab"), Str(_T("%d"), m_iIntensity));
	EnableButtons(/*fEnableApply*/bEnable, /*fEnableReset*/bEnable);
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnMakeGray(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("MakeGray"), _T(""));
	EnableButtons(/*Apply*/true, /*Reset*/true);
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnInvert(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("Invert"), _T(""));
	EnableButtons(/*Apply*/true, /*Reset*/true);
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnHScroll(int nSBCode, short nPos, HWND hWnd)
{
	CTrackBarCtrl pSliderCtrl(hWnd);
	switch (nSBCode)
	{
		case TB_THUMBTRACK:
		case TB_ENDTRACK:
		{
			int iValue = pSliderCtrl.GetPos();

			if (pSliderCtrl.m_hWnd == m_IntensitySlider.m_hWnd)
			{
				if (m_iIntensity == iValue)
					return;

				m_iIntensity = iValue;
				m_staticIntensity.SetWindowText(Str(_T("%d"), m_iIntensity));
				DoCommand(_T("EffectsTab"), Str(_T("%d"), m_iIntensity));
			}

			break;
		}
	}
}
