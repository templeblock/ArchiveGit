#include "stdafx.h"
#include "TabControl.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTabControl, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelChange)
	ON_WM_DESTROY()
	ON_WM_ENABLE()
	ON_WM_SIZING()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CTabControl::CTabControl()
{
	m_iPreviousTab = 0;
	m_iMaxWidth = 0;
	m_iMaxHeight = 0;

	for (int i=0; i<sizeof(m_pTabDialogs)/sizeof(m_pTabDialogs[0]); i++)
		m_pTabDialogs[i] = NULL;

	m_iTabCount = 0;
	m_bSetMaxSize = false;
}

/////////////////////////////////////////////////////////////////////////////
CTabControl::~CTabControl()
{
}

/////////////////////////////////////////////////////////////////////////////
void CTabControl::PreSubclassWindow()
{
	CTabCtrl::PreSubclassWindow();

	ModifyStyle(0/*dwRemove*/, WS_CLIPCHILDREN/*dwAdd*/);

	// Make ourselves as big as our parent's client area
	if (m_bSetMaxSize && GetParent())
	{
		CRect Rect;
		GetParent()->GetClientRect(&Rect);
		AdjustRect(false, &Rect);
		MoveWindow(&Rect, false);
	}

	CreateTabs();
}

/////////////////////////////////////////////////////////////////////////////
void CTabControl::CreateTabs()
{
	// Create the tabs; initially hidden and disabled
	m_iTabCount = 0;

	m_pTabDialogs[m_iTabCount] = &m_MainTab;
	CreateTab(m_iTabCount++, CMainTab::IDD);

	m_pTabDialogs[m_iTabCount] = &m_BordersTab;
	CreateTab(m_iTabCount++, CBordersTab::IDD);

	m_pTabDialogs[m_iTabCount] = &m_ContrastBrightnessTab;
	CreateTab(m_iTabCount++, CContrastBrightnessTab::IDD);

	m_pTabDialogs[m_iTabCount] = &m_ColorsTab;
	CreateTab(m_iTabCount++, CColorsTab::IDD);

	m_pTabDialogs[m_iTabCount] = &m_CropTab;
	CreateTab(m_iTabCount++, CCropTab::IDD);

	m_pTabDialogs[m_iTabCount] = &m_EffectsTab;
	CreateTab(m_iTabCount++, CEffectsTab::IDD);

	m_pTabDialogs[m_iTabCount] = &m_FileTab;
	CreateTab(m_iTabCount++, CFileTab::IDD);

	m_pTabDialogs[m_iTabCount] = &m_LayoutTab;
	CreateTab(m_iTabCount++, CLayoutTab::IDD);

	m_pTabDialogs[m_iTabCount] = &m_RecolorTab;
	CreateTab(m_iTabCount++, CRecolorTab::IDD);

	m_pTabDialogs[m_iTabCount] = &m_RedEyeTab;
	CreateTab(m_iTabCount++, CRedEyeTab::IDD);

	m_pTabDialogs[m_iTabCount] = &m_ResizeTab;
	CreateTab(m_iTabCount++, CResizeTab::IDD);

	SetCurSel(0);

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
	CResizeDlg* pDialog = m_pTabDialogs[iIndex];
	if (!pDialog)
		return false;

	if (InsertItem(iIndex, _T("")) < 0)
		return false;

	if (!pDialog->Create(nId, this/*pParentWnd*/))
		return false;

	pDialog->SetDlgCtrlID(nId);

	TCITEM TabCtrlItem;
	::ZeroMemory(&TabCtrlItem, sizeof(TabCtrlItem));
	CString strTabName;
	pDialog->GetWindowText(strTabName);
	TabCtrlItem.mask = TCIF_TEXT;
	TabCtrlItem.pszText = strTabName.GetBuffer();
	bool bOK = !!SetItem(iIndex, &TabCtrlItem);

	if (!iIndex) pDialog->SetFocus();
	pDialog->EnableWindow(!iIndex ? true : false);
	pDialog->ShowWindow(!iIndex ? SW_SHOW : SW_HIDE);

	CRect WindowRect;
	pDialog->GetWindowRect(&WindowRect);
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
bool CTabControl::SetCurrentTab(UINT nId)
{
	for (int i=0; i<m_iTabCount; i++)
	{
		if (!m_pTabDialogs[i])
			continue;

		int nDialogId = ((CDialog*)m_pTabDialogs[i])->GetDlgCtrlID();
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
	LRESULT lResult = 0;
	OnSelChange(&NotifyMshHeader, &lResult);
	return nItemRet;
}

/////////////////////////////////////////////////////////////////////////////
void CTabControl::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	if (m_iPreviousTab == GetCurSel())
		return;

	m_pTabDialogs[m_iPreviousTab]->EnableWindow(false);
	m_pTabDialogs[m_iPreviousTab]->ShowWindow(SW_HIDE);
	m_iPreviousTab = GetCurSel();
	m_pTabDialogs[m_iPreviousTab]->EnableWindow(true);
	m_pTabDialogs[m_iPreviousTab]->ShowWindow(SW_SHOW);
	m_pTabDialogs[m_iPreviousTab]->SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
void CTabControl::OnDestroy()
{
	CTabCtrl::OnDestroy();

	for (int i=0; i<m_iTabCount; i++)
	{
		if (m_pTabDialogs[i])
			m_pTabDialogs[i]->DestroyWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTabControl::OnEnable(BOOL bEnable)
{
	CTabCtrl::OnEnable(bEnable);

	for (int i=0; i<m_iTabCount; i++)
	{
		if (m_pTabDialogs[i])
			m_pTabDialogs[i]->EnableWindow(bEnable);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTabControl::OnSizing(UINT nSide, LPRECT lpRect)
{
	CTabCtrl::OnSizing(nSide, lpRect);

	for (int i=0; i<m_iTabCount; i++)
	{
		if (m_pTabDialogs[i])
			m_pTabDialogs[i]->OnSizing(nSide, lpRect);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTabControl::OnSize(UINT nType, int cx, int cy)
{
	CTabCtrl::OnSize(nType, cx, cy);
	ResizeDialogs();
}
