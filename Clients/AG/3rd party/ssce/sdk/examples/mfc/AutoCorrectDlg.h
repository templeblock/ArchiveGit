// Sentry Spelling Checker Engine
// AutoCorrectDlg: Example showing automatic correction of misspellings.

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: AutoCorrectDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(AUTOCORRECTDLG_H_)
#define AUTOCORRECTDLG_H_

class CAutoCorrectDlg: public CDialog {
public:
	CAutoCorrectDlg(CWnd *parentWnd = NULL);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnStart();

	DECLARE_MESSAGE_MAP()
};

#endif // AUTOCORRECTDLG_H_
