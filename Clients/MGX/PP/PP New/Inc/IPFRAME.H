/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

                                ipframe.h

*****************************************************************************
*/

#ifndef _IPFRAME_H
#define _IPFRAME_H

#ifndef __AFXEXT_H__
#include <afxext.h>         // for access to CToolBar and CStatusBar
#endif

#ifdef WIN32
#ifndef __AFXPRIV_H__
#include <afxpriv.h>
#endif
#endif

#include "pptbars.h"
#include "svrview.h"


/* ******************************* Public Data ***************************** */

/* ********************************* Constants ****************************** */

/* *************************** Class Declarations ************************** */

class CInPlaceFrame;

/* ******************************* Typedefs ******************************* */

/* ******************************* Classes ******************************** */

class CPPOleResizeBar : public COleResizeBar
{
public:

//	CPPOleResizeBar() {m_tracker.m_nStyle |= CRectTracker::proportionalResizeOnly; };
CPPOleResizeBar() {};
};

class CRibbonDialog : public CDialog
{
public:

	CRibbonDialog() : CDialog () {};
    BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL) 
		{return CDialog::Create(nIDTemplate,pParentWnd);};
    BOOL Create(LPCSTR lpszTemplateName, CWnd* pParentWnd = NULL)
		{return CDialog::Create(lpszTemplateName,pParentWnd);};

protected:

    void CalcSysMenuRect(BOOL bClient, CRect &rMenu);
    void CalcTitleRect(BOOL bClient, CRect &rTitle);
    void InvertSysMenu();

	DECLARE_MESSAGE_MAP()
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnClose();
	afx_msg BOOL OnNcActivate(BOOL f);
	afx_msg void OnNcPaint();
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);

};

class CInPlaceFrame : public COleIPFrameWnd
{
	DECLARE_DYNCREATE(CInPlaceFrame)
public:
	CInPlaceFrame();

	// Implementation
public:
	virtual ~CInPlaceFrame();

BOOL CInPlaceFrame::CanGetBorderSpace(
    LPCRECT lpRectBorder, LPOLEINPLACEUIWINDOW lpUIWindow, BOOL bFrame);

#if _MFC_VER == 0x0250
	HWND  GetClientFrameWnd() {return m_hWndFrame;};
	HWND  GetDocFrameWnd() {return m_hWndDocFrame;};
#else
	HWND  GetClientFrameWnd() {return m_pMainFrame->GetSafeHwnd();};
	HWND  GetDocFrameWnd() {return m_pDocFrame->GetSafeHwnd();};
#endif // _MFC_VER == 0x0250
	HMENU GetIPMenu() { return m_hSharedMenu; };
   HWND  GetCurrRibbon() { return m_hCurrRibbon;};

	void DestroyPasteFromFileDlg ();
	BOOL InstallRibbon(UINT idRibbon, DLGPROC lpRibbonProc,	BOOL bShow = TRUE);
	BOOL InstallToolBox(BOOL bShow);
	BOOL NeedSetInitToolbarPos() {return (m_bNeedSetInitToolbarPos); };

	BOOL NeedCacheUpdated() {return (m_bNeedCacheUpdate); };
	void SetUpdateCacheFlag(BOOL bNeedUpdate) { m_bNeedCacheUpdate = bNeedUpdate; };

	void SetHintString(LPCSTR lpHintStr);
	BOOL ShowHidePalette(int iShowPalette); // -1=statusOnly,0=FALSE,1+=TRUE

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL OnCreateControlBars(CWnd* pWndFrame, CWnd* pWndDoc);

protected:

	// control bar embedded members
	CPPOleResizeBar m_wndResizeBar;
	CDialogBar      m_wndRibbonBar;
	CRibbonDialog   m_wndFloatRibbonBar; // new jtm
	CPPToolBar*     m_pToolBox;
	CStatusBar      m_wndStatusBar;

	BOOL 			m_bNeedSetInitToolbarPos;

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
afx_msg BOOL OnQueryNewPalette();
	afx_msg LRESULT OnResizeChild(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnSetCursor( CWnd *pWnd, UINT nHitTest, UINT msg);
	afx_msg void OnUpdateContextHelp(CCmdUI* pCmdUI);

    afx_msg void OnPalette();
    afx_msg void OnUpdatePalette(CCmdUI* pCmdUI);
	afx_msg void OnPasteFromFile();
	afx_msg LONG OnPasteFromFileClosed(UINT wParam, LONG lParam);
	afx_msg void OnUpdatePasteFromFile(CCmdUI* pCmdUI);

private:

	BOOL m_bNeedCacheUpdate;
	BOOL m_bUseOwnStatusBar;
	HWND m_hCurrRibbon;
	int  m_idCurrRibbon;
	HWND m_hPasteFromFileDlg;
	HWND m_hPaletteDlg;

	void init_ImageToInPlace(CWnd * pParentWnd);
};

/* *************************** Inline Methods ****************************** */

/* *************************** Global Data ********************************* */


/* *************************** Global Function ***************************** */

/*************************************************************************/
#endif //_IPFRAME_H



