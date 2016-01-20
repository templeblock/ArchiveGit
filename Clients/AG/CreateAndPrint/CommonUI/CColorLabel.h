#pragma once

class CColorLabel : public CWindowImpl<CColorLabel, CStatic>
{
public:
	CColorLabel();
	virtual ~CColorLabel();
	void InitDialog(HWND hWndLabel);

	BOOL InitDialogItem (HWND hWnd);
	void SetBkgdColor(COLORREF bkgdColor) { m_bkgdColor = bkgdColor; }
	void SetColor1(COLORREF clrFrom) {m_clrFrom = clrFrom;}
	void SetColor2(COLORREF clrTo) {m_clrTo = clrTo;}
	void SetHorzGradient() { m_bHorzGradient = true; }
	void SetVertGradient() { m_bHorzGradient = false; }
	void SetTransparent(bool bTransparent) { m_bTransparent = bTransparent; }

public:
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);


	BEGIN_MSG_MAP(CColorLabel)
		MESSAGE_HANDLER (WM_PAINT, OnPaint);
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

protected:
	void DrawTranparentRect(CDC& hDC, POINT* pPoints);
	void DrawPolygon(CDC& hDC, POINT* pPoints, int nPoints, COLORREF bkgdClr, COLORREF lineClr);
	void FillLinearGradient(CDC& hDC, POINT* pPoints, COLORREF clrFrom, COLORREF clrTo);
	void FillRectangle(CDC& hDC, POINT* pPoints, COLORREF FillColor);
	void DrawRectangle(CDC& hDC, POINT* pPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	void DrawPath(CDC& hDC, COLORREF LineColor, int LineWidth, COLORREF FillColor);

	COLORREF m_bkgdColor;
	COLORREF m_clrBorder;
	COLORREF m_dfltLineColor;
	COLORREF m_clrFrom;
	COLORREF m_clrTo;

	bool		m_bTransparent;
	bool		m_bHorzGradient;
	HBRUSH		m_hBackgroundBrush;
};
