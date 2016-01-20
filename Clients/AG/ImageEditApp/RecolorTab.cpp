#include "stdafx.h"
#include "RecolorTab.h"
#include "TabControl.h"
#include "MainDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CRecolorTab, CBaseTab)
BEGIN_MESSAGE_MAP(CRecolorTab, CBaseTab)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_RECOLOR_SOLID, OnRecolorSolid)
	ON_BN_CLICKED(IDC_RECOLOR_SWEEP_DOWN, OnRecolorSweepDown)
	ON_BN_CLICKED(IDC_RECOLOR_SWEEP_RIGHT, OnRecolorSweepRight)
	ON_BN_CLICKED(IDC_RECOLOR_SWEEP_CENTER, OnRecolorSweepFromCenter)
	ON_BN_CLICKED(IDC_RECOLOR_PRIMARY, OnRecolorPrimary)
	ON_BN_CLICKED(IDC_RECOLOR_SECONDARY, OnRecolorSecondary)
	ON_BN_CLICKED(IDC_RECOLOR_PRIMARY_PATCH, OnRecolorPrimaryPatch)
	ON_BN_CLICKED(IDC_RECOLOR_SECONDARY_PATCH, OnRecolorSecondaryPatch)
	ON_BN_DOUBLECLICKED(IDC_RECOLOR_PRIMARY_PATCH, OnRecolorPrimaryPatchDoubleClick)
	ON_BN_DOUBLECLICKED(IDC_RECOLOR_SECONDARY_PATCH, OnRecolorSecondaryPatchDoubleClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RECOLOR_PRIMARY, m_btnPrimary);
	DDX_Control(pDX, IDC_RECOLOR_SECONDARY, m_btnSecondary);
	DDX_Control(pDX, IDC_RECOLOR_PRIMARY_PATCH, m_btnPrimaryPatch);
	DDX_Control(pDX, IDC_RECOLOR_SECONDARY_PATCH, m_btnSecondaryPatch);
	DDX_Control(pDX, IDC_TOLERANCE, m_ToleranceSlider);
	DDX_Control(pDX, IDC_CURRENT_TOLERANCE, m_staticTolerance);
}

/////////////////////////////////////////////////////////////////////////////
CRecolorTab::CRecolorTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CRecolorTab::IDD, pParent)
{
	m_PrimaryColor = RGB(255,255,255);
	m_SecondaryColor = RGB(255,255,255);
}

/////////////////////////////////////////////////////////////////////////////
CRecolorTab::~CRecolorTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRecolorTab::OnInitDialog()
{
	// Initialize the resize dialog base class
	CBaseTab::OnInitDialog();

	CheckRadioButton(IDC_RECOLOR_SOLID, IDC_RECOLOR_SWEEP_CENTER, IDC_RECOLOR_SOLID);

	m_btnPrimary.GetWindowText(m_strPrimaryText);
	m_btnSecondary.GetWindowText(m_strSecondaryText);

	m_btnPrimary.SetCheck(BST_CHECKED);
	m_btnSecondary.SetCheck(BST_UNCHECKED);

	m_btnPrimaryPatch.SetColor(m_PrimaryColor);
	m_btnSecondaryPatch.SetColor(m_SecondaryColor);

	m_iTolerance = 0;

	m_ToleranceSlider.SetRange(0, 100);
	m_ToleranceSlider.SetTicFreq(10);
	m_ToleranceSlider.SetPos(m_iTolerance);

	UpdateData(true);
	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnEnterTab()
{
	int nFillSelection = GetCheckedRadioButton(IDC_RECOLOR_SOLID, IDC_RECOLOR_SWEEP_CENTER);
	switch (nFillSelection)
	{
		case IDC_RECOLOR_SOLID:
			DoCommand(_T("Recolor"), _T("SolidFill"));
			break;
		case IDC_RECOLOR_SWEEP_DOWN:
			DoCommand(_T("Recolor"), _T("SweepDown"));
			break;
		case IDC_RECOLOR_SWEEP_RIGHT:
			DoCommand(_T("Recolor"), _T("SweepRight"));
			break;
		case IDC_RECOLOR_SWEEP_CENTER:
			DoCommand(_T("Recolor"), _T("SweepCenter"));
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("RecolorTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSolid()
{
	CheckRadioButton(IDC_RECOLOR_SOLID, IDC_RECOLOR_SWEEP_CENTER, IDC_RECOLOR_SOLID);
	DoCommand(_T("Recolor"), _T("SolidFill"));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSweepDown()
{
	CheckRadioButton(IDC_RECOLOR_SOLID, IDC_RECOLOR_SWEEP_CENTER, IDC_RECOLOR_SWEEP_DOWN);
	DoCommand(_T("Recolor"), _T("SweepDown"));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSweepRight()
{
	CheckRadioButton(IDC_RECOLOR_SOLID, IDC_RECOLOR_SWEEP_CENTER, IDC_RECOLOR_SWEEP_RIGHT);
	DoCommand(_T("Recolor"), _T("SweepRight"));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSweepFromCenter()
{
	CheckRadioButton(IDC_RECOLOR_SOLID, IDC_RECOLOR_SWEEP_CENTER, IDC_RECOLOR_SWEEP_CENTER);
	DoCommand(_T("Recolor"), _T("SweepCenter"));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorPrimary()
{
	m_btnPrimary.SetCheck(BST_CHECKED);
	m_btnSecondary.SetCheck(BST_UNCHECKED);
	m_btnPrimary.SetWindowText(m_strPrimaryText);
	m_btnSecondary.SetWindowText(m_strSecondaryText);
	DoCommand(_T("Recolor"), _T("SelectPrimaryColor"));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSecondary()
{
	m_btnPrimary.SetCheck(BST_UNCHECKED);
	m_btnSecondary.SetCheck(BST_CHECKED);
	m_btnPrimary.SetWindowText(m_strSecondaryText);
	m_btnSecondary.SetWindowText(m_strPrimaryText);
	DoCommand(_T("Recolor"), _T("SelectSecondaryColor"));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorPrimaryPatch()
{
	OnRecolorPrimary();
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSecondaryPatch()
{
	OnRecolorSecondary();
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorPrimaryPatchDoubleClick()
{
	if (!m_btnPrimaryPatch.PickColor())
		return;

	COLORREF Color = m_btnPrimaryPatch.GetColor();
	if (m_PrimaryColor == Color)
		return;

	m_PrimaryColor = Color;
	CString strColor;
	strColor.Format(_T("%d"), m_PrimaryColor);
	DoCommand(_T("SetPrimaryColor"), strColor);
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSecondaryPatchDoubleClick()
{
	if (!m_btnSecondaryPatch.PickColor())
		return;

	COLORREF Color = m_btnSecondaryPatch.GetColor();
	if (m_SecondaryColor == Color)
		return;

	m_SecondaryColor = Color;
	CString strColor;
	strColor.Format(_T("%d"), m_SecondaryColor);
	DoCommand(_T("SetSecondaryColor"), strColor);
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSliderCtrl = (CSliderCtrl*)pScrollBar;
	switch (nSBCode)
	{
		case TB_THUMBTRACK:
		case TB_ENDTRACK:
		{
			int iValue = pSliderCtrl->GetPos();

			if (pSliderCtrl == &m_ToleranceSlider)
			{
				if (m_iTolerance == iValue)
					return;

				m_iTolerance = iValue;

				CString strValue = Str(_T("%d"), m_iTolerance);
				m_staticTolerance.SetWindowText(strValue);
				DoCommand(_T("Tolerance"), strValue);
			}

			break;
		}
	}
}
