// Sentry Spelling Checker Engine
// MFC wrapper around Sentry Windows API.
//
// Copyright (c) 1994 Wintertree Software Inc.
// www.wintertree-software.com
//
// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.
//
// $Id: sscemfc.hpp,v 5.14 2000/06/28 17:31:12 wsi Exp wsi $

#if !defined(SSCEMFC_HPP_)
#define SSCEMFC_HPP_

#include <ssce.h>

#if !defined(ARRAY_SIZE)
	#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

// Spelling-checker dialog class:
class CSentrySpellDlg {
public:
	CSentrySpellDlg(CWnd *pParentWnd = NULL) :
	  parentWnd(pParentWnd) {
	  	// Do nothing.
	}

	// Interactively check the spelling of a CString.
	SSCE_S32 Check(CString &str, BOOL showContext = TRUE,
	  HINSTANCE clientInst = 0, const TCHAR *spellDlgTmplt = 0,
	  const TCHAR *dictDlgTmplt = 0, const TCHAR *optDlgTmplt = 0,
	  const TCHAR *newLexDlgTmplt = 0) {
		// The CString's buffer will be modified in place. Allow
		// room for growth.
		SSCE_S32 growth = str.GetLength() / 5;
		growth = max(growth, 256);
		SSCE_S32 bfrSz = str.GetLength() + growth;
		SSCE_S32 rv = SSCE_CheckBlockDlgTmplt(parentWnd->GetSafeHwnd(),
		  (SSCE_CHAR *)str.GetBuffer(bfrSz), (SSCE_S32)str.GetLength(),
		  bfrSz, showContext, clientInst, spellDlgTmplt, dictDlgTmplt,
		  optDlgTmplt, newLexDlgTmplt);
		str.ReleaseBuffer();
		return (rv);
	}

	// Interactively check the spelling of a text block.
	SSCE_S32 Check(SSCE_CHAR *block, SSCE_S32 blkLen, SSCE_S32 blkSz,
	  BOOL showContext = TRUE, HINSTANCE clientInst = 0,
	  const TCHAR *spellDlgTmplt = 0, const TCHAR *dictDlgTmplt = 0,
	  const TCHAR *optDlgTmplt = 0, const TCHAR *newLexDlgTmplt = 0) {
	  	return (SSCE_CheckBlockDlgTmplt(parentWnd->GetSafeHwnd(), block, blkLen,
	  	  blkSz, showContext, clientInst, spellDlgTmplt, dictDlgTmplt,
	  	  optDlgTmplt, newLexDlgTmplt));
	}

	// Interactively check the spelling of a CWnd-derived control.
	SSCE_S16 Check(CWnd *control, BOOL checkSelectedOnly = FALSE,
	  HINSTANCE clientInst = 0, const TCHAR *spellDlgTmplt = 0,
	  const TCHAR *dictDlgTmplt = 0, const TCHAR *optDlgTmplt = 0,
	  const TCHAR *newLexDlgTmplt = 0) {
		return (SSCE_CheckCtrlDlgTmplt(parentWnd->GetSafeHwnd(),
		  control->m_hWnd, checkSelectedOnly, clientInst, spellDlgTmplt,
		  dictDlgTmplt, optDlgTmplt, newLexDlgTmplt));
	}

	// Interactively check the spelling of a control based on its hWnd.
	SSCE_S16 Check(HWND ctrlWin, BOOL checkSelectedOnly = FALSE,
	  HINSTANCE clientInst = 0, const TCHAR *spellDlgTmplt = 0,
	  const TCHAR *dictDlgTmplt = 0, const TCHAR *optDlgTmplt = 0,
	  const TCHAR *newLexDlgTmplt = 0) {
		return (SSCE_CheckCtrlDlgTmplt(parentWnd->GetSafeHwnd(),
		  ctrlWin, checkSelectedOnly, clientInst, spellDlgTmplt,
		  dictDlgTmplt, optDlgTmplt, newLexDlgTmplt));
	}

#if defined(_WIN32)
	// Check the spelling of a rich-edit control in the background
	SSCE_S16 CheckBackground(CRichEditCtrl *control) {
		return (SSCE_CheckCtrlBackground(control->m_hWnd));
	}
#endif

	SSCE_S16 CheckBackground(HWND ctrlWin) {
		return (SSCE_CheckCtrlBackground(ctrlWin));
	}

	SSCE_S16 GetAutoCorrect(void) {
		return (SSCE_GetAutoCorrect());
	}

	const CString GetHelpFile(void) {
		TCHAR helpFileName[512];
		SSCE_GetHelpFile(helpFileName, ARRAY_SIZE(helpFileName));
		return (CString(helpFileName));
	}

	SSCE_S16 GetLexId(const TCHAR *lexFileName) {
		return (SSCE_GetLexId(lexFileName));
	}

	const CString GetMainLexPath(void) {
		TCHAR path[512];
		SSCE_GetMainLexPath(path, ARRAY_SIZE(path));
		return (CString(path));
	}

	const CString GetMainLexFiles(void) {
		TCHAR files[512];
		SSCE_GetMainLexFiles(files, ARRAY_SIZE(files));
		return (CString(files));
	}

	SSCE_S16 GetMinSuggestDepth(void) {
		return (SSCE_GetMinSuggestDepth());
	}

	const CString GetRegTreeName(void) {
		TCHAR treeName[512];
		SSCE_GetRegTreeName(treeName, ARRAY_SIZE(treeName));
		return (CString(treeName));
	}

	const CString GetSelUserLexFile(void) {
		TCHAR fileName[512];
		SSCE_GetSelUserLexFile(fileName, ARRAY_SIZE(fileName));
		return (CString(fileName));
	}

	SSCE_S16 GetSid(void) {
		return (SSCE_GetSid());
	}

	void GetStatistics(SSCE_U32 &wordsChecked, SSCE_U32 &wordsChanged,
	  SSCE_U32 &errorsDetected) {
	  	SSCE_GetStatistics(&wordsChecked, &wordsChanged, &errorsDetected);
	}

	const CString GetStringTableName(void) {
		TCHAR stringTableName[80];
		SSCE_GetStringTableName(stringTableName, ARRAY_SIZE(stringTableName));
		return (CString(stringTableName));
	}

	const CString GetUserLexFiles(void) {
		TCHAR files[512];
		SSCE_GetUserLexFiles(files, ARRAY_SIZE(files));
		return (CString(files));
	}

	const CString GetUserLexPath(void) {
		TCHAR path[512];
		SSCE_GetUserLexPath(path, ARRAY_SIZE(path));
		return (CString(path));
	}

	SSCE_S16 ResetLex(void) {
		return (SSCE_ResetLex());
	}

	SSCE_S16 SetAutoCorrect(SSCE_S16 autoCorrect) {
		return (SSCE_SetAutoCorrect(autoCorrect));
	}

	void SetDialogOrigin(SSCE_S16 x = 0, SSCE_S16 y = 0) {
		SSCE_SetDialogOrigin(x, y);
	}

	SSCE_S16 SetHelpFile(const TCHAR *helpFile) {
		return (SSCE_SetHelpFile(helpFile));
	}

	SSCE_S16 SetIniFile(const TCHAR *iniFile) {
		return (SSCE_SetIniFile(iniFile));
	}

	SSCE_S16 SetKey(SSCE_U32 key) {
		return (SSCE_SetKey(key));
	}

	SSCE_S16 SetMainLexPath(const TCHAR *path) {
		return (SSCE_SetMainLexPath(path));
	}

	SSCE_S16 SetMainLexFiles(const TCHAR *fileList) {
		return (SSCE_SetMainLexFiles(fileList));
	}

	SSCE_S16 SetMinSuggestDepth(SSCE_S16 depth) {
		return (SSCE_SetMinSuggestDepth(depth));
	}

	SSCE_S16 SetRegTreeName(const TCHAR *regTreeName) {
		return (SSCE_SetRegTreeName(regTreeName));
	}

	SSCE_S16 SetSelUserLexFile(const TCHAR *fileName) {
		return (SSCE_SetSelUserLexFile(fileName));
	}

	SSCE_S16 SetStringTableName(const TCHAR *stringTableName) {
		return (SSCE_SetStringTableName(stringTableName));
	}

	SSCE_S16 SetUserLexFiles(const TCHAR *fileList) {
		return (SSCE_SetUserLexFiles(fileList));
	}

	SSCE_S16 SetUserLexPath(const TCHAR *path) {
		return (SSCE_SetUserLexPath(path));
	}

protected:
	CWnd *parentWnd;
};

// Spelling options dialog class:
class CSentryOptionsDlg {
public:
	CSentryOptionsDlg(CWnd *parent = NULL) :
	  parentWnd(parent) {
	  	// Do nothing.
	}

	int DoModal(void) {
		return ((int)SSCE_OptionsDlg(parentWnd->GetSafeHwnd()));
	}
protected:
	CWnd *parentWnd;
};

// Dictionaries dialog class:
class CSentryDictDlg {
public:
	CSentryDictDlg(CWnd *parent = NULL) :
	  parentWnd(parent) {
	  	// Do nothing.
	}

	int DoModal(void) {
		return ((int)SSCE_EditLexDlg(parentWnd->GetSafeHwnd()));
	}
protected:
	CWnd *parentWnd;
};

#endif // SSCEMFC_HPP_

