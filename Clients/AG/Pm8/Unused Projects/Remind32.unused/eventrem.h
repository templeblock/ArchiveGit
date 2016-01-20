/*
// $Workfile: eventrem.h $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/eventrem.h $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:50   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:16   Fred
// Initial revision.
*/

#ifndef __EVENTREM_H__
#define __EVENTREM_H__

#include "event.h"


/////////////////////////////////////////////////////////////////////////////
// CReminderListBoxItem
//

class CReminderListBoxItem : public CCheckedListBoxItem
{

public:

	CReminderListBoxItem(CEvent* pEvent);
	virtual ~CReminderListBoxItem();
	
public:

	CEvent* GetEvent(void) const
		{ return m_pEvent; }
	
protected:

	CEvent* m_pEvent;

protected:

	virtual void ComputeBounds(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crCheckBox, CRect& crItem);
	virtual void PaintItem(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
	virtual BOOL IsFocused(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
};


/////////////////////////////////////////////////////////////////////////////
// CReminderListBox
//

class CReminderListBox : public CCheckedListBox
{
// Construction
public:
	CReminderListBox();

// Attributes
public:

// Operations
public:
	CEvent* GetEvent(int nIndex) const;
	void Rebuild(CEvent* pSelectEvent = NULL);

protected:
	virtual BOOL RemoveSelectIfNotFocused(void);

// Implementation
public:
	virtual ~CReminderListBox();

protected:
	// Generated message map functions
	//{{AFX_MSG(CReminderListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
};


/////////////////////////////////////////////////////////////////////////////
// CEventReminderList dialog

class CEventReminderList : public CDialog
{
// Construction
public:
	CEventReminderList(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEventReminderList)
	enum { IDD = IDD_EVENT_REMINDER_LIST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CEventReminderList)
	virtual BOOL OnInitDialog();
	afx_msg int OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex);
	afx_msg void OnNew();
	afx_msg void OnChange();
	afx_msg void OnDelete();
	afx_msg void OnSelchangeList();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:	
	void UpdateButtons(void);
	
public:
	void SetChanged(BOOL fChanged = TRUE)
		{ m_fChanged = fChanged; }
	BOOL GetChanged(void) const
		{ return m_fChanged; }

protected:
	CReminderListBox m_ReminderList;
	BOOL m_fChanged;
};

#endif
