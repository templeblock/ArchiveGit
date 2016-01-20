#pragma once

class CMatrix;

class CGrid
{
public:
	CGrid();
	void Draw(HDC hDC, const CMatrix& Matrix, const CSize& PageSize);
	void Snap(CPoint& pt);
	void Snap(CRect& rect);
	void SnapSize(CRect& rect);
	void SetSize(const CSize& size);
	CSize GetSize();
	bool IsOn();
	void On(bool bOn);

private:
	CSize m_Size;
	bool m_bOn;
};
