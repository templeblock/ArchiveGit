// Sentry Spelling Checker Engine
// CCheckTextControlDlg: Example showing how to spell-check text controls
//	and rich-text controls

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: CheckTextControlDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(CHECKTEXTCONTROLDLG_H_)
#define CHECKTEXTCONTROLDLG_H_

class CCheckTextControlDlg: public CDialog {
public:
	CCheckTextControlDlg(CWnd *parentWnd = NULL);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnCheckTextControl();
	afx_msg void OnCheckRichTextControl();

	DECLARE_MESSAGE_MAP()
};

#endif // CHECKTEXTCONTROLDLG_H_
