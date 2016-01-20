// Sentry Spelling Checker Engine
// CCheckBackgroundDlg: Example showing how to spell-check rich-edit
//	controls in the background (as the user types)

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: CheckBackgroundDlg.h,v 5.14.1 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(CHECKBACKGROUNDDLG_H_)
#define CHECKBACKGROUNDDLG_H_

class CCheckBackgroundDlg: public CDialog {
public:
	CCheckBackgroundDlg(CWnd *parentWnd = NULL);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnSetFocusRichEdit1();
	afx_msg void OnSetFocusRichEdit2();

	DECLARE_MESSAGE_MAP()
};

#endif // CHECKBACKGROUNDDLG_H_
