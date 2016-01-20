#pragma once

#include "resource.h"
#include "AGDoc.h"
#include "AGText.h"
#include "Font.h"
#include "ColorScheme.h"
#include "Label.h"
#include "BmpButton.h"
#include "ColorCombo.h"
#include "Combo.h"

class CCtp;

const UINT SELECTED_PANEL = -1;
const UINT DEFAULT_PANEL = 0;
const UINT TEXT_PANEL = 1;
const UINT IMAGE_PANEL = 2;
const UINT GRAPHICS_PANEL = 3;
const UINT CALENDAR_PANEL = 4;
const UINT NUM_PANELS = 5;

/////////////////////////////////////////////////////////////////////////////
class CCtlPanel : public CDialogImpl<CCtlPanel>
{
public:
	CCtlPanel(CCtp* pMainWnd, UINT nResID);
	virtual ~CCtlPanel();

	//enum { IDD = m_nResID };

	HWND Create(HWND hWndParent);
	void ActivateNewDoc();
	virtual void UpdateControls();
	void UpdateTextControls(CAGText* pText);
	void SetFonts();
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFont(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFillType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnLineWidth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnTextSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnColorComboChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBoldItalicUnderline(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnShapeNameComboChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBackButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

public:
	COLORREF GetCurrentLineColor() { return m_CurrentLineColor; };
	COLORREF GetCurrentFillColor() { return m_CurrentFillColor; };
	COLORREF GetCurrentFillColor2() { return m_CurrentFillColor2; };
	COLORREF GetCurrentFillColor3() { return m_CurrentFillColor3; };
	FillType GetCurrentFillType() { return m_CurrentFillType;};
	int GetCurrentLineWidth() { return m_LineWidth; };
	int GetCurrentCalendarMonth() { return m_iCalendarMonth; };
	int GetCurrentCalendarYear() { return m_iCalendarYear; };
	int GetCurrentTextSize() { return m_iTextSize; };
	int GetCurrentShape() { return m_iCurrentShape; };
	void ShowHideColors();

protected:
	void InitLabels();
	void InitLineCombos(bool bTextPanel);

	void AddToolTip(UINT idControl);
	void SetShapeName(LPCSTR pShapeName);
	void SetFillType(FillType Fill);
	void SetFillColor(COLORREF Color);
	void SetFillColor2(COLORREF Color);
	void SetFillColor3(COLORREF Color);
	void SetLineWidth(int iLineWidth);
	void SetLineColor(COLORREF Color);
	void SetTextSize(int iSize);
	void SetFontControl(LOGFONT* pFont, bool bClearIfBad);
	void SetTitleText(LPCSTR Str);

public:
	BEGIN_MSG_MAP(CCtlPanel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSCROLLBAR, OnCtlColor)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)

		COMMAND_ID_HANDLER(IDC_BOLD, OnBoldItalicUnderline)
		COMMAND_ID_HANDLER(IDC_ITALIC, OnBoldItalicUnderline)
		COMMAND_ID_HANDLER(IDC_UNDERLINE, OnBoldItalicUnderline)

		COMMAND_ID_HANDLER(IDC_FONT, OnFont)
		COMMAND_ID_HANDLER(IDC_FILL_TYPE, OnFillType)
		COMMAND_ID_HANDLER(IDC_LINE_SIZE, OnLineWidth)
		COMMAND_ID_HANDLER(IDC_TEXT_SIZE, OnTextSize)

		COMMAND_ID_HANDLER(IDC_FILL_COLOR, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_FILL_COLOR2, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_FILL_COLOR3, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_LINE_COLOR, OnColorComboChanged)

		COMMAND_ID_HANDLER(IDC_SHAPE_NAME, OnShapeNameComboChanged)

		COMMAND_ID_HANDLER(IDC_BACK, OnBackButton)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	UINT m_nResID;

protected:
	CCtp* m_pCtp;
	CColorScheme m_ColorScheme;
	COLORREF m_CurrentLineColor;
	COLORREF m_CurrentFillColor;
	COLORREF m_CurrentFillColor2;
	COLORREF m_CurrentFillColor3;
	FillType m_CurrentFillType;
	int m_LineWidth;
	int m_iTextSize;
	int m_iCurrentShape;
	int m_iCalendarMonth;
	int m_iCalendarYear;
	CToolTipCtrl m_ToolTip;
	CLabel m_LabelType;
	CLabel m_LabelInstruct;
	CLabel m_LabelSmall1;
	CLabel m_LabelBig1;
	CLabel m_LabelBig2;
	CLabel m_LabelBig3;
	CLabel m_LabelBig4;
	CBmpButton m_ButtonBold;
	CBmpButton m_ButtonItalic;
	CBmpButton m_ButtonUnderline;
	CBmpButton m_ButtonLeft;
	CBmpButton m_ButtonCenter;
	CBmpButton m_ButtonRight;
	CBmpButton m_ButtonTop;
	CBmpButton m_ButtonMiddle;
	CBmpButton m_ButtonBottom;
	CBmpButton m_ButtonPrev;
	CColorCombo m_ColorComboFill;
	CColorCombo m_ColorComboFill2;
	CColorCombo m_ColorComboFill3;
	CColorCombo m_ColorComboLine;
	CCombo m_ComboFillType;
	CCombo m_ComboLineWidth;
	CCombo m_ComboTextSize;
	CCombo m_ComboShapeName;
	CCombo m_ComboFonts;

	CImageList m_ImageList;
};
