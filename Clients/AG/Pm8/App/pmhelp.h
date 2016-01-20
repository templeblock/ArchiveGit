/*
// $Workfile: pmhelp.h $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
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
// $Log: /PM8/App/pmhelp.h $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 12    9/29/98 1:07p Mwilson
// added support for view instruction pages in HTML help window
// 
// 11    7/17/98 9:09p Hforman
// using HH_DISPLAY_TOPIC instead of HH_HELP_CONTEXT since the latter is
// broken.
// 
// 10    7/01/98 6:43p Hforman
// working on floating help - more to come
// 
// 9     6/15/98 6:37p Hforman
// interrum checkin
// 
// 8     6/06/98 2:17p Fredf
// Help window improvments.
// 
// 7     5/25/98 4:40p Fredf
// Fixed TCard notifcation problem.
// 
// 6     4/28/98 11:17a Fredf
// Reduction of flashing on expand/contract.
// 
// 5     4/27/98 5:30p Fredf
// Improved bar positioning.
// 
// 4     4/23/98 7:20p Fredf
// Persistant Show/Hide Help Window.
// 
// 3     4/14/98 7:59p Fredf
// First attempt at Help Window.
// 
// 2     4/14/98 4:05p Fredf
// Ported from test project.
// 
// 1     4/14/98 4:05p Fredf
*/

#ifndef __PMHELP_H__
#define __PMHELP_H__

#include "hhctrl.h"
#include "btnnrect.h"


class CPmHelpWindow : public CDialogBar
{
// Construction
public:
	CPmHelpWindow();

// Attributes
public:

	CSize m_czDocked;
	CSize m_czFloating;

	CRect m_crBorder;

	int m_nNavigationTabWidth;

	BOOL m_fSizesValid;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPmHelpWindow)
	public:
	virtual BOOL Create(LPCSTR pszHelpFile, CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID, int nNavigationTabWidth);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL
    virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);

// Implementation
public:
	virtual ~CPmHelpWindow();

	virtual void DisplayTopic(LPCSTR pszTopic, BOOL bUseCompiledHtmlFile = TRUE);
	virtual void HelpContext(DWORD dwContext);

	// Mainframe::RecalcLayout() should call these.
	virtual void PreRecalcLayout(void);
	virtual void PostRecalcLayout(void);

	BOOL IsValid(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CPmHelpWindow)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTCard(UINT idAction, DWORD dwActionData);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG
   afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnReframeHelpWindow(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	virtual void PreSetWinType(HH_WINTYPE* pWinType);

	void ReframeHelpWindow(void);

	void StartResize(const CPoint& cpDragPoint);
	BOOL ResizeLoop(void);
	void CancelResize(void);
	void EndResize(void);

	BOOL IsEnabled(void);
	BOOL GetCaptionRect(CRect& crCaption);
	LPCTSTR GetHelpTopic(DWORD dwContext);

protected:

	CString m_csHelpFile;
	CHtmlHelpControl m_HtmlHelpControl;
	UINT m_uID;
	CPoint m_cpDragStart;
	CRect m_crResizeStart;
	CRect m_crResize;
	CSize m_czResize;
	CRect m_crResizeLast;
	CSize m_czResizeLast;
	CDC* m_pResizeDC;
	BOOL m_fExpanded;
	HWND m_hwndNotify;
	CNonRectButton m_btnClose;
	BOOL m_fCaptionEnabled;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
