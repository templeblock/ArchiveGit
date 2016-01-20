//{{AFX_INCLUDES()
#include "calendar.h"
//}}AFX_INCLUDES
#if !defined(AFX_PROJECTCALENDAR_H__2E0B01D1_6522_11D2_9B93_00A0C99F6B3C__INCLUDED_)
#define AFX_PROJECTCALENDAR_H__2E0B01D1_6522_11D2_9B93_00A0C99F6B3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectCalendar.h : header file
//

#include "ProjectDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CProjectCalendarDlg dialog

class CProjectCalendarDlg : public CProjectDialog
{
// Construction
public:
	CProjectCalendarDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjectCalendarDlg)
	enum { IDD = IDD_PROJECT_CALENDAR_TYPE };
	CCalendar	m_cCalendar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectCalendarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjectCalendarDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNext();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CBitmap m_cBitmap;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTCALENDAR_H__2E0B01D1_6522_11D2_9B93_00A0C99F6B3C__INCLUDED_)
