/*
// $Workfile: bkgobj.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
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
// $Log: /PM8/App/bkgobj.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 12    2/01/99 4:39p Johno
// Flipped backgrounds drawing origin correction
// 
// 11    2/01/99 2:01p Johno
// Backgrounds now flip
// 
// 10    2/01/99 9:38a Johno
// Changes for paneled HTML
// 
// 9     1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// Preliminary separate panel backgrounds
// 
// 8     1/07/99 6:08p Johno
// Changes to eliminate some h file dependecies
// 
// 7     10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 6     5/29/98 10:31a Johno
// Fixed "transparent is black" bug in CBackgroundObject::update()
// 
// 5     3/02/98 5:04p Johno
// Compile update
// 
// 4     2/27/98 2:29p Dennis
// Added special logic for CGMs.
// 
// 
// 3     2/24/98 9:15a Dennis
// More complete background object
// 
// 2     2/17/98 11:39a Dennis
// 
// 1     2/17/98 9:09a Dennis
// First Revision
// 
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "docdib.h"	// For CDIBInfo
#include "bkgobj.h"
#include "utils.h"   // for COLOR conversion
#include "grafobj.h" // For GraphicObject
#include "mainfrm.h"
#include "gif.h"     // For TRANSPARENT_INDEX
#include "webdoc.h"
#include "htmlconv.h"

extern CPalette *pOurPal;

CBackgroundObject::CBackgroundObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		: RectPageObject(type, owner)
{
   m_dbRecNumTexture = m_dbRecNumDIB = 0;
   SetBackgroundColor(UNDEFINED_COLOR);
   m_pGraphicObject = NULL;
}

CBackgroundObject::~CBackgroundObject()
{
   Empty();
}

void CBackgroundObject::SetTextureRecNum(DB_RECORD_NUMBER dbNewRecNum, BOOL Flipped /*FALSE*/)
{
   ERRORCODE   errorcode;
   // If Texture isn't changing, do nothing
   if(dbNewRecNum == m_dbRecNumTexture)
      return;
   // Free up previous objects
   Empty();

   m_dbRecNumTexture = dbNewRecNum;
   // Load new texture (if any)
   if(dbNewRecNum > 0)
   {
      PMGDatabasePtr    pPMGDataBase = get_database();

      ASSERT(pPMGDataBase);
      if(pPMGDataBase)
      {
         m_pGraphicObject = pPMGDataBase->create_graphic_object();
         ASSERT(m_pGraphicObject);
         if (m_pGraphicObject != NULL)
         {
            // set graphic ID 
            m_pGraphicObject->set_graphic_record(GetTextureRecNum());
            // Set dimensions of object
	         GraphicPtr pGraphicRecord =
		         (GraphicPtr) pPMGDataBase->get_record(GetTextureRecNum(), &errorcode, RECORD_TYPE_Graphic);
	         if (pGraphicRecord != NULL && errorcode == ERRORCODE_None)
            {
               PBOX  pboxBound;
               USHORT usXRes, usYRes;

               if(pGraphicRecord->record.type == GRAPHIC_TYPE_CGM ||
                  pGraphicRecord->record.type == GRAPHIC_TYPE_WMF)
               {
                  usXRes = pGraphicRecord->record.x_resolution;
                  usYRes = pGraphicRecord->record.y_resolution;
               }
               else
               {
                  usXRes = usYRes = 96;
               }

               pboxBound.x0 = 0;
               pboxBound.y0 = 0;
               pboxBound.x1 = pGraphicRecord->record.x_size;
               pboxBound.y1 = pGraphicRecord->record.y_size;
               ScalePBOXWithRounding(&pboxBound, 
                  PAGE_RESOLUTION, 
                  usXRes, 
                  PAGE_RESOLUTION, 
                  usYRes);
               // Done with graphic record
      	      pGraphicRecord->release();
               pGraphicRecord = NULL;

               if (Flipped)
					{
						m_pGraphicObject->add_flags(OBJECT_FLAG_xflipped);
						m_pGraphicObject->add_flags(OBJECT_FLAG_yflipped);
					}
					m_pGraphicObject->set_unrotated_bound(pboxBound);
            }
         }
      }
   }
}

void CBackgroundObject::Empty()
{
   m_dbRecNumTexture = 0;
   FreeDIB();
   FreeGraphicObject();
}

void CBackgroundObject::FreeDIB()
{
   m_cdibInfo.Free();
   m_dbRecNumDIB = 0;
}

void CBackgroundObject::FreeGraphicObject()
{
   if(m_pGraphicObject == NULL)
      return;
   // "Unattach" the record from the object so it won't get deleted
   m_pGraphicObject->set_graphic_record(0);
   // Remove from the object record from the document.
   m_pGraphicObject->destroy();
   delete m_pGraphicObject;      // Free it
   m_pGraphicObject = NULL;
}

ERRORCODE CBackgroundObject::CreateDIB(RedisplayContextPtr rc)
{
   CRect             crDeviceObject;
   POINT             ptMaxDims;
   CPtrList          plObjects;
   ERRORCODE         errorcode;
   COLORREF          crFillColor;
   CPmwDoc*          pDoc = NULL;
   PBOX              pboxBound;
   // If already have image for current texture, do nothing
   if(m_dbRecNumDIB == m_dbRecNumTexture)
      return ERRORCODE_None;
   // Note: this call to get document is Slow!
   PMGDatabasePtr    pPMGDataBase = get_database();

   ASSERT(pPMGDataBase);
   if(pPMGDataBase)
      pDoc = (CPmwDoc *) pPMGDataBase->GetDocument();
   ASSERT(pDoc);
   if(pDoc == NULL)
      return ERRORCODE_IntError;

   if(GetBackgroundColor() == UNDEFINED_COLOR)
      crFillColor = PALETTEINDEX(TRANSPARENT_INDEX);
   else
      crFillColor = colorref_from_color(GetBackgroundColor());
   // Add graphic object to CPtrList
   plObjects.AddTail(m_pGraphicObject);

   CHTMLObject::LogicalToDeviceUnits(
      m_pGraphicObject,
      crDeviceObject);
   ptMaxDims.x = crDeviceObject.Width();
   ptMaxDims.y = crDeviceObject.Height();

   pboxBound = m_pGraphicObject->get_unrotated_bound();
   // Render image as DIB
   errorcode = pDoc->BuildDIB(
      m_cdibInfo,
      pboxBound,
      ptMaxDims,
      &plObjects,
      !(rc->info.type == RASTER_TYPE_PALETTE),  // TrueColor?
      crFillColor);              
   ASSERT(errorcode == ERRORCODE_None);

   if(errorcode == ERRORCODE_None)
   {
         m_dbRecNumDIB = m_dbRecNumTexture;
         // Uncomment this block to write image to file
#ifdef _DEBUG
#if 0
      CFile   cfileDIB;
      if(cfileDIB.Open("DUMP.BMP", CFile::modeCreate | CFile::modeReadWrite))
      {
			BITMAPFILEHEADER    bmpFileHeader;
			BITMAPINFO* pbi = m_cdibInfo.BitmapInfo();

			bmpFileHeader.bfType = 0x4D42;      // "BM"
			bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                    (pbi->bmiHeader.biClrUsed*sizeof(RGBQUAD)); // Palette ?
			bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + pbi->bmiHeader.biSizeImage;
			bmpFileHeader.bfReserved1 = 0;
			bmpFileHeader.bfReserved2 = 0;

         cfileDIB.WriteHuge(&bmpFileHeader,  sizeof(BITMAPFILEHEADER));
         cfileDIB.WriteHuge(m_cdibInfo.BitmapInfo(),  pbi->bmiHeader.biSizeImage + sizeof(BITMAPINFOHEADER));
         cfileDIB.Close();
      }
#endif
#endif
   }

   return errorcode;
}
// The Update() method draw the object
UpdateStatePtr 
CBackgroundObject::update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type)
{
   CDC         dc;
	CRect       crClip;
   CRgn        rgnPreview;
   // If object has no background, nothing to do
   if (!HasBackground())
      return NULL;
   // Get ready to use DC
   dc.Attach(rc->destination_hdc);
   // Transform page coords
	PBOX pboxURBound = get_unrotated_bound();
   PBOX pboxScreenBound = pboxURBound;
	rc->pbox_to_screen(&pboxScreenBound, TRUE);
   CRect crScreenBound;
   // Convert PBOX to CRect
	rc->convert_pbox(&pboxScreenBound, &crScreenBound, NULL);

   int   nSavedDC = dc.SaveDC();
   ASSERT(nSavedDC);
   // Setup a clipping region
   if (nSavedDC)
   {
      crClip.IntersectRect(crScreenBound, clip);
      if (rgnPreview.CreateRectRgn(crClip.left,
                                   crClip.top,
                                   crClip.right,
                                   crClip.bottom))
      {
         dc.SelectClipRgn(&rgnPreview);
      }

      rc->toggle(FALSE, &crClip);
   }

   HPALETTE hOldPalette = ::SelectPalette(dc, (HPALETTE)pOurPal->GetSafeHandle(), TRUE);

   if (HasTexture() && m_pGraphicObject)
   {  
      CRect       crTextureBound;

      CreateDIB(rc);
      // Transform Texture Object coords
      PBOX pboxURBoundTexture = m_pGraphicObject->get_unrotated_bound();
      PBOX pboxScreenBoundTexture = pboxURBoundTexture;
	   rc->pbox_to_screen(&pboxScreenBoundTexture, TRUE);
      // Convert pbox to crect
	   rc->convert_pbox(&pboxScreenBoundTexture, &crTextureBound, NULL);
      crTextureBound.NormalizeRect();
      int nTextureWidth = crTextureBound.Width();
      int nTextureHeight = crTextureBound.Height();

      BITMAPINFO* pBitmapInfo = m_cdibInfo.BitmapInfo();

      ASSERT(pBitmapInfo && m_cdibInfo.BitmapData());
      if (pBitmapInfo && m_cdibInfo.BitmapData())
      {
         CRect crDestBound;

         ::SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);
         
			FLAGS	f = m_pGraphicObject->get_flags();

			if ((f & OBJECT_FLAG_xflipped) || (f & OBJECT_FLAG_yflipped))
			{
				crDestBound.bottom = crScreenBound.bottom;
				crDestBound.top = crDestBound.bottom - nTextureHeight;
				crDestBound.right = crScreenBound.right;
				crDestBound.left = crDestBound.right - nTextureWidth;

				while (crDestBound.top > 0)
				{
					::StretchDIBits(dc.GetSafeHdc(),
         			crDestBound.left,          // DestX
						crDestBound.top,           // DestY
         			nTextureWidth,             // nDestWidth
						nTextureHeight,            // nDestHeight
						0,                         // SrcX
         			0,                         // SrcY
         			pBitmapInfo->bmiHeader.biWidth,  // Source width
						pBitmapInfo->bmiHeader.biHeight, // Source height
						m_cdibInfo.BitmapData(),   // lpBits
						pBitmapInfo,               // lpBitsInfo
         			DIB_RGB_COLORS,
						SRCCOPY);                  // iUsage

					crDestBound.left -= nTextureWidth;
					crDestBound.right -= nTextureWidth;
					if (crDestBound.right < crScreenBound.left)
					{
						crDestBound.right = crScreenBound.right;
						crDestBound.left = crDestBound.right - nTextureWidth;
						crDestBound.top -= nTextureHeight;
						crDestBound.bottom -= nTextureHeight;
					}
				}
			}
			else
			{
				crDestBound = crScreenBound;
				while (crDestBound.top < crScreenBound.bottom)
				{
					::StretchDIBits(dc.GetSafeHdc(),
         			crDestBound.left,          // DestX
						crDestBound.top,           // DestY
         			nTextureWidth,             // nDestWidth
						nTextureHeight,            // nDestHeight
						0,                         // SrcX
         			0,                         // SrcY
         			pBitmapInfo->bmiHeader.biWidth,  // Source width
						pBitmapInfo->bmiHeader.biHeight, // Source height
						m_cdibInfo.BitmapData(),   // lpBits
						pBitmapInfo,               // lpBitsInfo
         			DIB_RGB_COLORS,
						SRCCOPY);                  // iUsage

					crDestBound.left += nTextureWidth;
					crDestBound.right += nTextureWidth;
					if (crDestBound.left > crScreenBound.right)
					{
						crDestBound.left = crScreenBound.left;
						crDestBound.right = crScreenBound.right;
						crDestBound.top += nTextureHeight;
						crDestBound.bottom += nTextureHeight;
					}
				}
			}
      }
   }
   else 
   {
      COLOR    bgc = GetBackgroundColor();
      if (bgc == TRANSPARENT_COLOR)
         bgc = COLOR_WHITE;
         
      if (bgc != UNDEFINED_COLOR)
      {
         CBrush      fillBrush, *pOldBrush;
         CPen        fillPen, *pOldPen;

         fillBrush.CreateSolidBrush(colorref_from_color(bgc));
         fillPen.CreatePen(PS_SOLID, 1, colorref_from_color(bgc));
         // Select brush for drawing
         pOldBrush = (CBrush *) dc.SelectObject(&fillBrush);
         pOldPen   = (CPen *) dc.SelectObject(&fillPen);
         // Draw Color rectangle
         dc.Rectangle(&crScreenBound);

         dc.SelectObject(pOldBrush);
         dc.SelectObject(pOldPen);
      }
   }

   if (nSavedDC)
   {
      dc.RestoreDC(nSavedDC);

      rc->toggle(TRUE, &crClip);
   }

	if (hOldPalette != NULL)
      ::SelectPalette(dc, hOldPalette, TRUE);
   // Done with DC
   dc.Detach();
   return NULL;
}

