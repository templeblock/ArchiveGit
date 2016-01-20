// Sentry Spelling Checker Engine
// CCustomDialogBoxDlg: Example showing how to spell-check using a
//	custom dialog box written in C++ with MFC.

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: CustomDialogBoxDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(CUSTOMDLGBOXDLG_H_)
#define CUSTOMDIALOGBOXDLG_H_

class CCustomDialogBoxDlg: public CDialog {
public:
	CCustomDialogBoxDlg(CWnd *parentWnd = NULL);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnCheckSpelling();

	DECLARE_MESSAGE_MAP()
};

#endif // CUSTOMDIALOGBOXDLG_H_
