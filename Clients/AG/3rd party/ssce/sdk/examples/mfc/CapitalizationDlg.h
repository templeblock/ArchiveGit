// Sentry Spelling Checker Engine
// CCapitalizationDlg: Example showing detection of capitalization errors.

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: CapitalizationDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(CAPITALIZATIONDLG_H_)
#define CAPITALIZATIONDLG_H_

class CCapitalizationDlg: public CDialog {
public:
	CCapitalizationDlg(CWnd *parentWnd = NULL);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnStart();

	DECLARE_MESSAGE_MAP()
};

#endif // CAPITALIZATIONDLG_H_
