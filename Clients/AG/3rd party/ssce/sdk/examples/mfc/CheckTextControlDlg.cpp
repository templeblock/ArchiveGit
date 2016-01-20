// Sentry Spelling Checker Engine
// CCheckTextControlDlg: Example showing how to spell-check text controls
//	and rich-text controls

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: CheckTextControlDlg.cpp,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "CheckTextControlDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CCheckTextControlDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECKTEXTCONTROL, OnCheckTextControl)
	ON_BN_CLICKED(IDC_CHECKRICHTEXTCONTROL, OnCheckRichTextControl)
END_MESSAGE_MAP()

static DWORD CALLBACK streamInCb(DWORD cookie, BYTE *bfr, LONG nBytes,
  LONG *nBytesCopied);

CCheckTextControlDlg::CCheckTextControlDlg(CWnd *parentWnd) :
  CDialog(IDD_CHECKTEXTCONTROL, parentWnd) {
	// Do nothing.
}

BOOL CCheckTextControlDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CString sampleText(_T("Alice was begining to get very tired of siting by her sister on the "
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

	// Load the contents of the rich-text control from a file.
	CRichEditCtrl *richEditCtrl = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT1);
	richEditCtrl->LimitText(0xfffffff);
	CFile inFile;
	// alice.rtf is located in ssce\sdk\examples; current directory
	// should be ssce\sdk\examples\mfc\mfcexample
	if (inFile.Open(_T("..\\alice.rtf"), CFile::modeRead)) {
		EDITSTREAM es;
		es.dwCookie = (DWORD)&inFile;
		es.dwError = 0;
		es.pfnCallback = streamInCb;
		richEditCtrl->StreamIn(SF_RTF, es);
		inFile.Close();
	}
	else {
		richEditCtrl->SetWindowText(sampleText);
	}
	return (TRUE);
}

void CCheckTextControlDlg::OnCheckTextControl() {
	CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_EDIT1);
	CSentrySpellDlg sentry(this);
	SSCE_S16 rv = sentry.Check(editCtrl);
	if (rv < 0) {
		CString msg;
		msg.Format(_T("Check returned %hd"), rv);
	}
}

void CCheckTextControlDlg::OnCheckRichTextControl() {
	CRichEditCtrl *richEditCtrl = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT1);
	CSentrySpellDlg sentry(this);
	SSCE_S16 rv = sentry.Check(richEditCtrl);
	if (rv < 0) {
		CString msg;
		msg.Format(_T("Check returned %hd"), rv);
	}
}

// Callback function used to load the contents of the rich-edit control
// from a file.
static DWORD CALLBACK streamInCb(DWORD cookie, BYTE *bfr, LONG nBytes,
  LONG *nBytesCopied) {
   CFile *inFile = (CFile *)cookie;

   *nBytesCopied = inFile->Read(bfr, nBytes);

   return 0;
}
