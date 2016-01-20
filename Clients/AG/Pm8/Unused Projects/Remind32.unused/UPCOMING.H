/*
// $Workfile: UPCOMING.H $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/UPCOMING.H $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:25:02   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:34   Fred
// Initial revision.
*/

class CUpcomingEventCheckedListBoxItem;
class CUpcomingEventCheckedListBox;
class CUpcomingEventsDialog;

#ifndef __UPCOMING_H__
#define __UPCOMING_H__

#include "event.h"


/////////////////////////////////////////////////////////////////////////////
// CUpcomingEventCheckedListBoxItem
//

class CUpcomingEventCheckedListBoxItem : public CCheckedListBoxItem
{

public:

	CUpcomingEventCheckedListBoxItem(CEvent* pEvent, int nIndent = 0);
	virtual ~CUpcomingEventCheckedListBoxItem();
	
public:

	CEvent* GetEvent(void) const
		{ return m_pEvent; }
	
protected:

	CEvent* m_pEvent;
	
protected:

	virtual int CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct);
	
	virtual void PaintItem(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
	virtual BOOL IsFocused(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
};


/////////////////////////////////////////////////////////////////////////////
// CUpcomingEventCheckedListBox
//

class CUpcomingEventCheckedListBox : public CCheckedListBox
{
// Construction
public:
	CUpcomingEventCheckedListBox(DWORD dwStyle = 0);

// Attributes
public:

// Operations
public:
	virtual void MarkItem(CCheckedListBoxItem* pItem, int nIndex, BOOL fMark);
	
	CUpcomingEventCheckedListBoxItem* GetListBoxItem(int nIndex) const;
	CEvent* GetEvent(int nIndex) const;
	void Rebuild(void);
	
protected:
	virtual BOOL RemoveSelectIfNotFocused(void);

// Implementation
public:
	virtual ~CUpcomingEventCheckedListBox();

protected:
	// Generated message map functions
	//{{AFX_MSG(CEventCheckedListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CUpcomingEventsDialog dialog

class CUpcomingEventsDialog : public CDialog
{
// Construction
public:
	CUpcomingEventsDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CUpcomingEventsDialog)
	enum { IDD = IDD_UPCOMING_EVENTS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CUpcomingEventsDialog)
	virtual BOOL OnInitDialog();
	afx_msg int OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex);
	afx_msg void OnEventReminderList();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	virtual void OnCancel();
	
public:
	BOOL GetSaveEvents(void) const
		{ return m_fSaveEvents; }
	
protected:
	void UpdateInterface(void);
	CUpcomingEventCheckedListBox m_EventList;
	BOOL m_fSaveEvents;
};

#endif
