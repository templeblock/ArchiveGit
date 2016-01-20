// Sentry Spelling Checker Engine
// CCheckHTMLDlg: Example showing how to spell-check text in HTML

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: CheckHTMLDlg.cpp,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "CheckHTMLDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CCheckHTMLDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECKSPELLING, OnCheckSpelling)
END_MESSAGE_MAP()

CCheckHTMLDlg::CCheckHTMLDlg(CWnd *parentWnd) :
  CDialog(IDD_CHECKHTML, parentWnd) {
	// Do nothing.
}

BOOL CCheckHTMLDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CString sampleText(_T("<HTML>\r\n"
	 "<HEAD>\r\n"
	 " <TITLE>A passage from Alice in Wonderland</TITLE>\r\n"
	 "</HEAD>\r\n"
	 "<BODY BGCOLOR=\"WHITE\">\r\n"
	 " <H1 ALIGN=CENTER>\r\n"
	 "  A Passage from <I>Alice in Wonderlad</I></H1>\r\n"
	 " <H2 ALIGN=CENTER>\r\n"
	 "  by Lewis Caroll</H2>\r\n"
	 " <P>\r\n"
	 "  &quot;Please would you tell me,&quot; said <A HREF=\"http://www.alice.com\">Alice</A>,\r\n"
	 "  a little <B>timmidly</B>, for she was not quite sure <I>wheter</I> \r\n"
	 "  it was good manners for her to <U>speek</U> first, &quot;why your cat \r\n"
	 "  grins like that?&quot; </P>\r\n"
	 " <P>\r\n"
	 "  &quot;It's a <A HREF=\"#Cat\">Cheshire cat</A>,&quot; said the Duchess, \r\n"
	 "  &quot;and that's why. Pig!&quot; </P>\r\n"
	 " <P>\r\n"
	 "  She said the last word with such sudden violence that Alice quite <FONT COLOR=\"RED\">jumped</FONT>;\r\n"
	 "  but she saw in another moment that it was addressed to the baby, and \r\n"
	 "  not to her, so she took courage, and went on again:-- </P>\r\n"
	 " <P>\r\n"
	 "  <A NAME=\"Cat\"></A>&quot;I didn't know that Cheshire cats always \r\n"
	 "  grinned; in fact, I didn't know that cats could grin.&quot; </P>\r\n"
	 " <P>\r\n"
	 "  &quot;They all can,&quot; said the Duchess; &quot;and most of 'em \r\n"
	 "  do.&quot; </P>\r\n"
	 " <P>\r\n"
	 "  &quot;I don't know of any that do,&quot; Alice said very <FONT FACE=\"Arial,Helvetica,Monaco\">politly</FONT>,\r\n"
	 "  feeling quite pleased to have got into a conversation. \r\n"
	 "</BODY>\r\n"
	"</HTML>\r\n"));

	CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_EDIT1);
	editCtrl->SetWindowText(sampleText);
	editCtrl->SetSel(0, 0);

	return (TRUE);
}

void CCheckHTMLDlg::OnCheckSpelling() {
	// Make sure we ignore HTML markups.
	SSCE_U32 save = SSCE_SetOption(SSCE_GetSid(), SSCE_IGNORE_HTML_MARKUPS_OPT, 1);

	// Check the contents of the edit control.
	CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_EDIT1);
	CSentrySpellDlg sentry(this);
	SSCE_S16 rv = sentry.Check(editCtrl);
	if (rv < 0) {
		CString msg;
		msg.Format(_T("Check returned %hd"), rv);
	}

	// Restore the original option setting.
	SSCE_SetOption(SSCE_GetSid(), SSCE_IGNORE_HTML_MARKUPS_OPT, save);
}
