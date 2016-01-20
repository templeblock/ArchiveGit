/*
// $Header: /PM8/Toolbars/PrintShop/PMWTOOL.CPP 1     3/03/99 6:35p Gbeddow $
//
// pmwtool.cpp : implementation file
//
// $Log: /PM8/Toolbars/PrintShop/PMWTOOL.CPP $
// 
// 1     3/03/99 6:35p Gbeddow
// 
// 1     2/24/99 12:24p Ddusich
// Print Shop Toolbar DLL
// 
// 3     2/10/99 9:22a Johno
// New 3D planet tool
// 
// 2     2/09/99 4:43p Rgrenfel
// Added code to support the Made to Order and Headline toolbar buttons.
// 
// 1     1/20/99 9:56a Mwilson
// 
// 2     12/17/98 12:01p Mwilson
// 
// 1     12/11/98 11:04a Mwilson
// 
// 28    12/08/98 2:47p Mwilson
// added yywb to creataparty
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
#include "pmwtool.h"
#include "util.h"

#include "..\..\resource\resource.h"
#include "resource.h"

#include <..\src\afximpl.h>

#include <afxpriv.h>

#define ID_ZERO_SEPARATOR  (UINT)-4
#define ID_WIDE_SEPARATOR  (UINT)-5

extern CPalette *pOurPal;

/////////////////////////////////////////////////////////////////////////////
// CFileToolBar

UINT CFileToolBar::m_uBitmapIds[] =
{
   ID_HUB_NEW,
   ID_FILE_SAVE,
   ID_FILE_PRINT,
   ID_FILE_PRINT_PREVIEW,
   IDC_SPELLING,
   ID_EDIT_CUT,
   ID_EDIT_COPY,
   ID_EDIT_PASTE,
   ID_EDIT_UNDO,
   ID_EDIT_REDO,
	ID_VIEW_HELPWINDOW,
	ID_DESKTOP_ARTSTORE,
	ID_3DPLANET
};

// The format for the global tool bar. This defines what the user sees.
UINT CFileToolBar::m_uFormat[] =
{
   ID_HUB_NEW,
      ID_SEPARATOR,
   ID_FILE_SAVE,
   ID_FILE_PRINT,
      ID_SEPARATOR,
   ID_FILE_PRINT_PREVIEW,
   IDC_SPELLING,
      ID_SEPARATOR,
   ID_EDIT_CUT,
   ID_EDIT_COPY,
   ID_EDIT_PASTE,
      ID_SEPARATOR,
   ID_EDIT_UNDO,
   ID_EDIT_REDO,
      ID_SEPARATOR,
	ID_VIEW_HELPWINDOW,
		ID_SEPARATOR,
      ID_SEPARATOR,
      ID_SEPARATOR,
	ID_DESKTOP_ARTSTORE,
	ID_3DPLANET
};



CFileToolBar::CFileToolBar()
{
   m_uID = ID_VIEW_FILEBAR;
}

BOOL CFileToolBar::Create(CWnd* pParentWnd, DWORD dwStyle)
{
   if (!CPmwToolBar::Create(pParentWnd, dwStyle))
   {
      return FALSE;
   }

	if (!SetPalette(pOurPal)
	|| !SetButtons(m_uFormat, sizeof(m_uFormat)/sizeof(m_uFormat[0]))
	|| !SetButtonBitmaps(IDB_TOOLBAR_FILE, (COLORREF)-1, CSize(31,30), m_uBitmapIds, sizeof(m_uBitmapIds)/sizeof(m_uBitmapIds[0]))
	|| !SetBackgroundBitmap(IDB_TOOLBAR_BACKGROUND))
	{
		return FALSE;
	}

   return TRUE;
}

const UINT* CFileToolBar::GetBitmapIds(void)
{
   return m_uBitmapIds;
}


/////////////////////////////////////////////////////////////////////////////
// CFormatToolBar

// The ids of the icons in the bitmap in bitmap order.
UINT CFormatToolBar::m_uBitmapIds[] =
{
   ID_TEXT_BOLD,
   ID_TEXT_ITALIC,
   ID_TEXT_UNDERLINE,
   ID_TEXT_SHAPE,
   IDC_TEXT_STRETCHING,
   ID_ALIGN_LEFT,
   ID_ALIGN_CENTER,
   ID_ALIGN_RIGHT,
   ID_ALIGN_JUSTIFY,
   ID_LAYER_FORWARDONE,
   ID_LAYER_BACKONE,
   ID_XFLIP,
   ID_YFLIP
};

// The format for the text tool bar. This defines what the user sees.
UINT CFormatToolBar::m_uFormat[] =
{
   IDC_TEXT_TYPEFACE,
   IDC_TEXT_SIZE,
      ID_SEPARATOR,
   ID_TEXT_BOLD,
   ID_TEXT_ITALIC,
   ID_TEXT_UNDERLINE,
   ID_TEXT_SHAPE,
   IDC_TEXT_STRETCHING,
      ID_SEPARATOR,
   ID_ALIGN_LEFT,
   ID_ALIGN_CENTER,
   ID_ALIGN_RIGHT,
   ID_ALIGN_JUSTIFY,
      ID_SEPARATOR,
   ID_LAYER_FORWARDONE,
   ID_LAYER_BACKONE,
      ID_SEPARATOR,
   ID_XFLIP,
   ID_YFLIP
};

CFormatToolBar::CFormatToolBar()
{
   m_uID = ID_VIEW_FORMATBAR;
}

BEGIN_MESSAGE_MAP(CFormatToolBar, CPmwToolBar)
   //{{AFX_MSG_MAP(CFormatToolBar)
   ON_MESSAGE(UM_GET_FONT, OnGetSelFont)
   ON_MESSAGE(UM_GET_FONT_SIZE, OnGetFontSize)
   ON_MESSAGE(UM_UPDATE_FONTS, OnUpdateFonts)
   ON_MESSAGE(UM_UPDATE_FONT_MRU, OnUpdateMRU)
   ON_MESSAGE(UM_HAS_FOCUS, HasFocus)
	//}}AFX_MSG_MAP

   END_MESSAGE_MAP()

BOOL CFormatToolBar::Create(CWnd* pParentWnd, DWORD dwStyle)
{
   if (!CPmwToolBar::Create(pParentWnd, dwStyle))
   {
      return FALSE;
   }

	// Create the two "edit text" combo boxes.

   CRect rect;

   rect.top = 3;
   rect.bottom = rect.top + DropHeight;
   rect.left = 0;
   rect.right = 150;

#ifdef WANT_STYLE
   if (!m_StyleBox.Create(CBS_DROPDOWN
                           | WS_VSCROLL
                           | WS_CLIPSIBLINGS,
                         rect,
                         this,
                         IDC_TEXT_STYLE))
   {
      TRACE0("Failed to style combo-box\n");
      return FALSE;
   }
   m_StyleBox.LimitText(8);
#endif

   if (!m_TypefaceBox.Create(CBS_DROPDOWN
                              | CBS_SORT
							  | CBS_HASSTRINGS
                              | CBS_AUTOHSCROLL
							  | CBS_OWNERDRAWVARIABLE
                              | WS_VSCROLL
                              | WS_CLIPSIBLINGS,
                             rect,
                             this,
                             IDC_TEXT_TYPEFACE))
   {
      TRACE0("Failed to create typeface combo-box\n");
      return FALSE;
   }

   m_TypefaceBox.LimitText(100);

   if (!m_SizeBox.Create(CBS_DROPDOWN
                           | CBS_AUTOHSCROLL
                           | WS_VSCROLL
                           | WS_CLIPSIBLINGS,
                         rect,
                         this,
                         IDC_TEXT_SIZE))
   {
      TRACE0("Failed to create size combo-box\n");
      return FALSE;
   }

   m_SizeBox.LimitText(4);

   //  Create a font for the comboboxes
   LOGFONT logFont;
   memset(&logFont, 0, sizeof(logFont));

   if (!::GetSystemMetrics(SM_DBCSENABLED))
   {
      // Since design guide says toolbars are fixed height so is the font.
      logFont.lfHeight = -10;
      logFont.lfWeight = FW_BOLD;
      logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
      CString strDefaultFont;
//    strDefaultFont.LoadString(IDS_DEFAULT_FONT);
      strDefaultFont = "MS Sans Serif";         // Cheat a little (for now!)
      lstrcpy(logFont.lfFaceName, strDefaultFont);
      if (!m_font.CreateFontIndirect(&logFont))
      {
         TRACE0("Could not create font for combo boxes\n");
      }
      else
      {
#ifdef WANT_STYLE
         m_StyleBox.SetFont(&m_font);
#endif
         m_TypefaceBox.SetFont(&m_font);
         m_SizeBox.SetFont(&m_font);

		 // Set the height of the edit field
		 MEASUREITEMSTRUCT MeasureItemStruct;
		 MeasureItemStruct.CtlType = ODT_COMBOBOX;
		 m_TypefaceBox.MeasureItem(&MeasureItemStruct);
		 m_TypefaceBox.SetItemHeight(-1, MeasureItemStruct.itemHeight + 2);	// sorry about the '2'
      }
   }
   else
   {
      m_font.Attach(::GetStockObject(SYSTEM_FONT));
#ifdef WANT_STYLE
      m_StyleBox.SetFont(&m_font);
#endif
      m_TypefaceBox.SetFont(&m_font);
      m_SizeBox.SetFont(&m_font);
   }
   
   m_nFontAverageWidth = 8;
   
   if (m_font.GetObject(sizeof(logFont), &logFont) == sizeof(logFont))
   {
      m_nFontAverageWidth = logFont.lfHeight;
      if (m_nFontAverageWidth < 0)
      {
         m_nFontAverageWidth = -m_nFontAverageWidth;
      }
      m_nFontAverageWidth = (m_nFontAverageWidth*3)/4;
      if (m_nFontAverageWidth < 4)
      {
         m_nFontAverageWidth = 4;
      }
   }

#ifdef WANT_STYLE
   BuildStyleList();
#endif
   BuildTypefaceList();
   BuildPointSizeList();

   int	numIDs = sizeof(m_uBitmapIds)/sizeof(m_uBitmapIds[0]);
	int	numFormats = sizeof(m_uFormat)/sizeof(m_uFormat[0]);

	if (!SetPalette(pOurPal)
	|| !SetButtonBitmaps(IDB_TOOLBAR_FORMAT, (COLORREF)-1, CSize(31,30), m_uBitmapIds, numIDs)
	|| !SetBackgroundBitmap(IDB_TOOLBAR_BACKGROUND)
	|| !SetButtons(m_uFormat, numFormats))
	{
		return FALSE;
	}
	SetupCombo(0, IDC_TEXT_TYPEFACE, m_nFontAverageWidth*15, m_TypefaceBox);
	SetupCombo(1, IDC_TEXT_SIZE, m_nFontAverageWidth*5, m_SizeBox);

   return TRUE;
}

const UINT* CFormatToolBar::GetBitmapIds(void)
{
   return m_uBitmapIds;
}

/*
// Does the format bar have the focus?
*/

BOOL CFormatToolBar::HasFocus(void)
{
   CWnd* pFocus = GetFocus();

   return
#ifdef WANT_STYLE
         (pFocus == m_StyleBox.GetEdit()) ||
#endif
         (pFocus == m_TypefaceBox.GetEdit()) ||
         (pFocus == m_SizeBox.GetEdit());
}

LRESULT CFormatToolBar::OnUpdateFonts(WPARAM wParam, LPARAM lParam)
{
	BuildTypefaceList();
	return 0L;
}

LRESULT CFormatToolBar::OnGetSelFont(WPARAM wParam, LPARAM lParam)
{
	m_TypefaceBox.GetWindowText(*((CString*)lParam));
	return 0L;

}

LRESULT CFormatToolBar::OnUpdateMRU(WPARAM wParam, LPARAM lParam)
{
	m_TypefaceBox.UpdateMRU();
	return 0L;

}

LRESULT CFormatToolBar::OnGetFontSize(WPARAM wParam, LPARAM lParam)
{
	CString csSize;
	m_SizeBox.GetWindowText(csSize);
	return atoi(csSize);

}

LRESULT CFormatToolBar::HasFocus(WPARAM wParam, LPARAM lParam)
{
   CWnd* pFocus = GetFocus();
	return (pFocus == m_TypefaceBox.GetEdit()) || (pFocus == m_SizeBox.GetEdit());
}

void CFormatToolBar::EnableFontBoxes(BOOL fEnable)
{
	// Enable/disable the font related combo boxes
	if (::IsWindow(m_TypefaceBox.m_hWnd))
	{
		m_TypefaceBox.EnableWindow(fEnable);
	}
	if (::IsWindow(m_SizeBox.m_hWnd))
	{
		m_SizeBox.EnableWindow(fEnable);
	}
}

/*
// Build the typeface list.
*/

void CFormatToolBar::BuildTypefaceList(void)
{
   m_TypefaceBox.SetRedraw(FALSE);
   m_TypefaceBox.ResetContent();

   // Build the MRU combo box
   m_TypefaceBox.BuildList();

   m_TypefaceBox.SetRedraw(TRUE);
   if (m_TypefaceBox.IsWindowVisible())
   {
      m_TypefaceBox.InvalidateRect(NULL);
   }
}

/*
// Build the pointsize list.
*/

void CFormatToolBar::BuildPointSizeList(void)
{
   m_SizeBox.SetRedraw(FALSE);
   m_SizeBox.ResetContent();

   m_SizeBox.AddString("6");
   m_SizeBox.AddString("8");
   m_SizeBox.AddString("10");
   m_SizeBox.AddString("12");
   m_SizeBox.AddString("14");
   m_SizeBox.AddString("18");
   m_SizeBox.AddString("24");
   m_SizeBox.AddString("36");
   m_SizeBox.AddString("48");
   m_SizeBox.AddString("60");
   m_SizeBox.AddString("72");

   m_SizeBox.SetRedraw(TRUE);
   if (m_SizeBox.IsWindowVisible())
   {
      m_SizeBox.InvalidateRect(NULL);
   }
}

#ifdef WANT_STYLE
void CFormatToolBar::BuildStyleList(void)
{
   m_StyleBox.SetRedraw(FALSE);
   m_StyleBox.ResetContent();

   m_StyleBox.AddString("Normal");
   m_StyleBox.AddString("Wishful thinking");

   m_StyleBox.SetRedraw(TRUE);
   if (m_StyleBox.IsWindowVisible())
   {
      m_StyleBox.InvalidateRect(NULL);
   }
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditToolBar

#define	SPECIAL_DIVIDER	((UINT)-2)

CEditToolBar::CEditToolBar()
{
}

void CEditToolBar::SetFormat(void)
{
	const UINT* pFormatIds = GetFormatIds();
	int nFormatIds = GetToolBarCtrl().GetButtonCount();

	// Perform any special case button formating.
	for (int nIndex = 0; nIndex < nFormatIds; nIndex++)
	{
		switch (pFormatIds[nIndex])
		{
			case SPECIAL_DIVIDER:
			{
				// For some reason (maybe because the toolbar is vertical),
				// the *actual* height of the separator is 2/3 of the size
				// specified. For example, if you were to specify a size of
				// 20, the actual distance between buttons would be 13 pixels.
				// This is just an empirical observation, I have not
				// checked out the toolbar code to see where the 2/3 is
				// coming from.
				SetButtonInfo(nIndex, ID_SEPARATOR, TBBS_SEPARATOR, 20);
				break;
			}
		}
	}
}

// DrawSeparator() is overridden to do special drawing for separators with the
// SPECIAL_DIVIDER format. It basically draws a "dip" in the center of the separator
// area. This code *assumes* that the tool bar is vertical. Because of the way toolbar
// drawing works, only the client (button) area can be drawn in. Since we want to draw
// across the entire width of the toolbar, we have to draw the other parts in the
// DrawBackground() method.
void CEditToolBar::DrawSeparator(CDC& dc, const CRect& crButton, int nButtonIndex, UINT uButtonID, WORD wButtonStyle, WORD wButtonState)
{
	const UINT* pFormatIds = GetFormatIds();
	if (pFormatIds[nButtonIndex] != SPECIAL_DIVIDER)
	{
		CPmwToolBar::DrawSeparator(dc, crButton, nButtonIndex, uButtonID, wButtonStyle, wButtonState);
	}
	else
	{
		// Empirically, the button rectangle that is passed in (crButton) for the
		// edit bars is kind of wacky. I don't if this is because the button is a
		// separator, or if it's because the toolbar is vertical, or both. But, the
		// height of crButton is 2/3 of the height specified in the call to
		// SetButtonInfo(), and the width of crButton is the height specified (not
		// the width of the button area!) So, we use the client area as the width to
		// draw.
		//
		// *** UPDATE 4/7/98 ***
		//
		// The above is true for only for some versions of COMCTL32.DLL. So, the
		// line drawing code that was being used worked only for some versions of
		// COMCTL32.DLL and not for others. I've changed the code to not use the
		// crButton passed in for the separator, but, instead, to center the line
		// between the two adjacent buttons. This seems to work and is more stable.

		CRect crRect;
		GetClientRect(crRect);

		// Figure out top and bottom coordinates of area between
		// the surrounding buttons.
		int nTop = crRect.top;
		int nBottom = crRect.bottom;

		if (nButtonIndex > 0)
		{
			CRect crBefore;
			GetItemRect(nButtonIndex-1, crBefore);
			nTop = crBefore.bottom;
		}

		if (nButtonIndex < GetToolBarCtrl().GetButtonCount()-1)
		{
			CRect crAfter;
			GetItemRect(nButtonIndex+1, crAfter);
			nBottom = crAfter.top;
		}

		// Draw the divider in the middle of the separator area.
		int nY = (nTop+nBottom)/2;

		// Draw the shadow line.
		crRect.top = nY-1;
		crRect.bottom = nY;
		dc.FillSolidRect(crRect, RGB(0,0,0));

		// Draw the highlight line.
		crRect.top = nY;
		crRect.bottom = nY+1;
		dc.FillSolidRect(crRect, RGB(255,255,255));
	}
}

// DrawBackground() is overridden to do special drawing for separators with the
// SPECIAL_DIVIDER format. It basically draws a "dip" in the center of the separator
// area. This code *assumes* that the tool bar is vertical. Because of the way toolbar
// drawing works, DrawBackground() can only draw in the nonclient areas of the toolbar.
// The other (client) parts are drawn in the DrawSeparator() method.
void CEditToolBar::DrawBackground(CDC& dc, const CRect& crBackground, const CSize& czOffset)
{
	// Do the normal drawing.
	CPmwToolBar::DrawBackground(dc, crBackground, czOffset);

	// Draw any SPECIAL_DIVIDER separators.
	const UINT* pFormatIds = GetFormatIds();
	for (int nButton = 0; nButton < GetToolBarCtrl().GetButtonCount(); nButton++)
	{
		// Check for SPECIAL_DIVIDER.
		if (pFormatIds[nButton] == SPECIAL_DIVIDER)
		{
			// Use the full (nonclient) width of the window
			CRect crRect;
			GetWindowRect(crRect);
			ScreenToClient(crRect);
			crRect -= czOffset;

			// Figure out top and bottom coordinates of area between
			// the surrounding buttons.
			int nTop = crRect.top;
			int nBottom = crRect.bottom;

			if (nButton > 0)
			{
				CRect crBefore;
				GetItemRect(nButton-1, crBefore);
				nTop = crBefore.bottom-czOffset.cy;
			}

			if (nButton < GetToolBarCtrl().GetButtonCount()-1)
			{
				CRect crAfter;
				GetItemRect(nButton+1, crAfter);
				nBottom = crAfter.top-czOffset.cy;
			}

			// Draw in the center of the separator area.
			int nY = (nTop+nBottom)/2;

			// Draw shadow line.
			crRect.top = nY-1;
			crRect.bottom = nY;
			dc.FillSolidRect(crRect, RGB(0,0,0));

			// Draw highlight line.
			crRect.top = nY;
			crRect.bottom = nY+1;
			dc.FillSolidRect(crRect, RGB(255,255,255));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEdit1ToolBar

// The ids of the icons in the bitmap in bitmap order.

UINT CEdit1ToolBar::m_uBitmapIds[] =
{
   ID_PICTURES_IMPORT,
   ID_ADD_NFX,
	ID_DRAWPLUS,
   ID_YYWB,
	ID_ADD_HEADLINE,
   ID_TEXT_CREATE,
   ID_DRAW_LINE,
   ID_CROP,
   ID_DRAW_RECTANGLE,
   ID_DRAW_FREEDRAW
};

// The format for the draw tool bar. This defines what the user sees.
UINT CEdit1ToolBar::m_uFormat[] =
{
   ID_PICTURES_IMPORT,
      ID_SEPARATOR,
   ID_ADD_NFX,
      ID_SEPARATOR,
	ID_DRAWPLUS,
      ID_SEPARATOR,
	ID_YYWB,
		ID_SEPARATOR,
	ID_ADD_HEADLINE,
      SPECIAL_DIVIDER,
   ID_TEXT_CREATE,
      ID_SEPARATOR,
   ID_DRAW_LINE,
      ID_SEPARATOR,
   ID_CROP,
      ID_SEPARATOR,
   ID_DRAW_RECTANGLE,
      ID_SEPARATOR,
   ID_DRAW_FREEDRAW
};

CEdit1ToolBar::CEdit1ToolBar()
{
   m_uID = ID_VIEW_EDIT1BAR;
}

BOOL CEdit1ToolBar::Create(CWnd* pParentWnd, DWORD dwStyle)
{
   if (!CEditToolBar::Create(pParentWnd, dwStyle))
   {
      return FALSE;
   }
   int nFormatCount = sizeof(m_uFormat)/sizeof(m_uFormat[0]);

	if (!SetPalette(pOurPal)
	|| !SetButtonBitmaps(IDB_TOOLBAR_EDIT1, (COLORREF)-1, CSize(31,30), m_uBitmapIds, sizeof(m_uBitmapIds)/sizeof(m_uBitmapIds[0]))
	|| !SetBackgroundBitmap(IDB_TOOLBAR_BACKGROUND)
	|| !SetButtons(m_uFormat, nFormatCount))
	{
		return FALSE;
	}
	SetFormat();

   return TRUE;
}

const UINT* CEdit1ToolBar::GetBitmapIds(void)
{
	return m_uBitmapIds;
}

const UINT* CEdit1ToolBar::GetFormatIds(void)
{
	return m_uFormat;
}

#ifdef LOCALIZE

int CEdit1ToolBar::SetupFormatIds(void)
{
   int i = 0;
   m_uFormat[i++] = ID_PICTURES_IMPORT;
   m_uFormat[i++] = ID_SEPARATOR;
   if (!GetConfiguration()->RemoveCartoonMaker())
   {
		m_uFormat[i++] = ID_ADD_NFX;
		m_uFormat[i++] = ID_SEPARATOR;
   }

   if (!GetConfiguration()->RemoveSerifDraw())
   {
      m_uFormat[i++] = ID_DRAWPLUS;
		m_uFormat[i++] = ID_SEPARATOR;
   }
   
   if (!GetConfiguration()->RemoveYearYouWereBorn())
   {
		m_uFormat[i++] = ID_YYWB;
		m_uFormat[i++] = ID_SEPARATOR;
   }

	m_uFormat[i++] = ID_ADD_HEADLINE;
	m_uFormat[i++] = ID_SEPARATOR;

   
   if (!GetConfiguration()->RemoveDividers())
   {
	   if ((i>0) && (m_uFormat[i-1] == ID_SEPARATOR))
		   m_uFormat[i-1] = SPECIAL_DIVIDER;
	   else
		   m_uFormat[i++] = SPECIAL_DIVIDER;
   }

   
   m_uFormat[i++] = ID_TEXT_CREATE;
   m_uFormat[i++] = ID_SEPARATOR;
   m_uFormat[i++] = ID_DRAW_LINE;
   m_uFormat[i++] = ID_SEPARATOR;
   m_uFormat[i++] = ID_CROP;
   m_uFormat[i++] = ID_SEPARATOR;
   m_uFormat[i++] = ID_DRAW_RECTANGLE;
   m_uFormat[i++] = ID_SEPARATOR;
   m_uFormat[i++] = ID_DRAW_FREEDRAW;
   int Ret = i;
   while(i < (sizeof(m_uFormat)/sizeof(m_uFormat[0])))
   {
	m_uFormat[i++] = 0;
   }
   return Ret;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdit2ToolBar

// The ids of the icons in the bitmap in bitmap order.

// The ids of the icons in the bitmap in bitmap order.
UINT CEdit2ToolBar::m_uBitmapIds[] =
{
   ID_ADD_SENTIMENT,
// ID_PHOTO_ENHANCER,
   ID_PHOTO_WORKSHOP_BUTTON,
	ID_BORDERPLUS,
	ID_ADD_TABLE,
	ID_MADE_TO_ORDER_BUTTON,
   ID_FILL_COLOR,
   ID_OUTLINE_WIDTH,
   ID_SHADOW,
   ID_DRAW_ELLIPSE,
   ID_PICTURES_DRAWING_OBJECT
};

// The format for the modify tool bar. This defines what the user sees.
UINT CEdit2ToolBar::m_uFormat[] =
{
   ID_ADD_SENTIMENT,
      ID_SEPARATOR,
// ID_PHOTO_ENHANCER,
   ID_PHOTO_WORKSHOP_BUTTON,
      ID_SEPARATOR,
   ID_BORDERPLUS,
      ID_SEPARATOR,
	ID_ADD_TABLE,
		ID_SEPARATOR,
	ID_MADE_TO_ORDER_BUTTON,
      SPECIAL_DIVIDER,
   ID_FILL_COLOR,
      ID_SEPARATOR,
   ID_OUTLINE_WIDTH,
      ID_SEPARATOR,
   ID_SHADOW,
      ID_SEPARATOR,
   ID_DRAW_ELLIPSE,
      ID_SEPARATOR,
   ID_PICTURES_DRAWING_OBJECT
};

CEdit2ToolBar::CEdit2ToolBar()
{
   m_uID = ID_VIEW_EDIT2BAR;
}

BOOL CEdit2ToolBar::Create(CWnd* pParentWnd, DWORD dwStyle)
{
   if (!CEditToolBar::Create(pParentWnd, dwStyle))
   {
      return FALSE;
   }

	int nFormatCount = sizeof(m_uFormat)/sizeof(m_uFormat[0]);

#ifdef LOCALIZE
		  nFormatCount = SetupFormatIds();
#endif

	if (!SetPalette(pOurPal)
	|| !SetButtonBitmaps(IDB_TOOLBAR_EDIT2, (COLORREF)-1, CSize(31,30), m_uBitmapIds, sizeof(m_uBitmapIds)/sizeof(m_uBitmapIds[0]))
	|| !SetBackgroundBitmap(IDB_TOOLBAR_BACKGROUND)
	|| !SetButtons(m_uFormat, nFormatCount))
	{
		return FALSE;
	}
	SetFormat();

   return TRUE;
}

const UINT* CEdit2ToolBar::GetBitmapIds(void)
{
	return m_uBitmapIds;
}

const UINT* CEdit2ToolBar::GetFormatIds(void)
{
   return m_uFormat;
}

#ifdef LOCALIZE

int CEdit2ToolBar::SetupFormatIds(void)
{
   int i = 0;
   m_uFormat[i++] = ID_ADD_SENTIMENT;
   m_uFormat[i++] = ID_SEPARATOR;
   if (!GetConfiguration()->RemovePhotoEditor())
   {
      m_uFormat[i++] = ID_PHOTO_ENHANCER;
      m_uFormat[i++] = ID_SEPARATOR;
   }
   m_uFormat[i++] = ID_BORDERPLUS;
   m_uFormat[i++] = ID_SEPARATOR;
   m_uFormat[i++] = ID_ADD_TABLE;
   m_uFormat[i++] = ID_SEPARATOR;
	m_uFormat[i++] = ID_MADE_TO_ORDER_BUTTON;
	m_uFormat[i++] = ID_SEPARATOR;

   if (!GetConfiguration()->RemoveDividers())
   {
	   if ((i>0) && (m_uFormat[i-1] == ID_SEPARATOR))
		   m_uFormat[i-1] = SPECIAL_DIVIDER;
	   else
           m_uFormat[i++] = SPECIAL_DIVIDER;
   }
   m_uFormat[i++] = ID_FILL_COLOR;
   m_uFormat[i++] = ID_SEPARATOR;
   m_uFormat[i++] = ID_OUTLINE_WIDTH;
   m_uFormat[i++] = ID_SEPARATOR;
   m_uFormat[i++] = ID_SHADOW;
   m_uFormat[i++] = ID_SEPARATOR;
   m_uFormat[i++] = ID_DRAW_ELLIPSE;
   m_uFormat[i++] = ID_SEPARATOR;
   m_uFormat[i++] = ID_PICTURES_DRAWING_OBJECT;

   int Ret = i;
   while(i < (sizeof(m_uFormat)/sizeof(m_uFormat[0])))
   {
	m_uFormat[i++] = 0;
   }
   return Ret;
}
#endif



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
//      od("child id is %d\r\n", nID);
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
