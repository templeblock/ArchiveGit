// fdmap.h : header file
//

#define FDM_WIDTH		161
#define FDM_HEIGHT	129

/////////////////////////////////////////////////////////////////////////////
// CFdMap window

class CFdMap : public CWnd
{
// Construction
public:
	CFdMap(HWND pParent,RECT rect);

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CFdMap();
	virtual void DrawMap(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CFdMap)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
