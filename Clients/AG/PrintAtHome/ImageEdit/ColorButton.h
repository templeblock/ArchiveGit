#pragma once

class CColorButton : public CButton
{
public:
	CColorButton();
	virtual ~CColorButton();

public:
	void SetColor(const COLORREF Color);
	COLORREF GetColor() const;
	bool PickColor();
	void SetCustomColors(COLORREF* pCustomColors, int iCount); // Count cannot exceed 16

protected:
	DECLARE_DYNAMIC(CColorButton)
	DECLARE_MESSAGE_MAP()

private:
	void PreSubclassWindow();
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	COLORREF m_Color;
	COLORREF m_CustomColors[16];
	int m_nCustomColors;
};
