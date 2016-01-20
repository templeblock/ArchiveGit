// Sentry Spelling Checker Engine
// CCheckTextInCodeDlg: Example showing how to spell-check text
//	without interacting with the user.

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: CheckTextInCodeDlg.cpp,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "CheckTextInCodeDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CCheckTextInCodeDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECKSPELLING, OnCheckSpelling)
END_MESSAGE_MAP()

CCheckTextInCodeDlg::CCheckTextInCodeDlg(CWnd *parentWnd) :
  CDialog(IDD_CHECKTEXTINCODE, parentWnd) {
	// Do nothing.
}

BOOL CCheckTextInCodeDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CString sampleText(_T("Alice was begining to get very tired of sittign by her sister on the "
	  "bank, and of having nothing to do: once or twice she had peeped into the "
	  "book her sister was reading, but it had no pitures or conversatoins in "
	  "it, \"and what is the use of a book,\" thought Alice \"without pictures or "
	  "conversation?\"\r\n\r\n"
	  "So she was considerring in her own mind (as well as she could, for the "
	  "hot day made her feel very sleepy and stuppid), whether the plesure of "
	  "making a daisy-chain would be worth the trouble of getting up and "
	  "picking the daisys, when sudenly a White Rabbit with pink eyes ran "
	  "close by her."));

	CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_EDIT1);
	editCtrl->SetWindowText(sampleText);
	editCtrl->SetSel(0, 0);

	return (TRUE);
}

void CCheckTextInCodeDlg::OnCheckSpelling() {
	// Load the contents of the edit control into a CString.
	CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_EDIT1);
	CString text;
	editCtrl->GetWindowText(text);
	SSCE_CHAR *str = (SSCE_CHAR *)text.GetBuffer(text.GetLength() + 1);
	SSCE_S32 cursor = 0;

	// Check the spelling of the text. CheckString will return when it
	// finds a misspelled word or reaches the end of the text.
	SSCE_S16 rv;
	SSCE_CHAR word[SSCE_MAX_WORD_SZ];
	SSCE_CHAR otherWord[SSCE_MAX_WORD_SZ];
	while ((rv = SSCE_CheckString(SSCE_GetSid(), str, &cursor, word, sizeof(word),
	  otherWord, sizeof(otherWord))) >= 0 && rv != SSCE_END_OF_BLOCK_RSLT) {
		if (rv & SSCE_MISSPELLED_WORD_RSLT) {

			// This word is misspelled. Get suggested replacements for it.
			SSCE_CHAR suggestions[1024];
			SSCE_S16 scores[16];
			SSCE_Suggest(SSCE_GetSid(), word, 50, suggestions, sizeof(suggestions),
			  scores, sizeof(scores) / sizeof(scores[0]));
			CString suggStr;

			// Each suggestion in the buffer is null terminated, and the end
			// of the list is marked with a double null.
			for (SSCE_CHAR *p = suggestions; *p != '\0';
			  p += lstrlen((TCHAR *)p) + 1) {
				if (suggStr.GetLength() > 0) {
					suggStr += _T(", ");
				}
				suggStr += (TCHAR *)p;
			}

			// Display the misspelled word and suggestions at the end of the report.
			CString report((TCHAR *)word);
			report += _T(": ");
			report += suggStr;
			report += _T("\r\n\r\n");
			CEdit *results = (CEdit *)GetDlgItem(IDC_EDIT2);
			results->SetSel(-1, -1);
			results->ReplaceSel(report);
		}

		// Skip to the next word.
		cursor += lstrlen((const char *)word);
	}

	text.ReleaseBuffer();
}
