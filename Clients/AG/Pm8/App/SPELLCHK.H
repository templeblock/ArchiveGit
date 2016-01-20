/*
// $Workfile: SPELLCHK.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SPELLCHK.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:58   Fred
// Initial revision.
// 
//    Rev 1.3   02 Aug 1996 17:26:16   Jay
// Auto-replace got stuck (vs->vs.)
// 
//    Rev 1.2   02 Aug 1996 17:13:30   Jay
// Differentiates correcting a misspelling from just finding one.
// 
//    Rev 1.1   01 Aug 1996 18:20:24   Jay
// Spell check highlight.
// 
//    Rev 1.0   31 Jul 1996 18:22:26   Jay
// Initial revision.
*/

class CSpellChecker;
class CSpellCheckWords;

#ifndef __SPELLCHK_H__
#define __SPELLCHK_H__

class CPmwView;
class CPmwDoc;
class CTextRecord;
class CViewEdiState;

#include "spelleng.h"

#define DEFAULT_LEXICON_LANG SSCE_AMER_ENGLISH_LANG
#define MAX_SUGGEST_WORDS	16

/////////////////////////////////////////////////////////////////////////////
// The all-powerful spell-checker class.

class CSpellChecker
{
public:
	enum
	{
		SPELLCHECK_Error = -1,
		SPELLCHECK_Cancelled = 0,			// User said no more.
		SPELLCHECK_Continue					// Continue checking.
	};

	CSpellChecker(CPmwView* pView);
	virtual ~CSpellChecker();

	// Session management.
	BOOL OpenSession(void);
	void CloseSession(void);

	// Generic entry. Do the entire spelling cycle.
	int CheckSpelling(void);

	// Spell checker story spell check sub-entry.
	int CheckStorySpelling(DB_RECORD_NUMBER lStory, CHARACTER_INDEX lStart = 0, CHARACTER_INDEX lEnd = -1, CHARACTER_INDEX* pAlignedStart = NULL);

	// Spell check some words.
	// Entry class is a general, abstracted group of words.
	// Generally useful.
	int CheckWordsSpelling(CSpellCheckWords* pWords);

protected:
	CPmwView* m_pView;
	CPmwDoc* m_pDoc;
	CViewEditState* m_pViewEditState;
	BOOL m_fFoundMisspelling;
	BOOL m_fCorrectedMisspelling;

	CSpellEngine m_Engine;
	S16 m_nSession;

	// The lexicons we use.
	S16 m_nMainLexicon;
	S16 m_nMainLexiconLang;			// Language of the main lexicon.
	S16 m_nUserLexicon;
	S16 m_nIgnoreAllLexicon;
	S16 m_nChangeAllLexicon;

	// Spelling configuration flags.
	BOOL m_fIgnoreNumbers;
	BOOL m_fIgnoreUpperCase;

	// Lexicon management.
	BOOL OpenMainLexicon(void);
	BOOL OpenUserLexicon(void);
	BOOL OpenIgnoreAllLexicon(void);
	BOOL OpenChangeAllLexicon(void);
	void CloseLexicon(S16& nLexicon);
	S16 GetMainLexiconLang(void);

	int HandleMisspelledWord(LPCSTR pWord, LPSTR pReplWord, int nReplWordSize);

	void ReadIniFileSettings(void);
	void WriteIniFileSettings(void);
};

/////////////////////////////////////////////////////////////////////////////
// The abstract base class spell-check word source.
// Not much functionality here.

class CSpellCheckWords
{
public:
	virtual void StartSpellCheck(void) = 0;
	virtual BOOL NextWord(char* pWord, int nWordSizeMax) = 0;
	virtual void ShowCurrentWord(void) = 0;
	virtual void ReplaceWord(LPCSTR pReplaceText, BOOL fAutoReplace) = 0;
	virtual void FinishSpellCheck(BOOL fFinished) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// The word source based on a text record.

class CTextRecordWords : public CSpellCheckWords
{
public:
	CTextRecordWords(CPmwView* pView, CTextRecord* pText, CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd);

	virtual void StartSpellCheck(void);
	virtual BOOL NextWord(char* pWord, int nWordSizeMax);
	virtual void ShowCurrentWord(void);
	virtual void ReplaceWord(LPCSTR pReplaceText, BOOL fAutoReplace);
	virtual void FinishSpellCheck(BOOL fFinished);

	CHARACTER_INDEX StartCharacter(void) const
		{ return m_lStart; }
protected:
	CPmwView* m_pView;
	CTextRecord* m_pText;
	CHARACTER_INDEX m_lStart;
	CHARACTER_INDEX m_lEnd;

	CHARACTER_INDEX m_lWordStart;
	CHARACTER_INDEX m_lWordEnd;

	// How much text have we replaced?
	CHARACTER_INDEX m_lReplaceStart;
	CHARACTER_INDEX m_lReplaceEnd;

	// Character iterators.
	CHARACTER_INDEX m_lIndex;
	CHARACTER_COUNT m_lCharacters;
	CHARACTER* m_pCharacter;

	int NextBaseWord(char* pWord, int nWordSizeMax);
	CHARACTER GetCharacter(void);
	void NextCharacter(void);

	// Flow all text that has been replaced up to now.
	void FlowReplacedText(void);

	// Make sure we are not starting in the middle of a solid word.
	void WordAlign(void);
};

#endif
