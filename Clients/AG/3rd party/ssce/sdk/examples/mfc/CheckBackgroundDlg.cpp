// Sentry Spelling Checker Engine
// CCheckTextControlDlg: Example showing how to spell-check text controls
//	and rich-text controls

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: CheckBackgroundDlg.cpp,v 5.14.1 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "CheckBackgroundDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CCheckBackgroundDlg, CDialog)
	ON_EN_SETFOCUS(IDC_RICHEDIT1, OnSetFocusRichEdit1)
	ON_EN_SETFOCUS(IDC_RICHEDIT2, OnSetFocusRichEdit2)
END_MESSAGE_MAP()

CCheckBackgroundDlg::CCheckBackgroundDlg(CWnd *parentWnd) :
  CDialog(IDD_CHECKBACKGROUND, parentWnd) {
	// Do nothing.
}

BOOL CCheckBackgroundDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CRichEditCtrl *richEditCtrl = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT1);
	richEditCtrl->SetFocus();

	return (TRUE);
}

void CCheckBackgroundDlg::OnSetFocusRichEdit1() {
	// Check RichEdit1.
	CRichEditCtrl *richEditCtrl = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT1);
	CSentrySpellDlg sentry(this);
	sentry.CheckBackground(richEditCtrl);
}

void CCheckBackgroundDlg::OnSetFocusRichEdit2() {
	// Check RichEdit2.
	CRichEditCtrl *richEditCtrl = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2);
	CSentrySpellDlg sentry(this);
	sentry.CheckBackground(richEditCtrl);
}
