#include "stdafx.h"
#include "EffectsTab.h"
#include "TabControl.h"
#include "MainDlg.h"
#include "Utility.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CEffectsTab, CBaseTab)
BEGIN_MESSAGE_MAP(CEffectsTab, CBaseTab)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_SHARPEN, OnSharpen)
	ON_BN_CLICKED(IDC_SHARPEN_APPLY, OnSharpenApply)
	ON_BN_CLICKED(IDC_SHARPEN_CANCEL, OnSharpenCancel)
	ON_BN_CLICKED(IDC_SMOOTH, OnSmooth)
	ON_BN_CLICKED(IDC_SMOOTH_APPLY, OnSmoothApply)
	ON_BN_CLICKED(IDC_SMOOTH_CANCEL, OnSmoothCancel)
	ON_BN_CLICKED(IDC_EDGES, OnEdges)
	ON_BN_CLICKED(IDC_EDGES_APPLY, OnEdgesApply)
	ON_BN_CLICKED(IDC_EDGES_CANCEL, OnEdgesCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHARPEN_SLIDER, m_SharpenSlider);
	DDX_Control(pDX, IDC_SMOOTH_SLIDER, m_SmoothSlider);
	DDX_Control(pDX, IDC_EDGES_SLIDER, m_EdgesSlider);
}

/////////////////////////////////////////////////////////////////////////////
CEffectsTab::CEffectsTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CEffectsTab::IDD, pParent)
{
	m_iLastValue = -1;
}

/////////////////////////////////////////////////////////////////////////////
CEffectsTab::~CEffectsTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CEffectsTab::OnInitDialog()
{
	// Initialize the resize dialog base class
	CBaseTab::OnInitDialog();

	EnableControl(IDC_SHARPEN, true);
	ShowControl(IDC_SHARPEN_APPLY, SW_HIDE);
	ShowControl(IDC_SHARPEN_CANCEL, SW_HIDE);

	m_SharpenSlider.EnableWindow(false);
	m_SharpenSlider.SetRange(0, 100);
	m_SharpenSlider.SetTicFreq(10);
	m_SharpenSlider.SetPos(0);

	EnableControl(IDC_SMOOTH, true);
	ShowControl(IDC_SMOOTH_APPLY, SW_HIDE);
	ShowControl(IDC_SMOOTH_CANCEL, SW_HIDE);

	m_SmoothSlider.EnableWindow(false);
	m_SmoothSlider.SetRange(0, 100);
	m_SmoothSlider.SetTicFreq(10);
	m_SmoothSlider.SetPos(0);

	EnableControl(IDC_EDGES, true);
	ShowControl(IDC_EDGES_APPLY, SW_HIDE);
	ShowControl(IDC_EDGES_CANCEL, SW_HIDE);

	m_EdgesSlider.EnableWindow(false);
	m_EdgesSlider.SetRange(0, 100);
	m_EdgesSlider.SetTicFreq(10);
	m_EdgesSlider.SetPos(0);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnEnterTab()
{
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnLeaveTab()
{
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnSharpen()
{
	if (!DoCommand(_T("Sharpen"), Str(_T("%d"), EDIT_START)))
		return;

	EnableControl(IDC_SHARPEN, false);
	ShowControl(IDC_SHARPEN_APPLY, SW_SHOW);
	ShowControl(IDC_SHARPEN_CANCEL, SW_SHOW);

	m_SharpenSlider.SetPos(0);
	m_SharpenSlider.EnableWindow(true);
	m_iLastValue = -1;
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnSharpenApply()
{
	if (!DoCommand(_T("Sharpen"), Str(_T("%d"), EDIT_APPLY)))
		return;

	EnableControl(IDC_SHARPEN, true);
	ShowControl(IDC_SHARPEN_APPLY, SW_HIDE);
	ShowControl(IDC_SHARPEN_CANCEL, SW_HIDE);

	m_SharpenSlider.SetPos(0);
	m_SharpenSlider.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnSharpenCancel()
{
	if (!DoCommand(_T("Sharpen"), Str(_T("%d"), EDIT_RESET)))
		return;

	EnableControl(IDC_SHARPEN, true);
	ShowControl(IDC_SHARPEN_APPLY, SW_HIDE);
	ShowControl(IDC_SHARPEN_CANCEL, SW_HIDE);

	m_SharpenSlider.SetPos(0);
	m_SharpenSlider.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnSmooth()
{
	if (!DoCommand(_T("Smooth"), Str(_T("%d"), EDIT_START)))
		return;

	EnableControl(IDC_SMOOTH, false);
	ShowControl(IDC_SMOOTH_APPLY, SW_SHOW);
	ShowControl(IDC_SMOOTH_CANCEL, SW_SHOW);

	m_SmoothSlider.SetPos(0);
	m_SmoothSlider.EnableWindow(true);
	m_iLastValue = -1;
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnSmoothApply()
{
	if (!DoCommand(_T("Smooth"), Str(_T("%d"), EDIT_APPLY)))
		return;

	EnableControl(IDC_SMOOTH, true);
	ShowControl(IDC_SMOOTH_APPLY, SW_HIDE);
	ShowControl(IDC_SMOOTH_CANCEL, SW_HIDE);

	m_SmoothSlider.SetPos(0);
	m_SmoothSlider.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnSmoothCancel()
{
	if (!DoCommand(_T("Smooth"), Str(_T("%d"), EDIT_RESET)))
		return;

	EnableControl(IDC_SMOOTH, true);
	ShowControl(IDC_SMOOTH_APPLY, SW_HIDE);
	ShowControl(IDC_SMOOTH_CANCEL, SW_HIDE);

	m_SmoothSlider.SetPos(0);
	m_SmoothSlider.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnEdges()
{
	if (!DoCommand(_T("Edges"), Str(_T("%d"), EDIT_START)))
		return;

	EnableControl(IDC_EDGES, false);
	ShowControl(IDC_EDGES_APPLY, SW_SHOW);
	ShowControl(IDC_EDGES_CANCEL, SW_SHOW);

	m_EdgesSlider.SetPos(0);
	m_EdgesSlider.EnableWindow(true);
	m_iLastValue = -1;
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnEdgesApply()
{
	if (!DoCommand(_T("Edges"), Str(_T("%d"), EDIT_APPLY)))
		return;

	EnableControl(IDC_EDGES, true);
	ShowControl(IDC_EDGES_APPLY, SW_HIDE);
	ShowControl(IDC_EDGES_CANCEL, SW_HIDE);

	m_EdgesSlider.SetPos(0);
	m_EdgesSlider.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnEdgesCancel()
{
	if (!DoCommand(_T("Edges"), Str(_T("%d"), EDIT_RESET)))
		return;

	EnableControl(IDC_EDGES, true);
	ShowControl(IDC_EDGES_APPLY, SW_HIDE);
	ShowControl(IDC_EDGES_CANCEL, SW_HIDE);

	m_EdgesSlider.SetPos(0);
	m_EdgesSlider.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CEffectsTab::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

			if (pSliderCtrl == &m_SharpenSlider)
				DoCommand(_T("Sharpen"), strValue);
			else
			if (pSliderCtrl == &m_SmoothSlider)
				DoCommand(_T("Smooth"), strValue);
			else
			if (pSliderCtrl == &m_EdgesSlider)
				DoCommand(_T("Edges"), strValue);
			break;
		}
	}
}
