#if !defined(AFX_MONTHEDIT_H__D6A4DB64_E4EF_11D1_B06E_00A024EA69C0__INCLUDED_)
#define AFX_MONTHEDIT_H__D6A4DB64_E4EF_11D1_B06E_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MonthEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMonthEdit window

class CMonthEdit : public CEdit
{
// Construction
public:
	CMonthEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonthEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMonthEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMonthEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONTHEDIT_H__D6A4DB64_E4EF_11D1_B06E_00A024EA69C0__INCLUDED_)
