/*
// $Header: /PM8/App/TEXTREC.CPP 1     3/03/99 6:12p Gbeddow $
//
// Text record routines.
//
// $Log: /PM8/App/TEXTREC.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 7     3/01/99 11:55a Cschende
// added functions to make permanent font substitutions in the document
// after it is read in for font which are no longer going to be
// distributed with PrintMaster
// 
// 6     7/24/98 1:26p Johno
// The comment below is wrong; Just a Diagnostic ASSERT()
// 
// 5     7/23/98 9:54p Johno
// Diagnostic ASSERTS, kludge fix in CTxp::CalcFrame()
// 
// 4     4/10/98 12:19p Johno
// Fixed bug in ReferenceStyleChange(): no longer copy "empty" hyperlinks
// 
// 3     3/27/98 5:49p Johno
// Changes for reference counts
// 
// 2     3/18/98 11:15a Jayn
// Page numbers
// 
//    Rev 1.0   14 Aug 1997 15:26:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:46   Fred
// Initial revision.
// 
//    Rev 1.38   23 Jul 1997 11:13:30   Jay
// Fixed multiple old color tokens in paragraph.
// 
//    Rev 1.37   15 Jul 1997 09:33:12   Jay
// Improved Dump()
// 
//    Rev 1.36   08 Jul 1997 14:47:04   Jay
// Fixed moving array element heap corruption
// 
//    Rev 1.35   07 Jul 1997 10:44:58   Jay
// Fixes old style changes
// 
//    Rev 1.34   04 Mar 1997 17:03:56   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.33   06 Jan 1997 10:13:42   Jay
// Optimizations
// 
//    Rev 1.32   05 Nov 1996 14:06:56   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.31   26 Aug 1996 15:32:56   Jay
// Sets modified flag on stretch change.
// 
//    Rev 1.30   31 Jul 1996 11:58:56   Fred
// 16-bit compatibility
// 
//    Rev 1.29   15 Jul 1996 11:34:32   Jay
//  
// 
//    Rev 1.28   12 Jul 1996 16:48:12   Jay
// Fix
// 
//    Rev 1.27   11 Jul 1996 11:35:26   Jay
//  
// 
//    Rev 1.26   10 Jul 1996 10:25:50   Jay
// Text states no longer allocated.
// 
//    Rev 1.25   09 Jul 1996 15:37:18   Jay
//  
// 
//    Rev 1.24   05 Jul 1996 10:47:16   Jay
//  
// 
//    Rev 1.23   03 Jul 1996 14:30:34   Jay
//  
// 
//    Rev 1.22   02 Jul 1996 11:24:52   Jay
//  
// 
//    Rev 1.21   02 Jul 1996 08:55:10   Jay
//  
// 
//    Rev 1.20   28 Jun 1996 16:18:40   Jay
//  
// 
//    Rev 1.19   27 Jun 1996 17:20:48   Jay
//  
// 
//    Rev 1.18   27 Jun 1996 13:28:18   Jay
//  
// 
//    Rev 1.17   26 Jun 1996 14:22:26   Jay
//  
// 
//    Rev 1.16   26 Jun 1996 09:06:56   Jay
//  
// 
//    Rev 1.15   25 Jun 1996 10:25:34   Jay
//  
// 
//    Rev 1.14   24 Jun 1996 18:06:16   Jay
//  
// 
//    Rev 1.13   21 Jun 1996 14:30:32   Jay
// Stretch text, etc.
// 
//    Rev 1.12   20 Jun 1996 17:40:02   Jay
// Some speedups and diagnostics.
// 
//    Rev 1.11   20 Jun 1996 14:59:52   Jay
//  
// 
//    Rev 1.10   19 Jun 1996 13:45:52   Jay
//  
// 
//    Rev 1.9   18 Jun 1996 16:14:54   Jay
//  
// 
//    Rev 1.8   14 Jun 1996 17:05:44   Jay
//  
// 
//    Rev 1.7   14 Jun 1996 08:57:08   Jay
//  
// 
//    Rev 1.6   13 Jun 1996 17:46:10   Jay
//  
// 
//    Rev 1.5   13 Jun 1996 15:53:10   Jay
//  
// 
//    Rev 1.4   04 Jun 1996 17:38:22   Jay
//  
// 
//    Rev 1.3   24 May 1996 16:15:58   Fred
// TRACEx
// 
//    Rev 1.2   24 May 1996 09:27:02   Jay
// New text code.
// 
//    Rev 1.1   23 Apr 1996 08:07:38   Jay
// More new stuff
// 
//    Rev 1.0   12 Apr 1996 13:08:56   Jay
// New text stuff
*/

#include "stdafx.h"
#include "pmgdb.h"

#include "textrec.h"
#include "stylerec.h"
#include "fontsrv.h"
#include "framerec.h"
#include "pararec.h"
#include "macrorec.h"
#include "textflow.h"
#include "hyperrec.h"

#include "pmgfont.h"
#define DEBUG_RCSx

/////////////////////////////////////////////////////////////////////////////
// Text record.

/*
// The creator for a text record.
*/

ERRORCODE CTextRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */

	*record = NULL;
	TRY
	{
		*record = new CTextRecord(number, type, owner, where);
	}
	END_TRY

	return (*record == NULL) ? ERRORCODE_Memory : ERRORCODE_None;
}

/*
// The constructor for a text record.
*/

CTextRecord::CTextRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	memset(&m_Record, 0, sizeof(m_Record));

	m_pCharacterArray = NULL;
	m_pStyleArray = NULL;
	m_pParagraphArray = NULL;

	m_pEditingText = NULL;

	// Statistics.
	m_lGetParagraph = 0;
	m_lModifyParagraph = 0;
	m_lGetParagraphStyle = 0;
	m_lGetStyleChange = 0;
	m_lModifyStyleChange = 0;
	m_lAdvanceTextState = 0;
}

/*
// The destructor for a text record.
*/

CTextRecord::~CTextRecord()
{
#if 0
	TRACE("GetParagraph: %ld\n"
			"ModifyParagraph: %ld\n"
			"GetParagraphStyle: %ld\n"
			"GetStyleChange: %ld\n"
			"ModifyStyleChange: %ld\n"
			"AdvanceTextState: %ld\n",
			m_lGetParagraph,
			m_lModifyParagraph,
			m_lGetParagraphStyle,
			m_lGetStyleChange,
			m_lModifyStyleChange,
			m_lAdvanceTextState);
#endif

	SetEditingText(NULL);
	ReleaseRecords();
}

/*
// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

void CTextRecord::destroy(void)
{
	// Release any locked records before freeing.
	ReleaseRecords();
	DestroyRecords();
}

/*
// The database is going away. All records are being freed.
// Release any sub-records we have locked.
*/

void CTextRecord::ReleaseSubrecords(void)
{
	ReleaseRecords();
}

void CTextRecord::DestroyRecords(void)
{
	// Free the character array.
	DeleteSubrecord(m_Record.m_lCharacterArray, RECORD_TYPE_CharacterArray);
	// Free the style array.
	DeleteSubrecord(m_Record.m_lStyleArray, RECORD_TYPE_StyleArray);
	// Free the paragraph array.
	DeleteSubrecord(m_Record.m_lParagraphArray, RECORD_TYPE_ParagraphArray);
}

/////////////////////////////////////////////////////////////////////////////
// SelfTest()

void CTextRecord::DumpCounts(void)
{
	TRACE1("Number of characters: %ld\n", NumberOfCharacters());
	TRACE1("Number of style changes: %ld\n", NumberOfStyleChanges());
	TRACE1("Number of paragraphs: %ld\n", NumberOfParagraphs());
}

void CTextRecord::SelfTest(void)
{
#if 0
	TRACE0("CTextRecord: Begin SelfTest\n");
	TRY
	{
		TRACE0("Insert 5 characters...\n");
		CHARACTER c[5] = { 'H', 'e', 'l', 'l', 'o' };
		InsertCharacter(0, c, 5);

		TRACE0("Insert 1 paragraph...\n");
		CTextParagraph Paragraph;
		InsertParagraph(0, &Paragraph, 1);

		DumpCounts();
		TRACE0("ReleaseRecords...\n");
		DumpCounts();
	}
	CATCH(CErrorcodeException, e)
	{
		TRACE1("Got errorcode exception: %d\n", e->m_error);
	}
	AND_CATCH_ALL(e)
	{
		TRACE0("Got an exception!\n");
	}
	END_CATCH_ALL
	TRACE0("CTextRecord: End SelfTest\n");
#else
	ASSERT(FALSE);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Old document support functions.

// Convert the contents of an old text paragraph into this object.
ERRORCODE CTextRecord::FromOldParagraph(ParagraphRecord* pParagraph)
{
	ERRORCODE error = ERRORCODE_None;

	// Copy the text over.
	ASSERT(NumberOfCharacters() == 0);		// No text for us yet!
	CHARACTER* pCharacters = pParagraph->get_character(0);
	int nCharacters = pParagraph->number_of_characters();
	// We may need a text iterator.
	CTextStyle Style((PMGDatabase*)database);

	// Pick up the base paragraph style.
	Style.FromOldStyle(pParagraph->get_style());

	TRY
	{
		if (nCharacters > 0)
		{
			// Convert all special characters to normal characters.
			CHARACTER* cp = pCharacters;
			for (int n = 0; n < nCharacters; n++, cp++)
			{
				if (*cp == BREAK_CHARACTER)
				{
					*cp = '\n';
				}
				else if (*cp == TAB_CHARACTER)
				{
					*cp = '\t';
				}
			}

			// Insert the text.
			InsertText(0, pCharacters, nCharacters);

			// Apply all style changes.
			int nChanges = pParagraph->number_of_changes();

			CHARACTER_INDEX lStart = 0;

			if (nChanges > 0)
			{
				// Loop through all the style changes.
				S_INDEX nChange = 0;
				STYLE_CHANGE* pOldChange = pParagraph->get_style_change(0);
				while (nChange < nChanges)
				{
					CHARACTER_INDEX lEnd = pOldChange->c_index;

					ChangeRangeStyle(lStart, lEnd, &Style);
					// Pick up all style changes at this location.
					do
					{
						Style.ApplyOldChange(pOldChange++, FALSE);
					} while (++nChange < nChanges && pOldChange->c_index == lEnd);
					lStart = lEnd;
				}
			}
			// Do the final style change.
			ChangeRangeStyle(lStart, NumberOfCharacters(), &Style);
		}
		else
		{
			// Set the one paragraph style.
			SetParagraphStyle(0, &Style);
		}

		// And set all the paragraph styles.
		ChangeParagraphStyle(0, NumberOfParagraphs(), &Style);
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

	ASSERT(error == ERRORCODE_None);
	return error;
}

/////////////////////////////////////////////////////////////////////////////
// Helper functions.

/*
// Release any records we may have locked.
*/

void CTextRecord::ReleaseRecords(void)
{
	// Release the character array.
	ReleaseSubrecord((DatabaseRecord*&)m_pCharacterArray);
	// Release the style array.
	ReleaseSubrecord((DatabaseRecord*&)m_pStyleArray);
	// Release the paragraph array.
	ReleaseSubrecord((DatabaseRecord*&)m_pParagraphArray);
}

//
// Lock the character array.
//

void CTextRecord::LockCharacterArray(void)
{
	if (m_pCharacterArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pCharacterArray, m_Record.m_lCharacterArray, RECORD_TYPE_CharacterArray, TRUE);
	}
}

//
// Lock the style array.
//

void CTextRecord::LockStyleArray(void)
{
	if (m_pStyleArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pStyleArray, m_Record.m_lStyleArray, RECORD_TYPE_StyleArray, TRUE);
	}
}

//
// Lock the paragraph array.
//

void CTextRecord::LockParagraphArray(void)
{
	if (m_pParagraphArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pParagraphArray, m_Record.m_lParagraphArray, RECORD_TYPE_ParagraphArray, TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Text state routines.

//
// Initialize a text state.
//

void CTextRecord::InitializeTextState(CTextState* pState, CHARACTER_INDEX Index, BOOL fForceReset /*=TRUE*/)
{
	// Start the text state out at something.
	// Maybe someday we'll find the closest existing state. For now we'll just
	// start at the beginning.
	if (fForceReset || Index < pState->m_lCharacter)
	{
		pState->Reset();
	}
	// Now move on from here.
	ASSERT(pState->m_lCharacter <= Index);

	// Advance to where our index is. May do nothing if already there...
	AdvanceTextState(pState, Index);
}

void CTextRecord::AdvanceTextState(CTextState* pState, CHARACTER_INDEX lIndex)
{
	m_lAdvanceTextState++;
// Advance to the correct paragraph.
	CHARACTER_COUNT lCount = lIndex - pState->m_lCharacter;
	ASSERT(lCount >= 0);
	PARAGRAPH_COUNT lParagraphs = 0;
	CTextParagraph* pParagraph = NULL;
	PARAGRAPH_COUNT lLastParagraph = NumberOfParagraphs()-1;

	while (lCount > 0)
	{
		if (lParagraphs == 0)
		{
			pParagraph = GetParagraph(pState->m_lParagraph, &lParagraphs);
		}

		CHARACTER_COUNT lThisCount = pParagraph->m_lCharacters - pState->m_lCharacterInParagraph;
		if (lCount >= lThisCount)
		{
		/* Text count uses up this paragraph. */
			if (pState->m_lParagraph == lLastParagraph)
			{
			/* Move to the end of text. */
//j				ASSERT(lCount == lThisCount);
				pState->m_lCharacterInParagraph = pParagraph->m_lCharacters;
				lCount = 0;
			}
			else
			{
			/* Move to the next paragraph. */
				lCount -= lThisCount;
				pState->m_lCharacterInParagraph = 0;
				pState->m_lParagraph++;

				lParagraphs--;
				pParagraph++;
			}
		}
		else
		{
		/* Move forward within this paragraph. */
			pState->m_lCharacterInParagraph += lCount;
			lCount = 0;
		}
	}
	pState->m_lCharacter = lIndex;
}

/////////////////////////////////////////////////////////////////////////////
// Character routines.

CHARACTER_COUNT CTextRecord::NumberOfCharacters(void)
{
	LockCharacterArray();
	ASSERT(m_pCharacterArray != NULL);
	return (m_pCharacterArray == NULL) ? 0 : m_pCharacterArray->ElementCount();
}

void CTextRecord::InsertCharacter(CHARACTER_INDEX Index, const CHARACTER* c, CHARACTER_COUNT Count /*=1*/)
{
	LockCharacterArray();
	ASSERT(m_pCharacterArray != NULL);
	if (m_pCharacterArray != NULL)
	{
		m_pCharacterArray->Insert(Index, (LPVOID)c, Count);

		// Count how many macro characters we are inserting.
		while (Count-- > 0)
		{
			if (*c++ >= MACRO_CHARACTER)
			{
				m_Record.m_lMacroCount++;
			}
		}
	}
}

void CTextRecord::DeleteCharacter(CHARACTER_INDEX Index, CHARACTER_COUNT Count /*=1*/)
{
	LockCharacterArray();
	ASSERT(m_pCharacterArray != NULL);
	if (m_pCharacterArray != NULL)
	{
		// Count how many macro characters we are deleting.
		// We accumulate the count first in case the actual delete fails.
		// Actually, if the actual delete fails, then some macro characters
		// may be deleted without our knowing it. Oh, well.

		CHARACTER_INDEX lDeleteIndex = Index;
		CHARACTER_COUNT lDeleteCount = Count;
		CHARACTER_COUNT lMacroCount = 0;

		while (lDeleteCount > 0)
		{
			// Figure out how many consecutive characters we can access.
			CHARACTER_COUNT lThisCount;
			CHARACTER* cp = (CHARACTER*)m_pCharacterArray->GetAt(lDeleteIndex, (DWORD*)&lThisCount);
			if (lThisCount > lDeleteCount)
			{
				lThisCount = lDeleteCount;
			}
			lDeleteIndex += lThisCount;
			lDeleteCount -= lThisCount;

			// Count the macro characters.
			while (lThisCount-- > 0)
			{
				if (*cp++ >= MACRO_CHARACTER)
				{
					lMacroCount++;
				}
			}
		}

		// Do the delete.
		m_pCharacterArray->Delete(Index, Count);

		// Update the macro count.
		ASSERT(lMacroCount <= m_Record.m_lMacroCount);
		if ((m_Record.m_lMacroCount -= lMacroCount) < 0)
		{
			// Do not let the count go negative.
			m_Record.m_lMacroCount = 0;
		}
	}
}

CHARACTER* CTextRecord::GetCharacter(CHARACTER_INDEX Index)
{
	LockCharacterArray();
	ASSERT(m_pCharacterArray != NULL);
	return (m_pCharacterArray == NULL) ? NULL : (CHARACTER*)m_pCharacterArray->GetAt(Index);
}

CHARACTER* CTextRecord::GetCharacters(CHARACTER_INDEX Index, CHARACTER_COUNT* pCharactersAvailable)
{
	LockCharacterArray();
	ASSERT(m_pCharacterArray != NULL);

	if (m_pCharacterArray == NULL)
	{
		*pCharactersAvailable = 0;
		return NULL;
	}
	return (CHARACTER*)m_pCharacterArray->GetAt(Index, (DWORD*)pCharactersAvailable);
}

/////////////////////////////////////////////////////////////////////////////
// Style change routines.

STYLE_COUNT CTextRecord::NumberOfStyleChanges(void)
{
	LockStyleArray();
	ASSERT(m_pStyleArray != NULL);
	return (m_pStyleArray == NULL) ? 0 : m_pStyleArray->ElementCount();
}

CTextStyleChange* CTextRecord::NewStyleChange(STYLE_INDEX* plChange, const CTextStyleChange* pSourceChange /*=NULL*/)
{
	LockStyleArray();
	ASSERT(m_pStyleArray != NULL);
	if (m_pStyleArray == NULL)
	{
		return NULL;
	}
	else
	{
		modified();
		CTextStyleChange* pChange = m_pStyleArray->NewStyleChange(plChange);
		if (pChange != NULL && pSourceChange != NULL)
		{
			*pChange = *pSourceChange;
			if (pChange->m_wToken == TOKEN_Face && pChange->m_lValue != 0)
			{
				((PMGDatabase*)database)->inc_face((DB_RECORD_NUMBER)pChange->m_lValue);
			}
         else
			if (pChange->m_wToken == TOKEN_Hyperlink && pChange->m_lValue != 0)
			{
				((PMGDatabase*)database)->IncrementHyperlinkRecord((DB_RECORD_NUMBER)pChange->m_lValue);
			}

		}
		return pChange;
	}
}

CTextStyleChange* CTextRecord::GetStyleChange(STYLE_INDEX lChange)
{
	m_lGetStyleChange++;

	LockStyleArray();
	ASSERT(m_pStyleArray != NULL);
	return (m_pStyleArray == NULL) ? NULL : m_pStyleArray->GetStyleChange(lChange);
}

CTextStyleChange* CTextRecord::ModifyStyleChange(STYLE_INDEX lChange)
{
	m_lModifyStyleChange++;

	LockStyleArray();
	ASSERT(m_pStyleArray != NULL);
	return (m_pStyleArray == NULL) ? NULL : m_pStyleArray->ModifyStyleChange(lChange);
}

void CTextRecord::DeleteStyleChange(STYLE_INDEX lChange, BOOL fAdjustCounts /*=TRUE*/)
{
	LockStyleArray();
	ASSERT(m_pStyleArray != NULL);
	if (m_pStyleArray != NULL)
	{
		modified();

		if (fAdjustCounts)
		{
			CTextStyleChange* pChange = GetStyleChange(lChange);
			// If this is a face style change, decrement its count.
			if (pChange->m_wToken == TOKEN_Face)
			{
				((PMGDatabase*)database)->free_face((DB_RECORD_NUMBER)pChange->m_lValue, FALSE);
			}
         else
         if (pChange->m_wToken == TOKEN_Hyperlink)
         {
            ((PMGDatabase*)database)->FreeHyperlinkRecord((DB_RECORD_NUMBER)pChange->m_lValue, FALSE);
         }
		}

		m_pStyleArray->DeleteStyleChange(lChange);
	}
}

/*
// Special variant of GetStyleChange() that allows lChange == -1 to access
// the paragraph style.
*/

CTextStyleChange* CTextRecord::GetStyleChange(PARAGRAPH_INDEX lParagraph, STYLE_INDEX lChange)
{
	if (lChange == -1)
	{
		ASSERT(lParagraph >= 0 && lParagraph < NumberOfParagraphs());
		CTextParagraph* pParagraph = GetParagraph(lParagraph);
		return &pParagraph->m_StyleChange;
	}
	else
	{
		return GetStyleChange(lChange);
	}
}

/*
// Special variant of ModifyStyleChange() that allows lChange == -1 to access
// the paragraph style.
*/

CTextStyleChange* CTextRecord::ModifyStyleChange(PARAGRAPH_INDEX lParagraph, STYLE_INDEX lChange)
{
	if (lChange == -1)
	{
		ASSERT(lParagraph >= 0 && lParagraph < NumberOfParagraphs());
		CTextParagraph* pParagraph = ModifyParagraph(lParagraph);
		return &pParagraph->m_StyleChange;
	}
	else
	{
		return ModifyStyleChange(lChange);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Paragraph routines.

PARAGRAPH_COUNT CTextRecord::NumberOfParagraphs(void)
{
	// Make sure we have a paragraph array.
	if (m_pParagraphArray == NULL)
	{
		LockParagraphArray();
		if (m_pParagraphArray == NULL)
		{
			ASSERT(FALSE);
			return 0;
		}
	}
	return m_pParagraphArray->ElementCount();
}

void CTextRecord::InsertParagraph(PARAGRAPH_INDEX Index, const CTextParagraph* pParagraph, PARAGRAPH_COUNT Count /*=1*/)
{
	// Make sure we have a paragraph array.
	if (m_pParagraphArray == NULL)
	{
		LockParagraphArray();
		if (m_pParagraphArray == NULL)
		{
			ASSERT(FALSE);
			return;
		}
	}

	((PMGDatabase*)database)->IncTextStyleRecord(pParagraph->m_lBaseParagraphStyle);
	m_pParagraphArray->Insert(Index, (LPVOID)pParagraph, Count);
}

void CTextRecord::DeleteParagraph(PARAGRAPH_INDEX Index, PARAGRAPH_COUNT Count /*=1*/)
{
	// Make sure we have a paragraph array.
	if (m_pParagraphArray == NULL)
	{
		LockParagraphArray();
		if (m_pParagraphArray == NULL)
		{
			ASSERT(FALSE);
			return;
		}
	}

	// Get the paragraph to decrement its base style.
	CTextParagraph* pParagraph = GetParagraph(Index);
	((PMGDatabase*)database)->FreeTextStyleRecord(pParagraph->m_lBaseParagraphStyle);

	// Do the actual deletion.
	m_pParagraphArray->Delete(Index, Count);
}

CTextParagraph* CTextRecord::GetParagraph(PARAGRAPH_INDEX Index)
{
	m_lGetParagraph++;

	// Make sure we have a paragraph array.
	if (m_pParagraphArray == NULL)
	{
		LockParagraphArray();
		if (m_pParagraphArray == NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}
	}
	return (CTextParagraph*)m_pParagraphArray->GetAt(Index);
}

CTextParagraph* CTextRecord::GetParagraph(PARAGRAPH_INDEX Index, PARAGRAPH_COUNT* pParagraphsAvailable)
{
	// Make sure we have a paragraph array.
	if (m_pParagraphArray == NULL)
	{
		LockParagraphArray();
		if (m_pParagraphArray == NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}
	}
	return (CTextParagraph*)m_pParagraphArray->GetAt(Index, (DWORD*)pParagraphsAvailable);
}

CTextParagraph* CTextRecord::ModifyParagraph(PARAGRAPH_INDEX Index)
{
	m_lModifyParagraph++;

	// Make sure we have a paragraph array.
	if (m_pParagraphArray == NULL)
	{
		LockParagraphArray();
		if (m_pParagraphArray == NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}
	}
	return (CTextParagraph*)m_pParagraphArray->ModifyAt(Index);
}

void CTextRecord::GetParagraphStyle(PARAGRAPH_INDEX Index, CTextStyle& Style, BOOL fApplyParagraphChanges /*=TRUE*/)
{
	m_lGetParagraphStyle++;

	// Lock the style record that this paragraph refers to.
	PMGDatabase* pDatabase = (PMGDatabase*)database;
	CTextParagraph* pParagraph = GetParagraph(Index);
	CTextStyleRecord* pStyleRecord;
	if (pParagraph->m_lBaseParagraphStyle == 0
		 || (pStyleRecord = (CTextStyleRecord*)pDatabase->get_record(pParagraph->m_lBaseParagraphStyle, NULL, RECORD_TYPE_TextStyle)) == NULL)
	{
		ASSERT(pParagraph->m_lBaseParagraphStyle == 0);
		// No base style or error getting record. Attempt a recovery using default style.
		if (Style.Database() != NULL)
		{
			pDatabase = Style.Database();
		}
		pDatabase->get_font_server()->SetDefault(&Style);
	}
	else
	{
		// Got the record. Grab its style.
		Style = pStyleRecord->Style();
		pStyleRecord->release();
	}
	ASSERT(Style.Size() != 0);
	ASSERT(Style.BaseSize() != 0);

	if (fApplyParagraphChanges)
	{
		ApplyParagraphChanges(pParagraph, Style);
	}
}

/*
// Set the paragraph's style.
// This places style changes into the paragraph to effect the style passed.
*/

void CTextRecord::SetParagraphStyle(PARAGRAPH_INDEX lParagraph, const CTextStyle* pNewStyle)
{
	CTextStyle CurrentStyle((PMGDatabase*)database);
	GetParagraphStyle(lParagraph, CurrentStyle, FALSE);
	CTextStyle NewStyle((PMGDatabase*)database, FALSE);

	if (pNewStyle == NULL)
	{
		pNewStyle = &CurrentStyle;
		/*
		// Why would you want pNewStyle to be NULL? Well, let's see what happens:
		// CurrentStyle will get the style for the paragraph not including all
		// the paragraph style changes. If we then run through this loop, the
		// base style will re-emerge.
		*/
	}
	else
	{
		NewStyle = *pNewStyle;
		pNewStyle = &NewStyle;
	}

	CTextParagraph* pParagraph = GetParagraph(lParagraph);
	
	// This is a lazy loop (i.e. there is a more optimized way to do this).
	// However, why write optimized code for something that is rarely called
	// and that readily available code handles adequately?

	for (int nToken = FIRST_TEXT_TOKEN;
							nToken < LAST_TEXT_TOKEN;
							nToken++)
	{
		if (pNewStyle->TestStyle(nToken))
		{
			ChangeParagraphStyle(pParagraph, lParagraph, &CurrentStyle, pNewStyle, nToken);
		}
	}

	NewStyle.Destroy();
}

/*
// Apply a paragraph's changes to the passed style.
*/

void CTextRecord::ApplyParagraphChanges(CTextParagraph* pParagraph, CTextStyle& Style)
{
	STYLE_INDEX lStyle = pParagraph->m_lFirstParagraphStyleChange;
	BOOL fUpdateMetrics = FALSE;

	while (lStyle != 0)
	{
		CTextStyleChange* pChange = GetStyleChange(lStyle);
		ASSERT(pChange != NULL);
		if (pChange == NULL)
		{
			break;
		}
		if (pChange->m_wToken >= FIRST_TEXT_TOKEN && pChange->m_wToken < LAST_TEXT_TOKEN)
		{
			fUpdateMetrics = TRUE;
		}
		// Apply the style change.
		Style.ApplyChange(pChange, FALSE);
		// Move to the next style change.
		lStyle = pChange->m_lNext;
	}

	if (fUpdateMetrics)
	{
		Style.UpdateFontMetrics();
	}
	// Apply the stretching to this style.
	ApplyStretch(Style);
}

/////////////////////////////////////////////////////////////////////////////
// Frame support.

// Find the frame corresponding to a character location.
// The frame is the current or recommended (best guess) frame.
// This may throw an exception.

DB_RECORD_NUMBER CTextRecord::GetFrameOfCharacter(CHARACTER_INDEX lCharacter, DB_RECORD_NUMBER lStartFrame /*=0*/, BOOL fEnd /*=FALSE*/)
{
	// Validate that the character exists.
	if (lCharacter < 0 || lCharacter > NumberOfCharacters())
	{
		ASSERT(FALSE);
		ThrowErrorcodeException(ERRORCODE_DoesNotExist);
	}

	// We need to find the frame the character is located in.

	CFrameRecord* pFrame = NULL;
	DB_RECORD_NUMBER lFrame = lStartFrame;
	if (lFrame <= 0)
	{
		lFrame = FirstFrame();
	}

	BOOL fFound = FALSE;

	TRY
	{
		int nDirection = 0;				// No direction yet.

		do
		{
			// Lock the frame. An error results in an exception.
			ERRORCODE error;
			pFrame = (CFrameRecord*)database->get_record(lFrame, &error, RECORD_TYPE_Frame);
			if (pFrame == NULL)
			{
				ThrowErrorcodeException(error);
			}

			// See how we stand in relation to this frame.
			CHARACTER_INDEX lFirstCharacter = pFrame->FirstCharacter();
			CHARACTER_COUNT lCharacters = pFrame->NumberOfCharacters();
			BOOL fBoundary;

			if (lCharacter < lFirstCharacter || lFirstCharacter == -1)
			{
				switch (nDirection)
				{
					case 1:
					{
						// Switching off. This is bad!
						ThrowErrorcodeException(ERRORCODE_IntError);
					}
					case 0:
					{
						// See if we should jump to the front.
						if (lCharacter < lFirstCharacter/2)
						{
							nDirection = 1;
							lFrame = FirstFrame();
							break;
						}
						nDirection = -1;
					// Fall through to...
					}
					case -1:
					{
						DB_RECORD_NUMBER lPreviousFrame = pFrame->PreviousFrame();
						if (lPreviousFrame == 0)
						{
							// Structures are inconsistent. Recover as best as we can.
							ASSERT(FALSE);
							fFound = TRUE;
							break;
						}
						else
						{
							lFrame = lPreviousFrame;
						}
						break;
					}
					default:
					{
						ASSERT(FALSE);
						break;
					}
				}
			}
			else if (lCharacter > lFirstCharacter + lCharacters
							|| (fBoundary = (!fEnd && lCharacter == lFirstCharacter + lCharacters)))
			{
				DB_RECORD_NUMBER lNextFrame = pFrame->NextFrame();
				if (fBoundary && lNextFrame != 0)
				{
					// Get the next frame.
					CFrameRecord* pNextFrame = (CFrameRecord*)database->get_record(lNextFrame, &error, RECORD_TYPE_Frame);
					if (pNextFrame == NULL)
					{
						ThrowErrorcodeException(error);
					}
					if (pNextFrame->FirstCharacter() == -1)
					{
						// Cannot move forward. We are at the right spot.
						fFound = TRUE;
					}
					pNextFrame->release();
				}

				// Equal or after.
				if (!fFound)
				{
					switch (nDirection)
					{
						case -1:
						{
							// Switching off. This is bad!
							ThrowErrorcodeException(ERRORCODE_IntError);
						}
						case 0:
						{
							nDirection = 1;
						// Fall through to...
						}
						case 1:
						{
							if (lNextFrame == 0)
							{
								// No next frame.
								// This is OK. Text may be off the end of the last frame.
								fFound = TRUE;
							}
							else
							{
								lFrame = lNextFrame;
							}
							break;
						}
						default:
						{
							ASSERT(FALSE);
							break;
						}
					}
				}
			}
			else
			{
				// Found it! Break out of the for loop.
				fFound = TRUE;
			}
			pFrame->release();
			pFrame = NULL;
		} while (!fFound);
	}
	CATCH_ALL(e)
	{
		if (pFrame != NULL)
		{
			pFrame->release();
		}
		THROW_LAST();

		// Sometimes exceptions don't throw if they can't be caught.
		ASSERT(FALSE);
		lFrame = 0;			// Did you TRY to CATCH me?
	}
	END_CATCH_ALL

	ASSERT(pFrame == NULL);
	return lFrame;
}

/////////////////////////////////////////////////////////////////////////////
// Stretching support.

/*
// Set stretching for this text stream.
*/

void CTextRecord::SetStretch(CFixed lSizeNumerator, CFixed lSizeDenominator, CFixed lExpansion)
{
	m_Record.m_lStretchNumerator = lSizeNumerator;
	m_Record.m_lStretchDenominator = lSizeDenominator;
	m_Record.m_lStretchExpansion = lExpansion;
	ASSERT(m_Record.m_lStretchDenominator != 0);
	ASSERT(m_Record.m_lStretchExpansion != 0);
	modified();
}

/*
// Clear stretching for this text stream.
*/

void CTextRecord::ClearStretch(void)
{
	m_Record.m_lStretchNumerator = 0;
	modified();
}

void CTextRecord::ApplyStretch(CTextStyle& Style)
{
	if (m_Record.m_lStretchNumerator != 0)
	{
		// Apply size stretch.
		ASSERT(m_Record.m_lStretchDenominator != 0);
		Style.Size(MulDivFixed(Style.Size(), m_Record.m_lStretchNumerator, m_Record.m_lStretchDenominator));

		// Apply expansion stretch.
		ASSERT(m_Record.m_lStretchExpansion != 0);
		Style.Expansion(MulFixed(Style.Expansion(), m_Record.m_lStretchExpansion));
		Style.UpdateFontMetrics();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Higher-level Text Manipulation routines.

ERRORCODE CTextRecord::InsertText(CHARACTER_INDEX lIndex, const CHARACTER* pText, CHARACTER_COUNT lCount /*=1*/, DB_RECORD_NUMBER lFrame /*=0*/)
{
	// Get a state so we can do something.

	CTextState State;

	TRY
	{
		InitializeTextState(&State, lIndex);

		PARAGRAPH_INDEX lParagraph = State.m_lParagraph;
		CHARACTER_INDEX lCharacterInParagraph = State.m_lCharacterInParagraph;

		// Set up some stuff for the state adjustment.
		PARAGRAPH_COUNT lStartParagraphs = NumberOfParagraphs();
		CHARACTER_COUNT lInsertCount = lCount;

		// Insertion.

		ASSERT(lCount > 0);

		while (lCount > 0)
		{
		// Insert characters into the current paragraph.
		// Accumulate characters up to and including the next carriage return.

			CTextParagraph* pParagraph = ModifyParagraph(lParagraph);

			CHARACTER_COUNT lThisCount = 0;
			BOOL fNewParagraph = FALSE;

			do
			{
				lCount--;
				if (pText[lThisCount++] == '\n')
				{
				// Hit the end of the paragraph.
					fNewParagraph = TRUE;
					break;
				}
			} while (lCount != 0);

			// Find the style change to increase.

			CTextStyleChange* pChange = &pParagraph->m_StyleChange;
			CHARACTER_INDEX lStyleChangeStart = 0;
			STYLE_INDEX lThisChange = -1;
			while (lStyleChangeStart + pChange->m_lCharacters < lCharacterInParagraph)
			{
				lStyleChangeStart += pChange->m_lCharacters;
				if (pChange->m_lNext == 0)
				{
					pChange = NULL;
					break;
				}
				pChange = GetStyleChange(lThisChange = pChange->m_lNext);
			}

			if (pChange != NULL)
			{
				pChange->m_lCharacters += lThisCount;
				if (lThisChange != -1)
				{
					ModifyStyleChange(lThisChange);
				}
			}

			// Add the text that we have accumulated for this paragraph.
			// (Could we just add all the text at once at the beginning? It seems
			// like that would save time...)

			InsertCharacter(lIndex, pText, lThisCount);
			lIndex += lThisCount;
			pText += lThisCount;

			// Bump the character count in the paragraph.
			pParagraph->m_lCharacters += lThisCount;

			if (fNewParagraph)
			{
			/*
			// We are adding text that ends with a carriage return.
			// This will split the current paragraph. (It is necessary
			// to pay attention to where we are in the paragraph for this.)
			*/

				// Split the current paragraph data.
				SplitParagraph(lParagraph, lCharacterInParagraph+lThisCount);

				// Move to the start of the new paragraph.
				lParagraph++;
				lCharacterInParagraph = 0;
			}
			else
			{
			/*
			// Straight text to end. Add it to the current paragraph.
			*/
				ASSERT(lCount == 0);
			}
		}

		AdjustFrameStates(&State,
								lInsertCount,
								NumberOfParagraphs() - lStartParagraphs,
								lFrame);
	}
	END_TRY
	return ERRORCODE_None;
}

/*
// Split a paragraph at an offset into another paragraph.
*/

void CTextRecord::SplitParagraph(PARAGRAPH_INDEX lParagraph, CHARACTER_INDEX lSplit)
{
	CTextIterator Iterator;
	CTextStyle StartStyle((PMGDatabase*)database);
	PARAGRAPH_INDEX lNewParagraph = lParagraph+1;

	TRY
	{
		// Build the new paragraph.
		CTextParagraph NewParagraph;

		// Add the new paragraph.
		InsertParagraph(lNewParagraph, &NewParagraph);

		// Get the source paragraph.
		CTextParagraph* pParagraph = ModifyParagraph(lParagraph);
		// Get the destination paragraph.
		CTextParagraph* pNewParagraph = ModifyParagraph(lNewParagraph);

		//
		// Duplicate the paragraph style changes over.
		// This is not hyper-critical, so recover from (ignore) exceptions.
		//

		TRY
		{
			CopyBaseStyle(pNewParagraph, pParagraph, (PMGDatabase*)GetDatabase());
			CopyParagraphFormatting(pNewParagraph, pParagraph, this, TRUE);
		}
		END_TRY

		//
		// Split the character stream.
		//

		pNewParagraph->m_lCharacters = pParagraph->m_lCharacters - lSplit;
		pParagraph->m_lCharacters = lSplit;

		//
		// Split the style stream.
		//

		// Prepare an iterator to parse the styles.
		Iterator.SetRecord(this);
		Iterator.ParagraphIndex(lParagraph);
		Iterator.ResetToParagraphStyle();

		// Find the style change that the offset sits on.
		// This style change is the one who ends 
		while (Iterator.NextChangeStart() <= lSplit
						&& Iterator.CurrentChange()->m_lNext != 0)
		{
			// Grab the current style change.
			Iterator.GrabCurrentChange();
			if (Iterator.CurrentChangeStart() == lSplit)
			{
				// Delete the current change.
				Iterator.DeleteCurrentChange();
				// If the current change extends past the split, stop now.
				if (Iterator.NextChangeStart() > lSplit)
				{
					// Stop without advancing.
					break;
				}
				// Next change begins at split also.
				// Continue on and grab it as well.
				// (Next should never start less than split.)
				ASSERT(Iterator.NextChangeStart() == lSplit);
			}
			Iterator.AdvanceCurrentChange();
		}

		// Now, we have the change that the split point sits on.
		// Split it.

		// Compute how much of the current style change we need to split.

		CTextStyleChange* pChange = Iterator.CurrentChange();
		CHARACTER_COUNT lCount = lSplit - Iterator.CurrentChangeStart();
		ASSERT(lCount >= 0);
		CHARACTER_COUNT lNewCount = Iterator.NextChangeStart() - lSplit;
		ASSERT(lNewCount >= 0);

		if (Iterator.CurrentChangeIndex() == -1)
		{
			// Splitting the paragraph style.
			// We just move the difference over to the new paragraph style.

			pNewParagraph->m_StyleChange.m_lCharacters = lNewCount;
			pNewParagraph->m_StyleChange.m_lNext = pChange->m_lNext;

			pChange->m_lCharacters = lCount;
			pChange->m_lNext = 0;
		}
		else
		{
			// Splitting a real style change.
			ModifyStyleChange(Iterator.CurrentChangeIndex());

			// Grab the current change.
			// We will be putting this change back in later.
			Iterator.GrabCurrentChange();

			// Point the base paragraph style to the style change following
			// the old style change.
			pNewParagraph->m_StyleChange.m_lNext = pChange->m_lNext;
			pNewParagraph->m_StyleChange.m_lCharacters = lNewCount;

			// Adjust the old style change.
			pChange->m_lCharacters = lCount;
			pChange->m_lNext = 0;

			// If the current change has zero characters in it, then it
			// is sitting at the end of the paragraph. It needs to be deleted.
			if (pChange->m_lCharacters == 0)
			{
				Iterator.DeleteCurrentChange();
			}

			if (pNewParagraph->m_lCharacters == 0)
			{
				SetParagraphStyle(lNewParagraph, &Iterator.Style());
			}
			else
			{
				// Now we need to add style changes back in to the new paragraph to
				// create the style that we had at the split point.
				// Remember the accumulated style.
				StartStyle = Iterator.Style();
				// Set up for the insertion.
				Iterator.ParagraphIndex(lNewParagraph);
				Iterator.ResetToParagraphStyle();

				// Insertion point is sitting at the front, which is where
				// we want it to be. There should be no style changes sitting
				// at the front of the text at this point.
				CTextStyleChange sc;

				for (sc.m_wToken = FIRST_TEXT_TOKEN;
									sc.m_wToken < LAST_TEXT_TOKEN;
									sc.m_wToken++)
				{
				/* If this entry has a defined change... */
					if (!Iterator.Style().CompareStyles(sc.m_wToken, &StartStyle))
					{
					/* Insert a token for this change. */

						Iterator.Style().BuildChange(&sc, &StartStyle);

#ifdef DEBUG_RCS
						TRACE("\tInsert token after split (%x: %lx)\n",
								sc.m_wToken,
								sc.m_lValue);
#endif
						// Use the iterator to insert the style change.
						Iterator.InsertStyleChange(sc);
					}
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		ASSERT(FALSE);
		Iterator.Reset();
		StartStyle.Reset();
		THROW_LAST();
	}
	END_CATCH_ALL
}

/*
// Delete text at a certain location.
// This needs to update the paragraphs and any affected style changes.
*/

ERRORCODE CTextRecord::DeleteText(CHARACTER_INDEX lIndex, CHARACTER_COUNT lCount /*=1*/, DB_RECORD_NUMBER lFrame /*=0*/)
{
	// Get a state so we can do something.

	ERRORCODE error;
	CTextState State;

	TRY
	{
		InitializeTextState(&State, lIndex);

		PARAGRAPH_INDEX lParagraph = State.m_lParagraph;
		CHARACTER_INDEX lCharacterInParagraph = State.m_lCharacterInParagraph;

		// Set up some stuff for the state adjustment.
		PARAGRAPH_COUNT lStartParagraphs = NumberOfParagraphs();
		CHARACTER_COUNT lDeleteCount = lCount;

		// First, we will delete the text.

		DeleteCharacter(lIndex, lCount);

		// Now, fixup the paragraphs.

		while (lCount != 0)
		{
			// Compute how much text is left in the current paragraph.
			// If there is not enough, paragraphs are merged until we
			// have enough.

			CTextParagraph* pParagraph = ModifyParagraph(lParagraph);
			CHARACTER_COUNT lThisCount = pParagraph->m_lCharacters - lCharacterInParagraph;

			if (lThisCount > lCount
				 || (lThisCount == lCount && lParagraph == NumberOfParagraphs()-1))
			{
				// We do not delete to the end. Just remove the characters.
				pParagraph->m_lCharacters -= lCount;
				DeleteStyles(lParagraph,
								 lCharacterInParagraph,
								 lCount);
				lCount = 0;
			}
			else
			{
				// The end of this paragraph is going away.
				// We need to merge it with the next one (which goes away).

				if (lParagraph < NumberOfParagraphs()-1)
				{
					if (lCharacterInParagraph == 0)
					{
						// All the text in this paragraph is going away!
						// We can simply remove this paragraph.
						ASSERT(lThisCount == pParagraph->m_lCharacters);
						DeleteParagraph(lParagraph);

						ASSERT(lThisCount <= lCount);
						lCount -= lThisCount;
					}
					else
					{
						MergeParagraphs(lParagraph);
					}
				}
				else
				{ 
					// Error!
					// There are no more paragraphs to delete!
					// An attempt was made to delete too much text.
					ASSERT(FALSE);
					// Only delete what is there.
					pParagraph->m_lCharacters -= lThisCount;
					DeleteStyles(lParagraph,
									 lCharacterInParagraph,
									 lThisCount);
					lCount = 0;
				}
			}
		}

		AdjustFrameStates(&State,
								-lDeleteCount,
								NumberOfParagraphs() - lStartParagraphs,
								lFrame);
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
	return error;
}

/*
// Adjust all the frame states after a change.
*/

void CTextRecord::AdjustFrameStates(const CTextState* pChangeState,
												CHARACTER_COUNT lCharacterDelta,
												PARAGRAPH_COUNT lParagraphDelta,
												DB_RECORD_NUMBER lFrame /*=0*/)
{
	ERRORCODE error;

	if (lFrame == 0)
	{
		lFrame = FirstFrame();
	}

	CFrameRecord* pFrame = NULL;

	TRY
	{
		CTextState LastState;
		BOOL fFirst = TRUE;

		while (lFrame != 0)
		{
			// Lock the current one.
			pFrame = (CFrameRecord*)database->get_record(lFrame, &error, RECORD_TYPE_Frame);
			if (pFrame == NULL)
			{
				// Could not get the frame. See 'error' to find out why.
				ASSERT(FALSE);
				break;
			}
			// Get the text state.
			CTextState* pState = &pFrame->TextState();
			if (fFirst && pState->m_lCharacter > pChangeState->m_lCharacter)
			{
				if (lFrame == FirstFrame())
				{
					ASSERT(FALSE);
					ThrowErrorcodeException(ERRORCODE_IntError);
				}
				// Bad parameter passed. Start over at the front.
				lFrame = FirstFrame();
			}
			else
			{
				// Adjust the state.
				AdjustTextState(pState,
									 pChangeState,
									 lCharacterDelta,
									 lParagraphDelta,
									 fFirst ? NULL : &LastState);
				LastState = *pState;
				lFrame = pFrame->NextFrame();
				fFirst = FALSE;
			}

			pFrame->release();
			pFrame = NULL;
		}
	}
	END_TRY

	if (pFrame != NULL)
	{
		pFrame->release();
	}
}

/*
// Adjust this state if necessary.
// The goal:
//  The text state will remain pointing at the same character it was before
//  the change (moved accordingly) unless that character gets deleted (in
//  which case, the text state will point to the change point).
*/

void CTextRecord::AdjustTextState(CTextState* pState,
											 const CTextState* pChangeState,
											 CHARACTER_COUNT lCharacterDelta,
											 PARAGRAPH_COUNT lParagraphDelta,
											 const CTextState* pLastState /*=NULL*/)
{
	// Cannot have paragraphs changed without characters changed.
	ASSERT(lCharacterDelta != 0 || lParagraphDelta == 0);
	CHARACTER_INDEX lCharacter = pChangeState->m_lCharacter;

	// See if we need to make any adjustments.
	// We need to adjust if:
	// Characters were added or deleted before our location.

	if (pState->m_lCharacter > lCharacter)
	{
		// After the change point. Things must move.

		if (pState->m_lCharacter + lCharacterDelta < pChangeState->m_lCharacter)
		{
			// The current character position got deleted. Set us back to
			// the change point.
			*pState = *pChangeState;
		}
		else
		{
			// The current position does not get deleted.
			// Move it to match the character we are attached to.

			PARAGRAPH_INDEX lParagraph = pChangeState->m_lParagraph;
			ASSERT(pState->m_lParagraph >= lParagraph);

			// Adjust the paragraph.

			if (lParagraphDelta == 0)
			{
				// Paragraph count has not changed.
				// Adjust the "character in paragraph" field if we are in the
				// change paragraph.

				if (pState->m_lParagraph == lParagraph)
				{
					// Bump the character in paragraph.
					pState->m_lCharacterInParagraph += lCharacterDelta;

					// Since no paragraphs were added or deleted, this must be true.
					ASSERT(pState->m_lCharacterInParagraph >= 0);
				}
			}
			else
			{
				// At least one new paragraph got inserted or deleted.

				if (pState->m_lParagraph != lParagraph
						&& pState->m_lParagraph + lParagraphDelta > lParagraph)
				{
					// Complete paragraph bumps up or down.
					// Simply adjust the paragraph. "Character in paragraph"
					// remains the same.
					pState->m_lParagraph += lParagraphDelta;
				}
				else
				{
					// Do something special.
					// Compute the new character position and then update the
					// state from the last state to the new position.

					CHARACTER_INDEX lNewCharacter = pState->m_lCharacter + lCharacterDelta;
					ASSERT(lNewCharacter >= lCharacter);
					*pState = *((pLastState == NULL) ? pChangeState : pLastState);
					lCharacterDelta = 0;
					AdvanceTextState(pState, lNewCharacter);
				}
			}

			// This is the easy part.
			pState->m_lCharacter += lCharacterDelta;
		}
	}
	else
	{
		// Before the change point. Not affected.
		ASSERT(pState->m_lParagraph <= pChangeState->m_lParagraph);
	}
}

/*
// Merge this paragraph with the next one.
*/

void CTextRecord::MergeParagraphs(PARAGRAPH_INDEX lParagraph)
{
	if (lParagraph != NumberOfParagraphs()-1)
	{
		TRY
		{
			// Get the paragraphs to work with.
			CTextParagraph* pParagraph = ModifyParagraph(lParagraph);
			CTextParagraph* pNextParagraph = ModifyParagraph(lParagraph+1);

			// Search for the last style change in the paragraph.
			CTextStyleChange* pLastChange = &pParagraph->m_StyleChange;
			STYLE_INDEX lLastChange = -1;
			while (pLastChange->m_lNext != 0)
			{
				pLastChange = GetStyleChange(lLastChange = pLastChange->m_lNext);
			}

			/*
			// Join the style changes.
			// To effect this, we are going to allocate an in-line style change
			// to take the place of the one in the paragraph we are deleting.
			// It should get deleted during the DeleteStyles() phase, but it will
			// have the effect of reverting to the paragraph style, and it gives
			// us a place to put the characters.
			*/

			// Allocate the new one.
			STYLE_INDEX lNewChange;
			// Duplicate the base style change from the next paragraph.
			CTextStyleChange* pNewChange = NewStyleChange(&lNewChange, &pNextParagraph->m_StyleChange);

			// Next paragraph style is nothing.
			pNextParagraph->m_StyleChange.m_lCharacters = 0;
			pNextParagraph->m_StyleChange.m_lNext = 0;

			// Hook us in after this. We re-get the last change in case it moved.
			if (lLastChange != -1)
			{
				pLastChange = GetStyleChange(lLastChange);
			}
			pLastChange->m_lNext = lNewChange;
			ModifyStyleChange(lParagraph, lLastChange);

			// We say this new change is the "last" one. We want to add our
			// paragraph-stored character formatting changes right after this
			// guy (if there are any).
			lLastChange = lNewChange;

			// Delete the paragraph style changes.
			// Any non-paragraph style changes found are appended onto the
			// regular style changes.

			DeleteParagraphStyles(pNextParagraph, lLastChange);

			//
			// Join the characters.
			//
			pParagraph->m_lCharacters += pNextParagraph->m_lCharacters;
			pNextParagraph->m_lCharacters = 0;
			pNextParagraph->m_StyleChange.m_lCharacters = 0;

			//	Delete the next paragraph.
			DeleteParagraph(lParagraph+1);
		}
		CATCH_ALL(e)
		{
			THROW_LAST();
		}
		END_CATCH_ALL
	}
}

/*
// Delete the paragraph styles in a paragraph.
// Character styles are alternately allowed to be saved.
*/

void CTextRecord::DeleteParagraphStyles(CTextParagraph* pParagraph, STYLE_INDEX lLastChange /*=0*/)
{
	STYLE_INDEX lChange;
	while ((lChange = pParagraph->m_lFirstParagraphStyleChange) != 0)
	{
		// Get the next style change.
		CTextStyleChange* pChange = GetStyleChange(lChange);
		if (pChange == NULL)
		{
			break;
		}
		// Unhook it from the list.
		pParagraph->m_lFirstParagraphStyleChange = pChange->m_lNext;

		// If it is a paragraph style change, delete it.
		if (pChange->m_wToken >= FIRST_PARAGRAPH_TOKEN
				&& pChange->m_wToken < LAST_PARAGRAPH_TOKEN)
		{
			DeleteStyleChange(lChange);
		}
		else if (lLastChange != 0)
		{
			// Otherwise, it is a character change. Hook it in after
			// the last "paragraph" change we added. Re-get the last
			// change so we can do the add (it may have moved).
			ModifyStyleChange(lChange);
			CTextStyleChange* pLastChange;
			if (lLastChange == -1)
			{
				pLastChange = &pParagraph->m_StyleChange;
				// Modification?
			}
			else
			{
				pLastChange = ModifyStyleChange(lLastChange);
			}

			// Hook us off the end.
			pChange->m_lNext = pLastChange->m_lNext;
			pLastChange->m_lNext = lChange;

			// We move the characters down the line. The last "paragraph"
			// style change must have the character count of the original
			// paragraph style change. All other style changes before this
			// one have a count of 0 (all changes bunch up at the front).
			pChange->m_lCharacters = pLastChange->m_lCharacters;
			pLastChange->m_lCharacters = 0;

			// We are now the last "paragraph" style change.
			lLastChange = lChange;
		}
	}
}

/*
// Delete the styles in a particular range.
*/

void CTextRecord::DeleteStyles(PARAGRAPH_INDEX lParagraph, CHARACTER_INDEX lStart, CHARACTER_COUNT lCount)
{
	CTextStyle StartStyle((PMGDatabase*)database);
	CTextIterator Iterator;

	TRACE("DeleteStyles(%ld: %ld, %ld)\n", lParagraph, lStart, lCount);

	TRY
	{
		/*
		// Adjusting the styles has two phases.
		//
		// Phase 1: Walk through the style changes and adjust the ones which are
		// in the passed range. All tokens stop at the deletion start
		// (bunching up) in order to preserve the style following that point.
		*/

		CTextParagraph* pParagraph = GetParagraph(lParagraph);

		BOOL fDeleted = FALSE;
		BOOL fDone = FALSE;

		// Prepare to loop.
		Iterator.SetRecord(this);

		Iterator.ParagraphIndex(lParagraph);
		Iterator.ResetToParagraphStyle();

		while (!fDone)
		{
			CTextStyleChange* pChange = Iterator.CurrentChange();
			if (pChange == NULL)
			{
				break;
			}

			// See if this style change needs to be adjusted.

			if (Iterator.CurrentChangeStart() == lStart
						|| Iterator.NextChangeStart() > lStart)
			{
				CHARACTER_COUNT lThisCount = pChange->m_lCharacters - (lStart - Iterator.CurrentChangeStart());
				if (lThisCount > lCount)
				{
					lThisCount = lCount;
					fDone = TRUE;
				}
				pChange->m_lCharacters -= lThisCount;
				lCount -= lThisCount;
				ASSERT(lCount >= 0);
			}

			// Check our bunching status.

			BOOL fDelete = Iterator.CurrentChangeStart() == lStart;
			if (fDelete && !fDeleted)
			{
				// First change to be deleted. Save style before.
				StartStyle = Iterator.Style();
			}

			// Grab the current change into the iterator style. We need to do
			// this before deleting but after we set the start style.

			Iterator.GrabCurrentChange();

			if (fDelete)
			{
#ifdef DEBUG_RCS
				TRACE("\tDelete token %ld (%x: %lx) size:%ld\n",
							Iterator.CurrentChangeIndex(),
							(int)Iterator.CurrentChange()->m_wToken,
							Iterator.CurrentChange()->m_lValue,
							Iterator.CurrentChange()->m_lCharacters);
#endif
				// Delete this bunched token.
				Iterator.DeleteCurrentChange();
				fDeleted = TRUE;
			}
			Iterator.AdvanceCurrentChange();
		}

		/*
		// Phase 2: Handle any tokens which were brought to the start point
		// and deleted. Reinsert any style changes which were not cancelled
		// or overridden.
		*/

		if (fDeleted && (lStart != pParagraph->m_lCharacters))
		{
			// Place it here.
			Iterator.BackupAChange();
			Iterator.CharacterIndex(lStart);
			Iterator.CharacterInParagraph(lStart);

			CTextStyleChange sc;

			for (sc.m_wToken = FIRST_TEXT_TOKEN;
								sc.m_wToken < LAST_TEXT_TOKEN;
								sc.m_wToken++)
			{
			/* If this entry has a defined change... */
				if (!StartStyle.CompareStyles(sc.m_wToken, &Iterator.Style()))
				{
				/* Insert a token for this change. */

					StartStyle.BuildChange(&sc, &Iterator.Style());

#ifdef DEBUG_RCS
					TRACE("\tInsert token (%x: %lx) size:%ld\n",
							sc.m_wToken,
							sc.m_lValue,
							sc.m_lCharacters);
#endif
					// Use the iterator to insert the style change.
					Iterator.InsertStyleChange(sc);
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		Iterator.Reset();
		StartStyle.Reset();
		THROW_LAST();
	}
	END_CATCH_ALL
}

//////////////////////////////////////////////////////////////
// SubstituteFonts()
//
// Walks through the style changes for this text record and
// tests if a font substitution is neccessary. If so modifies
// the style change to use the new font.
//
// returns true if a styel change was modifies
bool CTextRecord::SubstituteFonts( PMGFontServer *pFontServer )
{
	bool fModified = FALSE;

	// walk the style changes
	STYLE_COUNT count = NumberOfStyleChanges();
	for( STYLE_COUNT i = 1; i <= count; i++ )
	{
		CTextStyleChange* pChange = m_pStyleArray->GetStyleChange( i );
		// is it a font change
		if( pChange->m_wToken == TOKEN_Face )
		{
			DB_RECORD_NUMBER id = pFontServer->FindSubstitute( pChange->m_lValue );
			// found the substitute font ID
			if( id > 0 )
			{
				((PMGDatabase*)database)->free_face((DB_RECORD_NUMBER)pChange->m_lValue, FALSE);
				((PMGDatabase*)database)->inc_face(id, FALSE);
				pChange->m_lValue = id;
				m_pStyleArray->modified();
				fModified = true;
			}
		}
	}

#if 0 // is this neccessary?
	// Test the style change at the start of the paragraph. I
	PARAGRAPH_COUNT count = NumberOfParagraphs();
	for( PARAGRAPH_COUNT i = 0; i < count; i++ )
	{
		CTextParagraph *pParagraph = GetParagraph( i );
		if( pParagraph->m_StyleChange.m_wToken == TOKEN_Face )
		{
			DB_RECORD_NUMBER id = pFontServer->FindSubstitute( pParagraph->m_StyleChange.m_lValue );
			if( id > 0 )
			{
				((PMGDatabase*)database)->free_face((DB_RECORD_NUMBER)pParagraph->m_StyleChange.m_lValue, FALSE);
				((PMGDatabase*)database)->inc_face(id, FALSE);
				pParagraph->m_StyleChange.m_lValue = id;
				m_pParagraphArray->modified();
				fModified = true;
			}
		}
	}
#endif

	return fModified;
}

/////////////////////////////////////////////////////////////////////////////
// Storage routines.

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE CTextRecord::ReadData(StorageDevice* pDevice)
{
	ERRORCODE error = pDevice->read_record(&m_Record, sizeof(m_Record));
	if (error == ERRORCODE_None)
	{
		if (m_Record.m_wTokenRevision != TOKEN_Revision)
		{
//			TRACE("Before Fix...\n");
//			Dump();

			FixOldTokens();

//			TRACE("After Fix...\n");
		}
//		Dump();
	}
	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE CTextRecord::WriteData(StorageDevice* pDevice)
{
	m_Record.m_wTokenRevision = TOKEN_Revision;
	return pDevice->write_record(&m_Record, sizeof(m_Record));
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE CTextRecord::SizeofData(StorageDevice* pDevice)
{
	return pDevice->size_record(sizeof(m_Record));
}

/*
// Assign method.
*/

ERRORCODE CTextRecord::assign(DatabaseRecordRef srecord)
{
	ERRORCODE error;

	// Make sure we have no records.
	ReleaseRecords();
	DestroyRecords();

	// Assign the base record first.
	if ((error = DatabaseRecord::assign(srecord)) == ERRORCODE_None)
	{
	/*
	// Copy the data over.
	// Care must be taken with sub-records.
	*/
		CTextRecord& pref = (CTextRecord&)srecord;

		PMGDatabase* pSourceDatabase = (PMGDatabase*)pref.database;
		PMGDatabase* pDestDatabase = (PMGDatabase*)database;

		// Make sure we are added to the story list.
		// If this is happening because of a duplicate_record(), then we
		// have been created without going through the story mechanism.
		// Ugh!

		pDestDatabase->AddStory(Id());

		// Copy the record over.

		m_Record.m_wFlags = pref.m_Record.m_wFlags;
		m_Record.m_lCharacterArray = pSourceDatabase->duplicate_record(pref.m_Record.m_lCharacterArray, pDestDatabase);
		m_Record.m_lStyleArray = pSourceDatabase->duplicate_record(pref.m_Record.m_lStyleArray, pDestDatabase);
		m_Record.m_lParagraphArray = pSourceDatabase->duplicate_record(pref.m_Record.m_lParagraphArray, pDestDatabase);
		// The "first frame" field is left untouched. It is externally set and
		// is not our responsibility beyond not disturbing it.
		m_Record.m_lStretchNumerator = pref.m_Record.m_lStretchNumerator;
		m_Record.m_lStretchDenominator = pref.m_Record.m_lStretchDenominator;
		m_Record.m_lStretchExpansion = pref.m_Record.m_lStretchExpansion;
		m_Record.m_lMacroCount = pref.m_Record.m_lMacroCount;

		// If there is a macro character in here, and we are going to a new
		// database, we need to transfer it (them).

		if (HasMacro() && pSourceDatabase != pDestDatabase)
		{
			FixupMacros(pSourceDatabase);
		}

		// If we are copying from another database, we need to move all face
		// and text style record references over to our database.
		// Otherwise, we just need to increment all such references within our
		// database.

		PARAGRAPH_COUNT lParagraphs = NumberOfParagraphs();
		PARAGRAPH_COUNT lThisParagraphs = 0;
		CTextParagraph* pParagraph = NULL;

		for (PARAGRAPH_INDEX lParagraph = 0; lParagraph < lParagraphs; lParagraph++)
		{
			if (lThisParagraphs == 0)
			{
				pParagraph = GetParagraph(lParagraph, &lThisParagraphs);
			}

			CopyBaseStyle(pParagraph, pParagraph, pSourceDatabase);

			//
			// Handle all the face style changes.
			//

			ReferenceStyleChanges(pParagraph->m_lFirstParagraphStyleChange, pSourceDatabase, pDestDatabase);
			ReferenceStyleChanges(pParagraph->m_StyleChange.m_lNext, pSourceDatabase, pDestDatabase);

			pParagraph++;
			lThisParagraphs--;
		}
	}
	return error;
}

void CTextRecord::CopyBaseStyle(CTextParagraph* pDestParagraph,
										  CTextParagraph* pSourceParagraph,
										  PMGDatabase* pSourceDatabase)
{
	PMGDatabase* pDestDatabase = (PMGDatabase*)GetDatabase();
	DB_RECORD_NUMBER lBaseStyle = pSourceParagraph->m_lBaseParagraphStyle;

	if (pDestParagraph->m_lBaseParagraphStyle != 0)
	{
		pDestDatabase->FreeTextStyleRecord(pDestParagraph->m_lBaseParagraphStyle);
	}

	pDestParagraph->m_lBaseParagraphStyle = pDestDatabase->ImportTextStyleRecord(lBaseStyle, pSourceDatabase);

	// The root style change has the paragraph base style.
	pDestParagraph->m_StyleChange.m_lValue = pDestParagraph->m_lBaseParagraphStyle;
}

/*
// Copy the paragraph formatting.
// This may ignore character formatting style changes.
*/

void CTextRecord::CopyParagraphFormatting(CTextParagraph* pDestParagraph,
														CTextParagraph* pSourceParagraph,
														CTextRecord* pSourceText,
														BOOL fCopyCharacterFormatting /*=FALSE*/)
{
	ASSERT(pDestParagraph != pSourceParagraph);
	// If this is not true, then write code to advance lDestParent to the end
	// of the paragraph style changes.

	// If we are not copying character formatting, then leave it as is.
	STYLE_INDEX lCharacterChange = 0;
	if (!fCopyCharacterFormatting)
	{
		// We want to leave the character formatting as is.
		NewStyleChange(&lCharacterChange, &pDestParagraph->m_StyleChange);
	}

	// Delete the paragraph styles.
	// Any character changes will be saved if we want them to be.
	DeleteParagraphStyles(pDestParagraph, lCharacterChange);
	ASSERT(pDestParagraph->m_lFirstParagraphStyleChange == 0);

	// Hook the character changes back in if we wanted to save them.
	if (lCharacterChange != 0)
	{
		CTextStyleChange* pChange = GetStyleChange(lCharacterChange);
		pDestParagraph->m_lFirstParagraphStyleChange = pChange->m_lNext;
		DeleteStyleChange(lCharacterChange);
	}

	// Get the databases for conversion purposes.
	PMGDatabase* pSourceDatabase = (PMGDatabase*)pSourceText->GetDatabase();
	PMGDatabase* pDestDatabase = (PMGDatabase*)GetDatabase();

	// Prepare for the loop.
	STYLE_INDEX lStyle = pSourceParagraph->m_lFirstParagraphStyleChange;
	STYLE_INDEX lDestParent = -1;

	// Loop until all paragraph formatting style changes have been copied.
	while (lStyle != 0)
	{
		CTextStyleChange* pChange = pSourceText->GetStyleChange(lStyle);
		// If this is a paragraph formatting token, copy it over.
		if (fCopyCharacterFormatting
				|| (pChange->m_wToken >= FIRST_PARAGRAPH_TOKEN
					 && pChange->m_wToken < LAST_PARAGRAPH_TOKEN))
		{
			// Allocate a new style change.
			STYLE_INDEX lNewStyle;
			CTextStyleChange* pNewChange = NewStyleChange(&lNewStyle);
			// Reload the style change in case it moved.
			pChange = pSourceText->GetStyleChange(lStyle);
			*pNewChange = *pChange;
			ReferenceStyleChange(pNewChange, pSourceDatabase, pDestDatabase);

			STYLE_INDEX* pIndex;
			if (lDestParent == -1)
			{
				// First token. Store in the paragraph.
				pIndex = &pDestParagraph->m_lFirstParagraphStyleChange;
			}
			else
			{
				// We need to reget the parent each time since the style change
				// allocate can move the array around.
				pIndex = &GetStyleChange(lDestParent)->m_lNext;
			}
			pNewChange->m_lNext = *pIndex;
			*pIndex = lNewStyle;

			lDestParent = lNewStyle;
		}
		lStyle = pChange->m_lNext;
	}
}

/*
// Copy the character formatting.
*/

void CTextRecord::CopyCharacterFormatting(CTextParagraph* pDestParagraph,
														CTextParagraph* pSourceParagraph,
														CTextRecord* pSourceText)
{
	ASSERT(pDestParagraph != pSourceParagraph);

	// If this is not true, then write code to advance lDestParent to the end
	// of the paragraph style changes.
	ASSERT(pDestParagraph->m_StyleChange.m_lNext == 0);

	// Get the databases for conversion purposes.
	PMGDatabase* pSourceDatabase = (PMGDatabase*)pSourceText->GetDatabase();
	PMGDatabase* pDestDatabase = (PMGDatabase*)GetDatabase();

	// Prepare for the loop.
	STYLE_INDEX lStyle = pSourceParagraph->m_StyleChange.m_lNext;
	STYLE_INDEX lDestParent = -1;

	// Loop until all paragraph formatting style changes have been copied.
	while (lStyle != 0)
	{
		CTextStyleChange* pChange = pSourceText->GetStyleChange(lStyle);

		// Allocate a new style change.
		STYLE_INDEX lNewStyle;
		CTextStyleChange* pNewChange = NewStyleChange(&lNewStyle);
		*pNewChange = *pChange;
		pNewChange->m_lNext = 0;
		ReferenceStyleChange(pNewChange, pSourceDatabase, pDestDatabase);

		if (lDestParent == -1)
		{
			// First token. Store in the paragraph.
			pDestParagraph->m_StyleChange.m_lNext = lNewStyle;
		}
		else
		{
			// We need to reget the parent each time since the style change
			// allocate can move the array around.
			CTextStyleChange* pParentChange = GetStyleChange(lDestParent);
			pParentChange->m_lNext = lNewStyle;
		}
		lDestParent = lNewStyle;
		lStyle = pChange->m_lNext;
	}
}

/*
// Handle all references in the style changes. These usually are face changes.
*/

void CTextRecord::ReferenceStyleChanges(STYLE_INDEX lChange, PMGDatabase* pSourceDatabase, PMGDatabase* pDestDatabase)
{
	while (lChange != 0)
	{
		CTextStyleChange* pChange = GetStyleChange(lChange); 
		if (ReferenceStyleChange(pChange, pSourceDatabase, pDestDatabase))
		{
			ModifyStyleChange(lChange);
		}
		lChange = pChange->m_lNext;
	}
}

/*
// Handle a new reference to a face style change.
// Returns whether the style change has been modified.
*/

BOOL CTextRecord::ReferenceStyleChange(CTextStyleChange* pChange, PMGDatabase* pSourceDatabase, PMGDatabase* pDestDatabase)
{
	BOOL fChanged = FALSE;
	if (pChange->m_wToken == TOKEN_Face)
	{
		DB_RECORD_NUMBER lFont = (DB_RECORD_NUMBER)pChange->m_lValue;

		if (pDestDatabase == pSourceDatabase)
		{
			pDestDatabase->inc_face(lFont);
		}
		else
		{
			CHAR name[80];

			/*
			// Database to database transfer.
			// We need to transfer the font record by name.
			*/

			pSourceDatabase->font_data_name(lFont, name);
			pChange->m_lValue = (long)pDestDatabase->new_font_record(name);
			pChange->m_lOldValue = 0;		// Not really used.
		}
		fChanged = TRUE;
	}
   else
	if (pChange->m_wToken == TOKEN_Hyperlink)
	{
		DB_RECORD_NUMBER link;
      if ((link = (DB_RECORD_NUMBER)pChange->m_lValue) != 0)
      {
		   if (pDestDatabase == pSourceDatabase)
		   {
			   pDestDatabase->IncrementHyperlinkRecord(link);
		   }
		   else
		   {
	         ERRORCODE         error;
            HyperlinkData     Data;
            HyperlinkRecord   *phyperdata;

			   if ((phyperdata = (HyperlinkRecord*)pSourceDatabase->get_record(link, &error, RECORD_TYPE_Hyperlink)) != NULL)
            {
               phyperdata->GetData(&Data);
               phyperdata->release();
            }
         
            pChange->m_lValue = pDestDatabase->NewHyperlinkRecord(&Data);
            fChanged = TRUE;
		   }
		   fChanged = TRUE;
      }
	}

	return fChanged;
}

/*
// Fixup all references to macros in this paragraph according to the database
// passed.
*/

void CTextRecord::FixupMacros(PMGDatabase* pSrcDatabase, CHARACTER_INDEX lStart /*=0*/, CHARACTER_INDEX lEnd /*=-1*/)
{
	MacroListRecord* pSrcList = pSrcDatabase->GetMacroList();
	if (pSrcList != NULL)
	{
		MacroListRecord* pDstList = ((PMGDatabase*)database)->GetMacroList();
		if (pDstList != NULL)
		{
			// Fixup the end if the default argument was passed.
			if (lEnd == -1)
			{
				lEnd = NumberOfCharacters();
			}

			// Look for all the macro characters in this range.

			CHARACTER* cp = NULL;
			CHARACTER_COUNT lCharacters = 0;

			ASSERT(m_pCharacterArray != NULL);

			while (lStart < lEnd)
			{
				// Grab the next character. Prime the character pointer if necessary.
				if (lCharacters == 0)
				{
					cp = (CHARACTER*)m_pCharacterArray->GetAt(lStart, (DWORD*)&lCharacters);
				}
				CHARACTER c = *cp;

				if (c >= MACRO_CHARACTER && c <= MACRO_CHARACTER_MAX)
				{
					WORD wMacroId = pDstList->ImportMacro(c - MACRO_CHARACTER, pSrcList);
					if (wMacroId == 0)
					{
						// Assign it something harmless.
						*cp = '?';

						// One less macro. Do not let macro count go negative in
						// case things are inconsistent.

						ASSERT(m_Record.m_lMacroCount > 0);
						if (m_Record.m_lMacroCount > 0)
						{
							m_Record.m_lMacroCount--;
						}
					}
					else
					{
					/* Assign it the new value. */
						*cp = wMacroId + MACRO_CHARACTER;
					}
				}
				cp++;
				lCharacters--;
				lStart++;
			}

			pDstList->release();
		}
		pSrcList->release();
	}
}

/*
// Change the style of a range of characters.
*/

int CTextRecord::ChangeRangeStyle(CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd, const CTextStyle* pNewStyle)
{
	PMGDatabasePtr pDatabase = (PMGDatabasePtr)database;

	// If the style belongs to another database, bring it local.
	// This is often the case during text pasting.

	CTextStyle LocalStyle(pDatabase, FALSE);		// Must be FALSE.
	BOOL fLocal = FALSE;

	int nRet = 0;

	TRY
	{
		if (pNewStyle->Database() != pDatabase)
		{
			LocalStyle = *pNewStyle;
			pNewStyle = &LocalStyle;
			fLocal = TRUE;
		}

	#ifdef DEBUG_RCS
		od("ChangeRangeStyle %ld to %ld", lStart, lEnd);
	#endif

	#ifdef DEBUG_RCS
		od(" (cnt:%d)\r\n", NumberOfStyleChanges());
	#endif

	/* Fix the range. */

		if (lStart < 0)
		{
			lStart = 0;
		}

		if (lEnd <= 0 || lEnd > NumberOfCharacters())
		{
			lEnd = NumberOfCharacters();
		}

		// Run through the character range and apply the style changes to each
		// paragraph we run through.

		CTextState State;
		State.Reset();

		while (lStart < lEnd)
		{
			// Advance the state so we can pick up the paragraph info.
			AdvanceTextState(&State, lStart);

			// Grab the paragraph.
			CTextParagraph* pParagraph = GetParagraph(State.m_lParagraph);

			// Compute the last index in the paragraph.
			CHARACTER_INDEX lParagraphStart = State.m_lCharacter
														- State.m_lCharacterInParagraph;
			CHARACTER_INDEX lParagraphEnd = lParagraphStart
														+ pParagraph->m_lCharacters;

			// Clip to the end.
			if (lParagraphEnd > lEnd)
			{
				lParagraphEnd = lEnd;
			}

			// Change the range style for this paragraph.
			nRet += ChangeParagraphRangeStyle(State, lParagraphEnd, pNewStyle);

			// Move to here next time.
			lStart = lParagraphEnd;
		}
	}
	CATCH_ALL(e)
	{
		nRet = -1;
	}
	END_CATCH_ALL

	// If we needed a local style (to move the style over from another database),
	// we needed it to be non-temporary (to do the copy); this means we need to
	// destroy() it now to free the face.

	if (fLocal)
	{
		LocalStyle.Destroy();
	}

	return nRet;
}

/*
// Change the range style within one paragraph.
*/

int CTextRecord::ChangeParagraphRangeStyle(const CTextState& TextState, CHARACTER_INDEX lEnd, const CTextStyle* pNewStyle)
{
/*
// We proceed as follows:
// (1) Get the style of the first character in this range, so we can know
// where we're starting.
// (2) Put any tokens necessary at the start of the range to set us up with
// the new style at that location.
// (3) March through the style changes (from start to end). If a style change
// has been superceded, remove it from the array of changes. As we parse
// style changes, build a composite style of what the style used to be so we
// will know what the end style is.
// (4) Put tokens in at the end of the range to insure that the style following
// the range remains intact.
*/

	CTextIterator Iterator;
	int nRet = 0;

	TRY
	{
	/* Find the style at the beginning of the range. */

#ifdef DEBUG_RCS
		TRACE("Find start of range...\n");
#endif

		CTextStyleChange sc;

	/* Compute which character to get the style for. */

		Iterator.SetRecord(this);
		// get it now... SetRecord may have moved the state array.
		Iterator.State() = TextState;
		Iterator.ResetToParagraphStyle();

		CTextParagraph* pParagraph = GetParagraph(TextState.m_lParagraph);

		while (Iterator.NextChangeStart() < TextState.m_lCharacter)
		{
			if (Iterator.CurrentChange() == NULL)
			{
				ASSERT(FALSE);
				ThrowErrorcodeException(ERRORCODE_IntError);
			}
			Iterator.AdvanceCurrentChange();
			Iterator.GrabCurrentChange();
#ifdef DEBUG_RCS
			const CTextStyleChange* pChange = Iterator.CurrentChange();
			ASSERT(pChange != NULL);
			TRACE("\tgrab change (%x: %lx) size:%ld\n",
					(int)pChange->m_wToken,
					pChange->m_lValue,
					pChange->m_lCharacters);
#endif
		}

	/*
	// We may need to put tokens in at the front of the range to set us up with
	// the new style. Do that now.
	*/

#ifdef DEBUG_RCS
		TRACE("Insert tokens before...\n");
#endif
		for (sc.m_wToken = FIRST_TEXT_TOKEN;
							sc.m_wToken < LAST_TEXT_TOKEN;
							sc.m_wToken++)
		{
		/* If this entry has a defined change... */
			if (pNewStyle->TestStyle(sc.m_wToken)
				 && !pNewStyle->CompareStyles(sc.m_wToken, &Iterator.Style()))
			{
			/* Insert a token for this change. */

				Iterator.Style().BuildChange(&sc, pNewStyle);

#ifdef DEBUG_RCS
				TRACE("\tInsert token (%x: %lx) size:%ld\n",
						sc.m_wToken,
						sc.m_lValue,
						sc.m_lCharacters);
#endif
				// Use the iterator to insert the style change.
				Iterator.InsertStyleChange(sc);
				nRet++;
			}
		}

	/*
	// Now we have the style at the start of the range.
	// Begin processing the tokens.
	// Delete any which are represented in the new style.
	*/

#ifdef DEBUG_RCS
		TRACE("Parse existing range tokens...\n");
#endif

		// The first style is the one we just inserted.
		// Do not do things to it.
		BOOL fFirstStyle = TRUE;

		while (Iterator.NextChangeStart() <= lEnd && Iterator.CurrentChange()->m_lNext != 0)
		{
			Iterator.AdvanceCurrentChange();

			if (Iterator.CurrentChange() == NULL)
			{
				ASSERT(FALSE);
				ThrowErrorcodeException(ERRORCODE_IntError);
			}
		/* Update the "old" style in the iterator. */

			if (pNewStyle->TestStyle(Iterator.CurrentChange()->m_wToken))
			{
				// Component is changing.
				// Grab the change and delete the token.

				Iterator.GrabCurrentChange();

#ifdef DEBUG_RCS
				TRACE("\tDelete token %ld (%x: %lx) size:%ld\n",
							Iterator.CurrentChangeIndex(),
							(int)Iterator.CurrentChange()->m_wToken,
							Iterator.CurrentChange()->m_lValue,
							Iterator.CurrentChange()->m_lCharacters);
#endif
			/*
			// Delete it!
			// This will merge the current change into the previous one.
			*/

				Iterator.DeleteCurrentChange();

				nRet++;
			}
		}

	/*
	// Now, we need to put tokens in at the end of the range to fix up the style
	// to be what it was before.
	*/

#ifdef DEBUG_RCS
		TRACE("Insert tokens after the range...\n");
#endif

		CHARACTER_INDEX lEndCharacterInParagraph = lEnd - (TextState.m_lCharacter - TextState.m_lCharacterInParagraph);
		if (lEndCharacterInParagraph != pParagraph->m_lCharacters)
		{
			Iterator.State().m_lCharacter = lEnd;
			Iterator.State().m_lCharacterInParagraph = lEndCharacterInParagraph;

			for (sc.m_wToken = FIRST_TEXT_TOKEN;
								sc.m_wToken < LAST_TEXT_TOKEN;
								sc.m_wToken++)
			{
			/* If this entry has a defined change in the new style... */

				if (pNewStyle->TestStyle(sc.m_wToken)
						&& !pNewStyle->CompareStyles(sc.m_wToken, &Iterator.Style()))
				{
				/* Insert a token for this change. */

					pNewStyle->BuildChange(&sc, &Iterator.Style());
#ifdef DEBUG_RCS
					TRACE("\tInsert token (%x: %lx) size:%ld\n",
							(int)sc.m_wToken,
							sc.m_lValue,
							sc.m_lCharacters);
#endif
					// Use the iterator to insert the style change.
					Iterator.InsertStyleChange(sc);
					nRet++;
				}
			}
		}

#ifdef DEBUG_RCS
		TRACE("Done (cnt:%ld)...\n", NumberOfStyleChanges());
		Dump();
#endif
	}
	CATCH_ALL(e)
	{
		Iterator.Reset();
		THROW_LAST();
	}
	END_CATCH_ALL

	return nRet;
}

/*
// Change the style of a range of paragraphs given:
//
// (1) The paragraph range.
// (2) The new style
//
// Returns the number of changes.
*/

int CTextRecord::ChangeParagraphStyle(PARAGRAPH_INDEX lStart, PARAGRAPH_INDEX lEnd, const CTextStyle* pNewStyle)
{
	// Validate arguments.
	if (lStart < 0)
	{
		lStart = 0;
	}

	if (lEnd < 0 || lEnd >= NumberOfParagraphs())
	{
		lEnd = NumberOfParagraphs()-1;
	}

	CTextStyle BaseStyle;

	int nChanged = 0;
	TRY
	{
		PARAGRAPH_COUNT lParagraphs = 0;
		CTextParagraph* pParagraph = NULL;
		while (lStart <= lEnd)
		{
			GetParagraphStyle(lStart, BaseStyle, FALSE);
			if (lParagraphs == 0)
			{
				pParagraph = GetParagraph(lStart, &lParagraphs);
			}

			int nTokensChanged = 0;
			for (int nToken = FIRST_PARAGRAPH_TOKEN;
						nToken < LAST_PARAGRAPH_TOKEN;
						nToken++)
			{
				if (pNewStyle->TestStyle(nToken))
				{
					nTokensChanged += ChangeParagraphStyle(pParagraph, lStart, &BaseStyle, pNewStyle, nToken);
				}
			}
			if (nTokensChanged)
			{
				// Another paragraph changed.
				nChanged++;
			}
			// Next paragraph.
			lStart++;
			lParagraphs--;
			pParagraph++;
		}
	}
	CATCH_ALL(e)
	{
		BaseStyle.Reset();
		THROW_LAST();
	}
	END_CATCH_ALL
	return nChanged;
}

/*
// Change the style of a range of paragraphs given:
//
// (1) The paragraph range.
// (2) The new style
// (3) The particular token that changed.
//
// Returns the number of changes.
*/

int CTextRecord::ChangeParagraphStyle(PARAGRAPH_INDEX lStart, PARAGRAPH_INDEX lEnd, const CTextStyle* pNewStyle, int Token)
{
	CTextStyle BaseStyle;

	int nChanged = 0;
	TRY
	{
		PARAGRAPH_COUNT lParagraphs = 0;
		CTextParagraph* pParagraph = NULL;

		while (lStart <= lEnd)
		{
			// Grab the base paragraph style.
			GetParagraphStyle(lStart, BaseStyle, FALSE);

			if (lParagraphs == 0)
			{
				// Grab the paragraph.
				pParagraph = GetParagraph(lStart, &lParagraphs);
			}

			// Change the paragraph style.
			nChanged += ChangeParagraphStyle(pParagraph, lStart, &BaseStyle, pNewStyle, Token);

			// Next paragraph.
			lStart++;
			pParagraph++;
			lParagraphs--;
		}
	}
	CATCH_ALL(e)
	{
		BaseStyle.Reset();
		THROW_LAST();
	}
	END_CATCH_ALL
	return nChanged;
}

/*
// Change the style of a paragraph given:
// (1) The paragraph.
// (2) The base style
// (3) The new style
// (4) The particular token that changed.
//
// Returns the number of changes (0 or 1)
*/

int CTextRecord::ChangeParagraphStyle(CTextParagraph* pParagraph, PARAGRAPH_INDEX lParagraph, const CTextStyle* pBaseStyle, const CTextStyle* pNewStyle, int Token)
{
	// Determine whether the base style matches the new attribute.
	BOOL fSame = pBaseStyle->CompareStyles(Token, pNewStyle);

	// If not the same, build a style change.
	CTextStyleChange NewChange;
	if (!fSame)
	{
		memset(&NewChange, 0, sizeof(NewChange));
		NewChange.m_wToken = Token;
		pBaseStyle->BuildChange(&NewChange, pNewStyle);
	}

	// Count if we changed or not.
	int nChanged = 0;

	// Now run through the style changes and see if it exists.
	STYLE_INDEX* pChangeIndex = &pParagraph->m_lFirstParagraphStyleChange;
	BOOL fAdd = !fSame;
	STYLE_INDEX lThisChange = -1;

	while (*pChangeIndex != 0)
	{
		CTextStyleChange* pChange = GetStyleChange(*pChangeIndex);
		ASSERT(pChange != NULL);
		if (pChange->m_wToken == (WORD)Token)
		{
			// We found it!
			if (fSame)
			{
				// Style attributes are now the same. We need to delete override.
				STYLE_INDEX lStyle = *pChangeIndex;
				*pChangeIndex = pChange->m_lNext;
				// The change pointed to by pChangeIndex has been modified.
				if (lThisChange == -1)
				{
					ModifyParagraph(lParagraph);
				}
				else
				{
					ModifyStyleChange(lThisChange);
				}
				DeleteStyleChange(lStyle);
				// This is a change.
				nChanged = 1;
			}
			else
			{
				// We need this override. Make sure the value is correct.
				if (pChange->m_lValue != NewChange.m_lValue)
				{
					// Set the new value.
					pChange->m_lValue = NewChange.m_lValue;
					// Style element indirectly modified.
					ASSERT(m_pStyleArray != NULL);
					ModifyStyleChange(*pChangeIndex);
					// This is a change.
					nChanged = 1;
				}
				fAdd = FALSE;
			}
			break;
		}
		lThisChange = *pChangeIndex;
		pChangeIndex = &pChange->m_lNext;
	}

	// If we need to add it, do so now.
	if (fAdd)
	{
		// Allocate the new style change.
		STYLE_INDEX lNewChange;
		CTextStyleChange* pNewChange = NewStyleChange(&lNewChange, &NewChange);

		// Hook the new change into the list.
		pNewChange->m_lNext = pParagraph->m_lFirstParagraphStyleChange;
		pParagraph->m_lFirstParagraphStyleChange = lNewChange;
		ModifyParagraph(lParagraph);

		// This is a change.
		nChanged = 1;
	}

	return nChanged;
}

/*
// Change the tabs of a range of paragraphs.
*/

int CTextRecord::ChangeParagraphTabs(PARAGRAPH_INDEX lStart, PARAGRAPH_INDEX lEnd, CTabArray* pTabs, BOOL fRelative /*=FALSE*/)
{
	CTextStyle BaseStyle;

	int nChanged = 0;
	TRY
	{
		while (lStart <= lEnd)
		{
			// Grab the base paragraph style.
			GetParagraphStyle(lStart, BaseStyle, FALSE);

			// Change the paragraph tabs.
			nChanged += ChangeParagraphTabs(lStart, &BaseStyle, pTabs, fRelative);

			// Next paragraph.
			lStart++;
		}
	}
	CATCH_ALL(e)
	{
		BaseStyle.Reset();
		THROW_LAST();
	}
	END_CATCH_ALL
	return nChanged;
}

/*
// Change the tabs of a particular paragraph.
*/

int CTextRecord::ChangeParagraphTabs(PARAGRAPH_INDEX lParagraph, CTextStyle* pBaseStyle, CTabArray* pTabs, BOOL fRelative)
{
	CTabArray* pOldTabs = &pBaseStyle->m_Tabs;
	CTextStyle Style;

	// Grab the paragraph.
	CTextParagraph* pParagraph = GetParagraph(lParagraph);

	if (fRelative)
	{
		// Grab the base style.
		Style = *pBaseStyle;

		// Apply the base style changes.
		for (STYLE_INDEX lStyle = pParagraph->m_lFirstParagraphStyleChange;
			  lStyle != 0;)
		{
			CTextStyleChange* pChange = GetStyleChange(lStyle);
			ASSERT(pChange != NULL);
			if (pChange == NULL)
			{
				break;
			}
			// Apply the style change.
			Style.ApplyChange(pChange, FALSE);
			// Move to the next style change.
			lStyle = pChange->m_lNext;
		}

		// Merge in the new tabs.
		int nTabs = pTabs->NumberOfTabs();
		for (int nTab = 0; nTab < nTabs; nTab++)
		{
			CTextTab* pTab = pTabs->GetAt(nTab);
			if (pTab != NULL)
			{
				CTextStyleChange Change;
				pTab->ToStyleChange(&Change);
				Style.ApplyChange(&Change, FALSE);
			}
		}
		Style.m_Tabs.DefaultTabSize(pTabs->DefaultTabSize());
		pTabs = &Style.m_Tabs;
	}

	// Run through the existing paragraph changes and remove all tab changes.
	DeleteTabChanges(pParagraph, lParagraph);

	// Now reconstruct the tab style changes.
	// First, check for tabs to be added.

	int nNewTabs = pTabs->NumberOfTabs();
	for (int nNewTab = 0; nNewTab < nNewTabs; nNewTab++)
	{
		AddTab(pParagraph, pOldTabs, pTabs->GetAt(nNewTab), lParagraph);
	}

	// Second, check for tabs to be removed.

	int nOldTabs = pOldTabs->NumberOfTabs();
	for (int nOldTab = 0; nOldTab < nOldTabs; nOldTab++)
	{
		RemoveTab(pParagraph, pTabs, pOldTabs->GetAt(nOldTab), lParagraph);
	}

	// Finally, see if we need to put in a change for the default tab size.
	CFixed lDefaultTabSize = pTabs->DefaultTabSize();
	if (lDefaultTabSize != UNDEFINED_CFIXED && lDefaultTabSize != pBaseStyle->m_Tabs.DefaultTabSize())
	{
		// Allocate a style change for the default tab size.
		STYLE_INDEX lChange;
		CTextStyleChange* pChange = NewStyleChange(&lChange);

		// Construct the change by hand.
		memset(pChange, 0, sizeof(CTextStyleChange));
		pChange->m_wToken = TOKEN_DefaultTabSize;
		pChange->m_lValue = lDefaultTabSize;
		pChange->m_lOldValue = pBaseStyle->m_Tabs.DefaultTabSize();

		// Hook it into the chain.
		pChange->m_lNext = pParagraph->m_lFirstParagraphStyleChange;
		pParagraph->m_lFirstParagraphStyleChange = lChange;
		ModifyParagraph(lParagraph);
	}

	return 0;
}

/*
// Delete all tab changes from the style change list.
*/

void CTextRecord::DeleteTabChanges(CTextParagraph* pParagraph, PARAGRAPH_INDEX lParagraph)
{
	STYLE_INDEX lStyle;
	STYLE_INDEX lParentStyle = -1;
	for (lStyle = pParagraph->m_lFirstParagraphStyleChange; lStyle != 0; )
	{
		CTextStyleChange* pStyle = GetStyleChange(lStyle);
		if (pStyle == NULL)
		{
			ASSERT(FALSE);
			break;
		}
		STYLE_INDEX lNext = pStyle->m_lNext;

		// If this is a token to be deleted, delete it now.
		if (pStyle->m_wToken == TOKEN_TabStop
				|| pStyle->m_wToken == TOKEN_DefaultTabSize)
		{
			// Remove this style change.
			if (lParentStyle == -1)
			{
				pParagraph->m_lFirstParagraphStyleChange = lNext;
				ModifyParagraph(lParagraph);
			}
			else
			{
				CTextStyleChange* pParentChange = ModifyStyleChange(lParentStyle);
				pParentChange->m_lNext = lNext;
				ASSERT(m_pStyleArray != NULL);
			}
			DeleteStyleChange(lStyle);
		}
		else
		{
			lParentStyle = lStyle;
		}
		lStyle = lNext;
	}
}

/*
// Add a tab.
*/

void CTextRecord::AddTab(CTextParagraph* pParagraph, CTabArray* pTabs, const CTextTab* pTab, PARAGRAPH_INDEX lParagraph)
{
	if (pTab != NULL && pTab->m_nType != CTextTab::TAB_remove)
	{
		BOOL fAdd = TRUE;
		int nTab = pTabs->GetTabAt(pTab->m_Position);
		if (nTab != -1)
		{
			CTextTab* pCurrentTab = pTabs->GetAt(nTab);
			if (memcmp(pCurrentTab, pTab, sizeof(CTextTab)) == 0)
			{
				fAdd = FALSE;
			}
		}

		if (fAdd)
		{
			AddTabChange(pParagraph, pTab, lParagraph);
		}
	}
}

/*
// Remove a tab if it doesn't exist in the passed array.
*/

void CTextRecord::RemoveTab(CTextParagraph* pParagraph, CTabArray* pTabs, const CTextTab* pTab, PARAGRAPH_INDEX lParagraph)
{
	if (pTab != NULL)
	{
		int nNewTab = pTabs->GetTabAt(pTab->m_Position);
		if (nNewTab == -1)
		{
			// Tab does not exist in new scheme.
			// Remove it.
			CTextTab Tab = *pTab;
			Tab.m_nType = CTextTab::TAB_remove;
			AddTabChange(pParagraph, &Tab, lParagraph);
		}
	}
}

/*
// Add a tab style change to the paragraph.
*/

void CTextRecord::AddTabChange(CTextParagraph* pParagraph, const CTextTab* pTab, PARAGRAPH_INDEX lParagraph)
{
	// Create the new style change.
	STYLE_INDEX lNewChange;
	CTextStyleChange* pNewChange = NewStyleChange(&lNewChange);
	pTab->ToStyleChange(pNewChange);

	pNewChange->m_lNext = pParagraph->m_lFirstParagraphStyleChange;
	pParagraph->m_lFirstParagraphStyleChange = lNewChange;

	ModifyParagraph(lParagraph);
}

/////////////////////////////////////////////////////////////////////////////
// Macro (fill-in field) routines.

/*
// Reflow all macros in this frame.
*/

ERRORCODE CTextRecord::ReflowMacros(CWordArray* pMacros /*=NULL*/, CFrameObject* pObject /*=NULL*/)
{
	ERRORCODE error = ERRORCODE_None;

	if (HasMacro() != 0)
	{
		if (FirstFrame() != 0)
		{
			CTextFlow* pTextFlow = NULL;
			TRY
			{
				if (pObject == NULL)
				{
					pTextFlow = new CTextFlow(FirstFrame(),
													  (PMGDatabase*)database,
													  0, -1);
				}
				else
				{
					pTextFlow = new CTextFlow(pObject, 0, -1);
				}
				pTextFlow->FlowingMacros(TRUE, pMacros);

				pTextFlow->FlowText();
				// Assume GetError() is always valid.
				error = pTextFlow->GetError();
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

			delete pTextFlow;
		}
	}
	return error;
}

/*
// Get all macros in use.
*/

void CTextRecord::GetMacrosInUse(CWordArray& cwaMacros)
{
	// Only proceed if we actually have a macro character.
	if (m_pEditingText != NULL)
	{
		m_pEditingText->GetMacrosInUse(cwaMacros);
	}
	else
	{
		// Not being overridden by another text record.
		if (HasMacro())
		{
			// Current macro size.
			int nMacros = cwaMacros.GetSize();

			// Look for all the macro characters.
			CHARACTER_COUNT lMacrosToFind = MacroCount();
			CHARACTER_INDEX lCharacter = 0;
			CHARACTER_COUNT lCharacters = NumberOfCharacters();

			CHARACTER* cp = NULL;
			CHARACTER_COUNT lThisCharacters = 0;

			ASSERT(m_pCharacterArray != NULL);

			while (lCharacter < lCharacters)
			{
				// Grab the next character. Prime the character pointer if necessary.
				if (lThisCharacters == 0)
				{
					cp = (CHARACTER*)m_pCharacterArray->GetAt(lCharacter, (DWORD*)&lThisCharacters);
				}
				CHARACTER c = *cp++;
				lThisCharacters--;
				lCharacter++;

				if (c >= MACRO_CHARACTER && c <= MACRO_CHARACTER_MAX)
				{
					// Look for the macro in the list.
					int nMacro;
					for (nMacro = 0; nMacro < nMacros; nMacro++)
					{
						if (cwaMacros[nMacro] == c)
						{
							break;
						}
					}

					// If we did not find it, add it to the list.
					if (nMacro == nMacros)
					{
						cwaMacros.Add((WORD)c);
						nMacros++;
					}

					// If we found all macros, stop. (Why keep looking?)
					if (--lMacrosToFind == 0)
					{
						// Found them all.
						break;
					}
				}
			}
		}
	}
}

void CTextRecord::SetEditingText(CTextRecord* pText)
{
	if (m_pEditingText != NULL)
	{
		m_pEditingText->release();
	}

	m_pEditingText = pText;
}

/*
// Fix any old tokens.
*/

void CTextRecord::FixOldTokens(void)
{
	// Run through all the paragraphs.
	for (PARAGRAPH_INDEX lParagraph = 0; lParagraph < NumberOfParagraphs(); lParagraph++)
	{
		// Set a default style to accumulate changes in.
		CTextStyle Style((PMGDatabase*)database);
		Style.SetDefault();

		CTextParagraph* pParagraph = GetParagraph(lParagraph);
		FixOldTokens(pParagraph->m_lFirstParagraphStyleChange, Style, TRUE);
		FixOldTokens(pParagraph->m_StyleChange.m_lNext, Style, FALSE);
	}
}

void CTextRecord::FixOldTokens(STYLE_INDEX lChange, CTextStyle& Style, BOOL fParagraph)
{
	// Variables used for Paragraph case (only one of each token can be added).
	STYLE_INDEX lFillColorChange = 0;
	STYLE_INDEX lOutlineColorChange = 0;
	STYLE_INDEX lShadowColorChange = 0;
	STYLE_INDEX lLastChange = 0;

	while (lChange != 0)
	{
		// Get the next style change.
		CTextStyleChange* pChange = GetStyleChange(lChange);
		// Remember the next change index.
		STYLE_INDEX lNextChange = pChange->m_lNext;
		// Now see what kind of style change it is.
		switch (pChange->m_wToken)
		{
			case TOKEN_Pattern:
			{
				// Replace this pattern token with:
				//  FillPattern
				//  FillForegroundColor

				// Grab the old values first.
				int nOldFill = Style.FillPattern();
				COLOR lOldFillColor = Style.FillForegroundColor();

				// Pick up the style change so we have new values.
				Style.ApplyChange(pChange);

				// Replace this style change with the fill pattern.
				pChange->m_wToken = TOKEN_FillPattern;
				pChange->m_lOldValue = (long)nOldFill;
				pChange->m_lValue = (long)Style.FillPattern();
				ModifyStyleChange(lChange);

				if (lFillColorChange == 0)
				{
					// Insert a new token for the fill foreground color.
					STYLE_INDEX lNewChange;
					CTextStyleChange* pNewChange = AppendNewChange(lChange, &lNewChange);
					pNewChange->m_wToken = TOKEN_FillForegroundColor;
					pNewChange->m_lOldValue = (long)lOldFillColor;
					pNewChange->m_lValue = (long)Style.FillForegroundColor();
					if (fParagraph)
					{
						// Remember this change.
						lFillColorChange = lNewChange;
					}

					// Update this so the "last" change will be set correctly.
					lChange = lNewChange;
				}
				else
				{
					// Already added a fill color change. Modify it.
					CTextStyleChange* pNewChange = GetStyleChange(lFillColorChange);
					pNewChange->m_lValue = (long)Style.FillForegroundColor();
				}

				break;
			}
			case TOKEN_Outline:
			{
				// Replace this outline token with:
				//  OutlineStyle,
				//  OutlineWidthType,
				//  OutlineWidth,
				//  OutlinePattern,
				//  OutlineForegroundColor,

				// Grab the old values first.
				SHORT nOldStyle = Style.OutlineStyle();
				SHORT nOldWidthType = Style.OutlineWidthType();
				CFixed lOldWidth = Style.OutlineWidth();
				SHORT nOldPattern = Style.OutlinePattern();
				COLOR lOldColor = Style.OutlineForegroundColor();

				// Pick up the style change so we have new values.
				Style.ApplyChange(pChange);

				// Replace this style change with the outline style.
				pChange->m_wToken = TOKEN_OutlineStyle;
				pChange->m_lOldValue = (long)nOldStyle;
				pChange->m_lValue = (long)Style.OutlineStyle();
				ModifyStyleChange(lChange);

				// Append a new token for the outline width type.
				STYLE_INDEX lNewChange;
				CTextStyleChange* pNewChange = AppendNewChange(lChange, &lNewChange);
				pNewChange->m_wToken = TOKEN_OutlineWidthType;
				pNewChange->m_lOldValue = (long)nOldWidthType;
				pNewChange->m_lValue = (long)Style.OutlineWidthType();

				// Append a new token for the outline width.
				pNewChange = AppendNewChange(lNewChange, &lNewChange);
				pNewChange->m_wToken = TOKEN_OutlineWidth;
				pNewChange->m_lOldValue = (long)lOldWidth;
				pNewChange->m_lValue = (long)Style.OutlineWidth();

				// Append a new token for the outline pattern.
				pNewChange = AppendNewChange(lNewChange, &lNewChange);
				pNewChange->m_wToken = TOKEN_OutlinePattern;
				pNewChange->m_lOldValue = (long)nOldPattern;
				pNewChange->m_lValue = (long)Style.OutlinePattern();

				if (lOutlineColorChange == 0)
				{
					// Append a new token for the outline color.
					pNewChange = AppendNewChange(lNewChange, &lNewChange);
					pNewChange->m_wToken = TOKEN_OutlineForegroundColor;
					pNewChange->m_lOldValue = (long)lOldColor;
					pNewChange->m_lValue = (long)Style.OutlineForegroundColor();
					if (fParagraph)
					{
						// Remember this change.
						lOutlineColorChange = lNewChange;
					}
				}
				else
				{
					// Already added an outline color change. Modify it.
					CTextStyleChange* pNewChange = GetStyleChange(lOutlineColorChange);
					pNewChange->m_lValue = (long)Style.OutlineForegroundColor();
				}

				// Update this so the "last" change will be set correctly.
				lChange = lNewChange;
				break;
			}
			case TOKEN_Shadow:
			{
				// Replace this shadow token with:
				//  ShadowStyle,
				//  ShadowOffsetType,
				//  ShadowXOffset,
				//  ShadowYOffset,
				//  ShadowPattern,
				//  ShadowForegroundColor,

				// Grab the old values first.
				SHORT nOldStyle = Style.ShadowStyle();
				SHORT nOldOffsetType = Style.ShadowOffsetType();
				CFixed lOldXOffset = Style.ShadowXOffset();
				CFixed lOldYOffset = Style.ShadowYOffset();
				SHORT nOldPattern = Style.ShadowPattern();
				COLOR lOldColor = Style.ShadowForegroundColor();

				// Pick up the style change so we have new values.
				Style.ApplyChange(pChange);

				// Replace this style change with the shadow style.
				pChange->m_wToken = TOKEN_ShadowStyle;
				pChange->m_lOldValue = (long)nOldStyle;
				pChange->m_lValue = (long)Style.ShadowStyle();
				ModifyStyleChange(lChange);

				// Append a new token for the shadow offset type.
				STYLE_INDEX lNewChange;
				CTextStyleChange* pNewChange = AppendNewChange(lChange, &lNewChange);
				pNewChange->m_wToken = TOKEN_ShadowOffsetType;
				pNewChange->m_lOldValue = (long)nOldOffsetType;
				pNewChange->m_lValue = (long)Style.ShadowOffsetType();

				// Append a new token for the shadow x offset.
				pNewChange = AppendNewChange(lNewChange, &lNewChange);
				pNewChange->m_wToken = TOKEN_ShadowXOffset;
				pNewChange->m_lOldValue = (long)lOldXOffset;
				pNewChange->m_lValue = (long)Style.ShadowXOffset();

				// Append a new token for the shadow y offset.
				pNewChange = AppendNewChange(lNewChange, &lNewChange);
				pNewChange->m_wToken = TOKEN_ShadowYOffset;
				pNewChange->m_lOldValue = (long)lOldYOffset;
				pNewChange->m_lValue = (long)Style.ShadowYOffset();

				// Append a new token for the shadow pattern.
				pNewChange = AppendNewChange(lNewChange, &lNewChange);
				pNewChange->m_wToken = TOKEN_ShadowPattern;
				pNewChange->m_lOldValue = (long)nOldPattern;
				pNewChange->m_lValue = (long)Style.ShadowPattern();

				if (lShadowColorChange == 0)
				{
					// Append a new token for the shadow foreground color.
					pNewChange = AppendNewChange(lNewChange, &lNewChange);
					pNewChange->m_wToken = TOKEN_ShadowForegroundColor;
					pNewChange->m_lOldValue = (long)lOldColor;
					pNewChange->m_lValue = (long)Style.ShadowForegroundColor();
					if (fParagraph)
					{
						// Remember this change.
						lShadowColorChange = lNewChange;
					}
				}
				else
				{
					// Already added a shadow color change. Modify it.
					CTextStyleChange* pNewChange = GetStyleChange(lShadowColorChange);
					pNewChange->m_lValue = (long)Style.ShadowForegroundColor();
				}

				// Update this so the "last" change will be set correctly.
				lChange = lNewChange;

				break;
			}
			case TOKEN_Color:
			{
				// Replace this color token with:
				//  FillForegroundColor
				//  OutlineForegroundColor
				//  ShadowForegroundColor

				BOOL fUsedOriginal = FALSE;		// Have we used the original change yet?
				// Grab the old values first.
				COLOR lOldFillColor = Style.FillForegroundColor();
				COLOR lOldOutlineColor = Style.OutlineForegroundColor();
				COLOR lOldShadowColor = Style.ShadowForegroundColor();

				// Pick up the style change so we have new values.
				Style.ApplyChange(pChange);

				STYLE_INDEX lNewChange = 0;
				CTextStyleChange* pNewChange;

				if (lFillColorChange == 0)
				{
					ASSERT(!fUsedOriginal);
					// Replace this style change with the fill foreground color.
					pChange->m_wToken = TOKEN_FillForegroundColor;
					pChange->m_lOldValue = (long)lOldFillColor;
					pChange->m_lValue = (long)Style.FillForegroundColor();
					ModifyStyleChange(lChange);
					fUsedOriginal = TRUE;
					lNewChange = lChange;			// For subsequent adds.
					if (fParagraph)
					{
						// Remember this change.
						lFillColorChange = lChange;
					}
				}
				else
				{
					// Already added a fill color change. Modify it.
					CTextStyleChange* pNewChange = GetStyleChange(lFillColorChange);
					pNewChange->m_lValue = (long)Style.FillForegroundColor();
				}

				ASSERT(!fUsedOriginal || lNewChange != 0);

            // Handle the outline color.
				if (lOutlineColorChange == 0)
				{
					if (fUsedOriginal)
					{
						// Append a new token for the outline foreground color.
						ASSERT(lNewChange != 0);
						pNewChange = AppendNewChange(lNewChange, &lNewChange);
						pNewChange->m_wToken = TOKEN_OutlineForegroundColor;
						pNewChange->m_lOldValue = (long)lOldOutlineColor;
						pNewChange->m_lValue = (long)Style.OutlineForegroundColor();
						if (fParagraph)
						{
							// Remember this change.
							lOutlineColorChange = lNewChange;
						}
					}
					else
					{
						// Replace this style change with the outline foreground color.
						pChange->m_wToken = TOKEN_OutlineForegroundColor;
						pChange->m_lOldValue = (long)lOldOutlineColor;
						pChange->m_lValue = (long)Style.OutlineForegroundColor();
						ModifyStyleChange(lChange);
						fUsedOriginal = TRUE;
						lNewChange = lChange;			// For subsequent adds.
						if (fParagraph)
						{
							// Remember this change.
							lOutlineColorChange = lChange;
						}
					}
				}
				else
				{
					// Already added an outline color change. Modify it.
					CTextStyleChange* pNewChange = GetStyleChange(lOutlineColorChange);
					pNewChange->m_lValue = (long)Style.OutlineForegroundColor();
				}

				ASSERT(!fUsedOriginal || lNewChange != 0);

				// Append a new token for the shadow foreground color.
				if (lShadowColorChange == 0)
				{
					if (fUsedOriginal)
					{
						ASSERT(lNewChange != 0);
						pNewChange = AppendNewChange(lNewChange, &lNewChange);
						pNewChange->m_wToken = TOKEN_ShadowForegroundColor;
						pNewChange->m_lOldValue = (long)lOldShadowColor;
						pNewChange->m_lValue = (long)Style.ShadowForegroundColor();
						if (fParagraph)
						{
							// Remember this change.
							lShadowColorChange = lNewChange;
						}
					}
					else
					{
						// Replace this style change with the shadow foreground color.
						pChange->m_wToken = TOKEN_ShadowForegroundColor;
						pChange->m_lOldValue = (long)lOldShadowColor;
						pChange->m_lValue = (long)Style.ShadowForegroundColor();
						ModifyStyleChange(lChange);
						fUsedOriginal = TRUE;
						lNewChange = lChange;			// For subsequent adds.
						if (fParagraph)
						{
							// Remember this change.
							lShadowColorChange = lChange;
						}
					}
				}
				else
				{
					// Already added a shadow color change. Modify it.
					CTextStyleChange* pNewChange = GetStyleChange(lShadowColorChange);
					pNewChange->m_lValue = (long)Style.ShadowForegroundColor();
				}

				// If we did not use the original style change, get rid of it now.
				if (!fUsedOriginal)
				{
					// The only way we could get here is if all the tokens
					// represented by this change were already added. So, there
					// must be a last change!
					ASSERT(fParagraph);
					ASSERT(lLastChange != 0);
					if (lLastChange != 0)
					{
						// Unhook this style change.
						CTextStyleChange* pLastChange = ModifyStyleChange(lLastChange);
						pLastChange->m_lNext = lNextChange;
						// And delete this style change.
						DeleteStyleChange(lChange);
					}
				}

				break;
			}
			case TOKEN_FillForegroundColor:
			{
				if (fParagraph)
				{
					lFillColorChange = lChange;
				}
				break;
			}
			case TOKEN_OutlineForegroundColor:
			{
				if (fParagraph)
				{
					lOutlineColorChange = lChange;
				}
				break;
			}
			case TOKEN_ShadowForegroundColor:
			{
				if (fParagraph)
				{
					lShadowColorChange = lChange;
				}
				break;
			}
			default:
			{
				break;
			}
		}
		lLastChange = lChange;
		lChange = lNextChange;
	}
}

/*
// Add a new token after this one.
// It is assumed these style changes apply to the same place.
// So the characters for this style change are applied to the new one.
*/

CTextStyleChange* CTextRecord::AppendNewChange(STYLE_INDEX lChange, STYLE_INDEX* plNewChange)
{
	CTextStyleChange* pNewChange = NewStyleChange(plNewChange);
	CTextStyleChange* pChange = GetStyleChange(lChange);
	
	pNewChange->m_lNext = pChange->m_lNext;
	pChange->m_lNext = *plNewChange;
	// Character count moves to appended change.
	pNewChange->m_lCharacters = pChange->m_lCharacters;
	pChange->m_lCharacters = 0;

	return pNewChange;
}

//////////////////////////////////////////////////////////////////////////////
// Reflow()
// reflows all text in the record
void CTextRecord::Reflow()
{
	// should do more then asserts?
	ASSERT( database );
	ASSERT( FirstFrame() > 0 );
	CTextFlow TextFlow( FirstFrame(), (PMGDatabase *)database, 0, -1);
	TextFlow.FlowText();
}

/////////////////////////////////////////////////////////////////////////////
// Dump()

void CTextRecord::Dump(void)
{
	for (PARAGRAPH_INDEX lParagraph = 0; lParagraph < NumberOfParagraphs(); lParagraph++)
	{
		CTextParagraph* pParagraph = GetParagraph(lParagraph);
		TRACE("Paragraph %ld: %ld characters\n", lParagraph, pParagraph->m_lCharacters);
		STYLE_INDEX lChange;
		// Dump paragraph changes.
		for (lChange = pParagraph->m_lFirstParagraphStyleChange; lChange != 0; )
		{
			CTextStyleChange* pChange = GetStyleChange(lParagraph, lChange);
			TRACE("\tPChange %ld - token: %d, chrs: %ld, value: %lx, oldvalue: %lx\n",
							lChange,
							pChange->m_wToken,
							pChange->m_lCharacters,
							pChange->m_lValue,
							pChange->m_lOldValue);
			lChange = pChange->m_lNext;
		}
		// Dump in-line changes.
		for (lChange = -1; lChange != 0; )
		{
			CTextStyleChange* pChange = GetStyleChange(lParagraph, lChange);
			TRACE("\tIChange %ld - token: %d, chrs: %ld, value: %lx, oldvalue: %lx\n",
							lChange,
							pChange->m_wToken,
							pChange->m_lCharacters,
							pChange->m_lValue,
							pChange->m_lOldValue);
			lChange = pChange->m_lNext;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCharacterArrayRecord

CCharacterArrayRecord::CCharacterArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
	: CArrayRecord(number, type, owner, where)
{
	ElementSize(sizeof(CHARACTER));			// 2 bytes
	MaxBlockElements(2048);						// 2*2048 = 4096
	ElementIncrement(64);						// 2*64 = 128 bytes
}

/*
// The creator for a character array record.
*/

ERRORCODE CCharacterArrayRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */

	*record = NULL;
	TRY
	{
		*record = new CCharacterArrayRecord(number, type, owner, where);
	}
	END_TRY

	return (*record == NULL) ? ERRORCODE_Memory : ERRORCODE_None;
}

/////////////////////////////////////////////////////////////////////////////
// CStyleArrayRecord
// Style changes are not inserted and deleted in the usual sense.
// So we do not need to worry about memmove sizes during insertions and
// deletions. This allows us to make the block size a little bigger.

CStyleArrayRecord::CStyleArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
	: CArrayRecord(number, type, owner, where)
{
	ElementSize(sizeof(CTextStyleChange));	// ~20 bytes
	MaxBlockElements(600);						// ~20*600 = 12000 bytes
	ElementIncrement(8);							// ~20*8 = 160

	memset(&m_Record2, 0, sizeof(m_Record2));
}

/*
// The creator for a style array record.
*/

ERRORCODE CStyleArrayRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */

	*record = NULL;
	TRY
	{
		*record = new CStyleArrayRecord(number, type, owner, where);
	}
	END_TRY

	return (*record == NULL) ? ERRORCODE_Memory : ERRORCODE_None;
}

//
// Allocate a new text state.
//

CTextStyleChange* CStyleArrayRecord::NewStyleChange(STYLE_INDEX* plChange)
{
	// Search the array starting at the last place.
	for (;;)
	{
		// Are we at the end of the array?
		if (m_Record2.m_lNextStyleChange == (STYLE_INDEX)ElementCount())
		{
			// Expand the array.
			CTextStyleChange Change;
			memset(&Change, 0, sizeof(Change));
			Change.m_wToken = (WORD)TOKEN_Free;
			Add((LPVOID)&Change);
		}

		// Get the text state.
		CTextStyleChange* pChange = (CTextStyleChange*)GetAt(m_Record2.m_lNextStyleChange++);

		// See if it is in use.
		if (pChange->m_wToken == (WORD)TOKEN_Free)
		{
			// We have found a free style change.
			// Set its type to something not "Free"
			*plChange = m_Record2.m_lNextStyleChange;		// Already incremented.
			pChange->m_wToken = (WORD)TOKEN_Illegal;		// But not free...
			return pChange;
		}
	}
	// We either allocate or throw an exception.
}

//
// Get a style change (given the id of the change).
//

CTextStyleChange* CStyleArrayRecord::GetStyleChange(STYLE_INDEX lChange)
{
	lChange--;
	// Validate the element number.
	if ((lChange < 0) || (lChange >= (STYLE_INDEX)ElementCount()))
	{
		ASSERT(FALSE);
		ThrowErrorcodeException(ERRORCODE_NotAllocated);
	}

	// Get the element.
	CTextStyleChange* pChange = (CTextStyleChange*)GetAt(lChange);

	// Validate element is allocated.
	if (pChange->m_wToken == TOKEN_Free)
	{
		ASSERT(FALSE);
		ThrowErrorcodeException(ERRORCODE_NotAllocated);
	}

	// Everything seems to be all right.
	return pChange;
}

//
// Get a style change (given the id of the change).
//

CTextStyleChange* CStyleArrayRecord::ModifyStyleChange(STYLE_INDEX lChange)
{
	CTextStyleChange* pChange = GetStyleChange(lChange);
	// If we got here with no exception, assume lChange is valid.
	ModifyAt(lChange-1);
	return pChange;
}

//
// Delete a style change (given the id of the change).
//

void CStyleArrayRecord::DeleteStyleChange(STYLE_INDEX lChange)
{
	lChange--;
	if (lChange >= 0 && lChange < (STYLE_INDEX)ElementCount())
	{
		// Mark this change as free.
		CTextStyleChange* pChange = (CTextStyleChange*)GetAt(lChange);

		if (pChange->m_wToken != TOKEN_Free)
		{
			pChange->m_wToken = TOKEN_Free;

			// Update the min style change if we are under.
			if (lChange < m_Record2.m_lNextStyleChange)
			{
				m_Record2.m_lNextStyleChange = lChange;
			}
			modified();
			return;
		}
	}
	ASSERT(FALSE);
	ThrowErrorcodeException(ERRORCODE_NotAllocated);
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE CStyleArrayRecord::ReadData(StorageDevice* pDevice)
{
	ERRORCODE error;

	if ((error = CArrayRecord::ReadData(pDevice)) == ERRORCODE_None)
	{
		error = pDevice->read_record(&m_Record2, sizeof(m_Record2));
	}
	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE CStyleArrayRecord::WriteData(StorageDevice* pDevice)
{
	ERRORCODE error;

	if ((error = CArrayRecord::WriteData(pDevice)) == ERRORCODE_None)
	{
		error = pDevice->write_record(&m_Record2, sizeof(m_Record2));
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE CStyleArrayRecord::SizeofData(StorageDevice* pDevice)
{
	return CArrayRecord::SizeofData(pDevice)
					+ pDevice->size_record(sizeof(m_Record2));
}

/////////////////////////////////////////////////////////////////////////////
// CParagraphArrayRecord

CParagraphArrayRecord::CParagraphArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
	: CArrayRecord(number, type, owner, where)
{
	ElementSize(sizeof(CTextParagraph));	// ~12 bytes
	MaxBlockElements(300);						// ~12*300 = 3600 bytes
	ElementIncrement(8);							// ~12*8 = 96
}

/*
// The creator for a paragraph array record.
*/

ERRORCODE CParagraphArrayRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */

	*record = NULL;
	TRY
	{
		*record = new CParagraphArrayRecord(number, type, owner, where);
	}
	END_TRY

	return (*record == NULL) ? ERRORCODE_Memory : ERRORCODE_None;
}
