#include "stdafx.h"
#include "BandToolBarReflectorCtrl.h"

CBandToolBarReflectorCtrl::CBandToolBarReflectorCtrl(void)
{
}

CBandToolBarReflectorCtrl::~CBandToolBarReflectorCtrl(void)
{
	if (IsWindow())	
		DestroyWindow();
}
const DWORD DEFAULT_TOOLBAR_STYLE = 
							WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP | 
							CCS_TOP | CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN | 
							TBSTYLE_TOOLTIPS | TBSTYLE_LIST | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT;
		

LRESULT CBandToolBarReflectorCtrl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rect;
	GetClientRect(&rect);
	m_ToolbarWnd.CreateToolBarCtrl(m_hWnd, rect, DEFAULT_TOOLBAR_STYLE);
	//m_ToolbarWnd.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS); 
	return 0;
}