#if !defined(AFX_ADDCALENDARPAGEDIALOG_H__A489E3B1_4971_11D1_90B4_00A0240C7400__INCLUDED_)
#define AFX_ADDCALENDARPAGEDIALOG_H__A489E3B1_4971_11D1_90B4_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AddCalendarPageDialog.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CAddCalendarPageDialog dialog

class CAddCalendarPageDialog : public CDialog
{
// Construction
public:
	CAddCalendarPageDialog(short iPages, BOOL bPicture, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddCalendarPageDialog)
	enum { IDD = IDD_ADD_CALENDAR_PAGE };
	CComboBox	m_ctlCopyPage;
	int		m_iAddNumber;
	
	int		m_iContents;
	int		m_iWhere;
	CString	m_strPageNumber;
	//}}AFX_DATA
	int		m_iCopyPage;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddCalendarPageDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddCalendarPageDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeCopypageCmbo();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	
	void update_page_control();
	short m_iPages;
	BOOL m_bPicture;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDCALENDARPAGEDIALOG_H__A489E3B1_4971_11D1_90B4_00A0240C7400__INCLUDED_)
