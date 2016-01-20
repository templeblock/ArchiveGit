// Sentry Spelling Checker Engine
// C++ API for the Sentry Spelling-Checker Engine.
//
// Copyright (c) 1994 Wintertree Software Inc.
// www.wintertree-software.com
//
// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.
//
// $Id: ssce.hpp,v 5.14 2000/06/28 17:31:12 wsi Exp wsi $

#if !defined(SSCE_HPP_)
#define SSCE_HPP_

#include <ssce.h>

// SSCE session class:
class SSCE_Session {
public:
	SSCE_Session(void) {
		sid = SSCE_OpenSession();
	}

	// Construct a session given an open session id:
	SSCE_Session(SSCE_S16 aSid) :
	  sid(aSid) {
	  	// Do nothing.
	}

	~SSCE_Session() {
		if (sid >= 0) {
			SSCE_CloseSession(sid);
		}
	}

	SSCE_S16 CheckString(const SSCE_CHAR FAR *str, SSCE_S32 FAR *cursor,
	  SSCE_CHAR FAR *errWord, SSCE_S16 errWordSz,
	  SSCE_CHAR FAR *otherWord, SSCE_S16 otherWordSz) {
		return (SSCE_CheckString(sid, str, cursor, errWord, errWordSz,
		  otherWord, otherWordSz));
	}

	SSCE_S16 CheckWord(const SSCE_CHAR FAR *word,
	  SSCE_CHAR FAR *replWord, SSCE_S16 replWordSz) {
	  	return (SSCE_CheckWord(sid, word, replWord, replWordSz));
	}

	void CompressLexAbort(void) {
		SSCE_CompressLexAbort(sid);
	}

	SSCE_S16 CompressLexInit(const char FAR *lexFileName,
	  const char FAR *suffixFileName, SSCE_S16 langId, SSCE_U32 FAR *errLine) {
	  	return (SSCE_CompressLexInit(sid, lexFileName, suffixFileName,
	  	  langId, errLine));
	}

	SSCE_S16 CompressLexEnd(void) {
		return (SSCE_CompressLexEnd(sid));
	}

	SSCE_S16 CompressLexFile(const char FAR *fileName, SSCE_U32 FAR *errLine) {
		return (SSCE_CompressLexFile(sid, fileName, errLine));
	}

	SSCE_S32 CountStringWords(const SSCE_CHAR FAR *str) {
		return (SSCE_CountStringWords(sid, str));
	}

	SSCE_S32 DelStringText(SSCE_CHAR FAR *str, SSCE_S32 cursor,
	  SSCE_S32 numChars) {
		return (SSCE_DelStringText(sid, str, cursor, numChars));
	}

	SSCE_S32 DelStringWord(SSCE_CHAR FAR *str, SSCE_S32 cursor,
	  SSCE_CHAR FAR *delText, SSCE_S32 delTextSz) {
		return (SSCE_DelStringWord(sid, str, cursor, delText, delTextSz));
	}

	SSCE_U32 GetOption(SSCE_U32 opt) {
		return (SSCE_GetOption(sid, opt));
	}

	SSCE_S32 GetStringWord(const SSCE_CHAR FAR *str, SSCE_S32 cursor,
	  SSCE_CHAR FAR *word, SSCE_S16 wordSz) {
		return (SSCE_GetStringWord(sid, str, cursor, word, wordSz));
	}

	int IsValid(void) {
		return (sid >= 0);
	}

	SSCE_S32 InsertStringText(SSCE_CHAR FAR *str, SSCE_S32 strSz,
	  SSCE_S32 cursor, const SSCE_CHAR FAR *text) {
		return (SSCE_InsertStringText(sid, str, strSz, cursor, text));
	}

	SSCE_S32 ReplaceStringWord(SSCE_CHAR FAR *str, SSCE_S32 strSz,
	  SSCE_S32 cursor, const SSCE_CHAR FAR *word) {
		return (SSCE_ReplaceStringWord(sid, str, strSz, cursor, word));
	}

	void SetDebugFile(const char FAR *debugFile) {
		SSCE_SetDebugFile(debugFile);
	}

	SSCE_U32 SetOption(SSCE_U32 opt, SSCE_U32 val) {
		return (SSCE_SetOption(sid, opt, val));
	}

	SSCE_S16 Suggest(const SSCE_CHAR FAR *word, SSCE_S16 depth,
	  SSCE_CHAR FAR *suggBfr, SSCE_S32 suggBfrSz, SSCE_S16 FAR *scores, SSCE_S16 scoreSz) {
	  	return (SSCE_Suggest(sid, word, depth, suggBfr, suggBfrSz, scores,
	  	  scoreSz));
	}

	void Version(char FAR *version, SSCE_S16 versionSz) {
		SSCE_Version(version, versionSz);
	}

	friend class SSCE_Block;
	friend class SSCE_Lexicon;
protected:
	SSCE_S16 sid;
};

// Text block used within an SSCE session.
class SSCE_Block {
public:
	SSCE_Block(SSCE_Session FAR *aSession, SSCE_CHAR FAR *block,
	  SSCE_S32 blkLen, SSCE_S32 blkSz, SSCE_S16 copyBlock = FALSE) :
	  session(aSession) {
	  	blkId = SSCE_OpenBlock(session->sid, block, blkLen, blkSz, copyBlock);
	}

	// Deferred opening.
	SSCE_Block(void) :
	  session(0),
	  blkId(-1) {
	  	// Do nothing.
	}

	~SSCE_Block() {
		if (blkId >= 0) {
			SSCE_CloseBlock(session->sid, blkId);
		}
	}

	SSCE_S16 Check(SSCE_CHAR FAR *errWord, SSCE_S16 errWordSz,
	  SSCE_CHAR FAR *otherWord, SSCE_S16 otherWordSz) {
	  	return (SSCE_CheckBlock(session->sid, blkId, errWord,
	  	  errWordSz, otherWord, otherWordSz));
	}

	SSCE_S16 DeleteText(SSCE_S32 numChars) {
		return (SSCE_DelBlockText(session->sid, blkId, numChars));
	}
	
	SSCE_S32 DeleteWord(void) {
		return (SSCE_DelBlockWord(session->sid, blkId, NULL, 0));
	}

	SSCE_S32 DeleteWord(SSCE_CHAR FAR *delText, SSCE_S16 delTextSz) {
		return (SSCE_DelBlockWord(session->sid, blkId, delText, delTextSz));
	}

	SSCE_S16 GetInfo(SSCE_S32 FAR *blkLen, SSCE_S32 FAR *blkSz, SSCE_S32 FAR *curPos,
	  SSCE_S32 FAR *wordCount) {
	  	return (SSCE_GetBlockInfo(session->sid, blkId, blkLen, blkSz, curPos,
	  	  wordCount));
	}

	SSCE_S16 GetWord(SSCE_CHAR FAR *word, SSCE_S16 wordSz) {
		return (SSCE_GetBlockWord(session->sid, blkId, word, wordSz));
	}

	SSCE_S32 GetBlock(SSCE_CHAR FAR *block, SSCE_S32 blkSz) {
		return (SSCE_GetBlock(session->sid, blkId, block, blkSz));
	}

	SSCE_S16 InsertText(const SSCE_CHAR FAR *text) {
		return (SSCE_InsertBlockText(session->sid, blkId, text));
	}

	int IsValid(void) {
		return (session != 0 && blkId >= 0);
	}

	SSCE_S16 NextWord(void) {
		return (SSCE_NextBlockWord(session->sid, blkId));
	}

	SSCE_S16 Open(SSCE_Session FAR *aSession, SSCE_CHAR FAR *block, SSCE_S32 blkLen,
	  SSCE_S32 blkSz, SSCE_S16 copyBlock = FALSE) {
	  	session = aSession;
		if (blkId >= 0) {
			SSCE_CloseBlock(session->sid, blkId);
		}
		blkId = SSCE_OpenBlock(session->sid, block, blkLen, blkSz, copyBlock);
		return (blkId);
	}

	SSCE_S16 ReplaceWord(const SSCE_CHAR FAR *word) {
		return (SSCE_ReplaceBlockWord(session->sid, blkId, word));
	}

	SSCE_S16 SetCursor(SSCE_S32 cursor) {
		return (SSCE_SetBlockCursor(session->sid, blkId, cursor));
	}

protected:
	SSCE_Session *session;
	SSCE_S16 blkId;
};

// Lexicon used within a session:
class SSCE_Lexicon {
public:
	// Open existing lexicon
	SSCE_Lexicon(SSCE_Session FAR *aSession, const char FAR *fileName,
	  SSCE_S32 memBudget = 0) :
	  session(aSession) {
	  	lexId = SSCE_OpenLex(session->sid, fileName, memBudget);
	}

	// Create a new lexicon
	SSCE_Lexicon(SSCE_Session FAR *aSession, const char FAR *fileName,
	  SSCE_S16 lang) :
	  session(aSession) {
		lexId = SSCE_CreateLex(session->sid, fileName, lang);
	} 

	// Deferred open or creation.
	SSCE_Lexicon(void) :
	  session(0),
	  lexId(-1) {
	  	// Do nothing.
	}

	~SSCE_Lexicon() {
		if (lexId >= 0) {
			SSCE_CloseLex(session->sid, lexId);
		}
	}

	SSCE_S16 AddWord(const SSCE_CHAR FAR *word, SSCE_S16 action,
	  const SSCE_CHAR FAR *otherWord) {
		return (SSCE_AddToLex(session->sid, lexId, word, action, otherWord));
	}

	SSCE_S16 Clear(void) {
		return (SSCE_ClearLex(session->sid, lexId));
	}

	SSCE_S16 Create(SSCE_Session FAR *aSession, const char FAR *fileName,
	  SSCE_S16 lang) {
	  	session = aSession;
		if (lexId >= 0) {
			SSCE_CloseLex(session->sid, lexId);
		}
		lexId = SSCE_CreateLex(session->sid, fileName, lang);
		return (lexId);
	}

	SSCE_S16 DeleteWord(const SSCE_CHAR FAR *word) {
		return (SSCE_DelFromLex(session->sid, lexId, word));
	}

	SSCE_S16 FindLexWord(const SSCE_CHAR FAR *word,
	  SSCE_CHAR FAR *otherWord, SSCE_S16 otherWordSz) {
		return (SSCE_FindLexWord(session->sid, lexId, word, otherWord,
		  otherWordSz));
	}

	SSCE_S32 GetText(SSCE_CHAR FAR *lexBfr, SSCE_S32 lexBfrSz) {
		return (SSCE_GetLex(session->sid, lexId, lexBfr, lexBfrSz));
	}

	SSCE_S16 GetInfo(SSCE_S32 FAR *size, SSCE_S16 FAR *format,
	  SSCE_S16 FAR *lang) {
		return (SSCE_GetLexInfo(session->sid, lexId, size, format, lang));
	}

	int IsValid(void) {
		return (session != 0 && lexId >= 0);
	}

	SSCE_S16 Open(SSCE_Session FAR *aSession, const char FAR *fileName,
	  SSCE_S32 memoryBudget = 0) {
	  	session = aSession;
		if (lexId >= 0) {
			SSCE_CloseLex(session->sid, lexId);
		}
		lexId = SSCE_OpenLex(session->sid, fileName, memoryBudget);
		return (lexId);
	}

protected:
	SSCE_Session *session;
	SSCE_S16 lexId;
};

#endif // SSCE_HPP_
