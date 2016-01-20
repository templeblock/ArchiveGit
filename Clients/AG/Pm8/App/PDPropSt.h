#if !defined(AFX_PDPROPST_H__69A641A0_FA31_11D1_B06E_00A024EA69C0__INCLUDED_)
#define AFX_PDPROPST_H__69A641A0_FA31_11D1_B06E_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PDPropSt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPDPropSheet

class CPDPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPDPropSheet)

// Construction
public:
	CPDPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPDPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPDPropSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPDPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPDPropSheet)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PDPROPST_H__69A641A0_FA31_11D1_B06E_00A024EA69C0__INCLUDED_)
