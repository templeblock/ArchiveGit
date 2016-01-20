/*
// $Workfile: SPELLENG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SPELLENG.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:00   Fred
// Initial revision.
// 
//    Rev 1.1   02 Aug 1996 12:54:42   Jay
// 16-bit fix.
// 
//    Rev 1.0   31 Jul 1996 18:23:34   Jay
// Initial revision.
*/

class CSpellEngine;

#ifndef __SPELLENG_H__
#define __SPELLENG_H__

#ifndef WIN32
#define _WIN16					// It wants this!
#endif

#include "ssce.h"

/////////////////////////////////////////////////////////////////////////////
// The all-mighty spell-engine class.

class CSpellEngine
{
	// Session management
	typedef S16 (API* FN_OpenSession)(void);
	typedef void (API* FN_CloseSession)(S16 sid);

	// Lexicon management.
	typedef S16 (API* FN_OpenLex)(S16 sid, const char FAR *fileName, S32 memBudget);
	typedef S16 (API* FN_CreateLex)(S16 sid, const char FAR *fileName, S16 type, S16 lang);
	typedef S16 (API* FN_CloseLex)(S16 sid, S16 lexId);
	typedef S16 (API* FN_AddToLex)(S16 sid, S16 lexId, const SSCE_CHAR FAR *word, const SSCE_CHAR FAR *otherWord);
	typedef S16 (API* FN_GetLexInfo)(S16 sid, S16 lexId, S32 FAR *size, S16 FAR *format, S16 FAR *type, S16 FAR *lang);

	// Word spell check.
	typedef S16 (API* FN_CheckWord)(S16 sid, U32 options, const SSCE_CHAR FAR *word, SSCE_CHAR FAR *replWord, S16 replWordSz);
	typedef S16 (API* FN_Suggest)(S16 sid, const SSCE_CHAR FAR *word, S16 depth, SSCE_CHAR FAR *suggBfr, S32 suggBfrSz, S16 FAR *scores, S16 scoreSz);

public:
	CSpellEngine();
	virtual ~CSpellEngine();

	BOOL Startup(void);
	void Shutdown(void);

	BOOL IsValid(void) const
		{ return m_hLibrary != NULL; }

	// Session management
	S16 OpenSession(void);
	void CloseSession(S16 sid);

	// Lexicon management.
	S16 OpenLex(S16 sid, const char FAR *fileName, S32 memBudget);
	S16 CreateLex(S16 sid, const char FAR *fileName, S16 type, S16 lang);
	S16 CloseLex(S16 sid, S16 lexId);
	S16 AddToLex(S16 sid, S16 lexId, const SSCE_CHAR FAR *word, const SSCE_CHAR FAR *otherWord);
	S16 GetLexInfo(S16 sid, S16 lexId, S32 FAR *size, S16 FAR *format, S16 FAR *type, S16 FAR *lang);

	// Word spell check.
	S16 CheckWord(S16 sid, U32 options, const SSCE_CHAR FAR *word, SSCE_CHAR FAR *replWord, S16 replWordSz);
	S16 Suggest(S16 sid, const SSCE_CHAR FAR *word, S16 depth, SSCE_CHAR FAR *suggBfr, S32 suggBfrSz, S16 FAR *scores, S16 scoreSz);

protected:
	BOOL GetEntryPoints(void);

	HINSTANCE m_hLibrary;

	// Session management entry points.
	FN_OpenSession m_OpenSession;
	FN_CloseSession m_CloseSession;

	// Lexicon management entry points.
	FN_OpenLex m_OpenLex;
	FN_CreateLex m_CreateLex;
	FN_CloseLex m_CloseLex;
	FN_AddToLex m_AddToLex;
	FN_GetLexInfo m_GetLexInfo;

	// Word spell check entry points.
	FN_CheckWord m_CheckWord;
	FN_Suggest m_Suggest;
};

#endif
