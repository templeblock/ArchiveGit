/*
// $Workfile: BTNWND.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/BTNWND.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:18   Fred
// Initial revision.
// 
//    Rev 1.11   03 Feb 1997 15:55:02   Jay
// Custom tool buttons
// 
//    Rev 1.10   15 Jan 1997 10:49:18   Jay
// New toolbars
// 
//    Rev 1.9   03 Aug 1996 16:42:52   Fred
//  
// 
//    Rev 1.8   02 Aug 1996 13:10:50   Fred
// More 16-bit changes
// 
//    Rev 1.7   01 Aug 1996 09:22:30   Fred
// More 16-bit changes
// 
//    Rev 1.6   31 Jul 1996 11:56:20   Fred
// 16-bit compatibility
// 
//    Rev 1.5   28 Jul 1996 13:33:32   Fred
// Disabling button pops it up
// 
//    Rev 1.4   24 Jul 1996 18:25:14   Jay
// PaintCustomize() call
// 
//    Rev 1.3   23 Jul 1996 16:51:56   Fred
// Redraws on WM_ENABLE
// 
//    Rev 1.2   23 Jul 1996 09:05:20   Fred
// Double clicks turned into single clicks
// 
//    Rev 1.1   23 Jul 1996 08:04:06   Fred
// Added size to bitmap flag
// 
//    Rev 1.0   11 Jul 1996 17:46:14   Jay
//  
*/ 

#include "stdafx.h"
#include "btnwnd.h"
#include "util.h"

#ifndef WIN32
#ifndef COLOR_BTNHILIGHT
#define  COLOR_BTNHILIGHT  COLOR_BTNHIGHLIGHT
#endif
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CButtonWnd

CButtonWnd::CButtonWnd()
{
   m_pBitmap = NULL;
   m_nBitmapWidth = 0;
   m_nBitmapHeight = 0;
   m_fSelect = FALSE;
   m_fOwnsBitmap = TRUE;
}

CButtonWnd::~CButtonWnd()
{
   DeleteBitmap();
   DestroyWindow();
}

// More advanced constructor.
CButtonWnd::CButtonWnd(LPCSTR pTitle, CWnd* pParent, UINT uID, UINT uBitmap)
{
   m_nBitmapWidth = 0;
   m_nBitmapHeight = 0;
   m_pBitmap = NULL;
   m_fSelect = FALSE;
   m_fOwnsBitmap = TRUE;

   Create(pTitle, pParent, uID, uBitmap);
}

BOOL CButtonWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext /*=NULL*/)
{
   return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
  
// More advanced create.
BOOL CButtonWnd::Create(LPCSTR pTitle, CWnd* pParent, UINT uID, UINT uBitmap)
{
   if (!CWnd::Create(NULL, pTitle, WS_CHILD | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), pParent, uID))
   {
      return FALSE;
   }

   if (uBitmap != 0)
   {
      CBitmap* pBitmap = NULL;
      TRY
      {
         pBitmap = new CBitmap;
         pBitmap->LoadBitmap(uBitmap);

         SetBitmap(pBitmap);
      }
      CATCH_ALL(e)
      {
         delete pBitmap;
         return FALSE;
      }
      END_CATCH_ALL
   }
   return TRUE;
}

void CButtonWnd::DeleteBitmap(void)
{
   if (m_fOwnsBitmap)
   {
      delete m_pBitmap;
      m_pBitmap = NULL;
   }
}

BEGIN_MESSAGE_MAP(CButtonWnd, CWnd)
   //{{AFX_MSG_MAP(CButtonWnd)
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_MOUSEMOVE()
   ON_WM_PAINT()
   ON_WM_ENABLE()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CButtonWnd::Initialize(UINT nID, CWnd* pParent)
{
   CWnd* pControl = pParent->GetDlgItem(nID);
   if (pControl != NULL)
   {
      CRect crControl;
      pControl->GetWindowRect(crControl);
      pParent->ScreenToClient(crControl);
      CWnd::Create(NULL, "", pControl->GetStyle(), crControl, pParent, nID, NULL);
      SetWindowPos(pControl, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
      pControl->DestroyWindow();
   }
}

void CButtonWnd::SetBitmap(CBitmap* pBitmap, BOOL fOwnsBitmap /*=TRUE*/, BOOL fSizeToBitmap /*=TRUE*/)
{
   // Get rid of any existing bitmap.
   DeleteBitmap();

   // Do we own the bitmap?
   m_fOwnsBitmap = fOwnsBitmap;

   // Assign the bitmap.
   if ((m_pBitmap = pBitmap) != NULL)
   {
      BITMAP Bitmap;
      if (Util::GetBitmap(m_pBitmap, &Bitmap))
      {
         // Remember bitmap dimensions
         m_nBitmapWidth = Bitmap.bmWidth;
         m_nBitmapHeight = Bitmap.bmHeight;

         if (fSizeToBitmap)
         {
            // Size ourselves to the bitmap with room for border.
            SetWindowPos(NULL, 0, 0, 2+m_nBitmapWidth+2, 2+m_nBitmapHeight+2, SWP_NOZORDER|SWP_NOMOVE);
         }
      }
   }
}

void CButtonWnd::SetSelect(BOOL fSelect /*=TRUE*/)
{
   if (m_fSelect != fSelect)
   {
      // Selection changed.
      m_fSelect = fSelect;
      Invalidate();
   }
}

/////////////////////////////////////////////////////////////////////////////
// CButtonWnd message handlers

void CButtonWnd::OnPaint() 
{
   CPaintDC dc(this);

   // Upack the state of the button.
   BOOL fSelected = GetSelect();
   BOOL fFocused = FALSE;
   BOOL fDisabled = !IsWindowEnabled();
   BOOL fDefault = FALSE;

   // Create a bitmap which will be used to draw the button image.
   // When the bitmap is complete, it will be drawn onto the button.
   CRect crButton;
   GetClientRect(crButton);

   CBitmap bmButton;
   bmButton.CreateCompatibleBitmap(&dc, crButton.Width(), crButton.Height());

   CDC dcButton;
   dcButton.CreateCompatibleDC(&dc);

   dcButton.SelectObject(&bmButton);

   // Initialize the button bitmap to the button face color.
   Util::FillRectangleWithColor(dcButton, crButton, GetSysColor(COLOR_BTNFACE));

   // Compute the area available for content.
   CRect crContent(crButton);

   // Compute the bitmap dimensions and its nominal position.
   CRect crBitmap;
   crBitmap.SetRectEmpty();

   if (!crContent.IsRectEmpty())
   {
      if (m_pBitmap != NULL)
      {
         crBitmap.SetRect(0, 0, m_nBitmapWidth, m_nBitmapHeight);
         crBitmap.OffsetRect((crContent.Width()-crBitmap.Width())/2, (crContent.Height()-crBitmap.Height())/2);
      }
   }

   // If the button is selected, offset the bitmap.
   if (fSelected)
   {
      crBitmap.OffsetRect(1, 1);
   }

   // Draw the bitmap onto the button image at the computed position.
   if (!crBitmap.IsRectEmpty() && m_pBitmap != NULL)
   {
      // Draw the bitmap onto the button.
      CDC dcBitmap;
      dcBitmap.CreateCompatibleDC(&dc);

      dcBitmap.SelectObject(m_pBitmap);
      dcButton.BitBlt(crBitmap.left, crBitmap.top, crBitmap.Width(), crBitmap.Height(), &dcBitmap, 0, 0, SRCCOPY);
   }

   // If the button is disabled, indicate this by embossing the bitmap.
   if (fDisabled)
   {
      Util::Emboss(dcButton, crButton, GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHILIGHT));
   }

   // Draw the button in its current state.
   if (fSelected)
   {
      // Button is selected, draw a dark border.
      CRect crDraw(crButton);
      Util::DrawBorderWithColor(dcButton, crDraw, GetSysColor(COLOR_BTNSHADOW), 1);
   }
   else
   {
      CRect crDraw(crButton);

      // If the button is focused or the default button, draw a
      // black border around it. The rest of the image moves in
      // to make room.
      if (fFocused || fDefault)
      {
         Util::DrawBorderWithColor(dcButton, crDraw, GetSysColor(COLOR_BTNTEXT), 1);
         crDraw.InflateRect(-1, -1);
      }

      // Draw the raised 3D border:
      //
      //    L-----------LB
      //    |W---------WD|
      //    ||          ||
      //    ||          ||
      //    |W          ||
      //    LD----------D|
      //    B------------B


      CRect crSide;

      // L horizontal
      crSide.SetRect(crDraw.left, crDraw.top, crDraw.right-1, crDraw.top+1);
      Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNFACE));

      // L vertical
      crSide.SetRect(crDraw.left, crDraw.top+1, crDraw.left+1, crDraw.bottom-1);
      Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNFACE));

      // W horizontal
      crSide.SetRect(crDraw.left+1, crDraw.top+1, crDraw.right-2, crDraw.top+2);
      Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNHILIGHT));

      // W vertical
      crSide.SetRect(crDraw.left+1, crDraw.top+2, crDraw.left+2, crDraw.bottom-2);
      Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNHILIGHT));

      // B horizontal
      crSide.SetRect(crDraw.left, crDraw.bottom-1, crDraw.right, crDraw.bottom);
      Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNTEXT));

      // B vertical
      crSide.SetRect(crDraw.right-1, crDraw.top, crDraw.right, crDraw.bottom-1);
      Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNTEXT));

      // D horizontal
      crSide.SetRect(crDraw.left+1, crDraw.bottom-2, crDraw.right-1, crDraw.bottom-1);
      Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNSHADOW));

      // D vertical
      crSide.SetRect(crDraw.right-2, crDraw.top+1, crDraw.right-1, crDraw.bottom-2);
      Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNSHADOW));
   }

   // If the button is focused, draw the focus rectangle.
   if (fFocused)
   {
      CRect crDraw(crButton);
      crDraw.InflateRect(-4, -4);
      dcButton.DrawFocusRect(crDraw);
   }

   PaintCustomize(dcButton, crButton);

   // Copy the bitmap onto the button.
   dc.BitBlt(crButton.left, crButton.top, crButton.Width(), crButton.Height(), &dcButton, 0, 0, SRCCOPY);

   dcButton.DeleteDC();
   bmButton.DeleteObject();
}

void CButtonWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
   // Set the state to selected and capture the mouse until the button is released.
   SetSelect(TRUE);
   SetCapture();
}
 
void CButtonWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
   // Remember the select state before losing it.
   BOOL fSelect = m_fSelect;

   // The user finally let go, pop the button back up.
   SetSelect(FALSE);
   ReleaseCapture();

   if (fSelect)
   {
      // Let our owner know that the button was pressed.
      CWnd* pOwner = GetOwner();
      if (pOwner != NULL)
      {
         Util::SendWmCommand(pOwner, GetDlgCtrlID(), this, 0);
      }
   }
}

void CButtonWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   // Default behavior turns double clicks into single clicks.
   SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
}

void CButtonWnd::OnMouseMove(UINT nFlags, CPoint point)
{
   // The user is moving the mouse around willy-nilly. If the mouse is
   // moved off the button, pop it up until it's moved back on.
   // 16-bit: TOOLBARX sets the focus to the child window it's doing
   // tool-tips for. That confused this code; now we check for the button
   // being down as well.

   if (GetCapture() == this && (nFlags & MK_LBUTTON))
   {
      CRect crClient;
      GetClientRect(crClient);
      SetSelect(crClient.PtInRect(point));
   }
}

void CButtonWnd::OnEnable(BOOL bEnable) 
{
   // WM_ENABLE is sent AFTER the WS_DISABLED bit is set.
   if (bEnable == IsWindowEnabled())
   {
      // If button is being disabled, pop it back out.
      if (!bEnable)
      {
         SetSelect(FALSE);
         ReleaseCapture();
      }

      Default();
      Invalidate();
   }
}

void CButtonWnd::PaintCustomize(CDC& dcButton, CRect crButton)
{
   // Nothing by default.
}
