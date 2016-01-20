#include "stdafx.h"
#include "CropTab.h"
#include "TabControl.h"
#include "MainDlg.h"
#include "Utility.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

const static tagSIZE g_Sizes[] = 
{ // Always list the minimum size first (as in portrait)
	{0, 0},
	{3, 4},
	{3, 5},
	{4, 6},
	{5, 7},
	{8, 10},
};

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCropTab, CBaseTab)
BEGIN_MESSAGE_MAP(CCropTab, CBaseTab)
	ON_BN_CLICKED(IDC_CROP_PORTRAIT, OnCropPortrait)
	ON_BN_CLICKED(IDC_CROP_LANDSCAPE, OnCropLandscape)
	ON_CBN_SELCHANGE(IDC_CROP_ASPECTRATIO, OnCropAspectRatio)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CCropTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CROP_ASPECTRATIO, m_comboAspectRatio);
	DDX_Control(pDX, IDC_CROP_LEFT, m_editLeft);
	DDX_Control(pDX, IDC_CROP_TOP, m_editTop);
	DDX_Control(pDX, IDC_CROP_RIGHT, m_editRight);
	DDX_Control(pDX, IDC_CROP_BOTTOM, m_editBottom);
	DDX_Control(pDX, IDC_CROP_PORTRAIT, m_radioPortrait);
	DDX_Control(pDX, IDC_CROP_LANDSCAPE, m_radioLandscape);
	DDX_Control(pDX, IDC_CROP_ORIGINAL, m_staticOriginalDimensions);
	DDX_Control(pDX, IDC_CROP_NEW, m_staticNewDimensions);
}

/////////////////////////////////////////////////////////////////////////////
CCropTab::CCropTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CCropTab::IDD, pParent)
{
}

/////////////////////////////////////////////////////////////////////////////
CCropTab::~CCropTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCropTab::OnInitDialog()
{
	// Initialize the resize dialog base class
	CBaseTab::OnInitDialog();

	m_comboAspectRatio.AddString(_T("Any"));
	int nSizes = sizeof(g_Sizes) / sizeof(CSize);
	for (int i=1; i<nSizes; i++)
		m_comboAspectRatio.AddString(Str(_T("%d x %d"), g_Sizes[i].cx, g_Sizes[i].cy));

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnEnterTab()
{
	long lWidth = GetCommand(_T("GetWidth"));
	long lHeight = GetCommand(_T("GetHeight"));
	bool bPortrait = (lWidth <= lHeight);

	m_SizeImage.cx = lWidth;
	m_SizeImage.cy = lHeight;

	int iCurSel = 0;
	m_comboAspectRatio.SetCurSel(iCurSel);
	m_radioPortrait.EnableWindow(iCurSel != 0);
	m_radioLandscape.EnableWindow(iCurSel != 0);
	m_radioPortrait.SetCheck(bPortrait ? BST_CHECKED : BST_UNCHECKED);
	m_radioLandscape.SetCheck(!bPortrait ? BST_CHECKED : BST_UNCHECKED);
	m_staticOriginalDimensions.SetWindowText(Str(_T("%d x %d"), lWidth, lHeight));
	UpdateDimensions();

	bool bOK = DoCommand(_T("CropTab"), Str(_T("%d"), EDIT_START));
	SetTimer(123, 1000, NULL);
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnLeaveTab()
{
	KillTimer(123);
	bool bOK = DoCommand(_T("CropTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnApply()
{
	bool bOK = DoCommand(_T("CropTab"), Str(_T("%d"), EDIT_APPLY));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnReset()
{
	bool bOK = DoCommand(_T("CropTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnCropPortrait()
{
	m_radioPortrait.SetCheck(BST_CHECKED);
	m_radioLandscape.SetCheck(BST_UNCHECKED);
	SetCropAspect();
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnCropLandscape()
{
	m_radioPortrait.SetCheck(BST_UNCHECKED);
	m_radioLandscape.SetCheck(BST_CHECKED);
	SetCropAspect();
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnCropAspectRatio()
{
	int iCurSel = m_comboAspectRatio.GetCurSel();
	m_radioPortrait.EnableWindow(iCurSel != 0);
	m_radioLandscape.EnableWindow(iCurSel != 0);
	SetCropAspect();
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnTimer(UINT_PTR idTimer)
{
	UpdateDimensions();
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::SetCropAspect()
{
	int iCurSel = m_comboAspectRatio.GetCurSel();
	if (iCurSel < 0) iCurSel = 0;
	CSize Size = g_Sizes[iCurSel];
	if (!Size.cx && !Size.cy)
		return;
	bool bPortrait = (m_radioPortrait.GetCheck() == BST_CHECKED);
	if (!bPortrait)
		SWAP(Size.cx, Size.cy);
	bool bOK = DoCommand(_T("CropTab"), Str(_T("%d"), MAKELONG(Size.cx, Size.cy)));
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::UpdateDimensions()
{
//CStringArray arry;
//func::split("-", "a-b-c", arry);
//for (int i = 0; i < arry.GetSize(); i++) {
//MessageBox(NULL, arry[i], NULL, MB_OK); } 

	long l = GetCommand(_T("GetCropLeft"));
	long t = GetCommand(_T("GetCropTop"));
	long r = GetCommand(_T("GetCropRight"));
	long b = GetCommand(_T("GetCropBottom"));
	CRect CropRect(l, t, r, b);
	if (CropRect.EqualRect(m_CropRect))
		return;

	m_CropRect = CropRect;
	m_editLeft.SetWindowText(Str(_T("%d"), m_CropRect.left));
	m_editTop.SetWindowText(Str(_T("%d"), m_CropRect.top));
	m_editRight.SetWindowText(Str(_T("%d"), m_CropRect.right));
	m_editBottom.SetWindowText(Str(_T("%d"), m_CropRect.bottom));
	m_staticNewDimensions.SetWindowText(Str(_T("%d x %d"), m_CropRect.Width(), m_CropRect.Height()));
}
