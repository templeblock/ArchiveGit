// MainFrm.h : interface of the CPreviewFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREVIEWFRM_H__6048601A_4BE3_4014_9FBC_6B5BE88F941A__INCLUDED_)
#define AFX_PREVIEWFRM_H__6048601A_4BE3_4014_9FBC_6B5BE88F941A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <atlbase.h>       // base ATL classes
#include <atlapp.h>        // base WTL classes
//extern CAppModule _Module; // WTL version of CComModule
#include <atlwin.h>        // ATL GUI classes
#include <atlframe.h>      // WTL frame window classes
#include <atlmisc.h>       // WTL utility classes like CString
#include <atlcrack.h>      // WTL enhanced msg map macros
#include <atlctrlw.h>
#include "resource.h"
#include "PrintPreviewWindowEx.h"


// MyWindow.h:
class CPreviewFrame : public CFrameWindowImpl<CPreviewFrame>, public CUpdateUI<CPreviewFrame>,
		public CMessageFilter, public CIdleHandler
{
public:
    DECLARE_FRAME_WND_CLASS(NULL, IDR_PREVIEWFRAME);
 
public:
    BEGIN_MSG_MAP_EX(CPreviewFrame)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(ID_PREV_PAGE, OnPrevPage)
        COMMAND_ID_HANDLER_EX(ID_NEXT_PAGE, OnNextPage)
        COMMAND_ID_HANDLER_EX(ID_ZOOM_IN, OnZoomIn)
        COMMAND_ID_HANDLER_EX(ID_ZOOM_OUT, OnZoomOut)
        COMMAND_ID_HANDLER_EX(ID_CLOSE, OnClose)
        COMMAND_ID_HANDLER_EX(ID_FILE_PRINT, OnFilePrint)
      CHAIN_MSG_MAP(CFrameWindowImpl<CPreviewFrame>)
    END_MSG_MAP()

       // MSG_WM_DESTROY(OnDestroy)

	BEGIN_UPDATE_UI_MAP(CPreviewFrame)
		UPDATE_ELEMENT(ID_PREV_PAGE, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_NEXT_PAGE, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_ZOOM_IN, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_ZOOM_OUT, UPDUI_TOOLBAR)
	END_UPDATE_UI_MAP()

	CPreviewFrame();

	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	LRESULT OnDestroy();
	void ResetWindowState();
	void OnPrevPage(UINT uCode, int nID, HWND hwndCtrl);
	void OnNextPage(UINT uCode, int nID, HWND hwndCtrl);
	void OnZoomIn(UINT uCode, int nID, HWND hwndCtrl);
	void OnZoomOut(UINT uCode, int nID, HWND hwndCtrl);
	void OnClose(UINT uCode, int nID, HWND hwndCtrl);
	void OnFilePrint(UINT uCode, int nID, HWND hwndCtrl);
	void SetPreviewWindow(CPrintPreviewWindowEx* pPreviewWnd);
	CPrintPreviewWindowEx* GetPreviewWindow() { return m_pPreviewWnd; };
	void SetMainHwnd(HWND hWnd) { m_hMainWnd = hWnd;};
	HWND GetMainHwnd() { return m_hMainWnd; };
	void SetWindowCoor(int x, int y);
	void ZoomPrintPreview(int nZoomFactor);
	void UpdateToolBarState();

private:
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CPreviewFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return TRUE;
	}

	virtual BOOL OnIdle()
	{
        UIEnable(ID_PREV_PAGE, m_fPrevPage);
        UIEnable(ID_NEXT_PAGE, m_fNextPage);
        UIEnable(ID_ZOOM_IN, m_fZoomIn);
        UIEnable(ID_ZOOM_OUT, m_fZoomOut);
		UIUpdateToolBar();
		return FALSE;
	}

	POINT m_ptMinTrackSize;
	int m_iOrientation;
	bool m_bPrintPreview;
	//HWND m_hWndToolbar;
	BOOL m_fPrevPage;
	BOOL m_fNextPage;
	BOOL m_fZoomIn;
	BOOL m_fZoomOut;
	BOOL m_fPrint;
	BOOL m_fClose;
	HWND m_hWndToolbar;
	CCommandBarCtrl m_CmdBar;
	int m_nCurrentPage;
	CPrintPreviewWindowEx* m_pPreviewWnd;
	HWND m_hMainWnd;
	int m_nMaxPage;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__6048601A_4BE3_4014_9FBC_6B5BE88F941A__INCLUDED_)
