#if !defined(AFX_NUMBEREDIT_H__D6A4DB63_E4EF_11D1_B06E_00A024EA69C0__INCLUDED_)
#define AFX_NUMBEREDIT_H__D6A4DB63_E4EF_11D1_B06E_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NumberEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNumberEdit window

class CNumberEdit : public CEdit
{
// Construction
public:
	CNumberEdit();
	void NumbersOnly(BOOL bNum){m_bNumOnly = bNum;}
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNumberEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNumberEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNumberEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	BOOL m_bNumOnly;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NUMBEREDIT_H__D6A4DB63_E4EF_11D1_B06E_00A024EA69C0__INCLUDED_)
