//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/colldrw.CPP 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/colldrw.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 4     2/24/99 1:31p Gbeddow
// fixed bug in regular (non-online) collection building that didn't set
// the thumbnail extents properly
// 
// 3     2/09/99 1:48p Gbeddow
// Support in collection building for creating online collection GIF and
// JPEG thumbnail files, and related changes, per spec from Alexei
// 
// 2     10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 1     10/14/98 12:35p Jayn
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "colldrw.h"
#include "pmwcoll.h"
#include "util.h"

#ifdef NEW_THUMBNAILS
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwtempl.h"
#include "carddoc.h"
#include "cdibdc.h"
#include "utils.h"
#include "grafobj.h"
#include "grafrec.h"
#include "memdev.h"
#endif

extern BOOL standard_check_interrupt(VOIDPTR data);
extern CPalette* pOurPal;     // PrintMaster palette.
extern BYTE far screen_gamma_curve[];

/////////////////////////////////////////////////////////////////////////////
// Utility functions.

static CPmwDoc* OpenDocument(LPCSTR pszName, LPCSTR pszOriginalName=NULL)
{
	return GET_PMWAPP()->OpenHiddenDocument(pszName, pszOriginalName);
}

static CPmwDoc* NewDocument(void)
{
	return GET_PMWAPP()->NewHiddenDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CThumbnailMask

class CThumbnailMask
{
public:
	CThumbnailMask();
	~CThumbnailMask();

	// Make a mask. This assumes 24-bit data.
	BOOL MakeMask(int nWidth, int nHeight, LPBYTE pData, COLORREF clMask);
	// Intersect two masks. The destination is "this".
	void IntersectMask(CThumbnailMask& Mask);

	// Get the mask data.
	LPBYTE GetMaskData(void)
		{ return m_pMaskData; }

	// Free the mask data.
	void FreeMaskData(void);

protected:

	// Member variables.
	int m_nWidth;
	int m_nHeight;
	LPBYTE m_pMaskData;
};

CThumbnailMask::CThumbnailMask()
{
	m_pMaskData = NULL;
}

CThumbnailMask::~CThumbnailMask()
{
	FreeMaskData();
}

void CThumbnailMask::FreeMaskData(void)
{
	delete [] m_pMaskData;
	m_pMaskData = NULL;
}

// Make a mask. This assumes 24-bit data.
BOOL CThumbnailMask::MakeMask(int nWidth, int nHeight, LPBYTE pData, COLORREF clMask)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	int nCount = nWidth*nHeight;
	FreeMaskData();

	if (nWidth == 0 || nHeight == 0)
	{
		// Bad dimensions.
		ASSERT(FALSE);
		return FALSE;
	}
	TRY
	{
		m_pMaskData = new BYTE[nCount];
	}
	CATCH_ALL(e)
	{
		// Failed to allocate.
		return FALSE;
	}
	END_CATCH_ALL

	// Compute the size of a source row and the amount to add for the next row.
	int nSourceByteWidth = Util::RoundToDWORD(nWidth*3);
	int nSourceDelta = nSourceByteWidth-(nWidth*3);

	LPBYTE pSource = pData;
	LPBYTE pDest = m_pMaskData;
	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		for (int nByte = 0; nByte < nWidth; nByte++)
		{
			*pDest++ = (RGB(pSource[2], pSource[1], pSource[0]) == clMask) ? 255 : 0;
			pSource += 3;
		}
		pSource += nSourceDelta;
	}
	return TRUE;
}

// Intersect two masks. The destination is "this".
void CThumbnailMask::IntersectMask(CThumbnailMask& Mask)
{
	ASSERT(Mask.m_nWidth == m_nWidth);
	ASSERT(Mask.m_nHeight == m_nHeight);

	int nCount = m_nWidth*m_nHeight;
	LPBYTE pThis = GetMaskData();
	LPBYTE pThat = Mask.GetMaskData();
	for (int n = 0; n < nCount; n++)
	{
		*pThis++ &= *pThat++;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPMCollectionDraw

CPMCollectionDraw::CPMCollectionDraw()
{
	m_pDoc = NULL;
	m_pRefreshExtent = NULL;
	m_pPathManager = NULL;
}

CPMCollectionDraw::~CPMCollectionDraw()
{
   CloseDocument();
	if (m_pPathManager != NULL)
	{
		m_pPathManager->Release();
		m_pPathManager = NULL;
	}
}

void CPMCollectionDraw::Init(IPathManager* pPathManager)
{
	m_pPathManager = pPathManager;
	ASSERT(m_pPathManager != NULL);
	if (m_pPathManager != NULL)
	{
		m_pPathManager->AddRef();
	}
}

void CPMCollectionDraw::CloseDocument(void)
{
	if (m_pDoc != NULL)
	{
		m_pDoc->OnCloseDocument();
		m_pDoc = NULL;
	}
}

void CPMCollectionDraw::SetDocument(CPmwDoc *pDoc, LPCSTR pszFileName)
{
	CloseDocument();
	m_csFileName = pszFileName;
	m_pDoc = pDoc;
}

int CPMCollectionDraw::DrawDocument(CCollectionDrawContext &DrawContext, CPMWCollectionData* pClientCD)
{
	int nDrawType = DrawContext.m_nDrawType;

	// Only know how to deal with these two right now.
	if (nDrawType != CCollectionDrawContext::Thumbnail
		 && nDrawType != CCollectionDrawContext::View)
	{
		// Not a type we know about.
		ASSERT(FALSE);
		return ERRORCODE_IntError;
	}

	BOOL bCreateThumbnail = (nDrawType == CCollectionDrawContext::Thumbnail);
	BOOL bDrawPreview = bCreateThumbnail;

	// arbitrary colors used to determine transparency (if applicable)
// REVIEW GCB 2/5/99 - in case this is a JPEG clipped photo ending up in an online JPEG thumbnail file,
// set clrFirstTrans to use a white background for the clipped photo rather than a red one; ideally we'd
// render a clipped JPEG as a GIF with transparent color instead but I'm leaving that as a task for later
#if 0
	COLORREF clrFirstTrans = RGB(255,0,0);
#else
	COLORREF clrFirstTrans = RGB(255,255,255);
#endif
	COLORREF clrSecondTrans = RGB(255,255,0);

   int nRetVal;
   CRect crDest;
	REFRESH_EXTENT* pRefEx;

	switch (nDrawType)
	{
		case CCollectionDrawContext::Thumbnail:
		{
			crDest = CRect(CPoint(0, 0), DrawContext.m_Thumbnail.m_czPreviewDims);
			ASSERT(crDest.right != 0 && crDest.bottom != 0);
			pRefEx = NULL;
			break;
		}
		case CCollectionDrawContext::View:
		{
			ASSERT(DrawContext.m_View.m_pDestRect != NULL);
			crDest = *(DrawContext.m_View.m_pDestRect);
			pRefEx = DrawContext.m_View.m_pRefreshExtent;
			break;
		}
	}

   CPmwDoc *pDoc = GetDocument();

   // Setup the bound.
   CSize sizeOfPreview;
   PBOX  Bound;
   PPNT  Dims;

	pDoc->GetPreviewInfo(crDest, bDrawPreview, sizeOfPreview, Bound);
   int   nWidth = sizeOfPreview.cx, nHeight = sizeOfPreview.cy;
   Dims.x = Bound.x1 - Bound.x0;
   Dims.y = Bound.y1 - Bound.y0;

   int nBits;

   CPalette* pPalette = pOurPal;

   CDibDC DibDC;
   CPalette* pOldPalette = NULL;
   LPBYTE lpDib = NULL;
   BOOL fDrawn = FALSE;
   BOOL fHaveADC = FALSE;
   DWORD dwHeaderSize = sizeof(BITMAPINFO);     // 24-bit

   CRect crBounds;
//   if(crDest.Width() == GetPreviewDims().cx && crDest.Height() == GetPreviewDims().cy)
	if (bCreateThumbnail)
      crBounds = CRect(0, 0, nWidth, nHeight);
   else
   {
      crBounds = CRect(
         crDest.left + ((crDest.Width() - nWidth)/2),
         crDest.top + ((crDest.Height() - nHeight)/2),
         0,
         0);
      crBounds.right = crBounds.left + nWidth;
      crBounds.bottom = crBounds.top + nHeight;

   }

	// Construct the redisplay context now to correct any round-off problems
	// in the mapping.

	RedisplayContext rc;

	switch (nDrawType)
	{
		case CCollectionDrawContext::Thumbnail:
		{
			// Get a screen DC to get the resolution.
			HDC hDC = ::GetDC(NULL);
			if (hDC != NULL)
			{
				rc.x_resolution = ::GetDeviceCaps(hDC, LOGPIXELSX);
				rc.y_resolution = ::GetDeviceCaps(hDC, LOGPIXELSY);
				::ReleaseDC(NULL, hDC);
			}
			else
			{
				// Default to something.
				rc.x_resolution = rc.y_resolution = 96;
			}
			break;
		}
		case CCollectionDrawContext::View:
		{
			// Use the resolution of the passed DC.
			rc.x_resolution = DrawContext.m_View.m_pDC->GetDeviceCaps(LOGPIXELSX);
			rc.y_resolution = DrawContext.m_View.m_pDC->GetDeviceCaps(LOGPIXELSY);
			break;
		}
	}

	rc.scaled_source_x0 = Bound.x0*rc.x_resolution;
	rc.scaled_source_y0 = Bound.y0*rc.y_resolution;

	rc.destination_x0 = crBounds.left;
	rc.destination_y0 = crBounds.top;

	rc.destination_rect = crBounds;

	rc.clip_rect = rc.destination_rect;
	rc.clip_region = (nDrawType == CCollectionDrawContext::View)
		? DrawContext.m_View.m_hrgnClip
		: NULL;

	rc.outline_gamma_curve =
		rc.bitmap_gamma_curve = screen_gamma_curve;

	rc.source_pbox = Bound;

	rc.is_thumbnail = TRUE;

	PCOORD Scale;
	int n;
	if (Dims.x > Dims.y)
	{
		Scale = scale_pcoord(Dims.x, rc.x_resolution, PAGE_RESOLUTION);
		n = crBounds.Width();
	}
	else
	{
		Scale = scale_pcoord(Dims.y, rc.y_resolution, PAGE_RESOLUTION);
		n = crBounds.Height();
	}

	while (Scale > 32000)
	{
		n >>= 1;
		Scale >>= 1;
	}
	if (n == 0)
	{
		n++;
	}

	rc.SetScaling(n, (USHORT)Scale);

   TRY
   {
      nRetVal = ERRORCODE_Fail;

      LPBITMAPINFO pbi = NULL;
      DWORD dwBytes = 0;
		if (nDrawType == CCollectionDrawContext::Thumbnail)
		{
			// Compute the real dims after being passed through the rc.
			PBOX pb = rc.source_pbox;
			rc.pbox_to_screen(&pb, TRUE);
			nWidth = pb.x1 - pb.x0;
			nHeight = pb.y1 - pb.y0;
			crBounds.right = crBounds.left + nWidth;
			crBounds.bottom = crBounds.top + nHeight;

         dwBytes = (DWORD)((nWidth*3 + 3) & ~3) * (DWORD)nHeight;    // 24-bit
         lpDib = new BYTE[dwHeaderSize];
         pbi = (LPBITMAPINFO)lpDib;

         memset(&pbi->bmiHeader, 0, sizeof(BITMAPINFOHEADER));

         pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
         pbi->bmiHeader.biWidth = nWidth;
         pbi->bmiHeader.biHeight = nHeight;
         pbi->bmiHeader.biPlanes = 1;
         pbi->bmiHeader.biBitCount = 24;
         pbi->bmiHeader.biCompression = BI_RGB;
         pbi->bmiHeader.biSizeImage = dwBytes;

         nBits = pbi->bmiHeader.biBitCount;

         fHaveADC = DibDC.Create(pbi);
      }
      else
		{
         fHaveADC = DibDC.Attach(DrawContext.m_View.m_pDC->GetSafeHdc());
		}

      if (fHaveADC)
      {
         if ((pOldPalette = DibDC.SelectPalette(pPalette, FALSE)) != NULL)
            DibDC.RealizePalette();

			// If we're doing background fill, fill the bitmap with background color,
			// unless we've been interrupted.
			if (!DrawContext.m_fDrawTransparent && (pRefEx == NULL || pRefEx->update_object == NULL))
			{
				CBrush brush;
				brush.CreateSolidBrush(DrawContext.m_clrFillColor);
				DibDC.FillRect(&crBounds, &brush);
         }
			else if (DrawContext.m_fDrawTransparent && bCreateThumbnail)
			{
				// Fill with initial "transparent" color. This graphic may not be
				// transparent, in which case this background will not be seen anyway.
				CBrush brush;
				brush.CreateSolidBrush(clrFirstTrans);
				DibDC.FillRect(&crBounds, &brush);
			}

         // Draw the preview.

         rc.destination_hdc = DibDC.GetSafeHdc();

         rc.hwnd = NULL;
			switch (nDrawType)
			{
				case CCollectionDrawContext::Thumbnail:
				{
					rc.info.bpp = pbi->bmiHeader.biBitCount;
					rc.info.planes = pbi->bmiHeader.biPlanes;
					ASSERT(rc.info.planes == 1);
					switch (rc.info.bpp)
					{
						case 1:
						{
							rc.info.type = RASTER_TYPE_BILEVEL;
							break;
						}
						case 3:
						case 4:
						{
							rc.info.type = RASTER_TYPE_8;
							break;
						}
						case 8:
						{
							rc.info.type = RASTER_TYPE_256;
							break;
						}
						default:
						case 24:
						{
							rc.info.type = RASTER_TYPE_24;
							rc.info.bpp = 24;
							rc.info.planes = 1;
							break;
						}
					}
					break;
				}
				case CCollectionDrawContext::View:
				{
					// Figure out color depth from the destination DC.
					rc.set_info(rc.destination_hdc);
					break;
				}
			}

      //    od("dims:%ld, %ld\r\n", dims);

         REFRESH_EXTENT re;

         // Build the refresh extent.
         re.extent = rc.source_pbox;
         re.refresh_type = REFRESH_ALL;
         re.my_object = NULL;

         if (pRefEx != NULL)
         {
            // if we have a refresh extent, we want to allow interrupting.
            // set the extent with stuff from the doc's extent
            re.update_object = pRefEx->update_object;
            re.update_state = pRefEx->update_state;
            re.did_erase = pRefEx->did_erase;

            // Set the function which will be called to check for interrupts.
            rc.set_check_interrupt(standard_check_interrupt, (VOIDPTR)&rc);
			}
         else
         {
            re.update_object = NULL;
            re.update_state = NULL;
         }

         if (bDrawPreview)
            fDrawn = !pDoc->DrawPreview(&re, &rc);
         else
            fDrawn = !pDoc->draw_objects(&re, &rc);

			LPBYTE pTransMaskData = NULL;
			BOOL fIsTransparent = FALSE;

			if (DrawContext.m_fDrawTransparent && bCreateThumbnail)
			{
				fIsTransparent = TRUE;

				// Check if this is a "transparent" graphic. First check if extension
				// is "pmo", since these aren't handled by GraphicObject
				CString filename = GetFileName();
				int nArtType = type_of_art_extension(filename);
				if (nArtType != GRAPHIC_TYPE_PMO &&
					 nArtType != GRAPHIC_TYPE_DPP && 
					 nArtType != GRAPHIC_TYPE_UNKNOWN)
				{
					// create a graphic object to check for transparency
					GRAPHIC_CREATE_STRUCT gcs;
					gcs.m_csFileName = filename;
					gcs.proof = FALSE;
					GraphicObjectPtr object = pDoc->get_database()->create_graphic_object(&gcs);
					if (object)
					{
						fIsTransparent = !object->is_opaque(&rc);
						object->destroy();
						delete object;
					}
				}
			}

			// The mask. If never initialized, then Mask.GetMaskData() will be NULL.
			CThumbnailMask Mask;

			if (fDrawn && fIsTransparent)
			{
				// create mask of first bitmap "transparency"

				if (Mask.MakeMask(crBounds.Width(),
										 crBounds.Height(),
										 DibDC.GetData(),
										 clrFirstTrans))
				{
					// ok, now draw the image again with a different background color
					CBrush brush;
					brush.CreateSolidBrush(clrSecondTrans);
					DibDC.FillRect(&crBounds, &brush);
					fDrawn = !pDoc->draw_objects(&re, &rc);

					CThumbnailMask SecondMask;
					if (SecondMask.MakeMask(crBounds.Width(),
													crBounds.Height(),
													DibDC.GetData(),
													clrSecondTrans))
					{
						Mask.IntersectMask(SecondMask);
					}
					else
					{
						// Free the mask data from the first pass.
						Mask.FreeMaskData();
					}
				}
			}

         if (!fDrawn)
         {
            if (re.update_object != NULL)
            {
               if (pRefEx != NULL)
               {
                  pRefEx->update_object = re.update_object;
                  pRefEx->update_state = re.update_state;
                  pRefEx->did_erase = re.did_erase;
               }
               else
					{
                  delete re.update_state;
					}
            }
         }
         else if (pRefEx)
         {
            // we're done drawing, so reset the refresh extent
            pRefEx->update_object = NULL;
            pRefEx->update_state = NULL;
         }

         // End of preview draw.

         if (fDrawn)
         {
				if (nDrawType == CCollectionDrawContext::Thumbnail)
            {
					CThumbnailDataEntry* pThumbnailData = DrawContext.m_Thumbnail.m_pThumbnailData;
					if (pThumbnailData != NULL)
					{
						// We have the bits!
#if defined(WIN32) && !defined(_MAC)
						LPBYTE pDIBData = DibDC.GetData();
#else
						BYTE __huge* pDIBData = ((BYTE __huge*)lpDib) + dwHeaderSize;
#endif

						// GCB 2/5/99 - support for online thumbnail file creation
						CString csOnlineThumbnailFile;
						if (pClientCD && !pClientCD->m_strOnlineThumbnailDir.IsEmpty())
						{
							csOnlineThumbnailFile = pClientCD->m_strOnlineThumbnailDir;
							if (pClientCD->m_strRelativeFileName[0] == '\\')
								Util::RemoveBackslashFromPath(csOnlineThumbnailFile);
							else
								Util::AppendBackslashToPath(csOnlineThumbnailFile);
							CString filename;
							if (pClientCD->m_fAddSubdirectory)
								filename = pClientCD->m_strRelativeFileName;
							else
							{
								int index = pClientCD->m_strRelativeFileName.ReverseFind( '\\' );
								if (index < 0)
									index = -1;
								filename = pClientCD->m_strRelativeFileName.Right( pClientCD->m_strRelativeFileName.GetLength() - index - 1 );
								Util::AppendBackslashToPath(csOnlineThumbnailFile);
							}
							csOnlineThumbnailFile += filename;
						}

						pThumbnailData->SetThumbnailData(pbi, pDIBData, Mask.GetMaskData(),
																	nWidth, nHeight,
																	nBits, dwBytes,
																	csOnlineThumbnailFile);
					}
					else
					{
						// No thumbnail data to attach to!
						ASSERT(FALSE);
					}
            }
            nRetVal = ERRORCODE_None;
         }

         if (pOldPalette != NULL)
            DibDC.SelectPalette(pOldPalette, FALSE);
      }
      else
      {
         ASSERT(FALSE);
//       AfxMessageBox("Couldn't create DC!");
      }
   }
   CATCH(CMemoryException, e)
   {
      nRetVal = ERRORCODE_Memory;
   }
   AND_CATCH_ALL(e)
   {
      nRetVal = ERRORCODE_Fail;
   }
   END_CATCH_ALL

	switch (nDrawType)
	{
		case CCollectionDrawContext::Thumbnail:
		{
			DibDC.DeleteDC();
			if (lpDib != NULL)
				delete [] lpDib;
			break;
		}
		case CCollectionDrawContext::View:
		{
			DibDC.Detach();
			break;
		}
	}

   return nRetVal;
}

int CPMCollectionDraw::CreateTempProjectFile(LPCSTR pszFileName,
															StorageDevice *pSource,
															TmpFile *pDestFile)
{
	ASSERT(pSource != NULL);
	ASSERT(pDestFile != NULL);

	CString csTempFileName = m_pPathManager->LocatePath(pszFileName);

	CString csExtension;
	Util::SplitPath(csTempFileName,
						 NULL,            // no drive
						 NULL,            // no directory
						 NULL,            // no base name
						 &csExtension);   // What we want!

	// Generate a nidded filename with the correct extension.
	char cbBuffer[30] = "_xxx";
	GET_PMWAPP()->nid_filename(cbBuffer);
//	strcat(cbBuffer, csExtension);
	pDestFile->GenerateTempName(cbBuffer);

	// Copy from memory to temp file
	int error;
	if ((error = copy_file(pSource, pDestFile)) == ERRORCODE_None)
	{
		// Close the file so it can be opened again later.
		pDestFile->flush();
	}
	else
	{
		// Get rid of the bad file.
		pDestFile->zap();
	}

	return error;
}

int CPMCollectionDraw::Draw(int nType,
									 LPCSTR pszContentFileName,
									 CDC *pDC, CRect &crDest, HRGN hrgnClip,
									 COLORREF clrFillColor,
									 REFRESH_EXTENT* pRefreshExtent /*=NULL*/)
{
	int	nRetVal = ERRORCODE_Fail;

	CCollectionDrawContext DrawContext;

	// Setup to view.
	DrawContext.SetView(pDC, &crDest, hrgnClip, pRefreshExtent);
	DrawContext.m_clrFillColor = clrFillColor;

	switch (nType)
	{
		case CPMWCollection::typeProjects:
		{
			nRetVal = DrawProject(pszContentFileName,
										 NULL,					// No Original filename needed
										 DrawContext);
			break;
		}
		case CPMWCollection::typeArt:
		{
			nRetVal = DrawPicture(pszContentFileName,
										 NULL,                // No Source Device
										 DrawContext,
										 NULL);
			break;
		}
		case CPMWCollection::typeSentiments:
		{
			break;
		}
		default:
		{
			// What type is this?
			ASSERT(FALSE);
			break;
		}
	}

   return nRetVal;
}

// Public method to allow client to open document separately from doc. draw
int CPMCollectionDraw::CreateDocument(LPCSTR pszContentFileName, int nType)
{
	int nRetVal = ERRORCODE_Fail;

	switch (nType)
	{
		case CPMWCollection::typeProjects:
		{
			nRetVal = GetProjectDocument(pszContentFileName);
			break;
		}
		case CPMWCollection::typeArt:
		{
			nRetVal = GetPictureDocument(pszContentFileName);
			break;
		}
		default:
		{
			// What type is this?
			ASSERT(FALSE);
			nRetVal = ERRORCODE_BadParameter;
			break;
		}
	}
	return nRetVal;
}

int CPMCollectionDraw::DrawProject(LPCSTR pszContentFileName,
											  LPCSTR	pszOriginalFileName,
											  CCollectionDrawContext& DrawContext)
{
	int nRetVal = GetProjectDocument(pszContentFileName, pszOriginalFileName);

   if (nRetVal != ERRORCODE_None || GetDocument() == NULL)
   {
      nRetVal = ERRORCODE_Open;
   }
   else
   {
		// Fix the fill color.
		if (DrawContext.m_clrFillColor == UNSPECIFIED_COLOR)
			DrawContext.m_clrFillColor = RGB(255, 255, 255);

		// Draw the document.
		nRetVal = DrawDocument(DrawContext, NULL);
	}
   return nRetVal;
}

int CPMCollectionDraw::GetProjectDocument(LPCSTR pszContentFileName,
														LPCSTR pszOriginalFileName /*=NULL*/)
{
   int            nRetVal;
   TmpFile        tempFile;
   StorageFile    sourceFile;
   BOOL           bCreatedTempFile=FALSE;

   CPmwDoc* pDoc = GetDocument();

   // If we were not supplied an existing document or
   // the exising document does not match the one we want to draw, create one.
   if (pDoc == NULL || strcmp(GetFileName(), pszContentFileName) != 0)
	{
		CString csDocToOpen = pszContentFileName;

		// If we are NOT creating a Thumbnail, pstrOriginalFileName 
		// will be NULL, so go get Document from content file and put
		// in a temp file.

		if (pszOriginalFileName == NULL)
		{
			sourceFile.set_name(pszContentFileName);
			nRetVal = CreateTempProjectFile(pszContentFileName,
													  &sourceFile, &tempFile);
			if(nRetVal != ERRORCODE_None)
				return nRetVal;
			csDocToOpen = tempFile.get_name();
			bCreatedTempFile=TRUE;
		}

		// We now have a document to open.
		// We need pstrOriginalFileName since the temp file does not retain
		// the original documents file extension and we need to pass the
		// original filename on to open document so can identify it.
		pDoc = OpenDocument(csDocToOpen,
								  pszOriginalFileName == NULL
										? pszContentFileName
										: pszOriginalFileName);

		if (pDoc == NULL && pszOriginalFileName != NULL)
		{
			// Thumbnail creation must not fail. This causes the
			// thumbnail database to get out of sync with the other
			// databases. So we just draw from a blank document.
			pDoc = NewDocument();
			if (pDoc != NULL)
			{
				pDoc->size_to_paper();
			}
		}

		// If we're drawing a project for the first time and we're
		// not drawing a thumbnail, set the default panel for the
		// document since we will be using draw_objects
		if (pszOriginalFileName == NULL && pDoc != NULL)
			pDoc->GoToFirstPage();

		SetDocument(pDoc, pszContentFileName);
		nRetVal = (pDoc == NULL) ? ERRORCODE_Open : ERRORCODE_None;
	}
	else
		nRetVal = ERRORCODE_None;

	return nRetVal;
}

int CPMCollectionDraw::GetPictureDocument(LPCSTR pszContentFileName,
														StorageDevice* pSource /*=NULL*/)
{
   int nRetVal = ERRORCODE_Fail;

   CPmwDoc* pDoc = GetDocument();

   // If we were not supplied an existing document or
   // the exising document doesn't match the one we want to draw, create one
   if (pDoc == NULL || strcmp(GetFileName(), pszContentFileName) != 0)
   {
		// See if we have a "PMObjects" file, since it has a document inside
		// Create a blank document to use.
		pDoc = NewDocument();
		if (pDoc != NULL)
		{
			pDoc->size_to_paper();
		}

		SetDocument(pDoc, pszContentFileName);
   }

   if (pDoc == NULL)
   {
      nRetVal = ERRORCODE_Fail;
   }
   else 
   {
      // If we do not have a picture attached, get it and attach to document.
		if (pDoc->objects() == NULL)
      {
         //
         // Create the graphic to draw in the preview.
         //

         // Get the database to use for everything.
         PMGDatabase* pDatabase = pDoc->get_database();

         // Create the graphic.

         GRAPHIC_CREATE_STRUCT* pGCS = pDoc->get_gcs();
			pDoc->SetImportPictureName(pszContentFileName);
         pGCS->proof = TRUE;
         pGCS->embed_source = TRUE;
         pGCS->pSourceDevice = pSource;

			if ((nRetVal = pDoc->create_chosen_picture()) == ERRORCODE_None)
			{
				// Size the document. There may be multiple objects if a PMO
				// file was selected.
				// Compute the bound of all the objects.
				pDoc->select_all();
				if (pDoc->selected_objects() != NULL)
				{
					PBOX Bound;
					pDoc->GetSelectBound(&Bound);
					PMGPageObjectPtr pObject = pDoc->selected_objects();
					while (pObject != NULL)
					{
						PBOX unrotbound = ((RectPageObject*)pObject)->get_unrotated_bound();
						unrotbound.x0 -= Bound.x0;
						unrotbound.x1 -= Bound.x0;
						unrotbound.y0 -= Bound.y0;
						unrotbound.y1 -= Bound.y0;
						((RectPageObject*)pObject)->set_unrotated_bound(unrotbound);
						pObject->calc();

						pObject = pObject->next_selected_object();
					}

					PPNT dims;
					dims.x = Bound.x1 - Bound.x0;
					dims.y = Bound.y1 - Bound.y0;
					pDoc->set_dimensions(dims);

					pDoc->deselect_all();
				}
			}
      }
      else
         nRetVal = ERRORCODE_None;
   }

   return nRetVal;
}

int CPMCollectionDraw::DrawPicture(LPCSTR pszContentFileName,
											  StorageDevice* pSource,
											  CCollectionDrawContext& DrawContext,
											  CPMWCollectionData* pClientCD)
{
	int nRetVal;

	// We'll assume that the graphic is transparent for now, unless there's
	// a specific fill color sent in. The test for transparency happens later,
	// in DrawDocumentPreview()
	DrawContext.m_fDrawTransparent = (DrawContext.m_clrFillColor == UNSPECIFIED_COLOR);

	nRetVal = GetPictureDocument(pszContentFileName, pSource);
   if(nRetVal == ERRORCODE_None)
   {
      // Draw this document.
		nRetVal = DrawDocument(DrawContext, pClientCD);
   }
   else if (DrawContext.m_nDrawType == CCollectionDrawContext::Thumbnail
				&& GetDocument() != NULL)
   {
      extern BOOL fWantDebugMessages;

      // Drawing preview. Do the draw anyway..
      nRetVal = DrawDocument(DrawContext, pClientCD);

      if (fWantDebugMessages)
      {
         // Record that something bad took place.
			FILE* fp = fopen(m_pPathManager->ExpandPath("badpict.log"), "a+");
			if (fp != NULL)
			{
				fprintf(fp, "Bad picture '%s'\n", pszContentFileName);
				fclose(fp);
			}
      }
   }
   return nRetVal;
}

int CPMCollectionDraw::BuildPictureThumbnail(CPMWCollectionData* pClientCD, CThumbnailDataEntry* pThumbnailData)
{
   // Possible device to use for stream data.
   MemoryDevice MemDev;
   // The source device used if the memory device is used.
   StorageDevice* pSource = NULL;

   ASSERT(pClientCD != NULL);
   if (pClientCD->m_ContentData.GetData() != NULL)
   {
      // Get a pointer to the content stream data.
      LPVOID pData = pClientCD->m_ContentData.GetData();
      DWORD dwSize = pClientCD->m_ContentData.GetDataSize();

      // Set our memory device to use this memory chunk.
      MemDev.AddPiece(pData, dwSize);

      // Our source is the memory device.
      pSource = &MemDev;
   }
	CString csFileName = pClientCD->m_strFullFileName.IsEmpty()
					? pClientCD->m_strContentFileName
					: pClientCD->m_strFullFileName;

	// Set the draw context for a thumbnail.
	CCollectionDrawContext DrawContext;
	DrawContext.SetThumbnail(pThumbnailData, CSize(pClientCD->m_dwThumbnailExtents, pClientCD->m_dwThumbnailExtents));

   return DrawPicture(csFileName, pSource, DrawContext, pClientCD);
}

int CPMCollectionDraw::BuildProjectThumbnail(CPMWCollectionData* pClientCD, CThumbnailDataEntry* pThumbnailData)
{
   int error;

   // We want to open the document.
   // The document may exist in a separate file, or it may exist
   // in the content stream.

   TmpFile        File;
   MemoryDevice   MemDev;
   CString        csName;
   BOOL           fUsingFile;

   ASSERT(pClientCD != NULL);
   if (pClientCD->m_ContentData.GetData() == NULL)
   {
      // No content stream. Get the file from disk.
		csName = pClientCD->m_strFullFileName.IsEmpty()
						? pClientCD->m_strContentFileName
						: pClientCD->m_strFullFileName;
		csName = m_pPathManager->ExpandPath(csName);
      fUsingFile = FALSE;
   }
   else
   {
      // Get a pointer to the content stream data.
      LPVOID pData = pClientCD->m_ContentData.GetData();
      DWORD dwSize = pClientCD->m_ContentData.GetDataSize();

      // Set our memory device to use this memory chunk.
      MemDev.AddPiece(pData, dwSize);

      error = CreateTempProjectFile(pClientCD->m_strContentFileName,
         &MemDev, &File);

      if(error != ERRORCODE_None)
         return error;

      // Get the full file name.
      csName = File.get_name();
      fUsingFile = TRUE;
   }

	// Set the draw context for a thumbnail.
	CCollectionDrawContext DrawContext;
	DrawContext.SetThumbnail(pThumbnailData, CSize(160, 160));

	return DrawProject(csName, pClientCD->m_strContentFileName, DrawContext);
}

//
// Implementation of interface function BuildThumbnail.
//

int CPMCollectionDraw::BuildThumbnail(int nType, CPMWCollectionData* pData, CThumbnailDataEntry* pThumbnailData)
{
	int nRetVal;
	switch (nType)
	{
		case CPMWCollection::typeProjects:
		{
			nRetVal = BuildProjectThumbnail(pData, pThumbnailData);
			break;
		}
		case CPMWCollection::typeArt:
		{
			nRetVal = BuildPictureThumbnail(pData, pThumbnailData);
			break;
		}
		case CPMWCollection::typeSentiments:
		{
			// Do sentiments have thumbnails?
			return ERRORCODE_None;
		}
		default:
		{
			// What type is this?
			ASSERT(FALSE);
			nRetVal = ERRORCODE_BadParameter;
			break;
		}
	}

	// Make sure the document is closed.
	CloseDocument();

	return nRetVal;
}

/////////////////////////////////////////////////////////////////////////////
// Interface map for CPMCollectionDraw

BEGIN_INTERFACE_MAP(CPMCollectionDraw, CCmdTarget)
	INTERFACE_PART(CPMCollectionDraw, IID_ICollectionDraw, CollectionDraw)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IUnknown interface for CPMCollectionDraw.
// This is standard stuff brought over from the docs.

STDMETHODIMP_(ULONG) CPMCollectionDraw::XCollectionDraw::AddRef()
{
	METHOD_PROLOGUE(CPMCollectionDraw, CollectionDraw)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CPMCollectionDraw::XCollectionDraw::Release()
{
	METHOD_PROLOGUE(CPMCollectionDraw, CollectionDraw)
	return pThis->ExternalRelease();
}

STDMETHODIMP CPMCollectionDraw::XCollectionDraw::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{
	METHOD_PROLOGUE(CPMCollectionDraw, CollectionDraw)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
// Implementations for the ICollectionDraw interface for CPMCollectionDraw.

//
// Build a thumbnail.
//

STDMETHODIMP_(int) CPMCollectionDraw::XCollectionDraw::BuildThumbnail(int nType, CPMWCollectionData* pData, CThumbnailDataEntry* pThumbnailData)
{
	METHOD_PROLOGUE(CPMCollectionDraw, CollectionDraw)
	return pThis->BuildThumbnail(nType, pData, pThumbnailData);
}
