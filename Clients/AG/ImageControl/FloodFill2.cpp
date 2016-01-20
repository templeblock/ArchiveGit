// DibData.cpp

#include "stdafx.h"
#include "FloodFill2.h"
#include "HSL.h"
#include "Utility.h"

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

	if (x < 0 || x >= DibWidth(m_pDib) || y < 0 || y >= DibHeight(m_pDib))
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

	if (y < 0 || y >= DibHeight(m_pDib))
		return false;

	if (x1 > x2)
		SWAP(x1, x2);

	if (x2 < 0 || x1 >= DibWidth(m_pDib))
		return false;

	x1 = bound(x1, 0, DibWidth(m_pDib));
	x2 = bound(x2, 0, DibWidth(m_pDib));

	int x = x1;
	LPBYTE pPixel = DibPtrXYExact(m_pDib, x1, y);
	LPBYTE pEnd = DibPtrXYExact(m_pDib, x2, y);

	while (pPixel <= pEnd)
	{
		if (m_bRedEye)
		{
			Color = GetPixel(x++, y); 
			RedEyeRemove(Color);
		}
		else
		if (m_pBufferDib)
			Color = m_pBufferDib->GetPixel(x++, y);

		*pPixel++ = GetBValue(Color);
		*pPixel++ = GetGValue(Color);
		*pPixel++ = GetRValue(Color);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CFloodFill2::RedEyeRemove(COLORREF& Color)
{
	// This function should not change the pixel anymore
	// All changes are made in DrawRedEyeCircle
	// Uncomment the line below to see the filled pixels
//j	Color = RGB(0,255,0);

#ifdef NOTUSED
	BYTE R = GetRValue(Color);
	BYTE G = GetGValue(Color);
	BYTE B = GetBValue(Color);

	R = dtoi(0.05*R + 0.6*G + 0.3*B);
//j	// Another approach
//j	R = max(G, B);
//j	G = min(dtoi(G * 1.20), 255);
//j	B = min(dtoi(B * 1.20), 255);
	Color = RGB(R, G, B);
#endif NOTUSED
}

/////////////////////////////////////////////////////////////////////////////
int CFloodFill2::ScanRight(int x, int y, int xmax, COLORREF Color, bool bMatch)
{
	if (!m_pDib || m_pDib->biBitCount != 24)
		return ++xmax;

	if (xmax < 0)
		return ++xmax;

	if (x < 0 || x >= DibWidth(m_pDib) || y < 0 || y >= DibHeight(m_pDib))
		return ++xmax;

	if (Color == CLR_NONE || DibWidth(m_pDib) <= xmax)
		return ++xmax;

	LPBYTE pPixel = DibPtrXYExact(m_pDib, x, y);
	for (; x <= xmax; ++x)
	{
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

	if (x < 0 || x >= DibWidth(m_pDib) || y < 0 || y >= DibHeight(m_pDib))
		return --xmin;

	if (Color == CLR_NONE || DibWidth(m_pDib) <= xmin)
		return --xmin;

	LPBYTE pPixel = DibPtrXYExact(m_pDib, x, y);
	for (; x >= xmin; --x)
	{
		BYTE B = *pPixel++;
		BYTE G = *pPixel++;
		BYTE R = *pPixel++;
		pPixel -= 6;
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
		if (nWhiteIndex >= 2)
			return true;
			
		// Red eyes tend to be red, but not nearly as Green or Blue
		// The first test ensures that Red meets the minimum value
		// The second test ensures that Red is at last twice Green
		#define MIN_RED_VAL 40 // out of 255; was 50
		if (R <= MIN_RED_VAL || R < 2*G)
			return false;

//j		// Other tests that have been tried and rejected
//j		if (!((4*R - 2*G - 2*B) / R))
//j			return false;

//j		int iSum = (R + G + B);
//j		float dR = (float)R / iSum;
//j		float dG = (float)G / iSum;
//j		float dB = (float)B / iSum;
//j		if (dR < 0.40 || dG > 0.31 || dB > 0.36)
//j			return false;

//j		HSL hsl;
//j		RGBtoHSL(R, G, B, &hsl);
//j		if (hsl.hue > 10 && hsl.hue < 200)
//j			return false;
//j		if (hsl.sat < 98 && hsl.lum < 98)
//j			return false;

		return true;
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
	int iDiameter = max(Rect.Height(), Rect.Width());
	if (iDiameter <= 2)
		return;


	iDiameter += 4; // Grow the diameter to catch any fringe pixels
	int iRadius = iDiameter / 2;
	CPoint ptCenter = Rect.CenterPoint();
	CRect rcCircle(ptCenter.x - iRadius, ptCenter.y - iRadius, ptCenter.x + iRadius, ptCenter.y + iRadius);

	// For each pixel inside the circle...
	for (int y = rcCircle.top; y <= rcCircle.bottom; y++)
	{
		if (y < 0 || y >= DibHeight(m_pDib))
			continue;

		int dy = y - ptCenter.y;
		for (int x = rcCircle.left; x <= rcCircle.right; x++)
		{
			if (x < 0 || x >= DibWidth(m_pDib))
				continue;

			int dx = x - ptCenter.x;
			int iDistance = lsqrt((dx * dx) + (dy * dy));
			if (iDistance >= iRadius)
				continue;

			LPBYTE pPixel = DibPtrXYExact(m_pDib, x, y);
			BYTE B = *pPixel++;
			BYTE G = *pPixel++;
			BYTE R = *pPixel++;
			pPixel -= 3;

			// Desaturate the target pixel
			// R' = 0.05*R + 0.6*G + 0.3*B 
			double fAlpha = (double)iDistance / iRadius;
			fAlpha *= fAlpha;
			fAlpha *= fAlpha;
			R = dtoi((fAlpha * R) + ((1.0-fAlpha) * (0.05*R + 0.6*G + 0.3*B)));

//j			// Another approach
//j			R = max(G, B);
//j			G = min(dtoi(G * 1.20), 255);
//j			B = min(dtoi(B * 1.20), 255);

			*pPixel++ = B;
			*pPixel++ = G;
			*pPixel++ = R;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CFloodFill2::FillRectBand(COLORREF Color, const CRect& Rect)
{
	CRect FillRect = Rect;
	DibIntersectRect(m_pDib, FillRect);
	for (int y = FillRect.top; y < FillRect.bottom; y++)
	{
		LPBYTE pPixel = DibPtrXYExact(m_pDib, FillRect.left, y);
		LPBYTE pEnd = DibPtrXYExact(m_pDib, FillRect.right, y);

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
	CRect FillRect = Rect;
	DibIntersectRect(m_pDib, FillRect);

	float fStep;
	if (a_bVertical)
		fStep = (float)FillRect.Height() / 255.0f;	
	else
		fStep = (float)FillRect.Width() / 255.0f; // width of color's band
	
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
			SetRect(&stepR, FillRect.left, FillRect.top+(int)(iOnBand * fStep), FillRect.right, FillRect.top+(int)((iOnBand+1)* fStep));	
		else
			SetRect(&stepR, FillRect.left+(int)(iOnBand * fStep), FillRect.top, FillRect.left+(int)((iOnBand+1)* fStep), FillRect.bottom);	

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
void CFloodFill2::DrawRadialGradient(const CRect& Rect, COLORREF clrFrom, COLORREF clrTo, bool bCenter)
{
	CRect FillRect = Rect;
	DibIntersectRect(m_pDib, FillRect);
	int iRadius = min(FillRect.Width(), FillRect.Height());
	if (bCenter)
		iRadius /= 2;

	int cx = (bCenter ? FillRect.Width() / 2 : 0);
	int cy = (bCenter ? FillRect.Height() / 2 : 0);

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

	for (int x = 0; x <= FillRect.Width(); x++) 
	{
		int x2 = x - cx; x2 *= x2;
		for (int y = 0; y < FillRect.Height(); y++) 
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

			LPBYTE pPixel = DibPtrXYExact(m_pDib, FillRect.left + x, FillRect.top + y);
			*pPixel++ = GetBValue(Color);
			*pPixel++ = GetGValue(Color);
			*pPixel++ = GetRValue(Color);
		}
	}
}
