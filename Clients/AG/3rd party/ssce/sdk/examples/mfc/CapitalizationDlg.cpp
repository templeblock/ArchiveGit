// Sentry Spelling Checker Engine
// CCapitalizationDlg: Example showing detection of capitalization errors.

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: CapitalizationDlg.cpp,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "CapitalizationDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CCapitalizationDlg, CDialog)
	ON_BN_CLICKED(IDC_START, OnStart)
END_MESSAGE_MAP()

CCapitalizationDlg::CCapitalizationDlg(CWnd *parentWnd) :
  CDialog(IDD_CAPITALIZATION, parentWnd) {
	// Do nothing.
}

BOOL CCapitalizationDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CString sampleText(_T("dna dacron dakota germany american arizona mtv "
	  "madagascar montana Tnt tallahassee Sql scandinavia canada gmt"));

	CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_EDIT1);
	editCtrl->SetWindowText(sampleText);
	editCtrl->SetSel(0, 0);

	return (TRUE);
}

void CCapitalizationDlg::OnStart() {
	CSentrySpellDlg sentry(this);

	// Check the contents of the edit control.
	CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_EDIT1);
	SSCE_S16 rv = sentry.Check(editCtrl);
	if (rv < 0) {
		CString msg;
		msg.Format(_T("Check returned %hd"), rv);
	}
}
