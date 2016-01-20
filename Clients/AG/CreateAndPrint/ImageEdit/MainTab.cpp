#include "stdafx.h"
#include "MainTab.h"
#include "TabControl.h"
#include "MainDlg.h"

/////////////////////////////////////////////////////////////////////////////
CMainTab::CMainTab()
{
}

/////////////////////////////////////////////////////////////////////////////
CMainTab::~CMainTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainTab::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	DoDataExchange(DDX_LOAD);
	SetMsgHandled(false);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnEnterTab(bool bFirstTime)
{
	bool bClipArt = GetMainDlg().IsClipArt();
	bool bClipArtAdded = !bClipArt && !GetMainDlg().GetClipArtInfo().IsEmpty();
	bool bEnableRecolor = bClipArt || bClipArtAdded;
	bool bEnableRedEye = !bClipArt;
	bool bEnableFileAndResize = false;

	#ifndef _DEBUG
	#define SHIFT (GetAsyncKeyState(VK_SHIFT)<0)
	if (SHIFT)
	#endif _DEBUG
	{
		bEnableRecolor = true;
		bEnableFileAndResize = true;
	}

	// Handle the disabling of the recolor functionality
//j	GetDlgItem(IDC_IME_LABEL5).EnableWindow(bEnableRecolor);
//j	GetDlgItem(IDC_IME_ETCHEDLINE_COLOR).EnableWindow(bEnableRecolor);
	m_Recolor.EnableWindow(bEnableRecolor);
	m_iconRecolor.ShowWindow(bEnableRecolor ? SW_SHOW : SW_HIDE);	

	// Handle the disabling of the redeye functionality
	m_RedEye.EnableWindow(bEnableRedEye);
	m_iconRedEye.ShowWindow(bEnableRedEye ? SW_SHOW : SW_HIDE);

	// Handle the disabling of the resize and file functionality
	m_File.ShowWindow(bEnableFileAndResize ? SW_SHOW : SW_HIDE);
	m_Resize.ShowWindow(bEnableFileAndResize ? SW_SHOW : SW_HIDE);
	m_iconResize.ShowWindow(bEnableFileAndResize ? SW_SHOW : SW_HIDE);

	if (bFirstTime)
	{
		StandardizeHyperLink(m_Borders);
		StandardizeHyperLink(m_Color);
		StandardizeHyperLink(m_ContrastBrightness);
		StandardizeHyperLink(m_Crop);
		StandardizeHyperLink(m_Effects);
		StandardizeHyperLink(m_File);
		StandardizeHyperLink(m_FlipRotate);
		StandardizeHyperLink(m_RedEye);
		StandardizeHyperLink(m_Recolor);
		StandardizeHyperLink(m_Resize);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnLeaveTab()
{
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnBorders(UINT uCode, int nID, HWND hwndCtrl)
{
	CTabControl& TabControl = GetTabControl();
	TabControl.SetCurrentTab(CBordersTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnColor(UINT uCode, int nID, HWND hwndCtrl)
{
	CTabControl& TabControl = GetTabControl();
	TabControl.SetCurrentTab(CColorsTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnContrastBrightness(UINT uCode, int nID, HWND hwndCtrl)
{
	CTabControl& TabControl = GetTabControl();
	TabControl.SetCurrentTab(CContrastBrightnessTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnCrop(UINT uCode, int nID, HWND hwndCtrl)
{
	CTabControl& TabControl = GetTabControl();
	TabControl.SetCurrentTab(CCropTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnEffects(UINT uCode, int nID, HWND hwndCtrl)
{
	CTabControl& TabControl = GetTabControl();
	TabControl.SetCurrentTab(CEffectsTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnFile(UINT uCode, int nID, HWND hwndCtrl)
{
	CTabControl& TabControl = GetTabControl();
	TabControl.SetCurrentTab(CFileTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnFlipRotate(UINT uCode, int nID, HWND hwndCtrl)
{
	CTabControl& TabControl = GetTabControl();
	TabControl.SetCurrentTab(CFlipRotateTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnRecolor(UINT uCode, int nID, HWND hwndCtrl)
{
	CTabControl& TabControl = GetTabControl();
	TabControl.SetCurrentTab(CRecolorTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnRedEye(UINT uCode, int nID, HWND hwndCtrl)
{
	CTabControl& TabControl = GetTabControl();
	TabControl.SetCurrentTab(CRedEyeTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnResize(UINT uCode, int nID, HWND hwndCtrl)
{
	CTabControl& TabControl = GetTabControl();
	TabControl.SetCurrentTab(CResizeTab::IDD);
}
