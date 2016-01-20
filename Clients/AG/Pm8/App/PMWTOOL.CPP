/*
// $Header: /PM8/App/PMWTOOL.CPP 1     3/03/99 6:09p Gbeddow $
//
// pmwtool.cpp : implementation file
//
// $Log: /PM8/App/PMWTOOL.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 29    1/20/99 10:28a Mwilson
// moved toolbars into seperate DLL
// 
// 28    1/14/99 3:42p Jfleischhauer
// replaced ID_PHOTO_WORKSHOP with ID_PHOTO_WORKSHOP_BUTTON
// 
// 27    1/13/99 1:23p Jfleischhauer
// replaced PhotoEnhancer with Photo Workshop
// 
// 26    11/23/98 1:27p Mwilson
// Added creataparty configuration
// 
// 25    11/19/98 3:17p Johno
// Move online art button to file toolbar
// 
// 24    11/19/98 11:41a Johno
// Desktop online art button
// 
// 23    10/13/98 4:45p Mwilson
// removed tutorial button
// 
// 21    10/12/98 1:26p Mwilson
// added craft deluxe configuration
// 
// 20    9/25/98 11:07a Mwilson
// added support for instruction page
// 
// 19    8/29/98 8:22a Dennis
// No border around buttons for AG
// 
// 18    7/12/98 5:33p Jayn
// Changed how Serif DrawPlus tool bar button worked (add vs. edit)
// 
// 17    6/21/98 11:56a Cboggio
// Allowed for Serif Draw and YYWB to be removed for localization
// 
// 16    6/20/98 11:42a Cboggio
// Changes to allow some toolbar buttons to be completely removed for
// certain localized configurations
// 
// 15    6/11/98 9:36a Dennis
// Added AG Specific Toolbars
// 
// 14    5/26/98 2:06p Dennis
// Added unique edit toolbar for American Greetings which is used based on
// Configuration.
// 
// 13    5/07/98 3:48p Fredf
// New preview bitmaps and toolbar buttons.
// 
// 12    4/30/98 7:56p Jayn
// Changes for Border Plus and Add Picture From Disk.
// 
// 11    4/23/98 9:33a Fredf
// Added help button to file toolbar.
// 
// 10    4/10/98 4:56p Rlovejoy
// Changes for new hub implementation.
// 
// 9     4/07/98 7:18p Fredf
// Border and drawing object toolbar buttons now mean "Add" instead of
// "Edit".
// 
// 8     4/07/98 6:51p Fredf
// Draws special toolbar separator line in a way that works with the old
// COMCTL32.DLL
// 
// 7     3/24/98 2:14p Rlovejoy
// Changes for MRU font combo box.
// 
// 6     3/23/98 11:55a Fredf
// Special divider line for Edit toolbars.
// 
// 5     3/21/98 1:18p Fredf
// New menus and toolbar names.
// 
// 4     3/17/98 7:10p Fredf
// Removed unused code.
// 
// 3     3/17/98 2:43p Fredf
// New workspace toolbars.
// 
// 2     1/19/98 1:21p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
//    Rev 1.0   14 Aug 1997 15:24:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:56   Fred
// Initial revision.
// 
//    Rev 1.15   15 Jul 1997 16:20:42   Jay
// Some fixes to make 16-bit look better.
// Title font changed to Normal.
// Padding added to end of combo boxes.
// 
//    Rev 1.14   09 May 1997 15:40:28   Jay
// New global tools
// 
//    Rev 1.13   09 Apr 1997 17:14:08   Jay
// PhotoEnhancer
// 
//    Rev 1.12   03 Apr 1997 14:31:24   Jay
// Resource cleanup
// 
//    Rev 1.11   03 Apr 1997 09:31:28   Fred
// More text cleanup
// 
//    Rev 1.10   21 Mar 1997 16:24:10   Jay
// Added stretching to text bar.
// 
//    Rev 1.9   21 Mar 1997 15:56:48   Jay
// Changed bar title text color to black.
// 
//    Rev 1.8   17 Mar 1997 09:40:08   Jay
// Popup warp field palette
// 
//    Rev 1.7   07 Feb 1997 09:17:46   Jay
// Draws line between add and modify bars.
// 
//    Rev 1.6   06 Feb 1997 09:13:10   Jay
// Rulers!
// 
//    Rev 1.5   03 Feb 1997 15:55:10   Jay
// Custom tool buttons
// 
//    Rev 1.4   31 Jan 1997 15:51:10   Jay
// Some new toolbar bitmaps.
// 
//    Rev 1.3   29 Jan 1997 17:24:34   Jay
// Next and previous object support
// 
//    Rev 1.2   23 Jan 1997 11:05:36   Jay
// New popup palettes
// 
//    Rev 1.1   17 Jan 1997 09:17:28   Jay
// New status panes.
// 
//    Rev 1.0   15 Jan 1997 10:49:30   Jay
// New toolbars
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "mainfrm.h"
#include "pmwtool.h"
#include "typeface.h"
#include "util.h"
#include "pmwcfg.h"
#include "rmagcom.h"


#include <..\src\afximpl.h>

#include <afxpriv.h>

#define ID_ZERO_SEPARATOR  (UINT)-4
#define ID_WIDE_SEPARATOR  (UINT)-5

extern CPalette *pOurPal;



/////////////////////////////////////////////////////////////////////////////
// CPmwToolBar

#define ALIGN_ANY      0xF000
#define ALIGN_LEFT     0x1000
#define ALIGN_TOP      0x2000
#define ALIGN_RIGHT    0x4000
#define ALIGN_BOTTOM   0x8000
#define BORDER_ANY     0x0F00
#define BORDER_LEFT    0x0100
#define BORDER_TOP     0x0200
#define BORDER_RIGHT   0x0400
#define BORDER_BOTTOM  0x0800

CPmwToolBar::CPmwToolBar()
{
   // Set tool bar defaults.
   m_uID = 0;
   m_fInDialog = FALSE;
}

BOOL CPmwToolBar::Create(CWnd* pParentWnd, DWORD dwStyle)
{
   m_cxLeftBorder = m_cyTopBorder = __min(m_cxLeftBorder, m_cyTopBorder);

   if (!CCustomToolBar::Create(pParentWnd, dwStyle, m_uID))
   {
      return FALSE;
   }

   SetBarStyle(GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);

   return TRUE;
}

BEGIN_MESSAGE_MAP(CPmwToolBar, CCustomToolBar)
   //{{AFX_MSG_MAP(CPmwToolBar)
   //}}AFX_MSG_MAP
   ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

LRESULT CPmwToolBar::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
	// Pre-process this to see if any of our own controls are hit.
   CWnd* pWnd;

   if ((pWnd = ChildWindowFromPoint(CPoint(lParam))) != NULL
         && pWnd != this)
   {
      UINT nID = _AfxGetDlgCtrlID(pWnd->GetSafeHwnd());
      od("child id is %d\r\n", nID);
      if (nID != 0 && nID != (UINT)-1)
      {
         return nID;
      }
   }

	// Call default processing to handle the button bitmaps.
   return CCustomToolBar::OnHelpHitTest(wParam, lParam);
}

LRESULT CPmwToolBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
   if (m_fInDialog)
   {
      if (GetStyle() & WS_VISIBLE)        // ignore if child is invisible
      {
         CFrameWnd* pTarget = (CFrameWnd*)GetOwner();
         if (pTarget != NULL)
            OnUpdateCmdUI(pTarget, (BOOL)wParam);
      }
      return 0L;
   }

   // Call the default.
   return CCustomToolBar::OnIdleUpdateCmdUI(wParam, lParam);
}

/*
// Setup a combo box on the format bar.
*/

void CPmwToolBar::SetupCombo(int nIndex, UINT uId, int nWidth, CComboBox& Combo)
{
   nWidth += GetSystemMetrics(SM_CXVSCROLL);
   SetButtonInfo(nIndex, uId, TBBS_SEPARATOR, nWidth);
   CRect rect;

   GetItemRect(nIndex, &rect);
   rect.top = 3;
   rect.bottom = rect.top + DropHeight;

   Combo.SetWindowPos(NULL,
                      rect.left,
                      rect.top,
                      rect.Width(),
                      rect.Height(),
                      SWP_NOACTIVATE
                        | SWP_NOZORDER
                        | SWP_SHOWWINDOW
                        | SWP_NOREDRAW);
}

void CPmwToolBar::HideCombo(CComboBox& Combo)
{
   Combo.SetWindowPos(NULL,
                      -2,
                      -2,
                      1,
                      1,
                      SWP_NOACTIVATE
                        | SWP_HIDEWINDOW
                        | SWP_NOZORDER
                        | SWP_NOREDRAW);
}

void CPmwToolBar::DrawBorder(CDC& dc, CRect& crBorder)
{
	CCustomToolBar::DrawBorder(dc, crBorder);
}

void CPmwToolBar::DrawBackground(CDC& dc, const CRect& crBackground, const CSize& czOffset)
{
	CCustomToolBar::DrawBackground(dc, crBackground, czOffset);

   // American Greetings doesn't draw a border around buttons
   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
      return;

	for (int nButton = 0; nButton < GetToolBarCtrl().GetButtonCount(); nButton++)
	{
		CRect crButton;
		GetItemRect(nButton, crButton);

		crButton -= czOffset;

		// Check if the button is in the update region. If not, then
		// there's nothing to draw.
		CRect crButtonWithBorders(crButton);
		crButtonWithBorders.InflateRect(1,1);
		if (crButtonWithBorders.IntersectRect(crButtonWithBorders, crBackground))
		{
			// Get the button information.
			UINT uButtonID;
			UINT uButtonStyleState;
			int nButtonBitmap;
			GetButtonInfo(nButton, uButtonID, uButtonStyleState, nButtonBitmap);
			WORD wButtonStyle = LOWORD(uButtonStyleState);
			WORD wButtonState = HIWORD(uButtonStyleState);

			if ((wButtonState & TBSTATE_HIDDEN) == 0)
			{
				if (wButtonStyle != TBSTYLE_SEP)
				{
					BOOL fStartOfGroup = IsStartOfGroup(nButton);
					BOOL fEndOfGroup = IsEndOfGroup(nButton);

					CRect crRect;

					// Draw top border.
					crRect.SetRect(
						crButton.left,
						crButton.top-1,
						crButton.right,
						crButton.top);

               dc.FillSolidRect(crRect, RGB(0,0,0));

					// Draw bottom border.
					crRect.SetRect(
						crButton.left,
						crButton.bottom,
						crButton.right,
						crButton.bottom+1);

					if (fStartOfGroup)
					{
						crRect.left--;
					}

					dc.FillSolidRect(crRect, RGB(255,255,255));

					// Draw left border.
					if (fStartOfGroup)
					{
						crRect.SetRect(
							crButton.left-1,
							crButton.top-1,
							crButton.left,
							crButton.bottom);

						dc.FillSolidRect(crRect, RGB(0,0,0));
					}

					// Draw right border.
					if (fEndOfGroup)
					{
						crRect.SetRect(
							crButton.right,
							crButton.top-1,
							crButton.right+1,
							crButton.bottom+1);

						dc.FillSolidRect(crRect, RGB(255,255,255));
					}
				}
			}
		}
	}
}

void CPmwToolBar::DrawButton(CDC& dc, const CRect& crButton, int nButtonIndex, UINT uButtonID, WORD wButtonStyle, WORD wButtonState)
{
	CCustomToolBar::DrawButton(dc, crButton, nButtonIndex, uButtonID, wButtonStyle, wButtonState);
}

void CPmwToolBar::DrawSeparator(CDC& dc, const CRect& crButton, int nButtonIndex, UINT uButtonID, WORD wButtonStyle, WORD wButtonState)
{
	CCustomToolBar::DrawSeparator(dc, crButton, nButtonIndex, uButtonID, wButtonStyle, wButtonState);
}

int CPmwToolBar::IsStartOfGroup(int nButtonIndex)
{
	int nPreviousButtonIndex = nButtonIndex-1;

	// The current button is at the start of a group if it's the first button.
	if (nPreviousButtonIndex < 0)
	{
		return TRUE;
	}

	// Get the previous button information.
	UINT uPreviousButtonID;
	UINT uPreviousButtonStyleState;
	int nPreviousButtonBitmap;
	GetButtonInfo(nPreviousButtonIndex, uPreviousButtonID, uPreviousButtonStyleState, nPreviousButtonBitmap);
	WORD wPreviousButtonStyle = LOWORD(uPreviousButtonStyleState);
	WORD wPreviousButtonState = HIWORD(uPreviousButtonStyleState);

	// The current button is at the start of a group if the previous button
	// is a wrap point or a separator.
	if ((wPreviousButtonStyle == TBSTYLE_SEP)
	 || ((wPreviousButtonState & TBSTATE_WRAP) != 0))
	{
		return TRUE;
	}

	return FALSE;
}

int CPmwToolBar::IsEndOfGroup(int nButtonIndex)
{
	int nNextButtonIndex = nButtonIndex+1;

	// The current button is at the end of a group if it's the last button.
	if (nNextButtonIndex >= GetToolBarCtrl().GetButtonCount())
	{
		return TRUE;
	}

	// Get the current button information.
	UINT uButtonID;
	UINT uButtonStyleState;
	int nButtonBitmap;
	GetButtonInfo(nButtonIndex, uButtonID, uButtonStyleState, nButtonBitmap);
	WORD wButtonStyle = LOWORD(uButtonStyleState);
	WORD wButtonState = HIWORD(uButtonStyleState);

	// The current button is at the end of a group if it's a wrap point.
	if ((wButtonState & TBSTATE_WRAP) != 0)
	{
		return TRUE;
	}

	// Get the next button information.
	UINT uNextButtonID;
	UINT uNextButtonStyleState;
	int nNextButtonBitmap;
	GetButtonInfo(nNextButtonIndex, uNextButtonID, uNextButtonStyleState, nNextButtonBitmap);
	WORD wNextButtonStyle = LOWORD(uNextButtonStyleState);
	WORD wNextButtonState = HIWORD(uNextButtonStyleState);

	// The current button is at the end of a group if the next button is a separator.
	if (wNextButtonStyle == TBSTYLE_SEP)
	{
		return TRUE;
	}

	return FALSE;
}
