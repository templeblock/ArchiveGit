/*
// Text insertion point "object" routines.
//
// $Header: /PM8/App/CTXP.CPP 1     3/03/99 6:04p Gbeddow $
//
// $Log: /PM8/App/CTXP.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 8     3/01/99 7:13p Psasse
// Support for Vertical Banner Text
// 
// 7     2/17/99 5:17p Cschende
// fixed selection across linked text boxes
// 
// 6     2/16/99 6:11p Cschende
// fixed text wrap not wrapping correctly with a drop cap bug. fixed
// hanging bug when text box is narrower then the drop cap character
//  
// 
// 5     2/15/99 10:01a Cschende
// Initial rewrite of ComputeSelectionBounds
// 
// 4     7/24/98 1:24p Johno
// Added test / fix in  CTxp::CommonInit() for a cursor position beyond
// the text. Can be caused when spell checking with the cursor at the end
// of the text results in shorter text.
// 
// 3     7/23/98 9:54p Johno
// Diagnostic ASSERTS, kludge fix in CTxp::CalcFrame()
// 
// 2     12/04/97 2:24p Hforman
// CArray -> CVarArray
// 
//    Rev 1.0   14 Aug 1997 15:19:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:52   Fred
// Initial revision.
// 
//    Rev 1.36   16 Jul 1997 13:19:44   Fred
// Fixed text selection bugs
// 
//    Rev 1.35   30 May 1997 11:26:32   Fred
// Initializes pointer to null
// 
//    Rev 1.34   09 Apr 1997 14:50:42   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.33   12 Mar 1997 16:52:16   Fred
// Up/Down arrows now work in warped and rotated text boxes
// 
//    Rev 1.32   12 Mar 1997 14:54:38   Fred
// Changes for flipped warped and rotated text
// 
//    Rev 1.31   07 Feb 1997 08:44:20   Fred
// Passes GetCharacterFormatting() to AdvanceWidth()
// 
//    Rev 1.30   06 Jan 1997 15:44:36   Jay
// Now initializes m_lCursorFrame.
// 
//    Rev 1.29   24 Dec 1996 12:07:18   Jay
// Text import.
// 
//    Rev 1.28   06 Dec 1996 10:42:00   Jay
// Uses the cursor's frame record if different from the "bound" one.
// 
//    Rev 1.27   08 Nov 1996 08:57:38   Jay
// Removed 16-bit compress.obj
// 
//    Rev 1.26   06 Aug 1996 10:43:06   Jay
// Changes to CTxp
// 
//    Rev 1.25   02 Aug 1996 17:13:06   Jay
// Sets text record as well in NewFrame().
// 
//    Rev 1.24   01 Aug 1996 14:04:14   Jay
// 
//    Rev 1.23   31 Jul 1996 18:21:06   Jay
// Fixes, etc!
// 
//    Rev 1.22   31 Jul 1996 11:58:58   Fred
// 16-bit compatibility
// 
//    Rev 1.21   22 Jul 1996 18:06:32   Jay
// SelectText can now not set DesiredX
// 
//    Rev 1.20   22 Jul 1996 13:06:10   Jay
// New text select mechanism
// 
//    Rev 1.19   17 Jul 1996 14:10:08   Jay
// Database bookkeeping
// 
//    Rev 1.18   15 Jul 1996 11:34:24   Jay
//  
// 
//    Rev 1.17   11 Jul 1996 11:35:22   Jay
//  
// 
//    Rev 1.16   09 Jul 1996 15:37:12   Jay
//  
// 
//    Rev 1.15   08 Jul 1996 17:51:24   Jay
// Linked frames, etc.
// 
//    Rev 1.14   03 Jul 1996 14:30:24   Jay
//  
// 
//    Rev 1.13   27 Jun 1996 17:20:32   Jay
//  
// 
//    Rev 1.12   26 Jun 1996 14:22:22   Jay
//  
// 
//    Rev 1.11   25 Jun 1996 13:13:14   Jay
//  
// 
//    Rev 1.10   25 Jun 1996 10:25:30   Jay
//  
// 
//    Rev 1.9   24 Jun 1996 18:06:06   Jay
//  
// 
//    Rev 1.8   21 Jun 1996 17:49:36   Jay
//  
// 
//    Rev 1.7   21 Jun 1996 14:30:20   Jay
// Stretch text, etc.
// 
//    Rev 1.6   20 Jun 1996 14:59:44   Jay
//  
// 
//    Rev 1.5   19 Jun 1996 17:32:08   Jay
//  
// 
//    Rev 1.4   17 Jun 1996 12:01:24   Jay
//  
// 
//    Rev 1.3   14 Jun 1996 08:57:08   Jay
//  
// 
//    Rev 1.2   13 Jun 1996 15:52:58   Jay
//  
// 
//    Rev 1.1   05 Jun 1996 12:10:56   Jay
// LeftWord() and RightWord().
// 
//    Rev 1.0   04 Jun 1996 17:37:48   Jay
//  
*/

#include "stdafx.h"
#include "pmgdb.h"
#include "ctxp.h"
#include "textrec.h"
#include "framerec.h"
#include "frameobj.h"
#include "pagerec.h"
#include "textflow.h"
#include "fontsrv.h"
#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/*
// The constructor for a txp.
*/

CTxp::CTxp(PMGDatabase* pDatabase)
	:	m_Style(pDatabase, TRUE),
		m_SelectStyle(pDatabase, TRUE),
		m_SelectBounds(sizeof(PBOX)),
		m_SelectFrames(sizeof(CFrameObject*))
{
	m_pDatabase = pDatabase;
	Reset();
}

/*
// Reset the txp to not reference anything.
// This will get rid of all face references as well.
*/

void CTxp::Reset(BOOL fSaveFace /*=FALSE*/)
{
	m_pObject = NULL;
	m_lIndex = -1;
	m_lFrameRecord = 0;
	m_lTextRecord = 0;
	if (fSaveFace)
	{
	/* Just reset in case of stretching fudges. */
		m_Style.Size(m_Style.BaseSize());
		m_Style.Expansion(MakeFixed(1));
	}
	else
	{
		// Release the face for the style.
		m_Style.Reset();
	}
	// Release the face for the select style.
	m_SelectStyle.Reset();
	InvalidateSelectBounds();
	m_fWantFlowText = TRUE;
   m_lCursorFrame = 0;
}

/*
// Say whether a txp is valid or not.
*/

BOOL CTxp::Valid(void)
{
	return (m_lFrameRecord != 0) && (m_lIndex >= 0);
}

/*
// Initialize an insertion point.
*/

void CTxp::Init(CFrameObject* pObject, CHARACTER_INDEX lIndex, TOGGLE_FUNCTION pToggleFunction /*=NULL*/, LPVOID pToggleData /*=NULL*/)
{
	CommonInit(pObject, pObject->FrameRecord(), pObject->get_database(), lIndex, pToggleFunction, pToggleData);
}

void CTxp::Init(DB_RECORD_NUMBER lFrame, PMGDatabase* pDatabase, CHARACTER_INDEX lIndex, TOGGLE_FUNCTION pToggleFunction /*=NULL*/, LPVOID pToggleData /*=NULL*/)
{
	CommonInit(NULL, lFrame, pDatabase, lIndex, pToggleFunction, pToggleData);
}

void CTxp::CommonInit(CFrameObject* pObject,
							 DB_RECORD_NUMBER lFrame,
							 PMGDatabase* pDatabase,
							 CHARACTER_INDEX lIndex,
							 TOGGLE_FUNCTION pToggleFunction,
							 LPVOID pToggleData)
{
	Reset();

	m_pObject = pObject;
	m_pDatabase = pDatabase;
	m_lIndex = m_lSelectStart = m_lSelectEnd = lIndex;
	InvalidateSelectBounds();
	ResetSelectionFrames();
	m_lFrameRecord = lFrame;
	m_nVisible = 0;
	m_pToggleFunction = pToggleFunction;
	m_pToggleData = pToggleData;
	m_fTransition = TRUE;

	// Get the record number of the text record.

	if (m_lFrameRecord == 0)
	{
		m_lTextRecord = 0;
	}
	else
	{
		CFrameRecord* pFrame = (CFrameRecord*)m_pDatabase->get_record(m_lFrameRecord, NULL, RECORD_TYPE_Frame);
		if (pFrame == NULL)
		{
			// Failure. Reset.
			Reset();
		}
		else
		{
			m_lTextRecord = pFrame->TextRecord();
			pFrame->release();
		}
	}

	if (Valid())
	{
		if (m_lIndex > LastOffsetInText())
		{
			ASSERT(FALSE);
			// JRO Someone is trying to set the cursor past the end of the text
			// This will cause problems later, so will be corrected here
			m_lIndex = LastOffsetInText();
		}
	}
}

/*
// Move to a new frame while editing.
// This is used for navigating linked frames.
*/

void CTxp::NewFrame(CFrameObject* pObject)
{
	m_pObject = pObject;
	m_lFrameRecord = pObject->FrameRecord();
	m_lTextRecord = pObject->TextRecord();

	ASSERT(m_pDatabase == pObject->get_database());
}

/*
// Toggle all or part of a txp.
*/

void CTxp::Toggle(int nWhich /*=CTxp::TOGGLE_All*/)
{
	if (m_lSelectStart == m_lSelectEnd)
	{
		nWhich &= ~TOGGLE_Select;
	}

	if (nWhich != 0 && m_pToggleFunction != NULL)
	{
		m_pToggleFunction(this, nWhich, m_pToggleData);
	}
}

/*
// Turn the txp cursor(s) on.
*/

void CTxp::On(int nWhich)
{
	if (Valid())
	{
		if ((nWhich &= (m_nVisible ^ nWhich)) != 0)
		{
			m_nVisible ^= nWhich;

			CalcAll();

			Toggle(nWhich);
		}
	}
}

/*
// Turn the txp cursor(s) off.
*/

void CTxp::Off(int nWhich)
{
	if (Valid())
	{
		if ((nWhich &= m_nVisible) != 0)
		{
			m_nVisible ^= nWhich;

			Toggle(nWhich);
		}
	}
}

/*
// Compute the style of the txp.
*/

ERRORCODE CTxp::CalcStyle(void)
{
	if (!Valid())
	{
		return ERRORCODE_NotInitialized;
	}

	ERRORCODE error = ERRORCODE_None;
	CTextRecord* pText = NULL;
	CFrameRecord* pFrame = NULL;

/* Get the paragraph where the txp currently is. */

	TRY
	{
		pText = LockText();
		pFrame = LockFrame();
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	if (error != ERRORCODE_None)
	{
		if (pText != NULL)
		{
			pText->release();
		}
		if (pFrame != NULL)
		{
			pFrame->release();
		}
		return error;
	}
	ASSERT(pText != NULL);
	ASSERT(pFrame != NULL);

/* Initialize our style. */

	CTextIterator Iterator(pText);

/* Now get the style where we are. */

	if (m_lIndex > 0)
	{
		Iterator.SetPosition(m_lIndex);
	}
	if (Iterator.CharacterInParagraph() > 0)
	{
		Iterator.SetPosition(m_lIndex-1);
	}

	/* Set the style we found. */

	m_Style = Iterator.Style();

	/* Get the composite style of the selected range */

	if (m_lSelectStart != m_lSelectEnd)
	{
		// Compute the range style.
		Iterator.ComputeStyleRange(m_lSelectStart, m_lSelectEnd, &m_SelectStyle);
		// Compute the start and end frames.
		TRY
		{
			m_lSelectStartFrame = pText->GetFrameOfCharacter(m_lSelectStart, m_lSelectStartFrame);
			m_lSelectEndFrame = pText->GetFrameOfCharacter(m_lSelectEnd, m_lSelectEndFrame);
		}
		CATCH_ALL(e)
		{
			m_lSelectStartFrame = m_lSelectEndFrame = 0;
		}
		END_CATCH_ALL
	}
	else
	{
		m_SelectStyle = m_Style;
		ResetSelectionFrames();
	}

	// Compute the frame attributes.
	CalcFrameAttributes();

/* Release the records. */

	pFrame->release();
	pText->release();

/* No longer in a transition state. */

	m_fTransition = FALSE;

	return ERRORCODE_None;
}

/*
// Calculate the frame attributes.
*/

void CTxp::CalcFrameAttributes(void)
{
	DB_RECORD_NUMBER lStartFrame;
	DB_RECORD_NUMBER lEndFrame;
	GetFrameRange(&lStartFrame, &lEndFrame);

	// The attributes are not set yet.
	m_FrameAttributes.m_fSet = FALSE;

	// Process frames until done.
	CFrameRecord* pFrame = NULL;
	TRY
	{
		DB_RECORD_NUMBER lNextFrame = lStartFrame;
		DB_RECORD_NUMBER lFrame;

		do
		{
			// Move to the next frame.
			lFrame = lNextFrame;

			// Lock the frame.
			pFrame = LockFrame(lFrame);

			// Get its attributes.
			pFrame->GetAttributes(&m_FrameAttributes);

			// Set up for the loop.
			lNextFrame = pFrame->NextFrame();
			pFrame->release();
			pFrame = NULL;
		} while (lFrame != lEndFrame);
	}
	END_TRY

	if (pFrame != NULL)
	{
		pFrame->release();
	}
}

/*
// Set the frame attributes.
*/

void CTxp::SetFrameAttributes(const CFrameAttributes& Attributes)
{
	DB_RECORD_NUMBER lStartFrame;
	DB_RECORD_NUMBER lEndFrame;
	GetFrameRange(&lStartFrame, &lEndFrame);

	// Process frames until done.
	CFrameRecord* pFrame = NULL;
	TRY
	{
		DB_RECORD_NUMBER lNextFrame = lStartFrame;
		DB_RECORD_NUMBER lFrame;

		do
		{
			// Move to the next frame.
			lFrame = lNextFrame;

			// Lock the frame.
			pFrame = LockFrame(lFrame);

			// Get its attributes.
			pFrame->SetAttributes(Attributes);

			// Set up for the loop.
			lNextFrame = pFrame->NextFrame();
			pFrame->release();
		} while (lFrame != lEndFrame);
	}
	END_TRY

	if (pFrame != NULL)
	{
		pFrame->release();
	}
}

/*
// Get the frame range corresponding to this txp.
*/

void CTxp::GetFrameRange(DB_RECORD_NUMBER* pStart, DB_RECORD_NUMBER* pEnd)
{
	if (m_lSelectStart == m_lSelectEnd)
	{
		*pStart = *pEnd = m_lFrameRecord;
	}
	else
	{
		*pStart = m_lSelectStartFrame;
		*pEnd = m_lSelectEndFrame;
	}
}

/*
// Lock the frame we are bound to.
*/

CFrameRecord* CTxp::LockFrame(DB_RECORD_NUMBER lFrame /*=-1*/)
{
	if (!Valid())
	{
		ThrowErrorcodeException(ERRORCODE_NotInitialized);
	}

	// Lock the frame.

	CFrameRecord* pFrame;
	ERRORCODE error;

	if (lFrame == -1)
	{
		lFrame = m_lFrameRecord;
	}

	if ((pFrame = (CFrameRecord*)m_pDatabase->get_record(lFrame, &error, RECORD_TYPE_Frame)) == NULL)
	{
		ThrowErrorcodeException(error);
	}
	return pFrame;
}

/*
// Lock the text record we are bound to.
*/

CTextRecord* CTxp::LockText(void)
{
	if (!Valid())
	{
		ThrowErrorcodeException(ERRORCODE_NotInitialized);
	}

	// Lock the text.

	CTextRecord* pText;
	ERRORCODE error;

	if ((pText = (CTextRecord*)m_pDatabase->get_record(m_lTextRecord, &error, RECORD_TYPE_Text)) == NULL)
	{
		ThrowErrorcodeException(error);
	}
	return pText;
}

/*
// Return the last offset in the text.
*/

CHARACTER_INDEX CTxp::LastOffsetInText(void)
{
	CHARACTER_INDEX lIndex = 0;
	CTextRecord* pText = NULL;

	TRY
	{
		pText = LockText();
		lIndex = pText->NumberOfCharacters();
	}
	END_TRY

	if (pText != NULL)
	{
		pText->release();
	}

	return lIndex;
}

/*
// Get a location corresponding to the current position.
*/

void CTxp::GetFrameLocation(CFrameLocation* pLocation, DB_RECORD_NUMBER lFrame /*=-1*/)
{
	CFrameRecord* pFrame = LockFrame(lFrame);

	TRY
	{
		pLocation->m_lCharacter = m_lIndex;
		pFrame->GetCharacterLocation(pLocation);
		pFrame->release();
	}
	CATCH_ALL(e)
	{
		pFrame->release();
		THROW_LAST();
	}
	END_CATCH_ALL
}

/*
// Compute the x offset of ptxp->c_index.
*/

PCOORD CTxp::XOffset(CFrameLocation* pLocation /*=NULL*/)
{
	PCOORD x = -1;
	CTextRecord* pText = NULL;
	CFrameLocation Location;
	CTextIterator Iterator;

	TRY
	{
	/* Get the location for us to use. */

		if (pLocation == NULL)
		{
			GetFrameLocation(pLocation = &Location);
		}

	/* Get the paragraph to do our computation. */

		pText = LockText();

	/* Initialize the style. */
		Iterator.SetRecord(pText);

	/*
	// Now we have the word, so we know the start of the word (x-wise).
	// Find the character x_offset within the word.
	*/

		FontServerPtr pFontServer = m_pDatabase->get_font_server();

		CHARACTER_INDEX lCharacter = pLocation->m_lFirstCharacterInWord;
		CHARACTER_INDEX lSearchIndex = m_lIndex;
		CHARACTER_INDEX lWordEnd = lCharacter + pLocation->m_Word.m_lCharacters;
		if (lSearchIndex > lWordEnd)
		{
			lSearchIndex = lWordEnd;
		}
		BOOL fTabWord = (pLocation->m_Word.m_wType == WORD_TYPE_tab);
		x = pLocation->m_Word.m_XOffset;
		while (lCharacter < lSearchIndex)
		{
			CHARACTER c = *(pText->GetCharacter(lCharacter));
			if (c == '\t' && !fTabWord)
			{
				// Some tabs are not tabs...
				c = ' ';
			}
			Iterator.SetPosition(lCharacter);
			x += pFontServer->GetAdvanceWidth(&Iterator.Style(), c);
			lCharacter++;
		}
	}
	CATCH_ALL(e)
	{
		x = -1;
	}
	END_CATCH_ALL

	if (pText != NULL)
	{
		pText->release();
	}

	return x;
}

/*
// Calculate the frame of the txp's current location.
*/

ERRORCODE CTxp::CalcFrame(BOOL fEnd /*=FALSE*/)
{
	ERRORCODE error;

	if (!fEnd)
	{
		// If we are at the end of a selection, we want to use the "end" method.
//		fEnd = (m_lIndex == m_lSelectEnd) && (m_lSelectStart != m_lSelectEnd);
	}

	CTextRecord* pText = NULL;

	TRY
	{
		// Lock the text record.
		pText = LockText();
		if (m_lIndex > pText->NumberOfCharacters())
		{
			ASSERT(FALSE);
			// JRO The cursor position is invalid
			// Maybe we should not correct this here, but find the REAL bug
			//m_lIndex = pText->NumberOfCharacters();
		}
		m_lCursorFrame = pText->GetFrameOfCharacter(m_lIndex, m_lFrameRecord, fEnd);
//		TRACE("cursor: %ld; frame: %ld (%ld)\n", m_lIndex, m_lCursorFrame, m_lFrameRecord);
		error = ERRORCODE_None;
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

	// Release the text record if we got one.
	if (pText != NULL)
	{
		pText->release();
	}

	return error;
}

/*
// Calculate the current cursor box.
*/

ERRORCODE CTxp::CalcCursor(void)
{
	CFrameRecord* pFrame = NULL;
	ERRORCODE error;

	TRY
	{
		pFrame = LockFrame(m_lCursorFrame);

		// Get the current frame location.
		CFrameLocation Location;
		GetFrameLocation(&Location, m_lCursorFrame);

		PBOX box;

		// Get the x offset.
		box.x0 = XOffset(&Location);

		// Initialize the flags.
		m_nFlags = 0;

		// Now we can set the cursor box.

		m_nCursorColumn = Location.m_Line.m_nColumn;

		PBOX Bound = pFrame->ObjectBounds();

		box.y0 = Location.m_Line.m_Baseline - Location.m_Line.m_Ascend;
		box.y1 = Location.m_Line.m_Baseline + Location.m_Line.m_Descend;
		box.x1 = box.x0
//					+ scale_pcoord(1, PAGE_RESOLUTION, redisplay_x_resolution);
					+ scale_pcoord(1, PAGE_RESOLUTION, 96);		// Cheat!

		m_CursorBox.x0 = box.x0+Bound.x0;
		m_CursorBox.x1 = box.x1+Bound.x0;
		m_CursorBox.y0 = box.y0+Bound.y0;
		m_CursorBox.y1 = box.y1+Bound.y0;

		DWORD dwFrameFlags = pFrame->FrameFlags();

		if (dwFrameFlags & FRAME_FLAG_stretch_text)
		{
			m_nFlags |= FLAG_stretch_text;
		}

		if (dwFrameFlags & FRAME_FLAG_stretch_frame)
		{
			m_nFlags |= FLAG_stretch_frame;
		}

		if (pFrame->PreviousFrame() == 0 && pFrame->NextFrame() == 0)
		{
			m_nFlags |= FLAG_can_stretch_text;
		}

		// Update the rest of the flags.
		// Is this the first character in the column?
		if (m_lIndex == Location.m_lFirstCharacterInLine
					&& Location.m_Line.m_nLineInColumn == 0)
		{
			m_nFlags |= FLAG_start;
		}

		CHARACTER_INDEX lLast = Location.m_lFirstCharacterInLine + Location.m_Line.m_lCharacters;
		if (Location.m_Word.m_wType != WORD_TYPE_solid && Location.m_Word.m_wType != WORD_TYPE_dropcap )
		{
			lLast--;
		}
		BOOL fLastCharacterInLine = m_lIndex >= lLast
								&& m_lIndex != LastOffsetInText()-1;
		BOOL fLastLineInColumn = (Location.m_lLine == pFrame->NumberOfLines()-1
											|| pFrame->GetLine(Location.m_lLine+1)->m_nColumn != Location.m_Line.m_nColumn);
		// Is this the last character in the column?
		if (fLastCharacterInLine && fLastLineInColumn)
		{
			m_nFlags |= FLAG_end;
		}
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

	// Release the frame if we got one.
	if (pFrame != NULL)
	{
		pFrame->release();
	}

	return error;
}

/*
// Calculate where the txp currently is on the page.
// The cursor box is constructed and placed in the txp for reference.
*/

ERRORCODE CTxp::CalcAll(void)
{
	ERRORCODE error;

	// A bad time to be calling this routine.

	if (!Valid())
	{
		return ERRORCODE_NotInitialized;
	}

	// Calculate the frame we are now in, and compute the txp style.

	if (m_fTransition)
	{
		InvalidateSelectBounds();

		if ((error = CalcFrame()) != ERRORCODE_None
//			 || (error = ReframeCursor()) != ERRORCODE_None
			 ||	(error = CalcStyle()) != ERRORCODE_None)
		{
			return error;
		}
	}

	// Finally, calculate the cursor box.

	return CalcCursor();
}

/*
// Set the 'desired_x' field of the txp to the current x position.
*/

void CTxp::SetDesiredX()
{
	CalcFrame();

	// Get the current frame location.
	CFrameLocation Location;
	GetFrameLocation(&Location, m_lCursorFrame);

	m_DesiredX = XOffset(&Location) - Location.m_ColumnLeft;
}

/*
// Set a new position.
// which_end:
//		-1 is left end
//		 0 is no more selection
//		 1 is right end
//		 2 is unknown
*/

void CTxp::SetPosition(CHARACTER_INDEX lNew, int nWhichEnd, BOOL fSetDesiredX)
{
	CHARACTER_INDEX lOld = m_lIndex;

/* Validate the position. */

	if (lNew < 0)
	{
		lNew = 0;
	}
	else
	{
		CHARACTER_INDEX lLast = LastOffsetInText();

		if (lNew > lLast)
		{
			lNew = lLast;
		}
	}

/* See what we're doing. */

	if (nWhichEnd == END_NoSelection)
	{
		Off(TOGGLE_Select);
		m_lSelectStart = m_lSelectEnd = lNew;
		InvalidateSelectBounds();
		// No frames yet.
		ResetSelectionFrames();
	}
	else if (lOld != lNew)
	{
		CHARACTER_INDEX lNewStart, lNewEnd;
		CHARACTER_INDEX lStart = m_lSelectStart,
								lEnd = m_lSelectEnd;

	/* Handle special end cases. */

		if (lStart == lEnd)
		{
		/* No selection yet. */
			nWhichEnd = lNew < lOld ? END_Left : END_Right;
		}
		else if (nWhichEnd == END_Unknown)
		{
			if (lNew <= lStart)
			{
				nWhichEnd = END_Left;
			}
			else if (lNew >= lEnd)
			{
				nWhichEnd = END_Right;
			}
			else
			{
				nWhichEnd = END_Current;
			}
		}

		if (nWhichEnd == END_Current)
		{
			if (lOld == lStart)
			{
				nWhichEnd = END_Left;
			}
			else
			{
				nWhichEnd = END_Right;
			}
		}

	/* Keep things oriented correctly. */

		if (nWhichEnd == END_Left && lNew > lEnd)
		{
			CHARACTER_INDEX lTmp;

			nWhichEnd = END_Right;

			lTmp = lStart;
			lStart = lEnd;
			lEnd = lTmp;
		}

		if (nWhichEnd == END_Right && lNew < lStart)
		{
			CHARACTER_INDEX lTmp;

			nWhichEnd = END_Left;

			lTmp = lStart;
			lStart = lEnd;
			lEnd = lTmp;
		}

	/* Now move the appropriate end. */

		if (nWhichEnd == END_Left)
		{
			lNewStart = lNew;
			lNewEnd = lEnd;

			if (lNewStart < lStart)
			{
				m_lSelectStart = lNewStart;
				m_lSelectEnd = lStart;
			}
			else
			{
				m_lSelectEnd = lNewStart;
			}
		}
		else
		{
			lNewStart = lStart;
			lNewEnd = lNew;

			if (lNewEnd > lEnd)
			{
				m_lSelectStart = lEnd;
				m_lSelectEnd = lNewEnd;
			}
			else
			{
				m_lSelectStart = lNewEnd;
			}
		}

	/* Toggle what changed. */

		if (m_lSelectStart != m_lSelectEnd)
		{
			InvalidateSelectBounds();
			Toggle(TOGGLE_Select);
			m_nVisible |= TOGGLE_Select;
		}

	/* Set the new endpoints for the select. */

		m_lSelectStart = lNewStart;
		m_lSelectEnd = lNewEnd;
		InvalidateSelectBounds();
	}

/* Now move the cursor if it needs moving. */

	if (lOld != lNew)
	{
		Off(TOGGLE_Cursor);
		m_lIndex = lNew;

		CalcFrame();

	/* Set the desired x position if requested. */

		if (fSetDesiredX)
		{
			TRY
			{
				SetDesiredX();
			}
			CATCH_ALL(e)
			{
				ASSERT(FALSE);
			}
			END_CATCH_ALL
		}
	}
	m_fTransition = TRUE;			// Always?
}

/*
// Select all text.
*/

void CTxp::SelectAll(void)
{
	Off(TOGGLE_All);
	m_lSelectStart = 0;
//	m_lIndex =
 	 m_lSelectEnd = LastOffsetInText();
	InvalidateSelectBounds();
	ResetSelectionFrames();
	m_fTransition = TRUE;
	SetDesiredX();
}

/*
// Select some text.
*/

void CTxp::SelectText(CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd, BOOL fSetDesiredX /*=TRUE*/)
{
	if (lStart > lEnd)
	{
		CHARACTER_INDEX lTmp = lStart;
		lStart = lEnd;
		lEnd = lTmp;
	}

	CHARACTER_INDEX lMin = 0;
	CHARACTER_INDEX lMax = LastOffsetInText();

	if (lStart < lMin)
	{
		lStart = lMin;
	}

	if (lEnd > lMax)
	{
		lEnd = lMax;
	}

	Off(TOGGLE_All);
	m_lSelectStart = lStart;
	m_lIndex = m_lSelectEnd = lEnd;
	m_fTransition = TRUE;
	InvalidateSelectBounds();
	if (fSetDesiredX)
	{
		TRY
		{
			SetDesiredX();
		}
		END_TRY
	}
}

/*
// Deselect all text in a txp.
*/

void CTxp::Deselect(void)
{
	Off(TOGGLE_Select);
	m_lSelectStart = m_lSelectEnd = m_lIndex;
	InvalidateSelectBounds();
}

/*
// Insert a character at the current txp.
*/

ERRORCODE CTxp::InsertText(CHARACTER* s, CHARACTER_COUNT lCount, BOOL fSingle)
{
/* Make sure we have an insertion point. */

	if (!Valid())
	{
		return ERRORCODE_NotInitialized;
	}

	CFrameRecord* pFrame = NULL;
	CTextRecord* pText = NULL;
	ERRORCODE error = ERRORCODE_None;

	TRY
	{
	/* Make sure we're all deselected. */

		if (m_lSelectStart != m_lSelectEnd)
		{
			DeleteText(1);
		}

	/* Get the frame to check our flags. */

		pFrame = LockFrame();

		DWORD dwFrameFlags = pFrame->FrameFlags();
		PBOX ObjectBound = pFrame->ObjectBounds();

		PCOORD FrameHeight = ObjectBound.y1 - ObjectBound.y0;
	/* End of bottom line. */
		CTextLine* pLine = pFrame->GetLine(pFrame->NumberOfLines()-1);
		PCOORD NextTop = pLine->m_Baseline + pLine->m_Descend;
	/* Top of top line. */
		pLine = pFrame->GetLine(0);
		NextTop -= pLine->m_Baseline - pLine->m_Ascend;

	/* Get the text record so we can insert. */

		pText = LockText();

	/* Validate the insertion position. */

		if (m_lIndex > pText->NumberOfCharacters())
		{
			od("txp_insert: invalid position\r\n");
			ThrowErrorcodeException(ERRORCODE_BadParameter);
		}

	/* Pickup the style here. */

		CTextIterator Iterator(pText);
		Iterator.SetPosition(m_lIndex);

		CTextStyle StartStyle = Iterator.Style();
		CHARACTER_COUNT lSkipped = 0;
		int nNewSize = -1;

		CFrameLocation Location;
		Location.m_lCharacter = m_lIndex;
		pFrame->GetCharacterLocation(&Location);

	/* Insert all the text! */

		CHARACTER_INDEX lChangeIndex;

		CHARACTER_INDEX lInsert = m_lIndex;
		CHARACTER_COUNT l;				// Accessible outside loop.
		for (l = 0; l < lCount; l++)
		{
			CHARACTER c = *s;
			if (!fSingle)
			{
				s++;
			}

			/* Don't allow ENTER as first character in banners */

			if (c == '\n'
				&& (dwFrameFlags & FRAME_FLAG_no_initial_break)
				&& lInsert == 0)
			{
				++lSkipped;
				continue;
			}

		/*
		// If this is the last character, this is a fit_text frame, and we're
		// inserting a BREAK character, do our special fit.
		*/

			if (lInsert == pText->NumberOfCharacters()
						&& (dwFrameFlags & FRAME_FLAG_fit_text)
						&& c == '\n')
			{
				PCOORD NewBottom;

				if (nNewSize != -1)
				{
				/* Been here already! */
					break;
				}

			/*
			// We need to fit this text to the frame.
			*/

			/* The txp style should be the current. Use its ascend and descend.*/

				NewBottom = NextTop + m_Style.Ascend() + m_Style.Descend();
				
				//special  for Vertical Banners
				if(!m_fIgnoreFrameBoundaries)
				{
					if (NewBottom > FrameHeight)
					{
					/*
					// This line is too tall.
					// Try to generate a smaller point size to make it fit.
					*/
						nNewSize = (int)scale_pcoord(FixedInteger(m_Style.Size()),
													FrameHeight-NextTop,
													NewBottom-NextTop)
											- 4;		/* A fudge factor! */

						lChangeIndex = lInsert+1;
						if (nNewSize < MINIMUM_SHRINK_SIZE)
						{
							nNewSize = -1;
		//					warning_beep();
							break;
						}
					}
				}
			}

		/* Do the insertion. */

			if ((error = pText->InsertText(lInsert++, &c, 1, pFrame->Id())) != ERRORCODE_None)
			{
				ThrowErrorcodeException(error);
			}
		}

		if ((lCount = l - lSkipped) != 0)
		{
			// Update the line and word sizes.

			CTextLine* pLine = pFrame->GetLine(Location.m_lLine);
			pLine->m_lCharacters += lCount;
			CTextWord* pWord = pFrame->GetWord(Location.m_lWord);
			pWord->m_lCharacters += lCount;

		/* If we need to change the size, do it now. */

			if (nNewSize != -1)
			{
				m_Style.BaseSize(MakeFixed(nNewSize));
				m_Style.Size(MakeFixed(nNewSize));

				Iterator.SetPosition(lChangeIndex);

				pText->SetParagraphStyle(Iterator.ParagraphIndex(), &m_Style);
			}

		/* See if we need to adjust the range to fit the insertion style. */

			if (StartStyle != m_Style)
			{
				ASSERT(lCount > 0);
				pText->ChangeRangeStyle(m_lIndex, m_lIndex+lCount, &m_Style);
			}

			// Flow the text.
			FlowText(m_lIndex, m_lIndex+lCount);

			// Move the cursor past the inserted text.
			SetPosition(m_lIndex+lCount, CTxp::END_NoSelection, m_fWantFlowText /*TRUE*/);
		}
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

	if (pFrame != NULL)
	{
		pFrame->release(TRUE);
	}
	if (pText != NULL)
	{
		pText->release(TRUE);
	}

	return error;
}

/*
// Insert a string of 8-bit characters.
*/

ERRORCODE CTxp::InsertString(LPCSTR pszText)
{
	ERRORCODE error;

	// Compute the length of the new text.
	int nLength = strlen(pszText);
	ASSERT(nLength > 0);

	// Allocate a buffer for converting the text to be inserted.
	CHARACTER* pCharacters = NULL;
	TRY
	{
		pCharacters = new CHARACTER[nLength];
	}
	END_TRY

	if (pCharacters == NULL)
	{
		error = ERRORCODE_Memory;
	}
	else
	{
		// Convert the text over.
		for (int n = 0; n < nLength; n++)
		{
			pCharacters[n] = (CHARACTER)(unsigned char)pszText[n];
		}
		error = InsertText(pCharacters, nLength);
		delete [] pCharacters;
	}
	return error;
}

/*
// Delete a character at the current txp.
*/

ERRORCODE CTxp::DeleteText(CHARACTER_COUNT lCount)
{
/* Make sure we have an insertion point. */

	if (!Valid())
	{
		return ERRORCODE_NotInitialized;
	}

	CFrameRecord* pFrame = NULL;
	CTextRecord* pText = NULL;
	ERRORCODE error = ERRORCODE_None;

	TRY
	{
		CHARACTER_INDEX lStart, lEnd;
		DB_RECORD_NUMBER lFrame;

		// Set the range to delete.

		if (m_lSelectStart == m_lSelectEnd)
		{
			Off(TOGGLE_Cursor);
			lEnd = (lStart = m_lIndex) + lCount;
			lFrame = m_lFrameRecord;
		}
		else
		{
			lStart = m_lSelectStart;
			lEnd = m_lSelectEnd + lCount-1;
			lFrame = m_lSelectStartFrame;

			// Cursor will end up at the start of this range.

			SetPosition(m_lSelectStart, END_NoSelection, FALSE);
		}

		// Get the text record so we can see if we need to delete.

		pText = LockText();

	/* Validate the insertion position. */

		CHARACTER_COUNT lTextCount = pText->NumberOfCharacters();

		if (lStart != lTextCount)
		{
			if (lEnd > lTextCount)
			{
				lEnd = lTextCount;
			}

			if (lEnd < lStart)
			{
				od("txp_delete: invalid position\r\n");
				ThrowErrorcodeException(ERRORCODE_BadParameter);
			}

			// Delete the stuff from the frame record.

			DeleteTextFromFrames(lStart, lEnd);

			// Delete the stuff from the text record.

			pText->DeleteText(lStart, lEnd-lStart, lFrame);

			// Flow the text.

			FlowText(lStart, lStart);

			SetDesiredX();
		}

		m_fTransition = TRUE;
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

	// Release any records we locked.

	if (pFrame != NULL)
	{
		pFrame->release(TRUE);
	}
	if (pText != NULL)
	{
		pText->release(TRUE);
	}

	return error;
}

/*
// Handle a backspace request.
*/

ERRORCODE CTxp::Backspace(CHARACTER_COUNT lCount)
{
	if (SomethingSelected())
	{
		return DeleteText(lCount);
	}

	if (lCount > m_lIndex)
	{
		lCount = m_lIndex;
	}

	if (lCount != 0)
	{
		SetPosition(m_lIndex-lCount, END_NoSelection, FALSE);
		return DeleteText(lCount);
	}
	return ERRORCODE_None;
}

/*
// Set the txp m_lIndex via a point location on the page.
*/

ERRORCODE CTxp::MoveToPoint(PPNT p, int nWhichEnd, BOOL fSetDesiredX)
{
	CFrameRecord* pFrame = NULL;
	ERRORCODE error;

	TRY
	{
		pFrame = LockFrame();
		CFrameLocation Location;
		pFrame->GetPointLocation(p, &Location, Object());

		/* Set the new position. */
		SetPosition(Location.m_lCharacter, nWhichEnd, fSetDesiredX);
		error = ERRORCODE_None;
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	// Release the frame if we got it.
	if (pFrame != NULL)
	{
		pFrame->release();
	}

	return error;
}

/*
// Change lines.
*/

ERRORCODE CTxp::ChangeLine(LINE_COUNT lLineDelta, BOOL fShifted)
{
	CFrameRecord* pFrame = NULL;
	ERRORCODE error;

	TRY
	{
		DB_RECORD_NUMBER lOriginalFrame = m_lFrameRecord;

		pFrame = LockFrame(lOriginalFrame);

		CFrameLocation Location;
		Location.m_lCharacter = m_lIndex;
		pFrame->GetCharacterLocation(&Location);

		LINE_INDEX lOriginalLine = Location.m_lLine;
		LINE_INDEX lNewLine = lOriginalLine;

		// Locate the frame the new line will be in.

		LINE_COUNT lLines = lLineDelta;
		if (lLines < 0)
		{
			// Seek backward.
			lLines = -lLines;
			for (;;)
			{
				if (lNewLine >= lLines)
				{
					lNewLine -= lLines;
					break;
				}
				else
				{
					lLines -= (lNewLine+1);
					DB_RECORD_NUMBER lFrame = pFrame->PreviousFrame();
					if (lFrame == 0)
					{
						break;
					}
					pFrame->release();
					pFrame = LockFrame(lFrame);
					lNewLine = pFrame->NumberOfLines()-1;
				}
			}
		}
		else if (lLines > 0)
		{
			// Seek forward.
			for (;;)
			{
				LINE_COUNT lLinesAfter = pFrame->NumberOfLines()-(lNewLine+1);
				if (lLinesAfter >= lLines)
				{
					lNewLine += lLines;
					break;
				}
				else
				{
					lLines -= (lLinesAfter+1);
					DB_RECORD_NUMBER lFrame = pFrame->NextFrame();
					if (lFrame == 0)
					{
						break;
					}
					// Make sure we can move to the next frame.
					CFrameRecord* pNextFrame = LockFrame(lFrame);
					CHARACTER_INDEX lFirst = pNextFrame->FirstCharacter();
					pNextFrame->release();
					if (lFirst == -1)
					{
						break;
					}
					// Move to the next frame.
					pFrame->release();
					pFrame = LockFrame(lFrame);
					lNewLine = 0;
				}
			}
		}

		// Clip to the frame. Eventually, this would move to other linked
		// frames.

		// PMGTODO: The above comment appears to be obsolete. Is this so?
		// If so, it should be removed.

		ASSERT(lNewLine >= 0);
		ASSERT(lNewLine < pFrame->NumberOfLines());

		error = ERRORCODE_None;

		// See if we need to move.
		if ((lNewLine != lOriginalLine) || (pFrame->Id() != lOriginalFrame))
		{
			PBOX Bound = pFrame->ObjectBounds();
			FLAGS Flags = pFrame->ObjectFlags();

			// Get the line so we can see what column it is in.
			CTextLine* pLine = pFrame->GetLine(lNewLine);
			PCOORD TheX = m_DesiredX;

			if ((pFrame->Id() == lOriginalFrame)
			 && (pLine->m_nColumn == Location.m_Line.m_nColumn))
			{
				// We're in the original column.
				TheX += Location.m_ColumnLeft;
			}
			else
			{
				// We need to compute the column left;
				int nColumns;
				PCOORD lColumnSpacing;
				PCOORD lColumnWidth;

				pFrame->GetColumnData(&nColumns, &lColumnSpacing, &lColumnWidth);
				TheX += pLine->m_nColumn*lColumnWidth;
			}

			PPNT p;

			// Compute the x coordinate.
			p.x = Bound.x0 + TheX;
	
			// Compute the y coordinate.
			/* MH 5/21/93: do -1 because of case of font with descent=0 (TWIG) */
			p.y = Bound.y0+pLine->m_Baseline-1;
			
			// Get the location for this point.
			pFrame->GetPointLocation(p, &Location, Object(), TRUE);

			// Set the new position.
			SetPosition(
				Location.m_lCharacter,
				fShifted ? CTxp::END_Current : CTxp::END_NoSelection,
				FALSE);
		}
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	// Release the frame if we got it.
	if (pFrame != NULL)
	{
		pFrame->release();
	}

	return error;
}

/*
// Text is about to be deleted from the text record.
// Do what we need to do to the text frames to make text flow work correctly.
*/

void CTxp::DeleteTextFromFrames(CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd)
{
	ASSERT(lStart < lEnd);

	// We need to remove all words and (to-be-empty) lines that contain the
	// text to be deleted.
	CFrameRecord* pStartFrame = NULL;
	CFrameRecord* pEndFrame = NULL;
	CFrameRecord* pFrame = NULL;

	TRY
	{
		// Find the starting location.
		CFrameLocation StartLocation;
		StartLocation.m_lCharacter = lStart;
		FindCharacterLocation(&StartLocation, 0, -1);

		// Find the ending location.
		CFrameLocation EndLocation;
		EndLocation.m_lCharacter = lEnd;
		FindCharacterLocation(&EndLocation, 1, 0);

		// Now see what words we need to delete!
		// Note that offsets are right-oriented (space between two words
		// belongs to right word).

		// Lock the first frame.

		pStartFrame = LockFrame(StartLocation.m_lFrame);
		pEndFrame = LockFrame(EndLocation.m_lFrame);

		// Orient end toward left word in between-word case.
		if (EndLocation.m_lCharacter == EndLocation.m_lFirstCharacterInWord && EndLocation.m_Word.m_lCharacters != 0)
		{
			ASSERT(EndLocation.m_lWord != 0);
			EndLocation.m_lWord--;
			EndLocation.m_Word = *(pEndFrame->GetWord(EndLocation.m_lWord));
			EndLocation.m_lFirstCharacterInWord -= EndLocation.m_Word.m_lCharacters;
		}

		//
		// Step 1: Handle the starting word.
		//

		BOOL fSameFrame = StartLocation.m_lFrame == EndLocation.m_lFrame;
		BOOL fSameLine = fSameFrame && (StartLocation.m_lLine == EndLocation.m_lLine);
		BOOL fSameWord = fSameFrame && (StartLocation.m_lWord == EndLocation.m_lWord);

		CTextLine* pLine = pStartFrame->GetLine(StartLocation.m_lLine);
		CTextWord* pWord = pStartFrame->GetWord(StartLocation.m_lWord);

		// Compute the number of characters to delete from the start word.
		CHARACTER_INDEX lEndOfWord = fSameWord
										? EndLocation.m_lCharacter
										: StartLocation.m_lFirstCharacterInWord + pWord->m_lCharacters;

		CHARACTER_COUNT lCharacters = lEndOfWord - StartLocation.m_lCharacter;

		// Update the character count for the line.
		ASSERT(pLine->m_lCharacters >= lCharacters);
		pLine->m_lCharacters -= lCharacters;

		// Update the word.
		if (lCharacters >= pWord->m_lCharacters)
		{
			// Entire word deleted.
			ASSERT(lCharacters == pWord->m_lCharacters);
			pStartFrame->DeleteWord(StartLocation.m_lWord--);
			if (fSameFrame)
			{
				EndLocation.m_lWord--;
				if (!fSameLine)
				{
					ASSERT(StartLocation.m_lLine < EndLocation.m_lLine);
					EndLocation.m_lFirstWordInLine--;
				}
			}

			// Remove the word from the line.
			if (--pLine->m_lWords == 0)
			{
#if 0
				// Entire line deleted.
				ASSERT(pLine->m_lCharacters == 0);
				ASSERT(StartLocation.m_lFirstWordInLine == StartLocation.m_lWord+1);

				pStartFrame->DeleteLine(StartLocation.m_lLine);
				if (fSameFrame)
				{
					ASSERT(fSameWord || EndLocation.m_lLine != StartLocation.m_lLine);
					EndLocation.m_lLine--;
				}
#endif
			}
		}
		else
		{
			// Just remove the characters from this word.
			pWord->m_lCharacters -= lCharacters;
			pWord->m_wFlags |= WORD_FLAG_needs_building;
		}

		if (!fSameWord)
		{
			//
			// Step 2: Handle the ending word.
			//

			pLine = pEndFrame->GetLine(EndLocation.m_lLine);
			pWord = pEndFrame->GetWord(EndLocation.m_lWord);

			// Compute the number of characters to delete from the end word.
			lCharacters = EndLocation.m_lCharacter - EndLocation.m_lFirstCharacterInWord;

			// Update the character count for the line.
			ASSERT(pLine->m_lCharacters >= lCharacters);
			pLine->m_lCharacters -= lCharacters;

			// Update the word.
			if (lCharacters >= pWord->m_lCharacters)
			{
				// Entire end word deleted.
				ASSERT(lCharacters == pWord->m_lCharacters);
				pEndFrame->DeleteWord(EndLocation.m_lWord);

				// Remove the word from the line.
				if (--pLine->m_lWords == 0)
				{
#if 0
					// Entire end line deleted.
					ASSERT(pLine->m_lCharacters == 0);
					pEndFrame->DeleteLine(EndLocation.m_lLine--);
#endif
				}
			}
			else
			{
				// Just remove the characters from this word.
				pWord->m_lCharacters -= lCharacters;
				pWord->m_wFlags |= WORD_FLAG_needs_building;
			}

			//
			// Step #3: Delete all words between the start and end.
			// 

			DB_RECORD_NUMBER lFrame = StartLocation.m_lFrame;
			for (;;)
			{
				// Sanity check.
				if (lFrame == 0)
				{
					ASSERT(FALSE);
					break;
				}

				pFrame = LockFrame(lFrame);

				WORD_INDEX lStartWord;
				WORD_INDEX lWordInLine;
				LINE_INDEX lLine;

#if 0
				if (lFrame != StartLocation.m_lFrame
						&& lFrame != EndLocation.m_lFrame)
				{
					// Do a fast delete.
					pFrame->DeleteWord(0, pFrame->NumberOfWords());
					pFrame->DeleteLine(0, pFrame->NumberOfLines());
				}
				else
#endif
				{
					if (lFrame == StartLocation.m_lFrame)
					{
						lStartWord = StartLocation.m_lWord+1;
						lWordInLine = lStartWord - StartLocation.m_lFirstWordInLine;
						ASSERT(lWordInLine >= 0);
						lLine = StartLocation.m_lLine;
					}
					else
					{
						lStartWord = 0;
						lWordInLine = 0;
						lLine = 0;
					}

					WORD_INDEX lEndWord = (lFrame == EndLocation.m_lFrame)
												? EndLocation.m_lWord-1
												: pFrame->NumberOfWords()-1;

					WORD_COUNT lWords = lEndWord-lStartWord+1;

					if (lWords > 0)
					{
						// Delete the words from this frame.
						pFrame->DeleteWord(lStartWord, lWords);

						// Remove the words from the lines.
						while (lWords > 0)
						{
							CTextLine* pLine = pFrame->GetLine(lLine);
							// Compute how many words to remove from this line.
							WORD_COUNT lThisWords = pLine->m_lWords - lWordInLine;
							ASSERT(lThisWords >= 0);
							if (lThisWords > lWords)
							{
								lThisWords = lWords;
							}
							pLine->m_lWords -= lThisWords;
							ASSERT(pLine->m_lWords >= 0);
#if 0
							if (pLine->m_lWords == 0)
							{
								// We need to delete this line.
								pFrame->DeleteLine(lLine);
							}
							else
#endif
							{
								lLine++;
							}
							lWords -= lThisWords;

							// Next line starts at the front.
							lWordInLine = 0;
						}
					}
				}

				// Grab the next frame number.
				DB_RECORD_NUMBER lNextFrame = pFrame->NextFrame();

				// Release this frame.
				pFrame->release();
				pFrame = NULL;

				// See if we should stop now.
				if (lFrame == EndLocation.m_lFrame)
				{
					// Done!
					break;
				}
				// Move to next frame.
				lFrame = lNextFrame;
			}
		}

		ASSERT(pStartFrame != NULL);
		pStartFrame->release();
		pStartFrame = NULL;

		ASSERT(pEndFrame != NULL);
		pEndFrame->release();
		pEndFrame = NULL;
	}
	CATCH_ALL(e)
	{
		if (pStartFrame != NULL)
		{
			pStartFrame->release();
		}
		if (pEndFrame != NULL)
		{
			pEndFrame->release();
		}
		if (pFrame != NULL)
		{
			pFrame->release();
		}
		THROW_LAST();
	}
	END_CATCH_ALL
}

/*
// Set the horizontal alignment for the selected paragraphs.
*/

BOOL CTxp::SetHorizontalAlignment(ALIGN_TYPE Type)
{
	BOOL fResult = FALSE;

	CTextStyle Style(m_pDatabase);
	CTextRecord* pText = NULL;

	TRY
	{
		// Lock the text.
		pText = LockText();

		// Compute the first and last paragraphs.
		PARAGRAPH_INDEX lStart, lEnd;
		GetParagraphRange(&lStart, &lEnd);

		// Build the style change and use the style to carry it.
		CTextStyleChange sc;
		sc.m_wToken = TOKEN_Alignment;
		sc.m_lValue = Type;
		Style.ApplyChange(&sc, FALSE);
		pText->ChangeParagraphStyle(lStart, lEnd, &Style, sc.m_wToken);

		fResult = TRUE;
	}
	END_TRY

	if (pText != NULL)
	{
		pText->release();
	}
	return fResult;
}

/*
// Set the horizontal alignment for the selected frames.
*/

BOOL CTxp::SetVerticalAlignment(VERT_ALIGN_TYPE Type)
{
	BOOL fResult = FALSE;
	CFrameRecord* pFrame = NULL;
	TRY
	{
		pFrame = LockFrame();

		if (pFrame->VerticalAlignment() != Type)
		{
			pFrame->VerticalAlignment(Type);
			FlowText(m_lIndex, m_lIndex);
		}
		fResult = TRUE;
	}
	END_TRY

	if (pFrame != NULL)
	{
		pFrame->release();
	}
	return fResult;
}

/*
// Move the insertion point left if possible.
*/

void CTxp::Left(BOOL fShifted)
{
	SetPosition(m_lIndex-1,
					fShifted ? END_Current : END_NoSelection,
					TRUE);
}

/*
// Move the insertion point right if possible.
*/

void CTxp::Right(BOOL fShifted)
{
	SetPosition(m_lIndex+1,
					fShifted ? END_Current : END_NoSelection,
					TRUE);
}

/*
// Move the insertion point left a solid word if possible.
// The insertion point ends up on the beginning of the word.
*/

void CTxp::LeftWord(BOOL fShifted)
{
	CFrameRecord* pFrame = NULL;
	ERRORCODE error;

	TRY
	{
		pFrame = LockFrame();
		CFrameLocation Location;
		GetFrameLocation(&Location);

		if (Location.m_Word.m_wType != WORD_TYPE_solid
					|| Location.m_lCharacter == Location.m_lFirstCharacterInWord)
		{
			// Need to go left until we find a solid word.
			// If no solid word is found, the cursor is not moved (but any selected
			// text is deselected if not shifted).

			for (;;)
			{
				// Back up a word.
				if (Location.m_lWord == 0)
				{
					// We want to go to the previous frame.
					DB_RECORD_NUMBER lFrame = pFrame->PreviousFrame();
					if (lFrame == 0)
					{
						// Nowhere to go.
						break;
					}
					// Out with the old.
					pFrame->release();
					pFrame = NULL;

					// In with the new
					pFrame = LockFrame(lFrame);
					Location.m_lWord = pFrame->NumberOfWords();
					ASSERT(Location.m_lWord != 0);
				}
				Location.m_lWord--;
				// Grab the word.
				Location.m_Word = *(pFrame->GetWord(Location.m_lWord));
				// Adjust the current character.
				Location.m_lFirstCharacterInWord -= Location.m_Word.m_lCharacters;

			/* Is it solid? */
				if (Location.m_Word.m_wType == WORD_TYPE_solid )
				{
					break;
				}
			}
		}

	/* Move to where we want to go. */

		SetPosition(Location.m_lFirstCharacterInWord,
								fShifted ? END_Current : END_NoSelection,
								TRUE);
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	// Release the frame if we got it.
	if (pFrame != NULL)
	{
		pFrame->release();
	}
}

/*
// Move the insertion point right a solid word if possible.
// The insertion point ends up on the end of the word.
*/

void CTxp::RightWord(BOOL fShifted)
{
	CFrameRecord* pFrame = NULL;
	ERRORCODE error;

	TRY
	{
		pFrame = LockFrame();
		CFrameLocation Location;
		GetFrameLocation(&Location);

		// Need to go right until we find a solid word. Any solid word we are
		// currently on does not count.
		// If no solid word is found, the cursor is not moved (but any selected
		// text is deselected if not shifted).

		do
		{
			// Update our solid status.

			if (Location.m_Word.m_wType == WORD_TYPE_solid)
			{
				break;
			}

			// Grab the next word.
			Location.m_lFirstCharacterInWord += Location.m_Word.m_lCharacters;
			if (++Location.m_lWord == pFrame->NumberOfWords())
			{
				// Want to move to the next frame.
				DB_RECORD_NUMBER lFrame = pFrame->NextFrame();
				if (lFrame == 0)
				{
					// Hit end of frames without hitting 'eot'.
					break;
				}
				// Out with the old.
				pFrame->release();
				pFrame = NULL;
				// In with the new.
				pFrame = LockFrame(lFrame);
				Location.m_lWord = 0;
			}

			Location.m_Word = *(pFrame->GetWord(Location.m_lWord));
		} while (Location.m_Word.m_wType != WORD_TYPE_eot);

		// Move to where we want to go.

		SetPosition(Location.m_lFirstCharacterInWord + Location.m_Word.m_lCharacters,
							fShifted ? END_Current : END_NoSelection,
							TRUE);
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	// Release the frame if we got it.
	if (pFrame != NULL)
	{
		pFrame->release();
	}
}

/*
// Move the insertion point up a line if possible.
*/

void CTxp::Up(BOOL fShifted)
{
	ChangeLine(-1, fShifted);
}

/*
// Move the insertion point down a line if possible.
*/

void CTxp::Down(BOOL fShifted)
{
	ChangeLine(1, fShifted);
}

/*
// Return the first character index for this line.
*/

CHARACTER_INDEX CTxp::FirstCharacterInLine(void)
{
	CHARACTER_INDEX lStart = -1;

	TRY
	{
		CFrameLocation Location;
		GetFrameLocation(&Location);
		lStart = Location.m_lFirstCharacterInLine;
	}
	END_TRY;

	return lStart;
}

/*
// Move the insertion point to beginning of line.
*/

void CTxp::Bol(BOOL fShifted)
{
	CHARACTER_INDEX lNew;

/* Move to the first character in this word. */

	if ((lNew = FirstCharacterInLine()) != -1)
	{
		SetPosition(lNew,
						fShifted ? END_Current : END_NoSelection,
						TRUE);
	}
}

CHARACTER_INDEX CTxp::LastCharacterInLine(void)
{
	CHARACTER_INDEX lEnd = -1;

	CTextRecord* pText = NULL;
	CFrameRecord* pFrame = NULL;
	TRY
	{
		CFrameLocation Location;
		GetFrameLocation(&Location);
		lEnd = Location.m_lFirstCharacterInLine;
		if (Location.m_Line.m_lCharacters > 0)
		{
			lEnd += Location.m_Line.m_lCharacters;
		}
		pText = LockText();
		pFrame = LockFrame(Location.m_lFrame);
		if (lEnd != pText->NumberOfCharacters()
				|| pFrame->GetWord(Location.m_lFirstWordInLine + Location.m_Line.m_lWords-1)->m_wType == WORD_TYPE_break)
		{
			lEnd--;
		}
	}
	END_TRY;

	if (pText != NULL)
	{
		pText->release();
	}
	if (pFrame != NULL)
	{
		pFrame->release();
	}

	return lEnd;
}

/*
// Move the insertion point to end of line.
*/

void CTxp::Eol(BOOL fShifted)
{
	CHARACTER_INDEX lNew;

	if ((lNew = LastCharacterInLine()) != -1)
	{
		SetPosition(lNew,
						fShifted ? END_Current : END_NoSelection,
						TRUE);
	}
}

/*
// Move the insertion point to beginning of text.
*/

void CTxp::Bot(BOOL fShifted)
{
	SetPosition(0, fShifted ? END_Current : END_NoSelection, TRUE);
}

/*
// Move the insertion point to end of paragraph.
*/

void CTxp::Eot(BOOL fShifted)
{
	SetPosition(0x7fffffff, fShifted ? END_Current : END_NoSelection, TRUE);
}

/*
// Get the character range corresponding to this txp.
*/

void CTxp::GetRange(CHARACTER_INDEX* pStart, CHARACTER_INDEX* pEnd)
{
	CHARACTER_INDEX lLast = LastOffsetInText();

	if (m_lSelectStart == m_lSelectEnd)
	{
//		*pStart = 0;
//		*pEnd = lLast;
		*pStart = *pEnd = m_lIndex;
	}
	else
	{
		*pStart = m_lSelectStart;
		*pEnd = m_lSelectEnd;
	}

//	od("get_crange(%ld, %ld-1) [0, %ld]\r\n", *pStart, *pEnd, lLast);
}

/*
// Get the paragraph range corresponding to this txp.
*/

void CTxp::GetParagraphRange(PARAGRAPH_INDEX* pStart, PARAGRAPH_INDEX* pEnd)
{
	CTextIterator Iterator;
	CTextRecord* pText = NULL;

	TRY
	{
		pText = LockText();
		Iterator.SetRecord(pText);

		Iterator.SetPosition(SelectStart());
		*pStart = Iterator.ParagraphIndex();
		Iterator.SetPosition(SelectEnd());
		*pEnd = Iterator.ParagraphIndex();

		pText->release();
	}
	CATCH_ALL(e)
	{
		// Clean up...
		Iterator.Reset();
		if (pText != NULL)
		{
			pText->release();
		}
		// ...and throw up.
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CTxp::UpdateRange(CHARACTER_INDEX* pStart /*=NULL*/, CHARACTER_INDEX* pEnd /*=NULL*/)
{
	CHARACTER_INDEX lStart, lEnd;

	if (pStart == NULL)
	{
		ASSERT(pEnd == NULL);
		GetRange(&lStart, &lEnd);
	}
	else
	{
		ASSERT(pEnd != NULL);
		lStart = *pStart;
		lEnd = *pEnd;
	}

	FlowText(lStart, lEnd);

	// Refresh the frame(s).
	if (m_pDatabase->can_refresh())
	{
		CFrameRecord* pFrame = NULL;
		CTextRecord* pText = NULL;

		TRY
		{
			pText = LockText();
			DB_RECORD_NUMBER lStartFrame = pText->GetFrameOfCharacter(lStart, m_lFrameRecord);
			DB_RECORD_NUMBER lEndFrame = pText->GetFrameOfCharacter(lEnd, m_lFrameRecord);

			DB_RECORD_NUMBER lLastFrame = 0;
			DB_RECORD_NUMBER lFrame = lStartFrame;

			do
			{
				// Lock the frame.
				pFrame = LockFrame(lLastFrame = lFrame);

				if (pFrame->Page() == m_pDatabase->CurrentPage())
				{
					LINE_INDEX lStartLine, lEndLine;

					if (lFrame == lStartFrame)
					{
						CFrameLocation Location;
						Location.m_lCharacter = lStart;
						pFrame->GetCharacterLocation(&Location);
						lStartLine = Location.m_lLine;
					}
					else
					{
						lStartLine = 0;
					}

					if (lFrame == lEndFrame)
					{
						CFrameLocation Location;
						Location.m_lCharacter = lEnd;
						pFrame->GetCharacterLocation(&Location);
						lEndLine = Location.m_lLine;
					}
					else
					{
						lEndLine = pFrame->NumberOfLines()-1;
					}

					PBOX Bound = pFrame->ObjectBounds();
					FLAGS wObjectFlags = pFrame->ObjectFlags();
					PBOX AllBox;

					if (lStartLine <= lEndLine)
					{
						// Refresh all the lines.
						for (LINE_INDEX lLine = lStartLine; lLine <= lEndLine; lLine++)
						{
							CTextLine* pLine = pFrame->GetLine(lLine);

							PBOX Refresh;
							Refresh.x0 = pLine->m_ExtentLeft;
							Refresh.y0 = pLine->m_Baseline - pLine->m_Ascend;
							Refresh.x1 = pLine->m_ExtentRight;
							Refresh.y1 = pLine->m_Baseline + pLine->m_Descend;

							PBOX Box;

							if (wObjectFlags & OBJECT_FLAG_xflipped)
							{
								Box.x0 = Bound.x1 - Refresh.x1;
								Box.x1 = Bound.x1 - Refresh.x0;
							}
							else
							{
								Box.x0 = Bound.x0 + Refresh.x0;
								Box.x1 = Bound.x0 + Refresh.x1;
							}

							if (wObjectFlags & OBJECT_FLAG_yflipped)
							{
								Box.y0 = Bound.y1 - Refresh.y1;
								Box.y1 = Bound.y1 - Refresh.y0;
							}
							else
							{
								Box.y0 = Bound.y0 + Refresh.y0;
								Box.y1 = Bound.y0 + Refresh.y1;
							}
							// Add this line extent to the whole box.
							if (lLine == lStartLine)
							{
								AllBox = Box;
							}
							else
							{
								UnionBox(&AllBox, &AllBox, &Box);
							}
						}
						m_pDatabase->do_refresh_notify(&AllBox, REFRESH_ERASE, NULL, NULL);
					}
				}

				lFrame = pFrame->NextFrame();
				pFrame->release();
				pFrame = NULL;
			} while (lFrame != 0 && lLastFrame != lEndFrame);
		}
		CATCH_ALL(e)
		{
			ASSERT(FALSE);

			if (pFrame != NULL)
			{
				pFrame->release();
			}
		}
		END_CATCH_ALL

		if (pText != NULL)
		{
			pText->release();
		}
	}
}

/*
// Find the location of a character.
*/

void CTxp::FindCharacterLocation(CFrameLocation* pLocation, int nStartBias, int nEndBias)
{
	DB_RECORD_NUMBER lFrame = m_lFrameRecord;

	CFrameRecord* pFrame = NULL;

	TRY
	{
		for (;;)
		{
			pFrame = LockFrame(lFrame);
			CHARACTER_INDEX lStart = pFrame->FirstCharacter() + nStartBias;

			if (pLocation->m_lCharacter < lStart)
			{
			// Go to the previous frame.
				lFrame = pFrame->PreviousFrame();
			}
			else
			{
				CHARACTER_INDEX lEnd = lStart + pFrame->NumberOfCharacters() + nEndBias;
				if (pLocation->m_lCharacter > lEnd)
				{
					// Go to the next frame.
					lFrame = pFrame->NextFrame();
				}
				else
				{
					break;
				}
			}

			if (lFrame == 0)
			{
				// Nowhere to go. Stay here.
				// This will happen on the text endpoints depending on the bias.
				break;
			}
			pFrame->release();
		}

		// Get the location in the found frame.

		if (pFrame == NULL)
		{
			// Should never get here.
			ASSERT(FALSE);
			ThrowErrorcodeException(ERRORCODE_IntError);
		}
		else
		{
			// Get the location.
			pFrame->GetCharacterLocation(pLocation);
			pFrame->release();
			pFrame = NULL;
		}
	}
	CATCH_ALL(e)
	{
		if (pFrame != NULL)
		{
			pFrame->release();
		}
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CTxp::UpdateSelectBounds(void)
{
	if (!m_fSelectBoundsValid)
	{
		ComputeSelectBounds();
		m_fSelectBoundsValid = TRUE;
	}
}

#if 1
/////////////////////////////////////
// ComputeSelectBounds
//
// Calculates the rectangles of the text selection.
// Rewritten to account for the selection fo Drop Cap characters
// and to build more accurate selection rectangles -ces
////////////////////////////////////
void CTxp::ComputeSelectBounds(void)
{
	CVarArray* pArray = &m_SelectBounds;
	CVarArray* pFrameArray = &m_SelectFrames;

	pArray->RemoveAll();
	pFrameArray->RemoveAll();

	if (SelectStart() != SelectEnd())
	{
		PMGDatabase* pDatabase = Database();


		CFrameLocation startLocation, endLocation;
		CTxp temp_txp(pDatabase);
		// Initialize our temporary txp.
		temp_txp.Init(Object(), 0, NULL);

		// Calculate start cursor values.
		temp_txp.Index(SelectStart());
		temp_txp.CalcFrame();
		temp_txp.CalcCursor();

		DB_RECORD_NUMBER lStartFrame = temp_txp.CursorFrame();
		temp_txp.GetFrameLocation( &startLocation, lStartFrame );

		// Calculate end cursor values.
		temp_txp.Index(SelectEnd());
		temp_txp.CalcFrame(/*TRUE*/);
		temp_txp.CalcCursor();

		DB_RECORD_NUMBER lEndFrame = temp_txp.CursorFrame();
		temp_txp.GetFrameLocation( &endLocation, lEndFrame );

		DB_RECORD_NUMBER lNextFrame = lStartFrame;
		DB_RECORD_NUMBER lFrame;

		// Get the current page number. We only need to show selects that
		// occur in frames on the current page.
		DB_RECORD_NUMBER lCurrentPage = pDatabase->CurrentPage();
		ASSERT(lCurrentPage != 0);
		if (lCurrentPage != 0)
		{
			// Get the record for the page we are dealing with. We need this in order to
			// find the frame object associated with the frame.
			PageRecord* pPage = (PageRecord*)m_pDatabase->get_record(lCurrentPage, NULL, RECORD_TYPE_Page);
			ASSERT(pPage != NULL);
			if (pPage != NULL)
			{
				ASSERT(pPage->Id() == lCurrentPage);

				// keep track of the current character location so we can look-up drop caps
				// start at the start of the first line
				CHARACTER_INDEX charindex = startLocation.m_lFirstCharacterInLine;
							
				// Process frames until done.
				do
				{
					// Move to the next frame.
					lFrame = lNextFrame;

					// Grab the frame.
					CFrameRecord *pFrame = (CFrameRecord*)pDatabase->get_record(lFrame, NULL, RECORD_TYPE_Frame);
					if (pFrame == NULL)
					{
						break;
					}

					if (pFrame->Page() == lCurrentPage)
					{
						// Get the frame object corresponding to the frame.
						// We only need to look on the current page.
						CFrameObject* pFrameObject = pDatabase->FindFrameObject(lFrame, pPage->objects());
						ASSERT(pFrameObject != NULL);
						if (pFrameObject != NULL)
						{
							// Get the bound and flags.
							PBOX frameBox = pFrame->ObjectBounds();	
							
							// compute rectangles for each line in this frame in the selection
							LINE_INDEX lCurrentline, lStartline, lEndline; 
							if( lFrame == lStartFrame )
								lStartline = startLocation.m_lLine;
							else
								lStartline = 0;

							if( lFrame == lEndFrame )
								lEndline = endLocation.m_lLine;
							else
								lEndline = pFrame->NumberOfLines() - 1;

							lCurrentline = lStartline;

							while( lCurrentline <= lEndline )
							{
								// calculate the selection rectangle for the line, clip to the frame bounds
								CTextLine *pLine = pFrame->GetLine( lCurrentline );
								ASSERT( pLine );
								PBOX box;
			
								// if at the start of the selection and not at the start of the line
								// put x0 at the 1st char of the selection
								if( lFrame == lStartFrame && lCurrentline == lStartline &&
									startLocation.m_lCharacter != startLocation.m_lFirstCharacterInLine)
								{
									// set m_lIndex to the location character charindex because XOffset does not
									// use the location charindex but m_lIndex
									CHARACTER_INDEX lTemp = m_lIndex;
									m_lIndex = startLocation.m_lCharacter;
									box.x0 = Max( XOffset(&startLocation) + frameBox.x0, frameBox.x0 );
									m_lIndex = lTemp;
								}
								else
								{
									// is it expensive to calculate the frame location at the start of each line?
									CFrameLocation location;
									CHARACTER_INDEX lTemp = m_lIndex;
									m_lIndex = charindex;
									GetFrameLocation(&location, lFrame);
									m_lIndex = lTemp;

									// is the current line the first line of a drop cap
									if( pLine->m_DropCapBaseline > pLine->m_Baseline 
										&& location.m_Word.m_wType == WORD_TYPE_dropcap )
									{
										box.x0 = Max(  location.m_Word.m_XOffset + frameBox.x0, frameBox.x0 );
										box.x1 = Min( location.m_Word.m_Width + frameBox.x0, frameBox.x1);
										box.y0 = Max( (pLine->m_Baseline - pLine->m_Ascend) + frameBox.y0, frameBox.y0 );
										box.y1 = Min( (pLine->m_DropCapBaseline + pLine->m_Descend) + frameBox.y0, frameBox.y1 );
										TRACE( "adding Drop Cap text selection box left %d top %d right %d bottom %d\n", box.x0, box.y0, box.x1, box.y1 );
										pArray->Add(&box);
										pFrameArray->Add(&pFrameObject);
										// set box.x0 for the rest of the selection
										box.x0 = box.x1;
									}
									else 
										box.x0 = Max( location.m_Word.m_XOffset + frameBox.x0, frameBox.x0 );
								}
								// if on the end line put x1 at the end of the selection
								if( lFrame == lEndFrame && lCurrentline == lEndline )
								{
									// if the end loctaion is at the 1st character on the line don't hilight (causes bugs if m_ExtendLetf is set)
//									if( endLocation.m_lCharacter == endLocation.m_lFirstCharacterInLine )
									{
										// make it an empty rectangle so it won't get added
//										box.x1 = box.x0;
									}
//									else
									{
										CHARACTER_INDEX lTemp = m_lIndex;
										m_lIndex = endLocation.m_lCharacter;
										box.x1 = Min( XOffset(&endLocation) + frameBox.x0, frameBox.x1 );
										m_lIndex = lTemp;
									}
								}
								else
									box.x1 = Min( pLine->m_ExtentRight + frameBox.x0, frameBox.x1 );

								box.y0 = Max( (pLine->m_Baseline - pLine->m_Ascend) + frameBox.y0, frameBox.y0 );
								box.y1 = Min( (pLine->m_Baseline + pLine->m_Descend) + frameBox.y0, frameBox.y1 );

								if( box.x1 > box.x0 && box.y1 > box.y0 )
								{
									TRACE( "adding text selection box left %d top %d right %d bottom %d\n", box.x0, box.y0, box.x1, box.y1 );
									pArray->Add(&box);
									pFrameArray->Add(&pFrameObject);
								}
								charindex += pLine->m_lCharacters;
								lCurrentline++;
							}
						}
					}
					lNextFrame = pFrame->NextFrame();
					pFrame->release();
				} while (lFrame != lEndFrame);

				// Release page record.
				pPage->release();
			}
		}
	}

	ASSERT(m_SelectBounds.GetSize() == m_SelectFrames.GetSize());
}
/////////////////////////////////////////////////////////////
#else
//// No longer used, does not support hilight of drop cpa character
void CTxp::ComputeSelectBounds(void)
{
	CVarArray* pArray = &m_SelectBounds;
	CVarArray* pFrameArray = &m_SelectFrames;

	pArray->RemoveAll();
	pFrameArray->RemoveAll();

	if (SelectStart() != SelectEnd())
	{
		PMGDatabase* pDatabase = Database();
		CTxp temp_txp(pDatabase);

		// Initialize our temporary txp.
		temp_txp.Init(Object(), 0, NULL);

		// Calculate start cursor values.
		temp_txp.Index(SelectStart());
		temp_txp.CalcFrame();
		temp_txp.CalcCursor();

		DB_RECORD_NUMBER lStartFrame = temp_txp.CursorFrame();
		int nStartColumn = temp_txp.CursorColumn();
		PBOX StartCursorBox = temp_txp.CursorBox();
		FLAGS uStartFlags = temp_txp.Flags();

		// Calculate end cursor values.
		temp_txp.Index(SelectEnd());
		temp_txp.CalcFrame(/*TRUE*/);
		temp_txp.CalcCursor();

		DB_RECORD_NUMBER lEndFrame = temp_txp.CursorFrame();
		int nEndColumn = temp_txp.CursorColumn();
		PBOX EndCursorBox = temp_txp.CursorBox();
		FLAGS uEndFlags = temp_txp.Flags();

		DB_RECORD_NUMBER lNextFrame = lStartFrame;
		DB_RECORD_NUMBER lFrame;

		// Get the current page number. We only need to show selects that
		// occur in frames on the current page.
		DB_RECORD_NUMBER lCurrentPage = pDatabase->CurrentPage();
		ASSERT(lCurrentPage != 0);
		if (lCurrentPage != 0)
		{
			// Get the record for the page we are dealing with. We need this in order to
			// find the frame object associated with the frame.
			PageRecord* pPage = (PageRecord*)m_pDatabase->get_record(lCurrentPage, NULL, RECORD_TYPE_Page);
			ASSERT(pPage != NULL);
			if (pPage != NULL)
			{
				ASSERT(pPage->Id() == lCurrentPage);
			
				// Process frames until done.
				do
				{
					// Move to the next frame.
					lFrame = lNextFrame;

					// Grab the frame.
					CFrameRecord* pFrame = (CFrameRecord*)pDatabase->get_record(lFrame, NULL, RECORD_TYPE_Frame);
					if (pFrame == NULL)
					{
						break;
					}

					if (pFrame->Page() == lCurrentPage)
					{
						// Get the frame object corresponding to the frame.
						// We only need to look on the current page.
						CFrameObject* pFrameObject = pDatabase->FindFrameObject(lFrame, pPage->objects());
						ASSERT(pFrameObject != NULL);
						if (pFrameObject != NULL)
						{
							// Get the bound and flags.
							PBOX Bound = pFrame->ObjectBounds();			

							// Setup the column bound (so far).
							PBOX ColumnBound = Bound;

							// Get the column data from the frame.
							int nColumns;
							PCOORD lColumnSpacing;
							PCOORD lColumnWidth;
							pFrame->GetColumnData(&nColumns, &lColumnSpacing, &lColumnWidth);

							// We need to loop on columns.
							int nColumn = (lFrame == lStartFrame) ? nStartColumn : 0;
							int nThisEndColumn = (lFrame == lEndFrame) ? nEndColumn : nColumns-1;

							PCOORD lColumnLeft = nColumn*lColumnWidth;
							while (nColumn <= nThisEndColumn)
							{
								// Compute the full width of the column.
								ColumnBound.x0 = Bound.x0 + lColumnLeft;
								ColumnBound.x1 = ColumnBound.x0+lColumnWidth;

								// A flag saying whether to record or not.
								BOOL fRecord = TRUE;

								// Compute the starting box.
								PBOX StartBox = StartCursorBox;
								BOOL fFlushStart = FALSE;

								if (lFrame != lStartFrame || nColumn != nStartColumn)
								{
									// Text precedes.
									fFlushStart = TRUE;

									// If our contribution to this select is just the little
									// piece before our text, we do not draw it (it will be
									// drawn at a different time if necessary).
									if (uEndFlags & CTxp::FLAG_start)
									{
										fRecord = FALSE;
									}
								}
								else if (uStartFlags & CTxp::FLAG_start)
								{
									fFlushStart = TRUE;
								}

								if (fFlushStart)
								{
									StartBox.x0 = ColumnBound.x0;
									StartBox.y0 = ColumnBound.y0;
								}

								// Compute the ending box.
								PBOX EndBox = EndCursorBox;
								BOOL fFlushEnd = FALSE;

								if (lFrame != lEndFrame || nColumn != nEndColumn)
								{
									// Text follows.
									fFlushEnd = TRUE;

									// If our contribution to this select is just the little
									// piece after our text, we do not draw it (it will be
									// drawn at a different time if necessary).
									if (uStartFlags & CTxp::FLAG_end)
									{
										fRecord = FALSE;
									}
								}
								else if (uEndFlags & CTxp::FLAG_end)
								{
									fFlushEnd = TRUE;
								}

								if (fFlushEnd)
								{
									EndBox.x0 = EndBox.x1 = ColumnBound.x1;
									EndBox.y0 = EndBox.y1 = ColumnBound.y1;
								}

								// Now record the boxes.
								if (fRecord)
								{
									BOOL fClip = lFrame == Object()->FrameRecord();
									if (fFlushStart && fFlushEnd)
									{
										// Full frame draw.
										StartBox.x1 = EndBox.x1;
										StartBox.y1 = EndBox.y1;

										pArray->Add(&StartBox);
										pFrameArray->Add(&pFrameObject);
									}
									else
									{
										// Not full frame.
										if (StartBox.y0 != EndBox.y0)
										{
											// At least two rows. Draw the first row.
											StartBox.x1 = ColumnBound.x1;

											pArray->Add(&StartBox);
											pFrameArray->Add(&pFrameObject);
											StartBox.x0 = ColumnBound.x0;

											// Move to the second row.
											StartBox.y0 = StartBox.y1;
											if (StartBox.y0 != EndBox.y0)
											{
												// Three rows. Draw the second row.
												StartBox.y1 = EndBox.y0;
												pArray->Add(&StartBox);
												pFrameArray->Add(&pFrameObject);
												StartBox.y0 = StartBox.y1;
											}
											StartBox.y1 = EndBox.y1;
										}
										else
										{
											StartBox.y1 = EndBox.y1;
										}

										// Draw the final row.
										StartBox.x1 = EndBox.x0;
										pArray->Add(&StartBox);
										pFrameArray->Add(&pFrameObject);
									}
								}

								// Advance to next column.
								lColumnLeft += lColumnWidth;
								nColumn++;
							}
						}
					}

					lNextFrame = pFrame->NextFrame();
					pFrame->release();
				} while (lFrame != lEndFrame);

				// Release page record.
				pPage->release();
			}
		}
	}

	ASSERT(m_SelectBounds.GetSize() == m_SelectFrames.GetSize());
}
#endif ///////////

void CTxp::FlowText(CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd)
{
	if (m_fWantFlowText)
	{
		CTextFlow TextFlow;

		if (m_lCursorFrame != 0 && m_lCursorFrame != m_lFrameRecord)
		{
			TextFlow.Init(m_lCursorFrame, m_pDatabase, lStart, lEnd);
		}
		else if (m_pObject != NULL)
		{
			TextFlow.Init(m_pObject, lStart, lEnd);
		}
		else
		{
			TextFlow.Init(m_lFrameRecord, m_pDatabase, lStart, lEnd);
		}

		TextFlow.FlowText();
		if (TextFlow.GetStatus() == CTextFlow::STATUS_Error)
		{
			ERRORCODE error = TextFlow.GetError();
			TextFlow.Cleanup();
			ThrowErrorcodeException(error);
		}
	}
}
