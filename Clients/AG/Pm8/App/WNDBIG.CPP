/*
// $Workfile: WNDBIG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:14p $
//
*/

/*
// Revision History:
//
// $Log: /PM8/App/WNDBIG.CPP $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 2     4/07/98 9:33a Fredf
// Start of new Hub window.
// 
//    Rev 1.0   14 Aug 1997 15:27:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:41:08   Fred
// Initial revision.
// 
//    Rev 1.7   01 Jun 1997 11:53:24   johno
// Some simplifications
// 
//    Rev 1.6   23 May 1997 15:59:02   johno
// Allow creation with ID
// (needed for sound effects)
// 
// 
//    Rev 1.5   03 Apr 1997 15:39:04   johno
// Uses colordef.h
// 
//    Rev 1.4   28 Mar 1997 14:35:40   johno
// I don't remember
// 
//    Rev 1.3   11 Mar 1997 16:21:54   johno
// Tweak
// 
//    Rev 1.2   07 Mar 1997 17:35:42   johno
// Tweaks for HUB in PM
// 
// 
//    Rev 1.1   06 Mar 1997 17:04:42   johno
// Tweak
// 
//    Rev 1.0   28 Feb 1997 11:37:02   johno
// Initial revision.
*/

// WndBig.cpp : implementation file
//

#include "stdafx.h"
#include "WndBig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define  JTRACE_ON
#include "jtrace.h"
#include "colordef.h"

/////////////////////////////////////////////////////////////////////////////
// CWndBig

CWndBig::CWndBig (COLORREF cr)
   : CWnd ()
{
   Color = cr;
}

CWndBig::~CWndBig()
{
   if (IsWindow (m_hWnd) != FALSE)
      DestroyWindow ();
}


BEGIN_MESSAGE_MAP(CWndBig, CWnd)
   //{{AFX_MSG_MAP(CWndBig)
   ON_WM_ERASEBKGND()
   ON_WM_CREATE()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void
CWndBig::GetRect (CRect *pRect)
{
   CWnd* pParent;

   pRect->SetRectEmpty ();

   if ((pParent = GetParent ()) != NULL)
      pParent->GetClientRect (pRect);
}

void
CWndBig::SizeToParent (void)
{
   CRect DesiredWindowRect;

   GetRect (&DesiredWindowRect);
   
   if (DesiredWindowRect.IsRectEmpty () == 0)
      SetWindowPos (&wndTop, DesiredWindowRect.left, DesiredWindowRect.top, DesiredWindowRect.Width (), DesiredWindowRect.Height (), 0);
}

BOOL CWndBig::Create (LPCTSTR lpszClassName, LPCTSTR lpszWindowName, CWnd* pParentWnd, UINT id, CCreateContext* pContext)
{
   BOOL  b;
   DWORD Style;

   CRect Rect;

   if (pParentWnd != NULL)
      pParentWnd->GetClientRect (&Rect);   
   if (Rect.IsRectEmpty())
      Rect.SetRect (0, 0, 16, 16);

   Style = WS_POPUP | WS_VISIBLE | WS_DLGFRAME;

   b = CWnd::Create (lpszClassName, lpszWindowName, Style, Rect, pParentWnd, id, pContext);
   // Someone else will have to turn these on
   if (b != FALSE)
      ShowScrollBar (SB_BOTH, FALSE);

   BringWindowToTop ();

   return b;
}

/////////////////////////////////////////////////////////////////////////////
// CWndBig message handlers

BOOL CWndBig::OnEraseBkgnd(CDC* pDC) 
{
   // TODO: Add your message handler code here and/or call default
   
   //return CWnd::OnEraseBkgnd(pDC);
   
JTRACE ("CWndBig::OnEraseBkgnd: Erasing Background\n");   
   // Set brush to desired background color
   CBrush backBrush (Color);

   // Save the old brush
   CBrush* pOldBrush = pDC->SelectObject(&backBrush);

   // Get the current clipping boundary
   CRect rect;
   pDC->GetClipBox(&rect);

   if (rect.IsRectEmpty () == 0)
   {
JTRACE ("CWndBig::OnEraseBkgnd: Clipbox Empty\n");
      GetClientRect (&rect);
   }

   // Erase the area needed
   pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(),
      PATCOPY);

   pDC->SelectObject(pOldBrush); // Select the old brush back

   return TRUE;  // message handled
}

int CWndBig::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   return CWnd::OnCreate(lpCreateStruct);
}

