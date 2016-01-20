#include "stdafx.h"
#include "BordersTab.h"
#include "TabControl.h"
#include "MainDlg.h"
#include "Image.h"

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
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_RESET, OnReset)
END_MESSAGE_MAP()

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

	m_btnChoose.SetFont(_T("Arial"), 12, FW_NORMAL, false/*bItalic*/);
	m_btnChoose.UnderlineAlways(false);
	m_btnChoose.IsLink(false);
	m_btnChoose.SetColors(RGB(0,0,255)/*crLinkUp*/, RGB(0,0,0)/*crLinkHover*/, RGB(100,100,100)/*crLinkDown*/, GetSysColor(COLOR_BTNFACE)/*crBackGround*/);

	m_comboFadedEdgeShape.AddString(_T("Rectangle"));
	m_comboFadedEdgeShape.AddString(_T("Oval"));
	m_comboFadedEdgeShape.AddString(_T("Diamond"));
	m_comboFadedEdgeShape.AddString(_T("Heart"));
	m_comboFadedEdgeShape.AddString(_T("Star"));

	m_comboFadedEdgeDirection.AddString(_T("Center"));
	m_comboFadedEdgeDirection.AddString(_T("Left"));
	m_comboFadedEdgeDirection.AddString(_T("Top"));
	m_comboFadedEdgeDirection.AddString(_T("Right"));
	m_comboFadedEdgeDirection.AddString(_T("Bottom"));

	m_editFadedEdgeWidth.SetWindowText(Str(_T("%d"), 100));
	m_editFadedEdgeOpacity.SetWindowText(Str(_T("%d"), 0));

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnEnterTab()
{
	OnBorderNone();
	m_comboFadedEdgeShape.SetCurSel(0);
	m_comboFadedEdgeDirection.SetCurSel(0);
	m_editBorderImageFileName.SetWindowText(_T("<Image file name>"));
	bool bOK = DoCommand(_T("BordersTab"), Str(_T("%d"), EDIT_START));
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("BordersTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnApply()
{
	bool bOK = DoCommand(_T("BordersTab"), Str(_T("%d"), EDIT_APPLY));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnReset()
{
	bool bOK = DoCommand(_T("BordersTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
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

	DoCommand(_T("DeactivateBorderImage"), _T(""));
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

	DoCommand(_T("ActivateBorderImage"), _T(""));
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
	#define FILTER_WITH_GDIPLUS    _T("All Supported Files (*.bmp, *.gif, *.jpg, *.tif, *.png, *.wmf, *.ico, *.emf)|*.bmp;*.gif;*.jpg;*.tif*;*.png;*.wmf;*.ico;*.emf|Bitmap Files (*.bmp)|*.bmp|GIF Files (*.gif)|*.gif|JPG Files (*.jpg)|*.jpg|TIF Files (*.tif)|*.tif*|PNG Files (*.png)|*.png|Windows Metafiles Files (*.wmf)|*.wmf|Windows Enhanced Metafiles Files (*.emf)|*.emf|Icon Files (*.ico)|*.ico|All Files (*.*)|*.*||")
	#define FILTER_WITHOUT_GDIPLUS _T("All Supported Files (*.bmp, *.gif, *.jpg, *.png, *.wmf, *.ico, *.emf)|*.bmp;*.gif;*.jpg;*.png;*.wmf;*.ico;*.emf|Bitmap Files (*.bmp)|*.bmp|GIF Files (*.gif)|*.gif|JPG Files (*.jpg)|*.jpg|PNG Files (*.png)|*.png|Windows Metafiles Files (*.wmf)|*.wmf|Windows Enhanced Metafiles Files (*.emf)|*.emf|Icon Files (*.ico)|*.ico|All Files (*.*)|*.*||")
	LPCTSTR pstrFilter = (CImage::IsGdiPlusInstalled() ? FILTER_WITH_GDIPLUS : FILTER_WITHOUT_GDIPLUS);

	CFileDialog FileDialog(true, _T("jpg"), NULL/*pszFileName*/, 
		OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
		pstrFilter, this/*hWndParent*/);

	FileDialog.m_ofn.lpstrTitle = _T("Open an Image File");
	if (FileDialog.DoModal() == IDCANCEL)
		return;

	CString strFileName = FileDialog.m_ofn.lpstrFile; //j FileDialog.GetPathName();

	// Save the path in the registry
//j	if (regkey.m_hKey)
//j		bool bOK = (regkey.SetStringValue(REGVAL_MRU_PICTURE_PATH, StrPath(szFileName)) == ERROR_SUCCESS);

	CString strName = FileDialog.m_ofn.lpstrFileTitle;
	int iDot = strName.ReverseFind('.');
	if (iDot >= 0)
		strName = strName.Left(iDot);
	m_editBorderImageFileName.SetWindowText(strName);

	bool bOK = DoCommand(_T("OpenBorderImage"), strFileName);
	if (bOK)
		OnBorderImage();
}
