#pragma once

class CColorButton1 : public CWindowImpl<CColorButton1, CButton>, public CCustomDraw<CColorButton1>
{
public:
	CColorButton1();
	virtual ~CColorButton1();

	BEGIN_MSG_MAP_EX(CColorButton1)
		CHAIN_MSG_MAP_ALT(CCustomDraw<CColorButton1>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	void SetColor(const COLORREF Color);
	COLORREF GetColor() const;
	bool PickColor();
	void SetCustomColors(COLORREF* pCustomColors, int iCount); // Count cannot exceed 16

protected:
	void Init();

public:
	// CCustomDraw Overrideables
	DWORD OnPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw);
	DWORD OnPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw);
	DWORD OnPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw);
	DWORD OnPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw);
	DWORD OnItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw);
	DWORD OnItemPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw);
	DWORD OnItemPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw);
	DWORD OnItemPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw);
	DWORD OnSubItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw);

protected:
	COLORREF m_Color;
	COLORREF m_CustomColors[16];
	int m_nCustomColors;
};
