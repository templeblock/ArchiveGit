// $Workfile: BrowDrop.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// BrowDrop.cpp: file for implementing Drag and Drop in the Browsers
//
//////////////////////////////////////////////////////////////////////////////
//
// Revision History:
//
// $Log: /PM8/App/BrowDrop.cpp $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 10    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 9     9/21/98 5:25p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 8     4/30/98 5:46p Hforman
// added default category when pasting/dropping
// 
// 7     4/13/98 1:22p Hforman
// modified params to ImportItem()
// 
// 6     2/10/98 5:45p Hforman
// modified friendly name creation for data importing
// 
// 5     2/06/98 11:35a Jayn
// Broke some functions out to use elsewhere.
// 
// 4     2/03/98 12:42p Hforman
// change type_of_graphic_extension() to type_of_art_extension
// 
// 3     1/29/98 8:12p Hforman
// add support for "pmo" files
// 
// 2     1/22/98 7:30p Hforman
// finished functionality
// 
// 1     1/15/98 3:55p Hforman
//
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <..\src\oleimpl2.h>
#include "pmw.h"
#include "memdev.h"
#include "grafrec.h"
#include "grafobj.h"
#include "wmf.h"
#include "bmp.h"
#include "util.h"
#include "clip.h"
#include "browser.h"
#include "browsupp.h"
#include "BrowDrop.h"

DROPEFFECT CArtOleDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
#ifdef _DEBUG
	pDataObject->BeginEnumFormats();
	FORMATETC fmt;
	while (pDataObject->GetNextFormat(&fmt))
	{
		if (fmt.cfFormat >= 0xc000)
		{
			char buffer[80];
			GetClipboardFormatName(fmt.cfFormat, buffer, sizeof(buffer));
			TRACE2("Format '%s' (%u)\n", buffer, fmt.cfFormat);
		}
		else
		{
			TRACE1("Format (%u)\n", fmt.cfFormat);
		}
	}
#endif // _DEBUG

	return OnDragOver(pWnd, pDataObject, dwKeyState, point);
}

DROPEFFECT CArtOleDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return GetDropEffect(pDataObject, dwKeyState);
}

void CArtOleDropTarget::OnDragLeave(CWnd* pWnd)
{
	COleDropTarget::OnDragLeave(pWnd);
}

BOOL CArtOleDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	CArtBrowserDialog* pBrowser = (CArtBrowserDialog*)pWnd;
	CPMWCollection* pCollection = pBrowser->GetCollectionManager()->GetUserCollection(CPMWCollection::typeArt);

	if (pDataObject->IsDataAvailable(Clipboard::m_uFilenameFormat))
	{
		CString filename = GetUniqueUserArtFilename("pmo");
		if (SaveClipObjectsToUserArt(pDataObject, filename))
			AddUntitledPictToBrowser(filename, pBrowser);
	}
	else if (pDataObject->IsDataAvailable(CF_METAFILEPICT))
	{
		CString filename = GetUniqueUserArtFilename("wmf");
		if (Save_CF_METAFILEPICT_ToUserArt(pDataObject, filename))
			AddUntitledPictToBrowser(filename, pBrowser);
	}
	else if (pDataObject->IsDataAvailable(CF_DIB))
	{
		CString filename = GetUniqueUserArtFilename("bmp");
		if (Save_CF_DIB_ToUserArt(pDataObject, filename))
			AddUntitledPictToBrowser(filename, pBrowser);
	}
	else if (pDataObject->IsDataAvailable(CF_BITMAP))
	{
		CString filename = GetUniqueUserArtFilename("bmp");
		if (Save_CF_BITMAP_ToUserArt(pDataObject, filename))
			AddUntitledPictToBrowser(filename, pBrowser);
	}
	else if (pDataObject->IsDataAvailable(CF_HDROP))
	{
		HDROP hDropInfo;
		if ((hDropInfo = (HDROP)pDataObject->GetGlobalData(CF_HDROP)) != NULL)
		{
			UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
			CBrowserProgressDialog ProgressDialog(IDD_BROWSER_IMPORT_PROGRESS, nFiles, pWnd, IDS_CONFIRM_CANCEL_BROWSER_IMPORT);

			BOOL fPromptForOverwrite = TRUE; // used for overwriting of local files

			// load the default art category
			CString strCategory;
			strCategory.LoadString(IDS_DEF_ART_CATEGORY);

			for (UINT iFile = 0; iFile < nFiles; iFile++)
			{
				TCHAR szFileName[_MAX_PATH];
				::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);
				if (type_of_art_extension(szFileName) != GRAPHIC_TYPE_UNKNOWN)
				{
					BOOL fDoImport = TRUE;

					ProgressDialog.SetPosition(iFile, szFileName);
					if (ProgressDialog.CheckForAbort())
						break;

					if (dropEffect == DROPEFFECT_COPY)
					{
						// copy file to user's Art directory
						CString csNewFileName;
						int overwriteRetVal = IDOK;
						if ((fDoImport = pBrowser->CopyFileToUserArtDir(szFileName, csNewFileName, fPromptForOverwrite, &overwriteRetVal)))
						{
							strcpy(szFileName, csNewFileName);
						}

						// see what needs to be done next
						if (overwriteRetVal == IDYESALL)
							fPromptForOverwrite = FALSE;	// no more prompting
						else if (overwriteRetVal == IDNO)
							fDoImport = FALSE;	// go on to next one
						else if (overwriteRetVal == IDCANCEL)
							break;		// we're out 'o here
					}

					if (fDoImport)
						pCollection->ImportItem(szFileName, NULL, NULL, strCategory);
				}
			} // end for loop

		
			ProgressDialog.SetPosition(iFile, "");
			pBrowser->UpdateAllBrowsers();
		}
	}

	return TRUE;		// We always return TRUE; else, the main frame takes over.
}

DROPEFFECT CArtOleDropTarget::GetDropEffect(COleDataObject* pDataObject, DWORD dwKeyState)
{
	DROPEFFECT de = DROPEFFECT_NONE;

	if (IsValidFormat(pDataObject))
	{
		if (!pDataObject->IsDataAvailable(CF_HDROP))
		{
			// when dragging data, we always create a new file
			de = DROPEFFECT_COPY;
		}
		else
		{
			// when dragging files, the default effect will be LINK if dragging from
			// a fixed drive, COPY if dragging from a removable drive. If either
			// Ctrl or Shift are down, the opposite takes effect.
			if (dwKeyState & (MK_CONTROL | MK_SHIFT))
				de = IsFixedDrive(pDataObject) ? DROPEFFECT_COPY : DROPEFFECT_LINK;
			else
				de = IsFixedDrive(pDataObject) ? DROPEFFECT_LINK : DROPEFFECT_COPY;
		}
	}

	return de;
}

BOOL CArtOleDropTarget::IsValidFormat(COleDataObject* pDataObject)
{
	pDataObject->BeginEnumFormats();
	BOOL fGotHDROP = FALSE;

	// don't allow drop from Art Gallery to Art Gallery!
	if (pDataObject->IsDataAvailable(CArtBrowserDialog::m_cfDataExchange))
		return FALSE;

	if (pDataObject->IsDataAvailable(Clipboard::m_uFilenameFormat) ||
		 pDataObject->IsDataAvailable(CF_METAFILEPICT) ||
		 pDataObject->IsDataAvailable(CF_DIB) ||
		 pDataObject->IsDataAvailable(CF_BITMAP))
	{
		return TRUE;
	}
	else if (pDataObject->IsDataAvailable(CF_HDROP))
	{
		// return TRUE if any file is a graphic file (based on extension)
		HDROP hDropInfo;
		if ((hDropInfo = (HDROP)pDataObject->GetGlobalData(CF_HDROP)) != NULL)
		{
			UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
			for (UINT iFile = 0; iFile < nFiles; iFile++)
			{
				TCHAR szFileName[_MAX_PATH];
				::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);
				if (type_of_art_extension(szFileName) != GRAPHIC_TYPE_UNKNOWN)
					return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CArtOleDropTarget::IsFixedDrive(COleDataObject* pDataObject)
{
	HDROP hDropInfo;
	if ((hDropInfo = (HDROP)pDataObject->GetGlobalData(CF_HDROP)) != NULL)
	{
		TCHAR szFileName[_MAX_PATH];
		::DragQueryFile(hDropInfo, 0, szFileName, _MAX_PATH);

		CString drive, dir, name, ext;
		Util::SplitPath(szFileName, &drive, &dir, &name, &ext);
		drive += '\\';
		UINT type = GetDriveType(drive);
		if (type == DRIVE_FIXED)
			return TRUE;
		else
			return FALSE;
	}	

	return FALSE;
}

void CArtOleDropTarget::AddUntitledPictToBrowser(const CString& filename, CArtBrowserDialog* pBrowser)
{
	// Add dropped or pasted art data file to browser...

	// Friendly name will be "Untitled n" - n being the number on the end of filename.
	// The filenames should be of the form "Artnnnnn.???", starting with "Art00001"

	CString friendlyName;
	friendlyName.LoadString(IDS_UNTITLED);

	CString strNum = filename.Mid(filename.ReverseFind('.') - 5, 5);
	strNum.Format(" %d", atoi(strNum));
	friendlyName += strNum;

	// load the default art category
	CString strCategory;
	strCategory.LoadString(IDS_DEF_ART_CATEGORY);

	CPMWCollection* pCollection = pBrowser->GetCollectionManager()->GetUserCollection(CPMWCollection::typeArt);
	if (pCollection->ImportItem(filename, friendlyName, NULL, strCategory) == ERRORCODE_None)
		pBrowser->UpdateAllBrowsers();
}

BOOL CArtOleDropTarget::SaveClipObjectsToUserArt(COleDataObject* pDataObject, LPCTSTR pFilename)
{
	CPmwDoc* pTempDoc = GET_PMWAPP()->NewHiddenDocument();
	if (pTempDoc == NULL)
	{
		ASSERT(pTempDoc);
		return FALSE;
	}

	pTempDoc->size_to_paper();

	BOOL retval = TRUE;
	if ((clipboard.PasteObjectsFromDatabase(pTempDoc) != ERRORCODE_None) ||
		 (pTempDoc->SaveToFile(pFilename, TRUE) != TRUE))
	{
		retval = FALSE;
	}

	pTempDoc->OnCloseDocument();
	return retval;
}

BOOL CArtOleDropTarget::Save_CF_METAFILEPICT_ToUserArt(COleDataObject* pDataObject, LPCTSTR pFilename)
{
	HGLOBAL hData;
	METAFILEPICT* pPict;
	if ((hData = pDataObject->GetGlobalData(CF_METAFILEPICT)) == NULL ||
		 (pPict = (METAFILEPICT*)::GlobalLock(hData)) == NULL)
		return FALSE;
	
	BOOL fRet = (CreateWMFFromMetaFilePict(pPict, pFilename) == ERRORCODE_None);

	::GlobalUnlock(hData);

	return fRet;
}

BOOL CArtOleDropTarget::Save_CF_DIB_ToUserArt(COleDataObject* pDataObject, LPCTSTR pFilename)
{
	HGLOBAL hData;
	BITMAPINFO* pBitmap;
	if ((hData = pDataObject->GetGlobalData(CF_DIB)) == NULL ||
		 (pBitmap = (BITMAPINFO*)::GlobalLock(hData)) == NULL)
		return FALSE;

	BOOL retval = SaveDIBToUserArt(pBitmap, pFilename);
	::GlobalUnlock(hData);
	::GlobalFree(hData);

	return retval;
}

BOOL CArtOleDropTarget::Save_CF_BITMAP_ToUserArt(COleDataObject* pDataObject, LPCTSTR pFilename)
{
	// setup OLE structs for data acquisition
	STGMEDIUM stgmed;
	stgmed.tymed = TYMED_GDI;
	stgmed.pUnkForRelease = NULL;

	FORMATETC formatEtc;
	formatEtc.ptd = NULL;
	formatEtc.dwAspect = DVASPECT_CONTENT;
	formatEtc.lindex = -1;
	formatEtc.tymed = TYMED_GDI;
	formatEtc.cfFormat = CF_BITMAP;

	HBITMAP hBitmap = NULL;
	if (pDataObject->GetData(CF_BITMAP, &stgmed, &formatEtc))
		hBitmap = stgmed.hBitmap;
	else
		return FALSE;

	HPALETTE hPalette = NULL;
	if (pDataObject->IsDataAvailable(CF_PALETTE))
	{
		formatEtc.cfFormat = CF_PALETTE;
		if (pDataObject->GetData(CF_PALETTE, &stgmed, &formatEtc))
			hPalette = (HPALETTE)stgmed.hBitmap;
	}

	BOOL retval = TRUE;

	HGLOBAL hDIB;
	BITMAPINFO* pBitmap = NULL;
	if (::BitmapToDIB(hBitmap, hPalette, &hDIB) == ERRORCODE_None &&
		 (pBitmap = (BITMAPINFO*)::GlobalLock(hDIB)) != NULL)
	{
		retval = SaveDIBToUserArt(pBitmap, pFilename);
	}
	else
		retval = FALSE;

	// clean up our act
	::GlobalUnlock(hDIB);
	::GlobalFree(hDIB);

	::DeleteObject(hBitmap);
	if (hPalette)
		::DeleteObject(hPalette);

	return retval;
}

BOOL CArtOleDropTarget::SaveDIBToUserArt(BITMAPINFO* pBitmap, LPCTSTR pFilename)
{
// TODO: create general function in bmp.cpp to do the DIB stuff?

	// Compute the size of the BMP data.
	DWORD nByteWidth = (pBitmap->bmiHeader.biWidth*pBitmap->bmiHeader.biBitCount+7)/8;
	nByteWidth = (nByteWidth + 3) & ~3;

	DWORD dwSizeImage = (DWORD)nByteWidth * (DWORD)pBitmap->bmiHeader.biHeight;
	int nColors;
	if (pBitmap->bmiHeader.biClrUsed != 0)
		nColors = pBitmap->bmiHeader.biClrUsed;
	else if (pBitmap->bmiHeader.biBitCount > 8)
		nColors = 0;
	else
		nColors = (1 << pBitmap->bmiHeader.biBitCount);

	DWORD dwSizeColors = nColors * sizeof(pBitmap->bmiColors[0]);
	DWORD dwDataSize = pBitmap->bmiHeader.biSize + dwSizeColors + dwSizeImage;

	// Build the header.
	BITMAPFILEHEADER Header;
	Header.bfType = (SHORT)'B' + (((SHORT)'M') << 8);
	Header.bfSize = sizeof(Header) + dwDataSize;
	Header.bfReserved1 = 0;
	Header.bfReserved2 = 0;
	Header.bfOffBits = sizeof(Header) + pBitmap->bmiHeader.biSize + dwSizeColors;

	// Write the bitmap file to memory, then save to disk
	MemoryDevice DataDevice;
	DataDevice.AddPiece(&Header, sizeof(Header));
	DataDevice.AddPiece(pBitmap, dwDataSize);

	// save BMP to file in user art directory
	StorageFile Dest(pFilename);
	if (copy_file(&DataDevice, &Dest) != ERRORCODE_None)
	{
		Dest.flush();
		Dest.zap();
		return FALSE;
	}

	return TRUE;
}

CString CArtOleDropTarget::GetUniqueUserArtFilename(CString ext)
{
	// Create a filename for new art in the user's art directory.
	// The default name is "Art00001.<ext>"
	// If this file exists, bump the number by one, etc.

	CString firstpart = GetGlobalPathManager()->ExpandPath("[[U]]\\ART\\Art");
	CString filename = firstpart + "00001.";
	filename += '*';

	int num = 0;
	CFileIterator fileIter;
	while (fileIter.FindFirst(filename, 0))
	{
		fileIter.Close();
		filename.Format("%s%05d.*", firstpart, ++num);
	}

	filename = filename.Left(filename.GetLength()-1) + ext;
	return filename;
}
