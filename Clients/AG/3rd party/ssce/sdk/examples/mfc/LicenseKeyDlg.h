// Sentry Spelling Checker Engine
// LicenseKeyDlg: Prompt the user for the Sentry license key

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: LicenseKeyDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(LICENSEKEYDLG_H_)
#define LICENSEKEYDLG_H_

class CLicenseKeyDlg: public CDialog {
public:
	CLicenseKeyDlg(CWnd *parentWnd = NULL);

	unsigned long m_licenseKey;
protected:
	afx_msg void OnOK();;
	DECLARE_MESSAGE_MAP()
};

#endif // LICENSEKEYDLG_H_
