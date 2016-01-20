/***************************************************************************
*  FILE:        COLORCBO.CPP                                               *
*  SUMMARY:     OwnerDraw Combo class for choosing a color.                *
*  AUTHOR:      Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
// $Log: /PM8/App/ColorCbo.cpp $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 9     4/29/98 4:23p Rlovejoy
// Code to fix memory leak.
// 
// 8     4/27/98 8:52a Fredf
// Fixed Type-o.
// 
// 7     4/24/98 6:28p Fredf
// Handles TRANSPARENT_COLOR and UNDEFINED_COLOR.
// 
// 6     4/24/98 5:38p Rlovejoy
// Exteneded for use with gradient dialog.
// 
// 5     4/24/98 4:29p Rlovejoy
// Added switch for gradient dialog.
// 
// 4     4/15/98 11:30a Johno
// Start log

*  Initials Legend:                                                        *
*     DGP       Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Revision History                                                        *
*                                                                          *
*  Date     Initials  Description of Change                                *
*  ------------------------------------------------------------------------*
*  2/12/98  DGP      Created                                               *
***************************************************************************/
// ColorCbo.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "ColorCbo.h"
#include "popup.h"
#include "util.h"
#include "utils.h"
#include "colordef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Static Data Members
CFont CColorCombo::sm_cfontText;

/////////////////////////////////////////////////////////////////////////////
// CColorCombo

CColorCombo::CColorCombo()
{
   Init();

	// Create color palette window
	m_pColorPaletteWnd = new CColorPalette;
	m_bWndAllocated = TRUE;
}

void CColorCombo::Init()
{
   m_color = COLOR_BLUE;
   m_bShowPalette = TRUE;
   m_bSizedItems = FALSE;
}

CColorCombo::~CColorCombo()
{
	if (m_bWndAllocated)
	{
		delete m_pColorPaletteWnd;
	}
}

#define ID_WEB_PAGE_COLOR_PALETTE   2000

BEGIN_MESSAGE_MAP(CColorCombo, CComboBox)
	//{{AFX_MSG_MAP(CColorCombo)
   ON_COMMAND(ID_WEB_PAGE_COLOR_PALETTE, OnColorPalette)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorCombo message handlers

void CColorCombo::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   CBrush      fillBrush, *pOldBrush;
   CPen        fillPen, *pOldPen;
   CDC         dc;
   CRect       crFill, crBorder;
   
   dc.Attach(lpDrawItemStruct->hDC);

   // Set rectangles equal to drawing area of control
   crFill = lpDrawItemStruct->rcItem;
   crBorder = crFill;

   crFill.InflateRect(-2, -2, -2 ,-2);

   if(lpDrawItemStruct->itemState & ODS_FOCUS)
      dc.DrawFocusRect(crBorder);

	if(m_color == UNDEFINED_COLOR)
		{
         fillBrush.CreateSolidBrush(RGB_COLOR_WHITE);
         fillPen.CreatePen(PS_SOLID, 0, RGB_COLOR_WHITE);

         // Select brush for drawing
         pOldBrush = (CBrush *) dc.SelectObject(&fillBrush);
         pOldPen   = (CPen *) dc.SelectObject(&fillPen);

         // Draw Color rectangle
         dc.Rectangle(&crFill);

         dc.SelectObject(pOldBrush);
         dc.SelectObject(pOldPen);
		}
   else if (m_color == TRANSPARENT_COLOR)
      {
         ASSERT(CreateItemFont(crFill));
         if(CreateItemFont(crFill))
            {
               int      prevBkMode;
               COLORREF crOldTextColor;
               CFont    *pOldFont;
               CString  csResource;

               prevBkMode = dc.SetBkMode(TRANSPARENT);
               crOldTextColor = dc.SetTextColor(RGB_COLOR_BLACK);
               pOldFont = dc.SelectObject(&sm_cfontText);

               csResource.LoadString(IDS_COLORCOMBO_NONE);
               ASSERT(!csResource.IsEmpty());

               // Actually draw the text.
               Util::DrawText(&dc, csResource, crFill,
                              DT_CENTER | DT_VCENTER | DT_TOP | DT_NOPREFIX | DT_SINGLELINE);

               // restore everything
               dc.SetBkMode(prevBkMode);
               dc.SetTextColor(crOldTextColor);
               if (pOldFont)
                  dc.SelectObject(pOldFont);
            }
      }
   else
      {
         fillBrush.CreateSolidBrush(colorref_from_color(m_color));
         fillPen.CreatePen(PS_SOLID, 0, (m_color != COLOR_WHITE) ? colorref_from_color(m_color) : RGB_COLOR_BLACK);

         // Select brush for drawing
         pOldBrush = (CBrush *) dc.SelectObject(&fillBrush);
         pOldPen   = (CPen *) dc.SelectObject(&fillPen);

         // Draw Color rectangle
         dc.Rectangle(&crFill);

         dc.SelectObject(pOldBrush);
         dc.SelectObject(pOldPen);
      }

   // Done with dc
   dc.Detach();
}

BOOL CColorCombo::FontMetrics(TEXTMETRIC& Metrics)
{
	BOOL fResult = FALSE;
	
	CClientDC cdcWnd(this);
					
	CFont* pcfWnd = GetFont();
				
	if (pcfWnd != NULL)
	{
		CFont* pcfOld = cdcWnd.SelectObject(pcfWnd);
					
		if (pcfOld != NULL)
		{
			if (cdcWnd.GetTextMetrics(&Metrics))
			{
				fResult = TRUE;
			}
						
			cdcWnd.SelectObject(pcfOld);
		}
	}
	
	return fResult;
}

void CColorCombo::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	lpMeasureItemStruct->itemWidth = 0;
	lpMeasureItemStruct->itemHeight = 0;
	
   // Set the item dimensions based on the current font.

   TEXTMETRIC tmFont;

   if (FontMetrics(tmFont))
   {
	   CRect crClient;
	   GetClientRect(crClient);
					
	   lpMeasureItemStruct->itemWidth = crClient.Width();
	   lpMeasureItemStruct->itemHeight = tmFont.tmHeight+2;
   }
	
}

void CColorCombo::DoDropDown() 
{
   CRect          crectThis;
   UINT           uPaletteID;

   // Make sure it doesn't exist
   m_pColorPaletteWnd->DestroyWindow();
   if(!m_bShowPalette)
   {
      m_bShowPalette = TRUE;
      return;
   }

   m_bShowPalette = FALSE;
   m_pColorPaletteWnd->SetColor(m_color);
   GetWindowRect(&crectThis);

   uPaletteID = ID_WEB_PAGE_COLOR_PALETTE; //ID_FILL_COLOR_PALETTE;

   // Create the window.
   m_pColorPaletteWnd->SetOwner(this); // Set owner to dialog
   m_pColorPaletteWnd->Create(AfxGetMainWnd(), ID_WEB_PAGE_COLOR_PALETTE);

   // Get the window rect so we can know how big this guy is.
   CRect crWindow;
   m_pColorPaletteWnd->GetWindowRect(crWindow);

   int nWidth = crWindow.Width();
   int nHeight = crWindow.Height();

   // Determine the position for the ColorPaletteWnd.
   CPoint p;

   p.x = crectThis.left;
   p.y = crectThis.bottom;

   // Make sure palette window is on the screen.
   CRect crScreen;
#ifdef WIN32
   if (SystemParametersInfo(SPI_GETWORKAREA, 0, (LPRECT)crScreen, FALSE) == 0)
   {
      crScreen.SetRectEmpty();
   }
#else
   crScreen.SetRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#endif

   if (!crScreen.IsRectEmpty())
   {
      if (p.x+nWidth > crScreen.right) 
      {
         p.x = crScreen.right-nWidth;
      }
      if (p.y+nHeight > crScreen.bottom) 
      {
         p.y = crScreen.bottom-nHeight;
      }
      if (p.x < crScreen.left)
      {
         p.x = crScreen.left;
      }
      if (p.y < crScreen.top)
      {
         p.y = crScreen.top;
      }
   }

   // Position the ColorPaletteWnd.
   m_pColorPaletteWnd->SetWindowPos(NULL, p.x, p.y, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);

   // Show the ColorPaletteWnd.
   m_pColorPaletteWnd->ShowWindow(SW_SHOW);
   m_pColorPaletteWnd->SetFocus();
}

void CColorCombo::SizeItems(void)
{
	MEASUREITEMSTRUCT mis;
				
	mis.CtlType = ODT_COMBOBOX;
	mis.CtlID = GetDlgCtrlID();
	mis.itemWidth = 0;
	mis.itemHeight = 0;
				
	DWORD dwStyle = ::GetWindowLong(GetSafeHwnd(), GWL_STYLE);
			
	mis.itemID = (UINT)-1;
	mis.itemData = 0;

	if (GetParent()->SendMessage(WM_MEASUREITEM, (WPARAM)(mis.CtlID), (LPARAM)(&mis)) != 0)
	{
#ifdef WIN32
		SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), (LPARAM)(mis.itemHeight));
#else
		SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), MAKELPARAM(mis.itemHeight, 0));
		
		// It seems that on some 16-bit systems, CB_SETITEMHEIGHT(-1, ...) sets the height
		// of the control, not the edit area. So, we have to check for this and reset the
		// height to a large value if this is the case.
		CRect crRect;
		GetWindowRect(crRect);
		if (crRect.Height() <= (int)mis.itemHeight)
		{
			SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), MAKELPARAM(mis.itemHeight+6, 0));
		}
#endif
	}
	
	if ((dwStyle & CBS_OWNERDRAWFIXED) != 0)
	{
		mis.itemID = 0;
		mis.itemData = 0;
				
		if (GetParent()->SendMessage(WM_MEASUREITEM, (WPARAM)(mis.CtlID), (LPARAM)(&mis)) != 0)
		{
#ifdef WIN32
			SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), (LPARAM)(mis.itemHeight));
#else
			SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), MAKELPARAM(mis.itemHeight, 0));
#endif
		}
	}
			
	if ((dwStyle & CBS_OWNERDRAWVARIABLE) != 0)
	{
		UINT nCount = (UINT)SendMessage(CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
				
		for (mis.itemID = 0; mis.itemID < nCount; mis.itemID++)
		{
			mis.itemData = SendMessage(CB_GETITEMDATA, (WPARAM)(mis.itemID), (LPARAM)0);
				
			if (GetParent()->SendMessage(WM_MEASUREITEM, (WPARAM)(mis.CtlID), (LPARAM)(&mis)) != 0)
			{
#ifdef WIN32
				SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), (LPARAM)(mis.itemHeight));
#else
				SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), MAKELPARAM(mis.itemHeight, 0));
#endif
			}
		}
	}
   m_bSizedItems = TRUE;
}



void CColorCombo::OnColorPalette()
   {
      m_color = m_pColorPaletteWnd->GetColor();
      m_bShowPalette = TRUE;  // Allow palette to be shown again
      SetFocus();
      Invalidate();           // Repaint control with new color

		// Notify our owner.
		CWnd* pOwner = GetOwner();
		if (pOwner != NULL)
		{
			pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBN_SELCHANGE), (LPARAM)GetSafeHwnd());
		}
   }

void CColorCombo::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   // Eat Mouse Message

//	CComboBox::OnLButtonDblClk(nFlags, point);
}

void CColorCombo::OnLButtonDown(UINT nFlags, CPoint point) 
{
   // Eat Mouse Message
//	CComboBox::OnLButtonDown(nFlags, point);
}

void CColorCombo::OnLButtonUp(UINT nFlags, CPoint point) 
{
   // If got here from tab key, ignore
   if(point.x < 0 || point.y < 0)
      return;

	DoDropDown();
	
   // Eat Mouse Message
//	CComboBox::OnLButtonUp(nFlags, point);
}

void CColorCombo::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CComboBox::OnKeyUp(nChar, nRepCnt, nFlags);

   if(nChar == VK_DOWN)
      DoDropDown();
}

void CColorCombo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CColorCombo::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();

   // Have control size itself if it hasn't already done so
   if(!m_bSizedItems)
      SizeItems();
}

BOOL CColorCombo::CreateItemFont(CRect &crDims)
   {
      if(sm_cfontText.GetSafeHandle())
         return TRUE;

      LOGFONT lfFont;

      lfFont.lfHeight = crDims.Height();
      lfFont.lfWidth = 0;
      lfFont.lfEscapement = 0;
      lfFont.lfOrientation = 0;
      lfFont.lfWeight = 400;
      lfFont.lfItalic = 0;
      lfFont.lfUnderline = 0;
      lfFont.lfStrikeOut = 0;
      lfFont.lfCharSet = ANSI_CHARSET;
      lfFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
      lfFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
      lfFont.lfQuality = DEFAULT_QUALITY;
      lfFont.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
      strcpy(lfFont.lfFaceName, "MS Sans Serif");
      return sm_cfontText.CreateFontIndirect(&lfFont);
   }

void CColorCombo::OnDestroy() 
{
	CComboBox::OnDestroy();
	
   m_pColorPaletteWnd->DestroyWindow();

   Init();
}

COLOR CColorCombo::GetColor() const
   {
//      return colorref_from_color(m_color);
      return m_color;
   }

void CColorCombo::SetColor(COLOR color)
   {
      m_color = color;
//      m_color = color_from_colorref(crColor);
   }


UINT CColorCombo::OnGetDlgCode() 
{
	// TODO: Add your message handler code here and/or call default
	
	return CComboBox::OnGetDlgCode();
}

void CColorCombo::OnCancelMode() 
{
	CComboBox::OnCancelMode();

   m_bShowPalette = TRUE;  // Allow palette to be shown again
   SetFocus();
   Invalidate();           // Repaint control with new color
}

/////////////////////////////////////////////////////////////////////////////
// CGradientColorCombo

CGradientColorCombo::CGradientColorCombo() :
	CColorCombo()
{
   Init();

	// Create color palette window
	if (m_bWndAllocated)
	{
		delete m_pColorPaletteWnd;
	}
	m_pColorPaletteWnd = new CGradientColorPalette;
	m_bWndAllocated = TRUE;
}
