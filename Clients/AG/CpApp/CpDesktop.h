#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "HPapp_i.h"

class CCpDesktopModule :
	public CAtlMfcModule
{
public:
	DECLARE_LIBID(LIBID_HPappLib);
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_HPAPP, "{316936D8-5EE5-4312-8D42-0CD67F42F619}");
};

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

	bool RegistryUpToDate(CString strKeyName, CString strFileVersion);

	DECLARE_MESSAGE_MAP()
	BOOL ExitInstance(void);

};

extern CCpDesktop theApp;
extern CCpDesktopModule _AtlModule;
