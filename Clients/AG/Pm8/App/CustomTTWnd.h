#if !defined(AFX_CUSTOMTTWND_H__1E4A7A63_A423_11D2_B98D_00A02416AB46__INCLUDED_)
#define AFX_CUSTOMTTWND_H__1E4A7A63_A423_11D2_B98D_00A02416AB46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomTTWnd.h : header file
//

#include "transbmp.h"

/////////////////////////////////////////////////////////////////////////////
// Constants

const int		kDelayTime		= 1000;	// 1 second delay
const int		kDurTime			= 2000;		// 2 second duration

const CString	kDefTipFont		= "Arial";
const int		kDefFontSize	= 8;
const COLORREF	kDefFontColor	= RGB( 0, 0, 0 );

const COLORREF	kCrTransColor	= RGB(   255,  255,   255 );


// timer ids
#define TM_SHOWTIP	WM_USER + 1000
#define TM_HIDETIP	WM_USER + 1001


/////////////////////////////////////////////////////////////////////////////
// CTipObj

class CTipObj : public CObject
{
// Construction
public:
	CTipObj	(CWnd* pWnd, CString& theText, int index);
	~CTipObj	();

	CWnd*		GetTipCtrlWnd() const { return m_pCtrlWnd; } 
	int		GetIndex() const { return m_nIndex; }

	void		SetTipText(CString& theText) { m_strCtrlTipText = theText; }
	CString	GetTipText() const{ return m_strCtrlTipText; } 

private:
	CWnd*		m_pCtrlWnd;
	CString	m_strCtrlTipText;
	int		m_nIndex;
};

/////////////////////////////////////////////////////////////////////////////
// CCustomTTWnd window

class CCustomTTWnd : public CWnd
{
// Construction
public:
	CCustomTTWnd();
	virtual ~CCustomTTWnd();

// Operations
public:
	BOOL	Create(	CWnd*		pParentWnd,
						int		nBitmapID,
						LPCRECT	rcMargin,
						BOOL		bActivate = TRUE,
						CString	strTipFont = kDefTipFont,
						int		nTipFontSize = kDefFontSize);
	void	RelayEvent(MSG* pMsg) ;

// Attributes
public:

	BOOL	AddTool(CWnd *pWnd, CString tipText) ;
	BOOL	AddTool(CWnd *pWnd);
	void	DelTool(CWnd *pWnd);
	int	GetToolCount() { return m_nToolCount; }

	int	GetDelayTime() { return m_nShowDelay; }
	int	GetDurationTime() { return m_nHideDelay; }

	void	SetDelayTime(int time) { m_nShowDelay = time; }
	void	SetDurationTime(int time) { m_nHideDelay = time; }

	void	Activate(BOOL bActivate) { m_bActivate = bActivate; }

	void	SetTipFont(CString strTipFont, int nTipFontSize);
	void	UpdateTipText( LPCTSTR lpszText, CWnd* pWnd );
	void	GetText( CString& str, CWnd* pWnd ) const;
	void	SetTipTextColor( COLORREF clr ) { m_crTextColor = clr; }
	COLORREF GetTipTextColor( ) const { return m_crTextColor; }


	void	GetMargin( LPRECT lprc ) const { *lprc = m_rcMargin; }
	void	SetMargin( LPRECT lprc ) { m_rcMargin = *lprc; }

private:
	void			AdjustWindowRect();
	void			HandleMouseMove(POINT point);
	void			ResetTip();
	void			SetTip(CString theString);

	void			Show();
	void			Hide();
	void			NewMousePos();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomTTWnd)
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	//{{AFX_MSG(CCustomTTWnd)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL			m_bActivate;

	CPoint		m_LastMouseMovePoint;

	CString		m_strTip;
	CRect			m_textRect;
	CFont*		m_pTipFont;
	COLORREF		m_crTextColor;

	CWnd*			m_pParentCtrl;
	static LPCSTR m_pszWndClass; // Registered class name
	
	int			m_nShowDelay;
	int			m_nHideDelay;

	UINT			m_nTimerId;
	UINT			m_nHideTimerId;

	CRect			m_rcDisplayRect;

	int			m_nBitmapID;
	CRect			m_rcMargin;
	CTransBmp	*m_pTransbmpBubble;
	CSize			m_bmpSize;

	CPtrList		*m_pTipObjList;
	CTipObj		*m_pCurTipObj;
	int			m_nToolCount;
	int			m_nCurToolIndex;


};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMTTWND_H__1E4A7A63_A423_11D2_B98D_00A02416AB46__INCLUDED_)
