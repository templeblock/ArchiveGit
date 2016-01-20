/*
// $Workfile: SPINBUTT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SPINBUTT.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:31:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:34   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:33:52   JAY
// Reverted. New series.
// 
//    Rev 1.1   04 Aug 1994 08:30:58   JAY
// Large model and 32-bit changes
// 
//    Rev 1.0   15 Jun 1994 09:22:52   FRED
// Initial revision.
*/ 

#ifndef _SPINBUTT_H_
#define _SPINBUTT_H_

/////////////////////////////////////////////////////////////////////////////
// CSpinButton window

class CSpinButton : public CWnd
{
// flags and constants
public:

	// Control state flags
	enum
	{
		STATE_grayed		=	0x0001,
		STATE_hidden		=	0x0002,
		STATE_mouseout		=	0x0004,
		STATE_upclick		=	0x0008,
		STATE_downclick	=	0x0010,
		STATE_leftclick	=	0x0008,	// repeated since MSS_VERTICAL and
		STATE_rightclick	=	0x0010,	// MSS_HORIZONTAL are exclusive

		// Combination of click states
		STATE_clicked		=	(STATE_leftclick | STATE_rightclick),

		// Combination of state flags
		STATE_all			=	0x001f
	};
	
	// Timer identifiers
	enum
	{
		IDT_FIRSTCLICK		=	500,
		IDT_HOLDCLICK		=	501
	};
	
	// Timing constants
	enum
	{
		CTICKS_FIRSTCLICK	=	400,
		CTICKS_HOLDCLICK	=	50,
	};

	// Default range and position constants
	enum
	{
		DEFAULT_MIN			=	0,
		DEFAULT_MAX			=	9,
		DEFAULT_POS			=	5
	};
	
	// Notification codes sent via WM_COMMAND from the control
	enum
	{
		SBN_ASSOCIATEGAIN	=	1,
		SBN_ASSOCIATELOSS	=	2,
		SBN_RANGECHANGE	=	3
	};

	// Color indices for CSB_COLORSET/GET and SBColorSet/Get
	enum
	{
		SBCOLOR_FACE		=	0,
		SBCOLOR_ARROW		=	1,
		SBCOLOR_SHADOW		=	2,
		SBCOLOR_HIGHLIGHT	=	3,
		SBCOLOR_FRAME		=	4,
		CCOLORS				=	5
	};
	
	// Control specific styles
	enum
	{
		SBS_VERTICAL		=	0x0001,
		SBS_HORIZONTAL		=	0x0002,
		SBS_NOPEGSCROLL	=	0x0004,
		SBS_TEXTHASRANGE	=	0x0008,
		SBS_INVERTRANGE	=	0x0010
	};
	
// Construction
public:
	CSpinButton();

// Implementation
public:
	virtual ~CSpinButton();

protected:
	WORD StateSet(WORD wFlags)		{ return (m_wState |= wFlags); }
	WORD StateClear(WORD wFlags)	{ return (m_wState &= ~wFlags); }
	WORD StateTest(WORD wFlags)	{ return (m_wState & wFlags) != 0; }
	
	void Draw3DButtonRect(CDC *pcdcDraw, CPen *pcpHighlight, CPen *pcpShadow, int x1, int y1, int x2, int y2, BOOL fClicked);
	BOOL ParseText(LPSTR lpszText, LPINT pnMin, LPINT pnMax, LPINT pnPos);
	WORD TranslateUpToChar(LPSTR FAR *lplpszText, char cDelimeter);
	void ClickedRectCalc(CRect &crRect);
	void PositionChange(void);

protected:
	// Generated message map functions
	//{{AFX_MSG(CSpinButton)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCancelMode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	CWnd *m_pcwAssociate;			// Associate Window
	DWORD m_dwStyle;					// Copy of GetWindowLong(GWL_STYLE)
	int m_nMin;							// Minimum position
	int m_nMax;							// Maximum position
	int m_nPos;							// Current position
	WORD m_wState;						// State flags
	COLORREF m_crColor[CCOLORS];	// Configurable colors.

// stuff for supporting creation by class name
public:
	// Member function to register a window class
	static BOOL RegisterMyClass();

protected:	
	static WNDPROC m_pfnSuperWndProc ;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
private:		
	static LONG CALLBACK AFX_EXPORT FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);

	BOOL m_fCreatedByClassName ;
	virtual void PostNcDestroy();
};

/////////////////////////////////////////////////////////////////////////////

#endif
