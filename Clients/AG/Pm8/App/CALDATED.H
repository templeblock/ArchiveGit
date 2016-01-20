#if !defined(AFX_CALDATED_H__E12A8BA2_4E6A_11D1_90C4_00A0240C7400__INCLUDED_)
#define AFX_CALDATED_H__E12A8BA2_4E6A_11D1_90C4_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CalDateD.h : header file
//
#include "pdlgclr.h"
#include "calinfo.h"

/////////////////////////////////////////////////////////////////////////////
// CCalendarDateDlg dialog

class CCalendarDateDlg : public CPmwDialogColor
{
// Construction
public:
	CCalendarDateDlg(CALENDAR_INFO_REF info, BOOL bFull = TRUE, DWORD flags = WIZBUT_OKCAN, COLORREF cr = INVALID_DIALOG_COLOR, short n_o_c = 1, CWnd* pParent = NULL);   // standard constructor

//Dialog Data
	//{{AFX_DATA(CCalendarDateDlg)
	enum { IDD = IDD_CALENDAR_STARTDATE };
	CEdit	m_ctlEdNumber;
	CStatic	m_ctlYearText;
	CStatic	m_ctlWeekText;
	CSpinButtonCtrl	m_ctlSpinYear;
	CSpinButtonCtrl	m_ctlSpinNumber;
	CStatic	m_ctlMonthText;
	CComboBox	m_ctlWeek;
	CStatic	m_ctlNumOfX;
	CButton	m_btnNow;
	CComboBox	m_ctlMonth;
	int		m_iMonth; // month
	int		m_iWeek; // week
	int		m_iYear; // year
	int		m_iNumObjects; // number of calendars to create
	//}}AFX_DATA

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalendarDateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCalendarDateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBack();
	afx_msg void OnFinish();
	afx_msg void OnCalwhenNow();
	afx_msg void OnSelchangeCalwhenMonth();
	afx_msg void OnChangeEditYear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  DWORD               m_WizFlags;
	CALENDAR_INFO				m_info;
	BOOL								m_bFull;
	void SetupDialogControls();
	void update_week_selection();

public:
	void get_exit_info(CALENDAR_INFO_REF info);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALDATED_H__E12A8BA2_4E6A_11D1_90C4_00A0240C7400__INCLUDED_)
