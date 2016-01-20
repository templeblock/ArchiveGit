/*
// $Workfile: NOFILLIN.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/NOFILLIN.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:36:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:42   FRED
// Initial revision.
// 
//    Rev 1.0   23 Aug 1995 07:41:30   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "nofillin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNoFillInFieldsDialog dialog


CNoFillInFieldsDialog::CNoFillInFieldsDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CNoFillInFieldsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNoFillInFieldsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CNoFillInFieldsDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNoFillInFieldsDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNoFillInFieldsDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CNoFillInFieldsDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNoFillInFieldsDialog message handlers
