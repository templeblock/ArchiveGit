/*
// $Workfile: Agurl.h $
// $Revision: 1 $
// $Date: 3/03/99 6:01p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/Agurl/Agurl.h $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 1     7/06/98 8:55a Mwilson
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

class CAgurlApp;

#ifndef __Agurl_H__
#define __Agurl_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAgurlApp:
// See Agurl.cpp for the implementation of this class
//

class CAgurlApp : public CWinApp
{
public:
	CAgurlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgurlApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAgurlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString m_csUrlFile;
};


/////////////////////////////////////////////////////////////////////////////

#endif
