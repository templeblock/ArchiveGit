#pragma once

class CAGBrush;

class CColorScheme
{
public:
	CColorScheme()
	{
		m_ColorTextDialog = NULL;
		m_ColorTextStatic = NULL;
		m_ColorTextMsgBox = NULL;
		m_ColorTextEdit = NULL;
		m_ColorTextListbox = NULL;
		m_ColorTextButton = NULL;

		m_ColorBgDialog = NULL;
		m_ColorBgStatic = NULL;
		m_ColorBgMsgBox = NULL;
		m_ColorBgEdit = NULL;
		m_ColorBgListbox = NULL;
		m_ColorBgButton = NULL;
		m_ColorBgScrollbar = NULL;
		m_LastColor = 0xFFFFFFFF;
	}

	~CColorScheme()
	{
	}

	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam)
	{
		HDC hDC = (HDC)wParam;
		::SetBkMode(hDC, TRANSPARENT); // text is always drawn transparently

		switch (uMsg)
		{
			case WM_CTLCOLORDLG:
			{
				::SetTextColor(hDC, m_ColorTextDialog);
				::SetBkColor(hDC, m_ColorBgDialog);
				return (LRESULT)GetBgBrush(m_ColorBgDialog);
			}
			case WM_CTLCOLORSTATIC:
			{ // Includes radio button and checkbox text
				::SetTextColor(hDC, m_ColorTextStatic);
				::SetBkColor(hDC, m_ColorBgStatic);
				return (LRESULT)GetBgBrush(m_ColorBgStatic);
			}
			case WM_CTLCOLORMSGBOX:
			{
				::SetTextColor(hDC, m_ColorTextMsgBox);
				::SetBkColor(hDC, m_ColorBgMsgBox);
				return (LRESULT)GetBgBrush(m_ColorBgMsgBox);
			}
			case WM_CTLCOLOREDIT:
			{ // Includes combo box text
				::SetTextColor(hDC, m_ColorTextEdit);
				::SetBkColor(hDC, m_ColorBgEdit);
				return (LRESULT)GetBgBrush(m_ColorBgEdit);
			}
			case WM_CTLCOLORLISTBOX:
			{ // Includes combo box dropdown lists
				::SetTextColor(hDC, m_ColorTextListbox);
				::SetBkColor(hDC, m_ColorBgListbox);
				return (LRESULT)GetBgBrush(m_ColorBgListbox);
			}
			case WM_CTLCOLORBTN:
			{ // doesn't work?
				::SetTextColor(hDC, m_ColorTextButton);
				::SetBkColor(hDC, m_ColorBgButton);
				return (LRESULT)GetBgBrush(m_ColorBgButton);
			}
			case WM_CTLCOLORSCROLLBAR:
			{
				::SetBkColor(hDC, m_ColorBgScrollbar);
				return (LRESULT)GetBgBrush(m_ColorBgScrollbar);
			}
			default:
			{
				return NULL;
			}
		}

		return NULL;
	}

private:
	HBRUSH GetBgBrush(COLORREF Color)
	{
		HBRUSH hBrush = m_Brush.GetHBrush();
		if (hBrush && m_LastColor == Color)
			return hBrush;

		hBrush = m_Brush.Create(Color);
		if (!hBrush)
			return NULL;

		m_LastColor = Color;
		return hBrush;
	}

public:
	COLORREF m_ColorTextDialog;
	COLORREF m_ColorTextStatic;
	COLORREF m_ColorTextMsgBox;
	COLORREF m_ColorTextEdit;
	COLORREF m_ColorTextListbox;
	COLORREF m_ColorTextButton;

	COLORREF m_ColorBgDialog;
	COLORREF m_ColorBgStatic;
	COLORREF m_ColorBgMsgBox;
	COLORREF m_ColorBgEdit;
	COLORREF m_ColorBgListbox;
	COLORREF m_ColorBgButton;
	COLORREF m_ColorBgScrollbar;
private:
	CAGBrush m_Brush;
	COLORREF m_LastColor;
};
