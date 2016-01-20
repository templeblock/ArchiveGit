#include "stdafx.h"
#include "CColorLabel.h"
#include <math.h>

/////////////////////////////////////////////////////////////////////////////
CColorLabel::CColorLabel() // : m_wndPicker (this, 2)
{
	m_bkgdColor = RGB(229, 228, 232);
	m_clrBorder = RGB(103, 103, 103);
	m_dfltLineColor = RGB(255, 0, 0);
	m_hBackgroundBrush = ::CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
	m_bTransparent = false;
	m_bHorzGradient = true;
	SetColor1(m_bkgdColor);
	SetColor2(m_bkgdColor);
}

/////////////////////////////////////////////////////////////////////////////
CColorLabel::~CColorLabel()
{
	if (m_hBackgroundBrush)
		::DeleteObject(m_hBackgroundBrush);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CColorLabel::InitDialogItem (HWND hWnd)
{
	if (hWnd)
		SubclassWindow(hWnd);
	ModifyStyle (0, BS_OWNERDRAW);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CColorLabel::OnPaint (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
	ATLTRACE("Label Painting.\n");

	CPaintDC dc (m_hWnd);

	RECT DestRect;
	GetClientRect(&DestRect);

	if (!m_bTransparent)
	{
		POINT pt[4] = { {DestRect.left, DestRect.top}, {DestRect.right, DestRect.top}, {DestRect.right, DestRect.bottom}, {DestRect.left, DestRect.bottom} };
		FillLinearGradient(dc, pt, m_clrFrom, m_clrTo);
	}
	else
	{
		POINT pt[4] = { {DestRect.left, DestRect.top}, {DestRect.right, DestRect.top}, {DestRect.right, DestRect.bottom}, {DestRect.left, DestRect.bottom} };
		DrawTranparentRect(dc, pt);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CColorLabel::DrawTranparentRect(CDC& hDC, POINT* pPoints)
{
	//FillRectangle(hDC, pPoints, m_bkgdColor);
	//POINT pt[6] = { pPoints[3], pPoints[0], pPoints[1], pPoints[2], pPoints[3], pPoints[1] };
	//DrawPolygon(hDC, pt, 6, CLR_NONE, m_dfltLineColor);
	{
		HBRUSH hWhiteBrush = ::CreateSolidBrush(RGB(255, 255, 255));
		HBRUSH pOldBrush = hDC.SelectBrush(hWhiteBrush);
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_clrBorder;
		lb.lbHatch = 0 ;
		DWORD dwPenStyle = PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_ROUND;
		HPEN hBlackPen = ::ExtCreatePen(dwPenStyle, 1, &lb, 0, NULL);
		//HPEN hBlackPen = ::CreatePen(PS_SOLID, 1, m_clrBorder);

		HPEN pOldPen = hDC.SelectPen(hBlackPen);

		hDC.Rectangle(pPoints[0].x, pPoints[0].y, pPoints[2].x, pPoints[2].y);
		hDC.SelectBrush(pOldBrush);
		hDC.SelectPen(pOldPen);

		if (pOldBrush)
			::DeleteObject(pOldBrush);
		if (pOldPen)
			::DeleteObject(pOldPen);

		::BeginPath(hDC);

		::MoveToEx(hDC, pPoints[1].x-2, pPoints[1].y+1, NULL);
		::LineTo(hDC, pPoints[3].x+1, pPoints[3].y-2);
		::CloseFigure(hDC);

		::EndPath(hDC);

		DrawPath(hDC, m_dfltLineColor, 1, CLR_NONE);
	}
}

/////////////////////////////////////////////////////////////////////////////
//void CColorLabel::DrawPolygon(CDC& hDC, POINT* pPoints, int nPoints, COLORREF bkgdClr, COLORREF lineClr)
//{
//	if (!hDC)
//		return;
//
//	::BeginPath(hDC);
//
//	for (int i=0; i<nPoints-1; i++)
//	{
//		::MoveToEx(hDC, pPoints[i].x, pPoints[i].y, NULL);
//		::LineTo(hDC, pPoints[i+1].x, pPoints[i+1].y);
//		::CloseFigure(hDC);
//	}
//
//	::CloseFigure(hDC);
//	::EndPath(hDC);
//	DrawPath(hDC, lineClr, 1, bkgdClr);
//}

#define FRAC_BITS 16
#define FRAC_MASK 0xFFFF
#define STEP_PRECISION 2

//////////////////////////////////////////////////////////////////////
void CColorLabel::FillLinearGradient(CDC& hDC, POINT* pPoints, COLORREF clrFrom, COLORREF clrTo)
{
	if (!hDC)
		return;

	if (clrFrom == clrTo)
	{
		FillRectangle(hDC, pPoints, clrFrom);
		return;
	}
	
	// Compute polygon step increments using fixed point arithmetic: (16.16), (whole, fractional)
	int dx = 0;
	int dy = 0;

	if (m_bHorzGradient)
	{
		dx = pPoints[1].x - pPoints[0].x;
		dy = pPoints[1].y - pPoints[0].y;
	}
	else
	{
		dx = pPoints[2].x - pPoints[1].x;
		dy = pPoints[2].y - pPoints[1].y;
	}
	
	long nSpan = (long)sqrt((double)((dx * dx) + (dy * dy)));
	long nSteps = nSpan / STEP_PRECISION;
	if (!nSteps)
	{
		FillRectangle(hDC, pPoints, clrFrom);
		return;
	}

	CPoint ptStep;
	ptStep.x = (dx << FRAC_BITS) / nSteps;
	ptStep.y = (dy << FRAC_BITS) / nSteps;

	// Setup color increments
	DWORD r1 = GetRValue(clrFrom) << FRAC_BITS;
	DWORD g1 = GetGValue(clrFrom) << FRAC_BITS;
	DWORD b1 = GetBValue(clrFrom) << FRAC_BITS;
	DWORD r2 = GetRValue(clrTo) << FRAC_BITS;
	DWORD g2 = GetGValue(clrTo) << FRAC_BITS;
	DWORD b2 = GetBValue(clrTo) << FRAC_BITS;
	
	long dwRStep = ((long)r2 - (long)r1) / nSteps;
	long dwGStep = ((long)g2 - (long)g1) / nSteps;
	long dwBStep = ((long)b2 - (long)b1) / nSteps;

	// Setup the initial color
	DWORD r = r1;
	DWORD g = g1;
	DWORD b = b1;

	// Setup the initial color bar
	CPoint ptBarHorz[] = {pPoints[0], pPoints[0], pPoints[3], pPoints[3]};
	CPoint ptBarVert[] = {pPoints[1], pPoints[1], pPoints[0], pPoints[0]};
	CPoint* ptBar = (m_bHorzGradient ? ptBarHorz : ptBarVert);
	
	HPEN hOldPen = (HPEN)::SelectObject(hDC, ::GetStockObject(NULL_PEN));
	CBrush Brush;
	CPoint ptFraction(0,0);

	// Draw bars at each increment
	for (int i = 0; i < nSteps; i++)
	{
		// Increment a fractional step
		ptFraction += ptStep;
		long xStep = ptFraction.x >> FRAC_BITS;
		long yStep = ptFraction.y >> FRAC_BITS;
		ptFraction.x &= FRAC_MASK;
		ptFraction.y &= FRAC_MASK;

		// Bump up the bar
		ptBar[1].x += xStep;
		ptBar[1].y += yStep;
		ptBar[2].x += xStep;
		ptBar[2].y += yStep;

		// Bump the color value
		r += dwRStep;
		g += dwGStep;
		b += dwBStep;

		// If the integer part of the color has changed, draw the next bar
		if ((r >> FRAC_BITS) != (r1 >> FRAC_BITS) ||
			(g >> FRAC_BITS) != (g1 >> FRAC_BITS) ||
			(b >> FRAC_BITS) != (b1 >> FRAC_BITS))
		{			
			// Draw the filled bar
			//Brush.Create((BYTE)(r >> FRAC_BITS), (BYTE)(g >> FRAC_BITS), (BYTE)(b >> FRAC_BITS));
			Brush = CreateSolidBrush(RGB((BYTE)(r >> FRAC_BITS), (BYTE)(g >> FRAC_BITS), (BYTE)(b >> FRAC_BITS)));
			HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, Brush);
			::Polygon(hDC, ptBar, 4);
			::SelectObject(hDC, hOldBrush);

			// Reset the color for next cycle
			r1 = r;
			g1 = g;
			b1 = b;

			// Inch the bar along
			ptBar[0] = ptBar[1];
			ptBar[3] = ptBar[2];
		}
	}

	// Draw final bar
	Brush = ::CreateSolidBrush(RGB((BYTE)(r >> FRAC_BITS), (BYTE)(g >> FRAC_BITS), (BYTE)(b >> FRAC_BITS)));
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, Brush);
	ptBar[1] = pPoints[1];
	ptBar[2] = pPoints[2];
	::Polygon(hDC, ptBar, 4);
	::SelectObject(hDC, hOldBrush);
	::SelectObject(hDC, hOldPen);
}

//////////////////////////////////////////////////////////////////////
void CColorLabel::FillRectangle(CDC& hDC, POINT* pPoints, COLORREF FillColor)
{
	DrawRectangle(hDC, pPoints, CLR_NONE/*LineColor*/, 0/*LineWidth*/, FillColor);
}

//////////////////////////////////////////////////////////////////////
void CColorLabel::DrawRectangle(CDC& hDC, POINT* pPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor)
{
	if (!hDC)
		return;

	::BeginPath(hDC);
	::Polygon(hDC, pPoints, 4);
	::EndPath(hDC);

	DrawPath(hDC, LineColor, LineWidth, FillColor);
}

//////////////////////////////////////////////////////////////////////
void CColorLabel::DrawPath(CDC& hDC, COLORREF LineColor, int LineWidth, COLORREF FillColor)
{
	HPEN hPen = NULL;
	HPEN hOldPen = NULL;
	if (LineColor != CLR_NONE)
	{
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = LineColor;
		lb.lbHatch = 0 ;
		DWORD dwPenStyle = PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_ROUND;
		hPen = ::ExtCreatePen(dwPenStyle, LineWidth, &lb, 0, NULL);
		hOldPen = (HPEN)::SelectObject(hDC, hPen);
	}

	CBrush Brush;
	Brush = CreateSolidBrush(FillColor);
	HBRUSH hOldBrush = NULL;
	if (FillColor != CLR_NONE)
		hOldBrush = (HBRUSH)::SelectObject(hDC, Brush);

	if (FillColor != CLR_NONE && LineColor != CLR_NONE)
		::StrokeAndFillPath(hDC);
	else
	if (FillColor != CLR_NONE)
		::FillPath(hDC);
	else
	if (LineColor != CLR_NONE)
		::StrokePath(hDC);

	if (hOldBrush)
		::SelectObject(hDC, hOldBrush);
	if (hOldPen)
		::SelectObject(hDC, hOldPen);
	if (hPen)
		::DeleteObject(hPen);
	if (Brush)
		::DeleteObject(Brush);
}