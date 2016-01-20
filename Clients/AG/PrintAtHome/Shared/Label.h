#pragma once

#include <atlctrls.h>

enum FlashType {None, Text, Background };

class CEditBox : public CWindowImpl<CEditBox, CEdit>
{
public:
	CEditBox() {}
	virtual ~CEditBox() {}
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return (LRESULT)(HBRUSH)GetStockObject(WHITE_BRUSH);
	}

private:
	BEGIN_MSG_MAP(CEditBox)
		MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtlColor)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

};

class CLabel : public CWindowImpl<CLabel, CStatic>
{
public:
	CLabel();
	virtual ~CLabel();
	void InitDialog(HWND hWndLabel);
	void SetBackgroundColor(COLORREF clrBackground);
	void SetTextColor(COLORREF clrText);
	void SetText(const CString& strText);
	void SetFontBold(bool bBold);
	void SetFontName(const CString& strFont);
	void SetFontUnderline(bool bSet);
	void SetFontItalic(bool bSet);
	void SetFontSize(int iSize);
	void SetFontRotation(int iAngle);
	void SetSunken(bool bSet);
	void SetBorder(bool bSet);
	void FlashText(bool bActivate);
	void FlashBackground(bool bActivate, COLORREF clrFlashBackground);
	void SetLink(bool bLink);
	void SetLinkCursor(HCURSOR hCursor);
	void SetUrl(LPCTSTR szUrl);

protected:
	void Redraw();
	void ReconstructFont();

protected:
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CLabel)
		MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtlColor)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

protected:
	COLORREF	m_clrText;
	HBRUSH		m_hBackgroundBrush;
	bool		m_bTransparentDraw;
	LOGFONT		m_lf;
	HFONT		m_hFont;
	int			m_idTimer;
	bool		m_bFlashState;
	CString		m_strFlashSavedText;
	FlashType	m_FlashType;
	HBRUSH		m_hFlashBrush;
	bool		m_bLink;
	HCURSOR		m_hLinkCursor;
	int			m_iAngle;
	CString		m_szUrl;
};
