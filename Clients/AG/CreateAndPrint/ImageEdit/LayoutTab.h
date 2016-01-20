#pragma once

#include "resource.h"
#include "BaseTab.h"

// CFlipRotateTab dialog

class CFlipRotateTab : public CBaseTab<CFlipRotateTab>
{
public:
	CFlipRotateTab();
	virtual ~CFlipRotateTab();

	BEGIN_MSG_MAP_EX(CFlipRotateTab)
		COMMAND_HANDLER_EX(IDC_IME_FLIP_HORIZONTAL, BN_CLICKED, OnFlipHorizontal)
		COMMAND_HANDLER_EX(IDC_IME_FLIP_VERTICAL, BN_CLICKED, OnFlipVertical)
		COMMAND_HANDLER_EX(IDC_IME_ROTATE_LEFT, BN_CLICKED, OnRotateLeft)
		COMMAND_HANDLER_EX(IDC_IME_ROTATE_RIGHT, BN_CLICKED, OnRotateRight)
		COMMAND_HANDLER_EX(IDC_IME_APPLY, BN_CLICKED, OnApply)
		COMMAND_HANDLER_EX(IDC_IME_RESET, BN_CLICKED, OnReset)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_HSCROLL(OnHScroll)
		CHAIN_MSG_MAP(CBaseTab<CFlipRotateTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CFlipRotateTab)
		DDX_CONTROL_HANDLE(IDC_IME_ROTATE_SLIDER, m_RotateSlider);
		DDX_CONTROL_HANDLE(IDC_IME_ROTATE_AMOUNT, m_staticRotateAmount);
	END_DDX_MAP()

	enum { IDD = IDD_IME_FLIP_ROTATE_TAB };

protected:
	virtual BOOL OnInitDialog(HWND hWnd, LPARAM lParam);

private:
	void OnEnterTab(bool bFirstTime = false);
	void OnLeaveTab();
	void OnApply(UINT uCode, int nID, HWND hwndCtrl);
	void OnReset(UINT uCode, int nID, HWND hwndCtrl);
	void OnHScroll(int nSBCode, short nPos, HWND hWnd);
	void OnFlipHorizontal(UINT uCode, int nID, HWND hwndCtrl);
	void OnFlipVertical(UINT uCode, int nID, HWND hwndCtrl);
	void OnRotateLeft(UINT uCode, int nID, HWND hwndCtrl);
	void OnRotateRight(UINT uCode, int nID, HWND hwndCtrl);

private:
	int PositiveDegrees();

private:
	int m_iDegrees;
	CTrackBarCtrl m_RotateSlider;
	CStatic m_staticRotateAmount;
};
