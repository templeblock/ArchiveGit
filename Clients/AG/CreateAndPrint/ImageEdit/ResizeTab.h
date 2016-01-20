#pragma once

#include "resource.h"
#include "BaseTab.h"

// CResizeTab dialog

class CResizeTab : public CBaseTab<CResizeTab>
{
public:
	CResizeTab();
	virtual ~CResizeTab();

	BEGIN_MSG_MAP_EX(CResizeTab)
		COMMAND_HANDLER_EX(IDC_IME_RESIZE_PREDEFINED_COMBO, CBN_SELCHANGE, OnResizePredefinedChanged)
		COMMAND_HANDLER_EX(IDC_IME_RESIZE_ORIGINAL, BN_CLICKED, OnResizeOriginal)
		COMMAND_HANDLER_EX(IDC_IME_RESIZE_PREDEFINED, BN_CLICKED, OnResizePredefined)
		COMMAND_HANDLER_EX(IDC_IME_RESIZE_CUSTOM, BN_CLICKED, OnResizeCustom)
		COMMAND_HANDLER_EX(IDC_IME_RESIZE_CUSTOM_WIDTH, EN_CHANGE, OnResizeCustomSizeChanged)
		COMMAND_HANDLER_EX(IDC_IME_RESIZE_CUSTOM_HEIGHT, EN_CHANGE, OnResizeCustomSizeChanged)
		COMMAND_HANDLER_EX(IDC_IME_RESIZE_PERCENTAGE, BN_CLICKED, OnResizePercentage)
		COMMAND_HANDLER_EX(IDC_IME_RESIZE_PERCENTAGE_EDIT, EN_CHANGE, OnResizePercentageChanged)
		COMMAND_HANDLER_EX(IDC_IME_APPLY, BN_CLICKED, OnApply)
		COMMAND_HANDLER_EX(IDC_IME_RESET, BN_CLICKED, OnReset)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(CBaseTab<CResizeTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CResizeTab)
		DDX_CONTROL_HANDLE(IDC_IME_RESIZE_ORIGINAL, m_radioOriginalSize);
		DDX_CONTROL_HANDLE(IDC_IME_RESIZE_PREDEFINED, m_radioPredefinedSize);
		DDX_CONTROL_HANDLE(IDC_IME_RESIZE_CUSTOM, m_radioCustomSize);
		DDX_CONTROL_HANDLE(IDC_IME_RESIZE_PERCENTAGE, m_radioPercetageofOriginal);
		DDX_CONTROL_HANDLE(IDC_IME_RESIZE_CUSTOM_WIDTH, m_editCustomWidth);
		DDX_CONTROL_HANDLE(IDC_IME_RESIZE_CUSTOM_HEIGHT, m_editCustomHeight);
		DDX_CONTROL_HANDLE(IDC_IME_RESIZE_PERCENTAGE_EDIT, m_editPercentage);
		DDX_CONTROL_HANDLE(IDC_IME_RESIZE_PREDEFINED_COMBO, m_comboPredefinedSizes);
		DDX_CONTROL_HANDLE(IDC_IME_RESIZE_ORIGINAL_DIMENSIONS, m_staticOriginalDimensions);
		DDX_CONTROL_HANDLE(IDC_IME_RESIZE_NEW_DIMENSIONS, m_staticNewDimensions);
	END_DDX_MAP()

	enum { IDD = IDD_IME_RESIZE_TAB };

protected:
	virtual BOOL OnInitDialog(HWND hWnd, LPARAM lParam);

private:
	void OnEnterTab(bool bFirstTime = false);
	void OnLeaveTab();
	void OnApply(UINT uCode, int nID, HWND hwndCtrl);
	void OnReset(UINT uCode, int nID, HWND hwndCtrl);
	void OnResizeOriginal(UINT uCode, int nID, HWND hwndCtrl);
	void OnResizePredefined(UINT uCode, int nID, HWND hwndCtrl);
	void OnResizePredefinedChanged(UINT uCode, int nID, HWND hwndCtrl);
	void OnResizeCustom(UINT uCode, int nID, HWND hwndCtrl);
	void OnResizeCustomSizeChanged(UINT uCode, int nID, HWND hwndCtrl);
	void OnResizePercentage(UINT uCode, int nID, HWND hwndCtrl);
	void OnResizePercentageChanged(UINT uCode, int nID, HWND hwndCtrl);

private:
	void UpdateDimensions();

private:
	CSize m_SizeImage;
	CSize m_SizeLimits;
	CSize m_Size;
	CComboBox m_comboPredefinedSizes;
	CEdit m_editCustomWidth;
	CEdit m_editCustomHeight;
	CEdit m_editPercentage;
	CButton m_radioOriginalSize;
	CButton m_radioPredefinedSize;
	CButton m_radioCustomSize;
	CButton m_radioPercetageofOriginal;
	CStatic m_staticOriginalDimensions;
	CStatic m_staticNewDimensions;
};
