#pragma once

// ColorControl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorControl window

class CColorControl : public CButton
{
// Construction
public:
	CColorControl();

// Attributes
public:
	COLORREF m_lColor;
	CBrush* m_pBrush;

// Operations
public:
	void SetRGBColor(COLORREF lColor);
	void SetLABColor(double* pLab);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorControl)
//	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) { return; }
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorControl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorControl)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
