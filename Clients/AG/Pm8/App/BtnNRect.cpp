//
// $Workfile: BtnNRect.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//	  Copyright © 1997 Mindscape, Inc. All rights reserved.
//
//
// Revision History:
//
// $Log: /PM8/App/BtnNRect.cpp $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 3     4/20/98 7:38p Fredf
// Uses Util::LoadResourceBitmap to do 256-colors correctly.
// 
// 2     12/04/97 5:23p Hforman
// a bit more functionality
// 
// 1     12/04/97 12:37p Hforman
//

#include "stdafx.h"
#include "pmw.h"
#include "Util.h"
#include "BtnNRect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNonRectButton
//
// Creates non-rectangular buttons. Must have an "Up" and a "Down" bitmap
// in the resource file. Any color in these bitmaps can be the transparent
// color (call SetTransparentColor() to set it.) Only the non-transparent
// parts of the bitmaps will be part of the button.
//
//////////////////////////////////////////////////////////////////////////////
CNonRectButton::CNonRectButton()
{
	m_pbmUp = NULL;
	m_pbmDown = NULL;

	// set default transparent color to black
	m_clrTransparent = RGB(0,0,0);
}

CNonRectButton::~CNonRectButton()
{
	DeleteBitmaps();
}

BOOL CNonRectButton::Create(CRect rect, CWnd* pParentWnd, UINT nID)
{
	 return CWnd::Create(NULL, "", WS_CHILD|WS_VISIBLE, rect, pParentWnd, nID);
}

void CNonRectButton::SetBitmaps(UINT nUpResID, UINT nDownResID, CPalette* pPalette /*=NULL*/)
{
	DeleteBitmaps();

	m_pbmUp = new CBitmap;
	if (!Util::LoadResourceBitmap(*m_pbmUp, MAKEINTRESOURCE(nUpResID), pPalette))
	{
		delete m_pbmUp;
		m_pbmUp = NULL;
	}

	m_pbmDown = new CBitmap;
	if (!Util::LoadResourceBitmap(*m_pbmDown, MAKEINTRESOURCE(nDownResID), pPalette))
	{
		delete m_pbmDown;
		m_pbmDown = NULL;
	}

	SizeButton();
}

void CNonRectButton::SetBitmaps(LPCTSTR strUpRes, LPCTSTR strDownRes, CPalette* pPalette /*=NULL*/)
{
	DeleteBitmaps();

	m_pbmUp = new CBitmap;
	if (!Util::LoadResourceBitmap(*m_pbmUp, strUpRes, pPalette))
	{
		delete m_pbmUp;
		m_pbmUp = NULL;
	}

	m_pbmDown = new CBitmap;
	if (!Util::LoadResourceBitmap(*m_pbmDown, strDownRes, pPalette))
	{
		delete m_pbmDown;
		m_pbmDown = NULL;
	}

	SizeButton();
}

void CNonRectButton::SizeButton()
{
	// size button to "Up" bitmap
	if (m_pbmUp)
	{
		BITMAP bm;
		m_pbmUp->GetBitmap(&bm);
		SetWindowPos(NULL, 0, 0, bm.bmWidth, bm.bmHeight, SWP_NOZORDER | SWP_NOMOVE);
	}
}

void CNonRectButton::DeleteBitmaps()
{
	if (m_pbmUp)
	{
		m_pbmUp->DeleteObject();
		delete m_pbmUp;
		m_pbmUp = NULL;
	}
	if (m_pbmDown)
	{
		m_pbmDown->DeleteObject();
		delete m_pbmDown;
		m_pbmDown = NULL;
	}
}

BEGIN_MESSAGE_MAP(CNonRectButton, CButtonWnd)
	//{{AFX_MSG_MAP(CNonRectButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNonRectButton message handlers

BOOL CNonRectButton::OverTransparentColor(CPoint point)
{
	// Get the color at point
	CDC* pDC = GetDC();
	CDC dcBitmap;
	dcBitmap.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dcBitmap.SelectObject(m_pbmUp);
	COLORREF color = dcBitmap.GetPixel(point.x, point.y);
	dcBitmap.SelectObject(pOldBitmap);
	ReleaseDC(pDC);

	return (color == m_clrTransparent);
}

void CNonRectButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Test if we're over transparent area. If so, send WM_LBUTTONDOWN
	// message to parent and exit.
	if (OverTransparentColor(point))
	{
		CWnd* pParent = GetParent();
		if (pParent)
		{
			CPoint pt;
			::GetCursorPos(&pt);
			pParent->ScreenToClient(&pt);
			pParent->SendMessage(WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(pt.x, pt.y));
		}
	}
	else
	{
		// normal button processing
		CButtonWnd::OnLButtonDown(nFlags, point);
	}
}

void CNonRectButton::OnMouseMove(UINT nFlags, CPoint point) 
{
   // If the user moves the mouse off the button with LBUTTONDOWN,
	// pop it up until it's moved back on.
   if (GetCapture() == this && (nFlags & MK_LBUTTON))
   {
      CRect crClient;
      GetClientRect(crClient);
      SetSelect(crClient.PtInRect(point) && !OverTransparentColor(point));
   }
}

void CNonRectButton::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect crButton;
	GetClientRect(crButton);

	BOOL fSelected = GetSelect();

	CDC dcBitmap;
	dcBitmap.CreateCompatibleDC(&dc);
	CBitmap* pBitmap = fSelected ? m_pbmDown : m_pbmUp;
	dcBitmap.SelectObject(pBitmap);

//	dc.BitBlt(0, 0, crButton.Width(), crButton.Height(), &dcBitmap, 0, 0, SRCCOPY);
	Util::TransparentBlt(dc, crButton, dcBitmap, crButton, m_clrTransparent);
}

