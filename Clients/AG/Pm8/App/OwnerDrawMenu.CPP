//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/OwnerDrawMenu.CPP 1     3/03/99 6:08p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/OwnerDrawMenu.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 1     2/25/99 11:15a Dennis
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include  <ctype.h>

#include  "OwnerDrawMenu.h"            

#define   BITMAPGAPH 2  // Space allowed around check bitmap
#define   BITMAPGAPV 1

CPOwnerDrawMenuItem::CPOwnerDrawMenuItem()
{
	m_pText  = NULL;

	m_pBitmapUnCheck = NULL;  
	m_nUnCheckHeight = 0;

	m_pBitmapCheck = NULL;  
	m_nCheckHeight = 0;

	m_nFlag = 0;
	m_bIsSeparator = FALSE;
}
    

CPMenuSpecial::CPMenuSpecial ()
{
  DWORD dw;
  
  // Get normal system colors for these to start
  m_lColorText =    GetSysColor (COLOR_MENUTEXT);               
  m_lColorBk =      GetSysColor (COLOR_MENU); 
  m_lColorHiText =  GetSysColor (COLOR_HIGHLIGHTTEXT);
  m_lColorHiBk =    GetSysColor (COLOR_HIGHLIGHT);
  m_lColorGrayed =  GetSysColor (COLOR_GRAYTEXT);
  
  dw = GetMenuCheckMarkDimensions ();
  
  m_nDefaultCheckHeight = HIWORD(dw);
  m_nWindowsCheckWidth = m_nDefaultCheckWidth = LOWORD(dw);
  
  m_bmDefaultCheck.LoadOEMBitmap (OBM_CHECK);
  
  m_nDefaultTextHeight = GetSystemMetrics (SM_CYMENU); 
  
  m_hPalette = NULL;
  
  //m_hFontHandle = NULL;
}  

CPMenuSpecial::~CPMenuSpecial ()
{
  m_bmDefaultCheck.DeleteObject ();
  m_Font.DeleteObject ();
}

BOOL CPMenuSpecial::SetDefaultCheckMark (UINT ID)
{
  BITMAP  bm;
  
  m_bmDefaultCheck.DeleteObject ();  
  m_bmDefaultCheck.LoadBitmap (ID);

  m_bmDefaultCheck.GetObject (sizeof (bm), &bm);
  
  m_nDefaultCheckHeight = bm.bmHeight;
  m_nDefaultCheckWidth = bm.bmWidth;
  
  return TRUE;
}

void CPMenuSpecial::MeasureItem (LPMEASUREITEMSTRUCT lpMIS)
{
  CDC   TVDC;
  CSize Size;
  UINT  Width = m_nDefaultCheckWidth; // At least this wide
  HFONT hOldFont = NULL;  
  // Do we have text?
  if (((CPOwnerDrawMenuItem*)lpMIS->itemData)->m_pText != NULL)
  {
    //if (TVDC.CreateDC ("DISPLAY", NULL, NULL, NULL) != 0)
    if (TVDC.CreateCompatibleDC (NULL) != 0)
    {
      char    chr;
      BOOL    DidAmper = FALSE;
      CString tmp;
      
      for (int i = 0; i < ((CPOwnerDrawMenuItem*)lpMIS->itemData)->m_pText->GetLength (); i ++)
      {
        chr = ((CPOwnerDrawMenuItem*)lpMIS->itemData)->m_pText->GetAt (i);
        if (chr == '&')
        {
          if (DidAmper == FALSE)
            DidAmper = TRUE;
          else
          {
            DidAmper = FALSE;
            tmp += chr;
          }  
        }
        else
          tmp += chr;
      }  
      
      //if (m_hFontHandle != NULL)
        //hOldFont = (HFONT)::SelectObject (TVDC.m_hDC, m_hFontHandle);
      
      if (m_Font.m_hObject != NULL)
        hOldFont = (HFONT)::SelectObject (TVDC.m_hDC, m_Font.m_hObject);
      
      TVDC.SetMapMode (MM_TEXT);
      Size = TVDC.GetTextExtent (tmp,
                                tmp.GetLength ());
      
      if (hOldFont != NULL)
        ::SelectObject (TVDC.m_hDC, hOldFont);
        
      Width += Size.cx;
    }
  }
  
  if (m_nWindowsCheckWidth != m_nDefaultCheckWidth)
    Width = (Width - m_nWindowsCheckWidth) + m_nDefaultCheckWidth;
  
  lpMIS->itemWidth = Width;
  lpMIS->itemHeight = max(m_nDefaultTextHeight, m_nDefaultCheckHeight);
    
  // Lie and make room for fake separator
  if (((CPOwnerDrawMenuItem*)lpMIS->itemData)->m_bIsSeparator == TRUE)
    lpMIS->itemHeight += m_nDefaultTextHeight / 2;
} 

void CPMenuSpecial::DrawItem (LPDRAWITEMSTRUCT pDraw)
{
  CBrush  BrushNorm (m_lColorBk), BrushSelect (m_lColorHiBk), *pOldBrush;
  CBitmap *pBitmap;
  CDC     *pDC = CDC::FromHandle (pDraw->hDC);
  RECT    TextRect, SelectRect, BitmapRect;
  HFONT   hOldFont = NULL;  

  BOOL    IsSeparator = ((CPOwnerDrawMenuItem*)pDraw->itemData)->m_bIsSeparator;
  BOOL    GotText = FALSE, GotCheck = FALSE, GotUnCheck = FALSE;//, DrawBitmap = FALSE;
  
  if (((CPOwnerDrawMenuItem*)pDraw->itemData)->m_pText != NULL)
    GotText = TRUE;
  if (((CPOwnerDrawMenuItem*)pDraw->itemData)->m_pBitmapCheck != NULL)
    GotCheck = TRUE;
  if (((CPOwnerDrawMenuItem*)pDraw->itemData)->m_pBitmapUnCheck != NULL)  
    GotUnCheck = TRUE;
  
  if (m_hPalette != NULL)
  {
    if (::SelectPalette (pDraw->hDC, m_hPalette, TRUE) != NULL)
      ::RealizePalette (pDraw->hDC);
  }
  
  //if (m_hFontHandle != NULL)
    //hOldFont = (HFONT)::SelectObject (pDraw->hDC, m_hFontHandle);
  if (m_Font.m_hObject != NULL)
    hOldFont = (HFONT)::SelectObject (pDraw->hDC, m_Font.m_hObject);
  
  TextRect = SelectRect = pDraw->rcItem;

  // Make room for bitmap
  TextRect.left += m_nDefaultCheckWidth;            
  
  if (IsSeparator == TRUE)
  {
    // Make room for fake separator
    TextRect.bottom -= m_nDefaultTextHeight / 2;   
    SelectRect.bottom -= m_nDefaultTextHeight / 2;
  }  

  BitmapRect = SelectRect;
  BitmapRect.right = BitmapRect.left + m_nDefaultCheckWidth;
  // Paint the item
  if (pDraw->itemAction & ODA_DRAWENTIRE)
  {
    if (pDraw->itemState & ODS_SELECTED)
    {
      if (pDraw->itemState & ODS_GRAYED) 
        pDC->SetTextColor (m_lColorGrayed);
      else
        pDC->SetTextColor (m_lColorHiText);
          
      pDC->SetBkColor (m_lColorHiBk);

      pDC->FillRect (&SelectRect, &BrushSelect);
    }
    else
    {
      if (pDraw->itemState & ODS_GRAYED) 
        pDC->SetTextColor (m_lColorGrayed);
      else
        pDC->SetTextColor (m_lColorText);
            
      pDC->SetBkColor (m_lColorBk);

      pDC->FillRect (&pDraw->rcItem, &BrushNorm);
    }  
        
    if (GotText == TRUE)
      pDC->DrawText (((CPOwnerDrawMenuItem*)pDraw->itemData)->m_pText->GetBuffer (1), 
                      -1,
                      &TextRect,
                      DT_LEFT | DT_SINGLELINE);  
    
    if (IsSeparator == TRUE)    
    {
      POINT prnt;
      
      pOldBrush = pDC->SelectObject (&BrushNorm);
      
      prnt.x = pDraw->rcItem.left;
      prnt.y = TextRect.bottom + m_nDefaultTextHeight / 4;

      pDC->MoveTo (prnt);
      prnt.x = pDraw->rcItem.right;
      pDC->LineTo (prnt);
      
      pDC->SelectObject (pOldBrush);
    }
  }
  // Selection changed
  if (pDraw->itemAction & ODA_SELECT)
  {
    if (pDraw->itemState & ODS_SELECTED)
    {
      if (pDraw->itemState & ODS_GRAYED) 
        pDC->SetTextColor (m_lColorGrayed);
      else
        pDC->SetTextColor (m_lColorHiText);
            
      pDC->SetBkColor (m_lColorHiBk);

      pDC->FillRect (&SelectRect, &BrushSelect);
    }
    else                      
    {
      if (pDraw->itemState & ODS_GRAYED) 
        pDC->SetTextColor (m_lColorGrayed);
      else
        pDC->SetTextColor (m_lColorText);
            
      pDC->SetBkColor (m_lColorBk);
          
      pDC->FillRect (&pDraw->rcItem, &BrushNorm);   
    }
    
    if (GotText == TRUE)
      pDC->DrawText (((CPOwnerDrawMenuItem*)pDraw->itemData)->m_pText->GetBuffer (1), 
                      -1,
                      &TextRect,
                      DT_LEFT | DT_SINGLELINE);  
    if (IsSeparator == TRUE)
    {
      POINT prnt;
      
      pOldBrush = pDC->SelectObject (&BrushNorm);
      
      prnt.x = pDraw->rcItem.left;
      prnt.y = TextRect.bottom + m_nDefaultTextHeight / 4;
      pDC->MoveTo (prnt);
      prnt.x = pDraw->rcItem.right;
      pDC->LineTo (prnt);
      
      pDC->SelectObject (pOldBrush);
    }

/*    
    if (pDraw->itemState & ODS_FOCUS)    
      pDC->DrawFocusRect (&pDraw->rcItem);                  
*/      
  }
      
  if (IsSeparator == FALSE)
  {
    int BitmapWidth = m_nDefaultCheckWidth;
    int BitmapHeight = m_nDefaultCheckHeight;
    int BitmapLeft = pDraw->rcItem.left;
    int BitmapTop = pDraw->rcItem.top;

    if (pDraw->itemState & ODS_CHECKED)    
    {
      if (GotCheck == TRUE)
      {
        pBitmap = ((CPOwnerDrawMenuItem*)pDraw->itemData)->m_pBitmapCheck;
        
        BitmapWidth -= (BITMAPGAPH * 2);
        BitmapHeight -= (BITMAPGAPV * 2);
        BitmapLeft += BITMAPGAPH;
        BitmapTop += BITMAPGAPV;
      }  
      else
      {
        pBitmap = &m_bmDefaultCheck;
/*        
        BitmapWidth = m_nDefaultCheckWidth;
        BitmapHeight = m_nDefaultCheckHeight;
        BitmapLeft = pDraw->rcItem.left;// + BITMAPGAPH;
        BitmapTop = pDraw->rcItem.top;
*/        
      }  
    }
    else
    {
      if (GotUnCheck == TRUE)
      {
        pBitmap = ((CPOwnerDrawMenuItem*)pDraw->itemData)->m_pBitmapUnCheck;
        
        BitmapWidth -= (BITMAPGAPH * 2);
        BitmapHeight -= (BITMAPGAPV * 2);
        BitmapLeft += BITMAPGAPH;
        BitmapTop += BITMAPGAPV;
      }  
      else
        pBitmap = NULL;
    }
    
    if (pBitmap != NULL)
    {
      CDC     DCTemp;
      CBitmap *pOldBitmap;
          
      if ((DCTemp.CreateCompatibleDC (pDC)) == TRUE)
      {
        if ((pOldBitmap = DCTemp.SelectObject (pBitmap)) != NULL)
        {
          int i = ((TextRect.bottom - TextRect.top) - (BitmapHeight)) / 2;
          if (i > 0)
            BitmapTop += i;

			 pDC->SetBkMode (TRANSPARENT);          

          pDC->BitBlt (BitmapLeft,// + BITMAPGAPH, 
                        BitmapTop, 
                        BitmapWidth, 
                        BitmapHeight, 
                        &DCTemp, 
                        0, 
                        0, 
                        SRCCOPY);  
              
          DCTemp.SelectObject (pOldBitmap);
        }
      }              
    }  
  }  
  
  if (hOldFont != NULL)
     ::SelectObject (pDraw->hDC, hOldFont);
/*
CRgn  rgn;
SelectRect = pDraw->rcItem;
SelectRect.left -= 8;
SelectRect.top -= 8;
SelectRect.right += 8;
SelectRect.bottom += 8;
rgn.CreateRectRgnIndirect (&SelectRect);
CBrush br (m_lColorBk);
pDC->FrameRgn (&rgn, &br, 8, 8);
*/
}

BOOL CPMenuSpecial::SetFont (CFont  &pFont)
{
  TEXTMETRIC  tm;
  CFont       *pOldFont;
  CDC         TVDC;
  LOGFONT LogFont;
  
  m_Font.DeleteObject ();
  if (pFont.GetObject (sizeof (LogFont), &LogFont) != 0)
  {
    if (m_Font.CreateFontIndirect (&LogFont) != 0)
    {
      if (m_Font.GetObject (sizeof (LogFont), &LogFont) != 0)
      {
        if (TVDC.CreateDC ("DISPLAY", NULL, NULL, NULL) != 0)
        {
          if ((pOldFont = TVDC.SelectObject (&m_Font)) != NULL)
          {
            TVDC.GetTextMetrics (&tm);
            m_nDefaultTextHeight = tm.tmHeight;
            TVDC.SelectObject (pOldFont);
        
            return TRUE;
          }
        } 
      }
      m_Font.DeleteObject ();
    }
  }
  return FALSE;
}

CPOwnerDrawMenu::CPOwnerDrawMenu ()
{
}

CPOwnerDrawMenu::~CPOwnerDrawMenu ()
{
  KillAllItems ();
}

BOOL CPOwnerDrawMenu::SetFont (HFONT hFont)
{
	BOOL  b;
	CFont Font;
	if (Font.Attach (hFont) != FALSE)
	{
	  b = SetFont (Font);
	  Font.Detach ();
	  return b;
	}
	return FALSE;
}      
	
LRESULT CPOwnerDrawMenu::IsDisAKey (UINT key)
{
  UINT    k;
  int     i;
  LRESULT ret = 0x00000000;
  
  for (i = 0; i <= m_AccKeys.GetUpperBound (); i ++)
  {
    k = m_AccKeys.GetAt (i);
    if ((UINT)tolower (k) == key)
    {
      ret = 0x00020000;
      ret |= i;
      break;
    }  
  }

  return (ret);  
}

BOOL CPOwnerDrawMenu::AppendMenu (UINT flag, UINT id, LPCSTR Strg)
{
  UINT          i, menuidx;
  BOOL          r;
  CPOwnerDrawMenuItem  *pItem;
  
  if (flag & MF_OWNERDRAW)
  {
    menuidx = m_Menu.GetMenuItemCount ();
    
    if (flag & MF_SEPARATOR)
    {
      // Tell menu item above this to fake a separator
      if (menuidx > 0)
      {
        pItem = (CPOwnerDrawMenuItem*)m_Items.GetAt (menuidx - 1);                    
        pItem->m_bIsSeparator = TRUE;
        return TRUE;
      }
      else
        return FALSE;
    }
    
    pItem = new CPOwnerDrawMenuItem;  
    pItem->m_nFlag = flag;
    
    pItem->m_pText = new CString (Strg);
    
    r = m_Menu.AppendMenu (flag, id, (LPCSTR)pItem);  
    
    if (r == TRUE)
    {
      if (pItem->m_bIsSeparator == FALSE)
      {
        for (i = 0; Strg [i] != 0; i ++)
        {
          if (Strg [i] == '&')
          {
            if (Strg [i + 1] != 0)
            {
              m_AccKeys.SetAtGrow (menuidx, Strg [i + 1]);
            }
            break;
          }
        }
      }  
      m_Items.Add (pItem);
    }  
    else
      delete pItem;
  }
  else
    r = m_Menu.AppendMenu (flag, id, Strg);  
    
  return r;  
}

BOOL CPOwnerDrawMenu::SetMenuItemBitmaps (UINT nPos, 
                              UINT nFlags, 
                              const CBitmap *uncheck, 
                              const CBitmap *check)
{
  CPOwnerDrawMenuItem  *pItem;
  BITMAP        bm;
  
  if (nFlags & MF_BYPOSITION)
  {
    pItem = (CPOwnerDrawMenuItem*)m_Items.GetAt (nPos);
    if (pItem == NULL)
      pItem = new CPOwnerDrawMenuItem;  
    
    pItem->m_pBitmapUnCheck = (CBitmap *)uncheck;
    if (uncheck == NULL)
      pItem->m_nUnCheckHeight = 0;      
    else
    {
      uncheck->GetObject (sizeof (bm), &bm);
      pItem->m_nUnCheckHeight = bm.bmHeight;
    }  
      
    pItem->m_pBitmapCheck = (CBitmap *)check;
    if (check == NULL)
      pItem->m_nCheckHeight = 0;
    else
    {
      check->GetObject (sizeof (bm), &bm);
      pItem->m_nCheckHeight = bm.bmHeight;
    }
    
    m_Items.SetAtGrow (nPos, pItem);
    
    return TRUE;
  }
  else 
    return FALSE;
}                        

BOOL CPOwnerDrawMenu::DestroyMenu ()
{
  BOOL          b;
  b = m_Menu.DestroyMenu ();
  
  KillAllItems ();
  
  return b;
}

void CPOwnerDrawMenu::KillAllItems (void)
{
  int           i;
  CPOwnerDrawMenuItem  *pItem;
  
  m_AccKeys.RemoveAll ();
  
  for (i = 0; i <= m_Items.GetUpperBound (); i ++)
  {
    pItem = (CPOwnerDrawMenuItem*)m_Items.GetAt (i);
    if (pItem != NULL)
    {
      if (pItem->m_pText != NULL) // Hey, could happen
        delete pItem->m_pText;
        
      delete pItem;
    }  
  }    
  m_Items.RemoveAll ();
}

CHECKMENUITEMRETURN CPOwnerDrawMenu::CheckMenuItem (UINT id, UINT chk)
{
  if (chk & MF_BYPOSITION) 
    return m_Menu.CheckMenuItem (id, chk);

#ifdef	WIN32
	return	0xffffffff;
#else
  return -1;  
#endif
}
