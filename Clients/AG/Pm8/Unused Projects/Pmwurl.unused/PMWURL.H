/*
// $Workfile: PMWURL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/PMWURL/PMWURL.H $
// 
// 1     3/03/99 6:26p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:39:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:28:34   Fred
// Initial revision.
// 
//    Rev 1.0   20 Aug 1996 18:06:30   Fred
// Initial revision.
*/

class CPmwurlApp;

#ifndef __PMWURL_H__
#define __PMWURL_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPmwurlApp:
// See pmwurl.cpp for the implementation of this class
//

class CPmwurlApp : public CWinApp
{
public:
	CPmwurlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPmwurlApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPmwurlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString m_csUrlFile;
};


/////////////////////////////////////////////////////////////////////////////

#endif
