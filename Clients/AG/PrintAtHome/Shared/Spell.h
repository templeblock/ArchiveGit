#pragma once
#include "ssce.h" // Wintertree Spell Check

typedef SSCE_S16 (API *PFNGetSid)(void);
typedef SSCE_S16 (API *PFNCheckString)(SSCE_S16 sid, const SSCE_CHAR FAR *str, SSCE_S32 FAR *cursor, SSCE_CHAR FAR *errWord, SSCE_S16 errWordSz, SSCE_CHAR FAR *otherWord, SSCE_S16 otherWordSz);
typedef SSCE_S32 (API *PFNCheckStringDlg)(HWND hWndParent, SSCE_CHAR FAR* block, SSCE_S32 blkLen, SSCE_S32 blkSz, SSCE_S16 showContext, HINSTANCE clientInst, const TCHAR *spellDlgTmplt, const TCHAR* dictDlgTmplt, const TCHAR* optDlgTmplt, const TCHAR* newLexDlgTmplt);
typedef SSCE_S16 (API *PFNResetLex)(void);
typedef SSCE_S16 (API *PFNSetKey)(SSCE_U32 key);
typedef SSCE_U32 (API *PFNSetOption)(SSCE_S16 sid, SSCE_U32 opt, SSCE_U32 val);
typedef SSCE_S16 (API *PFNSetMainLexPath)(const TCHAR *path);
typedef SSCE_S16 (API *PFNSetUserLexPath)(const TCHAR *path);
typedef SSCE_S16 (API *PFNSetMainLexFiles)(const TCHAR *fileList);
typedef SSCE_S16 (API *PFNSetUserLexFiles)(const TCHAR *fileList);
typedef void (API *PFNGetStatistics)(SSCE_U32 *wordsChecked, SSCE_U32 *wordsChanged,  SSCE_U32 *errorsDetected);
typedef SSCE_S16 (API *PFNSetAutoCorrect)(SSCE_S16 ac);
typedef SSCE_S16 (API *PFNSetMinSuggestDepth)(SSCE_S16 depth);

class CSpell
{
public:
	CSpell(bool bUK);
	~CSpell();
	void GetFileNames(CString& strFileDLL, CString& strFileMainLex1, CString& strFileMainLex2, CString& strFileUserLex);
	bool Init(bool bCheckFiles, UINT idFileDLL = NULL, UINT idFileMainLex1 = NULL, UINT idFileMainLex2 = NULL, UINT idFileUserLex = NULL);
	void GetSpellCheckerPath(CString& strPath);
	bool SpellCheck(BYTE* pTextString, int iLength, int iMaxLength, DWORD* pdwWordsChecked, DWORD* pdwWordsChanged, DWORD* pdwErrorsDetected);
	bool CheckString(const CString& strText, CString& strResult, int iFlags);

protected:
	bool m_bUK;
	HMODULE m_hSpellLib;
	PFNGetSid m_pfnGetSid;
	PFNCheckString m_pfnCheckString;
	PFNCheckStringDlg m_pfnCheckStringDlg;
	PFNGetStatistics m_pfnGetStatistics;
};
