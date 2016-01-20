/* SSCE: Sentry Spelling Checker Engine
 * Copyright (c) 1993 Wintertree Software Inc.
 * www.wintertree-software.com
 *
 * Use, duplication, and disclosure of this file is governed by
 * a license agreement between Wintertree Software Inc. and
 * the licensee.
 *
 * $Id: ssce.h,v 5.14.10 2000/06/28 17:31:12 wsi Exp wsi $
 */

#if !defined(SSCE_H_)
#define SSCE_H_

#include <stddef.h>

#if (defined(_WIN16) || defined(_WIN32)) && !defined(SSCE_NO_WINDOWS)
  #include <windows.h>
  #include <tchar.h>	/* contains definition of TCHAR */
  #if !defined(TCHAR)
    #define TCHAR char
  #endif
#endif

/* Limits. */
#define SSCE_MAX_WORD_LEN 80
#define SSCE_MAX_WORD_SZ (SSCE_MAX_WORD_LEN + 1)
#define SSCE_MAX_SUGGEST_DEPTH 100

/* Error codes. Note that these are always negative. */
#define SSCE_TOO_MANY_SESSIONS_ERR (-1)
#define SSCE_BAD_SESSION_ID_ERR (-2)
#define SSCE_WORD_NOT_FOUND_ERR (-3)
#define SSCE_FILE_NOT_FOUND_ERR (-4)
#define SSCE_TOO_MANY_LEXICONS_ERR (-5)
#define SSCE_UNKNOWN_ACTION_ERR (-6)
#define SSCE_BAD_LEXICON_ID_ERR (-7)
#define SSCE_BUFFER_TOO_SMALL_ERR (-8)
#define SSCE_READ_ONLY_LEXICON_ERR (-9)
#define SSCE_OUT_OF_MEMORY_ERR (-12)
#define SSCE_UNSUPPORTED_ACTION_ERR (-13)
#define SSCE_LEXICON_EXISTS_ERR (-14)
#define SSCE_TOO_MANY_BLOCKS_ERR (-15)
#define SSCE_BAD_BLOCK_ID_ERR (-16)
#define SSCE_CANCEL_ERR (-17)
#define SSCE_INVALID_WORD_ERR (-19)
#define SSCE_WORD_OUT_OF_SEQUENCE_ERR (-20)
#define SSCE_FILE_READ_ERR (-22)
#define SSCE_FILE_WRITE_ERR (-23)
#define SSCE_FILE_OPEN_ERR (-24)
#define SSCE_BUSY_ERR (-25)
#define SSCE_UNKNOWN_LEX_FORMAT_ERR (-26)

/* Spell-check result masks. Note that these will always result in a
 * positive value.
 */
#define SSCE_OK_RSLT 0x0000
#define SSCE_MISSPELLED_WORD_RSLT 0x0001
#define SSCE_AUTO_CHANGE_WORD_RSLT 0x0002
#define SSCE_CONDITIONALLY_CHANGE_WORD_RSLT 0x0004
#define SSCE_UNCAPPED_WORD_RSLT 0x0008
#define SSCE_MIXED_CASE_WORD_RSLT 0x0010
#define SSCE_MIXED_DIGITS_WORD_RSLT 0x0020
#define SSCE_END_OF_BLOCK_RSLT 0x0040
#define SSCE_END_OF_TEXT_RSLT SSCE_END_OF_BLOCK_RSLT
#define SSCE_DOUBLED_WORD_RSLT 0x0080

/* Options. */
#define SSCE_IGNORE_CAPPED_WORD_OPT 0x0001L
#define SSCE_IGNORE_MIXED_CASE_OPT 0x0002L
#define SSCE_IGNORE_MIXED_DIGITS_OPT 0x0004L
#define SSCE_IGNORE_ALL_CAPS_WORD_OPT 0x0008L
#define SSCE_REPORT_UNCAPPED_OPT 0x0010L
#define SSCE_REPORT_MIXED_CASE_OPT 0x0020L
#define SSCE_REPORT_MIXED_DIGITS_OPT 0x0040L
#define SSCE_REPORT_SPELLING_OPT 0x0080L
#define SSCE_REPORT_DOUBLED_WORD_OPT 0x0100L
#define SSCE_CASE_SENSITIVE_OPT 0x0200L
#define SSCE_SPLIT_HYPHENATED_WORDS_OPT 0x0400L
#define SSCE_SPLIT_CONTRACTED_WORDS_OPT 0x0800L
#define SSCE_SPLIT_WORDS_OPT 0x1000L
#define SSCE_SUGGEST_SPLIT_WORDS_OPT 0x2000L
#define SSCE_SUGGEST_PHONETIC_OPT 0x4000L
#define SSCE_SUGGEST_TYPOGRAPHICAL_OPT 0x8000L
#define SSCE_STRIP_POSSESSIVES_OPT 0x10000L
#define SSCE_IGNORE_NON_ALPHA_WORD_OPT 0x20000L
#define SSCE_IGNORE_DOMAIN_NAMES_OPT 0x40000L
#define SSCE_ALLOW_ACCENTED_CAPS_OPT 0x80000L
#define SSCE_CHECK_SINGLE_WORD_OPT 0x100000L
#define SSCE_IGNORE_HTML_MARKUPS_OPT 0x200000L
#define SSCE_LANGUAGE_OPT 0x80000002L

/* Lexicon formats: */
#define SSCE_COMPRESSED_LEX_FMT 0
#define SSCE_TEXT_LEX_FMT 1

/* Actions associated with words in text lexicons. */
#define SSCE_AUTO_CHANGE_ACTION (SSCE_S16)'a'
#define SSCE_AUTO_CHANGE_PRESERVE_CASE_ACTION (SSCE_S16)'A'
#define SSCE_CONDITIONAL_CHANGE_ACTION (SSCE_S16)'c'
#define SSCE_CONDITIONAL_CHANGE_PRESERVE_CASE_ACTION (SSCE_S16)'C'
#define SSCE_EXCLUDE_ACTION (SSCE_S16)'e'
#define SSCE_IGNORE_ACTION (SSCE_S16)'i'

/* Language ids: */
#define SSCE_ANY_LANG 30840					/* "xx" */
#define SSCE_AMER_ENGLISH_LANG 24941		/* "am" */
#define SSCE_BRIT_ENGLISH_LANG 25202		/* "br" */
#define SSCE_CANADIAN_ENGLISH_LANG 25441	/* "ca" */
#define SSCE_CATALAN_LANG 29539				/* "sc" */
#define SSCE_CZECH_LANG 25466				/* "cz" */
#define SSCE_DANISH_LANG 25697				/* "da" */
#define SSCE_DUTCH_LANG 25717				/* "du" */
#define SSCE_FINNISH_LANG 26217				/* "fi" */
#define SSCE_FRENCH_LANG 26226				/* "fr" */
#define SSCE_GERMAN_LANG 26469				/* "ge" */
#define SSCE_HUNGARIAN_LANG 26741			/* "hu" */
#define SSCE_ITALIAN_LANG 26996				/* "it" */
#define SSCE_NORWEGIAN_BOKMAL_LANG 25442	/* "cb" */
#define SSCE_NORWEGIAN_NYNORSK_LANG 25444	/* "cd" */
#define SSCE_POLISH_LANG 28780				/* "pl" */
#define SSCE_PORTUGUESE_BRAZIL_LANG 28770	/* "pb" */
#define SSCE_PORTUGUESE_IBERIAN_LANG 28783	/* "po" */
#define SSCE_RUSSIAN_LANG 29301				/* "ru" */
#define SSCE_SPANISH_LANG 29552				/* "sp" */
#define SSCE_SWEDISH_LANG 29559				/* "sw" */

/* Character sets: */
#define SSCE_LATIN1_CHARSET 1
#define SSCE_UNICODE_CHARSET 2

/* Data types used to ensure portability across compilers and O/Ss. These
 * may need to be redefined to suit a particular platform. The types are
 * named as follows: S = signed; U = unsigned; the number following is the
 * (minimum) width in bits.
 */
#if !defined(SSCE_S8)
  #define SSCE_S8 signed char
#endif
#if !defined(SSCE_U8)
  #define SSCE_U8 unsigned char
#endif
#if !defined(SSCE_S16)
  #define SSCE_S16 signed short
#endif
#if !defined(SSCE_U16)
  #define SSCE_U16 unsigned short
#endif
#if !defined(SSCE_S32)
  #define SSCE_S32 signed long
#endif
#if !defined(SSCE_U32)
  #define SSCE_U32 unsigned long
#endif

/* Data type used to represent text checked by the engine. This can be
 * redefined to just "char" if your application never uses accented characters.
 */
#if !defined(SSCE_CHAR)
	#if defined(SSCE_UNICODE)
		#define SSCE_CHAR wchar_t
	#else
		#define SSCE_CHAR SSCE_U8
	#endif
#endif

/* Non-local pointer qualifier for segmented architectures: */
#if !defined(FAR)
  #if defined(_WIN16)
	#define FAR _far
  #else
	#define FAR
  #endif
#endif

/* API function declaration qualifier: */
#if !defined(API)
  #if (defined(_WIN16) || defined(_WIN32)) && !defined(SSCE_NO_WINDOWS)
	#define API WINAPI
  #else
	#define API
  #endif
#endif

/* Some useful boolean constants: */
#if !defined(TRUE)
  #define TRUE 1
#endif
#if !defined(FALSE)
  #define FALSE 0
#endif

/* Function definitions: */

#if defined(__cplusplus)
extern "C" {
#endif

/* Functions available in the basic SSCE API: */

extern SSCE_S16 API SSCE_AddToLex(SSCE_S16 sid, SSCE_S16 lexId,
  const SSCE_CHAR FAR *word, SSCE_S16 action, const SSCE_CHAR FAR *otherWord);

extern SSCE_S16 API SSCE_CheckBlock(SSCE_S16 sid, SSCE_S16 blkId,
  SSCE_CHAR FAR *errWord, SSCE_S16 errWordSz,
  SSCE_CHAR FAR *otherWord, SSCE_S16 otherWordSz);

extern SSCE_S16 API SSCE_CheckString(SSCE_S16 sid, const SSCE_CHAR FAR *str,
  SSCE_S32 FAR *cursor, SSCE_CHAR FAR *errWord, SSCE_S16 errWordSz,
  SSCE_CHAR FAR *otherWord, SSCE_S16 otherWordSz);

extern SSCE_S16 API SSCE_CheckWord(SSCE_S16 sid, const SSCE_CHAR FAR *word,
  SSCE_CHAR FAR *otherWord, SSCE_S16 otherWordSz);

extern SSCE_S16 API SSCE_ClearLex(SSCE_S16 sid, SSCE_S16 lexId);

extern SSCE_S16 API SSCE_CloseBlock(SSCE_S16 sid, SSCE_S16 blkId);

extern SSCE_S16 API SSCE_CloseLex(SSCE_S16 sid, SSCE_S16 lexId);

extern SSCE_S16 API SSCE_CloseSession(SSCE_S16 sid);

extern void API SSCE_CompressLexAbort(SSCE_S16 sid);

extern SSCE_S16 API SSCE_CompressLexEnd(SSCE_S16 sid);

extern SSCE_S16 API SSCE_CompressLexFile(SSCE_S16 sid,
  const char FAR *fileName, SSCE_U32 FAR *errLine);
 
extern SSCE_S16 API SSCE_CompressLexInit(SSCE_S16 sid,
  const char FAR *lexFileName, const char FAR *suffixFileName,
  SSCE_S16 langId, SSCE_U32 FAR *errLine);

extern SSCE_S32 API SSCE_CountStringWords(SSCE_S16 sid,
  const SSCE_CHAR FAR *str);

extern SSCE_S16 API SSCE_CreateLex(SSCE_S16 sid,
  const char FAR *fileName, SSCE_S16 lang);

extern SSCE_S16 API SSCE_DelBlockText(SSCE_S16 sid, SSCE_S16 blkId,
  SSCE_S32 numChars);

extern SSCE_S32 API SSCE_DelBlockWord(SSCE_S16 sid, SSCE_S16 blkId,
  SSCE_CHAR FAR *delText, SSCE_S16 delTextSz);

extern SSCE_S16 API SSCE_DelFromLex(SSCE_S16 sid, SSCE_S16 lexId,
  const SSCE_CHAR FAR *word);

extern SSCE_S32 API SSCE_DelStringText(SSCE_S16 sid, SSCE_CHAR FAR *str,
  SSCE_S32 cursor, SSCE_S32 numChars);

extern SSCE_S32 API SSCE_DelStringWord(SSCE_S16 sid, SSCE_CHAR *str,
  SSCE_S32 cursor, SSCE_CHAR FAR *delText, SSCE_S32 delTextSz);

extern SSCE_S16 API SSCE_FindLexWord(SSCE_S16 sid, SSCE_S16 lexId,
  const SSCE_CHAR FAR *word, SSCE_CHAR FAR *otherWord, SSCE_S16 otherWordSz);

extern SSCE_S32 API SSCE_GetBlock(SSCE_S16 sid, SSCE_S16 blkId,
  SSCE_CHAR FAR *block, SSCE_S32 blkSz);

extern SSCE_S16 API SSCE_GetBlockInfo(SSCE_S16 sid, SSCE_S16 blkId,
  SSCE_S32 FAR *blkLen, SSCE_S32 FAR *blkSz, SSCE_S32 FAR *curPos,
  SSCE_S32 FAR *wordCount);

extern SSCE_S16 API SSCE_GetBlockWord(SSCE_S16 sid, SSCE_S16 blkId,
  SSCE_CHAR FAR *word, SSCE_S16 wordSz);

extern SSCE_S32 API SSCE_GetLex(SSCE_S16 sid, SSCE_S16 lexId,
  SSCE_CHAR FAR *lexBfr, SSCE_S32 lexBfrSz);

extern SSCE_S16 API SSCE_GetLexInfo(SSCE_S16 sid, SSCE_S16 lexId,
  SSCE_S32 FAR *size, SSCE_S16 FAR *format, SSCE_S16 FAR *lang);

extern SSCE_U32 API SSCE_GetOption(SSCE_S16 sid, SSCE_U32 opt);

extern SSCE_S32 API SSCE_GetStringWord(SSCE_S16 sid, const SSCE_CHAR FAR *str,
  SSCE_S32 cursor, SSCE_CHAR FAR *word, SSCE_S16 wordSz);

extern SSCE_S16 API SSCE_InsertBlockText(SSCE_S16 sid, SSCE_S16 blkId,
  const SSCE_CHAR FAR *text);

extern SSCE_S32 API SSCE_InsertStringText(SSCE_S16 sid, SSCE_CHAR FAR *str,
  SSCE_S32 strSz, SSCE_S32 cursor, const SSCE_CHAR FAR *text);

extern SSCE_S16 API SSCE_NextBlockWord(SSCE_S16 sid, SSCE_S16 blkId);

extern SSCE_S16 API SSCE_OpenBlock(SSCE_S16 sid, SSCE_CHAR FAR *block,
  SSCE_S32 blkLen, SSCE_S32 blkSz, SSCE_S16 copyBlock);

extern SSCE_S16 API SSCE_OpenLex(SSCE_S16 sid,
  const char FAR *fileName, SSCE_S32 memBudget);

extern SSCE_S16 API SSCE_OpenSession(void);

extern SSCE_S16 API SSCE_ReplaceBlockWord(SSCE_S16 sid,
  SSCE_S16 blkId, const SSCE_CHAR FAR *word);

extern SSCE_S32 API SSCE_ReplaceStringWord(SSCE_S16 sid, SSCE_CHAR FAR *str,
  SSCE_S32 strSz, SSCE_S32 cursor, const SSCE_CHAR FAR *word);

extern SSCE_S16 API SSCE_SetBlockCursor(SSCE_S16 sid, SSCE_S16 blkId,
  SSCE_S32 cursor);

extern void API SSCE_SetDebugFile(const char FAR *debugFile);

extern SSCE_U32 API SSCE_SetOption(SSCE_S16 sid, SSCE_U32 opt,
  SSCE_U32 val);

extern SSCE_S16 API SSCE_Suggest(SSCE_S16 sid, const SSCE_CHAR FAR *word,
  SSCE_S16 depth, SSCE_CHAR FAR *suggBfr, SSCE_S32 suggBfrSz,
  SSCE_S16 FAR *scores, SSCE_S16 scoreSz);

extern void API SSCE_Version(char *version, SSCE_U16 versionSz);

/* Functions available in the Windows API only: */

#if (defined(_WIN16) || defined(_WIN32)) && !defined(SSCE_NO_WINDOWS)

extern SSCE_S32 API SSCE_CheckBlockDlg(HWND parent, SSCE_CHAR FAR *block,
  SSCE_S32 blkLen, SSCE_S32 blkSz, SSCE_S16 showContext);

extern SSCE_S32 API SSCE_CheckBlockDlgTmplt(HWND parent, SSCE_CHAR FAR *block,
  SSCE_S32 blkLen, SSCE_S32 blkSz, SSCE_S16 showContext, HINSTANCE clientInst,
  const TCHAR FAR *spellDlgTmpltName,
  const TCHAR FAR *lexDlgTmpltName, const TCHAR FAR *optDlgTmpltName,
  const TCHAR FAR *newLexDlgTmpltName);

extern SSCE_S16 API SSCE_CheckCtrlBackground(HWND ctrlWin);

extern SSCE_S16 API SSCE_CheckCtrlDlg(HWND parent, HWND ctrlWin,
  SSCE_S16 selTextOnly);

extern SSCE_S16 API SSCE_CheckCtrlDlgTmplt(HWND parent, HWND ctrlWin,
  SSCE_S16 selTextOnly, HINSTANCE clientInst,
  const TCHAR FAR *spellDlgTmpltName,
  const TCHAR FAR *lexDlgTmpltName, const TCHAR FAR *optDlgTmpltName,
  const TCHAR FAR *newLexDlgTmpltName);

extern SSCE_S16 API SSCE_EditLexDlg(HWND parent);

extern SSCE_S16 API SSCE_EditLexDlgTmplt(HWND parent, HINSTANCE hInst,
  const TCHAR FAR *dlgTmpltName, const TCHAR FAR *newLexDlgTmptName);

extern SSCE_S16 API SSCE_GetAutoCorrect(void);

extern void API SSCE_GetHelpFile(TCHAR FAR *fileName, SSCE_U16 fileNameSz);

extern SSCE_S16 API SSCE_GetLexId(const TCHAR FAR *lexFileName);

extern void API SSCE_GetMainLexFiles(TCHAR FAR *fileList,
  SSCE_U16 fileListSz);

extern void API SSCE_GetMainLexPath(TCHAR FAR *path, SSCE_U16 pathSz);

extern SSCE_S16 API SSCE_GetMinSuggestDepth(void);

extern void API SSCE_GetRegTreeName(TCHAR FAR *regTreeName,
  SSCE_U16 regTreeNameSz);

extern void API SSCE_GetSelUserLexFile(TCHAR FAR *fileName,
  SSCE_U16 fileNameSz);

extern SSCE_S16 API SSCE_GetSid(void);

extern void API SSCE_GetStatistics(SSCE_U32 FAR *wordsChecked,
  SSCE_U32 FAR *wordsChanged, SSCE_U32 FAR *errorsDetected);

extern void API SSCE_GetStringTableName(TCHAR FAR *tableName,
  SSCE_U16 tableNameSz);

extern void API SSCE_GetUserLexFiles(TCHAR FAR *fileList,
  SSCE_U16 fileListSz);

extern void API SSCE_GetUserLexPath(TCHAR FAR *path, SSCE_U16 pathSz);

extern SSCE_S16 API SSCE_OptionsDlg(HWND parent);

extern SSCE_S16 API SSCE_OptionsDlgTmplt(HWND parent, HINSTANCE hInst,
  const TCHAR FAR *tmpltName);

extern SSCE_S16 API SSCE_ResetLex(void);

extern SSCE_S16 API SSCE_SetAutoCorrect(SSCE_S16 ac);

extern void API SSCE_SetDialogOrigin(SSCE_S16 x, SSCE_S16 y);

extern SSCE_S16 API SSCE_SetHelpFile(const TCHAR FAR *helpFile);

extern SSCE_S16 API SSCE_SetIniFile(const TCHAR FAR *iniFile);

extern SSCE_S16 API SSCE_SetKey(SSCE_U32 key);

extern SSCE_S16 API SSCE_SetMainLexFiles(const TCHAR FAR *fileList);

extern SSCE_S16 API SSCE_SetMainLexPath(const TCHAR FAR *path);

extern SSCE_S16 API SSCE_SetMinSuggestDepth(SSCE_S16 depth);

extern SSCE_S16 API SSCE_SetRegTreeName(const TCHAR FAR *regTreeName);

extern SSCE_S16 API SSCE_SetSelUserLexFile(const TCHAR FAR *fileName);

extern SSCE_S16 API SSCE_SetStringTableName(const TCHAR FAR *tableName);

extern SSCE_S16 API SSCE_SetUserLexFiles(const TCHAR FAR *fileList);

extern SSCE_S16 API SSCE_SetUserLexPath(const TCHAR FAR *path);

#endif /* (defined(_WIN16) || defined(_WIN32) */

#if defined(__cplusplus)
}
#endif

#endif /* SSCE_H_ */
