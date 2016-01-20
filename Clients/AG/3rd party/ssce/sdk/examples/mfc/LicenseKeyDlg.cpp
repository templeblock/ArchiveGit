// Sentry Spelling Checker Engine
// CLicenseKeyDlg: Prompt the user for the Sentry license key

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: LicenseKeyDlg.cpp,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "LicenseKeyDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CLicenseKeyDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnOK)
END_MESSAGE_MAP()

CLicenseKeyDlg::CLicenseKeyDlg(CWnd *parentWnd) :
  CDialog(IDD_LICENSEKEY, parentWnd) {
	// Do nothing.
}

void CLicenseKeyDlg::OnOK() {

	// We need to conver the license key in hex, so DDX can't be used here.
	char str[50];
	GetDlgItemText(IDC_LICENSEKEY, str, sizeof(str));
	m_licenseKey = strtoul(str, 0, 16);
	CDialog::OnOK();
}
