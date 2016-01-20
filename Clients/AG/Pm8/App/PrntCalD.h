#if !defined(AFX_PRNTCALD_H__437D7C21_5543_11D1_90C9_00A0240C7400__INCLUDED_)
#define AFX_PRNTCALD_H__437D7C21_5543_11D1_90C9_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PrntCalD.h : header file
//

#include "pmwprint.h"
#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPrintCalendarDlg dialog

class CPrintCalendarDlg : public CPmwPrint
{
// Construction
public:
	CPrintCalendarDlg(CPmwView* pView, int nID = IDD);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPrintCalendarDlg)
	enum { IDD = IDD_PRINT_CALENDAR };
	BOOL	m_fDoubleSided;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintCalendarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrintCalendarDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPrintSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void update_buttons();

};

 
#endif // !defined(AFX_PRNTCALD_H__437D7C21_5543_11D1_90C9_00A0240C7400__INCLUDED_)
