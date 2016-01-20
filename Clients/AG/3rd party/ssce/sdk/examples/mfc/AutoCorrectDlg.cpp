// Sentry Spelling Checker Engine
// CAutocorrectDlg: Example showing automatic correction of misspelled words.

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: AutoCorrectDlg.cpp,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "AutoCorrectDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CAutoCorrectDlg, CDialog)
	ON_BN_CLICKED(IDC_START, OnStart)
END_MESSAGE_MAP()

CAutoCorrectDlg::CAutoCorrectDlg(CWnd *parentWnd) :
  CDialog(IDD_AUTOCORRECT, parentWnd) {
	// Do nothing.
}

BOOL CAutoCorrectDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CString sampleText(_T("temperture naturaly convertable posession knowlege "
	  "changeing rememberance sucessfull useing rythm oposite foriegn untill "
	  "proceedure morgage neccessarily differance busness"));

	CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_EDIT1);
	editCtrl->SetWindowText(sampleText);
	editCtrl->SetSel(0, 0);

	return (TRUE);
}

void CAutoCorrectDlg::OnStart() {
	CSentrySpellDlg sentry(this);

	// Make sure auto correction is enabled.
	sentry.SetAutoCorrect(TRUE);

	// Check the contents of the edit control.
	CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_EDIT1);
	SSCE_S16 rv = sentry.Check(editCtrl);
	if (rv < 0) {
		CString msg;
		msg.Format(_T("Check returned %hd"), rv);
	}
}
