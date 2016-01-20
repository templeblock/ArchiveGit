// Sentry Spelling Checker Engine
// CSpellingDlg: Custom spelling-checker dialog box implemented in C++ and MFC

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: SpellingDlg.cpp,v 5.14.8 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "SpellingDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CSpellingDlg, CDialog)
	ON_BN_CLICKED(IDC_IGNORE, OnIgnore)
	ON_BN_CLICKED(IDC_IGNOREALL, OnIgnoreAll)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_BN_CLICKED(IDC_CHANGEALL, OnChangeAll)
	ON_BN_CLICKED(IDC_SUGGEST, OnSuggest)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_LBN_SELCHANGE(IDC_SUGGESTIONSLIST, OnSuggestionsListSelChange)
	ON_LBN_DBLCLK(IDC_SUGGESTIONSLIST, OnSuggestionsListDblClk)
END_MESSAGE_MAP()

CSpellingDlg::CSpellingDlg(CWnd *parentWnd, CEdit *edit) :
  CDialog(IDD_SPELLING, parentWnd),
  editCtrl(edit),
  text(0),
  textSz(0),
  cursor(0) {
	// Do nothing.
}

CSpellingDlg::~CSpellingDlg() {
	if (text != 0) {
		delete [] text;
	}
}

BOOL CSpellingDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	// Get the contents of the edit control into a buffer. Allocate
	// extra space to allow room for growth.
	CString contents;
	editCtrl->GetWindowText(contents);
	int textLen = contents.GetLength();
	int growth = textLen / 5;
	growth = max(growth, 256);
	textSz = textLen + growth;
	text = new SSCE_CHAR[textSz];
	lstrcpy((TCHAR *)text, contents);

	cursor = 0;
	runChecker();

	return (TRUE);
}

void CSpellingDlg::OnIgnore() {
	// Skip to the next word.
	SSCE_CHAR word[SSCE_MAX_WORD_SZ];
	cursor = SSCE_GetStringWord(SSCE_GetSid(), text, cursor, word, sizeof(word));
	cursor += lstrlen((TCHAR *)word);

	// Keep checking.
	runChecker();
}

void CSpellingDlg::OnIgnoreAll() {
    // Add the current word to the temporary dictionary
    SSCE_S16 lexId = SSCE_GetLexId(0);
    if (lexId >= 0) {
		CString problemWord;
		GetDlgItemText(IDC_PROBLEMWORD, problemWord);
        SSCE_AddToLex(SSCE_GetSid(), lexId, (SSCE_CHAR *)(const TCHAR *)problemWord,
          SSCE_IGNORE_ACTION, 0);
    }
    
    // Keep checking. The current word will automatically be skipped.
	runChecker();
}

void CSpellingDlg::OnChange() {
	CString changeToWord;
	GetDlgItemText(IDC_CHANGETO, changeToWord);
    if (changeToWord.GetLength() > 0) {
        // Replace the current word with the word in the Change To field.
        if (SSCE_ReplaceStringWord(SSCE_GetSid(), text, textSz, cursor,
		  (SSCE_CHAR *)(const TCHAR *)changeToWord) >= 0) {
            // Change the word in the edit control.
			CString problemWord;
			GetDlgItemText(IDC_PROBLEMWORD, problemWord);
			editCtrl->SetSel(cursor, cursor + problemWord.GetLength());
			editCtrl->ReplaceSel(changeToWord);
        }
    }
	else {
        // The change-to field is empty, so this is a deletion.
		// Delete the current word and any leading whitespace.
		CString delText;
        cursor = SSCE_DelStringWord(SSCE_GetSid(), text, cursor,
		  (SSCE_CHAR *)(TCHAR *)delText.GetBuffer(SSCE_MAX_WORD_SZ * 2), SSCE_MAX_WORD_SZ * 2);
		delText.ReleaseBuffer();
        if (cursor >= 0) {
            // Delete the word and leading whitespace in the edit control.
			editCtrl->SetSel(cursor, cursor + delText.GetLength());
			editCtrl->ReplaceSel(_T(""));
        }
    }

	// Keep checking. We don't skip to the next word so the replacement
	// will be spell-checked.
    runChecker();
}

void CSpellingDlg::OnChangeAll() {
	// Add the problem word and replacement word to the temporary dictionary
	// with SSCE_AUTO_CHANGE_PRESERVE_CASE_ACTION. The word will be
	// automatically corrected.
	CString changeToWord;
	GetDlgItemText(IDC_CHANGETO, changeToWord);
    if (changeToWord.GetLength() > 0) {
        // Add the word and replacement to the temporary dictionary
        SSCE_S16 lexId = SSCE_GetLexId(0);
        if (lexId >= 0) {
			CString problemWord;
			GetDlgItemText(IDC_PROBLEMWORD, problemWord);

            SSCE_AddToLex(SSCE_GetSid(), lexId,
			  (SSCE_CHAR *)(const TCHAR *)problemWord,
              SSCE_AUTO_CHANGE_PRESERVE_CASE_ACTION,
			  (SSCE_CHAR *)(const TCHAR *)changeToWord);
        }
    }

	// Keep checking. No need to skip this word because it will be replaced
	// automatically.
	runChecker();
}

void CSpellingDlg::OnSuggest() {
    // Locate better suggestions by increasing the suggestion depth.
    suggestionDepth += 10;
    fillSuggestionsList();
}

void CSpellingDlg::OnAdd() {
    // Add the current word to the selected user dictionary.
    // A different (slightly more complicated method) would involve
    // displaying a dropdown-list of user dictionary files (call
    // SSCE_GetUserLexFiles) and letting the user pick the user
    // dictionary file to add the word to.
	CString userDictFile;
    SSCE_GetSelUserLexFile(userDictFile.GetBuffer(MAX_PATH), MAX_PATH);
	userDictFile.ReleaseBuffer();
    if (userDictFile.GetLength() > 0) {
        SSCE_S16 lexId = SSCE_GetLexId(userDictFile);
        if (lexId >= 0) {
			CString problemWord;
			GetDlgItemText(IDC_PROBLEMWORD, problemWord);
            SSCE_AddToLex(SSCE_GetSid(), lexId,
			  (SSCE_CHAR *)(const TCHAR *)problemWord, SSCE_IGNORE_ACTION, 0);
        }
    }
    
    // Keep checking. No need to skip this word because it will be automatically
	// ignored.
    runChecker();
}

void CSpellingDlg::OnSuggestionsListSelChange() {
	// Copy the new selection to the Change To field.
	CListBox *suggestionsList = (CListBox *)GetDlgItem(IDC_SUGGESTIONSLIST);
	int i = suggestionsList->GetCurSel();
	if (i >= 0) {
		CString selText;
		suggestionsList->GetText(i, selText);
		SetDlgItemText(IDC_CHANGETO, selText);
	}
}

void CSpellingDlg::OnSuggestionsListDblClk() {
	// Automatically change the current word with the new selection.
	OnSuggestionsListSelChange();
	OnChange();
}

// fillSuggestionsList:
// Fill the suggestions list with suggestions for the current word.
void CSpellingDlg::fillSuggestionsList() {
	CWaitCursor busy;

	SSCE_CHAR problemWord[SSCE_MAX_WORD_SZ];
	GetDlgItemText(IDC_PROBLEMWORD, (TCHAR *)problemWord, sizeof(problemWord));

	const int maxSuggestions = 16;
	SSCE_CHAR suggestions[maxSuggestions * SSCE_MAX_WORD_SZ];
	SSCE_S16 scores[maxSuggestions];
    SSCE_Suggest(SSCE_GetSid(), problemWord, suggestionDepth,
      suggestions, sizeof(suggestions), scores, maxSuggestions);

	CListBox *suggestionsList = (CListBox *)GetDlgItem(IDC_SUGGESTIONSLIST);
	suggestionsList->ResetContent();
    for (const SSCE_CHAR *p = suggestions; *p != _T('\0');
	  p += lstrlen((TCHAR *)p) + 1) {
		suggestionsList->AddString((TCHAR *)p);
	}

    // Select the first suggestion and copy it to the Change To field.
    if (suggestionsList->GetCount() > 0) {
		suggestionsList->SetSel(0);
		CString word1;
		suggestionsList->GetText(0, word1);
		SetDlgItemText(IDC_CHANGETO, word1);
    }
}

// runChecker:
// Run the spelling checker and display the next misspelled word.
void CSpellingDlg::runChecker() {
	SSCE_CHAR word[SSCE_MAX_WORD_SZ];
	SSCE_CHAR otherWord[SSCE_MAX_WORD_SZ];

	CWaitCursor busy;
    
    // Process auto-corrections
	SSCE_S16 result;
    do {
        result = SSCE_CheckString(SSCE_GetSid(), text, &cursor, word, sizeof(word),
		  otherWord, sizeof(otherWord));
        if (result == SSCE_AUTO_CHANGE_WORD_RSLT) {
            // Replace the word in the text block
            if (SSCE_ReplaceStringWord(SSCE_GetSid(), text, textSz, cursor,
			  otherWord) >= 0) {
                // Replace the word in the edit control.
				editCtrl->SetSel(cursor, cursor + lstrlen((TCHAR *)word));
				editCtrl->ReplaceSel((TCHAR *)otherWord);
            }

            // Skip over the replaced word to avoid problems caused by
            // recursive replacements
			cursor += lstrlen((TCHAR *)otherWord);
        }
    } while (result == SSCE_AUTO_CHANGE_WORD_RSLT);
    
    if (result == SSCE_END_OF_BLOCK_RSLT) {
        // End of text reached.
        EndDialog(IDOK);
	}
    else {
		CString problemDesc;

        // A problem was detected.
        if (result == (SSCE_MISSPELLED_WORD_RSLT | SSCE_UNCAPPED_WORD_RSLT)) {
            problemDesc = _T("Capitalization:");
		}
        else if (result & SSCE_MISSPELLED_WORD_RSLT) {
            problemDesc = _T("Not in dictionary:");
		}
        else if (result == SSCE_CONDITIONALLY_CHANGE_WORD_RSLT) {
            problemDesc = _T("Consider changing:");
		}
        else if (result == SSCE_DOUBLED_WORD_RSLT) {
            problemDesc = _T("Doubled word:");
        }
		SetDlgItemText(IDC_PROBLEMDESC, problemDesc);
		SetDlgItemText(IDC_PROBLEMWORD, (TCHAR *)word);

		CListBox *suggestionsList = (CListBox *)GetDlgItem(IDC_SUGGESTIONSLIST);
        if (result & SSCE_MISSPELLED_WORD_RSLT) {
            // Fill the suggestions list with suggestions.
            suggestionDepth = SSCE_GetMinSuggestDepth();
            fillSuggestionsList();
		}
        else if (result == SSCE_CONDITIONALLY_CHANGE_WORD_RSLT) {
            // Add the replacement word to the suggestions list.
			suggestionsList->ResetContent();
            suggestionsList->AddString((TCHAR *)otherWord);
            suggestionsList->SetCurSel(0);
		}
        else if (result == SSCE_DOUBLED_WORD_RSLT) {
            // Clear the suggestions list.
            suggestionsList->ResetContent();
			SetDlgItemText(IDC_CHANGETO, _T(""));
        }
        
        // Highlight the problem word in the edit control.
		editCtrl->SetSel(cursor, cursor + lstrlen((TCHAR *)word));
    }
}

