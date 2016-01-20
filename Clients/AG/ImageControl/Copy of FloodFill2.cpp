// DibData.cpp

#include "stdafx.h"
#include "FloodFill2.h"
#include "HSL.h"
#include "Utility.h"
#include <math.h>

/////////////////////////////////////////////////////////////////////////////
CFloodFill2::CFloodFill2()
{
	m_pDib = NULL;
	m_pBufferDib = NULL;
	m_fTolerance = 0.0;
	m_bRedEye = false;
	m_bCopy = false;
}

/////////////////////////////////////////////////////////////////////////////
CFloodFill2::~CFloodFill2()
{
	if (m_bCopy)
		DibFree(m_pDib);
	m_pDib = NULL;
}

/////////////////////////////////////////////////////////////////////////////
void CFloodFill2::SetTolerance(int nValue)
{
	m_fTolerance = (double)nValue * 2.55;
}

/////////////////////////////////////////////////////////////////////////////
bool CFloodFill2::InitDIB(BITMAPINFOHEADER* pbmpSrc, bool bCopy)
{
	m_bCopy = bCopy;
	if (m_bCopy)
		m_pDib = DibCopy(pbmpSrc);
	else
		m_pDib = pbmpSrc;

	return true; 
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CFloodFill2::GetPixel(int x, int y) const
{
	if (!m_pDib || m_pDib->biBitCount != 24)
		return CLR_NONE;

	LPBYTE pPixel = DibPtrXYExact(m_pDib, x, y);
	BYTE B = *pPixel++;
	BYTE G = *pPixel++;
	BYTE R = *pPixel++;
	return RGB(R, G, B);
}

/////////////////////////////////////////////////////////////////////////////
bool CFloodFill2::DrawLineH(int x1, int x2, int y, COLORREF Color, const CRect& Rect)
{
	if (!m_pDib)
		return false;

	if (!m_bRedEye && Color == CLR_NONE)
		return false;

	if (x1 > x2)
		x1^=x2, x2^=x1, x1^=x2;

	if (x1 < 0 && x2 < 0)
		return false;

	LONG nWidth = GetWidth();
	if (x1 >= nWidth && x2 >= nWidth)
		return false;

	if (x1 < 0)
		x1 = 0;

	if (x2 >= nWidth)
		x2 = nWidth - 1;

	int x = x1;
	LPBYTE pPixel = DibPtrXYExact(m_pDib, x1, y);
	LPBYTE pEnd = DibPtrXYExact(m_pDib, x2, y);

	while (pPixel <= pEnd)
	{
		if (m_pBufferDib)
		{
			if (!m_bRedEye)
				Color = m_pBufferDib->GetPixel(x++, y);
			else
			{
				Color = GetPixel(x++, y); 
				RedEyeRemove(Color);
			}
		}

		*pPixel++ = GetBValue(Color);
		*pPixel++ = GetGValue(Color);
		*pPixel++ = GetRValue(Color);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CFloodFill2::RedEyeRemove(COLORREF& Color)
{
	BYTE R = GetRValue(Color);
	BYTE G = GetGValue(Color);
	BYTE B = GetBValue(Color);

	// Try to adjust the white pixels
	int nWhiteIndex = 0;
	if (R >= 200)
		nWhiteIndex++;

	if (G >= 200)
		nWhiteIndex++;

	if (B >= 200)
		nWhiteIndex++;

	// Assume that the pixel was meant to be white
	if (nWhiteIndex > 1)
	{
		int nWhiteLevel = max(R, max(G, B));
		Color = RGB(nWhiteLevel, nWhiteLevel, nWhiteLevel);
	}
	else
	{
		R = max(G, B);
		Color = RGB(min(R, 255), min(G * 1.20, 255), min(B * 1.20, 255));
	}
}

/////////////////////////////////////////////////////////////////////////////
int CFloodFill2::ScanRight(int x, int y, int xmax, COLORREF Color, bool bMatch)
{
	if (!m_pDib || m_pDib->biBitCount != 24)
		return ++xmax;

	if (xmax < 0)
		return ++xmax;

	LONG nWidth = GetWidth();
	if (x < 0 || nWidth  <= x || y < 0 || abs(GetHeight()) <= y)
		return ++xmax;

	if (Color == CLR_NONE || nWidth <= xmax)
		return ++xmax;

	LPBYTE pPixel = DibPtrXYExact(m_pDib, x, y);
	for (; x <= xmax; ++x)
//j	while (x <= xmax)
	{
//j		x++;
		BYTE B = *pPixel++;
		BYTE G = *pPixel++;
		BYTE R = *pPixel++;
		COLORREF PixelColor = RGB(R, G, B);
		bool bOK = IsColorToBeFilled(Color, PixelColor);
		if (bOK == bMatch)
			break;
	}

	return x;
}

/////////////////////////////////////////////////////////////////////////////
int CFloodFill2::ScanLeft(int x, int y, int xmin, COLORREF Color, bool bMatch)
{
	if (!m_pDib || m_pDib->biBitCount != 24)
		return --xmin;

	if (xmin < 0)
		return --xmin;

	LONG nWidth = GetWidth();
	if (x < 0 || nWidth  <= x || y < 0 || abs(GetHeight()) <= y)
		return --xmin;

	if (Color == CLR_NONE || nWidth <= xmin)
		return --xmin;

	LPBYTE pPixel = DibPtrXYExact(m_pDib, x, y);
	for (; x >= xmin; --x)
//j	while (x >= xmin)
	{
//j		x--;
		BYTE B = *pPixel++;
		BYTE G = *pPixel++;
		BYTE R = *pPixel++;
		COLORREF PixelColor = RGB(R, G, B);
		bool bOK = IsColorToBeFilled(Color, PixelColor);
		if (bOK == bMatch)
			break;
	}

	return x;
}

/////////////////////////////////////////////////////////////////////////////
bool CFloodFill2::IsColorToBeFilled(COLORREF FillColor, COLORREF PixelColor)
{
	BYTE R = GetRValue(PixelColor);
	BYTE G = GetGValue(PixelColor);
	BYTE B = GetBValue(PixelColor);

	if (m_bRedEye)
	{
		// Try to adjust the white pixels if the user clicks directly on a white part of an eye.
		// I need to account for this and continue. This may not be pure white in the picture. 
		int nWhiteIndex = 0;
		if (R >= 200)
			nWhiteIndex++;

		if (G >= 200)
			nWhiteIndex++;

		if (B >= 200)
			nWhiteIndex++;

		// Assume that the pixel was meant to be white
		if (nWhiteIndex > 1)
			return true;
			
		HSL hsl;
		RGBtoHSL(R, G, B, &hsl);
		if ((hsl.hue >= 0 && hsl.hue <= 10) || hsl.hue >= 200)
		{
			if (hsl.sat < 98 && hsl.lum < 98)
				return false;

			float dR = ((float)R / (float)(R + G + B));
			float dG = ((float)G / (float)(R + G + B));
			float dB = ((float)B / (float)(R + G + B));
			if (R > 50 && dR > 0.40 && dG < 0.31 && dB < 0.36)
			{
				if ((4 * R -(G + B) - min(G,B) - max(G,B))/R)
					return true;
			}
		}
	}
	else
	{
		if (FillColor == PixelColor)
			return true;

		int dR = abs((int)GetRValue(FillColor) - R);
		int dG = abs((int)GetGValue(FillColor) - G);
		int dB = abs((int)GetBValue(FillColor) - B);
		int nMaxDiff = max(max(dR, dG), dB);
		if (nMaxDiff <= m_fTolerance)
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
void CFloodFill2::DrawRedEyeCircle(const CRect& Rect)
{
	// First lets draw a circle in the bitmap with a radius about a third smaller
	// then the smallest dimension of the bitmap
	int nDiameter = (min(Rect.Height(), Rect.Width()) * 2) / 3;
	CRect rcCircle(0, 0, nDiameter, nDiameter);
	rcCircle.OffsetRect((Rect.Width() - nDiameter) / 2, (Rect.Height() - nDiameter)/2);

	// For each pixel inside the circle...
	CPoint ptCenter   = rcCircle.CenterPoint();
	int nSqrRadius = nDiameter * nDiameter / 4;
	for (int x=rcCircle.left; x<rcCircle.right; x++)
	{
		int nDX = x - ptCenter.x;
		for (int y=rcCircle.top; y<rcCircle.bottom; y++)
		{
			int nDY = y - ptCenter.y;
			if ((nDX * nDX + nDY * nDY) >= nSqrRadius)
				continue;

			COLORREF Color = GetPixelRedEye(rcCircle.left + x, rcCircle.top + y);
			LPBYTE pPixel = DibPtrXYExact(m_pDib, rcCircle.left + x, rcCircle.top + y);
			*pPixel++ = GetBValue(Color);
			*pPixel++ = GetGValue(Color);
			*pPixel++ = GetRValue(Color);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CFloodFill2::GetPixelRedEye(int x, int y)
{
	COLORREF Color = GetPixel(x, y);
	BYTE R = GetRValue(Color);
	BYTE G = GetGValue(Color);
	BYTE B = GetBValue(Color);

	// Try to adjust the white pixels
	int nWhiteIndex = 0;
	if (R >= 200)
		nWhiteIndex++;

	if (G >= 200)
		nWhiteIndex++;

	if (B >= 200)
		nWhiteIndex++;

	// Assume that the pixel was meant to be white
	if (nWhiteIndex > 1)
	{
		int nWhiteLevel = max(R, max(G, B));
		return RGB(nWhiteLevel, nWhiteLevel, nWhiteLevel);
	}

	R = min(G, B);
	return RGB(R, G, B);
}

/////////////////////////////////////////////////////////////////////////////
bool CFloodFill2::FillRectBand(COLORREF Color, const CRect& Rect)
{
	for (int y = Rect.top; y < Rect.bottom; y++)
	{
		LPBYTE pPixel = DibPtrXYExact(m_pDib, Rect.left, y);
		LPBYTE pEnd = DibPtrXYExact(m_pDib, Rect.right, y);

		while (pPixel < pEnd)
		{
			*pPixel++ = GetBValue(Color);
			*pPixel++ = GetGValue(Color);
			*pPixel++ = GetRValue(Color);
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CFloodFill2::DrawGradRect(const CRect& Rect, COLORREF cLeft, COLORREF cRight, bool a_bVertical)
{
	float fStep;
	if (a_bVertical)
		fStep = ((float)Rect.Height()) / 255.0f;	
	else
		fStep = ((float)Rect.Width()) / 255.0f; // width of color's band
	
	BYTE Rl = GetRValue(cLeft);
	BYTE Gl = GetGValue(cLeft);
	BYTE Bl = GetBValue(cLeft);

	int dR = (int)GetRValue(cRight) - Rl;
	int dG = (int)GetGValue(cRight) - Gl;
	int dB = (int)GetBValue(cRight) - Bl;

	for (int iOnBand = 0; iOnBand < 255; iOnBand++) 
	{
		// set current band
		RECT stepR;
		if (a_bVertical)
			SetRect(&stepR, Rect.left, Rect.top+(int)(iOnBand * fStep), Rect.right, Rect.top+(int)((iOnBand+1)* fStep));	
		else
			SetRect(&stepR, Rect.left+(int)(iOnBand * fStep), Rect.top, Rect.left+(int)((iOnBand+1)* fStep), Rect.bottom);	

		// set current color
		COLORREF color = RGB(
			Rl + ((dR * iOnBand) / 255),
			Gl + ((dG * iOnBand) / 255),
			Bl + ((dB * iOnBand) / 255));

		// fill current band
		FillRectBand(color, &stepR);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CFloodFill2::DrawRadialGradient(const CRect& rect, COLORREF clrFrom, COLORREF clrTo, bool bCenter)
{
	int iWidth = rect.Width(); 
	int iHeight = rect.Height(); 

	int iRadius = min(iWidth, iHeight);
	if (bCenter)
		iRadius /= 2;

	int cx = (bCenter ? rect.Width() / 2 : 0);
	int cy = (bCenter ? rect.Height() / 2 : 0);

	int r1 = GetRValue(clrFrom);
	int g1 = GetGValue(clrFrom);
	int b1 = GetBValue(clrFrom);
	int r2 = GetRValue(clrTo);
	int g2 = GetGValue(clrTo);
	int b2 = GetBValue(clrTo);

	int iRadius2 = iRadius * iRadius;
	double dr = (double)(r1 - r2) / iRadius2;
	double dg = (double)(g1 - g2) / iRadius2;
	double db = (double)(b1 - b2) / iRadius2;

	for (int x = 0; x <= rect.Width(); x++) 
	{
		int x2 = x - cx; x2 *= x2;
		for (int y = 0; y < rect.Height(); y++) 
		{
			int y2 = y - cy; y2 *= y2;
			int x2y2 = x2 + y2;

			COLORREF Color = 0;
			int iMultiplier = iRadius2 - x2y2;
			if (iMultiplier <= 0)
				Color = RGB(r2,g2,b2);
			else
			{
				int r = r2 + (int)(dr * iMultiplier);
				int g = g2 + (int)(dg * iMultiplier);
				int b = b2 + (int)(db * iMultiplier);
				Color = RGB(r, g, b);
			}

			LPBYTE pPixel = DibPtrXYExact(m_pDib, rect.left + x, rect.top + y);
			*pPixel++ = GetBValue(Color);
			*pPixel++ = GetGValue(Color);
			*pPixel++ = GetRValue(Color);
		}
	}
}
