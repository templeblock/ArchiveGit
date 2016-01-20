/*
// $Header: /PM8/Toolbars/PrintShop/BARCOMBO.CPP 1     3/03/99 6:35p Gbeddow $
//
// Combo box and edit for dialog bars.
//
// $Log: /PM8/Toolbars/PrintShop/BARCOMBO.CPP $
// 
// 1     3/03/99 6:35p Gbeddow
// 
// 1     2/24/99 12:24p Ddusich
// Print Shop Toolbar DLL
// 
// 1     1/20/99 9:55a Mwilson
// 
// 1     12/11/98 11:04a Mwilson
// 
// 4     3/27/98 10:33a Rlovejoy
// Renamed UpdateMRU() function.
// 
// 3     3/25/98 12:07p Rlovejoy
// No more time to fight with ON_CONTROL_REFLECT. OnKillFocus is called
// from view to update MRU.
// 
// 2     3/24/98 3:22p Rlovejoy
// Changes for CBarMRUCombo class.
// 
//    Rev 1.0   14 Aug 1997 15:18:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:24:56   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:26:26   FRED
// Initial revision.
// 
//    Rev 1.1   29 Mar 1995 09:17:54   JAY
// Lots of changes
// 
//    Rev 1.0   15 Mar 1995 11:06:42   JAY
// Initial revision.
// 
*/

#include "stdafx.h"
#include "barcombo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/*
// This file implements a combo box which will sit on a dialog bar.
// 
// We want the following behavior:
// (1) The control gains the focus by the user clicking on either the edit
//     field or the down button of the combo box. (The initial contents of
//     the edit field are remembered.)
// (2) The user can type in a value in the edit field.
// (3) The user can choose a value from the drop-down list.
// (4) The list can be dropped down or closed up without the control losing
//     focus, except...
// (5) If the user clicks (and possibly drags) in the listbox and releases
//     within it, the value chosen becomes the "accepted" value, and the
//     focus is removed from the control.
// (6) If the user presses Enter, the value is accepted, and the control loses
//     focus.
// (7) If the user presses Escape or removes focus from the control, the
//     control's value reverts to what it was when the control initially
//     gained focus.
// (8) While editing the value, the following keys are in effect:
//     a) Up and down arrows: drop down the list if not dropped. Otherwise,
//        they move the highlight within the list. (*)
//     b) ALT-up and down arrows: toggles the dropped and closed state of the
//        drop-down list. (*)
//     c) Enter and escape as above.
//     d) Tab and backtab have no effect.
//
//     (*) this behavior is part of the standard for Windows 3.1.
//
// The functionality of this control is the same as the pointsize list in
// Word 6.0.
*/

/////////////////////////////////////////////////////////////////////////////
// CBarEdit

CBarEdit::CBarEdit()
{
}

CBarEdit::~CBarEdit()
{
}

BEGIN_MESSAGE_MAP(CBarEdit, CEdit)
	//{{AFX_MSG_MAP(CBarEdit)
	ON_WM_GETDLGCODE()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarEdit message handlers

UINT CBarEdit::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS | CEdit::OnGetDlgCode();
}

void CBarEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
//	od("OnChar: %x\r\n", nChar);

	switch (nChar)
	{
		case VK_RETURN:
		case VK_TAB:
		case VK_ESCAPE:
		{
			break;
		}
		default:
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

void CBarEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
//	od("OnKeyDown: %x\r\n", nChar);

	switch (nChar)
	{
		case VK_RETURN:
		case VK_TAB:
		case VK_ESCAPE:
		case VK_UP:
		case VK_DOWN:
		{
			CWnd* pParent;
			if ((pParent = GetParent()) != NULL)
			{
			/* Just pass it straight. */
				if (ComboBox()->EditKeyStroke(nChar, nRepCnt, nFlags))
				{
					return;
				}
			}
		}
		default:
		{
			break;
		}
	}
// Do the standard thing.
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CBarEdit::OnKillFocus(CWnd* pNewWnd)
{
	ComboBox()->OnEditKillFocus(pNewWnd);
	CEdit::OnKillFocus(pNewWnd);
}

void CBarEdit::OnSetFocus(CWnd* pOldWnd)
{
	ComboBox()->OnEditSetFocus(pOldWnd);
	CEdit::OnSetFocus(pOldWnd);
}

WNDPROC* CBarEdit::GetSuperWndProcAddr()
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}

/////////////////////////////////////////////////////////////////////////////
// CBarCombo

CBarCombo::CBarCombo()
{
}

CBarCombo::~CBarCombo()
{
}

BEGIN_MESSAGE_MAP(CBarCombo, CComboBox)
	//{{AFX_MSG_MAP(CBarCombo)
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_LBN_SELCHANGE(1000, OnSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarCombo message handlers

BOOL CBarCombo::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (INHERITED::Create(dwStyle, rect, pParentWnd, nID))
	{
		HWND hWndChild = ::GetWindow(GetSafeHwnd(), GW_CHILD);
		if (hWndChild != NULL)
		{
			m_Edit.SubclassWindow(hWndChild);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CBarCombo::EditKeyStroke(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL fHandled = TRUE;

	switch (nChar)
	{
		case VK_TAB:
		{
		// Do nothing.
			break;
		}
		case VK_RETURN:
		{
			ConfirmState();
			ShowDropDown(FALSE);
			break;
		}
		case VK_ESCAPE:
		{
			AbortState();
			ShowDropDown(FALSE);
			break;
		}
		case VK_UP:
		case VK_DOWN:
		{
			if (!GetDroppedState())
			{
				ShowDropDown(TRUE);
			}
			else
			{
				fHandled = FALSE;
			}
			break;
		}
		default:
		{
			fHandled = FALSE;
			break;
		}
	}
	return fHandled;
}

void CBarCombo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CBarCombo::OnEditKillFocus(CWnd* pNewWnd)
{
	if (!m_fAccepted)
	{
		AbortState(FALSE);
	}
}

void CBarCombo::OnEditSetFocus(CWnd* pOldWnd)
{
	m_fAccepted = FALSE;

	SaveState();
	if (pOldWnd != this)
	{
		m_hOldWnd = pOldWnd->GetSafeHwnd();
	}
}

void CBarCombo::SaveState(void)
{
	GetWindowText(m_SavedState);
}

void CBarCombo::AbortState(BOOL fSetFocus /*=TRUE*/)
{
	SetWindowText(m_SavedState);
	if (fSetFocus)
	{
		::SetFocus(m_hOldWnd);
	}
}

void CBarCombo::ConfirmState(void)
{
	m_fAccepted = TRUE;
	::SetFocus(m_hOldWnd);
}

BOOL CBarCombo::HasFocus(void)
{
	return GetFocus() == &m_Edit;
}

void CBarCombo::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);
}

void CBarCombo::OnSetFocus(CWnd* pOldWnd)
{
	m_hOldWnd = pOldWnd->GetSafeHwnd();

	CComboBox::OnSetFocus(pOldWnd);
}

void CBarCombo::OnSelChange()
{
	Default();
	if (!GetDroppedState())
	{
	/* We are no longer open! */
		ConfirmState();
	}
}

WNDPROC* CBarCombo::GetSuperWndProcAddr()
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}

/////////////////////////////////////////////////////////////////////////////
// CBarMRUCombo

CBarMRUCombo::CBarMRUCombo(UINT nStart, LPCTSTR lpszSection,
		LPCTSTR lpszEntryFormat, int nSize,
		int nMaxDispLen /*= AFX_ABBREV_FILENAME_LEN*/) :
	CBarCombo(),
	m_RecentItemList(nStart, lpszSection, lpszEntryFormat, nSize, nMaxDispLen)
{
	m_nCurSel = CB_ERR;
}

BOOL CBarMRUCombo::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (INHERITED::Create(dwStyle, rect, pParentWnd, nID))
	{
		// Read MRU list from ini file & determine # of valid entries
		m_nMRUItems = 0;
		m_RecentItemList.ReadList();
		while (!m_RecentItemList[m_nMRUItems].IsEmpty())
		{
			if (++m_nMRUItems == m_RecentItemList.GetSize())
				break;
		}
		return TRUE;
	}
	return FALSE;
}

CBarMRUCombo::~CBarMRUCombo()
{
	// Write the MRU list
	m_RecentItemList.WriteList();
}

void CBarMRUCombo::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{  
	// if we're not the ones drawing, bail out!
	if (lpMeasureItemStruct->CtlType != ODT_COMBOBOX)
		return;

	// Set the Combo box dimensions based on the font
	CClientDC cdcWnd(this);				
	CFont* pcfWnd = GetFont();			
	if (pcfWnd != NULL)
	{
		CFont* pcfOld = cdcWnd.SelectObject(pcfWnd);
				
		if (pcfOld != NULL)
		{
			TEXTMETRIC tmFont;
					
			if (cdcWnd.GetTextMetrics(&tmFont))
			{
				CRect crClient;
				GetClientRect(crClient);
						
				lpMeasureItemStruct->itemWidth = crClient.Width();
				lpMeasureItemStruct->itemHeight = tmFont.tmHeight;
			}					
			cdcWnd.SelectObject(pcfOld);
		}
	}

}

BEGIN_MESSAGE_MAP(CBarMRUCombo, CBarCombo)
	//{{AFX_MSG_MAP(CBarMRUCombo)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBarMRUCombo message handlers

void CBarMRUCombo::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if (((LONG)(lpDrawItemStruct->itemID) >= 0) &&
		(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
	{
		CString cs;
		GetLBText(lpDrawItemStruct->itemID, cs);

		// Draw highlighted or unhighlighted text
		COLORREF newTextColor = ::GetSysColor(COLOR_WINDOWTEXT);  // light gray
		COLORREF oldTextColor = pDC->SetTextColor(newTextColor);

		COLORREF newBkColor = ::GetSysColor(COLOR_WINDOW);
		COLORREF oldBkColor = pDC->SetBkColor(newBkColor);

		if (newTextColor == newBkColor)
			newTextColor = RGB(0xC0, 0xC0, 0xC0);   // dark gray

		if ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0)
		{
			pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
		}

		pDC->ExtTextOut(
			lpDrawItemStruct->rcItem.left,
			lpDrawItemStruct->rcItem.top, 
			ETO_OPAQUE,
			&(lpDrawItemStruct->rcItem), 
			cs,
			cs.GetLength(),
			NULL);

		pDC->SetTextColor(oldTextColor);
		pDC->SetBkColor(oldBkColor);
	}

	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
		pDC->DrawFocusRect(&(lpDrawItemStruct->rcItem));
	
	// Draw the separator line with the last MRU item
	if ((int)lpDrawItemStruct->itemID == m_nMRUItems-1)
	{
		RECT rcItem = lpDrawItemStruct->rcItem;
		pDC->SelectStockObject(BLACK_PEN);
		pDC->MoveTo(rcItem.left, rcItem.bottom-1);
		pDC->LineTo(rcItem.right, rcItem.bottom-1);
	}
}

void CBarMRUCombo::UpdateMRU() 
{
	// Update the MRU list if selection is not from the MRU list
	int nCurSel = GetCurSel();
	if (nCurSel != m_nCurSel && nCurSel != CB_ERR)
	{
		int i;
		BOOL bSelOnMRU = FALSE;
		DWORD dwSelData = GetItemData(GetCurSel());
		for (i = 0; i < m_nMRUItems; i++)
		{
			if ((bSelOnMRU = (dwSelData == GetItemData(i))))
				break;
		}

		// Put at the top of the list.
		// If not found on MRU, add it
		CString cs;
		GetLBText(GetCurSel(), cs);
		if (bSelOnMRU)
		{
			// Remove (for now)
			DeleteString(i);
			m_RecentItemList.Remove(i);
			m_nMRUItems--;
		}

		// Insert at the top
		InsertString(0, cs);
		SetItemData(0, dwSelData);

		// Add it to the MRU
		m_RecentItemList.Add(cs);
		if (++m_nMRUItems > m_RecentItemList.GetSize())
		{
			m_nMRUItems = m_RecentItemList.GetSize();

			// Remove the last item
			DeleteString(m_nMRUItems);
		}
	}
	m_nCurSel = nCurSel;

}


