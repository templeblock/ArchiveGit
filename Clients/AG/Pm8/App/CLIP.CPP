// $Header: /PM8/App/CLIP.CPP 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
// clip.cpp: Implementation for clipboard objects.
//
// $Log: /PM8/App/CLIP.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 19    2/11/99 5:28p Rgrenfel
// Performed an UpdateContents before checking for supported clipboard
// formats.
// 
// 18    1/25/99 3:38p Rgrenfell
// Added support for object descriptors in Drag/Drop
// 
// 17    1/07/99 5:09p Rgrenfel
// Added new clip render methods.
// 
// 16    12/10/98 10:59a Lwilson
// Removed byte alignment pragmas from framework dependencies, as they are
// now implemented in the individual framework includes.  Also, removed
// FrameworkIncludes.h as it is now in the precompiled header.
// 
// 15    12/08/98 5:18p Lwilson
// Initial Print Shop integration.
// 
// 14    11/10/98 2:07p Gbeddow
// oleimpl.h does not exist in VC6
// 
// 13    10/28/98 11:14a Mwilson
// fixed paste bug
// 
// 12    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 11    9/14/98 12:06p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 10    8/10/98 3:46p Hforman
// getting "crop copyright" flag from clipboard data
// 
// 9     7/23/98 6:45p Jayn
// SavePMObjectsTo... can skip special objects (like calendars)
// 
// 8     7/14/98 6:34p Jayn
// 
// 7     6/17/98 7:18p Jayn
// Now calls ModifyParagraph instead of GetParagraph().
// Fixes a corruption bug.
// 
// 6     3/20/98 6:03p Jayn
// Serif Drawing Object
// 
// 5     2/06/98 11:35a Jayn
// Broke some functions out to use elsewhere.
// 
// 4     1/29/98 8:02p Hforman
// add Selected param to CopyObjectsToDatabase()
// 
// 3     1/27/98 6:42p Hforman
// cleanup, made some methods global, changed the way H_BITMAP data is
// read, etc.
// 
// 2     12/08/97 2:52p Jayn
// Always uses 24-bit mode for metafile generation.
// 
//    Rev 1.0   14 Aug 1997 15:19:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:36   Fred
// Initial revision.
// 
//    Rev 1.28   07 Aug 1997 09:14:14   Jay
// Sets the embed_source variable for the GCS.
// 
//    Rev 1.27   16 Jun 1997 14:28:26   Jay
// Uses database of document during copy if exists (fixes crash).
// 
//    Rev 1.26   05 Jun 1997 16:45:36   Jay
// Changed upto_object in DrawObjects
// 
//    Rev 1.25   12 May 1997 18:08:28   Jay
// Browser drag & drop
// 
//    Rev 1.24   16 Apr 1997 12:12:18   Jay
// Fixes for text undo
// 
//    Rev 1.23   09 Apr 1997 17:13:46   Jay
// Renamed internal clipboard format.
// 
//    Rev 1.22   23 Dec 1996 17:00:56   Jay
// Better RTF paste and text paste.
// 
//    Rev 1.21   18 Dec 1996 09:11:00   Jay
// More RTF parsing
// 
//    Rev 1.20   13 Dec 1996 13:13:22   JAY
// RTF Parser.
// 
//    Rev 1.19   05 Nov 1996 17:31:14   Jay
// New network stuff
// 
//    Rev 1.18   18 Oct 1996 10:49:42   Jay
// Now sets the update time duirng a drop.
// 
//    Rev 1.17   15 Oct 1996 09:02:02   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.16   31 Jul 1996 11:57:58   Fred
// 16-bit compatibility
// 
//    Rev 1.15   23 Jul 1996 13:15:34   Jay
// Renamed clipboard format since it's incompatible with earlier versions.
// 
//    Rev 1.14   22 Jul 1996 18:08:38   Jay
// Fixed bug with incorrectly set "desired x" after paste.
// 
//    Rev 1.13   16 Jul 1996 11:26:04   Jay
// Linked frames with cut/copy/paste/duplicate
// 
//    Rev 1.12   12 Jul 1996 16:49:30   Jay
// Fixed a bug
// 
//    Rev 1.11   11 Jul 1996 11:35:44   Jay
//  
// 
//    Rev 1.10   05 Jul 1996 10:47:40   Jay
//  
// 
//    Rev 1.9   03 Jul 1996 14:31:12   Jay
//  
// 
//    Rev 1.8   02 Jul 1996 11:25:14   Jay
//  
// 
//    Rev 1.7   02 Jul 1996 08:55:30   Jay
//  
// 
//    Rev 1.6   28 Jun 1996 17:32:18   Jay
// OnPage() routine.
// 
//    Rev 1.5   27 Jun 1996 13:26:06   Jay
//  
// 
//    Rev 1.4   26 Jun 1996 09:07:30   Jay
//  
// 
//    Rev 1.3   05 Jun 1996 11:35:26   Jay
// No longer translates \n and \t characters.
// 
//    Rev 1.2   05 Jun 1996 08:21:44   Jay
//  
// 
//    Rev 1.1   07 May 1996 12:43:30   Jay
// From PMW2
// 
//    Rev 2.23   26 Apr 1996 14:23:46   JAY
// New 'scratch' template for internal use
// 
//    Rev 2.22   15 Jan 1996 09:16:40   JAY
// Changes to fix changes made for 32-bit version.
// 
//    Rev 2.21   26 Dec 1995 10:29:14   FRED
// Project structure changes for mac port
// 
//    Rev 2.20   20 Dec 1995 09:56:38   JAY
// 32-bit fixes
// 
//    Rev 2.19   15 Nov 1995 15:23:08   JAY
// OLE fixes, etc.
// 
//    Rev 2.18   18 Oct 1995 15:59:52   FRED
// Fixes for 16 bit version after 32 bit changes
// 
//    Rev 2.17   29 Sep 1995 16:00:10   JAY
// More drag/drop/paste/OLE stuff
// 
//    Rev 2.16   28 Sep 1995 16:49:40   JAY
// Paste Special
// 
//    Rev 2.15   28 Sep 1995 14:53:28   JAY
// Multi-file drops now work.
// 
//    Rev 2.14   28 Sep 1995 12:13:12   JAY
// More drag/drop stuff
// 
//    Rev 2.13   27 Sep 1995 15:26:28   JAY
// New OLE code
// 
//    Rev 2.12   19 Sep 1995 15:45:10   JAY
// 32-bit changes.
// 
//    Rev 2.11   14 Aug 1995 17:17:58   JAY
// Extensions for cut and paste from warp text dialog.
// 
//    Rev 2.10   05 Jul 1995 11:16:44   JAY
// Rearrange includes to reduce symbol count.
// 
//    Rev 2.9   26 Jun 1995 17:02:12   JAY
//  
// 
//    Rev 2.8   05 Jun 1995 11:35:08   JAY
//  
// 
//    Rev 2.7   01 May 1995 15:27:50   JAY
// Restructured the macro stuff
// 
//    Rev 2.6   15 Mar 1995 11:13:14   JAY
// Got rid of some debugging messages.
// 
//    Rev 2.5   15 Mar 1995 10:48:02   JAY
// Support for new text editing
// 
//    Rev 2.4   09 Mar 1995 08:28:56   JAY
// More undo.
// 
//    Rev 2.3   17 Feb 1995 16:48:12   JAY
// Text editing undo and redo.
// 
//    Rev 2.2   15 Feb 1995 17:15:46   JAY
// Undo
// 
//    Rev 2.1   08 Feb 1995 13:35:28   JAY
// Reverted. New series.
// 
//    Rev 1.13   07 Feb 1995 08:39:24   JAY
// Added PrePaste() to allow control of conditions before pasting objects.
// 
//    Rev 1.12   30 Jan 1995 09:32:22   JAY
// Cut and paste
// 
//    Rev 1.11   09 Jan 1995 16:59:26   JAY
// Cut & paste, etc.
// 
//    Rev 1.10   30 Nov 1994 17:02:46   JAY
//  
// 
//    Rev 1.9   04 Aug 1994 07:35:06   JAY
// Large model and 32-bit changes
// 
//    Rev 1.8   18 Jul 1994 10:05:10   JAY
// Fixed paste bug - EOP word was not being sized.
// Now uses access_file().
// 
//    Rev 1.7   26 May 1994 13:49:52   JAY
// Now uses remove_slash().
// 
//    Rev 1.6   08 Apr 1994 11:06:52   JAY
// Now reports the appropriate "Full" message when the paragraph will not receive
// any more characters.
// 
//    Rev 1.5   16 Feb 1994 14:01:46   JAY
// Now uses StorageFile::base_path instead of "." for tempnam() dir parameter.
// 
//    Rev 1.4   19 Jan 1994 17:09:18   JAY
//  
// 
//    Rev 1.3   13 Jan 1994 17:03:28   JAY
//  
// 
//    Rev 1.2   05 Jan 1994 17:15:14   JAY
// Cut and paste!
// 
//    Rev 1.1   05 Jan 1994 08:11:02   JAY
// Restructuring, cut/paste, images, etc.
//
//   Rev 1.0   16 Dec 1993 16:28:20   JAY
// 
*/

#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"
#include "pmwview.h"
#include "pmwmdi.h"

#include "clip.h"

#include "frameobj.h"
#include "framerec.h"
#include "textrec.h"
#include "prevrec.h"
#include "textflow.h"
#include "utils.h"
#include "grafobj.h"
#include "macrorec.h"
#include "macrostd.h"

#include "wmf.h"
#include "bmp.h"

#include "prvwdlg.h"
#include "tcommand.h"
#include "pagerec.h"
#include "grpobj.h"
#include "compobj.h"

#include "rtfparse.h"
#include "txpdest.h"
#include "browser.h"

#include <io.h>
#if _MSC_VER < 1200 // GCB 11/10/98 - oleimpl.h does not exist in VC6
	#include <..\src\oleimpl.h>
#endif
#include <..\src\oleimpl2.h>
#include "cntritem.h"
#include "oleobj.h"

// Framework support
#include "DataTransfer.h"

// Declare our global clipboard
Clipboard clipboard;

// initialize static data
UINT Clipboard::m_uFilenameFormat = ::RegisterClipboardFormat("PMG clipboard file 5.0");
UINT Clipboard::m_uRichTextFormat = ::RegisterClipboardFormat("Rich Text Format");
char Clipboard::m_cbClipboardTemplate[5+1] = "~xxxC";
BOOL Clipboard::m_fRenderOnExit = TRUE;

PRIVATE char clipboard_preamble[] = "PrintMaster Gold Clipboard\r\n\x1a";

extern BYTE far screen_gamma_curve[];
extern CPalette *pOurPal;
extern CPalette* CreateIdentityPalette(void);

UINT Clipboard::m_cfEmbeddedObject = ::RegisterClipboardFormat(_T("Embedded Object"));
UINT Clipboard::m_cfEmbedSource = ::RegisterClipboardFormat(_T("Embed Source"));
UINT Clipboard::m_cfLinkSource= ::RegisterClipboardFormat(_T("Link Source"));
UINT Clipboard::m_cfFileName= ::RegisterClipboardFormat(_T("FileName"));
UINT Clipboard::m_cfObjectDescriptor = ::RegisterClipboardFormat(_T("Object Descriptor"));

/*
// INI file strings.
*/

static char BASED_CODE SECTION_Clipboard[] = "Clipboard";
static char BASED_CODE ENTRY_Format1[] = "Format1";
static char BASED_CODE ENTRY_Format2[] = "Format2";
static char BASED_CODE ENTRY_Format3[] = "Format3";

/////////////////////////////////////////////////////////////////////////////
// CClipboardOpenState

CClipboardOpenState::CClipboardOpenState()
{
	m_pDoc = NULL;
	m_pPage = NULL;
}

CClipboardOpenState::~CClipboardOpenState()
{
	if (m_pPage != NULL)
	{
		m_pPage->release();
	}
	if (m_pDoc != NULL)
	{
		m_pDoc->OnCloseDocument();
	}
}

PMGDatabase* CClipboardOpenState::GetDatabase(void)
{
	return (m_pDoc == NULL) ? m_Mapping.database : m_pDoc->get_database();
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

Clipboard::Clipboard()
		: m_Formats(sizeof(UINT))
{
	m_pszClipboardName = NULL;
	m_Type = CLIPBOARD_TYPE_unknown;
	m_dwUpdateTime = 0L;
	m_fRenderedMetafile = TRUE;
	m_fRenderedBitmap = TRUE;
	m_fRenderedPalette = TRUE;
}

Clipboard::~Clipboard()
{
	DestroyContents();
}

void Clipboard::DestroyContents(VOID)
{
//	od("Destroy clipboard...\r\n");
	if (m_pszClipboardName != NULL)
	{
		::unlink(m_pszClipboardName);
		::free(m_pszClipboardName);
		m_pszClipboardName = NULL;
	}

/* We don't know after this. */

	m_Type = CLIPBOARD_TYPE_unknown;
}

VOID Clipboard::UpdateContents(HWND hWnd)
{
	od("Update clipboard...\r\n");

#if 0
	COleDataObject Data;

	if (Data.AttachClipboard())
	{
		SetUpdateTime();

		m_Formats.empty_array();

		FORMATETC fmt;

		Data.BeginEnumFormats();
		while (Data.GetNextFormat(&fmt))
		{
			UINT uFormat = fmt.cfFormat;
			m_Formats.add_element(&uFormat);

			if (uFormat >= 0xc000)
			{
				char Buffer[80];
				GetClipboardFormatName(uFormat, Buffer, sizeof(Buffer));

				od("Format '%s' (%u) is available\n", Buffer, uFormat);
			}
			else
			{
				od("Format (%u) is available\r\n", uFormat);
			}
		}
	}
#else
	if (::OpenClipboard(hWnd))
	{
		SetUpdateTime();
	/* See what's in the clipboard. */

		m_Formats.empty_array();

		for (UINT uFormat = 0;
					(uFormat = ::EnumClipboardFormats(uFormat)) != 0; )
		{
			m_Formats.add_element(&uFormat);

			if (uFormat >= 0xc000)
			{
				char Buffer[80];
				GetClipboardFormatName(uFormat, Buffer, sizeof(Buffer));

				od("Format '%s' (%u) is available\n", Buffer, uFormat);
			}
			else
			{
				od("Format (%u) is available\r\n", uFormat);
			}
		}

		::CloseClipboard();
	}
#endif
	od("[Done with update clipboard...]\r\n");
}

BOOL Clipboard::FormatIsAvailable(UINT uFormat)
{
// A simple up-front check.

	if (uFormat == 0)
	{
		return FALSE;
	}

// Search for it.

	UINT far *fp = (UINT far *)m_Formats.get_element(0);

	for (SHORT i = 0; i < m_Formats.count(); i++)
	{
		if (*fp++ == uFormat)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Clipboard::CanPasteText()
{
	UpdateContents(NULL);
	return FormatIsAvailable(CF_TEXT)
				|| FormatIsAvailable(CF_OEMTEXT)
				|| FormatIsAvailable(m_uRichTextFormat)
				|| TypeOfMlsClipboard(FALSE) == CLIPBOARD_TYPE_text;
}

BOOL Clipboard::CanPasteObjects()
{
#ifdef NO_PASTE_OBJECTS
	return FALSE;
#else
	UpdateContents(NULL);
	return FormatIsAvailable(CF_METAFILEPICT)
				|| FormatIsAvailable(CF_BITMAP)
				|| FormatIsAvailable(CF_DIB)
				|| FormatIsAvailable(m_cfEmbeddedObject)
				|| FormatIsAvailable(m_cfEmbedSource)
				|| FormatIsAvailable(m_cfLinkSource)
				|| FormatIsAvailable(m_cfFileName)
				|| FormatIsAvailable(CF_HDROP)
				|| FormatIsAvailable(kComponentFormat)
				|| TypeOfMlsClipboard(FALSE) == CLIPBOARD_TYPE_objects;
#endif
}

/*
// Create the clipboard database.
*/

ERRORCODE Clipboard::PrepareClipboardForCopy(CClipboardOpenState* pState, BOOL fNeedDocument /*=FALSE*/)
{
//	od("Open clipboard...\r\n");

/* Open the clipboard. */

	if (!OpenClipboard())
	{
		return ERRORCODE_Busy;
	}

//	od("Empty clipboard...\r\n");
	if (!EmptyClipboard())
	{
//		od("Unable to empty clipboard!\r\n");
		CloseClipboard();
		return ERRORCODE_Busy;
	}

	if (m_pszClipboardName != NULL)
	{
//		od("Clipboard name has not been freed!\r\n");
	/*
 	// Just do it now and live with it.
	// But this is not supposed to happen...
	*/
		DestroyContents();
	}

//	od("Create the temp name...\r\n");

	char buffer[_MAX_PATH];

	strcpy(buffer, GetGlobalPathManager()->GetWorkingDirectory());
	remove_slash(buffer);

	if ((m_pszClipboardName = ::_tempnam(buffer, m_cbClipboardTemplate)) == NULL)
	{
		CloseClipboard();
		return ERRORCODE_Full;
	}

/*
// Setup the database.
*/

	ERRORCODE error = ERRORCODE_None;

	if (fNeedDocument)
	{
	/* We need to create a document with this name. */
		WORD fGlobalSave = CPmwDoc::m_wGlobalDocumentFlags;
		CPmwDoc::m_wGlobalDocumentFlags = CPmwDoc::FLAG_ClipDocument;
		CPmwDoc* pDoc = (CPmwDoc*)GET_PMWAPP()->m_pScratchTemplate->OpenDocumentFile(NULL, FALSE);
		CPmwDoc::m_wGlobalDocumentFlags = fGlobalSave;

		if (pDoc != NULL)
		{
#ifndef WIN32
			if (!pDoc->DoNewDocument())
			{
				pDoc->OnCloseDocument();
				pDoc = NULL;
				error = ERRORCODE_Open;
			}
#endif
		}
		else
		{
			error = ERRORCODE_Open;
		}
		pState->m_pDoc = pDoc;
	}
	else
	{
		if ((error = pState->m_Mapping.map_to_file(m_pszClipboardName, FALSE)) != ERRORCODE_None
				|| (error = pState->m_Mapping.initialize(clipboard_preamble,
														SIGNATURE,
														FILE_TYPE,
														0)) != ERRORCODE_None)
		{
			CloseClipboard();
			DestroyContents();
			return error;
		}

	/* Create the mandatory records. */

		if ((error = pState->m_Mapping.CreateMandatoryRecords()) != ERRORCODE_None)
		{
			pState->m_Mapping.flush_database();

			CloseClipboard();
			DestroyContents();
			return error;
		}
	}

	return error;
}

/*
// Create the standard clipboard format.
// If this doesn't succeed, we just ignore it.
*/

VOID Clipboard::CreateStandardClipboard(CLIPBOARD_TYPE type, DB_RECORD_NUMBER record, PBOX* pWorld)
{
	if (m_uFilenameFormat != 0)
	{
	/* We were able to register. */

		HGLOBAL hName;

		if ((hName = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
					sizeof(CLIPBOARD_FILENAME))) != NULL)
		{
			CLIPBOARD_FILENAME_PTR data;

			if ((data = (CLIPBOARD_FILENAME_PTR)::GlobalLock(hName)) == NULL)
			{
				::GlobalFree(hName);
			}
			else
			{
				data->m_Type = type;
				data->m_uFlags = 0;
				data->m_dwRecord = record;
				data->m_World = *pWorld;
				strcpy(data->m_cbName, m_pszClipboardName);
				::GlobalUnlock(hName);

			/* Set the data. */

				::SetClipboardData(m_uFilenameFormat, hName);
			}
		}
	}
}

/*
// Copy text from one text record to another.
*/

void CopyTextToText(CTextRecord* pDestText, CHARACTER_INDEX lDestination, CTextRecord* pSourceText, CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd, UNDO_STATE nUndo /*=UNDO_None*/, DB_RECORD_NUMBER lFrame /*=0*/)
{
	PMGDatabase* pDatabase = (PMGDatabase*)pSourceText->GetDatabase();
	PMGDatabase* pDestDatabase = (PMGDatabase*)pDestText->GetDatabase();
	CTextIterator SourceIterator(NULL);
	CTextIterator DestIterator(NULL);

	TRY
	{
		// Setup the iterators.
		SourceIterator.SetRecord(pSourceText);
		DestIterator.SetRecord(pDestText);

		// Compute the starting position.
		SourceIterator.SetPosition(lStart);
		DestIterator.SetPosition(lDestination);

		// Variables to help update the frame text states.
		CTextState StartState = DestIterator.State();
		CHARACTER_COUNT lStartCharacters = pDestText->NumberOfCharacters();
		PARAGRAPH_COUNT lStartParagraphs = pDestText->NumberOfParagraphs();

		// We are going to loop on paragraphs.
		// This will prevent a lot of redundant work on the destination side.
		// (For example, if we used the InsertText() function to stuff the text
		// in all at once, a bunch of paragraphs with default style would be
		// created - style that we would then be changing immediately to the
		// source paragraph style. By creating the paragraphs by hand, we avoid
		// that extra work.)

		BOOL fBroke = FALSE;

		while (SourceIterator.CharacterIndex() < lEnd)
		{
			// Copy the next paragraph.
			// There are four cases generated by clipping or not clipping each
			// of the front or end of the source paragraph:
			// (1) Start is not start of paragraph. End is end. (Front clipped).
			// (2) Start is start. End is end. (No clipping).
			// (3) Start is start. End is not end of paragraph. (End clipped).
			// (4) Start and end are both in middle paragraph (both clipped).
			// The special case is (2) where we can copy the entire paragraph
			// without parsing it. This will hopefully be fast.

			CTextParagraph* pSourceParagraph = pSourceText->GetParagraph(SourceIterator.ParagraphIndex());
			CTextParagraph* pDestParagraph = pDestText->ModifyParagraph(DestIterator.ParagraphIndex());

			// Compute how many characters to copy.
			// Clip to end of selection.
			CHARACTER_INDEX lParagraphEnd = SourceIterator.CharacterIndex() - SourceIterator.CharacterInParagraph() + pSourceParagraph->m_lCharacters;
			CHARACTER_COUNT lCharacters = ((lParagraphEnd < lEnd) ? lParagraphEnd : lEnd) - SourceIterator.CharacterIndex();

			// Determine the insertion context. We need to determine:
			// (1) What paragraph (possibly new) to insert into.
			// (2) Whether the paragraph style should be applied to the paragraph.

			// Determine if we need to add a paragraph.
			// We need to add one if the source paragraph ends with a carriage
			// return.

			BOOL fCopyIt = FALSE;
			fBroke = FALSE;
			if (*(pSourceText->GetCharacter(SourceIterator.CharacterIndex() + lCharacters-1)) == '\n')
			{
				// We need to add a paragraph. There are two possibilities:
				// (1) adding at the front of the destination paragraph
				// (2) adding past the front of the destination paragraph
				// We could simply split the paragraph in all cases, but splitting
				// does more work than we need.

				if (DestIterator.CharacterInParagraph() == 0)
				{
					// Adding at the front. We will put our new paragraph in front
					// of the current one.
					CTextParagraph Paragraph;

					Paragraph.m_lBaseParagraphStyle = pDestDatabase->ImportTextStyleRecord(DestIterator.Style().ParagraphStyle(), DestIterator.Database());
					Paragraph.m_StyleChange.m_lValue = Paragraph.m_lBaseParagraphStyle;

					pDestText->InsertParagraph(DestIterator.ParagraphIndex(), &Paragraph, 1);
					// Get rid of the extra ref we have on the style.
					// InsertParagraph() incremented it for us.
					pDestDatabase->FreeTextStyleRecord(Paragraph.m_lBaseParagraphStyle);
				}
				else
				{
					// Not adding at the front. Split the current paragraph.
					// Our text will go at the end.
					pDestText->SplitParagraph(DestIterator.ParagraphIndex(), DestIterator.CharacterInParagraph());
				}
				// Reload the destination paragraph.
				pDestParagraph = pDestText->ModifyParagraph(DestIterator.ParagraphIndex());

				// Note that the iterator is still correct when we are done, with
				// the exception of the current style change which must be
				// reloaded.
//				DestIterator.ReloadCurrentChange();
				DestIterator.UpdateStyle(TRUE);
				fBroke = TRUE;
			}

			// Now, get the confirmed destination paragraph, and determine if
			// we will be exclusively occupying it (or otherewise need to set the paragraph style).

			BOOL fEmptyDestination = pDestParagraph->m_lCharacters == 0;

			// If we are doing the deleting undo setup, then we want the
			// paragraph style to remain as is even if inserting into an
			// empty paragraph.
			if (nUndo == UNDO_Deleting && DestIterator.CharacterIndex() > 0)
			{
				fEmptyDestination = FALSE;
			}

			if (fEmptyDestination
					|| (DestIterator.CharacterInParagraph() == 0 
							&& SourceIterator.CharacterIndex() != lStart))
			{
				// If we are copying all characters, then we can do a straight
				// copy.
				fCopyIt = fEmptyDestination && (lCharacters == pSourceParagraph->m_lCharacters);

				// The destination paragraph has no text. This means that the
				// paragraph being pasted will take over the destination paragraph
				// as far as paragraph formatting goes.
				//
				// We convert over the style change of the source paragraph and
				// copy all the paragraph formatting changes.

				pDestText->CopyBaseStyle(pDestParagraph, pSourceParagraph, pDatabase);
				pDestText->CopyParagraphFormatting(pDestParagraph, pSourceParagraph, pSourceText, fCopyIt);

				if (!fCopyIt)
				{
					// Recompute the style information since it just changed.
					CHARACTER_INDEX lIndex = DestIterator.CharacterIndex();
					DestIterator.ResetToParagraphStyle();
					DestIterator.SetPosition(lIndex);
				}
			}

			// Copy the text over to the destination.

			CHARACTER_INDEX lSourceCharacter = SourceIterator.CharacterIndex();
			CHARACTER_INDEX lDestCharacter = DestIterator.CharacterIndex();
			for (CHARACTER_COUNT lCopyCharacters = lCharacters;
					lCopyCharacters > 0; )
			{
				CHARACTER_COUNT lThisCharacters;
				CHARACTER* cp = pSourceText->GetCharacters(lSourceCharacter, &lThisCharacters);
				if (lThisCharacters > lCopyCharacters)
				{
					lThisCharacters = lCopyCharacters;
				}
				// Insert the text characters.
				pDestText->InsertCharacter(lDestCharacter, cp, lThisCharacters);
				lSourceCharacter += lThisCharacters;
				lDestCharacter += lThisCharacters;
				lCopyCharacters -= lThisCharacters;
			}

			// Add the characters into the character count.
			pDestParagraph->m_lCharacters += lCharacters;

			CHARACTER_INDEX lNextSource = SourceIterator.CharacterIndex() + lCharacters;
			CHARACTER_INDEX lNextDest = DestIterator.CharacterIndex() + lCharacters;

			// Copy the style changes over.
			// If we are in a new paragraph all by ourselves, then we can
			// bring over our style changes whole.

			if (fCopyIt)
			{
				// Copy the style changes from the source paragraph to here.
				pDestParagraph->m_StyleChange.m_lCharacters = pSourceParagraph->m_StyleChange.m_lCharacters;
				pDestText->CopyCharacterFormatting(pDestParagraph, pSourceParagraph, pSourceText);
			}
			else
			{
				// Copy the style changes using the iterators.
				if (DestIterator.CurrentChange() == NULL)
				{
					DestIterator.BackupAChange();
				}
				ASSERT(DestIterator.CurrentChange() != NULL);
				DestIterator.CurrentChange()->m_lCharacters += lCharacters;

				CHARACTER_COUNT lAdjust = DestIterator.CharacterIndex() - SourceIterator.CharacterIndex();
				CHARACTER_INDEX lStyleStart = SourceIterator.CharacterIndex();

				while (lStyleStart < lEnd && lStyleStart != -1)
				{
					while (SourceIterator.NextChangeStart() >= 0 && SourceIterator.NextChangeStart() <= lStyleStart)
					{
						// Advance to that change.
						SourceIterator.AdvanceCurrentChange();
						// And grab it.
						SourceIterator.GrabCurrentChange();
					}
					CHARACTER_INDEX lStyleEnd = SourceIterator.NextChangeStart();
					if (lStyleEnd > lEnd)
					{
						lStyleEnd = lEnd;
					}
					if (lStyleStart < lStyleEnd)
					{
						pDestText->ChangeRangeStyle(lStyleStart + lAdjust,
															 lStyleEnd + lAdjust,
															 &SourceIterator.Style());
					}
					lStyleStart = lStyleEnd;
				}
			}

			// Advance to the next paragraph.
			SourceIterator.SetPosition(lNextSource);
			DestIterator.SetPosition(lNextDest);
		}

#if 1
		if (fBroke && nUndo != UNDO_None)
		{
			// Last character inserted was a carriage return.
			// We want to set the style of the broken paragraph to that of the
			// paragraph after us. Since the last character was a CR, there
			// must be another paragraph in the source (if things are consistent
			// as they should be).

			if (SourceIterator.ParagraphIndex() >= pSourceText->NumberOfParagraphs()
				 || DestIterator.ParagraphIndex() >= pDestText->NumberOfParagraphs())
			{
				// No next paragraph in the source after a CR!
				// Or no paragraph after a split!
				ASSERT(FALSE);
			}
			else
			{
				// Set the paragraph style of the next paragraph.
				CTextStyle Style;
				pSourceText->GetParagraphStyle(SourceIterator.ParagraphIndex(), Style);
				PARAGRAPH_INDEX lParagraph = DestIterator.ParagraphIndex();
				pDestText->SetParagraphStyle(lParagraph, &Style);
				pDestText->ChangeParagraphStyle(lParagraph, lParagraph, &Style);
			}
		}
#endif
		// Fixup all the macro characters we inserted.
		pDestText->FixupMacros(pDatabase, lDestination, lDestination + lEnd-lStart);

		// Adjust the text states.
		pDestText->AdjustFrameStates(&StartState,
											  pDestText->NumberOfCharacters()-lStartCharacters,
											  pDestText->NumberOfParagraphs()-lStartParagraphs,
											  lFrame);
	}
	CATCH_ALL(e)
	{
		ASSERT(FALSE);
		SourceIterator.Reset();
		DestIterator.Reset();
		THROW_LAST();
	}
	END_CATCH_ALL
}

/*
// Copy text to a paragraph in another database.
*/

ERRORCODE CopyTextToDatabase(CTxp* pTxp, PMGDatabasePtr pDestDatabase, DB_RECORD_NUMBER *plParagraph, UNDO_STATE nUndo /*=UNDO_None*/)
{
	ERRORCODE error = ERRORCODE_None;

/* Create the clip text record. */

	CTextRecord* pText = NULL;
	CTextRecord* pNewText = NULL;

	TRY
	{
		// Create a new story in which to place the text.
		if ((pNewText = pDestDatabase->NewStory()) == NULL)
		{
			ASSERT(pDestDatabase->last_creation_error() != ERRORCODE_None);
			ThrowErrorcodeException(pDestDatabase->last_creation_error());
		}

		// Create an empty paragraph that we can insert into.
		CTextParagraph Paragraph;

		Paragraph.m_lBaseParagraphStyle = pDestDatabase->ImportTextStyleRecord(pTxp->Style().ParagraphStyle(), pTxp->Database());
		Paragraph.m_StyleChange.m_lValue = Paragraph.m_lBaseParagraphStyle;

		pNewText->InsertParagraph(0, &Paragraph);
		// Get rid of the extra ref we have on the style.
		// InsertParagraph() incremented it for us.
		pDestDatabase->FreeTextStyleRecord(Paragraph.m_lBaseParagraphStyle);

		// Lock the source text.
		pText = pTxp->LockText();

		// Copy text to this new text record.
		CopyTextToText(pNewText, 0, pText, pTxp->SelectStart(), pTxp->SelectEnd(), nUndo);
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH(CMemoryException, e)
	{
		error = ERRORCODE_Memory;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	// Get rid of the source text.
	if (pText != NULL)
	{
		pText->release();
	}

	// Handle the destination text.
	if (pNewText != NULL)
	{
		DB_RECORD_NUMBER lText = pNewText->Id();
		pNewText->release();

		if (error == ERRORCODE_None)
		{
			// Save it.
			*plParagraph = lText;
		}
		else
		{
			// Delete it.
			pDestDatabase->DeleteStory(lText);
		}
	}
	return error;
}

/*
// Copy objects to the database.
*/

ERRORCODE CopyObjectsToDatabase(CPmwDoc* pDoc, CClipboardOpenState* pState, DB_RECORD_NUMBER *new_precord, BOOL fSelectedOnly/*=TRUE*/, BOOL fSkipSpecial /*=FALSE*/)
{
	ERRORCODE error = ERRORCODE_None;
	ObjectListPtr pList = NULL;
	PMGDatabase* dst_database = pState->GetDatabase();

	if (pState->m_pDoc == NULL)
	{
		PMGDatabasePtr database = pDoc->get_database();
	//	DB_RECORD_NUMBER p_record = txp->get_paragraph();

	/* Create the clip page. */

		if ((pState->m_pPage = dst_database->create_page()) == NULL)
		{
			return dst_database->last_creation_error();
		}

	/* Get a pointer to the list so we can copy the objects. */

		pList = pState->m_pPage->objects();

	/* Return the record number. */

		*new_precord = pState->m_pPage->Id();
		pState->m_pPage->modified();
	}
	else
	{
		pList = pState->m_pDoc->object_list();
	}

	// Copy the objects to the clip page.
	// We will build a map of old and new frame record numbers so we can
	// go through later and fix up the linked frames.

	CMapPtrToPtr Map;
	PMGPageObjectPtr pObject = fSelectedOnly ? pDoc->selected_objects() : pDoc->objects();
	while (pObject != NULL)
	{
		if (!fSkipSpecial ||
			 (!(pObject->get_select_flags() & SELECT_FLAG_solo_select)
			  && !(pObject->get_flags() & OBJECT_FLAG_locked)))
		{
			PMGPageObject* pNewObject = pDoc->CopyObject(pObject, pList);
			if (pNewObject != NULL)
			{
				PMGDatabase::MapCopiedFrames(pObject, pNewObject, Map);
			}
		}

		pObject = fSelectedOnly ?
			pObject->next_selected_object() : (PMGPageObjectPtr)pObject->next_object();
	}

	// We have finished copying the objects. Go through and link everything
	// up as necessary.

	dst_database->LinkCopiedFrames(pDoc->get_database(), Map, FALSE);

	return error;			/* Should be ERRORCODE_None! May be ERRORCODE_Full! */
}

/*
// Copy objects to the Windows clipboard.
*/

ERRORCODE Clipboard::CopyObjectsToClipboard(CPmwDoc* pDoc)
{
	SetClipboardData(CF_METAFILEPICT, NULL);		// Delayed rendering
	m_fRenderedMetafile = FALSE;

	SetClipboardData(CF_DIB, NULL);					// Delayed rendering
	SetClipboardData(CF_BITMAP, NULL);				// Delayed rendering
	m_fRenderedBitmap = FALSE;

	SetClipboardData(CF_PALETTE, NULL);		// Delayed rendering
	m_fRenderedPalette = FALSE;

	return ERRORCODE_None;
}

/*
// Copy text to the Windows clipboard.
*/

ERRORCODE CopyTextToClipboard(CTxp* pTxp)
{
	PMGDatabasePtr database = pTxp->Database();
	DB_RECORD_NUMBER lText = pTxp->TextRecord();
	ERRORCODE error;
	CMacroServer* pMacroServer = database->GetMacroServer();

/* Get the source text. */

	CTextRecord* pText;

	if ((pText = (CTextRecord*)database->get_record(lText, &error, RECORD_TYPE_Text)) == NULL)
	{
		return error;
	}

	CHARACTER_INDEX lStart = pTxp->SelectStart();
	CHARACTER_INDEX lEnd = pTxp->SelectEnd();

	// Compute how many characters we want to put on the clipboard.
	CHARACTER_COUNT lCharacters = 0;

	CHARACTER* pCharacter = NULL;
	CHARACTER_COUNT lThisCharacters = 0;

	CHARACTER_INDEX l;

	for (l = lStart; l < lEnd; l++)
	{
		if (lThisCharacters == 0)
		{
			pCharacter = pText->GetCharacters(l, &lThisCharacters);
		}

		CHARACTER c = *pCharacter++;
		lThisCharacters--;

		if (c >= MACRO_CHARACTER)
		{
			CHARACTER* pMacro;
			if (pMacroServer == NULL
				 || (pMacro = pMacroServer->GetMacroText(c)) == NULL)
			{
			/* Just stick in a '?'. */
				lCharacters++;
			}
			else
			{
				while (*pMacro++ != 0)
				{
					lCharacters++;
				}
			}
		}
		else
		{
			switch (c)
			{
				case '\n':
				{
					lCharacters++;
				/* Fall through to... */
				}
				default:
				{
					lCharacters++;
					break;
				}
			}
		}
	}

	// Allocate a block for our text.
	HGLOBAL hClipText;
	if ((hClipText = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, lCharacters+1)) == NULL)
	{
		pText->release();
		return ERRORCODE_Memory;
	}

	LPSTR pClipText;
	if ((pClipText = (LPSTR)::GlobalLock(hClipText)) == NULL)
	{
		::GlobalFree(hClipText);
		pText->release();
		return ERRORCODE_Memory;
	}

	// Copy the text over to the allocated block.
	LPSTR p = pClipText;

	pCharacter = NULL;
	lThisCharacters = 0;

	for (l = lStart; l < lEnd; l++)
	{
		if (lThisCharacters == 0)
		{
			pCharacter = pText->GetCharacters(l, &lThisCharacters);
		}

		CHARACTER c = *pCharacter++;
		lThisCharacters--;

		if (c >= MACRO_CHARACTER)
		{
			CHARACTER* pMacro;
			if (pMacroServer == NULL
				 || (pMacro = pMacroServer->GetMacroText(c)) == NULL)
			{
			/* Just stick in a '?'. */
				*p++ = '?';
			}
			else
			{
				while ((c = *pMacro++) != 0)
				{
					if (c >= 32 && c < 128)
					{
					/* Standard ASCII. */
						*p++ = (char)c;
					}
					else
					{
					/* Not defined. */
						*p++ = '*';
					}
				}
			}
		}
		else
		{
			switch (c)
			{
				case '\n':
				{
					*p++ = '\r';
					*p++ = '\n';
					break;
				}
				default:
				{
					if (c >= 32 && c < 128)
					{
					/* Standard ASCII. */
						*p++ = (char)c;
					}
					else
					{
					/* Not defined. */
						*p++ = '*';
					}
					break;
				}
			}
		}
	}
	*p = '\0';

	::GlobalUnlock(hClipText);
	::SetClipboardData(CF_TEXT, hClipText);

	pText->release();

	return ERRORCODE_None;
}

/*
// Copy text to the clipboard.
*/

ERRORCODE Clipboard::CopyText(CPmwView* pView)
{
	CTxp* pTxp = pView->GetTxp();
	CPmwDoc* pDoc = pView->GetDocument();
	ERRORCODE error;

	if (!pTxp->Valid() || !pTxp->SomethingSelected())
	{
		return ERRORCODE_BadParameter;
	}

/*
// Prepare the clipboard for us to write to it.
// The clipboard is left open if all is well (otherwise, it's left closed).
*/

	CClipboardOpenState OpenState;

	if ((error = PrepareClipboardForCopy(&OpenState)) != ERRORCODE_None)
	{
		return error;
	}

/*
// Generate the text in this database.
*/

	DB_RECORD_NUMBER new_record = 0;
	error = CopyTextToDatabase(pTxp, OpenState.m_Mapping.database, &new_record);

/*
// Flush the database.
*/

	OpenState.m_Mapping.flush_database();

	if (error != ERRORCODE_None)
	{
		CloseClipboard();
		DestroyContents();
		return error;
	}

/*
// Create the standard clipboard format.
*/

	m_World.x0 =
		m_World.y0 =
		m_World.x1 =
		m_World.y1 = 0;

	CreateStandardClipboard(CLIPBOARD_TYPE_text, new_record, &m_World);

/*
// Generate text in the Windows clipboard.
*/

	if ((error = CopyTextToClipboard(pTxp)) != ERRORCODE_None)
	{
		EmptyClipboard();
		CloseClipboard();
		DestroyContents();
		return error;
	}

	CloseClipboard();

/* We have something internal. */

	m_Type = CLIPBOARD_TYPE_text;
	m_dwRecord = new_record;

	return ERRORCODE_None;
}

/*
// Cut text to the clipboard (copy then delete).
*/

ERRORCODE Clipboard::CutText(CPmwView* pView, CCommandList* pCommandList /*=NULL*/)
{
	ERRORCODE error;

/*
// Create an undo/redo command.
*/

	CTextCommand* pCommand = new CTextCommand(IDCmd_CutText);

	if (!pCommand->Before(pView))
	{
		delete pCommand;
		pCommand = NULL;
	}

/*
// Do the cut.
*/

   CTxp* pTxp = pView->GetTxp();
   CPmwDoc* pDoc = pView->GetDocument();

	if ((error = CopyText(pView)) == ERRORCODE_None)
	{
	/* Delete the selected text which we copied. */
		error = pTxp->DeleteText(1);
	}

/*
// Finish up the command.
*/

	if (pCommand != NULL)
	{
		if (pCommand->After())
		{
			if (pDoc == NULL)
			{
				if (pCommandList == NULL)
				{
					delete pCommand;
				}
				else
				{
					pCommandList->AddCommand(pCommand);
				}
			}
			else
			{
				pDoc->AddCommand(pCommand, FALSE);
			}
		}
		else
		{
			delete pCommand;
		}
	}

	return error;
}

/*
// Read the clipboard info for a database paste.
// This will set 'record' and the name passed in.
*/

ERRORCODE Clipboard::PrepareDocumentPaste(CPmwDoc** ppDoc)
{
	ERRORCODE error;
	char name[_MAX_PATH];

	*ppDoc = NULL;

	if ((error = EnsureLocalClipboard(name)) != ERRORCODE_None)
	{
		return error;
	}

	WORD wGlobalSave = CPmwDoc::m_wGlobalDocumentFlags;
	CPmwDoc::m_wGlobalDocumentFlags = CPmwDoc::FLAG_ClipDocument;
	*ppDoc = (CPmwDoc*)GET_PMWAPP()->m_pScratchTemplate->OpenDocumentFile(name, FALSE);
	CPmwDoc::m_wGlobalDocumentFlags = wGlobalSave;

	return (*ppDoc == NULL) ? ERRORCODE_Open : ERRORCODE_None;
}

ERRORCODE Clipboard::PrepareObjectPaste(CClipboardOpenState* pState)
{
/*
// Prepare as if we were pasting.
*/

	ERRORCODE error = ERRORCODE_None;

	pState->m_pList = NULL;

	switch (TypeOfMlsClipboard(FALSE))
	{
		case CLIPBOARD_TYPE_objects:
		{
			if ((error = PrepareDatabasePaste(&pState->m_Mapping)) != ERRORCODE_None)
			{
				return error;
			}

		/* Get the source (clipboard) page. */

			if ((pState->m_pPage = (PagePtr)pState->m_Mapping.database->get_record(m_dwRecord, &error, RECORD_TYPE_Page)) == NULL)
			{
				return error;
			}

			pState->m_pList = pState->m_pPage->objects();
			break;
		}
		case CLIPBOARD_TYPE_object_document:
		{
			if ((error = PrepareDocumentPaste(&pState->m_pDoc)) != ERRORCODE_None)
			{
				return error;
			}
			ASSERT(pState->m_pDoc != NULL);

			pState->m_pList = pState->m_pDoc->object_list();

			break;
		}
		default:
		{
			error = ERRORCODE_IllegalType;
			break;
		}
	}
	return error;
}

/*
// Read the clipboard info for a database paste.
// This will set 'record' and the name passed in.
*/

ERRORCODE Clipboard::PrepareDatabasePaste(DatabaseMapping *mapping)
{
	ERRORCODE error;
	char name[_MAX_PATH];

	if ((error = EnsureLocalClipboard(name)) != ERRORCODE_None)
	{
		return error;
	}

/* Create a database on top of this file (read-only). */

	if ((error = mapping->map_to_file(name, TRUE)) != ERRORCODE_None)
	{
		return error;
	}

/* Open and read the database state. */

	SHORT signature;
	SHORT file_type, file_flags;

	if ((error = mapping->open(&signature, &file_type, &file_flags)) == ERRORCODE_None)
	{
	/* Get the document record. */
		DocumentPtr pDocument;

		if ((pDocument = (DocumentPtr)mapping->database->get_record(PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document)) == NULL)
		{
			return error;
		}
		DB_RECORD_NUMBER nMRecord = pDocument->get_macro_list_record();
		pDocument->release();

	/* Lock the macro list record. */

		if (nMRecord != 0)
		{
			MacroListRecord* pMacroRecord = (MacroListRecord*)mapping->database->get_record(nMRecord, &error, RECORD_TYPE_MacroList);
			if (pMacroRecord != NULL)
			{
			/* Set the list we got. */
				mapping->m_pMacroServer->SetMacroList(pMacroRecord);
			}
		}
	}

	return error;
}

ERRORCODE Clipboard::EnsureLocalClipboard(LPSTR pName)
{
	ERRORCODE error = ERRORCODE_None;

	if (m_Type == CLIPBOARD_TYPE_unknown || m_pszClipboardName == NULL)
	{
	/* We need to use the one in the Windows clipboard. */

		error = ERRORCODE_Busy;

		if (OpenClipboard())
		{
			HGLOBAL hData;
			if ((hData = ::GetClipboardData(m_uFilenameFormat)) == NULL)
			{
				error = ERRORCODE_DoesNotExist;
			}
			else
			{
				CLIPBOARD_FILENAME_PTR pData;

				error = ERRORCODE_Memory;

				if ((pData = (CLIPBOARD_FILENAME_PTR)::GlobalLock(hData)) != NULL)
				{
				/* We got the record. Extract the info we need. */
					strcpy(pName, pData->m_cbName);
					m_dwRecord = pData->m_dwRecord;
					if (pData->m_Type > CLIPBOARD_TYPE_text)
					{
					/*
					// This field may not exist in previous version clipboards
					// (which only stored text).
					*/
						m_World = pData->m_World;
						TRACE("Clipboard world is %ld, %ld, %ld, %ld\n", m_World);
					}
					::GlobalUnlock(hData);
					error = ERRORCODE_None;
				}
			}
			CloseClipboard();
		}
	}
	else
	{
	/* Use our local info ('m_dwRecord' is set from the cut/copy). */
		strcpy(pName, m_pszClipboardName);
	}
	return error;
}

/*
// Paste from the database file.
*/

ERRORCODE Clipboard::PasteTextFromDatabase(CTxp* pTxp, BOOL fWarpTextPaste /*=FALSE*/)
{
	DatabaseMapping mapping;
	ERRORCODE error;

/* Prepare the database mapping so we can paste. */

	if ((error = PrepareDatabasePaste(&mapping)) != ERRORCODE_None)
	{
		return error;
	}

	return PasteTextFromText(pTxp, mapping.database, m_dwRecord, UNDO_None, fWarpTextPaste);
}

ERRORCODE PasteTextFromText(CTxp* pTxp, PMGDatabase* pSrcDatabase, DB_RECORD_NUMBER lSourceText, UNDO_STATE nUndo /*=UNDO_None*/, BOOL fWarpTextPaste /*=FALSE*/)
{
#if 1
	CTextRecord* pSourceText = NULL;
	CTextRecord* pDestText = NULL;
	CFrameRecord* pDestFrame = NULL;
	ERRORCODE error;

	TRY
	{
		// Lock the source text.
		pSourceText = (CTextRecord*)pSrcDatabase->get_record(lSourceText, &error, RECORD_TYPE_Text);
		if (pSourceText == NULL)
		{
			ThrowErrorcodeException(error);
		}

		// Lock the destination text.
		pDestText = pTxp->LockText();

		CHARACTER_INDEX lInsert = pTxp->Index();
		CHARACTER_COUNT lCount = pSourceText->NumberOfCharacters();

		DB_RECORD_NUMBER lFrame = pDestText->GetFrameOfCharacter(lInsert, pTxp->FrameRecord());

		// Update the counts in the line and word.
		pDestFrame = pTxp->LockFrame(lFrame);

		CFrameLocation Location;
		Location.m_lCharacter = lInsert;
		pDestFrame->GetCharacterLocation(&Location);

		CTextLine* pLine = pDestFrame->GetLine(Location.m_lLine);
		pLine->m_lCharacters += lCount;
		CTextWord* pWord = pDestFrame->GetWord(Location.m_lWord);
		pWord->m_lCharacters += lCount;

		CopyTextToText(pDestText, lInsert, pSourceText, 0, pSourceText->NumberOfCharacters(), nUndo, pTxp->FrameRecord());

		// Update the text.
		pTxp->SelectText(lInsert, lInsert+lCount, FALSE);
		pTxp->UpdateRange();
		pTxp->SetDesiredX();
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH(CMemoryException, e)
	{
		error = ERRORCODE_Memory;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	// Get rid of any text records we may have locked.
	if (pSourceText != NULL)
	{
		pSourceText->release();
	}
	if (pDestText != NULL)
	{
		pDestText->release();
	}
	if (pDestFrame != NULL)
	{
		pDestFrame->release();
	}
	return error;
#else
/* Get the paragraph we'll be pasting into. */

	ERRORCODE error;
	PMGDatabasePtr database = txp->get_database();
	DB_RECORD_NUMBER p_record = txp->get_paragraph();
	ParagraphPtr paragraph;

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) == NULL)
	{
		return error;
	}

/* Get the source (clipboard) paragraph. */

	ParagraphPtr src_para;
	if ((src_para = (ParagraphPtr)pSrcDatabase->get_record(src_record, &error, RECORD_TYPE_Paragraph)) == NULL)
	{
		paragraph->release();
		return error;
	}

/*
// Copy all the text over.
// Compute how many characters actually got inserted.
*/

	SHORT inserted = paragraph->number_of_characters();
	C_INDEX here = txp->get_index();		/* Where to insert */

	od("Pasting %d characters @ %d...\r\n", src_para->number_of_characters(), here);

	error = txp->insert_text(src_para->get_character(0),
 							src_para->number_of_characters());
	inserted = paragraph->number_of_characters() - inserted;

/*
// Fixup all macros in the text.
*/

	MacroListRecord* pSrcList = pSrcDatabase->GetMacroList();
	MacroListRecord* pDstList = database->GetMacroList();

	C_INDEX c_index;
	for (c_index = here; c_index < here + inserted; c_index++)
	{
		CHARACTER* cp = paragraph->get_character(c_index);
		if (*cp >= MACRO_CHARACTER)
		{
			CHARACTER c;

			if (pSrcList != NULL
				 && pDstList != NULL
				 && (c = pDstList->ImportMacro(*cp - MACRO_CHARACTER, pSrcList)) != 0)
			{
			/* We converted the macro! */
				*cp = c + MACRO_CHARACTER;
			}
			else
			{
			/* Turn it back into something "normal". */
				*cp = '?';
			}
		}
	}
	if (pSrcList != NULL)
	{
		pSrcList->release();
	}
	if (pDstList != NULL)
	{
		pDstList->release();
	}

	paragraph->UpdateMacroState();

/*
// Get the style of the source paragraph.
// We make this a "permanent" style so that the font record number will be
// converted to our database.
*/

	TextStyle style(database, FALSE);			/* <- Not temp! */
	style = src_para->get_style();

	if (fWarpTextPaste)
	{
	/*
	// Remove all style attributes that we don't support in a warp text
	// paragraph.
	*/
		TextStyle& DstStyle = paragraph->get_style();
		style.set_size(DstStyle.get_size());
		style.set_size_fraction(DstStyle.get_size_fraction());
		style.set_base_size(DstStyle.get_base_size());
		style.set_base_size_fraction(DstStyle.get_base_size_fraction());
		style.set_pattern(DstStyle.get_pattern());
		style.set_outline(DstStyle.get_outline());
		style.set_background(DstStyle.get_background());
		style.set_shadow(DstStyle.get_shadow());
		style.set_color(DstStyle.get_color());
		style.set_bcolor(DstStyle.get_bcolor());
		style.UnderlineStyle(DstStyle.UnderlineStyle());
		style.UnderlineColor(DstStyle.UnderlineColor());
		style.update_font_metrics();
	}

	CHARACTER_RANGE crange;
	crange.c_start = here;

	SHORT changes = src_para->number_of_changes();
	SHORT i;
	STYLE_CHANGE_PTR scp;

	for (i = 0, scp = src_para->get_style_change(0); i < changes; )
	{
		C_INDEX scp_c_index = scp->c_index;

		crange.c_end = scp_c_index + here-1;

	/* Do the style to here. */

		paragraph->change_range_style(crange, &style, FALSE);

	/* Pick up this style for the next range. */

		crange.c_start = crange.c_end+1;

	/* Advance to the next style change which is not here. */

		do
		{
			DWORD value_save = scp->new_value;

			if (scp->token == TOKEN_Face)
			{
				CHAR name[80];

			/*
			// Database to database transfer.
		 	// We need to transfer the font record by name.
		 	*/

				DB_RECORD_NUMBER old_face = (DB_RECORD_NUMBER)scp->new_value;

				pSrcDatabase->font_data_name(old_face, name);
				scp->new_value = (LONG)database->new_font_record(name);
			}

			if (!fWarpTextPaste
				 || scp->token == TOKEN_Face
				 || scp->token == TOKEN_Bold
				 || scp->token == TOKEN_Italic)
			{
				style.apply_change(scp);
			}
			scp->new_value = value_save;

			i++;
			scp++;
		} while (i < changes && scp->c_index == scp_c_index /*&& scp->c_index < here + inserted */);
	}

/* Now do the final range. */

	crange.c_end = txp->get_index() - 1;
	paragraph->change_range_style(crange, &style, FALSE);

/* Now fixup all style changes we just did. */

	crange.c_start = here;
	crange.c_end++;

	WORD_RANGE wrange;
	paragraph->crange_to_wrange(crange, &wrange);
	paragraph->size_words(wrange, TRUE);

/* Clean up. */

	src_para->release();
	paragraph->release(TRUE);
	style.destroy();								/* Explicitly destroy the style. */

/* Flow the text. */

	TextFlow textflow(database, txp->get_object());

	textflow.rebuild_lines(txp->get_object(), wrange);

/* Select the text we just pasted. */

	txp->select_text(here, txp->get_index());

	return error;
#endif
	return ERRORCODE_None;
}

/*
// Paste from the Windows clipboard.
*/

ERRORCODE Clipboard::PasteTextFromClipboard(CTxp* pTxp)
{
	UINT uFormat;
	ERRORCODE error = ERRORCODE_DoesNotExist;

/* Find the format we will paste. */

	if (FormatIsAvailable(uFormat = m_uRichTextFormat)
			|| FormatIsAvailable(uFormat = CF_TEXT)
			|| FormatIsAvailable(uFormat = CF_OEMTEXT))
	{
	/* Open the clipboard to access the data. */

		if (OpenClipboard())
		{
			HGLOBAL hData;

		/* Get the data we want. */

			if ((hData = ::GetClipboardData(uFormat)) != NULL)
			{
				LPSTR pData;

				if ((pData = (LPSTR)::GlobalLock(hData)) == NULL)
				{
					error = ERRORCODE_Memory;
				}
				else
				{
					// We got the data.
					USHORT len = strlen(pData);
					if (uFormat == m_uRichTextFormat)
					{
						// Remember where we were when we started.
						CHARACTER_INDEX c_start = pTxp->Index();

						TRACE("Got Rich Text (%u, %lu)!\n", len, GlobalSize(hData));
						// Setup our source.
						CMemoryRTFSource Source((LPBYTE)pData, GlobalSize(hData)-1);
						CTxpRTFDestination Destination(pTxp);

						// Setup our parser.
						CRTFParser Parser(&Source, &Destination);

						TRY
						{
							Parser.Parse();
						}
						CATCH(CRTFException, e)
						{
							TRACE("Got parse exception: %d\n", e->m_nError);
						}
						AND_CATCH_ALL(e)
						{
							ASSERT(FALSE);
						}
						END_CATCH_ALL

						pTxp->SelectText(c_start, pTxp->Index());
					}
					else
					{
						if (len > 32766)
						{
							len = 32766;
						}

						CHARACTER_PTR char_mem = NULL;

						TRY
							char_mem = new CHARACTER[len];
						END_TRY
						if (char_mem == NULL)
						{
							error = ERRORCODE_Memory;
						}
						else
						{
							CHARACTER_PTR cp = char_mem;

							LPSTR p;
							USHORT i;
							for (p = pData, i = 0, cp = char_mem; i < len; i++)
							{
								BYTE c;

								switch (c = (BYTE)*p++)
								{
									case '\r':
									{
									/* Ignore this. */
										break;
									}
									default:
									{
										*cp++ = (CHARACTER)c;
										break;
									}
								}
							}

							error = ERRORCODE_None;
							if (cp != char_mem)
							{
								CHARACTER_INDEX c_start = pTxp->Index();
								error = pTxp->InsertText(char_mem, cp - char_mem);
								pTxp->SelectText(c_start, pTxp->Index());
							}

							delete [] char_mem;
						}
					}
					::GlobalUnlock(hData);
				}
			}
			CloseClipboard();
		}
		else
		{
			error = ERRORCODE_Busy;
		}
	}

	return error;
}

/*
// Paste text from the clipboard.
*/

ERRORCODE Clipboard::PasteText(CPmwView* pView, CCommandList* pCommandList /*=NULL*/, BOOL fWarpTextPaste /*=FALSE*/)
{
   CTxp* pTxp = pView->GetTxp();
   CPmwDoc* pDoc = pView->GetDocument();

	if (!pTxp->Valid())
	{
		return ERRORCODE_BadParameter;
	}

/*
// Create an undo/redo command.
*/

	CTextCommand* pCommand = new CTextCommand(IDCmd_PasteText);

	if (!pCommand->Before(pView))
	{
		delete pCommand;
		pCommand = NULL;
	}

	if (pTxp->SomethingSelected())
	{
	/* Get rid of the text here first. */
		pTxp->DeleteText(1);
	}

/* Now do the paste. */

	ERRORCODE error;

/* Try the database clipboard. */

	if (TypeOfMlsClipboard(FALSE) != CLIPBOARD_TYPE_text
				|| (error = PasteTextFromDatabase(pTxp, fWarpTextPaste)) != ERRORCODE_None)
	{
	/* Try the Windows clipboard. */
		error = PasteTextFromClipboard(pTxp);
	}

	if (pCommand != NULL)
	{
		if (pCommand->After())
		{
			if (pDoc == NULL)
			{
				if (pCommandList == NULL)
				{
					delete pCommand;
				}
				else
				{
					pCommandList->AddCommand(pCommand);
				}
			}
			else
			{
				pDoc->AddCommand(pCommand, FALSE);
			}
		}
		else
		{
			delete pCommand;
		}
	}

	pTxp->Deselect();

	return error;
}

BOOL Clipboard::NeedDocumentClipboard(CPmwDoc* pDoc)
{
	PMGPageObjectPtr pObject = pDoc->selected_objects();

	while (pObject != NULL)
	{
		if (NeedDocumentClipboard(pObject))
		{
			return TRUE;
		}

		pObject = pObject->next_selected_object();
	}
	return FALSE;
}

BOOL Clipboard::NeedDocumentClipboard(PMGPageObjectPtr pObject)
{
	switch (pObject->type())
	{
		case OBJECT_TYPE_OleObject:
		case OBJECT_TYPE_Serif:
		{
		/* We need a document for us! */
			return TRUE;
		}
		case OBJECT_TYPE_Group:
		case OBJECT_TYPE_Calendar:
		{
			ObjectList* pList = ((GroupObject*)pObject)->object_list();

			PMGPageObjectPtr pObject = (PMGPageObjectPtr)pList->first_object();

			while (pObject != NULL)
			{
				if (NeedDocumentClipboard(pObject))
				{
					return TRUE;
				}

				pObject = (PMGPageObjectPtr)pObject->next_object();
			}
			break;
		}
		default:
		{
			break;
		}
	}
	return FALSE;
}

/*
// Copy objects to the clipboard.
*/

ERRORCODE Clipboard::CopyObjects(CPmwDoc* pDoc)
{
	ERRORCODE error;

	if (pDoc->selected_objects() == NULL)
	{
		return ERRORCODE_BadParameter;
	}

	BOOL fNeedDocument = NeedDocumentClipboard(pDoc);

/*
// Prepare the clipboard for us to write to it.
// The clipboard is left open if all is well (otherwise, it's left closed).
*/

	CClipboardOpenState OpenState;

	if ((error = PrepareClipboardForCopy(&OpenState, fNeedDocument)) != ERRORCODE_None)
	{
		return error;
	}

/*
// Generate the text in this database.
*/

	DB_RECORD_NUMBER new_record = 0;

	error = CopyObjectsToDatabase(pDoc, &OpenState, &new_record);

/*
// Flush the database.
*/

	if (fNeedDocument)
	{
		if (error == ERRORCODE_None)
		{
		/* We need to save the document. */
			OpenState.m_pDoc->SaveToFile(m_pszClipboardName, TRUE); 
		}
//		OpenState.m_pDoc->OnCloseDocument();
	}
	else
	{
		OpenState.m_Mapping.flush_database();
	}

	if (error != ERRORCODE_None)
	{
		CloseClipboard();
		DestroyContents();
		return error;
	}

/*
// Create the standard clipboard format.
*/

	pDoc->get_panel_world(&m_World);

	CLIPBOARD_TYPE nNewType = fNeedDocument ? CLIPBOARD_TYPE_object_document : CLIPBOARD_TYPE_objects;
	CreateStandardClipboard(nNewType,
									new_record, &m_World);

/*
// Generate objects on the Windows clipboard.
*/

	if ((error = CopyObjectsToClipboard(pDoc)) != ERRORCODE_None)
	{
		EmptyClipboard();
		CloseClipboard();
		DestroyContents();
		return error;
	}

	CloseClipboard();

/* We have something internal. */

	m_Type = nNewType;
	m_dwRecord = new_record;

	return ERRORCODE_None;
}

/*
// Cut objects to the clipboard (copy then delete).
*/

ERRORCODE Clipboard::CutObjects(CPmwDoc* pDoc)
{
	ERRORCODE error;

	if ((error = CopyObjects(pDoc)) == ERRORCODE_None)
	{
	/* Delete the selected objects which we copied. */
		pDoc->delete_selects(FALSE, IDCmd_CutObjects);
	}
	return error;
}

static BOOL GetObjectBounds(ObjectListPtr pList, PBOX_PTR pBounds)
{
	PageObjectPtr pObject;
	BOOL fGotOne = FALSE;

	for (pObject = pList->first_object(); pObject != NULL; pObject = pObject->next_object())
	{
		PBOX ObjBound;

		ObjBound = ((RectPageObjectPtr)pObject)->get_bound();

		if (fGotOne)
		{
			UnionBox(pBounds, pBounds, &ObjBound);
		}
		else
		{
			*pBounds = ObjBound;
			fGotOne = TRUE;
		}
	}

	if (!fGotOne)
	{
		pBounds->x0 =
			pBounds->y0 =
			pBounds->x1 =
			pBounds->y1 = 0;
	}

	return fGotOne;
}


/* Determines the bounding rect for the objects in the pointer list.
 */
static BOOL GetObjectBounds(CPtrList *pList, PBOX_PTR pBounds)
{
	PageObjectPtr pObject;
	BOOL fGotOne = FALSE;

	POSITION posList = pList->GetHeadPosition();
	while (posList != NULL)
	{
		pObject = static_cast<PageObjectPtr>(pList->GetNext( posList ));
		PBOX ObjBound;

		ObjBound = ((RectPageObjectPtr)pObject)->get_bound();

		if (fGotOne)
		{
			UnionBox(pBounds, pBounds, &ObjBound);
		}
		else
		{
			*pBounds = ObjBound;
			fGotOne = TRUE;
		}
	}

	if (!fGotOne)
	{
		pBounds->x0 =
			pBounds->y0 =
			pBounds->x1 =
			pBounds->y1 = 0;
	}

	return fGotOne;
}

/*
// Paste from the database file.
*/

ERRORCODE Clipboard::PasteObjectsFromDatabase(CPmwDoc* pDoc)
{
	ERRORCODE error;

	CClipboardOpenState OpenState;

/* Prepare the database mapping so we can paste. */

	if ((error = PrepareObjectPaste(&OpenState)) != ERRORCODE_None)
	{
		return error;
	}

	return PasteObjectList(pDoc, OpenState.m_pList, m_World);
}

/*
// Helper function that pastes a list of objects into the passed document.
*/

ERRORCODE PasteObjectList(CPmwDoc* pDoc, ObjectList* pList, PBOX pbxWorld)
{
	PBOX Bounds;

	if (GetObjectBounds(pList, &Bounds))
	{
	/*
	// Copy all the objects over.
	// The document provides the pasting operations.
	// We maintain a map of copied frames so we can link them up afterwards.
	*/

		CMapPtrToPtr Map;

		for (PMGPageObject* pObject = (PMGPageObject*)pList->first_object();
			  pObject != NULL;
			  pObject = (PMGPageObject*)pObject->next_object())
		{
			PMGPageObject* pNewObject = pDoc->PasteObject(pObject, &pbxWorld, &Bounds);
			if (pNewObject != NULL)
			{
				PMGDatabase::MapCopiedFrames(pObject, pNewObject, Map);
			}
		}

		// Link up any frames that are copied over (that need it).
		pDoc->get_database()->LinkCopiedFrames((PMGDatabase*)pList->get_database(), Map, TRUE);
	}

	return ERRORCODE_None;
}

/*
// Build the name of a pasted object.
*/

void Clipboard::BuildPasteName(LPCSTR pszExtension, LPSTR pszName)
{
	sprintf(pszName,
			  "%02x\\%08lx\\CLIP%s",
			  GET_PMWAPP()->m_nUserID,
			  m_dwUpdateTime,
			  pszExtension);
}

/*
// Paste from the Windows clipboard.
*/

ERRORCODE Clipboard::PasteObjectsFromClipboard(CPmwView* pView)
{
	ERRORCODE error = ERRORCODE_DoesNotExist;
	CPmwApp* pApp = GET_PMWAPP();
	CPmwDoc* pDoc = pView->GetDocument();

/*
// Find the format we will paste.
*/

	UINT uINIFormat;
	UINT uFormat = 0;

/*
// Search in the INI file for the preferred formats.
// The order of this 'if' determines the precedence of paste formats.
// The Art Gallery format is also a preferred format.
*/

	if (FormatIsAvailable(uINIFormat = pApp->OurGetProfileInt(SECTION_Clipboard, ENTRY_Format1, 0))
			|| FormatIsAvailable(uINIFormat = pApp->OurGetProfileInt(SECTION_Clipboard, ENTRY_Format2, 0))
			|| FormatIsAvailable(uINIFormat = pApp->OurGetProfileInt(SECTION_Clipboard, ENTRY_Format3, 0))
			|| FormatIsAvailable(uINIFormat = CArtBrowserDialog::m_cfDataExchange))
	{
		uFormat = uINIFormat;
	}
	else
	{
	/*
	// Find the first format that is one we can paste.
	// Precedence is determined by the order in which formats are placed
	// on the clipboard (which is as it should be).
	*/

		int nFormats = m_Formats.count();
		for (int nFormat = 0; nFormat < nFormats; nFormat++)
		{
			UINT uLocalFormat = *(UINT far *)m_Formats.get_element(nFormat);
			if (uLocalFormat == CF_METAFILEPICT
					|| uLocalFormat == m_cfEmbeddedObject
					|| uLocalFormat == m_cfEmbedSource
					|| uLocalFormat == m_cfLinkSource
					|| uLocalFormat == m_cfFileName
					|| uLocalFormat == CF_HDROP
					|| uLocalFormat == CF_DIB
					|| uLocalFormat == CF_BITMAP
					|| uLocalFormat == kComponentFormat)
			{
				uFormat = uLocalFormat;
				break;
			}
		}
	}

	if (uFormat != 0)
	{
		COleDataObject Data;

		if (Data.AttachClipboard())
		{
			error = PasteFromOleDataObject(pView, uFormat, &Data);
		}
		else
		{
			error = ERRORCODE_Busy;
		}
	}
	return error;
}

void Clipboard::SetUpdateTime(void)
{
	m_dwUpdateTime = ::GetTickCount();
}

ERRORCODE Clipboard::PasteFromOleDataObject(CPmwView* pView, UINT uFormat, COleDataObject* pDataObject, PPNT* pOrigin /*=NULL*/, BOOL fUpdateTime /*=FALSE*/)
{
	CPmwDoc* pDoc = pView->GetDocument();

	ERRORCODE error = ERRORCODE_DoesNotExist;

	if (fUpdateTime)
	{
		SetUpdateTime();
	}

	if (uFormat == 0
		 || uFormat == m_cfEmbeddedObject
		 || uFormat == m_cfEmbedSource
		 || uFormat == m_cfFileName
		 || uFormat == m_cfLinkSource)
	{
		TRACE("Paste Object format %u...\n", uFormat);
		
		error = ERRORCODE_None;

		pDoc->BeginWaitCursor();

		CPmwCntrItem* pItem = NULL;
		OleObject* pObject = NULL;

		TRY
		{
		/*
		// Get the object size before we create the item.
		*/
			PPNT Size;
			PPNT Offset;

			if (!GetObjectInfo(pDataObject, &Size, &Offset))
			{
				Offset.x = 0;
				Offset.y = 0;
				Size.x = 0;
				Size.y = 0;
			}

			// Create new item connected to this document.
			ASSERT_VALID(pDoc);
			pItem = new CPmwCntrItem(pDoc);
			ASSERT_VALID(pItem);

			// Initialize the item from the data object.
			if (!pItem->CreateFromData(pDataObject))
			{
				error = ERRORCODE_IllegalType;
				AfxThrowMemoryException();  // any exception will do
			}
			ASSERT_VALID(pItem);

			/* Get the creation extent of the object. */
			CSize czExtent;

			pItem->GetExtent(&czExtent);

			TRACE("Creation extent is (%d, %d)\n", czExtent.cx, czExtent.cy);

			if (Size.x == 0 && Size.y == 0)
			{
			/* Use the creation extent if we didn't get one at place time. */
				Size.x = scale_pcoord(czExtent.cx, PAGE_RESOLUTION, 2540);
				Size.y = scale_pcoord(czExtent.cy, PAGE_RESOLUTION, 2540);
			}

			// Create the object to hold this item.

			PMGDatabase* pDatabase = pDoc->get_database();

			OLEOBJECT_CREATE_STRUCT ocs;

			ocs.m_OriginalSize.x = czExtent.cx;
			ocs.m_OriginalSize.y = czExtent.cy;

			if (pOrigin == NULL)
			{
				ocs.m_Bound.x0 = 0;
				ocs.m_Bound.y0 = 0;
			}
			else
			{
				ocs.m_Bound.x0 = pOrigin->x;
				ocs.m_Bound.y0 = pOrigin->y;
			}

			ocs.m_Bound.x0 += Offset.x;
			ocs.m_Bound.y0 += Offset.y;
			ocs.m_Bound.x1 = ocs.m_Bound.x0 + Size.x;
			ocs.m_Bound.y1 = ocs.m_Bound.y0 + Size.y;
			ocs.m_pItem = pItem;
			
			pObject = (OleObject*)pDatabase->new_object(OBJECT_TYPE_OleObject, &ocs);
			if (pObject == NULL)
			{
				error = pDatabase->last_creation_error();
				AfxThrowMemoryException();
			}

			// Hook us the other way.
			pItem->Object(pObject);

			ASSERT_VALID(pItem);

			if (pOrigin == NULL)
			{
				pDoc->PasteObject(pObject, NULL, NULL, FALSE);
			}
			else
			{
				pDoc->append_object(pObject);
				pDoc->deselect_all();
				pDoc->select_object(pObject);
				pDoc->refresh_object(pObject);
				pDoc->SetModifiedFlag();
			}
		}
		CATCH(CException, e)
		{
			if (pItem != NULL)
			{
				ASSERT_VALID(pItem);
				pItem->Delete();
			}
			if (pObject != NULL)
			{
				delete pObject;
			}

			if (error == ERRORCODE_None)
			{
				error = ERRORCODE_Memory;
			}
		}
		END_CATCH

		pDoc->EndWaitCursor();
	}
	else if (uFormat == CF_HDROP)
	{
		HDROP hDrop;

		if ((hDrop = (HDROP)pDataObject->GetGlobalData(uFormat)) != NULL)
		{
			pDoc->PasteDropFiles(hDrop, pOrigin);
			error = ERRORCODE_None;
		}
	}
	else if (uFormat == CF_BITMAP)
	{
		// setup OLE structs for data acquisition
		STGMEDIUM stgmed;
		stgmed.tymed = TYMED_GDI;
		stgmed.pUnkForRelease = NULL;

		FORMATETC formatEtc;
		formatEtc.cfFormat = CF_BITMAP;
		formatEtc.ptd = NULL;
		formatEtc.dwAspect = DVASPECT_CONTENT;
		formatEtc.lindex = -1;
		formatEtc.tymed = TYMED_GDI;

		HBITMAP hBitmap = NULL;
		if (pDataObject->GetData(CF_BITMAP, &stgmed, &formatEtc))
		{
			hBitmap = stgmed.hBitmap;
			if (hBitmap)
			{
				char cbName[30];
				BuildPasteName(".BMP", cbName);

				// see if there's a palette
				HPALETTE hPalette = NULL;
				if (pDataObject->IsDataAvailable(CF_PALETTE))
				{
					formatEtc.cfFormat = CF_PALETTE;
					if (pDataObject->GetData(CF_PALETTE, &stgmed, &formatEtc))
						hPalette = (HPALETTE)stgmed.hBitmap;
				}

				GraphicObject* pObject = NULL;
				error = CreateBMPFromBitmap(cbName,
													 pDoc->get_database(),
													 hBitmap,
													 pObject,
													 hPalette);

				if (error == ERRORCODE_None && pObject != NULL)
				{
					pDoc->PasteObject(pObject, NULL, NULL, FALSE);
				}

				// clean up our act
				::DeleteObject(hBitmap);
				if (hPalette)
					::DeleteObject(hPalette);
			}
		}
	}
	else if (uFormat == kComponentFormat)
	{
		PBOX world;
		pDoc->get_panel_world(&world);

		PPNT point = { world.CenterX(), world.CenterY() };
		error = ComponentObject::PasteComponent( pDataObject, pDoc, point );
	}
	else
	{
	/* Get the data we want. */

		FORMATETC formatEtc;
		formatEtc.cfFormat = uFormat;
		formatEtc.ptd = NULL;
		formatEtc.dwAspect = DVASPECT_CONTENT;
		formatEtc.lindex = -1;
		formatEtc.tymed = TYMED_HGLOBAL|TYMED_MFPICT;

		HGLOBAL hData;

		if ((hData = pDataObject->GetGlobalData(uFormat, &formatEtc)) != NULL)
		{
			LPVOID pData;
			BOOL fLockData = uFormat != CF_BITMAP;

			if (fLockData && (pData = ::GlobalLock(hData)) == NULL)
			{
				error = ERRORCODE_Memory;
			}
			else
			{
			/* We got the data. */
				if (uFormat == CArtBrowserDialog::m_cfDataExchange)
				{
					CBrowserDataExchange* pExchange = (CBrowserDataExchange*)pData;
					CString csFilePath = ((LPCSTR)pExchange) + (int)pExchange->m_dwFilePathOffset;
					CString csFriendlyName = ((LPCSTR)pExchange) + (int)pExchange->m_dwFriendlyNameOffset;
					BYTE fCropCopyright = *(((LPCSTR)pExchange) + (int)pExchange->m_dwCropCopyrightOffset);
					pDoc->free_import_picture_name();

					GRAPHIC_CREATE_STRUCT* pGCS = pDoc->get_gcs();

					pGCS->m_csFileName = csFilePath;
					pGCS->m_csFriendlyName = csFriendlyName;
					pGCS->m_fSpecialCrop = fCropCopyright;
					pGCS->proof = TRUE;
					pGCS->embed_source = pDoc->GetPathManager()->FileIsRemovable(pGCS->m_csFileName);

					if (pView->CreateChosenPicture(pOrigin, FALSE))
					{
						error = ERRORCODE_None;
					}
				}
				else
				{
					// Now switch on the remaining formats.
					switch (uFormat)
					{
						case CF_METAFILEPICT:
						{
							char cbName[30];
							BuildPasteName(".WMF", cbName);

							GraphicObject* pObject = NULL;

							error = CreateWMFFromMetaFilePict(cbName,
																		 pDoc->get_database(),
																		 (METAFILEPICT*)pData,
																		 pObject);

							if (error == ERRORCODE_None && pObject != NULL)
							{
								pDoc->PasteObject(pObject, NULL, NULL, FALSE);
							}

							break;
						}
						case CF_DIB:
						{
							char cbName[30];
							BuildPasteName(".BMP", cbName);

							GraphicObject* pObject = NULL;

							error = CreateBMPFromDIB(cbName,
															 pDoc->get_database(),
															 (BITMAPINFO*)pData,
															 pObject);

							if (error == ERRORCODE_None && pObject != NULL)
							{
								pDoc->PasteObject(pObject, NULL, NULL, FALSE);
							}

							break;
						}
						default:
						{
							break;
						}
					}
				}
				if (fLockData)
				{
					::GlobalUnlock(hData);
				}
			}
		}
	}

	return error;
}

/*
// Paste objects from the clipboard.
*/

ERRORCODE Clipboard::PasteObjects(CPmwView* pView)
{
/*
// Make sure our selection is clear.
// In another life, this would be a call to delete_selects().
*/

	CPmwDoc* pDoc = pView->GetDocument();

	pDoc->PrePaste();

/* Now do the paste. */

	ERRORCODE error;

/* Try the database clipboard. */
/* If that fails, try the Windows clipboard. */

	if ((error = PasteObjectsFromDatabase(pDoc)) == ERRORCODE_None
			|| ((error = PasteObjectsFromClipboard(pView)) == ERRORCODE_None))
	{
		pDoc->PostPaste();
	}
	else
	{
		pDoc->AbortPaste();
	}
	return error;
}

/*
// Return the type of the MLS clipboard.
*/

CLIPBOARD_TYPE Clipboard::TypeOfMlsClipboard(BOOL is_open)
{
/* Check the local registry. */

	if (m_Type != CLIPBOARD_TYPE_unknown)
	{
		if (access_file(m_pszClipboardName) == 0)
		{
			return m_Type;
		}
		return CLIPBOARD_TYPE_unknown;
	}

/* Check the Windows registry. */

	if (m_uFilenameFormat != 0 && FormatIsAvailable(m_uFilenameFormat))
	{
		if (is_open || OpenClipboard())
		{
			HGLOBAL hData;

			if ((hData = ::GetClipboardData(m_uFilenameFormat)) != NULL)
			{
			/* Lock the data. */
				CLIPBOARD_FILENAME_PTR pData;
				if ((pData = (CLIPBOARD_FILENAME_PTR)::GlobalLock(hData)) != NULL)
				{
					CLIPBOARD_TYPE clip_type = (CLIPBOARD_TYPE)pData->m_Type;
					char name[_MAX_PATH];
					
					strcpy(name, pData->m_cbName);

					if (access_file(name) != 0)
					{
					/* File is no longer present! */
						clip_type = CLIPBOARD_TYPE_unknown;
					}
					::GlobalUnlock(hData);
					return clip_type;
				}
			}

			if (!is_open)
			{
			/* "If you open something, close it..." */
				CloseClipboard();
			}
		}
	}
	return CLIPBOARD_TYPE_unknown;
}

/*
// Render a format and place it in the (already open) clipboard.
*/

VOID Clipboard::RenderFormat(UINT uFormat, CUpdatingClipboard* pDialog)
{
	od("Render format %u...\r\n", uFormat);
#ifndef NO_PASTE_OBJECTS
	switch (uFormat)
	{
		case CF_METAFILEPICT:
		{
			RenderAsWMF(pDialog);
			break;
		}
		case CF_BITMAP:
		case CF_DIB:
		{
			RenderAsBitmap(pDialog);
			break;
		}
		case CF_PALETTE:
		{
			CPalette* pPalette = CreateIdentityPalette();
			if (pPalette != NULL)
			{
				HPALETTE hPalette = (HPALETTE)pPalette->Detach();
				::SetClipboardData(uFormat, hPalette);
				delete pPalette;
			}
			break;
		}
		default:
		{
			break;
		}
	}
#endif
}

/*
// Render all formats into the clipboard.
*/

VOID Clipboard::RenderAllFormats(VOID)
{
	od("Render all formats...\r\n");

	if (OpenClipboard())
	{
		UINT far *fp = (UINT far *)m_Formats.get_element(0);

	/* Create the cancel dialog. */

//		CWnd *main_wnd = AfxGetApp()->m_pMainWnd;
//		CBuildingPreview pdlg(main_wnd);
		HWND hActiveWnd;
		HWND hFocusWnd;

	/*
	// Throw up the cancel dialog.
	*/

		BOOL fWantDialog = m_fRenderOnExit
									&& (!m_fRenderedMetafile
										 || !m_fRenderedBitmap);

		CUpdatingClipboard* pdlg = NULL;

		if (fWantDialog)
		{
			pdlg = new CUpdatingClipboard;
			if (pdlg != NULL)
			{
				hActiveWnd = ::GetActiveWindow();
				hFocusWnd = ::GetFocus();
				pdlg->ShowWindow(SW_SHOW);
			}
//			main_wnd->EnableWindow(FALSE);    // Disable main window
		}

		for (SHORT i = 0; i < m_Formats.count(); i++)
		{
			UINT uFormat = *fp++;

			if (uFormat == m_uFilenameFormat)
			{
			/* Remove it from the clipboard. */
				::SetClipboardData(uFormat, NULL);
			}
			else
			{
				if (m_fRenderOnExit && !pdlg->abort_check(pdlg))
				{
				/* Attempt to render it. */
					switch (uFormat)
					{
						case CF_METAFILEPICT:
						{
							if (!m_fRenderedMetafile)
							{
								RenderFormat(uFormat, pdlg);
							}
							break;
						}
						case CF_BITMAP:
						case CF_DIB:
						{
							if (!m_fRenderedBitmap)
							{
								RenderFormat(uFormat, pdlg);
							}
							break;
						}
						case CF_PALETTE:
						{
							if (!m_fRenderedPalette)
							{
								RenderFormat(uFormat, pdlg);
							}
							break;
						}
						default:
						{
							break;
						}
					}
				}
			}
		}

	/*
	// Take down the dialog.
	*/

		if (pdlg != NULL)
		{
			::SetActiveWindow(hActiveWnd);
			::SetFocus(hFocusWnd);

			pdlg->DestroyWindow();
			delete pdlg;
//			pdlg = NULL;
		}

		CloseClipboard();
	}
}

#ifndef NO_PASTE_OBJECTS
/*
// Render the data as a METAFILEPICT clipboard record.
*/

ERRORCODE Clipboard::RenderAsWMF(CUpdatingClipboard* pDialog)
{
	if (pDialog != NULL && pDialog->abort_check(pDialog))
	{
	// Pick an error for canceled.
		return ERRORCODE_Busy;
	}

	AfxGetApp()->BeginWaitCursor();

	CClipboardOpenState OpenState;

	ERRORCODE error;

	if ((error = PrepareObjectPaste(&OpenState)) != ERRORCODE_None)
	{
		AfxGetApp()->EndWaitCursor();
		return error;
	}

	ObjectList* pList = OpenState.m_pList;

	// Create the MetaFilePict.
	HGLOBAL hMetaFilePict = CreateMetafilePictFromObjects(pList, pDialog);
	if (hMetaFilePict != NULL)
	{
		m_fRenderedMetafile = TRUE;		/* Don't try anymore. */
		::SetClipboardData(CF_METAFILEPICT, hMetaFilePict);
	}
	else
	{
		// Could not create.
		error = ERRORCODE_Busy;
	}

	AfxGetApp()->EndWaitCursor();

	return error;
}


//
// Create an object descriptor for the objects provided in the list.
//
HGLOBAL CreateObjectDescriptorFromObjects(CPtrList *pList)
{
	HGLOBAL hObjectDescriptor;

	if ((hObjectDescriptor = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, sizeof(OBJECTDESCRIPTOR))) != NULL)
	{
		BOOL fSuccess = FALSE;
		OBJECTDESCRIPTOR* pObjectDescriptor;

		if ((pObjectDescriptor = (OBJECTDESCRIPTOR*)::GlobalLock(hObjectDescriptor)) != NULL)
		{
			PBOX Bounds;

			// Get the bounding rectangle of the list of objects and the subsequent size.
			VERIFY(GetObjectBounds(pList, &Bounds));

			PCOORD DimX = Bounds.x1 - Bounds.x0;
			PCOORD DimY = Bounds.y1 - Bounds.y0;

			// Translate the dimensions into HIMETRIC (100ths of millimeters)
			int nPageResolution = 2540;
			int nResolution = nPageResolution;

			DimX = scale_pcoord(DimX, nResolution, PAGE_RESOLUTION);
			DimY = scale_pcoord(DimY, nResolution, PAGE_RESOLUTION);

			// Fill out the object descriptor.
			pObjectDescriptor->cbSize = sizeof( OBJECTDESCRIPTOR );	// Expand this size if strings are appended?
			pObjectDescriptor->clsid = CLSID_NULL;
			pObjectDescriptor->dwDrawAspect = DVASPECT_CONTENT;
			pObjectDescriptor->sizel.cx = (int)DimX;
			pObjectDescriptor->sizel.cy = (int)DimY;
			pObjectDescriptor->pointl.x = 0;
			pObjectDescriptor->pointl.y = 0;
			pObjectDescriptor->dwStatus = 0;
			pObjectDescriptor->dwFullUserTypeName = 0;
			pObjectDescriptor->dwSrcOfCopy = 0;

			// Release the handle from its pointer.
			::GlobalUnlock(hObjectDescriptor);
		}
		if (!fSuccess)
		{
			::GlobalFree(hObjectDescriptor);
			hObjectDescriptor = NULL;
		}
	}

	return hObjectDescriptor;
}


//
// Create a METAFILEPICT from the list of objects passed.
//

HGLOBAL CreateMetafilePictFromObjects(ObjectList* pList, CUpdatingClipboard* pDialog /*=NULL*/)
{
	CPtrList LocalList;

	// Copy the objects into a generic list type.
	for (	PMGPageObjectPtr pObject = (PMGPageObjectPtr)pList->first_object();
			pObject != NULL;
			pObject = (PMGPageObjectPtr)pObject->next_object() )
	{
		LocalList.AddTail(pObject);
	}

	// Call the main creation method.
	return CreateMetafilePictFromObjects( &LocalList, pDialog );
}

HGLOBAL CreateMetafilePictFromObjects(CPtrList* pList, CUpdatingClipboard* pDialog /*=NULL*/)
{
	HGLOBAL hMetaFilePict;

	if ((hMetaFilePict = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, sizeof(METAFILEPICT))) != NULL)
	{
		BOOL fSuccess = FALSE;
		METAFILEPICT* pMetaFilePict;

		if ((pMetaFilePict = (METAFILEPICT*)::GlobalLock(hMetaFilePict)) != NULL)
		{
			PBOX Bounds;

			VERIFY(GetObjectBounds(pList, &Bounds));

			PCOORD DimX = Bounds.x1 - Bounds.x0;
			PCOORD DimY = Bounds.y1 - Bounds.y0;

		// Our map mode will MM_ANISOTROPIC (units are HIMETRIC).

			pMetaFilePict->mm = MM_ANISOTROPIC;
			int nPageResolution = 2540;
			int nResolution = nPageResolution;

			DimX = scale_pcoord(DimX, nResolution, PAGE_RESOLUTION);
			DimY = scale_pcoord(DimY, nResolution, PAGE_RESOLUTION);

		/*
		// Force the dimensions to be something reasonable.
		*/

			BOOL fAbort = FALSE;

			while (!fAbort && (DimX > 32000 || DimY > 32000))
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

			pMetaFilePict->xExt = (int)DimX;
			pMetaFilePict->yExt = (int)DimY;

		/*
		// Force the dimensions to be something reasonable for generation.
		*/

			while (!fAbort && (DimX > 2048 || DimY > 2048))
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

			CMetaFileDC dc;

//			if (dc.Create("c:\\tmp\\test.wmf"))
			if (dc.Create(NULL))
			{
				int nSave = dc.SaveDC();
				dc.SetMapMode(MM_ANISOTROPIC);
				dc.SetWindowOrg(0, 0);
				dc.SetWindowExt((int)DimX, (int)DimY);

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
					::SetWindowExtEx(hAttribDC, (int)DimX, (int)DimY, NULL);
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

					HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();

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

					rc.scaled_source_x0 =  Bounds.x0*rc.x_resolution;
					rc.scaled_source_y0 =  Bounds.y0*rc.y_resolution;

					rc.destination_rect.left =
						rc.destination_rect.top =
						rc.destination_x0 =
						rc.destination_y0 = 0;

					rc.destination_rect.right = (int)DimX;
					rc.destination_rect.bottom = (int)DimY;

					rc.clip_rect = rc.destination_rect;

					rc.outline_gamma_curve =
						rc.bitmap_gamma_curve = screen_gamma_curve;

					rc.source_pbox = Bounds;

	//				od("dims:%ld, %ld\r\n", dims);

				/* Mono device. */

					rc.is_thumbnail = TRUE;
					rc.is_metafile = TRUE;			/* Yes, this is a metafile. */

					if (pDialog != NULL)
					{
					/* Install the abort interrupt check. */

						rc.set_check_interrupt(pDialog->abort_check);
						rc.terminate_interrupts = TRUE;
					}

					REFRESH_EXTENT re;

					/* Build the refresh extent. */

					re.extent = rc.source_pbox;
					re.refresh_type = REFRESH_ALL;
					re.my_object = NULL;
					re.update_object = NULL;
					re.update_state = NULL;

					fSuccess = !DrawObjects(pList,
													&re,
													&rc);

#if 0
/*
/ Don't do this. It doesn't make sense with a metafile DC.
// hOldPal will be 1.
*/
					if (hOldPal != NULL)
					{
						SelectPalette(rc.destination_hdc, hOldPal, FALSE);
					}
#endif
					if (hOldAttribPal != NULL)
					{
						SelectPalette(hAttribDC, hOldAttribPal, FALSE);
					}

					::ReleaseDC(NULL, hAttribDC);
				}

				dc.RestoreDC(nSave);

				pMetaFilePict->hMF = dc.Close();

				if (!fSuccess)
				{
					::DeleteMetaFile(pMetaFilePict->hMF);
					pMetaFilePict->hMF = NULL;
				}
			}
			::GlobalUnlock(hMetaFilePict);
		}
		if (!fSuccess)
		{
			::GlobalFree(hMetaFilePict);
			hMetaFilePict = NULL;
		}
	}

	return hMetaFilePict;
}


//
// Create a METAFILEPICT from the list of objects passed.
//

HGLOBAL CreateEnhMetafilePictFromObjects(CPtrList* pList, CUpdatingClipboard* pDialog /*=NULL*/)
{
	HENHMETAFILE hMetaFilePict = NULL;

	BOOL fSuccess = FALSE;
	PBOX Bounds;

	VERIFY(GetObjectBounds(pList, &Bounds));

	PCOORD DimX = Bounds.x1 - Bounds.x0;
	PCOORD DimY = Bounds.y1 - Bounds.y0;

// Our map mode will MM_ANISOTROPIC (units are HIMETRIC).

	int nPageResolution = 2540;	// Conversion from inches to 100ths of millimeters (25.4 mmpi)
	int nResolution = nPageResolution;

	DimX = scale_pcoord(DimX, nResolution, PAGE_RESOLUTION);
	DimY = scale_pcoord(DimY, nResolution, PAGE_RESOLUTION);

	PCOORD ScaledX = DimX;
	PCOORD ScaledY = DimY;

	CDC refDC;
	refDC.CreateCompatibleDC( NULL );
	// Fix up the viewport conversion for correct sizing.
	double fDpmmX = (double)refDC.GetDeviceCaps(HORZRES) / (double)refDC.GetDeviceCaps(HORZSIZE);
	double fDpmmY = (double)refDC.GetDeviceCaps(VERTRES) / (double)refDC.GetDeviceCaps(VERTSIZE);

	CRect rcBounds( 0, 0, DimX, DimY );

/*
// Force the dimensions to be something reasonable.
*/

	BOOL fAbort = FALSE;

	while (!fAbort && (DimX > 2048 || DimY > 2048))
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

	CMetaFileDC dc;
	if (dc.CreateEnhanced(NULL, NULL, &rcBounds, NULL))
	{
		int nSave = dc.SaveDC();
		dc.SetMapMode(MM_ANISOTROPIC);
		dc.SetWindowOrg(0,0);
		dc.SetWindowExt((int)DimX, (int)DimY);

		dc.SetTextColor(RGB(0, 0, 0));
		dc.SetBkColor(RGB(255, 255, 255));
		dc.SetBkMode(OPAQUE);
		dc.SetROP2(R2_COPYPEN);

		dc.SetViewportExt( (ScaledX / 100.0) * fDpmmX, (ScaledY / 100.0) * fDpmmY );

		HDC hAttribDC = ::GetDC(NULL);
		if (hAttribDC != NULL)
		{
			// This is useful for OLE objects.
			::SetMapMode(hAttribDC, MM_ANISOTROPIC);
			::SetWindowOrgEx(hAttribDC, 0, 0, NULL);
			::SetWindowExtEx(hAttribDC, (int)DimX, (int)DimY, NULL);
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

			HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();

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

//			od("num:%d, denom:%d\r\n", rc.GetScaleNumerator, rc.GetScaleDenominator());

			rc.scaled_source_x0 =  Bounds.x0*rc.x_resolution;
			rc.scaled_source_y0 =  Bounds.y0*rc.y_resolution;

			rc.destination_rect.left =
				rc.destination_rect.top =
				rc.destination_x0 =
				rc.destination_y0 = 0;

			rc.destination_rect.right = (int)DimX;
			rc.destination_rect.bottom = (int)DimY;

			rc.clip_rect = rc.destination_rect;

			rc.outline_gamma_curve =
				rc.bitmap_gamma_curve = screen_gamma_curve;

			rc.source_pbox = Bounds;

//			od("dims:%ld, %ld\r\n", dims);

		/* Mono device. */

			rc.is_thumbnail = TRUE;
			rc.is_metafile = TRUE;			/* Yes, this is a metafile. */

			if (pDialog != NULL)
			{
			/* Install the abort interrupt check. */

				rc.set_check_interrupt(pDialog->abort_check);
				rc.terminate_interrupts = TRUE;
			}

			REFRESH_EXTENT re;

			/* Build the refresh extent. */

			re.extent = rc.source_pbox;
			re.refresh_type = REFRESH_ALL;
			re.my_object = NULL;
			re.update_object = NULL;
			re.update_state = NULL;

			fSuccess = !DrawObjects(pList,
											&re,
											&rc);

#if 0
/*
/ Don't do this. It doesn't make sense with a metafile DC.
// hOldPal will be 1.
*/
			if (hOldPal != NULL)
			{
				SelectPalette(rc.destination_hdc, hOldPal, FALSE);
			}
#endif
			if (hOldAttribPal != NULL)
			{
				SelectPalette(hAttribDC, hOldAttribPal, FALSE);
			}

			::ReleaseDC(NULL, hAttribDC);
		}

		dc.RestoreDC(nSave);

		hMetaFilePict = dc.CloseEnhanced();

		if (!fSuccess)
		{
			TRACE( "Failure to properly render out an enhanced metafile.\n" );
			ASSERT( 0 );
			::DeleteEnhMetaFile(hMetaFilePict);
			hMetaFilePict = NULL;
		}
	}

	return hMetaFilePict;
}


/*
 *	This routine renders the list of objects to a bitmap.
 */
HGLOBAL RenderObjectsAsBitmap(CPtrList* pList, CUpdatingClipboard* pDialog /*=NULL*/)
{
	HBITMAP hBitmap = NULL;

	if (pDialog != NULL && pDialog->abort_check(pDialog))
	{
	// Pick an error for canceled.
		return hBitmap;
	}

	AfxGetApp()->BeginWaitCursor();

/*
// Get a screen DC.
*/

	HDC hScreenDC = ::GetDC(NULL);

	if (hScreenDC != NULL)
	{
		CDC ScreenDC;
		ScreenDC.Attach(hScreenDC);

		PBOX Bounds;

		VERIFY(GetObjectBounds(pList, &Bounds));

		PCOORD DimX = Bounds.x1 - Bounds.x0;
		PCOORD DimY = Bounds.y1 - Bounds.y0;

		// Using screen DPI keeps the resulting image on the transfer size at
		// the same size it was within PM.
		int nXResolution = ScreenDC.GetDeviceCaps(LOGPIXELSX);
		int nYResolution = ScreenDC.GetDeviceCaps(LOGPIXELSY);
		// Render to 200 dpi, this should allow for better print in the app the
		// image is transferred to although it causes the initial size to be too large.
//		int nXResolution = 200;
//		int nYResolution = 200;

		DimX = scale_pcoord(DimX, nXResolution, PAGE_RESOLUTION);
		DimY = scale_pcoord(DimY, nYResolution, PAGE_RESOLUTION);

	/*
	// Force the dimensions to be something reasonable for generation.
	*/

		BOOL fAbort = FALSE;

		while (!fAbort && (DimX > 2048 || DimY > 2048))
		{
			DimX >>= 1;
			DimY >>= 1;
			nXResolution >>= 1;
			nYResolution >>= 1;

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
			if (nXResolution == 0)
			{
				nXResolution = 1;
				fAbort = TRUE;
			}
			if (nYResolution == 0)
			{
				nYResolution = 1;
				fAbort = TRUE;
			}
		}

	/* Allocate the Bitmap structure. */

		CBitmap Bitmap;

		if (Bitmap.CreateCompatibleBitmap(&ScreenDC, (int)DimX, (int)DimY))
		{
			CDC dc;

			if (dc.CreateCompatibleDC(&ScreenDC))
			{
				CBitmap* pOldBitmap;

				if ((pOldBitmap = dc.SelectObject(&Bitmap)) != NULL)
				{
					HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();

					HDC hAttribDC = hScreenDC;

					RedisplayContext rc;

					rc.destination_hdc = dc.GetSafeHdc();
					rc.AttributeDC(hAttribDC);

					rc.hwnd = NULL;
					rc.set_info(hAttribDC);

					// Establish the palettes if necessary.

					HPALETTE hOldPal = NULL;
					HPALETTE hOldAttribPal = NULL;

					if (rc.info.type == RASTER_TYPE_PALETTE)
					{
						hOldPal = SelectPalette(rc.destination_hdc, hOurPal, FALSE);
						RealizePalette(rc.destination_hdc);
						hOldAttribPal = SelectPalette(hAttribDC, hOurPal, FALSE);
						RealizePalette(hAttribDC);
					}

					rc.x_resolution = nXResolution;
					rc.y_resolution = nYResolution;

	//				od("num:%d, denom:%d\r\n", rc.GetScaleNumerator, rc.GetScaleDenominator());

					rc.scaled_source_x0 =  Bounds.x0*rc.x_resolution;
					rc.scaled_source_y0 =  Bounds.y0*rc.y_resolution;

					rc.destination_rect.left =
						rc.destination_rect.top =
						rc.destination_x0 =
						rc.destination_y0 = 0;

					rc.destination_rect.right = (int)DimX;
					rc.destination_rect.bottom = (int)DimY;

					rc.clip_rect = rc.destination_rect;

					rc.outline_gamma_curve =
						rc.bitmap_gamma_curve = screen_gamma_curve;

					rc.source_pbox = Bounds;

	//				od("dims:%ld, %ld\r\n", dims);

					rc.is_thumbnail = TRUE;

					if (pDialog != NULL)
					{
					/* Install the abort interrupt check. */

						rc.set_check_interrupt(pDialog->abort_check);
						rc.terminate_interrupts = TRUE;
					}

					REFRESH_EXTENT re;

					/* Start everything out white. */

					dc.PatBlt(0, 0, (int)DimX, (int)DimY, WHITENESS);

					/* Build the refresh extent. */

					re.extent = rc.source_pbox;
					re.refresh_type = REFRESH_ALL;
					re.my_object = NULL;
					re.update_object = NULL;
					re.update_state = NULL;

					BOOL fSuccess = !DrawObjects(pList,
														  &re,
														  &rc);

					if (hOldPal != NULL)
					{
						SelectPalette(rc.destination_hdc, hOldPal, FALSE);
					}
					if (hOldAttribPal != NULL)
					{
						SelectPalette(hAttribDC, hOldAttribPal, FALSE);
					}

					dc.SelectObject(pOldBitmap);

					hBitmap = (HBITMAP)Bitmap.Detach();

					if (!fSuccess)
					{
					/* Get rid of what got aborted. */
						::DeleteObject(hBitmap);
						hBitmap = NULL;
					}
				}
				// Get rid of the DC we created.
				dc.DeleteDC();
			}
		}

		::ReleaseDC(NULL, (HDC)ScreenDC.Detach());
	}

	AfxGetApp()->EndWaitCursor();

	return hBitmap;
}


/*
 *	This routine renders the list of objects to a DIB.
 */
HGLOBAL RenderObjectsAsDIB(CPtrList* pList, CUpdatingClipboard* pDialog /*=NULL*/)
{
	HGLOBAL hDIB = NULL;
	HBITMAP hBitmap = NULL;

	if (pDialog != NULL && pDialog->abort_check(pDialog))
	{
	// Pick an error for canceled.
		return hDIB;
	}

	AfxGetApp()->BeginWaitCursor();

/*
// Get a screen DC.
*/

	HDC hScreenDC = ::GetDC(NULL);

	if (hScreenDC != NULL)
	{
		CDC ScreenDC;
		ScreenDC.Attach(hScreenDC);

		PBOX Bounds;

		VERIFY(GetObjectBounds(pList, &Bounds));

		PCOORD DimX = Bounds.x1 - Bounds.x0;
		PCOORD DimY = Bounds.y1 - Bounds.y0;

		// Using screen DPI keeps the resulting image on the transfer size at
		// the same size it was within PM.
		int nXResolution = ScreenDC.GetDeviceCaps(LOGPIXELSX);
		int nYResolution = ScreenDC.GetDeviceCaps(LOGPIXELSY);
		// Render to 200 dpi, this should allow for better print in the app the
		// image is transferred to although it causes the initial size to be too large.
//		int nXResolution = 200;
//		int nYResolution = 200;

		DimX = scale_pcoord(DimX, nXResolution, PAGE_RESOLUTION);
		DimY = scale_pcoord(DimY, nYResolution, PAGE_RESOLUTION);

	/*
	// Force the dimensions to be something reasonable for generation.
	*/

		BOOL fAbort = FALSE;

		while (!fAbort && (DimX > 2048 || DimY > 2048))
		{
			DimX >>= 1;
			DimY >>= 1;
			nXResolution >>= 1;
			nYResolution >>= 1;

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
			if (nXResolution == 0)
			{
				nXResolution = 1;
				fAbort = TRUE;
			}
			if (nYResolution == 0)
			{
				nYResolution = 1;
				fAbort = TRUE;
			}
		}

	/* Allocate the Bitmap structure. */

		CBitmap Bitmap;

		if (Bitmap.CreateCompatibleBitmap(&ScreenDC, (int)DimX, (int)DimY))
		{
			CDC dc;

			if (dc.CreateCompatibleDC(&ScreenDC))
			{
				CBitmap* pOldBitmap;

				if ((pOldBitmap = dc.SelectObject(&Bitmap)) != NULL)
				{
					HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();

					HDC hAttribDC = hScreenDC;

					RedisplayContext rc;

					rc.destination_hdc = dc.GetSafeHdc();
					rc.AttributeDC(hAttribDC);

					rc.hwnd = NULL;
					rc.set_info(hAttribDC);

					// Establish the palettes if necessary.

					HPALETTE hOldPal = NULL;
					HPALETTE hOldAttribPal = NULL;

					if (rc.info.type == RASTER_TYPE_PALETTE)
					{
						hOldPal = SelectPalette(rc.destination_hdc, hOurPal, FALSE);
						RealizePalette(rc.destination_hdc);
						hOldAttribPal = SelectPalette(hAttribDC, hOurPal, FALSE);
						RealizePalette(hAttribDC);
					}

					rc.x_resolution = nXResolution;
					rc.y_resolution = nYResolution;

	//				od("num:%d, denom:%d\r\n", rc.GetScaleNumerator, rc.GetScaleDenominator());

					rc.scaled_source_x0 =  Bounds.x0*rc.x_resolution;
					rc.scaled_source_y0 =  Bounds.y0*rc.y_resolution;

					rc.destination_rect.left =
						rc.destination_rect.top =
						rc.destination_x0 =
						rc.destination_y0 = 0;

					rc.destination_rect.right = (int)DimX;
					rc.destination_rect.bottom = (int)DimY;

					rc.clip_rect = rc.destination_rect;

					rc.outline_gamma_curve =
						rc.bitmap_gamma_curve = screen_gamma_curve;

					rc.source_pbox = Bounds;

	//				od("dims:%ld, %ld\r\n", dims);

					rc.is_thumbnail = TRUE;

					if (pDialog != NULL)
					{
					/* Install the abort interrupt check. */

						rc.set_check_interrupt(pDialog->abort_check);
						rc.terminate_interrupts = TRUE;
					}

					REFRESH_EXTENT re;

					/* Start everything out white. */

					dc.PatBlt(0, 0, (int)DimX, (int)DimY, WHITENESS);

					/* Build the refresh extent. */

					re.extent = rc.source_pbox;
					re.refresh_type = REFRESH_ALL;
					re.my_object = NULL;
					re.update_object = NULL;
					re.update_state = NULL;

					BOOL fSuccess = !DrawObjects(pList,
														  &re,
														  &rc);

					if (hOldPal != NULL)
					{
						SelectPalette(rc.destination_hdc, hOldPal, FALSE);
					}
					if (hOldAttribPal != NULL)
					{
						SelectPalette(hAttribDC, hOldAttribPal, FALSE);
					}

					dc.SelectObject(pOldBitmap);

					hBitmap = (HBITMAP)Bitmap.Detach();

					if (fSuccess)
					{
						// Convert the BMP generated into a DIB.
						if (::BitmapToDIB(hBitmap, hOurPal, &hDIB) != ERRORCODE_None)
						{
							// We had an error, so make sure the DIB is empty.
							hDIB = NULL;
							ASSERT( 0 );	// This shouldn't happen, so flag it.
						}
					}
				}
				// Get rid of the DC we created.
				dc.DeleteDC();
			}

			// Clean up the BMP based bitmap, we only want the DIB.
			::DeleteObject( hBitmap );
			hBitmap = NULL;
		}

		::ReleaseDC(NULL, (HDC)ScreenDC.Detach());
	}

	AfxGetApp()->EndWaitCursor();

	return hDIB;
}


/*
// Render the data as a BITMAP clipboard record.
*/

ERRORCODE Clipboard::RenderAsBitmap(CUpdatingClipboard* pDialog)
{
	if (pDialog != NULL && pDialog->abort_check(pDialog))
	{
	// Pick an error for canceled.
		return ERRORCODE_Busy;
	}

	AfxGetApp()->BeginWaitCursor();

	CClipboardOpenState OpenState;

	ERRORCODE error;

	if ((error = PrepareObjectPaste(&OpenState)) != ERRORCODE_None)
	{
		AfxGetApp()->EndWaitCursor();
		return error;
	}

	ObjectList* pList = OpenState.m_pList;

/*
// Get a screen DC.
*/

	HDC hScreenDC = ::GetDC(NULL);

	if (hScreenDC != NULL)
	{
		CDC ScreenDC;
		ScreenDC.Attach(hScreenDC);

		PBOX Bounds;

		VERIFY(GetObjectBounds(pList, &Bounds));

		PCOORD DimX = Bounds.x1 - Bounds.x0;
		PCOORD DimY = Bounds.y1 - Bounds.y0;

		int nXResolution = ScreenDC.GetDeviceCaps(LOGPIXELSX);
		int nYResolution = ScreenDC.GetDeviceCaps(LOGPIXELSY);

		DimX = scale_pcoord(DimX, nXResolution, PAGE_RESOLUTION);
		DimY = scale_pcoord(DimY, nYResolution, PAGE_RESOLUTION);

	/*
	// Force the dimensions to be something reasonable for generation.
	*/

		BOOL fAbort = FALSE;

		while (!fAbort && (DimX > 2048 || DimY > 2048))
		{
			DimX >>= 1;
			DimY >>= 1;
			nXResolution >>= 1;
			nYResolution >>= 1;

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
			if (nXResolution == 0)
			{
				nXResolution = 1;
				fAbort = TRUE;
			}
			if (nYResolution == 0)
			{
				nYResolution = 1;
				fAbort = TRUE;
			}
		}

	/* Allocate the Bitmap structure. */

		CBitmap Bitmap;

		if (Bitmap.CreateCompatibleBitmap(&ScreenDC, (int)DimX, (int)DimY))
		{
			CDC dc;

			if (dc.CreateCompatibleDC(&ScreenDC))
			{
				CBitmap* pOldBitmap;

				if ((pOldBitmap = dc.SelectObject(&Bitmap)) != NULL)
				{
	//				dc.SetMapMode(MM_ANISOTROPIC);
	//				dc.SetWindowOrg(0, 0);
	//				dc.SetWindowExt((int)DimX, (int)DimY);

	//				dc.SetTextColor(RGB(0, 0, 0));
	//				dc.SetBkColor(RGB(255, 255, 255));
	//				dc.SetBkMode(OPAQUE);
	//				dc.SetROP2(R2_COPYPEN);
					
					HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();

					HDC hAttribDC = hScreenDC;

					RedisplayContext rc;

					rc.destination_hdc = dc.GetSafeHdc();
					rc.AttributeDC(hAttribDC);

					rc.hwnd = NULL;
					rc.set_info(hAttribDC);

					// Establish the palettes if necessary.

					HPALETTE hOldPal = NULL;
					HPALETTE hOldAttribPal = NULL;

					if (rc.info.type == RASTER_TYPE_PALETTE)
					{
						hOldPal = SelectPalette(rc.destination_hdc, hOurPal, FALSE);
						RealizePalette(rc.destination_hdc);
						hOldAttribPal = SelectPalette(hAttribDC, hOurPal, FALSE);
						RealizePalette(hAttribDC);
					}

					rc.x_resolution = nXResolution;
					rc.y_resolution = nYResolution;

	//				od("num:%d, denom:%d\r\n", rc.GetScaleNumerator, rc.GetScaleDenominator());

					rc.scaled_source_x0 =  Bounds.x0*rc.x_resolution;
					rc.scaled_source_y0 =  Bounds.y0*rc.y_resolution;

					rc.destination_rect.left =
						rc.destination_rect.top =
						rc.destination_x0 =
						rc.destination_y0 = 0;

					rc.destination_rect.right = (int)DimX;
					rc.destination_rect.bottom = (int)DimY;

					rc.clip_rect = rc.destination_rect;

					rc.outline_gamma_curve =
						rc.bitmap_gamma_curve = screen_gamma_curve;

					rc.source_pbox = Bounds;

	//				od("dims:%ld, %ld\r\n", dims);

					rc.is_thumbnail = TRUE;

					if (pDialog != NULL)
					{
					/* Install the abort interrupt check. */

						rc.set_check_interrupt(pDialog->abort_check);
						rc.terminate_interrupts = TRUE;
					}

					REFRESH_EXTENT re;

					/* Start everything out white. */

					dc.PatBlt(0, 0, (int)DimX, (int)DimY, WHITENESS);

					/* Build the refresh extent. */

					re.extent = rc.source_pbox;
					re.refresh_type = REFRESH_ALL;
					re.my_object = NULL;
					re.update_object = NULL;
					re.update_state = NULL;

					BOOL fSuccess = !DrawObjects(pList,
														  &re,
														  &rc);

					if (hOldPal != NULL)
					{
						SelectPalette(rc.destination_hdc, hOldPal, FALSE);
					}
					if (hOldAttribPal != NULL)
					{
						SelectPalette(hAttribDC, hOldAttribPal, FALSE);
					}

					dc.SelectObject(pOldBitmap);

					HBITMAP hBitmap = (HBITMAP)Bitmap.Detach();

					m_fRenderedBitmap = TRUE;			/* Don't try anymore. */

					if (fSuccess)
					{
						// Generate the DIB from the bitmap.

						HGLOBAL hDIB;
						if (::BitmapToDIB(hBitmap, hOurPal, &hDIB) == ERRORCODE_None)
						{
							::SetClipboardData(CF_DIB, hDIB);
						}

						::SetClipboardData(CF_BITMAP, hBitmap);
					}
					else
					{
					/* Get rid of what got aborted. */
						::DeleteObject(hBitmap);
						error = ERRORCODE_Busy;
					}
				}
				// Get rid of the DC we created.
				dc.DeleteDC();
			}
		}

		::ReleaseDC(NULL, (HDC)ScreenDC.Detach());
	}

	AfxGetApp()->EndWaitCursor();

	return error;
}

/*
// Utility to get object info from a data object.
*/

BOOL GetObjectInfo(COleDataObject* pDataObject, PPNT* pSize, PPNT* pOffset)
{
   BOOL fRead = FALSE;
	ASSERT(pSize != NULL);

	OBJECTDESCRIPTOR ObjDesc;
	memset(&ObjDesc, 0, sizeof(ObjDesc));

	// Try to get the data in File format.
   // This will also get global data if available.

   CFile* pFile = pDataObject->GetFileData(Clipboard::m_cfObjectDescriptor);
	if (pFile != NULL)
	{
		pFile->Read(&ObjDesc, sizeof(ObjDesc));
		fRead = TRUE;
		pFile->Close();
		delete pFile;
	}

	// Scale coordinates to PAGE_RESOLUTION.
   pSize->x = scale_pcoord(ObjDesc.sizel.cx, PAGE_RESOLUTION, 2540);
	pSize->y = scale_pcoord(ObjDesc.sizel.cy, PAGE_RESOLUTION, 2540);
	if (pOffset != NULL)
	{
		pOffset->x = scale_pcoord(ObjDesc.pointl.x, PAGE_RESOLUTION, 2540);
		pOffset->y = scale_pcoord(ObjDesc.pointl.y, PAGE_RESOLUTION, 2540);
	}

	return fRead;
}

#endif

