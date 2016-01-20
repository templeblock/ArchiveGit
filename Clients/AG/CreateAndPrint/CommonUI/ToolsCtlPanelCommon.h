#pragma once

#include "ToolsCtlPanel.h"
#include "BmpButton.h"
//#include "ColorPickerBtn.h"
#include "CColorLabel.h"
//#include "ColorCombo.h"

class CCtp;

class CToolsCtlPanelCommon : public CToolsCtlPanel
{
public:
	CToolsCtlPanelCommon(CCtp* pMainWnd);
	virtual ~CToolsCtlPanelCommon();

public:
	enum { IDD = IDD_TOOLSPANEL_COMMON };

	COLORREF GetCurrentLineColor() { return m_CurrentLineColor; };
	COLORREF GetCurrentFillColor() { return m_CurrentFillColor; };
	COLORREF GetCurrentFillColor2() { return m_CurrentFillColor2; };
	COLORREF GetCurrentFillColor3() { return m_CurrentFillColor3; };
	void SetFillColor(COLORREF Color);
	void SetFillColor2(COLORREF Color);
	void SetFillColor3(COLORREF Color);
	FillType GetCurrentFillType() { return m_CurrentFillType;};
	int GetCurrentLineWidth() { return m_LineWidth; };

public:
	void UpdateControls();
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFillType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnLineWidth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnColorComboChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CToolsCtlPanelCommon)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

		CHAIN_MSG_MAP(CToolsCtlPanel)
	END_MSG_MAP()

protected:
	void InitToolTips();
	void InitToolCtrls(bool bFillType);
	void InitLineCombos(bool bTextPanel);

	void ShowHideColors();
	//void UpdateFillCtrls();
	//void UpdateLineCtrls();
	void SetFillType(FillType Fill);
	void SetLineWidth(int iLineWidth);
	void SetLineColor(COLORREF Color);

protected:
	CColorCombo m_ColorComboFill;
	CColorCombo m_ColorComboFill2;
	CColorCombo m_ColorComboFill3;
	CColorCombo m_ColorComboLine;

	////////////////////////////
	//CColorPickerBtn m_ColorBtnFill;
	//CColorPickerBtn m_ColorBtnFill2;
	//CColorPickerBtn m_ColorBtnFill3;
	//CColorPickerBtn m_ColorBtnLine;

	//CColorLabel  m_ColorFillDsp;
	//CColorLabel  m_ColorLineDsp;

	COLORREF m_PanelBkgdColor;
	////////////////////////////


	CCombo m_ComboFillType;
	CCombo m_ComboLineWidth;

	COLORREF m_CurrentLineColor;
	COLORREF m_CurrentFillColor;
	COLORREF m_CurrentFillColor2;
	COLORREF m_CurrentFillColor3;
	FillType m_CurrentFillType;
	int m_LineWidth;
};
