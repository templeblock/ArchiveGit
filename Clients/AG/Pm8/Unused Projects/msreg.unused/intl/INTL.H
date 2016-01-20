/*
// $Workfile: INTL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/intl/INTL.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/07/98 4:20p Jayn
// 
//    Rev 1.0   03 Oct 1996 10:31:00   FRED
// Initial revision.
*/

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef __INTL_H__
#define __INTL_H__

#include "intldll.h"
#include "intlres.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIntlApp
// See intl.cpp for the implementation of this class
//

class CIntlApp : public CWinApp
{
public:
	CIntlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIntlApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CIntlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

#endif
