#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

class CCpDesktop : public CWinApp
{
public:
	CCpDesktop();
	~CCpDesktop();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
	bool FirstInstance();

	DECLARE_MESSAGE_MAP()
};

extern CCpDesktop theApp;
