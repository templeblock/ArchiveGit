/*
// $Workfile: dib2file.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/dib2file.cpp $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 26    1/28/99 6:19p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// 
// 25    9/23/98 2:40p Johno
// Stripped out cache to tinycash.*
// 
// 24    7/23/98 6:45p Jayn
// SavePMObjectsTo... can skip special objects(like calendars)
// 
// 23    7/11/98 2:47p Jayn
// Forces 24-bit mode for WMFs(no palettes!)
// 
// 22    5/28/98 4:10p Jayn
// More cool table stuff(cell formatting, etc.)
// 
// 21    2/11/98 5:10p Hforman
// moved SavePMObjectsToFile() here
// 
// 20    1/27/98 6:46p Hforman
// minor cleanup
// 
// 19    1/13/98 11:15a Jayn
// Transparent GIF support.
// 
// 18    1/09/98 5:26p Johno
// Changes for "Save in Art Gallery" option with "Save as Graphic File".
// 
// 17    12/09/97 4:05p Johno
// Got rid of Status - use only ERRORCODE
// 
// 16    12/08/97 5:00p Johno
// Made write cache buffer dynamic
// Changed WMF resolution
// 
// 15    12/08/97 9:55a Johno
// Use new instead of GlobalAllocPtr
// 
// 14    12/05/97 4:23p Johno
// Got rid of yFlip for JPEGs
// 
// 13    12/04/97 5:09p Johno
// Checks for GIF support
// 
// 12    12/03/97 4:52p Johno
// Cleaned up
// 
// 11    12/02/97 3:45p Johno
// Meta files
// 
// 10    11/25/97 10:40a Johno
// Fixed 256 color bitmap write; use exceptions
// 
// 9     11/21/97 5:21p Johno
// General update
// 
// 8     11/20/97 10:37a Johno
// Use CDIBInfo::PrepareData
// 
// 6     11/19/97 2:38p Johno
// Re - sync to Source Safe
// 
// 5     11/18/97 5:49p Johno
// Moved TIFF code to tiffdrv.cpp
// 
// 4     11/18/97 9:46a Johno                                            
// Tiff almost works
// 
// 3     11/14/97 11:58a Johno
// 
// 2     11/13/97 5:21p Johno
// 
// 1     11/13/97 5:06p Johno
// 
*/

#include "stdafx.h"
#include "pmw.h"

#include "dib2file.h"
#include "bmp.h"
#include "gif.h"
#include "util.h"       // For ROUNDTO32BITS
#include "jpegimpl.h"
#include "tiffdrv.h"
#include "pngimpl.h"
#include "pcx.h"
#include "grafobj.h"
#include "wmf.h"
#include "clip.h"		// for CopyObjectsToDatabase()
#include "pmwcfg.h"
#include "docdib.h"		// For CDIBInfo

extern CPalette *pOurPal;
extern BYTE far screen_gamma_curve[];
extern PCOORD scale_pcoord(PCOORD x, PCOORD n, PCOORD d);

void 
CDibToGraphicFile::Breathe(HWND BreatheDlg)
{
   MSG Message;

   while(PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
   {
      if ((BreatheDlg == NULL) || !::IsDialogMessage(BreatheDlg, &Message))
      {
         ::TranslateMessage(&Message);
         ::DispatchMessage(&Message);
      }
   }
}

CDibToGraphicFile::CDibToGraphicFile(LPCSTR  FileName)
   : CStorageFileTinyCache(FileName)
{
}

BOOL
CDibToGraphicFile::CreateGraphicFile(CPmwDoc *pDoc, enum SaveGraphicType Type, CPoint Dims, BOOL Selected, HWND BreatheDlg, long dpi)
{
   ASSERT(pDoc != NULL);
   if (pDoc == NULL)
      ERRORCODESave(ERRORCODE_BadParameter);
   else
   {
      Breathe(BreatheDlg);   // Allow the dialog box to go away
      CWaitCursor WaitCursor;
      if (Type == WMeta)
      {
			// Metafiles are different - don't build a DIB
         if (ERRORCODESave(initialize()) != ERRORCODE_None)
            return FALSE;

         CPtrList List;
         // Selected objects, or all objects
         PBOX World;
         if (Selected == TRUE)
         {
            pDoc->GetSelectBound(&World);
            
            PMGPageObjectPtr  po = pDoc->selected_objects();
            while(po != NULL)
            {
               List.AddTail(po);
               po = po->next_selected_object();
            }
         }   
         else
         {
            pDoc->get_panel_world(&World);
            
            PageObjectPtr  po = pDoc->objects();
            while(po != NULL)
            {
               List.AddTail(po);
               po = po->next_object();
            }
         }
         
         DoWmf(World, Dims, &List, dpi);
      }
      else
      {
         // 24 bit color for those that need it
         BOOL  TrueColor =((Type == Bmp24) ||(Type == Jpeg) ||(Type == Tiff) ||(Type == Png)) ? TRUE : FALSE;
			BOOL  fTransparent =(Type == Gif);
			COLORREF  clBackground = fTransparent ? PALETTEINDEX(TRANSPARENT_INDEX) : RGB(255, 255, 255);
         CDIBInfo Info;
         // Build a dib of selected or all objects
         if (Selected == FALSE)
            ERRORCODESave(pDoc->BuildPanelDIB(Info, Dims, TrueColor, clBackground));
         else
         {
            PBOX World;
            pDoc->GetSelectBound(&World);
         
            CPtrList          List;
            PMGPageObjectPtr  po = pDoc->selected_objects();

            while(po != NULL)
            {
               List.AddTail(po);
               po = po->next_selected_object();
            }

            ERRORCODESave(pDoc->BuildDIB(Info, World, Dims, &List, TrueColor, clBackground));
         }

         if (ErrorCode == ERRORCODE_None)
         {
            // Create the output file
            if (ERRORCODESave(initialize()) == ERRORCODE_None)
            {
               switch(Type)
               {
                  case Bmp8:     // 8 / 24 bit decision already made
                  case Bmp24:
                  DoBmp(&Info);
                  break;
               
                  case Gif:
                  DoGif (&Info);
                  break;

                  case Jpeg:
                  DoJpeg(&Info);
                  break;

                  case Tiff:
                  DoTiff(&Info, dpi);
                  break;

                  case Png:
                  DoPng(&Info);
                  break;

                  case Pcx:
                  DoPcx(&Info, dpi);
                  break;

                  default:
                  ERRORCODESave(ERRORCODE_BadParameter);
                  break;
               }
            }
         }
      }
   }

   // Something wrong? Kill file...
   if (ErrorCode != ERRORCODE_None)
   {
      ERRORCODE   Save = ErrorCode;
      zap();
      ErrorCode = Save;
   }

   return(ErrorCode == ERRORCODE_None);
}
void 
CDibToGraphicFile::DoBmp(CDIBInfo *pInfo)
{
   BITMAPINFO  *pbi = pInfo->BitmapInfo();
   DWORD       DataSize = pbi->bmiHeader.biSizeImage;
   WORD        PalSize = DIBPaletteSize(&pbi->bmiHeader);

   BITMAPFILEHEADER  bfh;

   bfh.bfType        = 'MB';
   bfh.bfSize        = sizeof(BITMAPFILEHEADER) + pbi->bmiHeader.biSize + PalSize + DataSize;
   bfh.bfReserved1   = bfh.bfReserved2 = 0;
   bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + pbi->bmiHeader.biSize + PalSize;

   TRY
   {
      ERRORCODESave(huge_write(&bfh, sizeof(BITMAPFILEHEADER)));
      ERRORCODESave(huge_write(pbi, sizeof(BITMAPINFOHEADER)));
      if (PalSize > 0)
         ERRORCODESave(huge_write(pbi->bmiColors, PalSize));      
      ERRORCODESave(huge_write(pInfo->BitmapData(), DataSize));
      ERRORCODESave(flush());
   }
   CATCH(CErrorcodeException, e)
   {
      ERRORCODESave(e->m_error);
   }
   AND_CATCH_ALL(e)
   {
      ERRORCODESave(ERRORCODE_Fail);
   }
   END_CATCH_ALL
}

void 
CDibToGraphicFile::DoTiff(CDIBInfo *pInfo, long dpi)
{
   BITMAPINFO  *pbi = pInfo->BitmapInfo();
   // Must have a 24 bit dib
   ASSERT(pbi->bmiHeader.biBitCount == 24);

   CDIBtoTIFFWriter  DIBtoTIFFWriter;   
   
   TRY
   {
      DIBtoTIFFWriter.CreateTIFF(this, pInfo, dpi);
   }
   CATCH(CErrorcodeException, e)
   {
      ERRORCODESave(e->m_error);
   }
   AND_CATCH_ALL(e)
   {
      ERRORCODESave(ERRORCODE_Fail);
   }
   END_CATCH_ALL

   ERRORCODESave(DIBtoTIFFWriter.GetError());
}

void 
CDibToGraphicFile::DoGif (CDIBInfo *pInfo)
{
   if (!GetConfiguration()->SupportsLZW())
      return;
   
   CGIFDataSinkDevice Sink(this);
   CDIBtoGIFWriter Writer;

   TRY
   {
      Writer.CreateGIF(pInfo->BitmapInfo(), pInfo->BitmapData(), &Sink, FALSE, TRANSPARENT_INDEX);
      ERRORCODESave(ERRORCODE_None);
   }
   CATCH(CErrorcodeException, e)
   {
      ERRORCODESave(e->m_error);
   }
   AND_CATCH_ALL(e)
   {
      ERRORCODESave(ERRORCODE_Fail);  // Need more detail here?
   }
   END_CATCH_ALL
}

void 
CDibToGraphicFile::DoJpeg(CDIBInfo *pInfo)
{
   BITMAPINFO  *pbi = pInfo->BitmapInfo();
   // CJPEGCompressor must have a 24 bit dib
   ASSERT(pbi->bmiHeader.biBitCount == 24);

   CJPEGCompressor   JPEGCompressor;   

   TRY
   {
      ERRORCODESave(JPEGCompressor.Init(this, pbi->bmiHeader.biWidth, pbi->bmiHeader.biHeight, 3));
      ERRORCODESave(JPEGCompressor.WriteLines((LPBYTE)pInfo->BitmapData(), pbi->bmiHeader.biHeight, pbi->bmiHeader.biHeight > 0));
      ERRORCODESave(JPEGCompressor.Finish());
   }
   CATCH(CErrorcodeException, e)
   {
      ERRORCODESave(e->m_error);
   }
   AND_CATCH_ALL(e)
   {
      ERRORCODESave(ERRORCODE_Fail);
   }
   END_CATCH_ALL
}

void
CDibToGraphicFile::DoPng(CDIBInfo *pInfo)
{
   BITMAPINFO  *pbi = pInfo->BitmapInfo();
   // Must have a 24 bit dib
   ASSERT(pbi->bmiHeader.biBitCount == 24);
   
   LPBYTE   pData = pInfo->BitmapData();
   DWORD    RowBytes =(pbi->bmiHeader.biBitCount * pbi->bmiHeader.biWidth) >> 3;
   DWORD    Rows = pbi->bmiHeader.biHeight;

   CPNGWriter  PNGWriter;

   TRY
   {
      ERRORCODESave((ERRORCODE)PNGWriter.Initialize(this));
      ERRORCODESave((ERRORCODE)PNGWriter.SetInfo(pbi->bmiHeader.biBitCount >> 3, 
                                          RowBytes, 
                                          pbi->bmiHeader.biWidth, 
                                          pbi->bmiHeader.biHeight));
      ERRORCODESave((ERRORCODE)PNGWriter.WriteDIB(pData, TRUE));
   }
   CATCH(CErrorcodeException, e)
   {
      ERRORCODESave(e->m_error);
   }
   AND_CATCH_ALL(e)
   {
      ERRORCODESave(ERRORCODE_Fail);
   }
   END_CATCH_ALL
}

void
CDibToGraphicFile::DoPcx(CDIBInfo *pInfo, long dpi)
{
   BITMAPINFO  *pbi = pInfo->BitmapInfo();
   // Must have a 8 bit dib
   ASSERT(pbi->bmiHeader.biBitCount == 8);
   DWORD    RowBytes =(pbi->bmiHeader.biBitCount * pbi->bmiHeader.biWidth) >> 3;
   DWORD    Rows = pbi->bmiHeader.biHeight;

   CDIBtoPCXWriter   DIBtoPCXWriter;

   DIBtoPCXWriter.CreatePCX(this, pInfo, dpi); 
}

void
CDibToGraphicFile::DoWmf(PBOX World, CPoint Dims, CPtrList *pList, long dpi)
{
   ERRORCODESave(ERRORCODE_Fail);

   CMetaFileDC MetaFileDC;

   if (MetaFileDC.Create(NULL) == TRUE)
   {
      CRect Rect;
      Rect.SetRectEmpty();
      Rect.right = Dims.x;
      Rect.bottom = Dims.y;

      DrawIntoMetafile(MetaFileDC, World, pList);

      HMETAFILE  hem = MetaFileDC.Close();

      if (hem != NULL)
      {
         UINT  Size = GetMetaFileBitsEx(hem, 0, NULL);

         BYTE  *pMem = NULL;
            
         if (Size > 0)
         {
            TRY
			   {
				   pMem = new BYTE [Size];
               if (::GetMetaFileBitsEx(hem, Size, pMem) == Size)
               {
                  ALDUS_WMF_HEADER  Header;

                  Header.key = ALDUS_WMF_KEY;
                  Header.hmf = NULL;
                  Header.bbox.left   =(SHORT)Rect.left;   
                  Header.bbox.top    =(SHORT)Rect.top;   
                  Header.bbox.right  =(SHORT)Rect.right;   
                  Header.bbox.bottom =(SHORT)Rect.bottom;   
                  Header.inch = 96;
                  //Header.inch =(SHORT)dpi;
                  //Header.inch = 2540;
	               Header.reserved = 0;
	               Header.checksum = 0;
               
                  WORD    *p;

                  for(p =(WORD *)&Header, Header.checksum = 0; p <(WORD *)&(Header.checksum); ++p)
                     Header.checksum ^= *p;

                  ERRORCODESave(huge_write(&Header, sizeof(ALDUS_WMF_HEADER)));
                  ERRORCODESave(huge_write(pMem, Size));
               }
            }                
            CATCH(CMemoryException, e)
            {
               ERRORCODESave(ERRORCODE_Memory);
            }
            CATCH(CErrorcodeException, e)
            {
               ERRORCODESave(e->m_error);
            }
            AND_CATCH_ALL(e)
            {
               ERRORCODESave(ERRORCODE_Fail);
            }
            END_CATCH_ALL
         }
         else
            ERRORCODESave(ERRORCODE_Fail);

         if (pMem != NULL)
            delete [] pMem;

         DeleteMetaFile(hem);
      }
      else
         DEBUG_ERRORBOX("MetaFileDC.Close() failed");
   }
}

// From CLIP.CPP - RenderAsWMF
void
CDibToGraphicFile::DrawIntoMetafile(CDC &dc, PBOX Bounds, CPtrList *pList)
{
	PCOORD DimX = Bounds.x1 - Bounds.x0;
	PCOORD DimY = Bounds.y1 - Bounds.y0;

// Our map mode will MM_ANISOTROPIC(units are HIMETRIC).

	//int nPageResolution = 2540;
	//int nResolution = nPageResolution;
//int   nResolution = 2540;
int   nResolution = 96;

	DimX = scale_pcoord(DimX, nResolution, PAGE_RESOLUTION);
	DimY = scale_pcoord(DimY, nResolution, PAGE_RESOLUTION);

/*
// Force the dimensions to be something reasonable.
*/

	BOOL fAbort = FALSE;
/*
// Force the dimensions to be something reasonable for generation.
*/

	//while(!fAbort &&(DimX > 2048 || DimY > 2048))
   while(!fAbort &&(DimX > 4096 || DimY > 4096))
	{
		DimX >>= 1;
		DimY >>= 1;
		nResolution >>= 1;

		if (DimX == 0)
		{
			DimX = 1;
			fAbort = TRUE;
		}
		if (DimY == 0)
		{
			DimY = 1;
			fAbort = TRUE;
		}
		if (nResolution == 0)
		{
			nResolution = 1;
			fAbort = TRUE;
		}
	}

	int nSave = dc.SaveDC();
	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowOrg(0, 0);
	//dc.SetWindowExt(Dims.x, Dims.y);
   dc.SetWindowExt(DimX, DimY);

	dc.SetTextColor(RGB(0, 0, 0));
	dc.SetBkColor(RGB(255, 255, 255));
	dc.SetBkMode(OPAQUE);
	dc.SetROP2(R2_COPYPEN);

	HDC hAttribDC = ::GetDC(NULL);
	if (hAttribDC != NULL)
	{
		// This is useful for OLE objects.
		::SetMapMode(hAttribDC, MM_ANISOTROPIC);
		::SetWindowOrgEx(hAttribDC, 0, 0, NULL);
		//::SetWindowExtEx(hAttribDC,(int)Dims.x,(int)Dims.y, NULL);
      ::SetWindowExtEx(hAttribDC, DimX, DimY, NULL);
		dc.SetAttribDC(hAttribDC);

		RedisplayContext rc;

		rc.destination_hdc = dc.GetSafeHdc();
		rc.AttributeDC(hAttribDC);

		rc.hwnd = NULL;
		rc.set_info(hAttribDC);

		// Force 24-bit mode.
		rc.info.type = RASTER_TYPE_24;
		rc.info.bpp = 24;
		rc.info.planes = 1;

		// Establish the palettes if necessary.

		HPALETTE hOurPal =(HPALETTE)pOurPal->GetSafeHandle();

		HPALETTE hOldPal = NULL;
		HPALETTE hOldAttribPal = NULL;

		if (rc.info.type == RASTER_TYPE_PALETTE)
		{
			hOldPal = SelectPalette(rc.destination_hdc, hOurPal, FALSE);
			RealizePalette(rc.destination_hdc);
			hOldAttribPal = SelectPalette(hAttribDC, hOurPal, FALSE);
			RealizePalette(hAttribDC);
		}

		rc.x_resolution =
			rc.y_resolution = nResolution;

//				od("num:%d, denom:%d\r\n", rc.GetScaleNumerator, rc.GetScaleDenominator());

		rc.scaled_source_x0 =  Bounds.x0 * rc.x_resolution;
		rc.scaled_source_y0 =  Bounds.y0 * rc.y_resolution;

		rc.destination_rect.left =
			rc.destination_rect.top =
			rc.destination_x0 =
			rc.destination_y0 = 0;

		//rc.destination_rect.right =(int)Dims.x;
      rc.destination_rect.right = DimX;
		//rc.destination_rect.bottom =(int)Dims.y;
      rc.destination_rect.bottom = DimY;

		rc.clip_rect = rc.destination_rect;

		rc.outline_gamma_curve =
			rc.bitmap_gamma_curve = screen_gamma_curve;

		rc.source_pbox = Bounds;

//				od("dims:%ld, %ld\r\n", dims);

	/* Mono device. */

		rc.is_thumbnail = TRUE;
		rc.is_metafile = TRUE;			/* Yes, this is a metafile. */

		REFRESH_EXTENT re;

		/* Build the refresh extent. */

		re.extent = rc.source_pbox;
		re.refresh_type = REFRESH_ALL;
		re.my_object = NULL;
		re.update_object = NULL;
		re.update_state = NULL;

      DrawObjects(pList, &re, &rc);

		if (hOldAttribPal != NULL)
		{
			SelectPalette(hAttribDC, hOldAttribPal, FALSE);
		}

		::ReleaseDC(NULL, hAttribDC);
	}

	dc.RestoreDC(nSave);
}

// save PrintMaster objects to a file
ERRORCODE SavePMObjectsToFile
(
	const CString& filename,
	CPmwDoc* pDoc,
	BOOL fSelOnly		// save selected object or all objects?
)
{
	CPmwDoc* pTempDoc = GET_PMWAPP()->NewHiddenDocument();
	if (pTempDoc == NULL)
	{
		ASSERT(pTempDoc);
		return ERRORCODE_Fail;
	}

	pTempDoc->size_to_paper();

	ERRORCODE error = ERRORCODE_None;
	CClipboardOpenState OpenState;
	OpenState.m_pDoc = pTempDoc;
	DB_RECORD_NUMBER new_record = 0;
	if ((error = ::CopyObjectsToDatabase(pDoc, &OpenState, &new_record, fSelOnly, TRUE)) == ERRORCODE_None)
	{
		// size the "hidden" document to just fit the objects -- we must
		// set all the objects bounds to the new document size
		PBOX newDocBound;
		pTempDoc->select_all(NULL);
		pTempDoc->GetSelectBound(&newDocBound);

		PMGPageObjectPtr pObject = pTempDoc->selected_objects();
		while(pObject != NULL)
		{
//			PBOX rotbound = pObject->get_bound();
			PBOX unrotbound =((RectPageObject*)pObject)->get_unrotated_bound();
			unrotbound.x0 -= newDocBound.x0;
			unrotbound.x1 -= newDocBound.x0;
			unrotbound.y0 -= newDocBound.y0;
			unrotbound.y1 -= newDocBound.y0;
			((RectPageObject*)pObject)->set_unrotated_bound(unrotbound);
			pObject->calc();

			pObject = pObject->next_selected_object();
		}

		PPNT dims;
		dims.x = newDocBound.x1 - newDocBound.x0;
		dims.y = newDocBound.y1 - newDocBound.y0;
		pTempDoc->set_dimensions(dims);

		if (!pTempDoc->SaveToFile(filename, TRUE))
			error = ERRORCODE_Write;
	}

	OpenState.m_pDoc = NULL;
	pTempDoc->OnCloseDocument();
	return error;
}
