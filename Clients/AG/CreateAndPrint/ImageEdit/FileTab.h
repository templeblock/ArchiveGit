#pragma once

#include "resource.h"
#include "BaseTab.h"

// CFileTab dialog

class CFileTab : public CBaseTab<CFileTab>
{
public:
	CFileTab();
	virtual ~CFileTab();

	BEGIN_MSG_MAP_EX(CFileTab)
		COMMAND_HANDLER_EX(IDC_IME_IMAGE_OPEN, BN_CLICKED, OnImageOpen)
		COMMAND_HANDLER_EX(IDC_IME_IMAGE_SAVE, BN_CLICKED, OnImageSave)
		COMMAND_HANDLER_EX(IDC_IME_IMAGE_CLOSE, BN_CLICKED, OnImageClose)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(CBaseTab<CFileTab>)
		REFLECT_NOTIFICATIONS()
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CMainTab)
	END_DDX_MAP()

	enum { IDD = IDD_IME_FILE_TAB };

protected:
	virtual BOOL OnInitDialog(HWND hWnd, LPARAM lParam);

private:
	void OnEnterTab(bool bFirstTime = false);
	void OnLeaveTab();
	void OnImageOpen(UINT uCode, int nID, HWND hwndCtrl);
	void OnImageSave(UINT uCode, int nID, HWND hwndCtrl);
	void OnImageClose(UINT uCode, int nID, HWND hwndCtrl);
};
