// Sentry Spelling Checker Engine
// CMfcExampleApp: Example Sentry application written using MFC

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: MFCExampleApp.cpp,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "MFCExampleApp.h"
#include "MainDlg.h"

CMfcExampleApp mfcExampleApp;

BOOL CMfcExampleApp::InitInstance(void) {
	AfxInitRichEdit();	// needed for rich-edit controls

	m_pMainWnd = new CMainDlg;
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
	return (TRUE);
}
