// ColorPicker.h : Declaration of the CColorPicker
#pragma once
#include "resource.h"

class CColorBox : public CWindowImpl<CColorBox, CStatic>
{
public:
	CColorBox()
	{
		m_r = 0;
		m_g = 0;
		m_b = 0;
	}

	virtual ~CColorBox()
	{
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam)
		{
			DoPaint((HDC)wParam, NULL);
		}
		else
		{
			PAINTSTRUCT PaintStruct;
			HDC hPaintDC = ::BeginPaint(m_hWnd, &PaintStruct);
			if (hPaintDC)
				DoPaint(hPaintDC, &PaintStruct.rcPaint);
			::EndPaint(m_hWnd, &PaintStruct);
		}

		return S_OK;
	}

	void SetColor(short r, short g, short b)
	{
		bool bChanged = (r != m_r || g != m_g || b != m_b);

		m_r = r;
		m_g = g;
		m_b = b;

		if (bChanged && ::IsWindow(m_hWnd))
			InvalidateRect(NULL);
	}

private:
	void DoPaint(HDC hDC, LPRECT pRect)
	{
		RECT rect;
		if (pRect)
			rect = *pRect;
		else
			GetClientRect(&rect);
		HBRUSH hBackgroundBrush = CreateSolidBrush(RGB(m_r, m_g, m_b));
		::FillRect(hDC, &rect, hBackgroundBrush);
	}

private:
	BEGIN_MSG_MAP(CColorBox)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

private:
	short m_r;
	short m_g;
	short m_b;
};

class CColorPicker : public CDialogImpl<CColorPicker>
{
public:
	CColorPicker()
	{
		m_r = 0;
		m_g = 0;
		m_b = 0;
		m_strColor = "";
		m_iFormat = 0;
		m_bCapture = false;
		m_bComplete = false;
	}
	
	~CColorPicker()
	{
		if (m_bCapture)
			ReleaseCapture();
	}

	enum { IDD = IDD_COLORPICKER };

	BEGIN_MSG_MAP(CColorPicker)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_NCACTIVATE, OnNCActivate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HDC hDC = CreateDC("DISPLAY", 0, 0, 0);
		int dx = ::GetDeviceCaps(hDC, HORZRES);
		int dy = ::GetDeviceCaps(hDC, VERTRES);
		::DeleteDC(hDC);

		SetWindowPos(HWND_TOPMOST, 0, 0, dx, dy, SWP_SHOWWINDOW);
		m_ColorBox.SubclassWindow(GetDlgItem(IDC_COLORBOX));

		SetCapture();
		::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDC_PROBE)));
		m_bCapture = true;

		return 1;  // Let the system set the focus
	}

	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDC_PROBE)));
		return S_OK;
	}
	
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		MapWindowPoints(NULL, &pt, 1);
		GetScreenColor(pt);
		return S_OK;
	}
	
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		MapWindowPoints(NULL, &pt, 1);
		GetScreenColor(pt);
		Complete(IDOK);
		return S_OK;
	}
	
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Complete(IDCANCEL);
		return S_OK;
	}

	LRESULT OnNCActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL fActive = (BOOL)wParam;
		if (!fActive)
			Complete(IDCANCEL);

		return 0; // prevent the title bar or icon from being deactivated
	}

	CString& GetColor()
	{
		return m_strColor;
	}
	
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return S_OK; // do nothing
	}

	void SetFormat(int iFormat)
	{
		m_iFormat = iFormat;
	}

private:
	void GetScreenColor(POINT& pt)
	{
		HDC hDC = CreateDC("DISPLAY", 0, 0, 0);
		COLORREF c = ::GetPixel(hDC, pt.x, pt.y);

		short r = GetRValue(c);
		short g = GetGValue(c);
		short b = GetBValue(c);
		bool bChanged = (r != m_r || g != m_g || b != m_b);

		m_r = r;
		m_g = g;
		m_b = b;

		if (bChanged)
		{
			if (m_iFormat)
			{
				m_strColor.Format("%02x%02x%02x", m_b, m_g, m_r);
				m_strColor.MakeUpper();
				m_strColor = "0x" + m_strColor;
			}
			else
			{
				m_strColor.Format("#%02x%02x%02x", m_r, m_g, m_b);
				m_strColor.MakeUpper();
			}
			
			HWND m_ctrlColor = GetDlgItem(IDC_COLOREDIT);
			if (m_ctrlColor)
			{
				CWindow(m_ctrlColor).SetWindowText(m_strColor);
				InvalidateRect(NULL);
			}

			m_ColorBox.SetColor(m_r, m_g, m_b);
		}
		
		::DeleteDC(hDC);
	}
	
	void Complete(int retc)
	{
		if (m_bComplete)
			return;
		
		if (!::IsWindow(m_hWnd))
			return;

		m_bComplete = true; // Must be before the EndDialog
		EndDialog(retc);
	}
	
private:
	CColorBox m_ColorBox;
	CString m_strColor;
	short m_r;
	short m_g;
	short m_b;
	int m_iFormat;
	bool m_bCapture;
	bool m_bComplete;
};

class CColorPicker2 : public CDialogImpl<CColorPicker2>
{
public:
	CColorPicker2()
	{
		m_nTimer = 1;
		m_r = 0;
		m_g = 0;
		m_b = 0;
		m_strColor = "";
		m_iFormat = 0;
		m_bCapture = false;
		m_bComplete = false;
	}

	~CColorPicker2()
	{
		if (m_bCapture)
			ReleaseCapture();
	}

	enum { IDD = IDD_COLORPICKER2 };

	BEGIN_MSG_MAP(CColorPicker2)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_NCACTIVATE, OnNCActivate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetTimer(m_nTimer, 100, NULL);
		SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);

		return 1;  // Let the system set the focus
	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDC_PROBE)));

		POINT pt;
		::GetCursorPos(&pt);
		GetScreenColor(pt);
		return S_OK;
	}
	
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		MapWindowPoints(NULL, &pt, 1);
		GetScreenColor(pt);
		Complete(IDOK);
		return S_OK;
	}
	
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Complete(IDCANCEL);
		return S_OK;
	}

	LRESULT OnNCActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL fActive = (BOOL)wParam;
		if (!fActive)
			Complete(IDCANCEL);

		return 0; // prevent the title bar or icon from being deactivated
	}

	CString& GetColor()
	{
		return m_strColor;
	}
	
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (!m_bCapture)
		{
			SetCapture();
			::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDC_PROBE)));
			m_bCapture = true;
		}

		if (wParam)
		{
			DoPaint((HDC)wParam, NULL);
		}
		else
		{
			PAINTSTRUCT PaintStruct;
			HDC hPaintDC = ::BeginPaint(m_hWnd, &PaintStruct);
			if (hPaintDC)
				DoPaint(hPaintDC, &PaintStruct.rcPaint);
			::EndPaint(m_hWnd, &PaintStruct);
		}

		return S_OK;
	}

	void SetFormat(int iFormat)
	{
		m_iFormat = iFormat;
	}

private:
	void DoPaint(HDC hDC, LPRECT pRect)
	{
		RECT rect;
		if (pRect)
			rect = *pRect;
		else
			GetClientRect(&rect);
		HBRUSH hBackgroundBrush = CreateSolidBrush(RGB(m_r, m_g, m_b));
		::FillRect(hDC, &rect, hBackgroundBrush);
	}

	void GetScreenColor(POINT& pt)
	{
		HDC hDC = CreateDC("DISPLAY", 0, 0, 0);
		COLORREF c = ::GetPixel(hDC, pt.x, pt.y);

		short r = GetRValue(c);
		short g = GetGValue(c);
		short b = GetBValue(c);
		bool bChanged = (r != m_r || g != m_g || b != m_b);

		m_r = r;
		m_g = g;
		m_b = b;

		if (bChanged)
		{
			if (m_iFormat)
			{
				m_strColor.Format("%02x%02x%02x", m_b, m_g, m_r);
				m_strColor.MakeUpper();
				m_strColor = "0x" + m_strColor;
			}
			else
			{
				m_strColor.Format("#%02x%02x%02x", m_r, m_g, m_b);
				m_strColor.MakeUpper();
			}
			
			HWND m_ctrlColor = GetDlgItem(IDC_COLOREDIT);
			if (m_ctrlColor)
			{
				CWindow(m_ctrlColor).SetWindowText(m_strColor);
				InvalidateRect(NULL);
			}
		}
		
		::DeleteDC(hDC);
	}
	
	void Complete(int retc)
	{
		if (m_bComplete)
			return;
		
		if (!::IsWindow(m_hWnd))
			return;

		m_bComplete = true; // Must be before the EndDialog
		KillTimer(m_nTimer);
		EndDialog(retc);
	}
	
private:
	UINT m_nTimer;
	CString m_strColor;
	short m_r;
	short m_g;
	short m_b;
	int m_iFormat;
	bool m_bCapture;
	bool m_bComplete;
};

