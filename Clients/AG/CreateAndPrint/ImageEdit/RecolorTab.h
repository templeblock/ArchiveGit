#pragma once

#include "resource.h"
#include "BaseTab.h"
#include "ColorButton.h"
#include "CustomDrawGradient.h"

// CRecolorTab dialog

class CRecolorTab : public CBaseTab<CRecolorTab>
{
public:
	CRecolorTab();
	virtual ~CRecolorTab();

	BEGIN_MSG_MAP_EX(CRecolorTab)
		COMMAND_HANDLER_EX(IDC_IME_RECOLOR_SOLID, BN_CLICKED, OnRecolorSolid)
		COMMAND_HANDLER_EX(IDC_IME_RECOLOR_SWEEP_DOWN, BN_CLICKED, OnRecolorSweepDown)
		COMMAND_HANDLER_EX(IDC_IME_RECOLOR_SWEEP_RIGHT, BN_CLICKED, OnRecolorSweepRight)
		COMMAND_HANDLER_EX(IDC_IME_RECOLOR_SWEEP_CENTER, BN_CLICKED, OnRecolorSweepFromCenter)
		COMMAND_HANDLER_EX(IDC_IME_RECOLOR_SWAP_COLORS, BN_CLICKED, OnRecolorSwapColors)
		COMMAND_HANDLER_EX(IDC_IME_RECOLOR_PRIMARY1_PATCH, BN_CLICKED, OnRecolorPrimary1Patch)
		COMMAND_HANDLER_EX(IDC_IME_RECOLOR_PRIMARY2_PATCH, BN_CLICKED, OnRecolorPrimary2Patch)
		COMMAND_HANDLER_EX(IDC_IME_RECOLOR_SECONDARY_PATCH, BN_CLICKED, OnRecolorSecondaryPatch)
		COMMAND_HANDLER_EX(IDC_IME_RECOLOR_PRIMARY1_PATCH, BN_DOUBLECLICKED, OnRecolorPrimary1PatchDoubleClick)
		COMMAND_HANDLER_EX(IDC_IME_RECOLOR_PRIMARY2_PATCH, BN_DOUBLECLICKED, OnRecolorPrimary2PatchDoubleClick)
		COMMAND_HANDLER_EX(IDC_IME_RECOLOR_SECONDARY_PATCH, BN_DOUBLECLICKED, OnRecolorSecondaryPatchDoubleClick)
		COMMAND_HANDLER_EX(IDC_IME_APPLY, BN_CLICKED, OnApply)
		COMMAND_HANDLER_EX(IDC_IME_RESET, BN_CLICKED, OnReset)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_HSCROLL(OnHScroll)
		MSG_WM_TIMER(OnTimer)
		CHAIN_MSG_MAP(CBaseTab<CRecolorTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CRecolorTab)
		DDX_CONTROL(IDC_IME_RECOLOR_PRIMARY1_PATCH, m_btnPrimary1Patch);
		DDX_CONTROL(IDC_IME_RECOLOR_PRIMARY2_PATCH, m_btnPrimary2Patch);
		DDX_CONTROL(IDC_IME_RECOLOR_SECONDARY_PATCH, m_btnSecondaryPatch);
		DDX_CONTROL_HANDLE(IDC_IME_RECOLOR_SWAP_COLORS, m_iconSwapColors);
		DDX_CONTROL(IDC_IME_RECOLOR_STATIC_GRADIENT, m_staticGradient);
		DDX_CONTROL_HANDLE(IDC_IME_TOLERANCE, m_ToleranceSlider);
		DDX_CONTROL_HANDLE(IDC_IME_CURRENT_TOLERANCE, m_staticTolerance);
	END_DDX_MAP()

	enum { IDD = IDD_IME_RECOLOR_TAB };

protected:
	virtual BOOL OnInitDialog(HWND hWnd, LPARAM lParam);
	int GetFlags();

private:
	void OnEnterTab(bool bFirstTime = false);
	void OnLeaveTab();
	void OnApply(UINT uCode, int nID, HWND hwndCtrl);
	void OnReset(UINT uCode, int nID, HWND hwndCtrl);
	void OnRecolorSolid(UINT uCode, int nID, HWND hwndCtrl);
	void OnRecolorSweepDown(UINT uCode, int nID, HWND hwndCtrl);
	void OnRecolorSweepRight(UINT uCode, int nID, HWND hwndCtrl);
	void OnRecolorSweepFromCenter(UINT uCode, int nID, HWND hwndCtrl);
	void OnRecolorSwapColors(UINT uCode, int nID, HWND hwndCtrl);
	void OnRecolorPrimary1Patch(UINT uCode, int nID, HWND hwndCtrl);
	void OnRecolorPrimary2Patch(UINT uCode, int nID, HWND hwndCtrl);
	void OnRecolorSecondaryPatch(UINT uCode, int nID, HWND hwndCtrl);
	void OnRecolorPrimary1PatchDoubleClick(UINT uCode, int nID, HWND hwndCtrl);
	void OnRecolorPrimary2PatchDoubleClick(UINT uCode, int nID, HWND hwndCtrl);
	void OnRecolorSecondaryPatchDoubleClick(UINT uCode, int nID, HWND hwndCtrl);
	void OnHScroll(int nSBCode, short nPos, HWND hWnd);
	void OnTimer(UINT nIDEvent, TIMERPROC TimerProc);

private:
	COLORREF m_Primary1Color;
	COLORREF m_Primary2Color;
	COLORREF m_SecondaryColor;
	int m_iTolerance;

	CColorButton1 m_btnPrimary1Patch;
	CColorButton1 m_btnPrimary2Patch;
	CColorButton1 m_btnSecondaryPatch;
	CStatic m_iconSwapColors;
	CStatic m_staticTolerance;
	CStaticGradient m_staticGradient;
	CTrackBarCtrl m_ToleranceSlider;
};
