#pragma once

#include "resource.h"
#include "BaseTab.h"
//j#include "CustomSlider.h"
#include "CustomDrawGradient.h"

// CContrastBrightnessTab dialog

class CContrastBrightnessTab : public CBaseTab<CContrastBrightnessTab>
{
public:
	CContrastBrightnessTab();
	virtual ~CContrastBrightnessTab();

	BEGIN_MSG_MAP_EX(CContrastBrightnessTab)
		COMMAND_HANDLER_EX(IDC_IME_AUTOCORRECT, BN_CLICKED, OnAutoCorrect)
		COMMAND_HANDLER_EX(IDC_IME_APPLY, BN_CLICKED, OnApply)
		COMMAND_HANDLER_EX(IDC_IME_RESET, BN_CLICKED, OnReset)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_HSCROLL(OnHScroll)
		CHAIN_MSG_MAP(CBaseTab<CContrastBrightnessTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CContrastBrightnessTab)
		DDX_CONTROL(IDC_IME_CONTRAST_SLIDER, m_ContrastSlider);
		DDX_CONTROL(IDC_IME_BRIGHTNESS_SLIDER, m_BrightnessSlider);
		DDX_CONTROL_HANDLE(IDC_IME_CURRENT_CONTRAST, m_staticContrast);
		DDX_CONTROL_HANDLE(IDC_IME_CURRENT_BRIGHTNESS, m_staticBrightness);
	END_DDX_MAP()

	enum { IDD = IDD_IME_CONTRAST_BRIGHTNESS_TAB };

protected:
	virtual BOOL OnInitDialog(HWND hWnd, LPARAM lParam);

private:
	void OnEnterTab(bool bFirstTime = false);
	void OnLeaveTab();
	void OnApply(UINT uCode, int nID, HWND hwndCtrl);
	void OnReset(UINT uCode, int nID, HWND hwndCtrl);
	void OnAutoCorrect(UINT uCode, int nID, HWND hwndCtrl);
	void OnHScroll(int nSBCode, short nPos, HWND hWnd);

private:
	int m_iContrast;
	int m_iBrightness;
	CTrackBarGradient m_ContrastSlider;
	CTrackBarGradient m_BrightnessSlider;
	CStatic m_staticContrast;
	CStatic m_staticBrightness;
};
