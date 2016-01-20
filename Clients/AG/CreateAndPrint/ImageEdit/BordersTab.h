#pragma once

#include "resource.h"
#include "BaseTab.h"


// CBordersTab dialog

class CBordersTab : public CBaseTab<CBordersTab>
{
public:
	CBordersTab();
	virtual ~CBordersTab();

	BEGIN_MSG_MAP_EX(CBordersTab)
		COMMAND_HANDLER_EX(IDC_IME_BORDER_NONE, BN_CLICKED, OnBorderNone)
		COMMAND_HANDLER_EX(IDC_IME_BORDER_IMAGE, BN_CLICKED, OnBorderImage)
		COMMAND_HANDLER_EX(IDC_IME_BORDER_FADE, BN_CLICKED, OnBorderFade)
		COMMAND_HANDLER_EX(IDC_IME_BORDER_IMAGE_CHOOSE, BN_CLICKED, OnBorderImageChoose)
		COMMAND_HANDLER_EX(IDC_IME_APPLY, BN_CLICKED, OnApply)
		COMMAND_HANDLER_EX(IDC_IME_RESET, BN_CLICKED, OnReset)
		COMMAND_HANDLER_EX(IDC_IME_BORDER_FADE_TYPE, CBN_SELCHANGE, OnBorderTypeSelect)
		COMMAND_HANDLER_EX(IDC_IME_BORDER_FADE_STRETCH, BN_CLICKED, OnBorderStretch)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_HSCROLL(OnHScroll)
		CHAIN_MSG_MAP(CBaseTab<CBordersTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CBordersTab)
		DDX_CONTROL_HANDLE(IDC_IME_BORDER_IMAGE_CHOOSE, m_btnChoose);
		DDX_CONTROL_HANDLE(IDC_IME_BORDER_FADE_TYPE, m_comboFadeType);
		DDX_CONTROL_HANDLE(IDC_IME_BORDER_FADE_STRETCH, m_checkFadeStretch);
		DDX_CONTROL_HANDLE(IDC_IME_BORDER_FADE_SIZE, m_FadeSizeSlider);
		DDX_CONTROL_HANDLE(IDC_IME_BORDER_FADE_SOFTNESS, m_FadeSoftnessSlider);
	END_DDX_MAP()

	enum { IDD = IDD_IME_BORDERS_TAB };

public:
	static void CALLBACK MyArtGalleryCallback(HWND hwnd, UINT uMsgId, LPARAM lParam, LPARAM lArtGalleryPtr, LPCTSTR szValue);
	bool LoadBorderImage(LPCTSTR szFileName);
	

protected:
	virtual BOOL OnInitDialog(HWND hWnd, LPARAM lParam);

private:
	void OnEnterTab(bool bFirstTime = false);
	void OnLeaveTab();
	void OnApply(UINT uCode, int nID, HWND hwndCtrl);
	void OnReset(UINT uCode, int nID, HWND hwndCtrl);
	void OnBorderNone(UINT uCode, int nID, HWND hwndCtrl);
	void OnBorderImage(UINT uCode, int nID, HWND hwndCtrl);
	void OnBorderFade(UINT uCode, int nID, HWND hwndCtrl);
	void OnBorderImageChoose(UINT uCode, int nID, HWND hwndCtrl);
	void OnBorderTypeSelect(UINT uCode, int nID, HWND hwndCtrl);
	void OnBorderStretch(UINT uCode, int nID, HWND hwndCtrl);
	void OnHScroll(int nSBCode, short nPos, HWND hWnd);

private:
	bool m_bFadeStretch;
	CString m_strClipArtInfo;
	int m_iFadeSize;
	int m_iFadeSoftness;
	CString m_strBorderImageFileName;
	CButton m_btnChoose;
	CComboBox m_comboFadeType;
	CButton m_checkFadeStretch;
	CTrackBarCtrl m_FadeSizeSlider;
	CTrackBarCtrl m_FadeSoftnessSlider;
	
};
