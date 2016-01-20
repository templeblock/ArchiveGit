/*
// $Workfile: SCROLLGA.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SCROLLGA.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:50   Fred
// Initial revision.
// 
//    Rev 1.6   03 Aug 1996 16:43:20   Fred
//  
// 
//    Rev 1.5   02 Aug 1996 15:02:22   Fred
// 16-bit changes
// 
//    Rev 1.4   02 Aug 1996 13:11:18   Fred
// More 16-bit changes
// 
//    Rev 1.3   25 Jul 1996 18:21:32   Fred
//  
// 
//    Rev 1.2   24 Jul 1996 13:18:30   Fred
// Gadget improvements
// 
//    Rev 1.1   23 Jul 1996 16:53:58   Fred
// Start of button/view hookup
// 
//    Rev 1.0   23 Jul 1996 08:16:42   Fred
// Initial revision.
*/

class CMlsGadget;
class CMlsScrollGadgetButton;
class CMlsScrollGadgetPanel;
class CMlsScrollGadget;
class CMlsPageGadget;
class CMlsZoomGadget;

#ifndef __SCROLLGA_H__
#define __SCROLLGA_H__

#include "btnwnd.h"
#include "panel.h"

class CPmwView;

/////////////////////////////////////////////////////////////////////////////
// CMlsGadget
// Base class for gadgets.

class CMlsGadget : public CControlBar
{
	INHERIT(CMlsGadget, CControlBar)

public:
	CMlsGadget();
	virtual ~CMlsGadget();

public:
	CRect& GetRectangle(void)
		{ return m_crRect; }

public:
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

protected:
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

protected:
	// Generated message map functions
	//{{AFX_MSG(CMlsGadget)
	afx_msg LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	
	virtual LRESULT WindowProc(UINT uMessage, WPARAM wParam, LPARAM lPARAM);

	void DeleteFont(void);

protected:
	void SetRectangle(CRect& crRect)
		{ m_crRect = crRect; }
	CRect m_crRect;
	HFONT m_hFont;
	BOOL m_fInSizeParent;
	CSize m_czAvailableSize;

protected:
	virtual CPmwView* GetCurrentView(void);
	virtual UINT GadgetId(void);
	virtual BOOL GadgetVisible(void);
	virtual BOOL GadgetEnabled(UINT uPart);
	virtual void GadgetClicked(UINT uPart);
	virtual void GadgetText(CString& csText);
};

/////////////////////////////////////////////////////////////////////////////
// CMlsSeparatorGadget

class CMlsSeparatorGadget : public CMlsGadget
{
	INHERIT(CMlsSeparatorGadget, CMlsGadget)

public:
	CMlsSeparatorGadget();
	virtual ~CMlsSeparatorGadget();

public:
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

protected:
	virtual void DoPaint(CDC* pDC);

protected:
	// Generated message map functions
	//{{AFX_MSG(CMlsSeparatorGadget)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL GadgetVisible(void);
};

/////////////////////////////////////////////////////////////////////////////
// CMlsScrollGadgetButton
// Variation of CButtonWnd which sends ID command on right mouse click.

class CMlsScrollGadgetButton : public CButtonWnd
{
	INHERIT(CMlsScrollGadgetButton, CButtonWnd)

public:
	CMlsScrollGadgetButton();
	virtual ~CMlsScrollGadgetButton();

protected:
	// Generated message map functions
	//{{AFX_MSG(CMlsScrollGadgetButton)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMlsScrollGadgetPanel
// Variation of CMlsPanel which sends ID command on mouse click.

class CMlsScrollGadgetPanel : public CMlsPanel
{
	INHERIT(CMlsScrollGadgetPanel, CMlsPanel)

public:
	CMlsScrollGadgetPanel();
	virtual ~CMlsScrollGadgetPanel();

protected:
	// Generated message map functions
	//{{AFX_MSG(CMlsScrollGadgetPanel)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMlsScrollGadget
// Base class for scrolling gadgets.

class CMlsScrollGadget : public CMlsGadget
{
	INHERIT(CMlsScrollGadget, CMlsGadget)

public:
	CMlsScrollGadget();
	virtual ~CMlsScrollGadget();

public:
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual void SetButtons(UINT uBackButton, UINT uNextButton);
	virtual BOOL GetButtonRectangles(CSize czSize, CRect& crBack, CRect& crText, CRect& crNext);
	virtual void PositionButtons(void);

protected:
	virtual void DoPaint(CDC* pDC);

protected:
	// Generated message map functions
	//{{AFX_MSG(CMlsScrollGadget)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateBack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateText(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNext(CCmdUI* pCmdUI);
	afx_msg void OnBack();
	afx_msg void OnText();
	afx_msg void OnNext();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CMlsScrollGadgetButton m_BackButton;
	CMlsScrollGadgetButton m_NextButton;
	CMlsScrollGadgetPanel m_TextPanel;
};

/////////////////////////////////////////////////////////////////////////////
// CMlsPageGadget

class CMlsPageGadget : public CMlsScrollGadget
{
	INHERIT(CMlsPageGadget, CMlsScrollGadget)

public:
	CMlsPageGadget();
	virtual ~CMlsPageGadget();

protected:
	// Generated message map functions
	//{{AFX_MSG(CMlsPageGadget)
	afx_msg LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMlsZoomGadget

class CMlsZoomGadget : public CMlsScrollGadget
{
	INHERIT(CMlsZoomGadget, CMlsScrollGadget)

public:
	CMlsZoomGadget();
	virtual ~CMlsZoomGadget();

protected:
	// Generated message map functions
	//{{AFX_MSG(CMlsZoomGadget)
	afx_msg LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
