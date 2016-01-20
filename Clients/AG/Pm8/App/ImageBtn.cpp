//****************************************************************************
//
// File Name:  ImageBtn.cpp
//
// Project:    Renaissance headline user interface
//
// Author:     Lance Wilson
//
// Description: Implements the ROwnerDrawBtn class
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software, Inc.
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/ImageBtn.cpp                                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:06p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "stdafx.h"
ASSERTNAME

#include "ImageBtn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT kCheckState   = 0x0001 ;
const UINT kHilightState = 0x0004 ;
const UINT kFocusState   = 0x0008 ;

/////////////////////////////////////////////////////////////////////////////
// ROwnerDrawBtn

ROwnerDrawBtn::ROwnerDrawBtn () 
{
	m_uiBtnState = 0 ;
	m_hBitmap = NULL ;

	m_nImageNormal  = -1 ; 
	m_nImageFocus   = -1 ; 
	m_nImageDisable = -1 ; 

	m_cxItemWidth  = 0 ;
	m_cyItemHeight = 0 ;
}

ROwnerDrawBtn::~ROwnerDrawBtn()
{
	if (m_hBitmap)
		::DeleteObject (m_hBitmap) ;
}


BEGIN_MESSAGE_MAP(ROwnerDrawBtn, CButton)
	//{{AFX_MSG_MAP(ROwnerDrawBtn)
	ON_MESSAGE(BM_GETCHECK, OnGetCheck)
	ON_MESSAGE(BM_SETCHECK, OnSetCheck)
	ON_MESSAGE(BM_GETSTATE, OnGetState)
	ON_MESSAGE(BM_SETSTATE, OnSetState)
	ON_MESSAGE(WM_ENABLE, OnEnable)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ROwnerDrawBtn message handlers

void ROwnerDrawBtn::OnPaint() 
{
	// Draw Entire
	CPaintDC dc (this) ;
	SendDrawItem (ODA_DRAWENTIRE, &dc) ;
}

void ROwnerDrawBtn::PreSubclassWindow() 
{
//	uiBtnStyle = GetButtonStyle () | BS_OWNERDRAW ;
//	SetButtonStyle (BS_OWNERDRAW) ;
	m_uiBtnState = GetState() ;
	
	CButton::PreSubclassWindow();
}

LRESULT ROwnerDrawBtn::OnGetState(WPARAM, LPARAM)
{
	return (LRESULT) m_uiBtnState ;
}

LRESULT ROwnerDrawBtn::OnSetState(WPARAM wParam, LPARAM)
{
	if (!wParam)
	{
		// Clear the state flag.
		m_uiBtnState &= ~kHilightState ;
	}
	else
	{
		// Set the state flag.
		m_uiBtnState |= kHilightState ;
	}

	// Update display
	CClientDC dc(this) ;
	SendDrawItem (ODA_SELECT, &dc) ;

	return 0L ;
}

LRESULT ROwnerDrawBtn::OnGetCheck(WPARAM, LPARAM)
{
	return (LRESULT) (m_uiBtnState & kCheckState) ;
}

LRESULT ROwnerDrawBtn::OnSetCheck(WPARAM wParam, LPARAM)
{

	UINT uiDlgCode = SendMessage (WM_GETDLGCODE) ;

	if (wParam)
	{
		m_uiBtnState |= static_cast<UINT>( wParam ) & kCheckState ;

		if (uiDlgCode & DLGC_RADIOBUTTON)
			SetWindowLong (m_hWnd, GWL_STYLE, GetStyle() | WS_TABSTOP) ;

		//
		// REVIEW: move the below code to the mouse up handler>
		//
		if (GetStyle() & BS_AUTORADIOBUTTON)
		{
			CWnd* pWnd = this ;

			while (pWnd && !(pWnd->GetStyle() & WS_GROUP))
				pWnd = pWnd->GetNextWindow (GW_HWNDPREV) ;

			if (pWnd)
			{
				do
				{
					if (pWnd != this && pWnd->SendMessage (WM_GETDLGCODE) & DLGC_RADIOBUTTON)
					{
						if (pWnd->SendMessage (BM_GETCHECK))
						{
							pWnd->SendMessage (BM_SETCHECK, FALSE) ;
							break ;
						}
					}

					pWnd = pWnd->GetNextWindow (GW_HWNDNEXT) ;

				} while (pWnd && !(pWnd->GetStyle() & WS_GROUP)) ;
			}

		} // endif (BS_AUTORADIOBUTTON)
	}
	else
	{
		m_uiBtnState &= ~kCheckState ;

		if (uiDlgCode & DLGC_RADIOBUTTON)
			SetWindowLong (m_hWnd, GWL_STYLE, GetStyle() & ~WS_TABSTOP) ;
	}

	// Update display
	CClientDC dc(this) ;
	SendDrawItem (ODA_SELECT, &dc) ;

	return 0L ;
}

LRESULT ROwnerDrawBtn::OnEnable(WPARAM wParam, LPARAM lParam) 
{
//	CButton::OnEnable(bEnable);

	InvalidateRect (NULL) ;
	return ::DefWindowProc (m_hWnd, WM_ENABLE, wParam, lParam) ;
}

void ROwnerDrawBtn::OnSetFocus(CWnd* /*pOldWnd*/) 
{
	m_uiBtnState |= kFocusState ;

	// Draw focused
	CClientDC dc(this) ;
	SendDrawItem (ODA_FOCUS, &dc) ;
}

void ROwnerDrawBtn::OnKillFocus(CWnd* /*pNewWnd*/) 
{
	m_uiBtnState &= ~kFocusState ;

	// Draw unfocused
	CClientDC dc(this) ;
	SendDrawItem (ODA_FOCUS, &dc) ;
}

void ROwnerDrawBtn::OnLButtonUp(UINT /*nFlags*/, CPoint point) 
{
	if (GetState() & kHilightState)
		SetState (FALSE) ;

	CRect rect ;
	GetClientRect (rect) ;

	if (rect.PtInRect (point))
	{
		LRESULT lDlgCode   = SendMessage (WM_GETDLGCODE) ;
		UINT    uiBtnStyle = GetStyle() ;

		if (lDlgCode & DLGC_RADIOBUTTON && uiBtnStyle & BS_AUTORADIOBUTTON)
		{
			SetCheck (TRUE) ;
		}

		else if (uiBtnStyle & BS_AUTOCHECKBOX)
		{
			SetCheck (!GetCheck()) ;
		}


		// Tell the parent that we've been clicked.
		GetParent()->SendMessage (
			WM_COMMAND, 
			MAKEWPARAM ((WORD) GetDlgCtrlID(), (WORD) BN_CLICKED), 
			(LPARAM) m_hWnd) ;
	}

	::ReleaseCapture () ;
}

void ROwnerDrawBtn::SendDrawItem (UINT uiItemAction, CDC* pDC) 
{
	DRAWITEMSTRUCT itemData ;
	UINT uiBtnState = GetState() ;

	itemData.CtlType    = (UINT) ODT_BUTTON ;
	itemData.CtlID      = (UINT) GetDlgCtrlID() ;
	itemData.itemID     = 0 ;
	itemData.itemAction = uiItemAction ;
	itemData.itemState  = 0 ;
	itemData.hwndItem   = m_hWnd ;
	itemData.hDC		= pDC->GetSafeHdc() ;
	itemData.itemData   = 0L ;

	GetClientRect (&itemData.rcItem) ;

	if (uiBtnState & kFocusState)
		itemData.itemState |= ODS_FOCUS ;

	if (uiBtnState & kHilightState)
		itemData.itemState |= ODS_SELECTED ;

	if (uiBtnState & kCheckState)
		itemData.itemState |= ODS_CHECKED ;

	if (!IsWindowEnabled())
		itemData.itemState |= ODS_DISABLED ; 

	GetParent()->SendMessage (WM_DRAWITEM, itemData.CtlID, (LPARAM) &itemData) ;
}

void ROwnerDrawBtn::DrawButton(CDC* pDC, CRect& rectItem, UINT uiItemState) 
{
	CRect rect (rectItem) ;

	//
	// Draw the button
	//

	if (uiItemState & ODS_CHECKED)
	{
		static BITMAP   bitmap = { 0, 8, 8, 2, 1, 1 } ;
		static WORD wPatternBits[] = 
		{ 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA } ;
		
		try
		{
			CBitmap bmpPattern ;
			CRect rectPattern (rect.left + 2, rect.top + 2,
				rect.right - 2, rect.bottom - 2) ;

			bitmap.bmBits = (LPSTR) wPatternBits ;
			bmpPattern.CreateBitmapIndirect (&bitmap) ;

			CBrush   brush (&bmpPattern) ;
			COLORREF crOldBackColor = pDC->SetBkColor(GetSysColor (COLOR_BTNFACE)) ;
			COLORREF crOldTextColor = pDC->SetTextColor(GetSysColor (COLOR_BTNHILIGHT)) ;

			pDC->FillSolidRect (rect, GetSysColor (COLOR_BTNFACE)) ;
			pDC->FillRect (rectPattern, &brush) ;
			pDC->SetBkColor (crOldBackColor) ;
			pDC->SetTextColor (crOldTextColor) ;
		}
		catch (...)
		{
		}
	}
	else 
		pDC->FillSolidRect (rect, GetSysColor (COLOR_BTNFACE)) ;

	if (uiItemState & (ODS_CHECKED | ODS_SELECTED))
	{
		// Draw the button pressed
		pDC->Draw3dRect (rect, RGB (0, 0, 0), 
			GetSysColor (COLOR_BTNHILIGHT)) ;

		CPen pen (PS_SOLID, 1, GetSysColor (COLOR_BTNSHADOW)) ;
		CPen *pOldPen = pDC->SelectObject (&pen) ;

		pDC->MoveTo (rect.left + 1, rect.bottom - 3) ;
		pDC->LineTo (rect.left + 1, rect.top + 1) ;
		pDC->LineTo (rect.right - 2, rect.top + 1) ;

		pDC->SelectObject (pOldPen) ;
	}
	else
	{
		
		rect.right--; rect.bottom-- ;

		// Draw the button normal
		pDC->Draw3dRect (rect, 
			GetSysColor (COLOR_BTNHILIGHT), 
			GetSysColor (COLOR_BTNSHADOW)) ;

		HPEN hPen = (HPEN) pDC->SelectObject (::GetStockObject (BLACK_PEN)) ;

		pDC->MoveTo (rect.left, rect.bottom) ;
		pDC->LineTo (rect.right, rect.bottom) ;
		pDC->LineTo (rect.right, rect.top - 1) ;

		pDC->SelectObject (hPen) ;
	}
}

void ROwnerDrawBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	BOOL  fImageDrawn = FALSE ;
	CRect rect(lpDrawItemStruct->rcItem) ;

	CDC dc ;
	dc.Attach (lpDrawItemStruct->hDC) ;

	UINT uiDrawState = 0 ;

	if (lpDrawItemStruct->itemAction == ODA_SELECT   &&
		(lpDrawItemStruct->itemState  &  ODS_SELECTED) &&
		!(SendMessage (WM_GETDLGCODE)  &  DLGC_RADIOBUTTON))

		DrawButton (&dc, rect, ODS_SELECTED) ;

	else
		DrawButton (&dc, rect, lpDrawItemStruct->itemState) ;


	//
	// Draw the focus rect if necessary
	//
	if (lpDrawItemStruct->itemState & ODS_FOCUS)
	{
	}

	dc.Detach () ;
}

void ROwnerDrawBtn::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_SPACE)
	{
		if (GetState() & kHilightState)
			SetState (FALSE) ;

		LRESULT lDlgCode   = SendMessage (WM_GETDLGCODE) ;
		UINT    uiBtnStyle = GetStyle() ;

		if (lDlgCode & DLGC_RADIOBUTTON && uiBtnStyle & BS_AUTORADIOBUTTON)
		{
			SetCheck (TRUE) ;
		}

		else if (uiBtnStyle & BS_AUTOCHECKBOX)
		{
			SetCheck (!GetCheck()) ;
		}


		// Tell the parent that we've been clicked.
		GetParent()->SendMessage (
			WM_COMMAND, 
			MAKEWPARAM ((WORD) GetDlgCtrlID(), (WORD) BN_CLICKED), 
			(LPARAM) m_hWnd) ;

		::ReleaseCapture () ;

		return ;
	}
	
	CButton::OnKeyUp(nChar, nRepCnt, nFlags);
}



RImageBtn::RImageBtn()
{
	m_pImage = NULL ;
	m_nImageIndex = 0 ;
}

void RImageBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ROwnerDrawBtn::DrawItem (lpDrawItemStruct) ;

	if (!m_pImage)
	{
		return ;
	}

	CDC dc ;
	dc.Attach (lpDrawItemStruct->hDC) ;

	CRect rect ;
	GetClientRect (rect) ;

	CSize size (m_pImage->GetBitmapDimensions()) ;
	CRect rectImage (0, 0, size.cx, size.cy) ;

	rectImage.OffsetRect (
		(rect.Width() / 2 - size.cx / 2) - 1,
		(rect.Height() / 2 - size.cy / 2) - 1) ;

	if (lpDrawItemStruct->itemState & (ODS_CHECKED | ODS_SELECTED))
	{
			rectImage.OffsetRect (1, 1) ;
	}

	CPoint pt = rectImage.TopLeft() ;

	UINT uiFlags = RWindowsImageList::kTransparent ;

	if (lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		uiFlags |= RWindowsImageList::kFocused ;
	}

	if (!IsWindowEnabled())
	{
		uiFlags |= RWindowsImageList::kDisabled ;
	}

	m_pImage->Draw( dc, pt, m_nImageIndex, uiFlags ) ;
//	else
//	{
//		m_bmpImage.DrawDisabled (dc, (short) rectImage.left, 
//			(short) rectImage.top) ;
//	}

//	if (lpDrawItemStruct->itemState & ODS_FOCUS)
//	{
//		dc.DrawFocusRect (rectImage) ;
//	}

	dc.Detach() ;
}

void RImageBtn::SetImage( RWindowsImageList* pImage, int nIndex )
{
	m_pImage      = pImage ;
	m_nImageIndex = nIndex ;

	if (GetSafeHwnd())
	{
		InvalidateRect( NULL ) ;
	}
}

