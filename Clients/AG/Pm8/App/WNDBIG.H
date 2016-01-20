// WndBig.h : header file
// Handles sizing, black background

/////////////////////////////////////////////////////////////////////////////
// CWndBig window

#ifndef  WNDBIG_H
#define  WNDBIG_H
/////////////////////////////////////////////////////////////////////////////
class CWndBig : public CWnd
{
// Construction
public:
	CWndBig (COLORREF cr);

// Attributes
public:

// Operations
public:

   virtual void
   SizeToParent (void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndBig)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, CWnd* pParentWnd, UINT id = (UINT)-1, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWndBig();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndBig)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

   COLORREF Color;

   void
   GetRect (CRect *pRect);
};

/////////////////////////////////////////////////////////////////////////////
#endif
