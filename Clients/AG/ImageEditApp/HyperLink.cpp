#include "stdafx.h"
#include "HyperLink.h"

// CHyperLink

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CHyperLink, CButton)
BEGIN_MESSAGE_MAP(CHyperLink, CButton)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CHyperLink::CHyperLink()
{
	Initialize();
}

/////////////////////////////////////////////////////////////////////////////
CHyperLink::~CHyperLink()
{
	if (m_lpLinkText)
		delete [] m_lpLinkText;
	if (m_lpLinkUrl)
		delete [] m_lpLinkUrl;

	m_hReg  = NULL;
	m_hHand = NULL;

	//m_hFont.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
void CHyperLink::Initialize() 
{
	m_lpLinkText = NULL;
	m_lpLinkUrl = NULL;

	m_hFont.CreateFont(14,0,0,0,FW_NORMAL,0,0,0,
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,DEFAULT_PITCH | FF_DONTCARE, _T("Arial"));

	m_bUnderlineAlways = false;
	m_bIsLink          = true;
	m_bLockInPosition  = false;

	m_crLinkUp     = RGB(0,0,255);
	m_crLinkHover  = RGB(255,0,0);
	m_crLinkDown   = RGB(200,0,225);
	m_crBackGround = ::GetSysColor(COLOR_BTNFACE);

	m_hReg  = NULL;
	m_hHand = ::LoadCursor(NULL, MAKEINTRESOURCE(32649));

	m_hIconUp    = NULL;
	m_hIconHover = NULL;
	m_hIconDown  = NULL;
	m_uShowIcon  = SI_ICONUP_ON | SI_ICONUP_LEFT |        //Default: icons on the left Up/Hover/Down
				   SI_ICONHOVER_ON | SI_ICONHOVER_LEFT | 
				   SI_ICONDOWN_ON | SI_ICONDOWN_LEFT;

	//Status and Mouse control variables
	m_bDisabled  = false;
	m_bMouseIn   = false;
	m_bMouseDown = false;
	m_bChangePosAndSize = true;
}

/////////////////////////////////////////////////////////////////////////////
int CHyperLink::OpenUrl()
{
	if (!m_lpLinkUrl || !m_bIsLink)
		return -1;

	HINSTANCE result = ShellExecute(NULL, _T("open"), m_lpLinkUrl, NULL,NULL, SW_SHOWDEFAULT);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
TCHAR* CHyperLink::GetLinkText()
{
	if (!m_lpLinkText)
		return NULL;

	int iLength = _tcslen(m_lpLinkText) + 1;
	TCHAR* lpRet = new TCHAR[iLength];
	memcpy(lpRet, m_lpLinkText, iLength);
	return lpRet;
}

/////////////////////////////////////////////////////////////////////////////
void CHyperLink::SetLinkText(TCHAR* lpText)
{
	m_bChangePosAndSize = true;

	if (m_lpLinkText)
		delete [] m_lpLinkText;

	if (lpText)
	{
		int iLength = _tcslen(lpText) + 1;
		m_lpLinkText = new TCHAR[iLength];
		memcpy(m_lpLinkText, lpText, iLength);
	}
	else
		m_lpLinkText = NULL;
}

/////////////////////////////////////////////////////////////////////////////
TCHAR* CHyperLink::GetLinkUrl()
{
	if (!m_lpLinkUrl)
		return NULL;

	int iLength = _tcslen(m_lpLinkUrl) + 1;
	TCHAR* lpRet = new TCHAR[iLength];
	memcpy(lpRet, m_lpLinkUrl, iLength);
	return lpRet;
}

/////////////////////////////////////////////////////////////////////////////
void CHyperLink::SetLinkUrl(TCHAR* lpUrl)
{
	if (m_lpLinkUrl)
		delete [] m_lpLinkUrl;

	if (lpUrl)
	{
		int iLength = _tcslen(lpUrl) + 1;
		m_lpLinkUrl = new TCHAR[iLength];
		memcpy(m_lpLinkUrl, lpUrl, iLength);
	}
	else
		m_lpLinkUrl = NULL;

	m_ToolTip.UpdateTipText(m_lpLinkUrl,this);
}

/////////////////////////////////////////////////////////////////////////////
void CHyperLink::SetColors(COLORREF crLinkUp, COLORREF crLinkHover, COLORREF crLinkDown, COLORREF crBackGround /* = ::GetSysColor(COLOR_BTNFACE) */)
{
	m_crLinkUp     = crLinkUp;
	m_crLinkHover  = crLinkHover;
	m_crLinkDown   = crLinkDown;
	m_crBackGround = crBackGround;
}

/////////////////////////////////////////////////////////////////////////////
void CHyperLink::SetIcons(HICON hIconUp, HICON hIconHover, HICON hIconDown, UINT uShowIcons)
{
	if (!m_hIconUp || hIconUp)
		m_hIconUp    = hIconUp;
	if (!m_hIconHover || hIconHover)
		m_hIconHover = hIconHover;
	if (!m_hIconDown || hIconDown)
		m_hIconDown  = hIconDown;

	m_uShowIcon  = uShowIcons;
}

/////////////////////////////////////////////////////////////////////////////
bool CHyperLink::SetFont(TCHAR* lpFaceName /* = "Arial" */, int nSize /* = 14 */, int nWeight /* = FW_NORMAL */, bool bItalic /* = false */)
{
	m_bChangePosAndSize = true;

	m_hFont.DeleteObject();

	return !!m_hFont.CreateFont(nSize,0,0,0,nWeight,(BYTE)bItalic,0,0,
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,DEFAULT_PITCH | FF_DONTCARE,lpFaceName);
}

/////////////////////////////////////////////////////////////////////////////
void CHyperLink::OnPaint()
{
	CPaintDC dc(this); // device context for painting
    dc.SaveDC();
	dc.SetMapMode(MM_TEXT);
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(m_hFont);

	CRect rc;
    GetClientRect(&rc);
	CBrush cb;
	cb.CreateSolidBrush(m_crBackGround);
	dc.FillRect(&rc,&cb);

	CPen p;

	bool bWndTxt = false;
	if (!m_lpLinkText)
	{
		bWndTxt = true;
		int nlen = GetWindowTextLength();
		m_lpLinkText = new TCHAR[nlen+1];
		m_lpLinkText[nlen] = (TCHAR)0;
		GetWindowText(m_lpLinkText,nlen+1);
	}

	GetWindowRect(rc);
	GetParent()->ScreenToClient(&rc);

	CSize sz = dc.GetTextExtent(m_lpLinkText, _tcslen(m_lpLinkText));

	int x = rc.left;
	int y = sz.cy + 2;

	if (m_bChangePosAndSize && (m_uShowIcon & SI_ICONUP_ON || m_uShowIcon & SI_ICONHOVER_ON || m_uShowIcon & SI_ICONDOWN_ON))
	{
		m_bChangePosAndSize = false;

		if (y < 18)
			y = 18;
		y = rc.Height() - y;
		rc.bottom = rc.bottom - (y);

		x = x + 1 + sz.cx + 1;

		if (m_uShowIcon & SI_ICONUP_LEFT || m_uShowIcon & SI_ICONHOVER_LEFT || m_uShowIcon & SI_ICONDOWN_LEFT || m_bLockInPosition)
			x = x + 16 + 3;
	
		if (m_uShowIcon & SI_ICONUP_RIGHT || m_uShowIcon & SI_ICONHOVER_RIGHT || m_uShowIcon & SI_ICONDOWN_RIGHT)
			x = x + 2 + 16 + 1;

		rc.right = x;

		MoveWindow(rc.left,rc.top,rc.Width(),rc.Height(),false);
	}

	GetClientRect(rc);
	dc.FillRect(&rc,&cb);

	y = rc.Height() - 16;
	y = (y - (y % 2)) / 2 + 1;

	if (m_bLockInPosition)
		x = 20;
	else
		x = 1;

	if (m_bDisabled)
		dc.SetTextColor(::GetSysColor(COLOR_BTNSHADOW));
	else if (!m_bDisabled && m_bMouseIn && !m_bMouseDown)
	{
		if ((m_uShowIcon & SI_ICONHOVER_ON && m_uShowIcon & SI_ICONHOVER_LEFT) && m_hIconHover)
		{
			x = 20;
			DrawIconEx(dc.m_hDC,1,y,m_hIconHover,16,16,0,NULL,DI_IMAGE | DI_MASK);
		}
		if ((m_uShowIcon & SI_ICONHOVER_ON && m_uShowIcon & SI_ICONHOVER_RIGHT) && m_hIconHover)
			DrawIconEx(dc.m_hDC,x + sz.cx + 3,y,m_hIconHover,16,16,0,NULL,DI_IMAGE | DI_MASK);
		p.CreatePen(PS_SOLID,1,m_crLinkHover);
		dc.SetTextColor(m_crLinkHover);
	}
	else if (!m_bDisabled && m_bMouseIn && m_bMouseDown)
	{
		p.CreatePen(PS_SOLID,1,m_crLinkDown);
		if ((m_uShowIcon & SI_ICONDOWN_ON && m_uShowIcon & SI_ICONDOWN_LEFT) && m_hIconDown)
		{
			x = 20;
			DrawIconEx(dc.m_hDC,1,y,m_hIconDown,16,16,0,NULL,DI_IMAGE | DI_MASK);
		}
		if ((m_uShowIcon & SI_ICONDOWN_ON && m_uShowIcon & SI_ICONDOWN_RIGHT) && m_hIconDown)
			DrawIconEx(dc.m_hDC,x + sz.cx + 3,y,m_hIconDown,16,16,0,NULL,DI_IMAGE | DI_MASK);
		dc.SetTextColor(m_crLinkDown);
	}
	else if (!m_bDisabled && !m_bMouseIn)
	{
		if (m_bUnderlineAlways)
			p.CreatePen(PS_SOLID,1,m_crLinkUp);
		else
			p.CreatePen(PS_SOLID,1,m_crBackGround);
		if ((m_uShowIcon & SI_ICONUP_ON && m_uShowIcon & SI_ICONUP_LEFT) && m_hIconUp)
		{
			x = 20;
			DrawIconEx(dc.m_hDC,1,y,m_hIconUp,16,16,0,NULL,DI_IMAGE | DI_MASK);
		}
		if ((m_uShowIcon & SI_ICONUP_ON && m_uShowIcon & SI_ICONUP_RIGHT) && m_hIconUp)
			DrawIconEx(dc.m_hDC,x + sz.cx + 3,y,m_hIconUp,16,16,0,NULL,DI_IMAGE | DI_MASK);
		dc.SetTextColor(m_crLinkUp);
	}

	y = rc.Height() - sz.cy;
	y = (y - (y % 2)) / 2 + 1;

	dc.TextOut(x,y,m_lpLinkText, _tcslen(m_lpLinkText));

	dc.MoveTo(x,y+sz.cy);
	dc.SelectObject(&p);
	dc.LineTo(x+sz.cx,y+sz.cy);

	if (bWndTxt)
	{
		delete [] m_lpLinkText;
		m_lpLinkText = NULL;
	}

	dc.RestoreDC(-1);
}

/////////////////////////////////////////////////////////////////////////////
void CHyperLink::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bDisabled)
	{
		ShowCursor(false);
		m_hReg = ::SetCursor(m_hHand);
		ShowCursor(true);
	}

	if (m_bMouseIn)
		return;

	TRACKMOUSEEVENT EventTrack;
	EventTrack.cbSize = sizeof(TRACKMOUSEEVENT);
	EventTrack.dwFlags = TME_LEAVE;
	EventTrack.dwHoverTime = 0;
	EventTrack.hwndTrack = m_hWnd;
	TrackMouseEvent(&EventTrack);

	m_bMouseIn = true;

	m_bMouseDown = (nFlags & MK_LBUTTON);

	Invalidate();

	m_ToolTip.Activate(true);

}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHyperLink::OnMouseLeave(WPARAM, LPARAM)
{
	if (!m_bDisabled)
	{
		ShowCursor(false);
		::SetCursor(m_hReg);
		ShowCursor(true);
	}

	TRACKMOUSEEVENT EventTrack;
	EventTrack.cbSize = sizeof(TRACKMOUSEEVENT);
	EventTrack.dwFlags = TME_CANCEL | TME_LEAVE;
	EventTrack.dwHoverTime = 0;
	EventTrack.hwndTrack = m_hWnd;
	TrackMouseEvent(&EventTrack);

	m_bMouseIn   = false;
	m_bMouseDown = false;

	Invalidate();

	m_ToolTip.Activate(false);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CHyperLink::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bDisabled)
	{
		ShowCursor(false);
		m_hReg = ::SetCursor(m_hHand);
		ShowCursor(true);
	}

	m_bMouseDown = false;

	Invalidate();

	if (!m_bDisabled)
		OpenUrl();

	if (!m_bDisabled && (!m_bIsLink || (GetStyle() & SS_NOTIFY)))
	{
		CWnd* pParent = GetParent();

		if (pParent)
		{
			WORD wID = GetDlgCtrlID();
			if (wID)
				pParent->SendMessage(WM_COMMAND, MAKEWPARAM(wID,STN_CLICKED),(LPARAM)m_hWnd);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CHyperLink::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bDisabled)
	{
		ShowCursor(false);
		m_hReg = ::SetCursor(m_hHand);
		ShowCursor(true);
	}

	m_bMouseDown = true;

	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
void CHyperLink::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!m_bDisabled && (!m_bIsLink || GetStyle() & SS_NOTIFY))
		CButton::OnLButtonDblClk(nFlags,point);

	if (!m_bDisabled)
	{
		ShowCursor(false);
		m_hReg = ::SetCursor(m_hHand);
		ShowCursor(true);
	}

	m_bMouseDown = true;

	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
void CHyperLink::PreSubclassWindow()
{
	CButton::PreSubclassWindow();

	ModifyStyle(SS_BITMAP | SS_ICON, SS_OWNERDRAW | SS_NOTIFY);

	if (GetStyle() & WS_DISABLED)
	{
		m_bDisabled = true;
		ModifyStyle(WS_DISABLED, 0);
	}

	m_ToolTip.Create(this);
	m_ToolTip.AddTool(this);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHyperLink::PreTranslateMessage(MSG* pMsg)
{
	if (m_ToolTip.m_hWnd)
	{
		m_ToolTip.RelayEvent(pMsg);
	}

	return CButton::PreTranslateMessage(pMsg);
}
