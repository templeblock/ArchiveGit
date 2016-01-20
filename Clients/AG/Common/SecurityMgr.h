#pragma once

class CSecurityMgr
{
public:
	CSecurityMgr(void);
	virtual ~CSecurityMgr(void);

	bool FixupSecurityZone(bool bSilent, DWORD dwClient);
	bool FixupRunActiveX(bool bSilent, DWORD dwClient);
	bool FixupHtmlSettings(bool bSilent, DWORD dwClient);
	bool FixupEmailEditor(bool bSilent, bool bAllowChange);
	bool IsUrlActionAllowed(HWND hIEWindow, DWORD dwUrlAction, LPCWSTR pwszUrl);
	bool CSecurityMgr::EnableBrowserExtensions(bool bSilent);

protected:
	IInternetSecurityManager* GetInternetSecurityManager(HWND hCompose);

protected:
	HINSTANCE m_hMSAAInst;
	bool m_bMustRestartToSetZone;
	bool m_bMustRestartToSetRunActiveX;
	bool m_bMustRestartToSetHtmlSettings;
	bool m_bMustRestartToSetEmailEditor;
};
