// Sentry Spelling Checker Engine
// CSuggestionsDlg: Example showing how to look up suggestions.

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: SuggestionsDlg.h,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#if !defined(SUGGESTIONSDLG_H_)
#define SUGGESTIONSDLG_H_

class CSuggestionsDlg: public CDialog {
public:
	CSuggestionsDlg(CWnd *parentWnd = NULL);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnSuggest();

	DECLARE_MESSAGE_MAP()
};

#endif // SUGGESTIONSDLG_H_
