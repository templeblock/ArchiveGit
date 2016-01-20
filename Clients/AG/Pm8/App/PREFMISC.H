/*
// $Workfile: PREFMISC.H $
// $Revision: 1 $
// $Date: 3/03/99 6:10p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PREFMISC.H $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 5     11/20/98 1:33p Mwilson
// added capability to turn off auto instructions in preferences
// 
// 4     5/28/98 12:55p Hforman
// change resource ID
// 
// 3     5/28/98 12:23p Hforman
// remove Welcome stuff; add voice choice for Helpful Hints
// 
// 2     12/16/97 5:37p Hforman
// change CPreferencesMiscPage to CPreferencesHelpPage, add Helpful Hints
// stuff
// 
//    Rev 1.0   14 Aug 1997 15:24:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:24   Fred
// Initial revision.
// 
//    Rev 1.3   22 May 1997 12:51:10   johno
// New controls, functionality for beta
// 
//    Rev 1.2   21 May 1997 18:16:44   johno
// Changes for welcome dialogs
// 
// 
//    Rev 1.1   21 Apr 1997 19:16:00   johno
// cleanup
// 
//    Rev 1.0   21 Apr 1997 16:51:48   johno
// Initial revision.
// 
*/

class CPreferencesHelpPage;

#ifndef __PREFMISC_H__
#define __PREFMISC_H__

#include "pmwprop.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesHelpPage dialog

class CPreferencesHelpPage : public CPmwPropertyPage
{
// Construction
public:
   CPreferencesHelpPage(); // standard constructor
   virtual ~CPreferencesHelpPage();

// Dialog Data
   //{{AFX_DATA(CPreferencesHelpPage)
	enum { IDD = IDD_PREFS_HELP };
	BOOL	m_fShowDesignTips;
	BOOL	m_fShowHelpfulHints;
	BOOL	m_fPlayVoice;
	BOOL	m_fAutoInstructions;
	//}}AFX_DATA
   
// Implementation
protected:
   virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support
   // Generated message map functions
   //{{AFX_MSG(CPreferencesHelpPage)
   virtual BOOL OnInitDialog();
	afx_msg void OnHintsReset();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
   
protected:
};

/////////////////////////////////////////////////////////////////////////////
#endif
