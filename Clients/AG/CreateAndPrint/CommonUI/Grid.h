#pragma once
#include "AGMatrix.h"

class CGrid
{
public:
	CGrid();
	void Draw(HDC hDC, const CAGMatrix& Matrix, const SIZE& PageSize);
	void Snap(POINT& pt);
	void Snap(RECT& rect);
	void SnapSize(RECT& rect);
	void SetSize(const SIZE& size);
	SIZE GetSize();
	bool IsOn();
	void On(bool bOn);

private:
	SIZE m_Size;
	bool m_bOn;
};
