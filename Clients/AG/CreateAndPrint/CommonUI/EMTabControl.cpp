#include "stdafx.h"
#include "EMTabControl.h"
#include "EMDlg.h"

/////////////////////////////////////////////////////////////////////////////
CEMTabControl::CEMTabControl(CEMDlg * pEMDlg) : m_EMAddressTab(pEMDlg),  m_EMDesignTab(pEMDlg)
{
	m_iPreviousTab = 0;
	m_iMaxWidth = 0;
	m_iMaxHeight = 0;

	m_pEMDlg = pEMDlg;

	int nTabs = sizeof(m_pTabDialogs) / sizeof(m_pTabDialogs[0]);
	for (int i=0; i<nTabs; i++)
		m_pTabDialogs[i] = NULL;

	m_iTabCount = 0;
	m_bSetMaxSize = false;
}

/////////////////////////////////////////////////////////////////////////////
CEMTabControl::~CEMTabControl()
{
}

/////////////////////////////////////////////////////////////////////////////
void CEMTabControl::Init(UINT idTab)
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
void CEMTabControl::CreateTabs()
{
	// Create the tabs; initially hidden and disabled
	m_iTabCount = 0;

	m_EMAddressTab.Create(m_hWnd);
	m_EMDesignTab.Create(m_hWnd);
	

	m_pTabDialogs[m_iTabCount] = (CDialogImplBase*)&m_EMAddressTab;
	CreateTab(m_iTabCount++, CEMAddressTab::IDD);

	m_pTabDialogs[m_iTabCount] = (CDialogImplBase*)&m_EMDesignTab;
	CreateTab(m_iTabCount++, CEMDesignTab::IDD);

	SetCurSel(0);

	ResizeDialogs();
}

/////////////////////////////////////////////////////////////////////////////
// Create the dialogs that are the tab control's contents.
// The parent must be the same parent as the tab control's.
// Therefore they will be repositioned later. Until
// then we gather the maximum sizes so the tab control can
// be adjusted in size so the tab dialogs will fit.
bool CEMTabControl::CreateTab(int iIndex, UINT nId)
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
void CEMTabControl::ResizeDialogs()
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
CWindow& CEMTabControl::GetBaseTab()
{
	UINT idTab = GetCurrentTab();
	if (idTab == CEMAddressTab::IDD)
		return m_EMAddressTab;

	if (idTab == CEMDesignTab::IDD)
		return m_EMDesignTab;
	
		
	return m_EMAddressTab;
}


/////////////////////////////////////////////////////////////////////////////
UINT CEMTabControl::GetCurrentTab()
{
	int i = GetCurSel();
	if (i < 0)
		return 0;

	return m_pTabDialogs[i]->GetDlgCtrlID();
}

/////////////////////////////////////////////////////////////////////////////
bool CEMTabControl::SetCurrentTab(UINT nId)
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
int CEMTabControl::SetCurSel(int nItem)
{
	int nItemRet = CTabCtrl::SetCurSel(nItem);
	NMHDR NotifyMshHeader;
	::ZeroMemory(&NotifyMshHeader, sizeof(NotifyMshHeader));
	BOOL bHandled = true;
	LRESULT lResult = OnSelChange(GetDlgCtrlID(), &NotifyMshHeader, bHandled);
	return nItemRet;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CEMTabControl::OnSelChange(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
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
void CEMTabControl::OnDestroy()
{
	for (int i=0; i<m_iTabCount; i++)
	{
		if (m_pTabDialogs[i])
			m_pTabDialogs[i]->DestroyWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CEMTabControl::OnEnable(BOOL bEnable)
{
	for (int i=0; i<m_iTabCount; i++)
	{
		if (!m_pTabDialogs[i])
			continue;

		m_pTabDialogs[i]->EnableWindow(bEnable);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CEMTabControl::OnSize(UINT nType, CSize Size)
{
	ResizeDialogs();
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CEMTabControl::OnGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)this;
}
