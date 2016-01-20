#pragma once

#include <atlframe.h>		// For CFrameWindowImpl
#include <atlctrlw.h>		// For CCommandBarCtrl
#include "resource.h"
#include "PrintPreviewWindowEx.h"

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

	BEGIN_UPDATE_UI_MAP(CPreviewFrame)
		UPDATE_ELEMENT(ID_PREV_PAGE, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_NEXT_PAGE, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_ZOOM_IN, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_ZOOM_OUT, UPDUI_TOOLBAR)
	END_UPDATE_UI_MAP()

	CPreviewFrame();

	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	void OnDestroy();
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
	void ZoomPreview(int nZoomFactor);
	void UpdateToolBarState();

private:
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (CFrameWindowImpl<CPreviewFrame>::PreTranslateMessage(pMsg))
			return true;

		return true;
	}

	virtual BOOL OnIdle()
	{
		UIEnable(ID_PREV_PAGE, m_bPrevPage);
		UIEnable(ID_NEXT_PAGE, m_bNextPage);
		UIEnable(ID_ZOOM_IN, m_bZoomIn);
		UIEnable(ID_ZOOM_OUT, m_bZoomOut);
		UIUpdateToolBar();
		return FALSE;
	}

	CPoint m_ptMinTrackSize;
	int m_iOrientation;
	bool m_bPreview;
	bool m_bPrevPage;
	bool m_bNextPage;
	bool m_bZoomIn;
	bool m_bZoomOut;
	HWND m_hWndToolbar;
	CCommandBarCtrl m_CmdBar;
	int m_nCurrentPage;
	CPrintPreviewWindowEx* m_pPreviewWnd;
	HWND m_hMainWnd;
	int m_nMaxPage;
};
