#include "stdafx.h"
#include "ResizeTab.h"
#include "MainDlg.h"
#include "Utility.h"

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
CResizeTab::CResizeTab()
{
}

/////////////////////////////////////////////////////////////////////////////
CResizeTab::~CResizeTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CResizeTab::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	DoDataExchange(DDX_LOAD);
	SetMsgHandled(false);

	int nSizes = sizeof(g_Sizes) / sizeof(CPredefined);
	for (int i=0; i<nSizes; i++)
		m_comboPredefinedSizes.AddString(Str(_T("%s (%d x %d)"), g_Sizes[i].pName, g_Sizes[i].cx, g_Sizes[i].cy));
	m_comboPredefinedSizes.SetCurSel(0);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnEnterTab(bool bFirstTime)
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

	m_comboPredefinedSizes.SetCurSel(0);

	m_editCustomWidth.SetWindowText(Str(_T("%d"), lWidth));
	m_editCustomHeight.SetWindowText(Str(_T("%d"), lHeight));
	m_editPercentage.SetWindowText(Str(_T("%d"), 100));
	m_staticOriginalDimensions.SetWindowText(Str(_T("%d x %d"), lWidth, lHeight));

	OnResizeOriginal(0, 0, 0);

	bool bOK = DoCommand(_T("ResizeTab"), Str(_T("%d"), EDIT_START));
	EnableButtons(/*Apply*/false, /*Reset*/false);
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("ResizeTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnApply(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("ResizeApply"), Str(_T("%d,%d"), m_Size.cx, m_Size.cy));
	OnEnterTab();
	GoBack(); // added
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnReset(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("ResizeTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizeOriginal(UINT uCode, int nID, HWND hwndCtrl)
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
void CResizeTab::OnResizePredefined(UINT uCode, int nID, HWND hwndCtrl)
{
	m_radioOriginalSize.SetCheck(BST_UNCHECKED);
	m_radioPredefinedSize.SetCheck(BST_CHECKED);
	m_radioCustomSize.SetCheck(BST_UNCHECKED);
	m_radioPercetageofOriginal.SetCheck(BST_UNCHECKED);

	m_comboPredefinedSizes.EnableWindow(true);
	m_editCustomWidth.EnableWindow(false);
	m_editCustomHeight.EnableWindow(false);
	m_editPercentage.EnableWindow(false);

	OnResizePredefinedChanged(uCode, nID, hwndCtrl);
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizePredefinedChanged(UINT uCode, int nID, HWND hwndCtrl)
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
void CResizeTab::OnResizeCustom(UINT uCode, int nID, HWND hwndCtrl)
{
	m_radioOriginalSize.SetCheck(BST_UNCHECKED);
	m_radioPredefinedSize.SetCheck(BST_UNCHECKED);
	m_radioCustomSize.SetCheck(BST_CHECKED);
	m_radioPercetageofOriginal.SetCheck(BST_UNCHECKED);

	m_comboPredefinedSizes.EnableWindow(false);
	m_editCustomWidth.EnableWindow(true);
	m_editCustomHeight.EnableWindow(true);
	m_editPercentage.EnableWindow(false);

	OnResizeCustomSizeChanged(uCode, nID, hwndCtrl);
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizeCustomSizeChanged(UINT uCode, int nID, HWND hwndCtrl)
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
void CResizeTab::OnResizePercentage(UINT uCode, int nID, HWND hwndCtrl)
{
	m_radioOriginalSize.SetCheck(BST_UNCHECKED);
	m_radioPredefinedSize.SetCheck(BST_UNCHECKED);
	m_radioCustomSize.SetCheck(BST_UNCHECKED);
	m_radioPercetageofOriginal.SetCheck(BST_CHECKED);

	m_comboPredefinedSizes.EnableWindow(false);
	m_editCustomWidth.EnableWindow(false);
	m_editCustomHeight.EnableWindow(false);
	m_editPercentage.EnableWindow(true);

	OnResizePercentageChanged(uCode, nID, hwndCtrl);
}

/////////////////////////////////////////////////////////////////////////////
void CResizeTab::OnResizePercentageChanged(UINT uCode, int nID, HWND hwndCtrl)
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

	bool bEnableButtons = !(m_Size == m_SizeImage);
	EnableButtons(/*Apply*/bEnableButtons, /*Reset*/bEnableButtons);
}
