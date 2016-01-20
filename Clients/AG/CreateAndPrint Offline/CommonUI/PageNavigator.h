#pragma once

#include "resource.h"
#include "AGDoc.h"
#include "Label.h"
#include "atlwin.h"

class CCtp;

class CPageNavigator : public CWindowImpl<CPageNavigator, CListViewCtrl>
{
public:
	CPageNavigator(CCtp* pMainWnd);
	virtual ~CPageNavigator();

public:
	void InitDialog(HWND hWndListCtrl, HWND hWndLabel, HWND hWndPrevButton, HWND hWndNextButton);
	void ActivateNewDoc(CAGDoc* pAGDoc, int nDefaultPage = 0);
	void SetPage(int nPage);
	int GetPage();
	bool AddPage(LPCSTR strPageName);
	bool DeleteCurrentPage();
	int ChangePage(int iDelta);
private:
	bool SelectPage(int nPage, bool bSetState);
	void GetIconRect(int nItem, RECT* pRect);
	void EnsureIconVisible(int nItem);

//j	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CPageNavigator)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
//j		MESSAGE_HANDLER(OCM_NOTIFY, OnNotify) // NO LONGER USED
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

protected:
	CCtp* m_pCtp;
	CImageList m_ImageList;
	HWND m_hWndPrevButton;
	HWND m_hWndNextButton;
	int m_nPage;
	int m_dxIcon;
	int m_dyIcon;
	int m_yTop;
	int m_xLeft;
	CAGDoc* m_pAGDoc;
	CLabel m_LabelPage;
	bool m_bCapture;
	bool m_bDown;
	bool m_bIsCard;
};
