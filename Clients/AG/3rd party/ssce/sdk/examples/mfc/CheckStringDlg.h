// Sentry Spelling Checker Engine
// CCheckStringDlg: Example showing how to spell-check CStrings
//	interactively

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: CheckStringDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(CHECKSTRINGDLG_H_)
#define CHECKSTRINGDLG_H_

class CCheckStringDlg: public CDialog {
public:
	CCheckStringDlg(CWnd *parentWnd = NULL);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnCheckSpelling();

	DECLARE_MESSAGE_MAP()
};

#endif // CHECKSTRINGDLG_H_
