#include "stdafx.h"
#include "CropTab.h"
#include "MainDlg.h"
#include "Utility.h"

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
CCropTab::CCropTab()
{
}

/////////////////////////////////////////////////////////////////////////////
CCropTab::~CCropTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCropTab::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	DoDataExchange(DDX_LOAD);
	SetMsgHandled(false);

	m_comboAspectRatio.AddString(_T("Any"));
	int nSizes = sizeof(g_Sizes) / sizeof(CSize);
	for (int i=1; i<nSizes; i++)
		m_comboAspectRatio.AddString(Str(_T("%d x %d"), g_Sizes[i].cx, g_Sizes[i].cy));

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnEnterTab(bool bFirstTime)
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
	EnableButtons(/*Apply*/false, /*Reset*/false);
	SetTimer(TAB_TIMER, 1000, NULL);
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnLeaveTab()
{
	KillTimer(TAB_TIMER);
	bool bOK = DoCommand(_T("CropTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnApply(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("CropTab"), Str(_T("%d"), EDIT_APPLY));
	OnEnterTab();
	GoBack(); // added
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnReset(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("CropTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnCropPortrait(UINT uCode, int nID, HWND hwndCtrl)
{
	m_radioPortrait.SetCheck(BST_CHECKED);
	m_radioLandscape.SetCheck(BST_UNCHECKED);
	SetCropAspect();
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnCropLandscape(UINT uCode, int nID, HWND hwndCtrl)
{
	m_radioPortrait.SetCheck(BST_UNCHECKED);
	m_radioLandscape.SetCheck(BST_CHECKED);
	SetCropAspect();
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnCropAspectRatio(UINT uCode, int nID, HWND hwndCtrl)
{
	int iCurSel = m_comboAspectRatio.GetCurSel();
	m_radioPortrait.EnableWindow(iCurSel != 0);
	m_radioLandscape.EnableWindow(iCurSel != 0);
	SetCropAspect();
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnTimer(UINT nIDEvent, TIMERPROC TimerProc)
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
	long l = GetCommand(_T("GetCropLeft"));
	long t = GetCommand(_T("GetCropTop"));
	long r = GetCommand(_T("GetCropRight"));
	long b = GetCommand(_T("GetCropBottom"));
	CRect CropRect(l, t, r, b);
	if (CropRect.EqualRect(m_CropRect))
		return;

	EnableButtons(/*Apply*/true, /*Reset*/true);
	m_CropRect = CropRect;
	m_staticNewDimensions.SetWindowText(Str(_T("%d x %d"), m_CropRect.Width(), m_CropRect.Height()));
}
