#pragma once

#include "resource.h"
#include "BaseTab.h"

// CCropTab dialog

class CCropTab : public CBaseTab<CCropTab>
{
public:
	CCropTab();
	virtual ~CCropTab();

	BEGIN_MSG_MAP_EX(CCropTab)
		COMMAND_HANDLER_EX(IDC_IME_CROP_PORTRAIT, BN_CLICKED, OnCropPortrait)
		COMMAND_HANDLER_EX(IDC_IME_CROP_LANDSCAPE, BN_CLICKED, OnCropLandscape)
		COMMAND_HANDLER_EX(IDC_IME_CROP_ASPECTRATIO, CBN_SELCHANGE, OnCropAspectRatio)
		COMMAND_HANDLER_EX(IDC_IME_APPLY, BN_CLICKED, OnApply)
		COMMAND_HANDLER_EX(IDC_IME_RESET, BN_CLICKED, OnReset)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_TIMER(OnTimer)
		CHAIN_MSG_MAP(CBaseTab<CCropTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CCropTab)
		DDX_CONTROL_HANDLE(IDC_IME_CROP_ASPECTRATIO, m_comboAspectRatio);
		DDX_CONTROL_HANDLE(IDC_IME_CROP_PORTRAIT, m_radioPortrait);
		DDX_CONTROL_HANDLE(IDC_IME_CROP_LANDSCAPE, m_radioLandscape);
		DDX_CONTROL_HANDLE(IDC_IME_CROP_ORIGINAL, m_staticOriginalDimensions);
		DDX_CONTROL_HANDLE(IDC_IME_CROP_NEW, m_staticNewDimensions);
	END_DDX_MAP()

	enum { IDD = IDD_IME_CROP_TAB };

protected:
	virtual BOOL OnInitDialog(HWND hWnd, LPARAM lParam);

private:
	void OnEnterTab(bool bFirstTime = false);
	void OnLeaveTab();
	void OnApply(UINT uCode, int nID, HWND hwndCtrl);
	void OnReset(UINT uCode, int nID, HWND hwndCtrl);
	void OnCropLandscape(UINT uCode, int nID, HWND hwndCtrl);
	void OnCropPortrait(UINT uCode, int nID, HWND hwndCtrl);
	void OnCropAspectRatio(UINT uCode, int nID, HWND hwndCtrl);
	void OnTimer(UINT nIDEvent, TIMERPROC TimerProc);

private:
	void SetCropAspect();
	void UpdateDimensions();

private:
	CSize m_SizeImage;
	CComboBox m_comboAspectRatio;
	CStatic m_staticOriginalDimensions;
	CStatic m_staticNewDimensions;
	CRect m_CropRect;
	CButton m_radioPortrait;
	CButton m_radioLandscape;
};
