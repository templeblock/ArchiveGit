/*
// $Header: /PM8/App/BORDER/POPUP.H 1     3/03/99 6:03p Gbeddow $
//
// popup.h : Pop-up window classes
//
// $Log: /PM8/App/BORDER/POPUP.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 1     4/22/98 11:44a Fredf
*/

#ifndef __POPUP_H__
#define __POPUP_H__

#define UNDEFINED_COLOR	((COLORREF)-1)
#define TRANSPARENT_COLOR ((COLORREF)-2)

/////////////////////////////////////////////////////////////////////////////
// CPopupWnd : Base popup window class

class CPopupWnd : public CWnd
{
public:
	CPopupWnd();

   BOOL Create(CRect& r, CWnd* pParent);

protected:
	// Generated message map functions
	//{{AFX_MSG(CPopupWnd)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnCancel(void);

	static CFont m_Font;				// The font for general use.
	virtual CFont* GetPopupFont(void);
	void GetPopupFontMetrics(TEXTMETRIC& tm);
};

/////////////////////////////////////////////////////////////////////////////
// CPopupPalette : Popup palette window class

class CPopupPalette : public CPopupWnd
{
public:
	CPopupPalette();

   virtual BOOL Create(CWnd* pParent, UINT uID = 0);

	virtual BOOL SetPalette(HPALETTE hPalette);

	CPoint ItemCount(void)
		{ return m_cpItemCount; }
	CSize ItemSize(void)
		{ return m_czItemSize; }

	virtual void InitItems(CWnd* pParent) = 0;
	virtual void DrawItemBackground(CDC* pDC) = 0;
	virtual void DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected) = 0;

	virtual void OnOK(void);
	virtual void OnMore(void);
	virtual void OnColor(void);

	void SelectItem(CPoint cpItem);			// By x, y
	void SelectItem(int nSelection);			// By index

	// Special item numbers.
	enum
	{
		ITEM_None = 1000,
		ITEM_More,
		ITEM_Color,
	};

protected:
	// Generated message map functions
	//{{AFX_MSG(CPopupPalette)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	// Variables set by InitItems();
	CPoint m_cpItemCount;
	CSize m_czItemSize;
	BOOL m_fHasNone;
	BOOL m_fHasMore;
	BOOL m_fHasColor;

	// Other variables.
	CRect m_crItems;
	CRect m_crNone;			// Rect for "none" button.
	CRect m_crMore;			// Rect for "more" button.
	CRect m_crColor;			// Rect for "color" button.

	int m_nSelectedItem;

	HPALETTE m_hPalette;

	UINT m_uID;					// The ID for the WM_COMMAND message.

	void RedrawItem(int nItem, CDC* pDC = NULL);
	void NotifyOwner(void);
	virtual void DrawString(CDC* pDC, CString& csText, CRect crDraw, int nMethod);		// Utility function
	virtual void DrawNone(CDC* pDC, CRect crDraw);		// Utility function
	virtual void DrawMore(CDC* pDC, CRect crDraw);		// Utility function
	virtual void DrawColor(CDC* pDC, CRect crDraw);		// Utility function

	virtual int SizeNone(int nDefaultHeight);
	virtual int SizeMore(int nDefaultHeight);
	virtual int SizeColor(int nDefaultHeight);

	CPoint ItemToXY(int nItem);
	int XYToItem(CPoint p);
};

/////////////////////////////////////////////////////////////////////////////
// CColorPalette : Color palette window class

class CColorPalette : public CPopupPalette
{
public:
	CColorPalette();

   virtual BOOL Create(CWnd* pParent, UINT uID = 0);

	virtual void InitItems(CWnd* pParent);
	virtual void DrawItemBackground(CDC* pDC);
	virtual void DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected);

	COLORREF GetColor(void);
	void SetColor(COLORREF Color);

	BOOL IsNone(void);
	void SetNone(void);

	virtual void OnOK(void);
	virtual void OnMore(void);

protected:
	// Generated message map functions
	//{{AFX_MSG(CColorPalette)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	static COLORREF m_Colors[];
	COLORREF m_Color;				// The current color.
};

#endif
