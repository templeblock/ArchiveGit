#include "StdAfx.h"
#include "Gradient.h"
#include "Shape.h"
#include "Utility.h"
#include "AGDC.h"
#include "GdiClasses.h"
#include <math.h>

/////////////////////////////////////////////////////////////////////////////
CGradient::CGradient(int iBitCount, int iWidth, int iHeight, COLORREF clrFrom, COLORREF clrTo, int iSizePercentage, int iSoftness, bool bStretch)
{
	RGBQUAD Colors[256];
	for (int i=0; i<256; i++)
	{
		Colors[i].rgbRed   = i;
		Colors[i].rgbGreen = i;
		Colors[i].rgbBlue  = i;
		Colors[i].rgbReserved  = 0;
	}

	m_pDIB.Create(iBitCount, iWidth, iHeight, Colors);

	m_FromR = GetRValue(clrFrom);
	m_FromG = GetGValue(clrFrom);
	m_FromB = GetBValue(clrFrom);

	m_ToR = GetRValue(clrTo);
	m_ToG = GetGValue(clrTo);
	m_ToB = GetBValue(clrTo);

	m_dR = (int)m_ToR - m_FromR;
	m_dG = (int)m_ToG - m_FromG;
	m_dB = (int)m_ToB - m_FromB;

	m_iSizePercentage = iSizePercentage;
	m_iSoftness = 100 - iSoftness;
	m_bStretch = bStretch;
}
	
/////////////////////////////////////////////////////////////////////////////
CGradient::~CGradient()
{
}

/////////////////////////////////////////////////////////////////////////////
RGBTRIPLE* CGradient::CreateColorArray(int iMaxSize, bool bForward)
{
	int iSize = ((iMaxSize * m_iSizePercentage) + 50) / 100;
	if (iSize == iMaxSize) iSize--;
	int iMinSize = ((iSize * m_iSoftness) + 50) / 100;
	int iRange = iSize - iMinSize;
	if (!iRange) iRange = 1;
	int iRangeHalf = iRange / 2;

	RGBTRIPLE* pColors = new RGBTRIPLE[iMaxSize+1];
	for (int i = 0; i <= iMaxSize; i++)
	{
		int iDistance = (bForward ? i : iMaxSize - i);
		if (iDistance <= iMinSize)
		{
			pColors[i].rgbtRed   = m_FromR;
			pColors[i].rgbtGreen = m_FromG;
			pColors[i].rgbtBlue  = m_FromB;
		}
		else
		if (iDistance > iSize)
		{
			pColors[i].rgbtRed   = m_ToR;
			pColors[i].rgbtGreen = m_ToG;
			pColors[i].rgbtBlue  = m_ToB;
		}
		else
		{
			int iIndex = iDistance - iMinSize;
			pColors[i].rgbtRed   = m_FromR + ((m_dR * iIndex) + iRangeHalf) / iRange;
			pColors[i].rgbtGreen = m_FromG + ((m_dG * iIndex) + iRangeHalf) / iRange;
			pColors[i].rgbtBlue  = m_FromB + ((m_dB * iIndex) + iRangeHalf) / iRange;
		}
	}

	return pColors;
}

/////////////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CGradient::CreateCurtain(bool bVertical, bool bForward)
{
	if (!m_pDIB)
		return NULL;

	int iBitCount = m_pDIB.BitCount();
	int xCount = m_pDIB.Width();
	int yCount = m_pDIB.Height();
	int cx = xCount / 2;
	int cy = yCount / 2;

	int iMaxSize = (bVertical ? yCount : xCount) - 1;
	RGBTRIPLE* pColors = CreateColorArray(iMaxSize, bForward);

	for (int y = 0; y < yCount; y++)
	{
		// Get the pointer to the row of pixels
		BYTE* pPixel = m_pDIB.PtrXYExact(0, y);

		for (int x = 0; x < xCount; x++)
		{
			// Compute the pixel
			int iDistance = (bVertical ? y : x);

			// Store the pixel
			RGBTRIPLE rgb = pColors[iDistance];
			*pPixel++ = rgb.rgbtBlue;
			if (iBitCount >= 24)
			{
				*pPixel++ = rgb.rgbtGreen;
				*pPixel++ = rgb.rgbtRed;
			}
		}
	}

	delete pColors;
	return m_pDIB;
}

/////////////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CGradient::CreateRectangle()
{
	return CreateGeometricShape(eRectangle);
}

/////////////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CGradient::CreateCircle()
{
	return CreateGeometricShape(eCircle);
}

/////////////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CGradient::CreateDiamond()
{
	return CreateGeometricShape(eDiamond);
}

/////////////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CGradient::CreateGeometricShape(EGeometricShape GeometricShape)
{
	if (!m_pDIB)
		return NULL;

	int iBitCount = m_pDIB.BitCount();
	int xCount = m_pDIB.Width();
	int yCount = m_pDIB.Height();
	int cx = xCount / 2;
	int cy = yCount / 2;

	int iMaxSize = min(cx, cy);
	RGBTRIPLE* pColors = CreateColorArray(iMaxSize, true/*bForward*/);
	RGBTRIPLE rgbTo = {m_ToR, m_ToG, m_ToB};

	int nx = 0;
	int ny = 0;
	int hcx = cx / 2;
	int hcy = cy / 2;
	bool bSmallerX = (cx < cy);

	for (int y = 0; y < yCount; y++)
	{
		// Get the pointer to the row of pixels
		BYTE* pPixel = m_pDIB.PtrXYExact(0, y);

		for (int x = 0; x < xCount; x++)
		{
			nx = abs(x - cx);
			ny = abs(y - cy);
//j Debug	nx = (nx / 10) * 10;
//			ny = (ny / 10) * 10;

			// Square up the coordinates
			if (m_bStretch)
			{
				if (bSmallerX)
					ny = ((ny * cx) + hcy) / cy;
				else
					nx = ((nx * cy) + hcx) / cx;
			}
			else
			{
				if (bSmallerX)
					{ if (ny > cx) ny = cx; }
				else
					{ if (nx > cy) nx = cy; }
			}

			// Compute the distance
			int iDistance = 0;

			switch (GeometricShape)
			{
				case eRectangle:
					iDistance = max(nx, ny);
					break;
				case eCircle:
					iDistance = lsqrt((nx * nx) + (ny * ny));
					break;
				case eDiamond:
					iDistance = (nx + ny);
					break;
			}

			// Store the pixel
			RGBTRIPLE rgb = (iDistance <= iMaxSize ? pColors[iDistance] : rgbTo);
			*pPixel++ = rgb.rgbtBlue;
			if (iBitCount >= 24)
			{
				*pPixel++ = rgb.rgbtGreen;
				*pPixel++ = rgb.rgbtRed;
			}
		}
	}

	delete pColors;
	return m_pDIB;
}

/////////////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CGradient::CreateShape(LPCTSTR pShapeName)
{
	if (!m_pDIB)
		return NULL;

	CAGDIBSectionDC dc(m_pDIB, DIB_RGB_COLORS);
	HDC hDC = dc.GetHDC();
	if (!hDC)
		return NULL;

	dc.Clear(RGB(255,255,255));

	int xCount = m_pDIB.Width();
	int yCount = m_pDIB.Height();
	int xMaxSize = (m_bStretch ? xCount : min(xCount, yCount));
	int yMaxSize = (m_bStretch ? yCount : min(xCount, yCount));
	int xSize = (xMaxSize * m_iSizePercentage) / 100;
	int ySize = (yMaxSize * m_iSizePercentage) / 100;
	int xRange = xSize - ((m_iSoftness * xSize) + 50) / 100;
	int yRange = ySize - ((m_iSoftness * ySize) + 50) / 100;

	CShape Shape;
	COLORREF LastColor = CLR_NONE;
	for (int i=0; i<256; i++)
	{
		int iIndex = 255 - i;
		BYTE R = m_FromR + ((m_dR * iIndex) + 127) / 255;
		BYTE G = m_FromG + ((m_dG * iIndex) + 127) / 255;
		BYTE B = m_FromB + ((m_dB * iIndex) + 127) / 255;
		COLORREF Color = RGB(R, G, B);
		if (LastColor == Color)
			continue;
		LastColor = Color;
		int xShapeSize = xSize - ((xRange * i) / 255);
		int yShapeSize = ySize - ((yRange * i) / 255);
		int x = (xCount - xShapeSize) / 2;
		int y = (yCount - yShapeSize) / 2;
		CRect Rect(x, y, x + xShapeSize, y + yShapeSize);
		bool bOK = Shape.Generate(hDC, pShapeName, Rect, m_bStretch);
		dc.DrawPath(CLR_NONE/*LineColor*/, 0/*LineWidth*/, Color);
	}

	BITMAPINFOHEADER* pDibDouble = dc.ExtractDib();
	if (!pDibDouble)
		return NULL;

	m_pDIB.Attach(pDibDouble);
	return m_pDIB;
}
