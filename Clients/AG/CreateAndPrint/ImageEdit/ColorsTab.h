#pragma once

#include "resource.h"
#include "BaseTab.h"
#include "CustomDrawGradient.h"

// CColorsTab dialog

class CColorsTab : public CBaseTab<CColorsTab>
{
public:
	CColorsTab();
	virtual ~CColorsTab();

	BEGIN_MSG_MAP_EX(CColorsTab)
		COMMAND_HANDLER_EX(IDC_IME_APPLY, BN_CLICKED, OnApply)
		COMMAND_HANDLER_EX(IDC_IME_RESET, BN_CLICKED, OnReset)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_HSCROLL(OnHScroll)
		CHAIN_MSG_MAP(CBaseTab<CColorsTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CColorsTab)
		DDX_CONTROL(IDC_IME_AMOUNT_SLIDER, m_AmountSlider);
		DDX_CONTROL(IDC_IME_HUE_SLIDER, m_HueSlider);
		DDX_CONTROL(IDC_IME_SATURATION_SLIDER, m_SaturationSlider);
		DDX_CONTROL_HANDLE(IDC_IME_CURRENT_AMOUNT, m_staticAmount);
		DDX_CONTROL_HANDLE(IDC_IME_CURRENT_HUE, m_staticHue);
		DDX_CONTROL_HANDLE(IDC_IME_CURRENT_SATURATION, m_staticSaturation);
	END_DDX_MAP()

	enum { IDD = IDD_IME_COLORS_TAB };

protected:
	virtual BOOL OnInitDialog(HWND hWnd, LPARAM lParam);

private:
	void OnEnterTab(bool bFirstTime = false);
	void OnLeaveTab();
	void OnApply(UINT uCode, int nID, HWND hwndCtrl);
	void OnReset(UINT uCode, int nID, HWND hwndCtrl);
	void OnHScroll(int nSBCode, short nPos, HWND hWnd);

private:
	int m_iAmount;
	int m_iHue;
	int m_iSaturation;
	CTrackBarGradient m_AmountSlider;
	CTrackBarGradient m_HueSlider;
	CTrackBarGradient m_SaturationSlider;
	CStatic m_staticAmount;
	CStatic m_staticHue;
	CStatic m_staticSaturation;
};
