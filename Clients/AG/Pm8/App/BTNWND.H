/*
// $Workfile: BTNWND.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/BTNWND.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:48   Fred
// Initial revision.
// 
//    Rev 1.7   15 Jan 1997 10:49:36   Jay
// New toolbars
// 
//    Rev 1.6   03 Aug 1996 16:42:52   Fred
//  
// 
//    Rev 1.5   02 Aug 1996 13:10:50   Fred
// More 16-bit changes
// 
//    Rev 1.4   24 Jul 1996 18:25:16   Jay
// PaintCustomize() call
// 
//    Rev 1.3   23 Jul 1996 16:51:56   Fred
// Redraws on WM_ENABLE
// 
//    Rev 1.2   23 Jul 1996 09:05:20   Fred
// Double clicks turned into single clicks
// 
//    Rev 1.1   23 Jul 1996 08:04:06   Fred
// Added size to bitmap flag
// 
//    Rev 1.0   11 Jul 1996 17:46:16   Jay
//  
*/

class CButtonWnd;

#ifndef __BTNWND_H__
#define __BTNWND_H__

//////////////////////////////////////////////////////////////////////////
// CButtonWnd

class CButtonWnd : public CWnd
{
	INHERIT(CButtonWnd, CWnd)

public:
	CButtonWnd();
	virtual ~CButtonWnd();

   // Standard create.
   virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
  
	// More advanced constructor.
	CButtonWnd(LPCSTR pTitle, CWnd* pParent, UINT uID, UINT uBitmap);
    
	// More advanced Create.
	BOOL Create(LPCSTR pTitle, CWnd* pParent, UINT uID, UINT uBitmap);
    
    // Create the button from an existing control.
	void Initialize(UINT nID, CWnd* pParent);
	
	// Set bitmap to use.
	void SetBitmap(CBitmap* pBitmap, BOOL fOwnsBitmap = TRUE, BOOL fSizeToBitmap = TRUE);

	// Get/Set select state.
	BOOL GetSelect(void)
		{ return m_fSelect; }
	void SetSelect(BOOL fSelect = TRUE);

	int GetWidth(void) const
		{ return 2+m_nBitmapWidth+2; }
	int GetHeight(void) const
		{ return 2+m_nBitmapHeight+2; }

protected:
	// Generated message map functions
	//{{AFX_MSG(CButtonWnd)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CBitmap* m_pBitmap;

	int m_nBitmapWidth;
	int m_nBitmapHeight;

	BOOL m_fSelect;
	BOOL m_fOwnsBitmap;

	void DeleteBitmap(void);

	virtual void PaintCustomize(CDC& dcButton, CRect crButton);
};

#endif
