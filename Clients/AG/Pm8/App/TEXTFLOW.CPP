/*
// Textflow routines.
//
// $Header: /PM8/App/TEXTFLOW.CPP 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TEXTFLOW.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 11    2/19/99 12:17p Cschende
// 1. Fixed flow bug with drop caps and linked text boxes by setting
// m_dropbaseline  to 0 when the current line in the paragraph is greater
// then the number of lines droped. The bug occured when calculating the
// baseline for a paragraph which followed a drop cap paragraph which was
// started in a different frame. 2. Account for interparagraph spacing in
// AdjustBaselines().
// 
// 10    2/17/99 5:16p Cschende
// fixed bug when calculating m_LineInParagraph when the paragraph starts
// in a different frame
// 
// 9     2/17/99 11:09a Cschende
// fixed dropcap baseline calculation bug
// fixed calculation of baseline for paragraphs following a drop cap when
// the drop cap paragraph has less lines then the drop cap
// tweeked size calculation of drop characters
// 
// 8     2/16/99 6:13p Cschende
// fixed text wrap not wrapping correctly with a drop cap bug. fixed
// hanging bug when text box is narrower then the drop cap character
// 
// 7     2/15/99 10:16a Cschende
// fixed refresh/update problems with drop caps and bullets
// 
// 6     2/11/99 11:55a Cschende
// Added code for Drop Caps implementation
// 
// 5     12/15/98 5:42p Cschende
// removed commented out bullet code
// 
// 4     12/10/98 4:11p Cschende
// Added bullet, numbered list and multilevel list support in text
// 
// 3     3/19/98 2:32p Jayn
// Finish up master pages.
// 
// 2     3/18/98 11:15a Jayn
// Page numbers
// 
//    Rev 1.0   14 Aug 1997 15:26:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:44   Fred
// Initial revision.
// 
//    Rev 1.56   15 Jul 1997 14:34:00   Jay
// Fixed bug where word macro flag was being turned off.
// 
//    Rev 1.55   07 Jul 1997 13:23:58   Jay
// Fixed the code that makes empty fill-in fields go away.
// 
//    Rev 1.54   09 Apr 1997 14:51:42   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.53   13 Mar 1997 12:02:36   Fred
// Support for rotated text flowaround
// 
//    Rev 1.52   12 Mar 1997 14:55:08   Fred
// Changes for flipped warped and rotated text
// 
//    Rev 1.51   05 Mar 1997 14:36:40   Fred
// More text object stuff
// 
//    Rev 1.50   07 Feb 1997 08:53:38   Fred
// Accounts for warping when adding refresh extents
// 
//    Rev 1.49   06 Dec 1996 11:36:44   Jay
// 
//    Rev 1.48   06 Dec 1996 10:41:32   Jay
// Now handles rotated frames (like title text).
// Won't try to refresh invalid lines.
// 
//    Rev 1.47   05 Nov 1996 14:06:54   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.46   28 Oct 1996 13:46:32   Jay
// Changes from 3.01 code
// 
//    Rev 1.46   25 Oct 1996 10:29:28   Jay
// Frame not on page support.
// 
//    Rev 1.45   03 Sep 1996 17:35:04   Jay
// Text flow with extents improvements.
// 
//    Rev 1.44   03 Sep 1996 15:34:12   Jay
// Now handles flipped frames with intruding flow extents.
// 
//    Rev 1.43   01 Sep 1996 09:23:12   Jay
//  
// 
//    Rev 1.42   27 Aug 1996 08:12:16   Jay
// Fixed stretching, which got broken by columns, and text extents, and...
// 
//    Rev 1.41   31 Jul 1996 18:21:10   Jay
// Fixes, etc!
// 
//    Rev 1.40   30 Jul 1996 11:29:56   Jay
// Fixed WordFitsHorizontally() for stretch case with EOT word.
// 
//    Rev 1.39   27 Jul 1996 09:14:14   Jay
// Fixed overflow in text flow extent code (rounding macro was brain-dead).
// 
//    Rev 1.38   22 Jul 1996 18:29:12   Jay
// Certain conditions prevent wrap around objects.
// 
//    Rev 1.37   22 Jul 1996 18:07:26   Jay
// Tab code now works with tabs in columns.
// 
//    Rev 1.36   17 Jul 1996 18:07:56   Jay
// One attempt to keep it from running away.
// 
//    Rev 1.35   17 Jul 1996 15:42:50   Jay
//  
// 
//    Rev 1.34   17 Jul 1996 12:55:54   Jay
// Blank macros don't cause line breaks.
// 
//    Rev 1.33   16 Jul 1996 11:25:22   Jay
// Removed erroneous ASSERT
// 
//    Rev 1.32   15 Jul 1996 17:46:02   Jay
//  
// 
//    Rev 1.31   12 Jul 1996 16:48:22   Jay
// Text wrap
// 
//    Rev 1.30   11 Jul 1996 17:44:22   Jay
//  
// 
//    Rev 1.29   11 Jul 1996 11:35:24   Jay
//  
// 
//    Rev 1.28   10 Jul 1996 10:25:50   Jay
// Text states no longer allocated.
// 
//    Rev 1.27   09 Jul 1996 17:34:44   Jay
// Fix for blocks of lines that are identical maintaining flow when they're
// supposed to be moving up or down. (Textflow was optimizing out a case it
// shouldn't have been.)
// 
// 
//    Rev 1.26   09 Jul 1996 15:37:16   Jay
//  
// 
//    Rev 1.25   08 Jul 1996 17:51:36   Jay
// Linked frames, etc.
// 
//    Rev 1.24   05 Jul 1996 10:47:14   Jay
//  
// 
//    Rev 1.23   03 Jul 1996 14:30:32   Jay
//  
// 
//    Rev 1.22   28 Jun 1996 16:18:36   Jay
//  
// 
//    Rev 1.21   27 Jun 1996 13:27:46   Jay
//  
// 
//    Rev 1.20   26 Jun 1996 14:22:26   Jay
//  
// 
//    Rev 1.19   25 Jun 1996 13:13:12   Jay
//  
// 
//    Rev 1.18   25 Jun 1996 10:25:32   Jay
//  
// 
//    Rev 1.17   24 Jun 1996 18:06:14   Jay
//  
// 
//    Rev 1.16   21 Jun 1996 17:49:38   Jay
//  
// 
//    Rev 1.15   21 Jun 1996 15:43:40   Jay
// Stretch flow tweaks.
// 
//    Rev 1.14   21 Jun 1996 14:56:04   Jay
// Full extent words are not broken in stretch modes.
// 
//    Rev 1.13   21 Jun 1996 14:30:30   Jay
// Stretch text, etc.
// 
//    Rev 1.12   20 Jun 1996 16:15:56   Jay
// Frame refresh offsets
// 
//    Rev 1.11   20 Jun 1996 14:59:50   Jay
//  
// 
//    Rev 1.10   19 Jun 1996 17:32:14   Jay
//  
// 
//    Rev 1.9   19 Jun 1996 13:45:54   Jay
//  
// 
//    Rev 1.8   18 Jun 1996 16:14:52   Jay
//  
// 
//    Rev 1.7   17 Jun 1996 13:19:46   Jay
//  
// 
//    Rev 1.6   17 Jun 1996 12:01:28   Jay
//  
// 
//    Rev 1.5   14 Jun 1996 17:05:42   Jay
//  
// 
//    Rev 1.4   13 Jun 1996 15:53:08   Jay
//  
// 
//    Rev 1.3   05 Jun 1996 11:34:54   Jay
// Another fix.
// 
//    Rev 1.2   05 Jun 1996 10:28:18   Jay
//  
// 
//    Rev 1.1   04 Jun 1996 17:38:22   Jay
//  
*/

#include "stdafx.h"

#include "pmgdb.h"

#include "textrec.h"
#include "framerec.h"
#include "frameobj.h"

#include "textflow.h"
#include "fontsrv.h"
#include "objlist.h"
#include "grpobj.h"
#include "pagerec.h"

#include "linecros.h"

#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define MAX_COLUMN_WIDTH (32000L*PAGE_RESOLUTION)				// 32000"

/////////////////////////////////////////////////////////////////////////////
// CTextFlow - the state needed for interruptable text flow.
// This contains the original input parameters as well as anything generated
// in-process that is needed to continue on after an interruption.

CTextFlow::CTextFlow()
				: 	m_WordArray(sizeof(CTextWord)),
					m_ExtentArray(sizeof(CTextFlowExtent))
{
	CommonConstruct();
}

CTextFlow::CTextFlow(DB_RECORD_NUMBER lFrame,
						PMGDatabase* pDatabase,
						CHARACTER_INDEX lStartCharacter,
						CHARACTER_INDEX lEndCharacter)
				: 	m_WordArray(sizeof(CTextWord)),
					m_ExtentArray(sizeof(CTextFlowExtent))
{
	CommonConstruct();
	Init(lFrame, pDatabase, lStartCharacter, lEndCharacter);
}

CTextFlow::CTextFlow(CFrameObject* pFrameObject,
						CHARACTER_INDEX lStartCharacter,
						CHARACTER_INDEX lEndCharacter)
				: 	m_WordArray(sizeof(CTextWord)),
					m_ExtentArray(sizeof(CTextFlowExtent))
{
	CommonConstruct();
	Init(pFrameObject, lStartCharacter, lEndCharacter);
}

void CTextFlow::CommonConstruct(void)
{
	m_pTextRecord = NULL;
	m_pTextState = NULL;
	m_pFrameRecord = NULL;
	m_pFramePage = NULL;

	m_nStretchMode = STRETCH_MODE_None;
	m_fFlowingMacros = FALSE;
	m_pMacros = NULL;

	m_pFrameObject = NULL;
	m_lFrame = 0;
	m_pDatabase = NULL;
	m_lStartCharacter = -1;
	m_lEndCharacter = -1;

	m_LeftOffset = 0;
	m_DropBaseline = 0;
	m_nLineInParagraph = -1;
	m_DropLines = 0;
	m_DropSize = 0;

}
								
void CTextFlow::Init(DB_RECORD_NUMBER lFrame,
							PMGDatabase* pDatabase,
							CHARACTER_INDEX lStartCharacter,
							CHARACTER_INDEX lEndCharacter)
{
	// Set the input parameters.
	m_pFrameObject = NULL;
	m_lFrame = lFrame;
	m_pDatabase = pDatabase;
	m_lStartCharacter = lStartCharacter;
	m_lEndCharacter = lEndCharacter;

	m_LeftOffset = 0;
	m_DropBaseline = 0;
	m_nLineInParagraph = -1;
	m_DropLines = 0;
	m_DropSize = 0;

	// We are in an initialization state.
	SetStatus(STATUS_Initialize);
}

void CTextFlow::Init(CFrameObject* pFrameObject,
							CHARACTER_INDEX lStartCharacter,
							CHARACTER_INDEX lEndCharacter)
{
	// Set the input parameters.

	if (pFrameObject == NULL)
	{
		ASSERT(FALSE);
		SetStatus(STATUS_Error, ERRORCODE_BadParameter);
	}
	else
	{
		m_pFrameObject = pFrameObject;
		m_lFrame = pFrameObject->FrameRecord();
		m_pDatabase = pFrameObject->get_database();
		m_lStartCharacter = lStartCharacter;
		m_lEndCharacter = lEndCharacter;

		m_LeftOffset = 0;

		// We are in an initialization state.
		SetStatus(STATUS_Initialize);
	}
}

CTextFlow::~CTextFlow()
{
	Cleanup();
}

void CTextFlow::FlowText(void)
{
	// Enter the main TRY block. Exceptions do not pass here.
	TRY
	{
		switch (GetStatus())
		{
			case STATUS_Error:
			case STATUS_Done:
			{
				// Finished or got error last time. Not much to do this time.
				return;
			}
			case STATUS_Initialize:
			case STATUS_Interrupted:
			{
				break;
			}
			default:
			{
				// Invalid status value.
				ASSERT(FALSE);
				return;
			}
		}

		// Lock the frame record. This is required.

		ASSERT(m_pFrameRecord == NULL);
		ASSERT(m_pTextRecord == NULL);

		GetFrame();

		// Handle any stretching now.
		if ((m_pFrameRecord->FrameFlags() & FRAME_FLAG_stretch_text)
					&& m_nStretchMode == STRETCH_MODE_None)
		{
			// Stretch the text.
			ERRORCODE error;

			if ((error = m_pFrameObject->StretchText()) != ERRORCODE_None)
			{
				ThrowErrorcodeException(error);
			}
			SetStatus(STATUS_Done);
		}
		else
		{
			// Do the flow.

			// Initialize the state if not continuing.
			if (GetStatus() == STATUS_Initialize)
			{
				// We need to initialize the state.
				InitializeFlow();
			}

			// Initialize the source word.
			InitializeSource();
			// Initialize the destination word.
			InitializeDestination();

			// Set that we are now flowing text.
			SetStatus(STATUS_Flowing);

			m_fWordsExhausted = FALSE;
			m_fSpaceExhausted = FALSE;

			// While we are flowing text, flow text.
			do
			{
				FlowLine();
			} while (GetStatus() == STATUS_Flowing);

			// Done or interrupted.
			ASSERT(GetStatus() == STATUS_Done
					 || GetStatus() == STATUS_Interrupted);

			if (GetStatus() == STATUS_Done)
			{
				FinishFrame(m_fWordsExhausted || m_fSpaceExhausted);
				if (m_fWordsExhausted && !m_fSpaceExhausted)
				{
					// We need to go to all the rest of the frames and empty them.
					DB_RECORD_NUMBER lNext;
					while ((lNext = m_pFrameRecord->NextFrame()) != 0)
					{
						m_lFrame = lNext;
						m_lCurrentLine = 0;
						GetFrame(TRUE, TRUE);
						FinishFrame(TRUE, TRUE);
					}
				}
			}

			// Handle any stretching now.
			if (m_dwFrameFlags & (FRAME_FLAG_stretch_frame|FRAME_FLAG_ystretch_frame))
			{
				// Stretch the frame.
				ERRORCODE error;
				if ((error = m_pFrameObject->StretchFrame()) != ERRORCODE_None)
				{
					ThrowErrorcodeException(error);
				}
				SetStatus(STATUS_Done);
			}
		}
	}
	CATCH(CErrorcodeException, e)
	{
		SetStatus(STATUS_Error, e->m_error);
	}
	AND_CATCH(CMemoryException, e)
	{
		SetStatus(STATUS_Error, ERRORCODE_Memory);
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
		SetStatus(STATUS_Error, ERRORCODE_IntError);
	}
	END_CATCH_ALL

	// Do our cleanup before we leave.
	Cleanup();
}

/*
// Cleanup our state.
*/

void CTextFlow::Cleanup(void)
{
	// Get rid of the text record.
	if (m_pTextRecord != NULL)
	{
		m_pTextRecord->release();
		m_pTextRecord = NULL;
	}

	// Get rid of the frame record.
	ReleaseFrame();

	// Get rid of any page we loaded for the frame object.
	ReleaseFramePage();

	// Get rid of the source frame record.
	m_Source.Cleanup();

	// Get rid of the destination frame record.
	m_Destination.Cleanup();
}

/*
// Get the current frame.
*/

void CTextFlow::GetFrame(BOOL fSetState /*=FALSE*/, BOOL fResetFrame /*=FALSE*/)
{
	ERRORCODE error;
	if (m_pFrameRecord == NULL || m_pFrameRecord->Id() != m_lFrame)
	{
		// Out with the old.
		ReleaseFrame();
		if ((m_pFrameRecord = (CFrameRecord*)m_pDatabase->get_record(m_lFrame, &error, RECORD_TYPE_Frame)) == NULL)
		{
			ThrowErrorcodeException(error);
		}

		if (m_nStretchMode == STRETCH_MODE_Flow)
		{
			if ((m_FrameWidth = m_StretchDims.x) > MAX_COLUMN_WIDTH)
			{
				m_FrameWidth = MAX_COLUMN_WIDTH;
			}
		}
		else
		{
			m_FrameWidth = m_pFrameRecord->ObjectBounds().x1 - m_pFrameRecord->ObjectBounds().x0;
		}
		m_FrameHeight = m_pFrameRecord->ObjectBounds().y1 - m_pFrameRecord->ObjectBounds().y0;
		m_dwFrameFlags = m_pFrameRecord->FrameFlags();
	}

	// Make sure we have the frame object for this frame record.
	// If we are just resetting the frame, only get it if it is on the current
	// page.

	if (fResetFrame && m_pDatabase->CurrentPage() != m_pFrameRecord->Page())
	{
		m_pFrameObject = NULL;
	}
	else
	{
		GetFrameObject();
	}

	// Get the text record. This is required.
	// It is assumed that there is one common text record for all the frames.

	if (m_pTextRecord == NULL)
	{
		// We do not have the text record yet. Get it now.
		if ((m_pTextRecord = (CTextRecord*)m_pDatabase->get_record(m_pFrameRecord->TextRecord(), &error, RECORD_TYPE_Text)) == NULL)
		{
			ThrowErrorcodeException(error);
		}
		ASSERT(m_pTextRecord->Id() == m_pFrameRecord->TextRecord());
		m_Iterator.SetRecord(m_pTextRecord);
	}
	else
	{
		// We already have the text record. Make sure everything is consistent.
		if (m_pTextRecord->Id() != m_pFrameRecord->TextRecord())
		{
			// The text record we have does not match the frame.
			ASSERT(FALSE);
			ThrowErrorcodeException(ERRORCODE_InvalidRecord);
		}
	}

	// Get a pointer to the text state.

	m_pTextState = &m_pFrameRecord->TextState();

	// If we need to set the state, do it now.
	if (fSetState)
	{
		// Use the iterator to get the state. It should already be close.
		if (fResetFrame)
		{
			m_pTextState->m_lCharacter = -1;
			m_pTextState->m_lParagraph = -1;
			m_pTextState->m_lCharacterInParagraph = 0;
		}
		else
		{
			m_Iterator.SetPosition(m_lCharacter);
			*m_pTextState = m_Iterator.State();
		}
	}

	if (!fResetFrame && m_pTextState->m_lCharacter != -1)
	{
		// Compute the amount of "before space" to put at the front of the frame.
		if (m_pTextState->m_lCharacterInParagraph == 0)
		{
			// Compute the style of the first character in the frame.
			m_Iterator.SetPosition(m_pTextState->m_lCharacter);
			m_FirstLineSpace = InchesToPage(m_Iterator.Style().SpaceBefore());
		}
		else
		{
			m_FirstLineSpace = 0;
		}
	}
}

/*
// Get the object for the current frame.
*/

void CTextFlow::GetFrameObject(void)
{
	if (m_pFrameObject == NULL || m_pFrameObject->FrameRecord() != m_lFrame)
	{
		// The frame object is not what we need to do our flow.
		m_pFrameObject = NULL;			// Make sure.

		// Make sure we have a page!
		DB_RECORD_NUMBER lPage = m_pFrameRecord->Page();
		if (lPage == 0)
		{
			ASSERT(FALSE);
			ThrowErrorcodeException(ERRORCODE_IntError);
		}

		// See if we have the page loaded.
		if (m_pFramePage == NULL || m_pFramePage->Id() != lPage)
		{
			// Out with any old.
			ReleaseFramePage();
			ASSERT(m_pFramePage == NULL);

			// In with the new.
			ERRORCODE error;
			m_pFramePage = (PageRecord*)m_pDatabase->get_record(lPage, &error, RECORD_TYPE_Page);
			if (m_pFramePage == NULL)
			{
				ThrowErrorcodeException(error);
			}
		}

		// Look for the frame on the page.
		m_pFrameObject = m_pDatabase->FindFrameObject(m_lFrame, m_pFramePage->objects());
		if (m_pFrameObject == NULL)
		{
			// The frame object for a frame record can not be found.
			// Things are not consistent.
			ASSERT(FALSE);
			ThrowErrorcodeException(ERRORCODE_IntError);
		}
	}
}

/*
// Release the frame if we have one locked.
*/

void CTextFlow::ReleaseFrame(void)
{
	if (m_pFrameRecord != NULL)
	{
		m_pFrameRecord->release(TRUE);		// Assume it has been modified.
		m_pFrameRecord = NULL;
	}
}

/*
// Release any page we have locked for a frame object.
*/

void CTextFlow::ReleaseFramePage(void)
{
	if (m_pFramePage != NULL)
	{
		m_pFrameObject = NULL;

		// Release the page. We say it has been modified since the object
		// may have changed during textflow.
		m_pFramePage->release(TRUE);
		m_pFramePage = NULL;
	}
}

/*
// Initialize the word source.
*/

void CTextFlow::InitializeSource(void)
{
	m_Source.Initialize();
}

/*
// Get the current source word.
*/

CTextWord* CTextFlow::GetSourceWord(void)
{
	CTextWord* pWord = m_Source.GetWord();
	if (pWord != NULL && pWord->m_wType == WORD_TYPE_eot)
	{
		m_Source.m_lFrame = 0;
		m_Source.GetFrame();
		return NULL;
	}
	return pWord;
}

/*
// Initialize the word destination.
*/

void CTextFlow::InitializeDestination(void)
{
	m_Destination.Initialize();
//	m_Destination.m_fCanAdvance = FALSE;
}

/*
// Initialize the text flow.
*/

void CTextFlow::InitializeFlow(void)
{
	if (m_lStartCharacter == -1)
	{
		// Start at front of frame.
		m_lStartCharacter = m_pTextState->m_lCharacter;
	}

	// Move to the line containing this character.
	FindLineOfCharacter(m_lStartCharacter);

	// Back up a line.
	BackupALine();

	// We have not place the EOT word yet.
	m_fPlacedEOTWord = FALSE;

	// Start at the top of the frame.
	InitializeFrame();

	// We source characters until past m_lEndCharacter (then varies).
	m_nWordSource = WORD_SOURCE_Characters;

	// Setup the macro flow.
	if (m_fFlowingMacros)
	{
		m_nWordSource = WORD_SOURCE_Words;
		m_lMacrosToFlow = m_pTextRecord->MacroCount();
		m_lMacrosFlowed = 0;
	}

	// Setup source
	m_Source.m_pDatabase = m_pDatabase;
	m_Source.m_lFrame = m_lFrame;
	m_Source.m_lWord = m_lFirstWordInLine;

	// Setup destination.
	m_Destination.m_pDatabase = m_pDatabase;
	m_Destination.m_lFrame = m_lFrame;
	m_Destination.m_lWord = m_lFirstWordInLine;

	m_lCharacterInSourceWord = 0;
}

/*
// Initialize the flow within the current frame.
*/

void CTextFlow::InitializeFrame(void)
{
	// Get the frame's margins.
	m_Margins.x0 = InchesToPage(m_pFrameRecord->Margins().Left);
	m_Margins.y0 = InchesToPage(m_pFrameRecord->Margins().Top);
	m_Margins.x1 = InchesToPage(m_pFrameRecord->Margins().Right);
	m_Margins.y1 = InchesToPage(m_pFrameRecord->Margins().Bottom);

	// Get the frame's column data.

	if (m_nStretchMode != STRETCH_MODE_None)
	{
		m_nColumns = 1;
		m_ColumnWidth = m_FrameWidth;					// Original stretch dims.
	}
	else
	{
		m_pFrameRecord->GetColumnData(&m_nColumns, &m_ColumnSpacing, &m_ColumnWidth);
	}

	// Compute the style here.
	m_Iterator.SetPosition(m_lFirstCharacterInLine);

	// Compute the initial leading values.
	ComputeLeading(m_Iterator.Style());

	// Setup the column.
	m_nColumn = 0;
	InitializeColumn(TRUE);
}

/*
// Finish up the current frame.
*/

void CTextFlow::FinishFrame(BOOL fChopLines, BOOL fResetFrame /*=FALSE*/)
{
	if (fChopLines)
	{
		// Handle getting rid of the lines.
		if (fResetFrame)
		{
			m_lCurrentLine = 0;
		}
		while (m_lCurrentLine < m_pFrameRecord->NumberOfLines())
		{
			// Get the line.
			CTextLine* pLine = m_pFrameRecord->GetLine(m_lCurrentLine);

			// Refresh the line.
			RefreshLine(pLine, REFRESH_ERASE);

			// Delete the line
			m_pFrameRecord->DeleteLine(m_lCurrentLine);
		}

		// Handle getting rid of the words.
		if (fResetFrame)
		{
			if (m_pFrameRecord->NumberOfWords() != 0)
			{
				m_pFrameRecord->DeleteWord(0, m_pFrameRecord->NumberOfWords());
			}
		}
		else
		{
			/*
			// We need to move any remaining words to the next frame.
			// If there is no next frame, they are still deleted.
			// There are some very strong assumptions here:
			// (1) If there are words in this frame, then the source MUST be
			// pointing to first of them (accounting for m_lCharacterInSourceWord
			// being negative -> backed over the last word).
			// (2) m_lCharacterInSourceWord points to the start of the current
			// source word (after accounting for backed over words); i.e. it's
			// effectively zero.
			// (3) We will remain sync'ed by moving all remaining words to the
			// next frame and setting the source to point to the first word in
			// the next frame.
			*/

			WORD_COUNT lWordsToDelete = m_pFrameRecord->NumberOfWords() - m_lFirstWordInLine;

			if (lWordsToDelete > 0)
			{
				// We could test some of our assumptions above.
				// Maybe later...

				if (m_pFrameRecord->NextFrame() != 0)
				{
					// We need to move the remainder of the words to the next
					// frame. This will keep our source in sync.
					CFrameRecord* pNextFrame = (CFrameRecord*)m_pDatabase->get_record(m_pFrameRecord->NextFrame(), NULL, RECORD_TYPE_Frame);
					if (pNextFrame == NULL)
					{
						// Could not get it. We will ignore this error.
						ASSERT(FALSE);
					}
					else
					{
						WORD_COUNT lWordsLeft = lWordsToDelete;
						WORD_INDEX lWord = m_lFirstWordInLine;
						WORD_INDEX lDestWord = 0;
						while (lWordsLeft > 0)
						{
							WORD_COUNT lThisWords;
							CTextWord* pWord = m_pFrameRecord->GetWord(lWord, &lThisWords);
							if (lThisWords > lWordsLeft)
							{
								lThisWords = lWordsLeft;
							}

							pNextFrame->InsertWord(lDestWord, pWord, lThisWords);

							lWordsLeft -= lThisWords;
							lWord += lThisWords;
							lDestWord += lThisWords;
						}

						m_Source.m_lFrame = pNextFrame->Id();
						m_Source.m_lWord = 0;
						m_lCharacterInSourceWord = 0;

						if (pNextFrame->NumberOfLines() > 0)
						{
							pNextFrame->GetLine(0)->m_lWords += lWordsToDelete;
						}

						pNextFrame->release();
					}
				}

				// Now we can delete the remainder of the words in the frame.
				m_pFrameRecord->DeleteWord(m_lFirstWordInLine, lWordsToDelete);
			}
		}
	}
	else
	{
		m_lCurrentLine = m_lLastLineInColumn;
	}

	FinishColumn();
	m_pFrameRecord->UpdateRefreshOffsets();
}

/*
// Set the current column information.
// This will compute the currrent column left and right.
*/

void CTextFlow::InitializeColumn(BOOL fFirstTime)
{
	// Compute the line we are starting in.
	LINE_COUNT lLines = m_pFrameRecord->NumberOfLines();
	CTextLine* pLine = NULL;
	if (m_lCurrentLine > 0 && m_lCurrentLine != lLines)
	{
		pLine = m_pFrameRecord->GetLine(m_lCurrentLine);
	}

	// Compute the column and line in column.
	if (fFirstTime && pLine != NULL)
	{
		m_nColumn = pLine->m_nColumn;
		m_nLineInColumn = pLine->m_nLineInColumn;
	}
	else
	{
		// m_nColumn should already be set.
		m_nLineInColumn = 0;
		pLine = NULL;		/////
	}

	// Calculate the column left and right.
	m_ColumnLeft = m_ColumnWidth*m_nColumn;
	m_ColumnRight = m_ColumnLeft + m_ColumnWidth;

	m_ColumnLeft += (m_nColumn == 0) ? m_Margins.x0 : (m_ColumnSpacing/2);
	m_ColumnRight -= (m_nColumn == m_nColumns-1) ? m_Margins.x1 : (m_ColumnSpacing/2);

	// Compute the first known line in this column.
	m_lFirstLineInColumn = m_lCurrentLine - m_nLineInColumn;

	// Compute the last known line in this column.
	int nLineInColumn = m_nLineInColumn;
	for (m_lLastLineInColumn = m_lCurrentLine; m_lLastLineInColumn < lLines; m_lLastLineInColumn++, nLineInColumn++)
	{
		CTextLine* pLine = m_pFrameRecord->GetLine(m_lLastLineInColumn);
		if (pLine->m_nColumn != m_nColumn || pLine->m_nLineInColumn != nLineInColumn)
		{
			// Stop now.
			break;
		}
	}

	// Compute our extent.
	m_LastLinesExtent = -1;
	m_CurrentLinesExtent = ComputeLinesExtent();

	// Compute the starting top of cell.
	if (pLine == NULL)
	{
		// Compute the current top of cell.
		m_TopOfCell = ComputeTopLineOffset();
	}
	else
	{
		switch (m_nLeadingType)
		{
			case LEADING_lines:
			{
				// Relative spacing.
				m_TopOfCell = pLine->m_Baseline - pLine->m_Ascend;
				break;
			}
			case LEADING_points:
			{
				m_TopOfCell = pLine->m_Baseline - m_CellAscend;
				break;
			}
			default:
			{
				// Invalid leading type.
				ASSERT(FALSE);
				break;
			}
		}
	}
}

/*
// Finish up the current column.
*/

void CTextFlow::FinishColumn(void)
{
	// Cut it off right here.
	m_lLastLineInColumn = m_lCurrentLine;
	UpdateVerticalAlignment();
}

/*
// Compute the leading given the passed fixed leading value.
*/

void CTextFlow::ComputeLeading(const CTextStyle& Style)
{
	m_nLeadingType = Style.LeadingType();
	m_LeadingValue = Style.Leading();
	ASSERT((PAGE_RESOLUTION/72)*72 == PAGE_RESOLUTION);
	switch (m_nLeadingType)
	{
		case LEADING_lines:
		{
			m_CellHeight =
				m_CellAscend =
				m_CellDescend = 0;
			break;
		}
		case LEADING_points:
		{
			m_CellHeight = (PCOORD)MulFixed(m_LeadingValue, PAGE_RESOLUTION/72);
			m_CellDescend = m_CellHeight/4;
			m_CellAscend = m_CellHeight - m_CellDescend;
			break;
		}
		default:
		{
			ASSERT(FALSE);
			return;
		}
	}

}

/*
// Find the line containing an offset.
// The way this is currently written, if you specify a text offset off
// the end of the last frame, you get the imaginary line just past the
// end of the frame. Beware!
*/

void CTextFlow::FindLineOfCharacter(CHARACTER_INDEX lSearchCharacter)
{
	// Variables to be moved into the state upon success.

	LINE_INDEX lCurrentLine;
	WORD_INDEX lFirstWordInLine;
	CHARACTER_INDEX lFirstCharacterInLine;

	// Find the line that contains the starting offset.
	for (;;)
	{
		// Make sure we have the current frame info.

		GetFrame();

		// See if we need to back up a frame.
		//
		// It is assumed that a NULL text state implies there actually is
		// none (i.e. the frame is empty), not some kind of error condition.

		if (m_pTextState == NULL
				|| m_pTextState->m_lCharacter == -1
				|| lSearchCharacter < m_pTextState->m_lCharacter)
		{
			/// Back up a frame.
			DB_RECORD_NUMBER lPrevious = m_pFrameRecord->PreviousFrame();
			if (lPrevious == 0)
			{
				// Uh-oh! No previous frame.
				// We will start at this first frame. Set the parameters.
				lFirstCharacterInLine = m_pTextState->m_lCharacter;
				lFirstWordInLine = 0;
				lCurrentLine = 0;

				// Adjust the start character.
//				m_lStartCharacter = lFirstCharacterInLine;
				break;
			}
			else
			{
				// Move to the previous frame.
				ASSERT(lPrevious != m_lFrame);
				m_lFrame = lPrevious;
			}
		}
		else
		{
			// Check the lines in the current frame.
			lFirstCharacterInLine = m_pTextState->m_lCharacter;
			lFirstWordInLine = 0;

			LINE_COUNT lEnd = m_pFrameRecord->NumberOfLines();
			if (m_pFrameRecord->NextFrame() == 0)
			{
				// If there is no next frame, we want to stop in this frame.
				lEnd--;
			}

			for (lCurrentLine = 0;
				  lCurrentLine < lEnd;
				  lCurrentLine++)
			{
				CTextLine* m_pLine = m_pFrameRecord->GetLine(lCurrentLine);

				if (lSearchCharacter < lFirstCharacterInLine + m_pLine->m_lCharacters)
				{
				/* This is the line. */
					break;
				}
				// Update for next line.
				lFirstCharacterInLine += m_pLine->m_lCharacters;
				lFirstWordInLine += m_pLine->m_lWords;
			}

			if (lCurrentLine == 0 || lCurrentLine != m_pFrameRecord->NumberOfLines())
			{
				// We found the line.
				// Break now.
				break;
			}

			// We did not find the line in this frame.
			// Try to move to the next frame.
			DB_RECORD_NUMBER lNext = m_pFrameRecord->NextFrame();
			if (lNext == 0)
			{
				// Should not get here!
				ASSERT(FALSE);
				break;
			}
			else
			{
				BOOL fCanAdvance = FALSE;
				CFrameRecord* pNextFrame = m_pFrameRecord->LockNextFrame();
				if (pNextFrame != NULL)
				{
					if (pNextFrame->FirstCharacter() != -1)
					{
						// Text flow has reached next frame. We can advance.
						fCanAdvance = TRUE;
					}
					pNextFrame->release();
				}
				if (!fCanAdvance)
				{
					break;
				}
				// Move to the next frame.
				m_lFrame = lNext;
			}
		}
	}

	// Copy the current variables to the state.
	m_lCurrentLine = lCurrentLine;
	m_lFirstWordInLine = lFirstWordInLine;
	m_lFirstCharacterInLine = lFirstCharacterInLine;
}

/*
// Make the line traversal variables go back a line.
*/

void CTextFlow::BackupALine(void)
{
	for (;;)
	{
		if (m_lCurrentLine == 0)
		{
			// We need to go to the previous frame.
			DB_RECORD_NUMBER lPrevious = m_pFrameRecord->PreviousFrame();
			if (lPrevious != 0)
			{
				ASSERT(lPrevious != m_lFrame);

				m_lFrame = lPrevious;
				GetFrame();

				ASSERT(m_pTextState != NULL);

				m_lFirstCharacterInLine = m_pTextState->m_lCharacter;
				m_lFirstWordInLine = 0;

			// Skip down to the last line in the frame.
				for (m_lCurrentLine = 0;
					  m_lCurrentLine < m_pFrameRecord->NumberOfLines()-1;
					  m_lCurrentLine++)
				{
					CTextLine* pLine = m_pFrameRecord->GetLine(m_lCurrentLine);
					ASSERT(pLine != NULL);
					m_lFirstCharacterInLine += pLine->m_lCharacters;
					m_lFirstWordInLine += pLine->m_lWords;
				}
			}
			else
			{
				// Nowhere to go.
				break;
			}
		}
		else
		{
			// Back up a line in this frame.
			
			CTextLine* pLine = m_pFrameRecord->GetLine(--m_lCurrentLine);
			ASSERT(pLine != NULL);

			m_lFirstCharacterInLine -= pLine->m_lCharacters;
			m_lFirstWordInLine -= pLine->m_lWords;
		}
		if (m_lCurrentLine >= m_pFrameRecord->NumberOfLines()
				|| (m_pFrameRecord->GetLine(m_lCurrentLine))->m_lCharacters != 0)
		{
			break;
		}
	}
}

/*
// Flow the next line of text.
*/

void CTextFlow::FlowLine(void)
{
	// Start the line. This will save any information necessary to restart
	// the line if we need to.

	StartLine();

	// Construct the display word list for this line.
	while (!m_fLineComplete && m_ExtentArray.GetSize() != 0)
	{
		// Loop along the line until done.

		FlowWord();

		if (m_fRestartLine)
		{
			RestartLine();
		}
	}

	// Position the display words in the line.

	ApplyLineAttributes();

	// Add the display words into the current line.

	BOOL fLineChanged = MoveWordsToLine();

	// Update the vertical alignment if necessary.

	BOOL fAlignmentChanged = UpdateVerticalAlignment();

	// If we ran out of words, stop now.

	if (m_fWordsExhausted)
	{
		SetStatus(STATUS_Done);
	}

	// See if we can stop.

	if (m_lEndCharacter != -1
			&& m_lCharacter > m_lEndCharacter
			&& !fLineChanged
			&& !fAlignmentChanged)
	{
		TRACE("*** Optimizing finish (%ld to %ld at %ld). ***\n",
				m_lStartCharacter, m_lEndCharacter, m_lCharacter);
		SetStatus(STATUS_Done);
	}

	// Move to the next line.

	MoveToNextLine();
}

/*
// Restart the line.
*/

void CTextFlow::RestartLine(void)
{
	// Setup to flow the line.

	m_LeftOffset = 0;

	m_WordArray.RemoveAll();		// Remove all words in the array.

	m_Baseline = m_TopOfCell + m_CellAscend;
	m_lCharacter = m_lFirstCharacterInLine;

	// update drop cap status before re-flowing the line
	m_Iterator.SetPosition( m_lCharacter );
	// drop caps and bullets are mutually exclusive, if both exsist, apply bullets
	m_DropLines = m_Iterator.Style().BulletType()== BULLET_none ? m_Iterator.Style().DropLines() : 0;
	// update the current line in paragraph counter before calling SetDropDimensions
	if( m_nLineInParagraph == -1 )	
		UpdateLineInParagraph();
	else if( m_Iterator.CharacterInParagraph() == 0 )
		m_nLineInParagraph = 0;
	// set drop dimensions before computing flow extents because fow extents are modified by drop caps
	SetDropDimensions();
	
	m_fExtentClipped = FALSE;

	ComputeFlowExtents();

	SetBaseWordEmpty();

	// Reset the word source.
	m_Source.m_lFrame = m_lSourceFrameSave;
	m_Source.m_lWord = m_lSourceWordSave;
	m_Source.GetFrame();
	m_lCharacterInSourceWord = m_lSourceCharacterSave;
	m_nWordSource = m_nWordSourceSave;

	m_fRestartLine = FALSE;
	m_fLineComplete = FALSE;

	m_fAddedWordsToLine = FALSE;

	// No current tab, yet.
	m_nCurrentTab = -1;

	// Do some special stretch text stuff here.
	m_lWordsToFlow = -1;							// Assume nothing special.
	if (m_nStretchMode == STRETCH_MODE_Size)
	{
		// Get the number of words to flow on this line.
		ASSERT(m_lCurrentLine < m_pFrameRecord->NumberOfLines());
		if (m_lCurrentLine < m_pFrameRecord->NumberOfLines())
		{
			CTextLine* pLine = m_pFrameRecord->GetLine(m_lCurrentLine);
			m_lWordsToFlow = pLine->m_lWords;
		}
	}
}

/*
// Start the line.
// This does appropriate first-time initialization, then calls RestartLine().
*/

void CTextFlow::StartLine(void)
{
	// First-time initialization.
	m_LineAscend = 0;
	m_LineDescend = 0;

	// Save the source line parameters.

	m_lSourceFrameSave = m_Source.m_lFrame;
	m_lSourceWordSave = m_Source.m_lWord;
	m_lSourceCharacterSave = m_lCharacterInSourceWord;
	m_nWordSourceSave = m_nWordSource;

	// Remember the line parameters.
	m_Iterator.SetPosition(m_lFirstCharacterInLine);
	m_nLineAlignment = m_Iterator.Style().Alignment();
	m_SpaceAfter = m_Iterator.Style().SpaceAfter();

	// Compute the cell metrics.
	ComputeLeading(m_Iterator.Style());

	// Restart the line.
	RestartLine();
}

void CTextFlow::FlowWord(void)
{
	// Get the next word to insert.
	GetNextWord();

	// Update any currently active tab with the new word.
	UpdateCurrentTab();

	if (m_fWordsExhausted
				|| m_CurrentWord.m_wType == WORD_TYPE_break)
	{
		m_fLineComplete = TRUE;

		// If it is a break word, add it to the line.
		if (m_CurrentWord.m_wType == WORD_TYPE_break)
		{
			// Add the break word to the line.
			if (WordFitsVertically())
			{
				// Add it to the line...
				AddWordToLine();

				// See if we should flow to the next line.
				ASSERT(m_WordArray.GetSize() > 0);
				CTextWord* pLastWord = (CTextWord*)m_WordArray.GetAt(m_WordArray.GetSize()-1);
				ASSERT(pLastWord->m_wType == WORD_TYPE_break);
				if (m_WordArray.GetSize() >= 2
					 && m_pTextFlowExtent != NULL
					 && pLastWord->m_XOffset == m_pTextFlowExtent->m_ExtentLeft)
				{
					// If:
					// (1) we just added a break word.
					// (2) there are at least 2 words in the line.
					// (3) the break word we just added is at the start of the line.
					// then we stay on this line.
					// This handles empty macro characters.
					m_fLineComplete = FALSE;
				}
			}
			else
			{
				// Enlarge the line...
				EnlargeLine();
				// And restart.
				m_fRestartLine = TRUE;
			}
		}
	}
	else
	{
		// See if the word fits horizontally in the line.
		if (!WordFitsHorizontally())
		{
			// The word does not fit horizontally.
			// Try to break the word. If that does not work, move it.

			if (!BreakWord())
			{
				// Make sure we know it was broken.
				MoveWord();
				return;
			}
		}

		// The word fits horizontally.
		// If the word fits vertically, then add it to the line.
		// Otherwise, enlarge the line to make it fit and restart.

		if (WordFitsVertically())
		{
			// Add it to the line...
			AddWordToLine();
		}
		else
		{
			// Enlarge the line...
			EnlargeLine();
			// And restart.
			m_fRestartLine = TRUE;
		}
	}
}

/*
// Get the next word to flow from the text stream.
//
// What do we need for a display word while flowing? We need everything in the
// normal CTextWord.
*/

void CTextFlow::GetNextWord(void)
{
	// Get the next base word if we do not have one yet.
	if (IsBaseWordEmpty())
	{
		// Ask for the next word.
		GetBaseWord();
	}

	// Copy the base word to the current word (empty or not).
	m_CurrentWord = m_BaseWord;
}

/*
// Get the next base word.
//
// A word can come from different places:
// (1) It can be built up by parsing characters into words.
// (2) It can be extracted from existing parsed text words.
*/

void CTextFlow::GetBaseWord(CHARACTER_COUNT nCharacters /*=0*/)
{
	// Parse the next word from the character stream.
	CHARACTER_COUNT lCharacters = m_pTextRecord->NumberOfCharacters();

	// If we are sourcing words, grab the next word.
	if (m_nWordSource == WORD_SOURCE_Words)
	{
		if (nCharacters == 0 && m_Source.m_pFrame != NULL && m_lCharacterInSourceWord == 0)
		{
			ASSERT(m_Source.m_lWord < m_Source.m_pFrame->NumberOfWords());
			// Look at the next word.
			CTextWord* pWord = GetSourceWord();
			// If this word does not need building, use it.
			if (pWord != NULL
					&& pWord->m_wType != WORD_TYPE_tab		// Always rebuild
					&& pWord->m_wType != WORD_TYPE_eot
					&& !(pWord->m_wFlags & WORD_FLAG_needs_building)
					&& !(pWord->m_wFlags & WORD_FLAG_has_page_macro)	// always!
					&& (!m_fFlowingMacros || !(pWord->m_wFlags & WORD_FLAG_has_macro)))
			{
				ASSERT(pWord->m_lCharacters > 0);

				FLAGS wFlags = pWord->m_wFlags;

				memset(&m_BaseWord, 0, sizeof(m_BaseWord));
				m_BaseWord = *pWord;
            // Turn off all flags except macro one.
				m_BaseWord.m_wFlags = (m_BaseWord.m_wFlags & WORD_FLAG_macro_mask);
				m_BaseWord.m_XOffset = 0;

				NextSourceWord();

				while (m_Source.m_pFrame != NULL && (wFlags & WORD_FLAG_broken))
				{
					pWord = GetSourceWord();
					if (pWord == NULL || pWord->m_wType != m_BaseWord.m_wType)
					{
						// Some kind of flow error.
						break;
					}
					// Merge the words.
					MergeWords(&m_BaseWord, pWord);

					// Grab the words.
					wFlags = pWord->m_wFlags;

					// Preserve macro status if merging words.
					m_BaseWord.m_wFlags |= (wFlags & WORD_FLAG_macro_mask);

					// Move to the next source word.
					NextSourceWord();
				}

				if (!(wFlags & WORD_FLAG_broken))
				{
					if (m_lCharacter + m_BaseWord.m_lCharacters <= lCharacters)
					{
						// Successful word grab. Advance character position.
						m_lCharacter += m_BaseWord.m_lCharacters;
						// Go back with the word.
						return;
					}
				}
			}
		}

		// Enforce this for next time.
		m_nWordSource = WORD_SOURCE_Characters;
	}

	// Grab the next source word by characters.
	// We still need to maintain the "source word" variables since we will
	// want to switch back to grabbing whole words whenever we can.

	if (nCharacters == 0)
	{
		// Empty the base word before beginning.
		SetBaseWordEmpty();
	}

	if (m_lCharacter == lCharacters)
	{
		if (m_fPlacedEOTWord)
		{
			// All done!
			m_fWordsExhausted = TRUE;
		}
		else
		{
			// At End-Of-Text.

			// Update the style to what we have here.
			m_Iterator.SetPosition(m_lCharacter);

			// Setup the base word.
			memset(&m_BaseWord, 0, sizeof(CTextWord));
			m_BaseWord.m_wType = WORD_TYPE_eot;

			// Fill in the metrics. All the rest are zero.
			m_BaseWord.m_Ascend = m_Iterator.Style().Ascend();
			m_BaseWord.m_Descend = m_Iterator.Style().Descend();
		}
	}
	else
	{
		FontServer* pFontServer = m_pDatabase->get_font_server();

		// Set up the word we will be building.
		CTextWord CurrentWord;

		if (nCharacters == 0 || IsBaseWordEmpty())
		{
			// Zero sets good defaults for most things in the word.
			memset(&CurrentWord, 0, sizeof(CurrentWord));
			CurrentWord.m_wType = (WORD)WORD_TYPE_unknown;
		}
		else
		{
			// Continue on with the base word.
			CurrentWord = m_BaseWord;
		}

		CTextWord* pSourceWord = GetSourceWord();

		for (;;)
		{
			// If we are done, stop now.
			if (m_lCharacter == lCharacters)
			{
				break;
			}

			// Update the style to what we have here.
			m_Iterator.SetPosition(m_lCharacter);
			PCOORD Ascend = m_Iterator.Style().Ascend();
			PCOORD Descend = m_Iterator.Style().Descend();
			PCOORD FontXMin = m_Iterator.Style().FontXMin();
			PCOORD FontXMax = m_Iterator.Style().FontXMax();
			PCOORD Temp;

			// Grab the next character.
			CHARACTER c = *(m_pTextRecord->GetCharacter(m_lCharacter));

			// We do not want tabs in stretching mode. Convert tabs to spaces.
			if (c == '\t' && m_nStretchMode != STRETCH_MODE_None)
			{
				c = ' ';
			}

			// Determine its type.
			WORD wType;

			if( m_DropLines > 1 && m_Iterator.CharacterInParagraph() == 0 )
			{
				wType = WORD_TYPE_dropcap;
			}
			else if (c == ' ')
			{
				wType = WORD_TYPE_space;
			}
			else if (c == '\t')
			{
				wType = WORD_TYPE_tab;
			}
			else if (c == '\n')
			{
				wType = WORD_TYPE_break;
			}
			else
			{
				if (c >= MACRO_CHARACTER)
				{
					CurrentWord.m_wFlags |= WORD_FLAG_has_macro;
				}
				if (c == MACRO_CHARACTER_Page)
				{
					CurrentWord.m_wFlags |= WORD_FLAG_has_page_macro;
				}
				wType = WORD_TYPE_solid;
			}

			if (CurrentWord.m_wType == (WORD)WORD_TYPE_unknown)
			{
				// First character in word.
				CurrentWord.m_wType = wType;
				CurrentWord.m_Ascend = Ascend;
				CurrentWord.m_Descend = Descend;
				if (FontXMin < 0)
				{
					CurrentWord.m_DrawLeft = FontXMin;
				}
				if (FontXMax > 0)
				{
					CurrentWord.m_DrawWidth = FontXMax;
				}
			}
			else if (wType == CurrentWord.m_wType)
			{
				// Same type. Not first character in word.
				if (Ascend > CurrentWord.m_Ascend)
				{
					CurrentWord.m_Ascend = Ascend;
				}
				if (Descend > CurrentWord.m_Descend)
				{
					CurrentWord.m_Descend = Descend;
				}
			}
			else
			{
				// Different type. End of word.
				break;
			}

			// We grabbed another character.
			// Synchronize with the source word.
			if (pSourceWord != NULL)
			{
				if (++m_lCharacterInSourceWord >= pSourceWord->m_lCharacters)
				{
					ASSERT(m_lCharacterInSourceWord == pSourceWord->m_lCharacters);
					NextSourceWord();
					pSourceWord = GetSourceWord();
				}
			}

			m_lCharacter++;
			CurrentWord.m_lCharacters++;

			if (CurrentWord.m_wType == WORD_TYPE_break)
			{
				// Handle a break word.
				CurrentWord.m_Width = 0;
				CurrentWord.m_DrawLeft = 0;
				CurrentWord.m_DrawWidth = 0;
				break;
			}
			else if (CurrentWord.m_wType == WORD_TYPE_tab)
			{
				// Handle a tab.
				PBOX Extent;
				Extent.x0 = m_ColumnLeft;
				Extent.y0 = 0;
				Extent.x1 = m_ColumnRight;
				Extent.y1 = m_FrameHeight;

				PCOORD CurrentPosition = m_ColumnLeft;
				if (m_pTextFlowExtent != NULL)
				{
					CurrentPosition = (m_pTextFlowExtent->m_ExtentRight
													- m_pTextFlowExtent->m_UnusedWidth);
				}

				CTextTab Tab;
				if (m_Iterator.Style().m_Tabs.FindTab(Extent,
																	 CurrentPosition,
																	 &Tab))
				{
					CurrentWord.m_Width = Tab.ComputePosition(Extent, CurrentPosition) - CurrentPosition;
					// Reset the current tab. It will be set when the tab is added
					// to the line.
					m_nCurrentTab = -1;
					m_lCurrentTabEnd = m_lCharacter;
					m_CurrentTab = Tab;
					m_CurrentTabStartWidth = CurrentWord.m_Width;
				}
				else
				{
					CurrentWord.m_Width = 0;
				}
				CurrentWord.m_DrawLeft = 0;
				CurrentWord.m_DrawWidth = CurrentWord.m_Width;
				break;
			}
			else if( CurrentWord.m_wType == WORD_TYPE_dropcap )
			{
				// set drop cap word values
				CurrentWord.m_Width = m_DropWidth;
				CurrentWord.m_DrawLeft = 0;
				CurrentWord.m_DrawWidth = m_DropWidth;

				CTextStyle style = m_Iterator.Style();
				style.Size( m_DropSize );

				// Update DrawLeft
				Temp = CurrentWord.m_Width + style.FontXMin();
				if (Temp < CurrentWord.m_DrawLeft)
				{
					CurrentWord.m_DrawLeft = Temp;
				}

				// Update DrawWidth
				Temp = CurrentWord.m_Width + style.FontXMax();
				if (Temp > CurrentWord.m_DrawWidth)
				{
					CurrentWord.m_DrawWidth = Temp;
				}

				break;
			}

			// Solid or white letter.
			ASSERT(m_pFrameRecord != NULL);
			CurrentWord.m_Width += pFontServer->GetAdvanceWidth(&m_Iterator.Style(), c, m_pFrameRecord->Page());

			// Update DrawLeft
			Temp = CurrentWord.m_Width + FontXMin;
			if (Temp < CurrentWord.m_DrawLeft)
			{
				CurrentWord.m_DrawLeft = Temp;
			}

			// Update DrawWidth
			Temp = CurrentWord.m_Width + FontXMax;
			if (Temp > CurrentWord.m_DrawWidth)
			{
				CurrentWord.m_DrawWidth = Temp;
			}

			if (nCharacters != 0)
			{
				if (--nCharacters == 0)
				{
					// Got all we came for.
					break;
				}
			}
		}

		// Now stick it in the base word.
		m_BaseWord = CurrentWord;

		// Switch to whole words next time if the current state satisfies the
		// necessary conditions.

		if (m_lCharacter != lCharacters
				&& m_lCharacterInSourceWord == 0
				&& (m_fFlowingMacros
						|| (m_lEndCharacter != -1 && m_lCharacter > m_lEndCharacter))
				&& m_Source.m_pFrame != NULL)
		{
			m_nWordSource = WORD_SOURCE_Words;
		}
	}
}

/*
// Merge a word into the passed word.
*/

void CTextFlow::MergeWords(CTextWord* pWord, CTextWord* pOtherWord)
{
	// Number of characters.
	pWord->m_lCharacters += pOtherWord->m_lCharacters;

	// Horizontal metrics.
	if (pWord->m_DrawLeft > pWord->m_Width + pOtherWord->m_DrawLeft)
	{
		pWord->m_DrawLeft = pWord->m_Width + pOtherWord->m_DrawLeft;
	}
	if (pWord->m_DrawWidth < pWord->m_Width + pOtherWord->m_DrawWidth)
	{
		pWord->m_DrawWidth = pWord->m_Width + pOtherWord->m_DrawWidth;
	}
	pWord->m_Width += pOtherWord->m_Width;

	// Vertical metrics.
	if (pWord->m_Ascend < pOtherWord->m_Ascend)
	{
		pWord->m_Ascend = pOtherWord->m_Ascend;
	}
	if (pWord->m_Descend < pOtherWord->m_Descend)
	{
		pWord->m_Descend = pOtherWord->m_Descend;
	}

	// Flags.
	pWord->m_wFlags |= (pOtherWord->m_wFlags & ~WORD_FLAG_broken);
}

/*
// Compute the flow extents given the current line.
*/

void CTextFlow::ComputeFlowExtents(void)
{
	// Remove any extents we currently have. 
	m_ExtentArray.RemoveAll();

	// Compute the extents for the current line.

	BOOL fDoAdd = TRUE;
	BOOL fTentativeAdd = FALSE;

	// If the bottom line is clipped, we will attempt to abort this column's
	// flow if we have a new place to go (like a new column or frame), and
	// assuming we're not doing something special like stretching.
	// account for drop caps at the bottom of the frame, try to push them to the next frame
	if (Max( m_Baseline, m_DropBaseline ) + m_LineDescend > m_FrameHeight)
	{
		// Bottom line is clipped!
		if (m_nStretchMode == STRETCH_MODE_None
			&& (m_pFrameRecord->NextFrame() != 0 || m_nColumn < m_nColumns-1))
		{
			if (m_pFrameRecord->VerticalAlignment() == ALIGN_top)
			{
				fDoAdd = FALSE;
			}
			else
			{
				// The line is vertically clipped.
				// If there is enough space in the frame, allow the extent to proceed.
				// The assumption is that space in the frame implies a vertical
				// alignment which will use the space once this line is added.
				// Else, do not add any extents.

				// Compute the extent of the current lines.
				PCOORD Top;
				if (m_pFrameRecord->NumberOfLines() > m_lFirstLineInColumn)
				{
					CTextLine* pLine = m_pFrameRecord->GetLine(m_lFirstLineInColumn);
					Top = pLine->m_Baseline - pLine->m_Ascend;
				}
				else
				{
					Top = m_Baseline - m_LineAscend;
				}

				// Compute including this line.
				PCOORD Bottom = m_Baseline + m_LineDescend;

				// See how much space we have.
				fDoAdd = (m_FrameHeight - (m_FirstLineSpace + (Bottom - Top))) >= 0;
			}
		}
		else
		{
			// We are off the bottom of the frame with nowhere else to go.
			// We really want to know if there is room to flow. If not,
			// then we want to stop. Otherwise, the textflow code will go
			// on forever.
			fTentativeAdd = TRUE;
		}
	}

	if (fDoAdd)
	{
		GenerateTextFlowExtents();

		/*
		// The computed flow extents have not been clipped to the various indents
		// possible for the current paragraph:
		//
		//    FirstLineIndent  -  indent of first line relative to body indent
		//    LeftMargin       -  left margin relative to left edge of frame
		//    RightMargin      -  right margin relative to right edge of frame
		//		Bullets			  -  text indent for bulleted paragraphs
		//		Drop caps		  -  text indent for drop cap
		//
		// These values are stored in the current style.
		*/

		// Update the style to what we have here.
		m_Iterator.SetPosition(m_lCharacter);

		// Compute the clipping values based on left, right and first line indent

		PCOORD ClipX0 = m_ColumnLeft;
		PCOORD ClipX1 = m_ColumnRight;

		CFixed X0Adjust = m_Iterator.Style().LeftMargin();
		CFixed X1Adjust = m_Iterator.Style().RightMargin();

		if (m_Iterator.CharacterInParagraph() == 0)
		{
			X0Adjust += m_Iterator.Style().FirstLineIndent();
		}
		// Convert to PCOORD from CFixed (MakeFixed(1) == 1 inch).
		ClipX0 += InchesToPage(X0Adjust);
		ClipX1 -= InchesToPage(X1Adjust);

		if( m_Iterator.Style().BulletType() != BULLET_none )
		{
			ClipX0 += m_Iterator.Style().BulletIndent();
			m_LeftOffset += -m_Iterator.Style().BulletIndent();
		}
		else
			m_LeftOffset = 0;

		// Now clip each extent.
		for (int nExtent = 0; nExtent < m_ExtentArray.GetSize(); )
		{
			// this will get flow extents around avoided objects
			CTextFlowExtent* pExtent = GetTextFlowExtent(nExtent);
			ASSERT(pExtent != NULL);
			if (pExtent != NULL)
			{
				if (pExtent->m_ExtentLeft < ClipX0)
				{
					// this assignment igonores the left, 1st line indent amount and just takes
					// the greater of the two.
					pExtent->m_ExtentLeft = ClipX0;
				}
				// if we are within a drop cap adjust the left margin
				// only adjust for lines after the first in the paragraph
				// because the width of drop cap in the word WORD_type_dropcap 
				if( m_nLineInParagraph > 0  && m_nLineInParagraph < m_DropLines )
					pExtent->m_ExtentLeft += m_DropWidth;

				if (pExtent->m_ExtentRight > ClipX1)
				{
					pExtent->m_ExtentRight = ClipX1;
				}
				if (((pExtent->m_UnusedWidth = pExtent->m_ExtentRight - pExtent->m_ExtentLeft) < 0)
						&& !(m_dwFrameFlags & FRAME_FLAG_stretch_frame))
				{
					// The extent is totally clipped. Remove it.
					m_ExtentArray.RemoveAt(nExtent);
				}
				else
				{
					// Advance to the next one.
					nExtent++;
				}
			}
		}

		if (fTentativeAdd && m_ExtentArray.GetSize() == 0)
		{
			// We are off the bottom of the last frame/column with no
			// place to put text. We want to stop now!
			m_fExtentClipped = TRUE;
		}
	}
	else
	{
		m_fExtentClipped = TRUE;
	}

	// Start us off at the first flow extent.
	SetCurrentFlowExtent(0);
}

/*
// Set the current flow extent.
// m_pTextFlowExtent will be NULL when this is done if we ran out of extents.
*/

void CTextFlow::SetCurrentFlowExtent(int nExtent)
{
	m_pTextFlowExtent = GetTextFlowExtent(m_nCurrentFlowExtent = nExtent);
}

/*
// Add a text flow extent to the current line.
*/

void CTextFlow::AddTextFlowExtent(PCOORD Left, PCOORD Right)
{
	CTextFlowExtent Extent;

	Extent.m_ExtentLeft = Left;
	Extent.m_ExtentRight = Right;
	Extent.m_UnusedWidth = Right-Left;
	Extent.m_nFirstWord = -1;
	Extent.m_nNumberOfWords = 0;

	m_ExtentArray.Add(&Extent);
}

/*
// Get a text flow extent.
*/

CTextFlowExtent* CTextFlow::GetTextFlowExtent(int nExtent)
{
	if (nExtent >= m_ExtentArray.GetSize())
	{
		ASSERT(nExtent == m_ExtentArray.GetSize());
		// That extent is not valid. This will happen during normal flow.
		return NULL;
	}

	return (CTextFlowExtent*)m_ExtentArray.GetAt(nExtent);
}

/*
// Update any pending tabs with this new word.
*/

void CTextFlow::UpdateCurrentTab(void)
{
	if (m_nCurrentTab != -1 && m_CurrentTab.m_nType != CTextTab::TAB_left)
	{
		// We have a current tab that needs adjusting.
		// Compute the width of the word.
		PCOORD Adjust = 0;
		switch (m_CurrentTab.m_nType)
		{
			case CTextTab::TAB_left:
			{
				break;
			}
			case CTextTab::TAB_center:
			case CTextTab::TAB_right:
			{
				for (int nWord = m_nCurrentTab+1; nWord < m_WordArray.GetSize(); nWord++)
				{
					CTextWord* pWord = (CTextWord*)m_WordArray.GetAt(nWord);
					Adjust += pWord->m_Width;
				}
				Adjust += m_CurrentWord.m_Width;
				if (m_CurrentTab.m_nType == CTextTab::TAB_center)
				{
					Adjust /= 2;
				}
				break;
			}
			case CTextTab::TAB_decimal:
			{
				CHARACTER cDecimal = m_CurrentTab.m_DecimalCharacter;

				// Look for the decimal character in the words after the tab.
				// If the decimal is not found, the words are right aligned.
				CHARACTER_INDEX lSearch = m_lCurrentTabEnd;
				BOOL fFound = FALSE;

				for (int nWord = m_nCurrentTab+1; nWord <= m_WordArray.GetSize(); nWord++)
				{
					// Grab the next word. We do a trick to include the current
					// word in the loop.

					CTextWord* pWord;
					if (nWord < m_WordArray.GetSize())
					{
						pWord = (CTextWord*)m_WordArray.GetAt(nWord);
					}
					else
					{
						pWord = &m_CurrentWord;
					}

					// Search for the decimnal character in the this word.
					for (CHARACTER_COUNT l = 0; l < pWord->m_lCharacters; l++, lSearch++)
					{
						if (*(m_pTextRecord->GetCharacter(lSearch)) == cDecimal)
						{
							// We found it!
							fFound = TRUE;
							// Grab the font server so we can compute metrics.
							FontServer* pFontServer = m_pDatabase->get_font_server();
							// Compute the width up to the decimal character.
							for (lSearch -= l; l >= 0; l--, lSearch++)
							{
								CHARACTER c = *(m_pTextRecord->GetCharacter(lSearch));
								m_Iterator.SetPosition(lSearch);

								ASSERT(m_pFrameRecord != NULL);
								PCOORD Width = pFontServer->GetAdvanceWidth(&m_Iterator.Style(), c, m_pFrameRecord->Page());
								if (l == 0)
								{
									// Add only half the decimal character width.
									Width /= 2;
								}
								Adjust += Width;
							}
							break;
						}
					}

					if (fFound)
					{
						break;
					}
					// We did not find the character, skip the whole word.
					Adjust += pWord->m_Width;
				}
				break;
			}
			default:
			{
				break;
			}
		}

		if (Adjust != 0)
		{
			if (Adjust > m_CurrentTabStartWidth)
			{
				Adjust = m_CurrentTabStartWidth;
			}
			CTextWord* pTabWord = (CTextWord*)m_WordArray.GetAt(m_nCurrentTab);
			ASSERT(pTabWord != NULL);
			pTabWord->m_Width = m_CurrentTabStartWidth - Adjust;
			PCOORD UnusedWidth = m_CurrentTabUnusedWidth + Adjust;
			PCOORD XOffset = pTabWord->m_XOffset + pTabWord->m_Width;
			// Now readjust all following words.
			for (int nWord = m_nCurrentTab+1; nWord < m_WordArray.GetSize(); nWord++)
			{
				CTextWord* pWord = (CTextWord*)m_WordArray.GetAt(nWord);
				pWord->m_XOffset = XOffset;
				XOffset += pWord->m_Width;
				UnusedWidth -= pWord->m_Width;
			}
			ASSERT(m_pTextFlowExtent != NULL);
			m_pTextFlowExtent->m_UnusedWidth = UnusedWidth;
		}
	}
}

/*
// Does the current word fit vertically in the line?
*/

BOOL CTextFlow::WordFitsVertically(void)
{
	return (m_CurrentWord.m_Ascend <= m_LineAscend
			  && m_CurrentWord.m_Descend <= m_LineDescend);
}

/*
// Does the current word fit horizontally in the line?
*/

BOOL CTextFlow::WordFitsHorizontally(void)
{
	// No flow extent automatically means it does not fit.
	if (m_pTextFlowExtent == NULL)
	{
		return FALSE;
	}

	// If the frame will be stretching, then words always fit.
	if (m_dwFrameFlags & FRAME_FLAG_stretch_frame)
	{
		return TRUE;
	}

	// Normal case.
	if (m_CurrentWord.m_wType == WORD_TYPE_break
		 || m_CurrentWord.m_wType == WORD_TYPE_eot)
	{
		if (m_lWordsToFlow > 0)
		{
			m_lWordsToFlow--;
		}
		return TRUE;
	}

	// If we only flow a certain number of words, check if we are done.
	if (m_lWordsToFlow != -1)
	{
		if (m_lWordsToFlow == 0)
		{
			return FALSE;
		}
		else
		{
			m_lWordsToFlow--;
			return TRUE;
		}
	}

	// Make sure the current word fits completely in the extent.
	return m_CurrentWord.m_Width <= m_pTextFlowExtent->m_UnusedWidth;
}

/*
// Add the current word to the line.
*/

void CTextFlow::AddWordToLine(void)
{
	if (m_pTextFlowExtent == NULL)
	{
		// No flow extent to add word to!
		ASSERT(FALSE);
	}
	else
	{
		// See if there is a current word.
		// If not, simply return.
		if (m_CurrentWord.m_wType == (WORD)WORD_TYPE_unknown)
		{
			ASSERT(FALSE);
			return;
		}

		// Add the word to the flow extent.
		if (m_pTextFlowExtent->m_nNumberOfWords == 0)
		{
			m_pTextFlowExtent->m_nFirstWord = m_WordArray.GetSize();
		}

		m_CurrentWord.m_XOffset = (m_pTextFlowExtent->m_ExtentRight-m_pTextFlowExtent->m_ExtentLeft)
											- m_pTextFlowExtent->m_UnusedWidth
											+ m_pTextFlowExtent->m_ExtentLeft;

		m_WordArray.Add(&m_CurrentWord);

		m_pTextFlowExtent->m_nNumberOfWords++;
		m_pTextFlowExtent->m_UnusedWidth -= m_CurrentWord.m_Width;

		// If we are placing a tab, remember that fact.

		if (m_CurrentWord.m_wType == WORD_TYPE_tab && m_CurrentWord.m_Width != 0)
		{
			// Remember the index of the tab. This should have been reset at the
			// time that the tab was found.
			ASSERT(m_nCurrentTab == -1);
			m_nCurrentTab = m_WordArray.GetSize()-1;
			ASSERT(m_CurrentTabStartWidth == m_CurrentWord.m_Width);
			// Remember the unused width as we will be adjusting it as we add
			// words past the tab.
			m_CurrentTabUnusedWidth = m_pTextFlowExtent->m_UnusedWidth;
		}

		// If we are placing the EOT word, remember that fact.

		else if (m_CurrentWord.m_wType == WORD_TYPE_eot)
		{
			m_fPlacedEOTWord = TRUE;
			m_fWordsExhausted = TRUE;
		}

		SetBaseWordEmpty();
	}
}

/*
// Enlarge the line to make the current word fit.
*/

void CTextFlow::EnlargeLine(void)
{
	// Enlarge the line to accomodate this word.
	if (m_LineAscend < m_CurrentWord.m_Ascend)
	{
		m_LineAscend = m_CurrentWord.m_Ascend;
	}
	if (m_LineDescend < m_CurrentWord.m_Descend)
	{
		m_LineDescend = m_CurrentWord.m_Descend;
	}

	switch (m_nLeadingType)
	{
		case LEADING_lines:
		{
			// Cell is now this big as well.
			m_CellAscend = m_LineAscend;
			m_CellDescend = m_LineDescend;
			m_CellHeight = m_CellAscend + m_CellDescend;
			break;
		}
		default:
		{
			break;
		}
	}
}

/*
// Break the current word into a smaller word that fits in the extent.
*/

BOOL CTextFlow::BreakWord(void)
{
//	TRACE("Break word (type %d), Unused Width: %ld\n",
//			(int)m_CurrentWord.m_wType, m_pTextFlowExtent->m_UnusedWidth);

	// See if there is even any room in the extent.
	if (m_pTextFlowExtent == NULL
		 || m_pTextFlowExtent->m_UnusedWidth <= 0
		 || m_CurrentWord.m_wType == (WORD)WORD_TYPE_unknown)		// Or no word...
	{
		// No room left in the extent.
		return FALSE;
	}
	ASSERT(m_CurrentWord.m_lCharacters != 0);

	CHARACTER_COUNT lFullCharacters = m_CurrentWord.m_lCharacters;

	if (m_CurrentWord.m_wType == WORD_TYPE_solid || m_CurrentWord.m_wType == WORD_TYPE_dropcap)
	{
		// This is a solid word.
		// If the word fits across the whole extent, then we want to force
		// a break in it (unless we are stretching).

		BackUpAWord();

		if (m_lCharacter == m_lFirstCharacterInLine
			 && m_pTextFlowExtent->m_UnusedWidth == m_pTextFlowExtent->m_ExtentRight -m_pTextFlowExtent->m_ExtentLeft)
		{
			// Word is full extent.

			// If we're stretching, we don't break solid words.
			if (m_nStretchMode != STRETCH_MODE_None)
			{
				// Use full word! Pretend we broke it.
				GetBaseWord();
				GetNextWord();
				return TRUE;
			}

			// Word is full extent. Break it.
			// We want to grab all the characters up to (but not including)
			// the character that does not fit. Of course, 

			CHARACTER_COUNT lCharacters = 0;
			for (;;)
			{
				if (lCharacters == lFullCharacters)
				{
					// Whole word again? How?
					ASSERT(FALSE);
					break;
				}

				// Append the next character to the base word.
				GetBaseWord(1);
				GetNextWord();

				// If the word no longer fits, 
				if (!WordFitsHorizontally())
				{
					break;
				}
				lCharacters++;
			}

			// Found the characters that fit.
			// If zero characters fit, then we make it one.
			if (lCharacters == 0)
			{
				lCharacters = 1;
			}
			// Reconstruct the word with the characters that fit.
			BackUpAWord();
			GetBaseWord(lCharacters);
			GetNextWord();
			m_CurrentWord.m_wFlags |= WORD_FLAG_broken;

			return TRUE;
		}
	}
	else if (m_CurrentWord.m_wType == WORD_TYPE_space)
	{
		// This is a white word.
		// We want to grab all the characters up to and including
		// the character that does not fit.
		BackUpAWord();

		CHARACTER_COUNT lCharacters = 0;
		for (;;)
		{
			if (lCharacters == lFullCharacters)
			{
				// Whole word again? How?
				ASSERT(FALSE);
				break;
			}

			// Append the next character to the base word.
			GetBaseWord(1);
			GetNextWord();
			lCharacters++;

			// If the word no longer fits, 
			if (!WordFitsHorizontally())
			{
				// Go with what we have.
				if (lCharacters != lFullCharacters)
				{
					m_CurrentWord.m_wFlags |= WORD_FLAG_broken;
				}
				return TRUE;
			}
		}
	}
	else if (m_CurrentWord.m_wType == WORD_TYPE_tab)
	{
		// If the tab is the full extent of the frame, "break" it.
		// Otherwise, just move it to the next line.
		return (m_lCharacter == m_lFirstCharacterInLine+1
			 && m_pTextFlowExtent->m_UnusedWidth == m_pTextFlowExtent->m_ExtentRight - m_pTextFlowExtent->m_ExtentLeft);
	}

	return FALSE;
}

void CTextFlow::MoveWord(void)
{
	// "Unget" the word.
	BackUpAWord();

	// Advance to the next flow extent.
	SetCurrentFlowExtent(m_nCurrentFlowExtent+1);
	if (m_pTextFlowExtent == NULL)
	{
		// We have finished the current line.
		m_fLineComplete = TRUE;
	}
}

void CTextFlow::BackUpAWord(void)
{
	if (!IsBaseWordEmpty())
	{
		// Back up the current character position.
		m_lCharacter -= m_CurrentWord.m_lCharacters;
		// Back up the source word offset. This can go negative.
		m_lCharacterInSourceWord -= m_CurrentWord.m_lCharacters;
		m_nWordSource = WORD_SOURCE_Characters;

		SetBaseWordEmpty();
	}
}

/*
// Apply any line attributes like alignment.
*/

void CTextFlow::ApplyLineAttributes(void)
{
	// Line attributes are applied on a per-extent basis. Words have been
	// associated with the extents for just this purpose.

	if (m_nLineAlignment == ALIGN_left)
	{
		// Nothing to do. Words are already left-aligned.
		return;
	}

	BOOL fLastLineInParagraph = FALSE;

	// Compute whether this is the last line in the paragraph or not.
	if (m_fWordsExhausted)
	{
		// Must be the last line.
		fLastLineInParagraph = TRUE;
	}
	else
	{
		// Does the next line start a new paragraph?
		m_Iterator.SetPosition(m_lCharacter);
		fLastLineInParagraph = (m_Iterator.CharacterInParagraph() == 0);
	}

	// Run through each extent and apply the line attributes.

	for (int nExtent = 0; nExtent < m_ExtentArray.GetSize(); nExtent++)
	{
		CTextFlowExtent* pExtent = GetTextFlowExtent(nExtent);
		ASSERT(pExtent != NULL);
		int nLineAlignment = m_nLineAlignment;

		if (pExtent != NULL)
		{
			// Check if we can do justified text.
			// If not, map it back to the non-justified case.
			if (nLineAlignment >= ALIGN_justify_left)
			{
				if (pExtent->m_nNumberOfWords <= 2
						|| fLastLineInParagraph)
				{
					// Map down to the non-justified case.
					nLineAlignment -= 3;
				}
			}

			switch (nLineAlignment)
			{
				case ALIGN_left:
				{
					// Already left aligned.
					break;
				}
				case ALIGN_center:
				{
					AlignCenter(pExtent);
					break;
				}
				case ALIGN_right:
				{
					AlignRight(pExtent);
					break;
				}
				case ALIGN_justify_left:
				{
					AlignJustify(pExtent);
					break;
				}
			}
		}
	}
}

/*
// Center the display words in the extent.
*/

void CTextFlow::AlignCenter(CTextFlowExtent* pExtent)
{
	PCOORD Amount;

	/*
	// Center alignment:
	//
	//		The positions of all the words are adjusted to the right until the
	//	   distance spanned by the left edge of the first solid word and the
	//		right edge of the last solid word is centered in the flow extent.
	//		This could result in white words being positioned all or partially
	//		outside of either side of the flow extent rectangle.
	*/

	/*
	// We first compute the amount to slide the words in the flow extent.
	// This will be half of the sum of the unused width in the flow extent
	// plus the width of any white words at the beginning of the flow extent
	// plus the width of any white words at the end of the flow extent.
	*/

	Amount = pExtent->m_UnusedWidth
				+ FlowExtentLeadingSpace(pExtent)
				+ FlowExtentTrailingSpace(pExtent);

	Amount = Amount/2
				- FlowExtentLeadingSpace(pExtent);

	/*
	// Adjust the words by the computed amount.
	*/

	AdjustExtent(pExtent, Amount);
}

/*
// Right align the display words in the extent.
*/

void CTextFlow::AlignRight(CTextFlowExtent* pExtent)
{
	/*
	// Right alignment:
	//
	//		The positions of all the words are adjusted to the right until the
	//		right side of the last solid word is flush against the right side
	//		of the flow extent.  Any white words at the end of the line will be
	//		positioned past the right edge of the flow extent.
	*/

	/*
	// We first compute the amount to slide the words in the flow extent.
	// This will be the unused width in the flow extent plus the width
	// of any white words at the end of the flow extent.
	//
	// Adjust the words by the computed amount.
	*/

	AdjustExtent(pExtent,
					 pExtent->m_UnusedWidth + FlowExtentTrailingSpace(pExtent));
}

/*
// Justify the display words in the extent.
*/

void CTextFlow::AlignJustify(CTextFlowExtent* pExtent)
{
	/*
	// Justification:
	//
	//		Each word after the first word in the flow extent is moved by an
	//		amount so that the final solid word in the flow extent is positioned
	//		flush right against the right edge of the flow extent, and the
	//		space added between words is as equal as possible.
	*/

	/*
	// Compute the total amount of space to insert into the flow extent.  This
	// will be the sum of the unused space in the flow extent plus the width
	// of any white words at the end of the flow extent.
	*/

	PCOORD AdjustAmount = pExtent->m_UnusedWidth
								+ FlowExtentTrailingSpace(pExtent);

	/*
	// Compute the amount of space to add between every two words.  Since the
	// total amount of space may not divide evenly, the extra space will
	// be divided as evenly as possible.  The maximum difference between the
	// space added between any two words will be one (in whatever units are
	// being used).
	*/

	int nNumberOfWords = pExtent->m_nNumberOfWords;
	PCOORD WordAdjustAmount = AdjustAmount / (nNumberOfWords-2);
	PCOORD WordAdjustError  = AdjustAmount % (nNumberOfWords-2);

	/*
	// To distribute the error, several techniques are possible:
	//
	//		1) Distribute the error over consecutive words starting at the
	//			first word in the flow extent until the error is exhausted.
	//
	//			This will increase spacing at the start of the flow extent.  If
	//			the spacing units are small enough, this will not be perceptable.
	//
	//		2) Distribute the error randomly over the words in the flow extent.
	//
	//			This might look the best, but words could shift slightly as the
	//			user typed in the line, an annoying and time consuming "feature".
	//
	//		3) Use a simple DDA to distribute the error as evenly as possible.
	//
	//			This has the benifit of spreading the error over the entire line
	//			so it is will be less noticable.
	//
	//		4) Use a more sophisticated algorithm which considers the space
	//			which is already between words and the type and style of the
	//			words themselves to best hide the error.
	//
	// Note: if the resolution is fine enough, all of these will give the same
	//			result as far as the user is concerned.
	//
	// For this version of text flow, a DDA will be used.
	*/

	/*
	// Compute the components of the DDA.
	*/

	int nDdaFraction  = (nNumberOfWords-2)/2;
	int nDdaIncrement = (int)WordAdjustError;
	int nDdaLimit     = nNumberOfWords-2;

	/*
	// The adjust amount is cummulative since moving a word should move all
	// the words following it by an equal amount.
	*/

	PCOORD CurrentAdjustAmount = 0;

	/*
	// Get the display word list and march down it adjusting the words.
	*/

	int nWord = pExtent->m_nFirstWord;
	int nLastWord = nWord + nNumberOfWords-1;
	while (nWord < nLastWord)
	{
		CTextWord* pWord = (CTextWord*)m_WordArray.GetAt(nWord);
		ASSERT(pWord != NULL);
		if (pWord != NULL)
		{
			/*
			// Adjust the current word.  The first word will not be adjusted
			// at all.
			*/

			pWord->m_XOffset += CurrentAdjustAmount;

			/*
			// Compute the amount to adjust the next word.
			*/

			if (nWord < nLastWord-1)
			{
				CurrentAdjustAmount += WordAdjustAmount;

				/*
				// Advance the DDA and distribute the error.
				*/

				nDdaFraction += nDdaIncrement;

				if (nDdaFraction >= nDdaLimit)
				{
					/*
					// Time to distribute part of the error.
					*/

					nDdaFraction -= nDdaLimit;

					CurrentAdjustAmount++;
				}
			}
		}
		nWord++;
	}

	/*
	// Adjust the last white word the same amount as the last solid word.
	*/

	CTextWord* pWord = (CTextWord*)m_WordArray.GetAt(nWord);
	ASSERT(pWord != NULL);
	if (pWord != NULL)
	{
		pWord->m_XOffset += CurrentAdjustAmount;
	}
}

/*
// Adjust the words in an extent by an amount.
*/

void CTextFlow::AdjustExtent(CTextFlowExtent* pExtent, PCOORD Amount)
{
	int nWord = pExtent->m_nFirstWord;
	int nLastWord =  nWord + pExtent->m_nNumberOfWords;
	while (nWord < nLastWord)
	{
		CTextWord* pWord = (CTextWord*)m_WordArray.GetAt(nWord);
		ASSERT(pWord != NULL);
		if (pWord != NULL)
		{
			pWord->m_XOffset += Amount;
		}
		nWord++;
	}
}

/*
// Return the width of any leading white words in the flow extent.
//
//		*** If the flow extent contains ONLY white words, zero is returned.
//			 This feature is needed so that when a routine calls both
//			 FlowExtentLeadingSpace() and FlowExtentTrailingSpace(),
//			 the white words are not counted twice if the line is all white.
//
//			 If the calling routine wants to know whether the line is truly
//			 empty or simply white, it can call FlowExtentTrailingSpace()
//			 when this function return zero, and use that value.
*/

PCOORD CTextFlow::FlowExtentLeadingSpace(CTextFlowExtent* pExtent)
{
	PCOORD LeadingSpace = 0;

	/*
	// Accumulate the leading space by marching down the display word list
	// until a solid word is found.  If no solid word is found, then either
	// the display word list is empty or all the words are white.  In either
	// case we return zero.
	*/

	int nWord = pExtent->m_nFirstWord;
	int nLastWord =  nWord + pExtent->m_nNumberOfWords;
	while (nWord < nLastWord)
	{
		CTextWord* pWord = (CTextWord*)m_WordArray.GetAt(nWord++);
		ASSERT(pWord != NULL);
		if (pWord != NULL)
		{
			/*
			// Branch on the type of word.
			*/

			switch (pWord->m_wType)
			{
				case WORD_TYPE_space:
				{
					/*
					// Found a tab, or white word, accumulate its width.
					*/

					LeadingSpace += pWord->m_Width;

					break;
				}

				case WORD_TYPE_tab:
				case WORD_TYPE_solid:
				case WORD_TYPE_dropcap:
				{
					/*
					// Found a solid word, return the current width.
					*/

					return LeadingSpace;
				}

				case WORD_TYPE_break:
				case WORD_TYPE_framebreak:
				case WORD_TYPE_eot:
				{
					// Ignore these.
					break;
				}

				default:
				{
					/*
					// Unknown word type in display word list.
					*/
					ASSERT(FALSE);
					break;
				}
			}
		}
	}

	/*
	// We looked at all the words and either the display word list was empty
	// or they were all white.  In either case, we return zero.
	*/

	return 0;
}

/*
// Return the width of any trailing white words in the flow extent.
*/

PCOORD CTextFlow::FlowExtentTrailingSpace(CTextFlowExtent* pExtent)
{
	PCOORD TrailingSpace = 0;

	/*
	// Accumulate the trailing space by marching down the display word list.
	// When a white word is found, its width is accumulated.  When a solid
	// word is found, the count is reset to zero.  When the end of the
	// list is found, the current count is returned.
	*/

	int nWord = pExtent->m_nFirstWord;
	int nLastWord =  nWord + pExtent->m_nNumberOfWords;
	while (nWord < nLastWord)
	{
		CTextWord* pWord = (CTextWord*)m_WordArray.GetAt(nWord++);
		ASSERT(pWord != NULL);
		if (pWord != NULL)
		{
			/*
			// Branch on the type of word.
			*/

			switch (pWord->m_wType)
			{
				case WORD_TYPE_space:
				{
					/*
					// Found a tab, or white word, accumulate its width.
					*/

					TrailingSpace += pWord->m_Width;

					break;
				}

				case WORD_TYPE_tab:
				case WORD_TYPE_solid:
				case WORD_TYPE_dropcap:
				{
					/*
					// Found a solid word; reset white width to zero.
					*/

					TrailingSpace = 0;
					break;
				}

				case WORD_TYPE_break:
				case WORD_TYPE_framebreak:
				case WORD_TYPE_eot:
				{
					// Ignore these.
					break;
				}

				default:
				{
					/*
					// Unknown word type in display word list.
					*/
					ASSERT(FALSE);
					break;
				}
			}
		}
	}

	/*
	// We have looked at all the words and the current count contains the
	// width of any trailing white space.
	*/

	return TrailingSpace;
}

/*
// Move the words in the word array into the current line.
*/
BOOL CTextFlow::MoveWordsToLine(void)
{
	if (m_WordArray.GetSize() == 0)
	{
		// No words to add. Do not create the line or do anything.
		return TRUE;
	}

	// If we are at the end of the current lines, add one for ourselves.
	if (m_lCurrentLine == m_pFrameRecord->NumberOfLines())
	{
		// We need to add a new line.
		CTextLine Line;
		memset(&Line, 0, sizeof(Line));
		m_pFrameRecord->InsertLine(m_lCurrentLine, &Line);
	}

	// Get the line to flow into.
	CTextLine* pLine = m_pFrameRecord->GetLine(m_lCurrentLine);
	ASSERT(pLine != NULL);

	CTextLine NewLine;
	NewLine.m_lCharacters = m_lCharacter - m_lFirstCharacterInLine;
	NewLine.m_lWords = m_WordArray.GetSize();
	NewLine.m_Baseline = m_Baseline;
	NewLine.m_DropCapBaseline = m_DropBaseline;
	NewLine.m_DropCapSize = m_DropSize;
	NewLine.m_Ascend = m_LineAscend;
	NewLine.m_Descend = m_LineDescend;
//	NewLine.m_Ascend = m_CellAscend;
//	NewLine.m_Descend = m_CellDescend;
	NewLine.m_ExtentLeft = 0x7fffffff;
	NewLine.m_ExtentRight = -NewLine.m_ExtentLeft;
	NewLine.m_LeftOffset = m_LeftOffset;

	NewLine.m_nColumn = m_nColumn;
	NewLine.m_nLineInColumn = m_nLineInColumn;

	BOOL fWordsChanged = FALSE;

	// Set the destination iterator.
	m_Destination.m_lFrame = m_lFrame;
	m_Destination.m_lWord = m_lFirstWordInLine;
	m_Destination.Initialize();

#if 0
	TRACE("Put %ld words (Index %ld) in line %ld\n",
				(WORD_COUNT)m_WordArray.GetSize(),
				m_lFirstWordInLine,
				m_lCurrentLine);
#endif

	// Set up the box used to refresh new words.
	PBOX NewRefresh;
	NewRefresh.y0 = NewLine.m_Baseline - NewLine.m_Ascend;
	NewRefresh.y1 = NewLine.m_Baseline + NewLine.m_Descend;
	NewRefresh.x0 = 0x7fffffff;
	NewRefresh.x1 = -NewRefresh.x0;

	ASSERT(m_WordArray.GetSize() != 0);

	//
	// We now have an array of new words for this line. The line currently
	// has words in it. We will attempt to "merge" these words into the line.
	// The idea behind this is that we want to attempt to preserve as much
	// information as we can. So we will be very careful about what words
	// we replace and which ones we delete, only affecting those that have
	// characters that the new words account for.
	//

	// Merge the words into the line.
	WORD_INDEX lWordIndex = m_lFirstWordInLine;
	BOOL fRefreshLine = FALSE;

	// Set up a character-level traversal for macro determination.
	CHARACTER_INDEX lFirstCharacterInWord = m_lFirstCharacterInLine;

	WORD_COUNT lWordDelta = m_pFrameRecord->NumberOfWords();

	for (int nWord = 0; nWord < m_WordArray.GetSize(); nWord++)
	{
		CTextWord* pNewWord = (CTextWord*)m_WordArray.GetAt(nWord);
		ASSERT(pNewWord != NULL);

		// Update the line extent.
		NewLine.m_ExtentLeft = __min(NewLine.m_ExtentLeft, pNewWord->m_XOffset + pNewWord->m_DrawLeft);
		NewLine.m_ExtentRight = __max(NewLine.m_ExtentRight, pNewWord->m_XOffset + pNewWord->m_DrawWidth);

		// Get the old word.
		BOOL fRefreshNew = FALSE;
		BOOL fInsertNew = TRUE;
		CTextWord* pOldWord = m_Destination.GetWord();

		// Add the word.
		if (pOldWord != NULL)
		{
			ASSERT(m_Destination.m_lFrame != m_lFrame || m_Destination.m_lWord == lWordIndex);
			if (memcmp(pOldWord, pNewWord, sizeof(CTextWord)) == 0 && m_Destination.m_lFrame == m_lFrame)
			{
				// Already there.
				fInsertNew = FALSE;
				// Advance to the next line.
				m_Destination.NextWord();
			}
			else
			{
				// Get rid of any words or parts of words taken up by this
				// new word (to keep things synchronized).

				CHARACTER_COUNT lCharactersLeft = pNewWord->m_lCharacters;
				while (lCharactersLeft != 0 && pOldWord != NULL) 
				{
					// Suck up as much of the next word as we can.
					if (pOldWord->m_lCharacters > lCharactersLeft)
					{
						// We can only use part of the next word.
						pOldWord->m_lCharacters -= lCharactersLeft;
						pOldWord->m_wFlags |= WORD_FLAG_needs_building;

						// Adjust the source gathering.
						if (GetSourceWord() == pOldWord)
						{
							ASSERT(m_lCharacterInSourceWord >= lCharactersLeft);
							m_lCharacterInSourceWord -=lCharactersLeft;
						}

						lCharactersLeft = 0;
					}
					else
					{
						// Use all of this word and get rid of it.
						lCharactersLeft -= pOldWord->m_lCharacters;
						if (pOldWord->m_wType != WORD_TYPE_eot)
						{
							fRefreshLine = TRUE;
						}
						DeleteDestinationWord();
						// We need to refresh the line.

						// Reload the current destination word.
						m_Destination.LoadWord();
						pOldWord = m_Destination.GetWord();
					}
					fWordsChanged = TRUE;
				}
			}
		}

		// Insert the new word if we need to.
		if (fInsertNew)
		{
			InsertWord(lWordIndex, pNewWord);
			fWordsChanged = TRUE;
			fRefreshNew = TRUE;

		}

		// If we are not refreshing the word, yet it contains a macro during
		// macro flow, determine whether it is one to be changed.
		if (!fRefreshNew
				&& m_fFlowingMacros
				&& (pNewWord->m_wFlags & WORD_FLAG_has_macro))
		{
			// This word contains a macro which may need to be refreshed.
			if (m_pMacros == NULL)
			{
				// Refresh all.
				fRefreshNew = TRUE;
				fRefreshLine = TRUE;
			}
			else
			{
				CHARACTER_INDEX lCharacter = lFirstCharacterInWord;
				int nMacros = m_pMacros->GetSize();
				for (CHARACTER_INDEX lCharacterInWord = 0;
						lCharacterInWord < pNewWord->m_lCharacters;
						lCharacterInWord++)
				{
					CHARACTER c = *(m_pTextRecord->GetCharacter(lCharacter++));
					if (c >= MACRO_CHARACTER)
					{
						for (int nMacro = 0; nMacro < nMacros; nMacro++)
						{
							if (m_pMacros->GetAt(nMacro) == c)
							{
								fRefreshNew = TRUE;
								fRefreshLine = TRUE;
								break;
							}
						}
						if (fRefreshNew)
						{
							break;
						}
					}
				}
			}
		}

		// Refresh the new word.
		if (fRefreshNew)
		{
			PCOORD Left = pNewWord->m_XOffset + pNewWord->m_DrawLeft;
			PCOORD Right = pNewWord->m_XOffset + pNewWord->m_DrawWidth;
			ASSERT(Left <= Right);
			if (Left < NewRefresh.x0)
			{
				NewRefresh.x0 = Left;
			}
			if (Right > NewRefresh.x1)
			{
				NewRefresh.x1 = Right;
			}
		}

		// Advance the first character in the word.
		lFirstCharacterInWord += pNewWord->m_lCharacters;

		// Advance to the next destination word.
//		m_Destination.NextWord();
		lWordIndex++;
	}

	lWordDelta = m_pFrameRecord->NumberOfWords() - lWordDelta;

	// All the words have been added in to the new line.
	// See how this affects the old line.
	// In order for this to be accurate, we need to take into account
	// all the words added or deleted by the word adding process (as
	// recorded by lWordDelta).

	BOOL fLineChanged = TRUE;			// Assume so.
	WORD_COUNT lOldWords = pLine->m_lWords + lWordDelta;
	LINE_COUNT lOldLines = m_pFrameRecord->NumberOfLines();
	if (lOldWords > NewLine.m_lWords)
	{
		// Less words in new line. Create another line to hold the leftover
		// words or merge the words onto the following line.
		BOOL fInsert = TRUE;

		WORD_INDEX lLastWord = m_lFirstWordInLine + lOldWords - 1;
		if (m_lCurrentLine+1 < m_pFrameRecord->NumberOfLines()
				&& lLastWord >= 0
				&& lLastWord < m_pFrameRecord->NumberOfWords())
		{
			TRY
			{
				CTextWord* pLastWord = m_pFrameRecord->GetWord(lLastWord);
				if (pLastWord->m_wType != WORD_TYPE_break)
				{
					// Merge the words onto the following line.
					CTextLine* pNextLine = m_pFrameRecord->GetLine(m_lCurrentLine+1);
					pNextLine->m_lCharacters +=  pLine->m_lCharacters - NewLine.m_lCharacters;
					pNextLine->m_lWords += lOldWords - NewLine.m_lWords;
					fInsert = FALSE;
				}
			}
			END_TRY
		}

		if (fInsert)
		{
			CTextLine Line;
			memset(&Line, 0, sizeof(Line));
			Line.m_lCharacters = pLine->m_lCharacters - NewLine.m_lCharacters;
			Line.m_lWords = lOldWords - NewLine.m_lWords;
			Line.m_ExtentLeft = 0x7fffffff;
			Line.m_ExtentRight = -NewLine.m_ExtentLeft;
			NewLine.m_LeftOffset = m_LeftOffset;
			Line.m_nColumn = -1;
			Line.m_nLineInColumn = -1;

			Line.m_DropCapBaseline = m_DropBaseline;
			Line.m_DropCapSize = m_DropSize;

			// Add the new line.
			m_pFrameRecord->InsertLine(m_lCurrentLine+1, &Line);
		}
	}
	else if (lOldWords < NewLine.m_lWords)
	{
		// More words in new line. We want to suck up the lines that contain
		// these words.
		LINE_INDEX lLine = m_lCurrentLine+1;
		WORD_COUNT lWords = NewLine.m_lWords - lOldWords;
		while (lWords > 0 && lLine < m_pFrameRecord->NumberOfLines())
		{
			CTextLine* pNextLine = NULL;
			TRY
				pNextLine = m_pFrameRecord->GetLine(lLine);
			END_TRY
			if (pNextLine == NULL)
			{
				break;
			}

			if (pNextLine->m_lWords > lWords)
			{
				// This satisfies our need for lines.
				pNextLine->m_lWords -= lWords;
				lWords = 0;
			}
			else
			{
				// We are going to use up this line.
				lWords -= pNextLine->m_lWords;
				// Delete the line???
				pNextLine->m_lWords = 0;
			}
			lLine++;
		}
	}
	else
	{
		fLineChanged = (memcmp(&NewLine, pLine, sizeof(CTextLine)) != 0);
	}

	// Lines added or deleted?
	if (lOldLines != m_pFrameRecord->NumberOfLines())
	{
		// Reload the line in case it moved.
		pLine = m_pFrameRecord->GetLine(m_lCurrentLine);
	}

//	BOOL fLineChanged = (memcmp(&NewLine, pLine, sizeof(CTextLine)) != 0);

	// If we need to refresh the old line, do it now.
	if (fRefreshLine || fLineChanged)
	{
		// Set up the box used to refresh old words.
		RefreshLine(pLine, REFRESH_ERASE);
	}

	// refreash all lines in a drop cap
	if( m_nLineInParagraph < m_DropLines )
	{
		long prevline = m_lCurrentLine - 1;
		for( int i = 0; i< m_DropLines && prevline >= 0; i++ ) 
		{
			CTextLine *pLastLine = m_pFrameRecord->GetLine(prevline--);
			RefreshLine(pLine, REFRESH_REDRAW);
		}
	}

	// If we need to refresh any part of the new line.
	if (fLineChanged)
	{
		NewRefresh.x0 = NewLine.m_ExtentLeft + NewLine.m_LeftOffset;
		NewRefresh.x1 = NewLine.m_ExtentRight;
		if( m_nLineInParagraph < m_DropLines )
			NewRefresh.y1 = Max( NewLine.m_Baseline, NewLine.m_DropCapBaseline );
	}

	if (NewRefresh.x0 < NewRefresh.x1 )
	{
		AddRefreshExtent(&NewRefresh, REFRESH_REDRAW);
	}

	else if( NewLine.m_LeftOffset )
	{
		NewRefresh.x0 = NewLine.m_ExtentLeft + NewLine.m_LeftOffset;
		NewRefresh.x1 = NewLine.m_ExtentRight;
		AddRefreshExtent(&NewRefresh, REFRESH_REDRAW);
	}

	// Copy the line itself.
	if (fLineChanged)
	{
		// Adjust the line extent.
		*pLine = NewLine;
	}

	m_fAddedWordsToLine = TRUE;

	// Update end of column for vertical alignment.
	if (m_lLastLineInColumn <= m_lCurrentLine)
	{
		m_lLastLineInColumn = m_lCurrentLine+1;
	}

	return (fWordsChanged || fLineChanged);
}

/*
// Delete a word from the current frame.
*/

void CTextFlow::DeleteDestinationWord(void)
{
	ASSERT(m_Destination.m_pFrame != NULL);
//	ASSERT(m_Destination.m_lFrame == m_lFrame);
	ASSERT(m_Destination.m_lWord < m_Destination.m_pFrame->NumberOfWords());
	m_Destination.m_pFrame->DeleteWord(m_Destination.m_lWord);
	if (m_Destination.m_lFrame != m_lFrame)
	{
		// We have removed a word from the next frame.
		// Remove it from the line as well.
		LINE_COUNT lLines = m_Destination.m_pFrame->NumberOfLines();
		TRY
		{
			for (LINE_INDEX lLine = 0; lLine < lLines; lLine++)
			{
				CTextLine* pLine = m_Destination.m_pFrame->GetLine(lLine);
				if (pLine->m_lWords > 0)
				{
					// Found a line to remove the word from. Do it.
					pLine->m_lWords--;
					break;
				}
			}
		}
		END_TRY
	}
	// Adjust the source iterator.
	if (m_Source.m_lFrame == m_Destination.m_lFrame)
	{
		if (m_Source.m_lWord > m_Destination.m_lWord
				|| (m_Source.m_lWord == m_Source.m_pFrame->NumberOfWords()))
		{
			ASSERT(m_Source.m_lWord != 0);
			if (m_Source.m_lWord != 0)
			{
				m_Source.m_lWord--;
			}
		}
	}
}

/*
// Insert a word into the current frame.
*/

void CTextFlow::InsertWord(WORD_INDEX lWord, CTextWord* pWord)
{
	m_pFrameRecord->InsertWord(lWord, pWord);
	// Adjust the source iterator.
	if (m_Source.m_lFrame == m_lFrame)
	{
		if (m_Source.m_lWord >= lWord)
		{
			m_Source.m_lWord++;
		}
	}
	if (m_Destination.m_lFrame == m_lFrame)
	{
		ASSERT(m_Destination.m_lWord >= lWord);
		{
			m_Destination.m_lWord++;
		}
	}
}

/*
// Refresh a line.
*/

void CTextFlow::RefreshLine(CTextLine* pLine, REFRESH_TYPE nType)
{
   if (pLine->m_ExtentLeft + pLine->m_LeftOffset < pLine->m_ExtentRight)
   {
	   // Set up the box used to refresh the line.
	   PBOX Refresh;

		Refresh.x0 = pLine->m_ExtentLeft + pLine->m_LeftOffset;
	   Refresh.y0 = pLine->m_Baseline - pLine->m_Ascend;
	   Refresh.x1 = pLine->m_ExtentRight;
	   Refresh.y1 = Max( pLine->m_Baseline + pLine->m_Descend, pLine->m_DropCapBaseline );

	   AddRefreshExtent(&Refresh, nType);
   }
}

/*
// Add a refresh extent.
*/

void CTextFlow::AddRefreshExtent(PBOX* pRefresh, REFRESH_TYPE nRefreshType)
{
	if (m_pDatabase->can_refresh()
			&& (m_pFrameObject == NULL
					|| m_pFrameObject->IsOnPage())
 			&& m_pDatabase->CurrentPage() == m_pFrameRecord->Page())
	{
		ASSERT(m_pFrameRecord != NULL);

		// Get unrotated bounds.
		PBOX UnrotatedBound = m_pFrameRecord->ObjectBounds();

		// Translate refresh bounds into world coordinates (it was local to frame.)
		PBOX Box = *pRefresh;
		Box.x0 += UnrotatedBound.x0;
		Box.x1 += UnrotatedBound.x0;
		Box.y0 += UnrotatedBound.y0;
		Box.y1 += UnrotatedBound.y0;

		// Apply any warping, flipping, and/or rotation.
		if (m_pFrameObject->GetWarpedFlippedAndRotatedBoundingBox(&Box))
		{
			// Warping and/or rotation was invloved, so we have to draw,
			// not just erase, so the REDRAW_ERASE optimization is defeated.
			nRefreshType = REFRESH_REDRAW;
		}

		// Get "rotated" bounds.
		PBOX RotatedBound = m_pFrameObject->get_bound();

		// Make sure refresh extent is useful.
		if (IsBoxOnBox(&Box, &RotatedBound))
		{
			m_pDatabase->do_refresh_notify(&Box, nRefreshType, m_pFrameObject, NULL);
		}
	}
}

/*
// We want to flow to the next line.
// Advance the necessary variables.
*/

void CTextFlow::MoveToNextLine(void)
{
	// If we added at least one word, move to the next line.
	// Otherwise, we need to wait until we process the line.
	if (m_fAddedWordsToLine)
	{
		ASSERT(m_lCurrentLine < m_pFrameRecord->NumberOfLines());
		CTextLine* pLine = m_pFrameRecord->GetLine(m_lCurrentLine);

		m_lFirstWordInLine += pLine->m_lWords;
		m_lFirstCharacterInLine += pLine->m_lCharacters;
		m_lCurrentLine++;
		m_nLineInColumn++;
		m_nLineInParagraph++;
	}

	if (m_fExtentClipped)
	{
		// The line has gone off the bottom of the column.
		// Move to the next column if there is one.

		if (m_nColumn == m_nColumns-1)
		{
			// The line has gone off the bottom of the frame.
			// Move to the next frame in the chain if there is one.

			DB_RECORD_NUMBER lNext = m_pFrameRecord->NextFrame();
			if (lNext == 0)
			{
				// No next frame. Stop flowing now.
				m_fSpaceExhausted = TRUE;
				SetStatus(STATUS_Done);
			}
			else
			{
				// Prepare to move on.
				FinishFrame(TRUE);

				// Move to the next frame.
				m_lFrame = lNext;

				GetFrame(TRUE);

				// Start at the top of that frame.
				m_lFirstCharacterInLine = m_pTextState->m_lCharacter;
				m_lFirstWordInLine = 0;
				m_lCurrentLine = 0;

				// Initialize the flow within this new frame.
				InitializeFrame();
			}
		}
		else
		{
			// Move to the next column.
			FinishColumn();
			m_nColumn++;
			InitializeColumn(FALSE);
		}
	}
	else
	{
		// Compute the cell height to add on to the current cell top.
		PCOORD CellHeight = m_CellHeight;

		if (m_nLeadingType == LEADING_lines)
		{
			if (!m_fAddedWordsToLine)
			{
				// Words did not fit. Advance just a small amount.
				CellHeight = 0;
			}
			else
			{
				// Handle multi-line leading.
				if (m_LeadingValue != MakeFixed(1))
				{
					CFixed lCellHeight = PageToInches(CellHeight);
					lCellHeight	= MulFixed(lCellHeight, m_LeadingValue);
					CellHeight = InchesToPage(lCellHeight);
				}
			}
		}

		if (CellHeight == 0)
		{
			// Set it to 1 point.
			CellHeight = PAGE_RESOLUTION/72;
		}

		// Bump the current cell down by the current cell height.
		m_TopOfCell += CellHeight;

		// If we just processed the end of a pargraph, we may need to add
		// addtional space to get to the next paragraph. Compute all this now.

		if (m_fWordsExhausted)
		{
			// This is implicit end of text. Do we add space after?
		}
		else
		{
			m_Iterator.SetPosition(m_lFirstCharacterInLine);
			if (m_Iterator.CharacterInParagraph() == 0)
			{
				// Next line is start of paragraph. This must be the end!
				CFixed Space = m_SpaceAfter + m_Iterator.Style().SpaceBefore();
				m_TopOfCell += InchesToPage(Space);
			}
		}
	}
}

/*
// Compute the extent of the lines.
*/

PCOORD CTextFlow::ComputeLinesExtent(void)
{
	PCOORD LinesExtent = 0;
	CFrameRecord* pFrame = m_pFrameRecord;
	if (pFrame != NULL)
	{
		LINE_COUNT lLines = pFrame->NumberOfLines();
		if (m_lFirstLineInColumn < m_lLastLineInColumn
				&& m_lLastLineInColumn <= lLines)
		{
			// We go from the top of the first line to the bottom of the
			// last line.
			CTextLine* pLine = pFrame->GetLine(m_lFirstLineInColumn);
			PCOORD Top = pLine->m_Baseline - pLine->m_Ascend;
			pLine = pFrame->GetLine(m_lLastLineInColumn-1);
			PCOORD Bottom = pLine->m_Baseline + pLine->m_Descend;
			LinesExtent = Bottom - Top;
		}
	}
	return LinesExtent;
}

/*
// Update the vertical alignment.
*/

BOOL CTextFlow::UpdateVerticalAlignment(void)
{
	if (m_pFrameRecord->VerticalAlignment() != ALIGN_top)
	{
		m_CurrentLinesExtent = ComputeLinesExtent();

		if (m_CurrentLinesExtent != m_LastLinesExtent)
		{
			// This is the value for next time.
			m_LastLinesExtent = m_CurrentLinesExtent;

			CFrameRecord* pFrame = m_pFrameRecord;
			LINE_COUNT lLines; 
			if (pFrame != NULL && (lLines = pFrame->NumberOfLines()) != 0)
			{
				// Compute the offset of the top line from the top of the frame.
				PCOORD TopLineOffset = ComputeTopLineOffset();

				// Get the top line and see if it is in the correct spot.
				// We assume all the rest are correct if the top line is.
				CTextLine* pLine = pFrame->GetLine(m_lFirstLineInColumn);
				PCOORD Delta = TopLineOffset - (pLine->m_Baseline - pLine->m_Ascend);
				if (Delta != 0)
				{
					// Top line is not correct. Readjust the lines.
					// We assume that the lines all need to be offset the same amount.
					for (LINE_INDEX lLine = m_lFirstLineInColumn; lLine < m_lLastLineInColumn; lLine++)
					{
						pLine = pFrame->GetLine(lLine);
						RefreshLine(pLine, REFRESH_ERASE);
						pLine->m_Baseline += Delta;
						RefreshLine(pLine, REFRESH_REDRAW);
					}
					m_TopOfCell += Delta;
					m_Baseline += Delta;
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

/*
// Compute the top line offset for this frame.
*/

PCOORD CTextFlow::ComputeTopLineOffset(void)
{
	PCOORD Space = m_FrameHeight - m_CurrentLinesExtent;
	if (Space < 0)
	{
		Space = 0;
	}

	// We pay attention to the first line space for top alignment.
	// We ignore it for the rest.

	switch (m_pFrameRecord->VerticalAlignment())
	{
		default:
		{
			// Illegal vertical alignment.
			ASSERT(FALSE);
			// Fall through to...
		}
		case ALIGN_top:
		{
			Space = 0;
			break;
		}
		case ALIGN_middle:
		{
			Space /= 2;
			break;
		}
		case ALIGN_bottom:
		{
			break;
		}
	}
	if (Space < m_FirstLineSpace)
	{
		Space = m_FirstLineSpace;
	}
	return Space;
}

/////////////////////////////////////////////////////////////////////////////
// Code to compute flow extents flowing around intruding extents.

/*
// Generate the text flow extents for the current line.
*/

void CTextFlow::GenerateTextFlowExtents(void)
{
	// Build the CFixed band corresponding to this line.
	PCOORD FrameLeft = m_pFrameRecord->ObjectBounds().x0;
	PCOORD FrameTop = m_pFrameRecord->ObjectBounds().y0;
	PCOORD FrameWidth = m_pFrameRecord->ObjectBounds().x1 - FrameLeft;
	PCOORD FrameHeight = m_pFrameRecord->ObjectBounds().y1 - FrameTop;

	PBOX PageBand;
	PageBand.x0 = m_ColumnLeft;
	PageBand.y0 = m_Baseline - m_LineAscend;
	PageBand.x1 = m_ColumnRight;
	PageBand.y1 = m_Baseline + m_LineDescend;

	BOOL fXFlipped = (m_pFrameRecord->ObjectFlags() & OBJECT_FLAG_xflipped);
	BOOL fYFlipped = (m_pFrameRecord->ObjectFlags() & OBJECT_FLAG_yflipped);

	if (fXFlipped)
	{
		PCOORD t = FrameWidth - PageBand.x0;
		PageBand.x0 = FrameWidth - PageBand.x1;
		PageBand.x1 = t;
	}

	if (fYFlipped)
	{
		PCOORD t = FrameHeight - PageBand.y0;
		PageBand.y0 = FrameHeight - PageBand.y1;
		PageBand.y1 = t;
	}

	CFixedRect Band;
	Band.Left = PageToInches(PageBand.x0 + FrameLeft);
	Band.Top = PageToInches(PageBand.y0 + FrameTop);
	Band.Right = PageToInches(PageBand.x1 + FrameLeft);
	Band.Bottom = PageToInches(PageBand.y1 + FrameTop);
	if (Band.Right == Band.Left)
	{
		Band.Right++;
	}

	// Compute the crossings.
	CLineCrossings Crossings;
	BOOL fAddPath =
			(m_pFrameRecord->VerticalAlignment() == ALIGN_top)
		&& ((m_pFrameRecord->FrameFlags() & (FRAME_FLAG_stretch_text|FRAME_FLAG_stretch_frame)) == 0)
		&& (m_nStretchMode == STRETCH_MODE_None)
		&& (m_pFrameRecord->GetWarpField() == NULL);

	Crossings.Compute(Band, fAddPath ? &m_pFrameRecord->Intersections() : NULL);

	// Add extents for all the crossings.
	int nCrossings = Crossings.Crossings();
	int nCount = 0;				// Currently "clear".
	CFixed lLeft = UNDEFINED_CFIXED;
	for (int nCrossing = 0; nCrossing < nCrossings; nCrossing++)
	{
		CLineCrossing* pCrossing = Crossings.Crossing(nCrossing);
		CFixed lPosition = pCrossing->m_lPosition;
		nCount += pCrossing->m_nCount;
		if (nCount == 0)
		{
			// We have entered into a clear area.
			// Remember this as the left position.
			ASSERT(lLeft == UNDEFINED_CFIXED);
			lLeft = lPosition;
			ASSERT(lLeft != UNDEFINED_CFIXED);
		}
		else
		{
			// Not in a clear area. See if we just exited one.
			if (lLeft != UNDEFINED_CFIXED)
			{
				// We just left a clear area. Add an extent.
				ASSERT(lLeft < lPosition);
				ASSERT(lLeft >= Band.Left);
				ASSERT(lPosition <= Band.Right);
//				AddTextFlowExtent(InchesToPage(lLeft) - FrameLeft,
//										InchesToPage(lPosition) - FrameLeft);
//#define RoundedInchesToPage(x)	FixedInteger(MulFixed(x, MakeFixed(PAGE_RESOLUTION))+0x08000)
#define RoundedInchesToPage(x)	MulFixed(x+MakeFixed(1.0/(PAGE_RESOLUTION*2.0)), PAGE_RESOLUTION)
				PCOORD Left = InchesToPage(lLeft) - FrameLeft;
				PCOORD Right = RoundedInchesToPage(lPosition) - FrameLeft;

				if (fXFlipped)
				{
					PCOORD Width = m_pFrameRecord->ObjectBounds().x1 - m_pFrameRecord->ObjectBounds().x0;
					PCOORD t =  Width - Left;
					Left = Width - Right;
					Right = t;
				}
				AddTextFlowExtent(Left, Right);
				lLeft = UNDEFINED_CFIXED;
			}
		}
	}
//	ASSERT(m_ExtentArray.GetSize() > 0);
}

//////////////////////////////////
// Function: CTextFlow::SetDropDimensions()
//
// Purpose: based on the style of the second character in the paragraph,
//	the 1st character after the drop cap, sets the height of the drop cap, 
// m_DropBaseline.  Given the  height of the drop cap, and based on the style of
// the character to drop calculates the point size for the drop cap, m_DropSize,
// and the amount of indent for the drop cap character, m_DropWidth
//
// Author: cschendel
// TODO make this size calculation more accurate for the box of the drop cap
// TODO macro characters inserted before the drop cap?
void CTextFlow::SetDropDimensions()
{
	// only do this if we have text!
	int nTotalCharacters = m_pTextRecord->NumberOfCharacters();

	// if it is not a drop cap paragraph, the line in the paragraph is below the drop cap, or there is no text, 
	if( m_DropLines < 2  || m_nLineInParagraph > m_DropLines || (nTotalCharacters  < 1  && m_lCharacter < 0) )
	{
		m_DropBaseline = m_DropSize = m_DropWidth = 0; // unset drop cap dimensions
	}
	// else if on the 1st line of the paragraph or the parameters are uninitialzed, set them
	else if( m_nLineInParagraph == 0  || m_DropBaseline == 0 || m_DropSize == 0 ) 
	{
		// save current place in text because we are going to move to the start of the paragarph
		CHARACTER_INDEX index = m_Iterator.CharacterIndex();
		m_Iterator.SetPosition( m_lCharacter );
		CHARACTER_INDEX charactersInParagraph = m_Iterator.CharacterInParagraph();
		CHARACTER_INDEX paragraphStart = m_lCharacter - charactersInParagraph;
		// if we are not at the start of the paragarph, go to the start 
		if( charactersInParagraph > 0 )
				m_Iterator.SetPosition( paragraphStart );

		// create a copy of the style for the drop cap to use when calculating it's size
		CTextStyle styleDropChar = m_Iterator.Style();
		// move to the first character after the drop cap

		// iterate to find the 2nd real character in the paragraph
		CHARACTER c = 0;
		if( paragraphStart < nTotalCharacters )
			c = *( m_pTextRecord->GetCharacter(paragraphStart) );
		// if the character at the start of the paragraph is not a new paragraph continue
		if( c == '\n' )
		{
			m_DropLines = 0;
		}
		else
		{
			CHARACTER_INDEX i = paragraphStart + 1;
			while(  (c < 32 || c > MACRO_CHARACTER) && i < nTotalCharacters )
			{
				m_Iterator.SetPosition( i );
				c = *(m_pTextRecord->GetCharacter(i++));
				if( c == '\n' )
					break;
			}
			// get the amount of drop based on the style of 2nd character in the paragraph (could be own function)
			PCOORD ascend, descend, height;
			ascend = descend = height = 0;
			// account for the leading of the style
			int type = m_Iterator.Style().LeadingType();
			CFixed value = m_Iterator.Style().Leading();
			if( type == LEADING_lines )
			{
				ascend = m_Iterator.Style().Ascend();
				height = m_Iterator.Style().Descend() + ascend;
				if( value != MakeFixed(1) )
				{
					CFixed lHeight = PageToInches(height);
					lHeight	= MulFixed(lHeight, value);
					height = InchesToPage(lHeight);

					CFixed lAscend = PageToInches(ascend);
					lAscend	= MulFixed(lAscend, value);
					ascend = InchesToPage(lAscend);

					CFixed lDescend = PageToInches(descend);
					lDescend	= MulFixed(lDescend, value);
					descend = InchesToPage(lDescend);
				}
			}
			else if( type == LEADING_points )
			{
				height = (PCOORD)MulFixed(value, PAGE_RESOLUTION/72);
				descend = height/4;
				ascend = height - descend;
			}
			m_DropBaseline = ( height * (m_DropLines - 1) ) + ascend;
			
			// TODO make this size calculation more accurate for the box of the drop cap
			// this is just fudging it by test.
			int fudgefactor = height;
			if( m_DropLines == 2 )
				m_DropSize = MakeFixed(  ((m_DropBaseline + (fudgefactor >> 1)) * 72) / 1800);
			else if( m_DropLines == 3 )
				m_DropSize = MakeFixed(  ((m_DropBaseline + fudgefactor) * 72) / 1800);
			else
				m_DropSize = MakeFixed(  ((m_DropBaseline + (fudgefactor * 1.2) ) * 72) / 1800);

			// set the size in the temporary style to compute the width
			styleDropChar.Size( m_DropSize );
			// compute the metrics for the drop cap character
			FontServer* pFontServer = m_pDatabase->get_font_server();
			pFontServer->FontMetrics( &styleDropChar );
			// get the drop cap character and calculate it's width at the new size
			// at the start of a new paragraph there are no characters to look at
			if( paragraphStart < nTotalCharacters )
			{
				c = *( m_pTextRecord->GetCharacter(paragraphStart) );
				m_DropWidth = pFontServer->GetAdvanceWidth(&styleDropChar, c, m_pFrameRecord->Page());
			}
		}
		// reset the iterator position to what it was (should be m_lCharacter? )
		m_Iterator.SetPosition( index );
	}
	// adjust the text and drop cap baselines if neccessary. 
	AdjustBaselines();
}
//////////////////////////////////
// Function: CTextFlow::AdjustBaselines()
//
// Purpose: Call at the start of a line. 
// Adjusts m_Baseline for the first line of a paragraph to use the Drop Cap
// baseline of the previous paragraph had drop caps and the drop cap extends 
// lower then the end of that paragraph.
// Offsets m_DropCapBaseline by m_Baseline. m_DropCapBaseline and m_Baseline
// are coordinates measured from the top of the text cell.
//
// Author: cschendel
// 
void CTextFlow::AdjustBaselines()
{
	//If we use the iterator to go to the last paragraph and look at the dropcap baseline in the 1st line
	// of that paragarph we can set the dropcap baseline of paragraphs greater then the drop cap to 0
	//and that will cure the problem when we are adjusting the baseline for a para after a para that started in teh previous frame.

	// only need to adjust the baselines at the start of the paragraph
	if( m_nLineInParagraph == 0 ) 
	{
		CTextLine *pLine = NULL;
		// index to last line of the paragraph above this one 
		LINE_INDEX lastLineInPrevPara = m_lCurrentLine - 1;
		// if the end of text is a <cr> a new line might not be added yet, in this case do dimensions matter?
		if( lastLineInPrevPara >= 0 && lastLineInPrevPara < m_pFrameRecord->NumberOfLines() )
			pLine = m_pFrameRecord->GetLine( lastLineInPrevPara );
		if( pLine)
		{
			// calculate aditional inter-paragraph space
			// set the iterator to the last paragraph, get space to add after the last paragraph
			m_Iterator.SetPosition( m_lFirstCharacterInLine - 1);
			CFixed space = m_Iterator.Style().SpaceAfter();
			// set the iteratator back and get the space to add before this paragraph
			m_Iterator.SetPosition( m_lCharacter );
			space += m_Iterator.Style().SpaceBefore();
			PCOORD xtra = InchesToPage(space);

			// we are on the 1ts line of a paragraph see if it should start at m_Baseline
			// or the baseline of the drop cap of the previous paragraph.
			if( pLine->m_DropCapBaseline + xtra > m_Baseline )
			{
				m_Baseline = pLine->m_DropCapBaseline + pLine->m_Descend + xtra;
				m_TopOfCell = m_Baseline - m_CellDescend;
			}
			// adjust the dropcap baseline for this paragraph too. 
			if( m_DropBaseline > 0 )
				m_DropBaseline += m_Baseline - m_LineAscend;
		}
	}
}

//////////////////////////////////
// Function: CTextFlow::UpdateLineInParagraph()
//
// Purpose: Updates the m_nLineInParagraph counter
//
// Author: cschendel
void CTextFlow::UpdateLineInParagraph()
{
	m_nLineInParagraph = 0;

	if( m_lCurrentLine < m_pFrameRecord->NumberOfLines() &&
		 m_lCharacter > 0 && m_pTextRecord->NumberOfCharacters() > m_lCharacter )
	{
		m_Iterator.SetPosition( m_lCharacter );
		CHARACTER_INDEX charactersInParagraph = m_Iterator.CharacterInParagraph();
		// now set the line in the paragraph
		CTextLine* pLine = m_pFrameRecord->GetLine(m_lCurrentLine);
		// count back lines until we get to the first line of the paragraph
		if( charactersInParagraph != 0 )
		{	
			int prevoiusLine = m_lCurrentLine - 1;
			// lineStart and paragraphStart are offsets from the start of text
			int nCharactersCounted = 0;
			CFrameRecord *pFrameRecord = m_pFrameRecord;
			while( nCharactersCounted < charactersInParagraph )
			{
				// In a linked frame, paragraph starts in the previous frame 
				if( prevoiusLine < 0 )
				{
					DB_RECORD_NUMBER lFrame = pFrameRecord->PreviousFrame();
					if( lFrame > 0 )
					{
						if( pFrameRecord != m_pFrameRecord )
							pFrameRecord->release();
						pFrameRecord = (CFrameRecord*)m_pDatabase->get_record(lFrame, NULL, RECORD_TYPE_Frame);
						if( !pFrameRecord )
							break;
						prevoiusLine = pFrameRecord->NumberOfLines() - 1;
					}
					else
						break;
				}
				pLine = pFrameRecord->GetLine(prevoiusLine--);
				nCharactersCounted += pLine->m_lCharacters;
				m_nLineInParagraph++;
			}
			if( pFrameRecord && pFrameRecord != m_pFrameRecord )
					pFrameRecord->release();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWordIterator

/*
// Constructor
*/

CWordIterator::CWordIterator(void)
{
	m_lFrame = 0;
	m_pFrame = NULL;			// Word source frame.
	m_fCanAdvance = TRUE;
}

/*
// Destructor
*/

CWordIterator::~CWordIterator(void)
{
	ReleaseFrame();
}

/*
// Release the frame if we have one locked.
*/

void CWordIterator::ReleaseFrame(void)
{
	if (m_pFrame != NULL)
	{
		m_pFrame->release();
		m_pFrame = NULL;
	}
}
/*
// Initialize the word iterator (so we can begin iterating!)
*/

void CWordIterator::Initialize(void)
{
	if (m_lFrame != 0)
	{
		GetFrame();
		if (m_lWord >= m_pFrame->NumberOfWords())
		{
			NextWord();
		}
	}
}

/*
// Get the current word.
*/

CTextWord* CWordIterator::GetWord(void)
{
	if (m_pFrame == NULL)
	{
		return NULL;
	}
	else
	{
		return m_pFrame->GetWord(m_lWord);
	}
}

/*
// Move to the next word.
*/

void CWordIterator::NextWord(void)
{
	if (m_pFrame != NULL)
	{
		// Try next word in frame.
		m_lWord++;
		LoadWord();
	}
}

/*
// Make sure we're pointing at the word we want.
*/

void CWordIterator::LoadWord(void)
{
	// Move to next frame while we need to and we can.
	while (m_pFrame != NULL && m_lWord >= m_pFrame->NumberOfWords())
	{
		m_lFrame = m_fCanAdvance ? m_pFrame->NextFrame() : 0;
		GetFrame();

		m_lWord = 0;
	}
#if 0
	// Move to previous frame while we need to and we can.
	while (m_pFrame != NULL && m_lWord < 0)
	{
		m_lFrame = m_pFrame->NextFrame();
		GetFrame();

		m_lWord = m_pFrame->NumberOfWords()-1;
	}
#endif
}

/*
// Get the current frame.
*/

void CWordIterator::GetFrame(void)
{
	if (m_pFrame == NULL || m_pFrame->Id() != m_lFrame)
	{
		// Out with the old.
		ReleaseFrame();
		if (m_lFrame == 0)
		{
			// No frame to get.
			m_pFrame = NULL;
			m_lWord = 0;
		}
		else
		{
			ERRORCODE error;
			// We have a frame number. Get the frame record.
			if ((m_pFrame = (CFrameRecord*)m_pDatabase->get_record(m_lFrame, &error, RECORD_TYPE_Frame)) == NULL)
			{
				ThrowErrorcodeException(error);
			}
		}
	}
}

