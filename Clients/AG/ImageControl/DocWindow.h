#pragma once

#include "resource.h"
#include "ImageObject.h"
#include "DocCommands.h"
#include "AGDC.h"
#include "Grid.h"
#include "Select.h"
#include "Gdip.h"

class CDocWindow :
	public CWindowImpl<CDocWindow>
{
	friend class CImageControl;
	friend class CDocCommands;

public:
	CDocWindow(CImageControl& ImageControl);
	~CDocWindow();

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDocOpen(UINT code, UINT id, HWND hWnd, BOOL& bHandled);
	LRESULT OnDocClose(UINT code, UINT id, HWND hWnd, BOOL& bHandled);
	LRESULT OnDocSave(UINT code, UINT id, HWND hWnd, BOOL& bHandled);
	LRESULT OnDocTransparent(UINT code, UINT id, HWND hWnd, BOOL& bHandled);
	LRESULT OnDocZoomIn(UINT code, UINT id, HWND hWnd, BOOL& bHandled);
	LRESULT OnDocZoomOut(UINT code, UINT id, HWND hWnd, BOOL& bHandled);
	LRESULT OnDocZoomFull(UINT code, UINT id, HWND hWnd, BOOL& bHandled);
	LRESULT OnDocGrid(UINT code, UINT id, HWND hWnd, BOOL& bHandled);
	LRESULT OnDocHidden(UINT code, UINT id, HWND hWnd, BOOL& bHandled);
	LRESULT OnDocShowHidden(UINT code, UINT id, HWND hWnd, BOOL& bHandled);
	LRESULT OnDocAbout(UINT code, UINT id, HWND hWnd, BOOL& bHandled);

	void InvalidatePage();
	void Repaint(const CRect& Rect);
	void SetBackgroundMessage(LPCSTR pMessage);
	bool DoCommand(LPCTSTR strCommand, LPCTSTR strValue);
	long GetCommand(LPCTSTR strCommand);

	void SetPageRect(CImageObject* pBaseObject, bool bUnZoom);
	CRect& GetPageRect()
		{ return m_PageRect; }
	CImageObject* OpenImage(LPCTSTR strFileName, const BITMAPINFOHEADER* pDIB, CMatrix* pMatrix);
	bool StartOver();
	bool CloseImage(CImageObject* pObject);
	CImageObject* FindImage(UINT id);
	CImageObject* GetSelectedObject();
	CImageObject* GetBaseObject();
	void InvalidateImage(CImageObject* pObject);

	CSelect& GetSelect()
		{ return m_Select; }
	HHOOK GetKeyboardHook()
		{ return m_hKeyboardHook; }
	bool HasFocus()
		{ return m_bHasFocus; }
	bool SpecialsEnabled()
		{ return m_bSpecialsEnabled; }
	void SetSpecialsEnabled(bool bEnabled)
		{ m_bSpecialsEnabled = bEnabled; }
	bool GetOverrideHidden()
		{ return m_bOverrideHidden; }
	CAGClientDC* GetClientDC()
		{ return m_pClientDC; }
	CRect& GetPageViewRect()
		{ return m_PageViewRect; }
	CGrid& GetGrid()
		{ return m_Grid; }

protected:
	void Free();

protected:
	void DoDropImage(LPCSTR strFileName, POINTL Pt);
	void DoDragOver(POINTL Pt);
	void DoDragLeave();
	void SetupPageView(bool bSetScrollbars);
	void SetupScrollbars(int xPos, int yPos, int xSize, int ySize, int xVisible, int yVisible, bool bRedraw);
	void EliminateScrollbars(bool bRedraw);
	void ResetScrollPos(int nBar, int nPos, bool bRedraw);
	void SetScrollPage(int nBar, UINT nPage, bool bRedraw);
	UINT GetScrollPage(int nBar);
	bool Zoom(int iOffset);
	bool ZoomEx(UINT nZoomLevel, double fZoomScale);
	static LRESULT CALLBACK ImageControlKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	static UINT CALLBACK OpenFileHookProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	void OnToolButtonDown(CPoint Pt, CImageObject* pObject);
	bool IsRecolorSelected();
	bool IsRedEyeSelected();

public:
	BEGIN_MSG_MAP(CDocWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow) // window 'hide' is the logical OnClose
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSTATIC, OnCtlColor)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_HSCROLL, OnScroll)
		MESSAGE_HANDLER(WM_VSCROLL, OnScroll)

		COMMAND_ID_HANDLER(IDC_DOC_OPEN, OnDocOpen)
		COMMAND_ID_HANDLER(IDC_DOC_CLOSE, OnDocClose)
		COMMAND_ID_HANDLER(IDC_DOC_SAVE, OnDocSave)
		COMMAND_ID_HANDLER(IDC_DOC_TRANSPARENT, OnDocTransparent)
		COMMAND_ID_HANDLER(IDC_DOC_ZOOMIN, OnDocZoomIn)
		COMMAND_ID_HANDLER(IDC_DOC_ZOOMOUT, OnDocZoomOut)
		COMMAND_ID_HANDLER(IDC_DOC_ZOOMFULL, OnDocZoomFull)
		COMMAND_ID_HANDLER(IDC_DOC_GRID, OnDocGrid)
		COMMAND_ID_HANDLER(IDC_DOC_HIDDEN, OnDocHidden)
		COMMAND_ID_HANDLER(IDC_DOC_SHOWHIDDEN, OnDocShowHidden)
		COMMAND_ID_HANDLER(IDC_ABOUT, OnDocAbout)
	END_MSG_MAP()

protected:
	// Create a global instance, so GDI+ will be always available
	// Just call CGdip::IsOK() and then use GDI+ classes
	CGdip m_Gdip;
	static CDocWindow* g_pDocWindow;
	CImageControl& m_ImageControl; // Our parent
	CDocCommands m_DocCommands;
	CSimpleArray<CImageObject*> m_ImageArray;
	CAGClientDC* m_pClientDC;
	HHOOK m_hKeyboardHook;
	bool m_bSpecialsEnabled;
	bool m_bOverrideHidden;
	CRect m_PageViewRect;
	CRect m_PageRect;
	bool m_bHasFocus;
	int m_iZoom;
	double m_fZoomScale;
	bool m_bIgnoreSetCursor;
	CString m_strBackgroundMessage;
	CGrid m_Grid;
	CSelect m_Select;
	CSize m_ShadowSize;
	CToolSettings m_ToolSettings;
};
