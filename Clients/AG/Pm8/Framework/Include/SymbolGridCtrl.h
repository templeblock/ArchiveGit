// SymbolGridCtrl.h : header file
//
#ifndef _SYMBOLGRIDCTRL_H
#define _SYMBOLGRIDCTRL_H

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// for the grid
const int kNumGridColumns = 16;
const int kNumGridRows = 14;
const int kTotalNumSymbols = kNumGridColumns * kNumGridRows; // columns * rows
const int kSymbolCharStart = 161;

const char kSymbolFont[] = "PressWriter Symbols";

#define UM_SYMBOL_CHANGED		WM_USER + 100
#define UM_SYMBOL_DBLCLICKED	WM_USER + 101


/////////////////////////////////////////////////////////////////////////////
// RSymbolGridCtrl window

class FrameworkLinkage RSymbolGridCtrl : public CStatic
{
// Construction
public:
	RSymbolGridCtrl(int numCol = kNumGridColumns, int numRow = kNumGridRows, 
					BYTE charSet = SYMBOL_CHARSET, CString fontName = "PressWriter Symbols");
	virtual ~RSymbolGridCtrl();


// Operations
private:
    void    HighlightSymbol( int nSymbol );
	void	HandleArrowKeys( WORD theKey );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RSymbolGridCtrl)
	//}}AFX_VIRTUAL

 
	// Generated message map functions
protected:
	//{{AFX_MSG(RBitmapStatic)
	virtual void PreSubclassWindow( );
	afx_msg void OnPaint();
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point); 
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point); 
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg void OnSetFocus( CWnd* pOldWnd );
	afx_msg UINT OnGetDlgCode( );
	//}}AFX_MSG

private:
	int		m_nNumCol;
	int		m_nNumRow;
	int		m_nTotalNumSym;
	BYTE	m_charSet;
	CString	m_fontName;

    int     m_symbolChar;
    CFont   m_symbolFont;

    CRect   m_rectArray[kTotalNumSymbols];
    CRect   m_gridRect;
    int     m_rectWidth;
	int		m_rectHeight;

	int		m_nSelIndex;


	DECLARE_MESSAGE_MAP()
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif  // SYMBOLGRIDCTRL_H