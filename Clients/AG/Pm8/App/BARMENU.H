// barmenu.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBarMenu window

class CBarMenu : public CDialogBar
{
// Construction
public:
	CBarMenu();

// Attributes
public:

// Operations
private:
	BOOL create_from_menu(CWnd* pParentWnd, UINT nIDMenu, UINT nStyle, UINT nID);
public:

/* Overridden Create method (not virtual, but redefined). */

	BOOL Create(CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID);
	
// Implementation
public:
	virtual ~CBarMenu();

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Generated message map functions
protected:
	UINT m_nIDMenu;
	LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	
	//{{AFX_MSG(CBarMenu)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
