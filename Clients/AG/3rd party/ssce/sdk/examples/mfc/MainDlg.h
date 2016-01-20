// Sentry Spelling Checker Engine
// CMainDlg: Main dialog

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: MainDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(MAINDLG_H_)
#define MAINDLG_H_

class CMainDlg: public CDialog {
public:
	CMainDlg();
	virtual BOOL OnInitDialog();

protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
	virtual void OnCheckTextBox();
	virtual void OnCheckBackground();
	virtual void OnCheckString();
	virtual void OnCheckTextInCode();
	virtual void OnPerformanceTest();
	virtual void OnCustomDialogBox();
	virtual void OnCheck3rdParty();
	virtual void OnCheckHTML();
	virtual void OnSuggestions();
	virtual void OnDoubledWords();
	virtual void OnAutoCorrect();
	virtual void OnCapitalization();

	void getLicenseKey();

	SSCE_U32 licenseKey;

	DECLARE_MESSAGE_MAP()
};

#endif // MAINDLG_H_
