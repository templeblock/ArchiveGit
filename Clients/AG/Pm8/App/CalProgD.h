#if !defined(AFX_CALPROGD_H__C0188D31_60F3_11D1_90D6_00A0240C7400__INCLUDED_)
#define AFX_CALPROGD_H__C0188D31_60F3_11D1_90D6_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CalProgD.h : header file
//

#include "pdlgclr.h"

/////////////////////////////////////////////////////////////////////////////
// CCalendarProgressDlg dialog

class CCalendarProgressDlg : public CPmwDialogColor
{
// Construction
public:
	CCalendarProgressDlg(int, CString strTitle, COLORREF cr = INVALID_DIALOG_COLOR, CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(CCalendarProgressDlg)
	enum { IDD = IDD_CALENDAR_PROGRESS };
	CProgressCtrl	m_ctlProgress;
	CStatic	m_ctlPercent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalendarProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void Step();

	int GetStep (void)
	{ return m_steps; }
	void SetStep (const int nSteps)
	{ m_steps = nSteps; }

	void SetRange (const int nlow, const int nHigh)
	{ m_ctlProgress.SetRange (nlow, nHigh); }

protected:
	
	int m_steps;
	int m_current;
	CString m_strTitle;
	// Generated message map functions
	//{{AFX_MSG(CCalendarProgressDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALPROGD_H__C0188D31_60F3_11D1_90D6_00A0240C7400__INCLUDED_)
