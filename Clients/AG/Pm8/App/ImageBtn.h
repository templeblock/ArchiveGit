//****************************************************************************
//
// File Name:  ImageBtn.cpp
//
// Project:    Renaissance headline user interface
//
// Author:     Lance Wilson
//
// Description: Definition of the ROwnerDrawBtn class
//
// Portability: Windows Specific
//
// Developed by:   Broderbund Software, Inc.
//				   500 Redwood Blvd.
//				   Novato, CA 94948
//			       (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/ImageBtn.h                                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:06p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _IMAGEBTN_H_
#define _IMAGEBTN_H_

/////////////////////////////////////////////////////////////////////////////
// ROwnerDrawBtn window

class ROwnerDrawBtn : public CButton
{
// Construction
public:

					ROwnerDrawBtn() ;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ROwnerDrawBtn)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~ROwnerDrawBtn();

	// Generated message map functions
protected:
	//{{AFX_MSG(ROwnerDrawBtn)
	afx_msg LRESULT OnGetCheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetCheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetState(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetState(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEnable(WPARAM wParam, LPARAM lParam);

	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	void DrawButton (CDC* pDC, CRect& rect, UINT uiItemState) ;
	void SendDrawItem (UINT uiItemAction, CDC* pDC = NULL) ;

	DECLARE_MESSAGE_MAP()

protected:

//	CBitmap m_bmpList ;

	UINT	m_uiBtnState ;
	HBITMAP	m_hBitmap ;

	int m_cxItemWidth ;
	int m_cyItemHeight ;

	int	m_nImageNormal ;
	int m_nImageFocus ;
	int m_nImageDisable ;
};

/////////////////////////////////////////////////////////////////////////////

#ifndef _WINDOWSBITMAP_H_
	#include "WindowsImageList.h"
#endif

class RImageBtn : public ROwnerDrawBtn
{
public:

					RImageBtn() ;

	void			SetImage( RWindowsImageList* pImage, int nIndex = 0 ) ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RImageBtn)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

private:

	RWindowsImageList* m_pImage ;

	int				m_nImageIndex ;

//	CSize	szBitmap ;
} ;

#endif // _IMAGEBTN_H_
