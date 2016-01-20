#if !defined(AFX_FIRSTDIALOG_H__26D794E4_B734_11D1_9EE4_006008947D80__INCLUDED_)
#define AFX_FIRSTDIALOG_H__26D794E4_B734_11D1_9EE4_006008947D80__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FirstDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFirstDialog dialog

class CFirstDialog : public CDialog
{
// Construction
public:
	CFirstDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFirstDialog)
	enum { IDD = IDD_FISRT_PATCH };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFirstDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFirstDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIRSTDIALOG_H__26D794E4_B734_11D1_9EE4_006008947D80__INCLUDED_)
