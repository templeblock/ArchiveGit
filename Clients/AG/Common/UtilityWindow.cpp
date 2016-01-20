#include "stdafx.h"
#include "Utilitywindow.h"

CUtilityWindow::CUtilityWindow(void)
{
	m_pCallBack = NULL;
	m_lParam1 = NULL;
	m_lParam2 = NULL;
}

CUtilityWindow::~CUtilityWindow(void)
{
	if (::IsWindow(m_hWnd))
		DestroyWindow();
}
////////////////////////////////////////////////////////////////////////////////////////
bool CUtilityWindow::Initialize(HWND hWnd, UTILITYCALLBACK pCallBack, LPARAM lParam1, LPARAM lParam2, bool bShow)
{	
	if (::IsWindow(m_hWnd))
		return false;

	RECT rect;
    rect.left = rect.right = rect.top = rect.bottom = 0;

    // Create a hidden window 
	HWND hwndParent = GetDesktopWindow();
    HWND hwnd = Create (hwndParent, rect, "Hidden Utility Window");
	if (!hwnd)
		return false;

	m_hCallbackWnd = hWnd;
	m_pCallBack = pCallBack;
	m_lParam1 = lParam1;
	m_lParam2 = lParam2;

	if (bShow)
		ShowWindow(SW_SHOW);
	else
		ShowWindow(SW_HIDE);

	return true;
}
//=============================================================================
//	OnRelayMessage
//=============================================================================
LRESULT CUtilityWindow::OnRelayMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	
	if (m_hCallbackWnd)
	{
		::PostMessage(m_hCallbackWnd, uMsg, wParam, lParam);
	}
	else if (m_pCallBack)
	{
		m_pCallBack(uMsg, m_lParam1, m_lParam2);
	}
	
	return S_OK;
}