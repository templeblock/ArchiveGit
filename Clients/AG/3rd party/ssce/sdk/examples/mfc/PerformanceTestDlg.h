// Sentry Spelling Checker Engine
// CPerformanceTestDlg: Measure Sentry's spell-checking performance

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: PerformanceTestDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(PERFORMANCETESTDLG_H_)
#define PERFORMANCETESTDLG_H_

class CPerformanceTestDlg: public CDialog {
public:
	CPerformanceTestDlg(CWnd *parentWnd = NULL);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnCheckSpelling();

	DECLARE_MESSAGE_MAP()
};

#endif // PERFORMANCETESTDLG_H_
