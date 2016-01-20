#include "stdafx.h"
#include "ColorsTab.h"
#include "TabControl.h"
#include "MainDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CColorsTab, CBaseTab)
BEGIN_MESSAGE_MAP(CColorsTab, CBaseTab)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_MAKEGRAY, OnMakeGray)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_RESET, OnReset)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AMOUNT_SLIDER, m_AmountSlider);
	DDX_Control(pDX, IDC_HUE_SLIDER, m_HueSlider);
	DDX_Control(pDX, IDC_SATURATION_SLIDER, m_SaturationSlider);
}

/////////////////////////////////////////////////////////////////////////////
CColorsTab::CColorsTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CColorsTab::IDD, pParent)
{
}

/////////////////////////////////////////////////////////////////////////////
CColorsTab::~CColorsTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CColorsTab::OnInitDialog()
{
	// Initialize the resize dialog base class
	CBaseTab::OnInitDialog();

	EnableControl(IDC_APPLY, false);
	EnableControl(IDC_RESET, false);

	m_AmountSlider.SetRange(0, 100);
	m_HueSlider.SetRange(0, 100);
	m_SaturationSlider.SetRange(0, 100);

	m_AmountSlider.SetTicFreq(10);
	m_HueSlider.SetTicFreq(10);
	m_SaturationSlider.SetTicFreq(10);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnEnterTab()
{
	m_iAmount = 0;
	m_iHue = 50;
	m_iSaturation = 50;

	m_AmountSlider.SetPos(m_iAmount);
	m_HueSlider.SetPos(m_iHue);
	m_SaturationSlider.SetPos(m_iSaturation);

	bool bOK = DoCommand(_T("HueSaturationTab"), Str(_T("%d"), EDIT_START));
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("HueSaturationTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnApply()
{
	bool bOK = DoCommand(_T("HueSaturationTab"), Str(_T("%d"), EDIT_APPLY));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnReset()
{
	bool bOK = DoCommand(_T("HueSaturationTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnMakeGray()
{
	DoCommand(_T("MakeGray"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSliderCtrl = (CSliderCtrl*)pScrollBar;
	switch (nSBCode)
	{
		case TB_THUMBTRACK:
		case TB_ENDTRACK:
		{
			int iValue = pSliderCtrl->GetPos();

			if (pSliderCtrl == &m_AmountSlider)
			{
				if (m_iAmount == iValue)
					return;

				m_iAmount = iValue;
			}
			else
			if (pSliderCtrl == &m_HueSlider)
			{
				if (m_iHue == iValue)
					return;

				m_iHue = iValue;
			}
			else
			if (pSliderCtrl == &m_SaturationSlider)
			{
				if (m_iSaturation == iValue)
					return;

				m_iSaturation = iValue;
			}

			EnableControl(IDC_APPLY, true);
			EnableControl(IDC_RESET, true);

			iValue = MAKELONG(MAKEWORD(m_iHue, m_iSaturation), m_iAmount);
			bool bOK = DoCommand(_T("HueSaturationTab"), Str(_T("%d"), iValue));
			break;
		}
	}
}
