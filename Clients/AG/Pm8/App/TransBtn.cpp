// TransBtn.cpp: implementation of the CTransBtn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "TransBtn.h"
#include "CustCtrlDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


// Repurpose the ODS_GRAYED flag
#define ODS_MOUSEOVER			ODS_GRAYED

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransBtn::CTransBtn():
	m_fMouseIn			(FALSE),
	m_fMouseCaptured	(FALSE),
	m_fMouseDown		(FALSE),
	m_uiState			(0),
	m_nState				(CPImageControl::UP),
	m_pBtnFont			(NULL),
	m_fIsStateBtn		(FALSE)
{

	m_clrText = RGB(0,0,0);		// black
	m_clrDisabledText = RGB(128,128,128);	// gray
	m_clrFocusText = RGB(0, 128, 0);// green
}

CTransBtn::~CTransBtn()
{
	if (m_pBtnFont != NULL)
		delete m_pBtnFont;
}

// implement runtime type checking
IMPLEMENT_DYNCREATE(CTransBtn, CButton)

BEGIN_MESSAGE_MAP(CTransBtn, CButton)
	//{{AFX_MSG_MAP(CTransBtn)
   ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
	
	ON_MESSAGE( WM_SETFONT, OnSetFont )
	ON_MESSAGE( UM_SETPUSHED, OnSetPushed )
	ON_MESSAGE( UM_GETPUSHED, OnGetPushed )
	ON_MESSAGE( UM_SIZECTRL, OnSizeCtrl )

END_MESSAGE_MAP()

// tell the button what the image control is
void CTransBtn::SetImageControl(CPImageControl* pImageCtrl)
{
	m_pImageControl = pImageCtrl;
}

void CTransBtn::FireClick()
{
	
//	if ((GetStyle() & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON)
	if (m_fIsStateBtn)	// if this is a state button
	{
		CWnd* pWnd = GetNextWindow( GW_HWNDPREV );

		// go back in the chain and uncheck
		while (pWnd)
		{
			if (!pWnd->IsKindOf( RUNTIME_CLASS( CTransBtn ) ) )
				break;


			DWORD dwStyle = pWnd->GetStyle();

			// Uncheck the button
			pWnd->SendMessage( UM_SETPUSHED, UST_UNPUSHED, 0L );

			if (dwStyle & WS_GROUP)
				break;

			pWnd = pWnd->GetNextWindow( GW_HWNDPREV );
		}

		// go forward in the chain and uncheck
		pWnd = GetNextWindow( GW_HWNDNEXT );

		while (pWnd)
		{
			DWORD dwStyle = pWnd->GetStyle();

			if (!pWnd->IsKindOf( RUNTIME_CLASS( CTransBtn ) ) )
				break;

			// Uncheck the button
			pWnd->SendMessage( UM_SETPUSHED, UST_UNPUSHED, 0L );
			pWnd = pWnd->GetNextWindow( GW_HWNDNEXT );
		}

	}
	else if ((GetStyle() & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX)
	{
		m_uiState ^= ODS_CHECKED;
	}

	// Send BN_CLICKED message
	GetParent()->PostMessage( WM_COMMAND, MAKEWPARAM( GetDlgCtrlID(), 
		BN_CLICKED ), (LPARAM) m_hWnd );
}

/////////////////////////////////////////////////////////////////////////////
// CTransBtn message handlers


void CTransBtn::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_pImageControl) 
	{
		CButton::OnLButtonDown( nFlags, point );
	}

	if (!m_pImageControl->InClickableRegion(point))	
		return;

	// we are within the clickable area?
	if (GetFocus() != this)
		SetFocus();

	if (!(m_uiState & ODS_SELECTED))
	{
		Invalidate( FALSE );

		m_uiState |= ODS_SELECTED;	// set to selected
		m_nState = CPImageControl::DOWN;

		SetCapture();
		m_fMouseCaptured = TRUE;
		m_fMouseDown = TRUE;

		if (m_fIsStateBtn)
			FireClick();
	}

}

/////////////////////////////////////////////////////////////////////////////
void CTransBtn::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_pImageControl)
	{
		CButton::OnLButtonUp( nFlags, point );
		return;
	}

	if (m_uiState & ODS_SELECTED)
	{
		if ((GetStyle() & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON)
		{
			// Set the checked state, and the tab stop for this control
			::SetWindowLong( m_hWnd, GWL_STYLE, GetStyle() | WS_TABSTOP );
			m_uiState |= ODS_CHECKED;
		}
		else
		{
			if ((GetStyle() & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX)
				m_uiState ^= ODS_CHECKED;
		}

		if (!m_fIsStateBtn)					// not a state button
			m_uiState &= ~ODS_SELECTED;	// turn off selected
		
		int lastState = m_nState;

		CRect rcWin;
		GetWindowRect(&rcWin);
		CPoint ptScreen = point;
		ClientToScreen(&ptScreen);

		BOOL	fInCtrl = FALSE;
		// we are within in the rect and the clickable area?
		if (rcWin.PtInRect(ptScreen) && m_pImageControl->InClickableRegion(point))
		{
			if (m_fIsStateBtn)
				m_nState = CPImageControl::DOWN;	// state buttons stay down
			else
				m_nState = CPImageControl::GLOW;
			fInCtrl = TRUE;
		}
		else
		{
			if (m_uiState & ODS_FOCUS)
				m_nState = CPImageControl::DEFAULT;
			else
				m_nState = CPImageControl::UP;
		}

		if (m_fMouseCaptured)
		{
			ReleaseCapture();
			m_fMouseCaptured = FALSE;
		}
		
		if (lastState != m_nState)
		{
			Invalidate( FALSE );
			RedrawWindow();
		}
		m_fMouseDown = FALSE;

		if (m_fIsStateBtn == FALSE && fInCtrl)
			FireClick();	// only fire if we are within the region
	}

}

/////////////////////////////////////////////////////////////////////////////
void CTransBtn::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (m_uiState & ODS_SELECTED)
	{
		m_uiState &= ~ODS_SELECTED;
	}

	// Send BN_CLICKED message
	GetParent()->PostMessage( WM_COMMAND, MAKEWPARAM( GetDlgCtrlID(), 
		BN_DOUBLECLICKED ), (LPARAM) m_hWnd );
}

/////////////////////////////////////////////////////////////////////////////
void CTransBtn::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_pImageControl)
	{
		CButton::OnMouseMove( nFlags, point );
	}

	int lastState = m_nState;

	CRect rcWin;
	GetWindowRect(&rcWin);
	CPoint ptScreen = point;
	ClientToScreen(&ptScreen);

	// we are within in the rect and the clickable area?
	if (rcWin.PtInRect(ptScreen) && m_pImageControl->InClickableRegion(point))
	{
		if (m_uiState & ODS_SELECTED)
		{
			if (m_fMouseDown)
			{
				// user has moused back in after mouse down
				// don't need to set the m_uiState coz it is at selected
				m_nState = CPImageControl::DOWN;
			}
			else
				m_nState = CPImageControl::SELECTEDGLOW;
		}
		else
		{
			// glow show only happen if button isn't selected
			m_uiState |= ODS_MOUSEOVER;
			m_nState = CPImageControl::GLOW;
		}
		m_fMouseIn = TRUE;
	}
	else
	{
		m_uiState &= ~ODS_MOUSEOVER;	// no longer over
		
		if (m_uiState & ODS_FOCUS)
		{
			if (m_uiState & ODS_SELECTED)	// this can happen if the button is a state button
				m_nState = CPImageControl::DOWN;
			else
				m_nState = CPImageControl::DEFAULT;
		}
		else
		{
			if (m_uiState & ODS_SELECTED)	// this can happen if the button is a state button
				m_nState = CPImageControl::DOWN;
			else
				m_nState = CPImageControl::UP;
		}
		m_fMouseIn = FALSE;
	}

	if (m_nState != lastState)
		Invalidate( FALSE );
	
	// if the mouse isn't in the region and it isn't selected
	// release capture
	if (!m_fMouseIn && !m_fMouseDown)
	{
		ReleaseCapture();
		m_fMouseCaptured = FALSE;
	}
	else
	{
		SetCapture();
		m_fMouseCaptured = TRUE;
	}
/*
	if (m_fMouseIn )//&& !(m_wStyle & IMAGEDATA_ONDOWN))
	{
		CRect rcClient;
		GetClientRect( &rcClient );

		if (!rcClient.PtInRect( point ))
		{
			if (m_uiState & ODS_SELECTED)
			{
				m_uiState &= ~ODS_SELECTED;

				//SetNextState( -1 );
				Invalidate( FALSE );
			}
		}
		else if (!(m_uiState & ODS_SELECTED))
		{
			// Upon re-entry of the client area, push button style 
			// objects should toggle the selection state back on.
			m_uiState |= ODS_SELECTED;

			//SetNextState();
			Invalidate( FALSE );
		}
	}
*/
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTransBtn::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CButton::OnEraseBkgnd(pDC);
}

/////////////////////////////////////////////////////////////////////////////
void CTransBtn::OnPaint() 
{
	if (!m_pImageControl)
	{
		CButton::OnPaint();
	}
	else
	{
		if (IsWindowEnabled() == FALSE)
		{
			// the window is disabled, draw the disable state
			m_nState = CPImageControl::DISABLED;
		}

		CPaintDC dc(this); // device context for painting
		
	
		// get the client rectangle
		CRect rcClient;
		GetClientRect(rcClient);

		// draw the button
		//int retVal = m_pImageControl->Draw(m_nState, destDS, rcClient, rcClient);
		int retVal = m_pImageControl->Draw(m_nState, dc, rcClient, rcClient);

		// get the caption for the button
		CString strCaption;
		GetWindowText(strCaption);

		if (!strCaption.IsEmpty())
		{
			RIntRect  rcText( rcClient );
			
			CFont *oldFont = (CFont*)dc.SelectObject((CFont*)m_pBtnFont);
			dc.DrawText(strCaption, (CRect*)&rcText, DT_CALCRECT);
			rcText.CenterRectInRect( (RIntRect)rcClient );
			
			if (m_nState == CPImageControl::DOWN || m_nState == CPImageControl::SELECTEDGLOW)
			{
				rcClient.OffsetRect(2,2);

				RRealSize theOffset ( 2, 2 );
				rcText.Offset(theOffset);
			}

			// set up the dc for text drawing
			int nPrevMode = dc.SetBkMode(TRANSPARENT);			// draw transparently
	
			// find the appropriate text color
			COLORREF curTextColor = m_clrText;
			if (m_nState == CPImageControl::DISABLED)
				curTextColor = m_clrDisabledText;
			else if (m_uiState & ODS_FOCUS)
				curTextColor = m_clrFocusText;
			
			COLORREF oldColor = dc.SetTextColor(curTextColor);				
			dc.DrawText(strCaption, (CRect*)&rcText, DT_SINGLELINE);
			
			// restore the dc
			dc.SelectObject((CFont*)oldFont);
			dc.SetBkMode(nPrevMode);
			dc.SetTextColor(oldColor);

			if (m_uiState & ODS_FOCUS)
			{
				rcClient.DeflateRect( 10, 5 );
				dc.DrawFocusRect(&rcClient);
			}

		}
		


		/*CRect rect;
		GetClientRect( rect );
		dc.IntersectClipRect( &rect );

		int nState = m_nState;

		// Note, odd states indicate a pressed button which should 
		// already account for the control having focus.
		if ((m_wStyle & IMAGEDATA_wFOCUS) == IMAGEDATA_wFOCUS && 
			(m_uiState & ODS_FOCUS)                           && 
			(m_nState % 2) == 0)
		{
			nState = ((nState + 1) / 2) + m_nImageCount;
		}
		else if (
			(m_wStyle & IMAGEDATA_wFOCUS) == IMAGEDATA_wFOCUS && 
			(m_uiState & ODS_MOUSEOVER)                       && 
			(m_nState % 2) == 0)
		{
			nState = ((nState + 1) / 2) + m_nImageCount;
		}

		m_cImageData.Draw( &dc, nState, rect.TopLeft(), ILD_NORMAL );

*/
	}
}


/////////////////////////////////////////////////////////////////////////////
LRESULT CTransBtn::OnSetPushed( WPARAM wParam, LPARAM )
{
	if (!m_pImageControl)
		return Default();

	DWORD dwStyle = GetStyle();

	// want the button pushed but it isn't yet
	if (UST_PUSHED == wParam && !(m_uiState & ODS_SELECTED))
	{
		::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle | WS_TABSTOP );

		m_uiState |= ODS_SELECTED;
		m_nState = CPImageControl::DOWN;
		Invalidate( FALSE );
	}
	// want the button unpushed but it is pushed already
	else if (UST_PUSHED != wParam && (m_uiState & ODS_SELECTED))
	{
		::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle & ~WS_TABSTOP);

		m_uiState &= ~ODS_SELECTED;
		m_nState = CPImageControl::UP;
		Invalidate( FALSE );
	}

	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CTransBtn::OnGetPushed( WPARAM, LPARAM )
{
	if (!m_pImageControl)
		return Default();

	if (m_uiState & ODS_SELECTED)
		return UST_PUSHED;

	return UST_UNPUSHED;
}


/////////////////////////////////////////////////////////////////////////////
void CTransBtn::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
 	if (nChar == ' ')
	{
		if (!(m_uiState & ODS_SELECTED))
		{
			CRect rcClient;
			GetClientRect(&rcClient);
			// tell it mouse down smack in the center of the button
			// where it should be solid
			CTransBtn::OnLButtonDown( nFlags, CPoint( rcClient.Width()/2, rcClient.Height()/2 ) );
		}
		return;
	}
	
	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
void CTransBtn::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == ' ')
	{
		if (m_uiState & ODS_SELECTED)
		{
			CRect rcClient;
			GetClientRect(&rcClient);
			// tell it mouse up smack in the center of the button
			// where it should be solid
			CTransBtn::OnLButtonUp( nFlags, CPoint( rcClient.Width()/2, rcClient.Height()/2 ) );
		}
		return;
	}
	
	CButton::OnKeyUp(nChar, nRepCnt, nFlags);
}


/////////////////////////////////////////////////////////////////////////////
LRESULT CTransBtn::OnSizeCtrl( WPARAM wParam, LPARAM lParam)
{
	if (m_pImageControl)
	{
		CRect *rcSize = (CRect*)lParam;
		m_pImageControl->SetControlSize( *rcSize );
	}

	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CTransBtn::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	CFont *pNewFont = new CFont;
	if (wParam != 0)
	{
		LOGFONT LogFont;
		if (::GetObject((HFONT)wParam, sizeof(LogFont), &LogFont) != 0)
		{
			pNewFont->CreateFontIndirect( &LogFont );
		}
	}
	
	if (m_pBtnFont)
		delete m_pBtnFont;	// clear out the old
	m_pBtnFont = pNewFont;
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CTransBtn::OnSetFocus(CWnd* pOldWnd) 
{
	if (!m_pImageControl)
	{
		CButton::OnSetFocus(pOldWnd);
		return;
	}

	m_uiState |= ODS_FOCUS;
	if (m_uiState & ODS_SELECTED)	// this can happen if the button is a state button
		m_nState = CPImageControl::DOWN;
	else
		m_nState = CPImageControl::DEFAULT;
	Invalidate( FALSE );	
}

/////////////////////////////////////////////////////////////////////////////
void CTransBtn::OnKillFocus(CWnd* pNewWnd) 
{
	if (!m_pImageControl)
	{
		CButton::OnKillFocus(pNewWnd);
		return;
	}
	
	m_uiState &= ~ODS_FOCUS;
	if (m_uiState & ODS_SELECTED)	// this can happen if the button is a state button
		m_nState = CPImageControl::DOWN;
	else
		m_nState = CPImageControl::UP;
	Invalidate( FALSE );	
}

