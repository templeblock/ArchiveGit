/*
// $Workfile: PREFSND.H $
// $Revision: 1 $
// $Date: 3/03/99 6:10p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PREFSND.H $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 2     5/28/98 12:32p Hforman
// change resource ID
// 
//    Rev 1.0   14 Aug 1997 15:24:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:26   Fred
// Initial revision.
// 
//    Rev 1.2   22 May 1997 12:52:28   johno
// New controls, functionality for beta
// 
// 
//    Rev 1.1   21 May 1997 18:16:22   johno
// Changes for welcome dialogs
// 
// 
//    Rev 1.0   14 Mar 1996 13:30:14   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:54   FRED
// Initial revision.
// 
//    Rev 1.1   02 Sep 1995 15:06:12   FRED
// More changes to make help work
// 
//    Rev 1.0   16 Jul 1995 15:08:20   FRED
// Initial revision.
*/

class CPreferencesSoundPage;

#ifndef __PREFSND_H__
#define __PREFSND_H__

#include "pmwprop.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesSoundPage dialog

class CPreferencesSoundPage : public CPmwPropertyPage
{
// Construction
public:
	CPreferencesSoundPage();	// standard constructor
	virtual ~CPreferencesSoundPage();

// Dialog Data
	//{{AFX_DATA(CPreferencesSoundPage)
	enum { IDD = IDD_PREFS_SOUND };
   BOOL  m_SoundEnabled;
   BOOL  m_SoundEnabledNot;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPreferencesSoundPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
};

/////////////////////////////////////////////////////////////////////////////
#endif
