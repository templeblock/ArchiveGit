/*
// $Workfile: Upcoming.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/Upcoming.cpp $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:04a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:42   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:08   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "remind.h"
#include "event.h"
#include "eventrem.h"
#include "upcoming.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpcomingEventCheckedListBoxItem

CUpcomingEventCheckedListBoxItem::CUpcomingEventCheckedListBoxItem(CEvent* pEvent, int nIndent /*=0*/) :
	CCheckedListBoxItem(nIndent)
{
	m_pEvent = pEvent;
}

CUpcomingEventCheckedListBoxItem::~CUpcomingEventCheckedListBoxItem()
{
	m_pEvent = NULL;
}

int CUpcomingEventCheckedListBoxItem::CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	int nResult = 0;
	
	// Get our event.
	CEvent* pEvent = GetEvent();
	
	// Get the event we're comparing against.
	DWORD dwCompareEvent = pCompareItemStruct->itemData2;
	if (dwCompareEvent != (DWORD)-1)
	{
		CUpcomingEventCheckedListBoxItem* pCompareItem = (CUpcomingEventCheckedListBoxItem*)dwCompareEvent;
		if (pCompareItem != NULL)
		{
			CEvent* pCompareEvent = pCompareItem->GetEvent();
			
			// Make sure both events are valid.
			if (pEvent->IsValid() && pCompareEvent->IsValid())
			{
				// We sort the events chronologically (nearest events first.)
				nResult = pEvent->GetDays()-pCompareEvent->GetDays();
			}
		}
	}
	
	return nResult;
}
	
void CUpcomingEventCheckedListBoxItem::PaintItem(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	CEvent* pEvent = GetEvent();
	
	if ((pEvent != NULL) && (pEvent->IsValid()))
	{
		CString csEvent;
		if (pEvent->GetEventString(csEvent))
		{
			COLORREF clOldForeground = pDC->SetTextColor(clForeground);
			COLORREF clOldBackground = pDC->SetBkColor(clBackground);
		
			pDC->TextOut(crBounds.left,  crBounds.top+1, csEvent,  csEvent.GetLength());
			
			pDC->SetTextColor(clOldForeground);
			pDC->SetBkColor(clOldBackground);
		}
	}
}

BOOL CUpcomingEventCheckedListBoxItem::IsFocused(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	return (pDrawItemStruct->itemState & ODS_FOCUS) != 0;
}

/////////////////////////////////////////////////////////////////////////////
// CUpcomingEventCheckedListBox

CUpcomingEventCheckedListBox::CUpcomingEventCheckedListBox(DWORD dwStyle /*=0*/) :
	CCheckedListBox(dwStyle)
{
}

CUpcomingEventCheckedListBox::~CUpcomingEventCheckedListBox()
{
}

BEGIN_MESSAGE_MAP(CUpcomingEventCheckedListBox, CCheckedListBox)
	//{{AFX_MSG_MAP(CUpcomingEventCheckedListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CUpcomingEventCheckedListBox::MarkItem(CCheckedListBoxItem* pItem, int nIndex, BOOL fMark)
{
	CCheckedListBox::MarkItem(pItem, nIndex, fMark);
	
	if (pItem != NULL)
	{
		CEvent* pEvent = ((CUpcomingEventCheckedListBoxItem*)pItem)->GetEvent();
		if ((pEvent != NULL) && (pEvent->IsValid()))
		{
			int nIndex = DisabledEvents.Find(pEvent);
			
			// Check if the event is being marked as disabled.
			if (fMark)
			{
				// The event is being disabled. Add it to the disabled list if it doesn't exist.
				if (nIndex == -1)
				{
					// Create a copy of the event, since this event is owned by the
					// upcoming events list.
					CEvent* pDisabledEvent = NULL;
					TRY
					{
						pDisabledEvent = new CEvent(*pEvent);
						DisabledEvents.Add(pDisabledEvent);
						pDisabledEvent = NULL;
						TRACE("Add disabled event\n");
					}
					END_TRY
					
					// delete the disabled event if we did not use it.
					delete pDisabledEvent;
					pDisabledEvent = NULL;
				}
			}
			else
			{
				// The event is being unmarked (enabled). Delete it from the
				// disabled event list.
				if (nIndex != -1)
				{
					DisabledEvents.Delete(pEvent);
					TRACE("Delete disabled event\n");
				}
			}
		}
	}
}

CUpcomingEventCheckedListBoxItem* CUpcomingEventCheckedListBox::GetListBoxItem(int nIndex) const
{
	return (CUpcomingEventCheckedListBoxItem*)(CCheckedListBox::GetListBoxItem(nIndex));
}

CEvent* CUpcomingEventCheckedListBox::GetEvent(int nIndex) const
{
	CUpcomingEventCheckedListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
	
	if (pListBoxItem == NULL)
	{
		return NULL;
	}
	
	return pListBoxItem->GetEvent();
}

// Rebuild the contents of the list box from the application's upcoming event list.	
void CUpcomingEventCheckedListBox::Rebuild(void)
{
	SetRedraw(FALSE);
	ResetContent();
	
	CUpcomingEventCheckedListBoxItem* pItem = NULL;
	
	TRY
	{
		for (int nEvent = 0; nEvent < UpcomingEvents.GetSize(); nEvent++)
		{
			CEvent* pEvent = UpcomingEvents[nEvent];
			if ((pEvent != NULL) && (pEvent->IsValid()))
			{
				// Create the list box item.
				pItem = new CUpcomingEventCheckedListBoxItem(pEvent);
				if (pItem != NULL)
				{
					// Add the event to the event list box.
					int nIndex = AddString((LPCSTR)pItem);
					if (nIndex != LB_ERR)
					{
						// If the item is disabled, mark.
						if (GETAPP()->IsEventDisabled(pEvent))
						{
							MarkItem(pItem, nIndex, TRUE);
						}
						
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
	
	SetRedraw(TRUE);
	Invalidate();
}

BOOL CUpcomingEventCheckedListBox::RemoveSelectIfNotFocused(void)
{
	// Leave selected item highlighted, even if we don't have the focus.
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CEventCheckedListBox message handlers


/////////////////////////////////////////////////////////////////////////////
// CUpcomingEventsDialog dialog

CUpcomingEventsDialog::CUpcomingEventsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CUpcomingEventsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUpcomingEventsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_fSaveEvents = TRUE;
}

void CUpcomingEventsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpcomingEventsDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUpcomingEventsDialog, CDialog)
	//{{AFX_MSG_MAP(CUpcomingEventsDialog)
	ON_WM_CHARTOITEM()
	ON_BN_CLICKED(IDC_EVENT_REMINDER_LIST, OnEventReminderList)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CUpcomingEventsDialog::UpdateInterface(void)
{
	// Check if we have any events to report. We morph the interface
	// based upon whether we have any.
	BOOL fHaveUpcomingEvents = UpcomingEvents.GetSize() > 0;
	
	// Set the dialog title.
	CString csTitle;
	TRY
	{
		csTitle.LoadString(fHaveUpcomingEvents ? IDS_EVENTS_TITLE : IDS_NO_EVENTS_TITLE);
		SetWindowText(csTitle);
	}
	END_TRY
	
	// Set the reminder text item.
	CWnd* pReminder = GetDlgItem(IDC_REMINDER);
	if (pReminder != NULL)
	{
		CString csReminder;
		TRY
		{
			csReminder.LoadString(fHaveUpcomingEvents ? IDS_EVENTS_REMINDER : IDS_NO_EVENTS_REMINDER);
			pReminder->SetWindowText(csReminder);
		}
		END_TRY
	}
	
	// Check if we have a nearest upcoming event. This further defines the desired interface.
	BOOL fHaveNearestUpcomingEvent = NearestUpcomingEvent->IsValid();
	
	// Show or hide various elements based on the interface we want.
	CWnd* pListHint = GetDlgItem(IDC_LIST_HINT);
	CWnd* pNextEvent = GetDlgItem(IDC_NEXT_EVENT);
	CWnd* pNextEventBorder = GetDlgItem(IDC_NEXT_EVENT_BORDER);
	CWnd* pHint1 = GetDlgItem(IDC_NO_EVENTS_HINT1);
	CWnd* pHint2 = GetDlgItem(IDC_NO_EVENTS_HINT2);

	m_EventList.ShowWindow(fHaveUpcomingEvents);
	
	if (pListHint != NULL)
	{
		pListHint->ShowWindow(fHaveUpcomingEvents);
	}
	
	if (pNextEvent != NULL)
	{
		pNextEvent->ShowWindow(!fHaveUpcomingEvents && fHaveNearestUpcomingEvent);
	}
	
	if (pNextEventBorder != NULL)
	{
		pNextEventBorder->ShowWindow(!fHaveUpcomingEvents && fHaveNearestUpcomingEvent);
	}
	
	if (pHint1 != NULL)
	{
		pHint1->ShowWindow(!fHaveUpcomingEvents);
	}
	
	if (pHint2 != NULL)
	{
		pHint2->ShowWindow(!fHaveUpcomingEvents);
	}
	
	// If we have upcoming events, rebuild the list box from the upcoming event list.
	if (fHaveUpcomingEvents)
	{
		// Show the upcoming events.
		m_EventList.Rebuild();
	}
	else
	{
		// Show the nearest upcoming event.
		if (fHaveNearestUpcomingEvent)
		{
			CString csEvent;
			if (NearestUpcomingEvent->GetEventString(csEvent))
			{
				pNextEvent->SetWindowText(csEvent);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CUpcomingEventsDialog message handlers

BOOL CUpcomingEventsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetConfiguration()->ReplaceDialogText(this);
	
	CenterWindow();
	
	m_EventList.SubclassDlgItem(IDC_LIST, this);
	
	UpdateInterface();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUpcomingEventsDialog::OnCancel()
{
	CDialog::OnCancel();
}

int CUpcomingEventsDialog::OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex)
{
	if (pListBox == &m_EventList)
	{
		return m_EventList.DoCharToItem(nChar, nIndex);
	}
	
	return CDialog::OnCharToItem(nChar, pListBox, nIndex);
}

void CUpcomingEventsDialog::OnEventReminderList()
{
	// Bring up the event reminder list for editing.
	CEventReminderList ReminderList;
	ReminderList.DoModal();
	if (ReminderList.GetChanged())
	{
		GETAPP()->RebuildUpcomingEvents();
		UpdateInterface();
	}
}

void CUpcomingEventsDialog::OnClose()
{
	// Check if the user wants to save any changes to their events.
	int nResult = AfxMessageBox(IDS_SAVE_EVENTS, MB_YESNOCANCEL);
	
	if (nResult == IDCANCEL)
	{
		// User does not want to close at thie time.
		return;
	}
	
	// Set the save events flag.
	m_fSaveEvents = (nResult == IDYES);
	
	// Go ahead and close.
	CDialog::OnClose();
}

