/*
// $Workfile: BITBTN.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/BITBTN.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:44   Fred
// Initial revision.
// 
//    Rev 1.9   22 Apr 1997 13:16:52   Fred
// Start of project and art browsers
// 
//    Rev 1.8   08 Aug 1996 17:30:24   Fred
// More 16-bit changes
// 
//    Rev 1.7   08 Aug 1996 11:11:16   Fred
//  
// 
//    Rev 1.6   03 Aug 1996 16:42:52   Fred
//  
// 
//    Rev 1.5   02 Aug 1996 13:10:48   Fred
// More 16-bit changes
// 
//    Rev 1.4   01 Aug 1996 14:18:34   FRED
// SuperWndProc
// 
//    Rev 1.3   28 Jul 1996 13:33:34   Fred
// Disabling button pops it up
// 
//    Rev 1.2   26 Jul 1996 17:42:52   Fred
//  
// 
//    Rev 1.1   26 Jul 1996 13:27:06   Fred
// CMlsSimpleBitmapButton
// 
//    Rev 1.0   03 Jun 1996 08:44:10   Fred
//  
*/

class CMlsBitmapButton;

#ifndef __BITBTN_H__
#define __BITBTN_H__

#include "propstr.h"

/////////////////////////////////////////////////////////////////////////////
// MLS Bitmap button
//
// A Bitmap button that works in WIN16 and WIN32 and properly handles
// default button processing.
//
// To use the button in a dialog, create a button control as usual and subclass
// the control using the CMlsBitmapButton.Initialize() function. Create a normal
// button control, do not create and owner draw button. If you want the button to
// be the default button, simply set the default button style bit.
//
// The button properties are set by adding properties to the button caption text:
//
// Button Caption:
//
//		<Button Caption>|<Bitmap Resource Name>|<Layout>
//
//	<Layout> has the following format:
//
//		<Alignment><Variation><MultipleLine><WordWrap>
//
//		Where:
//
//			<Alignment> is the alignment of the Bitmap:
//
//				L = align bitmap to the left of the button
//				R = align bitmap to the right of the button
//				T = align bitmap to the top of the button
//				B = align bitmap to the bottom of the button
//
//				The text is placed on the opposite side of the bitmap.
//
//			<Variation> is the text alignment variation:
//
//				0 = align text next to bitmap
//				1 = center text between bitmap and edge of button
//				2 = align text to edge of button
//				3 = align text next to bitmap and center bitmap and text in button
//
//			<MulitpleLine> is the multiple line modifier
//
//				S = caption text is a single line (default)
//				M = caption text is multiple lines (\n starts a new line)
//
//			<WordWrap> is the word wrap modifier (only used with multiple line text)
//
//				N = no word wrap, new lines start only at \n characters (default)
//				W = wrap words which do not fit on the current line
//
//		Examples:
//
//               +-----------------+
//               |                 |
//               | BBB             |
//          L0:  | BBB TTTT        |
//               | BBB             |
//               |                 |
//               +-----------------+
//
//               +-----------------+
//               |                 |
//               | BBB             |
//          L1:  | BBB    TTTT     |
//               | BBB             |
//               |                 |
//               +-----------------+
//
//               +-----------------+
//               |                 |
//               | BBB             |
//         L2:   | BBB        TTTT |
//               | BBB             |
//               |                 |
//               +-----------------+
//
//               +-----------------+
//               |                 |
//               |     BBB         |
//          L3:  |     BBB TTTT    |
//               |     BBB         |
//               |                 |
//               +-----------------+
//
//		Other alignments have similar variations.

// CMlsBitmapButton
class CMlsBitmapButton : public CButton
{
	INHERIT(CMlsBitmapButton, CButton)

public:
// Construction
	CMlsBitmapButton();

// Implementation
public:
	virtual ~CMlsBitmapButton();

protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
protected:
	// Generated message map functions
	//{{AFX_MSG(CMlsBitmapButton)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg LRESULT OnSetState(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void Initialize(UINT nID, CWnd* pParent);

protected:

	enum Alignment
	{
		Left,
		Right,
		Top,
		Bottom
	};

	enum
	{
		MinimumMargin	=	5,
		Separator		=	4
	};

protected:
	CMlsPropertyString m_Properties;
	BOOL m_fHaveBitmap;
	CBitmap m_bmBitmap;
	CBitmap m_bmMask;
	int m_nBitmapWidth;
	int m_nBitmapHeight;
	Alignment m_Alignment;
	int m_nVariation;
	BOOL m_fMultipleLine;
	BOOL m_fWordWrap;

protected:
	void UnpackAlignment(const CString& csStyle);
	virtual void DrawButton(CDC* pDC);

public:
	COLORREF m_clFace;
	COLORREF m_clText;
	COLORREF m_clHilight;
	COLORREF m_clShadow;
	CPalette* m_pPalette;
};

/////////////////////////////////////////////////////////////////////////////
// MLS Simple Bitmap button 
//
// A Bitmap button that works the same as CMlsBitmapButton except it is based
// on CWnd and does not do focus and default button processing.

// CMlsSimpleBitmapButton
class CMlsSimpleBitmapButton : public CWnd
{
	INHERIT(CMlsSimpleBitmapButton, CWnd)

public:
// Construction
	CMlsSimpleBitmapButton();

// Implementation
public:
	virtual ~CMlsSimpleBitmapButton();

protected:
	// Generated message map functions
	//{{AFX_MSG(CMlsSimpleBitmapButton)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetTextLength(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	void Initialize(UINT nID, CWnd* pParent);

	// Get/Set select state.
	BOOL GetSelect(void)
		{ return m_fSelect; }
	void SetSelect(BOOL fSelect = TRUE);

protected:

	enum Alignment
	{
		Left,
		Right,
		Top,
		Bottom
	};

	enum
	{
		MinimumMargin	=	5,
		Separator		=	4
	};

protected:
	CString m_csText;
	HFONT m_hFont;
	CMlsPropertyString m_Properties;
	BOOL m_fHaveBitmap;
	CBitmap m_bmBitmap;
	CBitmap m_bmMask;
	int m_nBitmapWidth;
	int m_nBitmapHeight;
	Alignment m_Alignment;
	int m_nVariation;
	BOOL m_fMultipleLine;
	BOOL m_fWordWrap;
	BOOL m_fSelect;

protected:
	void UnpackProperties(void);
	void UnpackAlignment(const CString& csStyle);
	virtual void DrawButton(CDC* pDC);
	void DeleteFont(void);
};

#endif
