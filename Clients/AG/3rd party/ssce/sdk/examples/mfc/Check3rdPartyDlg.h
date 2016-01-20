// Sentry Spelling Checker Engine
// CCheck3rdPartyDlg: Example showing how to spell-check the
//	text in a 3rd-party text control that doesn't respond
//	to standard messages.

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: Check3rdPartyDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(CHECK3RDPARTYDLG_H_)
#define CHECK3RDPARTYDLG_H_

class CCheck3rdPartyDlg: public CDialog {
public:
	CCheck3rdPartyDlg(CWnd *parentWnd = NULL);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnCheckSpelling();

	DECLARE_MESSAGE_MAP()
};

#endif // CHECK3RDPARTYDLG_H_
