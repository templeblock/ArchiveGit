#pragma once
#pragma warning (disable : 4700) // local variable 'Brush' used without having been initialized

enum COLORTYPE { eStock, eSystem };

class CAGBrush
{
public:
	CAGBrush()
	{
		m_hBrush = NULL;
		m_bDeleteBrush = false;
	}
	CAGBrush(BYTE r, BYTE g, BYTE b)
	{
		m_hBrush = NULL;
		m_bDeleteBrush = false;
		Create(r, g, b);
	}
	CAGBrush(COLORREF Color)
	{
		m_hBrush = NULL;
		m_bDeleteBrush = false;
		Create(Color);
	}
	CAGBrush(int iColorIndex, COLORTYPE eType)
	{
		m_hBrush = NULL;
		m_bDeleteBrush = false;
		Create(iColorIndex, eType);
	}
	~CAGBrush()
	{
		Destroy();
	}
	HBRUSH Create(BYTE r, BYTE g, BYTE b)
	{
		Destroy();
		m_bDeleteBrush = true;
		m_hBrush = ::CreateSolidBrush(RGB(r,g,b));
		return m_hBrush;
	}
	HBRUSH Create(COLORREF Color)
	{
		Destroy();
		m_bDeleteBrush = true;
		m_hBrush = ::CreateSolidBrush(Color);
		return m_hBrush;
	}
	HBRUSH Create(int iColorIndex, COLORTYPE eType)
	{
		Destroy();
		m_bDeleteBrush = false;
		if (eType == eStock)
			m_hBrush = (HBRUSH)::GetStockObject(iColorIndex);
		else
		if (eType == eSystem)
			m_hBrush = ::GetSysColorBrush(iColorIndex);
		else
			m_hBrush = NULL;
		return m_hBrush;
	}
	void Destroy()
	{
		if (!m_hBrush)
			return;

		if (m_bDeleteBrush)
			::DeleteObject(m_hBrush);
		m_hBrush = NULL;
	}
	HBRUSH GetHBrush() const
	{
		return m_hBrush;
	}
	operator HBRUSH() const
	{
		return m_hBrush;
	}
private:
	HBRUSH m_hBrush;
	bool m_bDeleteBrush;
};
