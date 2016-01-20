/*
// $Workfile: PMBMBUT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMBMBUT.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 2     1/05/98 12:26p Johno
// Changes for proper transparent color selection
// with and without a palette
// 
//    Rev 1.0   14 Aug 1997 15:23:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:44   Fred
// Initial revision.
// 
//    Rev 1.8   04 Jun 1997 13:31:14   johno
// Turned off JTRACE
// 
//    Rev 1.7   30 May 1997 16:25:08   johno
// Fixed transparent color problem (Fred)
// 
//    Rev 1.6   27 May 1997 14:40:04   johno
// Transparent color now comes from pixel 0,0
// 
//    Rev 1.5   04 Apr 1997 09:59:28   Jay
// Use PMG palette when making CDDBs.
// 
//    Rev 1.4   03 Apr 1997 17:14:10   johno
// use define in COLORDEF.H for transparent color
// 
//    Rev 1.3   03 Apr 1997 17:09:18   johno
// Compile option for use of global Printmaster palette
// 
// 
//    Rev 1.2   13 Mar 1997 17:30:30   johno
// Someone snuck in, probably in the middle of the night,
// and put 2 references to stdafx.h in here
// 
//    Rev 1.1   12 Mar 1997 10:28:44   johno
// Changed AfxGetInstanceHandle () to AfxGetResourceHandle ()
// 
//    Rev 1.0   06 Mar 1997 16:39:34   johno
// Initial revision.
*/

#include "stdafx.h"

#include "pmbmbut.h"

#include "util.h"
#include "ddb.h"
#include "colordef.h"

//#define  JTRACE_ON
#include "jtrace.h"

#ifdef  USE_GLOBAL_PALETTE

extern CPalette *pOurPal;     // THE PrintMaster Palette (from PMWVIEW.CPP)

#define  PALETTE_VALID  ((pOurPal != NULL) && (pOurPal->m_hObject != NULL))
#define  PALETTE_NAME   pOurPal

#else
#pragma message ("Warning: Compiling for bitmap palette")

#define  PALETTE_NAME   &m_Palette
#define  PALETTE_VALID  (m_Palette.m_hObject != NULL)

#endif

/////////////////////////////////////////////////////////////////////////////
// CPmwBitmapButton

IMPLEMENT_DYNAMIC(CPmwBitmapButton, CBitmapButton)

BEGIN_MESSAGE_MAP(CPmwBitmapButton, CBitmapButton)
   //{{AFX_MSG_MAP(CPmwBitmapButton)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPmwBitmapButton::CPmwBitmapButton()
{
#ifndef  USE_GLOBAL_PALETTE
   KillPalette ();   
#endif
}

CPmwBitmapButton::~CPmwBitmapButton()
{
}

BOOL 
CPmwBitmapButton::LoadBitmaps (LPCTSTR lpszBitmapResource, LPCTSTR lpszBitmapResourceSel, LPCTSTR lpszBitmapResourceFocus, LPCTSTR lpszBitmapResourceDisabled)
{
#if 0
   return CBitmapButton::LoadBitmaps (lpszBitmapResource, lpszBitmapResourceSel, lpszBitmapResourceFocus, lpszBitmapResourceDisabled);
#else
   // delete old bitmaps (if present)
   m_bitmap.DeleteObject();
   m_bitmapSel.DeleteObject();
   m_bitmapFocus.DeleteObject();
   m_bitmapDisabled.DeleteObject();

#ifndef  USE_GLOBAL_PALETTE
   KillPalette ();
#endif

   BOOL bAllLoaded = FALSE;

   CDC   *pDC;

   if ((pDC = GetDC ()) != NULL)
   {
      HINSTANCE   hi = AfxGetResourceHandle ();
      HBITMAP     hBitmap;

#ifdef  USE_GLOBAL_PALETTE
		CPalette* pOldPalette = pDC->SelectPalette(pOurPal, FALSE);
		pDC->RealizePalette();
#endif
      CDDB  *pDDB = new CDDB (hi, lpszBitmapResource, RT_BITMAP, pDC->GetSafeHdc(), FALSE);   

      hBitmap = pDDB->GetBitmap ();

#ifdef   USE_GLOBAL_PALETTE
      pDDB->Detach (TRUE);
#else
      HPALETTE    hPalette;

      hPalette = pDDB->GetPalette ();
      m_Palette.Attach (hPalette);

      pDDB->Detach (FALSE);
#endif
      delete pDDB;

      if (m_bitmap.Attach (hBitmap) == 0)
         JTRACE0("Failed to load bitmap for normal image.\n");
      else
      {
         bAllLoaded = TRUE;
   
         if (lpszBitmapResourceSel != NULL)
         {
            pDDB = new CDDB (hi, lpszBitmapResourceSel, RT_BITMAP, pDC->GetSafeHdc(), FALSE);   
   
            hBitmap = pDDB->GetBitmap ();
            pDDB->Detach ();
            delete pDDB;
   
            if (m_bitmapSel.Attach (hBitmap) == 0)    
            {
               JTRACE0("Failed to load bitmap for selected image.\n");
               bAllLoaded = FALSE;
            }
         }
   
         if (lpszBitmapResourceFocus != NULL)
         {
            pDDB = new CDDB (hi, lpszBitmapResourceFocus, RT_BITMAP, pDC->GetSafeHdc(), FALSE);   
   
            hBitmap = pDDB->GetBitmap ();
            pDDB->Detach ();
            delete pDDB;
   
            if (m_bitmapFocus.Attach (hBitmap) == 0)     
               bAllLoaded = FALSE;
         }
      
         if (lpszBitmapResourceDisabled != NULL)
         {
            pDDB = new CDDB (hi, lpszBitmapResourceDisabled, RT_BITMAP, pDC->GetSafeHdc(), FALSE);
   
            hBitmap = pDDB->GetBitmap ();
            pDDB->Detach ();
            delete pDDB;
   
            if (m_bitmapDisabled.Attach (hBitmap) == 0)     
               bAllLoaded = FALSE;
         }
   
   #ifdef _MAC
         ASSERT(m_bitmap.m_hObject != NULL);
         ::SetBitmapReadOnly((HBITMAP)m_bitmap.m_hObject, BRO_READONLY);
         if (m_bitmapSel.m_hObject != NULL)
            ::SetBitmapReadOnly((HBITMAP)m_bitmapSel.m_hObject, BRO_READONLY);
         if (m_bitmapFocus.m_hObject != NULL)
            ::SetBitmapReadOnly((HBITMAP)m_bitmapFocus.m_hObject, BRO_READONLY);
         if (m_bitmapDisabled.m_hObject != NULL)
            ::SetBitmapReadOnly((HBITMAP)m_bitmapDisabled.m_hObject, BRO_READONLY);
   #endif
      }
      
#ifdef  USE_GLOBAL_PALETTE
		if (pOldPalette != NULL)
		{
			pDC->SelectPalette (pOldPalette, FALSE);
		}
#endif
      ReleaseDC (pDC);
   }
   
   return bAllLoaded;
#endif
}

void 
CPmwBitmapButton::DrawItem (LPDRAWITEMSTRUCT lpDIS)
{
   HPALETTE hp = NULL;

   if (PALETTE_VALID)
   {
#ifdef  USE_GLOBAL_PALETTE
      hp = ::SelectPalette (lpDIS->hDC, (HPALETTE)pOurPal->m_hObject, FALSE);
#else
      hp = ::SelectPalette (lpDIS->hDC, (HPALETTE)m_Palette.m_hObject, FALSE);
#endif
      
      RealizePalette (lpDIS->hDC);
   }
   
   CBitmapButton::DrawItem (lpDIS);

   if (hp != NULL)
   {
      ::SelectPalette (lpDIS->hDC, hp, FALSE);
   }
}

BOOL
CPmwBitmapButton::ConvertToTransparent (CBitmap *pBitmapBack, CRect &Rect)
{
   if (m_bitmap.m_hObject != NULL)
      ConvertBitmap (pBitmapBack, Rect, &m_bitmap);

   if (m_bitmapSel.m_hObject != NULL)
      ConvertBitmap (pBitmapBack, Rect, &m_bitmapSel);

   if (m_bitmapFocus.m_hObject != NULL)
      ConvertBitmap (pBitmapBack, Rect, &m_bitmapFocus);

   if (m_bitmapDisabled.m_hObject != NULL)
      ConvertBitmap (pBitmapBack, Rect, &m_bitmapDisabled);

   return FALSE;
}

BOOL
CPmwBitmapButton::ConvertBitmap (CBitmap *pBitmapBack, CRect &Rect, CBitmap *pBitmapButton)
{
   CBitmap  BitmapTemp, *pBitmapOldBack, *pBitmapOldSource;
   CPalette *pPaletteOldBack = NULL, *pPaletteOldSource = NULL;
   CDC      DCBack, DCSource;
   CRect    RectBitmap (0, 0, Rect.Width (), Rect.Height());

   if (DCSource.CreateCompatibleDC (NULL) != FALSE)
   {
      if (PALETTE_VALID)
      {
         pPaletteOldSource = DCSource.SelectPalette (PALETTE_NAME, FALSE);
         DCSource.RealizePalette ();
      }
      
      pBitmapOldSource = DCSource.SelectObject (pBitmapBack);
      BitmapTemp.CreateCompatibleBitmap (&DCSource, Rect.Width (), Rect.Height ());
         
      if (DCBack.CreateCompatibleDC (&DCSource) != FALSE)
      {
         if (PALETTE_VALID)
         {
            pPaletteOldBack = DCBack.SelectPalette (PALETTE_NAME, FALSE);
            DCBack.RealizePalette ();
         }

         pBitmapOldBack = DCBack.SelectObject (&BitmapTemp);

         DCBack.BitBlt (0, 0, Rect.Width (), Rect.Height(), &DCSource, Rect.left, Rect.top, SRCCOPY);
         
         DCSource.SelectObject (pBitmapButton);

         COLORREF TransparentColor;
         if (DCBack.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
         {
            // With palette
            TransparentColor = PALETTEINDEX (PALETTE_NAME->GetNearestPaletteIndex(DCSource.GetPixel (0, 0)));
         }
         else
         {
            // Without palette
            TransparentColor = DCSource.GetPixel (0, 0);
         }

JTRACE1 ("\nTransparent Color: %06lx\n", (DWORD)TransparentColor);         
         Util::TransparentBlt (DCBack, RectBitmap, DCSource, RectBitmap, TransparentColor);

         DCSource.BitBlt (0, 0, RectBitmap.Width (), RectBitmap.Height(), &DCBack, 0, 0, SRCCOPY);
TransparentColor = DCSource.GetPixel (0, 0);
JTRACE1 ("Transparent Color: %06lx\n", (DWORD)TransparentColor);         

         if (pPaletteOldBack != NULL)
            DCBack.SelectPalette (pPaletteOldBack, FALSE);

         DCBack.SelectObject (pBitmapOldBack);
      }
      
      if (pPaletteOldSource != NULL)
         DCSource.SelectPalette (pPaletteOldSource, FALSE);

      DCSource.SelectObject (pBitmapOldSource);

      return TRUE;
   }

   return FALSE;
}


