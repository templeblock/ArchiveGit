// tilesbtn.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "tilesbtn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTilesButton

CTilesButton::CTilesButton()
{
	m_captured = FALSE;
}

CTilesButton::~CTilesButton()
{
}


BEGIN_MESSAGE_MAP(CTilesButton, CButton)
	//{{AFX_MSG_MAP(CTilesButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTilesButton message handlers

void CTilesButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	m_captured = TRUE;

	::SendMessage(::GetParent(GetSafeHwnd()), UWM_CHILDBUTTONDOWN, nFlags, MAKELPARAM(point.x, point.y));
}

void CTilesButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_captured)
	{
		m_captured = FALSE;

		ReleaseCapture();

		::SendMessage(::GetParent(GetSafeHwnd()), UWM_CHILDBUTTONUP, nFlags, MAKELPARAM(point.x, point.y));
	}
}

void CTilesButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_captured)
	{
		if (nFlags & MK_LBUTTON)
		{
		/* Button still down. */
			::SendMessage(::GetParent(GetSafeHwnd()), UWM_CHILDMOUSEMOVE, nFlags, MAKELPARAM(point.x, point.y));
		}
		else
		{
		/* Somehow lost the UP message. */
			ReleaseCapture();
			m_captured = FALSE;
		}
	}
}

void CTilesButton::OnSetFocus(CWnd* pOldWnd)
{
/* We don't want it! */
	pOldWnd->SetFocus();
}
