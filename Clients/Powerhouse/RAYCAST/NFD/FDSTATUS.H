// fdstatus.h : header file
//

#define FDS_WIDTH		160
#define FDS_HEIGHT	100

/////////////////////////////////////////////////////////////////////////////
// CFdStatus window

class CFdStatus : public CWnd
{
// Construction
public:
	CFdStatus(HWND pParent,RECT rect);

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CFdStatus();
	virtual void TextHealth(void);
	virtual void TextAmmo(void);
	virtual void TextMsg(char *msg);
	virtual void DrawKeys(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CFdStatus)
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
