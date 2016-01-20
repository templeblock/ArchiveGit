#pragma once

#include "resource.h"
#include "BaseTab.h"

// CMainTab dialog

class CMainTab : public CBaseTab<CMainTab>
{
public:
	CMainTab();
	virtual ~CMainTab();

	BEGIN_MSG_MAP_EX(CMainTab)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_BORDERS, BN_CLICKED, OnBorders)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_BORDERS1, BN_CLICKED, OnBorders)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_COLOR, BN_CLICKED, OnColor)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_COLOR1, BN_CLICKED, OnColor)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_CONTRAST_BRIGHTNESS, BN_CLICKED, OnContrastBrightness)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_CONTRAST_BRIGHTNESS1, BN_CLICKED, OnContrastBrightness)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_CROP, BN_CLICKED, OnCrop)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_CROP1, BN_CLICKED, OnCrop)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_EFFECTS, BN_CLICKED, OnEffects)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_EFFECTS1, BN_CLICKED, OnEffects)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_FILE, BN_CLICKED, OnFile)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_FILE1, BN_CLICKED, OnFile)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_FLIP_ROTATE, BN_CLICKED, OnFlipRotate)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_FLIP_ROTATE1, BN_CLICKED, OnFlipRotate)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_RED_EYE, BN_CLICKED, OnRedEye)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_RED_EYE1, BN_CLICKED, OnRedEye)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_RECOLOR, BN_CLICKED, OnRecolor)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_RECOLOR1, BN_CLICKED, OnRecolor)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_RESIZE, BN_CLICKED, OnResize)
		COMMAND_HANDLER_EX(IDC_IME_MAIN_RESIZE1, BN_CLICKED, OnResize)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(CBaseTab<CMainTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CMainTab)
		DDX_CONTROL(IDC_IME_MAIN_BORDERS, m_Borders);
		DDX_CONTROL(IDC_IME_MAIN_COLOR, m_Color);
		DDX_CONTROL(IDC_IME_MAIN_CONTRAST_BRIGHTNESS, m_ContrastBrightness);
		DDX_CONTROL(IDC_IME_MAIN_CROP, m_Crop);
		DDX_CONTROL(IDC_IME_MAIN_EFFECTS, m_Effects);
		DDX_CONTROL(IDC_IME_MAIN_FLIP_ROTATE, m_FlipRotate);
		DDX_CONTROL(IDC_IME_MAIN_RED_EYE, m_RedEye);
		DDX_CONTROL(IDC_IME_MAIN_RECOLOR, m_Recolor);
		DDX_CONTROL(IDC_IME_MAIN_RESIZE, m_Resize);
		DDX_CONTROL(IDC_IME_MAIN_FILE, m_File);
		DDX_CONTROL_HANDLE(IDC_IME_MAIN_BORDERS1, m_iconBorders);
		DDX_CONTROL_HANDLE(IDC_IME_MAIN_COLOR1, m_iconColor);
		DDX_CONTROL_HANDLE(IDC_IME_MAIN_CONTRAST_BRIGHTNESS1, m_iconContrastBrightness);
		DDX_CONTROL_HANDLE(IDC_IME_MAIN_CROP1, m_iconCrop);
		DDX_CONTROL_HANDLE(IDC_IME_MAIN_EFFECTS1, m_iconEffects);
		DDX_CONTROL_HANDLE(IDC_IME_MAIN_FLIP_ROTATE1, m_iconFlipRotate);
		DDX_CONTROL_HANDLE(IDC_IME_MAIN_RED_EYE1, m_iconRedEye);
		DDX_CONTROL_HANDLE(IDC_IME_MAIN_RECOLOR1, m_iconRecolor);
		DDX_CONTROL_HANDLE(IDC_IME_MAIN_RESIZE1, m_iconResize);
	END_DDX_MAP()

	enum { IDD = IDD_IME_MAIN_TAB };

protected:
	virtual BOOL OnInitDialog(HWND hWnd, LPARAM lParam);

private:
	void OnEnterTab(bool bFirstTime = false);
	void OnLeaveTab();
	void OnBorders(UINT uCode, int nID, HWND hwndCtrl);
	void OnColor(UINT uCode, int nID, HWND hwndCtrl);
	void OnContrastBrightness(UINT uCode, int nID, HWND hwndCtrl);
	void OnCrop(UINT uCode, int nID, HWND hwndCtrl);
	void OnEffects(UINT uCode, int nID, HWND hwndCtrl);
	void OnFile(UINT uCode, int nID, HWND hwndCtrl);
	void OnFlipRotate(UINT uCode, int nID, HWND hwndCtrl);
	void OnRedEye(UINT uCode, int nID, HWND hwndCtrl);
	void OnRecolor(UINT uCode, int nID, HWND hwndCtrl);
	void OnResize(UINT uCode, int nID, HWND hwndCtrl);
	void OnTest(UINT uCode, int nID, HWND hwndCtrl);

private:
	CHyperLink m_Borders;
	CHyperLink m_Color;
	CHyperLink m_ContrastBrightness;
	CHyperLink m_Crop;
	CHyperLink m_Effects;
	CHyperLink m_File;
	CHyperLink m_FlipRotate;
	CHyperLink m_RedEye;
	CHyperLink m_Recolor;
	CHyperLink m_Resize;
	CStatic m_iconBorders;
	CStatic m_iconColor;
	CStatic m_iconContrastBrightness;
	CStatic m_iconCrop;
	CStatic m_iconEffects;
	CStatic m_iconFlipRotate;
	CStatic m_iconRedEye;
	CStatic m_iconRecolor;
	CStatic m_iconResize;
};
