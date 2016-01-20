/*
// $Workfile: FILEITEM.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/FILEITEM.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
//    Rev 1.0   14 Aug 1997 15:20:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:26   Fred
// Initial revision.
// 
//    Rev 1.19   10 Oct 1996 14:31:22   Jay
// More dynamic graphic/item updating.
// 
//    Rev 1.18   16 Sep 1996 14:11:16   Jay
// Totem support.
// 
//    Rev 1.17   08 Sep 1996 16:54:32   Jay
// Fixed a bug I introduced last time.
// 
//    Rev 1.16   08 Sep 1996 16:32:08   Jay
// An item's category must be merged for the item to be considered licensed.
// 
//    Rev 1.15   08 Sep 1996 07:52:38   Jay
// Fixed error in IsLicensed.
// 
//    Rev 1.14   07 Sep 1996 10:50:18   Jay
// New Search Stuff
// 
//    Rev 1.13   03 Sep 1996 15:08:26   Jay
// Free pictures not licensed if not registered.
// 
//    Rev 1.12   03 Sep 1996 14:00:34   Jay
// Thumbnail vs palette parameter rename bug.
// 
//    Rev 1.11   01 Sep 1996 12:51:42   Jay
// Removes all keywords in OnDestroy.
// 
//    Rev 1.10   01 Sep 1996 12:42:14   Jay
// CreateThumbNail not ifdef'd out!
// 
//    Rev 1.9   01 Sep 1996 09:22:46   Jay
//  
// 
//    Rev 1.8   30 Aug 1996 10:45:22   Jay
// Keyword/item stuff
// 
//    Rev 1.7   25 Aug 1996 12:57:22   Jay
// New merging schemes.
// 
//    Rev 1.6   21 Aug 1996 16:38:06   Jay
// Calls DeleteDC() on the DibDC.
// Update the friendly name in Update().
// 
//    Rev 1.5   14 Aug 1996 15:28:02   Jay
// More project album stuff.
// 
//    Rev 1.4   09 Aug 1996 16:07:14   Jay
// 16-bit code is limited to 8-bit DIBs.
// 
//    Rev 1.3   09 Aug 1996 15:24:46   Jay
// 16-bit changes.
// 
//    Rev 1.2   08 Aug 1996 18:10:24   Jay
// JPEG, etc!
// 
//    Rev 1.1   07 Aug 1996 17:50:08   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:33:30   Jay
// Initial revision.
// 
//    Rev 1.1   13 Mar 1996 15:00:00   VSI
// _MAC specific changes just to compile -- not working on Mac yet.
// 
//    Rev 1.0   16 Feb 1996 12:28:30   FRED
// Initial revision.
// 
//    Rev 1.11   18 Oct 1995 15:59:56   FRED
// Fixes for 16 bit version after 32 bit changes
// 
//    Rev 1.10   21 Sep 1995 13:47:18   JAY
// Changes to CDibDC
// 
//    Rev 1.9   15 Sep 1995 14:05:10   JAY
// More 32-bit changes.
// 
//    Rev 1.8   16 Aug 1995 08:12:48   JAY
// Blanks out bitmap before drawing thumbnail. This prevents junk.
// 
//    Rev 1.7   04 Aug 1995 15:19:14   JAY
// Preview updating, etc.
// 
//    Rev 1.6   17 Jul 1995 15:10:56   JAY
// Some extra buffering.
// 
//    Rev 1.5   26 Jun 1995 16:58:48   JAY
//  
// 
//    Rev 1.4   19 Jun 1995 17:19:16   JAY
// Item extensions
// 
//    Rev 1.3   16 Jun 1995 13:00:32   JAY
//  
// 
//    Rev 1.2   07 Jun 1995 15:39:58   JAY
//  
// 
//    Rev 1.1   06 Jun 1995 16:55:06   JAY
// Project previews in the album.
// 
//    Rev 1.0   06 Jun 1995 10:13:18   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmw.h"

#include "cmanalb.h"				// For embedded file support, etc.
#include "cbkitem.h"
#include "fileitem.h"
#include "cdrawcon.h"
#include "efiledat.h"
#include "thumbrec.h"
#include "util.h"
#include "cdibdc.h"
#include "redisp.h"
#include "keyword.h"
#include "booktest.h"
#include "itemprox.h"
#include "compfn.h"

#ifndef WIN32
#include <malloc.h>
#endif

extern const RGBQUAD* GetIdentityPalette(void);

/////////////////////////////////////////////////////////////////////////////
// CFileItemCreationData

CFileItemCreationData::CFileItemCreationData()
{
	m_pFileName = NULL;
	m_pFriendlyName = NULL;
	m_fEmbed = FALSE;
	m_fCreateThumbNail = FALSE;
	m_dwFlags = 0;
	m_idParentCategory = NULL_DATAID;
}

//////////////////////////////////////////////////////////////////////////
// CFileItemData

// Construct a CFileItemData object.

CFileItemData::CFileItemData()
{
	memset(&m_Record, 0, sizeof(m_Record));
	m_Record.m_wFileType = FILE_TYPE_Generic;
	m_fUpdating = FALSE;
	m_pThumbNailDIB = NULL;
	m_fLicensed = -1;			// We just don't know yet.
}

// Destroy a CFileItemData object.
CFileItemData::~CFileItemData()
{
} 

void CFileItemData::Draw(CDrawContext* pContext, const CRect& crContent, int nSubItem)
{
	ASSERT(pContext != NULL);
	ASSERT(nSubItem == 0);

	m_pDrawingContext = pContext;			// For breathing.

	CDC* pDC = pContext->GetDC();

	int nContext;
	if ((nContext = pDC->SaveDC()) != -1)
	{
		BOOL fDrawn = FALSE;

	/*
	// If we are instructed to use thumbnails, make our best effort to do so.
	*/
		BOOL fUseThumbNails = pContext->UseThumbNails();
		int nExists = -1;

		if (!fUseThumbNails)
		{
			IPathManager* pPathManager = GetGlobalPathManager();
			BOOL fOldPrompt = pPathManager->SetPromptForContent(FALSE);
			nExists = pPathManager->FileExists(FileName(), TRUE);
			pPathManager->SetPromptForContent(fOldPrompt);

			if (!nExists)
			{
				fUseThumbNails = TRUE;
			}
		}

		if (fUseThumbNails && m_Record.m_idThumbNail != NULL_DATAID)
		{
		/* We want to try and draw our thumbnail. */

			CThumbNailData* pThumbNail;

			if ((pThumbNail = (CThumbNailData*)m_pManager->GetRecord(m_Record.m_idThumbNail)) == NULL)
			{
//				ASSERT(FALSE);
			}
			else
			{
				fDrawn = pThumbNail->Draw(pDC, pContext, crContent);
//				ASSERT(fDrawn);
				pThumbNail->Release();
			}
		}

	/*
	// If we haven't drawn yet, draw the image from scratch.
	// This will either read from embedded file data (if available) or the
	// original file.
	*/

		if (!fDrawn)
		{
			if (nExists == -1)
			{
				IPathManager* pPathManager = GetGlobalPathManager();
				BOOL fOldPrompt = pPathManager->SetPromptForContent(FALSE);
				nExists = pPathManager->FileExists(FileName(), TRUE);
				pPathManager->SetPromptForContent(fOldPrompt);
			}

			if (!nExists)
			{
				// File does not exist. Nothing we can do.
				fDrawn = TRUE;
			}
			else
			{
				CRect crExtent;
				crExtent.left = crContent.left+2;
				crExtent.top = crContent.top+2;
				crExtent.right = crContent.right-3;
				crExtent.bottom = crContent.bottom-3;

				CRect crClip;
				pDC->GetClipBox(crClip);
				crClip &= crExtent;

				CRgn rgnClip;
				if (rgnClip.CreateRectRgn(crClip.left,
												  crClip.top,
												  crClip.right,
												  crClip.bottom))
				{
					pDC->SelectClipRgn(&rgnClip);
					fDrawn = DoDraw(pDC,
										 crExtent,
										 FALSE,
										 DrawBreatheHook, this);
				}
			}
		}

		pDC->RestoreDC(nContext);

		if (!fDrawn)
		{
		/* We want to try again later? */

			pContext->ReleaseDC();

			ThrowDrawException();
		}
	}
	pContext->ReleaseDC();
}

BOOL CFileItemData::DrawBreatheHook(LPVOID pData)
{
	return ((CFileItemData*)pData)->Breathe();
}

BOOL CFileItemData::Breathe(void)
{
	ASSERT(m_pDrawingContext != NULL);

	return m_pDrawingContext->Breathe();
}

BOOL CFileItemData::InitFile(void)
{
	return FALSE;
}

BOOL CFileItemData::Init(DATAID id, DATATYPE type, CStandardDataManager* pManager, LPVOID pData, ST_DEV_POSITION* in_where)
{
	BOOL fSuccess = INHERITED::Init(id, type, pManager, NULL, in_where);

	if (fSuccess)
	{
		if (pData != NULL)
		{
			CFileItemCreationData* pCData = (CFileItemCreationData*)pData;

			TRY
			{
			// Save the full name.
				ASSERT(pCData->m_pFileName != NULL);
				if (pCData->m_pFileName != NULL)
				{
					FileName(pCData->m_pFileName);
				}
				if (pCData->m_pFriendlyName != NULL)
				{
					FriendlyName(pCData->m_pFriendlyName);
				}
				Flags(pCData->m_dwFlags);
				m_Record.m_idParentCategory = pCData->m_idParentCategory;
			}
			CATCH_ALL(e)
			{
				return FALSE;
			}
			END_CATCH_ALL

			if (pCData->m_fEmbed)
			{
			/* Try to embed the file. Pass the full path. */
				EmbedSource(pCData->m_pFileName);
			}

			if (!InitFile())
			{
				fSuccess = FALSE;
			}
			else
			{
				if (pCData->m_fCreateThumbNail)
				{
					CreateThumbNail();
				}
			}
		}
	}
	return fSuccess;
}

void CFileItemData::EmbedSource(LPCSTR pFileName /*=NULL*/)
{
/*
// Handle any old source.
*/

	if (m_Record.m_idSource != 0)
	{
		m_pManager->DeleteRecord(m_Record.m_idSource, DATATYPE_EmbeddedFile);
		m_Record.m_idSource = 0;
	}

	if (pFileName == NULL)
	{
		pFileName = m_csFileName;
	}

/*
// Create the embedded file.
*/

	CEmbeddedFileData* pFileData;

	if ((pFileData = (CEmbeddedFileData*)m_pManager->NewRecord(DATATYPE_EmbeddedFile, (LPVOID)pFileName)) == NULL)
	{
	/* Oh, well. No can do. */
		return;
	}

/* Remember the ID. */

	m_Record.m_idSource = pFileData->Id();

/* Release the embedded file. */

	pFileData->Release();

/* We have been modified. */

	Modified();
}

void CFileItemData::ThumbNailDims(int& nWidth, int& nHeight, BOOL fMonochrome /*=FALSE*/)
{
/* Fit the image into a 48x48 box. */

	int nFitWidth = 48;
	int nFitHeight = 48;

	if (fMonochrome)
	{
		nFitWidth = 96;
		nFitHeight = 96;
	}

	if (nWidth > nHeight)
	{
		nHeight = MulDiv(nFitWidth, nHeight, nWidth);
		nWidth = nFitWidth;
	}
	else
	{
		nWidth = MulDiv(nFitHeight, nWidth, nHeight);
		nHeight = nFitHeight;
	}
}

void CFileItemData::ThumbNailPreCreate(CThumbNailCreationData* pData)
{
	pData->m_dwTryCompression = TRY_COMPRESSION_ALL;
	if (pData->m_nBPP != 24)
	{
		// Turn off JPEG.
		pData->m_dwTryCompression &= ~TRY_COMPRESSION_JPEG;
	}
}

BOOL CFileItemData::MonochromeThumbNail(void)
{
	return FALSE;
}

/*
// Create a thumbnail for this graphic.
*/

void CFileItemData::CreateThumbNail(void)
{
#if 1
/*
// Handle any old source.
*/

	if (m_Record.m_idThumbNail != 0)
	{
		m_pManager->DeleteRecord(m_Record.m_idThumbNail, DATATYPE_ThumbNail);
		m_Record.m_idThumbNail = 0;
	}

/* Get the dimensions for this thumbnail. */

	int nRealWidth = m_Record.m_crBounds.right;
	int nRealHeight = m_Record.m_crBounds.bottom;

	int nWidth = nRealWidth;
	int nHeight = nRealHeight;
	int nBits;

	ThumbNailDims(nWidth, nHeight);

  	CPalette* pPalette = CDrawContext::Palette();

	CDibDC DibDC;
	CDC DrawDC;
	HBITMAP hBitmap = NULL;
	CBitmap Bitmap;
	CBitmap* pOldBitmap = NULL;
	CPalette* pOldPalette = NULL;
	LPBYTE lpBits = NULL;
	LPBYTE lpDib = NULL;
	BOOL fDrawn = FALSE;
#ifdef WIN32
	DWORD dwHeaderSize = sizeof(BITMAPINFO);		// 24-bit
#else
	// 8-bit with palette.
	DWORD dwHeaderSize = sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD);
#endif

	CRect crBounds = CRect(0, 0, nWidth, nHeight);

	BOOL fMonochrome = MonochromeThumbNail();

	TRY
	{
		if (fMonochrome)
		{
			nBits = 1;
		/* Do a monochrome thumbnail. */

			if (DrawDC.CreateCompatibleDC(NULL))
			{
				CBitmap Bitmap;

				if (Bitmap.CreateBitmap((nWidth + 31) & ~31, nHeight, 1, 1, NULL))
				{
					int nByteWidth = (((nWidth+7)/8)+3)&~3;
					DWORD dwSize = (DWORD)nByteWidth*nHeight;

					if ((lpBits = new BYTE[(size_t)dwSize]) != NULL)
					{
						if ((pOldBitmap = DrawDC.SelectObject(&Bitmap)) != NULL)
						{
							fDrawn = ThumbNailDraw(&DrawDC, crBounds);

							DrawDC.SelectObject(pOldBitmap);
							pOldBitmap = NULL;

							if (fDrawn)
							{
							/* We need to extract the bits. */
								Bitmap.GetBitmapBits(dwSize, lpBits);
							}
						}
					}
				}
			}
		}
		else
		{
#ifdef WIN32
			DWORD dwBytes = (DWORD)((nWidth*3 + 3) & ~3) * (DWORD)nHeight;		// 24-bit
			lpDib = new BYTE[dwHeaderSize];
#else
			DWORD dwBytes = (DWORD)((nWidth + 3) & ~3) * (DWORD)nHeight;
			lpDib = (LPBYTE)_halloc(dwHeaderSize + dwBytes, 1);
#endif
			LPBITMAPINFO pbi = (LPBITMAPINFO)lpDib;

			memset(&pbi->bmiHeader, 0, sizeof(BITMAPINFOHEADER));

			pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			pbi->bmiHeader.biWidth = nWidth;
			pbi->bmiHeader.biHeight = nHeight;
			pbi->bmiHeader.biPlanes = 1;
#ifdef WIN32
			pbi->bmiHeader.biBitCount = 24;		// 24-bit
#else
			pbi->bmiHeader.biBitCount = 8;
#endif
			pbi->bmiHeader.biCompression = BI_RGB;
			pbi->bmiHeader.biSizeImage = dwBytes;

			nBits = pbi->bmiHeader.biBitCount;

#ifndef WIN32
			memcpy(pbi->bmiColors, GetIdentityPalette(), 256*sizeof(RGBQUAD));
#endif

			if (DibDC.Create(pbi))
			{
			/*
			// Plug in our palette.
			// We allow this to fail (I don't know why it would). If it does
			// fail, we proceed as if we don't have a palette selected (which
			// we don't!).
			*/

				if ((pOldPalette = DibDC.SelectPalette(pPalette, FALSE)) != NULL)
				{
					DibDC.RealizePalette();
				}

			/* Clear the bitmap to white. */

				CBrush brFoo;
				brFoo.CreateSolidBrush(RGB(255, 255, 255));
				DibDC.FillRect(&crBounds, &brFoo);

//#ifdef JNWUZHERE
				m_pThumbNailDIB = pbi;
				fDrawn = ThumbNailDraw(&DibDC, crBounds);
				m_pThumbNailDIB = NULL;

//#else
//				fDrawn = TRUE;		// JN!
//#endif
				if (pOldPalette != NULL)
				{
					DibDC.SelectPalette(pOldPalette, FALSE);
					pOldPalette = NULL;
				}
			}
			else
			{
				ASSERT(FALSE);
//				AfxMessageBox("Couldn't create DC!");
			}
		}

		if (fDrawn)
		{
		/* We have the bits! */
			TRACE("Got the bits!");

			CThumbNailCreationData cdata;

			cdata.m_nWidth = nWidth;
			cdata.m_nHeight = nHeight;
			cdata.m_nBPP = nBits;

			if (fMonochrome)
			{
				cdata.m_pBitmap = lpBits;
			}
			else
			{
#if defined(WIN32) && !defined(_MAC)
				cdata.m_pBitmap = (LPBYTE)DibDC.GetData();
#else
				cdata.m_pBitmap = (LPBYTE)(((BYTE __huge*)lpDib) + dwHeaderSize);
#endif
			}

			// Plug in the compressions to try.
			ThumbNailPreCreate(&cdata);

			CThumbNailData* pThumbNail;

			if ((pThumbNail = (CThumbNailData*)m_pManager->NewRecord(DATATYPE_ThumbNail, &cdata)) != NULL)
			{
			/* We have the thumbnail. */
				m_Record.m_idThumbNail = pThumbNail->Id();
				pThumbNail->Release();
			}
		}
	}
	END_TRY

	if (fMonochrome)
	{
		delete [] lpBits;
	}
	else
	{
		DibDC.DeleteDC();
#ifdef WIN32
		delete [] lpDib;
#else
		_hfree(lpDib);
#endif
	}

	if (hBitmap != NULL)
	{
		::DeleteObject(hBitmap);
		hBitmap = NULL;
	}

	if (pOldBitmap != NULL)
	{
		DrawDC.SelectObject(pOldBitmap);
		pOldBitmap = NULL;
	}

	if (pOldPalette != NULL)
	{
		DrawDC.SelectPalette(pOldPalette, FALSE);
		pOldPalette = NULL;
	}
	Modified();
#endif
}

BOOL CFileItemData::ThumbNailDraw(CDC* pDC, CRect& crBounds)
{
	CRect crExtent;
	crExtent.left = crBounds.left+1;
	crExtent.top = crBounds.top+1;
	crExtent.right = crBounds.right-1;
	crExtent.bottom = crBounds.bottom-1;

	return DoDraw(pDC, crExtent, TRUE);
}

/*
// Low-level draw.
*/

BOOL CFileItemData::DoDraw(CDC* pDC, CRect& crBounds, BOOL fThumbNail, BREATHE_FUNCTION pBreathe /*=NULL*/, LPVOID pBreatheData /*=NULL*/)
{
// Nothing to do. So we must have done it.
	return FALSE;
}

/*
// Set the filename for this item.
*/

void CFileItemData::FileName(const char* pName)
{
	m_csFileName = pName;
}

/*
// Get the filename for this item.
*/

const CString& CFileItemData::FileName(void) const
{
	return m_csFileName;
}

// Set the friendly name for this item.
void CFileItemData::FriendlyName(const char* pName)
{
	m_csFriendlyName = pName;
}

// Get the friendly name for this item. May be empty!
const CString& CFileItemData::FriendlyName(void) const
{
	return m_csFriendlyName;
}

void CFileItemData::Name(CString& csName)
{
	if (m_csFriendlyName.IsEmpty())
	{
		// Use the file name.
		Util::SplitPath(FileName(), NULL, &csName);
	}
	else
	{
		// Use the friendly name.
		csName = m_csFriendlyName;
	}
}

/*
// Return the number of subitems (generated items) in this item.
*/

int CFileItemData::SubItems(void) const
{
	return 1;
}

/*
// Return the bounds for an item.
*/

CRect CFileItemData::Bounds(int nSubItem) const
{
	ASSERT(nSubItem == 0);
	CRect crBounds(m_Record.m_crBounds.left,
						m_Record.m_crBounds.top,
						m_Record.m_crBounds.right,
						m_Record.m_crBounds.bottom);
	return crBounds;
}

/*
// Return the resolution for an item.
*/

CSize CFileItemData::Resolution(int nSubItem) const
{
	ASSERT(nSubItem == 0);
	CSize crResolution(m_Record.m_czResolution.cx, m_Record.m_czResolution.cy);
	return crResolution;
}

ERRORCODE CFileItemData::ExtractFile(LPCSTR pDestName)
{
	ERRORCODE error = ERRORCODE_DoesNotExist;

	ASSERT(m_Record.m_idSource != 0);

	if (m_Record.m_idSource != 0)
	{
		CEmbeddedFileData* pFileData;
		if ((pFileData = (CEmbeddedFileData*)Manager()->GetRecord(m_Record.m_idSource)) != NULL)
		{
		// Extract the file.
			error = pFileData->ExtractFile(pDestName);
			pFileData->Release();
		}
	}
	return error;
}

/*
// Assignment for a CFileItemData object.
*/

void CFileItemData::Assign(const CDataRecord& Record)
{
/* Base level assignment. */

	INHERITED::Assign(Record);

/* Assign our specific stuff. */

	const CFileItemData& s = (const CFileItemData&)Record;

	m_csFileName = s.m_csFileName;
	m_csFriendlyName = s.m_csFriendlyName;
	m_Record.m_crBounds = s.m_Record.m_crBounds;
	m_Record.m_czResolution = s.m_Record.m_czResolution;
	m_Record.m_wFlags = s.m_Record.m_wFlags;
	m_Record.m_wFileType = s.m_Record.m_wFileType;
	m_Record.m_idThumbNail = s.Manager()->DuplicateRecord(s.m_Record.m_idThumbNail, Manager());
#ifdef _DEBUG
	if (m_Record.m_idSource != 0)
	{
		TRACE("FileItemData::Assign : Embedded source not assigned.\n");
	}
#endif
}

void CFileItemData::OnDelete(void)
{
	if (m_Record.m_idSource != 0)
	{
		m_pManager->DeleteRecord(m_Record.m_idSource, DATATYPE_EmbeddedFile);
		m_Record.m_idSource = 0;
	}

	if (m_Record.m_idThumbNail != 0)
	{
		m_pManager->DeleteRecord(m_Record.m_idThumbNail, DATATYPE_ThumbNail);
		m_Record.m_idThumbNail = 0;
	}

	RemoveAllKeywords();
}

void CFileItemData::OnFlush(void)
{
	if (m_Record.m_idSource != 0)
	{
		m_pManager->FlushRecord(m_Record.m_idSource);
	}

	if (m_Record.m_idThumbNail != 0)
	{
		m_pManager->FlushRecord(m_Record.m_idThumbNail);
	}
}

void CFileItemData::AddKeyword(DATAID idKeyword)
{
	if (m_Keywords.find(idKeyword) == -1)
	{
		m_Keywords.add(idKeyword);
	}
}

int CFileItemData::Keywords(void)
{
	return m_Keywords.count();
}

DATAID CFileItemData::GetKeyword(int nKeyword)
{
	return m_Keywords.get(nKeyword);
}

void CFileItemData::RemoveKeyword(DATAID idKeyword)
{
	CKeywordRecord* pKeyword = (CKeywordRecord*)m_pManager->GetRecord(idKeyword, DATATYPE_Keyword);
	if (pKeyword != NULL)
	{
		pKeyword->RemoveItem(Id());
		pKeyword->Modified();
		pKeyword->Release();
		// We only remove the keyword if we removed ourselves from it.
		m_Keywords.remove(idKeyword);
	}
}

void CFileItemData::RemoveAllKeywords()
{
	for (int nKeyword = Keywords(); nKeyword-- > 0; )
	{
		RemoveKeyword(GetKeyword(nKeyword));
	}
}

BOOL CFileItemData::SetupDevice(StorageFile* pFile)
{
	BOOL fSuccess = FALSE;

/*
// Optimal case:
// Try to read data from the embedded record.
*/

	if (m_Record.m_idSource != NULL_DATAID)
	{
		CEmbeddedFileData* pFileData;

		if ((pFileData = (CEmbeddedFileData*)m_pManager->GetRecord(m_Record.m_idSource)) != NULL)
		{
			fSuccess = (pFileData->PrepFile(pFile) == ERRORCODE_None);
		// We always Release() what we got.
		// There will be a reference count added for the device if successful.
			pFileData->Release();
		}
	}

#if 0
	if (!fSuccess)
	{
		pFileData->set_name(FileName());
		fSuccess = TRUE;
	}
#endif
	return fSuccess;
}

ERRORCODE CFileItemData::ReadData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	ReadOnlyFile File(pDevice);

	ST_DEV_POSITION pos, end;
	pDevice->tell(&pos);						// 0 is here
	pDevice->length(&end);					// allow reads to end of file
	File.set_subfile(pos, end);

	// Make a guess for best case.
	File.set_read_buffer(128);

	if ((error = INHERITED::ReadData(&File)) == ERRORCODE_None
		&& (error = File.read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None
		&& (error = File.read_cstring(m_csFileName)) == ERRORCODE_None
		&& (error = File.read_cstring(m_csFriendlyName)) == ERRORCODE_None)
	{
		if (Flags() & FILEITEM_FLAG_HasKeywords)
		{
			error = m_Keywords.read(&File);
		}
	}
	// Seek to the end as if we read normally.
	File.tell(&end);
	pDevice->seek(pos + end, ST_DEV_SEEK_SET);

	return error;
}

ERRORCODE CFileItemData::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;

	if (Keywords() == 0)
	{
		m_Record.m_dwFlags &= ~FILEITEM_FLAG_HasKeywords;
	}
	else
	{
		m_Record.m_dwFlags |= FILEITEM_FLAG_HasKeywords;
	}

	if ((error = INHERITED::WriteData(device)) == ERRORCODE_None
		&& (error = device->write_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None
		&& (error = device->write_cstring(m_csFileName)) == ERRORCODE_None
		&& (error = device->write_cstring(m_csFriendlyName)) == ERRORCODE_None)
	{
		if (Flags() & FILEITEM_FLAG_HasKeywords)
		{
			error = m_Keywords.write(device);
		}
	}

	return error;
}

ST_MAN_SIZE CFileItemData::SizeofData(StorageDevicePtr device)
{
	ST_MAN_SIZE nSize = INHERITED::SizeofData(device)
			+ device->size_record(sizeof(m_Record))
			+ device->size_cstring(m_csFileName)
			+ device->size_cstring(m_csFriendlyName);
	if (Keywords() != 0)
	{
		nSize += m_Keywords.size(device);
	}
	return nSize;
}

/*
// Sort key generator for a file item.
*/

BOOL CFileItemData::SortKey(int nKey, CString& csKey)
{
	switch (nKey)
	{
		case ITEM_SORT_Name:
		{
			Name(csKey);
			return TRUE;
		}
		default:
		{
			return FALSE;
		}
	}
}

void CFileItemData::Update(const CItemDataDescriptor& Descriptor)
{
	// Reinit the file.
	if (InitFile())
	{
		// All we really do here is update the thumbnail...
		m_fUpdating = TRUE;
		CFileItemCreationData* pData = (CFileItemCreationData*)Descriptor.m_pInitData;
		if (pData->m_fCreateThumbNail)
		{
			CreateThumbNail();
		}
		m_fUpdating = FALSE;

		// ...and set the new friendly name.
		m_csFriendlyName.Empty();
		if (pData->m_pFriendlyName != NULL)
		{
			m_csFriendlyName = pData->m_pFriendlyName;
		}
		Modified();
	}
}

void CFileItemData::SetRCInfo(RedisplayContext* pRC, HDC hDC, BOOL fThumbNail)
{
	if (fThumbNail && m_pThumbNailDIB != NULL)
	{
		pRC->info.bpp = m_pThumbNailDIB->bmiHeader.biBitCount;
		pRC->info.planes = m_pThumbNailDIB->bmiHeader.biPlanes;
		ASSERT(pRC->info.planes == 1);
		switch (pRC->info.bpp)
		{
			case 1:
			{
				pRC->info.type = RASTER_TYPE_BILEVEL;
				break;
			}
			case 3:
			case 4:
			{
				pRC->info.type = RASTER_TYPE_8;
				break;
			}
			case 8:
			{
				if (GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE)
				{
					pRC->info.type = RASTER_TYPE_PALETTE;
				}
				else
				{
					pRC->info.type = RASTER_TYPE_256;
				}
				break;
			}
			default:
			case 24:
			{
				pRC->info.type = RASTER_TYPE_24;
				pRC->info.bpp = 24;
				pRC->info.planes = 1;
				break;
			}
		}
	}
	else
	{
		pRC->set_info(hDC);
	}
}

// Is this item licensed?
BOOL CFileItemData::IsLicensed(void)
{
	// If this is a free item and the user is not registered, return FALSE.
	if ((Flags() & FILEITEM_FLAG_Free)
			&& !GET_PMWAPP()->user_is_registered())
	{
		return FALSE;
	}

	if (m_fLicensed == -1)
	{
		m_fLicensed = TRUE;			// Assume so.
		DATAID idParentCategory = ParentCategory();
		if (idParentCategory != NULL_DATAID)
		{
			CSimpleCategory* pCategory = (CSimpleCategory*)Manager()->GetRecord(idParentCategory, DATATYPE_SimpleCategory);
			if (pCategory != NULL)
			{
				// Check whether the category as a whole is licensed.
				m_fLicensed = FALSE;
				if (pCategory->IsLicensed())
				{
					/*
					// Check whether we ourselves are licensed.
					// We need to get the category we are merged into and check if
					// we are in that category. If we are not joined (i.e. if
					// we're native), then we're licensed by default.
					*/

					int nOurManagerNumber = pCategory->GetOurJoinedManagerNumber();
					CSimpleCategory* pJoineeCategory = pCategory->GetJoineeCategory();
					if (pJoineeCategory != NULL)
					{
						// If the item inclusion array is NULL, then everybody is
						// included - including this item!
						if (pJoineeCategory->GetItemInclusionArray() == NULL)
						{
							m_fLicensed = TRUE;
						}
						else
						{
							// OK, so we need to check.
							// Update the items so we can look for ours.
							// During the update, items will get thrown out that do not
							// belong. If we're in the final list, then we're licensed.
							pJoineeCategory->UpdateItems();		// Crunch!

							// Build our item number.
							DATAID idThis = CMultiDataManager::BuildDataId(Id(), nOurManagerNumber);

							// See if our item is in the list.
							int nItems = pJoineeCategory->Items();
							for (int nItem = 0; nItem < nItems; nItem++)
							{
								CItemProxy* pProxy = pJoineeCategory->Item(nItem);
								if (pProxy != NULL && pProxy->DataID() == idThis)
								{
									m_fLicensed = TRUE;
									break;
								}
							}
						}

						pJoineeCategory->Release();
					}
				}
				pCategory->Release();
			}
		}
	}
	return m_fLicensed;
}
