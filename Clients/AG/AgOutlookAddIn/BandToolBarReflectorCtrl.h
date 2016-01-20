#pragma once
#include "BandToolBarCtrl.h"

class CBandToolBarReflectorCtrl : public CWindowImpl<CBandToolBarReflectorCtrl, CToolBarCtrl>
{
public:
	CBandToolBarReflectorCtrl(void);
	virtual ~CBandToolBarReflectorCtrl(void);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(CBandToolBarReflectorCtrl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		REFLECT_NOTIFICATIONS()
		//CHAIN_MSG_MAP_MEMBER(m_ToolbarWnd)
	END_MSG_MAP()

// Handler prototypes:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


public:
	inline CBandToolBarCtrl* GetToolBar() { return &m_ToolbarWnd;};

private:
	CBandToolBarCtrl m_ToolbarWnd;

};
