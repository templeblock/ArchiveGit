/*
// $Workfile: PANEL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PANEL.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:58   Fred
// Initial revision.
// 
//    Rev 1.3   14 Aug 1996 11:38:26   Fred
// Project Finder
// 
//    Rev 1.2   02 Aug 1996 13:11:16   Fred
// More 16-bit changes
// 
//    Rev 1.1   23 Jul 1996 08:06:50   Fred
// Uses util
// 
//    Rev 1.0   05 Jun 1996 08:54:28   Fred
// Initial revision.
*/

class CMlsPanel;

#ifndef __PANEL_H__
#define __PANEL_H__

#include "propstr.h"

/////////////////////////////////////////////////////////////////////////////
// MLS Panel
//

// CMlsPanel
class CMlsPanel : public CWnd
{
	INHERIT(CMlsPanel, CWnd)
	
public:
// Construction
	CMlsPanel();

// Implementation
public:
	virtual ~CMlsPanel();

protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
protected:
	// Generated message map functions
	//{{AFX_MSG(CMlsPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetTextLength(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void Initialize(UINT nID, CWnd* pParent, LPCSTR pszText = NULL);

protected:
	enum BorderStyle
	{
		None,
		Frame,
		Sunken,
		Raised,
		Bump,
		Dip
	};

protected:
	void DrawBorder(CDC& dc, CRect& crRect, BorderStyle nStyle, BOOL fOutBorder);

protected:
	HFONT m_hFont;
	CString m_csText;
	CMlsPropertyString m_Properties;
	BorderStyle m_OutBorderStyle;
	BorderStyle m_InBorderStyle;

protected:
	void UnpackProperties(void);
	void UnpackStyle(const CString& csStyle);
	virtual void DrawPanel(CDC* pDC);
	void DeleteFont(void);
};

#endif
