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
	ON_BN_CLICKED(IDC_HUE, OnHue)
	ON_BN_CLICKED(IDC_HUE_APPLY, OnHueApply)
	ON_BN_CLICKED(IDC_HUE_CANCEL, OnHueCancel)
	ON_BN_CLICKED(IDC_SATURATION, OnSaturation)
	ON_BN_CLICKED(IDC_SATURATION_APPLY, OnSaturationApply)
	ON_BN_CLICKED(IDC_SATURATION_CANCEL, OnSaturationCancel)
	ON_BN_CLICKED(IDC_MAKEGRAY, OnMakeGray)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CColorsTab::CColorsTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CColorsTab::IDD, pParent)
{
	m_iLastValue = -1;
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

	EnableControl(IDC_HUE, true);
	ShowControl(IDC_HUE_APPLY, SW_HIDE);
	ShowControl(IDC_HUE_CANCEL, SW_HIDE);

	m_AmountSlider.EnableWindow(false);
	m_AmountSlider.SetRange(0, 100);
	m_AmountSlider.SetTicFreq(10);
	m_AmountSlider.SetPos(50);

	m_HueSlider.EnableWindow(false);
	m_HueSlider.SetRange(0, 100);
	m_HueSlider.SetTicFreq(10);
	m_HueSlider.SetPos(50);

	EnableControl(IDC_SATURATION, true);
	ShowControl(IDC_SATURATION_APPLY, SW_HIDE);
	ShowControl(IDC_SATURATION_CANCEL, SW_HIDE);

	m_SaturationSlider.EnableWindow(false);
	m_SaturationSlider.SetRange(0, 100);
	m_SaturationSlider.SetTicFreq(10);
	m_SaturationSlider.SetPos(50);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AMOUNT_SLIDER, m_AmountSlider);
	DDX_Control(pDX, IDC_HUE_SLIDER, m_HueSlider);
	DDX_Control(pDX, IDC_SATURATION_SLIDER, m_SaturationSlider);
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnHue()
{
	if (!DoCommand(_T("Hue"), _T("-1")))
		return;

	EnableControl(IDC_HUE, false);
	ShowControl(IDC_HUE_APPLY, SW_SHOW);
	ShowControl(IDC_HUE_CANCEL, SW_SHOW);

	m_HueSlider.SetPos(50);
	m_HueSlider.EnableWindow(true);
	m_iLastValue = -1;
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnHueApply()
{
	if (!DoCommand(_T("Hue"), _T("-2")))
		return;

	EnableControl(IDC_HUE, true);
	ShowControl(IDC_HUE_APPLY, SW_HIDE);
	ShowControl(IDC_HUE_CANCEL, SW_HIDE);

	m_HueSlider.SetPos(50);
	m_HueSlider.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnHueCancel()
{
	if (!DoCommand(_T("Hue"), _T("-3")))
		return;

	EnableControl(IDC_HUE, true);
	ShowControl(IDC_HUE_APPLY, SW_HIDE);
	ShowControl(IDC_HUE_CANCEL, SW_HIDE);

	m_HueSlider.SetPos(50);
	m_HueSlider.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnSaturation()
{
	if (!DoCommand(_T("Saturation"), _T("-1")))
		return;

	EnableControl(IDC_SATURATION, false);
	ShowControl(IDC_SATURATION_APPLY, SW_SHOW);
	ShowControl(IDC_SATURATION_CANCEL, SW_SHOW);

	m_SaturationSlider.SetPos(50);
	m_SaturationSlider.EnableWindow(true);
	m_iLastValue = -1;
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnSaturationApply()
{
	if (!DoCommand(_T("Saturation"), _T("-2")))
		return;

	EnableControl(IDC_SATURATION, true);
	ShowControl(IDC_SATURATION_APPLY, SW_HIDE);
	ShowControl(IDC_SATURATION_CANCEL, SW_HIDE);

	m_SaturationSlider.SetPos(50);
	m_SaturationSlider.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CColorsTab::OnSaturationCancel()
{
	if (!DoCommand(_T("Saturation"), _T("-3")))
		return;

	EnableControl(IDC_SATURATION, true);
	ShowControl(IDC_SATURATION_APPLY, SW_HIDE);
	ShowControl(IDC_SATURATION_CANCEL, SW_HIDE);

	m_SaturationSlider.SetPos(50);
	m_SaturationSlider.EnableWindow(false);
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
			if (m_iLastValue == iValue)
				break;

			m_iLastValue = iValue;
			CString strValue;
			strValue.Format(_T("%d"), iValue);

			if (pSliderCtrl == &m_AmountSlider)
				DoCommand(_T("HSLAmount"), strValue);
			else
			if (pSliderCtrl == &m_HueSlider)
				DoCommand(_T("Hue"), strValue);
			else
			if (pSliderCtrl == &m_SaturationSlider)
				DoCommand(_T("Saturation"), strValue);
			break;
		}
	}
}
