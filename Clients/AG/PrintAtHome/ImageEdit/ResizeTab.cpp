#include "stdafx.h"
#include "ResizeTab.h"
#include "TabControl.h"
#include "MainDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CResizeTab, CBaseTab)
BEGIN_MESSAGE_MAP(CResizeTab, CBaseTab)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_RESIZE_ORIGINAL, OnResizeOriginal)
	ON_BN_CLICKED(IDC_RESIZE_PREDEFINED, OnResizePredefine)
	ON_BN_CLICKED(IDC_RESIZE_CUSTOM, OnResizeCustom)
	ON_BN_CLICKED(IDC_RESIZE_PERCENTAGE, OnResizePercentage)
	ON_BN_CLICKED(IDC_RESIZE, OnResize)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CResizeTab::CResizeTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CResizeTab::IDD, pParent)
{
}

/////////////////////////////////////////////////////////////////////////////
CResizeTab::~CResizeTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CResizeTab::OnInitDialog()
{
	// Initialize the resize dialog base class
	CBaseTab::OnInitDialog();

	CheckRadioButton(IDC_RESIZE_ORIGINAL, IDC_RESIZE_PERCENTAGE, IDC_RESIZE_ORIGINAL);

	m_comboPredefinedSizes.AddString(_T("Document large (1024 x 768 px)"));
	m_comboPredefinedSizes.AddString(_T("Document small (800 x 600 px)"));
	m_comboPredefinedSizes.AddString(_T("Web large (640 x 480 px)"));
	m_comboPredefinedSizes.AddString(_T("Web small (440 x 336 px)"));
	m_comboPredefinedSizes.AddString(_T("Email large (314 x 235 px)"));
	m_comboPredefinedSizes.AddString(_T("Email small (160 x0 160 px)"));
	m_comboPredefinedSizes.SetCurSel(0);
	m_comboPredefinedSizes.EnableWindow(false);

//	CImageControl ImageControl = GetImageControl();

	CString str;

	long lWidth = GetCommand(_T("GetWidth"));
	str.Format(_T("%d"), lWidth);
	m_editCustomWidth.SetWindowText(str);
	m_editCustomWidth.EnableWindow(false);

	long lHeight = GetCommand(_T("GetHeight"));
	str.Format(_T("%d"), lHeight);
	m_editCustomHeight.SetWindowText(str);
	m_editCustomHeight.EnableWindow(false);

	str.Format(_T("%d"), 100);
	m_editPercentage.SetWindowText(str);
	m_editPercentage.EnableWindow(false);

	m_btnResize.EnableWindow(false);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESIZE_CUSTOM_WIDTH, m_editCustomWidth);
	DDX_Control(pDX, IDC_RESIZE_CUSTOM_HEIGHT, m_editCustomHeight);
	DDX_Control(pDX, IDC_RESIZE_PERCENTAGE_EDIT, m_editPercentage);
	DDX_Control(pDX, IDC_RESIZE_PREDEFINED_COMBO, m_comboPredefinedSizes);
	DDX_Control(pDX, IDC_RESIZE, m_btnResize);
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResize()
{
	if (!DoCommand(_T("Resize"), _T("-1")))
		return;

	EnableControl(IDC_RESIZE, true);
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizeOriginal()
{
	CheckRadioButton(IDC_RESIZE_ORIGINAL, IDC_RESIZE_PERCENTAGE, IDC_RESIZE_ORIGINAL);

	m_comboPredefinedSizes.EnableWindow(false);
	m_editCustomWidth.EnableWindow(false);
	m_editCustomHeight.EnableWindow(false);
	m_editPercentage.EnableWindow(false);
	m_btnResize.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizePredefine()
{
	CheckRadioButton(IDC_RESIZE_ORIGINAL, IDC_RESIZE_PERCENTAGE, IDC_RESIZE_PREDEFINED);

	m_comboPredefinedSizes.EnableWindow(true);
	m_editCustomWidth.EnableWindow(false);
	m_editCustomHeight.EnableWindow(false);
	m_editPercentage.EnableWindow(false);
	m_btnResize.EnableWindow(true);
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizeCustom()
{
	CheckRadioButton(IDC_RESIZE_ORIGINAL, IDC_RESIZE_PERCENTAGE, IDC_RESIZE_CUSTOM);

	m_comboPredefinedSizes.EnableWindow(false);
	m_editCustomWidth.EnableWindow(true);
	m_editCustomHeight.EnableWindow(true);
	m_editPercentage.EnableWindow(false);
	m_btnResize.EnableWindow(true);
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizePercentage()
{
	CheckRadioButton(IDC_RESIZE_ORIGINAL, IDC_RESIZE_PERCENTAGE, IDC_RESIZE_PERCENTAGE);

	m_comboPredefinedSizes.EnableWindow(false);
	m_editCustomWidth.EnableWindow(false);
	m_editCustomHeight.EnableWindow(false);
	m_editPercentage.EnableWindow(true);
	m_btnResize.EnableWindow(true);
}
