/*
// $Workfile: STATBAR.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/STATBAR.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:34   Fred
// Initial revision.
// 
//    Rev 1.9   21 Mar 1997 11:10:44   Jay
// Print preview changes
// 
//    Rev 1.8   18 Mar 1997 13:23:20   Jay
// Preview stuff
// 
//    Rev 1.7   11 Mar 1997 16:13:58   Jay
// Extensions to the workspace
// 
//    Rev 1.6   06 Feb 1997 09:12:20   Jay
// WM_COMMANDs on mouse click.
// 
//    Rev 1.5   17 Jan 1997 09:17:36   Jay
// New status panes.
// 
//    Rev 1.4   16 Jan 1997 13:02:50   Jay
// New status stuff. More cleanup.
// 
//    Rev 1.3   02 Aug 1996 13:11:52   Fred
// More 16-bit changes
// 
//    Rev 1.2   31 Jul 1996 11:56:22   Fred
// 16-bit compatibility
// 
//    Rev 1.1   23 Jul 1996 16:53:38   Fred
//  
// 
//    Rev 1.0   23 Jul 1996 08:16:52   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "mainfrm.h"
#include "statbar.h"
#include "util.h"

#include <afxpriv.h>

#ifndef WIN32
#include <auxdata.h>    // for afxData
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static UINT BASED_CODE uIndicatorIDs[] =
{
   ID_SEPARATOR,        // status line indicator
   ID_INDICATOR_POSITION,
   ID_INDICATOR_SIZE,
   ID_INDICATOR_ROTATE
};

const int nFirstPane = 1;			// First pane we care about.
const int nPanes = sizeof(uIndicatorIDs)/sizeof(uIndicatorIDs[0]);

//////////////////////////////////////////////////////////////////////////
// CPmwStatusBar

CPmwStatusBar::CPmwStatusBar()
{
#ifndef WIN32
   if (!afxData.bDBCS)
   {
      // We want to create our own (smaller) font.

      // Delete any created font.
      if (afxData.hStatusFont != NULL)
      {
         DeleteObject(afxData.hStatusFont);
         afxData.hStatusFont = NULL;
      }

      // load status bar font
      LOGFONT logfont;
      memset(&logfont, 0, sizeof(logfont));
      {
         // 8 point height Sans Serif font
         logfont.lfHeight = -MulDiv(8, afxData.cyPixelsPerInch, 72);
         logfont.lfWeight = FW_NORMAL;
         logfont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
         static char BASED_CODE szFaceName[] = "MS Sans Serif";
         lstrcpy(logfont.lfFaceName, szFaceName);
         afxData.hStatusFont = ::CreateFontIndirect(&logfont);
      }
      if (afxData.hStatusFont == NULL)
      {
         TRACE0("Warning: Using system font for status font\n");
         afxData.hStatusFont = (HFONT)::GetStockObject(SYSTEM_FONT);
      }
   }
#endif
}

CPmwStatusBar::~CPmwStatusBar()
{
}

BOOL CPmwStatusBar::Create(CWnd* pParentWnd)
{
   if (!INHERITED::Create(pParentWnd))
   {
      return FALSE;
   }

   LoadIndicator(0, IDB_POSITION_INDICATOR);
   LoadIndicator(1, IDB_SIZE_INDICATOR);
   LoadIndicator(2, IDB_ROTATE_INDICATOR);

	SetEditPanes();

   return TRUE;
}

void CPmwStatusBar::SetEditPanes(void)
{
	SetIndicators(uIndicatorIDs, sizeof(uIndicatorIDs)/sizeof(UINT));

   // Set up the panes.
   UINT uID;
   UINT uStyle;
   int nWidth;

   GetPaneInfo(0, uID, uStyle, nWidth);
   SetPaneInfo(0, uID, uStyle, 1);

#ifdef WIN32
   UINT uAddedStyle = SBT_OWNERDRAW;
#else
   UINT uAddedStyle = 0;
#endif
   GetPaneInfo(1, uID, uStyle, nWidth);
   SetPaneInfo(1, uID, (uStyle|uAddedStyle), nWidth + m_Indicators[0].m_Size.cx + 2);

   GetPaneInfo(2, uID, uStyle, nWidth);
   SetPaneInfo(2, uID, (uStyle|uAddedStyle), nWidth + m_Indicators[1].m_Size.cx + 2);

   GetPaneInfo(3, uID, uStyle, nWidth);
   SetPaneInfo(3, uID, (uStyle|uAddedStyle), nWidth + m_Indicators[2].m_Size.cx + 2);
}

void CPmwStatusBar::SetPreviewPanes(void)
{
	SetIndicators(uIndicatorIDs, 1);

   // Set up the panes.
   UINT uID;
   UINT uStyle;
   int nWidth;

   GetPaneInfo(0, uID, uStyle, nWidth);
   SetPaneInfo(0, uID, uStyle, 1);

#if 0
   SetPaneInfo(1, 0, 0, 0);
   SetPaneInfo(2, 0, 0, 0);
   SetPaneInfo(3, 0, 0, 0);
#endif
}

BEGIN_MESSAGE_MAP(CPmwStatusBar, CStatusBar)
   //{{AFX_MSG_MAP(CPmwStatusBar)
   ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
   ON_WM_LBUTTONDOWN()
   ON_WM_RBUTTONDOWN()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

#ifdef WIN32
BOOL CPmwStatusBar::PreCreateWindow(CREATESTRUCT& cs)
{
   // The size grip does really bad things if the status bar doesn't
   // extend all the way to the right. We don't care that much about
   // it, so we just get rid of it.

   cs.style &= ~SBARS_SIZEGRIP;
   return INHERITED::PreCreateWindow(cs);
}

CSize CPmwStatusBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
   CSize size = INHERITED::CalcFixedLayout(bStretch, bHorz);

   CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
   if (pMainFrame != NULL)
   {
      CMlsScrollGadget& ZoomGadget = pMainFrame->ZoomGadget();
      CRect crZoomGadget = ZoomGadget.GetRectangle();
      size.cy = crZoomGadget.Height();
   }

   return size;
}

#endif

/////////////////////////////////////////////////////////////////////////////
// CPmwStatusBar message handlers

LRESULT CPmwStatusBar::OnSizeParent(WPARAM wParam, LPARAM lParam)
{
   CRect crSaveRect;
   
   AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;
   if (lpLayout != NULL)
   {
      crSaveRect = lpLayout->rect;
      CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
      if (pMainFrame != NULL)
      {
         CMlsScrollGadget& ZoomGadget = pMainFrame->ZoomGadget();
         CRect crZoomGadget = ZoomGadget.GetRectangle();
         lpLayout->rect.left = crZoomGadget.right;
         lpLayout->rect.bottom = crZoomGadget.bottom;
      }
   }

#ifndef WIN32
   CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
   if (pMainFrame != NULL)
   {
      CMlsScrollGadget& ZoomGadget = pMainFrame->ZoomGadget();
      CRect crZoomGadget = ZoomGadget.GetRectangle();
      m_sizeFixedLayout.cy = crZoomGadget.Height();
   }
#endif

   LRESULT lResult = INHERITED::OnSizeParent(wParam, lParam);

   if (lpLayout != NULL)
   {
      lpLayout->rect.left = crSaveRect.left;

      CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
      if (pMainFrame != NULL)
      {
         CMlsGadget& SeparatorGadget = pMainFrame->SeparatorGadget();
         CRect crSeparatorGadget = SeparatorGadget.GetRectangle();
         lpLayout->rect.left = crSeparatorGadget.left;
      }
   }

   return lResult;
}

void CPmwStatusBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
   CFont* pFont = GetFont();
   CRect rcItem = lpDrawItemStruct->rcItem;

   HDC hDC = lpDrawItemStruct->hDC;

   CString csText;
   GetPaneText(lpDrawItemStruct->itemID, csText);

   int nBitmapIndex = lpDrawItemStruct->itemID-1;
   if (m_Indicators[nBitmapIndex].m_fValid)
   {
      // We have a bitmap to draw.
      CDC dcBitmap;
      if (dcBitmap.CreateCompatibleDC(CDC::FromHandle(hDC)))
      {
         CBitmap* pOldBitmap = dcBitmap.SelectObject(&m_Indicators[nBitmapIndex].m_Bitmap);
         if (pOldBitmap != NULL)
         {
            CRect r;

            CSize Size = m_Indicators[nBitmapIndex].m_Size;
            r.left = rcItem.left + 2;
            r.right = r.left + Size.cx;
            r.top = (lpDrawItemStruct->rcItem.top + lpDrawItemStruct->rcItem.bottom - Size.cy)/2;
            r.bottom = r.top + Size.cy;
            ::BitBlt(hDC,
                     r.left,
                     r.top,
                     r.Width(),
                     r.Height(),
                     dcBitmap.GetSafeHdc(),
                     0,
                     0,
                     SRCCOPY);
            dcBitmap.SelectObject(pOldBitmap);

            rcItem.left = r.right + 2;
         }
         dcBitmap.DeleteDC();
      }
   }

   HFONT hOldFont = (HFONT)::SelectObject(hDC, pFont->GetSafeHandle());
   if (hOldFont != NULL)
   {
      int nOldMode = SetBkMode(hDC, TRANSPARENT);
      ::DrawText(hDC,
                 csText,
                 csText.GetLength(),
                 &rcItem,
                 DT_CENTER | DT_SINGLELINE | DT_VCENTER);
      SetBkMode(hDC, nOldMode);
      ::SelectObject(hDC, hOldFont);
   }
}

BOOL CPmwStatusBar::LoadIndicator(int nIndex, UINT uID)
{
   m_Indicators[nIndex].m_fValid = FALSE;

   if (m_Indicators[nIndex].m_Bitmap.LoadBitmap(uID))
   {
      BITMAP Bitmap;
      if (Util::GetBitmap(&m_Indicators[nIndex].m_Bitmap, &Bitmap))
      {
         // Remember bitmap dimensions
         m_Indicators[nIndex].m_Size.cx = Bitmap.bmWidth;
         m_Indicators[nIndex].m_Size.cy = Bitmap.bmHeight;
         m_Indicators[nIndex].m_fValid = TRUE;
      }
   }
   return m_Indicators[nIndex].m_fValid;
}

#ifndef WIN32
void CPmwStatusBar::DoPaint(CDC* pDC)
{
   INHERITED::DoPaint(pDC);

	if (m_nCount > 1)
	{
		DRAWITEMSTRUCT dis;

		memset(&dis, 0, sizeof(dis));

		dis.hwndItem = GetSafeHwnd();
		dis.hDC = pDC->m_hDC;

		for (dis.itemID = 1; dis.itemID <= 3; dis.itemID++)
		{
			UINT uStyle;
			int nWidth;

			// Get the pane info to get the ID.
			GetPaneInfo(dis.itemID, dis.CtlID, uStyle, nWidth);

			// Get the item rect.
			GetItemRect(dis.itemID, &dis.rcItem);

			// If we need to draw this item, do it now.
			if (::RectVisible(pDC->m_hDC, &dis.rcItem))
			{
				DrawItem(&dis);
			}
		}
	}
}
#endif

CPmwStatusBar::CIndicator::CIndicator()
{
}

void CPmwStatusBar::OnRButtonDown(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

void CPmwStatusBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	for (int nPane = nFirstPane; nPane < nPanes; nPane++)
	{
		CRect crPane;
      // Get the item rect.
      GetItemRect(nPane, crPane);
		if (crPane.PtInRect(point))
		{
			// We hit this pane!
			Util::SendWmCommand(AfxGetMainWnd(), uIndicatorIDs[nPane], this, 0);
			break;
		}
	}
}

