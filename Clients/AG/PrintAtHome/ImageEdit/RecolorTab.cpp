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
	ON_WM_ENABLE()
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
	CheckRadioButton(IDC_RECOLOR_PRIMARY, IDC_RECOLOR_SECONDARY, IDC_RECOLOR_PRIMARY);

	//SetDefaultFillState();
	m_btnPrimaryPatch.SetColor(m_PrimaryColor);
	m_btnSecondaryPatch.SetColor(m_SecondaryColor);

	UpdateData(true);
	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RECOLOR_PRIMARY_PATCH, m_btnPrimaryPatch);
	DDX_Control(pDX, IDC_RECOLOR_SECONDARY_PATCH, m_btnSecondaryPatch);
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSolid()
{
	CheckRadioButton(IDC_RECOLOR_SOLID, IDC_RECOLOR_SWEEP_CENTER, IDC_RECOLOR_SOLID);
	m_btnSecondaryPatch.EnableWindow(false);
	DoCommand(_T("SolidFill"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSweepDown()
{
	CheckRadioButton(IDC_RECOLOR_SOLID, IDC_RECOLOR_SWEEP_CENTER, IDC_RECOLOR_SWEEP_DOWN);
	m_btnSecondaryPatch.EnableWindow(true);
	DoCommand(_T("SweepDown"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSweepRight()
{
	CheckRadioButton(IDC_RECOLOR_SOLID, IDC_RECOLOR_SWEEP_CENTER, IDC_RECOLOR_SWEEP_RIGHT);
	m_btnSecondaryPatch.EnableWindow(true);
	DoCommand(_T("SweepRight"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSweepFromCenter()
{
	CheckRadioButton(IDC_RECOLOR_SOLID, IDC_RECOLOR_SWEEP_CENTER, IDC_RECOLOR_SWEEP_CENTER);
	m_btnSecondaryPatch.EnableWindow(true);
	DoCommand(_T("SweepCenter"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorPrimary()
{
	CheckRadioButton(IDC_RECOLOR_PRIMARY, IDC_RECOLOR_SECONDARY, IDC_RECOLOR_PRIMARY);
	m_btnSecondaryPatch.EnableWindow(true);
	DoCommand(_T("SelectColor"), _T("Primary"));
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnRecolorSecondary()
{
	CheckRadioButton(IDC_RECOLOR_PRIMARY, IDC_RECOLOR_SECONDARY, IDC_RECOLOR_SECONDARY);
	m_btnSecondaryPatch.EnableWindow(true);
	DoCommand(_T("SelectColor"), _T("Secondary"));
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
void CRecolorTab::SetDefaultFillState()
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_RECOLOR_SOLID);
	if (pButton != NULL)
	{
		DoCommand(_T("SolidFill"), _T(""));
		pButton->SetCheck(true);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CRecolorTab::OnEnable(BOOL bEnable)
{
	CBaseTab::OnEnable(bEnable);

	if (bEnable)
	{
		int nFillSelection = GetCheckedRadioButton(IDC_RECOLOR_SOLID, IDC_RECOLOR_SWEEP_CENTER);

		switch(nFillSelection)
		{
			case IDC_RECOLOR_SOLID:
				DoCommand(_T("SolidFill"), _T(""));
				break;
			case IDC_RECOLOR_SWEEP_DOWN:
				DoCommand(_T("SweepDown"), _T(""));
				break;
			case IDC_RECOLOR_SWEEP_RIGHT:
				DoCommand(_T("SweepRight"), _T(""));
				break;
			case IDC_RECOLOR_SWEEP_CENTER:
				DoCommand(_T("SweepCenter"), _T(""));
				break;
			default:
				DoCommand(_T("DisableFill"), _T(""));
				break;
		}
	}
	else
	{
		DoCommand(_T("DisableFill"), _T(""));
	}
}
