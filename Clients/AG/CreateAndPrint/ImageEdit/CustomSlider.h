#pragma once
#include "HSL.h"

class CCustomSlider : public CWindowImpl<CCustomSlider, CTrackBarCtrl>, public CCustomDraw<CCustomSlider>
{
public:
	CCustomSlider();
	virtual ~CCustomSlider();

	BEGIN_MSG_MAP_EX(CCustomSlider)
		CHAIN_MSG_MAP_ALT(CCustomDraw<CCustomSlider>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

protected:
	void Init();
	void PrePaintChannel(LPNMCUSTOMDRAW lpNMCustomDraw);
	void PostPaintChannel(LPNMCUSTOMDRAW lpNMCustomDraw);
	void HorizontalGradient(CDC& dc, const CRect& rect, COLORREF clrFrom, COLORREF clrTo);
	void HueGradient(CDC& dc, const CRect& rect, const HSL& HueFrom, const HSL& HueTo);

public:
	void SetStyle(LPCTSTR pstrStyle);
	CString GetStyle();

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
	CString m_strStyle;
};
