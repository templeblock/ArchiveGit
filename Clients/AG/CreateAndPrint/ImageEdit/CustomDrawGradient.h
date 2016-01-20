#pragma once
#include "HSL.h"

class CStaticGradient;
class CTrackBarGradient;

#define CDG_SOLID		1
#define CDG_VERTICAL	2
#define CDG_HORIZONTAL	4
#define CDG_FROMCENTER	8
#define CDG_HUE			16
#define CDG_CONTRAST	32
#define CDG_HASITEMS	64

class CCustomDrawGradient
{
public:
	CCustomDrawGradient();
	virtual ~CCustomDrawGradient();

	void SetStyle(HWND hWnd, DWORD dwFlags, COLORREF ColorFrom, COLORREF ColorTo);

protected:
	void Init(HWND hWnd);
	void PrePaintChannel(LPNMCUSTOMDRAW lpNMCustomDraw);
	void PostPaintChannel(LPNMCUSTOMDRAW lpNMCustomDraw);
	void DrawGradient(CDC& dc, const CRect& rect, COLORREF clrFrom, COLORREF clrTo);
	void DrawSolid(CDC& dc, const CRect& rect, COLORREF clrFrom, COLORREF clrTo);
	void DrawVertical(CDC& dc, const CRect& rect, COLORREF clrFrom, COLORREF clrTo);
	void DrawHorizontal(CDC& dc, const CRect& rect, COLORREF clrFrom, COLORREF clrTo);
	void DrawFromCenter(CDC& dc, const CRect& rect, COLORREF clrFrom, COLORREF clrTo);
	void DrawHorizontalHues(CDC& dc, const CRect& rect, const HSL& HueFrom, const HSL& HueTo);

protected:
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
	CWindow m_Window;
	COLORREF m_ColorFrom;
	COLORREF m_ColorTo;
	DWORD m_dwFlags;
};

class CStaticGradient : public CWindowImpl<CStaticGradient, CStatic>, public CCustomDraw<CStaticGradient>, public CCustomDrawGradient
{
public:
	CStaticGradient() {}
	virtual ~CStaticGradient() {}

public:
	// CCustomDraw Overrideables
	DWORD OnPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnPrePaint(idCtrl, lpNMCustomDraw); }
	DWORD OnPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnPostPaint(idCtrl, lpNMCustomDraw); }
	DWORD OnPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnPreErase(idCtrl, lpNMCustomDraw); }
	DWORD OnPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnPostErase(idCtrl, lpNMCustomDraw); }
	DWORD OnItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnItemPrePaint(idCtrl, lpNMCustomDraw); }
	DWORD OnItemPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnItemPostPaint(idCtrl, lpNMCustomDraw); }
	DWORD OnItemPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnItemPreErase(idCtrl, lpNMCustomDraw); }
	DWORD OnItemPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnItemPostErase(idCtrl, lpNMCustomDraw); }
	DWORD OnSubItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnSubItemPrePaint(idCtrl, lpNMCustomDraw); }

	BEGIN_MSG_MAP_EX(CStaticGradient)
		CHAIN_MSG_MAP_ALT(CCustomDraw<CStaticGradient>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
};

class CTrackBarGradient : public CWindowImpl<CTrackBarGradient, CTrackBarCtrl>, public CCustomDraw<CTrackBarGradient>, public CCustomDrawGradient
{
public:
	CTrackBarGradient() {}
	virtual ~CTrackBarGradient() {}

public:
	// CCustomDraw Overrideables
	DWORD OnPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnPrePaint(idCtrl, lpNMCustomDraw); }
	DWORD OnPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnPostPaint(idCtrl, lpNMCustomDraw); }
	DWORD OnPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnPreErase(idCtrl, lpNMCustomDraw); }
	DWORD OnPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnPostErase(idCtrl, lpNMCustomDraw); }
	DWORD OnItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnItemPrePaint(idCtrl, lpNMCustomDraw); }
	DWORD OnItemPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnItemPostPaint(idCtrl, lpNMCustomDraw); }
	DWORD OnItemPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnItemPreErase(idCtrl, lpNMCustomDraw); }
	DWORD OnItemPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnItemPostErase(idCtrl, lpNMCustomDraw); }
	DWORD OnSubItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{ return CCustomDrawGradient::OnSubItemPrePaint(idCtrl, lpNMCustomDraw); }

	BEGIN_MSG_MAP_EX(CTrackBarGradient)
		CHAIN_MSG_MAP_ALT(CCustomDraw<CTrackBarGradient>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
};
