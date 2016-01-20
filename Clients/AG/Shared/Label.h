#pragma once

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
	void SetText(LPCSTR strText);
	void SetFontBold(bool bBold);
	void SetFontName(LPCTSTR strFont);
	void SetFontUnderline(bool bSet);
	void SetFontItalic(bool bSet);
	void SetFontSize(int iSize);
	void SetFontRotation(int iAngle);
	void SetSunken(bool bSet);
	void SetBorder(bool bSet);

protected:
	void Redraw();
	void ReconstructFont();

protected:
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CLabel)
		MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtlColor)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

protected:
	COLORREF	m_clrText;
	HBRUSH		m_hBackgroundBrush;
	bool		m_bTransparentDraw;
	LOGFONT		m_lf;
	HFONT		m_hFont;
	int			m_iAngle;
};
