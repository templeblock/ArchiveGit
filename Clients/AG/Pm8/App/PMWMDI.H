// pmwmdi.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPmwMDIChild frame

class CPmwMDIChild : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CPmwMDIChild)
protected:
	CPmwMDIChild();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
	virtual ~CPmwMDIChild();

	virtual void ActivateFrame(int nCmdShow = -1);

	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
					CWnd* pParentWnd, CCreateContext* pContext = NULL);

	// Generated message map functions
	//{{AFX_MSG(CPmwMDIChild)
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
