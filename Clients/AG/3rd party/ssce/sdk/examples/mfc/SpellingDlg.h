// Sentry Spelling Checker Engine
// CSpellingDlg: Custom spelling-checker dialog box implemented in C++ and MFC

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: SpellingDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(SPELLINGDLG_H_)
#define SPELLINGDLG_H_

class CSpellingDlg: public CDialog {
public:
	CSpellingDlg(CWnd *parentWnd, CEdit *edit);
	virtual ~CSpellingDlg();
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnIgnore();
	afx_msg void OnIgnoreAll();
	afx_msg void OnChange();
	afx_msg void OnChangeAll();
	afx_msg void OnSuggest();
	afx_msg void OnAdd();
	afx_msg void OnSuggestionsListSelChange();
	afx_msg void OnSuggestionsListDblClk();

	DECLARE_MESSAGE_MAP()

private:
	CEdit *editCtrl;
	SSCE_CHAR *text;
	SSCE_S32 textSz;
	SSCE_S32 cursor;
	SSCE_S16 suggestionDepth;

	void runChecker();
	void fillSuggestionsList();
};

#endif // SPELLINGDLG_H_
