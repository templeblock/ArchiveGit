#include "stdafx.h"
#include "TabControl.h"

/////////////////////////////////////////////////////////////////////////////
CTabControl::CTabControl()
{
	m_iPreviousTab = 0;
	m_iMaxWidth = 0;
	m_iMaxHeight = 0;

	int nTabs = sizeof(m_pTabDialogs) / sizeof(m_pTabDialogs[0]);
	for (int i=0; i<nTabs; i++)
		m_pTabDialogs[i] = NULL;

	m_iTabCount = 0;
	m_bSetMaxSize = false;
}

/////////////////////////////////////////////////////////////////////////////
CTabControl::~CTabControl()
{
}

/////////////////////////////////////////////////////////////////////////////
void CTabControl::Init(UINT idTab)
{
	ModifyStyle(0/*dwRemove*/, WS_CLIPCHILDREN/*dwAdd*/);

	// Make ourselves as big as our parent's client area
	if (m_bSetMaxSize && GetParent())
	{
		CRect Rect;
		GetParent().GetClientRect(&Rect);
		AdjustRect(false, &Rect);
		MoveWindow(&Rect, false);
	}

	CreateTabs();
	SetCurrentTab(idTab);
}

/////////////////////////////////////////////////////////////////////////////
void CTabControl::CreateTabs()
{
	// Create the tabs; initially hidden and disabled
	m_iTabCount = 0;

	m_MainTab.Create(m_hWnd);
	m_FileTab.Create(m_hWnd);
	m_CropTab.Create(m_hWnd);
	m_ContrastBrightnessTab.Create(m_hWnd);
	m_EffectsTab.Create(m_hWnd);
	m_ColorsTab.Create(m_hWnd);
	m_ResizeTab.Create(m_hWnd);
	m_FlipRotateTab.Create(m_hWnd);
	m_RedEyeTab.Create(m_hWnd);
	m_BordersTab.Create(m_hWnd);
	m_RecolorTab.Create(m_hWnd);

	m_pTabDialogs[m_iTabCount] = (CDialogImplBase*)&m_MainTab;
	CreateTab(m_iTabCount++, CMainTab::IDD);

	m_pTabDialogs[m_iTabCount] = (CDialogImplBase*)&m_FileTab;
	CreateTab(m_iTabCount++, CFileTab::IDD);

	m_pTabDialogs[m_iTabCount] = (CDialogImplBase*)&m_CropTab;
	CreateTab(m_iTabCount++, CCropTab::IDD);

	m_pTabDialogs[m_iTabCount] = (CDialogImplBase*)&m_ContrastBrightnessTab;
	CreateTab(m_iTabCount++, CContrastBrightnessTab::IDD);

	m_pTabDialogs[m_iTabCount] = (CDialogImplBase*)&m_EffectsTab;
	CreateTab(m_iTabCount++, CEffectsTab::IDD);

	m_pTabDialogs[m_iTabCount] = (CDialogImplBase*)&m_ColorsTab;
	CreateTab(m_iTabCount++, CColorsTab::IDD);

	m_pTabDialogs[m_iTabCount] = (CDialogImplBase*)&m_ResizeTab;
	CreateTab(m_iTabCount++, CResizeTab::IDD);

	m_pTabDialogs[m_iTabCount] = (CDialogImplBase*)&m_FlipRotateTab;
	CreateTab(m_iTabCount++, CFlipRotateTab::IDD);

	m_pTabDialogs[m_iTabCount] = (CDialogImplBase*)&m_RedEyeTab;
	CreateTab(m_iTabCount++, CRedEyeTab::IDD);

	m_pTabDialogs[m_iTabCount] = (CDialogImplBase*)&m_BordersTab;
	CreateTab(m_iTabCount++, CBordersTab::IDD);

	m_pTabDialogs[m_iTabCount] = (CDialogImplBase*)&m_RecolorTab;
	CreateTab(m_iTabCount++, CRecolorTab::IDD);

//j	SetCurSel(0);

	ResizeDialogs();
}

/////////////////////////////////////////////////////////////////////////////
// Create the dialogs that are the tab control's contents.
// The parent must be the same parent as the tab control's.
// Therefore they will be repositioned later. Until
// then we gather the maximum sizes so the tab control can
// be adjusted in size so the tab dialogs will fit.
bool CTabControl::CreateTab(int iIndex, UINT nId)
{
	CDialogImplBase* pTabDialog = m_pTabDialogs[iIndex];
	if (!pTabDialog)
		return false;

	if (InsertItem(iIndex, _T("")) < 0)
	{
		m_pTabDialogs[iIndex] = NULL;
		return false;
	}

	pTabDialog->SetDlgCtrlID(nId);

	TCITEM TabCtrlItem;
	::ZeroMemory(&TabCtrlItem, sizeof(TabCtrlItem));
	CString strTabName;
	pTabDialog->GetWindowText(strTabName);
	TabCtrlItem.mask = TCIF_TEXT;
	TabCtrlItem.pszText = strTabName.GetBuffer();
	bool bOK = !!SetItem(iIndex, &TabCtrlItem);

	if (!iIndex) pTabDialog->SetFocus();
	pTabDialog->EnableWindow(!iIndex ? true : false);
	pTabDialog->ShowWindow(!iIndex ? SW_SHOW : SW_HIDE);

	CRect WindowRect;
	pTabDialog->GetWindowRect(&WindowRect);
	int tw = WindowRect.Width();
	m_iMaxWidth = tw < m_iMaxWidth ? m_iMaxWidth : tw;
	int th = WindowRect.Height();
	m_iMaxHeight = th < m_iMaxHeight ? m_iMaxHeight : th;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CTabControl::ResizeDialogs()
{
	CRect Rect;
	GetClientRect(&Rect);
	AdjustRect(false, &Rect);

	for (int i=0; i<m_iTabCount; i++)
	{
		if (m_pTabDialogs[i])
			m_pTabDialogs[i]->MoveWindow(Rect.left, Rect.top, Rect.Width(), Rect.Height(), false);
	}
}

/////////////////////////////////////////////////////////////////////////////
CWindow& CTabControl::GetBaseTab()
{
	UINT idTab = GetCurrentTab();
	if (idTab == CMainTab::IDD)
		return m_MainTab;
	if (idTab == CFileTab::IDD)
		return m_FileTab;
	if (idTab == CCropTab::IDD)
		return m_CropTab;
	if (idTab == CContrastBrightnessTab::IDD)
		return m_ContrastBrightnessTab;
	if (idTab == CEffectsTab::IDD)
		return m_EffectsTab;
	if (idTab == CColorsTab::IDD)
		return m_ColorsTab;
	if (idTab == CResizeTab::IDD)
		return m_ResizeTab;
	if (idTab == CFlipRotateTab::IDD)
		return m_FlipRotateTab;
	if (idTab == CRedEyeTab::IDD)
		return m_RedEyeTab;
	if (idTab == CBordersTab::IDD)
		return m_BordersTab;
	if (idTab == CRecolorTab::IDD)
		return m_RecolorTab;
		
	return m_MainTab;
}

/////////////////////////////////////////////////////////////////////////////
bool CTabControl::CanApply()
{
	CWindow TabWindow = CTabControl::GetBaseTab();
	CWindow Control = TabWindow.GetDlgItem(IDC_IME_APPLY);
	if (!Control.m_hWnd)
		return false;

	return !!Control.IsWindowEnabled();
}

/////////////////////////////////////////////////////////////////////////////
bool CTabControl::CanReset()
{
	CWindow TabWindow = CTabControl::GetBaseTab();
	CWindow Control = TabWindow.GetDlgItem(IDC_IME_RESET);
	if (!Control.m_hWnd)
		return false;

	return !!Control.IsWindowEnabled();
}

/////////////////////////////////////////////////////////////////////////////
bool CTabControl::Apply()
{
	UINT idTab = GetCurrentTab();
	if (idTab == CMainTab::IDD)
		m_MainTab.Apply();
	else
	if (idTab == CFileTab::IDD)
		m_FileTab.Apply();
	else
	if (idTab == CCropTab::IDD)
		m_CropTab.Apply();
	else
	if (idTab == CContrastBrightnessTab::IDD)
		m_ContrastBrightnessTab.Apply();
	else
	if (idTab == CEffectsTab::IDD)
		m_EffectsTab.Apply();
	else
	if (idTab == CColorsTab::IDD)
		m_ColorsTab.Apply();
	else
	if (idTab == CResizeTab::IDD)
		m_ResizeTab.Apply();
	else
	if (idTab == CFlipRotateTab::IDD)
		m_FlipRotateTab.Apply();
	else
	if (idTab == CRedEyeTab::IDD)
		m_RedEyeTab.Apply();
	else
	if (idTab == CBordersTab::IDD)
		m_BordersTab.Apply();
	else
	if (idTab == CRecolorTab::IDD)
		m_RecolorTab.Apply();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CTabControl::Reset()
{
	UINT idTab = GetCurrentTab();
	if (idTab == CMainTab::IDD)
		m_MainTab.Reset();
	else
	if (idTab == CFileTab::IDD)
		m_FileTab.Reset();
	else
	if (idTab == CCropTab::IDD)
		m_CropTab.Reset();
	else
	if (idTab == CContrastBrightnessTab::IDD)
		m_ContrastBrightnessTab.Reset();
	else
	if (idTab == CEffectsTab::IDD)
		m_EffectsTab.Reset();
	else
	if (idTab == CColorsTab::IDD)
		m_ColorsTab.Reset();
	else
	if (idTab == CResizeTab::IDD)
		m_ResizeTab.Reset();
	else
	if (idTab == CFlipRotateTab::IDD)
		m_FlipRotateTab.Reset();
	else
	if (idTab == CRedEyeTab::IDD)
		m_RedEyeTab.Reset();
	else
	if (idTab == CBordersTab::IDD)
		m_BordersTab.Reset();
	else
	if (idTab == CRecolorTab::IDD)
		m_RecolorTab.Reset();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
UINT CTabControl::GetCurrentTab()
{
	int i = GetCurSel();
	if (i < 0)
		return 0;

	return m_pTabDialogs[i]->GetDlgCtrlID();
}

/////////////////////////////////////////////////////////////////////////////
bool CTabControl::SetCurrentTab(UINT nId)
{
	for (int i=0; i<m_iTabCount; i++)
	{
		if (!m_pTabDialogs[i])
			continue;

		int nDialogId = m_pTabDialogs[i]->GetDlgCtrlID();
		if (nDialogId != nId)
			continue;

		return SetCurSel(i) >= 0;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
int CTabControl::SetCurSel(int nItem)
{
	int nItemRet = CTabCtrl::SetCurSel(nItem);
	NMHDR NotifyMshHeader;
	::ZeroMemory(&NotifyMshHeader, sizeof(NotifyMshHeader));
	BOOL bHandled = true;
	LRESULT lResult = OnSelChange(GetDlgCtrlID(), &NotifyMshHeader, bHandled);
	return nItemRet;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CTabControl::OnSelChange(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	if (m_iPreviousTab == GetCurSel())
		return S_OK;

	m_pTabDialogs[m_iPreviousTab]->EnableWindow(false);
	m_pTabDialogs[m_iPreviousTab]->ShowWindow(SW_HIDE);
	m_iPreviousTab = GetCurSel();
	m_pTabDialogs[m_iPreviousTab]->EnableWindow(true);
	m_pTabDialogs[m_iPreviousTab]->ShowWindow(SW_SHOW);
	m_pTabDialogs[m_iPreviousTab]->SetFocus();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CTabControl::OnDestroy()
{
	for (int i=0; i<m_iTabCount; i++)
	{
		if (m_pTabDialogs[i])
			m_pTabDialogs[i]->DestroyWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTabControl::OnEnable(BOOL bEnable)
{
	for (int i=0; i<m_iTabCount; i++)
	{
		if (!m_pTabDialogs[i])
			continue;

		m_pTabDialogs[i]->EnableWindow(bEnable);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTabControl::OnSize(UINT nType, CSize Size)
{
	ResizeDialogs();
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CTabControl::OnGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)this;
}
