#include "stdafx.h"
#include "ResizeTab.h"
#include "TabControl.h"
#include "MainDlg.h"
#include "Utility.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

typedef struct
{
	LPCTSTR pName;
    LONG cx;
    LONG cy;
} CPredefined;

const static CPredefined g_Sizes[] = 
{ // Always list the maximum size first (as in landscape)
	{_T("Document large"),	1024, 768},
	{_T("Document small"),	800, 600},
	{_T("Web large"),		640, 480},
	{_T("Web small"),		440, 336},
	{_T("Email large"),		315, 235},
	{_T("Email small"),		160, 160},
};

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CResizeTab, CBaseTab)
BEGIN_MESSAGE_MAP(CResizeTab, CBaseTab)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_RESIZE_PREDEFINED_COMBO, OnResizePredefinedChanged)
	ON_BN_CLICKED(IDC_RESIZE_ORIGINAL, OnResizeOriginal)
	ON_BN_CLICKED(IDC_RESIZE_PREDEFINED, OnResizePredefined)
	ON_BN_CLICKED(IDC_RESIZE_CUSTOM, OnResizeCustom)
	ON_EN_CHANGE(IDC_RESIZE_CUSTOM_WIDTH, OnResizeCustomSizeChanged)
	ON_EN_CHANGE(IDC_RESIZE_CUSTOM_HEIGHT, OnResizeCustomSizeChanged)
	ON_BN_CLICKED(IDC_RESIZE_PERCENTAGE, OnResizePercentage)
	ON_EN_CHANGE(IDC_RESIZE_PERCENTAGE_EDIT, OnResizePercentageChanged)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_RESET, OnReset)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESIZE_ORIGINAL, m_radioOriginalSize);
	DDX_Control(pDX, IDC_RESIZE_PREDEFINED, m_radioPredefinedSize);
	DDX_Control(pDX, IDC_RESIZE_CUSTOM, m_radioCustomSize);
	DDX_Control(pDX, IDC_RESIZE_PERCENTAGE, m_radioPercetageofOriginal);
	DDX_Control(pDX, IDC_RESIZE_CUSTOM_WIDTH, m_editCustomWidth);
	DDX_Control(pDX, IDC_RESIZE_CUSTOM_HEIGHT, m_editCustomHeight);
	DDX_Control(pDX, IDC_RESIZE_PERCENTAGE_EDIT, m_editPercentage);
	DDX_Control(pDX, IDC_RESIZE_PREDEFINED_COMBO, m_comboPredefinedSizes);
	DDX_Control(pDX, IDC_RESIZE_ORIGINAL_DIMENSIONS, m_staticOriginalDimensions);
	DDX_Control(pDX, IDC_RESIZE_NEW_DIMENSIONS, m_staticNewDimensions);
	DDX_Control(pDX, IDC_APPLY, m_btnApply);
	DDX_Control(pDX, IDC_RESET, m_btnReset);
}

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

	int nSizes = sizeof(g_Sizes) / sizeof(CPredefined);
	for (int i=0; i<nSizes; i++)
		m_comboPredefinedSizes.AddString(Str(_T("%s (%d x %d)"), g_Sizes[i].pName, g_Sizes[i].cx, g_Sizes[i].cy));

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnEnterTab()
{
	long lWidth = GetCommand(_T("GetWidth"));
	long lHeight = GetCommand(_T("GetHeight"));

	m_SizeImage.cx = lWidth;
	m_SizeImage.cy = lHeight;
	m_SizeLimits = m_SizeImage;
	m_Size = m_SizeImage;

	m_radioOriginalSize.SetCheck(BST_CHECKED);
	m_radioPredefinedSize.SetCheck(BST_UNCHECKED);
	m_radioCustomSize.SetCheck(BST_UNCHECKED);
	m_radioPercetageofOriginal.SetCheck(BST_UNCHECKED);

	int iCurSel = 0;
	m_comboPredefinedSizes.SetCurSel(iCurSel);

	m_editCustomWidth.SetWindowText(Str(_T("%d"), lWidth));
	m_editCustomHeight.SetWindowText(Str(_T("%d"), lHeight));
	m_editPercentage.SetWindowText(Str(_T("%d"), 100));
	m_staticOriginalDimensions.SetWindowText(Str(_T("%d x %d"), lWidth, lHeight));

	OnResizeOriginal();

	bool bOK = DoCommand(_T("ResizeTab"), Str(_T("%d"), EDIT_START));
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("ResizeTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnApply()
{
	bool bOK = DoCommand(_T("ResizeApply"), Str(_T("%d,%d"), m_Size.cx, m_Size.cy));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnReset()
{
	bool bOK = DoCommand(_T("ResizeTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizeOriginal()
{
	m_radioOriginalSize.SetCheck(BST_CHECKED);
	m_radioPredefinedSize.SetCheck(BST_UNCHECKED);
	m_radioCustomSize.SetCheck(BST_UNCHECKED);
	m_radioPercetageofOriginal.SetCheck(BST_UNCHECKED);

	m_comboPredefinedSizes.EnableWindow(false);
	m_editCustomWidth.EnableWindow(false);
	m_editCustomHeight.EnableWindow(false);
	m_editPercentage.EnableWindow(false);

	m_SizeLimits = m_SizeImage;

	UpdateDimensions();
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizePredefined()
{
	m_radioOriginalSize.SetCheck(BST_UNCHECKED);
	m_radioPredefinedSize.SetCheck(BST_CHECKED);
	m_radioCustomSize.SetCheck(BST_UNCHECKED);
	m_radioPercetageofOriginal.SetCheck(BST_UNCHECKED);

	m_comboPredefinedSizes.EnableWindow(true);
	m_editCustomWidth.EnableWindow(false);
	m_editCustomHeight.EnableWindow(false);
	m_editPercentage.EnableWindow(false);

	OnResizePredefinedChanged();
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizePredefinedChanged()
{
	int iCurSel = m_comboPredefinedSizes.GetCurSel();
	if (iCurSel < 0) iCurSel = 0;
	CPredefined Size = g_Sizes[iCurSel];
	bool bLandscape = (m_SizeImage.cx > m_SizeImage.cy);
	if (!bLandscape)
		SWAP(Size.cx, Size.cy);

	m_SizeLimits.cx = Size.cx;
	m_SizeLimits.cy = Size.cy;

	UpdateDimensions();
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizeCustom()
{
	m_radioOriginalSize.SetCheck(BST_UNCHECKED);
	m_radioPredefinedSize.SetCheck(BST_UNCHECKED);
	m_radioCustomSize.SetCheck(BST_CHECKED);
	m_radioPercetageofOriginal.SetCheck(BST_UNCHECKED);

	m_comboPredefinedSizes.EnableWindow(false);
	m_editCustomWidth.EnableWindow(true);
	m_editCustomHeight.EnableWindow(true);
	m_editPercentage.EnableWindow(false);

	OnResizeCustomSizeChanged();
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizeCustomSizeChanged()
{
	CString strText;
	m_editCustomWidth.GetWindowText(strText);
	long lWidth = _ttol(strText);
	m_editCustomHeight.GetWindowText(strText);
	long lHeight = _ttol(strText);
	m_SizeLimits.cx = lWidth;
	m_SizeLimits.cy = lHeight;

	UpdateDimensions();
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizePercentage()
{
	m_radioOriginalSize.SetCheck(BST_UNCHECKED);
	m_radioPredefinedSize.SetCheck(BST_UNCHECKED);
	m_radioCustomSize.SetCheck(BST_UNCHECKED);
	m_radioPercetageofOriginal.SetCheck(BST_CHECKED);

	m_comboPredefinedSizes.EnableWindow(false);
	m_editCustomWidth.EnableWindow(false);
	m_editCustomHeight.EnableWindow(false);
	m_editPercentage.EnableWindow(true);

	OnResizePercentageChanged();
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizePercentageChanged()
{
	CString strText;
	m_editPercentage.GetWindowText(strText);
	long lPercentage = _ttol(strText);
	double fScale = (double)lPercentage / 100;
	m_SizeLimits = m_SizeImage;
	m_SizeLimits.cx = dtoi(m_SizeLimits.cx * fScale);
	m_SizeLimits.cy = dtoi(m_SizeLimits.cy * fScale);

	UpdateDimensions();
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::UpdateDimensions()
{
	int dx = m_SizeLimits.cx;
	int dy = m_SizeLimits.cy;
	double fScale = ScaleToFit(&dx, &dy, m_SizeImage.cx, m_SizeImage.cy, true/*bUseSmallerFactor*/);
	m_Size.cx = dx;
	m_Size.cy = dy;
	m_staticNewDimensions.SetWindowText(Str(_T("%d x %d"), m_Size.cx, m_Size.cy));

	bool bEnableApply = !(m_Size == m_SizeImage);
	m_btnApply.EnableWindow(bEnableApply);
}
