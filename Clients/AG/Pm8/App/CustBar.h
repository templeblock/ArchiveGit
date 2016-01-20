/*
// $Workfile: CustBar.h $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CustBar.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 4     6/11/98 9:41a Dennis
// Added HINSTANCE to bitmap loading. Now uses Util::LoadResourceBitmap
// 
// 3     4/03/98 2:54p Fredf
// Traps WM_DISPLAYCHANGE, WM_WININICHANGE, and WM_SETTINGCHANGED to
// default weird behavior of toolbar control which sizes the buttons very
// small and changes the bitmap colors.
// 
// 2     3/16/98 6:46p Fredf
// 
// 1     3/16/98 5:29p Fredf
*/

class CCustomToolBar;

#if !defined(AFX_CUSTBAR_H__52703F90_9C0D_11D1_BCE4_00A0246D41C3__INCLUDED_)
#define AFX_CUSTBAR_H__52703F90_9C0D_11D1_BCE4_00A0246D41C3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CustBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomToolBar window

class CCustomToolBar : public CToolBar
{
// Construction
public:
	CCustomToolBar();

	// These are overridden because CCustomToolBar does not work
	// with ToolBar resources created in the developer studio or
	// with normal bitmap resources. Use SetButtonBitmaps() instead.
	BOOL LoadToolBar(LPCTSTR lpszResourceName)
		{ ASSERT(FALSE); return FALSE; }
	BOOL LoadToolBar(UINT nIDResource)
		{ ASSERT(FALSE); return FALSE; }
	BOOL LoadBitmap(LPCTSTR lpszResourceName)
		{ ASSERT(FALSE); return FALSE; }
	BOOL LoadBitmap(UINT nIDResource)
		{ ASSERT(FALSE); return FALSE; }
	BOOL SetBitmap(HBITMAP hbmImageWell)
		{ ASSERT(FALSE); return FALSE; }
	void SetSizes(SIZE sizeButton, SIZE sizeImage)
		{ ASSERT(FALSE); }

	// Set the palette used for the toolbar. All bitmaps and colors
	// drawn for the toolbar are coerced into this palette. The palette
	// should be set before setting the background and button bitmaps.
	BOOL SetPalette(CPalette* pPalette);

	// Methods for setting toolbar background.
	BOOL SetBackgroundBitmap(LPCTSTR pszResourceName, HINSTANCE hInstance=AfxGetResourceHandle());
	BOOL SetBackgroundBitmap(UINT nIDResource, HINSTANCE hInstance=AfxGetResourceHandle())
		{ return SetBackgroundBitmap(MAKEINTRESOURCE(nIDResource), hInstance); }
	BOOL SetBackgroundColor(COLORREF clColor);

	// Methods for setting button bitmaps.
	BOOL SetButtonBitmaps(LPCTSTR pszResourceName, COLORREF clTransparent, SIZE sizeButton, const UINT* lpIDArray, int nIDCount, HINSTANCE hInstance=AfxGetResourceHandle());
	BOOL SetButtonBitmaps(UINT nIDResource, COLORREF clTransparent, SIZE sizeButton, const UINT* lpIDArray, int nIDCount, HINSTANCE hInstance=AfxGetResourceHandle())
		{ return SetButtonBitmaps(MAKEINTRESOURCE(nIDResource), clTransparent, sizeButton, lpIDArray, nIDCount, hInstance); }

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCustomToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCustomToolBar)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNcPaint();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnDisplayChange(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CPalette* m_pPalette;
	CBitmap* m_pBackground;
	COLORREF m_clBackground;
	CBitmap* m_pBitmap;
	CBitmap* m_pBitmapMask;
	int m_nBitmapIDCount;
	UINT* m_pBitmapIDArray;
	int m_nLastButtonUnderCursor;
	UINT m_uTimer;

protected:

	// Check if we have a bitmap for the buttons.
	BOOL BitmapInitialized(void) const
		{ return m_pBitmap->GetSafeHandle() != NULL; }

	// Free allocated memory.
	void FreeBackground(void);
	void FreeBitmaps(void);

	// Load a bitmap from the resource file and map it to our palette.
	// Select and deselect the palette.
	void SelectPalette(CDC& dc, CPalette*& pOldPalette, BOOL fForceBackground);
	void DeselectPalette(CDC& dc, CPalette*& pOldPalette, BOOL fForceBackground);

	// Return the "paletized" version of the specified color.
	COLORREF PaletteColor(COLORREF Color)
		{ return (m_pPalette == NULL) ? Color : PALETTEINDEX(m_pPalette->GetNearestPaletteIndex(Color)); }

	// Check if this tool bar is active.
	BOOL IsActive(void)
		{ return GetTopLevelParent()->IsWindowEnabled(); }

	// Overridable methods for drawing background, buttons, and separators.
	virtual void DrawBorder(CDC& dc, CRect& crBorder);
	virtual void DrawBackground(CDC& dc, const CRect& crBackground, const CSize& czOffset);
	virtual void DrawButton(CDC& dc, const CRect& crButton, int nButtonIndex, UINT uButtonID, WORD wButtonStyle, WORD wButtonState);
	virtual void DrawSeparator(CDC& dc, const CRect& crButton, int nButtonIndex, UINT uButtonID, WORD wButtonStyle, WORD wButtonState);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTBAR_H__52703F90_9C0D_11D1_BCE4_00A0246D41C3__INCLUDED_)
