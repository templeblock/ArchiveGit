#if !defined(AFX_PSBUTTON_H__B555EF93_5307_11D2_BD0E_006008473DD0__INCLUDED_)
#define AFX_PSBUTTON_H__B555EF93_5307_11D2_BD0E_006008473DD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PSButton.h : header file
//

#define UM_MOUSEMOVE		(WM_USER + 10)
#define UM_MOUSEENTER		(WM_USER + 11)
#define UM_MOUSEEXIT		(WM_USER + 12)

/////////////////////////////////////////////////////////////////////////////
// CPSButton window

class CPSButton : public CButton
{
// Construction
public:
	CPSButton( BOOL fAutoDelete = FALSE );
	virtual ~CPSButton();

// Attributes
public:

// Operations
public:

	BOOL		Initialize( CArchive& ar, DWORD dwVersion );

	DWORD		GetUserData() { return m_dwUserData; }
	BOOL		GetInfoText( CString& str );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPSButton)
	//}}AFX_VIRTUAL

// Implementation
protected:

	void FireClick();
	void SetNextState( int nDir = 1 );

	// Generated message map functions
protected:
	//{{AFX_MSG(CPSButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	afx_msg LRESULT OnSetCheck( WPARAM, LPARAM );
	afx_msg LRESULT OnGetCheck( WPARAM, LPARAM );
	afx_msg LRESULT OnMouseEnter( WPARAM, LPARAM );
	afx_msg LRESULT OnMouseExit( WPARAM, LPARAM );

	DECLARE_MESSAGE_MAP()

private:

	BOOL		m_fAutoDelete;
	BOOL		m_fMouseDown;
	BOOL		m_fMouseCaptured;
	UINT		m_uiState;
	WORD		m_wStyle;
	DWORD		m_dwUserData;

	WORD		m_wInfoTextId;

	CImageList	m_cImageData;
	COLORREF	m_crTransColor;
	int			m_nImageCount;
	int			m_nState;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSBUTTON_H__B555EF93_5307_11D2_BD0E_006008473DD0__INCLUDED_)
