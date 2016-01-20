// Sentry Spelling Checker Engine
// CDoubledWordsDlg: Example showing detection and correction of doubled words.

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: DoublledWordsDlg.cpp,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "DoubledWordsDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CDoubledWordsDlg, CDialog)
	ON_BN_CLICKED(IDC_START, OnStart)
END_MESSAGE_MAP()

CDoubledWordsDlg::CDoubledWordsDlg(CWnd *parentWnd) :
  CDialog(IDD_DOUBLEDWORDS, parentWnd) {
	// Do nothing.
}

BOOL CDoubledWordsDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CString sampleText(_T("Presently she began again. \"I wonder if I shall "
	  "fall right through the the earth! How funny it'll seem to come out "
	  "among the people that walk with their heads downward downward! The "
	  "Antipathies, I think--\" (she was rather glad there was no one one "
	  "listening, this time, as it it didn't sound at all the right word) \"--but "
	  "I shall have to ask them what the name of the country is, you know. "
	  "Please, Ma'am, is this New Zealand or Australia?\" (and she tried tried to "
	  "curtsey as as she spoke--fancy curtseying as you're falling through the air! "
	  "Do you think you could manage it?) \"And what an ignorant little girl "
	  "she'll think me for asking! No, it'll never do to ask: perhaps I shall "
	  "see it written up somewhere."));

	CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_EDIT1);
	editCtrl->SetWindowText(sampleText);
	editCtrl->SetSel(0, 0);

	return (TRUE);
}

void CDoubledWordsDlg::OnStart() {
	// Make sure we report doubled words.
	SSCE_U32 save = SSCE_SetOption(SSCE_GetSid(), SSCE_REPORT_DOUBLED_WORD_OPT, 1);

	// Check the contents of the edit control.
	CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_EDIT1);
	CSentrySpellDlg sentry(this);
	SSCE_S16 rv = sentry.Check(editCtrl);
	if (rv < 0) {
		CString msg;
		msg.Format(_T("Check returned %hd"), rv);
	}

	// Restore the original option setting.
	SSCE_SetOption(SSCE_GetSid(), SSCE_REPORT_DOUBLED_WORD_OPT, save);
}
