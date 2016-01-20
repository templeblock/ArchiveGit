#include "stdafx.h"
#include "Resource.h"
#include "Combo.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CCombo::CCombo()
{
	m_bAfterCloseUp = false;
	m_wLastNotifyCode = 0;
	m_bEditable = false;
}

//////////////////////////////////////////////////////////////////////
CCombo::~CCombo()
{
}

//////////////////////////////////////////////////////////////////////
void CCombo::InitDialog(HWND hWndCtrl)
{
	if (!hWndCtrl)
		return;

	SubclassWindow(hWndCtrl);

	m_bEditable = !((GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST);
}

//////////////////////////////////////////////////////////////////////
bool CCombo::GetAfterCloseUp()
{	
	return m_bAfterCloseUp;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCombo::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = false;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCombo::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(ID_HAND)));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCombo::OnComboCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// When we send a notification to our parent about the combo changes (below),
	// that notification it is actually reflected back to us.
	// Be sure to bail un-handled in that case
	if (wNotifyCode & COMBO_NOTIFCATION_FLAG)
	{ 
		bHandled = false;
		return S_OK;
	}

	bool bAfterCloseUp = (m_wLastNotifyCode == CBN_CLOSEUP);
	m_wLastNotifyCode = wNotifyCode;

	if (wNotifyCode == CBN_SELCHANGE)
	{
		m_bAfterCloseUp = bAfterCloseUp;
		::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), COMBO_NOTIFCATION_FLAG|wNotifyCode), (LPARAM)m_hWnd);
	}
	else
	if (wNotifyCode == CBN_EDITUPDATE)
	{
		m_bAfterCloseUp = false;
		::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), COMBO_NOTIFCATION_FLAG|wNotifyCode), (LPARAM)m_hWnd);
	}

	return S_OK;
}
