// Sentry Spelling Checker Engine
// CSuggestionsDlg: Example showing how to look up suggestions

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: SuggestionsDlg.cpp,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "SuggestionsDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CSuggestionsDlg, CDialog)
	ON_BN_CLICKED(IDC_SUGGEST, OnSuggest)
END_MESSAGE_MAP()

CSuggestionsDlg::CSuggestionsDlg(CWnd *parentWnd) :
  CDialog(IDD_SUGGESTIONS, parentWnd) {
	// Do nothing.
}

BOOL CSuggestionsDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CString depthStr;
	depthStr.Format(_T("%hd"), SSCE_GetMinSuggestDepth());
	SetDlgItemText(IDC_DEPTH, depthStr);

	BOOL opt = (BOOL)SSCE_GetOption(SSCE_GetSid(), SSCE_SUGGEST_TYPOGRAPHICAL_OPT);
	CButton *typoBtn = (CButton *)GetDlgItem(IDC_TYPOGRAPHICAL);
	typoBtn->SetCheck(opt ? BST_CHECKED : BST_UNCHECKED);

	opt = (BOOL)SSCE_GetOption(SSCE_GetSid(), SSCE_SUGGEST_PHONETIC_OPT);
	CButton *phoneticBtn = (CButton *)GetDlgItem(IDC_PHONETIC);
	phoneticBtn->SetCheck(opt ? BST_CHECKED : BST_UNCHECKED);

	return (TRUE);
}

void CSuggestionsDlg::OnSuggest() {
	CWaitCursor busy;

	// Set options based on the selected algorithm(s)
	CButton *typoBtn = (CButton *)GetDlgItem(IDC_TYPOGRAPHICAL);
	if (typoBtn->GetCheck() == BST_CHECKED) {
		SSCE_SetOption(SSCE_GetSid(), SSCE_SUGGEST_TYPOGRAPHICAL_OPT, 1);
	}
	else {
		SSCE_SetOption(SSCE_GetSid(), SSCE_SUGGEST_TYPOGRAPHICAL_OPT, 0);
	}

	CButton *phoneticBtn = (CButton *)GetDlgItem(IDC_PHONETIC);
	if (phoneticBtn->GetCheck() == BST_CHECKED) {
		SSCE_SetOption(SSCE_GetSid(), SSCE_SUGGEST_PHONETIC_OPT, 1);
	}
	else {
		SSCE_SetOption(SSCE_GetSid(), SSCE_SUGGEST_PHONETIC_OPT, 0);
	}

	if (typoBtn->GetCheck() == BST_UNCHECKED &&
	  phoneticBtn->GetCheck() == BST_UNCHECKED) {
		AfxMessageBox(_T("Note that at least one algorithm must be selected "
		  "or no suggestions will be produced."), MB_OK);
	}

	CString depthStr;
	GetDlgItemText(IDC_DEPTH, depthStr);
	int depth = _ttoi(depthStr);

	// Look up suggestions using the indicated settings.
	CString word;
	GetDlgItemText(IDC_WORD, word);
	const int maxSuggestions = 16;
	SSCE_CHAR suggestions[maxSuggestions * SSCE_MAX_WORD_SZ];
	SSCE_S16 scores[maxSuggestions];
    SSCE_Suggest(SSCE_GetSid(), (SSCE_CHAR *)(const TCHAR *)word, depth,
      suggestions, sizeof(suggestions), scores, maxSuggestions);

	// Display the suggestions in the list.
	CListBox *suggestionsList = (CListBox *)GetDlgItem(IDC_SUGGESTIONSLIST);
	suggestionsList->ResetContent();
    for (const SSCE_CHAR *p = suggestions; *p != _T('\0');
	  p += lstrlen((TCHAR *)p) + 1) {
		suggestionsList->AddString((TCHAR *)p);
	}
}
