// tbardlgs.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#include "octools.h"

/////////////////////////////////////////////////////////////////////////////
// CMacroListBox window
/////////////////////////////////////////////////////////////////////////////
class OCCustom;

struct OCCONTROL
{
	UINT 	nID;
	int		CaptionID;
	LPTSTR	ClassName;
	UINT	Style;
	int		w;
	int		h;
} ;

class CMacroListBox : public CListBox
{
// Construction
public:
	CMacroListBox();

// Attributes
public:

public:
	OCCustom* 	m_pDlg;

// Implementation
public:
	virtual ~CMacroListBox();

protected:
	// Generated message map functions
	//{{AFX_MSG(CMacroListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTBList window
/////////////////////////////////////////////////////////////////////////////

class CTBList : public CListBox
{
// Construction
public:
	CTBList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTBList)
	public:
	virtual void  DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) ;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTBList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTBList)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	int		m_ItemHeight;
};

/////////////////////////////////////////////////////////////////////////////
// TBarDlgList dialog
/////////////////////////////////////////////////////////////////////////////

class TBarDlgList : public CDialog
{
// Construction
public:
	TBarDlgList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TBarDlgList)
	enum { IDD = IDD_TOOLBARS };
	CTBList	m_tbarlist;
	BOOL	m_ColorIcons;
	BOOL	m_ToolTips;
	//}}AFX_DATA
	CString		m_Delete;
	CString		m_Reset;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TBarDlgList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TBarDlgList)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCustomize();
	afx_msg void OnNew();
	afx_msg void OnOption();
	afx_msg void OnDelReset();
	afx_msg void OnSelchangeList();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// OCButtonSelector
/////////////////////////////////////////////////////////////////////////////

class OCButtonSelector
{
public:
	OCButtonSelector(CRect r, CSize Size);
	~OCButtonSelector();
	inline COCButton* GetButtonPtr(int nIndex) const;
	inline COCButton** GetButtonPtrPtr(int nIndex) const;
	BOOL BuildBitmap();
	void DeleteButtons();
	BOOL AllocElements(int nElements);
	BOOL FillMacro(CListBox*	pList);
	BOOL SetButtons(CWnd* pParent, const OCBUTTON* lpIDArray, int nIDCount);
	COCButton* CreateWinButton(int index, CWnd* pParent);
	BOOL HitTest(CPoint Pt, CRect &r);
	BOOL Draw(CDC	*pPaintDC);
	void StartMacroDrag(int Sel, CWnd *pOwner, CPoint Pt, CRect r);
	void StartDrag(CWnd *pOwner, CPoint Pt);
	void MoveDrag(CPoint Pt);
	void EndDrag(CPoint Pt);
	void ExecuteDrag();
public:
	COCButton**	m_pData;
	int			m_nCount;
	CSize		m_Size;			// of image
	COCBitmap	m_OCBM;
	CRect		m_Area;
	COCButton  *m_Selected;
};


/////////////////////////////////////////////////////////////////////////////
// OCCustom dialog
/////////////////////////////////////////////////////////////////////////////

class OCCustom : public CDialog
{
// Construction
public:
	OCCustom(CWnd* pParent = NULL);   // standard constructor
	~OCCustom();
	void DoMacroDrag(CPoint Pt);
	void EnableMacros(BOOL f);
	void FillMacroList(int Sel);
	int GetDragMode();
// Dialog Data
	//{{AFX_DATA(OCCustom)
	enum { IDD = IDD_OCCustom };
	CListBox		m_Cat;
	CMacroListBox	m_Macros;
	OCButtonSelector	*pButtonSelector;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OCCustom)
	public:
	virtual BOOL Create();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillCategory(int Sel);

	// Generated message map functions
	//{{AFX_MSG(OCCustom)
	afx_msg void OnSelchangeCat();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CNameToolBar : public CDialog
{
// Construction
public:
	CNameToolBar(CString *Name, COCToolBar*	pBar = NULL);	// standard constructor
	~CNameToolBar();

// Dialog Data
	//{{AFX_DATA(CNameToolBar)
	enum { IDD = IDD_RENAMECUSTOOL };
	CEdit	    m_ToolName;
	CString*	m_pNameVal;
	COCToolBar*		m_pBar;
	BOOL		m_LargeIcons;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CNameToolBar)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLargeButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
