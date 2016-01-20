#if !defined(AFX_SPLSHTXT_H__19079882_0462_11D2_B06E_00A024EA69C0__INCLUDED_)
#define AFX_SPLSHTXT_H__19079882_0462_11D2_B06E_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SplshTxt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplashText dialog

class CSplashText : public CDialog
{
// Construction
public:
	CSplashText(CString csText, CBitmap& bitmap, COLORREF crBackColor, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CSplashText)
	enum { IDD = IDD_SPLASH_TEXT };
	CStatic	m_cStaticText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashText)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBitmap& m_cbBitmap;
	COLORREF m_crBkgndColor;
	CString m_csText;

	// Generated message map functions
	//{{AFX_MSG(CSplashText)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLSHTXT_H__19079882_0462_11D2_B06E_00A024EA69C0__INCLUDED_)
