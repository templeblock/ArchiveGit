// PSButton.cpp : implementation file
//

#include "stdafx.h"
#include "LaunchResource.h"

#include "PSButton.h"

// Repurpose the ODS_GRAYED flag
#define ODS_MOUSEOVER			ODS_GRAYED

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPSButton

CPSButton::CPSButton( BOOL fAutoDelete /* = FALSE*/ ) :
	m_fAutoDelete( fAutoDelete ),
	m_dwUserData( 0L ),
	m_wInfoTextId( 0 )
{
//	m_fResize		 = FALSE;
	m_fMouseDown	 = FALSE;
	m_fMouseCaptured = FALSE;
	m_uiState		 = 0;

	m_nState      = 0;
	m_nImageCount = 0;
}

CPSButton::~CPSButton()
{
}

BOOL CPSButton::Initialize( CArchive& ar, DWORD dwVersion )
{
	ar >> m_wStyle;

	WORD wHigh, wLow;
	ar >> wLow;	ar >> wHigh;
	m_dwUserData = MAKELONG( wLow, wHigh );

	if (m_wStyle & IMAGEDATA)
	{
		WORD wWidth = 0;
		WORD wImageListID = 0;
		ar >> wImageListID;

		if (wImageListID)
		{
			ar >> m_crTransColor;
			ar >> wWidth;

			BITMAP bm;
			CBitmap bitmap;
			bitmap.LoadBitmap( wImageListID );
			bitmap.GetObject( sizeof( bm ), &bm );

			if (m_cImageData.Create( wWidth, bm.bmHeight, ILC_COLOR16, bm.bmWidth / wWidth, 0 ))
			{
				m_cImageData.Add( &bitmap, m_crTransColor );
				m_nImageCount = m_cImageData.GetImageCount();
			}
		}

		if ((m_wStyle & IMAGEDATA_wFOCUS) == IMAGEDATA_wFOCUS)
		{
			ar >> wImageListID;

			if (wImageListID)
			{
				BITMAP bm;
				CBitmap bitmap;
				bitmap.LoadBitmap( wImageListID );
				bitmap.GetObject( sizeof( bm ), &bm );

				// Add focus images to end of list
				m_cImageData.Add( &bitmap, m_crTransColor );
			}
		}
	}

	if (m_wStyle & IMAGEDATA_INFOTEXT)
	{
		ar >> m_wInfoTextId;
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CPSButton, CButton)
	//{{AFX_MSG_MAP(CPSButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP

	ON_MESSAGE( BM_SETCHECK, OnSetCheck )
	ON_MESSAGE( BM_GETCHECK, OnGetCheck )
	ON_MESSAGE( UM_MOUSEENTER, OnMouseEnter )
	ON_MESSAGE( UM_MOUSEEXIT, OnMouseExit )
END_MESSAGE_MAP()

BOOL CPSButton::GetInfoText( CString& str )
{
	if (m_wInfoTextId && str.LoadString( m_wInfoTextId ))
		return TRUE;

	return FALSE;
}

void CPSButton::FireClick()
{
	if ((GetStyle() & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON)
	{
		HWND  hWnd = ::GetNextWindow( m_hWnd, GW_HWNDPREV ) ;

		while (hWnd)
		{
			DWORD dwStyle = ::GetWindowLong( hWnd, GWL_STYLE );

			if ((dwStyle & BS_AUTORADIOBUTTON) != BS_AUTORADIOBUTTON)
				break;

			// Uncheck the button
			::SendMessage( hWnd, BM_SETCHECK, BST_UNCHECKED, 0L );

			if (dwStyle & WS_GROUP)
				break;

			hWnd = ::GetNextWindow( hWnd, GW_HWNDPREV );
		}

		hWnd = ::GetNextWindow( m_hWnd, GW_HWNDNEXT );

		while (hWnd)
		{
			DWORD dwStyle = ::GetWindowLong( hWnd, GWL_STYLE );

			if ((dwStyle & BS_AUTORADIOBUTTON) != BS_AUTORADIOBUTTON || (dwStyle & WS_GROUP))
				break;

			// Uncheck the button
			::SendMessage( hWnd, BM_SETCHECK, BST_UNCHECKED, 0L );
			hWnd = ::GetNextWindow( hWnd, GW_HWNDNEXT );
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

void CPSButton::SetNextState( int nAmt /*= 1*/ )
{
	m_nState += nAmt;

	if (m_nState >= m_nImageCount)
	{
		m_nState %= m_nImageCount;
	}
	else if (m_nState < 0)
	{
		m_nState += m_nImageCount;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPSButton message handlers

void CPSButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_cImageData.m_hImageList)
	{
		if (GetFocus() != this)
			SetFocus();

		if (!(m_uiState & ODS_SELECTED))
		{
			SetNextState();
			Invalidate( FALSE );

			m_uiState |= ODS_SELECTED;
			m_fMouseDown = TRUE;

			SetCapture();
			m_fMouseCaptured = TRUE;

			if (m_wStyle & IMAGEDATA_ONDOWN)
			{
				FireClick();
			}
		}
	}
	else
	{
		CButton::OnLButtonDown( nFlags, point );
	}
}

void CPSButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_cImageData.m_hImageList)
	{
		if (m_uiState & ODS_SELECTED)
		{
			if ((GetStyle() & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON)
			{
				int nDir = (m_uiState & ODS_CHECKED ? -1 : 1);
				SetNextState( nDir );
				
				// Set the checked state, and the tab stop for this control
				::SetWindowLong( m_hWnd, GWL_STYLE, GetStyle() | WS_TABSTOP );
				m_uiState |= ODS_CHECKED;
			}
			else
			{
				SetNextState();

				if ((GetStyle() & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX)
					m_uiState ^= ODS_CHECKED;
			}

			m_uiState &= ~ODS_SELECTED;
			Invalidate( FALSE );

			// Send BN_CLICKED message
			if (!(m_wStyle & IMAGEDATA_ONDOWN))
				FireClick();
		}

		if (m_fMouseCaptured)
		{
			ReleaseCapture();
			m_fMouseCaptured = FALSE;
		}

		m_fMouseDown = FALSE;
	}
	else
	{
		CButton::OnLButtonUp( nFlags, point );
	}
}

void CPSButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (m_uiState & ODS_SELECTED)
	{
		m_uiState &= ~ODS_SELECTED;
	}

	// Send BN_CLICKED message
	GetParent()->PostMessage( WM_COMMAND, MAKEWPARAM( GetDlgCtrlID(), 
		BN_DOUBLECLICKED ), (LPARAM) m_hWnd );
}

void CPSButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_cImageData.m_hImageList)
	{
		if (m_fMouseCaptured && !(m_wStyle & IMAGEDATA_ONDOWN))
		{
			CRect rcClient;
			GetClientRect( &rcClient );

			if (!rcClient.PtInRect( point ))
			{
				if (m_uiState & ODS_SELECTED)
				{
					m_uiState &= ~ODS_SELECTED;

					SetNextState( -1 );
					Invalidate( FALSE );
				}
			}
			else if (!(m_uiState & ODS_SELECTED))
			{
				// Upon re-entry of the client area, push button style 
				// objects should toggle the selection state back on.
				m_uiState |= ODS_SELECTED;

				SetNextState();
				Invalidate( FALSE );
			}
		}

		// Notify parent
		GetParent()->PostMessage( UM_MOUSEMOVE, GetDlgCtrlID(), (LPARAM) m_hWnd );
	}
	else
	{
		CButton::OnMouseMove( nFlags, point );
	}
}

BOOL CPSButton::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CButton::OnEraseBkgnd(pDC);
}

void CPSButton::OnPaint() 
{
	if (!m_cImageData.m_hImageList)
	{
		CButton::OnPaint();
	}
	else
	{
		CPaintDC dc(this); // device context for painting

		CRect rect;
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

		if (m_wStyle & IMAGEDATA_TEXT)
		{
			CFont* pFont = GetParent()->GetFont();
			CFont* pOldFont = dc.SelectObject( pFont );
			int nOldMode = dc.SetBkMode( TRANSPARENT );

			CString str;
			GetWindowText( str );
			dc.DrawText( str, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
			dc.SelectObject( pOldFont );
			dc.SetBkMode( nOldMode );
		}
	}
}

void CPSButton::OnDestroy() 
{
	CButton::OnDestroy();

	if (m_fAutoDelete)
		delete this;
}

LRESULT CPSButton::OnSetCheck( WPARAM wParam, LPARAM )
{
	if (!m_cImageData.m_hImageList)
		return Default();

	DWORD dwStyle = GetStyle();

	if (BST_CHECKED == wParam && !(m_uiState & ODS_CHECKED))
	{
		::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle | WS_TABSTOP );

		m_uiState |= ODS_CHECKED;
		SetNextState( 2 );
		Invalidate( FALSE );
	}
	else if (BST_CHECKED != wParam && (m_uiState & ODS_CHECKED))
	{
		::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle & ~WS_TABSTOP);

		m_uiState &= ~ODS_CHECKED;
		SetNextState( -2 );
		Invalidate( FALSE );
	}

	return 0L;
}

LRESULT CPSButton::OnGetCheck( WPARAM, LPARAM )
{
	if (!m_cImageData.m_hImageList)
		return Default();

	if (m_uiState & ODS_CHECKED)
		return BST_CHECKED;

	return BST_UNCHECKED;
}


void CPSButton::OnSetFocus(CWnd* pOldWnd) 
{
	if (!m_cImageData.m_hImageList)
	{
		CButton::OnSetFocus(pOldWnd);
	
		return;
	}

	m_uiState |= ODS_FOCUS;
	Invalidate( FALSE );	
}

void CPSButton::OnKillFocus(CWnd* pNewWnd) 
{
	if (!m_cImageData.m_hImageList)
	{
		CButton::OnKillFocus(pNewWnd);
	
		return;
	}
	
	m_uiState &= ~ODS_FOCUS;
	Invalidate( FALSE );	
}

void CPSButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == ' ')
	{
		if (!(m_uiState & ODS_SELECTED))
			CPSButton::OnLButtonDown( nFlags, CPoint( 1, 1 ) );
		
		return;
	}
	
	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPSButton::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == ' ')
	{
		if (m_uiState & ODS_SELECTED)
			CPSButton::OnLButtonUp( nFlags, CPoint( 1, 1 ) );
		
		return;
	}
	
	CButton::OnKeyUp(nChar, nRepCnt, nFlags);
}

LRESULT CPSButton::OnMouseEnter( WPARAM, LPARAM )
{
	m_uiState |= ODS_MOUSEOVER;
	Invalidate( FALSE );

	return 0L;
}

LRESULT CPSButton::OnMouseExit( WPARAM, LPARAM )
{
	m_uiState &= ~ODS_MOUSEOVER;
	Invalidate( FALSE );

	return 0L;
}
