#pragma once

class CShape  
{
public:
	CShape();
	virtual ~CShape();

	bool Generate(HDC hDC, LPCTSTR strShapeName, CRect& DestRect, bool bStretch);
	double GetMinDistance(const POINT& pt, bool& bInside);

private:
	bool MakePath(HDC hDC, POINT* pCommands, int nCommands, POINT* pPoints, int nPoints);

private:
	CDC m_hDC;
//j	CRgn m_hRegion;
//j	int m_nFlatPoints;
//j	POINT* m_pFlatPoints;
//j	BYTE* m_pFlatPointTypes;
};
