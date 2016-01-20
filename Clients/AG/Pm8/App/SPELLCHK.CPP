/*
// $Workfile: SPELLCHK.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SPELLCHK.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 3     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     3/06/98 3:58p Jayn
// Fixed a spell-check text corruption bug.
// 
//    Rev 1.0   14 Aug 1997 15:25:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:30   Fred
// Initial revision.
// 
//    Rev 1.10   18 Jul 1997 11:00:52   Jay
// Handles corrupted user lexicon.
// 
//    Rev 1.9   07 May 1997 12:10:52   Jay
// User directory and collections
// 
//    Rev 1.8   04 Sep 1996 18:11:28   Jay
// Spell-check fixes.
// 
//    Rev 1.7   06 Aug 1996 11:17:42   Jay
//  
// 
//    Rev 1.6   02 Aug 1996 17:26:16   Jay
// Auto-replace got stuck (vs->vs.)
// 
//    Rev 1.5   02 Aug 1996 17:13:46   Jay
// Differentiates correcting a misspelling from just finding one.
// 
//    Rev 1.4   02 Aug 1996 15:25:32   Jay
// Bug fixes
// 
//    Rev 1.3   01 Aug 1996 18:20:24   Jay
// Spell check highlight.
// 
//    Rev 1.2   01 Aug 1996 14:40:44   Jay
// Spelling fixes
// 
//    Rev 1.1   01 Aug 1996 09:29:52   Jay
// Fixed up some message boxes.
// 
//    Rev 1.0   31 Jul 1996 18:22:26   Jay
// Initial revision.

*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "spellchk.h"
#include "docrec.h"
#include "textrec.h"
#include "spelldlg.h"
#include "textflow.h"
#include <string.h>
#include <ctype.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char far SECTION_Spelling[] = "Spelling";
char far ENTRY_IgnoreNumbers[] = "IgnoreNumbers";
char far ENTRY_IgnoreUpperCase[] = "IgnoreUpperCase";

// Constructor.
CSpellChecker::CSpellChecker(CPmwView* pView)
{
	ASSERT(m_pView != NULL);
	m_pView = pView;
	m_pDoc = (pView == NULL) ? NULL : pView->GetDocument();
	m_nSession = -1;

	m_nMainLexicon = -1;
	m_nMainLexiconLang = 0;
	m_nUserLexicon = -1;
	m_nIgnoreAllLexicon = -1;
	m_nChangeAllLexicon = -1;

	m_fIgnoreNumbers = FALSE;
	m_fIgnoreUpperCase = FALSE;

	m_pViewEditState = NULL;
	m_fFoundMisspelling = FALSE;
	m_fCorrectedMisspelling = FALSE;
}

// Destructor.
CSpellChecker::~CSpellChecker()
{
	CloseSession();
}

// Open a session.
BOOL CSpellChecker::OpenSession(void)
{
	// Read the ini file settings.
	ReadIniFileSettings();

	ASSERT(m_nSession < 0);
	BOOL fResult = FALSE;
	UINT uMessage = IDS_SPELLING_NOT_AVAILABLE;

	// Try to startup the engine.
	if (m_Engine.Startup())
	{
		// Open a spelling session.
		m_nSession = m_Engine.OpenSession();
		if (m_nSession >= 0)
		{
			//
			// PrintMaster uses three lexicons during a spell check:
			//
			// (1) The main compressed lexicon
			//     This is the lexicon shipped with the WinterTree package.
			// (2) The user lexicon
			//     This lexicon holds user-added words (user clicks "Add").
			// (3) The "ignore" lexicon
			//     This is a temporary lexicon which holds the ignored words
			//     for this spell-check session (user clicks "Ignore All").
			//

			if (OpenUserLexicon()
				 && OpenIgnoreAllLexicon()
				 && OpenChangeAllLexicon()
				 && OpenMainLexicon())
			{
				fResult = TRUE;
			}
		}
		else
		{
//			AfxMessageBox("Unable to Open Session.");
		}
	}

	// If we failed, clean up.
	if (!fResult)
	{
		CloseSession();
		AfxMessageBox(uMessage);
	}
	return fResult;
}

// Close the current session (if open).
void CSpellChecker::CloseSession(void)
{
	if (m_Engine.IsValid())
	{
		if (m_nSession >= 0)
		{
			// Close the lexicons.
			CloseLexicon(m_nMainLexicon);
			CloseLexicon(m_nUserLexicon);
			CloseLexicon(m_nIgnoreAllLexicon);
			CloseLexicon(m_nChangeAllLexicon);

			// Close the session.
			m_Engine.CloseSession(m_nSession);
			m_nSession = -1;
		}
		m_Engine.Shutdown();
	}

	// Out with the edit state.
	delete m_pViewEditState;
	m_pViewEditState = NULL;

	// Write the ini file settings.
	WriteIniFileSettings();

	// We should be totally shut down at this point.
	ASSERT(m_nSession < 0);
	ASSERT(m_nMainLexicon < 0);
	ASSERT(m_nUserLexicon < 0);
	ASSERT(m_nIgnoreAllLexicon < 0);
	ASSERT(m_nChangeAllLexicon < 0);
	ASSERT(!m_Engine.IsValid());
}

void CSpellChecker::ReadIniFileSettings(void)
{
	m_fIgnoreNumbers = GET_PMWAPP()->OurGetProfileInt(SECTION_Spelling, ENTRY_IgnoreNumbers, m_fIgnoreNumbers);
	m_fIgnoreUpperCase = GET_PMWAPP()->OurGetProfileInt(SECTION_Spelling, ENTRY_IgnoreUpperCase, m_fIgnoreUpperCase);
}

void CSpellChecker::WriteIniFileSettings(void)
{
	AfxGetApp()->WriteProfileInt(SECTION_Spelling, ENTRY_IgnoreNumbers, m_fIgnoreNumbers);
	AfxGetApp()->WriteProfileInt(SECTION_Spelling, ENTRY_IgnoreUpperCase, m_fIgnoreUpperCase);
}

/*
// Close a lexicon.
*/

void CSpellChecker::CloseLexicon(S16& nLexicon)
{
	if (nLexicon >= 0)
	{
		m_Engine.CloseLex(m_nSession, nLexicon);
		nLexicon = -1;
	}
}

/*
// Open the main lexicon.
// This is the massive, read-only, shared lexicon.
*/

BOOL CSpellChecker::OpenMainLexicon(void)
{
	// Build the file name.
	CString csName = GetGlobalPathManager()->ExpandPath("PMW.CLX");

	// Open the lexicon. Give it the all the memBudget (0) it wants.
	if ((m_nMainLexicon = m_Engine.OpenLex(m_nSession, csName, 0)) >= 0)
	{
		if (m_Engine.GetLexInfo(m_nSession, m_nMainLexicon, NULL, NULL, NULL, &m_nMainLexiconLang) < 0)
		{
			// Default this failing sucker to something.
			// This is not enough of an error to get worked up about.
			m_nMainLexiconLang = DEFAULT_LEXICON_LANG;
		}
		return TRUE;
	}

//	AfxMessageBox("Unable to open Main Lexicon.");
	// Main lexicon could not be opened. m_nMainLexicon holds the error value.
	ASSERT(FALSE);
	return FALSE;
}

S16 CSpellChecker::GetMainLexiconLang(void)
{
	if (m_nMainLexiconLang == 0)
	{
		if (OpenMainLexicon())
		{
			CloseLexicon(m_nMainLexicon);
		}
	}
	return (m_nMainLexiconLang == 0) ? DEFAULT_LEXICON_LANG : m_nMainLexiconLang;
}

/*
// Open the user lexicon.
// This is user's NID'd personal writeable lexicon.
*/

BOOL CSpellChecker::OpenUserLexicon(void)
{
	// Build the file name.
	CString csName = GetGlobalPathManager()->ExpandPath("[[U]]\\USER.TLX");

	if ((m_nUserLexicon = m_Engine.OpenLex(m_nSession, csName, 0)) < 0)
	{
		// Could not open. If the lexicon does not exist, try to create it.
		if (m_nUserLexicon == SSCE_UNKNOWN_LEXICON_TYPE_ERR)   // 0-byte file, etc.
		{
			// Delete it!
			if (AfxMessageBox(IDS_UserLexiconCorrupted, MB_YESNO|MB_ICONEXCLAMATION) == IDYES)
			{
				// We need to delete the file, or create will fail.
				TRY
				{
					CFile::Remove(csName);
				}
				END_TRY

				// And pretend it does not exist.
				m_nUserLexicon = SSCE_FILE_NOT_FOUND_ERR;
			}
		}
		if (m_nUserLexicon == SSCE_FILE_NOT_FOUND_ERR)
		{
			// Lexicon does not exist. Try to create it.
			m_nUserLexicon = m_Engine.CreateLex(m_nSession, csName, SSCE_IGNORE_LEX_TYPE, GetMainLexiconLang());
		}
	}

#if 0
	if (m_nUserLexicon < 0)
	{
		CString cs;
		cs.Format("Unable to Open User Lexicon '%s'.", csName);
		AfxMessageBox(cs);
	}
#endif
	ASSERT(m_nUserLexicon >= 0);
	return m_nUserLexicon >= 0;
}

/*
// Open the "ignore all" lexicon.
// This is a temporary lexicon holding words being ignored this session.
*/

BOOL CSpellChecker::OpenIgnoreAllLexicon(void)
{
	// Try to create the temporary ignore lexicon.
	if ((m_nIgnoreAllLexicon = m_Engine.CreateLex(m_nSession, NULL, SSCE_IGNORE_LEX_TYPE, GetMainLexiconLang())) >= 0)
	{
		return TRUE;
	}

//	AfxMessageBox("Unable to Create Ignore All Lexicon.");
	// Ignore lexicon could not be created. m_nIgnoreAllLexicon is the error value.
	ASSERT(FALSE);
	return FALSE;
}

/*
// Open the "change all" lexicon.
// This is a temporary lexicon holding words being automatically changed this session.
*/

BOOL CSpellChecker::OpenChangeAllLexicon(void)
{
	// Try to create the temporary ignore lexicon.
	if ((m_nChangeAllLexicon = m_Engine.CreateLex(m_nSession, NULL, SSCE_CHANGE_LEX_TYPE, GetMainLexiconLang())) >= 0)
	{
		return TRUE;
	}

//	AfxMessageBox("Unable to Create Change All Lexicon.");
	// Ignore lexicon could not be created. m_nChangeAllLexicon is the error value.
	ASSERT(FALSE);
	return FALSE;
}

/*
// Generic entry. Do the entire spelling cycle.
*/

int CSpellChecker::CheckSpelling(void)
{
	ASSERT(m_pView != NULL);
	ASSERT(m_pDoc != NULL);
	DB_RECORD_NUMBER lExcludeStory = 0;
	int nResult = SPELLCHECK_Continue;
	DocumentRecord* pDocument = m_pDoc->DocumentRecord();
	DWORD dwStories = pDocument->NumberOfStories();

	m_pViewEditState = m_pView->CreateEditState();
	m_pView->SaveEditState(m_pViewEditState);
	m_fFoundMisspelling = FALSE;
	m_fCorrectedMisspelling = FALSE;

	// If we have a current story, check it first.
	if (m_pView->EditingText())
	{
		CTxp* pTxp = m_pView->GetTxp();
		lExcludeStory = pTxp->TextRecord();
		CHARACTER_INDEX lStart = pTxp->Index();
		if (pTxp->SomethingSelected() && pTxp->SelectStart() < lStart)
		{
			lStart = pTxp->SelectStart();
		}

		if ((nResult = CheckStorySpelling(lExcludeStory, lStart, -1, &lStart)) == SPELLCHECK_Continue)
		{
			if (lStart > 0)
			{
				nResult = CheckStorySpelling(lExcludeStory, 0, lStart);
			}

			if (nResult == SPELLCHECK_Continue)
			{
				// See if the user wants to check the rest of the stories.
				if (dwStories > 1)
				{
					if (AfxMessageBox(IDS_CONTINUE_SPELLCHECK_STORIES, MB_YESNO|MB_ICONQUESTION) != IDYES)
					{
						nResult = SPELLCHECK_Cancelled;
					}
				}
				else
				{
					ASSERT(dwStories == 1);
					ASSERT(pDocument->GetStory(0) == lExcludeStory);
				}
			}
		}
	}

	// Now check the rest of the stories if so desired.

	for (DWORD dwStoryIndex = 0; nResult == SPELLCHECK_Continue && dwStoryIndex < dwStories; dwStoryIndex++)
	{
		DB_RECORD_NUMBER lStory = pDocument->GetStory(dwStoryIndex);
		if (lStory != lExcludeStory)
		{
			nResult = CheckStorySpelling(lStory);
		}
	}

	if (nResult == SPELLCHECK_Continue)
	{
		AfxMessageBox(IDS_SPELL_CHECK_COMPLETE, MB_OK|MB_ICONINFORMATION);
	}
	else if (nResult == SPELLCHECK_Error)
	{
		AfxMessageBox(IDS_SPELL_CHECK_ERROR);
	}

	if (m_fFoundMisspelling)
	{
		m_pView->RestoreEditState(m_pViewEditState);
		if (m_fCorrectedMisspelling)
		{
			m_pDoc->resize_project();
			m_pDoc->SetModifiedFlag();
			m_pDoc->ClearAllCommands();		// No undo before this point.
		}
	}

	return nResult;
}

/*
// Spell checker story spell check sub-entry.
*/

int CSpellChecker::CheckStorySpelling(DB_RECORD_NUMBER lStory, CHARACTER_INDEX lStart /*=0*/, CHARACTER_INDEX lEnd /*=-1*/, CHARACTER_INDEX* pAlignedStart /*=NULL*/)
{
	int nResult = SPELLCHECK_Continue;

	// Lock the text record. An error generates a "Continue" result.
	CTextRecord* pText = (CTextRecord*)m_pDoc->get_database()->get_record(lStory, NULL, RECORD_TYPE_Text);
	if (pText != NULL)
	{
		// Create the word source based on this text record.
		CTextRecordWords Words(m_pView, pText, lStart, lEnd);
		// Check the spelling.
		nResult = CheckWordsSpelling(&Words);
		// Bye-bye to you, text.
		pText->release();

		// Pass back the aligned start if requested.
		if (pAlignedStart != NULL)
		{
			*pAlignedStart = Words.StartCharacter();
		}
	}

	return nResult;
}

static BOOL CheckThisWord(LPCSTR pszWord)
{
	int nLength = strlen(pszWord);
	// Single character (or less) words are useless to check.
	if (nLength <= 1)
	{
		// Do not bother.
		return FALSE;
	}
	// Ignore words with all digits.
	BOOL fHasNonDigit = FALSE;
	for (LPCSTR p = pszWord; *p != '\0'; *p++)
	{
		if (!isdigit(*p))
		{
			fHasNonDigit = TRUE;
			break;
		}
	}
	if (!fHasNonDigit)
	{
		// All digits is a stupid thing to check.
		return FALSE;
	}

	// Put other checks here.

	// Check this word!
	return TRUE;
}

/*
// Spell check (potentially) a bunch of words.
*/

int CSpellChecker::CheckWordsSpelling(CSpellCheckWords* pWords)
{
	int nResult = SPELLCHECK_Continue;
	// We are beginning.
	pWords->StartSpellCheck();

	TRY
	{
		char cbWord[SSCE_MAX_WORD_SZ];
		char cbReplWord[SSCE_MAX_WORD_SZ];

		while (pWords->NextWord(cbWord, sizeof(cbWord)-1) && nResult == SPELLCHECK_Continue)
		{
			// Check the next word.
			U32 lOptions = SSCE_REPORT_SPELLING_OPT
								| SSCE_SPLIT_HYPHENATED_WORDS_OPT
								| SSCE_SPLIT_CONTRACTED_WORDS_OPT
								| SSCE_STRIP_POSSESSIVES_OPT;
			if (m_fIgnoreNumbers)
			{
				lOptions |= SSCE_IGNORE_MIXED_DIGITS_OPT;
			}
			if (m_fIgnoreUpperCase)
			{
				lOptions |= SSCE_IGNORE_ALL_CAPS_WORD_OPT;
			}
			S16 nCheckResult = 0;
			if (CheckThisWord(cbWord))
			{
				nCheckResult = m_Engine.CheckWord(m_nSession,
															  lOptions,
															  (const SSCE_CHAR*)cbWord,
															  (SSCE_CHAR*)cbReplWord,
															  sizeof(cbReplWord));
			}

			TRACE("Word '%s' returned %x\n", cbWord, nCheckResult);

			if (nCheckResult < 0)
			{
				ASSERT(FALSE);
				nResult = SPELLCHECK_Error;
			}
			else
			{
				BOOL fReplace = FALSE;
				BOOL fAutoReplace = FALSE;
				switch (nCheckResult)
				{
					case 0:
					{
						// Word is spelled correctly!
						break;
					}
					case SSCE_MISSPELLED_WORD_RSLT:
					{
						m_fFoundMisspelling = TRUE;
						pWords->ShowCurrentWord();

						nResult = HandleMisspelledWord(cbWord, cbReplWord, sizeof(cbReplWord));
						fReplace = (nResult == SPELLCHECK_Continue && cbReplWord[0] != '\0');
						break;
					}
					case SSCE_CHANGE_WORD_RSLT:
					{
						// Auto-replace.
						fReplace = TRUE;
						fAutoReplace = TRUE;
						break;
					}
					default:
					{
						// Unexpected value of nCheckResult.
						ASSERT(FALSE);
						break;
					}
				}

				if (fReplace)
				{
					TRACE("Replace |%s| with |%s|\n", (LPCSTR)cbWord, (LPCSTR)cbReplWord);
					pWords->ReplaceWord(cbReplWord, fAutoReplace);
					m_fCorrectedMisspelling = TRUE;
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		nResult = SPELLCHECK_Error;
	}
	END_CATCH_ALL

	// We are ending.
	pWords->FinishSpellCheck(nResult == SPELLCHECK_Continue);

	return nResult;
}

/*
// A word is misspelled.
// Put up a dialog and let the user change it.
*/

int CSpellChecker::HandleMisspelledWord(LPCSTR pWord, LPSTR pReplWord, int nReplWordSize)
{
	int nResult = SPELLCHECK_Continue;
	*pReplWord = '\0';

	// Get some suggestions from the engine.
	SSCE_CHAR* pSuggestions = NULL;
	int nSuggestionsSize = MAX_SUGGEST_WORDS*SSCE_MAX_WORD_SZ;
	TRY
		pSuggestions = new SSCE_CHAR[nSuggestionsSize];
	END_TRY
	S16 nScores[MAX_SUGGEST_WORDS];

	if (pSuggestions != NULL)
	{
		m_Engine.Suggest(m_nSession,
							  (const SSCE_CHAR*)pWord,
							  SSCE_AUTO_SEARCH_DEPTH,
							  pSuggestions,
							  nSuggestionsSize,
							  nScores,
							  MAX_SUGGEST_WORDS);
	}

	CSpellingDialog Dialog(m_pView);

	Dialog.m_csWord = pWord;
	Dialog.m_csChangeTo = pWord;			// Change to is same
	Dialog.m_fIgnoreNumbers = m_fIgnoreNumbers;
	Dialog.m_fIgnoreUpperCase = m_fIgnoreUpperCase;
	Dialog.m_pSuggestions = (LPCSTR)pSuggestions;
	Dialog.m_nMaxWordSize = SSCE_MAX_WORD_LEN;		// Omit terminator for this.

	switch (Dialog.DoModal())
	{
		case IDC_ADD:
		{
			// The user wants to Add the word to the user lexicon.
			ASSERT(m_nUserLexicon >= 0);
			S16 nAddResult = m_Engine.AddToLex(m_nSession,
														  m_nUserLexicon,
														  (const SSCE_CHAR*)pWord,
														  (const SSCE_CHAR*)pReplWord);		// Not used...
			if (nAddResult < 0)
			{
				// Could not add to lexicon!
				ASSERT(FALSE);
				AfxMessageBox(IDS_COULD_NOT_ADD_WORD);
			}
			break;
		}
		case IDC_CHANGE_ALL:
		{
			strcpy(pReplWord, Dialog.m_csChangeTo);

			ASSERT(*pReplWord != '\0');
			ASSERT(m_nChangeAllLexicon >= 0);
			S16 nAddResult = m_Engine.AddToLex(m_nSession,
														  m_nChangeAllLexicon,
														  (const SSCE_CHAR*)pWord,
														  (const SSCE_CHAR*)pReplWord);
			ASSERT(nAddResult >= 0);
			break;
		}
		case IDC_CHANGE:
		{
			strcpy(pReplWord, Dialog.m_csChangeTo);
			break;
		}
		case IDC_IGNORE_ALL:
		{
			ASSERT(m_nIgnoreAllLexicon >= 0);
			S16 nAddResult = m_Engine.AddToLex(m_nSession,
														  m_nIgnoreAllLexicon,
														  (const SSCE_CHAR*)pWord,
														  (const SSCE_CHAR*)pReplWord);		// Not used...
			ASSERT(nAddResult >= 0);
			break;
		}
		case IDC_IGNORE:
		{
			break;
		}
		default:
		{
			nResult = SPELLCHECK_Cancelled;
			break;
		}
	}

	// Copy back out the settings in case the user wanted them changed.
	m_fIgnoreNumbers = Dialog.m_fIgnoreNumbers;
	m_fIgnoreUpperCase = Dialog.m_fIgnoreUpperCase;

	// Delete the suggestions buffer.
	delete [] pSuggestions;
	return nResult;
}

/////////////////////////////////////////////////////////////////////////////
// CTextRecordWords

CTextRecordWords::CTextRecordWords(CPmwView* pView, CTextRecord* pText, CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd)
{
	m_pView = pView;
	m_pText = pText;
	m_lStart = lStart;
	if ((m_lEnd = lEnd) == -1)
	{
		m_lEnd = m_pText->NumberOfCharacters();
	}

	WordAlign();

	m_lWordEnd = m_lStart;
	// Init the character iterator.
	m_lIndex = m_lStart;
	m_lCharacters = 0;
	m_pCharacter = NULL;

}

void CTextRecordWords::StartSpellCheck(void)
{
	m_lReplaceStart = m_lReplaceEnd = -1;
}

BOOL CTextRecordWords::NextWord(char* pWord, int nWordSizeMax)
{
	for (;;)
	{
		// Get the next word (of any type).
		int nWordType = NextBaseWord(pWord, nWordSizeMax);

		// If we ran out of text, we are all done.
		if (nWordType == WORD_TYPE_eot)
		{
			return FALSE;
		}

		// If we have a solid word, we have something to return.
		if (nWordType == WORD_TYPE_solid)
		{
			return TRUE;
		}

		// Skip this word.
	}
}

void CTextRecordWords::ShowCurrentWord(void)
{
	// Show the misspelled word in the view.
	// Since we are going to be showing things in the view, we need to
	// update the text.
	FlowReplacedText();

	// Find the frame corresponding to the first character of the word.
	DB_RECORD_NUMBER lFrame = m_pText->GetFrameOfCharacter(m_lWordStart);

	// Show that text.
	m_pView->ViewText(lFrame, m_lWordStart, m_lWordEnd);
}

void CTextRecordWords::ReplaceWord(LPCSTR pReplaceText, BOOL fAutoReplace)
{
	// Replace the current word with new text.
	// Initialize the Txp for cool usage.
	CTxp Txp(NULL);
//	Txp.Init(m_pText->FirstFrame(),
	Txp.Init(m_pView->GetTxp()->CursorFrame(),
				(PMGDatabase*)m_pText->GetDatabase(),
				m_lWordStart);
	Txp.WantFlowText(FALSE);

	// Calculate all parameters for being here.
	Txp.CalcAll();

	// Insert the new text.
	Txp.InsertString(pReplaceText);

	// Delete the old text.
	Txp.DeleteText(m_lWordEnd - m_lWordStart);

	// Update the ends since things have changed.
	CHARACTER_INDEX lNewWordEnd = m_lWordStart + strlen(pReplaceText);
	m_lEnd += lNewWordEnd - m_lWordEnd;
	// Advance to end if auto; else, remain and check user's choice.
	m_lWordEnd = fAutoReplace ? lNewWordEnd : m_lWordStart;

	// Flush the character buffering that is going on.
	m_lCharacters = 0;
	m_lIndex = m_lWordEnd;

	// Compute the range for later text flow.
	if (m_lReplaceStart == -1)
	{
		m_lReplaceStart = m_lWordStart;
		m_lReplaceEnd = m_lWordEnd;
	}
	else
	{
		if (m_lWordStart < m_lReplaceStart)
		{
			m_lReplaceStart = m_lWordStart;
		}
		if (m_lWordEnd > m_lReplaceEnd)
		{
			m_lReplaceEnd = m_lWordEnd;
		}
	}
}

void CTextRecordWords::FinishSpellCheck(BOOL fFinished)
{
	FlowReplacedText();
}

void CTextRecordWords::FlowReplacedText(void)
{
	if (m_lReplaceStart != -1)
	{
		CTextFlow TextFlow(m_pText->FirstFrame(), (PMGDatabase*)m_pText->GetDatabase(), m_lReplaceStart, m_lReplaceEnd);
		TextFlow.FlowText();
		m_lReplaceStart = m_lReplaceEnd = -1;
	}
}

static char cbSpace[] = " \t\n\"!@#$%^&*(),.<>;:+={}[]<>\\|/?~`";

/*
// Gather the next word from the source stream.
// This will return solid words, white words, etc.
*/

int CTextRecordWords::NextBaseWord(char* pWord, int nWordSizeMax)
{
	// Start at the end of the last word.
	m_lWordStart = m_lWordEnd;

	// Search for the new word end.
	ASSERT(m_lWordStart == m_lIndex);

	BOOL fFoundSolid = FALSE;

	BOOL fHasWeirdCharacter = FALSE;
	int nWordType = WORD_TYPE_unknown;		// Borrow from text words defs.

	for (;;)
	{
		if (m_lIndex >= m_lEnd)
		{
			if (nWordType == WORD_TYPE_unknown)
			{
				nWordType = WORD_TYPE_eot;
			}
			break;
		}

		// Compute the type of the next character.
		CHARACTER c = GetCharacter();

		BOOL fSolid = FALSE;

		if (c > 255)
		{
			// We consider this solid, but weird.
			fSolid = TRUE;
			fHasWeirdCharacter = TRUE;
		}
		else if (strchr(cbSpace, (char)c) != NULL
					|| (c == '\'' && !fFoundSolid))
		{
			// Space character.
			fSolid = FALSE;
		}
		else
		{
			// Solid character.
			fSolid = TRUE;
			if (c < ' ')
			{
				fHasWeirdCharacter = TRUE;
			}
		}

		if (fSolid != fFoundSolid)
		{
			if (fFoundSolid)
			{
				break;
			}
			m_lWordStart = m_lIndex;
			fFoundSolid = fSolid;
		}

		if (fFoundSolid)
		{
			// Add the character if there is room.
			if (nWordSizeMax > 0)
			{
				*pWord++ = (char)c;
				nWordSizeMax--;
			}
		}

		// Move to the next character.
		m_pCharacter++;
		m_lCharacters--;
		m_lIndex++;
	}

	// Finished the word. Set the end index.
	*pWord = '\0';
	m_lWordEnd = m_lIndex;

	// Fix the type if we hit strange characters.
	if (!fFoundSolid)
	{
		return WORD_TYPE_eot;
	}
	else if (fHasWeirdCharacter)
	{
		return WORD_TYPE_unknown;
	}
	else
	{
		return WORD_TYPE_solid;
	}
}

CHARACTER CTextRecordWords::GetCharacter(void)
{
	if (m_lCharacters == 0)
	{
		m_pCharacter = m_pText->GetCharacters(m_lIndex, &m_lCharacters);
	}
	return *m_pCharacter;
}

static BOOL CharacterIsSolid(CHARACTER c)
{
	return (c > 255) || strchr(cbSpace, (char)c) == NULL;
}

/*
// Make sure our start position is not in the middle of a solid word.
*/

void CTextRecordWords::WordAlign(void)
{
	TRY
	{
		if (m_lStart == m_pText->NumberOfCharacters())
		{
			m_lStart = 0;
		}

		// This kind of fakes it: we back up until we are on a non-solid character.
		while (m_lStart > 0 && CharacterIsSolid(*(m_pText->GetCharacter(m_lStart))))
		{
			m_lStart--;
		}
	}
	END_TRY
}
