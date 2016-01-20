//
// $Workfile: BtnNRect.h $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
//
//
// Revision History:
//
// $Log: /PM8/App/BtnNRect.h $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 4     6/15/98 6:00p Psasse
// Added get methods for m_pbmup/down
// 
// 3     4/20/98 7:38p Fredf
// Uses Util::LoadResourceBitmap to do 256-colors correctly.
// 
// 2     12/04/97 5:23p Hforman
// a bit more functionality
// 
// 1     12/04/97 12:40p Hforman
//

#ifndef __BTNNRECT_H__
#define __BTNNRECT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "btnwnd.h"

/////////////////////////////////////////////////////////////////////////////
// CNonRectButton window

class CNonRectButton : public CButtonWnd
{
// Construction
public:
				CNonRectButton();
	virtual	~CNonRectButton();

public:
	BOOL		Create(const CRect rect, CWnd* pParentWnd, UINT nID);

	void		SetBitmaps(UINT nUpResID, UINT nDownResID, CPalette* pPalette = NULL);
	void		SetBitmaps(LPCTSTR strUpRes, LPCTSTR strDownRes, CPalette* pPalette = NULL);
	void		SetTransparentColor(COLORREF color)		{ m_clrTransparent = color; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNonRectButton)
	//}}AFX_VIRTUAL

protected:
	void			SizeButton();
	void			DeleteBitmaps();
	BOOL			OverTransparentColor(CPoint point);

	CBitmap*		m_pbmUp;
	CBitmap*		m_pbmDown;

	COLORREF		m_clrTransparent;	 // color used for transparency

public:
	CBitmap* GetUpBmp ( void )
	{  return m_pbmUp;}

	CBitmap* GetDownBmp ( void )
	{  return m_pbmDown;}

	// Generated message map functions
protected:
	//{{AFX_MSG(CNonRectButton)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __BTNNRECT_H__
