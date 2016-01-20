#include "stdafx.h"
#include "ProgressDlg.h"
#include "Utility.h"
#include "ImageControl.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CProgressDlg::CProgressDlg(CImageControl* pMainWnd)
{
	m_pImageControl = pMainWnd;

	COLORREF ColorText = RGB(0,0,0); //j ::GetSysColor(COLOR_WINDOWTEXT);
	COLORREF ColorBgDialog = BACKGOUND_COLOR;
	COLORREF ColorBgControl = BACKGOUND_COLOR;

	m_ColorScheme.m_ColorTextDialog		= ColorText;
	m_ColorScheme.m_ColorTextStatic		= ColorText;
	m_ColorScheme.m_ColorTextMsgBox		= ColorText;
	m_ColorScheme.m_ColorTextEdit		= ColorText;
	m_ColorScheme.m_ColorTextListbox	= ColorText;
	m_ColorScheme.m_ColorTextButton		= ColorText;
	m_ColorScheme.m_ColorBgDialog		= ColorBgDialog;
	m_ColorScheme.m_ColorBgStatic		= ColorBgDialog;
	m_ColorScheme.m_ColorBgMsgBox		= ColorBgDialog;
	m_ColorScheme.m_ColorBgEdit			= ColorBgControl;
	m_ColorScheme.m_ColorBgListbox		= ColorBgControl;
	m_ColorScheme.m_ColorBgButton		= ColorBgControl;
	m_ColorScheme.m_ColorBgScrollbar	= ColorBgDialog;
}

//////////////////////////////////////////////////////////////////////
CProgressDlg::~CProgressDlg()
{
}

//////////////////////////////////////////////////////////////////////
LRESULT CProgressDlg::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	CenterInParent();
	return IDOK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CProgressDlg::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return m_ColorScheme.OnCtlColor(uMsg, wParam);
}

//////////////////////////////////////////////////////////////////////
void CProgressDlg::SetProgressText(CString strText)
{
	SetDlgItemText(IDC_PROGRESSTEXT, strText);
}

//////////////////////////////////////////////////////////////////////
void CProgressDlg::CenterInParent()
{
	RECT rect;
	GetWindowRect(&rect);
	::MapWindowPoints(NULL, GetParent(), (POINT*)&rect.left, 2);
	RECT r;
	{
		// The output rect will be the intersection of the plug-in and its container
		// This is so we can see the message regardless of the view size
		HWND hParent = ::GetParent(m_hWnd);
		HWND hGrandParent = ::GetParent(hParent);
		RECT rp;
		::GetClientRect(hParent, &rp);
		RECT rg;
		::GetClientRect(hGrandParent, &rg);
		::MapWindowPoints(hGrandParent, hParent, (POINT*)&rg, 2);
		::IntersectRect(&r/*out*/, &rp, &rg);
	}

	int dx = (WIDTH(r) - WIDTH(rect)) / 2;
	int dy = (HEIGHT(r) - HEIGHT(rect)) / 2;
	::OffsetRect(&rect, dx - rect.left, dy - rect.top);
	MoveWindow(&rect, true/*bRepaint*/);
	Invalidate(); // Force a repaint even if the size doesn't change
}

//////////////////////////////////////////////////////////////////////
LRESULT CProgressDlg::OnCancel(WORD, WORD, HWND, BOOL&)
{
	return S_OK;
}
