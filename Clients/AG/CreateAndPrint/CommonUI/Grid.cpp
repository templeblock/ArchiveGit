#include "stdafx.h"
#include "Grid.h"

/////////////////////////////////////////////////////////////////////////////
CGrid::CGrid()
{
	m_Size.cx = INCHES(0.125);
	m_Size.cy = INCHES(0.125);
	m_bOn = false;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::Draw(HDC hDC, const CAGMatrix& Matrix, const SIZE& PageSize)
{
	if (!m_bOn)
		return;

	for (int x = m_Size.cx; x < PageSize.cx; x += m_Size.cx)
	{
		for (int y = m_Size.cy; y < PageSize.cy; y += m_Size.cy)
		{
			POINT pt = {x, y};
			Matrix.Transform(pt);
			int OldROP2 = ::SetROP2(hDC, R2_NOT);
			::MoveToEx(hDC, pt.x, pt.y, NULL);
			::LineTo(hDC, pt.x+1, pt.y);
			::SetROP2(hDC, OldROP2);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::Snap(POINT& pt)
{
	if (!m_bOn)
		return;

	pt.x += m_Size.cx/2; 
	pt.x -= (pt.x % m_Size.cx);
	pt.y += m_Size.cy/2;
	pt.y -= (pt.y % m_Size.cy);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::Snap(RECT& rect)
{
	if (!m_bOn)
		return;

	rect.left	 += m_Size.cx/2;
	rect.left	 -= (rect.left % m_Size.cx);
	rect.top	 += m_Size.cy/2;
	rect.top	 -= (rect.top % m_Size.cy);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SnapSize(RECT& rect)
{
	if (!m_bOn)
		return;

	rect.left	 += m_Size.cx/2;
	rect.left	 -= (rect.left % m_Size.cx);
	rect.top	 += m_Size.cy/2;
	rect.top	 -= (rect.top % m_Size.cy);
	rect.right	 += m_Size.cx/2;
	rect.right	 -= (rect.right % m_Size.cx);
	rect.bottom	 += m_Size.cy/2;
	rect.bottom	 -= (rect.bottom % m_Size.cy);
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::SetSize(const SIZE& size)
{
	m_Size = size;
	if (m_Size.cx <= 0) m_Size.cx = 1;
	if (m_Size.cy <= 0) m_Size.cy = 1;
}

/////////////////////////////////////////////////////////////////////////////
SIZE CGrid::GetSize()
{
	return m_Size;
}

/////////////////////////////////////////////////////////////////////////////
bool CGrid::IsOn()
{
	return m_bOn;
}

/////////////////////////////////////////////////////////////////////////////
void CGrid::On(bool bOn)
{
	m_bOn = bOn;
}
