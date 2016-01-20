/*
// $Workfile: ADDREDIT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:01p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/ADDREDIT.CPP $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 6     9/14/98 12:05p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 5     7/11/98 2:48p Jayn
// Got rid of some resource leaks.
// 
// 4     5/20/98 2:53p Cboggio
// Put country names in the resource file
// 
// 3     1/27/98 6:40p Hforman
// remove extern Clipboard near clipboard
// 
// 2     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
//    Rev 1.0   14 Aug 1997 15:18:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:02   Fred
// Initial revision.
// 
//    Rev 1.27   02 Jul 1997 09:03:22   Jay
// Modal picture selection
// 
//    Rev 1.26   06 May 1997 10:30:04   Jay
// New browser stuff.
// 
//    Rev 1.25   17 Apr 1997 13:06:06   Fred
// Added source rectangle to TransparentBltBitmap
// 
//    Rev 1.24   17 Apr 1997 12:24:30   Fred
//  
// 
//    Rev 1.23   03 Apr 1997 14:31:06   Jay
// Resource cleanup
// 
//    Rev 1.22   07 Nov 1996 17:06:50   johno
// Moved strings to resource DLL
// 
//    Rev 1.21   28 Oct 1996 09:36:44   johno
// Strings moved to resource file
// 
//    Rev 1.20   15 Oct 1996 09:02:00   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.19   12 Aug 1996 09:45:02   Fred
// Changes to 16-bit edit create to defeat CTL3D
// 
//    Rev 1.18   09 Aug 1996 16:50:12   Fred
// More 16-bit fixes
// 
//    Rev 1.17   08 Aug 1996 11:10:42   Fred
// 16-bit fixes
// 
//    Rev 1.16   02 Aug 1996 13:59:36   FRED
// Delete __huge requires NULL check
// 
//    Rev 1.15   01 Aug 1996 09:22:38   Fred
// More 16-bit changes
// 
//    Rev 1.14   31 Jul 1996 11:56:20   Fred
// 16-bit compatibility
// 
//    Rev 1.13   23 Jul 1996 08:00:26   Fred
// Refresh gets modified flags from database
// 
//    Rev 1.12   18 Jul 1996 18:43:58   Fred
// Palette fixes
// 
//    Rev 1.11   18 Jul 1996 07:28:40   Fred
// Fixed picture bug; Moved countries to ADDRDATA
// 
//    Rev 1.10   17 Jul 1996 19:08:32   Fred
// Picture blobs now have compression type
// 
//    Rev 1.9   17 Jul 1996 16:38:38   Fred
// Uses addrdata; compresses pictures
// 
//    Rev 1.8   17 Jul 1996 09:04:58   Fred
// Some address book improvements
// 
//    Rev 1.7   16 Jul 1996 08:50:48   Fred
// Hooked up address/sender book
// 
//    Rev 1.6   06 Jul 1996 12:17:42   Fred
// Address book improvements
// 
//    Rev 1.5   06 Jul 1996 09:25:20   Fred
// Picture editor and other improvements
// 
//    Rev 1.4   03 Jul 1996 18:52:24   Fred
// Picture editor
// 
//    Rev 1.3   01 Jul 1996 07:30:22   Fred
// Receiver list
// 
//    Rev 1.2   27 Jun 1996 10:42:50   Fred
// Receiver list. Before removing receiver from non-receiver views
// 
//    Rev 1.1   24 Jun 1996 19:16:22   Fred
// Some fixes and improvments
// 
//    Rev 1.0   24 Jun 1996 00:31:16   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "memdev.h"
#include "pmwdoc.h"
#include "cdibdc.h"
#include "grafrec.h"
#include "grafobj.h"
#include "clip.h"
#include "util.h"
#include "addrdata.h"
#include "addredit.h"
#include "bmp.h"
#include "cdeflate.h"

#define new DEBUG_NEW

extern BYTE far screen_gamma_curve[];
extern CPalette *pOurPal;		// THE PrintMaster Palette (from PMWVIEW.CPP)

extern void DrawCheckMark(CDC& dc, const CRect crRect, COLORREF clColor);

// Helper function to create a thumbnail bitmap from a picture file.
LPBITMAPINFO MakeThumbnail(LPCSTR pszFileName, int nMaxWidth, int nMaxHeight, CPalette* pPalette /*=NULL*/)
{
	LPBITMAPINFO pBitmap = NULL;

	// Set the palette to use when generating the bitmap .
	if (pPalette == NULL)
	{
		pPalette = pOurPal;
	}

	if (pPalette != NULL)
	{
		// Create a document which will hold the picture.

		// Use the first template.
		POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
		CDocTemplate* pTemplate = AfxGetApp()->GetNextDocTemplate(pos);

		// Create an "album" document.
		WORD fGDFSave = CPmwDoc::m_wGlobalDocumentFlags;
		CPmwDoc::m_wGlobalDocumentFlags = CPmwDoc::FLAG_AlbumDocument;
		CPmwDoc* pDocument = (CPmwDoc*)pTemplate->OpenDocumentFile(NULL, FALSE);
		CPmwDoc::m_wGlobalDocumentFlags = fGDFSave;

		if (pDocument != NULL)
		{
			// Create the graphic.
			GRAPHIC_CREATE_STRUCT gcs;

			gcs.proof = FALSE;

			// Now, we get the graphic record to use for the description.
			gcs.m_csFileName = pszFileName;

			DB_RECORD_NUMBER lGraphic;
			if ((lGraphic = pDocument->get_database()->new_graphic_record(&gcs)) != 0)
			{
				GraphicPtr graphic;

				if ((graphic = (GraphicPtr)(pDocument->get_database()->get_record(lGraphic, NULL, RECORD_TYPE_Graphic))) != NULL)
				{
					// Compute the size to fit within the original rect.
					int nWidth, nHeight;
					nHeight = nMaxHeight;
					if ((nWidth = MulDiv(nHeight, graphic->record.x_size, graphic->record.y_size)) > nMaxWidth)
					{
						nWidth = nMaxWidth;
						nHeight = MulDiv(nWidth, graphic->record.y_size, graphic->record.x_size);
					}

					// Release the graphic.
					graphic->release();

					// Create the object.
					GraphicObjectPtr object;

					if ((object = pDocument->get_database()->create_graphic_object()) != NULL)
					{
	 					// Attach the record to the object.
						// The record now belongs to the object.
						object->set_graphic_record(lGraphic);

						PBOX bound;
						bound.x0 = (PCOORD)0;
						bound.y0 = (PCOORD)0;
						bound.x1 = (PCOORD)nWidth;
						bound.y1 = (PCOORD)nHeight;
						object->set_bound(bound);

						// Allocate the packed DIB.
						int nByteWidth = (nWidth + 3) & ~3;
						WORD wDataBytes = (WORD)nByteWidth * (WORD)nHeight;
						int nHeaderBytes = sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD);
						LPBITMAPINFO pbi = NULL;
						TRY
						{
							pbi = (LPBITMAPINFO)(new BYTE[nHeaderBytes + wDataBytes]);
						}
						END_TRY;

						if (pbi != NULL)
						{
							// Initialize the DIB header.
							pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
							pbi->bmiHeader.biWidth = (LONG)nWidth;
							pbi->bmiHeader.biHeight = (LONG)nHeight;
							pbi->bmiHeader.biPlanes = 1;
							pbi->bmiHeader.biBitCount = 8;
							pbi->bmiHeader.biCompression = BI_RGB;
							pbi->bmiHeader.biSizeImage = (DWORD)wDataBytes;
							pbi->bmiHeader.biXPelsPerMeter =
								pbi->bmiHeader.biYPelsPerMeter = 0;
							pbi->bmiHeader.biClrUsed =
								pbi->bmiHeader.biClrImportant = 0;

							// GetPaletteEntries() returns PALETTEENTRY structures. We have to
							// change these to RGBQUAD structures.
							UINT nEntries = pPalette->GetPaletteEntries(0, 256, (LPPALETTEENTRY)(&(pbi->bmiColors[0])));
							RGBQUAD* pEntry = &(pbi->bmiColors[0]);
							for (UINT nEntry = 0; nEntry < nEntries; nEntry++)
							{
								RGBQUAD dwEntry = *pEntry;
								pEntry->rgbRed= dwEntry.rgbBlue;
								pEntry->rgbBlue = dwEntry.rgbRed;
								pEntry->rgbReserved = 0;
								pEntry++;
							}

							// Create a DIB DC. In WIN32, CreateDIBSection is used.
							CDibDC dc;

							if (dc.Create(pbi))
							{
								CPalette* pOldPal = dc.SelectPalette(pPalette, FALSE);
								dc.RealizePalette();

								// Clear the map. Redisplay code won't since it's "printing".
								dc.PatBlt(0, 0, nWidth, nHeight, WHITENESS);

	 							// Build the redisplay context.
								RedisplayContext rc;

								rc.destination_hdc = dc.GetSafeHdc();
								rc.hwnd = NULL;

								rc.scaled_source_x0 =
	 								rc.scaled_source_y0 = 0;

								rc.destination_rect.left = 0;
								rc.destination_rect.top = 0;
								rc.destination_rect.right = nWidth;
								rc.destination_rect.bottom = nHeight;

								rc.destination_x0 = 0;
								rc.destination_y0 = 0;

	 							// Set the resolutions to that of the page.
								rc.x_resolution = rc.y_resolution = PAGE_RESOLUTION;

								rc.bitmap_gamma_curve =
									rc.outline_gamma_curve = screen_gamma_curve;

								rc.info.bpp = 8;
								rc.info.planes = 1;
								rc.info.type = RASTER_TYPE_256;

//								rc.is_printing = TRUE;			/* Behave like printing. */
								rc.is_thumbnail = TRUE;

								// Draw the object!

								UpdateStatePtr ustate;

								if ((ustate = object->update(&rc, &bound, &rc.destination_rect, NULL, REFRESH_ALL)) != NULL)
								{
									// See what happened.
									switch (ustate->type)
									{
										case UPDATE_TYPE_Interrupted:
										{
											// This should not happen.
											delete ustate;
											// FALL THROUGH TO...
										}
										case UPDATE_TYPE_Terminated:
										{
											ASSERT(FALSE);
											break;
										}
										default:
										{
											break;
										}
									}
								}

								// Select the old palette back in.

								dc.SelectPalette(pOldPal, FALSE);

								//* Destroy the object... it takes the record with it.
								object->destroy();
								delete object;

								// WIN32 uses CreateDIBSection, so we must copy the DIB section
								// data to our bitmap. (WIN16 writes directly to the bitmap.)
								memcpy(((LPBYTE)pbi) + nHeaderBytes, dc.GetData(), wDataBytes);

								pBitmap = pbi;
							}

							if (pBitmap == NULL)
							{
								delete pbi;
							}
						}
					}
				}
			}

			// Free the document.
			pDocument->OnCloseDocument();
			pDocument = NULL;
		}
	}

	return pBitmap;
}

// Helper function to copy a graphic to the clipboard.
BOOL CopyPictureToClipboard(LPCSTR pszFileName, LPBITMAPINFO pBitmap)
{
	BOOL fResult = FALSE;

	// Create a document which will hold the picture.

	// Use the first template.
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = AfxGetApp()->GetNextDocTemplate(pos);

	// Create an "album" document.
	WORD fGDFSave = CPmwDoc::m_wGlobalDocumentFlags;
	CPmwDoc::m_wGlobalDocumentFlags = CPmwDoc::FLAG_AddressBookDocument;
	CPmwDoc* pDocument = (CPmwDoc*)pTemplate->OpenDocumentFile(NULL, FALSE);
	CPmwDoc::m_wGlobalDocumentFlags = fGDFSave;

	if (pDocument != NULL)
	{
		// Create the graphic.
		GRAPHIC_CREATE_STRUCT* pgcs = pDocument->get_gcs();

		pgcs->proof = FALSE;

		// Now, we get the graphic record to use for the description.
		pgcs->m_csFileName = pszFileName;

		if (pDocument->create_chosen_picture() != ERRORCODE_None)
		{
			// Creation from file failed. Try to create from the thumbnail.
			char cbName[30];
			clipboard.BuildPasteName(".BMP", cbName);

			GraphicObject* pObject = NULL;

			if (CreateBMPFromDIB(cbName,
										 pDocument->get_database(),
										 (BITMAPINFO*)pBitmap,
										 pObject) == ERRORCODE_None)
			{
				ASSERT(pObject != NULL);
				pDocument->handle_new_object(pObject);
			}
		}
		if (pDocument->selected_objects() != NULL)
		{
			if (clipboard.CopyObjects(pDocument) == ERRORCODE_None)
			{
				fResult = TRUE;
			}
		}

		// Free the document.
		pDocument->OnCloseDocument();
		pDocument = NULL;
	}

	if (!fResult)
	{
		ASSERT(FALSE);
	}

	return fResult;
}

// Helper function to let the user choose a picture file.
BOOL ChoosePicture(CString& csName, LPCSTR pszLastName = NULL)
{
	BOOL fResult = FALSE;

	// Create a document which will hold the picture.

	// Use the first template.
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = AfxGetApp()->GetNextDocTemplate(pos);

	// Create an "album" document.
	WORD fGDFSave = CPmwDoc::m_wGlobalDocumentFlags;
	CPmwDoc::m_wGlobalDocumentFlags = CPmwDoc::FLAG_AddressBookDocument;
	CPmwDoc* pDocument = (CPmwDoc*)pTemplate->OpenDocumentFile(NULL, FALSE);
	CPmwDoc::m_wGlobalDocumentFlags = fGDFSave;

	if (pDocument != NULL)
	{
		// Choose a picture without using the album.
		if (pDocument->ChoosePicture(pszLastName) == IDOK)
		{
			TRY
			{
				csName = pDocument->get_gcs()->m_csFileName;
				fResult = TRUE;
			}
			END_TRY
		}

		// Free the document.
		pDocument->OnCloseDocument();
		pDocument = NULL;
	}

	return fResult;
}

// Helper function to move a window only if it's really going to move.
_inline void MoveChildWindow(CWnd* pWnd, LPCRECT pRect, BOOL fRepaint = TRUE)
{
	pWnd->MoveWindow(pRect, fRepaint);
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditor
//
// The base wrapper class for a field editor.

CAddressBookFieldEditor::CAddressBookFieldEditor(CFlatFileDatabaseField* pField)
{
	m_pField = pField;
	m_pEditor = NULL;
}

CAddressBookFieldEditor::~CAddressBookFieldEditor()
{
	DestroyEditor();
}

void CAddressBookFieldEditor::DestroyEditor(void)
{
	if (m_pEditor != NULL)
	{
		m_pEditor->DestroyWindow();
		delete m_pEditor;
		m_pEditor = NULL;
	}
}

// Create a proper wrapper class based on the field type.
CAddressBookFieldEditor* CAddressBookFieldEditor::Create(CFlatFileDatabaseField* pField, CWnd* pParent)
{
	CAddressBookFieldEditor* pWrapper = NULL;
	
	TRY
	{
		ASSERT(pField != NULL);
		if (pField != NULL)
		{
			switch (FIELD_USER_DATA_EDITOR(pField->GetDescription()->GetUserData()))
			{
				case FIELD_EDIT_NORMAL:		pWrapper = new CAddressBookFieldEditorNormalWrapper(pField);	break;
				case FIELD_EDIT_LIST:		pWrapper = new CAddressBookFieldEditorListWrapper(pField);		break;
				case FIELD_EDIT_MEMO:		pWrapper = new CAddressBookFieldEditorMemoWrapper(pField);		break;
				case FIELD_EDIT_PICTURE:	pWrapper = new CAddressBookFieldEditorPictureWrapper(pField);	break;
			}

			if (pWrapper != NULL)
			{
				pWrapper->CreateEditor(pParent);
			}
		}
	}
	END_TRY

	return pWrapper;
}

void CAddressBookFieldEditor::Move(const CRect& crPosition)
{
	MoveChildWindow(GetEditor(), crPosition);
}

BOOL CAddressBookFieldEditor::IsEditing(void)
{
	// Return TRUE if the focused window is the editor or has
	// the editor as an ancestor.
	CWnd* pFocus = CWnd::GetFocus();
	while (pFocus != NULL)
	{
		if (pFocus == GetEditor())
		{
			return TRUE;
		}
		pFocus = pFocus->GetParent();
	}

	return FALSE;
}

void CAddressBookFieldEditor::Save(void)
{
	CWnd* pWnd = GetEditor();
	if (pWnd != NULL)
	{
		CFlatFileDatabaseField* pField = GetField();
		if (pField != NULL)
		{
			if (IsModified())
			{
				CString csText;
				TRY
				{
					pWnd->GetWindowText(csText);

					TRY
					{
						Util::Trim(csText);
					}
					END_TRY

					pField->SetAsString(csText);
				}
				END_TRY

				SetModified(FALSE);
			}
		}
	}
}

void CAddressBookFieldEditor::Refresh(void)
{
	CWnd* pWnd = GetEditor();
	if (pWnd != NULL)
	{
		CFlatFileDatabaseField* pField = GetField();
		if (pField != NULL)
		{
			pWnd->SetWindowText(m_pField->GetAsString());
			SetModified(pField->IsModified());
		}
	}
}

void CAddressBookFieldEditor::Redraw(void)
{
	CWnd* pWnd = GetEditor();
	if (pWnd != NULL)
	{
		pWnd->UpdateWindow();
	}
}

void CAddressBookFieldEditor::ExcludeClipRect(CDC* pParentDC)
{
	CWnd* pWnd = GetEditor();
	if (pWnd != NULL)
	{
		CRect crEditor;
		pWnd->GetWindowRect(crEditor);
		pWnd->GetParent()->ScreenToClient(crEditor);
		pParentDC->ExcludeClipRect(crEditor);
	}
}

void CAddressBookFieldEditor::Show(BOOL fShow)
{
	CWnd* pWnd = GetEditor();
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(fShow ? SW_SHOW : SW_HIDE);
	}
}

void CAddressBookFieldEditor::SetFocus(void)
{
	CWnd* pWnd = GetEditor();
	if (pWnd != NULL)
	{
		pWnd->SetFocus();
	}
}

void CAddressBookFieldEditor::BringToTop(CWnd* pInsertAfter /*=NULL*/)
{
	CWnd* pWnd = GetEditor();
	if (pWnd != NULL)
	{
		if (pInsertAfter == NULL)
		{
			pWnd->SetWindowPos(&(pWnd->wndTop), 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		}
		else
		{
			pWnd->SetWindowPos(pInsertAfter, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorNormalWrapper
//
// The wrapper class for the normal field editor.

CAddressBookFieldEditorNormalWrapper::CAddressBookFieldEditorNormalWrapper(CFlatFileDatabaseField* pField) :
	CAddressBookFieldEditor(pField)
{
}

CAddressBookFieldEditorNormalWrapper::~CAddressBookFieldEditorNormalWrapper()
{
}

void CAddressBookFieldEditorNormalWrapper::Reset(void)
{
	CAddressBookFieldEditorNormal* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		pEditor->SetSel(0, 0);
	}
}

BOOL CAddressBookFieldEditorNormalWrapper::IsModified(void)
{
	CAddressBookFieldEditorNormal* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		return pEditor->GetModify();
	}

	return FALSE;
}

void CAddressBookFieldEditorNormalWrapper::SetModified(BOOL fModified /*=TRUE*/)
{
	CAddressBookFieldEditorNormal* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		pEditor->SetModify(fModified);
	}
}

void CAddressBookFieldEditorNormalWrapper::CreateEditor(CWnd* pParent)
{
	TRY
	{
		DestroyEditor();
		m_pEditor = new CAddressBookFieldEditorNormal(this, pParent);
	}
	END_TRY
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorListWrapper
//
// The wrapper class for the list field editor.

CAddressBookFieldEditorListWrapper::CAddressBookFieldEditorListWrapper(CFlatFileDatabaseField* pField) :
	CAddressBookFieldEditorNormalWrapper(pField)
{
}

CAddressBookFieldEditorListWrapper::~CAddressBookFieldEditorListWrapper()
{
}

void CAddressBookFieldEditorListWrapper::Move(const CRect& crPosition)
{
	CAddressBookFieldEditorList* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		pEditor->Move(crPosition);
	}
}

void CAddressBookFieldEditorListWrapper::Redraw(void)
{
	INHERITED::Redraw();

	CAddressBookFieldEditorList* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		CAddressBookFieldEditorListButton* pButton = pEditor->GetButton();
		ASSERT(pButton != NULL);
		if (pButton != NULL)
		{
			pButton->UpdateWindow();
		}
	}
}

void CAddressBookFieldEditorListWrapper::ExcludeClipRect(CDC* pParentDC)
{
	INHERITED::ExcludeClipRect(pParentDC);

	CAddressBookFieldEditorList* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		CAddressBookFieldEditorListButton* pButton = pEditor->GetButton();
		ASSERT(pButton != NULL);
		if (pButton != NULL)
		{
			if (pButton->IsWindowVisible())
			{
				CRect crButton;
				pButton->GetWindowRect(crButton);
				pButton->GetParent()->ScreenToClient(crButton);
				pParentDC->ExcludeClipRect(crButton);
			}
		}
	}
}

void CAddressBookFieldEditorListWrapper::Show(BOOL fShow /*=TRUE*/)
{
	CAddressBookFieldEditorList* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		pEditor->Show(fShow);
	}
}

void CAddressBookFieldEditorListWrapper::BringToTop(CWnd* pInsertAfter /*=NULL*/)
{
	INHERITED::BringToTop();

	CAddressBookFieldEditorList* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		CAddressBookFieldEditorListButton* pButton = pEditor->GetButton();
		ASSERT(pButton != NULL);
		if (pButton != NULL)
		{
			pButton->SetWindowPos(pEditor, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		}
	}
}

void CAddressBookFieldEditorListWrapper::CreateEditor(CWnd* pParent)
{
	TRY
	{
		DestroyEditor();
		m_pEditor = new CAddressBookFieldEditorList(this, pParent);
	}
	END_TRY
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorMemoWrapper
//
// The wrapper class for the memo field editor.

CAddressBookFieldEditorMemoWrapper::CAddressBookFieldEditorMemoWrapper(CFlatFileDatabaseField* pField) :
	CAddressBookFieldEditor(pField)
{
}

CAddressBookFieldEditorMemoWrapper::~CAddressBookFieldEditorMemoWrapper()
{
}

void CAddressBookFieldEditorMemoWrapper::Reset(void)
{
	CAddressBookFieldEditorMemo* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		pEditor->SetSel(0, 0);
	}
}

BOOL CAddressBookFieldEditorMemoWrapper::IsModified(void)
{
	CAddressBookFieldEditorMemo* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		return pEditor->GetModify();
	}
	
	return FALSE;
}

void CAddressBookFieldEditorMemoWrapper::SetModified(BOOL fModified /*=TRUE*/)
{
	CAddressBookFieldEditorMemo* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		pEditor->SetModify(fModified);
	}
}

void CAddressBookFieldEditorMemoWrapper::Save(void)
{
	if (IsModified())
	{
		CFlatFileDatabaseField* pField = GetField();
		ASSERT(pField != NULL);
		if (pField != NULL)
		{
			CAddressBookFieldEditorMemo* pEditor = GetEditor();
			ASSERT(pEditor != NULL);
			if (pEditor != NULL)
			{
				// Save the editor text as the field's blob data.
				CString csText;

				TRY
				{
					// Get the new text.
					pEditor->GetWindowText(csText);

					// Trim off any leading or trailing whitespace.
					TRY
					{
						Util::Trim(csText);
					}
					END_TRY

					// If the text is empty, simply set the blob number to 0 (no data.)
					DB_RECORD_NUMBER dwNumber = 0;
					int nLength = csText.GetLength();
					if (nLength > 0)
					{
						// We have the new text. Create a blob for it.
						MemoryDevice Device;
						DWORD dwType = BLOB_TYPE_MEMO;
						Device.AddPiece(&dwType, sizeof(dwType));
						Device.AddPiece((LPVOID)(LPCSTR)csText, nLength);

						// Write out the new blob.
						pField->GetDatabase()->SetBlobData(&Device, &dwNumber);
					}

					// Delete the old blob.
					if (pField->GetAsUnsigned32() != 0)
					{
						pField->GetDatabase()->DeleteRecord(pField->GetAsUnsigned32());
					}

					// Save the number of the new blob.
					pField->SetAsUnsigned32(dwNumber);
				}
				END_TRY
			}
		}

		SetModified(FALSE);
	}
}

void CAddressBookFieldEditorMemoWrapper::Refresh(void)
{
	CFlatFileDatabaseField* pField = GetField();
	ASSERT(pField != NULL);
	if (pField != NULL)
	{
		CAddressBookFieldEditorMemo* pEditor = GetEditor();
		ASSERT(pEditor != NULL);
		if (pEditor != NULL)
		{
			// Refresh modified flag.
			SetModified(pField->IsModified());

			// Set the editor text to the field's blob data.
			DWORD dwNumber = pField->GetAsUnsigned32();
			if (dwNumber == 0)
			{
				pEditor->SetWindowText("");
			}
			else
			{
				// Set up a storage file to access the blob data.
				StorageFile File;
				if (pField->GetDatabase()->GetBlobDataAsFile(&File, dwNumber) == ERRORCODE_None)
				{
					// Get the blob size.
					DWORD dwSize;
					if (File.length((ST_DEV_POSITION*)&dwSize) == ERRORCODE_None)
					{
						// Read the type of blob.
						DWORD dwType;
						if (File.huge_read(&dwType, sizeof(dwType), NULL) == ERRORCODE_None)
						{
							ASSERT(dwType == BLOB_TYPE_MEMO);
							if (dwType == BLOB_TYPE_MEMO)
							{
								dwSize -= sizeof(dwType);

								// Read the blob data.
								CString csText;
								TRY
								{
									LPSTR pBuffer = csText.GetBuffer((int)dwSize);
									if (pBuffer != NULL)
									{
										if (File.huge_read(pBuffer, dwSize, NULL) == ERRORCODE_None)
										{
											// Set the window text.
											csText.ReleaseBuffer();
											pEditor->SetWindowText(csText);
										}
									}
								}
								END_TRY
							}
						}
					}
				}
			}
		}
	}
}

void CAddressBookFieldEditorMemoWrapper::CreateEditor(CWnd* pParent)
{
	TRY
	{
		DestroyEditor();
		m_pEditor = new CAddressBookFieldEditorMemo(this, pParent);
	}
	END_TRY
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorPictureWrapper
//
// The wrapper class for the picture field editor.

CAddressBookFieldEditorPictureWrapper::CAddressBookFieldEditorPictureWrapper(CFlatFileDatabaseField* pField) :
	CAddressBookFieldEditor(pField)
{
}

CAddressBookFieldEditorPictureWrapper::~CAddressBookFieldEditorPictureWrapper()
{
}

void CAddressBookFieldEditorPictureWrapper::Reset(void)
{
}

void CAddressBookFieldEditorPictureWrapper::Move(const CRect& crPosition)
{
	CAddressBookFieldEditorPicture* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		pEditor->Move(crPosition);
	}
}

BOOL CAddressBookFieldEditorPictureWrapper::IsModified(void)
{
	CAddressBookFieldEditorPicture* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		return pEditor->IsModified();
	}
	
	return FALSE;
}

void CAddressBookFieldEditorPictureWrapper::SetModified(BOOL fModified /*=TRUE*/)
{
	CAddressBookFieldEditorPicture* pEditor = GetEditor();
	ASSERT(pEditor != NULL);
	if (pEditor != NULL)
	{
		pEditor->SetModified(fModified);
	}
}

void CAddressBookFieldEditorPictureWrapper::Save(void)
{
	if (IsModified())
	{
		CFlatFileDatabaseField* pField = GetField();
		ASSERT(pField != NULL);
		if (pField != NULL)
		{
			CAddressBookFieldEditorPicture* pEditor = GetEditor();
			ASSERT(pEditor != NULL);
			if (pEditor != NULL)
			{
				DB_RECORD_NUMBER dwNumber = 0;

				// Check if there is anything to save. If not, set the record number to zero.
				if (pEditor->HasBitmap())
				{
					// We have a bitmap, attempt to create a blob for it (dwNumber is set to record number.)
					SavePictureBlob(pField->GetDatabase(), pEditor->GetBitmapName(), pEditor->GetBitmapData(), dwNumber);
				}

				TRY
				{
					// Delete the old blob.
					if (pField->GetAsUnsigned32() != 0)
					{
						pField->GetDatabase()->DeleteRecord(pField->GetAsUnsigned32());
					}
				}
				END_TRY

				TRY
				{
					// Save the number of the new blob.
					pField->SetAsUnsigned32(dwNumber);
				}
				END_TRY
			}
		}

		SetModified(FALSE);
	}
}

void CAddressBookFieldEditorPictureWrapper::Refresh(void)
{
	CFlatFileDatabaseField* pField = GetField();
	ASSERT(pField != NULL);
	if (pField != NULL)
	{
		CAddressBookFieldEditorPicture* pEditor = GetEditor();
		ASSERT(pEditor != NULL);
		if (pEditor != NULL)
		{
			// Refresh modified flag.
			SetModified(pField->IsModified());

			// Set the editor picture to the field's blob data.
			DWORD dwNumber = pField->GetAsUnsigned32();
			if (dwNumber == 0)
			{
				pEditor->SetBitmap("", NULL);
			}
			else
			{
				// Load the blob data from the database.
				CString csName;
				LPBITMAPINFO pData = NULL;
				TRY
				{
					if (LoadPictureBlob(pField->GetDatabase(), csName, pData, dwNumber))
					{
						// Hand over the data to the editor.
						pEditor->SetBitmap(csName, pData);
						pData = NULL;
						csName.Empty();
					}
				}
				END_TRY
			}
		}
	}
}

void CAddressBookFieldEditorPictureWrapper::CreateEditor(CWnd* pParent)
{
	TRY
	{
		DestroyEditor();
		m_pEditor = new CAddressBookFieldEditorPicture(this, pParent);
	}
	END_TRY
}

BOOL CAddressBookFieldEditorPictureWrapper::SavePictureBlob(CFlatFileDatabase* pDatabase, LPCSTR pszName, LPBITMAPINFO pData, DB_RECORD_NUMBER& dwNumber)
{
	ASSERT(pDatabase != NULL);
	ASSERT(pszName != NULL);
	ASSERT(pData != NULL);

	BOOL fResult = FALSE;

	LPBYTE pCompressedData = NULL;
	DWORD dwCompressedData = 0;
	TRY
	{
		// Initialize the record number to 0 (failure).
		dwNumber = 0;

		// Get the size of the name.
		DWORD dwName = strlen(pszName)+1;

		// Compute the uncompressed size of the bitmap data.
		DWORD dwData = sizeof(BITMAPINFO)+255*sizeof(RGBQUAD)+pData->bmiHeader.biSizeImage;

		// Compress the data, always use CDeflate.
		DWORD dwCompression = BLOB_COMPRESS_DEFLATE;

		CDeflate Deflate;
		if (Deflate.Compress((LPBYTE)pData, dwData, &pCompressedData, &dwCompressedData))
		{
			if (pCompressedData != NULL)
			{
				TRACE("Saving address book picture: %lu (%lu compressed)\n", dwData, dwCompressedData);
		
				// We have all the pieces, create a blob for it.
				MemoryDevice Device;
				DWORD dwType = BLOB_TYPE_PICTURE;
				Device.AddPiece(&dwType, sizeof(dwType));								// blob type (picture)
				Device.AddPiece(&dwName, sizeof(dwName));								// name length
				Device.AddPiece((LPVOID)(LPCSTR)pszName, dwName);					// name
				Device.AddPiece(&dwCompression, sizeof(dwCompression));			// bitmap compression type
				Device.AddPiece(&dwCompressedData, sizeof(dwCompressedData));	// bitmap length
				Device.AddPiece((LPVOID)pCompressedData, dwCompressedData);		// compressed bitmap data

				// Write out the new blob to the database and return the record number.
				pDatabase->SetBlobData(&Device, &dwNumber);

				fResult = TRUE;
			}
		}
	}
	END_TRY

	// Free any compressed data.
	CDeflate::FreeData(pCompressedData);

	// If we did not have success, free any blob record that was created.
	if (!fResult)
	{
		pDatabase->DeleteRecord(dwNumber);
		dwNumber = 0;
	}

	return fResult;
}

BOOL CAddressBookFieldEditorPictureWrapper::LoadPictureBlob(CFlatFileDatabase* pDatabase, CString& csName, LPBITMAPINFO& pData, DB_RECORD_NUMBER dwNumber)
{
	BOOL fResult = FALSE;

	ASSERT(pDatabase != NULL);

	StorageFile File;
	TRY
	{
		// Initialize output variables.
		csName.Empty();
		pData = NULL;

		ERRORCODE Error;

		// Set up a storage file to access the blob data.
		if ((Error = pDatabase->GetBlobDataAsFile(&File, dwNumber)) != ERRORCODE_None)
		{
			ThrowErrorcodeException(Error);
		}

		// Get the blob size.
		DWORD dwSize;
		if ((Error = File.length((ST_DEV_POSITION*)&dwSize)) != ERRORCODE_None)
		{
			ThrowErrorcodeException(Error);
		}

		// Read the type of blob.
		DWORD dwType;
		if ((Error = File.huge_read(&dwType, sizeof(dwType), NULL)) != ERRORCODE_None)
		{
			ThrowErrorcodeException(Error);
		}

		// Make sure it's a picture.
		ASSERT(dwType == BLOB_TYPE_PICTURE);
		if (dwType == BLOB_TYPE_PICTURE)
		{
			dwSize -= sizeof(dwType);

			// Read the name size.
			DWORD dwName;
			if ((Error = File.huge_read(&dwName, sizeof(dwName), NULL)) != ERRORCODE_None)
			{
				ThrowErrorcodeException(Error);
			}

			dwSize -= sizeof(dwName);

			// Read the name.
			if (dwName > 0)
			{
				LPSTR pBuffer = csName.GetBuffer((int)(dwName-1));
				ASSERT(pBuffer != NULL);
				if ((Error = File.huge_read(pBuffer, dwName, NULL)) != ERRORCODE_None)
				{
					ThrowErrorcodeException(Error);
				}

				csName.ReleaseBuffer();
				dwSize -= dwName;

				// Read the bitmap compression type.
				DWORD dwCompression;
				if ((Error = File.huge_read(&dwCompression, sizeof(dwCompression), NULL)) != ERRORCODE_None)
				{
					ThrowErrorcodeException(Error);
				}

				dwSize -= sizeof(dwCompression);

				// Read the bitmap size.
				DWORD dwData;
				if ((Error = File.huge_read(&dwData, sizeof(dwData), NULL)) != ERRORCODE_None)
				{
					ThrowErrorcodeException(Error);
				}

				dwSize -= sizeof(dwData);

				// Read the data.
				dwSize -= dwData;
				ASSERT(dwSize == 0);
				if (dwSize != 0)
				{
					ThrowErrorcodeException(ERRORCODE_IntError);
				}

				pData = (LPBITMAPINFO)new BYTE[dwData];
				ASSERT(pData != NULL);
				if ((Error = File.huge_read((LPBYTE)pData, dwData, NULL)) != ERRORCODE_None)
				{
					ThrowErrorcodeException(ERRORCODE_IntError);
				}

				// We have the compressed data, uncompress it.
				switch (dwCompression)
				{
					case BLOB_COMPRESS_NONE:
					{
						// Nothing to do, pData is properly set.
						break;
					}
					case BLOB_COMPRESS_DEFLATE:
					{
						LPBYTE pDecompressedData = NULL;
						DWORD dwDecompressedData = 0;

						TRY
						{
							// Deflate the data.
							BOOL fDecompressed = FALSE;
							TRY
							{
								CDeflate Deflate;
								fDecompressed = Deflate.Decompress((LPBYTE)pData, dwData, &pDecompressedData, &dwDecompressedData);
							}
							END_TRY

							// Get rid of compressed data.
							delete [] (BYTE*)pData;
							pData = NULL;

							// If decompression was successful, copy it to memory that we allocate.
							if (fDecompressed)
							{
								pData = (LPBITMAPINFO)new BYTE[dwDecompressedData];
								ASSERT(pData != NULL);
								memcpy(pData, pDecompressedData, dwDecompressedData);
							}
						}
						END_TRY
						
						// Free the decompressed data.
						CDeflate::FreeData(pDecompressedData);

						break;
					}
					default:
					{
						ThrowErrorcodeException(ERRORCODE_IntError);
					}
				}

				// The data, if it exists, has been decompressed.
				fResult = (pData != NULL);
			}
		}
	}
	END_TRY

	// If we did not have success, reset the output variables.
	if (!fResult)
	{
		TRY
		{
			csName.Empty();
			delete [] (BYTE*)pData;
			pData = NULL;
		}
		END_TRY
	}

	return fResult;
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorNormal
//
// The "normal" field editor which lets users type in text.

WNDPROC CAddressBookFieldEditorNormal::m_pfnSuperWndProc = NULL;

CAddressBookFieldEditorNormal::CAddressBookFieldEditorNormal(CAddressBookFieldEditor* pWrapper, CWnd* pParent)
{
	m_pWrapper = pWrapper;

	ASSERT(m_pWrapper != NULL);
	if (m_pWrapper != NULL)
	{
		// Create the editor.
		CFlatFileDatabaseField* pField = GetField();
		ASSERT(pField != NULL);
		if (pField != NULL)
		{
			CRect crPosition(0,0,0,0);
			Create(WS_CHILD | /*WS_CLIPSIBLINGS |*/ ES_AUTOHSCROLL, crPosition, pParent, (UINT)-1);
			SetFont(GetParent()->GetFont());
			LimitText((int)(pField->GetType() == FFT_String ? pField->GetSize() : 10));
		}
	}
}

CAddressBookFieldEditorNormal::~CAddressBookFieldEditorNormal()
{
}

BEGIN_MESSAGE_MAP(CAddressBookFieldEditorNormal, CEdit)
	//{{AFX_MSG_MAP(CAddressBookFieldEditorNormal)
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAddressBookFieldEditorNormal::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL fResult = INHERITED::PreCreateWindow(cs);
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorNormal message handlers

void CAddressBookFieldEditorNormal::OnLButtonDown(UINT nFlags, CPoint point)
{
	INHERITED::OnLButtonDown(nFlags, point);
	if (IsEditing())
	{
		CPoint cpScreen(point);
		ClientToScreen(&cpScreen);
		int nHitTest = (int)SendMessage(WM_NCHITTEST, (WPARAM)0, MAKELPARAM(cpScreen.x, cpScreen.y));
		SendMessage(WM_SETCURSOR, (WPARAM)GetSafeHwnd(), MAKELPARAM(nHitTest, WM_LBUTTONDOWN));
	}
}

void CAddressBookFieldEditorNormal::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_RETURN:
		case VK_ESCAPE:
		case VK_TAB:
		{
			// Pass it up to our parent.
			const MSG* pMsg = GetCurrentMessage();
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;
		}
		default:
		{
			INHERITED::OnChar(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

void CAddressBookFieldEditorNormal::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_RETURN:
		case VK_ESCAPE:
		case VK_UP:
		case VK_DOWN:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_TAB:
		{
			// Pass it up to our parent.
			const MSG* pMsg = GetCurrentMessage();
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;
		}
		default:
		{
			INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

UINT CAddressBookFieldEditorNormal::OnGetDlgCode()
{
	UINT uRet = INHERITED::OnGetDlgCode() | DLGC_WANTTAB;
	if (Util::IsKeyMessage((LPMSG)(GetCurrentMessage()->lParam), VK_RETURN, VK_ESCAPE, 0))
	{
		uRet |= DLGC_WANTMESSAGE;
	}
	return uRet;
}

void CAddressBookFieldEditorNormal::OnSetFocus(CWnd* pOldWnd)
{
	INHERITED::OnSetFocus(pOldWnd);
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		pParent->PostMessage(WM_EDITOR_ACTIVATED, 0, (LPARAM)GetWrapper());
	}
}

void CAddressBookFieldEditorNormal::OnKillFocus(CWnd* pNewWnd)
{
	INHERITED::OnKillFocus(pNewWnd);
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		pParent->PostMessage(WM_EDITOR_DEACTIVATED, 0, (LPARAM)GetWrapper());
	}
}

BOOL CAddressBookFieldEditorNormal::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// Don't change the cursor if the control is not focused.
	if (!IsEditing())
	{
		return TRUE;
	}
	return INHERITED::OnSetCursor(pWnd, nHitTest, message);
}

void CAddressBookFieldEditorNormal::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!IsEditing())
	{
		// Translate point to parent coordinates and pass the message up.
		ClientToScreen(&point);
		GetParent()->ScreenToClient(&point);
		GetParent()->SendMessage(WM_MOUSEMOVE, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
	}
	else
	{
		INHERITED::OnMouseMove(nFlags, point);
	}
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorListButton
//
// The list box drop button used by the list field editor.

WNDPROC CAddressBookFieldEditorListButton::m_pfnSuperWndProc = NULL;

CAddressBookFieldEditorListButton::CAddressBookFieldEditorListButton()
{
	m_pBitmap = NULL;
	m_nBitmapWidth = 0;
	m_nBitmapHeight = 0;
	m_fSelect = FALSE;
}

CAddressBookFieldEditorListButton::~CAddressBookFieldEditorListButton()
{
	DestroyWindow();
}

BOOL CAddressBookFieldEditorListButton::CreateControl(DWORD dwStyle, CWnd* pParent, UINT nID)
{
	BOOL fResult = CreateEx(0, "MLSComboButton", "Combo Button", dwStyle, 0, 0, 0, 0, pParent->GetSafeHwnd(), (HMENU)nID, NULL);
	
	if (!fResult)
	{
		AfxMessageBox(IDS_NO_LIST_BUTTON);
	}
	
	return fResult;
}

BEGIN_MESSAGE_MAP(CAddressBookFieldEditorListButton, CWnd)
	//{{AFX_MSG_MAP(CAddressBookFieldEditorListButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAddressBookFieldEditorListButton::SetBitmap(CBitmap* pBitmap)
{
	m_pBitmap = pBitmap;
	BITMAP Bitmap;
	if (Util::GetBitmap(m_pBitmap, &Bitmap))
	{
		// Remember bitmap dimensions
		m_nBitmapWidth = Bitmap.bmWidth;
		m_nBitmapHeight = Bitmap.bmHeight;

		// Size ourselves to the bitmap with room for border.
		SetWindowPos(NULL, 0, 0, 2+m_nBitmapWidth+2, 2+m_nBitmapHeight+2, SWP_NOZORDER|SWP_NOMOVE);
	}
}

void CAddressBookFieldEditorListButton::SetSelect(BOOL fSelect /*=TRUE*/)
{
	if (m_fSelect != fSelect)
	{
		// Selection changed.
		m_fSelect = fSelect;
		Invalidate();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorListButton message handlers

void CAddressBookFieldEditorListButton::OnPaint() 
{
	CPaintDC dc(this);

	// Upack the state of the button.
	BOOL fSelected = GetSelect();
	BOOL fFocused = FALSE;
	BOOL fDisabled = FALSE;
	BOOL fDefault = FALSE;

	// Create a bitmap which will be used to draw the button image.
	// When the bitmap is complete, it will be drawn onto the button.
	CRect crButton;
	GetClientRect(crButton);

	CBitmap bmButton;
	bmButton.CreateCompatibleBitmap(&dc, crButton.Width(), crButton.Height());

	CDC dcButton;
	dcButton.CreateCompatibleDC(&dc);

	dcButton.SelectObject(&bmButton);

	// Initialize the button bitmap to the button face color.
	Util::FillRectangleWithColor(dcButton, crButton, GetSysColor(COLOR_BTNFACE));

	// Compute the area available for content.
	CRect crContent(crButton);

	// Compute the bitmap dimensions and its nominal position.
	CRect crBitmap;
	crBitmap.SetRectEmpty();

	if (!crContent.IsRectEmpty())
	{
		if (m_pBitmap != NULL)
		{
			crBitmap.SetRect(0, 0, m_nBitmapWidth, m_nBitmapHeight);
			crBitmap.OffsetRect((crContent.Width()-crBitmap.Width())/2, (crContent.Height()-crBitmap.Height())/2);
		}
	}

	// If the button is selected, offset the bitmap.
	if (fSelected)
	{
		crBitmap.OffsetRect(1, 1);
	}

	// Draw the bitmap onto the button image at the computed position.
	if (!crBitmap.IsRectEmpty())
	{
		CRect crSource;
		crSource.SetRectEmpty();
		Util::TransparentBltBitmap(dcButton, crBitmap, *m_pBitmap, crSource);
	}

	// If the button is disabled, indicate this by embossing the bitmap.
	if (fDisabled)
	{
		Util::Emboss(dcButton, crButton, GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHILIGHT));
	}

	// Draw the button in its current state.
	if (fSelected)
	{
		// Button is selected, draw a dark border.
		CRect crDraw(crButton);
		Util::DrawBorderWithColor(dcButton, crDraw, GetSysColor(COLOR_BTNSHADOW), 1);
	}
	else
	{
		CRect crDraw(crButton);

		// If the button is focused or the default button, draw a
		// black border around it. The rest of the image moves in
		// to make room.
		if (fFocused || fDefault)
		{
			Util::DrawBorderWithColor(dcButton, crDraw, GetSysColor(COLOR_BTNTEXT), 1);
			crDraw.InflateRect(-1, -1);
		}

		// Draw the raised 3D border:
		//
		//    L-----------LB
		//    |W---------WD|
		//    ||          ||
		//    ||          ||
		//    |W          ||
		//    LD----------D|
		//    B------------B


		CRect crSide;

		// L horizontal
		crSide.SetRect(crDraw.left, crDraw.top, crDraw.right-1, crDraw.top+1);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNFACE));

		// L vertical
		crSide.SetRect(crDraw.left, crDraw.top+1, crDraw.left+1, crDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNFACE));

		// W horizontal
		crSide.SetRect(crDraw.left+1, crDraw.top+1, crDraw.right-2, crDraw.top+2);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNHILIGHT));

		// W vertical
		crSide.SetRect(crDraw.left+1, crDraw.top+2, crDraw.left+2, crDraw.bottom-2);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNHILIGHT));

		// B horizontal
		crSide.SetRect(crDraw.left, crDraw.bottom-1, crDraw.right, crDraw.bottom);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNTEXT));

		// B vertical
		crSide.SetRect(crDraw.right-1, crDraw.top, crDraw.right, crDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNTEXT));

		// D horizontal
		crSide.SetRect(crDraw.left+1, crDraw.bottom-2, crDraw.right-1, crDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNSHADOW));

		// D vertical
		crSide.SetRect(crDraw.right-2, crDraw.top+1, crDraw.right-1, crDraw.bottom-2);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNSHADOW));
	}

	// If the button is focused, draw the focus rectangle.
	if (fFocused)
	{
		CRect crDraw(crButton);
		crDraw.InflateRect(-4, -4);
		dcButton.DrawFocusRect(crDraw);
	}

	// Copy the bitmap onto the button.
	dc.BitBlt(crButton.left, crButton.top, crButton.Width(), crButton.Height(), &dcButton, 0, 0, SRCCOPY);

	dcButton.DeleteDC();
	bmButton.DeleteObject();
}

void CAddressBookFieldEditorListButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Let our owner know that the button was pressed.
	CWnd* pOwner = GetOwner();
	if (pOwner != NULL)
	{
		pOwner->SendMessage(WM_LIST_BUTTON_CLICKED, (WPARAM)0, (LPARAM)0);
	}

	// Set the state to selected and capture the mouse until the button is released.
	SetSelect(TRUE);
	SetCapture();
}

void CAddressBookFieldEditorListButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// The user finally let go, pop the button back up.
	SetSelect(FALSE);
	ReleaseCapture();
}

void CAddressBookFieldEditorListButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// The user is moving the mouse around willy-nilly. If the mouse is
	// moved off the button, pop it up until it's moved back on.
	if (GetCapture() == this)
	{
		CRect crClient;
		GetClientRect(crClient);
		SetSelect(crClient.PtInRect(point));
	}
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorListBox
//
// The list box used by the list field editor.

WNDPROC CAddressBookFieldEditorListBox::m_pfnSuperWndProc = NULL;

CAddressBookFieldEditorListBox::CAddressBookFieldEditorListBox()
{
}

CAddressBookFieldEditorListBox::~CAddressBookFieldEditorListBox()
{
}

BOOL CAddressBookFieldEditorListBox::CreateControl(DWORD dwStyle, CWnd* pParent, UINT nID)
{
	BOOL fResult = CreateEx(0, "MLSComboList", "Combo ListBox", dwStyle, 0, 0, 0, 0, pParent->GetSafeHwnd(), (HMENU)nID, NULL);
	
	if (!fResult)
	{
		AfxMessageBox(IDS_NO_LIST_BOX);
	}
	
	return fResult;
}

BEGIN_MESSAGE_MAP(CAddressBookFieldEditorListBox, CListBox)
	//{{AFX_MSG_MAP(CAddressBookFieldEditorListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Helper which sends a message to our owner indicating a new list box item has been selected.
void CAddressBookFieldEditorListBox::SelectIfChanged(BOOL fMouseSelected, int nOldSelection /*=LB_ERR*/)
{
	int nNewSelection = GetCurSel();
	if ((nNewSelection != LB_ERR) && (nNewSelection != nOldSelection))
	{
		CWnd* pOwner = GetOwner();
		if (pOwner != NULL)
		{
			pOwner->PostMessage(WM_LIST_ITEM_SELECTED, (WPARAM)fMouseSelected, (LPARAM)GetCurSel());
		}
	}
}

// Return the index of the item containing the specified point (or -1).
int CAddressBookFieldEditorListBox::Index(const CPoint& cpPoint)
{
	CRect crClient;
	GetClientRect(crClient);
	
	int nCount = GetCount();
	int nIndex = GetTopIndex();
	
	while (nIndex < nCount)
	{
		CRect crItem;
		
		if (GetItemRect(nIndex, crItem) != LB_ERR)
		{
			if (crItem.top >= crClient.bottom)
			{
				break;
			}
			
			if (cpPoint.y < crItem.top)
			{
				nIndex--;
				
				if (nIndex < 0)
				{
					nIndex = 0;
				}
				
				break;
			}
			
			if (cpPoint.y < crItem.bottom)
			{
				break;
			}
		}
		
		nIndex++;
	}

	if (nIndex == nCount)
	{
		// Past last item, return -1.
		nIndex = -1;
	}
	
	return nIndex;
}
	
/////////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorListBox message handlers

void CAddressBookFieldEditorListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	SelectIfChanged(TRUE);
}

void CAddressBookFieldEditorListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	int nItem = Index(point);
	if (nItem != -1)
	{
		if (nItem != GetCurSel())
		{
			SetCurSel(nItem);
		}
	}
#if 0
	BOOL fOutside;
	int nItem = ItemFromPoint(point, fOutside);
	if (!fOutside && (nItem != LB_ERR))
	{
		if (nItem != GetCurSel())
		{
			SetCurSel(nItem);
		}
	}
#endif
}

void CAddressBookFieldEditorListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Get current selection, process the character, and check if the selected changed.
	int nSelection = GetCurSel();
	INHERITED::OnChar(nChar, nRepCnt, nFlags);
	SelectIfChanged(FALSE, nSelection);
}

void CAddressBookFieldEditorListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Get current selection, process the key, and check if the selected changed.
	int nSelection = GetCurSel();
	INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
	SelectIfChanged(FALSE, nSelection);
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorList
//
// The list field editor which lets users type in text of select from a list.

CAddressBookFieldEditorList::CAddressBookFieldEditorList(CAddressBookFieldEditor* pWrapper, CWnd* pParent) :
	CAddressBookFieldEditorNormal(pWrapper, pParent)
{
	ASSERT(m_pWrapper != NULL);
	if (m_pWrapper != NULL)
	{
		// Load the button bitmap.
		m_bmButton.LoadBitmap(IDB_COMBOBUTTON);

		// Create button.
		m_Button.CreateControl(WS_CHILD, pParent, 0);
		VERIFY(m_Button.m_hWnd != NULL);
		m_Button.BringWindowToTop();
		m_Button.SetOwner(this);
		m_Button.SetFont(pParent->GetFont());
		m_Button.SetBitmap(&m_bmButton);

		// Button is initially hidden.
		m_fButtonHidden = TRUE;

		// Create list box.
		m_List.CreateControl(WS_CHILD | /*WS_CLIPSIBLINGS |*/ WS_BORDER | WS_VSCROLL | LBS_SORT, GetDesktopWindow(), 0);
		VERIFY(m_List.m_hWnd != NULL);
		m_List.BringWindowToTop();
		m_List.SetOwner(this);
		m_List.SetFont(pParent->GetFont());
	}
}

CAddressBookFieldEditorList::~CAddressBookFieldEditorList()
{
	if (m_Button.m_hWnd != NULL)
	{
		m_Button.DestroyWindow();
	}
	if (m_List.m_hWnd != NULL)
	{
		m_List.DestroyWindow();
	}
}

BEGIN_MESSAGE_MAP(CAddressBookFieldEditorList, CAddressBookFieldEditorNormal)
	//{{AFX_MSG_MAP(CAddressBookFieldEditorList)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_LIST_ITEM_SELECTED, OnListItemSelected)
	ON_MESSAGE(WM_LIST_BUTTON_CLICKED, OnListButtonClicked)
END_MESSAGE_MAP()

void CAddressBookFieldEditorList::Move(const CRect& crPosition)
{
	// Compute position of button.
	CRect crButton;
	m_Button.GetClientRect(crButton);
	crButton.OffsetRect(crPosition.right-crButton.Width(), crPosition.top+(crPosition.Height()-crButton.Height())/2);

	// Compute position of edit.
	CRect crEdit(crPosition);
	crEdit.right = crButton.left;

	// Get rid of the button if we don't have much room.
	if (crButton.IsRectEmpty() || crEdit.IsRectEmpty())
	{
		m_Button.ShowWindow(SW_HIDE);
		m_fButtonHidden = TRUE;

		// Adjust edit bounds to full width since there's no button.
		crEdit.right = crPosition.right;
	}
	else
	{
		// Position the button.
		MoveChildWindow(&m_Button, crButton);
		m_Button.ShowWindow(SW_SHOW);
		m_fButtonHidden = FALSE;
	}

	// Position the edit control.
	MoveChildWindow(this, crEdit);
}

void CAddressBookFieldEditorList::Show(BOOL fShow /*=TRUE*/)
{
	if (fShow)
	{
		ShowWindow(SW_SHOW);
		if (!m_fButtonHidden)
		{
			m_Button.ShowWindow(SW_SHOW);
		}
	}
	else
	{
		ShowWindow(SW_HIDE);
		m_Button.ShowWindow(SW_HIDE);
	}
}

void CAddressBookFieldEditorList::HideList(void)
{
	if (m_List.m_hWnd != NULL)
	{
		m_List.ShowWindow(SW_HIDE);
	}
}

void CAddressBookFieldEditorList::ShowList(void)
{
	if (m_List.m_hWnd != NULL)
	{
		CString csText;
		TRY
		{
			GetWindowText(csText);
			int nIndex = m_List.FindStringExact(-1, csText);
			if (nIndex != LB_ERR)
			{
				BOOL fModified = IsModified();
				m_List.GetText(nIndex, csText);
				SetWindowText(csText);
				SetModify(fModified);
			}
		}
		END_TRY

		CRect crList;
		ListRectangle(crList);
		m_List.MoveWindow(crList);
		m_List.BringWindowToTop();
		m_List.ShowWindow(SW_SHOW);
		m_List.EnableWindow(TRUE);
	}
}

void CAddressBookFieldEditorList::ListRectangle(CRect& crList)
{
	GetWindowRect(crList);

	crList.top = crList.bottom+2;
	crList.bottom += 100;

	if (m_Button.IsWindowVisible())
	{
		CRect crButton;
		m_Button.GetWindowRect(crButton);
		crList.right = crButton.right;
	}
}

void CAddressBookFieldEditorList::FindString(void)
{
	if (m_List.m_hWnd != NULL)
	{
		m_List.SetCurSel(-1);
		CString csText;
		CString csItem;
		int nMaxItem = -1;
		int nMaxScore = -1;
		TRY
		{
			GetWindowText(csText);
			csText.MakeUpper();
			int nTextLength = csText.GetLength();
			for (int nItem = 0; nItem < m_List.GetCount(); nItem++)
			{
				m_List.GetText(nItem, csItem);
				csItem.MakeUpper();

				int nLength = csItem.GetLength();
				if (nLength > nTextLength)
				{
					nLength = nTextLength;
				}

				for (int nScore = 0; nScore < nLength; nScore++)
				{
					if (csItem[nScore] != csText[nScore])
					{
						break;
					}
				}

				if (nScore > nMaxScore)
				{
					nMaxItem = nItem;
					nMaxScore = nScore;
				}
			}
		}
		END_TRY

		if (nMaxItem != -1)
		{
			m_List.SetCurSel(nMaxItem);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorList message handlers

void CAddressBookFieldEditorList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_ESCAPE:
		{
			// Pass it up to our parent.
			const MSG* pMsg = GetCurrentMessage();
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;
		}
		default:
		{
			INHERITED::OnChar(nChar, nRepCnt, nFlags);
			FindString();
			break;
		}
	}
}

void CAddressBookFieldEditorList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_ESCAPE:
		case VK_TAB:
		{
			// Force list up on Escape.
			HideList();

			// Pass it up to our parent.
			const MSG* pMsg = GetCurrentMessage();
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;
		}
		case VK_UP:
		case VK_DOWN:
		case VK_NEXT:
		case VK_PRIOR:
		{
			// If the list box is showing, use the special navigation keys.
			if ((m_List.m_hWnd != NULL) && (m_List.IsWindowVisible()))
			{
				m_List.SendMessage(WM_KEYDOWN, (WPARAM)nChar, MAKELPARAM(nRepCnt, nFlags));
			}
			else
			{
				// Process special key in editor.
				INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
			}
			break;
		}
		default:
		{
			INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

void CAddressBookFieldEditorList::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_UP:
		case VK_DOWN:
		{
			if (m_List.m_hWnd != NULL)
			{
				if (m_List.IsWindowVisible())
				{
					HideList();
				}
				else
				{
					ShowList();
				}
			}
			break;
		}
		default:
		{
			INHERITED::OnSysKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

void CAddressBookFieldEditorList::OnSetFocus(CWnd* pOldWnd)
{
	INHERITED::OnSetFocus(pOldWnd);

	CFlatFileDatabaseField* pField = GetField();
	ASSERT(pField != NULL);
	if (pField != NULL)
	{
		// Build the list contents.
		m_List.ResetContent();
		if (pField != NULL)
		{
			CSortedPtrArray* pList = (CSortedPtrArray*)(GetParent()->SendMessage(WM_GET_FIELD_LIST, (WPARAM)TRUE, (LPARAM)(pField->GetDescription())));
			if (pList != NULL)
			{
				for (int i = 0; i < pList->GetSize(); i++)
				{
					m_List.AddString(pList->GetAt(i)->GetAsString());
				}
			}

			// Add in the fixed entries based on the list type.
			if (FIELD_USER_DATA_EDITOR(pField->GetDescription()->GetUserData()) == FIELD_EDIT_LIST)
			{
				// Check for special types of lists with additional fixed entries.
				switch (FIELD_USER_DATA_WORD(pField->GetDescription()->GetUserData()))
				{
					case FIELD_LIST_RELATIONSHIP:
					{
						// Relationship list, add standard fixed values.
						// MLSTODO: Move these strings out of code!
						// JRO
                  /*
                  static char *pszRelationships[] =
						{
							"Client",
							"Family",
							"Friend",
							"Vendor",
						};
                  */
                  const char *pszRelationships[] =
						{
						   GET_PMWAPP()->GetResourceStringPointer (IDS_CLIENT),
                     GET_PMWAPP()->GetResourceStringPointer (IDS_FAMILY),
                     GET_PMWAPP()->GetResourceStringPointer (IDS_FRIEND),
                     GET_PMWAPP()->GetResourceStringPointer (IDS_VENDOR)
                  };

						for (int i = 0; i < sizeof(pszRelationships)/sizeof(pszRelationships[0]); i++)
						{
							if (m_List.FindStringExact(-1, pszRelationships[i]) == LB_ERR)
							{
								m_List.AddString(pszRelationships[i]);
							}
						}

						break;
					}
					case FIELD_LIST_COUNTRY:
					{
						// Relationship list, add standard fixed values.
						CString csCountry;
						UINT* pCountry = &(AddressBookCountries[0]);
						while (*pCountry != 0)
						{
							csCountry.LoadString(*pCountry);
							if (m_List.FindStringExact(-1, csCountry) == LB_ERR)
							{
								m_List.AddString(csCountry);
							}
							pCountry++;
						}

						break;
					}
				}
			}
		}
	}
}

void CAddressBookFieldEditorList::OnKillFocus(CWnd* pNewWnd)
{
	INHERITED::OnKillFocus(pNewWnd);
	HideList();
}

// This method is called when a WM_LIST_ITEM_SELECTED message is received from
// our combo list box. LPARAM is the index of the list item selected. WPARAM is
// nonzero if the selection was made with the mouse, in which case we hide the list.
LRESULT CAddressBookFieldEditorList::OnListItemSelected(WPARAM wParam, LPARAM lParam)
{
	if (wParam != 0)
	{
		HideList();
	}

	CString csText;
	TRY
	{
		m_List.GetText((int)lParam, csText);
	}
	END_TRY

	SetWindowText(csText);
	SetSel(0, -1);
	SetModified(TRUE);

	return 0;
}

LRESULT CAddressBookFieldEditorList::OnListButtonClicked(WPARAM wParam, LPARAM lParam)
{
	SetFocus();
	if (m_List.IsWindowVisible())
	{
		HideList();
	}
	else
	{
		ShowList();
	}
	return 0;
}

void CAddressBookFieldEditorList::OnCancelMode()
{
	INHERITED::OnCancelMode();
	HideList();
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorMemo
//
// The memo field editor which lets users type in text.

WNDPROC CAddressBookFieldEditorMemo::m_pfnSuperWndProc = NULL;

CAddressBookFieldEditorMemo::CAddressBookFieldEditorMemo(CAddressBookFieldEditor* pWrapper, CWnd* pParent)
{
	m_pWrapper = pWrapper;

	ASSERT(m_pWrapper != NULL);
	if (m_pWrapper != NULL)
	{
		// Create the editor.
		CRect crPosition(0,0,0,0);
		Create(WS_CHILD | /*WS_CLIPSIBLINGS |*/ WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN, crPosition, pParent, (UINT)-1);
		SetFont(pParent->GetFont());
		LimitText(1024);
	}
}

CAddressBookFieldEditorMemo::~CAddressBookFieldEditorMemo()
{
}

BEGIN_MESSAGE_MAP(CAddressBookFieldEditorMemo, CEdit)
	//{{AFX_MSG_MAP(CAddressBookFieldEditorMemo)
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorMemo message handlers

void CAddressBookFieldEditorMemo::OnLButtonDown(UINT nFlags, CPoint point)
{
	INHERITED::OnLButtonDown(nFlags, point);
	if (IsEditing())
	{
		CPoint cpScreen(point);
		ClientToScreen(&cpScreen);
		int nHitTest = (int)SendMessage(WM_NCHITTEST, (WPARAM)0, MAKELPARAM(cpScreen.x, cpScreen.y));
		SendMessage(WM_SETCURSOR, (WPARAM)GetSafeHwnd(), MAKELPARAM(nHitTest, WM_LBUTTONDOWN));
	}
}

void CAddressBookFieldEditorMemo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_ESCAPE:
		{
			// Pass it up to our parent.
			const MSG* pMsg = GetCurrentMessage();
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;
		}
		case VK_RETURN:
		{
			// Filter out returns if at first character in memo.
			int nStartChar;
			int nEndChar;
			GetSel(nStartChar, nEndChar);
			if ((nStartChar != 0) || (nEndChar != 0))
			{
				INHERITED::OnChar(nChar, nRepCnt, nFlags);
			}
			break;
		}
		default:
		{
			INHERITED::OnChar(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

void CAddressBookFieldEditorMemo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_ESCAPE:
		case VK_TAB:
		{
			// Pass it up to our parent.
			const MSG* pMsg = GetCurrentMessage();
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;
		}
		case VK_UP:
		case VK_DOWN:
		case VK_NEXT:
		case VK_PRIOR:
		{
			char szBuffer[2];
			if (GetWindowText(&(szBuffer[0]), sizeof(szBuffer)) == 0)
			{
				// Pass it up to our parent.
				const MSG* pMsg = GetCurrentMessage();
				GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			}
			else
			{
				INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
			}
			break;
		}
		case VK_RETURN:
		{
			int nStartChar;
			int nEndChar;
			GetSel(nStartChar, nEndChar);
			if ((nStartChar == 0) && (nEndChar == 0))
			{
				// Pass it up to our parent.
				const MSG* pMsg = GetCurrentMessage();
				GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			}
			else
			{
				INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
			}
			break;
		}
		default:
		{
			INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

UINT CAddressBookFieldEditorMemo::OnGetDlgCode()
{
	UINT uRet = INHERITED::OnGetDlgCode() | DLGC_WANTTAB;
	if (Util::IsKeyMessage((LPMSG)(GetCurrentMessage()->lParam), VK_RETURN, VK_ESCAPE, 0))
	{
		uRet |= DLGC_WANTMESSAGE;
	}
	return uRet;
}

void CAddressBookFieldEditorMemo::OnSetFocus(CWnd* pOldWnd)
{
	INHERITED::OnSetFocus(pOldWnd);
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		pParent->PostMessage(WM_EDITOR_ACTIVATED, 0, (LPARAM)GetWrapper());
	}
}

void CAddressBookFieldEditorMemo::OnKillFocus(CWnd* pNewWnd)
{
	INHERITED::OnKillFocus(pNewWnd);
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		pParent->PostMessage(WM_EDITOR_DEACTIVATED, 0, (LPARAM)GetWrapper());
	}
}

BOOL CAddressBookFieldEditorMemo::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// Don't change the cursor if the control is not focused.
	if (!IsEditing())
	{
		return TRUE;
	}
	return INHERITED::OnSetCursor(pWnd, nHitTest, message);
}

void CAddressBookFieldEditorMemo::OnMouseMove(UINT nFlags, CPoint point)
{
	// If we are not editing, let the parent handle this message.
	if (!IsEditing())
	{
		// Translate point to parent coordinates and pass the message up.
		ClientToScreen(&point);
		GetParent()->ScreenToClient(&point);
		GetParent()->SendMessage(WM_MOUSEMOVE, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
	}
	else
	{
		INHERITED::OnMouseMove(nFlags, point);
	}
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorPictureButton
//
// A special button window used by the picture editor.

WNDPROC CAddressBookFieldEditorPictureButton::m_pfnSuperWndProc = NULL;

CAddressBookFieldEditorPictureButton::CAddressBookFieldEditorPictureButton()
{
}

CAddressBookFieldEditorPictureButton::~CAddressBookFieldEditorPictureButton()
{
}

BEGIN_MESSAGE_MAP(CAddressBookFieldEditorPictureButton, CButton)
	//{{AFX_MSG_MAP(CAddressBookFieldEditorPictureButton)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorPictureButton message handlers

void CAddressBookFieldEditorPictureButton::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_ESCAPE:
		{
			// Pass it up to our parent.
			const MSG* pMsg = GetCurrentMessage();
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;
		}
		default:
		{
			INHERITED::OnChar(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

void CAddressBookFieldEditorPictureButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_RETURN:
		case VK_ESCAPE:
		case VK_TAB:
		case VK_UP:
		case VK_DOWN:
		case VK_NEXT:
		case VK_PRIOR:
		{
			// Pass it up to our parent.
			const MSG* pMsg = GetCurrentMessage();
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;
		}
		case VK_LEFT:
		{
			// Move to the previous button.
			CWnd* pWnd = GetWindow(GW_HWNDPREV);
			if (pWnd == NULL)
			{
				pWnd = GetWindow(GW_HWNDLAST);
			}
			if (pWnd != NULL)
			{
				pWnd->SetFocus();
			}
			break;
		}
		case VK_RIGHT:
		{
			// Move to the next button.
			CWnd* pWnd = GetWindow(GW_HWNDNEXT);
			if (pWnd == NULL)
			{
				pWnd = GetWindow(GW_HWNDFIRST);
			}
			if (pWnd != NULL)
			{
				pWnd->SetFocus();
			}
			break;
		}
#if 0
		case VK_RETURN:
		{
			// Simulate a button press.
			Util::SendWmCommand(GetParent(), GetDlgCtrlID(), NULL, 0);
			break;
		}
#endif
		default:
		{
			INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

UINT CAddressBookFieldEditorPictureButton::OnGetDlgCode()
{
	UINT uRet = INHERITED::OnGetDlgCode() | DLGC_WANTTAB | DLGC_WANTARROWS;
	if (Util::IsKeyMessage((LPMSG)(GetCurrentMessage()->lParam), VK_RETURN, VK_ESCAPE, 0))
	{
		uRet |= DLGC_WANTMESSAGE;
	}
	return uRet;
}

void CAddressBookFieldEditorPictureButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// Ownerdraw buttons get double clicks. Turn then into a single clicks.
	SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
}

void CAddressBookFieldEditorPictureButton::OnKillFocus(CWnd* pNewWnd) 
{
	INHERITED::OnKillFocus(pNewWnd);

	CAddressBookFieldEditorPicture* pParent = (CAddressBookFieldEditorPicture*)GetParent();
	if (pParent != NULL)
	{
		pParent->ShowButtons();
	}
}

void CAddressBookFieldEditorPictureButton::OnSetFocus(CWnd* pOldWnd) 
{
	INHERITED::OnSetFocus(pOldWnd);
	
	CAddressBookFieldEditorPicture* pParent = (CAddressBookFieldEditorPicture*)GetParent();
	if (pParent != NULL)
	{
		pParent->ShowButtons();
	}
}

void CAddressBookFieldEditorPictureButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Upack the state of the button.
	DWORD dwState = SendMessage(BM_GETSTATE, 0, 0);
	DWORD dwCode = SendMessage(WM_GETDLGCODE, 0, 0);
	BOOL fSelected = (dwState & 4) != 0;
	BOOL fFocused = (dwState & 8) != 0;
	BOOL fDisabled = !IsWindowEnabled();
	BOOL fDefault = FALSE;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// Create a bitmap which will be used to draw the button image.
	// When the bitmap is complete, it will be drawn onto the button.
	CRect crButton;
	GetClientRect(crButton);

	CBitmap bmButton;
	bmButton.CreateCompatibleBitmap(pDC, crButton.Width(), crButton.Height());

	CDC dcButton;
	dcButton.CreateCompatibleDC(pDC);

	dcButton.SelectObject(&bmButton);

	// Initialize the button bitmap to the button face color.
	Util::FillRectangleWithColor(dcButton, crButton, GetSysColor(COLOR_BTNFACE));

	// Compute the area available for content.
	CRect crContent(crButton);
	CRect crTextContent(crContent);
	crTextContent.InflateRect(-6, -6);

	// Compute the text dimensions and its nominal position.
	CRect crText;
	crText.SetRectEmpty();
	CString csText;

	if (!crTextContent.IsRectEmpty())
	{
		GetWindowText(csText);
		if (!csText.IsEmpty())
		{
			// Get the font which we are using for the text.
			CFont* pFont = GetFont();
			if (pFont != NULL)
			{
				// Select the font we are going to use for drawing the text.
				dcButton.SelectObject(pFont);

				// Compute the extent of the text. The text is formatted into the area next to
				// the button. If the text is multiple line, the full width is always used.
				crText.SetRect(0, 0, crTextContent.Width(), crTextContent.Height());
				int nTextHeight = Util::DrawText(&dcButton, csText, crText, DT_CALCRECT | DT_SINGLELINE);

				// Build the rectangle that has the dimensions of the text.
				crText.bottom = nTextHeight;

				// Clip to content dimensions.
				if (crText.Width() > crTextContent.Width())
				{
					crText.right = crTextContent.Width();
				}
				if (crText.Height() > crTextContent.Height())
				{
					crText.bottom = crTextContent.Height();
				}

				// Center text rectangle within content rectangle.
				crText.OffsetRect(
					crTextContent.left+(crTextContent.Width()-crText.Width())/2,
					crTextContent.top+(crTextContent.Height()-crText.Height())/2);
			}
		}
	}

	// If the button is selected, offset the content.
	if (fSelected)
	{
		crText.OffsetRect(1, 1);
	}

	// Draw the text onto the button image at the computed position.
	if (!crText.IsRectEmpty())
	{
		// Draw the text.
		dcButton.SetTextColor(GetSysColor(COLOR_BTNTEXT));
		dcButton.SetBkMode(TRANSPARENT);
		Util::DrawText(&dcButton, csText, crText, DT_SINGLELINE|DT_VCENTER);
	}

	// If the button is disabled, indicate this by embossing the content.
	if (fDisabled)
	{
		Util::Emboss(dcButton, crButton, GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHILIGHT));
	}

	// Draw the button in its current state.
	if (fSelected)
	{
		// Button is selected, draw a black border with a shadow border inside.
		CRect crDraw(crButton);
		Util::DrawBorderWithColor(dcButton, crDraw, GetSysColor(COLOR_BTNTEXT), 1);
		crDraw.InflateRect(-1, -1);
		Util::DrawBorderWithColor(dcButton, crDraw, GetSysColor(COLOR_BTNSHADOW), 1);
	}
	else
	{
		CRect crDraw(crButton);

#if 0
		// If the button is focused or the default button, draw a
		// black border around it. The rest of the image moves in
		// to make room.
		if (fFocused | fDefault)
		{
			Util::DrawBorderWithColor(dcButton, crDraw, GetSysColor(COLOR_BTNTEXT), 1);
			crDraw.InflateRect(-1, -1);
		}
#endif

		// Draw the raised 3D border:
		//
		//		W-----------WB
		//    |           D|
		//    |           ||
		//    |           ||
		//    WD----------D|
		//    B------------B


		CRect crSide;

		// W horizontal
		crSide.SetRect(crDraw.left, crDraw.top, crDraw.right-1, crDraw.top+1);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNHILIGHT));

		// W vertical
		crSide.SetRect(crDraw.left, crDraw.top+1, crDraw.left+1, crDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNHILIGHT));

		// B horizontal
		crSide.SetRect(crDraw.left, crDraw.bottom-1, crDraw.right, crDraw.bottom);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNTEXT));

		// B vertical
		crSide.SetRect(crDraw.right-1, crDraw.top, crDraw.right, crDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNTEXT));

		// D horizontal
		crSide.SetRect(crDraw.left+1, crDraw.bottom-2, crDraw.right-1, crDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNSHADOW));

		// D vertical
		crSide.SetRect(crDraw.right-2, crDraw.top+1, crDraw.right-1, crDraw.bottom-2);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNSHADOW));
	}

	// If the button is focused, draw the focus rectangle.
	if (fFocused)
	{
		CRect crDraw(crButton);
		crDraw.InflateRect(-4, -4);
		dcButton.DrawFocusRect(crDraw);
	}

	// Copy the bitmap onto the button.
	pDC->BitBlt(crButton.left, crButton.top, crButton.Width(), crButton.Height(), &dcButton, 0, 0, SRCCOPY);

	dcButton.DeleteDC();
	bmButton.DeleteObject();
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorPicture
//
// The picture field editor which lets users pick a picture.

CAddressBookFieldEditorPicture::CAddressBookFieldEditorPicture(CAddressBookFieldEditor* pWrapper, CWnd* pParent)
{
	m_pWrapper = pWrapper;

	ASSERT(m_pWrapper != NULL);
	if (m_pWrapper != NULL)
	{
		// Create the editor.
		CRect crPosition(0,0,0,0);
		Create(NULL, "Picture Editor", WS_CHILD /*| WS_CLIPSIBLINGS*/, crPosition, pParent, (UINT)-1);
		SetFont(pParent->GetFont());
	}

	m_fModified = FALSE;

	m_crPicture.SetRectEmpty();
	m_pBitmap = NULL;
}

CAddressBookFieldEditorPicture::~CAddressBookFieldEditorPicture()
{
	DeleteBitmap();
}

BEGIN_MESSAGE_MAP(CAddressBookFieldEditorPicture, CWnd)
	//{{AFX_MSG_MAP(CAddressBookFieldEditorPicture)
	ON_BN_CLICKED(ID_NEW_PICTURE, OnNewPicture)
	ON_BN_CLICKED(ID_DELETE_PICTURE, OnDeletePicture)
	ON_BN_CLICKED(ID_COPY_PICTURE, OnCopyPicture)
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_EDITOR_ACTIVATED, OnEditorActivated)
END_MESSAGE_MAP()

void CAddressBookFieldEditorPicture::Move(const CRect& crPosition)
{
	// Compute position of picture.
	m_crPicture = crPosition;
	m_crPicture.right = m_crPicture.left+(crPosition.Height()*4)/3;
	GetParent()->ClientToScreen(m_crPicture);
	ScreenToClient(m_crPicture);

	// Compute the positions of our buttons.
	CString csNew;
	CString csDelete;
	CString csCopy;
	CDC* pDC = NULL;
	CFont* pOldFont = NULL;
	TRY
	{
		m_NewButton.GetWindowText(csNew);
		m_DeleteButton.GetWindowText(csDelete);
		m_CopyButton.GetWindowText(csCopy);
		pDC = GetDC();
		if (pDC != NULL)
		{
			CFont* pFont = GetParent()->GetFont();
			pOldFont = pDC->SelectObject(pFont);
			if (pOldFont != NULL)
			{
				// Get text extents.
				CSize czNew = Util::GetTextExtent(pDC, csNew);
				CSize czDelete = Util::GetTextExtent(pDC, csDelete);
				CSize czCopy = Util::GetTextExtent(pDC, csCopy);

				// Add in room for borders.
				CSize czBorder(12,12);
				czNew += czBorder;
				czDelete += czBorder;
				czCopy += czBorder;

				// Compute button rectangles.
				CRect crNew(0, 0, czNew.cx, czNew.cy);
				CRect crDelete(0, 0, czDelete.cx, czDelete.cy);
				crDelete.OffsetRect(crNew.right, 0);
				CRect crCopy(0, 0, czCopy.cx, czCopy.cy);
				crCopy.OffsetRect(crDelete.right, 0);

				crNew.OffsetRect(3,3);
				crDelete.OffsetRect(3,3);
				crCopy.OffsetRect(3,3);

				// Position the windows.
				MoveChildWindow(this, crPosition);
				MoveChildWindow(&m_NewButton, crNew);
				MoveChildWindow(&m_DeleteButton, crDelete);
				MoveChildWindow(&m_CopyButton, crCopy);
			}
		}
	}
	END_TRY

	// Cleanup.
	if (pDC != NULL)
	{
		if (pOldFont != NULL)
		{
			pDC->SelectObject(pOldFont);
		}
		ReleaseDC(pDC);
		pDC = NULL;
	}
}

void CAddressBookFieldEditorPicture::ShowButtons(void)
{
	// Show the buttons if we're editing.
	int nCmdShow = IsEditing() ? SW_SHOW : SW_HIDE;
	if (m_NewButton.m_hWnd != NULL)		m_NewButton.ShowWindow(nCmdShow);
	if (m_DeleteButton.m_hWnd != NULL)	m_DeleteButton.ShowWindow(nCmdShow);
	if (m_CopyButton.m_hWnd != NULL)		m_CopyButton.ShowWindow(nCmdShow);
}

void CAddressBookFieldEditorPicture::GetImageRectangle(CRect& crImage)
{
	crImage.SetRectEmpty();

	if (m_pBitmap != NULL)
	{
		// Get image dimensions that preserve the aspect, but fit
		// inside the rectangle m_crPicture.
		int nWidth = (int)(m_pBitmap->bmiHeader.biWidth);
		int nHeight = (int)(m_pBitmap->bmiHeader.biHeight);
		if (nWidth > m_crPicture.Width())
		{
			nHeight = MulDiv(nHeight, m_crPicture.Width(), nWidth);
			nWidth = m_crPicture.Width();
		}
		if (nHeight > m_crPicture.Height())
		{
			nWidth = MulDiv(nWidth, m_crPicture.Height(), nHeight);
			nHeight = m_crPicture.Height();
		}

		// Center the image vertically.
		crImage.SetRect(0, 0, nWidth, nHeight);
		crImage.OffsetRect(0, (m_crPicture.Height()-crImage.Height())/2);
	}
}

void CAddressBookFieldEditorPicture::ExcludeButtonRectangles(CDC& dc)
{
	if (m_NewButton.IsWindowVisible())
	{
		CRect crRect;
		m_NewButton.GetWindowRect(crRect);
		ScreenToClient(crRect);
		dc.ExcludeClipRect(crRect);
	}
	if (m_DeleteButton.IsWindowVisible())
	{
		CRect crRect;
		m_DeleteButton.GetWindowRect(crRect);
		ScreenToClient(crRect);
		dc.ExcludeClipRect(crRect);
	}
	if (m_CopyButton.IsWindowVisible())
	{
		CRect crRect;
		m_CopyButton.GetWindowRect(crRect);
		ScreenToClient(crRect);
		dc.ExcludeClipRect(crRect);
	}
}

void CAddressBookFieldEditorPicture::SetBitmap(LPCSTR pszName, LPBITMAPINFO pData)
{
	TRY
	{
		// Delete any existing bitmap.
		DeleteBitmap();

		// Set the new bitmap.
		m_csName = pszName;
		m_pBitmap = pData;

		// Clear modified flag.
		SetModified(FALSE);
	}
	CATCH_ALL(e)
	{
		DeleteBitmap();
	}
	END_CATCH_ALL

	// Readraw ourselves with the new bitmap.
	Invalidate();
}

void CAddressBookFieldEditorPicture::DeleteBitmap(void)
{
	delete [] (BYTE*)m_pBitmap;
	m_pBitmap = NULL;
	m_csName.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorPicture message handlers

int CAddressBookFieldEditorPicture::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (INHERITED::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString csNew;
	CString csDelete;
	CString csCopy;
	TRY
	{
		csNew.LoadString(IDS_NEW_PICTURE);
		csDelete.LoadString(IDS_DELETE_PICTURE);
		csCopy.LoadString(IDS_COPY_PICTURE);
	}
	END_TRY

	CRect crButton(0,0,0,0);
	m_NewButton.Create(csNew, WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, crButton, this, ID_NEW_PICTURE);
	m_NewButton.SetFont(GetParent()->GetFont());
	m_DeleteButton.Create(csDelete, WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, crButton, this, ID_DELETE_PICTURE);
	m_DeleteButton.SetFont(GetParent()->GetFont());
	m_CopyButton.Create(csCopy, WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, crButton, this, ID_COPY_PICTURE);
	m_CopyButton.SetFont(GetParent()->GetFont());
	
	return 0;
}

void CAddressBookFieldEditorPicture::OnDestroy() 
{
	INHERITED::OnDestroy();
}

void CAddressBookFieldEditorPicture::OnLButtonDown(UINT nFlags, CPoint point)
{
	INHERITED::OnLButtonDown(nFlags, point);
	SetFocus();
}

void CAddressBookFieldEditorPicture::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_ESCAPE:
		{
			// Pass it up to our parent.
			const MSG* pMsg = GetCurrentMessage();
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;
		}
		default:
		{
			INHERITED::OnChar(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

void CAddressBookFieldEditorPicture::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_RETURN:
		case VK_ESCAPE:
		case VK_TAB:
		case VK_UP:
		case VK_DOWN:
		case VK_NEXT:
		case VK_PRIOR:
		{
			// Pass it up to our parent.
			const MSG* pMsg = GetCurrentMessage();
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;
		}
		default:
		{
			INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

UINT CAddressBookFieldEditorPicture::OnGetDlgCode()
{
	UINT uRet = INHERITED::OnGetDlgCode() | DLGC_WANTTAB;
	if (Util::IsKeyMessage((LPMSG)(GetCurrentMessage()->lParam), VK_RETURN, VK_ESCAPE, 0))
	{
		uRet |= DLGC_WANTMESSAGE;
	}
	return uRet;
}

void CAddressBookFieldEditorPicture::OnSetFocus(CWnd* pOldWnd)
{
	INHERITED::OnSetFocus(pOldWnd);
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		pParent->PostMessage(WM_EDITOR_ACTIVATED, 0, (LPARAM)GetWrapper());
	}

	if ((pOldWnd != &m_NewButton)
	 && (pOldWnd != &m_DeleteButton)
	 && (pOldWnd != &m_CopyButton))
	{
		pOldWnd = &m_NewButton;
	}

	SendMessage(WM_EDITOR_ACTIVATED, 0, (LPARAM)pOldWnd);
}

void CAddressBookFieldEditorPicture::OnKillFocus(CWnd* pNewWnd)
{
	INHERITED::OnKillFocus(pNewWnd);

	if ((pNewWnd != &m_NewButton)
	 && (pNewWnd != &m_DeleteButton)
	 && (pNewWnd != &m_CopyButton))
	{
		CWnd* pParent = GetParent();
		if (pParent != NULL)
		{
			pParent->PostMessage(WM_EDITOR_DEACTIVATED, 0, (LPARAM)GetWrapper());
		}
	}
}

BOOL CAddressBookFieldEditorPicture::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// Don't change the cursor if the control is not focused.
	if (!IsEditing())
	{
		return TRUE;
	}
	return INHERITED::OnSetCursor(pWnd, nHitTest, message);
}

void CAddressBookFieldEditorPicture::OnMouseMove(UINT nFlags, CPoint point)
{
	// If we are not editing, let the parent handle this message.
	if (!IsEditing())
	{
		// Translate point to parent coordinates and pass the message up.
		ClientToScreen(&point);
		GetParent()->ScreenToClient(&point);
		GetParent()->SendMessage(WM_MOUSEMOVE, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
	}
	else
	{
		INHERITED::OnMouseMove(nFlags, point);
	}
}

void CAddressBookFieldEditorPicture::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (m_pBitmap != NULL)
	{
		CRect crImage;
		GetImageRectangle(crImage);
		if (!crImage.IsRectEmpty())
		{
			ExcludeButtonRectangles(dc);
			CPalette* pOldPal = dc.SelectPalette(pOurPal, FALSE);
			dc.RealizePalette();

			dc.SetStretchBltMode(STRETCH_DELETESCANS);
			StretchDIBits(
				dc.GetSafeHdc(),
				crImage.left,
				crImage.top,
				crImage.Width(),
				crImage.Height(),
				0,
				0,
				(int)(m_pBitmap->bmiHeader.biWidth),
				(int)(m_pBitmap->bmiHeader.biHeight),
				((LPBYTE)m_pBitmap)+sizeof(BITMAPINFO)+255*sizeof(RGBQUAD),
				m_pBitmap,
				DIB_RGB_COLORS,
				SRCCOPY);

			if (pOldPal != NULL)
			{
				dc.SelectPalette(pOldPal, FALSE);
			}
		}
	}
}

BOOL CAddressBookFieldEditorPicture::OnEraseBkgnd(CDC* pDC) 
{
	if (pDC->SaveDC() != 0)
	{
		CRect crImage;
		GetImageRectangle(crImage);
		if (!crImage.IsRectEmpty())
		{
			pDC->ExcludeClipRect(crImage);
		}
		ExcludeButtonRectangles(*pDC);

		CRect crBackground;
		GetClientRect(crBackground);
		Util::FillRectangleWithColor(*pDC, crBackground, GetSysColor(COLOR_WINDOW));

		pDC->RestoreDC(-1);
	}

	return TRUE;
}

LRESULT CAddressBookFieldEditorPicture::OnEditorActivated(WPARAM wParam, LPARAM lParam)
{
	((CWnd*)lParam)->SetFocus();
	return 0;
}

void CAddressBookFieldEditorPicture::OnNewPicture() 
{
	// Give the user a chance to choose a new picture.
	CString csName;
	TRY
	{
		if (ChoosePicture(csName, m_csName))
		{
			// The user picked a new name, try to create a thumbnail for it.
			BeginWaitCursor();
			LPBITMAPINFO pBitmap = MakeThumbnail(csName, m_crPicture.Width(), m_crPicture.Height());
			EndWaitCursor();

			if (pBitmap != NULL)
			{
				// We got a bitmap, switch to the new picture.
				DeleteBitmap();
				m_pBitmap = pBitmap;
				m_csName = csName;

				// Readraw ourselves.
				m_fModified = TRUE;
				Invalidate();
			}
			else
			{
				AfxMessageBox(IDS_CANT_USE_NEW_PICTURE);
			}
		}
	}
	END_TRY
}

void CAddressBookFieldEditorPicture::OnDeletePicture() 
{
	// The user wants to clear the current picture.
	if (HasBitmap())
	{
		// Delete the current bitmap.
		DeleteBitmap();

		// Redraw ourselves.
		m_fModified = TRUE;
		Invalidate();
	}
}

void CAddressBookFieldEditorPicture::OnCopyPicture() 
{
	// the user wants to copy the picture to the clipboard.
	if (HasBitmap())
	{
		if (CopyPictureToClipboard(m_csName, m_pBitmap))
		{
			AfxMessageBox(IDS_PICTURE_COPIED_TO_CLIPBOARD,  MB_OK | MB_ICONINFORMATION);
		}
		else
		{
			AfxMessageBox(IDS_CANT_COPY_PICTURE_TO_CLIPBOARD);
		}
	}
}
