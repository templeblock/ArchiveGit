/*
// $Workfile: PMWWIZ.H $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/PMWWIZ.H $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:56   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:26   Fred
// Initial revision.
*/

#ifndef WIN32
class CPmwWizardPropertyPage;
class CPmwWizardTabItem;
class CPmwWizardTabControl;
class CPmwWizardPropertySheet;
#endif
class CPmwWizardBitmap;
class CPmwWizardPage;
class CPmwWizard;

#ifdef WIN32
#define	PROPERTY_SHEET_CLASS	CPropertySheet
#define	PROPERTY_PAGE_CLASS	CPropertyPage
#define	TAB_CONTROL_CLASS		CTabCtrl
#else
#define	PROPERTY_SHEET_CLASS	CPmwWizardPropertySheet
#define	PROPERTY_PAGE_CLASS	CPmwWizardPropertyPage
#define	TAB_CONTROL_CLASS		CPmwWizardTabControl
#define	ID_WIZBACK  			0x3023
#define	ID_WIZNEXT				0x3024
#define	ID_WIZFINISH  			0x3025
#define PSWIZB_BACK				0x000000001L
#define PSWIZB_NEXT				0x000000002L
#define PSWIZB_FINISH			0x000000004L
#define PSWIZB_DISABLEDFINISH	0x000000008L
#endif

#ifndef __PMWWIZ_H__
#define __PMWWIZ_H__

#ifndef WIN32
////////////////////////////////////////////////////////////////////////////
// CPmwWizardPropertyPage -- one page of a tabbed dialog

class CPmwWizardPropertyPage : public CDialog
{
	DECLARE_DYNAMIC(CPmwWizardPropertyPage)

// Construction
public:
	CPmwWizardPropertyPage(UINT nIDTemplate, UINT nIDCaption = 0);
	CPmwWizardPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);

// Operations
public:
	void CancelToClose();           // called when the property sheet should display close instead of cancel
	// lets the property sheet activate the apply now button
	void SetModified(BOOL bChanged = TRUE);

// Overridables
public:
	virtual BOOL OnSetActive();     // called when this page gets the focus
	virtual BOOL OnKillActive();    // perform validation here
	virtual void OnOK();            // ok or apply now pressed -- KillActive is called first
	virtual void OnCancel();        // cancel pressed

// Implementation
public:
	virtual ~CPmwWizardPropertyPage();
	virtual BOOL PreTranslateMessage(MSG* pMsg); // handle tab, enter, and escape keys
	
protected:
	CString m_strCaption;
	BOOL m_bChanged;

	void CommonConstruct(LPCTSTR lpszTemplateName, UINT nIDCaption);
		// loads the resource indicated by CDialog::m_lpDialogTemplate
	BOOL PreTranslateKeyDown(MSG* pMsg);
	BOOL ProcessTab(MSG* pMsg); // handles tab key from PreTranslateMessage
	BOOL CreatePage();  // called from CPmwWizardPropertySheet to create the dialog
						// by loading the dialog resource into memory and
						// turning off WS_CAPTION before creating
	void LoadCaption();
		// gets the caption of the dialog from the resource and puts it in m_strCaption

	// Generated message map functions
	//{{AFX_MSG(CPmwWizardPropertyPage)
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpcs);
	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual LRESULT OnWizardBack(void);
	virtual LRESULT OnWizardNext(void);
	virtual BOOL OnWizardFinish(void);
	virtual void OnPaletteChanged(CWnd* pFocusWnd);
	virtual BOOL OnQueryNewPalette();
	
	friend class CPmwWizardPropertySheet;
};

////////////////////////////////////////////////////////////////////////////
// CPmwWizardTabItem  is used only in CPmwWizardTabControl

class CPmwWizardTabItem : public CObject
{
public:
	CPmwWizardTabItem(LPCTSTR szCaption, int nWidth);
	void Draw(CDC* pDC, HFONT hFont, BOOL bCurTab);

	CString m_strCaption;
	CRect   m_rect;
	CRect   m_rectPrev;
	int     m_nWidth;
};

////////////////////////////////////////////////////////////////////////////
// CPmwWizardTabControl -- internal use only
//  Implementation for a generic row of tabs along the top of dialog
//  Future versions of MFC may or may not include this exact class.

// TCN_ messages are tab control notifications
#define TCN_TABCHANGING     1
#define TCN_TABCHANGED      2

class CPmwWizardTabControl : public CWnd
{
	DECLARE_DYNAMIC(CPmwWizardTabControl)

public:
// Construction
	CPmwWizardTabControl();

// Attributes
	BOOL m_bInSize;
	int m_nHeight;
	BOOL SetCurSel(int nTab, BOOL fNotify = FALSE);
	int GetCurSel() const
		{ return m_nCurTab; }
	int GetItemCount() const
		{ return m_tabs.GetSize(); }

// Operations
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	void AddTab(LPCTSTR lpszCaption);
	void RemoveTab(int nTab);

// Implementation
public:
	virtual ~CPmwWizardTabControl();
	BOOL NextTab(BOOL bNext);

protected:
	void Scroll(int nDirection);
	void ScrollIntoView(int nTab);
	void DrawFocusRect(CDC* pDC = NULL);
	void InvalidateTab(int nTab, BOOL bInflate = TRUE);
	int TabFromPoint(CPoint pt);
	void Capture(int nDirection);
	void LayoutTabs(int nTab);

	enum
	{
		SCROLL_LEFT = -5,       // all the SCROLL_ items must be less
		SCROLL_RIGHT = -6,      // than -1 to avoid ID conflict
		SCROLL_NULL = -7,
		TIMER_ID = 15,          // timer constants
		TIMER_DELAY = 500
	};

	void DrawScrollers(CDC* pDC);

	BOOL CanScroll();
	void SetFirstTab(int nTab);
	CPmwWizardTabItem* GetTabItem(int nTab) const
		{ return (CPmwWizardTabItem*)(m_tabs.GetAt(nTab)); }
	BOOL IsTabVisible(int nTab, BOOL bComplete = FALSE) const;

	// Member variables
	HFONT m_hBoldFont;
	HFONT m_hThinFont;
	CRect m_rectScroll; // location of scroll buttons
	int m_nCurTab;      // index of current selected tab
	int m_nFirstTab;    // index of leftmost visible tab
	int m_nScrollState; // shows whether left or right scroll btn is down
	BOOL m_bScrollPause;// if we have capture, has the mouse wandered off btn?

	CPtrArray m_tabs;   // list of CPmwWizardTabItems, in order

	// Generated message map functions
	//{{AFX_MSG(CPmwWizardTabControl)
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////
// CPmwWizardPropertySheet -- a tabbed "dialog" (really a popup-window)

class CPmwWizardPropertySheet : public CWnd
{
	DECLARE_DYNAMIC(CPmwWizardPropertySheet)

// Construction
public:
	CPmwWizardPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL,
		UINT iSelectPage = 0);
	CPmwWizardPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL,
		UINT iSelectPage = 0);

	// for modeless creation
	BOOL Create(CWnd* pParentWnd = NULL, DWORD dwStyle =
		WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | WS_VISIBLE,
		DWORD dwExStyle = WS_EX_DLGMODALFRAME);

	void SetWizardMode();
	void SetWizardButtons(DWORD dwFlags);

// Attributes
public:
	int GetPageCount() const
		{ return m_pages.GetSize(); }
	CPmwWizardPropertyPage* GetPage(int nPage) const
		{ return (CPmwWizardPropertyPage*)(m_pages.GetAt(nPage)); }

// Operations
public:
	int DoModal();
	void AddPage(CPmwWizardPropertyPage* pPage);
	void RemovePage(CPmwWizardPropertyPage* pPage);
	void RemovePage(int nPage);
	void EndDialog(int nEndID); // used to terminate a modal dialog

// Implementation
public:
	virtual ~CPmwWizardPropertySheet();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	BOOL SetActivePage(int nPage);

protected:
	HWND FindNextControl(HWND hWnd, TCHAR ch);
	void GotoControl(HWND hWnd, TCHAR ch);
	BOOL ProcessChars(MSG* pMsg);
	BOOL ProcessTab(MSG* pMsg);
	BOOL CreateStandardButtons();
	BOOL PumpMessage();
	void PageChanged();
	void CancelToClose();
	void CommonConstruct(CWnd* pParent, UINT iSelectPage);
	void RecalcLayout();
	CPmwWizardPropertyPage* GetActivePage() const
		{ return GetPage(m_nCurPage); }
	void CheckDefaultButton(HWND hFocusBefore, HWND hFocusAfter);
	void CheckFocusChange();

	// implementation data members
	HFONT m_hFont;          // sizes below dependent on this font
	CSize m_sizeButton;
	CSize m_sizeTabMargin;
	int m_cxButtonGap;
	BOOL m_bModeless;

	int m_nCurPage;
	int m_nID;              // ID passed to EndDialog and returned from DoModal

	CPtrArray m_pages;      // array of CPmwWizardPropertyPage pointers
	HWND m_hWndDefault;     // current default push button if there is one
	HWND m_hFocusWnd;       // focus when we lost activation
	HWND m_hLastFocus;      // tracks last window with focus
	CWnd* m_pParentWnd;     // owner of the tabbed dialog
	CString m_strCaption;   // caption of the pseudo-dialog
//	CPmwWizardTabControl m_tabRow;   // entire row of tabs at top of dialog
	BOOL m_bParentDisabled; // TRUE if parent was disabled by DoModal

	// Generated message map functions
	//{{AFX_MSG(CPmwWizardPropertySheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnCancel();
	afx_msg void OnWizardBack();
	afx_msg void OnWizardNext();
	afx_msg LRESULT OnGetFont(WPARAM, LPARAM);
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnQueryNewPalette();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	DWORD m_dwWizardButtons;

	friend class CPmwWizardPropertyPage; // for tab handler
};
#endif

/////////////////////////////////////////////////////////////////////////////
// CPmwWizardBitmap
// This control is a window that gets its bitmap from the parent wizard.

class CPmwWizardBitmap : public CWnd
{
	INHERIT(CPmwWizardBitmap, CWnd)
	
public:
// Construction
	CPmwWizardBitmap();

// Implementation
public:
	virtual ~CPmwWizardBitmap();

protected:
	// Generated message map functions
	//{{AFX_MSG(CPmwWizardBitmap)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void Initialize(UINT nID, CWnd* pParent);

protected:
	void DrawBorder(CDC& dc, CRect& crRect);

protected:
	virtual void DrawBitmap(CDC* pDC);
};

/////////////////////////////////////////////////////////////////////////////
// CPmwWizard - base class for PMW wizards.

class CPmwWizard : public PROPERTY_SHEET_CLASS
{
	INHERIT(CPmwWizard, PROPERTY_SHEET_CLASS)
	
public:
	CPmwWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPmwWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CPmwWizard();

public:
	virtual CBitmap* GetBitmap(void);
	virtual void InitDialog(void);

protected:
	// Generated message map functions
	//{{AFX_MSG(CPmwWizard)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPmwWizardPage - this is the base class for all pages in a PMW wizard.

class CPmwWizardPage : public PROPERTY_PAGE_CLASS
{
	INHERIT(CPmwWizardPage, PROPERTY_PAGE_CLASS)

// Construction
public:
	CPmwWizardPage(UINT nIDTemplate, UINT nIDCaption = 0);
	CPmwWizardPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDoubleSidedPrintWizardPage)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPmwWizardPage();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPmwWizardPage)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnPaletteChanged(CWnd* pFocusWnd);
	virtual BOOL OnQueryNewPalette();
	
public:
	// Inherited access functions.
	CPmwWizard* GetWizard(void)
		{ return (CPmwWizard*)GetParent(); }
	void SetWizardButtons(DWORD dwFlags)
		{ GetWizard()->SetWizardButtons(dwFlags); }
	CBitmap* GetBitmap(void)
		{ return GetWizard()->GetBitmap(); }
protected:
	CPmwWizardBitmap m_Bitmap;
};

#endif
