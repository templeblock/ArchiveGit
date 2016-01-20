// Sentry Spelling Checker Engine
// CCheckTextInCodeDlg: Example showing how to spell-check text
//	without interacting with the user.

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: CheckTextInCodeDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(CHECKTEXTINCODEDLG_H_)
#define CHECKTEXTINCODEDLG_H_

class CCheckTextInCodeDlg: public CDialog {
public:
	CCheckTextInCodeDlg(CWnd *parentWnd = NULL);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnCheckSpelling();

	DECLARE_MESSAGE_MAP()
};

#endif // CHECKTEXTINCODEDLG_H_
