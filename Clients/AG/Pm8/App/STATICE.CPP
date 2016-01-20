/*
// $Workfile: STATICE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/STATICE.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 3     6/09/98 10:31a Mwilson
// changed defaults to transparent bkgnd
// 
// 2     2/27/98 11:15a Hforman
// checking for TRANSPARENT_COLOR in OnPaint()
// 
//    Rev 1.1   13 Oct 1997 13:38:22   hal
// set m_bInitialized flag in OnSetText()
// 
//    Rev 1.0   14 Aug 1997 15:26:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:34   Fred
// Initial revision.
// 
//    Rev 1.2   29 May 1997 11:50:30   dennisp
// Added support for SetWindowText()
// 
//    Rev 1.1   25 Apr 1997 11:43:22   Jay
// Added GetSuperWndProc
// 
//    Rev 1.0   22 Apr 1997 12:23:18   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "pagedefs.h"	// for TRANSPARENT_COLOR
#include "statice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

WNDPROC CStaticExtended::m_pfnSuperWndProc = NULL;

/////////////////////////////////////////////////////////////////////////////
// CStaticExtended

CStaticExtended::CStaticExtended()
{
   m_nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
   m_rgbBkColor    = TRANSPARENT_COLOR;  // Gray
   m_rgbTextColor  = GetSysColor(COLOR_WINDOWTEXT);  // Black
   m_hPalette = NULL;
   m_bInitialized = FALSE;	
}

CStaticExtended::~CStaticExtended()
{
}


BEGIN_MESSAGE_MAP(CStaticExtended, CWnd)
   //{{AFX_MSG_MAP(CStaticExtended)
   ON_WM_ERASEBKGND()
   ON_WM_PAINT()
   ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticExtended message handlers

BOOL CStaticExtended::OnEraseBkgnd(CDC* pDC) 
{
	// we're doing our own background drawing, thank you
   return TRUE; 
}

void CStaticExtended::OnPaint() 
{
   int               prevBkMode;
   COLORREF          prevTextColor;
   CRect             controlRect, crectFill;
   HPALETTE          hOldPalette;

   CPaintDC dc(this); // device context for painting

   GetClientRect(&controlRect);

   if(m_hPalette)
   {
      hOldPalette = ::SelectPalette(dc.GetSafeHdc(), m_hPalette, FALSE);
      dc.RealizePalette();
   }

	CBrush brushFill;
	if (m_rgbBkColor != TRANSPARENT_COLOR)
	{
		brushFill.CreateSolidBrush(m_rgbBkColor);
		dc.FillRect(&controlRect, &brushFill);
		brushFill.DeleteObject();
	}

   prevBkMode = dc.SetBkMode(TRANSPARENT);
   prevTextColor = dc.SetTextColor(m_rgbTextColor);

   CFont* pOldFont = NULL;
   CFont* pFont = GetFont();
   if (pFont != NULL)
   {
      pOldFont = dc.SelectObject(pFont);
   }

   if(!m_bInitialized)
   {
      GetWindowText(m_strWindowText);
      m_bInitialized = TRUE;
   }
   dc.DrawText(m_strWindowText, -1, &controlRect, m_nFormat);

   if (pOldFont != NULL)
   {
      dc.SelectObject(pOldFont);
   }

   dc.SetTextColor(prevTextColor);
   dc.SetBkMode(prevBkMode);
   
   if(m_hPalette)
      ::SelectPalette(dc.GetSafeHdc(), hOldPalette, FALSE);

   // Do not call CStatic::OnPaint() for painting messages
}

LRESULT CStaticExtended::OnSetText(WPARAM wParam, LPARAM lParam)
   {
      m_strWindowText = (LPCSTR) lParam;
      m_bInitialized = TRUE;
	  if(m_rgbBkColor == TRANSPARENT_COLOR )
	  {

		  CRect rcWindow;
		  GetWindowRect(&rcWindow);
		  CWnd* pParent = GetParent();
		  if(pParent)
		  {
			  pParent->ScreenToClient(&rcWindow);
			  pParent->InvalidateRect(rcWindow);
		  }
	  }

	  Invalidate();
      return 0;
   }
