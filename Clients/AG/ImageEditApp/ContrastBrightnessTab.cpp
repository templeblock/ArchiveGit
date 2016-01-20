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
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_RESET, OnReset)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONTRAST_SLIDER, m_ContrastSlider);
	DDX_Control(pDX, IDC_BRIGHTNESS_SLIDER, m_BrightnessSlider);
}

/////////////////////////////////////////////////////////////////////////////
CContrastBrightnessTab::CContrastBrightnessTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CContrastBrightnessTab::IDD, pParent)
{
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

	EnableControl(IDC_APPLY, false);
	EnableControl(IDC_RESET, false);

	m_ContrastSlider.SetRange(0, 100);
	m_BrightnessSlider.SetRange(0, 100);

	m_ContrastSlider.SetTicFreq(10);
	m_BrightnessSlider.SetTicFreq(10);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnEnterTab()
{
	m_iContrast = 50;
	m_iBrightness = 50;

	m_ContrastSlider.SetPos(m_iContrast);
	m_BrightnessSlider.SetPos(m_iBrightness);

	bool bOK = DoCommand(_T("ContrastBrightnessTab"), Str(_T("%d"), EDIT_START));
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("ContrastBrightnessTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnApply()
{
	bool bOK = DoCommand(_T("ContrastBrightnessTab"), Str(_T("%d"), EDIT_APPLY));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnReset()
{
	bool bOK = DoCommand(_T("ContrastBrightnessTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CContrastBrightnessTab::OnAutoCorrect()
{
	bool bOK = DoCommand(_T("AutoCorrect"), _T(""));
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

			if (pSliderCtrl == &m_ContrastSlider)
			{
				if (m_iContrast == iValue)
					return;

				m_iContrast = iValue;
			}
			else
			if (pSliderCtrl == &m_BrightnessSlider)
			{
				if (m_iBrightness == iValue)
					return;

				m_iBrightness = iValue;
			}

			EnableControl(IDC_APPLY, true);
			EnableControl(IDC_RESET, true);

			iValue = MAKEWORD(m_iContrast, m_iBrightness);
			bool bOK = DoCommand(_T("ContrastBrightnessTab"), Str(_T("%d"), iValue));
			break;
		}
	}
}
