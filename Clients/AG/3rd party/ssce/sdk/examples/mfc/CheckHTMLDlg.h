// Sentry Spelling Checker Engine
// CCheckHTMLDlg: Example showing how to spell-check text
//	in HTML

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: CheckHTMLDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(CHECKHTMLDLG_H_)
#define CHECKHTMLDLG_H_

class CCheckHTMLDlg: public CDialog {
public:
	CCheckHTMLDlg(CWnd *parentWnd = NULL);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnCheckSpelling();

	DECLARE_MESSAGE_MAP()
};

#endif // CHECKHTMLDLG_H_
