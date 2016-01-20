#include "stdafx.h"
#include "ContrastBrightnessTab.h"
#include "TabControl.h"
#include "MainDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CContrastBrightnessTab, CBaseTab)
BEGIN_MESSAGE_MAP(CContrastBrightnessTab, CBaseTab)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_AUTOCORRECT, OnAutoCorrect)
	ON_BN_CLICKED(IDC_CONTRAST, OnContrast)
	ON_BN_CLICKED(IDC_CONTRAST_APPLY, OnContrastApply)
	ON_BN_CLICKED(IDC_CONTRAST_CANCEL, OnContrastCancel)
	ON_BN_CLICKED(IDC_BRIGHTNESS, OnBrightness)
	ON_BN_CLICKED(IDC_BRIGHTNESS_APPLY, OnBrightnessApply)
	ON_BN_CLICKED(IDC_BRIGHTNESS_CANCEL, OnBrightnessCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CContrastBrightnessTab::CContrastBrightnessTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CContrastBrightnessTab::IDD, pParent)
{
	m_iLastValue = -1;
}

/////////////////////////////////////////////////////////////////////////////
CContrastBrightnessTab::~CContrastBrightnessTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CContrastBrightnessTab::OnInitDialog()
{
	// Initialize the resize dialog base class
	CBaseTab::OnInitDialog();

	EnableControl(IDC_CONTRAST, true);
	ShowControl(IDC_CONTRAST_APPLY, SW_HIDE);
	ShowControl(IDC_CONTRAST_CANCEL, SW_HIDE);

	m_ContrastSlider.EnableWindow(false);
	m_ContrastSlider.SetRange(0, 100);
	m_ContrastSlider.SetTicFreq(10);
	m_ContrastSlider.SetPos(50);

	EnableControl(IDC_BRIGHTNESS, true);
	ShowControl(IDC_BRIGHTNESS_APPLY, SW_HIDE);
	ShowControl(IDC_BRIGHTNESS_CANCEL, SW_HIDE);

	m_BrightnessSlider.EnableWindow(false);
	m_BrightnessSlider.SetRange(0, 100);
	m_BrightnessSlider.SetTicFreq(10);
	m_BrightnessSlider.SetPos(50);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONTRAST_SLIDER, m_ContrastSlider);
	DDX_Control(pDX, IDC_BRIGHTNESS_SLIDER, m_BrightnessSlider);
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnAutoCorrect()
{
	DoCommand(_T("AutoCorrect"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnContrast()
{
	if (!DoCommand(_T("Contrast"), _T("-1")))
		return;

	EnableControl(IDC_CONTRAST, false);
	ShowControl(IDC_CONTRAST_APPLY, SW_SHOW);
	ShowControl(IDC_CONTRAST_CANCEL, SW_SHOW);

	m_ContrastSlider.SetPos(50);
	m_ContrastSlider.EnableWindow(true);
	m_iLastValue = -1;
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnContrastApply()
{
	if (!DoCommand(_T("Contrast"), _T("-2")))
		return;

	EnableControl(IDC_CONTRAST, true);
	ShowControl(IDC_CONTRAST_APPLY, SW_HIDE);
	ShowControl(IDC_CONTRAST_CANCEL, SW_HIDE);

	m_ContrastSlider.SetPos(50);
	m_ContrastSlider.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnContrastCancel()
{
	if (!DoCommand(_T("Contrast"), _T("-3")))
		return;

	EnableControl(IDC_CONTRAST, true);
	ShowControl(IDC_CONTRAST_APPLY, SW_HIDE);
	ShowControl(IDC_CONTRAST_CANCEL, SW_HIDE);

	m_ContrastSlider.SetPos(50);
	m_ContrastSlider.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnBrightness()
{
	if (!DoCommand(_T("Brightness"), _T("-1")))
		return;

	EnableControl(IDC_BRIGHTNESS, false);
	ShowControl(IDC_BRIGHTNESS_APPLY, SW_SHOW);
	ShowControl(IDC_BRIGHTNESS_CANCEL, SW_SHOW);

	m_BrightnessSlider.SetPos(50);
	m_BrightnessSlider.EnableWindow(true);
	m_iLastValue = -1;
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnBrightnessApply()
{
	if (!DoCommand(_T("Brightness"), _T("-2")))
		return;

	EnableControl(IDC_BRIGHTNESS, true);
	ShowControl(IDC_BRIGHTNESS_APPLY, SW_HIDE);
	ShowControl(IDC_BRIGHTNESS_CANCEL, SW_HIDE);

	m_BrightnessSlider.SetPos(50);
	m_BrightnessSlider.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnBrightnessCancel()
{
	if (!DoCommand(_T("Brightness"), _T("-3")))
		return;

	EnableControl(IDC_BRIGHTNESS, true);
	ShowControl(IDC_BRIGHTNESS_APPLY, SW_HIDE);
	ShowControl(IDC_BRIGHTNESS_CANCEL, SW_HIDE);

	m_BrightnessSlider.SetPos(50);
	m_BrightnessSlider.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

			if (pSliderCtrl == &m_ContrastSlider)
				DoCommand(_T("Contrast"), strValue);
			else
			if (pSliderCtrl == &m_BrightnessSlider)
				DoCommand(_T("Brightness"), strValue);
			break;
		}
	}
}
