#pragma once

#ifndef __ATLCTRLS_H__
	#error ToolTipDialog.h requires atlctrls.h to be included first
#endif

#ifndef TTS_BALLOON
	#define TTS_BALLOON 0
#endif

namespace WTL
{
template <class T, class CToolTipCtrlTT = CToolTipCtrl>
class CToolTipDialog
{
public:
	BEGIN_MSG_MAP(CToolTipDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
	END_MSG_MAP()
	
public:
	CToolTipDialog(UINT uToolTipStyle = TTS_NOPREFIX | TTS_BALLOON, UINT uToolFlags = TTF_IDISHWND | TTF_SUBCLASS)
	{
		m_ToolTipCtrl = NULL;
		m_uToolTipStyle = uToolTipStyle;
		m_uToolFlags = uToolFlags | TTF_SUBCLASS;
	}

	CToolTipCtrlTT& GetToolTipCtrl()
	{
		return m_ToolTipCtrl;
	}

	bool AddToolTip(HWND hControl)
	{
		return !!SetTool(hControl, (LPARAM)(T*)this);
	}

protected:
	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL& bHandled)
	{
		InitToolTip();
		bHandled = false;
		return true;
	}

	LRESULT OnMouseMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		T* pT = (T*)this;
		bHandled = false;
		if (m_ToolTipCtrl.IsWindow())
			m_ToolTipCtrl.RelayEvent((LPMSG)pT->GetCurrentMessage());
		return 0;
	}

	void InitToolTip()
	{
		T* pT= (T*)this;
		m_ToolTipCtrl.Create(*pT, NULL, NULL, m_uToolTipStyle);

		CString strTipText;
		strTipText.LoadString(pT->IDD);
		if (!strTipText.IsEmpty())
		{
			CToolInfo ToolInfo(pT->m_uToolFlags, *pT, NULL/*nIDTool*/, NULL/*lpRect*/, (LPTSTR)(LPCTSTR)strTipText);
			m_ToolTipCtrl.AddTool(&ToolInfo);
		}

		::EnumChildWindows(*pT, SetTool, (LPARAM)pT);
		m_ToolTipCtrl.SetMaxTipWidth(0);
		m_ToolTipCtrl.SetDelayTime(TTDT_INITIAL, 1000);
		m_ToolTipCtrl.SetDelayTime(TTDT_RESHOW, 1000);
		m_ToolTipCtrl.SetDelayTime(TTDT_AUTOPOP, 10000);
		m_ToolTipCtrl.Activate(true);
	}

	HWND IDToHWND(UINT idControl)
	{
		return ::GetDlgItem(*(T*)this, idControl);
	}

	static BOOL CALLBACK SetTool(HWND hControl, LPARAM pDlg)
	{
		T* pT = (T*)pDlg;
		UINT idControl = ::GetWindowLong(hControl, GWL_ID);
		if (idControl == IDC_STATIC)
			return true;

		CString strTipText;
		strTipText.LoadString(idControl);
		if (!strTipText.IsEmpty())
		{
			CToolInfo ToolInfo(pT->m_uToolFlags, hControl, NULL/*nIDTool*/, NULL/*lpRect*/, (LPTSTR)(LPCTSTR)strTipText);
			pT->m_ToolTipCtrl.AddTool(&ToolInfo);
		}

		return true;
	}

protected:
	CToolTipCtrlTT m_ToolTipCtrl;
	UINT m_uToolTipStyle;
	UINT m_uToolFlags;
};
} // namespace WTL
