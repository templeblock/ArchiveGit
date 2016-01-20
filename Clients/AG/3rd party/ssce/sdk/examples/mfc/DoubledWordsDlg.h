// Sentry Spelling Checker Engine
// DoubledWordsDlg: Example showing detection and correction of doubled words.

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: DoubledWordsDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(DOUBLEDWORDSDLG_H_)
#define DOUBLEDWORDSDLG_H_

class CDoubledWordsDlg: public CDialog {
public:
	CDoubledWordsDlg(CWnd *parentWnd = NULL);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnStart();

	DECLARE_MESSAGE_MAP()
};

#endif // DOUBLEDWORDSDLG_H_
