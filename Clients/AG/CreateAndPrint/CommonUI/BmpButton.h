#pragma once

// bitmap button extended styles
#define BMPBTN_HOVER			0x00000001
#define BMPBTN_AUTO3D_SINGLE	0x00000002
#define BMPBTN_AUTO3D_DOUBLE	0x00000004
#define BMPBTN_AUTOSIZE			0x00000008
#define BMPBTN_AUTOFIRE			0x00000020

#define MAX_IMAGECOUNT 5

class CBmpButton : public CWindowImpl<CBmpButton, CButton>
{
public:
	enum
	{
		_nImageOff = 0,
		_nImageHoverOff,
		_nImageHoverOn,
		_nImageOn,
		_nImageDisabled,
	};

public:
	CBmpButton();
	virtual ~CBmpButton();
	void InitDialog(HWND hWndButtonCtrl, UINT nBitmapID, DWORD dwExtendedStyle = NULL, COLORREF clrTransparent = CLR_NONE, COLORREF clrBackground = CLR_NONE);
	DWORD GetBitmapButtonExtendedStyle() const;
	DWORD SetBitmapButtonExtendedStyle(DWORD dwExtendedStyle, DWORD dwMask = 0);
	HIMAGELIST GetImageList() const;
	HIMAGELIST SetImageList(HIMAGELIST hImageList);
	int GetToolTipTextLength() const;
	bool GetToolTipText(LPTSTR lpstrText, int nLength) const;
	bool SetToolTipText(LPCTSTR lpstrText);
	void SetImages(int nOff, int nOn = -1, int nHoverOff = -1, int nHoverOn = -1, int nDisabled = -1);
	bool SizeToImage();
	bool IsHoverMode() const;
	bool IsChecked();
	void Check(bool bChecked);
	void Redraw();
	void DoPaint(CDCHandle dc);

public:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCheck(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	bool StartTrackMouseLeave();

	BEGIN_MSG_MAP(CBmpButton)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(BM_SETCHECK, OnSetCheck)
		MESSAGE_HANDLER(WM_ENABLE, OnEnable)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

protected:
	DWORD m_dwExtendedStyle;
	CImageList m_ImageList;
	int m_nImage[MAX_IMAGECOUNT];
	CToolTipCtrl m_tip;
	LPTSTR m_lpstrToolTipText;
	bool m_bHover;
	bool m_bOn;
	bool m_bCapture;
	bool m_bTransparentDraw;
};
