/*
// $Workfile: CANTREG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/CANTREG.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   12 Dec 1995 17:58:00   FRED
// Initial revision.
*/ 

// cantreg.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
#include "msregini.h"
#include "propctl.h"
#include "cantreg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCantRegisterDialog dialog


CCantRegisterDialog::CCantRegisterDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CCantRegisterDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCantRegisterDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCantRegisterDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCantRegisterDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCantRegisterDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CCantRegisterDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCantRegisterDialog message handlers

BOOL CCantRegisterDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCantRegisterDialog::OnOK() 
{
	CBaseDialog::OnOK();

	// Mark the user as registered so they wont be pestered.

	GetApp()->UserIniFile().WriteInteger(GetApp()->ApplicationName(), MSREGUSRINI_Application_Registered, 1);
}
