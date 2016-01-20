#pragma once

#include "resource.h"
#include "BaseTab.h"

// CEffectsTab dialog

class CEffectsTab : public CBaseTab<CEffectsTab>
{
public:
	CEffectsTab();
	virtual ~CEffectsTab();

	BEGIN_MSG_MAP_EX(CEffectsTab)
		COMMAND_HANDLER_EX(IDC_IME_EFFECTS_LIST, LBN_SELCHANGE, OnEffectsListChanged)
		COMMAND_HANDLER_EX(IDC_IME_MAKEGRAY, BN_CLICKED, OnMakeGray)
		COMMAND_HANDLER_EX(IDC_IME_INVERT, BN_CLICKED, OnInvert)
		COMMAND_HANDLER_EX(IDC_IME_APPLY, BN_CLICKED, OnApply)
		COMMAND_HANDLER_EX(IDC_IME_RESET, BN_CLICKED, OnReset)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_HSCROLL(OnHScroll)
		CHAIN_MSG_MAP(CBaseTab<CEffectsTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CEffectsTab)
		DDX_CONTROL_HANDLE(IDC_IME_EFFECTS_LIST, m_listEffects);
		DDX_CONTROL_HANDLE(IDC_IME_EFFECTS_SLIDER, m_IntensitySlider);
		DDX_CONTROL_HANDLE(IDC_IME_EFFECTS_AMOUNT, m_staticIntensity);
	END_DDX_MAP()

	enum { IDD = IDD_IME_EFFECTS_TAB };

protected:
	virtual BOOL OnInitDialog(HWND hWnd, LPARAM lParam);

private:
	void OnEnterTab(bool bFirstTime = false);
	void OnLeaveTab();
	void OnApply(UINT uCode, int nID, HWND hwndCtrl);
	void OnReset(UINT uCode, int nID, HWND hwndCtrl);
	void OnEffectsListChanged(UINT uCode, int nID, HWND hwndCtrl);
	void OnMakeGray(UINT uCode, int nID, HWND hwndCtrl);
	void OnInvert(UINT uCode, int nID, HWND hwndCtrl);
	void OnHScroll(int nSBCode, short nPos, HWND hWnd);

private:
	int m_iIntensity;
	CListBox m_listEffects;
	CTrackBarCtrl m_IntensitySlider;
	CStatic m_staticIntensity;
};
