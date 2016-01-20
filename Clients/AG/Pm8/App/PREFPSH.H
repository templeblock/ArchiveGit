/*
// $Workfile: PREFPSH.H $
// $Revision: 1 $
// $Date: 3/03/99 6:10p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PREFPSH.H $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 6     1/07/99 4:41p Cschende
// added the "date" tab property page to the property sheet
// 
// 5     10/15/98 3:47p Dennis
// New OCX Connection Manager.
// 
// 4     2/04/98 10:44a Dennis
// Added Online Preferences.  Removed dead sound code.  Preferences dialog
// code now be used by static method or by normal object construction.
// 
// 3     1/28/98 5:39p Dennis
// Added PreferencesOnlinePage
// 
// 2     12/16/97 5:38p Hforman
// m_WelcomePage now m_HelpPage; added Helpful Hints stuff
// 
//    Rev 1.0   14 Aug 1997 15:24:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:24   Fred
// Initial revision.
// 
//    Rev 1.4   21 May 1997 18:14:22   johno
// Changes for welcome dialogs
// 
//    Rev 1.3   21 Apr 1997 16:55:32   johno
// Changes for welcome dialogs
// 
// 
//    Rev 1.2   28 Mar 1997 17:11:30   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.1   25 Jul 1996 11:45:38   Fred
// Address argument for starting page
// 
//    Rev 1.0   14 Mar 1996 13:30:12   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:54   FRED
// Initial revision.
// 
//    Rev 1.2   02 Sep 1995 15:04:56   FRED
// More changes to make help work
// 
//    Rev 1.1   16 Aug 1995 16:22:24   JAY
// New stuff.
// 
//    Rev 1.0   16 Jul 1995 15:08:18   FRED
// Initial revision.
*/

class CPreferencesSheet;
class CConnectionSettings;

#ifndef __PREFPSH_H__
#define __PREFPSH_H__

#include "pmwprop.h"
#include "prefwrk.h"
#include "prefsnd.h"
#include "prefmisc.h"
#include "preferencesdatepage.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesSheet

class CPreferencesSheet : public CPmwPropertySheet
{
	DECLARE_DYNAMIC(CPreferencesSheet)

// Construction
public:
	CPreferencesSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

   virtual int DoModal();

   // Returns same value as DoModal()
	static int SelectPreferences(int nPage = 0);	// Call this to do the preferences dialog.

// Attributes
public:
protected:
   static CConnectionSettings * m_pConnectionSettings;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPreferencesSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPreferencesSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Property pages
protected:
	void AddPages(void);
	
public:
	CPreferencesWorkspacePage	m_WorkspacePage;
	CPreferencesSoundPage		m_SoundPage;
   CPreferencesHelpPage			m_HelpPage;
   CPreferencesDatePage			m_DatePage;	
};

/////////////////////////////////////////////////////////////////////////////

#endif
