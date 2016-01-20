// tilesbtn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTilesButton window

class CTilesButton : public CButton
{
protected:
	BOOL	m_captured;
// Construction
public:
	CTilesButton();

// Attributes
public:

	enum
	{
		UWM_CHILDBUTTONDOWN	= WM_USER+1000,
		UWM_CHILDMOUSEMOVE	= WM_USER+1001,
		UWM_CHILDBUTTONUP		= WM_USER+1002
	};

// Operations
public:

// Implementation
public:
	virtual ~CTilesButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTilesButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
