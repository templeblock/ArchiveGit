/*
// $Workfile: Eventrem.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/Eventrem.cpp $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:04a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:26   Fred
// Initial revision.
// 
//    Rev 1.1   26 Feb 1997 13:52:46   Fred
// Fixed some problems with sender books
// 
//    Rev 1.0   20 Feb 1997 13:55:48   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "remind.h"
#include "eventwiz.h"
#include "eventrem.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReminderListBoxItem

CReminderListBoxItem::CReminderListBoxItem(CEvent* pEvent) :
	CCheckedListBoxItem(0)
{
	m_pEvent = pEvent;
}

CReminderListBoxItem::~CReminderListBoxItem()
{
	m_pEvent = NULL;
}

void CReminderListBoxItem::ComputeBounds(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crCheckBox, CRect& crItem)
{
	CCheckedListBoxItem::ComputeBounds(pList, pDC, pDrawItemStruct, crCheckBox, crItem);
	
	// Get rid of check box.
	crItem.left = crCheckBox.left;
	crCheckBox.SetRectEmpty();
	
}

void CReminderListBoxItem::PaintItem(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	CEvent* pEvent = GetEvent();
	
	if ((pEvent != NULL) && (pEvent->IsValid()))
	{
		COLORREF clOldForeground = pDC->SetTextColor(clForeground);
		COLORREF clOldBackground = pDC->SetBkColor(clBackground);
		
		CString csText;
		CWnd* pTitle;
		
		TRY
		{
			// Draw the address book name.
			pTitle = pList->GetParent()->GetDlgItem(IDC_ADDRESS_BOOK_TITLE);
			if (pTitle != NULL)
			{
				// Compute the rectangle in which to draw the text.
				CRect crText;
				pTitle->GetWindowRect(crText);
				pList->ScreenToClient(crText);
				crText.top = crBounds.top+1;
				crText.bottom = crBounds.bottom;
				
				// Get the text to draw.
				csText = pEvent->GetAddressBookUserName();
				
				// Draw the text.
				pDC->DrawText(
					csText,
					csText.GetLength(),
					crText,
					DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_TOP);
			}
			
			// Draw the event type.
			pTitle = pList->GetParent()->GetDlgItem(IDC_EVENT_TITLE);
			if (pTitle != NULL)
			{
				// Compute the rectangle in which to draw the text.
				CRect crText;
				pTitle->GetWindowRect(crText);
				pList->ScreenToClient(crText);
				crText.top = crBounds.top+1;
				crText.bottom = crBounds.bottom;
				
				// Get the text to draw.
				csText = pEvent->GetEventType();
				
				// Draw the text.
				pDC->DrawText(
					csText,
					csText.GetLength(),
					crText,
					DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_TOP);
			}
				
			// Draw the event name.
			pTitle = pList->GetParent()->GetDlgItem(IDC_NAME_TITLE);
			if (pTitle != NULL)
			{
				// Compute the rectangle in which to draw the text.
				CRect crText;
				pTitle->GetWindowRect(crText);
				pList->ScreenToClient(crText);
				crText.top = crBounds.top+1;
				crText.bottom = crBounds.bottom;
				
				// Get the text to draw.
				csText = pEvent->GetName();
				
				// Draw the text.
				pDC->DrawText(
					csText,
					csText.GetLength(),
					crText,
					DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_TOP);
			}
			
			// Draw the number of deys before to remind.
			pTitle = pList->GetParent()->GetDlgItem(IDC_DAYS_TITLE);
			if (pTitle != NULL)
			{
				// Compute the rectangle in which to draw the text.
				CRect crText;
				pTitle->GetWindowRect(crText);
				pList->ScreenToClient(crText);
				crText.top = crBounds.top+1;
				crText.bottom = crBounds.bottom;
				
				// Get the text to draw.
				csText.Format("%d", pEvent->GetDays());
				
				// Draw the text.
				pDC->DrawText(
					csText,
					csText.GetLength(),
					crText,
					DT_CENTER|DT_NOPREFIX|DT_SINGLELINE|DT_TOP);
			}
		}
		END_TRY
		
		pDC->SetTextColor(clOldForeground);
		pDC->SetBkColor(clOldBackground);
	}
}

BOOL CReminderListBoxItem::IsFocused(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	return (pDrawItemStruct->itemState & ODS_FOCUS) != 0;
}


/////////////////////////////////////////////////////////////////////////////
// CReminderListBox

CReminderListBox::CReminderListBox() :
	CCheckedListBox()
{
}

CReminderListBox::~CReminderListBox()
{
}

BEGIN_MESSAGE_MAP(CReminderListBox, CCheckedListBox)
	//{{AFX_MSG_MAP(CReminderListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CEvent* CReminderListBox::GetEvent(int nIndex) const
{
	CReminderListBoxItem* pListBoxItem = (CReminderListBoxItem*)GetListBoxItem(nIndex);
	
	if (pListBoxItem == NULL)
	{
		return NULL;
	}
	
	return pListBoxItem->GetEvent();
}

// Rebuild the contents of the list box from the application's current event list.	
void CReminderListBox::Rebuild(CEvent* pSelectEvent /*=NULL*/)
{
	SetRedraw(FALSE);
	ResetContent();
	
	CReminderListBoxItem* pItem = NULL;
	
	TRY
	{
		for (int nEvent = 0; nEvent < CurrentEvents.GetSize(); nEvent++)
		{
			CEvent* pEvent = CurrentEvents[nEvent];
			if ((pEvent != NULL) && (pEvent->IsValid()))
			{
				pItem = new CReminderListBoxItem(pEvent);
				if (pItem != NULL)
				{
					if (AddString((LPCSTR)pItem) != LB_ERR)
					{
						// Item is now owned by list box.
						pItem = NULL;
					}
					
					// Delete the item if it wasn't used.
					delete pItem;
					pItem = NULL;
				}
			}
		}
	}
	END_TRY
	
	delete pItem;
	pItem = NULL;
	
	// Run through the items and select the one with the given event.
	if (pSelectEvent != NULL)
	{
		int nCount = GetCount();
		if (nCount != LB_ERR)
		{
			for (int nIndex = 0; nIndex < nCount; nIndex++)
			{
				if (GetEvent(nIndex) == pSelectEvent)
				{
					SetCurSel(nIndex);
					break;
				}
			}
		}
	}
		
	SetRedraw(TRUE);
	Invalidate();
}

BOOL CReminderListBox::RemoveSelectIfNotFocused(void)
{
	// Leave selected item highlighted, even if we don't have the focus.
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CReminderListBox message handlers


/////////////////////////////////////////////////////////////////////////////
// CEventReminderList dialog

CEventReminderList::CEventReminderList(CWnd* pParent /*=NULL*/)
	: CDialog(CEventReminderList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventReminderList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_fChanged = FALSE;
}

void CEventReminderList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventReminderList)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEventReminderList, CDialog)
	//{{AFX_MSG_MAP(CEventReminderList)
	ON_WM_CHARTOITEM()
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEventReminderList::UpdateButtons(void)
{
	CWnd* pWnd = NULL;
	
	BOOL fItemSelected = m_ReminderList.GetCurSel() != LB_ERR;
	
	if ((pWnd = GetDlgItem(IDC_NEW)) != NULL)
	{
		pWnd->EnableWindow(TRUE);
	}
	
	if ((pWnd = GetDlgItem(IDC_CHANGE)) != NULL)
	{
		if (!fItemSelected && (pWnd == GetFocus()))
		{
			NextDlgCtrl();
		}
		pWnd->EnableWindow(fItemSelected);
	}
	
	if ((pWnd = GetDlgItem(IDC_DELETE)) != NULL)
	{
		if (!fItemSelected && (pWnd == GetFocus()))
		{
			NextDlgCtrl();
		}
		pWnd->EnableWindow(fItemSelected);
	}
	
	if ((pWnd = GetDlgItem(IDCANCEL)) != NULL)
	{
		pWnd->EnableWindow(TRUE);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CEventReminderList message handlers

BOOL CEventReminderList::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetConfiguration()->ReplaceDialogText(this);
	CenterWindow();
	 
	m_ReminderList.SubclassDlgItem(IDC_LIST, this);
	m_ReminderList.Rebuild();
	
	UpdateButtons();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

int CEventReminderList::OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex)
{
	if (pListBox == &m_ReminderList)
	{
		return m_ReminderList.DoCharToItem(nChar, nIndex);
	}
	
	return CDialog::OnCharToItem(nChar, pListBox, nIndex);
}

void CEventReminderList::OnSelchangeList()
{
	// The user clicked in the box, make sure the interface is up to date.
	UpdateButtons();
}

void CEventReminderList::OnNew()
{
	// Add a new event.
	CEvent* pEvent = CEventWizard::Run(NULL);
	if (pEvent != NULL)
	{	
		// Add the event to the current event reminder list. Any events
		// which are very similar (same except for number of days) are
		// automatically deleted.
		GETAPP()->AddEvent(*pEvent);
				
		// Update the interface
		m_ReminderList.Rebuild(pEvent);
		SetChanged();
	}
	
	UpdateButtons();
}

void CEventReminderList::OnChange()
{
	// Change an existing event.
	int nIndex = m_ReminderList.GetCurSel();
	if (nIndex != LB_ERR)
	{
		CReminderListBoxItem* pItem = (CReminderListBoxItem*)(m_ReminderList.GetItemData(nIndex));
		if (pItem != NULL)
		{
			CEvent* pOldEvent = pItem->GetEvent();
			if (pOldEvent != NULL)
			{
				// Run the wizard to get the new event.
				CEvent* pEvent = CEventWizard::Run(NULL, pOldEvent);
				if (pEvent != NULL)
				{
					// Delete the old event.
					GETAPP()->DeleteEvent(*pOldEvent);
					
					// Add in the new event,
					GETAPP()->AddEvent(*pEvent);
					
					// Update the interface
					m_ReminderList.Rebuild(pEvent);
					SetChanged();
				}
			}
		}
	}
	
	UpdateButtons();
}

void CEventReminderList::OnDelete()
{
	int nIndex = m_ReminderList.GetCurSel();
	if (nIndex != LB_ERR)
	{
		CReminderListBoxItem* pItem = (CReminderListBoxItem*)(m_ReminderList.GetItemData(nIndex));
		if (pItem != NULL)
		{
			CEvent* pEvent = pItem->GetEvent();
			if (pEvent != NULL)
			{
				// Delete the event.
				GETAPP()->DeleteEvent(*pEvent);
				
				// Update the interface
				m_ReminderList.Rebuild();
				SetChanged();
			}
		}
	}
	
	UpdateButtons();
}
