/*
// $Workfile: LATER.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/LATER.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.1   05 Dec 1995 13:02:36   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:52:04   FRED
// Initial revision.
// 
//    Rev 1.4   16 Nov 1995 10:31:18   FRED
//  
// 
//    Rev 1.3   31 Oct 1995 15:09:12   FRED
// Uses CWnd::CenterWindow()
// 
//    Rev 1.2   30 Oct 1995 12:11:48   FRED
//  
// 
//    Rev 1.1   26 Oct 1995 10:27:54   FRED
//  
*/ 

// later.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
#include "later.h"
#include "util.h"
#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegisterLaterDialog dialog


CRegisterLaterDialog::CRegisterLaterDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CRegisterLaterDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegisterLaterDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRegisterLaterDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterLaterDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegisterLaterDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CRegisterLaterDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRegisterLaterDialog message handlers

BOOL CRegisterLaterDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
