#pragma once

#include "resource.h"
#include "AGDoc.h"
#include "ColorScheme.h"
#include "PageNavigator.h"
#include "BmpButton.h"

class CCtp;

/////////////////////////////////////////////////////////////////////////////
class CPagePanel : public CDialogImpl<CPagePanel>
{
public:
	CPagePanel(CCtp* pMainWnd);
	~CPagePanel();

	enum { IDD = IDD_NAV_PANEL };

	HWND Create(HWND hWndParent);
	void ActivateNewDoc();
	bool PageChange(int nLastPage = -1, int nPage = -1);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPageChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPagePrev(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPageNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPageAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPageDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	void AddToolTip(UINT idControl);

public:
	BEGIN_MSG_MAP(CPagePanel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSCROLLBAR, OnCtlColor)
		COMMAND_ID_HANDLER(IDC_DOC_PAGENAV, OnPageChange)
		COMMAND_ID_HANDLER(IDC_DOC_PAGEPREV, OnPagePrev)
		COMMAND_ID_HANDLER(IDC_DOC_PAGENEXT, OnPageNext)
		COMMAND_ID_HANDLER(IDC_DOC_PAGEADD, OnPageAdd)
		COMMAND_ID_HANDLER(IDC_DOC_PAGEDELETE, OnPageDelete)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

protected:
	CCtp* m_pCtp;
	CColorScheme m_ColorScheme;
	CToolTipCtrl m_ToolTip;
	CBmpButton m_ButtonPageAdd;
	CBmpButton m_ButtonPageDelete;
	CBmpButton m_ButtonPrev;
	CBmpButton m_ButtonNext;
public:
	CPageNavigator m_PageNavigator;
};
