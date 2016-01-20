#pragma once

#include "Dib.h"

class CFloodFill2
{
public:
	CFloodFill2();
	~CFloodFill2();

	COLORREF GetPixel(int x, int y) const;
	void SetTolerance(int nTolerance);
	void SetBufferDib(CFloodFill2* pBufferDib){m_pBufferDib = pBufferDib;}

	// DIB creation/initialization
	bool InitDIB(BITMAPINFOHEADER* pbmpSrc, bool bCopy);

	bool IsColorToBeFilled(COLORREF FillColor, COLORREF PixelColor);
	void SetRedEye(bool bRedEye) { m_bRedEye = bRedEye; };

	LONG  GetWidth() const
		{ return DibWidth(m_pDib); }
	LONG  GetHeight() const
		{ return DibHeight(m_pDib); }

	// Line scanning functions
	int ScanRight(int x, int y, int xmax, COLORREF Color, bool bMatch);
	int ScanLeft(int x, int y, int xmin, COLORREF Color, bool bMatch);

	void DrawGradRect(const CRect& Rect, COLORREF cLeft, COLORREF cRight, bool bVertical);
	void DrawRedEyeCircle(const CRect& pRect);
	void DrawRadialGradient(const CRect& Rect, COLORREF clrFrom, COLORREF clrTo, bool bCenter);
	bool DrawLineH(int x1, int x2, int y, COLORREF Color, const CRect& Rect);

protected:
	bool FillRectBand(COLORREF Color, const CRect& Rect);
	COLORREF GetPixelRedEye(int x, int y);
	void RedEyeRemove(COLORREF& RedEyePixel);

protected:
	BITMAPINFOHEADER* m_pDib;
	double m_fTolerance;
	CFloodFill2* m_pBufferDib;
	bool m_bRedEye;
	bool m_bCopy;
};
