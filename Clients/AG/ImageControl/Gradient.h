#pragma once
#include "Dib.h"
#include "ColorMap.h"

class CGradient
{
public:
	CGradient(int iBitCount, int iWidth, int iHeight, COLORREF clrFrom, COLORREF clrTo, int iSizePercentage, int iSoftness, bool bStretch);
	~CGradient();

	BITMAPINFOHEADER* CreateCurtain(bool bVertical, bool bForward);
	BITMAPINFOHEADER* CreateRectangle();
	BITMAPINFOHEADER* CreateCircle();
	BITMAPINFOHEADER* CreateDiamond();
	BITMAPINFOHEADER* CreateShape(LPCTSTR pShapeName);

protected:
	enum EGeometricShape
	{
		eRectangle,
		eCircle,
		eDiamond,
	};
	
	BITMAPINFOHEADER* CreateGeometricShape(EGeometricShape GeometricShape);
	RGBTRIPLE* CreateColorArray(int iMaxDistance, bool bForward);

protected:
	CDib m_pDIB;
	BYTE m_FromR;
	BYTE m_FromG;
	BYTE m_FromB;
	BYTE m_ToR;
	BYTE m_ToG;
	BYTE m_ToB;
	int m_dR;
	int m_dG;
	int m_dB;
	int m_iSizePercentage;
	int m_iSoftness;
	bool m_bStretch;
};
