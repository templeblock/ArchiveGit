#include "stdafx.h"
#include "BordersTab.h"
#include "TabControl.h"
#include "MainDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CBordersTab, CBaseTab)
BEGIN_MESSAGE_MAP(CBordersTab, CBaseTab)
	ON_BN_CLICKED(IDC_BORDER_NONE, OnBorderNone)
	ON_BN_CLICKED(IDC_BORDER_IMAGE, OnBorderImage)
	ON_BN_CLICKED(IDC_BORDER_FADE, OnBorderFade)
	ON_BN_CLICKED(IDC_BORDER_IMAGE_CHOOSE, OnBorderImageChoose)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CBordersTab::CBordersTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CBordersTab::IDD, pParent)
{
}

/////////////////////////////////////////////////////////////////////////////
CBordersTab::~CBordersTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBordersTab::OnInitDialog()
{
	// Initialize the resize dialog base class
	CBaseTab::OnInitDialog();

	CheckRadioButton(IDC_BORDER_NONE, IDC_BORDER_FADE, IDC_BORDER_NONE);

	m_btnChoose.SetFont(_T("Arial"), 12, FW_NORMAL, false/*bItalic*/);
	m_btnChoose.UnderlineAlways(false);
	m_btnChoose.IsLink(false);
	m_btnChoose.SetColors(RGB(0,0,255)/*crLinkUp*/, RGB(0,0,0)/*crLinkHover*/, RGB(100,100,100)/*crLinkDown*/, GetSysColor(COLOR_BTNFACE)/*crBackGround*/);
//j	m_btnChoose.EnableWindow(false);

	m_editBorderImageFileName.SetWindowText(_T("<Image file name>"));
	m_editBorderImageFileName.EnableWindow(false);

	m_comboFadedEdgeShape.AddString(_T("Rectangle"));
	m_comboFadedEdgeShape.AddString(_T("Oval"));
	m_comboFadedEdgeShape.AddString(_T("Diamond"));
	m_comboFadedEdgeShape.AddString(_T("Heart"));
	m_comboFadedEdgeShape.AddString(_T("Star"));
	m_comboFadedEdgeShape.SetCurSel(0);
	m_comboFadedEdgeShape.EnableWindow(false);

	m_comboFadedEdgeDirection.AddString(_T("Center"));
	m_comboFadedEdgeDirection.AddString(_T("Left"));
	m_comboFadedEdgeDirection.AddString(_T("Top"));
	m_comboFadedEdgeDirection.AddString(_T("Right"));
	m_comboFadedEdgeDirection.AddString(_T("Bottom"));
	m_comboFadedEdgeDirection.SetCurSel(0);
	m_comboFadedEdgeDirection.EnableWindow(false);

	CString str;

	str.Format(_T("%d"), 100);
	m_editFadedEdgeWidth.SetWindowText(str);
	m_editFadedEdgeWidth.EnableWindow(false);

	str.Format(_T("%d"), 0);
	m_editFadedEdgeOpacity.SetWindowText(str);
	m_editFadedEdgeOpacity.EnableWindow(false);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BORDER_IMAGE_CHOOSE, m_btnChoose);
	DDX_Control(pDX, IDC_BORDER_IMAGE_FILENAME, m_editBorderImageFileName);
	DDX_Control(pDX, IDC_BORDER_FADE_SHAPE, m_comboFadedEdgeShape);
	DDX_Control(pDX, IDC_BORDER_FADE_DIRECTION, m_comboFadedEdgeDirection);
	DDX_Control(pDX, IDC_BORDER_FADE_WIDTH, m_editFadedEdgeWidth);
	DDX_Control(pDX, IDC_BORDER_FADE_OPACITY, m_editFadedEdgeOpacity);
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnBorderNone()
{
	CheckRadioButton(IDC_BORDER_NONE, IDC_BORDER_FADE, IDC_BORDER_NONE);

//j	m_btnChoose.EnableWindow(false);
	m_editBorderImageFileName.EnableWindow(false);
	m_comboFadedEdgeShape.EnableWindow(false);
	m_comboFadedEdgeDirection.EnableWindow(false);
	m_editFadedEdgeWidth.EnableWindow(false);
	m_editFadedEdgeOpacity.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnBorderImage()
{
	CheckRadioButton(IDC_BORDER_NONE, IDC_BORDER_FADE, IDC_BORDER_IMAGE);

//j	m_btnChoose.EnableWindow(true);
	m_editBorderImageFileName.EnableWindow(true);
	m_comboFadedEdgeShape.EnableWindow(false);
	m_comboFadedEdgeDirection.EnableWindow(false);
	m_editFadedEdgeWidth.EnableWindow(false);
	m_editFadedEdgeOpacity.EnableWindow(false);
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnBorderFade()
{
	CheckRadioButton(IDC_BORDER_NONE, IDC_BORDER_FADE, IDC_BORDER_FADE);

//j	m_btnChoose.EnableWindow(false);
	m_editBorderImageFileName.EnableWindow(false);
	m_comboFadedEdgeShape.EnableWindow(true);
	m_comboFadedEdgeDirection.EnableWindow(true);
	m_editFadedEdgeWidth.EnableWindow(true);
	m_editFadedEdgeOpacity.EnableWindow(true);
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnBorderImageChoose()
{
	DoCommand(_T("OpenBorderImage"), _T(""));
}
