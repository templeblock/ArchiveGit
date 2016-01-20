// Sentry Spelling Checker Engine
// CMainDlg: Main dialog

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: MainDlg.cpp,v 5.14.8 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <ssce.h>
#include "AutoCorrectDlg.h"
#include "CapitalizationDlg.h"
#include "Check3rdPartyDlg.h"
#include "CheckBackgroundDlg.h"
#include "CheckHTMLDlg.h"
#include "CheckStringDlg.h"
#include "CheckTextControlDlg.h"
#include "CheckTextInCodeDlg.h"
#include "CustomDialogBoxDlg.h"
#include "DoubledWordsDlg.h"
#include "LicenseKeyDlg.h"
#include "MainDlg.h"
#include "PerformanceTestDlg.h"
#include "resource.h"
#include "SuggestionsDlg.h"

// Set to your Sentry license key, provided by Wintertree Software.
const SSCE_U32 SentryLicenseKey = 0;

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECKTEXTBOX, OnCheckTextBox)
	ON_BN_CLICKED(IDC_CHECKBACKGROUND, OnCheckBackground)
	ON_BN_CLICKED(IDC_CHECKSTRING, OnCheckString)
	ON_BN_CLICKED(IDC_CHECKTEXTINCODE, OnCheckTextInCode)
	ON_BN_CLICKED(IDC_PERFORMANCETEST, OnPerformanceTest)
	ON_BN_CLICKED(IDC_CUSTOMDIALOGBOX, OnCustomDialogBox)
	ON_BN_CLICKED(IDC_CHECK3RDPARTY, OnCheck3rdParty)
	ON_BN_CLICKED(IDC_CHECKHTML, OnCheckHTML)
	ON_BN_CLICKED(IDC_SUGGESTIONS, OnSuggestions)
	ON_BN_CLICKED(IDC_DOUBLEDWORDS, OnDoubledWords)
	ON_BN_CLICKED(IDC_AUTOCORRECT, OnAutoCorrect)
	ON_BN_CLICKED(IDC_CAPITALIZATION, OnCapitalization)
	ON_BN_CLICKED(IDC_CLOSE, OnCancel)
END_MESSAGE_MAP()

CMainDlg::CMainDlg() {
	Create(IDD_MAIN);
}

BOOL CMainDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CStatic *label = (CStatic *)GetDlgItem(IDC_VERSION);
	char version[32];
	SSCE_Version(version, sizeof(version));
	CString v;
	v.Format(_T("Version %s"), version);
	label->SetWindowText(v);

	licenseKey = SentryLicenseKey;

	return (TRUE);
}

void CMainDlg::OnCancel() {
	DestroyWindow();
}

void CMainDlg::PostNcDestroy() {
	delete this;
}

void CMainDlg::OnCheckTextBox() {
	getLicenseKey();
	CCheckTextControlDlg dlg(this);
	dlg.DoModal();
}

void CMainDlg::OnCheckBackground() {
	getLicenseKey();
	CCheckBackgroundDlg dlg(this);
	dlg.DoModal();
}

void CMainDlg::OnCheckString() {
	getLicenseKey();
	CCheckStringDlg dlg(this);
	dlg.DoModal();
}

void CMainDlg::OnCheckTextInCode() {
	getLicenseKey();
	CCheckTextInCodeDlg dlg(this);
	dlg.DoModal();
}

void CMainDlg::OnPerformanceTest() {
	getLicenseKey();
	CPerformanceTestDlg dlg(this);
	dlg.DoModal();
}

void CMainDlg::OnCustomDialogBox() {
	getLicenseKey();
	CCustomDialogBoxDlg dlg(this);
	dlg.DoModal();
}

void CMainDlg::OnCheck3rdParty() {
	getLicenseKey();
	CCheck3rdPartyDlg dlg(this);
	dlg.DoModal();
}

void CMainDlg::OnCheckHTML() {
	getLicenseKey();
	CCheckHTMLDlg dlg(this);
	dlg.DoModal();
}

void CMainDlg::OnSuggestions() {
	getLicenseKey();
	CSuggestionsDlg dlg(this);
	dlg.DoModal();
}

void CMainDlg::OnDoubledWords() {
	getLicenseKey();
	CDoubledWordsDlg dlg(this);
	dlg.DoModal();
}

void CMainDlg::OnAutoCorrect() {
	getLicenseKey();
	CAutoCorrectDlg dlg(this);
	dlg.DoModal();
}

void CMainDlg::OnCapitalization() {
	getLicenseKey();
	CCapitalizationDlg dlg(this);
	dlg.DoModal();
}

// Get the Sentry license key from the user, if necessary. The examples
// won't run without it.
void CMainDlg::getLicenseKey() {
	if (licenseKey == 0) {
		CLicenseKeyDlg dlg(this);
		dlg.m_licenseKey = licenseKey;
		dlg.DoModal();
		licenseKey = dlg.m_licenseKey;
	}
	if (licenseKey != 0) {
		SSCE_SetKey(licenseKey);
	}
}

