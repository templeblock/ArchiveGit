/*
// $Header: /PM8/App/TABSDLG.CPP 1     3/03/99 6:12p Gbeddow $
//
// Tabs dialog
//
// $Log: /PM8/App/TABSDLG.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:40   Fred
// Initial revision.
// 
//    Rev 1.4   01 Aug 1996 09:22:34   Fred
// More 16-bit changes
// 
//    Rev 1.3   21 Jun 1996 17:50:46   Jay
//  
// 
//    Rev 1.2   20 Jun 1996 15:39:48   Jay
// Now handles multiple paragraph tabs correctly.
// 
//    Rev 1.1   19 Jun 1996 13:46:10   Jay
//  
// 
//    Rev 1.0   18 Jun 1996 16:16:16   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "Tabsdlg.h"
#include "text.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int nTabPrecision = -3;

/////////////////////////////////////////////////////////////////////////////
// CTabsDialog dialog

CTabsDialog::CTabsDialog(CTabArray* pTabs, BOOL fMultiple, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CTabsDialog::IDD, pParent), m_fMultiple(fMultiple)
{
	if (m_fMultiple)
	{
		// Just copy over the default size.
		m_Tabs.DefaultTabSize(pTabs->DefaultTabSize());
	}
	else
	{
		m_Tabs = *pTabs;
	}

	m_fDefaultTabSize = (m_Tabs.DefaultTabSize() != UNDEFINED_CFIXED);
	m_dDefaultTabSize = MakeDouble(m_Tabs.DefaultTabSize());

	//{{AFX_DATA_INIT(CTabsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CTabsDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabsDialog)
		DDX_Control(pDX, IDC_TAB_STOPS, m_TabStops);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		GetDoubleValue(pDX, IDC_DEFAULT_TABS, m_dDefaultTabSize, m_fDefaultTabSize, 0., 99.);
	}
	else
	{
		SetDoubleValue(IDC_DEFAULT_TABS, m_dDefaultTabSize, -4, m_fDefaultTabSize);
	}
}

BEGIN_MESSAGE_MAP(CTabsDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CTabsDialog)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_CLEAR_ALL, OnClearAll)
	ON_LBN_SELCHANGE(IDC_TAB_STOPS, OnSelchangeTabStops)
	ON_BN_CLICKED(IDC_SET_TAB, OnSetTab)
	ON_BN_CLICKED(IDC_TAB_CENTER, OnTabAlignment)
	ON_EN_CHANGE(IDC_POSITION, OnChangePosition)
	ON_BN_CLICKED(IDC_LEADER_CUSTOM, OnTabLeader)
	ON_BN_CLICKED(IDC_TAB_DECIMAL, OnTabAlignment)
	ON_BN_CLICKED(IDC_TAB_LEFT, OnTabAlignment)
	ON_BN_CLICKED(IDC_TAB_RIGHT, OnTabAlignment)
	ON_BN_CLICKED(IDC_LEADER_NONE, OnTabLeader)
	ON_BN_CLICKED(IDC_LEADER_UNDERLINE, OnTabLeader)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTabArray* CTabsDialog::GetTabs(void)
{
	if (m_fDefaultTabSize)
	{
		m_Tabs.DefaultTabSize(MakeFixed(m_dDefaultTabSize));
	}
	return &m_Tabs;
}

CTextTab* CTabsDialog::GetCurrentTab(void)
{
	CTextTab* pTab = NULL;

	int nCurSel = m_TabStops.GetCurSel();
	if (nCurSel != -1)
	{
		CFixed Position = m_TabStops.GetItemData(nCurSel);
		int nTab = m_Tabs.GetTabAt(Position);
		if (nTab != -1)
		{
			pTab = m_Tabs.GetAt(nTab);
		}
	}

	if (pTab == NULL)
	{
		pTab = &m_NewTab;
	}
	return pTab;
}

/*
// Update the tab list.
*/

void CTabsDialog::UpdateTabList(void)
{
	// Prevent drawing while we update.
	m_TabStops.SetRedraw(FALSE);
	// Clear all existing tabstops.
	m_TabStops.ResetContent();

	int nTabs = m_Tabs.NumberOfTabs();
	for (int nTab = 0; nTab < nTabs; nTab++)
	{
		AddTabStop(m_Tabs.GetAt(nTab));
	}

	// Allow drawing again.
	m_TabStops.SetRedraw(TRUE);
}

/*
// Update the values for the current tab.
*/

void CTabsDialog::UpdateCurrentTab(BOOL fUpdatePosition)
{
	CTextTab* pTab = GetCurrentTab();
	ASSERT(pTab != NULL);
	if (pTab != NULL)
	{
		// Update the alignment.
		CheckRadioButton(IDC_TAB_LEFT, IDC_TAB_DECIMAL, IDC_TAB_LEFT+pTab->m_nType);
		OnTabAlignment();

		// Update the leader.
		UINT uID;
		if (pTab->m_Leader[0] == 0)
		{
			uID = IDC_LEADER_NONE;
		}
		else if (pTab->m_Leader[0] == UNDERLINE_LEADER)
		{
			uID = IDC_LEADER_UNDERLINE;
		}
		else
		{
			uID = IDC_LEADER_CUSTOM;
		}
		CheckRadioButton(IDC_LEADER_NONE, IDC_LEADER_CUSTOM, uID);
		OnTabLeader();

		// Update the position.
		if (fUpdatePosition
				&& pTab->m_Position != -1
				&& GetFocus() != GetDlgItem(IDC_POSITION))
		{
			CString csValue;
			Util::FormatDouble(MakeDouble(pTab->m_Position), nTabPrecision, csValue);
			SetDlgItemText(IDC_POSITION, csValue);
		}
	}
}

/*
// Update the buttons.
*/

void CTabsDialog::UpdateButtons(void)
{
	// Compute whether we have a valid position typed in or not.
	BOOL fEmpty;
	CFixed Position = GetPosition(&fEmpty);
	fEmpty = fEmpty || (Position < 0);

	// Handle the clear button.
	CWnd* pChild;
	if ((pChild = GetDlgItem(IDC_CLEAR)) != NULL)
	{
		BOOL fEnable;

		if (m_fMultiple)
		{
			fEnable = !fEmpty;
		}
		else
		{
			fEnable = (m_TabStops.GetCurSel() >= 0);
		}
		pChild->EnableWindow(fEnable);
	}

	// Handle the clear all button.
	if ((pChild = GetDlgItem(IDC_CLEAR_ALL)) != NULL)
	{
		if (m_fMultiple)
		{
			// The multiple case, we always allow clear all.
		}
		else
		{
			pChild->EnableWindow(m_TabStops.GetCount() > 0);
		}
	}

	// Handle the set button.
	if ((pChild = GetDlgItem(IDC_SET_TAB)) != NULL)
	{
		pChild->EnableWindow(!fEmpty);
	}
}

/*
// Select the tab by position.
*/

void CTabsDialog::SelectTab(CFixed Position, BOOL fUpdate /*=TRUE*/)
{
	int nItems = m_TabStops.GetCount();
	int nItem;
	for (nItem = 0; nItem < nItems; nItem++)
	{
		if ((CFixed)m_TabStops.GetItemData(nItem) == Position)
		{
			break;
		}
	}

	if (nItem == nItems)
	{
		nItem = -1;
	}
	if (nItem != m_TabStops.GetCurSel())
	{
		m_TabStops.SetCurSel(nItem);
		if (fUpdate)
		{
			OnSelchangeTabStops();
		}
	}
}

/*
// Get the current position.
*/

CFixed CTabsDialog::GetPosition(BOOL* pfEmpty /*=NULL*/)
{
	CString csValue;
	Util::GetDlgItemText(this, IDC_POSITION, csValue);

	if (pfEmpty != NULL)
	{
		*pfEmpty = csValue.IsEmpty();
	}

	double dValue = atof(csValue);
	Util::FormatDouble(dValue, nTabPrecision, csValue);
	return MakeFixed(atof(csValue));
}

/*
// Add a note to clear a tabstop.
// This is used in the multiple paragraph case.
*/

void CTabsDialog::AddTabStopClear(CFixed Position)
{
	int nTabStopsToClear = m_cdaTabStopsToClear.GetSize();
	for (int n = 0; n < nTabStopsToClear; n++)
	{
		if (m_cdaTabStopsToClear.GetAt(n) == (DWORD)Position)
		{
			return;
		}
	}

	m_cdaTabStopsToClear.Add((DWORD)Position);
}

/////////////////////////////////////////////////////////////////////////////
// CTabsDialog message handlers

void CTabsDialog::OnClear() 
{
	if (m_fMultiple)
	{
		BOOL fEmpty;
		CFixed Position = GetPosition(&fEmpty);
		if (!fEmpty)
		{
			AddTabStopClear(Position);
		}
	}

	int nCurSel = m_TabStops.GetCurSel();
	if (nCurSel >= 0)
	{
		CFixed Position = (CFixed)m_TabStops.GetItemData(nCurSel);
		int nTab = m_Tabs.GetTabAt(Position);
		if (nTab != -1)
		{
			m_Tabs.RemoveAt(nTab);
			UpdateTabList();
			int nTabs = m_TabStops.GetCount();
			m_TabStops.SetCurSel(nTab >= nTabs ? nTabs-1 : nTab);
			UpdateCurrentTab(TRUE);
			UpdateButtons();
		}
	}

}

void CTabsDialog::OnClearAll() 
{
	if (m_fMultiple)
	{
		AddTabStopClear(UNDEFINED_CFIXED);
	}

	// Remove any tabs from the list.
	if (m_Tabs.NumberOfTabs() > 0)
	{
		m_Tabs.RemoveAll();
		UpdateTabList();
		UpdateCurrentTab(TRUE);
		UpdateButtons();
	}
}

void CTabsDialog::OnSelchangeTabStops() 
{
	UpdateCurrentTab(TRUE);
	UpdateButtons();
}

BOOL CTabsDialog::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_DECIMAL_CHARACTER))->LimitText(1);
	((CEdit*)GetDlgItem(IDC_LEADER_CHARACTER))->LimitText(2);

	m_NewTab.Default();

	// Update things.
	UpdateTabList();
	UpdateCurrentTab(FALSE);
	UpdateButtons();

	return TRUE;
}

/*
// Add a tab stop to the list.
*/

void CTabsDialog::AddTabStop(CTextTab* pTab)
{
	// Find the place to add the tab stop.
	// We sort on position.
	CFixed Position = pTab->m_Position;

	int nItems = m_TabStops.GetCount();
	int nItem;
	for (nItem = 0; nItem < nItems; nItem++)
	{
		CFixed ThisPosition = (CFixed)m_TabStops.GetItemData(nItem);
		ASSERT(ThisPosition != Position);
		if (ThisPosition > Position)
		{
			break;
		}
	}
	// Now we have the place to insert the item.
	CString csValue;
	Util::FormatDouble(MakeDouble(Position), nTabPrecision, csValue);
	int nNewItem = m_TabStops.InsertString(nItem, csValue);
	if (nNewItem != LB_ERR)
	{
		ASSERT(nNewItem == nItem);
		m_TabStops.SetItemData(nNewItem, (DWORD)Position);
	}
}

void CTabsDialog::OnSetTab() 
{
	CTextTab* pTab = GetCurrentTab();

	if (pTab == &m_NewTab)
	{
		// We need to add a tab at the current position.
		CTextTab Tab = m_NewTab;
		Tab.m_Position = GetPosition();

		// Add the tab.
		m_Tabs.Add(&Tab);

		// Update the list.
		UpdateTabList();

		// Select the tab by position.
		SelectTab(Tab.m_Position, FALSE);
		UpdateButtons();

		// Get the new tab.
		pTab = GetCurrentTab();
		if (pTab == NULL)
		{
			ASSERT(FALSE);
			return;
		}
	}

	// Set all the rest of the stuff.
	// Set the alignment.
	int nType = CTextTab::TAB_left;		// some default
	if (IsDlgButtonChecked(IDC_TAB_CENTER))
	{
		nType = CTextTab::TAB_center;
	}
	else if (IsDlgButtonChecked(IDC_TAB_RIGHT))
	{
		nType = CTextTab::TAB_right;
	}
	else if (IsDlgButtonChecked(IDC_TAB_DECIMAL))
	{
		nType = CTextTab::TAB_decimal;
		CString csValue;
		Util::GetDlgItemText(this, IDC_DECIMAL_CHARACTER, csValue);
		pTab->m_DecimalCharacter = (csValue.GetLength() > 0)
											? (BYTE)csValue[0]
											: '.';
	}
	pTab->m_nType = nType;

	// Set the leader.
	if (IsDlgButtonChecked(IDC_LEADER_NONE))
	{
		pTab->m_Leader[0] =
			pTab->m_Leader[1] = 0;
	}
	else if (IsDlgButtonChecked(IDC_LEADER_UNDERLINE))
	{
		pTab->m_Leader[0] = UNDERLINE_LEADER;
	}
	else
	{
		// Custom.
		ASSERT(IsDlgButtonChecked(IDC_LEADER_CUSTOM));

		CString csValue;
		Util::GetDlgItemText(this, IDC_LEADER_CHARACTER, csValue);

		int nLen = csValue.GetLength();
		pTab->m_Leader[0] = (nLen > 0) ? (BYTE)csValue[0] : 0;
		pTab->m_Leader[1] = (nLen > 1) ? (BYTE)csValue[1] : 0;
	}
}

void CTabsDialog::OnTabAlignment() 
{
	BOOL fEnable = IsDlgButtonChecked(IDC_TAB_DECIMAL);

	CTextTab* pTab = GetCurrentTab();
	ASSERT(pTab != NULL);
	if (pTab == NULL)
	{
		fEnable = FALSE;
	}

	// Compute the text to show.
	char cbText[2];
	if (fEnable)
	{
		ASSERT(pTab != NULL);
		if (pTab->m_DecimalCharacter == 0)
		{
			pTab->m_DecimalCharacter = '.';
		}
		cbText[0] = (char)pTab->m_DecimalCharacter;
	}
	else
	{
		cbText[0] = '\0';
	}
	cbText[1] = '\0';
	SetDlgItemText(IDC_DECIMAL_CHARACTER, cbText);

	GetDlgItem(IDC_DECIMAL_CHARACTER)->EnableWindow(fEnable);
}

void CTabsDialog::OnChangePosition() 
{
	SelectTab(GetPosition());
	UpdateButtons();
}

void CTabsDialog::OnTabLeader() 
{
	BOOL fEnable = IsDlgButtonChecked(IDC_LEADER_CUSTOM);

	CTextTab* pTab = GetCurrentTab();
	ASSERT(pTab != NULL);
	if (pTab == NULL)
	{
		fEnable = FALSE;
	}

	// Compute the text to show.
	char cbText[3];
	if (fEnable)
	{
		ASSERT(pTab != NULL);
		cbText[0] = (char)pTab->m_Leader[0];
		cbText[1] = (char)pTab->m_Leader[1];
		cbText[2] = '\0';
	}
	else
	{
		cbText[0] = '\0';
	}
	SetDlgItemText(IDC_LEADER_CHARACTER, cbText);

	GetDlgItem(IDC_LEADER_CHARACTER)->EnableWindow(fEnable);
}

void CTabsDialog::OnOK() 
{
	CPmwDialog::OnOK();

	// If this is the multiple paragraph case, add all the tabstop clears.
	if (m_fMultiple)
	{
		int nTabStopsToClear = m_cdaTabStopsToClear.GetSize();
		for (int nTab = 0; nTab < nTabStopsToClear; nTab++)
		{
			// Construct the tab.
			CTextTab Tab;
			memset(&Tab, 0, sizeof(Tab));
			Tab.m_nType = CTextTab::TAB_remove;
			Tab.m_Position = (CFixed)m_cdaTabStopsToClear.GetAt(nTab);

			// Add it at the beginning.
			m_Tabs.Insert(&Tab, 0);
		}
	}
}
