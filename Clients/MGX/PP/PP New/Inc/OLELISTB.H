// olelistb.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COlelistBox window

class COlelistBox : public CListBox
{
// Construction
public:
	COlelistBox();

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~COlelistBox();

protected:
	// Generated message map functions
	//{{AFX_MSG(COlelistBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
