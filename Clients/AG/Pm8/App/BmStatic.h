#if !defined(AFX_BMSTATIC_H__EF57AAA2_1025_11D2_B06E_00A024EA69C0__INCLUDED_)
#define AFX_BMSTATIC_H__EF57AAA2_1025_11D2_B06E_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BmStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBmpStatic window

class CBmpStatic : public CStatic
{
// Construction
public:
	CBmpStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBmpStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBmpStatic)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMSTATIC_H__EF57AAA2_1025_11D2_B06E_00A024EA69C0__INCLUDED_)
